/*
 * Smoker Web Server - ESP32-S3
 *
 * This firmware runs on an ESP32-S3 and provides a web interface
 * for the Smoker Controller. It communicates with the main controller
 * (ESP32-P4) via serial connection.
 *
 * Features:
 * - WiFi connectivity with credentials stored in Preferences
 * - AsyncWebServer for REST API
 * - WebSocket for real-time updates
 * - Mobile-friendly web dashboard
 * - WiFi configuration portal
 * - OTA updates via ElegantOTA
 */

#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include <LittleFS.h>
#include "ElegantOTA.h"
#include "webpage.h"
#include "SessionManager.h"

// Serial Communication Settings
#define SERIAL_BAUD 115200
#define CONTROLLER_SERIAL Serial2  // Use Serial2 for communication with controller (independent of USB)
#define CONTROLLER_TX_PIN 17       // TX pin for controller communication
#define CONTROLLER_RX_PIN 18       // RX pin for controller communication
#define SERIAL_BUFFER_SIZE 256

// MAX6675 Temperature Sensor Settings (SPI)
// Using HSPI on ESP32-S3 to avoid conflicts with UART pins
#define SPI_CLK_PIN 12             // SPI Clock
#define SPI_MISO_PIN 13            // SPI MISO (data from sensors)
#define SMOKER_CS_PIN 10           // Chip Select for smoker probe
#define FOOD_CS_PIN 11             // Chip Select for food probe
#define TEMP_READ_INTERVAL 500     // Read sensors every 500ms
#define TEMP_SEND_INTERVAL 500     // Send temps to P4 every 500ms
#define WS_BROADCAST_INTERVAL 1000 // Broadcast state to web clients every 1 second
#define DEBUG_PRINT_INTERVAL 2000  // Print debug info every 2 seconds

// Web Server Settings
#define WEB_SERVER_PORT 80
#define WEBSOCKET_PATH "/ws"

// WiFi Settings
#define WIFI_CONNECT_TIMEOUT 10000  // 10 seconds
#define AP_SSID "SmokerSetup"
#define AP_PASSWORD "smoker123"
#define HOSTNAME "smoker"           // Network hostname (accessible as smoker.local)

// Create objects
AsyncWebServer server(WEB_SERVER_PORT);
AsyncWebSocket ws(WEBSOCKET_PATH);
Preferences preferences;

// Use dedicated HSPI bus for MAX6675 sensors to avoid conflicts
SPIClass hspi(HSPI);

// Serial buffer for controller communication
char serialBuffer[SERIAL_BUFFER_SIZE];
int serialBufferIndex = 0;

// Temperature sensor variables
int localSmokerTemp = 0;       // Temperature read from smoker probe
int localFoodTemp = 0;         // Temperature read from food probe
unsigned long lastTempReadTime = 0;

// Temperature calibration offsets (stored in NVram)
int smokerTempOffset = 0;      // Calibration offset for smoker probe (degrees F)
int foodTempOffset = 0;        // Calibration offset for food probe (degrees F)
unsigned long lastTempSendTime = 0;
unsigned long lastWsBroadcastTime = 0;
unsigned long lastDebugPrintTime = 0;

// Temperature History Settings
#define HISTORY_SIZE 60           // 60 data points = 2 hours at 2-minute intervals
#define HISTORY_INTERVAL 120000   // 2 minutes in milliseconds

// Temperature history arrays
int smokerTempHistory[HISTORY_SIZE];
int foodTempHistory[HISTORY_SIZE];
int historyIndex = 0;
int historyCount = 0;  // Number of valid data points
unsigned long lastHistoryUpdate = 0;

// Current smoker state (parsed from DATA messages)
struct SmokerState {
  int smokerTemp = 0;
  int foodTemp = 0;
  int setTemp = 225;
  int rpm = 0;
  int pwm = 0;
  bool fanOn = false;
  unsigned long timerMs = 0;
  float Kp = 7.0;
  float Ki = 0.0;
  float Kd = 0.2;
  unsigned long lastUpdate = 0;
} smokerState;

// WiFi credentials
String wifiSSID = "";
String wifiPassword = "";
String hostname = HOSTNAME;  // Default hostname, can be changed via web interface
bool wifiConnected = false;
bool apMode = false;

// Controller connection timeout (if no data received for 5 seconds, assume disconnected)
#define CONTROLLER_TIMEOUT 5000

// Function declarations
void setupWiFi();
void setupWebServer();
void setupWebSocket();
void setupTemperatureSensors();
void readTemperatureSensors();
void sendTemperaturesToController();
float readMAX6675(int csPin);
int readSmokerTemperature();
int readFoodTemperature();
void checkControllerSerial();
void parseDataMessage(char* data);
void broadcastState();
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
               AwsEventType type, void *arg, uint8_t *data, size_t len);
String getStateJson();
String getHistoryJson();
void sendCommandToController(const char* cmd);
void recordTemperatureHistory();
void initializeHistory();

void setup() {
  // Initialize USB serial for debugging
  Serial.begin(115200);
  delay(2000);  // Longer delay for USB CDC to enumerate

  Serial.println();
  Serial.println("*** BOOT ***");  // Very first message
  Serial.flush();  // Force output
  delay(100);

  Serial.println("========================================");
  Serial.println("    Smoker Web Server - ESP32-S3");
  Serial.println("========================================");
  Serial.println();

  // Initialize MAX6675 temperature sensors FIRST (before Serial1)
  setupTemperatureSensors();

  // Initialize serial for controller communication on dedicated pins
  Serial.print("Initializing controller serial on TX:");
  Serial.print(CONTROLLER_TX_PIN);
  Serial.print(" RX:");
  Serial.println(CONTROLLER_RX_PIN);
  CONTROLLER_SERIAL.begin(SERIAL_BAUD, SERIAL_8N1, CONTROLLER_RX_PIN, CONTROLLER_TX_PIN);
  delay(100);

  // Initialize preferences for WiFi storage
  preferences.begin("smoker", false);
  wifiSSID = preferences.getString("ssid", "");
  wifiPassword = preferences.getString("password", "");
  hostname = preferences.getString("hostname", HOSTNAME);

  // Load temperature calibration offsets from NVram
  smokerTempOffset = preferences.getInt("smokerOffset", 0);
  foodTempOffset = preferences.getInt("foodOffset", 0);
  Serial.print("Loaded calibration offsets - Smoker: ");
  Serial.print(smokerTempOffset);
  Serial.print("F, Food: ");
  Serial.print(foodTempOffset);
  Serial.println("F");

  // Initialize temperature history
  initializeHistory();

  // Initialize session manager (LittleFS for persistent session storage)
  if (!sessionManager.begin()) {
    Serial.println("Warning: Session recording disabled - LittleFS init failed");
  }

  // Setup WiFi
  setupWiFi();

  // Setup web server and WebSocket
  setupWebSocket();
  setupWebServer();

  Serial.println();
  Serial.println("========================================");
  Serial.println("    Setup Complete!");
  Serial.println("========================================");
  if (wifiConnected) {
    Serial.print("Connected to WiFi: ");
    Serial.println(wifiSSID);
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Hostname: ");
    Serial.print(hostname);
    Serial.println(".local");
  } else {
    Serial.println("WiFi AP Mode Active");
    Serial.print("SSID: ");
    Serial.println(AP_SSID);
    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());
  }
  Serial.println("Web server running on port 80");
  Serial.println("========================================");
  Serial.println();
}

void loop() {
  // Handle ElegantOTA
  ElegantOTA.loop();

  // Read temperature sensors at regular intervals
  if ((millis() - lastTempReadTime) >= TEMP_READ_INTERVAL) {
    readTemperatureSensors();
    lastTempReadTime = millis();
  }

  // Send temperatures to controller at regular intervals
  if ((millis() - lastTempSendTime) >= TEMP_SEND_INTERVAL) {
    sendTemperaturesToController();
    lastTempSendTime = millis();
  }

  // Check for data from controller
  checkControllerSerial();

  // Record temperature history every 2 minutes
  if ((millis() - lastHistoryUpdate) >= HISTORY_INTERVAL) {
    recordTemperatureHistory();
    lastHistoryUpdate = millis();
  }

  // Session manager periodic tasks (temperature sampling during recording)
  sessionManager.tick(smokerState.smokerTemp, smokerState.foodTemp);

  // Broadcast state to WebSocket clients every second for real-time updates
  if ((millis() - lastWsBroadcastTime) >= WS_BROADCAST_INTERVAL) {
    broadcastState();
    lastWsBroadcastTime = millis();
  }

  // Print debug info periodically
  if ((millis() - lastDebugPrintTime) >= DEBUG_PRINT_INTERVAL) {
    Serial.print("[");
    Serial.print(millis() / 1000);
    Serial.print("s] Smoker: ");
    Serial.print(localSmokerTemp);
    Serial.print("F | Food: ");
    Serial.print(localFoodTemp);
    Serial.print("F | Set: ");
    Serial.print(smokerState.setTemp);
    Serial.print("F | Fan: ");
    Serial.print(smokerState.fanOn ? "ON" : "OFF");
    Serial.print(" | WS clients: ");
    Serial.println(ws.count());
    lastDebugPrintTime = millis();
  }

  // Clean up WebSocket clients
  ws.cleanupClients();

  // Small delay to prevent watchdog issues
  delay(1);
}

void initializeHistory() {
  // Initialize history arrays with 0
  for (int i = 0; i < HISTORY_SIZE; i++) {
    smokerTempHistory[i] = 0;
    foodTempHistory[i] = 0;
  }
  historyIndex = 0;
  historyCount = 0;
  lastHistoryUpdate = millis();
}

void recordTemperatureHistory() {
  // Store current temperatures in circular buffer
  smokerTempHistory[historyIndex] = smokerState.smokerTemp;
  foodTempHistory[historyIndex] = smokerState.foodTemp;

  // Move to next index (circular)
  historyIndex = (historyIndex + 1) % HISTORY_SIZE;

  // Track how many valid data points we have (max HISTORY_SIZE)
  if (historyCount < HISTORY_SIZE) {
    historyCount++;
  }
}

void setupWiFi() {
  if (wifiSSID.length() > 0) {
    // Try to connect to saved WiFi
    // IMPORTANT: Hostname must be set BEFORE WiFi.mode() is called
    // because setHostname() only takes effect when the mode changes
    WiFi.disconnect(true);  // Ensure WiFi is fully reset
    delay(100);
    WiFi.setHostname(hostname.c_str());  // Set hostname before mode
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());

    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - startTime) < WIFI_CONNECT_TIMEOUT) {
      delay(500);
    }

    if (WiFi.status() == WL_CONNECTED) {
      wifiConnected = true;
      apMode = false;

      // Start mDNS service for .local domain
      if (MDNS.begin(hostname.c_str())) {
        MDNS.addService("http", "tcp", 80);
      }

      return;
    }
  }

  // Start AP mode for configuration
  WiFi.disconnect(true);  // Ensure WiFi is fully reset
  delay(100);
  WiFi.softAPsetHostname(hostname.c_str());  // Set hostname before mode
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  apMode = true;
  wifiConnected = false;
}

void setupWebSocket() {
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
}

void setupWebServer() {
  // Serve main page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });

  // API endpoint to get current state
  server.on("/api/state", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "application/json", getStateJson());
  });

  // API endpoint to send command
  server.on("/api/command", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      StaticJsonDocument<256> doc;
      DeserializationError error = deserializeJson(doc, data, len);

      if (!error && doc.containsKey("command")) {
        String cmd = doc["command"].as<String>();
        sendCommandToController(cmd.c_str());
        request->send(200, "application/json", "{\"status\":\"ok\"}");
      } else {
        request->send(400, "application/json", "{\"error\":\"Invalid request\"}");
      }
    });

  // WiFi configuration endpoint
  server.on("/wifi", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      StaticJsonDocument<256> doc;
      DeserializationError error = deserializeJson(doc, data, len);

      if (!error) {
        String ssid = doc["ssid"] | "";
        String password = doc["password"] | "";
        String newHostname = doc["hostname"] | HOSTNAME;

        if (ssid.length() > 0) {
          preferences.putString("ssid", ssid);
          preferences.putString("password", password);
          preferences.putString("hostname", newHostname);

          request->send(200, "application/json", "{\"message\":\"WiFi settings saved. Restarting...\"}");
          delay(1000);
          ESP.restart();
        } else {
          request->send(400, "application/json", "{\"error\":\"SSID required\"}");
        }
      } else {
        request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      }
    });

  // Get WiFi status
  server.on("/wifi/status", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<256> doc;
    doc["connected"] = wifiConnected;
    doc["apMode"] = apMode;
    doc["ssid"] = wifiSSID;
    doc["hostname"] = hostname;
    doc["ip"] = wifiConnected ? WiFi.localIP().toString() : WiFi.softAPIP().toString();

    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
  });

  // Temperature calibration endpoint
  server.on("/api/calibration", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      StaticJsonDocument<256> doc;
      DeserializationError error = deserializeJson(doc, data, len);

      if (!error) {
        // Get calibration values (allow negative numbers for subtraction)
        if (doc.containsKey("smokerOffset")) {
          smokerTempOffset = doc["smokerOffset"].as<int>();
          preferences.putInt("smokerOffset", smokerTempOffset);
        }
        if (doc.containsKey("foodOffset")) {
          foodTempOffset = doc["foodOffset"].as<int>();
          preferences.putInt("foodOffset", foodTempOffset);
        }

        Serial.print("Calibration updated - Smoker: ");
        Serial.print(smokerTempOffset);
        Serial.print("F, Food: ");
        Serial.print(foodTempOffset);
        Serial.println("F");

        // Return success with current values
        StaticJsonDocument<128> response;
        response["status"] = "ok";
        response["smokerOffset"] = smokerTempOffset;
        response["foodOffset"] = foodTempOffset;
        String responseStr;
        serializeJson(response, responseStr);
        request->send(200, "application/json", responseStr);
      } else {
        request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      }
    });

  // Get current calibration values
  server.on("/api/calibration", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<128> doc;
    doc["smokerOffset"] = smokerTempOffset;
    doc["foodOffset"] = foodTempOffset;

    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
  });

  // ==================== Session Recording API Endpoints ====================

  // GET /api/sessions - List all sessions
  server.on("/api/sessions", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json = sessionManager.getSessionList();
    request->send(200, "application/json", json);
  });

  // POST /api/sessions - Start new recording session
  server.on("/api/sessions", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      StaticJsonDocument<256> doc;
      DeserializationError error = deserializeJson(doc, data, len);

      if (!error) {
        const char* name = doc["name"] | "Unnamed Session";
        const char* meat = doc["meat"] | "Unknown";
        int setTemp = doc["setTemp"] | smokerState.setTemp;
        uint32_t timestamp = doc["timestamp"] | 0;

        if (sessionManager.startSession(name, meat, setTemp, timestamp)) {
          request->send(200, "application/json", "{\"status\":\"recording\"}");
        } else {
          request->send(400, "application/json", "{\"error\":\"Already recording or failed to start\"}");
        }
      } else {
        request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      }
    });

  // DELETE /api/sessions/active - Stop current recording
  server.on("/api/sessions/active", HTTP_DELETE, [](AsyncWebServerRequest *request) {
    if (sessionManager.endSession()) {
      request->send(200, "application/json", "{\"status\":\"stopped\"}");
    } else {
      request->send(400, "application/json", "{\"error\":\"No active session\"}");
    }
  });

  // POST /api/note - Add note to active session
  server.on("/api/note", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      StaticJsonDocument<256> doc;
      DeserializationError error = deserializeJson(doc, data, len);

      if (!error && doc.containsKey("note")) {
        const char* noteText = doc["note"];
        if (sessionManager.addNote(noteText)) {
          request->send(200, "application/json", "{\"status\":\"added\"}");
        } else {
          request->send(400, "application/json", "{\"error\":\"Failed to add note\"}");
        }
      } else {
        request->send(400, "application/json", "{\"error\":\"Invalid JSON or missing note\"}");
      }
    });

  // GET /api/session/details - Get session details (singular to avoid route conflict)
  server.on("/api/session/details", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("id")) {
      uint32_t id = request->getParam("id")->value().toInt();
      String json = sessionManager.getSessionDetails(id);
      request->send(200, "application/json", json);
    } else {
      request->send(400, "application/json", "{\"error\":\"Missing id parameter\"}");
    }
  });

  // DELETE /api/session/delete - Delete a session (singular to avoid route conflict)
  server.on("/api/session/delete", HTTP_DELETE, [](AsyncWebServerRequest *request) {
    if (request->hasParam("id")) {
      uint32_t id = request->getParam("id")->value().toInt();
      if (sessionManager.deleteSession(id)) {
        request->send(200, "application/json", "{\"status\":\"deleted\"}");
      } else {
        request->send(400, "application/json", "{\"error\":\"Failed to delete or session is active\"}");
      }
    } else {
      request->send(400, "application/json", "{\"error\":\"Missing id parameter\"}");
    }
  });

  // POST /api/session/load - Load session settings (singular to avoid route conflict)
  server.on("/api/session/load", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("id")) {
      uint32_t id = request->getParam("id")->value().toInt();
      int setTemp = sessionManager.getSessionSetTemp(id);

      if (setTemp > 0) {
        // Send command to controller to set temperature
        char cmd[32];
        snprintf(cmd, sizeof(cmd), "SET_TEMP:%d", setTemp);
        sendCommandToController(cmd);

        StaticJsonDocument<64> response;
        response["status"] = "loaded";
        response["setTemp"] = setTemp;
        String responseStr;
        serializeJson(response, responseStr);
        request->send(200, "application/json", responseStr);
      } else {
        request->send(404, "application/json", "{\"error\":\"Session not found\"}");
      }
    } else {
      request->send(400, "application/json", "{\"error\":\"Missing id parameter\"}");
    }
  });

  // Initialize ElegantOTA
  ElegantOTA.begin(&server);    // Start ElegantOTA
  // ElegantOTA page will be accessible at http://<IP>/update

  server.begin();
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
               AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      // Send current state to new client
      client->text(getStateJson());
      break;

    case WS_EVT_DISCONNECT:
      break;

    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;

    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;

    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, (char*)data);

    if (!error && doc.containsKey("command")) {
      String cmd = doc["command"].as<String>();
      sendCommandToController(cmd.c_str());
    }
  }
}

void checkControllerSerial() {
  while (CONTROLLER_SERIAL.available()) {
    char c = CONTROLLER_SERIAL.read();

    if (c == '\n' || c == '\r') {
      if (serialBufferIndex > 0) {
        serialBuffer[serialBufferIndex] = '\0';

        // Parse the message
        if (strncmp(serialBuffer, "DATA:", 5) == 0) {
          parseDataMessage(serialBuffer + 5);
          broadcastState();
        } else if (strncmp(serialBuffer, "CONFIG:", 7) == 0) {
          // Handle config response - could broadcast to clients
        }

        serialBufferIndex = 0;
      }
    } else if (serialBufferIndex < SERIAL_BUFFER_SIZE - 1) {
      serialBuffer[serialBufferIndex++] = c;
    }
  }
}

void parseDataMessage(char* data) {
  // Format: smokerTemp,foodTemp,setTemp,rpm,pwm,fanOn,timerMs,Kp,Ki,Kd
  // NOTE: We skip cases 0 and 1 (smokerTemp, foodTemp) because the ESP32-S3
  // reads temperatures locally via MAX6675 sensors. The display controller
  // sends -1 for these values since it doesn't have temperature sensors.
  char* token = strtok(data, ",");
  int index = 0;

  // Track previous setTemp to detect changes for session recording
  int previousSetTemp = smokerState.setTemp;

  while (token != NULL && index < 10) {
    switch (index) {
      case 0: break;  // Skip - smokerTemp read locally via MAX6675
      case 1: break;  // Skip - foodTemp read locally via MAX6675
      case 2: smokerState.setTemp = atoi(token); break;
      case 3: smokerState.rpm = atoi(token); break;
      case 4: smokerState.pwm = atoi(token); break;
      case 5: smokerState.fanOn = atoi(token) == 1; break;
      case 6: smokerState.timerMs = atol(token); break;
      case 7: smokerState.Kp = atof(token); break;
      case 8: smokerState.Ki = atof(token); break;
      case 9: smokerState.Kd = atof(token); break;
    }
    token = strtok(NULL, ",");
    index++;
  }

  smokerState.lastUpdate = millis();

  // Record temperature change for session if setTemp changed
  if (smokerState.setTemp != previousSetTemp && sessionManager.isRecording()) {
    sessionManager.recordTempChange(smokerState.setTemp);
  }
}

String getStateJson() {
  // Use larger document for history data
  DynamicJsonDocument doc(4096);

  // Check if controller is connected (received data within timeout period)
  bool controllerConnected = (smokerState.lastUpdate > 0) &&
                              ((millis() - smokerState.lastUpdate) < CONTROLLER_TIMEOUT);

  doc["controllerConnected"] = controllerConnected;
  doc["smokerTemp"] = smokerState.smokerTemp;
  doc["foodTemp"] = smokerState.foodTemp;
  doc["setTemp"] = smokerState.setTemp;
  doc["rpm"] = smokerState.rpm;
  doc["pwm"] = smokerState.pwm;
  doc["fanOn"] = smokerState.fanOn;
  doc["timerMs"] = smokerState.timerMs;
  doc["Kp"] = smokerState.Kp;
  doc["Ki"] = smokerState.Ki;
  doc["Kd"] = smokerState.Kd;
  doc["lastUpdate"] = smokerState.lastUpdate;

  // Add calibration offsets
  doc["smokerOffset"] = smokerTempOffset;
  doc["foodOffset"] = foodTempOffset;

  // Add session recording status
  doc["sessionRecording"] = sessionManager.isRecording();
  if (sessionManager.isRecording()) {
    doc["activeSessionId"] = sessionManager.getActiveSessionId();
    doc["activeSessionName"] = sessionManager.getActiveSessionName();
  }

  // Add temperature history arrays
  JsonArray smokerHistory = doc.createNestedArray("smokerHistory");
  JsonArray foodHistory = doc.createNestedArray("foodHistory");

  // Output data in chronological order (oldest to newest)
  // Start from the oldest data point and wrap around
  int startIndex = (historyCount < HISTORY_SIZE) ? 0 : historyIndex;

  for (int i = 0; i < historyCount; i++) {
    int idx = (startIndex + i) % HISTORY_SIZE;
    smokerHistory.add(smokerTempHistory[idx]);
    foodHistory.add(foodTempHistory[idx]);
  }

  doc["historyCount"] = historyCount;

  String response;
  serializeJson(doc, response);
  return response;
}

String getHistoryJson() {
  DynamicJsonDocument doc(3072);

  JsonArray smokerHistory = doc.createNestedArray("smokerHistory");
  JsonArray foodHistory = doc.createNestedArray("foodHistory");

  // Output data in chronological order (oldest to newest)
  int startIndex = (historyCount < HISTORY_SIZE) ? 0 : historyIndex;

  for (int i = 0; i < historyCount; i++) {
    int idx = (startIndex + i) % HISTORY_SIZE;
    smokerHistory.add(smokerTempHistory[idx]);
    foodHistory.add(foodTempHistory[idx]);
  }

  doc["historyCount"] = historyCount;

  String response;
  serializeJson(doc, response);
  return response;
}

void broadcastState() {
  String json = getStateJson();
  ws.textAll(json);
}

void sendCommandToController(const char* cmd) {
  // Send command to controller via Serial
  CONTROLLER_SERIAL.print("CMD:");
  CONTROLLER_SERIAL.println(cmd);
}

// ==================== MAX6675 Temperature Sensor Functions ====================
// Using hardware SPI for reliable communication

void setupTemperatureSensors() {
  // Initialize chip select pins
  pinMode(SMOKER_CS_PIN, OUTPUT);
  digitalWrite(SMOKER_CS_PIN, HIGH);
  pinMode(FOOD_CS_PIN, OUTPUT);
  digitalWrite(FOOD_CS_PIN, HIGH);

  // Initialize dedicated HSPI bus: CLK, MISO, MOSI (unused), SS (unused)
  hspi.begin(SPI_CLK_PIN, SPI_MISO_PIN, -1, -1);

  delay(250);  // MAX6675 needs time after power-up

  Serial.println("MAX6675 temperature sensors initialized (HSPI bus)");
  Serial.print("SPI CLK: GPIO"); Serial.print(SPI_CLK_PIN);
  Serial.print(", MISO: GPIO"); Serial.print(SPI_MISO_PIN);
  Serial.print(", Smoker CS: GPIO"); Serial.print(SMOKER_CS_PIN);
  Serial.print(", Food CS: GPIO"); Serial.println(FOOD_CS_PIN);
}

// Read MAX6675 using HSPI bus - returns temperature in Fahrenheit or NAN on error
float readMAX6675(int csPin) {
  digitalWrite(csPin, LOW);
  uint16_t data = hspi.transfer16(0x00);
  digitalWrite(csPin, HIGH);

  delay(100);  // Give sensor time between reads

  // Bit 2 is the open thermocouple flag
  if (data & 0x04) {
    return NAN;  // Open thermocouple
  }

  // Bits 3-14 contain temperature data (12 bits), LSB = 0.25°C
  // Convert to Fahrenheit
  return ((data >> 3) & 0x0FFF) * 0.25 * 9.0 / 5.0 + 32.0;
}

int readSmokerTemperature() {
  float tempF = readMAX6675(SMOKER_CS_PIN);

  // Check for error conditions
  if (isnan(tempF) || tempF >= 1000 || tempF < -100) {
    return -999;  // Return error code
  }

  // Apply calibration offset
  return (int)tempF + smokerTempOffset;
}

int readFoodTemperature() {
  float tempF = readMAX6675(FOOD_CS_PIN);

  // Check for error conditions
  if (isnan(tempF) || tempF >= 1000 || tempF < -100) {
    return -999;  // Return error code
  }

  // Apply calibration offset
  return (int)tempF + foodTempOffset;
}

void readTemperatureSensors() {
  // Read both sensors and store in local variables
  localSmokerTemp = readSmokerTemperature();
  localFoodTemp = readFoodTemperature();

  // Update the smokerState with locally read temperatures
  // (These will be overwritten by DATA messages from P4, but we use them as fallback)
  smokerState.smokerTemp = localSmokerTemp;
  smokerState.foodTemp = localFoodTemp;
}

void sendTemperaturesToController() {
  // Send temperature readings to ESP32-P4 controller
  // Format: TEMPS:smokerTemp,foodTemp
  // Using -999 as error indicator (same as original code)
  CONTROLLER_SERIAL.print("TEMPS:");
  CONTROLLER_SERIAL.print(localSmokerTemp);
  CONTROLLER_SERIAL.print(",");
  CONTROLLER_SERIAL.println(localFoodTemp);
}

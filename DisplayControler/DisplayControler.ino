#include <lvgl.h>
#include <Arduino.h>
#include <PID_v1.h>
#include <Preferences.h>
#include "esp_timer.h"
#include "style.h"
#include "pins_config.h"
#include "src/lcd/jd9165_lcd.h"
#include "src/touch/gt911_touch.h"

// Definitions
#define PWM_PIN 28           // this is the pin that is used to drive the FAN via PID
#define TACH_PIN 29          // This is the imput pin for the fan Tac
#define LowTemp 150          // the lowest PID controlled temperature
#define MaxTemp 500          // Maximum PID controled temperature
#define MAX_PID_OUTPUT 255   // Limit max fan speed for safety
#define StartupTemp 225      // set the start up temperature
#define TACH_SAMPLE_TIME 500 // the frequincy to check and update the fan tacometer
#define default_P 7.0        // Defaule PID value for P if nothing is in the NVram
#define default_I 0.0        // Defaule PID value for I if nothing is in the NVram
#define default_D 0.2        // Defaule PID value for D if nothing is in the NVram

// Temperature data from ESP32-S3 Web Server
#define TEMP_WATCHDOG_TIMEOUT 10000  // 10 seconds - shutdown if no temp data received

// Serial Communication Settings
#define ENABLE_USB_SERIAL_DEBUG false // Set to false to disable USB serial debug output
#define WEB_SERIAL_RX 33             // GPIO 33 for Web Server RX
#define WEB_SERIAL_TX 34             // GPIO 34 for Web Server TX
#define SERIAL_BAUD 115200
#define SERIAL_UPDATE_INTERVAL 500 // Send data every 500ms
#define SERIAL_BUFFER_SIZE 128

// Preferences namespace
#define PREFS_NAMESPACE "smoker"

// Serial communication variables
HardwareSerial WebSerial(1); // UART1 for web server communication on GPIO 33/34
unsigned long lastSerialUpdateTime = 0;
char serialBuffer[SERIAL_BUFFER_SIZE];
int serialBufferIndex = 0;

// Temperature data received from ESP32-S3 Web Server
int receivedSmokerTemp = -999;    // -999 indicates no valid data
int receivedFoodTemp = -999;      // -999 indicates no valid data
unsigned long lastTempReceiveTime = 0;  // Timestamp of last TEMPS message
bool tempSensorConnected = false;       // Connection status for display

jd9165_lcd lcd = jd9165_lcd(LCD_RST);
gt911_touch touch = gt911_touch(TP_I2C_SDA, TP_I2C_SCL, TP_RST, TP_INT);

lv_display_t *disp_drv;
static uint32_t *buf;
static uint32_t *buf1;

lv_obj_t *timeLabel = nullptr;
lv_obj_t *btnUP = nullptr;
lv_obj_t *btnDOWN = nullptr;
lv_obj_t *lblSetTemp = nullptr;
lv_obj_t *lblActualTemp = nullptr;
lv_obj_t *btnSSTmr = nullptr;
lv_obj_t *btnTimerReset = nullptr;
lv_obj_t *fanSwitch = nullptr;
lv_obj_t *driveBar = nullptr;
lv_obj_t *drivePercent = nullptr;
lv_obj_t *rpmLabel = nullptr;
lv_obj_t *rpmBar = nullptr;
lv_obj_t *rpmPercent = nullptr;
lv_obj_t *lblFoodTemp = nullptr;
lv_obj_t *tempChart = nullptr;
lv_chart_series_t *smokerSeries = nullptr;
lv_chart_series_t *foodSeries = nullptr;
lv_obj_t *currentSmokerTemp = nullptr;
lv_obj_t *currentFoodTemp = nullptr;

// Global container pointers for alternating display
lv_obj_t *ActualTempContainer = nullptr;
lv_obj_t *FoodTempContainer = nullptr;

lv_timer_t *time_Timer = NULL;
lv_timer_t *containerSwapTimer = NULL;

static lv_style_t styleUP;
static lv_style_t styleDate;
static lv_style_t styleDaysOfWeek;
static lv_style_t styleTime;
static lv_style_t styleSetTemp;
static lv_style_t styleSSTmr;
static lv_style_t styleTimerReset;

static unsigned long timerStartTime = 0; // When timer started/resumed
static unsigned long elapsedTime = 0;    // Total elapsed time in ms
static bool timerRunning = false;        // Track if timer is running

// ----------------> Global Variables
char timeHour[3] = "00";
char timeMin[3] = "00";
char timeSec[3];
String _Time = "";
int SetTemp;
int ActualTemp;                            // variable to hold Smoker temperature
double Kp = default_P;                     // PID "P" variable
double Ki = default_I;                     // PID "I" variable
double Kd = default_D;                     // PID "D" variable
double Input, Output, Setpoint = 225.0;    // Vasiable for the PID contorller
unsigned long lastDisplayUpdateTime;       // used to treck the last time the display was updated
unsigned long displayUpdateInterval = 500; // the intervale that the display is updated (configurable)
unsigned long chartUpdateInterval = 60000; // Chart updates every 60 seconds (configurable)
unsigned long lastChartUpdateTime = 0;     // Track last chart update
bool fanEnabled = false;                   // used to track if the smoker is active or not
volatile unsigned long tachPulseCount = 0; // variable for counting the fan pulses for teh tachometer
volatile int lastTachState = HIGH;         // track previous tach pin state for edge detection
esp_timer_handle_t tachTimerHandle = NULL; // hardware timer for tach polling
int globalRPM;                             // main variable for holding the realtime RPM
// Timing variables for container swap logic
unsigned long lastButtonPressTime = 0;
bool showingActualTemp = true;       // Track which container is currently visible
unsigned long buttonHoldTime = 5000; // Time after button press before swapping (configurable)
unsigned long swapInterval = 2000;   // Time between container swaps (configurable)
int fadeDuration = 100;              // Animation duration in milliseconds (configurable)

// --------------> Object instanciations
Preferences preferences;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

// --------------> Preferences Functions
void saveSettingsToEEPROM()
{
  preferences.begin(PREFS_NAMESPACE, false); // Open in read-write mode
  preferences.putDouble("Kp", Kp);
  preferences.putDouble("Ki", Ki);
  preferences.putDouble("Kd", Kd);
  preferences.putULong("dispInterval", displayUpdateInterval);
  preferences.putULong("chartInterval", chartUpdateInterval);
  preferences.putULong("buttonHold", buttonHoldTime);
  preferences.putULong("swapInterval", swapInterval);
  preferences.putInt("fadeDuration", fadeDuration);
  preferences.end();
} // end of Save Settings to EEPROM function

void loadSettingsFromEEPROM()
{
  preferences.begin(PREFS_NAMESPACE, true); // Open in read-only mode

  // Check if preferences exist by checking for a key
  if (preferences.isKey("Kp"))
  {
    Kp = preferences.getDouble("Kp", default_P);
    Ki = preferences.getDouble("Ki", default_I);
    Kd = preferences.getDouble("Kd", default_D);
    displayUpdateInterval = preferences.getULong("dispInterval", 500);
    chartUpdateInterval = preferences.getULong("chartInterval", 60000);
    buttonHoldTime = preferences.getULong("buttonHold", 5000);
    swapInterval = preferences.getULong("swapInterval", 2000);
    fadeDuration = preferences.getInt("fadeDuration", 100);

    // Update PID with loaded values
    myPID.SetTunings(Kp, Ki, Kd);
  }
  // If preferences don't exist, keep default values

  preferences.end();
} // end of load settings from EEPROM function

// --------------> Serial Communication Functions
void sendSerialData()
{
  // Format: DATA:smokerTemp,foodTemp,setTemp,rpm,pwm,fanOn,timerMs,Kp,Ki,Kd
  unsigned long currentElapsed = elapsedTime;
  if (timerRunning)
  {
    currentElapsed = elapsedTime + (millis() - timerStartTime);
  }

  // Use temperature data received from ESP32-S3 Web Server
  int smokerTemp = receivedSmokerTemp;
  int foodTemp = receivedFoodTemp;

  // Send to Web Server via hardware serial (GPIO 33/34)
  WebSerial.print("DATA:");
  // Send -1 for error conditions instead of -999 for cleaner protocol
  WebSerial.print(smokerTemp == -999 ? -1 : smokerTemp);
  WebSerial.print(",");
  WebSerial.print(foodTemp == -999 ? -1 : foodTemp);
  WebSerial.print(",");
  WebSerial.print(SetTemp);
  WebSerial.print(",");
  WebSerial.print(globalRPM);
  WebSerial.print(",");
  WebSerial.print((int)Output);
  WebSerial.print(",");
  WebSerial.print(fanEnabled ? 1 : 0);
  WebSerial.print(",");
  WebSerial.print(currentElapsed);
  WebSerial.print(",");
  WebSerial.print(Kp, 2);
  WebSerial.print(",");
  WebSerial.print(Ki, 2);
  WebSerial.print(",");
  WebSerial.println(Kd, 2);

  // Optionally duplicate to USB serial for debugging
  #if ENABLE_USB_SERIAL_DEBUG
  Serial.print("DATA:");
  Serial.print(smokerTemp == -999 ? -1 : smokerTemp);
  Serial.print(",");
  Serial.print(foodTemp == -999 ? -1 : foodTemp);
  Serial.print(",");
  Serial.print(SetTemp);
  Serial.print(",");
  Serial.print(globalRPM);
  Serial.print(",");
  Serial.print((int)Output);
  Serial.print(",");
  Serial.print(fanEnabled ? 1 : 0);
  Serial.print(",");
  Serial.print(currentElapsed);
  Serial.print(",");
  Serial.print(Kp, 2);
  Serial.print(",");
  Serial.print(Ki, 2);
  Serial.print(",");
  Serial.println(Kd, 2);
  #endif
} // end of Send Serial Data function 

// handles srial commands and data sent from the web server 
void processSerialCommand(char *cmd)
{
  // Parse incoming messages from Web Server ESP32
  // Format: CMD:COMMAND:VALUE or TEMPS:smokerTemp,foodTemp

  // Handle temperature data from ESP32-S3
  if (strncmp(cmd, "TEMPS:", 6) == 0)
  {
    char *tempData = cmd + 6;
    char *token = strtok(tempData, ",");
    if (token != NULL)
    {
      receivedSmokerTemp = atoi(token);
      token = strtok(NULL, ",");
      if (token != NULL)
      {
        receivedFoodTemp = atoi(token);
      }
    }
    lastTempReceiveTime = millis();
    tempSensorConnected = true;

    #if ENABLE_USB_SERIAL_DEBUG
    Serial.print("RX TEMPS: Smoker=");
    Serial.print(receivedSmokerTemp);
    Serial.print(", Food=");
    Serial.println(receivedFoodTemp);
    #endif
    return;
  }

  // Handle CMD messages
  if (strncmp(cmd, "CMD:", 4) != 0)
    return;

  char *command = cmd + 4;

  if (strncmp(command, "SET_TEMP:", 9) == 0)
  {
    int temp = atoi(command + 9);
    if (temp >= LowTemp && temp <= MaxTemp)
    {
      SetTemp = temp;
      Setpoint = temp;
      lv_label_set_text(lblSetTemp, ((String)SetTemp + "°F").c_str());
    }
  }
  else if (strncmp(command, "FAN:", 4) == 0)
  {
    if (strcmp(command + 4, "ON") == 0)
    {
      fanEnabled = true;
      myPID.SetMode(AUTOMATIC);
      // Update UI switch state
      if (fanSwitch)
        lv_obj_add_state(fanSwitch, LV_STATE_CHECKED);
    }
    else if (strcmp(command + 4, "OFF") == 0)
    {
      fanEnabled = false;
      analogWrite(PWM_PIN, 0);
      Output = 0;
      myPID.SetMode(MANUAL);
      // Update UI switch state
      if (fanSwitch)
        lv_obj_clear_state(fanSwitch, LV_STATE_CHECKED);
    }
  }
  else if (strncmp(command, "TIMER:", 6) == 0)
  {
    if (strcmp(command + 6, "START") == 0)
    {
      if (elapsedTime == 0 && !timerRunning)
        startTimer();
      else
        resumeTimer();
      // Update button text to STOP
      if (btnSSTmr)
      {
        lv_obj_t *label = lv_obj_get_child(btnSSTmr, 0);
        if (label)
          lv_label_set_text(label, "STOP");
      }
    }
    else if (strcmp(command + 6, "STOP") == 0)
    {
      pauseTimer();
      // Update button text to START
      if (btnSSTmr)
      {
        lv_obj_t *label = lv_obj_get_child(btnSSTmr, 0);
        if (label)
          lv_label_set_text(label, "START");
      }
    }
    else if (strcmp(command + 6, "RESET") == 0)
    {
      resetTimer();
    }
  }
  else if (strncmp(command, "PID:", 4) == 0)
  {
    // Format: CMD:PID:Kp:Ki:Kd
    char *pidValues = command + 4;
    char *token = strtok(pidValues, ":");
    if (token)
      Kp = atof(token);
    token = strtok(NULL, ":");
    if (token)
      Ki = atof(token);
    token = strtok(NULL, ":");
    if (token)
      Kd = atof(token);
    myPID.SetTunings(Kp, Ki, Kd);
    saveSettingsToEEPROM();
  }
  else if (strncmp(command, "DISPLAY_INT:", 12) == 0)
  {
    displayUpdateInterval = atol(command + 12);
    saveSettingsToEEPROM();
  }
  else if (strncmp(command, "CHART_INT:", 10) == 0)
  {
    chartUpdateInterval = atol(command + 10);
    saveSettingsToEEPROM();
  }
  else if (strncmp(command, "HOLD_TIME:", 10) == 0)
  {
    buttonHoldTime = atol(command + 10);
    saveSettingsToEEPROM();
  }
  else if (strncmp(command, "SWAP_INT:", 9) == 0)
  {
    swapInterval = atol(command + 9);
    lv_timer_set_period(containerSwapTimer, swapInterval);
    saveSettingsToEEPROM();
  }
  else if (strncmp(command, "FADE_DUR:", 9) == 0)
  {
    fadeDuration = atoi(command + 9);
    saveSettingsToEEPROM();
  }
  else if (strcmp(command, "GET_CONFIG") == 0)
  {
    // Send current configuration to Web Server
    WebSerial.print("CONFIG:");
    WebSerial.print(displayUpdateInterval);
    WebSerial.print(",");
    WebSerial.print(chartUpdateInterval);
    WebSerial.print(",");
    WebSerial.print(buttonHoldTime);
    WebSerial.print(",");
    WebSerial.print(swapInterval);
    WebSerial.print(",");
    WebSerial.print(fadeDuration);
    WebSerial.print(",");
    WebSerial.print(Kp, 2);
    WebSerial.print(",");
    WebSerial.print(Ki, 2);
    WebSerial.print(",");
    WebSerial.println(Kd, 2);

    // Optionally duplicate to USB serial for debugging
    #if ENABLE_USB_SERIAL_DEBUG
    Serial.print("CONFIG:");
    Serial.print(displayUpdateInterval);
    Serial.print(",");
    Serial.print(chartUpdateInterval);
    Serial.print(",");
    Serial.print(buttonHoldTime);
    Serial.print(",");
    Serial.print(swapInterval);
    Serial.print(",");
    Serial.print(fadeDuration);
    Serial.print(",");
    Serial.print(Kp, 2);
    Serial.print(",");
    Serial.print(Ki, 2);
    Serial.print(",");
    Serial.println(Kd, 2);
    #endif
  }
}

// Function to cech and see if there is data coming from the web server via Serial
void checkSerialInput()
{
  // Check for incoming commands from Web Server on GPIO 33/34
  while (WebSerial.available())
  {
    char c = WebSerial.read();
    if (c == '\n' || c == '\r')
    {
      if (serialBufferIndex > 0)
      {
        serialBuffer[serialBufferIndex] = '\0';
        processSerialCommand(serialBuffer);

        // Optionally echo command to USB serial for debugging
        #if ENABLE_USB_SERIAL_DEBUG
        Serial.print("RX CMD: ");
        Serial.println(serialBuffer);
        #endif

        serialBufferIndex = 0;
      }
    }
    else if (serialBufferIndex < SERIAL_BUFFER_SIZE - 1)
    {
      serialBuffer[serialBufferIndex++] = c;
    }
  }
} // end of check serial input function

// Timer callback for tachometer polling (runs every 100 microseconds)
// This polls the GPIO state to detect falling edges, avoiding interrupt issues with LVGL/DMA
void IRAM_ATTR tachTimerCallback(void* arg)
{
  int currentState = digitalRead(TACH_PIN);
  // Detect falling edge (HIGH to LOW transition)
  if (lastTachState == HIGH && currentState == LOW) {
    tachPulseCount++;
  }
  lastTachState = currentState;
}

// Setup hardware timer for tach polling
void setupTachTimer()
{
  esp_timer_create_args_t timerArgs = {
    .callback = tachTimerCallback,
    .arg = NULL,
    .dispatch_method = ESP_TIMER_TASK,
    .name = "tach_timer",
    .skip_unhandled_events = true
  };

  esp_timer_create(&timerArgs, &tachTimerHandle);
  // Poll every 100 microseconds (10kHz) - fast enough for fan tach signals
  esp_timer_start_periodic(tachTimerHandle, 100);
}

// start of my_disp_flush function
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *color_map)
{
  const int offsetx1 = area->x1;
  const int offsetx2 = area->x2;
  const int offsety1 = area->y1;
  const int offsety2 = area->y2;
  lcd.lcd_draw_bitmap(offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
  lv_display_flush_ready(disp);
} // end of my_disp_flush function

// start of my_touchpad_read function
// handles screen touch events
void my_touchpad_read(lv_indev_t *indev_driver, lv_indev_data_t *data)
{
  bool touched;
  uint16_t touchX, touchY;

  touched = touch.getTouch(&touchX, &touchY);

  if (!touched)
  {
    data->state = LV_INDEV_STATE_REL;
  }
  else
  {
    data->state = LV_INDEV_STATE_PR;

    data->point.x = touchX;
    data->point.y = touchY;

    #if ENABLE_USB_SERIAL_DEBUG
    Serial.printf("x=%d,y=%d \r\n", touchX, touchY);
    #endif
  }
} // end of my_touchpad_read function

// start of Setup Function.
// Handles board and screen setup
void setup()
{
  // Initialize USB Serial for debugging
  #if ENABLE_USB_SERIAL_DEBUG
  Serial.begin(SERIAL_BAUD);
  // Wait for serial connection (USB CDC on ESP32-P4)
  unsigned long serialTimeout = millis();
  while (!Serial && (millis() - serialTimeout < 3000))
  {
    delay(10);
  }
  delay(100);
  Serial.println("Smoker Controller Starting...");
  #endif

  // Initialize Web Server Serial on GPIO 33 (RX) and 34 (TX)
  WebSerial.begin(SERIAL_BAUD, SERIAL_8N1, WEB_SERIAL_RX, WEB_SERIAL_TX);

  #if ENABLE_USB_SERIAL_DEBUG
  Serial.println("Web Server Serial initialized on GPIO 33/34");
  #endif

  // Load saved settings from Preferences
  loadSettingsFromEEPROM();

  #if ENABLE_USB_SERIAL_DEBUG
  Serial.println("Preferences loaded");
  #endif

  lv_init();
  // Serial.println("[✓] LVGL initialized");

  lcd.begin();
  touch.begin();

  uint32_t buffer_size = LCD_H_RES * LCD_V_RES;
  buf = (uint32_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM);
  buf1 = (uint32_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM);
  assert(buf);
  assert(buf1);
  disp_drv = lv_display_create(LCD_H_RES, LCD_V_RES);
  lv_display_set_flush_cb(disp_drv, my_disp_flush);
  lv_display_set_buffers(disp_drv, buf, buf1, buffer_size * sizeof(uint32_t), LV_DISPLAY_RENDER_MODE_FULL);

  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, my_touchpad_read);
  // Serial.println("[✓] Display configured");

  SetTemp = StartupTemp; // set the start up temperature
  Setpoint = SetTemp;

  initialStyle();
  initialObjects();

  time_Timer = lv_timer_create(printLocalTimeTimer, 1000, NULL);
  lv_timer_set_repeat_count(time_Timer, -1);

  // Create timer for container swap (every swapInterval ms)
  containerSwapTimer = lv_timer_create(containerSwapTimerCb, swapInterval, NULL);
  lv_timer_set_repeat_count(containerSwapTimer, -1);

  pinMode(TACH_PIN, INPUT_PULLUP); // set the Fan tachometer pin to Input_PullUP
  lastTachState = digitalRead(TACH_PIN); // initialize last state
  setupTachTimer();                // Setup hardware timer for tach polling

  pinMode(PWM_PIN, OUTPUT); // set the fan pin to OUTPUT
  analogWrite(PWM_PIN, 0);  // set the fan to off at start up

  // Temperature sensors are now on ESP32-S3 Web Server
  // This controller receives temperature data via TEMPS: messages
  #if ENABLE_USB_SERIAL_DEBUG
  Serial.println("Temperature sensors handled by ESP32-S3 Web Server");
  Serial.println("Waiting for TEMPS: messages...");
  Serial.print("Watchdog timeout: ");
  Serial.print(TEMP_WATCHDOG_TIMEOUT / 1000);
  Serial.println(" seconds");
  #endif

  Output = 0;
  myPID.SetOutputLimits(0, MAX_PID_OUTPUT);
  myPID.SetMode(MANUAL); // make sure the PID controller is not running at startup

  lastDisplayUpdateTime = millis();
  lastChartUpdateTime = millis();

} // End of Setup Function

// Main loop functions
void loop()
{
  lv_tick_inc(5);
  lv_timer_handler();
  updateDisplay();

  // Handle serial communication
  checkSerialInput();

  // Send data to Web Server ESP32 at regular intervals
  if ((millis() - lastSerialUpdateTime) > SERIAL_UPDATE_INTERVAL)
  {
    sendSerialData();
    lastSerialUpdateTime = millis();
  }

  delay(5);
} // end of Loop Function

// start up update display function
// This function updates the display regularly
// with state changes and variable changes
// this is also where I will manage the PID controlloer if it is on
void updateDisplay()
{
  if ((millis() - lastDisplayUpdateTime) > displayUpdateInterval)
  {
    // ============ TEMPERATURE WATCHDOG CHECK ============
    // Check if we've received temperature data recently from ESP32-S3
    if (lastTempReceiveTime > 0 && (millis() - lastTempReceiveTime) > TEMP_WATCHDOG_TIMEOUT)
    {
      // No temperature data received within timeout - SAFETY SHUTDOWN
      tempSensorConnected = false;

      // Force disable PID and fan for safety
      if (fanEnabled == true)
      {
        fanEnabled = false;
        analogWrite(PWM_PIN, 0);
        Output = 0;
        myPID.SetMode(MANUAL);

        // Update UI switch state
        if (fanSwitch)
          lv_obj_clear_state(fanSwitch, LV_STATE_CHECKED);

        #if ENABLE_USB_SERIAL_DEBUG
        Serial.println("WATCHDOG: Temperature sensor timeout - FAN DISABLED FOR SAFETY!");
        #endif
      }
    }
    else if (lastTempReceiveTime > 0)
    {
      tempSensorConnected = true;
    }

    // Handle PID control if active (only if temp sensor connected)
    if (fanEnabled == true && tempSensorConnected)
    {
      myPID.SetMode(AUTOMATIC);
      myPID.Compute();
      analogWrite(PWM_PIN, Output);
    }
    else
    {
      analogWrite(PWM_PIN, 0);
      Output = 0;
      myPID.SetMode(MANUAL);
    }

    // update the fan drive percent bar
    int speedPercent = map(Output, 0, 255, 0, 100);            // convert current output value to a percent
    lv_bar_set_value(driveBar, speedPercent, LV_ANIM_ON);      // display it on the bar
    lv_label_set_text_fmt(drivePercent, "%d%%", speedPercent); // and the text

    // Use temperature data received from ESP32-S3 Web Server
    int smokerTemp = receivedSmokerTemp;
    int foodTemp = receivedFoodTemp;

    // update the smoker actual temperature display
    if (!tempSensorConnected)
    {
      // Show connection error
      lv_label_set_text(lblActualTemp, "NO LINK");
      lv_label_set_text(currentSmokerTemp, "Smoker: NO LINK");
    }
    else if (smokerTemp == -999)
    {
      lv_label_set_text(lblActualTemp, "---°F");
      lv_label_set_text(currentSmokerTemp, "Smoker: ---°F");
    }
    else
    {
      lv_label_set_text_fmt(lblActualTemp, "%d°F", smokerTemp);
      Input = smokerTemp; // Update PID input
      lv_label_set_text_fmt(currentSmokerTemp, "Smoker: %d°F", smokerTemp);
    }

    // update the Food temperature display
    if (!tempSensorConnected)
    {
      // Show connection error
      lv_label_set_text(lblFoodTemp, "NO LINK");
      lv_label_set_text(currentFoodTemp, "Food: NO LINK");
    }
    else if (foodTemp == -999)
    {
      lv_label_set_text(lblFoodTemp, "---°F");
      lv_label_set_text(currentFoodTemp, "Food: ---°F");
    }
    else
    {
      lv_label_set_text_fmt(lblFoodTemp, "%d°F", foodTemp);
      lv_label_set_text_fmt(currentFoodTemp, "Food: %d°F", foodTemp);
    }

    // Update chart only at the specified interval
    if ((millis() - lastChartUpdateTime) > chartUpdateInterval)
    {
      // Only update chart with valid temperature readings
      if (smokerTemp != -999 && tempSensorConnected)
      {
        lv_chart_set_next_value(tempChart, smokerSeries, smokerTemp);
      }
      if (foodTemp != -999 && tempSensorConnected)
      {
        lv_chart_set_next_value(tempChart, foodSeries, foodTemp);
      }
      lastChartUpdateTime = millis();
    }

    Serial.print("Pulses: "); Serial.println(tachPulseCount);
    // get the current RPM and update the display with the value
    unsigned long rpm = (tachPulseCount * 60);  // 2 pulses/rev, 500ms sample: (*120/2 = *60)
    int RPMtemp = map(rpm, 0, 3000, 0, 100);
    lv_bar_set_value(rpmBar, RPMtemp, LV_ANIM_ON);      // display it on the bar
    lv_label_set_text_fmt(rpmPercent, "%d%%", RPMtemp); // and the text
    globalRPM = rpm;
    tachPulseCount = 0;

    lastDisplayUpdateTime = millis(); // reset the current timer time
  }
}

// NOTE: MAX6675 temperature sensor functions have been moved to ESP32-S3 Web Server
// Temperature data is now received via TEMPS: messages from the S3

// start of initialObjects function
// This function creates the display objects and displays them
void initialObjects()
{

  LV_FONT_DECLARE(Antonio_Bold_36_NOHSP);
  LV_FONT_DECLARE(Antonio_Bold_40_NOHSP);
  LV_FONT_DECLARE(Antonio_Bold_60_NOHSP);
  LV_FONT_DECLARE(Arial_60);
  LV_FONT_DECLARE(Arial_Black_60);
  LV_FONT_DECLARE(Arial_Black_12);
  LV_FONT_DECLARE(UpDown120);
  LV_FONT_DECLARE(lv_font_montserrat_14);
  LV_FONT_DECLARE(lv_font_montserrat_20);
  LV_FONT_DECLARE(lv_font_montserrat_26);
  lv_obj_t *scr = lv_screen_active();

  // top swoop
  lv_obj_t *obj0 = lv_obj_create(scr);
  lv_obj_add_style(obj0, &style0, 0);
  lv_obj_set_style_bg_color(obj0, lv_color_make(136, 153, 255), LV_PART_MAIN);
  lv_obj_set_pos(obj0, -10, -40);

  // top row of screen devider
  lv_obj_t *obj1 = lv_obj_create(scr);
  lv_obj_add_style(obj1, &style1, 0);
  lv_obj_set_style_bg_color(obj1, lv_color_make(136, 153, 255), LV_PART_MAIN);
  lv_obj_set_pos(obj1, 80, 128);

  // top left corner
  lv_obj_t *obj2 = lv_obj_create(scr);
  lv_obj_add_style(obj2, &style2, 0);
  lv_obj_set_style_bg_color(obj2, lv_color_make(204, 153, 255), LV_PART_MAIN);
  lv_obj_set_pos(obj2, 0, 0);

  lv_obj_t *obj3 = lv_obj_create(scr);
  lv_obj_add_style(obj3, &styleSeparatorHorizontal, 0);
  lv_obj_set_style_bg_color(obj3, lv_color_black(), LV_PART_MAIN);
  lv_obj_set_pos(obj3, 0, 45);

  lv_obj_t *obj4 = lv_obj_create(scr);
  lv_obj_add_style(obj4, &style3, 0);
  lv_obj_set_style_bg_color(obj4, lv_color_black(), LV_PART_MAIN);
  lv_obj_set_pos(obj4, 90, -17);

  // bottom left swoop
  lv_obj_t *obj5 = lv_obj_create(scr);
  lv_obj_add_style(obj5, &style4, 0);
  lv_obj_set_style_bg_color(obj5, lv_color_make(204, 68, 68), LV_PART_MAIN);
  lv_obj_set_pos(obj5, -10, 140);

  // bottom of screen devider
  lv_obj_t *obj6 = lv_obj_create(scr);
  lv_obj_add_style(obj6, &style5, 0);
  lv_obj_set_style_bg_color(obj6, lv_color_make(204, 68, 68), LV_PART_MAIN);
  lv_obj_set_pos(obj6, 80, 142);

  lv_obj_t *obj7 = lv_obj_create(scr);
  lv_obj_add_style(obj7, &styleSeparatorHorizontal, 0);
  lv_obj_set_style_bg_color(obj7, lv_color_make(0, 0, 0), LV_PART_MAIN);
  lv_obj_set_pos(obj7, 0, 240);

  lv_obj_t *obj8 = lv_obj_create(scr);
  lv_obj_add_style(obj8, &style6, 0);
  lv_obj_set_style_bg_color(obj8, lv_color_make(204, 68, 68), LV_PART_MAIN);
  lv_obj_set_pos(obj8, 0, 245);

  lv_obj_t *obj9 = lv_obj_create(scr);
  lv_obj_add_style(obj9, &styleSeparatorHorizontal, 0);
  lv_obj_set_style_bg_color(obj9, lv_color_make(0, 0, 0), LV_PART_MAIN);
  lv_obj_set_pos(obj9, 0, 385);

  // second from bottom box on left
  lv_obj_t *obj10 = lv_obj_create(scr);
  lv_obj_add_style(obj10, &style7, 0);
  lv_obj_set_style_bg_color(obj10, lv_color_make(255, 136, 0), LV_PART_MAIN);
  lv_obj_set_pos(obj10, 0, 390);
  lv_obj_set_scroll_dir(obj10, LV_DIR_NONE);

  lv_obj_t *obj11 = lv_obj_create(scr);
  lv_obj_add_style(obj11, &style8, 0);
  lv_obj_set_style_bg_color(obj11, lv_color_make(0, 0, 0), LV_PART_MAIN);
  lv_obj_set_pos(obj11, 90, 152);

  lv_obj_t *obj12 = lv_obj_create(scr);
  lv_obj_add_style(obj12, &styleSeparatorHorizontal, 0);
  lv_obj_set_style_bg_color(obj12, lv_color_make(0, 0, 0), LV_PART_MAIN);
  lv_obj_set_pos(obj12, 0, 425);
  lv_obj_set_scroll_dir(obj12, LV_DIR_NONE);

  // bottom left corner box
  lv_obj_t *obj13 = lv_obj_create(scr);
  lv_obj_add_style(obj13, &style9, 0);
  lv_obj_set_style_bg_color(obj13, lv_color_make(255, 153, 102), LV_PART_MAIN);
  lv_obj_set_pos(obj13, 0, 430);

  lv_obj_t *obj14 = lv_obj_create(scr);
  lv_obj_add_style(obj14, &styleSeparatorVertical, 0);
  lv_obj_set_style_bg_color(obj14, lv_color_make(0, 0, 0), LV_PART_MAIN);
  lv_obj_set_pos(obj14, 400, 128);

  lv_obj_t *obj15 = lv_obj_create(scr);
  lv_obj_add_style(obj15, &styleSeparatorVertical, 0);
  lv_obj_set_style_bg_color(obj15, lv_color_make(0, 0, 0), LV_PART_MAIN);
  lv_obj_set_pos(obj15, 450, 128);

  lv_obj_t *obj16 = lv_obj_create(scr);
  lv_obj_add_style(obj16, &styleSeparatorVertical, 0);
  lv_obj_set_style_bg_color(obj16, lv_color_make(0, 0, 0), LV_PART_MAIN);
  lv_obj_set_pos(obj16, 600, 128);

  lv_obj_t *obj17 = lv_obj_create(scr);
  lv_obj_add_style(obj17, &styleSeparatorVertical, 0);
  lv_obj_set_style_bg_color(obj17, lv_color_make(0, 0, 0), LV_PART_MAIN);
  lv_obj_set_pos(obj17, 790, 128);

  // start of timer
  // Create the main blue/purple rounded rectangle container
  lv_obj_t *mainContainer = lv_obj_create(lv_scr_act());
  lv_obj_set_size(mainContainer, 400, 120);
  lv_obj_set_pos(mainContainer, 110, 1);
  lv_obj_set_style_bg_color(mainContainer, lv_color_make(136, 153, 255), 0);
  lv_obj_set_style_radius(mainContainer, 25, 0);  // Rounded corners
  lv_obj_set_style_pad_all(mainContainer, 0, 0); // Inner padding
  lv_obj_set_style_border_width(mainContainer, 0, 0);
  lv_obj_set_scroll_dir(mainContainer, LV_DIR_NONE);

  // Create the black time display section at the top
  lv_obj_t *timeSection = lv_obj_create(mainContainer);
  lv_obj_set_size(timeSection, 320, 50);
  lv_obj_set_pos(timeSection, 40, 5);
  lv_obj_set_style_bg_color(timeSection, lv_color_hex(0x000000), 0); // Black
  lv_obj_set_style_radius(timeSection, 15, 0);
  lv_obj_set_style_border_width(timeSection, 0, 0);
  lv_obj_set_style_pad_all(timeSection, 5, 0);
  lv_obj_set_scroll_dir(timeSection, LV_DIR_NONE);

  // Time label with yellow/orange text
  timeLabel = lv_label_create(timeSection); // ← Remove "lv_obj_t *" - use global
  lv_obj_set_style_text_color(timeLabel, lv_color_make(255, 170, 0), 0);
  lv_obj_set_style_text_font(timeLabel, &Antonio_Bold_40_NOHSP, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_center(timeLabel);
  lv_label_set_text(timeLabel, "00:00:00");
  lv_obj_set_scroll_dir(timeLabel, LV_DIR_NONE);

  // Create button container area for timer
  lv_obj_t *buttonArea = lv_obj_create(mainContainer);
  lv_obj_set_size(buttonArea, 370, 60);
  lv_obj_set_pos(buttonArea, 20, 60);
  lv_obj_set_style_bg_color(buttonArea, lv_color_make(136, 153, 255), 0);
  lv_obj_set_style_pad_top(buttonArea, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_bottom(buttonArea, 0, LV_PART_MAIN);
  lv_obj_set_style_border_width(buttonArea, 0, 0);
  lv_obj_set_scroll_dir(buttonArea, LV_DIR_NONE);

  // Start/Stop button left side
  btnSSTmr = lv_btn_create(buttonArea); // ← Remove "lv_obj_t *" - use global
  lv_obj_set_size(btnSSTmr, 180, 58);
  lv_obj_set_pos(btnSSTmr, 0, 0);
  lv_obj_set_style_bg_color(btnSSTmr, lv_color_hex(0xFFA500), 0);
  lv_obj_set_style_radius(btnSSTmr, 12, 0);
  lv_obj_set_style_border_width(btnSSTmr, 0, 0);
  lv_obj_t *lblStartStop = lv_label_create(btnSSTmr);
  lv_label_set_text(lblStartStop, "START");
  lv_obj_set_style_text_color(lblStartStop, lv_color_hex(0x000000), 0);
  lv_obj_set_style_text_font(lblStartStop, &Antonio_Bold_40_NOHSP, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_center(lblStartStop);
  lv_obj_add_event_cb(btnSSTmr, SSTimer, LV_EVENT_ALL, NULL);

  // Reset button (gray) - right side
  btnTimerReset = lv_btn_create(buttonArea); // ← Remove "lv_obj_t *" - use global
  lv_obj_set_size(btnTimerReset, 110, 58);
  lv_obj_set_pos(btnTimerReset, 210, 0);
  lv_obj_set_style_bg_color(btnTimerReset, lv_color_make(228, 17, 52), 0);
  lv_obj_set_style_radius(btnTimerReset, 12, 0);
  lv_obj_set_style_border_width(btnTimerReset, 0, 0);
  lv_obj_t *lblReset = lv_label_create(btnTimerReset);
  lv_label_set_text(lblReset, "RESET");
  lv_obj_set_style_text_color(lblReset, lv_color_hex(0x000000), 0);
  lv_obj_set_style_text_font(lblReset, &Antonio_Bold_40_NOHSP, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_center(lblReset);
  lv_obj_add_event_cb(btnTimerReset, TimerResetFunction, LV_EVENT_ALL, NULL);

  lv_style_init(&styleTime);
  lv_style_set_text_align(&styleTime, LV_TEXT_ALIGN_CENTER);
  lv_style_set_text_color(&styleTime, lv_color_make(255, 255, 255));

  lv_style_init(&styleUP);
  lv_style_set_text_align(&styleUP, LV_TEXT_ALIGN_CENTER);
  lv_style_set_height(&styleUP, 90);
  lv_style_set_pad_all(&styleUP, 0); // Sets all padding to 0
  lv_style_set_bg_color(&styleUP, lv_color_make(255, 136, 102));
  lv_style_set_border_width(&styleUP, 0);
  lv_style_set_shadow_color(&styleUP, lv_color_make(255, 153, 0));

  lv_style_init(&styleSetTemp);
  lv_style_set_text_align(&styleSetTemp, LV_TEXT_ALIGN_CENTER);
  lv_style_set_height(&styleSetTemp, 70);
  lv_style_set_text_color(&styleSetTemp, lv_color_make(255, 255, 255));
  lv_style_set_bg_color(&styleSetTemp, lv_color_make(255, 255, 0));

  lv_style_init(&styleSSTmr);
  lv_style_set_text_align(&styleSSTmr, LV_TEXT_ALIGN_CENTER);
  lv_style_set_height(&styleSSTmr, 70);
  lv_style_set_bg_color(&styleSSTmr, lv_color_make(200, 100, 0));
  lv_style_set_border_width(&styleSSTmr, 0);
  lv_style_set_shadow_color(&styleSSTmr, lv_color_make(0, 0, 0));

  lv_style_init(&styleTimerReset);
  lv_style_set_text_align(&styleTimerReset, LV_TEXT_ALIGN_CENTER);
  lv_style_set_height(&styleTimerReset, 70);
  lv_style_set_bg_color(&styleTimerReset, lv_color_make(200, 200, 0));
  lv_style_set_border_width(&styleTimerReset, 0);
  lv_style_set_shadow_color(&styleTimerReset, lv_color_make(0, 0, 0));

  static lv_style_t black_bg_style;
  lv_style_init(&black_bg_style);
  lv_style_set_bg_color(&black_bg_style, lv_color_black());
  lv_obj_add_style(scr, &black_bg_style, 0);

  // Actual temperature setion
  ActualTempContainer = lv_obj_create(lv_scr_act());
  lv_obj_set_size(ActualTempContainer, 300, 135);
  lv_obj_set_pos(ActualTempContainer, 110, 160);
  lv_obj_set_style_bg_color(ActualTempContainer, lv_color_make(255, 170, 0), 0);
  lv_obj_set_style_radius(ActualTempContainer, 25, 0); // Rounded corners
  lv_obj_set_style_pad_all(ActualTempContainer, 2, 0); // Inner padding
  lv_obj_set_style_border_width(ActualTempContainer, 0, 0);
  lv_obj_set_scroll_dir(ActualTempContainer, LV_DIR_NONE);

  lv_obj_t *ActualTempLabel = lv_label_create(ActualTempContainer); // ← Remove "lv_obj_t *" - use global
  lv_obj_align(ActualTempLabel, LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_set_style_text_color(ActualTempLabel, lv_color_make(0, 0, 0), 0);
  lv_obj_set_style_text_font(ActualTempLabel, &Antonio_Bold_60_NOHSP, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_label_set_text(ActualTempLabel, "SMOKER TEMP");
  lv_obj_set_scroll_dir(ActualTempLabel, LV_DIR_NONE);

  // Display actual temperature on screen display
  lblActualTemp = lv_label_create(ActualTempContainer);
  lv_obj_align(lblActualTemp, LV_ALIGN_TOP_MID, 0, 70);
  lv_obj_add_style(lblActualTemp, &styleSetTemp, 0);
  lv_obj_set_style_text_font(lblActualTemp, &Arial_Black_60, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(lblActualTemp, lv_color_make(0, 0, 0), 0);
  lv_label_set_text(lblActualTemp, "150°F");

  // Set Temp section
  lv_obj_t *SetTempContainer = lv_obj_create(lv_scr_act());
  // lv_obj_t *SetTempTextBox = lv_obj_create(SetTempContainer);
  lv_obj_set_size(SetTempContainer, 300, 135);
  lv_obj_set_pos(SetTempContainer, 110, 310);
  lv_obj_set_style_bg_color(SetTempContainer, lv_color_make(255, 136, 102), 0);
  lv_obj_set_style_radius(SetTempContainer, 25, 0); // Rounded corners
  lv_obj_set_style_pad_all(SetTempContainer, 2, 0); // Inner padding
  lv_obj_set_style_border_width(SetTempContainer, 0, 0);
  lv_obj_set_scroll_dir(SetTempContainer, LV_DIR_NONE);
  lv_obj_t *SetTempLabel = lv_label_create(SetTempContainer); // ← Remove "lv_obj_t *" - use global
  lv_obj_align(SetTempLabel, LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_set_style_text_color(SetTempLabel, lv_color_make(0, 0, 0), 0);
  lv_obj_set_style_text_font(SetTempLabel, &Antonio_Bold_60_NOHSP, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_label_set_text(SetTempLabel, "SET TEMP");
  lv_obj_set_scroll_dir(SetTempLabel, LV_DIR_NONE);

  // set temperature on screen display
  lblSetTemp = lv_label_create(SetTempContainer);
  lv_obj_align(lblSetTemp, LV_ALIGN_TOP_MID, 0, 70);
  lv_obj_add_style(lblSetTemp, &styleSetTemp, 0);
  lv_obj_set_style_text_font(lblSetTemp, &Arial_Black_60, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(lblSetTemp, lv_color_make(0, 0, 0), 0);
  lv_label_set_text(lblSetTemp, (String(SetTemp) + "°F").c_str());

  // up and down arrows for settin temperature
  // Time label with yellow/orange text
  lv_obj_t *UpDownContainer = lv_obj_create(lv_scr_act());
  // lv_obj_t *SetTempTextBox = lv_obj_create(SetTempContainer);
  lv_obj_set_size(UpDownContainer, 300, 120);
  lv_obj_set_pos(UpDownContainer, 110, 460);
  lv_obj_set_style_bg_color(UpDownContainer, lv_color_make(255, 136, 102), 0);
  lv_obj_set_style_radius(UpDownContainer, 25, 0);  // Rounded corners
  lv_obj_set_style_pad_all(UpDownContainer, 15, 0); // Inner padding
  lv_obj_set_style_border_width(UpDownContainer, 0, 0);
  lv_obj_set_scroll_dir(UpDownContainer, LV_DIR_NONE);

  btnUP = lv_btn_create(UpDownContainer);
  lv_obj_t *lblUP = lv_label_create(btnUP);
  lv_label_set_text(lblUP, "▲");
  lv_obj_set_pos(btnUP, 145, 5);
  lv_obj_add_style(btnUP, &styleUP, 0);
  lv_obj_set_style_text_font(btnUP, &UpDown120, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(btnUP, lv_color_make(0, 0, 0), 0);
  lv_obj_add_event_cb(btnUP, incrementSetTemp, LV_EVENT_ALL, NULL);

  btnDOWN = lv_btn_create(UpDownContainer);
  lv_obj_t *lblDOWN = lv_label_create(btnDOWN);
  lv_label_set_text(lblDOWN, "▼");
  lv_obj_set_pos(btnDOWN, 0, 5);
  lv_obj_add_style(btnDOWN, &styleUP, 0);
  lv_obj_set_style_text_font(btnDOWN, &UpDown120, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(btnDOWN, lv_color_make(0, 0, 0), 0);
  lv_obj_add_event_cb(btnDOWN, decrementSetTemp, LV_EVENT_ALL, NULL);

  // Food temperature section
  FoodTempContainer = lv_obj_create(lv_scr_act());
  lv_obj_set_size(FoodTempContainer, 300, 135);
  lv_obj_set_pos(FoodTempContainer, 110, 160);
  lv_obj_set_style_bg_color(FoodTempContainer, lv_color_make(204, 68, 68), 0);
  lv_obj_set_style_radius(FoodTempContainer, 25, 0); // Rounded corners
  lv_obj_set_style_pad_all(FoodTempContainer, 2, 0); // Inner padding
  lv_obj_set_style_border_width(FoodTempContainer, 0, 0);
  lv_obj_set_scroll_dir(FoodTempContainer, LV_DIR_NONE);

  lv_obj_t *FoodTempLabel = lv_label_create(FoodTempContainer); // ← Remove "lv_obj_t *" - use global
  lv_obj_align(FoodTempLabel, LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_set_style_text_color(FoodTempLabel, lv_color_make(0, 0, 0), 0);
  lv_obj_set_style_text_font(FoodTempLabel, &Antonio_Bold_60_NOHSP, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_label_set_text(FoodTempLabel, "FOOD TEMP");
  lv_obj_set_scroll_dir(FoodTempLabel, LV_DIR_NONE);

  // Display food temperature on screen display
  lblFoodTemp = lv_label_create(FoodTempContainer);
  lv_obj_align(lblFoodTemp, LV_ALIGN_TOP_MID, 0, 70);
  lv_obj_add_style(lblFoodTemp, &styleSetTemp, 0);
  lv_obj_set_style_text_font(lblFoodTemp, &Arial_Black_60, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(lblFoodTemp, lv_color_make(0, 0, 0), 0);
  lv_label_set_text(lblFoodTemp, "75°F");

  // Initially hide FoodTempContainer (ActualTempContainer shows first)
  lv_obj_add_flag(FoodTempContainer, LV_OBJ_FLAG_HIDDEN);

  // end of food temperature section

  // fan section
  // Create the main FAN container
  lv_obj_t *fanContainer = lv_obj_create(lv_scr_act());
  lv_obj_set_size(fanContainer, 450, 120);
  lv_obj_set_pos(fanContainer, 550, 0); // Adjust position as needed
  lv_obj_set_style_bg_color(fanContainer, lv_color_hex(0xffaa00), 0);
  lv_obj_set_style_radius(fanContainer, 25, 0);
  lv_obj_set_style_pad_all(fanContainer, 10, 0);
  lv_obj_set_style_border_width(fanContainer, 0, 0);
  lv_obj_clear_flag(fanContainer, LV_OBJ_FLAG_SCROLLABLE);

  // Create title "FAN"
  lv_obj_t *fanTitle = lv_label_create(fanContainer);
  lv_label_set_text(fanTitle, "FAN");
  lv_obj_align(fanTitle, LV_ALIGN_TOP_RIGHT, -35, 0);
  lv_obj_set_style_text_color(fanTitle, lv_color_black(), 0);
  lv_obj_set_style_text_font(fanTitle, &lv_font_montserrat_20, 0);

  // Create LARGE ON/OFF switch
  fanSwitch = lv_switch_create(fanContainer);
  lv_obj_set_size(fanSwitch, 110, 60);         // Larger size for easier touch
  lv_obj_set_style_anim_time(fanSwitch, 0, 0); // Faster (100ms)
  lv_obj_align(fanSwitch, LV_ALIGN_TOP_RIGHT, 0, 30);

  // Optional: Style the switch colors
  lv_obj_set_style_bg_color(fanSwitch, lv_color_hex(0x404040), LV_PART_MAIN);
  lv_obj_set_style_bg_color(fanSwitch, lv_color_hex(0x00ff00), LV_PART_INDICATOR);

  // Add switch event handler (optional)
  lv_obj_add_event_cb(fanSwitch, fan_switch_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

  // Create Fan RPM Bar
  lv_obj_t *rpmLabel = lv_label_create(fanContainer);
  lv_label_set_text(rpmLabel, "Fan RPM");
  lv_obj_align(rpmLabel, LV_ALIGN_LEFT_MID, 10, -10);
  lv_obj_set_style_text_color(rpmLabel, lv_color_black(), 0);
  lv_obj_set_style_text_font(rpmLabel, &lv_font_montserrat_20, 0);

  rpmBar = lv_bar_create(fanContainer);
  lv_obj_set_size(rpmBar, 150, 20);
  lv_obj_align_to(rpmBar, rpmLabel, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
  lv_bar_set_range(rpmBar, 0, 100);
  lv_bar_set_value(rpmBar, 75, LV_ANIM_ON); // Set to 75% as example
  lv_obj_set_style_bg_color(rpmBar, lv_color_hex(0x303030), LV_PART_MAIN);
  lv_obj_set_style_bg_color(rpmBar, lv_color_hex(0x00ff00), LV_PART_INDICATOR);
  lv_obj_set_style_radius(rpmBar, 5, LV_PART_MAIN);

  // Create RPM percentage label
  rpmPercent = lv_label_create(fanContainer);
  lv_label_set_text_fmt(rpmPercent, "%d%%", 75); // Update this with actual value
  lv_obj_align_to(rpmPercent, rpmBar, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
  lv_obj_set_style_text_color(rpmPercent, lv_color_black(), 0);
  lv_obj_set_style_text_font(rpmPercent, &lv_font_montserrat_20, 0);

  // Create Fan Drive Bar
  lv_obj_t *driveLabel = lv_label_create(fanContainer);
  lv_label_set_text(driveLabel, "Fan Drive");
  lv_obj_align(driveLabel, LV_ALIGN_LEFT_MID, 10, 20);
  lv_obj_set_style_text_color(driveLabel, lv_color_black(), 0);
  lv_obj_set_style_text_font(driveLabel, &lv_font_montserrat_20, 0);

  driveBar = lv_bar_create(fanContainer);
  lv_obj_set_size(driveBar, 150, 20);
  lv_obj_align_to(driveBar, driveLabel, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
  lv_bar_set_range(driveBar, 0, 100);
  lv_bar_set_value(driveBar, 50, LV_ANIM_ON); // Set to 50% as example
  lv_obj_set_style_bg_color(driveBar, lv_color_hex(0x303030), LV_PART_MAIN);
  lv_obj_set_style_bg_color(driveBar, lv_color_hex(0x00aaff), LV_PART_INDICATOR);
  lv_obj_set_style_radius(driveBar, 5, LV_PART_MAIN);

  // Create Drive percentage label
  drivePercent = lv_label_create(fanContainer);
  lv_label_set_text_fmt(drivePercent, "%d%%", 50); // Update this with actual value
  lv_obj_align_to(drivePercent, driveBar, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
  lv_obj_set_style_text_color(drivePercent, lv_color_black(), 0);
  lv_obj_set_style_text_font(drivePercent, &lv_font_montserrat_20, 0);

  // graph section
  // Create the chart container
  lv_obj_t *chartContainer = lv_obj_create(lv_scr_act());
  lv_obj_set_size(chartContainer, 580, 400);
  lv_obj_set_pos(chartContainer, 420, 160); // Adjust position as needed
  lv_obj_set_style_bg_color(chartContainer, lv_color_hex(0x666688), 0);
  lv_obj_set_style_radius(chartContainer, 15, 0);
  lv_obj_set_style_border_width(chartContainer, 6, LV_PART_MAIN);
  lv_obj_set_style_border_color(chartContainer, lv_color_hex(0xfdfd96), 0);
  lv_obj_set_style_pad_all(chartContainer, 10, 0);
  lv_obj_clear_flag(chartContainer, LV_OBJ_FLAG_SCROLLABLE);

  // Create title
  lv_obj_t *chartTitle = lv_label_create(chartContainer);
  lv_label_set_text(chartTitle, "Temperature Monitor");
  lv_obj_set_pos(chartTitle, 20, 5);
  lv_obj_set_style_text_font(chartTitle, &Antonio_Bold_36_NOHSP, 0);
  lv_obj_set_style_text_color(chartTitle, lv_color_white(), 0);

  // Create the chart
  tempChart = lv_chart_create(chartContainer);
  lv_obj_set_size(tempChart, 530, 280);
  lv_obj_align(tempChart, LV_ALIGN_BOTTOM_MID, 0, -10);

  // Configure chart
  lv_chart_set_type(tempChart, LV_CHART_TYPE_LINE);
  lv_chart_set_point_count(tempChart, 50);                         // Number of data points to show
  lv_chart_set_range(tempChart, LV_CHART_AXIS_PRIMARY_Y, 0, 500);  // Temperature range 0-300°F
  lv_chart_set_update_mode(tempChart, LV_CHART_UPDATE_MODE_SHIFT); // Scroll left as new data comes in

  // Style the chart
  lv_obj_set_style_bg_color(tempChart, lv_color_hex(0x1a1a1a), LV_PART_MAIN);
  lv_obj_set_style_border_width(tempChart, 2, LV_PART_MAIN);
  lv_obj_set_style_border_color(tempChart, lv_color_hex(0x444444), LV_PART_MAIN);
  lv_obj_set_style_radius(tempChart, 5, LV_PART_MAIN);
  lv_obj_set_style_pad_all(tempChart, 10, LV_PART_MAIN);

  // Set grid lines
  lv_chart_set_div_line_count(tempChart, 5, 7); // 5 horizontal, 7 vertical lines
  lv_obj_set_style_line_color(tempChart, lv_color_hex(0x404040), LV_PART_MAIN);

  // Add data series for Smoker Temperature (Red)
  smokerSeries = lv_chart_add_series(tempChart,
                                     lv_color_hex(0xff4444),
                                     LV_CHART_AXIS_PRIMARY_Y);
  lv_chart_set_series_color(tempChart, smokerSeries, lv_color_hex(0xff4444));

  // Add data series for Food Temperature (Orange)
  foodSeries = lv_chart_add_series(tempChart,
                                   lv_color_hex(0xffaa44),
                                   LV_CHART_AXIS_PRIMARY_Y);
  lv_chart_set_series_color(tempChart, foodSeries, lv_color_hex(0xffaa44));

  // Create legend
  lv_obj_t *legendContainer = lv_obj_create(chartContainer);
  lv_obj_set_size(legendContainer, 190, 60);
  lv_obj_set_pos(legendContainer, 400, 2);
  lv_obj_set_style_bg_opa(legendContainer, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(legendContainer, 0, 0);
  lv_obj_set_style_pad_all(legendContainer, 0, 0);
  lv_obj_clear_flag(legendContainer, LV_OBJ_FLAG_SCROLLABLE);

  // Smoker temp legend
  lv_obj_t *smokerLegend = lv_obj_create(legendContainer);
  lv_obj_set_size(smokerLegend, 20, 20);
  lv_obj_set_pos(smokerLegend, 0, 5);
  lv_obj_set_style_bg_color(smokerLegend, lv_color_hex(0xff4444), 0);
  lv_obj_set_style_radius(smokerLegend, 3, 0);
  lv_obj_set_style_border_width(smokerLegend, 0, 0);

  lv_obj_t *smokerLabel = lv_label_create(legendContainer);
  lv_label_set_text(smokerLabel, "Smoker");
  lv_obj_align_to(smokerLabel, smokerLegend, LV_ALIGN_OUT_RIGHT_MID, 5, -3);
  lv_obj_set_style_text_color(smokerLabel, lv_color_white(), 0);
  lv_obj_set_style_text_font(smokerLabel, &lv_font_montserrat_20, 0);

  // Food temp legend
  lv_obj_t *foodLegend = lv_obj_create(legendContainer);
  lv_obj_set_size(foodLegend, 20, 20);
  lv_obj_set_pos(foodLegend, 0, 30);
  lv_obj_set_style_bg_color(foodLegend, lv_color_hex(0xffaa44), 0);
  lv_obj_set_style_radius(foodLegend, 3, 0);
  lv_obj_set_style_border_width(foodLegend, 0, 0);

  lv_obj_t *foodLabel = lv_label_create(legendContainer);
  lv_label_set_text(foodLabel, "Food");
  lv_obj_align_to(foodLabel, foodLegend, LV_ALIGN_OUT_RIGHT_MID, 5, -3);
  lv_obj_set_style_text_color(foodLabel, lv_color_white(), 0);
  lv_obj_set_style_text_font(foodLabel, &lv_font_montserrat_20, 0);

  // Current value display
  currentSmokerTemp = lv_label_create(chartContainer);
  lv_label_set_text(currentSmokerTemp, "Smoker: ---°F");
  lv_obj_align_to(currentSmokerTemp, tempChart, LV_ALIGN_OUT_TOP_LEFT, 0, -10);
  lv_obj_set_style_text_font(currentSmokerTemp, &lv_font_montserrat_26, 0);
  lv_obj_set_style_text_color(currentSmokerTemp, lv_color_hex(0xff4444), 0);

  currentFoodTemp = lv_label_create(chartContainer);
  lv_label_set_text(currentFoodTemp, "Food: ---°F");
  lv_obj_align_to(currentFoodTemp, currentSmokerTemp, LV_ALIGN_OUT_BOTTOM_LEFT, 220, -30);
  lv_obj_set_style_text_font(currentFoodTemp, &lv_font_montserrat_26, 0);
  lv_obj_set_style_text_color(currentFoodTemp, lv_color_hex(0xffaa44), 0);
}
// end of initialObjects function

// start of printLocalTimeTimer Function
// handles the on screen timer display
void printLocalTimeTimer(lv_timer_t *t)
{
  unsigned long currentElapsed = elapsedTime;

  // Only update elapsed time if timer is running
  if (timerRunning)
  {
    currentElapsed = elapsedTime + (millis() - timerStartTime);
  }

  // Convert to seconds
  unsigned long totalSeconds = currentElapsed / 1000;

  // Calculate hours, minutes, seconds
  int hours = totalSeconds / 3600;
  int minutes = (totalSeconds % 3600) / 60;
  int seconds = totalSeconds % 60;

  // Format the time strings
  sprintf(timeHour, "%02d", hours);
  sprintf(timeMin, "%02d", minutes);
  sprintf(timeSec, "%02d", seconds);

  // Create the time string
  _Time = String(timeHour) + ":" + String(timeMin) + ":" + String(timeSec);

  // Update the time label
  lv_label_set_text(timeLabel, _Time.c_str());
  // Serial.println(_Time);
}
// end of printLocalTimeTimer Function

// start of TimerResetFunction
void TimerResetFunction(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED)
  {
    resetTimer();
  }
}
//  end of TimerResetFunction

// start of SSTimer funtion
//  handles the touch screen function for the start and stop buttons
//  for the timer
void SSTimer(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED)
  {
    // Need to cast void* to lv_obj_t* for C++
    lv_obj_t *btn = (lv_obj_t *)lv_event_get_target(e);

    // Get the label of the button (first child)
    lv_obj_t *label = lv_obj_get_child(btn, 0);

    // Get current text
    const char *current_text = lv_label_get_text(label);

    // Toggle between SS and PP
    if (strcmp(current_text, "START") == 0)
    {
      lv_label_set_text(label, "STOP");
      if (elapsedTime == 0 && !timerRunning)
      {
        startTimer(); // First time - start from 00:00:00
      }
      else
      {
        resumeTimer(); // Resume from paused time
      }
    }
    else
    {

      lv_label_set_text(label, "START");
      pauseTimer();
    }
  }
}
// end of TSSTimer function

// start of the startTimer function
void startTimer()
{
  timerStartTime = millis();
  elapsedTime = 0;
  timerRunning = true;
}
// end of startTimer function

// Pause the timer (keeps current time)
void pauseTimer()
{
  if (timerRunning)
  {
    // Save the elapsed time before pausing
    elapsedTime += (millis() - timerStartTime);
    timerRunning = false;
  }
} // end of pauseTimer function

// Resume the timer from where it was paused
void resumeTimer()
{
  if (!timerRunning)
  {
    timerStartTime = millis();
    timerRunning = true;
  }
} // end of resumeTimer function

// Reset timer to 00:00:00
void resetTimer()
{
  elapsedTime = 0;
  timerStartTime = millis();
  // Keep the current running state
} // end of resetTimer function

// Animation callback for opacity changes
static void opacityAnimCb(void *var, int32_t v)
{
  lv_obj_set_style_opa((lv_obj_t *)var, v, 0);
}

// Callback when fade-out completes - hide the container
static void fadeOutReadyCb(lv_anim_t *a)
{
  lv_obj_add_flag((lv_obj_t *)a->var, LV_OBJ_FLAG_HIDDEN);
}

// Perform cross-fade between containers
void crossFadeContainers(void)
{
  lv_obj_t *containerToHide = showingActualTemp ? ActualTempContainer : FoodTempContainer;
  lv_obj_t *containerToShow = showingActualTemp ? FoodTempContainer : ActualTempContainer;

  // Prepare the new container for fade-in
  lv_obj_set_style_opa(containerToShow, LV_OPA_TRANSP, 0);
  lv_obj_clear_flag(containerToShow, LV_OBJ_FLAG_HIDDEN);

  // Create fade-out animation for current container
  lv_anim_t fadeOut;
  lv_anim_init(&fadeOut);
  lv_anim_set_var(&fadeOut, containerToHide);
  lv_anim_set_values(&fadeOut, LV_OPA_COVER, LV_OPA_TRANSP);
  lv_anim_set_time(&fadeOut, fadeDuration);
  lv_anim_set_exec_cb(&fadeOut, opacityAnimCb);
  lv_anim_set_ready_cb(&fadeOut, fadeOutReadyCb);
  lv_anim_start(&fadeOut);

  // Create fade-in animation for new container (runs simultaneously)
  lv_anim_t fadeIn;
  lv_anim_init(&fadeIn);
  lv_anim_set_var(&fadeIn, containerToShow);
  lv_anim_set_values(&fadeIn, LV_OPA_TRANSP, LV_OPA_COVER);
  lv_anim_set_time(&fadeIn, fadeDuration);
  lv_anim_set_exec_cb(&fadeIn, opacityAnimCb);
  lv_anim_start(&fadeIn);

  // Toggle the tracking variable
  showingActualTemp = !showingActualTemp;
}

// Immediately switch to ActualTempContainer (for temperature adjustment)
void switchToActualTemp(void)
{
  if (!showingActualTemp)
  {
    // Stop any running animations on these containers
    lv_anim_del(ActualTempContainer, opacityAnimCb);
    lv_anim_del(FoodTempContainer, opacityAnimCb);

    // Immediately hide FoodTempContainer and show ActualTempContainer
    lv_obj_add_flag(FoodTempContainer, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_opa(FoodTempContainer, LV_OPA_COVER, 0);

    lv_obj_clear_flag(ActualTempContainer, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_opa(ActualTempContainer, LV_OPA_COVER, 0);

    showingActualTemp = true;
  }
}

// Timer callback for swapping temperature containers
void containerSwapTimerCb(lv_timer_t *timer)
{
  // Check if we're within the button hold time
  unsigned long currentTime = millis();
  if (lastButtonPressTime > 0 && (currentTime - lastButtonPressTime) < buttonHoldTime)
  {
    // Still within hold time after button press, don't swap
    return;
  }

  // Reset the button press tracker after hold time expires
  if (lastButtonPressTime > 0 && (currentTime - lastButtonPressTime) >= buttonHoldTime)
  {
    lastButtonPressTime = 0;
  }

  // Perform the cross-fade
  crossFadeContainers();
}

// start of incrementSetTemp function
// the button event when the up button is pressed
void incrementSetTemp(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED)
  {
    // Serial.println(SetTemp);
    SetTemp += 5;
    if (SetTemp > MaxTemp)
    {
      SetTemp = MaxTemp;
    }
    lv_label_set_text(lblSetTemp, ((String)SetTemp + "°F").c_str());

    // Switch to ActualTempContainer immediately and track button press time
    switchToActualTemp();
    lastButtonPressTime = millis();
    Setpoint = SetTemp;
  }
} // end of incrementSetTemp function

// start of the decrementSetTemp function
// event handler for the down in screen button
void decrementSetTemp(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED)
  {
    // Serial.println(SetTemp);
    SetTemp -= 5;
    if (SetTemp < LowTemp)
    {
      SetTemp = LowTemp;
    }
    lv_label_set_text(lblSetTemp, ((String)SetTemp + "°F").c_str());

    // Switch to ActualTempContainer immediately and track button press time
    switchToActualTemp();
    lastButtonPressTime = millis();
    Setpoint = SetTemp;
  }
} // end of decrementSetTemp function

// start of the fan_switch_event_handler function
// this is the event handler for the on scren fan switch
static void fan_switch_event_handler(lv_event_t *e)
{
  lv_obj_t *sw = (lv_obj_t *)lv_event_get_target(e); // Add explicit cast
  if (lv_obj_has_state(sw, LV_STATE_CHECKED))
  {
    // Fan ON logic
    myPID.SetMode(AUTOMATIC);
    myPID.SetOutputLimits(0, MAX_PID_OUTPUT);
    myPID.Compute();
    analogWrite(PWM_PIN, Output);

    fanEnabled = true;

    int speedPercent = map(Output, 0, 255, 0, 100);
    lv_bar_set_value(driveBar, speedPercent, LV_ANIM_ON);      // display it on the bar
    lv_label_set_text_fmt(drivePercent, "%d%%", speedPercent); // and the text
  }
  else
  {
    // Fan OFF logic
    analogWrite(PWM_PIN, 0);
    Output = 0;
    myPID.SetMode(MANUAL);
    fanEnabled = false;
  }
} // end of fan_switch_event_handler function

#include "SessionManager.h"

SessionManager sessionManager;

SessionManager::SessionManager() {
  memset(&activeSession, 0, sizeof(activeSession));
  activeSession.isRecording = false;
}

bool SessionManager::begin() {
  if (!LittleFS.begin(true)) {  // true = format if mount fails
    Serial.println("LittleFS mount failed!");
    return false;
  }

  Serial.println("LittleFS mounted successfully");

  // Create sessions directory if it doesn't exist
  if (!createSessionsDir()) {
    Serial.println("Failed to create sessions directory");
    return false;
  }

  // Create index file if it doesn't exist
  if (!ensureIndexExists()) {
    Serial.println("Failed to create session index");
    return false;
  }

  Serial.println("SessionManager initialized");
  return true;
}

bool SessionManager::createSessionsDir() {
  if (!LittleFS.exists(SESSIONS_DIR)) {
    return LittleFS.mkdir(SESSIONS_DIR);
  }
  return true;
}

bool SessionManager::ensureIndexExists() {
  if (!LittleFS.exists(INDEX_FILE)) {
    DynamicJsonDocument doc(256);
    doc["version"] = 1;
    doc["nextId"] = 1;
    doc["sessions"] = doc.createNestedArray("sessions");

    File f = LittleFS.open(INDEX_FILE, "w");
    if (!f) return false;
    serializeJson(doc, f);
    f.close();
  }
  return true;
}

uint32_t SessionManager::generateSessionId() {
  // Read current nextId from index and increment
  File f = LittleFS.open(INDEX_FILE, "r");
  if (!f) return millis();  // Fallback to millis if can't read

  DynamicJsonDocument doc(16384);
  DeserializationError error = deserializeJson(doc, f);
  f.close();

  if (error) return millis();

  uint32_t id = doc["nextId"] | 1;
  doc["nextId"] = id + 1;

  // Write back
  f = LittleFS.open(INDEX_FILE, "w");
  if (f) {
    serializeJson(doc, f);
    f.close();
  }

  return id;
}

String SessionManager::getSessionFilePath(uint32_t id) {
  char path[48];
  snprintf(path, sizeof(path), "%s/session_%06lu.json", SESSIONS_DIR, (unsigned long)id);
  return String(path);
}

unsigned long SessionManager::getElapsedSeconds() {
  if (!activeSession.isRecording) return 0;
  return (millis() - activeSession.startMillis) / 1000;
}

bool SessionManager::startSession(const char* name, const char* meatType, int setTemp, uint32_t timestamp) {
  if (activeSession.isRecording) {
    Serial.println("Already recording a session");
    return false;
  }

  // Generate unique ID
  activeSession.id = generateSessionId();
  strncpy(activeSession.name, name, MAX_SESSION_NAME - 1);
  activeSession.name[MAX_SESSION_NAME - 1] = '\0';
  strncpy(activeSession.meatType, meatType, MAX_MEAT_TYPE - 1);
  activeSession.meatType[MAX_MEAT_TYPE - 1] = '\0';
  activeSession.initialSetTemp = setTemp;
  activeSession.currentSetTemp = setTemp;
  activeSession.startMillis = millis();
  activeSession.lastSampleTime = millis();
  activeSession.startTimestamp = timestamp;
  activeSession.isRecording = true;

  // Create session file
  DynamicJsonDocument doc(2048);
  doc["id"] = activeSession.id;
  doc["name"] = activeSession.name;
  doc["meat"] = activeSession.meatType;
  doc["setTemp"] = activeSession.initialSetTemp;
  doc["start"] = activeSession.startTimestamp;
  doc["end"] = 0;
  doc["duration"] = 0;
  doc["active"] = true;
  doc.createNestedArray("notes");
  JsonArray events = doc.createNestedArray("events");

  // Add start event
  JsonObject startEvent = events.createNestedObject();
  startEvent["t"] = 0;
  startEvent["e"] = EVENT_START;
  startEvent["st"] = setTemp;

  String path = getSessionFilePath(activeSession.id);
  File f = LittleFS.open(path, "w");
  if (!f) {
    Serial.println("Failed to create session file");
    activeSession.isRecording = false;
    return false;
  }
  serializeJson(doc, f);
  f.close();

  // Add to index
  File indexFile = LittleFS.open(INDEX_FILE, "r");
  if (!indexFile) {
    Serial.println("Failed to open index for reading");
    return false;
  }

  DynamicJsonDocument indexDoc(16384);
  DeserializationError error = deserializeJson(indexDoc, indexFile);
  indexFile.close();

  if (error) {
    Serial.println("Failed to parse index");
    return false;
  }

  JsonArray sessions = indexDoc["sessions"];
  JsonObject newSession = sessions.createNestedObject();
  newSession["id"] = activeSession.id;
  newSession["name"] = activeSession.name;
  newSession["meat"] = activeSession.meatType;
  newSession["setTemp"] = activeSession.initialSetTemp;
  newSession["start"] = activeSession.startTimestamp;
  newSession["end"] = 0;
  newSession["duration"] = 0;
  newSession["active"] = true;

  indexFile = LittleFS.open(INDEX_FILE, "w");
  if (!indexFile) {
    Serial.println("Failed to open index for writing");
    return false;
  }
  serializeJson(indexDoc, indexFile);
  indexFile.close();

  Serial.printf("Session %lu started: %s (%s)\n", (unsigned long)activeSession.id, name, meatType);
  return true;
}

bool SessionManager::endSession() {
  if (!activeSession.isRecording) {
    return false;
  }

  unsigned long duration = getElapsedSeconds();
  unsigned long endTime = activeSession.startTimestamp + duration;

  // Add end event to session file
  String path = getSessionFilePath(activeSession.id);
  File f = LittleFS.open(path, "r");
  if (!f) return false;

  DynamicJsonDocument doc(32768);
  DeserializationError error = deserializeJson(doc, f);
  f.close();

  if (error) return false;

  doc["end"] = endTime;
  doc["duration"] = duration;
  doc["active"] = false;

  JsonArray events = doc["events"];
  JsonObject endEvent = events.createNestedObject();
  endEvent["t"] = duration;
  endEvent["e"] = EVENT_END;

  f = LittleFS.open(path, "w");
  if (!f) return false;
  serializeJson(doc, f);
  f.close();

  // Update index
  updateIndexEntry(activeSession.id, false, endTime, duration);

  Serial.printf("Session %lu ended. Duration: %lu seconds\n", (unsigned long)activeSession.id, duration);

  activeSession.isRecording = false;
  memset(&activeSession, 0, sizeof(activeSession));

  return true;
}

bool SessionManager::isRecording() {
  return activeSession.isRecording;
}

uint32_t SessionManager::getActiveSessionId() {
  return activeSession.isRecording ? activeSession.id : 0;
}

String SessionManager::getActiveSessionName() {
  return activeSession.isRecording ? String(activeSession.name) : String("");
}

void SessionManager::recordTempChange(int newSetTemp) {
  if (!activeSession.isRecording) return;
  if (newSetTemp == activeSession.currentSetTemp) return;  // No change

  activeSession.currentSetTemp = newSetTemp;
  unsigned long elapsed = getElapsedSeconds();

  // Append event to session file
  String path = getSessionFilePath(activeSession.id);
  File f = LittleFS.open(path, "r");
  if (!f) return;

  DynamicJsonDocument doc(32768);
  DeserializationError error = deserializeJson(doc, f);
  f.close();

  if (error) return;

  JsonArray events = doc["events"];
  JsonObject event = events.createNestedObject();
  event["t"] = elapsed;
  event["e"] = EVENT_TEMP_CHANGE;
  event["st"] = newSetTemp;

  f = LittleFS.open(path, "w");
  if (!f) return;
  serializeJson(doc, f);
  f.close();

  Serial.printf("Recorded temp change to %d at %lu seconds\n", newSetTemp, elapsed);
}

void SessionManager::recordTempSample(int smokerTemp, int foodTemp) {
  if (!activeSession.isRecording) return;

  unsigned long elapsed = getElapsedSeconds();

  // Append event to session file
  String path = getSessionFilePath(activeSession.id);
  File f = LittleFS.open(path, "r");
  if (!f) return;

  DynamicJsonDocument doc(32768);
  DeserializationError error = deserializeJson(doc, f);
  f.close();

  if (error) return;

  JsonArray events = doc["events"];
  JsonObject event = events.createNestedObject();
  event["t"] = elapsed;
  event["e"] = EVENT_TEMP_SAMPLE;
  event["s"] = smokerTemp;
  event["f"] = foodTemp;

  f = LittleFS.open(path, "w");
  if (!f) return;
  serializeJson(doc, f);
  f.close();
}

bool SessionManager::addNote(const char* noteText) {
  if (!activeSession.isRecording) return false;

  unsigned long elapsed = getElapsedSeconds();

  // Append note to session file
  String path = getSessionFilePath(activeSession.id);
  File f = LittleFS.open(path, "r");
  if (!f) return false;

  DynamicJsonDocument doc(32768);
  DeserializationError error = deserializeJson(doc, f);
  f.close();

  if (error) return false;

  // Get or create notes array
  JsonArray notes = doc["notes"];
  if (notes.isNull()) {
    notes = doc.createNestedArray("notes");
  }

  if (notes.size() >= MAX_NOTES_PER_SESSION) {
    return false;
  }

  JsonObject note = notes.createNestedObject();
  note["t"] = elapsed;
  note["text"] = noteText;

  // Also add as event for timeline
  JsonArray events = doc["events"];
  if (events.isNull()) {
    events = doc.createNestedArray("events");
  }
  JsonObject event = events.createNestedObject();
  event["t"] = elapsed;
  event["e"] = EVENT_NOTE;

  f = LittleFS.open(path, "w");
  if (!f) return false;
  serializeJson(doc, f);
  f.close();

  return true;
}

String SessionManager::getSessionList() {
  File f = LittleFS.open(INDEX_FILE, "r");
  if (!f) {
    return "{\"sessions\":[]}";
  }

  DynamicJsonDocument doc(16384);
  DeserializationError error = deserializeJson(doc, f);
  f.close();

  if (error) {
    return "{\"sessions\":[]}";
  }

  // If there's an active session, update its duration in the response
  if (activeSession.isRecording) {
    JsonArray sessions = doc["sessions"];
    for (JsonObject session : sessions) {
      if (session["id"] == activeSession.id) {
        session["duration"] = getElapsedSeconds();
        break;
      }
    }
  }

  String result;
  serializeJson(doc, result);
  return result;
}

String SessionManager::getSessionDetails(uint32_t id) {
  String path = getSessionFilePath(id);
  File f = LittleFS.open(path, "r");
  if (!f) {
    return "{\"error\":\"Session not found\"}";
  }

  DynamicJsonDocument doc(32768);
  DeserializationError error = deserializeJson(doc, f);
  f.close();

  if (error) {
    return "{\"error\":\"Failed to parse session\"}";
  }

  // If this is the active session, update duration
  if (activeSession.isRecording && activeSession.id == id) {
    doc["duration"] = getElapsedSeconds();
  }

  String result;
  serializeJson(doc, result);
  return result;
}

bool SessionManager::deleteSession(uint32_t id) {
  // Can't delete active session
  if (activeSession.isRecording && activeSession.id == id) {
    return false;
  }

  // Delete session file
  String path = getSessionFilePath(id);
  if (LittleFS.exists(path)) {
    LittleFS.remove(path);
  }

  // Remove from index
  File f = LittleFS.open(INDEX_FILE, "r");
  if (!f) return false;

  DynamicJsonDocument doc(16384);
  DeserializationError error = deserializeJson(doc, f);
  f.close();

  if (error) return false;

  JsonArray sessions = doc["sessions"];
  for (size_t i = 0; i < sessions.size(); i++) {
    if (sessions[i]["id"] == id) {
      sessions.remove(i);
      break;
    }
  }

  f = LittleFS.open(INDEX_FILE, "w");
  if (!f) return false;
  serializeJson(doc, f);
  f.close();

  Serial.printf("Deleted session %lu\n", (unsigned long)id);
  return true;
}

int SessionManager::getSessionSetTemp(uint32_t id) {
  String path = getSessionFilePath(id);
  File f = LittleFS.open(path, "r");
  if (!f) return -1;

  DynamicJsonDocument doc(1024);  // Only need to read header
  DeserializationError error = deserializeJson(doc, f);
  f.close();

  if (error) return -1;

  return doc["setTemp"] | -1;
}

bool SessionManager::updateIndexEntry(uint32_t sessionId, bool active, unsigned long endTime, unsigned long duration) {
  File f = LittleFS.open(INDEX_FILE, "r");
  if (!f) return false;

  DynamicJsonDocument doc(16384);
  DeserializationError error = deserializeJson(doc, f);
  f.close();

  if (error) return false;

  JsonArray sessions = doc["sessions"];
  for (JsonObject session : sessions) {
    if (session["id"] == sessionId) {
      session["active"] = active;
      session["end"] = endTime;
      session["duration"] = duration;
      break;
    }
  }

  f = LittleFS.open(INDEX_FILE, "w");
  if (!f) return false;
  serializeJson(doc, f);
  f.close();

  return true;
}

void SessionManager::tick(int smokerTemp, int foodTemp) {
  if (!activeSession.isRecording) return;

  // Periodic temperature sampling every 2 minutes
  if ((millis() - activeSession.lastSampleTime) >= SAMPLE_INTERVAL) {
    recordTempSample(smokerTemp, foodTemp);
    activeSession.lastSampleTime = millis();
  }
}

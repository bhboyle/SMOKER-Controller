#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

// Configuration
#define SESSIONS_DIR "/sessions"
#define INDEX_FILE "/sessions/index.json"
#define MAX_SESSIONS 1000
#define MAX_SESSION_NAME 32
#define MAX_MEAT_TYPE 32
#define MAX_NOTE_LENGTH 128
#define MAX_NOTES_PER_SESSION 50
#define SAMPLE_INTERVAL 120000  // 2 minutes between temp samples (matches history chart)

// Event types
enum SessionEventType : uint8_t {
  EVENT_START = 0,
  EVENT_TEMP_CHANGE = 1,
  EVENT_NOTE = 2,
  EVENT_TEMP_SAMPLE = 3,
  EVENT_END = 4
};

// Active session state (kept in RAM during recording)
struct ActiveSession {
  uint32_t id;
  char name[MAX_SESSION_NAME];
  char meatType[MAX_MEAT_TYPE];
  int initialSetTemp;
  int currentSetTemp;
  unsigned long startMillis;
  unsigned long lastSampleTime;
  uint32_t startTimestamp;  // Unix timestamp from browser
  bool isRecording;
};

class SessionManager {
public:
  SessionManager();

  // Initialization
  bool begin();

  // Session lifecycle
  bool startSession(const char* name, const char* meatType, int setTemp, uint32_t timestamp);
  bool endSession();
  bool isRecording();
  uint32_t getActiveSessionId();
  String getActiveSessionName();

  // During recording
  void recordTempChange(int newSetTemp);
  void recordTempSample(int smokerTemp, int foodTemp);
  bool addNote(const char* noteText);

  // Session management
  String getSessionList();              // Returns JSON array of sessions
  String getSessionDetails(uint32_t id); // Returns full session JSON
  bool deleteSession(uint32_t id);

  // Reload functionality
  int getSessionSetTemp(uint32_t id);   // Get initial set temp for reload

  // Maintenance
  void tick(int smokerTemp, int foodTemp);  // Call from loop() to handle periodic tasks

private:
  ActiveSession activeSession;

  // File operations
  bool createSessionsDir();
  bool ensureIndexExists();
  bool appendEventToSession(uint32_t sessionId, JsonObject& event);
  bool appendNoteToSession(uint32_t sessionId, const char* noteText, unsigned long timestamp);
  bool updateIndexEntry(uint32_t sessionId, bool active, unsigned long endTime, unsigned long duration);

  // Helpers
  uint32_t generateSessionId();
  String getSessionFilePath(uint32_t id);
  unsigned long getElapsedSeconds();
};

extern SessionManager sessionManager;

#endif

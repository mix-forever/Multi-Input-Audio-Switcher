// ============================================================
// GLOBALS - deklaracje zmiennych globalnych i forward declarations
// ============================================================
#pragma once

#include <Arduino.h>
#include <set>
#include <WiFi.h>
#include <Preferences.h>
#include <ESPAsyncWebServer.h>
#include <AsyncWebSocket.h>
#include <AsyncMqttClient.h>
#include <TFT_eSPI.h>
#include <IRrecv.h>
#include "PT2314.h"
#include "config.h"

// ============================================================
// Obiekty globalne
// ============================================================
extern TFT_eSPI tft;
extern TFT_eSprite audioSprite;
extern IRrecv irrecv;
extern PT2314 audio;
extern AsyncWebServer server;
extern AsyncWebSocket ws;
extern AsyncMqttClient mqttClient;
extern Preferences prefs;

// ============================================================
// Stan aplikacji
// ============================================================
extern uint8_t       currentInput;
extern unsigned long lastIR;
extern uint32_t      lastIRCode;
extern unsigned long lastIRTime;
extern bool          irRepeating;
extern uint32_t      irCodes[9];
extern bool          learningIR;
extern String        learningKey;
extern unsigned long learnStart;

extern String wifiSSID;
extern String wifiPass;
extern String localIP;
extern bool   staConnected;

extern uint8_t brightness;
extern bool    dispInvert;
extern bool    dispRotate;

extern String   mqttHost;
extern uint16_t mqttPort;
extern String   mqttUser;
extern String   mqttPass;
extern String   mqttTopic;
extern bool     mqttEnabled;
extern bool     mqttConnected;
extern bool     mqttSetupDone;
extern unsigned long mqttLastReconnect;
extern String        mqttPayloadBuf;
extern char          mqttFragTopic[128];
extern char          mqttClientIdBuf[32];
extern char          mqttLwtTopicBuf[64];

extern String        adminPass;
extern std::set<uint32_t> wsAuthorized;

extern bool otaInProgress;
extern bool otaRestartPending;
extern unsigned long otaRestartAt;
extern bool otaMutePending;
extern bool otaDrawPercentPending;
extern int  otaDrawPercentValue;
extern bool otaOverlayReset;

extern bool restartRequested;
extern unsigned long restartAtMs;

extern unsigned long lastHeapLogMs;

extern bool    wifiTestInProgress;
extern String  wifiTestSSID;
extern String  wifiTestPass;
extern uint32_t wifiTestClientId;
extern int     wifiTestTries;
extern unsigned long wifiTestNextMs;

extern bool relayEnabled[4];

extern uint8_t audioVolume;
extern int8_t  audioBass;
extern int8_t  audioTreble;

extern bool          audioOverlayActive;
extern bool          audioOverlayDrawn;
extern unsigned long audioOverlayStart;
extern uint8_t       audioFocus;

extern bool          audioSettingsDirty;
extern unsigned long audioSettingsDirtyAt;

// ============================================================
// Stałe globalne (definicje w globals.cpp)
// ============================================================
extern const char* INPUT_NAMES[];
extern const uint16_t* INPUT_ICONS[];
extern const uint8_t RELAY_PINS[4];

// ============================================================
// Helpery
// ============================================================
int8_t   normalizeToneDb(int value);
uint8_t  normalizeInputIndex(int value);
uint8_t  normalizeVolume(int value);
uint8_t  normalizeBrightness(int value);
uint16_t normalizeMqttPort(int value);
String   normalizeMqttTopic(String value);
String   jsonEscape(const String& input);

bool extractJsonString(const String& json, const char* key, String& out);
bool extractJsonInt(const String& json, const char* key, int& out);
bool extractJsonUInt(const String& json, const char* key, uint32_t& out);
bool extractJsonBoolish(const String& json, const char* key, bool& out);
bool extractWSCommand(const String& json, String& out);

// ============================================================
// Forward declarations - NVS / Storage
// ============================================================
void loadPrefs();
void saveLastInput(uint8_t input);
void saveBrightness(uint8_t val);
void saveDisplaySettings();
void saveIRCodes();
void saveWifiCreds(const String& ssid, const String& pass);
void saveMqttSettings();
void saveRelayEnabled();
void saveAudioSettings();
void saveAdminPass(const String& pass);
void markAudioSettingsDirty();
void flushAudioSettingsIfDirty(bool force = false);

// ============================================================
// Forward declarations - Display
// ============================================================
void applyDisplaySettings();
void drawUI();
void updateInput(uint8_t oldInput, uint8_t newInput);
void showWifiOverlay(const String& title, const String& subtitle, uint16_t color);
void showIROverlay(const String& msg);
void clearOverlay();
void showOTAOverlay(int percent, bool reset = false);
void showAudioOverlay();
void resetAudioOverlay();
void drawSplash();
void setBrightness(uint8_t val);

// ============================================================
// Forward declarations - Audio / Relay
// ============================================================
void initAudio(uint8_t input);
void adjustAudio(int delta);
void changeFocus(int dir);
void switchInput(uint8_t newInput);
void applyRelays(uint8_t input);

// ============================================================
// Forward declarations - IR
// ============================================================
void handleIR(uint32_t irCode);

// ============================================================
// Forward declarations - Network / Web
// ============================================================
void setupWifi();
void setupServer();
String buildStatusJSON();
void notifyAll(const String& msg);
void notifyAudioChanged();
void sendWSError(AsyncWebSocketClient* client, const char* code);
void handleWSMessage(AsyncWebSocketClient* client, const String& data);
void setupOTA();

// ============================================================
// Forward declarations - Display extras
// ============================================================
void showSplash();

// ============================================================
// Forward declarations - MQTT
// ============================================================
void onMqttConnect(bool sessionPresent);
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
void onMqttMessage(char* topic, char* payload,
                   AsyncMqttClientMessageProperties properties,
                   size_t len, size_t index, size_t total);
void setupMqtt();
void mqttReconnectIfNeeded();
void mqttPublishInput(uint8_t input);
void mqttPublishStatus(const char* status);
void mqttPublishAudio();

// ============================================================
// Log buffer (ring buffer dla logów systemowych)
// ============================================================
#define LOG_BUFFER_SIZE  60
#define LOG_MAX_LINE_LEN 140

struct LogEntry {
    unsigned long ms;
    char msg[LOG_MAX_LINE_LEN];
};

extern LogEntry logBuffer[LOG_BUFFER_SIZE];
extern uint8_t  logHead;
extern uint8_t  logCount;

void logMsg(const char* fmt, ...);
void logClear();
String logToText();

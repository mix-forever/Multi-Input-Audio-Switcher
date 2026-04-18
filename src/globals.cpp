// ============================================================
// GLOBALS - definicje zmiennych i helperów
// ============================================================
#include "globals.h"
#include "icons.h"
#include "logo.h"

const char* INPUT_NAMES[] = {"Input1", "Input2", "Input3", "Input4"};
const uint16_t* INPUT_ICONS[] = {icon_input1, icon_input2, icon_input3, icon_input4};
const uint8_t RELAY_PINS[4] = {RELAY_1, RELAY_2, RELAY_3, RELAY_4};

// Obiekty globalne
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite audioSprite = TFT_eSprite(&tft);
IRrecv irrecv(IR_PIN, 300, 15, true);
PT2314 audio;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncMqttClient mqttClient;
Preferences prefs;

// Stan aplikacji
uint8_t       currentInput  = 0;
unsigned long lastIR        = 0;
uint32_t      lastIRCode    = 0;
unsigned long lastIRTime    = 0;
bool          irRepeating   = false;
uint32_t      irCodes[9]    = {0, 0, 0, 0, 0, 0, 0, 0, 0};
bool          learningIR    = false;
String        learningKey   = "";
unsigned long learnStart    = 0;

String wifiSSID     = "";
String wifiPass     = "";
String localIP      = "";
bool   staConnected = false;

uint8_t brightness = BL_DEFAULT;
bool    dispInvert = false;
bool    dispRotate = false;

String   mqttHost    = "";
uint16_t mqttPort    = 1883;
String   mqttUser    = "";
String   mqttPass    = "";
String   mqttTopic   = MQTT_TOPIC_DEFAULT;
bool     mqttEnabled = false;
bool     mqttConnected = false;
bool     mqttSetupDone = false;
unsigned long mqttLastReconnect = 0;
String        mqttPayloadBuf;
char          mqttFragTopic[128] = {0};
char          mqttClientIdBuf[32] = {0};
char          mqttLwtTopicBuf[64] = {0};

String        adminPass = "";
std::set<uint32_t> wsAuthorized;

bool otaInProgress = false;
bool otaRestartPending = false;
unsigned long otaRestartAt = 0;
bool otaMutePending = false;
bool otaDrawPercentPending = false;
int  otaDrawPercentValue = 0;
bool otaOverlayReset = false;

bool restartRequested = false;
unsigned long restartAtMs = 0;

unsigned long lastHeapLogMs = 0;

bool    wifiTestInProgress = false;
String  wifiTestSSID = "";
String  wifiTestPass = "";
uint32_t wifiTestClientId = 0;
int     wifiTestTries = 0;
unsigned long wifiTestNextMs = 0;

bool relayEnabled[4] = {false, false, false, false};

uint8_t audioVolume = 56;
int8_t  audioBass   = 0;
int8_t  audioTreble = 0;

bool          audioOverlayActive = false;
bool          audioOverlayDrawn  = false;
unsigned long audioOverlayStart  = 0;
uint8_t       audioFocus         = 0;

bool          audioSettingsDirty = false;
unsigned long audioSettingsDirtyAt = 0;

// ============================================================
// Log buffer (ring buffer)
// ============================================================
#include <stdarg.h>

LogEntry logBuffer[LOG_BUFFER_SIZE];
uint8_t  logHead  = 0;
uint8_t  logCount = 0;

void logMsg(const char* fmt, ...) {
    char buf[LOG_MAX_LINE_LEN];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    buf[sizeof(buf) - 1] = '\0';

    // Zapisz do ring buffer
    uint8_t idx = logCount < LOG_BUFFER_SIZE ? logCount : logHead;
    logBuffer[idx].ms = millis();
    strncpy(logBuffer[idx].msg, buf, sizeof(logBuffer[idx].msg) - 1);
    logBuffer[idx].msg[sizeof(logBuffer[idx].msg) - 1] = '\0';

    if(logCount < LOG_BUFFER_SIZE) {
        logCount++;
        logHead = logCount;
    } else {
        logHead = (logHead + 1) % LOG_BUFFER_SIZE;
    }

    // Drukuj też na Serial
    Serial.println(buf);
}

void logClear() {
    logHead  = 0;
    logCount = 0;
    for(int i = 0; i < LOG_BUFFER_SIZE; i++) {
        logBuffer[i].ms = 0;
        logBuffer[i].msg[0] = '\0';
    }
}

String logToText() {
    String text;
    if(logCount > 0) {
        uint8_t start = (logCount < LOG_BUFFER_SIZE) ? 0 : logHead;
        for(uint8_t i = 0; i < logCount; i++) {
            uint8_t idx = (start + i) % LOG_BUFFER_SIZE;
            // Format: [MM:SS.mmm] message
            unsigned long ms = logBuffer[idx].ms;
            unsigned long totalSec = ms / 1000;
            unsigned int minutes = totalSec / 60;
            unsigned int seconds = totalSec % 60;
            unsigned int millis = ms % 1000;
            
            char timeBuf[16];
            snprintf(timeBuf, sizeof(timeBuf), "[%02u:%02u.%03u] ", minutes, seconds, millis);
            
            text += timeBuf;
            text += logBuffer[idx].msg;
            text += "\n";
        }
    }
    return text;
}

// ============================================================
// Helpery
// ============================================================
int8_t normalizeToneDb(int value) {
    if(value < -14) value = -14;
    if(value > 14) value = 14;
    if(value % 2 != 0) value += (value > 0) ? 1 : -1;
    return (int8_t)value;
}

uint8_t normalizeInputIndex(int value) {
    if(value < 0 || value > 3) return 0;
    return (uint8_t)value;
}

uint8_t normalizeVolume(int value) {
    if(value < 0) value = 0;
    if(value > 56) value = 56;
    return (uint8_t)value;
}

uint8_t normalizeBrightness(int value) {
    if(value < 0) value = 0;
    if(value > 255) value = 255;
    return (uint8_t)value;
}

uint16_t normalizeMqttPort(int value) {
    if(value <= 0 || value > 65535) return 1883;
    return (uint16_t)value;
}

String normalizeMqttTopic(String value) {
    value.trim();
    if(value.length() == 0) return String(MQTT_TOPIC_DEFAULT);
    for(size_t i = 0; i < value.length(); i++) {
        char c = value[i];
        if(c == ' ') value[i] = '_';
        else if(c == '#' || c == '+' || c == '$') value[i] = '_';
        else if(c < 32 || c > 126) value[i] = '_';
    }
    while(value.length() > 0 && value[0] == '/') value.remove(0, 1);
    while(value.length() > 0 && value[value.length() - 1] == '/') value.remove(value.length() - 1);
    if(value.length() == 0) return String(MQTT_TOPIC_DEFAULT);
    return value;
}

String jsonEscape(const String& input) {
    String out;
    out.reserve(input.length() + 8);
    for(size_t i = 0; i < input.length(); i++) {
        char c = input[i];
        switch(c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\b': out += "\\b";  break;
            case '\f': out += "\\f";  break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default:
                if((uint8_t)c < 0x20) out += ' ';
                else out += c;
                break;
        }
    }
    return out;
}

bool extractJsonString(const String& json, const char* key, String& out) {
    String search = "\"" + String(key) + "\":\"";
    int idx = json.indexOf(search);
    if(idx < 0) return false;
    int pos = idx + search.length();
    out = "";
    while(pos < (int)json.length()) {
        char c = json[pos++];
        if(c == '"') return true;
        if(c == '\\' && pos < (int)json.length()) {
            char esc = json[pos++];
            switch(esc) {
                case '"':  out += '"';  break;
                case '\\': out += '\\'; break;
                case '/':  out += '/';  break;
                case 'b':  out += '\b'; break;
                case 'f':  out += '\f'; break;
                case 'n':  out += '\n'; break;
                case 'r':  out += '\r'; break;
                case 't':  out += '\t'; break;
                default:   out += esc;  break;
            }
        } else {
            out += c;
        }
    }
    out = "";
    return false;
}

bool extractJsonInt(const String& json, const char* key, int& out) {
    String search = "\"" + String(key) + "\":";
    int idx = json.indexOf(search);
    if(idx < 0) return false;
    String value = json.substring(idx + search.length());
    value.trim();
    out = value.toInt();
    return true;
}

bool extractJsonUInt(const String& json, const char* key, uint32_t& out) {
    String search = "\"" + String(key) + "\":";
    int idx = json.indexOf(search);
    if(idx < 0) return false;
    String value = json.substring(idx + search.length());
    value.trim();
    out = strtoul(value.c_str(), nullptr, 10);
    return true;
}

bool extractJsonBoolish(const String& json, const char* key, bool& out) {
    String search = "\"" + String(key) + "\":";
    int idx = json.indexOf(search);
    if(idx < 0) return false;
    String value = json.substring(idx + search.length(), idx + search.length() + 8);
    value.trim();
    out = value.startsWith("true") || value.startsWith("1") || value.startsWith("\"true\"");
    return true;
}

bool extractWSCommand(const String& json, String& out) {
    if(extractJsonString(json, "cmd", out)) return true;
    if(extractJsonString(json, "type", out)) return true;
    return false;
}

// ============================================================
// AUDIO SWITCHER v4.0
// ESP32-WROOM-32 + PT2314E + ST7789 + IR + WiFi + WS + MQTT + OTA
// ============================================================

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <WiFi.h>
#include <Preferences.h>
#include <ESPAsyncWebServer.h>
#include <AsyncWebSocket.h>
#include <AsyncMqttClient.h>
#include <Update.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRutils.h>
#include "PT2314.h"
#include "icons.h"
#include "logo.h"
#include "web.h"

// ============================================================
// PINS
// ============================================================
#define TFT_CS    5
#define TFT_DC   17
#define TFT_RST  16
#define TFT_MOSI 23
#define TFT_SCLK 18
#define PT_SDA   21
#define PT_SCL   22
#define IR_PIN   19
#define TFT_BL   15
#define RELAY_BT  4   // SSR high-level trigger - BT power

// PWM backlight
#define BL_CHANNEL   0
#define BL_FREQ   5000
#define BL_RES       8
#define BL_DEFAULT 200

// ============================================================
// DISPLAY
// ============================================================
#define SCREEN_W 284
#define SCREEN_H  76

// ============================================================
// WiFi AP defaults
// ============================================================
#define AP_SSID  "AudioSwitcher"
#define AP_PASS  "audio1234"
#define AP_IP    "192.168.4.1"

// ============================================================
// IR
// ============================================================
#define IR_DEBOUNCE      300
#define IR_LEARN_TIMEOUT 10000

// ============================================================
// MQTT defaults
// ============================================================
#define MQTT_RECONNECT_DELAY 5000
#define MQTT_TOPIC_DEFAULT   "audio_switcher"

// ============================================================
// KOLORY
// ============================================================
#define COLOR_BG       0x0000
#define COLOR_ACTIVE   0x07E0
#define COLOR_INACTIVE 0x0000
#define COLOR_BORDER   0x4A69
#define COLOR_WHITE    0xFFFF
#define COLOR_GRAY     0x8410
#define COLOR_GREEN    0x07E0
#define COLOR_RED      0xF800

// ============================================================
// LAYOUT kafelków
// ============================================================
#define TILE_MARGIN  5
#define TILE_H      (SCREEN_H - TILE_MARGIN * 2)
#define TILE_W      ((SCREEN_W - TILE_MARGIN * 5) / 4)
#define TILE_RADIUS  6

// ============================================================
// NAZWY WEJŚĆ
// ============================================================
const char* INPUT_NAMES[] = {"TV", "BT", "NET", "RES"};
const uint16_t* INPUT_ICONS[] = {icon_tv, icon_bt, icon_radio, icon_other};

// ============================================================
// GLOBALS
// ============================================================
Adafruit_ST7789  tft    = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
IRrecv           irrecv(IR_PIN);
PT2314           audio;
AsyncWebServer   server(80);
AsyncWebSocket   ws("/ws");
AsyncMqttClient  mqttClient;
Preferences      prefs;

uint8_t       currentInput  = 0;
unsigned long lastIR        = 0;

// IR kody (ładowane z NVS)
uint32_t irCodes[5] = {0, 0, 0, 0, 0};
// 0=source, 1=input1..4=input4

// IR learning state
bool          learningIR  = false;
String        learningKey = "";
unsigned long learnStart  = 0;

// WiFi state
String wifiSSID     = "";
String wifiPass     = "";
String localIP      = "";
bool   staConnected = false;

// Backlight
uint8_t brightness = BL_DEFAULT;

// Display settings
bool dispInvert = false;
bool dispRotate = false;

// MQTT state
String        mqttHost    = "";
uint16_t      mqttPort    = 1883;
String        mqttUser    = "";
String        mqttPass    = "";
String        mqttTopic   = MQTT_TOPIC_DEFAULT;
bool          mqttEnabled = false;
bool          mqttConnected = false;
unsigned long mqttLastReconnect = 0;

// OTA state
bool otaInProgress = false;

// ============================================================
// NVS
// ============================================================
void loadPrefs() {
    prefs.begin("switcher", false);

    wifiSSID     = prefs.getString("wifi_ssid",  "");
    wifiPass     = prefs.getString("wifi_pass",  "");
    currentInput = prefs.getUChar("last_input",  0);
    brightness   = prefs.getUChar("brightness",  BL_DEFAULT);
    dispInvert   = prefs.getBool("disp_invert",  false);
    dispRotate   = prefs.getBool("disp_rotate",  false);

    irCodes[0]   = prefs.getUInt("ir_source",  0);
    irCodes[1]   = prefs.getUInt("ir_input1",  0);
    irCodes[2]   = prefs.getUInt("ir_input2",  0);
    irCodes[3]   = prefs.getUInt("ir_input3",  0);
    irCodes[4]   = prefs.getUInt("ir_input4",  0);

    mqttHost     = prefs.getString("mqtt_host",  "");
    mqttPort     = prefs.getUShort("mqtt_port",  1883);
    mqttUser     = prefs.getString("mqtt_user",  "");
    mqttPass     = prefs.getString("mqtt_pass",  "");
    mqttTopic    = prefs.getString("mqtt_topic", MQTT_TOPIC_DEFAULT);
    mqttEnabled  = prefs.getBool("mqtt_en",      false);

    prefs.end();
    Serial.println("NVS: Prefs loaded");
}

void saveLastInput(uint8_t input) {
    prefs.begin("switcher", false);
    prefs.putUChar("last_input", input);
    prefs.end();
}

void saveBrightness(uint8_t val) {
    prefs.begin("switcher", false);
    prefs.putUChar("brightness", val);
    prefs.end();
}

void saveDisplaySettings() {
    prefs.begin("switcher", false);
    prefs.putBool("disp_invert", dispInvert);
    prefs.putBool("disp_rotate", dispRotate);
    prefs.end();
}

void saveIRCodes() {
    prefs.begin("switcher", false);
    prefs.putUInt("ir_source", irCodes[0]);
    prefs.putUInt("ir_input1", irCodes[1]);
    prefs.putUInt("ir_input2", irCodes[2]);
    prefs.putUInt("ir_input3", irCodes[3]);
    prefs.putUInt("ir_input4", irCodes[4]);
    prefs.end();
    Serial.println("NVS: IR codes saved");
}

void saveWifiCreds(const String& ssid, const String& pass) {
    prefs.begin("switcher", false);
    prefs.putString("wifi_ssid", ssid);
    prefs.putString("wifi_pass", pass);
    prefs.end();
    Serial.println("NVS: WiFi saved");
}

void saveMqttSettings() {
    prefs.begin("switcher", false);
    prefs.putString("mqtt_host",  mqttHost);
    prefs.putUShort("mqtt_port",  mqttPort);
    prefs.putString("mqtt_user",  mqttUser);
    prefs.putString("mqtt_pass",  mqttPass);
    prefs.putString("mqtt_topic", mqttTopic);
    prefs.putBool("mqtt_en",      mqttEnabled);
    prefs.end();
    Serial.println("NVS: MQTT saved");
}

// ============================================================
// DISPLAY - forward declarations
// ============================================================
void drawUI();
void notifyAll(const String& msg);

// ============================================================
// DISPLAY SETTINGS
// ============================================================
void applyDisplaySettings() {
    tft.setRotation(dispRotate ? 3 : 1);
    tft.invertDisplay(dispInvert);
    drawUI();
    Serial.printf("Display: rotate=%d invert=%d\n", dispRotate, dispInvert);
}

// ============================================================
// DISPLAY - ikona z PROGMEM
// ============================================================
void drawIcon(int16_t x, int16_t y, const uint16_t* icon, uint16_t bg) {
    uint16_t buf[ICON_W];
    for(int row = 0; row < ICON_H; row++) {
        for(int col = 0; col < ICON_W; col++) {
            uint16_t px = pgm_read_word(&icon[row * ICON_W + col]);
            buf[col] = (px == 0x0000) ? bg : px;
        }
        tft.drawRGBBitmap(x, y + row, buf, ICON_W, 1);
    }
}

// ============================================================
// DISPLAY - kafelek
// ============================================================
void drawTile(uint8_t index, bool active) {
    uint16_t x = TILE_MARGIN + index * (TILE_W + TILE_MARGIN);
    uint16_t y = TILE_MARGIN;

    tft.fillRoundRect(x, y, TILE_W, TILE_H, TILE_RADIUS, COLOR_INACTIVE);

    if(active) {
        tft.drawRoundRect(x,     y,     TILE_W,     TILE_H,     TILE_RADIUS,     COLOR_ACTIVE);
        tft.drawRoundRect(x + 1, y + 1, TILE_W - 2, TILE_H - 2, TILE_RADIUS - 1, COLOR_ACTIVE);
        tft.drawRoundRect(x + 2, y + 2, TILE_W - 4, TILE_H - 4, TILE_RADIUS - 2, COLOR_ACTIVE);
    } else {
        tft.drawRoundRect(x, y, TILE_W, TILE_H, TILE_RADIUS, COLOR_BORDER);
    }

    int16_t iconX = x + (TILE_W - ICON_W) / 2;
    int16_t iconY = y + (TILE_H - ICON_H) / 2;
    drawIcon(iconX, iconY, INPUT_ICONS[index], COLOR_INACTIVE);
}

void drawUI() {
    tft.fillScreen(COLOR_BG);
    for(uint8_t i = 0; i < 4; i++) drawTile(i, i == currentInput);
}

void updateInput(uint8_t oldInput, uint8_t newInput) {
    drawTile(oldInput, false);
    drawTile(newInput, true);
}

// ============================================================
// DISPLAY - overlaye
// ============================================================
void showWifiOverlay(const String& line1, const String& line2, uint16_t color) {
    tft.fillRect(0, 0, SCREEN_W, 14, COLOR_BG);
    tft.setTextSize(1);
    tft.setTextColor(color);
    tft.setCursor(4, 3);
    tft.print(line1);
    if(line2.length() > 0) {
        tft.setCursor(SCREEN_W - line2.length() * 6 - 4, 3);
        tft.print(line2);
    }
}

void showIROverlay(const String& msg) {
    tft.fillRect(0, 0, SCREEN_W, 14, COLOR_BG);
    tft.setTextSize(1);
    tft.setTextColor(COLOR_GREEN);
    tft.setCursor(4, 3);
    tft.print(msg);
}

void showOTAOverlay(int percent) {
    tft.fillRect(0, 0, SCREEN_W, 14, COLOR_BG);
    tft.setTextSize(1);
    tft.setTextColor(COLOR_GREEN);
    tft.setCursor(4, 3);
    tft.print("OTA: " + String(percent) + "%");
    // Pasek postępu
    int barW = (SCREEN_W - 80) * percent / 100;
    tft.fillRect(60, 4, barW, 6, COLOR_GREEN);
}

void clearOverlay() {
    tft.fillRect(0, 0, SCREEN_W, 14, COLOR_BG);
    for(uint8_t i = 0; i < 4; i++) {
        uint16_t x = TILE_MARGIN + i * (TILE_W + TILE_MARGIN);
        if(i == currentInput) {
            tft.drawRoundRect(x,     TILE_MARGIN,     TILE_W,     TILE_H,     TILE_RADIUS,     COLOR_ACTIVE);
            tft.drawRoundRect(x + 1, TILE_MARGIN + 1, TILE_W - 2, TILE_H - 2, TILE_RADIUS - 1, COLOR_ACTIVE);
            tft.drawRoundRect(x + 2, TILE_MARGIN + 2, TILE_W - 4, TILE_H - 4, TILE_RADIUS - 2, COLOR_ACTIVE);
        } else {
            tft.drawRoundRect(x, TILE_MARGIN, TILE_W, TILE_H, TILE_RADIUS, COLOR_BORDER);
        }
    }
}

// ============================================================
// DISPLAY - splash
// ============================================================
void showSplash() {
    tft.startWrite();
    tft.setAddrWindow(0, 0, LOGO_W, LOGO_H);
    const uint16_t CHUNK = 64;
    uint16_t buf[CHUNK];
    uint32_t total = LOGO_W * LOGO_H;
    for(uint32_t i = 0; i < total; i += CHUNK) {
        uint16_t count = min((uint32_t)CHUNK, total - i);
        for(uint16_t j = 0; j < count; j++)
            buf[j] = pgm_read_word(&bootlogo[i + j]);
        tft.writePixels(buf, count);
    }
    tft.endWrite();
    delay(2500);
}

// ============================================================
// BACKLIGHT
// ============================================================
void setBrightness(uint8_t val) {
    brightness = val;
    ledcWrite(BL_CHANNEL, val);
}

// ============================================================
// AUDIO
// ============================================================
void initAudio(uint8_t input) {
    audio.setVolume(0);
    audio.setBass(0);
    audio.setTreble(0);
    audio.setAttL(0);
    audio.setAttR(0);
    audio.setSwitch(input, 0, 0);
}

// ============================================================
// MQTT - publish
// ============================================================
void mqttPublishInput(uint8_t input) {
    if(!mqttConnected) return;
    String topic = mqttTopic + "/state";
    String payload = String(input + 1);  // 1-4
    mqttClient.publish(topic.c_str(), 0, true, payload.c_str());
    Serial.printf("MQTT pub: %s = %s\n", topic.c_str(), payload.c_str());
}

void mqttPublishStatus(const char* status) {
    if(!mqttConnected) return;
    String topic = mqttTopic + "/status";
    mqttClient.publish(topic.c_str(), 0, true, status);
}

// ============================================================
// INPUT SWITCH
// ============================================================
void switchInput(uint8_t newInput) {
    if(newInput > 3) return;
    uint8_t old = currentInput;
    currentInput = newInput;
    audio.setSwitch(currentInput, 0, 0);
    updateInput(old, currentInput);
    saveLastInput(currentInput);
    digitalWrite(RELAY_BT, currentInput == 1 ? HIGH : LOW);
    mqttPublishInput(currentInput);
    Serial.printf("Input: %d [%s] BT relay: %s\n",
        currentInput + 1, INPUT_NAMES[currentInput],
        currentInput == 1 ? "ON" : "OFF");
}

// ============================================================
// WEBSOCKET - status JSON
// ============================================================
String buildStatusJSON() {
    String json = "{\"type\":\"status\"";
    json += ",\"input\":"        + String(currentInput);
    json += ",\"ip\":\""         + (staConnected ? localIP : String(AP_IP)) + "\"";
    json += ",\"wifi_ssid\":\""  + wifiSSID + "\"";
    json += ",\"brightness\":"   + String(brightness);
    json += ",\"disp_invert\":"  + String(dispInvert ? "true" : "false");
    json += ",\"disp_rotate\":"  + String(dispRotate ? "true" : "false");
    json += ",\"mqtt_host\":\""  + mqttHost + "\"";
    json += ",\"mqtt_port\":"    + String(mqttPort);
    json += ",\"mqtt_user\":\""  + mqttUser + "\"";
    json += ",\"mqtt_topic\":\"" + mqttTopic + "\"";
    json += ",\"mqtt_enabled\":" + String(mqttEnabled ? "true" : "false");
    json += ",\"mqtt_connected\":" + String(mqttConnected ? "true" : "false");
    json += ",\"ir\":{";
    json += "\"source\":"  + String(irCodes[0]);
    json += ",\"input1\":" + String(irCodes[1]);
    json += ",\"input2\":" + String(irCodes[2]);
    json += ",\"input3\":" + String(irCodes[3]);
    json += ",\"input4\":" + String(irCodes[4]);
    json += "}}";
    return json;
}

void notifyAll(const String& msg) {
    ws.textAll(msg);
}

// ============================================================
// WEBSOCKET - obsługa wiadomości
// ============================================================
void handleWSMessage(AsyncWebSocketClient* client, const String& data) {
    Serial.println("WS: " + data);

    if(data.indexOf("\"get_status\"") >= 0) {
        client->text(buildStatusJSON());
    }
    else if(data.indexOf("\"set_input\"") >= 0) {
        int idx = data.indexOf("\"input\":");
        if(idx >= 0) {
            switchInput(data.substring(idx + 8).toInt());
            notifyAll("{\"type\":\"input_changed\",\"input\":" + String(currentInput) + "}");
        }
    }
    else if(data.indexOf("\"learn_ir\"") >= 0) {
        int idx = data.indexOf("\"key\":\"");
        if(idx >= 0) {
            int s = idx + 7, e = data.indexOf("\"", s);
            learningKey = data.substring(s, e);
            learningIR  = true;
            learnStart  = millis();
            showIROverlay("IR: Press button for " + learningKey + "...");
        }
    }
    else if(data.indexOf("\"save_ir\"") >= 0) {
        const char* keys[] = {"source","input1","input2","input3","input4"};
        for(int i = 0; i < 5; i++) {
            String search = "\"" + String(keys[i]) + "\":";
            int idx = data.indexOf(search);
            if(idx >= 0)
                irCodes[i] = strtoul(data.substring(idx + search.length()).c_str(), nullptr, 10);
        }
        saveIRCodes();
        notifyAll("{\"type\":\"ir_saved\"}");
    }
    else if(data.indexOf("\"set_brightness\"") >= 0) {
        int idx = data.indexOf("\"value\":");
        if(idx >= 0) {
            uint8_t val = (uint8_t)data.substring(idx + 8).toInt();
            setBrightness(val);
            saveBrightness(val);
        }
    }
    else if(data.indexOf("\"set_invert\"") >= 0) {
        int idx = data.indexOf("\"value\":");
        if(idx >= 0) {
            dispInvert = data.substring(idx + 8).startsWith("true");
            tft.invertDisplay(dispInvert);
            saveDisplaySettings();
        }
    }
    else if(data.indexOf("\"set_rotate\"") >= 0) {
        int idx = data.indexOf("\"value\":");
        if(idx >= 0) {
            dispRotate = data.substring(idx + 8).startsWith("true");
            applyDisplaySettings();
            saveDisplaySettings();
        }
    }
    else if(data.indexOf("\"save_wifi\"") >= 0) {
        int si = data.indexOf("\"ssid\":\""), pi = data.indexOf("\"pass\":\"");
        if(si >= 0 && pi >= 0) {
            int ss = si+8, se = data.indexOf("\"",ss);
            int ps = pi+8, pe = data.indexOf("\"",ps);
            saveWifiCreds(data.substring(ss,se), data.substring(ps,pe));
            notifyAll("{\"type\":\"wifi_saved\"}");
            delay(1500);
            ESP.restart();
        }
    }
    else if(data.indexOf("\"test_wifi\"") >= 0) {
        int si = data.indexOf("\"ssid\":\""), pi = data.indexOf("\"pass\":\"");
        if(si >= 0 && pi >= 0) {
            int ss = si+8, se = data.indexOf("\"",ss);
            int ps = pi+8, pe = data.indexOf("\"",ps);
            WiFi.begin(data.substring(ss,se).c_str(), data.substring(ps,pe).c_str());
            int tries = 0;
            while(WiFi.status() != WL_CONNECTED && tries < 20) { delay(500); tries++; }
            if(WiFi.status() == WL_CONNECTED)
                client->text("{\"type\":\"wifi_test_ok\",\"ip\":\"" + WiFi.localIP().toString() + "\"}");
            else
                client->text("{\"type\":\"wifi_test_fail\"}");
        }
    }
    else if(data.indexOf("\"save_mqtt\"") >= 0) {
        auto extract = [&](const String& key) -> String {
            String search = "\"" + key + "\":\"";
            int idx = data.indexOf(search);
            if(idx < 0) return "";
            int s = idx + search.length();
            return data.substring(s, data.indexOf("\"", s));
        };
        mqttHost    = extract("host");
        mqttUser    = extract("user");
        mqttPass    = extract("pass");
        mqttTopic   = extract("topic");
        int pi = data.indexOf("\"port\":");
        if(pi >= 0) mqttPort = data.substring(pi + 7).toInt();
        int ei = data.indexOf("\"enabled\":");
        if(ei >= 0) mqttEnabled = data.substring(ei + 10).startsWith("true");
        saveMqttSettings();
        notifyAll("{\"type\":\"mqtt_saved\"}");
        // Reconnect z nowymi ustawieniami
        if(mqttClient.connected()) mqttClient.disconnect();
    }
}

// ============================================================
// WEBSOCKET - event handler
// ============================================================
void onWSEvent(AsyncWebSocket* server, AsyncWebSocketClient* client,
               AwsEventType type, void* arg, uint8_t* data, size_t len) {
    if(type == WS_EVT_CONNECT) {
        Serial.printf("WS: client %u connected\n", client->id());
        client->text(buildStatusJSON());
    }
    else if(type == WS_EVT_DISCONNECT) {
        Serial.printf("WS: client %u disconnected\n", client->id());
    }
    else if(type == WS_EVT_DATA) {
        String msg = "";
        for(size_t i = 0; i < len; i++) msg += (char)data[i];
        handleWSMessage(client, msg);
    }
}

// ============================================================
// MQTT - callbacks
// ============================================================
void onMqttConnect(bool sessionPresent) {
    mqttConnected = true;
    Serial.println("MQTT: Connected");

    // Subscribe na temat /set
    String subTopic = mqttTopic + "/set";
    mqttClient.subscribe(subTopic.c_str(), 0);
    Serial.println("MQTT: Subscribed to " + subTopic);

    // Publish status online + aktualny stan
    mqttPublishStatus("online");
    mqttPublishInput(currentInput);

    // Powiadom web interface
    notifyAll("{\"type\":\"mqtt_connected\"}");
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
    mqttConnected = false;
    Serial.println("MQTT: Disconnected");
    notifyAll("{\"type\":\"mqtt_disconnected\"}");
}

void onMqttMessage(char* topic, char* payload,
                   AsyncMqttClientMessageProperties properties,
                   size_t len, size_t index, size_t total) {
    String msg = "";
    for(size_t i = 0; i < len; i++) msg += (char)payload[i];
    String topicStr = String(topic);

    Serial.printf("MQTT recv: %s = %s\n", topic, msg.c_str());

    // {topic}/set - przełącz wejście
    if(topicStr == mqttTopic + "/set") {
        int input = msg.toInt() - 1;  // 1-4 → 0-3
        if(input >= 0 && input <= 3) {
            switchInput(input);
            notifyAll("{\"type\":\"input_changed\",\"input\":" + String(currentInput) + "}");
        }
    }
}

// ============================================================
// MQTT - setup i reconnect
// ============================================================
void setupMqtt() {
    if(mqttHost.length() == 0 || !mqttEnabled) return;

    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onMessage(onMqttMessage);

    mqttClient.setServer(mqttHost.c_str(), mqttPort);

    if(mqttUser.length() > 0)
        mqttClient.setCredentials(mqttUser.c_str(), mqttPass.c_str());

    // Last Will Testament
    String lwtTopic = mqttTopic + "/status";
    mqttClient.setWill(lwtTopic.c_str(), 0, true, "offline");

    mqttClient.setClientId("AudioSwitcher");

    mqttClient.connect();
    Serial.printf("MQTT: Connecting to %s:%d\n", mqttHost.c_str(), mqttPort);
}

void mqttReconnectIfNeeded() {
    if(!mqttEnabled || mqttHost.length() == 0) return;
    if(mqttConnected) return;
    if(!staConnected) return;

    unsigned long now = millis();
    if(now - mqttLastReconnect < MQTT_RECONNECT_DELAY) return;
    mqttLastReconnect = now;

    Serial.println("MQTT: Reconnecting...");
    mqttClient.connect();
}

// ============================================================
// OTA - setup
// ============================================================
void setupOTA() {
    // Endpoint odbioru pliku .bin
    server.on("/update", HTTP_POST,
        // Odpowiedź po zakończeniu
        [](AsyncWebServerRequest* req) {
            bool success = !Update.hasError();
            String msg = success ? "{\"type\":\"ota_ok\"}" : "{\"type\":\"ota_fail\"}";
            notifyAll(msg);
            req->send(200, "application/json", success ? "{\"ok\":true}" : "{\"ok\":false}");
            if(success) {
                delay(1000);
                ESP.restart();
            }
        },
        // Odbiór danych
        [](AsyncWebServerRequest* req, String filename,
           size_t index, uint8_t* data, size_t len, bool final) {

            if(index == 0) {
                Serial.printf("OTA: Start - %s\n", filename.c_str());
                otaInProgress = true;
                if(!Update.begin(UPDATE_SIZE_UNKNOWN)) {
                    Update.printError(Serial);
                }
            }

            if(Update.write(data, len) != len) {
                Update.printError(Serial);
            }

            // Progress
            if(Update.size() > 0) {
                int percent = (index + len) * 100 / req->contentLength();
                showOTAOverlay(percent);
                notifyAll("{\"type\":\"ota_progress\",\"percent\":" + String(percent) + "}");
            }

            if(final) {
                if(Update.end(true)) {
                    Serial.printf("OTA: Done - %u bytes\n", index + len);
                } else {
                    Update.printError(Serial);
                }
                otaInProgress = false;
            }
        }
    );
}

// ============================================================
// WIFI SETUP
// ============================================================
void setupWifi() {
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(AP_SSID, AP_PASS);
    Serial.println("WiFi: AP started - " + String(AP_SSID));

    if(wifiSSID.length() > 0) {
        Serial.println("WiFi: Connecting to " + wifiSSID);
        showWifiOverlay("WiFi: " + wifiSSID, "", COLOR_GRAY);

        WiFi.begin(wifiSSID.c_str(), wifiPass.c_str());
        int tries = 0;
        while(WiFi.status() != WL_CONNECTED && tries < 20) { delay(500); tries++; }

        if(WiFi.status() == WL_CONNECTED) {
            localIP      = WiFi.localIP().toString();
            staConnected = true;
            Serial.println("WiFi: Connected! IP: " + localIP);
            showWifiOverlay("WiFi: " + localIP, "AP: " + String(AP_IP), COLOR_GREEN);
        } else {
            Serial.println("WiFi: Failed - AP only");
            showWifiOverlay("WiFi: Failed", "AP: " + String(AP_IP), COLOR_RED);
        }
    } else {
        showWifiOverlay("WiFi Setup: " + String(AP_SSID), AP_IP, COLOR_GREEN);
    }
    delay(3000);
    clearOverlay();
}

// ============================================================
// SERVER SETUP
// ============================================================
void setupServer() {
    ws.onEvent(onWSEvent);
    server.addHandler(&ws);
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send_P(200, "text/html", INDEX_HTML);
    });
    setupOTA();
    server.begin();
    Serial.println("HTTP: Server started");
}

// ============================================================
// IR
// ============================================================
void handleIR(uint32_t code) {
    if(learningIR) {
        Serial.printf("IR learned: 0x%08X for %s\n", code, learningKey.c_str());
        if(learningKey == "source")      irCodes[0] = code;
        else if(learningKey == "input1") irCodes[1] = code;
        else if(learningKey == "input2") irCodes[2] = code;
        else if(learningKey == "input3") irCodes[3] = code;
        else if(learningKey == "input4") irCodes[4] = code;
        learningIR = false;
        clearOverlay();
        String msg = "{\"type\":\"ir_received\",\"key\":\"" + learningKey + "\",\"code\":" + String(code) + "}";
        notifyAll(msg);
        return;
    }

    if(code == irCodes[0] && irCodes[0] != 0) {
        switchInput((currentInput + 1) % 4);
        notifyAll("{\"type\":\"input_changed\",\"input\":" + String(currentInput) + "}");
    } else {
        for(int i = 0; i < 4; i++) {
            if(code == irCodes[i+1] && irCodes[i+1] != 0) {
                switchInput(i);
                notifyAll("{\"type\":\"input_changed\",\"input\":" + String(currentInput) + "}");
                break;
            }
        }
    }
}

// ============================================================
// SETUP
// ============================================================
void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n╔═══════════════════════════════════════╗");
    Serial.println("║      AUDIO SWITCHER v4.0              ║");
    Serial.println("╚═══════════════════════════════════════╝\n");

    // Backlight PWM
    ledcSetup(BL_CHANNEL, BL_FREQ, BL_RES);
    ledcAttachPin(TFT_BL, BL_CHANNEL);
    ledcWrite(BL_CHANNEL, BL_DEFAULT);

    // Relay BT - domyślnie wyłączony
    pinMode(RELAY_BT, OUTPUT);
    digitalWrite(RELAY_BT, LOW);

    // Display
    tft.init(SCREEN_H, SCREEN_W);
    tft.setRotation(1);
    tft.invertDisplay(false);
    tft.fillScreen(COLOR_BG);
    showSplash();

    // NVS
    loadPrefs();
    ledcWrite(BL_CHANNEL, brightness);
    tft.setRotation(dispRotate ? 3 : 1);
    tft.invertDisplay(dispInvert);
    digitalWrite(RELAY_BT, currentInput == 1 ? HIGH : LOW);  // Przywróć stan BT

    // PT2314E
    Wire.begin(PT_SDA, PT_SCL);
    Wire.setClock(100000);
    delay(200);
    initAudio(currentInput);

    // IR
    irrecv.enableIRIn();

    // UI
    drawUI();

    // WiFi
    setupWifi();

    // Web server + OTA
    setupServer();

    // MQTT (tylko gdy STA połączony)
    if(staConnected) setupMqtt();

    Serial.println("\n✓ System Ready!");
    Serial.printf("  AP:  http://%s\n", AP_IP);
    if(staConnected) Serial.printf("  STA: http://%s\n", localIP.c_str());
    if(mqttEnabled)  Serial.printf("  MQTT: %s:%d\n", mqttHost.c_str(), mqttPort);
}

// ============================================================
// LOOP
// ============================================================
void loop() {
    // IR learning timeout
    if(learningIR && (millis() - learnStart > IR_LEARN_TIMEOUT)) {
        learningIR = false;
        clearOverlay();
        notifyAll("{\"type\":\"ir_timeout\",\"key\":\"" + learningKey + "\"}");
    }

    // IR receive
    decode_results results;
    if(!otaInProgress && irrecv.decode(&results)) {
        unsigned long now = millis();
        if(now - lastIR > IR_DEBOUNCE) {
            if(results.value != 0xFFFFFFFF && results.value != 0) {
                handleIR(results.value);
                lastIR = now;
            }
        }
        irrecv.resume();
    }

    // MQTT reconnect
    mqttReconnectIfNeeded();

    // WebSocket cleanup
    ws.cleanupClients();

    delay(10);
}

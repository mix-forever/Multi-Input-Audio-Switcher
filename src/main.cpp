// ============================================================
// AUDIO SWITCHER v4.2
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
#include <esp_ota_ops.h>
#include <TFT_eSPI.h>
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

// Relay pins - SSR high-level trigger
#define RELAY_1  32
#define RELAY_2  33
#define RELAY_3  25
#define RELAY_4  26
const uint8_t RELAY_PINS[4] = {RELAY_1, RELAY_2, RELAY_3, RELAY_4};

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
#define COLOR_YELLOW   0xFFE0  // Żółty
#define COLOR_BLUE     0x001F  // Niebieski
#define COLOR_CYAN     0x07FF  // Cyan (jaśniejszy niebieski)
#define COLOR_ORANGE   0xFD20  // Pomarańczowy

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
const char* INPUT_NAMES[] = {"Input1", "Input2", "Input3", "Input4"};
const uint16_t* INPUT_ICONS[] = {icon_input1, icon_input2, icon_input3, icon_input4};

// ============================================================
// GLOBALS
// ============================================================
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite audioSprite = TFT_eSprite(&tft);

IRrecv           irrecv(IR_PIN, 300, 15, true);  // pin, buffer_size, timeout, use_modulation
PT2314           audio;
AsyncWebServer   server(80);
AsyncWebSocket   ws("/ws");
AsyncMqttClient  mqttClient;
Preferences      prefs;

uint8_t       currentInput  = 0;
unsigned long lastIR        = 0;

// IR auto-repeat dla Vol+/Vol-/↑/↓
uint32_t      lastIRCode    = 0;
unsigned long lastIRTime    = 0;
bool          irRepeating   = false;
#define IR_REPEAT_DELAY  500   // Opóźnienie przed startem auto-repeat (ms)
#define IR_REPEAT_RATE   150   // Interwał między powtórzeniami (ms)

// IR kody (ładowane z NVS)
uint32_t irCodes[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
// 0=source, 1-4=input1-4, 5=vol+, 6=vol-, 7=up, 8=down

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
bool          mqttSetupDone = false;  // Track if setupMqtt() has been called once
unsigned long mqttLastReconnect = 0;

// OTA state
bool otaInProgress = false;


// Relay enable - czy dane wejście steruje swoim przekaźnikiem
// relayEnabled[i] = true → Input(i+1) włącza Relay(i+1)
bool relayEnabled[4] = {false, false, false, false};

// Audio settings
uint8_t audioVolume = 0;    // 0-63 (0=max, 63=mute - odwrotna skala PT2314E)
int8_t  audioBass   = 0;    // -14 do +14 dB
int8_t  audioTreble = 0;    // -14 do +14 dB

// Audio overlay state
bool          audioOverlayActive = false;
bool          audioOverlayDrawn  = false;  // Czy struktura została narysowana
unsigned long audioOverlayStart  = 0;
uint8_t       audioFocus         = 0;  // 0=Volume, 1=Bass, 2=Treble
#define AUDIO_OVERLAY_TIMEOUT 2000


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
    irCodes[5]   = prefs.getUInt("ir_volp",  0);
    irCodes[6]   = prefs.getUInt("ir_volm",  0);
    irCodes[7]   = prefs.getUInt("ir_up",    0);
    irCodes[8]   = prefs.getUInt("ir_down",  0);

    mqttHost     = prefs.getString("mqtt_host",  "");
    mqttPort     = prefs.getUShort("mqtt_port",  1883);
    mqttUser     = prefs.getString("mqtt_user",  "");
    mqttPass     = prefs.getString("mqtt_pass",  "");
    mqttTopic    = prefs.getString("mqtt_topic", MQTT_TOPIC_DEFAULT);
    mqttEnabled  = prefs.getBool("mqtt_en",      false);

    relayEnabled[0] = prefs.getBool("relay_en0", false);
    relayEnabled[1] = prefs.getBool("relay_en1", false);
    relayEnabled[2] = prefs.getBool("relay_en2", false);
    relayEnabled[3] = prefs.getBool("relay_en3", false);

    audioVolume = prefs.getUChar("audio_vol",  0);
    audioBass   = prefs.getChar("audio_bass",  0);
    audioTreble = prefs.getChar("audio_treb",  0);

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
    prefs.putUInt("ir_volp",  irCodes[5]);
    prefs.putUInt("ir_volm",  irCodes[6]);
    prefs.putUInt("ir_up",    irCodes[7]);
    prefs.putUInt("ir_down",  irCodes[8]);
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

void saveRelayEnabled() {
    prefs.begin("switcher", false);
    prefs.putBool("relay_en0", relayEnabled[0]);
    prefs.putBool("relay_en1", relayEnabled[1]);
    prefs.putBool("relay_en2", relayEnabled[2]);
    prefs.putBool("relay_en3", relayEnabled[3]);
    prefs.end();
    Serial.println("NVS: Relay config saved");
}


void saveAudioSettings() {
    prefs.begin("switcher", false);
    prefs.putUChar("audio_vol",  audioVolume);
    prefs.putChar("audio_bass",  audioBass);
    prefs.putChar("audio_treb",  audioTreble);
    prefs.end();
    Serial.println("NVS: Audio settings saved");
}

// ============================================================
// DISPLAY - forward declarations
// ============================================================
void drawUI();
void notifyAll(const String& msg);
void setupMqtt();
void mqttPublishAudio();

// ============================================================
// RELAY CONTROL
// ============================================================
void applyRelays(uint8_t input) {
    for(uint8_t i = 0; i < 4; i++) {
        // Relay(i) aktywny tylko gdy: aktywny jest Input(i) ORAZ ten relay jest włączony
        bool state = (i == input) && relayEnabled[i];
        digitalWrite(RELAY_PINS[i], state ? HIGH : LOW);
        Serial.printf("  Relay%d: %s\n", i+1, state ? "ON" : "off");
    }
    Serial.printf("Relays applied for input %d\n", input + 1);
}

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

    static uint16_t buf[ICON_W];

    for(int row = 0; row < ICON_H; row++) {

        for(int col = 0; col < ICON_W; col++) {
            uint16_t px = pgm_read_word(&icon[row * ICON_W + col]);
            buf[col] = (px == 0x0000) ? bg : px;
        }

        tft.pushImage(x, y + row, ICON_W, 1, buf);
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
    static int lastDrawn = -1;
    static int lastPercent = -1;
    
    int barX = 60;
    int barY = 4;
    int barW = SCREEN_W - 80;
    int barH = 10;  // Wyższy pasek
    int fillW = barW * percent / 100;

    if (lastDrawn == -1) {
        // Pierwsze wywołanie: rysuj tło, napis, ramkę
        tft.fillRect(0, 0, SCREEN_W, 16, COLOR_BG);
        tft.setTextSize(1);
        tft.setTextColor(COLOR_GREEN);
        tft.setCursor(4, 4);
        tft.print("OTA: " + String(percent) + "%");
        
        // Tło paska z zaokrąglonymi rogami (jak audio bar)
        tft.fillRoundRect(barX, barY, barW, barH, 4, tft.color565(25,25,25));
        tft.drawRoundRect(barX, barY, barW, barH, 4, COLOR_BORDER);
        tft.fillRect(barX + 2, barY + 2, barW - 4, barH - 4, tft.color565(15,15,15));
        
        lastDrawn = 0;
        lastPercent = -1;
    }
    
    // Aktualizuj napis jeśli procent się zmienił
    if(percent != lastPercent) {
        tft.fillRect(4, 4, 50, 8, COLOR_BG);
        tft.setCursor(4, 4);
        tft.setTextColor(COLOR_GREEN);
        tft.print("OTA: " + String(percent) + "%");
        
        // Czyść poprzednie wypełnienie
        tft.fillRect(barX + 2, barY + 2, barW - 4, barH - 4, tft.color565(15,15,15));
        
        // Gradient + połysk (jak volume bar)
        if(fillW > 4) {
            for(int i = 0; i < fillW - 4; i++) {
                int boost = (i * 40) / (fillW - 4);
                
                uint8_t r = ((COLOR_GREEN >> 11) & 0x1F) << 3;
                uint8_t g = ((COLOR_GREEN >> 5) & 0x3F) << 2;
                uint8_t b = (COLOR_GREEN & 0x1F) << 3;
                
                r = min(255, r + boost);
                g = min(255, g + boost);
                b = min(255, b + boost);
                
                uint16_t grad = tft.color565(r, g, b);
                
                tft.drawFastVLine(barX + 2 + i, barY + 2, barH - 4, grad);
            }
            
            // Połysk na górze
            tft.drawFastHLine(barX + 3, barY + 2, fillW - 6, tft.color565(255,255,255));
        }
        
        lastPercent = percent;
    }
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
// AUDIO GRADIENT COLORS
// ============================================================
uint16_t getVolumeColor(uint8_t volume) {
    // Volume: 0=max głośno, 63=mute cicho
    // Inwersja: 0-63 → 63-0
    int level = 63 - volume;  // 0-63
    int percent = level * 100 / 63;  // 0-100%
    
    // Odwrotny gradient: pasek rośnie = głośniej
    if(percent <= 30) return COLOR_GREEN;    // 0-30% = zielony (cicho, bezpieczne)
    if(percent <= 70) return COLOR_YELLOW;   // 31-70% = żółty (normalnie)
    return COLOR_RED;                        // 71-100% = czerwony (głośno, niebezpieczne)
}

uint16_t getToneColor(int8_t value) {
    // Bass/Treble: -14 do +14
    if(value <= -5) return COLOR_CYAN;       // -14 do -5 = cyan (dużo cut)
    if(value >= 5)  return COLOR_ORANGE;     // +5 do +14 = pomarańczowy (dużo boost)
    return COLOR_GREEN;                      // -4 do +4 = zielony (neutralnie)
}

void resetAudioOverlay() {
    audioOverlayDrawn = false;  // Wymusza pełny redraw przy następnym otwarciu
}

// ============================================================
// AUDIO OVERLAY - sprite z gradientem
// ============================================================

void showAudioOverlay() {
    static uint8_t lastFocus = 255;
    static uint8_t lastVolume = 255;
    static int8_t lastBass = 127;
    static int8_t lastTreble = 127;
    
    bool focusChanged = (lastFocus != audioFocus);
    bool valuesChanged = (lastVolume != audioVolume || lastBass != audioBass || lastTreble != audioTreble);
    bool firstDraw = !audioOverlayDrawn || (lastFocus == 255);

    int y1 = 10, y2 = 32, y3 = 54;
    int arrowX = 4;
    int labelX = 16;
    int barX = 70;
    int barW = SCREEN_W - barX - 40;
    int barH = 8;
    int centerX = barX + barW / 2;

    // Pełny redraw gdy zmienia się focus lub pierwszy raz
    if(firstDraw || focusChanged) {
        audioSprite.fillSprite(COLOR_BG);
        audioSprite.setTextSize(1);

        for(int i = 0; i < 3; i++) {
            int y = (i == 0) ? y1 : (i == 1) ? y2 : y3;
            bool focused = (i == audioFocus);
            uint16_t color = focused ? COLOR_GREEN : COLOR_GRAY;

            audioSprite.setTextColor(color);

            if(focused) {
                audioSprite.setCursor(arrowX, y);
                audioSprite.print(">");
            }

            audioSprite.setCursor(labelX, y);
            if(i == 0) audioSprite.print("VOLUME");
            else if(i == 1) audioSprite.print("BASS");
            else audioSprite.print("TREBLE");

            // Tło barów z zaokrąglonymi rogami
            audioSprite.fillRoundRect(barX, y, barW, barH, 4, audioSprite.color565(25,25,25));
            audioSprite.drawRoundRect(barX, y, barW, barH, 4, focused ? COLOR_GREEN : COLOR_BORDER);
            audioSprite.fillRect(barX + 2, y + 2, barW - 4, barH - 4, audioSprite.color565(15,15,15));
        }

        lastFocus = audioFocus;
    }

    // Aktualizuj wartości jeśli się zmieniły
    if(valuesChanged || firstDraw || focusChanged) {
        
        // =====================================================
        // ===================== VOLUME ========================
        // =====================================================
        
        // Czyść obszar wartości
        audioSprite.fillRect(barX + 2, y1 + 2, barW - 4, barH - 4, audioSprite.color565(15,15,15));
        audioSprite.fillRect(SCREEN_W - 30, y1, 30, 8, COLOR_BG);
        
        int level = 63 - audioVolume;
        int percent = level * 100 / 63;
        int fillW = (barW - 4) * level / 63;
        uint16_t volColor = getVolumeColor(audioVolume);
        
        // Gradient + połysk
        if(fillW > 0) {
            for(int i = 0; i < fillW; i++) {
                int boost = (i * 40) / fillW;
                
                uint8_t r = ((volColor >> 11) & 0x1F) << 3;
                uint8_t g = ((volColor >> 5) & 0x3F) << 2;
                uint8_t b = (volColor & 0x1F) << 3;
                
                r = min(255, r + boost);
                g = min(255, g + boost);
                b = min(255, b + boost);
                
                uint16_t grad = audioSprite.color565(r, g, b);
                
                audioSprite.drawFastVLine(barX + 2 + i, y1 + 2, barH - 4,
                                          audioFocus == 0 ? grad : COLOR_GRAY);
            }
            
            // Połysk
            audioSprite.drawFastHLine(barX + 3, y1 + 2, fillW - 2,
                                      audioSprite.color565(255,255,255));
        }
        
        // Wartość procentowa
        String volStr = String(percent) + "%";
        audioSprite.setTextColor(audioFocus == 0 ? volColor : COLOR_GRAY);
        audioSprite.setCursor(SCREEN_W - volStr.length() * 6 - 4, y1);
        audioSprite.print(volStr);
        
        // =====================================================
        // ====================== BASS =========================
        // =====================================================
        
        // Czyść obszar wartości
        audioSprite.fillRect(barX + 2, y2 + 2, barW - 4, barH - 4, audioSprite.color565(15,15,15));
        audioSprite.fillRect(SCREEN_W - 24, y2, 24, 8, COLOR_BG);
        
        uint16_t bassColor = getToneColor(audioBass);
        
        // Gradient dla Bass (od środka w prawo dla + lub w lewo dla -)
        if(audioBass > 0) {
            int fillW2 = ((barW - 4) / 2) * audioBass / 14;
            
            for(int i = 0; i < fillW2; i++) {
                int boost = (i * 40) / fillW2;
                
                uint8_t r = ((bassColor >> 11) & 0x1F) << 3;
                uint8_t g = ((bassColor >> 5) & 0x3F) << 2;
                uint8_t b = (bassColor & 0x1F) << 3;
                
                r = min(255, r + boost);
                g = min(255, g + boost);
                b = min(255, b + boost);
                
                uint16_t grad = audioSprite.color565(r, g, b);
                
                audioSprite.drawFastVLine(centerX + i, y2 + 2, barH - 4,
                                          audioFocus == 1 ? grad : COLOR_GRAY);
            }
            
            // Połysk
            if(fillW2 > 2) {
                audioSprite.drawFastHLine(centerX + 1, y2 + 2, fillW2 - 2,
                                          audioSprite.color565(255,255,255));
            }
        }
        else if(audioBass < 0) {
            int fillW2 = ((barW - 4) / 2) * (-audioBass) / 14;
            
            for(int i = 0; i < fillW2; i++) {
                int boost = (i * 40) / fillW2;
                
                uint8_t r = ((bassColor >> 11) & 0x1F) << 3;
                uint8_t g = ((bassColor >> 5) & 0x3F) << 2;
                uint8_t b = (bassColor & 0x1F) << 3;
                
                r = min(255, r + boost);
                g = min(255, g + boost);
                b = min(255, b + boost);
                
                uint16_t grad = audioSprite.color565(r, g, b);
                
                audioSprite.drawFastVLine(centerX - i - 1, y2 + 2, barH - 4,
                                          audioFocus == 1 ? grad : COLOR_GRAY);
            }
            
            // Połysk
            if(fillW2 > 2) {
                audioSprite.drawFastHLine(centerX - fillW2 + 1, y2 + 2, fillW2 - 2,
                                          audioSprite.color565(255,255,255));
            }
        }
        
        // Wartość dB
        String bassStr = String(audioBass > 0 ? "+" : "") + String(audioBass);
        audioSprite.setTextColor(audioFocus == 1 ? bassColor : COLOR_GRAY);
        audioSprite.setCursor(SCREEN_W - bassStr.length() * 6 - 4, y2);
        audioSprite.print(bassStr);
        
        // =====================================================
        // ===================== TREBLE ========================
        // =====================================================
        
        // Czyść obszar wartości
        audioSprite.fillRect(barX + 2, y3 + 2, barW - 4, barH - 4, audioSprite.color565(15,15,15));
        audioSprite.fillRect(SCREEN_W - 24, y3, 24, 8, COLOR_BG);
        
        uint16_t trebColor = getToneColor(audioTreble);
        
        // Gradient dla Treble (od środka w prawo dla + lub w lewo dla -)
        if(audioTreble > 0) {
            int fillW3 = ((barW - 4) / 2) * audioTreble / 14;
            
            for(int i = 0; i < fillW3; i++) {
                int boost = (i * 40) / fillW3;
                
                uint8_t r = ((trebColor >> 11) & 0x1F) << 3;
                uint8_t g = ((trebColor >> 5) & 0x3F) << 2;
                uint8_t b = (trebColor & 0x1F) << 3;
                
                r = min(255, r + boost);
                g = min(255, g + boost);
                b = min(255, b + boost);
                
                uint16_t grad = audioSprite.color565(r, g, b);
                
                audioSprite.drawFastVLine(centerX + i, y3 + 2, barH - 4,
                                          audioFocus == 2 ? grad : COLOR_GRAY);
            }
            
            // Połysk
            if(fillW3 > 2) {
                audioSprite.drawFastHLine(centerX + 1, y3 + 2, fillW3 - 2,
                                          audioSprite.color565(255,255,255));
            }
        }
        else if(audioTreble < 0) {
            int fillW3 = ((barW - 4) / 2) * (-audioTreble) / 14;
            
            for(int i = 0; i < fillW3; i++) {
                int boost = (i * 40) / fillW3;
                
                uint8_t r = ((trebColor >> 11) & 0x1F) << 3;
                uint8_t g = ((trebColor >> 5) & 0x3F) << 2;
                uint8_t b = (trebColor & 0x1F) << 3;
                
                r = min(255, r + boost);
                g = min(255, g + boost);
                b = min(255, b + boost);
                
                uint16_t grad = audioSprite.color565(r, g, b);
                
                audioSprite.drawFastVLine(centerX - i - 1, y3 + 2, barH - 4,
                                          audioFocus == 2 ? grad : COLOR_GRAY);
            }
            
            // Połysk
            if(fillW3 > 2) {
                audioSprite.drawFastHLine(centerX - fillW3 + 1, y3 + 2, fillW3 - 2,
                                          audioSprite.color565(255,255,255));
            }
        }
        
        // Wartość dB
        String trebStr = String(audioTreble > 0 ? "+" : "") + String(audioTreble);
        audioSprite.setTextColor(audioFocus == 2 ? trebColor : COLOR_GRAY);
        audioSprite.setCursor(SCREEN_W - trebStr.length() * 6 - 4, y3);
        audioSprite.print(trebStr);
        
        // Zapamiętaj wartości
        lastVolume = audioVolume;
        lastBass = audioBass;
        lastTreble = audioTreble;
    }
    
    // Push sprite na ekran
    audioSprite.pushSprite(0, 0);
    audioOverlayDrawn = true;
}




// ============================================================
// DISPLAY - splash
// ============================================================
void showSplash() {
    tft.pushImage(0, 0, LOGO_W, LOGO_H, bootlogo);
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
// AUDIO ADJUSTMENT
// ============================================================
void adjustAudio(int delta) {
    // Reguluje parametr wskazywany przez audioFocus
    if(audioFocus == 0) {
        // Volume - PT2314E ma odwrotną skalę
        int newVol = (int)audioVolume + delta;
        if(newVol < 0) newVol = 0;
        if(newVol > 63) newVol = 63;
        audioVolume = (uint8_t)newVol;
        audio.setVolume(audioVolume);
        Serial.printf("Volume: %d (inv: %d = %d%%)\n", 
            audioVolume, 63-audioVolume, (63-audioVolume)*100/63);
    }
    else if(audioFocus == 1) {
        // Bass - odwróć delta bo Vol+/Vol- mają odwrotną logikę
        int newBass = audioBass + (-delta);
        if(newBass < -14) newBass = -14;
        if(newBass > 14) newBass = 14;
        audioBass = (int8_t)newBass;
        audio.setBass(audioBass);
        Serial.printf("Bass: %+d dB\n", audioBass);
    }
    else if(audioFocus == 2) {
        // Treble - odwróć delta bo Vol+/Vol- mają odwrotną logikę
        int newTreble = audioTreble + (-delta);
        if(newTreble < -14) newTreble = -14;
        if(newTreble > 14) newTreble = 14;
        audioTreble = (int8_t)newTreble;
        audio.setTreble(audioTreble);
        Serial.printf("Treble: %+d dB\n", audioTreble);
    }
    
    // Powiadom wszystkich klientów o zmianie
    String msg = "{\"type\":\"audio_changed\",";
    msg += "\"vol\":" + String(audioVolume) + ",";
    msg += "\"bass\":" + String(audioBass) + ",";
    msg += "\"treble\":" + String(audioTreble) + "}";
    notifyAll(msg);
    
    // Zapisz do NVS
    saveAudioSettings();
    
    // Publikuj do MQTT
    mqttPublishAudio();
    
    audioOverlayActive = true;
    audioOverlayStart = millis();
    showAudioOverlay();
}

void changeFocus(int delta) {
    audioFocus = (audioFocus + delta + 3) % 3;  // 0-2 z wraparound
    resetAudioOverlay();  // Zmiana fokusu - wymaga pełnego redraw
    audioOverlayActive = true;
    audioOverlayStart = millis();
    showAudioOverlay();
    Serial.printf("Focus: %d (Vol/Bass/Treb)\n", audioFocus);
}

// ============================================================
// AUDIO
// ============================================================
void initAudio(uint8_t input) {
    audio.setVolume(audioVolume);
    audio.setBass(audioBass);
    audio.setTreble(audioTreble);
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

void mqttPublishAudio() {
    if(!mqttConnected) return;
    
    // JSON z danymi audio
    String topic = mqttTopic + "/audio";
    String payload = "{";
    
    // Volume - inwertuj PT2314E (0=głośno, 63=cicho)
    int volPercent = (63 - audioVolume) * 100 / 63;
    payload += "\"volume\":" + String(volPercent);
    payload += ",\"volume_raw\":" + String(audioVolume);
    
    // Bass & Treble
    payload += ",\"bass\":" + String(audioBass);
    payload += ",\"treble\":" + String(audioTreble);
    
    payload += "}";
    
    mqttClient.publish(topic.c_str(), 0, false, payload.c_str());
    Serial.printf("MQTT pub: %s = %s\n", topic.c_str(), payload.c_str());
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
    applyRelays(currentInput);
    mqttPublishInput(currentInput);
    Serial.printf("Input: %d [%s]\n", currentInput + 1, INPUT_NAMES[currentInput]);
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
    json += ",\"mqtt_pass_set\":" + String(mqttPass.length() > 0 ? "true" : "false");
    json += ",\"mqtt_topic\":\"" + mqttTopic + "\"";
    json += ",\"mqtt_enabled\":" + String(mqttEnabled ? 1 : 0);
    json += ",\"mqtt_connected\":" + String(mqttConnected ? 1 : 0);
    json += ",\"relay_masks\":[";
    for(int i = 0; i < 4; i++) {
        if(i > 0) json += ",";
        json += relayEnabled[i] ? "1" : "0";
    }
    json += "]";
    json += ",\"audio_vol\":" + String(audioVolume);
    json += ",\"audio_bass\":" + String(audioBass);
    json += ",\"audio_treble\":" + String(audioTreble);
    json += ",\"ir\":{";
    json += "\"source\":"  + String(irCodes[0]);
    json += ",\"input1\":" + String(irCodes[1]);
    json += ",\"input2\":" + String(irCodes[2]);
    json += ",\"input3\":" + String(irCodes[3]);
    json += ",\"input4\":" + String(irCodes[4]);
    json += ",\"volp\":"   + String(irCodes[5]);
    json += ",\"volm\":"   + String(irCodes[6]);
    json += ",\"up\":"     + String(irCodes[7]);
    json += ",\"down\":"   + String(irCodes[8]);
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
    else if(data.indexOf("\"set_mqtt_enabled\"") >= 0) {
        int ei = data.indexOf("\"enabled\":");
        if(ei >= 0) {
            String val = data.substring(ei + 10, ei + 15);
            val.trim();
            bool newEnabled = val.startsWith("true") || val.startsWith("1");
            if(newEnabled != mqttEnabled) {
                mqttEnabled = newEnabled;
                saveMqttSettings();
                Serial.printf("MQTT: set_mqtt_enabled = %d\n", mqttEnabled);
                
                if(!mqttEnabled && mqttClient.connected()) {
                    // Wyłączamy MQTT - disconnect
                    mqttClient.disconnect();
                    Serial.println("MQTT: Disconnecting...");
                }
                // Włączanie MQTT - loop() sam połączy przez mqttReconnectIfNeeded()
            }
            notifyAll(buildStatusJSON());
        }
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
    else if(data.indexOf("\"clear_ir\"") >= 0) {
        int idx = data.indexOf("\"key\":\"");
        if(idx >= 0) {
            int s = idx + 7, e = data.indexOf("\"", s);
            String key = data.substring(s, e);
            
            // Znajdź indeks i wyzeruj kod
            if(key == "source")       irCodes[0] = 0;
            else if(key == "input1")  irCodes[1] = 0;
            else if(key == "input2")  irCodes[2] = 0;
            else if(key == "input3")  irCodes[3] = 0;
            else if(key == "input4")  irCodes[4] = 0;
            else if(key == "volp")    irCodes[5] = 0;
            else if(key == "volm")    irCodes[6] = 0;
            else if(key == "up")      irCodes[7] = 0;
            else if(key == "down")    irCodes[8] = 0;
            
            saveIRCodes();
            Serial.println("IR cleared: " + key);
            notifyAll("{\"type\":\"ir_cleared\",\"key\":\"" + key + "\"}");
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
            notifyAll("{\"type\":\"status_updated\",\"disp_invert\":" + String(dispInvert ? "true" : "false") + "}");
        }
    }
    else if(data.indexOf("\"set_rotate\"") >= 0) {
        int idx = data.indexOf("\"value\":");
        if(idx >= 0) {
            dispRotate = data.substring(idx + 8).startsWith("true");
            applyDisplaySettings();
            saveDisplaySettings();
            notifyAll("{\"type\":\"status_updated\",\"disp_rotate\":" + String(dispRotate ? "true" : "false") + "}");
        }
    }
    else if(data.indexOf("\"save_relays\"") >= 0) {
        Serial.printf("DEBUG: Parsing relays from: %s\n", data.c_str());
        for(int i = 0; i < 4; i++) {
            String key = "\"mask" + String(i) + "\":";
            int idx = data.indexOf(key);
            if(idx >= 0) {
                String valStr = data.substring(idx + key.length());
                int val = valStr.toInt();
                relayEnabled[i] = (val != 0);
                Serial.printf("  mask%d: idx=%d, valStr='%s', val=%d, enabled=%d\n", 
                    i, idx, valStr.substring(0, 5).c_str(), val, relayEnabled[i]);
            }
        }
        saveRelayEnabled();
        applyRelays(currentInput);
        // Wyślij pełny status ze Updated relay_masks
        String statusJson = "{\"type\":\"relays_updated\",\"relay_masks\":[";
        for(int i = 0; i < 4; i++) {
            if(i > 0) statusJson += ",";
            statusJson += relayEnabled[i] ? "1" : "0";
        }
        statusJson += "]}";
        notifyAll(statusJson);
        Serial.println("Relay config updated");
    }
    else if(data.indexOf("\"set_volume\"") >= 0) {
        int idx = data.indexOf("\"value\":");
        if(idx >= 0) {
            audioVolume = (uint8_t)data.substring(idx + 8).toInt();
            if(audioVolume > 63) audioVolume = 63;
            audio.setVolume(audioVolume);
            saveAudioSettings();
            mqttPublishAudio();
        }
    }
    else if(data.indexOf("\"set_bass\"") >= 0) {
        int idx = data.indexOf("\"value\":");
        if(idx >= 0) {
            audioBass = (int8_t)data.substring(idx + 8).toInt();
            if(audioBass < -14) audioBass = -14;
            if(audioBass > 14) audioBass = 14;
            audio.setBass(audioBass);
            saveAudioSettings();
            mqttPublishAudio();
        }
    }
    else if(data.indexOf("\"set_treble\"") >= 0) {
        int idx = data.indexOf("\"value\":");
        if(idx >= 0) {
            audioTreble = (int8_t)data.substring(idx + 8).toInt();
            if(audioTreble < -14) audioTreble = -14;
            if(audioTreble > 14) audioTreble = 14;
            audio.setTreble(audioTreble);
            saveAudioSettings();
            mqttPublishAudio();
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
        if(ei >= 0) {
            // Akceptuj zarówno true/false (bez cudzysłowu), jak i "true"/"false" (string)
            String val = data.substring(ei + 10, ei + 15);
            val.trim();
            if(val.startsWith("true") || val.startsWith("1")) mqttEnabled = true;
            else mqttEnabled = false;
        }
        saveMqttSettings();
        Serial.printf("MQTT: Config saved - enabled=%d, host=%s, port=%d\n", 
                      mqttEnabled, mqttHost.c_str(), mqttPort);
        notifyAll("{\"type\":\"mqtt_saved\"}");
        
        // Disconnect i rekonfiguruj z nowymi parametrami
        if(mqttClient.connected()) {
            mqttClient.disconnect();
            Serial.println("MQTT: Disconnecting to apply new config...");
        }
        
        if(mqttEnabled && staConnected) {
            Serial.println("MQTT: Reconfiguring with new parameters...");
            setupMqtt();  // Zaktualizuj parametry (host, port, credentials)
            // Połączenie nastąpi przez mqttReconnectIfNeeded() w loop()
        }
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
void WiFiEvent(WiFiEvent_t event) {
    switch(event) {
        case SYSTEM_EVENT_STA_GOT_IP:
            Serial.println("WiFi: Got IP - MQTT can now connect");
            staConnected = true;
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            Serial.println("WiFi: Disconnected - MQTT will disconnect");
            staConnected = false;
            mqttConnected = false;
            break;
        default:
            break;
    }
}

void onMqttConnect(bool sessionPresent) {
    mqttConnected = true;
    Serial.println("MQTT: Connected");

    // Subscribe na temat /set (input)
    String subTopic = mqttTopic + "/set";
    mqttClient.subscribe(subTopic.c_str(), 0);
    Serial.println("MQTT: Subscribed to " + subTopic);
    
    // Subscribe na tematy audio
    mqttClient.subscribe((mqttTopic + "/volume/set").c_str(), 0);
    mqttClient.subscribe((mqttTopic + "/bass/set").c_str(), 0);
    mqttClient.subscribe((mqttTopic + "/treble/set").c_str(), 0);
    Serial.println("MQTT: Subscribed to audio topics");

    // Publish status online + aktualny stan
    mqttPublishStatus("online");
    mqttPublishInput(currentInput);
    mqttPublishAudio();  // Publikuj początkowy stan audio

    // Powiadom web interface
    notifyAll("{\"type\":\"mqtt_connected\"}");
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
    mqttConnected = false;
    
    // Wyświetl kod błędu
    Serial.print("MQTT: Disconnected - reason: ");
    switch(reason) {
        case AsyncMqttClientDisconnectReason::TCP_DISCONNECTED:
            Serial.println("TCP_DISCONNECTED"); break;
        case AsyncMqttClientDisconnectReason::MQTT_UNACCEPTABLE_PROTOCOL_VERSION:
            Serial.println("UNACCEPTABLE_PROTOCOL_VERSION"); break;
        case AsyncMqttClientDisconnectReason::MQTT_IDENTIFIER_REJECTED:
            Serial.println("IDENTIFIER_REJECTED"); break;
        case AsyncMqttClientDisconnectReason::MQTT_SERVER_UNAVAILABLE:
            Serial.println("SERVER_UNAVAILABLE"); break;
        case AsyncMqttClientDisconnectReason::MQTT_MALFORMED_CREDENTIALS:
            Serial.println("MALFORMED_CREDENTIALS"); break;
        case AsyncMqttClientDisconnectReason::MQTT_NOT_AUTHORIZED:
            Serial.println("NOT_AUTHORIZED - check username/password!"); break;
        default:
            Serial.println("UNKNOWN (" + String((int)reason) + ")"); break;
    }
    
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
    // {topic}/volume/set - ustaw głośność (0-100%)
    else if(topicStr == mqttTopic + "/volume/set") {
        int percent = msg.toInt();
        if(percent >= 0 && percent <= 100) {
            // Konwersja procent → PT2314E (inwersja!)
            audioVolume = 63 - (percent * 63 / 100);
            audio.setVolume(audioVolume);
            saveAudioSettings();
            notifyAll("{\"type\":\"audio_changed\",\"vol\":" + String(audioVolume) + 
                     ",\"bass\":" + String(audioBass) + ",\"treble\":" + String(audioTreble) + "}");
            mqttPublishAudio();
            Serial.printf("MQTT: Volume set to %d%%\n", percent);
        }
    }
    // {topic}/bass/set - ustaw bass (-14 do +14)
    else if(topicStr == mqttTopic + "/bass/set") {
        int val = msg.toInt();
        if(val >= -14 && val <= 14) {
            audioBass = (int8_t)val;
            audio.setBass(audioBass);
            saveAudioSettings();
            notifyAll("{\"type\":\"audio_changed\",\"vol\":" + String(audioVolume) + 
                     ",\"bass\":" + String(audioBass) + ",\"treble\":" + String(audioTreble) + "}");
            mqttPublishAudio();
            Serial.printf("MQTT: Bass set to %+d dB\n", audioBass);
        }
    }
    // {topic}/treble/set - ustaw treble (-14 do +14)
    else if(topicStr == mqttTopic + "/treble/set") {
        int val = msg.toInt();
        if(val >= -14 && val <= 14) {
            audioTreble = (int8_t)val;
            audio.setTreble(audioTreble);
            saveAudioSettings();
            notifyAll("{\"type\":\"audio_changed\",\"vol\":" + String(audioVolume) + 
                     ",\"bass\":" + String(audioBass) + ",\"treble\":" + String(audioTreble) + "}");
            mqttPublishAudio();
            Serial.printf("MQTT: Treble set to %+d dB\n", audioTreble);
        }
    }
}

// ============================================================
// MQTT - setup i reconnect
// ============================================================
void setupMqtt() {
    if(mqttHost.length() == 0 || !mqttEnabled) return;

    mqttClient.setServer(mqttHost.c_str(), mqttPort);

    if(mqttUser.length() > 0) {
        mqttClient.setCredentials(mqttUser.c_str(), mqttPass.c_str());
        Serial.printf("MQTT: Using credentials - user='%s', pass='%s'\n", 
                      mqttUser.c_str(), mqttPass.length() > 0 ? "***" : "(empty)");
    } else {
        Serial.println("MQTT: No credentials set (anonymous)");
    }

    // Last Will Testament
    String lwtTopic = mqttTopic + "/status";
    mqttClient.setWill(lwtTopic.c_str(), 0, true, "offline");

    mqttClient.setClientId("AudioSwitcher");
    
    Serial.printf("MQTT: Configured for %s:%d\n", mqttHost.c_str(), mqttPort);
}

void mqttReconnectIfNeeded() {
    if(!mqttEnabled || mqttHost.length() == 0) return;
    if(mqttConnected) return;
    if(!staConnected) return;

    unsigned long now = millis();
    if(now - mqttLastReconnect < MQTT_RECONNECT_DELAY) return;
    mqttLastReconnect = now;

    Serial.println("MQTT: Attempting to connect...");
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
            
            if(success) {
                Serial.println("OTA: Update SUCCESS!");
                Serial.printf("OTA: New firmware will boot on next restart\n");
            } else {
                Serial.println("OTA: Update FAILED!");
                Update.printError(Serial);
            }
            
            String msg = success ? "{\"type\":\"ota_ok\"}" : "{\"type\":\"ota_fail\"}";
            notifyAll(msg);
            req->send(200, "application/json", success ? "{\"ok\":true}" : "{\"ok\":false}");
            
            if(success) {
                Serial.println("OTA: Restarting in 2 seconds...");
                delay(2000);
                ESP.restart();
            }
        },
        // Odbiór danych
        [](AsyncWebServerRequest* req, String filename,
           size_t index, uint8_t* data, size_t len, bool final) {

            if(index == 0) {
                Serial.printf("OTA: Start - %s\n", filename.c_str());
                otaInProgress = true;
                
                // Pokaż aktualną partycję
                const esp_partition_t* running = esp_ota_get_running_partition();
                const esp_partition_t* target = esp_ota_get_next_update_partition(NULL);
                
                Serial.printf("OTA: Running from: %s (0x%x)\n", running->label, running->address);
                Serial.printf("OTA: Will update: %s (0x%x)\n", target->label, target->address);
                
                // Użyj konkretnego rozmiaru zamiast UPDATE_SIZE_UNKNOWN
                size_t updateSize = req->contentLength();
                Serial.printf("OTA: Size = %u bytes\n", updateSize);
                
                // Begin update - automatycznie wykryje typ partycji
                if(!Update.begin(updateSize)) {
                    Update.printError(Serial);
                    Serial.println("OTA: Update.begin() failed!");
                    return;
                }
                
                Serial.println("OTA: Update.begin() OK - receiving data...");
            }

            if(Update.write(data, len) != len) {
                Update.printError(Serial);
            }

            // Progress - ograniczaj powiadomienia do co 5%
            if(Update.size() > 0) {
                int percent = (index + len) * 100 / req->contentLength();
                showOTAOverlay(percent);
                static int lastPercent = -10; // -10 by zawsze wysłać pierwsze
                if (percent / 5 != lastPercent / 5) { // tylko co 5%
                    notifyAll("{\"type\":\"ota_progress\",\"percent\":" + String(percent) + "}");
                    lastPercent = percent;
                }
            }

            if(final) {
                if(Update.end(true)) {
                    Serial.printf("OTA: Done - %u bytes written\n", index + len);
                    Serial.printf("OTA: MD5: %s\n", Update.md5String().c_str());
                    
                    // Sprawdź czy zapisano do właściwej partycji
                    if(Update.hasError()) {
                        Serial.println("OTA: Verification FAILED!");
                        Update.printError(Serial);
                    } else {
                        Serial.println("OTA: Verification OK!");
                        
                        // WAŻNE: Ustaw nową partycję jako bootującą
                        const esp_partition_t* update_partition = esp_ota_get_next_update_partition(NULL);
                        if(update_partition != NULL) {
                            esp_err_t err = esp_ota_set_boot_partition(update_partition);
                            if(err == ESP_OK) {
                                Serial.printf("OTA: Set boot partition to %s\n", update_partition->label);
                            } else {
                                Serial.printf("OTA: Failed to set boot partition! Error: 0x%x\n", err);
                            }
                        }
                    }
                } else {
                    Serial.println("OTA: Update.end() FAILED!");
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
    // Register WiFi event handler PRZED WiFi.begin()
    WiFi.onEvent(WiFiEvent);
    
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
        if(learningKey == "source")       irCodes[0] = code;
        else if(learningKey == "input1")  irCodes[1] = code;
        else if(learningKey == "input2")  irCodes[2] = code;
        else if(learningKey == "input3")  irCodes[3] = code;
        else if(learningKey == "input4")  irCodes[4] = code;
        else if(learningKey == "volp")    irCodes[5] = code;
        else if(learningKey == "volm")    irCodes[6] = code;
        else if(learningKey == "up")      irCodes[7] = code;
        else if(learningKey == "down")    irCodes[8] = code;
        learningIR = false;
        clearOverlay();
        String msg = "{\"type\":\"ir_received\",\"key\":\"" + learningKey + "\",\"code\":" + String(code) + "}";
        notifyAll(msg);
        return;
    }

    // Sprawdź czy to kod który wspiera auto-repeat (Vol+/Vol-/↑/↓)
    bool isRepeatableCode = (code == irCodes[5] || code == irCodes[6] || 
                             code == irCodes[7] || code == irCodes[8]) && code != 0;
    
    // Jeśli to ten sam kod - oznacz jako repeating
    if(code == lastIRCode && isRepeatableCode) {
        irRepeating = true;
    } else {
        irRepeating = false;
    }
    
    lastIRCode = code;
    lastIRTime = millis();

    if(code == irCodes[0] && irCodes[0] != 0) {
        switchInput((currentInput + 1) % 4);
        notifyAll("{\"type\":\"input_changed\",\"input\":" + String(currentInput) + "}");
    } 
    // Vol+ / Vol- - regulacja aktywnego parametru
    else if(code == irCodes[5] && irCodes[5] != 0) {
        adjustAudio(-1);  // Vol+ = głośniej = mniejsza wartość PT2314E
    }
    else if(code == irCodes[6] && irCodes[6] != 0) {
        adjustAudio(+1);  // Vol- = ciszej = większa wartość PT2314E
    }
    // ↑ / ↓ - zmiana fokusu
    else if(code == irCodes[7] && irCodes[7] != 0) {
        changeFocus(-1);  // ↑ = do góry listy
    }
    else if(code == irCodes[8] && irCodes[8] != 0) {
        changeFocus(+1);  // ↓ = w dół listy
    }
    // Direct input select
    else {
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
    Serial.println("║      AUDIO SWITCHER v4.2              ║");
    Serial.println("╚═══════════════════════════════════════╝\n");
    
    // Informacje o partycjach OTA
    Serial.printf("Sketch size: %u / %u bytes\n", ESP.getSketchSize(), ESP.getFreeSketchSpace());
    Serial.printf("Free heap: %u bytes\n", ESP.getFreeHeap());
    
    const esp_partition_t* running = esp_ota_get_running_partition();
    if(running) {
        Serial.printf("Running partition: %s (0x%x, %u bytes)\n", 
                      running->label, running->address, running->size);
    }
    
    const esp_partition_t* next = esp_ota_get_next_update_partition(NULL);
    if(next) {
        Serial.printf("Next OTA partition: %s (0x%x, %u bytes)\n\n", 
                      next->label, next->address, next->size);
    }

    // Backlight PWM
    ledcSetup(BL_CHANNEL, BL_FREQ, BL_RES);
    ledcAttachPin(TFT_BL, BL_CHANNEL);
    ledcWrite(BL_CHANNEL, BL_DEFAULT);

    // Relays - domyślnie wyłączone
    for(uint8_t i = 0; i < 4; i++) {
        pinMode(RELAY_PINS[i], OUTPUT);
        digitalWrite(RELAY_PINS[i], LOW);
    }

    // Display
    tft.init();
    tft.setSwapBytes(true);   // WAŻNE dla RGB565 z PROGMEM
    tft.setRotation(3);
    tft.invertDisplay(false);
    tft.fillScreen(COLOR_BG);
    showSplash();

    audioSprite.setColorDepth(16);
    audioSprite.createSprite(SCREEN_W, SCREEN_H);

    // NVS
    loadPrefs();
    ledcWrite(BL_CHANNEL, brightness);
    tft.setRotation(dispRotate ? 3 : 1);
    tft.invertDisplay(dispInvert);
    applyRelays(currentInput);  // Przywróć stan przekaźników

    // PT2314E
    Wire.begin(PT_SDA, PT_SCL);
    Wire.setClock(100000);
    delay(200);
    initAudio(currentInput);

    // IR - włącz wszystkie protokoły z większą tolerancją
    irrecv.setTolerance(30);  // Domyślnie 25%, zwiększ do 30%
    irrecv.enableIRIn();
    Serial.println("IR: Receiver enabled - RC6 tolerance increased to 30%");

    // UI
    drawUI();

    // WiFi
    setupWifi();

    // Web server + OTA
    setupServer();

    // MQTT - rejestruj callbacki (RAZ, na stałe)
    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onMessage(onMqttMessage);
    
    // MQTT - konfiguracja (parametry mogą się zmieniać)
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
    // Audio overlay timeout - powrót do kafelków po 2s
    if(audioOverlayActive && (millis() - audioOverlayStart > AUDIO_OVERLAY_TIMEOUT)) {
        audioOverlayActive = false;
        resetAudioOverlay();  // Reset dla następnego otwarcia
        drawUI();
    }

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
            
            uint32_t irCode = 0;
            
            // RC6: Użyj hashu z PEŁNEGO sygnału (gdzie są dane przycisku)
            if(results.decode_type == RC6 && results.rawlen >= 20) {
                // Hash z CAŁEGO sygnału - każda wartość ma znaczenie!
                uint32_t hash = 0;
                
                // Użyj wszystkich wartości od 5 do końca
                for(int i = 5; i < results.rawlen && i < 60; i++) {
                    // Zaokrąglij do 100us - kompromis między stabilnością a unikalnością
                    uint16_t val = (results.rawbuf[i] / 100) * 100;
                    hash = (hash * 31 + val) & 0xFFFFFFFF;
                }
                irCode = hash;
                
                Serial.println("=== IR RECEIVED (RC6) ===");
                Serial.printf("Raw length: %d\n", results.rawlen);
                Serial.printf("Full hash: 0x%08X\n", hash);
                
                // DEBUG: Pokaż WSZYSTKIE raw values (gdzie są dane przycisku!)
                Serial.print("Full raw: ");
                for(int i = 5; i < min((int)results.rawlen, 60); i++) {
                    Serial.printf("%d ", results.rawbuf[i]);
                    if((i - 5) % 15 == 14) Serial.println();
                }
                Serial.println();
                Serial.println("===================");
            } else {
                // Inne protokoły - użyj decoded value
                irCode = results.value;
                
                Serial.println("=== IR RECEIVED ===");
                Serial.printf("Protocol: %s\n", typeToString(results.decode_type).c_str());
                Serial.printf("Value: 0x%08X\n", results.value);
                Serial.printf("Bits: %d\n", results.bits);
                Serial.println("===================");
            }
            
            // Ignoruj repeat codes i puste
            if(irCode != 0xFFFFFFFF && irCode != 0 && irCode != 0x00000003) {
                handleIR(irCode);
                lastIR = now;
            }
        }
        irrecv.resume();
    }
    
    // IR Auto-repeat dla Vol+/Vol-/↑/↓
    if(irRepeating && lastIRCode != 0) {
        unsigned long now = millis();
        unsigned long timeSincePress = now - lastIRTime;
        
        // Po 500ms zaczynam auto-repeat co 150ms
        if(timeSincePress > IR_REPEAT_DELAY) {
            static unsigned long lastRepeat = 0;
            if(now - lastRepeat > IR_REPEAT_RATE) {
                // Powtórz akcję
                if(lastIRCode == irCodes[5]) {
                    adjustAudio(-1);  // Vol+
                }
                else if(lastIRCode == irCodes[6]) {
                    adjustAudio(+1);  // Vol-
                }
                else if(lastIRCode == irCodes[7]) {
                    changeFocus(-1);  // ↑
                }
                else if(lastIRCode == irCodes[8]) {
                    changeFocus(+1);  // ↓
                }
                lastRepeat = now;
            }
        }
    } else {
        // Reset gdy przycisk puszczony (brak sygnału przez >500ms)
        if(millis() - lastIRTime > 500) {
            irRepeating = false;
            lastIRCode = 0;
        }
    }

    // MQTT reconnect only
    mqttReconnectIfNeeded();

    // WebSocket cleanup
    ws.cleanupClients();

    delay(10);
}

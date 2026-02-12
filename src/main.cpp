
// ============================================================
// AUDIO SWITCHER v4.2
// ESP32-WROOM-32 + PT2314E + ST7789 + IR + WiFi + WS + MQTT + OTA
// Główny plik programu - obsługa logiki, sprzętu, sieci, MQTT, WebSocket, IR, NVS, UI
// ============================================================

#include <Arduino.h>
#include <Wire.h>         // I2C - sterowanie PT2314E
#include <SPI.h>          // SPI - sterowanie wyświetlaczem
#include <WiFi.h>         // WiFi - tryb AP/STA
#include <Preferences.h>  // NVS - trwałe ustawienia
#include <ESPAsyncWebServer.h> // Async WebServer - HTTP/WS
#include <AsyncWebSocket.h>
#include <AsyncMqttClient.h>   // MQTT
#include <Update.h>       // OTA
#include <Adafruit_GFX.h> // Grafika
#include <Adafruit_ST7789.h> // Wyświetlacz
#include <IRrecv.h>       // Odbiornik IR
#include <IRremoteESP8266.h>
#include <IRutils.h>
#include "PT2314.h"      // Sterownik audio
#include "icons.h"       // Ikony wejść
#include "logo.h"        // Logo startowe
#include "web.h"         // Wbudowany frontend (HTML)

// ============================================================
// PINS - definicje pinów ESP32
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


// Przekaźniki - SSR, sterowanie wysokim poziomem
#define RELAY_1  32
#define RELAY_2  33
#define RELAY_3  25
#define RELAY_4  26
const uint8_t RELAY_PINS[4] = {RELAY_1, RELAY_2, RELAY_3, RELAY_4}; // Tablica pinów przekaźników


// PWM - podświetlenie TFT
#define BL_CHANNEL   0      // Kanał PWM
#define BL_FREQ   5000      // Częstotliwość
#define BL_RES       8      // Rozdzielczość (8 bitów)
#define BL_DEFAULT 200      // Domyślna jasność


// ============================================================
// DISPLAY - parametry ekranu
// ============================================================
#define SCREEN_W 284   // Szerokość ekranu
#define SCREEN_H  76   // Wysokość ekranu


// ============================================================
// WiFi AP defaults - dane do trybu Access Point
// ============================================================
#define AP_SSID  "AudioSwitcher"
#define AP_PASS  "audio1234"
#define AP_IP    "192.168.4.1"


// ============================================================
// IR - parametry obsługi pilota
// ============================================================
#define IR_DEBOUNCE      300     // Debounce IR (ms)
#define IR_LEARN_TIMEOUT 10000   // Timeout nauki IR (ms)


// ============================================================
// MQTT defaults - domyślne ustawienia MQTT
// ============================================================
#define MQTT_RECONNECT_DELAY 5000      // Opóźnienie ponownego łączenia (ms)
#define MQTT_TOPIC_DEFAULT   "audio_switcher" // Domyślny topic

#define COLOR_BG       0x0000   // Tło (czarny)
#define COLOR_ACTIVE   0x07E0   // Aktywny (zielony)
#define COLOR_INACTIVE 0x0000   // Nieaktywny (czarny)
#define COLOR_BORDER   0x4A69   // Ramka
#define COLOR_WHITE    0xFFFF   // Biały
#define COLOR_GRAY     0x8410   // Szary
#define COLOR_GREEN    0x07E0   // Zielony
#define COLOR_RED      0xF800   // Czerwony
#define COLOR_YELLOW   0xFFE0   // Żółty
#define COLOR_BLUE     0x001F   // Niebieski
#define COLOR_CYAN     0x07FF   // Cyan
#define COLOR_ORANGE   0xFD20   // Pomarańczowy
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

#define TILE_MARGIN  5
#define TILE_H      (SCREEN_H - TILE_MARGIN * 2)
#define TILE_W      ((SCREEN_W - TILE_MARGIN * 5) / 4)
#define TILE_RADIUS  6
// Layout kafelków wejść na ekranie
#define TILE_MARGIN  5
#define TILE_H      (SCREEN_H - TILE_MARGIN * 2)
#define TILE_W      ((SCREEN_W - TILE_MARGIN * 5) / 4)
#define TILE_RADIUS  6


// ============================================================
// NAZWY WEJŚĆ i ikony
// ============================================================
const char* INPUT_NAMES[] = {"Input1", "Input2", "Input3", "Input4"}; // Nazwy wejść
const uint16_t* INPUT_ICONS[] = {icon_input1, icon_input2, icon_input3, icon_input4}; // Ikony wejść


// ============================================================
// GLOBALS - zmienne globalne, instancje klas, stany
// ============================================================
Adafruit_ST7789  tft    = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST); // Wyświetlacz
IRrecv           irrecv(IR_PIN);    // Odbiornik IR
PT2314           audio;             // Sterownik audio
AsyncWebServer   server(80);        // Serwer HTTP
AsyncWebSocket   ws("/ws");        // WebSocket
AsyncMqttClient  mqttClient;        // MQTT klient
Preferences      prefs;             // NVS

uint8_t       currentInput  = 0;    // Aktualnie wybrane wejście (0-3)
unsigned long lastIR        = 0;    // Czas ostatniego odebrania IR

// IR kody (ładowane z NVS)
uint32_t irCodes[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0}; // Kody IR: 0=source, 1-4=input1-4, 5=vol+, 6=vol-, 7=up, 8=down

// IR learning state
bool          learningIR  = false;  // Czy trwa nauka IR
String        learningKey = "";     // Klucz do nauki IR
unsigned long learnStart  = 0;      // Start nauki IR

// WiFi state
String wifiSSID     = "";           // SSID WiFi
String wifiPass     = "";           // Hasło WiFi
String localIP      = "";           // IP w trybie STA
bool   staConnected = false;        // Czy połączono z WiFi

// Backlight
uint8_t brightness = BL_DEFAULT;    // Jasność podświetlenia

// Display settings
bool dispInvert = false;            // Inwersja kolorów
bool dispRotate = false;            // Obrót ekranu

// MQTT state
String        mqttHost    = "";     // Host MQTT
uint16_t      mqttPort    = 1883;   // Port MQTT
String        mqttUser    = "";     // Użytkownik MQTT
String        mqttPass    = "";     // Hasło MQTT
String        mqttTopic   = MQTT_TOPIC_DEFAULT; // Topic MQTT
bool          mqttEnabled = false;  // Czy MQTT włączone
bool          mqttConnected = false;// Czy połączono z MQTT
bool          mqttSetupDone = false;// Czy setupMqtt() już wywołane
unsigned long mqttLastReconnect = 0;// Czas ostatnej próby połączenia

// OTA state
bool otaInProgress = false;         // Czy trwa OTA

// Relay enable - czy dane wejście steruje swoim przekaźnikiem
// relayEnabled[i] = true → Input(i+1) włącza Relay(i+1)
bool relayEnabled[4] = {false, false, false, false}; // Maska przekaźników

// Audio settings
uint8_t audioVolume = 0;    // 0-63 (0=max, 63=mute - odwrotna skala PT2314E)
int8_t  audioBass   = 0;    // -14 do +14 dB
int8_t  audioTreble = 0;    // -14 do +14 dB

// Audio overlay state
bool          audioOverlayActive = false;   // Czy overlay audio jest aktywny
bool          audioOverlayDrawn  = false;   // Czy overlay został narysowany
unsigned long audioOverlayStart  = 0;       // Czas startu overlay
uint8_t       audioFocus         = 0;       // 0=Volume, 1=Bass, 2=Treble
#define AUDIO_OVERLAY_TIMEOUT 2000           // Timeout overlay (ms)


#include <Arduino.h>
// ============================================================
// NVS - obsługa trwałych ustawień (Preferences)
// ============================================================
// Wczytaj wszystkie ustawienia z NVS
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

// Zapisz ostatnio wybrane wejście do NVS
void saveLastInput(uint8_t input) {
    prefs.begin("switcher", false);
    prefs.putUChar("last_input", input);
    prefs.end();
}

// Zapisz jasność podświetlenia do NVS
void saveBrightness(uint8_t val) {
    prefs.begin("switcher", false);
    prefs.putUChar("brightness", val);
    prefs.end();
}

// Zapisz ustawienia wyświetlacza (obrót, inwersja) do NVS
void saveDisplaySettings() {
    prefs.begin("switcher", false);
    prefs.putBool("disp_invert", dispInvert);
    prefs.putBool("disp_rotate", dispRotate);
    prefs.end();
}

// Zapisz kody IR do NVS
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

// Zapisz dane WiFi do NVS
void saveWifiCreds(const String& ssid, const String& pass) {
    prefs.begin("switcher", false);
    prefs.putString("wifi_ssid", ssid);
    prefs.putString("wifi_pass", pass);
    prefs.end();
    Serial.println("NVS: WiFi saved");
}

// Zapisz ustawienia MQTT do NVS
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

// Zapisz maskę przekaźników do NVS
void saveRelayEnabled() {
    prefs.begin("switcher", false);
    prefs.putBool("relay_en0", relayEnabled[0]);
    prefs.putBool("relay_en1", relayEnabled[1]);
    prefs.putBool("relay_en2", relayEnabled[2]);
    prefs.putBool("relay_en3", relayEnabled[3]);
    prefs.end();
    Serial.println("NVS: Relay config saved");
}


// Zapisz ustawienia audio do NVS
void saveAudioSettings() {
    prefs.begin("switcher", false);
    prefs.putUChar("audio_vol",  audioVolume);
    prefs.putChar("audio_bass",  audioBass);
    prefs.putChar("audio_treb",  audioTreble);
    prefs.end();
    Serial.println("NVS: Audio settings saved");
}


// ============================================================
// DISPLAY - deklaracje funkcji związanych z UI
// ============================================================
void drawUI();                    // Rysuje główny interfejs
void notifyAll(const String& msg); // Wysyła wiadomość do wszystkich klientów WS
void setupMqtt();                  // Konfiguruje klienta MQTT
void mqttPublishAudio();           // Publikuje stan audio do MQTT

// ============================================================
// RELAY CONTROL - sterowanie przekaźnikami
// ============================================================
// Ustawia stany przekaźników na podstawie wybranego wejścia i maski relayEnabled
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
// DISPLAY SETTINGS - stosuje ustawienia ekranu
// ============================================================
void applyDisplaySettings() {
    tft.setRotation(dispRotate ? 3 : 1);
    tft.invertDisplay(dispInvert);
    drawUI();
    Serial.printf("Display: rotate=%d invert=%d\n", dispRotate, dispInvert);
}

// ============================================================
// DISPLAY - rysowanie ikony z PROGMEM
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
// DISPLAY - rysowanie kafelka wejścia
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

// Rysuje cały interfejs (wszystkie kafelki)
void drawUI() {
    tft.fillScreen(COLOR_BG);
    for(uint8_t i = 0; i < 4; i++) drawTile(i, i == currentInput);
}

// Aktualizuje wyświetlanie po zmianie wejścia
void updateInput(uint8_t oldInput, uint8_t newInput) {
    drawTile(oldInput, false);
    drawTile(newInput, true);
}

// ============================================================
// DISPLAY - overlaye (paski, komunikaty)
// ============================================================
// Overlay z informacją o WiFi
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

// Overlay z informacją o IR
void showIROverlay(const String& msg) {
    tft.fillRect(0, 0, SCREEN_W, 14, COLOR_BG);
    tft.setTextSize(1);
    tft.setTextColor(COLOR_GREEN);
    tft.setCursor(4, 3);
    tft.print(msg);
}

// Overlay z postępem OTA
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

// Czyści overlay (górny pasek)
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
// AUDIO OVERLAY - 3 paski z fokusem (bez migotania)
// ============================================================

// ============================================================
// AUDIO GRADIENT COLORS - kolory pasków audio
// ============================================================
// Zwraca kolor paska głośności w zależności od poziomu
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

// Zwraca kolor paska bass/treble w zależności od wartości
uint16_t getToneColor(int8_t value) {
    // Bass/Treble: -14 do +14
    if(value <= -5) return COLOR_CYAN;       // -14 do -5 = cyan (dużo cut)
    if(value >= 5)  return COLOR_ORANGE;     // +5 do +14 = pomarańczowy (dużo boost)
    return COLOR_GREEN;                      // -4 do +4 = zielony (neutralnie)
}

// Wymusza pełny redraw overlay audio przy następnym otwarciu
void resetAudioOverlay() {
    audioOverlayDrawn = false; 
}

// Rysuje overlay audio (3 paski: volume, bass, treble)
void showAudioOverlay() {
    static uint8_t lastFocus = 255;
    bool firstDraw = !audioOverlayDrawn || (lastFocus == 255);
    
    // Współrzędne 3 wierszy
    int y1 = 10, y2 = 32, y3 = 54;
    int arrowX = 4;
    int labelX = 16;
    int barX = 70;
    int barW = SCREEN_W - barX - 40; 
    int barH = 8;
    
    // Jeśli pierwszy raz lub zmiana fokusu → narysuj strukturę
    if(firstDraw || lastFocus != audioFocus) {
        tft.fillScreen(COLOR_BG);
        tft.setTextSize(1);
        
        // Rysuj wszystkie etykiety i ramki
        for(int i = 0; i < 3; i++) {
            int y = (i == 0) ? y1 : (i == 1) ? y2 : y3;
            bool focused = (i == audioFocus);
            uint16_t color = focused ? COLOR_GREEN : COLOR_GRAY;
            
            tft.setTextColor(color);
            
            // Strzałka fokusu
            if(focused) {
                tft.setCursor(arrowX, y);
                tft.print(">");
            }
            
            // Etykieta
            tft.setCursor(labelX, y);
            if(i == 0) tft.print("VOLUME");
            else if(i == 1) tft.print("BASS");
            else tft.print("TREBLE");
            
            // Ramka paska
            tft.drawRect(barX, y, barW, barH, color);
        }
        
        lastFocus = audioFocus;
    }
    
    // Odśwież tylko wartości i wypełnienia pasków
    int centerX = barX + barW / 2;
    
    // Volume - czyść obszar paska i wartość
    tft.fillRect(barX + 1, y1 + 1, barW - 2, barH - 2, COLOR_BG);
    tft.fillRect(SCREEN_W - 30, y1, 30, 8, COLOR_BG);  // Czyść wartość
    
    int level = 63 - audioVolume;
    int percent = level * 100 / 63;
    int fillW = (barW - 2) * level / 63;
    uint16_t volColor = getVolumeColor(audioVolume);
    if(fillW > 0) tft.fillRect(barX + 1, y1 + 1, fillW, barH - 2, audioFocus == 0 ? volColor : COLOR_GRAY);
    
    String volStr = String(percent) + "%";
    tft.setTextColor(audioFocus == 0 ? volColor : COLOR_GRAY);
    tft.setCursor(SCREEN_W - volStr.length() * 6 - 4, y1);
    tft.print(volStr);
    
    // Bass - czyść obszar paska i wartość
    tft.fillRect(barX + 1, y2 + 1, barW - 2, barH - 2, COLOR_BG);
    tft.fillRect(SCREEN_W - 24, y2, 24, 8, COLOR_BG);
    
    uint16_t bassColor = getToneColor(audioBass);
    if(audioBass > 0) {
        int fillW2 = ((barW - 2) / 2) * audioBass / 14;
        tft.fillRect(centerX, y2 + 1, fillW2, barH - 2, audioFocus == 1 ? bassColor : COLOR_GRAY);
    } else if(audioBass < 0) {
        int fillW2 = ((barW - 2) / 2) * (-audioBass) / 14;
        tft.fillRect(centerX - fillW2, y2 + 1, fillW2, barH - 2, audioFocus == 1 ? bassColor : COLOR_GRAY);
    }
    
    String bassStr = String(audioBass > 0 ? "+" : "") + String(audioBass);
    tft.setTextColor(audioFocus == 1 ? bassColor : COLOR_GRAY);
    tft.setCursor(SCREEN_W - bassStr.length() * 6 - 4, y2);
    tft.print(bassStr);
    
    // Treble - czyść obszar paska i wartość
    tft.fillRect(barX + 1, y3 + 1, barW - 2, barH - 2, COLOR_BG);
    tft.fillRect(SCREEN_W - 24, y3, 24, 8, COLOR_BG);
    
    uint16_t trebColor = getToneColor(audioTreble);
    if(audioTreble > 0) {
        int fillW3 = ((barW - 2) / 2) * audioTreble / 14;
        tft.fillRect(centerX, y3 + 1, fillW3, barH - 2, audioFocus == 2 ? trebColor : COLOR_GRAY);
    } else if(audioTreble < 0) {
        int fillW3 = ((barW - 2) / 2) * (-audioTreble) / 14;
        tft.fillRect(centerX - fillW3, y3 + 1, fillW3, barH - 2, audioFocus == 2 ? trebColor : COLOR_GRAY);
    }
    
    String trebStr = String(audioTreble > 0 ? "+" : "") + String(audioTreble);
    tft.setTextColor(audioFocus == 2 ? trebColor : COLOR_GRAY);
    tft.setCursor(SCREEN_W - trebStr.length() * 6 - 4, y3);
    tft.print(trebStr);
    
    audioOverlayDrawn = true;  // Struktura narysowana, kolejne wywołania tylko odświeżą paski
}




// ============================================================
// DISPLAY - splash screen (logo startowe)
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
// BACKLIGHT - ustawianie jasności podświetlenia
// ============================================================
void setBrightness(uint8_t val) {
    brightness = val;
    ledcWrite(BL_CHANNEL, val);
}

// ============================================================
// AUDIO ADJUSTMENT - regulacja parametrów audio (volume, bass, treble)
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

// Zmiana fokusu (volume/bass/treble) w overlay audio
void changeFocus(int delta) {
    audioFocus = (audioFocus + delta + 3) % 3;  // 0-2 z wraparound
    resetAudioOverlay();  // Zmiana fokusu - wymaga pełnego redraw
    audioOverlayActive = true;
    audioOverlayStart = millis();
    showAudioOverlay();
    Serial.printf("Focus: %d (Vol/Bass/Treb)\n", audioFocus);
}

// ============================================================
// AUDIO - inicjalizacja układu audio PT2314E
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
// MQTT - publish - publikowanie stanu do MQTT
// ============================================================
// Publikuje aktualny input do MQTT
void mqttPublishInput(uint8_t input) {
    if(!mqttConnected) return;
    String topic = mqttTopic + "/state";
    String payload = String(input + 1);  // 1-4
    mqttClient.publish(topic.c_str(), 0, true, payload.c_str());
    Serial.printf("MQTT pub: %s = %s\n", topic.c_str(), payload.c_str());
}

// Publikuje status (online/offline) do MQTT
void mqttPublishStatus(const char* status) {
    if(!mqttConnected) return;
    String topic = mqttTopic + "/status";
    mqttClient.publish(topic.c_str(), 0, true, status);
}

// Publikuje stan audio (volume, bass, treble) do MQTT
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
// INPUT SWITCH - zmiana wejścia audio
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
// WEBSOCKET - budowanie statusu JSON dla frontendu
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

// Wysyła wiadomość do wszystkich klientów WebSocket
void notifyAll(const String& msg) {
    ws.textAll(msg);
}

// ============================================================
// WEBSOCKET - obsługa wiadomości od klienta (JSON)
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
// WEBSOCKET - event handler (połączenie, rozłączenie, dane)
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
// MQTT - callbacks (połączenie, rozłączenie, wiadomości)
// ============================================================
// Callback zdarzeń WiFi (połączenie/rozłączenie)
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

// Callback po połączeniu z MQTT
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

// Callback po rozłączeniu z MQTT
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

// Callback po odebraniu wiadomości MQTT
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
// MQTT - konfiguracja klienta MQTT
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

// Próba ponownego połączenia z MQTT jeśli potrzeba
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
// OTA - konfiguracja endpointu OTA
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
// WIFI SETUP - konfiguracja WiFi (AP/STA)
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
// SERVER SETUP - konfiguracja serwera HTTP/WebSocket/OTA
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
// IR - obsługa odebranego kodu IR
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
// SETUP - główna inicjalizacja systemu
// ============================================================
void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n╔═══════════════════════════════════════╗");
    Serial.println("║      AUDIO SWITCHER v4.1              ║");
    Serial.println("╚═══════════════════════════════════════╝\n");

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
    applyRelays(currentInput);  // Przywróć stan przekaźników

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
// LOOP - główna pętla programu
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
            if(results.value != 0xFFFFFFFF && results.value != 0) {
                handleIR(results.value);
                lastIR = now;
            }
        }
        irrecv.resume();
    }

    // MQTT reconnect only
    mqttReconnectIfNeeded();

    // WebSocket cleanup
    ws.cleanupClients();

    delay(10);
}

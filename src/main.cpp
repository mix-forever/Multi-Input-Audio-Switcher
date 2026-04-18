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
#include <set>
#include <TFT_eSPI.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRutils.h>
#include "PT2314.h"
#include "config.h"
#include "globals.h"
#include "icons.h"
#include "logo.h"
#include "web.h"

// ============================================================
// PINS
// ============================================================
// (moved to config.h)

// ============================================================
// GLOBALS
// ============================================================
// (moved to globals.cpp)
// ============================================================
// DISPLAY - forward declarations
// ============================================================
void drawUI();
void notifyAll(const String& msg);
void notifyAudioChanged();
void setupMqtt();
void mqttPublishAudio();

// SETUP
// ============================================================
void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n╔═══════════════════════════════════════╗");
    Serial.println("║      MultiAMP v4.2              ║");
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
    Wire.setClock(50000);  // 50kHz - test wolniejszego I2C (mniej EMI)
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
    // Wywołaj nawet jeśli staConnected=false; loop() sama połączy gdy WiFi gotowe.
    if(mqttEnabled && mqttHost.length() > 0) setupMqtt();

    logMsg("System Ready!");
    logMsg("  AP:  http://%s", AP_IP);
    if(staConnected) logMsg("  STA: http://%s", localIP.c_str());
    if(mqttEnabled)  logMsg("  MQTT: %s:%d", mqttHost.c_str(), mqttPort);
}

// ============================================================
// LOOP
// ============================================================
void loop() {
    static uint8_t rc6TailCount = 0;
    
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
        notifyAll("{\"type\":\"ir_timeout\",\"key\":\"" + jsonEscape(learningKey) + "\"}");
    }

    // IR receive
    decode_results results;
    if(!otaInProgress && irrecv.decode(&results)) {
        unsigned long now = millis();
        
        uint32_t irCode = 0;
        
        // RC6: Użyj hashu z PEŁNEGO sygnału (gdzie są dane przycisku)
        if(results.decode_type == RC6 && results.rawlen >= 20) {
            uint32_t hash = 0;
            for(int i = 5; i < results.rawlen && i < 60; i++) {
                uint16_t val = (results.rawbuf[i] / 100) * 100;
                hash = (hash * 31 + val) & 0xFFFFFFFF;
            }
            irCode = hash;
        } else {
            irCode = results.value;
        }
        
        // Ignoruj śmieci (type=-1/UNKNOWN) - odbiornik łapie zakłócenia
        if(results.decode_type == 7) {  // 7 = RC6
            // Loguj tylko RC6
            logMsg("IR: RC6 val=0x%08X%08X hash=0x%08X len=%u repeat=%d", 
                   (uint32_t)(results.value >> 32), (uint32_t)results.value,
                   irCode, results.rawlen, results.repeat);
            
            // Repeat dla RC6: biblioteka NIE ustawia results.repeat (zawsze false)
            // Twój pilot wysyła repeat jako osobny kod z prefixem 0x7070.
            // Pierwszy 0x7070 po normalnym kodzie to "tail" pojedynczego kliknięcia.
            // Dopiero drugi+ 0x7070 to prawdziwy repeat (przytrzymanie).
            static uint64_t lastRc6Value = 0;
            bool isTailCode = ((irCode & 0xFFFF0000) == 0x70700000) && (lastIRCode != 0) && (now - lastIRTime < 300);
            
            if(isTailCode) {
                rc6TailCount++;
                if(rc6TailCount >= 2) {
                    // Drugi+ tail = prawdziwy repeat (przytrzymanie)
                    irRepeating = true;
                    lastIRTime = now;
                    lastIR = now;
                    logMsg("IR: REPEAT detected (0x%08X), lastIRCode=0x%08X", irCode, lastIRCode);
                } else {
                    // Pierwszy tail = koniec pojedynczego kliknięcia, nie repeat
                    lastIRTime = now;
                    logMsg("IR: TAIL ignored (0x%08X)", irCode);
                }
            }
            // Normalny kod RC6 - tylko jeśli minął debounce
            else if(now - lastIR > IR_DEBOUNCE && irCode != 0) {
                lastRc6Value = results.value;
                rc6TailCount = 0;  // reset licznika taili przy nowym przycisku
                handleIR(irCode);
                lastIR = now;
                logMsg("IR: HANDLED RC6=0x%08X", irCode);
            }
            else if(irCode != 0) {
                logMsg("IR: DEBOUNCED RC6=0x%08X (ago=%lu)", irCode, now - lastIR);
            }
        }
        // else: ignoruj non-RC6
        
        irrecv.resume();
    }
    
    // Reset stanu IR gdy przycisk puszczony (brak sygnału przez >500ms)
    if(millis() - lastIRTime > 500) {
        irRepeating = false;
        lastIRCode = 0;
        rc6TailCount = 0;
    }

    // Async WiFi test (przeniesione z callbacku WebSocket)
    if(wifiTestInProgress) {
        unsigned long now = millis();
        if(now >= wifiTestNextMs) {
            wifiTestNextMs = now + 500;
            wifiTestTries++;
            if(WiFi.status() == WL_CONNECTED) {
                wifiTestInProgress = false;
                String resp = "{\"type\":\"wifi_test_ok\",\"ip\":\"" + jsonEscape(WiFi.localIP().toString()) + "\"}";
                AsyncWebSocketClient* c = ws.client(wifiTestClientId);
                if(c) c->text(resp);
                else notifyAll(resp);
                logMsg("WiFi: Test OK, IP=%s", WiFi.localIP().toString().c_str());
            } else if(wifiTestTries >= 20) {
                wifiTestInProgress = false;
                String resp = "{\"type\":\"wifi_test_fail\"}";
                AsyncWebSocketClient* c = ws.client(wifiTestClientId);
                if(c) c->text(resp);
                else notifyAll(resp);
                logMsg("WiFi: Test FAILED");
            }
        }
    }

    // MQTT reconnect only
    mqttReconnectIfNeeded();

    // WebSocket cleanup
    ws.cleanupClients();

    // Opóźniony zapis audio po zakończeniu serii zmian.
    flushAudioSettingsIfDirty();

    // OTA restart poza callbackiem HTTP
    if(otaRestartPending && millis() >= otaRestartAt) {
        ESP.restart();
    }

    // Restart po save_wifi (poza callbackem WebSocket)
    if(restartRequested && millis() >= restartAtMs) {
        ESP.restart();
    }

    // OTA async operations — wykonywane w loop() zamiast w callbacku async
    if(otaMutePending) {
        otaMutePending = false;
        audio.setVolume(0);
        logMsg("OTA: Audio muted (loop)");
    }
    if(otaDrawPercentPending) {
        otaDrawPercentPending = false;
        // reset=true przy pierwszym rysowaniu (index==0 w callbacku OTA ustawia to na true)
        showOTAOverlay(otaDrawPercentValue, otaOverlayReset);
        otaOverlayReset = false;
    }

    // Log free heap co 30 sekund
    unsigned long now = millis();
    if(now - lastHeapLogMs >= 30000) {
        lastHeapLogMs = now;
        logMsg("[HEAP] free=%u minFree=%u maxBlock=%u",
                      ESP.getFreeHeap(), ESP.getMinFreeHeap(), ESP.getMaxAllocHeap());
    }

    delay(50);  // Zmniejszone z 10ms - może zmniejszyć SPI interference
}

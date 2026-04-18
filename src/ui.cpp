// ui.cpp
#include "globals.h"
#include "config.h"
#include "icons.h"
#include "logo.h"

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
// Animowana kropka dla ekranu ładowania
static uint8_t wifiDotPhase = 0;

void showWifiOverlay(const String& line1, const String& line2, uint16_t color) {
    // Pełny ekran ładowania — czyścimy cały wyświetlacz
    tft.fillScreen(COLOR_BG);

    // Logo/tytuł na górze
    tft.setTextSize(1);
    tft.setTextColor(tft.color565(180, 180, 180));
    tft.setTextDatum(MC_DATUM);
    tft.drawString("MultiAMP", SCREEN_W / 2, 10);

    // Animowane kropki "łączenie..." (tylko w trybie STA)
    if(WiFi.getMode() == WIFI_STA || WiFi.getMode() == WIFI_AP_STA) {
        wifiDotPhase = (wifiDotPhase + 1) % 4;
        String dots = "";
        for(int i = 0; i < wifiDotPhase; i++) dots += ".";
        tft.setTextColor(tft.color565(100, 100, 100));
        tft.drawString("connecting" + dots, SCREEN_W / 2, 26);
    } else if(WiFi.getMode() == WIFI_AP) {
        // Tryb AP — pokaż instrukcję
        tft.setTextColor(tft.color565(100, 100, 100));
        tft.drawString("Setup mode", SCREEN_W / 2, 26);
    }

    // Główna linia statusu (np. nazwa sieci lub AP)
    tft.setTextColor(color);
    tft.drawString(line1, SCREEN_W / 2, 44);

    // Druga linia (np. IP, hasło AP)
    if(line2.length() > 0) {
        tft.setTextColor(tft.color565(140, 140, 140));
        tft.drawString(line2, SCREEN_W / 2, 58);
    }

    tft.setTextDatum(TL_DATUM);  // reset
}

void showIROverlay(const String& msg) {
    tft.fillRect(0, 0, SCREEN_W, 14, COLOR_BG);
    tft.setTextSize(1);
    tft.setTextColor(COLOR_GREEN);
    tft.setCursor(4, 3);
    tft.print(msg);
}

void showOTAOverlay(int percent, bool reset) {
    static int lastDrawn = -1;
    static int lastPercent = -1;

    if(reset) {
        lastDrawn = -1;
        lastPercent = -1;
    }

    // Pasek centralny — szeroki, wyraźny
    int barW = 220;
    int barH = 14;
    int barX = (SCREEN_W - barW) / 2;
    int barY = (SCREEN_H - barH) / 2 - 6;  // nieco wyżej od środka
    int fillW = barW * percent / 100;

    if (lastDrawn == -1) {
        // Czarny ekran — całkowicie czyścimy wyświetlacz
        tft.fillScreen(COLOR_BG);

        // Napis "UPDATING" nad paskiem
        tft.setTextSize(1);
        tft.setTextColor(tft.color565(150, 150, 150));
        int16_t tw, th;
        tft.setTextDatum(MC_DATUM);
        tft.drawString("UPDATING", SCREEN_W / 2, barY - 14);
        tft.setTextDatum(TL_DATUM);  // reset

        // Tło paska
        tft.fillRoundRect(barX, barY, barW, barH, 3, tft.color565(30, 30, 30));
        tft.drawRoundRect(barX, barY, barW, barH, 3, tft.color565(80, 80, 80));

        lastDrawn = 0;
        lastPercent = -1;
    }

    if (percent != lastPercent) {
        // Wypełnienie paska — zielony
        if (fillW > 0) {
            tft.fillRect(barX + 2, barY + 2, fillW - 4, barH - 4, COLOR_GREEN);
        }
        // Czyszczenie reszty paska (gdy fillW maleje — raczej nie, ale na wszelki wypadek)
        if (fillW < barW - 4) {
            tft.fillRect(barX + 2 + fillW, barY + 2, (barW - 4) - fillW, barH - 4, tft.color565(30, 30, 30));
        }

        // Procent pod paskiem
        tft.setTextSize(1);
        tft.setTextColor(COLOR_GREEN);
        tft.setTextDatum(MC_DATUM);
        String pctStr = String(percent) + "%";
        // Czyść poprzedni procent (małe pole)
        tft.fillRect(SCREEN_W / 2 - 20, barY + barH + 4, 40, 10, COLOR_BG);
        tft.drawString(pctStr, SCREEN_W / 2, barY + barH + 9);
        tft.setTextDatum(TL_DATUM);  // reset

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
    // Skala użytkowa volume: 0=cicho/mute, 56=maks.
    int percent = volume * 100 / 56;  // 0-100%
    
    // Gradient: pasek rośnie = głośniej
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
        
        int level = audioVolume;  // Skala użytkowa: 0=cicho/mute, 56=maks.
        int percent = level * 100 / 56;
        int fillW = (barW - 4) * level / 56;
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
        String bassStr = String(audioBass > 0 ? "+" : "") + String((int)audioBass);
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
        String trebStr = String(audioTreble > 0 ? "+" : "") + String((int)audioTreble);
        audioSprite.setTextColor(audioFocus == 2 ? trebColor : COLOR_GRAY);
        audioSprite.setCursor(SCREEN_W - trebStr.length() * 6 - 4, y3);
        audioSprite.print(trebStr);
        
        // Zapamiętaj wartości
        lastVolume = audioVolume;
        lastBass = audioBass;
        lastTreble = audioTreble;
    }
    
    // Push sprite na ekran - throttling dla zmniejszenia interference
    static unsigned long lastPush = 0;
    unsigned long now = millis();
    if(now - lastPush > 100) {  // Max 10fps zamiast 60fps
        audioSprite.pushSprite(0, 0);
        lastPush = now;
    }
    
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

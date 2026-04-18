# 🎛️ MultiAMP v4.2

**Profesjonalny 4-kanałowy wzmacniacz audio z procesorem PT2314E, wyświetlaczem LCD, pilotem IR i pełną integracją IoT**

> Poprzednia nazwa: "Audio Switcher". Zmieniona aby lepiej odzwierciedlać funkcję wzmacniacza.

![ESP32](https://img.shields.io/badge/ESP32-WROOM--32U-blue)
![PlatformIO](https://img.shields.io/badge/PlatformIO-compatible-orange)
![MQTT](https://img.shields.io/badge/MQTT-enabled-green)
![License](https://img.shields.io/badge/license-MIT-blue)

---

## 📋 Spis Treści

- [Funkcje](#-funkcje)
- [Hardware](#-hardware)
- [Struktura Projektu](#-struktura-projektu)
- [Instalacja](#-instalacja)
- [⚠️ Konfiguracja TFT_eSPI](#-konfiguracja-tft_espi)
- [Interfejs WWW](#-interfejs-www)
- [API MQTT](#-api-mqtt)
- [Pilot IR](#-pilot-ir)
- [Konfiguracja](#-konfiguracja)
- [Rozwiązywanie Problemów](#-rozwiązywanie-problemów)
- [Specyfikacje Techniczne](#-specyfikacje-techniczne)
- [Changelog](#-changelog)
- [Licencja](#-licencja)

---

## ✨ Funkcje

### Przetwarzanie Audio
- **4 wejścia audio** z płynnym przełączaniem
- **Procesor audio PT2314E**:
  - Kontrola głośności (0-56, skala użytkowa; biblioteka PT2314 wykonuje inwersję do komendy sprzętowej)
  - Kontrola basów (-14 do +14 dB, kroki 2 dB)
  - Kontrola tonów wysokich (-14 do +14 dB, kroki 2 dB)
- **Sterowanie przekaźnikami SSR** (4 kanały)
- **Trwałość NVS** - wszystkie ustawienia przetrwają restart
- **Opóźniony zapis audio** - chroni flash przed zużyciem przy szybkich regulacjach

### Wyświetlacz i Sterowanie
- **Wyświetlacz ST7789 LCD** (284x76) z Material Design
- **Sprite rendering** - płynne animacje bez migotania
- **Nakładka audio** z gradientami i połyskiem:
  - Volume: Zielony→Żółty→Czerwony
  - Bass/Treble: Cyan→Zielony→Pomarańczowy
  - Biały połysk na wszystkich paskach
- **Nauka pilota IR** - uniwersalne rozpoznawanie (NEC, RC6, Samsung, Sony, LG)
- **Krok głośności +3** na naciśnięcie - szybka regulacja bez repeat

### Łączność
- **WiFi STA** - łączy się z Twoją siecią domową
- **WiFi AP fallback** - tryb konfiguracji gdy STA nie działa
- **WebSocket** - synchronizacja w czasie rzeczywistym z parsowaniem JSON i walidacją komend
- **MQTT** - pełna dwukierunkowa kontrola z unikalnym ClientId (MAC-based)
- **OTA** - aktualizacje firmware przez WebUI z autoryzacją Basic Auth
- **Home Assistant ready** 🏠

---

## 🔧 Hardware

### Komponenty

| Komponent | Model | Pin |
|-----------|-------|-----|
| Mikrokontroler | ESP32-WROOM-32U | - |
| Procesor Audio | PT2314E | SDA=21, SCL=22 |
| Wyświetlacz | ST7789 284x76 | CS=5, DC=17, RST=16 |
| Odbiornik IR | VS1838B | OUT=19 |
| Przekaźniki | 4x SSR | 32, 33, 25, 26 |

---

## 📁 Struktura Projektu

```
src/
├── main.cpp              # setup(), loop() — główna pętla
├── config.h              # Piny, kolory, layouty, timeouty
├── globals.h / globals.cpp # Deklaracje zmiennych globalnych i helpery
├── storage.cpp           # NVS — loadPrefs, save*, opóźniony zapis audio
├── ui.cpp                # Cały UI na TFT — kafelki, overlaye, splash, ekran OTA
├── audio_ctrl.cpp        # Audio, relay, input switch
├── network.cpp           # WebSocket, OTA, MQTT, WiFi, serwer HTTP
├── PT2314.h / PT2314.cpp # Biblioteka PT2314E (I2C)
├── web.h                 # Interfejs WWW (PROGMEM)
├── icons.h               # Ikony wejść
└── logo.h                # Logo startup
```

> **Uwaga:** Kod był wcześniej w jednym pliku `main.cpp` (~2200 linii). Został podzielony na moduły dla lepszej utrzymywalności.

---

## 📦 Instalacja

### Szybki Start

```bash
git clone https://github.com/twojnick/audio-switcher.git
cd audio-switcher
pio run --target upload
```

### platformio.ini

```ini
[env:esp32-wroom-32]
platform = platformio/espressif32@6.12.0
board = esp32dev
framework = arduino
board_build.partitions = min_spiffs.csv

monitor_speed = 115200
monitor_filters = esp32_exception_decoder

lib_deps =
    https://github.com/crankyoldgit/IRremoteESP8266.git
    esphome/ESPAsyncWebServer-esphome@3.1.0
    https://github.com/OttoWinter/async-mqtt-client.git
    Bodmer/TFT_eSPI

build_flags =
    -D CORE_DEBUG_LEVEL=0
    -D CONFIG_ASYNC_TCP_RUNNING_CORE=1
    -D DECODE_RC6=1
```

> **Wymagana konfiguracja TFT_eSPI** — patrz sekcja poniżej!

---

## ⚠️ Konfiguracja TFT_eSPI

### KRYTYCZNE — Ręczna Konfiguracja Wymagana!

Wyświetlacz ST7789 284x76 wymaga **3 modyfikacji** w bibliotece TFT_eSPI.

**Lokalizacja biblioteki:**
```
~/.platformio/packages/framework-arduinoespressif32/libraries/TFT_eSPI/
```

---

### Modyfikacja 1: User_Setup.h

**Plik:** `User_Setup.h`

**Dodaj te linie (zakomentuj inne definicje wyświetlacza):**
```cpp
#define ST7789_DRIVER
#define TFT_WIDTH  284
#define TFT_HEIGHT 76
#define CGRAM_OFFSET

#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   5
#define TFT_DC   17
#define TFT_RST  16
#define TFT_BL   15

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SMOOTH_FONT

#define SPI_FREQUENCY  40000000
```

---

### Modyfikacja 2: TFT_eSPI.cpp — setRotation()

**Plik:** `TFT_eSPI.cpp`

**Znajdź `setRotation()` i dodaj na końcu:**
```cpp
void TFT_eSPI::setRotation(uint8_t m) {
    // ... istniejący kod ...
    
    // ST7789 284x76 FIX OFFSETU
    if (_init_width == 284 && _init_height == 76) {
        if (m == 1 || m == 3) {
            _addr_row_offset = 26;
            _addr_col_offset = 0;
        } else {
            _addr_row_offset = 0;
            _addr_col_offset = 26;
        }
    }
}
```

---

### Modyfikacja 3: TFT_eSPI.cpp — writecommand()

**Plik:** `TFT_eSPI.cpp`

**Znajdź `writecommand()` i dodaj:**
```cpp
void TFT_eSPI::writecommand(uint8_t c) {
    // ... istniejący kod ...
    
    // ST7789 284x76 SEKWENCJA INICJALIZACJI
    if (c == 0x11 && _init_width == 284) {
        delay(120);
        writecommand(0x36); writedata(0x00);
        writecommand(0x3A); writedata(0x55);
        writecommand(0xB2); writedata(0x0C); writedata(0x0C);
        writecommand(0xB7); writedata(0x35);
        writecommand(0xBB); writedata(0x1A);
        writecommand(0xC0); writedata(0x2C);
        writecommand(0xC2); writedata(0x01);
        writecommand(0xC3); writedata(0x0B);
        writecommand(0xC4); writedata(0x20);
        writecommand(0xC6); writedata(0x0F);
        writecommand(0xD0); writedata(0xA4); writedata(0xA1);
        writecommand(0xE0); 
        for (int i=0; i<14; i++) writedata(0x00);
        writecommand(0xE1);
        for (int i=0; i<14; i++) writedata(0x00);
        writecommand(0x21);
        writecommand(0x11);
        delay(120);
        writecommand(0x29);
        delay(120);
    }
}
```

---

## 🌐 Interfejs WWW

### Sekcje

| Sekcja | Opis |
|---------|-------------|
| **Dashboard** | 4 kafelki wejść z zintegrowanymi przełącznikami relay |
| **Audio** | Suwaki Volume, Bass, Treble (zawsze widoczne) |
| **Zakładka Settings** | Display (jasność, inwersja, obrót) + kody IR |
| **Zakładka Network** | Konfiguracja WiFi + MQTT |
| **Zakładka System** | Security (hasło admina), OTA, System Logs |

**Układ:** Dashboard + suwaki Audio na górze, reszta w 3 zakładkach poniżej.

### Kontrolki Logów

- **Enable logging** przełącznik — pokaż/ukryj logi
- **Refresh** przycisk — ręczne odświeżenie
- **Clear** przycisk — wyczyść bufor logów
- **Auto-refresh** checkbox — automatyczne aktualizacje
- **Interval** pole — ustaw częstotliwość odświeżania (1-30 sekund)

---

## 📡 API MQTT

### Topiki

```
audio_switcher/status      → "online" / "offline" (LWT)
audio_switcher/input       → 0-3
audio_switcher/volume      → 0-56
audio_switcher/bass        → -14 do +14
audio_switcher/treble      → -14 do +14

audio_switcher/set         → input: 0-3
audio_switcher/volume/set  → 0-56
audio_switcher/bass/set    → -14 do +14
audio_switcher/treble/set  → -14 do +14
```

### Przykład Home Assistant

```yaml
switch:
  - platform: mqtt
    name: "Audio Input 1"
    state_topic: "audio_switcher/input"
    command_topic: "audio_switcher/set"
    payload_on: '{"input":0}'
    payload_off: '{"input":0}'
    state_on: "0"
    state_off: "1"
```

---

## 🎮 Pilot IR

### Wspierane Protokoły

| Protokół | Status | Uwagi |
|----------|--------|-------|
| NEC | ✅ Pełny | Standardowe kody |
| RC6 | ✅ Pełny | Hash-based (obsługa toggle bit) |
| Samsung | ✅ Pełny | 32-bit |
| Sony | ✅ Pełny | 12/15/20-bit |
| LG | ✅ Pełny | 28-bit |

### Mapowanie Przycisków

| Funkcja | Domyślna Akcja |
|----------|---------------|
| Source | Następne wejście (0→1→2→3→0) |
| Input 1-4 | Bezpośredni wybór |
| Vol+ | Zwiększ głośność (+3) |
| Vol- | Zmniejsz głośność (-3) |
| ↑ | Poprzedni focus (Vol→Bass→Treble) |
| ↓ | Następny focus |

### Problem Toggle Bit w RC6

**Problem:** Toggle bit zmienia się przy każdym naciśnięciu.

**Rozwiązanie:** Hash z raw timing:
```cpp
// Zaokrąglenie do 100us + hash całego sygnału
for(int i = 5; i < rawlen; i++) {
    uint16_t val = (rawbuf[i] / 100) * 100;
    hash = (hash * 31 + val) & 0xFFFFFFFF;
}
```

**Efekt:** Ten sam przycisk = ten sam hash! 🎯

### Nawigacja Audio Overlay

```
> VOLUME   [████████████████        ] 75%  ← Zielona ramka
  BASS     [      ████              ] +5
  TREBLE   [    ██                  ] -2
```

- **↑/↓** - zmień focus
- **Vol+/-** - reguluj wartość
- **Auto-hide** po 5 sekundach

---

## ⚙️ Konfiguracja

### Pierwsze Uruchomienie

1. Włącz zasilanie → próba połączenia z zapisaną siecią WiFi
2. Jeśli brak zapisanej sieci lub połączenie nieudane → tryb AP
3. Połącz się z AP "AudioSwitcher" (hasło: `audio1234`)
4. Przejdź do http://192.168.4.1
5. Skonfiguruj WiFi → Zapisz → urządzenie restartuje się
6. Po restarcie → łączy się z Twoją siecią, pokazuje IP na wyświetlaczu

### MQTT

1. Włącz przełącznik MQTT
2. Host + port (domyślnie 1883)
3. Uwierzytelnianie (opcjonalne)
4. Prefix topicu
5. Zapisz → Status: Połączono ✅

> Zmiana konfiguracji MQTT działa natychmiast — bez restartu urządzenia.

### Nauka Pilota

1. Kliknij **Learn** przy funkcji
2. Naciśnij przycisk na pilocie (10s okno)
3. System zapisuje kod/hash
4. Powtórz dla wszystkich przycisków
5. Test: naciśnij Vol+ → powinien zwiększyć głośność 🔊

### Security

1. Przejdź do sekcji **Security** w Web UI
2. **Admin Password** — ustaw hasło do ochrony OTA i komend WebSocket (puste pole = brak ochrony)

> Hasło AP jest stałe (`audio1234`) i nie można go zmienić przez WebUI.

---

## 🐛 Rozwiązywanie Problemów

### Wyświetlacz

**Czarny ekran:**
- Sprawdź okablowanie SPI
- **Zweryfikuj konfigurację TFT_eSPI** (patrz sekcja wyżej!)
- Sprawdź `CGRAM_OFFSET` i offset w init()
- Test podświetlenia (GPIO15)

**Złe kolory:**
- Dodaj `tft.setSwapBytes(true)` w setup()

### IR

**Pilot nie działa:**
- GPIO19 poprawnie podłączone
- Zasilanie 5V odbiornika
- Sprawdź logi Serial - pokazują protokół i kod

**Ten sam przycisk — różne kody:**
- To normalne dla RC6 Mode 6 (toggle bit)
- System używa hash-based matching
- Hash powinien być stabilny

### WiFi / MQTT

**MQTT Status = Disconnected:**
- Sprawdź System Logs — powinien być `WiFi: Got IP` i `MQTT: Configured for ...`
- Upewnij się, że broker działa i jest osiągalny
- Sprawdź czy ClientId nie koliduje z innym urządzeniem
- Włącz/wyłącz MQTT w UI — reconnect następuje automatycznie

### OTA

**Upload OTA odrzucony (401 Unauthorized):**
- Wpisz hasło administratora w prompt w Web UI
- Lub ustaw hasło w sekcji Security

**Firmware nie bootuje:**
- Sprawdź `board_build.partitions = min_spiffs.csv`
- Logi powinny pokazać app0 ↔ app1 switching
- Jeśli zawsze app0 — problem z partycjami

---

## 📝 Specyfikacje Techniczne

### Audio
- **Pasmo:** 20Hz-20kHz
- **THD+N:** <0.1%
- **SNR:** >90dB
- **Skala głośności:** 0-56 (użytkowa), PT2314 inwertuje do 0-63 sprzętowych

### System
- **Boot:** ~3s
- **Switch wejścia:** <50ms
- **WebSocket:** <100ms
- **Display:** 60fps (sprite)
- **Log heapu:** co 30s w System Logs (`[HEAP] free=... minFree=... maxBlock=...`)

### IR
- **Tolerancja:** 30%
- **Buffer:** 300 raw values
- **Debounce:** 300ms
- **Krok głośności:** +3/-3 na naciśnięcie

### WiFi
- **Tryb:** STA najpierw, AP fallback
- **AP SSID:** `AudioSwitcher`
- **AP Hasło:** `audio1234` (stałe)

### Pamięć
- **Flash:** 4MB
- **RAM:** ~17.4% przy starcie (~57KB używane)
- **Firmware:** ~60.9% flash (~1.2MB)
- **Partycje:**
  - app0: 1.875MB
  - app1: 1.875MB (OTA)
  - SPIFFS: 192KB

---

## 📋 Changelog

### v4.2 — Obecna wersja

#### 🆕 Nowe
- 🎛️ **Rebranding** — "Audio Switcher" → "MultiAMP"
- 🖥️ **Dashboard WebUI** — przeprojektowany układ: dashboard (wejścia + relaye) + suwaki audio zawsze widoczne, reszta w 3 zakładkach (Settings/Network/System)
- 🖥️ **Status dots** — wskaźniki połączenia WiFi + MQTT w nagłówku
- 🔒 **Security** — hasło administratora (OTA Basic Auth, autoryzacja WebSocket)
- 🌐 **Tryby WiFi STA/AP** — STA najpierw, AP fallback do konfiguracji
- 🌐 **Asynchroniczny test WiFi** — nie blokuje interfejsu
- 🔄 **Asynchroniczny restart** po zapisie WiFi — odpowiedź wysyłana przed restartem
- 🛡️ **Walidacja danych NVS** — wszystkie wartości clampowane do poprawnych zakresów
- 💾 **Opóźniony zapis audio** — ochrona flash przy szybkich regulacjach (1500ms delay)
- 📡 **MQTT** — unikalny ClientId z MAC, walidacja topicu, buforowanie fragmentów
- 🔧 **Walidacja topicu MQTT** — usuwa znaki problematyczne (`#`, `+`, `$`)
- 🖥️ **System Logs** — logi na żywo w WebUI z włącz/wyłącz, auto-odświeżaniem, kontrolą interwału
- 🖥️ **Ekran OTA fullscreen** — czarny ekran z paskiem postępu na środku
- 🖥️ **Ekran ładowania WiFi** — czysty fullscreen overlay ze statusem

#### 🔧 Poprawki
- ✅ **Kierunek Vol+/Vol-** — poprawiony w IR
- ✅ **Skala Volume** — ujednolicona 0-56 (0=cicho, 56=max)
- ✅ **Krok głośności** — +3/-3 na naciśnięcie (bez repeat)
- ✅ **Skala Bass/Treble** — API PT2314 przyjmuje dB (-14..+14), mapowanie wewnętrzne
- ✅ **Bass/Treble WebUI fix** — rzutowanie `int8_t` → `int` przed `String()` w JSON (wcześniej wysyłało znaki ASCII zamiast liczb)
- ✅ **Reset Bass/Treble** — sanity check przy starcie resetuje wartości poza zakresem
- ✅ **Soft-start audio** — `audio.setVolume(0)` zamiast błędnego `setVolume(63)`
- ✅ **Rejestr treble PT2314** — poprawiono `0x7F` → `0x77` dla 0dB
- ✅ **OTA upload** — przywrócono `FormData` z polem `update` (multipart/form-data wymagany przez ESPAsyncWebServer)
- ✅ **WiFi auto-reconnect** — `WiFi.setAutoReconnect(true)` + `WiFi.persistent(false)` zapobiega zużyciu flash
- ✅ **WiFi reconnect IP fix** — `localIP` aktualizuje się poprawnie po reconnect
- ✅ **MQTT disconnect przy utracie WiFi** — `mqttClient.disconnect()` wywoływane na `STA_DISCONNECTED`
- ✅ **Eventy WiFi** — nowe nazwy `ARDUINO_EVENT_*` dla Arduino-ESP32 v3.x
- ✅ **OTA** — sprawdzanie nullptr partycji, odrzucanie uploadu z zerowym rozmiarem
- ✅ **OTA restart** — przeniesiony z callbacku HTTP do `loop()`
- ✅ **Parser WebSocket** — ekstraktory `extractJsonString/Int/UInt/Boolish`, odpowiedzi błędów
- ✅ **JSON escape** — `jsonEscape()` dla pól tekstowych w statusie WebSocket
- ✅ **WiFi test** — async w `loop()` zamiast blokującej pętli w callbacku
- ✅ **Double-trigger IR** — debounce 150ms zapobiega podwójnym akcjom

#### 🏗️ Architektura
- **Podział na moduły** — `config.h`, `globals.cpp`, `storage.cpp`, `ui.cpp`, `audio_ctrl.cpp`, `network.cpp`
- **PlatformIO** — przypięte wersje: `espressif32@6.12.0`, `ESPAsyncWebServer@3.1.0`

---

## 📄 Licencja

MIT License — zobacz [LICENSE](LICENSE)

---

## 🙏 Podziękowania

- **Bodmer** — TFT_eSPI
- **crankyoldgit** — IRremoteESP8266
- **me-no-dev** — ESPAsyncWebServer
- **marvinroger** — AsyncMqttClient
- **ua6em** — ST7789 284x76 offsets discovery

---

## 🔗 Linki

- **TFT_eSPI Setup:** [Forum Arduino](https://forum.arduino.cc/t/tft-espi-setup-for-st7789p3-2-25-76x284-tft/1407473/14)
- **IRremoteESP8266:** [GitHub](https://github.com/crankyoldgit/IRremoteESP8266)

---

**Stworzone z ❤️ używając ESP32**

⭐ **Gwiazdka w repo jeśli uznasz to za przydatne!**

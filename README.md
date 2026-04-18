# 🎛️ MultiAMP v4.2

**Professional 4-channel audio amplifier with PT2314E processor, LCD display, IR remote and full IoT integration**

> Previously known as "Audio Switcher". Renamed to reflect the amplifier functionality.

![ESP32](https://img.shields.io/badge/ESP32-WROOM--32U-blue)
![PlatformIO](https://img.shields.io/badge/PlatformIO-compatible-orange)
![MQTT](https://img.shields.io/badge/MQTT-enabled-green)
![License](https://img.shields.io/badge/license-MIT-blue)

---

## 📋 Table of Contents

- [Features](#-features)
- [Hardware](#-hardware)
- [Project Structure](#-project-structure)
- [Installation](#-installation)
- [⚠️ TFT_eSPI Configuration](#-tft_espi-configuration)
- [Web Interface](#-web-interface)
- [MQTT API](#-mqtt-api)
- [IR Remote](#-ir-remote)
- [Configuration](#-configuration)
- [Troubleshooting](#-troubleshooting)
- [Technical Specifications](#-technical-specifications)
- [Changelog](#-changelog)
- [License](#-license)

---

## ✨ Features

### Audio Processing
- **4 audio inputs** with seamless switching
- **PT2314E audio processor**:
  - Volume control (0-56 user scale; PT2314 library inverts to hardware command)
  - Bass control (-14 to +14 dB, 2 dB steps)
  - Treble control (-14 to +14 dB, 2 dB steps)
- **SSR relay control** (4 channels)
- **NVS persistence** — all settings survive restart
- **Deferred audio save** — protects flash from wear during rapid adjustments

### Display and Control
- **ST7789 LCD Display** (284x76) with Material Design
- **Sprite rendering** — smooth animations without flicker
- **Audio overlay** with gradients and gloss:
  - Volume: Green→Yellow→Red
  - Bass/Treble: Cyan→Green→Orange
  - White gloss on all bars
- **IR remote learning** — universal recognition (NEC, RC6, Samsung, Sony, LG)
- **Volume step +3** per button press — fast adjustment without repeat

### Connectivity
- **WiFi STA mode** — connects to your home network
- **WiFi AP fallback** — configuration mode when STA fails
- **WebSocket** — real-time synchronization with JSON parsing and command validation
- **MQTT** — full bidirectional control with unique MAC-based ClientId
- **OTA** — firmware updates via WebUI with Basic Auth
- **Home Assistant ready** 🏠

---

## 🔧 Hardware

### Components

| Component | Model | Pin |
|-----------|-------|-----|
| Microcontroller | ESP32-WROOM-32U | - |
| Audio Processor | PT2314E | SDA=21, SCL=22 |
| Display | ST7789 284x76 | CS=5, DC=17, RST=16 |
| IR Receiver | VS1838B | OUT=19 |
| Relays | 4x SSR | 32, 33, 25, 26 |

---

## 📁 Project Structure

```
src/
├── main.cpp              # setup(), loop() — main loop
├── config.h              # Pins, colors, layouts, timeouts
├── globals.h / globals.cpp # Global variable declarations and helpers
├── storage.cpp           # NVS — loadPrefs, save*, deferred audio save
├── ui.cpp                # Full TFT UI — tiles, overlays, splash, OTA screen
├── audio_ctrl.cpp        # Audio, relay, input switch
├── network.cpp           # WebSocket, OTA, MQTT, WiFi, HTTP server
├── PT2314.h / PT2314.cpp # PT2314E library (I2C)
├── web.h                 # Web interface (PROGMEM)
├── icons.h               # Input icons
└── logo.h                # Startup logo
```

> **Note:** Code was previously in a single `main.cpp` (~2200 lines). It has been split into modules for better maintainability.

---

## 📦 Installation

### Quick Start

```bash
git clone https://github.com/yourusername/audio-switcher.git
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

> **TFT_eSPI configuration required** — see section below!

---

## ⚠️ TFT_eSPI Configuration

### CRITICAL — Manual Configuration Required!

The ST7789 284x76 display requires **3 modifications** to the TFT_eSPI library.

**Library location:**
```
~/.platformio/packages/framework-arduinoespressif32/libraries/TFT_eSPI/
```

---

### Modification 1: User_Setup.h

**File:** `User_Setup.h`

**Add these lines (comment out other display definitions):**
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

### Modification 2: TFT_eSPI.cpp — setRotation()

**File:** `TFT_eSPI.cpp`

**Find `setRotation()` and add at the end:**
```cpp
void TFT_eSPI::setRotation(uint8_t m) {
    // ... existing code ...
    
    // ST7789 284x76 OFFSET FIX
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

### Modification 3: TFT_eSPI.cpp — writecommand()

**File:** `TFT_eSPI.cpp`

**Find `writecommand()` and add:**
```cpp
void TFT_eSPI::writecommand(uint8_t c) {
    // ... existing code ...
    
    // ST7789 284x76 INIT SEQUENCE
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

## 🌐 Web Interface

### Sections

| Section | Description |
|---------|-------------|
| **Dashboard** | 4 input tiles with integrated relay toggles |
| **Audio** | Volume, Bass, Treble sliders (always visible) |
| **Settings Tab** | Display (brightness, invert, rotate) + IR codes |
| **Network Tab** | WiFi config + MQTT setup |
| **System Tab** | Security (admin password), OTA, System Logs |

**Layout:** Dashboard + Audio sliders at top, everything else organized in 3 tabs below.

### System Logs Controls

- **Enable logging** toggle — show/hide logs
- **Refresh** button — manual refresh
- **Clear** button — clear log buffer
- **Auto-refresh** checkbox — automatic updates
- **Interval** field — set refresh rate (1-30 seconds)

---

## 📡 MQTT API

### Topics

```
audio_switcher/status      → "online" / "offline" (LWT)
audio_switcher/input       → 0-3
audio_switcher/volume      → 0-56
audio_switcher/bass        → -14 to +14
audio_switcher/treble      → -14 to +14

audio_switcher/set         → input: 0-3
audio_switcher/volume/set  → 0-56
audio_switcher/bass/set    → -14 to +14
audio_switcher/treble/set  → -14 to +14
```

### Home Assistant Example

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

## 🎮 IR Remote

### Supported Protocols

| Protocol | Status | Notes |
|----------|--------|-------|
| NEC | ✅ Full | Standard codes |
| RC6 | ✅ Full | Hash-based (toggle bit handling) |
| Samsung | ✅ Full | 32-bit |
| Sony | ✅ Full | 12/15/20-bit |
| LG | ✅ Full | 28-bit |

### Button Mapping

| Function | Default Action |
|----------|---------------|
| Source | Next input (0→1→2→3→0) |
| Input 1-4 | Direct select |
| Vol+ | Increase volume (+3) |
| Vol- | Decrease volume (-3) |
| ↑ | Previous focus (Vol→Bass→Treble) |
| ↓ | Next focus |

### RC6 Toggle Bit Problem

**Problem:** Toggle bit changes with each press.

**Solution:** Hash from raw timing:
```cpp
// Round to 100us + hash entire signal
for(int i = 5; i < rawlen; i++) {
    uint16_t val = (rawbuf[i] / 100) * 100;
    hash = (hash * 31 + val) & 0xFFFFFFFF;
}
```

**Result:** Same button = same hash! 🎯

### Audio Overlay Navigation

```
> VOLUME   [████████████████        ] 75%  ← Green frame
  BASS     [      ████              ] +5
  TREBLE   [    ██                  ] -2
```

- **↑/↓** - change focus
- **Vol+/-** - adjust value
- **Auto-hide** after 5 seconds

---

## ⚙️ Configuration

### First Boot

1. Power on → attempts to connect to saved WiFi
2. If no saved network or connection fails → AP mode
3. Connect to AP "AudioSwitcher" (password: `audio1234`)
4. Navigate to http://192.168.4.1
5. Configure WiFi → Save → device restarts
6. After restart → connects to your network, shows IP on display

### MQTT Setup

1. Enable MQTT toggle
2. Host + port (default 1883)
3. Authentication (optional)
4. Topic prefix
5. Save → Status: Connected ✅

> MQTT config changes apply immediately — no device restart needed.

### IR Remote Learning

1. Click **Learn** next to function
2. Press button on remote (10s window)
3. System saves code/hash
4. Repeat for all buttons
5. Test: press Vol+ → should increase volume 🔊

### Security

1. Go to **Security** section in Web UI
2. **Admin Password** — set password to protect OTA and WebSocket commands (empty = no protection)

> AP password is fixed (`audio1234`) and cannot be changed via WebUI.

---

## 🐛 Troubleshooting

### Display Issues

**Black screen:**
- Check SPI wiring
- **Verify TFT_eSPI configuration** (see section above!)
- Check `CGRAM_OFFSET` and offsets in init()
- Test backlight (GPIO15)

**Wrong colors:**
- Add `tft.setSwapBytes(true)` in setup()

### IR Issues

**Remote doesn't work:**
- GPIO19 correctly connected
- 5V power to receiver
- Check Serial logs — shows protocol and code

**Same button — different codes:**
- Normal for RC6 Mode 6 (toggle bit)
- System uses hash-based matching
- Hash should be stable

### WiFi / MQTT

**MQTT Status = Disconnected:**
- Check System Logs — should show `WiFi: Got IP` and `MQTT: Configured for ...`
- Make sure broker is running and reachable
- Check if ClientId doesn't conflict with another device
- Toggle MQTT off/on in UI — reconnect happens automatically

### OTA Issues

**Upload rejected (401 Unauthorized):**
- Enter admin password in the Web UI prompt
- Or set password in Security section

**Firmware doesn't boot:**
- Check `board_build.partitions = min_spiffs.csv`
- Logs should show app0 ↔ app1 switching
- If always app0 — partition problem

---

## 📝 Technical Specifications

### Audio
- **Bandwidth:** 20Hz-20kHz
- **THD+N:** <0.1%
- **SNR:** >90dB
- **Volume scale:** 0-56 (user), PT2314 inverts to 0-63 hardware

### System
- **Boot:** ~3s
- **Switch time:** <50ms
- **WebSocket:** <100ms
- **Display:** 60fps (sprite)
- **Heap logging:** every 30s in System Logs (`[HEAP] free=... minFree=... maxBlock=...`)

### IR
- **Tolerance:** 30%
- **Buffer:** 300 raw values
- **Debounce:** 300ms
- **Volume step:** +3/-3 per press

### WiFi
- **Mode:** STA first, AP fallback
- **AP SSID:** `AudioSwitcher`
- **AP Password:** `audio1234` (fixed)

### Memory
- **Flash:** 4MB
- **RAM:** ~17.4% at startup (~57KB used)
- **Firmware:** ~60.9% flash (~1.2MB)
- **Partitions:**
  - app0: 1.875MB
  - app1: 1.875MB (OTA)
  - SPIFFS: 192KB

---

## 📋 Changelog

### v4.2 — Current Version

#### 🆕 New
- 🎛️ **Rebranding** — "Audio Switcher" → "MultiAMP"
- 🖥️ **Dashboard WebUI** — redesigned layout: dashboard (inputs + relays) + audio sliders always visible, rest organized in 3 tabs (Settings/Network/System)
- 🖥️ **Status dots** — WiFi + MQTT connection status in header
- 🔒 **Security** — admin password (OTA Basic Auth, WebSocket authorization)
- 🌐 **WiFi STA/AP modes** — STA first, AP fallback for configuration
- 🌐 **Async WiFi test** — doesn't block the interface
- 🔄 **Async restart** after WiFi save — response sent before restart
- 🛡️ **NVS data validation** — all values clamped to valid ranges
- 💾 **Deferred audio save** — flash protection during rapid adjustments (1500ms delay)
- 📡 **MQTT** — unique MAC-based ClientId, topic validation, payload fragmentation
- 🔧 **MQTT topic validation** — removes problematic chars (`#`, `+`, `$`)
- 🖥️ **System Logs** — live logs in WebUI with enable/disable, auto-refresh, interval control
- 🖥️ **OTA fullscreen UI** — black screen with centered progress bar
- 🖥️ **WiFi loading screen** — clean fullscreen overlay with status

#### 🔧 Fixes
- ✅ **Vol+/Vol- direction** — fixed in IR
- ✅ **Volume scale** — unified 0-56 (0=mute, 56=max)
- ✅ **Volume step** — +3/-3 per press (no repeat needed)
- ✅ **Bass/Treble scale** — PT2314 API accepts dB (-14..+14), internal mapping
- ✅ **Bass/Treble WebUI fix** — `int8_t` → `int` cast before `String()` in JSON (was sending ASCII chars instead of numbers)
- ✅ **Bass/Treble reset** — sanity check on boot resets out-of-range values
- ✅ **Audio soft-start** — `audio.setVolume(0)` instead of broken `setVolume(63)`
- ✅ **PT2314 treble register** — fixed `0x7F` → `0x77` for 0dB
- ✅ **OTA upload** — restored `FormData` with `update` field (multipart/form-data required by ESPAsyncWebServer)
- ✅ **WiFi auto-reconnect** — `WiFi.setAutoReconnect(true)` + `WiFi.persistent(false)` prevents flash wear
- ✅ **WiFi reconnect IP fix** — `localIP` now updates correctly after WiFi reconnect
- ✅ **MQTT disconnect on WiFi loss** — `mqttClient.disconnect()` called on `STA_DISCONNECTED`
- ✅ **WiFi events** — new `ARDUINO_EVENT_*` names for Arduino-ESP32 v3.x
- ✅ **OTA** — nullptr partition checks, reject zero-size uploads
- ✅ **OTA restart** — moved from HTTP callback to `loop()`
- ✅ **WebSocket parser** — `extractJsonString/Int/UInt/Boolish` extractors, error responses
- ✅ **JSON escape** — `jsonEscape()` for text fields in WebSocket status
- ✅ **WiFi test** — async in `loop()` instead of blocking loop in callback
- ✅ **IR double-trigger** — 150ms debounce prevents duplicate actions

#### 🏗️ Architecture
- **Modular split** — `config.h`, `globals.cpp`, `storage.cpp`, `ui.cpp`, `audio_ctrl.cpp`, `network.cpp`
- **PlatformIO** — pinned versions: `espressif32@6.12.0`, `ESPAsyncWebServer@3.1.0`

---

## 📄 License

MIT License — see [LICENSE](LICENSE)

---

## 🙏 Acknowledgments

- **Bodmer** — TFT_eSPI library
- **crankyoldgit** — IRremoteESP8266
- **me-no-dev** — ESPAsyncWebServer
- **marvinroger** — AsyncMqttClient
- **ua6em** — ST7789 284x76 offsets discovery

---

## 🔗 Links

- **TFT_eSPI Setup:** [Arduino Forum](https://forum.arduino.cc/t/tft-espi-setup-for-st7789p3-2-25-76x284-tft/1407473/14)
- **IRremoteESP8266:** [GitHub](https://github.com/crankyoldgit/IRremoteESP8266)

---

**Made with ❤️ using ESP32**

⭐ **Star this repo if you find it useful!**

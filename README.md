# 🎛️ Audio Switcher v4.2

**Professional 4-channel audio switcher with PT2314E processor, LCD display, IR remote with auto-repeat and full IoT integration**

![ESP32](https://img.shields.io/badge/ESP32-WROOM--32U-blue)
![PlatformIO](https://img.shields.io/badge/PlatformIO-compatible-orange)
![MQTT](https://img.shields.io/badge/MQTT-enabled-green)
![License](https://img.shields.io/badge/license-MIT-blue)

---

## 📋 Table of Contents

- [Features](#-features)
- [Hardware](#-hardware)
- [Installation](#-installation)
- [⚠️ TFT_eSPI Configuration](#-tft_espi-configuration)
- [Web Interface](#-web-interface)
- [MQTT API](#-mqtt-api)
- [IR Remote](#-ir-remote)
- [Configuration](#-configuration)
- [Troubleshooting](#-troubleshooting)
- [Technical Specifications](#-technical-specifications)
- [Changelog v4.2](#-changelog-v42)
- [License](#-license)

---

## ✨ Features

### Audio Processing
- **4 audio inputs** with seamless switching
- **PT2314E audio processor**:
  - Volume control (0-63, 0.25dB steps)
  - Bass control (-14 to +14 dB)
  - Treble control (-14 to +14 dB)
- **SSR relay control** (4 channels)
- **NVS persistence** - all settings survive restart

### Display and Control
- **ST7789 LCD Display** (284x76) with Material Design
- **Sprite rendering** - smooth animations without flicker
- **Audio overlay** with gradients and gloss:
  - Volume: Green→Yellow→Red
  - Bass/Treble: Cyan→Green→Orange
  - White gloss on all bars
- **IR remote learning** - universal recognition (NEC, RC6, Samsung, Sony, LG)
- **Auto-repeat** for Vol+/Vol-/↑/↓ - hold for continuous adjustment

### Connectivity
- **Dual WiFi** (AP + STA)
- **WebSocket** - real-time synchronization
- **MQTT** - full bidirectional control
- **OTA** - firmware updates via WebUI
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

## 📦 Installation

### Quick Start

```bash
git clone https://github.com/yourusername/audio-switcher.git
cd audio-switcher
pio lib install
# IMPORTANT: Configure TFT_eSPI (see below!)
pio run --target upload
```

### platformio.ini

```ini
[env:esp32-wroom-32]
platform = espressif32
board = esp32dev
framework = arduino
board_build.partitions = min_spiffs.csv

lib_deps =
    bodmer/TFT_eSPI@^2.5.43
    crankyoldgit/IRremoteESP8266@^2.8.6
    esphome/ESPAsyncWebServer-esphome@^3.1.0
    https://github.com/OttoWinter/async-mqtt-client.git
```

---

## ⚠️ TFT_eSPI Configuration

### CRITICAL - Manual Configuration Required!

The ST7789 284x76 display requires **3 modifications** to the TFT_eSPI library.

**Library location:**
```
~/.platformio/packages/framework-arduinoespressif32/libraries/TFT_eSPI/
```

---

### Modification 1: User_Setup.h

**File:** `User_Setup.h`

**1. Uncomment driver:**
```cpp
#define ST7789_DRIVER
```

**2. Set resolution:**
```cpp
#define TFT_WIDTH  76
#define TFT_HEIGHT 284
```

---

### Modification 2: ST7789_Defines.h (OFFSET)

**File:** `TFT_Drivers/ST7789_Defines.h`

**Add at the end of file:**
```cpp
// ALI ST7789P3 2.25 TFT support (always use offset)
#if (TFT_HEIGHT == 284) && (TFT_WIDTH == 76)
  #ifndef CGRAM_OFFSET
    #define CGRAM_OFFSET
  #endif
#endif
```

**Why:** This display requires automatic offset for correct pixel addressing.

---

### Modification 3: ST7789_Rotation.h (CUSTOM OFFSETS)

**File:** `TFT_Drivers/ST7789_Rotation.h`

**Find the `switch(rotation)` section and add to case 1 and case 3:**

```cpp
switch (rotation) {
  case 0: // Portrait
    // ... existing code ...
    break;

  case 1: // Landscape (Portrait + 90)
    // ... existing code ...
    
    // ADD THIS:
    else if (_init_width == 76)
    {
      colstart = 18;
      rowstart = 82;
    }
    break;

  case 2: // Inverted portrait
    // ... existing code ...
    break;

  case 3: // Inverted landscape
    // ... existing code ...
    
    // ADD THIS:
    else if (_init_width == 76)
    {
      colstart = 18;
      rowstart = 82;
    }
    break;
}
```

**Why:** These offsets (18, 82) align the image on the physical 76x284 display.

---

### Verification in Project

**In `main.cpp`:**
```cpp
tft.init();
tft.setSwapBytes(true);   // IMPORTANT for RGB565!
tft.setRotation(3);       // Uses case 3 with offsets
```

---

### ⚠️ Library Update Warning

**Problem:** Updating `TFT_eSPI` will overwrite these changes!

**Solutions:**
1. **Document these changes** (keep in project README)
2. **After update** reapply modifications
3. **Or:** Lock library version in `platformio.ini`:
   ```ini
   bodmer/TFT_eSPI@2.5.43  # Locked version
   ```

---

### 🔗 Sources and Credits

- **ua6em** - discovered offsets for this display
- **Arduino Forum:** [ST7789P3 2.25" Setup](https://forum.arduino.cc/t/tft-espi-setup-for-st7789p3-2-25-76x284-tft/1407473/14)
- **TFT_eSPI GitHub:** [bodmer/TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)

---

## 🌐 Web Interface

### Access
- **AP:** http://192.168.4.1 (SSID: AudioSwitcher, password: audio1234)
- **STA:** http://[device-ip]

### Features
- 🎵 **4 input tiles** with depth effect
- 🎚️ **Audio sliders** with gradients (Volume, Bass, Treble)
- 🔌 **Relays** - toggle for each input
- 📡 **IR learning** - all protocols
- 📶 **WiFi configuration** with connection test
- 🔗 **MQTT** - broker, auth, prefix
- 🔄 **OTA** - drag & drop firmware with progressbar

---

## 📡 MQTT API

### Subscribed Topics (Commands)

```bash
# Switch input (1-4)
mosquitto_pub -t audio_switcher/set -m "2"

# Set volume (0-100%)
mosquitto_pub -t audio_switcher/volume/set -m "75"

# Set bass (-14 to +14 dB)
mosquitto_pub -t audio_switcher/bass/set -m "5"

# Set treble (-14 to +14 dB)
mosquitto_pub -t audio_switcher/treble/set -m "-3"
```

### Published Topics (Status)

```yaml
audio_switcher/status:  "online"/"offline" (LWT, retained)
audio_switcher/state:   1-4 (retained)
audio_switcher/audio:   {"volume":75,"volume_raw":15,"bass":5,"treble":-2}
```

### Home Assistant Integration

```yaml
mqtt:
  number:
    - name: "Audio Volume"
      command_topic: "audio_switcher/volume/set"
      state_topic: "audio_switcher/audio"
      value_template: "{{ value_json.volume }}"
      min: 0
      max: 100
```

---

## 🎮 IR Remote

### Supported Protocols
✅ NEC, RC6, Samsung, Sony, RC5, LG, Panasonic  
✅ **RC6 Mode 6** - hash-based matching (ignores toggle bit)  
✅ **Auto-repeat** for Vol+/Vol-/↑/↓

### Functions

| Button | Action | Auto-Repeat |
|--------|--------|-------------|
| Source | Switch inputs (1→2→3→4) | ❌ |
| Input 1-4 | Direct selection | ❌ |
| Vol+ | Increase parameter | ✅ 500ms + 150ms |
| Vol- | Decrease parameter | ✅ 500ms + 150ms |
| ↑ | Focus up | ✅ 500ms + 150ms |
| ↓ | Focus down | ✅ 500ms + 150ms |

### RC6 Mode 6 - Special Handling

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
- **Auto-hide** after 2 seconds

---

## ⚙️ Configuration

### First Boot

1. Power on → WiFi AP "AudioSwitcher"
2. Connect (password: audio1234)
3. Navigate to http://192.168.4.1
4. Configure WiFi → Test → Save
5. Restart → connects to network

### MQTT Setup

1. Enable MQTT toggle
2. Host + port (default 1883)
3. Authentication (optional)
4. Topic prefix
5. Save → Status: Connected ✅

### IR Remote Learning

1. Click **Learn** next to function
2. Press button on remote (10s window)
3. System saves code/hash
4. Repeat for all buttons
5. Test: press Vol+ → should increase volume 🔊

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
- Check Serial logs - shows protocol and code

**Same button - different codes:**
- Normal for RC6 Mode 6 (toggle bit)
- System uses hash-based matching
- Hash should be stable

**Auto-repeat doesn't work:**
- Check if learned as Vol+/Vol-/↑/↓
- Hold >500ms

### OTA Issues

**Firmware doesn't boot:**
- Check `board_build.partitions = min_spiffs.csv`
- Logs should show app0 ↔ app1 switching
- If always app0 - partition problem

---

## 📝 Technical Specifications

### Audio
- **Bandwidth:** 20Hz-20kHz
- **THD+N:** <0.1%
- **SNR:** >90dB

### System
- **Boot:** ~3s
- **Switch:** <50ms
- **WebSocket:** <100ms
- **Display:** 60fps (sprite)

### IR
- **Tolerance:** 30%
- **Buffer:** 300 raw values
- **Debounce:** 300ms
- **Auto-repeat:** 500ms + 150ms rate

### Memory
- **Flash:** 4MB
- **Partitions:**
  - app0: 1.875MB
  - app1: 1.875MB (OTA)
  - SPIFFS: 192KB

---

## 📋 Changelog v4.2

### 🆕 New Features
- ✨ Migration to TFT_eSPI
- 🎨 Sprite rendering (60fps, zero flicker)
- 🌈 Gradient + gloss on bars
- 📡 Universal IR (NEC, RC6, Samsung, Sony)
- 🔄 Auto-repeat Vol+/Vol-/↑/↓
- 🎯 RC6 Mode 6 hash-based matching
- 🚀 OTA with gradient progressbar

### 🔧 Fixes
- ✅ RC6 toggle bit handling
- ✅ OTA partition switching
- ✅ MQTT callback deduplication
- ✅ Audio persistence from all sources
- ✅ FormData for OTA upload

### 🎨 Visual Improvements
- Rounded corners (radius 4px)
- Triple border on active tiles
- Green frame on active bar
- Gradient boost 0-40
- White gloss effect

---

## 📄 License

MIT License - see [LICENSE](LICENSE)

---

## 🙏 Acknowledgments

- **Bodmer** - TFT_eSPI library
- **crankyoldgit** - IRremoteESP8266
- **me-no-dev** - ESPAsyncWebServer
- **marvinroger** - AsyncMqttClient
- **ua6em** - ST7789 284x76 offsets discovery

---

## 🔗 Links

- **TFT_eSPI Setup:** [Arduino Forum](https://forum.arduino.cc/t/tft-espi-setup-for-st7789p3-2-25-76x284-tft/1407473/14)
- **IRremoteESP8266:** [GitHub](https://github.com/crankyoldgit/IRremoteESP8266)

---

**Made with ❤️ using ESP32**

⭐ **Star this repo if you find it useful!**

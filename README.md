# 🎛️ Audio Switcher v4.2

**Professional 4-channel audio switcher with PT2314E processor, LCD display, IR remote, and full IoT integration**

![ESP32](https://img.shields.io/badge/ESP32-WROOM--32U-blue)
![PlatformIO](https://img.shields.io/badge/PlatformIO-compatible-orange)
![MQTT](https://img.shields.io/badge/MQTT-enabled-green)
![License](https://img.shields.io/badge/license-MIT-blue)

---

## 📋 Table of Contents

- [Features](#-features)
- [Hardware](#-hardware)
- [Software Architecture](#-software-architecture)
- [Installation](#-installation)
- [Web Interface](#-web-interface)
- [MQTT API](#-mqtt-api)
- [IR Remote Control](#-ir-remote-control)
- [Configuration](#-configuration)
- [Troubleshooting](#-troubleshooting)
- [License](#-license)

---

## ✨ Features

### Audio Processing
- **4 audio inputs** with seamless switching
- **PT2314E audio processor** with:
  - Volume control (0-63, 0.25dB steps)
  - Bass control (-14 to +14 dB)
  - Treble control (-14 to +14 dB)
- **SSR relay control** (4 channels) for external equipment
- **NVS persistence** - all settings survive power cycles

### Display & Control
- **ST7789 LCD** (284x76) with Material Design UI
- **Real-time audio overlay** with:
  - Volume, Bass, Treble visualization
  - Gradient color coding (green → yellow → red)
  - 4-button control (Vol+/Vol-, ↑/↓ focus)
  - 2-second timeout to tiles view
- **IR remote learning** - program any remote control
- **LED backlight** with PWM brightness control

### Connectivity
- **Dual WiFi** (AP mode for setup + STA mode for network)
- **WebSocket real-time sync** between all control methods
- **MQTT integration** with full bidirectional control
- **OTA firmware updates** via web interface
- **Material Design WebUI** optimized for mobile & desktop

### Smart Home Integration
- **Home Assistant** ready
- **Node-RED** compatible
- **MQTT auto-discovery** support
- **RESTful status API**

---

## 🔧 Hardware

### Components

| Component | Model | Purpose |
|-----------|-------|---------|
| MCU | ESP32-WROOM-32U | Main controller |
| Audio Processor | PT2314E | Volume/Bass/Treble control |
| Display | ST7789 284x76 | Visual feedback |
| IR Receiver | VS1838B (or compatible) | Remote control input |
| Relays | 4x SSR (high-level trigger) | External equipment control |
| Power | 5V/2A | System power supply |

### Pin Configuration

```cpp
// Display (SPI)
TFT_CS    = 5
TFT_DC    = 17
TFT_RST   = 16
TFT_MOSI  = 23
TFT_SCLK  = 18
TFT_BL    = 15  // PWM backlight

// Audio (I2C)
PT_SDA    = 21
PT_SCL    = 22

// Control
IR_PIN    = 19

// Relays (SSR high-level trigger)
RELAY_1   = 32
RELAY_2   = 33
RELAY_3   = 25
RELAY_4   = 26
```

### Wiring Diagram

```
ESP32-WROOM-32U
├── ST7789 LCD (SPI)
│   ├── CS   → GPIO5
│   ├── DC   → GPIO17
│   ├── RST  → GPIO16
│   ├── MOSI → GPIO23
│   ├── SCLK → GPIO18
│   └── BL   → GPIO15 (PWM)
├── PT2314E (I2C)
│   ├── SDA  → GPIO21
│   └── SCL  → GPIO22
├── IR Receiver
│   └── OUT  → GPIO19
└── SSR Relays
    ├── IN1  → GPIO32
    ├── IN2  → GPIO33
    ├── IN3  → GPIO25
    └── IN4  → GPIO26
```

---

## 🏗️ Software Architecture

### System Overview

```
┌─────────────────────────────────────────────────┐
│                   ESP32 Core                    │
├─────────────────────────────────────────────────┤
│  ┌──────────┐  ┌──────────┐  ┌──────────┐     │
│  │ PT2314E  │  │ ST7789   │  │ IR Recv  │     │
│  │  Audio   │  │  Display │  │  Remote  │     │
│  └──────────┘  └──────────┘  └──────────┘     │
├─────────────────────────────────────────────────┤
│               Control Layer                     │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐     │
│  │ WebUI    │  │ MQTT     │  │ IR Code  │     │
│  │ WebSocket│  │ Pub/Sub  │  │ Learning │     │
│  └──────────┘  └──────────┘  └──────────┘     │
├─────────────────────────────────────────────────┤
│             Storage & Network                   │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐     │
│  │   NVS    │  │  WiFi    │  │   OTA    │     │
│  │ Settings │  │ AP + STA │  │ Updates  │     │
│  └──────────┘  └──────────┘  └──────────┘     │
└─────────────────────────────────────────────────┘
```

### Key Features

- **Event-driven architecture** with WebSocket for real-time updates
- **State persistence** via ESP32 NVS (Non-Volatile Storage)
- **Modular design** - easy to extend with new features
- **Thread-safe** MQTT and WebSocket handlers
- **Debounced inputs** for IR remote reliability

---

## 📦 Installation

### Prerequisites

- **PlatformIO** (recommended) or Arduino IDE
- **ESP32 board support** installed
- **USB driver** for ESP32 (CP210x or CH340)

### Quick Start

1. **Clone the repository**
```bash
git clone https://github.com/yourusername/audio-switcher.git
cd audio-switcher
```

2. **Install dependencies** (PlatformIO auto-installs)
```bash
pio lib install
```

3. **Build and upload**
```bash
pio run --target upload
```

4. **Monitor serial output**
```bash
pio device monitor -b 115200
```

### Dependencies

All dependencies are auto-installed from `platformio.ini`:
- Adafruit GFX Library
- Adafruit ST7735 and ST7789 Library
- ESPAsyncWebServer
- AsyncTCP
- AsyncMqttClient
- IRremoteESP8266

---

## 🌐 Web Interface

### Access

- **AP Mode:** http://192.168.4.1 (SSID: AudioSwitcher, password: audio1234)
- **STA Mode:** http://[device-ip] (shown in serial console)

### Features

#### 🎵 Audio Control
- **4 input tiles** - click to switch between sources
- **Volume slider** (0-100%) with live gradient colors
- **Bass slider** (-14 to +14 dB)
- **Treble slider** (-14 to +14 dB)
- **Real-time sync** with LCD and IR remote

#### 🔌 Relay Control
- **4 toggle switches** - one per input
- **Auto-apply** when switching inputs
- **Visual feedback** - green when enabled

#### 📺 Display Settings
- **Brightness control** (PWM 10-255)
- **Color inversion** toggle
- **Rotation** (0° or 180°)

#### 📡 Remote Control
- **IR code learning** with 10-second countdown
- **Clear individual codes** with confirmation
- **Save all codes** in one action
- Supported keys: Source, Input1-4, Vol+/-, ↑/↓

#### 📶 Network Settings
- **WiFi SSID/Password** configuration
- **Test connection** before committing
- **Save & Restart** to apply changes

#### 🔗 MQTT Configuration
- **Enable/Disable** toggle with instant feedback
- **Broker settings** (host, port)
- **Authentication** (username, password)
- **Topic prefix** customization
- **Connection status** (Disabled/Connecting/Connected/Disconnected)

#### 🔄 Firmware Update
- **Drag & drop** or click to browse
- **Progress bar** with percentage
- **Auto-restart** on success

---

## 📡 MQTT API

### Connection Settings

Default configuration:
```yaml
mqtt:
  base_topic: audio_switcher
  port: 1883
  qos: 0
```

### Subscribe Topics (Commands)

#### Switch Input
```
Topic: audio_switcher/set
Payload: 1-4 (input number)
Example: mosquitto_pub -t audio_switcher/set -m "3"
```

#### Set Volume
```
Topic: audio_switcher/volume/set
Payload: 0-100 (percent)
Example: mosquitto_pub -t audio_switcher/volume/set -m "75"
```

#### Set Bass
```
Topic: audio_switcher/bass/set
Payload: -14 to +14 (dB)
Example: mosquitto_pub -t audio_switcher/bass/set -m "5"
```

#### Set Treble
```
Topic: audio_switcher/treble/set
Payload: -14 to +14 (dB)
Example: mosquitto_pub -t audio_switcher/treble/set -m "-3"
```

### Publish Topics (Status)

#### Device Status (Last Will Testament)
```
Topic: audio_switcher/status
Payload: "online" | "offline"
Retained: true
```

#### Current Input
```
Topic: audio_switcher/state
Payload: 1-4
Retained: true
```

#### Audio State
```
Topic: audio_switcher/audio
Payload: JSON
Retained: false

Example:
{
  "volume": 75,        // percent 0-100
  "volume_raw": 15,    // PT2314E raw (0=max, 63=min)
  "bass": 5,           // -14 to +14 dB
  "treble": -2         // -14 to +14 dB
}
```

**Published automatically when:**
- Device connects to MQTT
- Input is changed
- Audio parameter is adjusted (from any source: IR, WebUI, MQTT)

### Home Assistant Integration

```yaml
# configuration.yaml
mqtt:
  switch:
    - name: "Audio Input TV"
      command_topic: "audio_switcher/set"
      payload_on: "1"
      state_topic: "audio_switcher/state"
      state_on: "1"
      
  number:
    - name: "Audio Volume"
      command_topic: "audio_switcher/volume/set"
      state_topic: "audio_switcher/audio"
      value_template: "{{ value_json.volume }}"
      min: 0
      max: 100
      unit_of_measurement: "%"
      
    - name: "Audio Bass"
      command_topic: "audio_switcher/bass/set"
      state_topic: "audio_switcher/audio"
      value_template: "{{ value_json.bass }}"
      min: -14
      max: 14
      unit_of_measurement: "dB"
      
    - name: "Audio Treble"
      command_topic: "audio_switcher/treble/set"
      state_topic: "audio_switcher/audio"
      value_template: "{{ value_json.treble }}"
      min: -14
      max: 14
      unit_of_measurement: "dB"
```

### Command Line Examples

```bash
# Subscribe to all topics
mosquitto_sub -h 10.4.0.7 -u vds -P password -t "audio_switcher/#" -v

# Switch to input 2
mosquitto_pub -h 10.4.0.7 -u vds -P password -t audio_switcher/set -m "2"

# Set volume to 80%
mosquitto_pub -h 10.4.0.7 -u vds -P password -t audio_switcher/volume/set -m "80"

# Boost bass +6dB
mosquitto_pub -h 10.4.0.7 -u vds -P password -t audio_switcher/bass/set -m "6"

# Cut treble -4dB
mosquitto_pub -h 10.4.0.7 -u vds -P password -t audio_switcher/treble/set -m "-4"
```

---

## 🎮 IR Remote Control

### Learning Mode

1. Click **Learn** button in WebUI next to desired function
2. LCD displays "IR: Press button for [function]..."
3. Point your remote at IR receiver
4. Press the button you want to assign
5. LCD shows the learned code
6. Repeat for all functions

### Supported Functions

| Function | Description |
|----------|-------------|
| **Source** | Cycle through inputs (1→2→3→4→1) |
| **Input 1-4** | Direct selection of input |
| **Vol+** | Increase active parameter |
| **Vol-** | Decrease active parameter |
| **↑** | Change focus (Volume→Bass→Treble) |
| **↓** | Change focus (Treble→Bass→Volume) |

### Audio Overlay Navigation

Press **Vol+**, **Vol-**, **↑**, or **↓** to show the audio overlay:

```
> VOLUME   [████████████████        ] 75%
  BASS     [      ████              ] +5
  TREBLE   [    ██                  ] -2
```

**Features:**
- **Focus indicator:** `>` shows active parameter
- **Color coding:**
  - Volume: Green (safe) → Yellow (normal) → Red (loud)
  - Bass/Treble: Cyan (cut) → Green (neutral) → Orange (boost)
- **Navigation:** ↑/↓ changes focus, Vol+/- adjusts value
- **Auto-hide:** Returns to input tiles after 2 seconds

### Clearing Codes

1. Click **Clear** button in WebUI
2. Confirm the action
3. Code is removed and shows "not set"

---

## ⚙️ Configuration

### First-Time Setup

1. **Power on** - device creates WiFi AP "AudioSwitcher"
2. **Connect** to AP (password: `audio1234`)
3. **Navigate** to http://192.168.4.1
4. **Configure WiFi** in Network Settings
5. **Test connection** before saving
6. **Save & Restart** - device connects to your network

### WiFi Configuration

#### Via WebUI
1. Go to **WiFi Settings**
2. Enter SSID and password
3. Click **Test** to verify
4. Click **Save & Restart**

#### Via Code (before first upload)
```cpp
// In main.cpp
#define DEFAULT_SSID "YourNetwork"
#define DEFAULT_PASS "YourPassword"
```

### MQTT Setup

1. Enable MQTT toggle
2. Enter broker IP/hostname
3. Enter port (default: 1883)
4. Add credentials if required
5. Set topic prefix (default: audio_switcher)
6. Click **Save**
7. Status shows **Connecting...** then **Connected**

### Relay Assignment

1. For each input, toggle desired relays
2. Changes save automatically
3. When you switch to that input, assigned relays activate
4. Example: Input 1 → Relay 1 ON (powers amplifier)

---

## 🐛 Troubleshooting

### Display Issues

**Symptom:** Black screen or garbage
- Check SPI wiring (CS, DC, RST, MOSI, SCLK)
- Verify backlight is connected to GPIO15
- Test backlight with PWM (should dim/brighten)

**Symptom:** Display inverted or rotated wrong
- Use WebUI Display Settings to adjust
- Or edit `dispRotate` and `dispInvert` in code

### Audio Issues

**Symptom:** No sound from PT2314E
- Check I2C wiring (SDA=21, SCL=22)
- Verify PT2314E has 5V power
- Test I2C scan: Should detect device at 0x44
- Check input switching (relay clicks?)

**Symptom:** Volume/Bass/Treble not working
- Use WebUI to test controls
- Check serial console for PT2314E commands
- Try factory reset and reconfigure

### WiFi Connection

**Symptom:** Can't connect to network
- Verify SSID and password in WebUI
- Check 2.4GHz WiFi (ESP32 doesn't support 5GHz)
- Try AP mode fallback: "AudioSwitcher" network

**Symptom:** WiFi drops frequently
- Check signal strength (RSSI in serial)
- Move closer to router
- Reduce WiFi power save in code

### MQTT Issues

**Symptom:** Won't connect to broker
- Verify broker IP and port
- Test broker: `mosquitto_pub -h [IP] -t test -m "hello"`
- Check username/password
- Look for error codes in serial:
  - `TCP_DISCONNECTED`: Network issue
  - `NOT_AUTHORIZED`: Wrong credentials
  - `SERVER_UNAVAILABLE`: Broker offline

**Symptom:** Connects then disconnects
- Check Last Will Testament conflicts
- Verify QoS settings
- Increase `MQTT_RECONNECT_DELAY` if broker is slow

### IR Remote

**Symptom:** Remote not learning codes
- Check IR receiver wiring (OUT → GPIO19)
- Verify 5V power to receiver
- Test with known working remote
- Some protocols not supported by library

**Symptom:** Learned codes don't work
- Re-learn the code (10-second window)
- Check if remote uses rolling codes (not supported)
- Try different remote or buttons

### OTA Updates

**Symptom:** Upload fails
- Check free heap (needs ~150KB)
- Verify .bin file is valid firmware
- Try smaller firmware (disable features)
- Upload via USB if OTA keeps failing

### Serial Console

Expected boot output:
```
╔═══════════════════════════════════════╗
║      AUDIO SWITCHER v4.2              ║
╚═══════════════════════════════════════╝

NVS: Prefs loaded
WiFi: Connected! IP: 10.4.8.93
MQTT: Connected
✓ System Ready!
  AP:  http://192.168.4.1
  STA: http://10.4.8.93
  MQTT: 10.4.0.7:1883
```

---

## 📝 Technical Specifications

### Audio Performance
- **Frequency Response:** 20Hz - 20kHz
- **THD+N:** <0.1% @ 1kHz
- **SNR:** >90dB
- **Volume Range:** 0 to -79dB (0.25dB steps)
- **Bass/Treble Range:** ±14dB (2dB steps)

### System Performance
- **Boot Time:** ~3 seconds
- **Input Switch:** <50ms
- **WebSocket Latency:** <100ms
- **MQTT Latency:** <200ms
- **Display Refresh:** 60fps (overlay)

### Power Consumption
- **Idle:** ~200mA @ 5V (1W)
- **Active:** ~300mA @ 5V (1.5W)
- **Peak:** ~500mA @ 5V (2.5W)

---

## 📄 License

MIT License - see [LICENSE](LICENSE) file for details.

---

## 🙏 Acknowledgments

- **Adafruit** - ST7789 display library
- **me-no-dev** - ESPAsyncWebServer & AsyncTCP
- **marvinroger** - AsyncMqttClient
- **crankyoldgit** - IRremoteESP8266
- **Espressif** - ESP32 framework

---

**Made with ❤️ using ESP32**

⭐ **Star this repo if you find it useful!**

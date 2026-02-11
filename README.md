# 🎵 Multi Input Audio Switcher

> ESP32-based 4-input audio switcher with TFT display, IR remote, web interface, MQTT and OTA updates.

![Platform](https://img.shields.io/badge/platform-ESP32-blue)
![Framework](https://img.shields.io/badge/framework-Arduino-teal)
![Version](https://img.shields.io/badge/version-4.0-green)
![License](https://img.shields.io/badge/license-MIT-green)

---

## Overview

A compact audio source switcher built around the **PT2314E** audio processor chip and **ESP32-WROOM-32** microcontroller. Switch between up to 4 audio inputs using an IR remote, a built-in web interface, or MQTT messages from any home automation system. Designed to work as a transparent preamplifier — volume and tone controls are handled by a downstream amplifier (e.g. ZK-T21 2.1).

Input 2 (Bluetooth) has dedicated power management: a solid-state relay cuts power to the BT module when not in use, eliminating idle power draw and avoiding unwanted Bluetooth connections when the input is not selected.

### Features

- **4 audio inputs** — TV, Bluetooth, Internet Radio, Reserve
- **TFT display** (284×76 ST7789) with Material Design tile UI and boot logo
- **IR remote control** — SOURCE button (cycle) or 4 direct input buttons, fully learnable
- **PWM backlight control** — adjustable brightness via web interface, saved across reboots
- **Display options** — color inversion and 180° rotation, configurable from web interface
- **Bluetooth power relay** — SSR cuts BT module power when input 2 is not selected
- **Web interface** — single-page app with WebSocket, no page reloads; switch inputs, learn IR codes, configure WiFi, MQTT and display settings
- **MQTT integration** — subscribe to switch inputs, publish state changes; compatible with Home Assistant, Node-RED and any MQTT broker
- **OTA firmware update** — drag and drop `.bin` file in browser, progress shown on LCD and in browser
- **Dual WiFi mode** — AP for first-time setup, STA for home network integration; AP always available as fallback
- **Persistent storage** — all settings and IR codes saved in NVS, survive power cycles

---

## Hardware

### Bill of Materials

| Component | Description |
|-----------|-------------|
| ESP32-WROOM-32 | Main microcontroller |
| PT2314E | 4-input stereo audio processor, I²C |
| ST7789 TFT | 284×76 px SPI display |
| IR receiver | 38kHz (e.g. TSOP1738, VS1838B) |
| 2N2222 NPN transistor | Backlight PWM driver |
| SSR module | High-level trigger, for BT power control |
| 2.2kΩ resistor | 2N2222 base resistor |
| 220Ω resistors ×2 | I²C pull-ups for SDA/SCL |
| 22µF capacitor | PT2314E REF pin (pin 28) to GND — **critical** |
| Coupling capacitors | Per PT2314E datasheet (input/output) |
| 5V power supply | External, for PT2314E VDD |

### PT2314E Notes

- **VDD range:** 4–10V (5V from external supply recommended)
- **I²C address:** 0x44
- **Critical:** Pin 28 (REF) requires a 22µF capacitor to AGND. Without it the chip pulls SDA LOW and blocks the entire I²C bus
- **Pull-ups:** Use 220Ω resistors on SDA/SCL (PT2314E modules often have 1kΩ series resistors which form a voltage divider with standard 4.7kΩ pull-ups)
- All GND pins (AGND pin 2, DGND pin 25) must share common ground with ESP32

### Pin Connections

#### Display ST7789 (SPI)

| ESP32 GPIO | ST7789 |
|------------|--------|
| GPIO 18 | SCLK |
| GPIO 23 | MOSI |
| GPIO 5 | CS |
| GPIO 17 | DC |
| GPIO 16 | RST |
| GPIO 15 | BL (via 2N2222) |

#### Backlight Circuit (2N2222)

The display backlight draws 80mA at 2.8–3.2V, exceeding the ESP32 GPIO limit. A 2N2222 NPN transistor is used as a PWM-controlled switch:

```
ESP32 GPIO15 ──┤2.2kΩ├──► Base (2N2222)
                            Emitter ──► GND
                            Collector ──► BL pin (display)
```

Pinout of 2N2222 in TO-92 package (flat side facing you): Emitter — Base — Collector.

#### PT2314E (I²C)

| ESP32 | PT2314E |
|-------|---------|
| GPIO 21 (SDA) | Pin 26 (SDA) |
| GPIO 22 (SCL) | Pin 27 (SCL) |
| GND | Pin 2 (AGND) |
| GND | Pin 25 (DGND) |
| — | Pin 1 (VDD) → External 5V |
| — | Pin 28 (REF) → 22µF → GND |

#### IR Receiver

| ESP32 GPIO | IR Receiver |
|------------|-------------|
| GPIO 19 | OUT |
| 3.3V | VCC |
| GND | GND |

#### Bluetooth Power Relay (SSR)

| ESP32 GPIO | SSR Module |
|------------|------------|
| GPIO 4 | IN (high-level trigger) |
| 3.3V or 5V | VCC (per module spec) |
| GND | GND |

SSR output connects between the BT module power supply and BT module VCC. The relay is normally open — the BT module only receives power when Input 2 is selected.

---

## Software

### Project Structure

```
audio_switcher/
├── platformio.ini
└── src/
    ├── main.cpp        # Main application (v4.0)
    ├── PT2314.h        # PT2314E driver header
    ├── PT2314.cpp      # PT2314E driver implementation
    ├── icons.h         # Icon bitmaps in RGB565 PROGMEM (auto-generated)
    ├── logo.h          # Boot logo in RGB565 PROGMEM (auto-generated)
    └── web.h           # Web interface HTML in PROGMEM (auto-generated)

convert_icons.py        # Asset pipeline — run after changing any PNG
index.html              # Web interface template (source for web.h)
tv.png                  # Icon sources (any size, auto-scaled to 40×40)
bt.png
radio.png
other.png
logo.png                # Boot logo source (must be 284×76 px)
```

### Dependencies

| Library | Version | Purpose |
|---------|---------|---------|
| Adafruit ST7735 and ST7789 | ^1.10.3 | TFT display driver |
| Adafruit GFX | ^1.11.9 | Graphics primitives |
| IRremoteESP8266 | ^2.8.6 | IR receive and decode |
| ESPAsyncWebServer-esphome | ^3.1.0 | Async HTTP + WebSocket server |
| async-mqtt-client (OttoWinter fork) | latest | Async MQTT client |

> **Important:** Use the OttoWinter fork of async-mqtt-client (`https://github.com/OttoWinter/async-mqtt-client.git`). It shares the same AsyncTCP layer as ESPAsyncWebServer-esphome, avoiding linker conflicts that occur with the original marvinroger version.

Python dependency for the asset pipeline:
```bash
pip install Pillow
```

---

## Getting Started

### 1. Clone and open in PlatformIO

```bash
git clone https://github.com/youruser/audio-switcher.git
cd audio-switcher
```

### 2. Prepare your assets

Place PNG files in the project root:

| File | Size | Description |
|------|------|-------------|
| `logo.png` | 284×76 px | Boot logo (full screen) |
| `tv.png` | any | Input 1 icon |
| `bt.png` | any | Input 2 icon |
| `radio.png` | any | Input 3 icon |
| `other.png` | any | Input 4 icon |

Icons are automatically scaled to 40×40 px. RGBA PNG with transparent background recommended.

### 3. Run the asset pipeline

```bash
python3 convert_icons.py
cp icons.h logo.h web.h src/
```

The script generates three files: `icons.h` and `logo.h` with RGB565 PROGMEM bitmaps for the TFT, and `web.h` with the complete HTML web interface (base64-embedded images included). Run it again after changing any PNG file.

### 4. Build and flash

```bash
pio run -t upload
pio device monitor
```

---

## First Boot & WiFi Setup

On first boot (no WiFi credentials stored) the device starts in AP-only mode:

```
SSID:     AudioSwitcher
Password: audio1234
URL:      http://192.168.4.1
```

1. Connect to the `AudioSwitcher` WiFi network
2. Open `http://192.168.4.1`
3. Go to **WiFi Settings**, enter your home network SSID and password
4. Click **Save & Restart**

After restart the device connects to your home network while keeping the AP active as a fallback. The assigned IP is shown on the TFT display for 3 seconds.

---

## IR Remote Setup

IR codes are not pre-programmed — you teach them from the web interface:

1. Open the web interface
2. Go to **Remote Control**
3. Click **Learn** next to the function you want to assign
4. Press the desired button on your remote within 10 seconds
5. The code appears in the field automatically
6. Repeat for other buttons, then click **Save IR Codes**

### Supported Functions

| Function | Description |
|----------|-------------|
| Source | Cycle: 1 → 2 → 3 → 4 → 1 |
| Input 1 | Direct select — TV |
| Input 2 | Direct select — BT |
| Input 3 | Direct select — NET |
| Input 4 | Direct select — RES |

---

## MQTT Integration

Configure from the web interface under **MQTT Settings**.

### Topics

| Topic | Direction | Payload | Description |
|-------|-----------|---------|-------------|
| `{topic}/set` | Subscribe | `1`–`4` | Switch to input number |
| `{topic}/state` | Publish | `1`–`4` | Current active input (retained) |
| `{topic}/status` | Publish | `online` / `offline` | Availability with LWT |

Default base topic: `audio_switcher` — configurable in web interface.

### Home Assistant Example

```yaml
mqtt:
  select:
    - name: "Audio Input"
      state_topic: "audio_switcher/state"
      command_topic: "audio_switcher/set"
      options: ["1", "2", "3", "4"]
      availability_topic: "audio_switcher/status"
      payload_available: "online"
      payload_not_available: "offline"
```

### Node-RED

Use an MQTT out node with topic `audio_switcher/set` and payload `2` to switch to Bluetooth input.

---

## OTA Firmware Update

1. Build new firmware: `pio run`
2. Open web interface → **Firmware Update**
3. Drag and drop `.pio/build/esp32-wroom-32/firmware.bin` onto the drop zone, or click to browse
4. Progress is shown in the browser and on the TFT display
5. Device restarts automatically after a successful update

IR reception is disabled during OTA to avoid interference.

---

## Web Interface

Single-page application served from ESP32 flash (PROGMEM). Uses a persistent WebSocket connection — all controls update in real time across all connected clients.

### Sections

| Section | Description |
|---------|-------------|
| **Inputs** | Four icon tiles — click to switch active input |
| **Remote Control** | IR code learning and management |
| **Display** | Brightness slider, color inversion toggle, 180° rotation toggle |
| **MQTT** | Broker credentials, topic, enable toggle, live connection status |
| **WiFi Settings** | Network credentials with connection test |
| **Firmware Update** | OTA via drag & drop with progress bar |

### WebSocket Protocol

```json
// Client → ESP32
{ "type": "set_input",      "input": 1 }
{ "type": "learn_ir",       "key": "input2" }
{ "type": "save_ir",        "codes": { "source": 16753445, "input1": 16724045 } }
{ "type": "set_brightness", "value": 200 }
{ "type": "set_invert",     "value": true }
{ "type": "set_rotate",     "value": false }
{ "type": "save_mqtt",      "host": "192.168.1.10", "port": 1883, "user": "", "pass": "", "topic": "audio_switcher", "enabled": true }
{ "type": "save_wifi",      "ssid": "MyNetwork", "pass": "mypassword" }
{ "type": "test_wifi",      "ssid": "MyNetwork", "pass": "mypassword" }
{ "type": "get_status" }

// ESP32 → Client
{ "type": "status", "input": 0, "ip": "192.168.1.42", "brightness": 200,
  "disp_invert": false, "disp_rotate": false,
  "mqtt_host": "192.168.1.10", "mqtt_port": 1883, "mqtt_topic": "audio_switcher",
  "mqtt_enabled": true, "mqtt_connected": true, "ir": { ... } }
{ "type": "input_changed",    "input": 1 }
{ "type": "ir_received",      "key": "input1", "code": 16724045 }
{ "type": "ir_timeout",       "key": "input1" }
{ "type": "ir_saved" }
{ "type": "mqtt_connected" }
{ "type": "mqtt_disconnected" }
{ "type": "mqtt_saved" }
{ "type": "wifi_saved" }
{ "type": "wifi_test_ok",     "ip": "192.168.1.42" }
{ "type": "wifi_test_fail" }
{ "type": "ota_progress",     "percent": 45 }
{ "type": "ota_ok" }
{ "type": "ota_fail" }
```

---

## Persistent Storage (NVS)

All settings stored in ESP32 NVS under the `switcher` namespace:

| Key | Type | Description |
|-----|------|-------------|
| `wifi_ssid` | String | Home network SSID |
| `wifi_pass` | String | Home network password |
| `last_input` | UChar | Last active input (0–3), restored on boot |
| `brightness` | UChar | Backlight brightness (10–255) |
| `disp_invert` | Bool | Color inversion |
| `disp_rotate` | Bool | 180° rotation |
| `ir_source` | UInt | IR code — SOURCE (cycle) |
| `ir_input1` | UInt | IR code — Input 1 direct |
| `ir_input2` | UInt | IR code — Input 2 direct |
| `ir_input3` | UInt | IR code — Input 3 direct |
| `ir_input4` | UInt | IR code — Input 4 direct |
| `mqtt_host` | String | MQTT broker hostname or IP |
| `mqtt_port` | UShort | MQTT broker port (default 1883) |
| `mqtt_user` | String | MQTT username (optional) |
| `mqtt_pass` | String | MQTT password (optional) |
| `mqtt_topic` | String | MQTT base topic |
| `mqtt_en` | Bool | MQTT enable flag |

---

## Bluetooth Power Management

Input 2 (BT) is connected to an SSR (Solid State Relay, high-level trigger) on GPIO 4. The relay is normally open — the BT module only receives power when Input 2 is active.

- Switching **to** Input 2 → GPIO 4 goes HIGH → SSR closes → BT module powers on
- Switching **away** from Input 2 → GPIO 4 goes LOW → SSR opens → BT module powers off
- On boot, relay state is restored to match the last saved input from NVS

---

## Troubleshooting

**PT2314E not detected / SDA stuck LOW**
Check that Pin 28 (REF) has a 22µF capacitor to AGND. This is the most common cause of I²C bus lockup with this chip.

**No audio output**
Confirm PT2314E has 5V on VDD. Check I²C wiring (SDA→pin 26, SCL→pin 27) and that input/output coupling capacitors are placed per datasheet.

**Backlight not working or wrong brightness**
Check 2N2222 transistor pinout in TO-92 (flat side: E–B–C). Verify 2.2kΩ base resistor is present.

**BT module not turning on when Input 2 selected**
Verify SSR wiring: GPIO 4 → IN. Confirm the module is high-level trigger type. Check common ground between ESP32 and SSR module.

**IR remote not working after learning**
Click **Save IR Codes** after learning all buttons. IR is disabled during OTA — restart if needed.

**MQTT not connecting**
Verify broker IP and port in web interface. Make sure the OttoWinter fork is used (see Dependencies).

**Linker error: multiple definition of AsyncClient**
Two conflicting AsyncTCP libraries are present. Use the OttoWinter fork in `platformio.ini` and clean the build:
```bash
pio pkg uninstall --library "AsyncMqttClient"
pio run --target clean
pio run
```

**Web interface not loading**
AP fallback is always available at `http://192.168.4.1`. Check Serial Monitor output for the STA IP address.

---

## License

MIT License — see [LICENSE](LICENSE) for details.

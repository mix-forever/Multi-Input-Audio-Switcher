# 🎵 Multi Input Audio Switcher

> ESP32-based 4-input audio switcher with TFT display, IR remote control and web interface.

![Platform](https://img.shields.io/badge/platform-ESP32-blue)
![Framework](https://img.shields.io/badge/framework-Arduino-teal)
![License](https://img.shields.io/badge/license-MIT-green)

---

## Overview

A compact audio source switcher built around the **PT2314E** audio processor chip and **ESP32-WROOM-32** microcontroller. Switch between up to 4 audio inputs using an IR remote or a built-in web interface accessible from any device on your network. Designed to work as a transparent preamplifier — volume and tone controls are handled by a downstream amplifier (e.g. ZK-T21 2.1).

### Features

- **4 audio inputs** — TV, Bluetooth, Internet Radio, Reserve
- **TFT display** (284×76 ST7789) with Material Design tile UI
- **IR remote control** — single SOURCE button (cycle) or 4 direct input buttons
- **IR learning** — teach any remote directly from the web interface
- **Web interface** — switch inputs, learn IR codes, configure WiFi (WebSocket-based, no page reloads)
- **Dual WiFi mode** — AP for first-time setup, STA for home network integration
- **Persistent storage** — IR codes, WiFi credentials and last active input saved in NVS (survives power cycles)
- **Boot logo** — full-screen 284×76 splash screen on startup
- **PNG asset pipeline** — single Python script converts PNG icons and logo to C++ PROGMEM headers

---

## Hardware

### Bill of Materials

| Component | Description |
|-----------|-------------|
| ESP32-WROOM-32 | Main microcontroller (ESP32dev board) |
| PT2314E | 4-input stereo audio processor, I²C control |
| ST7789 TFT | 284×76 px SPI display |
| IR receiver | 38kHz (e.g. TSOP1738, VS1838B) |
| 220Ω resistors ×2 | I²C pull-ups for SDA/SCL |
| 22µF capacitor | PT2314E REF pin (pin 28) to GND |
| Coupling capacitors | Per PT2314E datasheet (input/output) |
| 5V power supply | External, for PT2314E VDD |

### PT2314E Notes

- **VDD range:** 4–10V (5V from external supply recommended)
- **I²C address:** 0x44
- **Important:** Pin 28 (REF) requires a 22µF capacitor to AGND — without it the chip pulls SDA LOW and blocks the I²C bus
- **Pull-ups:** Use 220Ω resistors on SDA/SCL (not 4.7kΩ — PT2314E modules often have 1kΩ series resistors which create a voltage divider)
- All GND pins (AGND pin 2, DGND pin 25) must be connected to a common ground shared with ESP32

### Pin Connections

#### Display ST7789 (SPI)

| ESP32 GPIO | ST7789 Pin |
|------------|------------|
| GPIO 18 | SCLK |
| GPIO 23 | MOSI |
| GPIO 5 | CS |
| GPIO 17 | DC |
| GPIO 16 | RST |
| GND | BL |
| 3.3V | VCC |

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

---

## Software

### Project Structure

```
audio_switcher/
├── platformio.ini
└── src/
    ├── main.cpp        # Main application
    ├── PT2314.h        # PT2314E driver header
    ├── PT2314.cpp      # PT2314E driver implementation
    ├── icons.h         # Icon bitmaps in RGB565 PROGMEM (auto-generated)
    ├── logo.h          # Boot logo in RGB565 PROGMEM (auto-generated)
    └── web.h           # Web interface HTML in PROGMEM (auto-generated)

convert_icons.py        # Asset pipeline script
index.html              # Web interface template (source)
tv.png                  # Icon source files
bt.png
radio.png
other.png
logo.png                # Boot logo source (284×76 px)
```

### Dependencies

Managed via PlatformIO (`platformio.ini`):

| Library | Version | Purpose |
|---------|---------|---------|
| Adafruit ST7735 and ST7789 | ^1.10.3 | TFT display driver |
| Adafruit GFX | ^1.11.9 | Graphics primitives |
| IRremoteESP8266 | ^2.8.6 | IR receive and decode |
| ESPAsyncWebServer-esphome | ^3.1.0 | Async HTTP + WebSocket server |

Python dependency for asset pipeline:
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

Place your PNG files in the project root:

| File | Size | Description |
|------|------|-------------|
| `logo.png` | 284×76 px | Boot logo (full screen) |
| `tv.png` | any | Input 1 icon |
| `bt.png` | any | Input 2 icon |
| `radio.png` | any | Input 3 icon |
| `other.png` | any | Input 4 icon |

Icons are automatically scaled to 40×40 px. Transparent backgrounds (RGBA PNG) work best.

### 3. Run the asset pipeline

```bash
python3 convert_icons.py
```

This generates three files in the current directory:

- `icons.h` — RGB565 PROGMEM bitmaps for the TFT display
- `logo.h` — RGB565 PROGMEM boot logo for the TFT display
- `web.h` — HTML web interface with base64-embedded images for PROGMEM

Copy all three to `src/`:

```bash
cp icons.h logo.h web.h src/
```

### 4. Build and flash

```bash
pio run -t upload
pio device monitor
```

---

## First Boot & WiFi Setup

On first boot (no WiFi credentials stored), the device starts in **AP-only mode**:

```
SSID:     AudioSwitcher
Password: audio1234
URL:      http://192.168.4.1
```

1. Connect your phone or laptop to the `AudioSwitcher` WiFi network
2. Open `http://192.168.4.1` in your browser
3. Scroll to **WiFi Settings**
4. Enter your home network SSID and password
5. Click **Save & Restart**

After restart the device connects to your home network (**STA mode**) while keeping the AP active as a fallback. The assigned IP address is shown on the TFT display for 3 seconds.

On subsequent boots the device connects to your home network automatically.

---

## IR Remote Setup

IR codes are not pre-programmed — you teach them from the web interface:

1. Open the web interface (`http://<device-ip>`)
2. Go to the **Remote Control** section
3. Click **Learn** next to the function you want to assign
4. Point your remote at the IR receiver and press the desired button within 10 seconds
5. The code appears automatically in the field
6. Repeat for other buttons
7. Click **Save IR Codes**

### Supported functions

| Function | Description |
|----------|-------------|
| Source | Cycle inputs: 1 → 2 → 3 → 4 → 1 |
| Input 1 | Direct select — TV |
| Input 2 | Direct select — BT |
| Input 3 | Direct select — NET |
| Input 4 | Direct select — RES |

You can assign either a single **Source** button for cycling, or individual buttons for each input, or both simultaneously.

---

## Web Interface

The web interface is a single-page application served directly from ESP32 flash (PROGMEM). It uses a persistent WebSocket connection for real-time updates — no page reloads needed.

### Sections

**Inputs** — four tiles matching the TFT display. Click any tile to switch the active input. The active input is highlighted with a green border and synced in real time across all connected clients.

**Remote Control** — IR code management. Each function shows its current code and a Learn button. The 10-second learning countdown is shown inline.

**WiFi Settings** — change the home network credentials. Use **Test** to verify connectivity before saving. **Save & Restart** writes to NVS and reboots the device.

### WebSocket Protocol

All communication uses JSON messages over `/ws`:

```json
// Client → ESP32
{ "type": "set_input",  "input": 2 }
{ "type": "learn_ir",   "key": "input1" }
{ "type": "save_ir",    "codes": { "source": 16753445, "input1": 16724045 } }
{ "type": "save_wifi",  "ssid": "MyNetwork", "pass": "mypassword" }
{ "type": "test_wifi",  "ssid": "MyNetwork", "pass": "mypassword" }
{ "type": "get_status" }

// ESP32 → Client
{ "type": "status",         "input": 0, "ip": "192.168.1.42", "ir": {...} }
{ "type": "input_changed",  "input": 2 }
{ "type": "ir_received",    "key": "input1", "code": 16724045 }
{ "type": "ir_timeout",     "key": "input1" }
{ "type": "ir_saved" }
{ "type": "wifi_saved" }
{ "type": "wifi_test_ok",   "ip": "192.168.1.42" }
{ "type": "wifi_test_fail" }
```

---

## Asset Pipeline

`convert_icons.py` is a single script that handles all image assets:

```
PNG files  →  convert_icons.py  →  icons.h  (RGB565 for TFT)
                                →  logo.h   (RGB565 for TFT bootlogo)
                                →  web.h    (base64 in HTML for browser)
```

Run it every time you update any PNG file. The script:

- Scales icons to 40×40 px maintaining aspect ratio (centered with padding)
- Handles transparent backgrounds (alpha < 128 → 0x0000 transparent pixel)
- Scales logo to 284×76 px (full screen)
- Replaces `{{LOGO_B64}}`, `{{TV_B64}}`, `{{BT_B64}}`, `{{RADIO_B64}}`, `{{OTHER_B64}}` placeholders in `index.html`
- Reports PROGMEM usage for each output

---

## Persistent Storage (NVS)

All settings are stored in ESP32 NVS (Non-Volatile Storage) under the `switcher` namespace:

| Key | Type | Description |
|-----|------|-------------|
| `wifi_ssid` | String | Home network SSID |
| `wifi_pass` | String | Home network password |
| `last_input` | UChar | Last active input (0–3) |
| `ir_source` | UInt | IR code for SOURCE button |
| `ir_input1` | UInt | IR code for Input 1 |
| `ir_input2` | UInt | IR code for Input 2 |
| `ir_input3` | UInt | IR code for Input 3 |
| `ir_input4` | UInt | IR code for Input 4 |

---

## Troubleshooting

**PT2314E not detected on I²C scan / SDA stuck LOW**
- Check that Pin 28 (REF) has a 22µF capacitor to AGND
- Verify all GND connections share a common ground with ESP32
- Check that VDD (Pin 1) has 5V

**No audio output**
- Confirm PT2314E has 5V on VDD
- Check I²C connections (SDA → Pin 26, SCL → Pin 27)
- Ensure input/output coupling capacitors are correctly placed per datasheet
- Verify the correct input is selected

**I²C hangs / ESP32 freezes during scan**
- SDA or SCL line is stuck LOW — disconnect PT2314E and check for short circuits
- Missing pull-up resistors on SDA/SCL

**IR remote not working after learning**
- Make sure to click **Save IR Codes** after learning
- Check IR receiver wiring (OUT → GPIO 19)
- Try reducing distance to IR receiver during learning

**Web interface not loading**
- Confirm you are connected to the correct network
- Check Serial Monitor for the device IP address
- AP fallback is always available at `http://192.168.4.1`

---

## License

MIT License — see [LICENSE](LICENSE) for details.

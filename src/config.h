// ============================================================
// CONFIG - stałe, piny, kolory, layouty
// ============================================================
#pragma once

// Display pins
#define TFT_CS    5
#define TFT_DC   17
#define TFT_RST  16
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_BL   15

// I2C for PT2314
#define PT_SDA   21
#define PT_SCL   22

// IR receiver pin
#define IR_PIN   19

// Relay pins - SSR high-level trigger
#define RELAY_1  32
#define RELAY_2  33
#define RELAY_3  25
#define RELAY_4  26

// PWM backlight
#define BL_CHANNEL   0
#define BL_FREQ  25000  // 25kHz - poza audio range
#define BL_RES       8
#define BL_DEFAULT 200

// Display
#define SCREEN_W 284
#define SCREEN_H  76

// WiFi AP defaults
#define AP_SSID  "AudioSwitcher"
#define AP_PASS  "audio1234"
#define AP_IP    "192.168.4.1"

// IR
#define IR_DEBOUNCE      300
#define IR_LEARN_TIMEOUT 10000
#define IR_REPEAT_DELAY  600
#define IR_REPEAT_RATE   150

// MQTT defaults
#define MQTT_RECONNECT_DELAY 5000
#define MQTT_TOPIC_DEFAULT   "audio_switcher"

// Kolory
#define COLOR_BG       0x0000
#define COLOR_ACTIVE   0x07E0
#define COLOR_INACTIVE 0x0000
#define COLOR_BORDER   0x4A69
#define COLOR_WHITE    0xFFFF
#define COLOR_GRAY     0x8410
#define COLOR_GREEN    0x07E0
#define COLOR_RED      0xF800
#define COLOR_YELLOW   0xFFE0
#define COLOR_BLUE     0x001F
#define COLOR_CYAN     0x07FF
#define COLOR_ORANGE   0xFD20

// Layout kafelków
#define TILE_MARGIN  5
#define TILE_H      (SCREEN_H - TILE_MARGIN * 2)
#define TILE_W      ((SCREEN_W - TILE_MARGIN * 5) / 4)
#define TILE_RADIUS  6

// Audio overlay timeout
#define AUDIO_OVERLAY_TIMEOUT 5000

// Audio save delay (ochrona flash przy IR auto-repeat)
#define AUDIO_SAVE_DELAY 1500

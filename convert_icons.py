#!/usr/bin/env python3
"""
PNG → RGB565 PROGMEM Converter for ESP32 Audio Switcher
========================================================
Konwertuje ikony i bootlogo PNG do tablic C++ dla Adafruit GFX.

Użycie:
    python3 convert_icons.py

Pliki wejściowe (PNG):
    tv.png, bt.png, radio.png, other.png  → icons.h  (40x40px)
    logo.png                               → logo.h   (284x76px, pełny ekran)

Wymagania:
    pip install Pillow
"""

from PIL import Image
import os
import sys
import base64

# ============================================================
# KONFIGURACJA
# ============================================================

# Rozmiar ikon kafelków
ICON_W = 40
ICON_H = 40

# Rozmiar bootlogo (pełny ekran ST7789)
LOGO_W = 284
LOGO_H = 76

# Ikony kafelków: plik → nazwa zmiennej C++
ICONS = {
    "tv":    "tv",
    "bt":    "bt",
    "radio": "radio",
    "other": "other",
}

# Pliki wyjściowe
ICONS_OUTPUT = "icons.h"
LOGO_OUTPUT  = "logo.h"
WEB_OUTPUT   = "web.h"

# Plik HTML template (musi być w tym samym folderze co skrypt)
HTML_TEMPLATE = "index.html"

# ============================================================
# KONWERSJA RGB888 → RGB565
# ============================================================
def rgb_to_565(r, g, b):
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

# ============================================================
# PRZETWARZANIE OBRAZKA
# ============================================================
def process_image(filepath, target_w, target_h, transparent=True):
    """
    Wczytuje PNG, skaluje do target_w x target_h.
    transparent=True  → piksele alpha<128 stają się 0x0000
    transparent=False → wszystkie piksele konwertowane (logo bez przezroczystości)
    """
    print(f"  Loading:  {filepath}")

    img = Image.open(filepath)
    print(f"  Original: {img.size[0]}x{img.size[1]} px, mode: {img.mode}")

    # Konwertuj do RGBA
    if img.mode != "RGBA":
        img = img.convert("RGBA")

    # Skaluj z zachowaniem proporcji
    img.thumbnail((target_w, target_h), Image.LANCZOS)

    # Padding jeśli proporcje się nie zgadzają
    if img.size != (target_w, target_h):
        padded = Image.new("RGBA", (target_w, target_h), (0, 0, 0, 255))
        offset_x = (target_w - img.size[0]) // 2
        offset_y = (target_h - img.size[1]) // 2
        padded.paste(img, (offset_x, offset_y))
        img = padded
        print(f"  Padded:   {target_w}x{target_h} (centered)")
    else:
        print(f"  Scaled:   {target_w}x{target_h}")

    # Konwertuj do RGB565
    pixels = []
    transparent_count = 0

    for y in range(target_h):
        for x in range(target_w):
            r, g, b, a = img.getpixel((x, y))

            if transparent and a < 128:
                pixels.append(0x0000)  # Transparentny
                transparent_count += 1
            else:
                value = rgb_to_565(r, g, b)
                # Unikaj 0x0000 dla nieprzezroczystych pikseli
                if value == 0x0000 and (not transparent or a >= 128):
                    value = 0x0821
                pixels.append(value)

    if transparent:
        print(f"  Transp:   {transparent_count}/{target_w * target_h} px")

    return pixels

# ============================================================
# GENEROWANIE PROGMEM
# ============================================================
def pixels_to_progmem(name, pixels, w, h, comment=""):
    lines = []
    if comment:
        lines.append(f"// {comment}")
    lines.append(f"const uint16_t PROGMEM {name}[{w * h}] = {{")

    values = [f"0x{p:04X}" for p in pixels]
    for i in range(0, len(values), 8):
        chunk = values[i:i+8]
        comma = "," if i + 8 < len(values) else ""
        lines.append("  " + ", ".join(chunk) + comma)

    lines.append("};")
    return "\n".join(lines)

# ============================================================
# KONWERSJA IKON
# ============================================================
def convert_icons():
    print("=" * 60)
    print("  Converting ICONS")
    print("=" * 60)
    print(f"  Target size: {ICON_W}x{ICON_H} px\n")

    icon_data = {}
    missing = []

    for filename, varname in ICONS.items():
        filepath = None
        for ext in [".png", ".PNG", ".jpg", ".JPG", ".jpeg", ".JPEG"]:
            if os.path.exists(filename + ext):
                filepath = filename + ext
                break

        if filepath is None:
            print(f"  WARNING: {filename}.png not found - skipping!")
            missing.append(filename)
            continue

        print(f"\n[{varname}]")
        try:
            pixels = process_image(filepath, ICON_W, ICON_H, transparent=True)
            icon_data[varname] = pixels
            print(f"  ✓ OK")
        except Exception as e:
            print(f"  ERROR: {e}")
            missing.append(filename)

    if not icon_data:
        print("\nERROR: No icons converted!")
        return False

    # Generuj icons.h
    print(f"\nGenerating {ICONS_OUTPUT}...")

    lines = []
    lines.append("// ============================================================")
    lines.append("// Auto-generated icon data for ESP32 Audio Switcher")
    lines.append(f"// Icon size: {ICON_W}x{ICON_H} px, RGB565, PROGMEM")
    lines.append("// 0x0000 = transparent (replaced with tile background color)")
    lines.append("// ============================================================")
    lines.append("")
    lines.append("#pragma once")
    lines.append("#include <pgmspace.h>")
    lines.append("")
    lines.append(f"#define ICON_W {ICON_W}")
    lines.append(f"#define ICON_H {ICON_H}")
    lines.append("")

    for varname, pixels in icon_data.items():
        lines.append(pixels_to_progmem(
            f"icon_{varname}", pixels, ICON_W, ICON_H,
            comment=f"Icon: {varname} ({ICON_W}x{ICON_H} px)"
        ))
        lines.append("")

    with open(ICONS_OUTPUT, "w") as f:
        f.write("\n".join(lines))

    total_bytes = len(icon_data) * ICON_W * ICON_H * 2
    print(f"\n  Converted: {len(icon_data)}/{len(ICONS)} icons")
    if missing:
        print(f"  Missing:   {', '.join(missing)}")
    print(f"  PROGMEM:   {total_bytes} bytes ({total_bytes/1024:.1f} KB)")
    print(f"  Output:    {ICONS_OUTPUT}")

    return True

# ============================================================
# KONWERSJA LOGO
# ============================================================
def convert_logo():
    print("\n" + "=" * 60)
    print("  Converting BOOTLOGO")
    print("=" * 60)
    print(f"  Target size: {LOGO_W}x{LOGO_H} px (full screen)\n")

    filepath = None
    for ext in [".png", ".PNG", ".jpg", ".JPG", ".jpeg", ".JPEG"]:
        if os.path.exists("logo" + ext):
            filepath = "logo" + ext
            break

    if filepath is None:
        print("  WARNING: logo.png not found - skipping!")
        print(f"  Place logo.png ({LOGO_W}x{LOGO_H} px) in this directory.")
        return False

    print("[bootlogo]")
    try:
        pixels = process_image(filepath, LOGO_W, LOGO_H, transparent=False)
    except Exception as e:
        print(f"  ERROR: {e}")
        return False

    # Generuj logo.h
    print(f"\nGenerating {LOGO_OUTPUT}...")

    lines = []
    lines.append("// ============================================================")
    lines.append("// Auto-generated bootlogo for ESP32 Audio Switcher")
    lines.append(f"// Size: {LOGO_W}x{LOGO_H} px, RGB565, PROGMEM")
    lines.append("// ============================================================")
    lines.append("")
    lines.append("#pragma once")
    lines.append("#include <pgmspace.h>")
    lines.append("")
    lines.append(f"#define LOGO_W {LOGO_W}")
    lines.append(f"#define LOGO_H {LOGO_H}")
    lines.append("")
    lines.append(pixels_to_progmem(
        "bootlogo", pixels, LOGO_W, LOGO_H,
        comment=f"Bootlogo ({LOGO_W}x{LOGO_H} px)"
    ))
    lines.append("")

    with open(LOGO_OUTPUT, "w") as f:
        f.write("\n".join(lines))

    total_bytes = LOGO_W * LOGO_H * 2
    print(f"\n  PROGMEM:   {total_bytes} bytes ({total_bytes/1024:.1f} KB)")
    print(f"  Output:    {LOGO_OUTPUT}")
    print(f"  ✓ OK")

    return True

# ============================================================
# MAIN
# ============================================================
# ============================================================
# GENEROWANIE web.h - HTML + base64 obrazki z PNG
# ============================================================
def generate_web_h():
    print("\n" + "=" * 60)
    print("  Generating WEB.H")
    print("=" * 60 + "\n")

    # Sprawdź czy jest template HTML
    if not os.path.exists(HTML_TEMPLATE):
        print(f"  ERROR: {HTML_TEMPLATE} not found!")
        print(f"  Place {HTML_TEMPLATE} in this directory.")
        return False

    html = open(HTML_TEMPLATE).read()

    # Sprawdź czy HTML ma placeholdery
    placeholders = {
        "{{LOGO_B64}}":  "logo",
        "{{TV_B64}}":    "tv",
        "{{BT_B64}}":    "bt",
        "{{RADIO_B64}}": "radio",
        "{{OTHER_B64}}": "other",
    }

    # Zamień placeholdery na base64
    found_any = False
    for placeholder, name in placeholders.items():
        if placeholder not in html:
            print(f"  SKIP: {placeholder} not in HTML")
            continue

        # Szukaj pliku PNG
        filepath = None
        for ext in [".png", ".PNG", ".jpg", ".JPG"]:
            if os.path.exists(name + ext):
                filepath = name + ext
                break

        if filepath is None:
            print(f"  WARNING: {name}.png not found - placeholder left empty")
            html = html.replace(placeholder, "")
            continue

        with open(filepath, "rb") as f:
            b64 = base64.b64encode(f.read()).decode()

        html = html.replace(placeholder, b64)
        print(f"  {placeholder} → {filepath} ({len(b64)} chars base64)")
        found_any = True

    if not found_any:
        print("\n  WARNING: No placeholders found in HTML!")
        print(f"  Add placeholders like {{{{LOGO_B64}}}} to {HTML_TEMPLATE}")
        print("  Generating web.h anyway (without image substitution)...")

    # Sprawdź czy HTML zawiera )rawliteral (przerwałoby raw string C++)
    if ")rawliteral" in html:
        print("\n  ERROR: HTML contains ')rawliteral' - conflicts with C++ raw string!")
        print("  Remove or rename this string in the HTML file.")
        return False

    # Generuj web.h
    output = "// ============================================================\n"
    output += "// Auto-generated web interface - PROGMEM\n"
    output += f"// Generated from: {HTML_TEMPLATE}\n"
    output += f"// Size: {len(html)} bytes\n"
    output += "// ============================================================\n"
    output += "#pragma once\n"
    output += "#include <pgmspace.h>\n\n"
    output += 'const char INDEX_HTML[] PROGMEM = R"rawliteral(\n'
    output += html
    output += '\n)rawliteral";\n'

    with open(WEB_OUTPUT, "w") as f:
        f.write(output)

    print(f"\n  Size:   {len(html)/1024:.1f} KB")
    print(f"  Output: {WEB_OUTPUT}")
    print(f"  ✓ OK")
    return True


def main():
    print("\n" + "=" * 60)
    print("  PNG → RGB565 PROGMEM Converter")
    print("  ESP32 Audio Switcher")
    print("=" * 60 + "\n")

    try:
        from PIL import Image
    except ImportError:
        print("ERROR: Pillow not installed!")
        print("Run: pip install Pillow")
        sys.exit(1)

    icons_ok = convert_icons()
    logo_ok  = convert_logo()
    web_ok   = generate_web_h()

    print("\n" + "=" * 60)
    print("  DONE")
    print("=" * 60)
    print(f"  icons.h : {'✓ generated' if icons_ok else '✗ failed'}")
    print(f"  logo.h  : {'✓ generated' if logo_ok  else '✗ skipped (no logo.png)'}")
    print(f"  web.h   : {'✓ generated' if web_ok   else '✗ failed'}")
    print()
    print("  Copy icons.h, logo.h and web.h to your PlatformIO src/ directory")
    print("=" * 60 + "\n")

if __name__ == "__main__":
    main()

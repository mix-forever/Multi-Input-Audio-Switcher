// ============================================================
// Auto-generated web interface - PROGMEM
// Generated from: index.html
// Size: 124650 bytes
// ============================================================
#pragma once
#include <pgmspace.h>

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>MultiAMP</title>
<link rel="icon" href="data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 100 100'%3E%3Crect width='100' height='100' rx='20' fill='%232dc55e'/%3E%3Ctext x='50' y='68' font-size='55' text-anchor='middle' fill='%23000' font-family='Arial'%3E%26%23x266B;%3C/text%3E%3C/svg%3E">
<style>
  :root {
    --bg:       #121212;
    --surface:  #1e1e1e;
    --surface2: #2a2a2a;
    --surface3: #333;
    --green:    #2dc55e;
    --red:      #e02828;
    --text:     #e0e0e0;
    --subtext:  #888;
    --border:   #333;
    --radius:   12px;
  }
  * { box-sizing: border-box; margin: 0; padding: 0; }
  body {
    background: var(--bg);
    color: var(--text);
    font-family: 'Segoe UI', system-ui, sans-serif;
    min-height: 100vh;
    padding-bottom: 40px;
  }

  /* HEADER */
  header {
    background: var(--bg);
    border-bottom: 1px solid var(--border);
    padding: 0 16px;
    position: sticky;
    top: 0;
    z-index: 100;
  }
  .header-inner {
    max-width: 600px;
    margin: 0 auto;
    padding: 12px 0;
    display: flex;
    align-items: center;
    justify-content: space-between;
  }
  header img.logo {
    height: 70px;
    image-rendering: pixelated;
  }
  .status-dots {
    display: flex;
    align-items: center;
    gap: 12px;
  }
  .status-dot {
    display: flex;
    align-items: center;
    gap: 6px;
    font-size: 12px;
    color: var(--subtext);
  }
  .status-dot .dot {
    width: 8px; height: 8px;
    border-radius: 50%;
    background: var(--subtext);
    transition: background .3s, box-shadow .3s;
  }
  .status-dot.online .dot {
    background: var(--green);
    box-shadow: 0 0 6px var(--green);
  }
  .status-dot.offline .dot {
    background: var(--red);
    box-shadow: 0 0 6px var(--red);
  }

  /* MAIN */
  main {
    max-width: 600px;
    margin: 0 auto;
    padding: 20px 16px;
    display: flex;
    flex-direction: column;
    gap: 20px;
  }

  /* CARD */
  .card {
    background: var(--surface);
    border: 1px solid var(--border);
    border-radius: var(--radius);
    overflow: hidden;
  }
  .card-header {
    padding: 12px 20px;
    font-size: 11px;
    font-weight: 700;
    letter-spacing: 1.5px;
    text-transform: uppercase;
    color: var(--subtext);
    border-bottom: 1px solid var(--border);
    background: #000;
  }
  .card-body {
    padding: 20px;
  }

  /* DASHBOARD GRID */
  .dash-grid {
    display: grid;
    grid-template-columns: repeat(4, 1fr);
    gap: 12px;
  }
  .dash-tile {
    background: #000;
    border: 2px solid var(--border);
    border-radius: 10px;
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    gap: 6px;
    cursor: pointer;
    transition: border-color .2s, box-shadow .2s, transform .1s;
    user-select: none;
    padding: 12px 4px;
  }
  .dash-tile:hover {
    border-color: #555;
    transform: translateY(-1px);
  }
  .dash-tile:active {
    transform: scale(0.96);
  }
  .dash-tile.active {
    border-color: var(--green);
    box-shadow: 0 0 0 1px var(--green), 0 4px 16px rgba(45,197,94,.2);
  }
  .dash-tile img {
    width: 40px;
    height: 40px;
    image-rendering: pixelated;
    opacity: 0.6;
    transition: opacity .2s;
  }
  .dash-tile.active img {
    opacity: 1;
    filter: drop-shadow(0 0 4px var(--green));
  }
  .dash-tile .tile-label {
    font-size: 11px;
    font-weight: 600;
    letter-spacing: 1px;
    color: var(--subtext);
    text-transform: uppercase;
  }
  .dash-tile.active .tile-label {
    color: var(--green);
  }
  /* RELAYS ROW */
  .relays-row {
    display: grid;
    grid-template-columns: repeat(4, 1fr);
    gap: 16px;
    margin-top: 20px;
    padding-top: 20px;
    border-top: 1px solid var(--border);
  }
  .relay-item {
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 10px;
    padding: 14px 8px;
    background: #000;
    border: 1px solid var(--border);
    border-radius: 10px;
    cursor: pointer;
    transition: border-color .2s, background .2s;
    user-select: none;
  }
  .relay-item:hover {
    border-color: #555;
    background: #0a0a0a;
  }
  .relay-item.active {
    border-color: var(--green);
    background: rgba(45,197,94,0.05);
  }
  .relay-item .relay-label {
    font-size: 12px;
    font-weight: 600;
    letter-spacing: 1px;
    text-transform: uppercase;
    color: var(--subtext);
  }
  .relay-item.active .relay-label {
    color: var(--green);
  }

  /* AUDIO SLIDERS */
  .audio-row {
    margin-bottom: 16px;
  }
  .audio-row:last-child {
    margin-bottom: 0;
  }
  .audio-label {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 8px;
    font-size: 12px;
    font-weight: 600;
    letter-spacing: 1px;
    text-transform: uppercase;
    color: var(--subtext);
  }
  .audio-label .val {
    color: var(--text);
    font-weight: 700;
  }
  .slider-wrap {
    display: flex;
    align-items: center;
    gap: 10px;
  }
  .slider-wrap .icon {
    font-size: 16px;
    flex-shrink: 0;
    color: var(--subtext);
  }
  .slider-wrap input[type="range"] {
    flex: 1;
    height: 6px;
    border-radius: 3px;
    background: var(--surface2);
    outline: none;
    -webkit-appearance: none;
  }
  .slider-wrap input[type="range"]::-webkit-slider-thumb {
    -webkit-appearance: none;
    width: 18px;
    height: 18px;
    border-radius: 50%;
    background: var(--green);
    cursor: pointer;
  }
  .slider-wrap input[type="range"]::-moz-range-thumb {
    width: 18px;
    height: 18px;
    border-radius: 50%;
    background: var(--green);
    border: none;
    cursor: pointer;
  }

  /* BRIGHTNESS ROW (full width like audio) */
  .brightness-row {
    display: flex;
    align-items: center;
    gap: 12px;
  }
  .brightness-icon {
    font-size: 20px;
    flex-shrink: 0;
  }
  .brightness-row input[type="range"] {
    flex: 1;
    height: 6px;
    border-radius: 3px;
    background: var(--surface2);
    outline: none;
    -webkit-appearance: none;
  }
  .brightness-row input[type="range"]::-webkit-slider-thumb {
    -webkit-appearance: none;
    width: 18px;
    height: 18px;
    border-radius: 50%;
    background: var(--green);
    cursor: pointer;
  }
  .brightness-row input[type="range"]::-moz-range-thumb {
    width: 18px;
    height: 18px;
    border-radius: 50%;
    background: var(--green);
    border: none;
    cursor: pointer;
  }
  .brightness-val {
    min-width: 50px;
    text-align: right;
    font-size: 13px;
    color: var(--subtext);
  }

  /* TABS */
  .tabs {
    display: flex;
    gap: 4px;
    padding: 4px;
    background: #000;
    border-bottom: 1px solid var(--border);
  }
  .tab-btn {
    flex: 1;
    padding: 10px 8px;
    background: transparent;
    border: none;
    border-radius: 8px;
    color: var(--subtext);
    font-size: 13px;
    font-weight: 600;
    cursor: pointer;
    transition: background .2s, color .2s;
  }
  .tab-btn:hover {
    background: var(--surface2);
    color: var(--text);
  }
  .tab-btn.active {
    background: var(--surface);
    color: var(--green);
  }
  .tab-panel {
    display: none;
    padding: 20px;
  }
  .tab-panel.active {
    display: block;
  }

  /* TOGGLE */
  .toggle-row {
    display: flex;
    align-items: center;
    justify-content: space-between;
    padding: 10px 0;
    cursor: pointer;
  }
  .toggle-label {
    font-size: 14px;
  }
  .toggle-switch {
    position: relative;
    width: 44px;
    height: 24px;
    flex-shrink: 0;
  }
  .toggle-switch input {
    opacity: 0;
    width: 0;
    height: 0;
    position: absolute;
  }
  .toggle-slider {
    position: absolute;
    inset: 0;
    background: var(--border);
    border-radius: 24px;
    transition: background .2s;
    cursor: pointer;
  }
  .toggle-slider::before {
    content: '';
    position: absolute;
    width: 18px;
    height: 18px;
    left: 3px;
    top: 3px;
    background: #fff;
    border-radius: 50%;
    transition: transform .2s;
  }
  .toggle-switch input:checked + .toggle-slider {
    background: var(--green);
  }
  .toggle-switch input:checked + .toggle-slider::before {
    transform: translateX(20px);
  }

  /* IR REMOTE */
  .ir-rows {
    display: flex;
    flex-direction: column;
    gap: 12px;
  }
  .ir-row {
    display: grid;
    grid-template-columns: 70px 1fr 45px 60px 60px;
    gap: 8px;
    align-items: center;
  }
  .ir-label {
    font-size: 13px;
    color: var(--subtext);
  }
  .ir-code {
    flex: 1;
    background: var(--bg);
    border: 1px solid var(--border);
    border-radius: 8px;
    padding: 9px 14px;
    font-family: 'Courier New', monospace;
    font-size: 13px;
    color: var(--text);
    min-width: 0;
  }
  .countdown {
    font-size: 12px;
    color: var(--green);
    text-align: center;
  }
  .ir-save-row {
    margin-top: 12px;
    display: flex;
    justify-content: flex-end;
  }

  /* BUTTONS */
  .btn {
    padding: 9px 16px;
    border: none;
    border-radius: 8px;
    font-size: 13px;
    font-weight: 600;
    cursor: pointer;
    transition: opacity .2s, transform .1s;
    white-space: nowrap;
  }
  .btn:hover  { opacity: 0.85; }
  .btn:active { transform: scale(0.96); }
  .btn-green  { background: var(--green); color: #000; }
  .btn-red    { background: var(--red);   color: #fff; }
  .btn-gray   { background: #000; color: var(--text); border: 1px solid var(--border); }
  .btn:disabled { opacity: 0.4; cursor: not-allowed; transform: none; }

  /* WIFI */
  .wifi-form {
    display: flex;
    flex-direction: column;
    gap: 10px;
  }
  .wifi-input {
    width: 100%;
    padding: 10px;
    background: var(--surface2);
    border: 1px solid var(--border);
    border-radius: 8px;
    color: var(--text);
    font-size: 14px;
  }
  .wifi-input::placeholder {
    color: var(--subtext);
  }
  .wifi-buttons {
    display: flex;
    gap: 8px;
  }
  .wifi-buttons button {
    flex: 1;
  }
  .wifi-status {
    padding: 8px;
    border-radius: 6px;
    font-size: 13px;
    text-align: center;
  }

  /* OTA */
  .ota-drop-zone {
    border: 2px dashed var(--border);
    border-radius: 12px;
    padding: 40px 20px;
    text-align: center;
    cursor: pointer;
    transition: border-color 0.2s;
  }
  .ota-drop-zone:hover {
    border-color: var(--green);
  }
  .ota-icon {
    font-size: 48px;
    margin-bottom: 12px;
  }
  .ota-text {
    font-size: 14px;
    color: var(--subtext);
  }
  .ota-progress {
    margin-top: 16px;
  }
  .ota-bar {
    height: 8px;
    background: var(--surface2);
    border-radius: 4px;
    overflow: hidden;
  }
  .ota-fill {
    height: 100%;
    background: var(--green);
    width: 0%;
    transition: width 0.3s;
  }
  .ota-percent {
    text-align: center;
    margin-top: 8px;
    font-size: 13px;
    color: var(--subtext);
  }

  /* LOGS - improved styling */
  .logs-area {
    width: 100%;
    height: 320px;
    background: #0d0d0d;
    color: #c8c8c8;
    font-family: 'SF Mono', 'Menlo', 'Monaco', 'Courier New', monospace;
    font-size: 12px;
    line-height: 1.6;
    border: 1px solid var(--border);
    border-radius: 8px;
    padding: 14px;
    resize: vertical;
    white-space: pre-wrap;
    overflow-y: auto;
    tab-size: 4;
  }
  .logs-area .log-time {
    color: #666;
    font-weight: 600;
  }
  .logs-area .log-warn {
    color: #ffaa00;
  }
  .logs-area .log-err {
    color: #ff4444;
  }
  .logs-area .log-info {
    color: #44aaff;
  }
  .logs-area::-webkit-scrollbar {
    width: 8px;
  }
  .logs-area::-webkit-scrollbar-track {
    background: #0a0a0a;
    border-radius: 4px;
  }
  .logs-area::-webkit-scrollbar-thumb {
    background: var(--surface3);
    border-radius: 4px;
  }
  .logs-area::-webkit-scrollbar-thumb:hover {
    background: #555;
  }
  .logs-controls {
    display: flex;
    align-items: center;
    gap: 10px;
    margin-top: 12px;
    flex-wrap: wrap;
  }

  /* TOAST */
  .toast {
    position: fixed;
    bottom: 20px;
    left: 50%;
    transform: translateX(-50%);
    background: var(--surface);
    border: 1px solid var(--border);
    border-radius: 8px;
    padding: 12px 20px;
    font-size: 14px;
    z-index: 1000;
    animation: slideUp 0.3s;
  }
  @keyframes slideUp {
    from { bottom: -50px; opacity: 0; }
    to { bottom: 20px; opacity: 1; }
  }

  /* RESPONSIVE */
  @media (max-width: 480px) {
    .dash-grid {
      grid-template-columns: repeat(2, 1fr);
    }
    .ir-row {
      grid-template-columns: 60px 1fr 40px 50px 50px;
      gap: 4px;
    }
    .ir-code {
      padding: 7px 8px;
      font-size: 11px;
    }
    header img.logo {
      height: 50px;
    }
  }
</style>
</head>
<body>

<header>
  <div class="header-inner">
    <img class="logo" src="data:image/png;base64,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" alt="MultiAMP">
    <div class="status-dots">
      <div class="status-dot" id="status-wifi">
        <div class="dot"></div>
        <span>WiFi</span>
      </div>
      <div class="status-dot" id="status-mqtt">
        <div class="dot"></div>
        <span>MQTT</span>
      </div>
    </div>
  </div>
</header>

<main>

  <!-- DASHBOARD: Inputs + Relays -->
  <div class="card">
    <div class="card-header">Dashboard</div>
    <div class="card-body">
      <div class="dash-grid">
        <div class="dash-tile" id="tile-0" onclick="selectInput(0)">
          <img src="data:image/png;base64,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" alt="Input1">
          <span class="tile-label">Input1</span>
        </div>
        <div class="dash-tile" id="tile-1" onclick="selectInput(1)">
          <img src="data:image/png;base64,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" alt="Input2">
          <span class="tile-label">Input2</span>
        </div>
        <div class="dash-tile" id="tile-2" onclick="selectInput(2)">
          <img src="data:image/png;base64,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" alt="Input3">
          <span class="tile-label">Input3</span>
        </div>
        <div class="dash-tile" id="tile-3" onclick="selectInput(3)">
          <img src="data:image/png;base64,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" alt="Input4">
          <span class="tile-label">Input4</span>
        </div>
      </div>

      <!-- Relays -->
      <div class="relays-row">
        <div class="relay-item" id="relay-item-0" onclick="toggleRelay(0)">
          <div class="toggle-switch" onclick="event.stopPropagation()">
            <input type="checkbox" id="relay-0">
            <span class="toggle-slider"></span>
          </div>
          <span class="relay-label">Relay 1</span>
        </div>
        <div class="relay-item" id="relay-item-1" onclick="toggleRelay(1)">
          <div class="toggle-switch" onclick="event.stopPropagation()">
            <input type="checkbox" id="relay-1">
            <span class="toggle-slider"></span>
          </div>
          <span class="relay-label">Relay 2</span>
        </div>
        <div class="relay-item" id="relay-item-2" onclick="toggleRelay(2)">
          <div class="toggle-switch" onclick="event.stopPropagation()">
            <input type="checkbox" id="relay-2">
            <span class="toggle-slider"></span>
          </div>
          <span class="relay-label">Relay 3</span>
        </div>
        <div class="relay-item" id="relay-item-3" onclick="toggleRelay(3)">
          <div class="toggle-switch" onclick="event.stopPropagation()">
            <input type="checkbox" id="relay-3">
            <span class="toggle-slider"></span>
          </div>
          <span class="relay-label">Relay 4</span>
        </div>
      </div>
    </div>
  </div>

  <!-- AUDIO -->
  <div class="card">
    <div class="card-header">Audio</div>
    <div class="card-body">
      <!-- Volume -->
      <div class="audio-row">
        <div class="audio-label">
          <span>Volume</span>
          <span class="val" id="volume-val">100%</span>
        </div>
        <div class="slider-wrap">
          <span class="icon">🔇</span>
          <input type="range" id="audio-volume" min="0" max="100" value="100"
                 oninput="onVolumeChange(this.value)">
          <span class="icon">🔊</span>
        </div>
      </div>
      <!-- Bass -->
      <div class="audio-row">
        <div class="audio-label">
          <span>Bass</span>
          <span class="val" id="bass-val">0 dB</span>
        </div>
        <div class="slider-wrap">
          <span class="icon">−</span>
          <input type="range" id="audio-bass" min="-14" max="14" value="0"
                 oninput="onBassChange(this.value)">
          <span class="icon">+</span>
        </div>
      </div>
      <!-- Treble -->
      <div class="audio-row">
        <div class="audio-label">
          <span>Treble</span>
          <span class="val" id="treble-val">0 dB</span>
        </div>
        <div class="slider-wrap">
          <span class="icon">−</span>
          <input type="range" id="audio-treble" min="-14" max="14" value="0"
                 oninput="onTrebleChange(this.value)">
          <span class="icon">+</span>
        </div>
      </div>
    </div>
  </div>

  <!-- TABS -->
  <div class="card">
    <div class="tabs">
      <button class="tab-btn active" onclick="switchTab('settings')" id="tab-btn-settings">Settings</button>
      <button class="tab-btn" onclick="switchTab('network')" id="tab-btn-network">Network</button>
      <button class="tab-btn" onclick="switchTab('system')" id="tab-btn-system">System</button>
    </div>

    <!-- SETTINGS TAB -->
    <div class="tab-panel active" id="tab-settings">
      <!-- Display -->
      <div style="margin-bottom:20px;">
        <div class="card-header" style="margin:-20px -20px 16px -20px; border-radius:0;">Display</div>
        <div class="brightness-row" style="margin-bottom:16px;">
          <span class="brightness-icon">🔅</span>
          <input type="range" id="brightness" min="10" max="255" value="200"
                 oninput="onBrightnessChange(this.value)">
          <span class="brightness-icon">🔆</span>
          <span class="brightness-val" id="brightness-val">78%</span>
        </div>
        <div class="toggle-row" onclick="toggleInvert()">
          <span class="toggle-label">Invert colors</span>
          <div class="toggle-switch" onclick="event.stopPropagation(); toggleInvert()">
            <input type="checkbox" id="disp-invert">
            <span class="toggle-slider"></span>
          </div>
        </div>
        <div class="toggle-row" onclick="toggleRotate()">
          <span class="toggle-label">Rotate 180°</span>
          <div class="toggle-switch" onclick="event.stopPropagation(); toggleRotate()">
            <input type="checkbox" id="disp-rotate">
            <span class="toggle-slider"></span>
          </div>
        </div>
      </div>

      <!-- IR Remote -->
      <div>
        <div class="card-header" style="margin:0 -20px 16px -20px; border-radius:0;">Remote Control — IR Codes</div>
        <div class="ir-rows">
          <div class="ir-row">
            <span class="ir-label">Source</span>
            <input class="ir-code" id="ir-source" type="text" readonly placeholder="not set">
            <span class="countdown" id="cd-source"></span>
            <button class="btn btn-gray" onclick="learnIR('source')">Learn</button>
            <button class="btn btn-red" onclick="clearIR('source')">Clear</button>
          </div>
          <div class="ir-row">
            <span class="ir-label">Input 1</span>
            <input class="ir-code" id="ir-input1" type="text" readonly placeholder="not set">
            <span class="countdown" id="cd-input1"></span>
            <button class="btn btn-gray" onclick="learnIR('input1')">Learn</button>
            <button class="btn btn-red" onclick="clearIR('input1')">Clear</button>
          </div>
          <div class="ir-row">
            <span class="ir-label">Input 2</span>
            <input class="ir-code" id="ir-input2" type="text" readonly placeholder="not set">
            <span class="countdown" id="cd-input2"></span>
            <button class="btn btn-gray" onclick="learnIR('input2')">Learn</button>
            <button class="btn btn-red" onclick="clearIR('input2')">Clear</button>
          </div>
          <div class="ir-row">
            <span class="ir-label">Input 3</span>
            <input class="ir-code" id="ir-input3" type="text" readonly placeholder="not set">
            <span class="countdown" id="cd-input3"></span>
            <button class="btn btn-gray" onclick="learnIR('input3')">Learn</button>
            <button class="btn btn-red" onclick="clearIR('input3')">Clear</button>
          </div>
          <div class="ir-row">
            <span class="ir-label">Input 4</span>
            <input class="ir-code" id="ir-input4" type="text" readonly placeholder="not set">
            <span class="countdown" id="cd-input4"></span>
            <button class="btn btn-gray" onclick="learnIR('input4')">Learn</button>
            <button class="btn btn-red" onclick="clearIR('input4')">Clear</button>
          </div>
          <div class="ir-row">
            <span class="ir-label">Vol +</span>
            <input class="ir-code" id="ir-volp" type="text" readonly placeholder="not set">
            <span class="countdown" id="cd-volp"></span>
            <button class="btn btn-gray" onclick="learnIR('volp')">Learn</button>
            <button class="btn btn-red" onclick="clearIR('volp')">Clear</button>
          </div>
          <div class="ir-row">
            <span class="ir-label">Vol −</span>
            <input class="ir-code" id="ir-volm" type="text" readonly placeholder="not set">
            <span class="countdown" id="cd-volm"></span>
            <button class="btn btn-gray" onclick="learnIR('volm')">Learn</button>
            <button class="btn btn-red" onclick="clearIR('volm')">Clear</button>
          </div>
          <div class="ir-row">
            <span class="ir-label">↑ Up</span>
            <input class="ir-code" id="ir-up" type="text" readonly placeholder="not set">
            <span class="countdown" id="cd-up"></span>
            <button class="btn btn-gray" onclick="learnIR('up')">Learn</button>
            <button class="btn btn-red" onclick="clearIR('up')">Clear</button>
          </div>
          <div class="ir-row">
            <span class="ir-label">↓ Down</span>
            <input class="ir-code" id="ir-down" type="text" readonly placeholder="not set">
            <span class="countdown" id="cd-down"></span>
            <button class="btn btn-gray" onclick="learnIR('down')">Learn</button>
            <button class="btn btn-red" onclick="clearIR('down')">Clear</button>
          </div>
        </div>
        <div class="ir-save-row">
          <button class="btn btn-green" onclick="saveIR()">Save IR Codes</button>
        </div>
      </div>
    </div>

    <!-- NETWORK TAB -->
    <div class="tab-panel" id="tab-network">
      <!-- WiFi -->
      <div style="margin-bottom:20px;">
        <div class="card-header" style="margin:-20px -20px 16px -20px; border-radius:0;">WiFi</div>
        <div class="wifi-form">
          <input type="text" id="wifi-ssid" placeholder="SSID" class="wifi-input">
          <input type="password" id="wifi-pass" placeholder="Password" class="wifi-input">
          <div class="wifi-buttons">
            <button class="btn btn-gray" onclick="testWifi()">Test</button>
            <button class="btn btn-green" onclick="saveWifi()">Save & Reboot</button>
          </div>
          <div id="wifi-status" class="wifi-status"></div>
        </div>
      </div>

      <!-- MQTT -->
      <div>
        <div class="card-header" style="margin:0 -20px 16px -20px; border-radius:0;">MQTT</div>
        <label class="toggle-row" onclick="toggleMqttEnable()">
          <span class="toggle-label">Enable MQTT</span>
          <div class="toggle-switch" onclick="event.stopPropagation(); toggleMqttEnable()">
            <input type="checkbox" id="mqtt-enable">
            <span class="toggle-slider"></span>
          </div>
        </label>
        <div id="mqtt-fields" style="margin-top:12px;">
          <input type="text" id="mqtt-host" placeholder="mqtt.example.com" class="wifi-input" style="margin-bottom:8px;">
          <input type="number" id="mqtt-port" placeholder="1883" class="wifi-input" style="margin-bottom:8px;">
          <input type="text" id="mqtt-user" placeholder="Username (optional)" class="wifi-input" style="margin-bottom:8px;">
          <input type="password" id="mqtt-pass" placeholder="Password (optional)" class="wifi-input" style="margin-bottom:8px;">
          <input type="text" id="mqtt-topic" placeholder="Topic" class="wifi-input" style="margin-bottom:12px;">
          <button class="btn btn-green" onclick="saveMqtt()">Save MQTT</button>
        </div>
        <div style="margin-top:8px; font-size:13px; color:var(--subtext);">
          Status: <span id="mqtt-status">Disabled</span>
        </div>
      </div>
    </div>

    <!-- SYSTEM TAB -->
    <div class="tab-panel" id="tab-system">
      <!-- Security -->
      <div style="margin-bottom:20px;">
        <div class="card-header" style="margin:-20px -20px 16px -20px; border-radius:0;">Security</div>
        <div>
          <label style="display:block; font-size:12px; color:var(--subtext); margin-bottom:4px;">Admin Password</label>
          <input type="password" id="admin-pass" placeholder="New password" class="wifi-input" style="margin-bottom:8px;">
          <button class="btn btn-green" onclick="saveAdminPass()">Save Admin Password</button>
        </div>
      </div>

      <!-- Firmware Update -->
      <div style="margin-bottom:20px;">
        <div class="card-header" style="margin:0 -20px 16px -20px; border-radius:0;">Firmware Update</div>
        <div id="ota-drop-zone" class="ota-drop-zone">
          <div class="ota-icon">📦</div>
          <div class="ota-text">Drop .bin file here or click to select</div>
          <input type="file" id="ota-file" accept=".bin" style="display:none;">
        </div>
        <div id="ota-progress" class="ota-progress" style="display:none;">
          <div class="ota-bar">
            <div class="ota-fill" id="ota-fill"></div>
          </div>
          <div id="ota-percent" class="ota-percent">0%</div>
        </div>
      </div>

      <!-- Logs -->
      <div>
        <div class="card-header" style="margin:0 -20px 16px -20px; border-radius:0;">System Logs</div>
        <label class="toggle-row" style="margin-bottom:10px;" onclick="toggleLogsEnable()">
          <span class="toggle-label">Enable logging</span>
          <div class="toggle-switch" onclick="event.stopPropagation(); toggleLogsEnable()">
            <input type="checkbox" id="logs-enable" checked>
            <span class="toggle-slider"></span>
          </div>
        </label>
        <div id="logs-area" class="logs-area"></div>
        <div class="logs-controls">
          <button class="btn btn-gray" onclick="refreshLogs()">Refresh</button>
          <button class="btn btn-red" onclick="clearLogs()">Clear</button>
          <label style="display:flex; align-items:center; gap:6px; font-size:13px; cursor:pointer;">
            <input type="checkbox" id="logs-auto" checked onchange="toggleLogsAuto(this.checked)">
            <span>Auto-refresh</span>
          </label>
          <label style="display:flex; align-items:center; gap:6px; font-size:13px;">
            <span>Interval:</span>
            <input type="number" id="logs-interval" value="5" min="1" max="30" style="width:50px; padding:4px; background:var(--surface2); border:1px solid var(--border); border-radius:4px; color:var(--text); font-size:13px; text-align:center;" onchange="onLogsIntervalChange(this.value)">
            <span>s</span>
          </label>
        </div>
      </div>
    </div>
  </div>

</main>

<script>

// ============================================================
// WEBSOCKET
// ============================================================
let ws;

function connect() {
  ws = new WebSocket('ws://' + window.location.hostname + '/ws');
  
  ws.onopen = () => {
    console.log('WS connected');
    setupOTA();
    send({type: 'get_status'});
  };
  
  ws.onmessage = (e) => {
    const msg = JSON.parse(e.data);
    handleMessage(msg);
  };
  
  ws.onclose = () => {
    console.log('WS disconnected');
    setTimeout(connect, 2000);
  };
}

function send(obj) {
  if (ws && ws.readyState === WebSocket.OPEN) {
    ws.send(JSON.stringify(obj));
  }
}

// ============================================================
// MESSAGE HANDLER
// ============================================================
function handleMessage(msg) {
  switch(msg.type) {
    case 'status':
      // WiFi status
      if (msg.ip) {
        setWifiStatus(true, msg.ip);
      } else {
        setWifiStatus(false, 'AP mode');
      }
      
      // Input selection
      if (msg.input !== undefined) updateInput(msg.input);
      
      // Relay masks
      if (msg.relay_masks) setRelayMasks(msg.relay_masks);
      
      // Brightness
      if (msg.brightness !== undefined) {
        document.getElementById('brightness').value = msg.brightness;
        updateBrightnessDisplay(msg.brightness);
      }
      
      // Display settings
      if (msg.disp_invert !== undefined) {
        document.getElementById('disp-invert').checked = msg.disp_invert;
      }
      if (msg.disp_rotate !== undefined) {
        document.getElementById('disp-rotate').checked = msg.disp_rotate;
      }
      
      // Audio
      if (msg.audio_vol !== undefined) {
        document.getElementById('audio-volume').value = msg.audio_vol;
        updateVolumeDisplay(msg.audio_vol);
      }
      if (msg.audio_bass !== undefined) {
        document.getElementById('audio-bass').value = msg.audio_bass;
        updateBassDisplay(msg.audio_bass);
      }
      if (msg.audio_treble !== undefined) {
        document.getElementById('audio-treble').value = msg.audio_treble;
        updateTrebleDisplay(msg.audio_treble);
      }
      
      // IR codes
      if (msg.ir) setIRCodes(msg.ir);
      
      // WiFi
      if (msg.wifi_ssid) document.getElementById('wifi-ssid').value = msg.wifi_ssid;
      
      // MQTT
      if (msg.mqtt_host) document.getElementById('mqtt-host').value = msg.mqtt_host;
      if (msg.mqtt_port) document.getElementById('mqtt-port').value = msg.mqtt_port;
      if (msg.mqtt_user) document.getElementById('mqtt-user').value = msg.mqtt_user;
      const mqttPassEl = document.getElementById('mqtt-pass');
      if (mqttPassEl) {
        mqttPassEl.placeholder = msg.mqtt_pass_set ? '••••••••' : 'Password (optional)';
      }
      if (msg.mqtt_topic) document.getElementById('mqtt-topic').value = msg.mqtt_topic;
      if (msg.mqtt_enabled !== undefined) {
        const mqttEnable = document.getElementById('mqtt-enable');
        if (mqttEnable) {
          mqttEnable.checked = msg.mqtt_enabled;
          setMqttFieldsEnabled(msg.mqtt_enabled);
        }
      }
      setMqttStatus(!!msg.mqtt_connected);
      break;
      
    case 'input_changed':
      if (msg.input !== undefined) updateInput(msg.input);
      break;
      
    case 'audio_changed':
      if (msg.vol !== undefined) {
        document.getElementById('audio-volume').value = msg.vol;
        updateVolumeDisplay(msg.vol);
      }
      if (msg.bass !== undefined) {
        document.getElementById('audio-bass').value = msg.bass;
        updateBassDisplay(msg.bass);
      }
      if (msg.treble !== undefined) {
        document.getElementById('audio-treble').value = msg.treble;
        updateTrebleDisplay(msg.treble);
      }
      break;
      
    case 'ir_received':
      if (msg.key && msg.code !== undefined) {
        if (irCountdowns[msg.key]) {
          clearInterval(irCountdowns[msg.key]);
          delete irCountdowns[msg.key];
        }
        const cdEl = document.getElementById('cd-' + msg.key);
        if (cdEl) cdEl.textContent = '';
        
        const codeEl = document.getElementById('ir-' + msg.key);
        if (codeEl) {
          codeEl.value = msg.code || 'not set';
        }
        
        showToast('IR code learned!', 'green');
      }
      break;
      
    case 'ir_timeout':
      if (msg.key) {
        if (irCountdowns[msg.key]) {
          clearInterval(irCountdowns[msg.key]);
          delete irCountdowns[msg.key];
        }
        const cdEl = document.getElementById('cd-' + msg.key);
        if (cdEl) cdEl.textContent = '';
        
        showToast('IR timeout - no signal received', 'red');
      }
      break;
      
    case 'ir_cleared':
      if (msg.key) {
        document.getElementById('ir-' + msg.key).value = 'not set';
      }
      break;
      
    case 'wifi_test_ok':
      setWifiStatus(true, msg.ip || 'Connected');
      showToast('WiFi test OK: ' + (msg.ip || ''), 'green');
      break;
      
    case 'wifi_test_fail':
      setWifiStatus(false, 'Test failed');
      showToast('WiFi test failed', 'red');
      break;
      
    case 'relays_updated':
      if (msg.relay_masks) setRelayMasks(msg.relay_masks);
      showToast('Relays updated!', 'green');
      break;
      
    case 'ir_saved':
      showToast('IR codes saved!', 'green');
      break;
      
    case 'mqtt_saved':
      showToast('MQTT saved!', 'green');
      setMqttStatus(false, false);
      break;
      
    case 'mqtt_connected':
      setMqttStatus(true);
      break;
      
    case 'mqtt_disconnected':
      setMqttStatus(false, false);
      break;
      
    case 'status_updated':
      if (msg.disp_invert !== undefined) {
        document.getElementById('disp-invert').checked = msg.disp_invert;
      }
      if (msg.disp_rotate !== undefined) {
        document.getElementById('disp-rotate').checked = msg.disp_rotate;
      }
      break;
      
    case 'wifi_saved':
      showToast('WiFi saved — rebooting...', 'green');
      break;
      
    case 'admin_saved':
      showToast('Admin password saved!', 'green');
      break;
      
    case 'auth_ok':
      showToast('Authenticated', 'green');
      break;
      
    case 'auth_fail':
      showToast('Authentication failed', 'red');
      break;
      
    case 'error':
      console.error('WS error:', msg.message);
      showToast('Error: ' + (msg.message || 'Unknown'), 'red');
      break;
      
    case 'ota_progress':
      if (msg.percent !== undefined) {
        const fill = document.getElementById('ota-fill');
        const pct = document.getElementById('ota-percent');
        if (fill) fill.style.width = msg.percent + '%';
        if (pct) pct.textContent = msg.percent + '%';
      }
      break;
      
    case 'ota_ok':
      showToast('Update successful! Rebooting...', 'green');
      setTimeout(() => location.reload(), 5000);
      break;
      
    case 'ota_fail':
      showToast('Update failed', 'red');
      break;
  }
}

// ============================================================
// TABS
// ============================================================
function switchTab(tabName) {
  document.querySelectorAll('.tab-panel').forEach(p => p.classList.remove('active'));
  document.querySelectorAll('.tab-btn').forEach(b => b.classList.remove('active'));
  
  const panel = document.getElementById('tab-' + tabName);
  const btn = document.getElementById('tab-btn-' + tabName);
  if (panel) panel.classList.add('active');
  if (btn) btn.classList.add('active');
}

// ============================================================
// STATUS DOTS
// ============================================================
function setWifiStatus(online, text) {
  const el = document.getElementById('status-wifi');
  if (!el) return;
  el.classList.remove('online', 'offline');
  el.classList.add(online ? 'online' : 'offline');
  const span = el.querySelector('span');
  if (span && text) span.textContent = text;
}

function setMqttStatus(connected, enabled) {
  const el = document.getElementById('status-mqtt');
  if (!el) return;
  el.classList.remove('online', 'offline');
  
  const mqttEnabled = document.getElementById('mqtt-enable')?.checked;
  if (!mqttEnabled) {
    el.classList.add('offline');
    el.querySelector('span').textContent = 'MQTT';
    document.getElementById('mqtt-status').textContent = 'Disabled';
    return;
  }
  
  if (connected) {
    el.classList.add('online');
    document.getElementById('mqtt-status').textContent = 'Connected';
  } else {
    el.classList.add('offline');
    document.getElementById('mqtt-status').textContent = 'Disconnected';
  }
}

// ============================================================
// INPUTS
// ============================================================
function selectInput(input) {
  send({type: 'set_input', input: input});
}

function updateInput(input) {
  for (let i = 0; i < 4; i++) {
    const tile = document.getElementById('tile-' + i);
    if (tile) {
      if (i === input) {
        tile.classList.add('active');
      } else {
        tile.classList.remove('active');
      }
    }
  }
}

function setRelayMasks(masks) {
  for (let i = 0; i < 4; i++) {
    const cb = document.getElementById('relay-' + i);
    const item = document.getElementById('relay-item-' + i);
    if (cb) cb.checked = !!(masks[i]);
    if (item) {
      if (masks[i]) item.classList.add('active');
      else item.classList.remove('active');
    }
  }
}

function toggleRelay(index) {
  const cb = document.getElementById('relay-' + index);
  if (cb) {
    cb.checked = !cb.checked;
    setRelayMasks([
      document.getElementById('relay-0').checked ? 1 : 0,
      document.getElementById('relay-1').checked ? 1 : 0,
      document.getElementById('relay-2').checked ? 1 : 0,
      document.getElementById('relay-3').checked ? 1 : 0
    ]);
    saveRelays();
  }
}

function saveRelays() {
  const masks = [0, 0, 0, 0];
  for (let i = 0; i < 4; i++) {
    const cb = document.getElementById('relay-' + i);
    if (cb && cb.checked) masks[i] = 1;
  }
  send({
    type: 'save_relays',
    mask0: masks[0],
    mask1: masks[1],
    mask2: masks[2],
    mask3: masks[3]
  });
}

// ============================================================
// BRIGHTNESS
// ============================================================
function onBrightnessChange(val) {
  updateBrightnessDisplay(val);
  send({type: 'set_brightness', value: parseInt(val)});
}

function updateBrightnessDisplay(val) {
  const pct = Math.round(val * 100 / 255);
  document.getElementById('brightness-val').textContent = pct + '%';
}

// ============================================================
// DISPLAY - Toggle helpers (fix for invisible checkbox)
// ============================================================
function toggleInvert() {
  const cb = document.getElementById('disp-invert');
  if (cb) {
    cb.checked = !cb.checked;
    send({type: 'set_invert', value: cb.checked});
  }
}

function toggleRotate() {
  const cb = document.getElementById('disp-rotate');
  if (cb) {
    cb.checked = !cb.checked;
    send({type: 'set_rotate', value: cb.checked});
  }
}

// ============================================================
// AUDIO
// ============================================================
function onVolumeChange(val) {
  const percent = parseInt(val);
  document.getElementById('volume-val').textContent = percent + '%';
  send({type: 'set_volume', value: percent});
}

function updateVolumeDisplay(val) {
  const percent = parseInt(val);
  document.getElementById('volume-val').textContent = percent + '%';
}

function onBassChange(val) {
  updateBassDisplay(val);
  send({type: 'set_bass', value: parseInt(val)});
}

function updateBassDisplay(val) {
  const v = parseInt(val);
  document.getElementById('bass-val').textContent = (v > 0 ? '+' : '') + v + ' dB';
}

function onTrebleChange(val) {
  updateTrebleDisplay(val);
  send({type: 'set_treble', value: parseInt(val)});
}

function updateTrebleDisplay(val) {
  const v = parseInt(val);
  document.getElementById('treble-val').textContent = (v > 0 ? '+' : '') + v + ' dB';
}

// ============================================================
// IR REMOTE
// ============================================================
let irCountdowns = {};

function learnIR(key) {
  send({type: 'learn_ir', key: key});
  
  let countdown = 10;
  const cdEl = document.getElementById('cd-' + key);
  if (!cdEl) return;
  
  const input = document.getElementById('ir-' + key);
  input.value = 'Press remote button...';
  
  if (irCountdowns[key]) clearInterval(irCountdowns[key]);
  
  irCountdowns[key] = setInterval(() => {
    cdEl.textContent = countdown + 's';
    countdown--;
    if (countdown < 0) {
      clearInterval(irCountdowns[key]);
      cdEl.textContent = '';
    }
  }, 1000);
}

function clearIR(key) {
  const el = document.getElementById('ir-' + key);
  if (el && el.value && el.value !== 'not set') {
    if (confirm('Clear IR code for ' + key + '?')) {
      el.value = 'not set';
      send({type: 'clear_ir', key: key});
    }
  }
}

function setIRCodes(codes) {
  const keys = ['source', 'input1', 'input2', 'input3', 'input4', 'volp', 'volm', 'up', 'down'];
  keys.forEach(key => {
    if (codes[key] !== undefined) {
      const el = document.getElementById('ir-' + key);
      if (el) el.value = codes[key] || 'not set';
    }
  });
}

function saveIR() {
  const keys = ['source', 'input1', 'input2', 'input3', 'input4', 'volp', 'volm', 'up', 'down'];
  const codes = {};
  keys.forEach(key => {
    const el = document.getElementById('ir-' + key);
    if (el) {
      const val = el.value;
      codes[key] = (val && val !== 'not set') ? parseInt(val) : 0;
    }
  });
  
  send({
    type: 'save_ir',
    source: codes.source,
    input1: codes.input1,
    input2: codes.input2,
    input3: codes.input3,
    input4: codes.input4,
    volp: codes.volp,
    volm: codes.volm,
    up: codes.up,
    down: codes.down
  });
}

// ============================================================
// WIFI
// ============================================================
function testWifi() {
  const ssid = document.getElementById('wifi-ssid').value;
  const pass = document.getElementById('wifi-pass').value;
  if (!ssid) { showToast('Enter SSID', 'red'); return; }
  
  send({type: 'test_wifi', ssid: ssid, pass: pass});
  
  const statusEl = document.getElementById('wifi-status');
  statusEl.textContent = 'Testing...';
  statusEl.style.background = 'var(--surface2)';
  statusEl.style.color = 'var(--text)';
}

function saveWifi() {
  const ssid = document.getElementById('wifi-ssid').value;
  const pass = document.getElementById('wifi-pass').value;
  if (!ssid) { showToast('Enter SSID', 'red'); return; }
  
  send({type: 'save_wifi', ssid: ssid, pass: pass});
  showToast('WiFi saved — rebooting...', 'green');
}

function showWifiStatus(success, message) {
  const el = document.getElementById('wifi-status');
  el.textContent = message;
  if (success) {
    el.style.background = 'rgba(45,197,94,0.15)';
    el.style.color = 'var(--green)';
  } else {
    el.style.background = 'rgba(224,40,40,0.15)';
    el.style.color = 'var(--red)';
  }
}

// ============================================================
// MQTT
// ============================================================
function toggleMqttEnable() {
  const cb = document.getElementById('mqtt-enable');
  if (cb) {
    cb.checked = !cb.checked;
    setMqttFieldsEnabled(cb.checked);
    if (!cb.checked) setMqttStatus(false);
    // Wyslij do backendu natychmiastowa zmiane enabled
    send({type: 'set_mqtt_enabled', enabled: cb.checked});
  }
}

function setMqttFieldsEnabled(enabled) {
  const fields = document.getElementById('mqtt-fields');
  if (fields) {
    fields.style.opacity = enabled ? '1' : '0.4';
    fields.style.pointerEvents = enabled ? 'auto' : 'none';
  }
}

function saveMqtt() {
  const enabled = document.getElementById('mqtt-enable').checked;
  const host = document.getElementById('mqtt-host').value;
  const port = parseInt(document.getElementById('mqtt-port').value) || 1883;
  const user = document.getElementById('mqtt-user').value;
  const pass = document.getElementById('mqtt-pass').value;
  const topic = document.getElementById('mqtt-topic').value;
  
  send({
    type: 'save_mqtt',
    enabled: enabled,
    host: host,
    port: port,
    user: user,
    pass: pass,
    topic: topic
  });
}

// ============================================================
// SECURITY
// ============================================================
function saveAdminPass() {
  const pass = document.getElementById('admin-pass').value;
  if (!pass) { showToast('Enter password', 'red'); return; }
  if (pass.length < 4) { showToast('Password too short (min 4)', 'red'); return; }
  
  send({type: 'save_admin', pass: pass});
  document.getElementById('admin-pass').value = '';
  showToast('Admin password saved!', 'green');
}

// ============================================================
// OTA
// ============================================================
function setupOTA() {
  const dropZone = document.getElementById('ota-drop-zone');
  const fileInput = document.getElementById('ota-file');
  
  dropZone.addEventListener('click', () => fileInput.click());
  
  dropZone.addEventListener('dragover', (e) => {
    e.preventDefault();
    dropZone.style.borderColor = 'var(--green)';
  });
  
  dropZone.addEventListener('dragleave', () => {
    dropZone.style.borderColor = 'var(--border)';
  });
  
  dropZone.addEventListener('drop', (e) => {
    e.preventDefault();
    dropZone.style.borderColor = 'var(--border)';
    const files = e.dataTransfer.files;
    if (files.length > 0) uploadFirmware(files[0]);
  });
  
  fileInput.addEventListener('change', (e) => {
    if (e.target.files.length > 0) uploadFirmware(e.target.files[0]);
  });
}

function uploadFirmware(file) {
  if (!file.name.endsWith('.bin')) {
    showToast('Select a .bin file!', 'red');
    return;
  }
  
  setOtaProgress(0, 'Uploading...');
  showToast('Uploading firmware...', '');

  // WAŻNE: Użyj FormData dla multipart/form-data
  const formData = new FormData();
  formData.append('update', file);

  const xhr = new XMLHttpRequest();
  xhr.open('POST', '/update');

  xhr.upload.onprogress = (e) => {
    if (e.lengthComputable) {
      const pct = Math.round(e.loaded / e.total * 100);
      setOtaProgress(pct, 'Uploading... ' + pct + '%');
    }
  };
  
  xhr.onload = () => {
    try {
      const res = JSON.parse(xhr.responseText);
      if (res.ok) {
        setOtaProgress(100, 'Success! Rebooting...');
        showToast('Update success! Device rebooting...', 'green');
        setTimeout(() => location.reload(), 5000);
      } else {
        setOtaProgress(0, 'Failed: ' + (res.error || 'Unknown error'));
        showToast('Update failed: ' + (res.error || 'Unknown error'), 'red');
      }
    } catch (e) {
      setOtaProgress(0, 'Update failed');
      showToast('Update failed', 'red');
    }
  };
  
  xhr.onerror = () => {
    setOtaProgress(0, 'Network error');
    showToast('Network error during upload', 'red');
  };

  // Wyślij jako FormData
  xhr.send(formData);
}

function setOtaProgress(percent, text) {
  const prog = document.getElementById('ota-progress');
  const fill = document.getElementById('ota-fill');
  const pct = document.getElementById('ota-percent');
  const zone = document.getElementById('ota-drop-zone');
  
  if (percent > 0) {
    if (zone) zone.style.display = 'none';
    if (prog) prog.style.display = 'block';
    if (fill) fill.style.width = percent + '%';
    if (pct) pct.textContent = text || (percent + '%');
  } else {
    if (zone) zone.style.display = 'block';
    if (prog) prog.style.display = 'none';
  }
}

// ============================================================
// LOGS
// ============================================================
let logsAutoRefresh = true;
let logsInterval = 5000;
let logsTimer = null;
let logsEnabled = true;

function toggleLogsEnable() {
  const cb = document.getElementById('logs-enable');
  if (cb) {
    cb.checked = !cb.checked;
    logsEnabled = cb.checked;
    if (!logsEnabled) {
      document.getElementById('logs-area').innerHTML = '<span style="color:#444">// Logging disabled</span>';
      if (logsTimer) { clearInterval(logsTimer); logsTimer = null; }
    } else {
      refreshLogs();
      startLogsTimer();
    }
  }
}

function refreshLogs() {
  if (!logsEnabled) return;
  fetch('/api/logs')
    .then(r => r.text())
    .then(text => {
      const area = document.getElementById('logs-area');
      area.innerHTML = formatLogs(text);
      area.scrollTop = area.scrollHeight;
    })
    .catch(() => {});
}

function formatLogs(text) {
  if (!text || text.trim().length === 0) return '<span style="color:#444">// No logs</span>';
  return text.split('\n').map(line => {
    if (!line.trim()) return '';
    // Koloruj timestamp [MM:SS.mmm]
    line = line.replace(/^(\[\d{2}:\d{2}\.\d{3}\])/, '<span class="log-time">$1</span>');
    // Koloruj poziomy logów
    if (line.includes('ERROR') || line.includes('Failed') || line.includes('fail')) {
      line = '<span class="log-err">' + line + '</span>';
    } else if (line.includes('WARN') || line.includes('warning')) {
      line = '<span class="log-warn">' + line + '</span>';
    } else if (line.includes('MQTT:') || line.includes('WiFi:') || line.includes('HTTP:')) {
      // Subtelne podkolorowanie tagów systemowych
      line = line.replace(/(MQTT:|WiFi:|HTTP:|OTA:|IR:|NVS:)/g, '<span class="log-info">$1</span>');
    }
    return line;
  }).filter(l => l).join('<br>');
}

function clearLogs() {
  fetch('/api/logs/clear', {method: 'POST'})
    .then(() => {
      document.getElementById('logs-area').innerHTML = '<span style="color:#444">// Logs cleared</span>';
      showToast('Logs cleared', 'green');
    })
    .catch(() => {});
}

function toggleLogsAuto(enabled) {
  logsAutoRefresh = enabled;
  if (enabled) {
    startLogsTimer();
  } else {
    if (logsTimer) { clearInterval(logsTimer); logsTimer = null; }
  }
}

function onLogsIntervalChange(val) {
  const sec = parseInt(val);
  if (sec >= 1 && sec <= 30) {
    logsInterval = sec * 1000;
    if (logsAutoRefresh) startLogsTimer();
  }
}

function startLogsTimer() {
  if (logsTimer) clearInterval(logsTimer);
  if (logsAutoRefresh && logsEnabled) {
    logsTimer = setInterval(refreshLogs, logsInterval);
  }
}

// ============================================================
// TOAST
// ============================================================
function showToast(message, color) {
  const existing = document.querySelector('.toast');
  if (existing) existing.remove();
  
  const toast = document.createElement('div');
  toast.className = 'toast';
  toast.textContent = message;
  if (color === 'red') {
    toast.style.borderColor = 'var(--red)';
    toast.style.color = 'var(--red)';
  }
  document.body.appendChild(toast);
  
  setTimeout(() => toast.remove(), 3000);
}

// ============================================================
// INIT
// ============================================================
connect();
startLogsTimer();
</script>

</body>
</html>

)rawliteral";

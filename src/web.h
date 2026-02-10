// ============================================================
// Auto-generated web interface - PROGMEM
// Generated from: index.html
// Size: 74450 bytes
// ============================================================
#pragma once
#include <pgmspace.h>

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Audio Switcher</title>
<style>
  :root {
    --bg:       #121212;
    --surface:  #1e1e1e;
    --surface2: #2a2a2a;
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
    padding: 16px 0;
    display: flex;
    align-items: center;
    justify-content: space-between;
  }
  header img.logo {
    height: 38px;
    image-rendering: pixelated;
  }
  .wifi-badge {
    display: flex;
    align-items: center;
    gap: 8px;
    font-size: 13px;
    color: var(--subtext);
  }
  .wifi-dot {
    width: 8px; height: 8px;
    border-radius: 50%;
    background: var(--green);
    box-shadow: 0 0 6px var(--green);
    animation: pulse 2s infinite;
  }
  @keyframes pulse {
    0%,100% { opacity: 1; }
    50%      { opacity: 0.4; }
  }

  /* MAIN CONTENT */
  main {
    max-width: 600px;
    margin: 0 auto;
    padding: 24px 16px;
    display: flex;
    flex-direction: column;
    gap: 20px;
  }

  /* SECTION CARD */
  .card {
    background: var(--surface);
    border: 1px solid var(--border);
    border-radius: var(--radius);
    overflow: hidden;
  }
  .card-header {
    padding: 14px 20px;
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

  /* INPUTS */
  .inputs-grid {
    display: grid;
    grid-template-columns: repeat(4, 1fr);
    gap: 12px;
  }
  .input-tile {
    aspect-ratio: 1;
    background: #000;
    border: 2px solid var(--border);
    border-radius: 10px;
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    gap: 8px;
    cursor: pointer;
    transition: border-color .2s, box-shadow .2s, transform .1s;
    user-select: none;
  }
  .input-tile:hover {
    border-color: #555;
    transform: translateY(-1px);
  }
  .input-tile:active {
    transform: scale(0.96);
  }
  .input-tile.active {
    border-color: var(--green);
    box-shadow: 0 0 0 1px var(--green), 0 4px 16px rgba(45,197,94,.2);
  }
  .input-tile img {
    width: 40px;
    height: 40px;
    image-rendering: pixelated;
    opacity: 0.6;
    transition: opacity .2s;
  }
  .input-tile.active img {
    opacity: 1;
    filter: drop-shadow(0 0 4px var(--green));
  }
  .input-tile span {
    font-size: 11px;
    font-weight: 600;
    letter-spacing: 1px;
    color: var(--subtext);
    text-transform: uppercase;
  }
  .input-tile.active span {
    color: var(--green);
  }

  /* REMOTE / IR */
  .ir-rows {
    display: flex;
    flex-direction: column;
    gap: 12px;
  }
  .ir-row {
    display: flex;
    align-items: center;
    gap: 12px;
  }
  .ir-label {
    width: 80px;
    font-size: 13px;
    color: var(--subtext);
    flex-shrink: 0;
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
  .ir-code.learning {
    border-color: var(--green);
    color: var(--green);
    animation: blink-border 0.6s infinite;
  }
  @keyframes blink-border {
    0%,100% { border-color: var(--green); }
    50%      { border-color: transparent; }
  }
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

  /* COUNTDOWN */
  .countdown {
    font-size: 12px;
    color: var(--green);
    width: 32px;
    text-align: center;
    flex-shrink: 0;
    display: none;
  }
  .countdown.visible { display: block; }

  /* IR SAVE */
  .ir-save-row {
    margin-top: 8px;
    display: flex;
    justify-content: flex-end;
  }

  /* WIFI SETTINGS */
  .wifi-form {
    display: flex;
    flex-direction: column;
    gap: 14px;
  }
  .form-group {
    display: flex;
    flex-direction: column;
    gap: 6px;
  }
  .form-group label {
    font-size: 12px;
    font-weight: 600;
    color: var(--subtext);
    letter-spacing: 0.8px;
    text-transform: uppercase;
  }
  .form-group input {
    background: var(--bg);
    border: 1px solid var(--border);
    border-radius: 8px;
    padding: 10px 14px;
    font-size: 14px;
    color: var(--text);
    outline: none;
    transition: border-color .2s;
  }
  .form-group input:focus {
    border-color: var(--green);
  }
  .wifi-save-row {
    display: flex;
    justify-content: flex-end;
    gap: 10px;
    margin-top: 4px;
  }

  /* TOAST */
  .toast {
    position: fixed;
    bottom: 24px;
    left: 50%;
    transform: translateX(-50%) translateY(80px);
    background: #000;
    border: 1px solid var(--border);
    border-radius: 10px;
    padding: 12px 24px;
    font-size: 14px;
    transition: transform .3s ease;
    z-index: 999;
    white-space: nowrap;
  }
  .toast.show {
    transform: translateX(-50%) translateY(0);
  }
  .toast.green { border-color: var(--green); color: var(--green); }
  .toast.red   { border-color: var(--red);   color: var(--red);   }

  /* STATUS BAR */
  .status-bar {
    background: #000;
    border-top: 1px solid var(--border);
    padding: 8px 20px;
    font-size: 11px;
    color: var(--subtext);
    display: flex;
    justify-content: space-between;
  }
</style>
</head>
<body>

<header>
  <div class="header-inner">
    <img class="logo" src="data:image/png;base64,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X8oHTtH0YVnZTJ02lRXLl/Pa669x2223M2GinTx8yy23cM0113DuOecwe85sbrjhBpqbm51cQFv67xfwjYJskjBRQjgVPQWbQ/DHmmZnbPeUYSkam46WFlrJOLfIhdC25kwLQajToHpvO6l5bo6/eDhuv+nwM056kDKcLAOJQNHZGgQE0iNIyfaCUig0UhsIPM5mdSZaRAgLRcYIwawzS/n3H3cy+fjhCK99fUO50Upilw+z87VMIiQN08w4vZjKjQcpGZ9Jco5wri9RlkRbCkNotGFhISE5yMQTc1mzZC+HK2fl6k2mnH/eV5g9exa/e+wxHnroIe69917mz59PU1MT9917D1kZmdx0881MnjSJwsVeKscbeArSqHA3o3IlhlZE9oWofXE71uaOxHB1P+GCgTyIiooKzj777BgEVkpz9cKria9xcNddP+Ouu38WE9e88s9XePWfr8a+19j7Ie3fvz828V78+wv8/e8vJpDAMV9UShY/vpgJEyfw+muv8cDPH4hzBMXg0E1v/xRNSoGH9DwfSX6DEZOz2PexXfJh+PhhGB4whMBIsigsTadlf6czyI24ltkbwsanj/SnqXEBRV6JU1uAHI/LIQJtiO/ScGtOOltCYV5u76Jns2bBf+VmU9HZxQfdwVipSiEOjUS11owZO4ZAMMD2HTs58fgTYuVEs4dl8/eX/s4D99/Pb/+f36KUoqGxAaUUfr+fk046mW3btzGqfBRXXXUVefl5PPzLh1m48BoALr7kYtCaRx59lNLSUoLBIF6Plzxnh4cNGzZy4YUXYymLc887D2Up7lj0UwoKCqiuqeaXv/wlN910ExpNU3NPeY3TTj8dv99PyfDhXHHFVeTk5PD888/z7nvvsnnzFk499TSkFDz7zDM89dRTMc4jmnh5aV4GE7wSrSyHNtU0aBe/rayhq59uyhueYrucWtN10GDdW3UoS8QQrBCCUIeiuznCcReU40uTKAeprFvSyI419YQCJkIKPEkevD43XS0WoEnN9pKU6kYTBi346NV6qnd0AZr8kSmceEk+YBfKSvK7AYXPm0JblQQMand08vE7u4iYEXKKUjj322PBbRdNV8reeK9weC4qaEDYoLkmwjsv7iTQ0U1qmpdzv1uOJ9UuC4yIJOaN6cMZHGdcz503j+5AgCeffILTTzud6667juRku7q/ZUa46uqF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" alt="Audio Switcher">
    <div class="wifi-badge">
      <div class="wifi-dot"></div>
      <span id="ip-addr">connecting...</span>
    </div>
  </div>
</header>

<main>

  <!-- INPUTS -->
  <div class="card">
    <div class="card-header">Inputs</div>
    <div class="card-body">
      <div class="inputs-grid">
        <div class="input-tile" id="tile-0" onclick="selectInput(0)">
          <img src="data:image/png;base64,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" alt="TV">
          <span>TV</span>
        </div>
        <div class="input-tile" id="tile-1" onclick="selectInput(1)">
          <img src="data:image/png;base64,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" alt="BT">
          <span>BT</span>
        </div>
        <div class="input-tile" id="tile-2" onclick="selectInput(2)">
          <img src="data:image/png;base64,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" alt="NET">
          <span>NET</span>
        </div>
        <div class="input-tile" id="tile-3" onclick="selectInput(3)">
          <img src="data:image/png;base64,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" alt="RES">
          <span>RES</span>
        </div>
      </div>
    </div>
  </div>

  <!-- REMOTE -->
  <div class="card">
    <div class="card-header">Remote Control — IR Codes</div>
    <div class="card-body">
      <div class="ir-rows">
        <div class="ir-row">
          <span class="ir-label">Source</span>
          <input class="ir-code" id="ir-source" type="text" readonly placeholder="not set">
          <span class="countdown" id="cd-source"></span>
          <button class="btn btn-gray" onclick="learnIR('source')">Learn</button>
        </div>
        <div class="ir-row">
          <span class="ir-label">Input 1 — TV</span>
          <input class="ir-code" id="ir-input1" type="text" readonly placeholder="not set">
          <span class="countdown" id="cd-input1"></span>
          <button class="btn btn-gray" onclick="learnIR('input1')">Learn</button>
        </div>
        <div class="ir-row">
          <span class="ir-label">Input 2 — BT</span>
          <input class="ir-code" id="ir-input2" type="text" readonly placeholder="not set">
          <span class="countdown" id="cd-input2"></span>
          <button class="btn btn-gray" onclick="learnIR('input2')">Learn</button>
        </div>
        <div class="ir-row">
          <span class="ir-label">Input 3 — NET</span>
          <input class="ir-code" id="ir-input3" type="text" readonly placeholder="not set">
          <span class="countdown" id="cd-input3"></span>
          <button class="btn btn-gray" onclick="learnIR('input3')">Learn</button>
        </div>
        <div class="ir-row">
          <span class="ir-label">Input 4 — RES</span>
          <input class="ir-code" id="ir-input4" type="text" readonly placeholder="not set">
          <span class="countdown" id="cd-input4"></span>
          <button class="btn btn-gray" onclick="learnIR('input4')">Learn</button>
        </div>
      </div>
      <div class="ir-save-row">
        <button class="btn btn-green" onclick="saveIR()">Save IR Codes</button>
      </div>
    </div>
  </div>

  <!-- WIFI SETTINGS -->
  <div class="card">
    <div class="card-header">WiFi Settings</div>
    <div class="card-body">
      <div class="wifi-form">
        <div class="form-group">
          <label>Network SSID</label>
          <input type="text" id="wifi-ssid" placeholder="Your WiFi network name">
        </div>
        <div class="form-group">
          <label>Password</label>
          <input type="password" id="wifi-pass" placeholder="WiFi password">
        </div>
        <div class="wifi-save-row">
          <button class="btn btn-gray" onclick="testWifi()">Test</button>
          <button class="btn btn-green" onclick="saveWifi()">Save &amp; Restart</button>
        </div>
      </div>
    </div>
  </div>

</main>

<div class="toast" id="toast"></div>

<script>
// ============================================================
// WebSocket
// ============================================================
let ws;
let currentInput = 0;
let learningKey = null;
let countdownInterval = null;

function connectWS() {
  ws = new WebSocket('ws://' + location.hostname + '/ws');

  ws.onopen = () => {
    console.log('WS connected');
    ws.send(JSON.stringify({type: 'get_status'}));
  };

  ws.onmessage = (e) => {
    const msg = JSON.parse(e.data);
    handleMessage(msg);
  };

  ws.onclose = () => {
    console.log('WS closed, reconnecting...');
    setTimeout(connectWS, 2000);
  };

  ws.onerror = () => ws.close();
}

function send(obj) {
  if (ws && ws.readyState === WebSocket.OPEN) {
    ws.send(JSON.stringify(obj));
  }
}

// ============================================================
// Message handler
// ============================================================
function handleMessage(msg) {
  switch(msg.type) {

    case 'status':
      // Pełny status przy połączeniu
      setActiveInput(msg.input);
      document.getElementById('ip-addr').textContent = msg.ip || '';
      if (msg.ir) {
        setIRCodes(msg.ir);
      }
      if (msg.wifi_ssid) {
        document.getElementById('wifi-ssid').value = msg.wifi_ssid;
      }
      break;

    case 'input_changed':
      setActiveInput(msg.input);
      showToast('Input ' + (msg.input + 1) + ' selected', 'green');
      break;

    case 'ir_received':
      // Kod IR odebrany podczas nauki
      if (learningKey === msg.key) {
        stopLearning(msg.key, msg.code);
      }
      break;

    case 'ir_timeout':
      if (learningKey === msg.key) {
        stopLearning(msg.key, null);
        showToast('No IR signal received', 'red');
      }
      break;

    case 'ir_saved':
      showToast('IR codes saved!', 'green');
      break;

    case 'wifi_saved':
      showToast('WiFi saved! Restarting...', 'green');
      break;

    case 'wifi_test_ok':
      showToast('WiFi OK: ' + msg.ip, 'green');
      break;

    case 'wifi_test_fail':
      showToast('WiFi connection failed!', 'red');
      break;
  }
}

// ============================================================
// INPUTS
// ============================================================
function selectInput(index) {
  send({type: 'set_input', input: index});
}

function setActiveInput(index) {
  currentInput = index;
  document.querySelectorAll('.input-tile').forEach((tile, i) => {
    tile.classList.toggle('active', i === index);
  });
}

// ============================================================
// IR LEARNING
// ============================================================
function learnIR(key) {
  if (learningKey) return;  // Już uczymy

  learningKey = key;
  const input = document.getElementById('ir-' + key);
  const cd    = document.getElementById('cd-' + key);

  input.classList.add('learning');
  input.value = 'Press remote button...';
  cd.classList.add('visible');

  // Countdown 10s
  let secs = 10;
  cd.textContent = secs + 's';
  countdownInterval = setInterval(() => {
    secs--;
    cd.textContent = secs + 's';
    if (secs <= 0) clearInterval(countdownInterval);
  }, 1000);

  // Wyślij do ESP32
  send({type: 'learn_ir', key: key});
}

function stopLearning(key, code) {
  clearInterval(countdownInterval);
  learningKey = null;

  const input = document.getElementById('ir-' + key);
  const cd    = document.getElementById('cd-' + key);

  input.classList.remove('learning');
  cd.classList.remove('visible');

  if (code) {
    input.value = '0x' + code.toString(16).toUpperCase().padStart(8, '0');
    showToast('IR code learned!', 'green');
  } else {
    input.value = input.dataset.prev || '';
  }
}

function setIRCodes(ir) {
  const keys = ['source', 'input1', 'input2', 'input3', 'input4'];
  keys.forEach(key => {
    if (ir[key]) {
      const el = document.getElementById('ir-' + key);
      el.value = '0x' + ir[key].toString(16).toUpperCase().padStart(8, '0');
      el.dataset.prev = el.value;
    }
  });
}

function saveIR() {
  const keys = ['source', 'input1', 'input2', 'input3', 'input4'];
  const codes = {};
  keys.forEach(key => {
    const val = document.getElementById('ir-' + key).value;
    if (val && val !== 'not set') {
      codes[key] = parseInt(val, 16);
    }
  });
  send({type: 'save_ir', codes: codes});
}

// ============================================================
// WIFI
// ============================================================
function saveWifi() {
  const ssid = document.getElementById('wifi-ssid').value.trim();
  const pass = document.getElementById('wifi-pass').value;
  if (!ssid) { showToast('Enter SSID!', 'red'); return; }
  send({type: 'save_wifi', ssid: ssid, pass: pass});
}

function testWifi() {
  const ssid = document.getElementById('wifi-ssid').value.trim();
  const pass = document.getElementById('wifi-pass').value;
  if (!ssid) { showToast('Enter SSID!', 'red'); return; }
  send({type: 'test_wifi', ssid: ssid, pass: pass});
  showToast('Testing WiFi...', '');
}

// ============================================================
// TOAST
// ============================================================
function showToast(msg, type) {
  const t = document.getElementById('toast');
  t.textContent = msg;
  t.className = 'toast ' + type + ' show';
  setTimeout(() => t.classList.remove('show'), 3000);
}

// ============================================================
// INIT
// ============================================================
connectWS();
</script>
</body>
</html>
)rawliteral";

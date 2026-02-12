// ============================================================
// Auto-generated web interface - PROGMEM
// Generated from: index.html
// Size: 102804 bytes
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

  /* MAIN */
  main {
    max-width: 600px;
    margin: 0 auto;
    padding: 16px;
  }

  /* CARD */
  .card {
    background: var(--surface);
    border-radius: var(--radius);
    margin-bottom: 16px;
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
    padding: 16px;
  }

  /* INPUTS GRID */
  .inputs-grid {
    display: grid;
    grid-template-columns: repeat(4, 1fr);
    gap: 10px;
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
  .input-relay {
    display: flex;
    align-items: center;
    gap: 6px;
    font-size: 13px;
    cursor: pointer;
  }
  .input-relay input[type="checkbox"] {
    width: 18px;
    height: 18px;
    cursor: pointer;
  }

  /* BRIGHTNESS ROW */
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
    min-width: 100px;
    text-align: right;
    font-size: 13px;
    color: var(--subtext);
  }

  /* TOGGLE */
  .toggle-rows {
    margin-top: 12px;
  }
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
  .toggle-cb {
    width: 44px;
    height: 24px;
    -webkit-appearance: none;
    appearance: none;
    background: var(--surface2);
    border-radius: 12px;
    position: relative;
    cursor: pointer;
    transition: background 0.2s;
  }
  .toggle-cb:checked {
    background: var(--green);
  }
  .toggle-cb::before {
    content: '';
    position: absolute;
    width: 18px;
    height: 18px;
    border-radius: 50%;
    background: white;
    top: 3px;
    left: 3px;
    transition: left 0.2s;
  }
  .toggle-cb:checked::before {
    left: 23px;
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
    gap: 8px;
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
    background: var(--surface2);
    border: 1px solid var(--border);
    border-radius: 6px;
    padding: 6px 8px;
    font-size: 12px;
    font-family: monospace;
    color: var(--text);
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
    padding: 8px 14px;
    border-radius: 8px;
    border: none;
    font-size: 13px;
    font-weight: 500;
    cursor: pointer;
    transition: opacity 0.2s;
  }
  .btn:hover {
    opacity: 0.85;
  }
  .btn-green {
    background: var(--green);
    color: white;
  }
  .btn-gray {
    background: var(--surface2);
    color: var(--text);
  }
  .btn-red {
    background: var(--red);
    color: white;
  }

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
    <div class="card-header">Input Selection</div>
    <div class="card-body">
      <div class="inputs-grid">
        <div class="input-tile" id="tile-0" onclick="selectInput(0)">
          <img src="data:image/png;base64,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" alt="Input1">
          <span>Input1</span>
        </div>
        <div class="input-tile" id="tile-1" onclick="selectInput(1)">
          <img src="data:image/png;base64,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" alt="Input2">
          <span>Input2</span>
        </div>
        <div class="input-tile" id="tile-2" onclick="selectInput(2)">
          <img src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAAAXNSR0IB2cksfwAAAARnQU1BAACxjwv8YQUAAAAgY0hSTQAAeiYAAICEAAD6AAAAgOgAAHUwAADqYAAAOpgAABdwnLpRPAAAAAZiS0dEAAAAAAAA+UO7fwAAAAlwSFlzAAALEwAACxMBAJqcGAAAAAd0SU1FB+oCCg4MKhPmipAAABbCSURBVHja7ZtpsBzXdd9/597unnk7gAcQCxcQNBfBkAiKFAGKEklAm2FJrNCSSpFoxUm5XFoSR5H8JZVUolTlY1KKEpXsKi8f7FJMWzYTS7QlWJZIiqRAUiQAggAJEHwgdjwsD2/B22amu+89+XC7Z3rmPUCsykdxXr3q6Z7bfe8596z/c1oA5Vf4Y/gV/7zDgHcY8Cv+icTUAClOK/ZQq5cVESkuKdr+oRijUjkpvwqoD79J+TDtvq34Jkss8dIr4RkatixSqAF1wCu6KJABPqwVLYaXdOgvkwCtjFItzjvEgxZkayCseLK0mVM5bz/Hh3Mpr3mkILZDesnQ6lzLbATlmsL9NjE8+vB7+cNvbOLBD1jMsILtvq99W7kZSPcGlRLQdaZUCCwWJBVNkYLg9lTaHi4ibSK7N06Lx/bseXsuRVS75i3Ha3v+MFYEvv7VL/KB99/L6OpVrBvN2Pqub/GhI8/Tml1me6UiCaLdUl1MFHWLh3aLi3SuS5vSjth2nqhIkI/i47sXsVSnAkH49gAtZVU6o4wJVyOj/KtHH+ChnR9jUUc5fKbB1OH9fOaelDRtoLmCl0DjsuKuy2iWFhIgPXoi0l6r9AhF2JWCAVKSXZCn2tbmNqkiV1lLZZyUoh0U17T/YdeD1/P5f/FxNt1yK+cnI/7pxcscO32M1Ss99ewAe1oH+KMfQD5tkLzD4HKzl9V96V6DiKl1VtwWOemIqlQeLMs8jKVqu+yQrrEd1TEFo63xxFbZfFPMFx7dyfu2343WVrLvaJNnDkxwcfwSr77wHHNXLuMjFwyhBc0VaQG5dARYK3bzagIhpSBWGdC78IrYd9lqCWcVYQn62vt0ulQYUIyE3TYSRDuxykjiefSzN/Kxj+9k5ar1vHlhFU8fmObs+EUOvrSX40fGuryBFxeoMyA+iD++mLdchy7H+aWMEDF1pWpslhFfkc556Ql6TELhLHQJ4RTeQwQiA7FATTy12PP+uwf4/O98mM2/fgvTi33sGxvi0LFpjh4+wrM/fgr1vqMuWtoWQcV3623phpcQ36P42mWUKhLQXmjp66Wz61LqabABIsEgSsUDUJUADbsLnWMsYaf7Y9iwAj79mXfz4Y9vpT54B2+ernHg8AXOnJ/g6af2cPL4eAgftNcbVty0aMdolrKp2va4elUdrTCq3FBTMkC0CFqk8+A2oYoxgXgjYIthpWfw+CCGoljAiBKJEhklNp7hWNn1wCo+8ehHuOnm25mYrXPsxDRjp6bYt/8NfvrkftSD9x0jq0t2syC5zRldPmBaIgRyzUhQxNS048CkYgSDviIeawLR1ihRKco2jPIKOSAqgXA8Nav0x8qWjRG/9eh27tz6LpxZw9TUIidOnOS1Y+d5/ImDXJ72qAqu8Jq+CHZKGnstua9IWZdUcK1oT9oxy3LMEDGJClW30LH45Y5HRokMJFapWxhJlOFaGNPMYNEFRtQMjA4qjzxyAzs+eh+rVq1nYjbh5NhRJi4f5+d7z/DjZ5tkeYdYr9oOPstrvQFp13dKxndz6OruX67KIwkqkGjV8ZcCYIr/2ARx7othIIbVdc+NQ7BrB2x5v2VuIeLP/jhlaO2t/NYXPsl7fn0V2jpPY36GUxfG+cW+Izz+xGXmFiB1Qu4Lotv6qnhfincZygcV8Bo03Gsp+tIZV7m/S+yrQtKVp1yTAd0aIKJYA5ERIuPps8pQolzXp9w2Al/9Oty5rY7LEq6MG/ZN389E9iBR4wI3D7/A5PwZdv/DBG8cdSxk0CoIzx04LTML8BqkzntwhSSEfCYwyfsgWb7CGN8+dqSn5EHJjG7h6I7oOu66lHSJtTR25VhjwIqSWKhFylDsua5P2XGX8Pv/xrB2wyhnTo5w9NV5Ji7njNy8mWcPnWKDnOPEWE6zBXkOrRxSB86HhTtX/Cs4gbzQf+eV3CuZB6dC7sNvuScwrWQEglMJ96vBe20zwdMxniUDSoPaDme1x24ECYjbjtsU46yBxCj1SBmMlTX9yo6t8B+/LvQPr2XvkY30XX8/JvJgcuK4jkoNvEfzJmiGYgrGe9R7VA1KjNccVQ/iUQxeY5QEjy9imRivMXmueG/xashd8ItiElQ9zoGYOoot1EZQD61WkwMHj/HNb/9vcucr0iHdqkGXG4zbGaOgWCNERqlbz3CirBkQ/vmnhE/d59kwCsdOr2F4eA2ra2NonqE5QVRzwTvBZWH3nQrqBZcpWSq0ckMzi1hoWZqZoZXBQsvQzCyLmWEhFVrOkHlDrpZGLjSdpemElhOcibA2IVMhdaAYlKBaToU8V0wUsW3bXYwdP8f//f5PCpXQnpiiOC8kQoyJtXT9RgIDkkgZjh3rB+Fb/9Vw79YvYFZ9BK2PFIZJO+mMCqoedR48qLdBNDGoB+ccuYPclyphybwlc9DKPa3ck+ZK5oVMDd4LuVdyJRxzX8xWqounkj4F6SroM1aIrcU5z4WLl/jRE3/HvueeDnqvii4DvkSBI9LGDKQIdAZj+PJvW7bd/QfIpt9l9tJRpscOkM1dQLNFNMtAHXhPnnnSFLIWNBY9jZaS5oZmJixm0PCGhjPMuYg5H7HgIhZdTMtbWs7jcod6j/ce1Acxx+AA7zUYTg1GMy9shnOK90GN8KWaSdv49ffVuesDu5ieXeDYgZepmMkePKCClZSghTVKLYJ7tyjmxk9ycuxlXnrxZ5w5P8nc7AKtuXlajbSwuJ48U7IcmpnSTKHhDGAKC66kWFKJaZoaqUTkasgI+i0i3LFuiJEkJ6k5arEjsg5jPTZSxCjOB5FHI9KsxnwrYbGlNFNPlhcGNhcWU8fYWyfwqhgbMTe/wLYdv8GxA/uhnT9UfKQoUQeUChZUDJgwHXb4Vlr5Zfa88hrPH15g/FKT1vwsX3xogpcOKZs2ZMzMCjaGtcMZB45bdt7l+M4P6nxpV4Nv/30/f/DZRf72yRofuheeOTjC7pMDOBV8EQRtves++twZarSoR0oc5SSxI0oybJQDDuc93gnexSB9xL6PGlGwW+rxLqiLZMqaDRs59fp+xFqmkzprRldUt7gNupQuMirzfumKmAxOHS2tMz91lFfenOTwmQXmrixiFuYwfpq8pcTeIRk4p7RiT0KMTzMS1yDRJlHWwLgWkvcxGMEgOYtzDic2zO89F6dS9uzZTd9OuPXdhtHhiL44xhghM44FUha9o9lUFqY8l89a5qfqcCWGWUUbQAYylcFCxk3bH8ZnLcRZxKXgssLgmQpSJd02oI3NVULlzAtpnjI3eYqxMzNcmFwgW2xSazVZM7DISGIZTRwkSkMNaxPHRGJZmUCsGRsHHZF3rEwd/eIYssporUXeauHFhgBNPYgFIyzYlEPHc6JhJR4EM6zc+i7LYB3mZz0r+qA2D2cXFfJ+zPq1mLSfrDEAtg/dcwhZnMZaC94F4jSlTxsFAyoBkPq2G4y60+YwKBgexbsWjbkpLk3NMz/fQFstfJrx8hsRr512eKdMzkEUCTNX4Mi5lKG65dKc5/k3DBcXlFdPRhyf8Bx6yzB2gcJw0UaZrTWoA72ouCvghpV0GMxqaN0Bp484br/DogacONasE8brq3E/Ow0PfgCRm9Czi3D7u+DCCxgJhlkEaq7BqnwiKLlIERZrB8khJHcdv2A6MbNTcGlGOjfLwnyTVjNF8gzrPB+6s8XCtPDB93gmpgTnlXVDDlHhw5tzXnpd2XWP4/u/ED76nozDJ2tsvy3FNYS/fVOD9yiZIMGVMa/QBFJBm8Hyp7PK9aOWxRkYGRXoNyxcDpqrd28Gl6JEyLo16JsnUKBuPILHIPTTZIW7VGCZvmuTyzwhqsJVqp0kxHlwCy3y5ixZw5GlKbgc65U//1HCwROOiSuWyblgRkf7hZNTgqpw5ILnu08nnLoi/MVTEfvOWsxzloNnohALt7MuRfxCwfEEboqhL8MkOZJ4Tj6rDPQb0jTkJnnT0jpvYPYt0Bpu0sCVA2imSCsQOGgyjDgigUFaDPopTAGgaBVDUO2ogFaKGqV4OgVaTYxZwDpQFxiAz/n9R5p8d7ew66OGs+PQmvX82nrYd9zysXs8r59Vvvw5z+69hn/7WeHyXyhf+rTjyRci9vygzATDAlYlk3ztkbUMJ4odSDAR2CjD1h1REjyMMYoo5C6mtTqi1ewn0zVkWYRPm2iribYaDLgGb82M86o4ElEGTcqQn8SI4nvKHaVdiDrxUTVWLrKxVpNaNE+fJu1gI/Pwhz9KeOHVjMvzysUZRbyyoi6cn1NOjVsOnsr4zmMJl1LPt/7G8vRYxL5vDTG5fhdm5A3c9JlglAWe//mLxBuFdQOO/lqTPpvTX8sZ7MtR41Cj1GMfGJAKswsxM4sJi/klGqnQaOakqUNzz2Ij59WJc8QCsRgGRBjSSax6XDV9XlIZagOqwUD4Yoz1Ss2nDCAYDRmbKPzOb+bUEX7zPsPx8RIIgdONmAfvMlycha/8dsJPDqR89dMx564Yvn8qIf/Ev8f/5HvoC3+M1pTr/906Jk9N8szf5EQCsfUBShMhNpaaKDXxxCYkVpkaGjksZjmNfAFXQGhlqGsUHrkj5xufb/KNv+rDNpTEzxLhyboKN9oOe6NqgiDSQRu8B809SZQxFMVEErA/j/LU3oixcUffq3BxWmmlhtE+GM8MqfMcHofHfgqXFpXHnss5OhHTaC3iH/8mnDwUoOwRx0PbHuLFlS9z4q+Pk6M4r9x2802sHBni1dcOI14w+AJnLENhIStS61/bdAMrV65g775DbQufNoX5WUOjJYziML4ZAjsx7eSoijFGy6FISqjO+NyT2JxVfUI9EZwTEq/cf0fO+bPCu29RVl6G4UQZiA3nbI333p5y8y+Uh+7O+e7fw867DT9/Xdg759HTP0TSJCT35+CxrzyGzBtwnu33vJuvff1rbLp5I1mWMTk9wz/u/kf+5E//DBHTVlOvsP19W/nKv/4SmzZtwnmYmZnhmaee4X/+r+/wwzMxT347YoWFj63NsD4lxtDEIxX0s4yGzVLiaWdfsUJfv+e6YaFeixAbI9ZwftqSesvEbMT8ouHcpGFqHibONbg47mhkwsT5GFuLmETIaopaBclQ8jC5Ezhr4EqoFXzmc59jy51bWb3+Rk5Pt3h57AJbtj/Iro9/ktwZMidkOcRxxL/8vS+yZevdDI6u45WxcQ6emuLuD36Ihx/5Z+TekHoLCIPisOqISoqkY+zblalOcluJBwo1MEBtxLB+TcRAf0yURETWMBKBYukzSj0ShuoGEei3SpQpViHyOViLdw6NFdZ5SHwVvwopqld27HyQuG+IRg4/e3E/33v8hzz53EucnVrkvffvQL0UsDncu30b885A0sdLB17j757YzY+f3MPJywtse2BnkcUYYoQhEwi1qlcBx6WqAtJVVdBCQZI1NW66ocaqMzGXFxR1lnx4hIGBRWrDYFIYWVUnJmd6uk79un5WDF+mPjzIydkmD/+PmHzLInJnhr4hcPI69KRWStfK5JUFntt/hIMnLnHk9Tc4efIUq9ZvYN26dUxfONcFkc8s5jy39zVOXpxh/979HHtjjBVr1jMwMEDmG+14rt/AyiS40Eg7dk61U8wtjKB0YewacpRQsIzBrBxm3bo6K4cjZBLSVj9vnZ3j3FTChsuWs5c8s40E5/uYWYwY6RNaWT9vnTUs+Bq5y6EOdrOSX1S40Cm+aNEbcGF8HD98jvzEReZmZhhaOcoD92/jzts28dKl0wWkFbhw+sQJ8vp1HD1+nrnpKUZG17Jt+/u4fdP1jB26iBFDbJSRWFlVCzUNytIhPaCo0isBgTNlrVEA6R9mdLTG6mFDnCizdpj/dMN3SBcOs3toBb7P0EgSkqSfxswVGFyHG3iWx0d+A2e+DGYeTsfkTytcGYVajHCRavxx7sQprt+0mf5VG7hl441sueMWPvLBe5HWPD/43mNdRf+p8xfZtLnJiuuu545bb2HzrRv5yAfvYSCG//a9v8KKZShyrE8ca+u+cJ8l0rwUHo96mjPaaGrmIZtSyB1DccbaujAc5SyIZ+6b/wWX5zRcyMVxOU11gEXpA5PDC/8EkQ+NPFMDMNYHosjr56m2WCACJmHvsz/lfQ/sYNWNq1m/oo/XXtnL44/9JXv27KtopoBYXnnmp+z4RI2bbruPG9eNcuzI6/yfv/5Lnn/2ZVYksDFJ2dzf4oYRxamwqKbTsiFSdK20+wMSlS6eBBfYZz3/eRt87T+sYe5cP//wYsITRyNOTSmzDU+aBiwvzx3OebyGyVwBpyiKqiuOvihedkD73jnDb76DTBkbMlLvl1R4pKg/hmhWsNYSCQxZZWMt5d7BeXZel/HAFuWJCcNXdsdkKsUsnYqyIoEB1QpauSAryvpE+e+fEx7ebpg+YTlxrsbl2ZiFlpDlSpYLWRbAzTyHNLekuQS8X4Xce3IXavieUD8tky2tpN6ZK5kk5AX+bzAgIS1vS6aYduRXFs6tsYh6YhwjkXJjX4s7VzZ5z/XKaREe/ZHlbG7wJSKk3QX8CgN6tcNjgESUT9ykfOpeWN0PeSuALN6DyxWfheJFlkOWgU/LQobgQmre8SwmENeu9flQ/cmLcbkPQIxTsCIYE2qTZcuLFliG19BdhEioVquSiFKzMBApNlJemRP+/JTQKhhK73+BArerw9IuixXwuARsEFV8Wb8ra3qFKLXreEt7j5btR5GrdwEuV+Fedqxes+gtyzxHuroCA/BrQIJ7iIyYdlNSZCAxEFulHkF/BP2x0h97hmswmEAtEiITxLCVCwuZ5UqaMJtaGs7gPUTiiMWRiCcxSmw9ifEkxhFVegzKBorSCnuhqBaF4kf4L+J/L+Qa4bxFMcWfFHlLUUcspCj1liaWhhdSryF3UI8vIPc2NqgFItTbRtjd6yftGn7uwaoJLsVDqjbA2xKj1iAYrIUITyx5QbQnsS4cxWBNgN1NGYdXglAvUjAA1EhIma1gvEFUwEWoN3g1RUFGihZM8CiOYIi9MeCDekg74FqG0jIbLMXYeW3X0pxC7oVmrkRGmG4KcQTWFGKkkKsl9ULLKS3vcUV7jEWJKKrLEvTZYjAibWlb2vwS9LroKMIVGWkJcIVMUDs76TucaxtGDVCebxdQtbinmgRql8ZE7eCnkgz5opSVOaXR7g4BY0quBr126ovaXFn2lkLTgiO0UlTwyqrzsq12QnejrXTK2+V61Lerw167EK0uJlRahSvepvt6VzutSkCEqKAlgfthl3IE8VoYRsGYTpcYWtb4CyOp3U0pUj1K57ynSauC0FZAmR758Lq0Aaxd5S1bakqcuaurpNto97aRSleTVM/uSLUfsN0hRseFaE8KrUuC6q7vUjzrl/QsVXyGLpmDagtNBcNbKt2dalf19+X8SREHyDL9vNWm0WpfYI+B7LRNs2y/YU+zZNcMWuD113Sh9DQ5dTe2soxqL/84XfaqiCR6rV2RnncA5JqTLG3Gkqv26i717D1vIlxDSvQabXFX6YnVngJQ0S4jYmIN/YGVLqNrNAX/MgbItYIVkatul3ZV/d/eR6+1El2uO1yWvFMgIrF2X5elTNDlCFF4m82Ib2f5b2+sLHOub3MavUqjpMR6jTbKtx+jsswaqzsuyxCgS99LWPb1nbdjNXtf+lDtkq6rbVHURUzvUXsXRxea0jV+WUboVfRdu8pjy+/u/8enp9FYrsGzaMmVLqKu0XAvy3Rl69WMgP5yhi03Rrv62q8i/drVDNlrJq/SLt3+Lrzz6uw7L06+w4B3GPAr/Pl/VTeDn0lcg7kAAAAASUVORK5CYII=" alt="Input3">
          <span>Input3</span>
        </div>
        <div class="input-tile" id="tile-3" onclick="selectInput(3)">
          <img src="data:image/png;base64,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" alt="Input4">
          <span>Input4</span>
        </div>
      </div>

      <!-- Relay checkboxes + Save button -->
      <div style="margin-top:16px; padding-top:16px; border-top:1px solid var(--border);">
        <div style="display:grid; grid-template-columns:repeat(4,1fr); gap:12px; margin-bottom:12px;">
          <label class="toggle-row" style="flex-direction:column; gap:6px; padding:8px 0; border:none;">
            <span class="toggle-label" style="font-size:12px;">Relay 1</span>
            <div class="toggle-switch">
              <input type="checkbox" id="relay-0">
              <span class="toggle-slider" onclick="toggleRelay(0)"></span>
            </div>
          </label>
          <label class="toggle-row" style="flex-direction:column; gap:6px; padding:8px 0; border:none;">
            <span class="toggle-label" style="font-size:12px;">Relay 2</span>
            <div class="toggle-switch">
              <input type="checkbox" id="relay-1">
              <span class="toggle-slider" onclick="toggleRelay(1)"></span>
            </div>
          </label>
          <label class="toggle-row" style="flex-direction:column; gap:6px; padding:8px 0; border:none;">
            <span class="toggle-label" style="font-size:12px;">Relay 3</span>
            <div class="toggle-switch">
              <input type="checkbox" id="relay-2">
              <span class="toggle-slider" onclick="toggleRelay(2)"></span>
            </div>
          </label>
          <label class="toggle-row" style="flex-direction:column; gap:6px; padding:8px 0; border:none;">
            <span class="toggle-label" style="font-size:12px;">Relay 4</span>
            <div class="toggle-switch">
              <input type="checkbox" id="relay-3">
              <span class="toggle-slider" onclick="toggleRelay(3)"></span>
            </div>
          </label>
        </div>
      </div>
  </div>

  <!-- AUDIO -->
  <div class="card">
    <div class="card-header">Audio</div>
    <div class="card-body">
      <!-- Volume -->
      <div style="margin-bottom:16px;">
        <span class="brightness-val" id="volume-val" style="display:block;margin-bottom:6px;">VOLUME: 100%</span>
        <div class="brightness-row">
          <span class="brightness-icon">🔇</span>
          <input type="range" id="audio-volume" min="0" max="63" value="0" style="direction:rtl"
                 oninput="onVolumeChange(this.value)">
          <span class="brightness-icon">🔊</span>
        </div>
      </div>
      
      <!-- Bass -->
      <div style="margin-bottom:16px;">
        <span class="brightness-val" id="bass-val" style="display:block;margin-bottom:6px;">BASS: 0</span>
        <div class="brightness-row">
          <span class="brightness-icon" style="font-size:16px;">−</span>
          <input type="range" id="audio-bass" min="-14" max="14" value="0"
                 oninput="onBassChange(this.value)">
          <span class="brightness-icon" style="font-size:16px;">+</span>
        </div>
      </div>
      
      <!-- Treble -->
      <div style="margin-bottom:0;">
        <span class="brightness-val" id="treble-val" style="display:block;margin-bottom:6px;">TREBLE: 0</span>
        <div class="brightness-row">
          <span class="brightness-icon" style="font-size:16px;">−</span>
          <input type="range" id="audio-treble" min="-14" max="14" value="0"
                 oninput="onTrebleChange(this.value)">
          <span class="brightness-icon" style="font-size:16px;">+</span>
        </div>
      </div>
    </div>
  </div>

  <!-- DISPLAY -->
  <div class="card">
    <div class="card-header">Display</div>
    <div class="card-body">
      <div class="brightness-row">
        <span class="brightness-icon">🔅</span>
        <input type="range" id="brightness" min="10" max="255" value="200"
               oninput="onBrightnessChange(this.value)">
        <span class="brightness-icon">🔆</span>
        <span class="brightness-val" id="brightness-val">78%</span>
      </div>
      <div class="toggle-rows">
        <label class="toggle-row">
          <span class="toggle-label">Invert colors</span>
          <div class="toggle-switch">
            <input type="checkbox" id="disp-invert" onchange="onDispInvertChange(this.checked)">
            <span class="toggle-slider"></span>
          </div>
        </label>
        <label class="toggle-row">
          <span class="toggle-label">Rotate 180°</span>
          <div class="toggle-switch">
            <input type="checkbox" id="disp-rotate" onchange="onDispRotateChange(this.checked)">
            <span class="toggle-slider"></span>
          </div>
        </label>
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

  <!-- WIFI -->
  <div class="card">
    <div class="card-header">WiFi</div>
    <div class="card-body">
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
  </div>

  <!-- MQTT -->
  <div class="card">
    <div class="card-header">MQTT</div>
    <div class="card-body">
      <label class="toggle-row">
        <span class="toggle-label">Enable MQTT</span>
        <div class="toggle-switch">
          <input type="checkbox" id="mqtt-enable" onchange="onMqttEnableChange(this.checked)">
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

  <!-- FIRMWARE UPDATE -->
  <div class="card">
    <div class="card-header">Firmware Update</div>
    <div class="card-body">
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
      // IP address
      if (msg.ip) document.getElementById('ip-addr').textContent = msg.ip;
      
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
      if (msg.mqtt_pass_set) {
        document.getElementById('mqtt-pass').placeholder = '••••••••';
      }
      if (msg.mqtt_topic) document.getElementById('mqtt-topic').value = msg.mqtt_topic;
      if (msg.mqtt_enabled !== undefined) {
        const mqttEnable = document.getElementById('mqtt-enable');
        if (mqttEnable) {
          mqttEnable.checked = msg.mqtt_enabled;
          setMqttFieldsEnabled(msg.mqtt_enabled);
        }
      }
      setMqttStatus(msg.mqtt_connected === true);
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
        // Stop countdown
        if (irCountdowns[msg.key]) {
          clearInterval(irCountdowns[msg.key]);
          delete irCountdowns[msg.key];
        }
        const cdEl = document.getElementById('cd-' + msg.key);
        if (cdEl) cdEl.textContent = '';
        
        // Ustaw kod
        const codeEl = document.getElementById('ir-' + msg.key);
        if (codeEl) {
          codeEl.value = msg.code || 'not set';
        }
        
        showToast('IR code learned!', 'green');
      }
      break;
      
    case 'ir_timeout':
      if (msg.key) {
        // Stop countdown
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
      
    case 'wifi_test_result':
      showWifiStatus(msg.success, msg.message);
      break;
      
    case 'relays_saved':
      showToast('Relays saved!', 'green');
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
    if (cb) cb.checked = !!(masks[i]);
  }
}

// onRelayChange() - wyeliminowana, teraz saveRelays() jest wywoływany bezpośrednio na onChange

function toggleRelay(index) {
  const cb = document.getElementById('relay-' + index);
  if (cb) {
    cb.checked = !cb.checked;
    console.log(`Relay ${index} toggled to: ${cb.checked}`);
    saveRelays();
  }
}

function saveRelays() {
  console.log("DEBUG: saveRelays() called");
  const masks = [0, 0, 0, 0];
  for (let i = 0; i < 4; i++) {
    const cb = document.getElementById('relay-' + i);
    if (cb) {
      console.log(`  relay-${i}: checked=${cb.checked}`);
      if (cb.checked) masks[i] = 1;
    }
  }
  console.log("DEBUG: Sending relay masks:", masks);
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
// DISPLAY
// ============================================================
function onDispInvertChange(checked) {
  send({type: 'set_invert', value: checked});
}

function onDispRotateChange(checked) {
  send({type: 'set_rotate', value: checked});
}

// ============================================================
// AUDIO
// ============================================================
function onVolumeChange(val) {
  updateVolumeDisplay(val);
  send({type: 'set_volume', value: parseInt(val)});
}

function updateVolumeDisplay(val) {
  const level = 63 - val;
  const percent = Math.round(level * 100 / 63);
  document.getElementById('volume-val').textContent = 'VOLUME: ' + percent + '%';
}

function onBassChange(val) {
  updateBassDisplay(val);
  send({type: 'set_bass', value: parseInt(val)});
}

function updateBassDisplay(val) {
  document.getElementById('bass-val').textContent = 'BASS: ' + (val > 0 ? '+' : '') + val;
}

function onTrebleChange(val) {
  updateTrebleDisplay(val);
  send({type: 'set_treble', value: parseInt(val)});
}

function updateTrebleDisplay(val) {
  document.getElementById('treble-val').textContent = 'TREBLE: ' + (val > 0 ? '+' : '') + val;
}

// ============================================================
// IR REMOTE
// ============================================================
let irCountdowns = {};

function learnIR(key) {
  send({type: 'learn_ir', key: key});
  
  // Countdown
  let countdown = 10;
  const cdEl = document.getElementById('cd-' + key);
  if (!cdEl) return;
  
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
  if (!ssid) {
    showWifiStatus(false, 'Enter SSID');
    return;
  }
  send({type: 'test_wifi', ssid: ssid, pass: pass});
  showWifiStatus(null, 'Testing...');
}

function saveWifi() {
  const ssid = document.getElementById('wifi-ssid').value;
  const pass = document.getElementById('wifi-pass').value;
  if (!ssid) {
    showWifiStatus(false, 'Enter SSID');
    return;
  }
  if (confirm('Save WiFi and reboot?')) {
    send({type: 'save_wifi', ssid: ssid, pass: pass});
    showWifiStatus(null, 'Rebooting...');
  }
}

function showWifiStatus(success, message) {
  const el = document.getElementById('wifi-status');
  if (!el) return;
  el.textContent = message;
  if (success === true) {
    el.style.background = 'rgba(45, 197, 94, 0.2)';
    el.style.color = 'var(--green)';
  } else if (success === false) {
    el.style.background = 'rgba(224, 40, 40, 0.2)';
    el.style.color = 'var(--red)';
  } else {
    el.style.background = 'rgba(136, 136, 136, 0.2)';
    el.style.color = 'var(--subtext)';
  }
}

// ============================================================
// MQTT
// ============================================================
function onMqttEnableChange(checked) {
  setMqttFieldsEnabled(checked);
  send({type: 'set_mqtt_enabled', enabled: checked});
  setMqttStatus(false, false);
}

function setMqttFieldsEnabled(enabled) {
  const ids = ['mqtt-host','mqtt-port','mqtt-user','mqtt-pass','mqtt-topic'];
  ids.forEach(id => {
    const el = document.getElementById(id);
    if (el) el.disabled = !enabled;
  });
}

function saveMqtt() {
  const host = document.getElementById('mqtt-host').value;
  const port = document.getElementById('mqtt-port').value;
  const user = document.getElementById('mqtt-user').value;
  const pass = document.getElementById('mqtt-pass').value;
  const topic = document.getElementById('mqtt-topic').value;
  send({
    type: 'save_mqtt',
    host: host,
    port: parseInt(port) || 1883,
    user: user,
    pass: pass,
    topic: topic
  });
}

function setMqttStatus(connected, connecting) {
  const el = document.getElementById('mqtt-status');
  const enableCb = document.getElementById('mqtt-enable');
  
  if (el) {
    // Sprawdź czy MQTT jest enabled
    const enabled = enableCb ? enableCb.checked : false;
    
    if (!enabled) {
      el.textContent = 'Disabled';
      el.style.color = 'var(--subtext)';
    } else if (connecting) {
      el.textContent = 'Connecting...';
      el.style.color = 'var(--subtext)';
    } else if (connected) {
      el.textContent = 'Connected';
      el.style.color = 'var(--green)';
    } else {
      el.textContent = 'Disconnected';
      el.style.color = 'var(--red)';
    }
  }
}

// ============================================================
// OTA UPDATE
// ============================================================
function setupOTA() {
  const dropZone = document.getElementById('ota-drop-zone');
  const fileInput = document.getElementById('ota-file');

  if (!dropZone || !fileInput) return;

  // Click to select
  dropZone.onclick = () => fileInput.click();

  // File selected
  fileInput.onchange = (e) => {
    if (e.target.files.length > 0) {
      otaUpload(e.target.files[0]);
    }
  };

  // Drag & drop
  dropZone.ondragover = (e) => {
    e.preventDefault();
    dropZone.style.borderColor = 'var(--green)';
  };

  dropZone.ondragleave = () => {
    dropZone.style.borderColor = 'var(--border)';
  };

  dropZone.ondrop = (e) => {
    e.preventDefault();
    dropZone.style.borderColor = 'var(--border)';
    if (e.dataTransfer.files.length > 0) {
      otaUpload(e.dataTransfer.files[0]);
    }
  };
}

function otaUpload(file) {
  if (!file.name.endsWith('.bin')) {
    showToast('Select a .bin file!', 'red');
    return;
  }
  
  setOtaProgress(0, 'Uploading...');
  showToast('Uploading firmware...', '');

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
        setTimeout(() => location.reload(), 3000);
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

  xhr.send(file);
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
// TOAST
// ============================================================
function showToast(message, color) {
  const toast = document.createElement('div');
  toast.className = 'toast';
  toast.textContent = message;
  if (color === 'green') toast.style.borderColor = 'var(--green)';
  if (color === 'red') toast.style.borderColor = 'var(--red)';
  document.body.appendChild(toast);
  setTimeout(() => toast.remove(), 3000);
}

// ============================================================
// INIT
// ============================================================
connect();
</script>
</body>
</html>

)rawliteral";

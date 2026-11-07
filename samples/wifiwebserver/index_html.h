#pragma once
#include <pgmspace.h>

// Control page served on "/". Stored in flash (PROGMEM).
static const char INDEX_HTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP Control</title>
<style>
  body { font-family: system-ui, sans-serif; background: #111; color: #eee;
         display: flex; flex-direction: column; align-items: center; padding: 2rem 1rem; margin: 0; }
  h1 { font-size: 1.4rem; font-weight: 600; }
  .led { width: 80px; height: 80px; border-radius: 50%; margin: 1.5rem; background: #333;
         border: 3px solid #555; transition: background .2s, box-shadow .2s; }
  .led.on { background: #4af; box-shadow: 0 0 30px #4af; border-color: #8cf; }
  #state { font-size: 1.1rem; margin-bottom: 1.5rem; }
  .buttons { display: flex; gap: .75rem; }
  button { font-size: 1rem; padding: .7rem 1.4rem; border: 0; border-radius: 8px;
           background: #2a2a2a; color: #eee; cursor: pointer; }
  button:active { background: #444; }
  #err { color: #f66; margin-top: 1rem; min-height: 1.2em; }
</style>
</head>
<body>
  <h1>Built-in LED</h1>
  <div id="led" class="led"></div>
  <div id="state">...</div>
  <div class="buttons">
    <button onclick="setLed('on')">On</button>
    <button onclick="setLed('off')">Off</button>
    <button onclick="setLed('toggle')">Toggle</button>
  </div>
  <div id="err"></div>
<script>
  function show(d) {
    const on = d.led === 'on';
    document.getElementById('led').classList.toggle('on', on);
    document.getElementById('state').textContent = on ? 'ON' : 'OFF';
    document.getElementById('err').textContent = '';
  }
  function fail() { document.getElementById('err').textContent = 'Board not reachable'; }
  function refresh() { fetch('/api/led').then(r => r.json()).then(show).catch(fail); }
  function setLed(s) {
    fetch('/api/led?state=' + s, { method: 'POST' }).then(r => r.json()).then(show).catch(fail);
  }
  refresh();
  setInterval(refresh, 2000);
</script>
</body>
</html>
)rawliteral";

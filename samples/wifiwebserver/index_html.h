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
  h1 { font-size: 1.2rem; font-weight: 600; margin: 0 0 1rem; color: #aaa;
       text-transform: uppercase; letter-spacing: .08em; }
  .row { display: flex; flex-wrap: wrap; justify-content: center; gap: 1.25rem;
         width: 100%; max-width: 1140px; margin-bottom: 1.25rem; }
  section { flex: 1 1 300px; max-width: 360px; box-sizing: border-box; background: #1b1b1b; border-radius: 12px;
            padding: 1.5rem 1rem; display: flex; flex-direction: column; align-items: center; }
  .tiles { display: flex; gap: 1rem; width: 100%; }
  .tile { flex: 1; background: #232323; border-radius: 10px; padding: 1rem .5rem; text-align: center; }
  .label { font-size: .8rem; color: #999; text-transform: uppercase; letter-spacing: .06em; }
  .value { font-size: 2.2rem; font-weight: 600; margin-top: .3rem; font-variant-numeric: tabular-nums; }
  .unit { font-size: 1rem; color: #999; margin-left: .15rem; }
  .temp .value { color: #fb7; }
  .hum .value { color: #7cf; }
  .meta { font-size: .8rem; color: #777; margin-top: .9rem; min-height: 1em; }
  .meta.bad { color: #f66; }
  .ind { width: 80px; height: 80px; border-radius: 50%; margin: .5rem 0 1rem; background: #333;
         border: 3px solid #555; transition: background .2s, box-shadow .2s; }
  .ind.led.on   { background: #4af; box-shadow: 0 0 30px #4af; border-color: #8cf; }
  .ind.relay    { border-radius: 14px; }
  .ind.relay.on { background: #e94; box-shadow: 0 0 30px #e94; border-color: #fc8; }
  .state { font-size: 1.1rem; margin-bottom: 1.25rem; }
  .buttons { display: flex; gap: .75rem; }
  button { font-size: 1rem; padding: .7rem 1.4rem; border: 0; border-radius: 8px;
           background: #2a2a2a; color: #eee; cursor: pointer; }
  button:active { background: #444; }
  details { width: 100%; max-width: 360px; box-sizing: border-box; background: #1b1b1b; border-radius: 12px;
            padding: 1rem; margin-bottom: 1.25rem; font-size: .9rem; }
  summary { cursor: pointer; color: #aaa; text-transform: uppercase; letter-spacing: .08em; font-weight: 600; }
  table { width: 100%; border-collapse: collapse; margin-top: .75rem; }
  th { text-align: left; color: #999; font-weight: 500; padding: .5rem .25rem .25rem; font-size: .8rem;
       text-transform: uppercase; letter-spacing: .05em; }
  td { padding: .3rem .25rem; border-top: 1px solid #2a2a2a; }
  td:last-child { font-family: ui-monospace, monospace; color: #fda; }
  .note { color: #888; font-size: .8rem; margin: .4rem 0 0; }
  #err { color: #f66; min-height: 1.2em; }
</style>
</head>
<body>
 <div class="row">
  <section>
    <h1>Climate</h1>
    <div class="tiles">
      <div class="tile temp">
        <div class="label">Temperature</div>
        <div class="value"><span id="t">--</span><span class="unit">&deg;C</span></div>
      </div>
      <div class="tile hum">
        <div class="label">Humidity</div>
        <div class="value"><span id="h">--</span><span class="unit">%</span></div>
      </div>
    </div>
    <div id="smeta" class="meta"></div>
  </section>

  <section>
    <h1>Relay</h1>
    <div id="relay-ind" class="ind relay"></div>
    <div id="relay-state" class="state">...</div>
    <div class="buttons">
      <button onclick="setOut('relay','on')">On</button>
      <button onclick="setOut('relay','off')">Off</button>
      <button onclick="setOut('relay','toggle')">Toggle</button>
    </div>
  </section>

  <section>
    <h1>Built-in LED</h1>
    <div id="led-ind" class="ind led"></div>
    <div id="led-state" class="state">...</div>
    <div class="buttons">
      <button onclick="setOut('led','on')">On</button>
      <button onclick="setOut('led','off')">Off</button>
      <button onclick="setOut('led','toggle')">Toggle</button>
    </div>
  </section>
 </div>

  <details>
    <summary>Wiring</summary>
    <table>
      <tr><th colspan="2">DHT22-M sensor</th></tr>
      <tr><td>+</td><td>3V3</td></tr>
      <tr><td>OUT</td><td>D2 (GPIO4)</td></tr>
      <tr><td>&minus;</td><td>GND</td></tr>
      <tr><th colspan="2">RELC-1CH-5V-UNI relay</th></tr>
      <tr><td>DC+</td><td>VIN (5V)</td></tr>
      <tr><td>DC&minus;</td><td>GND</td></tr>
      <tr><td>IN</td><td>D5 (GPIO14)</td></tr>
      <tr><td>Trigger jumper</td><td>H (high)</td></tr>
    </table>
    <p class="note">Relay ON = COM&ndash;NO closed, COM&ndash;NC open.</p>
  </details>

  <div id="err"></div>
<script>
  const $ = id => document.getElementById(id);
  function ok() { $('err').textContent = ''; }
  function fail() { $('err').textContent = 'Board not reachable'; }

  function showOut(name, d) {
    const on = d[name] === 'on';
    $(name + '-ind').classList.toggle('on', on);
    $(name + '-state').textContent = on ? 'ON' : 'OFF';
    ok();
  }
  function getOut(name) {
    fetch('/api/' + name).then(r => r.json()).then(d => showOut(name, d)).catch(fail);
  }
  function setOut(name, s) {
    fetch('/api/' + name + '?state=' + s, { method: 'POST' })
      .then(r => r.json()).then(d => showOut(name, d)).catch(fail);
  }
  function showSensor(d) {
    const m = $('smeta');
    if (d.ok) {
      $('t').textContent = d.temperature.toFixed(1);
      $('h').textContent = d.humidity.toFixed(1);
      m.textContent = 'updated ' + (d.age_ms / 1000).toFixed(1) + ' s ago';
      m.classList.remove('bad');
    } else {
      $('t').textContent = '--';
      $('h').textContent = '--';
      m.textContent = 'Sensor: ' + d.error;
      m.classList.add('bad');
    }
    ok();
  }
  function refresh() {
    getOut('relay');
    getOut('led');
    fetch('/api/sensor').then(r => r.json()).then(showSensor).catch(fail);
  }
  refresh();
  setInterval(refresh, 2000);
</script>
</body>
</html>
)rawliteral";

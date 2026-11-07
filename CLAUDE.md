# CLAUDE.md

## Hardware
- Board: NodeMCU 1.0 (ESP-12E, ESP8266), USB-serial chip CP2102 on **COM3**.
  - COM5 / COM6 are Bluetooth serial ports — not the board.
  - `arduino-cli board list` shows the board as "Unknown"; identify it by port, not auto-detect.
- FQBN: `esp8266:esp8266:nodemcuv2` (core `esp8266:esp8266` 3.1.2 installed).
- Built-in LED: `LED_BUILTIN` (GPIO2) is **active LOW** (`LOW` = on).
- Pinout / wiring references: `files/` (NodeMCU pinout images, relay notes, `terv.*` design).

## Toolchain (arduino-cli 1.5.1, Windows)
- Sketch folder name must match the `.ino` name (e.g. `blink/blink.ino`).
- Compile: `arduino-cli compile --fqbn esp8266:esp8266:nodemcuv2 <sketch>`
- Upload: **use 115200 upload baud** — the default speed fails mid-flash with
  `Failed to write compressed data to flash ... (result was C100)`:
  `arduino-cli upload -p COM3 --fqbn esp8266:esp8266:nodemcuv2:baud=115200 <sketch>`
- Monitor: `arduino-cli monitor -p COM3 --config 74880` (Ctrl+C to quit).
  Sketches use `Serial.begin(74880)` = the ESP8266 ROM boot-message speed, so boot log + sketch output are both readable.
  Upload speed (`:baud=115200` in FQBN) is unrelated and stays 115200.
- `arduino-cli upload` (1.5.1) exits 0 even when esptool fails (e.g. port busy) — check output for
  `Hash of data verified` instead of trusting the exit code (`flush.cmd` does this).
  Close any monitor before uploading — COM3 can only be opened by one program.
- Non-interactive serial check (PowerShell), useful for verifying output:
  `$p = New-Object System.IO.Ports.SerialPort 'COM3',74880; $p.Open(); Start-Sleep 7; $p.ReadExisting(); $p.Close()`

## Conventions
- Serial output at 74880 baud.
- Prefer non-blocking `millis()` timing over `delay()` in `loop()`.

## Projects
- `samples/blink/` — built-in LED 3 s on / 3 s off, prints `[ms] LED ON/OFF (cycle n)` over serial.
- `samples/wifiwebserver/` — joins home Wi-Fi (STA), serves LED control page on `/` (HTML in `index_html.h`,
  PROGMEM) + API: `GET /api/led`, `POST /api/led?state=on|off|toggle`, `GET /api/sensor` (cached DHT22 reading,
  refreshed every 2.5 s in `loop()`). IP is DHCP (was 192.168.0.10), printed on serial.

## Wiring
- DHT22-M module (3-pin, pull-up on board): `+` -> 3V3, `OUT` -> D2 (GPIO4), `-` -> GND. Libs: "DHT sensor library",
  "Adafruit Unified Sensor". Power from 3V3, not VIN/5V (module pull-up would put 5 V on the GPIO).
- RELC-1CH-5V-UNI relay: `DC+` -> VIN (5V from USB), `DC-` -> GND, `IN` -> D5 (GPIO14), trigger jumper = HIGH
  (GPIO HIGH = on = COM-NO closed; see `files/RELAY.txt`). Driven OFF first thing in `setup()`.
  (`_old/.../egycommand` had the relay on D2 — moved to D5 because D2 is now the DHT22.)
- API: `GET /api/relay`, `POST /api/relay?state=on|off|toggle`. LED and relay share the `Output` struct in the sketch.
- PWMMOS-4 (4-ch MOSFET board, 60N03 MOSFETs, LTV217 opto inputs; datasheet hestore prod_10047366): `PWM1` -> D1 (GPIO5),
  `GND1` -> ESP GND. `DC+/DC-` = load supply (3.7-27 V), kept isolated from ESP GND. Low-side switch: load on
  `OUTx+`/`OUTx-`, measure across those. 3.3 V input works (IN LED faint) and switches the output (user-verified).
  PWM must be ~100 Hz (`analogWriteFreq(100)`): gate discharges via 100k, too slow for 1 kHz.
  API: `GET /api/pwm1`, `POST /api/pwm1?duty=0..100`. Slider control via D1 user-verified with a meter.
- Free GPIOs for later: D6 (GPIO12), D7 (GPIO13); D0 (GPIO16) has no PWM.
- Sketch gotcha: declare all structs before the first function — the Arduino preprocessor inserts auto-prototypes there.
- Avoid D3/D4/D8 (GPIO0/2/15) for peripherals: boot-mode pins. D4 is also the built-in LED.
  `flush.cmd` in the folder compiles + uploads (note: strip trailing `\` from `%~dp0` before quoting it).

## Secrets
- Wi-Fi credentials live OUTSIDE the repo in a private library:
  `C:\Users\ghjkl\Documents\Arduino\libraries\Secrets\Secrets.h` (`WIFI_SSID`, `WIFI_PASS`), used via `#include <Secrets.h>`.
  Template: `samples/Secrets.example.h`.
- Never read, print or log that file or its values (don't print SSID/password to serial either).
- ESP8266 ROM bootloader prints at 74880 baud on every reset (`rst cause:` 2 = reset pin/RTS, 4 = watchdog crash).
- `_old/ConsoleApp1/` — older .NET serial console app (System.IO.Ports), not active.

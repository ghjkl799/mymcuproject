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
  PROGMEM) + API: `GET /api/led`, `POST /api/led?state=on|off|toggle`. IP is DHCP (was 192.168.0.10), printed on serial.
  `flush.cmd` in the folder compiles + uploads (note: strip trailing `\` from `%~dp0` before quoting it).

## Secrets
- Wi-Fi credentials live OUTSIDE the repo in a private library:
  `C:\Users\ghjkl\Documents\Arduino\libraries\Secrets\Secrets.h` (`WIFI_SSID`, `WIFI_PASS`), used via `#include <Secrets.h>`.
  Template: `samples/Secrets.example.h`.
- Never read, print or log that file or its values (don't print SSID/password to serial either).
- ESP8266 ROM bootloader prints at 74880 baud on every reset (`rst cause:` 2 = reset pin/RTS, 4 = watchdog crash).
- `_old/ConsoleApp1/` — older .NET serial console app (System.IO.Ports), not active.

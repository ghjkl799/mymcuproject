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
- Monitor: `arduino-cli monitor -p COM3 --config 115200` (Ctrl+C to quit).
  Close any monitor before uploading — COM3 can only be opened by one program.
- Non-interactive serial check (PowerShell), useful for verifying output:
  `$p = New-Object System.IO.Ports.SerialPort 'COM3',115200; $p.Open(); Start-Sleep 7; $p.ReadExisting(); $p.Close()`

## Conventions
- Serial output at 115200 baud.
- Prefer non-blocking `millis()` timing over `delay()` in `loop()`.

## Projects
- `samples/blink/` — built-in LED 3 s on / 3 s off, prints `[ms] LED ON/OFF (cycle n)` over serial.
- `samples/wifiwebserver/` — SoftAP `myesp` / `123456789aA`, web server on http://192.168.4.1/ serving "Hello World".
  `flush.cmd` in the folder compiles + uploads (note: strip trailing `\` from `%~dp0` before quoting it).
- Serial garbage at boot is the ESP8266 ROM bootloader at 74880 baud — normal.
- `_old/ConsoleApp1/` — older .NET serial console app (System.IO.Ports), not active.

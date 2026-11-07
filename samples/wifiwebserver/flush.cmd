@echo off
rem Compile and upload this sketch to the NodeMCU (ESP-12E) on COM3.
rem Close any serial monitor on COM3 first, or the upload will fail.
setlocal
set PORT=COM3
set FQBN=esp8266:esp8266:nodemcuv2
set SKETCH=%~dp0
set SKETCH=%SKETCH:~0,-1%

arduino-cli compile --fqbn %FQBN% "%SKETCH%" || goto :error
rem arduino-cli 1.5.1 exits 0 even when esptool fails, so check the log for a verified write.
set LOG=%TEMP%\flush_upload.log
arduino-cli upload -p %PORT% --fqbn %FQBN%:baud=115200 "%SKETCH%" > "%LOG%" 2>&1
set RC=%ERRORLEVEL%
type "%LOG%"
if not "%RC%"=="0" goto :error
findstr /C:"Hash of data verified" "%LOG%" >nul || goto :error

echo.
echo Upload OK. Monitor with: arduino-cli monitor -p %PORT% --config 74880
exit /b 0

:error
echo.
echo FAILED (is a serial monitor still holding %PORT%?)
exit /b 1

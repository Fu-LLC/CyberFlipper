@echo off
REM CYBERFLIPPER Build & Package Script (Windows)
REM Creates the deployment .tgz for Flipper Zero SD card installation
REM Usage: build.cmd [package|clean]

echo.
echo   =============================================
echo     CYBERFLIPPER v1.0.0 - Build System
echo   =============================================
echo.

if "%1"=="clean" goto clean
if "%1"=="flash" goto flash
goto package

:package
echo [*] Creating deployment package...
if not exist dist mkdir dist
if not exist dist\CYBERFLIPPER mkdir dist\CYBERFLIPPER

echo   [+] Copying firmware.dfu...
if exist firmware.dfu copy /Y firmware.dfu dist\CYBERFLIPPER\ >nul
echo   [+] Copying radio.bin...
if exist radio.bin copy /Y radio.bin dist\CYBERFLIPPER\ >nul
echo   [+] Copying splash.bin...
if exist splash.bin copy /Y splash.bin dist\CYBERFLIPPER\ >nul
echo   [+] Copying updater.bin...
if exist updater.bin copy /Y updater.bin dist\CYBERFLIPPER\ >nul
echo   [+] Copying update.fuf...
if exist update.fuf copy /Y update.fuf dist\CYBERFLIPPER\ >nul
echo   [+] Copying resources.tar...
if exist resources.tar copy /Y resources.tar dist\CYBERFLIPPER\ >nul

echo.
echo   [+] Creating CYBERFLIPPER-v1.0.0.tgz...
if exist "C:\Program Files\Git\usr\bin\tar.exe" (
    "C:\Program Files\Git\usr\bin\tar.exe" -czf dist\CYBERFLIPPER-v1.0.0.tgz -C dist CYBERFLIPPER
) else (
    tar -czf dist\CYBERFLIPPER-v1.0.0.tgz -C dist CYBERFLIPPER
)
echo.
echo   [SUCCESS] Package created: dist\CYBERFLIPPER-v1.0.0.tgz
goto end

:clean
echo [*] Cleaning build artifacts...
if exist dist rmdir /S /Q dist
echo [+] Clean complete
goto end

:flash
echo.
echo   === FLASHING INSTRUCTIONS ===
echo   1. Insert your Flipper Zero SD card into your PC
echo   2. Copy the CYBERFLIPPER folder to: SD:\update\CYBERFLIPPER
echo   3. Safely eject the SD card
echo   4. On Flipper: Browse to update.fuf and execute
echo.
goto end

:end
echo.

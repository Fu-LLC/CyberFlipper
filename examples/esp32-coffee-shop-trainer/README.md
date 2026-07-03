# ESP32 Coffee Shop WiFi Training Node

Lab-only Arduino sketch for an ESP32/XH-32S style board. It creates a controlled training AP, serves a local telemetry page, scans nearby 2.4 GHz WiFi networks, and exposes JSON endpoints for a packet-analysis dashboard.

This is designed for legal training environments. It does not collect credentials, does not impersonate real networks, does not deauthenticate clients, and does not decrypt private traffic.

## Hardware

Known-good target:

- Sparkle IoT XH-32S / ESP32-DevKit style board
- ESP32 Dev Module profile in Arduino IDE
- Micro-USB data cable, not a charge-only cable

The most common flashing failure is the USB cable. A power-only cable will light the board but will not show a serial port or will behave inconsistently.

## Arduino IDE setup

1. Install Arduino IDE 2.x.
2. Add ESP32 boards package:
   - File / Settings / Additional Boards Manager URLs
   - Add: `https://espressif.github.io/arduino-esp32/package_esp32_index.json`
3. Open Boards Manager and install `esp32 by Espressif Systems`.
4. Select board: `ESP32 Dev Module`.
5. Start with these settings:
   - Upload Speed: `115200`
   - CPU Frequency: `240MHz (WiFi/BT)`
   - Flash Frequency: `40MHz`
   - Flash Mode: `DIO`
   - Flash Size: `4MB`
   - Partition Scheme: `Default 4MB with spiffs`
   - Core Debug Level: `None`

If flashing is stable later, raise upload speed to `460800` or `921600`. If it estimates very long upload times, stop and fix the USB/port/driver issue first.

## Manual bootloader sequence

Many ESP32 dev boards need this timing:

1. Plug in the ESP32 with a known data cable.
2. Select the correct serial port.
3. Click Upload.
4. When Arduino says `Connecting...`, hold `BOOT`.
5. When writing starts, release `BOOT`.
6. If the sketch does not start after flashing, press `EN`/`RST` once.

Do not hold both buttons the whole time. Usually only `BOOT` is held during the `Connecting...` phase.

## Verify the cable before fighting the board

A real data cable should cause a USB serial device to appear.

Windows:

- Open Device Manager.
- Look under `Ports (COM & LPT)`.
- Common names: `Silicon Labs CP210x`, `USB-SERIAL CH340`, `USB Serial Device`.

macOS:

```bash
ls /dev/cu.*
```

Expected examples:

```text
/dev/cu.SLAB_USBtoUART
/dev/cu.usbserial-0001
/dev/cu.wchusbserial*
```

Linux:

```bash
ls /dev/ttyUSB* /dev/ttyACM*
```

Expected examples:

```text
/dev/ttyUSB0
/dev/ttyACM0
```

If no serial device appears, the cable is probably charge-only or the USB serial driver is missing.

## Optional esptool sanity test

Install esptool:

```bash
python -m pip install esptool
```

Check the chip. Replace the port with your actual port.

Windows:

```bash
python -m esptool --chip esp32 --port COM3 chip_id
```

macOS/Linux:

```bash
python -m esptool --chip esp32 --port /dev/cu.usbserial-0001 chip_id
```

Erase flash only when you want a clean reset:

```bash
python -m esptool --chip esp32 --port COM3 erase_flash
```

Then flash from Arduino IDE again.

## What the sketch exposes

After flashing, connect to:

```text
SSID: FuLLC-CoffeeLab
Password: training123
URL: http://192.168.4.1/
```

Endpoints:

```text
/          training dashboard
/status    JSON status
/scan      JSON nearby 2.4 GHz WiFi networks
/events    JSON event log
```

Use this ESP32 as the edge training node. For real packet capture and analysis, use a laptop/Raspberry Pi with Wireshark, tshark, Zeek, or Suricata, then feed clean training datasets into the web app.

## Common flashing errors

`A fatal error occurred: Failed to connect to ESP32: Timed out waiting for packet header`

Fix:

- Hold `BOOT` only when `Connecting...` appears.
- Try upload speed `115200`.
- Press `EN` once, then upload again.
- Try another USB data cable.

`No serial port` or port disappears:

Fix:

- Use a real data cable.
- Install CP210x or CH340 driver depending on your board.
- Try another USB port.

Upload estimate says it will take many minutes:

Fix:

- Stop the upload.
- Use a better cable.
- Set upload speed to `115200` for reliability, then test `460800` later.
- Close Serial Monitor before uploading.

Sketch uploads but nothing happens:

Fix:

- Open Serial Monitor at `115200` baud.
- Press `EN`/`RST`.
- Confirm the AP appears as `FuLLC-CoffeeLab`.

## File

Sketch: `CoffeeShopTrainer.ino`

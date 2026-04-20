# SD Card Structure

After extracting `CYBERFLIPPER-v1.2.1-SD_CARD.zip`, copy these folders to the root of your Flipper SD card via the **qFlipper SD Card tab**:

```
SD:/
  badusb/         -- BadUSB HID payloads + CVE scripts
  infrared/       -- IR remote databases
  nfc/            -- NFC dumps, Amiibo, hotel keys
  lfrfid/         -- Low-frequency RFID dumps
  subghz/         -- Sub-GHz signals, gate codes, vehicles
  dolphin/        -- Dolphin XP level data and passport icon
  apps/           -- Extra .fap applications (Games, Tools, NFC, etc.)
  u2f/            -- U2F key assets
```

## What NOT to copy
- Do **not** copy `firmware.dfu`, `radio.bin`, `updater.bin`, `splash.bin`, or `update.fuf` to the SD card.
- These are firmware flash files and are not used with the SD card content method.
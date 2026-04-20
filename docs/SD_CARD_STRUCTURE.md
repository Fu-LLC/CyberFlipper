# SD Card Structure

## After Step 1 (Firmware Update)
The firmware update goes in:
```
SD:/update/CYBERFLIPPER-v1.2.1/
    firmware.dfu
    radio.bin
    splash.bin
    updater.bin
    update.fuf
    resources.tar
```
Run `update.fuf` from the Flipper File Browser to apply.

## After Step 2 (Content Pack)
Copy these folders to the SD card root:
```
SD:/
    badusb/         -- BadUSB HID payloads + CVE scripts
    infrared/       -- IR remote databases
    nfc/            -- NFC dumps, Amiibo, hotel keys
    lfrfid/         -- Low-frequency RFID dumps
    subghz/         -- Sub-GHz signals, gate codes, vehicles
    dolphin/        -- Dolphin XP level data
    apps/           -- Extra .fap applications
    u2f/            -- U2F key assets
```
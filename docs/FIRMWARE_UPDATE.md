# CyberFlipper — SD Card Installation

CyberFlipper is installed by copying content to your Flipper's SD card using qFlipper. There is no firmware flashing involved.

## Step-by-Step

1. Download `CYBERFLIPPER-v1.2.1-SD_CARD.zip` from the [Releases page](https://github.com/Fu-LLC/CyberFlipper/releases).
2. Extract the zip to a folder on your PC.
3. Open **qFlipper** and connect your Flipper via USB.
4. Click the **SD Card** tab.
5. Copy the following folders to the SD card root:
   - `badusb/` — BadUSB / HID payloads + CVE scripts
   - `infrared/` — IR remote databases
   - `nfc/` — NFC card dumps and Amiibo
   - `subghz/` — Sub-GHz signals and brute-force sets
   - `lfrfid/` — Low-frequency RFID dumps
   - `dolphin/` — Dolphin XP level data and passport icon
   - `apps/` — Extra .fap applications (Games, Tools, NFC, etc.)
   - `u2f/` — U2F key data
6. Eject via qFlipper and reboot your Flipper.

## Building the SD Card zip locally

```bash
python scripts/build_update.py
```

This produces `CYBERFLIPPER-v<VER>-SD_CARD.zip` — the only release artifact.

## Notes
- Do **not** use "Install from file" in qFlipper — that method is not supported.
- Do **not** attempt to flash `update.fuf`, `.tgz`, or `.dfu` files.
- All content is SD card data only; your Flipper firmware is unchanged.

For setup help see `docs/GETTING_STARTED.md`.
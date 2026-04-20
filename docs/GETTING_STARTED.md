# Getting Started with CyberFlipper

## Requirements
- Flipper Zero with SD card inserted
- [qFlipper](https://flipperzero.one/update) installed on your PC/Mac/Linux

---

## Step 1 — Firmware Update (do this first)

1. Download **`CYBERFLIPPER-v1.2.1-UPDATE.zip`** from the [Releases page](https://github.com/Fu-LLC/CyberFlipper/releases).
2. Extract the zip — you get a folder named `CYBERFLIPPER-v1.2.1/`.
3. Open **qFlipper** → connect your Flipper → click the **SD Card** tab.
4. Copy the entire `CYBERFLIPPER-v1.2.1/` folder into the **`update/`** folder on the SD card.
   - Full path on SD card: `SD:/update/CYBERFLIPPER-v1.2.1/`
5. On your Flipper: **Main Menu → File Browser → update → CYBERFLIPPER-v1.2.1 → update.fuf → Run in App**
6. The Flipper will reboot and apply the firmware update automatically.

---

## Step 2 — Load SD Card Content

1. Download **`CYBERFLIPPER-v1.2.1-SD_CARD.zip`** from the [Releases page](https://github.com/Fu-LLC/CyberFlipper/releases).
2. Extract the zip — you get folders: `badusb/`, `infrared/`, `nfc/`, `subghz/`, `lfrfid/`, `dolphin/`, `apps/`, `u2f/`.
3. Open **qFlipper** → **SD Card** tab → drag and drop all extracted folders onto the SD card root.
4. Eject and reboot your Flipper.

---

## Web Dashboard
Open `web/index.html` in your browser for the CyberFlipper intelligence dashboard.

## Support
Open an issue on GitHub or visit [fllc.net](https://fllc.net)
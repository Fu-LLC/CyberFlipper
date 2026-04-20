# Firmware Update Instructions

## Build a CyberFlipper release package locally

1. Clone the repository and change into its root folder.
2. Ensure these files exist in the repo root:
   - `firmware.dfu`
   - `updater.bin`
   - `radio.bin`
   - `splash.bin`
   - `resources.tar`
   - `update.fuf`
   - `manifest.json`
3. Build the release package:
   ```bash
   bash scripts/build_update.sh
   ```
4. The script creates:
   - `CYBERFLIPPER-v<VER>.tgz`
   - `CYBERFLIPPER-v<VER>-SD_CARD.zip`
   - `CYBERFLIPPER-v<VER>-FULL.zip`

## Install using qFlipper or mobile app

1. Open qFlipper and choose **Install from file**.
2. Select `CYBERFLIPPER-v<VER>.tgz`.
3. Follow the prompts until the update completes.

## Manual SD card install

1. Extract `CYBERFLIPPER-v<VER>.tgz`.
2. Copy the `update/` folder to the root of your Flipper SD card.
3. Copy the contents of `CYBERFLIPPER-v<VER>-SD_CARD.zip` to the SD card root.
4. On the Flipper: **Settings → Storage → Run update**.

## GitHub release assets

Use the `.tgz` release asset from the GitHub release page for the cleanest install. If the GitHub release is still building, wait for the latest automated workflow to complete.

For troubleshooting, see `docs/GETTING_STARTED.md`.

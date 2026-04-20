# CyberFlipper — Installation Guide

## Two packages, two steps:

| Package | Purpose | Where it goes |
| :--- | :--- | :--- |
| `CYBERFLIPPER-v1.2.1-UPDATE.zip` | Firmware update | `SD:/update/CYBERFLIPPER-v1.2.1/` |
| `CYBERFLIPPER-v1.2.1-SD_CARD.zip` | Apps, payloads, signals | SD card root |

---

## Step 1 — Apply Firmware Update

1. Download and extract `CYBERFLIPPER-v1.2.1-UPDATE.zip`.
2. In **qFlipper SD Card tab**, copy the `CYBERFLIPPER-v1.2.1/` folder into `SD:/update/`.
3. On Flipper: **File Browser → update → CYBERFLIPPER-v1.2.1 → update.fuf → Run in App**.
4. Flipper reboots and installs the firmware. Wait for it to complete.

## Step 2 — Load Content Pack

1. Download and extract `CYBERFLIPPER-v1.2.1-SD_CARD.zip`.
2. In **qFlipper SD Card tab**, copy all folders to SD card root.
3. Reboot Flipper.

## Build locally

```bash
python scripts/build_update.py
```

Produces both `UPDATE.zip` and `SD_CARD.zip`.

## Notes
- Do **not** use qFlipper "Install from file" — it is not supported for this package.
- The firmware update (Step 1) must be done before apps will work correctly.
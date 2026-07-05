# CyberFlipper Awesome Watch — 2026-07-05

## Defensive source signals

This update converts current Flipper ecosystem activity into safe lab documentation and visible local workstation worksheets. No offensive code was copied.

### Official firmware

Observed release posture:

- `flipperdevices/flipperzero-firmware` still presents `1.4.3` as the latest listed release.
- `1.4.3` is a bugfix release for `1.4.2`, focused on an Infrared CLI plugin import fix.
- `1.4.2` remains the larger defensive-review signal: NFC CLI expansion, FeliCa and MIFARE parser work, BLE pairing-security guidance, Sub-GHz protocol additions/fixes, BadUSB key-combo handling fixes, HID remote changes, CLI improvements, iButton write support, and LF RFID display improvements.

CyberFlipper conversion:

- Treat firmware/app changes as change-control inputs.
- Track parser, protocol, CLI, and HID changes as validation targets.
- Require sandbox notes before running new `.fap`, BadUSB, RF, NFC/RFID, or firmware-adjacent content.

### Unleashed firmware dev branch

Recent dev-history signals include app bumps, NFC parser/display work, BLE sync/delay fixes, CI/API-change reporting, free-flash reporting, and a Sub-GHz RAW transmission crash-prevention fix.

CyberFlipper conversion:

- App bumps become compatibility-checklist entries.
- NFC and LFRFID changes become owned-card lab validation notes only.
- BLE changes become re-pairing and pairing-record hygiene notes.
- Sub-GHz RAW crash-prevention becomes a safety-control case study: do not transmit unknown files; validate receive-only first; record firmware version, region, and antenna/module state.
- CI/API-change reporting becomes a CyberFlipper review pattern for source-watch updates.

### Risky-source conversion

Repositories involving Marauder, jammer, fuzzing, cracking, captive portals, RF transmission, or offensive payloads are not copied into CyberFlipper. They are used only for:

- safety disclaimers,
- lawful lab boundary notes,
- defensive detections,
- change-intake forms,
- interoperability warnings,
- sandbox requirements,
- and mitigation-first documentation.

## Level added

Level 080: Sandboxed Analysis Setup and Runbook.

Purpose:

- Create a repeatable pre-run worksheet before testing new firmware, apps, BadUSB scripts, IR packs, NFC/RFID notes, or RF-related files.
- Force visible local output using `cyberflipper_` filenames.
- Improve change discipline before public release or merge.

Files:

```text
badusb/CyberFlipper_Lab/level_080/README.md
badusb/CyberFlipper_Lab/level_080/cf_l080_windows_sandbox_readiness.txt
badusb/CyberFlipper_Lab/level_080/cf_l080_linux_sandbox_readiness.txt
badusb/CyberFlipper_Lab/level_080/cf_l080_macos_sandbox_readiness.txt
docs/community/CYBERFLIPPER_LEVEL_080_SANDBOX_RUNBOOK.md
docs/community/CYBERFLIPPER_SOURCE_WATCH_2026-07-05.md
```

## Human review requirement

Human review is required before merge because the pack launches local shells/editors and creates workstation readiness worksheets. The scripts are visible, local-only, and defensive, but still qualify as host-interactive BadUSB content.

## Exclusions

This update does not include credential theft, token/cookie extraction, browser-profile collection, persistence, stealth, destructive actions, privilege abuse, AV bypass, RF jamming, Wi-Fi cracking, captive credential capture, exploit chains, malware behavior, or unauthorized testing guidance.

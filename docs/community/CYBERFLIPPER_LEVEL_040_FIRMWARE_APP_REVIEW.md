# CyberFlipper Level 040 — Firmware and App Review Notes

## Review model

Level 040 treats firmware and app ecosystem changes as provenance and compatibility work, not exploit replication.

## Firmware review fields

| Field | Record |
|---|---|
| Device | Flipper Zero serial or lab asset tag |
| Firmware channel | Official / community / lab build |
| Firmware version | Manual entry |
| qFlipper version | Manual entry |
| SD-card package date | Manual entry |
| App source | Official catalog / source repository / local build |
| License reviewed | Yes / No |
| Risk class | Safe docs / host shell / firmware-sensitive / RF-sensitive |
| Human approval required | Yes / No |

## App catalog review fields

| Field | Purpose |
|---|---|
| App name | Identify package under review. |
| Source repository | Confirm provenance. |
| Maintainer | Identify upstream owner. |
| Last update date | Support stale-package review. |
| Firmware compatibility | Confirm target firmware branch. |
| Interfaces | Note GPIO, NFC, Sub-GHz, IR, BLE, USB, storage, or external module usage. |
| Lab status | Public-safe, owned-device only, RF-sensitive, classroom-only, or do-not-publish. |

## Community firmware handling

Community firmware may be useful for UX comparison, plugin discovery, and compatibility research. CyberFlipper should not copy firmware features blindly. Record deltas, cite source names, and keep risky behavior out of public training packs.

## Risky source handling

For Marauder, jammer, fuzzing, cracking, RF, NFC/RFID, and offensive-adjacent repositories:

- Extract safety controls.
- Extract detection ideas.
- Extract lab-only warnings.
- Extract interoperability notes.
- Do not copy exploitation flows.
- Do not publish RF misuse steps.
- Do not publish credential capture or cracking workflows.

## Merge policy

Human review is required for shell-launching BadUSB files, host inventory scripts, firmware bundles, RF/Sub-GHz material, NFC/RFID emulation notes, fuzzer-derived test plans, and content that could be misunderstood as operational offensive guidance.

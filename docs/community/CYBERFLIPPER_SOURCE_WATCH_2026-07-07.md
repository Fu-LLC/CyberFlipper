# CyberFlipper Source Watch — 2026-07-07

## Summary

The current safe update path is not to add more capability. The current safe update path is to improve release discipline around the capabilities already represented in CyberFlipper: BadUSB worksheets, SD-card content organization, firmware/app compatibility review, IR/NFC/RFID notes, GPIO/hardware practice, and community-source attribution.

## Upstream signals

| Source | Observed signal | CyberFlipper action |
|---|---|---|
| `flipperdevices/flipperzero-firmware` | `1.4.3` latest release; bugfix for `1.4.2`; Infrared CLI plugin fix. | Add release-gate field for firmware version and CLI-impact notes. |
| `flipperdevices/flipperzero-firmware` | `1.4.2` feature notes include NFC CLI/parsers, Sub-GHz protocol changes, BLE pairing-security guidance, JS GUI changes, Infrared updates, BadUSB key-combo fixes, HID, CLI, iButton, and LF RFID changes. | Keep Level 099 focused on provenance, compatibility, and reviewer signoff. |
| `flipperdevices/flipper-application-catalog` | App workflows continue to make SD-card provenance and compatibility important. | Keep app-catalog review fields in the release worksheet. |
| `DarkFlippers/unleashed-firmware` | Community firmware is useful for app bump, API-change, and compatibility-watch patterns. | Convert to feature-matrix and review-gate language only. |
| `Flipper-XFW/Xtreme-Firmware` | Community UX and firmware feature organization can inspire documentation layout. | Use only for safe comparison and compatibility notes. |
| `RogueMaster/flipperzero-firmware-wPlugins` | Plugin-heavy firmware reinforces the need for app inventory and source provenance. | Require source and license notes for public packs. |
| `UberGuidoZ/Flipper` | Large SD-card collections show the value of taxonomy and disclaimers. | Use for organization patterns, not direct payload copying. |
| `UberGuidoZ/Flipper-IRDB` and `Lucaslhm/Flipper-IRDB` | IR databases show remote taxonomy, attribution needs, and duplicate-management concerns. | Keep IR content attribution-first and benign. |
| `RocketGod-git/ProtoPirate` | Presentation style and feature-matrix framing are useful. | Use for documentation structure only. |
| `RocketGod-git/flipper-zero-rf-jammer` | RF misuse risk is high. | Do not copy behavior. Convert to lawful RF safety and detection notes only. |
| `SkeletonMan03/FZEasyMarauderFlash` | Flashing workflow UX can help lab setup documentation. | Use for setup checklists and consent warnings only. |
| `kbembedded/Flipper-Zero-Game-Boy-Pokemon-Trading` | GPIO timing and retro hardware work are useful for safe bench notes. | Convert to hardware bring-up and test-fixture guidance. |
| `DarkFlippers/Multi_Fuzzer` | Fuzzing concepts create safety concerns. | Convert to offline parser-review and crash-taxonomy worksheets only. |
| `DarkFlippers/qFlipper` | Desktop tooling supports update and device-management workflows. | Use for operator update discipline and provenance records. |

## Defensive source alignment

CyberFlipper should continue to prioritize:

- CISA KEV-style exposure and urgency tracking.
- CISA advisory language for affected products, mitigations, and known exploitation.
- NIST CSF 2.0 governance language for authorization, roles, review, and recovery.
- Vendor advisories for firmware, desktop OS, browser, and device-management changes.
- Official Flipper documentation and firmware release notes for device behavior.
- Community repositories only when attribution is clear and the conversion is public-safe.

## Content recommendation

Add Level 099 as a capstone release gate. The pack should not introduce a new technical action. It should require the operator to prove that all prior technical actions are authorized, visible, local, attributed, documented, and reviewable.

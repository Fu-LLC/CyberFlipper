# CyberFlipper Awesome Watch — 2026-07-03

## Daily Theme

Level 060 adds SD-card provenance, SBOM-lite inventory, and evidence discipline for CyberFlipper lab packs.

This update treats Flipper ecosystem changes as supply-chain and lab-validation signals: firmware changes, app catalog changes, community SD-card collections, IR databases, Marauder-style flashing flows, fuzzing tools, and jammer/RF tooling are reviewed for documentation, attribution, compatibility, and safety controls only.

## Source Review

Primary watch areas:

- `flipperdevices/flipperzero-firmware` — official firmware behavior, CLI, NFC, Sub-GHz, BadUSB, BLE, and storage behavior.
- `flipperdevices/flipper-application-catalog` — app metadata, app packaging, app compatibility, release hygiene.
- `DarkFlippers/unleashed-firmware` — community firmware deltas and API/feature surface changes.
- `Flipper-XFW/Xtreme-Firmware` — community UX, feature organization, and compatibility notes.
- `RogueMaster/flipperzero-firmware-wPlugins` — plugin bundle organization and user-facing app surface.
- `UberGuidoZ/Flipper`, `UberGuidoZ/Flipper-IRDB`, `Lucaslhm/Flipper-IRDB` — SD-card organization, IR attribution patterns, collection hygiene.
- `SkeletonMan03/FZEasyMarauderFlash` — flashing UX and user-proof setup documentation.
- `DarkFlippers/Multi_Fuzzer` — reader-test lab disclaimers, safety boundaries, controlled-target assumptions.
- `RocketGod-git/ProtoPirate` and RF/jammer-themed examples — presentation and warnings only; no RF abuse, jamming, transmission, or bypass logic.

## External Defensive Anchors

- CISA Known Exploited Vulnerabilities Catalog: https://www.cisa.gov/known-exploited-vulnerabilities-catalog
- CISA Secure by Design: https://www.cisa.gov/securebydesign
- NIST SP 800-61 Rev. 2 Computer Security Incident Handling Guide: https://csrc.nist.gov/publications/detail/sp/800-61/rev-2/final
- NIST SP 800-53 Rev. 5 security controls: https://csrc.nist.gov/publications/detail/sp/800-53/rev-5/final
- Official Flipper Zero documentation: https://docs.flipper.net/
- Official Flipper Zero firmware repository: https://github.com/flipperdevices/flipperzero-firmware
- Official Flipper application catalog repository: https://github.com/flipperdevices/flipper-application-catalog

## What Changed

Previous daily levels added host triage, patch-SLA worksheets, USB/HID policy review, firmware/app review notes, and evidence handoff. Level 060 shifts to artifact provenance:

- What SD-card files exist?
- Which files are expected for a lab pack?
- Which files lack source notes?
- Which files need license attribution?
- Which folders should be excluded from public release?
- Which files changed between test runs?
- Which outputs can be hashed and handed off cleanly?

## Why It Matters

Flipper repositories often blend firmware, apps, IR databases, BadUSB text, NFC/RFID examples, Sub-GHz notes, GPIO experiments, and community assets. Without provenance, a public lab repo becomes hard to review and easy to misrepresent.

CyberFlipper should keep a clear separation between:

- Original training scripts.
- Attributed community examples.
- Official documentation references.
- Lab-only material.
- Public-safe release material.
- Material requiring human review.

## Safe Content Added

Level 060 adds:

```text
badusb/CyberFlipper_Lab/level_060/README.md
badusb/CyberFlipper_Lab/level_060/cf_l060_windows_sd_provenance.txt
badusb/CyberFlipper_Lab/level_060/cf_l060_linux_sd_provenance.txt
badusb/CyberFlipper_Lab/level_060/cf_l060_macos_sd_provenance.txt
docs/community/CYBERFLIPPER_LEVEL_060_SBOM_PROVENANCE.md
docs/community/CYBERFLIPPER_BADUSB_LINT_CHECKLIST.md
```

The BadUSB files create visible local `cyberflipper_` reports. They do not hide execution, transmit data, collect credentials, inspect browser profiles, touch password stores, modify system policy, perform RF actions, crack Wi-Fi, capture captive credentials, or exploit anything.

## Human Review Required

Do not auto-merge this pack without review. The BadUSB files launch visible local shells and enumerate files in user-selected CyberFlipper/Flipper SD-card folders. That is safe for authorized lab work, but still security-sensitive enough to require a human review gate.

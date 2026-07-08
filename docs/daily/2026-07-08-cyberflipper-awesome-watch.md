# CyberFlipper Awesome Watch — 2026-07-08

## Daily focus

Level 010 adds an authorized-use kickoff pack: operator authorization, scope boundaries, device provenance, firmware/app source notes, and release-review discipline before any BadUSB, RF, NFC/RFID, IR, firmware, Marauder, fuzzer, jammer, or offensive-adjacent community idea is converted into CyberFlipper training material.

This is intentionally early-level content. It gives new users a safe starting ritual before they touch higher-risk Flipper Zero workflows.

## Source signals reviewed

Primary public signals:

- Official Flipper firmware releases page:
  - `1.4.3` is listed as the latest release.
  - `1.4.3` is a bugfix release for firmware `1.4.2`.
  - `1.4.2` included NFC CLI and parser work, Sub-GHz protocol additions, BLE pairing-security guidance, BadUSB key-combo fixes, HID Remote configuration changes, CLI improvements, iButton write support, LF RFID animal-tag country display, JS GUI bindings, and Infrared remote updates.
  - Source: https://github.com/flipperdevices/flipperzero-firmware/releases

- NIST Cybersecurity Framework page:
  - NIST frames CSF 2.0 as a way for industry, government, and organizations to reduce cybersecurity risk.
  - The current NIST page highlights CSF 2.0 resources, profiles, mappings, quick-start guides, and ransomware/community profile updates.
  - Source: https://www.nist.gov/cyberframework

- CISA KEV posture:
  - The KEV catalog remains the correct external signal for exploited-vulnerability prioritization.
  - CISA’s public catalog page may block automated fetches, so this pack records KEV as a prioritization input rather than claiming a new item without a directly verified current entry.
  - Source: https://www.cisa.gov/known-exploited-vulnerabilities-catalog

Community and ecosystem ideas reviewed as defensive inspiration only:

- `flipperdevices/flipperzero-firmware`
- `flipperdevices/flipper-application-catalog`
- `DarkFlippers/unleashed-firmware`
- `Flipper-XFW/Xtreme-Firmware`
- `RogueMaster/flipperzero-firmware-wPlugins`
- `UberGuidoZ/Flipper`
- `UberGuidoZ/Flipper-IRDB`
- `Lucaslhm/Flipper-IRDB`
- `RocketGod-git/ProtoPirate`
- `RocketGod-git/flipper-zero-rf-jammer`
- `SkeletonMan03/FZEasyMarauderFlash`
- `kbembedded/Flipper-Zero-Game-Boy-Pokemon-Trading`
- `DarkFlippers/Multi_Fuzzer`
- `DarkFlippers/qFlipper`

## What changed for CyberFlipper

Recent CyberFlipper work has reached Level 099 capstone release-gate content in draft PRs. Level 010 now backfills the front of the curriculum with a repeatable authorization worksheet so the repository has a clearer training path:

1. Confirm the operator owns or administers the device, SD card, host, lab tag, remote, or test fixture.
2. Confirm RF/NFC/RFID/IR activities stay in lawful, controlled, non-interference settings.
3. Confirm BadUSB files create visible local `cyberflipper_` worksheets only.
4. Confirm source material is attributed and transformed into safe documentation rather than copied.
5. Confirm no credential, token, cookie, browser-profile, password-manager, persistence, evasion, destructive, RF-jamming, Wi-Fi-cracking, or unauthorized exploitation behavior is present.

## Why it matters

CyberFlipper is collecting ideas from an ecosystem that includes legitimate firmware/app work and offensive-adjacent community projects. A safe public lab needs a standard intake step before a user runs anything. The Level 010 worksheet turns authorization into evidence: who approved the lab, what host/device is in scope, what source inspired the work, what is out of scope, and what result file was produced.

## Safe content added

```text
badusb/CyberFlipper_Lab/level_010/README.md
badusb/CyberFlipper_Lab/level_010/cf_l010_windows_authorized_use_worksheet.txt
badusb/CyberFlipper_Lab/level_010/cf_l010_linux_authorized_use_worksheet.txt
badusb/CyberFlipper_Lab/level_010/cf_l010_macos_authorized_use_worksheet.txt
docs/community/CYBERFLIPPER_LEVEL_010_AUTHORIZATION_STANDARD.md
docs/community/CYBERFLIPPER_SOURCE_INTAKE_CARD.md
```

## Safety decision

This update is draft-only. It includes BadUSB-style text files. They are visible, local-output-only worksheets, but they still automate host input and therefore require human review before merge or public release.

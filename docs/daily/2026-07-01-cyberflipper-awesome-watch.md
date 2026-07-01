# CyberFlipper Awesome Watch — 2026-07-01

## Daily position

Today’s safe pack moves CyberFlipper into Level 040: firmware, application-catalog, and HID-policy review. The goal is not to reproduce offensive Flipper ecosystem behavior. The goal is to turn public firmware and community signals into defensive lab documentation, provenance checks, review worksheets, and visible local workstation posture reports.

## Source signals reviewed

| Source area | Signal | Safe CyberFlipper response |
|---|---|---|
| Official Flipper firmware | Firmware release notes remain the safest baseline for device behavior, APIs, BLE, BadUSB, NFC, Sub-GHz, CLI, and desktop compatibility notes. | Add a firmware-review checklist that records firmware version, qFlipper version, app source, SD-card state, and regression notes. |
| Official application catalog | App catalog changes are useful for provenance, metadata review, app install discipline, and feature-delta tracking. | Add an app-catalog provenance worksheet focused on source, license, permissions, update date, and lab-only status. |
| Community firmware projects | Community firmware is useful for compatibility research and UX comparison, but should not be treated as a default security baseline. | Document feature deltas without copying code or recommending risky actions. |
| IR/NFC/RFID repositories | Community databases are useful for taxonomy, file organization, attribution, and test-fixture documentation. | Add attribution and owned-device-only language. Avoid uncontrolled replay guidance. |
| Wi-Fi, Marauder, jammer, fuzzer, and offensive-adjacent repositories | These projects are high-risk if copied literally. | Extract only detection ideas, lab disclaimers, safety controls, and mitigation-first notes. |
| CISA/NIST/vendor advisory signals | Patch priority is moving toward faster, exposure-based triage. | Carry forward Level 030 patch-SLA logic and apply it to firmware/app review. |

## Why it matters

Flipper Zero projects tend to drift into file dumps, payload collections, and unexplained firmware bundles. That is not a developer-quality repository. CyberFlipper should instead show repeatable engineering process: what changed, what was reviewed, what is safe to run, what requires human approval, what is lab-only, and what evidence proves the review happened.

Level 040 adds that missing layer: provenance, firmware review, application catalog review, USB/HID policy review, and SD-card package discipline.

## Repository update

Added Level 040:

```text
badusb/CyberFlipper_Lab/level_040/README.md
badusb/CyberFlipper_Lab/level_040/cf_l040_windows_hid_policy_review.txt
badusb/CyberFlipper_Lab/level_040/cf_l040_linux_usb_hid_review.txt
badusb/CyberFlipper_Lab/level_040/cf_l040_macos_usb_hid_review.txt
docs/community/CYBERFLIPPER_LEVEL_040_FIRMWARE_APP_REVIEW.md
docs/community/CYBERFLIPPER_SD_CARD_PROVENANCE_CHECKLIST.md
```

## Safety boundary

No credential collection. No token, cookie, password-manager, VPN-secret, browser-profile, message-content, or document collection. No persistence. No hidden execution. No RF transmission instructions. No jamming. No Wi-Fi cracking. No captive credential capture. No exploit chains. No destructive logic.

The BadUSB files launch visible local shells and create `cyberflipper_` reports. Human review is required before publishing or merging.
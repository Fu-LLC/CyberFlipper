# CyberFlipper Level 021 — NFC Parser Boundary and Firmware Provenance Review

## Objective
Build a visible local review worksheet for firmware provenance, parser-boundary controls, rollback readiness, and authorized NFC lab planning. This level does not generate malformed NFC data, emulate tags, transmit radio signals, fuzz a device, or reproduce an exploit.

## Defensive lesson
Variable-length input must be validated against the actual destination capacity before a copy occurs. The official Flipper firmware fix reviewed for this level added bounds checks to MIFARE Ultralight FAST_READ and DESFire file-settings parsing.

Review workflow:
1. Record firmware version and exact commit.
2. Record application/plugin source and API compatibility.
3. Confirm license and attribution requirements.
4. Confirm a known-good rollback image exists.
5. Test only owned lab tags and readers.
6. Record expected rejection behavior for invalid input without creating exploit material.
7. Preserve logs, screenshots, and reviewer sign-off.

## Files
- `cf_l021_windows_nfc_parser_review.txt`
- `cf_l021_linux_nfc_parser_review.txt`
- `cf_l021_macos_nfc_parser_review.txt`
- `VALIDATION_CHECKLIST.md`

Each BadUSB file creates and opens a visible local worksheet whose filename begins with `cyberflipper_`.

## Prohibited activity
No credential access, hidden execution, privilege elevation, persistence, remote transfer, malformed-frame generation, exploit reproduction, NFC credential cloning, unauthorized emulation, RF transmission, Wi-Fi activity, jamming, destructive changes, or third-party targeting.

## Human approval
Do not merge or publish automatically. A maintainer must inspect every keystroke and test it on an isolated, non-production host before release.

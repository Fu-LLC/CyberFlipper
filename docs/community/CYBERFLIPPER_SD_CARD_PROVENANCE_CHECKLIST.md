# CyberFlipper SD Card Provenance Checklist

Use this checklist before packaging or publishing SD-card content.

## Package identity

```text
Package name:
Package date:
Repository branch:
Commit SHA:
Maintainer:
Reviewer:
```

## Required checks

```text
[ ] Package contains only intended folders.
[ ] BadUSB files are plain .txt files.
[ ] BadUSB files visibly create cyberflipper_ output.
[ ] No hidden execution patterns.
[ ] No credential/token/cookie/password-manager/browser-profile collection.
[ ] No persistence, evasion, destructive behavior, or privilege-abuse logic.
[ ] No RF jamming/transmission abuse.
[ ] No Wi-Fi credential cracking or captive credential capture.
[ ] No copied third-party code without license review.
[ ] Attribution notes exist for borrowed structure, ideas, or datasets.
[ ] Firmware/app compatibility notes are documented.
[ ] Human review is complete for shell-launching, inventory, firmware, RF, or NFC/RFID content.
```

## Folder review

| Folder | Review note |
|---|---|
| `badusb/` | Validate visible local-only output and syntax. |
| `infrared/` | Confirm source and remote taxonomy. |
| `nfc/` | Confirm owned-card/test-tag-only framing. |
| `lfrfid/` | Confirm owned-card/test-tag-only framing. |
| `subghz/` | Confirm region-aware and no misuse instructions. |
| `apps/` | Confirm app source, license, compatibility, and update date. |
| `docs/` | Confirm attribution and review status. |
| `hardware/` | Confirm electrical safety, voltage, pinout, and test fixture notes. |
| `web/` | Confirm public-facing content does not overclaim or link to broken paths. |

## Release note template

```text
Date:
Summary:
Files added:
Files changed:
Source inspiration:
Safety review:
Human approval required:
Known limitations:
```

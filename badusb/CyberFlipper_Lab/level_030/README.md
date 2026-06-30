# CyberFlipper Level 030 - Patch SLA and Firmware Review

Level 030 converts current security-research signals into safe developer practice: patch triage, firmware compatibility review, SD-card content review, and detection-oriented worksheets.

## Files

| File | Platform | Purpose |
|---|---|---|
| `cf_l030_windows_patch_sla_worksheet.txt` | Windows | Creates a visible patch-SLA worksheet with local OS, update, Defender, and firewall posture notes. |
| `cf_l030_linux_patch_sla_worksheet.txt` | Linux | Creates a visible patch-SLA worksheet with local kernel, package-manager, service, and socket notes. |
| `cf_l030_macos_patch_sla_worksheet.txt` | macOS | Creates a visible patch-SLA worksheet with local macOS version, update prompt, firewall state, and listener notes. |
| `docs/community/CYBERFLIPPER_LEVEL_030_FIRMWARE_REVIEW.md` | Docs | Developer template for Flipper firmware/app compatibility and SD-card review. |

## Expected output

Each script creates a local report using the `cyberflipper_` prefix. Reports are written to the Desktop or home directory and opened visibly.

## Defensive value

Level 030 teaches the habits behind rapid vulnerability response:

- identify asset and owner context;
- determine whether a device or workstation is exposed;
- document patch status;
- assign remediation urgency;
- verify local security controls;
- avoid undocumented firmware and SD-card drift;
- record what changed before and after a device update.

## Detection notes

Defenders may observe interactive shell launch from keyboard input, creation of `cyberflipper_l030_*` files, local package or update status commands, and local firewall or listener review commands.

These scripts do not transmit reports.

## Review requirements

Human review is required before merge or public release because these scripts launch command interpreters and enumerate local workstation posture.

## Safety boundary

This level is visible, local, non-destructive, and report-only. It is for owned systems, classroom labs, and written-scope assessments.

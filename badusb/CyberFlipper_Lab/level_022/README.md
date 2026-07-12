# CyberFlipper Level 022 — Application Provenance and Permission Review

Level 022 teaches maintainers to review a Flipper application before installation or publication. It is documentation-first and does not exercise radio, Wi-Fi, NFC/RFID, infrared, GPIO, USB attack, or fuzzing capabilities.

## Learning objectives

- Record the exact application version, source repository, commit, and license.
- Identify declared and observed hardware or interface capabilities.
- Check firmware and application API compatibility.
- Document expected files, settings, and user-visible behavior.
- Preserve backup and rollback evidence.
- Require a named reviewer and approval decision.

## Files

- `cf_l022_windows_app_provenance_review.txt`
- `cf_l022_linux_app_provenance_review.txt`
- `cf_l022_macos_app_provenance_review.txt`
- `VALIDATION_CHECKLIST.md`

Each BadUSB file opens a visible local text editor, types a review worksheet, and saves it with the `cyberflipper_` prefix. It does not collect host secrets, enumerate credentials, access networks, elevate privileges, hide activity, or transfer data.

## Expected output

`cyberflipper_l022_app_provenance_review.txt`

## Human review required

Inspect every keystroke and test on an isolated, non-production host before merge. Editor shortcuts vary by operating-system version and desktop environment.
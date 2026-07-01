# CyberFlipper Level 040 — Firmware, App Catalog, and HID Review

Level 040 is a defensive review pack for firmware provenance, app catalog notes, SD-card package review, and visible host-side USB/HID policy worksheets.

## Files

```text
cf_l040_windows_hid_policy_review.txt
cf_l040_linux_usb_hid_review.txt
cf_l040_macos_usb_hid_review.txt
```

## Purpose

- Record firmware and app review context.
- Create visible local `cyberflipper_` worksheet reports.
- Document USB/HID review points for authorized labs.
- Support detection engineering and endpoint-control review.

## Expected reports

```text
cyberflipper_l040_windows_hid_policy_review.txt
cyberflipper_l040_linux_usb_hid_review.txt
cyberflipper_l040_macos_usb_hid_review.txt
```

## Review notes

Run only on owned or administered systems. Record the firmware version, qFlipper version, keyboard layout, test window, approving owner, and observed detections.

Human review is required before public release because these files open local command interpreters and create workstation review notes.

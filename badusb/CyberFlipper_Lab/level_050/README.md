# CyberFlipper Level 050 — Evidence and Provenance

Level 050 adds basic evidence discipline to CyberFlipper labs. The goal is to make every lab run easier to review, reproduce, and hand off.

## Files

| File | Platform | Purpose |
|---|---|---|
| `cf_l050_windows_evidence_manifest.txt` | Windows | Creates a visible evidence handoff worksheet. |
| `cf_l050_linux_evidence_manifest.txt` | Linux | Creates a visible evidence handoff worksheet. |
| `cf_l050_macos_evidence_manifest.txt` | macOS | Creates a visible evidence handoff worksheet. |

## Expected output

Each script creates or displays a visible local worksheet using a `cyberflipper_` prefix where the platform supports file creation:

```text
cyberflipper_l050_windows_evidence_worksheet.txt
cyberflipper_l050_linux_evidence_worksheet.txt
cyberflipper_l050_macos_evidence_worksheet.txt
```

## Defensive value

This level teaches:

- report file tracking
- hash-based file integrity notes
- local lab timestamps
- firmware/version note prompts
- SD-card provenance habits
- clean handoff records for classroom, internal, and client-approved labs

## Review requirements

Human review is required before merge or public release because these scripts launch local command interpreters or visible editor flows.

## Boundary

No credential capture, token extraction, browser-profile access, persistence, stealth, evasion, destructive behavior, privilege abuse, RF transmission, Wi-Fi cracking, captive credential capture, or third-party targeting is included.

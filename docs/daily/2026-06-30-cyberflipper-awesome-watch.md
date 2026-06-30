# CyberFlipper Awesome Watch - 2026-06-30

## Source signals

### Official Flipper firmware

Latest visible upstream firmware release reviewed: `flipperdevices/flipperzero-firmware` release `1.4.3`, dated 2026-12-05 on the GitHub releases page. The release is described as a bugfix release for `1.4.2` and specifically mentions an Infrared CLI plugin MissingImports fix.

The preceding `1.4.2` release notes are more useful for CyberFlipper training content. Relevant defensive/dev signals:

- NFC: new low-level `nfc` CLI command, FeliCa and MIFARE Ultralight C improvements, new parsers, and bug fixes.
- Sub-GHz: nine additional protocols and minor protocol improvements.
- BLE: pairing-security improvements; upstream recommends unpairing and re-pairing because existing pairings may be susceptible to a brute-force issue.
- JavaScript: more GUI bindings for advanced script UI.
- Infrared: additional remotes and autoscroll for long button names.
- BadUSB: key-combo handling and Windows example-script fixes.
- CLI: command handling improvements and a new `buzzer` command.
- LF RFID: country-name display for animal tags.

CyberFlipper action: add a Level 030 developer pack that treats firmware and app changes as compatibility and documentation tasks, not exploitation tasks.

### CISA vulnerability-response signal

Public reporting on CISA's June 2026 federal vulnerability directive says the most critical vulnerabilities must be fixed, disabled, or removed from internet exposure within three calendar days, using risk criteria such as public exposure, KEV presence, automation potential, and access level granted.

CyberFlipper action: add a local-only BadUSB worksheet that creates a `cyberflipper_` patch-SLA report on the host. It does not scan remote networks, exploit anything, or collect secrets. It teaches triage discipline.

### Firmware/IoT research signal

Recent firmware-security research continues to emphasize SBOM-backed vulnerability triage, KEV correlation, EPSS/CVSS context, and reducing unprioritized alert noise.

CyberFlipper action: add `CYBERFLIPPER_LEVEL_030_FIRMWARE_REVIEW.md` as a safe review template for Flipper firmware, app catalog, and SD-card content compatibility.

## Repository additions

```text
badusb/CyberFlipper_Lab/level_030/README.md
badusb/CyberFlipper_Lab/level_030/cf_l030_windows_patch_sla_worksheet.txt
badusb/CyberFlipper_Lab/level_030/cf_l030_linux_patch_sla_worksheet.txt
badusb/CyberFlipper_Lab/level_030/cf_l030_macos_patch_sla_worksheet.txt
docs/community/CYBERFLIPPER_LEVEL_030_FIRMWARE_REVIEW.md
```

## Human review required

This update launches local command interpreters and writes visible workstation reports. It is safe, local-only, and non-destructive, but still requires human review before merge or public release.

## Safety boundary

No credential collection. No tokens. No cookies. No password stores. No persistence. No stealth. No exploitation. No RF transmission abuse. No Wi-Fi cracking. No captive credential capture. No remote callbacks.

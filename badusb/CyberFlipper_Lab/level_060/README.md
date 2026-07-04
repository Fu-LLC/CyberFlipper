# CyberFlipper Level 060 — SD Provenance and SBOM-Lite

Level 060 teaches artifact discipline for Flipper Zero lab packs. The goal is to produce a visible local report showing what is on a CyberFlipper SD-card folder, which files were observed, and which hashes can be used for review.

## Files

| File | Platform | Output |
|---|---|---|
| `cf_l060_windows_sd_provenance.txt` | Windows | Desktop `cyberflipper_l060_windows_sd_provenance.txt` |
| `cf_l060_linux_sd_provenance.txt` | Linux | Home folder `cyberflipper_l060_linux_sd_provenance.txt` |
| `cf_l060_macos_sd_provenance.txt` | macOS | Desktop `cyberflipper_l060_macos_sd_provenance.txt` |

## Expected Use

Use this level after building or copying a CyberFlipper SD-card pack. The scripts are designed to inspect a lab folder or removable media folder, not the whole workstation.

Recommended folder names:

```text
CyberFlipper_SD_CARD
FLIPPER_SD
CYBERFLIPPER
```

## Defensive Value

This level supports:

- SBOM-lite review.
- SD-card release hygiene.
- File-count validation.
- Hash manifests.
- Source attribution checks.
- Public-release review gates.
- Lab handoff discipline.

## Safety Boundary

The scripts create visible local reports only. They do not copy private files, inspect browser profiles, collect credentials, read password stores, alter host policy, write to remote systems, use RF functions, perform Wi-Fi actions, exploit services, or hide execution.

## Review Requirements

Human review is required before merge because these files launch local command interpreters and enumerate files in a selected lab folder.

Review checklist:

```text
[ ] Output file uses cyberflipper_ prefix.
[ ] Script is visible while running.
[ ] Target path is a lab folder or removable SD-card path.
[ ] No home-directory or full-drive recursive scan.
[ ] No credential/token/browser/profile collection.
[ ] No destructive commands.
[ ] No RF, Wi-Fi, exploit, or persistence behavior.
[ ] Report clearly marks this as authorized lab work.
```

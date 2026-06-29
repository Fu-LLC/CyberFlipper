# CyberFlipper

Developer-focused Flipper Zero lab content for SD-card organization, BadUSB training, IR/NFC/RFID notes, GPIO practice, firmware review notes, hardware documentation, and defensive security education.

CyberFlipper is maintained for practical builders: people who want clean file layouts, readable payloads, repeatable tests, and documentation that can survive public review.

## Repository Focus

| Area | Purpose |
|---|---|
| `badusb/` | Flipper-readable `.txt` training scripts with visible local output. |
| `badusb/CyberFlipper_Lab/` | Progressive Level 001-099 curriculum. |
| `docs/daily/` | Daily research notes, source-watch summaries, and release notes. |
| `docs/community/` | Feature matrices, attribution notes, validation checklists, and project positioning. |
| `infrared/` | IR organization and remote-control learning material. |
| `nfc/` | NFC notes for owned cards, lab tags, and test fixtures. |
| `lfrfid/` | 125 kHz RFID notes for owned cards, lab tags, and test fixtures. |
| `subghz/` | Region-aware Sub-GHz documentation and receive-only learning notes. |
| `apps/` | App catalog notes, compatibility notes, and `.fap` organization references. |
| `hardware/` | GPIO, soldering, sensors, badge/SAO planning, fixtures, and bench notes. |
| `web/` | Public project pages, payload catalog views, and developer-facing documentation. |

## BadUSB Training Standard

Every CyberFlipper BadUSB file should be:

- Plain `.txt` compatible with Flipper Zero BadUSB / DuckyScript-style syntax.
- Visible to the user while running.
- Local-output only.
- Named clearly by level and platform.
- Written to create a `cyberflipper_` report file.
- Documented in the level README.
- Reviewed before merge when it launches PowerShell, Terminal, shell, reads logs, lists services, or inventories installed software.

Preferred commands:

```text
REM
DELAY
STRING
ENTER
GUI
CTRL
ALT
SHIFT
TAB
ESC
```

Avoid fragile behavior: hidden windows, keyboard-layout-specific symbols where possible, hard-coded focus assumptions, silent execution, destructive commands, remote callbacks, credential stores, browser profile data, cookies, tokens, passwords, persistence, evasion, or privilege-abuse logic.

## Level Ladder

| Level | Theme | Developer output |
|---|---|---|
| 001 | Host identity | Basic host and OS reports. |
| 002 | Application inventory | Browser, office, PDF, and desktop tool reports. |
| 003 | Event and app review | Local event summaries and installed app views. |
| 004 | Business stack review | Productivity, design, backup, sync, and security tooling visibility. |
| 005 | Communications workflow | Chat, meeting, project, and knowledge-base software reports. |
| 006 | Community desktop review | Cross-platform desktop review suite. |
| 020 | IR and detection engineering | Triage reports, event counts, patch posture, backup posture, report manifests. |
| 030 | Benign validation | Detection-rule worksheets and tabletop emulation notes. |
| 040 | Firmware and app review | Firmware/API changes, app catalog notes, compatibility tables. |
| 060 | Evidence discipline | Hash manifests, metadata discipline, chain-of-custody notes. |
| 080 | Sandboxed research | Local-only lab harnesses and mitigation-first analysis templates. |
| 099 | Capstone | Full authorized lab workflow with docs, scripts, detection notes, and review checklist. |

## Current Community Pack

Level 006 and Level 020 are the active developer baselines.

Recommended files to review first:

```text
badusb/CyberFlipper_Lab/level_006/README.md
badusb/CyberFlipper_Lab/level_006/cf_l006_windows_desktop_stack_audit.txt
badusb/CyberFlipper_Lab/level_006/cf_l006_windows_browser_review.txt
badusb/CyberFlipper_Lab/level_006/cf_l006_linux_desktop_stack_audit.txt
badusb/CyberFlipper_Lab/level_006/cf_l006_macos_workstation_review.txt
badusb/CyberFlipper_Lab/level_020/README.md
docs/community/CYBERFLIPPER_COMMUNITY_FEATURE_MATRIX.md
docs/community/CYBERFLIPPER_SOURCE_ATTRIBUTION_2026-06-29.md
```

## SD Card Layout

CyberFlipper is distributed as SD-card content. No firmware flash is required for the training files.

Expected root layout:

```text
badusb/
infrared/
nfc/
lfrfid/
subghz/
apps/
dolphin/
u2f/
docs/
hardware/
web/
```

Install flow:

1. Copy the needed folders to the Flipper Zero SD card.
2. Keep level folders intact.
3. Run BadUSB files from the Flipper interface.
4. Confirm the host output file appears visibly.
5. Record firmware version, host OS, keyboard layout, and test result.

## Source Watch

CyberFlipper tracks ideas from the Flipper Zero ecosystem and converts them into safe documentation, lab validation, detection engineering, and defensive training.

Watched repositories and topics:

| Source | What to extract |
|---|---|
| `flipperdevices/flipperzero-firmware` | Official firmware behavior, APIs, hardware notes, compatibility changes. |
| `flipperdevices/flipper-application-catalog` | App metadata, app packaging patterns, catalog changes. |
| `DarkFlippers/unleashed-firmware` | Community firmware changes, app compatibility, feature deltas. |
| `Flipper-XFW/Xtreme-Firmware` | Community UX patterns, firmware feature organization, compatibility notes. |
| `RogueMaster/flipperzero-firmware-wPlugins` | Plugin ecosystem tracking and app compatibility notes. |
| `UberGuidoZ/Flipper` | Community file organization, payload structure, examples, and SD-card layout ideas. |
| `UberGuidoZ/Flipper-IRDB` | IR database organization and attribution patterns. |
| `Lucaslhm/Flipper-IRDB` | IR collection structure and remote taxonomy. |
| `RocketGod-git/ProtoPirate` | Project presentation, feature matrices, protocol documentation style. |
| `SkeletonMan03/FZEasyMarauderFlash` | Flashing workflow documentation and setup UX ideas. |
| `kbembedded/Flipper-Zero-Game-Boy-Pokemon-Trading` | GPIO timing, retro hardware documentation, test fixtures. |
| `DarkFlippers/Multi_Fuzzer` | Reader testing safety notes and lab-only validation patterns. |
| `DarkFlippers/qFlipper` | Desktop update workflow and device-management notes. |

Risky topics such as jamming, Wi-Fi attack tooling, fuzzing, cracking, and offensive firmware are only used for safety notes, defensive detection ideas, lawful lab disclaimers, and mitigation-first documentation.

## Developer Validation Checklist

Before opening or merging a PR:

```text
[ ] File names are clear and platform-specific.
[ ] BadUSB files are plain text.
[ ] Commands stay visible.
[ ] Output file uses cyberflipper_ prefix.
[ ] No credential, token, cookie, browser-profile, password-manager, VPN-secret, or message-content collection.
[ ] No persistence, stealth, evasion, destructive behavior, or privilege-abuse chain.
[ ] No RF transmission abuse, jamming, Wi-Fi cracking, or captive credential capture.
[ ] Level README exists.
[ ] Daily/source notes exist when the update came from ecosystem research.
[ ] Human review is requested for shell-launching or host-inventory scripts.
```

## Contribution Format

Use this structure for new levels:

```text
badusb/CyberFlipper_Lab/level_0XX/
  README.md
  cf_l0XX_windows_<topic>.txt
  cf_l0XX_linux_<topic>.txt
  cf_l0XX_macos_<topic>.txt

docs/daily/YYYY-MM-DD-cyberflipper-level-0XX.md
```

README sections per level:

```text
Purpose
Files
Expected output
Defensive value
Detection notes
Review requirements
```

## Project Boundary

CyberFlipper is for owned hardware, classroom labs, CTF ranges, client-approved scopes, defensive education, and public-safe developer documentation.

Do not use this repository for unauthorized access, credential capture, stealth, persistence, evasion, reverse shells, destructive actions, exploitation against third-party systems, RF abuse, or network abuse.

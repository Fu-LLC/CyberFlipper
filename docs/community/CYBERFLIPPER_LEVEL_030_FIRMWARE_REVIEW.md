# CyberFlipper Level 030 - Firmware and App Compatibility Review

## Purpose

Use this template when upstream Flipper firmware, qFlipper, app-catalog, community firmware, or SD-card content changes. The goal is to preserve compatibility and user safety without copying upstream code blindly.

## Review inputs

| Input | Review question |
|---|---|
| Firmware release notes | What changed in NFC, RFID, IR, Sub-GHz, BLE, JS, CLI, BadUSB, storage, or GUI behavior? |
| App catalog changes | Which apps changed packaging, dependencies, permissions, or user workflow? |
| Community firmware notes | Which UX patterns or compatibility warnings are useful as documentation only? |
| SD-card content | Do folders still map cleanly to Flipper expectations? |
| BadUSB examples | Do scripts still run visibly and create local `cyberflipper_` output? |
| Hardware notes | Are GPIO voltage, pinout, module, or enclosure assumptions still accurate? |

## Compatibility matrix

| Component | Current observed behavior | CyberFlipper impact | Action |
|---|---|---|---|
| BadUSB key combos | Review against current firmware notes | May affect Windows/macOS/Linux scripts | Retest level scripts. |
| NFC CLI and parsers | Review official release notes | Improves lab-card documentation | Update NFC owned-card notes. |
| BLE pairing | Review pairing-security notes | Affects device-management guidance | Recommend re-pair validation after firmware update. |
| Infrared database behavior | Review IR CLI and remote changes | Affects IR folder examples | Add attribution and taxonomy notes. |
| Sub-GHz protocol handling | Review protocol additions/fixes | Documentation only unless legally scoped | Keep receive-only and region-aware guidance. |
| JS GUI bindings | Review developer examples | Useful for safe UI demos | Add local-only UI examples later. |
| qFlipper workflow | Review desktop update behavior | Affects install docs | Keep SD-card copy instructions current. |

## Patch-SLA mapping

Use these categories for documentation and tabletop exercises:

| Criteria | Fast response indicator |
|---|---|
| Public exposure | Internet-facing, radio-facing, or unauthenticated local adjacency. |
| Known exploitation | Listed in KEV or confirmed by vendor/advisory. |
| Automation potential | Simple, repeatable, widely scriptable exploit conditions. |
| Access impact | Privilege increase, control-plane access, sensitive data exposure, or broad fleet impact. |

## Output requirement

Every Level 030 update should include:

```text
Source reviewed:
Date reviewed:
Firmware/app version:
Affected CyberFlipper folders:
Compatibility risk:
Safety risk:
Recommended repository update:
Human review required: yes/no
```

## Attribution notes

Credit upstream project names and maintainers in documentation. Do not copy source code or payloads without checking license compatibility and preserving attribution.

## Safety notes

Risky topics are converted only into defensive documentation, lab validation, detection notes, and mitigation guidance. CyberFlipper content should not include exploit chains, secret collection, stealth, persistence, destructive behavior, unauthorized radio activity, or third-party testing instructions.

# CyberFlipper Level 060 — SBOM and Provenance Notes

## Purpose

Level 060 turns CyberFlipper from a folder dump into a reviewable lab artifact. The goal is a lightweight provenance trail that a developer, instructor, reviewer, or client can read quickly.

## Minimum Provenance Fields

| Field | Meaning |
|---|---|
| Asset path | Exact repository or SD-card path. |
| Asset type | BadUSB, IR, NFC, RFID, Sub-GHz, app, doc, hardware note, image, data file. |
| Source | Original author, project, vendor, or original CyberFlipper. |
| Source URL | Stable source URL when applicable. |
| License | Known license, unknown, or internal-only. |
| Review status | Draft, reviewed, lab-only, public-safe, blocked. |
| Risk class | Low, medium, high, restricted. |
| Notes | Compatibility, attribution, safety caveat, or reviewer note. |

## Risk Classes

| Class | Examples | Handling |
|---|---|---|
| Low | README text, checklists, wiring notes, harmless diagrams. | Normal PR review. |
| Medium | Visible host review scripts, firmware compatibility notes, local logs. | Human review required. |
| High | HID automation, NFC/RFID examples, Sub-GHz notes, GPIO interaction. | Draft PR, reviewer approval, no auto-merge. |
| Restricted | Credential handling, hidden behavior, destructive behavior, radio interference, unauthorized access workflows. | Do not include. |

## Safe Conversion Pattern

When reviewing high-risk community material, convert it into:

- A safety note.
- A detection idea.
- A lab-only disclaimer.
- A mitigation checklist.
- A compatibility warning.
- A provenance entry.
- A public-safe worksheet.

Do not copy material that enables unauthorized activity or harmful behavior.

## Suggested `docs/community` Artifacts

```text
CYBERFLIPPER_SOURCE_ATTRIBUTION_YYYY-MM-DD.md
CYBERFLIPPER_COMMUNITY_FEATURE_MATRIX.md
CYBERFLIPPER_SD_CARD_PROVENANCE_CHECKLIST.md
CYBERFLIPPER_BADUSB_LINT_CHECKLIST.md
CYBERFLIPPER_RELEASE_REVIEW.md
```

## Release Gate

Before release:

```text
[ ] Every imported asset has a source note.
[ ] Every copied community asset has license review.
[ ] Every BadUSB script uses visible output and cyberflipper_ prefix.
[ ] Every RF/Sub-GHz/NFC/RFID item has a lawful-use note.
[ ] Restricted material is absent.
[ ] High-risk content is draft-only until reviewed.
[ ] A reviewer can reproduce the folder layout from the README.
```

# CyberFlipper Level 070 — Change Intake Guide

Level 070 defines how CyberFlipper reviews external firmware, app, payload, RF, NFC/RFID, Wi-Fi, fuzzing, BadUSB, and hardware-adjacent sources before they become repository content.

## Intake Fields

| Field | Required note |
|---|---|
| Source | Repository, advisory, release note, vendor doc, or creator writeup. |
| Date | Publication date, release date, commit date, or review date. |
| Version | Firmware version, app version, tag, branch, commit, or file revision. |
| License | License name and whether reuse is allowed. |
| Attribution | Required credit and source-note wording. |
| Risk domain | Firmware, BadUSB, NFC, LFRFID, Sub-GHz, IR, GPIO, Wi-Fi, BLE, fuzzer, jammer, app catalog, CI, docs. |
| Safe conversion | Documentation, checklist, matrix, worksheet, detection note, mitigation note, lab-only validation. |
| Blocked content | Anything involving unsafe access, secret collection, stealth behavior, unauthorized target interaction, RF misuse, or destructive behavior. |
| Human review | Required for shell-launching, host inventory, firmware flashing, RF, Wi-Fi, fuzzing, and security-sensitive changes. |

## Review Workflow

1. Read the upstream release note or commit summary.
2. Record the exact source and date.
3. Do not copy code blindly.
4. Check license and attribution requirements.
5. Classify the risk domain.
6. Decide what safe CyberFlipper artifact should be created.
7. Convert high-risk functionality into defender guidance, lab disclaimers, detection logic, or mitigation notes.
8. Keep shell-launching, host-inventory, RF, Wi-Fi, firmware, and fuzzing material in draft PRs.

## Useful Signals From Current Source Watch

| Signal | Safe CyberFlipper response |
|---|---|
| Firmware bugfix release | Add firmware review note and compatibility reminder. |
| NFC parser or CLI change | Add owned-tag lab worksheet and parser-risk note. |
| LFRFID write/wipe UX change | Add lab-only warning and tag-safety checklist. |
| Sub-GHz protocol or RAW behavior change | Add region/legal warning and receive-only lab guidance. |
| BLE pairing security note | Add re-pairing and device hygiene checklist. |
| BadUSB key handling change | Add payload lint checklist and keyboard-layout test case. |
| App catalog bump | Add app provenance and compatibility matrix entry. |
| CI/API-change report | Add developer validation workflow note. |

## Merge Gate

A Level 070 artifact can move from draft to ready only when:

```text
[ ] Source is named.
[ ] Date/version/commit is recorded.
[ ] License/attribution is documented.
[ ] Artifact is defensive or educational.
[ ] No prohibited behavior is present.
[ ] Human review requirement is explicitly stated.
[ ] Output behavior is visible and local.
```

## Repository Use

Place source-watch outputs in `docs/daily/` and stable conversion rules in `docs/community/`. Place DuckyScript-style worksheet generators in `badusb/CyberFlipper_Lab/level_070/`.

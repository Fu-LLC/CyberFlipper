# CyberFlipper Level 090 — Evidence Communications Guide

## Intent

Level 090 exists to make CyberFlipper lab work explainable to another human. A safe lab run should leave behind a short record that answers five questions:

1. Was this authorized?
2. What was tested?
3. What files or device versions were involved?
4. What happened visibly on the host?
5. Who reviewed the result before release?

## Defensive model

CyberFlipper content should treat every Flipper, SD card, app bundle, IR set, NFC/RFID lab note, USB/HID script, and hardware build as an artifact with provenance. That does not require a heavyweight forensic process. It requires consistent naming, a clear source note, and a human review gate.

## Required worksheet fields

| Field | Why it exists |
|---|---|
| Operator | Identifies who performed the authorized lab run. |
| Lab or classroom | Identifies the environment and helps avoid scope confusion. |
| Date | Supports version comparison and release notes. |
| Host | Documents the approved test machine without collecting private material. |
| Operating system | Helps reproduce keyboard and application behavior. |
| Keyboard layout | BadUSB-style scripts are layout-sensitive. |
| Flipper firmware | Firmware behavior changes over time. |
| CyberFlipper source | SD-card content should trace back to a release, branch, or commit. |
| Authorization statement | Keeps the exercise inside owned, administered, or written-scope systems. |
| Test scope | Defines what was intentionally reviewed. |
| Files reviewed | Names the local training files or documentation. |
| Expected result | Prevents vague success claims. |
| Actual result | Records visible behavior. |
| Artifacts created | Names outputs using the `cyberflipper_` prefix. |
| Follow-up owner | Assigns the next action. |
| Release decision | Publish, hold, revise, or reject. |
| Reviewer signoff | Confirms human review happened. |

## Safe content conversion rules

Use public ecosystem changes as prompts for defensive documentation only:

- Firmware changes become compatibility notes.
- App catalog changes become install and provenance checks.
- IR collections become attribution and lawful-use notes.
- NFC/RFID examples become owned-card lab documentation.
- USB/HID examples become visible local worksheets.
- Hardware projects become wiring, power, isolation, and safety notes.
- High-risk community ideas become tabletop questions, not executable replicas.

## Release decision language

Recommended decision values:

```text
PUBLISH: benign documentation only; no host-side execution.
DRAFT: needs more source notes or screenshots.
HOLD: launches host tools or needs security review.
REVISE: unclear language, missing scope, or poor provenance.
REJECT: unsafe behavior, missing authorization, or prohibited content.
```

## Human review gate

Level 090 does not remove the need for review. It provides a repeatable record so the reviewer can decide quickly. Any file that launches a shell, editor, terminal, local inventory command, USB/HID action, firmware change, radio workflow, or security-sensitive test should remain draft-only until reviewed.

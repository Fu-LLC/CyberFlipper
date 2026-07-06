# CyberFlipper Awesome Watch — 2026-07-06

## Executive summary

Today's safe lab pack adds Level 090: portable evidence review and communications tabletop. The pack turns current Flipper ecosystem signals into defensive process content: evidence handling, device provenance, USB/HID awareness, firmware/app review, lab-safety boundaries, and incident communications discipline.

No third-party code is copied. Community projects are used only as prompts for safer documentation, lawful lab scoping, compatibility notes, and mitigation-first review habits.

## Source signals reviewed

| Source family | Signal | CyberFlipper conversion |
|---|---|---|
| Official Flipper firmware | Recent stable notes emphasize bug fixes, NFC parser and CLI behavior, Sub-GHz handling, BLE pairing security, HID/BadUSB fixes, CLI polish, iButton, and LF RFID changes. | Add a firmware-review habit: document device firmware, app version, test date, keyboard layout, and expected output before running any lab file. |
| Flipper app catalog | App metadata and packaging patterns matter for compatibility and reproducible installs. | Treat every app or payload folder as an SD-card artifact with source, version, checksum, and test notes. |
| Community firmware | Fast-moving community builds expose useful UX, app, plugin, and compatibility patterns. | Extract compatibility deltas and safety controls only. |
| IR learning collections | Large signal libraries create attribution and provenance challenges. | Keep source notes, device model notes, and lawful-use boundaries with IR training content. |
| Hardware and reader-test examples | Lab hardware projects show why scope, consent, isolation, and repeatable notes matter. | Convert into tabletop checklists: scope, authorization, safe channels, lab isolation, and response questions. |
| qFlipper and official docs | qFlipper and SD-card workflows remain the safest install pattern for public training packs. | Preserve SD-card-only distribution for training scripts. |
| CISA/NIST/vendor advisories | Exploitation evidence and risk governance reinforce prioritizing exposure, observed abuse, and recovery planning. | Add communications and evidence worksheets tied to patch, firmware, and provenance decisions. |

## Why it matters

CyberFlipper already has Levels 030-080 covering patch SLAs, firmware/app review, evidence provenance, SD-card provenance, source conversion, and sandbox readiness. Level 090 is the next step: turn technical findings into an operator-readable handoff package.

The practical failure mode in public security-lab content is not only technical misuse. It is poor evidence handling, unclear authorization, missing provenance, and sloppy communication. Level 090 addresses that gap with visible local worksheets that force the user to document scope, artifacts, observed behavior, decisions, and follow-up owners.

## Files added

```text
badusb/CyberFlipper_Lab/level_090/README.md
badusb/CyberFlipper_Lab/level_090/cf_l090_windows_evidence_comms_tabletop.txt
badusb/CyberFlipper_Lab/level_090/cf_l090_linux_evidence_comms_tabletop.txt
badusb/CyberFlipper_Lab/level_090/cf_l090_macos_evidence_comms_tabletop.txt
docs/community/CYBERFLIPPER_LEVEL_090_EVIDENCE_COMMS.md
docs/community/CYBERFLIPPER_RELEASE_REVIEW_GATE.md
```

## Human review gate

Do not auto-merge this pack without human review. The BadUSB files open visible local shells or editors and create `cyberflipper_` workstation/tabletop notes. They are benign and local-output only, but they still launch host-side tools and are security-sensitive training artifacts.

## Explicit non-goals

This update does not add secrets collection, hidden activity, destructive commands, privilege-abuse logic, radio misuse, network misuse, malware behavior, or unauthorized testing instructions.

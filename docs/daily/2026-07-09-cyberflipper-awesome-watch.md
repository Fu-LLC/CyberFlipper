# CyberFlipper Awesome Watch - 2026-07-09

This update expands CyberFlipper past Level 010 and starts the continuous Level 011-019 workstation-basics track. The goal is to make levels 011-099 real, reviewable, and useful instead of leaving a gap between milestone packs.

## Source Review

Reviewed source categories:

- Official Flipper Zero documentation for firmware updates, BadUSB behavior, apps, and Sub-GHz behavior.
- Official firmware and application-catalog direction from Flipper Devices.
- Community firmware and app examples from Unleashed, Xtreme, RogueMaster, UberGuidoZ, IRDB projects, qFlipper, Multi_Fuzzer, ProtoPirate, Marauder-flashing examples, and related repos.
- Defensive prioritization sources such as CISA KEV, CISA advisories, vendor advisories, NIST-style inventory/control practices, and SBOM/firmware triage concepts.

## Defensive Takeaways

1. Firmware update workflow needs to be taught as a security-control habit. Stable release firmware is the default recommendation for ordinary users; dev and RC builds belong in lab validation.
2. BadUSB material must be visibly local, authorized, and non-stealth. CyberFlipper scripts should create `cyberflipper_` artifacts and tell the operator what happened.
3. RF/Sub-GHz material must stay region-aware and mitigation-first. The curriculum should cover inventory, policy, detection, and lawful testing boundaries before any signal work.
4. App-catalog and custom-firmware examples are useful for provenance, SD-card hygiene, release review, and extension risk—not blind copying.
5. Risky community ideas such as jammer, fuzzing, Marauder, and cracking projects are converted only into safety lessons, lab disclaimers, and defensive detection notes.

## Repo Changes in This Pack

- Adds a Level 011-099 expansion map.
- Adds Level 011-019 README files.
- Adds a source-attribution matrix for community inspiration.
- Adds a human-review gate for host-facing, RF, firmware, and HID automation.
- Defers executable BadUSB host-inventory scripts until a reviewer approves the exact commands and visible-output behavior.

## Level 011-019 Track

| Level | Name | Safe output target |
|---|---|---|
| 011 | Operator Scope Card | Authorization and test-window worksheet. |
| 012 | Host Context | OS, user, owner, and environment worksheet. |
| 013 | Network Context | Interfaces, routes, DNS, and listener review worksheet. |
| 014 | Browser Posture | Browser inventory and update worksheet. |
| 015 | Productivity Stack | Office/PDF/design/BI software inventory worksheet. |
| 016 | Communications Stack | Email/chat/meeting/collaboration app inventory worksheet. |
| 017 | Backup and Sync | Backup target, sync scope, and recovery-check worksheet. |
| 018 | Security Posture | Firewall, endpoint protection, encryption, and update worksheet. |
| 019 | Evidence Manifest | Hashes, artifact list, reviewer notes, and completion checklist. |

## Human Review Required

Do not merge shell-launching, host-inventory, radio/RF, firmware-flashing, fuzzing, or HID automation without manual review. Defensive documentation can merge first; executable scripts remain draft-only.

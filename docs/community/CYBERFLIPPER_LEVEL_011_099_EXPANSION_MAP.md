# CyberFlipper Level 011-099 Expansion Map

CyberFlipper now fills the gap after Level 010 with continuous, reviewable levels. Each level must teach one defensive skill, produce a visible `cyberflipper_` artifact, and include human-review notes when it touches host inventory, HID automation, RF, firmware, fuzzing, or security-sensitive behavior.

## Level Bands

| Band | Theme | Deliverables |
|---|---|---|
| 011-019 | Workstation basics | Scope, host context, network context, browser posture, software stack, collaboration apps, backups, security posture, evidence manifest. |
| 020-029 | Incident-response triage | Process review, listener review, persistence review checklist, event counts, patch posture, triage report, handoff notes. |
| 030-039 | Patch and exposure | Patch SLA worksheets, vendor-watch cards, KEV matching, EPSS/CVSS notes, remediation planning. |
| 040-049 | Firmware and app review | Firmware channel review, qFlipper update checklist, app-catalog provenance, SD-card hygiene, custom-firmware warning labels. |
| 050-059 | Evidence discipline | Hash manifests, collection notes, screenshot log, chain-of-custody worksheet, reviewer signoff. |
| 060-069 | SBOM and provenance | SD-card inventory, file manifests, source attribution, license notes, release review, lint rules. |
| 070-079 | Change intake | Community-source conversion, risky-source review, safe rewrite checklist, issue templates, release gates. |
| 080-089 | Sandbox readiness | Lab VM checklist, safe sample-handling worksheet, network isolation notes, rollback plan, mitigation rehearsal. |
| 090-099 | Capstone release | Public release notes, validation evidence, maintainer review, final safety screen, publish checklist. |

## Level 011-019 Immediate Implementation

| Level | Name | Defensive purpose | Output artifact |
|---|---|---|---|
| 011 | Operator Scope Card | Establish written authorization and test window. | `cyberflipper_l011_scope_card` |
| 012 | Host Context | Record system ownership and baseline environment. | `cyberflipper_l012_host_context` |
| 013 | Network Context | Review interfaces, DNS, routes, and listening services defensively. | `cyberflipper_l013_network_context` |
| 014 | Browser Posture | Document browsers, update status, extensions, and policy notes. | `cyberflipper_l014_browser_posture` |
| 015 | Productivity Stack | Inventory office, PDF, design, video, and BI apps. | `cyberflipper_l015_productivity_stack` |
| 016 | Communications Stack | Inventory email, chat, meeting, and collaboration apps. | `cyberflipper_l016_comms_stack` |
| 017 | Backup and Sync | Confirm backup target, sync scope, and recovery-test status. | `cyberflipper_l017_backup_sync` |
| 018 | Security Posture | Review firewall, endpoint protection, encryption, updates, and account policy. | `cyberflipper_l018_security_posture` |
| 019 | Evidence Manifest | Record files, hashes, notes, and review outcome. | `cyberflipper_l019_evidence_manifest` |

## Conversion Rule for Risky Sources

Risky community repos are not copied. They are converted as follows:

- Jammer or RF-transmission projects become RF policy, lawful-use, and interference-response worksheets.
- Marauder or Wi-Fi tooling becomes wireless asset inventory, detection, and defensive hardening notes.
- Fuzzers become sandbox setup, crash-reporting, responsible disclosure, and test-scope notes.
- Firmware forks become release-channel comparison, provenance, rollback, and SD-card hygiene checklists.
- BadUSB examples become visible local worksheets only; no credential access, stealth, persistence, or destructive actions.

## Review Gate

A level can merge as documentation-only when it is descriptive. A level must remain draft when it includes host commands, HID scripts, RF operations, firmware flashing, fuzzing, exploit simulation, or detection logic that could affect a third-party system.

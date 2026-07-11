# CyberFlipper Level 099 Capstone Release Gate

## Objective

Level 099 turns a CyberFlipper lab pack into a reviewable release candidate. The output is not an exploit result. The output is a release decision supported by authorization, provenance, evidence handling, detection value, and human review.

## Required release questions

```text
1. What system, device, or SD-card pack was tested?
2. Who authorized the work?
3. What was explicitly out of scope?
4. What firmware, app catalog, and community sources were reviewed?
5. What files changed?
6. What visible local output is created?
7. What defensive value does the content provide?
8. What detection, mitigation, or recovery note was added?
9. What source attribution and license notes are required?
10. Is the release publishable, internal-only, or blocked?
```

## Safe conversion rules

Risky community projects can be useful without copying their behavior. Convert them this way:

| Source type | Do not copy | Safe CyberFlipper conversion |
|---|---|---|
| Wi-Fi attack tooling | attack flows or credential capture | lab disclaimers, consent banners, detection checklists, RF safety notes |
| RF jammer projects | jamming behavior | lawful spectrum notes, incident-report templates, device-identification worksheets |
| Fuzzers | live target stress behavior | offline parser review, crash-report taxonomy, safe sample handling |
| Offensive firmware forks | bypass logic or unsafe defaults | compatibility matrices, API-change notes, review gates |
| BadUSB payload dumps | sensitive collection or stealth | visible worksheets, inventory reports, tabletop notes |
| NFC/RFID attack examples | unauthorized access replication | owned-card documentation, reader safety, access-control policy review |

## Release states

Use one of these states in every Level 099 worksheet:

```text
PUBLISHABLE
- Safe for public repository content after reviewer signoff.

INTERNAL ONLY
- Useful for authorized lab use but not appropriate for public distribution.

BLOCKED
- Requires rewrite, attribution fix, legal review, safety review, or removal.
```

## Reviewer checklist

```text
[ ] Scope is documented.
[ ] Authorization is documented.
[ ] Output file name starts with cyberflipper_ .
[ ] Commands are visible.
[ ] No sensitive data collection.
[ ] No hidden execution.
[ ] No destructive command.
[ ] No persistence or evasion logic.
[ ] No RF or network misuse.
[ ] Sources are attributed.
[ ] License concerns are noted.
[ ] Detection or mitigation value is clear.
[ ] Release state is explicit.
```

## Mapping to defensive frameworks

CyberFlipper Level 099 maps cleanly to common defensive language:

- Govern: authorization, scope, policy, reviewer signoff.
- Identify: inventory, provenance, firmware and SD-card versioning.
- Protect: safe defaults, bounded labs, no sensitive collection.
- Detect: local worksheet fields for observable behavior and logging ideas.
- Respond: communications handoff and review notes.
- Recover: rollback, known limitations, release blocking when unsafe.

## Merge policy

Do not auto-merge Level 099 changes. A human reviewer should inspect every file because the pack includes platform-specific files that open local tools and create host-side worksheets.

# CyberFlipper Level 010 Authorization Standard

## Scope

Level 010 is the minimum authorization standard for CyberFlipper lab work. It applies before running or publishing content related to:

- BadUSB
- GPIO
- Firmware or app catalog review
- RF, Sub-GHz, IR, NFC, LF RFID, or iButton notes
- Marauder, fuzzer, jammer, or offensive-adjacent community repositories
- CVE, 0day, red-team, tabletop, or detection-engineering content

## Required authorization fields

```text
Operator:
Organization / lab:
Approver:
Date:
Host or device in scope:
Physical location:
Flipper firmware version:
SD-card source:
Repository branch:
Public source reviewed:
Allowed activity:
Explicit exclusions:
Output file:
Reviewer:
```

## Required safety controls

```text
[ ] Owned or administered host/device only.
[ ] Written authorization recorded.
[ ] Visible local output only.
[ ] cyberflipper_ output prefix.
[ ] No credential, token, cookie, browser-profile, password-manager, or private-secret collection.
[ ] No hidden execution.
[ ] No persistence.
[ ] No evasion.
[ ] No destructive command.
[ ] No RF jamming or unauthorized transmission.
[ ] No Wi-Fi cracking or captive credential capture.
[ ] No testing against third-party systems, cards, devices, RF environments, or networks.
[ ] Source attribution recorded.
[ ] Human review required for shell-launching, host-inventory, RF, firmware, or security-sensitive updates.
```

## Conversion rule

CyberFlipper may read offensive-adjacent public projects for learning, but repository content must convert them into one of these safe forms:

```text
Safety disclaimer
Defensive detection note
Mitigation checklist
Lab-only interoperability note
Configuration review worksheet
Release-review gate
Attribution note
Hardware safety note
Firmware/app provenance note
```

Do not copy exploit logic, credential logic, stealth behavior, RF abuse logic, destructive behavior, or unauthorized access workflows.

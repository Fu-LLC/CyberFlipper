# CyberFlipper Risky Source Conversion Rules

CyberFlipper may watch offensive or dual-use sources, but public repository updates must convert them into defensive education.

## Conversion Table

| Risky source type | Do not copy | Safe conversion |
|---|---|---|
| RF jammer | Transmission logic, frequencies, timing, build instructions | Legal warning, detection note, interference-response checklist, lab disclaimer. |
| Marauder/Wi-Fi tooling | Deauth, cracking, capture, captive credential capture | Wi-Fi policy review, authorized lab scoping, detection and network-hardening checklist. |
| Fuzzer | Crash-triggering target logic or abuse steps | Parser robustness checklist, input-validation notes, safe test harness documentation. |
| BadUSB offensive payload | Credential access, stealth, persistence, bypass, exploitation | Visible local report generator, tabletop worksheet, defensive audit checklist. |
| NFC/RFID write/wipe | Unauthorized cloning, write/wipe steps for real systems | Owned-tag-only lab worksheet, tag handling safety note, access-control policy review. |
| Sub-GHz replay/protocol abuse | Replay, brute force, region-unsafe transmission | Receive-only documentation, region/legal note, defensive receiver/log review. |
| Firmware flashing | Unreviewed flashing steps or risky binary guidance | Version provenance, checksum, backup, compatibility, and rollback checklist. |
| 0day-themed content | Exploit chain, weaponization, bypass, privilege abuse | Disclosure timeline worksheet, patch-SLA note, detection engineering checklist. |

## Required Language For Risky Topics

Use direct wording:

```text
This material is for owned hardware, classroom labs, CTF ranges, and client-approved scopes only.
This file does not provide exploit, credential, stealth, persistence, jamming, cracking, or bypass instructions.
This source is used only for safety lessons, detection engineering, mitigation planning, and lab validation.
```

## Block List

Do not include:

```text
credential theft
password extraction
token extraction
cookie extraction
browser-profile collection
password-manager access
persistence
stealth
evasion
AV bypass
privileged service abuse
shadow-copy abuse
reparse-point abuse
system-path overwrite logic
reverse shells
malware behavior
RF jamming
unauthorized RF transmission
Wi-Fi cracking
captive credential capture
unauthorized exploitation steps
third-party compromise logic
```

## Preferred Outputs

```text
README improvements
source attribution notes
feature matrices
risk registers
detection checklists
mitigation checklists
patch-SLA worksheets
firmware provenance notes
SBOM-lite templates
visible local BadUSB reports
benign tabletop simulation notes
```

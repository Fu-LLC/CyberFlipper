# CyberFlipper Level 050 — Evidence Provenance Guide

## Purpose

Level 050 defines how CyberFlipper labs should record provenance before results are published, graded, or handed off.

This is developer process documentation, not an exploit workflow.

## Provenance fields

| Field | Why it matters |
|---|---|
| Date and operator | Establishes who ran the lab and when. |
| Reviewer | Establishes who approved the result. |
| Scope | Prevents lab results from being confused with unauthorized testing. |
| Flipper firmware version | Connects behavior to firmware state. |
| SD-card source | Tracks where payloads and assets came from. |
| Keyboard layout | Explains BadUSB reliability issues. |
| Host OS | Explains platform-specific behavior. |
| Report filenames | Makes handoff review easier. |
| SHA256 hashes | Lets reviewers confirm file integrity when needed. |

## SD-card provenance checklist

```text
[ ] SD-card source identified
[ ] Firmware version recorded
[ ] App/catalog source recorded
[ ] BadUSB level folder preserved
[ ] Report files use cyberflipper_ prefix
[ ] No private user content included
[ ] No credential/token/cookie/browser profile material included
[ ] Hashes recorded when reports are attached
[ ] Human approval captured before merge or release
```

## Safe source handling

Community projects can inspire organization and defensive documentation, but CyberFlipper should not blindly copy payloads or firmware changes.

Allowed extraction:

- README structure
- compatibility matrix format
- app taxonomy
- firmware-version note style
- safety disclaimers
- defensive detection notes
- classroom lab framing
- report templates
- attribution patterns

Not allowed:

- credential capture
- token extraction
- persistence
- stealth
- evasion
- destructive payloads
- unauthorized exploitation
- RF jamming or transmission abuse
- Wi-Fi cracking
- captive credential capture
- malware behavior

## Human approval gate

Require review before publishing or merging any file that launches a shell, opens PowerShell or Terminal, inventories host state, touches device policy, reviews firmware/app state, handles RF/radio topics, or could be misread as operational offensive guidance.

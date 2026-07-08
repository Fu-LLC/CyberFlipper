# CyberFlipper Level 010 — Authorized-Use Kickoff

Level 010 is the first operator checkpoint in the CyberFlipper curriculum.

It creates a visible local worksheet before any BadUSB, RF, NFC/RFID, IR, firmware, Marauder, fuzzer, jammer, or offensive-adjacent community idea is converted into CyberFlipper training material.

## Purpose

Level 010 teaches users to document:

- Who authorized the lab.
- What device, host, tag, card, remote, fixture, or SD-card pack is in scope.
- What is explicitly out of scope.
- Which public source inspired the work.
- Which safety controls apply.
- Where the visible `cyberflipper_` worksheet was saved.

## Files

```text
cf_l010_windows_authorized_use_worksheet.txt
cf_l010_linux_authorized_use_worksheet.txt
cf_l010_macos_authorized_use_worksheet.txt
```

## Expected output

Each script creates a visible local worksheet named with the `cyberflipper_` prefix.

Example:

```text
cyberflipper_l010_windows_authorized_use_worksheet.txt
cyberflipper_l010_linux_authorized_use_worksheet.txt
cyberflipper_l010_macos_authorized_use_worksheet.txt
```

## Defensive value

This level turns authorization into a repeatable artifact. It reduces ambiguity before higher-level labs create host posture reports, firmware notes, SD-card manifests, or detection-engineering worksheets.

## Review requirements

Human review is required before merge or public release because the files are BadUSB-style automation inputs.

Review for:

```text
[ ] Visible execution only.
[ ] Local output only.
[ ] cyberflipper_ output prefix.
[ ] No credentials, tokens, cookies, browser profile data, password-manager data, or secrets.
[ ] No persistence, stealth, evasion, privilege abuse, destructive behavior, RF jamming, Wi-Fi cracking, captive credential capture, or exploit chain.
[ ] Scope and authorization worksheet is clear.
[ ] Public sources are attributed.
```

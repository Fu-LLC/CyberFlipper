# CyberFlipper Awesome Watch — 2026-07-07

## Executive summary

Today's safe lab pack promotes CyberFlipper from isolated worksheets into a Level 099 capstone release gate. The goal is to help an operator prove that a Flipper Zero SD-card lab, BadUSB worksheet, firmware/app review, or community-source conversion is authorized, documented, reviewed, and safe to publish.

No offensive code was copied from community repositories. Risky source categories such as Marauder-style Wi-Fi tooling, jammer projects, fuzzers, cracking examples, and offensive firmware forks are treated only as sources for safety boundaries, lawful-use warnings, detection ideas, and lab review questions.

## Source signals reviewed

Primary source signals:

- `flipperdevices/flipperzero-firmware` release list currently shows `1.4.3` as the latest firmware release.
- Firmware `1.4.3` is a bugfix release for `1.4.2`, specifically noting an Infrared CLI plugin MissingImports fix.
- Firmware `1.4.2` remains the main feature-bearing release for this review cycle, with NFC CLI and parser changes, Sub-GHz protocol updates, BLE pairing-security guidance, JS GUI bindings, Infrared remote updates, BadUSB key-combo fixes, HID mouse-button configuration, CLI improvements, iButton write support, and LF RFID country-name display for FDX-B animal tags.
- CyberFlipper's existing README already defines Level 099 as a capstone for a full authorized lab workflow with docs, scripts, detection notes, and review checklist.
- CISA KEV-style prioritization and NIST CSF 2.0 governance language remain the right framing for safe CyberFlipper releases: authorization, inventory, exposure review, detection, response, recovery, and release control.

Community source conversion notes:

- Firmware forks and app catalogs are useful for compatibility tracking, API-change awareness, SD-card layout discipline, and review-gate examples.
- IRDB repositories are useful for taxonomy, attribution, duplicate handling, and benign remote-control organization.
- GPIO, Game Boy, badge, and SAO examples are useful for wiring diagrams, test fixtures, and hardware bring-up notes.
- Marauder, jammer, fuzzing, cracking, and offensive payload projects are not copied. They only inform defensive disclaimers, RF safety notes, detection-engineering worksheets, and lawful lab boundaries.

## Why it matters

Flipper-style labs often fail at the boring parts: provenance, consent, scope, evidence handling, and release review. Level 099 makes the operator write down those controls before treating any SD-card pack or payload set as publishable.

This is especially important when upstream changes affect HID, BadUSB key combinations, BLE pairing, NFC parsing, Sub-GHz protocol handling, CLI behavior, Infrared commands, or SD-card application workflows. Those areas can be harmless in a classroom but sensitive in public distribution.

## Repository-ready content added

```text
badusb/CyberFlipper_Lab/level_099/README.md
badusb/CyberFlipper_Lab/level_099/cf_l099_windows_release_gate.txt
badusb/CyberFlipper_Lab/level_099/cf_l099_linux_release_gate.txt
badusb/CyberFlipper_Lab/level_099/cf_l099_macos_release_gate.txt
docs/community/CYBERFLIPPER_LEVEL_099_CAPSTONE_RELEASE_GATE.md
docs/community/CYBERFLIPPER_SOURCE_WATCH_2026-07-07.md
docs/daily/2026-07-07-cyberflipper-awesome-watch.md
```

## Human review gate

This pack must remain draft-only until reviewed because it includes BadUSB-style files that launch visible local shells or editors and create `cyberflipper_` worksheets. The scripts are designed for owned systems, administered systems, classrooms, CTF ranges, and written-scope assessments only.

Reviewers should confirm:

```text
[ ] Commands are visible.
[ ] Output files use the cyberflipper_ prefix.
[ ] No hidden execution.
[ ] No credential, token, cookie, password-manager, browser-profile, or private-message collection.
[ ] No persistence, evasion, destructive command, exploit chain, RF abuse, Wi-Fi credential capture, or service abuse.
[ ] Source attribution is present.
[ ] Licensing is respected.
[ ] Risky-source inspiration has been converted into safety, detection, documentation, or mitigation guidance only.
```

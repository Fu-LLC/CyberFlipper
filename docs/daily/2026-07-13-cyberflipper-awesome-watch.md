# CyberFlipper Authorized Research Watch — 2026-07-13

## Scope

This update reviews public Flipper Zero ecosystem activity and converts it into defensive education, validation controls, and mitigation-first lab material. No offensive payload, RF transmission workflow, credential access, persistence, stealth, evasion, destructive action, or third-party exploitation is included.

## Notable public changes

### RogueMaster: Sub-GHz out-of-memory crash correction

A 2026-07-13 commit reports a fix for out-of-memory crashes in the Sub-GHz component. CyberFlipper does not reproduce or transmit arbitrary radio captures. The defensive lesson is broader: parsers, editors, capture viewers, and plugin workflows need explicit size ceilings, allocation-failure handling, repeatable test inputs, and recovery evidence.

Safe conversion:

- document the exact firmware build and application version;
- record free-space and SD-card health before testing;
- use owned, synthetic, receive-only, or pre-recorded lab data;
- define maximum file size, frame count, repeat count, and session duration;
- confirm the UI fails closed instead of hanging or rebooting;
- preserve a crash note without collecting secrets;
- verify restart, rollback, and known-good backup procedures.

### Unleashed: application tag synchronization

Unleashed updated its application tag on 2026-07-13. Frequent app-tag changes reinforce the need to record firmware/API compatibility, app source commit, manifest version, and rollback point before installing or publishing a lab.

### Official application catalog

Recent catalog activity includes PocketLab 1.3, Flipper Blinker, SubGHz-RAW-Edit 1.7, Gurpil, Subhound 1.2, a subnet utility, FReD FM, xRemote changes, and other additions. CyberFlipper treats these as provenance and capability-review examples. Radio-capable, GPIO, IR, USB, storage, or network-adjacent apps require a declared capability matrix and human approval.

### Official firmware engineering themes

Recent official firmware work moved the CCID USB layer into its test application and repaired documentation links. The safe lesson is to keep test-only interfaces isolated, reduce unnecessary firmware-level exposure, and ensure documentation references remain reproducible.

## Risky project intake

Projects involving Marauder, jamming, fuzzing, cracking, raw Sub-GHz editing, NFC/RFID emulation, or offensive BadUSB are not imported. CyberFlipper extracts only lawful-use boundaries, hardware and firmware interoperability notes, detection opportunities, failure-mode and rollback lessons, input-size ceilings, licensing, attribution, and isolated-lab validation.

## Added safe content

Level 023 introduces a **Memory Pressure and Recovery Review** with a provenance worksheet, bounded-input checklist, crash/reboot observation fields, SD-card and backup verification, recovery notes, and visible local BadUSB worksheets for Windows, Linux, and macOS.

All host-facing scripts visibly open a text editor and create a local file beginning with `cyberflipper_`. They do not elevate privileges, hide activity, access credentials, transmit data, or modify security controls.

## Human approval required

Do not merge or publish until a reviewer confirms every BadUSB keystroke is visible and benign; no command shell or remote destination is used; radio exercises are receive-only or use owned synthetic files; versions and commits are documented; recovery was tested on isolated hardware; and upstream licenses/source notes are retained.

## Source notes

- flipperdevices/flipperzero-firmware — official firmware engineering and documentation changes.
- flipperdevices/flipper-application-catalog — official application manifests and version updates.
- DarkFlippers/unleashed-firmware — application-tag synchronization; interoperability review only.
- RogueMaster/flipperzero-firmware-wPlugins — Sub-GHz out-of-memory fix and rapid plugin churn; defensive reliability lessons only.
- Other named ecosystem repositories remain review sources for licensing, safety boundaries, detection, and interoperability; no risky code is copied.

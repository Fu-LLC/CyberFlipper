# CyberFlipper Awesome Watch — 2026-07-02

## Safe theme

Level 050 adds evidence discipline for CyberFlipper labs: report provenance, file hashing, firmware/version notes, SD-card review habits, and clean handoff records.

This update treats firmware, app-catalog, RF, Marauder, jammer, fuzzing, and BadUSB community activity as defensive input only. No offensive code is copied. No RF transmission, jamming, credential capture, persistence, evasion, exploit chain, or third-party targeting logic is included.

## Signals reviewed

- Official Flipper firmware and documentation remain the baseline for hardware/API behavior, SD-card usage, app compatibility, and device-management assumptions.
- Official and community app catalogs remain useful for compatibility tracking, metadata comparison, and packaging notes.
- Community firmware repositories remain useful as change-signal sources, but CyberFlipper should document deltas rather than instructing users to bypass controls or flash risky builds.
- IRDB repositories remain useful for attribution structure, taxonomy, duplicate-control hygiene, and safe receive/use cases.
- Risky projects such as jammer, Marauder, fuzzer, cracking, and offensive payload repositories should only feed safety disclaimers, detection notes, lab boundary cards, and mitigation-first writeups.
- CISA KEV-style triage and current advisory review remain the right pattern for prioritizing CyberFlipper defensive labs: exposure, active exploitation, patch availability, mitigations, and observable control evidence.

## What changed for CyberFlipper

Added Level 050: Evidence, provenance, and lab handoff.

Files added:

```text
badusb/CyberFlipper_Lab/level_050/README.md
badusb/CyberFlipper_Lab/level_050/cf_l050_windows_evidence_manifest.txt
badusb/CyberFlipper_Lab/level_050/cf_l050_linux_evidence_manifest.txt
badusb/CyberFlipper_Lab/level_050/cf_l050_macos_evidence_manifest.txt
docs/community/CYBERFLIPPER_LEVEL_050_EVIDENCE_PROVENANCE.md
```

## Defensive value

Level 050 gives the project a repeatable way to document what was run, when it was run, which files were generated, what their hashes are, and which host/firmware/version assumptions were present during the lab.

This supports:

- classroom grading
- authorized client handoff
- internal regression testing
- payload quality review
- SD-card content provenance
- incident-response-style note discipline
- safer public release review

## Human-review gate

Do not auto-merge this pack. It launches local command interpreters and inventories local CyberFlipper report files. It is visible and report-only, but it still requires human review before public release.

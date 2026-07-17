# CyberFlipper Awesome Watch — 2026-07-12

## Authorized research scope

This update converts public Flipper Zero ecosystem activity into defensive education, provenance review, rollback planning, compatibility validation, and visible local lab documentation. It does not reproduce offensive payloads, RF abuse, credential capture, Wi-Fi cracking, NFC cloning, fuzzing attacks, jamming, persistence, stealth, or exploitation workflows.

## What changed

### Official application catalog

Recent catalog activity included:

- PocketLab 1.3 and its catalog addition.
- SubGHz-RAW-Edit 1.7.
- Subhound 1.2.
- A network-subnet utility.
- Flipper Blinker, Gurpil, FReD FM, a CO2 detector application, a remote-control application, and an xRemote pause-feature update.

These changes show rapid application turnover across hardware, radio-adjacent, utility, and entertainment categories. Catalog inclusion is not a substitute for local approval, source review, API compatibility checks, license verification, rollback readiness, or lawful-use constraints.

### Community firmware and documentation

- Unleashed firmware recently synchronized its application tag and API.
- UberGuidoZ repaired dead links in Sub-GHz documentation.
- RogueMaster incorporated Gurpil and SubGHz-RAW-Edit updates.

Community integrations are useful signals for interoperability testing, but CyberFlipper does not treat community inclusion as a security endorsement.

## Why it matters

Fast-moving firmware and application ecosystems create four recurring defensive risks:

1. Provenance drift: manifests may point to new commits without a reviewer recording what changed.
2. Capability drift: an update may add hardware, radio, storage, USB, or network-adjacent behavior.
3. Compatibility drift: firmware API changes can break applications or alter expected behavior.
4. Rollback failure: users may update without preserving known-good firmware, application versions, configuration, or SD-card backups.

## Safe CyberFlipper addition

Level 022 adds an Application Provenance and Permission Review lab. The lab requires reviewers to record application identity, exact source and license, requested interfaces, firmware/API compatibility, expected local changes, lawful-use boundaries, rollback steps, validation evidence, and reviewer approval.

## Community-source conversion matrix

| Source category | Safe CyberFlipper use | Excluded use |
|---|---|---|
| Official firmware/catalog | Release review, API compatibility, manifest provenance | Blind installation |
| Custom firmware | Diff review, rollback, interoperability notes | Restricted-feature operational guidance |
| IR databases | Attribution, data hygiene, duplicate and format validation | Unauthorized device control |
| Marauder/Wi-Fi tooling | Detection ideas, lab isolation, consent requirements | Credential capture or cracking |
| Jammer projects | Interference awareness, legal warnings, monitoring concepts | Construction or transmission instructions |
| Fuzzers | Parser-boundary lessons, crash handling, isolated validation | Attack corpora or unauthorized fuzzing |
| BadUSB examples | Visible defensive worksheets and local evidence | Stealth, persistence, secrets, or remote transfer |

## Required review gates

- Verify exact upstream commit and license.
- Read the application manifest and declared capabilities.
- Test only on isolated, owned hardware.
- Preserve a known-good rollback image and SD-card backup.
- Record expected files, settings, and visible behavior.
- Reject unexplained network access, hidden execution, secret collection, or destructive actions.
- Require human approval before merge or public release.

## Sources reviewed

- flipperdevices/flipperzero-firmware
- flipperdevices/flipper-application-catalog
- DarkFlippers/unleashed-firmware
- RogueMaster/flipperzero-firmware-wPlugins
- UberGuidoZ/Flipper
- Official Flipper documentation and release materials
- NIST Cybersecurity Framework guidance
- CISA vulnerability and advisory resources

No third-party code was copied into this update.
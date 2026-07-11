# CyberFlipper Awesome Watch — 2026-07-10

## Scope

This update converts recent Flipper Zero ecosystem activity into defensive education, validation, and mitigation-first repository work. No offensive code is copied. Radio, Wi-Fi, NFC/RFID, firmware, fuzzing, jammer, Marauder, BadUSB, and 0day-themed projects are treated as sources for safety controls, interoperability notes, detection engineering, and authorized lab exercises only.

## Upstream changes reviewed

### Official firmware

Recent `flipperdevices/flipperzero-firmware` commits include:

- `7432d21a7e362d4a5f636e24d6209fbb2eedff1f` — moved the CCID USB layer from firmware HAL into the `ccid_test` app.
- `d4b023ce5357a28eb3239013ed8ea4f23cf93c2d` — added CCID interrupt support.
- `101c20d736fed67bcf24fbbe87886ce4a26ae088` — added an NFC plugin for identifying some hotel systems.
- `d1019d3bb3f7c45a4f720eab769c454b820c1bd8` and `86d9141bb4bf9630321ea70f5dd2e58ca2af1e0e` — documentation-link repairs.

Defensive takeaway: USB smart-card behavior and NFC identification logic should be validated as distinct trust boundaries. CyberFlipper should document device-class visibility, expected host prompts, application provenance, and a no-authentication-bypass rule.

### Official application catalog

Recent `flipperdevices/flipper-application-catalog` commits include:

- `c30388b8248ce56d40c253d7d96ffa3cc4240dfe` — Access Audit v1.13.0.
- `3909d276f60f970abd31f50c5a10f27892046d7f` — Metroflip v2.0.
- `5b0a0df92f14abe1d87bc123ae16e9c15dc4ade1` — Flipper Share v1.4.
- `079076b9d085b29216b3f82d59d352f79d173316` — Xiaomi Filter Reset 1.0.
- `44eb9a31008273c5125d77acd7b9c6ac55db88e1` — Mayan Decoder.

Defensive takeaway: application review must record version, source repository, manifest permissions, hardware interfaces, storage behavior, and whether the application can transmit, emulate, write, or modify external systems.

### Community firmware and plugins

Recent RogueMaster activity included Sub-GHz raw-edit changes, repeat/gap handling changes, API updates, release refreshes, and a merge from the official development branch. Relevant commits include `67feacc2db2bc939e2e5732d14f86ecdb4c5b84d`, `b462daca38f309b4d1997e5db790a18b3cdb2c90`, `d9e8a0fe85126fe14f4525461a19996182f04f41`, and `6e350627c2975a85840ab4d58d077dde9d077fda`.

Defensive takeaway: timing changes can alter replay fidelity and interoperability. CyberFlipper must not publish replay instructions for third-party systems. Safe testing is limited to owned lab transmitters/receivers, shielded or isolated environments where required, receive-only analysis when possible, and documented regional radio rules.

## Community conversion matrix

| Source area | Safe CyberFlipper extraction | Prohibited conversion |
|---|---|---|
| Official firmware | API, USB-class, storage, NFC parser, build and release review | Authentication bypass or unauthorized emulation |
| Application catalog | Provenance, permissions, version tracking, manifest review | Blind installation or copied application code |
| Unleashed/Xtreme/RogueMaster | Compatibility matrix, feature-drift notes, rollback plans | Enabling unrestricted transmit behavior |
| IR databases | Data provenance, duplicate detection, lawful owned-device testing | Public-space disruption or uncontrolled blasting |
| ProtoPirate and protocol tools | Decoder validation and malformed-input handling | Exploit workflows against third-party devices |
| RF jammer projects | Detection, spectrum-policy notes, incident response | Construction, operation, or transmission guidance |
| Marauder tooling | Wireless logging controls and authorized lab boundaries | Credential capture, cracking, deauthentication, or captive portals |
| Multi_Fuzzer | Parser hardening, test-case governance, crash logging | Fuzzing production or third-party systems |
| qFlipper | Backup, restore, update integrity, and recovery drills | Downgrade abuse or unsafe firmware replacement |
| Game Boy trading | Interoperability testing and protocol documentation | Circumventing ownership or game-service controls |

## Level 1–99 additions

### Levels 1–10: foundations

- Authorization card, visible-output rule, source attribution, and basic device inventory.
- No hidden windows, no remote transfer, no credential collection.

### Levels 11–19: workstation context

- Operator scope card.
- Host and USB-class context.
- Network context without credential or packet capture.
- Browser, productivity, communications, backup, and endpoint-security posture.
- Final artifact manifest using the `cyberflipper_` prefix.

### Levels 20–39: triage and exposure

- Process and listener review.
- Event-log count summaries.
- Patch posture and remediation worksheets.
- Vendor advisory and CISA KEV mapping without exploit reproduction.

### Levels 40–59: firmware and evidence

- Firmware provenance and rollback notes.
- Application-catalog review cards.
- SD-card inventory and hash manifests.
- Evidence handoff and chain-of-custody worksheets.

### Levels 60–79: SBOM and change intake

- File manifests, source attribution, license review, and release linting.
- Risky-source conversion template documenting what was excluded.
- Human approval gate for shell-launching, host-inventory, radio/RF, firmware, or security-sensitive content.

### Levels 80–99: sandbox and capstone

- Isolated-lab readiness.
- Safe sample-handling worksheets.
- Mitigation plans, final validation, maintainer review, public-release notes, and rollback criteria.

## Safe BadUSB standard

Any future Flipper-compatible `.txt` script must:

1. Use visible local applications or terminals.
2. Create a visible local file beginning with `cyberflipper_`.
3. State its purpose in comments.
4. Avoid elevation, persistence, stealth, evasion, credential access, remote transfer, destructive changes, and third-party targeting.
5. Stop safely if expected UI context is not present.
6. Require human review before merge.

## Recommended repository work

- Add a `docs/review/COMMUNITY_SOURCE_INTAKE_CHECKLIST.md` checklist.
- Add Level 011–019 READMEs before executable scripts.
- Add a lint rule that rejects suspicious keywords and requires `cyberflipper_` output naming in BadUSB files.
- Add a firmware/app review template with version, commit, license, interfaces, transmit capability, storage writes, rollback, and approval fields.

## Human approval required

Do not merge shell-launching, host-inventory, radio/RF, firmware, Marauder, Wi-Fi, NFC/RFID emulation, fuzzing, or other security-sensitive changes without manual review. This daily note is documentation-only and safe to review as a draft.

## Attribution

Primary repositories reviewed:

- `flipperdevices/flipperzero-firmware`
- `flipperdevices/flipper-application-catalog`
- `DarkFlippers/unleashed-firmware`
- `Flipper-XFW/Xtreme-Firmware`
- `RogueMaster/flipperzero-firmware-wPlugins`
- `UberGuidoZ/Flipper`
- `UberGuidoZ/Flipper-IRDB`
- `Lucaslhm/Flipper-IRDB`
- `RocketGod-git/ProtoPirate`
- `RocketGod-git/flipper-zero-rf-jammer`
- `SkeletonMan03/FZEasyMarauderFlash`
- `kbembedded/Flipper-Zero-Game-Boy-Pokemon-Trading`
- `DarkFlippers/Multi_Fuzzer`
- `DarkFlippers/qFlipper`

Review licenses and repository-specific attribution requirements before reusing any material.
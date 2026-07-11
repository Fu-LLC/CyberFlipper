# CyberFlipper Community Source Matrix

This matrix records how community Flipper Zero work is used in CyberFlipper without copying risky behavior.

| Source family | Useful idea | CyberFlipper conversion |
|---|---|---|
| flipperdevices/flipperzero-firmware | Official firmware baseline, release notes, firmware source structure. | Firmware channel review, stable/dev/RC distinction, update hygiene checklist. |
| flipperdevices/flipper-application-catalog | App packaging and catalog governance. | App provenance, SD-card inventory, approved-app review worksheet. |
| DarkFlippers/unleashed-firmware | Community firmware capability comparison. | Custom-firmware risk disclosure, rollback plan, feature matrix. |
| Flipper-XFW/Xtreme-Firmware | Community firmware feature expansion. | Lab-only comparison checklist and user-risk notes. |
| RogueMaster/flipperzero-firmware-wPlugins | Plugin-heavy firmware examples. | Plugin provenance, licensing, and compatibility review. |
| UberGuidoZ/Flipper | Curated community payloads and docs. | Attribution-first education notes and safe rewrite ideas. |
| UberGuidoZ/Flipper-IRDB and Lucaslhm/Flipper-IRDB | IR remote database structure. | Asset inventory, IR database provenance, device-owner permission notes. |
| RocketGod-git/ProtoPirate | Protocol exploration ideas. | Lab-scope documentation and defensive protocol-observation worksheet. |
| RocketGod-git/flipper-zero-rf-jammer | RF-risk demonstration. | Interference-response tabletop, RF policy, and lawful-use warning only. |
| SkeletonMan03/FZEasyMarauderFlash | Wi-Fi developer-board flashing workflow. | Firmware provenance, flash-log worksheet, rollback and isolation checklist. |
| kbembedded/Flipper-Zero-Game-Boy-Pokemon-Trading | Benign hardware/app integration. | Safe GPIO/app integration example and user-facing lab notes. |
| DarkFlippers/Multi_Fuzzer | Fuzzing workflow concepts. | Sandbox-only fuzzing governance, crash triage, responsible disclosure notes. |
| DarkFlippers/qFlipper | Desktop update and recovery workflow. | qFlipper update validation, driver notes, recovery checklist. |

## Safety Rules

- Do not copy payloads that extract secrets, bypass controls, hide actions, persist, destroy data, jam radio, crack credentials, or target third-party systems.
- Every executable training item must create a visible local artifact with a `cyberflipper_` prefix.
- RF, Wi-Fi, firmware, fuzzing, and HID automation stay draft-only until reviewed.
- Licenses and upstream attribution must be checked before copying any nontrivial text, code, database content, or scripts.

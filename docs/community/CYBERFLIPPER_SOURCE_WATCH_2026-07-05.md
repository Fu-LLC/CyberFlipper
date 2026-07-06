# CyberFlipper Source Watch — 2026-07-05

## Reviewed source classes

This source watch reviewed public Flipper Zero ecosystem activity and general defensive guidance, then converted it into safe CyberFlipper lab content.

## Primary signals

| Source | Observed signal | Safe CyberFlipper conversion |
|---|---|---|
| `flipperdevices/flipperzero-firmware` | `1.4.3` bugfix release for `1.4.2`; Infrared CLI import fix. | Treat CLI/plugin changes as compatibility-review and smoke-test inputs. |
| `flipperdevices/flipperzero-firmware` | `1.4.2` included NFC CLI work, FeliCa/MIFARE parser changes, BLE pairing-security guidance, Sub-GHz protocol updates, BadUSB key-combo fixes, HID remote changes, CLI improvements, iButton support, LF RFID display changes. | Track parser/protocol/HID/BadUSB changes in safe regression worksheets. |
| `DarkFlippers/unleashed-firmware` | Recent dev history shows app bumps, NFC display/parser work, BLE sync fixes, API-change report patterns, free-flash report fixes, and Sub-GHz RAW crash-prevention work. | Convert to source-intake rules, version capture, receive-only RF safety notes, and crash/runbook stop conditions. |
| Flipper application ecosystem | App updates can change permissions, APIs, file layout, UI, and device behavior. | Require app provenance notes before adding or recommending apps. |
| IRDB projects | Large community remote databases need attribution, taxonomy, and device-owner testing. | Store source notes and test only with owned IR devices. |
| Marauder-style Wi-Fi tooling | High-risk wireless workflows. | Convert only into policy review, lab isolation, and detection notes. No cracking, capture, or credential collection. |
| Jammer projects | RF transmission abuse risk. | Convert only into legality warnings and detection/safety notes. No instructions or payloads. |
| Fuzzer projects | Parser and crash-testing value but unsafe on third-party systems. | Convert into parser-hardening checklists and local-only crash-report templates. |
| CISA KEV / advisories | Active exploitation should drive prioritization. | Use KEV status as a patch-SLA and tabletop-priority input, not as exploit guidance. |
| NIST CSF 2.0 | Govern, Identify, Protect, Detect, Respond, Recover framing. | Map lab packs to governance, asset identification, detection, response, and recovery evidence. |

## Attribution policy

CyberFlipper does not copy external payload code blindly. When a source inspires a CyberFlipper file:

```text
[ ] Record source repository or advisory.
[ ] Record observed change date or release version.
[ ] Record license if code or structure is reused.
[ ] Prefer summaries, matrices, and original safe worksheets.
[ ] Avoid copying offensive or dual-use logic.
[ ] Preserve creator credit in docs/community attribution notes.
```

## Defensive priorities for the next cycle

```text
1. Add app-catalog provenance fields to future level READMEs.
2. Add parser-change smoke-test templates for NFC/IR/LFRFID notes.
3. Expand BLE pairing hygiene guidance into a non-scripted checklist.
4. Keep RF-related content receive-only and documentation-only.
5. Keep BadUSB files visible, local, and worksheet-driven.
6. Add safe test-stop conditions to every level from 080 onward.
```

## Public-release gate

The Level 080 pack is safe for draft review. It is not safe for automatic merge because it launches local host tools through BadUSB text files.

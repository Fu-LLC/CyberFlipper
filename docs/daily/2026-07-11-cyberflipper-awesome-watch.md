# CyberFlipper Awesome Watch — 2026-07-11

## Scope
Authorized defensive research only. No offensive code was copied. RF, Marauder, jammer, fuzzing, cracking, NFC/RFID, BadUSB, and firmware projects are used only for safety controls, detection ideas, interoperability review, and mitigation-first education.

## What changed
- Official firmware recently fixed two NFC parser stack-buffer overflows in MIFARE Ultralight FAST_READ handling and DESFire file-settings parsing by adding explicit bounds checks before copying variable-length data into fixed-size stack storage.
- Recent official work also includes USB HID keyboard LED-state reporting fixes, file-copy retry behavior, CCID test-app USB refactoring, and documentation repairs.
- DarkFlippers Unleashed synchronized its application tag and API on 2026-07-11. Treat API synchronization as an interoperability and rollback checkpoint, not proof that every third-party app is safe.
- UberGuidoZ/Flipper repaired dead links in Sub-GHz documentation on 2026-07-11. Documentation integrity belongs in release validation.
- RogueMaster continued rapid plugin and Sub-GHz raw-editor updates. Record exact commit, firmware API, source, license, rollback image, and test hardware before installation.
- ProtoPirate is used only for protocol-boundary, memory-pressure, plugin-isolation, and lawful-radio-lab lessons. Brute-force, emulation, vehicle, or transmission instructions are excluded.

## Safe addition
This run adds Level 020: NFC Parser Boundary and Firmware Provenance Review, including visible local BadUSB worksheets and a mandatory human-review gate.

## Primary source notes
- flipperdevices/flipperzero-firmware commit `0dd3681f63af74fc12f6ad3f50e93c56e4b9dd28`.
- DarkFlippers/unleashed-firmware commit `9bcabc0134a4503e341b804999dd73ec890466af`.
- UberGuidoZ/Flipper commits `8a740059a34b8f67d80bd1495c83fc2ce8e0b5b1` and `a6f21a4e8bcdf3df8db06a2710eb7822f38507a4`.
- NIST Cybersecurity Framework 2.0.

## Publication gate
Human review is required before merge because the pack contains BadUSB-style host automation. Confirm visible execution, local-only output, `cyberflipper_` prefixes, no elevation, no hidden windows, no secret collection, and no remote transfer.

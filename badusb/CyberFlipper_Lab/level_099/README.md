# CyberFlipper Level 099 — Capstone Release Gate

## Purpose

Level 099 is the capstone workflow for CyberFlipper authorized lab releases. It proves scope, authorization, provenance, evidence discipline, defensive value, and release readiness.

This level is intended for owned devices, administered workstations, classrooms, CTF ranges, and written-scope security assessments. It creates a visible local `cyberflipper_` worksheet and prompts the operator to document the full lab lifecycle before publishing or merging a pack.

## Files

| File | Platform | Output |
|---|---|---|
| `cf_l099_windows_release_gate.txt` | Windows | Desktop `cyberflipper_l099_windows_release_gate.txt` |
| `cf_l099_linux_release_gate.txt` | Linux | Home directory `cyberflipper_l099_linux_release_gate.txt` |
| `cf_l099_macos_release_gate.txt` | macOS | Desktop `cyberflipper_l099_macos_release_gate.txt` |

## Expected output

Each script creates a worksheet with fields for operator context, authorization statement, device and SD-card provenance, firmware review, app-catalog review, source attribution, licensing notes, defensive use case, detection notes, mitigation notes, evidence inventory, release decision, and reviewer signoff.

## Defensive value

Level 099 closes the loop across the earlier levels: patch prioritization, firmware and app review, evidence provenance, SD-card SBOM-lite discipline, change intake, sandbox readiness, and evidence communications.

The capstone makes the operator state whether the content is safe to publish, safe only for internal lab use, or blocked pending review.

## Review requirements

Human review is required before merge or public release because the files launch visible local shells or editors and create host-side worksheets.

Reviewers should confirm:

```text
[ ] Commands are visible.
[ ] Output file uses the cyberflipper_ prefix.
[ ] No hidden execution.
[ ] No sensitive personal or authentication data collection.
[ ] No persistence, evasion, destructive behavior, exploit chaining, or privilege-abuse logic.
[ ] No RF misuse, network misuse, or credential capture.
[ ] Risky sources are converted into safety, detection, documentation, or mitigation guidance only.
[ ] Attribution and licensing notes are present.
[ ] The release decision is explicit.
```

## Operator note

Run only on systems you own, administer, or have written permission to test. Keep the generated worksheet with the release notes and review records. Treat it as proof of process, not proof of compromise.

# CyberFlipper Level 080 Sandbox Runbook

## Purpose

Level 080 is the pre-run gate for security-sensitive CyberFlipper content. It creates a local worksheet before testing material derived from firmware changes, app-catalog updates, BadUSB examples, NFC/RFID files, IR packs, Sub-GHz notes, GPIO experiments, fuzzing discussions, Marauder-style Wi-Fi projects, jammer projects, or exploit-themed advisories.

The goal is not to reproduce offensive behavior. The goal is to document the boundary before testing anything that could be misunderstood, mishandled, or unsafe outside a lab.

## Required lab boundary

Use a controlled environment:

```text
[ ] Owned device or written-scope device only
[ ] Test account only
[ ] VM, spare host, or sacrificial workstation preferred
[ ] Snapshot or rollback available
[ ] Network isolation considered
[ ] Host logging enabled
[ ] Expected output known
[ ] Stop conditions written down
[ ] Source URL and license recorded
[ ] No third-party target involved
```

## Safe source conversion

| Source type | CyberFlipper conversion |
|---|---|
| Firmware release notes | Version review, compatibility table, parser-change note, safe regression test. |
| App-catalog updates | App provenance checklist, install notes, permission expectations. |
| BadUSB examples | Visible local worksheet, defensive inventory, tabletop notes. |
| NFC/RFID research | Owned-card lab notes, parser compatibility matrix, legal boundary. |
| IR databases | Remote taxonomy, file provenance, owned-device testing. |
| Sub-GHz material | Region-aware receive-only documentation and safety notes. |
| Marauder/Wi-Fi tooling | Wireless policy review, lab-isolation checklist, no cracking or credential capture. |
| Jammer projects | RF legality warning, detection notes, no transmission instructions. |
| Fuzzers | Parser hardening checklist, crash-report template, no third-party fuzzing. |
| Exploit/0day advisories | Patch triage, KEV/EPSS/CVSS review, mitigation worksheet. |

## Reviewer questions

Before a PR leaves draft status, answer:

```text
What is the source?
What changed?
Why does it matter defensively?
What is the safe CyberFlipper transformation?
Does the file launch a shell, editor, terminal, or host inventory command?
Does it create only visible local output?
Does the output use a cyberflipper_ prefix?
Does it avoid credentials, tokens, cookies, browser data, persistence, stealth, exploit chains, RF abuse, and Wi-Fi abuse?
What human approval is required before merge?
```

## Stop conditions

Stop testing when any of the following appears:

```text
Unexpected privilege prompt
Unexpected network connection
Unexpected hidden window
Unexpected credential prompt
Unexpected destructive file operation
Unexpected RF transmission behavior
Unexpected Wi-Fi association, deauth, capture, cracking, or portal flow
Unexpected crash outside a disposable lab
Unclear license or attribution state
```

## Output standard

Every Level 080 script must create a visible worksheet, not a hidden report.

Required filename pattern:

```text
cyberflipper_l080_<platform>_sandbox_readiness.txt
```

Required fields:

```text
Title
Date
Host
Purpose
Safety statement
OS information
Reviewer checklist
```

## Merge rule

Level 080 content remains draft-only until reviewed. It launches local host tools and therefore requires human review even when the script content is benign.

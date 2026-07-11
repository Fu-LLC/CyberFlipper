# CyberFlipper BadUSB Review Backlog

Executable BadUSB `.txt` files for Level 011-019 are intentionally deferred in this PR. The repository should not merge host-facing HID automation until a human reviewer confirms that each file is authorized, visible, non-stealth, and limited to defensive local output.

## Required Script Properties

- File extension: `.txt`.
- Syntax: Flipper Zero BadUSB / DuckyScript-style text.
- Output: visible local file or window with a `cyberflipper_` prefix.
- Scope: local authorized host only.
- Behavior: no stealth, no persistence, no data exfiltration, no credential access, no destructive change, no privilege abuse.

## Candidate Files After Review

| Level | Candidate filename | Visible artifact |
|---|---|---|
| 011 | `cf_l011_scope_card.txt` | `cyberflipper_l011_scope_card` |
| 012 | `cf_l012_host_context.txt` | `cyberflipper_l012_host_context` |
| 013 | `cf_l013_network_context.txt` | `cyberflipper_l013_network_context` |
| 014 | `cf_l014_browser_posture.txt` | `cyberflipper_l014_browser_posture` |
| 015 | `cf_l015_productivity_stack.txt` | `cyberflipper_l015_productivity_stack` |
| 016 | `cf_l016_comms_stack.txt` | `cyberflipper_l016_comms_stack` |
| 017 | `cf_l017_backup_sync.txt` | `cyberflipper_l017_backup_sync` |
| 018 | `cf_l018_security_posture.txt` | `cyberflipper_l018_security_posture` |
| 019 | `cf_l019_evidence_manifest.txt` | `cyberflipper_l019_evidence_manifest` |

## Reviewer Checklist

- Confirm written authorization language is visible in the output.
- Confirm the user sees the activity happen.
- Confirm the script creates only CyberFlipper training artifacts.
- Confirm the script does not read secrets, private files, browser stores, password managers, tokens, cookies, SSH keys, cloud credentials, or unrelated user data.
- Confirm the script does not disable, bypass, or tamper with security controls.
- Confirm RF, Wi-Fi, firmware, fuzzing, and exploit behavior are not present.

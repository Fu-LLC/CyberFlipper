# CyberFlipper Level 020 - Authorized IR and Detection Triage

Level 020 moves CyberFlipper beyond basic inventory and into structured local incident-response practice. It is designed for authorized labs, classrooms, owned workstations, and blue-team validation exercises.

## Files

- `cf_l020_windows_ir_triage.txt` - Windows host, process, network, listener, and security posture triage.
- `cf_l020_windows_detection_counts.txt` - Windows event-provider and event-ID count summary for detection engineering practice.
- `cf_l020_linux_ir_triage.txt` - Linux host, process, service, socket, and warning-log triage.
- `cf_l020_macos_ir_triage.txt` - macOS host, process, listener, firewall, and update posture triage.
- `cf_l020_report_manifest.txt` - Windows report manifest and SHA256 hash list for CyberFlipper Level 020 outputs.

## Safety boundary

These scripts create visible local reports only. They do not collect passwords, tokens, cookies, browser stores, private documents, password-manager vaults, cloud sync content, VPN secrets, or third-party system data. They do not install software, create persistence, bypass controls, transmit data, exploit vulnerabilities, jam RF, crack Wi-Fi, alter security settings, or delete files.

## Defender detections to validate

- New USB HID keyboard device activity.
- Interactive PowerShell, Terminal, or shell launch soon after HID insertion.
- Rapid typed command sequences.
- Local report creation with `cyberflipper_l020` naming.
- Local enumeration of processes, listening ports, firewall state, patch posture, and event-log counts.

## Approval gate

Human approval is required before merge, publication, classroom use, or field use because these scripts launch command interpreters and enumerate host state. The proper use case is authorized defensive training only.

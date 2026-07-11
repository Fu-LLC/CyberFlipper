# Community Source Intake Checklist

Use this checklist before converting any external Flipper Zero project, commit, application, database, payload, firmware feature, or writeup into CyberFlipper content.

## Identification

- [ ] Source repository and owner recorded.
- [ ] Exact commit, tag, release, or application version recorded.
- [ ] Review date recorded.
- [ ] License identified and compatible with the intended use.
- [ ] Attribution requirements recorded.
- [ ] Primary source preferred over mirrors or reposts.

## Capability classification

- [ ] USB/HID/CCID behavior identified.
- [ ] NFC/RFID read, write, emulate, or identify capability recorded.
- [ ] Sub-GHz or other radio receive/transmit capability recorded.
- [ ] Infrared transmit capability recorded.
- [ ] Wi-Fi, Marauder, Bluetooth, serial, GPIO, or external-module capability recorded.
- [ ] Filesystem and SD-card writes recorded.
- [ ] Network or remote-transfer behavior recorded.
- [ ] Required privileges and host interaction recorded.

## Safety conversion

- [ ] Educational objective is defensive and specific.
- [ ] Third-party targeting is excluded.
- [ ] Credential, token, cookie, password, and secret access is excluded.
- [ ] Persistence, stealth, evasion, and destructive behavior are excluded.
- [ ] Jamming, unauthorized transmission, cracking, deauthentication, and captive credential capture are excluded.
- [ ] Fuzzing is limited to owned, isolated lab targets.
- [ ] Radio testing uses receive-only methods where possible.
- [ ] Regional radio rules and test authorization are documented.
- [ ] Mitigation and detection guidance are included.
- [ ] Rollback and recovery steps are included.

## BadUSB-specific checks

- [ ] File is valid DuckyScript-style text.
- [ ] Activity is visible to the operator.
- [ ] Output is local and begins with `cyberflipper_`.
- [ ] No hidden windows or silent execution.
- [ ] No elevation or privileged service manipulation.
- [ ] No credential or browser-secret collection.
- [ ] No remote upload or command-and-control behavior.
- [ ] No persistence, bypass, or security-control disabling.
- [ ] Script stops safely when expected UI context is unavailable.
- [ ] Script comments state purpose, platform, expected output, and limitations.

## Validation

- [ ] Tested only on an owned or explicitly authorized lab system.
- [ ] Test environment and firmware version recorded.
- [ ] Expected and observed outputs recorded.
- [ ] No unexpected network, radio, or filesystem activity observed.
- [ ] Static review completed.
- [ ] License and attribution review completed.
- [ ] Human approval obtained for shell-launching, host-inventory, radio/RF, firmware, Wi-Fi, NFC/RFID emulation, fuzzing, or other security-sensitive material.

## Publication decision

- [ ] Safe to publish as documentation only.
- [ ] Draft PR required.
- [ ] Maintainer review required.
- [ ] Legal/regulatory review required.
- [ ] Rejected or quarantined, with reason documented.

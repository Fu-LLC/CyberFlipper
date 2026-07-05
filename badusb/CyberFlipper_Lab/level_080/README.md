# CyberFlipper Level 080 — Sandboxed Analysis Setup

Level 080 creates a visible pre-run worksheet for testing new CyberFlipper content in a controlled environment.

Use it before testing:

- new BadUSB scripts,
- new `.fap` files,
- firmware-derived notes,
- SD-card packs,
- NFC/RFID learning files,
- IR packs,
- GPIO experiments,
- Sub-GHz documentation,
- or community-source conversions.

## Files

```text
cf_l080_windows_sandbox_readiness.txt
cf_l080_linux_sandbox_readiness.txt
cf_l080_macos_sandbox_readiness.txt
```

## Expected output

Each script creates a visible local worksheet using a `cyberflipper_` prefix.

Expected output examples:

```text
Desktop/cyberflipper_level080/cyberflipper_l080_windows_sandbox_readiness.txt
~/Desktop/cyberflipper_level080/cyberflipper_l080_linux_sandbox_readiness.txt
~/Desktop/cyberflipper_level080/cyberflipper_l080_macos_sandbox_readiness.txt
```

## Defensive value

This level forces the operator to record the analysis boundary before running security-sensitive lab content.

Minimum readiness notes:

- device owner,
- host OS,
- firmware or app version under review,
- source URL or repository,
- license and attribution state,
- sandbox or VM status,
- snapshot or rollback state,
- test account state,
- network isolation state,
- logging state,
- expected output,
- stop conditions.

## Detection notes

Host defenders should be able to see:

- user-opened terminal or editor,
- visible local output file creation,
- benign text-file writes,
- no hidden execution,
- no remote callbacks,
- no credential access,
- no persistence,
- no destructive commands.

## Review requirements

Do not merge Level 080 changes until a human reviewer confirms:

```text
[ ] Commands are visible.
[ ] Output uses cyberflipper_ prefix.
[ ] No credential, token, cookie, browser-profile, or password-manager access.
[ ] No stealth, persistence, evasion, destructive behavior, or privilege abuse.
[ ] No RF transmission, jamming, Wi-Fi cracking, or captive credential capture.
[ ] Scripts produce local defensive worksheets only.
[ ] Source-watch notes include attribution and safe conversion rules.
```

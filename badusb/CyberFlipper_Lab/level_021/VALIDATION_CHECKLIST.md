# Level 021 Validation Checklist

## Source and licensing
- [ ] Official firmware commit and date recorded.
- [ ] Community source, commit, license, and attribution recorded.
- [ ] No third-party code or data copied without license review.

## BadUSB safety
- [ ] Script visibly opens a local terminal or editor.
- [ ] Output filename begins with `cyberflipper_`.
- [ ] Output remains local and visible.
- [ ] No elevation, hidden window, encoded command, persistence, remote transfer, credential access, browser-data access, security-control changes, or destructive command.
- [ ] No malformed NFC data, tag emulation, RF transmission, Wi-Fi activity, jamming, fuzzing, or exploitation.

## Lab readiness
- [ ] Test host is isolated and non-production.
- [ ] Flipper firmware and application API versions are recorded.
- [ ] Known-good firmware rollback package and SD-card backup exist.
- [ ] Only owned lab tags/readers are in scope.
- [ ] Expected output and stop conditions are documented.

## Publication gate
- [ ] Human reviewer inspected every keystroke.
- [ ] Script was run successfully on the named operating system.
- [ ] Generated worksheet was manually reviewed for sensitive data.
- [ ] Maintainer approval recorded before merge or public release.

# Level 022 Validation Checklist

## Source and license

- [ ] Application name and version recorded.
- [ ] Upstream repository and exact commit recorded.
- [ ] License identified and compatible with intended use.
- [ ] Attribution and source notes included.

## Capability review

- [ ] Manifest and documentation reviewed.
- [ ] USB, storage, GPIO, infrared, NFC/RFID, Sub-GHz, BLE, and network-adjacent capabilities marked yes/no/unknown.
- [ ] No unexplained capability or hidden execution path remains.
- [ ] Lawful-use and authorization boundary documented.

## Compatibility and rollback

- [ ] Firmware version and API compatibility recorded.
- [ ] Known-good firmware or application version preserved.
- [ ] SD-card/configuration backup verified.
- [ ] Rollback procedure tested or explicitly deferred.

## BadUSB lint

- [ ] Visible editor only.
- [ ] Output filename begins with `cyberflipper_`.
- [ ] No elevation, credential access, persistence, stealth, destructive action, remote transfer, or radio operation.
- [ ] Every keystroke manually inspected.
- [ ] Tested only on an isolated non-production host.

## Release decision

- [ ] Evidence attached.
- [ ] Reviewer named.
- [ ] Decision recorded: approve, revise, or reject.
- [ ] Security-sensitive files remain unmerged pending human approval.
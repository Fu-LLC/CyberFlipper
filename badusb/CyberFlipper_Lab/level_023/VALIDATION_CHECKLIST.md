# Level 023 Validation Checklist

## Authorization and provenance

- [ ] Written authorization and asset owner recorded.
- [ ] Firmware name, version, build date, and source commit recorded.
- [ ] Application name, version, manifest/API target, source commit, and license recorded.
- [ ] Upstream source links and attribution retained.
- [ ] Known-good firmware and SD-card backup available.

## Safe data and radio controls

- [ ] Test data is synthetic, owned, receive-only, or pre-recorded.
- [ ] No Sub-GHz, Wi-Fi, BLE, NFC/RFID, infrared, or GPIO transmission is required.
- [ ] No jammer, cracking, credential capture, cloning, or unauthorized fuzzing behavior is present.
- [ ] Test location and applicable radio/legal restrictions documented.

## Resource boundaries

- [ ] Maximum input file size defined.
- [ ] Maximum record/frame/item count defined.
- [ ] Maximum repeat count defined.
- [ ] Maximum session duration defined.
- [ ] Free storage and SD-card health checked.
- [ ] Allocation failure, malformed input, and cancellation behavior documented.
- [ ] Expected fail-closed result defined.

## Host-facing BadUSB review

- [ ] Script visibly opens only the intended text editor.
- [ ] Script contains no command shell or terminal invocation.
- [ ] Script contains no elevation, hidden windows, encoded commands, or remote destinations.
- [ ] Script reads no credentials, tokens, cookies, browser data, private keys, or private documents.
- [ ] Script changes no security control, service, startup item, task, policy, or firewall rule.
- [ ] Output filename starts with `cyberflipper_`.
- [ ] Keyboard layout and timing validated on an isolated host.

## Recovery evidence

- [ ] Normal exit confirmed.
- [ ] Cancellation path confirmed.
- [ ] Device restart confirmed.
- [ ] Application settings restored.
- [ ] Rollback to known-good firmware/app confirmed where applicable.
- [ ] Unexpected reboot, hang, heat, storage growth, or file corruption recorded.
- [ ] Crash notes contain no secrets or third-party data.

## Publication gate

- [ ] Independent reviewer inspected every keystroke and document.
- [ ] Licensing/source notes are complete.
- [ ] Security-sensitive or radio-related material remains draft-only.
- [ ] Named reviewer approved merge and public release.

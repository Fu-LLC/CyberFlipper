# CyberFlipper Level 090 — Evidence Communications Tabletop

## Purpose

Level 090 turns a CyberFlipper lab run into a clean evidence and communications handoff. It is designed for authorized labs, classrooms, owned workstations, and internal security exercises where the user needs a visible worksheet instead of an opaque script.

This level does not perform exploitation, collection of private secrets, hidden execution, persistence, or destructive activity. It creates a local `cyberflipper_` worksheet and prompts the operator to document what was tested, why it was authorized, what artifacts were produced, what changed, and who needs to review it.

## Files

| File | Platform | Output |
|---|---|---|
| `cf_l090_windows_evidence_comms_tabletop.txt` | Windows | Desktop `cyberflipper_l090_windows_evidence_comms.txt` |
| `cf_l090_linux_evidence_comms_tabletop.txt` | Linux | Home directory `cyberflipper_l090_linux_evidence_comms.txt` |
| `cf_l090_macos_evidence_comms_tabletop.txt` | macOS | Desktop `cyberflipper_l090_macos_evidence_comms.txt` |

## Expected output

Each script opens a visible local shell or editor and creates a worksheet containing:

- operator and host context
- authorization statement
- lab scope
- firmware/app/SD-card provenance fields
- observed behavior
- evidence inventory
- release decision
- communications draft notes
- reviewer signoff fields

## Defensive value

Level 090 supports:

- audit-ready security education
- release review before publishing lab content
- SD-card provenance discipline
- firmware/app compatibility review
- tabletop incident communications
- safe handling of outputs from earlier levels

## Review requirements

Human review is required before merge or public release because these files open local host tools and create reports. Reviewers should confirm:

```text
[ ] Commands are visible.
[ ] Output uses the cyberflipper_ prefix.
[ ] No hidden execution.
[ ] No secrets collection.
[ ] No destructive commands.
[ ] No privilege-abuse logic.
[ ] No radio or network misuse.
[ ] Worksheet fields are appropriate for classroom, owned-lab, or approved-scope use.
```

## Operator note

Run only on systems you own, administer, or have written permission to test. Keep the generated worksheet with the lab notes, not as proof of compromise, but as proof of scope, method, and review discipline.

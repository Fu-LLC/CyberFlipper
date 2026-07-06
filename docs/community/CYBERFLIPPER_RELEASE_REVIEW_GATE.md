# CyberFlipper Release Review Gate

## Purpose

This gate prevents safe education material from drifting into unclear or unsafe public release. It applies to new CyberFlipper levels, daily watch notes, SD-card folders, BadUSB-style scripts, hardware notes, firmware notes, and community-source conversions.

## Review categories

| Category | Default decision | Reason |
|---|---|---|
| Documentation-only source notes | Publish after copyedit | Low operational risk. |
| Compatibility matrices | Publish after source check | Useful for reproducibility. |
| SD-card provenance checklists | Publish after path validation | Low risk when no execution is included. |
| Visible local worksheets | Hold for human review | Host-side interaction needs review. |
| Local inventory scripts | Hold for human review | May expose sensitive local context if poorly scoped. |
| Firmware/app install notes | Hold for human review | Version mismatch can break workflows. |
| Radio, reader, or hardware lab notes | Hold for human review | Requires lawful scope and safety confirmation. |
| Third-party community conversions | Hold for attribution review | Licensing and context matter. |

## Required checks

```text
[ ] Source is named.
[ ] License or attribution need is noted.
[ ] Date of source review is recorded.
[ ] Content is framed for owned, administered, classroom, or written-scope systems.
[ ] No hidden execution.
[ ] No private-secret collection.
[ ] No destructive behavior.
[ ] No privilege-abuse logic.
[ ] No unauthorized network or radio activity.
[ ] Output files use cyberflipper_ prefix when applicable.
[ ] Human review is required for host-side tools.
```

## PR body template

```markdown
## Summary

Adds CyberFlipper Level XXX content for <defensive topic>.

## Source notes

- Official docs / firmware / app catalog:
- Community examples reviewed:
- CISA/NIST/vendor guidance reviewed:

## Safety posture

- Visible local output only: yes/no
- Host-side tools launched: yes/no
- Network/radio activity: no unless explicitly scoped and reviewed
- Secrets collection: no
- Destructive commands: no

## Human review required

Yes. This PR remains draft until a maintainer confirms scope, source attribution, and safety checks.
```

## Merge standard

A maintainer may merge only when the PR is understandable to a non-author reviewer and the file paths, outputs, source notes, and release decision are clear. If a reviewer cannot quickly tell what the file does and why it is safe, the correct decision is `REVISE` or `HOLD`.

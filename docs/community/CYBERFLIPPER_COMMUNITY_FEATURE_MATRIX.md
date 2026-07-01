# CyberFlipper Community Feature Matrix

CyberFlipper is a training and documentation pack for authorized local workstation review. It should provide clear scope, visible outputs, repeatable lab workflows, and review gates.

## Script Families

| Family | Purpose | Output | Review level |
|---|---|---|---|
| Local Inventory | OS, hardware, and installed application visibility | Desktop text or HTML report | Standard review |
| Browser Posture | Browser presence, version, policy keys, and update-service visibility | Desktop report | Standard review |
| Communications Stack | Meeting, chat, and email client inventory | Desktop report | Standard review |
| Project Stack | Note-taking, planning, and productivity tool inventory | Desktop report | Standard review |
| Security Baseline | Defender, firewall, encryption, and backup-service visibility | Desktop report | Human review |
| Response Starter | Recent system events, running services, listening sockets, and process summary | Desktop report | Human review |
| Linux Desktop Audit | Package/tool presence, services, firewall status, network interfaces | Home-folder report | Human review |
| macOS Workstation Audit | System version, application presence, firewall state, application inventory | Desktop report | Human review |

## Level Ladder

| Level | Theme | Expected skill |
|---|---|---|
| 001 | Host identity and security baseline | Learn visible local reporting |
| 002 | Browser and document-tool inventory | Understand software exposure |
| 003 | Event and application review | Build audit habits |
| 004 | Business-stack visibility | Map common desktop workflows |
| 005 | Communications and workflow inventory | Tie software to response scoping |
| 006 | Community Pack: desktop review suite | Produce polished multi-category reports |
| 007-020 | Defensive endpoint review | Services, logs, updates, firewall, encryption |
| 021-040 | Response automation | Visible triage, timeline starters, handoff notes |
| 041-060 | Detection engineering | Local rule checks, mock alerts, benign lab telemetry |
| 061-080 | Collection discipline | Safe file metadata, hash manifests, chain-of-custody notes |
| 081-099 | Research harnesses | Sandboxed analysis setup and mitigation-first study notes |

## Publication Rules

1. Every script must create a visible local report.
2. Every command window must remain visible to the user.
3. Every output file must use a `cyberflipper_` prefix.
4. Every script must have a matching README entry.
5. Every level must include detection and mitigation guidance.
6. Human review is required before publication.

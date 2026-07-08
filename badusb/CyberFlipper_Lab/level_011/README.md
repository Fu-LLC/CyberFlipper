# CyberFlipper Level 011 - Operator Scope Card

Level 011 starts the continuous 011-019 workstation-basics sequence. It creates visible local worksheets for authorized testing scope, operator notes, host context, and expected output.

## Files

- `cf_l011_windows_operator_scope_card.txt`
- `cf_l011_linux_operator_scope_card.txt`
- `cf_l011_macos_operator_scope_card.txt`

## Expected Output

Each script creates a visible `cyberflipper_l011_operator_scope_card` worksheet on the Desktop or home folder.

## Defensive Value

Before any HID testing, a defender needs written scope, host identity, test window, operator name, and expected artifact location. Level 011 makes that habit executable.

## Review Requirement

Human review is required before merge because the scripts open local command tools and create worksheet files.

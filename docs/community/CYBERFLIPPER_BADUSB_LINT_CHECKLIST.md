# CyberFlipper BadUSB Lint Checklist

Use this checklist before adding or merging any `.txt` BadUSB training file.

## File Requirements

```text
[ ] File extension is .txt.
[ ] File name starts with cf_lXXX_ where XXX is the level number.
[ ] File name includes platform: windows, linux, or macos.
[ ] File lives under badusb/CyberFlipper_Lab/level_XXX/.
[ ] Matching level README exists.
[ ] Output file starts with cyberflipper_ prefix.
```

## Syntax Requirements

Preferred commands:

```text
REM
DELAY
STRING
ENTER
GUI
CTRL
ALT
SHIFT
TAB
ESC
```

Review items:

```text
[ ] Uses visible shell/editor/terminal flow.
[ ] Uses reasonable DELAY values.
[ ] Avoids fragile keyboard-layout assumptions where possible.
[ ] Avoids long one-line commands when a worksheet approach is safer.
[ ] Opens the final report visibly when practical.
```

## Safety Requirements

```text
[ ] No credential handling.
[ ] No token, cookie, browser-profile, password-manager, or VPN-secret handling.
[ ] No hidden execution.
[ ] No persistence.
[ ] No destructive actions.
[ ] No system policy changes.
[ ] No remote callbacks.
[ ] No third-party target testing.
[ ] No RF transmission behavior.
[ ] No Wi-Fi cracking or credential capture behavior.
```

## Output Requirements

```text
[ ] Report includes title and date.
[ ] Report identifies lab scope.
[ ] Report explains what was checked.
[ ] Report provides reviewer notes or next actions.
[ ] Report is local-only.
```

## Human Review Triggers

Require human review when a file:

```text
[ ] Launches PowerShell, Terminal, cmd, bash, zsh, or sh.
[ ] Enumerates files, apps, services, logs, USB devices, or policies.
[ ] Creates hashes or manifests.
[ ] References firmware, radio, NFC, RFID, HID, GPIO, or app compatibility.
[ ] Is derived from external community examples.
```

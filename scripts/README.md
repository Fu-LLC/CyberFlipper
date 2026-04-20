# scripts/

Automation scripts for CyberFlipper.

| Script | Purpose |
|---|---|
| `generate_cve_payloads.py` | Fetches CISA KEV JSON and generates DuckyScript BadUSB payloads. Run by `daily-cve-badusb.yml`. |
| `build_update.sh` | Builds the Flipper OTA `.tgz`, SD card zip, and full package locally on Unix-like systems. |
| `build_update.py` | Cross-platform local build helper for Windows, macOS, and Linux. |

## Manual run

```bash
pip install requests
python scripts/generate_cve_payloads.py

# Override date and limit
DATE_OVERRIDE=2026-04-18 MAX_PAYLOADS=5 python scripts/generate_cve_payloads.py
```

## Local build

```bash
bash scripts/build_update.sh
```

## Local build on Windows

```powershell
python scripts/build_update.py
```

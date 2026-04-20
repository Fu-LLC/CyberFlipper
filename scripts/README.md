# scripts/

Automation scripts for CyberFlipper.

| Script | Purpose |
|---|---|
| `generate_cve_payloads.py` | Fetches CISA KEV JSON and generates DuckyScript BadUSB payloads. Run by `daily-cve-badusb.yml`. |

## Manual run

```bash
pip install requests
python scripts/generate_cve_payloads.py

# Override date and limit
DATE_OVERRIDE=2026-04-18 MAX_PAYLOADS=5 python scripts/generate_cve_payloads.py
```

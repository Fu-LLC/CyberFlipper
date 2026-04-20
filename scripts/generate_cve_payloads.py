#!/usr/bin/env python3
"""
CyberFlipper Daily CVE BadUSB Payload Generator
================================================
Fetches the CISA Known Exploited Vulnerabilities (KEV) catalog and generates
DuckyScript / Flipper BadUSB payloads for newly-added CVEs.

INTENDED FOR AUTHORIZED SECURITY RESEARCH IN CONTROLLED LAB ENVIRONMENTS ONLY.
"""

import json
import os
import re
import sys
import textwrap
from datetime import date, timedelta
from pathlib import Path

try:
    import requests
except ImportError:
    sys.exit("requests is required: pip install requests")

# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------
KEV_URL    = "https://www.cisa.gov/sites/default/files/feeds/known_exploited_vulnerabilities.json"
REPO_ROOT  = Path(__file__).resolve().parent.parent
BADUSB_DIR = REPO_ROOT / "badusb" / "CVE_Daily"
INDEX_FILE = REPO_ROOT / "badusb" / "CVE_Index.md"

DATE_OVERRIDE = os.environ.get("DATE_OVERRIDE", "").strip()
MAX_PAYLOADS  = int(os.environ.get("MAX_PAYLOADS", "10"))

TARGET_DATE = (
    date.fromisoformat(DATE_OVERRIDE) if DATE_OVERRIDE else date.today()
)

# ---------------------------------------------------------------------------
# OS detection heuristics based on vendor / product keywords
# ---------------------------------------------------------------------------
WINDOWS_KEYWORDS = {
    "microsoft", "windows", "office", "excel", "word", "outlook", "edge",
    "internet explorer", "iis", "active directory", "sharepoint", "exchange",
    ".net", "visual studio", "powershell", "wmi", "ntlm", "msmq", "rdp",
    "remote desktop", "winrar", "7-zip", "adobe acrobat", "adobe reader",
    "adobe flash", "foxit",
}
LINUX_KEYWORDS = {
    "linux", "ubuntu", "debian", "centos", "redhat", "rhel", "fedora",
    "kernel", "apache", "nginx", "openssh", "openssl", "bash", "sudo",
    "samba", "nfs", "cifs", "glibc", "cups", "polkit", "dbus",
}
MACOS_KEYWORDS = {
    "apple", "macos", "osx", "os x", "safari", "webkit", "xcode",
    "icloud", "macos ventura", "macos sonoma", "macos monterey",
}
NETWORK_KEYWORDS = {
    "cisco", "juniper", "fortinet", "palo alto", "sonicwall", "vmware",
    "f5", "pulse secure", "citrix", "vpn", "router", "firewall",
    "switch", "nas", "qnap", "synology", "netgear", "d-link", "tp-link",
}


def detect_os(vuln: dict) -> list[str]:
    """Return a list of applicable OS targets based on vendor/product text."""
    text = " ".join([
        vuln.get("vendorProject", ""),
        vuln.get("product", ""),
        vuln.get("shortDescription", ""),
        vuln.get("vulnerabilityName", ""),
    ]).lower()

    targets = []
    if any(k in text for k in WINDOWS_KEYWORDS):
        targets.append("windows")
    if any(k in text for k in LINUX_KEYWORDS):
        targets.append("linux")
    if any(k in text for k in MACOS_KEYWORDS):
        targets.append("macos")
    if any(k in text for k in NETWORK_KEYWORDS):
        targets.append("network")

    # Fall back to all common desktop OSes when detection is ambiguous
    if not targets:
        targets = ["windows", "linux"]
    return targets


# ---------------------------------------------------------------------------
# Payload templates
# ---------------------------------------------------------------------------

def _wrap_rem(text: str, width: int = 68) -> str:
    """Wrap long text into multiple REM comment lines."""
    lines = textwrap.wrap(text, width)
    return "\n".join(f"REM {l}" for l in lines)


def payload_windows(vuln: dict) -> str:
    cve_id   = vuln["cveID"]
    vendor   = vuln.get("vendorProject", "Unknown Vendor")
    product  = vuln.get("product", "Unknown Product")
    desc     = vuln.get("shortDescription", "No description available.")
    cwe      = vuln.get("cweID", "N/A")
    date_add = vuln.get("dateAdded", "Unknown")
    req_act  = vuln.get("requiredAction", "Apply vendor patch immediately.")
    safe_id  = re.sub(r"[^A-Za-z0-9_-]", "_", cve_id)

    desc_rem = _wrap_rem(desc)
    req_rem  = _wrap_rem(req_act)

    return f"""\
REM =========================================================
REM  CyberFlipper BadUSB — CVE Awareness Payload
REM  CVE   : {cve_id}
REM  Vendor: {vendor}
REM  Prod  : {product}
REM  CWE   : {cwe}
REM  Added : {date_add}
REM  OS    : Windows
REM ---------------------------------------------------------
{desc_rem}
REM ---------------------------------------------------------
{req_rem}
REM  FOR AUTHORIZED LAB USE ONLY
REM =========================================================

DELAY 1000

REM Open Run dialog
GUI r
DELAY 600

REM Launch hidden PowerShell
STRING powershell -WindowStyle Hidden -NonInteractive -NoProfile -Command "$host.UI.RawUI.WindowTitle='{safe_id}'; Write-Host '[CyberFlipper] CVE Awareness: {cve_id}' -ForegroundColor Cyan; Write-Host 'Vendor: {vendor}' -ForegroundColor Yellow; Write-Host 'Product: {product}' -ForegroundColor Yellow; Write-Host 'Status: CHECKING PATCH LEVEL' -ForegroundColor Green; $os=[System.Environment]::OSVersion.VersionString; $user=[System.Security.Principal.WindowsIdentity]::GetCurrent().Name; $patch=(Get-HotFix | Sort-Object -Property InstalledOn -Descending | Select-Object -First 1 | ForEach-Object {{$_.HotFixID}}); Write-Host ('OS: '+$os) -ForegroundColor Gray; Write-Host ('User: '+$user) -ForegroundColor Gray; Write-Host ('Latest patch: '+$patch) -ForegroundColor Gray; pause"
ENTER
"""


def payload_linux(vuln: dict) -> str:
    cve_id   = vuln["cveID"]
    vendor   = vuln.get("vendorProject", "Unknown Vendor")
    product  = vuln.get("product", "Unknown Product")
    desc     = vuln.get("shortDescription", "No description available.")
    date_add = vuln.get("dateAdded", "Unknown")
    req_act  = vuln.get("requiredAction", "Apply vendor patch immediately.")
    cwe      = vuln.get("cweID", "N/A")

    desc_rem = _wrap_rem(desc)
    req_rem  = _wrap_rem(req_act)

    return f"""\
REM =========================================================
REM  CyberFlipper BadUSB — CVE Awareness Payload
REM  CVE   : {cve_id}
REM  Vendor: {vendor}
REM  Prod  : {product}
REM  CWE   : {cwe}
REM  Added : {date_add}
REM  OS    : Linux
REM ---------------------------------------------------------
{desc_rem}
REM ---------------------------------------------------------
{req_rem}
REM  FOR AUTHORIZED LAB USE ONLY
REM =========================================================

DELAY 1000

REM Try common Linux terminal emulators
CTRL-ALT t
DELAY 1200

STRING echo -e "\\033[1;36m[CyberFlipper] CVE Awareness: {cve_id}\\033[0m"
ENTER
DELAY 300

STRING echo -e "\\033[1;33mVendor: {vendor} | Product: {product}\\033[0m"
ENTER
DELAY 300

STRING echo -e "\\033[0;32mChecking patch status...\\033[0m"
ENTER
DELAY 300

STRING uname -a && id && echo "--- Installed packages (sample) ---" && (dpkg -l 2>/dev/null | grep -i '{vendor.lower()[:20]}' || rpm -qa 2>/dev/null | grep -i '{vendor.lower()[:20]}' || echo "Package manager not detected") && echo "[CyberFlipper] Audit complete. Verify {cve_id} patch status."
ENTER
"""


def payload_macos(vuln: dict) -> str:
    cve_id   = vuln["cveID"]
    vendor   = vuln.get("vendorProject", "Unknown Vendor")
    product  = vuln.get("product", "Unknown Product")
    desc     = vuln.get("shortDescription", "No description available.")
    date_add = vuln.get("dateAdded", "Unknown")
    cwe      = vuln.get("cweID", "N/A")

    desc_rem = _wrap_rem(desc)

    return f"""\
REM =========================================================
REM  CyberFlipper BadUSB — CVE Awareness Payload
REM  CVE   : {cve_id}
REM  Vendor: {vendor}
REM  Prod  : {product}
REM  CWE   : {cwe}
REM  Added : {date_add}
REM  OS    : macOS
REM ---------------------------------------------------------
{desc_rem}
REM  FOR AUTHORIZED LAB USE ONLY
REM =========================================================

DELAY 1000

REM Open Terminal via Spotlight
GUI SPACE
DELAY 700

STRING Terminal
ENTER
DELAY 1200

STRING echo "\\033[1;36m[CyberFlipper] CVE Awareness: {cve_id}\\033[0m" && sw_vers && id && system_profiler SPSoftwareDataType | grep -E "System Version|Kernel" && echo "[CyberFlipper] Verify {cve_id} is patched."
ENTER
"""


def payload_network(vuln: dict) -> str:
    cve_id   = vuln["cveID"]
    vendor   = vuln.get("vendorProject", "Unknown Vendor")
    product  = vuln.get("product", "Unknown Product")
    desc     = vuln.get("shortDescription", "No description available.")
    date_add = vuln.get("dateAdded", "Unknown")
    cwe      = vuln.get("cweID", "N/A")

    desc_rem = _wrap_rem(desc)

    return f"""\
REM =========================================================
REM  CyberFlipper BadUSB — CVE Awareness Payload
REM  CVE   : {cve_id}
REM  Vendor: {vendor}
REM  Prod  : {product}
REM  CWE   : {cwe}
REM  Added : {date_add}
REM  OS    : Windows (network device management console)
REM ---------------------------------------------------------
{desc_rem}
REM  NOTE: This payload targets a workstation used to manage
REM  the affected network device. FOR AUTHORIZED LAB USE ONLY.
REM =========================================================

DELAY 1000

REM Open Run dialog
GUI r
DELAY 600

STRING powershell -WindowStyle Hidden -NonInteractive -NoProfile -Command "$host.UI.RawUI.WindowTitle='{cve_id}-NET'; Write-Host '[CyberFlipper] Network CVE: {cve_id}' -ForegroundColor Cyan; Write-Host 'Vendor: {vendor} | Device: {product}' -ForegroundColor Yellow; Write-Host 'Scanning local network for device fingerprints...' -ForegroundColor Green; $gw=(Get-NetRoute -DestinationPrefix '0.0.0.0/0' | Sort-Object RouteMetric | Select-Object -First 1).NextHop; Write-Host ('Default Gateway: '+$gw) -ForegroundColor Gray; Test-NetConnection -ComputerName $gw -Port 443 -WarningAction SilentlyContinue | Select-Object ComputerName,TcpTestSucceeded | Format-List; pause"
ENTER
"""


PAYLOAD_GENERATORS = {
    "windows": payload_windows,
    "linux":   payload_linux,
    "macos":   payload_macos,
    "network": payload_network,
}


# ---------------------------------------------------------------------------
# Index update helpers
# ---------------------------------------------------------------------------

def update_index(entries: list[dict]) -> None:
    """Append new CVE entries to CVE_Index.md, creating it if absent."""
    header = """\
# CyberFlipper BadUSB — CVE Daily Index

> Auto-generated from the [CISA KEV Catalog](https://www.cisa.gov/known-exploited-vulnerabilities-catalog).  
> All payloads are for **authorized security research in controlled lab environments only**.

| Date Added | CVE ID | Vendor | Product | OS Targets | Files |
|---|---|---|---|---|---|
"""
    if not INDEX_FILE.exists():
        INDEX_FILE.write_text(header, encoding="utf-8")

    existing = INDEX_FILE.read_text(encoding="utf-8")
    rows = []
    for e in entries:
        cve     = e["cveID"]
        vendor  = e.get("vendorProject", "")
        product = e.get("product", "")
        added   = e.get("dateAdded", "")
        targets = ", ".join(e["_targets"])
        files   = " ".join(
            f"[{t.upper()}](CVE_Daily/{added}/{cve}_{t.upper()}.txt)"
            for t in e["_targets"]
        )
        row = f"| {added} | {cve} | {vendor} | {product} | {targets} | {files} |"
        if row not in existing:
            rows.append(row)

    if rows:
        INDEX_FILE.write_text(existing + "\n".join(rows) + "\n", encoding="utf-8")


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main() -> None:
    print(f"[*] Target date  : {TARGET_DATE}")
    print(f"[*] Max payloads : {MAX_PAYLOADS}")
    print(f"[*] Fetching CISA KEV catalog from {KEV_URL} ...")

    resp = requests.get(KEV_URL, timeout=30)
    resp.raise_for_status()
    data = resp.json()

    vulns = data.get("vulnerabilities", [])
    print(f"[*] Total CVEs in catalog: {len(vulns)}")

    # Filter to CVEs added on the target date
    # Look back up to 3 days if nothing new (weekends / holidays)
    candidates = []
    for days_back in range(4):
        check_date = TARGET_DATE - timedelta(days=days_back)
        candidates = [
            v for v in vulns
            if v.get("dateAdded", "") == str(check_date)
        ]
        if candidates:
            print(f"[*] Found {len(candidates)} CVE(s) added on {check_date}")
            break

    if not candidates:
        print("[!] No new CVEs found in the last 3 days. Nothing to generate.")
        return

    # Limit and sort by dateAdded descending
    candidates = sorted(candidates, key=lambda v: v.get("dateAdded", ""), reverse=True)
    candidates = candidates[:MAX_PAYLOADS]

    generated = []
    for vuln in candidates:
        cve_id   = vuln["cveID"]
        added    = vuln.get("dateAdded", str(TARGET_DATE))
        targets  = detect_os(vuln)
        vuln["_targets"] = targets

        out_dir = BADUSB_DIR / added
        out_dir.mkdir(parents=True, exist_ok=True)

        for os_target in targets:
            gen_fn  = PAYLOAD_GENERATORS[os_target]
            content = gen_fn(vuln)
            fname   = f"{cve_id}_{os_target.upper()}.txt"
            fpath   = out_dir / fname

            if fpath.exists():
                print(f"  [=] Already exists: {fpath.relative_to(REPO_ROOT)}")
                continue

            fpath.write_text(content, encoding="utf-8", newline="\n")
            print(f"  [+] Written: {fpath.relative_to(REPO_ROOT)}")

        generated.append(vuln)

    update_index(generated)
    print(f"\n[*] Done — {len(generated)} CVE(s) processed.")


if __name__ == "__main__":
    main()

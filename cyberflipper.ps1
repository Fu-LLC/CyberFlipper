<#
.SYNOPSIS
    CYBERFLIPPER v1.0.0 — PowerShell Deployment & Management Toolkit
.DESCRIPTION
    Provides automated firmware packaging, SD card flashing, app validation,
    and device management for the CYBERFLIPPER Flipper Zero build.
.NOTES
    Author: Personfu @ FLLC
    Version: 1.0.0
#>

param(
    [Parameter(Position=0)]
    [ValidateSet("package","flash","validate","inventory","checksum","help")]
    [string]$Command = "help"
)

$Version = "1.0.0"
$ReleaseName = "CYBERFLIPPER-v$Version"
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$FirmwareFiles = @("firmware.dfu","radio.bin","splash.bin","updater.bin","update.fuf","resources.tar")

function Show-Banner {
    Write-Host ""
    Write-Host "  ╔══════════════════════════════════════╗" -ForegroundColor Cyan
    Write-Host "  ║   CYBERFLIPPER v$Version              ║" -ForegroundColor Cyan
    Write-Host "  ║   PowerShell Management Toolkit      ║" -ForegroundColor Cyan
    Write-Host "  ╚══════════════════════════════════════╝" -ForegroundColor Cyan
    Write-Host ""
}

function Build-Package {
    Show-Banner
    Write-Host "  [*] Building release package..." -ForegroundColor Yellow
    
    $distDir = Join-Path $ScriptDir "dist"
    $pkgDir = Join-Path $distDir "CYBERFLIPPER"
    
    if(Test-Path $distDir) { Remove-Item $distDir -Recurse -Force }
    New-Item -ItemType Directory -Path $pkgDir -Force | Out-Null
    
    $totalSize = 0
    foreach($f in $FirmwareFiles) {
        $src = Join-Path $ScriptDir $f
        if(Test-Path $src) {
            $size = (Get-Item $src).Length
            $totalSize += $size
            Copy-Item $src $pkgDir
            Write-Host "  [+] $f ($([math]::Round($size/1MB,2)) MB)" -ForegroundColor Green
        } else {
            Write-Host "  [!] $f (missing)" -ForegroundColor Red
        }
    }
    
    # Copy app sources
    $appDirs = @("apps","badusb","subghz","infrared","nfc","dolphin","lfrfid","u2f")
    foreach($dir in $appDirs) {
        $src = Join-Path $ScriptDir $dir
        if(Test-Path $src) {
            $dest = Join-Path $pkgDir $dir
            Copy-Item $src $dest -Recurse
            $count = (Get-ChildItem $src -Recurse -File).Count
            Write-Host "  [+] $dir/ ($count files)" -ForegroundColor Green
        }
    }
    
    # Create .tgz
    $tgzPath = Join-Path $distDir "$ReleaseName.tgz"
    Write-Host ""
    Write-Host "  [*] Compressing to $ReleaseName.tgz..." -ForegroundColor Yellow
    
    tar -czf $tgzPath -C $distDir "CYBERFLIPPER"
    
    $tgzSize = (Get-Item $tgzPath).Length
    $hash = (Get-FileHash $tgzPath -Algorithm SHA256).Hash
    
    Write-Host ""
    Write-Host "  ╔══════════════════════════════════════╗" -ForegroundColor Green
    Write-Host "  ║  PACKAGE BUILT SUCCESSFULLY          ║" -ForegroundColor Green
    Write-Host "  ╚══════════════════════════════════════╝" -ForegroundColor Green
    Write-Host "  File: $tgzPath" -ForegroundColor White
    Write-Host "  Size: $([math]::Round($tgzSize/1MB,2)) MB" -ForegroundColor White
    Write-Host "  SHA256: $hash" -ForegroundColor Gray
    Write-Host ""
}

function Flash-SDCard {
    Show-Banner
    Write-Host "  [*] Detecting removable drives..." -ForegroundColor Yellow
    
    $drives = Get-WmiObject Win32_LogicalDisk | Where-Object { $_.DriveType -eq 2 }
    
    if($drives) {
        Write-Host "  [+] Found removable drives:" -ForegroundColor Green
        $i = 1
        foreach($d in $drives) {
            Write-Host "      [$i] $($d.DeviceID) — $($d.VolumeName) ($([math]::Round($d.Size/1GB,1)) GB)" -ForegroundColor White
            $i++
        }
        Write-Host ""
        $choice = Read-Host "  Select drive number"
        if($choice -ge 1 -and $choice -le $drives.Count) {
            $target = $drives[$choice-1].DeviceID + "\update\CYBERFLIPPER"
            Write-Host "  [*] Flashing to $target..." -ForegroundColor Yellow
            
            if(!(Test-Path $target)) { New-Item -ItemType Directory -Path $target -Force | Out-Null }
            
            foreach($f in $FirmwareFiles) {
                $src = Join-Path $ScriptDir $f
                if(Test-Path $src) {
                    Copy-Item $src $target -Force
                    Write-Host "  [+] $f" -ForegroundColor Green
                }
            }
            Write-Host ""
            Write-Host "  [SUCCESS] Firmware written. Eject SD and run update.fuf on Flipper." -ForegroundColor Green
        }
    } else {
        Write-Host "  [!] No removable drives found" -ForegroundColor Red
        Write-Host "  [!] Insert your Flipper's SD card and try again" -ForegroundColor Yellow
    }
}

function Show-Inventory {
    Show-Banner
    Write-Host "  === APP INVENTORY ===" -ForegroundColor Cyan
    Write-Host ""
    
    $categories = Get-ChildItem (Join-Path $ScriptDir "apps") -Directory
    $totalApps = 0
    
    foreach($cat in $categories) {
        $apps = Get-ChildItem $cat.FullName -Directory | Where-Object {
            (Get-ChildItem $_.FullName -Filter "*.c" -EA 0).Count -gt 0 -or
            (Get-ChildItem $_.FullName -Filter "application.fam" -EA 0).Count -gt 0
        }
        if($apps.Count -gt 0) {
            Write-Host "  $($cat.Name): $($apps.Count) apps" -ForegroundColor Yellow
            foreach($a in $apps) {
                Write-Host "    [+] $($a.Name)" -ForegroundColor Gray
                $totalApps++
            }
            Write-Host ""
        }
    }
    
    $badusb = (Get-ChildItem (Join-Path $ScriptDir "badusb") -Recurse -Filter "*.txt" -EA 0).Count
    Write-Host "  BadUSB Payloads: $badusb" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "  TOTAL: $totalApps FAP apps + $badusb BadUSB payloads" -ForegroundColor Green
}

function Get-Checksums {
    Show-Banner
    Write-Host "  === SHA-256 CHECKSUMS ===" -ForegroundColor Cyan
    Write-Host ""
    foreach($f in $FirmwareFiles) {
        $path = Join-Path $ScriptDir $f
        if(Test-Path $path) {
            $hash = (Get-FileHash $path -Algorithm SHA256).Hash
            $size = [math]::Round((Get-Item $path).Length/1MB,2)
            Write-Host "  $($f.PadRight(20)) $hash ($size MB)" -ForegroundColor White
        }
    }
    Write-Host ""
}

# Main dispatch
switch($Command) {
    "package"   { Build-Package }
    "flash"     { Flash-SDCard }
    "validate"  { python (Join-Path $ScriptDir "validate_apps.py") }
    "inventory" { Show-Inventory }
    "checksum"  { Get-Checksums }
    "help"      {
        Show-Banner
        Write-Host "  Usage: .\cyberflipper.ps1 <command>" -ForegroundColor White
        Write-Host ""
        Write-Host "  Commands:" -ForegroundColor Cyan
        Write-Host "    package    — Build .tgz release archive"
        Write-Host "    flash      — Flash firmware to SD card"
        Write-Host "    validate   — Validate all FAP applications"
        Write-Host "    inventory  — Display full app inventory"
        Write-Host "    checksum   — Generate SHA-256 checksums"
        Write-Host ""
    }
}

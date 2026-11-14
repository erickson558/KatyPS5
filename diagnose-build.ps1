#!/usr/bin/env pwsh
# KatyPS5 Build Diagnostics
# This script helps diagnose common build environment issues

Write-Host @"
╔════════════════════════════════════════════════════════════════════════════╗
║           KatyPS5 Build Environment Diagnostics                           ║
╚════════════════════════════════════════════════════════════════════════════╝
"@ -ForegroundColor Cyan

Write-Host ""

# ============================================================================
# CHECK BUILD TOOLS
# ============================================================================

Write-Host "🔍 Checking Build Tools..." -ForegroundColor Yellow
Write-Host ""

$tools = @{
    "CMake" = @{
        command = "cmake"
        version_flag = "--version"
        required = $true
    }
    "Ninja" = @{
        command = "ninja"
        version_flag = "--version"
        required = $true
    }
    "Git" = @{
        command = "git"
        version_flag = "--version"
        required = $true
    }
}

foreach ($toolName in $tools.Keys) {
    $tool = $tools[$toolName]
    try {
        $output = & $tool.command $tool.version_flag 2>&1
        if ($LASTEXITCODE -eq 0) {
            Write-Host "✓ ${toolName}: FOUND" -ForegroundColor Green
            Write-Host "  $($output[0])" -ForegroundColor Gray
        } else {
            Write-Host "✗ ${toolName}: ERROR" -ForegroundColor Red
        }
    } catch {
        if ($tool.required) {
            Write-Host "✗ ${toolName}: NOT FOUND (required)" -ForegroundColor Red
        } else {
            Write-Host "ℹ ${toolName}: NOT FOUND (optional)" -ForegroundColor Yellow
        }
    }
}

Write-Host ""

# ============================================================================
# CHECK Qt6
# ============================================================================

Write-Host "🔍 Checking Qt6 Installation..." -ForegroundColor Yellow
Write-Host ""

if ($Env:Qt6_DIR) {
    Write-Host "✓ Qt6_DIR is set: $Env:Qt6_DIR" -ForegroundColor Green
    
    if (Test-Path $Env:Qt6_DIR) {
        Write-Host "  ✓ Directory exists" -ForegroundColor Green
        
        $subdirs = @("bin", "lib", "include", "plugins", "licenses")
        foreach ($subdir in $subdirs) {
            $path = Join-Path $Env:Qt6_DIR $subdir
            if (Test-Path $path) {
                Write-Host "    ✓ $subdir found" -ForegroundColor Green
            } else {
                Write-Host "    ⚠ $subdir NOT found" -ForegroundColor Yellow
            }
        }
    } else {
        Write-Host "  ✗ Directory does NOT exist!" -ForegroundColor Red
    }
} else {
    Write-Host "⚠ Qt6_DIR environment variable is NOT set" -ForegroundColor Yellow
    
    # Try to auto-detect
    Write-Host ""
    Write-Host "Searching for Qt6 installations..." -ForegroundColor Gray
    
    $possiblePaths = @(
        "C:\Qt\6.10.3\msvc2022_64",
        "C:\Qt\6.9.0\msvc2022_64",
        "C:\Qt\6.8.0\msvc2022_64",
        "C:\Qt\6.7.0\msvc2022_64",
        "C:\Qt\6.6.0\msvc2022_64"
    )
    
    $foundPaths = @()
    foreach ($path in $possiblePaths) {
        if (Test-Path $path) {
            $foundPaths += $path
            Write-Host "  Found: $path" -ForegroundColor Green
        }
    }
    
    if ($foundPaths.Count -gt 0) {
        Write-Host ""
        Write-Host "To fix this, run:" -ForegroundColor Yellow
        Write-Host "  `$Env:Qt6_DIR = '$($foundPaths[0])'" -ForegroundColor Gray
        Write-Host "  .\create-local-release.ps1" -ForegroundColor Gray
    } else {
        Write-Host "  ✗ No Qt6 installations found in default locations" -ForegroundColor Red
        Write-Host ""
        Write-Host "To fix this:" -ForegroundColor Yellow
        Write-Host "  1. Download Qt6 from: https://www.qt.io/download" -ForegroundColor Gray
        Write-Host "  2. Install to: C:\Qt\6.10.3\msvc2022_64" -ForegroundColor Gray
        Write-Host "  3. Then run: .\create-local-release.ps1" -ForegroundColor Gray
    }
}

Write-Host ""

# ============================================================================
# CHECK BUILD DIRECTORY
# ============================================================================

Write-Host "🔍 Checking Build Directory..." -ForegroundColor Yellow
Write-Host ""

$projectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$buildDir = Join-Path $projectRoot "_Build"
$windowsBuildDir = Join-Path $buildDir "windows"

Write-Host "Project root: $projectRoot" -ForegroundColor Gray
Write-Host ""

if (Test-Path $buildDir) {
    Write-Host "✓ _Build directory exists" -ForegroundColor Green
    
    if (Test-Path $windowsBuildDir) {
        Write-Host "  ✓ _Build/windows exists" -ForegroundColor Green
        
        $emulatorExe = Join-Path $windowsBuildDir "src\katyEmulator\katy_emulator.exe"
        $launcherExe = Join-Path $windowsBuildDir "src\launcher\launcher.exe"
        
        if (Test-Path $emulatorExe) {
            $size = (Get-Item $emulatorExe).Length / 1MB
            Write-Host "    ✓ katy_emulator.exe found ($([math]::Round($size, 1)) MB)" -ForegroundColor Green
        } else {
            Write-Host "    ✗ katy_emulator.exe NOT found" -ForegroundColor Red
            Write-Host "      Expected: $emulatorExe" -ForegroundColor Gray
        }
        
        if (Test-Path $launcherExe) {
            Write-Host "    ✓ launcher.exe found" -ForegroundColor Green
        } else {
            Write-Host "    ✗ launcher.exe NOT found" -ForegroundColor Red
            Write-Host "      Expected: $launcherExe" -ForegroundColor Gray
        }
    } else {
        Write-Host "  ⚠ _Build/windows does NOT exist" -ForegroundColor Yellow
        Write-Host "    You need to run: .\compile.ps1" -ForegroundColor Gray
    }
} else {
    Write-Host "⚠ _Build directory does NOT exist" -ForegroundColor Yellow
    Write-Host "  You need to run: .\compile.ps1" -ForegroundColor Gray
}

Write-Host ""

# ============================================================================
# CHECK CMakeLists.txt
# ============================================================================

Write-Host "🔍 Checking CMakeLists.txt..." -ForegroundColor Yellow
Write-Host ""

$cmakePath = Join-Path $projectRoot "CMakeLists.txt"
if (Test-Path $cmakePath) {
    Write-Host "✓ CMakeLists.txt found" -ForegroundColor Green
    
    $match = Select-String -Path $cmakePath -Pattern 'project\(.*VERSION\s+([\d.]+)' | Select-Object -First 1
    if ($match) {
        $version = $match.Matches[0].Groups[1].Value
        Write-Host "  Project version: $version" -ForegroundColor Gray
    }
} else {
    Write-Host "✗ CMakeLists.txt NOT found" -ForegroundColor Red
}

Write-Host ""

# ============================================================================
# SUMMARY
# ============================================================================

Write-Host "═════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
Write-Host "📊 Diagnostic Summary" -ForegroundColor Cyan
Write-Host "═════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
Write-Host ""

Write-Host "Next steps:" -ForegroundColor Yellow
Write-Host ""
Write-Host "1. If build tools are missing:" -ForegroundColor White
Write-Host "   .\install-build-tools.ps1" -ForegroundColor Gray
Write-Host ""
Write-Host "2. If Qt6 is missing:" -ForegroundColor White
Write-Host "   • Download from: https://www.qt.io/download" -ForegroundColor Gray
Write-Host "   • Install to default location (C:\Qt\6.10.3\msvc2022_64)" -ForegroundColor Gray
Write-Host ""
Write-Host "3. If binaries are missing:" -ForegroundColor White
Write-Host "   .\compile.ps1" -ForegroundColor Gray
Write-Host ""
Write-Host "4. To create release:" -ForegroundColor White
Write-Host "   .\create-local-release.ps1" -ForegroundColor Gray
Write-Host ""

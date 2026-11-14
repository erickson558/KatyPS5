# Quick Installation Script for KatyPS5 Build Tools
# Installs CMake, Ninja, Git without user interaction
# Run: powershell -ExecutionPolicy Bypass -File quick-install.ps1

Write-Host @"
╔════════════════════════════════════════════════════════════════════════════╗
║              🚀 KatyPS5 - Quick Build Tools Installation                   ║
╚════════════════════════════════════════════════════════════════════════════╝
"@ -ForegroundColor Cyan

# Check Administrator
$isAdmin = [bool]([System.Security.Principal.WindowsIdentity]::GetCurrent().Groups -match "S-1-5-32-544")
if (-not $isAdmin) {
    Write-Host "⚠️  This script needs Administrator privileges" -ForegroundColor Red
    Write-Host "Restarting as Administrator..." -ForegroundColor Yellow
    Start-Process powershell.exe -ArgumentList "-ExecutionPolicy Bypass -File `"$PSCommandPath`"" -Verb RunAs -Wait
    exit
}

Write-Host "✅ Running as Administrator`n" -ForegroundColor Green

# Function to check if tool exists in PATH
function Test-ToolExists($toolName) {
    try {
        & $toolName --version 2>&1 | Out-Null
        return $true
    } catch {
        return $false
    }
}

# Function to install with winget
function Install-Tool($packageId, $displayName) {
    Write-Host "📦 Installing $displayName..." -ForegroundColor Yellow
    
    try {
        $output = & winget install --id=$packageId --exact --silent `
            --accept-source-agreements --accept-package-agreements 2>&1
        
        Write-Host "   ✓ $displayName installation completed" -ForegroundColor Green
        return $true
    } catch {
        Write-Host "   ⚠️  Error during $displayName installation" -ForegroundColor Yellow
        Write-Host "   Error: $_" -ForegroundColor Gray
        return $false
    }
}

# Check if winget exists
Write-Host "🔍 Checking system..." -ForegroundColor Cyan
$hasWinget = $null -ne (Get-Command winget -ErrorAction SilentlyContinue)

if (-not $hasWinget) {
    Write-Host "⚠️  winget not found. Installing App Installer from Microsoft Store..." -ForegroundColor Yellow
    Write-Host "   A Store window will open. Install 'App Installer' and close it." -ForegroundColor Gray
    Start-Process "ms-windows-store://pdp/?productid=9NBLGGH4NNS1" -Wait
    Start-Sleep -Seconds 2
    Write-Host "   After installation, please run this script again." -ForegroundColor Green
    exit
}

Write-Host "✓ winget found`n" -ForegroundColor Green

# Install tools
Write-Host "═══════════════════════════════════════════════════════════════════════════════" -ForegroundColor Blue
Write-Host "Installing Build Tools..." -ForegroundColor Cyan
Write-Host "═══════════════════════════════════════════════════════════════════════════════`n" -ForegroundColor Blue

$tools = @(
    @{ id = "Kitware.CMake"; name = "CMake" },
    @{ id = "Ninja-build.Ninja"; name = "Ninja" },
    @{ id = "Git.Git"; name = "Git" }
)

$installed = 0
foreach ($tool in $tools) {
    if (Install-Tool $tool.id $tool.name) {
        $installed++
    }
}

Write-Host "`n═══════════════════════════════════════════════════════════════════════════════" -ForegroundColor Blue

if ($installed -gt 0) {
    Write-Host @"
✅ Installation Complete!

Next Steps:
───────────

1️⃣  CLOSE AND REOPEN PowerShell
   Herramientas installed need PATH to be refreshed
   
2️⃣  Download Qt6
   Visit: https://www.qt.io/download
   Install Qt 6.10.3 (MSVC 2022 64-bit) to C:\Qt\6.10.3\msvc2022_64

3️⃣  Download Vulkan SDK  
   Visit: https://vulkan.lunarg.com/sdk/home
   Install to default location

4️⃣  Run diagnostics
   .\diagnose-build.ps1

5️⃣  Compile locally
   .\compile.ps1

6️⃣  Create release package
   .\create-local-release.ps1

═══════════════════════════════════════════════════════════════════════════════
"@ -ForegroundColor Green
} else {
    Write-Host "⚠️  No tools were installed. Check the errors above." -ForegroundColor Yellow
}

Write-Host "Press ENTER to close this window..." -ForegroundColor Gray
Read-Host

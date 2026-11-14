# KatyPS5 Build Tools Installation Script
# This script installs all required dependencies for local compilation
# Run as Administrator: Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser

param(
    [Switch]$SkipVisualStudio = $false,
    [Switch]$SkipQt6 = $false,
    [Switch]$SkipVulkan = $false,
    [Switch]$Verbose = $false
)

$ErrorActionPreference = "Continue"
$ProgressPreference = "Continue"

Write-Host @"
╔════════════════════════════════════════════════════════════════════════════╗
║              KatyPS5 Build Tools Installation Script                       ║
║                  Windows - CMake, Ninja, Qt6, Vulkan SDK                   ║
╚════════════════════════════════════════════════════════════════════════════╝
"@ -ForegroundColor Cyan

# Check if running as Administrator
$isAdmin = [bool]([System.Security.Principal.WindowsIdentity]::GetCurrent().Groups -match "S-1-5-32-544")
if (-not $isAdmin) {
    Write-Host "⚠️  This script must run as Administrator" -ForegroundColor Red
    Write-Host "Please restart PowerShell as Administrator and try again"
    exit 1
}

Write-Host "`n✅ Running as Administrator`n" -ForegroundColor Green

# Function to check if a command exists
function Test-CommandExists($command) {
    try {
        $null = & $command --version 2>&1
        return $true
    } catch {
        return $false
    }
}

# Function to download and install using winget
function Install-WithWinget($packageId, $name) {
    Write-Host "📦 Installing $name..." -ForegroundColor Yellow
    
    if (-not (Test-CommandExists "winget")) {
        Write-Host "❌ winget not found. Installing from Microsoft Store..." -ForegroundColor Yellow
        Start-Process "ms-windows-store://pdp/?productid=9NBLGGH4NNS1" -Wait
        Write-Host "⚠️  Please install App Installer from Microsoft Store, then run this script again"
        return $false
    }
    
    try {
        & winget install --id=$packageId --exact --silent --accept-source-agreements --accept-package-agreements
        if ($LASTEXITCODE -eq 0) {
            Write-Host "✅ $name installed successfully" -ForegroundColor Green
            return $true
        } else {
            Write-Host "⚠️  $name installation may have issues (exit code: $LASTEXITCODE)" -ForegroundColor Yellow
            return $true
        }
    } catch {
        Write-Host "❌ Failed to install $name" -ForegroundColor Red
        return $false
    }
}

# Function to download and install from GitHub (for Ninja)
function Install-NinjaFromGithub {
    Write-Host "📦 Downloading Ninja from GitHub..." -ForegroundColor Yellow
    
    $url = "https://github.com/ninja-build/ninja/releases/latest/download/ninja-win.zip"
    $tempPath = "$env:TEMP\ninja-win.zip"
    $installPath = "C:\tools\ninja"
    
    try {
        # Create tools directory if it doesn't exist
        if (-not (Test-Path $installPath)) {
            New-Item -ItemType Directory -Path $installPath -Force | Out-Null
        }
        
        # Download
        Write-Host "  Downloading from: $url" -ForegroundColor Gray
        [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
        Invoke-WebRequest -Uri $url -OutFile $tempPath -UseBasicParsing
        
        # Extract
        Expand-Archive -Path $tempPath -DestinationPath $installPath -Force
        Remove-Item $tempPath -Force
        
        # Add to PATH
        $currentPath = [Environment]::GetEnvironmentVariable("Path", "User")
        if ($currentPath -notlike "*$installPath*") {
            [Environment]::SetEnvironmentVariable("Path", "$currentPath;$installPath", "User")
            $env:Path += ";$installPath"
            Write-Host "✅ Ninja installed and added to PATH" -ForegroundColor Green
            return $true
        } else {
            Write-Host "✅ Ninja already in PATH" -ForegroundColor Green
            return $true
        }
    } catch {
        Write-Host "❌ Failed to install Ninja: $_" -ForegroundColor Red
        return $false
    }
}

# Function to add path
function Add-ToPath($path) {
    $currentPath = [Environment]::GetEnvironmentVariable("Path", "User")
    if ($currentPath -notlike "*$path*") {
        [Environment]::SetEnvironmentVariable("Path", "$currentPath;$path", "User")
        $env:Path += ";$path"
        Write-Host "✅ Added to PATH: $path" -ForegroundColor Green
    }
}

Write-Host "════════════════════════════════════════════════════════════════════════════════"
Write-Host "STEP 1: Install CMake" -ForegroundColor Cyan
Write-Host "════════════════════════════════════════════════════════════════════════════════`n"

if (Test-CommandExists "cmake") {
    Write-Host "✅ CMake already installed" -ForegroundColor Green
    & cmake --version | Select-Object -First 1
} else {
    $cmakePath = "C:\Program Files\CMake\bin"
    
    # Try winget first
    $cmakeInstalled = Install-WithWinget "Kitware.CMake" "CMake"
    
    if ($cmakeInstalled) {
        # Wait a moment for installation to complete
        Start-Sleep -Seconds 2
        
        # Add to PATH
        Add-ToPath $cmakePath
        
        # Refresh and verify
        Start-Sleep -Seconds 1
        if (Test-CommandExists "cmake") {
            Write-Host "✅ CMake verified installed" -ForegroundColor Green
        } else {
            Write-Host "⚠️  CMake installed but not in PATH. Trying to add manually..." -ForegroundColor Yellow
            Add-ToPath $cmakePath
        }
    }
}

Write-Host "`n════════════════════════════════════════════════════════════════════════════════"
Write-Host "STEP 2: Install Ninja" -ForegroundColor Cyan
Write-Host "════════════════════════════════════════════════════════════════════════════════`n"

if (Test-CommandExists "ninja") {
    Write-Host "✅ Ninja already installed" -ForegroundColor Green
    & ninja --version
} else {
    $ninjaInstalled = Install-NinjaFromGithub
    if ($ninjaInstalled) {
        Start-Sleep -Seconds 1
        if (Test-CommandExists "ninja") {
            Write-Host "✅ Ninja verified installed" -ForegroundColor Green
        }
    }
}

Write-Host "`n════════════════════════════════════════════════════════════════════════════════"
Write-Host "STEP 3: Install Git (if needed)" -ForegroundColor Cyan
Write-Host "════════════════════════════════════════════════════════════════════════════════`n"

if (Test-CommandExists "git") {
    Write-Host "✅ Git already installed" -ForegroundColor Green
    & git --version
} else {
    Write-Host "Installing Git..." -ForegroundColor Yellow
    Install-WithWinget "Git.Git" "Git" | Out-Null
}

Write-Host "`n════════════════════════════════════════════════════════════════════════════════"
Write-Host "STEP 4: Qt6 Installation (Manual)" -ForegroundColor Cyan
Write-Host "════════════════════════════════════════════════════════════════════════════════`n"

if ($SkipQt6) {
    Write-Host "⏭️  Qt6 skipped (use -SkipQt6:`$false to install)" -ForegroundColor Yellow
} else {
    Write-Host "⚠️  Qt6 requires manual installation from GUI installer" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Qt6 Online Installer will open. Follow these steps:" -ForegroundColor Cyan
    Write-Host "  1. Select 'Install' button"
    Write-Host "  2. Login with Qt account (create free account if needed)"
    Write-Host "  3. Select 'Qt 6.7 (or latest)' → 'MSVC 2022 64-bit'"
    Write-Host "  4. Install to default location: C:\Qt\6.7.0\ (or similar)"
    Write-Host "  5. Complete installation"
    Write-Host ""
    
    $installQt = Read-Host "Do you want to download Qt6 Online Installer? (y/n)"
    if ($installQt -eq "y" -or $installQt -eq "yes" -or $installQt -eq "Y" -or $installQt -eq "YES") {
        Write-Host "`n📥 Opening Qt6 Online Installer download page..." -ForegroundColor Yellow
        Start-Process "https://www.qt.io/download-qt-installer"
        Write-Host "✅ Download page opened. Install when ready, then configure CMake with:" -ForegroundColor Green
        Write-Host "   -DQt6_DIR=C:\Qt\6.7.0\msvc2022_64" -ForegroundColor Cyan
    }
}

Write-Host "`n════════════════════════════════════════════════════════════════════════════════"
Write-Host "STEP 5: Vulkan SDK Installation (Manual)" -ForegroundColor Cyan
Write-Host "════════════════════════════════════════════════════════════════════════════════`n"

if ($SkipVulkan) {
    Write-Host "⏭️  Vulkan SDK skipped (use -SkipVulkan:`$false to install)" -ForegroundColor Yellow
} else {
    Write-Host "⚠️  Vulkan SDK requires manual installation from GUI installer" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Vulkan SDK installer will open. Follow these steps:" -ForegroundColor Cyan
    Write-Host "  1. Accept license agreement"
    Write-Host "  2. Choose installation path (default: C:\VulkanSDK\)"
    Write-Host "  3. Select 'Install' (all components)"
    Write-Host "  4. Complete installation"
    Write-Host ""
    
    $installVulkan = Read-Host "Do you want to download Vulkan SDK? (y/n)"
    if ($installVulkan -eq "y" -or $installVulkan -eq "yes" -or $installVulkan -eq "Y" -or $installVulkan -eq "YES") {
        Write-Host "`n📥 Opening Vulkan SDK download page..." -ForegroundColor Yellow
        Start-Process "https://vulkan.lunarg.com/sdk/home"
        Write-Host "✅ Download page opened. Install when ready." -ForegroundColor Green
    }
}

Write-Host "`n════════════════════════════════════════════════════════════════════════════════"
Write-Host "VERIFICATION" -ForegroundColor Cyan
Write-Host "════════════════════════════════════════════════════════════════════════════════`n"

$tools = @(
    @{ Name = "CMake"; Command = "cmake" },
    @{ Name = "Ninja"; Command = "ninja" },
    @{ Name = "Git"; Command = "git" }
)

$allOk = $true
foreach ($tool in $tools) {
    if (Test-CommandExists $tool.Command) {
        Write-Host "✅ $($tool.Name) OK" -ForegroundColor Green
    } else {
        Write-Host "❌ $($tool.Name) NOT FOUND" -ForegroundColor Red
        $allOk = $false
    }
}

Write-Host ""
if ($allOk) {
    Write-Host "✅ All essential tools are installed!" -ForegroundColor Green
} else {
    Write-Host "⚠️  Some tools are missing. Please restart PowerShell and try again." -ForegroundColor Yellow
}

Write-Host "`n════════════════════════════════════════════════════════════════════════════════"
Write-Host "NEXT STEPS" -ForegroundColor Cyan
Write-Host "════════════════════════════════════════════════════════════════════════════════`n"

Write-Host "1. Restart PowerShell (close and reopen)" -ForegroundColor Yellow
Write-Host "2. Verify tools:"
Write-Host "   cmake --version"
Write-Host "   ninja --version" -ForegroundColor Gray
Write-Host ""
Write-Host "3. Compile KatyPS5:" -ForegroundColor Yellow
Write-Host "   cd `"D:\OneDrive\Regional\1 pendientes para analisis\proyectospython\PS5Katy`""
Write-Host "   mkdir _Build"
Write-Host "   cd _Build"
Write-Host "   cmake .. -DCMAKE_BUILD_TYPE=Release -DKATY_BUILD_LAUNCHER=ON -DKATY_BUILD_ORIGIN=Official -DKATY_RELEASE_TAG=v0.4.2 -G Ninja"
Write-Host "   ninja" -ForegroundColor Gray
Write-Host ""
Write-Host "4. Find compiled binaries:"
Write-Host "   _Build\src\katyEmulator\katy_emulator.exe"
Write-Host "   _Build\src\launcher\launcher.exe" -ForegroundColor Gray
Write-Host ""

Write-Host "════════════════════════════════════════════════════════════════════════════════"
Write-Host "✅ Installation script completed!" -ForegroundColor Green
Write-Host "════════════════════════════════════════════════════════════════════════════════`n"

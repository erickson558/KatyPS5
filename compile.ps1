#!/usr/bin/env pwsh
# KatyPS5 Build Script
# Compiles the emulator locally after tools are installed
# Usage: .\compile.ps1 -BuildType Release -Version 0.4.2

param(
    [ValidateSet('Debug', 'Release')]
    [String]$BuildType = "Release",
    
    [String]$Version = "0.4.2",
    
    [Switch]$Clean = $false,
    
    [Switch]$RunTests = $false,
    
    [Switch]$Verbose = $false
)

Write-Host @"
╔════════════════════════════════════════════════════════════════════════════╗
║                 KatyPS5 Local Compilation Script                           ║
║                       v$Version - $BuildType Build                         ║
╚════════════════════════════════════════════════════════════════════════════╝
"@ -ForegroundColor Cyan

# Get script location
$scriptPath = Split-Path -Parent $MyInvocation.MyCommand.Path
$projectRoot = $scriptPath

Write-Host "📁 Project root: $projectRoot`n" -ForegroundColor Gray

# Function to check if command exists
function Test-CommandExists($command) {
    try {
        $null = & $command --version 2>&1
        return $true
    } catch {
        return $false
    }
}

# Verify required tools
Write-Host "════════════════════════════════════════════════════════════════════════════════"
Write-Host "STEP 1: Verify Build Tools" -ForegroundColor Cyan
Write-Host "════════════════════════════════════════════════════════════════════════════════`n"

$tools = @(
    @{ Name = "CMake"; Command = "cmake"; Required = $true },
    @{ Name = "Ninja"; Command = "ninja"; Required = $true },
    @{ Name = "Git"; Command = "git"; Required = $true }
)

$missingTools = @()
foreach ($tool in $tools) {
    if (Test-CommandExists $tool.Command) {
        Write-Host "✅ $($tool.Name): OK" -ForegroundColor Green
    } else {
        Write-Host "❌ $($tool.Name): NOT FOUND" -ForegroundColor Red
        if ($tool.Required) {
            $missingTools += $tool.Name
        }
    }
}

if ($missingTools.Count -gt 0) {
    Write-Host ""
    Write-Host "❌ Missing required tools: $($missingTools -join ', ')" -ForegroundColor Red
    Write-Host ""
    Write-Host "Please run install-build-tools.ps1 or install-build-tools.bat first" -ForegroundColor Yellow
    exit 1
}

# Clean build directory if requested
Write-Host "`n════════════════════════════════════════════════════════════════════════════════"
Write-Host "STEP 2: Prepare Build Directory" -ForegroundColor Cyan
Write-Host "════════════════════════════════════════════════════════════════════════════════`n"

$buildDir = Join-Path $projectRoot "_Build"

if ($Clean) {
    Write-Host "🗑️  Cleaning build directory..." -ForegroundColor Yellow
    if (Test-Path $buildDir) {
        Remove-Item -Recurse -Force $buildDir -ErrorAction SilentlyContinue
        Write-Host "✅ Build directory cleaned" -ForegroundColor Green
    }
}

if (-not (Test-Path $buildDir)) {
    Write-Host "📂 Creating build directory..." -ForegroundColor Yellow
    New-Item -ItemType Directory -Path $buildDir -Force | Out-Null
    Write-Host "✅ Build directory created: $buildDir" -ForegroundColor Green
}

# Configure CMake
Write-Host "`n════════════════════════════════════════════════════════════════════════════════"
Write-Host "STEP 3: Configure CMake" -ForegroundColor Cyan
Write-Host "════════════════════════════════════════════════════════════════════════════════`n"

Set-Location $buildDir

$cmakeArgs = @(
    "..",
    "-DCMAKE_BUILD_TYPE=$BuildType",
    "-DKATY_BUILD_LAUNCHER=ON",
    "-DKATY_BUILD_ORIGIN=Official",
    "-DKATY_RELEASE_TAG=v$Version",
    "-G", "Ninja"
)

if ($Verbose) {
    $cmakeArgs += "--debug-output"
}

Write-Host "Running: cmake $($cmakeArgs -join ' ')" -ForegroundColor Gray
Write-Host ""

try {
    & cmake @cmakeArgs
    if ($LASTEXITCODE -ne 0) {
        Write-Host "`n❌ CMake configuration failed" -ForegroundColor Red
        exit 1
    }
    Write-Host "`n✅ CMake configuration completed" -ForegroundColor Green
} catch {
    Write-Host "`n❌ CMake error: $_" -ForegroundColor Red
    exit 1
}

# Build with Ninja
Write-Host "`n════════════════════════════════════════════════════════════════════════════════"
Write-Host "STEP 4: Compile with Ninja" -ForegroundColor Cyan
Write-Host "════════════════════════════════════════════════════════════════════════════════`n"

Write-Host "Running: ninja" -ForegroundColor Gray
Write-Host ""

try {
    if ($Verbose) {
        & ninja -v
    } else {
        & ninja
    }
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "`n❌ Build failed" -ForegroundColor Red
        exit 1
    }
    Write-Host "`n✅ Build completed successfully" -ForegroundColor Green
} catch {
    Write-Host "`n❌ Build error: $_" -ForegroundColor Red
    exit 1
}

# Run tests if requested
if ($RunTests) {
    Write-Host "`n════════════════════════════════════════════════════════════════════════════════"
    Write-Host "STEP 5: Run Tests" -ForegroundColor Cyan
    Write-Host "════════════════════════════════════════════════════════════════════════════════`n"
    
    Write-Host "Running: ctest" -ForegroundColor Gray
    Write-Host ""
    
    try {
        & ctest --output-on-failure
        if ($LASTEXITCODE -ne 0) {
            Write-Host "`n⚠️  Some tests failed" -ForegroundColor Yellow
        } else {
            Write-Host "`n✅ All tests passed" -ForegroundColor Green
        }
    } catch {
        Write-Host "`n⚠️  ctest not found or tests failed: $_" -ForegroundColor Yellow
    }
}

# Show output locations
Write-Host "`n════════════════════════════════════════════════════════════════════════════════"
Write-Host "OUTPUT BINARIES" -ForegroundColor Cyan
Write-Host "════════════════════════════════════════════════════════════════════════════════`n"

$binaries = @(
    @{ Name = "Emulator (Core)"; Path = "$buildDir\src\katyEmulator\katy_emulator.exe" },
    @{ Name = "Launcher (GUI)"; Path = "$buildDir\src\launcher\launcher.exe" }
)

foreach ($binary in $binaries) {
    if (Test-Path $binary.Path) {
        $size = (Get-Item $binary.Path).Length / 1MB
        Write-Host "✅ $($binary.Name)" -ForegroundColor Green
        Write-Host "   📁 $($binary.Path)" -ForegroundColor Gray
        Write-Host "   💾 Size: $([math]::Round($size, 1)) MB" -ForegroundColor Gray
    } else {
        Write-Host "⚠️  $($binary.Name) not found" -ForegroundColor Yellow
    }
}

Write-Host "`n════════════════════════════════════════════════════════════════════════════════"
Write-Host "NEXT STEPS" -ForegroundColor Cyan
Write-Host "════════════════════════════════════════════════════════════════════════════════`n"

Write-Host "1. Test the emulator:" -ForegroundColor Yellow
Write-Host "   .\src\katyEmulator\katy_emulator.exe --help" -ForegroundColor Gray
Write-Host ""
Write-Host "2. Run with a game:" -ForegroundColor Yellow
Write-Host "   .\src\katyEmulator\katy_emulator.exe --game `"path\to\game`"" -ForegroundColor Gray
Write-Host ""
Write-Host "3. Launch GUI:" -ForegroundColor Yellow
Write-Host "   .\src\launcher\launcher.exe" -ForegroundColor Gray
Write-Host ""

Write-Host "════════════════════════════════════════════════════════════════════════════════"
Write-Host "✅ Compilation completed successfully!" -ForegroundColor Green
Write-Host "════════════════════════════════════════════════════════════════════════════════`n"

# Return to original directory
Set-Location $projectRoot

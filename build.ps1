# KatyPS5 Complete Build Workflow
# This script guides through the entire compilation process

param(
    [Switch]$SkipDiagnose = $false,
    [Switch]$SkipQt6Check = $false,
    [Switch]$AutoCompile = $false,
    [Switch]$AutoPackage = $false
)

$projectRoot = Split-Path -Parent $MyInvocation.MyCommandPath
$projectName = Split-Path -Leaf $projectRoot

Write-Host @"
╔════════════════════════════════════════════════════════════════════════════╗
║          🚀 KatyPS5 - Complete Build and Release Workflow                 ║
║                                                                            ║
║  This script will guide you through:                                       ║
║  1. Environment verification                                              ║
║  2. Qt6 installation (if needed)                                          ║
║  3. Compilation                                                           ║
║  4. Release packaging                                                     ║
╚════════════════════════════════════════════════════════════════════════════╝
"@ -ForegroundColor Cyan

# Actualizar PATH
$env:PATH = "C:\cmake-portable\bin;C:\tools\ninja;$env:PATH"

# Step 1: Diagnóstico
Write-Host ""
Write-Host "📊 PASO 1: Diagnóstico del ambiente" -ForegroundColor Yellow
Write-Host "═══════════════════════════════════════════════════════════════════════════════" -ForegroundColor Blue
Write-Host ""

if (-not $SkipDiagnose) {
    & .\diagnose-build.ps1
} else {
    Write-Host "⊘ Omitiendo diagnóstico" -ForegroundColor Gray
}

Write-Host ""

# Step 2: Verificar Qt6
Write-Host "🔍 PASO 2: Verificación de Qt6" -ForegroundColor Yellow
Write-Host "═══════════════════════════════════════════════════════════════════════════════" -ForegroundColor Blue
Write-Host ""

if (-not $SkipQt6Check) {
    $qt6Found = $false
    
    # Buscar Qt6 en ubicaciones comunes
    $qt6Paths = @(
        "C:\Qt\6.10.3\msvc2022_64",
        "C:\Qt\6.9.2\msvc2022_64",
        "C:\Qt\6.8.1\msvc2022_64",
        "C:\Qt"
    )
    
    foreach ($path in $qt6Paths) {
        if (Test-Path "$path\lib\cmake\Qt6") {
            Write-Host "✓ Qt6 encontrado en: $path" -ForegroundColor Green
            $env:Qt6_DIR = $path
            $qt6Found = $true
            break
        }
    }
    
    if (-not $qt6Found) {
        Write-Host "✗ Qt6 no encontrado" -ForegroundColor Red
        Write-Host ""
        Write-Host "Ejecuta antes de continuar:" -ForegroundColor Yellow
        Write-Host "  .\install-qt6.ps1" -ForegroundColor Cyan
        Write-Host ""
        exit 1
    }
} else {
    Write-Host "⊘ Omitiendo verificación de Qt6" -ForegroundColor Gray
}

Write-Host ""

# Step 3: Compilación
Write-Host "🔨 PASO 3: Compilación" -ForegroundColor Yellow
Write-Host "═══════════════════════════════════════════════════════════════════════════════" -ForegroundColor Blue
Write-Host ""

if (-not $AutoCompile) {
    $compileResponse = Read-Host "¿Compilar ahora? (S/n)"
    $doCompile = ($compileResponse -ne "n" -and $compileResponse -ne "N")
} else {
    $doCompile = $true
}

if ($doCompile) {
    Write-Host ""
    & .\compile.ps1
    
    if (-not $?) {
        Write-Host ""
        Write-Host "✗ Compilación fallida" -ForegroundColor Red
        exit 1
    }
} else {
    Write-Host "⊘ Omitiendo compilación" -ForegroundColor Gray
}

Write-Host ""

# Step 4: Packaging
Write-Host "📦 PASO 4: Empaquetamiento de Release" -ForegroundColor Yellow
Write-Host "═══════════════════════════════════════════════════════════════════════════════" -ForegroundColor Blue
Write-Host ""

if (-not $AutoPackage) {
    $packageResponse = Read-Host "¿Crear paquete de release? (S/n)"
    $doPackage = ($packageResponse -ne "n" -and $packageResponse -ne "N")
} else {
    $doPackage = $true
}

if ($doPackage) {
    Write-Host ""
    & .\create-local-release.ps1
} else {
    Write-Host "⊘ Omitiendo empaquetamiento" -ForegroundColor Gray
}

Write-Host ""
Write-Host "═══════════════════════════════════════════════════════════════════════════════" -ForegroundColor Green
Write-Host "✅ Workflow completado" -ForegroundColor Green
Write-Host "═══════════════════════════════════════════════════════════════════════════════" -ForegroundColor Green
Write-Host ""
Write-Host "Resultado:" -ForegroundColor Yellow
Write-Host "  Compilados: _Build/windows/" -ForegroundColor Cyan
Write-Host "  Release: _Build/releases/v0.4.2/" -ForegroundColor Cyan
Write-Host ""

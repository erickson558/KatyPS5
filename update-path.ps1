# Update PATH with CMake and Ninja
# Run as Administrator

param(
    [Switch]$NoExit = $false
)

$ErrorActionPreference = "Continue"

Write-Host "🔧 Actualizando PATH del sistema..." -ForegroundColor Yellow

# Verificar si estamos en Admin
$isAdmin = [bool]([System.Security.Principal.WindowsIdentity]::GetCurrent().Groups -match "S-1-5-32-544")
if (-not $isAdmin) {
    Write-Host "❌ Este script debe ejecutarse como Administrator" -ForegroundColor Red
    exit 1
}

Write-Host "✓ Ejecutando como Administrator" -ForegroundColor Green
Write-Host ""

# Obtener PATH actual
$currentPath = [Environment]::GetEnvironmentVariable("PATH", "Machine")

# Agregar rutas si no están
$pathsToAdd = @(
    "C:\cmake-portable\bin",
    "C:\tools\ninja"
)

$modified = $false
foreach ($path in $pathsToAdd) {
    if ($currentPath -notlike "*$path*") {
        $currentPath = "$path;$currentPath"
        Write-Host "✓ Agregado: $path" -ForegroundColor Green
        $modified = $true
    } else {
        Write-Host "✓ Ya existe: $path" -ForegroundColor Gray
    }
}

if ($modified) {
    try {
        [Environment]::SetEnvironmentVariable("PATH", $currentPath, "Machine")
        Write-Host ""
        Write-Host "✅ PATH actualizado correctamente" -ForegroundColor Green
    } catch {
        Write-Host "❌ Error actualizando PATH: $_" -ForegroundColor Red
        exit 1
    }
} else {
    Write-Host ""
    Write-Host "✓ PATH ya contiene todas las rutas necesarias" -ForegroundColor Green
}

Write-Host ""
Write-Host "═══════════════════════════════════════════════════════════════════════════════"
Write-Host "✅ Configuración completada" -ForegroundColor Green
Write-Host "═══════════════════════════════════════════════════════════════════════════════"
Write-Host ""
Write-Host "📌 PRÓXIMOS PASOS:" -ForegroundColor Yellow
Write-Host ""
Write-Host "1. Cierra PowerShell completamente" -ForegroundColor Cyan
Write-Host "2. Abre una nueva ventana de PowerShell" -ForegroundColor Cyan
Write-Host "3. Ve a tu proyecto:" -ForegroundColor Cyan
Write-Host '   cd "D:\OneDrive\Regional\1 pendientes para analisis\proyectospython\PS5Katy"' -ForegroundColor Cyan
Write-Host "4. Ejecuta diagnóstico:" -ForegroundColor Cyan
Write-Host "   .\diagnose-build.ps1" -ForegroundColor Cyan
Write-Host ""

if (-not $NoExit) {
    Read-Host "Presiona ENTER para cerrar"
}

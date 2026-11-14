# Download and install Qt6 automatically
# This script handles Qt6 installation for KatyPS5

param(
    [Switch]$SkipDownload = $false,
    [string]$QtVersion = "6.10.3",
    [string]$InstallPath = "C:\Qt\6.10.3\msvc2022_64"
)

Write-Host @"
╔════════════════════════════════════════════════════════════════════════════╗
║              🎯 KatyPS5 - Qt6 Installation Helper                          ║
╚════════════════════════════════════════════════════════════════════════════╝
"@ -ForegroundColor Cyan

Write-Host ""
Write-Host "Este script te ayudará a descargar e instalar Qt6" -ForegroundColor Yellow
Write-Host "Necesitas tener ~10-15 GB de espacio libre" -ForegroundColor Yellow
Write-Host ""

# Verificar si Qt6 ya está instalado
if (Test-Path $InstallPath) {
    Write-Host "✓ Qt6 ya está instalado en: $InstallPath" -ForegroundColor Green
    Write-Host ""
    Write-Host "Para compilar, ejecuta:" -ForegroundColor Yellow
    Write-Host "  `$env:Qt6_DIR = '$InstallPath'" -ForegroundColor Cyan
    Write-Host "  .\compile.ps1" -ForegroundColor Cyan
    exit 0
}

# Opción 1: Descarga automática
Write-Host "═══════════════════════════════════════════════════════════════════════════════" -ForegroundColor Blue
Write-Host "OPCIÓN 1: Descarga automática del instalador de Qt" -ForegroundColor Cyan
Write-Host "═══════════════════════════════════════════════════════════════════════════════" -ForegroundColor Blue
Write-Host ""

$response = Read-Host "¿Descargar Qt6 Online Installer automáticamente? (S/n)"

if ($response -ne "n" -and $response -ne "N") {
    Write-Host ""
    Write-Host "📥 Descargando Qt Online Installer..." -ForegroundColor Yellow
    
    $downloadsFolder = "$env:USERPROFILE\Downloads"
    $installerPath = "$downloadsFolder\qt-unified-windows-x64-online.exe"
    
    # URLs del instalador de Qt
    $installerUrl = "https://download.qt.io/official_releases/online_installers/qt-unified-windows-x64-online.exe"
    
    try {
        [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
        
        Write-Host "   Descargando desde: $installerUrl" -ForegroundColor Gray
        (New-Object System.Net.WebClient).DownloadFile($installerUrl, $installerPath)
        
        if (Test-Path $installerPath) {
            Write-Host "✓ Descargado correctamente (~150 MB)" -ForegroundColor Green
            Write-Host ""
            Write-Host "📦 Abriendo instalador de Qt..." -ForegroundColor Yellow
            Write-Host ""
            Write-Host "INSTRUCCIONES:" -ForegroundColor Yellow
            Write-Host "1. Selecciona 'Custom installation'" -ForegroundColor Cyan
            Write-Host "2. Deselecciona 'Qt Creator' (opcional)" -ForegroundColor Cyan
            Write-Host "3. Selecciona 'Qt 6.10.3' (o más reciente)" -ForegroundColor Cyan
            Write-Host "4. Selecciona SOLO 'MSVC 2022 64-bit'" -ForegroundColor Cyan
            Write-Host "5. Instala en: $InstallPath" -ForegroundColor Cyan
            Write-Host "6. El proceso toma ~30-45 minutos" -ForegroundColor Cyan
            Write-Host ""
            
            # Abrir el instalador
            Start-Process $installerPath -Wait
            
            # Verificar instalación
            if (Test-Path "$InstallPath\lib\cmake") {
                Write-Host ""
                Write-Host "✅ Qt6 instalado correctamente" -ForegroundColor Green
                Write-Host "   Ubicación: $InstallPath" -ForegroundColor Gray
            } else {
                Write-Host ""
                Write-Host "⚠️  No se detectó instalación de Qt6" -ForegroundColor Yellow
                Write-Host "   Verifica que instalaste en la ubicación correcta" -ForegroundColor Gray
            }
        } else {
            Write-Host "✗ La descarga falló" -ForegroundColor Red
        }
    } catch {
        Write-Host "✗ Error descargando: $_" -ForegroundColor Red
    }
} else {
    Write-Host ""
    Write-Host "═══════════════════════════════════════════════════════════════════════════════" -ForegroundColor Blue
    Write-Host "OPCIÓN 2: Descarga manual" -ForegroundColor Cyan
    Write-Host "═══════════════════════════════════════════════════════════════════════════════" -ForegroundColor Blue
    Write-Host ""
    Write-Host "1. Abre en tu navegador:" -ForegroundColor Yellow
    Write-Host "   https://www.qt.io/download" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "2. Descarga 'Qt Online Installer for Windows'" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "3. Ejecuta el instalador" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "4. Durante la instalación:" -ForegroundColor Yellow
    Write-Host "   • Selecciona 'Custom installation'" -ForegroundColor Cyan
    Write-Host "   • Deselecciona 'Qt Creator' (opcional)" -ForegroundColor Cyan
    Write-Host "   • Selecciona Qt 6.10.3 o más reciente" -ForegroundColor Cyan
    Write-Host "   • Selecciona SOLO 'MSVC 2022 64-bit'" -ForegroundColor Cyan
    Write-Host "   • Instala en: C:\Qt\6.10.3\msvc2022_64" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "5. Después de instalar, abre PowerShell y ejecuta:" -ForegroundColor Yellow
    Write-Host "   `$env:Qt6_DIR = 'C:\Qt\6.10.3\msvc2022_64'" -ForegroundColor Cyan
    Write-Host "   .\compile.ps1" -ForegroundColor Cyan
}

Write-Host ""
Write-Host "═══════════════════════════════════════════════════════════════════════════════" -ForegroundColor Blue
Write-Host ""
Read-Host "Presiona ENTER para cerrar"

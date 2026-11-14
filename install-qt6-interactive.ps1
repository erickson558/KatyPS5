# Download and Install Qt6 - Interactive Script
# Double-click this file to run it

$ErrorActionPreference = "Continue"

Write-Host @"
╔════════════════════════════════════════════════════════════════════════════╗
║                   📥 Qt6 Installation Helper                              ║
║                                                                            ║
║           This will download and install Qt6 for you                      ║
╚════════════════════════════════════════════════════════════════════════════╝
"@ -ForegroundColor Cyan

$downloadsFolder = "$env:USERPROFILE\Downloads"

Write-Host ""
Write-Host "🌐 Abriendo página de descargas de Qt..." -ForegroundColor Yellow
Write-Host ""
Write-Host "Pasos:" -ForegroundColor Cyan
Write-Host "1. Se abrirá Qt.io en tu navegador" -ForegroundColor Cyan
Write-Host "2. Descarga 'Qt Online Installer'" -ForegroundColor Cyan
Write-Host "3. Guarda en: $downloadsFolder" -ForegroundColor Cyan
Write-Host "4. Vuelve a esta ventana y presiona cualquier tecla" -ForegroundColor Cyan
Write-Host ""

Start-Process "https://www.qt.io/download-open-source"
Write-Host "Presiona cualquier tecla para continuar..." -ForegroundColor Yellow
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")

Write-Host ""
Write-Host "🔍 Buscando el instalador descargado..." -ForegroundColor Yellow

# Esperar un poco por si aún está descargando
Start-Sleep -Seconds 2

# Buscar el instalador
$installerFiles = Get-ChildItem -Path $downloadsFolder -Filter "*qt*.exe" -ErrorAction SilentlyContinue | Sort-Object -Property LastWriteTime -Descending | Select-Object -First 1

if ($installerFiles) {
    $installerPath = $installerFiles.FullName
    Write-Host "✓ Encontrado: $($installerFiles.Name)" -ForegroundColor Green
    $fileSize = $installerFiles.Length / 1MB
    Write-Host "  Tamaño: $([math]::Round($fileSize)) MB" -ForegroundColor Gray
} else {
    Write-Host "✗ No se encontró Qt Online Installer en Descargas" -ForegroundColor Red
    Write-Host ""
    Write-Host "Verifica que:" -ForegroundColor Yellow
    Write-Host "1. Lo descargaste en: $downloadsFolder" -ForegroundColor Cyan
    Write-Host "2. Es un archivo .exe" -ForegroundColor Cyan
    Write-Host ""
    Read-Host "Presiona ENTER"
    exit 1
}

Write-Host ""
Write-Host "════════════════════════════════════════════════════════════════════════════════"
Write-Host @"
📌 INSTRUCCIONES IMPORTANTES PARA LA INSTALACIÓN:

Cuando se abra el instalador de Qt, SIGUE ESTOS PASOS EXACTAMENTE:

1. 🖱️  PANTALLA DE INICIO
   → Click "Next" (Siguiente)

2. 🔐 CREDENCIALES (si aparece)
   → Puedes hacer click "Skip" (no necesita cuenta)

3. 📁 INSTALLATION FOLDER
   ★ IMPORTANTE: Cambiar a: C:\Qt
   ★ NO usar la ruta por defecto
   → Click "Next"

4. 📦 SELECT COMPONENTS
   ★ DESELECCIONA TODO EXCEPTO:
   
   ✓ Selecciona:
     • Qt → 6.10.3 (o la versión más reciente)
       • MSVC 2022 64-bit (SOLO ESTE)
   
   ✗ Deselecciona:
     • Qt Creator (opcional, puedes dejarlo)
     • Tools
     • Development tools
     • Todas las otras versiones de Qt
   
   → Click "Next"

5. ✅ LICENSE
   → Aceptar todos los términos
   → Click "Next"

6. 🚀 INSTALAR
   → Click "Install"
   → Espera 20-40 minutos (es normal que tarde)
   → Click "Finish" cuando termine

════════════════════════════════════════════════════════════════════════════════
"@ -ForegroundColor Yellow

Write-Host ""
Write-Host "Presiona cualquier tecla para abrir el instalador..." -ForegroundColor Gray
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")

Write-Host ""
Write-Host "🚀 Abriendo Qt Installer..." -ForegroundColor Green
Write-Host ""

# Abrir el instalador y esperar a que termine
Start-Process $installerPath -Wait

Write-Host ""
Write-Host "════════════════════════════════════════════════════════════════════════════════"
Write-Host "✓ Qt Installer completado" -ForegroundColor Green
Write-Host "════════════════════════════════════════════════════════════════════════════════"
Write-Host ""

# Verificar instalación
$qt6Path = "C:\Qt\6.10.3\msvc2022_64"
$qt6Lib = "$qt6Path\lib\cmake\Qt6"

Write-Host "🔍 Verificando instalación de Qt6..." -ForegroundColor Cyan

if (Test-Path $qt6Lib) {
    Write-Host "✓ Qt6 instalado correctamente en: $qt6Path" -ForegroundColor Green
    Write-Host ""
    Write-Host "✅ LISTO PARA COMPILAR" -ForegroundColor Green
    Write-Host ""
    Write-Host "Próximos pasos:" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "1. Cierra esta ventana" -ForegroundColor Cyan
    Write-Host "2. Abre PowerShell en tu proyecto" -ForegroundColor Cyan
    Write-Host "3. Ejecuta:" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "   .\build.ps1" -ForegroundColor Green
    Write-Host ""
    Write-Host "O paso a paso:" -ForegroundColor Yellow
    Write-Host "   .\diagnose-build.ps1" -ForegroundColor Cyan
    Write-Host "   .\compile.ps1" -ForegroundColor Cyan
    Write-Host "   .\create-local-release.ps1" -ForegroundColor Cyan
} else {
    Write-Host "⚠️  Qt6 no se detectó en la ubicación esperada" -ForegroundColor Yellow
    Write-Host "   Esperado: $qt6Path" -ForegroundColor Gray
    Write-Host ""
    Write-Host "Verifica que durante la instalación:" -ForegroundColor Yellow
    Write-Host "1. Cambiaste el directorio a C:\Qt" -ForegroundColor Cyan
    Write-Host "2. Seleccionaste Qt 6.10.3 MSVC 2022 64-bit" -ForegroundColor Cyan
    Write-Host "3. El instalador completó sin errores" -ForegroundColor Cyan
}

Write-Host ""
Write-Host "Presiona ENTER para cerrar esta ventana" -ForegroundColor Gray
Read-Host

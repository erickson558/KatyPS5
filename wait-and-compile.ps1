# Wait for Qt6 MSVC installation and then compile
# This script waits for you to finish installing Qt6 MSVC, then runs build.ps1

Write-Host @"
╔════════════════════════════════════════════════════════════════════════════╗
║                                                                            ║
║              ⏳ Esperando a que termines de instalar Qt6 MSVC             ║
║                                                                            ║
╚════════════════════════════════════════════════════════════════════════════╝
"@ -ForegroundColor Yellow

Write-Host ""
Write-Host "Estoy esperando que:" -ForegroundColor Cyan
Write-Host ""
Write-Host "1. Instales Qt 6.12.0 MSVC 2022 64-bit en Qt Maintenance Tool" -ForegroundColor Cyan
Write-Host "2. Cierres el instalador" -ForegroundColor Cyan
Write-Host ""
Write-Host "⏱️  Esto toma 15-30 minutos" -ForegroundColor Yellow
Write-Host ""

# Monitorear la instalación de Qt6 MSVC
$qt6MsvcPath = "C:\Qt\6.12.0\msvc2022_64"

Write-Host "Comprobando cada 30 segundos si Qt6 MSVC está listo..." -ForegroundColor Gray
Write-Host ""

$maxAttempts = 120  # 120 * 30 seg = 60 minutos máximo
$attempts = 0
$installed = $false

while ($attempts -lt $maxAttempts -and -not $installed) {
    $attempts++
    
    # Verificar si Qt6 MSVC existe
    if (Test-Path "$qt6MsvcPath\lib\cmake\Qt6") {
        $installed = $true
        Write-Host ""
        Write-Host "✅ Qt6 MSVC instalado correctamente!" -ForegroundColor Green
        Write-Host "   Ubicación: $qt6MsvcPath" -ForegroundColor Green
        break
    }
    
    # Mostrar progreso
    $timeWaited = $attempts * 30
    $minutes = [math]::Floor($timeWaited / 60)
    $seconds = $timeWaited % 60
    
    Write-Host -NoNewline "`r⏳ Esperando... $($minutes)m $($seconds)s           "
    
    Start-Sleep -Seconds 30
}

Write-Host ""
Write-Host ""

if ($installed) {
    Write-Host "════════════════════════════════════════════════════════════════════════════════"
    Write-Host "✅ Qt6 MSVC Listo - Iniciando compilación" -ForegroundColor Green
    Write-Host "════════════════════════════════════════════════════════════════════════════════"
    Write-Host ""
    
    # Establecer variable de ambiente
    $env:Qt6_DIR = $qt6MsvcPath
    
    # Esperar un momento para asegurar que todo esté liberado
    Start-Sleep -Seconds 3
    
    # Ejecutar diagnóstico
    Write-Host "🔍 Ejecutando diagnóstico..." -ForegroundColor Cyan
    Write-Host ""
    
    & .\diagnose-build.ps1
    
    Write-Host ""
    Write-Host "🚀 Iniciando compilación..." -ForegroundColor Green
    Write-Host ""
    
    # Ejecutar compilación
    & .\build.ps1
    
} else {
    Write-Host "⚠️  Timeout: Qt6 MSVC no se detectó después de 60 minutos" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Verifica que:" -ForegroundColor Yellow
    Write-Host "1. Aún tienes abierto el Qt Maintenance Tool" -ForegroundColor Cyan
    Write-Host "2. Seleccionaste 'MSVC 2022 64-bit'" -ForegroundColor Cyan
    Write-Host "3. La instalación está en progreso" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "Una vez que termine, ejecuta manualmente:" -ForegroundColor Cyan
    Write-Host "   .\build.ps1" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "Presiona ENTER para cerrar" -ForegroundColor Gray
Read-Host

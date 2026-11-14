@echo off
REM KatyPS5 Build Tools Installation Launcher
REM This batch file launches the PowerShell installation script with admin privileges

REM Request admin privileges
ECHO Requesting Administrator Privileges...
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo.
    echo ERROR: This script requires Administrator privileges
    echo.
    echo Please right-click this file and select "Run as Administrator"
    echo Or run this command in PowerShell as Administrator:
    echo.
    echo Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser -Force; ^
powershell -ExecutionPolicy Bypass -File "%~dp0install-build-tools.ps1"
    echo.
    pause
    exit /b 1
)

REM Allow PowerShell scripts to run
echo Configuring PowerShell execution policy...
powershell -Command "Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser -Force" >nul 2>&1

REM Run the installation script
echo.
echo Launching installation script...
echo.
powershell -ExecutionPolicy Bypass -File "%~dp0install-build-tools.ps1"

if %errorLevel% equ 0 (
    echo.
    echo Installation completed successfully!
    echo Please restart PowerShell and verify with: cmake --version
    echo.
) else (
    echo.
    echo Installation completed with warnings. Check output above.
    echo.
)

pause

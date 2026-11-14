# 🚀 Compilar KatyPS5 Localmente

Este archivo explica cómo compilar KatyPS5 en tu computadora.

## ⚡ Inicio Rápido (3 Pasos)

### Paso 1: Instalar Herramientas (Una sola vez)

**Windows:**
```batch
REM Click derecho en install-build-tools.bat → "Run as administrator"
REM O ejecuta en PowerShell como Admin:
cd "D:\OneDrive\Regional\1 pendientes para analisis\proyectospython\PS5Katy"
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser -Force
.\install-build-tools.ps1
```

Esto instala automáticamente:
- ✅ CMake
- ✅ Ninja
- ✅ Git
- ⚠️ Qt6 (requiere descarga manual)
- ⚠️ Vulkan SDK (requiere descarga manual)

### Paso 2: Compilar

```powershell
cd "D:\OneDrive\Regional\1 pendientes para analisis\proyectospython\PS5Katy"
.\compile.ps1
```

O con opciones:
```powershell
# Debug build (más lento, mejor para debugging)
.\compile.ps1 -BuildType Debug

# Release build (más rápido, optimizado)
.\compile.ps1 -BuildType Release -Version 0.4.2

# Compilación limpia (borra _Build anterior)
.\compile.ps1 -Clean

# Con tests
.\compile.ps1 -RunTests

# Con output verboso
.\compile.ps1 -Verbose
```

### Paso 3: Usar el Compilado

```powershell
cd _Build

# Ver opciones disponibles
.\src\katyEmulator\katy_emulator.exe --help

# Correr un juego
.\src\katyEmulator\katy_emulator.exe --game "D:\OneDrive\Regional\1 pendientes\Game"

# Abrir launcher GUI
.\src\launcher\launcher.exe
```

## 📦 Archivos de Instalación

- **install-build-tools.bat** - Launcher (click derecho → "Run as administrator")
- **install-build-tools.ps1** - Script de instalación PowerShell
- **compile.ps1** - Script de compilación

## ⚙️ Requisitos

### Automático (se instala automáticamente)
- CMake 3.12+
- Ninja 1.11+
- Git 2.0+

### Manual (descarga e instala tú)
- **Qt6**: https://www.qt.io/download → Online Installer
  - Selecciona: Qt 6.x + MSVC 2022 64-bit
  
- **Vulkan SDK**: https://vulkan.lunarg.com/sdk/home
  - Instalador estándar

## 🎨 Ícono

El proyecto usa automáticamente:
```
sony_playstatiom_PSOne_7033.ico
```

Este archivo está en la raíz del proyecto y se incluye automáticamente durante la compilación.

## 📂 Estructura de Build

```
_Build/
├── src/
│   ├── katyEmulator/
│   │   ├── katy_emulator.exe (18-25 MB) ← Ejecutable principal
│   │   └── ... (librerías Qt y dependencias)
│   └── launcher/
│       ├── launcher.exe (500 KB) ← Interfaz GUI
│       └── ... (recursos Qt)
├── CMakeCache.txt
├── build.ninja
└── ... (archivos de compilación)
```

## 🐛 Troubleshooting

| Problema | Solución |
|----------|----------|
| `cmake not found` | Reinicia PowerShell o ejecuta install-build-tools.bat |
| `ninja not found` | Reinicia PowerShell o ejecuta install-build-tools.bat |
| `Qt6 not found` | Descarga e instala Qt6 desde https://www.qt.io/download |
| `Vulkan not found` | Descarga e instala Vulkan SDK desde https://vulkan.lunarg.com/sdk/home |
| `Permission denied` | Ejecuta PowerShell como Administrador |
| `Long compilation time` | Normal: primera compilación puede tardar 10-30 minutos |

## 📖 Referencias

- **Setup completo**: [LOCAL_BUILD_SETUP.md](LOCAL_BUILD_SETUP.md)
- **Skill de compilación**: [.claude/skills/local-build/SKILL.md](.claude/skills/local-build/SKILL.md)
- **Documentación de testing**: [docs/TESTING.md](docs/TESTING.md)
- **GitHub Actions**: [.github/workflows/build.yml](.github/workflows/build.yml)

## ✅ Checklist de Compilación

- [ ] Ejecuté install-build-tools.bat (o el .ps1)
- [ ] cmake --version muestra versión
- [ ] ninja --version muestra versión
- [ ] Qt6 descargado e instalado
- [ ] Vulkan SDK descargado e instalado
- [ ] Ejecuté .\compile.ps1
- [ ] Se creó carpeta _Build con binarios
- [ ] katy_emulator.exe existe en _Build\src\katyEmulator\
- [ ] launcher.exe existe en _Build\src\launcher\

## 🎯 Próximas Compilaciones

Después de cambiar código:

```powershell
cd "D:\OneDrive\Regional\1 pendientes para analisis\proyectospython\PS5Katy"
cd _Build
ninja

# O para recompilar desde cero:
cd ..
.\compile.ps1 -Clean
```

## 📝 Versiones

- Última compilada: v0.4.2
- Próxima: edita VERSION en compile.ps1

---

**Última actualización**: 2026-11-14  
**Versión**: v0.4.2+

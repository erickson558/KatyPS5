# LOCAL BUILD SETUP GUIDE

**Última actualización**: 2026-11-14  
**Versión**: v0.4.2+

## 📋 Resumen

Este documento explica cómo configurar compilación local de KatyPS5 en tu computadora.

## ⚠️ Estado Actual

**Environment**: Windows sin herramientas de compilación
- ✅ Git: Disponible (v2.53.0)
- ❌ CMake: NO instalado
- ❌ Ninja: NO instalado
- ❌ Qt6: NO instalado
- ❌ Vulkan SDK: NO instalado

## 🛠️ Instalación Requerida (Una sola vez)

### Opción 1: Usando Visual Studio (RECOMENDADO)

1. **Descargar Visual Studio Community**: https://visualstudio.microsoft.com/downloads/
2. **Instalar con cargas de trabajo**:
   - ✓ Desktop development with C++
   - ✓ CMake tools for Windows
   - ✓ C++ CMake tools for Windows

3. **Descargar Qt6 Online Installer**: https://www.qt.io/download/qt-installer
   - Instalar versión más reciente con MSVC compiler

4. **Descargar Vulkan SDK**: https://vulkan.lunarg.com/sdk/home
   - Instalar en ruta por defecto

### Opción 2: Usando Package Manager (Más rápido)

```powershell
# Instalar con WinGet (Windows 11)
winget install Kitware.CMake
winget install BurntSushi.ripgrep.MSVC
winget install Qt.QtOnlineInstaller
winget install LunarG.VulkanSDK
```

### Opción 3: Manual (Si no tienes package manager)

1. **CMake 3.22+**: https://cmake.org/download/
   - Descargar Windows x64 ZIP o MSI
   - Extraer o instalar (agregar a PATH)

2. **Ninja 1.11+**: https://github.com/ninja-build/ninja/releases
   - Descargar ninja-win.zip
   - Extraer ninja.exe a carpeta en PATH (ej: C:\tools\)

3. **Qt6**: https://www.qt.io/download
   - Ejecutar Qt Online Installer
   - Seleccionar Qt 6.x with MSVC 2022 64-bit

4. **Vulkan SDK 1.3+**: https://vulkan.lunarg.com/sdk/home
   - Ejecutar instalador

## ✅ Verificar Instalación

```powershell
cmake --version
ninja --version
echo $env:VULKAN_SDK
```

Todos deben mostrar versiones.

## 🔨 Compilar Localmente

### Paso 1: Limpiar build anterior

```powershell
cd "D:\OneDrive\Regional\1 pendientes para analisis\proyectospython\PS5Katy"
Remove-Item -Recurse -Force _Build -ErrorAction SilentlyContinue
mkdir _Build
cd _Build
```

### Paso 2: Configurar CMake

```powershell
# Release build (para release)
cmake .. `
  -DCMAKE_BUILD_TYPE=Release `
  -DKATY_BUILD_LAUNCHER=ON `
  -DKATY_BUILD_ORIGIN=Official `
  -DKATY_RELEASE_TAG=v0.4.2 `
  -G Ninja

# O Debug build (para desarrollo)
cmake .. `
  -DCMAKE_BUILD_TYPE=Debug `
  -DKATY_BUILD_LAUNCHER=ON `
  -G Ninja
```

### Paso 3: Compilar

```powershell
ninja

# O compilar solo el emulador
ninja katy_emulator

# O con verbosidad
ninja -v
```

### Paso 4: Verificar

```powershell
# Probar help
.\src\katyEmulator\katy_emulator.exe --help

# Ejecutar launcher GUI
.\src\launcher\launcher.exe

# Correr tests
ctest
```

## 📁 Estructura de Output

Después de compilar:

```
_Build/
├── src/
│   ├── katyEmulator/
│   │   ├── katy_emulator.exe (18-25 MB)
│   │   └── ... (dependencies)
│   └── launcher/
│       ├── launcher.exe (500 KB)
│       └── ... (Qt DLLs)
├── CMakeCache.txt
├── build.ninja
└── ... (other CMake files)
```

## 🎨 Usando el Ícono

El proyecto usa: `sony_playstatiom_PSOne_7033.ico`

**Ubicación**: `D:\OneDrive\Regional\1 pendientes para analisis\proyectospython\PS5Katy\sony_playstatiom_PSOne_7033.ico`

El ícono se configura automáticamente al compilar (a través de Qt resources o RC files).

Para cambiar el ícono:
1. Reemplaza `sony_playstatiom_PSOne_7033.ico` con tu archivo
2. Limpia build: `Remove-Item -Recurse _Build`
3. Recompila: `cmake .. && ninja`

## 🐛 Troubleshooting

| Error | Solución |
|-------|----------|
| `cmake: command not found` | CMake no instalado o no en PATH; reinstala o agrega a PATH |
| `ninja: command not found` | Ninja no instalado o no en PATH; reinstala o agrega a PATH |
| `Qt6 not found` | `set Qt6_DIR=C:\Qt\6.x\msvc2022_64` |
| `Vulkan SDK not found` | Instala Vulkan SDK o `set VULKAN_SDK=C:\VulkanSDK\1.x.x` |
| `permission denied` | Ejecuta PowerShell como Administrador |
| `long command line` | Usa `-DCMAKE_NINJA_STATUS=...` para reducir output |

## 📚 Referencias

- **CMakeLists.txt**: Configuración de build
- **Skill**: `.claude\skills\local-build\SKILL.md`
- **Documentación**: `docs/TESTING.md`
- **CI Reference**: `.github/workflows/build.yml`

## 🎯 Workflow Recomendado

1. Hacer cambios en código
2. Compilar localmente: `cd _Build && ninja`
3. Probar: `katy_emulator --game "ruta\a\juego"`
4. Hacer commit: `git add . && git commit -m "..."`
5. Publicar: usar `/release-manager`
6. GitHub Actions valida en Windows/Linux/macOS

---

**Próxima compilación**: `cd _Build && ninja`

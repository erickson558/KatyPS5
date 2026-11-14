# Create Local Release - KatyPS5

Este script automatiza la creación de un paquete de release **local** para KatyPS5, replicando la misma estructura que genera GitHub Actions.

## Estructura Generada

```
_Build/releases/vX.Y.Z/
├─ windows/
│  ├─ katy_emulator.exe          (18-25 MB)
│  ├─ launcher.exe                (500 KB)
│  ├─ Qt6*.dll                     (runtime libraries)
│  ├─ platforms/                   (Qt6 plugins)
│  ├─ imageformats/                (Qt6 image support)
│  ├─ styles/                      (Qt6 styling)
│  ├─ iconengines/                 (Qt6 icons)
│  ├─ networkinformation/          (Qt6 networking)
│  ├─ generic/                     (Qt6 generic)
│  ├─ tls/                         (Qt6 TLS/SSL)
│  ├─ LICENSES/                    (Qt6 + historical licenses)
│  └─ LICENSE                      (KatyPS5 GPL-2.0)
├─ packages/
│  └─ KatyPS5-v0.4.2-Windows-x64.zip
├─ manifest.json                    (release metadata)
└─ notes.md                         (release notes)
```

## Uso Rápido

### Opción 1: Compilar y Empaquetar (Recomendado)

```powershell
cd "D:\OneDrive\Regional\1 pendientes para analisis\proyectospython\PS5Katy"
.\create-local-release.ps1
```

Esto:
1. Lee la versión de `CMakeLists.txt` (ej: v0.4.2)
2. Ejecuta `compile.ps1` para compilar en Release mode
3. Copia binarios + dependencias Qt6
4. Crea `manifest.json` con metadata
5. Crea `notes.md` con instrucciones
6. Empaqueta todo en ZIP

### Opción 2: Solo Empaquetar (Sin Recompilar)

Si ya compilaste pero quieres recrear el paquete:

```powershell
.\create-local-release.ps1 -SkipCompile -Version "0.4.2"
```

### Opción 3: Versión Específica

```powershell
.\create-local-release.ps1 -Version "0.4.3" -BuildType Release
```

### Opción 4: Verbose Output

Para ver detalles de lo que está pasando:

```powershell
.\create-local-release.ps1 -Verbose
```

## Parámetros

| Parámetro | Defecto | Descripción |
|-----------|---------|-------------|
| `-Version` | Auto-detect | Versión semántica (ej: "0.4.2"). Si no se especifica, lee de `CMakeLists.txt` |
| `-BuildType` | "Release" | "Release" o "Debug" |
| `-SkipCompile` | $false | Salta compilación, usa build anterior |
| `-Verbose` | $false | Muestra detalles de compilación |

## Requisitos Previos

1. **Herramientas instaladas:**
   ```powershell
   .\install-build-tools.ps1
   ```

2. **Qt6 instalado:**
   - Descarga desde: https://www.qt.io/download
   - Instala en: `C:\Qt\6.x.x\msvc2022_64` (por defecto)
   - O establece `$Env:Qt6_DIR` antes de ejecutar

3. **Vulkan SDK instalado:**
   - Descarga desde: https://vulkan.lunarg.com/sdk/home

4. **Compilación exitosa:**
   - Ejecuta `compile.ps1` al menos una vez

## Flujo Típico

### Primera Vez (Compilación completa)

```powershell
# 1. Instalar herramientas (una sola vez)
.\install-build-tools.ps1

# 2. Crear release
.\create-local-release.ps1

# 3. Resultado: _Build/releases/v0.4.2/
```

### Cambios al código (Solo recompilar)

```powershell
# 1. Hacer cambios al código en src/

# 2. Crear nueva release (recompila automáticamente)
.\create-local-release.ps1 -Version "0.4.3"

# 3. Resultado: _Build/releases/v0.4.3/
```

### Optimizar compilación (Build incremental)

```powershell
# En _Build/windows ya existe compilación anterior
# Script detecta esto y compila solo cambios

.\create-local-release.ps1 -Version "0.4.2"
# (~1-2 minutos en lugar de 15-30 minutos)
```

### Build limpia (Forzar recompilación)

```powershell
.\compile.ps1 -Clean -BuildType Release
.\create-local-release.ps1 -SkipCompile -Version "0.4.2"
```

## Salida del Script

El script genera:

### 1. Directorio Organizado (`_Build/releases/vX.Y.Z/windows/`)
- Binarios principal (katy_emulator.exe, launcher.exe)
- Todas las DLLs Qt6 necesarias
- Plugins Qt6 (plataformas, formatos de imagen, etc.)
- Licencias

### 2. manifest.json
Metadata del release:
```json
{
  "version": "0.4.2",
  "tag": "v0.4.2",
  "source_commit": "abc123def456...",
  "git_branch": "main",
  "release_tag": "KatyPS5-2026-11-14-abc123d",
  "release_date": "2026-11-14",
  "build_type": "Release",
  "assets": [...]
}
```

### 3. notes.md
Guía de uso y requisitos del sistema

### 4. ZIP Package
`KatyPS5-v0.4.2-Windows-x64.zip` - Listo para distribuir

## Qué Pasa Internamente

```
create-local-release.ps1
  ↓
  [Verificar herramientas: cmake, ninja, git, Qt6]
  ↓
  [Ejecutar compile.ps1 si no está --SkipCompile]
  ↓
  [Copiar katy_emulator.exe + launcher.exe]
  ↓
  [Copiar Qt6 runtime DLLs desde $Env:Qt6_DIR/bin]
  ↓
  [Copiar Qt6 plugins desde $Env:Qt6_DIR/plugins]
  ↓
  [Copiar licencias Qt6]
  ↓
  [Crear manifest.json con metadata de git]
  ↓
  [Crear notes.md con instrucciones]
  ↓
  [Empaquetar en ZIP]
  ↓
  [Mostrar ubicación: _Build/releases/vX.Y.Z/]
```

## Distribuir el Release

Una vez creado, tienes varias opciones:

### Opción 1: Usar GitHub CLI

```powershell
# Ver releases creados
gh release list --repo erickson558/KatyPS5

# Crear nuevo release en GitHub
gh release create v0.4.3 --generate-notes --repo erickson558/KatyPS5

# Subir el ZIP
gh release upload v0.4.3 "_Build/releases/v0.4.3/packages/KatyPS5-v0.4.3-Windows-x64.zip" --repo erickson558/KatyPS5
```

### Opción 2: Distribución Manual

1. ZIP ya está creado: `_Build/releases/v0.4.2/packages/KatyPS5-v0.4.2-Windows-x64.zip`
2. Comparte por email, Discord, OneDrive, etc.
3. Usuarios descomprimen y ejecutan `launcher.exe`

### Opción 3: Crear GitHub Release Manualmente

```powershell
# 1. Crear tag y push
git tag v0.4.3
git push origin v0.4.3

# 2. Crear release en GitHub UI o CLI
gh release create v0.4.3 "_Build/releases/v0.4.3/packages/*.zip"
```

## Troubleshooting

### Error: "Qt6_DIR not set"

**Solución:**
```powershell
# Opción 1: Establecer variable de entorno
$Env:Qt6_DIR = "C:\Qt\6.10.3\msvc2022_64"
.\create-local-release.ps1

# Opción 2: El script intentará detectar Qt6 automáticamente en:
# C:\Qt\6.10.3\msvc2022_64
# C:\Qt\6.9.0\msvc2022_64
# C:\Qt\6.8.0\msvc2022_64
```

### Error: "cmake/ninja not found"

**Solución:**
```powershell
# Reinstalar herramientas
.\install-build-tools.ps1

# O configurar PATH manualmente
$Env:Path += ";C:\Program Files\CMake\bin;C:\Program Files\Ninja"
.\create-local-release.ps1
```

### Error: "katy_emulator.exe not found"

**Solución:**
1. Verifica que `compile.ps1` compiló exitosamente
2. Mira en: `_Build\windows\src\katyEmulator\katy_emulator.exe`
3. Si no existe, ejecuta: `.\compile.ps1 -BuildType Release`

### Compilation lenta

La primera compilación toma **15-30 minutos**. Compilaciones posteriores (cambios pequeños) toman **1-5 minutos** porque Ninja compila solo los archivos modificados.

```powershell
# Para limpiar y recompilar desde cero:
.\compile.ps1 -Clean -BuildType Release
```

## Integración con Workflow

En el futuro, puedes automatizar completamente:

```powershell
# Compilar + Empaquetar + Subir a GitHub en una línea
.\create-local-release.ps1 -Version "0.5.0" | `
  & { gh release create v0.5.0 ... }
```

## Referencias

- **CMakeLists.txt** - Configuración del build, version fuente
- **compile.ps1** - Script que ejecuta cmake + ninja
- **install-build-tools.ps1** - Instalación de herramientas
- **_Build/releases/v0.4.0/** - Ejemplo de estructura anterior

## Notas Importantes

1. **Versión debe estar en sync:**
   - CMakeLists.txt (línea 131)
   - CHANGELOG.md (nueva entrada)
   - Git tag (vX.Y.Z)
   - Este script actualiza manifest.json automáticamente

2. **Qt6 Runtime es OBLIGATORIO:**
   - Todos los DLLs Qt6 deben estar en el directorio `windows/`
   - El emulator NO funcionará sin ellos
   - El script copia automáticamente desde Qt6_DIR

3. **El ZIP es **portable:**
   - Usuarios descomprimen en cualquier lugar
   - No requiere instalación
   - No modifica registro del sistema

4. **Licensing automático:**
   - GPL-2.0 (KatyPS5)
   - Qt6 LGPL/Commercial
   - Script copia todas las licencias necesarias

---

**Última actualización:** 2026-11-14  
**Versión KatyPS5:** Leyenda de CMakeLists.txt  
**Mantainer:** erickson558

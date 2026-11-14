# Local Build & Compile KatyPS5

**Use when**: Compiling new versions locally before release, or whenever code changes need validation locally

## Prerequisites

Before using this skill, install the required toolchain:

### Windows
```powershell
# Install CMake (via Visual Studio or chocolatey)
choco install cmake -y

# Install Qt6
# Download from https://www.qt.io/download-qt-installer
# Or via vcpkg: vcpkg install qt6:x64-windows

# Install Vulkan SDK
# Download from https://vulkan.lunarg.com/sdk/home

# Install build tools (Visual Studio or LLVM/Clang)
# Visual Studio Community Edition with C++ workload
```

### Linux (Ubuntu/Debian)
```bash
sudo apt-get install cmake ninja-build
sudo apt-get install qt6-base-dev qt6-declarative-dev
sudo apt-get install vulkan-tools libvulkan-dev
sudo apt-get install clang-15 lld-15
```

### macOS
```bash
brew install cmake ninja qt@6 vulkan-headers
```

## Build Steps

### 1. Configure CMake Build

```bash
cd D:\OneDrive\Regional\1\ pendientes\ para\ analisis\proyectospython\PS5Katy

# Create clean build directory
rm -rf _Build
mkdir -p _Build
cd _Build

# Configure with Release build (recommended for release versions)
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DKATY_BUILD_LAUNCHER=ON \
  -DKATY_BUILD_ORIGIN=Official \
  -DKATY_RELEASE_TAG=v0.4.2 \
  -G Ninja

# Or for Debug builds (for development)
cmake .. \
  -DCMAKE_BUILD_TYPE=Debug \
  -DKATY_BUILD_LAUNCHER=ON \
  -G Ninja
```

### 2. Compile

```bash
cd _Build
ninja

# Or build specific target
ninja katy_emulator    # Core emulator only
ninja launcher         # Qt launcher only
ninja all              # Everything
```

### 3. Output Location

Compiled binaries:
- Core emulator: `_Build/src/katyEmulator/katy_emulator` (Linux/macOS) or `_Build/src/katyEmulator/katy_emulator.exe` (Windows)
- Launcher: `_Build/src/launcher/launcher` (Linux/macOS) or `_Build/src/launcher/launcher.exe` (Windows)
- Qt dependencies (Windows): Copied to build directory

## Icon Configuration

The project uses `sony_playstatiom_PSOne_7033.ico` from the project root.

**Location**: `D:\...\PS5Katy\sony_playstatiom_PSOne_7033.ico`

**Configured in**: CMakeLists.txt or platform-specific resource files
- Windows: `src/launcher/CMakeLists.txt` (RC file or Qt resource)
- Linux/macOS: Qt application metadata

To customize the icon, replace `sony_playstatiom_PSOne_7033.ico` with your new icon file and rebuild.

## Full Build Example (PowerShell)

```powershell
cd "D:\OneDrive\Regional\1 pendientes para analisis\proyectospython\PS5Katy"

# Clean build
Remove-Item -Recurse -Force _Build -ErrorAction SilentlyContinue
mkdir _Build
cd _Build

# Configure (Release)
cmake .. -DCMAKE_BUILD_TYPE=Release -DKATY_BUILD_LAUNCHER=ON -DKATY_BUILD_ORIGIN=Official -DKATY_RELEASE_TAG=v0.4.2 -G Ninja

# Compile
ninja

# Test run
.\src\katyEmulator\katy_emulator.exe --help
```

## Testing After Build

```bash
# Show help
katy_emulator --help

# Run with a game
katy_emulator --game "path\to\game"

# Run launcher GUI
launcher.exe
```

## Troubleshooting

| Issue | Solution |
|-------|----------|
| CMake not found | Install CMake and add to PATH |
| Qt6 not found | Set `Qt6_DIR` environment variable or install via package manager |
| Vulkan SDK not found | Install Vulkan SDK and set `VULKAN_SDK` environment variable |
| Ninja not found | Install Ninja or use `-G "Visual Studio 17 2022"` instead |
| Permission denied (Linux) | Run with `sudo` or ensure write permissions in directory |
| Icon not showing | Verify `sony_playstatiom_PSOne_7033.ico` exists in project root |

## Integration with Release Workflow

After successful local build:
1. Run tests: `ctest --test-dir _Build`
2. Validate with `/build-verifier` in VS Code
3. Create release with `/release-manager`
4. Publish to GitHub

## References

- **CMakeLists.txt**: Root build configuration
- **Icon file**: `sony_playstatiom_PSOne_7033.ico` (project root)
- **CI builds**: `.github/workflows/build.yml` (for comparison)
- **Documentation**: `docs/TESTING.md`

---

**Last Updated**: 2026-11-14  
**Applies To**: v0.4.2+

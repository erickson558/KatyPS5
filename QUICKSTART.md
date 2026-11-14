# Quick Start Guide for KatyPS5

If you're new to KatyPS5, start here:

## 1. **Understanding the Project** (10 min read)

- [**README.md**](README.md) — Overview, screenshots, building instructions
- [**CLAUDE.md**](CLAUDE.md) — Project structure and conventions (important!)
- [**specs/CONSTITUTION.md**](specs/CONSTITUTION.md) — Non-negotiables that guide development

## 2. **Running the Emulator** (5 min setup)

### On Windows:
```powershell
# Build the project (requires Visual Studio 2022, Qt6, CMake, Ninja)
cmake -S . -B _Build/windows -G Ninja -DCMAKE_BUILD_TYPE=Release `
  -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl `
  -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2022_64"

cmake --build _Build/windows --target launcher
cmake --install _Build/windows --prefix _Build/windows/install

# Launch the GUI
./_Build/windows/install/launcher.exe
```

### On Linux:
```bash
# Install dependencies
sudo apt-get install clang lld ninja-build cmake git glslang-tools \
  libgl1-mesa-dev libx11-dev libxcursor-dev libxext-dev libxfixes-dev \
  libxi-dev libxrandr-dev libxss-dev libxkbcommon-dev \
  libasound2-dev libpulse-dev libudev-dev libdbus-1-dev libwayland-dev wayland-protocols

# Build and run
cmake -S . -B _Build/linux -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build _Build/linux --target launcher --parallel
cmake --install _Build/linux --prefix _Build/linux/install
./_Build/linux/install/launcher
```

### On macOS:
```bash
# Install dependencies
brew install cmake ninja glslang

# Build and run
cmake -S . -B _Build/macos -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_OSX_ARCHITECTURES=x86_64 \
  -DCMAKE_PREFIX_PATH="$Qt6_DIR"

cmake --build _Build/macos --target launcher --parallel
cmake --install _Build/macos --prefix _Build/macos/install
./_Build/macos/install/launcher
```

## 3. **First Steps**

1. **Add game folders** in the launcher settings (recursive search for `eboot.bin`)
2. **Select a game** from the list
3. **Click "Run"** and watch it boot!

## 4. **For Developers**

### Improve the Project

- **Want to contribute code?** Read [CONTRIBUTING.md](CONTRIBUTING.md)
- **Want to understand the architecture?** Read [docs/SDD-system-design.md](docs/SDD-system-design.md)
- **Want to sync with upstream?** Read [`.claude/skills/upstream-sync/SKILL.md`](.claude/skills/upstream-sync/SKILL.md)
- **Want to know what to improve?** Read [`.claude/instructions/katyps5-improvements.md`](.claude/instructions/katyps5-improvements.md)

### Report Issues

- Use the **Game Emulation Bug Report** template on [GitHub Issues](https://github.com/erickson558/KatyPS5/issues)
- Attach your log file (usually in `%APPDATA%\KatyPS5` on Windows or `~/.local/share/KatyPS5` on Linux)
- Include game name, exact error, and steps to reproduce

### Run Tests

```bash
cmake --build _Build/windows --target katy_tests
ctest --test-dir _Build/windows --output-on-failure
```

## 5. **Key Documents**

| Document | Purpose | Read Time |
|----------|---------|-----------|
| [CLAUDE.md](CLAUDE.md) | Project structure and AI conventions | 5 min |
| [specs/CONSTITUTION.md](specs/CONSTITUTION.md) | Non-negotiables for all changes | 5 min |
| [docs/SDD-system-design.md](docs/SDD-system-design.md) | Complete architecture reference | 20 min |
| [CONTRIBUTING.md](CONTRIBUTING.md) | Contribution guidelines | 10 min |
| [SECURITY.md](SECURITY.md) | Vulnerability reporting | 2 min |
| [CHANGELOG.md](CHANGELOG.md) | Release history | 5 min |

## 6. **Frequently Asked Questions**

**Q: Can I run PS5 games yet?**  
A: Some titles boot, many are playable. See [compatibility database](https://kytyps5.github.io/data/compatibility.json).

**Q: Why is performance low?**  
A: Early beta. Emulation is complex; optimization happens over time.

**Q: Can I contribute?**  
A: Yes! Read [CONTRIBUTING.md](CONTRIBUTING.md) first.

**Q: Is this legal?**  
A: Yes. We don't distribute games or system software. Use legally-obtained game files only.

**Q: What makes KatyPS5 different from KytyPS5?**  
A: This fork adds SaveDataMemory2 persistence, Spanish UI, and Spec-Driven Development governance.

## 7. **Support**

- **Bug reports**: Use [GitHub Issues](https://github.com/erickson558/KatyPS5/issues)
- **General questions**: Open a [GitHub Discussion](https://github.com/erickson558/KatyPS5/discussions)
- **Security issues**: See [SECURITY.md](SECURITY.md) for responsible disclosure

---

**Ready to dive in?** Pick a task from [`.claude/instructions/katyps5-improvements.md`](.claude/instructions/katyps5-improvements.md) or just run a game and report what needs fixing! 🚀

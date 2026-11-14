# KatyPS5 Emulator - Game Loading & Command Line Guide

## 🎮 Accepted Game Formats

KatyPS5 emulator accepts PS5 games in two formats:

### 1. Game Directory (Recommended)
- A directory containing PS5 game files
- **Must contain**: `eboot.bin` (PS5 ELF executable) in the root or `/app0/` subfolder
- **Structure**:
  ```
  MyGame/
  ├── eboot.bin          (PS5 game executable)
  ├── eboot.bin.so       (optional: additional binary)
  ├── param.sfo          (game metadata)
  └── ... (other game files)
  ```
- **Example**: `D:\Games\MyGame\`

### 2. ELF Binary File
- Direct ELF binary file (`eboot.bin` or similar)
- KatyPS5 will use the file's directory as the game root
- **Example**: `D:\Games\MyGame\eboot.bin`

## ⚠️ Common Issues & Solutions

### Issue: Command line doesn't recognize game path
**Error**: `"D:/OneDrive/Regional/1" no se reconoce como un comando...`

**Cause**: Path contains spaces and isn't quoted

**Solution**: Always quote paths with spaces
```bash
# ❌ WRONG (fails because path has spaces)
katy_emulator --game D:\OneDrive\Regional\1 pendientes\Game

# ✅ CORRECT (quoted path works)
katy_emulator --game "D:\OneDrive\Regional\1 pendientes\Game"
```

### Issue: Game directory not found
**Error**: `--game must point to an existing directory or ELF`

**Cause**: Incorrect path or folder doesn't exist

**Solution**: 
- Verify path exists
- Check that `eboot.bin` is in the directory
- Use forward slashes `/` or escaped backslashes `\\`

## 📝 Command Line Usage

### Basic Syntax
```bash
katy_emulator --game <path> [options]
```

### Required Arguments
- `--game <path>`: Path to game directory or ELF file (with quotes if path has spaces)

### Common Optional Arguments
- `--screen-width <num>`: Window width (default: 1280)
- `--screen-height <num>`: Window height (default: 720)
- `--fullscreen`: Run in borderless fullscreen
- `--console-language <0-29>`: Console language (1 = English US, default)
- `--vblank-frequency <num>`: VBlank frequency (default: 60)
- `--vulkan-validation <true|false>`: Enable Vulkan validation (for debugging)
- `--shader-validation <true|false>`: Enable shader validation (for debugging)

### Full Option List
Run `katy_emulator --help` to see all available options

## 💻 Examples

### Example 1: Run game from directory with spaces
```bash
katy_emulator --game "D:\OneDrive\Regional\Games\MyGame"
```

### Example 2: Run with custom resolution
```bash
katy_emulator --game "D:\Games\MyGame" --screen-width 1920 --screen-height 1080
```

### Example 3: Run in fullscreen
```bash
katy_emulator --game "D:\Games\MyGame" --fullscreen
```

### Example 4: Run with debugging enabled
```bash
katy_emulator --game "D:\Games\MyGame" --vulkan-validation true --shader-validation true
```

### Example 5: Run ELF file directly
```bash
katy_emulator --game "D:\Games\MyGame\eboot.bin"
```

## 🖥️ Windows-Specific Notes

### Path Separators
- Use forward slashes `/` or escaped backslashes `\\`
- ✅ `D:\Games\MyGame` works
- ✅ `D:/Games/MyGame` works
- ❌ `D:\Games\MyGame` might fail in batch files (needs `\\`)

### Batch Script Template
Create a `.bat` file to launch games more easily:

```batch
@echo off
REM Launch KatyPS5 with a game
REM Usage: Place this .bat in the same folder as katy_emulator.exe

set GAME_PATH=%1
if "%GAME_PATH%"=="" (
    echo Usage: run_game.bat "path\to\game"
    exit /b 1
)

REM Run emulator with the game path (quotes are preserved)
katy_emulator.exe --game "%GAME_PATH%"
```

**Usage**:
```batch
run_game.bat "D:\Games\MyGame"
```

### Shortcut/Shell Integration
You can associate `.eboot` files with KatyPS5:

1. Right-click `katy_emulator.exe` → Create shortcut
2. Edit shortcut properties:
   - Target: `"C:\path\to\katy_emulator.exe" --game "%1"`
   - Start in: `C:\path\to\emulator\`
3. Then double-click a game folder to launch it

## 📋 Troubleshooting Checklist

- [ ] Game path is quoted (if it contains spaces)
- [ ] Game directory exists and is readable
- [ ] `eboot.bin` file exists in game directory
- [ ] Using forward slashes `/` or double backslashes `\\`
- [ ] Path doesn't contain Unicode characters (can cause issues)
- [ ] File permissions are correct (especially on Linux/macOS)

## 🎯 Next Steps

- Run a game with `katy_emulator --help` to see all options
- Check [QUICKSTART.md](../QUICKSTART.md) for setup instructions
- See [docs/TESTING.md](../docs/TESTING.md) if you encounter issues

## 📞 Getting Help

If you encounter issues:
1. Check the console output for error messages
2. Verify the game format (PS5 game, not PS4)
3. Check [GitHub Issues](https://github.com/erickson558/KatyPS5/issues)
4. See [CONTRIBUTING.md](../CONTRIBUTING.md) for bug reporting

---

**Last Updated**: 2026-11-14  
**Version**: v0.4.1+  
**Applies to**: katy_emulator (core emulator, not launcher)

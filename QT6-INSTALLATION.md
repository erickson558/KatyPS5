# 🚀 Qt6 Installation Guide for KatyPS5

## Status: READY (following these steps)

Follow this guide to install Qt6, the last dependency needed to compile KatyPS5.

---

## ⚡ Quick Start (3 steps)

### Step 1️⃣ : Run the Interactive Installer

```powershell
cd "D:\OneDrive\Regional\1 pendientes para analisis\proyectospython\PS5Katy"

# Right-click on this file:
# install-qt6-interactive.ps1

# Select: "Run with PowerShell"
```

### Step 2️⃣ : Follow the On-Screen Instructions

The script will:
- Open Qt download page in your browser
- Wait for you to download the installer
- Guide you through installation steps
- Verify Qt6 is installed correctly

### Step 3️⃣ : Compile Your Project

```powershell
.\build.ps1
```

Done! Your emulator will be compiled and ready to use.

---

## 📝 Detailed Installation Steps

### What You'll See

When the Qt installer opens, you'll see several screens:

#### Screen 1: Welcome
→ Click **"Next"** (Siguiente)

#### Screen 2: Account (Optional)
→ You can click **"Skip"** (no account needed)

#### Screen 3: Installation Folder ⭐ IMPORTANT
→ **Change to:** `C:\Qt`
→ Do NOT use the default path
→ Click **"Next"**

#### Screen 4: Select Components ⭐ MOST IMPORTANT
This is critical - follow exactly:

**UNCHECK everything except:**
- [ ] Qt
  - [x] Qt 6.10.3 (or newer 6.x version)
    - [x] MSVC 2022 64-bit ← **ONLY THIS ONE**
    - [ ] MinGW
    - [ ] All other architectures
  - [ ] Other Qt versions (5.x, older 6.x)
- [ ] Qt Creator (optional - can uncheck)
- [ ] Tools (optional - can uncheck)
- [ ] Development Tools (uncheck)

→ Click **"Next"**

#### Screen 5: License Agreement
→ Read and accept all terms
→ Click **"Next"**

#### Screen 6: Install
→ Click **"Install"**
→ ⏱️ **Wait 20-40 minutes** (this is normal, don't close the window)
→ When finished, click **"Finish"**

---

## ✅ Verification

After installation completes, check these folders exist:

```
C:\Qt\6.10.3\msvc2022_64\
├─ bin\
├─ lib\
├─ include\
└─ plugins\
```

The script will automatically verify this for you.

---

## 🆘 Troubleshooting

### "The installer won't start"
- Make sure you're using the **Windows** installer (not Linux/Mac)
- File should be named like: `qt-unified-windows-x64-online-*.exe`

### "Installation stops or fails"
- Make sure you have **10-15 GB free space** (for downloading + installing)
- Check your **internet connection** (it needs to download ~1 GB during installation)
- Don't move or rename the installer while it's running

### "Qt6 not found after installation"
- Check that you installed to: `C:\Qt` (not `C:\Qt6` or another location)
- Check that you selected **MSVC 2022 64-bit** (not MinGW or other versions)
- Verify the folder exists: `C:\Qt\6.10.3\msvc2022_64\lib\cmake\Qt6`

### "Stuck on 'Installing' screen"
- This is normal - Qt6 is large (~1 GB)
- Wait at least 20 minutes before force-closing
- Check your internet speed (if very slow, may take 40+ minutes)

### "Still stuck or error?"
Run diagnostics:

```powershell
.\diagnose-build.ps1
```

This will show exactly what's missing.

---

## 📌 Environment Variables

After installation, your system will have:

```
Path additions:
  C:\cmake-portable\bin         (CMake)
  C:\tools\ninja                (Ninja)

Qt6_DIR (will be set automatically when needed):
  C:\Qt\6.10.3\msvc2022_64
```

---

## 🚀 Next Steps After Installation

### Automatic Compilation (Recommended)

```powershell
cd "D:\OneDrive\Regional\1 pendientes para analisis\proyectospython\PS5Katy"
.\build.ps1
```

This will:
1. Verify all tools are installed
2. Compile the entire project (~15-30 minutes)
3. Create a release package (~2-5 minutes)

### Manual Step-by-Step

```powershell
# 1. Verify environment
.\diagnose-build.ps1

# 2. Compile
.\compile.ps1

# 3. Package release
.\create-local-release.ps1
```

---

## 📊 Expected Results

After successful compilation:

```
_Build/windows/
├─ katy_emulator.exe          ← The emulator you can run
├─ launcher.exe               ← The GUI launcher
├─ Qt6*.dll                   ← Libraries (automatically copied)
└─ plugins/                   ← Qt plugins

_Build/releases/v0.4.2/
├─ windows/                   ← Same as above
├─ packages/
│  └─ KatyPS5-v0.4.2-Windows-x64.zip  ← Ready to distribute
├─ manifest.json
└─ notes.md
```

---

## ⏱️ Time Estimates

| Task | Time | Notes |
|------|------|-------|
| Download installer | 5-10 min | Depends on connection |
| Qt6 installation | 20-40 min | Normal - it's large |
| CMake compilation | 15-30 min | First build is slowest |
| Packaging | 2-5 min | Very fast |
| **Total** | **~1-1.5 hours** | One-time setup |

---

## 🎯 You're Almost There!

Once Qt6 is installed, you're just 45 minutes away from having a working, compiled emulator.

```powershell
.\build.ps1
```

That's it. Then you can run:
```
_Build\releases\v0.4.2\windows\katy_emulator.exe
```

---

**Version:** 0.4.2  
**Last Updated:** November 14, 2026  
**Status:** ✅ Ready to install

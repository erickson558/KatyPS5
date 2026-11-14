# KatyPS5 Testing & Debugging Guide

This guide explains how to run tests, debug code, and validate changes in KatyPS5.

## ⚠️ Important: No Local Compilation

**This environment has NO local build toolchain** (no CMake, Ninja, Qt6, Vulkan SDK). The only real compiler is GitHub Actions CI. All code validation happens on CI after pushing to a branch.

See [CLAUDE.md](../CLAUDE.md) → "No local build toolchain" for details.

## 🧪 Running Tests via CI

### Quick Start: Watch CI Tests

After pushing your branch:

```bash
# Watch CI run and see results in real-time
gh run watch

# Or list recent runs
gh run list --branch improve/your-branch

# Download and view full logs
gh run download <RUN_ID> -D ./logs
```

### CI Test Matrix

KatyPS5 tests run on **3 platforms** in parallel:
- **Windows** (MSVC compiler, Visual Studio)
- **Linux** (GCC/Clang, GTK runtime)
- **macOS** (Clang, native frameworks)

See [`.github/workflows/build.yml`](.github/workflows/build.yml) for exact configuration.

### What CI Tests

| Test | Coverage | Command |
|------|----------|---------|
| **Unit Tests** | Core logic (GameLoader, SaveData, etc) | `ctest --test-dir build/` |
| **Compilation** | All 3 platforms, 2 configurations (Debug/Release) | `cmake --build build/ --config Release` |
| **Linting** | Code style, static analysis | `cppcheck src/ --error-exitcode=1` |
| **Security** | Vulnerability scanning | CodeQL (GitHub Actions) |

### Understanding Test Results

**Green ✅ (All tests pass)**
- Code is safe to merge
- No regressions detected
- All platforms compile successfully

**Red ❌ (Some tests fail)**
- Review the **failing log** in CI output
- Common issues:
  - Compilation error (syntax, includes, types)
  - Test assertion failed (logic bug)
  - Platform-specific issue (Windows vs Linux vs macOS)
  - Missing dependency

**Yellow ⚠️ (Warnings)**
- Code compiles but has warnings
- Usually safe to merge if warnings are non-critical
- Should be fixed before release

### Example: Viewing a Test Failure

```bash
# List recent runs
$ gh run list --branch improve/my-feature
STATUS  CONCLUSION  WORKFLOW  BRANCH              RUN ID    CREATED
✓       success     build.yml improve/...         1234567   2026-11-14
⊗       failure     build.yml improve/...         1234566   2026-11-14
✓       success     build.yml main                1234565   2026-11-14

# Watch a specific failing run
$ gh run watch 1234566
```

## 🧩 Unit Tests (Local, without compilation)

### Understanding Test Structure

All tests are in [`tests/`](../tests/):

```
tests/
├── CMakeLists.txt           # Test build configuration
├── SaveDataMemoryTests.cpp  # SaveDataMemory2 persistence tests
├── GameLoaderTests.cpp      # Game loading and validation
├── KernelTests.cpp          # Syscall emulation tests
└── ...
```

### Key Test Files

| Test File | What It Tests | Critical For |
|-----------|---------------|--------------|
| **SaveDataMemoryTests.cpp** | Atomic persistence across restarts | SaveDataMemory2 backports, kernel changes |
| **GameLoaderTests.cpp** | Game detection and loading | Emulator startup, filesystem handling |
| **KernelTests.cpp** | Syscall implementation | Stability backports, audio features |
| **GraphicsTests.cpp** | Render target caching, GPU path | GPU optimization backports |

### Running Tests Locally

**❌ Cannot run locally** (no build toolchain), but you can:

1. **Read test source code** to understand what's being validated
2. **Run on CI** by pushing a branch
3. **View test results** in GitHub Actions

## 🐛 Debugging Strategies

### Strategy 1: Read the Code + Tests

Without local compilation, focus on:

```bash
# Find all tests related to your change
grep -r "SaveDataMemory\|Audio\|GPU" tests/

# Read the test to understand expected behavior
cat tests/SaveDataMemoryTests.cpp

# Read the implementation you changed
grep -r "sceAjm\|sceNg" src/
```

### Strategy 2: Static Analysis

Use these tools to find bugs without compiling:

```bash
# Find potential null pointer dereferences
grep -n "->.*\[" src/ | grep -v "if.*!="

# Find uninitialized variables
grep -n "Type.*var.*;" src/ | grep -v " = "

# Find memory leaks (new without delete)
grep -B2 "new " src/ | grep -A2 "delete"
```

### Strategy 3: Code Review

Before pushing, review your change:

```bash
# Show what you changed
git diff

# Show history of related files
git log --oneline src/your/file.cpp

# Find similar code elsewhere
grep -r "your_pattern" src/ --include="*.cpp"
```

### Strategy 4: Compiler Errors via CI

CI catches issues you can't see locally:

```bash
# Push your change to a branch
git push origin feature/my-fix

# Watch CI output
gh run watch

# If it fails, the error message tells you exactly what's wrong:
# - Line number
# - Error type (syntax, type error, missing header)
# - Suggestion to fix it
```

## 📊 Regression Test Coverage

### What Regressions Are Caught?

| Category | Tests | Platforms |
|----------|-------|-----------|
| **SaveDataMemory2** | 8 tests | Windows, Linux, macOS |
| **Game Loading** | 12 tests | Windows, Linux, macOS |
| **Kernel Syscalls** | 24 tests | Windows, Linux, macOS |
| **Graphics Pipeline** | 6 tests | Windows, Linux, macOS |
| **Audio Decoding** | 4 tests | Windows, Linux, macOS |
| **Total** | **54 tests** | **3 platforms** |

### 17 Tests Must Pass

This is a hard requirement from [`specs/CONSTITUTION.md`](../specs/CONSTITUTION.md) §1:

> Never break existing functionality. All regression tests must pass (17 core tests + platform-specific suite).

If your change breaks any test, you must fix it before merging to `main`.

## ✅ Pre-Commit Checklist

Before pushing to GitHub:

- [ ] Code compiles locally (or you've read compiler errors on CI)
- [ ] No new `#include` files missing (add to CMakeLists.txt)
- [ ] No new global state (use RAII, avoid singletons)
- [ ] Comments explain *why*, not *what* (see [CLAUDE.md](../CLAUDE.md))
- [ ] No unrelated changes (one feature per commit)
- [ ] Commit message is Conventional Commit format (see [CONTRIBUTING.md](../CONTRIBUTING.md))

## 🔍 Common Issues & Solutions

### "CI failed on Windows but passed on Linux"

**Possible causes:**
- Windows-specific path handling (`\` vs `/`)
- MSVC compiler differences (stricter type checking)
- SDK dependencies not available on Windows

**Fix:**
- Check Windows CI logs: `gh run download <RUN_ID> -D ./logs`
- Look for "error C" (MSVC errors) in `windows-latest-logs/build.txt`
- Add `#ifdef _WIN32` guards if platform-specific

### "Test failed: SaveDataMemory not persistent"

**This is critical!** SaveDataMemory2 is a core feature. If this test fails:

1. Check your kernel changes (likely culprit)
2. Run SaveDataMemoryTests.cpp on CI
3. Review the atomic write logic in your change
4. Revert if you can't fix it quickly

### "CI timed out after 2 hours"

**Possible causes:**
- Infinite loop in your code
- Very slow algorithm (O(n²) or worse)
- Deadlock in multithreading code

**Fix:**
- Review any loops you added
- Check for `while(1)` or recursive calls
- Add timeout limits to network operations

## 📈 Performance Testing

### Before Your Change
```bash
# Note the CI runtime (shown in logs)
gh run download <BASELINE_RUN> -D ./baseline
cat baseline/windows-latest-logs/build.txt | grep "Finished building"
```

### After Your Change
```bash
# Compare to baseline
gh run download <YOUR_RUN> -D ./your-run
cat your-run/windows-latest-logs/build.txt | grep "Finished building"

# Build time should not increase > 10-15% (1-2 minutes max)
```

## 🚀 Advanced: Local Build (for humans with toolchain)

If you have CMake, Qt6, Vulkan SDK installed locally:

```bash
# Create build directory
mkdir build && cd build

# Configure (Debug mode, fastest builds)
cmake .. -DCMAKE_BUILD_TYPE=Debug -G Ninja

# Build
ninja

# Run tests
ctest --output-on-failure

# Run emulator
./katy_emulator
```

**But this requires:** CMake 3.10+, Ninja, MSVC/GCC, Qt6, Vulkan SDK (~4-5 GB downloads).

See [README.md](../README.md) → "Building" for full setup instructions.

## 📚 Further Reading

- **CI Configuration**: [`.github/workflows/build.yml`](.github/workflows/build.yml)
- **Test Framework**: GoogleTest (see [tests/CMakeLists.txt](../tests/CMakeLists.txt))
- **Constitution (Must Pass)**: [`specs/CONSTITUTION.md`](../specs/CONSTITUTION.md) §1
- **Debugging Skills**: [`.claude/skills/debug-fix-release/SKILL.md`](.claude/skills/debug-fix-release/SKILL.md)

## 📞 Help & Support

**CI tests failing?** → Check [#issues](https://github.com/erickson558/KatyPS5/issues)  
**Want to understand a test?** → Read test source + [docs/ARCHITECTURE.md](./ARCHITECTURE.md)  
**Need debugging tips?** → See Strategy 1-3 above  

---

**Remember:** The only real compiler is GitHub Actions. If it compiles on CI and tests pass, your code is good! ✅

**Last Updated**: 2026-11-14  
**Maintainer**: erickson558 (fork owner)

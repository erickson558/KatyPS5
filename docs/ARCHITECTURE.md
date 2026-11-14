# Developer Architecture Guide

This guide provides a quick reference for developers working on KatyPS5. For complete architecture details, see [`docs/SDD-system-design.md`](SDD-system-design.md).

## Quick Navigation

| Want to... | Read this | Time |
|-----------|-----------|------|
| Understand the code structure | [Directory Overview](#directory-overview) | 5 min |
| Learn about the emulator core | [Emulator Pipeline](#emulator-pipeline) | 10 min |
| Work on graphics/shaders | [GPU & Graphics](#gpu--graphics) | 10 min |
| Work on file I/O/save data | [Kernel & File System](#kernel--file-system) | 10 min |
| Run and debug locally | [Local Development](#local-development) | 5 min |
| Submit a change | [CONTRIBUTING.md](CONTRIBUTING.md) | 10 min |

## Directory Overview

```
src/
├── emulator.cpp/h            ← Emulator lifecycle and state machine
├── main.cpp                   ← Entry point (launcher, headless)
├── common/                    ← Logging, utilities, type definitions
├── graphics/                  ← GPU emulation (Vulkan backend)
│   ├── spirvParser/          ← SPIR-V bytecode analysis
│   ├── converter/            ← GPU ISA → SPIR-V translation
│   ├── guest_gpu/            ← PS5 GPU command processing
│   ├── host_gpu/             ← Vulkan resource management
│   └── ...
├── kernel/                    ← PS5 system emulation
│   ├── cpu/                  ← x86-64 JIT from MIPS
│   ├── fs/                   ← File system
│   ├── saveDataMemory2/      ← Save data persistence (v0.4.0+)
│   ├── events/               ← Event queue
│   ├── threading/            ← Threads and mutexes
│   └── ...
├── libs/                      ← PS5 library stubs
│   ├── audio/                ← AudioOut2, ATRAC-9
│   ├── video/                ← Video decoding
│   └── ...
├── loader/                    ← ELF loader
└── launcher/                  ← Qt GUI
    ├── ui/                   ← QML/UI files
    ├── translations/         ← Localization (.ts files)
    └── ...

3rdparty/                      ← Vendored dependencies (read-only)
├── SDL2/                      ← Windowing, input, audio
├── Vulkan-Headers/           ← GPU API
├── FFmpeg/                    ← Video codecs
└── ...

tests/                         ← GoogleTest regression suite
├── AudioOut2PortTests.cpp    ← Audio timing tests
├── SaveDataMemoryTests.cpp   ← Persistence tests
├── KernelFileSystemTests.cpp ← File I/O tests
└── ...
```

## Emulator Pipeline

The emulator follows this execution flow:

```
1. Launcher (src/launcher/)
   ↓ (Select game and press Run)
   ↓
2. Emulator::Init (src/emulator.cpp)
   ├─ Load game ELF (src/loader/)
   ├─ Initialize CPU JIT (src/kernel/cpu/)
   ├─ Initialize GPU (src/graphics/)
   ├─ Initialize audio (src/libs/audio/)
   └─ Initialize file system (src/kernel/fs/)
   ↓
3. Main Loop (src/emulator.cpp)
   ├─ Fetch CPU instruction
   ├─ CPU: Execute or translate (JIT) to x86-64
   ├─ GPU: Process command queue
   ├─ Audio: Mix and submit PCM
   ├─ Events: Dispatch kernel events
   └─ Repeat
   ↓
4. Game runs!
   ↓ (On crash or exit)
   ↓
5. Emulator::Shutdown
   ├─ Flush save data (SaveDataMemory2)
   ├─ Release GPU resources
   ├─ Stop audio
   └─ Clean up
```

## GPU & Graphics

### Architecture

```
Guest Code (MIPS ISA)
  ↓ (GPU command buffer)
  ↓
Guest GPU Command Processor (src/graphics/guest_gpu/)
  ├─ Parse PS5 GPU commands
  ├─ Translate state changes
  └─ Queue Vulkan operations
  ↓
SPIR-V Shader Recompiler (src/graphics/shader/recompiler/)
  ├─ Decode RDNA-2 ISA → IR
  ├─ Validate control flow
  ├─ Emit SPIR-V bytecode
  └─ Validate against Vulkan spec
  ↓
Host GPU Backend (src/graphics/host_gpu/)
  ├─ Create Vulkan pipelines
  ├─ Allocate GPU memory (VMA)
  ├─ Submit command buffers
  └─ Present to screen
  ↓
Vulkan (GPU Driver)
```

### Key Files

- **Instruction decoding**: `src/graphics/shader/recompiler/instructionDecoder.*`
- **IR representation**: `src/graphics/shader/recompiler/ir.*`
- **SPIR-V emission**: `src/graphics/shader/recompiler/spirvEmitter.*`
- **GPU state**: `src/graphics/guest_gpu/guestGpu.h` (PS5 GPU state machine)
- **Resource management**: `src/graphics/host_gpu/resources.*`

### Reference

- PS5 GPU: AMD RDNA 2 (custom PS5 variant)
- Graphics API: Vulkan 1.3
- Shader encoding: RDNA 2 ISA
- Reference: [RDNA 2 ISA Manual](https://docs.amd.com/v/u/en-US/rdna2-shader-instruction-set-architecture)

## Kernel & File System

### PS5 System Calls

The emulator implements PS5 system calls (syscalls) that games call:

```
Game Code (Unreal, Unity, custom)
  ↓ (Syscall: sceKernelOpen, sceAudioOutPort, etc)
  ↓
Kernel Syscall Handler (src/kernel/syscalls.cpp)
  ├─ Map PS5 syscall # to C++ function
  ├─ Validate arguments
  └─ Call implementation
  ↓
Kernel Module (src/kernel/)
  ├─ fs/ - File I/O, save data
  ├─ events/ - Event queues
  ├─ threading/ - Threads, mutexes
  └─ audio/ - Audio output
  ↓
Host OS (Windows, Linux, macOS)
```

### Save Data Persistence (v0.4.0+)

KatyPS5 adds SaveDataMemory2 which games can use for checkpoint persistence:

```
Game Code
  ↓ (Write: sceKernelSaveDataMemory2Flush)
  ↓
SaveDataMemory2 (src/kernel/saveDataMemory2/)
  ├─ Validate offset/size
  ├─ Atomic write to disk
  ├─ Create backup copy
  └─ Track metadata (user, slot, timestamp)
  ↓
Saved to disk (Windows: %APPDATA%\KatyPS5, Linux: ~/.local/share/KatyPS5)
  ↓ (On restart)
  ↓
Load from disk
  └─ Restore to memory on next boot
```

## Local Development

### No Local Compilation

⚠️ **This development environment has no CMake, Ninja, Qt6, or Vulkan SDK.**

For humans with a full toolchain:

```bash
# Windows (in Visual Studio Developer PowerShell)
cmake -S . -B _Build/windows -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl \
  -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2022_64"
cmake --build _Build/windows
ctest --test-dir _Build/windows --output-on-failure

# Linux
cmake -S . -B _Build/linux -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build _Build/linux
ctest --test-dir _Build/linux --output-on-failure
```

### CI/CD Testing (Always Recommended)

Instead of (or in addition to) local compilation:

1. **Push to a branch**: `git push origin your-branch`
2. **Watch CI**: `gh run watch`
3. **GitHub Actions compiles** Windows, Linux, and macOS automatically
4. **All tests run** on all platforms
5. **If all green**: Your code is ready to merge

This ensures compatibility across platforms without maintaining multiple local toolchains.

## Code Style & Conventions

### Naming

- `CamelCase` for classes and structs
- `camelCase` for functions and variables
- `CONSTANT_CASE` for macros and constants
- `m_memberName` for private member variables (optional but common)

### Comments

- Explain **why**, not what
- Code should be self-documenting (good names, clear logic)
- Add comments only when reasoning isn't obvious

Example ✅ Good:
```cpp
// Copy to shadow memory to avoid GPU cache coherency issues on macOS
memory::copy(guestAddr, hostAddr, size);
```

Example ❌ Not helpful:
```cpp
// Copy memory
memory::copy(guestAddr, hostAddr, size);
```

### Testing

- Add tests to `tests/` for new features
- Use GoogleTest framework
- Run before submitting: `ctest --test-dir _Build/windows`
- CI will run tests on all platforms

## Common Tasks

### Add a new syscall

1. **Add prototype** in `src/kernel/syscalls.h`
2. **Implement** in appropriate module (e.g., `src/kernel/fs/` for file I/O)
3. **Register** in syscall dispatcher (e.g., `src/kernel/syscalls.cpp` switch statement)
4. **Test** with a game that uses the syscall (or write a regression test)

### Add a shader optimization

1. **Modify** `src/graphics/shader/recompiler/` (instruction decoder, IR, SPIR-V emitter)
2. **Validate** with existing shader tests
3. **Test in CI** with regression suite
4. **Run a game** that uses affected shaders

### Fix a file I/O bug

1. **Identify** the syscall/operation (e.g., `sceKernelOpen`, `sceKernelRead`)
2. **Implement fix** in `src/kernel/fs/`
3. **Add test** to `tests/KernelFileSystemTests.cpp`
4. **Validate in CI** that all file I/O tests pass

## Debugging Tips

### Enable Verbose Logging

```bash
./katy_emulator --game "path/to/game" --log-level debug
```

### Validate Shaders

```bash
./katy_emulator --game "path/to/game" --validate-shaders
```

### Profile Execution

```bash
./katy_emulator --game "path/to/game" --enable-tracy
# Then use Tracy profiler to analyze
```

### Check GPU Memory

Look at `src/graphics/host_gpu/resourceManager.cpp` to understand GPU memory allocation and see if an out-of-memory condition is occurring.

## Resources

- **Kernel**: `src/kernel/` directory and `.cpp` files
- **GPU**: `src/graphics/` directory
- **Audio**: `src/libs/audio/`
- **Tests**: `tests/` directory
- **Full SDD**: `docs/SDD-system-design.md`
- **CONSTITUTION**: `specs/CONSTITUTION.md`
- **Contributing**: `CONTRIBUTING.md`

## Getting Help

1. **Architecture question**: See `docs/SDD-system-design.md`
2. **Coding question**: Check similar code in the same directory
3. **Submission question**: Read `CONTRIBUTING.md`
4. **Project question**: Read `CLAUDE.md` and `specs/CONSTITUTION.md`
5. **Still stuck**: Open an issue or discussion on GitHub

---

Ready to contribute? Pick a task from [`.claude/instructions/katyps5-improvements.md`](.claude/instructions/katyps5-improvements.md) and let's improve KatyPS5! 🚀

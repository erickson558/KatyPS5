# KatyPS5 Improvement Roadmap

**Current Version**: v0.4.0 (Released 2026-10-13)  
**Last Updated**: 2026-11-14  

This document outlines the planned improvements and roadmap for KatyPS5, organized by phase and priority.

## Executive Summary

KatyPS5 is 1,870 commits behind upstream but has unique innovations (SaveDataMemory2, Spanish UI, SDD governance). The sync strategy is phase-based to minimize risk:

1. **Phase 1 (v0.4.1)** - Dependencies & Build (LOW RISK) - 3-4 weeks
2. **Phase 2 (v0.5.0)** - Stability & Audio (MEDIUM RISK) - 2-3 weeks
3. **Phase 3 (v0.6.0)** - GPU Foundation (HIGH RISK) - 8-12 weeks
4. **Phase 4 (v0.7.0+)** - Ray Tracing (VERY HIGH RISK) - 12+ weeks

See [`docs/SDD-system-design.md`](docs/SDD-system-design.md) section 3 (Roadmap) for full details.

## Phase 1: Dependencies & Build Foundation (v0.4.1)

**Timeline**: 3-4 weeks  
**Risk**: Low (isolated changes)  
**Blocks**: GPU backports, ray tracing

### SDL2 → SDL3 Migration (CRITICAL)

| Task | Status | Notes |
|------|--------|-------|
| Find all SDL2 includes/functions | 📋 TODO | `grep -r "SDL_" src/` |
| Update CMakeLists.txt (3rdparty) | 📋 TODO | Rename/link SDL3 libraries |
| Update launcher event loop | 📋 TODO | `src/launcher/src/*.cpp` |
| Update renderer initialization | 📋 TODO | `src/graphics/presentation/` |
| Test on Windows | 📋 TODO | Run CI: `gh run watch` |
| Test on Linux | 📋 TODO | Run CI |
| Test on macOS | 📋 TODO | Run CI |
| Create v0.4.1 release | 📋 TODO | Version bump, tag, release |

### Update Dependencies

| Dependency | Current | Upstream | Task |
|-----------|---------|----------|------|
| FFmpeg | (version?) | (latest) | 📋 TODO: Update recipe |
| Tracy | (version?) | (latest) | 📋 TODO: Update submodule |
| cpuinfo | (vendored) | (latest) | 📋 TODO: Sync/update |
| CMake minimum | 3.10+ | 3.10+ | ✅ Aligned |

### MSVC/Windows Compatibility

| Issue | PR | Status |
|-------|----|----|
| Macro `near` conflict | #2229 (upstream) | 📋 TODO: Backport |
| STL 14.51 compatibility | #2165 (upstream) | 📋 TODO: Backport |

**Estimated Release Date**: 2026-12 (December)

## Phase 2: Stability & Audio (v0.5.0)

**Timeline**: 2-3 weeks (parallel to Phase 1 end)  
**Risk**: Medium (kernel/audio changes)  
**Depends On**: Phase 1 (SDL3 must be stable)

### Security & Stability Backports

| Upstream Fix | Area | Impact | Status |
|------|------|--------|--------|
| Seek lock release | File I/O | Prevents hangs | 📋 TODO |
| Truncation descriptors | File I/O | Leak prevention | 📋 TODO |
| Event queueing | Kernel | Persistence | 📋 TODO |
| Audio clock sync | Audio | Prevents underrun | 📋 TODO |

### Audio MP3 Support

| Feature | Status | Notes |
|---------|--------|-------|
| sceAjmDecMp3ParseFrame | 📋 TODO | Parse MP3 headers |
| Time-stretching fixes | 📋 TODO | Reduce popping/artifacts |
| Bluetooth haptics | 📋 TODO | DualSense speaker/triggers |

### SaveDataMemory2 Compatibility

| Task | Status |
|------|--------|
| Run SaveDataMemoryTests.cpp | 📋 TODO (after each kernel change) |
| Verify restart survival | 📋 TODO |
| Verify atomic writes | 📋 TODO |

**Estimated Release Date**: 2027-01/02 (January/February)

## Phase 3: GPU Foundation (v0.6.0)

**Timeline**: 8-12 weeks  
**Risk**: High (core graphics path)  
**Depends On**: Phases 1-2 stable

### Render Target Optimization

| Component | Status | Estimated Effort |
|-----------|--------|-----------------|
| Layouts & formats | 📋 TODO | 3-4 weeks |
| BDA memory tracking | 📋 TODO | 2-3 weeks |
| Render target caching | 📋 TODO | 2 weeks |
| Aliasing fixes | 📋 TODO | 1-2 weeks |
| 10-11-11 storage format | 📋 TODO | 1 week |

### Image Page Tracking

| Feature | Status | Benefit |
|---------|--------|---------|
| Efficient readback | 📋 TODO | Faster GPU→CPU transfers |
| CPU page tracking | 📋 TODO | Consistent memory view |

### Testing & Validation

| Test | Status |
|------|--------|
| Regression suite green | 📋 TODO |
| No performance regression | 📋 TODO |
| 3D games boot/run | 📋 TODO |

**Estimated Release Date**: 2027-03+ (March and beyond)

## Phase 4: Ray Tracing (v0.7.0+)

**Timeline**: 12+ weeks  
**Risk**: Very High (largest diff, GPU subsystem rewrite)  
**Depends On**: Phases 1-3 complete and stable

### BVH Support (Bounding Volume Hierarchy)

| Feature | Status | Effort |
|---------|--------|--------|
| BVH structure validation | 📋 TODO | 2-3 weeks |
| Ray traversal kernels | 📋 TODO | 4-6 weeks |
| Shader bindings | 📋 TODO | 1-2 weeks |

### Advanced Features

| Feature | Status | Notes |
|---------|--------|-------|
| Wave64 queries | 📋 TODO | Simd optimization |
| HDR rendering | 📋 TODO | Extended color precision |
| Denoising | 📋 TODO | Image quality |

### Testing & Validation

| Test | Status |
|------|--------|
| Ray tracing tests | 📋 TODO |
| Game compatibility | 📋 TODO |
| Performance benchmarks | 📋 TODO |

**Estimated Release Date**: 2027-05+ (May and beyond)

## Ongoing Maintenance

| Task | Frequency | Owner |
|------|-----------|-------|
| Run regression tests | Every commit | CI (GitHub Actions) |
| Update dependencies | Weekly | Dependabot |
| Security scanning | Weekly | CodeQL |
| Spanish UI translations | Per release | Maintainers |

## Success Criteria

### Phase 1 Success
- [ ] SDL3 migration complete
- [ ] Dependencies updated
- [ ] All platforms compile (Windows, Linux, macOS)
- [ ] Regression tests 17/17 passing
- [ ] v0.4.1 released

### Phase 2 Success
- [ ] Security backports applied
- [ ] Audio MP3 parsing working
- [ ] SaveDataMemory2 still reliable
- [ ] v0.5.0 released

### Phase 3 Success
- [ ] GPU rendering optimized
- [ ] No performance regression
- [ ] Game compatibility improved
- [ ] v0.6.0 released

### Phase 4 Success
- [ ] Ray tracing functional
- [ ] Visual fidelity improved
- [ ] Performance acceptable (60+ FPS on typical hardware)
- [ ] v0.7.0 released

## How to Contribute

1. **Pick a task** from the TODO lists above
2. **Create a spec** in `specs/NNN-slug/` (use `TEMPLATE-spec.md`)
3. **Implement** on a feature branch
4. **Test in CI** with `gh run watch`
5. **Submit PR** with reference to this roadmap
6. **Merge** once approved

**Important**: Follow [`CONTRIBUTING.md`](CONTRIBUTING.md) and [`specs/CONSTITUTION.md`](specs/CONSTITUTION.md) before starting.

## Known Risks & Mitigations

| Risk | Mitigation |
|------|-----------|
| **SDL2→SDL3 API breaking changes** | Phase changes in isolation, extensive CI testing |
| **GPU changes regressing games** | Comprehensive regression test suite, multiple platforms |
| **Ray tracing integration failing** | Phase-by-phase approach, prerequisites must pass first |
| **SaveDataMemory2 incompatibility** | Run SaveDataMemoryTests.cpp after every kernel change |

## Related Documents

- **Full Architecture**: [`docs/SDD-system-design.md`](docs/SDD-system-design.md)
- **Contributing Guide**: [`CONTRIBUTING.md`](CONTRIBUTING.md)
- **Constitution**: [`specs/CONSTITUTION.md`](specs/CONSTITUTION.md)
- **Upstream Analysis**: [`docs/upstream-review-2026-10-12.md`](docs/upstream-review-2026-10-12.md)
- **Improvement Instructions**: [`.claude/instructions/katyps5-improvements.md`](.claude/instructions/katyps5-improvements.md)
- **Sync Workflow**: [`.claude/skills/upstream-sync/SKILL.md`](.claude/skills/upstream-sync/SKILL.md)

---

**Last Updated**: 2026-11-14  
**Maintained By**: erickson558 (fork owner)  
**License**: GPL-2.0-only

Have a task you'd like to pick up? Start here: [`CONTRIBUTING.md`](CONTRIBUTING.md) 🚀

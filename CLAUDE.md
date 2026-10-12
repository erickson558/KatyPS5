# CLAUDE.md

Guidance for Claude Code (and any other AI agent) working in this repository.

## What this project is

KatyPS5 is a fork of [KytyPS5](https://github.com/KytyPS5/KytyPS5) — a free/open-source
PlayStation 5 emulator written in C++, built with CMake + Qt6 + Vulkan. KytyPS5 is itself based on
a heavily modified version of the original [Kyty](https://github.com/InoriRus/Kyty) (MIT-licensed).
This fork is licensed **GPL-2.0-only**, same as upstream — it cannot be relicensed to a permissive
license (Apache/MIT) without consent from every upstream copyright holder, so don't attempt that.

## No local build toolchain

This environment does **not** have CMake, Ninja, clang-cl/clang, Qt6, or the Vulkan SDK installed.
There is no way to compile or run the emulator locally here. Treat GitHub Actions
(`.github/workflows/build.yml`, plus `.github/workflows/codeql.yml`) as the only real compiler and
test runner: push changes to a branch and watch the run (`gh run watch`) before merging to `main`.
See `README.md` → *Building* for the full toolchain a human would need locally.

## Where things live

- `specs/` — Spec-Driven Development artifacts. `specs/CONSTITUTION.md` has the non-negotiables.
  Each feature/change of any real size gets a `specs/NNN-slug/{spec,plan,tasks}.md` folder — read
  the most recent ones before starting new work, and add a new one for new work instead of editing
  code with no written spec.
- `.claude/agents/` — specialized agents for this repo (build/CI verification, security, releases).
- `.claude/skills/` — reusable procedures those agents (and the main assistant) invoke: GitHub
  publishing, the analyze→fix→version→commit workflow, code annotation, security-tooling upkeep.
- `src/` — the emulator core (`kyty_emulator` target) and the Qt launcher (`src/launcher/`,
  `katy_emulator`/`launcher` targets — see naming note below).
- `tests/` — GoogleTest-based regression tests, wired into CTest (`ctest --test-dir <build dir>`).
- `3rdparty/` — vendored/submoduled dependencies. Never hand-edit; update via `.gitmodules` +
  `git submodule update`.

## Naming note (post-rebrand)

The product was renamed from Kyty/KytyPS5 to Katy/KatyPS5 (`specs/001-katyps5-rebrand/`). The
rename is a mechanical case-preserving token substitution (`Kyty`→`Katy`, `KYTY`→`KATY`,
`kyty`→`katy`) with a few deliberate exceptions that must stay untouched because they reference
real external/historical things, not our own branding:
- `LICENSES/Kyty-MIT.txt` — historical license notice for the original Kyty project.
- The README credit line to `InoriRus/Kyty` and `shadps4-emu/shadPS4`.
- The compatibility-database URL in `src/launcher/src/compatibilityDatabase.cpp`
  (`https://kytyps5.github.io/data/compatibility.json`) — an upstream resource; do not rebrand its
  hostname. The former `Nmzik/KytyPS5` release URL returned HTTP 404 and was replaced in 0.3.2.

## Working conventions

- Don't break existing functionality. This is a real, working emulator with real users on the
  upstream project; changes on this fork should be additive/renaming, not regressions.
- Follow semver (`vX.Y.Z`) and keep the version in sync across `CMakeLists.txt` `project()`,
  `CHANGELOG.md`, and the git tag — see `.claude/skills/debug-fix-release/SKILL.md`.
- Use Conventional Commits (`feat:`, `fix:`, `chore:`, `docs:`, `security:` …).
- New/changed C++ code should get comments that explain *why*, not *what* — see
  `.claude/skills/code-annotator/SKILL.md`.
- Publishing (commits, tags, releases) goes through `.claude/skills/github-publish/SKILL.md`,
  which uses the `erickson558` GitHub account already authenticated via `gh`.

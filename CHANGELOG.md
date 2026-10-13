# Changelog

All notable changes to this project are documented here. Format loosely follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/). Versioning is `vX.Y.Z`
(see `specs/CONSTITUTION.md` §3 for what bumps what).

## [0.3.2] - 2026-10-12

### Fixed
- File operations release the file mutex after a rejected negative seek, preventing subsequent
  operations from another thread from hanging (upstream `b9aa0dc3`).
- Rejected read-only truncation opens release their descriptor instead of leaking slots and memory
  (upstream `0f3674f5`).
- Games importing `sceKernelFtruncate` resolve the existing native implementation instead of an
  unresolved import (upstream `048fe9f4`).
- Persistent user events are returned once per wait, allowing following events to be delivered
  while preserving queued payload order (upstream `afbb322b`).
- The launcher fetches compatibility reports from upstream's current endpoint; the old release
  download returns HTTP 404 (upstream `0bbe3011`). Download limits and timeouts are retained.
- Synchronous AudioOut2 PCM submissions use the real output device's clock, avoiding duplicate
  pacing and underruns; asynchronous submissions and contexts without device-backed PCM retain
  queue limits. Mixed speaker/vibration batches use the speaker's clock (upstream `f380d69b`).
- Output batches require device-backed PCM before using the device clock; an empty speaker
  submission alongside vibration retains the fallback wait instead of returning unpaced.
- CPU file reads before renderer initialization no longer abort while trying to invalidate
  nonexistent GPU resources; invalidation is unchanged once a GPU resource manager is installed.

### Added
- Filesystem regression tests for seek lock release, descriptor reuse and native truncation imports.
- Audio/event regression coverage adapted from upstream, including vibration-only queue limits.
- A documented comparison and dependency-aware roadmap in `docs/upstream-review-2026-10-12.md`.

### Changed
- `katy_tests` includes the AudioOut2 test it previously omitted.
- Windows, Linux and macOS CI build/run filesystem and event queue tests alongside existing audio
  and virtual-memory checks. Relevant tests have a 60-second timeout to expose deadlocks.
- Audio timing tests exercise production scheduling with deterministic SDL2 queues, including
  empty-device PCM, mixed speaker/vibration output and fallback pacing.
- Installed packages include the project license and preserved historical license notices.
- Documentation-only pushes skip binary rebuilds/releases and CodeQL; code and build changes,
  pull requests, manual runs and scheduled analysis keep their validation triggers.

The implementation was compiled and regression-tested on Windows, Linux and macOS in GitHub
Actions (17 passing suites). Release packages are produced from successful main builds.

## [0.3.1] - 2026-08-04

### Changed
- `runtimeLinker.cpp`: `RelocateRecord()` called `RegisterStubbedImport()` a second time purely
  to get the thunk address for a log line, after already calling it once to compute the patched
  value. Both calls always returned the same address (`RegisterStubbedImport` dedups by
  `patch_vaddr`), so the second call was a pure-waste linear scan of `g_stubbed_imports`. Cached
  the thunk address from the first call instead. Investigated while looking into
  [KytyPS5#155](https://github.com/KytyPS5/KytyPS5/issues/155) (an access-violation regression
  after unresolved-import stubbing); this change is a real but unproven-unrelated cleanup found
  along the way, not a confirmed fix for that crash - root-causing it further needs a complete
  log (with the register dump) or live debugging, neither available in this environment.

## [0.3.0] - 2026-08-04

### Added
- Spec-Driven Development scaffold (`CLAUDE.md`, `specs/CONSTITUTION.md`, `specs/TEMPLATE-spec.md`,
  `specs/001-katyps5-rebrand/`).
- Claude Code agents (`.claude/agents/build-verifier.md`, `security-auditor.md`,
  `release-manager.md`) and skills (`github-publish`, `debug-fix-release`, `code-annotator`,
  `security-hardening`) for this repo's ongoing maintenance.
- Security tooling: `SECURITY.md`, CodeQL analysis (`.github/workflows/codeql.yml`), Dependabot for
  submodules and GitHub Actions (`.github/dependabot.yml`).
- Donate button in the launcher toolbar and `.github/FUNDING.yml` (PayPal).
- Multi-language infrastructure: Qt Linguist `.ts`/`.qm` pipeline, a Spanish translation
  (`src/launcher/translations/katyps5_es.ts`), and a language switcher in the global settings
  dialog (Automatic / English / Español).

### Changed
- Rebranded from KytyPS5 to **KatyPS5** throughout the build system, source, CI, and docs
  (`Kyty`→`Katy`, `KYTY`→`KATY`, `kyty`→`katy`), forked to `github.com/erickson558/KatyPS5`.
  Historical/external references were deliberately left untouched: the original Kyty MIT notice
  (`LICENSES/Kyty-MIT.txt`), the upstream project credit, and the third-party compatibility-database
  URL. License remains GPL-2.0-only.
- `.github/workflows/build.yml`'s build-origin detection and release gate now recognize
  `erickson558/KatyPS5` as this project's own canonical repository, so releases can be cut from
  this fork (previously only `KytyPS5/KytyPS5` pushes could trigger a release).

### Security
- Windows game/emulator launch no longer passes raw game-folder/log-folder paths through
  `cmd.exe`'s command-line tail unescaped; `&`, `|`, `<`, `>`, `^` are now caret-escaped before
  being handed to `cmd.exe /K`, closing a local command-injection vector via a crafted folder name
  (`src/launcher/src/mainDialog.cpp`).
- The compatibility-database download now aborts once the response exceeds 5 MB instead of
  buffering an unbounded reply into memory (`src/launcher/src/compatibilityDatabase.cpp`).

## [0.2.2] and earlier

See upstream [KytyPS5](https://github.com/KytyPS5/KytyPS5) history — this fork branched from that
project's `0.2.2` release line.

# Changelog

All notable changes to this project are documented here. Format loosely follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/). Versioning is `vX.Y.Z`
(see `specs/CONSTITUTION.md` §3 for what bumps what).

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

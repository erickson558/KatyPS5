# Spec 001: KatyPS5 rebrand + SDD/agents/skills + security hardening + donate button + i18n

## Status
Done (see `plan.md` and `tasks.md` for what actually shipped and in which commit)

## Context
This repo started as a plain clone of the upstream KytyPS5 emulator (`github.com/KytyPS5/KytyPS5`,
GPL-2.0), with read-only access for this GitHub account. The owner (`erickson558`) wants their own
public fork with: a rebrand to "KatyPS5", a Spec-Driven Development workflow, dedicated Claude Code
agents/skills for this project, real security tooling, a PayPal donation button, and multi-language
support — while keeping the emulator working and the license unchanged (GPL-2.0, since relicensing
GPL code to Apache-2.0 isn't legally possible without every contributor's consent).

## Goal
- Repo forked to `github.com/erickson558/KatyPS5` (done), product renamed Kyty→Katy consistently
  across build system, code, CI, and docs.
- `specs/`, `.claude/agents/`, `.claude/skills/` scaffolding in place and used by this very change.
- `SECURITY.md`, CodeQL workflow, Dependabot config added; at least one concrete hardening fix
  applied to the compat-DB/network path.
- A "Donate" control in the launcher UI opening the given PayPal link, plus `.github/FUNDING.yml`.
- Working Qt Linguist i18n infrastructure with English + Spanish translations for the new/top-level
  UI strings.
- Version bumped to `v0.3.0`, `CHANGELOG.md` added, tagged release published from this fork's own
  CI run (the upstream release job only fires for `KytyPS5/KytyPS5` pushes — fixed so this fork can
  release too).

## Non-goals
- Not translating every existing dialog string in the app (infra is extensible; see `plan.md`).
- Not an exhaustive security audit of the ~400k-line emulator core — bounded to the network/file
  paths reachable from the launcher (see `plan.md` Phase 4).
- Not relicensing to Apache-2.0 — stays GPL-2.0-only (`specs/CONSTITUTION.md` §2).
- Not attempting a local compile — no CMake/Ninja/Qt6/Vulkan SDK in this environment; verification
  is via the fork's own GitHub Actions run.

## Approach
See `plan.md` for the phased implementation and `tasks.md` for the checklist actually executed.

## Risk / impact
- Blast radius: touches almost every non-`3rdparty` file (mechanical token rename), plus new files
  under `.claude/`, `specs/`, `.github/`, and a handful of Qt launcher source files for the donate
  button and i18n. `3rdparty/` submodules are untouched.
- Verification: pushed to `rebrand/katyps5` branch first; GitHub Actions must go green on all three
  platform jobs (Windows/Linux/macOS) before merging to `main`.
- Backward compatibility: the settings file/registry key renames (`Kyty.ini`/`HKCU\Software\Kyty`
  → `Katy.ini`/`HKCU\Software\Katy`) mean a prior KytyPS5 install's settings won't carry over —
  acceptable since this is a new fork identity with no existing KatyPS5 user base.

## Version impact
Minor (`0.2.2` → `0.3.0`): new features added (donate button, i18n, security tooling), nothing
removed or made backward-incompatible for anyone starting fresh on this fork.

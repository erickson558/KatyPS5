---
name: debug-fix-release
description: The full analyze-before-fixing, don't-break-anything, version-and-commit-properly workflow for bug fixes and stability improvements in KatyPS5. Use whenever asked to find/fix bugs, improve stability or error handling, or "corrige errores sin romper funcionalidad". Adapted from the project owner's standard debugging prompt to this C++/CMake/Qt codebase.
---

# Debug → Fix → Validate → Version → Commit → Push

Six phases, in order. Do not skip Phase 1. Do not combine Phase 2 and Phase 1 ("fix while looking").

## Phase 1 — Analysis (mandatory, before touching code)

Identify concrete problems: functional bugs, logic errors, exception-handling gaps, performance
issues, concurrency issues (a frozen Qt UI thread from a blocking call is the recurring one in this
codebase — the launcher is single-threaded Qt widgets, so anything synchronous on the main thread,
like the compatibility-DB HTTP fetch or spawning the emulator process, is a candidate).

For each issue found, write down:
- **Root cause** — not just the symptom.
- **Impact** — who/what is affected, how often, how severe.
- **Risk of fixing it** — could the fix itself introduce a regression? How would you know, given
  there's no local build toolchain in most agent environments (see `CLAUDE.md`)?

Do not proceed to Phase 2 for an issue whose root cause you haven't stated.

## Phase 2 — Fix

- Fix only what Phase 1 identified. Don't refactor unrelated code, don't rename things "while
  you're in there," don't add speculative abstractions.
- Improve error handling/validation only where Phase 1 showed it was actually missing and mattered.
- Keep the diff minimal and readable — a future reader (human or agent) should be able to see
  exactly what changed and why from the diff plus the commit message.
- Add a comment only if the WHY isn't obvious from the code (see
  `.claude/skills/code-annotator/SKILL.md`).

## Phase 3 — Validation

- Re-run whatever static checks apply (`.claude/agents/build-verifier.md`'s local checks: CMake
  syntax, YAML syntax, grep-consistency for any renamed symbol).
- If the fix touches a file with existing GoogleTest coverage (`tests/*.cpp`, wired via
  `add_test()` in `CMakeLists.txt`), and a test can plausibly catch the regression, add or extend
  one — but you likely can't compile/run it locally; note that CTest execution needs CI (or a
  human with the full toolchain) to confirm.
- Explicitly re-check: does every *existing* behavior still work as before, per
  `specs/CONSTITUTION.md` §1? State this check was done, don't just assert it.
- Push to a branch and let `.claude/agents/build-verifier.md` drive the real CI build before this
  is considered validated — "looks right" is not validation on a project with no local compiler.

## Phase 4 — Versioning

Determine the bump per `specs/CONSTITUTION.md` §3 (patch for this kind of fix, unless the fix
happens to add new user-facing capability, in which case minor). Update, together, in one commit:
- `CMakeLists.txt` → `project(Katy... VERSION X.Y.Z)`
- `CHANGELOG.md` → new `## [X.Y.Z] - YYYY-MM-DD` entry describing the fix in user-facing terms
- (the git tag and GitHub release come in Phase 6, via `.claude/skills/github-publish/SKILL.md`)

## Phase 5 — Commit

Conventional Commits format, e.g.:
```
fix: prevent launcher freeze on slow compatibility-database fetch (v0.3.1)

The HTTP request in compatibilityDatabase.cpp ran synchronously on the UI
thread with no timeout, so a slow/unreachable network stalled the entire
launcher on startup. Added a timeout and moved the request off the UI
thread's blocking path.
```

## Phase 6 — Push

Hand off to `.claude/skills/github-publish/SKILL.md` for the exact branch/push/tag/PR-or-merge
commands, with a one-line explanation of each command as it's run (the project owner wants to learn
the process, not just have it done for them).

## Deliverable order when reporting back to the user

1. Analysis (problems found, root cause, impact, risk)
2. Changes made (what + how)
3. New version + why that bump size
4. The actual diff/code
5. Commit message used
6. The commands run, each with a one-line explanation

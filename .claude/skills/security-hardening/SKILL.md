---
name: security-hardening
description: How to run and interpret static analysis (cppcheck/clang-tidy), and how to maintain CodeQL/Dependabot/SECURITY.md over time for KatyPS5. Use when asked to check for vulnerabilities, harden the project against attacks, or review/update the security tooling itself (as opposed to reviewing one specific diff, which is .claude/agents/security-auditor.md's job).
---

# Security tooling for KatyPS5

## What's in place and why

- **`.github/workflows/codeql.yml`** — GitHub CodeQL's C/C++ query suite, run on push/PR to `main`
  and on a weekly schedule. This is the primary automated SAST for this project; it runs in CI
  where the full build toolchain exists (CodeQL needs to actually build the code to analyze it).
- **`.github/dependabot.yml`** — watches two ecosystems: `gitsubmodule` (the 12 vendored
  dependencies under `3rdparty/`) and `github-actions` (the versions pinned in `build.yml` and
  `codeql.yml`). Review each PR it opens against the canonical-vs-fork note below before merging.
- **`SECURITY.md`** — reporting policy. Keep it current if the maintainer/contact changes.

## Running static analysis locally, when tools ARE available

This environment usually has neither `cppcheck` nor `clang-tidy` installed (no local C++ toolchain
at all — see `CLAUDE.md`). Check first: `command -v cppcheck`, `command -v clang-tidy`. If present:

```
cppcheck --enable=warning,portability,performance --inline-suppr --std=c++20 \
  -I src -I src/common <specific-dir-or-file>   # scope it — don't run unscoped over 3rdparty/
```

`clang-tidy` needs a compile database (`compile_commands.json`, produced by CMake's
`-DCMAKE_EXPORT_COMPILE_COMMANDS=ON`) which requires the full CMake configure step — not usually
possible here. If neither tool is available, say so and rely on manual review
(`.claude/agents/security-auditor.md`) plus CodeQL's CI results instead of fabricating a report.

## Supply-chain review checklist (for new/updated submodules or Actions)

1. Is the source the canonical upstream project, or a fork/mirror maintained by a third party?
   (Known accepted exceptions already in this repo: `ext-ffmpeg-core` and `ext-LibAtrac9`, both
   `shadps4-emu`-maintained forks — don't re-flag these unless their source changes.)
   Additionally, `compatibilityDatabase.cpp` downloads a compatibility-report JSON at runtime from
   `Nmzik/KytyPS5` releases — this is a *runtime* third-party data dependency, not a build-time one,
   worth the same scrutiny (is it still the actively maintained source? does the launcher validate
   the response before trusting it?).
2. Is a GitHub Action pinned to a full commit SHA or at least a major-version tag, not a mutable
   branch ref like `@main`?
3. Does the dependency's license stay compatible with GPL-2.0-only (this project's license)?

## Reviewing CodeQL findings

Triage in the Security tab (`gh api repos/erickson558/KatyPS5/code-scanning/alerts` also works from
the CLI). For each alert: is it a true positive in reachable code, or in an unreachable/test-only
path? Fix true positives via `.claude/skills/debug-fix-release/SKILL.md`; dismiss false positives
with a reason via `gh api`, don't just leave them open and unaddressed.

## Keeping SECURITY.md honest

State plainly what's actually covered: this fork (`erickson558/KatyPS5`) vs. upstream
(`KytyPS5/KytyPS5`) — a report about upstream code should be routed there, not handled silently
here as if this fork were the canonical project.

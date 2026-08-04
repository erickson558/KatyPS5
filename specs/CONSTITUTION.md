# KatyPS5 Project Constitution

Non-negotiable principles for any change made to this repository, by a human or an AI agent.
These take precedence over convenience, speed, or a specific spec if they ever conflict.

## 1. Never break existing functionality

KatyPS5 is a working emulator. A change confined to one platform's code path must not regress
other platforms; a change to shared code must not regress Windows (the primary target). Prefer
additive changes and renames over rewrites. If a change's safety can't be verified locally (no
build toolchain in most agent environments — see `CLAUDE.md`), it must be verified via CI
(`.github/workflows/build.yml`) before merging to `main`.

## 2. License stays GPL-2.0-only

This code is derived from GPL-2.0 work. It cannot be relicensed to a permissive license (Apache-2.0,
MIT, etc.) without consent from every upstream copyright holder — that consent doesn't exist, so
don't propose it. Preserve the original Kyty MIT notice (`LICENSES/Kyty-MIT.txt`) and attribution
chain (Kyty → KytyPS5 → KatyPS5) unchanged.

## 3. Semantic versioning, kept in sync everywhere

Version format is `vX.Y.Z`. Increment:
- **patch** — bug fixes, no behavior/API change.
- **minor** — new functionality, backward compatible.
- **major** — breaking changes to CLI flags, config file format, or save-data compatibility.

The version must match in `CMakeLists.txt` (`project(... VERSION X.Y.Z)`), `CHANGELOG.md`, the git
tag, and the GitHub Release title. See `.claude/skills/debug-fix-release/SKILL.md`.

## 4. Analyze before fixing

No blind fixes. Before changing code to fix a bug: identify the root cause, state the impact, and
state the risk of the fix itself. This applies to every bug fix regardless of size.

## 5. Security is a first-class concern

Every change that touches network I/O (e.g. the compatibility-database fetch), file/process
handling, or memory management should be considered through a security lens — see
`.claude/agents/security-auditor.md` and `.claude/skills/security-hardening/SKILL.md`. New
dependencies (including submodules) get a quick supply-chain sanity check (is this the canonical
upstream, or a fork/mirror? who maintains it?).

## 6. Comments explain WHY, not WHAT

New or changed code gets a comment only when the reasoning isn't obvious from the code itself
(a hidden constraint, a workaround, a non-obvious invariant). Don't blanket-comment the existing
~400k-line codebase — annotate what you touch. See `.claude/skills/code-annotator/SKILL.md`.

## 7. Spec-first for anything non-trivial

Any change bigger than a one-line fix gets a `specs/NNN-slug/{spec,plan,tasks}.md` before code is
written, using `specs/TEMPLATE-spec.md`. Trivial fixes (typos, obvious one-liners) don't need one.

## 8. Respect the project's own AI-use policy

The upstream README states that AI tools may assist, but repository communication (PR descriptions,
issue comments) must come from the human contributor, who remains responsible for reviewing and
testing all submitted code. This applies in full if/when contributing back upstream to
`KytyPS5/KytyPS5`; on this fork, AI-authored commits are fine as long as a human (the repo owner)
has reviewed the actual diff before it's pushed.

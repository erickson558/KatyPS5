# Plan for Spec 001

1. **SDD scaffold** — `CLAUDE.md`, `specs/CONSTITUTION.md`, `specs/TEMPLATE-spec.md`, this spec
   folder.
2. **Agents** — `.claude/agents/build-verifier.md`, `security-auditor.md`, `release-manager.md`.
3. **Skills** — `.claude/skills/github-publish/`, `debug-fix-release/`, `code-annotator/`,
   `security-hardening/` (each a `SKILL.md`).
4. **Security hardening** — `SECURITY.md`, `.github/workflows/codeql.yml`,
   `.github/dependabot.yml`; bounded review of `compatibilityDatabase.cpp` (network fetch) and
   `mainDialog.cpp` (process spawn / file paths) with concrete fixes where found.
5. **Rebrand** — scripted case-preserving token rename (`Kyty`→`Katy`/`KYTY`→`KATY`/`kyty`→`katy`)
   across all non-`3rdparty` files except the 4 requiring hand-editing (`README.md`,
   `.github/workflows/build.yml`, `src/launcher/src/compatibilityDatabase.cpp`,
   `LICENSES/Kyty-MIT.txt` untouched); path renames for
   `src/kytyGitVersion.h.in`→`katyGitVersion.h.in` and the issue template file; hand-edit the 3
   excepted files to preserve external/historical references while rebranding everything else in
   them.
6. **Donate button** — `QToolButton` in the launcher toolbar + `.github/FUNDING.yml`.
7. **i18n** — `src/launcher/translations/katyps5_{en,es}.ts`, CMake `lupdate`/`lrelease` wiring,
   `QTranslator` load in `main.cpp`, language combo in the global-settings dialog.
8. **Versioning & publish** — bump `CMakeLists.txt` version, `CHANGELOG.md`, fix the CI
   origin-detection/release-gate logic to also work for this fork, scoped conventional commits,
   push branch, `gh run watch`, merge to `main`, tag `v0.3.0`, confirm release.

## Verification
- Grep-audit for stray `kyty` tokens after the scripted rename (excluding the 4 documented
  exceptions).
- Python YAML-parse `build.yml` after hand-editing it.
- GitHub Actions green on Windows/Linux/macOS jobs on the `rebrand/katyps5` branch before merge.

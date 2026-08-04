# Security Policy

## Scope

This policy covers **this fork**, `erickson558/KatyPS5`. It is a personal fork of the upstream
[KytyPS5](https://github.com/KytyPS5/KytyPS5) emulator. If you found an issue that also affects
upstream KytyPS5 (most of the emulator core is shared, unchanged code), please also report it
there — this fork does not speak for the upstream project or its maintainers.

## Supported versions

Only the latest tagged release and the `main` branch receive security fixes. There is no
long-term-support branch.

## Reporting a vulnerability

Please report security issues privately rather than opening a public GitHub issue, via GitHub's
private vulnerability reporting: open the **Security** tab on this repository →
**Report a vulnerability**. Include:

- A description of the issue and its impact.
- Steps to reproduce, or a minimal example (e.g. a crafted game-folder name, a malformed
  compatibility-database response, a specific CLI flag combination).
- The platform (Windows/Linux/macOS) and build (source/CI artifact) affected.

You should get an initial response within a few days. This is a small, single-maintainer fork —
please be patient; complex fixes go through the same analyze-before-fixing discipline described in
`specs/CONSTITUTION.md` §4 rather than being rushed out.

## What's in scope

- The launcher (`src/launcher/`): process spawning, network access (compatibility-database fetch),
  settings/file handling.
- The build/release pipeline (`.github/workflows/`).
- Anything specific to this fork's own additions (donate button, i18n, rebrand-related changes).

## What's likely out of scope for this fork specifically

- Deep emulator-core issues (`src/graphics`, `src/kernel`, `src/libs`) that are identical to
  upstream KytyPS5 and not introduced by this fork — report those upstream too, since a fix here
  alone won't reach upstream users.
- Vulnerabilities in vendored third-party code under `3rdparty/` — report to the upstream project
  for that dependency; we'll take a version bump via Dependabot once one's available.
- Anything requiring local code execution the attacker already has (this is a native desktop app;
  "an attacker who can already run arbitrary code on your machine can do bad things" is not a
  vulnerability in the app itself).

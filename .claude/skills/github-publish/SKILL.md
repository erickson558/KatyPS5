---
name: github-publish
description: Publish local changes in this repo to GitHub using the erickson558 account — branching, committing, pushing, tagging, and creating releases against the erickson558/KatyPS5 fork (with KytyPS5/KytyPS5 wired as `upstream` for pulling upstream fixes later). Use whenever the user asks to "push", "subir a github", "crea un commit", "tag a release", or similar.
---

# GitHub publishing for KatyPS5

This repo's remotes (already configured):
```
origin    https://github.com/erickson558/KatyPS5.git   (the fork we own and push to)
upstream  https://github.com/KytyPS5/KytyPS5.git        (read-only upstream, for `git fetch upstream` later)
```

Authenticated account: `erickson558` via `gh auth status` (token scopes: gist, read:org, repo,
workflow). If `gh auth status` ever shows a different account active, switch with
`gh auth switch --hostname github.com --user erickson558` before pushing — never push as the wrong
account.

## Standard flow

1. **Check state first, always**: `git status` — never stage/commit over unreviewed changes.
   Stash (`git stash push -u`) anything unrelated to the current task before proceeding.
2. **Branch, don't commit straight to `main`** for anything non-trivial:
   `git checkout -b <type>/<short-slug>` (e.g. `fix/compat-db-timeout`, `feat/donate-button`).
3. **Stage specific paths**, never a blind `git add -A`/`git add .`:
   `git add <path1> <path2> ...`, then `git status` again to confirm exactly what's staged before
   committing. If anything unexpected (a stray local file, something that looks like a credential)
   shows up staged, unstage it (`git restore --staged <path>`) and ask before proceeding.
4. **Commit** with a Conventional Commits message (see `.claude/skills/debug-fix-release/SKILL.md`
   for the fuller analyze→fix→commit flow when the change is a bug fix):
   ```
   git commit -m "$(cat <<'EOF'
   <type>: <summary>

   <body — the why, not just the what>
   EOF
   )"
   ```
5. **Push the branch**: `git push -u origin <branch>`.
6. **Verify before merging** — hand off to `.claude/agents/build-verifier.md` to confirm CI is
   green on this branch (`gh run list --branch <branch>`, `gh run watch <run-id>`) before merging
   to `main`. Don't merge on faith; there's no local compiler in this environment to fall back on.
7. **Merge to `main`** once green: `git checkout main && git merge --ff-only <branch>` (fast-forward
   only — if it's not a fast-forward, rebase the branch first rather than creating a merge commit
   for a single-maintainer repo) `&& git push origin main`.
8. **Tag a release** when the change warrants one — see `.claude/agents/release-manager.md` for
   version-bump rules:
   ```
   git tag -a vX.Y.Z -m "<summary>"
   git push origin vX.Y.Z
   ```
   Then either let `.github/workflows/build.yml`'s release job pick it up (if it's wired for this
   repo — check the `github.repository ==` guard) or run
   `gh release create vX.Y.Z --generate-notes` directly.

## Enabling Actions on the fork (one-time, if needed)

Forks sometimes have Actions disabled by default. Check with:
```
gh api repos/erickson558/KatyPS5/actions/permissions
```
If `"enabled": false`, turn it on:
```
gh api -X PUT repos/erickson558/KatyPS5/actions/permissions -f enabled=true
```

## Pulling from upstream later

```
git fetch upstream
git log upstream/main --oneline -20   # see what's new before merging
git merge upstream/main               # or cherry-pick specific fixes
```
Expect merge conflicts on every renamed Kyty→Katy identifier if upstream has touched the same
lines — resolve in favor of the Katy naming, taking the *logic* change from upstream.

## Hard rules

- Never `--force` push `main`.
- Never `git commit --amend` a commit that's already been pushed — make a new commit instead.
- Never pass `--no-verify` to skip hooks, or `-c commit.gpgsign=false`.
- Never commit anything under `_Build/`, `.env`, or files that look like they hold credentials —
  check `.gitignore` covers build output directories before the first commit of a session.

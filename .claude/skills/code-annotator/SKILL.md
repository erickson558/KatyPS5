---
name: code-annotator
description: Add clear, purposeful comments to C++/CMake/Qt code so a reader understands what a non-obvious piece of code is doing and why. Use when the user asks to "comenta el código", "explica qué hace cada parte", or when adding new code that has a non-obvious constraint. Applies to code being written or touched now — not a blanket pass over the whole existing codebase.
---

# Commenting KatyPS5 code

## Scope discipline

This is a fork of a ~400k-line emulator. "Comment every part of the code" is not achievable (or
useful) as a single blanket pass, and it isn't what the underlying request actually needs. Apply
this skill to:
- Files you are already editing for some other reason (a fix, a feature).
- A specific file/function the user names explicitly ("explain what mainDialog.cpp does").
- New code you're writing as part of any task.

If asked to "comment the whole project," say so plainly and propose the realistic version: explain
the architecture at a higher level (a short doc/README section per major subsystem — `src/graphics`,
`src/kernel`, `src/libs`, `src/launcher`) rather than a comment on every line of existing code.

## What makes a good comment here (WHY, not WHAT)

Good — explains a non-obvious constraint, invariant, or reason:
```cpp
// PS5 GPU command buffers must stay 256-byte aligned or the Vulkan driver
// silently corrupts adjacent allocations on AMD hardware.
alignas(256) uint8_t command_buffer[COMMAND_BUFFER_SIZE];
```
```cpp
// Fetched once at launcher startup; a slow/dead server must not block the
// UI thread, hence the explicit timeout instead of Qt's default (no timeout).
request.setTimeout(5000);
```

Bad — restates what the code already says, delete these on sight when editing nearby code:
```cpp
// Loop over all games
for (auto& game : games) { ... }
```
```cpp
// Set the window title
setWindowTitle(title);
```

## For explaining existing code to the user (not modifying it)

When asked "what does this file/function do", answer in prose in your response — walk through the
control flow, name the key data structures, point out anything surprising — rather than sprinkling
comments into code that isn't otherwise being changed. Only add comments to unmodified code if the
user explicitly asks for the comments to be added to the file itself, not just explained.

## Style

- One line above the code it explains, or a trailing `//` for a short aside — match whatever the
  surrounding function already does.
- No comment banners/dividers (`// ==========`), no author/date tags, no commented-out old code.
- Doxygen-style (`/** ... */`) only on public API surfaces that don't already have it, and only if
  the file already uses that convention elsewhere — don't introduce a new documentation style
  inconsistent with its neighbors.

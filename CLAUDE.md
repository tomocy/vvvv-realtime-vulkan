# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Goal

Implement advanced real-time 3D rendering techniques from scratch using Vulkan, at a level suitable for games from 2026 onward — for the purpose of studying and reproducing them.

## Development Approach

Design with real-time frame-by-frame rendering in mind. During development, render a single frame and write it to an image file for validation — real-time display is impractical in the devcontainer on macOS. Real-time rendering is verified on Steam Deck when needed.

## Planning

Before implementing anything, write a plan to `doc/plan/{name}.md`. The `doc/plan/` directory is excluded from git via `.git/info/exclude` — plans are local only and never committed.

## References

For implementation, rely on authoritative primary sources in real-time 3D rendering such as GDC materials.

## Commands

All commands run inside the devcontainer:

```sh
devcontainer exec --workspace-folder . make build/vvvv  # configure + build
devcontainer exec --workspace-folder . make run          # build and run
devcontainer exec --workspace-folder . make clean        # remove build/
```

Before declaring a task complete, always run clang-format and clang-tidy:

```sh
devcontainer exec --workspace-folder . clang-format-18 -i <files>
devcontainer exec --workspace-folder . clang-tidy-18 <files>
```

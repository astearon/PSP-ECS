# Copilot instructions — PSP-ECS

Purpose: give an AI coding agent exactly the context needed to be productive editing this PSP homebrew C project.

Big picture
- Target: Sony PSP (EBOOT.PBP) using PSPSDK/pspdev and raylib4Psp.
- Architecture: small ECS game framework.
  - Component & ECS core: `include/ecs.h`, `src/ecs.c`.
  - Entry + high-level flow: `src/main.c` (initialization, main loop, system dispatch).
  - Scenes / gameplay: `src/scene.c`, `src/menu.c`.
  - Input mapping: `src/keybinds.c`.
  - Camera & rendering helpers: `include/camera.h`, `src/camera.c`.

Build & developer workflow (explicit)
- Primary build: run `make` at the repository root (see `BUILD.md`).
- Clean: `make clean`.
- Requires: PSP toolchain (`psptoolchain`) and `raylib4Psp` installed into `$PSPDEV/psp/sdk/`.
  - Toolchain install (example):
    ```bash
    git clone https://github.com/pspdev/psptoolchain.git
    cd psptoolchain
    ./toolchain.sh
    export PSPDEV=/usr/local/pspdev
    export PATH=$PATH:$PSPDEV/bin
    ```
  - raylib4Psp install (example):
    ```bash
    git clone https://github.com/raylib4Consoles/raylib4Psp.git
    cd raylib4Psp
    make -f Makefile.PSP
    make -f Makefile.PSP install
    ```
- Fast iteration: use the PPSSPP emulator to run the generated `EBOOT.PBP` (see `BUILD.md`).

Project-specific conventions & patterns
- ECS changes: to add a component type, update `include/ecs.h` (type and ids) and implement allocation/handling in `src/ecs.c`.
- Systems: functions are named like `System_<Name>()` and are invoked from the main loop/scene files (see `src/scene.c`). Follow that pattern.
- Rendering & debug: uses PSP-specific debug helpers like `pspDebugScreenPrintf()` and raylib4Psp APIs — prefer those over desktop-only APIs.
- Input: central mapping lives in `src/keybinds.c`; changing control behavior should start there.
- No unit tests in repo. Use emulator (PPSSPP) for runtime checks.

Integration points & external dependencies
- PSPSDK/pspdev (compiler, linker): builds use `psp-gcc`, etc.; do not replace toolchain settings in Makefile without testing on device/emulator.
- raylib4Psp: rendering/audio/input helpers. Headers expected in `$PSPDEV/psp/sdk/include/` and libs in `$PSPDEV/psp/sdk/lib/`.
- Packaging: final artifact is `EBOOT.PBP` created by the Makefile.

Editing guidance for AI agents
- Be conservative: small, focused edits; keep PSP/toolchain-specific build flags intact.
- Follow existing C style: clear identifiers (avoid one-letter vars), minimal inline comments, match existing indentation.
- Where to add features:
  - New component: `include/ecs.h` + `src/ecs.c`.
  - New system: create `src/system_<name>.c` or add `System_<Name>()` in an existing file and register/ call it from `src/scene.c` or `src/main.c`.
- Debugging: suggest adding `-DDEBUG` to `CFLAGS` (Makefile) and use `pspDebugScreenPrintf()` for early validation.

Examples (concrete pointers)
- Add component: update `include/ecs.h` enum of components and mirror initialization in `src/ecs.c`.
- Add rendering system: follow `System_Render()` pattern used in `src/scene.c` and use raylib4Psp drawing calls.
- Adjust controls: modify mappings in `src/keybinds.c` and check usages across `menu.c` and `scene.c`.

When uncertain
- If a change touches build, packaging, or the Makefile, run `make` and test on PPSSPP before proposing the change as complete.

What I couldn't infer
- No automated tests; no CI config found. If you want CI/formatting rules, ask the maintainers for preferred toolchain targets and lints.

Ask the repo owner which PSP firmware targets and PPSSPP settings to prefer for compatibility checks.

If you'd like changes merged into this file (clarifications, extra examples, or CI additions), tell me what to add.

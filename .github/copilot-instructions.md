# Copilot instructions — PSP-ECS

Purpose: give an AI coding agent exactly the context needed to be productive editing this PSP homebrew C project.

Big picture
- Target: Sony PSP (EBOOT.PBP) using PSPSDK/pspdev and raylib4Psp.
- Architecture: data-oriented ECS (Entity Component System) game framework.
  - Component & ECS core: `include/ecs.h`, `src/ecs.c` (max 256 entities, fixed-size pools).
  - Entry + high-level flow: `src/main.c` (PSP callbacks, main loop at 60fps, system dispatch).
  - Scenes / gameplay: `src/scene.c` (test scene creation, PSP save/load), `src/menu.c`.
  - Input mapping: `src/keybinds.c` (remappable action→button mappings).
  - Camera & rendering helpers: `include/camera.h`, `src/camera.c` (analog stick rotation, d-pad movement).

Build & developer workflow (explicit)
- Primary build: run `make` at the repository root (see `BUILD.md`). Generates `EBOOT.PBP` (~60KB homebrew).
- Clean: `make clean`.
- Debug build: already enabled with `-g -O0` in CFLAGS; add `-DDEBUG` for custom debug macros.
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
    ./pspbuild.sh  # Installs to $PSPDEV/psp/sdk/
    ```
- Fast iteration: use the PPSSPP emulator to run the generated `EBOOT.PBP` (see `BUILD.md`).

Project-specific conventions & patterns
- ECS architecture: entities are just integer IDs (0-255), components are heap-allocated data structs, systems are functions named `System_<Name>()`.
- Component addition flow: 
  1. Add enum to `ComponentType` in `include/ecs.h`
  2. Define struct (e.g., `MyComponent`) in `include/ecs.h`
  3. Add `case COMPONENT_MY:` to `ECS_AddComponent()` in `src/ecs.c` with malloc + initialization
  4. Update `COMPONENT_COUNT` if needed
- Systems: invoked manually from main loop (`src/main.c`) or scene files. See `System_Render()` (line 115-185 in `src/ecs.c`) for pattern: iterate entities, check `ECS_HasComponent()`, get components, do work.
- Input handling: uses double-buffered `SceCtrlData` (pad/oldPad) to detect button-down events. Central action→button mapping in `src/keybinds.c` via `Keybinds_GetBinding()`.
- PSP save system: custom implementation in `src/scene.c` using `sceIo*` APIs, auto-detects ms0:/ef0: mount, creates `PSP/SAVEDATA/` structure. See `Scene_SaveToFile()`/`Scene_LoadFromFile()`.
- Rendering: raylib4Psp uses OpenGL ES subset via `rlgl.h`. Custom helpers like `DrawPlaneWireframe()` (line 7 in `src/ecs.c`) for wireframe rendering.
- Memory: PSP has 32MB RAM. Component allocation uses `malloc()` with null checks; be mindful of entity/component counts.
- No unit tests in repo. Runtime validation via PPSSPP emulator + `pspDebugScreenPrintf()` for early debugging.

Integration points & external dependencies
- PSPSDK/pspdev (compiler, linker): builds use `psp-gcc`, etc.; do not replace toolchain settings in Makefile without testing on device/emulator.
- raylib4Psp: rendering/audio/input helpers. Headers expected in `$PSPDEV/psp/sdk/include/` and libs in `$PSPDEV/psp/sdk/lib/`.
- Packaging: final artifact is `EBOOT.PBP` created by the Makefile using PSP build.mak rules.
- PSP-specific libs: `-lpspctrl` (input), `-lpspdisplay` (screen), `-lpspgu`/`-lpspge` (graphics), `-lpspaudio` (sound), `-lpspvfpu` (VFPU math acceleration).
- Thread model: main thread has `THREAD_ATTR_USER | THREAD_ATTR_VFPU`, exit callback on separate thread (see `SetupGameCallbacks()` in `src/main.c`).

Editing guidance for AI agents
- Be conservative: small, focused edits; keep PSP/toolchain-specific build flags intact.
- Follow existing C style: clear identifiers (avoid one-letter vars), minimal inline comments, match existing indentation.
- Where to add features:
  - New component: `include/ecs.h` + `src/ecs.c`.
  - New system: create `src/system_<name>.c` or add `System_<Name>()` in an existing file and register/call it from `src/scene.c` or `src/main.c`.
- Debugging: suggest adding `-DDEBUG` to `CFLAGS` (Makefile) and use `pspDebugScreenPrintf()` for early validation.
- Key architectural constraints:
  - MAX_ENTITIES = 256, COMPONENT_COUNT = 4 (Transform, Renderable, Camera, Input).
  - Screen resolution: 480×272 pixels (PSP native).
  - Target framerate: 60fps (see `SetTargetFPS(60)` in `src/main.c`).

Examples (concrete pointers)
- Add component: update `include/ecs.h` enum of components and mirror initialization in `src/ecs.c`.
- Add rendering system: follow `System_Render()` pattern used in `src/scene.c` and use raylib4Psp drawing calls.
- Adjust controls: modify mappings in `src/keybinds.c` and check usages across `menu.c` and `scene.c`.
- Button-down detection pattern (from `src/main.c` line 117): `(pad.Buttons & button) && !(oldPad.Buttons & button)`

When uncertain
- If a change touches build, packaging, or the Makefile, run `make` and test on PPSSPP before proposing the change as complete.
- For PSP save/load, check `src/scene.c` (lines 1-100) for mount detection and directory structure patterns.

What I couldn't infer
- No automated tests; no CI config found. If you want CI/formatting rules, ask the maintainers for preferred toolchain targets and lints.
- Ask the repo owner which PSP firmware targets and PPSSPP settings to prefer for compatibility checks.

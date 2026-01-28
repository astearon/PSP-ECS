2# Building PSP-ECS

This document provides detailed instructions for building the PSP-ECS project.

## Prerequisites

### 1. PSP Toolchain
You need to install the PSP development toolchain (pspdev/psptoolchain).

#### Linux/macOS
```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get install build-essential autoconf automake cmake doxygen bison flex \
    libncurses5-dev libsdl1.2-dev libreadline-dev libusb-dev texinfo \
    libgmp3-dev libmpfr-dev libelf-dev libmpc-dev libfreetype6-dev zlib1g-dev \
    libtool libtool-bin git wget gawk

# Clone and build the toolchain
git clone https://github.com/pspdev/psptoolchain.git
cd psptoolchain
./toolchain.sh
```

#### Set Environment Variables
Add to your `~/.bashrc` or `~/.zshrc`:
```bash
export PSPDEV=/usr/local/pspdev
export PATH=$PATH:$PSPDEV/bin
```

### 2. raylib4Psp
Install raylib for PSP from https://github.com/raylib4Consoles/raylib4Psp

```bash
git clone https://github.com/raylib4Consoles/raylib4Psp.git
cd raylib4Psp
make -f Makefile.PSP
make -f Makefile.PSP install
```

This will install raylib to `$PSPDEV/psp/sdk/lib/` and headers to `$PSPDEV/psp/sdk/include/`.

## Building the Project

### Standard Build
```bash
cd PSP-ECS
make
```

This will:
1. Compile all `.c` files in `src/` to `.o` object files
2. Link the object files together
3. Create an ELF executable
4. Package it as `EBOOT.PBP` for PSP

### Clean Build
```bash
make clean
make
```

### Cleaning
```bash
make clean
```

## Deploying to PSP

### Option 1: Physical PSP
1. Connect your PSP via USB in USB mode
2. Copy `EBOOT.PBP` to `ms0:/PSP/GAME/PSP-ECS/` on your Memory Stick
3. Disconnect USB and launch from the Game menu

### Option 2: PPSSPP Emulator
1. Install PPSSPP from https://www.ppsspp.org/
2. Run PPSSPP
3. Navigate to and select the `EBOOT.PBP` file
4. The game will launch in the emulator

## Troubleshooting

### Build Errors

#### Error: "psp-gcc: command not found"
- The PSP toolchain is not installed or not in your PATH
- Solution: Install the toolchain and set the PSPDEV environment variable

#### Error: "raylib.h: No such file or directory"
- raylib4Psp is not installed
- Solution: Install raylib4Psp as described above

#### Linker errors about raylib functions
- raylib4Psp library is not installed correctly
- Solution: Reinstall raylib4Psp and ensure it's in `$PSPDEV/psp/sdk/lib/`

### Runtime Errors

#### Black screen on launch
- Check that all required libraries are included in LIBS in Makefile
- Ensure the PSP firmware is compatible (3.00 or higher recommended)

#### Controls not responding
- Check button mappings in `src/keybinds.c`
- Ensure controller is properly initialized

## Development Tips

### Quick Development Cycle
For faster iteration:
1. Use PPSSPP emulator for testing
2. Enable debug logging by adding `-DDEBUG` to CFLAGS
3. Use `make clean && make` after header changes

### Debugging
Add debug output:
```c
pspDebugScreenPrintf("Debug message: %d\n", value);
```

### Code Structure
- Keep components small and focused
- Add new component types in `include/ecs.h` and `src/ecs.c`
- Add new systems following the pattern in `System_Render()`

## Additional Resources
- [PSP Development Wiki](http://ps2dev.org/psp/)
- [PSPSDK Documentation](http://psp.jim.sh/pspsdk-doc/)
- [raylib Documentation](https://www.raylib.com/)
- [PSP Homebrew Community](https://www.reddit.com/r/PSP/)

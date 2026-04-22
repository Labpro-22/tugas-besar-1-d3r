# Monopoly Game - Build and Installation Guide

## Overview

This guide provides instructions for building the Monopoly Game project across Windows, macOS, and Linux platforms using CMake and the raylib graphics library.

---

## Prerequisites

### Common Requirements
- CMake version 3.20 or higher
- C++ compiler with C++17 support
- Git (for version control)

### Platform-Specific Requirements

#### macOS
- Xcode Command Line Tools
- Homebrew (for package management)

#### Linux
- GCC or Clang compiler
- Build essentials (build-essential package)
- pkg-config utility

#### Windows
- Visual Studio Community 2022 (or later)
- vcpkg package manager
- MinGW (alternative to Visual Studio)

---

## Installation Steps

### Step 1: Install CMake

#### macOS
```bash
brew install cmake
```

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install cmake
```

#### Linux (Fedora)
```bash
sudo dnf install cmake
```

#### Windows
Download from: https://cmake.org/download/

---

### Step 2: Install C++ Compiler

#### macOS
```bash
xcode-select --install
```

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get install build-essential
```

#### Linux (Fedora)
```bash
sudo dnf groupinstall "Development Tools"
```

#### Windows
Download Visual Studio Community: https://visualstudio.microsoft.com/
During installation, select "Desktop development with C++"

---

### Step 3: Install raylib

#### macOS
```bash
brew install raylib
```

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get install libraylib-dev pkg-config
```

#### Linux (Fedora)
```bash
sudo dnf install raylib-devel
```

#### Linux (Arch)
```bash
sudo pacman -S raylib
```

#### Windows (vcpkg method)
```cmd
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

REM Install raylib (64-bit)
.\vcpkg install raylib:x64-windows

REM Optional: Integrate with Visual Studio
.\vcpkg integrate install
```

#### Verify raylib Installation

macOS:
```bash
brew list raylib
```

Linux:
```bash
pkg-config --modversion raylib
```

Windows (vcpkg):
```cmd
.\vcpkg list | findstr raylib
```

---

## Building the Project

### Method 1: Using Helper Scripts (Recommended)

#### macOS and Linux
```bash
cd /path/to/tugas-besar-1-d3r
chmod +x build.sh
./build.sh run
```

#### Windows
```cmd
cd C:\path\to\tugas-besar-1-d3r
build.bat run
```

### Method 2: Manual CMake Build

#### macOS and Linux
```bash
cd /path/to/tugas-besar-1-d3r
mkdir -p build
cd build
cmake ..
cmake --build . --config Release
cd ..
./bin/MonopolyGame
```

#### Windows (Visual Studio)
```cmd
cd C:\path\to\tugas-besar-1-d3r
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
cd ..
.\bin\MonopolyGame.exe
```

#### Windows (MinGW)
```cmd
cd C:\path\to\tugas-besar-1-d3r
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
cd ..
.\bin\MonopolyGame.exe
```

---

## Available Build Commands

### Using build.sh (macOS/Linux)

```bash
./build.sh build       # Build the project only
./build.sh run         # Build and run the project
./build.sh clean       # Remove build files
./build.sh rebuild     # Clean and rebuild
./build.sh debug       # Build in debug mode
./build.sh release     # Build in release mode (optimized)
./build.sh help        # Display help message
```

### Using build.bat (Windows)

```cmd
build.bat build       REM Build the project only
build.bat run         REM Build and run the project
build.bat clean       REM Remove build files
build.bat rebuild     REM Clean and rebuild
build.bat debug       REM Build in debug mode
build.bat release     REM Build in release mode (optimized)
build.bat help        REM Display help message
```

---

## Troubleshooting

### Error: "raylib not found"

1. Verify raylib is installed using the appropriate verification command for your platform
2. Clear CMake cache and reconfigure:
   ```bash
   rm -rf build
   mkdir build && cd build
   cmake ..
   ```
3. If still not found, specify the path manually:
   ```bash
   cmake .. -DCMAKE_PREFIX_PATH=/path/to/raylib
   ```

### Error: "raylib.h not found"

1. Verify pkg-config can find raylib:
   ```bash
   pkg-config --cflags raylib
   ```
2. On Linux, ensure pkg-config is installed:
   ```bash
   sudo apt-get install pkg-config
   ```
3. Check that the header file exists:
   ```bash
   find /usr -name "raylib.h" 2>/dev/null
   ```

### Error: "C++ compiler not found"

1. Verify compiler is installed:
   - macOS: Run `xcode-select --install`
   - Linux: Install build-essential or development tools
   - Windows: Install Visual Studio Community

2. Check compiler version:
   ```bash
   g++ --version
   clang++ --version
   cl.exe /?
   ```

### Error: "CMake not found"

Install CMake using the appropriate method for your platform (see Installation Steps above).

### Linker errors on Linux

Install X11 development libraries:
```bash
sudo apt-get install libx11-dev
sudo dnf install libX11-devel
sudo pacman -S libx11
```

---

## Build Configurations

### Debug Build
For development with debugging symbols:
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

### Release Build
For optimized production build:
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

---

## Project Structure

```
tugas-besar-1-d3r/
├── include/
│   ├── core/        (Game logic headers)
│   ├── gui/         (Graphics headers)
│   ├── models/      (Data model headers)
│   ├── utils/       (Utility headers)
│   └── views/       (View headers)
├── src/
│   ├── core/        (Game logic implementation)
│   ├── gui/         (Graphics implementation)
│   ├── models/      (Data model implementation)
│   ├── utils/       (Utilities implementation)
│   ├── views/       (Views implementation)
│   └── main.cpp     (Entry point)
├── config/          (Configuration files)
├── data/            (Game data)
├── bin/             (Compiled executable)
├── build/           (Build artifacts)
├── CMakeLists.txt   (CMake configuration)
├── build.sh         (Build helper for macOS/Linux)
├── build.bat        (Build helper for Windows)
└── README.md        (Project overview)
```

---

## Platform Support Matrix

| Platform | Compiler | Status | Notes |
|----------|----------|--------|-------|
| macOS 10.14+ | Clang | Supported | Tested and working |
| Ubuntu 20.04+ | GCC/Clang | Supported | Tested and working |
| Fedora 35+ | GCC | Supported | Partial testing |
| Windows 10/11 | MSVC | Supported | Tested with VS2022 |
| Arch Linux | GCC | Supported | Partial testing |

---

## Additional Resources

- CMake Documentation: https://cmake.org/cmake/help/latest/
- raylib Documentation: https://www.raylib.com/
- raylib GitHub: https://github.com/raysan5/raylib
- vcpkg GitHub: https://github.com/Microsoft/vcpkg

---

## Support and Issues

For build-related issues:
1. Review the Troubleshooting section above
2. Check CMake diagnostic output during configuration
3. Verify all prerequisites are installed correctly
4. Clear build cache and reconfigure if problems persist

---

Last Updated: April 22, 2026

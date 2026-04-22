#!/bin/bash
# 
# Cross-platform CMake helper script for Monopoly Game
# Supports: macOS, Linux, Windows (MinGW/Git Bash)
#
# Usage: ./build.sh [command]
# Commands: build, run, clean, rebuild, debug, release
#

set -e

PROJECT_NAME="MonopolyGame"
BUILD_DIR="build"
BIN_DIR="bin"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Detect OS
if [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macOS"
    EXECUTABLE_EXT=""
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="Linux"
    EXECUTABLE_EXT=""
elif [[ "$OSTYPE" == "msys" || "$OSTYPE" == "mingw" ]]; then
    OS="Windows"
    EXECUTABLE_EXT=".exe"
else
    OS="Unknown"
    EXECUTABLE_EXT=""
fi

# Print welcome message
print_header() {
    echo -e "${BLUE}╔════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║     Monopoly Game - CMake Builder      ║${NC}"
    echo -e "${BLUE}║ Platform: $OS$(printf '%*s' $((25 - ${#OS})) '')║${NC}"
    echo -e "${BLUE}╚════════════════════════════════════════╝${NC}"
}

# Print success message
print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

# Print error message
print_error() {
    echo -e "${RED}✗ $1${NC}"
}

# Print info message
print_info() {
    echo -e "${YELLOW}ℹ $1${NC}"
}

# Check if raylib is installed
check_raylib() {
    if [[ "$OS" == "Linux" ]]; then
        if ! pkg-config --exists raylib 2>/dev/null; then
            print_error "raylib not found! Install with: sudo apt-get install libraylib-dev"
            return 1
        fi
    elif [[ "$OS" == "macOS" ]]; then
        if ! brew list raylib &>/dev/null 2>&1; then
            print_error "raylib not found! Install with: brew install raylib"
            return 1
        fi
    fi
    return 0
}

# Build the project
build() {
    print_header
    
    print_info "Checking raylib installation..."
    if ! check_raylib; then
        exit 1
    fi
    print_success "raylib found"
    
    print_info "Creating build directory..."
    mkdir -p "$BUILD_DIR"
    
    print_info "Configuring CMake..."
    cd "$BUILD_DIR"
    cmake .. -DCMAKE_BUILD_TYPE=Release
    cd ..
    
    print_info "Building project..."
    cmake --build "$BUILD_DIR" --config Release
    
    if [ -f "$BIN_DIR/${PROJECT_NAME}${EXECUTABLE_EXT}" ]; then
        print_success "Build successful!"
        echo -e "${GREEN}Executable: ${BIN_DIR}/${PROJECT_NAME}${EXECUTABLE_EXT}${NC}"
    else
        print_error "Build failed!"
        exit 1
    fi
}

# Run the game
run() {
    if [ ! -f "$BIN_DIR/${PROJECT_NAME}${EXECUTABLE_EXT}" ]; then
        print_error "Executable not found! Run './build.sh build' first"
        exit 1
    fi
    
    print_info "Running $PROJECT_NAME..."
    if [[ "$OS" == "Windows" ]]; then
        "$BIN_DIR/${PROJECT_NAME}${EXECUTABLE_EXT}"
    else
        "$BIN_DIR/${PROJECT_NAME}${EXECUTABLE_EXT}"
    fi
}

# Clean build files
clean() {
    print_info "Cleaning build files..."
    rm -rf "$BUILD_DIR"
    echo -e "${GREEN}Cleaned $BUILD_DIR${NC}"
}

# Rebuild (clean + build)
rebuild() {
    clean
    build
}

# Debug build
debug() {
    print_header
    
    print_info "Creating debug build..."
    mkdir -p "$BUILD_DIR"
    
    cd "$BUILD_DIR"
    cmake .. -DCMAKE_BUILD_TYPE=Debug
    cd ..
    
    cmake --build "$BUILD_DIR" --config Debug
    print_success "Debug build complete"
}

# Release build
release() {
    build
}

# Show help
show_help() {
    cat << EOF
${BLUE}CMake Build Helper for Monopoly Game${NC}

${YELLOW}Usage:${NC}
    ./build.sh [command]

${YELLOW}Commands:${NC}
    build       - Build the project (default)
    run         - Run the executable
    clean       - Remove build files
    rebuild     - Clean and rebuild
    debug       - Build in debug mode
    release     - Build in release mode (optimized)
    help        - Show this help message

${YELLOW}Examples:${NC}
    ./build.sh              # Build project
    ./build.sh run          # Build and run
    ./build.sh rebuild      # Clean build
    ./build.sh debug        # Debug build

${YELLOW}Platform:${NC} $OS
${YELLOW}Compiler:${NC} $(c++ --version | head -n 1)

EOF
}

# Main script
main() {
    case "${1:-build}" in
        build)
            build
            ;;
        run)
            build
            run
            ;;
        clean)
            clean
            ;;
        rebuild)
            rebuild
            ;;
        debug)
            debug
            ;;
        release)
            release
            ;;
        help|--help|-h)
            show_help
            ;;
        *)
            print_error "Unknown command: $1"
            show_help
            exit 1
            ;;
    esac
}

main "$@"

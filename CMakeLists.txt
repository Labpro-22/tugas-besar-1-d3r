cmake_minimum_required(VERSION 3.20)
project(MonopolyGame)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform-specific compiler flags
if(MSVC)
    # Visual Studio compiler (Windows)
    add_compile_options(/W4)
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Od")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2")
else()
    # GCC/Clang (macOS, Linux)
    add_compile_options(-Wall -Wextra -Wpedantic)
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Set directories
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(BIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# Create output directories
file(MAKE_DIRECTORY ${BIN_DIR})
file(MAKE_DIRECTORY ${BUILD_DIR})

# Include directories
include_directories(${INCLUDE_DIR})

# RAYLIB CONFIGURATION - Cross-platform Support
# ==========================================================

# First try to find raylib using standard CMake
find_package(raylib 4.0 QUIET)

# If not found, try pkg-config
if(NOT raylib_FOUND)
    find_package(PkgConfig QUIET)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(raylib raylib)
    endif()
endif()

# If still not found, provide helpful error message with OS-specific instructions
if(NOT raylib_FOUND)
    message(FATAL_ERROR 
        "\n=============== RAYLIB NOT FOUND ===============\n"
        "raylib library is not installed on your system.\n"
        "Please install it using the appropriate command for your OS:\n\n"
        "Windows (vcpkg):\n"
        "  vcpkg install raylib:x64-windows\n"
        "  (or x86-windows for 32-bit)\n\n"
        "macOS (Homebrew):\n"
        "  brew install raylib\n\n"
        "Linux (Ubuntu/Debian):\n"
        "  sudo apt-get install libraylib-dev\n"
        "  sudo apt-get install pkg-config\n\n"
        "Linux (Fedora/RHEL):\n"
        "  sudo dnf install raylib-devel\n\n"
        "Linux (Arch):\n"
        "  sudo pacman -S raylib\n\n"
        "Build from Source (All platforms):\n"
        "  https://github.com/raysan5/raylib/wiki\n"
        "  /CMake-Compilation-on-Linux\n\n"
        "After installing, reconfigure CMake:\n"
        "  rm -rf build && mkdir build && cd build && cmake ..\n"
        "================================================\n"
    )
endif()

# Ensure raylib include directories are set
if(raylib_INCLUDE_DIRS)
    message(STATUS "raylib include directories found: ${raylib_INCLUDE_DIRS}")
else()
    message(STATUS "raylib_INCLUDE_DIRS is empty, attempting to use default paths")
    set(raylib_INCLUDE_DIRS "/usr/include" "/usr/local/include" "/opt/homebrew/include")
endif()

# Add include directories globally
include_directories(${INCLUDE_DIR})
include_directories(${raylib_INCLUDE_DIRS})

# SOURCE FILES & EXECUTABLE
# ==========================================================

# Recursive find all .cpp files
file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Set executable output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
)

# Add include directories to target (modern CMake approach)
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${INCLUDE_DIR}
    ${raylib_INCLUDE_DIRS}
)

# LINK LIBRARIES - Platform Specific
# ==========================================================

# Link raylib
target_link_libraries(${PROJECT_NAME} ${raylib_LIBRARIES})

# macOS specific frameworks
if(APPLE)
    target_link_libraries(${PROJECT_NAME} 
        "-framework Cocoa"
        "-framework OpenGL"
        "-framework IOKit"
        "-framework CoreAudio"
        "-framework CoreVideo"
    )
    message(STATUS "macOS frameworks linked")
endif()

# Linux specific libraries
if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} 
        m
        pthread
        dl
        X11
    )
    message(STATUS "Linux libraries linked")
endif()

# Windows specific libraries
if(WIN32)
    target_link_libraries(${PROJECT_NAME} winmm)
    message(STATUS "Windows libraries linked")
endif()

# CUSTOM BUILD TARGETS
# ==========================================================

# Add custom target for running the game
if(WIN32)
    add_custom_target(run
        COMMAND ${BIN_DIR}/${PROJECT_NAME}.exe
        DEPENDS ${PROJECT_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running ${PROJECT_NAME}..."
    )
else()
    add_custom_target(run
        COMMAND ${BIN_DIR}/${PROJECT_NAME}
        DEPENDS ${PROJECT_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running ${PROJECT_NAME}..."
    )
endif()

# BUILD INFORMATION
# ==========================================================

# Detect platform
if(WIN32)
    set(PLATFORM_NAME "Windows")
elseif(APPLE)
    set(PLATFORM_NAME "macOS")
elseif(UNIX)
    set(PLATFORM_NAME "Linux")
else()
    set(PLATFORM_NAME "Unknown")
endif()

# Print build information
message(STATUS "")
message(STATUS "========== BUILD CONFIGURATION SUMMARY ==========")
message(STATUS "Platform:          ${PLATFORM_NAME}")
message(STATUS "Build Type:        ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard:      ${CMAKE_CXX_STANDARD}")
message(STATUS "C++ Compiler:      ${CMAKE_CXX_COMPILER}")
message(STATUS "")
message(STATUS "Source Directory:  ${SRC_DIR}")
message(STATUS "Include Directory: ${INCLUDE_DIR}")
message(STATUS "Binary Directory:  ${BIN_DIR}")
message(STATUS "")
if(raylib_VERSION)
    message(STATUS "raylib Version:    ${raylib_VERSION}")
else()
    message(STATUS "raylib Version:    Found")
endif()
message(STATUS "raylib Libraries:  ${raylib_LIBRARIES}")
message(STATUS "raylib Includes:   ${raylib_INCLUDE_DIRS}")
message(STATUS "==================================================")
message(STATUS "")

# Verify raylib.h can be found
find_file(RAYLIB_H_FOUND raylib.h PATHS ${raylib_INCLUDE_DIRS} NO_DEFAULT_PATH)
if(RAYLIB_H_FOUND)
    message(STATUS "[OK] raylib.h found at: ${RAYLIB_H_FOUND}")
else()
    message(WARNING "[WARNING] raylib.h NOT found! Checking common locations...")
    # Check common locations
    if(EXISTS "/usr/include/raylib.h")
        message(WARNING "  Found at: /usr/include/raylib.h")
    endif()
    if(EXISTS "/usr/local/include/raylib.h")
        message(WARNING "  Found at: /usr/local/include/raylib.h")
    endif()
    if(EXISTS "/opt/homebrew/include/raylib.h")
        message(WARNING "  Found at: /opt/homebrew/include/raylib.h")
    endif()
    if(APPLE AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/raylib")
        message(WARNING "  Found at: ${CMAKE_CURRENT_SOURCE_DIR}/raylib")
    endif()
endif()

message(STATUS "")
message(STATUS "Build configuration complete.")
message(STATUS "To build and run the project, use:")
message(STATUS "  cmake --build . --target run")
message(STATUS "")


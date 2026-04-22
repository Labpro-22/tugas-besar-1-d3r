@echo off
REM Windows CMake helper script for Monopoly Game
REM Supports: Windows 10/11 with Visual Studio or MinGW
REM
REM Usage: build.bat [command]
REM Commands: build, run, clean, rebuild, debug, release
REM

setlocal enabledelayedexpansion

set PROJECT_NAME=MonopolyGame
set BUILD_DIR=build
set BIN_DIR=bin
set EXECUTABLE=%BIN_DIR%\%PROJECT_NAME%.exe

:parse_args
if "%1"=="" goto build_project
if /i "%1"=="build" goto build_project
if /i "%1"=="run" goto run_project
if /i "%1"=="clean" goto clean_project
if /i "%1"=="rebuild" goto rebuild_project
if /i "%1"=="debug" goto debug_project
if /i "%1"=="release" goto release_project
if /i "%1"=="help" goto show_help
goto unknown_command

:header
cls
echo.
echo ==================================================
echo.    Monopoly Game - CMake Builder (Windows)
echo.
echo ==================================================
echo.
goto :eof

:build_project
call :header
echo [INFO] Building project...
echo.

if not exist "%BUILD_DIR%" (
    echo [INFO] Creating build directory...
    mkdir "%BUILD_DIR%"
)

cd "%BUILD_DIR%"
echo [INFO] Configuring CMake...
cmake .. -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Release
if !errorlevel! neq 0 (
    echo [ERROR] CMake configuration failed!
    cd ..
    goto :eof
)

echo [INFO] Building...
cmake --build . --config Release
if !errorlevel! neq 0 (
    echo [ERROR] Build failed!
    cd ..
    goto :eof
)

cd ..

if exist "%EXECUTABLE%" (
    echo.
    echo [SUCCESS] Build complete!
    echo Executable: %EXECUTABLE%
    echo.
) else (
    echo [ERROR] Executable not found!
    goto :eof
)
goto :eof

:run_project
if not exist "%EXECUTABLE%" (
    echo [ERROR] Executable not found! Run 'build.bat build' first
    goto :eof
)
echo [INFO] Running %PROJECT_NAME%...
echo.
"%EXECUTABLE%"
goto :eof

:clean_project
echo [INFO] Cleaning build files...
if exist "%BUILD_DIR%" (
    rmdir /s /q "%BUILD_DIR%"
    echo [SUCCESS] Cleaned %BUILD_DIR%
) else (
    echo [INFO] Nothing to clean
)
goto :eof

:rebuild_project
call :clean_project
call :build_project
goto :eof

:debug_project
call :header
echo [INFO] Building in debug mode...
echo.

if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

cd "%BUILD_DIR%"
cmake .. -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Debug
cmake --build . --config Debug
cd ..

echo [SUCCESS] Debug build complete!
goto :eof

:release_project
call :build_project
goto :eof

:show_help
echo.
echo CMake Build Helper for Monopoly Game (Windows)
echo.
echo Usage: build.bat [command]
echo.
echo Commands:
echo   build       - Build the project (default)
echo   run         - Build and run the executable
echo   clean       - Remove build files
echo   rebuild     - Clean and rebuild
echo   debug       - Build in debug mode
echo   release     - Build in release mode (optimized)
echo   help        - Show this help message
echo.
echo Examples:
echo   build.bat              # Build project
echo   build.bat run          # Build and run
echo   build.bat rebuild      # Clean build
echo   build.bat debug        # Debug build
echo.
goto :eof

:unknown_command
echo [ERROR] Unknown command: %1
call :show_help
goto :eof

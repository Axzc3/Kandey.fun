@echo off
echo ========================================
echo  Kandey.fun CS2 ESP Builder
echo ========================================
echo.

REM Try to find MSBuild in common locations
set MSBUILD_PATH=""

if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" (
    set MSBUILD_PATH="C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
)

if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe" (
    set MSBUILD_PATH="C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe"
)

if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe" (
    set MSBUILD_PATH="C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe"
)

if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe" (
    set MSBUILD_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe"
)

if %MSBUILD_PATH%=="" (
    echo [ERROR] MSBuild not found!
    echo Please install Visual Studio 2022 or 2019 with C++ Desktop Development workload
    echo Download from: https://visualstudio.microsoft.com/downloads/
    pause
    exit /b 1
)

echo [INFO] Found MSBuild at: %MSBUILD_PATH%
echo.

REM Create build directory if it doesn't exist
if not exist "build" mkdir build

echo [INFO] Building KandeyESP.dll (Release x64)...
echo.

%MSBUILD_PATH% KandeyESP.vcxproj /p:Configuration=Release /p:Platform=x64 /m

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo [SUCCESS] Build completed!
    echo ========================================
    echo.
    echo Output: build\KandeyESP.dll
    echo.
    echo Next steps:
    echo 1. Use a DLL injector to inject build\KandeyESP.dll into cs2.exe
    echo 2. Press INSERT key in-game to toggle menu
    echo 3. Enable ESP and configure settings
    echo.
) else (
    echo.
    echo ========================================
    echo [ERROR] Build failed!
    echo ========================================
    echo.
    echo Common issues:
    echo - Missing ImGui files in main/imgui/imgui-master/
    echo - Missing Windows SDK
    echo - Check BUILD_INSTRUCTIONS.md for details
    echo.
)

pause

@echo off
echo ====================================
echo Kandey.fun - Hot Rebuild Script
echo ====================================
echo.

REM Set the MSBuild path
set MSBUILD="C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\MSBuild.exe"

REM Change output directory temporarily to avoid locked file
echo [1/3] Building to temporary output...
%MSBUILD% KandeyESP.vcxproj /p:Configuration=Release /p:Platform=x64 /p:TargetName=KandeyESP_new /m

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo BUILD FAILED!
    pause
    exit /b 1
)

echo.
echo [2/3] Build successful!
echo.

REM Check if original DLL is locked
echo [3/3] Checking if original DLL is in use...
if exist "build\KandeyESP.dll" (
    del "build\KandeyESP.dll" 2>nul
    if exist "build\KandeyESP.dll" (
        echo WARNING: Original DLL is locked. New DLL saved as KandeyESP_new.dll
        echo Unload the old DLL from CS2 and manually rename KandeyESP_new.dll to KandeyESP.dll
    ) else (
        echo Original DLL deleted, renaming new DLL...
        ren "build\KandeyESP_new.dll" "KandeyESP.dll"
    )
) else (
    echo No existing DLL found, renaming new DLL...
    ren "build\KandeyESP_new.dll" "KandeyESP.dll"
)

echo.
echo ====================================
echo Done! DLL is at: build\KandeyESP.dll
echo ====================================
echo.
pause

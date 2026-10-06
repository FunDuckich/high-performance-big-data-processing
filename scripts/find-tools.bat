@echo off
set "LAB_VS_ROOT="
set "LAB_CMAKE="
set "LAB_VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%LAB_VSWHERE%" (
    echo Visual Studio Build Tools 2022 with C++ tools is required to build.
    exit /b 1
)
pushd "%ProgramFiles(x86)%\Microsoft Visual Studio\Installer"
if errorlevel 1 exit /b 1
for /f "delims=" %%I in ('vswhere.exe -latest -version "[17.0,18.0)" -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath') do set "LAB_VS_ROOT=%%I"
popd
if not defined LAB_VS_ROOT (
    echo MSVC C++ tools for Visual Studio 2022 were not found.
    exit /b 1
)
set "LAB_CMAKE=%LAB_VS_ROOT%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
if not exist "%LAB_CMAKE%" (
    set "LAB_CMAKE="
    for %%I in (cmake.exe) do set "LAB_CMAKE=%%~$PATH:I"
)
if not defined LAB_CMAKE (
    echo CMake was not found. Install the C++ CMake tools component or add CMake to PATH.
    exit /b 1
)
exit /b 0

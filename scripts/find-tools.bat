@echo off
set "LAB_VS_ROOT="
set "LAB_MSBUILD="
set "LAB_VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%LAB_VSWHERE%" (
    echo Не найдены средства Visual Studio 2017 или новее.
    echo Для сборки установите набор разработки классических приложений на C++ и Windows SDK.
    echo Готовые exe можно запускать без установки компилятора.
    exit /b 1
)
pushd "%ProgramFiles(x86)%\Microsoft Visual Studio\Installer"
if errorlevel 1 exit /b 1
for /f "delims=" %%I in ('vswhere.exe -latest -version "[15.0,)" -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 Microsoft.Component.MSBuild -property installationPath') do set "LAB_VS_ROOT=%%I"
popd
if not defined LAB_VS_ROOT (
    echo Не найден набор инструментов C++ для Visual Studio 2017 или новее.
    echo Добавьте MSVC x64/x86 и Windows SDK через Visual Studio Installer.
    exit /b 1
)
set "LAB_MSBUILD=%LAB_VS_ROOT%\MSBuild\Current\Bin\MSBuild.exe"
if not exist "%LAB_MSBUILD%" set "LAB_MSBUILD=%LAB_VS_ROOT%\MSBuild\15.0\Bin\MSBuild.exe"
if not exist "%LAB_MSBUILD%" (
    echo Не найден MSBuild. Восстановите инструменты сборки через Visual Studio Installer.
    exit /b 1
)
exit /b 0

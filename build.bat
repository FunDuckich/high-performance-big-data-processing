@echo off
setlocal
chcp 65001 >nul
set "LAB_TARGET=Build"
if not "%~2"=="" goto usage
if "%~1"=="" goto build
for %%L in (lab01 lab02 lab03 lab04 lab05 lab06 lab07) do if /i "%~1"=="%%L" set "LAB_TARGET=%%L"
if "%LAB_TARGET%"=="Build" goto usage
:build
call "%~dp0scripts\find-tools.bat"
if errorlevel 1 exit /b 1
echo Сборка Release x64. Инструменты: %LAB_VS_ROOT%
"%LAB_MSBUILD%" "%~dp0Labs.sln" /nologo /m:1 /nodeReuse:false /t:%LAB_TARGET% /p:Configuration=Release /p:Platform=x64 /verbosity:minimal
if errorlevel 1 (
    echo Сборка не выполнена. Проверьте сообщение выше.
    echo В Visual Studio Installer нужны MSVC x64/x86 и Windows SDK из набора разработки на C++.
    exit /b 1
)
echo Готово. Программы находятся в папках labXX\bin\Release.
exit /b 0
:usage
echo Использование: build.bat [lab01^|lab02^|lab03^|lab04^|lab05^|lab06^|lab07]
exit /b 2

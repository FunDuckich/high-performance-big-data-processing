@echo off
setlocal
set "LAB_TARGET=ALL_BUILD"
if not "%~2"=="" goto usage
if "%~1"=="" goto build
if /i "%~1"=="lab01" set "LAB_TARGET=lab01"
if /i "%~1"=="lab02" set "LAB_TARGET=lab02"
if /i "%~1"=="lab03" set "LAB_TARGET=lab03"
if "%LAB_TARGET%"=="ALL_BUILD" goto usage
:build
call "%~dp0scripts\find-tools.bat"
if errorlevel 1 exit /b 1
call "%LAB_VS_ROOT%\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul
if errorlevel 1 exit /b 1
"%LAB_CMAKE%" -S "%~dp0." -B "%~dp0build" -G "Visual Studio 17 2022" -A x64 -DCMAKE_GENERATOR_INSTANCE="%LAB_VS_ROOT%"
if errorlevel 1 exit /b 1
"%LAB_CMAKE%" --build "%~dp0build" --config Release --target %LAB_TARGET% --parallel 1 -- /nodeReuse:false
if errorlevel 1 exit /b 1
if not "%LAB_TARGET%"=="ALL_BUILD" exit /b 0
for %%I in ("%LAB_CMAKE%") do set "LAB_CTEST=%%~dpIctest.exe"
"%LAB_CTEST%" --test-dir "%~dp0build" -C Release --output-on-failure
exit /b %errorlevel%
:usage
echo Usage: build.bat [lab01^|lab02^|lab03]
exit /b 2

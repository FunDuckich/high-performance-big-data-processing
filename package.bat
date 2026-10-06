@echo off
setlocal
call "%~dp0build.bat"
if errorlevel 1 exit /b 1
call "%~dp0scripts\find-tools.bat"
if errorlevel 1 exit /b 1
"%LAB_CMAKE%" --build "%~dp0build" --config Release --target package --parallel 1 -- /nodeReuse:false
if errorlevel 1 exit /b 1
echo Archive: "%~dp0dist\labs01-03-windows-x64.zip"
exit /b 0

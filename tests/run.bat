@echo off
setlocal
chcp 65001 >nul
call "%~dp0..\scripts\find-tools.bat"
if errorlevel 1 exit /b 1
call "%LAB_VS_ROOT%\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul
if errorlevel 1 exit /b 1
if not exist "%~dp0..\build\tests" mkdir "%~dp0..\build\tests"
pushd "%~dp0..\build\tests"
if errorlevel 1 exit /b 1
for %%L in (lab01 lab02 lab03 lab04 lab05 lab06 lab07 table) do (
    cl /nologo /std:c++14 /EHsc /MT /O2 /utf-8 /W4 "%~dp0%%L.cpp" /Fe:%%L.exe
    if errorlevel 1 goto failed
    %%L.exe
    if errorlevel 1 goto failed
)
popd
echo Все проверки пройдены.
exit /b 0
:failed
popd
exit /b 1

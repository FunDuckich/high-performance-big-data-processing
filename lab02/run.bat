@echo off
setlocal
chcp 65001 >nul
if not exist "%~dp0bin\Release\lab02.exe" (
    echo Программа не найдена. Запустите build.bat lab02 из корня проекта.
    pause
    exit /b 1
)
"%~dp0bin\Release\lab02.exe"
set "LAB_RESULT=%errorlevel%"
echo.
if not "%LAB_RESULT%"=="0" echo Программа завершилась с ошибкой. Код: %LAB_RESULT%
pause
exit /b %LAB_RESULT%

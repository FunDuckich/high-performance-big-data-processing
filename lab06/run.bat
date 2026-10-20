@echo off
setlocal
chcp 65001 >nul
if not exist "%~dp0bin\Release\lab06.exe" (
    echo Программа не найдена. Запустите build.bat lab06 из корня проекта.
    pause
    exit /b 1
)
"%~dp0bin\Release\lab06.exe"
set "LAB_RESULT=%errorlevel%"
echo.
if not "%LAB_RESULT%"=="0" echo Программа завершилась с ошибкой. Код: %LAB_RESULT%
pause
exit /b %LAB_RESULT%

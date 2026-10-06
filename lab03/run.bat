@echo off
setlocal
chcp 65001 >nul
if not exist "%~dp0bin\Release\lab03.exe" (
    echo Программа не собрана. Запустите build.bat lab03 из корня исходного проекта.
    echo Для запуска без компилятора распакуйте готовый переносимый ZIP целиком.
    pause
    exit /b 1
)
"%~dp0bin\Release\lab03.exe" %*
set "LAB_RESULT=%errorlevel%"
echo.
if not "%LAB_RESULT%"=="0" echo Программа завершилась с ошибкой. Код: %LAB_RESULT%
pause
exit /b %LAB_RESULT%

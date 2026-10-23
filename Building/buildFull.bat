@echo off
cd /d "%~dp0"

powershell -NoProfile -ExecutionPolicy Bypass -File ".\1_buildEngine.ps1" %*
if errorlevel 1 goto fail
timeout /t 3 /nobreak >nul

powershell -NoProfile -ExecutionPolicy Bypass -File ".\2_copyToLimeVS.ps1"
if errorlevel 1 goto fail
timeout /t 3 /nobreak >nul

powershell -NoProfile -ExecutionPolicy Bypass -File ".\3_buildExtension.ps1"
if errorlevel 1 goto fail

echo.
echo Done.
pause
exit /b 0

:fail
echo.
echo Failed.
pause
exit /b 1

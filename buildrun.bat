@echo off
cd /d "%~dp0"
call build.bat || exit /b
call run.bat || exit /b
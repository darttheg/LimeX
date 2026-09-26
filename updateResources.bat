@echo off
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0updateResources.ps1" %*
exit /b %ERRORLEVEL%

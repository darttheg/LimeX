@echo off
call "%~dp0updateResources.bat"
cd /d "%~dp0LimeVS" && vsce package --allow-missing-repository
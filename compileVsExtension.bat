@echo off
call "%~dp0updateResources.bat" || exit /b 1
where vsce >nul 2>nul || (echo vsce not found, install it with: npm install -g @vscode/vsce& exit /b 1)
cd /d "%~dp0LimeVS"
if not exist node_modules (call npm install || exit /b 1)
vsce package --allow-missing-repository --allow-star-activation

@echo off
setlocal

set "ROOT=%~dp0"
set "SRC=%ROOT%resources"
set "ZIP=%ROOT%resources.zip"

if exist "%ZIP%" del /f /q "%ZIP%"

powershell -NoProfile -ExecutionPolicy Bypass -Command ^
	"Compress-Archive -Path '%SRC%\*' -DestinationPath '%ZIP%' -Force"

echo Compiled resources to %ZIP%
endlocal
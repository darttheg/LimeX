@echo off
xcopy /y ".\LimeEngine\api\Enums.lua" ".\LimeVS\api\"
xcopy /y ".\LimeEngine\api\Lime.lua" ".\LimeVS\api\"
xcopy /y ".\LimeEngine\bin\x64\Release\LimeEngine.dll" ".\LimeVS\template\"
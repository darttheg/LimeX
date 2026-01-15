@echo off
pushd "%~dp0LimeBuilder\x64\Release" || exit \b 1
LimeBuilder.exe "%~dp0LimeEngine/x64/Release"
popd
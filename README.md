# LimeX
LimeX is a 3D game framework powered by Lua and Ogre3D.

## Requirements
- CMake 3.x
- MSVC/Clang/GCC

## Building
* cmake -B build -G "Visual Studio 17 2022"
* cmake --build build --config Release

## Dependencies
* [Lua 5.4](https://www.lua.org/versions.html#5.4)
* [sol2](https://github.com/ThePhD/sol2/releases/tag/v3.3.0)
* [Irrlicht (Modified for Lime)](https://github.com/darttheg/IrrLime)
* [GLFW 3.4](https://www.glfw.org/download.html)

## Running
1. Create a LimeX project given the empty project in Releases.
2. Ensure your entry point, **main.lua** is located within the directory.
3. Run **LimeBuilder** with the directory path as an argument. This immediate directory should contain the .dlls required to run LimeX. Example usage: LimeBuilder ./Projects/MyLimeXProject
4. LimeBuilder will generate an executable titled 'app.exe' within the project directory. Run it to play your application.

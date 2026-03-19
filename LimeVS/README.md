## Lime Engine for Visual Studio Code
A powerful Visual Studio Code extension for Lime projects that cleanly embeds autocompletion and build/run/package commands. Stress free creation!

## Dependencies
It is recommended to use **EmmyLua** (`tangzx.emmylua`), but **lua-language-server** (`sumneko.lua`) is also supported in patching Lime completions. These extensions can be found on the Visual Studio Code extensions marketplace.

## Getting Started
(To be added)

## Running Your Project
To run your Lime application, it must be successfully built. Every Lime project requires:
- A main.lua entry point
- Required .dlls (*LimeEngine.dll...*)

Running **Lime: Build** will build your project, with output detailing the build process. 

## Shipping
(To be added)

## Help

| Command | Action |
| ----- | ----- |
| **Lime: Build** | Builds your project. |
| **Lime: Build and Run** | Builds and runs your project. |
| **Lime: Package Project** | Packages your built application. See `Shipping`. |
| **Lime: Create New Project** | Creates a new project. |
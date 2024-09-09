# SGXPos
Simple GTK3 X11 Positioning Library

This library aims to provide a simple way to position GTK3 windows in an X11 environment.

## Usage
See the <a href="https://github.com/ItzSelenux/SGXPos/wiki">wiki</a> for detailed information.

## Why?
This project is mainly focused on providing a similar implementation to GTK Layer Shell, but only with a focus on positioning windows.

I wanted to achieve Wayland compatibility in a project, but I didn't want to heavily extend the codebase. Therefore, having a library that mimics another library makes the codebase more maintainable.

## GTK Layer Shell Compatibility
This project includes some definitions to call functions similar to those in <a href="https://github.com/wmww/gtk-layer-shell">gtk-layer-shell</a>, but not every function is available. This compatibility is nearly incomplete, and I will only focus on what I need.

Therefore, GTK4 support is also not available, as I don't use that version of GTK.

If you are interested in contributing by adding functions to this library, feel free to open a pull request.
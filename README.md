# Pipboy OS implementation on pure C and SDL

## Features
2025/02/08	Animation of boot screen worked (build with cmd+shift+B)
		in linux run build.sh

i'am don't use any Makefile in this, it' a draft for future project

for dev i'am use
* vscode
* A boilerplate for C language development with GCC + VSCode in macOS.
* IntelliSense enabled
* Debugger with stdin support in the integrated terminal

Push [F5] and your source file currently opened in the editor will be compiled into `${fileBasenameNoExtension}.out` in the same directory.

## Requirements
* XCode
Standard libraries must be installed in `/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include`

* C/C++ (https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
* CodeLLDB (https://marketplace.visualstudio.com/items?itemName=vadimcn.vscode-lldb)

Recommended to use CodeLLDB because Microsoft's C/C++ extension does not support stdin for the integrated terminal.


# C++ Algo Study Monorepo

A lightweight, flexible CMake-based monorepo for algorithmic and study tasks. Designed to support modern C++ standards (C++23/C++26) and be fully compatible with MSVC, GCC, and Clang.

## How It Works

The structure dynamically detects new tasks without requiring you to manually update `CMakeLists.txt` every time:

1. **Simple Tasks**: Any `.cpp` file created directly inside the `tasks/` directory automatically becomes its own executable target.
   - *Example*: Creating `tasks/my_algo.cpp` generates an executable target named `my_algo`.
2. **Complex Tasks**: Any subdirectory within `tasks/` that contains its own `CMakeLists.txt` is automatically included as a subproject.
   - *Example*: Creating `tasks/complex_project/CMakeLists.txt` includes that folder automatically.

## Build and Debug Workflows

### 1. Visual Studio Code
The best experience in VSCode is using the **CMake Tools** extension. 
- Once installed, it will automatically detect the `CMakeLists.txt`.
- Select your active target from the status bar at the bottom.
- Click **Build**, **Run**, or **Debug** directly from the status bar.
- If you aren't using CMake Tools, the provided `.vscode/tasks.json` and `.vscode/launch.json` provide basic fallback support for building all tasks and launching the debugger.

### 2. Zed
The repository includes a `.zed/tasks.json` file.
- Open the command palette (`cmd-shift-p`) and run `task: spawn`.
- You will see tasks for **CMake: Build All** and **CMake: Configure**.

### 3. Xcode
You can easily generate a native Xcode project to leverage its advanced debugging tools:
```bash
cmake -G Xcode -B build-xcode
```
Then, open the generated `build-xcode/InterviewCpp.xcodeproj` in Xcode.

### 4. Command Line
You can configure and build everything standardly via the terminal:
```bash
# 1. Configure the project
cmake -S . -B build

# 2. Build a specific target (or leave out --target to build all)
cmake --build build --target hello

# 3. Run the executable
./build/tasks/hello
```

## C++ Standards & Compilers

- The project is configured to request **C++26** by default, with a fallback down to C++20/23 depending on your compiler's capabilities.
- Standard high-warning flags are enforced (`-Wall -Wextra -Wpedantic` for GCC/Clang, `/W4 /permissive-` for MSVC).
- The `Threads` library is globally linked to simplify concurrency tasks.

Project Build Instructions

Quick start

- Recommended to run in x64 Developer Command Prompt (Visual Studio) or PowerShell.
- The build script defaults to Qt installation path D:\Qt\6.8.3\msvc2022_64 and can be overridden with parameters.

Using build.ps1 (recommended)

Example: default Release build

```powershell
powershell -ExecutionPolicy Bypass -File build.ps1
```

Specify Debug:

```powershell
powershell -ExecutionPolicy Bypass -File build.ps1 -Config Debug
```

Specify Qt path and output directory:

```powershell
powershell -ExecutionPolicy Bypass -File build.ps1 -QtPath "E:\Qt\6.6.2\msvc2019_64" -BuildDir out
```

Enable vcpkg (script will clone and bootstrap vcpkg under the repo):

```powershell
powershell -ExecutionPolicy Bypass -File build.ps1 -UseVcpkg -VcpkgRoot "thirdparty\vcpkg" -VcpkgTriplet x64-windows
```

Parameters

- QtPath: Default Qt root directory (contains lib/cmake/Qt6).
- BuildDir: CMake output directory, default is build.
- Config: Debug or Release, default is Release.
- Arch: Target architecture, default is x64.
- UseVcpkg: Automatically install and use vcpkg.
- VcpkgRoot: vcpkg clone/install directory (relative or absolute).
- VcpkgTriplet: vcpkg triplet, e.g. x64-windows.
- ExportCmakePrefixPath: Export CMAKE_PREFIX_PATH into the current process environment.

Direct CMake usage

If you prefer to call cmake directly, specify CMAKE_PREFIX_PATH (pointing to Qt root):

```powershell
cmake -S . -B build -A x64 -DCMAKE_PREFIX_PATH="D:\Qt\6.8.3\msvc2022_64"
cmake --build build --config Release
```

Or specify Qt6_DIR:

```powershell
cmake -S . -B build -DQt6_DIR="D:\Qt\6.8.3\msvc2022_64\lib\cmake\Qt6"
```

Notes

- Prefer passing the Qt path via command line or environment variable to keep CMakeLists.txt portable.
- Ensure the selected CMake generator matches your Qt installation (for example Visual Studio 2022 x64 should use -A x64).
- If CMake reports it cannot find Qt6Config.cmake, verify Qt development components are installed and point CMAKE_PREFIX_PATH to the correct directory.

If you want a CI example (GitHub Actions) or translation tweaks, tell me which platform.

# DesktopAgent

项目构建说明

快速开始

- 推荐在 x64 开发者命令提示符（Visual Studio）或 PowerShell 中运行。
- 默认脚本使用 Qt 安装路径 D:\Qt\6.8.3\msvc2022_64，可通过参数覆盖。

使用 build.ps1（推荐）

示例：默认 Release 构建

```powershell
powershell -ExecutionPolicy Bypass -File build.ps1
```

指定 Debug：

```powershell
powershell -ExecutionPolicy Bypass -File build.ps1 -Config Debug
```

指定 Qt 路径与输出目录：

```powershell
powershell -ExecutionPolicy Bypass -File build.ps1 -QtPath "E:\Qt\6.6.2\msvc2019_64" -BuildDir out
```

启用 vcpkg（脚本会在仓库下克隆并引导安装 vcpkg）：

```powershell
powershell -ExecutionPolicy Bypass -File build.ps1 -UseVcpkg -VcpkgRoot "thirdparty\vcpkg" -VcpkgTriplet x64-windows
```

参数说明

- QtPath：默认 Qt 根目录（含 lib/cmake/Qt6）。
- BuildDir：CMake 输出目录，默认为 build。
- Config：Debug 或 Release，默认为 Release。
- Arch：体系结构，默认为 x64。
- UseVcpkg：自动安装并使用 vcpkg。
- VcpkgRoot：vcpkg 克隆/安装目录（相对或绝对）。
- VcpkgTriplet：vcpkg triplet，例如 x64-windows。
- ExportCmakePrefixPath：将 CMAKE_PREFIX_PATH 导出到当前进程环境。

直接使用 CMake

如需直接调用 cmake，可指定 CMAKE_PREFIX_PATH（指向 Qt 根目录）：

```powershell
cmake -S . -B build -A x64 -DCMAKE_PREFIX_PATH="D:\Qt\6.8.3\msvc2022_64"
cmake --build build --config Release
```

或指定 Qt6_DIR：

```powershell
cmake -S . -B build -DQt6_DIR="D:\Qt\6.8.3\msvc2022_64\lib\cmake\Qt6"
```

注意事项

- 优先通过命令行参数或环境变量传入 Qt 路径，避免在 CMakeLists.txt 中硬编码路径。
- 确保使用与 Qt 安装匹配的生成器（例如 Visual Studio 2022 x64 对应 -A x64）。
- 如果遇到找不到 Qt6Config.cmake，确认 Qt 的开发组件已安装并使用上述 -DCMAKE_PREFIX_PATH 指向正确目录。

如需把 README 翻译为英文或添加 CI 构建示例，请告知。

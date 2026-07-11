cmake_minimum_required(VERSION 3.22)
project(DesktopAgent
    VERSION 0.1.0
    DESCRIPTION "Desktop AI Assistant powered by Ollama"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 如果未通过命令行提供 Qt 路径，提供本地开发机器上的默认值（可覆盖）
if(NOT DEFINED CMAKE_PREFIX_PATH AND NOT DEFINED Qt6_DIR)
  # 指向本地 Qt 安装目录，可被 -DCMAKE_PREFIX_PATH 覆盖
  set(CMAKE_PREFIX_PATH "D:/Qt/6.8.3/msvc2022_64" CACHE PATH "Qt installation prefix (added by patch)")
endif()

# 尝试从环境或默认位置确定 Qt6_DIR（应指向包含 Qt6Config.cmake 的目录）
if(NOT DEFINED Qt6_DIR)
  if(DEFINED ENV{QT_DIR})
    set(Qt6_DIR "$ENV{QT_DIR}/lib/cmake/Qt6" CACHE PATH "Qt6 cmake dir")
  elseif(DEFINED ENV{Qt6_DIR})
    set(Qt6_DIR "$ENV{Qt6_DIR}" CACHE PATH "Qt6 cmake dir")
  else()
    # 本地默认路径（可通过 -DQt6_DIR 覆盖）
    set(Qt6_DIR "D:/Qt/6.8.3/msvc2022_64/lib/cmake/Qt6" CACHE PATH "Qt6 cmake dir")
  endif()
endif()

# 将 Qt 安装根目录加入 CMAKE_PREFIX_PATH，便于 find_package 查找
get_filename_component(Qt6_ROOT "${Qt6_DIR}" DIRECTORY) # Qt6_DIR -> .../lib/cmake/Qt6 的上级
list(APPEND CMAKE_PREFIX_PATH "${Qt6_ROOT}")
message(STATUS "Using Qt6_DIR=${Qt6_DIR}; CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}")

find_package(Qt6 6.2 REQUIRED COMPONENTS Core Gui Widgets Network)

find_package(Qt6 QUIET COMPONENTS Test)
set(HAS_QT_TEST ${Qt6Test_FOUND})

add_compile_definitions(
    QT_DISABLE_DEPRECATED_BEFORE=0x060200
    $<$<CONFIG:Debug>:DESKTOP_AGENT_DEBUG>
)

if(MSVC)
    add_compile_options(/W4 /utf-8 /Zc:__cplusplus)
endif()

add_subdirectory(src)

if(HAS_QT_TEST)
    enable_testing()
    add_subdirectory(tests)
endif()

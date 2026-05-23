cmake_minimum_required(VERSION 3.20)

project(CatBombGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(raylib CONFIG QUIET)
if (raylib_FOUND)
    # Use CMake package (vcpkg or system CMake config) when available
    set(RAYLIB_INCLUDE_DIRS "${raylib_INCLUDE_DIRS}")
    set(RAYLIB_LIBRARY_DIRS "${raylib_LIBRARY_DIRS}")
    set(RAYLIB_LIBRARIES raylib::raylib)
    set(RAYLIB_CFLAGS_OTHER "")
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(RAYLIB REQUIRED raylib)
endif()

add_executable(catbomb
    src/AssetManager.cpp
    src/GameUi.cpp
    src/main.cpp
)

# Native launcher executable (small wrapper to run the built game exe on Windows)
add_executable(launcher
        src/launcher.cpp
)

if (WIN32)
    target_compile_definitions(launcher PRIVATE _WIN32_WINNT=0x0601)
    target_link_libraries(launcher PRIVATE)
endif()

target_include_directories(catbomb PRIVATE src ${RAYLIB_INCLUDE_DIRS})
target_link_directories(catbomb PRIVATE ${RAYLIB_LIBRARY_DIRS})
target_link_libraries(catbomb PRIVATE ${RAYLIB_LIBRARIES})
target_compile_options(catbomb PRIVATE -Wall -Wextra -Wpedantic ${RAYLIB_CFLAGS_OTHER})

if(APPLE)
    target_link_libraries(catbomb PRIVATE
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework OpenGL"
    )
endif()

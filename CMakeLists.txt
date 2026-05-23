cmake_minimum_required(VERSION 3.20)

project(CatBombGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(RAYLIB REQUIRED raylib)

add_executable(catbomb
    src/AssetManager.cpp
    src/GameUi.cpp
    src/main.cpp
)

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

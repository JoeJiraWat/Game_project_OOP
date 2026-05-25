cmake_minimum_required(VERSION 3.20)

project(CatBombGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(RAYLIB REQUIRED raylib)

if(APPLE)
    add_executable(catbomb MACOSX_BUNDLE
        src/AssetManager.cpp
        src/GameUi.cpp
        src/main.cpp
    )
    # ตั้งค่าข้อมูลของ App Bundle (.app)
    set_target_properties(catbomb PROPERTIES
        MACOSX_BUNDLE_BUNDLE_NAME "Cat Bomb Game"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.yourname.catbomb"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0.0"
    )
else()
    add_executable(catbomb
        src/AssetManager.cpp
        src/GameUi.cpp
        src/main.cpp
    )
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

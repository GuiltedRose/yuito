cmake_minimum_required(VERSION 3.16)
project(Yuito VERSION 0.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt6 modules
find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGL OpenGLWidgets)

# Include paths
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/living-legacy
    ${CMAKE_SOURCE_DIR}/include/living-legacy/faction
    ${CMAKE_SOURCE_DIR}/include/living-legacy/legacy
    ${CMAKE_SOURCE_DIR}/include/living-legacy/reputation
    ${CMAKE_SOURCE_DIR}/include/living-legacy/rumor
    ${CMAKE_SOURCE_DIR}/include/living-legacy/world
    ${CMAKE_SOURCE_DIR}/include/worldgen
    ${CMAKE_SOURCE_DIR}/include/player
    ${CMAKE_SOURCE_DIR}/include/itemdb
    ${CMAKE_SOURCE_DIR}/libs     # Include path for lz4.h
)

# Add LZ4 static library
add_library(lz4 STATIC
    ${CMAKE_SOURCE_DIR}/libs/lz4.c
)

target_include_directories(lz4 PUBLIC
    ${CMAKE_SOURCE_DIR}/libs
)

# Collect all source files for the main game
file(GLOB_RECURSE SRC_FILES
    src/**/*.cpp
    include/**/*.h
)

# Main game executable
add_executable(Yuito ${SRC_FILES})

# Link libraries
target_link_libraries(Yuito
    PRIVATE Qt6::Widgets Qt6::OpenGL Qt6::OpenGLWidgets
            lz4
)

# -----------------------------
# Developer Tools (optional)
# -----------------------------
option(BUILD_DEV_TOOLS "Enable developer tools build" ON)

if (BUILD_DEV_TOOLS)
    add_executable(index_items src/dev-tools/index_items.cpp)
    target_link_libraries(index_items Qt6::Core lz4)
    target_include_directories(index_items PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/libs
    )
endif()

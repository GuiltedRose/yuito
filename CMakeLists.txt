cmake_minimum_required(VERSION 3.16)
project(Yuito VERSION 0.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt6 modules
find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGL OpenGLWidgets Core)

# Include paths
include_directories(${CMAKE_SOURCE_DIR}/include)

# Collect all source files
file(GLOB_RECURSE SRC_FILES
    src/**/*.cpp
    include/**/*.h
)

# Core static library to share between main executable and tools
add_library(yuito_core STATIC ${SRC_FILES})

target_link_libraries(yuito_core
    PUBLIC Qt6::Widgets Qt6::OpenGL Qt6::OpenGLWidgets Qt6::Core
)

target_include_directories(yuito_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# Main game executable
add_executable(Yuito)
target_link_libraries(Yuito PRIVATE yuito_core)

# Developer Tools (optional)
option(BUILD_DEV_TOOLS "Enable developer tools build" ON)

if (BUILD_DEV_TOOLS)
    add_executable(index_items src/dev-tools/index_items.cpp)

    target_link_libraries(index_items
        PRIVATE Qt6::Core yuito_core
    )

    target_include_directories(index_items PRIVATE
        ${CMAKE_SOURCE_DIR}/include
    )
endif()

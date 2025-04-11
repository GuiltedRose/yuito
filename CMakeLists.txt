cmake_minimum_required(VERSION 3.16)
project(Yuito VERSION 0.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt6 modules
find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGL OpenGLWidgets Core)

# Include paths
include_directories(${CMAKE_SOURCE_DIR}/include)

# === Core Source Files ===
file(GLOB_RECURSE CORE_SRC_FILES
    src/*.cpp
    include/**/*.h
)

# Exclude dev-tools sources unless BUILD_DEV_TOOLS is ON
option(BUILD_DEV_TOOLS "Enable developer tools build" OFF)
if(NOT BUILD_DEV_TOOLS)
    list(FILTER CORE_SRC_FILES EXCLUDE REGEX ".*/dev-tools/.*\\.cpp$")
    list(FILTER CORE_SRC_FILES EXCLUDE REGEX ".*/dev-tools/.*\\.h$")
endif()

# === Core Static Library ===
add_library(yuito_core STATIC ${CORE_SRC_FILES})

target_link_libraries(yuito_core
    PUBLIC Qt6::Widgets Qt6::OpenGL Qt6::OpenGLWidgets Qt6::Core
)

target_include_directories(yuito_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# Define ENABLE_DEVTOOLS for conditional compilation
if (BUILD_DEV_TOOLS)
    target_compile_definitions(yuito_core PUBLIC ENABLE_DEVTOOLS)
endif()

# === Main Game Executable ===
add_executable(Yuito)
target_link_libraries(Yuito PRIVATE yuito_core)

# === Developer Tools (Optional) ===
if (BUILD_DEV_TOOLS)
    file(GLOB_RECURSE DEV_TOOL_SOURCES src/dev-tools/*.cpp)

    add_executable(index_items ${DEV_TOOL_SOURCES})
    target_link_libraries(index_items PRIVATE Qt6::Core yuito_core)
    target_include_directories(index_items PRIVATE ${CMAKE_SOURCE_DIR}/include)
endif()

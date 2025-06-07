cmake_minimum_required(VERSION 3.16)

project(ObscuraPasswordVault VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Qt6 automatically handles high DPI scaling
qt6_standard_project_setup()

# Set source files
set(SOURCES
    main.cpp
    mainwindow.cpp
)

set(HEADERS
    mainwindow.h
)

# Create executable using qt6_add_executable
qt6_add_executable(ObscuraPasswordVault)

# Add sources to the target
target_sources(ObscuraPasswordVault PRIVATE ${SOURCES} ${HEADERS})

# Link Qt6 libraries using modern CMake syntax
target_link_libraries(ObscuraPasswordVault PRIVATE
    Qt6::Core
    Qt6::Widgets
)



# Set application properties
set_target_properties(ObscuraPasswordVault PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(ObscuraPasswordVault PRIVATE -Wall -Wextra)
endif()

if(MSVC)
    target_compile_options(ObscuraPasswordVault PRIVATE /W4)
endif()

cmake_minimum_required(VERSION 3.30)
set(CMAKE_CXX_STANDARD 23)
project(WinKey)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")

# Define the executable with source files
add_executable(WinKey
    src/main.cpp
    app.rc
)

# Include directories for the target
target_include_directories(WinKey PRIVATE include)

# Preprocessor definitions
add_definitions(-DUNICODE -D_UNICODE)

# Link against required Windows libraries
target_link_libraries(WinKey
    user32
    shell32
)

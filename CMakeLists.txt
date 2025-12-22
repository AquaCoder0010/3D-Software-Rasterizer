cmake_minimum_required(VERSION 3.20)
project(output LANGUAGES CXX)

# Use C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

list(FILTER SOURCE_FILES EXCLUDE REGEX ".*/CMakeFiles/.*")
list(FILTER SOURCE_FILES EXCLUDE REGEX ".*\\.#.*")

# Add an executable target named 'MyProject' using the found source files
add_executable(output ${SOURCE_FILES})

# Include directories
target_include_directories(output PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Link SFML (explicit libraries, matching your command)
target_link_directories(output PRIVATE
    /usr/lib/x86_64-linux-gnu
)

target_link_libraries(output PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
)

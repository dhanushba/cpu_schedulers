# 1. Project Setup
cmake_minimum_required(VERSION 3.16)
project(CPUScheduler VERSION 1.0 LANGUAGES CXX)

# Require C++17 (Mandatory for Qt6)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. Qt6 Configuration
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

# 3. Gather Source Files
# This automatically finds all .cpp files in your subdirectories
file(GLOB_RECURSE CORE_SOURCES "src/core/*.cpp")
file(GLOB_RECURSE GUI_SOURCES "src/gui/*.cpp")
set(MAIN_SOURCE "src/main.cpp")

# 4. Create the Executable
add_executable(${PROJECT_NAME} 
    ${MAIN_SOURCE}
    ${CORE_SOURCES}
    ${GUI_SOURCES}
)

# 5. Link Header Folders and Libraries
target_include_directories(${PROJECT_NAME} PRIVATE
    "include/core"
    "include/gui"
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    Qt6::Widgets
)
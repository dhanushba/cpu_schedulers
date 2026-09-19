# 1. Project Setup
cmake_minimum_required(VERSION 3.16)
project(CPUScheduler VERSION 1.0 LANGUAGES CXX)
include(GNUInstallDirs)
include(CTest)

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
file(GLOB_RECURSE GUI_HEADERS "include/gui/*.h")
set(MAIN_SOURCE "src/main.cpp")

# 4. Create the Core Library and Executable
add_library(SchedulerCore STATIC ${CORE_SOURCES})
target_include_directories(SchedulerCore PUBLIC "include/core")

add_executable(${PROJECT_NAME} 
    ${MAIN_SOURCE}
    ${GUI_SOURCES}
    ${GUI_HEADERS}
)

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE ON)
endif()

# 5. Link Header Folders and Libraries
target_include_directories(${PROJECT_NAME} PRIVATE
    "include/core"
    "include/gui"
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    SchedulerCore
    Qt6::Widgets
)

if(BUILD_TESTING)
    add_executable(SchedulerTests tests/SchedulerTests.cpp)
    target_link_libraries(SchedulerTests PRIVATE SchedulerCore)
    add_test(NAME scheduler-regression-suite COMMAND SchedulerTests)
endif()

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

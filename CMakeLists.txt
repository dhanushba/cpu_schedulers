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

    add_executable(SimulationControllerTests
        tests/SimulationControllerTests.cpp
        src/gui/SimulationController.cpp
        include/gui/SimulationController.h
    )
    target_include_directories(SimulationControllerTests PRIVATE "include/gui")
    target_link_libraries(SimulationControllerTests PRIVATE SchedulerCore Qt6::Core)
    add_test(NAME simulation-controller-regression-suite COMMAND SimulationControllerTests)

    add_executable(ComparisonRunnerTests tests/ComparisonRunnerTests.cpp)
    target_link_libraries(ComparisonRunnerTests PRIVATE SchedulerCore)
    add_test(NAME comparison-runner-regression-suite COMMAND ComparisonRunnerTests)

    add_executable(ComparisonWidgetTests
        tests/ComparisonWidgetTests.cpp
        src/gui/ComparisonWidget.cpp
        include/gui/ComparisonWidget.h
    )
    target_include_directories(ComparisonWidgetTests PRIVATE "include/gui")
    target_link_libraries(ComparisonWidgetTests PRIVATE SchedulerCore Qt6::Widgets)
    add_test(NAME comparison-widget-regression-suite COMMAND ComparisonWidgetTests)
    set_tests_properties(comparison-widget-regression-suite PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
    )

    add_executable(DecisionTraceWidgetTests
        tests/DecisionTraceWidgetTests.cpp
        src/gui/DecisionTraceWidget.cpp
        include/gui/DecisionTraceWidget.h
    )
    target_include_directories(DecisionTraceWidgetTests PRIVATE "include/gui")
    target_link_libraries(DecisionTraceWidgetTests PRIVATE SchedulerCore Qt6::Widgets)
    add_test(NAME decision-trace-widget-regression-suite COMMAND DecisionTraceWidgetTests)
    set_tests_properties(decision-trace-widget-regression-suite PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
    )

    add_executable(UsabilityWidgetTests
        tests/UsabilityWidgetTests.cpp
        src/gui/ControlPanelWidget.cpp
        src/gui/ProcessTableWidget.cpp
        include/gui/ControlPanelWidget.h
        include/gui/ProcessTableWidget.h
    )
    target_include_directories(UsabilityWidgetTests PRIVATE "include/gui")
    target_link_libraries(UsabilityWidgetTests PRIVATE SchedulerCore Qt6::Widgets)
    add_test(NAME usability-widget-regression-suite COMMAND UsabilityWidgetTests)
    set_tests_properties(usability-widget-regression-suite PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
    )
endif()

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

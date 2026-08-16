cmake_minimum_required(VERSION 3.21)

project(Skyward VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(SKYWARD_BUILD_BENCH "Build the collision benchmarks in bench/" OFF)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick LinguistTools)

qt_standard_project_setup(REQUIRES 6.5)

# game_core   simulation: game loop, physics, entities. No UI.
# game_ui     the "Skyward" QML module, every .qml file lives here
# appSkyward  thin executable that wires the two together
add_subdirectory(src/core)
add_subdirectory(src/ui)
add_subdirectory(src/app)

# bench       collision broad-phase benchmarks, off unless asked for
if(SKYWARD_BUILD_BENCH)
    enable_testing() #must be the top level, or ctest finds nothing in bench/
    add_subdirectory(bench)
endif()

include(GNUInstallDirs)
install(TARGETS appSkyward
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

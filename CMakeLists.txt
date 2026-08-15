cmake_minimum_required(VERSION 3.21)

project(Skyward VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick LinguistTools Test)

qt_standard_project_setup(REQUIRES 6.5)

# game_core   simulation: game loop, physics, entities. No UI.
# game_ui     the "Skyward" QML module, every .qml file lives here
# appSkyward  thin executable that wires the two together
add_subdirectory(src/core)
add_subdirectory(src/ui)
add_subdirectory(src/app)

# headless tests over game_core, run with ctest
enable_testing()
add_subdirectory(tests)

include(GNUInstallDirs)
install(TARGETS appSkyward
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

cmake_minimum_required(VERSION 3.16)

project(Game01 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.2 COMPONENTS Quick REQUIRED)

qt_add_executable(appGame01
    main.cpp
)

qt_add_qml_module(appGame01
    URI Game01
    VERSION 1.0
    QML_FILES main.qml
    SOURCES Controller.h Controller.cpp
    SOURCES Class2.h Class2.cpp
    SOURCES Class1.h Class1.cpp
    QML_FILES bullet.qml
    SOURCES Bullet.h Bullet.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appGame01 PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appGame01
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appGame01
    PRIVATE Qt6::Quick)

include(GNUInstallDirs)
install(TARGETS appGame01
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

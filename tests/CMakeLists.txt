# Headless tests for the simulation layer. game_core links only Qt6::Qml, so
# nothing here needs a window or a running scene graph.
qt_add_executable(tst_controller tst_controller.cpp)

target_link_libraries(tst_controller PRIVATE game_core Qt6::Test)

add_test(NAME tst_controller COMMAND tst_controller)

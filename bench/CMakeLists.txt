# Benchmarks and correctness checks for the collision broad phase.
# Opt in with -DSKYWARD_BUILD_BENCH=ON, they are not part of a normal app build.
#
# Qt6::Core only: these exercise the algorithm, not the QML side, so there is
# no reason to pull in Qml or Quick. They read entity sizes and the cell size
# out of core/GameConfig.h so retuning the game retunes the benchmark with it.

add_executable(collision_bench CollisionBench.cpp)
target_include_directories(collision_bench PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(collision_bench PRIVATE Qt6::Core)

add_executable(collision_verify CollisionVerify.cpp)
target_include_directories(collision_verify PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(collision_verify PRIVATE Qt6::Core)

# collision_verify exits non-zero on a mismatch, so ctest can run it as a test.
# enable_testing() lives in the top-level CMakeLists, a subdirectory call would
# not register anything with the root test file.
add_test(NAME collision_broadphase_matches_sweep COMMAND collision_verify)

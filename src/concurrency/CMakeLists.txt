add_library(concurrency
    SimulationThread.cpp
    SnapshotStore.cpp
)

target_include_directories(concurrency PUBLIC
    ${PROJECT_SOURCE_DIR}/src
)

target_compile_features(concurrency PUBLIC cxx_std_17)
target_link_libraries(concurrency PUBLIC Threads::Threads)
target_compile_options(concurrency PRIVATE
    -Wall -Wextra -Wpedantic
)

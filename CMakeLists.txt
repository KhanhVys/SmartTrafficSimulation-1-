cmake_minimum_required(VERSION 3.15.2)
project(SmartTrafficSimulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_CONCURRENCY_PROTOTYPE
       "Build the modular concurrency prototype"
       ON)
option(BUILD_STANDALONE_SINGLE_FILE
       "Build the one-file standalone prototype"
       OFF)

find_package(Threads REQUIRED)

add_subdirectory(src/concurrency)

if(BUILD_CONCURRENCY_PROTOTYPE)
    add_executable(concurrency_prototype
        examples/ConcurrencyPrototype.cpp
    )
    target_link_libraries(concurrency_prototype PRIVATE concurrency)
    target_compile_options(concurrency_prototype PRIVATE
        -Wall -Wextra -Wpedantic
    )
endif()

# This target is intentionally separate from the modular library.
# It has its own main() and must not be linked into the final application.
if(BUILD_STANDALONE_SINGLE_FILE)
    add_executable(concurrency_single_file
        src/ConcurrencyModule.cpp
    )
    target_link_libraries(concurrency_single_file PRIVATE Threads::Threads)
    target_compile_features(concurrency_single_file PRIVATE cxx_std_17)
    target_compile_options(concurrency_single_file PRIVATE
        -Wall -Wextra -Wpedantic
    )
endif()

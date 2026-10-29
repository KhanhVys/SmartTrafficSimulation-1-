cmake_minimum_required(VERSION 3.16)
project(SmartTrafficSimulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_GUI "Build ban co giao dien SFML 3" ON)

find_package(Threads REQUIRED)

add_library(common INTERFACE)
target_include_directories(common INTERFACE ${PROJECT_SOURCE_DIR}/src)

if(EXISTS ${PROJECT_SOURCE_DIR}/src/concurrency/CMakeLists.txt)
    add_subdirectory(src/concurrency)
endif()

if(EXISTS ${PROJECT_SOURCE_DIR}/src/integration/IntegrationCoordinator.cpp)
    add_library(integration STATIC src/integration/IntegrationCoordinator.cpp)
    target_link_libraries(integration PUBLIC common)
endif()

enable_testing()

# Logic library
add_library(logic STATIC
    src/Logic/TrafficRule.cpp
)
target_link_libraries(logic PUBLIC common)

# TrafficRule test
add_executable(TrafficRuleTest
    tests/TrafficRuleTest.cpp
)
target_link_libraries(TrafficRuleTest PRIVATE logic)
add_test(NAME TrafficRuleTest COMMAND TrafficRuleTest)

# ==========================================
# PHẦN BỔ SUNG: Core library (Lấy từ main)
# ==========================================
file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/core/*.cpp)
if(CORE_SOURCES)
    add_library(core STATIC ${CORE_SOURCES})
    target_link_libraries(core PUBLIC common logic)
endif()

# Các bài test cho Integration và Core
if(EXISTS ${PROJECT_SOURCE_DIR}/tests/IntegrationCoordinator.cpp)
    add_executable(IntegrationCoordinatorTest tests/IntegrationCoordinator.cpp)
    target_link_libraries(IntegrationCoordinatorTest PRIVATE integration)
    add_test(NAME IntegrationCoordinatorTest COMMAND IntegrationCoordinatorTest)
endif()

if(TARGET core AND EXISTS ${PROJECT_SOURCE_DIR}/tests/CoreSimulationTest.cpp)
    add_executable(CoreSimulationTest tests/CoreSimulationTest.cpp)
    target_link_libraries(CoreSimulationTest PRIVATE core)
    add_test(NAME CoreSimulationTest COMMAND CoreSimulationTest)
endif()

# ==========================================
# SFML 3 FetchContent (Giữ từ feature-gui)
# ==========================================
if(BUILD_GUI)
    include(FetchContent)
    set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.0
        GIT_SHALLOW ON
        SYSTEM)
    FetchContent_MakeAvailable(SFML)
endif()

# Main Application Executable
if(EXISTS ${PROJECT_SOURCE_DIR}/src/main.cpp)
    file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*.cpp)
    
    # Loại trừ các thư mục đã build thành thư viện riêng để tránh lỗi trùng lặp định nghĩa
    list(FILTER APP_SOURCES EXCLUDE REGEX ".*/src/(concurrency|integration|core|Logic)/.*")
    list(FILTER APP_SOURCES EXCLUDE REGEX ".*/ConcurrencyModule\\.cpp$")

    add_executable(SmartTrafficSimulation ${APP_SOURCES})
    target_link_libraries(SmartTrafficSimulation PRIVATE common Threads::Threads logic)
    
    if(TARGET concurrency)
        target_link_libraries(SmartTrafficSimulation PRIVATE concurrency)
    endif()
    if(TARGET integration)
        target_link_libraries(SmartTrafficSimulation PRIVATE integration)
    endif()
    if(TARGET core)
        target_link_libraries(SmartTrafficSimulation PRIVATE core)
    endif()
    if(BUILD_GUI)
        target_link_libraries(SmartTrafficSimulation PRIVATE SFML::Graphics)
    endif()
    if(NOT MSVC)
        target_compile_options(SmartTrafficSimulation PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()

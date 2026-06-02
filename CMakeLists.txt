cmake_minimum_required(VERSION 3.21)
project(hpc_disease_simulation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)
include_directories(simulation)

add_library(disease_core
        simulation/simulation.cpp
        simulation/Person.cpp
        simulation/Disease.cpp
)

add_executable(hpc_disease_simulation
        simulation/main.cpp
)

target_link_libraries(hpc_disease_simulation
        disease_core
)

enable_testing()

add_executable(all_tests
        simulation/test.cpp
)

target_link_libraries(all_tests
        disease_core
)

add_test(NAME run_doctest_all COMMAND all_tests)

if(ENABLE_COVERAGE)
  set(CMAKE_CXX_FLAGS "-O0 -coverage")

  find_program(LCOV lcov REQUIRED)
  find_program(GENHTML genhtml REQUIRED)

  add_custom_target(coverage
            COMMAND make test
            COMMAND ${LCOV} --directory . --capture --output-file coverage.info
            COMMAND ${GENHTML} --demangle-cpp -o coverage coverage.info
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()
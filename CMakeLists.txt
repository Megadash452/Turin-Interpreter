cmake_minimum_required(VERSION 3.22)
project(Turing_Interpreter)

set(CMAKE_CXX_STANDARD 14)

if(CMAKE_BUILD_TYPE EQUAL DEBUG)
    add_definitions(_DEBUG)
endif()

include_directories(src/include)
add_executable(Turing_Interpreter src/cpp/main.cpp src/cpp/Console.cpp src/cpp/TuringMachine.cpp)
target_link_libraries(Turing_Interpreter ncurses)
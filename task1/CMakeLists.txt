cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(Mealy2MooreConversation)

add_executable(${CMAKE_PROJECT_NAME} main.cpp)

FILE(GLOB_RECURSE SOURCE "src/*.h" "src/*.cpp")
add_library(utils ${SOURCE})

target_link_libraries(${CMAKE_PROJECT_NAME} utils)
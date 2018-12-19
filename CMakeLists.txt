cmake_minimum_required(VERSION 3.8)
project(vibration_gen)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV)

add_executable(vibration_gen main.cpp)
target_link_libraries( vibration_gen ${OpenCV_LIBS} )

cmake_minimum_required(VERSION 3.3)
project(Capture)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp capture.c capture/streamMJPEG.c)
add_executable(Capture ${SOURCE_FILES})
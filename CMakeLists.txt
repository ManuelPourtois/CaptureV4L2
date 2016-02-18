cmake_minimum_required(VERSION 3.3)
project(Capture)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp capture.c )
set(SOURCE_FILES2 capture/streamMJPEG.c)
add_executable(Capture ${SOURCE_FILES})
add_executable(Capture2 ${SOURCE_FILES2})
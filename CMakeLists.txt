cmake_minimum_required(VERSION 3.24)
project(HuaweiSoftwareCompetition)

set(CMAKE_CXX_STANDARD 17)

add_executable(HuaweiSoftwareCompetition main.cpp Include/test.h Source/test.cpp)
add_executable(robot robot.cpp)
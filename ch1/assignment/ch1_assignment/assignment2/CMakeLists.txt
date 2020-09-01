cmake_minimum_required(VERSION 2.8)

set(CMAKE_BUILD_TYPE Debug)
project(ch1-2)

set(CMAkE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(Eigen3)

INCLUDE_DIRECTORIES(${EIGEN3_INCLUDE_DIR})


add_executable(Ruptate rotationupdate.cpp)

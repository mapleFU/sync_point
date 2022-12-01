cmake_minimum_required(VERSION 3.22)
project(sync_point)

set(CMAKE_CXX_STANDARD 20)

add_library(sync_point sync_point.cpp sync_point_impl.cpp)

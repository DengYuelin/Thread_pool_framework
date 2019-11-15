cmake_minimum_required(VERSION 3.14)
project(multithread)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(multithread main.cpp threadpool.h threadpool_lambda.h)

cmake_minimum_required(VERSION 3.29)
project(Cache)

set(CMAKE_CXX_STANDARD 20)

add_executable(Cache main.cpp
        YCachePolicy.cpp
        YCachePolicy.h
        YLFuCache.cpp
        YLFuCache.h)

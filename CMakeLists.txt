cmake_minimum_required(VERSION 3.13.4)
project(GFTP)

set(CMAKE_CXX_STANDARD 17)

add_executable(GFTP main.cpp Connector.cpp Connector.h)

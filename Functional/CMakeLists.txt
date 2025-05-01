cmake_minimum_required(VERSION 3.31)
project(MPI_SAT_Unified)

set(CMAKE_CXX_STANDARD 20)

add_executable(MPI_SAT_Unified
        restest.cpp
        resolutionsolver.cpp
        satsolver.cpp
        DIMACSParser.cpp

)

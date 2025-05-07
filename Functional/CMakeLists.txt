cmake_minimum_required(VERSION 3.31)
project(MPI_SAT_Unified)

set(CMAKE_CXX_STANDARD 20)

# Add include directory to the include path
include_directories(${CMAKE_SOURCE_DIR}/Functional/include)

add_executable(MPI_SAT_Unified
        Tests/dptest.cpp
        include/dp/dpsolver.cpp
        include/dpll/dpllsolver.cpp
        include/general/satsolver.cpp
        include/parser/DIMACSParser.cpp
        include/res/resolutionsolver.cpp
        include/cdcl/cdclsolver.cpp
)
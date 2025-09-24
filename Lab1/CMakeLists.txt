# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(Lab1 VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


# Attempt to find OpenAL package
#find_package(OpenAL REQUIRED)

# Add the include directories for OpenAL
include_directories(${OPENAL_INCLUDE_DIR})


# Add source files
file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/code/Buzzy_Defender.cpp)

# Add the executable
add_executable(Lab1
    code/Buzzy_Defender.cpp
    code/ECE_Buzzy.cpp
    code/ECE_Buzzy.h
    code/ECE_LaserBlast.cpp
    code/ECE_LaserBlast.h
    code/ECE_Enemy.cpp
    code/ECE_Enemy.h)

include_directories(${PROJECT_SOURCE_DIR}/../SFML/include)

link_directories(${PROJECT_SOURCE_DIR}/../SFML/lib)

# Link the executable to the libraries in the lib directory
target_link_libraries(Lab1 PUBLIC sfml-graphics sfml-system sfml-window)# sfml-audio ${OPENAL_LIBRARY})

file(COPY "${PROJECT_SOURCE_DIR}/graphics" DESTINATION "${COMMON_OUTPUT_DIR}/bin/")
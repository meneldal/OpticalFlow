# CMakeList.txt : CMake project for OpticalFlow, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.9)

find_package(OpenCV REQUIRED)

project (OpticalFlow)
include_directories( ${OpenCV_INCLUDE_DIRS} )

include_directories ("${PROJECT_SOURCE_DIR}/include")
include_directories ("${PROJECT_SOURCE_DIR}/src")
# Add source to this project's executable.


add_subdirectory (src)

# TODO: Add tests and install targets if needed.

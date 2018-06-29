set(SOURCES "GaussianPyramid.cpp" "OpticalFlow.cpp")
add_library (OpticalFlowLib STATIC ${SOURCES})
target_link_libraries(OpticalFlowLib ${OpenCV_LIBS} )



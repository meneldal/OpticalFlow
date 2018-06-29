set(SOURCES "Coarse2FineTwoFrames.cpp")
add_executable (Test ${SOURCES})
target_link_libraries(Test ${OpenCV_LIBS} )



set(SOURCES "Coarse2FineTwoFrames.cpp")
add_executable (Test ${SOURCES})
target_link_libraries(Test ${OpenCV_LIBS} )
target_link_libraries(Test OpticalFlowLib)
add_custom_command(
        TARGET Test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/demo/car1.jpg
                ${CMAKE_CURRENT_BINARY_DIR}/demo/car1.jpg)
add_custom_command(
        TARGET Test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/demo/car2.jpg
                ${CMAKE_CURRENT_BINARY_DIR}/demo/car2.jpg)


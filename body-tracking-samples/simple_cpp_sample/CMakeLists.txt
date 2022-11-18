
add_executable(simple_cpp_sample main.cpp)
add_executable(server server.c)

find_package(OpenCV 4.1.0 REQUIRED)
if( OpenCV_FOUND )
list( APPEND ThirdParty_LIBS ${OpenCV_LIBS} )
    include_directories( ${OpenCV_INCLUDE_DIRS} )
endif( OpenCV_FOUND )

#target_include_directories(simple_cpp_sample PRIVATE ../sample_helper_includes)


# Dependencies of this library
target_link_libraries(simple_cpp_sample PRIVATE 
    k4a
    k4abt
    k4arecord
    #window_controller_3d::window_controller_3d
    #glfw::glfw
    ${OpenCV_LIBS}
    )



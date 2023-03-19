# cmake needs this line
cmake_minimum_required(VERSION 2.8)

# Define project name
project(urglaserrange)

# Find OpenCV, you may need to set OpenCV_DIR variable
# to the absolute path to the directory containing OpenCVConfig.cmake file
# via the command line or GUI
set( OpenCV_DIR "C:/Users/Sonny/Desktop/Workspace/Binaries/opencv_4.7" )
option( OpenCV_STATIC OFF )
find_package(OpenCV REQUIRED)

# URG sensor
set(URG_ROOT                 "C:/Users/Sonny/Desktop/Applications/urg_library-ver.1.2.7/current" )
set(URG_INCLUDE_DIRS         "C:/Users/Sonny/Desktop/Applications/urg_library-ver.1.2.7/current/include/cpp")
set(URG_INCLUDE_SAMPLE_DIRS  "C:/Users/Sonny/Desktop/Applications/urg_library-ver.1.2.7/current/samples/cpp")
set(URG_INCLUDE_DIR_C        "C:/Users/Sonny/Desktop/Applications/urg_library-ver.1.2.7/current/include/c")
set(URG_SRC                  "C:/Users/Sonny/Desktop/Applications/urg_library-ver.1.2.7/current/src")   
set(URG_LIB_RELEASE_DIR      "C:/Users/Sonny/Desktop/Applications/urg_library-ver.1.2.7/current/vs2019/cpp/x64/Release")

set(URG_LIBS_RELEASE  ${URG_LIB_RELEASE_DIR}/urg_cpp.lib)

# OSCPACK
set(OSCPACK_INCLUDE_DIR      "C:/Users/Sonny/Desktop/Applications/oscpack_1_1_0")

# GFLAGS
find_package(gflags REQUIRED)
set(GFLAGS_INCLUDE "C:/Users/Sonny/Desktop/Workspace/Binaries/gflags/include")
set(GFLAGS_LIB "C:/Users/Sonny/Desktop/Workspace/Binaries/gflags/lib/Release/gflags_nothreads_static.lib")

# Set installation folder
set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE )
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build CACHE PATH "Build directory" FORCE)

# Set additional include folders
set (ADDITIONAL_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/src/include")

include_directories(${ADDITIONAL_INCLUDE})
include_directories(${URG_INCLUDE_DIRS} ${URG_INCLUDE_SAMPLE_DIRS} ${OSCPACK_INCLUDE_DIR})
include_directories(${URG_INCLUDE_DIR_C})
include_directories(${GFLAGS_INCLUDE})
link_directories(${URG_LIB_RELEASE_DIR})

# If the package has been found, several variables will
# be set, you can find the full list with descriptions
# in the OpenCVConfig.cmake file.
# Print some message showing some of them
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "URG Sensor libraries : ${URG_INCLUDE_DIRS} ${URG_INCLUDE_SAMPLE_DIRS}")
message(STATUS "OSCPACK libraries : ${OSCPACK_INCLUDE_DIR}")

if(CMAKE_VERSION VERSION_LESS "2.8.11")
  # Add OpenCV headers location to your include paths
  include_directories(${OpenCV_INCLUDE_DIRS})
endif()


# Declare the executable target built from your sources
set(SOURCES
	src/main.cpp
	src/get_data.cpp
	src/classifier.cpp
	src/utilities.cpp
	src/communications.cpp
	${ADDITIONAL_INCLUDE}/utilities.h 
	${ADDITIONAL_INCLUDE}/classifier.h 
	${ADDITIONAL_INCLUDE}/communications.h 
	${ADDITIONAL_INCLUDE}/common.h 
	${ADDITIONAL_INCLUDE}/get_data.h
	${ADDITIONAL_INCLUDE}/flags.hpp
	${OSCPACK_INCLUDE_DIR}/osc/OscTypes.cpp
	${OSCPACK_INCLUDE_DIR}/osc/OscReceivedElements.cpp
	${OSCPACK_INCLUDE_DIR}/osc/OscPrintReceivedElements.cpp
	${OSCPACK_INCLUDE_DIR}/osc/OscOutboundPacketStream.cpp
	${OSCPACK_INCLUDE_DIR}/ip/IpEndpointName.cpp
	${OSCPACK_INCLUDE_DIR}/ip/win32/NetworkingUtils.cpp
	${OSCPACK_INCLUDE_DIR}/ip/win32/UdpSocket.cpp
	${URG_INCLUDE_SAMPLE_DIRS}/Connection_information.cpp	
	${URG_INCLUDE_SAMPLE_DIRS}/Connection_information.h	
	${URG_SRC}/Urg_driver.cpp)
add_executable(urglaserrange ${SOURCES} )

if(WIN32)
  target_link_libraries(urglaserrange wsock32 ws2_32 winmm setupapi)
endif()

# Link your application with OpenCV libraries
target_link_libraries(urglaserrange ${OpenCV_LIBS} ${URG_LIBS_RELEASE} ${GFLAGS_LIB} shlwapi.lib)
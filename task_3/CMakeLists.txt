cmake_minimum_required(VERSION 2.8)
project( DisplayImage )
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( DisplayImage task_3.cpp )
target_link_libraries( DisplayImage ${OpenCV_LIBS} )

find_package(PkgConfig REQUIRED)
pkg_check_modules(Mosquitto REQUIRED IMPORTED_TARGET libmosquitto)

target_link_libraries( DisplayImage PkgConfig::Mosquitto)

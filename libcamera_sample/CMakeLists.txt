cmake_minimum_required(VERSION 3.12)
project(libcamera_sample)

set(CMAKE_CXX_STANDARD 11)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBCAMERA REQUIRED libcamera)

include_directories(${LIBCAMERA_INCLUDE_DIRS})
link_directories(${LIBCAMERA_LIBRARY_DIRS})

add_executable(libcamera_sample main.cpp)
target_link_libraries(libcamera_sample ${LIBCAMERA_LIBRARIES})
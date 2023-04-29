cmake_minimum_required(VERSION 3.10)

project(seer CXX)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(OPENCV_DIR lib/opencv)
set(SOURCE_DIR src)
set(EXECUTABLE seer_executable)
file(GLOB SOURCE_FILES ${SOURCE_DIR}/*)

find_package(OpenCV REQUIRED PATHS ${OPENCV_DIR}/bin)

add_executable(${EXECUTABLE} ${SOURCE_FILES})
target_include_directories(${EXECUTABLE} PRIVATE ${SOURCE_DIR} ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${EXECUTABLE} PRIVATE ${OpenCV_LIBS})
target_compile_features(${EXECUTABLE} PRIVATE cxx_std_17)

file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)

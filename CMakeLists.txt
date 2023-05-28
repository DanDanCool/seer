cmake_minimum_required(VERSION 3.13)

set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreadedDebugDLL)
project(seer CXX)

#set(CMAKE_CXX_FLAGS_DEBUG "/MTd")
set(CMAKE_CXX_FLAGS_DEBUG "/MDd /Z7")
set(CMAKE_CXX_FLAGS_RELEASE "/MD")
set(OPENCV_DIR lib/opencv)
set(SOURCE_DIR src)
set(EXECUTABLE seer_executable)
set(LIBRARY seer_lib)

find_package(OpenCV REQUIRED PATHS ${OPENCV_DIR}/bin)

add_library(${LIBRARY} SHARED ${SOURCE_DIR}/VisionThread.h ${SOURCE_DIR}/Swapchain.h ${SOURCE_DIR}/VisionThread.cpp)
target_include_directories(${LIBRARY} PRIVATE ${SOURCE_DIR} ${OpenCV_INCLUDE_DIRS})

#target_link_directories(${LIBRARY} PRIVATE ${OPENCV_DIR}/bin/lib)
#target_link_libraries(${LIBRARY} PRIVATE opencv)

target_link_libraries(${LIBRARY} PRIVATE ${OpenCV_LIBS})
target_compile_features(${LIBRARY} PRIVATE cxx_std_17)
target_compile_definitions(${LIBRARY} PRIVATE SEER_EXPORT)


add_executable(${EXECUTABLE} ${SOURCE_DIR}/main.cpp)
target_include_directories(${EXECUTABLE} PRIVATE ${SOURCE_DIR} ${OpenCV_INCLUDE_DIRS})

#target_link_directories(${EXECUTABLE} PRIVATE ${OPENCV_DIR}/bin/lib)
#target_link_libraries(${EXECUTABLE} PRIVATE opencv ${LIBRARY})

target_link_libraries(${EXECUTABLE} PRIVATE ${OpenCV_LIBS} ${LIBRARY})
target_compile_features(${EXECUTABLE} PRIVATE cxx_std_17)

file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)

cmake_minimum_required(VERSION 3.13)

set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreadedDebugDLL)
project(seer CXX)

#set(CMAKE_CXX_FLAGS_DEBUG "/MTd")
set(CMAKE_CXX_FLAGS_DEBUG "/MDd /Z7")
set(CMAKE_CXX_FLAGS_RELEASE "/MD")

set(OPENCV_DIR lib/opencv)
set(TORCH_DIR lib/torch/debug)

set(SOURCE_DIR src)
set(EXECUTABLE seer_executable)
set(LIBRARY seer_lib)

find_package(OpenCV REQUIRED PATHS ${OPENCV_DIR}/bin)
find_package(Torch REQUIRED PATHS ${TORCH_DIR}/share/cmake/Torch)

add_library(${LIBRARY} SHARED ${SOURCE_DIR}/VisionThread.h ${SOURCE_DIR}/Swapchain.h ${SOURCE_DIR}/VisionThread.cpp)
target_include_directories(${LIBRARY} PRIVATE ${SOURCE_DIR} ${OpenCV_INCLUDE_DIRS})

target_link_libraries(${LIBRARY} PRIVATE ${OpenCV_LIBS})
target_compile_features(${LIBRARY} PRIVATE cxx_std_17)
target_compile_definitions(${LIBRARY} PRIVATE SEER_EXPORT)

add_executable(${EXECUTABLE} ${SOURCE_DIR}/main.cpp)
target_include_directories(${EXECUTABLE} PRIVATE ${SOURCE_DIR} ${OpenCV_INCLUDE_DIRS} ${TORCH_INCLUDE_DIRS})

target_link_libraries(${EXECUTABLE} PRIVATE ${OpenCV_LIBS} ${TORCH_LIBRARIES} ${LIBRARY})
target_compile_features(${EXECUTABLE} PRIVATE cxx_std_17)

if (MSVC)
  file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
  add_custom_command(TARGET ${EXECUTABLE}
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_if_different
                     ${TORCH_DLLS}
					 $<TARGET_FILE_DIR:${EXECUTABLE}>)
endif (MSVC)

file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)

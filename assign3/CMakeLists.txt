cmake_minimum_required(VERSION 3.0.0)
project(burst_photography VERSION 0.1.0)

include(CTest)
enable_testing()

find_package(PNG REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

include_directories(${eigen3_INCLUDE_DIR} ${PNG_INCLUDE_DIR} ${OpenCV_INCLUDE_DIR})

add_executable(burst_photography main.cpp algo.cpp csv.cpp)
target_link_libraries(burst_photography ${Eigen3_LIBRARIES} ${PNG_LIBRARIES} ${OpenCV_LIBRARIES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

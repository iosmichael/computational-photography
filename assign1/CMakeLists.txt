cmake_minimum_required(VERSION 3.0.0)
project(assign1 VERSION 0.1.0)

find_package(Eigen3 REQUIRED)
find_package(TIFF REQUIRED)
find_package(PNG REQUIRED)
find_package(OpenCV REQUIRED)
find_package(OpenEXR REQUIRED)

set(OpenEXR_INCLUDE_DIR /usr/local/include/OpenEXR)
set(OpenEXR_LIBS /usr/local/lib/libIlmImfUtil.so;
                 /usr/local/lib/libIlmImf.so;
                 /usr/local/lib/libIlmThread.so;
                 /usr/local/lib/libImath.so;
                 /usr/local/lib/libHalf-2_4.so;
                 /usr/local/lib/libHalf.so;
                 /usr/local/lib/libIexMath.so;
                 /usr/local/lib/libIex.so)

include(CTest)

# include_directories(${OpenEXR_INCLUDE_DIR})
include_directories(${OpenEXR_INCLUDE_DIR} ${OPENCV_INCLUDE_DIR} ${PNG_INCLUDE_DIR} ${EIGEN3_INCLUDE_DIR} ${TIFF_INCLUDE_DIR})


add_test("test1" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}")

enable_testing()

add_executable("${PROJECT_NAME}" main.cpp utils.cpp openexr_utils.cpp utils.h openexr_utils.h)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

file(COPY macbeth.png DESTINATION "${CMAKE_BINARY_DIR}")

target_link_libraries(${PROJECT_NAME} ${PNG_LIBRARIES} ${OpenCV_LIBS} ${TIFF_LIBRARIES} ${EIGEN3_LIBRARIES} ${OpenEXR_LIBS})

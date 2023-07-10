cmake_minimum_required(VERSION 3.24)
project(LJF_Opencv_ImageTest)

set(CMAKE_CXX_STANDARD 14)
set(OpenCV_DIR E:\\IDE\\OpenCVcpp\\OpenCV-MinGW-Build-OpenCV-4.5.5-x64\\x64\\mingw\\lib)
find_package(OpenCV REQUIRED)
include_directories(E:\\nodeanddata\\studio\\FSL\\Complete\\S18\\software\\LJF_Opencv_ImageTest)

add_executable(LJF_Opencv_ImageTest
        ImageBasic.cpp
        ImageBasic.h
        ImageConversion.cpp
        ImageConversion.h
        ImageWR.cpp
        ImageWR.h
        main.cpp
        main.h ImageConversion.cpp ImageTrack.cpp ImageTrack.h ImageProcess.cpp ImageProcess.h
        ImageCircleIsland.cpp ImageCircleIsland.cpp ImageSpecial.h Control.cpp Control.h ImageBarrier.cpp)
target_link_libraries(LJF_Opencv_ImageTest ${OpenCV_LIBS})

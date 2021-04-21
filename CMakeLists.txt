cmake_minimum_required(VERSION 3.17.5)
project(websocket_img)

set(CMAKE_CXX_STANDARD 17)

include_directories(/usr/include/jsoncpp)
include_directories(/usr/local/include/drogon)
include_directories(/usr/local/include/opencv4)

add_executable(websocket_img ThreadPool.h main.cpp ImageWebsocket.cpp ImageWebsocket.h
        JsonConvert.cpp JsonConvert.h)

target_link_directories(websocket_img PUBLIC /usr/local/lib)
target_link_libraries(websocket_img opencv_gapi opencv_highgui opencv_ml opencv_objdetect opencv_photo
        opencv_stitching opencv_video opencv_calib3d opencv_features2d opencv_dnn opencv_flann
        opencv_videoio opencv_imgcodecs opencv_imgproc opencv_core)
target_link_libraries(websocket_img drogon trantor)

target_link_directories(websocket_img PUBLIC /usr/local/lib/opencv4/3rdparty)
target_link_libraries(websocket_img ade ippiw libjpeg-turbo libprotobuf quirc IlmImf ittnotify
        libopenjp2 libtiff zlib ippicv libjasper libpng libwebp)

target_link_directories(websocket_img PUBLIC /usr/lib/x86_64-linux-gnu)
target_link_libraries(websocket_img jsoncpp uuid ssl crypto)

target_link_libraries(websocket_img dl pthread)



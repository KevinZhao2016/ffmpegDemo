cmake_minimum_required(VERSION 3.9)
project(ffmpegDemp)

include_directories(/usr/local/Cellar/ffmpeg/4.1.4_2/include/)
link_directories(/usr/local/Cellar/ffmpeg/4.1.4_2/lib/)

set(SOURCE_FILES crypto.cpp  frameSign.cpp framecrypto.cpp mpeg.cpp)

add_executable(ffmpegDemo ${SOURCE_FILES})

target_link_libraries(
        ffmpegDemo
        avcodec
        avdevice
        avfilter
        avformat
        avresample
        avutil
        postproc
        swresample
        swscale
        ssl
        crypto
)
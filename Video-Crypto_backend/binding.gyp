{
  "targets": [
    {
      "target_name": "Video",
      "sources": [
        "video-gyp.cc"
      ],
      "include_dirs": [
        "/usr/local/Cellar/ffmpeg/4.1.4_2/include/",
        "../",
        "/usr/local/include/openssl",
        "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.14.sdk/usr/include/openssl/"
      ],
      "libraries":[
        '-lssl -lcrypto',
        '-lavcodec -lavdevice -lavfilter -lavformat -lavresample -lavutil -lpostproc -lswresample -lswscale'
      ],
      "dependencies": [
        
      ],
      "cflags!": ["-fno-exceptions"],
      "cflags_cc!": ["-fno-exceptions"],
      "defines": ["NAPI_CPP_EXCEPTIONS"],
      "xcode_settings": {
        "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
      }
    }
  ]
}

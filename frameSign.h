//
// Created by Occulticplus on 2019/11/12.
//

#ifndef FFMPEGDEMO_FRAMESIGN_H
#define FFMPEGDEMO_FRAMESIGN_H

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
}

#include <iostream>
#include <cmath>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/ec.h>
#include <openssl/pem.h>
#include <openssl/sgd.h>
#include <openssl/zuc.h>
#include <openssl/gmapi.h>
#include <openssl/bio.h>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <iomanip>

#define at(m, x, y) m[(x) * (linelen) + (y)]

using namespace std;
namespace frameSign {
    typedef uint8_t pixel;
    void join_message(AVFrame *frame, pixel *sign, int height, int width, pixel *join_msg, int msglen);
    pixel* grab_message(AVFrame *frame, pixel *out, int height, int width);
    int grab_message_length();
}

#endif //FFMPEGDEMO_FRAMESIGN_H

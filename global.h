//
// Created by KevinZhao on 2019/9/30.
//

#ifndef FFMPEGDEMO_GLOBAL_H
#define FFMPEGDEMO_GLOBAL_H
extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include "libavutil/log.h"
}

#include <iostream>
#include <stdlib.h>

typedef struct signature {
    unsigned char message[730] = {0};//签名
    int size = 0;
} Signature;
Signature sig = Signature();
pair<string,string> zucKey; //strong,weak
string zuciv;

#endif //FFMPEGDEMO_GLOBAL_H

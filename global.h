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

#define KEYLENGTH 20

typedef struct signature {
    unsigned char message[730] = {0};//签名
    int size = 0;
} Signature;
Signature sig = Signature();
unsigned char zucStrongKey[KEYLENGTH - 1]; //strong,weak
unsigned char zucWeakKey[KEYLENGTH - 1]; //strong,weak
unsigned char zuciv[KEYLENGTH - 1];
string zucStrongKey_base; //strong
string zucWeakKey_base; //weak
string zuciv_base;


string randKey() {
    Base64 base64 = Base64();
    string key;
    unsigned char *key1, *key2;
//    const int SIZE_CHAR = 20; //生成10 + 1位C Style字符串
//        const unsigned char CCH[] = "_0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
    srand((unsigned) time(nullptr));
    unsigned char ch[KEYLENGTH + 1] = {0};
    for (int i = 0; i < KEYLENGTH; ++i) {
        int x = arc4random() / (RAND_MAX / (256 - 1));
        ch[i] = (uint8_t) x;
    }
    key1 = (unsigned char *) ch;
    key = base64.Encode(key1, KEYLENGTH);
//    for (int i = 0; i < SIZE_CHAR; ++i) {
//        int x = rand() / (RAND_MAX / (256 - 1));
//        ch[i] = (uint8_t) x;
//    }
//    key2 = (unsigned char *) ch;
//    weak_key = base64.Encode(key2, SIZE_CHAR);
    return key;
}

void changeKey(unsigned char *key) {
    for (int i = KEYLENGTH - 1; i >= 0; --i) {
        if (key[i] + 1 <= 255) {
            key[i] += 1;
            return;
        }else{
            key[i] = 0;
        }
    }
}

#endif //FFMPEGDEMO_GLOBAL_H

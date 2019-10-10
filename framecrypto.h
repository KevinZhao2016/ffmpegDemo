#ifndef FRAME_CRYPTO
#define FRAME_CRYPTO

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

#define at(m, x, y) m[(y) * (linelen) + (x)]
#define MODE_ENCRYPT 0
#define MODE_DECRYPT 1

using namespace std;

const int MAX_PADDING_LEN = 16;

typedef uint8_t pixel;
typedef int crypto_mode;
void encrypt_frame(AVFrame *frame, int height, int width, int encrypt_height, int encrypt_width);

void dct_frame(AVFrame *frame, int height, int width);
void stream_encrypt();
void idct_frame(AVFrame *frame, int height, int width);



#endif //FRAME_CRYPTO.h

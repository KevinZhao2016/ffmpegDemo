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
#include <cstdio>
#include <fstream>

#define at(m, x, y) m[(x) * (linelen) + (y)]


#define STRONG_LAYER_START 5
#define STRONG_LAYER_END 8
#define WEAK_LAYER_START 10
#define WEAK_LAYER_END 10

#define STRONG_MASK 8
#define WEAK_MASK 4

using namespace std;

const int MAX_PADDING_LEN = 16;

typedef uint8_t pixel;
typedef int crypto_mode;
void encrypt_frame(AVFrame *frame, EVP_CIPHER_CTX *strong_en,EVP_CIPHER_CTX *weak_en, int encrypt_height, int encrypt_width);
void decrypt_frame(AVFrame *frame, EVP_CIPHER_CTX *strong_en,EVP_CIPHER_CTX *weak_en, int encrypt_height, int encrypt_width);

void dct_frame(AVFrame *frame, int height, int width);
void stream_encrypt();
void idct_frame(AVFrame *frame, int height, int width);

void mat_mul(float *A, float *B, float *res, int a, int b, int c);

#endif //FRAME_CRYPTO.h

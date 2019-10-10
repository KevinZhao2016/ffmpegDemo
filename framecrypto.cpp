#include "FrameCrypto.h"

float msk[8][8] = { {16,11,10,16,24,40,51,61},{12,12,14,19,26,58,60,55},{14,13,16,24,40,57,69,56},{14,17,22,29,51,87,80,62},{18,22,37,56,68,109,103,77},{24,35,55,64,81,104,113,92},{49,64,78,87,103,121,120,101},{72,92,95,98,112,100,103,99} };
float pi = acos(-1.0);
float A[8][8], At[8][8];
EVP_CIPHER_CTX *strong_en, *weak_en;

void initDctMat(float **A, float ** At)  //计算8x8块的离散余弦变换系数
{
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
        {
            float a;
            if (i == 0)
                a = sqrt(1.0 / 8.0);
            else
                a = sqrt(2.0 / 8.0);

            At[j][i] = A[i][j] = a * cos((j + 0.5) * pi * i / 8);
            //A.ptr<float>(i)[j] = a * cos((j + 0.5)*pi*i / 8);
        }
}

void stream_encrypt(uint8_t* mat) {

}

void mat_mul(float *A, float *B, float *res, int a, int b, int c) { // Mat A(a*b) multiple with Mat B (b*c)
    int linelen = c;
    for (int i = 0; i < a; i++) {
        for (int j = 0; j < c; j++) {
            at(res, i, j) = 0;
            for (int k = 0; k < b; k++) {
                at(res, i, j) += at(A, i, k) * at(B, k, j);
            }
        }
    }
}

void dct_frame(float *mat, int __height, int __width) {
    float res[8][8];
    float slice[8][8];
    uint8_t discrete_slice[8][8];
    uint8_t msg[8];

    int height = (__height >> 3) << 3, width = (__width >> 3) << 3;
    int linelen = __width;
    for (int i = 0; i < height; i += 8) {
        for (int j = 0; j < width; j += 8) {

            for (int ii = 0; ii < 8; ii++) {
                for (int jj = 0; jj < 8; jj++) { // copy a 8 * 8 block to the mat.
                    slice[ii][jj] = at(mat, i + ii, j + jj);
                }
            }
            // dct = A * block * A^T
            mat_mul((float *)A, (float *)slice, (float *)res, 8, 8, 8);
            mat_mul((float *)res, (float *)At, (float *)slice, 8, 8, 8);

            for (int ii = 0; ii < 8; ii++) {
                for (int jj = 0; jj < 8; jj++) {
                    slice[ii][jj] /= msk[ii][jj];
                    discrete_slice[ii][jj] = (uint8_t)floor(slice[ii][jj]);
                }
            }

            //stream_encrypt();
            /*
             * strong key encryption.
             * layer: 2 (from [0, 2] to [2, 0]), 3, 4, 5
             */
            int msglen;
            for (int layer = 2; layer <= 5; layer++) {
                for (int ii = layer; ii >= 0; layer--) {
                    // encrypt pixel[ii][layer - ii].
                    EVP_EncryptUpdate(strong_en, msg, &msglen, &discrete_slice[ii][layer - ii], 1);
                    discrete_slice[ii][layer - ii] = msg[0];
                }
            }

            /*
             * strong key encryption.
             * layer: 4 (from [0, 4] to [4, 0]), 5
             */
            for (int layer = 4; layer <= 5; layer++) {
                for (int ii = layer; ii >= 0; layer--) {
                    // encrypt pixel[ii][layer - ii].
                    EVP_EncryptUpdate(weak_en, msg, &msglen, &discrete_slice[ii][layer - ii], 1);
                    discrete_slice[ii][layer - ii] = msg[0];
                }
            }
        }
    }


}

void idct_frame(float *mat, int __height, int __width) {
    float res[8][8];
    float slice[8][8];

    int height = (__height >> 3) << 3, width = (__width >> 3) << 3;
    int linelen = __width;
    for (int i = 0; i < height; i += 8) {
        for (int j = 0; j < width; j += 8) {

            for (int ii = 0; ii < 8; ii++) {
                for (int jj = 0; jj < 8; jj++) { // copy a 8 * 8 block to the mat.
                    slice[ii][jj] = at(mat, i + ii, j + jj) * msk[ii][jj];
                }
            }
            // idct = A^T * block(dct) * A
            mat_mul((float *)At, (float *)slice, (float *)res, 8, 8, 8);
            mat_mul((float *)res, (float *)A, (float *)slice, 8, 8, 8);

            for (int ii = 0; ii < 8; ii++) {
                for (int jj = 0; jj < 8; jj++) {
                    at(mat, i + ii, j + jj) = floor(slice[ii][jj]);
                }
            }

            //stream_encrypt();
        }
    }


}

void encrypt_frame(AVFrame *frame, int height, int width, int encrypt_height, int encrypt_width) {
    uint8_t* mat = frame->data[0];
    float* precise_mat = new float[encrypt_height * encrypt_width];
    for (int i = 0; i < encrypt_height * encrypt_width; ++i) {
        precise_mat[i] = (float) mat[i];
    }
    //float A[8][8], At[8][8];

    //EVP_CIPHER_CTX* strong_en;
    //EVP_CIPHER_CTX* weak_en;
    EVP_CIPHER_CTX_init(strong_en);
    const EVP_CIPHER *cipher_type;
    uint8_t *passkey, *passiv, *plaintxt;
    uint8_t *plaintext = nullptr;
    //uint8_t *strong_key, *weak_key;
    unsigned char *strong_key = (unsigned char*) "Tsutsukakushi tsukiko";
    unsigned char *weak_key = (unsigned char*) "Azuki azusa";


    uint8_t strong_iv[] = { 0x00 }, weak_iv[] = { 0x00 };
    cipher_type = EVP_aes_128_ecb();
    cout << EVP_EncryptInit_ex(strong_en, cipher_type, nullptr, strong_key, strong_iv) << endl;
    cout << EVP_EncryptInit_ex(weak_en, cipher_type, nullptr, weak_key, weak_iv) << endl;

    dct_frame(precise_mat, encrypt_height, encrypt_width);

    EVP_CIPHER_CTX_cleanup(strong_en);
    EVP_CIPHER_CTX_cleanup(weak_en);
    delete precise_mat;
}
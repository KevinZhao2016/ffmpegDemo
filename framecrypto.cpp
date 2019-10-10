#include "FrameCrypto.h"

#define debug2

float msk[8][8] = { {16,11,10,16,24,40,51,61},{12,12,14,19,26,58,60,55},{14,13,16,24,40,57,69,56},{14,17,22,29,51,87,80,62},{18,22,37,56,68,109,103,77},{24,35,55,64,81,104,113,92},{49,64,78,87,103,121,120,101},{72,92,95,98,112,100,103,99} };
float pi = acos(-1.0);
float A[8][8], At[8][8];
EVP_CIPHER_CTX *strong_en = EVP_CIPHER_CTX_new(), *weak_en= EVP_CIPHER_CTX_new();

void initDctMat()  //计算8x8块的离散余弦变换系数
{
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            float a;
            if (i == 0)
                a = sqrt(1.0 / 8.0);
            else
                a = sqrt(2.0 / 8.0);

            At[j][i] = A[i][j] = a * cos((j + 0.5) * pi * i / 8);
            cout << A[i][j] << ' ';
            //A.ptr<float>(i)[j] = a * cos((j + 0.5)*pi*i / 8);
        }
        cout << endl;
    }
    //int a;
    //cin >> a;
}

void stream_encrypt(uint8_t* mat) {

}

void mat_mul(float *A, float *B, float *res, int a, int b, int c) { // Mat A(a*b) multiple with Mat B (b*c)
    int linelen = c;
    for (int i = 0; i < a; i++) {
        for (int j = 0; j < c; j++) {
            at(res, i, j) = 0;
            for (int k = 0; k < b; k++) {
                at(res, i, j) += at(A, k, i) * at(B, j, k);
            }
        }
    }
}

void dct_frame(float *mat, int __height, int __width) {
    float res[8][8];
    float slice[8][8];
    uint8_t discrete_slice[8][8];
    uint8_t msg[8];
    float loss[8][8];

    int height = (__height >> 3) << 3, width = (__width >> 3) << 3;
    int linelen = __width;
    for (int i = 0; i < width; i += 8) {
        for (int j = 0; j < height; j += 8) {
            for (int ii = 0; ii < 8; ii++) {
                for (int jj = 0; jj < 8; jj++) { // copy a 8 * 8 block to the mat.
                    slice[ii][jj] = at(mat, i + ii, j + jj);
                }
            }
            // dct = A * block * A^T
#ifdef debug1
            static int matCount = 0;
            if (++matCount % 10 == 0) {
                for (int i = 0; i < 8; i++) {
                    for (int j = 0; j < 8; j++) {
                        cout << slice[i][j] << ' ';
                    }
                    cout << endl;
                }
            }
#endif
            mat_mul((float *)A, (float *)slice, (float *)res, 8, 8, 8);
            mat_mul((float *)res, (float *)At, (float *)slice, 8, 8, 8);

#ifdef debug1
            if (matCount % 10 == 0) {
                cout << "mat mul res:" << endl;
                for (int i = 0; i < 8; i++) {
                    for (int j = 0; j < 8; j++) {
                        cout << slice[i][j] << ' ';
                    }
                    cout << endl;
                }
            }
#endif

            for (int ii = 0; ii < 8; ii++) {
                for (int jj = 0; jj < 8; jj++) {
                    slice[ii][jj] /= msk[ii][jj];
#ifdef debug
                    if (slice[ii][jj] < 8 && fabs(slice[ii][jj] - at(mat, i + ii, j + jj)) > 64) {
                        static int count2 = 0;
                        printf("multiple loss %d: %.5f -> %.5f\n", ++count2, slice[ii][jj], at(mat, i + ii, j + jj));
                    }
#endif
                    discrete_slice[ii][jj] = (uint8_t)floor(slice[ii][jj]);
                    loss[ii][jj] = slice[ii][jj] - discrete_slice[ii][jj];
                }
            }

            //stream_encrypt();
            /*
             * strong key encryption.
             * layer: 2 (from [0, 2] to [2, 0]), 3, 4, 5
             */
            int msglen;
//            for (int layer = 2; layer <= 5; layer++) {
//                for (int ii = layer; ii >= 0; ii--) {
//                    // encrypt pixel[ii][layer - ii].
//                    EVP_EncryptUpdate(strong_en, msg, &msglen, &discrete_slice[ii][layer - ii], 1);
//                    discrete_slice[ii][layer - ii] = msg[0];
//                }
//            }
            /*
             * strong key encryption.
             * layer: 4 (from [0, 4] to [4, 0]), 5
             */
            static int count = 0;
            for (int layer = 10; layer <= 10; layer++) {
                for (int ii = layer; ii >= 0; ii--) {
                    // encrypt pixel[ii][layer - ii].
                    if (ii >= 8 || layer - ii >= 8) continue;

                    EVP_EncryptUpdate(weak_en, msg, &msglen, &discrete_slice[ii][layer - ii], 1);
#ifdef debug
                    if (fabs(discrete_slice[ii][layer - ii] - msg[0]) > 64) {
                        printf("encrypt loss %d: %d -> %d\n", ++count, (int)discrete_slice[ii][layer - ii], (int)msg[0]);
                    }
#endif
                    slice[ii][layer - ii] = (float)msg[0] + loss[ii][layer - ii];
                }
            }
            for (int ii = 0; ii < 8; ii++) {
                for (int jj = 0; jj < 8; jj++) {
                    at(mat, i + ii, j + jj) = slice[ii][jj];
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
    for (int i = 0; i < width; i += 8) {
        for (int j = 0; j < height; j += 8) {
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
    initDctMat();
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
    cipher_type = EVP_zuc();
    EVP_EncryptInit_ex(strong_en, cipher_type, nullptr, strong_key, strong_iv);
    EVP_EncryptInit_ex(weak_en, cipher_type, nullptr, weak_key, weak_iv);

    dct_frame(precise_mat, encrypt_height, encrypt_width);
    idct_frame(precise_mat, encrypt_height, encrypt_width);
    static int count;
//#ifdef spy
    for (int i = 0; i < encrypt_height * encrypt_width; ++i) {
#ifdef spy
         if (fabs(mat[i] - precise_mat[i]) > 64) {
             printf("%d: %d %d\n", ++count, (int)mat[i], (int)precise_mat[i]);
         }
#endif
         mat[i] = (uint8_t)precise_mat[i];
    }
//#endif
    EVP_CIPHER_CTX_cleanup(strong_en);
    EVP_CIPHER_CTX_cleanup(weak_en);
    delete precise_mat;
}
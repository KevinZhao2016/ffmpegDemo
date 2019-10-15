#include "framecrypto.h"

#define debug2
#define debugloss
#define debugZUC

#ifdef debugZUC
#define MODE_ENCRYPT 0
#define MODE_DECRYPT 1
#else
#define MODE_ENCRYPT 2
#define MODE_DECRYPT 2
#endif

float msk[8][8] = {{16, 11, 10, 16, 24,  40,  51,  61},
                   {12, 12, 14, 19, 26,  58,  60,  55},
                   {14, 13, 16, 24, 40,  57,  69,  56},
                   {14, 17, 22, 29, 51,  87,  80,  62},
                   {18, 22, 37, 56, 68,  109, 103, 77},
                   {24, 35, 55, 64, 81,  104, 113, 92},
                   {49, 64, 78, 87, 103, 121, 120, 101},
                   {72, 92, 95, 98, 112, 100, 103, 99}};
float pi = acos(-1.0);
float A[8][8], At[8][8];
pixel strong_plaintext[1 << 23], strong_ciphertext[1 << 23];
pixel weak_plaintext[1 << 23], weak_ciphertext[1 << 23];
int strong_plaintext_pointer = 0, strong_ciphertext_pointer = 0;
int weak_plaintext_pointer = 0, weak_ciphertext_pointer = 0;
static int counter = 0;
#ifdef debugloss
fstream plain_bef, plain_aft, cipher_bef, cipher_aft;
fstream mat_bef, mat_aft;
fstream dct_recover;
#endif

void initFstream() {
    if (counter > 1) return;
    plain_bef.open("plaintext_bef.txt", ios::app | ios::out);
    plain_aft.open("plaintext_aft.txt", ios::app | ios::out);
    cipher_bef.open("ciphertext_bef.txt", ios::app | ios::out);
    cipher_aft.open("ciphertext_aft.txt", ios::app | ios::out);
    mat_bef.open("mat_bef.txt", ios::app | ios::out);
    mat_aft.open("mat_aft.txt", ios::app | ios::out);
    dct_recover.open("dct_recover.txt", ios::app | ios::out);
}

void closeFstream() {
    if (counter > 1) return;
    plain_bef.close();
    plain_aft.close();
    cipher_bef.close();
    cipher_aft.close();
    mat_aft.close();
    mat_bef.close();
    dct_recover.close();
}

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
        }
    }

}

void stream_encrypt(EVP_CIPHER_CTX *ctx, pixel *ciphertext, int *cipher_len, pixel *plaintext, int *plain_len,
                    crypto_mode mode = 0) {
    /*
     * if mode is 0, encode; if mode is 1, decode.
     */

    int rec = 0;
#ifdef debugloss
    cerr << "(cipher_len, plain_len) is : " << *cipher_len << " " << *plain_len << endl;
#endif

    if (mode == 0) {
#ifdef debugloss
        for (int i = 0; i < *plain_len && counter < 2; i++) {
            plain_bef << (int) plaintext[i] << endl;
        }
#endif
        if (EVP_EncryptUpdate(ctx, ciphertext, cipher_len, plaintext, *plain_len) != 1) {
            cout << *plain_len << endl;
            cout << "Panic: Encrypt Update failed" << endl;
            return;
        }
        rec = *cipher_len;
        if (EVP_EncryptFinal_ex(ctx, ciphertext + *cipher_len, cipher_len) != 1) {
            cout << "Panic: Encrypt Final failed" << endl;
            return;
        }
        *cipher_len += rec;
#ifdef debugloss
        cerr << "(cipher_len, plain_len) is : " << *cipher_len << " " << *plain_len << "After ENcryption." << endl;
        for (int i = 0; i < *cipher_len && counter < 2; i++) {
            cipher_bef << (int) ciphertext[i] << endl;
        }
#endif
        //strong_ciphertext_pointer = *cipher_len;
    } else if (mode == 1) {
#ifdef debugloss
        for (int i = 0; i < *cipher_len && counter < 2; i++) {
            cipher_aft << (int) ciphertext[i] << endl;
        }
#endif
        if (EVP_DecryptUpdate(ctx, plaintext, plain_len, ciphertext, *cipher_len) != 1) {
            cout << "Panic: Decrypt failed" << endl;
            return;
        }
        rec = *plain_len;
        if (EVP_DecryptFinal_ex(ctx, plaintext + *plain_len, plain_len) != 1) {
            cout << "Panic: Decrypt failed" << endl;
            return;
        }
        *plain_len += rec;
#ifdef debugloss
        cerr << "(cipher_len, plain_len) is : " << *cipher_len << " " << *plain_len << "After DEcryption." << endl;
        for (int i = 0; i < *plain_len && counter < 2; i++) {
            plain_aft << (int) plaintext[i] << endl;
        }
#endif
        //strong_ciphertext_pointer = *plain_len;
    } else {
        //static int somethingcounter = 0;
        printf("Panic: mode is %d\n", mode);
        int len = max(*plain_len, *cipher_len);
        for (int i = 0; i < len; i++) {
            if (plaintext[i] & ciphertext[i] != 0) {
                printf("%d %d\n", plaintext[i], ciphertext[i]);
            }
            plaintext[i] = ciphertext[i] = plaintext[i] ^ ciphertext[i];

        }
        //cout << "panic" << endl;
    }
    counter++;

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

void
dct_frame(float *mat, int __height, int __width, EVP_CIPHER_CTX *strong_en = nullptr, EVP_CIPHER_CTX *weak_en = nullptr,
          int mode = 0) {
    /*
     * @declytped
     */
    float res[8][8];
    float slice[8][8];
    uint8_t discrete_slice[8][8];
    uint8_t msg[8];
    float loss[8][8];
    int strong_ciphertext_counter, weak_plaintext_counter;
    int height = (__height >> 3) << 3, width = (__width >> 3) << 3;
    int linelen = __width;
    for (int i = 0; i < height; i += 8) {
        for (int j = 0; j < width; j += 8) {
            for (int ii = 0; ii < 8; ii++) {
                for (int jj = 0; jj < 8; jj++) { // copy a 8 * 8 block to the mat.
                    slice[ii][jj] = at(mat, i + ii, j + jj);
#ifdef debugloss
                    if (counter < 2) mat_bef << (float) at(mat, i + ii, j + jj) << ' ';
#endif
                }
#ifdef debugloss
                if (counter < 2) mat_bef << endl;
#endif
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
            mat_mul((float *) A, (float *) slice, (float *) res, 8, 8, 8);
            mat_mul((float *) res, (float *) At, (float *) slice, 8, 8, 8);

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
                    //slice[ii][jj] /= msk[ii][jj];
#ifdef debug
                    if (slice[ii][jj] < 8 && fabs(slice[ii][jj] - at(mat, i + ii, j + jj)) > 64) {
                        static int count2 = 0;
                        printf("multiple loss %d: %.5f -> %.5f\n", ++count2, slice[ii][jj], at(mat, i + ii, j + jj));
                    }
#endif
                    discrete_slice[ii][jj] = (uint8_t) floor(slice[ii][jj]);
                    loss[ii][jj] = slice[ii][jj] - (float) discrete_slice[ii][jj];
                }
            }

            for (int ii = 0; ii < 8; ii++) {
                for (int jj = 0; jj < 8; jj++) {
                    at(mat, i + ii, j + jj) = slice[ii][jj];
                }
            }

            /*
             * Edited by occulticplus at 10/11/2019.
             */

            //printf("pointer is : %d %d \n", strong_plaintext_pointer, weak_plaintext_pointer);
            for (int layer = STRONG_LAYER_START; layer <= STRONG_LAYER_END; layer++) {
                for (int ii = layer; ii >= 0; ii--) {

                    if (ii >= 8 || layer - ii >= 8) continue;
                    strong_plaintext[strong_plaintext_pointer++] = discrete_slice[ii][layer - ii];
                    at(mat, i + ii, j + layer - ii) -= discrete_slice[ii][layer - ii];
                }
            }
            for (int layer = WEAK_LAYER_START; layer <= WEAK_LAYER_END; layer++) {
                for (int ii = layer; ii >= 0; ii--) {

                    if (ii >= 8 || layer - ii >= 8) continue;
                    weak_plaintext[weak_plaintext_pointer++] = discrete_slice[ii][layer - ii];
                    at(mat, i + ii, j + layer - ii) -= discrete_slice[ii][layer - ii];
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
    int strong_ciphertext_counter = 0, weak_ciphertext_counter = 0;
    for (int i = 0; i < height; i += 8) {
        for (int j = 0; j < width; j += 8) {

            // put the ciphertext back to pixels.
            for (int layer = STRONG_LAYER_START; layer <= STRONG_LAYER_END; layer++) {
                for (int ii = layer; ii >= 0; ii--) {
                    if (ii >= 8 || layer - ii >= 8) continue;
                    at(mat, i + ii, j + layer - ii) += strong_ciphertext[strong_ciphertext_counter++];
                }
            }
            for (int layer = WEAK_LAYER_START; layer <= WEAK_LAYER_END; layer++) {
                for (int ii = layer; ii >= 0; ii--) {
                    if (ii >= 8 || layer - ii >= 8) continue;
                    at(mat, i + ii, j + layer - ii) += weak_ciphertext[weak_ciphertext_counter++];
                }
            }

            for (int ii = 0; ii < 8; ii++) {
                for (int jj = 0; jj < 8; jj++) { // copy a 8 * 8 block to the mat.
                    slice[ii][jj] = at(mat, i + ii, j + jj);
#ifdef debugloss
                    if (counter <= 2) mat_aft << (float) at(mat, i + ii, j + jj) << ' ';
#endif
                }
#ifdef debugloss
                if (counter <= 2) mat_aft << endl;
#endif
            }
            // idct = A^T * block(dct) * A
            mat_mul((float *) At, (float *) slice, (float *) res, 8, 8, 8);
            mat_mul((float *) res, (float *) A, (float *) slice, 8, 8, 8);
            for (int ii = 0; ii < 8; ii++) {
                for (int jj = 0; jj < 8; jj++) {
                    at(mat, i + ii, j + jj) = floor(slice[ii][jj]);
#ifdef debugloss
                    if (counter <= 2) dct_recover << (float) slice[ii][jj] << ' ';
#endif
                }
#ifdef debugloss
                if (counter <= 2) dct_recover << endl;
#endif
            }
            //stream_encrypt();
        }
    }
}

void encrypt_frame(AVFrame *frame, EVP_CIPHER_CTX *strong_en, EVP_CIPHER_CTX *weak_en, int encrypt_height,
                   int encrypt_width) {

#ifdef debugloss
    initFstream();
#endif
    strong_plaintext_pointer = 0, strong_ciphertext_pointer = 0;
    weak_plaintext_pointer = 0, weak_ciphertext_pointer = 0;
    memset(strong_ciphertext, 0, sizeof(strong_ciphertext));
    memset(weak_ciphertext, 0, sizeof(weak_ciphertext));
    memset(strong_plaintext, 0, sizeof(strong_plaintext));
    memset(weak_plaintext, 0, sizeof(weak_plaintext));
    uint8_t *mat = frame->data[0];
    float *precise_mat = new float[encrypt_height * encrypt_width];
    for (int i = 0; i < encrypt_height * encrypt_width; ++i) {
        precise_mat[i] = (float) mat[i];
    }
    initDctMat();
    const EVP_CIPHER *cipher_type;
    uint8_t *passkey, *passiv, *plaintxt;
    uint8_t *plaintext = nullptr;

    dct_frame(precise_mat, encrypt_height, encrypt_width, strong_en, weak_en, MODE_ENCRYPT);

    stream_encrypt(strong_en, strong_ciphertext, &strong_ciphertext_pointer, strong_plaintext,
                   &strong_plaintext_pointer, MODE_ENCRYPT);
    stream_encrypt(weak_en, weak_ciphertext, &weak_ciphertext_pointer, weak_plaintext, &weak_plaintext_pointer, MODE_ENCRYPT);

    for (int i = 0; i < 10; i++) {
        cout << (int)strong_ciphertext[i] << ' ';
    }
    cout << endl << " ************** " << endl;
    idct_frame(precise_mat, encrypt_height, encrypt_width);
    static int count;
    for (int i = 0; i < encrypt_height * encrypt_width; ++i) {
#ifdef spy
        if (fabs(mat[i] - precise_mat[i]) > 64) {
            printf("%d: %d %d\n", ++count, (int)mat[i], (int)precise_mat[i]);
        }
#endif
        mat[i] = (uint8_t) precise_mat[i];
    }


//#endif
//    EVP_CIPHER_CTX_cleanup(strong_en);
//    EVP_CIPHER_CTX_cleanup(weak_en);

#ifdef debugloss
    closeFstream();
    counter++;
#endif
    delete[]precise_mat;
}

void decrypt_frame(AVFrame *frame, EVP_CIPHER_CTX *strong_en, EVP_CIPHER_CTX *weak_en, int decrypt_height,
                   int decrypt_width) {

#ifdef debugloss
    initFstream();
#endif
    strong_plaintext_pointer = 0, strong_ciphertext_pointer = 0;
    weak_plaintext_pointer = 0, weak_ciphertext_pointer = 0;
    memset(strong_ciphertext, 0, sizeof(strong_ciphertext));
    memset(weak_ciphertext, 0, sizeof(weak_ciphertext));
    memset(strong_plaintext, 0, sizeof(strong_plaintext));
    memset(weak_plaintext, 0, sizeof(weak_plaintext));
    uint8_t *mat = frame->data[0];
    float *precise_mat = new float[decrypt_height * decrypt_width];
    for (int i = 0; i < decrypt_height * decrypt_width; ++i) {
        precise_mat[i] = (float) mat[i];
    }
    initDctMat();

    dct_frame(precise_mat, decrypt_height, decrypt_width, strong_en, weak_en, MODE_DECRYPT);
    for (int i = 0; i < 10; i ++) {
        cout << (int) strong_plaintext[i] << ' ';
    }
    /* Attention: when decrypt, we should swap the order of the params, because dct/idct always regard input as plaintext.*/
    stream_encrypt(strong_en, strong_plaintext, &strong_plaintext_pointer, strong_ciphertext,
                   &strong_ciphertext_pointer, MODE_DECRYPT);
    stream_encrypt(weak_en, weak_plaintext, &weak_plaintext_pointer, weak_ciphertext, &weak_ciphertext_pointer, MODE_DECRYPT);

    idct_frame(precise_mat, decrypt_height, decrypt_width);

    for (int i = 0; i < decrypt_height * decrypt_width; ++i) {
        mat[i] = (uint8_t) precise_mat[i];
    }
#ifdef debugloss
        closeFstream();
        counter++;
#endif
        delete precise_mat;

}
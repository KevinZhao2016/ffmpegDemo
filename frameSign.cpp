//
// Created by Occulticplus on 2019/11/12.
//
#define debug_msg1

#include "frameSign.h"
namespace frameSign {
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
    pixel grab_msg[1000], join_msg[1000];
    int grab_counter, join_counter;

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

    void split_pixel(pixel *inl, pixel *outl, int bitlen, int bits = 2, int bounds = INF, int bits_for_input = 8) {
        pixel counter = 0, p = 0;
        while (counter < bitlen) {
            pixel pini = counter % bits_for_input, pino = counter % bits;
            pixel v = inl[counter / bits_for_input] & (1 << pini);
            if (counter % bits == 0) {
                p++;
                if (p >= bounds) {
                    cerr << "Error: split_pixel() out of bounds!" << endl;
                    return;
                }
                outl[p] = 0;
            }
            outl[p] += (v << pino);
            counter++;
        }
    }

    void comb_pixel(pixel *inl, pixel *outl, int bitlen, int bits = 2, int bounds = INF) {
        split_pixel(inl, outl, bitlen, 8, INF, bits);
    }

    pixel bit_message(pixel *inl, int counter) {
        // little-endian order.
        pixel a = counter / 8, b = counter % 8;
        return inl[a] & (1 << b);
    }

//    void solve(float *inp, pixel msg, bool isend = false, int linelen = 8, int bits = 2) {
//        int layer = 14 - bits;
//        for (int i = layer; i >= 0; i--) {
//            if (layer - i >= 8 || i >= 8) continue;
//            if (msg ==)
//        }
//    }

    void dct_frame(float *inp, int height, int width, bool isgrab = false) {

        float res[8][8];
        float slice[8][8];
        int __height = (height >> 3) << 3, __width = (width >> 3) << 3;
        int linelen = width;
        for (int i = 0; i < __height; i += 8) {
            for (int j = 0; j < __width; j += 8) {

                for (int ii = 0; ii < 8; ii++) {
                    for (int jj = 0; jj < 8; jj++) {
                        slice[ii][jj] = at(inp, i + ii, j + jj);
                    }
                }
                mat_mul((float *) A, (float *) slice, (float *) res, 8, 8, 8);
                mat_mul((float *) res, (float *) At, (float *) slice, 8, 8, 8);

#ifdef debug_msg
                static int check_counter = 0;
                static int check_grab_counter = 0;
                if (check_counter == 0 || (isgrab && check_grab_counter == 0)) {
                    for (int ii = 0; ii < 8; ii++) {
                        for (int jj = 0; jj < 8; jj++) {
                            cout << slice[ii][jj] << ' ';
                        }
                        cout << endl;
                    }
                    check_counter++;
                    if (isgrab) check_grab_counter++;
                }
#endif

                if (isgrab) {
                    float a = 0, b = 0;
                    int layer = 7;
                    for (int ii = layer; ii >= 4; ii--) {
                        a += slice[layer - ii][ii];
                    }
                    for (int ii = 3; ii >= 0; ii--) {
                        b += slice[3 - ii][ii];
                    }
#ifdef debug_msg
                    cout << "Get sum " << grab_counter << ": " << a << " , " << b << endl;
#endif
                    if (a - b < 8 && b - a < 8) {
                        // read EOF.
                        isgrab = false;
                    } else {
                        if (a - b > 10) {
                            grab_msg[grab_counter++] = 1;
                        } else if (b - a > 10) {
                            grab_msg[grab_counter++] = 0;
                        } else {
                            grab_counter++;
                            cerr << "cannot understand message." << endl;
                        }

#ifdef debug_msg
                        cerr << grab_counter - 1 << ": " << (int) grab_msg[grab_counter - 1] << endl;
#endif
                    }
                }

                for (int ii = 0; ii < 8; ii++) {
                    for (int jj = 0; jj < 8; jj++) {
                        at(inp, i + ii, j + jj) = slice[ii][jj];
                    }
                }
            }
        }
    }

    void idct_frame(float *inp, int height, int width, bool isjoin = false, pixel *join_msg = nullptr, int msglen = 0) {
        float res[8][8];
        float slice[8][8];
        int __height = (height >> 3) << 3, __width = (width >> 3) << 3;
        int linelen = width;

        for (int i = 0; i < __height; i += 8) {
            for (int j = 0; j < __width; j += 8) {

                for (int ii = 0; ii < 8; ii++) {
                    for (int jj = 0; jj < 8; jj++) {
                        slice[ii][jj] = at(inp, i + ii, j + jj);
                    }
                }
#ifdef debug_msg
                static int check_counter = 0;
                if (check_counter == 0) {
                    for (int ii = 0; ii < 8; ii++) {
                        for (int jj = 0; jj < 8; jj++) {
                            cout << slice[ii][jj] << ' ';
                        }
                        cout << endl;
                    }
                    check_counter++;
                }
#endif
                if (isjoin) {

                    int layer = 7;
                    float a = 0,
                    b = 0;
                    for (int ii = layer; ii >= 4; ii--) {
                        a += slice[layer - ii][ii];
                        b += slice[layer + 4 - ii][ii - 4];
                    }

                    if (join_counter == msglen) {
                        // insert EOF
                        static int EOF_counter = 0;
                        cout << "insert EOF " << EOF_counter << endl;
                        if (a > b) {
                            int des = floor((a - b) / 8);
                            for (int ii = layer; ii >= 4; ii--) {
                                slice[layer - ii][ii] -= des;
                                slice[layer + 4 - ii][ii - 4] += des;
                            }
                        } else {
                            int des = floor((b - a)/ 8);
                            for (int ii = layer; ii >= 4; ii--) {
                                slice[layer - ii][ii] += des;
                                slice[layer + 4 - ii][ii - 4] -= des;
                            }
                        }
#ifdef debug_msg
                        for (int ii = layer; ii >= 0; ii--) {
                            cout << slice[layer - ii][ii] << ' ';
                        }
                        cout << endl;

                        float a1 = 0, b1 = 0;
                        for (int ii = layer; ii >= 4; ii--) {
                            a1 += slice[layer - ii][ii];
                            b1 += slice[layer + 4 - ii][ii - 4];
                        }
                        cout << "EOF sum is :" << a1 << " " << b1 << endl;
#endif
                        isjoin = false;
                    } else {
                        if (bit_message(join_msg, join_counter) == 1) {
                            if (a - b < 16) {
                                int des = ceil((16 - (a - b)) / 8);
                                cout << "des is " << des << endl;
                                for (int ii = layer; ii >= 4; ii--) {
                                    slice[layer - ii][ii] += des;
                                    slice[layer + 4 - ii][ii - 4] -= des;
                                }
                            }
                        } else {
                            if (b - a < 16) {
                                int des = ceil((16 - (b - a)) / 8);
                                for (int ii = layer; ii >= 4; ii--) {
                                    slice[layer - ii][ii] -= des;
                                    slice[layer + 4 - ii][ii - 4] += des;
                                }
                            }
                        }
                    }
                    join_counter++;
                }
                static int counter = 0;
                mat_mul((float *) A, (float *) slice, (float *) res, 8, 8, 8);
                mat_mul((float *) res, (float *) At, (float *) slice, 8, 8, 8);
                for (int ii = 0; ii < 8; ii++) {
                    for (int jj = 0; jj < 8; jj++) {
                        at(inp, i + ii, j + jj) = slice[ii][jj];
                    }
                }
            }
        }
    }

    void join_message(AVFrame *frame, pixel *sign, int height, int width, pixel *join_msg, int msglen) {
#ifdef debug_msg
        for (int i = 0; i < 20; i++) {
            cout << (int)join_msg[i] << ' ';
        }
#endif
        initDctMat();
        pixel *mat = frame->data[0];
        float *precise_mat = new float[height * width];
        int linelen = width;
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                at(precise_mat, i, j) = (float) at(mat, i, j);
            }
        }
        cout << "iiin" << endl;
        dct_frame((float *) precise_mat, height, width);
        cout << "mmidle" << endl;
        idct_frame((float *) precise_mat, height, width, true, join_msg, msglen); // join message write in idct_frame, because grabbing message do not affect message.
        cout << "ooout" << endl;
    }

    pixel* grab_message(AVFrame *frame, pixel *out, int height, int width) {
        initDctMat();
        pixel *mat = frame->data[0];
        float *precise_mat = new float[height * width];
        int linelen = width;
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                at(precise_mat, i, j) = (float) at(mat, i, j);
            }
        }
        dct_frame((float *) precise_mat, height, width, true);
        int p = 0;
        for (int i = 0; i < grab_counter; i++) {
            if (i % 8 == 0 && i > 0) {
                p++;
            }
            join_msg[p] += ((grab_msg[i] & 1) << (i % 8));
        }
        join_counter = p + 1;
        return join_msg;
    }

    int grab_message_length() {
        return join_counter;
    }
}
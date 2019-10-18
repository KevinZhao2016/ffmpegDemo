//
// Created by KevinZhao on 2019/9/30.
//
#include "global.h"

using namespace std;

void char2bin(unsigned char num, char *str) {
    for (int i = 0; i < 8; i++) {
        str[i] = (char) (((num >> i) & 1) + '0');
    }
}

int getNum1(const char *str) {
    int count = 0;
    for (int i = 0; i < 8; ++i) {
        if (str[i] == '1')
            count++;
    }
    return count;
}

void insertMark(AVFrame *frame) {
    int i = 0;
    const int delta = 33;
    char str[9];//unsigned char 转二进制结果 小端
    for (i = 0; i < sig.size; ++i) {
        char2bin(sig.message[i], str);
        int start = 240 - delta * getNum1(str);
        int num = start;

        frame->data[1][i * 9] = start;

        int j = 0;
        for (j = 0; j < 8; ++j) {
            int d = (str[j] == '1') ? 1 : -1;
            frame->data[1][i * 9 + 1 + j] = frame->data[1][i * 9 + j] + d * delta;
        }
    }
//    cout << "marked!" << endl;
}

void getMark(AVFrame *frame) {
    int binValue[8] = {1, 2, 4, 8, 16, 32, 64, 128};
    int str[9];
    for (int i = 0; i < 74; ++i) {
        int num = 0;
        for (int j = 0; j < 8; ++j) {
            if (frame->data[1][i * 9 + 1 + j] > frame->data[1][i * 9 + j]) {
                str[j] = 1;
            } else
                str[j] = 0;
            num += str[j] * binValue[j];
        }
        if (i >= 69 && num == 0xaa) { //终结符
            sig.size = i;
//            cout << "size: " << sig.size << endl;
            return;
        } else{
            sig.message[i] = num;
            sig.size++;
        }
    }
}


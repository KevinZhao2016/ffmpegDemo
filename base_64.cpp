//
// Created by KevinZhao on 2019/9/28.
//
#include <iostream>
#include <string>
#include <cstring>
#include "base_64.h"

std::string Base64::Encode(const unsigned char *str, int bytes) {
    int num = 0, bin = 0, i;
    std::string _encode_result;
    const unsigned char *current;
    current = str;
    while (bytes > 2) {
        _encode_result += _base64_table[current[0] >> 2];
        _encode_result += _base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
        _encode_result += _base64_table[((current[1] & 0x0f) << 2) + (current[2] >> 6)];
        _encode_result += _base64_table[current[2] & 0x3f];

        current += 3;
        bytes -= 3;
    }
    if (bytes > 0) {
        _encode_result += _base64_table[current[0] >> 2];
        if (bytes % 3 == 1) {
            _encode_result += _base64_table[(current[0] & 0x03) << 4];
            _encode_result += "==";
        } else if (bytes % 3 == 2) {
            _encode_result += _base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
            _encode_result += _base64_table[(current[1] & 0x0f) << 2];
            _encode_result += "=";
        }
    }
    return _encode_result;
}

int Base64::Decode(const char *base64, unsigned char *bindata) {
    const char *base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int i, j;
    unsigned char k;
    unsigned char temp[4];
    for (i = 0, j = 0; base64[i] != '\0'; i += 4) {
        memset(temp, 0xFF, sizeof(temp));
        for (k = 0; k < 64; k++) {
            if (base64char[k] == base64[i])
                temp[0] = k;
        }
        for (k = 0; k < 64; k++) {
            if (base64char[k] == base64[i + 1])
                temp[1] = k;
        }
        for (k = 0; k < 64; k++) {
            if (base64char[k] == base64[i + 2])
                temp[2] = k;
        }
        for (k = 0; k < 64; k++) {
            if (base64char[k] == base64[i + 3])
                temp[3] = k;
        }

        bindata[j++] = ((unsigned char) (((unsigned char) (temp[0] << 2)) & 0xFC)) |
                       ((unsigned char) ((unsigned char) (temp[1] >> 4) & 0x03));
        if (base64[i + 2] == '=')
            break;

        bindata[j++] = ((unsigned char) (((unsigned char) (temp[1] << 4)) & 0xF0)) |
                       ((unsigned char) ((unsigned char) (temp[2] >> 2) & 0x0F));
        if (base64[i + 3] == '=')
            break;

        bindata[j++] = ((unsigned char) (((unsigned char) (temp[2] << 6)) & 0xF0)) |
                       ((unsigned char) (temp[3] & 0x3F));
    }
    return j;
}

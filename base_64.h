//
// Created by KevinZhao on 2019/9/28.
//

#ifndef FFMPEGDEMO_BASE_64_H
#define FFMPEGDEMO_BASE_64_H

class Base64 {
private:
    std::string _base64_table;
    static const char base64_pad = '=';
public:
    Base64() {
        _base64_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; /*这是Base64编码使用的标准字典*/

    }

    /**
     * 这里必须是unsigned类型，否则编码中文的时候出错
     */
    std::string Encode(const unsigned char *str, int bytes);

    int Decode(const char *base64, unsigned char *bindata);

    void Debug(bool open = true);
};

#endif //FFMPEGDEMO_BASE_64_H

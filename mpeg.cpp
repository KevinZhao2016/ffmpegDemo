//
// Created by KevinZhao on 2019/9/29.
//
#import "main.cpp"
#include "global.h"

using namespace std;

class Mpeg {
private:
    char inpath[40];
    char outpath[40];
    int videoidx;
    int audioidx;
    Crypto crypto;
    AVFormatContext *ic;
    AVFormatContext *oc;
    AVStream *videoStream;
    AVStream *audioStream;
    string base;
    Base64 base64;

public:
    Mpeg() {
        videoidx = -1;
        audioidx = -1;
        crypto = Crypto();
        ic = nullptr;
        oc = nullptr;
        videoStream = nullptr;
        audioStream = nullptr;
        avformat_network_init();
        base64 = Base64();
    }

    pair<string, string> getKeyPair() {
        pair<string, string> key = crypto.GenKey();
        PUBLIC_KEY = key.first;
        PRIVATE_KEY = key.second;
        cout << "publicKey" << endl;
        cout << key.first << endl;
        cout << "privateKey" << endl;
        cout << key.second << endl;
        return key;
    };

    string getSign(const char *infile, string privateKey) {
        PRIVATE_KEY = privateKey;
        strcpy(this->inpath, infile);
        Open_In_fine(inpath, videoidx, audioidx, ic);
        getPktSign(ic, videoidx, audioidx, 0, &sig);
        avformat_close_input(&ic);
        base = base64.Encode(sig.message, sig.size);
        return base;
    }

    int verifySign(const char *file, string sign, string publicKey) {
        strcpy(this->inpath, file);
        PUBLIC_KEY = "-----BEGIN PUBLIC KEY-----\n" + publicKey + "\n-----END PUBLIC KEY-----";
        sig.size = base64.Decode(sign.c_str(), sig.message);
//        cout << sig.size << endl;
        Open_In_fine(inpath, videoidx, audioidx, ic);
        int ans = getPktSign(ic, videoidx, audioidx, 1, &sig);
        avformat_close_input(&ic);
        return ans;
    }

    void encryptFrame(const char *infile, const char *outfile) {
        Open_In_fine(infile, videoidx, audioidx, ic);
        Open_out_put_file(outfile, videoidx, audioidx, videoStream, audioStream, ic, oc);
        write_url_file(ic, oc, videoidx, audioidx, false, 1);
        close_ffmpeg(ic, oc);
        cout << "strongKey" << endl;
        cout << zucStrongKey_base << endl;
        cout << "weakKey" << endl;
        cout << zucWeakKey_base << endl;
        cout << "iv" << endl;
        cout << zuciv_base << endl;
        getKeyPair();
        cout << outfile << endl;
        string sign = getSign(outfile, PRIVATE_KEY);
        cout << "signature" << endl;
        cout << sign << endl;
        string out = outfile;
        insertMark(outfile, ("sign_" + out).c_str(), sign);
        cout << "success" << endl;
    }

    void decryptFrame(const char *infile, const char *outfile, string strongKey, string weakKey, string iv) {
        zucStrongKey_base = strongKey;
        zucWeakKey_base = weakKey;
        zuciv_base = iv;
        Open_In_fine(infile, videoidx, audioidx, ic);
        Open_out_put_file(outfile, videoidx, audioidx, videoStream, audioStream, ic, oc);
        write_url_file(ic, oc, videoidx, audioidx, false, 0);
        close_ffmpeg(ic, oc);
        cout << "success" << endl;
    }

    void insertMark(const char *infile, const char *outfile, const string sign) {
        sig.size = base64.Decode(sign.c_str(), sig.message);
//        sig.message[sig.size++] = 0xaa; //结束符
        base64.Encode(sig.message, sig.size);
        for (int i = sig.size ; i < sig.size + 7; ++i) {
            sig.message[i] = 0;
        } // 添加结尾冗余信息
        sig.message[sig.size + 7] = 128;
        sig.size += 8;
        Open_In_fine(infile, videoidx, audioidx, ic);
        Open_out_put_file(outfile, videoidx, audioidx, videoStream, audioStream, ic, oc);
        write_url_file(ic, oc, videoidx, audioidx, true, -1);
        close_ffmpeg(ic, oc);
        cout << "success" << endl;
    }

    string getWaterMark(const char *file) {
        Open_In_fine(file, videoidx, audioidx, ic);
        getPkt(ic, videoidx, audioidx);
        avformat_close_input(&ic);
        string base = base64.Encode(sig.message, sig.size);
        return base;
    }
};

void signBenchMark(){
    Mpeg mpeg = Mpeg();
    int count = 0;
    for (int i = 0; i < 1; ++i) {
        string sign = mpeg.getSign("people_test_en.mp4", PRIVATE_KEY);
        mpeg.insertMark("people_test_en.mp4","sign_people_test_en.mp4",sign);
        sig = signature();
        cout << "sign " << sign << endl;
        string signOut = mpeg.getWaterMark("sign_people_test_en.mp4");
        cout << "signOut " << signOut << endl;
        if(sign == signOut)
            count++;
//        else
//            break;
        cout << "result: " << count << endl;
    }
    cout << "result: " << count << endl;
}

int main(int argc, char *argv[]) {
    Mpeg mpeg = Mpeg();
    av_log_set_level(AV_LOG_QUIET);
    switch (*argv[1]) {
        case '1':
            mpeg.encryptFrame(argv[2], argv[3]);
            break;
        case '2':
            mpeg.decryptFrame(argv[2], argv[3], argv[4], argv[5], argv[6]);
            break;
        case '3':
            cout << mpeg.getSign(argv[2], argv[3]) << endl;
            break;
        case '4':
            mpeg.insertMark(argv[2], argv[3], argv[4]);
            break;
        case '5':
            cout << mpeg.getWaterMark(argv[2]) << endl;
            break;
        case '6':
            cout << mpeg.verifySign(argv[2], argv[3], argv[4]) << endl;
            break;
        case '7':
            mpeg.getKeyPair();
            break;
        default:
            cout << argv[1] << endl;
            break;
    }

//    mpeg.getKeyPair();
//    mpeg.encryptFrame("people_test.mp4", "people_test_en.mp4");
//    mpeg.decryptFrame("people_test_en.mp4", "hf111.mp4","t/JiAagb+ZV1a2Ml1uuz/RLd9YE=","","F0oZp67i6HvCX+r77k6XjQSGeE0=");
//    cout << mpeg.getSign("people_test_en.mp4", PRIVATE_KEY) << endl;
//    mpeg.insertMark("people_test_en.mp4","sign_people_test_en.mp4","MEUCIHaBCJ7DgMiNZ81NEs1WREIz1aYB+EhxNXYquYtUXp44AiEAuDyzU0igd2o3I4fF2k35ozi1rBVF+3gABgAdgFqpxTI=");
//    sig = signature();
//    cout << mpeg.getWaterMark("sign_people_test_en.mp4") << endl;
//    cout << mpeg.verifySign("output.mp4","MEUCIGp/gDvx1NJ12chETiGjMeKrSR/xnHSu5QCKJDMDjmd5AiEAmCwh+xNMa0KyrlH64OYXckLB70U71ceMo2rcduuVrcI=","MFkwEwYHKoZIzj0CAQYIKoEcz1UBgi0DQgAEGXoWnxB3y+7fKJsVWZbhIkao5+97fU1vKWOIsoQOEshfBjSMt5FDA7Xqg7SBSEk5tAPSRjTDXn1M4w8UgoMGnQ==") << endl;
//    signBenchMark();
    return 0;
}
//
// Created by KevinZhao on 2019/9/29.
//
#import "main.cpp"
#include "global.h"

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
//        cout << "file sign is:" << endl;
//        for (int i = 0; i < sig.size; i++) {
//            printf("%02x ", sig.message[i]);
//        }
//        cout << endl;
        base = base64.Encode(sig.message, sig.size);
        return base;
    }

    int verifySign(const char *file, string sign, string publicKey) {
        strcpy(this->inpath, file);
        PUBLIC_KEY = publicKey;
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
        cout << zucKey.first << endl;
        cout << "weakKey" << endl;
        cout << zucKey.second << endl;
        cout << "iv" << endl;
        cout << zuciv << endl;
        cout << "success" << endl;
    }

    void decryptFrame(const char *infile, const char *outfile, string strongKey, string weakKey, string iv) {
        zucKey.first = strongKey;
        zucKey.second = weakKey;
        zuciv = iv;
        Open_In_fine(infile, videoidx, audioidx, ic);
        Open_out_put_file(outfile, videoidx, audioidx, videoStream, audioStream, ic, oc);
        write_url_file(ic, oc, videoidx, audioidx, false, 0);
        close_ffmpeg(ic, oc);
        cout << "success" << endl;
    }

    void insertMark(const char *infile, const char *outfile, const string sign) {
        sig.size = base64.Decode(sign.c_str(), sig.message);
        sig.message[sig.size++] = 0xaa; //结束符
        base64.Encode(sig.message, sig.size);
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


int main(int argc, char *argv[]) {
    Mpeg mpeg = Mpeg();
    Base64 base64 = Base64();
    av_log_set_level(AV_LOG_QUIET);
    switch (*argv[2]) {
        case '1':
            mpeg.encryptFrame(argv[3], argv[4]);
            break;
        case '2':
            mpeg.decryptFrame(argv[3], argv[4], argv[5], argv[6], argv[7]);
            break;
        case '3':
            cout << mpeg.getSign(argv[3], argv[4]) << endl;
            break;
        case '4':
            mpeg.insertMark(argv[3], argv[4], argv[5]);
            break;
        case '5':
            cout << mpeg.getWaterMark(argv[3]) << endl;
            break;
        case '6':
            cout << mpeg.verifySign(argv[3], argv[4], argv[5]) << endl;
            break;
        case '7':
            mpeg.getKeyPair();
            break;
        default:
            cout << argv[2] << endl;
            break;
    }

//    mpeg.getKeyPair();
//    mpeg.encryptFrame("test_golf.mp4", "test_golf_cry111.mp4");
//    mpeg.decryptFrame("test_golf_cry.mp4", "golf_hf1.mp4");
//    cout << mpeg.getSign("test_golf.mp4", PRIVATE_KEY) << endl;
//    mpeg.insertMark("test.mp4","test1.mp4","MEYCIQDlFzDPUXPPWv42xQoU6FUxdh/MXqlE9dRsK6GW7cFQLQIhAMES3Sf8Nh2BSOY8dM98OvBMDqw//yG0IXV2HvjX6I8B");
//    cout << mpeg.getWaterMark("test1.mp4") << endl;
//    cout << mpeg.verifySign("test1.mp4","MEYCIQDlFzDPUXPPWv42xQoU6FUxdh/MXqlE9dRsK6GW7cFQLQIhAMES3Sf8Nh2BSOY8dM98OvBMDqw//yG0IXV2HvjX6I8B",PUBLIC_KEY) << endl;
    return 0;
}
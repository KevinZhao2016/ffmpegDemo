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
        cout << "public key :" << endl;
        cout << key.first << endl;
        cout << "private key: " << endl;
        cout << key.second << endl;
        return key;
    };

    string getSign(const char *infile, string privateKey) {
        PRIVATE_KEY = privateKey;
        strcpy(this->inpath, infile);
        Open_In_fine(inpath, videoidx, audioidx, ic);
        getPktSign(ic, videoidx, audioidx, 0, &sig);
        avformat_close_input(&ic);
        cout << "file sign is:" << endl;
        for (int i = 0; i < sig.size; i++) {
            printf("%02x ", sig.message[i]);
        }
        cout << endl;
        base = base64.Encode(sig.message, sig.size);
        return base;
    }

    int verifySign(const char *file, string sign, string publicKey) {
        strcpy(this->inpath, file);
        PUBLIC_KEY = publicKey;
        sig.size = base64.Decode(sign.c_str(), sig.message);
        cout << sig.size << endl;
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
    }

    void decryptFrame(const char *infile, const char *outfile) {
        Open_In_fine(infile, videoidx, audioidx, ic);
        Open_out_put_file(outfile, videoidx, audioidx, videoStream, audioStream, ic, oc);
        write_url_file(ic, oc, videoidx, audioidx, false, 0);
        close_ffmpeg(ic, oc);
    }

    void waterMark(const char *infile, const char *outfile, const string sign) {
        sig.size = base64.Decode(sign.c_str(), sig.message);
        sig.message[sig.size++] = 0xaa; //结束符
        cout << base64.Encode(sig.message, sig.size) << endl;
        Open_In_fine(infile, videoidx, audioidx, ic);
        Open_out_put_file(outfile, videoidx, audioidx, videoStream, audioStream, ic, oc);
        write_url_file(ic, oc, videoidx, audioidx, true, -1);
        close_ffmpeg(ic, oc);
    }

    string getWaterMark(const char *file) {
        Open_In_fine(file, videoidx, audioidx, ic);
        getPkt(ic, videoidx, audioidx);
        avformat_close_input(&ic);
        string base = base64.Encode(sig.message, sig.size);
        return base;
    }
};

#define encrypt

void test() {
    typedef uint8_t uchar;
    const int maxsize = 1e5;
    uchar plaintext[maxsize] = "aaaaaaaaaaaaaaaa";
    uchar msg[maxsize];

    uchar loop[] = { 114, 51, 4, 191, 98, 10, 89, 33, 64 };
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    OPENSSL_config(NULL);
    uchar* key = (uchar *) "Inaba Himeko";
    uchar* text = (uchar *) "Araragi Koyomi, Karen and Tsukika";
    EVP_CIPHER_CTX* en = EVP_CIPHER_CTX_new();
//    EVP_CIPHER_CTX_init(en);
    uchar* iv = (uchar *) "Otonashi Yuzuru";
    const EVP_CIPHER* encode_type = EVP_zuc();
    EVP_EncryptInit_ex(en, encode_type, nullptr, key, iv);

#ifdef encrypt
    freopen("plaintext.txt", "w", stdout);
//	for (int i = 0; i < maxsize - 16; i++) {
//		plaintext[i] = loop[i % 7];
//		cout << (int)plaintext[i] << endl;
//	}
	freopen("ciphertext.txt", "w", stdout);


	int msglen = 0;
	if (EVP_EncryptUpdate(en, msg, &msglen, plaintext, 16) != 1) {
		cerr << "Something went wrong" << endl;
	}
	cerr << msglen << endl;
	if (1 != EVP_EncryptFinal_ex(en, msg + msglen, &msglen)) {
		cerr << "Something went wrong" << endl;
	}
	cerr << msglen << endl;

#else
    freopen("ciphertext.txt", "r", stdin);
    freopen("plaintext_decode.txt", "w", stdout);
    for (int i = 0; i < maxsize - 16; i++) {
        int k = 0;
        cin >> k;
        plaintext[i] = (uchar)k;
    }

    int msglen = 0;
    if (EVP_DecryptUpdate(en, msg, &msglen, plaintext, 16) != 1) {
        cerr << "Something went wrong" << endl;
    }
    cerr << msglen << endl;
    if (1 != EVP_DecryptFinal_ex(en, msg + msglen, &msglen)) {
        cerr << "Something went wrong" << endl;
    }
    cerr << msglen << endl;

#endif

    EVP_CIPHER_CTX_cleanup(en);

    for (int i = 0; i < maxsize - 16; i++) {
        int k = (unsigned int)msg[i];
        printf("%d\n", k);
        //cout << k << endl;
    }

}

int main() {
    Mpeg mpeg = Mpeg();
    Base64 base64 = Base64();
//    ZUCTest();
//    Crypto crypto1 = Crypto();
//    Crypto crypto2 = Crypto();
//    crypto1.initZUC((unsigned char *)"Tsutsukakushi tsukiko",(unsigned char *)"Azuki azusa");
//    crypto2.initZUC((unsigned char *)"Tsutsukakushi tsukiko",(unsigned char *)"Azuki azusa");
//    int outputlen = 0;
//    unsigned char msg1[17] = {0},msg2[17],out[17];
//    EVP_EncryptUpdate(crypto1.strong_en, msg1, &outputlen, (unsigned char *)"aaaaaaaaaaaaaaaa", 16);
//    EVP_EncryptFinal_ex(crypto1.strong_en,msg1+outputlen,&outputlen);
//
//    cout << (int)msg1[1] << endl;
//    cout << ((int)msg1[1]^0xaa) << endl;
//    msg1[1] = 0xaa;
//    EVP_DecryptUpdate(crypto2.strong_en, msg2, &outputlen, msg1, 16);
//    EVP_DecryptFinal_ex(crypto2.strong_en,out,&outputlen);
//    msg2[16] = '\0';
//    cout << ((int)msg2[1]^(int)'a') << endl;
//    test();
//    mpeg.encryptFrame("test.mp4", "test1.mp4");
//    mpeg.decryptFrame("test1.mp4", "test2.mp4");
//    cout << mpeg.getSign("test.mp4",PRIVATE_KEY) << endl;
//    mpeg.waterMark("test.mp4","test1.mp4","MEYCIQDlFzDPUXPPWv42xQoU6FUxdh/MXqlE9dRsK6GW7cFQLQIhAMES3Sf8Nh2BSOY8dM98OvBMDqw//yG0IXV2HvjX6I8B");
//    cout << mpeg.getWaterMark("test1.mp4") << endl;
//    cout << mpeg.verifySign("test1.mp4","MEYCIQDlFzDPUXPPWv42xQoU6FUxdh/MXqlE9dRsK6GW7cFQLQIhAMES3Sf8Nh2BSOY8dM98OvBMDqw//yG0IXV2HvjX6I8B",PUBLIC_KEY) << endl;
    return 0;
}
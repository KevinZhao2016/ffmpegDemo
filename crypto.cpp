//
// Created by KevinZhao on 2019/9/24.
//
#include <iostream>
#include <ctime>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/ec.h>
#include <openssl/pem.h>
#include <openssl/sgd.h>
#include <openssl/zuc.h>
#include <openssl/gmapi.h>
#include <openssl/bio.h>

using namespace std;

class Crypto {
public:
    EVP_MD_CTX *mdctx;
    EVP_CIPHER_CTX *strong_en, *weak_en;
    EC_KEY *Key;

    Crypto() {
/* Load the human readable error strings for libcrypto */
        ERR_load_crypto_strings();

/* Load all digest and cipher algorithms */
        OpenSSL_add_all_algorithms();

        ERR_load_crypto_strings();
    }

    ~Crypto() {
        /* Removes all digests and ciphers */
        EVP_cleanup();
        CRYPTO_cleanup_all_ex_data();
    }

    pair<string, string> GenKey() {
        EC_KEY *keypair = NULL;
        EC_GROUP *group1 = NULL;

        keypair = EC_KEY_new();
        if (!keypair) {
            cout << "Failed to Gen Key" << endl;
            exit(1);
        }

        group1 = EC_GROUP_new_by_curve_name(NID_sm2p256v1);

        if (group1 == NULL) {
            cout << "Failed to Gen Key" << endl;
            exit(1);
        }

        int ret1 = EC_KEY_set_group(keypair, group1);
        if (ret1 != 1) {
            cout << "Failed to Gen Key" << endl;
            exit(1);
        }

        int ret2 = EC_KEY_generate_key(keypair);
        if (ret2 != 1) {
            cout << "Failed to Gen Key" << endl;
            exit(1);
        }

        size_t pri_len;
        size_t pub_len;
        char *pri_key = NULL;
        char *pub_key = NULL;

        BIO *pri = BIO_new(BIO_s_mem());
        BIO *pub = BIO_new(BIO_s_mem());

        PEM_write_bio_ECPrivateKey(pri, keypair, NULL, NULL, 0, NULL, NULL);
        PEM_write_bio_EC_PUBKEY(pub, keypair);

        pri_len = BIO_pending(pri);
        pub_len = BIO_pending(pub);

        pri_key = new char[pri_len + 1];
        pub_key = new char[pub_len + 1];

        BIO_read(pri, pri_key, pri_len);
        BIO_read(pub, pub_key, pub_len);

        pri_key[pri_len] = '\0';
        pub_key[pub_len] = '\0';

        string public_key = pub_key;
        string private_key = pri_key;

        EC_KEY_free(keypair);
        BIO_free_all(pub);
        BIO_free_all(pri);
        delete[] pri_key;
        delete[] pub_key;

        return std::pair<string, string>(public_key, private_key);
    }

    EC_KEY *CreateEC(unsigned char *key, int is_public) {
        EC_KEY *ec_key = nullptr;
        BIO *keybio = nullptr;
        cout << key << endl;

        keybio = BIO_new_mem_buf(key, -1);
        if (keybio == nullptr) {
            cout << "Failed to Get Key" << endl;
            exit(1);
        }

        if (is_public) {
            ec_key = PEM_read_bio_EC_PUBKEY(keybio, nullptr, nullptr, nullptr);
        } else {
            ec_key = PEM_read_bio_ECPrivateKey(keybio, nullptr, nullptr, nullptr);
        }

        if (ec_key == nullptr) {
            cout << "Failed to Get Key" << endl;
            printError();
            exit(1);
        }
        return ec_key;
    }

    void getHashBySM3(unsigned char *msgbuf, unsigned char *dgst, int len) {
        EVP_MD_CTX *mdctx = nullptr;
        unsigned int dgstlen = EVP_MAX_MD_SIZE;
        mdctx = EVP_MD_CTX_new();
        EVP_DigestInit_ex(mdctx, EVP_sm3(), nullptr);

        EVP_DigestUpdate(mdctx, msgbuf, len);

        EVP_DigestFinal_ex(mdctx, dgst, &dgstlen);
        EVP_MD_CTX_free(mdctx);
    }

    void initSM2() {
        this->mdctx = EVP_MD_CTX_create();
        EVP_SignInit_ex(mdctx, EVP_sm3(), nullptr);
    }


    void UpdateSignBySM2(int64_t *msg, int len) {
        cout << "SM2 update:" << " ";
        cout << EVP_DigestUpdate(mdctx, msg, sizeof(msg)) << endl;
    }

    unsigned int finishSigh(unsigned char *sig, const string &private_key) {
        unsigned int slen = 0;
        this->Key = CreateEC((unsigned char *) private_key.c_str(), 0);
        EVP_PKEY *evpkey = EVP_PKEY_new();
        EVP_PKEY_set1_EC_KEY(evpkey, this->Key);

        EVP_SignFinal(this->mdctx, sig, &slen, evpkey);
        cout << "slen: " << slen << endl;
        EVP_MD_CTX_destroy(mdctx);
        return slen;
    }

    int finishVerify(unsigned char *sig, int size, const string &public_key) {
        int res = 0;
        this->Key = CreateEC((unsigned char *) public_key.c_str(), 1);
        EVP_PKEY *evpkey = EVP_PKEY_new();
        EVP_PKEY_set1_EC_KEY(evpkey, this->Key);
        res = EVP_VerifyFinal(this->mdctx, sig, size, evpkey);
        return res;
    }

    void initZUC(unsigned char *strong_key, unsigned char *weak_key) {
        uint8_t strong_iv[] = {0x00}, weak_iv[] = {0x00};
        this->strong_en = EVP_CIPHER_CTX_new();
        this->weak_en = EVP_CIPHER_CTX_new();
        EVP_EncryptInit_ex(strong_en, EVP_zuc(), nullptr, strong_key, strong_iv);
        EVP_EncryptInit_ex(weak_en, EVP_zuc(), nullptr, weak_key, weak_iv);
    }

    pair<string, string> randKey() {
        string strong_key, weak_key;
        const int SIZE_CHAR = 10; //生成10 + 1位C Style字符串
        const unsigned char CCH[] = "_0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
        srand((unsigned) time(nullptr));
        unsigned char ch[SIZE_CHAR + 1] = {0};
        for (int i = 0; i < SIZE_CHAR; ++i) {
            int x = rand() / (RAND_MAX / (sizeof(CCH) - 1));
            ch[i] = CCH[x];
        }
        strong_key = (char *)ch;
        for (int i = 0; i < SIZE_CHAR; ++i) {
            int x = rand() / (RAND_MAX / (sizeof(CCH) - 1));
            ch[i] = CCH[x];
        }
        weak_key = (char *)ch;
        return pair<string, string>(strong_key, weak_key);
    }

    void printError() {
        char errBuf[512] = {0};
        ERR_error_string_n(ERR_get_error(), errBuf, sizeof(errBuf));
        cout << errBuf << endl;
    }

};

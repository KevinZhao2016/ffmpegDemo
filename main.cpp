#include "crypto.cpp"
#include "base_64.cpp"
#include "signMark.cpp"
#include "global.h"
#include "framecrypto.h"
#include <ctime>

using namespace std;
int fcount = 0;

string PRIVATE_KEY = "-----BEGIN EC PRIVATE KEY-----\n"
        "MHcCAQEEIAtlQJ1LgbDD0ME126lKWwzdGd8Hc8TpH04ye9Y1pPEXoAoGCCqBHM9V\n"
        "AYItoUQDQgAEGXoWnxB3y+7fKJsVWZbhIkao5+97fU1vKWOIsoQOEshfBjSMt5FD\n"
        "A7Xqg7SBSEk5tAPSRjTDXn1M4w8UgoMGnQ==\n"
        "-----END EC PRIVATE KEY-----";
string PUBLIC_KEY = "-----BEGIN PUBLIC KEY-----\n"
        "MFkwEwYHKoZIzj0CAQYIKoEcz1UBgi0DQgAEGXoWnxB3y+7fKJsVWZbhIkao5+97\n"
        "fU1vKWOIsoQOEshfBjSMt5FDA7Xqg7SBSEk5tAPSRjTDXn1M4w8UgoMGnQ==\n"
        "-----END PUBLIC KEY-----";

string sig_base64 = "MEUCIEkjtHpklPWvrdncb3N2UZQltmpnGywEz77VmPvQQWRfAiEA5vM8jBBQtkKv6hXVGOMXzYLIDnQne+obo8U2SxeRrgE=";


int Open_In_fine(const char *infile, int &videoidx, int &audioidx, AVFormatContext *&ic) {
    avformat_open_input(&ic, infile, nullptr, nullptr);
    if (!ic) {
        cout << "avformat_open_input failed!" << endl;
        return -1;
    }
    for (int i = 0; i < ic->nb_streams; i++) {
        if (ic->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
            videoidx = i;
        if (ic->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
            audioidx = i;
    }
    if (videoidx == -1) {
        cout << "Codec find failed!" << endl;
        return -1;
    }
//    av_dump_format(ic, 0, infile, 0); //视频基本信息
//    cout << "open......" << endl;
    return 0;
}

int Open_out_put_file(const char *outfile, int &videoidx, int &audioidx, AVStream *videoStream, AVStream *audioStream,
                      AVFormatContext *&ic, AVFormatContext *&oc) {
    avformat_alloc_output_context2(&oc, nullptr, nullptr, outfile);
    if (!oc) {
        cout << "avformat_alloc_output_context2 " << outfile << " failed!" << endl;
        return -1;
    }
    if (videoidx != -1) {
        videoStream = avformat_new_stream(oc, nullptr);
        avcodec_parameters_copy(videoStream->codecpar, ic->streams[videoidx]->codecpar);
        videoStream->codecpar->codec_tag = 0;
    }
    if (audioidx != -1) {
        audioStream = avformat_new_stream(oc, nullptr);
        avcodec_parameters_copy(audioStream->codecpar, ic->streams[audioidx]->codecpar);
        audioStream->codecpar->codec_tag = 0;
    }

//    av_dump_format(oc, 0, outfile, 1);//视频基本信息
    int ret = avio_open(&oc->pb, outfile, AVIO_FLAG_WRITE);
    if (ret < 0) {
        cout << "avio open failed!" << endl;
        return -1;
    }
    ret = avformat_write_header(oc, nullptr);
    if (ret < 0) {
        cout << "avformat_write_header failed!" << endl;
    }
    return 0;
}

void Time_base(AVPacket *pkt, AVFormatContext *&ic, AVFormatContext *&oc) {
    pkt->pts = av_rescale_q_rnd(pkt->pts,
                                ic->streams[pkt->stream_index]->time_base,
                                oc->streams[pkt->stream_index]->time_base,
                                (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX)
    );
    pkt->dts = av_rescale_q_rnd(pkt->dts,
                                ic->streams[pkt->stream_index]->time_base,
                                oc->streams[pkt->stream_index]->time_base,
                                (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX)
    );
    pkt->pos = -1;
    pkt->duration = av_rescale_q_rnd(pkt->duration,
                                     ic->streams[pkt->stream_index]->time_base,
                                     oc->streams[pkt->stream_index]->time_base,
                                     (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX)
    );
}

//void encrypt_frame(AVFrame *frame, int height, int width) {
//    //todo
//    for (int y = 0; y < height; ++y) {
//        for (int x = 0; x < width; ++x) {
//            //帧加密
//            //frame->data[0][y * frame->linesize[0] + x] = x + y + 3;
//            frame->data[0][y * frame->linesize[0] + x] /= 2;
//        }
//    }
//}

bool flag = true;


void
av_decode_encode_frame(AVCodecContext *ct, AVCodecContext *outAVCodecContext, AVFormatContext *ic, AVFormatContext *oc,
                       AVPacket *pkt, AVFrame *frame, bool watermark, int mode, int &count) {
    int height = ct->height, width = ct->width;
    int value = avcodec_send_packet(ct, pkt);
    //水印时找第一个非关键帧 count
    if (value < 0) {
        cout << "result:" << value << endl;
        cout << "send frame failed" << endl;
        return;
    }

    while (value >= 0) {
        value = avcodec_receive_frame(ct, frame);
        if (value == AVERROR(EAGAIN) || value == AVERROR_EOF) {
            cout << "need more pkt" << endl;
            break;
        } else if (value < 0) {
            cout << "Error during decoding" << endl;
            return;
        }
//        printf("receive frame %3d\n", ct->frame_number);


        if (!watermark) {
//            fcount++;
//            frame->pict_type = AV_PICTURE_TYPE_I;
            if (mode == 1) {
                if (frame->key_frame) {
                    clock_t start, ends;
                    start = clock();
                    Crypto crypto = Crypto();
                    changeKey(zucStrongKey);
                    changeKey(zucWeakKey);
                    crypto.initZUC((unsigned char *) zucStrongKey, (unsigned char *) zucWeakKey,
                                   (unsigned char *) zuciv);
                    //加密关键帧
                    frameCrypto::encrypt_frame(frame, crypto.strong_en, crypto.weak_en, height, frame->linesize[0]);
                    ends = clock();
//                    cout << "time: " << (double) (ends - start) / CLOCKS_PER_SEC * 1000 << endl;

                }
                flag = false;
            } else if (mode == 0) {
                changeKey(zucStrongKey);
                changeKey(zucWeakKey);
                if (frame->key_frame) {
                    clock_t start, ends;
                    start = clock();
                    //解密
                    Crypto crypto = Crypto();
                    crypto.initZUC((unsigned char *) zucStrongKey, (unsigned char *) zucWeakKey,
                                   (unsigned char *) zuciv);
                    frameCrypto::decrypt_frame(frame, crypto.strong_en, crypto.weak_en, height, frame->linesize[0]);
                    ends = clock();
//                    cout << "time: " << (double) (ends - start) / CLOCKS_PER_SEC * 1000 << endl;
                }
                flag = false;
            }

        }

        if (watermark && count == 0 && frame->key_frame == 1) { //找到第一个关键帧，插入水印
            count++;
            clock_t start, ends;
            start = clock();
//            insertMark(frame);
            frameSign::join_message(frame, sig.message, height, width, sig.message, sig.size);
            ends = clock();
//            cout << "time: " << (double) (ends - start) / CLOCKS_PER_SEC * 1000 << endl;

        }

        value = avcodec_send_frame(outAVCodecContext, frame);
        if (value < 0) {
            cout << "value: " << value << endl;
            cout << "Error sending a frame for encoding" << endl;
            return;
        }
        while (value >= 0) {
            value = avcodec_receive_packet(outAVCodecContext, pkt);
            if (value == AVERROR(EAGAIN) || value == AVERROR_EOF) {
                return;
            } else if (value < 0) {
                cout << "Error during encoding" << endl;
                return;
            }
            Time_base(pkt, ic, oc);
            av_write_frame(oc, pkt);
        }
    }
}

int getPktSign(AVFormatContext *ic, int &videoidx, int &audioidx, int if_verify, signature *sig) {
    int count = 0; //从第二个关键帧开始签名
    Crypto crypto = Crypto();
    crypto.initSM2();
    int a = 100;
    crypto.UpdateSignBySM2(&a, 0);

    AVPacket *pkt = av_packet_alloc();
    enum AVCodecID codec_id = AV_CODEC_ID_H264;//解码编码
    AVCodec *pCodec = avcodec_find_decoder(codec_id);//找解码器
    AVCodecContext *pCodecCtx = avcodec_alloc_context3(pCodec);//分配AVCodecContext上下文
    avcodec_parameters_to_context(pCodecCtx, ic->streams[videoidx]->codecpar);
    if (!pCodecCtx) {
        cout << "Could not allocate video codec context" << endl;
        return -1;
    }
    if (avcodec_open2(pCodecCtx, pCodec, nullptr) < 0) {//打开解码器
        printf("Could not open decodec\n");
        return -1;
    }
    clock_t start, ends;
    start = clock();
    while (av_read_frame(ic, pkt) >= 0) {
        if (pkt->stream_index == videoidx) {
            if (pkt->flags == AV_PKT_FLAG_KEY) { //关键帧 取hash
                if (count > 1 && count <= 200)
                    crypto.UpdateSignBySM2(&pkt->flags, 4);
                count++;
            }
            continue;
        } else if (pkt->stream_index == audioidx) {

            continue;
        }
        av_packet_unref(pkt);
        av_packet_free(&pkt);
    }
    unsigned char *p = sig->message;
    if (if_verify) {
        int ans = crypto.finishVerify(p, sig->size, PUBLIC_KEY);
        ends = clock();
//        cout << "time: " << (double) (ends - start) / CLOCKS_PER_SEC << endl;
        return ans;
    } else {
        sig->size = crypto.finishSigh(p, PRIVATE_KEY);
        ends = clock();
//        cout << "time: " << (double) (ends - start) / CLOCKS_PER_SEC << endl;
        return -1;
    }
    avcodec_free_context(&pCodecCtx);
}

void decodeFrame(AVCodecContext *ct, AVPacket *pkt, AVFrame *frame, int &count) {
    int height = ct->height, width = ct->width;
    int value = avcodec_send_packet(ct, pkt);
    int i = 0;
    //水印时找第一个关键帧
    if (value < 0) {
        cout << "result:" << value << endl;
        cout << "send frame failed" << endl;
        return;
    }
    while (value >= 0) {
        value = avcodec_receive_frame(ct, frame);
        if (value == AVERROR(EAGAIN) || value == AVERROR_EOF) {
            cout << "need more pkt" << endl;
            break;
        } else if (value < 0) {
            cout << "Error during decoding" << endl;
            return;
        }
//        printf("receive frame %3d\n", ct->frame_number);
        if (frame->key_frame == 1) { //找到第一个关键帧，取出水印
//            cout << "found!" << endl;
            clock_t start, ends;
            start = clock();
//            getMark(frame);
            frameSign::grab_message(frame, sig.message, height, width);
            sig.size = frameSign::grab_message_length();
            sig.size -= 8;
            count++;
            ends = clock();
//            cout << "time: " << (double) (ends - start) / CLOCKS_PER_SEC * 1000 << endl;
            return;
        }
    }
}

void getPkt(AVFormatContext *ic, int &videoidx, int &audioidx) {
    AVPacket *pkt = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    enum AVCodecID codec_id = AV_CODEC_ID_H264;//解码编码
    AVCodec *pCodec = avcodec_find_decoder(codec_id);//找解码器
    AVCodecContext *pCodecCtx = avcodec_alloc_context3(pCodec);//分配AVCodecContext上下文
    avcodec_parameters_to_context(pCodecCtx, ic->streams[videoidx]->codecpar);
    if (!pCodecCtx) {
        cout << "Could not allocate video codec context" << endl;
        return;
    }
    AVDictionary *param = nullptr;
    av_dict_set(&param, "preset", "ultrafast", 0);
//    av_dict_set(&param, "qp", "0", 0);
//    av_dict_set(&param, "profile", "baseline", 0);
//    av_dict_set(&param, "tune", "zerolatency", 0);  //实现实时编码
    if (avcodec_open2(pCodecCtx, pCodec, &param) < 0) {//打开解码器
        printf("Could not open decodec\n");
        return;
    }
    int count = 0;
    while (av_read_frame(ic, pkt) >= 0 && count == 0) {
        if (pkt->stream_index == videoidx) {
            decodeFrame(pCodecCtx, pkt, frame, count);
            //获取签名 保存到sig
            continue;
        } else if (pkt->stream_index == audioidx) {

            continue;
        }
        av_packet_unref(pkt);
        av_packet_free(&pkt);
    }
    avcodec_free_context(&pCodecCtx);
}

void write_url_file(AVFormatContext *ic, AVFormatContext *oc, int &videoidx, int &audioidx, bool watermark, int mode) {
    AVPacket *pkt = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    enum AVCodecID codec_id = AV_CODEC_ID_H264;//解码编码

    AVCodec *pCodec = avcodec_find_decoder(codec_id);//找解码器
    AVCodec *penCodec = avcodec_find_encoder(codec_id);//找编码器

    AVCodecContext *pCodecCtx = avcodec_alloc_context3(pCodec);//分配AVCodecContext上下文
    avcodec_parameters_to_context(pCodecCtx, ic->streams[videoidx]->codecpar);

    if (!pCodecCtx) {
        cout << "Could not allocate video codec context" << endl;
        return;
    }
    AVDictionary *param = nullptr;
    av_dict_set(&param, "preset", "ultrafast", 0);
//    av_dict_set(&param, "qp", "0", 0);
//    av_dict_set(&param,"profile","0", 0);
    av_dict_set(&param, "tune", "zerolatency", 0);  //实现实时编码
    if (avcodec_open2(pCodecCtx, pCodec, &param) < 0) {//打开解码器
        printf("Could not open decodec\n");
        return;
    }

    AVCodecContext *poutCodecCtx = avcodec_alloc_context3(penCodec);//分配AVCodecContext上下文
    if (!poutCodecCtx) {
        cout << "Could not allocate video codec context" << endl;
        return;
    }
    if (avcodec_parameters_to_context(poutCodecCtx, ic->streams[videoidx]->codecpar) < 0) {
        cout << "copy params failed" << endl;
    }
    poutCodecCtx->time_base = (AVRational) {1, 30};
    poutCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    poutCodecCtx->bit_rate = 1000 * 1000 * 50;
    poutCodecCtx->qmax = 15;
    poutCodecCtx->qmin = 1;
//    poutCodecCtx->gop_size = 3;
    if (avcodec_open2(poutCodecCtx, penCodec, &param) < 0) {//打开编码器
        printf("Could not open encodec\n");
        return;
    }
    int count = 0;
    //生成密钥
    if (mode == 1) {
        zucStrongKey_base = randKey();
        zucWeakKey_base = randKey();
        zuciv_base = randKey();
    }
    Base64 base64 = Base64();
    base64.Decode(zucStrongKey_base.c_str(), zucStrongKey);
    base64.Decode(zucWeakKey_base.c_str(), zucWeakKey);
    base64.Decode(zuciv_base.c_str(), zuciv);

    while (av_read_frame(ic, pkt) >= 0) {
        if (pkt->stream_index == videoidx) {
            av_decode_encode_frame(pCodecCtx, poutCodecCtx, ic, oc, pkt, frame, watermark, mode, count);
//            cout << "__________" << endl;
//            cout << "pkt flag " << pkt->flags << endl;
//            if (pkt->flags == AV_PKT_FLAG_KEY) { //关键帧 取hash
//                cout << "KEY_PKT" << endl;
//                cout << pkt->size << endl;
//                for (int i = 0; i < 40; ++i) {
//                    cout << (long long)pkt->data[i] << " ";
//                }
//                cout << crypto.getHashBySM3(pkt->data, dgst, pkt->size) << endl;
//                for (int i = 0; i < 32; ++i) {
//                    printf("%02x ", dgst[i]);
//                    fileHash[i] ^= dgst[i];
//                }
//                cout << endl;
//            }
//            av_write_frame(oc, pkt);  //todo
            continue;
        } else if (pkt->stream_index == audioidx) {
            av_write_frame(oc, pkt);
            continue;
        }
        av_packet_unref(pkt);
        av_packet_free(&pkt);
    }
    av_write_trailer(oc);
    avcodec_free_context(&pCodecCtx);
    avcodec_free_context(&poutCodecCtx);
}

void close_ffmpeg(AVFormatContext *&ic, AVFormatContext *oc) {
    avformat_close_input(&ic);
    avformat_free_context(oc);
}


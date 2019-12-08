//
// Created by daijun on 2019-12-08.
//

#ifndef IJKPLAYER_DJ_RECORD_H
#define IJKPLAYER_DJ_RECORD_H

#include <pthread.h>
#include <android/log.h>
#include <vector>
#include <string>
#include <android/native_window.h>
#include "unistd.h"
extern "C"{
//#include "ff_ffplay_def.h"
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include "libswscale/swscale.h"
#include <libavutil/imgutils.h>
#include "libswresample/swresample.h"
#include "libavutil/timestamp.h"
#include "libavutil/mathematics.h"
#include "libavutil/opt.h"
#include "libavutil/avassert.h"
//封装格式处理
#include "libavformat/avformat.h"
//像素处理
#include "libswscale/swscale.h"
}

#define STREAM_DURATION   10.0
#define STREAM_FRAME_RATE 25 /* 25 images/s */
#define STREAM_PIX_FMT    AV_PIX_FMT_YUV420P /* default pix_fmt */
#define SCALE_FLAGS SWS_BICUBIC
#define AVFMT_RAWPICTURE    0x0020  //ffmpeg源码里缺失

//自定义宏
#define DX_FRAME_TYPE_VIDEO  0
#define DX_FRAME_TYPE_AUDIO 1

using std::vector;

typedef struct OutputStream {
    AVStream *st;
    /* pts of the next frame that will be generated */
    int64_t next_pts;
    int samples_count;
    AVFrame *frame;
    AVFrame *tmp_frame;
    float t, tincr, tincr2;
    struct SwsContext *sws_ctx;
    struct SwrContext *swr_ctx;
} OutputStream;

typedef struct InputSourceInfo{
    int width, height;
}InputSourceInfo;


//存储解码后的音频/视频数据
typedef struct DX_FrameData{
    uint8_t * data0;
    uint8_t * data1;
    int lineSize0;
    int lineSize1;
    //指示有几个data
    int dataNum;
    //帧数据类型0-视频 1-音频
    int frameType;
    //音频number of audio samples (per channel)
    int nb_samples;
    uint64_t channel_layout;
    int channels;
}DX_FrameData;

//录像相关信息
typedef struct DX_RecordRelateData{
    //示例代码中编码相关
    //输出上下文
    OutputStream video_st = { 0 }, audio_st = { 0 };
    AVFormatContext *oc;
    AVOutputFormat *fmt;
    AVCodec *audio_codec, *video_codec;

    // 录像文件名
    const char *fileName;
    //输入流格式
    InputSourceInfo srcFormat;
    // 保存的解码帧
    vector<DX_FrameData> recordDataFrames;

}DX_RecordRelateData;


void add_stream(OutputStream *ost, AVFormatContext *oc,
                AVCodec **codec,
                enum AVCodecID codec_id,InputSourceInfo inputSrcInfo);
void open_video(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg);
void open_audio(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg);
AVFrame *alloc_picture(enum AVPixelFormat pix_fmt, int width, int height);
AVFrame *alloc_audio_frame(enum AVSampleFormat sample_fmt,
                           uint64_t channel_layout,
                           int sample_rate, int nb_samples);
int write_video_frame(AVFormatContext *oc, OutputStream *ost,AVFrame * curFr);
int write_frame(AVFormatContext *fmt_ctx, const AVRational *time_base, AVStream *st, AVPacket *pkt);
void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt);
int write_audio_frame(AVFormatContext *oc, OutputStream *ost,AVFrame * curFr);
void close_stream(AVFormatContext *oc, OutputStream *ost);
//执行录像文件
void* doRecordFile(void *infoData);
void free_record_frames(DX_RecordRelateData* recData);
void fill_yuv_image(AVFrame *pict, int frame_index, int width, int height);
void test_func();

#endif //IJKPLAYER_DJ_RECORD_H
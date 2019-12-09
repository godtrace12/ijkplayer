//
// Created by daijun on 2019-12-08.
//
#include "dj_record.h"

#define LOGD(FORMAT,...) __android_log_print(ANDROID_LOG_INFO,"dj_record",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR,"dj_record",FORMAT,##__VA_ARGS__);

void add_stream(OutputStream *ost, AVFormatContext *oc, AVCodec **codec,
                enum AVCodecID codec_id,InputSourceInfo inpSrcInfo) {
    AVCodecContext *c;
    int i;
    //find the encoder

    *codec = avcodec_find_encoder(codec_id);
    if (!(*codec)) {
        fprintf(stderr, "Could not find encoder for '%s'\n",
                avcodec_get_name(codec_id));
        exit(1);
    }
    ost->st = avformat_new_stream(oc, *codec);
    if (!ost->st) {
        fprintf(stderr, "Could not allocate stream\n");
        exit(1);
    }
    ost->st->id = oc->nb_streams-1;
    c = ost->st->codec;
    switch ((*codec)->type) {
        case AVMEDIA_TYPE_AUDIO:
            c->sample_fmt  = (*codec)->sample_fmts ?
                             (*codec)->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
            c->bit_rate    = 64000;
            c->sample_rate = 44100;
            if ((*codec)->supported_samplerates) {
                c->sample_rate = (*codec)->supported_samplerates[0];
                for (i = 0; (*codec)->supported_samplerates[i]; i++) {
                    if ((*codec)->supported_samplerates[i] == 44100)
                        c->sample_rate = 44100;
                }
            }
            c->channels        = av_get_channel_layout_nb_channels(c->channel_layout);
            c->channel_layout = AV_CH_LAYOUT_STEREO;
            if ((*codec)->channel_layouts) {
                c->channel_layout = (*codec)->channel_layouts[0];
                for (i = 0; (*codec)->channel_layouts[i]; i++) {
                    if ((*codec)->channel_layouts[i] == AV_CH_LAYOUT_STEREO)
                        c->channel_layout = AV_CH_LAYOUT_STEREO;
                }
            }
            c->channels        = av_get_channel_layout_nb_channels(c->channel_layout);
            ost->st->time_base = (AVRational){ 1, c->sample_rate };
            break;
        case AVMEDIA_TYPE_VIDEO:
            c->codec_id = codec_id;
            c->bit_rate = 400000;
            //Resolution must be a multiple of two.

            c->width    = inpSrcInfo.width;
            c->height   = inpSrcInfo.height;
// timebase: This is the fundamental unit of time (in seconds) in terms
//             of which frame timestamps are represented. For fixed-fps content,
//             timebase should be 1/framerate and timestamp increments should be
//             identical to 1.

            ost->st->time_base = (AVRational){ 1, STREAM_FRAME_RATE };
            c->time_base       = ost->st->time_base;
            c->gop_size      = 12;
// emit one intra frame every twelve frames at most

            c->pix_fmt       = STREAM_PIX_FMT;
            if (c->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
// just for testing, we also add B frames

                c->max_b_frames = 2;
            }
            if (c->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
// Needed to avoid using macroblocks in which some coeffs overflow.
//                 This does not happen with normal video, it just happens here as
//                 the motion of the chroma plane does not match the luma plane.

                c->mb_decision = 2;
            }
            break;
        default:
            break;
    }
// Some formats want stream headers to be separate.

    if (oc->oformat->flags & AVFMT_GLOBALHEADER)
        c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
}

void open_video(AVFormatContext *oc, AVCodec *codec, OutputStream *ost,
                AVDictionary *opt_arg) {
    int ret;
    AVCodecContext *c = ost->st->codec;
    AVDictionary *opt = NULL;
    av_dict_copy(&opt, opt_arg, 0);
// open the codec

    ret = avcodec_open2(c, codec, &opt);
    av_dict_free(&opt);
    if (ret < 0) {
        fprintf(stderr, "Could not open video codec: %s\n", av_err2str(ret));
        LOGE("dj Could not open video codec:");
        exit(1);
    }
// allocate and init a re-usable frame

    ost->frame = dx_alloc_picture(c->pix_fmt, c->width, c->height);
    if (!ost->frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
// If the output format is not YUV420P, then a temporary YUV420P
//     picture is needed too. It is then converted to the required
//     output format.

    ost->tmp_frame = NULL;
    if (c->pix_fmt != AV_PIX_FMT_YUV420P) {
        ost->tmp_frame = dx_alloc_picture(AV_PIX_FMT_YUV420P, c->width, c->height);
        if (!ost->tmp_frame) {
            fprintf(stderr, "Could not allocate temporary picture\n");
            exit(1);
        }
    }
}


void open_audio(AVFormatContext *oc, AVCodec *codec, OutputStream *ost,
                AVDictionary *opt_arg) {
    AVCodecContext *c;
    int nb_samples;
    int ret;
    AVDictionary *opt = NULL;
    c = ost->st->codec;
// open it

    av_dict_copy(&opt, opt_arg, 0);
    ret = avcodec_open2(c, codec, &opt);
    av_dict_free(&opt);
    if (ret < 0) {
        fprintf(stderr, "Could not open audio codec: %s\n", av_err2str(ret));
        LOGE("Could not open audio codec:");
        exit(1);
    }
// init signal generator

    ost->t     = 0;
    ost->tincr = 2 * M_PI * 110.0 / c->sample_rate;
// increment frequency by 110 Hz per second

    ost->tincr2 = 2 * M_PI * 110.0 / c->sample_rate / c->sample_rate;
    if (c->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)
        nb_samples = 10000;
    else
        nb_samples = c->frame_size;
    ost->frame     = alloc_audio_frame(c->sample_fmt, c->channel_layout,
                                       c->sample_rate, nb_samples);
    ost->tmp_frame = alloc_audio_frame(AV_SAMPLE_FMT_FLTP, c->channel_layout,
                                       c->sample_rate, nb_samples);
// create resampler context

    ost->swr_ctx = swr_alloc();
    if (!ost->swr_ctx) {
        fprintf(stderr, "Could not allocate resampler context\n");
        exit(1);
    }
// set options

    av_opt_set_int       (ost->swr_ctx, "in_channel_count",   c->channels,       0);
    av_opt_set_int       (ost->swr_ctx, "in_sample_rate",     c->sample_rate,    0);
    av_opt_set_sample_fmt(ost->swr_ctx, "in_sample_fmt",      AV_SAMPLE_FMT_S16, 0);
    av_opt_set_int       (ost->swr_ctx, "out_channel_count",  c->channels,       0);
    av_opt_set_int       (ost->swr_ctx, "out_sample_rate",    c->sample_rate,    0);
    av_opt_set_sample_fmt(ost->swr_ctx, "out_sample_fmt",     c->sample_fmt,     0);
// initialize the resampling context

    if ((ret = swr_init(ost->swr_ctx)) < 0) {
        fprintf(stderr, "Failed to initialize the resampling context\n");
        exit(1);
    }
}

AVFrame* dx_alloc_picture(enum AVPixelFormat pix_fmt, int width, int height) {
    AVFrame *picture;
    int ret;
    picture = av_frame_alloc();
    if (!picture)
        return NULL;
    picture->format = pix_fmt;
    picture->width  = width;
    picture->height = height;
// allocate the buffers for the frame data

    ret = av_frame_get_buffer(picture, 32);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate frame data.\n");
        exit(1);
    }
    return picture;
}

AVFrame* alloc_audio_frame(enum AVSampleFormat sample_fmt, uint64_t channel_layout,
                           int sample_rate, int nb_samples) {
    AVFrame *frame = av_frame_alloc();
    int ret;
    if (!frame) {
        fprintf(stderr, "Error allocating an audio frame\n");
        exit(1);
    }
    frame->format = sample_fmt;
    frame->channel_layout = channel_layout;
    frame->sample_rate = sample_rate;
    frame->nb_samples = nb_samples;
    if (nb_samples) {
        ret = av_frame_get_buffer(frame, 0);
        if (ret < 0) {
            fprintf(stderr, "Error allocating an audio buffer\n");
            exit(1);
        }
    }
    return frame;
}


int write_video_frame(AVFormatContext *oc, OutputStream *ost,AVFrame * curFr) {
    int ret;
    AVCodecContext *c;
    AVFrame *frame;
    int got_packet = 0;
    c = ost->st->codec;
//    frame = get_video_frame(ost);
    frame = curFr;

    if (oc->oformat->flags & AVFMT_RAWPICTURE) {
// a hack to avoid data copy with some raw video muxers

        AVPacket pkt;
        av_init_packet(&pkt);
        if (!frame)
            return 1;
        pkt.flags        |= AV_PKT_FLAG_KEY;
        pkt.stream_index  = ost->st->index;
        pkt.data          = (uint8_t *)frame;
        pkt.size          = sizeof(AVPicture);
        pkt.pts = pkt.dts = frame->pts;
        av_packet_rescale_ts(&pkt, c->time_base, ost->st->time_base);
        ret = av_interleaved_write_frame(oc, &pkt);
    } else {
        AVPacket pkt = { 0 };
        av_init_packet(&pkt);
// encode the image

        ret = avcodec_encode_video2(c, &pkt, frame, &got_packet);
        if (ret < 0) {
            fprintf(stderr, "Error encoding video frame: %s\n", av_err2str(ret));
            exit(1);
        }
        if (got_packet) {
            ret = write_frame(oc, &c->time_base, ost->st, &pkt);
        } else {
            ret = 0;
        }
    }
    if (ret < 0) {
        fprintf(stderr, "Error while writing video frame: %s\n", av_err2str(ret));
        exit(1);
    }
    return (frame || got_packet) ? 0 : 1;
}


int write_frame(AVFormatContext *fmt_ctx, const AVRational *time_base, AVStream *st,
                AVPacket *pkt) {
// rescale output packet timestamp values from codec to stream timebase

    av_packet_rescale_ts(pkt, *time_base, st->time_base);
    pkt->stream_index = st->index;
// Write the compressed frame to the media file.

    log_packet(fmt_ctx, pkt);
    return av_interleaved_write_frame(fmt_ctx, pkt);
}

void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt) {
    AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;
    printf("pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
           av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
           av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
           av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
           pkt->stream_index);
}

// 实际编码时需要AV_SAMPLE_FMT_FLTP的音频采样数据，而目前的音频解码后的数据已经是AV_SAMPLE_FMT_FLTP，
// 所以就不需要再进行音频格式转换。
int write_audio_frame(AVFormatContext *oc, OutputStream *ost,AVFrame * curFr) {
    AVCodecContext *c;
    AVPacket pkt = { 0 }; // data and size must be 0;
    AVFrame *frame;
    int ret;
    int got_packet;
    int dst_nb_samples;
    av_init_packet(&pkt);
    c = ost->st->codec;
//    frame = get_audio_frame(ost);
    //换成从外部传入数据已处理好的音频帧
    frame = curFr;
    dst_nb_samples = curFr->nb_samples;
    if (frame) {
// convert samples from native format to destination codec format, using the resampler

// compute destination number of samples

//        dst_nb_samples = av_rescale_rnd(swr_get_delay(ost->swr_ctx, c->sample_rate) + frame->nb_samples,
//                                        c->sample_rate, c->sample_rate, AV_ROUND_UP);
//        av_assert0(dst_nb_samples == frame->nb_samples);
// when we pass a frame to the encoder, it may keep a reference to it
//         internally;
//         make sure we do not overwrite it here


//        ret = av_frame_make_writable(ost->frame);
//        if (ret < 0)
//            exit(1);
// convert to destination format

//        ret = swr_convert(ost->swr_ctx,
//                          ost->frame->data, dst_nb_samples,
//                          (const uint8_t **)frame->data, frame->nb_samples);
//        if (ret < 0) {
//            fprintf(stderr, "Error while converting\n");
//            exit(1);
//        }
//        frame = ost->frame;
        frame->pts = av_rescale_q(ost->samples_count, (AVRational){1, c->sample_rate}, c->time_base);
        ost->samples_count += dst_nb_samples;
    }
    ret = avcodec_encode_audio2(c, &pkt, frame, &got_packet);
    if (ret < 0) {
        fprintf(stderr, "Error encoding audio frame: %s\n", av_err2str(ret));
        exit(1);
    }
    if (got_packet) {
        ret = write_frame(oc, &c->time_base, ost->st, &pkt);
        if (ret < 0) {
            fprintf(stderr, "Error while writing audio frame: %s\n",
                    av_err2str(ret));
            exit(1);
        }
    }
    return (frame || got_packet) ? 0 : 1;
}

void close_stream(AVFormatContext *oc, OutputStream *ost) {
    avcodec_close(ost->st->codec);
    av_frame_free(&ost->frame);
    av_frame_free(&ost->tmp_frame);
    sws_freeContext(ost->sws_ctx);
    swr_free(&ost->swr_ctx);
}

//void* doRecordFile(void *infoData){
//
////    usleep(1000);
//    sleep(1);
//    DX_RecordRelateData *recordRelateDataPtr = (DX_RecordRelateData*)(infoData);
//
//
//    LOGE("线程中执行C函数执行停止执行录制 %s",recordRelateDataPtr->fileName);
////    free_record_frames();
//
//    LOGE("已录制Frame%d",recordRelateDataPtr->windex)
//
//    const char *fileName = recordRelateDataPtr->fileName;
//    //获取保存到的输入上下文
//    //输出上下文
//    AVFormatContext *oc;
//    AVOutputFormat *fmt;    //临时变量
//    OutputStream* videoStPtr;
//    OutputStream* audioStPtr;
////    AVCodec *audio_codec, *video_codec;
//    int ret;
//    int have_video = 0, have_audio = 0;
//    int encode_video = 0, encode_audio = 0;
//    AVDictionary *opt = NULL;
////    av_dict_set(&opt,"profile","baseline",AV_DICT_MATCH_CASE);
//
//    av_dict_set(&opt, "preset", "veryfast", 0); // av_opt_set(pCodecCtx->priv_data,"preset","fast",0);
//    av_dict_set(&opt, "tune", "zerolatency", 0);
//
//    LOGE("线程中avformat_alloc_output_context2");
//    avformat_alloc_output_context2(&(recordRelateDataPtr->oc),NULL,NULL,fileName);
//    oc = recordRelateDataPtr->oc;
//    if (!oc){
//        LOGD("startRecord avformat_alloc_output_context2 fail");
//        avformat_alloc_output_context2(&oc, NULL, "mp4", fileName);
//    }
//    if (!oc){
//        LOGE("avformat_alloc_output_context2 失败");
//        return NULL;
//    }else{
//        LOGE("avformat_alloc_output_context2 成功");
//    }
//    fmt = recordRelateDataPtr->oc->oformat;
//    //Add the audio and video streams using the default format codecs
//    // and initialize the codecs.
//    LOGE("线程中add_stream");
//    if (fmt->video_codec != AV_CODEC_ID_NONE) {
//        add_stream(&(recordRelateDataPtr->video_st), recordRelateDataPtr->oc, &(recordRelateDataPtr->video_codec), fmt->video_codec,recordRelateDataPtr->srcFormat);
//        have_video = 1;
//        encode_video = 1;
//    }
//    if (fmt->audio_codec != AV_CODEC_ID_NONE) {
//        add_stream(&(recordRelateDataPtr->audio_st), recordRelateDataPtr->oc, &recordRelateDataPtr->audio_codec, fmt->audio_codec,recordRelateDataPtr->srcFormat);
//        have_audio = 1;
//        encode_audio = 1;
//    }
//    LOGE("线程中 open video");
//    videoStPtr = &recordRelateDataPtr->video_st;
//    audioStPtr = &recordRelateDataPtr->audio_st;
//    if (have_video)
//        open_video(recordRelateDataPtr->oc, recordRelateDataPtr->video_codec, &recordRelateDataPtr->video_st, opt);
//    if (have_audio)
//        open_audio(recordRelateDataPtr->oc, recordRelateDataPtr->audio_codec, &recordRelateDataPtr->audio_st, opt);
//    av_dump_format(recordRelateDataPtr->oc, 0, fileName, 1);
//    //open the output file, if needed
//    LOGE("线程中 avio_open");
//    if (!(fmt->flags & AVFMT_NOFILE)) {
//        ret = avio_open(&oc->pb, fileName, AVIO_FLAG_WRITE);
//        if (ret < 0) {
//            LOGE("Could not open %s",fileName);
//            LOGE("dj Could not open %s",fileName);
//            return NULL;
//        }
//    }
//    //Write the stream header, if any.
//    LOGE("线程中 avformat_write_header");
//
//    ret = avformat_write_header(oc, &opt);
//    if (ret < 0) {
//        LOGE("Error occurred when opening output file %s",av_err2str(ret));
//        return NULL;
//    }
//    InputSourceInfo inSrcInfo = recordRelateDataPtr->srcFormat;
//    //v3 根据所有解码后的帧顺序(包括视频帧和音频帧来编码)
//    int frNum = recordRelateDataPtr->windex;
//    LOGE("总共需编码 %d 帧",frNum);
//    SwsContext *swsContext = sws_getContext(
//            inSrcInfo.width   //原图片的宽
//            ,inSrcInfo.height  //源图高
//            ,AV_PIX_FMT_RGBA //源图片format
//            ,inSrcInfo.width  //目标图的宽
//            ,inSrcInfo.height  //目标图的高
//            ,AV_PIX_FMT_YUV420P,SWS_FAST_BILINEAR
//            , NULL, NULL, NULL
//    );
//    LOGE("线程中 开始音视频编码");
//    for(int i=0;i<recordRelateDataPtr->windex;i++){
//        DX_FrameData frData = recordRelateDataPtr->recordFramesQueue[i];
////        LOGE("线程中 拿到一解码帧数据");
//        // 视频帧
//        if(frData.frameType == DX_FRAME_TYPE_VIDEO){
//            //对输出图像进行色彩，分辨率缩放，滤波处理
//            uint8_t *srcSlice[3];
//            srcSlice[0] = frData.data0;
//            srcSlice[1] = NULL;
//            srcSlice[2] = NULL;
//            int srcLineSize[3];
//            srcLineSize[0] = frData.lineSize0;
//            //yuv数据每个分量一个字节，一行大小等于宽度
//            int dstLineSize[3];
//            dstLineSize[0] = inSrcInfo.width;
//            dstLineSize[1] = inSrcInfo.width;
//            dstLineSize[2] = inSrcInfo.width;
//            sws_scale(swsContext, (const uint8_t *const *) srcSlice, srcLineSize, 0,
//                      inSrcInfo.height, recordRelateDataPtr->video_st.frame->data, recordRelateDataPtr->video_st.frame->linesize);
//            recordRelateDataPtr->video_st.frame->pts = recordRelateDataPtr->video_st.next_pts++;
////            fill_yuv_image(recordRelateDataPtr->video_st.frame,i,inSrcInfo.width,inSrcInfo.height);
//
//            LOGE("线程中 开始处理一帧视频数据");
//            write_video_frame(oc,&recordRelateDataPtr->video_st,recordRelateDataPtr->video_st.frame);
//            LOGE("线程中 完成一帧数据编码写入");
//        }else {     //音频帧
//            //avcodec_decode_audio4  解码出来的音频数据是 AV_SAMPLE_FMT_FLTP,所以数据在data0 data1中
////            LOGE("开始编码音频帧 nb_samples %d  channels %d  channel_layout %d",frData.nb_samples,frData.channels,frData.channel_layout);
//
//            AVFrame* tmpFr = audioStPtr->tmp_frame;
//            tmpFr->data[0] = (uint8_t *)av_malloc(frData.lineSize0);
//            tmpFr->data[1] = (uint8_t *)av_malloc(frData.lineSize1);
//            memcpy(tmpFr->data[0],frData.data0,frData.lineSize0);
//            memcpy(tmpFr->data[1],frData.data1,frData.lineSize1);
//            tmpFr->nb_samples = frData.nb_samples;
//            tmpFr->channels = frData.channels;
//            tmpFr->channel_layout = frData.channel_layout;
//            tmpFr->pts = audioStPtr->next_pts;
//            audioStPtr->next_pts += tmpFr->nb_samples;
////            LOGE("完成单帧音频编码数据拷贝");
////            LOGE("线程中 开始写视频帧");
//            write_audio_frame(oc,audioStPtr,tmpFr);
//
//        }
//    }
//    LOGE("线程中 音视频编码完毕");
//    free_record_frames(recordRelateDataPtr);
//
//
//    //Write the trailer, if any. The trailer must be written before you
//    //close the CodecContexts open when you wrote the header; otherwise
//    //av_write_trailer() may try to use memory that was freed on
//    //av_codec_close().
//
//    av_write_trailer(oc);
//    //Close each codec.
//
//    if (have_video)
//        close_stream(oc, videoStPtr);
//    if (have_audio)
//        close_stream(oc, audioStPtr);
//    if (!(fmt->flags & AVFMT_NOFILE))
//        //Close the output file.
//
//        avio_closep(&oc->pb);
//    //free the stream
//
//    avformat_free_context(oc);
//    return NULL;
//}

// 方式2  直接使用解码后的yuv420p数据
void* doRecordFile(void *infoData){

//    usleep(1000);
    sleep(1);
    DX_RecordRelateData *recordRelateDataPtr = (DX_RecordRelateData*)(infoData);


    LOGE("线程中执行C函数执行停止执行录制 %s",recordRelateDataPtr->fileName);
//    free_record_frames();

    LOGE("已录制Frame%d",recordRelateDataPtr->windex)


    const char *fileName = recordRelateDataPtr->fileName;
    //获取保存到的输入上下文
    //输出上下文
    AVFormatContext *oc;
    AVOutputFormat *fmt;    //临时变量
    OutputStream* videoStPtr;
    OutputStream* audioStPtr;
//    AVCodec *audio_codec, *video_codec;
    int ret;
    int have_video = 0, have_audio = 0;
    int encode_video = 0, encode_audio = 0;
    AVDictionary *opt = NULL;
//    av_dict_set(&opt,"profile","baseline",AV_DICT_MATCH_CASE);

    av_dict_set(&opt, "preset", "veryfast", 0); // av_opt_set(pCodecCtx->priv_data,"preset","fast",0);
    av_dict_set(&opt, "tune", "zerolatency", 0);

    LOGE("线程中avformat_alloc_output_context2");
    avformat_alloc_output_context2(&(recordRelateDataPtr->oc),NULL,NULL,fileName);
    oc = recordRelateDataPtr->oc;
    if (!oc){
        LOGD("startRecord avformat_alloc_output_context2 fail");
        avformat_alloc_output_context2(&oc, NULL, "mp4", fileName);
    }
    if (!oc){
        LOGE("avformat_alloc_output_context2 失败");
        return NULL;
    }else{
        LOGE("avformat_alloc_output_context2 成功");
    }
    fmt = recordRelateDataPtr->oc->oformat;
    //Add the audio and video streams using the default format codecs
    // and initialize the codecs.
    LOGE("线程中add_stream");
    if (fmt->video_codec != AV_CODEC_ID_NONE) {
        add_stream(&(recordRelateDataPtr->video_st), recordRelateDataPtr->oc, &(recordRelateDataPtr->video_codec), fmt->video_codec,recordRelateDataPtr->srcFormat);
        have_video = 1;
        encode_video = 1;
    }
    if (fmt->audio_codec != AV_CODEC_ID_NONE) {
        add_stream(&(recordRelateDataPtr->audio_st), recordRelateDataPtr->oc, &recordRelateDataPtr->audio_codec, fmt->audio_codec,recordRelateDataPtr->srcFormat);
        have_audio = 1;
        encode_audio = 1;
    }
    LOGE("线程中 open video");
    videoStPtr = &recordRelateDataPtr->video_st;
    audioStPtr = &recordRelateDataPtr->audio_st;
    if (have_video)
        open_video(recordRelateDataPtr->oc, recordRelateDataPtr->video_codec, &recordRelateDataPtr->video_st, opt);
    if (have_audio)
        open_audio(recordRelateDataPtr->oc, recordRelateDataPtr->audio_codec, &recordRelateDataPtr->audio_st, opt);
    av_dump_format(recordRelateDataPtr->oc, 0, fileName, 1);
    //open the output file, if needed
    LOGE("线程中 avio_open");
    if (!(fmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&oc->pb, fileName, AVIO_FLAG_WRITE);
        if (ret < 0) {
            LOGE("Could not open %s",fileName);
            LOGE("dj Could not open %s",fileName);
            return NULL;
        }
    }
    //Write the stream header, if any.
    LOGE("线程中 avformat_write_header");

    ret = avformat_write_header(oc, &opt);
    if (ret < 0) {
        LOGE("Error occurred when opening output file %s",av_err2str(ret));
        return NULL;
    }
    InputSourceInfo inSrcInfo = recordRelateDataPtr->srcFormat;
    //v3 根据所有解码后的帧顺序(包括视频帧和音频帧来编码)
    int frNum = recordRelateDataPtr->windex;
    LOGE("总共需编码 %d 帧",frNum);
    struct SwsContext *swsContext = sws_getContext(
            inSrcInfo.width   //原图片的宽
            ,inSrcInfo.height  //源图高
            ,AV_PIX_FMT_YUV420P //源图片format
            ,inSrcInfo.width  //目标图的宽
            ,inSrcInfo.height  //目标图的高
            ,AV_PIX_FMT_YUV420P,SWS_FAST_BILINEAR
            , NULL, NULL, NULL
    );
    LOGE("线程中 开始音视频编码");
//    for(int i=0;i<recordRelateDataPtr->recordDataFrames.size();i++){  // method 1
    for(int i=0;i<recordRelateDataPtr->windex;i++){        //method 2
//            DX_FrameData frData = recordRelateDataPtr->recordDataFrames[i]; // method 1
        DX_FrameData frData = recordRelateDataPtr->recordFramesQueue[i];    //method 2
        //        LOGE("线程中 拿到一解码帧数据");
        // 视频帧
        if(frData.frameType == DX_FRAME_TYPE_VIDEO){
            //对输出图像进行色彩，分辨率缩放，滤波处理
            uint8_t *srcSlice[3];
            srcSlice[0] = frData.data0;
            srcSlice[1] = frData.data1;
            srcSlice[2] = frData.data2;
            int srcLineSize[3];
            srcLineSize[0] = frData.lineSize0;
            srcLineSize[1] = frData.lineSize1;
            srcLineSize[2] = frData.lineSize2;
            //yuv数据每个分量一个字节，一行大小等于宽度
            int dstLineSize[3];
            dstLineSize[0] = inSrcInfo.width;
            dstLineSize[1] = inSrcInfo.width/2;
            dstLineSize[2] = inSrcInfo.width/2;
            sws_scale(swsContext, (const uint8_t *const *) srcSlice, srcLineSize, 0,
                      inSrcInfo.height, recordRelateDataPtr->video_st.frame->data, recordRelateDataPtr->video_st.frame->linesize);
            recordRelateDataPtr->video_st.frame->pts = recordRelateDataPtr->video_st.next_pts++;
            //            fill_yuv_image(recordRelateDataPtr->video_st.frame,i,inSrcInfo.width,inSrcInfo.height);

            LOGE("线程中 开始处理一帧视频数据");
            write_video_frame(oc,&recordRelateDataPtr->video_st,recordRelateDataPtr->video_st.frame);
            LOGE("线程中 完成一帧数据编码写入");
        }else {     //音频帧
            //avcodec_decode_audio4  解码出来的音频数据是 AV_SAMPLE_FMT_FLTP,所以数据在data0 data1中
            //            LOGE("开始编码音频帧 nb_samples %d  channels %d  channel_layout %d",frData.nb_samples,frData.channels,frData.channel_layout);

            AVFrame* tmpFr = audioStPtr->tmp_frame;
            tmpFr->data[0] = (uint8_t *)av_malloc(frData.lineSize0);
            tmpFr->data[1] = (uint8_t *)av_malloc(frData.lineSize1);
            memcpy(tmpFr->data[0],frData.data0,frData.lineSize0);
            memcpy(tmpFr->data[1],frData.data1,frData.lineSize1);
            tmpFr->nb_samples = frData.nb_samples;
            tmpFr->channels = frData.channels;
            tmpFr->channel_layout = frData.channel_layout;
            tmpFr->pts = audioStPtr->next_pts;
            audioStPtr->next_pts += tmpFr->nb_samples;
            //            LOGE("完成单帧音频编码数据拷贝");
            //            LOGE("线程中 开始写视频帧");
            write_audio_frame(oc,audioStPtr,tmpFr);

        }
    }
    LOGE("线程中 音视频编码完毕");
    free_record_frames(recordRelateDataPtr);


    //Write the trailer, if any. The trailer must be written before you
    //close the CodecContexts open when you wrote the header; otherwise
    //av_write_trailer() may try to use memory that was freed on
    //av_codec_close().

    av_write_trailer(oc);
    //Close each codec.

    if (have_video)
        close_stream(oc, videoStPtr);
    if (have_audio)
        close_stream(oc, audioStPtr);
    if (!(fmt->flags & AVFMT_NOFILE))
        //Close the output file.

        avio_closep(&oc->pb);
    //free the stream

    avformat_free_context(oc);
    return NULL;
}

void free_record_frames(DX_RecordRelateData* recData) {
    LOGE("C线程中执行录像内存释放");
    LOGE("recordDaraFrameQueue size=%d",recData->windex);
    //使用数组保存时的内存释放
    for(int i= 0;i<recData->windex;i++){
        DX_FrameData dataFrame = recData->recordFramesQueue[i];
        uint8_t *data0 = dataFrame.data0;
        uint8_t *data1 = dataFrame.data1;

        if(dataFrame.dataNum == 1){
            if(data0 != NULL){
                free(data0);
                data0 = NULL;
            }
        }else if(dataFrame.dataNum == 2){
            if (data1 != NULL){
                free(data1);
                data1 = NULL;
            }
        }
    }
    recData->windex = 0;

}


void fill_yuv_image(AVFrame *pict, int frame_index, int width, int height) {
    int x, y, i, ret;
    //when we pass a frame to the encoder, it may keep a reference to it
//      internally;
//      make sure we do not overwrite it here
    ret = av_frame_make_writable(pict);
    if (ret < 0)
        exit(1);
    i = frame_index;
    // Y
    for (y = 0; y < height; y++)
        for (x = 0; x < width; x++)
            pict->data[0][y * pict->linesize[0] + x] = x + y + i * 3;
    // Cb and Cr
    for (y = 0; y < height / 2; y++) {
        for (x = 0; x < width / 2; x++) {
            pict->data[1][y * pict->linesize[1] + x] = 128 + y + i * 2;
            pict->data[2][y * pict->linesize[2] + x] = 64 + x + i * 5;
        }
    }
    //初始化SwsContext

}

void test_func(){
    LOGE("录制测试函数");
}
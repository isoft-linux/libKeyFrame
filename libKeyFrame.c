/*
 * Copyright (C) 2016 Leslie Zhai <xiang.zhai@i-soft.com.cn>
 * Copyright (C) 2016 Cjacker <cjacker@foxmail.com>
 * Copyright (C) 2013 - 2015 A.Greensted <andy@labbookpages.co.uk>
 * Copyright (c) 2012 Stefano Sabatini
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <unistd.h>
#include <libgen.h>
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#ifdef ENABLE_LIBPNG
#include <png.h>
#endif

#include "libKeyFrame.h"
#include "TinyPngOut.h"

static AVFormatContext *fmt_ctx = NULL;
static AVCodecContext *video_dec_ctx = NULL;
static int width, height;
static enum AVPixelFormat pix_fmt;
static AVStream *video_stream = NULL;

static int video_stream_idx = -1;
static AVFrame *frame = NULL;
static AVPacket pkt;
static int video_frame_count = 0;

/* Enable or disable frame reference counting. You are not supposed to support
 * both paths in your application but pick the one most appropriate to your
 * needs. Look for the use of refcount in this example to see what are the
 * differences of API usage between them. */
static int refcount = 0;

static AVFrame *frameRGB = NULL;
static struct SwsContext *sws_ctx = NULL;
static char outputDir[PATH_MAX] = { '\0' };

/*
 * Only for test!
 * raw, linesize, width, height, filename
 */
#if 0
static void pgm_save(unsigned char *buf, int wrap, int xsize, int ysize,           
                     char *filename)                                               
{                                                                                  
    FILE *f = NULL;
    int i;

    f = fopen(filename, "wb");
    if (f) {
        fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);
        for (i = 0; i < ysize; i++)
            fwrite(buf + i * wrap, 1, xsize, f);
        fclose(f);
        f = NULL;
    }
}
#endif

#ifdef ENABLE_LIBPNG
static void png_save(unsigned char *buf, 
                     int wrap, 
                     int xsize, 
                     int ysize, 
                     char *filename) 
{
    int i, j;
	int code = 0;
	FILE *fp = NULL;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_bytep row = NULL;
	
	// Open file for writing (binary mode)
	fp = fopen(filename, "wb");
	if (fp == NULL) {
		fprintf(stderr, "Could not open file %s for writing\n", filename);
		code = 1;
		goto finalise;
	}

	// Initialize write structure
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fprintf(stderr, "Could not allocate write struct\n");
		code = 1;
		goto finalise;
	}

	// Initialize info structure
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fprintf(stderr, "Could not allocate info struct\n");
		code = 1;
		goto finalise;
	}

	// Setup Exception handling
	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr, "Error during png creation\n");
		code = 1;
		goto finalise;
	}

	png_init_io(png_ptr, fp);

	// Write header (8 bit colour depth)
	png_set_IHDR(png_ptr, 
                 info_ptr, 
                 xsize, 
                 ysize, 
                 8, 
                 PNG_COLOR_TYPE_RGB, 
                 PNG_INTERLACE_NONE, 
                 PNG_COMPRESSION_TYPE_BASE, 
                 PNG_FILTER_TYPE_BASE);

	// Set title
	if (filename != NULL) {
		png_text title_text;
		title_text.compression = PNG_TEXT_COMPRESSION_NONE;
		title_text.key = "Title";
		title_text.text = filename;
		png_set_text(png_ptr, info_ptr, &title_text, 1);
	}

	png_write_info(png_ptr, info_ptr);

	// Allocate memory for one row (3 bytes per pixel - RGB)
	row = (png_bytep) malloc(3 * xsize * sizeof(png_byte));

	// Write image data
    // FIXME: HOWTO *set* row
	for (i = 0; i < ysize; i++) {
		for (j = 0; j < xsize; j++) {
            row[j] = buf + (i * xsize + j) * wrap;
        }
        png_write_row(png_ptr, row);
	}

	// End write
	png_write_end(png_ptr, NULL);

finalise:
	if (fp != NULL) fclose(fp);
	if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	if (row != NULL) free(row);
}
#endif

static void png_save2(unsigned char *buf, 
                      int width, 
                      int height, 
                      char *filename) 
{
    FILE *fout = NULL;
    struct TinyPngOut pngout;

    fout = fopen(filename, "wb");
    if (fout == NULL || TinyPngOut_init(&pngout, fout, width, height) != TINYPNGOUT_OK)
        goto cleanup;

    if (TinyPngOut_write(&pngout, buf, width * height) != TINYPNGOUT_OK)
        goto cleanup;

    if (TinyPngOut_write(&pngout, NULL, 0) != TINYPNGOUT_DONE)
        goto cleanup;

cleanup:
    if (fout) {
        fclose(fout);
        fout = NULL;
    }
}

static int decode_packet(int *got_frame, int cached)
{
    int ret = 0;
    int decoded = pkt.size;

    *got_frame = 0;

    if (pkt.stream_index == video_stream_idx) {
        /* decode video frame */
        ret = avcodec_decode_video2(video_dec_ctx, frame, got_frame, &pkt);
        if (ret < 0) {
            fprintf(stderr, "Error decoding video frame (%s)\n", av_err2str(ret));
            return ret;
        }

        if (*got_frame) {
            if (frame->width != width || frame->height != height ||
                frame->format != pix_fmt) {
                /* To handle this change, one could call av_image_alloc again and
                 * decode the following frames into another rawvideo file. */
                fprintf(stderr, "Error: Width, height and pixel format have to be "
                        "constant in a rawvideo file, but the width, height or "
                        "pixel format of the input video changed:\n"
                        "old: width = %d, height = %d, format = %s\n"
                        "new: width = %d, height = %d, format = %s\n",
                        width, height, av_get_pix_fmt_name(pix_fmt),
                        frame->width, frame->height,
                        av_get_pix_fmt_name(frame->format));
                return -1;
            }
#ifdef DEBUG
            printf("video_frame%s n:%d coded_n:%d pts:%s\n",
                   cached ? "(cached)" : "",
                   video_frame_count, frame->coded_picture_number,
                   av_ts2timestr(frame->pts, &video_dec_ctx->time_base));
#endif
            /* This is key frame! */
            if (frame->key_frame == 1) {
                char buf[PATH_MAX] = { '\0' };
                snprintf(buf, sizeof(buf) - 1, "%s/%s%02d.png", 
                         outputDir, 
                         basename(fmt_ctx->filename),
                         video_frame_count++);
                /* Convert the image from its native format to RGB */
                sws_scale(sws_ctx, 
                          (const uint8_t * const*)frame->data, 
                          frame->linesize, 
                          0, 
                          height, 
                          frameRGB->data, 
                          frameRGB->linesize);
                /* Save to png */
                png_save2(frameRGB->data[0], width, height, buf);
            }
        }
    }

    /* If we use frame reference counting, we own the data and need
     * to de-reference it when we don't use it anymore */
    if (*got_frame && refcount)
        av_frame_unref(frame);

    return decoded;
}

static int open_codec_context(int *stream_idx,
                              AVCodecContext **dec_ctx, 
                              AVFormatContext *fmt_ctx, 
                              enum AVMediaType type)
{
    int ret, stream_index;
    AVStream *st;
    AVCodec *dec = NULL;
    AVDictionary *opts = NULL;

    ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not find %s stream in input file '%s'\n",
                av_get_media_type_string(type), fmt_ctx->filename);
        return ret;
    } else {
        stream_index = ret;
        st = fmt_ctx->streams[stream_index];

        /* find decoder for the stream */
#if LIBAVFORMAT_BUILD > AV_VERSION_INT(56, 40, 101)
        dec = avcodec_find_decoder(st->codecpar->codec_id);
#else
        *dec_ctx = st->codec;
        dec = avcodec_find_decoder(st->codec->codec_id);
#endif
        if (!dec) {
            fprintf(stderr, "Failed to find %s codec\n",
                    av_get_media_type_string(type));
            return AVERROR(EINVAL);
        }

#if LIBAVFORMAT_BUILD > AV_VERSION_INT(56, 40, 101)
        /* Allocate a codec context for the decoder */
        *dec_ctx = avcodec_alloc_context3(dec);
        if (!*dec_ctx) {
            fprintf(stderr, "Failed to allocate the %s codec context\n",
                    av_get_media_type_string(type));
            return AVERROR(ENOMEM);
        }

        /* Copy codec parameters from input stream to output codec context */
        if ((ret = avcodec_parameters_to_context(*dec_ctx, st->codecpar)) < 0) {
            fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
                    av_get_media_type_string(type));
            return ret;
        }
#endif

        /* Init the decoders, with or without reference counting */
        av_dict_set(&opts, "refcounted_frames", refcount ? "1" : "0", 0);
        if ((ret = avcodec_open2(*dec_ctx, dec, &opts)) < 0) {
            fprintf(stderr, "Failed to open %s codec\n",
                    av_get_media_type_string(type));
            return ret;
        }
        *stream_idx = stream_index;
    }

    return 0;
}

int findKeyFrame(char *src_filename, char *output_dir)
{
    int ret = 0, got_frame;
    uint8_t *buffer = NULL;

    if (access(output_dir, W_OK) == -1) {
        printf("ERROR: %s is not writable!\n", output_dir);
        return -1;
    }

    strncpy(outputDir, output_dir, PATH_MAX);

    /* register all formats and codecs */
    av_register_all();

    /* open input file, and allocate format context */
    if (avformat_open_input(&fmt_ctx, src_filename, NULL, NULL) < 0) {
        fprintf(stderr, "Could not open source file %s\n", src_filename);
        return -1;
    }

    /* retrieve stream information */
    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        fprintf(stderr, "Could not find stream information\n");
        return -1;
    }

    if (open_codec_context(&video_stream_idx, &video_dec_ctx, fmt_ctx, AVMEDIA_TYPE_VIDEO) >= 0) {
        video_stream = fmt_ctx->streams[video_stream_idx];

        /* allocate image where the decoded image will be put */
        width = video_dec_ctx->width;
        height = video_dec_ctx->height;
#ifdef DEBUG
        printf("DEBUG: %s, line %d: %dx%d\n", __func__, __LINE__, width, height);
#endif
        pix_fmt = video_dec_ctx->pix_fmt;
    }

    /* dump input information to stderr */
    av_dump_format(fmt_ctx, 0, src_filename, 0);

    if (!video_stream) {
        fprintf(stderr, "Could not find video stream in the input, aborting\n");
        ret = 1;
        goto end;
    }

    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate frame\n");
        ret = AVERROR(ENOMEM);
        goto end;
    }

    /*
     * Prepare for RGB frame and sws context
     * when decoding it needs to convert native mode to RGB mode 
     */
    frameRGB = av_frame_alloc();
    if (!frameRGB) {
        fprintf(stderr, "Could not allocate frameRGB\n");
        ret = AVERROR(ENOMEM);
        goto end;
    }
    buffer = malloc(avpicture_get_size(AV_PIX_FMT_RGB24, width, height) * sizeof(uint8_t));
    avpicture_fill((AVPicture *)frameRGB, buffer, AV_PIX_FMT_RGB24, width, height);
    sws_ctx = sws_getContext(width, 
                             height, 
                             pix_fmt, 
                             width, 
                             height, 
                             AV_PIX_FMT_RGB24, 
                             SWS_BILINEAR, 
                             NULL, 
                             NULL, 
                             NULL);

    /* initialize packet, set data to NULL */
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;

    /* read frames from the file */
    while (av_read_frame(fmt_ctx, &pkt) >= 0) {
        AVPacket orig_pkt = pkt;
        do {
            ret = decode_packet(&got_frame, 0);
            if (ret < 0)
                break;
            pkt.data += ret;
            pkt.size -= ret;
        } while (pkt.size > 0);
        av_packet_unref(&orig_pkt);
    }

    /* flush cached frames */
    pkt.data = NULL;
    pkt.size = 0;
    do {
        decode_packet(&got_frame, 1);
    } while (got_frame);

end:
    avcodec_free_context(&video_dec_ctx);
#if LIBAVFORMAT_BUILD >= AV_VERSION_INT(57, 48, 101)
    avformat_close_input(&fmt_ctx);
#endif
    av_frame_free(&frame);
    av_frame_free(&frameRGB);
    sws_ctx = NULL;
    if (buffer) free(buffer); buffer = NULL;

    return ret < 0;
}

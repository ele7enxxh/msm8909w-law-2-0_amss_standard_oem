/******************************************************************************
 *
 *  Copyright (c) 2016 Qualcomm Technologies International, Ltd.
 *  All Rights Reserved.
 *  Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 *
 ******************************************************************************/

#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/time.h>
#include "aptXbtenc.h"
#include "btif_util.h"
#include <android/log.h>
#include "aptXScheduler.h"

#define LOGERROR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__);
#define LOGWARN(...)  __android_log_print(ANDROID_LOG_WARN,  LOG_TAG, __VA_ARGS__);
#define LOGINFO(...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__);
#define LOGDEBUG(...)  __android_log_print(ANDROID_LOG_DEBUG,  LOG_TAG, __VA_ARGS__);

typedef struct _FramingParams {
    unsigned int pcm_reads;
    unsigned int pcm_bytes_per_read;
    unsigned int aptx_bytes;
    unsigned int sleepTimeNs;
    int frame_size_ctr;
} FramingParams;

typedef struct {
    ReadFn read_fn;
    BufferSendFn send_fn;
    Set_priority_fn set_priority_fn;
    AptX_codec_type aptX_codec_type;
    UINT16 sample_rate;
    UINT8 format_bits;
    UINT8 channel;
    BOOLEAN use_SCMS_T;
    BOOLEAN test;
    BOOLEAN trace;
    BOOLEAN stop;
    BOOLEAN is_aptx_started;
    UINT16 pcmBuffer[1024];
    void* encoder;
    pthread_mutex_t wait_mtx;
    pthread_cond_t wait_cnd;
} tAPTX_SCHEDULER_PARAMS;

static tAPTX_SCHEDULER_PARAMS params;

#define LOG_TAG "aptx_sched"
#define MAXPACKET 924
#define BT_APTX_VERSION "1.0.1"

static const char *APTX_ENC_LIB_NAME = "libaptX-1.0.0-rel-Android21-ARMv7A.so";
static void *aptXEncLibHandle = NULL;
int (*aptxenc_init)(void * state, short endian);
int (*aptxenc_encodestereo)(void * state, void* pcmL, void* pcmR, void* buffer);
int (*aptxenc_sizeofaptxbtenc)(void);

static void init_framing (FramingParams* framingParams)
{
    framingParams->pcm_bytes_per_read = 0;
    framingParams->pcm_reads = 0;
    framingParams->aptx_bytes = 0;
    framingParams->frame_size_ctr = 0;

    if (params.aptX_codec_type == APTX_CODEC) {
        if (params.sample_rate == 48000) {
            if (params.use_SCMS_T) {
                framingParams->sleepTimeNs = 13750000;
            } else {
                framingParams->sleepTimeNs = 14000000;
            }
        } else {
            if (params.use_SCMS_T) {
                framingParams->sleepTimeNs = 14000000;
            } else {
                framingParams->sleepTimeNs = 15000000;
            }
        }
    } else {
        LOGERROR ("invalid codec: %d\n", params.aptX_codec_type);
    }

}

static void update_framing (FramingParams* framingParams)
{

    if (params.aptX_codec_type == APTX_CODEC) {
        if (params.sample_rate == 48000) {
            if (params.use_SCMS_T) {
                framingParams->aptx_bytes = 660;
                framingParams->pcm_bytes_per_read = 240;
                framingParams->pcm_reads = 11;
            } else {
                framingParams->aptx_bytes = 672;
                framingParams->pcm_bytes_per_read = 224;
                framingParams->pcm_reads = 12;
            }
        } else {
            if (params.use_SCMS_T) {
                if (++framingParams->frame_size_ctr < 20) {
                    framingParams->aptx_bytes = 616;
                    framingParams->pcm_bytes_per_read = 224;
                    framingParams->pcm_reads = 11;
                } else {
                    framingParams->aptx_bytes = 644;
                    framingParams->pcm_bytes_per_read = 368;
                    framingParams->pcm_reads = 7;
                    framingParams->frame_size_ctr = 0;
                }
            } else {
                if (++framingParams->frame_size_ctr < 8) {
                    framingParams->aptx_bytes = 660;
                    framingParams->pcm_bytes_per_read = 240;
                    framingParams->pcm_reads = 11;
                } else {
                    framingParams->aptx_bytes = 672;
                    framingParams->pcm_bytes_per_read = 224;
                    framingParams->pcm_reads = 12;
                    framingParams->frame_size_ctr = 0;
                }
            }
        }
    } else {
        LOGERROR ("invalid codec: %d\n", params.aptX_codec_type);
    }
}

static UINT16 read_pcm (UINT8 channel_id, UINT16 bytes_needed)
{
    UINT16 bytes_read = 0;
    bytes_read = params.read_fn (channel_id, 0, (UINT8*) params.pcmBuffer, bytes_needed);

    return bytes_read;
}


static void timespec_add_ns (struct timespec *ts, long ns)
{
    unsigned long long new_sec = ts->tv_nsec + ns;

    if (new_sec > 1000000000) {
        ts->tv_sec += 1;
        ts->tv_nsec = new_sec % 1000000000;
    } else {
        ts->tv_nsec += ns;
    }
}


static void wait_until (struct timespec when)
{
    pthread_mutex_lock (&params.wait_mtx);
    pthread_cond_timedwait_monotonic (&params.wait_cnd, &params.wait_mtx, &when);
    pthread_mutex_unlock (&params.wait_mtx);
}

static size_t encode_16bit(FramingParams* framingParams,
                                  size_t* aptx_buf_index, UINT8* packet)
{
    size_t aptx_samples = 0;
    size_t pcm_bytes_encoded = 0;
    size_t frame = 0;

    if (!params.encoder) {
        params.encoder = malloc ( (size_t) aptxenc_sizeofaptxbtenc() );

        if (params.encoder) {
            aptxenc_init (params.encoder, 0);
        } else {
            return 0;
        }
    }

    for  (aptx_samples = 0; aptx_samples < framingParams->pcm_bytes_per_read / 16;
          aptx_samples++) {

        size_t i = 0;
        size_t j = 0;
        UINT32 pcmL[4];
        UINT32 pcmR[4];
        UINT16 encodedSample[2];

        for (i = 0, j = frame; i < 4; i++, j++) {
            pcmL[i] = 0x00000000;
            pcmR[i] = 0x00000000;
            pcmL[i] = (UINT16) * (params.pcmBuffer + (2 * j) );
            pcmR[i] = (UINT16) * (params.pcmBuffer + ( (2 * j) + 1) );
        }

        aptxenc_encodestereo (params.encoder, &pcmL, &pcmR, &encodedSample);

        packet[*aptx_buf_index] = (UINT8) ( (encodedSample[0] >> 8) & 0xff);
        packet[*aptx_buf_index + 1] = (UINT8) ( (encodedSample[0] >> 0) & 0xff);
        packet[*aptx_buf_index + 2] = (UINT8) ( (encodedSample[1] >> 8) & 0xff);
        packet[*aptx_buf_index + 3] = (UINT8) ( (encodedSample[1] >> 0) & 0xff);

        frame += 4;
        pcm_bytes_encoded += 16;
        *aptx_buf_index += 4;
    }

    return pcm_bytes_encoded;
}


void aptx_thread (void* arg)
{
    LOGDEBUG("aptx_thread: enter");
    (void) arg;

    UINT8* ptr;
    UINT8 packet[MAXPACKET] = { 0 };
    FramingParams framingParams = {0};
    struct timespec now_ts;

    init_framing (&framingParams);
    clock_gettime (CLOCK_MONOTONIC, &now_ts);

    params.set_priority_fn(TASK_HIGH_MEDIA);

    while (1) {

        size_t aptx_buf_index = 0;
        size_t pcm_bytes_encoded = 0;

        if (params.stop == TRUE ) {
            break;
        }

        update_framing (&framingParams);

        for (size_t reads = 0; reads < framingParams.pcm_reads; reads++) {
            size_t pcm_bytes_read = read_pcm (params.channel,
                                    framingParams.pcm_bytes_per_read);

            if (params.aptX_codec_type == APTX_CODEC) {
                pcm_bytes_encoded += encode_16bit(&framingParams, &aptx_buf_index,
                                     (UINT8*) &packet);
            }

            else {
                LOGERROR ("invalid codec: %d\n", params.aptX_codec_type);
            }
        }

        const int compression_ratio = 4;
        int encoded_bytes = pcm_bytes_encoded / compression_ratio;
        int sent = params.send_fn (packet, encoded_bytes, pcm_bytes_encoded);

        timespec_add_ns (&now_ts, framingParams.sleepTimeNs);
        wait_until (now_ts);

    }
    LOGDEBUG("aptx_thread: exit");
}


int aptx_scheduler_init()
{
    /* load the aptX encoder library */
    LOGDEBUG("aptx_scheduler_deinit: %d", (int)aptXEncLibHandle);
    if (!aptXEncLibHandle)
    {
        aptXEncLibHandle = dlopen(APTX_ENC_LIB_NAME, RTLD_NOW);
        if (!aptXEncLibHandle)
        {
            LOGERROR ("aptX Scheduler Init: aptX Encoder Library Missing\n");
            return -1;
        }

        aptxenc_init = (int (*)(void*, short))dlsym(aptXEncLibHandle, "aptxbtenc_init");
        if (!aptxenc_init)
        {
            LOGERROR ("aptX Scheduler Init: Can't find aptxbtenc_init function in encoder library\n");
            return -1;
        }

        aptxenc_encodestereo = (int (*)(void*, void*, void*, void*))dlsym(aptXEncLibHandle, "aptxbtenc_encodestereo");
        if (!aptxenc_encodestereo)
        {
            LOGERROR ("aptX Scheduler Init: Can't find aptxbtenc_encodestereo function in encoder library\n");
            return -1;
        }

        aptxenc_sizeofaptxbtenc = (int (*)(void))dlsym(aptXEncLibHandle, "SizeofAptxbtenc");
        if (!aptxenc_encodestereo)
        {
            LOGERROR ("aptX Scheduler Init: Can't find aptxenc_sizeofaptxbtenc function in encoder library\n");
            return -1;
        }
    }

    return 0;
}

void aptx_scheduler_deinit()
{
    /* unload the aptX encoder library */
    LOGDEBUG("aptx_scheduler_deinit: %d", (int)aptXEncLibHandle);
    if (aptXEncLibHandle)
    {
        dlclose(aptXEncLibHandle);
        aptxenc_init = NULL;
        aptxenc_encodestereo = NULL;
        aptxenc_sizeofaptxbtenc = NULL;
        aptXEncLibHandle = NULL;
    }

    return;
}

thread_fn aptx_scheduler_start ( void* encoder,
                            AptX_codec_type aptX_codec_type, BOOLEAN use_SCMS_T,
                            UINT16 sample_rate, UINT8 format_bits, UINT8 channel,
                            ReadFn read_fn, BufferSendFn send_fn, Set_priority_fn set_priority_fn,
                            BOOLEAN test, BOOLEAN trace)
{
    LOGDEBUG("aptx_scheduler_start");
    params.encoder = encoder;
    params.aptX_codec_type = aptX_codec_type;
    params.use_SCMS_T = use_SCMS_T;
    params.sample_rate = sample_rate;
    params.format_bits = format_bits;
    params.channel = channel;
    params.test = test;
    params.trace = trace;
    params.read_fn = read_fn;
    params.send_fn = send_fn;
    params.set_priority_fn = set_priority_fn;
    params.stop = FALSE;
    params.is_aptx_started = TRUE;

    pthread_mutex_init ( &(params.wait_mtx), NULL);
    pthread_cond_init ( &(params.wait_cnd), NULL);

    return aptx_thread;
}

BOOLEAN aptx_scheduler_stop()
{
    LOGDEBUG("aptx_scheduler_stop: %d", (int)params.is_aptx_started);
    if (params.is_aptx_started == TRUE) {
        params.stop = TRUE;
        params.is_aptx_started = FALSE;
        return TRUE;
    }
    return FALSE;
}

void aptx_scheduler_dump(int fd)
{
    (void) fd;
}

const char* aptx_scheduler_version()
{
    return BT_APTX_VERSION;
}

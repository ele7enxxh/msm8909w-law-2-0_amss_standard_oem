/*****************************************************************************S
 *
 *  Copyright (c) 2016 Qualcomm Technologies International, Ltd.
 *  All Rights Reserved.
 *  Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 *
 ******************************************************************************/

#ifndef BTAPTX_APTXSCHEDULER_H
#define BTAPTX_APTXSCHEDULER_H

typedef enum {
    APTX_CODEC_NONE=0,
    APTX_CODEC,
    APTX_CODEC_LL,
    APTX_CODEC_HD,
} AptX_codec_type;


typedef UINT32 (*ReadFn) (UINT8 ch_id, UINT16 *p_msg_evt, UINT8* p_buf, UINT32 len);
typedef int (*BufferSendFn) (UINT8*, int, int);
typedef void (*thread_fn)(void *context);
typedef void (*Set_priority_fn)(tHIGH_PRIORITY_TASK);

int aptx_scheduler_init();

void aptx_scheduler_deinit();

thread_fn aptx_scheduler_start ( void* encoder, AptX_codec_type aptX_codec_type, BOOLEAN use_SCMS_T,
          UINT16 sample_rate, UINT8 format_bits, UINT8 channel,
          ReadFn read_fn, BufferSendFn send_fn, Set_priority_fn set_priority_fn,
          BOOLEAN test, BOOLEAN trace);

BOOLEAN aptx_scheduler_stop();
void aptx_scheduler_dump();
const char* aptx_scheduler_version();

#endif

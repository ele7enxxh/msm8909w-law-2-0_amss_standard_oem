#ifndef __CVD_LOG_I_H__
#define __CVD_LOG_I_H__

/*
  Copyright (C) 2013 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/inc/private/cvd_log_i.h#1 $
  $Author: mplcsds1 $
*/

#include "log.h"
#include "apr_comdef.h"


/* Should be removed once the log_codes.h are released from the diag team */

#ifndef LOG_ADSP_CVD_STREAM_RX
  #define LOG_ADSP_CVD_STREAM_RX  ( ( 0x805 ) + LOG_1X_BASE_C )
#endif /* LOG_ADSP_CVD_STREAM_RX */

#ifndef LOG_ADSP_CVD_STREAM_TX
  #define LOG_ADSP_CVD_STREAM_TX  ( ( 0x804 ) + LOG_1X_BASE_C )
#endif /* LOG_ADSP_CVD_STREAM_TX */


/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

#define CVD_LOG_VOICE_CONTAINER_ID ( 0x00012ED5 )
#define CVD_LOG_SEGMENT_HEADER_CONTAINER_ID ( 0x00012ECF )
#define CVD_LOG_AUX_INFO_CONTAINER_ID ( 0x00012ED0 )

#define CVD_LOG_ENC_OUTPUT_TAP_POINT ( 0x00012ED1 )
#define CVD_LOG_DEC_INPUT_TAP_POINT ( 0x00012ED2 )


/*****************************************************************************
 * DEFINITIONS                                                               *
 ****************************************************************************/

typedef struct cvd_log_segment_header_t cvd_log_segment_header_t;

#include "apr_pack_begin.h"

struct cvd_log_segment_header_t
{
  uint16_t container_size;
  uint32_t container_id;
  uint16_t minor_version;
  uint32_t sequence_num;
  uint32_t total_size;
  uint32_t offset;
}
#include "apr_pack_end.h"
;

typedef struct cvd_log_aux_info_t cvd_log_aux_info_t;

#include "apr_pack_begin.h"

struct cvd_log_aux_info_t
{
  uint16_t container_size;
  uint32_t container_id;
  uint16_t minor_version;
  uint32_t instance;
  uint32_t call_num;
  uint64_t timestamp;
  uint32_t tap_id;
}
#include "apr_pack_end.h"
;

typedef struct cvd_log_raw_data_t cvd_log_raw_data_t;

#include "apr_pack_begin.h"

struct cvd_log_raw_data_t
{
  uint16_t container_size;
  uint32_t container_id;

#if __STDC_VERSION__ >= 199901L
  uint8_t payload[];
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;

typedef struct cvd_log_voice_container_t cvd_log_voice_container_t;

#include "apr_pack_begin.h"

struct cvd_log_voice_container_t
{
  uint16_t cointainer_size;
  uint32_t container_id;
  uint16_t minor_version;

  cvd_log_segment_header_t segment_header;

  cvd_log_aux_info_t aux_info;

  cvd_log_raw_data_t data;
}
#include "apr_pack_end.h"
;

typedef struct cvd_log_voice_t cvd_log_voice_t;

#include "apr_pack_begin.h"

struct cvd_log_voice_t
{
  log_hdr_type hdr;
  cvd_log_voice_container_t voice;
}
#include "apr_pack_end.h"
;

APR_INTERNAL int32_t cvd_log_commit_data (
  log_code_type log_code,
  uint32_t sequence_num,
  uint32_t total_size,
  uint32_t instance,
  uint32_t call_num,
  uint64_t timestamp,
  uint32_t tap_id,
  uint32_t media_id,
  uint8_t* data
);

#endif /* __CVD_LOG_I_H__ */


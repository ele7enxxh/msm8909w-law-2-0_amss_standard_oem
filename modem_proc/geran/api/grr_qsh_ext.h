#ifndef GRR_QSH_EXT_H
#define GRR_QSH_EXT_H

/*! \file grr_qsh_ext.h

  This file contains definitions for QSH crash dump and metrics data collection.

                Copyright (c) 2015 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/grr_qsh_ext.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

#include "qsh.h"


#define GRR_QSH_MAJOR_VER  1
#define GRR_QSH_MINOR_VER  1


/* -----------------------  Definitions for QSH Dump  --------------------- */

typedef enum
{
  GRR_QSH_DUMP_TAG_DATA    = 0,
  GRR_QSH_DUMP_TAG_DATA_2  = 1,
  GRR_QSH_DUMP_TAG_COUNT
} grr_qsh_dump_tag_e;

// Message logging. 9 bytes.
typedef struct
{
  uint32 timestamp;
  uint8  seq_num;
  uint32 msg_id;
} grr_qsh_dump_msg_s;

// Internal Message (IMSG) logging. 7 bytes.
typedef struct
{
  uint32 timestamp;
  uint8  seq_num;
  uint8  msg_set;
  uint8  msg_id;
} grr_qsh_dump_imsg_s;

// OTA message logging. 28 bytes.
typedef struct
{
  uint32 timestamp;
  uint8  ul_dl;
  uint8  msg[23];
} grr_qsh_dump_ota_msg_s;

// Crash trace buffer. 1090 bytes.
typedef struct
{
  grr_qsh_dump_msg_s      gl1_msg_buf[20];    //  20 x  9 =  180 bytes
  uint8                   gl1_msg_buf_index;  //               1 byte

  grr_qsh_dump_msg_s      gmac_msg_buf[20];   //  20 x  9 =  180 bytes
  uint8                   gmac_msg_buf_index; //               1 byte

  grr_qsh_dump_msg_s      nas_msg_buf[20];    //  20 x  9 =  180 bytes
  uint8                   nas_msg_buf_index;  //               1 byte

  grr_qsh_dump_msg_s      wrrc_msg_buf[10];   //  10 x  9 =   90 bytes
  uint8                   wrrc_msg_buf_index; //                1 byte

  grr_qsh_dump_msg_s      trrc_msg_buf[10];   //  10 x  9 =   90 bytes
  uint8                   trrc_msg_buf_index; //                1 byte

  grr_qsh_dump_msg_s      msgr_msg_buf[10];   //  10 x  9 =   90 bytes
  uint8                   msgr_msg_buf_index; //               1 byte

  grr_qsh_dump_imsg_s     grr_imsg_buf[20];   //  20 x  7 =  140 bytes
  uint8                   grr_imsg_buf_index; //               1 byte

  grr_qsh_dump_ota_msg_s  ota_msg_buf[5];     //   5 x 28 =  140 bytes
  uint8                   ota_msg_buf_index;  //               1 byte

} grr_qsh_dump_msg_buf_s;

// GRR internal event logging. 9 bytes.
typedef struct
{
  uint32 timestamp;
  uint8  seq_num;
  uint16 event;
  uint8  sm;
  uint8  state;
} grr_qsh_dump_event_s;

// GRR internal event logging buffer. 901 bytes.
typedef struct
{
  grr_qsh_dump_event_s    event_buf[100];     //            900 bytes
  uint8                   event_buf_index;    //              1 byte
} grr_qsh_dump_event_buf_s;

// GRR_QSH_DUMP_TAG_DATA structure for QSH. sizeof(hdr) + 1991 bytes.
typedef struct
{
  qsh_dump_tag_hdr_s        hdr;
  grr_qsh_dump_msg_buf_s    msg_buf;          //           1090 bytes
  grr_qsh_dump_event_buf_s  event_buf;        //            901 bytes
} grr_qsh_dump_tag_data_s;

/* --------------------  Definitions for QSH Metrics  --------------------- */

typedef enum
{
  GRR_QSH_METRIC_MSG            = 0,
  GRR_QSH_METRIC_IMSG           = 1,
  GRR_QSH_METRIC_OTA_MSG        = 2,
  GRR_QSH_METRIC_EVENT          = 3,
  GRR_QSH_METRIC_RESEL          = 4,
  GRR_QSH_METRIC_HANDOVER       = 5,
  GRR_QSH_METRIC_RACH           = 6,
  GRR_QSH_METRIC_CSFB           = 7,
  GRR_QSH_METRIC_SERVICE        = 8,
  GRR_QSH_METRIC_CELL_INFO      = 9,
  GRR_QSH_METRIC_CONN_END_INFO  = 10,
  GRR_QSH_METRIC_COUNT
} grr_qsh_metric_e;

// GRR_QSH_METRIC_MSG structure
typedef struct
{
  qsh_metric_hdr_s       hdr;
  uint8                  msg_set;
  uint32                 msg_id;
} grr_qsh_metric_msg_s;

// GRR_QSH_METRIC_IMSG structure
typedef struct
{
  qsh_metric_hdr_s       hdr;
  uint8                  msg_set;
  uint8                  msg_id;
} grr_qsh_metric_imsg_s;

// GRR_QSH_METRIC_IMSG structure
typedef struct
{
  qsh_metric_hdr_s       hdr;
  uint8                  channel_type;
  uint8                  msg_buf[23];
} grr_qsh_metric_ota_msg_s;

// GRR_QSH_METRIC_EVENT structure
typedef struct
{
  qsh_metric_hdr_s       hdr;
  uint16                 event;
  uint8                  sm;
  uint8                  state;
} grr_qsh_metric_event_s;

// GRR_QSH_METRIC_RESEL structure
typedef struct
{
  qsh_metric_hdr_s       hdr;
  uint8                  num_of_resel_g2g;
  uint8                  num_of_resel_g2wt;
  uint8                  num_of_resel_g2l;
} grr_qsh_metric_resel_s;

// GRR_QSH_METRIC_HANDOVER structure
typedef struct
{
  qsh_metric_hdr_s       hdr;
  uint8                  num_of_handover_g2g;
  uint8                  num_of_handover_g2w;
  uint8                  num_of_handover_w2g;
  uint8                  num_of_handover_l2g;
} grr_qsh_metric_handover_s;

// GRR_QSH_METRIC_RACH structure
typedef struct
{
  qsh_metric_hdr_s       hdr;
  uint8                  num_of_rach;
  uint8                  num_of_t3126_t3146_expiry;
  uint8                  num_of_ia_rcvd;
  uint8                  num_of_ia_rej_rcvd;
} grr_qsh_metric_rach_s;

// GRR_QSH_METRIC_CSFB
typedef struct
{
  qsh_metric_hdr_s       hdr;
  uint8                  num_of_csfb;
} grr_qsh_metric_csfb_s;

// GRR_QSH_METRIC_SERVICE
typedef struct
{
  qsh_metric_hdr_s       hdr;
  uint8                  num_of_dsf;
  uint8                  num_of_rlf;
  uint8                  num_of_oos;
} grr_qsh_metric_service_s;

// GRR_QSH_METRIC_CELL_INFO structure
typedef struct
{
  qsh_metric_hdr_s       hdr;
  uint16                 arfcn;
  uint8                  plmn_id[3];
  uint16                 lac;
  uint8                  rac;
  uint16                 cell_identity;
} grr_qsh_metric_cell_info_s;

/* --------  Definitions for QSH Metrics for Connection End Data -------- */

// GRR_QSH_METRIC_CONN_END_INFO structure
typedef struct
{
  qsh_metric_hdr_s       hdr;
  /* sys_call_end_event_e_type */
  uint8                  type;  
  /* sys_handover_failure_e_type / sys_rlf_weak_signal_e_type / cause IE in CHANNEL_RELEASE OTA message */
  uint8                  cause;
}grr_qsh_metric_conn_end_info_s;

#endif /* #ifndef GRR_QSH_EXT_H */

/* EOF */


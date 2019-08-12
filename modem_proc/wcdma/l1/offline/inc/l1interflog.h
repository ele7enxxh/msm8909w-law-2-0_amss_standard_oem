#ifndef L1INTERFLOG_H
#define L1INTERFLOG_H

/*===========================================================================
                     L 1 I N T E R F L O G . H

DESCRIPTION
  This header file describes the L1 inter-frequency log packet definition

Copyright (c) 2003, 2004, 2008 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/l1interflog.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
01/05/15    cc      New energy to EcIo formula for interF step1 0x41C6 and PN 0x417A
05/23/13    geg     WSW decoupling (except for component of QCHAT)
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
07/02/12    cc      New log packet 0x417A for W2W CM PN search results
09/22/09    sup     Removed log code assignments and placed them in
                    log_codes_wcdma_int_v.h
04/27/09    yh      Added STTD info in the PN log packet and reduced 
                    Coh length to 1 byte.
11/21/08    yh      Make CM PN window size Cx1
11/13/08    yh      Add CM support
06/10/08    ms      Added PACKED_POST attribute for structures to make it
                    compatible for GNU compiler.
05/10/04    gs      Changed L1_WCDMA_INTERF_LOG_LIST_SRCH_MAX_TASK to 16
10/24/03    gs      Created inter-freq search log packet definition header
                    file
===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "comdef.h"
#include "qw.h"
#include "log.h"
#include "log_codes_wcdma_int.h"
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* --------------------- */
/* LOG Packet definition */
/* --------------------- */

#ifndef L1_WCDMA_INTERF_STEP1_SRCH_LOG_PKT
#define L1_WCDMA_INTERF_STEP1_SRCH_LOG_PKT L1_WCDMA_INTERF_STEP1_0x41C6_SRCH_LOG_PKT
#else
#error "Redefinition of the log command code: L1_WCDMA_INTERF_STEP1_SRCH_LOG_PKT"
#endif

#ifndef L1_WCDMA_INTERF_LIST_SRCH_LOG_PKT
#define L1_WCDMA_INTERF_LIST_SRCH_LOG_PKT L1_WCDMA_INTERF_LIST_0x417A_SRCH_LOG_PKT
#else
#error "Redefinition of the log command code: L1_WCDMA_INTERF_LIST_SRCH_LOG_PKT"
#endif

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* UE state for WCDMA inter-frequency */
#define L1_WCDMA_INTERF_LOG_UE_STATE_IDLE      0
#define L1_WCDMA_INTERF_LOG_UE_STATE_CELL_FACH 1
#define L1_WCDMA_INTERF_LOG_UE_STATE_DEDICATED 2

/* Step 1 state in decicated mode for compressed mode */
#define L1_WCDMA_INTERF_LOG_STEP1_START 0
#define L1_WCDMA_INTERF_LOG_STEP1_CONT  1
#define L1_WCDMA_INTERF_LOG_STEP1_DONE  2

#define L1_WCDMA_INTERF_LOG_STEP1_RESULT_MAX_PEAKS 32

#define L1_WCDMA_INTERF_LOG_CELL_SET_ACTIVE    0
#define L1_WCDMA_INTERF_LOG_CELL_SET_MONITORED 1
#define L1_WCDMA_INTERF_LOG_CELL_SET_UNLISTED  2

#define L1_WCDMA_INTERF_LOG_PN_SRCH_MAX_TASK 64
#define L1_WCDMA_INTERF_LOG_PN_SRCH_MAX_PEAKS_PER_TASK 6

/* Header definition since the header definition is defined in log.c */
typedef PACKED struct PACKED_POST
{
  uint16 len;  /* Specifies the length, in bytes of 
                 the entry, including this header. */

  uint16 code; /* Specifies the log code for the 
                  entry as enumerated above.       
                  Note: This is specified as word 
                  to guarantee size.               */

  qword ts;    /* The system timestamp for the log entry. The upper 48 bits
                  represent elapsed time since 6 Jan 1980 00:00:00 
                  in 1.25 ms units. The low order 16 bits represent elapsed 
                  time since the last 1.25 ms tick in 1/32 chip units 
                  (this 16 bit counter wraps at the value 49152).          */
} l1_wcdma_interf_log_hdr_struct_type;

/* L1 WCDMA inter-freq step 1 search packet definition */
/* --------------------------------------------------- */

/* L1 WCDMA inter-frequency interf result */
typedef PACKED struct PACKED_POST
{
  /* PSC peaks position in Cx8 units */
  uint16 psc_pos_cx8;
  /* searcher energy in linear units */
  uint16 srch_egy;
} l1_wcdma_interf_step1_result_log_info_struct_type;

LOG_RECORD_DEFINE(L1_WCDMA_INTERF_STEP1_SRCH_LOG_PKT)
  /* Pkt Version */
  uint8 pkt_ver;
  /* EcIo conversion type */
  uint8 ecio_cov_type;

  /* WCDMA L1 UE state */
  uint8 ue_state;
  
  /* union of srch schedule time based on UE state */
  PACKED union PACKED_POST
  {
    /* IDLE and FACH state interf step 1 schedule info */
    PACKED struct PACKED_POST
    {
      /* search start CFN */
      uint8 search_start_cfn;
      /* search end CFN */
      uint8 search_end_cfn;
    } wcdma_idle_fach;

    /* Dedicated state interf step 1 schedule info */
    PACKED struct PACKED_POST
    {
      /* CFN at which gap starts in which step 1 is scheduled */
      uint8 gap_start_cfn;
      /* Step 1 search state (start/cont/end) */
      uint8 srch_state;
    } wcdma_dedicated;
  } search_sch_info;

  /* carrier frequency */
  uint16 arfcn;
  /* step 1 non coherent integration length */
  uint8 non_coh_int_len;
  /* total non coherent integration done */
  uint8 total_non_coh_int_done;
  /* number of peaks found in result */
  uint8 num_peaks;

  /* step 1 search result */
  l1_wcdma_interf_step1_result_log_info_struct_type
    peaks[L1_WCDMA_INTERF_LOG_STEP1_RESULT_MAX_PEAKS];
LOG_RECORD_END

/* L1 WCDMA inter-freq list search packet definition */
/* ------------------------------------------------- */
/* L1 WCDMA inter-freq list search task log info */
typedef PACKED struct PACKED_POST
{
  /* window size */
  uint8 win_sz_unit_cx1;
  /* list search coherent integration length */
  uint8 coh_int_len_unit_64_cx1;
  /* STTD info for the cell */
  uint8 sttd_info;
  /* list search non coherent integration length */
  uint8 non_coh_int_len;
  /* list search window start position */
  uint32 win_start_pos_cx8;
  /* scrambling code inex */
  uint16 scr_code_idx;
  /* cell set type (Active/Monitored/Detected) */
  uint8 cell_set_type;
  /* channelization code */
  uint8 chan_code;
} l1_wcdma_interf_pn_srch_task_log_info_struct_type;

/* L1 WCDMA inter-freq pn search task result log info */
typedef PACKED struct PACKED_POST
{
  /* PN peaks position in Cx8 units for primary chain */
  uint32 prim_pn_pos_cx8;
  /* searcher energy in linear units for primary chain */
  uint16 prim_srch_egy;
} l1_wcdma_interf_pn_srch_task_result_log_info_struct_type;

#define L1_WCDMA_INTERF_LOG_PN_SRCH_TASK_N_RESULT_BUF_SZ \
  (sizeof(l1_wcdma_interf_pn_srch_task_log_info_struct_type) + \
   (sizeof(l1_wcdma_interf_pn_srch_task_result_log_info_struct_type) *\
    (L1_WCDMA_INTERF_LOG_PN_SRCH_MAX_PEAKS_PER_TASK)))

LOG_RECORD_DEFINE(L1_WCDMA_INTERF_LIST_SRCH_LOG_PKT)
  /* Pkt Version */
  uint8 pkt_ver;
  /* EcIo conversion type */
  uint8 ecio_cov_type;
  /* WCDMA L1 UE state */
  uint8 ue_state;
  /* search start time */
  uint8 start_cfn;
  /* carrier frequency */
  uint16 arfcn;
  /* carrier RSSI */
  int8 rssi_dbm;
  /* number of peaks found in result */
  uint8 num_tasks;
  
  /* list search tasks and results */
  PACKED struct PACKED_POST
  {
    /* interf list search task info */
    l1_wcdma_interf_pn_srch_task_log_info_struct_type task;
    /* task results */
    l1_wcdma_interf_pn_srch_task_result_log_info_struct_type
      result[L1_WCDMA_INTERF_LOG_PN_SRCH_MAX_PEAKS_PER_TASK];
  } task_n_result[L1_WCDMA_INTERF_LOG_PN_SRCH_MAX_TASK];
LOG_RECORD_END
#endif /* L1INTERFLOG_H */

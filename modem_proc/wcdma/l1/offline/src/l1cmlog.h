#ifndef L1CMLOG_H
#define L1CMLOG_H

/*===========================================================================
                     L 1 C M L O G . H

DESCRIPTION
  This header file describes the CcTrCh log packet definitions

Copyright (c) 2000 - 2008 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/l1cmlog.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
05/23/13    geg     WSW decoupling (except for component of QCHAT)
09/22/09    sup     Removed log code assignments and placed them in
                    log_codes_wcdma_int_v.h
06/10/08    ms      Added PACKED_POST attribute for structures to make it
                    compatible for GNU compiler.
05/19/05    bbd     Changed datatype of fcch_tone_qs_offset to resolve lint error.
03/31/04    gs      Scaled back the threshold for swapping double buffer
                    for compressed mode GSM logging. It is in macro
                    L1_CM_LOG_SUBMIT_SIZE_LIMIT
11/06/03    gs      Deleted dulicate #defines copied from other modules.
10/04/02    gs      Corrected the type of RSSI in periodic RSSI log info.
                    Renamed cell_state to cell_meas_state to give more
                    logical name.
09/27/02    gs      Added GSM CM meas log entity Id
                    L1_CM_GSM_PERIODIC_RSSI_LOG_INFO_ID.
                    Added cell state to GSM periodic measurement log info.
08/09/02    gs      Created this file
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

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

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
} l1_cm_log_hdr_struct_type;

#define L1_CM_GSM_RSSI_MEAS_LOG_INFO_ID     0
#define L1_CM_GSM_FCCH_DETEC_LOG_INFO_ID    1
#define L1_CM_GSM_SCH_DECODE_LOG_INFO_ID    2
#define L1_CM_GSM_PERIODIC_RSSI_LOG_INFO_ID 3

typedef PACKED struct PACKED_POST
{
  /* WCDMA CFN when GAP starts */
  uint8 gap_cfn;
  /* GSM frame number when GAP starts */
  uint8 gap_gfn;
  /* GAP start GSM QS offset */
  uint16 gap_start_qs_offset;

  /* number of GSM RSSI scans */
  uint8 num_bursts;
} l1_cm_gsm_rssi_meas_log_info_struct_type;

typedef PACKED struct PACKED_POST
{
  /* Carrier RF channel number */
  uint16 carrier_arfcn;
  /* Burst start GSM QS offset */
  uint16 burst_start_qs_offset;

  /* LNA gain range choosen */
  uint8 gsm_lna_state;
  /* RSSI dBmx16 */
  int16 rssi_dbmx16;
} l1_cm_gsm_rssi_burst_log_info_struct_type;

typedef PACKED struct PACKED_POST
{
  /* WCDMA CFN when GAP starts */
  uint8 gap_cfn;
  /* GSM frame number when GAP starts */
  uint8 gap_gfn;
  /* GAP start GSM QS offset */
  uint16 gap_start_qs_offset;

  /* Carrier RF channel number */
  uint16 carrier_arfcn;
  /* ACQ start GSM QS offset */
  uint16 acq_start_qs_offset;

  /* number of FCCH tones detected */
  uint8 num_fcch_tone_detected;
  /* Tone QS offset from ACQ start offset */
  int16 fcch_tone_qs_offset;
  /* SNR estimate of the tone */
  uint16 snr_estimate;
} l1_cm_fcch_acq_log_info_struct_type;

#define CM_SCH_DEC_LOG_INFO_BSIC_VER_APPLICABLE_BIT    0
#define CM_SCH_DEC_LOG_INFO_BSIC_VER_BSIC_MISMATCH_BIT 1
#define CM_SCH_DEC_LOG_INFO_BSIC_VER_RFN_MISMATCH_BIT  2
#define CM_SCH_DEC_LOG_INFO_CRC_PASSED_BIT             3

typedef PACKED struct PACKED_POST
{
  /* WCDMA CFN when GAP starts */
  uint8 gap_cfn;
  /* GSM frame number when GAP starts */
  uint8 gap_gfn;
  /* GAP start GSM QS offset */
  uint16 gap_start_qs_offset;

  /* Carrier RF channel number */
  uint16 carrier_arfcn;
  /* GSM frame number at which SCH decoded */
  uint8 sch_decode_gfn;
  /* GSM QS offset to the center of the burst at which SCH is decoded */
  uint16 sch_burst_qs_offset;
  /* SCH frame nunber 1, 11, 12, 21, 22, 31, 32, 41, 42 */
  uint8 sch_decode_frame_number;

  /* indicate the BSIC verification status and CRC pass status */
  uint8 sch_dec_crc_bsic_verify_bf;
  /* Decoded SCH data */
  uint32 sch_decoded_data;

  /* RSSI dBmx16 */
  int16 rssi_dbmx16;
  /* Tone QS offset from ACQ start offset */
  int16 sch_burst_time_offset_qs;
  /* SNR estimate */
  uint16 snr_estimate;
} l1_cm_sch_dec_log_info_struct_type;

typedef PACKED struct PACKED_POST
{
  /* number of GSM carrier scanned */
  uint8 num_carriers;
  /* RSSI samples per carrier */
  uint8 num_samples;
  /* Time taken for collection of these samples */
  uint8 num_w_frames_used;
} l1_cm_rssi_report_prd_log_info_struct_type;

typedef PACKED struct PACKED_POST
{
  /* Carrier RF channel number */
  uint16 carrier_arfcn;
  /* RSSI dBmx16 */
  int16 rssi_dbmx16;
  /* This indicate cell state and is translation of
     srch_gsm_srch_meas_state in to uint8 */
  uint8 cell_meas_state;
} l1_cm_gsm_carrier_rssi_report_log_info_struct_type;

/* log packet info size */
#define L1_CM_GSM_RSSI_MEAS_LOG_INFO_SZ  (sizeof(l1_cm_gsm_rssi_meas_log_info_struct_type))
#define L1_CM_GSM_RSSI_BURST_LOG_INFO_SZ (sizeof(l1_cm_gsm_rssi_burst_log_info_struct_type))
#define L1_CM_GSM_FCCH_ACQ_LOG_INFO_SZ   (sizeof(l1_cm_fcch_acq_log_info_struct_type))
#define L1_CM_GSM_SCH_DEC_LOG_INFO_SZ    (sizeof(l1_cm_sch_dec_log_info_struct_type))
#define L1_CM_GSM_RSSI_RPT_PRD_LOG_INFO_SZ (sizeof(l1_cm_rssi_report_prd_log_info_struct_type))
#define L1_CM_GSM_CAR_RSSI_RPT_LOG_INFO_SZ (sizeof(l1_cm_gsm_carrier_rssi_report_log_info_struct_type))

/* L1 CM log packet definition */
/* --------------------------- */

#define L1_CM_LOG_MAX_BUF_SIZE 1000
#define L1_CM_LOG_SUBMIT_SIZE_LIMIT 800
#define L1_CM_LOG_SUBMIT_MAX_WFRAME_CNT 100

LOG_RECORD_DEFINE(L1_CM_GSM_MEAS_LOG_PKT)
  /* number of CM log entities. */
  uint8 num_log_entities;
  /* log buffer
  
    CM log buffer structure:

    CM log info Id
    Log info
    CM log info Id
    Log info
    CM log info Id
    Log info
    .......
    ....... */
  uint8 cm_log_buffer[L1_CM_LOG_MAX_BUF_SIZE];
LOG_RECORD_END

#endif /* L1CMLOG_H */


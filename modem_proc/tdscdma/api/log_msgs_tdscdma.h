#ifndef LOG_MSGS_TDSCDMA_H
#define LOG_MSGS_TDSCDMA_H

/*===========================================================================

                         Log Code Definitions

General Description
  This file contains log message macros for TDSCDMA L1/L2/L3.

Copyright (c) 2011-2013 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                             Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/api/log_msgs_tdscdma.h#1 $
$DateTime: 2016/12/13 07:58:19 $
$Author: mplcsds1 $
   
when       who     what, where, why
--------   ---    ----------------------------------------------------------
04/11/12   mkh    Now blocks any LOW, MED messages to get pumped to XDM 
12/04/11   mkh    Initial Version 

===========================================================================*/

extern boolean tds_nv_l3_trace_high_flg;
extern boolean tds_nv_l3_trace_med_flg;
extern boolean tds_nv_l2_trace_high_flg;
extern boolean tds_nv_l2_trace_med_flg;


/****************************************************************************
** TDSRLC_MSGx - Macros only take non-string parameters (like %d, not %s).
**
** Avoiding string support (%s) here because of excessive CPU overhead.
**
** Options for LEVEL:
** - MSG_LEGACY_LOW,
** - MSG_LEGACY_MED,
** - MSG_LEGACY_HIGH,
** - MSG_LEGACY_ERROR,
** - MSG_LEGACY_FATAL
**
** Example:
** - TDSRLC_MSG2(MSG_LEGACY_ERROR, "delta_vco_temp (%d) > 0.5ppm, bounded to %d",
**            ABS(delta_vco_temp), tcxomgr.vco_temp_update_thresh);
**************************************************************************/

#define TDSRRC_MSG0(LEVEL, FMT) \
  if (LEVEL==MSG_LEGACY_FATAL||LEVEL==MSG_LEGACY_ERROR||(LEVEL==MSG_LEGACY_HIGH&&tds_nv_l3_trace_high_flg)|| \
       ((LEVEL==MSG_LEGACY_MED||LEVEL==MSG_LEGACY_LOW)&&tds_nv_l3_trace_med_flg)) \
        MSG(MSG_SSID_TDSCDMA_RRC, LEVEL, FMT)

#define TDSRRC_MSG1(LEVEL, FMT, A) \
  if (LEVEL==MSG_LEGACY_FATAL||LEVEL==MSG_LEGACY_ERROR||(LEVEL==MSG_LEGACY_HIGH&&tds_nv_l3_trace_high_flg)|| \
       ((LEVEL==MSG_LEGACY_MED||LEVEL==MSG_LEGACY_LOW)&&tds_nv_l3_trace_med_flg)) \
        MSG_1(MSG_SSID_TDSCDMA_RRC, LEVEL, FMT, A)

#define TDSRRC_MSG2(LEVEL, FMT, A, B) \
  if (LEVEL==MSG_LEGACY_FATAL||LEVEL==MSG_LEGACY_ERROR||(LEVEL==MSG_LEGACY_HIGH&&tds_nv_l3_trace_high_flg)|| \
       ((LEVEL==MSG_LEGACY_MED||LEVEL==MSG_LEGACY_LOW)&&tds_nv_l3_trace_med_flg)) \
        MSG_2(MSG_SSID_TDSCDMA_RRC, LEVEL, FMT, A, B)

#define TDSRRC_MSG3(LEVEL, FMT, A, B, C) \
  if (LEVEL==MSG_LEGACY_FATAL||LEVEL==MSG_LEGACY_ERROR||(LEVEL==MSG_LEGACY_HIGH&&tds_nv_l3_trace_high_flg)|| \
       ((LEVEL==MSG_LEGACY_MED||LEVEL==MSG_LEGACY_LOW)&&tds_nv_l3_trace_med_flg)) \
        MSG_3(MSG_SSID_TDSCDMA_RRC, LEVEL, FMT, A, B, C)

#define TDSRRC_MSG4(LEVEL, FMT, A, B, C, D) \
  if (LEVEL==MSG_LEGACY_FATAL||LEVEL==MSG_LEGACY_ERROR||(LEVEL==MSG_LEGACY_HIGH&&tds_nv_l3_trace_high_flg)|| \
       ((LEVEL==MSG_LEGACY_MED||LEVEL==MSG_LEGACY_LOW)&&tds_nv_l3_trace_med_flg)) \
        MSG_4(MSG_SSID_TDSCDMA_RRC, LEVEL, FMT, A, B, C, D)

#define TDSRRC_MSG5(LEVEL, FMT, A, B, C, D, E) \
  if (LEVEL==MSG_LEGACY_FATAL||LEVEL==MSG_LEGACY_ERROR||(LEVEL==MSG_LEGACY_HIGH&&tds_nv_l3_trace_high_flg)|| \
       ((LEVEL==MSG_LEGACY_MED||LEVEL==MSG_LEGACY_LOW)&&tds_nv_l3_trace_med_flg)) \
        MSG_5(MSG_SSID_TDSCDMA_RRC, LEVEL, FMT, A, B, C, D, E)

#define TDSRRC_MSG6(LEVEL, FMT, A, B, C, D, E, F) \
  if (LEVEL==MSG_LEGACY_FATAL||LEVEL==MSG_LEGACY_ERROR||(LEVEL==MSG_LEGACY_HIGH&&tds_nv_l3_trace_high_flg)|| \
       ((LEVEL==MSG_LEGACY_MED||LEVEL==MSG_LEGACY_LOW)&&tds_nv_l3_trace_med_flg)) \
        MSG_6(MSG_SSID_TDSCDMA_RRC, LEVEL, FMT, A, B, C, D, E, F)

#define TDSRRC_MSG7(LEVEL, FMT, A, B, C, D, E, F, G) \
  if (LEVEL==MSG_LEGACY_FATAL||LEVEL==MSG_LEGACY_ERROR||(LEVEL==MSG_LEGACY_HIGH&&tds_nv_l3_trace_high_flg)|| \
       ((LEVEL==MSG_LEGACY_MED||LEVEL==MSG_LEGACY_LOW)&&tds_nv_l3_trace_med_flg)) \
        MSG_7(MSG_SSID_TDSCDMA_RRC, LEVEL, FMT, A, B, C, D, E, F, G)
  
#define TDSRRC_MSG8(LEVEL, FMT, A, B, C, D, E, F, G, H) \
  if (LEVEL==MSG_LEGACY_FATAL||LEVEL==MSG_LEGACY_ERROR||(LEVEL==MSG_LEGACY_HIGH&&tds_nv_l3_trace_high_flg)|| \
       ((LEVEL==MSG_LEGACY_MED||LEVEL==MSG_LEGACY_LOW)&&tds_nv_l3_trace_med_flg)) \
        MSG_8(MSG_SSID_TDSCDMA_RRC, LEVEL, FMT, A, B, C, D, E, F, G, H)

/***************************************************************************/

#define TDSRLC_MSG0(LEVEL, FMT) \
  if (LEVEL==MSG_LEGACY_FATAL||LEVEL==MSG_LEGACY_ERROR||(LEVEL==MSG_LEGACY_HIGH&&tds_nv_l2_trace_high_flg)|| \
       ((LEVEL==MSG_LEGACY_MED||LEVEL==MSG_LEGACY_LOW)&&tds_nv_l2_trace_med_flg)) \
        MSG(MSG_SSID_TDSCDMA_RLC, LEVEL, FMT)

#define TDSRLC_MSG1(LEVEL, FMT, A) \
  if (LEVEL==MSG_LEGACY_FATAL||LEVEL==MSG_LEGACY_ERROR||(LEVEL==MSG_LEGACY_HIGH&&tds_nv_l2_trace_high_flg)|| \
       ((LEVEL==MSG_LEGACY_MED||LEVEL==MSG_LEGACY_LOW)&&tds_nv_l2_trace_med_flg)) \
        MSG_1(MSG_SSID_TDSCDMA_RLC, LEVEL, FMT, A)

#define TDSRLC_MSG2(LEVEL, FMT, A, B) \
  if (LEVEL==MSG_LEGACY_FATAL||LEVEL==MSG_LEGACY_ERROR||(LEVEL==MSG_LEGACY_HIGH&&tds_nv_l2_trace_high_flg)|| \
       ((LEVEL==MSG_LEGACY_MED||LEVEL==MSG_LEGACY_LOW)&&tds_nv_l2_trace_med_flg)) \
        MSG_2(MSG_SSID_TDSCDMA_RLC, LEVEL, FMT, A, B)

#define TDSRLC_MSG3(LEVEL, FMT, A, B, C) \
  if (LEVEL==MSG_LEGACY_FATAL||LEVEL==MSG_LEGACY_ERROR||(LEVEL==MSG_LEGACY_HIGH&&tds_nv_l2_trace_high_flg)|| \
       ((LEVEL==MSG_LEGACY_MED||LEVEL==MSG_LEGACY_LOW)&&tds_nv_l2_trace_med_flg)) \
        MSG_3(MSG_SSID_TDSCDMA_RLC, LEVEL, FMT, A, B, C)

#define TDSRLC_MSG4(LEVEL, FMT, A, B, C, D) \
  if (LEVEL==MSG_LEGACY_FATAL||LEVEL==MSG_LEGACY_ERROR||(LEVEL==MSG_LEGACY_HIGH&&tds_nv_l2_trace_high_flg)|| \
       ((LEVEL==MSG_LEGACY_MED||LEVEL==MSG_LEGACY_LOW)&&tds_nv_l2_trace_med_flg)) \
        MSG_4(MSG_SSID_TDSCDMA_RLC, LEVEL, FMT, A, B, C, D)

#define TDSRLC_MSG5(LEVEL, FMT, A, B, C, D, E) \
  if (LEVEL==MSG_LEGACY_FATAL||LEVEL==MSG_LEGACY_ERROR||(LEVEL==MSG_LEGACY_HIGH&&tds_nv_l2_trace_high_flg)|| \
       ((LEVEL==MSG_LEGACY_MED||LEVEL==MSG_LEGACY_LOW)&&tds_nv_l2_trace_med_flg)) \
        MSG_5(MSG_SSID_TDSCDMA_RLC, LEVEL, FMT, A, B, C, D, E)

#define TDSRLC_MSG6(LEVEL, FMT, A, B, C, D, E, F) \
  if (LEVEL==MSG_LEGACY_FATAL||LEVEL==MSG_LEGACY_ERROR||(LEVEL==MSG_LEGACY_HIGH&&tds_nv_l2_trace_high_flg)|| \
       ((LEVEL==MSG_LEGACY_MED||LEVEL==MSG_LEGACY_LOW)&&tds_nv_l2_trace_med_flg)) \
        MSG_6(MSG_SSID_TDSCDMA_RLC, LEVEL, FMT, A, B, C, D, E, F)

/***************************************************************************/

#define TDSMAC_MSG0(LEVEL, FMT) \
  if (LEVEL==MSG_LEGACY_FATAL||LEVEL==MSG_LEGACY_ERROR||(LEVEL==MSG_LEGACY_HIGH&&tds_nv_l2_trace_high_flg)|| \
       ((LEVEL==MSG_LEGACY_MED||LEVEL==MSG_LEGACY_LOW)&&tds_nv_l2_trace_med_flg)) \
        MSG(MSG_SSID_TDSCDMA_MAC, LEVEL, FMT)

#define TDSMAC_MSG1(LEVEL, FMT, A) \
  if (LEVEL==MSG_LEGACY_FATAL||LEVEL==MSG_LEGACY_ERROR||(LEVEL==MSG_LEGACY_HIGH&&tds_nv_l2_trace_high_flg)|| \
       ((LEVEL==MSG_LEGACY_MED||LEVEL==MSG_LEGACY_LOW)&&tds_nv_l2_trace_med_flg)) \
        MSG_1(MSG_SSID_TDSCDMA_MAC, LEVEL, FMT, A)

#define TDSMAC_MSG2(LEVEL, FMT, A, B) \
  if (LEVEL==MSG_LEGACY_FATAL||LEVEL==MSG_LEGACY_ERROR||(LEVEL==MSG_LEGACY_HIGH&&tds_nv_l2_trace_high_flg)|| \
       ((LEVEL==MSG_LEGACY_MED||LEVEL==MSG_LEGACY_LOW)&&tds_nv_l2_trace_med_flg)) \
        MSG_2(MSG_SSID_TDSCDMA_MAC, LEVEL, FMT, A, B)

#define TDSMAC_MSG3(LEVEL, FMT, A, B, C) \
  if (LEVEL==MSG_LEGACY_FATAL||LEVEL==MSG_LEGACY_ERROR||(LEVEL==MSG_LEGACY_HIGH&&tds_nv_l2_trace_high_flg)|| \
       ((LEVEL==MSG_LEGACY_MED||LEVEL==MSG_LEGACY_LOW)&&tds_nv_l2_trace_med_flg)) \
        MSG_3(MSG_SSID_TDSCDMA_MAC, LEVEL, FMT, A, B, C)

#define TDSMAC_MSG4(LEVEL, FMT, A, B, C, D) \
  if (LEVEL==MSG_LEGACY_FATAL||LEVEL==MSG_LEGACY_ERROR||(LEVEL==MSG_LEGACY_HIGH&&tds_nv_l2_trace_high_flg)|| \
       ((LEVEL==MSG_LEGACY_MED||LEVEL==MSG_LEGACY_LOW)&&tds_nv_l2_trace_med_flg)) \
        MSG_4(MSG_SSID_TDSCDMA_MAC, LEVEL, FMT, A, B, C, D)

#define TDSMAC_MSG5(LEVEL, FMT, A, B, C, D, E) \
  if (LEVEL==MSG_LEGACY_FATAL||LEVEL==MSG_LEGACY_ERROR||(LEVEL==MSG_LEGACY_HIGH&&tds_nv_l2_trace_high_flg)|| \
       ((LEVEL==MSG_LEGACY_MED||LEVEL==MSG_LEGACY_LOW)&&tds_nv_l2_trace_med_flg)) \
        MSG_5(MSG_SSID_TDSCDMA_MAC, LEVEL, FMT, A, B, C, D, E)

#define TDSMAC_MSG6(LEVEL, FMT, A, B, C, D, E, F) \
  if (LEVEL==MSG_LEGACY_FATAL||LEVEL==MSG_LEGACY_ERROR||(LEVEL==MSG_LEGACY_HIGH&&tds_nv_l2_trace_high_flg)|| \
       ((LEVEL==MSG_LEGACY_MED||LEVEL==MSG_LEGACY_LOW)&&tds_nv_l2_trace_med_flg)) \
        MSG_6(MSG_SSID_TDSCDMA_MAC, LEVEL, FMT, A, B, C, D, E, F)

/***************************************************************************/

#define TDSL2_MSG0(LEVEL, FMT) \
  if (LEVEL==MSG_LEGACY_FATAL||LEVEL==MSG_LEGACY_ERROR||(LEVEL==MSG_LEGACY_HIGH&&tds_nv_l2_trace_high_flg)|| \
       ((LEVEL==MSG_LEGACY_MED||LEVEL==MSG_LEGACY_LOW)&&tds_nv_l2_trace_med_flg)) \
        MSG(MSG_SSID_TDSCDMA_L2, LEVEL, FMT)

#define TDSL2_MSG1(LEVEL, FMT, A) \
  if (LEVEL==MSG_LEGACY_FATAL||LEVEL==MSG_LEGACY_ERROR||(LEVEL==MSG_LEGACY_HIGH&&tds_nv_l2_trace_high_flg)|| \
       ((LEVEL==MSG_LEGACY_MED||LEVEL==MSG_LEGACY_LOW)&&tds_nv_l2_trace_med_flg)) \
        MSG_1(MSG_SSID_TDSCDMA_L2, LEVEL, FMT, A)

#define TDSL2_MSG2(LEVEL, FMT, A, B) \
  if (LEVEL==MSG_LEGACY_FATAL||LEVEL==MSG_LEGACY_ERROR||(LEVEL==MSG_LEGACY_HIGH&&tds_nv_l2_trace_high_flg)|| \
       ((LEVEL==MSG_LEGACY_MED||LEVEL==MSG_LEGACY_LOW)&&tds_nv_l2_trace_med_flg)) \
        MSG_2(MSG_SSID_TDSCDMA_L2, LEVEL, FMT, A, B)

#define TDSL2_MSG3(LEVEL, FMT, A, B, C) \
  if (LEVEL==MSG_LEGACY_FATAL||LEVEL==MSG_LEGACY_ERROR||(LEVEL==MSG_LEGACY_HIGH&&tds_nv_l2_trace_high_flg)|| \
       ((LEVEL==MSG_LEGACY_MED||LEVEL==MSG_LEGACY_LOW)&&tds_nv_l2_trace_med_flg)) \
        MSG_3(MSG_SSID_TDSCDMA_L2, LEVEL, FMT, A, B, C)

#define TDSL2_MSG4(LEVEL, FMT, A, B, C, D) \
  if (LEVEL==MSG_LEGACY_FATAL||LEVEL==MSG_LEGACY_ERROR||(LEVEL==MSG_LEGACY_HIGH&&tds_nv_l2_trace_high_flg)|| \
       ((LEVEL==MSG_LEGACY_MED||LEVEL==MSG_LEGACY_LOW)&&tds_nv_l2_trace_med_flg)) \
        MSG_4(MSG_SSID_TDSCDMA_L2, LEVEL, FMT, A, B, C, D)

#define TDSL2_MSG5(LEVEL, FMT, A, B, C, D, E) \
  if (LEVEL==MSG_LEGACY_FATAL||LEVEL==MSG_LEGACY_ERROR||(LEVEL==MSG_LEGACY_HIGH&&tds_nv_l2_trace_high_flg)|| \
       ((LEVEL==MSG_LEGACY_MED||LEVEL==MSG_LEGACY_LOW)&&tds_nv_l2_trace_med_flg)) \
        MSG_5(MSG_SSID_TDSCDMA_L2, LEVEL, FMT, A, B, C, D, E)

#define TDSL2_MSG6(LEVEL, FMT, A, B, C, D, E, F) \
  if (LEVEL==MSG_LEGACY_FATAL||LEVEL==MSG_LEGACY_ERROR||(LEVEL==MSG_LEGACY_HIGH&&tds_nv_l2_trace_high_flg)|| \
       ((LEVEL==MSG_LEGACY_MED||LEVEL==MSG_LEGACY_LOW)&&tds_nv_l2_trace_med_flg)) \
        MSG_6(MSG_SSID_TDSCDMA_L2, LEVEL, FMT, A, B, C, D, E, F)

/***************************************************************************/
#if 0 // need to sync up with tdsl1utils.h
#define TDSL1_MSG0(LEVEL, FMT) \
        MSG(MSG_SSID_TDSCDMA_L1, LEVEL, FMT)

#define TDSL1_MSG1(LEVEL, FMT, A) \
        MSG_1(MSG_SSID_TDSCDMA_L1, LEVEL, FMT, A)

#define TDSL1_MSG2(LEVEL, FMT, A, B) \
        MSG_2(MSG_SSID_TDSCDMA_L1, LEVEL, FMT, A, B)

#define TDSL1_MSG3(LEVEL, FMT, A, B, C) \
        MSG_3(MSG_SSID_TDSCDMA_L1, LEVEL, FMT, A, B, C)

#define TDSL1_MSG4(LEVEL, FMT, A, B, C, D) \
        MSG_4(MSG_SSID_TDSCDMA_L1, LEVEL, FMT, A, B, C, D)

#define TDSL1_MSG5(LEVEL, FMT, A, B, C, D, E) \
        MSG_5(MSG_SSID_TDSCDMA_L1, LEVEL, FMT, A, B, C, D, E)

#define TDSL1_MSG6(LEVEL, FMT, A, B, C, D, E, F) \
        MSG_6(MSG_SSID_TDSCDMA_L1, LEVEL, FMT, A, B, C, D, E, F)

#define TDSL1_MSG7(LEVEL, FMT, A, B, C, D, E, F, G) \
        MSG_7(MSG_SSID_TDSCDMA_L1, LEVEL, FMT, A, B, C, D, E, F, G)

#define TDSL1_MSG8(LEVEL, FMT, A, B, C, D, E, F, G, H) \
        MSG_8(MSG_SSID_TDSCDMA_L1, LEVEL, FMT, A, B, C, D, E, F, G, H)

#define TDSL1_MSG9(LEVEL, FMT, A, B, C, D, E, F, G, H, I) \
        MSG_9(MSG_SSID_TDSCDMA_L1, LEVEL, FMT, A, B, C, D, E, F, G, H, I)
#endif

#endif /* LOG_MSGS_TDSCDMA_H */


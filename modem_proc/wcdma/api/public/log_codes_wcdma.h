#ifndef LOG_CODES_WCDMA_H
#define LOG_CODES_WCDMA_H

/*===========================================================================

                         Log Code Definitions

General Description
  This file contains log code definitions and is shared with the tools.

Copyright (c) 1991-2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                             Edit History

$PVCSPath:  L:/src/asw/MSM5200/common/vcs/log_codes_wcdma.h_v   1.3   28 Jun 2002 14:31:04   tliou  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/api/public/log_codes_wcdma.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/22/09   sup     Moved all the WCDMA log code assignments to 
                   log_codes_wcdma_int.h
09/10/09   rmsd    Added 2 new codes.
09/09/09   rmsd    Mainlined feature FEATURE_RLC_LOG_MORE_AM_PDUS.
05/21/07   bd      Removed FEATURE_QVPHONE featurization per QVP team's direction
10/09/06   ttl     Put new log codes for LOG_WCDMA_UL_RLC_AM_USR_PLANE_PDU &
                   LOG_WCDMA_UL_RLC_AM_SIG_PLANE_PDU under
                   FEATURE_RLC_LOG_MORE_AM_PDUS.
09/22/06   ssg     Changed log codes for LOG_WCDMA_UL_RLC_AM_USR_PLANE_PDU &
                   LOG_WCDMA_UL_RLC_AM_SIG_PLANE_PDU.
08/30/04   jac     added some RLC log codes
08/11/03   asr     Changed LOG_WCDMA_DRX_MODE_C to be offset by 0x004 instead of 0x107
                   from LOG_WCDMA_BASE_C. 
06/09/03   jac     Added QVPHONE log packets
05/08/03   gsc     Added LOG_WCDMA_L1_CM_BLER_C.
12/11/02   gsc     Synced up with Rev. K of the Serial Interface Control
                   Document for W-CDMA (80-V2708-1). 
09/04/02   gsc     Synced up with Rev. H of the Serial Interface Control 
                   Document for W-CDMA (80-V2708-1). 
06/28/02   ttl     Added LOG_WCDMA_DL_AM_PDU, LOG_WCDMA_UL_NAK_AM_PDU and
                   LOG_WCDMA_DL_NAK_AM_PDU.
                   Changed LOG_WCDMA_UL_AM_STATISTIC, LOG_WCDMA_DL_AM_STATISTIC,
                   LOG_WCDMA_UL_AM_PDU and LOG_WCDMA_LAST_C.
05/17/02   gsc     Synced up with Rev. E of the Serial Interface Control 
                   Document for W-CDMA (80-V2708-1). 
04/23/02   ttl     Added LOG_WCDMA_UL_AM_STATISTIC, LOG_WCDMA_DL_AM_STATISTIC
                   and LOG_WCDMA_UL_AM_PDU.
02/12/02   ATM     Created log codes used by WCDMA

===========================================================================*/

#include "log_codes.h" //for LOG_WCDMA_BASE_C

#define LOG_H324_DL_223_PDU                          (0x900 + LOG_WCDMA_BASE_C) /* 0x4900 H324 Downlink 223 MUX PDU */
#define LOG_H324_DL_AUD_SDU                          (0x901 + LOG_WCDMA_BASE_C) /* 0x4901 H324 Downlink Audio SDU */
#define LOG_H324_DL_VID_SDU                          (0x902 + LOG_WCDMA_BASE_C) /* 0x4902 H324 Downlink Video SDU*/
#define LOG_H324_DL_SIG_SDU                          (0x903 + LOG_WCDMA_BASE_C) /* 0x4903 H324 Downlink 245 SDU */
#define LOG_H324_UL_223_PDU                          (0x904 + LOG_WCDMA_BASE_C) /* 0x4904 H324 Uplink 223 MUX PDU */
#define LOG_H324_UL_AUD_SDU                          (0x905 + LOG_WCDMA_BASE_C) /* 0x4905 H324 Uplink Audio SDU */
#define LOG_H324_UL_VID_SDU                          (0x906 + LOG_WCDMA_BASE_C) /* 0x4906 H324 Uplink Video SDU */
#define LOG_H324_UL_SIG_SDU                          (0x907 + LOG_WCDMA_BASE_C) /* 0x4907 H324 Uplink Video SDU */
#define LOG_H324_UL_LL_SIG                           (0x908 + LOG_WCDMA_BASE_C) /* 0x4908 H324 Uplink Adaptation Layer */
#define LOG_H324_DL_LL_SIG                           (0x909 + LOG_WCDMA_BASE_C) /* 0x4909 H324 Downlink Adaptation Layer */
#define LOG_H324_VID_STAT                            (0x90A + LOG_WCDMA_BASE_C) /* 0x490A H324 Video Statistics */
#define LOG_H324_VID_ENCODER                         (0x90B + LOG_WCDMA_BASE_C) /* 0x490B H324 Video ENCODER */
#define LOG_H324_VID_DECODER                         (0x90C + LOG_WCDMA_BASE_C) /* 0x490C H324 Video DECODER */
#define LOG_H324_UL_AUD_TS                           (0x90D + LOG_WCDMA_BASE_C) /* 0x490D H324 Uplink Audio Time Stamp and Seq. Number */
#define LOG_H324_DL_AUD_TS                           (0x90E + LOG_WCDMA_BASE_C) /* 0x490E H324 Downlink Audio Time Stamp and Seq. Number */
#define LOG_H324_UL_VID_TS                           (0x90F + LOG_WCDMA_BASE_C) /* 0x490F H324 Uplink Video Time Stamp and Seq. Number */
#define LOG_H324_DL_VID_TS                           (0x910 + LOG_WCDMA_BASE_C) /* 0x4910 H324 Downlink Video Time Stamp and Seq. Number */
/* The last defined WCDMA log code */
#define LOG_WCDMA_LAST_C                                 (0x910 + LOG_WCDMA_BASE_C)

#endif /* LOG_CODES_WCDMA_H */


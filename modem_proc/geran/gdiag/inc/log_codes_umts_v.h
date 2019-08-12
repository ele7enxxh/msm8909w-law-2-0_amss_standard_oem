#ifndef LOG_CODES_UMTS_V_H
#define LOG_CODES_UMTS_V_H
/*===========================================================================

                         Log Code Definitions

General Description
  This file contains log code definitions and is shared with the tools.

Copyright (c) 1991-2013 Qualcomm Technologies, Inc.
===========================================================================*/
/*===========================================================================

                             Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gdiag/inc/log_codes_umts_v.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/20/04   up      Added LOG_UMTS_NAS_FPLMN_LIST_UPDATE_LOG_PACKET_C,
                   LOG_UMTS_NAS_FPLMN_LIST_LOG_PACKET_C,
                   LOG_UMTS_NAS_HPLMN_SEARCH_TIMER_START_LOG_PACKET_C,
                   LOG_UMTS_NAS_HPLMN_SEARCH_TIMER_EXPIRE_LOG_PACKET_C,
                   LOG_UMTS_NAS_HPLMN_SEARCH_START_LOG_PACKET_C and
                   LOG_UMTS_NAS_HPLMN_SEARCH_END_LOG_PACKET_C for creating
                   log packet for FPLMN list update, FPLMN list, HPLMN search
                   timer start/expiry and HPLMN search start/end respectively.
09/29/04   KA      Added log packet for reporting location based service events
08/30/04   GK      Added log packets for PDAPI related LSM logs
08/16/04   AR      Relocate CS Data packets from local definitions
08/05/04   ymc     Added LOG_UMTS_TX_VOCODER_PACKET_DSP_C and
                   LOG_UMTS_RX_VOCODER_PACKET_DSP_C.
           ymc     Updated copyright.
06/03/04   GK      Adeed log packets for PDAPI related LSM logs
04/30/04   KWA     Added LOG_UMTS_NAS_EPLMN_LIST_LOG_PACKET_C
10/12/03   KA      Added log packets for call manager's use (USSD, passwd)
10/07/02   ATM     Moving CFA filtering from subsys cmd to log code range
07/12/02   gsc     Merged MMGPS changes by Ricky Yuen.
02/04/02   ATM     Cleanup -- renamed codes to fit convention
01/24/02   ATM     Created log codes used by UMTS
01/24/05   KGJ     Allocated 0x1E through 0x2F for MMGPS
===========================================================================*/

#include "log_codes.h" // for LOG_UMTS_BASE_C
#include "log_codes_umts.h"

/* NOTE: Steve is planning on taking all the UMTS/GSM code definitions out */
/*       of log_codes.h. Since this address space will be controlled by    */
/*       this VU, let's define the codes here                              */

/* ========================   MMGPS PACKETS   ============================ */
#define LOG_UMTS_MMGPS_REP_CLK_LOG_PACKET_C          (0x02 + LOG_UMTS_BASE_C)
#define LOG_UMTS_MMGPS_REP_POS_LOG_PACKET_C          (0x03 + LOG_UMTS_BASE_C)
#define LOG_UMTS_MMGPS_REP_MEAS_LOG_PACKET_C         (0x05 + LOG_UMTS_BASE_C)
#define LOG_UMTS_MMGPS_REP_RESERVED0_LOG_PACKET_C    (0x07 + LOG_UMTS_BASE_C)
#define LOG_UMTS_MMGPS_REP_RF_STATUS_LOG_PACKET_C    (0x08 + LOG_UMTS_BASE_C)
#define LOG_UMTS_MMGPS_REP_FREQ_CALIBRATION_LOG_PACKET_C \
                                                     (0x09 + LOG_UMTS_BASE_C)
#define LOG_UMTS_MMGPS_REP_SVR_TX_LOG_PACKET_C       (0x0A + LOG_UMTS_BASE_C)
#define LOG_UMTS_MMGPS_REP_SVR_RX_LOG_PACKET_C       (0x0B + LOG_UMTS_BASE_C)
#define LOG_UMTS_MMGPS_REP_SV_MEAS_LOG_PACKET_C      (0x0C + LOG_UMTS_BASE_C)
#define LOG_UMTS_MMGPS_REP_DWELL_LOG_PACKET_C        (0x0D + LOG_UMTS_BASE_C)
#define LOG_UMTS_MMGPS_REP_TIME_TAG_LOG_PACKET_C     (0x0E + LOG_UMTS_BASE_C)
#define LOG_UMTS_MMGPS_REP_RESERVED1_LOG_PACKET_C    (0x0F + LOG_UMTS_BASE_C)

#define LOG_UMTS_MMGPS_REP_GPS_INTFC_TX_LOG_PACKET_C (0x10 + LOG_UMTS_BASE_C)
#define LOG_UMTS_MMGPS_REP_GPS_INTFC_RX_LOG_PACKET_C (0x11 + LOG_UMTS_BASE_C)
#define LOG_UMTS_MMGPS_REP_EVT_LOG_PACKET_C          (0x12 + LOG_UMTS_BASE_C)

#define LOG_UMTS_PDSM_GET_POSITION_C                 (0x13 + LOG_UMTS_BASE_C)
#define LOG_UMTS_PDSM_SET_PARAMETERS_C               (0x14 + LOG_UMTS_BASE_C)
#define LOG_UMTS_PDSM_END_SESSION_C                  (0x15 + LOG_UMTS_BASE_C)
#define LOG_UMTS_PDSM_NOTIFY_VERIFY_RESP_C           (0x16 + LOG_UMTS_BASE_C)

#define LOG_UMTS_PDSM_POSITION_REPORT_CALLBACK_C     (0x17 + LOG_UMTS_BASE_C)
#define LOG_UMTS_PDSM_PD_EVENT_CALLBACK_C            (0x18 + LOG_UMTS_BASE_C)
#define LOG_UMTS_PDSM_PA_EVENT_CALLBACK_C            (0x19 + LOG_UMTS_BASE_C)
#define LOG_UMTS_PDSM_NOTIFY_VERIFY_REQUEST_C        (0x1A + LOG_UMTS_BASE_C)
#define LOG_UMTS_PDSM_PD_CMD_ERR_CALLBACK_C          (0x1B + LOG_UMTS_BASE_C)
#define LOG_UMTS_PDSM_PA_CMD_ERR_CALLBACK_C          (0x1C + LOG_UMTS_BASE_C)
#define LOG_UMTS_MMGPS_REP_UNC_ELLIPSE_LOG_PACKET_C  (0x1D + LOG_UMTS_BASE_C)
#define LOG_UMTS_MMGPS_REP_POS_TYPE2_LOG_PACKET_C    (0x1E + LOG_UMTS_BASE_C)
#define LOG_UMTS_MMGPS_REP_MEAS_TYPE3_LOG_PACKET_C   (0x1F + LOG_UMTS_BASE_C)
#define LOG_UMTS_MMGPS_PDSM_PD_DIAG_POS_REPORT_C     (0x20 + LOG_UMTS_BASE_C)
#define LOG_UMTS_PDSM_EXT_STATUS_NMEA_REPORT_C       (0x21 + LOG_UMTS_BASE_C)
#define LOG_UMTS_PDSM_EXT_STATUS_MEAS_REPORT_C       (0x22 + LOG_UMTS_BASE_C)
#define LOG_UMTS_PDSM_POS_ERROR_C                    (0x23 + LOG_UMTS_BASE_C)
#define LOG_UMTS_MMGPS_RESERVED_24                   (0x24 + LOG_UMTS_BASE_C)
#define LOG_UMTS_MMGPS_RESERVED_25                   (0x25 + LOG_UMTS_BASE_C)
#define LOG_UMTS_MMGPS_RESERVED_26                   (0x26 + LOG_UMTS_BASE_C)
#define LOG_UMTS_MMGPS_RESERVED_27                   (0x27 + LOG_UMTS_BASE_C)
#define LOG_UMTS_MMGPS_RESERVED_28                   (0x28 + LOG_UMTS_BASE_C)
#define LOG_UMTS_MMGPS_RESERVED_29                   (0x29 + LOG_UMTS_BASE_C)
#define LOG_UMTS_MMGPS_RESERVED_2A                   (0x2A + LOG_UMTS_BASE_C)
#define LOG_UMTS_MMGPS_RESERVED_2B                   (0x2B + LOG_UMTS_BASE_C)
#define LOG_UMTS_MMGPS_RESERVED_2C                   (0x2C + LOG_UMTS_BASE_C)
#define LOG_UMTS_MMGPS_RESERVED_2D                   (0x2D + LOG_UMTS_BASE_C)
#define LOG_UMTS_MMGPS_RESERVED_2E                   (0x2E + LOG_UMTS_BASE_C)
#define LOG_UMTS_MMGPS_RESERVED_2F                   (0x2F + LOG_UMTS_BASE_C)
#define LOG_UMTS_CFA_LAST_FILTER_C                  (0x2FF + LOG_UMTS_BASE_C)
#define LOG_UMTS_USS_DATA_CONF                      (0x402 + LOG_UMTS_BASE_C)
#define LOG_UMTS_USS_FWD_AOC                        (0x403 + LOG_UMTS_BASE_C)
#define LOG_LCS_MOLR_CONF_INFO                      (0x405 + LOG_UMTS_BASE_C)
#endif /* LOG_CODES_UMTS_V_H */

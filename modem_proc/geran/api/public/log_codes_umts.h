#ifndef LOG_CODES_UMTS_H
#define LOG_CODES_UMTS_H

/*===========================================================================

                         Log Code Definitions

General Description
  This file contains log code definitions and is shared with the tools.

Copyright (c) 2009-2016 Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                             Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/public/log_codes_umts.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
16-01-11   sjw     Added new NAS packet IDs
10-11-09   sjw     Added NAS support for Dual SIM log packet IDs
09-02-19   tjw     Automatically split from original header file and then tidied.
===========================================================================*/
#include "log_codes.h" // for LOG_UMTS_BASE_C

/* ========================   Call flow analysis  ======================== */
#define LOG_UMTS_CALL_FLOW_ANALYSIS_C               (0x01  + LOG_UMTS_BASE_C)

/* ========================   NAS LAYER PACKETS   ======================== */
#define LOG_UMTS_NAS_GMM_STATE_LOG_PACKET_C         (0x130 + LOG_UMTS_BASE_C)
#define LOG_UMTS_NAS_MM_STATE_LOG_PACKET_C          (0x131 + LOG_UMTS_BASE_C)
#define LOG_UMTS_NAS_REG_LOG_PACKET_C               (0x132 + LOG_UMTS_BASE_C)
#define LOG_UMTS_CS_CALL_SETUP_INFO_LOG_PACKET_C    (0x133 + LOG_UMTS_BASE_C)
#define LOG_UMTS_PS_CALL_INFO_LOG_PACKET_C          (0x134 + LOG_UMTS_BASE_C)
#define LOG_UMTS_MM_INFO_LOG_PACKET_C               (0x135 + LOG_UMTS_BASE_C)
#define LOG_UMTS_NAS_PS_CONNECTION_QOS_LOG_PACKET_C (0x136 + LOG_UMTS_BASE_C)
#define LOG_UMTS_NAS_CS_CONNECTION_BC_LOG_PACKET_C  (0x137 + LOG_UMTS_BASE_C)
#define LOG_UMTS_NAS_UE_DYNAMIC_ID_LOG_PACKET_C     (0x138 + LOG_UMTS_BASE_C)
#define LOG_UMTS_NAS_UE_STATIC_ID_LOG_PACKET_C      (0x139 + LOG_UMTS_BASE_C)
#define LOG_UMTS_NAS_OTA_MESSAGE_LOG_PACKET_C       (0x13A + LOG_UMTS_BASE_C)
#define LOG_UMTS_NAS_CFA_MESSAGE_LOG_PACKET_C       (0x13B + LOG_UMTS_BASE_C)
#define LOG_UMTS_NAS_ERROR_MESSAGE_LOG_PACKET_C     (0x13C + LOG_UMTS_BASE_C)
#define LOG_UMTS_CS_CALL_RELEASE_INFO_LOG_PACKET_C  (0x13D + LOG_UMTS_BASE_C)
#define LOG_UMTS_CS_CALL_CHANGE_INFO_LOG_PACKET_C   (0x13E + LOG_UMTS_BASE_C)
#define LOG_UMTS_NAS_EPLMN_LIST_LOG_PACKET_C        (0x150 + LOG_UMTS_BASE_C)
#define LOG_UMTS_NAS_FPLMN_LIST_UPDATE_LOG_PACKET_C (0x151 + LOG_UMTS_BASE_C)
#define LOG_UMTS_NAS_FPLMN_LIST_LOG_PACKET_C        (0x152 + LOG_UMTS_BASE_C)
#define LOG_UMTS_NAS_HPLMN_SEARCH_TIMER_START_LOG_PACKET_C  (0x153 + LOG_UMTS_BASE_C)
#define LOG_UMTS_NAS_HPLMN_SEARCH_TIMER_EXPIRE_LOG_PACKET_C (0x154 + LOG_UMTS_BASE_C)
#define LOG_UMTS_NAS_HPLMN_SEARCH_START_LOG_PACKET_C (0x155 + LOG_UMTS_BASE_C)
#define LOG_UMTS_NAS_HPLMN_SEARCH_END_LOG_PACKET_C   (0x156 + LOG_UMTS_BASE_C)
#define LOG_UMTS_NAS_CSG_WHITE_LIST_LOG_PACKET_C     (0x157 + LOG_UMTS_BASE_C)
#define LOG_UMTS_NAS_CSG_AVAILABLE_LIST_LOG_PACKET_C (0x158 + LOG_UMTS_BASE_C)
#define LOG_UMTS_NAS_CB_LOG_PACKET_C                 (0x159 + LOG_UMTS_BASE_C)
#define LOG_UMTS_NAS_PPLMN_LIST_LOG_PACKET_C         (0x160 + LOG_UMTS_BASE_C)

/* ======================== DUAL SIM NAS LAYER PACKETS   ======================== */
#define LOG_UMTS_DS_BASE_C  ((uint16) (LOG_UMTS_BASE_C + 0x0A00))

#define LOG_UMTS_DS_NAS_GMM_STATE_LOG_PACKET_C         (0x130 + LOG_UMTS_DS_BASE_C)
#define LOG_UMTS_DS_NAS_MM_STATE_LOG_PACKET_C          (0x131 + LOG_UMTS_DS_BASE_C)
#define LOG_UMTS_DS_NAS_REG_LOG_PACKET_C               (0x132 + LOG_UMTS_DS_BASE_C)
#define LOG_UMTS_DS_CS_CALL_SETUP_INFO_LOG_PACKET_C    (0x133 + LOG_UMTS_DS_BASE_C)
#define LOG_UMTS_DS_PS_CALL_INFO_LOG_PACKET_C          (0x134 + LOG_UMTS_DS_BASE_C)
#define LOG_UMTS_DS_MM_INFO_LOG_PACKET_C               (0x135 + LOG_UMTS_DS_BASE_C)
#define LOG_UMTS_DS_NAS_PS_CONNECTION_QOS_LOG_PACKET_C (0x136 + LOG_UMTS_DS_BASE_C)
#define LOG_UMTS_DS_NAS_CS_CONNECTION_BC_LOG_PACKET_C  (0x137 + LOG_UMTS_DS_BASE_C)
#define LOG_UMTS_DS_NAS_UE_DYNAMIC_ID_LOG_PACKET_C     (0x138 + LOG_UMTS_DS_BASE_C)
#define LOG_UMTS_DS_NAS_UE_STATIC_ID_LOG_PACKET_C      (0x139 + LOG_UMTS_DS_BASE_C)
#define LOG_UMTS_DS_NAS_OTA_MESSAGE_LOG_PACKET_C       (0x13A + LOG_UMTS_DS_BASE_C)
#define LOG_UMTS_DS_NAS_CFA_MESSAGE_LOG_PACKET_C       (0x13B + LOG_UMTS_DS_BASE_C)
#define LOG_UMTS_DS_NAS_ERROR_MESSAGE_LOG_PACKET_C     (0x13C + LOG_UMTS_DS_BASE_C)
#define LOG_UMTS_DS_CS_CALL_RELEASE_INFO_LOG_PACKET_C  (0x13D + LOG_UMTS_DS_BASE_C)
#define LOG_UMTS_DS_CS_CALL_CHANGE_INFO_LOG_PACKET_C   (0x13E + LOG_UMTS_DS_BASE_C)
#define LOG_UMTS_DS_NAS_EPLMN_LIST_LOG_PACKET_C        (0x150 + LOG_UMTS_DS_BASE_C)
#define LOG_UMTS_DS_NAS_FPLMN_LIST_UPDATE_LOG_PACKET_C (0x151 + LOG_UMTS_DS_BASE_C)
#define LOG_UMTS_DS_NAS_FPLMN_LIST_LOG_PACKET_C        (0x152 + LOG_UMTS_DS_BASE_C)
#define LOG_UMTS_DS_NAS_HPLMN_SEARCH_TIMER_START_LOG_PACKET_C  (0x153 + LOG_UMTS_DS_BASE_C)
#define LOG_UMTS_DS_NAS_HPLMN_SEARCH_TIMER_EXPIRE_LOG_PACKET_C (0x154 + LOG_UMTS_DS_BASE_C)
#define LOG_UMTS_DS_NAS_HPLMN_SEARCH_START_LOG_PACKET_C (0x155 + LOG_UMTS_DS_BASE_C)
#define LOG_UMTS_DS_NAS_HPLMN_SEARCH_END_LOG_PACKET_C   (0x156 + LOG_UMTS_DS_BASE_C)
#define LOG_UMTS_DS_NAS_CSG_WHITE_LIST_LOG_PACKET_C     (0x157 + LOG_UMTS_DS_BASE_C)
#define LOG_UMTS_DS_NAS_CSG_AVAILABLE_LIST_LOG_PACKET_C (0x158 + LOG_UMTS_DS_BASE_C)
#define LOG_UMTS_DS_NAS_CB_LOG_PACKET_C                 (0x159 + LOG_UMTS_DS_BASE_C)
#define LOG_UMTS_DS_NAS_PPLMN_LIST_LOG_PACKET_C         (0x160 + LOG_UMTS_DS_BASE_C)

/* ========================      AMR PACKETS     ========================= */
#define LOG_UMTS_TX_VOCODER_PACKET_C                (0x13F + LOG_UMTS_BASE_C)
#define LOG_UMTS_RX_VOCODER_PACKET_C                (0x140 + LOG_UMTS_BASE_C)
#define LOG_UMTS_TX_VOCODER_PACKET_DSP_C            (0x143 + LOG_UMTS_BASE_C)
#define LOG_UMTS_RX_VOCODER_PACKET_DSP_C            (0x144 + LOG_UMTS_BASE_C)

/* ========================   CS DATA PACKETS   ========================== */
#define LOG_UMTS_CS_DATA_RX_C                       (0x141 + LOG_UMTS_BASE_C)
#define LOG_UMTS_CS_DATA_TX_C                       (0x142 + LOG_UMTS_BASE_C)

/* ========================  CFA FILTER PACKETS  ========================= */
#define LOG_UMTS_CFA_FIRST_FILTER_C                 (0x200 + LOG_UMTS_BASE_C)

/* ========================  CM LOG PACKETS  ========================= */
#define LOG_UMTS_RESPONSE_NOTIFY                    (0x401 + LOG_UMTS_BASE_C)
#define LOG_GET_CALL_INFO                           (0x404 + LOG_UMTS_BASE_C)

/* The last defined UMTS log code */
#define LOG_UMTS_LAST_C                             (LOG_UMTS_DS_NAS_PPLMN_LIST_LOG_PACKET_C)

#endif /* LOG_CODES_UMTS_H */

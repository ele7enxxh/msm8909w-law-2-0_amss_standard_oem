/*!
  @file
  lte_log_codes.h

  @brief
  This file contains LTE log code definitions.

*/

/*=============================================================================

  Copyright (c) 2009-2013 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/lte_log_codes.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/14/15   mg      CR 779901: Embms DSDS DPD feature - Phase 2
12/18/14   mg      CR 768623: Embms DSDS DPD feature - Phase 1 
06/30/14   mg      CR 640102: Log packet of Number of packets with different 
                   delays in PDCP WMks for all bearers
06/06/13   ank     Add ML1 Sleep statistics log packet
07/25/12   dcw     Added LTE ML1 TDS Measurement results log packet
04/17/12   ru      CR 342859: Added eMBMS log codes
01/31/12   anm     eMBMS logpackets support
10/20/11   aa      Added Co-existence log packet support
13/10/11   anm     eMBMS monitor support
07/12/11   adi	   Added Inter Freq Meas log packet
05/25/11   tjc     Added PRS log packet codes
08/13/10   dx      Added TDD Logging Codes in ML1
08/13/10   dx      Added BPLMN Logging Codes
07/14/10   tdm     Added Band/System Scan log packet codes
08/11/10   vatsac  Added PLMN search log packets
06/30/10   ar      added PDCPDL/UL integrity log packet code
06/17/10   pa      Added sleep log code
05/05/10   nl      added ML1 Serving cell info. log code
04/30/10   bxwu    Added ML1->LL1 srch/meas log packets
04/30/10   bxwu    Added Cell Meas log packets for QXDM
04/28/10   bxwu    Added FFS/LFS log packets
03/22/10   ax      Added RLCDL profile, removed RLCDL AM/UM state Log packet
03/16/10   dk      Added GM RLM Report Log packet
12/04/09   ub      Added LL1 search/meas/pbch resp log packets
10/14/09   ub      Update idle log packet names
10/06/09   hnam    Added support for forbidden tracking are list (NAS)
10/05/09   ub      Updated idle log code names
07/15/09   anm     Name change for PDSCH result ind
07/02/09   ub      Updated idle log code names
06/22/09   fh      Changed Idle Log code names
06/04/09   MNK     Defined log codes for NAS
05/29/09   gsc     Defined/Updated log codes for ML1
05/07/09   anm     Added ML1-DL logging support
04/09/09   ar      added last log code macro for L2 and RRC
03/31/09   ar      Added RLC and PDCP log code macros
03/30/09   bn      initial version
=============================================================================*/

#ifndef LTE_LOG_CODES_H
#define LTE_LOG_CODES_H

/*=============================================================================

                   INCLUDE FILES

=============================================================================*/

#include <log_codes.h>

/*=============================================================================

                   EXTERNAL CONSTANT/MACRO DEFINITIONS

=============================================================================*/

/********************* START OF MAC LOG CODE DEFINES *************************/

/*! @brief LTE MAC based log code */
#define LOG_LTE_MAC_BASE_C                       (0x50 + LOG_LTE_BASE_C)

/*! @brief LTE MAC configure log code */
#define LOG_LTE_MAC_CONFIG_LOG_C                 (0x0 + LOG_LTE_MAC_BASE_C)

/*! @brief LTE MAC RACH TRIGGER log code */
#define LOG_LTE_MAC_RACH_TRIGGER_LOG_C           (0x1 + LOG_LTE_MAC_BASE_C)

/*! @brief LTE MAC RACH ATTEMPT log code */
#define LOG_LTE_MAC_RACH_ATTEMPT_LOG_C           (0x2 + LOG_LTE_MAC_BASE_C)

/*! @brief LTE MAC DL TB log code */
#define LOG_LTE_MAC_DL_TB_LOG_C                  (0x3 + LOG_LTE_MAC_BASE_C)

/*! @brief LTE MAC UL TB log code */
#define LOG_LTE_MAC_UL_TB_LOG_C                  (0x4 + LOG_LTE_MAC_BASE_C)

/*! @brief LTE MAC QoS Internal code */
#define LOG_LTE_MAC_QOS_INT_LOG_C                (0x5 + LOG_LTE_MAC_BASE_C)

/*! @brief LTE MAC QoS Internal code */
#define LOG_LTE_MAC_BUFFER_STATUS_INT_LOG_C      (0x6 + LOG_LTE_MAC_BASE_C)

/*! @brief LTE MAC UL TX Statistics code */
#define LOG_LTE_MAC_TX_STATISTICS_LOG_C          (0x7 + LOG_LTE_MAC_BASE_C)

/*! @brief LTE MAC eMBMS Statistics code */
#define LOG_LTE_MAC_EMBMS_STATISTICS_LOG_C       (0x8 + LOG_LTE_MAC_BASE_C)

/*! @brief last log code of MAC */
#define LOG_LTE_MAC_LAST_C                       (0x1F + LOG_LTE_MAC_BASE_C)

/*********************** END OF MAC LOG CODE DEFINES *************************/


/********************* START OF RLCDL LOG CODE DEFINES ***********************/

/*! @brief LTE RLCDL log code base */
#define LOG_LTE_RLCDL_BASE_C                     (0x70 + LOG_LTE_BASE_C)

/*! @brief LTE RLCDL reserved log code */
#define LOG_LTE_RLCDL_RESERVED_LOG_C             (0x0 + LOG_LTE_RLCDL_BASE_C)

/*! @brief LTE RLCDL configuration log code */
#define LOG_LTE_RLCDL_CONFIG_LOG_C               (0x1 + LOG_LTE_RLCDL_BASE_C)

/*! @brief LTE RLCDL AM all pdu log code */
#define LOG_LTE_RLCDL_AM_ALL_PDU_LOG_C           (0x2 + LOG_LTE_RLCDL_BASE_C)

/*! @brief LTE RLCDL AM control pdu log code */
#define LOG_LTE_RLCDL_AM_CONTROL_PDU_LOG_C       (0x3 + LOG_LTE_RLCDL_BASE_C)

/*! @brief LTE RLCDL AM polling pdu log code */
#define LOG_LTE_RLCDL_AM_POLLING_PDU_LOG_C       (0x4 + LOG_LTE_RLCDL_BASE_C)

/*! @brief LTE RLCDL AM signaling pdu log code */
#define LOG_LTE_RLCDL_AM_SIGNALING_PDU_LOG_C     (0x5 + LOG_LTE_RLCDL_BASE_C)

/*! @brief LTE RLCDL UM data pdu log code */
#define LOG_LTE_RLCDL_UM_DATA_PDU_LOG_C          (0x6 + LOG_LTE_RLCDL_BASE_C)

/*! @brief LTE RLCDL statistics log code */
#define LOG_LTE_RLCDL_STATISTICS_LOG_C           (0x7 + LOG_LTE_RLCDL_BASE_C)

/*! @brief LTE RLCDL AM state log code */
#define LOG_LTE_RLCDL_PROFILE_LOG_C              (0x8 + LOG_LTE_RLCDL_BASE_C)

/*! @brief LTE RLCDL Unified log code*/
#define LOG_LTE_RLCDL_UNIFIED_LOG_C              (0x9 + LOG_LTE_RLCDL_BASE_C)

/*! @brief LTE RLCDL eMBMS STATISTICS log code*/
#define LOG_LTE_RLCDL_EMBMS_STATISTICS_LOG_C     (0xA + LOG_LTE_RLCDL_BASE_C)

/*! @brief LTE RLCDL eMBMS CONFIGURATION log code*/
#define LOG_LTE_RLCDL_EMBMS_CONFIG_LOG_C         (0xB + LOG_LTE_RLCDL_BASE_C)

/*! @brief LTE RLCDL eMBMS DSDS service log code*/
#define LOG_LTE_RLCDL_EMBMS_DSDS_SERV_LOG_C      (0xC + LOG_LTE_RLCDL_BASE_C)

/*! @brief LTE RLCDL eMBMS Deep Packet Inspection SDU log code*/
#define LOG_LTE_RLCDL_EMBMS_DPI_SDU_LOG_C       (0xD + LOG_LTE_RLCDL_BASE_C)

/*! @brief last log code of RLCDL */
#define LOG_LTE_RLCDL_LAST_C                     (0x0F + LOG_LTE_RLCDL_BASE_C)

/*********************** END OF RLCDL LOG CODE DEFINES ***********************/


/********************* START OF RLCUL LOG CODE DEFINES ***********************/

/*! @brief LTE RLCUL log code base */
#define LOG_LTE_RLCUL_BASE_C                     (0x80 + LOG_LTE_BASE_C)

/*! @brief LTE RLCUL reserved log code */
#define LOG_LTE_RLCUL_RESERVED_LOG_C             (0x0 + LOG_LTE_RLCUL_BASE_C)

/*! @brief LTE RLCUL configuration log code */
#define LOG_LTE_RLCUL_CONFIG_LOG_C               (0x1 + LOG_LTE_RLCUL_BASE_C)

/*! @brief LTE RLCUL AM all pdu log code */
#define LOG_LTE_RLCUL_AM_ALL_PDU_LOG_C           (0x2 + LOG_LTE_RLCUL_BASE_C)

/*! @brief LTE RLCUL AM control pdu log code */
#define LOG_LTE_RLCUL_AM_CONTROL_PDU_LOG_C       (0x3 + LOG_LTE_RLCUL_BASE_C)

/*! @brief LTE RLCUL AM polling pdu log code */
#define LOG_LTE_RLCUL_AM_POLLING_PDU_LOG_C       (0x4 + LOG_LTE_RLCUL_BASE_C)

/*! @brief LTE RLCUL AM signaling pdu log code */
#define LOG_LTE_RLCUL_AM_SIGNALING_PDU_LOG_C     (0x5 + LOG_LTE_RLCUL_BASE_C)

/*! @brief LTE RLCUL UM data pdu log code */
#define LOG_LTE_RLCUL_UM_DATA_PDU_LOG_C          (0x6 + LOG_LTE_RLCUL_BASE_C)

/*! @brief LTE RLCUL statistics log code */
#define LOG_LTE_RLCUL_STATISTICS_LOG_C           (0x7 + LOG_LTE_RLCUL_BASE_C)

/*! @brief LTE RLCUL AM state log code */
#define LOG_LTE_RLCUL_AM_STATE_LOG_C             (0x8 + LOG_LTE_RLCUL_BASE_C)

/*! @brief LTE RLCUL UM state log code */
#define LOG_LTE_RLCUL_UM_STATE_LOG_C             (0x9 + LOG_LTE_RLCUL_BASE_C)

/*! @brief last log code of RLCUL */
#define LOG_LTE_RLCUL_LAST_C                     (0x0F + LOG_LTE_RLCUL_BASE_C)

/*********************** END OF RLCUL LOG CODE DEFINES ***********************/


/********************* START OF PDCPDL LOG CODE DEFINES ***********************/

/*! @brief LTE PDCPDL log code base */
#define LOG_LTE_PDCPDL_BASE_C                    (0x90 + LOG_LTE_BASE_C)

/*! @brief LTE PDCPDL configuration log code */
#define LOG_LTE_PDCPDL_CONFIG_LOG_C              (0x0 + LOG_LTE_PDCPDL_BASE_C)

/*! @brief LTE PDCPDL data pdu log code */
#define LOG_LTE_PDCPDL_DATA_PDU_LOG_C            (0x1 + LOG_LTE_PDCPDL_BASE_C)

/*! @brief LTE PDCPDL control pdu log code */
#define LOG_LTE_PDCPDL_CONTROL_PDU_LOG_C         (0x2 + LOG_LTE_PDCPDL_BASE_C)

/*! @brief LTE PDCPDL data pdu with ciphering log code */
#define LOG_LTE_PDCPDL_CIPHER_DATA_PDU_LOG_C     (0x3 + LOG_LTE_PDCPDL_BASE_C)

/*! @brief LTE PDCPDL statistics log code */
#define LOG_LTE_PDCPDL_STATISTICS_LOG_C          (0x4 + LOG_LTE_PDCPDL_BASE_C)

/*! @brief LTE PDCPDL data pdu with integrity log code */
#define LOG_LTE_PDCPDL_INTEG_DATA_PDU_LOG_C      (0x5 + LOG_LTE_PDCPDL_BASE_C)

/*! @brief last log code of PDCPDL */
#define LOG_LTE_PDCPDL_LAST_C                    (0x0F + LOG_LTE_PDCPDL_BASE_C)

/*********************** END OF PDCPDL LOG CODE DEFINES ***********************/


/********************* START OF PDCPUL LOG CODE DEFINES ***********************/

/*! @brief LTE PDCPUL log code base */
#define LOG_LTE_PDCPUL_BASE_C                    (0xA0 + LOG_LTE_BASE_C)

/*! @brief LTE PDCPUL configuration log code */
#define LOG_LTE_PDCPUL_CONFIG_LOG_C              (0x0 + LOG_LTE_PDCPUL_BASE_C)

/*! @brief LTE PDCPUL data pdu log code */
#define LOG_LTE_PDCPUL_DATA_PDU_LOG_C            (0x1 + LOG_LTE_PDCPUL_BASE_C)

/*! @brief LTE PDCPUL control pdu log code */
#define LOG_LTE_PDCPUL_CONTROL_PDU_LOG_C         (0x2 + LOG_LTE_PDCPUL_BASE_C)

/*! @brief LTE PDCPUL data pdu with ciphering log code */
#define LOG_LTE_PDCPUL_CIPHER_DATA_PDU_LOG_C     (0x3 + LOG_LTE_PDCPUL_BASE_C)

/*! @brief LTE PDCPUL statistics log code */
#define LOG_LTE_PDCPUL_STATISTICS_LOG_C          (0x4 + LOG_LTE_PDCPUL_BASE_C)

/*! @brief LTE PDCPUL data pdu with integrity log code */
#define LOG_LTE_PDCPUL_INTEG_DATA_PDU_LOG_C      (0x5 + LOG_LTE_PDCPUL_BASE_C)

/*! @brief LTE PDCPUL statistics log code */
#define LOG_LTE_PDCPUL_DELAY_STATISTICS_LOG_C    (0x6 + LOG_LTE_PDCPUL_BASE_C)


/*! @brief last log code of PDCPUL */
#define LOG_LTE_PDCPUL_LAST_C                    (0x0F + LOG_LTE_PDCPUL_BASE_C)

/*********************** END OF PDCPUL LOG CODE DEFINES ***********************/


/********************* START OF RRC LOG CODE DEFINES **************************/

/*! @brief LTE RRC log code base */
#define LOG_LTE_RRC_BASE_C                       (0xB0 + LOG_LTE_BASE_C)

/*! @brief LTE RRC OTA message log code */
#define LOG_LTE_RRC_OTA_MSG_LOG_C                (0x0 + LOG_LTE_RRC_BASE_C)

/*! @brief LTE RRC MIB log code */
#define LOG_LTE_RRC_MIB_LOG_C                    (0x1 + LOG_LTE_RRC_BASE_C)

/*! @brief LTE RRC serv cell info log code */
#define LOG_LTE_RRC_SERV_CELL_INFO_LOG_C         (0x2 + LOG_LTE_RRC_BASE_C)

/*! @brief LTE RRC plmn search req log code */
#define LOG_LTE_RRC_PLMN_SEARCH_REQ_LOG_C        (0x3 + LOG_LTE_RRC_BASE_C)

/*! @brief LTE RRC plmn search rsp log code */
#define LOG_LTE_RRC_PLMN_SEARCH_RSP_LOG_C        (0x4 + LOG_LTE_RRC_BASE_C)

/*! @brief LTE RRC eMBMS bearer info log code */
#define LOG_LTE_RRC_EMBMS_LOG_C                  (0x6 + LOG_LTE_RRC_BASE_C)

/*! @brief LTE RRC eMBMS OOS warning indication log code */
#define LOG_LTE_RRC_EMBMS_OOS_WARN_IND_LOG_C     (0x7 + LOG_LTE_RRC_BASE_C)

/*! @brief LTE RRC Idle meas config log code */
#define LTE_RRC_LOG_IDLE_MEAS_CONFIG_LOG_C       (0x8 + LOG_LTE_RRC_BASE_C)

/*! @brief LTE RRC MDT logged meas info log code */
#define LOG_LTE_RRC_MDT_LOG_MEAS_CFG_LOG_C      (0x9 + LOG_LTE_RRC_BASE_C)

/*! @brief LTE RRC MDT logged meas cfg log code */
#define LOG_LTE_RRC_MDT_LOG_MEAS_INFO_LOG_C       (0xA + LOG_LTE_RRC_BASE_C)

/*! @brief LTE RRC Paging UE ID log code */
#define LTE_RRC_LOG_PAGING_UE_ID_LOG_C           (0xB + LOG_LTE_RRC_BASE_C)

/*! @brief LTE RRC CGI INFO log code */
#define LOG_LTE_RRC_CGI_INFO_LOG_C       (0xC + LOG_LTE_RRC_BASE_C)

/*! @brief last log code of RRC */
#define LOG_LTE_RRC_LAST_C                       (0x1F + LOG_LTE_RRC_BASE_C)

/*********************** END OF RRC LOG CODE DEFINES **************************/

/********************* START OF ML1 LOG CODE DEFINES **************************/
/*! @brief LTE ML1 log code base
    LOG_LTE_BASE_C = 0xB010
    LOG_LTE_ML1_BASE_C = 0xB010 + 0x150 = 0xB160

    DO NOT CHANGE LOG PACKETS' LOG CODES. USE NEW LOGCODES INSTEAD.
*/
#define LOG_LTE_ML1_BASE_C                       (0x150 + LOG_LTE_BASE_C)

/*! @brief LTE ML1 DLM common cfg log code */
#define LOG_LTE_ML1_DLM_COMN_CFG_LOG_C           (0x0 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 DLM dedicated cfg log code */
#define LOG_LTE_ML1_DLM_DED_CFG_LOG_C            (0x1 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 ULM common cfg log code */
#define LOG_LTE_ML1_ULM_COMN_CFG_LOG_C           (0x2 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 ULM dedicated cfg log code */
#define LOG_LTE_ML1_ULM_DED_CFG_LOG_C            (0x3 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 GM common cfg log code */
#define LOG_LTE_ML1_GM_COMN_CFG_LOG_C            (0x4 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 GM dedicated cfg log code */
#define LOG_LTE_ML1_GM_DED_CFG_LOG_C             (0x5 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 PRACH CFG log code */
#define LOG_LTE_ML1_ULM_PRACH_CFG_LOG_C          (0x6 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 RACH MSG1 log code */
#define LOG_LTE_ML1_ULM_RACH_MSG1_LOG_C          (0x7 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 RACH MSG2 log code */
#define LOG_LTE_ML1_ULM_RACH_MSG2_LOG_C          (0x8 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 RACH MSG3 log code */
#define LOG_LTE_ML1_GM_RACH_MSG3_LOG_C           (0x9 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 RACH MSG4 log code */
#define LOG_LTE_ML1_GM_RACH_MSG4_LOG_C           (0xA + LOG_LTE_ML1_BASE_C)

/*! @brief LTE GM PDCCH Info log code */
#define LOG_LTE_ML1_GM_PDCCH_PHICH_INFO_LOG_C    (0xB + LOG_LTE_ML1_BASE_C)

/*! @brief LTE GM Grant Info log code */
#define LOG_LTE_ML1_GM_DCI_INFO_LOG_C            (0xC + LOG_LTE_ML1_BASE_C)

/*! @brief LTE GM TX Report log code */
#define LOG_LTE_ML1_GM_TX_REPORT_LOG_C           (0xD + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 PUSCH Power Control log code */
#define LOG_LTE_ML1_GM_PUSCH_PWR_CNTRL_LOG_C     (0xE + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 PUCCH Power Control log code */
#define LOG_LTE_ML1_GM_PUCCH_PWR_CNTRL_LOG_C     (0xF + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 SRS TX Report log code */
#define LOG_LTE_ML1_GM_SRS_TX_REPORT_LOG_C       (0x10 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 SRS Power Control log code */
#define LOG_LTE_ML1_GM_SRS_PWR_CNTRL_LOG_C       (0x11 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 GM UL PKT Build log code */
#define LOG_LTE_ML1_GM_UL_PKT_BUILD_LOG_C        (0x12 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 DLM PDSCH STAT Ind log code */
#define LOG_LTE_ML1_DLM_PDSCH_STAT_IND_LOG_C     (0x13 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 SRCH List Freq Scan log code */
#define LOG_LTE_ML1_SRCH_LIST_FREQ_SCAN_LOG_C    (0x14 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 SRCH Full Freq Scan log code */
#define LOG_LTE_ML1_SRCH_FULL_FREQ_SCAN_LOG_C    (0x15 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 SRCH Initial Acquisition log code */
#define LOG_LTE_ML1_SRCH_INIT_ACQ_LOG_C          (0x16 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 SRCH MEAS CONFIG log code */
#define LOG_LTE_ML1_SRCH_MEAS_CFG_LOG_C          (0x17 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 SRCH MEAS Report log code */
#define LOG_LTE_ML1_SRCH_INTRA_SRCH_MEAS_CFG_LOG_C       (0x18 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 SRCH INTRA FREQ MEAS  log code */
#define LOG_LTE_ML1_SRCH_INTRA_FREQ_MEAS_LOG_C   (0x19 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 SRCH INTRA FREQ MEAS  log code */
#define LOG_LTE_ML1_SRCH_VMR_LOG_C         (0x31 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1  CELL SELECT log code */
#define LOG_LTE_ML1_S_CRITERIA_CHECK_LOG_C       (0x1A + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1  Common Config log code */
//#define LOG_LTE_ML1_COMN_CFG_LOG_C               (0x1B + LOG_LTE_ML1_BASE_C)
#define LOG_LTE_ML1_DRX_SERV_CELL_EVAL_LOG_C     (0x1B + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1  Paging DRX Req log code */
#define LOG_LTE_ML1_PAGING_DRX_INFO_LOG_C        (0x1C + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1  Idle Meas Req log code */
#define LOG_LTE_ML1_IDLE_MEAS_REQ_LOG_C          (0x1D + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1  UE Mobility State Info log code */
#define LOG_LTE_ML1_UE_MOB_STATE_INFO_LOG_C      (0x1E + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1  Serving Cell Measurement log code */
#define LOG_LTE_ML1_SERV_MEAS_EVAL_LOG_C         (0x1F + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1  NBR Cell Measurement log code */
#define LOG_LTE_ML1_NBR_CELL_MEAS_LOG_C          (0x20 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1  Cell Reselection log code */
#define LOG_LTE_ML1_CELL_RESEL_LOG_C             (0x21 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 Multisim log code */
#define LOG_LTE_ML1_MGR_TAM_LOG_C             (0x22 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 SRCH PBCH Decode log code */
#define LOG_LTE_ML1_PBCH_DECODE_LOG_C            (0x23 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1  UTRA NBR MEAS log code */
#define LOG_LTE_ML1_UTRA_NBR_MEAS_LOG_C          (0x24 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1  NBR Decode log code */
#define LOG_LTE_ML1_CDMA_NBR_MEAS_LOG_C          (0x25 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1  LTE Serving meas response log code */
//#define LOG_LTE_ML1_SERV_MEAS_RESP_LOG_C      (0x26 + LOG_LTE_ML1_BASE_C)
/*! @brief LTE ML1  LTE Serving meas response log code */
#define LOG_LTE_ML1_RESEL_CANDIDATES_LOG_C       (0x26 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1  LTE Neighbor search response log code */
//#define LOG_LTE_ML1_NGBR_SRCH_RESP_LOG_C      (0x27 + LOG_LTE_ML1_BASE_C)
/*! @brief LTE ML1  IRAT meas request log code */
#define LOG_LTE_ML1_IDLE_IRAT_MEAS_REQ_LOG_C     (0x27 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1  LTE Neighbor meas response log code */
//#define LOG_LTE_ML1_NGBR_MEAS_RESP_LOG_C      (0x28 + LOG_LTE_ML1_BASE_C)
/*! @brief LTE ML1  IRAT MDB log code */
#define LOG_LTE_ML1_IRAT_MDB_LOG_C               (0x28 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1  LTE PBCH decode response log code */
//#define LOG_LTE_ML1_PBCH_DECODE_RESP_LOG_C    (0x29 + LOG_LTE_ML1_BASE_C)
/*! @brief LTE ML1  CDMA system time log code */
#define LOG_LTE_ML1_CDMA_SYSTIME_LOG_C           (0x29 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 GM RLM log code */
#define LOG_LTE_ML1_GM_RLM_LOG_C                 (0x2A + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 Sleep log code */
#define LOG_LTE_ML1_SLEEPMGR_LOG_C               (0x2B + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 Sleep Statistics log code */
#define LOG_LTE_ML1_SLEEPMGR_STATS_LOG_C         (0x2C + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 Band Scan Results*/
#define LOG_LTE_ML1_BAND_SCAN_CNF_LOG_C             (0x2D + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 System Scan Results*/
#define LOG_LTE_ML1_SYS_SCAN_CNF_LOG_C              (0x2E + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 AdvRx IC list log code */
#define LOG_LTE_ML1_ADVRX_IC_LIST_LOG_C             (0x2F + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 AdvRx IC request log code */
#define LOG_LTE_ML1_ADVRX_IC_REQ_LOG_C              (0x30 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 NEIGHBOR CELL MEAS REQ/REQP LOG*/
#define LOG_LTE_ML1_IDLE_NEIGHBOR_CELL_MEAS_REQ_RESP_LOG_C (0x32 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 SERV CELL MEAS RESULTS LOG */
#define LOG_LTE_ML1_SERV_CELL_MEAS_RESPONSE_LOG_C          (0x33 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 NEIGHBOR CELL SEARCH REQ/REQP LOG*/
#define LOG_LTE_ML1_NEIGHBOR_CELL_SRCH_REQ_RESP_LOG_C      (0x34 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 CONNECTED NEIGHBOR CELL MEAS REQ/REQP LOG*/
#define LOG_LTE_ML1_CONN_NEIGHBOR_CELL_MEAS_REQ_RESP_LOG_C (0x35 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 CELL MEAS RESULTS log code */
#define LOG_LTE_ML1_CELL_MEAS_RESULTS_LOG_C       (0x36 + LOG_LTE_ML1_BASE_C)
/*! @brief LTE ML1 Serving cell info. log code */
#define LOG_LTE_ML1_SERV_CELL_INFO_LOG_C            (0x37 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 CDRX events info log code */
#define LOG_LTE_ML1_GM_CDRX_EVENTS_INFO_LOG_C       (0x38 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 Band/System Scan Results*/
#define LOG_LTE_ML1_BAND_AND_SYS_SCAN_CNF_LOG_C     (0x39 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 Macro Sleep log code */
#define LOG_LTE_ML1_MSMGR_LOG_C                     (0x3A + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 DC offset log code */
#define LOG_LTE_ML1_CONN_WCDMA_MEAS_RESULTS_LOG_C   (0x3B + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 DC offset log code */
#define LOG_LTE_ML1_CONN_MEAS_EVAL_LOG_C            (0x3C + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 DC offset log code */
#define LOG_LTE_ML1_GSM_MEAS_RESULTS_LOG_C          (0x3D + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 SRCH INTER FREQ MEAS  log code */
#define LOG_LTE_ML1_SRCH_INTER_FREQ_MEAS_LOG_C   (0x3E + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 Coex tx power report code*/
#define LOG_LTE_ML1_COEX_TX_PWR_LOG_C             (0x3F + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 BPLMN start request log code */
#define LOG_LTE_ML1_BPLMN_START_REQ_LOG_C           (0x40 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 BPLMN cell request log code */
#define LOG_LTE_ML1_BPLMN_CELL_REQ_LOG_C            (0x41 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 BPLMN cell confirm log code */
#define LOG_LTE_ML1_BPLMN_CELL_CONFIRM_LOG_C        (0x42 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 BPLMN time avail log code */
#define LOG_LTE_ML1_BPLMN_TIME_AVAIL_IND_LOG_C      (0x43 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 BPLMN complete ind log code */
#define LOG_LTE_ML1_BPLMN_COMPLETE_IND_LOG_C        (0x44 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 BPLMN stop request log code */
#define LOG_LTE_ML1_BPLMN_STOP_REQ_LOG_C            (0x45 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 BPLMN stop confirm log code */
#define LOG_LTE_ML1_BPLMN_STOP_CONFIRM_LOG_C        (0x46 + LOG_LTE_ML1_BASE_C)


/*! @brief LTE ML1 SCHDLR write buffer log code */
#define LOG_LTE_ML1_SCHDLR_WRITE_BUFFER_LOG_C       (0x47 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 DLM link capacity status log code */
#define LOG_LTE_ML1_DLM_LINK_CAP_STAT_LOG_C        (0x48 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 DLM link capacity status log code */
#define LOG_LTE_ML1_DLM_RES_AVAIL_FRAC_STAT_LOG_C        (0x49 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 TDD cfg log code */
#define LOG_LTE_ML1_GM_TDD_CFG_LOG_C                (0x50 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 TDD ack nak log code */
#define LOG_LTE_ML1_GM_TDD_ACK_NAK_LOG_C            (0x51 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 DC offset log code */
#define LOG_LTE_ML1_DC_OFFSET_LOG_C                 (0x52 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 PRS cell update log code */
#define LOG_LTE_ML1_PRS_CELL_UPDATE_LOG_C           (0x53 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 PRS cell hypotheses log code */
#define LOG_LTE_ML1_PRS_CELL_HYPOTHESES_LOG_C       (0x54 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 DLM PMCH decode params log code */
#define LOG_LTE_ML1_DLM_PMCH_DECODE_PARAMS_LOG_C    (0x55 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 STM profile log code */
#define LOG_LTE_ML1_STM_PROFILE_LOG_C               (0x56 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 frame timing log code */
#define LOG_LTE_ML1_COEX_FRAME_TIMING_C             (0x57 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 event indication log code */
#define LOG_LTE_ML1_COEX_EVENT_IND_C                (0x58 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 Coex state info log code */
#define LOG_LTE_ML1_COEX_STATE_INFO_C               (0x59 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 DLM MSI params log code */
#define LOG_LTE_ML1_DLM_MSI_PARAMS_LOG_C            (0x5A + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 DLM MBSFN config log code */
#define LOG_LTE_ML1_DLM_MBSFN_CFG_LOG_C             (0x5B + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 DLM PMCH results log code */
#define LOG_LTE_ML1_DLM_PMCH_RESULTS_LOG_C          (0x5C + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 DLM All DL decodes results log code */
#define LOG_LTE_ML1_DLM_DECODE_RESULTS_LOG_C        (0x5D + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 PMCH results statistics log code */
#define LOG_LTE_ML1_DLM_PMCH_STATS_LOG_C            (0x5E + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 Coex Indications log code */
#define LOG_LTE_ML1_COEX_INDICATIONS_C              (0x5F + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 Scheduler profile log code */
#define LOG_LTE_ML1_SCHDLR_PROFILE_LOG_C            (0x60 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 TDS Measurement results log code */
#define LOG_LTE_ML1_TDS_MEAS_RESULTS_LOG_C          (0x61 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 TDS ACQ request log code */
#define LOG_LTE_ML1_TDS_ACQ_REQ_LOG_C          (0x62 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 TDS MEAS request log code */
#define LOG_LTE_ML1_TDS_MEAS_REQ_LOG_C          (0x63 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 mcvs vpe freq log code */
#define LOG_LTE_ML1_MCVS_VPE_FREQ_LOG_C             (0x64 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 Scheduler profile log code */
#define LOG_LTE_ML1_CA_SCELL_CONFIG_LOG_C           (0x65 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 Coex dsda frequency msg log code*/
#define LOG_LTE_ML1_COEX_DSDA_FREQID_MAP_LOG_C      (0x67 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 Coex dsda conflict check msg log code*/
#define LOG_LTE_ML1_COEX_DSDA_CONFLICT_CHECK_LOG_C  (0x68 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 DLM BLER statistics log code */
#define LOG_LTE_ML1_DLM_BLER_STATS_LOG_C            (0x69 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 Time Transfer log code*/
#define LOG_LTE_ML1_IRAT_TIME_TRANSFER_LOG_C    (0x6A + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 Coex dsda conflict check msg log code*/
#define LOG_LTE_ML1_COEX_DSDA_MODULE_EVENT_LOG_C    (0x70 + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 Coex dsda priority switch change log code*/
#define LOG_LTE_ML1_COEX_DSDA_SWITCH_REQUEST_LOG_C    (0x6B + LOG_LTE_ML1_BASE_C)

/*! @brief LTE ML1 Coex dsda priority switch change sent to FW log code*/
#define LOG_LTE_ML1_COEX_DSDA_SWITCH_REQUEST_SEND_FW_LOG_C    (0x6C + LOG_LTE_ML1_BASE_C)

/*! @brief Total number of LTE ML1 log codes. This macro needs to be updated if more log codes are added  */
#define LOG_LTE_ML1_TOTAL_LOG_CODES                 (LOG_LTE_ML1_COEX_DSDA_MODULE_EVENT_LOG_C-LOG_LTE_ML1_BASE_C+1)

/*********************** END OF ML1 LOG CODE DEFINES **************************/


/********************* START OF LTE/NAS LOG CODE DEFINES **************************/

/*! @brief LTE NAS log code base */
#define LOG_LTE_NAS_BASE_C                            (0xD0 + LOG_LTE_BASE_C)

/*! @brief LTE NAS ESM security protected OTA incoming message log code */
#define LOG_LTE_NAS_ESM_SEC_OTA_IN_MSG_LOG_C          (0x0 + LOG_LTE_NAS_BASE_C)

/*! @brief LTE NAS ESM security protected OTA outgoing  log code */
#define LOG_LTE_NAS_ESM_SEC_OTA_OUT_MSG_LOG_C         (0x1 + LOG_LTE_NAS_BASE_C)

/*! @brief LTE NAS ESM plain OTA incoming  log code */
#define LOG_LTE_NAS_ESM_OTA_IN_MSG_LOG_C              (0x2 + LOG_LTE_NAS_BASE_C)

/*! @brief LTE NAS ESM plain OTA outgoing  log code */
#define LOG_LTE_NAS_ESM_OTA_OUT_MSG_LOG_C             (0x3 + LOG_LTE_NAS_BASE_C)

/*! @brief LTE NAS ESM bearer context state code */
#define LOG_LTE_NAS_ESM_BEARER_CONTEXT_STATE_LOG_C    (0x4 + LOG_LTE_NAS_BASE_C)

/*! @brief LTE NAS ESM BEARER CONTEXT INFO code */
#define LOG_LTE_NAS_ESM_BEARER_CONTEXT_INFO_LOG_C     (0x5 + LOG_LTE_NAS_BASE_C)

/*! @brief LTE NAS ESM PROCEDURE LOG code */
#define LOG_LTE_NAS_ESM_PROCEDURE_STATE_LOG_C         (0x6 + LOG_LTE_NAS_BASE_C)

/*! @brief LTE NAS EMM security protected OTA outgoing  log code */
#define LOG_LTE_NAS_EMM_SEC_OTA_IN_MSG_LOG_C           (0xA + LOG_LTE_NAS_BASE_C)

/*! @brief LTE NAS EMM security protected OTA outgoing  log code */
#define LOG_LTE_NAS_EMM_SEC_OTA_OUT_MSG_LOG_C          (0xB + LOG_LTE_NAS_BASE_C)

/*! @brief LTE NAS EMM Plain OTA outgoing message log code */
#define LOG_LTE_NAS_EMM_OTA_IN_MSG_LOG_C                (0xC + LOG_LTE_NAS_BASE_C)

/*! @brief LTE NAS EMM Plain OTA incoming log code */
#define LOG_LTE_NAS_EMM_OTA_OUT_MSG_LOG_C               (0xD + LOG_LTE_NAS_BASE_C)

/*! @brief LTE NAS EMM state log code */
#define LOG_LTE_NAS_EMM_STATE_LOG_C                     (0xE + LOG_LTE_NAS_BASE_C)

/*! @brief LTE NAS EMM USIM card mode log code */
#define LOG_LTE_NAS_EMM_USIM_CARD_MODE_LOG_C            (0xF + LOG_LTE_NAS_BASE_C)

/*! @brief LTE NAS EMM Current securuty context log code */
#define LOG_LTE_NAS_EMM_CURRENT_SECUIRY_CONTEXT_LOG_C   (0x12 + LOG_LTE_NAS_BASE_C)

/*! @brief LTE NAS EMM CKIK for PS domain log code */
#define LOG_LTE_NAS_EMM_CKIK_FOR_PS_DOMAIN_LOG_C        (0x13 + LOG_LTE_NAS_BASE_C)

/*! @brief LTE NAS EMM native security context log code */
#define LOG_LTE_NAS_EMM_NATIVE_SECURITY_CONTEXT_LOG_C   (0x14 + LOG_LTE_NAS_BASE_C)

/*! @brief LTE NAS EMM USIM service table log code */
#define LOG_LTE_NAS_EMM_USIM_SERVICE_TABLE_LOG_C        (0x15 + LOG_LTE_NAS_BASE_C)

/*! @brief LTE NAS EMM FORBIDDEN TRACKING AREA LIST */
#define LOG_LTE_NAS_EMM_FORBIDDEN_TRACKING_AREA_LIST_LOG_C (0x16 + LOG_LTE_NAS_BASE_C)

/*********************** END OF LTE/NAS LOG CODE DEFINES **************************/


/*! @brief last log code of LTE */
#define LOG_LTE_LAST_LOG_C                       (0x200 + LOG_LTE_BASE_C)

#endif /* LTE_LOG_CODES_H */


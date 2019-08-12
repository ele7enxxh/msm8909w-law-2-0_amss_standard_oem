/*!
  @file lte_rlc_log.h

  @brief
  Macros and defines for RLC specific diagnostic messages

  @author
  araina axiao

*/

/*==============================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/rlc/src/lte_rlc_log.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
01/14/15   mg      CR 779901: Embms DSDS DPD feature - Phase 2
12/18/14   mg      CR 768623: Embms DSDS DPD feature - Phase 1
04/17/12   ru      CR 342859: Added eMBMS logging changes  
12/16/11   ax      feature clean up 
06/27/10   ax      added status field to support pdu logging during flow control
05/19/10   ar      added PDCP ciphering log packet support
03/26/10   ax      removed macro definitions based on FEATURE_PROFILE
03/18/10   ax      added profile log pkt
09/02/09   ax      removed multiple invokation of LTE_ERROR in LTE_RLC_ERROR
06/03/09   ax      added log pkt no aggregate support and profiling support
05/01/09   ar      removed rb config sub pkt support
04/29/09   ax      start RLC log subpkt id from its designated base
04/17/09   ar      moved log_mask from cfg sub pkt to pdu log sub pkt
04/17/09   ax      update RLCDL logging according to the ICD
04/07/09   ar      Added log_mask to lte_rlc_log_sub_pkt_rb_cfg_s and updated the
                   sub pkt log code enumurations
03/30/09   ar      changed priority of custom message macros
03/26/09   ar      added payload structs for LTE_RLCUL_FLUSH_RLCUL_LOG_BUF_IND
                   and  LTE_RLCUL_FLUSH_PDCPUL_LOG_BUF_IND
03/19/09   ax      fix lint warnings
11/04/08   ax      remove ...MSG_ERROR..., use LTE_RLC_ERROR()
09/03/08   ar      intial version
==============================================================================*/

#ifndef LTE_RLC_LOG_H
#define LTE_RLC_LOG_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <msg.h>
#include "lte_rlc_rrc.h"
#include "lte_l2_comdef.h"
#include "lte_rlc_cfg.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*-------------------------------------------------------------------------
 *                     COMMON RLC MESSAGE MACROS                          *
 ------------------------------------------------------------------------*/

/*-----------------------RLC MESSAGE ERROR MACRO-------------------------*/

#define LTE_RLC_ERROR(fmt, a, b, c)                                       \
  MSG_3(MSG_SSID_LTE, MSG_LEGACY_FATAL,  fmt, a, b, c );                  \
  lte_rlc_log_error_handler()

/*-------------------------------------------------------------------------
 *                   RLCUL SPECIFIC MESSAGE MACROS                        *
 ------------------------------------------------------------------------*/

/*! @brief Mask for configuration UL related logging */
#define LTE_RLCUL_CONFIG (MSG_MASK_5 | MSG_LEGACY_HIGH)

/*! @brief Mask for messages related to UL poll or poll triggers */
#define LTE_RLCUL_POLLING (MSG_MASK_6 | MSG_LEGACY_MED)

/*! @brief Mask for logs related to inter-task message exchanges */
#define LTE_RLCUL_INTER_TASK_MSGS (MSG_MASK_7 | MSG_LEGACY_MED)

/*! @brief Mask for UL status related logging - control PDUs, status triggers
 *  etc */
#define LTE_RLCUL_STATUS (MSG_MASK_8 | MSG_LEGACY_MED)

/*! @brief Mask for messages related to UL state variable values etc */
#define LTE_RLCUL_DATA_PATH (MSG_MASK_9 | MSG_LEGACY_LOW)

/*! @brief Mask for future use */
#define LTE_RLCUL_DEBUG_RESERVED1 (MSG_MASK_10 | MSG_LEGACY_HIGH)

/*! @brief Mask for future use */
#define LTE_RLCUL_DEBUG_RESERVED2 (MSG_MASK_11 | MSG_LEGACY_HIGH)

/*-------------------------------------------------------------------------
 *                   RLCDL SPECIFIC MESSAGE MACROS                        *
 ------------------------------------------------------------------------*/

/*! @brief Mask for configuration DL related logging */
#define LTE_RLCDL_CONFIG (MSG_MASK_5 | MSG_LEGACY_HIGH)

/*! @brief Mask for messages related to received poll or poll triggers */
#define LTE_RLCDL_POLLING (MSG_MASK_6 | MSG_LEGACY_MED)

/*! @brief Mask for logs related to inter-task message exchanges */
#define LTE_RLCDL_INTER_TASK_MSGS (MSG_MASK_7 | MSG_LEGACY_MED)

/*! @brief Mask for DL status related logging - control PDUs, status triggers
 *  etc */
#define LTE_RLCDL_STATUS (MSG_MASK_8 | MSG_LEGACY_MED)

/*! @brief Mask for messages related to DL state variable values etc */
#define LTE_RLCDL_DATA_PATH (MSG_MASK_9 | MSG_LEGACY_LOW)

/*! @brief Mask for future use */
#define LTE_RLCDL_DEBUG_RESERVED1 MSG_MASK_10

/*! @brief Mask for future use */
#define LTE_RLCDL_DEBUG_RESERVED2 MSG_MASK_11

/*---------------compile in/out data path msg --------------------------*/

#define LTE_RLCUL_MSG_0_DATA_PATH(xx_ss_mask, xx_fmt)

#define LTE_RLCUL_MSG_1_DATA_PATH(xx_ss_mask, xx_fmt, a)

#define LTE_RLCUL_MSG_2_DATA_PATH(xx_ss_mask, xx_fmt, a, b)

#define LTE_RLCUL_MSG_3_DATA_PATH(xx_ss_mask, xx_fmt, a, b, c)

#define LTE_RLCUL_MSG_4_DATA_PATH(xx_ss_mask, xx_fmt, a, b, c, d)

#define LTE_RLCUL_MSG_5_DATA_PATH(xx_ss_mask, xx_fmt, a, b, c, d, e)

#define LTE_RLCDL_MSG_0_DATA_PATH(xx_ss_mask, xx_fmt)

#define LTE_RLCDL_MSG_1_DATA_PATH(xx_ss_mask, xx_fmt, a)

#define LTE_RLCDL_MSG_2_DATA_PATH(xx_ss_mask, xx_fmt, a, b)

#define LTE_RLCDL_MSG_3_DATA_PATH(xx_ss_mask, xx_fmt, a, b, c)

#define LTE_RLCDL_MSG_4_DATA_PATH(xx_ss_mask, xx_fmt, a, b, c, d)

#define LTE_RLCDL_MSG_5_DATA_PATH(xx_ss_mask, xx_fmt, a, b, c, d, e)

/*---------------------RLCUL MESSAGE MACRO with mask input---------------*/

#define LTE_RLCUL_MSG_0(xx_ss_mask, xx_fmt)                               \
  MSG(MSG_SSID_LTE_RLCUL, (xx_ss_mask), xx_fmt)

#define LTE_RLCUL_MSG_1(xx_ss_mask, xx_fmt, a)                            \
  MSG_1(MSG_SSID_LTE_RLCUL, (xx_ss_mask), xx_fmt, a)

#define LTE_RLCUL_MSG_2(xx_ss_mask, xx_fmt, a, b)                         \
  MSG_2(MSG_SSID_LTE_RLCUL, (xx_ss_mask), xx_fmt, a, b)

#define LTE_RLCUL_MSG_3(xx_ss_mask, xx_fmt, a, b, c)                      \
  MSG_3(MSG_SSID_LTE_RLCUL, (xx_ss_mask), xx_fmt, a, b, c)

#define LTE_RLCUL_MSG_4(xx_ss_mask, xx_fmt, a, b, c, d)                   \
  MSG_4(MSG_SSID_LTE_RLCUL, (xx_ss_mask), xx_fmt, a, b, c, d)

#define LTE_RLCUL_MSG_5(xx_ss_mask, xx_fmt, a, b, c, d, e)                \
  MSG_5(MSG_SSID_LTE_RLCUL, (xx_ss_mask), xx_fmt, a, b, c, d, e)

/*------------------------RLCUL MESSAGE LOW MACROS-----------------------*/

#define LTE_RLCUL_MSG_0_LOW(xx_fmt)                                       \
  MSG(MSG_SSID_LTE_RLCUL, MSG_LEGACY_LOW, xx_fmt)

#define LTE_RLCUL_MSG_1_LOW(xx_fmt, a)                                    \
  MSG_1(MSG_SSID_LTE_RLCUL, MSG_LEGACY_LOW, xx_fmt, a)

#define LTE_RLCUL_MSG_2_LOW(xx_fmt, a, b)                                 \
  MSG_2(MSG_SSID_LTE_RLCUL, MSG_LEGACY_LOW, xx_fmt, a, b)

#define LTE_RLCUL_MSG_3_LOW(xx_fmt, a, b, c)                              \
  MSG_3(MSG_SSID_LTE_RLCUL, MSG_LEGACY_LOW, xx_fmt, a, b, c)

#define LTE_RLCUL_MSG_4_LOW(xx_fmt, a, b, c, d)                           \
  MSG_4(MSG_SSID_LTE_RLCUL, MSG_LEGACY_LOW, xx_fmt, a, b, c, d)

#define LTE_RLCUL_MSG_5_LOW(xx_fmt, a, b, c, d, e)                        \
  MSG_5(MSG_SSID_LTE_RLCUL, MSG_LEGACY_LOW, xx_fmt, a, b, c, d, e)

/*---------------------RLCUL MESSAGE MEDIUM MACROS-----------------------*/

#define LTE_RLCUL_MSG_0_MED(xx_fmt)                                       \
  MSG(MSG_SSID_LTE_RLCUL, MSG_LEGACY_MED, xx_fmt)

#define LTE_RLCUL_MSG_1_MED(xx_fmt, a)                                    \
  MSG_1(MSG_SSID_LTE_RLCUL, MSG_LEGACY_MED, xx_fmt, a)

#define LTE_RLCUL_MSG_2_MED(xx_fmt, a, b)                                 \
  MSG_2(MSG_SSID_LTE_RLCUL, MSG_LEGACY_MED, xx_fmt, a, b)

#define LTE_RLCUL_MSG_3_MED(xx_fmt, a, b, c)                              \
  MSG_3(MSG_SSID_LTE_RLCUL, MSG_LEGACY_MED, xx_fmt, a, b, c)

#define LTE_RLCUL_MSG_4_MED(xx_fmt, a, b, c, d)                           \
  MSG_4(MSG_SSID_LTE_RLCUL, MSG_LEGACY_MED, xx_fmt, a, b, c, d)

#define LTE_RLCUL_MSG_5_MED(xx_fmt, a, b, c, d, e)                        \
  MSG_5(MSG_SSID_LTE_RLCUL, MSG_LEGACY_MED, xx_fmt, a, b, c, d, e)

/*---------------------RLCUL MESSAGE HIGH MACROS-------------------------*/

#define LTE_RLCUL_MSG_0_HIGH(xx_fmt)                                      \
  MSG(MSG_SSID_LTE_RLCUL, MSG_LEGACY_HIGH, xx_fmt)

#define LTE_RLCUL_MSG_1_HIGH(xx_fmt, a)                                   \
  MSG_1(MSG_SSID_LTE_RLCUL, MSG_LEGACY_HIGH, xx_fmt, a)

#define LTE_RLCUL_MSG_2_HIGH(xx_fmt, a, b)                                \
  MSG_2(MSG_SSID_LTE_RLCUL, MSG_LEGACY_HIGH, xx_fmt, a, b)

#define LTE_RLCUL_MSG_3_HIGH(xx_fmt, a, b, c)                             \
  MSG_3(MSG_SSID_LTE_RLCUL, MSG_LEGACY_HIGH, xx_fmt, a, b, c)

#define LTE_RLCUL_MSG_4_HIGH(xx_fmt, a, b, c, d)                          \
  MSG_4(MSG_SSID_LTE_RLCUL, MSG_LEGACY_HIGH, xx_fmt, a, b, c, d)

#define LTE_RLCUL_MSG_5_HIGH(xx_fmt, a, b, c, d, e)                       \
  MSG_5(MSG_SSID_LTE_RLCUL, MSG_LEGACY_HIGH, xx_fmt, a, b, c, d, e)

/*---------------------RLCUL MESSAGE FATAL MACRO-------------------------*/

#define LTE_RLCUL_MSG_FATAL(xx_fmt, a, b, c)                              \
  MSG(MSG_SSID_LTE_RLCUL, MSG_LEGACY_FATAL, xx_fmt, a, b, c)

/*---------------------RLCDL MESSAGE MACRO with mask input---------------*/

#define LTE_RLCDL_MSG_0(xx_ss_mask, xx_fmt)                               \
  MSG(MSG_SSID_LTE_RLCDL, (xx_ss_mask), xx_fmt)

#define LTE_RLCDL_MSG_1(xx_ss_mask, xx_fmt, a)                            \
  MSG_1(MSG_SSID_LTE_RLCDL, (xx_ss_mask), xx_fmt, a)

#define LTE_RLCDL_MSG_2(xx_ss_mask, xx_fmt, a, b)                         \
  MSG_2(MSG_SSID_LTE_RLCDL, (xx_ss_mask), xx_fmt, a, b)

#define LTE_RLCDL_MSG_3(xx_ss_mask, xx_fmt, a, b, c)                      \
  MSG_3(MSG_SSID_LTE_RLCDL, (xx_ss_mask), xx_fmt, a, b, c)

#define LTE_RLCDL_MSG_4(xx_ss_mask, xx_fmt, a, b, c, d)                   \
  MSG_4(MSG_SSID_LTE_RLCDL, (xx_ss_mask), xx_fmt, a, b, c, d)

#define LTE_RLCDL_MSG_5(xx_ss_mask, xx_fmt, a, b, c, d, e)                \
  MSG_5(MSG_SSID_LTE_RLCDL, (xx_ss_mask), xx_fmt, a, b, c, d, e)

/*------------------------RLCDL MESSAGE LOW MACROS-----------------------*/

#define LTE_RLCDL_MSG_0_LOW(xx_fmt)                                       \
  MSG(MSG_SSID_LTE_RLCDL, MSG_LEGACY_LOW, xx_fmt)

#define LTE_RLCDL_MSG_1_LOW(xx_fmt, a)                                    \
  MSG_1(MSG_SSID_LTE_RLCDL, MSG_LEGACY_LOW, xx_fmt, a)

#define LTE_RLCDL_MSG_2_LOW(xx_fmt, a, b)                                 \
  MSG_2(MSG_SSID_LTE_RLCDL, MSG_LEGACY_LOW, xx_fmt, a, b)

#define LTE_RLCDL_MSG_3_LOW(xx_fmt, a, b, c)                              \
  MSG_3(MSG_SSID_LTE_RLCDL, MSG_LEGACY_LOW, xx_fmt, a, b, c)

#define LTE_RLCDL_MSG_4_LOW(xx_fmt, a, b, c, d)                           \
  MSG_4(MSG_SSID_LTE_RLCDL, MSG_LEGACY_LOW, xx_fmt, a, b, c, d)

#define LTE_RLCDL_MSG_5_LOW(xx_fmt, a, b, c, d, e)                        \
  MSG_5(MSG_SSID_LTE_RLCDL, MSG_LEGACY_LOW, xx_fmt, a, b, c, d, e)

/*---------------------RLCDL MESSAGE MED MACROS--------------------------*/

#define LTE_RLCDL_MSG_0_MED(xx_fmt)                                       \
  MSG(MSG_SSID_LTE_RLCDL, MSG_LEGACY_MED, xx_fmt)

#define LTE_RLCDL_MSG_1_MED(xx_fmt, a)                                    \
  MSG_1(MSG_SSID_LTE_RLCDL, MSG_LEGACY_MED, xx_fmt, a)

#define LTE_RLCDL_MSG_2_MED(xx_fmt, a, b)                                 \
  MSG_2(MSG_SSID_LTE_RLCDL, MSG_LEGACY_MED, xx_fmt, a, b)

#define LTE_RLCDL_MSG_3_MED(xx_fmt, a, b, c)                              \
  MSG_3(MSG_SSID_LTE_RLCDL, MSG_LEGACY_MED, xx_fmt, a, b, c)

#define LTE_RLCDL_MSG_4_MED(xx_fmt, a, b, c, d)                           \
  MSG_4(MSG_SSID_LTE_RLCDL, MSG_LEGACY_MED, xx_fmt, a, b, c, d)

#define LTE_RLCDL_MSG_5_MED(xx_fmt, a, b, c, d, e)                        \
  MSG_5(MSG_SSID_LTE_RLCDL, MSG_LEGACY_MED, xx_fmt, a, b, c, d, e)

/*---------------------RLCDL MESSAGE HIGH MACROS-------------------------*/

#define LTE_RLCDL_MSG_0_HIGH(xx_fmt)                                      \
  MSG(MSG_SSID_LTE_RLCDL, MSG_LEGACY_HIGH, xx_fmt)

#define LTE_RLCDL_MSG_1_HIGH(xx_fmt, a)                                   \
  MSG_1(MSG_SSID_LTE_RLCDL, MSG_LEGACY_HIGH, xx_fmt, a)

#define LTE_RLCDL_MSG_2_HIGH(xx_fmt, a, b)                                \
  MSG_2(MSG_SSID_LTE_RLCDL, MSG_LEGACY_HIGH, xx_fmt, a, b)

#define LTE_RLCDL_MSG_3_HIGH(xx_fmt, a, b, c)                             \
  MSG_3(MSG_SSID_LTE_RLCDL, MSG_LEGACY_HIGH, xx_fmt, a, b, c)

#define LTE_RLCDL_MSG_4_HIGH(xx_fmt, a, b, c, d)                          \
  MSG_4(MSG_SSID_LTE_RLCDL, MSG_LEGACY_HIGH, xx_fmt, a, b, c, d)

#define LTE_RLCDL_MSG_5_HIGH(xx_fmt, a, b, c, d, e)                       \
  MSG_5(MSG_SSID_LTE_RLCDL, MSG_LEGACY_HIGH, xx_fmt, a, b, c, d, e)

/*---------------------RLCDL MESSAGE FATAL MACRO-------------------------*/

#define LTE_RLCDL_MSG_FATAL(xx_fmt, a, b, c)                              \
  MSG(MSG_SSID_LTE_RLCDL, MSG_LEGACY_FATAL, xx_fmt, a, b, c)

#ifdef MSG_BUILD_MASK_MSG_SSID_LTE_RLCUL
  #undef MSG_BUILD_MASK_MSG_SSID_LTE_RLCUL
#endif /* MSG_BUILD_MASK_MSG_SSID_LTE_RLCUL */

/* MASK not included here will be supressed at runtime:
   * MSG_LEGACY_LOW
   * MSG_LEGACY_MED
   * LTE_RLCUL_POLLING
   * LTE_RLCUL_INTER_TASK_MSGS
   * LTE_RLCUL_STATUS
   * LTE_RLCUL_DATA_PATH
*/
#define MSG_BUILD_MASK_MSG_SSID_LTE_RLCUL   \
               (MSG_LEGACY_HIGH         |   \
                MSG_LEGACY_ERROR        |   \
                MSG_LEGACY_FATAL        |   \
                LTE_RLCUL_CONFIG)

#ifdef MSG_BUILD_MASK_MSG_SSID_LTE_RLCDL
  #undef MSG_BUILD_MASK_MSG_SSID_LTE_RLCDL
#endif /* MSG_BUILD_MASK_MSG_SSID_LTE_RLCUL */

/* MASK not included here will be supressed at runtime:
   * MSG_LEGACY_LOW
   * MSG_LEGACY_MED
   * LTE_RLCDL_POLLING
   * LTE_RLCDL_INTER_TASK_MSGS
   * LTE_RLCDL_STATUS
   * LTE_RLCDL_DATA_PATH
*/
#define MSG_BUILD_MASK_MSG_SSID_LTE_RLCDL   \
               (MSG_LEGACY_HIGH         |   \
                MSG_LEGACY_ERROR        |   \
                MSG_LEGACY_FATAL        |   \
                LTE_RLCDL_CONFIG)

/*=============================================================================

                   DATA TYPES

=============================================================================*/
/*! @brief This defines ation type on RB
*/
typedef uint8 lte_rlc_log_action_t;

/*! @brief This defines configuration reason type
*/
typedef uint8 lte_rlc_log_cfg_reason_t;

/*! @brief This defines Rb mode type
*/
typedef uint8 lte_rlc_log_rb_mode_t;

/*! @brief This defines Rb type
*/
typedef uint8 lte_rlc_log_rb_type_t;

/*! @brief Enumeration for reason as to why the statistic PDU was logged */
typedef enum
{
  LTE_RLC_STAT_INVALID = 0,      /*!< For initialization */
  LTE_RLC_STAT_PERIODIC,         /*!< stat pdu was logged on periodic basis */
  LTE_RLC_STAT_RE_ESTABLISHMENT, /*!< stat pdu was logged due to Re-est */
  LTE_RLC_STAT_RECONFIG,         /*!< stat pdu was logged due to Reconfig */
  LTE_RLC_STAT_RELEASE,          /*!< stat pdu was logged due to RB release */
  LTE_RLC_STAT_VALUE_MAX         /*!< Max Value for this enum */
} lte_rlc_stat_log_reason_e;

#ifdef _WIN32
    #pragma pack(push,1) // Save previous, and turn on 1 byte alignment
#endif

/*! @brief RLC configuation subpkt related structure
*/
typedef PACK(struct)
{
  lte_rb_cfg_idx_t       rb_cfg_idx; /*!< RB configuration index: unique */
  lte_rlc_log_action_t   action;     /*!< action on the RB */
} lte_rlc_log_modified_rb_s;

/*! @brief PDU log subpkt structure
*/
typedef PACK(struct)
{
  lte_sfn_s  sfn;                     /*!< Subframe number */
  uint16     pdu_size;                /*!< size of the RLC PDU */
  uint16     logged_bytes;            /*!< Actual length of PDU logged,
                                           actual payload to follow */
  uint8      status;                  /*!< status of the DL PDU, ignore for
                                           UL */
} lte_rlc_log_pdu_s;

/*! @brief PDU unified subpkt structure 
*/
typedef PACK(struct)
{
  lte_sfn_s  sfn;                   /*!< Subframe number */
  lte_mbsfn_area_id_t area_id;      /*!< MBSFN Area ID of this eMBMS packet */
  lte_pmch_id_t       pmch_id;      /*!< PMCH ID of this eMBMS packet */
  uint8               lcid;         /*!< logical channel ID */
  uint16              freq;         /*!< Frequency of the TB's cell */
  uint16              pdu_size;     /*!< the pdu size */
  uint8               status;       /*!< mark pdu category */
  uint16              logged_bytes; /*!< Number of bytes logged */
} lte_rlc_log_unified_pdu_s;

#ifdef _WIN32
   #pragma pack(pop) // Revert alignment to what it was previously
#endif

/*! @brief Log Id base for LTE RLCDL log packet. TODO: replace */
#define LTE_RLC_LOG_ID_BASE            0xB080

/*! @brief RLC log pkt ID's
*/
enum
{
  LTE_RLCDL_LOG_ID_CFG = LTE_RLC_LOG_ID_BASE,    /*!< DL configuration */
  LTE_RLCDL_LOG_ID_RX_PDU,                       /*!< DL received RLC PDU */
  LTE_RLCDL_LOG_ID_STAT,                         /*!< DL statistics */
  LTE_RLC_LOG_ID_MAX                             /*!< MAX */
};

/*! @brief RLC sub packet log codes
*/
enum
{
  LTE_RLCDL_LOG_SUB_PKT_ID_CFG = LTE_RLC_LOG_SUBPKT_ID_BASE,
                                        /*!< DL configuration */
  LTE_RLCDL_LOG_SUB_PKT_ID_RX_PDU,      /*!< received RLC PDU */
  LTE_RLCDL_LOG_SUB_PKT_ID_STAT,        /*!< RB statistics */
  LTE_RLCDL_LOG_SUB_PKT_ID_PROFILE,     /*!< DATAPATH profile */
  LTE_RLCDL_LOG_SUB_PKT_ID_UM_STATE,    /*!< state variables for UM */
  LTE_RLCUL_LOG_SUB_PKT_ID_CONFIG,      /*!< UL config sub pkt log code */
  LTE_RLCUL_LOG_SUB_PKT_ID_PDU,         /*!< RLC PDU subpkt log code */
  LTE_RLCUL_LOG_SUB_PKT_ID_STATISTICS,  /*!< RB statistics subpkt log code */
  LTE_RLCUL_LOG_SUB_PKT_ID_AM_STATE,    /*!< state variables for AM */
  LTE_RLCUL_LOG_SUB_PKT_ID_UM_STATE,    /*!< state variables for UM */
  LTE_RLCDL_LOG_SUB_PKT_ID_EMBMS_PDU,   /*!< Unified RLC DL PDU variable*/
  LTE_RLCDL_LOG_SUB_PKT_ID_EMBMS_STATS, /*!< eMBMS statistics */
  LTE_RLCDL_LOG_SUB_PKT_ID_EMBMS_CFG,   /*!< eMBMS configuration */
  LTE_RLCDL_LOG_SUB_PKT_ID_EMBMS_DSDS_SER, /*!< eMBMS monitored service*/
  LTE_RLCDL_LOG_SUB_PKT_ID_EMBMS_DPI_SDU, /*!< eMBMS DeepPacketInspection sdu*/
  LTE_RLC_LOG_SUB_PKT_ID_MAX            /*!< MAX */
};

/*! @brief flush RLCUL log buffer indication:
    LTE_RLCUL_FLUSH_RLCUL_LOG_BUF_IND
*/
typedef struct
{
  msgr_hdr_struct_type            hdr;         /*!< message header */
  lte_rb_cfg_idx_t                rb_cfg_idx;  /*!< rb config index */
} lte_rlcul_flush_rlcul_log_buf_ind_s;

/*! @brief flush PDCPUL log buffer indication:
    LTE_RLCUL_FLUSH_PDCPUL_LOG_BUF_IND
*/
typedef struct
{
  msgr_hdr_struct_type            hdr;         /*!< message header */
  lte_rb_cfg_idx_t                rb_cfg_idx;  /*!< rb config index */
  uint32                          log_code;    /*!< log code of log packet */
} lte_rlcul_flush_pdcpul_log_buf_ind_s;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

extern void lte_rlc_log_error_handler
(
  void
);

#endif /* LTE_RLC_LOG_H */

/*!
  @file
  lte_rlc_rrc.h

  @brief
  RLC header file related to RRC.

  @author
  gxiao, araina

*/

/*==============================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/lte_rlc_rrc.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
07/18/11   mm      Added interface changes for eMBMS
04/28/10   ar      added message structures for RLCUL and RLCDL START and STOP
                   request and confirm messages
04/27/09   ar      added pdcp_seq_len to RLCUL config struct, needed for pdcpul
                   pdu logging
04/10/09   ar      update poll_byte comments
04/10/09   ar      changed poll_byte type from uint16 to uint32
03/30/09   ar      changed the variable name max_dat to max_retx_threshold in
                   lte_rlcul_am_cfg_s
03/11/09   ar      Update comments for lte_rlc_rb_action_e enum
12/04/08   ar      Added message payload structure for LTE_RLCUL_MAX_RETX_IND
11/17/08   ax      use lte_rb_cfg_idx_t as a unique identifier across AS
07/31/08   ax      initial version
==============================================================================*/

#ifndef LTE_RLC_RRC_H
#define LTE_RLC_RRC_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <msgr.h>
#include <lte_as.h>
#include "lte_l2_comdef.h"
#include "lte_l1_types.h"
#include "lte_rrc_ext_msg.h"
/*==============================================================================

                   EXTERNAL CONSTANT/MACRO DEFINITIONS

==============================================================================*/


/*==============================================================================

                   EXTERNAL ENUMERATION TYPES

==============================================================================*/

/*! @brief action on the RB
*/
typedef enum
{
  LTE_RLC_RB_ACTION_INVALID      = 0x0, /*!< invalid - for initialization
                                              purpose */
  LTE_RLC_RB_ACTION_ADD          = 0x1, /*!< add RB */
  LTE_RLC_RB_ACTION_MODIFY       = 0x2, /*!< modify RB */
  LTE_RLC_RB_ACTION_RE_EST       = 0x4, /*!< re-establish RB, for future use */
  LTE_RLC_RB_ACTION_RE_EST_MODIFY= 0x6, /*!< re-establish and modify RB,
                                             for future use  */
  LTE_RLC_RB_ACTION_REMOVE       = 0x8  /*!< remove RB  */
} lte_rlc_rb_action_e;

/*==============================================================================

                   EXTERNAL STRUCTURE/UNION TYPES

==============================================================================*/

/*! @brief UL UM configuration definition
*/
typedef struct
{
  uint8  sn_length; /*!< Indicates the UM RLC SN field size in bits. */
} lte_rlcul_um_cfg_s;

/*! @brief UL AM configuration definition
*/
typedef struct
{
  uint32 poll_byte;          /*!< Indicates the value of constant Poll_Byte
                                  in bytes. Special value of
                                  LTE_OTA_IE_INFINITY_UINT32(0xffffffff) means
                                  infinite*/
  uint16 poll_pdu;           /*!< Indicates the value of constant Poll_Byte
                                  in pdus. Special value of
                                  LTE_OTA_IE_INFINITY_UINT16(0xffff) means
                                  infinite */
  uint16 t_poll_retransmit;  /*!< Indicates the value of timer T_poll_retransmit
                                  in ms */
  uint8  max_retx_threshold; /*!< Maximum number of re-transmissions */
} lte_rlcul_am_cfg_s;

/*! @brief DL UM configuration definition
*/
typedef struct
{
  uint16  t_reordering; /*!< Indicates the value of timer T_reordering in ms */
  uint8   sn_length;    /*!< Indicates the UM RLC SN field size in bits. */
} lte_rlcdl_um_cfg_s;

/*! @brief DL AM configuration definition
*/
typedef struct
{
  uint16  t_reordering;      /*!< Indicates the value of timer
                                  T_reordering in ms */
  uint16  t_status_prohibit; /*!< Indicates the value of timer
                                  T_status_prohibit in ms */
} lte_rlcdl_am_cfg_s;

/*! @brief union of UL configuration
*/
typedef union
{
  lte_rlcul_um_cfg_s  um_cfg; /*!< configuration for UM */
  lte_rlcul_am_cfg_s  am_cfg; /*!< configuration for AM */
} lte_rlcul_cfg_u;

/*! @brief union of DL cfg
*/
typedef union
{
  lte_rlcdl_um_cfg_s  um_cfg; /*!< configuration for UM */
  lte_rlcdl_am_cfg_s  am_cfg; /*!< configuration for AM */
} lte_rlcdl_cfg_u;

/*! @brief UL RB info definition
*/
typedef struct
{
  lte_rb_rlc_mode_e  rb_mode;             /*!< TM/UM/AM */
  lte_lc_id_t        lc_id;               /*!< logical channel ID */
  lte_rb_id_t        rb_id;               /*!< radio bearer ID, not unique */
  lte_eps_id_t       eps_id;              /*!< EPS bearer ID */
  lte_rb_cfg_idx_t   rb_cfg_idx;          /*!< RB configuration index: unique */
  boolean            pdcp_status_required;/*!< PDCP attribute */
  lte_rb_type_e      rb_type;             /*!< SRB/DRB */
  uint16             pdcp_discard_timer;  /*!< PDCP attribute; value 0 means
                                               not configured, ie no discard */
  uint8              pdcp_seq_len;        /*!< PDCP seq len in bits(5,7,12 bits)
                                               only used for PDCP pdu logging */
  lte_rlcul_cfg_u    cfg;                 /*!< configuration for the RB */
} lte_rlcul_rb_info_s;

/*! @brief DL RB info definition
*/
typedef struct
{
  lte_rb_rlc_mode_e   rb_mode;    /*!< TM/UM/AM */
  lte_lc_id_t         lc_id;      /*!< logical channel ID */
  lte_rb_id_t         rb_id;      /*!< radio bearer ID: not unique */
  lte_eps_id_t       eps_id;     /*!< EPS bearer ID */
  lte_rb_cfg_idx_t    rb_cfg_idx; /*!< RB configuration index: unique
                                       RB cfg idx 1 - 32 for DRBs
                                       RB cfg idx 33-34 for SRBs */
  lte_rb_type_e       rb_type;    /*!< SRB/DRB */
  lte_rlcdl_cfg_u     cfg;        /*!< configuration for the RB */

} lte_rlcdl_rb_info_s;

/*! @brief DL MRB info definition
*/
typedef struct
{
  lte_rb_rlc_mode_e   rb_mode;    /*!< Only UM applicable */
  lte_mbsfn_area_id_t area_id;    /*!< MBSFN area id */
  lte_pmch_id_t       pmch_id;    /*!< PMCH id */
  lte_rb_id_t         mrb_id;      /*!< MRB id, generated by RRC, 
                                        MRB id 35 used for MCCH by default,
                                        No RRC configuration for MCCH,
                                        MTCH valid range is 36 to 
                                        (36 + LTE_MAX_ACTIVE_MRB - 1) */
  lte_lc_id_t         lc_id;      /*!< logical channel ID */
  lte_rb_cfg_idx_t    mrb_cfg_idx;     /*!< MRB cfg idx: currently
                                            0 to (LTE_MAX_ACTIVE_MRB - 1) 
                                            is the valid index range*/
  lte_rb_type_e       rb_type;    /*!< Only MTCH is applicable */
  lte_rlcdl_um_cfg_s  cfg;        /*!< configuration for the RB */

  lte_rrc_embms_session_info_s session_info; /*!< TMGI/Session id for the MRB id */


  /*! Indicates the MCH scheduling period i.e. the periodicity used for providing 
    MCH scheduling information at lower layers (MAC) applicable for an MCH. Value rf8
    corresponds to 8 radio frames, rf16 corresponds to 16 radio frames and so on. 
  */
  lte_l1_mbsfn_mch_schdl_period_e    msi_schdl_period;

} lte_rlcdl_mrb_info_s;

/*! @brief definition of modified RB info
*/
typedef struct
{
  lte_rb_cfg_idx_t     rb_cfg_idx; /*!< RB configuration index: unique */
  lte_rlc_rb_action_e  action;     /*!< action on the RB */
} lte_rlc_modified_rb_s;

/*! @brief DL MRB info definition
*/
typedef struct
{
  uint8                  num_released_mrb;  /*!< number of released MRBs */
  lte_rb_cfg_idx_t       released_mrb_cfg_idx[LTE_MAX_ACTIVE_MRB]; 
                                          /*!< Released MRB cfg index
                                            0 to (LTE_MAX_ACTIVE_MRB - 1) 
                                            is the valid index range */
  uint8                  num_added_mrb; /*!< number of added MRBs */
  lte_rlc_modified_rb_s  added_mrb_cfg_idx[LTE_MAX_ACTIVE_MRB]; 
                                           /*!< added MRB cfg idx,
                                            0 to (LTE_MAX_ACTIVE_MRB - 1)
                                             is the valid index range */

  lte_rlcdl_mrb_info_s*  mrb_cfg_base_ptr;  /*!< base pointer to DL MRB
                                               configuration database
                                               of size LTE_MAX_ACTIVE_MRB */
} lte_rlcdl_embms_cfg_s;


/*! @brief configuration request message definition for UL
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;          /*!< common message router header */
  lte_l2_cfg_reason_e    cfg_reason;         /*!< configuration reason */
  uint8                  num_released_rb;  /*!< number of released RBs */
  lte_rb_cfg_idx_t       released_rb_cfg_idx[LTE_MAX_ACTIVE_RB]; /*!< Released
                                                             RB cfg index */
  uint8                  num_modified_rb;  /*!< number of add/modified RBs */
  lte_rlc_modified_rb_s  modified_rb[LTE_MAX_ACTIVE_RB]; /*!< add/modified RB
                                                          configuration index */
  lte_rlcul_rb_info_s*   rb_cfg_base_ptr;   /*!< base pointer to UL Radio Bearer
                                                configuration database
                                                of size LTE_MAX_RB_CFG_IDX */
} lte_rlcul_cfg_req_s;


/*! @brief configuration request message definition for DL
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;          /*!< common message router header */
  lte_l2_cfg_reason_e    cfg_reason;       /*!< configuration reason */
  uint8                  num_released_rb;  /*!< number of released RBs, 
                                              will be 0 if cfg_reason is 
                                              LTE_L2_CFG_EMBMS */
  lte_rb_cfg_idx_t       released_rb_cfg_idx[LTE_MAX_ACTIVE_RB]; /*!< Released
                                                             RB cfg index */
  uint8                  num_modified_rb;  /*!< number of add/modified RBs,
                                                will be 0 if cfg_reason is 
                                                LTE_L2_CFG_EMBMS */
  lte_rlc_modified_rb_s  modified_rb[LTE_MAX_ACTIVE_RB]; /*!< add/modified RB
                                                          configuration index */
  lte_rlcdl_rb_info_s*   rb_cfg_base_ptr;  /*!< base pointer to DL Radio Bearer
                                                configuration database
                                                of size LTE_MAX_RB_CFG_IDX */
  lte_rlcdl_embms_cfg_s  embms_cfg;        /*!< eMBMS config, applicable only
                                                if cfg_reason is 
                                                LTE_L2_CFG_EMBMS */

} lte_rlcdl_cfg_req_s;

/*! @brief configuration confirmation message definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;          /*!< common message router header */
  lte_errno_e            cfg_status;       /*!< status of configuration */
} lte_rlc_cfg_cnf_s;

/*! @brief LTE_RLCXX_START_REQ request message definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;    /*!< common message router header */
} lte_rlc_start_req_s;

/*! @brief LTE_RLCXX_START_CNF request message definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;    /*!< common message router header */
  lte_errno_e            cnf_status; /*!< confimation status message */
} lte_rlc_start_cnf_s;

/*! @brief LTE_RLCXX_STOP_REQ request message definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;  /*!< common message router header */
} lte_rlc_stop_req_s;

/*! @brief LTE_RLCXX_STOP_CNF request message definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;    /*!< common message router header */
  lte_errno_e            cnf_status; /*!< confimation status message */
} lte_rlc_stop_cnf_s;

/*! @brief LTE_RLCUL_MAX_RETX_IND message payload definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;          /*!< common message router header */
  lte_rb_cfg_idx_t       rb_cfg_idx;       /*!< Unique RB configuration index */
} lte_rlcul_max_retx_ind_s;

/*! @brief signal message MCCH (PDU) indication from RLC DL to RRC
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  msgr_attach_s                       dsm_attach; /*!< dsm attachment contained signal message */

  lte_mbsfn_area_id_t                 area_id; /*!< Area id of MCCH */

  lte_earfcn_t                        dl_cell_frequency;  /*!< The DL freq of the cell 
                                                  in which the MCCH PDU was received */

  lte_sfn_s                           sfn;  /*!< SFN in which the MCCH PDU was received */

} lte_rlcdl_mcch_pdu_ind_s;

/*! @brief LTE_RLCUL_ACK_TX_STS_REQ request message definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;  /*!< common message router header */
} lte_rlcul_ack_tx_sts_req_s;

/*! @brief LTE_RLCUL_ACK_TX_STS_CNF request message definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;    /*!< common message router header */
  lte_errno_e            cnf_status; /*!< confimation status message */
} lte_rlcul_ack_tx_sts_cnf_s;

#endif /* LTE_RLC_RRC_H */

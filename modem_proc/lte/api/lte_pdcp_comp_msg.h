/*!
  @file
  lte_pdcp_comp_msg.h

  @brief
  This file contains all External message data structure and IDs interfaces
  exported by PDCP COMP task.


*/

/*===========================================================================

  Copyright (c) 2016 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/lte_pdcp_comp_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/01/16   sb      CR979285: UDC HO fixes and enhancements
06/26/15   sb      CR862079: Change to upgrade PDCP support to
                   ELS spec ver D and UDC ver F
06/10/15   sb      CR850899: PDCP should send Ctrl PDU to disable UDC when UDC 
                   is released by enB while bearer is still active
05/13/15   sb      CR 803880: Initial version - modified PDCP interface for UDC feature

===========================================================================*/

#ifndef LTE_PDCPCOMP_MSG_H
#define LTE_PDCPCOMP_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <msgr.h>
#include <msgr_lte.h>
#include <lte_app.h>
#include "lte_l2_comdef.h"
#include <lte_security.h>
#include "lte_pdcp_msg.h"
#include "lte_pdcp_offload_msg.h"
#include "cfcm.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief configuration request from RLC UL to PDCP COMP 
whenever a UDC enabled RB is added */
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  /* Message payload */
  lte_rb_cfg_idx_t                    rb_cfg_idx;
  lte_eps_id_t                        eps_id;       /*!< EPS ID */
  lte_pdcpul_udc_cfg_s                udc_cfg;      /*!< ELS UDC Parameters*/  
  lte_pdcp_cfg_act_e                  cfg_action;   /*!< ADD/MOD */
  lte_l2_cfg_reason_e                 cfg_reason;   /*!< RECFG/HO/RLF */
} lte_pdcp_comp_rb_add_mod_ind_msg_s;

/*! @brief configuration request from RLC UL to PDCP COMP 
whenever a UDC enabled RB is reset (during RLF/HO)*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  /* Message payload */
  lte_rb_cfg_idx_t                    rb_cfg_idx; /*!< RB Cfg Index */
  lte_pdcp_udc_reset_cfg_s            udc_reset_cfg; /*!< Reset config */
} lte_pdcp_comp_rb_reset_ind_msg_s;

/*! @brief configuration request from RLC UL to PDCP COMP 
whenever a RRC resumes UDC enabled bearer (via MODIFY_RESUME sent to RLCUL task) 
after UE recovers from RLF*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
/* Message payload */
  lte_rb_cfg_idx_t                    rb_cfg_idx; /*!< RB Cfg Index */
} lte_pdcp_comp_mod_resume_ind_msg_s;

/*! @brief configuration request from RLC UL to PDCP COMP 
whenever a UDC enabled RB is removed*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  /* Message payload */
  lte_rb_cfg_idx_t                    rb_cfg_idx;
} lte_pdcp_comp_rb_rel_ind_msg_s;

/*! @brief COMP RB info definition for UL Data
*/
typedef struct
{
  lte_rb_cfg_idx_t      rb_cfg_idx; /*!< RB cfg index */
  lte_eps_id_t          eps_id;     /*!< EPS ID */
  lte_pdcpul_udc_cfg_s  udc_cfg; /*!< ELS UDC Parameters*/  
} lte_pdcp_comp_info_s;

/*----------------------------------------------------------------------------*/
/*     LTE PDCP/COMP External message definitions                             */
/*----------------------------------------------------------------------------*/

/*! @brief RAB register request from RLCUL to PDCP COMP for UL Data 
  whenever a PS registers a watermark with(i.e. when LTE_PDCPUL_RAB_REG_REQ 
  is sent to RLCUL task) UDC enabled bearer 
  */
typedef struct
{
  msgr_hdr_struct_type                hdr;         /*!< message header */
  /* Message payload */
  lte_rb_cfg_idx_t                    rb_cfg_idx; /*!< RB cfg index */
  lte_pdcp_cfg_act_e                  cfg_act; /*!< cfg_act */
  void *                              ps_pdcp_comp_wm_ptr; /*!< PS to PDCP WM ptr */
} lte_pdcp_comp_rab_reg_ind_msg_s;

/*! @brief RAB deregister request from PDCP to COMP for UL Data
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;         /*!< message header */
  /* Message payload */
  lte_rb_cfg_idx_t                    rb_cfg_idx; /*!< RB cfg index */
  lte_eps_id_t                        eps_id;     /*!< EPS ID */
  lte_pdcp_bearer_type_e              curr_state; /*!< Curr bearer state*/
  lte_pdcp_cfg_act_e                  cfg_act; /*!<cfg_act*/
  boolean                             send_ctrl_pdu;/*!< boolean to indicate  if
                                                    UDC Ctrl PDU needs to be sent */
} lte_pdcp_comp_rab_dereg_ind_msg_s;

/*! @brief indication that is triggered when data to watermark is enqueued
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  lte_rb_cfg_idx_t                    rb_cfg_idx; /*!< RB Cfg Index */
} lte_pdcp_comp_wm_enqueue_ind_msg_s;

/*! @brief indication that is triggered when low watermark is crosssed
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  lte_rb_cfg_idx_t                    rb_cfg_idx; /*!< RB Cfg Index */
} lte_pdcp_comp_wm_low_ind_msg_s;


/*! @brief LTE_PDCPCOMP_START_IND  message definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;    /*!< common message router header */
} lte_pdcp_comp_start_ind_msg_s;

/*! @brief LTE_PDCPCOMP_STOP_IND  message definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;  /*!< common message router header */
} lte_pdcp_comp_stop_ind_msg_s;

/*! @brief indication that is triggered when pkts need to be reenqueued to Wm
*/
typedef struct
{
  msgr_hdr_struct_type               hdr;  /*!< message header */
  msgr_attach_s                      dsm_attach; /*!< dsm attachment */
  lte_rb_cfg_idx_t                   rb_cfg_idx; /*!< RB cfg index */
  uint32                             num_reenq_pkts_bytes;
  lte_l2_cfg_reason_e                cfg_reason;  /*!< RECFG/HO/RLF */
  boolean                            no_ctrl_pdu; /*!< boolean to indicate  if  
                                                         UDC Ctrl PDU needs to be sent */
  boolean                            comp_unacked_sdus;  /*!< Boolean to indicate if 
                                                         unacked SDUs need to be compressed*/
} lte_pdcp_comp_reestab_ind_msg_s;

/*-----------------------------------------------------------------------*/
/*     LTE PDCP COMP External Indication messages                           */
/*-----------------------------------------------------------------------*/
/*! @brief external Indications that the LTE PDCP COMP module sends  */
enum
{
  LTE_PDCPCOMP_FIRST_IND = MSGR_UMID_BASE(MSGR_LTE_PDCPCOMP, MSGR_TYPE_IND),
 
  MSGR_DEFINE_UMID(LTE,PDCPCOMP,IND,WM_ENQUEUE,0x00,
                   lte_pdcp_comp_wm_enqueue_ind_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPCOMP,IND,WM_LOW,0x01,
                   lte_pdcp_comp_wm_low_ind_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPCOMP,IND,RB_ADD_MOD,0x02,
                   lte_pdcp_comp_rb_add_mod_ind_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPCOMP,IND,RB_REL,0x03,
                   lte_pdcp_comp_rb_rel_ind_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPCOMP,IND,RB_RESET,0x04,
                   lte_pdcp_comp_rb_reset_ind_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPCOMP,IND,RAB_REG,0x05,
                   lte_pdcp_comp_rab_reg_ind_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPCOMP,IND,RAB_DEREG,0x06,
                   lte_pdcp_comp_rab_dereg_ind_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPCOMP,IND,MODIFY_RESUME,0x07,
                   lte_pdcp_comp_mod_resume_ind_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPCOMP,IND,START,0x08,
                   lte_pdcp_comp_start_ind_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPCOMP,IND,STOP,0x09,
                   lte_pdcp_comp_stop_ind_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPCOMP,IND,REESTAB,0xa,
                   lte_pdcp_comp_reestab_ind_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPCOMP,IND,FC,0xb,
                   cfcm_cmd_msg_type_s),
  LTE_PDCPCOMP_MAX_IND,
  LTE_PDCPCOMP_LAST_IND = LTE_PDCPCOMP_MAX_IND - 1
};

/*-----------------------------------------------------------------------*/
/*     LTE PDCPCOMP External Request messages                         */
/*-----------------------------------------------------------------------*/

/*! @brief Supervisory messages that PDCP COMP receives or throws
*/
enum
{  
  MSGR_DEFINE_UMID(LTE, PDCPCOMP, SPR, LOOPBACK, MSGR_ID_LOOPBACK,
                   msgr_spr_loopback_s),
  MSGR_DEFINE_UMID(LTE, PDCPCOMP, SPR, LOOPBACK_REPLY, MSGR_ID_LOOPBACK_REPLY,
                   msgr_spr_loopback_reply_s),
  MSGR_DEFINE_UMID(LTE, PDCPCOMP, SPR, THREAD_READY, LTE_APP_ID_THREAD_READY,
                   none),
  MSGR_DEFINE_UMID(LTE, PDCPCOMP, SPR, THREAD_KILL, LTE_APP_ID_THREAD_KILL,
                   none)
};

#endif /* LTE_PDCPCOMP_MSG_H */

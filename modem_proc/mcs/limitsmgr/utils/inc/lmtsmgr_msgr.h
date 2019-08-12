#ifndef LMTSMGR_MSGR_H
#define LMTSMGR_MSGR_H
/*!
  @file
  lmtsmgr_msgr.h

  @brief
  APIs for LMTSMGR's MSGR interface

*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

===========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/utils/inc/lmtsmgr_msgr.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/10/14   ag      Added support for call in distress ind
08/29/14   jm      Change SL TL sim to use FreqId Pointer
08/12/14   jm      Slot level timeline support
06/23/14   sg      desense indication mechanism
04/28/14   jm      L+G Band Avoidance support
03/17/14   jm      WWAN coex based off of RF device assignment
04/08/12   ckk     Initial implementation

==========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "msgr.h"
#include "wwan_coex_mgr.h"
#include "wwcoex_action_iface.h"


/* This is a union of all the messages that LMTSMGR is interested in */
typedef union
{
  /* Used for Empty message packet */
  msgr_hdr_struct_type    hdr;

  cxm_stx_tech_state_ind_s stx_tech_on;

  cxm_stx_tech_txoff_ind_s stx_tech_off;
  
  cxm_stx_set_pwr_ind_s     stx_tech_power;

  cxm_stx_tech_state_chs_ind_s stx_tech_on_with_chs;

  cxm_wwan_tech_state_ind_s  tech_state_ind;

  /* Coex indications */
  cxm_coex_power_ind_s      coex_power_ind;

  cxm_activity_timeline_s   activity_tl_ind;

  cxm_inactivity_client_reg_s inactivity_reg_ind;

  cxm_chain_owner_msg_s    chain_own_msg_ind;

  /* Band avoidance indications */
  cxm_coex_ba_freq_ind_s   ba_serv_freq_ind;

  cxm_coex_ba_pwr_ind_s    ba_serv_pwr_ind;

  cxm_coex_ba_blist_res_s  ba_blist_resp;

  cxm_coex_desense_ind_req_s   desense_ind_req;

  cxm_tech_distress_ind_s  tech_distress_ind;
  
  cxm_timing_info_s       slot_lvl_act_tl_ind;
  
  cxm_sim_timing_info_s   slot_level_act_tl_sim_ind;

} lmtsmgr_msgr_rcv_msg_u;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

  FUNCTION:  lmtsmgr_msgr_init

===========================================================================*/
/*!
  @brief
    To initialize MSGR client & setup associated queue(s)

  @return
    errno_enum_type
*/
/*=========================================================================*/
errno_enum_type lmtsmgr_msgr_init (
  void
);

/*===========================================================================

  FUNCTION:  lmtsmgr_msgr_deinit

===========================================================================*/
/*!
  @brief
    To de-initialize MSGR client & associated queue(s)

  @return
    errno_enum_type
*/
/*=========================================================================*/
errno_enum_type lmtsmgr_msgr_deinit (
  void
);

/*===========================================================================

  FUNCTION:  lmtsmgr_msgr_register_msgs

===========================================================================*/
/*!
  @brief
    To register for all related UMIDs

  @return
    None
*/
/*=========================================================================*/
void lmtsmgr_msgr_register_msgs (
  void
);

/*===========================================================================

  FUNCTION:  lmtsmgr_msgr_deregister_msgs

===========================================================================*/
/*!
  @brief
    To deregister for all related UMIDs

  @return
    None
*/
/*=========================================================================*/
void lmtsmgr_msgr_deregister_msgs (
  void
);

/*===========================================================================

  FUNCTION:  lmtsmgr_msgr_send_msg

===========================================================================*/
/*!
  @brief
    Send a message via MSGR

  @return
    errno_enum_type
*/
/*=========================================================================*/
errno_enum_type lmtsmgr_msgr_send_msg (
  msgr_hdr_struct_type *msg_hdr_ptr,  /*!< Pointer to MSG Header  */
  msgr_umid_type        umid,         /*!< UMID of the message    */
  int32                 size          /*!< Size of the message    */
);

/*===========================================================================

  FUNCTION:  lmtsmgr_msgr_process_sig

===========================================================================*/
/*!
  @brief
    Process the LMTSMGR's MSGR REX signal & associated queue

  @return
    None
*/
/*=========================================================================*/
void lmtsmgr_msgr_process_sig (
  void
);

#endif /* LMTSMGR_MSGR_H */

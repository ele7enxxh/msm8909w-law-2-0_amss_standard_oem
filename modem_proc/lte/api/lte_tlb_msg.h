/*!
  @file
  lte_tlb_msg.h

  @brief
  This file contains all External message IDs interfaces exported by Test
  Loopback module


*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/lte_tlb_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/26/15   el      CR 796707: Clean up MSG_SPRINTF usage and add more F3 log
09/13/12   st      CR 364517: Added new messages and API changes for TLBMC
11/16/10   bn      Added API for DS to inform Close TL is ready
04/26/10   bn      Added in API to inform DS testloop open
08/12/09   bn      Initial version
===========================================================================*/

#ifndef LTE_TLB_MSG_H
#define LTE_TLB_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <msgr.h>
#include <msgr_lte.h>
#include <lte_app.h>
#include <msgcfg.h>
#include <msg.h>


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @define diag F3 macros for MAC TLB task */
#define LTE_TLB_MSG_SPRINTF_0(xx_ss_mask, xx_fmt)   \
  MSG(MSG_SSID_LTE, xx_ss_mask, xx_fmt)

#define   LTE_TLB_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_LTE, xx_ss_mask, xx_fmt, xx_arg1)

#define   LTE_TLB_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_LTE, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

#define   LTE_TLB_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_LTE, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define   LTE_TLB_MSG_SPRINTF_4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_LTE, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

#define   LTE_TLB_MSG_SPRINTF_5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
  MSG_5(MSG_SSID_LTE, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5)

#define   LTE_TLB_MSG_SPRINTF_6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_LTE, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6)

/*-----------------------------------------------------------------------*/
/*     LTE TLB External Indication messages                              */
/*-----------------------------------------------------------------------*/
/*! @brief external Indications that the LTE TLB module sends  */
enum
{
  LTE_TLB_FIRST_IND = MSGR_UMID_BASE(MSGR_LTE_TLB, MSGR_TYPE_IND),

  /* Indication send to DS to infor TLB is activated */
  MSGR_DEFINE_UMID(LTE,TLB,IND,LB_ACT,0x00,none),
  /* Indication send to DS to infor TLB is closed */
  MSGR_DEFINE_UMID(LTE,TLB,IND,LB_CLOSE_TL,0x01,none),
  /* Indication send to DS to infor TLB is deactivated */
  MSGR_DEFINE_UMID(LTE,TLB,IND,LB_DEACT,0x02,none),
  /* Indication from NAS which contains message for TLB. This should be
  moved to NAS, but for now, we can leave it here */
  MSGR_DEFINE_UMID(LTE,TLB,IND,DL_INFO,0x03,lte_tlb_dl_info_ind_s),
  /* Indication send to DS to inform TLB open test loop*/
  MSGR_DEFINE_UMID(LTE,TLB,IND,LB_OPEN,0x04,lte_tlb_lb_open_ind_s),
  /* Indication from DS to inform TLB that DS is ready for incoming packets*/
  MSGR_DEFINE_UMID(LTE,TLB,IND,DS_CTL_READY,0x05,lte_tlb_ds_close_tl_ready_ind_s),
  /* Indication to RRC to inform about Location Update Ind*/
  MSGR_DEFINE_UMID(LTE,TLB,IND,RRC_LOCATION_UPDATE,0x06,lte_tlb_ctrl_update_loc_info_s),
  LTE_TLB_MAX_IND,
  LTE_TLB_LAST_IND       = LTE_TLB_MAX_IND - 1
};

/*-----------------------------------------------------------------------*/
/*     LTE TLB External Request messages                              */
/*-----------------------------------------------------------------------*/
/*! @brief external Request that the LTE TLB module sends  */
enum
{
  LTE_TLB_FIRST_REQ = MSGR_UMID_BASE(MSGR_LTE_TLB, MSGR_TYPE_REQ),

  MSGR_DEFINE_UMID(LTE,TLB,REQ,UL_INFO,0x00,lte_tlb_ul_info_req_s),
  MSGR_DEFINE_UMID(LTE,TLB,REQ,RRC_CFG,0x01,lte_tlb_rrc_cfg_req_s),
  
  LTE_TLB_MAX_REQ,
  LTE_TLB_LAST_REQ       = LTE_TLB_MAX_REQ - 1
};

/*! @brief external confirmations that the LTE TLB module receives  */
enum
{
  LTE_TLB_FIRST_CNF = MSGR_UMID_BASE(MSGR_LTE_TLB, MSGR_TYPE_CNF),

  MSGR_DEFINE_UMID(LTE,TLB,CNF,RRC_CFG,0x00,lte_tlb_rrc_cfg_cnf_s),

  LTE_TLB_MAX_CNF,
  LTE_TLB_LAST_CNF       = LTE_TLB_MAX_CNF - 1
};

/*! @brief Supervisory messages that TLB receives or throws
*/
enum
{
  MSGR_DEFINE_UMID(LTE, TLB, SPR, LOOPBACK, MSGR_ID_LOOPBACK, 
                   msgr_spr_loopback_s),
  MSGR_DEFINE_UMID(LTE, TLB, SPR, LOOPBACK_REPLY, MSGR_ID_LOOPBACK_REPLY, 
                   msgr_spr_loopback_reply_s),
  MSGR_DEFINE_UMID(LTE, TLB, SPR, THREAD_READY, LTE_APP_ID_THREAD_READY, 
                   none),
  MSGR_DEFINE_UMID(LTE, TLB, SPR, THREAD_KILL, LTE_APP_ID_THREAD_KILL, 
                   none)
};

#endif /* LTE_TLB_MSG_H */

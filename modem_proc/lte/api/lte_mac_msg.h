/*!
  @file
  lte_mac_msg.h

  @brief
  This file contains all External message IDs interfaces exported by MAC layer.


*/

/*===========================================================================

  Copyright (c) 2014 QUALCOMM Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/lte_mac_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/23/14   sb      CR772190: Extend A2 diag framework for L2
06/12/14   ru      CR665154: SRLTE: Fast retransmission of UL RLC packets lost 
                   due to HARQ failures in Tuneaway
12/10/12   st      CR399603: Added a new API for handling VoLTE LCID
09/20/11   st      Added 2 new eMBMS related messages from MAC DL to RLC
07/13/11   bn      Added in MAC support for modem statistics
05/17/10   bn      Added in MAC/RRC API LTE_MAC_RELEASE_RESOURCES_IND
04/21/10   bn      Added in start/stop request structures
04/20/10   bn      Added in support for START/STOP MAC dynamic mem allocation
03/31/10   bn      Added in request for Up Link Flow Control
08/13/09   bn      Updated IND enums to accommodate SEND_PHR_FAILED_IND
07/28/09   bn      Put back the changes for lte common indication
07/27/09   bn      Removed the dependencies on lte_a2_phy_dl_msg.h
07/09/09   bn      Moved the mac contention indication to lte common inc.
12/18/08   dineshd Added UMID for CancelConnReq (lte_mac_cancel_conn_req_msg_s)
08/22/08   ahe     Removed emptyp enum for GSSAA.
05/15/08   bn      Initial version
===========================================================================*/

#ifndef LTE_MAC_MSG_H
#define LTE_MAC_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <msgr.h>
#include <msgr_lte.h>
#include <lte_app.h>
#include <lte_ind_msg.h>
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*-----------------------------------------------------------------------*/
/*     LTE MAC Supervisory                                               */
/*-----------------------------------------------------------------------*/
/*! @brief Supervisory messages the LTE MAC module receives.  */

/*-----------------------------------------------------------------------*/
/*     LTE MAC External Request messages                                 */
/*-----------------------------------------------------------------------*/
/*! @brief external requests that the LTE MAC module receives.  */
enum
{
  LTE_MAC_FIRST_REQ = MSGR_UMID_BASE(MSGR_LTE_MAC, MSGR_TYPE_REQ),

  MSGR_DEFINE_UMID(LTE,MAC,REQ,ACCESS,0x00,
                   lte_mac_access_req_msg_s),
  MSGR_DEFINE_UMID(LTE,MAC,REQ,CFG,0x01,
                   lte_mac_cfg_req_msg_s),
  MSGR_DEFINE_UMID(LTE,MAC,REQ,ACCESS_ABORT,0x02,
                   lte_mac_access_abort_req_msg_s),
  MSGR_DEFINE_UMID(LTE,MAC,REQ,CANCEL_CONN,0x03,
                   lte_mac_cancel_conn_req_msg_s),
  MSGR_DEFINE_UMID(LTE,MAC,REQ,FC_UL,0x04,
                   none),
  MSGR_DEFINE_UMID(LTE,MAC,REQ,START,0x05,
                   lte_mac_start_req_msg_s),
  MSGR_DEFINE_UMID(LTE,MAC,REQ,STOP,0x06,
                   lte_mac_stop_req_msg_s),
  MSGR_DEFINE_UMID(LTE,MAC,REQ,STATS_UPDATE,0x07,
                   lte_mac_stats_update_req_msg_s),
  MSGR_DEFINE_UMID(LTE,MAC,REQ,RACH_RPT,0x08,
                   lte_mac_rach_rpt_req_msg_s),
  LTE_MAC_MAX_REQ,
  LTE_MAC_LAST_REQ       = LTE_MAC_MAX_REQ - 1
};

/*-----------------------------------------------------------------------*/
/*     LTE MAC External Confirmation messages                            */
/*-----------------------------------------------------------------------*/
/*! @brief external Confirmations that the LTE MAC module sends  */
enum
{
  LTE_MAC_FIRST_CNF = MSGR_UMID_BASE(MSGR_LTE_MAC, MSGR_TYPE_CNF),

  MSGR_DEFINE_UMID(LTE,MAC,CNF,ACCESS,0x00,
                   lte_mac_access_cnf_msg_s),
  MSGR_DEFINE_UMID(LTE,MAC,CNF,CFG,0x01,
                   lte_mac_cfg_cnf_msg_s),
  MSGR_DEFINE_UMID(LTE,MAC,CNF,ACCESS_ABORT,0x02,
                   lte_mac_access_abort_cnf_msg_s),
  MSGR_DEFINE_UMID(LTE,MAC,CNF,START,0x03,
                   lte_mac_start_cnf_msg_s),
  MSGR_DEFINE_UMID(LTE,MAC,CNF,STOP,0x04,
                   lte_mac_stop_cnf_msg_s),
  MSGR_DEFINE_UMID(LTE,MAC,CNF,STATS_UPDATE,0x05,
                   lte_mac_stats_update_cnf_msg_s),
  MSGR_DEFINE_UMID(LTE,MAC,CNF,RACH_RPT,0x06,
                   lte_mac_rach_rpt_cnf_msg_s),

  LTE_MAC_MAX_CNF,
  LTE_MAC_LAST_CNF       = LTE_MAC_MAX_CNF - 1
};

/*-----------------------------------------------------------------------*/
/*     LTE MAC External Indication messages                              */
/*-----------------------------------------------------------------------*/
/*! @brief external Indications that the LTE MAC module sends  */
enum
{
  /* note that all the truely broastcast indications will be defined
  in lte_ind_msg.h. The IDNUM range is from 0x00 to the one below - 1 */
  MSGR_DEFINE_UMID(LTE,MAC,IND,RANDOM_ACCESS_PROBLEM,0x02,
                   lte_mac_random_access_problem_ind_msg_s),
  MSGR_DEFINE_UMID(LTE,MAC,IND,RLC_DL_DATA,0x3, 
                   msgr_hdr_s),
  MSGR_DEFINE_UMID(LTE,MAC,IND,RRC_PCCH_DL_DATA,0x04, 
                   lte_mac_rrc_dl_data_ind_s),
  MSGR_DEFINE_UMID(LTE,MAC,IND,RRC_CCCH_DL_DATA,0x05, 
                   lte_mac_rrc_dl_data_ind_s),
  MSGR_DEFINE_UMID(LTE,MAC,IND,RRC_BCCH_DL_DATA,0x06, 
                   lte_mac_rrc_dl_data_ind_s),
  MSGR_DEFINE_UMID(LTE,MAC,IND,A2_DL_PHY_DATA,0x07, 
                   lte_mac_a2_dl_phy_data_ind_s),
  MSGR_DEFINE_UMID(LTE,MAC,IND,RELEASE_RESOURCES,0x08, 
                   lte_mac_release_resources_ind_s),
  MSGR_DEFINE_UMID(LTE,MAC,IND,BCAST_DATA,0x09, 
                   lte_mac_bcast_data_ind_s),
  MSGR_DEFINE_UMID(LTE,MAC,IND,DUAL_DATA,0xA, 
                   lte_mac_dual_data_ind_s),
  MSGR_DEFINE_UMID(LTE,MAC,IND,LCID_PRIORITY,0xB, 
                   lte_mac_lcid_priority_ind_s),
  MSGR_DEFINE_UMID(LTE,MAC,IND,MAX_HARQ_RETX,0xC,
                   lte_mac_max_harq_retx_ind_s),
  MSGR_DEFINE_UMID(LTE,MAC,IND,DIAG_DATA,0xD,
                   lte_mac_diag_data_ind_s)
};

/*! @brief Supervisory messages that MAC receives or throws
*/
enum
{
  MSGR_DEFINE_UMID(LTE, MAC, SPR, LOOPBACK, MSGR_ID_LOOPBACK, 
                   msgr_spr_loopback_s),
  MSGR_DEFINE_UMID(LTE, MAC, SPR, LOOPBACK_REPLY, MSGR_ID_LOOPBACK_REPLY, 
                   msgr_spr_loopback_reply_s),
  MSGR_DEFINE_UMID(LTE, MAC, SPR, THREAD_READY, LTE_APP_ID_THREAD_READY, 
                   none),
  MSGR_DEFINE_UMID(LTE, MAC, SPR, THREAD_KILL, LTE_APP_ID_THREAD_KILL, 
                   none)
};

#endif /* LTE_MAC_MSG_H */

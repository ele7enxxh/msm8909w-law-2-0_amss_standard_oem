#ifndef CXM_MSGR_H
#define CXM_MSGR_H
/*!
  @file
  cxm_msgr.h

  @brief
  APIs for CXM's MSGR interface

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cxm/inc/cxm_msgr.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/08/12   ckk     Initial implementation

==========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "msgr.h"
#include <msgr_rex.h>
#include "cxm.h"
#include "wcn_coex_mgr.h"
#include "wwan_coex_mgr.h"

/*! @brief This is a union of all the messages that CXM is interested in */
typedef union
{
  msgr_hdr_struct_type                      hdr;            /*!< Used for Empty message packet */
  cxm_coex_generic_req_s                    lte_boot_params_req;
  cxm_coex_tech_tx_adv_ntc_ind_s            tech_tx_sfn_ind;
  cxm_coex_metrics_lte_bler_ind_s           lte_bler_ind;
  cxm_coex_metrics_lte_sinr_rsp_s           lte_sinr_read_cnf;
  cxm_coex_tx_pwr_lmt_lte_cndtns_rsp_s      tx_pwr_lmt_chk_cnf;
  cxm_coex_tx_pwr_lmt_lte_cndtns_ind_s      lte_rrc_state_ind;
  cxm_coex_wcn_txfrndnl_report_ind_s        lte_wcn_report_ind; /*!< LTE WCN report */
  cxm_coex_tech_sleep_wakeup_duration_ind_s wwan_slp_wkp_ind;
  cxm_wwan_tech_state_ind_s                 wwan_tech_state_ind; /*!< WWAN tech state */
  cxm_coex_frame_timing_ind_s               frame_timing_ind;
  cxm_coex_tech_activity_adv_ntc_ind_s      activity_adv_ntc_ind;
  cxm_coex_metrics_rsp_s                    metrics_rsp;
  cxm_coex_high_prio_ind_s                  high_prio_ind;
  cxm_timing_info_s                         cxm_timing_info;
  cxm_coex_log_async_ind_s                  log_async_ind;
} cxm_msgr_rcv_msg_u;

/* CXM message type */
typedef struct
{
  /* qlink for the REX Q associated to this task */
  q_link_type            qlink;

  /* MSGR message within this packet */
  cxm_msgr_rcv_msg_u     msg;

} cxm_msgr_rcv_msg_t;

/* CXM receive queue type */
typedef q_type cxm_msgr_rcv_queue_t;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

  FUNCTION:  cxm_msgr_init

===========================================================================*/
/*!
  @brief
    To initialize MSGR client & setup associated queue(s)

  @return
    errno_enum_type
*/
/*=========================================================================*/
errno_enum_type cxm_msgr_init (
  void
);

/*===========================================================================

  FUNCTION:  cxm_msgr_deinit

===========================================================================*/
/*!
  @brief
    To de-initialize MSGR client & associated queue(s)

  @return
    errno_enum_type
*/
/*=========================================================================*/
errno_enum_type cxm_msgr_deinit (
  void
);

/*===========================================================================

  FUNCTION:  cxm_msgr_register_msgs

===========================================================================*/
/*!
  @brief
    To register for all related UMIDs

  @return
    None
*/
/*=========================================================================*/
void cxm_msgr_register_msgs (
  void
);

/*===========================================================================

  FUNCTION:  cxm_msgr_deregister_msgs

===========================================================================*/
/*!
  @brief
    To deregister for all related UMIDs

  @return
    None
*/
/*=========================================================================*/
void cxm_msgr_deregister_msgs (
  void
);

/*===========================================================================

  FUNCTION:  cxm_msgr_send_msg

===========================================================================*/
/*!
  @brief
    Send a message via MSGR

  @return
    errno_enum_type
*/
/*=========================================================================*/
errno_enum_type cxm_msgr_send_msg (
  msgr_hdr_struct_type *msg_hdr_ptr,  /*!< Pointer to MSG Header  */
  msgr_umid_type        umid,         /*!< UMID of the message    */
  uint32                size          /*!< Size of the message    */
);

/*===========================================================================

  FUNCTION:  cxm_msgr_process_sig

===========================================================================*/
/*!
  @brief
    Process the CXM's MSGR REX signal & associated queue

  @return
    None
*/
/*=========================================================================*/
void cxm_msgr_process_sig (
  void
);

#endif /* CXM_MSGR_H */

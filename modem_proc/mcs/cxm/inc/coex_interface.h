#ifndef __COEX_INTERFACE_H__
#define __COEX_INTERFACE_H__
/*!
  @file coex_interface.h

  @brief
   APIs exposed by the COEX Layer

*/

/*=============================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
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

=============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cxm/inc/coex_interface.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/20/15   ckk     Update type7[rrc_c] using RAT layer 3 states
01/14/15   ckk     Add support for Tx antenna via WCI2 MWS_Connected Type 7
07/14/14   ckk     Added WCI2 type4 & type7[pwr_bit] support
01/10/13   tak     Added CA and TDS support for WWAN update and sleep indication 
12/19/12   btl     Port power limiting and bug fixes
04/08/12   ckk     Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include <IxErrno.h>
#include <qmi_csi.h>

#include "cxm_msgr.h"

/*=============================================================================

                       CONSTANTS AND DEFINES

=============================================================================*/


/*=============================================================================

                        FUNCTION DECLARATIONS

=============================================================================*/

/*=============================================================================

  FUNCTION:  coex_service_init

=============================================================================*/
/*!
    @brief
    Initializes COEX layer's functionalities
 
    @return
    None
*/
/*===========================================================================*/
void coex_service_init (
  void
);

/*=============================================================================

  FUNCTION:  coex_service_deinit

=============================================================================*/
/*!
    @brief
    Cleans up/tears down the underlying transport layer functionalities

    @return
    None
*/
/*===========================================================================*/
void coex_service_deinit (
  void
);

/*=============================================================================

  FUNCTION:  coex_qcsi_process_req

=============================================================================*/
/*!
    @brief
    Callback function called by QCSI infrastructure when a REQ message is received
 
    @note
    QCSI infrastructure decodes the data before forwarding it to this layer
 
    @return
    qmi_csi_cb_error
*/
/*===========================================================================*/
qmi_csi_cb_error coex_qcsi_process_req (
  void           *connection_handle,
  qmi_req_handle  req_handle,
  unsigned int    msg_id,
  void           *req_c_struct,
  unsigned int    req_c_struct_len,
  void           *service_cookie
);

/*=============================================================================

  FUNCTION:  coex_update_wwan_state_info

=============================================================================*/
/*!
    @brief
    Function to update the WWAN state info & if needed send out the 
	COEX_WWAN_STATE info IND message
 
    @return
    errno_enum_type
*/
/*===========================================================================*/
errno_enum_type coex_update_wwan_state_info_ind (
  void *rcv_msgr_msg_ptr
);

/*=============================================================================

  FUNCTION:  coex_handle_sleep_wakeup_ind

=============================================================================*/
/*!
    @brief
    Function to send out the sleep or wakeup indication message and
    send through QMI
 
    @return
    errno_enum_type
*/
/*===========================================================================*/
errno_enum_type coex_handle_sleep_wakeup_ind (
  void *rcv_msgr_msg_ptr
);

/*=============================================================================

  FUNCTION:  coex_handle_boot_params_req

=============================================================================*/
/*!
    @brief
    Function to send the boot parameters to ML1 and FW
 
    @return
*/
/*===========================================================================*/
errno_enum_type coex_handle_params_req (
  void *rcv_msgr_msg_ptr
);


/*=============================================================================

  FUNCTION:  coex_handle_tx_adv_ntc_ind

=============================================================================*/
/*!
    @brief
    Function to send out wci2 type 6 msg when SFN IND received

    @return qmi_csi_cb_error
*/
/*===========================================================================*/
errno_enum_type coex_handle_tx_adv_ntc_ind (
  void *rcv_msgr_msg_ptr
);

/*=============================================================================

  FUNCTION:  coex_handle_activity_timeline_ind

=============================================================================*/
/*!
    @brief
    Handle activity timeline message 

    @return
    errno_enum_type
*/
/*===========================================================================*/
errno_enum_type coex_handle_activity_timeline_ind (
  void *rcv_msgr_msg_ptr
);

/*=============================================================================

  FUNCTION:  coex_handle_bler_ind

=============================================================================*/
/*!
    @brief
    Forward BLER indication from LTE to AP over QMI as part of metrics

    @return qmi_csi_cb_error
*/
/*===========================================================================*/
errno_enum_type coex_handle_bler_ind (
  void *rcv_msgr_msg_ptr
);

/*=============================================================================

  FUNCTION:  coex_handle_lte_sinr_read_cnf

=============================================================================*/
/*!
    @brief
    Function to send QMI message with SINR stats from LTE

    @return qmi_csi_cb_error
*/
/*===========================================================================*/
errno_enum_type coex_handle_lte_sinr_read_cnf (
  void *rcv_msgr_msg_ptr
);

/*=============================================================================

  FUNCTION:  coex_handle_metrics_read_rsp

=============================================================================*/
/*!
    @brief
    Function to send QMI message with metrics stats

    @return qmi_csi_cb_error
*/
/*===========================================================================*/
errno_enum_type coex_handle_metrics_read_rsp (
  void *rcv_msgr_msg_ptr
);

/*=============================================================================

  FUNCTION:  coex_handle_lte_tx_power_limit_cnf

=============================================================================*/
/*!
    @brief
    Function to handle the response to the request to LTE ML1 for power
    limiting conditions. Evaluates conditions and starts power limiting if
    appropriate

    @return
    errno_enum_type
*/
/*===========================================================================*/
errno_enum_type coex_handle_lte_tx_power_limit_rsp (
  void *rcv_msgr_msg_ptr
);

/*=============================================================================

  FUNCTION:  coex_handle_lte_ml1_rrc_state_ind

=============================================================================*/
/*!
    @brief
    Function to handle indication that RRC procedure is in progress. If we
    are power limiting, stop and send condition fail

    @return
    errno_enum_type
*/
/*===========================================================================*/
errno_enum_type coex_handle_lte_tx_power_limit_ind (
  void *rcv_msgr_msg_ptr
);

/*=============================================================================

  FUNCTION:  coex_handle_lte_wcn_report_ind

=============================================================================*/
/*!
    @brief
    Handle message from ML1 to CXM reporting FW WCN threshold exceeded

    @return
    errno_enum_type
*/
/*===========================================================================*/
errno_enum_type coex_handle_wcn_txfrndnl_report_ind (
  void *rcv_msgr_msg_ptr
);

/*=============================================================================

  FUNCTION:  coex_handle_tech_wcn_txfrndnl_report_ind

=============================================================================*/
/*!
    @brief
    Handle message from tech L1 to CXM reporting FW WCN threshold exceeded

    @return
    errno_enum_type
*/
/*===========================================================================*/
errno_enum_type coex_handle_tech_wcn_txfrndnl_report_ind (
  void *rcv_msgr_msg_ptr
);

/*=============================================================================

  FUNCTION:  coex_handle_activity_adv_ntc

=============================================================================*/
/*!
    @brief
    Handle message from L1 to CXM reporting activity in advance

    @return
    errno_enum_type
*/
/*===========================================================================*/
errno_enum_type coex_handle_activity_adv_ntc (
  void *rcv_msgr_msg_ptr
);

/*=============================================================================

  FUNCTION:  coex_handle_high_priority_ind

=============================================================================*/
/*!
    @brief
    Message from WWAN techs informing of high priority events.

    @return
    errno_enum_type
*/
/*===========================================================================*/
errno_enum_type coex_handle_high_priority_ind(
  void *rcv_msgr_msg_ptr
);

/*=============================================================================

  FUNCTION:  coex_handle_tech_tx_pwr_report_ind

=============================================================================*/
/*!
    @brief
    Message from WWAN techs informing of the Tx power report.

    @return
    errno_enum_type
*/
/*===========================================================================*/
errno_enum_type coex_handle_tech_tx_pwr_report_ind(
  void *rcv_msgr_msg_ptr
);

/*=============================================================================

  FUNCTION:  coex_handle_tx_ant_sel_ind

=============================================================================*/
/*!
    @brief
    Handler to set transmit (Tx) antenna selection message.

    @return
    errno_enum_type
*/
/*===========================================================================*/
errno_enum_type coex_handle_tx_ant_sel_ind(
  void *rcv_msgr_msg_ptr
);

/*=============================================================================
û
  FUNCTION:  coex_handle_tech_l3_state_ind

=============================================================================*/
/*!
    @brief
    Message from WWAN techs informing of their layer 3 (RRC/CP/RR) state.

    @return
    errno_enum_type
*/
/*===========================================================================*/
errno_enum_type coex_handle_tech_l3_state_ind(
  void *rcv_msgr_msg_ptr
);

#endif /* __COEX_INTERFACE_H__ */

#ifndef __COEX_ALGOS_V2_H__
#define __COEX_ALGOS_V2_H__
/*!
  @file
  coex_algos_v2.c

  @brief
  This file contains the msg handlers for CoEx algorithms (version 2 of
  QMI COEX interface)

*/

/*===========================================================================

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

===========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cxm/inc/coex_algos_v2.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
03/18/14   btl     Add LTE SINR support
11/19/13   btl     Add antenna sharing
08/09/13   btl     Initial version

==========================================================================*/
/*=============================================================================

                      INCLUDES

=============================================================================*/
#include <coexistence_service_v02.h>
#include "cxm.h"
#include "wcn_coex_mgr.h"
#include "coex_algos.h"

/*=============================================================================

                      TYPES AND DEFINES

=============================================================================*/
/*=============================================================================

                      EXTERNAL FUNCTION DECLARATIONS

=============================================================================*/

/*=============================================================================

  FUNCTION:  coex_qmb_msg_timer_handler

=============================================================================*/
/*!
    @brief
    Timer handler for QMB recv msg timeout, set when we expect a response
    back from the WCN within a certain time

    @return
    void
*/
/*===========================================================================*/
void coex_qmb_msg_timer_handler(
  void
);

/*=============================================================================

  FUNCTION:  coex_qmb_nv_timer_handler

=============================================================================*/
/*!
    @brief
    Timer handler for QMB NV exchange timeout. When timeout occurs,
    we can stop holding the UART powered on

    @return
    void
*/
/*===========================================================================*/
void coex_qmb_nv_timer_handler( void );

/*=============================================================================

  FUNCTION:  coex_process_conflicts_v2

=============================================================================*/
/*!
    @brief
    Searches through WWAN and WCN state data and determines if there is a
    conflict then carries out appropriate action

    @detail
    Searches through current state to determine if there is a conflict. The
    search is based on the tech passed in since it will only search for conflicts
    that may be associated with this tech.

    @return
    boolean new_conflict_found
*/
/*===========================================================================*/
void coex_process_conflicts_v2 (
  cxm_tech_type     wwan_tech,
  cxm_wcn_tech_type wcn_tech,
  uint64           *max_conf_prio,
  uint8            *max_conf_prio_vctbl_idx
);

/*=============================================================================

  FUNCTION:  coex_algos_init_v2

=============================================================================*/
/*!
    @brief
    Initialize resources for COEX QMI V2 msg flows

    @return
    void
*/
/*===========================================================================*/
void coex_algos_init_v2(
  coex_config_params_v8 * coex_params
);

/*=============================================================================

  FUNCTION:  coex_algos_deinit_v2

=============================================================================*/
/*!
    @brief
    Deinitialize resources for COEX QMI v2 msg flows

    @return
    void
*/
/*===========================================================================*/
void coex_algos_deinit_v2(
  void
);

/*=============================================================================

  FUNCTION:  coex_handle_update_req_v02

=============================================================================*/
/*!
    @brief
    respond to the request for the state info of the given tech

    @return
    void
*/
/*===========================================================================*/
void coex_handle_update_req_v02(
  void* msg,
  unsigned int msg_len
);

/*=============================================================================

  FUNCTION:  coex_handle_wlan_wifi_state_ind_v02

=============================================================================*/
/*!
    @brief
    Update our copy of WIFI state with new WIFI state info

    @return
    void
*/
/*===========================================================================*/
void coex_handle_wlan_wifi_state_ind_v02(
  void* msg,
  unsigned int msg_len
);

/*=============================================================================

  FUNCTION:  coex_handle_sync_state_req_v02

=============================================================================*/
/*!
    @brief
    handle sync msg to synchronize the state between WWAN & WLAN modules

    @return
    void
*/
/*===========================================================================*/
void coex_handle_sync_state_req_v02( 
  void* msg,
  unsigned int msg_len
);

/*=============================================================================

  FUNCTION:  coex_handle_sync_state_resp_v02

=============================================================================*/
/*!
    @brief
    handle a TECH_SYNC_RESP msg (in response to a REQ we sent earlier)

    @return
    void
*/
/*===========================================================================*/
void coex_handle_sync_state_resp_v02(
  void* msg,
  unsigned int msg_len
);

/*=============================================================================

  FUNCTION:  coex_handle_sync_update_ind_v02

=============================================================================*/
/*!
    @brief
    handle QMI_COEX_TECH_SYNC_UPDATE_IND_V02 from WCN

    @return
    void
*/
/*===========================================================================*/
void coex_handle_sync_update_ind_v02(
  void* msg,
  unsigned int msg_len
);

/*=============================================================================

  FUNCTION:  coex_send_diversity_antenna_state_ind_v02

=============================================================================*/
/*!
    @brief
    Inform of the state of the diversity antenna chain. Sent over QMB.

    @return
    void
*/
/*===========================================================================*/
void coex_send_diversity_antenna_state_ind_v02(
  void
);

/*=============================================================================

  FUNCTION:  coex_handle_metrics_lte_sinr_start_ind_v02

=============================================================================*/
/*!
    @brief
    Function to handle the client's query request to start collecting/
    collating LTE SINR metrics

    @return
    void
*/
/*===========================================================================*/
void coex_handle_metrics_lte_sinr_start_ind_v02(
  void* msg,
  unsigned int msg_len
);

/*=============================================================================

  FUNCTION:  coex_handle_metrics_lte_sinr_read_req_v02

=============================================================================*/
/*!
    @brief
    Function to handle the client's query request to read current filter
    output for LTE SINR metrics

    @return
    void
*/
/*===========================================================================*/
void coex_handle_metrics_lte_sinr_read_req_v02(
  void* msg,
  unsigned int msg_len
);

/*=============================================================================

  FUNCTION:  coex_handle_metrics_lte_sinr_stop_ind_v02

=============================================================================*/
/*!
    @brief
    Function to handle teh client's query request to stop collecting/collating
    LTE SINR metrics

    @return
    void
*/
/*===========================================================================*/
void coex_handle_metrics_lte_sinr_stop_ind_v02( 
  void* msg, 
  unsigned int msg_len 
);

/*=============================================================================

  FUNCTION:  coex_update_wwan_state_info_msg_v2

=============================================================================*/
/*!
    @brief
    Update the local WWAN state info for the given tech

    @return
    void
*/
/*===========================================================================*/
void coex_process_wwan_state_info_msg_v2(
  cxm_tech_type    tech_id,
  cxm_tech_data_s *tech_data
);

/*=============================================================================

  FUNCTION:  coex_process_wwan_sleep_wake_v2

=============================================================================*/
/*!
    @brief
    Process the sleep/wakeup indication from the WWAN tech appropriately.
    If a COEX tech, send the appropriate SYNC message to WLAN.

    @return
    void
*/
/*===========================================================================*/
void coex_process_wwan_sleep_wake_v2( 
  cxm_coex_tech_sleep_wakeup_duration_ind_s* msg_ptr
);

/*=============================================================================

  FUNCTION:  coex_handle_lte_sinr_read_cnf_v2

=============================================================================*/
/*!
    @brief
    Function to send QMB message with LTE SINR stats

    @return
    void
*/
/*===========================================================================*/
errno_enum_type coex_handle_lte_sinr_read_cnf_v2( 
  void *rcv_msgr_msg_ptr
);

/*=============================================================================

  FUNCTION:  coex_handle_conflict_params_req_v02

=============================================================================*/
/*!
    @brief
    Handle the client's query for NV coex conflict params

    @return
    void
*/
/*===========================================================================*/
void coex_handle_conflict_params_req_v02(
  void* msg,
  unsigned int msg_len
);

#endif /* __COEX_ALGOS_V2_H__ */

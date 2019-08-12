#ifndef __LMTSMGR_CMD_H__
#define __LMTSMGR_CMD_H__
/*!
  @file lmtsmgr_cmd.h

  @brief
   Command structures exposed by the LMTSMGR Layer

*/

/*=============================================================================

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

=============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/core/inc/lmtsmgr_cmd.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/10/14   ag      added tech in distress indication
09/23/14   jm      FreqId generation support in sim mode
08/29/14   jm      Change SL TL sim to use FreqId Pointer
08/12/14   jm      Slot level timeline support
06/23/14   sg      desense indication mechanism
04/28/14   jm      L+G Band Avoidance support
03/17/14   jm      WWAN coex based off of RF device assignment
09/24/12   ag      Moved to msgr interface 
05/17/12   ag      Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "cxm.h"
#include "comdef.h"

/*=============================================================================

                       CONSTANTS AND DEFINES

=============================================================================*/

/*=============================================================================

  FUNCTION:  lmtsmgr_handle_tech_tx_on

=============================================================================*/
/*!
    @brief
    Sets the Tx state of the tech and triggers LMTSMGR core to run.
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_handle_tech_tx_on
(
  void *lmtsmgr_rcvd_msg /* struct we have recieved and are updating for */
);

/*=============================================================================

  FUNCTION:  lmtsmgr_handle_tech_tx_on_with_chs

=============================================================================*/
/*!
    @brief
    Sets the Tx state of the tech and triggers LMTSMGR core to run. Accepts an
    array of channels
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_handle_tech_tx_on_with_chs
(
  void *lmtsmgr_rcvd_msg /* struct we have recieved and are updating for */
);

/*=============================================================================

  FUNCTION:  lmtsmgr_handle_tech_power_update

=============================================================================*/
/*!
    @brief
    Updates the power information of the tech and then figures out if anything
    needs to be transferred to the LMTSMGR Server.
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_handle_tech_power_update
(
   void *lmtsmgr_rcvd_msg /* struct we have recieved and are updating for */
);
/*=============================================================================

  FUNCTION:  lmtsmgr_handle_tech_tx_off

=============================================================================*/
/*!
    @brief
    Clears the Tx state of the tech and triggers LMTSMGR core to run.
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_handle_tech_tx_off
(
  void *lmtsmgr_rcvd_msg /* struct we have recieved and are updating for */
);

/*=============================================================================

  FUNCTION:  lmtsmgr_handle_tech_state_ind

=============================================================================*/
/*!
    @brief
    Handles the Tx state and frequency indications
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_handle_tech_state_ind
(
  void *lmtsmgr_rcvd_msg, /* struct we have recieved and are updating for */
  boolean is_sim_msg
);

/*=============================================================================

  FUNCTION:  lmtsmgr_handle_coex_power_update

=============================================================================*/
/*!
    @brief
    Handles the coex power indications
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_handle_coex_power_update
(
  void *lmtsmgr_rcvd_msg /* struct we have recieved and are updating for */
);

/*=============================================================================

  FUNCTION:  lmtsmgr_handle_activity_tl_update

=============================================================================*/
/*!
    @brief
    Handles the activity timeline indications
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_handle_activity_tl_update
(
  void *lmtsmgr_rcvd_msg /* struct we have recieved and are updating for */
);

/*=============================================================================

  FUNCTION:  lmtsmgr_handle_inactivity_reg

=============================================================================*/
/*!
    @brief
    Handles the inactivity registrations
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_handle_inactivity_reg
(
  void *lmtsmgr_rcvd_msg /* struct we have recieved and are updating for */
);

/*=============================================================================

  FUNCTION:  lmtsmgr_handle_chain_owner_upd_ind

=============================================================================*/
/*!
    @brief
    Handles the chain owner updates
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_handle_chain_owner_upd_ind
(
  void *lmtsmgr_rcvd_msg /* struct we have recieved and are updating for */
);

/*=============================================================================

  FUNCTION:  lmtsmgr_handle_band_avoidance_freq_ind

=============================================================================*/
/*!
    @brief
    Handles the serving frequency updates (for band avoidance)
 
    @details
    If the serving frequency message is invalid in any form, it will be ignored. 
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_handle_band_avoidance_freq_ind
(
  void *lmtsmgr_rcvd_msg /* struct we have recieved and are updating for */
);

/*=============================================================================

  FUNCTION:  lmtsmgr_handle_band_avoidance_pwr_ind

=============================================================================*/
/*!
    @brief
    Handles the serving power updates (for band avoidance)
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_handle_band_avoidance_pwr_ind
(
  void *lmtsmgr_rcvd_msg /* struct we have recieved and are updating for */
);

/*==============================================================================

  FUNCTION:  lmtsmgr_handle_desense_ind_req

==============================================================================*/
/*!
    @brief
    process registration for desense indication.

    @detail
    Clients could register to get updates about desense indication using MSGR.
    This API process the request from MSGR.
    When a coarse level desense is deteced MCS would update the registered clients

    @return
    None
*/
/*============================================================================*/
void lmtsmgr_handle_desense_ind_req
(
  void* desense_req   /*!< pointer to the registration message */
);


/*=============================================================================

  FUNCTION:  lmtsmgr_handle_band_avoidance_blist_rsp

=============================================================================*/
/*!
    @brief
    Handles the band avoidance response from Tech L1
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_handle_band_avoidance_blist_rsp
(
  void *lmtsmgr_rcvd_msg /* struct we have recieved and are updating for */
);

/*=============================================================================
  
FUNCTION:  lmtsmgr_handle_distress_ind

=============================================================================*/
/*!
    @brief
    Handles the distress indication from Tech L1
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_handle_distress_ind
(
  void *lmtsmgr_rcvd_msg /* struct we have recieved and are updating for */
);

/*=============================================================================

  FUNCTION:  lmtsmgr_handle_slot_lvl_act_ind

=============================================================================*/
/*!
    @brief
    Handles the Tech Tx/Rx slot level activity indication from Tech L1
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_handle_slot_lvl_act_ind
(
  void *lmtsmgr_rcvd_msg /* struct we have recieved and are updating for */
);

/*=============================================================================

  FUNCTION:  lmtsmgr_handle_slot_lvl_act_sim_ind

=============================================================================*/
/*!
    @brief
    Handles the Tech Tx/Rx slot level activity indication from simulator (FW)
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_handle_slot_lvl_act_sim_ind
(
  void *lmtsmgr_rcvd_msg /* struct we have recieved and are updating for */
);


#endif /* __LMTSMGR_CMD_H__ */

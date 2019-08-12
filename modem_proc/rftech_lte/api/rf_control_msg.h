/*!
  @file
  rf_control_msg.h

  @brief
  Message router UMIDs header for RF task control. This file defines
  interfaces in the RF task control and query. This is a common header for
  rf driver of all technologies.

  @note
  This file must be named with a _msg.h suffix for the build environment
  to correctly process the messages defined within.

*/

/*===========================================================================

  Copyright (c) 2009 -2013 QUALCOMM Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_lte/api/rf_control_msg.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/30/14   kab     Add RF-ML1 MSGs for Scouting APIs, FW Triggered RF Tune
02/12/13   pl      remove redundant RFA_RF messages
07/12/12   pl      Moved FTM command processing to FTM task
                   removed obsolete RFA_RF_LTE_FTM messages
07/12/12   pl      Add FTM LTE CA support
06/07/12   pl      RF LTE MC state machine changed for CA support
04/04/12   pl      Remove obsolete states and comamnds
02/15/12   sar     Replaced FEATURE_RF_HAS_LTE with FEATURE_LTE.
02/07/12   can     Added FTM set modulation command.
12/05/11   sar     Fixed featurization for non-lte build. 
10/17/11   ars     Added FTM_RF_GET_RX_LEVEL_DBM command 
10/17/11   ars     Added FTM_SET_PA_STATE and FTM_SET_TX_POWER_DBM FTM Layer
09/14/11   can     Merge MDM9K tips to NikeL.
06/27/11   ars     Added FTM_SET_TX_POWER_DBM 
04/20/11   pl      Remove obsolete structures
02/09/11   aca     FTM dac setpoint value override support
02/08/11   aca     FTM modulation change support
01/13/11   aca     L2L Message routing interface
12/09/10   pl      Added new error code
07/30/10   tnt     Add message to receive TX_ON indication from ML1
07/01/10   jyu     Added support to power collapse
06/23/10   jyu     Added support to manage rx chain 
10/13/09   mkv     Removed set PA ON command.
10/06/09   mkv     Updated RF Calibration structure definitions.
09/01/09   can     Changes to L1 Interface.
08/26/09   mkv     Moving the rf_dispatch_outcome_t struct definition here.
07/21/09   mkv	   Changed file location from task\posix to task\rex folder.
05/14/09   mkv	   Added LTE FTM message definitions to rf_msg_u
04/15/09   can     Expanded rf_msg_u structure.
04/07/09   qma     Initial checkin and moved to common folder


===========================================================================*/

#ifndef RF_CONTROL_MSG_H
#define RF_CONTROL_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "appmgr_msg.h"
#include "rf_cap.h"
#include "rfa_msgr.h"
#include "msgr.h"
#include "msgr_types.h"
#ifdef FEATURE_LTE
  #include "rflte_msg.h"
  #ifdef FEATURE_RF_HAS_FTM
    #include "rf_lte_ftm_msg.h"
  #endif
#endif

/*===========================================================================

             EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/****************************************************************************
                RF Task Supervisory
****************************************************************************/

/*! @brief Supervisory message IDs the RF task receives and sends 
   This enum is never called by name. It just defines symbols.
*/
enum
{

  /*! Loopback message, payload is defined in msgr_spr_loopback_s */
  MSGR_DEFINE_UMID(RFA, RF_CONTROL, SPR, LOOPBACK, MSGR_ID_LOOPBACK,
                   msgr_spr_loopback_s),

  /*! Loopback reply message, payload is defined in sgr_spr_loopback_reply_s */
  MSGR_DEFINE_UMID(RFA, RF_CONTROL, SPR, LOOPBACK_REPLY, MSGR_ID_LOOPBACK_REPLY, 
                   msgr_spr_loopback_reply_s),

  /*! Thread ready indication - No Payload, just MSGR header */
  MSGR_DEFINE_UMID(RFA, RF_CONTROL, SPR, THREAD_READY, APPMGR_ID_THREAD_READY, 
                   none),

  /*! Thread kill message, no payload */
  MSGR_DEFINE_UMID(RFA, RF_CONTROL, SPR, THREAD_KILL, APPMGR_ID_THREAD_KILL, 
                   none)
};


/*! @brief Union of all RF receiving msgs that RF task can receive
*/

/* Note: The msgr_receive( ) API requires providing the size of the buffer 
to accommodate all receiving messages. Constrained by this buffer size requirement, 
we have to define a union of all receiving messages to consider the largest message 
size for copying of received messages by message router. 

This is dictated by the underlying OS support.  On UMB, we used Qube, which allocated 
the buffer internally, which then required the user to free it after getting the msg out.  
We are now using a Posix interface, which requires that the client provide the buffer.  */

/*! @todo May need to move this to a common_msg.h as the union of 
all receiving msgs (inclduing control, RF Tech subsystem msgs) is required 
due to the restriction on the msgr_receive( ) interface, which needs msgr
buffer size as the argument  */


typedef union
{

  /* Supervisor Requests */
  msgr_spr_loopback_s               msgr_spr_loopback;
  msgr_spr_loopback_reply_s         msgr_spr_loopback_reply;
#ifdef FEATURE_LTE
  /* LTE subsystem messages */
  /*! @todo: Future RF receive message types need to be present here*/
  rfa_rf_lte_enter_mode_req_s                rfa_rf_lte_enter_mode;
  rfa_rf_lte_exit_mode_req_s                 rfa_rf_lte_exit_mode;
  rfa_rf_lte_fdd_rx_config_req_s             rfa_rf_lte_fdd_rx_config;   
  rfa_rf_lte_fdd_tx_config_req_s             rfa_rf_lte_fdd_tx_config;    
  rfa_rf_lte_tdd_rx_config_req_s             rfa_rf_lte_tdd_rx_config;    
  rfa_rf_lte_fdd_tx_disable_req_s            rfa_rf_lte_fdd_disable_tx;     
  rfa_rf_lte_tdd_tx_disable_req_s            rfa_rf_lte_tdd_disable_tx;
  rfa_rf_lte_tdd_tx_config_req_s             rfa_rf_lte_tdd_tx_config;
  rfa_rf_lte_tdd_tx_disable_req_s            rfa_rf_lte_tdd_tx_disable;
//PATLIM  rfa_rf_lte_do_hdet_req_s                rfa_rf_lte_do_hdet;  
  rfa_rf_lte_tx_on_ind_s                     rfa_rf_lte_tx_on;
  rfa_rf_lte_l2l_build_scripts_req_s         rfa_rf_l2l_build_scripts;
  rfa_rf_lte_rx_script_based_config_req_s    rfa_rf_lte_rx_script_based_config;
  rfa_rf_lte_tx_script_based_config_req_s    rfa_rf_lte_tx_script_based_config;  
  rfa_rf_lte_get_single_chain_support_req_s  rfa_rf_lte_get_single_chain_support;
  rfa_rf_lte_check_config_req_s              rfa_rf_lte_check_config;
#endif

#ifdef FEATURE_RF_HAS_FTM
#ifdef FEATURE_LTE
  /*RF LTE FTM command definitions.*/
  rfa_rf_lte_ftm_configure_tx_sweep_cmd_s rfa_rf_lte_ftm_configure_tx_sweep;
#endif
#endif /* FFEATURE_RF_HAS_FTM */
} rf_msg_u;

/*!
  @brief
  Outcome of the dispatch. 

  @details The different outcomes of the dispatch to RF task used by LTE MC Dispatcher and LTE FTM Calibration Dispatch. 

*/
typedef enum
{
  RF_DISPATCH_SUCESSFUL=1,
  RF_DISPATCH_UNSUCESSFUL=2,
  RF_DISPATCH_NULL_PTR=3,
  RF_DISPATCH_INVALID_PARAM =4,
  RF_DISPATCH_MAX
}rf_dispatch_outcome_t;

#endif /* RF_CONTROL_MSG_H */



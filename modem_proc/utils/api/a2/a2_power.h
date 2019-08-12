/*!
  @file
  a2_power.h

  @brief
  The interface to the A2 power collapse feature implementation file

  @author
  araina
*/

/*==============================================================================

  Copyright (c) 2013 QUALCOMM Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/a2/a2_power.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
12/02/15   am      CR945970 Remove compiler warnings
01/08/14   sn      CR776718: qtf changes
09/17/13   vd      CR531369,add a2_power_task_sleep\a2_power_reset_sleep_counter
09/13/13   vd      CR469984: Support for mount/unmount feature
08/06/13   yjz     Add A2_POWER_REQ_VOLTE_SLEEP for modem freeze
02/06/12   yjz     Fix Mob link error
10/19/12   ar      added A2_POWER_CLIENT_A2_IP_FILTER client
10/12/12   ar      added A2_POWER_REQ_SMSM_NAK smsm request
09/14/12   ar      added a2_power_send_ack_and_pc_notification_to_apps() api 
04/16/12   ar      added a2 pc mutex operation related apis
02/13/12   ar      added support for A2 PC + SPS RESET feature
01/06/12   ar      added a2_power_collapse_disabled() api 
                   split the A2_POWER_CLIENT_INTERNAL client into PHY and PER
11/22/11   ar      added support for UL PER client for a2 pc
11/10/11   st      Added A2 power collapse support for the WCDMA RRC module
05/24/09   ar      initial version
==============================================================================*/

#ifndef A2_POWER_H
#define A2_POWER_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include "a2_common.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*!
   @brief
   Clients voting for A2 wake up / shut down
*/
typedef enum
{
  A2_POWER_CLIENT_INTERNAL_PHY   = 0,
  A2_POWER_CLIENT_INTERNAL_PER   = 1,
  A2_POWER_CLIENT_LTE_ML1_UL     = 2,
  A2_POWER_CLIENT_LTE_ML1_DL     = 3,
  A2_POWER_CLIENT_WL1_EUL        = 4,
  A2_POWER_CLIENT_WL1_HS         = 5,
  A2_POWER_CLIENT_WCDMA_RRC      = 6,
  A2_POWER_CLIENT_TDSCDMA_EUL    = 7,
  A2_POWER_CLIENT_TDSCDMA_HS     = 8,
  A2_POWER_CLIENT_DO_UL          = 9,
  A2_POWER_CLIENT_DO_DL          = 10,
  A2_POWER_CLIENT_APPS_SMSM      = 11,
  A2_POWER_CLIENT_A2_UL_PER      = 12,
  A2_POWER_CLIENT_A2_DL_PER      = 13,
  A2_POWER_CLIENT_A2_INACTIVITY  = 14,
  A2_POWER_CLIENT_A2_IP_FILTER   = 15,
  A2_POWER_CLIENT_PC_PENDING_TEMP= 16, /* Temporary client to keep A2 ON during
                                       DL PER PC pending state, while dl per
                                       module finishes the sleep procedure */
  A2_POWER_CLIENT_BOOTUP_TEMP    = 17, /* Temporary client to keep A2 ON during
                                       modem bootup, till DS opens the first sio
                                       port */
  A2_POWER_CLIENT_MOUNT_UNMOUNT  = 18, /* Client for mount/unmount feature */
  A2_POWER_CLIENT_MAX
} a2_power_client_e;

/*!
  This Macro is exposed to other modules. This is used to support the 
  legacy code of other modules. Once other modules implement DSDA chanegs,
  this macro can take an extra argument as sub_id.
  */
#define a2_power_register_req_proc_cb(client_id, cb, user_data)  \
  a2_poweri_register_req_proc_cb(client_id, cb, user_data)

/*!
  This Macro is exposed to other modules. This is used to support the 
  legacy code of other modules. Once other modules implement DSDA chanegs,
  this macro can take an extra argument as sub_id.
  */
#define a2_power_deregister_req_proc_cb(client_id)  \
  a2_poweri_deregister_req_proc_cb(client_id)


/*!
   @brief
   enum for a2 power request - shut down, wake up or apps smsm
*/
typedef enum
{
  A2_POWER_REQ_SHUT_DOWN            = 0,
  A2_POWER_REQ_WAKE_UP              = 1,
  A2_POWER_REQ_SMSM_ACK             = 2,
  A2_POWER_REQ_SMSM_NAK             = 3,
  A2_POWER_REQ_VOLTE_SLEEP          = 4, /*vote for modem freeze*/
  A2_POWER_REQ_MOUNT                = 5,
  A2_POWER_REQ_UNMOUNT              = 6,
  A2_POWER_REQ_MAX
} a2_power_req_e;

/*!
   @brief
   enum for a2 hw power states
*/
typedef enum
{
  A2_POWER_HW_STATE_OFF                 = 0,
  A2_POWER_HW_STATE_ON                  = 1,
  A2_POWER_HW_STATE_MAX
} a2_power_hw_state_e;

/*!
@brief
A2_Power_process_req() call back function prototype
*/
typedef void (*a2_power_req_proc_cb)
(
  uint32 user_data, /*!< clients can regester a call back with required user 
data to be sent back*/
  a2_power_client_e client_id, /*!< client id corresponding to the 
A2_power_process() call back*/
  a2_power_req_e curr_req, /*!< The power request that is being processed. */
  a2_timetick_t time_stamp, /*!< Time at which the call back is called*/
  a2_power_hw_state_e hw_state /*!< hw_state of a2 */
);

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  a2_power_init

==============================================================================*/
/*!
    @brief
    initializes the a2 power module
    
    @return
    none
*/
/*============================================================================*/
extern void a2_power_init
(
  void
);
/* a2_power_init() */

/*==============================================================================

  FUNCTION:  a2_power_deinit

==============================================================================*/
/*!
  @brief
  De-initializes the power module.

*/
/*============================================================================*/
extern void a2_power_deinit
(
  void
);
/* a2_power_deinit() */

/*==============================================================================

  FUNCTION:  a2_power_check_pending_smsm_req

==============================================================================*/
/*!
    @brief
    Function is called at a2 boot up to check for any outstanding smsm requests
 
    @detail
    Apps boots up before modem. Its possible apps may have voted for a2 wake
    up before a2 could register for the smsm callbacks. This function makes
    sure we dont miss such requests from Apps

    @return
    none
*/
/*============================================================================*/
extern void a2_power_check_pending_smsm_req
(
  void
);
/* a2_power_check_pending_smsm_req() */

/*==============================================================================

  FUNCTION:  a2_power_request

==============================================================================*/
/*!
    @brief
    API for a2 power clients to request for a2 services
 
    @detail
    function will send A2_POWER_REQ message to DL PER task.
 
    @return
    none
*/
/*============================================================================*/
extern void a2_power_request
(
  a2_power_client_e client, /*!< client making the request */
  a2_power_req_e    req     /*!< type of request */
);
/* a2_power_request() */

/*==============================================================================

  FUNCTION:  a2_power_process_req

==============================================================================*/
/*!
    @brief
    function processes the A2_POWER_REQ
 
    @detail
    this function runs under DL PER task context

    @return
    none
*/
/*============================================================================*/
extern void a2_power_process_req
(
  a2_power_client_e client, /*!< client making the request */
  a2_power_req_e    req     /*!< type of request */
);

/*==============================================================================

  FUNCTION:  a2_power_get_hw_state

==============================================================================*/
/*!
    @brief
    returns the power status of a2 hw

    @return
    a2_power_hw_state_e - a2 hw power state
*/
/*============================================================================*/
extern a2_power_hw_state_e a2_power_get_hw_state
(
  void
);
/* a2_power_get_hw_state() */

/*==============================================================================

  FUNCTION:  a2_power_get_hw_state_per_client

==============================================================================*/
/*!
    @brief
    returns the power status of a2 hw per client

    @return
    a2_power_hw_state_e - a2 hw power state
*/
/*============================================================================*/
a2_power_hw_state_e a2_power_get_hw_state_per_client
(
  a2_power_client_e client /*!< client */
);
/* a2_power_get_hw_state_per_client() */

/*==============================================================================

  FUNCTION:  a2_power_log_new_req_receive_time

==============================================================================*/
/*!
    @brief
    logs the ustmr time for the newly received power request

    @return
    none
*/
/*============================================================================*/
void a2_power_log_new_req_receive_time
(
  void
);
/* a2_power_log_new_req_receive_time() */

/*==============================================================================

  FUNCTION:  a2_power_collapse_disabled

==============================================================================*/
/*!
    @brief
    returns TRUE if A2 PC feature is completely disabled, FALSE otherwise

    @return
    returns TRUE if A2 PC feature is completely disabled, FALSE otherwise
*/
/*============================================================================*/
boolean a2_power_collapse_disabled
(
  void
);
/* a2_power_collapse_disabled() */

/*==============================================================================

  FUNCTION:  a2_power_send_ack_and_pc_notification_to_apps

==============================================================================*/
/*!
  @brief
  Function sends the SMSM ACK for APPS_PWR_CTRL=0 request(if needed) and also 
  sets the MDM_PWR_CTRL bit to 0, thus notifying Apps that A2 will soon be power
  collapsed.
 
  @return
  none

*/
/*============================================================================*/
void a2_power_send_ack_and_pc_notification_to_apps
(
  void
);

/*==============================================================================

  FUNCTION:  a2_power_finish_dl_per_sleep_procedure

==============================================================================*/
/*!
  @brief
  Function does required post processing once the DL PER sleep procedure is
  complete and power collapse A2 HW (if needed)
 
  @return
  none

*/
/*============================================================================*/
void a2_power_finish_dl_per_sleep_procedure
(
  void
);

/*==============================================================================

  FUNCTION:  a2_power_enter_crit_sect

==============================================================================*/
/*!
  @brief
  This function is called to enter the A2 PC critical section

*/
/*============================================================================*/
void a2_power_enter_crit_sect
(
  void
);

/*==============================================================================

  FUNCTION:  a2_power_exit_crit_sect

==============================================================================*/
/*!
  @brief
  This function is called to exit the A2 PC critical section

*/
/*============================================================================*/
void a2_power_exit_crit_sect
(
  void
);


/*==============================================================================

  FUNCTION:  a2_power_config_modem_for_peak_data_rates2

==============================================================================*/
/*!
  @brief
  Function configures the needed npa resources like q6 cpu, buses and memories
  to run for peak data rates
  
  @detail
  This function is expected to be called only in A2 PER loopback mode
  
  @return
  none

*/
/*============================================================================*/

void a2_power_config_modem_for_peak_data_rates2
(
   uint8 enable_peak_q6_frequency
);
/*==============================================================================

  FUNCTION:  a2_power_config_modem_for_peak_offline

==============================================================================*/
/*!
  @brief
  vote for max mss config bus frequency 
  
  @return
  boolean

*/
/*============================================================================*/
void a2_power_config_modem_for_peak_offline
(
 void
);

/*==============================================================================

  FUNCTION:  a2_power_task_sleep

==============================================================================*/
/*!
  @brief
  This function puts the current thread to sleep

*/
/*============================================================================*/
boolean a2_power_task_sleep
(
  void
);

/*==============================================================================

  FUNCTION:  a2_power_reset_sleep_counter

==============================================================================*/
/*!
  @brief
  This function resets the sleep counter to 0

*/
/*============================================================================*/
void a2_power_reset_sleep_counter
(
  void
);

void a2_poweri_register_req_proc_cb
(
  a2_power_client_e      client_id, /*!< client id */
  a2_power_req_proc_cb   cb, /*!< the cb to be regestered */ 
  uint32                 user_data   /*!< user data to be sent to the clients 
                                            when cb is called*/
);

/*==============================================================================
  FUNCTION:  a2_poweri_deregister_req_proc_cb

==============================================================================*/
/*!
  @brief
  This function Deregisters a callback which is registered as part of 
  a2_poweri_register_req_proc_cb() by clients.
  This function is called by individual techs. It is not in a2 task context. 

*/
/*============================================================================*/
void a2_poweri_deregister_req_proc_cb
(
  a2_power_client_e      client_id /*!< client id */
);

/*==============================================================================

  FUNCTION:  a2_power_try_enter_crit_sect

==============================================================================*/
/*!
  @brief
  This function is called to enter the try lock for A2 PC critical section
  @return
  A boolean to represent if the try lock is succeful or not.

*/
/*============================================================================*/
boolean a2_power_try_enter_crit_sect
(
  void
);
#endif /* A2_POWER_H */


#ifndef ECALL_APP_H_
#define ECALL_APP_H_

/**
  @file ecall_app.h
  @brief This header file is the externalized eCall functions header file.
*/

/*===========================================================================

                                E C A L L . H

DESCRIPTION
  Externalized eCall functions header file.

REFERENCES
  3GPP TS 26.267

EXTERNALIZED FUNCTIONS  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  
Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/public/ecall_app.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "cm.h"

#define ECALL_APP_ECALL_MSD_MAX_LEN 140

/** @addtogroup ecall_app_data_types  
@{ */
/** eCall origination type.  */
typedef enum
{
  ECALL_NO_ACTIVE_CALL = -1,   /**< No eCall in progress.                            */
  ECALL_TEST = 0,              /**< Test eCall for test purpose.                     */
  ECALL_EMERGENCY,             /**< Emergency eCall to call emergency center (PSAP). */
  ECALL_RECONFIG               /**< Reconfiguration eCall.                           */
}ecall_type_of_call;

/** eCall Initiation type   */
typedef enum
{
  ECALL_MANUAL_INITIATED = 0, /**< Manual trigger.    */
  ECALL_AUTOMATIC_INITIATED   /**< Automatic trigger. */
}ecall_activation_type;

/** eCall trigger could be either success or failure. */
typedef enum
{
  ECALL_UNKNOWN = -1,   /**< Initialized to unknown. */
  ECALL_PASS,           /**< Successful trigger.      */
  ECALL_FAILURE         /**< Unsuccessful trigger.    */           
} ecall_session_status_type;

/** eCall call origination extension info type */
typedef struct
{
  boolean                 is_as_id_available; /**< Is Modem as_id available */
  sys_modem_as_id_e_type  as_id;              /**< Modem as_id */

  boolean                 is_ecall_msd_available; /**< Is eCall Minimum Set Data (MSD) available */
  uint8                   ecall_msd[ECALL_APP_ECALL_MSD_MAX_LEN]; /**< eCall Minimum Set Data (MSD) */
} ecall_call_orig_exten_info_s_type;

typedef struct
{
  boolean is_external_msd; /**< Is the updated setting with external MSD */
  uint8   ecall_msd[ECALL_APP_ECALL_MSD_MAX_LEN]; /**< Updated setting with external MSD */
} ecall_session_update_msd_setting_s_type;

/** @ingroup func_start_session_cb_type  
  This callback function can be passed in to get an indication
  of session status (pass, failure). The caller of ecall_session_start() 
  can pass in NULL so that no session status notifications are received. 
      
   @datatypes
   #ecall_session_status_type

   @param[in] session_status Identifies the session status. 

   @return
   None.

   @dependencies
   None.

*/
  
typedef void (*ecall_start_session_cb_type) (
  ecall_session_status_type  session_status 
);


/** @ingroup func_session_call_info_cb_type 
  This callback function is used to register for current call info 
  such as call type and activation type.
      
  @datatypes
  #ecall_type_of_call
  #ecall_activation_type
 
  @param[in] type_of_call   Identifies the type of call; values are: 
                            - 1 -- No eCall in progress.       
                              0 -- Test eCall
                              1 -- Emergency eCall
                              2 -- Reconfiguration eCall
  @param[in] activation_type   Specifies the activation type; values are:
                              0 -- MANUAL_INITIATED
                              1 -- AUTOMATIC_INITIATED

   @return
   None.

   @dependencies
   None.

*/
     
typedef void (*ecall_session_call_info_cb_type) (
  ecall_type_of_call     type_of_call,    
  ecall_activation_type  activation_type 
);

/** @ingroup func_call_orig_info_cb_f_type 
  This callback function is used to get eCall call origination 
  info: data_block_ptr, session_status, call_id and client_id. 
   
  @datatypes 
  #ecall_session_status_type 
  #cm_call_id_type 
  #cm_client_id_type
   
  @param[in] data_block_ptr  Pointer to client's cb data block 
  @param[in] session_status  eCall session status 
  @param[in] call_id         Call Manager call_id
  @param[in] client_id       Call Manager client_id

  @return 
  None.

  @dependencies 
  None.

*/

typedef void (*ecall_call_orig_info_cb_f_type) (
  void                       *data_block_ptr,
  ecall_session_status_type  session_status,
  cm_call_id_type            call_id,
  cm_client_id_type          client_id
);

/** @ingroup func_call_orig_exten_info_cb_f_type 
  This callback function is used to get eCall call origination 
  extension info: data_block_ptr, session_status, call_id,
  client_id and exten_info
   
  @datatypes 
  #ecall_session_status_type 
  #cm_call_id_type 
  #cm_client_id_type 
  #ecall_call_orig_exten_info_s_type 
   
  @param[in] data_block_ptr  Pointer to client's cb data block 
  @param[in] session_status  eCall session status 
  @param[in] call_id         Call Manager call_id
  @param[in] client_id       Call Manager client_id 
  @param[in] exten_info      eCall call origination exten_info 

  @return 
  None.

  @dependencies 
  None.

*/
typedef void (*ecall_call_orig_exten_info_cb_f_type) (
  void                              *data_block_ptr,
  ecall_session_status_type          session_status,
  cm_call_id_type                    call_id,
  cm_client_id_type                  client_id,
  ecall_call_orig_exten_info_s_type  exten_info
);

/** @} */ /* end_addtogroup ecall_app_data_types */

/*===========================================================================

                  F U N C T I O N   D E C L A R A T I O N S

===========================================================================*/

/*==========================================================================
  FUNCTION ECALL_SESSION_START
===========================================================================*/

/** @ingroup func_session_start
  This function starts an eCall session according to the input parameters 
  (manual/auto-initiated, voice/emergency call). It collects the GPS fix, 
  constructs the MSD, originates the call, and sends the MSD to the modem.

  @datatypes
  #ecall_type_of_call
  #ecall_activation_type
 
  @param[in] type_of_call   Identifies the type of call; values are:
                            - 0 -- Test eCall
                            - 1 -- Emergency eCall
                            - 2 -- Reconfiguration eCall
  @param[in] activation_type   Specifies the activation type; values are:
                            - 0 -- MANUAL_INITIATED
                            - 1 -- AUTOMATIC_INITIATED
  @param[in] ecall_start_session_cb_func  Callback to get the status of the 
                            eCall session (PASS or FAIL); need to register 
                            this callback to get the status of the eCall 
                            session; if the status is not required, NULL
                            can be passed.

  @return 
   Boolean:  If 1, eCall session is started; if 0, eCall session could not be 
   started.

  @dependencies
  None.
  
*/

boolean ecall_session_start
(
  ecall_type_of_call type_of_call,
  ecall_activation_type activation_type,
  ecall_start_session_cb_type  ecall_start_session_cb_func
);

/*==========================================================================
  FUNCTION ECALL_SESSION_QMI_START
===========================================================================*/

/** @ingroup func_session_qmi_start
  This function starts an eCall session thru QMI_VOICE according to the input
  parameters (manual/auto-initiated, voice/emergency call). It collects the GPS
  fix, constructs the MSD, originates the call, and sends the MSD to the modem.

  @datatypes
  #ecall_type_of_call
  #ecall_activation_type
  #ecall_call_orig_info_cb_f_type
 
  @param[in] type_of_call    Identifies the type of call; values are:
                             - 0 -- Test eCall
                             - 1 -- Emergency eCall
                             - 2 -- Reconfiguration eCall
  @param[in] activation_type Specifies the activation type; values are:
                             - 0 -- MANUAL_INITIATED
                             - 1 -- AUTOMATIC_INITIATED
  @param[in] call_orig_info_cb_func   Callback to get the eCall session status
                             (PASS or FAIL), data block pointer, call orig
                             info call_id and client_id; need to register
                             this callback to get the call orig info; if the
                             info is not required, NULL can be passed.
  @param[in] data_block_ptr  Pointer to the client callback data block
  @param[in] exten_info      eCall call origination extened information
  

  @return 
   Boolean:  If 1, eCall session is started; if 0, eCall session could not be 
   started.

  @dependencies
  None.
  
*/

boolean ecall_session_qmi_start
(
  ecall_type_of_call                 type_of_call,
  ecall_activation_type              activation_type,
  ecall_call_orig_info_cb_f_type     call_orig_info_cb_func,
  void                              *data_block_ptr,
  ecall_call_orig_exten_info_s_type  exten_info
);

/*==========================================================================
  FUNCTION ECALL_SESSION_QMI_START_PER_SUBS
===========================================================================*/

/** @ingroup func_session_qmi_start_per_subs
  This function starts an eCall session thru QMI_VOICE according to the input
  parameters (manual/auto-initiated, voice/emergency call, as_id).
  It collects the GPS fix, constructs the MSD, originates the call, and sends
  the MSD to the modem.

  @datatypes
  #ecall_type_of_call
  #ecall_activation_type
  #ecall_call_orig_info_cb_f_type
  #sys_modem_as_id_e_type
 
  @param[in] type_of_call    Identifies the type of call; values are:
                             - 0 -- Test eCall
                             - 1 -- Emergency eCall
                             - 2 -- Reconfiguration eCall
  @param[in] activation_type Specifies the activation type; values are:
                             - 0 -- MANUAL_INITIATED
                             - 1 -- AUTOMATIC_INITIATED
  @param[in] call_orig_info_cb_func   Callback to get the eCall session status
                             (PASS or FAIL), data block pointer, call orig
                             info call_id and client_id; need to register
                             this callback to get the call orig info; if the
                             info is not required, NULL can be passed.
  @param[in] data_block_ptr  Pointer to the client callback data block
  @param[in] exten_info      eCall call origination extened information
  

  @return 
   Boolean:  If 1, eCall session is started; if 0, eCall session could not be 
   started.

  @dependencies
  None.
  
*/

boolean ecall_session_qmi_start_per_subs
(
  ecall_type_of_call                    type_of_call,
  ecall_activation_type                 activation_type,
  ecall_call_orig_exten_info_cb_f_type  call_orig_info_exten_cb_func,
  void                                 *data_block_ptr,
  ecall_call_orig_exten_info_s_type     exten_info 
);

/*==========================================================================
FUNCTION ECALL_SESSION_STOP
===========================================================================*/
/** @ingroup func_session_stop
  This function stops the ecall module, and resets the eCall state machine.

  @return
  Boolean: If 1, the eCall session is stopped; if 0, the eCall session could 
  not be stopped.

  @dependencies
  None.

*/

boolean ecall_session_stop
(
   void
);


/*==========================================================================
  FUNCTION ECALL_SESSION_GET_CALL_INFO
===========================================================================*/

/** @ingroup func_session_get_call_info
  Allow client to register a callback to get the current eCall info such as 
  call type and activation type.
 
 
  @param[in] call_info_cb  Callback to be invoked by eCall app to pass the 
                           current call info such as call type and activation
                           type to caller; cannot be NULL.

  @return 
   Boolean:  If 1, callback registration is successful; if 0, registration is failed. 

  @dependencies
  None.

*/ 
boolean ecall_session_get_call_info
(
  ecall_session_call_info_cb_type call_info_cb
);

/*==========================================================================
FUNCTION ECALL_SESSION_UPDATE_MSD_SETTING
===========================================================================*/
/** @ingroup func_session_update_msd_setting
  This function updates the eCall Minimum Set Data (MSD) setting, including
  eCall using external MSD or not.

  @return
  Boolean: If 1, the eCall MSD setting is successfully updated; if 0, eCall
  failed to update the MSD setting.

  @dependencies
  None.

*/

boolean ecall_session_update_msd_setting
(
  ecall_session_update_msd_setting_s_type  msd_setting
);


#endif /* ECALL_APP_H_ */

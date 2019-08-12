#ifndef DSUCSDAPPIF_APIS_H
#define DSUCSDAPPIF_APIS_H

/** 
  @file dsucsdappif_apis.h
  @brief
  This header file contains remoted interfaces that are required by UMTS
  circuit-switched data applications.  
*/
/* NOTE: For the output PDF generated using Doxygen and Latex, all file and 
         group descriptions are maintained in the Modem_Data_mainpage file. 
		 To change any of the the file/group text for the PDF, edit the 
		 Modem_Data_mainpage file, or contact Tech Pubs.

         The above description for this file is part of the 
		 "dsucsdappif_apis" group description in the Modem_Data_mainpage 
		 file. 
*/

/*=============================================================================

             U M T S   C I R C U I T - S W I T C H E D   D A T A

           R E M O T E D   A P P L I C A T I O N   I N T E R F A C E

                          H E A D E R   F I L E

  
  Copyright (c) 2003 - 2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/


/*=============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/dsucsdappif_apis.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/27/10   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
07/12/10   km      Added Doxygen tags
02/08/10   SA      Added RPC comments for ds_ucsd_api_get_calltype. 
02/20/08   SA      Deleted WM UCSD API's for CMI de-featurization.
10/03/08   SA      Added back WM UCSD API's for backward compatibility.
09/01/08   SA      Added modifications for CMI defeaturization.
08/08/08   SA      Added Support for Modem restart modification.
01/07/08   SA      Support for automatic client deregistration.
06/27/07   AR      Added unified API prototypes.
12/14/06   AR      Add deregister interface for WindowsMobile client. 
11/09/06   AR      Export API for Windows Mobile RIL access. 

===============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*==========================================================================

                      INCLUDE FILES FOR MODULE

==========================================================================*/

#include "dsucsdappif.h"

/** @addtogroup dsucsdappif_apis
@{ */

/* FUNCTION DS_UCSD_API_REGISTER_CLIENT */
/**
  Registers the application client with the UCSD
  subtask. This function should be called by the application task at 
  initialization.
    
  @param call_types [in]    Bitmap of the UCSD call types for which the client 
                            subscribes for call control notification 
                            events. Currently, fax and voice calls are not 
                            supported.
  @param call_event_cb [in] Client callback function to receive call control
                            notification events.
  @param user_info_ptr [in] Client context value, which is provided
                            with call control notification events.
  @return
  The caller is returned a client ID, which must be checked to determine
  the status of registration. A valid client ID is returned on successful 
  registration; DSUCSD_INVALID_CLIENT_ID on registration failure.
    
  @dependencies 
  UCSD client support must have been previously initialized.
*/
extern ds_ucsd_client_id_type ds_ucsd_api_register_client
(
  uint8   call_types,                             
  ds_ucsd_call_event_cb_type call_event_cb,       
  void  *user_info_ptr                            
);
/*~ FUNCTION  ds_ucsd_api_register_client 
    RELEASE_FUNC ds_ucsd_api_deregister_client (_RESULT_) 
    ONERROR return DSUCSD_INVALID_CLIENT_ID */
/*~ PARAM IN user_info_ptr POINTER */


/* FUNCTION DS_UCSD_API_DEREGISTER_CLIENT */
/**
  Deregisters the application client from the UCSD
  subtask. This function should be called by the application task at shutdown.
 
  @param client_id [in] Client ID returned on API registration.

  @return
  TRUE -- Operation is successful. \n
  FALSE -- Operation is not successful.
  
  @dependencies
  ds_ucsd_api_register_client() must have already been called.
*/ 
extern boolean ds_ucsd_api_deregister_client
(
  ds_ucsd_client_id_type client_id                  
);
/*~ FUNCTION ds_ucsd_api_deregister_client 
    ONERROR return FALSE */


/* FUNCTION DS_UCSD_API_ORIGINATE_CALL */
/**
  Originates a CS data call. This function sends the 
  DS_UCSD_APP_ORIG_CALL_CMD command to the UCSD subtask. The function 
  should be called by a CS data application. 

  @param client_id [in]  Client ID returned on API registration.
  @param call_mode [in]  Specify single or dual mode call. Applicable only
                         for builds supporting multimedia fallback 
						 (Service Change and UDI/RDI Fallback (SCUDIF)).
  @param speed [in]      Fixed network data rate, per +CBST AT command.
  @param name [in]       Synchronous or asynchronous name flag per +CBST 
                         AT command.
  @param connection_element [in] Connection element per +CBST AT command.
  @param waiur [in]              Wanted air interface rate per +CHSN AT 
                                 command.
  @param dial_string [in]        NULL-terminated dial string without dialing 
                                 modifiers.
  @param dial_string_len [in]    Length of dial string.
  @param modifiers [in]          Dialing modifiers.
  @param uus_info [in]           User-to-user signaling for videotelephony call
                                 two-stage dialing.

  @return 
  A CS data call instance ID is returned on success; DSUCSD_INVALID_INST_ID
  on failure.
 
  @dependencies
  ds_ucsd_api_register_client() must have already been called.

  @sideeffects
  May allocate an entry in the CS data call table.
*/
extern uint8 ds_ucsd_api_originate_call
( 
  ds_ucsd_client_id_type client_id,       
  uint8                call_mode,         
  uint8                speed,            
  uint8                name,             
  uint8                connection_element,
  uint8                waiur,            
  const uint8         *dial_string,       
  uint8                dial_string_len,   
  ds_ucsd_dial_modifier_info_type *modifiers,
  ds_ucsd_uus_info_type *uus_info         
); /* ds_ucsd_api_originate_call() */
/*~ FUNCTION ds_ucsd_api_originate_call 
    ONERROR return DSUCSD_INVALID_INST_ID */
/*~ PARAM IN dial_string STRING */
/*~ PARAM IN modifiers POINTER */
/*~ PARAM IN uus_info POINTER */


/* FUNCTION DS_UCSD_API_CONFIRM_CALL */
/**
  Confirms an incoming CS data call. This function sends the
  DS_UCSD_APP_CONFIRM_CALL_CMD command to the UCSD subtask.  The function
  should be called by a CS data application.

  @param client_id [in] Client ID returned on API registration.
  @param inst_id [in]   Allocated CS data call instance ID.
  @param call_type [in] Specify preferred mode of a dual mode call. 
                        Applicable only for builds supporting 
                        multimedia fallback (SCUDIF).
  @param result_params_ptr [in] Indicates if the setup is rejected and gives 
                                a cause value.

  @return
  TRUE -- Operation is successful. \n
  FALSE -- Operation is not successful.

  @dependencies
  ds_ucsd_api_register_client() must have already been called.

  @sideeffects
  If call_type is changed relative to the setup event, a preferred bearer 
  order will be changed in the network signaling.
*/
extern boolean ds_ucsd_api_confirm_call
( 
  ds_ucsd_client_id_type     client_id,        /* Client identifier        */
  uint8                      inst_id,          /* CSData call instance     */
  ds_ucsd_call_type          call_type,        /* Type of call             */
  ds_ucsd_answer_param_type *result_params_ptr /* Result parameter info    */
); /* ds_ucsd_api_confirm_call() */
/*~ FUNCTION ds_ucsd_api_confirm_call 
    ONERROR return FALSE */
/*~ PARAM IN result_params_ptr POINTER */


/* FUNCTION DS_UCSD_API_ANSWER_CALL */
/** 
  Answers a CS data call. This function sends the
  DS_UCSD_APP_ANSWER_CALL_CMD command to the UCSD subtask. The function 
  should be called by a CS data application.
    
  @param client_id [in]         Client ID returned on API registration.
  @param inst_id [in]           Allocated CS data call instance ID.
  @param answer_params_ptr [in] Indicates if the call answer is
                                rejected and gives a cause value.

  @return
  TRUE -- Operation is successful. \n
  FALSE -- Operation is not successful.
  
  @dependencies
  ds_ucsd_api_register_client() must have already been called.
*/
extern boolean ds_ucsd_api_answer_call
( 
  ds_ucsd_client_id_type     client_id,        
  uint8                      inst_id,         
  ds_ucsd_answer_param_type *answer_params_ptr 
); /* ds_ucsd_api_answer_call() */
/*~ FUNCTION ds_ucsd_api_answer_call 
    ONERROR return FALSE */
/*~ PARAM IN answer_params_ptr POINTER */


/* FUNCTION DS_UCSD_API_END_CALL */
/** 
  Ends a CS data call. This function sends the 
  DS_UCSD_APP_END_CALL_CMD command to the UCSD subtask. 
  The function should be called by a CS data application.

  @param client_id [in]      Client ID returned on API registration.
  @param inst_id [in]        Allocated CS data call instance ID.

  @return
  TRUE -- Operation is successful. \n
  FALSE -- Operation is not successful.

  @dependencies
  ds_ucsd_api_register_client() must have already been called.
*/

extern boolean ds_ucsd_api_end_call
( 
  ds_ucsd_client_id_type     client_id,        
  uint8                      inst_id          
); /* ds_ucsd_api_end_call() */
/*~ FUNCTION ds_ucsd_api_end_call 
    ONERROR return FALSE */


/* FUNCTION DS_UCSD_API_MODIFY_CALL */
/** 
  Modifies a CS data call to a new call type. This function 
  sends the DS_UCSD_APP_MODIFY_CALL_CMD command to the UCSD
  subtask. The function should be called by a CS data application.  
  It is applicable only for builds supporting multimedia fallback (SCUDIF).

  @param client_id [in]      Client ID returned on API registration.
  @param inst_id [in]        Allocated CS data call instance ID.
  @param msg_type [in]       Specifies command or response.
  @param new_call_type [in]  Indicates the new call type for the originating 
                             modify request.
  @param accept [in]         Indicates an accept/reject for the incoming modify 
                             request.
 
  @return
  TRUE -- Operation is successful. \n
  FALSE -- Operation is not successful.
 
  @dependencies
  ds_ucsd_api_register_client() must have already been called.
*/
extern boolean ds_ucsd_api_modify_call
( 
  ds_ucsd_client_id_type     client_id,        
  uint8                      inst_id,         
  ds_ucsd_msg_type           msg_type,        
  ds_ucsd_call_type          new_call_type,    
  boolean                    accept           
); /* ds_ucsd_api_modify_call() */
/*~ FUNCTION ds_ucsd_api_modify_call 
    ONERROR return FALSE */


/* FUNCTION DS_UCSD_API_LOOKUP_CM_CALLID */
/** 
  Performs a lookup in the CS data call table to find the
  Call Manager (CM) call ID that corresponds to the CS data call 
  instance ID.

  @param inst_id [in]  Allocated CS data call instance ID.

  @return 
  A mapped CM call ID is returned on success; CM_CALL_ID_INVALID on
  failure.

  @dependencies
  None.
*/
extern uint8 ds_ucsd_api_lookup_cm_callid
(
  const uint8 inst_id                              
);
/*~ FUNCTION ds_ucsd_api_lookup_cm_callid 
    ONERROR return CM_CALL_ID_INVALID */


/* FUNCTION DS_UCSD_API_SET_RLP */
/** 
  Changes the RLP parameters for
  the modem processor. The changes are system-wide and affect all future
  nontransparent CS data calls.

  @param client_id [in]      Client ID returned on API registration.
  @param rlp_params_ptr [in] Pointer to the RLP and data compression 
                             parameters.

  @return
  TRUE -- Operation is successful. \n
  FALSE -- Operation is not successful.

  @dependencies
  ds_ucsd_api_register_client() must have already been called.
*/
extern boolean ds_ucsd_api_set_rlp
(
  ds_ucsd_client_id_type      client_id,       
  const ds_ucsd_nt_info_type *rlp_params_ptr   
);
/*~ FUNCTION ds_ucsd_api_set_rlp 
    ONERROR return FALSE */
/*~ PARAM IN rlp_params_ptr POINTER */


/* FUNCTION DS_UCSD_API_GET_RLP */
/** 
  Queries the RLP parameters from the modem processor. The parameters 
  apply to nontransparent CS data calls.

  @param client_id [in]       Client ID returned on API registration.
  @param rlp_params_ptr [out] Pointer to the RLP and data compression 
                              parameters.
      
  @return
  TRUE -- Operation is successful. \n
  FALSE -- Operation is not successful.

  @dependencies
  ds_ucsd_api_register_client() must have already been called.
*/
extern boolean ds_ucsd_api_get_rlp
(
  ds_ucsd_client_id_type     client_id,        
  ds_ucsd_rlp_sets_type     *rlp_params_ptr    
);
/*~ FUNCTION ds_ucsd_api_get_rlp 
    ONERROR return FALSE */
/*~ PARAM OUT rlp_params_ptr POINTER */


/* FUNCTION DS_UCSD_API_GET_CALLSTATS */
/**
  Queries the data traffic statistics for the specified
  CS data call. The statistics are tracked only for those calls using the 
  SIO data path. If a call is inactive, the last statistics are returned. 

  @param client_id [in]       Client ID returned on API registration.
  @param inst_id [in]         Allocated CS data call instance ID.
  @param callstats_ptr [out]  Pointer to the call statistics information.
 
  @return
  TRUE -- Operation is successful. \n
  FALSE -- Operation is not successful.

  @dependencies
  ds_ucsd_api_register_client() must have already been called.
*/

extern boolean ds_ucsd_api_get_callstats
(
  ds_ucsd_client_id_type     client_id,       
  uint8                      inst_id,         
  ds_ucsd_call_stats_type   *callstats_ptr     
);
/*~ FUNCTION ds_ucsd_api_get_callstats 
    ONERROR return FALSE */
/*~ PARAM OUT callstats_ptr POINTER */


/* FUNCTION DS_UCSD_API_GET_CALLTYPE */
/**
  Queries the UCSD stack call type for the specified 
  CM call ID. If the call instance mapped to the call ID is 
  present, a UCSD call type is returned. If a call instance cannot be 
  found, the call type will be DS_UCSD_UNKNOWN_CALL, and the return  
  value will be FALSE. 

  @param cm_call_id [in]  CM call ID.
  @param call_type [out]  UCSD call type.
 
  @return
  TRUE -- Operation is successful. \n
  FALSE -- Operation is not successful.

  @dependencies
  None.
*/ 
extern boolean ds_ucsd_api_get_calltype 
( 
  const uint8     cm_call_id,      
  uint8*          call_type       
);
/*~ FUNCTION ds_ucsd_api_get_calltype 
    ONERROR return FALSE */
/*~ PARAM OUT call_type POINTER */

#ifdef __cplusplus
}

/** @} */ /* end_addtogroup_dsucsdappif_apis */

#endif /* __cplusplus */

#endif /* DSUCSDAPPIF_H */

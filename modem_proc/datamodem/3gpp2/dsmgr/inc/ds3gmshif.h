#ifndef DS3GMSIF_H
#define DS3GMSIF_H
/*===========================================================================

               3 G   D A T A   S E R V I C E S   M A N A G E R

                 M O D E - S P E C I F I C   S U B - T A S K  
       
                   I N T E R F A C E   F U N C T I O N S

                            H E A D E R    F I L E

DESCRIPTION
  This file contains functions for interfacing to mode-specific sub-tasks.
  This header file is intended to be used internal to the 3G Dsmgr module only.


  Copyright (c) 2002 - 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds3gmshif.h_v   1.7   28 Feb 2003 18:55:28   rchar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/inc/ds3gmshif.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/02/12   msh     Coan: Feature cleanup
02/04/11   mg      Process pwroff/lpm mode notification cmd
06/28/10   ss      Fixed compiler warnings.
05/17/10   hs      Added APN disable and PDN inactivity feature support.
02/15/10   sa      Addded ds3g_msh_is_atcop_call().
09/22/09   vrk	   Merged LTE changes.
03/09/09   sa      AU level CMI modifications.
02/14/09   ls      Added ds3g_get_cm_call_type()
10/18/07   vrk     Added DS3G_MSH_GO_DORMANT() for UE initiated dormancy
06/22/06   ar      Add ds3gi_msh_init() to setup call op critical section.
09/02/05   rc      Added support for Mobile Initiated QOS Modification.
06/30/05   ar      Added supprot for asynchronous call setup confirmation.
07/25/03   ak      Added function to change mode of a call.
02/26/03   rc      Added function ds3g_msh_ps_data_available_ind() to inform
                   CM that new data is available on the uplink.
12/10/02   tmr     Added ds3g_msh_is_call_connected() function
10/13/02   ak      Added function to set pkt state.  only for single-call
                   systems/JCDMA
10/10/02   tmr     Removed cmd_ptr parameter from ds3g_msh_complete_ll_connect()
10/03/02   rc      Changed prototype for ds3g_msh_call_rab_reestab_req().
08/19/02   ak      Updated for multiple calls.
04/19/02   vsk     Added PDP context preservation support
04/17/02   rc      Added prototype for ds3gi_process_initiate_call_cmd().
02/26/02   tmr     Initial revision

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "dstask_v.h"
#include "sys.h"
#include "cm.h"

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3GI_MSH_INIT

DESCRIPTION   This function initializes the Data Services Mode Specific
              Handler interface.  It initializes the critical section
              used for call operations.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_msh_init( void );

/*===========================================================================

FUNCTION DS3G_MSH_IS_CALL_CONNECTED

DESCRIPTION
  This function returns whether or not the call identified by the input 
  call ID is in the connected state or not.


DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the call_id is valid and the call is in the connected state,
  FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean  
ds3g_msh_is_call_connected
(
  cm_call_id_type call_id
);

/*===========================================================================

FUNCTION DS3G_MSH_COMPLETE_LL_CONNECT

DESCRIPTION
  This function processes the DS_COMPLETE_LL_CONNECT_CMD from lower
  layers when a connection has been delayed.  This is currently used by
  the GCSD protocol stack to support the +DR command which reports what
  type of compression is used in an NT data call. Compression is 
  negotiated at the RLP layer, and the +DR data must be reported by ATCOP prior
  to the CONNECT message, therefore the CONNECT is delayed while the RLP
  negotiation occurs.

  This function invokes the mode-specific complete_ll_connect_handler, and changes
  the serial mode to Rawdata or Packet, if directed to do so by the mode-
  specific handler. Note that the serial mode may have been changed earlier,
  either before the origination or when the call was answered.

DEPENDENCIES
  The DS_COMPLETE_LL_CONNECT_CMD command is valid only in the 
  DS3G_WAIT_FOR_LL_CONNECT_STATE call states, and is ignored in all other
  call states.

RETURN VALUE
  DS3G_SUCCESS if okay, else DS3G_FAILURE.

SIDE EFFECTS
  Updates the call_state. May also update the serial_state, and change the
  serial mode to Rawdata or Packet.

===========================================================================*/

boolean  ds3g_msh_complete_ll_connect
(
  cm_call_id_type         call_id,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================

FUNCTION DS3G_MSH_COMPLETE_LL_DISCONNECT

DESCRIPTION
  This function processes the DS_COMPLETE_LL_DISCONNECT_CMD from lower
  layers when a disconnect has been delayed.  This is currently used by
  the GCSD protocol stack for FAX calls so that any queued data can be 
  sent prior to a disconnect occuring.  This is necessary for FAX protocols

  This function invokes the mode-specific complete_ll_disconnect_handler, reports
  the disconnect to CM and outputs the AT response

DEPENDENCIES
  The DS_COMPLETE_LL_DISCONNECT_CMD command is valid only in the
  DS3G_WAIT_FOR_LL_DISCONNECT_STATE call state, and is ignored in all other
  call states.

RETURN VALUE
  DS3G_SUCCESS if okay, else DS3G_FAILURE.

SIDE EFFECTS
  Updates the call_state.

===========================================================================*/

boolean  ds3g_msh_complete_ll_disconnect
(
  cm_call_id_type          call_id,
  sys_modem_as_id_e_type   subs_id
);

/*===========================================================================

FUNCTION DS3G_MSH_CALL_RAB_REESTAB_REQ

DESCRIPTION
  This function processes the DS_CALL_RAB_REESTAB_REQ received from 
  Mode SPecific handler.

  A request is sent to CM to attempt  RAB re-establishment

DEPENDENCIES
  The DS_CALL_RAB_REESTAB_REQ is valid only in the 
  DS3G_CALL_ACTIVE_CALL_STATE call state, and is ignored in all other
  call states. Its is also ignored if the call id is not recognized.

RETURN VALUE
  DS3G_SUCCESS if okay, else DS3G_FAILURE.

SIDE EFFECTS
  None

===========================================================================*/
boolean  ds3g_msh_call_rab_reestab_req
(
  cm_call_id_type       call_id,
  cm_rab_reestab_params_s_type  *rab_params
);

/*===========================================================================

FUNCTION DS3G_MSH_INITIATE_CALL

DESCRIPTION
  This function originates a data call of the specified call type. It gets a
  call id from Call Manager, calls the mode-specific originate_call_handler,
  changes the serial mode (if requested to do so by the mode-specific
  handler), and then sends a command to Call Manager to originate a call.

  This function should be invoked by ATCoP when the dial command "ATD" (or
  equivalent) is received.

DEPENDENCIES
  The call will not be originated if any of these conditions is true:
  (a) The call state is not IDLE
  (b) No mode-specific handler table is registered for the current mode and
      call type
  (c) No originate_call_handler is registered for the current mode and call
      type (since this handler must specify the orig_params to be sent to
      Call Mgr)
  (d) A valid call id could not be obtained from Call Manager (this could
      occur if the phone is in use, offline etc.)

RETURN VALUE  TRUE - call was originated with CM.  
              FALSE - call could not be originated.

SIDE EFFECTS  Updates 3G Dsmgr state variables and may also change the
              serial mode, if the origination is successful.
===========================================================================*/
extern boolean ds3g_msh_initiate_call
(
  ds_call_type_e_type  call_type,     /* Call type - Circuit or Packet     */
  sys_sys_mode_e_type  sys_mode,      /* WCDMA, GSM, CDMA, or other?       */
  const byte          *dial_string,   /* Null-terminated dial string       */
  boolean              digit_mode,    /* 1 = dial str has non-digit chars  */
                                      /* 0 = dial str has only digits      */
  void                *user_info_ptr  /* points to user supplied info      */
);

/*===========================================================================
FUNCTION      DS3G_MSH_INITIATE_CALL_EX

DESCRIPTION   This function originates a data call of the specified call 
              type. It gets a call id from Call Manager, calls the mode-
              specific originate_call_handler, changes the serial mode (if
              requested to do so by the mode-specific handler), and then
              sends a command to Call Manager to originate a call.

              This function should be invoked by ATCoP when the dial command
              "ATD" (or equivalent) is received.

DEPENDENCIES  The call will not be originated if any of these conditions is
              true:
                (a) The call state is not IDLE
                (b) No mode-specific handler table is registered for the
                    current mode and call type
                (c) No originate_call_handler is registered for the current
                    mode and call type (since this handler must specify the
                    orig_params to be sent to Call Mgr)
                (d) A valid call id could not be obtained from Call Manager
                    (this could occur if the phone is in use, offline etc.)

RETURN VALUE  TRUE - call was originated.  FALSE - call was not originated.

SIDE EFFECTS  Updates 3G Dsmgr state variables and may also change the serial
              mode, if the origination is successful.
===========================================================================*/
boolean ds3g_msh_initiate_call_ex
(
  ds_call_type_e_type     call_type,     /* Call type - Circuit or Packet     */
  sys_sys_mode_e_type     sys_mode,      /* WCDMA, GSM, CDMA                  */
  const byte             *dial_string,   /* Null-terminated dial string       */
  boolean                 digit_mode,    /* 1 = dial str has non-digit chars  */
                                         /* 0 = dial str has only digits      */
  void                   *user_info_ptr, /* points to user supplied info      */
  sys_modem_as_id_e_type  subs_id        /* subs id */
);

#ifdef FEATURE_DATA_LTE

/*===========================================================================
 
FUNCTION DS3G_MSH_DISABLE_LTE_SELECTION_EX

DESCRIPTION
  This function disables the selection of LTE network. This function in turn
  invokes the CM API that carries out the desired function

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_msh_disable_lte_selection_ex
(
   sys_modem_as_id_e_type     subs_id
);
/*FUNCTION DS3G_MSH_ENABLE_LTE_SELECTION_EX

DESCRIPTION
  This function enables the selection of LTE network. This function in turn
  invokes the CM API that carries out the desired function

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_msh_enable_lte_selection_ex
(
  sys_modem_as_id_e_type     subs_id
);

/*===========================================================================
 
FUNCTION DS3G_MSH_ENABLE_LTE_SELECTION

DESCRIPTION
  This function enables the selection of LTE network. This function in turn
  invokes the CM API that carries out the desired function

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_msh_enable_lte_selection(void);

/*===========================================================================
FUNCTION DS3G_MSH_DISABLE_LTE_PER_PLMN_EX

DESCRIPTION
  This function disables the lte on the plmn id passed for timer value .
  This function in turn invokes the CM API that carries out the desired function

PARAMETERS 
 cm_plmn_blocking_info_s_type       plmn_blocking_info; 
  

DEPENDENCIES
  None.

RETURN VALUE  
  boolean - TRUE (blocking succesful) FALSE (blocking unsuccesful)

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_msh_disable_lte_per_plmn_ex
(
   cm_plmn_blocking_info_s_type       plmn_blocking_info
);

/*===========================================================================
FUNCTION DS3G_MSH_ENABLE_LTE_PLMN_UNBLOCKING

DESCRIPTION
  This function enable the lte plmn blocking on the current plmn id.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_msh_enable_lte_plmn_unblocking(void);

/*===========================================================================
FUNCTION DS3G_MSH_ENABLE_LTE_PLMN_BLOCKING_ON_PLMN_ID

DESCRIPTION
  This function resets the lte plmn blocking list on the plmn id passed. This function in turn
  invokes the CM API that carries out the desired function

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_msh_enable_lte_plmn_blocking_on_plmn_id
(
   sys_plmn_id_s_type plmn_id
);

/*===========================================================================
FUNCTION DS3G_MSH_ENABLE_LTE_PLMN_BLOCKING_ON_PLMN_ID_EX

DESCRIPTION
  This function resets the lte plmn blocking list on the plmn id passed. This function in turn
  invokes the CM API that carries out the desired function

PARAMETERS
  1. plmn_id 
  2. request_pref - CM pref type for enabling PLMN
  3. subs_id
DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_msh_enable_lte_plmn_blocking_on_plmn_id_ex
(
   sys_plmn_id_s_type       plmn_id ,
   uint16	            request_pref,
   sys_modem_as_id_e_type   subs_id
);

/*===========================================================================

FUNCTION DS3G_MSH_INITIATE_PDN_CONNECTION

DESCRIPTION
  This function originates a PDN connection. This is applicable only to the
  LTE mode of operation.

PARAMETERS
  user_info_ptr - currently unused. Will be needed if user_end_hdlr is to be
 				  invoked.
 
  seq_num	    - sequence number that needs to be sent out.

DEPENDENCIES
  

RETURN VALUE  
  DS3G_SUCCESS - call was originated with CM.  
  DS3G_FAILURE - call could not be originated.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_msh_initiate_pdn_connection
(
  void                *user_info_ptr,  /* points to user supplied info      */
  uint8                seq_num
);

/*===========================================================================

FUNCTION DS3G_MSH_INITIATE_PDN_CONNECTION_EX

DESCRIPTION
  This function originates a PDN connection. This is applicable only to the
  LTE mode of operation.

PARAMETERS
  user_info_ptr - currently unused. Will be needed if user_end_hdlr is to be
 				  invoked.
 
  seq_num	    - sequence number that needs to be sent out.

DEPENDENCIES
  

RETURN VALUE  
  DS3G_SUCCESS - call was originated with CM.  
  DS3G_FAILURE - call could not be originated.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_msh_initiate_pdn_connection_ex
(
  void                     *user_info_ptr,  /* points to user supplied info */
  uint8 	           seq_num,
  sys_modem_as_id_e_type   subs_id
);
/*===========================================================================

FUNCTION DS3G_MSH_ABORT_PDN_CONNECTION

DESCRIPTION
  This function aborts a PDN connection req. This is applicable only to the
  LTE mode of operation.

PARAMETERS
  user_info_ptr - currently unused. Will be needed if user_end_hdlr is to be
                  invoked
  sdf_id        - SDF Id to be passed to CM in the abort req params

DEPENDENCIES
  

RETURN VALUE  
  DS3G_SUCCESS - call was aborted with CM.  
  DS3G_FAILURE - call could not be aborted.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_msh_abort_pdn_connection
(
  void                *user_info_ptr, /* points to user supplied info      */
  uint16              sdf_id
);

/*===========================================================================

FUNCTION DS3G_MSH_ABORT_PDN_CONNECTION_EX

DESCRIPTION
  This function aborts a PDN connection req. This is applicable only to the
  LTE mode of operation.

PARAMETERS
  user_info_ptr - currently unused. Will be needed if user_end_hdlr is to be
                  invoked
  sdf_id        - SDF Id to be passed to CM in the abort req params

DEPENDENCIES
  

RETURN VALUE  
  DS3G_SUCCESS - call was aborted with CM.  
  DS3G_FAILURE - call could not be aborted.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_msh_abort_pdn_connection_ex
(
  void                *user_info_ptr, /* points to user supplied info      */
  uint16              sdf_id,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================

FUNCTION DS3G_MSH_PDN_DISCONNECT_REQ

DESCRIPTION
  This function initiates a PDN disconnect req. This is applicable only to the
  LTE mode of operation.

PARAMETERS
  user_info_ptr - currently unused. Will be needed if user_end_hdlr is to be
                  invoked
  sdf_id        - The SDF ID used to bring this connection up. This field is
                  of no use now and is passed only because CM interface has
                  a field for this in the disconnect params
  cm_call_id      call ID
  eps_bearer_id - EPS bearer Id to be passed to CM in the disconnect req
                  params
  local_discon  - Boolean to indicate if this is local clean up

DEPENDENCIES
  None

RETURN VALUE  
  DS3G_SUCCESS - call was aborted with CM.  
  DS3G_FAILURE - call could not be aborted.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_msh_pdn_disconnect_req
(
  void                *user_info_ptr, /* points to user supplied info      */
  uint16              sdf_id,
  cm_call_id_type     cm_call_id,
  eps_bearer_id_T     eps_bearer_id,
  boolean             local_discon
);

/*===========================================================================

FUNCTION DS3G_MSH_PDN_DISCONNECT_REQ_EX

DESCRIPTION
  This function initiates a PDN disconnect req. This is applicable only to the
  LTE mode of operation.

PARAMETERS
  user_info_ptr - currently unused. Will be needed if user_end_hdlr is to be
                  invoked
  sdf_id        - The SDF ID used to bring this connection up. This field is
                  of no use now and is passed only because CM interface has
                  a field for this in the disconnect params
  cm_call_id      call ID
  eps_bearer_id - EPS bearer Id to be passed to CM in the disconnect req
                  params
  local_discon  - Boolean to indicate if this is local clean up

DEPENDENCIES
  None

RETURN VALUE  
  DS3G_SUCCESS - call was aborted with CM.  
  DS3G_FAILURE - call could not be aborted.

SIDE EFFECTS
  None
===========================================================================*/

boolean ds3g_msh_pdn_disconnect_req_ex
(
  void                   *user_info_ptr, /* points to user supplied info      */
  uint16                  sdf_id,
  cm_call_id_type         cm_call_id,
  eps_bearer_id_T         eps_bearer_id,
  boolean                 local_discon,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================

FUNCTION DS3G_MSH_ALLOC_BEARER_REQ

DESCRIPTION
  This function requests a bearer allocation req. This is applicable only to the
  LTE mode of operation.

PARAMETERS
  user_info_ptr -  structure needed to fill in the CM request.

DEPENDENCIES
  

RETURN VALUE  
  DS3G_SUCCESS - Bearer resource alloc was requested with CM.  
  DS3G_FAILURE - Bearer resource alloc was not successful.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_msh_alloc_bearer_req
(
  void                *user_info_ptr /* points to user supplied info      */
);

/*===========================================================================

FUNCTION DS3G_MSH_ALLOC_BEARER_REQ_EX

DESCRIPTION
  This function requests a bearer allocation req. This is applicable only to the
  LTE mode of operation.

PARAMETERS
  user_info_ptr -  structure needed to fill in the CM request.

DEPENDENCIES


RETURN VALUE  
  DS3G_SUCCESS - Bearer resource alloc was requested with CM.  
  DS3G_FAILURE - Bearer resource alloc was not successful.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_msh_alloc_bearer_req_ex
(
  void                   *user_info_ptr, /* points to user supplied info */
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================

FUNCTION DS3G_MSH_MODIFY_BEARER_REQ

DESCRIPTION
  This function requests a bearer modification req. This is applicable only to the
  LTE mode of operation.

PARAMETERS
  user_info_ptr -  structure needed to fill in the CM request.

DEPENDENCIES
  

RETURN VALUE  
  DS3G_SUCCESS - Bearer resource alloc was requested with CM.  
  DS3G_FAILURE - Bearer resource alloc was not successful.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_msh_modify_bearer_req
(
  void                *user_info_ptr /* points to user supplied info      */
);

/*===========================================================================

FUNCTION DS3G_MSH_MODIFY_BEARER_REQ_EX

DESCRIPTION
  This function requests a bearer modification req. This is applicable only to the
  LTE mode of operation.

PARAMETERS
  user_info_ptr -  structure needed to fill in the CM request.

DEPENDENCIES


RETURN VALUE  
  DS3G_SUCCESS - Bearer resource alloc was requested with CM.  
  DS3G_FAILURE - Bearer resource alloc was not successful.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_msh_modify_bearer_req_ex
(
  void                   *user_info_ptr, /* points to user supplied info */
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================

FUNCTION DS3G_MSH_ABORT_BEARER_REQ

DESCRIPTION
  This function aborts a bearer allocation request. This is applicable only to the
  LTE mode of operation.

PARAMETERS
  user_info_ptr - currently unused. Will be needed if user_end_hdlr is to be
                  invoked
  sdf_id - EPS bearer Id to be passed to CM in the disconnect req
                  params

DEPENDENCIES
  None

RETURN VALUE  
  DS3G_SUCCESS - call was aborted with CM.  
  DS3G_FAILURE - call could not be aborted.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_msh_abort_bearer_req
(
  void                *user_info_ptr /* points to user supplied info      */
);

/*===========================================================================

FUNCTION DS3G_MSH_ABORT_BEARER_REQ_EX

DESCRIPTION
  This function aborts a bearer allocation request. This is applicable only to the
  LTE mode of operation.

PARAMETERS
  user_info_ptr - currently unused. Will be needed if user_end_hdlr is to be
                  invoked
  sdf_id - EPS bearer Id to be passed to CM in the disconnect req
                  params

DEPENDENCIES
  None

RETURN VALUE  
  DS3G_SUCCESS - call was aborted with CM.  
  DS3G_FAILURE - call could not be aborted.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_msh_abort_bearer_req_ex
(
  void                   *user_info_ptr, /* points to user supplied info */
  sys_modem_as_id_e_type  subs_id
);

#endif /* FEATURE_DATA_LTE */

/*===========================================================================

FUNCTION DS3G_MSH_ANSWER_CALL

DESCRIPTION
  This function answers an incoming data call. It calls the mode-specific
  user_answer_handler and then sends a command to Call Manager to either
  answer the call or signal user busy (for UMTS circuit-switched).
  
  This function should be invoked by ATCoP when the answer command "ATA" is
  received.  It is also invoked from the UMTS circuit-switched API.

DEPENDENCIES
  The call will not be answered if any of these conditions is true:
  (a) The call state is not RINGING
  (b) No mode-specific handler table is registered for the current mode and
      call type
  (c) No user_answer_handler is registered for the current mode and call
      type (since this handler must specify the ans_params to be sent to
      Call Mgr)

RETURN VALUE
  DS3G_SUCCESS if okay, else DS3G_FAILURE.

SIDE EFFECTS
  Updates 3G Dsmgr state variables and may also change the serial mode, if
  the call is answered.

===========================================================================*/
extern boolean ds3g_msh_answer_call
(
  cm_call_id_type      cm_call_id
);

/*===========================================================================

FUNCTION DS3G_MSH_ANSWER_CALL_EX

DESCRIPTION
  This function answers an incoming data call. It calls the mode-specific
  user_answer_handler and then sends a command to Call Manager to either
  answer the call or signal user busy (for UMTS circuit-switched).
  
  This function should be invoked by ATCoP when the answer command "ATA" is
  received.  It is also invoked from the UMTS circuit-switched API.

DEPENDENCIES
  The call will not be answered if any of these conditions is true:
  (a) The call state is not RINGING
  (b) No mode-specific handler table is registered for the current mode and
      call type
  (c) No user_answer_handler is registered for the current mode and call
      type (since this handler must specify the ans_params to be sent to
      Call Mgr)

RETURN VALUE
  DS3G_SUCCESS if okay, else DS3G_FAILURE.

SIDE EFFECTS
  Updates 3G Dsmgr state variables and may also change the serial mode, if
  the call is answered.

===========================================================================*/

extern boolean ds3g_msh_answer_call_ex
(
  cm_call_id_type            cm_call_id,
  sys_modem_as_id_e_type     subs_id
);

/*===========================================================================
FUNCTION      DS3G_MSH_HANGUP_CALL

DESCRIPTION   This function ends the currently active data call (if any).
              It calls the mode-specific user_end_handler, to allow any
              protocols to be shut down if necessary, and then sends a
              command to Call Manager to end the call.

DEPENDENCIES  No action is taken if no call is active i.e. the call state
              is IDLE.

RETURN VALUE  DS3G_SUCCESS if CM is told to end call.  
              DS3G_FAILURE if could not end call.

SIDE EFFECTS  Updates 3G Dsmgr state variables if a call was active.
===========================================================================*/
boolean ds3g_msh_hangup_call
(
  cm_call_id_type      call_id,       /* call id of call to end            */
  void                *user_info_ptr  /* points to user supplied info      */
);

/*===========================================================================
FUNCTION      DS3G_MSH_HANGUP_CALL_EX

DESCRIPTION   This function ends the currently active data call (if any).
              It calls the mode-specific user_end_handler, to allow any
              protocols to be shut down if necessary, and then sends a
              command to Call Manager to end the call.

DEPENDENCIES  No action is taken if no call is active i.e. the call state
              is IDLE.

RETURN VALUE  DS3G_SUCCESS if CM is told to end call.  
              DS3G_FAILURE if could not end call.

SIDE EFFECTS  Updates 3G Dsmgr state variables if a call was active.
===========================================================================*/
boolean ds3g_msh_hangup_call_ex
(
  cm_call_id_type         call_id,       /* call id of call to end            */
  void                   *user_info_ptr,  /* points to user supplied info      */
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3G_MSH_HANGUP_CALL_EX2

DESCRIPTION   This function ends the currently active data call (if any).
              It calls the mode-specific user_end_handler, to allow any
              protocols to be shut down if necessary, and then sends a
              command to Call Manager to end the call.

DEPENDENCIES  No action is taken if no call is active i.e. the call state
              is IDLE.

RETURN VALUE  DS3G_SUCCESS if CM is told to end call.  
              DS3G_FAILURE if could not end call.

SIDE EFFECTS  Updates 3G Dsmgr state variables if a call was active.
===========================================================================*/
boolean ds3g_msh_hangup_call_ex2
(
  cm_call_id_type         call_id,       /* call id of call to end            */
  void                   *user_info_ptr,  /* points to user supplied info      */
  sys_modem_as_id_e_type  subs_id,
  boolean                 local_disconnect /* flag to indicate whether 
                                              PDP should be locally deactivated*/
);

/*===========================================================================
FUNCTION      DS3G_MSH_PS_DATA_AVAILABLE_IND

DESCRIPTION   This function informs Call Manager that new data is available 
              on the uplink. This function is called by the mode specific
              handler when it has new data on the uplink during a 
              inter-RAT change. This indication triggers a Routing Area
              Update during inter-RAT change. 

DEPENDENCIES  No action is taken if no call is active i.e. the call state
              is IDLE.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3g_msh_ps_data_available_ind
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================

FUNCTION DS3G_MSH_PDP_MODIFY_REQ

DESCRIPTION
  This function processes the DS_PDP_MODIFY_REQ received from
  Mode SPecific handler.

  A request is sent to CM to attempt PDP Modification

DEPENDENCIES
  The DS_PDP_MODIFY_REQ is valid only in the
  DS3G_CALL_ACTIVE_CALL_STATE call state, and is ignored in all other
  call states. Its is also ignored if the call id is not recognized.

RETURN VALUE
  DS3G_SUCCESS if okay, else DS3G_FAILURE.

SIDE EFFECTS
  None

===========================================================================*/
boolean  ds3g_msh_pdp_modify_req
(
cm_call_id_type              call_id,
cm_pdp_modify_params_s_type *modify_params
);

/*===========================================================================
FUNCTION      DS3G_MSH_SET_PKT_STATE

DESCRIPTION   This function is appropriate for one-call systems only.  It 
              allows the mode-specific handler to tell CM the state of its
              packet connection.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds3g_msh_set_pkt_state
(
  cm_packet_state_e_type    pkt_state
);

/*===========================================================================
FUNCTION      DS3G_MSH_SET_NEW_CALL_MODE

DESCRIPTION   This function is called by the mode-specific handler when the
              mode (GSM/WCDMA/1x) changes during some part of the call.
              
              This is targeted to help out with inter-RAT handoffs.  By 
              doing this, DS & CM should stay consistent.

DEPENDENCIES  None.

RETURN VALUE  TRUE  = mode change successful
              FALSE = mode change failed

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_msh_set_new_call_mode
(
  cm_call_id_type      call_id,       /* call id of call whose mode changed*/
  sys_sys_mode_e_type  new_mode       /* new call mode                     */
);

/*===========================================================================
FUNCTION      DS3G_MSH_GET_CALL_MODE

DESCRIPTION   Fetch the call mode corresponding to the call id.

DEPENDENCIES  None.

RETURN VALUE  Call mode

SIDE EFFECTS  None
===========================================================================*/
sys_sys_mode_e_type ds3g_msh_get_call_mode
(
  cm_call_id_type      call_id       /* call id of call whose mode is needed*/
);

/*===========================================================================
FUNCTION      DS3G_MSH_CONFIRM_CALL

DESCRIPTION
  This function confirms the setup of an incoming data call.  The
  setup parameters should have been validated by the mode-specific
  handler.
  
  This function should be invoked by the mode-specific handler when
  the call setup respose was delayed.  Function is also called
  internally when response is not delayed.

DEPENDENCIES
  The call setup should have been validated by the mode-specific
  handler.

RETURN VALUE
  DS3G_SUCCESS if okay, else DS3G_FAILURE.

SIDE EFFECTS
  None

===========================================================================*/
boolean ds3g_msh_confirm_call
(
  cm_call_id_type              cm_call_id,       /* call id of call        */
  cm_setup_res_params_s_type * setup_params_ptr  /* setup parameters       */
);


#ifdef FEATURE_MULTIMEDIA_FALLBACK
#error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

#ifdef FEATURE_UE_INITIATED_DORMANCY 
/*===========================================================================
FUNCTION      DS3G_MSH_GO_DORMANT

DESCRIPTION   This function requests Call Manager to go dormant. This 
              function is called by the umts mode specific handler when the 
              application requests UE initiated dormancy. 

DEPENDENCIES  None.

RETURN VALUE  DS3G_SUCCESS if okay, else DS3G_FAILURE.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3g_msh_go_dormant(void);
#endif /* FEATURE_UE_INITIATED_DORMANCY */

/*===========================================================================
FUNCTION      DS3G_GET_CM_CALL_TYPE

DESCRIPTION   This function convert DS_call_type to CM_call_type

DEPENDENCIES  None.

RETURN VALUE  CM_CALL_TYPE

SIDE EFFECTS  None.
===========================================================================*/
cm_call_type_e_type ds3g_get_cm_call_type(ds_call_type_e_type ds_call_type);

/*===========================================================================
FUNCTION      DS3G_MSH_IS_ATCOP_CALL

DESCRIPTION
  This function sends query to Modehandler to check 
  whether the call was originated or answered by ATCoP

DEPENDENCIES
  None

RETURN VALUE
  DS3G_SUCCESS if okay, else DS3G_FAILURE.

SIDE EFFECTS
  None

===========================================================================*/
boolean ds3g_msh_is_atcop_call
(
  cm_call_type_e_type             call_type,         /* Call type          */
  cm_call_id_type                 cm_call_id,        /* call id of call        */
  sys_sys_mode_e_type             sys_mode           /* WCDMA, GSM, CDMA, or other?       */
);

/*===========================================================================
FUNCTION      DS3G_CM_DATA_PACKET_STATE_NOTIFY

DESCRIPTION   When DS receives card REFRESH/PWROFF/LPM mode notification from 
              CM, data session is torn down. This function notifies CM that
              data session is torn down.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_cm_data_packet_state_notify
(
  uint8 subs_id_msk
);

/*===========================================================================
FUNCTION      DS3G_MSH_CLEANUP_NONEPC_CALL

DESCRIPTION   This function requests Call Manager to cleanup the NonEPC PPP 
              session with network (Applicable only for Non-SV devices)

DEPENDENCIES  None.

RETURN VALUE  TRUE - If Cmd is accepted by CM 
              FALSE - Otherwise 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3g_msh_cleanup_nonepc_call(void);

#endif /* DS3GMSIF_H */

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               3 G   D A T A   S E R V I C E S   M A N A G E R

                   M O D E - S P E C I F I C   S U B - T A S K  
       
                   I N T E R F A C E   F U N C T I O N S

GENERAL DESCRIPTION
  This software unit contains functions for interfacing to Mode-Specific sub-tasks.

EXTERNALIZED FUNCTIONS
  ds3g_msh_complete_ll_connect()
    Process command to complete call connections

  ds3g_msh_complete_ll_disconnect()
    Process command to complete call disconnection
    
  ds3g_msh_release_call()
    Processes indications from lower level software to release calls
    
  ds3g_msh_initiate_call()
    Originates a data call

  ds3g_msh_answer_call()
    Answers an incoming data call

  ds3g_msh_hangup_call()
    Ends the currently active data call
    
  ds3g_msh_call_rab_reestab_req()
    Processes request for RAB re-establishment.

  ds3g_msh_confirm_call()
    Confirms incoming call setup.  

  ds3g_msh_pdp_modify_req()
   Processes request for QOS modification.

  ds3g_msh_modify_call()
    Request mobile initiated in-call modification for dual mode call  

  ds3g_msh_modify_call_res()
    Respond to network initiated in-call modification for dual mode call

  ds3g_msh_is_atcop_call()
    Returns whether the data call is initiated or 
    answered by ATCoP.
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  ds3gi_msh_init() must be called before any other 3G Dsmgr functions 
  are invoked.

  Copyright (c) 2002 - 2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds3gmshif.c_v   1.15   28 Feb 2003 18:55:04   rchar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/src/ds3gmshif.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/23/15   sd      Added call_id as a parameter in the new call end command 
                   callback from CM.
05/24/13   sd      TSTS changes for DS3G.
12/21/12   ssb     Supporting Centralized call control Feature
01/02/12   msh     Coan: Feature cleanup
08/09/11   sha     Global variable clean up.
01/17/11   ss      Added support for Iface linger.
01/12/11   ss      Fixed an issue where UMTS call_instance is freed before
                   CM_CALL_END_CMD is received.
11/22/10   ttv     Added support for DSDS.
09/09/10   op      Migrated to MSG 2.0 macros
06/28/10   ss      Fixed compiler warnings.
05/17/10   hs      Added APN disable and PDN inactivity feature support.
02/15/10   sa      Added ds3g_msh_is_atcop_call().
09/22/09   vrk     Merged LTE changes.
03/04/09   sa      AU level CMI modifications.
02/25/09   sn      Fixed off-target lint errors.
02/14/09   ls      Added ds3g_get_cm_call_type() to support IS707B data call
04/02/08   sn      Fixed Klocwork errors.
12/04/07   sn      Passing call instance as data block argument while sending 
                   a setup message response in ds3g_msh_confirm_call().
10/18/07   vrk     Added fn DS3G_MSH_GO_DORMANT for UE initiated dormancy 
03/28/07   ac      Remove the temporary debug message.
02/14/07   vd      Lint error fixes.
12/12/06   rsl     Removed assert from non-critical condition.
08/28/06   vd      Memset of parameters before invoking mode specific handlers.
06/22/06   ar      Add ds3gi_msh_init() to setup call op critical section.
02/16/06   ar      Remove obsolete call instance lookup on call confirmed.
11/21/05   ar      Post call end event on setup response error.
09/02/05   rc      Added support for Mobile Initiated QOS Modification.
08/26/05   ar      Added support for CS Data in-call modification & fallback.
06/30/05   ar      Added supprot for asynchronous call setup confirmation.
12/16/04   rsl     Data code now passes the value CM_DIGIT_MODE_4BIT_DTMF
                   to cm_digit_mode to fix issues with authentication.
11/24/04   mv      Fixed the bug in answer_call to compare call type
                   rather than call state.
11/22/04   mv      Added support for Mobile Terminated PDP.
09/21/04   rc      Featurized call to cm_mm_ss_cmd_ps_data_available() 
09/17/04   rc      Changed call from cm_mm_call_cmd_ps_data_available() to
                   cm_mm_ss_cmd_ps_data_available() in 
                   ds3g_msh_ps_data_available_ind().
08/06/04   ar      Set CM numbering plan on call origination.
02/19/04   vsk     Pass the call instance in cm_mm_call_cmd_orig
10/07/03   vsk     Multiple call support required for M-PDP 
08/28/03    sy     Mainlined the code where CM is always informed about the
                   DS call state(earlier it was under jcdma feature).
07/25/03   ak      Added function to change mode of a call.
04/09/03   rsl     Changes relating to CM ret. val. check.
03/10/03   ak      When orig'ing a call, check CM return value to see if
                   call failed.
02/26/03   rc      Added function ds3g_msh_ps_data_available_ind() to inform
                   CM that new data is available on the uplink.
02/12/03   wx      Accommodate cm_mm_call_cmd_end api change and 
                   cm_end_params_s_type struct change
12/10/02   tmr     Added ds3g_msh_is_call_connected() function
12/04/02   tmr     Changed ds3g_msh_hangup_call() so that repeated attempts
                   to hangup the same call are ignored.
12/02/02   ak      Removed old featurization in rab_reestab() for 1x builds.
11/04/02   tmr     Return FALSE if we already have an active call when
                   ds3g_msh_initiate_call is called.  Also added setting
                   of call_was_connected field in call info structure.
10/22/02   tmr     Removed dsat_send_result_code( DSAT_CONNECT ) from 
                   ds3g_msh_complete_ll_connect()
10/14/02   vsk     Store the system mode & call type  during origination 
                   to be used after the call has ended to get stats
10/13/02   ak      Added function to set pkt state.  only for single-call
                   systems/JCDMA
10/10/02   tmr     Removed cmd_ptr parameter from ds3g_msh_complete_ll_connect()
10/03/02   rc      Changed prototype for ds3g_msh_call_rab_reestab_req() 
08/23/02   atp     Removed dsat_send_result_code() from answer_call().
08/22/02   atp     Moved stop_alerting() from answer_call() to mode-sp hdlr.
08/19/02   ak      Updated for multiple calls.
08/05/02   ak      Moved ds3g_initiate_call(), ds3g_answer_call(), and 
                   ds3g_hangup_call to this file.
08/01/02   rc      Modified ds3gi_process_msh_complete_ll_disconnect_cmd() to 
                   to send the result code only if 
                   stack_type = DS_CALL_STACK_EXTERNAL.
06/18/02   tmr     Modified ds3g_hangup_call() to delay the disconnect if the
                   user_end_handler() indicates so.
06/18/02   tmr     Added ds3gi_process_msh_complete_ll_disconnect_cmd().
04/19/02   vsk     Added PDP context preservation support
04/17/02   rc      Added function ds3gi_process_initiate_call_cmd().
02/26/02   tmr     Initial revision
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include <stringl/stringl.h>

#ifdef FEATURE_DATA_LTE_OFFTARGET_TEST
#include "dsut.h"
#endif /* FEATURE_DATA_LTE_OFFTARGET_TEST */
#include "rex.h"
#include "amssassert.h"
#include "msg.h"
#include "err.h"
#include "cm.h"
#include "dstaski.h"
#include "ds3gmgr.h"
#include "ds3gmgrint.h"
#include "ds3gmmgsdiif.h"
#include "dsat_v.h"
#include "ds3gcmif.h"
#include "ds3gsiolib.h"
#include "ds3gmshif.h"
#include "ds_3gpp_pdn_context.h"
#include "sys.h"
#include "data_err_suppr.h"
#include "dsucsdi.h"
#include "dsumts_rmsm.h"
#include "data_msg.h"
#include "dsutil.h"
#include "ds_3gpp_hdlr.h"
#include "ds_3gpp_pdn_throttle_sm.h"

#ifdef FEATURE_DATA_PKT_CLASSIFIER
#include "ds707_pkt_classifier.h"
#endif/* FEATURE_DATA_PKT_CLASSIFIER */

/*===========================================================================

                      LOCAL DATA DECLARATIONS

===========================================================================*/

/* Declare a task critical section for serialized code execution. */
static rex_crit_sect_type ds3gi_call_op_crit_sect = {{NULL}};


#define DS3G_ENTER_CALL_OP_CS() rex_enter_crit_sect( &ds3gi_call_op_crit_sect );
#define DS3G_EXIT_CALL_OP_CS()  rex_leave_crit_sect( &ds3gi_call_op_crit_sect );



/*===========================================================================

                      INTERNAL FUNCTION DEFINITIONS

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
void ds3gi_msh_init( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------
    Initialize call operation critical section.
  ---------------------------------------------------------------------*/
  rex_init_crit_sect( &ds3gi_call_op_crit_sect );

} /* ds3gi_msh_init() */



/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/


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
boolean ds3g_msh_is_call_connected
(
  cm_call_id_type call_id
)
{
   uint32  call_inst;
   boolean rc = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_inst = ds3gi_find_call_instance(call_id);
  
  if (call_inst >= DS3GI_MAX_NUM_CALLS)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Bad call id for is call active %d", call_id);
  }
  else if (ds3gi_call_info[call_inst].call_state == DS3G_CALL_ACTIVE_CALL_STATE)
  {
    rc = TRUE;
  }

  return rc;
}

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
)
{
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr = NULL;    /* Ptr to mode-sp. hdlr table */
  boolean                   hdlr_result = FALSE;
  /* The following local variables are used to store return values from the
     mode specific handler (passed by reference to the handler function)   */
  cm_end_params_s_type      end_params;
  boolean                   ret_val = DS3G_FAILURE;
  uint32                    call_inst = DS3GI_INVALID_CALL_INSTANCE;
  boolean                   cm_ret_val = FALSE;
  cm_client_id_type         ds3g_cm_client_id;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Rx'ed DS_COMPLETE_LL_CONNECT_CMD");

  call_inst = ds3gi_find_call_instance(call_id);
  if (call_inst >= DS3GI_MAX_NUM_CALLS)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,"Bad call id for ll connect %d", call_id);
    return(ret_val);
  }

  if (ds3gi_call_info[call_inst].call_state == DS3G_WAIT_FOR_LL_CONNECT_STATE)
  {

    /*---------------------------------------------------------------------
      Get a pointer to the appropriate handler function table based
      on the current mode and call type.
    ----------------------------------------------------------------------*/
    hdlr_tbl_ptr = 
        ds3gi_get_hdlr_tbl_ptr_for_call_inst(&(ds3gi_call_info[call_inst]));


    if (hdlr_tbl_ptr == NULL)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,"hdlr_table_ptr is Null");
      return DS3G_FAILURE;
    }

    /*---------------------------------------------------------------------
     Ensure that a handler function table is registered, with a non-null
     completed_call_connected_handler.
    ---------------------------------------------------------------------*/

    if( hdlr_tbl_ptr->complete_ll_connect_handler != NULL )
    {
      /*-----------------------------------------------------------------
        Invoke the mode-specific handler function.
      -----------------------------------------------------------------*/
      /*----------------------------------------------------------------
        Intialize end_params before invoking mode specific handler.
      ---------------------------------------------------------------*/
      memset((void*)&end_params, 0, sizeof(cm_end_params_s_type));

      hdlr_result = hdlr_tbl_ptr->complete_ll_connect_handler(
                        ds3gi_call_info[call_inst].call_id,
                        &end_params, subs_id);

      /*-----------------------------------------------------------------
         Proceed only if the mode-specific handler was successful in
         completing the connection.
      -----------------------------------------------------------------*/
      if( hdlr_result == DS3G_SUCCESS )
      {
        /*---------------------------------------------------------------
        Send the CONNECT result code to the TE and set the call state
        to CALL_ACTIVE.
        ---------------------------------------------------------------*/
        ds3gi_call_info[call_inst].call_state = DS3G_CALL_ACTIVE_CALL_STATE;
        ds3gi_call_info[call_inst].call_was_connected = TRUE;

        DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                        "Delayed connect call now connected");
        ret_val = DS3G_SUCCESS;

      } /* if hdlr_result == DS3G_SUCCESS */
      else
      {
        ds3g_cm_client_id = ds3g_get_cm_client_id();
        /*---------------------------------------------------------------
         The mode-specific handler failed to complete the connection,
         so end the call.
        ---------------------------------------------------------------*/
        DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                        "Handler failed to connect, ending call");

        ds3gi_call_info[call_inst].call_state = DS3G_END_CALL_STATE;

        cm_ret_val = cm_mm_call_cmd_exten_end(ds3gi_cm_call_cmd_end_cb,
                                        (void*)call_inst,
                                        ds3g_cm_client_id,
                                        1,
                                        &end_params 
                                        );
        if (cm_ret_val == FALSE)
        {
          DATA_ERR_FATAL("CM end call failed");
          return ret_val;
        }
      } 
    } /* complete_ll_connect_handler != NULL */
    else
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "Complete call connected handler not registered");
    }
  } /* if call_state == DS3G_WAIT_FOR_LL_CONNECT_STATE */
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Unexpected DS_COMPLETE_LL_CONNECT_CMD rx'ed");
  }

  return(ret_val);

} /* ds3g_msh_complete_ll_connect() */


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
  cm_call_id_type         call_id,
  sys_modem_as_id_e_type  subs_id
)
{
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr = NULL;
  cm_end_params_s_type      end_params;
  uint32                    call_inst = DS3GI_INVALID_CALL_INSTANCE;
  boolean                   ret_val = DS3G_FAILURE;
  boolean                   cm_ret_val = FALSE;
  cm_client_id_type         ds3g_cm_client_id;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Rx'ed DS_COMPLETE_LL_DISCONNECT_CMD");

  call_inst = ds3gi_find_call_instance(call_id);
  if (call_inst >= DS3GI_MAX_NUM_CALLS)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Bad call id for ll disconnect %d", call_id);
    return(ret_val);
  }

  if (ds3gi_call_info[call_inst].call_state == DS3G_WAIT_FOR_LL_DISCONNECT_STATE)
  {
    /*---------------------------------------------------------------------
      Get a pointer to the appropriate handler function table based
      on the current mode and call type.
    ---------------------------------------------------------------------*/
    hdlr_tbl_ptr = 
        ds3gi_get_hdlr_tbl_ptr_for_call_inst(&(ds3gi_call_info[call_inst]));

    if (hdlr_tbl_ptr == NULL)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "hdlr_table_ptr is Null");
      return DS3G_FAILURE;
    }

    /*---------------------------------------------------------------------
     Ensure that a handler function table is registered, with a non-null
     complete_ll_disconnect_handler.
    ---------------------------------------------------------------------*/

    if( hdlr_tbl_ptr->complete_ll_disconnect_handler != NULL )
    {
      /*-----------------------------------------------------------------
        Invoke the mode-specific handler function.
      -----------------------------------------------------------------*/
      /*----------------------------------------------------------------
        Intialize end_params before invoking mode specific handler.
      ---------------------------------------------------------------*/
      memset((void*)&end_params, 
             0, 
             sizeof(cm_end_params_s_type));
      hdlr_tbl_ptr->complete_ll_disconnect_handler(
                        ds3gi_call_info[call_inst].call_id,
                        &end_params, subs_id);

      /*-----------------------------------------------------------------
        Set the call state to indicate that we are waiting for the call to
        be ended.
      -----------------------------------------------------------------*/
      ds3gi_call_info[call_inst].call_state = DS3G_END_CALL_STATE;

      /*------------------------------------------------------------------
        Now send a command to Call Manager to end the call.
      ------------------------------------------------------------------*/
      DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Sending End Call command to CM");

      ds3g_cm_client_id = ds3g_get_cm_client_id();
      cm_ret_val = cm_mm_call_cmd_exten_end( ds3gi_cm_call_cmd_end_cb,
                                       (void*) call_inst,
                                       ds3g_cm_client_id,
                                       1,
                                       &end_params );
      if (cm_ret_val == FALSE)
      {
        DATA_ERR_FATAL("CM end call failed");
        return ret_val;
      }
      else
      {
        ret_val = DS3G_SUCCESS;
      }
    } /* complete_ll_disconnect_handler != NULL */
    else
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "Complete call disc hdlr not registered");
    }
  } /* if call_state == DS3G_WAIT_FOR_LL_DISCONNECT_STATE */
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Unexpected DS_COMPLETE_LL_DISCONNECT_CMD");
  }

  return(ret_val);
} /* ds3g_msh_complete_ll_disconnect() */

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
  cm_call_id_type                call_id,
  cm_rab_reestab_params_s_type  *rab_params
)
{
  uint32                    call_inst;
  boolean                   ret_val = DS3G_FAILURE;
  boolean                   cm_ret_val = FALSE;
  cm_client_id_type         ds3g_cm_client_id;
  void                      *data_block_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - -*/

  call_inst = ds3gi_find_call_instance(call_id);
  if (call_inst >= DS3GI_MAX_NUM_CALLS)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Bad call id for rab reestab %d", call_id);
    return(ret_val);
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "Rx'ed CM_CALL_RAB_REESTAB_REQ_CMD, call_inst: %d", 
                  call_inst);
   
  /*------------------------------------------------------------------------ 
    Populate cb_data for call_cmd_cb 
  ------------------------------------------------------------------------*/
  data_block_ptr = (void *)(call_inst);

  /*-------------------------------------------------------------------------
    Rab RE-establishment is valid only in the Active call state.
  -------------------------------------------------------------------------*/
  if( ds3gi_call_info[call_inst].call_state == DS3G_CALL_ACTIVE_CALL_STATE )
  {
    ds3g_cm_client_id = ds3g_get_cm_client_id();
    cm_ret_val = cm_mm_call_cmd_rab_reestablish(ds3gi_cm_call_cmd_cb,
                                                data_block_ptr,
                                                ds3g_cm_client_id,
                                                ds3gi_call_info[call_inst].call_id,
                                                rab_params 
                                                );
    if (cm_ret_val == FALSE)
    {
      DATA_ERR_FATAL("CM call cmd RAB Re-establish failed");
      return(ret_val);
    }
    else
    {
      ret_val = DS3G_SUCCESS;
    }
  }
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Rx'ed RAB RE-estab req:Call not active");
  }
  return(ret_val);
} /* ds3g_msh_call_rab_reestab_req() */

/*===========================================================================
FUNCTION      DS3G_MSH_CONFIRM_CALL

DESCRIPTION
  This function confirms the setup of an incoming data call.  The
  setup parameters should have been validated by the mode-specific
  handler.  Note call setup events apply only to GSMS/WCDMA mode.
  
  This function should be invoked by the circuit-switched mode-
  specific handler when the call setup respose was delayed.  
  Function is also called internally when response is not delayed.

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
  cm_call_id_type              cm_call_id,
  cm_setup_res_params_s_type * setup_params_ptr
)
{
  boolean                   ret_val = DS3G_FAILURE;
  boolean                   cm_ret_val = FALSE;
  uint32                    call_inst = DS3GI_INVALID_CALL_INSTANCE;
  cm_client_id_type         ds3g_cm_client_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - -*/
  
  if(!(IS_IN_DS_TASK()))
  {
    DATA_ERR_FATAL("Not executing in ds task");
    return FALSE;
  }

  /* Lint error handler */
  DATA_MDM_ARG_NOT_CONST(setup_params_ptr);

  call_inst = ds3gi_find_call_instance(cm_call_id);
  if (call_inst >= DS3GI_MAX_NUM_CALLS)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Can't confirm incoming call setup - Bad call ID");
    return FALSE;
  }

  ds3g_cm_client_id = ds3g_get_cm_client_id();
  /*-------------------------------------------------------------------
    Send a setup response command to Call Manager. The mode-
    specific handler must specify whether the setup was accepted
    or rejected in the setup_params.
  -------------------------------------------------------------------*/
  cm_ret_val = cm_mm_call_cmd_setup_res( ds3gi_cm_call_cmd_cb,
                                         (void *) call_inst,
                                         ds3g_cm_client_id,
                                         cm_call_id,
                                         setup_params_ptr
                                       );
  if (cm_ret_val == FALSE)
  {
    DATA_ERR_FATAL("CM call cmd setup res failed");
    return DS3G_FAILURE;
  }
  else
  {
    ret_val = DS3G_SUCCESS;
  }

  return(ret_val);
} /* ds3g_msh_confirm_call() */

/*===========================================================================
FUNCTION      DS3G_MSH_ANSWER_CALL

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
boolean ds3g_msh_answer_call
(
  cm_call_id_type         cm_call_id
)
{
   sys_modem_as_id_e_type     subs_id = SYS_MODEM_AS_ID_1;
  /*----------------------------------------------------- */
  return ds3g_msh_answer_call_ex(cm_call_id, subs_id );
}

/*===========================================================================
FUNCTION      DS3G_MSH_ANSWER_CALL_EX

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
boolean ds3g_msh_answer_call_ex
(
  cm_call_id_type            cm_call_id,
  sys_modem_as_id_e_type     subs_id
)
{
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr = NULL;
  cm_ans_params_s_type      *ans_params = NULL;
  ie_cm_cc_cause_s_type     *cause_params = NULL;
  uint32                    call_inst = DS3GI_INVALID_CALL_INSTANCE;
  boolean                   ret_val = DS3G_FAILURE;
  boolean                   cm_ret_val = FALSE;
  cm_client_id_type         ds3g_cm_client_id;
  void                      *data_block_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - -*/

  if(!(IS_IN_DS_TASK()))
  {
    DATA_ERR_FATAL("Not executing in ds task");
    return FALSE;
  }

  call_inst = ds3gi_find_call_instance(cm_call_id);
  if (call_inst >= DS3GI_MAX_NUM_CALLS)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Bad call id for answer call", cm_call_id);
    return(ret_val);
  }

  data_block_ptr = (void *)(call_inst);

  if( ds3gi_call_info[call_inst].call_state == DS3G_RINGING_CALL_STATE )
  {
    /*---------------------------------------------------------------------
      Get a pointer to the appropriate handler function table based
      on the current mode and call type.
    ---------------------------------------------------------------------*/
    hdlr_tbl_ptr = 
        ds3gi_get_hdlr_tbl_ptr_for_call_inst(&(ds3gi_call_info[call_inst]));

    if (hdlr_tbl_ptr == NULL)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,"hdlr_table_ptr is Null");
      return DS3G_FAILURE;
    }

    /*-----------------------------------------------------------------------
      Ensure that a handler function table is registered, with a non-null
      user_answer_handler.
    -----------------------------------------------------------------------*/

    if( hdlr_tbl_ptr->user_answer_handler != NULL )
    {
      /*-------------------------------------------------------------------
        Invoke the mode-specific handler function.
      -------------------------------------------------------------------*/
      /*----------------------------------------------------------------
        Intialize parameters before invoking mode specific handler.
      ---------------------------------------------------------------*/
      
      ans_params =  (cm_ans_params_s_type *)modem_mem_alloc(
                                               sizeof(cm_ans_params_s_type), 
                                               MODEM_MEM_CLIENT_DATA_CRIT);
      cause_params = (ie_cm_cc_cause_s_type *)modem_mem_alloc(
                                                sizeof(ie_cm_cc_cause_s_type), 
                                                MODEM_MEM_CLIENT_DATA_CRIT);
      
      if(ans_params == NULL || cause_params == NULL)
      {
        DATA_MSG0_ERROR("memory allocation failed for ds3g_msh_answer_call");
        
        if (NULL != ans_params)
        {
          modem_mem_free(ans_params, MODEM_MEM_CLIENT_DATA_CRIT);
        }

        if (NULL != cause_params)
        {
          modem_mem_free(cause_params, MODEM_MEM_CLIENT_DATA_CRIT);
        }
        return DS3G_FAILURE;
      }
      
      memset((void*)ans_params,0, sizeof(cm_ans_params_s_type));
      memset((void*)cause_params, 0, sizeof(ie_cm_cc_cause_s_type));
      
      hdlr_tbl_ptr->user_answer_handler
                                    (
                                      ds3gi_call_info[call_inst].call_id,
                                      ans_params,
                                      cause_params,
                                       subs_id
                                    );

      /*-------------------------------------------------------------------
        If the call is a PS call, then check to see if mode handler
        accepted the call. If the flag is set to FALSE due to an error in 
        answer handler, then don't call cm_cmd_answer.
      -------------------------------------------------------------------*/
      if (((ds3gi_call_info[call_inst].call_type == DS_CALL_TYPE_PKT)&&
           (ans_params->ans_params.gw_ps_ans.accept == TRUE)) ||
          (ds3gi_call_info[call_inst].call_type == DS_CALL_TYPE_CKT))
      {
        /*-------------------------------------------------------------------
          Set the call state to indicate that we are waiting for the call to
          be connected.
        -------------------------------------------------------------------*/
        ds3gi_call_info[call_inst].call_state = DS3G_WAIT_FOR_CONN_CALL_STATE;
        ds3g_cm_client_id = ds3g_get_cm_client_id();

#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
        /*-------------------------------------------------------------------
          UMTS circuit-switched calls support sending user busy via call
          disconnect message.  Direct Call Manager to send this if required.
        -------------------------------------------------------------------*/
        if( (ds3gi_call_info[call_inst].call_type == DS_CALL_TYPE_CKT) &&
            (ans_params->ans_params.gw_cs_ans.accept == FALSE) &&
            (TRUE == cause_params->present) )
        {
          DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                          "CSData call declined: call id: %d  cause: %d",
                          ds3gi_call_info[call_inst].call_id,
                          cause_params->cause_value);

          if( USER_BUSY == cause_params->cause_value )
          {
            cm_call_sups_params_s_type  sups_params;
            
            /*-------------------------------------------------------------------
              Now send a command to Call Manager to signal user busy.
            -------------------------------------------------------------------*/
            sups_params.call_id = ds3gi_call_info[call_inst].call_id;
            sups_params.cause = cause_params->cause_value;
            cm_num_init( &sups_params.sups_number );

            cm_ret_val = cm_mm_call_cmd_sups(ds3gi_cm_call_cmd_cb,
                                             NULL,
                                             ds3g_cm_client_id,
                                             CM_CALL_SUPS_TYPE_HELD_CALL_REL_OR_UDUB,
                                             &sups_params
                                            );
          }
          else
          {
            ans_params->ans_params.gw_cs_ans.reject_cause_present = cause_params->present;
            ans_params->ans_params.gw_cs_ans.reject_cause = cause_params->cause_value;
            cm_ret_val = cm_mm_call_cmd_answer(ds3gi_cm_call_cmd_cb,
                                               data_block_ptr,
                                               ds3g_cm_client_id,
                                               ds3gi_call_info[call_inst].call_id,
                                               ans_params 
                                              );
          }
        }
        else
#endif /* (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */
        {
          /*-----------------------------------------------------------------
            Now send a command to Call Manager to answer the call.
          -----------------------------------------------------------------*/
          DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "Data call answered, call id: %d",
                          ds3gi_call_info[call_inst].call_id);

          cm_ret_val = cm_mm_call_cmd_answer(ds3gi_cm_call_cmd_cb,
                                             data_block_ptr,
                                             ds3g_cm_client_id,
                                             ds3gi_call_info[call_inst].call_id,
                                             ans_params 
                                            );
        }

        if (cm_ret_val == FALSE)
        {
          DATA_ERR_FATAL("CM call cmd answer failed");
          modem_mem_free((void *)ans_params, MODEM_MEM_CLIENT_DATA_CRIT );
          modem_mem_free((void *)cause_params, MODEM_MEM_CLIENT_DATA_CRIT );
          return(DS3G_FAILURE);
        } 
        else
        {
          ret_val = DS3G_SUCCESS;
        }
      }
    } /* if user_answer_handler != NULL */
    else
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "User answer handler not registered");
    }
  } /* if call_state == RINGING */
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR, "ATA rx'ed, but phone is not RINGING");
  }
  modem_mem_free((void *)ans_params, MODEM_MEM_CLIENT_DATA_CRIT );
  modem_mem_free((void *)cause_params, MODEM_MEM_CLIENT_DATA_CRIT );
  return(ret_val);
} /* ds3g_msh_answer_call() */

#ifdef FEATURE_DATA_LTE

/*===========================================================================

FUNCTION DS3G_MSH_DISABLE_LTE_SELECTION_EX

DESCRIPTION
  This function disables the selection of LTE network. This function in turn
  invokes the CM API that carries out the desired function

PARAMETERS
  Subscription ID

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
)
{
  cm_client_id_type  ds3g_cm_client_id;
  cm_sys_sel_pref_params_s_type sys_sel_pref_params;
  cm_rat_acq_order_pref_s_type  rat_acq_order_pref;
  boolean            ret_val = FALSE;
  cm_ph_cmd_cb_f_type  *sys_sel_cb_f_ptr = NULL;
  ds_3gpp_throt_sys_sel_retry_e_type retry_type = DS_3GPP_LTE_DISABLE_RETRY;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - -*/

  ds3g_cm_client_id = ds3g_get_cm_client_id();

  memset(&sys_sel_pref_params, 0, sizeof(cm_sys_sel_pref_params_s_type));
  memset(&rat_acq_order_pref, 0, sizeof(cm_rat_acq_order_pref_s_type));

  sys_sel_pref_params.mode_pref = CM_MODE_PREF_DISABLE_LTE;
  sys_sel_pref_params.term_pref = CM_PREF_TERM_PWR_CYCLE;

  rat_acq_order_pref.type = CM_ACQ_ORDER_TYPE_GW;
  rat_acq_order_pref.acq_order.gw_acq_order = CM_GW_ACQ_ORDER_PREF_NO_CHANGE;
  sys_sel_pref_params.rat_acq_order_pref_ptr = &rat_acq_order_pref;

  sys_sel_pref_params.band_pref = CM_BAND_PREF_NO_CHANGE;
#if (LTE_BAND_NUM)
  sys_sel_pref_params.lte_band_pref = SYS_LTE_BAND_MASK_CONST_NO_CHG;
#else
  sys_sel_pref_params.lte_band_pref = CM_BAND_PREF_NO_CHANGE;
#endif
  sys_sel_pref_params.tds_band_pref = CM_BAND_PREF_NO_CHANGE;
  sys_sel_pref_params.prl_pref = CM_PRL_PREF_NO_CHANGE;
  sys_sel_pref_params.roam_pref = CM_ROAM_PREF_NO_CHANGE;
  sys_sel_pref_params.hybr_pref = CM_HYBR_PREF_NO_CHANGE;
  sys_sel_pref_params.srv_domain_pref = CM_SRV_DOMAIN_PREF_NO_CHANGE;
  sys_sel_pref_params.network_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_NO_CHANGE;
  sys_sel_pref_params.ue_usage_setting = SYS_UE_USAGE_SETTING_NO_CHANGE;
  sys_sel_pref_params.voice_domain_pref = SYS_VOICE_DOMAIN_PREF_NO_CHANGE;
  sys_sel_pref_params.lte_disable_cause = CM_LTE_DISABLE_CAUSE_PERMANENT_DS;
  sys_sel_pref_params.csg_id = SYS_CSG_ID_INVALID;
  sys_sel_pref_params.csg_rat = SYS_RAT_NONE;

 /*---------------------------------------------------------------------- 
  Populate the sys_sel_cb_f_ptr for RETRY
  Also at this point the retry sm must always be cleared because DS
  has no other notifications from CM/NAS when the PLMN is unblocked.
  ---------------------------------------------------------------------*/
  sys_sel_cb_f_ptr = (cm_ph_cmd_cb_f_type *)ds3g_get_sys_sel_cb_fn_ptr();

  if (sys_sel_cb_f_ptr == NULL)
  {
    DS_3GPP_MSG0_HIGH("sys_sel_cb_f_ptr is NULL");
  }

  ds_3gpp_throt_sys_sel_clr_retry_sm();

  ds_3gpp_throt_sys_sel_set_retry_type(retry_type);

  ds_3gpp_throt_sys_sel_set_sys_sel_pref_params(&sys_sel_pref_params);

  ds_3gpp_throt_sys_sel_set_subs_id(subs_id);

  /*------------------------------------------------------------------------
    Call the CM API to disable the selection of LTE. This is done 
    by changing the service domain pref to CM_MODE_PREF_DISABLE_LTE
 ------------------------------------------------------------------------*/
  ret_val = cm_ph_cmd_sys_sel_pref_5(
                 sys_sel_cb_f_ptr,
                 NULL,
                 ds3g_cm_client_id,
                 subs_id,
                 0,
                 &sys_sel_pref_params);

  DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                  "CM API to disable LTE band returned: %d for sub id %d ",
                  ret_val,subs_id);

  if (ret_val)
  {
    ds3gpp_set_ds_lte_flag(FALSE, subs_id);
  }
  return;
}/*ds3g_msh_disable_lte_selection_ex()*/


/*===========================================================================
FUNCTION DS3G_MSH_ENABLE_LTE_SELECTION_EX

DESCRIPTION
  This function enables the selection of LTE network. This function in turn
  invokes the CM API that carries out the desired function

PARAMETERS
  subscription ID

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
)
{
  cm_client_id_type  ds3g_cm_client_id;
  boolean            ret_val = FALSE;
  cm_sys_sel_pref_params_s_type sys_sel_pref_params;
  cm_rat_acq_order_pref_s_type  rat_acq_order_pref;
  cm_ph_cmd_cb_f_type               *sys_sel_cb_f_ptr = NULL;
  ds_3gpp_throt_sys_sel_retry_e_type retry_type = DS_3GPP_LTE_ENABLE_RETRY;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - -*/

  ds3g_cm_client_id = ds3g_get_cm_client_id();


  memset(&sys_sel_pref_params, 0, sizeof(cm_sys_sel_pref_params_s_type));
  memset(&rat_acq_order_pref, 0, sizeof(cm_rat_acq_order_pref_s_type));
  
  sys_sel_pref_params.mode_pref = CM_MODE_PREF_ENABLE_LTE;
  sys_sel_pref_params.term_pref = CM_PREF_TERM_PWR_CYCLE;
  
  rat_acq_order_pref.type = CM_ACQ_ORDER_TYPE_GW;
  rat_acq_order_pref.acq_order.gw_acq_order = CM_GW_ACQ_ORDER_PREF_NO_CHANGE;
  sys_sel_pref_params.rat_acq_order_pref_ptr = &rat_acq_order_pref;
  
  sys_sel_pref_params.band_pref = CM_BAND_PREF_NO_CHANGE;
#if (LTE_BAND_NUM)
  sys_sel_pref_params.lte_band_pref = SYS_LTE_BAND_MASK_CONST_NO_CHG;
#else
  sys_sel_pref_params.lte_band_pref = CM_BAND_PREF_NO_CHANGE;
#endif
  sys_sel_pref_params.tds_band_pref = CM_BAND_PREF_NO_CHANGE;
  sys_sel_pref_params.prl_pref = CM_PRL_PREF_NO_CHANGE;
  sys_sel_pref_params.roam_pref = CM_ROAM_PREF_NO_CHANGE;
  sys_sel_pref_params.hybr_pref = CM_HYBR_PREF_NO_CHANGE;
  sys_sel_pref_params.srv_domain_pref = CM_SRV_DOMAIN_PREF_NO_CHANGE;
  sys_sel_pref_params.network_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_NO_CHANGE;
  sys_sel_pref_params.ue_usage_setting = SYS_UE_USAGE_SETTING_NO_CHANGE;
  sys_sel_pref_params.voice_domain_pref = SYS_VOICE_DOMAIN_PREF_NO_CHANGE;
  sys_sel_pref_params.lte_disable_cause = CM_LTE_DISABLE_CAUSE_NO_CHANGE;
  sys_sel_pref_params.csg_id = SYS_CSG_ID_INVALID;
  sys_sel_pref_params.csg_rat = SYS_RAT_NONE;

  /*---------------------------------------------------------------------- 
    Populate the sys_sel_cb_f_ptr for RETRY
    Also at this point the retry sm must always be cleared because DS
    has no other notifications from CM/NAS when the PLMN is unblocked.
  ---------------------------------------------------------------------*/
  sys_sel_cb_f_ptr = (cm_ph_cmd_cb_f_type *)ds3g_get_sys_sel_cb_fn_ptr();

  if (sys_sel_cb_f_ptr == NULL)
  {
    DS_3GPP_MSG0_HIGH("sys_sel_cb_f_ptr is NULL");
  }

  ds_3gpp_throt_sys_sel_clr_retry_sm();

  ds_3gpp_throt_sys_sel_set_retry_type(retry_type);

  ds_3gpp_throt_sys_sel_set_subs_id(subs_id);

  ds_3gpp_throt_sys_sel_set_sys_sel_pref_params(&sys_sel_pref_params);

  /*------------------------------------------------------------------------
    Call the CM API to enable the selection of LTE. This is done by
    changing the service domain pref to CM_MODE_PREF_ENABLE_LTE
  ------------------------------------------------------------------------*/
  if(ds3gpp_is_lte_flag_disabled(subs_id))
  {
    ret_val = cm_ph_cmd_sys_sel_pref_5(
                 sys_sel_cb_f_ptr,
                 NULL,
                 ds3g_cm_client_id,
                 subs_id,
                 0,
                 &sys_sel_pref_params);

    DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                    "CM API to enable LTE selection returned:%d for sub_id:%d ",
                    ret_val,subs_id);

    if (ret_val)
    {
      ds3gpp_set_ds_lte_flag(TRUE, subs_id);
    }

  }
  else
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                    "CM API Ignore to enable LTE %d for subs_id",
                    ret_val,subs_id);
  }
  return;
}/*ds3g_msh_enable_lte_selection_ex()*/

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
)
{

  boolean ret_val = FALSE;
  cm_ph_cmd_cb_f_type *sys_sel_cb_f_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*---------------------------------------------------------------------- 
    Populate the sys_sel_cb_f_ptr for RETRY
    Also at this point the retry sm must always be cleared because DS
    has no other notifications from CM/NAS when the PLMN is unblocked.
  ---------------------------------------------------------------------*/
  sys_sel_cb_f_ptr = (cm_ph_cmd_cb_f_type *)ds3g_get_sys_sel_cb_fn_ptr();

  if (sys_sel_cb_f_ptr == NULL)
  {
    DS_3GPP_MSG0_HIGH("sys_sel_cb_f_ptr is NULL");
  }

  DS_3GPP_MSG3_ERROR("Blocking Plmn for timer:%d subs_id:%d ",
                      plmn_blocking_info.blocking_interval,
                      plmn_blocking_info.asubs_id, 0);

  if ( plmn_blocking_info.blocking_interval > 0)
  {
    if(cm_ph_cmd_detach_and_plmn_blocking(sys_sel_cb_f_ptr, 
                                          (void *)plmn_blocking_info.asubs_id, 
                                          ds3g_get_cm_client_id(),
                                          plmn_blocking_info) != TRUE)
    {
      DS_3GPP_MSG0_ERROR("Error while invoking CM API to block PLMN");
      return ret_val;
    }

  }

  ret_val = TRUE;
  return ret_val;
}

/*===========================================================================
FUNCTION DS3G_MSH_ENABLE_LTE_PLMN_BLOCKING_ON_PLMN_ID_EX

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
boolean ds3g_msh_enable_lte_plmn_blocking_on_plmn_id_ex
(
  sys_plmn_id_s_type plmn_id,
  uint16                 request_pref,
  sys_modem_as_id_e_type subs_id
)
{
  sys_plmn_id_s_type zero_plmn_id;
  cm_plmn_blocking_info_s_type plmn_blocking_info;
  cm_ph_cmd_cb_f_type               *sys_sel_cb_f_ptr = NULL;
  ds_3gpp_throt_sys_sel_retry_e_type retry_type = 
                                             DS_3GPP_LTE_PLMN_UNBLOCK_RETRY;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - */

  memset((void*)&zero_plmn_id, 0, sizeof(sys_plmn_id_s_type));
  memset(&plmn_blocking_info, 0, sizeof(cm_plmn_blocking_info_s_type));
  if(memcmp((void*)&(plmn_id),
            (void*)&(zero_plmn_id), 
            sizeof(sys_plmn_id_s_type)) == 0)
  {
    DS_3GPP_MSG3_MED("PLMN being inspected is zero",0,0,0);
    return FALSE;
  }
  DS_3GPP_MSG1_HIGH("ds3g_msh_enable_lte_plmn_blocking_on_plmn_id_ex:preference :%d", request_pref);

 /*----------------------------------------------------------------------
   Populate the contents of the PLMN blocking structure
 ----------------------------------------------------------------------*/
  plmn_blocking_info.request_pref =(cm_plmn_blocking_pref_e_type)request_pref;
                     plmn_blocking_info.asubs_id = subs_id;
     
  memscpy((void*)&(plmn_blocking_info.plmn),
          sizeof(sys_plmn_id_s_type),
          (void*)&(plmn_id),
          sizeof(sys_plmn_id_s_type));

  if(! ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    DS_3GPP_MSG1_ERROR("Error in Sub id %d while sending detach"
                       " and plmn blocking",subs_id);
    return FALSE;    
  }

  subs_id = plmn_blocking_info.asubs_id;

    /*---------------------------------------------------------------------- 
    Populate the sys_sel_cb_f_ptr for RETRY
    Also at this point the retry sm must always be cleared because DS
    has no other notifications from CM/NAS when the PLMN is unblocked.
    ---------------------------------------------------------------------*/
  sys_sel_cb_f_ptr = (cm_ph_cmd_cb_f_type *)ds3g_get_sys_sel_cb_fn_ptr();

  if (sys_sel_cb_f_ptr == NULL)
  {
    DS_3GPP_MSG0_HIGH("sys_sel_cb_f_ptr is NULL");
  }

  ds_3gpp_throt_sys_sel_clr_retry_sm();

  ds_3gpp_throt_sys_sel_set_retry_type(retry_type);

  ds_3gpp_throt_sys_sel_set_plmn_blk_info(&plmn_blocking_info);

  if(cm_ph_cmd_detach_and_plmn_blocking(sys_sel_cb_f_ptr, 
                                        NULL, 
                                        ds3g_get_cm_client_id(),
                                        plmn_blocking_info) != TRUE)
  {
    DS_3GPP_MSG3_ERROR("Error while invoking CM API to unblock PLMN", 
                        0,0,0);
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================
FUNCTION DS3G_MSH_ENABLE_LTE_PLMN_BLOCKING_ON_PLMN_ID

DESCRIPTION
  This function resets the lte plmn blocking list on the plmn id passed.This
  function in turn invokes the CM API that carries out the desired function.

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
)
{
  sys_modem_as_id_e_type subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - */
  /* as this function getting used under RPM feature, it is better to use GW
     Primary subs_id */
  subs_id
    = ds3g_mmgsdi_get_sub_id_from_session_type(MMGSDI_GW_PROV_PRI_SESSION);

  return ds3g_msh_enable_lte_plmn_blocking_on_plmn_id_ex(
             plmn_id,
             CM_PLMN_BLOCKING_PREF_PLMN_UNBLOCKING_ONLY,
             subs_id );
}

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
boolean ds3g_msh_enable_lte_plmn_unblocking(void)
{
  sys_plmn_id_s_type      current_plmn_id;
  sys_modem_as_id_e_type  subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - */
  /* as this function getting used under RPM feature, it is better to use GW
     Primary subs_id */
  subs_id
    = ds3g_mmgsdi_get_sub_id_from_session_type(MMGSDI_GW_PROV_PRI_SESSION);
  /*----------------------------------------------------------------------------
    Memset the current plmn - this is a local 
  ----------------------------------------------------------------------------*/
  memset((void*)&current_plmn_id,0,sizeof(sys_plmn_id_s_type));

  /*----------------------------------------------------------------------------
    Get the cureent plmn 
  ----------------------------------------------------------------------------*/
  ds_3gpp_pdn_cntx_get_current_plmn_ex(&current_plmn_id, subs_id);

 return ds3g_msh_enable_lte_plmn_blocking_on_plmn_id_ex(
         current_plmn_id, CM_PLMN_BLOCKING_PREF_PLMN_UNBLOCKING_ONLY, subs_id);
}

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
)
{
  cm_pdn_connectivity_abort_s_type abort_params;  /* Params used to abort */
  boolean                          ret_val = FALSE;
  cm_client_id_type                ds3g_cm_client_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - */

  DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                  "Abort PDN connectivity request with sdf_id %d subs_id:%d",
                  sdf_id, subs_id);

  abort_params.sdf_id = sdf_id;
  abort_params.asubs_id = subs_id;
  ds3g_cm_client_id = ds3g_get_cm_client_id();

  ret_val = cm_mm_call_cmd_pdn_connectivity_abort_req(ds3gi_cm_call_cmd_cb,
                                                     (void*)&abort_params.sdf_id,
                                                      ds3g_cm_client_id,
                                                      &abort_params);

  if(ret_val != DS3G_SUCCESS)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"Could not abort PDN connection request");
  }

  return ret_val;
}/* ds3g_msh_abort_pdn_connection_ex */

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
)
{
  sys_modem_as_id_e_type subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - */
  subs_id = ds3g_get_ps_subs_id();
  return ds3g_msh_abort_pdn_connection_ex(user_info_ptr, sdf_id, subs_id );
}

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
)
{
  cm_pdn_disconnect_s_type         disconnect_params;/*Params used to disc*/
  boolean                          ret_val = FALSE;
  cm_client_id_type                ds3g_cm_client_id;
  uint32                           call_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - */

  DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                  "PDN disconnect request for cm_call_id %d, subs_id:%d",
                  cm_call_id,subs_id);

  if (CM_CALL_ID_INVALID == cm_call_id)
  {    
    return FALSE;
  }

  call_inst = ds3gi_find_call_instance(cm_call_id);
  if (call_inst >= DS3GI_MAX_NUM_CALLS)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Trying to end non-existent call id %d", cm_call_id);
    return(ret_val);
  }  

  memset(&disconnect_params,0, sizeof(disconnect_params));

  disconnect_params.sdf_id = sdf_id;
  disconnect_params.eps_bearer_id = eps_bearer_id;
  disconnect_params.asubs_id = subs_id;
  if(local_discon == TRUE)
  {
    /*------------------------------------------------------------------------
      Phone is not in LTE.
      Tear down the context locally. This is to be done by setting ESM local
      cause to ESM_CONTEXT_TRANSFERED_DUE_TO_IRAT
    ------------------------------------------------------------------------*/
    disconnect_params.esm_local_cause.valid = TRUE;
    disconnect_params.esm_local_cause.local_cause = 
                                            ESM_CONTEXT_TRANSFERED_DUE_TO_IRAT;
  }

  /*-----------------------------------------------------------------
    Set the call state to indicate that we are waiting for the call
    to be ended.
  -----------------------------------------------------------------*/
  ds3gi_call_info[call_inst].call_state = DS3G_END_CALL_STATE;

  ds3g_cm_client_id = ds3g_get_cm_client_id();
  ret_val = cm_mm_call_cmd_pdn_disconnect_req(ds3gi_cm_call_cmd_cb,
                               (void*)&disconnect_params.eps_bearer_id,
                               ds3g_cm_client_id,
                               &disconnect_params);

  if(ret_val != DS3G_SUCCESS)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"Could not send PDN disconnect request");
  }

  return ret_val;
}/* ds3g_msh_pdn_disconnect_req_ex */

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
)
{
  sys_modem_as_id_e_type subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - */
  subs_id = ds3g_get_ps_subs_id();
  return ds3g_msh_pdn_disconnect_req_ex(user_info_ptr,
                                        sdf_id,
                                        cm_call_id,
                                        eps_bearer_id,
                                        local_discon,
                                        subs_id);

}

/*===========================================================================

FUNCTION DS3G_MSH_INITIATE_PDN_CONNECTION_EX

DESCRIPTION
  This function originates a PDN connection. This is applicable only to the
  LTE mode of operation.

PARAMETERS
  user_info_ptr - currently unused. Will be needed if user_end_hdlr is to be
                  invoked
 
  seq_num       - Seq_num to be sent out
DEPENDENCIES
  

RETURN VALUE  
  DS3G_SUCCESS - call was originated with CM.  
  DS3G_FAILURE - call could not be originated.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_msh_initiate_pdn_connection_ex
(
  void                   *user_info_ptr,  /* points to user supplied info      */
  uint8                   seq_num,
  sys_modem_as_id_e_type  subs_id
)
{
  ds3g_hdlr_func_tbl_type      *hdlr_tbl_ptr;   /* Pointer to hdlr table */
  cm_pdn_connectivity_s_type   *lte_orig_params = NULL; /* Pointer to orig params*/
  ds3g_orig_params_s           orig_params;
  boolean                      lte_orig_params_changed = FALSE;
  boolean                      hdlr_result;
  boolean                      retVal = DS3G_FAILURE; /* Did call orig */
  cm_srv_type_e_type           cm_srv_type;
  cm_client_id_type            ds3g_cm_client_id;
  ds_3gpp_pdn_call_info_type  *call_info_p = (ds_3gpp_pdn_call_info_type*)user_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - */

  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"Initiate PDN connectivity request");
  lte_orig_params =  (cm_pdn_connectivity_s_type*)modem_mem_alloc(
                                            sizeof(cm_pdn_connectivity_s_type),
                                            MODEM_MEM_CLIENT_DATA_CRIT);

  if(lte_orig_params == NULL)
  {
    DATA_MSG0_ERROR("memory allocation failed for ds3g_msh_initiate_pdn_connection_ex");
    return retVal;
  }

  if (call_info_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("call_info_p is NULL");
    return retVal;
  }

  /* Memset the orig params structure to 0 */
  memset(lte_orig_params,0,sizeof(cm_pdn_connectivity_s_type));
  memset(&orig_params,0,sizeof(ds3g_orig_params_s));

  /*Map the orig params into the structure to be send in orig_call_hdlr_ext*/
  orig_params.lte_orig_params_p = lte_orig_params;
  orig_params.lte_orig_params_changed = &lte_orig_params_changed;
  lte_orig_params->pdn_data.seq_num = seq_num;
  lte_orig_params->asubs_id = subs_id;

  lte_orig_params->pdn_data.low_priority_signaling = 
                                           call_info_p->low_priority_signaling;
  /*-------------------------------------------------------------------------
    Point to the appropriate handler function table based on the requested
    mode and call type.
  -------------------------------------------------------------------------*/
  hdlr_tbl_ptr = ds3gi_hdlr_tbl[SYS_SYS_MODE_LTE][DS_CALL_TYPE_PKT];

  DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                  "PDN CONN REQ Seq Num sent out is %d, subs_id:%d",
                  lte_orig_params->pdn_data.seq_num, subs_id);

  /*-------------------------------------------------------------------------
    Ensure that a handler function table is registered, with a non-null
    originate_call_handler_ext.
  -------------------------------------------------------------------------*/
  if( hdlr_tbl_ptr != NULL )
  {
    if( hdlr_tbl_ptr->originate_call_handler_ext != NULL )
    {
      hdlr_result = hdlr_tbl_ptr->originate_call_handler_ext
                    (
                       NULL,
                       0,
                       orig_params,
                       &cm_srv_type,
                       user_info_ptr,
                       subs_id
                    );

      if(hdlr_result == DS3G_SUCCESS)
      {
        ds3g_cm_client_id = ds3g_get_cm_client_id();
        retVal = \
          cm_mm_call_cmd_pdn_connectivity_req(ds3gi_cm_call_cmd_cb,
                                              (void*)(uint32)(lte_orig_params->sdf_id),
                                              ds3g_cm_client_id,
                                              lte_orig_params);
         DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,"Call origination with SDF_ID %d", lte_orig_params->sdf_id); 
        if(retVal == DS3G_FAILURE)
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_HIGH ,"Call origination with CM failed");
        }
      }
      else
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                        "Orig Params returned FAILURE. Cannot orig PDN conn");
      } /* hdlr_result == DS3G_SUCCESS*/
    } /* originate_call_handler != NULL*/
  } /* hdlr_tbl_ptr != NULL */
  modem_mem_free((void *)lte_orig_params, MODEM_MEM_CLIENT_DATA_CRIT );
  return retVal;
}

/*===========================================================================

FUNCTION DS3G_MSH_INITIATE_PDN_CONNECTION

DESCRIPTION
  This function originates a PDN connection. This is applicable only to the
  LTE mode of operation.

PARAMETERS
  user_info_ptr - currently unused. Will be needed if user_end_hdlr is to be
                  invoked
 
  seq_num       - Seq_num to be sent out
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
)
{
  sys_modem_as_id_e_type     subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - */
  subs_id = ds3g_get_ps_subs_id();
  return ds3g_msh_initiate_pdn_connection_ex(user_info_ptr,seq_num,subs_id);
}
/*===========================================================================

FUNCTION DS3G_MSH_ALLOC_BEARER_REQ_EX

DESCRIPTION
  This function requests a bearer allocation req. This is applicable only to the
  LTE mode of operation.

PARAMETERS
  user_info_ptr -  structure needed to fill in the CM request.
  subs_id - subscription id
DEPENDENCIES
  

RETURN VALUE  
  DS3G_SUCCESS - Bearer resource alloc was requested with CM.  
  DS3G_FAILURE - Bearer resource alloc was not successful.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_msh_alloc_bearer_req_ex
(
  void                   *user_info_ptr, /* points to user supplied info      */
  sys_modem_as_id_e_type  subs_id
)
{
  cm_res_alloc_s_type     *bearer_alloc_params;  /* params used to request*/
  boolean                 ret_val = FALSE;
  cm_client_id_type       ds3g_cm_client_id;
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - -*/

  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,
                  "Bearer Resource Allocation Request, subs_id:%d",
                  subs_id);
  bearer_alloc_params = (cm_res_alloc_s_type *)user_info_ptr;
  bearer_alloc_params->asubs_id = subs_id;

  ds3g_cm_client_id = ds3g_get_cm_client_id();
  ret_val = cm_mm_call_cmd_res_alloc_req(ds3gi_cm_call_cmd_cb,
                                        (void*)&(bearer_alloc_params->sdf_id),
                                        ds3g_cm_client_id,
                                        bearer_alloc_params);
  if(ret_val != DS3G_SUCCESS)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,"bearer alloc request failed in ds3g");
  }
  return ret_val;
}/* ds3g_msh_alloc_bearer_req_ex */

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
)
{
  sys_modem_as_id_e_type subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - */
  subs_id = ds3g_get_ps_subs_id();
  return ds3g_msh_alloc_bearer_req_ex(user_info_ptr, subs_id);
}


/*===========================================================================

FUNCTION DS3G_MSH_MODIFY_BEARER_REQ_EX

DESCRIPTION
  This function requests a bearer modification req. This is applicable only to the
  LTE mode of operation.

PARAMETERS
  user_info_ptr -  structure needed to fill in the CM request.
  subs_id - subscription_id
DEPENDENCIES
  

RETURN VALUE  
  DS3G_SUCCESS - Bearer resource alloc was requested with CM.  
  DS3G_FAILURE - Bearer resource alloc was not successful.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_msh_modify_bearer_req_ex
(
  void                *user_info_ptr, /* points to user supplied info      */
  sys_modem_as_id_e_type subs_id
)
{
  cm_res_mod_s_type      *bearer_modify_params;  /* params used to request*/
  boolean                 ret_val = FALSE;
  cm_client_id_type       ds3g_cm_client_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - */

  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,
                  "Bearer Resource Modification Request, subs_id:%d",
                  subs_id);

  bearer_modify_params = (cm_res_mod_s_type *)user_info_ptr;
  bearer_modify_params->asubs_id = subs_id;

  ds3g_cm_client_id = ds3g_get_cm_client_id();
  ret_val = cm_mm_call_cmd_bearer_res_mod_req(ds3gi_cm_call_cmd_cb,
                                      (void*)&(bearer_modify_params->sdf_id),
                                      ds3g_cm_client_id,
                                      bearer_modify_params);
  if(ret_val != DS3G_SUCCESS)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,"bearer modify request failed in ds3g");
  }
  return ret_val;
}/* ds3g_msh_alloc_bearer_req */

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
)
{
  sys_modem_as_id_e_type subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - */
  subs_id = ds3g_get_ps_subs_id();
  return ds3g_msh_modify_bearer_req_ex(user_info_ptr, subs_id);
}

/*===========================================================================

FUNCTION DS3G_MSH_ABORT_BEARER_REQ_EX

DESCRIPTION
  This function aborts a bearer allocation request. This is applicable only to the
  LTE mode of operation.

PARAMETERS
  user_info_ptr - currently unused. Will be needed if user_end_hdlr is to be
                  invoked
  Subs_id - subscription_id

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
  void                *user_info_ptr, /* points to user supplied info      */
  sys_modem_as_id_e_type subs_id
)
{
  cm_res_alloc_abort_s_type      *abort_params = NULL;/*Params used to disc*/
  boolean                        ret_val = FALSE;
  cm_client_id_type              ds3g_cm_client_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - */

  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,"Abort Bearer Request, subs_id:%d", subs_id);

  abort_params =(cm_res_alloc_abort_s_type *)user_info_ptr;
  abort_params->asubs_id = subs_id;
  ds3g_cm_client_id = ds3g_get_cm_client_id();

  ret_val =  cm_mm_call_cmd_res_alloc_abort_req(ds3gi_cm_call_cmd_cb,
                                                (void*)&abort_params->sdf_id,
                                                ds3g_cm_client_id,
                                                abort_params);

  if(ret_val != DS3G_SUCCESS)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                    "bearer alloc abort request failed in ds3g");
  }
  return ret_val;
}/* ds3g_msh_abort_bearer_req */

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
)
{
  sys_modem_as_id_e_type subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - */
  subs_id = ds3g_get_ps_subs_id();
  return ds3g_msh_abort_bearer_req_ex(user_info_ptr, subs_id);
}

#endif /* FEATURE_DATA_LTE */

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
)
{
  ds3g_hdlr_func_tbl_type      *hdlr_tbl_ptr;
  boolean                       hdlr_result;
  const byte                   *s_ptr;
  uint8                         dial_string_len = 0;
  cm_digit_mode_e_type          cm_digit_mode;
  cm_num_s_type                 cm_dial_num;
  cm_num_s_type                 calling_num;
  cm_orig_alpha_s_type          alpha_buf;

  /* Make large structures static to avoid overflowing stack */  
  static cm_cdma_orig_params_s_type    cdma_orig_params;
  static cm_gw_cs_orig_params_s_type   gw_cs_orig_params;
  static cm_gw_ps_orig_params_s_type   gw_ps_orig_params;

  boolean                       cdma_params_changed  = FALSE;
  boolean                       gw_cs_params_changed = FALSE;
  boolean                       gw_ps_params_changed = FALSE;

  cm_cdma_orig_params_s_type   *cdma_orig_params_ptr  = NULL;
  cm_gw_cs_orig_params_s_type  *gw_cs_orig_params_ptr = NULL;
  cm_gw_ps_orig_params_s_type  *gw_ps_orig_params_ptr = NULL;

  cm_srv_type_e_type            cm_srv_type;
  uint32                        call_inst = DS3GI_INVALID_CALL_INSTANCE;

  boolean                       ret_val = DS3G_FAILURE;   /* did call orig */
  boolean                       cm_ret_val = FALSE; 
  ds_call_type_e_type           orig_call_type; /* saved original call_type */
  cm_call_type_e_type           cm_call_type;
  cm_client_id_type             ds3g_cm_client_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - */

  if(!(IS_IN_DS_TASK()))
  {
    DATA_ERR_FATAL("Not executing in ds task");
    return FALSE;
  }

  if( sys_mode == SYS_SYS_MODE_NONE )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Can't initiate call - System mode NONE");
    return FALSE;
  }

  if(( call_type == DS_NUM_CALL_TYPES ) ||
     (call_type == DS_CALL_TYPE_UNSPECIFIED))
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Can't initiate call - Invalid Call Type");
    return FALSE;
  }

   
  /*------------------------------------------------------------------------
    If it is DS_CALL_TYPE_PKT_IS707B, we need to convert it to DS_CALL_TYPE_PKT
    Because the call_type is used for accessing mode handler in DS module.
  -------------------------------------------------------------------------*/
  orig_call_type = call_type;
  if (call_type == DS_CALL_TYPE_PKT_IS707B)
  {
    call_type = DS_CALL_TYPE_PKT;
  }

  call_inst = ds3gi_alloc_call_instance_record(call_type,sys_mode,subs_id);
  if (call_inst >= DS3GI_MAX_NUM_CALLS)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "cant initiate - Too many calls");
    return(ret_val);
  }

  if (ds3gi_call_info[call_inst].call_state != DS3G_IDLE_CALL_STATE)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,"Can't orig, incoming call in progress");
    return ret_val;
  }

  /*-------------------------------------------------------------------------
    Point to the appropriate handler function table based on the requested
    mode and call type.
  -------------------------------------------------------------------------*/
  DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                  "Initiate data call with mode %d, subs_id:%d",
                  sys_mode, subs_id);
  hdlr_tbl_ptr = ds3gi_hdlr_tbl[sys_mode][call_type];

  /*-------------------------------------------------------------------------
    Ensure that a handler function table is registered, with a non-null
    originate_call_handler.
  -------------------------------------------------------------------------*/
  if( hdlr_tbl_ptr != NULL )
  {
    if( hdlr_tbl_ptr->originate_call_handler != NULL )
    {
      /*---------------------------------------------------------------------
        Count the number of characters/digits in the dial string.
      ---------------------------------------------------------------------*/
      s_ptr = dial_string;

      while( *s_ptr++ != '\0' )
      {
        dial_string_len++;
      };

      /*---------------------------------------------------------------------
        Enter critical section due to static Call Manager variables.
      ---------------------------------------------------------------------*/
      DS3G_ENTER_CALL_OP_CS();
      
      /*---------------------------------------------------------------------
        Invoke the mode-specific handler function, which will return all
        the necessary origination parameters.
      ---------------------------------------------------------------------*/
      /*----------------------------------------------------------------
          Intialize parameters before invoking mode specific handler.
      ---------------------------------------------------------------*/
      memset((void*)&cdma_orig_params, 
              0, 
              sizeof(cm_cdma_orig_params_s_type));
      memset((void*)&gw_cs_orig_params, 
              0, 
              sizeof(cm_gw_cs_orig_params_s_type));
      memset((void*)&gw_ps_orig_params,
              0, 
              sizeof(cm_gw_ps_orig_params_s_type));
      hdlr_result = hdlr_tbl_ptr->originate_call_handler
                    (
                       dial_string,
                       dial_string_len,
                       &cdma_orig_params,         /* cdma orig params      */
                       &cdma_params_changed,
                       &gw_cs_orig_params,        /* GW CS orig parms      */
                       &gw_cs_params_changed,
                       &gw_ps_orig_params,        /* GW PS orig parms      */
                       &gw_ps_params_changed,
                       &cm_srv_type,
                       user_info_ptr,
                       subs_id
                    );

      /*---------------------------------------------------------------------
        Originate a call only if the mode-specific handler says its ok
        to originate.
      ---------------------------------------------------------------------*/
      if( hdlr_result == DS3G_SUCCESS )
      {

        /*-------------------------------------------------------------------
          Set the digit mode.
          NOTE: The function prototype for cm_num_fill expects a CM enum
          for digit mode (cm_digit_mode) . But here we are setting it to 
          a boolean. Data Services only cares if it is a 0 or 1. Meaning 
          1 = dial str has non-digit chars and 0 = dial str has digits only
        -------------------------------------------------------------------*/
        if(digit_mode)
        {
          cm_digit_mode = CM_DIGIT_MODE_8BIT_ASCII;
        }else
        {
          cm_digit_mode = CM_DIGIT_MODE_4BIT_DTMF;
        }
        /*-------------------------------------------------------------------
          Convert the dial string to the format required by Call Manager.
        -------------------------------------------------------------------*/
        cm_num_fill( &cm_dial_num, 
                     dial_string, 
                     dial_string_len,
                     cm_digit_mode 
                   );

#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
        /*-------------------------------------------------------------------
          Set dialed number numbering plan for UMTS/GSM calls. 
        -------------------------------------------------------------------*/
        if ( (SYS_SYS_MODE_WCDMA == sys_mode) ||
             (SYS_SYS_MODE_GSM   == sys_mode) ||
             (SYS_SYS_MODE_TDS == sys_mode)
           )
        {
          cm_dial_num.number_plan = CM_NPI_ISDN;
        }
#endif /* (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */

        /*-------------------------------------------------------------------
          We do not specify any calling number or alpha tag, so set the
          lengths to zero.
        -------------------------------------------------------------------*/
        calling_num.len = 0;
        alpha_buf.len = 0;

        /*-------------------------------------------------------------------
         Set the call state to indicate that a data call origination is
         in progress, and remember the call type.
        -------------------------------------------------------------------*/

        ds3gi_call_info[call_inst].call_state = DS3G_ORIG_CALL_STATE;
        ds3gi_call_info[call_inst].call_type  = call_type;
        ds3gi_call_info[call_inst].call_mode  = sys_mode;

        /*-------------------------------------------------------------------
          Set orig params ptr appropriately.
        -------------------------------------------------------------------*/
        if (cdma_params_changed == TRUE)
        {
          cdma_orig_params_ptr = &cdma_orig_params;
        }

        if (gw_cs_params_changed == TRUE)
        {
          gw_cs_orig_params_ptr = &gw_cs_orig_params;
        }

        if (gw_ps_params_changed == TRUE)
        {
          gw_ps_orig_params_ptr = &gw_ps_orig_params;
        }

        ds3g_cm_client_id = ds3g_get_cm_client_id();
        /*-------------------------------------------------------------------
           Now send a command to Call Manager to originate the call.  Note
           that the call_id field is filled in below via the cm cmd.
           We convert orig_call_type, since it may be IS707B data call.
        -------------------------------------------------------------------*/
        cm_call_type = ds3g_get_cm_call_type(orig_call_type);
        if (cm_call_type == CM_CALL_TYPE_NONE)
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                          "Can't initiate call - CM_CALL_TYPE_NONE");
          ret_val = DS3G_FAILURE;
        }

        if(ds3gsubsmgr_is_device_mode_multi_sim_standby() == TRUE) 
        {

          if( ds3gsubsmgr_is_subs_id_valid(subs_id) )
          {
            cm_ret_val = cm_mm_call_cmd_orig_cc_per_subs
                         ( ds3gi_cm_call_cmd_ext_cb,       /* callback   */
                           (void*) call_inst,              /* data blk   */
                           ds3g_cm_client_id,              /* client id  */
                           cm_call_type,                   /* call_type  */
                           cm_srv_type,                    /* srv type   */
                           &calling_num,          /* calling number      */
                           &cm_dial_num,          /* # being called      */
                           &alpha_buf,            /* alpha buff/tag      */
                           cdma_orig_params_ptr,   /* cdma orig params   */
                           gw_cs_orig_params_ptr,  /* GW CS orig parms   */
                           gw_ps_orig_params_ptr,  /* GW PS orig parms   */
                           CM_CALL_ID_INVALID,    /*  Switch_out_call_id */
                           CM_ORIG_CLIENT_CATEG_USER, 
                                           /* Initiating client category */
                           subs_id,                  /*  Subscription id */
                           &ds3gi_call_info[call_inst].call_id
                           );
          }
          else
          {
            DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "INVALID SUBS_ID"); 
            ret_val = FALSE;
          }
        }
        else
        {
          cm_ret_val = cm_mm_call_cmd_orig_exten3
                       ( ds3gi_cm_call_cmd_ext_cb,       /* callback   */
                         (void*) call_inst,              /* data blk   */
                         ds3g_cm_client_id,              /* client id  */
                         cm_call_type,                   /* call_type  */
                         cm_srv_type,                    /* srv type   */
                         &calling_num,          /* calling number      */
                         &cm_dial_num,          /* # being called      */
                         &alpha_buf,            /* alpha buff/tag      */
                         cdma_orig_params_ptr,   /* cdma orig params   */
                         gw_cs_orig_params_ptr,  /* GW CS orig parms   */
                         gw_ps_orig_params_ptr,  /* GW PS orig parms   */
                         CM_CALL_ID_INVALID,    /*  Switch_out_call_id */
                         CM_ORIG_CLIENT_CATEG_USER,
                                        /*  Initiating client category */
                         &ds3gi_call_info[call_inst].call_id
                           );
        }

        /*---------------------------------------------------------------------
          Exit critical section.
        ---------------------------------------------------------------------*/
        DS3G_EXIT_CALL_OP_CS();
        
        if ((cm_ret_val == FALSE) ||
            (ds3gi_call_info[call_inst].call_id == CM_CALL_ID_INVALID))
        {
          /*-----------------------------------------------------------------
            If CM has run into errors, fail call origination & return an 
            error : ret_val is initialized to FALSE above. so just fall thru
          -----------------------------------------------------------------*/
          DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,"CM failed DS orig");
        }
        else
        {
          /*-----------------------------------------------------------------
             CM accepted the origination and gave us a call id.
          -----------------------------------------------------------------*/
          DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                          "Originating Data call, id: %d, type: %d",
                          ds3gi_call_info[call_inst].call_id, orig_call_type);


          /*-----------------------------------------------------------------
           Pass the call id allocated by Call Manager to the mode-specific
           handler.
          -----------------------------------------------------------------*/
          if( hdlr_tbl_ptr->call_id_handler != NULL )
          {
            hdlr_tbl_ptr->call_id_handler
                                      ( ds3gi_call_info[call_inst].call_id,
                                        user_info_ptr,
                                        subs_id
                                      );
          }

          /*------------------------------------------------------------------
            Keep a note of the call type and the mode in which call has been
            originated. this will be used to find the mode handler when the 
            call has ended to retrieve statistics
            NOTE: remove this HACK when CM supports M-PDP 
          ------------------------------------------------------------------*/
          ds3g_set_last_call_mode(sys_mode);
          ds3g_set_last_call_type(call_type);

          ret_val = TRUE;

#ifdef FEATURE_DATA_PKT_CLASSIFIER
          ds707_pkt_classifier_cm_call_connected(
                                         ds3g_cm_client_id,
                                         ds3gi_call_info[call_inst].call_id);
#endif/* FEATURE_DATA_PKT_CLASSIFIER */
        }
      } /* if hdlr_result == DS3G_SUCCESS */
      else
      {
        /*---------------------------------------------------------------------
          Exit critical section.
        ---------------------------------------------------------------------*/
        DS3G_EXIT_CALL_OP_CS();

        DATA_3GMGR_MSG0(MSG_LEGACY_ERROR, "Can't orig, Mode-sp. Hdlr error");
      }
    } /* if originate_call_handler != NULL */
    else
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "Can't orig, orig_handler not registered");
    }
  } /* if hdlr_tbl_ptr != NULL */
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Can't orig, handler table not registered");
  }

  /*-----------------------------------------------------------------
    if for any reasons we fail call setup, free the call instance
    allocated for the call
  -----------------------------------------------------------------*/
  if( ret_val!= TRUE && call_inst < DS3GI_MAX_NUM_CALLS )
  {
    ds3gi_free_call_instance_record(call_inst);
  }

  return(ret_val);
} /* ds3g_msh_initiate_call_ex() */

/*===========================================================================
FUNCTION      DS3G_MSH_INITIATE_CALL

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
boolean ds3g_msh_initiate_call
(
  ds_call_type_e_type  call_type,     /* Call type - Circuit or Packet     */
  sys_sys_mode_e_type  sys_mode,      /* WCDMA, GSM, CDMA                  */
  const byte          *dial_string,   /* Null-terminated dial string       */
  boolean              digit_mode,    /* 1 = dial str has non-digit chars  */
                                      /* 0 = dial str has only digits      */
  void                *user_info_ptr  /* points to user supplied info      */
)
{
  sys_modem_as_id_e_type       subs_id = SYS_MODEM_AS_ID_1;
  cm_call_type_e_type          cm_call_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - */
   cm_call_type = ds3g_get_cm_call_type(call_type);

   if ( ( cm_call_type == CM_CALL_TYPE_PS_DATA )  ||
        ( cm_call_type == CM_CALL_TYPE_PS_DATA_IS707B ) )
   {
      subs_id = ds3g_get_ps_subs_id();
   }
   else if( cm_call_type == CM_CALL_TYPE_CS_DATA )
   {
      subs_id = ds3g_get_cs_subs_id();
   }
   else
   {
     DATA_3GMGR_MSG1(MSG_LEGACY_LOW, " Invalid Call Type :%d ", cm_call_type);

     return FALSE;
   }
   
   return ds3g_msh_initiate_call_ex(call_type,
                                    sys_mode,
                                    dial_string,
                                    digit_mode,
                                    user_info_ptr,
                                    subs_id);
}


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
)
{
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_1;
  
  /*----------------------------------------------------- */
  /* This request will come from C2K MH (SUB1 always) */
 return ds3g_msh_hangup_call_ex(call_id,
                                user_info_ptr, 
                                subs_id );
}

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
  cm_call_id_type        call_id,       /* call id of call to end            */
  void                  *user_info_ptr,  /* points to user supplied info      */
  sys_modem_as_id_e_type subs_id
)
{
  return ds3g_msh_hangup_call_ex2(call_id,user_info_ptr, subs_id, FALSE);
} /* ds3g_msh_hangup_call() */

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
                                              PDP can be locally 
                                              deactivated */
)
{
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr = NULL;
  cm_end_params_s_type      end_params;
  boolean                   delay_disconnect;
  uint32                    call_inst = DS3GI_INVALID_CALL_INSTANCE;
  boolean                   ret_val = DS3G_FAILURE;
  boolean                   cm_ret_val = FALSE;
  cm_client_id_type         ds3g_cm_client_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - -*/

  if(!(IS_IN_DS_TASK()))
  {
    DATA_ERR_FATAL("Not executing in ds task");
    return FALSE;
  }
  call_inst = ds3gi_find_call_instance(call_id);
  if (call_inst >= DS3GI_MAX_NUM_CALLS)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Trying to end non-existent call id %d",call_id);
    return(ret_val);
  }

  if(ds3gi_call_info[call_inst].call_state == DS3G_END_CALL_STATE)
  {
    DATA_MSG0(MSG_LEGACY_HIGH,"A call_end is already pending. Return Success");
    return DS3G_SUCCESS;
  } /* if call_state == END_CALL_STATE */
 
  if( ( ds3gi_call_info[call_inst].call_state != DS3G_IDLE_CALL_STATE ) &&
      ( ds3gi_call_info[call_inst].call_state != DS3G_WAIT_FOR_LL_DISCONNECT_STATE ) &&
      ( ds3gi_call_info[call_inst].call_state != DS3G_END_CALL_STATE )
    )
  {
    /*---------------------------------------------------------------------
      Get a pointer to the appropriate handler function table based
      on the current mode and call type.
    ---------------------------------------------------------------------*/
    hdlr_tbl_ptr = 
        ds3gi_get_hdlr_tbl_ptr_for_call_inst(&(ds3gi_call_info[call_inst]));

    if (hdlr_tbl_ptr == NULL)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "hdlr_table_ptr is Null");
      return DS3G_FAILURE;
    }

    /*-----------------------------------------------------------------------
      Check if a handler function table is registered, with a non-null
      user_end_handler.
    -----------------------------------------------------------------------*/

    if( hdlr_tbl_ptr->user_end_handler != NULL )
    {
      /*-------------------------------------------------------------------
        Invoke the mode-specific handler function.                         ›
      -------------------------------------------------------------------*/
      /*----------------------------------------------------------------
        Intialize end_params before invoking mode specific handler.
      ---------------------------------------------------------------*/
      memset((void*)&end_params, 
             0, 
             sizeof(cm_end_params_s_type));
      hdlr_tbl_ptr->user_end_handler( ds3gi_call_info[call_inst].call_id,
                                      &end_params,
                                      &delay_disconnect,
                                      user_info_ptr,
                                      subs_id
                                    );

      if (delay_disconnect == TRUE)
      {
        ds3gi_call_info[call_inst].call_state = 
                                         DS3G_WAIT_FOR_LL_DISCONNECT_STATE;
        DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Delaying user disconnect");
      }
      else
      {
        ds3g_cm_client_id = ds3g_get_cm_client_id();
        /*-----------------------------------------------------------------
          Set the call state to indicate that we are waiting for the call
          to be ended.
        -----------------------------------------------------------------*/
        ds3gi_call_info[call_inst].call_state = DS3G_END_CALL_STATE;

        if (local_disconnect == TRUE)
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Sending Local Disconnect");
          end_params.end_params.gw_ps_end.cause = SM_LOCAL_PDP_DEACTIVATION;
        }

       /*------------------------------------------------------------------
          Now send a command to Call Manager to end the call.
        -----------------------------------------------------------------*/
        DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Sending End Call command to CM");

        cm_ret_val = cm_mm_call_cmd_exten_end(
                                        ds3gi_cm_call_cmd_end_cb,/* callback*/
                                        (void*)call_inst,   /* data block */
                                        ds3g_cm_client_id,   /* client id  */  
                                        1,                   /* # call id's*/
                                        &end_params          /* end parms  */ 
                                        );
        if (cm_ret_val == FALSE)
        {
          DATA_ERR_FATAL("CM call cmd end failed");
          return ret_val;
        }
      }
      ret_val = DS3G_SUCCESS;
    } /* if user_end_handler != NULL */
    else
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "User end handler not registered");
    }
  } /* if call_state != IDLE */
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                    "Can't hangup in Idle or terminating call state");
  }

  return(ret_val);
} /* ds3g_msh_hangup_call_ex2 */



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
boolean ds3g_msh_pdp_modify_req
(
  cm_call_id_type             call_id,
  cm_pdp_modify_params_s_type *modify_params
)
{
  uint32                    call_inst;
  boolean                   ret_val = DS3G_FAILURE;
  boolean                   cm_ret_val = FALSE; 
  cm_client_id_type         ds3g_cm_client_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "Rx'ed CM_PDP_MODIFY_REQ_CMD for call_id %d", call_id);

  /* Lint error handler */
  DATA_MDM_ARG_NOT_CONST(modify_params);

  call_inst = ds3gi_find_call_instance(call_id);
  if (call_inst >= DS3GI_MAX_NUM_CALLS)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Bad call id for pdp_modify_req %d", call_id);
    return(ret_val);
  }
  /*-----------------------------------------------------------------------
    QOS Modify is valid only in the Active call state.
  -----------------------------------------------------------------------*/
  if(ds3gi_call_info[call_inst].call_state == DS3G_CALL_ACTIVE_CALL_STATE )
  {
    ds3g_cm_client_id = ds3g_get_cm_client_id();
    cm_ret_val = cm_mm_call_cmd_pdp_modify(
                                      ds3gi_cm_call_cmd_cb,
                                      NULL,
                                      ds3g_cm_client_id,
                                      ds3gi_call_info[call_inst].call_id,  
                                      modify_params
                                     );
    if (cm_ret_val == FALSE)
    {
      DATA_ERR_FATAL("CM call cmd PDP MODIFY REQUEST failed");
      return(DS3G_FAILURE);
    }
    else
    {
      ret_val = DS3G_SUCCESS;
    }
  }
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Rx'ed MODIFY PDP REQ:Call not active");
  }

  return(ret_val);

} /* ds3g_msh_pdp_modify_req() */

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
)
{
  boolean                 cm_ret_val = FALSE;  
  cm_client_id_type       ds3g_cm_client_id;
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - -*/

  /*------------------------------------------------------------------------
    Send a command to Call Manager to indicate that data is available on 
    the uplink. 
  ------------------------------------------------------------------------*/
  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Sending PS DATA AVAILABLE command to CM");

  ds3g_cm_client_id = ds3g_get_cm_client_id();
#ifdef FEATURE_PS_DORMANT_PWR_SAVE
  cm_ret_val = cm_mm_ss_cmd_ps_data_available_per_subs 
                                             (ds3gi_cm_ss_cmd_cb,
                                              (void*)subs_id,
                                              ds3g_cm_client_id,
                                              subs_id
                                             );
#else
  cm_ret_val = cm_mm_ss_cmd_ps_data_available_per_subs
                                               (ds3gi_cm_call_cmd_cb,
                                                (void*)subs_id,
                                                ds3g_cm_client_id,
                                                subs_id
                                               );
#endif /* FEATURE_PS_DORMANT_PWR_SAVE */

  if (cm_ret_val == FALSE)
  {
    DATA_ERR_FATAL("CM call cmd ps_data_available failed");
    return;
  }
} /* ds3g_msh_ps_data_available_ind() */


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
)
{
  boolean            cm_ret_val = FALSE;
  cm_client_id_type  ds3g_cm_client_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - -*/

  ds3g_cm_client_id = ds3g_get_cm_client_id();
  cm_ret_val = cm_ph_cmd_packet_state(NULL,            /* cb func          */
                                      NULL,            /* data block ptr   */
                                      ds3g_cm_client_id, 
                                      pkt_state);
  if (cm_ret_val == FALSE)
  {
    DATA_ERR_FATAL("CM cmd_pkt_state failed");
    return;
  }
} /* ds3g_msh_set_pkt_state() */

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
)
{
  uint32  call_inst;
  boolean ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_inst = ds3gi_find_call_instance(call_id);

  if (call_inst >= DS3GI_MAX_NUM_CALLS)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Bad call id %d", call_id);
  }
  else if ((new_mode != SYS_SYS_MODE_CDMA)  &&
           (new_mode != SYS_SYS_MODE_WCDMA) &&
           (new_mode != SYS_SYS_MODE_GSM)   &&
           (new_mode != SYS_SYS_MODE_LTE)   &&
           (new_mode != SYS_SYS_MODE_TDS)
          )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,"Bad new mode %d",new_mode);
  }
  else
  {
    ds3gi_call_info[call_inst].call_mode = new_mode;
    ret_val = TRUE;
  }
  
  return(ret_val);
} /* ds3g_msh_set_new_call_mode() */

/*===========================================================================
FUNCTION      DS3G_MSH_GET_CALL_MODE

DESCRIPTION   Fetch the call mode corresponding to the call id.

DEPENDENCIES  None.

RETURN VALUE  Call mode

SIDE EFFECTS  None
===========================================================================*/
sys_sys_mode_e_type ds3g_msh_get_call_mode
(
  cm_call_id_type      call_id       /* call id of call whose mode changed*/
)
{
  uint32  call_inst = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_inst = ds3gi_find_call_instance(call_id);

  if (call_inst >= DS3GI_MAX_NUM_CALLS)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Bad call id %d", call_id);
  }
  else
  {
    return ds3gi_call_info[call_inst].call_mode;
  }
  
  return SYS_SYS_MODE_NONE;
} /* ds3g_msh_set_new_call_mode() */


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
boolean ds3g_msh_go_dormant(void)
{
  boolean            cm_ret_val = DS3G_SUCCESS;
  cm_client_id_type  ds3g_cm_client_id;
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - -*/

  /*------------------------------------------------------------------------
    Send a command to Call Manager to initiate dormancy by sending
    signalling conn rel ind for PS domain
  ------------------------------------------------------------------------*/
  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                  "Sending GO_DORMANT ps_sig_release command to CM");

  ds3g_cm_client_id = ds3g_get_cm_client_id();
  cm_ret_val = cm_mm_call_cmd_ps_sig_release (ds3gi_cm_call_cmd_cb,
                                              NULL,
                                              ds3g_cm_client_id
                                             );
  if (cm_ret_val == FALSE)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,"CM call cmd ps_sig_release failed");
    /* vrk: check if need to ASSERT */
    return DS3G_FAILURE; 
  }

  return cm_ret_val;
} /* ds3g_msh_go_dormant() */
#endif /* FEATURE_UE_INITIATED_DORMANCY */

/*===========================================================================
FUNCTION      DS3G_GET_CM_CALL_TYPE

DESCRIPTION   This function convert DS_call_type to CM_call_type

DEPENDENCIES  None.

RETURN VALUE  CM_CALL_TYPE

SIDE EFFECTS  None.
===========================================================================*/
cm_call_type_e_type ds3g_get_cm_call_type
(
  ds_call_type_e_type ds_call_type
)
{
  cm_call_type_e_type cm_type = CM_CALL_TYPE_NONE;

  switch(ds_call_type)
  {
    case DS_CALL_TYPE_CKT:
      cm_type = CM_CALL_TYPE_CS_DATA;
      break;
    case DS_CALL_TYPE_PKT:
      cm_type = CM_CALL_TYPE_PS_DATA;
      break;
    case DS_CALL_TYPE_PKT_IS707B:
      cm_type = CM_CALL_TYPE_PS_DATA_IS707B;
      break;
    default:
      break;
  }
  return cm_type;

}/* ds3g_get_cm_call_type */

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
)
{
  boolean  result = DS3G_SUCCESS;

  DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,"call type: %d, call mode: %d, call id: %d",
                  (int)call_type, (int)sys_mode, cm_call_id);
  switch(sys_mode)
  {
    case SYS_SYS_MODE_GSM:
    case SYS_SYS_MODE_WCDMA:
    case SYS_SYS_MODE_TDS:
      if(call_type == CM_CALL_TYPE_CS_DATA)
      {
#if ( defined( FEATURE_DATA_GCSD ) || defined( FEATURE_DATA_WCDMA_CS ) )
        result = ds_ucsd_is_atcop_call(cm_call_id);
#else
        DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,"UMTS CSD is not supported ");
        result =  DS3G_FAILURE;
#endif /* ( ( FEATURE_DATA_GCSD ) || ( FEATURE_DATA_WCDMA_CS ) ) */
      }
      else if (call_type == CM_CALL_TYPE_PS_DATA)
      {
#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
        result = dsumts_rmsm_is_dun_call(cm_call_id);
#else
        DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,"UMTS PS is not supported ");
        result = DS3G_FAILURE;
#endif /* defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) */
      }
      else
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_LOW,"Invalid call type");
        result = DS3G_FAILURE;
      }
      break;
    case SYS_SYS_MODE_LTE:
    case SYS_SYS_MODE_CDMA:
    case SYS_SYS_MODE_HDR:
      result = DS3G_FAILURE;
      break;
    default:
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,"Invalid Sys mode");
      result = DS3G_FAILURE;
  }
  return result;
}/* ds3g_msh_is_atcop_call */

/*===========================================================================
FUNCTION      DS3G_MSH_CLEANUP_NONEPC_CALL

DESCRIPTION   This function requests Call Manager to cleanup the NonEPC PPP 
              session with network (Applicable only for Non-SV devices)

DEPENDENCIES  None.

RETURN VALUE  TRUE - If Cmd is accepted by CM 
              FALSE - Otherwise 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3g_msh_cleanup_nonepc_call(void)
{
  cm_client_id_type  ds3g_cm_client_id;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG0(MSG_LEGACY_MED, "Calling cm_ac_cmd_1xPPP_nw_cleanup()");

  ds3g_cm_client_id = ds3g_get_cm_client_id();
  return cm_ac_cmd_PPP_nw_cleanup(ds3gi_cm_ac_cmd_cb, ds3gi_cm_ac_state_cb,
                                  NULL, ds3g_cm_client_id);
} /* ds3g_msh_cleanup_nonepc_call */


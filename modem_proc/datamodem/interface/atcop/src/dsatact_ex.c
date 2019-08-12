/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                ( C O M M O N   A C T I O N  
                  C O M M A N D S )

                P R O C E S S I N G

GENERAL DESCRIPTION
  This module executes the AT commands. It mainly executes the common 
  (across all modes) action commands.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001 - 2014 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatact_ex.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   tk      Initial revision (created file for Dynamic ATCoP).

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>


#include <stdio.h>
#include <string.h>

#include "dsati.h"
#include "dsatact.h"
#include "dsatctab.h"
#include "dsatdl.h"
#include "dsatparm.h"
#include "dsatvoice.h"
#include "dsatcmif.h"
#include "ds_cap.h" /* ITC_SPEECH */
#include "ds3gmgr.h"
#include "dstask.h"
#include "dstaski.h"
#include "err.h"
#include "mobile.h"
#include "msg.h"
#include "ds3gsiolib_ex.h"

#ifdef FEATURE_DSAT_ETSI_MODE
#include "dsatetsime.h"
#include "dsatetsicall.h"
#include "dsatetsicmif.h"
#include "dsatetsictab.h"

#ifdef FEATURE_DSAT_ETSI_DATA
#include "dsumts_rmsm.h"
#include "dsatetsipkt.h"
#endif /* FEATURE_DSAT_ETSI_DATA*/
#endif /* FEATURE_DSAT_ETSI_MODE */

#if defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS) 
#include "dsucsdhdlr.h"
#endif /* defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS) */

#ifdef FEATURE_DATA_IS707
#include "dsat707util.h"
#include "dsat707extctab.h"
#include "ds707_rmsm.h"
#include "ds707_async_mgr.h"
#include "snm.h"
#include "cai.h"
#include "ds707_roaming.h"
#include "ds707_pkt_mgr.h"
#endif /* FEATURE_DATA_IS707 */

#ifdef FEATURE_DATA_MUX
#include "ds_mux_ext_defs.h"
#endif /* FEATURE_DATA_MUX */
#include "ds3gdsdif.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/
/*-------------------------------------------------------------------------
            Constants:
-------------------------------------------------------------------------*/

/*-------------------------------------
  Local Variables
---------------------------------------*/

/*-------------------------------------
  Regional Variables
---------------------------------------*/
/* Regional variable that is used to retain the */
/* call back function pointers for ATD, ATA     */
/* ATH,+CGANS and abort handlers for ATA,       */
/* ATD and +CGANS                               */
dsat_dial_string_type dial_string_handler = {0};

#ifdef FEATURE_DATA_MUX
dsat_num_item_type dsat_port_open_count = 0;
rex_timer_type dsat_rdm_open_timer;
#endif /* FEATURE_DATA_MUX */
/*-------------------------------------------------------------------------
    Protypes for local functions:
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
            Function Definitions:
-------------------------------------------------------------------------*/

#ifdef FEATURE_DATA_MUX

/*===========================================================================

FUNCTION DSAT_CMUX_HANDLER

DESCRIPTION
Handler function which initializes MUX module.

DEPENDENCIES
  Physical port passed on to MUX should not be in open state.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_cmux_handler(void)
{
  DSAT_DL_CHECK_SYMBOL_ADDR(dsatdl_vtable.dsatact_cmux_handler_fp);

  dsatdl_vtable.dsatact_cmux_handler_fp();

  return;
}/* dsat_cmux_handler */

/*===========================================================================

FUNCTION DSATACT_RDM_NOTIFY_CB

DESCRIPTION
  RDM notify call back on closure of the physical port.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Adds command in DS command buffer

===========================================================================*/

LOCAL void dsatact_rdm_notify_cb
(
  rdm_assign_status_type status,
  rdm_service_enum_type service,
  rdm_device_enum_type device
)
{
  
  ds_cmd_type            *cmd_buf  = NULL;
  ds_at_rdm_notify_info_type *rdm_info = NULL;

  cmd_buf = dsat_get_cmd_buf(sizeof(ds_at_rdm_notify_info_type), FALSE);
  rdm_info = cmd_ptr->cmd_payload_ptr;

  /* send the message to ATCOP */
  cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_RDM_NOTIFY_CMD;
  rdm_info->status  = status;
  rdm_info->service = service;
  rdm_info->device  = device;
  ds_put_cmd(cmd_buf);
}/* dsatact_rdm_notify_cb */


/*===========================================================================

FUNCTION DSATACT_RDM_NOTIFY_HANDLER

DESCRIPTION
  RDM notify Handler on a physical port closure.
  On RDM falure to assign the port, we set a timer of 500ms and retry. We do a maximum of 3 retries to see if
  we can open the port back.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_rdm_notify_handler
(
  ds_cmd_type  * cmd_ptr   /* DS Command pointer		   */
)
{
#define MAX_PORT_RETRIES 3
  dsat_timer_s_type *t_ptr = NULL;
  ds_at_rdm_notify_info_type *rdm_info = dsat_get_cmd_payload_ptr(cmd_ptr);

  if(rdm_info->status == RDM_DONE_S)
  {
    DS_AT_MSG2_MED("Assigned %d port to %d service",rdm_info->device,rdm_info->service);
  }
  else
  {
    DS_AT_MSG1_MED("Assign port Failed with %d Status",rdm_info->status);

    t_ptr = &dsatutil_timer_table[DSAT_TIMER_RDM_OPEN_TIMEOUT];
    if(t_ptr != NULL)
    {
      /* Number of retires are limited to 3 */
      if(*(dsat_num_item_type *)(t_ptr->user_data) > MAX_PORT_RETRIES)
      {
        DS_AT_MSG0_ERROR("MAX retries reached");
        return DSAT_ASYNC_EVENT;
      }
      else
      {
       /* Increment the count to keep a track of number of retries made */
        *(dsat_num_item_type *)(t_ptr->user_data) += 1;
      }
      (void) rex_set_timer( t_ptr->timer_ptr, t_ptr->duration);
      t_ptr->is_running = TRUE;
    }
  }
  return DSAT_ASYNC_EVENT;
}/* dsatact_rdm_notify_handler */

/*===========================================================================

FUNCTION DSAT_INIT_PORT

DESCRIPTION
This function initializes the DATA_SRVC port to the one stored in EFS.

DEPENDENCIES
  EFS @ /atcop_persistent.txt 

RETURN VALUE
  None

SIDE EFFECTS
  In case, the EFS @ atcop_persistent.txt is absent we may never be able to re-open 

===========================================================================*/
void dsat_init_port(void)
{
  
  ds3g_siolib_port_e_type cmux_port = DS3G_SIOLIB_PORT_NONE;
  int fd;
  struct fs_stat temp_buf;

  DS_AT_MSG0_MED("dsat_init_port");

  if(efs_stat("atcop_persistent.txt",&temp_buf) != -1)
  {
    fd = efs_open("atcop_persistent.txt", O_RDONLY | O_TRUNC, S_IRUSR | S_IWUSR);
  
    if (fd < 0)
    {
      DS_AT_MSG1_ERROR("efs_open: errcode %d", efs_errno);
    }
    else
    {
      efs_read(fd,&cmux_port, sizeof(cmux_port));
      if((cmux_port > DS3G_SIOLIB_PORT_NONE) &&(cmux_port < DS3G_SIOLIB_PORT_MAX))
      {
        rdm_assign_port(RDM_DATA_SRVC,ds3g_siolib_convert_ds_to_rdm_port(cmux_port),dsatact_rdm_notify_cb);
      }
  
      efs_close(fd);
    }
  }
}/* dsat_init_port */
/*===========================================================================

FUNCTION  DSATACT_RDM_TIMER_EXPIRED_HANDLER 

DESCRIPTION
  This function is the handler function for the RDM timer expired cmd.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_EVENT

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_rdm_timer_expired_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  if(rdm_get_device(RDM_DATA_SRVC) == RDM_NULL_DEV)
  {
    DS_AT_MSG0_MED("Still a NULL Device for DATA_SRVC");
    dsat_init_port();
  }

  return DSAT_ASYNC_EVENT;
}/* dsatact_rdm_timer_expired_handler */
#endif /* FEATURE_DATA_MUX */


/*===========================================================================

FUNCTION DSAT_REGISTER_HANDLERS

DESCRIPTION
  This function is used to register a call back functions.
  It is used to register call back funtions for ATA and ATH.

  The call back handler can be NULL. If it is not an incoming call, 
  then ATA handler will be NULL.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK :  if successfully registered.
  DSAT_ERROR: if dsat_cgauto_val == 2(only in ETSI mode)
  
SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsat_register_handlers
(
  dsat_incom_answer_cb_type  call_answer_cb, /* Answer call back */
  dsat_call_hangup_cb_type   call_hangup_cb  /* Hangup call back */
)
{
  dsat_result_enum_type result = DSAT_OK;

#ifdef FEATURE_DATA_TE_MT_PDP
  dsati_mode_e_type             current_mode;
  current_mode = dsatcmdp_get_current_mode();
  
  /* +CGAUTO val setting is not applicable for 1x data calls. 
     Only MT CS data calls should not be answered if +CGAUTO == 2.
     Allow MO CSD calls. For MO CSD calls answer call back will be NULL.
  */
  if( ( IS_ETSI_MODE(current_mode) ) 
      &&((dsat_num_item_type)dsatutil_get_val( DSATETSI_EXT_CGAUTO_IDX,0,0,
                          NUM_TYPE) == DSAT_CGAUTO_MDM_COMPAT_PKT_DMN_ONLY ) 
      &&( NULL != call_answer_cb ) )
  {
    DS_AT_MSG0_HIGH("MT CSD calls cannot be manually answered when +CGAUTO=2");
    result = DSAT_ERROR;
  }
#endif /* FEATURE_DATA_TE_MT_PDP */

  /* ATA Handler */
  dial_string_handler.answer_cb = call_answer_cb; 
  /* ATH Handler */
  dial_string_handler.hangup_cb = call_hangup_cb;

  return result ;
} /* dsat_register_handlers */

#ifdef FEATURE_DATA_TE_MT_PDP

/*===========================================================================

FUNCTION DSAT_PDP_REGISTER_HANDLERS

DESCRIPTION
  This function is used to register a call back functions.
  It is used to register call back funtions for ATA ,ATH and
  +CGANS for MT PDP calls. The call back handlers can be NULL.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK: if registered successfully
  DSAT_ERROR: if trying to register when +CGAUTO=1

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsat_register_pdp_handlers
(
  dsat_pdp_incom_answer_cb_type  pdp_call_answer_cb, /* Answer call back */
  dsat_call_hangup_cb_type   call_hangup_cb  /* Hangup call back */
)
{
  dsat_result_enum_type result = DSAT_OK;

  /* Incoming MT PDP register handlers should not be honoured
     when +CGAUTO=1 */
  if ( (dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CGAUTO_IDX,
                                 0,0,NUM_TYPE) == DSAT_CGAUTO_AUTO_ANS_ON )
  {
    DS_AT_MSG0_HIGH("ATA/ATH handlers cannot be registered as +CGAUTO=1");
    return DSAT_ERROR;
  }

  /* ATA Handler and +CGANS handler */
  dial_string_handler.pdp_answer_cb = pdp_call_answer_cb; 
  /* ATH Handler */
  dial_string_handler.pdp_hangup_cb = call_hangup_cb;

  return result;
} /* dsat_register_pdp_handlers */

/*===========================================================================

FUNCTION DSAT_DEREGISTER_PDP_HANDLERS

DESCRIPTION
  This function is used to deregister the MT PDP call back functions.
  It resets all the registered MT PDP handlers to NULL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_deregister_pdp_handlers ( void )
{
  /* +CGANS / ATA Handler for MT PDP */
  dial_string_handler.pdp_answer_cb = NULL;
  /* +CGANS / ATH handler for MT PDP */
  dial_string_handler.pdp_hangup_cb = NULL;
  return;
} /* dsat_deregister_pdp_handlers */

#endif /* FEATURE_DATA_TE_MT_PDP */

/*===========================================================================

FUNCTION DSAT_DEREGISTER_HANDLERS

DESCRIPTION
  This function is used to deregister the call back functions.
  It resets all the registered handlers to NULL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_deregister_handlers ( void )
{
  /* ATA Handler */
  dial_string_handler.answer_cb = NULL; 
  /* ATH Handler */
  dial_string_handler.hangup_cb = NULL;
  return;
} /* dsat_deregister_handlers */


/*===========================================================================

FUNCTION DSAT_REG_ATZ_CB

DESCRIPTION
  This function is used to register a call back function for ATZ.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_reg_atz_cb
(
  dsat_atz_cb_type atz_cb  /* atz call back */
)
{
  dial_string_handler.atz_cb = atz_cb;
  return;
} /* dsat_reg_atz_cb */



/*===========================================================================

FUNCTION DSAT_INIT_CB_HANDLERS

DESCRIPTION
  Initialize all the registered call handlers to NULL
  Called from dsat_init() after every reset.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_init_cb_handlers(void)
{
  /* Reset all the CB handlers to NULL */
  memset ( &dial_string_handler, 0, sizeof(dsat_dial_string_type) );

  return;
}



/*===========================================================================

FUNCTION DSAT_PROC_DIAL_STR

DESCRIPTION
  Parses a dial string, looking for allowed non-digits, and
  illegal non-digits.  Digits, and allowed non-digits are copied
  to the output.  Illegal characters terminate processing.  All
  other characters are ignore.  Upper and lower case are treated
  as being the same.

  Since the output will be no longer than the input, the output
  string may be the same as the input string.

DEPENDENCIES
  None

RETURN VALUE
  Returns enum value indicating dial string content or error

SIDE EFFECTS
  None

===========================================================================*/
dsat_dial_val_e_type dsat_proc_dial_str
(
  const byte * in_ptr,          /*  Input string, null terminated   */
  byte * out_ptr                /*  Output string, null terminated  */
)
{
  dsat_dial_val_e_type result = DSAT_DIAL_EMPTY;

  if (DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatact_proc_dial_str_fp))
  {
    result = dsatdl_vtable.dsatact_proc_dial_str_fp(in_ptr, out_ptr);
  }

  return result;
} /* dsat_proc_dial_str () */

/*===========================================================================

FUNCTION DSATACT_TRACK_SEQUENCE_NUMBER

DESCRIPTION
  Track the sequence number (as described in 3GPP TS 22.030 section 6.5.5.1) 
  for this call ID and event: Set in call sequence number indexed array 
  dsat_seqnum_callid[] on call incoming or connected event and clear on call 
  end event.
  3GPP TS 22.030 section 6.5.5.1 reads - "X" is the numbering (starting with 1)
  of the call given by the sequence of setting up or receiving the calls 
  (active, held or waiting) as seen by the served subscriber. Calls hold their
  number until they are released. New calls take the lowest available number.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatact_track_sequence_number
(
  sys_modem_as_id_e_type subs_id,
  cm_call_id_type        call_id,
  cm_call_event_e_type   call_event,
  cm_call_type_e_type    call_type
)
{
  dsatetsicall_call_state_da_info  *call_da_val = NULL;
  /* Sequence numbers internal to this function are 
     really 22.030 sequence numbers - 1 */
  int index, first_avail_seqnum, callid_seqnum;
  DS_AT_MSG3_HIGH("In track_sequence_number %d call_id %d call_evt %d call_type",
                                  call_id,call_event,call_type);
  call_da_val = dsat_get_base_addr_per_subs(DSAT_CALL_DA_VALS, subs_id, TRUE);
  if (NULL == call_da_val)
  {
    return;
  }
  /* Search through sequence number indexed array... */
  for ( index = 0, first_avail_seqnum = callid_seqnum = CALL_SEQNUM_INVALID; 
        index < CM_CALL_ID_MAX; 
        index++ )
  {
    /* When first available sequence number is found... */
    if ( CALL_SEQNUM_INVALID == first_avail_seqnum && 
         CM_CALL_ID_INVALID ==  call_da_val->dsat_seqnum_callid[index].call_id )
    {
      first_avail_seqnum = index;  
    }
    
    /* If call ID found... */
    if ( CALL_SEQNUM_INVALID == callid_seqnum &&
         call_id == call_da_val->dsat_seqnum_callid[index].call_id )
    {
      callid_seqnum = index;
    }
  }

  switch( call_event )
  {
    case CM_CALL_EVENT_ORIG:
    case CM_CALL_EVENT_SETUP_IND:
    case CM_CALL_EVENT_ACT_BEARER_IND:
      /* Should never be existing sequence number for incoming call ID,
         try to clean up */
      if ( CALL_SEQNUM_INVALID != callid_seqnum )
      {
        DS_AT_MSG1_ERROR("Event incoming & already sequence number for call ID %d",
                   call_id);
        call_da_val->dsat_seqnum_callid[callid_seqnum].call_id = CM_CALL_ID_INVALID;
      }
      if ( CALL_SEQNUM_INVALID != first_avail_seqnum )
      {
        call_da_val->dsat_seqnum_callid[first_avail_seqnum].call_id = call_id;
        call_da_val->dsat_seqnum_callid[first_avail_seqnum].call_type = call_type;		
      }
      break;
    case CM_CALL_EVENT_CONNECT:
    case CM_CALL_EVENT_INCOM:
    case CM_CALL_EVENT_RAB_REESTAB_IND:
      /* May find existing sequence number for connecting call ID if previous 
         incoming call, else assign first available */
      if (( CALL_SEQNUM_INVALID == callid_seqnum ) && 
         ( CALL_SEQNUM_INVALID != first_avail_seqnum ))
      {
        call_da_val->dsat_seqnum_callid[first_avail_seqnum].call_id = call_id;
        call_da_val->dsat_seqnum_callid[first_avail_seqnum].call_type = call_type;
      }
      break;
    case CM_CALL_EVENT_END:
    case CM_CALL_EVENT_PDN_CONN_REJ_IND:
    case CM_CALL_EVENT_PDN_CONN_FAIL_IND:
    case CM_CALL_EVENT_RES_ALLOC_REJ_IND:
    case CM_CALL_EVENT_RES_ALLOC_FAIL_IND:
      /* Should find existing sequence number for ending call ID */
      if ( CALL_SEQNUM_INVALID != callid_seqnum )
      {
        call_da_val->dsat_seqnum_callid[callid_seqnum].call_id = CM_CALL_ID_INVALID;
        call_da_val->dsat_seqnum_callid[callid_seqnum].call_type = CM_CALL_TYPE_NONE;
      }
      break;
    default:
      DS_AT_MSG1_ERROR("Unanticipated event %d",call_event);
      break;
  }
}/* dsatact_track_sequence_number */

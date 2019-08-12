
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                        A T   C O M M A N D   

                        ( S M S  C O M M A N D S )

GENERAL DESCRIPTION
  This module contains the command tables and data definitions required
  to define the SMS related AT modem commands.

  This file contains common sms functions across modes.
  
EXTERNALIZED FUNCTIONS

  Public functions exported outside of sms module.

    dsatsms_flush_sms_indications
      Flushes buffer of SMS incoming message indications and sends indications
      to TE. Indications are buffered and sent based on AT parameter setting

    dsatsms_abort_cmd
      Called by At command processor to abort an SMS command. This function
      will send an DS CMD to the AT Command processor indicating that the 
      commaand has been aborted.
    
    dsatsms_send_sms_msg
      This function is used to transmit an SMS message (After SMS message 
      entry mode is entered). It calls the mode-specific functions to send 
      SMS message.

    dsatsms_get_msg_fmt_mode
      Returns the current sms message format mode, for example, pdu or text.

    dsatsmsi_cmd_cb_func
      Process the WMS command call back status.

    dsatsms_abt_handler
      Handler function for aborting a SMS command

    dsatsms_err_handler
      When there is an error in processing a SMS AT command, WMS sends a 
      command to handle the error condition. This function is the handler 
      function for the error case.

    dsatsms_init_sms
      SMS initialization function. This function does the following: 
      Initializes ATCOP as one of the clients to WMS, Registers the WMS 
      message and configuration call back functions and Sets up the message 
      routing configuration.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

   Copyright (c) 2003 - 2015 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatsms.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/18/15   skc     Fixed KW issues
04/15/15   sc      Fixed issue in event handling to use subs_id provided in 
                   event info.
02/10/15   tk      Fixed issues in subscription specific WMS API calls.
10/24/14   tk      ATCoP changes for dynamic memory optimizations.
7/27/14    pg      ATCoP changes for Data Plus Data.
06/27/14   tk      Added support for Dynamic ATCoP.
05/20/14   pg      Fixed +CSCA and +CSMP issue.
04/15/14   sc      Fixed +COPS DSDS issue.
03/11/14   tk      Optimized debug macros usage in ATCoP.
01/24/14   sc      Fixed ATCoP KW errors.
09/13/13   tk      ATCoP changes for LTE and DSDS concurrency solution.
04/24/13   tk      Fixed compilation issue with FEATURE_CDMA_SMS disabled.
04/23/13   tk      Fixed assertion issue in +CPMS command handling.
04/19/13   tk      Fixed issue with both DSDS and thin UI enabled.
03/28/13   tk      Mainlined FEATURE_THIN_UI using new NV-EFS item.
01/04/13   tk      ATCoP changes for DSDX C/G+G with True World Mode Support.
11/30/12   tk      ATCoP changes for Triton DSDA.
08/22/12   sk      Fixed CPMS issue when FEATURE_DSAT_EXTENDED_SMS enabled.
08/22/12   sk      Mainlining Nikel Changes.
05/18/12   tk      Migrated to MSG 2.0 macros
04/11/12   nc      Fixed operating mode checks for SMS related commands for NikeL targets. 
04/11/12   sk      Adding support for forwarding CFUN.
03/29/12   sk      Fixed errors & warnings when FEATURE_DSAT_EXTENDED_CMD is enabled.
02/06/12   nc      Added support for ^SPN and URC Queue.
01/19/12   sk      Feature cleanup.
11/04/11   sb      Fixed +CMGD command.
09/27/11   ht      Included dsut header to support offtarget stubs.
07/03/11   ad      Fixed memory corruption.
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
01/06/11   nc      Updated the subs_id correctly in sms_msg_cb_func.
10/25/10   ad      Init Changes for DSDS. 
10/14/10   ad      Added Debug message in SMS call back function. 
10/06/10   ad      Added Error msg before Assert(0).
10/12/10   bs      Fixed +COPS=? issue.
07/23/10   nc      Added 1x SMS Support for FUSION Architecture.
05/10/10   kk      Added support for ^HC SMS commands. 
02/10/10   kk      Fix to clear up WMS message watermarks.
12/15/09   nc      Featurisation changes for LTE.
09/09/09   bs      Fixed Klocwork criticals.
07/01/09   vg      Replacing deprecated MSG_XXX macro with MSG_SPRINTF_X.
06/19/09   ua      Correcting +CSAS and +CRES commands when MIDS are NULL.
04/29/09   ua      Fixed compiler warnings.
03/11/09   nc      Made 'sms_info' as a global variable.
12/23/08   sa      Replaced q_init with dsm_queue_init for watermark initialization.
12/17/08   dh      Lint fixes
12/12/08   ua      Fixed Off target lint errors.
12/02/08   ss      Off target Lint fix.
11/13/08   nc      Added Support for QCMGW,QCMSS,QCMGL,QCMGF Commands
11/10/08   ua      Allowing processing of tag MO template in 
                   dsatsmsi_memory_status_cfg_event_handler
09/26/08   nc      Fixing the +CMTI Report during Handling of MT SMS
                   and featurization issue  
07/22/08   bs      Fixed Featurization issue.
06/17/08   nc      Added a union sms_msg_mt_info_u to support CDMA mt sms.
05/28/08   nc      Fixed +CPMS command for SR and related issues 
                   in +CMGD and +CMGL.
03/03/08   bs      Fixing CSCA regression issues.
02/15/08   bs      Fixed +CSCA initial query command
01/11/08   ua      Removed SMS retry functionality from ATCOP as WMS is taking
                   care of that. 
11/24/07   ua      Fixed Lint-lows. 
10/31/07   ua      Added support for +CSAS command. 
10/09/07   ss      Correcting the behaviour for GSM+1x targets.
07/12/07   pp      Fixed Lint high errors.
06/01/07   sa      Mainlining SMS C2K changes
05/02/07   ua      Fixed lint medium errors. 
02/15/07   ss      Fixed lint high errors
02/13/07   pkp     Fixed Klocwork (NULL dereference) errors.
02/05/07   ua      Corrected CPMS values to be in sync with NV item at powerup
01/30/07   ss      Replaced banned string API calls
11/02/06   ua      Correcting +CSCA command
02/21/06   sa      Merged changes from Sirius Branch
                   -Get dsm_item from DSM rather than creating on stack so that
                   an attempt to DSM free it doesn't give bad header cookie
                   err fatal.
11/07/05   snb     Changes for increased WMS NV storage.
07/22/05   snb     Respond to SMS_CFG_EVENT_GW_READY event regardless of 
                   currently recorded command mode to avoid race condition 
                   where mode is changing but this event preceded that 
                   notification
05/31/05   sb      Fix for CR 63522 (setting of SMS retry attempts).
05/13/05   sb      Enable direct routing of SMS STATUS-REPORT
05/12/05   sb      Check for GCF test flag before setting retry timer
03/31/05   sb      When accessories are enabled, send the SMS memory full
                   indication to accessories.
04/05/05   ar      Add support for +CIND SMS full indication.
02/18/05   dvp     Added command ^SMGO
02/13/05   dvp     Added support for commands ^SGM[LR]
12/21/04   snb     Only allow processing  and clearing of dsatsmsi_pres_cmd in
                   SMS event handlers when processing related SMS command.
11/18/04   snb     Add MO SMS retry, clear dsatsmsi_pres_cmd on command abort
                   and support additional +CGSMS parms.
10/21/04   snb     Add template support to SMS code.
09/03/04   snb     Add support for +CMMS command.
05/20/04   snb     Correct SMS constant name and lint errors.
02/24/04   snb     Added support for handling ATCoP SMS via signals, flow
                   control of received messages via watermarks.
11/28/03   sb      created the module. CDMA SMS additions

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>
#include <stdio.h>

#if defined(T_WINNT)
#error code not present
#endif /* WINNT */

#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
#include <stdio.h>
#include <string.h>

#include "dsati.h"
#include "dsatsmsi.h"
#include "err.h"
#include "amssassert.h"
#include "queue.h"

#include "dsatsms.h"
#include "dsatctab.h"
#ifdef FEATURE_ETSI_SMS
#include "dsatetsictab.h"
#include "dsatetsime.h"
#include "dsatetsismsi.h"
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_CDMA_SMS
#include "dsat707sms.h"
#include "dsat707smsctab.h"
#endif /* FEATURE_CDMA_SMS */

/*===========================================================================
            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE
  This section contains reginal definitions for variables that are used
  in ATCOP SMS SUB SYSTEM
===========================================================================*/

/*--------------------------------------------------------------------------
  Debug macro for this module.
---------------------------------------------------------------------------*/
#define DEBUG( x )
/*--------------------------------------------------------------------------
  Import variables defined in other files of SMS module
---------------------------------------------------------------------------*/
#ifdef FEATURE_ETSI_SMS
extern dsat_mixed_param_val_type        dsat_csca_prev_val[];
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_ETSI_SMS_TEMPLATE
extern boolean dsatetsismsi_template_present;
#endif /* FEATURE_ETSI_SMS_TEMPLATE */

/*--------------------------------------------------------------------------
  Variables common for SMS module
---------------------------------------------------------------------------*/
#ifdef FEATURE_ETSI_SMS_TEMPLATE
/* Variable to write SCA, PID and DCS to templates */
extern wms_client_message_s_type *template_ptr;
#endif /* FEATURE_ETSI_SMS_TEMPLATE */
/* WMS client (ATCOP) ID */
extern wms_client_id_type dsatsmsi_client_id;

/* Variable to find out what command is being processed currently */
extern dsat_sms_cmd_s_type dsatsmsi_pres_cmd;

/* Variable used in SMS SM */
extern sms_cmd_sm_type dsatsmsi_cmd_sm;

/* Flag for wms readiness */
extern boolean dsatsmsi_wms_ready;

/* For +CPMS or $QCPMS cmd */
/* remembers the mem being reported */
cpms_mem_status_data_s_type dsatsmsi_mem_report = {0};

/* Watermarks and Queues for SMS. */
/* This one holds all other SMS events and does not perform flow control */
extern dsm_watermark_type  dsat_sms_wm;

#ifdef FEATURE_ETSI_SMS_TEMPLATE
boolean dsatsms_template_init_flag = FALSE;
#endif /* FEATURE_ETSI_SMS_TEMPLATE */

/*--------------------------------------------------------------------------
  Local function prototypes
---------------------------------------------------------------------------*/

#ifdef FEATURE_ETSI_SMS_CB
LOCAL void sms_cb_msg_cb_func
(
  wms_bc_mm_event_e_type                event,      /* Event ID              */
  const wms_bc_mm_event_info_s_type    *event_ptr   /* Pointer to Event info */
);

LOCAL sms_event_e_type cb_msg_event_lookup
(
 wms_bc_mm_event_e_type wms_event /* Event ID */
);
#endif /* FEATURE_ETSI_SMS_CB */

#ifdef FEATURE_ETSI_SMS_TEMPLATE
dsat_result_enum_type read_param_from_template
(
  const sms_event_info_s_type  *event_info   /* Event info ptr    */
);
#endif /* FEATURE_ETSI_SMS_TEMPLATE */


/*===========================================================================

FUNCTION DSATSMS_FLUSH_SMS_INDICATIONS

DESCRIPTION
  Flushes buffer of SMS incoming message indications and sends indications
  to TE. Indications are buffered and sent based on AT parameter setting.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
void dsatsms_flush_sms_indications
(
 void
)
{
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_1;
#ifdef FEATURE_ETSI_SMS 

  while (subs_id < DSAT_SYS_MODEM_AS_ID_MAX)
  {
     dsatetsisms_flush_sms_indications(subs_id);
     subs_id++;
  }

#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_CDMA_SMS 
    dsat707sms_flush_sms_indications();
#endif /* FEATURE_CDMA_SMS */

}/* dsatsms_flush_sms_indications */


/*===========================================================================

FUNCTION DSATSMS_ABORT_CMD

DESCRIPTION
  Called by At command processor to abort an SMS command. A command can be
  aborted if user enters ESC key or if the ATCOP runs out of buffers. This 
  function will send an DS CMD to the AT Command processor indicating that
  the commaand has been aborted.

  If status is 0, the function is called when user enters ESC key.
  If status is 1, the function is called when we run out of buffer
                  or other cmd error.
  If status is other number, there is a cms error.  This function can be
                             called when there is error sending sms 
                             messages.

  Event will contain the following result code:
  DSAT_ERROR: if we are out of buffer for the message to send
  DSAT_OK: if user sent ESC character while writing the message
  A CMS error number: if there is a cms error.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  DS CMD is generated.

===========================================================================*/
void dsatsms_abort_cmd
(
 dsati_cms_err_e_type error /* +CMS ERROR Number */
)
{
  dsat_result_enum_type         result_code;
  ds_cmd_type                 * cmd_ptr;
  ds_at_sms_abt_type          *sms_abt_ptr = NULL;

  /* tell atcop that there is a command abort */
  cmd_ptr  = dsat_get_cmd_buf(sizeof(ds_at_sms_abt_type), FALSE);
  sms_abt_ptr = cmd_ptr->cmd_payload_ptr;

  DS_AT_MSG0_HIGH("Aborting SMS command");
  /* check to see what type of abort it is */
  /* CMS_ERROR: Command ERROR              */
  /* CMS_NONE: ESC from user               */
  if(error)
  {
    result_code = DSAT_CMD_ERR_RSP;
    sms_abt_ptr->cms_error = (int)error;

  }
  else
  {
    result_code = DSAT_OK;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_ATCOP_SMS_ABT_CMD;
  sms_abt_ptr->cmd_status = (int16) result_code;
  ds_put_cmd(cmd_ptr);
}/* dsatsms_abort_cmd */


/*===========================================================================

FUNCTION DSATSMS_EXEC_SMS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes a SMS command.

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_CMD_ERR_RSP : if +CMS ERROR is being generated

SIDE EFFECTS

===========================================================================*/
dsat_result_enum_type dsatsms_exec_sms_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;

  if (DSATETSI_EXT_ACT_CMGS_ETSI_IDX == parse_table->cmd_id)
  {
    dsat_curr_cmd_var.proc_func = dsatetsisms_exec_cmgs_cmd;
  }
  else if (DSATETSI_EXT_ACT_CMGW_ETSI_IDX == parse_table->cmd_id)
  {
    dsat_curr_cmd_var.proc_func = dsatetsisms_exec_cmgw_cmd;
  }
  else if (DSATETSI_EXT_CSCA_IDX == parse_table->cmd_id)
  {
    dsat_curr_cmd_var.proc_func = dsatetsisms_exec_csca_cmd;
  }
  else if (DSATETSI_EXT_CSMP_IDX == parse_table->cmd_id)
  {
    dsat_curr_cmd_var.proc_func = dsatetsisms_exec_csmp_cmd;
  }
  else
  {
    return DSAT_ERROR;
  }

#ifdef FEATURE_ETSI_SMS_TEMPLATE
  if( !dsatsmsi_wms_ready )
  {
    /* WMS isn't ready so template hasn't been read yet and can't be written */
    dsatme_set_cme_error(DSAT_CME_SIM_BUSY, res_buff_ptr);
    return DSAT_CMD_ERR_RSP;
  }

  if (FALSE == dsatsms_template_init_flag)
  {
    dsatsms_template_init_flag = TRUE;

    dsatsmsi_pres_cmd.cmd_name = SMS_TEMPLATE;

    /* Get the template from SIM */
    if ( wms_cfg_ms_get_message_list( dsatsmsi_client_id,
                                      dsat_get_current_subs_id(FALSE),
                                      dsatsmsi_cmd_cb_func,
                                      (void *) &dsatsmsi_pres_cmd,
                                      WMS_MEMORY_STORE_SIM,
                                      WMS_TAG_MO_TEMPLATE) == WMS_OK_S )
    {
      return DSAT_ASYNC_CMD;
    }
    else
    {
      /* Failed to get the template from SIM */
      /* Clean SMS state and execute the command */
      DSAT_SMS_STATE_CLEAN();
    }
  }
#endif /* FEATURE_ETSI_SMS_TEMPLATE */

  result = dsat_curr_cmd_var.proc_func(mode, parse_table, tok_ptr, res_buff_ptr);

  return result;
} /* dsatsms_exec_sms_cmd */

/*===========================================================================

FUNCTION DSATSMS_PROCESS_SMS_CMD

DESCRIPTION
  This function processes a SMS command.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_CMD_ERR_RSP : if +CMS ERROR is being generated

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatsms_process_sms_cmd(void)
{
  /* Clean SMS state and execute the command */
  DSAT_SMS_STATE_CLEAN();

  return dsatcmdp_process_curr_cmd();
} /* dsatsms_process_sms_cmd */


/*===========================================================================

FUNCTION DSATSMS_SEND_SMS_MSG

DESCRIPTION
  Central place/function call for sending sms message.
  In case of actual message transmission, it is done in 2 phases. First you 
  get the actual command with all the parameters. Parameters are stored and 
  user is sent a prompt to indicate that message has to be typed. Then the 
  message is transmited. This function is used to transmit the actual message.
  It calls the mode-specific functions to send SMS message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatsms_send_sms_msg
(
 byte  * msg, /* Ptr to the text message that has to be sent */
 int   len    /* Len of the text message that has to be sent */
)
{
  dsati_operating_cmd_mode_type cmd_mode;

  cmd_mode = dsatcmdp_get_operating_cmd_mode();

  DS_AT_MSG3_MED("dsatsms_send_sms_msg, len: %d, msg: %c%c...",len, msg[0], msg[1]);


 #ifdef FEATURE_ETSI_SMS 
  if(dsatcmdp_is_etsi_cmd())
  {
    dsatetsisms_send_sms_msg ( msg, len );
  }
  else
#endif /* FEATURE_ETSI_SMS */
#ifdef FEATURE_CDMA_SMS 
  if(dsatcmdp_is_cdma_cmd())
  {
    dsat707sms_send_sms_msg ( msg, len );
  }
  else
#endif /* FEATURE_CDMA_SMS */
  {
    DS_AT_MSG1_ERROR("We are in wrong mode: %d",cmd_mode);
    return;
  }
  
}/* dsatsms_send_sms_msg */


/*===========================================================================

FUNCTION    DSATSMS_GET_MSG_FMT_MODE

DESCRIPTION
  Returns the current sms message format mode, for example, pdu or text.        

DEPENDENCIES
  None

RETURN VALUE
  Returns a dsatsms_fmt_mode_e_type enum.

SIDE EFFECTS
  None

===========================================================================*/
dsatsms_fmt_mode_e_type dsatsms_get_msg_fmt_mode (void)
{
  dsati_operating_cmd_mode_type cmd_mode;
  dsatsms_fmt_mode_e_type       fmt_mode = DSATSMS_TEXT_MODE;

  cmd_mode = dsatcmdp_get_operating_cmd_mode();

  if (cmd_mode == ETSI_CMD_MODE)
  {
#ifdef FEATURE_ETSI_SMS
    if ((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CMGF_IDX,0,0,NUM_TYPE) == 0)
    {
      fmt_mode = DSATSMS_PDU_MODE;
    }
#endif /* FEATURE_ETSI_SMS */
  }
  else
  {
    ASSERT(cmd_mode == CDMA_CMD_MODE);
  }

  return fmt_mode;
}/* dsatsms_get_msg_fmt_mode */



/*===========================================================================

FUNCTION    DSATSMSI_CMD_ERR_CB_FUNC

DESCRIPTION
  Process the WMS command call back status.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
void dsatsmsi_cmd_err_cb_func
(
  wms_cmd_id_e_type     cmd,   /* Cmd ID            */
  wms_cmd_err_e_type    status /* Command Status    */
)
{
  ds_cmd_type        *cmd_buf  = NULL;
  ds_at_sms_cmd_type *sms_cmd_ptr = NULL;

  cmd_buf  = dsat_get_cmd_buf(sizeof(ds_at_sms_cmd_type), FALSE);
  sms_cmd_ptr = cmd_buf->cmd_payload_ptr;

  /* send the message to ATCOP */
  cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_SMS_ERR_CMD;
  sms_cmd_ptr->cmd      = cmd;
  sms_cmd_ptr->cmd_info = status;
  ds_put_cmd(cmd_buf);

  return;
}/* dsatsmsi_cmd_err_cb_func */ 

/*===========================================================================

FUNCTION  DSATSMS_ABT_HANDLER

DESCRIPTION
  Handler function for aborting a SMS command

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  DSAT_ERROR: if we are out of buffer for the message to send
  DSAT_OK: if user sent ESC character while writing the message

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatsms_abt_handler
(
 ds_cmd_type * cmd_ptr     /* DS Command pointer */
)
{
  dsm_item_type               * res_buff = NULL;
  ds_at_sms_abt_type *sms_abt_ptr = dsat_get_cmd_payload_ptr(cmd_ptr);

  DEBUG(
        DS_AT_MSG1_MED("dsatsms_abt_handler, error:%d", 
                sms_abt_ptr->cms_error);
        );

  DSAT_SMS_STATE_CLEAN();

  /* Check to see if we need to send +CMSS ERROR */
  if ( (dsat_result_enum_type) sms_abt_ptr->cmd_status == 
       DSAT_CMD_ERR_RSP )
  {
    res_buff = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
    res_buff->used = (byte) dsatsmsi_cms_error(
                                  res_buff, 
                                  sms_abt_ptr->cms_error
                                  );
    /* send the response to the user */
    dsatcmdp_handle_async_cmd_rsp(res_buff, DSAT_COMPLETE_RSP);
    return DSAT_CMD_ERR_RSP;
  } else if(DSAT_ERROR ==
            (dsat_result_enum_type)sms_abt_ptr->cmd_status) {
    return DSAT_ERROR;
  } else
      {
    /* just return the result code */
    return (dsat_result_enum_type) sms_abt_ptr->cmd_status;
  }

}/* dsatsms_abt_handler */

/*===========================================================================

FUNCTION  DSATSMS_ERR_HANDLER

DESCRIPTION
  When there is an error in processing a SMS AT command, WMS sends a command
  to handle the error condition. This function is the handler function for the
  error case.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatsms_err_handler
(
 ds_cmd_type * cmd_ptr     /* DS Command pointer */
)
{
  dsat_result_enum_type result      = DSAT_OK;
  ds_at_sms_cmd_type *sms_cmd_ptr = dsat_get_cmd_payload_ptr(cmd_ptr);

#ifdef FEATURE_DSAT_ETSI_MODE
  dsati_mode_e_type current_mode = dsatcmdp_get_current_mode();
#endif /* FEATURE_DSAT_ETSI_MODE */
#ifdef FEATURE_DSAT_EXTENDED_SMS
  boolean hcmgsf_event_occurred = FALSE;
#endif /* FEATURE_DSAT_EXTENDED_SMS */

  if( dsatsmsi_pres_cmd.cmd_name == SMS_NONE )
  {
    result = DSAT_ASYNC_EVENT;
  }
#ifdef FEATURE_ETSI_SMS_TEMPLATE
  else if (dsatsmsi_pres_cmd.cmd_name == SMS_TEMPLATE)
  {
    return dsatsms_process_sms_cmd();
  }
#endif /* FEATURE_ETSI_SMS_TEMPLATE */
#ifndef FEATURE_DSAT_EXTENDED_SMS
  else if(((dsatsmsi_pres_cmd.cmd_name == SMS_CMGD) || 
            (dsatsmsi_pres_cmd.cmd_name == SMS_CMGR))&&
           ( sms_cmd_ptr->cmd_info == WMS_CMD_ERR_MSG_EMPTY_MESSAGE ) 
    )
  {
    /* We got reading or deleting an empty slot */
    /* Just return OK */
    DSAT_SMS_STATE_CLEAN_ALL();
    result = DSAT_OK;
  }
#endif /* #ifndef FEATURE_DSAT_EXTENDED_SMS */
  else if ( (( dsatsmsi_pres_cmd.cmd_name==SMS_CSCB )|| 
            ( dsatsmsi_pres_cmd.cmd_name == SMS_CSAS_CSCB )||
            ( dsatsmsi_pres_cmd.cmd_name == SMS_CRES_CSCB )) &&
            ( sms_cmd_ptr->cmd_info == WMS_CMD_ERR_BC_BAD_SRV_ID ) )
  {
    /* We got reading or deleting an empty slot */
    /* Just return OK */
    DSAT_SMS_STATE_CLEAN_ALL();
    result = DSAT_OK;
  }
  else
  {
    dsm_item_type        *res_buff;
    dsati_cms_err_e_type  cms_err = DSAT_CMS_UNKNOWN_ERR;

    res_buff = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);

    /* We got an error for WMS */
    if(
       /* User was trying to send a message of non-delivery type,
          not WMS_TAG_MO_NOT_SENT or WMS_TAG_MO_SENT */
       ( ( dsatsmsi_pres_cmd.cmd_name==SMS_CMSS )&& 
          ( sms_cmd_ptr->cmd_info == WMS_CMD_ERR_MSG_TAG) )
       || 
        /* Process WMS_CMD_ERR_MSG_SEND_MODE error since the 
           message to be sent was too long */
        ( ( (dsatsmsi_pres_cmd.cmd_name == SMS_CMGS) || 
            (dsatsmsi_pres_cmd.cmd_name == SMS_CMSS) ) &&
          (( sms_cmd_ptr->cmd_info == WMS_CMD_ERR_MSG_SEND_MODE )
#ifdef FEATURE_DSAT_EXTENDED_SMS
       || (sms_cmd_ptr->cmd_info == WMS_CMD_ERR_MSG_LPM)
#endif /* FEATURE_DSAT_EXTENDED_SMS */
        )) 
      )
    {
#ifdef FEATURE_ETSI_SMS
      if ( IS_ETSI_MODE(current_mode) )
      {
        if((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CMGF_IDX,0,0,NUM_TYPE))
        {
          cms_err = DSAT_CMS_INVALID_TXT_PARAM;
        }
        else
        {
          cms_err = DSAT_CMS_INVALID_PDU_PARAM;
        }
      }
#endif /* FEATURE_ETSI_SMS */
#ifdef FEATURE_DSAT_EXTENDED_SMS
      if ( (TRUE == dsatsmsi_pres_cmd.is_hc_cmd )&& 
           (dsatsmsi_pres_cmd.cmd_name == SMS_CMGS) && 
           (sms_cmd_ptr->cmd_info == WMS_CMD_ERR_MSG_LPM))
      {
        res_buff->used = (uint16)snprintf((char*)res_buff->data_ptr,
                                                res_buff->size,
                                                "^HCMGSF: %d",
                                              WMS_TL_OTHER_NETWORK_PROBLEM_S);
        hcmgsf_event_occurred = TRUE;
      }
#endif /* FEATURE_DSAT_EXTENDED_SMS */

    }
    else if(((dsatsmsi_pres_cmd.cmd_name == SMS_CSCA) || (dsatsmsi_pres_cmd.cmd_name == SMS_CSMP))
        && (sms_cmd_ptr->cmd == WMS_CMD_MSG_READ_TEMPLATE))
    {

      result = dsatetsisms_process_template_read(NULL);
      return result;
    }
    else
    {
      /* Map wms_cmd_err_e_type enum values to +CMS ERRORs */
      switch (sms_cmd_ptr->cmd_info) {
        case WMS_CMD_ERR_MSG_MEMORY_STORE:
              cms_err = DSAT_CMS_MEM_FAILURE;
          break;
          
        case WMS_CMD_ERR_MSG_SIM_WRITE:
        case WMS_CMD_ERR_MSG_SIM_READ:
        case WMS_CMD_ERR_MSG_SIM_DELETE:
#ifdef FEATURE_CDMA_SMS
        case WMS_CMD_ERR_MSG_RUIM_WRITE:
        case WMS_CMD_ERR_MSG_RUIM_READ:
        case WMS_CMD_ERR_MSG_RUIM_DELETE:
#endif /* FEATURE_CDMA_SMS */
          cms_err = DSAT_CMS_SIM_FAILURE;
            break;

#ifdef FEATURE_DSAT_EXTENDED_SMS
        case WMS_CMD_ERR_MSG_NV_WRITE:
        case WMS_CMD_ERR_MSG_NV_READ:
        case WMS_CMD_ERR_MSG_NV_DELETE:
          cms_err = DSAT_CMS_ME_FAILURE;
          break;
#endif /* FEATURE_DSAT_EXTENDED_SMS */

        case WMS_CMD_ERR_MSG_INDEX: 
        case WMS_CMD_ERR_MSG_EMPTY_MESSAGE:
          cms_err = DSAT_CMS_INVALID_INDEX;
          break;
          
        case WMS_CMD_ERR_MSG_MEMORY_FULL:
        case WMS_CMD_ERR_MSG_RAM_FULL:
        case WMS_CMD_ERR_MSG_SIM_FULL:
        case WMS_CMD_ERR_MSG_NV_FULL:
          cms_err = DSAT_CMS_MEM_FULL;
          break;

        case WMS_CMD_ERR_MSG_TPDU_TYPE:
          cms_err = DSAT_CMS_OP_NOT_ALLOWED;
          break;

        case WMS_CMD_ERR_MSG_TRANSACTION_ID:
          cms_err = DSAT_CMS_CNMA_NOT_EXP;
          break;

#ifdef FEATURE_DSAT_EXTENDED_SMS
        case WMS_CMD_ERR_UNSUPPORTED:
#endif /* FEATURE_DSAT_EXTENDED_SMS */
        case WMS_CMD_ERR_BC_DUPLICATE_SRV:
        case WMS_CMD_ERR_BC_CANNOT_ACTIVATE:
        case WMS_CMD_ERR_BC_CANNOT_ACTIVATE_ALL:
          cms_err = DSAT_CMS_OP_NOT_ALLOWED;
          break;
        case WMS_CMD_ERR_MSG_BLOCKED:
#ifdef FEATURE_DSAT_GOBI_MAINLINE
          cms_err = DSAT_CMS_MSG_BLOCKED;
#else
          cms_err = DSAT_CMS_OP_NOT_ALLOWED;
#endif /* FEATURE_DSAT_GOBI_MAINLINE */
          break;
#ifdef FEATURE_DSAT_EXTENDED_SMS
        case WMS_CMD_ERR_BUSY:
          if (dsatsmsi_pres_cmd.cmd_name == SMS_CPMS) 
          {
            DSATUTIL_SET_VAL(DSAT707_SMS_QCPMS_IDX,0,CPMS_MEM3,0,dsatsmsi_mem_report.prev_mem[CPMS_MEM3],NUM_TYPE)

            DSATUTIL_SET_VAL(DSAT707_SMS_QCPMS_IDX,0,CPMS_MEM2,0,dsatsmsi_mem_report.prev_mem[CPMS_MEM2],NUM_TYPE)

            DSATUTIL_SET_VAL(DSAT707_SMS_QCPMS_IDX,0,CPMS_MEM1,0,dsatsmsi_mem_report.prev_mem[CPMS_MEM1],NUM_TYPE)
            cms_err = DSAT_CMS_SIM_BUSY;
          }
          break;
#endif /* FEATURE_DSAT_EXTENDED_SMS */
        default:
              cms_err = DSAT_CMS_UNKNOWN_ERR;
          break;
      }
    }
#ifdef FEATURE_DSAT_EXTENDED_SMS
    if (hcmgsf_event_occurred == FALSE) 
#endif /* FEATURE_DSAT_EXTENDED_SMS */
    {
      res_buff->used = (uint16)dsatsmsi_cms_error ( res_buff, (int)cms_err );
    }
    result = DSAT_CMD_ERR_RSP;

#if defined(FEATURE_ETSI_SMS) && defined(FEATURE_ETSI_SMS_TEMPLATE)
    /* Restore CSCA previous values */
    if( dsatsmsi_pres_cmd.cmd_name == SMS_CSCA)
    {
      dsat_string_item_type * str_item1;
      dsat_string_item_type * str_item2;
      str_item1 = (uint8 *)dsatutil_get_val(DSATETSI_EXT_CSCA_IDX,dsat_get_current_subs_id(FALSE),0,MIX_STR_TYPE);
      str_item2 = (uint8 *)dsatutil_get_val(DSATETSI_EXT_CSCA_IDX,dsat_get_current_subs_id(FALSE),1,MIX_STR_TYPE);
      (void) strlcpy( (char *)str_item1,
                      (const char *)dsat_csca_prev_val[0].string_item,
                      CSCA_SCA_STR_MAX_LEN+1);
      (void) strlcpy( (char *)str_item2,
                      (const char *)dsat_csca_prev_val[1].string_item,
                      CSCA_TOSCA_STR_MAX_LEN+1);
    }
#endif /* FEATURE_ETSI_SMS && FEATURE_ETSI_SMS_TEMPLATE */
    
    /* send the response to the user */
    dsatcmdp_handle_async_cmd_rsp(res_buff, DSAT_COMPLETE_RSP);
    dsatutil_free_memory_ext((void **)&template_ptr);
    DSAT_SMS_STATE_CLEAN_ALL();
  }

  return result;
} /* dsatsms_err_handler */

#ifdef FEATURE_ETSI_SMS_CB 
/*===========================================================================

FUNCTION  DSATSMS_CB_HANDLER

DESCRIPTION
  Handler function for cell broadcast event from WMS

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  Possible values are:
    DSAT_OK : if the command has been successfully executed

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatsms_cb_handler 
( 
 dsat_mode_enum_type mode, /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  dsm_item_type * res_buff = NULL;
  byte * data_ptr = NULL;
  int  i = 0;
  uint16 index =0;
  dsat_result_enum_type result = DSAT_OK;
  dsat_string_item_type * str_item;
  
  DS_AT_MSG2_HIGH("ATCOP received an SMS CB message for cmd_type %d , event = %d ", 
                     dsatsmsi_pres_cmd.cmd_name, event_info->event);
  str_item = (uint8 *)dsatutil_get_val(DSATETSI_EXT_CSCB_IDX,0,CSCB_MIDS,MIX_STR_TYPE);
  memset ( (void *) str_item, 0x0, CSCB_MIDS_STR_MAX_LEN);
  if ( event_info->event == SMS_BC_MM_EVENT_SRV_IDS )
  {
    if ( dsatsmsi_pres_cmd.cmd_name == SMS_CSCB )
    {
      res_buff = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
      data_ptr = res_buff->data_ptr;

      res_buff-> used = (uint16)snprintf((char*)data_ptr,
                                   res_buff->size,
                                   "+CSCB: %d, \"",
                                   (dsat_num_item_type)dsatutil_get_val(
                                      DSATETSI_EXT_CSCB_IDX,0,0,MIX_NUM_TYPE));
      data_ptr = res_buff->data_ptr + res_buff-> used;
    }

    for ( i=0,index=0; i< event_info->u.stored_mids.size ; i++ )
    {
      if ( dsatsmsi_pres_cmd.cmd_name == SMS_CSCB )
      {
        if ( event_info->u.stored_mids.mids[i].from == event_info->u.stored_mids.mids[i].to )
        {
          /* Not a range */
          res_buff-> used+=(uint16)snprintf((char*)data_ptr,
                                         res_buff->size - res_buff->used,
                                         "%d,", event_info->u.stored_mids.mids[i].from);
          data_ptr = res_buff->data_ptr + res_buff-> used;
        }
        else
        {
          res_buff-> used += (uint16)snprintf((char*)data_ptr,
                                                    res_buff->size - res_buff->used,
                                                    "%d-%d,",
                                                    event_info->u.stored_mids.mids[i].from,
                                                    event_info->u.stored_mids.mids[i].to);
          data_ptr = res_buff->data_ptr + res_buff-> used;
        }
      }
      else if ( dsatsmsi_pres_cmd.cmd_name == SMS_CSAS_CSCB )
      /* Save CSCB settings for CSAS command*/
      {
        if ( event_info->u.stored_mids.mids[i].from == event_info->u.stored_mids.mids[i].to )
        {
#ifdef FEATURE_ETSI_SMS_CB
          index +=(uint16)snprintf((char*)&str_item[index],
                                         CSCB_MIDS_STR_MAX_LEN,
                                         "%d,", event_info->u.stored_mids.mids[i].from);
        }
        else
        {
          index +=(uint16)snprintf((char*)&str_item[index],
                                         CSCB_MIDS_STR_MAX_LEN,
                                         "%d-%d,", event_info->u.stored_mids.mids[i].from,
                                                   event_info->u.stored_mids.mids[i].to);
#endif /* FEATURE_ETSI_SMS_CB */
        }
      }
    }

    if ( dsatsmsi_pres_cmd.cmd_name == SMS_CSAS_CSCB )
    {
      /* Processing for CSAS command is done; update NV and return OK */
      DSAT_SMS_STATE_CLEAN();
#ifdef FEATURE_ETSI_SMS_CB
      memset ( (void*)&ds_nv_item, 0, sizeof(ds_nv_item));
      ds_nv_item.ds_at_csas_cscb_settings.mode = (dsat_num_item_type)
               dsatutil_get_val(DSATETSI_EXT_CSCB_IDX,0,CSCB_MODE,MIX_NUM_TYPE);
      
      (void) dsatutil_memscpy((void*)ds_nv_item.ds_at_csas_cscb_settings.mids,
              NV_CSCB_MIDS_STR_MAX_LEN,(void*)str_item,CSCB_MIDS_STR_MAX_LEN+1);
      
      DS_AT_MSG_SPRINTF_1_MED(
        "Storing CSAS_CSCB = %s ",str_item);

      str_item = (uint8 *)dsatutil_get_val(DSATETSI_EXT_CSCB_IDX,0,CSCB_DCSS,MIX_STR_TYPE);
      (void) dsatutil_memscpy((void*)ds_nv_item.ds_at_csas_cscb_settings.dcss,
              NV_CSCB_DCSS_STR_MAX_LEN,(void*)str_item,CSCB_DCSS_STR_MAX_LEN+1);
      if ( NV_DONE_S != dsatutil_put_nv_item(NV_DS_AT_CSAS_CSCB_SETTINGS_I, &ds_nv_item) )
      {
        return DSAT_ERROR;
      }
      return DSAT_OK;
#endif /* FEATURE_ETSI_SMS_CB */
    }
    if ( dsatsmsi_pres_cmd.cmd_name == SMS_CSCB )
    {
      if ( event_info->u.stored_mids.size != 0 )
      {
        *(data_ptr-1) = '\"';
      }
      else
      {
        *data_ptr++ = '\"';
        res_buff->used +=1;	
      }
      str_item = (uint8 *)dsatutil_get_val(DSATETSI_EXT_CSCB_IDX,0,2,MIX_STR_TYPE);
      res_buff-> used += (word)snprintf((char*)data_ptr,
                                              res_buff->size - res_buff->used,
                                              ",\"%s\"",
                                              str_item);
  
      /* send the response to the user */
      dsatcmdp_handle_async_cmd_rsp(res_buff, DSAT_COMPLETE_RSP);
      DSAT_SMS_STATE_CLEAN();
      result = DSAT_OK;
    }
  }
  else if ( event_info->event == SMS_BC_MM_EVENT_DELETE_ALL_SRVS &&
            dsatsmsi_pres_cmd.cmd_name == SMS_CRES_CSCB )
  {
    /* For CRES command, initially all the CSCB mids are 
       deleted from WMS. Deletion is succesfull, now add the 
       retreived MIDS from NV */
    result = dsatetsisms_process_cres ();
  }
  else if ( event_info->event == SMS_BC_MM_EVENT_ADD_SRVS &&
            dsatsmsi_pres_cmd.cmd_name == SMS_CRES_CSCB)
  {
    /* Adding MIDS retreived from NV to WMS is success; return OK*/
    result = DSAT_OK;
    DSAT_SMS_STATE_CLEAN();
  }

  return result;
}/* dsatsms_cb_handler */
#endif /* FEATURE_ETSI_SMS_CB */


/*===========================================================================

FUNCTION  DSATSMS_INIT_SMS

DESCRIPTION
  SMS initialization function. This function does the following:
  - Registers the WMS broadcast events callback function.

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
void dsatsms_init_sms ( void )
{
  wms_client_err_e_type result;

#ifdef FEATURE_ETSI_SMS_CB 
  result  = wms_client_reg_bc_mm_cb ( dsatsmsi_client_id,
                                      (wms_bc_mm_event_cb_type)sms_cb_msg_cb_func );
#endif /* FEATURE_ETSI_SMS_CB */

  /* This should not happen, raise an error */
  if (result != WMS_CLIENT_ERR_NONE)
  {
    DS_AT_MSG0_ERROR("Could not register call back function with WMS");
  }

  return;
} /* dsatsms_init_sms */

/*===========================================================================

FUNCTION  DSATSMS_DEINIT_SMS

DESCRIPTION
  SMS deinitialization function. This function does the following:
  - Deregisters the WMS broadcast events callback function.
  
DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
void dsatsms_deinit_sms ( void )
{
  wms_client_err_e_type result;

#ifdef FEATURE_ETSI_SMS_CB 
  result  = wms_client_reg_bc_mm_cb ( dsatsmsi_client_id,
                                      (wms_bc_mm_event_cb_type)NULL );
#endif /* FEATURE_ETSI_SMS_CB */

  /* This should not happen, raise an error */
  if (result != WMS_CLIENT_ERR_NONE)
  {
    DS_AT_MSG0_ERROR("Could not deregister call back function with WMS");
  }

  return;
} /* dsatsms_deinit_sms */

/**********************LOCAL UTILITY FUNCTIONS******************************/

/**********************CALL BACK FUNCTIONS**********************************/

#ifdef FEATURE_ETSI_SMS_CB
/*===========================================================================
FUNCTION  SMS_CB_MSG_CB_FUNC

DESCRIPTION
  SMS Cell Broadcast event callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer
===========================================================================*/
LOCAL void sms_cb_msg_cb_func
(
  wms_bc_mm_event_e_type            event,     /* Event ID              */
  const wms_bc_mm_event_info_s_type    *event_ptr /* Pointer to Event info */
)
{
  sms_event_info_s_type event_info;
  dsm_item_type * ds_cmd_buff;

  ASSERT (event_ptr != NULL);
  DS_AT_MSG1_HIGH(" SMS MSG Event", event);
  if ( ( dsatsmsi_pres_cmd.cmd_name == SMS_CSCB ) || 
       ( dsatsmsi_pres_cmd.cmd_name == SMS_CSAS_CSCB ) ||
       ( dsatsmsi_pres_cmd.cmd_name == SMS_CRES_CSCB ) ||
       ( dsatsmsi_pres_cmd.cmd_name == SMS_CNMI ) )
  {
    if ( ( event == WMS_BC_MM_EVENT_ADD_SRVS ) || 
         ( event == WMS_BC_MM_EVENT_DELETE_SRVS ) ||
         ( event == WMS_BC_MM_EVENT_PREF )        || 
         ( event == WMS_BC_MM_EVENT_DELETE_ALL_SRVS ) ||
         ( event == WMS_BC_MM_EVENT_SRV_IDS )     )
    {
      memset ( &event_info, 0, sizeof( sms_event_info_s_type ) );

      if ( event == WMS_BC_MM_EVENT_SRV_IDS )
      {
        int i = 0;
        /* Copy the information into local buffer */
        event_info.u.stored_mids.size = event_ptr->u.bc_mm_srv_ids.size;
        while ( i < event_info.u.stored_mids.size && i < MAX_MIDS )
        {
          event_info.u.stored_mids.mids[i].from = 
            event_ptr->u.bc_mm_srv_ids.entries[i].gw_cb_range.from;
          event_info.u.stored_mids.mids[i].to = 
            event_ptr->u.bc_mm_srv_ids.entries[i].gw_cb_range.to;
          i++;
        }
      }

      event_info.event = cb_msg_event_lookup(event);

      if ( event_info.event == SMS_EVENT_NONE )
      {
        DS_AT_MSG1_ERROR("unable to map, event:%d", event);
        return;
      }

      /* Place the information in a DSM item */
      ds_cmd_buff = dsat_dsm_create_packet(&event_info, sizeof(sms_event_info_s_type), TRUE);
      if (NULL != ds_cmd_buff)
      {
        dsm_enqueue(&dsat_sms_wm, &ds_cmd_buff);
      }
    }
  }


  return;
}/* sms_cb_msg_cb_func */
#endif /* FEATURE_ETSI_SMS_CB */


/*===========================================================================

FUNCTION  DSATSMS_ROUTE_CFG_EVENT_HANDLER

DESCRIPTION
  Handler function for route configuration event from WMS

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatsmsi_route_cfg_event_handler
(
 dsat_mode_enum_type mode, /* AT command mode    */
 sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  dsat_result_enum_type result;
  dsat_num_item_type  temp_bm_val = (dsat_num_item_type)dsatutil_get_val(
       DSATETSI_EXT_ACT_CNMI_ETSI_IDX,event_info->subs_id,CNMI_BM_VAL,NUM_TYPE);
  DEBUG(
        DS_AT_MSG1_MED("route_cfg_event_handler, pres_cmd:%d",
                dsatsmsi_pres_cmd.cmd_name);
        );

  if( dsatsmsi_pres_cmd.cmd_name == SMS_CPMS ||
      dsatsmsi_pres_cmd.cmd_name == SMS_CNMI)
  {
#ifdef FEATURE_ETSI_SMS_CB
    if ( temp_bm_val == 1 )
    {
      /* we are setting Cell broadcast, Enable the service */
#ifdef FEATURE_DUAL_SIM
      (void) wms_bc_ms_set_pref (
                                   dsatsmsi_client_id,
                                   event_info->subs_id,
                                   dsatsmsi_cmd_cb_func,
                                   NULL,
                                   WMS_MESSAGE_MODE_GW,
                                   WMS_BC_PREF_ACTIVATE_ALL
                                );
#else
      (void) wms_bc_mm_set_pref (
                                   dsatsmsi_client_id,
                                   dsatsmsi_cmd_cb_func,
                                   NULL,
                                   WMS_MESSAGE_MODE_GW,
                                   WMS_BC_PREF_ACTIVATE_ALL
                                );
#endif /* FEATURE_DUAL_SIM */
      dsatsmsi_pres_cmd.cmd_name = SMS_CNMI;
      result = DSAT_ASYNC_CMD;
    }
    else
#endif /* FEATURE_ETSI_SMS_CB */
    {
      DS_AT_MSG0_HIGH("ATCOP SMS: Route Config Event Handler");
      DSAT_SMS_STATE_CLEAN();
      result = DSAT_OK;
    }
  }
  else
  {
    result = DSAT_ASYNC_EVENT;
  }

  return result;
}/* dsatsmsi_route_cfg_event_handler */


/*===========================================================================

FUNCTION  DSATSMS_MEMORY_STATUS_CFG_EVENT_HANDLER

DESCRIPTION
  Handler function for memory status configuration event from WMS

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatsmsi_memory_status_cfg_event_handler
(
  dsat_mode_enum_type mode, /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  dsat_result_enum_type result = DSAT_ASYNC_EVENT;

  if( dsatsmsi_pres_cmd.cmd_name == SMS_CPMS )
  {
    dsm_item_type * raw_rsp_ptr;
    /* the pointer to write to the rsp dsm_item */
    char * buff_ptr; 
    wms_memory_status_s_type * mem_status_ptr = &event_info->u.mem_status;
    wms_status_e_type status;
    wms_memory_store_e_type mem_store;
    dsati_operating_cmd_mode_type cmd_mode;

#ifndef FEATURE_DSAT_EXTENDED_SMS
    dsat_rsp_enum_type type_of_rsp;
#endif /* FEATURE_DSAT_EXTENDED_SMS */
    /* MO template is also added now. The side effects is out of scope; 
       Please use a merciful return and ignore processing if,  MO template 
       causes any  trouble. */
    ASSERT((mem_status_ptr->tag == WMS_TAG_NONE)||
           (mem_status_ptr->tag == WMS_TAG_STATUS_RPT) ||
           (mem_status_ptr->tag == WMS_TAG_MO_TEMPLATE ));

    DS_AT_MSG1_MED("cmd entered is %d",dsatsmsi_pres_cmd.cmd_name);


    if(dsatcmdp_is_cdma_cmd())
    {
      cmd_mode = CDMA_CMD_MODE;
    }
    else if(dsatcmdp_is_etsi_cmd())
    {
      cmd_mode = ETSI_CMD_MODE;
    }
    else
    {
      return DSAT_ASYNC_EVENT;
    }

#ifdef FEATURE_DSAT_EXTENDED_SMS

    dsatsmsi_mem_report.used_tag_slots[dsatsmsi_mem_report.mem] = 
                                             mem_status_ptr->used_tag_slots;
    dsatsmsi_mem_report.max_slots[dsatsmsi_mem_report.mem] = 
                                             mem_status_ptr->max_slots;
    
    if (dsatsmsi_mem_report.mem == CPMS_MEM3) 
    {
      /* Allocate a dsm_item and write the status to the dsm buffer */
      raw_rsp_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
      buff_ptr = (char *) raw_rsp_ptr->data_ptr;
      if (dsatsmsi_mem_report.is_read) 
      {
        const def_list_type * list_ptr_m1 = dsatsmsi_get_mem_list(CPMS_MEM1,cmd_mode);
        const def_list_type * list_ptr_m2 = dsatsmsi_get_mem_list(CPMS_MEM2,cmd_mode);
        const def_list_type * list_ptr_m3 = dsatsmsi_get_mem_list(CPMS_MEM3,cmd_mode);
        
        /* sanity check */
        if ( (list_ptr_m1 == NULL) || 
             (list_ptr_m2 == NULL) || 
             (list_ptr_m3 == NULL) )
        {
          /* Its a bigger problem, we are neither in ETSI mode, 
             nor in CDMA mode */
          DS_AT_MSG0_ERROR("Wrong operating mode  ");
          DSAT_SMS_STATE_CLEAN();
          return DSAT_ERROR;
        } 
    
        raw_rsp_ptr->used = 
          (word) snprintf(buff_ptr,
                          raw_rsp_ptr->size - raw_rsp_ptr->used,
                          "%s: \"%s\",%ld,%ld,\"%s\",%ld,%ld,\"%s\",%ld,%ld", 
                          dsatsmsi_pres_cmd.cmd_hdr,
                          list_ptr_m1->list_v[dsatsmsi_get_mem_val(CPMS_MEM1,event_info->subs_id,cmd_mode)],
                          dsatsmsi_mem_report.used_tag_slots[CPMS_MEM1],
                          dsatsmsi_mem_report.max_slots[CPMS_MEM1],
                          list_ptr_m2->list_v[dsatsmsi_get_mem_val(CPMS_MEM2,event_info->subs_id,cmd_mode)],
                          dsatsmsi_mem_report.used_tag_slots[CPMS_MEM2],
                          dsatsmsi_mem_report.max_slots[CPMS_MEM2],
                          list_ptr_m3->list_v[dsatsmsi_get_mem_val(CPMS_MEM3,event_info->subs_id,cmd_mode)],
                          dsatsmsi_mem_report.used_tag_slots[CPMS_MEM3],
                          dsatsmsi_mem_report.max_slots[CPMS_MEM3]
                          );
      }
      else
      {
        raw_rsp_ptr->used = 
             (word) snprintf(buff_ptr,
                          raw_rsp_ptr->size - raw_rsp_ptr->used,
                          "%s: %d,%d,%d,%d,%d,%d",
                          dsatsmsi_pres_cmd.cmd_hdr,
                          dsatsmsi_mem_report.used_tag_slots[CPMS_MEM1], 
                          dsatsmsi_mem_report.max_slots[CPMS_MEM1],
                          dsatsmsi_mem_report.used_tag_slots[CPMS_MEM2],
                          dsatsmsi_mem_report.max_slots[CPMS_MEM2],
                          dsatsmsi_mem_report.used_tag_slots[CPMS_MEM3], 
                          dsatsmsi_mem_report.max_slots[CPMS_MEM3]);
      }
      dsatcmdp_handle_async_cmd_rsp(raw_rsp_ptr, DSAT_COMPLETE_RSP);
      dsatsmsi_mem_report.mem = CPMS_NONE;
      /* set route if mem3 changed */
      if( dsatsmsi_mem_report.mem3_changed )
      {
        /* Now set the routing configuration */
        if(dsatcmdp_is_cdma_cmd())
        {
          result = dsat707smsi_route_config ( 
                      (dsat_num_item_type)dsatutil_get_val(
                       DSAT707_SMS_QCNMI_IDX,0,CNMI_MT_VAL,NUM_TYPE), 
                      (dsat_num_item_type)dsatutil_get_val(
                       DSAT707_SMS_QCNMI_IDX,0,CNMI_DS_VAL,NUM_TYPE) );
        }
#ifdef FEATURE_ETSI_SMS
        else if(dsatcmdp_is_etsi_cmd())
        {
          result = dsatsmsi_route_config ( MT_VAL_0, DS_VAL_0,event_info->subs_id); 
        }
#endif /* FEATURE_ETSI_SMS */
        else
        {
          DSAT_SMS_STATE_CLEAN();
          result = DSAT_OK;
        }
      }
      else
      {
        DSAT_SMS_STATE_CLEAN();
        result = DSAT_OK;
      }
    }
    /* continue to report next memory status */
    else
    {
      dsatsmsi_mem_report.mem++;
    
      /* set mem_store */
      /* issue: hold to use lookup_mem function */
      mem_store = dsatsmsi_memory_lookup(
                  (int) dsatsmsi_get_mem_val(dsatsmsi_mem_report.mem,event_info->subs_id,cmd_mode), 
                                           dsatsmsi_mem_report.mem,cmd_mode);
    
#ifdef FEATURE_DUAL_SIM
      status = wms_cfg_ms_get_memory_status(dsatsmsi_client_id,
                                            event_info->subs_id,
                                            dsatsmsi_cmd_cb_func,
                                            &dsatsmsi_pres_cmd,
                                            mem_store,
                                            WMS_TAG_NONE);
#else
      status = wms_cfg_get_memory_status(dsatsmsi_client_id,
                                           dsatsmsi_cmd_cb_func,
                                           &dsatsmsi_pres_cmd,
                                           mem_store,
                                           WMS_TAG_NONE);
#endif /* FEATURE_DUAL_SIM */
    
      /* If the command is not accepted, the command callback will
       not be called, the mem event will not be invoked */
      if(status == WMS_OK_S)
      {
        result = DSAT_ASYNC_CMD;
      }
      else
      {
        result = DSAT_ERROR;
        DSAT_SMS_STATE_CLEAN();
        dsatsmsi_mem_report.mem = CPMS_NONE;
      }
    }
#else
    /* Allocate a dsm_item and write the status to the dsm buffer */
    raw_rsp_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
    buff_ptr = (char *) raw_rsp_ptr->data_ptr;
   

    if(dsatsmsi_mem_report.mem == CPMS_MEM1)
    {
      /* write the cmd info into the respond buffer */

      if(cmd_mode == ETSI_CMD_MODE )
      {
        raw_rsp_ptr->used += (word) snprintf(buff_ptr,
                                           raw_rsp_ptr->size - raw_rsp_ptr->used,
                                           "+CPMS: ");
      }
      else
      {
        raw_rsp_ptr->used += (word) snprintf(buff_ptr,
                                           raw_rsp_ptr->size - raw_rsp_ptr->used,
                                           "$QCPMS: ");
      }
      buff_ptr += raw_rsp_ptr->used;
    }
  
    /* this is for read command */
    if(dsatsmsi_mem_report.is_read)
    {
      const def_list_type * list_ptr = 
        dsatsmsi_get_mem_list(dsatsmsi_mem_report.mem,cmd_mode);

      /* sanity check */
      if ( list_ptr == NULL )
      {
        /* Its a bigger problem, we are neither in ETSI mode, nor in CDMA mode */
        DS_AT_MSG0_ERROR("Wrong operating mode  ");
        DSAT_SMS_STATE_CLEAN();
        return DSAT_ERROR;
      }

      raw_rsp_ptr->used += (word) snprintf(buff_ptr,
               raw_rsp_ptr->size - raw_rsp_ptr->used,"\"%s\",%ld,%ld,", 
               list_ptr->list_v[dsatsmsi_get_mem_val(dsatsmsi_mem_report.mem,event_info->subs_id,cmd_mode)],
               mem_status_ptr->used_tag_slots,
               mem_status_ptr->max_slots);
    }
    else /* for write command */
    {
       raw_rsp_ptr->used += (word) snprintf(buff_ptr,
                                    raw_rsp_ptr->size - raw_rsp_ptr->used,
                                    "%ld,%ld,", 
                                    mem_status_ptr->used_tag_slots,
                                    mem_status_ptr->max_slots);
    }
  
   

    /* queue the result */
    if(dsatsmsi_mem_report.mem == CPMS_MEM1)
    {
      type_of_rsp = DSAT_START_OF_RSP;
    }
    else if(dsatsmsi_mem_report.mem == CPMS_MEM2)
    {
      type_of_rsp = DSAT_INTERMEDIATE_RSP;
    }
    else if(dsatsmsi_mem_report.mem == CPMS_MEM3)
    {
      /* remove the last , */
      raw_rsp_ptr->used--;
      type_of_rsp = DSAT_END_OF_RSP;
    }
    else
    {
      DS_AT_MSG1_ERROR("Wrong cpms mem %d",dsatsmsi_mem_report.mem);
      DSAT_SMS_STATE_CLEAN();
      return DSAT_ERROR;
    }
    
    dsatcmdp_handle_async_cmd_rsp(raw_rsp_ptr, type_of_rsp);

    if( dsatsmsi_mem_report.mem == CPMS_MEM3 )
    {
      dsatsmsi_mem_report.mem = CPMS_NONE;
      /* set route if mem3 changed */
      if( dsatsmsi_mem_report.mem3_changed )
      {
        /* Now set the routing configuration */
#ifdef FEATURE_ETSI_SMS
      if(dsatcmdp_is_etsi_cmd())
      {
        result = dsatsmsi_route_config ( MT_VAL_0, DS_VAL_0,event_info->subs_id); 
      }
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_CDMA_SMS 
      if(dsatcmdp_is_cdma_cmd())
      {
        result = dsat707smsi_route_config(MT_VAL_0,DS_VAL_0);
      }
#endif /* FEATURE_CDMA_SMS */

      }
      else
      {
        DSAT_SMS_STATE_CLEAN();
        result = DSAT_OK;
      }
    }
    /* continue to report next memory status */
    else
    {
      dsatsmsi_mem_report.mem++;
    
      /* set mem_store */
      /* issue: hold to use lookup_mem function */
      mem_store = dsatsmsi_memory_lookup(
                  (int) dsatsmsi_get_mem_val(dsatsmsi_mem_report.mem,event_info->subs_id,cmd_mode), 
                                           dsatsmsi_mem_report.mem,cmd_mode);
#ifdef FEATURE_ETSI_SMS
      /* call the wms function to get memory status.
       * Check if the mem_store is "SR" , if so call the 
       * function using the tag WMS_TAG_STATUS_RPT  */
      DS_AT_MSG1_MED("Current referred memory is %d",dsatsmsi_mem_report.mem);
      if( (dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_ACT_CPMS_ETSI_IDX,
                    event_info->subs_id,dsatsmsi_mem_report.mem,NUM_TYPE) == 3 )
      {
#ifdef FEATURE_DUAL_SIM
        status = wms_cfg_ms_get_memory_status( dsatsmsi_client_id,
                                               event_info->subs_id,
                                               dsatsmsi_cmd_cb_func,
                                               &dsatsmsi_pres_cmd,
                                               mem_store,
                                               WMS_TAG_STATUS_RPT);
#else
        status = wms_cfg_get_memory_status(dsatsmsi_client_id,
                                           dsatsmsi_cmd_cb_func,
                                           &dsatsmsi_pres_cmd,
                                           mem_store,
                                           WMS_TAG_STATUS_RPT);
#endif /* FEATURE_DUAL_SIM */
      }
      else
#endif /* FEATURE_ETSI_SMS */
      {
#ifdef FEATURE_DUAL_SIM
        status = wms_cfg_ms_get_memory_status( dsatsmsi_client_id,
                                               event_info->subs_id,
                                               dsatsmsi_cmd_cb_func,
                                               &dsatsmsi_pres_cmd,
                                               mem_store,
                                               WMS_TAG_NONE);
#else
        status = wms_cfg_get_memory_status(dsatsmsi_client_id,
                                           dsatsmsi_cmd_cb_func,
                                           &dsatsmsi_pres_cmd,
                                           mem_store,
                                           WMS_TAG_NONE);
#endif /* FEATURE_DUAL_SIM */
      }

      /* If the command is not accepted, the command callback will
       not be called, the mem event will not be invoked */
      if(status == WMS_OK_S)
      {
        result = DSAT_ASYNC_CMD;
      }
      else
      {
        result = DSAT_ERROR;
        DSAT_SMS_STATE_CLEAN();
        dsatsmsi_mem_report.mem = CPMS_NONE;
      }
    }
#endif /* FEATURE_DSAT_EXTENDED_SMS */
  }
  else
  {
    /* Neglect this information */
    result = DSAT_ASYNC_EVENT;
  }

  return result;
} /* dsatsmsi_memory_status_cfg_event_handler */


/*===========================================================================

FUNCTION  DSATSMS_MESSAGE_LIST_CFG_EVENT_HANDLER

DESCRIPTION
  Handler function for message list configuration event from WMS

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatsmsi_message_list_cfg_event_handler
(
 dsat_mode_enum_type mode, /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsm_item_type * res_buff;
  byte * data_ptr;
  int num_of_indices;
  int loop = 0;

  DEBUG(
        DS_AT_MSG1_MED("memory_list_cfg...ndler, pres_cmd:%d ",
                dsatsmsi_pres_cmd.cmd_name);
        );

  if ( dsatsmsi_pres_cmd.cmd_name != SMS_CMGL &&
       dsatsmsi_pres_cmd.cmd_name != SMS_CMGD )
  {
#ifdef FEATURE_ETSI_SMS_TEMPLATE
    /* We only care about reading the template */
    if(event_info->u.msg_list.tag == WMS_TAG_MO_TEMPLATE)
    {
      /* Read the template information */
      return read_param_from_template( event_info );
    }
    else
#endif /* FEATURE_ETSI_SMS_TEMPLATE */
    {
      /* unwanted event */
      return DSAT_ASYNC_EVENT;
    }
  }

  DS_AT_MSG0_HIGH("ATCOP SMS: Message List Config Event Handler");

  /* Store the list if it is for a CMGL command */
  if ( dsatsmsi_pres_cmd.cmd_name == SMS_CMGL )
  {
    if ( event_info->u.msg_list.tag != WMS_TAG_NONE )
    {
      dsatsmsi_cmd_sm.num_of_indices = event_info->u.msg_list.len;
      dsatsmsi_cmd_sm.present_index = 0;
      DS_AT_MSG2_MED("cmd with tag %d,number of indices %d,",
               event_info->u.msg_list.tag,dsatsmsi_cmd_sm.num_of_indices);
      if( event_info->u.msg_list.len == 0 )
      {
        /* We got a zero length list                 
         * Check to see is we got more stats to read */
        if ( ! dsatsmsi_get_msg_list(event_info->subs_id) )
        {
          /* We are done with the list, clean up */
          if ( dsatsmsi_cmd_sm.list_begin )
          {
            dsatsmsi_cmd_sm.list_begin = FALSE;
          }
          else
          {
            res_buff = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
            res_buff->used = 0;
            dsatcmdp_handle_async_cmd_rsp(res_buff, DSAT_END_OF_RSP);
          }
          DSAT_SMS_STATE_CLEAN_ALL();
          return DSAT_OK;
        }
        else
        {
          return DSAT_ASYNC_CMD;
        }
      }
    
      for( loop = 0 ; loop < dsatsmsi_cmd_sm.num_of_indices ; loop++ )
      {
        dsatsmsi_cmd_sm.indices[loop] = 
          event_info->u.msg_list.indices[loop];
      }
    
      /* Read the message list */
      (void)dsatsmsi_read_msg_list(event_info->subs_id);

      /* Tell ATCOP that there is more to come */
      result = DSAT_ASYNC_CMD;
    }
    else
    {
      result = DSAT_ASYNC_EVENT;
    }
  } /* (dsatsmsi_pres_cmd.cmd_name == SMS_CMGL) */
  else if(  ( dsatsmsi_pres_cmd.cmd_name == SMS_CMGD ) && 
          ( !dsatsmsi_cmd_sm.one_index ))
  {
    if ( dsatsmsi_cmd_sm.is_test == 0 )
    {
      /* Check to see if this the message list we requested 
       * WMS sends the message_list_config_event_handler with
       * tag WMS_TAG_STATUS_RPT when we request for a MO_SENT 
       * tagged messages. We should not further process that event
       */

      if( ( event_info->u.msg_list.tag != WMS_TAG_NONE ) &&
          ( event_info->u.msg_list.tag != WMS_TAG_STATUS_RPT ))
      {
        /* We are deleting with a del flag           */
        /* Get the list and iterate through the list */
        dsatsmsi_cmd_sm.num_of_indices = event_info->u.msg_list.len;
        dsatsmsi_cmd_sm.present_index = 0;
    
        for( loop = 0 ; loop < dsatsmsi_cmd_sm.num_of_indices ; loop++ )
        {
          dsatsmsi_cmd_sm.indices[loop] = 
            event_info->u.msg_list.indices[loop];
        }
    
        /* got the list            */
        /* now delete the messages */
        result = dsatsmsi_delete_msg_list(event_info->subs_id);

        if ( result == DSAT_OK )
        {
          /* We are done with the commands, clean up */
          /* send the response to the user */
          DSAT_SMS_STATE_CLEAN_ALL();
          return result;
        }
        else if ( result == DSAT_CMD_ERR_RSP )
        {
          res_buff = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
          /* we got an error, format a +CMS ERROR */
          res_buff->used += (byte)dsatsmsi_cms_error(res_buff,
                                                     (int)DSAT_CMS_UNKNOWN_ERR);
          /* send the response to the user */
            dsatcmdp_handle_async_cmd_rsp(res_buff, DSAT_COMPLETE_RSP);
          /* We are done with the commands, clean up */
          DSAT_SMS_STATE_CLEAN_ALL();
          return result;
        }
        else
        {
          /* we got some more work to do */
          return result;
        }
      } /* if (! TAG_NONE) */
      else
      {
        return DSAT_ASYNC_EVENT;
      }
    } /* if(dsatsmsi_cmd_sm.is_test == 0) */
    else
    {
      /* this is a CMGD TEST command */
      res_buff = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
      data_ptr = res_buff->data_ptr;

      num_of_indices = event_info->u.msg_list.len;

      if(!dsatcmdp_is_etsi_cmd())
      {
        res_buff->used = (word)snprintf((char*)data_ptr,
                                              res_buff->size,
                                              "$QCMGD: ");
      }
      else
      {
        res_buff->used = (word)snprintf((char*)data_ptr,
                                              res_buff->size,
                                              "+CMGD: ");
      }

      data_ptr +=res_buff->used;
     
      *data_ptr++ = '(';

      for( loop = 0 ; loop < num_of_indices ; loop++ )
      {
        data_ptr = dsatutil_itoa(event_info->u.msg_list.indices[loop],
                                 data_ptr,
                                 10);
        *data_ptr++ = ',';
      }
  
      /* Overwrite last ',' from the buffer */
      if (num_of_indices > 0 )
      {
        data_ptr--;
      }

      *data_ptr++ = ')';
      res_buff->used = (uint16)(data_ptr - res_buff->data_ptr);

      *data_ptr++ = ',';
      res_buff->used++;
      res_buff->used+= (word)snprintf((char*)data_ptr,
                                            res_buff->size - res_buff->used,
                                            "(0-4)");

    
      /* send the response to the user */
      dsatcmdp_handle_async_cmd_rsp(res_buff,DSAT_COMPLETE_RSP);
      /* We are done with the commands, clean up */
      DSAT_SMS_STATE_CLEAN_ALL();
      result = DSAT_OK;
    }
  } /* (dsatsmsi_pres_cmd.cmd_name == SMS_CMGD) */
  /* Its OK to neglect rest of the message list notifications */
  else 
  {
    result = DSAT_ASYNC_EVENT;
  }

  return result;
} /* dsatsmsi_message_list_cfg_event_handler */

#ifdef FEATURE_ETSI_SMS_CB
/*===========================================================================
FUNCTION  CB_MSG_EVENT_LOOKUP

DESCRIPTION
  Maps the wms_bc_mm_event_e_type to sms_event_e_type

DEPENDENCIES
  None

RETURNS
  sms_event_e_type

SIDE EFFECTS
  None
===========================================================================*/
sms_event_e_type cb_msg_event_lookup
(
 wms_bc_mm_event_e_type wms_event /* Event ID */
)
{
  sms_event_e_type atcop_event;

  switch ( wms_event )
  {
    case WMS_BC_MM_EVENT_ADD_SRVS:
      atcop_event = SMS_BC_MM_EVENT_ADD_SRVS;
      break;
    case WMS_BC_MM_EVENT_DELETE_SRVS:
      atcop_event = SMS_BC_MM_EVENT_DELETE_SRVS;
      break;
    case WMS_BC_MM_EVENT_PREF:
      atcop_event = SMS_BC_MM_EVENT_PREF;
      break;
    case WMS_BC_MM_EVENT_DELETE_ALL_SRVS:
      atcop_event = SMS_BC_MM_EVENT_DELETE_ALL_SRVS;
      break;
    case WMS_BC_MM_EVENT_SRV_IDS:
      atcop_event = SMS_BC_MM_EVENT_SRV_IDS;
      break;
    default:
      atcop_event = SMS_EVENT_NONE;
      break;
  }

  return atcop_event;
}
#endif /* FEATURE_ETSI_SMS_CB */

#ifdef FEATURE_ETSI_SMS_TEMPLATE
/*===========================================================================

FUNCTION READ_PARAM_FROM_TEMPLATE

DESCRIPTION
Access SIM for SCA, DCS and PID parameters. WMS has listed a message in the
templates. Read to see if we have SCA, PID, DCS in it. If there are then
store the values in the local table.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type read_param_from_template
(
  const sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  dsat_result_enum_type result     = DSAT_ASYNC_EVENT;
  boolean               is_pending = TRUE;

  dsatsms_template_init_flag = TRUE;

  /* Check the length of the list */
  if ( event_info->u.msg_list.len == 0 ) 
  {
    /* No templates in the list */
    dsatetsismsi_template_present = FALSE;
    /* Valid template is not present in SIM so get it from NV 
       if the previous SIM read fails */
    if (event_info->u.msg_list.mem_store == WMS_MEMORY_STORE_SIM )
    {
       if ( wms_cfg_ms_get_message_list( dsatsmsi_client_id,
                                         event_info->subs_id,
                                         dsatsmsi_cmd_cb_func,
                                         (void *) &dsatsmsi_pres_cmd,
                                         WMS_MEMORY_STORE_NV_GW,
                                         WMS_TAG_MO_TEMPLATE ) != WMS_OK_S )
       {
         DS_AT_MSG0_ERROR("Failed to get msg list info from WMS");
         is_pending = FALSE;
       }
    }
    else
    {
      is_pending = FALSE;
    }
  }
  else
  {
    /* Read the first template : ATCOP limitation */
    dsatetsismsi_template_present = TRUE;

    if ( wms_msg_ms_read_template( dsatsmsi_client_id,
                                   event_info->subs_id,
                                   dsatsmsi_cmd_cb_func,
                                   (void *) &dsatsmsi_pres_cmd,
                                   event_info->u.msg_list.mem_store,
                                   event_info->u.msg_list.indices[0] ) != WMS_OK_S )
    {
      DS_AT_MSG0_ERROR("Failed to read template from WMS");
      is_pending = FALSE;
    }
  }

  if ( (FALSE == is_pending) &&
       (SMS_TEMPLATE == dsatsmsi_pres_cmd.cmd_name) )
  {
    result = dsatsms_process_sms_cmd();
  }

  return result;
}

#endif /* FEATURE_ETSI_SMS_TEMPLATE */

#ifdef FEATURE_DSAT_EXTENDED_SMS
/*===========================================================================

FUNCTION  DSATSMSI_MEMORY_FULL_HANDLER

DESCRIPTION
  Handler function for message full event from WMS

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatsmsi_memory_full_handler
(
 dsat_mode_enum_type mode, /* AT command mode    */
 sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  dsat_result_enum_type result = DSAT_ASYNC_EVENT;
  dsm_item_type *res_buff_ptr = NULL;

  res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, TRUE);
  if (NULL != res_buff_ptr)
  {
    if (event_info->u.mem_full == WMS_MEMORY_STORE_RUIM) 
    {
      res_buff_ptr->used += (word)snprintf((char*)res_buff_ptr->data_ptr,
                                                 res_buff_ptr->size - res_buff_ptr->used,
                                                 "^SMMEMFULL: \"SM\"");
  
    }
    else if (event_info->u.mem_full == WMS_MEMORY_STORE_NV_CDMA) 
    {
      res_buff_ptr->used += (word)snprintf((char*)res_buff_ptr->data_ptr,
                                       res_buff_ptr->size - res_buff_ptr->used,
                                       "^SMMEMFULL: \"ME\"");
    }
    else
    {
      dsm_free_packet( &res_buff_ptr );
      return result;
    }
    dsatcmdp_send_urc_per_subs(event_info->subs_id, res_buff_ptr, DSAT_COMPLETE_RSP);
  }

  return result;
}
#endif /* FEATURE_DSAT_EXTENDED_SMS */

#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) */

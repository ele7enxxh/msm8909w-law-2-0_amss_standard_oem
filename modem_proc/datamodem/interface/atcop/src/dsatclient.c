/*===========================================================================

                        D A T A   S E R V I C E S

                A T   C O M M A N D   P R O C E S S O R

    C L I E N T   I N T E R F A C E   I M P L E M E N T A T I O N  F I L E


DESCRIPTION
  This file contains the definitions of data structures, defines and
  enumerated constants, and function implementations required for the
  data services AT command processor client asynchronous interface.

  Copyright (c) 2005 - 2015 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatclient.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/30/15   sc      Fixed memory leak.
10/24/14   tk      ATCoP changes for dynamic memory optimizations.
06/27/14   tk      Added support for Dynamic ATCoP.
06/17/14   sc      Fixed KW errors.
06/05/14   sc      Fixed security bug.
05/23/14   sc      Fixed AT command response issue for forwarding commands.
03/11/14   tk      Optimized debug macros usage in ATCoP.
08/30/13   pg      Fixed predictable random number generation issue.
06/06/12   sb      Fixed compiler warning.
12/23/11   ua      Route the Operating mode Reset command through 
                   DMS->AT->QMI-AT->Apps Proc
05/18/12   tk      Migrated to MSG 2.0 macros
04/11/12   sk      Adding support for forwarding CFUN.
02/17/12   sb      Fixed Compiler and KW Warnings.
01/19/12   sk      Feature cleanup.
06/29/11   nc      Adding support for forwarding +CDIS,+CRSL,+CMAR,+CSO,+CSS to APPS_PROC
06/17/11   dvk     Global Variables Cleanup
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
10/27/10   bs      Replace REX_DISABLE_PREMPTION()/REX_ENABLE_PREMPTION().
10/25/10   ad      Init Changes for DSDS. 
10/06/10   ad      Added Error msg before Assert(0).
10/07/10   bs      Fixed validation issue in fwd_at_commands.
05/17/10   ua      Removing dependency of ATCoP Macro MAX_ARG at QMI.
06/17/10   ua      Changed the 4th param of CMEC as a new data (new optional 
                   TLV in QMI ).
01/15/10   nc      Correcting CSTA to CTSA for Modem Bridge.
12/15/09   nc      Featurisation changes for LTE.
04/20/09   bs/sa   Added support for Modem Bridge Architecture.
06/29/09   vg	   Replacing MSG_XXX with MSG_SPRINTF_X.
11/25/08   ss      Off target Lint fix.
09/26/08   sa      Added support for +CPIN for external client API.
07/16/08   ua      Added support for external client support.
03/24/08   bs      Fixed Klocwork criticals.
11/24/07   ua      Fixed Lint-lows. 
04/23/07   pp      Lint Medium fixes.
02/15/07   ss      Fixed lint high errors
02/21/06   sa      Merged changes from Sirius Branch 
                   -Change SS info callback interface.
11/08/05   ar      Added serving system query callback registration.
06/04/05   tkk     Added new operation mapping to get list of supported 
                   languages from UI.
05/24/05   hap     Modifications for handling spurious responses from UI
05/06/05   hap     Modified dsat_client_msg_submit
04/15/05   ar      Add dsatsrsacc_send_restricted_msg() on client reg.
04/15/05   sb      Fixed lint errors
04/15/05   tkk     Corrected ^SLNG event mapping.
04/01/05   tkk     Added support for sound/timezone/language/annunicator related
                   commands.
03/31/05   sb      Fixes from testing the serializer changes.
03/08/05   iv      Added support for +CRMP, ^SRMP & ^SSET commands
02/10/05   sb      Accessories related changes.
02/07/05   ar      Added asynchronous processing for accessory commands.
01/26/05   ar      Simplify callback table and routines.
                   Add accessory manager interface elements.
01/25/05   ibm     Initial version.

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"

#include <stringl/stringl.h>


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "amssassert.h"

#include "intconv.h"
#include "dsatclienti.h"
#include "dsati.h"
#include "ps_utils.h"


#ifdef FEATURE_MODEM_RCINIT_PHASE2
#include "rcevt.h"
#include "rcecb.h"

#endif /* FEATURE_MODEM_RCINIT_PHASE2 */

/*===========================================================================

                    REGIONAL DEFINITIONS AND DECLARATIONS

===========================================================================*/
  extern dsat_fwd_at_pending_type dsatcmdp_processing_fwd_cmd;



/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/
#ifdef FEATURE_DSAT_EXT_CLIENT_SUPPORT
  static rex_crit_sect_type dsat_client_crit_sect;
#endif /* FEATURE_DSAT_EXT_CLIENT_SUPPORT*/


#ifdef FEATURE_DSAT_EXT_CLIENT_SUPPORT
  dsat_client_info_s_type dsat_client_info[DSAT_EXT_CLIENT_ID_MAX];
#endif /* FEATURE_DSAT_EXT_CLIENT_SUPPORT */
/*-------------------------- Local Functions -------------------------------*/
#ifdef FEATURE_DSAT_EXT_CLIENT_SUPPORT
/*===========================================================================

FUNCTION DSATCLIENT_INIT_CLIENT_INFO

DESCRIPTION
  This function will initialize client info with default values.

DEPENDENCIES
  None
  
RETURN VALUE
   None
  
SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED*/
void  dsatclient_init_client_info
( 
  void
)
{
  uint8 index = 0;
     /*-------------------------------------------------------
                 Clean up the clients associated data 
      -------------------------------------------------------*/
  for(index = 0 ; index < DSAT_EXT_CLIENT_ID_MAX ;index++)
  {
    dsat_client_info[index].reg = FALSE;
    dsat_client_info[index].at_cmd_ptr = NULL;
    dsat_client_info[index].cmd_len      = 0;
    dsat_client_info[index].cb_func      = NULL;
    dsat_client_info[index].pending_flag = FALSE;
    dsat_client_info[index].client_data  = NULL;
    
    dsat_client_info[index].urc_reg = FALSE;
    dsat_client_info[index].urc_resp_cb_func = NULL;
    dsat_client_info[index].urc_client_data  = NULL;
    dsat_client_info[index].prev_response_type = DSAT_RESP_NONE;
  }
    
}/*dsatclient_init_client_info*/

/*===========================================================================

FUNCTION DSATCLIENT_REGISTER_CLIENT

DESCRIPTION
  This function allows a client to register with ATCOP for sending in AT 
  commands. The client ID will be a non-negative integer, if the call is 
  successful, and a negative value otherwise. Re-registrations are not allowed
  before releasing the earlier registration.

DEPENDENCIES
  None

RETURN VALUE
  TRUE on successful operation, FALSE otherwise.
  Client identifier is DSAT_EXT_CLIENT_ID_INVALID on error, non-negative
  integer on success.

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatclient_register_client 
( 
  dsat_client_e_type      client_type,   /* Known client type */
  dsat_ext_client_id_type *client_id_ptr /* Assign client ID upon success */
)
{
  boolean result = TRUE;
  ASSERT ( client_id_ptr != NULL );

  if(FALSE == dsat_is_atcop_allowed())
  {
     DS_AT_MSG0_ERROR(" ATcop is not initialized");
     return FALSE;
  }
  /*-------------------------------------------------------
               Client type should be known
  -------------------------------------------------------*/
  if ( VALIDATE_CLIENT_TYPE(client_type) )
  {
    DS_AT_MSG1_ERROR(" Invalid client type specified %d",client_type);
    *client_id_ptr = DSAT_EXT_CLIENT_ID_INVALID;
    return FALSE;
  }
  /*------------------------------------------------------------
            Validate and update the clients registration 
            status. This should happen in a critcal section
  -------------------------------------------------------------*/
  rex_enter_crit_sect(&dsat_client_crit_sect);

  if ( FALSE == dsat_client_info[client_type].reg )
  {
    dsat_client_info[client_type].reg = TRUE;
  }
  else
  {
    DS_AT_MSG1_ERROR("Client with type %d is already registered",client_type);
    *client_id_ptr = DSAT_EXT_CLIENT_ID_INVALID;
    result = FALSE;
  }
   rex_leave_crit_sect(&dsat_client_crit_sect);
  *client_id_ptr = (dsat_ext_client_id_type)client_type;
  return result;

} /* dsatclient_register_client  */

/*===========================================================================

FUNCTION DSATCLIENT_REGISTER_URC

DESCRIPTION
  This function registers clients for recieving URC.

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE on successful operation, FALSE otherwise.
  
SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED*/
boolean dsatclient_register_urc
( 
  dsat_ext_client_id_type       client_id,        /* Registered client id */
  void                         *client_data,      /* client data */
  dsat_client_cmd_rsp_cb_func   urc_response_func /* Response callback function */
)
{
  boolean result = TRUE;
 /*-------------------------------------------------------
                Validate the client
  -------------------------------------------------------*/
  if ( VALIDATE_CLIENT_ID(client_id) )
  {
    DS_AT_MSG1_ERROR(" Invalid client Client ID %d",client_id);
    return FALSE;
  }
    /*-------------------------------------------------------
                Validate the response call back function
  -------------------------------------------------------*/
  if ( NULL == urc_response_func )
  {
    DS_AT_MSG0_ERROR("Function Pointer can't be NULL ");
    return FALSE;
  }

  /*-------------------------------------------------------
                      Critical Section 
  -------------------------------------------------------*/
 
  /*Check, Client is registered */
 
    rex_enter_crit_sect(&dsat_client_crit_sect);
  {
    if ( TRUE != dsat_client_info[client_id].reg )
    {
      DS_AT_MSG1_ERROR(" Client %d is not a registred client",client_id);
      result =  FALSE;
    }
    else
    {
      dsat_client_info[client_id].urc_reg = TRUE;
      dsat_client_info[client_id].urc_client_data = client_data;
      dsat_client_info[client_id].urc_resp_cb_func = urc_response_func;
    }
  }
  rex_leave_crit_sect(&dsat_client_crit_sect);
  return result;
} /* dsatclient_register_urc */

/*===========================================================================

FUNCTION DSATCLIENT_DEREGISTER_URC

DESCRIPTION
  This function deregisters the client for URC.

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE on successful operation, FALSE otherwise.
  
SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED*/
boolean  dsatclient_deregister_urc
( 
  dsat_ext_client_id_type       client_id /* Registered client id */
)
{
  boolean result = TRUE;
  /*-------------------------------------------------------
            Validate the client
------------------------------------------------------*/
  if ( VALIDATE_CLIENT_ID(client_id) )
  {
    DS_AT_MSG1_ERROR(" Invalid client Client ID %d",client_id);
    return FALSE;
  }
  /*-------------------------------------------------------
                  Critical Section 
------------------------------------------------------*/
  rex_enter_crit_sect(&dsat_client_crit_sect);

  if ( (TRUE != dsat_client_info[client_id].reg) ||  
       (TRUE != dsat_client_info[client_id].urc_reg))
  {
    DS_AT_MSG1_ERROR(" Client %d is not a registred client",client_id);
    result =  FALSE;
  }else
  {
    /*-------------------------------------------------------
               Clean up the clients associated data 
       -------------------------------------------------------*/
    dsat_client_info[client_id].urc_reg = FALSE;
    dsat_client_info[client_id].urc_resp_cb_func = NULL;
    dsat_client_info[client_id].urc_client_data  = NULL;  
  }
   rex_leave_crit_sect(&dsat_client_crit_sect);
  
  return result;
} /* dsatclient_deregister_urc */

/*===========================================================================

FUNCTION DSATCLIENT_SEND_AT_CMD

DESCRIPTION
  This function allows external clients to send in AT commands. This function
  will be executed from clients context. 

  Internally, this client is mapped to DS3G_SIOLIB_CLIENT_VSP_PORT and 
  corresponding signal is raised to process the AT command.

DEPENDENCIES
  Currently, only +CSIM, +CRSM, +CLCK, AT commands are supported. 

RETURN VALUE
  TRUE: If commands are successfully posted to ATCOP in DS task. 
  FALSE: On any error and no further processing is done.

SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
boolean dsatclient_send_at_cmd
(
  dsat_ext_client_id_type      client_id,  /* Registered client id */
  dsat_data_s_type             at_cmd,     /* AT command buffer and length  */
  dsat_client_cmd_rsp_cb_func  cb_func,    /* Response callback function */
  void                        *client_data /* Client's data */
)
{
  
  ds_cmd_type            *cmd_buf  = NULL;
  ds_at_send_at_cmd_s_type *send_at_cmd_ptr = NULL;

  /*-------------------------------------------------------
                Validate the client ID, Command Buffer, 
                 Call back function, data length
  -------------------------------------------------------*/
  if ( VALIDATE_CLIENT_ID(client_id) || 
       ( FALSE == dsat_client_info[client_id].reg) )
  {
    DS_AT_MSG1_ERROR(" Client %d is not a registred client",client_id);
    return FALSE;
  }
  /*Input Buffer check*/
  if ( (at_cmd.buffer == NULL) || (at_cmd.length == 0) ||
       (at_cmd.length > DSAT_SW_API_CMD_LINE_SIZE ) ) 
  {
    DS_AT_MSG1_ERROR(" Invalid length %d ",at_cmd.length);
    return FALSE;
  }

  if ( cb_func == NULL )
  {
    DS_AT_MSG0_ERROR("Function Pointer can't be NULL ");
    return FALSE;
  }

  /*---------------------------------------------------------------
      Command checking and validation will be performed based on 
      pre-processor mode while parsing.
      'at_cmd.buffer' can be used for:
      1) Aborting can contain single or more words
      2) SMS mode
      3) A\
      4) AT commands
  ---------------------------------------------------------------*/

  /*-----------------------------------------------------------
     Post the command to DS task and return success to client
  ------------------------------------------------------------*/
  cmd_buf = dsat_get_cmd_buf(sizeof(ds_at_send_at_cmd_s_type), FALSE);
  send_at_cmd_ptr = cmd_buf->cmd_payload_ptr;

  cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_SEND_AT_CMD;
  (void) dsatutil_memscpy((void*)&send_at_cmd_ptr->at_cmd_buff[0],
          MAX_LINE_SIZE,(void*)at_cmd.buffer,at_cmd.length);

  send_at_cmd_ptr->cb_func     = cb_func;
  send_at_cmd_ptr->client_data = client_data;
  send_at_cmd_ptr->client_id   = client_id;
  send_at_cmd_ptr->cmd_len     = at_cmd.length;

  ds_put_cmd(cmd_buf);
  DS_AT_MSG3_MED("Client details Client ID %d  Reg %d Urc Reg %d",
    client_id,dsat_client_info[client_id].reg,dsat_client_info[client_id].urc_reg);

  return TRUE;
}/* dsatclient_send_at_cmd */

/*===========================================================================

FUNCTION DSATCLIENT_SEND_AT_CMD_HANDLER

DESCRIPTION
  This function is the handler to process the client's AT command with in DS
  task context. 

DEPENDENCIES
  NONE. 

RETURN VALUE
DSAT_ASYNC_EVENT

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type  dsatclient_send_at_cmd_handler 
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  ds_at_send_at_cmd_s_type *send_at_cmd_ptr = dsat_get_cmd_payload_ptr(cmd_ptr);

  dsatclient_process_send_at_cmd( 
                      send_at_cmd_ptr->at_cmd_buff,
                      send_at_cmd_ptr->cmd_len,
                      send_at_cmd_ptr->client_id,
                      send_at_cmd_ptr->cb_func,
                      send_at_cmd_ptr->client_data
                     );

  return DSAT_ASYNC_EVENT;
}/* dsatclient_send_at_cmd_handler */

/*===========================================================================

FUNCTION DSATCLIENT_PROCESS_SEND_AT_CMD

DESCRIPTION
  This function process the AT command from the client's command queue 
  buffer. After basic validation, this enques the water mark to simulate 
  as if the command has come from a virtual serial port. 

DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/
void dsatclient_process_send_at_cmd 
(
  char                       *at_cmd_buff, /* Buffer holding AT command */
  uint16                      cmd_len,     /* AT command length */
  dsat_ext_client_id_type     client_id,   /* Client which posted the command */
  dsat_client_cmd_rsp_cb_func cb_func,     /* Client's response call back */
  void                       *client_data  /* Cleint data to be sent back */
)
{
#define DSATCLIENT_SEND_PROCESS_RESP( passfail , rsp_typ ) \
  { \
   memset(&resp_type,0x0,sizeof(resp_type)); \
   resp_type.pass_fail = passfail; \
   resp_type.client_id = client_id; \
   resp_type.response_type = rsp_typ; \
   cb_func(resp_type, temp_var, client_data ); \
  }

  ds3g_siolib_state_info_type *state= NULL;
  dsm_item_type *at_cmd_ptr =  NULL;
  char *local_cmd_buff = NULL;
  dsat_data_s_type temp_var = {0};
  dsat_sw_api_resp_s_type      resp_type;
  dsat_ext_client_id_type      current_client;

  /*-------------------------------------------------------
                  Basic validations
  --------------------------------------------------------*/

  if( (VALIDATE_CLIENT_ID(client_id) )||
      ( at_cmd_buff == NULL ) ||
      ( cb_func == NULL ) )
  {
    /* This will be a severe problem */
    ASSERT(0);
  }

  /* Reject a second command to be queued from a client. 
     Only one command can be queued, while a command is being processed by another client */
       
  if( dsat_client_info[client_id].pending_flag == TRUE )
  {
    DS_AT_MSG1_MED("Client %d processing is pending ",client_id);
    /* call callback command processing */
    DSATCLIENT_SEND_PROCESS_RESP(FALSE,DSAT_RESP_NONE);
    return;
  }
  
  /* 
     Reject a command from the same client if the preprocessor mode is not in ABORTABLE state.
     This means that if the current command is not abortable, reject input from the same client.
  */
  current_client = ds3g_siolib_get_active_ext_client();
  
  if(
     ( current_client == client_id ) && 
     (( dsatprep_get_preprocessor_mode() == DSATI_AT_CMD_MODE )||
      ( dsatprep_get_preprocessor_mode() == DSATI_INVALID_MODE))
    )
  {
    DS_AT_MSG1_MED("Client %d command processing is in process and command is not abortable",client_id);
    /* call callback command processing */
    DSATCLIENT_SEND_PROCESS_RESP(FALSE,DSAT_RESP_NONE);
    return;
  }

  state = ds3g_siolib_get_ds3g_siolib_state(DS3G_SIOLIB_CLIENT_VSP_PORT);

  dsat_client_info[client_id].client_data = client_data;
  dsat_client_info[client_id].cb_func = cb_func;
  dsat_client_info[client_id].prev_response_type = DSAT_RESP_NONE;
  /*-------------------------------------------------------
      Send command only when no client is being serviced.
      If there is another client being processed, queue the
      command for later processing.
  --------------------------------------------------------*/

  if( current_client == DSAT_EXT_CLIENT_ID_INVALID ||
    ( current_client == client_id ))
  {
    /* The external client might send in the command buffer with a 
       terminating S3 character or not. We need to make sure there 
       that it should end with a S3 character. We are not sure about
       at_cmd_buff length so copy into a local buffer and append the S3
       character */

    cmd_len = MIN(cmd_len, DSAT_SW_API_CMD_LINE_SIZE);
    local_cmd_buff = (char *) dsat_alloc_memory(MIN(cmd_len, DSAT_SW_API_CMD_LINE_SIZE),FALSE);

    (void) dsatutil_memscpy((void*)&local_cmd_buff[0],
                            MAX_LINE_SIZE,
                            (void*)at_cmd_buff, 
                            cmd_len);

    /* Append command buffer with s3 character*/
    local_cmd_buff[cmd_len++]= (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
    at_cmd_ptr = dsat_dsm_create_packet(&local_cmd_buff[0], cmd_len, TRUE);
    if (NULL == at_cmd_ptr)
    {
      DSATCLIENT_SEND_PROCESS_RESP(FALSE,DSAT_RESP_NONE);

      /*Free the allocated command buffer*/
      dsatutil_free_memory_ext((void **) &local_cmd_buff);
      return;
    }
    ds3g_siolib_set_active_ext_client ( client_id );
  /*-------------------------------------------------------------------
      The enqueue would eventually raise signal for Client command 
      processing DS3G_SIOLIB_CLIENT_VSP_PORT port.
  ---------------------------------------------------------------------*/
    dsm_enqueue(&state->serial_info.rx_wm_ptr,&at_cmd_ptr);

    /*Free the allocated command buffer*/
    dsatutil_free_memory_ext((void **) &local_cmd_buff); 
  }
  else
  {
    DS_AT_MSG2_MED(" Client %d is in progress, Queue the command for client %d",
                                             current_client, 
                                             client_id);
    dsat_client_info[client_id].pending_flag = TRUE;

    /*Buffer is de-allocated in the dsat_client_send_response*/
    dsat_client_info[client_id].at_cmd_ptr = (char *) dsat_alloc_memory(cmd_len, FALSE);

    (void) dsatutil_memscpy((void*)dsat_client_info[client_id].at_cmd_ptr,
                            cmd_len,
                            (void*)at_cmd_ptr,
                            cmd_len);

    dsat_client_info[client_id].cmd_len = cmd_len;
  }
}/* dsatclient_process_send_at_cmd  */
/*===========================================================================

FUNCTION DSATCLIENT_SEND_URC_RESPONSE

DESCRIPTION
  This function is used to send the URC response to all the registered clients. 

DEPENDENCIES
  The client  must have registered to ATCoP through 
  dsatclient_register_client and dsatclient_register_urc. 

RETURN VALUE
  NONE

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatclient_send_urc_response
( 
  dsm_item_type             * tx_ptr, /* response data in dsm item */
  ds3g_siolib_status_e_type *ret_val,  /* return value of success/failure*/
  boolean       rearm_autodetect /* Done with one AT Command Line?*/
)
{
  static char               res_buff[MAX_RESP_SIZE] = {0};
  uint16                    res_buff_len = 0;
  dsat_data_s_type          response;
  dsat_sw_api_resp_s_type   resp_type;
  uint8  index = 0 ;
  
  if (tx_ptr == NULL || ret_val == NULL)
  {
    return;
  }
  
 *ret_val = DS3G_SIOLIB_OK;
  memset(&res_buff[0],0x0,MAX_RESP_SIZE);
  
  res_buff_len = MIN(tx_ptr->used,MAX_RESP_SIZE-1);
  /*Copy rquired data into local buffer, local buffer will sent to external client*/
  if(tx_ptr->data_ptr != NULL && res_buff_len != 0)
  {
    (void) dsatutil_memscpy((void*)res_buff,
          MAX_RESP_SIZE,(void*)tx_ptr->data_ptr,res_buff_len);   
  }else
  {
    DS_AT_MSG1_MED("URC pointer is empty, length %d",res_buff_len);
    return;
  }
  /*Make string NULL terminated*/
  res_buff[res_buff_len]='\0';
  
  
  /* Prepare response buffer */
  response.buffer = &res_buff[0];
  response.length = res_buff_len;

  /* Prepare response type */
  resp_type.pass_fail = TRUE;
  resp_type.response_type = DSAT_RESP_COMPLETE;
 
  for(index = 0 ; index < DSAT_EXT_CLIENT_ID_MAX ;index++)
  {   
    /*Send data to registered client only */
    if(TRUE == dsat_client_info[index].reg &&
       TRUE == dsat_client_info[index].urc_reg &&
       NULL != dsat_client_info[index].urc_resp_cb_func)
    {
      resp_type.client_id =  index;
      dsat_client_info[index].urc_resp_cb_func(resp_type,
                                  response,
                                  dsat_client_info[index].urc_client_data);
    }
  }
  return;
}/*dsatclient_send_urc_response*/

/*===========================================================================

FUNCTION DSAT_CLIENT_SEND_RESPONSE

DESCRIPTION
  This function is used to send the response to the client interface which 
  has issued an AT command through dsatclient_client_send_at_cmd. 

DEPENDENCIES
  The client  must have registered to ATCoP through 
  dsatclient_register_client. 

RETURN VALUE
  NONE

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatclient_send_response
( 
  dsm_item_type             * tx_ptr, /* response data in dsm item */
  ds3g_siolib_status_e_type *ret_val,  /* return value of success/failure*/
  boolean       rearm_autodetect /*  Rearm detect status */
)
{
/*
  'response_type' is decided based on rearm_autodetect state, 
   which is set when final response is sent out.
*/


#define  UPDATE_RESP_TYPE() \
{ \
  if(TRUE == rearm_autodetect) \
  { \
     if(DSAT_RESP_NONE == dsat_client_info[client_id].prev_response_type) \
     { \
       resp_type.response_type =  DSAT_RESP_COMPLETE; \
     } \
     else \
     { \
       resp_type.response_type =  DSAT_RESP_END; \
     } \
  }else \
  { \
    if(DSAT_RESP_NONE == dsat_client_info[client_id].prev_response_type) \
    { \
      resp_type.response_type =  DSAT_RESP_START; \
    } \
    else \
    { \
      resp_type.response_type =  DSAT_RESP_INTERMEDIATE; \
    } \
  } \
  dsat_client_info[client_id].prev_response_type = resp_type.response_type;\
} 
/*-------------------------------------------------------------------------------*/
  char res_buff[MAX_RESP_SIZE] = {0};
  dsat_ext_client_id_type client_id;
  uint16 res_buff_len = 0;
  dsat_data_s_type response;
  dsat_sw_api_resp_s_type   resp_type;

   if (tx_ptr == NULL || ret_val == NULL)
   { 
     return; 
   }

  /*-------------------------------------------------------
           Initalize the required variables 
  -------------------------------------------------------*/

  *ret_val = DS3G_SIOLIB_OK;
  client_id= ds3g_siolib_get_active_ext_client();

  res_buff_len = dsm_pullup(&tx_ptr, res_buff, MAX_RESP_SIZE -1);

  res_buff[res_buff_len]='\0';


  response.buffer = &res_buff[0];
  response.length = res_buff_len;
  
  resp_type.pass_fail = TRUE;
  resp_type.client_id =  client_id;
  
  if ( !(VALIDATE_CLIENT_ID(client_id) )&&(dsat_client_info[client_id].cb_func != NULL) )
  {
    UPDATE_RESP_TYPE();
    dsat_client_info[client_id].cb_func( 
                                  resp_type,
                                  response,
                                  dsat_client_info[client_id].client_data);
  }
  else
  {
    dsm_free_packet(&tx_ptr);
    return;
  } 
  /*-------------------------------------------------------
                Perform CLEAN UP 
  -------------------------------------------------------*/
  dsm_free_packet(&tx_ptr);
  if(TRUE == rearm_autodetect)
  {
  /* clear active client _id as the entire processing is done by now */
    ds3g_siolib_set_active_ext_client ( DSAT_EXT_CLIENT_ID_INVALID );
  /* Reset the variables associated with the command processed */
    dsat_client_info[client_id].cmd_len=0;
    dsat_client_info[client_id].cb_func= NULL;
    dsat_client_info[client_id].pending_flag = FALSE;
    dsat_client_info[client_id].client_data = NULL;
    dsat_client_info[client_id].prev_response_type = DSAT_RESP_NONE;

    /* Free the alllocated buffer for the command */
    dsatutil_free_memory_ext((void **) &dsat_client_info[client_id].at_cmd_ptr);
    dsat_client_info[client_id].at_cmd_ptr = NULL;
  }
}/* dsatclient_send_response */

/*===========================================================================

FUNCTION DSATCLIENT_DEREGISTER_CLIENT

DESCRIPTION
  This function allows client to disable all callbacks and release the
  client ID so that it can be allocated to other clients.  Used when a
  client does not want to send in any more AT commands. 
  
DEPENDENCIES
  The client must have previously called dsatclient_register_client_id()

RETURN VALUE
  TRUE on successful operation, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatclient_deregister_client
(
  dsat_ext_client_id_type client_id  /* Client id to be deregistered */
)
{
  boolean result = TRUE;
  /*-------------------------------------------------------
                Validate the client
  -------------------------------------------------------*/
  if ( VALIDATE_CLIENT_ID(client_id) )
  {
    DS_AT_MSG1_MED ("Invalid Client ID %d", client_id);
    return FALSE;
  }
  /*-------------------------------------------------------
                      Critical Section 
  -------------------------------------------------------*/
  rex_enter_crit_sect(&dsat_client_crit_sect);
  {
    if ( TRUE != dsat_client_info[client_id].reg )
    {
      DS_AT_MSG1_MED("Client %d is not a registred client",client_id);
      result =  FALSE;
    }else
    {
    /*-------------------------------------------------------
                 Clean up the clients associated data 
    -------------------------------------------------------*/
      dsat_client_info[client_id].reg = FALSE;
      dsat_client_info[client_id].cb_func = NULL;
      dsat_client_info[client_id].cmd_len = 0;
      dsat_client_info[client_id].pending_flag = FALSE;
      dsat_client_info[client_id].client_data = NULL;
      dsat_client_info[client_id].urc_reg = FALSE;
      dsat_client_info[client_id].urc_resp_cb_func = NULL;
      dsat_client_info[client_id].urc_client_data  = NULL;
      dsat_client_info[client_id].prev_response_type = DSAT_RESP_NONE;

      /*Free the buffer for the command if allocated*/
      dsatutil_free_memory_ext((void **) &dsat_client_info[client_id].at_cmd_ptr);
      dsat_client_info[client_id].at_cmd_ptr = NULL;
    }
  }
  rex_leave_crit_sect(&dsat_client_crit_sect);
  return result;
}/* dsatclient_deregister_client */

/*===========================================================================

FUNCTION DSAT_CLIENT_SERVICE_EXT_CLIENTS

DESCRIPTION
  This function checks the dsat_client_info[client_id].pending_flag to see 
  which client we  will process next. Round Robin Algorithm is used here. 
  The first client to be processed is the one next to the client just processed.
  This ensures fairness by not allowing a single client to hog the ATCOP engine.

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatclient_service_ext_clients ( void )
{
  int client_id = 0;
  dsat_ext_client_id_type curr_client_id = DSAT_EXT_CLIENT_ID_INVALID;
  dsat_ext_client_id_type max_client = DSAT_EXT_CLIENT_ID_MAX;
  uint8 loop = 0;

  /* Ger current client id */
  curr_client_id = ds3g_siolib_get_active_ext_client();
  /*------------------------------------------------------------ 
    Loop through like a round robin algo to check which next
     client has the valid data that needs to be processed. 

     We need to loop twice to really complete all the clients 
  --------------------------------------------------------------*/
  for(loop=0; loop < 2; loop++)
  {
    for(client_id = ++curr_client_id; client_id < max_client; client_id++) 
    {
     if(dsat_client_info[client_id].pending_flag == TRUE)
     {
       DS_AT_MSG1_HIGH("Processing queued client_id = %d",client_id);
       dsatclient_process_send_at_cmd(
                          dsat_client_info[client_id].at_cmd_ptr,
                          dsat_client_info[client_id].cmd_len,
                          client_id,
                          dsat_client_info[client_id].cb_func,
                          dsat_client_info[client_id].client_data);
       dsat_client_info[client_id].pending_flag= FALSE;
       ds3g_siolib_set_active_ext_client ( DSAT_CLIENT_TYPE_INVALID );
       return;
     } /* if pending flag */
    } /* for loop */
    client_id = (dsat_ext_client_id_type)DSAT_CLIENT_TYPE_INVALID;
    max_client = ds3g_siolib_get_active_ext_client() + 1;
  }

  ds3g_siolib_set_active_ext_client ( DSAT_EXT_CLIENT_ID_INVALID );
}/* dsatclient_service_ext_clients */

/*===========================================================================

FUNCTION DSATCLIENT_IS_CMD_SUPPORTED

DESCRIPTION
  This function checks if the command received from external client
  is supported or not.

DEPENDENCIES
  None

RETURN VALUE
  FALSE: If command is not in the supported list.
  TRUE: If command is in the supported list.

SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsatclient_is_cmd_supported
( 
  const char *in_cmd
)
{
  uint8 index = 0;
  
  char unsupported_at_cmds[][8]={{"D"},{"S"},{"O"},{"E"},{"Q"},{"X"},{"V"},{"&C"},{"&D"},{"+IPR"}
                                 ,{"+ICF"},{"+IFC"},{"+ILRR"},{"T"},{"P"},{"A"},{"H"},{"L"},{"M"},{"+CHUP"},{""}};

  if ( in_cmd == NULL )return FALSE;
  
  for (;unsupported_at_cmds[index][0] != '\0';++index)
  {
    if ( dsatutil_strncmp_ig_sp_case((const byte *)in_cmd,
                                     (const byte *)unsupported_at_cmds[index],
                                     strlen(unsupported_at_cmds[index])) == 0 )
    {
      DS_AT_MSG0_MED("Received a unsupported command ");
      return FALSE;
    }
  }

  return TRUE;
}/* dsatclient_is_cmd_supported */

#endif /* FEATURE_DSAT_EXT_CLIENT_SUPPORT */

/*===========================================================================

FUNCTION DSATCLIENT_DEREGISTER_FWD_CMD

DESCRIPTION
  This function handles the pending command response in forward AT command
  de-registration.

DEPENDENCIES
  None
  
RETURN VALUE
  DSAT_ASYNC_EVENT  -- if no client command processing is pending 
  DSAT_ERROR        -- if command response is still pending for this
                       client.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatclient_deregister_fwd_cmd
(
  int32 client_id
)
{
  dsat_result_enum_type result_code = DSAT_ASYNC_EVENT;

  /* Check if any processing is pending for this client*/
  if((dsatcmdp_processing_fwd_cmd.is_pending == TRUE) &&
     (dsatcmdp_processing_fwd_cmd.client_id == client_id))
  {
    /* Set the port and result code type not - UNSOLICITED*/
    ds3g_siolib_set_tx_type( DS3G_SIOLIB_UNSOLICTED_NONE,
                             dsatcmdp_processing_fwd_cmd.port );
    
    /* Indicate abort command processing has completed if not disabled.
       This function performs no action if abort command processing is
       not currently in progress. */
    if ( dsatcmdp_end_abort_in_send_rsp )
    {
      dsatcmdp_command_abort_complete_with_port( 
                             dsatcmdp_processing_fwd_cmd.port );
    }

    /* Clear the pending flag,client and port */
    dsatcmdp_processing_fwd_cmd.is_pending = FALSE;
    dsatcmdp_processing_fwd_cmd.port = DS3G_SIOLIB_DATA_PORT;
    dsatcmdp_processing_fwd_cmd.client_id = DSAT_INVALID_CLIENT_ID;
    memset(dsatcmdp_processing_fwd_cmd.cmd_name, 0x0, MAX_CMD_SIZE);
    DS_AT_MSG0_ERROR("De-registration when a command processing has not been completed");
    result_code = DSAT_ASYNC_EVENT;
  }

  return result_code;
}/* dsatclient_deregister_fwd_cmd */

/*===========================================================================

FUNCTION DSATCLIENT_EXT_SEND_RESPONSE

DESCRIPTION
  This function is called to post the Command response from an
  external client when the command processing is complete. No action
  will be taken if no command is pending in forwarded state.
  
DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void
dsatclient_ext_send_response
(
  boolean                                    cmd_status,   /* CMD status      */
  int32                                      client_id,    /* Client id       */
  dsat_client_result_enum_type               result_code,  /* result code     */
  dsat_fwd_resp_enum_type                    resp_type,    /* resp_type       */
  dsm_item_type                             *resp_buff_ptr,/* response buffer */
  uint32                                     atcop_info    /* Atcop Info      */
)
{
  ds_cmd_type            *cmd_ptr;
  ds_at_ext_client_resp_s_type *ext_at_resp_ptr = NULL;

  DS_AT_MSG5_HIGH("dsatclient_ext_send_response(): cmd_status = %d, client_id = %d, "
                  "result_code = %d, resp_type = %d, atcop_info = %d",
                  cmd_status, client_id, result_code, resp_type, atcop_info);

  if ( (uint32)DS3G_SIOLIB_PORT_MAX == atcop_info )
  {
    DS_AT_MSG0_HIGH("Posted on behalf of QMI DMS, ignore the response");
    dsm_free_packet(&resp_buff_ptr);
    return;
  }

  /* Allocate a DS task comamnd buffer */
  cmd_ptr = dsat_get_cmd_buf(sizeof(ds_at_ext_client_resp_s_type), FALSE);
  ext_at_resp_ptr = cmd_ptr->cmd_payload_ptr;

  cmd_ptr->hdr.cmd_id = DS_CMD_ATCOP_EXT_CMD_RESP;
  ext_at_resp_ptr->cmd_status    = cmd_status;
  ext_at_resp_ptr->client_id     = client_id;
  ext_at_resp_ptr->resp_type     = resp_type;
  ext_at_resp_ptr->result_code   = result_code;
  ext_at_resp_ptr->resp_buff_ptr = resp_buff_ptr;
  ext_at_resp_ptr->atcop_info    = atcop_info;
  ds_put_cmd(cmd_ptr);
}/* dsatclient_ext_send_response */

/*===========================================================================
FUNCTION DSATCLIENTI_EXT_SEND_URC

DESCRIPTION
  This function is called to post the URC from an
  external client when the command processing is complete. No action
  will be taken if no command is pending in forwarded state.
  
DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void
dsatclienti_ext_send_urc
(
  uint8                                    cmd_status,   /* CMD status      */
  int32                                      client_id,    /* Client id       */
  dsm_item_type                             *resp_buff_ptr/* response buffer */
)
{
  ds_cmd_type *cmd_ptr;
  ds_at_ext_client_urc_s_type *ext_at_urc_ptr = NULL;

  /* Allocate a DS task comamnd buffer */
  cmd_ptr  = dsat_get_cmd_buf(sizeof(ds_at_ext_client_urc_s_type), TRUE);

  if ( NULL != cmd_ptr )
  {
    DS_AT_MSG1_HIGH("In dsatclient_ext_send_urc cmd_status= (%d)", (int)cmd_status);
    
    cmd_ptr->hdr.cmd_id = DS_CMD_ATCOP_EXT_CMD_URC;
    ext_at_urc_ptr = cmd_ptr->cmd_payload_ptr;
    ext_at_urc_ptr->cmd_status    = cmd_status;
    ext_at_urc_ptr->client_id     = client_id;
    ext_at_urc_ptr->resp_buff_ptr = resp_buff_ptr;
    ds_put_cmd(cmd_ptr);
  }
}/* dsatclienti_ext_send_urc */

/*===========================================================================

FUNCTION DSATCLIENT_EXT_AT_RESP_HANDLER

DESCRIPTION
  This function is the handler to process the external client's AT command 
  response and forward it to correct port.
  
DEPENDENCIES
  None 

RETURN VALUE
  DSAT_ASYNC_EVENT

SIDE EFFECTS
  This will clear the curent async command processing.

===========================================================================*/
dsat_result_enum_type  dsatclient_ext_at_resp_handler 
(
  ds_cmd_type          *cmd_ptr              /* DS Command pointer         */
)
{
  ds_at_ext_client_resp_s_type* client_resp_ptr = dsat_get_cmd_payload_ptr(cmd_ptr);
  dsat_result_enum_type result     = DSAT_ASYNC_EVENT;
  ds3g_siolib_port_e_type  port    = DS3G_SIOLIB_PORT_NONE;

  DS_AT_MSG3_ERROR("In dsatclient_ext_at_resp_handler port_id=%d,cmd_status =%d,"
    "result_code = %d", client_resp_ptr->atcop_info,(int)client_resp_ptr->cmd_status,
    (int)client_resp_ptr->result_code);
  
  if (dsatcmdp_processing_fwd_cmd.is_pending == TRUE)
  {
    /* check the client_id validation*/
    if(dsatcmdp_processing_fwd_cmd.client_id != client_resp_ptr->client_id)
    {
      ERR_FATAL("Invalid client_id,processing client(%d),recieved client (%d)",
         dsatcmdp_processing_fwd_cmd.client_id,client_resp_ptr->client_id,0);
    }
    port = (ds3g_siolib_port_e_type)(client_resp_ptr->atcop_info);
  
    if(( port >= DS3G_SIOLIB_PORT_MAX ) ||( port <= DS3G_SIOLIB_DATA_PORT ))
    {
      ERR_FATAL("Invalid atcop_info (%d)",(int)port,0,0);
    }

    /* Clear the pending flag,client and port if resp_type is COMPLETE or END*/ 
    if ((client_resp_ptr->resp_type == DSAT_RESP_COMPLETE) ||
        (client_resp_ptr->resp_type == DSAT_RESP_END) ||
        (client_resp_ptr->cmd_status == DSAT_FAILURE) ||
        (client_resp_ptr->result_code == DSAT_CLIENT_RESULT_ERROR))
    {
      /* Indicate abort command processing has completed if not disabled.
         This function performs no action if abort command processing is
         not currently in progress. */
      if ( dsatcmdp_end_abort_in_send_rsp )
      {
        dsatcmdp_command_abort_complete_with_port( port );
      }

      dsatcmdp_processing_fwd_cmd.is_pending = FALSE;
      dsatcmdp_processing_fwd_cmd.port = DS3G_SIOLIB_DATA_PORT;
      dsatcmdp_processing_fwd_cmd.client_id = DSAT_INVALID_CLIENT_ID;
      memset(dsatcmdp_processing_fwd_cmd.cmd_name, 0x0, MAX_CMD_SIZE);
    }
    /* Set the port and result code type not - UNSOLICITED*/
    ds3g_siolib_set_tx_type( DS3G_SIOLIB_UNSOLICTED_NONE, port );

    /* It needs to be re-visted when multiple client processing is handled */
    /* Send ERROR result code to terminal and free the dsm_item if non_NULL 
       for cmd_status FALSE.
    */
    if ( client_resp_ptr->cmd_status == DSAT_FAILURE )
    {
      DS_AT_MSG0_HIGH("CMD status is DSAT_FAILURE");

      if ( client_resp_ptr->resp_buff_ptr != NULL )
      {
        /* Free the dsm item pointer allocated */
        dsm_free_packet(&(client_resp_ptr->resp_buff_ptr));
      }
      return DSAT_ERROR;
    }
    /* Currently we are ignoring response type. This field is added 
       for future purpose.
    */

    /* These result codes handling is required for CMD line processing. 
       In case of OK result code,  CLIENT will not append OK to the RESULT
       instead will set this result code and based on it we will continue 
         command line processing until end of line is reached or ERROR and report 
       the result accordingly. In case of intermediate and error scenarios
       we expect the response buffer to contain the exact ERROR. if it is kept as 
       NULL we will return ERROR to TE.
    */
     
    switch (client_resp_ptr->result_code)
    {
      case DSAT_CLIENT_RESULT_ERROR:
      case DSAT_CLIENT_RESULT_OTHER:
        if ( client_resp_ptr->resp_buff_ptr == NULL )
        {
          result = DSAT_ERROR;
        }
        else if (client_resp_ptr->resp_buff_ptr->used == 0 )
        {
          /* Free the dsm item pointer allocated */
          dsm_free_packet(&(client_resp_ptr->resp_buff_ptr));
          result = DSAT_ERROR;
        }
        else
        {
          /* in this case we will report the data populated in response buffer.
             It is considered as final and error scenario or NO RSP scenario.
          */
          result = DSAT_CMD_ERR_RSP;
        }
        break;
      case DSAT_CLIENT_RESULT_OK:
        if((client_resp_ptr->resp_type == DSAT_RESP_COMPLETE )||
          (client_resp_ptr->resp_type == DSAT_RESP_END ))
        {
        result = DSAT_OK;
        }
        else
        {
          result = DSAT_ASYNC_EVENT;
        }
        break;
      default:
        DS_AT_MSG0_ERROR("Invalid result code-clearing res_buffer, return ERROR");
        if ( client_resp_ptr->resp_buff_ptr != NULL )
        {
          /* Free the dsm item pointer allocated */
          dsm_free_packet(&(client_resp_ptr->resp_buff_ptr));
        }
        result = DSAT_ERROR;
        break;
    }
  } /* if (dsatcmdp_processing_fwd_cmd.is_pending == TRUE) */
  else
  {
    /* no command is pending free the dsm_item if present
       and send result as DSAT_ASYNC_EVENT and will not do validation checks 
    */
    if ( client_resp_ptr->resp_buff_ptr != NULL )
    {
      /* Free the dsm item pointer allocated */
      dsm_free_packet(&(client_resp_ptr->resp_buff_ptr));
    }
    DS_AT_MSG0_ERROR("No AT Command forwarding is pending, ignoring");
    return DSAT_ASYNC_EVENT;
  }
  if( client_resp_ptr->resp_buff_ptr != NULL )
  {
    /* send the response to the user */
    dsatcmdp_handle_async_cmd_rsp(client_resp_ptr->resp_buff_ptr,
                                  DSAT_INTERMEDIATE_RSP);
  }
  DS_AT_MSG1_ERROR("Result code = %d", (int)result);
  return result;
}/* dsatclient_ext_at_resp_handler */

/*===========================================================================

FUNCTION DSATCLIENT_EXT_AT_URC_HANDLER

DESCRIPTION
  This function is the handler to process the external client's AT command 
  response and forward it to correct port.
  
DEPENDENCIES
  None 

RETURN VALUE
  DSAT_ASYNC_EVENT

SIDE EFFECTS
  This will clear the curent async command processing.

===========================================================================*/
dsat_result_enum_type  dsatclient_ext_at_urc_handler 
(
  ds_cmd_type          *cmd_ptr              /* DS Command pointer         */
)
{
  ds_at_ext_client_urc_s_type  *client_urc_ptr = dsat_get_cmd_payload_ptr(cmd_ptr);

  DS_AT_MSG1_HIGH("In dsatclient_ext_at_urc_handler: status = %d ",
    client_urc_ptr->cmd_status);

  if(( client_urc_ptr->resp_buff_ptr != NULL )&& (client_urc_ptr->cmd_status <= DSAT_COMPLETE_RSP))
  {
    /* send the response to the user */
    dsatcmdp_send_urc(client_urc_ptr->resp_buff_ptr, DSAT_INTERMEDIATE_RSP);
  }
  else
  {
    DS_AT_MSG1_ERROR("In valid cmd_status %d ",client_urc_ptr->cmd_status);

    dsm_free_packet(&(client_urc_ptr->resp_buff_ptr));
  }
  return DSAT_ASYNC_EVENT;
}/* dsatclient_ext_at_resp_handler */


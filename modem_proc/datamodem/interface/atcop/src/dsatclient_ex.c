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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatclient_ex.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/18/15   skc     Fixed compiler warnings.
06/27/14   tk      Initial revision (created file for Dynamic ATCoP).

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
#include "dsatdl.h"
#include "ps_utils.h"


#ifdef FEATURE_MODEM_RCINIT_PHASE2
#include "rcevt.h"
#include "rcecb.h"

#endif /* FEATURE_MODEM_RCINIT_PHASE2 */

/*===========================================================================

                    REGIONAL DEFINITIONS AND DECLARATIONS

===========================================================================*/
  extern boolean dsat_init_completed;
  extern dsat_fwd_at_pending_type dsatcmdp_processing_fwd_cmd;


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/
  #define DSAT_MAX_CLIENT (1)

  /* The table that holds the data for the commands that are fowarded
  */
  dsat_fwd_at_cmd_table_type dsat_fwd_at_cmds[MAX_CMD_LIST];

  /* Current allowed list that can be forwarded to external client 
  */
  LOCAL byte allowed_list[][MAX_CMD_SIZE]={"+CLVL","+CKPD","+CMUT","+CTSA",
                                           "+CBKLT","+CFUN","+CDIS","+CRSL","+CMAR",
                                           "+CSO","+CSS","+CBC","$QCPWRDN",""};

  /* This holds the valid forwarded client_id's that has been regsitered
  */
  int32 dsat_fwd_at_clients[DSAT_MAX_CLIENT]={0};

/*-------------------------- Local Functions -------------------------------*/
/*===========================================================================

FUNCTION CLIENT_VALIDATE_FWD_AT_CMDS

DESCRIPTION
  This function verifies the AT commands requested for forwarding from 
  the client's command queue buffer.

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE on successful operation, FALSE otherwise.
  
SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL boolean client_validate_fwd_at_cmds 
(
  int32                               client_id,      /*  registered client_id  */
  uint8                               num_valid_cmds,/* number of valid commands*/
  const dsat_cmd_list_type            at_cmd_list[]  /* list of AT commands     */
);
/*===========================================================================

FUNCTION CLIENT_UPDATE_FWD_AT_CMDS

DESCRIPTION
  This function updates the internal tables with the forward AT 
  command registration from the client's 
  command queue buffer.

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE on successful operation, FALSE otherwise.
  
SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL boolean client_update_fwd_at_cmds 
(
  int32                              client_id,     /*  registered client_id  */
  uint8                              num_valid_cmds,/*number of valid commands*/
  const dsat_cmd_list_type           at_cmd_list[], /*list of AT commands     */
  dsatclient_ext_at_cmd_exec_cb      ext_at_cmd_exec_cb,/* execution call back*/
  dsatclient_ext_at_cmd_abort_cb     ext_at_abort_cb,  /* Abort call back     */
  void                              *user_info_ptr     /*       Client data   */
);

/*===========================================================================

FUNCTION CLIENT_VALIDATE_FWD_CLIENT_ID

DESCRIPTION
  This function checks for valid client id.

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE   -- When client-id given is a valid client id
  FALSE  -- client id is INVALID
  
SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL boolean client_validate_fwd_client_id 
(
  int32                              client_id  /* Client id */
);

/*===========================================================================

FUNCTION  CLIENT_GET_MAX_ENTRY_ALLOWED

DESCRIPTION
  This function gets the free spaces in cmd table.

DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/
INLINE uint8 client_get_max_entry_allowed 
(
  void
);

/*==========================================================================

FUNCTION DSATCLIENT_REGISTER_FWD_CLIENT

DESCRIPTION
  This function registers the client for forwarding AT commands.
  The client ID will be a non-negative integer

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE on successful operation,
  FALSE otherwise.
  
SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsatclient_register_fwd_client 
(
  dsatclient_cmd_status_cb_type   status_cb,      /*Status callback*/
  void                           *user_info_ptr   /* Client data   */
)
{
  ds_cmd_type            *cmd_buf  = NULL;
  dsat_fwd_at_cmd_s_type *forward_at_cmd = NULL;
  
  if(FALSE == dsat_is_atcop_allowed())
  {
     DS_AT_MSG0_ERROR(" ATcop is not initialized");
     return FALSE;
  }

  if((status_cb == NULL)||(dsat_init_completed == FALSE))
  {
    DS_AT_MSG1_ERROR("Registration failure.Init Status %d ",dsat_init_completed);
    return FALSE;
  }

  /*-----------------------------------------------------------
     Post the command to DS task and return success to client
  ------------------------------------------------------------*/
  cmd_buf  = dsat_get_cmd_buf(sizeof(dsat_fwd_at_cmd_s_type), FALSE);
  forward_at_cmd = cmd_buf->cmd_payload_ptr;

  cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_FWD_CLIENT_REG;
  forward_at_cmd->status_cb = status_cb;
  forward_at_cmd->user_info_ptr = user_info_ptr;
  ds_put_cmd(cmd_buf);

  return TRUE;
}/* dsatclient_register_fwd_client */

/*===========================================================================

FUNCTION DSATCLIENT_DEREGISTER_FWD_CLIENT

DESCRIPTION
  This function deregisters the client of AT command forwarding.

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE   --  When Successfully posted to DS task
  FALSE  --  client id is INVALID and ATCoP initialization is not complete
  
SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsatclient_deregister_fwd_client 
(
  int32           client_id /* Registered client id */
)
{
  ds_cmd_type            *cmd_buf  = NULL;
  dsat_fwd_at_cmd_s_type *forward_at_cmd = NULL;

  if((client_id == 0)||(dsat_init_completed == FALSE))
  {
    DS_AT_MSG1_ERROR("De-Registration failure.Init Status %d ",
                                      dsat_init_completed);
    return FALSE;
  }

  /*-----------------------------------------------------------
     Post the command to DS task and return success to client
  ------------------------------------------------------------*/
  cmd_buf = dsat_get_cmd_buf(sizeof(dsat_fwd_at_cmd_s_type), FALSE);
  forward_at_cmd = cmd_buf->cmd_payload_ptr;

  cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_FWD_CLIENT_DEREG;
  forward_at_cmd->client_id = client_id;
  ds_put_cmd(cmd_buf);

  return TRUE;
}/* dsatclient_deregister_fwd_client */

/*===========================================================================

FUNCTION DSATCLIENT_REGISTER_FWD_AT_CMD

DESCRIPTION
  This function allows client to register the list of commands with ATCOP 
  for forwarding.

DEPENDENCIES
  None

RETURN VALUE
  TRUE on successful operation
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatclient_register_fwd_at_cmd 
(
  int32                            client_id,      /*  registered client_id   */
  uint8                            num_valid_cmds, /* number of valid commands*/
  dsat_cmd_list_type               at_cmd_list[],  /* list of AT commands     */
  dsatclient_cmd_status_cb_type    status_cb,      /*status call back         */
  dsatclient_ext_at_cmd_exec_cb    ext_at_cmd_exec_cb, /* execution call back */
  dsatclient_ext_at_cmd_abort_cb   ext_at_abort_cb,  /* Abort call back       */
  void                            *user_info_ptr     /* user data  pointer    */
)
{
  uint8 valid_index = 0;
  uint8 i = 0;
  ds_cmd_type            *cmd_buf  = NULL;
  dsat_fwd_at_cmd_s_type *forward_at_cmd = NULL;

  /* Data Validation */
  if((status_cb == NULL)||(ext_at_cmd_exec_cb == NULL)||
     (dsat_init_completed == FALSE)||(at_cmd_list == NULL)||
     ( num_valid_cmds == 0 ) || ( num_valid_cmds > MAX_CMD_LIST))
  {
    DS_AT_MSG0_ERROR("Invalid data provided for ATCoP registration");
    return FALSE;
  }

  /* Abortability check */
  while(valid_index < num_valid_cmds)
  {
    if(at_cmd_list[valid_index].is_abortable == TRUE)
    {
      if( ext_at_abort_cb == NULL)
      {
        DS_AT_MSG0_ERROR("Invalid input data");
        return FALSE;
      }
    }

    if((at_cmd_list[valid_index].cmd_buff_len > MAX_CMD_SIZE)||
       (at_cmd_list[valid_index].cmd_buff_len == 0))
    {
      DS_AT_MSG0_ERROR("Invalid command data");
      return FALSE;
    }
    valid_index++;
  }/* while(valid_index < num_valid_cmds) */

  /*-----------------------------------------------------------
     Post the command to DS task and return success to client
  ------------------------------------------------------------*/
  cmd_buf  = dsat_get_cmd_buf(sizeof(dsat_fwd_at_cmd_s_type), FALSE);
  forward_at_cmd = cmd_buf->cmd_payload_ptr;

  cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_FWD_AT_CMD_REG;

  for ( i = 0; i < num_valid_cmds; ++i)
  {
    forward_at_cmd->at_cmd_list[i].cmd_buff_len = 
                                    at_cmd_list[i].cmd_buff_len;
    (void) dsatutil_memscpy((void*)&forward_at_cmd->at_cmd_list[i].cmd_buff[0],
             MAX_CMD_SIZE,
            (void*)&at_cmd_list[i].cmd_buff[0],
            forward_at_cmd->at_cmd_list[i].cmd_buff_len);      
    forward_at_cmd->at_cmd_list[i].is_abortable = 
                                    at_cmd_list[i].is_abortable;
  }
  forward_at_cmd->client_id = client_id;
  forward_at_cmd->num_valid_cmds = num_valid_cmds;
  forward_at_cmd->status_cb = status_cb;
  forward_at_cmd->ext_at_cmd_exec_cb = ext_at_cmd_exec_cb;
  forward_at_cmd->ext_at_abort_cb = ext_at_abort_cb;
  forward_at_cmd->user_info_ptr = user_info_ptr;
  DS_AT_MSG1_MED("Num of valid commands received - %d",num_valid_cmds);
  ds_put_cmd(cmd_buf);

  return TRUE;
}/* dsatclient_register_fwd_at_cmd */

/*===========================================================================
FUNCTION DSATCLIENT_DEREGISTER_FWD_AT_CMD

DESCRIPTION
  This function allows client to deregister the list of commands with ATCOP 
  for forwarding.

DEPENDENCIES
  None

RETURN VALUE
  TRUE on successful operation
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatclient_deregister_fwd_at_cmd 
(
  int32              client_id,      /*  registered client_id   */
  uint8              num_valid_cmds, /* number of valid commands*/
  dsat_cmd_list_type at_cmd_list[]   /* list of AT commands     */
)
{
  uint8 valid_index = 0;
  uint8 i = 0;
  ds_cmd_type * cmd_buf = NULL;
  dsat_fwd_at_cmd_s_type *fwd_at_cmd_ptr;

  /* Data Validation */
  if((dsat_init_completed == FALSE)||(at_cmd_list == NULL)||
     ( num_valid_cmds == 0 ) || ( num_valid_cmds > MAX_CMD_LIST))
  {
    DS_AT_MSG0_ERROR("Invalid data provided for ATCoP deregistration");
    return FALSE;
  }

  /* Abortability check */
  while(valid_index < num_valid_cmds)
  {
    if((at_cmd_list[valid_index].cmd_buff_len > MAX_CMD_SIZE)||
       (at_cmd_list[valid_index].cmd_buff_len == 0))
    {
      DS_AT_MSG0_ERROR("Invalid command data");
      return FALSE;
    }
    valid_index++;
  }/* while(valid_index < num_valid_cmds) */

  /*-----------------------------------------------------------
     Post the command to DS task and return success to client
  ------------------------------------------------------------*/
  cmd_buf = dsat_get_cmd_buf(sizeof(dsat_fwd_at_cmd_s_type), FALSE);
  fwd_at_cmd_ptr = cmd_buf->cmd_payload_ptr;

  cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_FWD_CLIENT_DEREG;
  for ( i = 0; i < num_valid_cmds; ++i)
  {
    fwd_at_cmd_ptr->at_cmd_list[i].cmd_buff_len = 
                                  at_cmd_list[i].cmd_buff_len;
    (void) dsatutil_memscpy((void*)&fwd_at_cmd_ptr->at_cmd_list[i].cmd_buff[0],
             MAX_CMD_SIZE,
            (void*)&at_cmd_list[i].cmd_buff[0],
            fwd_at_cmd_ptr->at_cmd_list[i].cmd_buff_len);
    fwd_at_cmd_ptr->at_cmd_list[i].cmd_buff[fwd_at_cmd_ptr->at_cmd_list[i].cmd_buff_len] = '\0';
    DS_AT_MSG_SPRINTF_3_ERROR("cmd name = %s, %s, %d",
         (void*)&at_cmd_list[i].cmd_buff[0],
         (void*)&fwd_at_cmd_ptr->at_cmd_list[i].cmd_buff[0],
         fwd_at_cmd_ptr->at_cmd_list[i].cmd_buff_len);

    fwd_at_cmd_ptr->at_cmd_list[i].is_abortable = 
                                    at_cmd_list[i].is_abortable;
  }
  fwd_at_cmd_ptr->client_id = client_id;
  fwd_at_cmd_ptr->num_valid_cmds = num_valid_cmds;
  DS_AT_MSG1_MED("Num of valid commands received - %d",num_valid_cmds);
  ds_put_cmd(cmd_buf);
  return TRUE;
}/* dsatclient_deregister_fwd_at_cmd */

/*===========================================================================

FUNCTION DSATCLIENT_REGISTER_FWD_CLIENT_HANDLER

DESCRIPTION
  This function process the forward AT command registration from the client's 
  command queue buffer.

DEPENDENCIES
  None
  
RETURN VALUE
  DSAT_ASYNC_EVENT
  
SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatclient_register_fwd_client_handler 
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  uint8 client =0;
  uint32 client_id = 0;
  boolean generate_client_id = FALSE;
  dsat_fwd_at_cmd_s_type *forward_at_cmd = dsat_get_cmd_payload_ptr(cmd_ptr);

  while(client < DSAT_MAX_CLIENT)
  {
    if(dsat_fwd_at_clients[client] == DSAT_INVALID_CLIENT_ID )
    {
      generate_client_id = TRUE;
      break;
    }
    client++;
  }/* while(client < DSAT_MAX_CLIENT) */

  if(generate_client_id == TRUE)
  {
    ps_utils_generate_rand_num( &client_id, sizeof(uint32) );

    /* client_validate_fwd_client_id returns false if
       client id is valid and not assigned already */

    if( (client_id == 0) || (FALSE != client_validate_fwd_client_id(client_id)) )
    {
      ERR_FATAL("Invalid client id",0,0,0);
    }
    DS_AT_MSG1_HIGH("New client id %d", client_id);
    dsat_fwd_at_clients[client] = client_id;
  }
    /* Update client of registration status */
  forward_at_cmd->status_cb(generate_client_id,
                     client_id,
                     forward_at_cmd->user_info_ptr);

  return DSAT_ASYNC_EVENT;
}/* dsatclient_register_fwd_client_handler */

/*===========================================================================

FUNCTION DSATCLIENT_DEREGISTER_FWD_CLIENT_HANDLER

DESCRIPTION
  This function process the forward AT command de-registration from the client's 
  command queue buffer.

DEPENDENCIES
  None
  
RETURN VALUE
  DSAT_ASYNC_EVENT  -- if no client command processing is pending 
  DSAT_ERROR        -- if command response is still pending for this
                       client.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatclient_deregister_fwd_client_handler 
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  uint8                  client = 0;
  uint8                  index = 0;
  dsat_result_enum_type  result_code = DSAT_ASYNC_EVENT;
  dsat_fwd_at_cmd_s_type *forward_at_cmd = dsat_get_cmd_payload_ptr(cmd_ptr);
  int32                  client_id = 0;

  client_id = forward_at_cmd->client_id;

  if (DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatclient_deregister_fwd_cmd_fp))
  {
    result_code = dsatdl_vtable.dsatclient_deregister_fwd_cmd_fp(client_id);
  }

  /* Clear the client id from client id table*/
  while(client < DSAT_MAX_CLIENT)
  {
    if(dsat_fwd_at_clients[client] == client_id)
    {
      dsat_fwd_at_clients[client] = DSAT_INVALID_CLIENT_ID;
      break;
    }
    client++;
  }

  index =0;

  /* Clear the registered commands associated with this client */
  while(index < MAX_CMD_LIST)
  {
    
    
#ifdef FEATURE_MODEM_RCINIT_PHASE2

 if ( 0 ==  dsatutil_strcmp_ig_sp_case( (const byte *)dsat_fwd_at_cmds[index].cmd_name, (const byte *)"+CFUN") )
    {
      (void)rcecb_unregister_name("SYSM:FWD:SHUTDOWN", (void *)dsat_reset_cmd);
    }
    
    if ( 0 ==  dsatutil_strcmp_ig_sp_case( (const byte *)dsat_fwd_at_cmds[index].cmd_name, (const byte *)"$QCPWRDN"))
    {
      (void)rcecb_unregister_name("SYSM:FWD:POWEROFF", (void *)dsat_shutdown_cmd);
    }
#endif 
    
    if(dsat_fwd_at_cmds[index].client_id == client_id)
    {
       memset((void *)&dsat_fwd_at_cmds[index],0x0,
                sizeof(dsat_fwd_at_cmds[index]));
    }
    index++;
  }
  
  return result_code;
}/* dsatclient_deregister_fwd_client_handler */ 

/*===========================================================================

FUNCTION DSATCLIENT_REGISTER_FWD_AT_CMD_HANDLER

DESCRIPTION
  This function process the forward AT command registration from the client's 
  command queue buffer.

DEPENDENCIES
  None
  
RETURN VALUE
  DSAT_ASYNC_EVENT
  
SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatclient_register_fwd_at_cmd_handler 
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  boolean result                   = DSAT_FAILURE;
  uint8 i                          = 0;
  dsat_fwd_at_cmd_s_type *forward_at_cmd = dsat_get_cmd_payload_ptr(cmd_ptr);

  if( FALSE == client_validate_fwd_client_id(
                      forward_at_cmd->client_id))
  {
    DS_AT_MSG0_ERROR("Invalid client id");
  }
  else
  {
    /* Check whether the commands are valid */
    result = client_validate_fwd_at_cmds( 
             forward_at_cmd->client_id,
             forward_at_cmd->num_valid_cmds,
             (const dsat_cmd_list_type *)forward_at_cmd->at_cmd_list
             );

    DS_AT_MSG1_HIGH("Validation done result = %d", (int)result);

    /* If validation is success update the client table and send status call
       back function with DSAT_SUCCESS..
       If validation fails send status call back with DSAT_FAILURE.
    */
    if(result == DSAT_SUCCESS)
    {
        /* Update the forwarded AT commands table*/
      result = client_update_fwd_at_cmds(
         forward_at_cmd->client_id,
         forward_at_cmd->num_valid_cmds,
         (const dsat_cmd_list_type *)forward_at_cmd->at_cmd_list,
         forward_at_cmd->ext_at_cmd_exec_cb,
         forward_at_cmd->ext_at_abort_cb,
         forward_at_cmd->user_info_ptr
         );
    }
  }

  /* Update client of registration status */
  DS_AT_MSG1_MED("Result = %d",(int)result);
  forward_at_cmd->status_cb(result,
       forward_at_cmd->client_id,
       forward_at_cmd->user_info_ptr);


  for( i=0; i<MAX_CMD_LIST; i++ )
  {

  #ifdef FEATURE_MODEM_RCINIT_PHASE2

  if( dsat_fwd_at_cmds[i].client_id == forward_at_cmd->client_id )
  {  
    if ( 0 ==  dsatutil_strcmp_ig_sp_case( (const byte *)dsat_fwd_at_cmds[i].cmd_name, (const byte *)"+CFUN") )
    {
      (void)rcecb_register_name("SYSM:FWD:SHUTDOWN", (void *)dsat_reset_cmd);
    }
    
    if ( 0 ==  dsatutil_strcmp_ig_sp_case( (const byte *)dsat_fwd_at_cmds[i].cmd_name, (const byte *)"$QCPWRDN"))
    {
      (void)rcecb_register_name("SYSM:FWD:POWEROFF", (void *)dsat_shutdown_cmd);
    }
  }  
  #endif 
  
  }

  return DSAT_ASYNC_EVENT;

}/* dsatclient_register_fwd_at_cmd_handler  */

/*===========================================================================

FUNCTION DSATCLIENT_DEREGISTER_FWD_AT_CMD_HANDLER

DESCRIPTION
  This function process the forward AT command deregistration from the
  client's command queue buffer.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_EVENT

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatclient_deregister_fwd_at_cmd_handler
(
  ds_cmd_type *        cmd_ptr              /* DS Command pointer */
)
{
  dsat_result_enum_type   result = DSAT_ASYNC_EVENT;
  dsat_fwd_at_cmd_s_type *fwd_at_cmd_ptr = dsat_get_cmd_payload_ptr(cmd_ptr);
  uint8                   index1 = 0;
  uint8                   index2 = 0;

   if( FALSE == client_validate_fwd_client_id(fwd_at_cmd_ptr->client_id) ||
       MAX_CMD_LIST <= fwd_at_cmd_ptr->num_valid_cmds )
   {
     DS_AT_MSG0_ERROR("Invalid client id");
   }
   else
   {
    if (DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatclient_deregister_fwd_cmd_fp))
    {
      /* Check if processing is pending for a command in this list */
      for (index1 = 0; index1 < fwd_at_cmd_ptr->num_valid_cmds; index1++)
      {
        if (0 == dsatutil_strcmp_ig_sp_case(
                   (const byte *)fwd_at_cmd_ptr->at_cmd_list[index1].cmd_buff,
                   (const byte *)dsatcmdp_processing_fwd_cmd.cmd_name))
        {
          result = dsatdl_vtable.dsatclient_deregister_fwd_cmd_fp(fwd_at_cmd_ptr->client_id);
          break;
        }
      }
    }

    /*No check for index1 ? and dont need to stop the processing command?*/
    
    /* Deregister each command in the list if it was registered for this client */
    for (index1 = 0; index1 < fwd_at_cmd_ptr->num_valid_cmds; index1++)
    {
      /* Check if the command is present in the registered commands table */
      for (index2 = 0; index2 < MAX_CMD_LIST; index2++)
      {
        DS_AT_MSG4_ERROR("index1 = %d, index2 = %d, client id = %d, %d",
          index1, index2, fwd_at_cmd_ptr->client_id, dsat_fwd_at_cmds[index2].client_id);
        DS_AT_MSG_SPRINTF_2_ERROR("cmd name = %s, %s",
          (const byte *)fwd_at_cmd_ptr->at_cmd_list[index1].cmd_buff,
          (const byte *)dsat_fwd_at_cmds[index2].cmd_name);
        /* Check if the command was registered for this client */
        if ((fwd_at_cmd_ptr->client_id == dsat_fwd_at_cmds[index2].client_id) &&
            (0 == dsatutil_strcmp_ig_sp_case(
                    (const byte *)fwd_at_cmd_ptr->at_cmd_list[index1].cmd_buff,
                    (const byte *)dsat_fwd_at_cmds[index2].cmd_name)))
        {
#ifdef FEATURE_MODEM_RCINIT_PHASE2
          if (0 == dsatutil_strcmp_ig_sp_case(
                     (const byte *) dsat_fwd_at_cmds[index2].cmd_name,
                     (const byte *) "+CFUN"))
          {
            (void)rcecb_unregister_name("SYSM:FWD:SHUTDOWN", (void *)dsat_reset_cmd);
          }
          else if (0 == dsatutil_strcmp_ig_sp_case(
                          (const byte *)dsat_fwd_at_cmds[index2].cmd_name,
                          (const byte *) "$QCPWRDN"))
          {
            (void)rcecb_unregister_name("SYSM:FWD:POWEROFF", (void *)dsat_shutdown_cmd);
          }
#endif
          memset(&dsat_fwd_at_cmds[index2], 0x0, sizeof(dsat_fwd_at_cmds[index2]));
          DS_AT_MSG1_ERROR("removed command at index %d",index2);
          break;
        }
      }
    }
  }

  return result;
} /* dsatclient_deregister_fwd_at_cmd_handler */

/*===========================================================================
FUNCTION CLIENT_FWD_CMD_REQUEST_HANDLER

DESCRIPTION
  This function is the handler function for forward command request. 

DEPENDENCIES
  None
  
RETURN VALUE
  DSAT_ERROR : if there was any problem .
  DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
======================================================================*/

LOCAL dsat_result_enum_type client_fwd_cmd_request_handler
(
  ds_at_fwd_cmd_request_s_type *fwd_cmd_req, /* Reset command request pointer */
  tokens_struct_type           *token_ptr    /* Token pointer */
)
{
  boolean             result     = FALSE;
  uint8               i          = 0;
  dsat_cmd_state_type curr_state;

  memset(&curr_state, 0x00, sizeof(dsat_cmd_state_type));

  for (i = 0; i < MAX_CMD_LIST; i++)
  {
    if ( 0 ==  dsatutil_strcmp_ig_sp_case( (const byte *)token_ptr->name, 
                           (const byte *)&dsat_fwd_at_cmds[i].cmd_name[0]) )
    {
      DS_AT_MSG_SPRINTF_1_HIGH("Found match for QMI RESET client processing cmd=%s ",
                               token_ptr->name);

      dsatclient_state_info_update(&curr_state);

    /* Do not over write any of the dsatcmdp_processing_fwd_cmd
           as it could corrupt the earlier valid data as both AT command and QMI DMS could 
           co-exist. The Port of .DS3G_SIOLIB_PORT_MAX is used as a unique value to identify 
           its belong to QMI DMS request. */

      if ( dsat_fwd_at_cmds[i].ext_at_cmd_exec_cb == NULL )
      {
        ERR_FATAL("Exec_callback function pointer is NULL",0,0,0);
      }
      /* DS3G_SIOLIB_PORT_MAX ==> This is used to to uniquely identify if this is for QMI DMS
          */
      if ( DSAT_SUCCESS == dsat_fwd_at_cmds[i].ext_at_cmd_exec_cb( 
                                       dsatcmdp_at_state,
                                       (const tokens_struct_type *)token_ptr,
                                       curr_state,
                                       (uint32)(int)(DS3G_SIOLIB_PORT_MAX),
                                       dsat_fwd_at_cmds[i].user_info_ptr))
      {
        result = TRUE;
       
      } /* Command forwarding is SUCCESS */
      else
      {
        result = FALSE;
      }
      break;
    }/* End of If .. */
  } /* end of for loop */

  if ((NULL != fwd_cmd_req) && (NULL != fwd_cmd_req->status_func_cb))
  {
    fwd_cmd_req->status_func_cb(result, fwd_cmd_req->user_info_ptr);
  }

  return DSAT_ASYNC_EVENT;
}/* client_fwd_cmd_request_handler */

/*===========================================================================
FUNCTION DSATCLIENT_RESET_CMD_REQUEST_HANDLER

DESCRIPTION
  This function is the handler function for reset request. 

DEPENDENCIES
  None
  
RETURN VALUE
  DSAT_ERROR : if there was any problem .
  DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
======================================================================*/

dsat_result_enum_type dsatclient_reset_cmd_request_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  ds_at_fwd_cmd_request_s_type *fwd_req_cmd = dsat_get_cmd_payload_ptr(cmd_ptr);
  tokens_struct_type token;

  /* Populate the token pointer info related to AT+CFUN=1,1 */
  token.name       = (byte *) "+CFUN";
  token.args_found = 2;
  token.arg[0]     = (byte *) "1";
  token.arg[1]     = (byte *) "1";
  token.op         = (unsigned int)(NA|EQ|AR);

  return client_fwd_cmd_request_handler(fwd_req_cmd, &token);
} /* dsatclient_reset_cmd_request_handler */

/*===========================================================================
FUNCTION DSATCLIENT_SHUTDOWN_CMD_REQUEST_HANDLER

DESCRIPTION
  This function is the handler function for shutdown request. 

DEPENDENCIES
  None
  
RETURN VALUE
  DSAT_ERROR : if there was any problem .
  DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
======================================================================*/

dsat_result_enum_type  dsatclient_shutdown_cmd_request_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  ds_at_fwd_cmd_request_s_type *fwd_req_cmd = dsat_get_cmd_payload_ptr(cmd_ptr);
  tokens_struct_type token;

  /* Populate the token pointer info related to AT$QCPWRDN */
  token.name       = (byte *) "$QCPWRDN";
  token.args_found = 0;
  token.op         = (unsigned int)(NA);

  return client_fwd_cmd_request_handler(fwd_req_cmd, &token);
} /* dsatclient_shutdown_cmd_request_handler */

/*===========================================================================

FUNCTION CLIENT_VALIDATE_FWD_AT_CMDS

DESCRIPTION
  This function verifies the AT commands requested for forwarding from the client's 
  command queue buffer.

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE on successful operation
  FALSE otherwise.
  
SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL boolean client_validate_fwd_at_cmds 
(
  int32                                     client_id,  /* Registered client */
  uint8                                     num_valid_cmds, /*no of commands*/
  const dsat_cmd_list_type                  at_cmd_list[] /* AT command list */
)
{
  uint8 cmd_loop = 0;
  uint8 i =0;
  boolean valid_cmd = FALSE;
  uint8 temp =0;
  uint8 allowed_list_len = ARR_SIZE(allowed_list);
  byte temp_fwd_at_cmd_name[MAX_CMD_SIZE]={0};

  /* Check if the number of valid commands is less than MAX_CMD_LIST */
  if( num_valid_cmds > MAX_CMD_LIST )
  {
    DS_AT_MSG1_ERROR("Num_valid_cmds(%d) is greater than allowed length",
      num_valid_cmds);
    return FALSE;
  }

  /* Get free spaces available */
  temp = client_get_max_entry_allowed();

  if(num_valid_cmds > temp)
  {
    DS_AT_MSG2_ERROR("No space available num_valid_cmds= %d, space available %d",
       num_valid_cmds,temp);
    return FALSE;
  }

  for(cmd_loop = 0; cmd_loop < num_valid_cmds; cmd_loop++)
  {
    /* command length validation */
    if( at_cmd_list[cmd_loop].cmd_buff_len > (MAX_CMD_SIZE-1))
    {
      DS_AT_MSG1_ERROR("Invalid command len %d",at_cmd_list[cmd_loop].cmd_buff_len);
      return FALSE;
    }

    /* cmd name string formation */
    (void) dsatutil_memscpy((void*)&temp_fwd_at_cmd_name[0],MAX_CMD_SIZE,
              (void*)at_cmd_list[cmd_loop].cmd_buff, 
                     MIN(MAX_CMD_SIZE,at_cmd_list[cmd_loop].cmd_buff_len));

    temp_fwd_at_cmd_name[at_cmd_list[cmd_loop].cmd_buff_len]='\0';

    /* Process the entry for validity against allowed list */
    for( i=0,valid_cmd = FALSE;i<( allowed_list_len-1);i++)
    {
      if(0 == dsatutil_strcmp_ig_sp_case((const byte *)temp_fwd_at_cmd_name,
                                         (const byte *)allowed_list[i]))
      {
        valid_cmd = TRUE;
        break;
      }
    }

    /* Even a single entry is invalid reject the request */
    if(valid_cmd == FALSE)
    {
      DS_AT_MSG0_ERROR("Un-supported command received");
      return FALSE;
    }

    /* Check if entry is already present in table */
    for(i=0; i<MAX_CMD_LIST; i++)
    {
      if(!dsatutil_strcmp_ig_sp_case(
                              (const byte *)temp_fwd_at_cmd_name,
                              (const byte *)dsat_fwd_at_cmds[i].cmd_name))
      {
        DS_AT_MSG_SPRINTF_1_ERROR("Command %s is already registered",temp_fwd_at_cmd_name);
        return FALSE;
      }
    }

    /* Check if entry is duplicated in the fwd at cmd sent list */
    for(i=0; i<cmd_loop; i++)
    {
      if(!dsatutil_strncmp_ig_sp_case(
                                 (const byte *)at_cmd_list[i].cmd_buff,
                                 (const byte *)temp_fwd_at_cmd_name,
                                 (uint32)at_cmd_list[i].cmd_buff_len))
      {
        DS_AT_MSG0_ERROR("Duplicated entry provided in registration");
        return FALSE;
      }
    }
  } /* for(cmd_loop = 0 ... */
  
  return TRUE;
}/* client_validate_fwd_at_cmds */

/*===========================================================================

FUNCTION CLIENT_UPDATE_FWD_AT_CMDS

DESCRIPTION
  This function updates the internal tables with the forward AT command 
  registration from the client's command queue buffer.

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE on successful operation
  FALSE otherwise.
  
SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL boolean client_update_fwd_at_cmds 
(
  int32                              client_id,         /* Registered client */
  uint8                               num_valid_cmds,   /*no of commands     */
  const dsat_cmd_list_type            at_cmd_list[],    /*AT command list    */
  dsatclient_ext_at_cmd_exec_cb  ext_at_cmd_exec_cb,    /*Execution handler  */
  dsatclient_ext_at_cmd_abort_cb     ext_at_abort_cb,   /*Abort handler      */
  void                           *user_info_ptr         /*client info        */
)
{
  uint8 index = 0;
  uint8 i =0;
  uint8 j =0;

  for( i=0; i<num_valid_cmds; i++ )
  {
    while(dsat_fwd_at_cmds[j].client_id != DSAT_INVALID_CLIENT_ID)
    {
      j++;
    }
    /* Static fwd cmd table free slot = j,update this row */
    index = j;
    /* Update the table */
    dsat_fwd_at_cmds[index].client_id = client_id;
    (void) dsatutil_memscpy((void*)dsat_fwd_at_cmds[index].cmd_name,
            MAX_CMD_SIZE,(void*)at_cmd_list[i].cmd_buff,
            MIN(MAX_CMD_SIZE,at_cmd_list[i].cmd_buff_len));
    dsat_fwd_at_cmds[index].cmd_name[at_cmd_list[i].cmd_buff_len]='\0';
    dsat_fwd_at_cmds[index].is_abortable = at_cmd_list[i].is_abortable;
    dsat_fwd_at_cmds[index].ext_at_cmd_exec_cb = ext_at_cmd_exec_cb;
    dsat_fwd_at_cmds[index].ext_at_abort_cb = ext_at_abort_cb;
    dsat_fwd_at_cmds[index].user_info_ptr = user_info_ptr;
    DS_AT_MSG_SPRINTF_2_HIGH("Cmd updated at %d = %s",
             index,dsat_fwd_at_cmds[index].cmd_name);

  }
  return TRUE;
}/* client_update_fwd_at_cmds */

/*===========================================================================

FUNCTION CLIENT_VALIDATE_FWD_CLIENT_ID

DESCRIPTION
  This function checks for valid client id.

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE   -- When client-id given is a valid client id
  FALSE  -- client id is INVALID
  
SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL boolean client_validate_fwd_client_id 
(
  int32                              client_id  /* client_id for validation */
)
{
  int i =0;

  /* Pre condition check */
  if(client_id == 0)
  {
    DS_AT_MSG1_ERROR("Invalid client id (%d)", client_id);
    return FALSE;
  }
  
  while(i < DSAT_MAX_CLIENT)
  {
    if(dsat_fwd_at_clients[i] == client_id)
    {
      DS_AT_MSG1_MED("ID already existing at index %d",i);
      return TRUE;
    }
    ++i;
  }
  
  DS_AT_MSG1_ERROR("Invalid Client id (%d)", client_id);
  return FALSE;
} /* client_validate_fwd_client_id */

/*===========================================================================

FUNCTION CLIENT_GET_MAX_ENTRY_ALLOWED

DESCRIPTION
  This function gets the free spaces in cmd table.

DEPENDENCIES
  None
  
RETURN VALUE
  Maximum free space allowable in teh command entry table
  
SIDE EFFECTS
  None
  
===========================================================================*/
INLINE uint8 client_get_max_entry_allowed 
(
  void
)
{
  uint8 count = 0;
  uint8 i = 0;

  /* Max no of commands that can be added */
  while(i < MAX_CMD_LIST)
  {
    if(dsat_fwd_at_cmds[i].client_id == 0)
    {
      count++;
    }
    i++;
  }
  return count;
}/* client_get_max_entry_allowed */

/*===========================================================================

FUNCTION DSATCLIENT_STATE_INFO_UPDATE

DESCRIPTION
  This function is called to update the state info values 
  that are exchanged between ATCOP and remote forwarded client.
  
DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatclient_state_info_update
(
  dsat_cmd_state_type * curr_state_ptr  /* State Info to update */
)
{

  memset((void*)curr_state_ptr, 0x0, 
          sizeof(dsat_cmd_state_type));

  curr_state_ptr->dsat_v_val = (dsat_num_item_type)dsatutil_get_val(
                                       DSAT_BASIC_V_IDX,0,0,NUM_TYPE);
  curr_state_ptr->dsat_q_val = (dsat_num_item_type)dsatutil_get_val(
                                       DSAT_BASIC_Q_IDX,0,0,NUM_TYPE);
  curr_state_ptr->dsat_s3_val = (dsat_num_item_type)dsatutil_get_val(
                                       DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
  curr_state_ptr->dsat_s4_val = (dsat_num_item_type)dsatutil_get_val(
                                       DSAT_SREG_S4_IDX,0,0,NUM_TYPE);
 curr_state_ptr->dsat_max_arg_size = DSAT_MAX_ARG;
#ifdef FEATURE_DSAT_ETSI_MODE
  curr_state_ptr->dsat_clir_val = (dsat_num_item_type)dsatutil_get_val(
                                   DSATETSI_EXT_ACT_CLIR_ETSI_IDX,0,0,NUM_TYPE);
  curr_state_ptr->dsat_colp_val = (dsat_num_item_type)dsatutil_get_val(
                                   DSATETSI_EXT_ACT_COLP_ETSI_IDX,0,0,NUM_TYPE);
  curr_state_ptr->dsat_cmee_val = (dsat_num_item_type)dsatutil_get_val(
                                               DSAT_EXT_CMEE_IDX,0,0,NUM_TYPE);

  curr_state_ptr->dsat_cscs_val = (dsat_num_item_type)dsatutil_get_val(
                                               DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);

  (void) dsatutil_memscpy((void*)&curr_state_ptr->dsat_ccug_val[0],
          (3* (sizeof(dsat_num_item_type))),
          (void*)dsatutil_get_val(DSATETSI_EXT_ACT_CCUG_ETSI_IDX,0,0,STR_TYPE),
         (3* (sizeof(dsat_num_item_type))));

 (void) dsatutil_memscpy((void*)&curr_state_ptr->dsat_cmec_val[0],
         (3* (sizeof(dsat_num_item_type))),
         (void*)dsatutil_get_val(DSATETSI_EXT_ACT_CMEC_ETSI_IDX,0,0,STR_TYPE),
         (3* (sizeof(dsat_num_item_type))));
 /* CMEC 4th Param will be sent as an optional TLV from QMI so add it in a
    separate variable */
  curr_state_ptr->dsat_cmec_val_4 = (dsat_num_item_type)dsatutil_get_val(
                                   DSATETSI_EXT_ACT_CMEC_ETSI_IDX,0,3,NUM_TYPE);
#endif /* #ifdef FEATURE_DSAT_ETSI_MODE */

} /* dsatclient_state_info_update */

/*===========================================================================

FUNCTION DSATCLIENT_EXT_SEND_RESPONSE_EX

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
dsatclient_ext_send_response_ex
(
  boolean                                    cmd_status,   /* CMD status      */
  int32                                      client_id,    /* Client id       */
  dsat_client_result_enum_type               result_code,  /* result code     */
  dsat_fwd_resp_enum_type                    resp_type,    /* resp_type       */
  dsm_item_type                             *resp_buff_ptr,/* response buffer */
  uint32                                     atcop_info    /* Atcop Info      */
)
{
  DSAT_DL_LOCK();

  if (DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatclient_ext_send_response_fp))
  {
    dsatdl_vtable.dsatclient_ext_send_response_fp(cmd_status, client_id, result_code,
                                                  resp_type, resp_buff_ptr, atcop_info);
  }
  else
  {
    dsm_free_packet(&resp_buff_ptr);
  }

  DSAT_DL_UNLOCK();

  return;
}/* dsatclient_ext_send_response_ex */

/*===========================================================================
FUNCTION DSATCLIENT_EXT_SEND_URC

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
dsatclient_ext_send_urc
(
  uint8                                    cmd_status,   /* CMD status      */
  int32                                      client_id,    /* Client id       */
  dsm_item_type                             *resp_buff_ptr/* response buffer */
)
{
  DSAT_DL_LOCK();

  if (DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatclient_ext_send_urc_fp))
  {
    dsatdl_vtable.dsatclient_ext_send_urc_fp(cmd_status, client_id, resp_buff_ptr);
  }
  else
  {
    dsm_free_packet(&resp_buff_ptr);
  }

  DSAT_DL_UNLOCK();

  return;
}/* dsatclient_ext_send_urc */

/*===========================================================================

FUNCTION CLIENT_FWD_CMD_REQUEST

DESCRIPTION
  This function is used for forward command request.

DEPENDENCIES 
  AT forward demon up and running.

RETURN VALUE
  1 : Input prameter validation successful
  0 : Otherwise
  
SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL boolean client_fwd_cmd_request
(
  ds_cmd_enum_type            cmd_id,         /* Command ID */
  dsat_cmd_status_cb_fn_type  status_func_cb, /* Status callback */
  void                       *user_info_ptr   /* Client data       */
)
{
  ds_cmd_type                  *cmd_ptr     = NULL;
  ds_at_fwd_cmd_request_s_type *fwd_req_cmd = NULL;

  if (FALSE == dsat_is_atcop_allowed())
  {
    DS_AT_MSG0_ERROR(" ATcop is not initialized");
    return FALSE;
  }

  /*-----------------------------------------------------------
     Post the command to DS task and return success to client
  ------------------------------------------------------------*/
  cmd_ptr = dsat_get_cmd_buf(sizeof(ds_at_fwd_cmd_request_s_type), FALSE);
  fwd_req_cmd = cmd_ptr->cmd_payload_ptr;

  cmd_ptr->hdr.cmd_id         = cmd_id;
  fwd_req_cmd->status_func_cb = status_func_cb;
  fwd_req_cmd->user_info_ptr  = user_info_ptr;

  ds_put_cmd(cmd_ptr);

  return TRUE;
} /* client_fwd_cmd_request */

/*===========================================================================
FUNCTION DSAT_RESET_CMD_FORWARDING

DESCRIPTION  
  Caller can call this API for phone reboot request.ATcop will check and honour this request.

PARAMETERS 

  status_func_cb  ATcop will intimate reboot status(success/failure) to caller  by calling this call back function.
  user_info_ptr    Contain user data.
  
DEPENDENCIES 
  AT forward demon up and running.

RETURN VALUE
  1 : Input prameter validation successful
  0 : Otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean dsat_reset_cmd_forwarding
(
  dsat_cmd_status_cb_fn_type   status_func_cb,      /* Status callback */
  void                        *user_info_ptr        /* Client data       */
)
{
  return client_fwd_cmd_request(DS_CMD_ATCOP_RESET_REQ_CMD,
                                status_func_cb, user_info_ptr);
}/*dsat_reset_cmd_forwarding*/
/*===========================================================================
FUNCTION DSAT_SHUTDOWN_CMD_FORWARDING

DESCRIPTION  
  Caller can call this API for phone shutdown request.ATcop will check and honour this request.

PARAMETERS 

  status_func_cb  ATcop will intimate reboot status(success/failure) to caller by calling this call back function.
  user_info_ptr    Contain user data.
  
DEPENDENCIES 
  AT forward demon up and running.

RETURN VALUE
  1 : Input prameter validation successful
  0 : Otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean dsat_shutdown_cmd_forwarding
(
  dsat_cmd_status_cb_fn_type   status_func_cb,      /* Status callback */
  void                        *user_info_ptr        /* Client data       */
)
{
  return client_fwd_cmd_request(DS_CMD_ATCOP_SHUTDOWN_REQ_CMD,
                                status_func_cb, user_info_ptr);
}

void dsat_reset_cmd (void)
{
  (void) client_fwd_cmd_request(DS_CMD_ATCOP_RESET_REQ_CMD, NULL, NULL);
}

void dsat_shutdown_cmd(void)
{
  (void) client_fwd_cmd_request(DS_CMD_ATCOP_SHUTDOWN_REQ_CMD, NULL, NULL);
}


#ifndef DSATCLIENT_H
#define DSATCLIENT_H
/*===========================================================================

                        D A T A   S E R V I C E S

                A T   C O M M A N D   P R O C E S S O R

   C L I E N T   I N T E R F A C E   E X T E R N A L  H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defines and
  enumerated constants, and function prototypes required for the
  data services AT command processor client asynchronous interface.

  Copyright (c) 2005 - 2015 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/inc/dsatclient.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   tk      Added support for Dynamic ATCoP.
12/23/11    ua     Route the Operating mode Reset command through 
                   DMS->AT->QMI-AT->Apps Proc.
01/19/12   sk      Feature cleanup.
06/29/11   nc      Adding support for forwarding +CDIS,+CRSL,+CMAR,+CSO,+CSS to APPS_PROC
06/17/10   ua      Changed the 4th param of CMEC as a new data (new optional 
                   TLV in QMI ).
05/10/10   kk      Removing dependency of ATCoP Macro MAX_ARG at QMI.
04/20/09   bs/sa   Added support for Modem Bridge Architecture.
06/28/09   vg      Fixed Lint errors.
07/16/08   ua      Added support for external client support.
04/02/08   ua      Moving the external header files to INC directory. 
02/21/06   sa      Merged changes from Sirius branch
                   -Change SS info callback interface.
                   -Added legacy acc IDs to enum which are not supported.
11/08/05   ar      Added serving system query callback registration.
06/04/05   tkk     Added new operation to get list of supported languages from
                   UI.
05/09/05   hap     Modified dsat_client_msg_submit prototype and
                   removed dsat_response_s_type from dsat_message_u_type 
04/14/05   ar      Added ATCOP restricted event for accessory clients.
04/01/05   tkk     Introduced new field for UI-ATCoP response data structure 
                   and added new operations. Changed the enum value for 
                   +CRMP/^SRMP ringer volume ranges.
03/31/05   sb      Fixes from testing the serializer changes.
02/25/05   ibm     Updated annunciator enumerations.
02/24/05   hap     Added commands to query profile setting and ringer volume
02/23/05   ibm     Added command to query the annunciator 
                   Added data structures for responses from the UI
02/22/05   ibm     Added annunciator enumerations.
02/21/05   hap     Added support for ^SNFS command.
02/21/05   hap     Added support for ^SSET,^SLNG and +CRMP/^SRMP commands
                   Added response codes.
02/07/05   ar      Added DSAT_ACC_INST_DEVID and DSAT_CLIENT_NULL_VAL
01/26/02   ar      Simplify callback table and routines.
                   Add accessory manager interface elements.
01/25/05   ibm     Moved private declarations to dsatclienti.h
                   Removed type dsat_client_id_e.
                   Moved DSAT_CLIENT_ID_MAX to dsatclienti.h
                   Added API function dsat_get_client_id().  
                   Added API function dsat_release_client_id().  
                   Modified prototype for function dsat_client_reg_cb_func().
01/13/05   ar      Initial version.

===========================================================================*/


#include "datamodem_variation.h"
#include "dsm.h"
#include "fs_public.h"
#include "nv_items.h"
#include "dsat_v.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#define DSAT_CLIENT_NULL_VAL (-1)

/* Client identifier type */
#define DSAT_CLIENT_ID_NULL  DSAT_CLIENT_NULL_VAL

typedef enum
{
   DSAT_RESP_NONE         = -1,
   DSAT_RESP_START        = 0,          /* Start of response      */
   DSAT_RESP_END          = 1,          /* End of response       */
   DSAT_RESP_INTERMEDIATE = 2,          /* Intermediate response   */
   DSAT_RESP_COMPLETE     = 3,          /* Complete response */
   DSAT_RESP_MAX = 0XFF
} dsat_fwd_resp_enum_type;


/*-----------------------------------------------------------------
  Definitions to manage ATCOP client registration
------------------------------------------------------------------*/
#define MAX_CMD_LIST             (20)  /* total no of commands that can be
                                          registered */

/* INVALID Client identifier type */
#define DSAT_INVALID_CLIENT_ID  (0)

#define DSAT_SUCCESS        1
#define DSAT_FAILURE        0

/* 
   Set the result code 
   1. DSAT_CLIENT_RESULT_ERROR  -- when res_buff contain ERROR details. ATCoP
                                   will send only the details in res_buffer 
                                   and will not append ERROR to result.
                                -- when only ERROR result code needs 
                                   to be sent.In this case res_buff should
                                   be NULL and result_code should be ERROR.
   2. DSAT_CLIENT_RESULT_OK     -- When User wants to send result buffer with 
                                   result as OK to TE, they should not append 
                                   the result buffer with OK,instead set this 
                                   result code and fill only those details other
                                   than OK in response buffer
                                -- when only OK result code needs 
                                   to be sent.In this case res_buff should
                                   be NULL and result_code should be set as OK.
   3. DSAT_CLIENT_RESULT_OTHER  -- This is added for future use. If use wants to
                                   send intermediate /unsolicited response, they
                                   need to set this enum and resp_type enum will
                                   differentiate between intermediate and 
                                   unsolicited response.
*/
typedef enum
{  
   DSAT_CLIENT_RESULT_ERROR,
   DSAT_CLIENT_RESULT_OK,
   DSAT_CLIENT_RESULT_OTHER,
   DSAT_CLIENT_RESULT_MAX = 0XFF
} dsat_client_result_enum_type;


/* 
  These are the state variable info that needs to be shared across
  two ATCoP modules. It includes general and command specific. If new info
  needs to be shared it needs to be added here.
*/

typedef struct dsat_cmd_state 
{
   dsat_num_item_type       dsat_ccug_val[3];
   dsat_num_item_type       dsat_v_val;
   dsat_num_item_type       dsat_q_val;
   dsat_num_item_type       dsat_s3_val;
   dsat_num_item_type       dsat_s4_val;
   dsat_num_item_type       dsat_clir_val;
   dsat_num_item_type       dsat_colp_val;
   dsat_num_item_type       dsat_cmee_val;
   dsat_num_item_type       dsat_cmec_val[3];
   dsat_num_item_type       dsat_cmec_val_4;
   dsat_num_item_type       dsat_cscs_val;
   dsat_num_item_type       dsat_max_arg_size;
 } dsat_cmd_state_type;

typedef struct dsat_cmd_list
{
  byte             cmd_buff[MAX_CMD_SIZE]; /* Command buffer */
  boolean          is_abortable;     /* Abort flag           */
  uint16           cmd_buff_len;    /* length of the command */
}dsat_cmd_list_type;

/* Response call back function prototype */
typedef void (*dsatclient_cmd_status_cb_type)
(
  boolean                                    cmd_status,  /* CMD status    */
  uint32                                     client_id,   /* Client id     */
  void                                      *user_info_ptr/* Client data   */
);

typedef boolean
(*dsatclient_ext_at_cmd_exec_cb)
(
  dsat_mode_enum_type                     at_cmd_mode,/* ATCoP processor mode*/
  const tokens_struct_type               *token_ptr,     /* Token pointer    */
  const dsat_cmd_state_type               at_state_type,/* AT Command state  */
  uint32                                  atcop_info,   /* ATCoP info        */
  void                                   *user_info_ptr /* Client data       */
);

typedef void
(*dsatclient_ext_at_cmd_abort_cb)
(
  void                                    *user_info_ptr,/* Client data       */
  uint32                                   atcop_info    /* ATCoP info        */
);

typedef struct fwd_at_cmd_table
{
  void                              *user_info_ptr;
  byte                               cmd_name[MAX_CMD_SIZE];
  dsatclient_ext_at_cmd_exec_cb      ext_at_cmd_exec_cb;
  dsatclient_ext_at_cmd_abort_cb     ext_at_abort_cb;
  boolean                            is_abortable;
  uint32                             client_id;
}dsat_fwd_at_cmd_table_type;

extern dsat_fwd_at_cmd_table_type dsat_fwd_at_cmds[MAX_CMD_LIST];


#ifdef FEATURE_DSAT_EXT_CLIENT_SUPPORT
/* /r/n need to be added at EOL hence CMD_LINE_SIZE is less by 2 */
#define  DSAT_SW_API_CMD_LINE_SIZE  MAX_LINE_SIZE - 2
typedef int8 dsat_ext_client_id_type;

typedef struct sw_api_response_data
{
  char     *buffer;
  uint16    length;
}dsat_data_s_type;

typedef struct sw_api_resp_type
{
  boolean                      pass_fail; /* Indicates if the command entered is a valid or not */
  dsat_fwd_resp_enum_type      response_type; /* Indicates if the response is complete or not */
  dsat_ext_client_id_type      client_id; /* Client ID of the client recieving the response */
}dsat_sw_api_resp_s_type;

/* Response call back function prototype */
typedef void (*dsat_client_cmd_rsp_cb_func)
(
  dsat_sw_api_resp_s_type  resp_type,    /* Response type containing pass_fail, resp_type */
  dsat_data_s_type response,     /* If pass_fail is TRUE, response is valid */
  void                    *client_data  /* Clients user data */
);

typedef enum 
{
  DSAT_CLIENT_TYPE_INVALID      = -1,
  DSAT_CLIENT_TYPE_SIM_DRIVER   =  0,
  DSAT_CLIENT_TYPE_BMP          =  1,
  DSAT_CLIENT_TYPE_EXTRA_CLIENT =  2,
  DSAT_CLIENT_TYPE_MAX          =  3
}dsat_client_e_type;

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
extern void  dsatclient_init_client_info
( 
  void
);

/*===========================================================================

FUNCTION DSATCLIENT_REGISTER_CLIENT

DESCRIPTION
  This function allows a client to register with ATCOP for able to send AT commands
  The client ID will be a non-negative integer, if the call is successful, 
  and a negative value otherwise.

DEPENDENCIES
  None

RETURN VALUE
  TRUE on successful operation, FALSE otherwise.
  Client identifier is DSAT_EXT_CLIENT_ID_INVALID on error, non-negative
  integer on success.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean dsatclient_register_client 
( 
  dsat_client_e_type      client_type,     /* Known client type */
  dsat_ext_client_id_type *client_id_ptr   /* Assign client ID upon success */
);

/*===========================================================================

FUNCTION DSATCLIENT_DEREGISTER_CLIENT

DESCRIPTION
  This function allows client to deregister itself with ATCoP.  Used when a
  client does not want to send in any more AT commands. 
  
DEPENDENCIES
  The client must have previously called dsatclient_register_client()

RETURN VALUE
  TRUE on successful operation, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean dsatclient_deregister_client
(
  dsat_ext_client_id_type client_id /* Client id to be deregistered */
);

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
extern boolean dsatclient_send_at_cmd
(
  dsat_ext_client_id_type      client_id,  /* Registered client id */
  dsat_data_s_type             at_cmd,     /* AT command buffer and length  */
  dsat_client_cmd_rsp_cb_func  cb_func,    /* Response callback function */
  void                         *client_data /* Client's data */
);

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

boolean dsatclient_register_urc
( 
  dsat_ext_client_id_type       client_id,        /* Registered client id */
  void                         *client_data,      /* client data */
  dsat_client_cmd_rsp_cb_func   urc_response_func /* Response callback function */
);

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

boolean dsatclient_deregister_urc
( 
  dsat_ext_client_id_type       client_id /* Registered client id */
);
#endif /* FEATURE_DSAT_EXT_CLIENT_SUPPORT */

/*===========================================================================

FUNCTION DSATCLIENT_REGISTER_FWD_CLIENT

DESCRIPTION
  This function registers client to process the forwarded AT commands.
  The client ID will be a non-negative integer

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE on successful operation, FALSE otherwise.
  
SIDE EFFECTS
  None
  
===========================================================================*/
extern boolean dsatclient_register_fwd_client 
(
  dsatclient_cmd_status_cb_type   status_cb,      /*Status callback*/
  void                           *user_info_ptr   /* Client data   */
);

/*===========================================================================

FUNCTION DSATCLIENT_DEREGISTER_FWD_CLIENT

DESCRIPTION
  This function deregisters the client of AT command forwarding.

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE on successful operation, FALSE otherwise.
  
SIDE EFFECTS
  None
  
===========================================================================*/
extern boolean dsatclient_deregister_fwd_client 
(
  int32           client_id /* Registered client id */
);

/*===========================================================================

FUNCTION DSATCLIENT_REGISTER_FWD_AT_CMD

DESCRIPTION
  This function allows client to register the list of commands with ATCOP 
  for forwarding.

DEPENDENCIES
  None

RETURN VALUE
  TRUE on successful operation, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean dsatclient_register_fwd_at_cmd 
(
  int32                            client_id,      /*  registered client_id   */
  uint8                            num_valid_cmds, /* number of valid commands*/
  dsat_cmd_list_type               at_cmd_list[],  /* list of AT commands     */
  dsatclient_cmd_status_cb_type    status_cb,      /*status call back         */
  dsatclient_ext_at_cmd_exec_cb    ext_at_cmd_exec_cb, /* execution call back */
  dsatclient_ext_at_cmd_abort_cb   ext_at_abort_cb,  /* Abort call back       */
  void                            *user_info_ptr     /* user data  pointer    */
);

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
);

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
);

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
);

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
);

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
  dsat_cmd_state_type             *curr_state_ptr /* State Info to update */
);

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
);
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
);

void dsat_reset_cmd (void);

void dsat_shutdown_cmd(void);


#endif /* DSATCLIENT_H  */


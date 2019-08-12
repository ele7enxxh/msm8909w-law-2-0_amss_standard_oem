/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  Position Determination Session Manager Module

GENERAL DESCRIPTION
  This module defines the Position Determination API for both the Location
  Services Manager (LSM) and the Position Determination Session Manager
  (PDSM).

  This API provides Position Determination session control and  event
  notification services to its clients.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Copyright (c) 2015-2016 Qualcomm Technologies, Inc. All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/common/src/pdapi.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/01/15   rh      Added get gps lock synchrous API for LBS
02/24/15   ssu     Adding GM in memory logging.
09/05/14   ssu     GM LOWI Integration
03/10/14   ah      Added support for Get Avail WWAN Pos
06/26/13   mj      Wrap z_wiper_position_report_ex_struct_data with QWIP featurization
09/03/12   rh      Added support for Cell Info and NI message injection
04/18/12   ssu     Added pdsm_pd_get_best_avail_pos()
03/14/12   ss      Added pdsm_reset_location_service() API
03/02/02   rh      Added pdsm_pd_inject_external_position()
06/02/11   rh      Added pdsm_lcs_set_vx_config() API
05/18/11   rc      Lowering priority of some standalone messages from MSG_HIGH to MSG_LOW
09/30/10   rh      Changed ERR_OTHER to more meaningful error codes
08/18/09   gk      ingore the file name length in the efs data  set params
08/10/09   gk      Added support for Version
03/02/09   jd      Added support for Wiper logging over RPC.
02/06/09   atien   Add missing PDSM_CMD_TYPE_XTRA_T in pdsm_cmd_queue
01/12/09   atien   XTRA-T support
07/28/08   gk      Store user data pointer so it can be returned correctly
07/28/08   gk      Fixed Klocwork errors
06/15/08   kj      Compensate for de-featurization of pdapi.h
06/12/08   lt      Support for external coarse position injection.
11/13/07   gk      Add SUPL certificate injection
08/01/07   rw      Add support for XTRA feature
07/20/07   gk      Removed obsoleted functions, move pdapi_init to here
4/2/07     gk      PC Time injection API 
3/2/07     gk      get a buff for app_info
2/14/07    gk      fixed supl flags, some Lint warnings
12/07/06   cl      Moved pdsm_pool_init to TM-core
12/06/06   gk      Fixed bug in get position
12/04/06   gk      Added LCS API and minor mods after code review
11/8/06    gk      PDAPI for converged GPS
===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gps_variation.h"
#include "customer.h"
#include "comdef.h"     /* Definition for basic types and macros */

#include <string.h>     /* Interface to string library */
#include <stdlib.h>
#include "pdapi.h"       /* Internal interface to pdsm.c */
#include "pdapidbg.h"    /* Interface to PDSM debug services */
#include "pdapiclient.h" /* Interface to PDSM Client Interface Block */
#include "queue.h"      /* Interface to queue services */
#include "pdapibuf.h"
#include "sm_api.h"
#include "aries_os_api.h"
#include "tm_api.h"
#include "gm_api.h"

/* <EJECT> */
/*===========================================================================

            GLOBAL DEFINITIONS AND DECLARATIONS

This section contains global definitions for constants, macros, types,
variables

===========================================================================*/

typedef enum
{
  PDSM_ACTIVE_TASK_NULL,

#ifdef FEATURE_GPSONE
  PDSM_ACTIVE_TASK_PDSM,
#endif
  PDSM_ACTIVE_TASK_MAX
} pdsm_active_task_type;

uint32 pdsm_lcs_client_info_map[PDSM_CLIENT_TYPE_MAX];

static pdsm_cmd_s_type cmd_data;

static t_wiper_position_report_ex_struct_type z_wiper_position_report_ex_struct_data;

/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

pdsm_active_task_type  pdsm_active_task;
/* test_data is used for initializing PE.
*/
/*===========================================================================

FUNCTION PDSM_CMD_QUEUE

DESCRIPTION
  This function is used queue commands to PDSM task.

RETURN VALUE
  None

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

boolean pdsm_cmd_queue( const pdsm_cmd_s_type  *cmd_ptr)
{
  os_IpcMsgType           *ipc_msg_ptr;
  pdsm_cmd_s_type         *cmd_hdr_ptr;

  uint32  length = 0;
  boolean status = FALSE;

  if (cmd_ptr == NULL)
  {
    return status;
  }

  switch (cmd_ptr->cmd_type)
  {
  /* Only 3 types of commands are supported right now */
    case PDSM_CMD_TYPE_PD:           /* Position Session related command */

  #ifdef FEATURE_VECTORONE
    case PDSM_CMD_TYPE_OR:           /* Orientation related command */
  #endif

    case PDSM_CMD_TYPE_LCS :                                /* Queue the command and set the Signal for PDSM */
    case PDSM_CMD_TYPE_EXT_POS:
    case PDSM_CMD_TYPE_XTRA:
    case PDSM_CMD_TYPE_XTRA_T:
    case PDSM_CMD_TYPE_WIPER:
    case PDSM_CMD_TYPE_XSPI:
    case PDSM_CMD_TYPE_MOTION:
    case PDSM_CMD_TYPE_DPO_STATUS_RPT:
    case PDSM_CMD_TYPE_EXT_CELL:
    case PDSM_CMD_TYPE_NI_MSG:
    case PDSM_CMD_TYPE_WIFI_ON_DEMAND: 
#ifdef FEATURE_GNSS_PERIODIC_PPM
    #error code not present
#endif
    case PDSM_CMD_TYPE_WIFI_ATTACHMENT_STATUS_IND:
    case PDSM_CMD_TYPE_WIFI_ENABLED_STATUS:
      ipc_msg_ptr = os_IpcCreate(sizeof(pdsm_cmd_s_type), IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_TM);
      /* Queue the commands in Client command Queue.
      */
      if (ipc_msg_ptr == NULL)
      {
        PDSM_SWITCH_ERR( "IPC message allocation failed",(cmd_ptr->cmd_type),0,0 );
      }
      else
      {

        cmd_hdr_ptr = (pdsm_cmd_s_type*)ipc_msg_ptr->p_Data; /*lint !e826 */
        *cmd_hdr_ptr = *cmd_ptr;
        ipc_msg_ptr->q_MsgId  = TM_CORE_MSG_ID_PDAPI;
        status    = os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM);
        if (status == FALSE)
        {
          PDSM_MSG_ERROR("Error in sending PDAPI msg_id=%d to TM thread",ipc_msg_ptr->q_MsgId,0,0);
          status = os_IpcDelete(ipc_msg_ptr);
          ipc_msg_ptr = NULL;
        } /* End status */
        else
        {
          PDSM_MSG_HIGH("sending PDAPI msg_id=%d to TM thread", ipc_msg_ptr->q_MsgId,0,0);
        } /* end else status */
      }
      break;
    case PDSM_CMD_TYPE_PARAM:
      if (cmd_ptr->cmd.pa.info.set_type == PDSM_PA_APP_INFO)
      {
        ipc_msg_ptr = os_IpcCreate(sizeof(pdsm_cmd_s_type) + PDSM_PA_APP_INFO_LENGTH, IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_TM);
      }
      else if (cmd_ptr->cmd.pa.info.set_type == PDSM_PA_EFS_DATA)
      {
        length = sizeof(pdsm_cmd_s_type) + PDSM_PD_FS_MAX_PATH_LEN_BYTES +
                 cmd_ptr->cmd.pa.info.pa_set.efs_data.efs_data_len; 
          
        ipc_msg_ptr = os_IpcCreate(length, IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_TM);
      }
      else
      {
        ipc_msg_ptr = os_IpcCreate(sizeof(pdsm_cmd_s_type), IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_TM);
      }

      /* Queue the commands in Client command Queue.
      */
      if (ipc_msg_ptr == NULL)
      {
        PDSM_SWITCH_ERR( "IPC message allocation failed",(cmd_ptr->cmd_type),0,0 );
      }
      else
      {

        cmd_hdr_ptr = (pdsm_cmd_s_type*)ipc_msg_ptr->p_Data; /*lint !e826 */
        *cmd_hdr_ptr = *cmd_ptr;
        ipc_msg_ptr->q_MsgId  = TM_CORE_MSG_ID_PDAPI;
        status    = os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM);
        if (status == FALSE)
        {
          PDSM_MSG_ERROR("Error in sending PDAPI msg_id=%d to TM thread",ipc_msg_ptr->q_MsgId,0,0);
          status = os_IpcDelete(ipc_msg_ptr);
          ipc_msg_ptr = NULL;
        } /* End status */
        else
        {
          PDSM_MSG_HIGH("sending PDAPI msg_id=%d to TM thread", ipc_msg_ptr->q_MsgId,0,0);
        } /* end else status */
      }
      break;


    default:
      PDSM_SWITCH_ERR( "Command not supported by PD API",(cmd_ptr->cmd_type),0,0 );
      break;
  } /* End Switch for PDAPI command */
  return status;

} /* End pdsm_cmd_queue( ) */


/*******************************************************************************
********************************************************************************
                          User Invoked Commands.
********************************************************************************
********************************************************************************/


/*===========================================================================

FUNCTION PDSM_GET_POSITION

DESCRIPTION
  This function is called by different clients to obtain Position Location
  Information.

RETURN VALUE
  TRUE - Command Accepted; FALSE - Command Rejected due to an error 

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

boolean pdsm_get_position(
   pdsm_pd_cmd_cb_f_type    cmd_cb_func,         /* Command callback */
  /*~ PARAM pd_cb_func POINTER */
   void                     *client_data_ptr,   /* user data pointer */
   pdsm_pd_option_s_type    *option_ptr,        /* PD option type */
  /*~ PARAM option_ptr POINTER */
   pdsm_pd_qos_type         *qos_ptr,           /* Ptr to user QOS for this request */
  /*~ PARAM qos_ptr POINTER */
   pdsm_client_id_type      client_id           /* Client id of user */
)
{
  pdsm_cmd_s_type         *cmd_ptr;
  pdsm_pd_cmd_s_type      *pd_cmd_ptr;
  pdsm_pd_cmd_info_s_type *cmd_info_ptr;
  pdsm_pd_cmd_err_e_type  cmd_err = PDSM_PD_CMD_ERR_NOERR;
  pdsm_cmd_s_type         z_cmd_data;
  boolean                 ret_value = FALSE;
  struct pdsm_client_s  *client_ptr;

  /* verify option and qos pointers */
  if (option_ptr == NULL || qos_ptr == NULL) 
  {
    return ret_value;
  }

  client_ptr = pdsmclient_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL)
  {
    return ret_value;
  }

  PDSM_MSG_LOW("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_get_position(). CDPtr %p", client_data_ptr,0,0);
  PDSM_MSG_LOW("------------------------------------------",0,0,0);


  cmd_ptr = &z_cmd_data;
  memset(cmd_ptr,0,sizeof(pdsm_cmd_s_type));


  /* Pointer to Command header and information.
  */
  pd_cmd_ptr   = &cmd_ptr->cmd.pd;
  cmd_info_ptr = &pd_cmd_ptr->info;

  /* Initialize the command type field.
  */
  cmd_ptr->cmd_type = PDSM_CMD_TYPE_PD;

  /* Initialize the command header fields.
  */
  pd_cmd_ptr->cmd_cb_func = cmd_cb_func;
  pd_cmd_ptr->client_id = client_id;
  pd_cmd_ptr->data_block_ptr = client_data_ptr;
  pd_cmd_ptr->cmd = PDSM_PD_CMD_GET_POS;

  /* use the latest client data pointer.
  */
  client_ptr->pd_data_ptr = client_data_ptr;  
  /* check if the parametrs are valid or not. */
  if ( option_ptr->session <= PDSM_PD_SESS_TYPE_MIN ||
      option_ptr->session >= PDSM_PD_SESS_TYPE_MAX )
  {
    cmd_err = PDSM_PD_CMD_ERR_SESS_TYPE_P;
  }
  if (cmd_err == PDSM_PD_CMD_ERR_NOERR )
  {
    memscpy((void *)&(cmd_info_ptr->option_type),
           sizeof(cmd_info_ptr->option_type),
           (void *)option_ptr,
           sizeof(*option_ptr));

    memscpy((void *)&(cmd_info_ptr->qos),
           sizeof(cmd_info_ptr->qos),
           (void *)qos_ptr,
           sizeof(*qos_ptr));

    /* Initialize the Command error field. In case of errors found in this function
    ** CM will inform the clients by calling their callbacks immediately.
    */
    cmd_info_ptr->cmd_err = cmd_err;

    /* Queue the command to PDSM Q */
  }
  ret_value = pdsm_cmd_queue( cmd_ptr );
  
  if (ret_value == FALSE) 
  {
    if ( cmd_cb_func != NULL )
    {
      cmd_cb_func( client_data_ptr, PDSM_PD_CMD_GET_POS, PDSM_PD_CMD_ERR_NO_BUF_L );
    } /* End if no callback is provided. */
  }
  return ret_value;

} /* End pdsm_get_position() */

/*===========================================================================

FUNCTION PDSM_END_SESSION

DESCRIPTION
  This function is used to end a PD session

RETURN VALUE
  TRUE - Command Accepted; FALSE - Command Rejected due to an error 

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

boolean pdsm_end_session
(
  pdsm_pd_cmd_cb_f_type        cmd_cb_func,       /* Command callback function */
  pdsm_pd_end_session_e_type   session_type,      /* End session type */
  void                         *client_data_ptr,  /* Client provided data pointer */
  pdsm_client_id_type          client_id          /* CLient ID */
)
{
  pdsm_cmd_s_type         *cmd_ptr;
  pdsm_pd_cmd_s_type      *pd_cmd_ptr;
  pdsm_pd_cmd_info_s_type *cmd_info_ptr;
  pdsm_pd_cmd_err_e_type  cmd_err = PDSM_PD_CMD_ERR_NOERR;
  pdsm_cmd_s_type         z_cmd_data;
  boolean                 ret_value = FALSE;
  struct pdsm_client_s  *client_ptr;

  client_ptr = pdsmclient_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL) 
  {
    return ret_value;
  }

  PDSM_MSG_LOW("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_end_session(). CDPtr %p", client_data_ptr, 0, 0); 
  PDSM_MSG_LOW("------------------------------------------",0,0,0);
  
  cmd_ptr = &z_cmd_data;
  memset(cmd_ptr,0,sizeof(pdsm_cmd_s_type));

  /* Get the Command and info pointer's.
  */
  pd_cmd_ptr = &cmd_ptr->cmd.pd;
  cmd_info_ptr = &pd_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type = PDSM_CMD_TYPE_PD;

  /* Init the command header fields.
  */
  pd_cmd_ptr->cmd_cb_func = cmd_cb_func;
  pd_cmd_ptr->client_id = client_id;
  pd_cmd_ptr->data_block_ptr = client_data_ptr;
  pd_cmd_ptr->cmd = PDSM_PD_CMD_END_SESSION;

  /* use the latest client data pointer.
  */
  client_ptr->pd_data_ptr = client_data_ptr;
  /* Initialize the Information fields. cmd_err field is
  ** initialized so that the interface of passing command's to Session
  ** manager in spite of errors is available.
  */
  cmd_info_ptr->cmd_err = cmd_err;

  /* Indicate user initiated end */
  cmd_info_ptr->end_status = PDSM_PD_END_CLIENT_END;

  /* Indicate the session we are trying to end.
  */
  cmd_info_ptr->session_end_info.end_session_type   = session_type;  
  cmd_info_ptr->session_end_info.receiver_off = FALSE;

  ret_value = pdsm_cmd_queue( cmd_ptr );
  if (ret_value == FALSE) 
  {
    if ( cmd_cb_func != NULL )
    {
      cmd_cb_func( client_data_ptr, PDSM_PD_CMD_END_SESSION, PDSM_PD_CMD_ERR_NO_BUF_L );
    } /* End if no callback is provided. */
  }

  return ret_value;


} /* End pdsm_end_session( ) */


/*===========================================================================

FUNCTION PDSM_END_SESSION

DESCRIPTION
  This function is used to end a PD session

RETURN VALUE
  TRUE - Command Accepted; FALSE - Command Rejected due to an error 

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

boolean pdsm_end_session_ex
(
  pdsm_pd_cmd_cb_f_type      pd_cb_func,    /* Command callback */
  /*~ PARAM pd_cb_func POINTER */
  pdsm_pd_end_session_e_type     session_type,    /* End session type */
  pdsm_pd_end_session_opt_s_type   *end_session_opt,  /*End Session Option type*/
  void               *client_data_ptr,  /* Client specified ptr */
  pdsm_client_id_type        client_id      /* requesting client ID */
)
{
  pdsm_cmd_s_type         *cmd_ptr;
  pdsm_pd_cmd_s_type      *pd_cmd_ptr;
  pdsm_pd_cmd_info_s_type *cmd_info_ptr;
  pdsm_pd_cmd_err_e_type  cmd_err = PDSM_PD_CMD_ERR_NOERR;
  pdsm_cmd_s_type         z_cmd_data;
  boolean                 ret_value = FALSE;
  struct pdsm_client_s  *client_ptr;

  /* verify end_session pointer */
  if(end_session_opt == NULL)
  {
     return ret_value;
  }
  client_ptr = pdsmclient_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL) 
  {
    return ret_value;
  } 

  PDSM_MSG_LOW("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_end_session_ex(). CDPtr %p", client_data_ptr, 0, 0); 
  PDSM_MSG_LOW("------------------------------------------",0,0,0);
  
  cmd_ptr = &z_cmd_data;
  memset(cmd_ptr,0,sizeof(pdsm_cmd_s_type));

  /* Get the Command and info pointer's.
  */
  pd_cmd_ptr = &cmd_ptr->cmd.pd;
  cmd_info_ptr = &pd_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type = PDSM_CMD_TYPE_PD;

  /* Init the command header fields.
  */
  pd_cmd_ptr->cmd_cb_func = pd_cb_func;
  pd_cmd_ptr->client_id = client_id;
  pd_cmd_ptr->data_block_ptr = client_data_ptr;
  pd_cmd_ptr->cmd = PDSM_PD_CMD_END_SESSION_EX;  

  /* use the latest client data pointer.
  */
  client_ptr->pd_data_ptr = client_data_ptr;
  /* Initialize the Information fields. cmd_err field is
  ** initialized so that the interface of passing command's to Session
  ** manager in spite of errors is available.
  */
  cmd_info_ptr->cmd_err = cmd_err;

  /* Indicate user initiated end */
  cmd_info_ptr->end_status = PDSM_PD_END_CLIENT_END;

  /* Indicate the session we are trying to end.
  */
  cmd_info_ptr->session_end_info.end_session_type   = session_type;
  cmd_info_ptr->session_end_info.receiver_off = end_session_opt->receiver_off;

  ret_value = pdsm_cmd_queue( cmd_ptr );
  if (ret_value == FALSE) 
  {
    if ( pd_cb_func != NULL )
    {
      pd_cb_func( client_data_ptr, PDSM_PD_CMD_END_SESSION, PDSM_PD_CMD_ERR_NO_BUF_L );
    } /* End if no callback is provided. */
  }

  return ret_value;


} /* End pdsm_end_session( ) */


/*===========================================================================

FUNCTION pdsm_pd_inject_external_time

DESCRIPTION
  Command PDSM to inject time information from an external clock.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  pdsm_init( ) must have been already done.

RETURN VALUE
  TRUE - Command Accepted; FALSE - Command Rejected due to an error 

SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM event.

===========================================================================*/
boolean pdsm_pd_inject_external_time(
   pdsm_pd_cmd_cb_f_type    cmd_cb_func,         /* Command callback */
   void                     *client_data_ptr,   /* user data pointer */
   pdsm_pd_external_time_info_type    *ext_time_ptr,        /* external time */
   pdsm_client_id_type      client_id           /* Client id of user */
)
{
  pdsm_cmd_s_type         *cmd_ptr;
  pdsm_pd_cmd_s_type      *pd_cmd_ptr;
  pdsm_pd_cmd_info_s_type *cmd_info_ptr;
  pdsm_pd_cmd_err_e_type  cmd_err = PDSM_PD_CMD_ERR_NOERR;
  pdsm_cmd_s_type         z_cmd_data;
  boolean                 ret_value = FALSE;
  struct pdsm_client_s  *client_ptr;

  /* verify time Info pointer */
  if (ext_time_ptr == NULL)
  {
    return ret_value;
  }

  client_ptr = pdsmclient_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL)
  {
    return ret_value;
  }

  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_pd_inject_external_time(). CDPtr %p", client_data_ptr, 0, 0); 
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);
  
  cmd_ptr = &z_cmd_data;
  memset(cmd_ptr,0,sizeof(pdsm_cmd_s_type));

  /* Get the Command and info pointer's.
  */
  pd_cmd_ptr = &cmd_ptr->cmd.pd;
  cmd_info_ptr = &pd_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type = PDSM_CMD_TYPE_PD;

  /* Init the command header fields.
  */
  pd_cmd_ptr->cmd_cb_func = cmd_cb_func;
  pd_cmd_ptr->client_id = client_id;
  pd_cmd_ptr->data_block_ptr = client_data_ptr;
  pd_cmd_ptr->cmd = PDSM_PD_CMD_INJECT_TIME;
  memscpy(&cmd_info_ptr->ext_time_info, sizeof(cmd_info_ptr->ext_time_info), ext_time_ptr, sizeof(*ext_time_ptr));
  /* Initialize the Information fields. cmd_err field is
  ** initialized so that the interface of passing command's to Session
  ** manager in spite of errors is available.
  */
  cmd_info_ptr->cmd_err = cmd_err;

  /* use the latest client data pointer.
  */
  client_ptr->pd_data_ptr = client_data_ptr;
  ret_value = pdsm_cmd_queue( cmd_ptr );
  if (ret_value == FALSE) 
  {
    if ( cmd_cb_func != NULL )
    {
      cmd_cb_func( client_data_ptr, PDSM_PD_CMD_INJECT_TIME, PDSM_PD_CMD_ERR_NO_BUF_L );
    } /* End if no callback is provided. */
  }

  return ret_value;

}

/*===========================================================================

FUNCTION pdsm_pd_inject_external_pos

DESCRIPTION
  !! DEPRECATED API !!, use pdsm_pd_inject_external_position() instead.
  Command PDSM to inject coarse position information from an external source.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  pdsm_init( ) must have been already done.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM event.

===========================================================================*/
boolean pdsm_pd_inject_external_pos(
   pdsm_pd_cmd_cb_f_type    cmd_cb_func,           /* Command callback */
   void                     *client_data_ptr,      /* user data pointer */
   pdsm_pd_external_pos_info_type    *ext_pos_ptr, /* external position */
   pdsm_client_id_type      client_id              /* Client id of user */
)
{
  pdsm_pd_external_pos_info_ex_type pos_info_ex;

  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("Warning ! Deprecated API called: pdsm_pd_inject_external_pos()",0,0,0);
  PDSM_MSG_HIGH("redirecting to pdsm_pd_inject_external_pos_ex()",0,0,0);
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);

  if (NULL == ext_pos_ptr) {
    PDSM_MSG_ERROR("PDSM: Null external pos info ptr()",0,0,0);
    return FALSE;
  }
  memset(&pos_info_ex, 0, sizeof(pos_info_ex));
  memscpy(&pos_info_ex.pos_info, sizeof(pos_info_ex.pos_info), ext_pos_ptr, sizeof(*ext_pos_ptr));
  pos_info_ex.e_HoriRelIndicator = PDSM_POSITION_RELIABILITY_NOT_SET;
  pos_info_ex.e_VertRelIndicator = PDSM_POSITION_RELIABILITY_NOT_SET;
  pos_info_ex.e_AltitudeSource   = PDSM_ALTITUDE_SOURCE_UNKNOWN;
  pos_info_ex.e_AltUncCoverage   = PDSM_ALT_COVERAGE_NOT_SPECIFIED;
  pos_info_ex.e_AltHorLinkage    = PDSM_ALT_HOR_LINK_NOT_SPECIFIED;

  return pdsm_pd_inject_external_pos_ex(cmd_cb_func, client_data_ptr,
    &pos_info_ex, client_id);
}

/*===========================================================================

FUNCTION pdsm_pd_inject_external_pos_ex

DESCRIPTION
  !! DEPRECATED API !!, use pdsm_pd_inject_external_position() instead.
  Command PDSM to inject coarse position information from an external source.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  pdsm_init( ) must have been already done.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM event.

===========================================================================*/
boolean pdsm_pd_inject_external_pos_ex(
   pdsm_pd_cmd_cb_f_type    cmd_cb_func,           /* Command callback */
   void                     *client_data_ptr,      /* user data pointer */
   pdsm_pd_external_pos_info_ex_type    *ext_pos_ptr, /* external position */
   pdsm_client_id_type      client_id              /* Client id of user */
)
{
  pdsm_pd_external_position_info_type pos_info;

  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("Warning ! Deprecated API called: pdsm_pd_inject_external_pos_ex()",0,0,0);
  PDSM_MSG_HIGH("redirecting to pdsm_pd_inject_external_position()",0,0,0);
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);

  if (NULL == ext_pos_ptr) {
    PDSM_MSG_ERROR("PDSM: Null external pos info ptr()",0,0,0);
    return FALSE;
  }

  memset(&pos_info, 0, sizeof(pos_info));

  pos_info.opt_field_mask = ext_pos_ptr->pos_info.opt_field_mask; 
  pos_info.time_info_type = ext_pos_ptr->pos_info.time_info_type;
  pos_info.timestamp_sec = ext_pos_ptr->pos_info.timestamp_sec;

  pos_info.alt_def = ext_pos_ptr->pos_info.alt_def;

  pos_info.source = ext_pos_ptr->pos_info.source;

  pos_info.latitude = (double)ext_pos_ptr->pos_info.latitude * (double)PDSM_COARSE_LAT_RAD_TO_DEG_SCALE_FACTOR;
  pos_info.longitude = (double)ext_pos_ptr->pos_info.longitude * (double)PDSM_COARSE_LON_RAD_TO_DEG_SCALE_FACTOR;

  pos_info.loc_unc_hor = (float)ext_pos_ptr->pos_info.loc_unc_hor;
  pos_info.loc_unc_hor_raw = ext_pos_ptr->pos_info.loc_unc_hor_raw;
  pos_info.altitude = (float)ext_pos_ptr->pos_info.altitude;
  pos_info.loc_unc_vert = (float)ext_pos_ptr->pos_info.loc_unc_vert;

  pos_info.confidence_hor = ext_pos_ptr->pos_info.confidence_hor; 
  pos_info.confidence_hor_raw = ext_pos_ptr->pos_info.confidence_hor_raw; 
  pos_info.confidence_vert = ext_pos_ptr->pos_info.confidence_vert; 

  pos_info.e_HoriRelIndicator = ext_pos_ptr->e_HoriRelIndicator;
  pos_info.e_VertRelIndicator = ext_pos_ptr->e_VertRelIndicator;
  pos_info.e_AltitudeSource   = ext_pos_ptr->e_AltitudeSource;
  pos_info.e_AltUncCoverage   = (pdsm_pd_altitude_unc_coverage_e_type)(ext_pos_ptr->e_AltHorLinkage);
  pos_info.e_AltHorLinkage    = (pdsm_pd_altitude_horizontal_linkage_e_type)(ext_pos_ptr->e_AltUncCoverage);

  return pdsm_pd_inject_external_position(cmd_cb_func, client_data_ptr,
    &pos_info, client_id);
}


/*===========================================================================
DESCRIPTION
  Command PDSM to inject coarse position information from an external source
  with reliability associated with the injected position.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
DEPENDENCIES
  pdsm_client_init( ) must have been already done.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM event.

===========================================================================*/
boolean pdsm_pd_inject_external_position(
   pdsm_pd_cmd_cb_f_type    cmd_cb_func,           /* Command callback */
   /*~ PARAM pd_cb_func POINTER */
   void                     *client_data_ptr,      /* user data pointer */
   pdsm_pd_external_position_info_type  *ext_pos_ptr, /* external position */
   /*~ PARAM ext_pos_ptr POINTER */
   pdsm_client_id_type      client_id              /* Client id of user */
)
{
  pdsm_cmd_s_type         *cmd_ptr;
  pdsm_pd_cmd_s_type      *pd_cmd_ptr;
  pdsm_pd_cmd_info_s_type *cmd_info_ptr;
  pdsm_pd_cmd_err_e_type  cmd_err = PDSM_PD_CMD_ERR_NOERR;
  pdsm_cmd_s_type         z_cmd_data;
  boolean                 ret_value = TRUE;

  struct pdsm_client_s  *client_ptr;

  client_ptr = pdsmclient_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL) {
    PDSM_MSG_ERROR("PDSM: client_id not found: %d",client_id,0,0);
    return FALSE;
  }
  if (NULL == ext_pos_ptr) {
    PDSM_MSG_ERROR("PDSM: Null external pos info ptr()",0,0,0);
    return FALSE;
  }

  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_pd_inject_external_position(). CDPtr %p", client_data_ptr, 0, 0); 
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);

  cmd_ptr = &z_cmd_data;
  memset(cmd_ptr,0,sizeof(pdsm_cmd_s_type));

  /* Get the Command and info pointer's.
  */
  pd_cmd_ptr = &cmd_ptr->cmd.pd;
  cmd_info_ptr = &pd_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type = PDSM_CMD_TYPE_PD;

  /* Init the command header fields.
  */
  pd_cmd_ptr->cmd_cb_func = cmd_cb_func;
  pd_cmd_ptr->client_id = client_id;
  pd_cmd_ptr->data_block_ptr = client_data_ptr;
  pd_cmd_ptr->cmd = PDSM_PD_CMD_INJECT_POS;
  memscpy(&cmd_info_ptr->ext_pos_info, sizeof(cmd_info_ptr->ext_pos_info), ext_pos_ptr, sizeof(*ext_pos_ptr));
  /* Initialize the Information fields. cmd_err field is
  ** initialized so that the interface of passing command's to Session
  ** manager in spite of errors is available.
  */
  cmd_info_ptr->cmd_err = cmd_err;

  /* use the latest client data pointer.
  */
  client_ptr->pd_data_ptr = client_data_ptr;
  ret_value = pdsm_cmd_queue( cmd_ptr );
  if (ret_value == FALSE) 
  {
    if ( cmd_cb_func != NULL )
    {
      cmd_cb_func( client_data_ptr, PDSM_PD_CMD_INJECT_POS, PDSM_PD_CMD_ERR_NO_BUF_L );
    } /* End if no callback is provided. */
  }

  return ret_value;
}  



/*===========================================================================
FUNCTION pdsm_pd_query_gps_state
DESCRIPTION
  Command PDSM to return the state information of the GPS engine.
  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
DEPENDENCIES
  pdsm_init( ) must have been already done.
RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request
SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM extended status events.
===========================================================================*/
boolean pdsm_pd_query_gps_state(
   pdsm_pd_cmd_cb_f_type     cmd_cb_func,       /* Command callback */
   /*~ PARAM cmd_cb_func POINTER */
   void                      *client_data_ptr,  /* Not sure if this is required */
   pdsm_pd_ext_gps_state_s_type  *gps_state_ptr,           /* Pointer to Parameter values to query */
   pdsm_client_id_type       client_id          /* Requesting client id */
)
{
  pdsm_cmd_s_type         *cmd_ptr;
  pdsm_pd_cmd_s_type      *pd_cmd_ptr;
  pdsm_pd_cmd_info_s_type *cmd_info_ptr;
  pdsm_pd_cmd_err_e_type  cmd_err = PDSM_PD_CMD_ERR_NOERR;
  pdsm_cmd_s_type         z_cmd_data;
  boolean                 ret_value = FALSE;
  struct pdsm_client_s  *client_ptr;

  client_ptr = pdsmclient_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL)
  {
    return ret_value;
  }

  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_pd_query_gps_state(). CDPtr %p", client_data_ptr, 0, 0); 
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);

  cmd_ptr = &z_cmd_data;
  memset(cmd_ptr,0,sizeof(pdsm_cmd_s_type));

  /* Get the Command and info pointer's.
  */
  pd_cmd_ptr = &cmd_ptr->cmd.pd;
  cmd_info_ptr = &pd_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type = PDSM_CMD_TYPE_PD;

  /* Init the command header fields.
  */
  pd_cmd_ptr->cmd_cb_func = cmd_cb_func;
  pd_cmd_ptr->client_id = client_id;
  pd_cmd_ptr->data_block_ptr = client_data_ptr;
  pd_cmd_ptr->cmd = PDSM_PD_CMD_GET_GPS_STATE;
  
  /* Initialize the Information fields. cmd_err field is
  ** initialized so that the interface of passing command's to Session
  ** manager in spite of errors is available.
  */
  cmd_info_ptr->cmd_err = cmd_err;

  /* use the latest client data pointer.
  */
  client_ptr->pd_data_ptr = client_data_ptr;
  client_ptr->ext_status_data_ptr = client_data_ptr;
    
  ret_value = pdsm_cmd_queue( cmd_ptr );
  if (ret_value == FALSE) 
  {
    if ( cmd_cb_func != NULL )
    {
      cmd_cb_func( client_data_ptr, PDSM_PD_CMD_GET_GPS_STATE, PDSM_PD_CMD_ERR_NO_BUF_L );
    } /* End if no callback is provided. */
  }

  return ret_value;
  

}

/*===========================================================================

FUNCTION PDSM_SET_PARAMETERS

DESCRIPTION
  This function is used to set PDSM Phone parameters.

RETURN VALUE
  TRUE - Command Accepted; FALSE - Command Rejected due to an error 

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

boolean pdsm_set_parameters
(
  pdsm_pa_cmd_cb_f_type    cmd_cb_func,        /* Command callback fn */
  void                     *client_data_ptr,   /* Client provided data pointer */
  pdsm_pa_e_type           pa_set,             /* Type of parameter to Set */
  pdsm_pa_info_type         *pa_ptr,           /* Pointer to Parameter values to bet set to */

  pdsm_client_id_type      client_id
)
{
  pdsm_cmd_s_type         *cmd_ptr;
  pdsm_pa_cmd_s_type      *pa_cmd_ptr;
  pdsm_pa_cmd_info_s_type *cmd_info_ptr;
  pdsm_pa_cmd_err_e_type  cmd_err = PDSM_PA_CMD_ERR_NOERR;
  boolean                 ret_value = FALSE;

  struct pdsm_client_s  *client_ptr;

  client_ptr = pdsmclient_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL) 
  {
    return ret_value;
  }

  if (pa_ptr == NULL)
  {
    PDSM_MSG_ERROR("PDSM: Null parameter pointer", 0,0,0);
    return ret_value;
  }
  PDSM_MSG_LOW("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_set_parameters()",0,0,0);
  PDSM_MSG_LOW("------------------------------------------",0,0,0);
  
  cmd_ptr = &cmd_data;
  memset(cmd_ptr,0,sizeof(pdsm_cmd_s_type));
  
  pa_cmd_ptr = &cmd_ptr->cmd.pa;
  cmd_info_ptr = &pa_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type = PDSM_CMD_TYPE_PARAM;

  /* Initalize the header fields.
  */
  pa_cmd_ptr->cmd_cb_func = cmd_cb_func;
  pa_cmd_ptr->client_id = client_id;
  pa_cmd_ptr->data_block_ptr = client_data_ptr;
  pa_cmd_ptr->cmd = PDSM_PA_CMD_SET_PARAM;

  /* use the latest client data pointer.
  */
  client_ptr->pa_data_ptr = client_data_ptr;
  /* Indicate what type of parameter is required to bet set */
  cmd_info_ptr->set_type = pa_set;

  if (pa_set == PDSM_PA_APP_INFO) 
  {
    if (pa_ptr->app_info.url != NULL)
    {
      cmd_info_ptr->pa_set.app_info.url = (char*)pdsm_getbuf(PDSM_PA_APP_INFO_LENGTH);
      if (cmd_info_ptr->pa_set.app_info.url == NULL)
      {
        PDSM_MSG_ERROR("PDSM: Null buffer returned from pdsm_getbuf", 0,0,0);
        return ret_value;
      }
    }
  }
  if (pa_set == PDSM_PA_EFS_DATA)
  {
    if (pa_ptr->efs_data.efs_data_filename != NULL)
    {
      cmd_info_ptr->pa_set.efs_data.efs_data_filename = (char*)pdsm_getbuf(PDSM_PD_FS_MAX_PATH_LEN_BYTES);
      if (cmd_info_ptr->pa_set.efs_data.efs_data_filename == NULL)
      {
        PDSM_MSG_ERROR("PDSM: Null buffer returned from pdsm_getbuf", 0,0,0);
        return ret_value;
      }
    }
    else
    {
      PDSM_MSG_ERROR("PDSM: Null file name", 0,0,0);
      return ret_value;
    }
    if (pa_ptr->efs_data.efs_data_ptr != NULL)
    {
      cmd_info_ptr->pa_set.efs_data.efs_data_ptr = (unsigned char*)pdsm_getbuf(pa_ptr->efs_data.efs_data_len);
      if (cmd_info_ptr->pa_set.efs_data.efs_data_ptr == NULL)
      {
        PDSM_MSG_ERROR("PDSM: Null buffer returned from pdsm_getbuf", 0,0,0);
        return ret_value;
      }
    }
    else
    {
      if ((pa_ptr->efs_data.efs_data_file_operation != PDSM_FS_DELETE_FILE) &&
          (pa_ptr->efs_data.efs_data_file_operation != PDSM_FS_PURGE_DIR))
      {
        PDSM_MSG_ERROR("PDSM: Null file data", 0,0,0);
        return ret_value;
      }
    }
  }

  /* Copy the new paramater */
  if (pa_set == PDSM_PA_APP_INFO)
  {
    if (pa_ptr->app_info.url)
    {
      memscpy(cmd_info_ptr->pa_set.app_info.url, PDSM_PA_APP_INFO_LENGTH, pa_ptr->app_info.url, PDSM_PA_APP_INFO_LENGTH);
    }
    cmd_info_ptr->pa_set.app_info.ver = pa_ptr->app_info.ver;
    cmd_info_ptr->pa_set.app_info.datum = pa_ptr->app_info.datum;
    cmd_info_ptr->pa_set.app_info.unit = pa_ptr->app_info.unit;
    cmd_info_ptr->pa_set.app_info.acry = pa_ptr->app_info.acry;
    cmd_info_ptr->pa_set.app_info.num = pa_ptr->app_info.num;
  }
  else if (pa_set == PDSM_PA_EFS_DATA)
  {
    if (GNSS_STRLCPY( cmd_info_ptr->pa_set.efs_data.efs_data_filename, 
                    pa_ptr->efs_data.efs_data_filename, 
                    PDSM_PD_FS_MAX_PATH_LEN_BYTES) > PDSM_PD_FS_MAX_PATH_LEN_BYTES)
    {
      PDSM_MSG_ERROR("FS name path was truncated.  Destination Length %u",
        PDSM_PD_FS_MAX_PATH_LEN_BYTES, 0, 0);
      return ret_value;
    }
    
    if (pa_ptr->efs_data.efs_data_ptr)
    {
      memscpy(cmd_info_ptr->pa_set.efs_data.efs_data_ptr, pa_ptr->efs_data.efs_data_len, pa_ptr->efs_data.efs_data_ptr, pa_ptr->efs_data.efs_data_len);
    }
    cmd_info_ptr->pa_set.efs_data.efs_data_filename_len = PDSM_PD_FS_MAX_PATH_LEN_BYTES;
    cmd_info_ptr->pa_set.efs_data.efs_data_len = pa_ptr->efs_data.efs_data_len;
    cmd_info_ptr->pa_set.efs_data.efs_data_reserved = pa_ptr->efs_data.efs_data_reserved;
    cmd_info_ptr->pa_set.efs_data.efs_data_file_operation = pa_ptr->efs_data.efs_data_file_operation;
    cmd_info_ptr->pa_set.efs_data.efs_data_part_number = pa_ptr->efs_data.efs_data_part_number;
    cmd_info_ptr->pa_set.efs_data.efs_data_total_parts = pa_ptr->efs_data.efs_data_total_parts;
  }
  else
  {
    cmd_info_ptr->pa_set = *pa_ptr;
  }

  cmd_info_ptr->cmd_err = cmd_err;
  /* Now queue the command for pdsm task handling.
  */
  ret_value = pdsm_cmd_queue( cmd_ptr );
  if (ret_value == FALSE) 
  {
    cmd_err = PDSM_PA_CMD_ERR_NO_BUF_L;
    if ( cmd_cb_func != NULL )
    {
      cmd_cb_func( client_data_ptr, PDSM_PA_CMD_SET_PARAM, cmd_err );
    } /* End if no callback is provided. */
  }
  
  return ret_value;

} /* End pdsm_set_parameters( ) */

/*===========================================================================

FUNCTION PDSM_GET_PARAMETERS

DESCRIPTION
  This function is used to set PDSM Phone parameters.

RETURN VALUE
  TRUE - Command Accepted; FALSE - Command Rejected due to an error 

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

boolean pdsm_get_parameters (
    pdsm_pa_cmd_cb_f_type    cmd_cb_func,        /* Command callback fn */
    void                     *client_data_ptr,   /* Client provided data pointer */
    pdsm_pa_e_type           pa_get,             /* Type of parameter to Get */
    pdsm_client_id_type      client_id
)
{
  pdsm_cmd_s_type             *cmd_ptr;
  pdsm_pa_cmd_s_type          *pa_cmd_ptr;
  pdsm_pa_cmd_info_s_type     *cmd_info_ptr;
  pdsm_pa_cmd_err_e_type      cmd_err = PDSM_PA_CMD_ERR_NOERR;
  pdsm_cmd_s_type             z_cmd_data;
  boolean                     ret_value = FALSE;
  

  struct pdsm_client_s  *client_ptr;

  client_ptr = pdsmclient_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL) 
  {
    return ret_value;
  }
  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_get_parameters()",0,0,0);
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);
  
  cmd_ptr = &z_cmd_data;
  memset(cmd_ptr,0,sizeof(pdsm_cmd_s_type));
  
  pa_cmd_ptr                  = &cmd_ptr->cmd.pa;
  cmd_info_ptr                = &pa_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type           = PDSM_CMD_TYPE_PARAM;

  /* Initalize the header fields.
  */
  pa_cmd_ptr->cmd_cb_func     = cmd_cb_func;
  pa_cmd_ptr->client_id       = client_id;
  pa_cmd_ptr->data_block_ptr  = client_data_ptr;
  pa_cmd_ptr->cmd             = PDSM_PA_CMD_GET_PARAM;

  /* Indicate what type of parameter is required to bet set */
  cmd_info_ptr->set_type      = pa_get;

  cmd_info_ptr->cmd_err       = cmd_err;

  /* use the latest client data pointer.
  */
  client_ptr->pa_data_ptr = client_data_ptr;
  /* Now queue the command for pdsm task handling.
  */
  ret_value = pdsm_cmd_queue( cmd_ptr );
  if (ret_value == FALSE) 
  {
    cmd_err = PDSM_PA_CMD_ERR_NO_BUF_L;
    if ( cmd_cb_func != NULL )
    {
      cmd_cb_func( client_data_ptr, PDSM_PA_CMD_GET_PARAM, cmd_err );
    } /* End if no callback is provided. */
  }

  return ret_value;

} /* End pdsm_get_parameters( ) */

/*===========================================================================

FUNCTION PDSM_LCS_AGENT_CLIENT_RESPONSE

DESCRIPTION
  This function is used by the LCS application to respond to a notification
  verification request.

RETURN VALUE
  TRUE - Command Accepted; FALSE - Command Rejected due to an error 

DEPENDENCIES
  Intended to be called by registered LCS clients.

===========================================================================*/
boolean pdsm_lcs_agent_client_response(
  pdsm_lcs_cmd_cb_f_type   cmd_cb_func,
  void                    *client_data_ptr,
  pdsm_lcs_notification_resp_s_type *resp_info_ptr,
  pdsm_client_id_type      client_id
) 
{
  pdsm_cmd_s_type             *cmd_ptr;
  pdsm_lcs_cmd_s_type          *lcs_cmd_ptr;
  pdsm_lcs_cmd_info_s_type     *cmd_info_ptr;
  pdsm_lcs_cmd_err_e_type      cmd_err = PDSM_LCS_CMD_ERR_NOERR;
  pdsm_cmd_s_type              z_cmd_data;
  boolean                      ret_value = FALSE;
  
  struct pdsm_client_s  *client_ptr;

  client_ptr = pdsmclient_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL)
  {
    return ret_value;
  }
  if (resp_info_ptr == NULL)
  {
    PDSM_MSG_ERROR("PDSM: Null client repsonse ptr()",0,0,0);
    return ret_value;
  }
  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_lcs_agent_client_response()",0,0,0);
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);
  
  cmd_ptr = &z_cmd_data;
  memset(cmd_ptr,0,sizeof(pdsm_cmd_s_type));

  lcs_cmd_ptr                  = &cmd_ptr->cmd.lcs;
  cmd_info_ptr                = &lcs_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type           = PDSM_CMD_TYPE_LCS;

  /* Initalize the header fields.
  */
  lcs_cmd_ptr->cmd_cb_func     = cmd_cb_func;
  lcs_cmd_ptr->client_id       = client_id;
  lcs_cmd_ptr->data_block_ptr  = client_data_ptr;
  lcs_cmd_ptr->cmd             = PDSM_LCS_CMD_NOTIFY_VERIFY_RESP;
  cmd_info_ptr->lcs_cmd_info.resp_info.resp = resp_info_ptr->resp;
  cmd_info_ptr->lcs_cmd_info.resp_info.lcs_info_cmd_type = resp_info_ptr->lcs_info_cmd_type;
  switch (cmd_info_ptr->lcs_cmd_info.resp_info.lcs_info_cmd_type)
  {
  case PDSM_LCS_CMD_TYPE_VX_REQ:
    memscpy(&cmd_info_ptr->lcs_cmd_info.resp_info.lcs_info.vx_req,
        sizeof(cmd_info_ptr->lcs_cmd_info.resp_info.lcs_info.vx_req),
        &resp_info_ptr->lcs_info.vx_req, sizeof(resp_info_ptr->lcs_info.vx_req));
    break;
  case PDSM_LCS_CMD_TYPE_SUPL_REQ:
    memscpy(&cmd_info_ptr->lcs_cmd_info.resp_info.lcs_info.supl_req,
        sizeof(cmd_info_ptr->lcs_cmd_info.resp_info.lcs_info.supl_req),
        &resp_info_ptr->lcs_info.supl_req, sizeof(resp_info_ptr->lcs_info.supl_req));
    break;
  case PDSM_LCS_CMD_TYPE_UMTS_CP_REQ:
    memscpy(&cmd_info_ptr->lcs_cmd_info.resp_info.lcs_info.umts_cp_req,
        sizeof(cmd_info_ptr->lcs_cmd_info.resp_info.lcs_info.umts_cp_req),
        &resp_info_ptr->lcs_info.umts_cp_req, sizeof(resp_info_ptr->lcs_info.umts_cp_req));
    break;
  default:
    break;
  }
  /* Indicate what type of parameter is required to bet set */
  cmd_info_ptr->set_type    = PDSM_LCS_CMD_NOTIFY_VERIFY_RESP;

  cmd_info_ptr->cmd_err       = cmd_err;


  /* use the latest client data pointer.
  */
  client_ptr->lcs_data_ptr = client_data_ptr;
  /* Now queue the command for pdsm task handling.
  */
  ret_value = pdsm_cmd_queue( cmd_ptr );
  if (ret_value == FALSE) 
  {
    cmd_err = PDSM_LCS_CMD_ERR_NO_BUF_L;
    if ( cmd_cb_func != NULL )
    {
      cmd_cb_func( client_data_ptr, PDSM_LCS_CMD_NOTIFY_VERIFY_RESP, cmd_err );
    } /* End if no callback is provided. */
  }

  return ret_value;


}

/*===========================================================================

FUNCTION pdsm_lcs_client_init

DESCRIPTION
  Used only in case of NON-BREW Applications.

RETURN VALUE
  TRUE - Command Accepted; FALSE - Command Rejected due to an error 

DEPENDENCIES

===========================================================================*/
boolean pdsm_lcs_client_init(
    uint32 class_id, 
    pdsm_client_type_e_type client_type, 
    boolean is_brew_app)
{
  if(is_brew_app)        
  {
    if ((client_type > 0) && (client_type < PDSM_CLIENT_TYPE_MAX))
    {
      pdsm_lcs_client_info_map[client_type] = class_id;
    }
  }
  return TRUE;

}

/*===========================================================================

FUNCTION pdsm_lcs_client_app_info_init

DESCRIPTION
  sets the app_info. cmd call back with send success/failure
  
RETURN VALUE
  TRUE - Command Accepted; FALSE - Command Rejected due to an error 

DEPENDENCIES

===========================================================================*/
boolean pdsm_lcs_client_app_info_init(
    uint32 class_id, 
    pdsm_client_type_e_type client_type, 
    char* app_info_ptr,
    boolean is_brew_app)
{
  pdsm_cmd_s_type              *cmd_ptr;
  pdsm_lcs_cmd_s_type          *lcs_cmd_ptr;
  pdsm_lcs_cmd_info_s_type     *cmd_info_ptr;
  pdsm_lcs_cmd_err_e_type      cmd_err = PDSM_LCS_CMD_ERR_NOERR;
  pdsm_cmd_s_type              z_cmd_data;
  boolean                      ret_value = TRUE;
  
  if (app_info_ptr == NULL)
  {
    ret_value = FALSE;
    PDSM_MSG_ERROR("PDSM: Null client Appinfo ptr()",0,0,0);
    return ret_value;
  }
  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_lcs_client_init()",0,0,0);
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);
  
  cmd_ptr = &z_cmd_data;
  memset(cmd_ptr,0,sizeof(pdsm_cmd_s_type));

  lcs_cmd_ptr                  = &cmd_ptr->cmd.lcs;
  cmd_info_ptr                = &lcs_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type           = PDSM_CMD_TYPE_LCS;

  /* Initalize the header fields.
  */
  lcs_cmd_ptr->cmd             = PDSM_LCS_CMD_CLIENT_APP_INFO_INIT;
  /* Indicate what type of parameter is required to bet set */
  cmd_info_ptr->set_type    = PDSM_LCS_CMD_CLIENT_APP_INFO_INIT;
  cmd_info_ptr->lcs_cmd_info.client_init_type.client_type = client_type;
  cmd_info_ptr->lcs_cmd_info.client_init_type.class_id = class_id;
  cmd_info_ptr->lcs_cmd_info.client_init_type.is_brew_app = is_brew_app;
  cmd_info_ptr->lcs_cmd_info.client_init_type.app_info_ptr= (char *)pdsm_getbuf(PDSM_PA_APP_INFO_LENGTH); 
  if (cmd_info_ptr->lcs_cmd_info.client_init_type.app_info_ptr)
  {
    memscpy((char*)cmd_info_ptr->lcs_cmd_info.client_init_type.app_info_ptr, PDSM_PA_APP_INFO_LENGTH,
           (char*)app_info_ptr, PDSM_PA_APP_INFO_LENGTH);
  }
  cmd_info_ptr->cmd_err       = cmd_err;

  /* Now queue the command for pdsm task handling.
  */
  ret_value = pdsm_cmd_queue( cmd_ptr );


  return ret_value;

}

/*===========================================================================

FUNCTION pdsm_get_client_type

DESCRIPTION

RETURN VALUE
  Class Type associated with the class_id

DEPENDENCIES

===========================================================================*/
pdsm_client_type_e_type pdsm_lcs_get_client_type(uint32 class_id)
{
    int i;

    /* If the App is BREW, classId is not present in the pdsm_client_info_map */
    pdsm_client_type_e_type    client_type = PDSM_CLIENT_TYPE_BREW;

    for(i=0; i< (int)PDSM_CLIENT_TYPE_MAX; i++) 
    {      
      if((pdsm_lcs_client_info_map[i] == class_id) && (class_id != 0) )
      { 
        client_type = (pdsm_client_type_e_type)i;
        break;
      }
    }
    return client_type;

}

/*===========================================================================

FUNCTION pdsm_lcs_client_release

DESCRIPTION
  Applications use this function to release LCS Agent instance.

RETURN VALUE
  TRUE - Command Accepted; FALSE - Command Rejected due to an error 

DEPENDENCIES

===========================================================================*/
boolean pdsm_lcs_client_release( 
  pdsm_lcs_cmd_cb_f_type   cmd_cb_func, 
  void                     *client_data_ptr,
  uint32                   class_id, 
  pdsm_client_type_e_type  client_type)
{
  pdsm_cmd_s_type             *cmd_ptr;
  pdsm_lcs_cmd_s_type          *lcs_cmd_ptr;
  pdsm_lcs_cmd_info_s_type     *cmd_info_ptr;
  pdsm_lcs_cmd_err_e_type      cmd_err = PDSM_LCS_CMD_ERR_NOERR;
  pdsm_cmd_s_type              z_cmd_data;
  boolean                      ret_value = TRUE;
  
  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_lcs_client_release()",0,0,0);
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);
  
  cmd_ptr = &z_cmd_data;
  memset(cmd_ptr,0,sizeof(pdsm_cmd_s_type));

  lcs_cmd_ptr                  = &cmd_ptr->cmd.lcs;
  cmd_info_ptr                = &lcs_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type           = PDSM_CMD_TYPE_LCS;

  /* Initalize the header fields.
  */
  lcs_cmd_ptr->cmd_cb_func     = cmd_cb_func;
  lcs_cmd_ptr->data_block_ptr  = client_data_ptr;
  lcs_cmd_ptr->cmd             = PDSM_LCS_CMD_CLIENT_RELEASE;
  /* Indicate what type of parameter is required to bet set */
  cmd_info_ptr->set_type    = PDSM_LCS_CMD_CLIENT_RELEASE;
  cmd_info_ptr->lcs_cmd_info.client_release_type.class_id = class_id;
  cmd_info_ptr->lcs_cmd_info.client_release_type.client_type = client_type;
  cmd_info_ptr->cmd_err       = cmd_err;

  /* Now queue the command for pdsm task handling.
  */
  ret_value = pdsm_cmd_queue( cmd_ptr );

  if (ret_value == FALSE) 
  {
    cmd_err = PDSM_LCS_CMD_ERR_NO_BUF_L;
    if ( cmd_cb_func != NULL )
    {
      cmd_cb_func( client_data_ptr, PDSM_LCS_CMD_CLIENT_RELEASE, cmd_err );
    } /* End if no callback is provided. */
  }

  return ret_value;


}

/*===========================================================================
FUNCTION pdsm_lcs_get_status

DESCRIPTION
   This returns the LCS status asynchronously via a LCS Event callback 

RETURN VALUE
  TRUE - Command Accepted; FALSE - Command Rejected due to an error 

DEPENDENCIES

===========================================================================*/
boolean pdsm_lcs_get_status(void)
{
  pdsm_cmd_s_type             *cmd_ptr;
  pdsm_lcs_cmd_s_type          *lcs_cmd_ptr;
  pdsm_lcs_cmd_info_s_type     *cmd_info_ptr;
  pdsm_lcs_cmd_err_e_type      cmd_err = PDSM_LCS_CMD_ERR_NOERR;
  pdsm_cmd_s_type              z_cmd_data;
  boolean                      ret_value = TRUE;
  

  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_lcs_get_status()",0,0,0);
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);
  
  cmd_ptr = &z_cmd_data;
  memset(cmd_ptr,0,sizeof(pdsm_cmd_s_type));

  lcs_cmd_ptr                  = &cmd_ptr->cmd.lcs;
  cmd_info_ptr                = &lcs_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type           = PDSM_CMD_TYPE_LCS;

  /* Initalize the header fields.
  */
  lcs_cmd_ptr->cmd             =  PDSM_LCS_CMD_GET_CLIENT_STATUS;
  /* Indicate what type of parameter is required to bet set */
  cmd_info_ptr->set_type    = PDSM_LCS_CMD_GET_CLIENT_STATUS;
  cmd_info_ptr->cmd_err       = cmd_err;

  /* Now queue the command for pdsm task handling.
  */
  ret_value = pdsm_cmd_queue( cmd_ptr );
  return ret_value;

}

/*===========================================================================

FUNCTION pdsm_lcs_query_vx_config

DESCRIPTION
   This returns the LCS status asynchronously via a LCS Event callback 

DEPENDENCIES
  none

RETURN VALUE
  TRUE - Command Accepted; FALSE - Command Rejected due to an error 

SIDE EFFECTS
  none

===========================================================================*/
boolean pdsm_lcs_query_vx_config(pdsm_client_type_e_type client_type)
{
  pdsm_cmd_s_type             *cmd_ptr;
  pdsm_lcs_cmd_s_type          *lcs_cmd_ptr;
  pdsm_lcs_cmd_info_s_type     *cmd_info_ptr;
  pdsm_lcs_cmd_err_e_type      cmd_err = PDSM_LCS_CMD_ERR_NOERR;
  pdsm_cmd_s_type              z_cmd_data;
  boolean                      ret_value = TRUE;
  

  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_lcs_query_vx_config()",0,0,0);
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);
  
  cmd_ptr = &z_cmd_data;
  memset(cmd_ptr,0,sizeof(pdsm_cmd_s_type));

  lcs_cmd_ptr                  = &cmd_ptr->cmd.lcs;
  cmd_info_ptr                = &lcs_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type           = PDSM_CMD_TYPE_LCS;

  /* Initalize the header fields.
  */
  lcs_cmd_ptr->cmd             = PDSM_LCS_CMD_QUERY_VX_CONFIG;
  /* Indicate what type of parameter is required to bet set */
  cmd_info_ptr->set_type    = PDSM_LCS_CMD_QUERY_VX_CONFIG;
  cmd_info_ptr->lcs_cmd_info.client_type  = client_type;
  cmd_info_ptr->cmd_err       = cmd_err;

  /* Now queue the command for pdsm task handling.
  */
  ret_value = pdsm_cmd_queue( cmd_ptr );
  return ret_value;

}

/*===========================================================================

FUNCTION pdsm_lcs_set_vx_config

DESCRIPTION
   This returns the LCS Set Vx Config result asynchronously via a LCS Event callback 

DEPENDENCIES
  none

RETURN VALUE
  TRUE - Command Accepted; FALSE - Command Rejected due to an error 

SIDE EFFECTS
  none

===========================================================================*/
boolean pdsm_lcs_set_vx_config(pdsm_client_type_e_type client_type,
                               pdsm_lcs_agent_set_vx_config_s_type set_vx_config) 
{
  pdsm_cmd_s_type             *cmd_ptr;
  pdsm_lcs_cmd_s_type          *lcs_cmd_ptr;
  pdsm_lcs_cmd_info_s_type     *cmd_info_ptr;
  pdsm_lcs_cmd_err_e_type      cmd_err = PDSM_LCS_CMD_ERR_NOERR;
  pdsm_cmd_s_type              z_cmd_data;
  boolean                      ret_value = TRUE;
  

  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_lcs_set_vx_config()",0,0,0);
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);
  
  cmd_ptr = &z_cmd_data;
  memset(cmd_ptr,0,sizeof(pdsm_cmd_s_type));

  lcs_cmd_ptr                  = &cmd_ptr->cmd.lcs;
  cmd_info_ptr                = &lcs_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type           = PDSM_CMD_TYPE_LCS;

  /* Initalize the header fields.
  */
  lcs_cmd_ptr->cmd             = PDSM_LCS_CMD_SET_VX_CONFIG;
  /* Indicate what type of parameter is required to bet set */
  cmd_info_ptr->set_type    = PDSM_LCS_CMD_SET_VX_CONFIG;
  memscpy((void *)&cmd_info_ptr->lcs_cmd_info.set_vx_config,
      sizeof(cmd_info_ptr->lcs_cmd_info.set_vx_config),
      (void *)&set_vx_config, sizeof(set_vx_config));

  cmd_info_ptr->cmd_err       = cmd_err;

  /* Now queue the command for pdsm task handling.
  */
  ret_value = pdsm_cmd_queue( cmd_ptr );
  return ret_value;

}

/*===========================================================================

FUNCTION pdsm_use_this_position

DESCRIPTION
  Command PDSM to use the supplied position to report a fix. This is used for
  the Position Shopping feature

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  pdsm_init( ) must have been already done.

RETURN VALUE
  TRUE - Command Accepted; FALSE - Command Rejected due to an error 

SIDE EFFECTS

===========================================================================*/
boolean pdsm_use_this_position(
   pdsm_ext_pos_cmd_cb_f_type    cmd_cb_func,         /* Command callback */
   pdsm_ext_use_this_pos_data_type *position_to_use_ptr,   /* Ptr to the Position data */
   void                     *client_data_ptr,  /* Client specified ptr */
   pdsm_client_id_type      client_id           /* Client id of user */
)
{
  pdsm_cmd_s_type              *cmd_ptr;
  pdsm_ext_pos_cmd_s_type      *pd_cmd_ptr;
  pdsm_ext_pos_cmd_info_s_type *cmd_info_ptr;
  pdsm_ext_pos_cmd_err_e_type   cmd_err = PDSM_EXT_POS_CMD_ERR_NOERR;
  pdsm_cmd_s_type              z_cmd_data;
  boolean                      ret_value = TRUE;


  /* Allocate a Dynamic buffer, this buffer has to be deallocated after use.
  */
  if (position_to_use_ptr == NULL)
  {
    ret_value = FALSE;
    PDSM_MSG_ERROR("PDSM: Null use this position()",0,0,0);
    return ret_value;
  }
  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_use_this_position()",0,0,0);
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);
  
  cmd_ptr = &z_cmd_data;
  memset(cmd_ptr,0,sizeof(pdsm_cmd_s_type));

  /* Pointer to Command header and information.
  */
  pd_cmd_ptr   = &cmd_ptr->cmd.ext_pos;
  cmd_info_ptr = &pd_cmd_ptr->info;

  /* Initialize the command type field.
  */
  cmd_ptr->cmd_type = PDSM_CMD_TYPE_EXT_POS;

  /* Initialize the command header fields.
  */
  pd_cmd_ptr->cmd_cb_func = cmd_cb_func;
  pd_cmd_ptr->client_id = client_id;
  pd_cmd_ptr->data_block_ptr = client_data_ptr;
  pd_cmd_ptr->cmd = PDSM_EXT_USE_THIS_POS;

  memscpy(&cmd_info_ptr->pos_to_use, sizeof(cmd_info_ptr->pos_to_use), position_to_use_ptr, sizeof(*position_to_use_ptr));
  
  /* Initialize the Command error field. In case of errors found in this function
  ** CM will inform the clients by calling their callbacks immediately.
  */
  cmd_info_ptr->cmd_err       = cmd_err;

  /* Queue the command to PDSM Q */
  ret_value = pdsm_cmd_queue( cmd_ptr );
  if (ret_value == FALSE) 
  {
    cmd_err = PDSM_EXT_POS_CMD_ERR_NOERR;
    if ( cmd_cb_func != NULL )
    {
      cmd_cb_func( client_data_ptr, PDSM_EXT_USE_THIS_POS, cmd_err );
    } /* End if no callback is provided. */
  }
  
  return ret_value;

} /* pdsm_use_this_position */

/*===========================================================================

FUNCTION pdsm_get_pdapi_version
DESCRIPTION
  Gets the pdapi version number
DEPENDENCIES

RETURN VALUE
  PDSM Version number as a UINT32

SIDE EFFECTS

===========================================================================*/
uint32 pdsm_get_pdapi_version(void)
{
# if defined (PDSM_API_MAJ_V) && defined (PDSM_API_MIN_V)
  return (uint32) PDSM_VERSION_NUMBER_SEVEN;
#else 
  return 0;
#endif
}


/*===========================================================================

FUNCTION pdsm_xtra_set_data

DESCRIPTION
  gpsOneXTRA client calls this function to send parts of the XTRA file to GPS 
  engine.  
  
DEPENDENCIES

RETURN VALUE
  TRUE - Data part has been accepted
  FALSE - Data part has been rejected

SIDE EFFECTS

===========================================================================*/
boolean pdsm_xtra_set_data
(
  pdsm_xtra_cmd_cb_f_type    xtra_cb_func,      /* Command callback */
  pdsm_client_id_type        client_id,         /* Client id of user */
  void                       *client_data_ptr,  /* user data pointer */
  uint32                     xtra_data_len,     /* Length of XTRA data */
  unsigned char              *xtra_data_ptr,    /* XTRA data */
  uint8                      part_number,       /* data part number */
  uint8                      total_parts,       /* total data parts */
  pdsm_xtra_dc_status_e_type xtra_dc_status     /* status of download */
)
{
#ifdef FEATURE_CGPS_XTRA
  pdsm_cmd_s_type           *cmd_ptr;
  pdsm_xtra_cmd_s_type      *xtra_cmd_ptr;
  pdsm_xtra_cmd_info_s_type *cmd_info_ptr;
  pdsm_xtra_cmd_err_e_type  cmd_err = PDSM_XTRA_CMD_ERR_NOERR;
  pdsm_cmd_s_type           z_cmd_data;
  struct pdsm_client_s      *client_ptr;

  client_ptr = pdsmclient_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL)
  {
    return FALSE;
  }
  
  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_xtra_set_data()",0,0,0);
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);

  cmd_ptr = &z_cmd_data;
  
  /* Get the Command and info pointers.
  */
  xtra_cmd_ptr = &cmd_ptr->cmd.xtra;
  cmd_info_ptr = &xtra_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type = PDSM_CMD_TYPE_XTRA;

  /* Init the command header fields.
  */
  xtra_cmd_ptr->cmd_cb_func = xtra_cb_func;
  xtra_cmd_ptr->client_id = client_id;
  xtra_cmd_ptr->data_block_ptr = client_data_ptr;
  xtra_cmd_ptr->cmd = PDSM_XTRA_CMD_SET_DATA;

  client_ptr->xtra_data_ptr = client_data_ptr;
  /* Initialize the Information fields. cmd_err field is
  ** initialized so that the interface of passing command's to Session
  ** manager in spite of errors is available.
  */
  cmd_info_ptr->cmd_err = cmd_err;

  if (PDSM_XTRA_CMD_ERR_NOERR != tm_xtra_copy_data(part_number,xtra_data_ptr,xtra_data_len))
  {
    return FALSE;
  }

  cmd_info_ptr->xtra_data_ptr = xtra_data_ptr;
  cmd_info_ptr->xtra_data_len = xtra_data_len;
  cmd_info_ptr->xtra_dc_status = xtra_dc_status;
  cmd_info_ptr->part_number = part_number;
  cmd_info_ptr->total_parts = total_parts;

  /* Send the command to TM
  */
  (void) pdsm_cmd_queue( cmd_ptr );
 
  return TRUE;
#else
  return FALSE;
#endif /* FEATURE_CGPS_XTRA */
  
} /* End pdsm_xtra_set_data( ) */


/*===========================================================================

FUNCTION pdsm_xtra_client_initiate_download_request

DESCRIPTION
   gpsOneXTRA client calls this function when it needs to download an extended 
   assistance file but it does not have necessary information 
   (i.e. server URL, etc). Upon getting the request, TM should send 
   gpsOneXTRA PD API event which contains required information to the 
   gpsOneXTRA client.
   
DEPENDENCIES

RETURN VALUE
   TRUE - Initiate download request success
   FALSE - Initiate download request failed

SIDE EFFECTS

===========================================================================*/
boolean pdsm_xtra_client_initiate_download_request
(
  pdsm_xtra_cmd_cb_f_type   xtra_cb_func,       /* Command callback */
  pdsm_client_id_type       client_id,         /* Client id of user */
  void                      *client_data_ptr   /* user data pointer */
)
{
#ifdef FEATURE_CGPS_XTRA
  pdsm_cmd_s_type           *cmd_ptr;
  pdsm_xtra_cmd_s_type      *xtra_cmd_ptr;
  pdsm_xtra_cmd_info_s_type *cmd_info_ptr;
  pdsm_xtra_cmd_err_e_type  cmd_err = PDSM_XTRA_CMD_ERR_NOERR;
  pdsm_cmd_s_type           z_cmd_data;
  struct pdsm_client_s      *client_ptr;

  client_ptr = pdsmclient_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL)
  {
    return FALSE;
  }
  
  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_xtra_client_initiate_download_request()",0,0,0);
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);

  cmd_ptr = &z_cmd_data;
  /* Get the Command and info pointers.
  */
  xtra_cmd_ptr = &cmd_ptr->cmd.xtra;
  cmd_info_ptr = &xtra_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type = PDSM_CMD_TYPE_XTRA;

  /* Init the command header fields.
  */
  xtra_cmd_ptr->cmd_cb_func = xtra_cb_func;
  xtra_cmd_ptr->client_id = client_id;
  xtra_cmd_ptr->data_block_ptr = client_data_ptr;
  xtra_cmd_ptr->cmd = PDSM_XTRA_CMD_INITIATE_DOWNLOAD;
  client_ptr->xtra_data_ptr = client_data_ptr;

  /* Initialize the Information fields. cmd_err field is
  ** initialized so that the interface of passing command to Session
  ** manager in spite of errors is available.
  */
  cmd_info_ptr->cmd_err = cmd_err;

  (void) pdsm_cmd_queue( cmd_ptr );

  return TRUE;
#else  
  return FALSE;
#endif /* FEATURE_CGPS_XTRA */
} /* End pdsm_xtra_client_initiate_download_request( ) */


/*===========================================================================

FUNCTION pdsm_xtra_set_auto_download_params

DESCRIPTION
  gpsOneXTRA client calls this function to modify parameters related to the 
  automatic download functionality.  enable_auto_download flag is used 
  to enable or disable the functionality.  The download_interval parameter 
  specifies the number of hours between automatic downloads and its range 
  is 1-168 (168 = number of hours in 1 week).
  
DEPENDENCIES

RETURN VALUE
  TRUE - Auto Download Parameters are updated
  FALSE - Auto Download Parameters are NOT updated

SIDE EFFECTS

===========================================================================*/
boolean pdsm_xtra_set_auto_download_params
(
  pdsm_xtra_cmd_cb_f_type  xtra_cb_func,         /* Command callback */
  pdsm_client_id_type      client_id,            /* Client id of user */
  void                     *client_data_ptr,     /* user data pointer */
  boolean                  enable_auto_download, /* turn on/off auto download */
  uint16                   download_interval     /* time between downloads */
)
{
#ifdef FEATURE_CGPS_XTRA
  pdsm_cmd_s_type           *cmd_ptr;
  pdsm_xtra_cmd_s_type      *xtra_cmd_ptr;
  pdsm_xtra_cmd_info_s_type *cmd_info_ptr;
  pdsm_xtra_cmd_err_e_type  cmd_err = PDSM_XTRA_CMD_ERR_NOERR;
  pdsm_cmd_s_type           z_cmd_data;
  
  struct pdsm_client_s  *client_ptr;

  client_ptr = pdsmclient_get_client_ptr( client_id );

  /* verify client pointer */
  if ( (client_ptr == NULL) || (client_ptr->init_mask != PDSM_INIT_MASK) )
  {
    return FALSE;
  }
  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_xtra_set_auto_download_params()",0,0,0);
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);

  cmd_ptr = &z_cmd_data;

  /* Get the Command and info pointers.
  */
  xtra_cmd_ptr = &cmd_ptr->cmd.xtra;
  cmd_info_ptr = &xtra_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type = PDSM_CMD_TYPE_XTRA;

  /* Init the command header fields.
  */
  xtra_cmd_ptr->cmd_cb_func = xtra_cb_func;
  xtra_cmd_ptr->client_id = client_id;
  xtra_cmd_ptr->data_block_ptr = client_data_ptr;
  xtra_cmd_ptr->cmd = PDSM_XTRA_CMD_SET_AUTO_DOWNLOAD_PARAMS;

  /* Initialize the Information fields. cmd_err field is
  ** initialized so that the interface of passing command's to Session
  ** manager in spite of errors is available.
  */
  cmd_info_ptr->cmd_err = cmd_err;

  cmd_info_ptr->enable_auto_download = enable_auto_download;
  /* use the latest client data pointer.
  */
  client_ptr->xtra_data_ptr = client_data_ptr;
  cmd_info_ptr->download_interval = download_interval;

  (void) pdsm_cmd_queue( cmd_ptr );

  return TRUE;
#else
  return FALSE;
#endif /* FEATURE_CGPS_XTRA */
} /* End pdsm_xtra_set_auto_download_params( ) */


/*===========================================================================

FUNCTION pdsm_xtra_query_data_validity

DESCRIPTION
  gpsOneXTRA client calls this function to request validity information. 
  Upon getting the request, the starting GPS week and GPS minutes 
  along with the duration of the current extended assistance data 
  will be sent to the gpsOneXTRA client.  
  
  If there is no data in the subsystem, the values of the start time and 
  duration will be set to 0.
  
DEPENDENCIES

RETURN VALUE
  TRUE - request is a success
  FALSE - request failed

SIDE EFFECTS

===========================================================================*/
boolean pdsm_xtra_query_data_validity
(
  pdsm_xtra_cmd_cb_f_type  xtra_cb_func,       /* Command callback */
  pdsm_client_id_type      client_id,          /* Client id of user */
  void                     *client_data_ptr    /* user data pointer */
)
{
#ifdef FEATURE_CGPS_XTRA
  pdsm_cmd_s_type           *cmd_ptr;
  pdsm_xtra_cmd_s_type      *xtra_cmd_ptr;
  pdsm_xtra_cmd_info_s_type *cmd_info_ptr;
  pdsm_xtra_cmd_err_e_type  cmd_err = PDSM_XTRA_CMD_ERR_NOERR;
  pdsm_cmd_s_type           z_cmd_data;
  
  struct pdsm_client_s  *client_ptr;

  client_ptr = pdsmclient_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL)
  {
    return FALSE;
  }
  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_xtra_query_data_validity()",0,0,0);
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);

  cmd_ptr = &z_cmd_data;
  
  /* Get the Command and info pointers.
  */
  xtra_cmd_ptr = &cmd_ptr->cmd.xtra;
  cmd_info_ptr = &xtra_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type = PDSM_CMD_TYPE_XTRA;

  /* Init the command header fields.
  */
  xtra_cmd_ptr->cmd_cb_func = xtra_cb_func;
  xtra_cmd_ptr->client_id = client_id;
  xtra_cmd_ptr->data_block_ptr = client_data_ptr;
  xtra_cmd_ptr->cmd = PDSM_XTRA_CMD_QUERY_DATA_VALIDITY;

  /* Initialize the Information fields. cmd_err field is
  ** initialized so that the interface of passing command's to Session
  ** manager in spite of errors is available.
  */
  cmd_info_ptr->cmd_err = cmd_err;

  /* use the latest client data pointer.
  */
  client_ptr->xtra_data_ptr = client_data_ptr;
  (void) pdsm_cmd_queue( cmd_ptr );

  return TRUE;
#else
  return FALSE;
#endif /* FEATURE_CGPS_XTRA */
} /* End pdsm_xtra_query_data_validity( ) */


/*===========================================================================

FUNCTION pdsm_xtra_inject_time_info

DESCRIPTION
   gpsOneXTRA client calls this function to inject XTRA Time
   
DEPENDENCIES

RETURN VALUE
    TRUE - Time injection success
    FALSE - Time injection failed

SIDE EFFECTS

===========================================================================*/
boolean pdsm_xtra_inject_time_info
(
  pdsm_xtra_cmd_cb_f_type  xtra_cb_func,      /* Command callback */
  pdsm_client_id_type      client_id,         /* Client id of user */
  void                     *client_data_ptr,  /* user data pointer */
  pdsm_xtra_time_info_type *time_info_ptr     /* pointer to time information */
)
{
#ifdef FEATURE_CGPS_XTRA
  pdsm_cmd_s_type           *cmd_ptr;
  pdsm_xtra_cmd_s_type      *xtra_cmd_ptr;
  pdsm_xtra_cmd_info_s_type *cmd_info_ptr;
  pdsm_xtra_cmd_err_e_type  cmd_err = PDSM_XTRA_CMD_ERR_NOERR;
  pdsm_cmd_s_type           z_cmd_data;
  struct pdsm_client_s  *client_ptr;

  client_ptr = pdsmclient_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL)
  {
    return FALSE;
  }

  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_xtra_inject_time_info()",0,0,0);
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);

  cmd_ptr = &z_cmd_data;
  
  /* Get the Command and info pointers.
  */
  xtra_cmd_ptr = &cmd_ptr->cmd.xtra;
  cmd_info_ptr = &xtra_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type = PDSM_CMD_TYPE_XTRA;

  /* Init the command header fields.
  */
  xtra_cmd_ptr->cmd_cb_func = xtra_cb_func;
  xtra_cmd_ptr->client_id = client_id;
  xtra_cmd_ptr->data_block_ptr = client_data_ptr;
  xtra_cmd_ptr->cmd = PDSM_XTRA_CMD_TIME_INFO_INJ;
  client_ptr->xtra_data_ptr = client_data_ptr;

  /* Initialize the Information fields. cmd_err field is
  ** initialized so that the interface of passing command's to Session
  ** manager in spite of errors is available.
  */
  cmd_info_ptr->cmd_err = cmd_err;

  if (time_info_ptr)
  {
    cmd_info_ptr->d_TimeMsec     = (double)time_info_ptr->d_TimeMsec;
    cmd_info_ptr->f_TimeUncMsec  = (float)time_info_ptr->f_TimeUncMsec;
    cmd_info_ptr->b_RefToUtcTime = time_info_ptr->b_RefToUtcTime;
    cmd_info_ptr->b_ForceFlag    = time_info_ptr->b_ForceFlag;
  }

  (void) pdsm_cmd_queue( cmd_ptr );

  return TRUE;
#else  
  return FALSE;
#endif /* FEATURE_CGPS_XTRA s*/
} /* End pdsm_xtra_inject_time_info( ) */


/*===========================================================================

FUNCTION pdsm_xtra_t_get_data

DESCRIPTION
   XTRA-T client calls this function to get CellDB data from GPS Engine.
   
DEPENDENCIES

RETURN VALUE
    TRUE  - XTRA-T get data command success
    FALSE - XTRA-T get data command failure

SIDE EFFECTS

===========================================================================*/
boolean pdsm_xtra_t_get_data
(
  pdsm_xtra_t_cmd_cb_f_type   xtra_t_cb_func,      /* Command callback */
  void                       *client_data_ptr,     /* Not sure if this is required */
  pdsm_xtra_t_get_s_type      xtra_t_get_type,     /* Parameter mask to be set */
  pdsm_client_id_type         client_id            /* Requesting client id */
)
{
#ifdef FEATURE_CGPS_XTRA_T
  pdsm_cmd_s_type             *cmd_ptr;
  pdsm_xtra_t_cmd_s_type      *xtra_t_cmd_ptr;
  pdsm_xtra_t_cmd_info_s_type *xtra_t_cmd_info_ptr;
  pdsm_xtra_t_cmd_err_e_type   cmd_err = PDSM_XTRA_T_CMD_ERR_NONE;
  pdsm_cmd_s_type              z_cmd_data;

  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_xtra_t_get_data()",0,0,0);
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);

  cmd_ptr = &z_cmd_data;
  
  /* Get the Command and info pointers.
  */
  xtra_t_cmd_ptr = &cmd_ptr->cmd.xtra_t;
  xtra_t_cmd_info_ptr = &xtra_t_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type = PDSM_CMD_TYPE_XTRA_T;

  /* Init the command header fields.
  */
  xtra_t_cmd_ptr->cmd_cb_func = xtra_t_cb_func;
  xtra_t_cmd_ptr->data_block_ptr = client_data_ptr;
  xtra_t_cmd_ptr->client_id = client_id;
  /* NOTE: xtra_t_cmd_ptr->client is updated in tm_pdapi_cmd_proc() */
  xtra_t_cmd_ptr->cmd = PDSM_XTRA_T_CMD_GET_DATA;

  /* Initialize the Information fields. cmd_err field is
  ** initialized so that the interface of passing command's to Session
  ** manager in spite of errors is available.
  */
  xtra_t_cmd_info_ptr->cmd_err  = cmd_err;
  xtra_t_cmd_info_ptr->get_data = xtra_t_get_type;

  (void) pdsm_cmd_queue( cmd_ptr );

  return TRUE;
#else  
  if ( xtra_t_cb_func != NULL )
   {
     xtra_t_cb_func(  client_data_ptr, 
                      PDSM_XTRA_T_CMD_GET_DATA, 
                      PDSM_XTRA_T_CMD_FEATURE_NOT_ENABLED);
   }
  return FALSE;
#endif /* FEATURE_CGPS_XTRA_T */
}
/* End pdsm_xtra_t_get_data ( ) */

/*===========================================================================

FUNCTION pdsm_send_wiper_position_report

DESCRIPTION
   gpsOne WIPER client calls this function to report Wiper position report
   
DEPENDENCIES

RETURN VALUE
    
SIDE EFFECTS

===========================================================================*/
void pdsm_send_wiper_position_report(
    pdsm_wiper_cmd_cb_f_type  wiper_cb_func,       /* Client command callback funct. */
    pdsm_client_id_type      client_id,          /* Client id of user */
    void                     *client_data_ptr,   /* Pointer to client data block */
    t_wiper_position_report_struct_type * position_report_ptr      /* Pointer to position report data */
)
{
  t_wiper_position_report_ex_struct_type pos_report_ex;

  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("Warning ! Deprecated call pdsm_send_wiper_position_report()",0,0,0);
  PDSM_MSG_HIGH("redirecting to pdsm_send_wiper_position_report_ex()",0,0,0);
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);

  if (NULL == position_report_ptr) {
    PDSM_MSG_HIGH("NULL position report ptr",0,0,0);
    return;
  }
  
  memscpy(&(pos_report_ex.wiper_pos_report), sizeof(pos_report_ex.wiper_pos_report),
    position_report_ptr, sizeof(*position_report_ptr));
  pos_report_ex.e_VertRelIndicator = PDSM_POSITION_RELIABILITY_NOT_SET;
  pos_report_ex.e_HoriRelIndicator = PDSM_POSITION_RELIABILITY_NOT_SET;

  pdsm_send_wiper_position_report_ex(wiper_cb_func, client_id,
    client_data_ptr, &pos_report_ex);
  return;
}

/*===========================================================================

FUNCTION pdsm_send_wiper_position_report

DESCRIPTION
   gpsOne WIPER client calls this function to report Wiper position report
   
DEPENDENCIES

RETURN VALUE
    
SIDE EFFECTS

===========================================================================*/
void pdsm_send_wiper_position_report_ex(
    pdsm_wiper_cmd_cb_f_type  wiper_cb_func,       /* Client command callback funct. */
    pdsm_client_id_type      client_id,          /* Client id of user */
    void                     *client_data_ptr,   /* Pointer to client data block */
    t_wiper_position_report_ex_struct_type * position_report_ptr      /* Pointer to position report data */
)
{
  pdsm_cmd_s_type           *cmd_ptr;
  pdsm_wiper_cmd_s_type      *wiper_cmd_ptr;
  pdsm_wiper_cmd_info_s_type *wiper_cmd_info_ptr;
  pdsm_wiper_cmd_err_e_type  cmd_err = PDSM_WIPER_CMD_ERR_NOERR;

  pdsm_cmd_s_type           z_cmd_data; 

  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_send_wiper_position_report_ex()",0,0,0);
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);

  cmd_ptr = &z_cmd_data;
  
  /* Get the Command and info pointers.
  */
  wiper_cmd_ptr = &cmd_ptr->cmd.wiper;
  wiper_cmd_info_ptr = &wiper_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type = PDSM_CMD_TYPE_WIPER;

  /* Init the command header fields.
  */
  wiper_cmd_ptr->cmd_cb_func = wiper_cb_func;
  wiper_cmd_ptr->client_id = client_id;
  wiper_cmd_ptr->data_block_ptr = client_data_ptr;
  wiper_cmd_ptr->cmd = PDSM_WIPER_CMD_LOG_POSITION_REPORT;

  /* Initialize the Information fields. cmd_err field is
  ** initialized so that the interface of passing command's to Session
  ** manager in spite of errors is available.
  */
  wiper_cmd_info_ptr->cmd_err = cmd_err;

  wiper_cmd_info_ptr->position_report_ptr = &z_wiper_position_report_ex_struct_data;

  memscpy(wiper_cmd_info_ptr->position_report_ptr, sizeof(*wiper_cmd_info_ptr->position_report_ptr),
          position_report_ptr, sizeof(*position_report_ptr));

  (void) pdsm_cmd_queue( cmd_ptr );

  return;
} /* End pdsm_send_wiper_position_report( ) */


/*===========================================================================

FUNCTION pdsm_pd_inject_external_spi

DESCRIPTION
   gpsOne PDAPI client calls this to inject motion information (XSPI) in 
   response to PDSM_PD_EVENT_REQUEST_EXTERNAL_SPI_INJECTION
   
DEPENDENCIES

RETURN VALUE
   TRUE if the request was successfully queued
   FALSE otherwise
    
SIDE EFFECTS

===========================================================================*/
boolean pdsm_pd_inject_external_spi(
   pdsm_pd_cmd_cb_f_type   pd_xspi_cb_func, /* command result callback function */
    /*~ PARAM pd_xspi_cb_func POINTER */
   pdsm_client_id_type   client_id,
   void   *client_data_ptr,
   pdsm_pd_external_spi_type    *ext_spi_ptr
    /*~ PARAM ext_spi_ptr POINTER */
)
{
  boolean b_retval = TRUE;
  pdsm_cmd_s_type        z_cmd_data;
  struct pdsm_client_s  *client_ptr = NULL;

  memset(&z_cmd_data, 0, sizeof(z_cmd_data));
  client_ptr = pdsmclient_get_client_ptr( client_id );
  if((NULL == client_ptr) || (NULL == ext_spi_ptr))
  {
    b_retval = FALSE;
  }
  else
  {
     PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
     PDSM_MSG_HIGH("pdsm_pd_inject_external_spi()",0,0,0);
     PDSM_MSG_HIGH("------------------------------------------",0,0,0);

     /* pack data for pdsm_cmd_queue */
     z_cmd_data.cmd_type = PDSM_CMD_TYPE_XSPI;
     z_cmd_data.cmd.xspi.client_id = client_id;
     z_cmd_data.cmd.xspi.client_data_ptr = client_data_ptr;
     z_cmd_data.cmd.xspi.cmd_cb_func = pd_xspi_cb_func;
     z_cmd_data.cmd.xspi.xspi_data = *ext_spi_ptr;

     b_retval = pdsm_cmd_queue(&z_cmd_data);

  } /* end else (NULL == client_ptr) */
  return b_retval;
} /* end pdsm_pd_inject_external_spi() */

/*===========================================================================

FUNCTION pdsm_pd_inject_motion_data

DESCRIPTION
   Inject motion data information from client. This is meant to replace XSPI
   as a more detailed API.

DEPENDENCIES

RETURN VALUE
   TRUE if the request was successfully queued
   FALSE otherwise

SIDE EFFECTS

===========================================================================*/
boolean pdsm_pd_inject_motion_data(
   pdsm_pd_cmd_cb_f_type   pd_motion_data_cb_func, /* command result callback function */
    /*~ PARAM pa_motion_data_cb_func POINTER */
   pdsm_client_id_type   client_id,
   void   *client_data_ptr,
   pdsm_pd_motion_data_s_type    *motion_data_ptr
    /*~ PARAM motion_data_ptr POINTER */
)
{
  boolean b_retval = TRUE;
  pdsm_cmd_s_type        z_cmd_data;
  struct pdsm_client_s  *client_ptr = NULL;

  memset(&z_cmd_data, 0, sizeof(z_cmd_data));
  client_ptr = pdsmclient_get_client_ptr( client_id );
  if((NULL == client_ptr) || (NULL == motion_data_ptr))
  {
    b_retval = FALSE;
  }
  else
  {
     PDSM_MSG_MED("--------- GPS Command --------------------",0,0,0);
     PDSM_MSG_MED("pdsm_pd_inject_motion_data()",0,0,0);
     PDSM_MSG_MED("------------------------------------------",0,0,0);

     /* pack data for pdsm_cmd_queue */
     z_cmd_data.cmd_type = PDSM_CMD_TYPE_MOTION;
     z_cmd_data.cmd.motion.client_id = client_id;
     z_cmd_data.cmd.motion.client_data_ptr = client_data_ptr;
     z_cmd_data.cmd.motion.cmd_cb_func = pd_motion_data_cb_func;
     z_cmd_data.cmd.motion.motion_data = *motion_data_ptr;

     b_retval = pdsm_cmd_queue(&z_cmd_data);

  } /* end else (NULL == client_ptr) */
  return b_retval;
} /* end pdsm_pd_inject_motion_data() */

/*===========================================================================

FUNCTION pdsm_start_periodic_ppm_report

DESCRIPTION
  Ask PDSM to start a periodic Pilot Phase Measurement Report. A periodic
  PPM report will be pumped out as a PDAPI EXT event once this API is called.

  The client must register for this PDAPI EXT event by calling 
  pdsm_client_ext_status_reg and registering for the appropriate event.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  pdsm_init( ) must have been already done.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM event.

===========================================================================*/
boolean pdsm_start_periodic_ppm_report(
   pdsm_pd_cmd_cb_f_type    cmd_cb_func,        /* Command callback */
   void                     *client_data_ptr,   /* user data pointer */
   uint16                   period,             /* time bet. two ppm repts */
                                                /* range 1 second to 1 hour */
   pdsm_client_id_type      client_id           /* Client id of user */
)
{
/* If  PERIODIC PPM features not defined, return error */
#if (!defined(FEATURE_GNSS_PERIODIC_PPM))
  if ( cmd_cb_func != NULL )
  {
    cmd_cb_func(  client_data_ptr, 
                  PDSM_PD_CMD_PERIODIC_PPM_START, 
                  PDSM_PD_CMD_ERR_FEATURE_NOT_ENABLED );
  }

  return FALSE;
#else
  #error code not present
#endif  /* FEATURE_GNSS_PERIODIC_PPM */
}


/*===========================================================================

FUNCTION pdsm_stop_periodic_ppm_report

DESCRIPTION
  Ask PDSM to stop a previously started periodic Pilot Phase Measurement 
  Report.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  pdsm_init( ) must have been already done.
  pdsm_start_periodic_ppm_report( ) must already have been done

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM event.

===========================================================================*/
boolean pdsm_stop_periodic_ppm_report(
   pdsm_pd_cmd_cb_f_type    cmd_cb_func,         /* Command callback */
  /*~ PARAM pd_cb_func POINTER */
   void                     *client_data_ptr,   /* user data pointer */
   pdsm_client_id_type      client_id           /* Client id of user */
)
{
/* If PERIODIC PPM features not defined, return error */
#if (!defined(FEATURE_GNSS_PERIODIC_PPM))
  if ( cmd_cb_func != NULL )
  {
    cmd_cb_func(  client_data_ptr, 
                  PDSM_PD_CMD_PERIODIC_PPM_STOP, 
                  PDSM_PD_CMD_ERR_FEATURE_NOT_ENABLED );
  }

  return FALSE;
#else
  #error code not present
#endif  /* FEATURE_GNSS_PERIODIC_PPM */
}

/*===========================================================================

FUNCTION pdsm_reset_location_service

DESCRIPTION
   Command PDSM to Location Service Reset GPS.

DEPENDENCIES

RETURN VALUE
   TRUE if the request was successfully queued
   FALSE otherwise

SIDE EFFECTS

===========================================================================*/
boolean pdsm_reset_location_service(
  pdsm_pd_cmd_cb_f_type pd_reset_location_service_cb_func,  /* command result callback function */
  /*~ PARAM pd_reset_location_service_cb_func POINTER */
  pdsm_client_id_type   client_id,
  void   *client_data_ptr
)
{
  pdsm_cmd_s_type         *cmd_ptr;
  pdsm_pd_cmd_s_type      *pd_cmd_ptr;
  pdsm_pd_cmd_info_s_type *cmd_info_ptr;
  pdsm_pd_cmd_err_e_type  cmd_err = PDSM_PD_CMD_ERR_NOERR;
  pdsm_cmd_s_type         z_cmd_data;
  boolean                 ret_value = FALSE;
  struct pdsm_client_s  *client_ptr;

  client_ptr = pdsmclient_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL) 
  {
    MSG_ERROR("Invalid Client Id",0,0,0);
    return ret_value;
  }

  PDSM_MSG_LOW("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_reset_location_service(). CDPtr %p", client_data_ptr, 0, 0); 
  PDSM_MSG_LOW("------------------------------------------",0,0,0);
  
  cmd_ptr = &z_cmd_data;
  memset(cmd_ptr,0,sizeof(pdsm_cmd_s_type));

  /* Get the Command and info pointer's.
  */
  pd_cmd_ptr = &cmd_ptr->cmd.pd;
  cmd_info_ptr = &pd_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type = PDSM_CMD_TYPE_PD;

  /* Init the command header fields.
  */
  pd_cmd_ptr->cmd_cb_func = pd_reset_location_service_cb_func;
  pd_cmd_ptr->client_id = client_id;
  pd_cmd_ptr->data_block_ptr = client_data_ptr;
  pd_cmd_ptr->cmd = PDSM_PD_CMD_RESET_LOCATION_SERVICE;

  /* use the latest client data pointer.
  */
  client_ptr->pd_data_ptr = client_data_ptr;
  /* Initialize the Information fields. cmd_err field is
  ** initialized so that the interface of passing command's to Session
  ** manager in spite of errors is available.
  */
  cmd_info_ptr->cmd_err = cmd_err;

  ret_value = pdsm_cmd_queue( cmd_ptr );
  if (ret_value == FALSE) 
  {    
    MSG_ERROR("Failed to queue pdsm command",0,0,0);
    if ( pd_reset_location_service_cb_func != NULL )
    {
      pd_reset_location_service_cb_func( client_data_ptr, PDSM_PD_CMD_RESET_LOCATION_SERVICE, PDSM_PD_CMD_ERR_NO_BUF_L );
    } /* End if no callback is provided. */
  }

  return ret_value;
}

/*===========================================================================

FUNCTION pdsm_pd_get_best_avail_pos

DESCRIPTION
  This API initiates a command to the engine to return the best available position. 
The response to this request is an asynchronous fix report sent to PDAPI. There are 
no assumptions made on the response delay for this fix .  

DEPENDENCIES

RETURN VALUE
   TRUE if the request was successfully queued
   FALSE otherwise

SIDE EFFECTS

===========================================================================*/
boolean pdsm_pd_get_best_avail_pos
(
   pdsm_pd_cmd_cb_f_type cmd_cb_func, /* command result callback function */
   /*~ PARAM pa_motion_data_cb_func POINTER */
   pdsm_client_id_type   client_id,
   void                 *client_data_ptr /* client provided reference data pointer */
)
{
  pdsm_cmd_s_type         *cmd_ptr;
  pdsm_pd_cmd_s_type      *pd_cmd_ptr;
  //pdsm_pd_cmd_info_s_type *cmd_info_ptr;  /* Removing set-but-not-used variable */
  pdsm_cmd_s_type         z_cmd_data;
  boolean                 ret_value = FALSE;
  struct pdsm_client_s  *client_ptr;

  client_ptr = pdsmclient_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL)
  {
    return ret_value;
  }

  PDSM_MSG_LOW("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_pd_get_best_avail_pos(). CDPtr %p", client_data_ptr, 0, 0); 
  PDSM_MSG_LOW("------------------------------------------",0,0,0);

  cmd_ptr = &z_cmd_data;
  memset(cmd_ptr,0,sizeof(pdsm_cmd_s_type));

  /* Pointer to Command header and information.
  */
  pd_cmd_ptr   = &cmd_ptr->cmd.pd;
  //cmd_info_ptr = &pd_cmd_ptr->info;  /* Removing set-but-not-used variable */
  
  /* Initialize the command type field.
  */
  cmd_ptr->cmd_type = PDSM_CMD_TYPE_PD;

  /* Initialize the command header fields.
  */
  pd_cmd_ptr->cmd_cb_func = cmd_cb_func;
  pd_cmd_ptr->client_id = client_id;
  pd_cmd_ptr->data_block_ptr = client_data_ptr;
  pd_cmd_ptr->cmd = PDSM_PD_CMD_GET_BEST_AVAIL_POS;

  /* use the latest client data pointer.
  */
  client_ptr->pd_data_ptr = client_data_ptr;
  ret_value = pdsm_cmd_queue( cmd_ptr );
  
  if (ret_value == FALSE) 
  {
    if ( cmd_cb_func != NULL )
    {
      cmd_cb_func( client_data_ptr, PDSM_PD_CMD_GET_BEST_AVAIL_POS, PDSM_PD_CMD_ERR_NO_BUF_L );
    } /* End if no callback is provided. */
  }
  return ret_value;
}

/*===========================================================================

FUNCTION pdsm_pd_get_avail_wwan_pos

DESCRIPTION
  This API initiates a command to the engine to return the available WWAN position. 
The response to this request is an asynchronous fix report sent to PDAPI. There are 
no assumptions made on the response delay for this fix.

DEPENDENCIES

RETURN VALUE
   TRUE if the request was successfully queued
   FALSE otherwise

SIDE EFFECTS

===========================================================================*/
boolean pdsm_pd_get_avail_wwan_pos
(
  pdsm_pd_cmd_cb_f_type   cmd_cb_func, /* command result callback function */
  /*~ PARAM pa_motion_data_cb_func POINTER */
  pdsm_client_id_type     client_id,
  void                   *client_data_ptr /* client provided reference data pointer */
)
{
  pdsm_cmd_s_type         *cmd_ptr;
  pdsm_pd_cmd_s_type      *pd_cmd_ptr;
  pdsm_pd_cmd_info_s_type *cmd_info_ptr;
  pdsm_cmd_s_type         z_cmd_data;
  boolean                 ret_value = FALSE;
  struct pdsm_client_s   *client_ptr;

  client_ptr = pdsmclient_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL)
  {
    return ret_value;
  }

  PDSM_MSG_LOW("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_pd_get_avail_wwan_pos(). CDPtr %p", client_data_ptr, 0, 0); 
  PDSM_MSG_LOW("------------------------------------------",0,0,0);

  cmd_ptr = &z_cmd_data;
  memset(cmd_ptr,0,sizeof(pdsm_cmd_s_type));

  /* Pointer to Command header and information.
  */
  pd_cmd_ptr   = &cmd_ptr->cmd.pd;
  cmd_info_ptr = &pd_cmd_ptr->info;

  /* Initialize the command type field.
  */
  cmd_ptr->cmd_type = PDSM_CMD_TYPE_PD;

  /* Initialize the command header fields.
  */
  pd_cmd_ptr->cmd_cb_func = cmd_cb_func;
  pd_cmd_ptr->client_id = client_id;
  pd_cmd_ptr->data_block_ptr = client_data_ptr;
  pd_cmd_ptr->cmd = PDSM_PD_CMD_GET_AVAIL_WWAN_POS;

  /* use the latest client data pointer.
  */
  client_ptr->pd_data_ptr = client_data_ptr;
  ret_value = pdsm_cmd_queue( cmd_ptr );
  
  if (ret_value == FALSE) 
  {
    if ( cmd_cb_func != NULL )
    {
      cmd_cb_func( client_data_ptr, PDSM_PD_CMD_GET_AVAIL_WWAN_POS, PDSM_PD_CMD_ERR_NO_BUF_L );
    } /* End if no callback is provided. */
  }
  return ret_value;
}

/*===========================================================================

FUNCTION pdsm_pd_get_available_pos

DESCRIPTION
  This API initiates a command to the engine to return the available  position. 
The response to this request is an asynchronous fix report sent to PDAPI. There are 
no assumptions made on the response delay for this fix.

DEPENDENCIES

RETURN VALUE
   TRUE if the request was successfully queued
   FALSE otherwise

SIDE EFFECTS

===========================================================================*/
boolean pdsm_pd_get_available_pos
(
  pdsm_pd_cmd_cb_f_type   cmd_cb_func, /* command result callback function */
  /*~ PARAM pa_motion_data_cb_func POINTER */
  pdsm_client_id_type     client_id,
  void                   *client_data_ptr /* client provided reference data pointer */
)
{
  pdsm_cmd_s_type   *cmd_ptr = NULL;
  pdsm_pd_cmd_s_type  *pd_cmd_ptr = NULL;
  pdsm_pd_cmd_info_s_type *cmd_info_ptr = NULL;
  pdsm_cmd_s_type   z_cmd_data = {0};
  boolean ret_value = TRUE;
  struct pdsm_client_s   *client_ptr = NULL;

  client_ptr = pdsmclient_get_client_ptr( client_id );

  /* verify client pointer */
  if (NULL == client_ptr)
  {
    return PDSM_CLIENT_ERR_CLIENT_ID;
  }

  PDSM_MSG_LOW("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_pd_get_available_pos(). CDPtr %p", client_data_ptr, 0, 0); 
  PDSM_MSG_HIGH("pdsm_pd_get_available_pos(). Data Source %d", *((uint32 *)client_data_ptr), 0, 0); 
  PDSM_MSG_LOW("------------------------------------------",0,0,0);

  cmd_ptr = &z_cmd_data;
  memset(cmd_ptr,0,sizeof(pdsm_cmd_s_type));

  /* Pointer to Command header and information.
  */
  pd_cmd_ptr   = &cmd_ptr->cmd.pd;
  cmd_info_ptr = &pd_cmd_ptr->info;

  /* Initialize the command type field.
  */
  cmd_ptr->cmd_type = PDSM_CMD_TYPE_PD;

  /* Initialize the command header fields.
  */
  pd_cmd_ptr->cmd_cb_func = cmd_cb_func;
  pd_cmd_ptr->client_id = client_id;
  pd_cmd_ptr->data_block_ptr = client_data_ptr;
  pd_cmd_ptr->cmd = PDSM_PD_CMD_GET_AVAIL_POS_FOR_SRC;


  /* use the latest client data pointer.
  */
  client_ptr->pd_data_ptr = client_data_ptr;
  ret_value = pdsm_cmd_queue( cmd_ptr );
  
  if (FALSE == ret_value) 
  {
    if (NULL != cmd_cb_func)
    {
      cmd_cb_func( client_data_ptr, PDSM_PD_CMD_GET_AVAIL_POS_FOR_SRC, PDSM_PD_CMD_ERR_NO_BUF_L );
    } /* End if no callback is provided. */
  }
  return ret_value;
}
boolean pdsm_enable_dpo_status_report(boolean enable)
{
#ifdef FEATURE_GNSS_DPO_STATUS_REPORT
  pdsm_cmd_s_type z_cmd_data;

  PDSM_MSG_MED("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_enable_dpo_status_report() enable %d",enable,0,0);
  PDSM_MSG_MED("------------------------------------------",0,0,0);

  /*  pack data for pdsm_cmd_queue */
  z_cmd_data.cmd_type = PDSM_CMD_TYPE_DPO_STATUS_RPT;
  z_cmd_data.cmd.dpo_status_rpt.enable = enable;
  return(pdsm_cmd_queue(&z_cmd_data));
#else
  return FALSE;
#endif /* FEATURE_GNSS_DPO_STATUS_REPORT */
}

/*
******************************************************************************
* pdsm_read_qmi_services_config
*
* Function description:
*
*  This synchronous API is called by QMI services to read QMI configuration at 
*  bootup. This function may be called only after PDAPI is initialized. The QMI 
*  configuration is used by the caller to register with PDAPI 
*
* Parameters:
*  qmi_Config : Configuration for QMI initialization
*
* Return value:
*  TRUE  - Request processed successfully
*  FALSE - Request could not be processed
*
* ******************************************************************************
*/
boolean pdsm_read_qmi_services_config( pdsm_QMIServiceConfig *qmi_Config )
{
  PDSM_MSG_MED("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_read_qmi_services_config() Query",0,0,0);
  PDSM_MSG_MED("------------------------------------------",0,0,0);

  if( NULL == qmi_Config )
  {
    MSG_ERROR("qmi_Config is NULL", 0, 0, 0);
    return FALSE;
  }

  /* zero out the input struct */
  memset(qmi_Config, 0, sizeof(*qmi_Config));

  if ( FALSE == pdsm_get_ready_flag() )
  {
    MSG_ERROR("PDSM not initialized yet", 0, 0, 0);
    return FALSE;
  }

  tm_read_qmi_services_configuration( qmi_Config );

  return TRUE;
}

/*===========================================================================

FUNCTION pdsm_pd_inject_external_cell_info

DESCRIPTION
  PDSM client calls this function to inject external cell information to SM
  Typically this is the cell info updated from HLOS for external/3rd-party
  modem. This external cell info will be used in SUPL if it's valid, otherwise
  CM's cell info will be used (typically for internal modem)
 
  The supplied callback function pd_cb_func will be called to report this
  command's success/failure status. There will be no PDAPI event for this command.
 
DEPENDENCIES

RETURN VALUE
  TRUE - request is a success
  FALSE - request failed
  
SIDE EFFECTS

===========================================================================*/
boolean pdsm_pd_inject_external_cell_info(
  pdsm_pd_cmd_cb_f_type    pd_cb_func,          /* Command callback */
  void                     *client_data_ptr,    /* user data pointer */
  pdsm_client_id_type      client_id,           /* Client id of user */
  pdsm_cell_info_s_type    *cell_info           /* Pointer to external cell info */
)
{
  boolean b_retval = TRUE;
  pdsm_cmd_s_type        z_cmd_data;
  struct pdsm_client_s  *client_ptr = NULL;

  memset(&z_cmd_data, 0, sizeof(z_cmd_data));
  client_ptr = pdsmclient_get_client_ptr( client_id );

  if((NULL == client_ptr) || (NULL == cell_info))
  {
    b_retval = FALSE;
  }
  else
  {
    PDSM_MSG_MED("--------- GPS Command --------------------",0,0,0);
    PDSM_MSG_MED("pdsm_pd_inject_external_cell_info()",0,0,0);
    PDSM_MSG_MED("------------------------------------------",0,0,0);

    /* pack data for pdsm_cmd_queue */
    z_cmd_data.cmd_type = PDSM_CMD_TYPE_EXT_CELL;
    z_cmd_data.cmd.ext_cell.client_id = client_id;
    z_cmd_data.cmd.ext_cell.client_data_ptr = client_data_ptr;
    z_cmd_data.cmd.ext_cell.cmd_cb_func = pd_cb_func;
    z_cmd_data.cmd.ext_cell.cell_info = *cell_info;

    b_retval = pdsm_cmd_queue(&z_cmd_data);

    if (b_retval == FALSE) 
    {
      MSG_ERROR("Failed to queue pdsm command INJECT_CELL_INFO",0,0,0);
      if ( pd_cb_func != NULL )
      {
        pd_cb_func( client_data_ptr, PDSM_PD_CMD_INJECT_CELL_INFO, PDSM_PD_CMD_ERR_NO_BUF_L );
      } /* End if no callback is provided. */
    }

  }

  return b_retval;
}


/*===========================================================================

FUNCTION pdsm_pd_inject_ni_message

DESCRIPTION
  PDSM client calls this function to inject NI message to SM
  This is typically used to inject SUPL NI SMS received from HLOS.
  It can be expanded to support other SMS like V1/V2, or NI message from
  other transport like UDP, etc.
 
  The supplied callback function pd_cb_func will be called to report this
  command's success/failure status. There will be no PDAPI event for this command.
 
DEPENDENCIES

RETURN VALUE
  TRUE - request is a success
  FALSE - request failed
  
SIDE EFFECTS

===========================================================================*/
extern boolean pdsm_pd_inject_ni_message(
  pdsm_pd_cmd_cb_f_type    pd_cb_func,        /* Command callback */
  /*~ PARAM pd_cb_func POINTER */
  void                     *client_data_ptr,   /* user data pointer */
  pdsm_client_id_type      client_id,          /* Client id of user */
  pdsm_ni_message_s_type   *ni_msg             /* NI message */
  /*~ PARAM ni_info POINTER */
)
{
#ifdef FEATURE_CGPS_PDCOMM 
  boolean b_retval = TRUE;
  pdsm_cmd_s_type        z_cmd_data;
  struct pdsm_client_s  *client_ptr = NULL;
  pdsm_ni_message_data_cmd_s_type *msg_ptr;

  memset(&z_cmd_data, 0, sizeof(z_cmd_data));
  client_ptr = pdsmclient_get_client_ptr( client_id );

  if ( (NULL == client_ptr) || (NULL == ni_msg) || 
       (ni_msg->len == 0) || (ni_msg->len > PDSM_MAX_NI_MSG_LEN))
  {
    b_retval = FALSE;
  }
  else
  {
    PDSM_MSG_MED("--------- GPS Command --------------------",0,0,0);
    PDSM_MSG_MED("pdsm_pd_inject_ni_message()",0,0,0);
    PDSM_MSG_MED("------------------------------------------",0,0,0);

    msg_ptr = &z_cmd_data.cmd.ni_msg;
    /* pack data for pdsm_cmd_queue */
    z_cmd_data.cmd_type = PDSM_CMD_TYPE_NI_MSG;
    msg_ptr->client_id = client_id;
    msg_ptr->client_data_ptr = client_data_ptr;
    msg_ptr->cmd_cb_func = pd_cb_func;
    msg_ptr->ni_msg.type = ni_msg->type;
    msg_ptr->ni_msg.len = ni_msg->len;

    msg_ptr->ni_msg.ni_data = (uint8*)os_MemAlloc(ni_msg->len, OS_MEM_SCOPE_TASK);
    if ( msg_ptr->ni_msg.ni_data == NULL)
    {
      MSG_ERROR("Failed to allocate %d bytes buffer for NI msg",ni_msg->len+1,0,0);
      b_retval = FALSE;
    }
    else
    {
      memscpy((void *)msg_ptr->ni_msg.ni_data, ni_msg->len, (void *)ni_msg->ni_data, ni_msg->len);

      b_retval = pdsm_cmd_queue(&z_cmd_data);
  
      if (b_retval == FALSE) 
      {    
        os_MemFree((void **)&(msg_ptr->ni_msg.ni_data)); // free the buffer

        MSG_ERROR("Failed to queue pdsm command INJECT_NI_MSG",0,0,0);
        if ( pd_cb_func != NULL )
        {
          pd_cb_func( client_data_ptr, PDSM_PD_CMD_INJECT_NI_MSG, PDSM_PD_CMD_ERR_NO_BUF_L );
        } /* End if no callback is provided. */
      }
    }
  }

  return b_retval;

#else /* FEATURE_CGPS_PDCOMM  */
  return FALSE;   /* can't send message to PDCOMM */
#endif /* FEATURE_CGPS_PDCOMM  */
}


/*===========================================================================

FUNCTION pdsm_pd_inject_wifi_info_on_demand()

DESCRIPTION
  PDSM client calls this function to inject on-demand WiFi scan results to SM
 
  The supplied callback function pd_cb_func will be called to report this
  command's success/failure status. There will be no PDAPI event for this command.
 
DEPENDENCIES

RETURN VALUE
  TRUE - request is a success
  FALSE - request failed
  
SIDE EFFECTS

===========================================================================*/
extern boolean pdsm_pd_inject_wifi_info_on_demand (
   pdsm_pd_cmd_cb_f_type    pd_cb_func,        /* Command callback */
   /*~ PARAM pd_cb_func POINTER */
  void                     *client_data_ptr,   /* user data pointer */
  pdsm_client_id_type      client_id,          /* Client id of user */
  pdsm_wifi_on_demand_info_list_s_type  *wifi_info_on_demand_data_list_ptr /* Pointer to wifi on-demand scan result */  
  /*~ PARAM ni_info POINTER */
)
{

  boolean b_retval = TRUE;
  pdsm_cmd_s_type        z_cmd_data;
  struct pdsm_client_s  *client_ptr = NULL;

  memset(&z_cmd_data, 0, sizeof(z_cmd_data));
  client_ptr = pdsmclient_get_client_ptr( client_id );

  if( (NULL == client_ptr) || (NULL == wifi_info_on_demand_data_list_ptr) || 
      ( wifi_info_on_demand_data_list_ptr->q_num_wifi_aps > QMI_LOC_WIFI_MAX_REPORTED_APS_PER_MSG_V01 ) ||
      ( wifi_info_on_demand_data_list_ptr->q_num_wifi_aps == 0 ) 
    )
  {
    b_retval = FALSE;
  }
  else
  {
    PDSM_MSG_MED("--------- GPS Command --------------------",0,0,0);
    PDSM_MSG_MED("pdsm_pd_inject_wifi_info_on_demand(): %u",wifi_info_on_demand_data_list_ptr->q_num_wifi_aps,0,0);
    PDSM_MSG_MED("------------------------------------------",0,0,0);

    /* pack data for pdsm_cmd_queue */
    z_cmd_data.cmd_type = PDSM_CMD_TYPE_WIFI_ON_DEMAND;
    z_cmd_data.cmd.wifi_on_demand_info_list_cmd.client_id = client_id;
    z_cmd_data.cmd.wifi_on_demand_info_list_cmd.client_data_ptr = client_data_ptr;
    z_cmd_data.cmd.wifi_on_demand_info_list_cmd.cmd_cb_func = pd_cb_func;

    z_cmd_data.cmd.wifi_on_demand_info_list_cmd.wifi_on_demand_info_list.q_num_wifi_aps = 
      wifi_info_on_demand_data_list_ptr->q_num_wifi_aps;

    z_cmd_data.cmd.wifi_on_demand_info_list_cmd.wifi_on_demand_info_list.first_wifi_ap_ptr = 
      (locEngWifiApInfoStructT_v01*)os_MemAlloc( (wifi_info_on_demand_data_list_ptr->q_num_wifi_aps * sizeof(locEngWifiApInfoStructT_v01) ), OS_MEM_SCOPE_TASK);

    if ( z_cmd_data.cmd.wifi_on_demand_info_list_cmd.wifi_on_demand_info_list.first_wifi_ap_ptr == NULL)
    {
      MSG_ERROR("Failed to allocate buffer for wifi-on-demand-scan", 0,0,0);
      b_retval = FALSE;
    }
    else
    {
      MSG_MED("Allocated mem. at %lx", z_cmd_data.cmd.wifi_on_demand_info_list_cmd.wifi_on_demand_info_list.first_wifi_ap_ptr, 0, 0); 
      memscpy( (void *)z_cmd_data.cmd.wifi_on_demand_info_list_cmd.wifi_on_demand_info_list.first_wifi_ap_ptr, 
               ( wifi_info_on_demand_data_list_ptr->q_num_wifi_aps * sizeof(locEngWifiApInfoStructT_v01) ),
               (void *)wifi_info_on_demand_data_list_ptr->first_wifi_ap_ptr,
               ( wifi_info_on_demand_data_list_ptr->q_num_wifi_aps * sizeof(locEngWifiApInfoStructT_v01) ) );

      b_retval = pdsm_cmd_queue(&z_cmd_data);
  
      if (b_retval == FALSE) 
      {    
        os_MemFree((void **)&(z_cmd_data.cmd.wifi_on_demand_info_list_cmd.wifi_on_demand_info_list.first_wifi_ap_ptr)); // free the buffer
        MSG_ERROR("Failed to queue pdsm command PDSM_CMD_TYPE_WIFI_ON_DEMAND",0,0,0);
      }
    }

  }


  if (b_retval == FALSE) 
  {    
    if ( pd_cb_func != NULL )
    {
      pd_cb_func( client_data_ptr, PDSM_PD_CMD_INJECT_WIFI_ON_DEMAND, PDSM_PD_CMD_ERR_NO_BUF_L );
      MSG_HIGH("Failed to process wifi injection", 0, 0, 0);
    } /* End if no callback is provided. */
  }
  return b_retval;
}


/*~ FUNCTION pdsm_pd_inject_wifi_info_on_demand */
/*===========================================================================

FUNCTION pdsm_wifi_attachment_status_ind_report

DESCRIPTION
    This function is used to inject Wifi attachment status indication like:
    . Connected to Access point
    . Disconnected to Access point
    . Handover from one acces point to another
 
DEPENDENCIES 
   * Client must have already been initialized with pdsm_client_init().
 
PARAMETERS
 *wifi_attachment_status_ind_info: Pointer to struct containing wifi attachment status indicaiton info 
RETURN VALUE
  Void
SIDE EFFECTS  
===========================================================================*/
void pdsm_wifi_attachment_status_ind_report(
  pdsm_wifi_attachment_status_ind_info *wifi_attachment_status_ind_info
)
{
    pdsm_cmd_s_type z_cmd_data;  
  
    PDSM_MSG_MED("--------- GPS Command --------------------",0,0,0);
    PDSM_MSG_HIGH("pdsm_wifi_attachment_status_ind_report()",0,0,0);
    PDSM_MSG_MED("------------------------------------------",0,0,0);
  
    /*  pack data for pdsm_cmd_queue */
    z_cmd_data.cmd_type = PDSM_CMD_TYPE_WIFI_ATTACHMENT_STATUS_IND;
    z_cmd_data.cmd.wifi_attachment_status_ind_info = *wifi_attachment_status_ind_info;
    
    (void)pdsm_cmd_queue(&z_cmd_data);
}

/*===========================================================================

FUNCTION pdsm_gm_in_mem_logging_start

DESCRIPTION
  This function is for start GM in memory logging.

DEPENDENCIES

RETURN VALUE
  TRUE/FAIL
SIDE EFFECTS
  None.

===========================================================================*/
boolean pdsm_gm_in_mem_logging_start(uint32 q_max_log_size, uint32 q_period_s)
{
  sm_gm_in_mem_log_config_type z_log_config = {0};

  PDSM_MSG_MED("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_gm_in_mem_logging_start() log size %d period %d",
                q_max_log_size,
                q_period_s,
                0);
  PDSM_MSG_MED("------------------------------------------",0,0,0);
  if(q_max_log_size > PDSM_GM_IN_MEM_MAX_LOG_SIZE_B)
  {
    q_max_log_size = PDSM_GM_IN_MEM_MAX_LOG_SIZE_B;
    MSG_MED("Log size is greater than the max supported. Setting to %d bytes", 
            q_max_log_size, 0, 0);
  }
  if(q_period_s < PDSM_GM_IN_MEM_MIN_PERIOD_S)
  {
    q_period_s = PDSM_GM_IN_MEM_MIN_PERIOD_S;
    MSG_MED("Log period is lesser than the min supported. Setting to %d s ", 
            q_period_s, 0, 0);
  }

  z_log_config.q_max_log_size = q_max_log_size;
  z_log_config.q_period_s = q_period_s;

  return gm_in_mem_logging_start(&z_log_config);
}

/*===========================================================================

FUNCTION pdsm_gm_in_mem_logging_stop

DESCRIPTION
  This function is for stopping GM in memory logging.

DEPENDENCIES

RETURN VALUE
  TRUE/FAIL
SIDE EFFECTS
  None.

===========================================================================*/
boolean pdsm_gm_in_mem_logging_stop(void)
{
  PDSM_MSG_MED("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_gm_in_mem_logging_stop()",0,0,0);
  PDSM_MSG_MED("------------------------------------------",0,0,0);
  return gm_in_mem_logging_stop();
}
/*===========================================================================

FUNCTION pdsm_wifi_enabled_status_report

DESCRIPTION
    This function is used to inject Wifi enabled status information.
 
DEPENDENCIES 
   * Client must have already been initialized with pdsm_client_init().

PARAMETERS
 *wifi_enabled_status: Indicates whether the wifi is enabled or disabled on the device.
RETURN VALUE
  Void
SIDE EFFECTS  
===========================================================================*/
void pdsm_wifi_enabled_status_report(
  pdsm_wifi_enabled_status_ind_type e_wifi_enabled_status
)
{
  pdsm_cmd_s_type z_cmd_data;  

  memset(&z_cmd_data, 0, sizeof(z_cmd_data));
  PDSM_MSG_MED("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_wifi_enabled_status_report() enabled ? :: %d",
                e_wifi_enabled_status,
                0,
                0);
  PDSM_MSG_MED("------------------------------------------",0,0,0);
  
  /*  pack data for pdsm_cmd_queue */
  z_cmd_data.cmd_type = PDSM_CMD_TYPE_WIFI_ENABLED_STATUS;
  z_cmd_data.cmd.wifi_enabled_status = e_wifi_enabled_status;
    
  (void)pdsm_cmd_queue(&z_cmd_data);
}

/*===========================================================================
FUNCTION pdsm_get_gps_lock_status()

DESCRIPTION
  PDSM client calls this function to get current GPS lock status
 
  This is a synchronous API, will return immediately
 
DEPENDENCIES

RETURN VALUE
  Current GPS lock status
  
SIDE EFFECTS

===========================================================================*/
pdsm_gps_lock_e_type pdsm_get_gps_lock_status(void)
{
  return tm_api_get_gps_lock_status();
}



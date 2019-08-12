/**
@file ds_profile_remote_client.c
@brief
This file defines function, variables and data structures for QMI WDS client

@copyright (c)2012 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsprofile/src/ds_profile_remote_profile_op.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/25/13    gk     Fix to prevent out of bound array access when a client 
                   tries to modify a 3GPP2 profile.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "ds_Utils_DebugMsg.h"

#include "dcc_task_defs.h"
#include "dcc_task.h"
#include "dcc_task_svc.h"
#include "ds_profile_remote_client.h"
#include "ds_profile.h"
#include "ds_profile_os.h"
#include "qmi_client.h"
#include "wireless_data_service_v01.h"
#include <stringl/stringl.h>

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
  #include "ds_profile_3gpp.h"
#endif /* defined (FEATURE_WCDMA) || defined (FEATURE_GSM) */

#ifdef FEATURE_8960_SGLTE_FUSION

#define DS_PROFILE_REMOTE_MAX_PARAM_VAL_LEN 150

static boolean ds_profile_remote_profile_3gpp_param_set[DS_PROFILE_3GPP_PROFILE_PARAM_MAX];

/*===========================================================================

                         LOCAL DATA DECLARATIONS

===========================================================================*/

/**
	@brief Subset of 3gpp profile parameters. This subset defines all the 
         parameters that are applicable to remote modem. Local modem will
         send out updates to only below parameters and would filter out
         rest and then send it to remote modem
*/
static const int ds_profile_remote_3gpp_profile_params[] = 
{
  DS_PROFILE_3GPP_PROFILE_PARAM_PROFILE_NAME, 
  DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE,
  DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_H_COMP,
  DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_D_COMP,
  DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN,
  DS_PROFILE_3GPP_PROFILE_PARAM_DNS_ADDR_V4_PRIMARY,
  DS_PROFILE_3GPP_PROFILE_PARAM_DNS_ADDR_V4_SECONDARY,
  DS_PROFILE_3GPP_PROFILE_PARAM_GPRS_REQ_QOS,
  DS_PROFILE_3GPP_PROFILE_PARAM_GPRS_MIN_QOS,
  DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_USERNAME,
  DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_PASSWORD,
  DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_TYPE,
  DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_ADDR_V4,
  DS_PROFILE_3GPP_PROFILE_PARAM_PCSCF_REQ_FLAG,
  DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_TE_MT_ACCESS_CTRL_FLAG,
  DS_PROFILE_3GPP_PROFILE_PARAM_PCSCF_DHCP_REQ_FLAG,
  DS_PROFILE_3GPP_PROFILE_PARAM_IM_CN_FLAG,
  DS_PROFILE_3GPP_PROFILE_PARAM_TFT_FILTER_ID1,
  DS_PROFILE_3GPP_PROFILE_PARAM_TFT_FILTER_ID2,
  DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_NUMBER,
  DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_SECONDARY_FLAG,
  DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PRIMARY_ID,
  DS_PROFILE_3GPP_PROFILE_PARAM_IPV4_ADDR_ALLOC
};

/*===========================================================================

                         EXTERN DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
   QMI Client handler
---------------------------------------------------------------------------*/
extern qmi_client_type ds_profile_remote_client_handle;

/*===========================================================================

                        STATIC FUNCTION DEFINITION

===========================================================================*/
/*===========================================================================
FUNCTION       DS_PROFILE_REMOTE_CLIENT_OP_CMD_HANDLER

DESCRIPTION    Handler registered with dcc task. It calls function to
               to peform operation on remote modem like create profile, 
               delete profile etc

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
static void ds_profile_remote_perform_operaton_cmd_handler
(
  dcc_cmd_enum_type    cmd,
  void *               userDataPtr
)
{
  ds_profile_remote_perform_operation_info_type * cmd_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_1("ds_profile_remote_perform_operaton_cmd_handler(): cmd %d ",
                  cmd );
  
  cmd_info = (ds_profile_remote_perform_operation_info_type *)userDataPtr;
  
  if (DCC_DS_PROFILE_REMOTE_PERFORM_OPERATION_CMD != cmd)
  {
    LOG_MSG_INVALID_INPUT_1("ds_profile_remote_perform_operaton_cmd_handler():"
                            " Invalid args cmd %d", cmd);
    goto bail;
  }
  
  if(cmd_info == NULL)
  {
    LOG_MSG_ERROR_0("ds_profile_remote_perform_operaton_cmd_handler():"
                    " command info is null" ); 
    goto bail;
  }

  LOG_MSG_INFO2_3("ds_profile_remote_perform_operaton_cmd_handler(): "
                  " operation %d , profile index %d, tech type %d",
                  cmd_info->operation_type,
                  cmd_info->profile_index,
                  cmd_info->tech_type );
                 
  switch(cmd_info->operation_type)
  {
    case DS_PROFILE_REMOTE_OPERATION_PROFILE_CREATE :
    {
      ds_profile_remote_create_profile_cmd_hdlr(cmd_info->tech_type);
      break;
    }

    case DS_PROFILE_REMOTE_OPERATION_PROFILE_MODIFY :
    {
      ds_profile_remote_modify_profile_cmd_hdlr(cmd_info->profile_index);
      break;
    }
 
    case DS_PROFILE_REMOTE_OPERATION_PROFILE_DELETE :
    {
      ds_profile_remote_delete_profile_cmd_hdlr(cmd_info->tech_type,
                                                cmd_info->profile_index);
      break;
    }
    
    case DS_PROFILE_REMOTE_OPERATION_PROFILE_RESET :
    {
      ds_profile_remote_reset_profile_to_default_cmd_hdlr(cmd_info->profile_index);
      break;
    }   

    default :  
    {    
      LOG_MSG_ERROR_1("ds_profile_remote_perform_operaton_cmd_handler():"
                      " Invalid operation %d", cmd_info->operation_type );
      break;
    }
  }

bail:  
  DS_PROFILE_MEM_FREE((void *)cmd_info,MODEM_MEM_CLIENT_DATA);
  
  return;

} /* ds_profile_remote_perform_operaton_cmd_handler() */

/*===========================================================================

                        EXTERNAL FUNCTION DEFINITION

===========================================================================*/

/*===========================================================================
FUNCTION DS_PROFILE_REMOTE_PROFILE_INDENTIFIER_SET

DESCRIPTION
  This API lets the local modem know that profile identifer Is set and it has
  to send it to remote modem.

PARAMETERS
  profile_identifier :  profile identifier
  is_set             :  Profile identifier is set or unset
  
DEPENDENCIES 
  
RETURN VALUE 

SIDE EFFECTS 
 
===========================================================================*/
void ds_profile_remote_profile_identifier_set
(
  int      profile_identifier, 
  boolean  is_set
)
{
  LOG_MSG_INFO2_1("ds_profile_remote_profile_identifier_set %d",
                  profile_identifier);
  if(profile_identifier >= DS_PROFILE_3GPP_PROFILE_PARAM_MAX)
  {
    LOG_MSG_ERROR_1("ds_profile_remote_profile_identifier_set:" 
      "invalid profile_identifier.profile_identifier = %d",profile_identifier);
    return;
  }
  ds_profile_remote_profile_3gpp_param_set[profile_identifier] = is_set;
}


/*===========================================================================
FUNCTION DS_PROFILE_REMOTE_IS_SET_PROFILE_INDENTIFIER

DESCRIPTION
  This API lets local modem know that profile identifier has not been set 
  and hence it neednt send it to remote modem

PARAMETERS
  profile_identifier :  profile identifier
  
DEPENDENCIES 
  
RETURN VALUE 
  TRUE   :   Profile identifier is set.
  FALSE  :   Profile identifier is not set.
  
SIDE EFFECTS 
 
===========================================================================*/
boolean ds_profile_remote_is_set_profile_identifier
(
  int profile_identifier
)
{
  return ds_profile_remote_profile_3gpp_param_set[profile_identifier];
}


/*===========================================================================
FUNCTION DS_PROFILE_REMOTE_CREATE_PROFILE_CMD_HDLR

DESCRIPTION
  This API sends request to remote modem to create the profile for given 
  tech type.

PARAMETERS
  tech :  Technology type
  
DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
 
===========================================================================*/
void ds_profile_remote_create_profile_cmd_hdlr
(
  ds_profile_tech_etype  tech
)
{
  wds_create_profile_req_msg_v01  * wds_create_profile_req_msg = NULL;
  wds_create_profile_resp_msg_v01 * wds_create_profile_resp_msg = NULL;
  int16                             errval = DS_PROFILE_REG_RESULT_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO1_1("ds_profile_remote_create_profile_cmd_hdlr(): tech %d",
                  tech);
  
  if( !ds_profile_remote_client_handle )
  {
     LOG_MSG_ERROR_0("ds_profile_remote_create_profile_cmd_hdlr(): QMI WDS client"
                     " handle null");
     return;
  }

  /*----------------------------------------------------------------------- 
    Allocate dynamic memory for response message. Memory will be freed
    during response message callback handler
  ------------------------------------------------------------------------*/
  wds_create_profile_req_msg 
    = (wds_create_profile_req_msg_v01 *)
        DS_PROFILE_MEM_ALLOC(sizeof(wds_create_profile_req_msg_v01),
                             MODEM_MEM_CLIENT_DATA_CRIT);

  if(wds_create_profile_req_msg == NULL)
  {
    LOG_MSG_ERROR_0("ds_profile_remote_create_profile_cmd_hdlr() : "
                    "Couldn't get data_block_ptr memory." );
    errval = DS_PROFILE_REG_RESULT_FAIL;
    goto bail;
  }

  LOG_MSG_INFO2_1("ds_profile_remote_create_profile_cmd_hdlr() : Allocated mem for set "
                  "event report resp message 0x%x",
                  wds_create_profile_req_msg);

  memset(wds_create_profile_req_msg,
         0,
         sizeof(wds_create_profile_req_msg_v01));
         
  wds_create_profile_req_msg->profile_type = tech;

  /*----------------------------------------------------------------------- 
    Allocate dynamic memory for response message. Memory will be freed
    during response message callback handler
  ------------------------------------------------------------------------*/
  wds_create_profile_resp_msg 
    = (wds_create_profile_resp_msg_v01*)
        DS_PROFILE_MEM_ALLOC(sizeof(wds_create_profile_resp_msg_v01),
                             MODEM_MEM_CLIENT_DATA_CRIT);

  if(wds_create_profile_resp_msg == NULL)
  {
    LOG_MSG_ERROR_0("ds_profile_remote_create_profile_cmd_hdlr(): Couldn't get "
                    "data_block_ptr memory." ); 
    errval = DS_PROFILE_REG_RESULT_FAIL;
    goto bail;
  }

  LOG_MSG_INFO2_1("ds_profile_remote_create_profile_cmd_hdlr(): Allocated mem "
                  "for set event report resp message 0x%x",
                  wds_create_profile_resp_msg);

  memset(wds_create_profile_resp_msg,
         0,
         sizeof(wds_create_profile_resp_msg_v01));
  
  /*----------------------------------------------------------------------- 
    Send request QMI_WDS_SET_EVENT_REPORT_REQ_MSG to QMI-WDS service
  ------------------------------------------------------------------------*/
  if(QMI_NO_ERR != qmi_client_send_msg_sync
                   (
                     ds_profile_remote_client_handle,
                     QMI_WDS_CREATE_PROFILE_REQ_V01,
                     wds_create_profile_req_msg,
                     sizeof(wds_create_profile_req_msg_v01),
                     wds_create_profile_resp_msg,
                     sizeof(wds_create_profile_resp_msg_v01),
                     500
                   ))
  {
    LOG_MSG_ERROR_0("ds_profile_remote_create_profile_cmd_hdlr(): Send msg async "
                    "failed.");
    errval = DS_PROFILE_REG_RESULT_FAIL;
    goto bail;
  }
  
  if(wds_create_profile_resp_msg->resp.error != QMI_ERR_NONE_V01)
  {
    LOG_MSG_ERROR_1("ds_profile_remote_create_profile_cmd_hdlr() "
                    "Response error=%d.",
                    wds_create_profile_resp_msg->resp.error);
    errval = DS_PROFILE_REG_RESULT_FAIL;
    goto bail;
  }
  
  LOG_MSG_INFO2_1("ds_profile_remote_create_profile_cmd_hdlr(): Profile on Remote modem "
                  "created %d",
                  wds_create_profile_resp_msg->profile.profile_index);
  
bail:
  /*-------------------------------------------------------------------------
     Release the memory 
  -------------------------------------------------------------------------*/ 
  LOG_MSG_INFO2_0("ds_profile_remote_create_profile_cmd_hdlr() "
                  "Freeing mem allocated "); 

  if(wds_create_profile_req_msg != NULL)
  {  
    DS_PROFILE_MEM_FREE((void *)wds_create_profile_req_msg,MODEM_MEM_CLIENT_DATA);       
  }    
  if(wds_create_profile_resp_msg != NULL)
  {
    DS_PROFILE_MEM_FREE((void *)wds_create_profile_resp_msg,MODEM_MEM_CLIENT_DATA);
  }
  
  if(errval != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    LOG_MSG_ERROR_0("ds_profile_remote_create_profile_cmd_hdlr() "
                    "Remote operation for create profile failed "); 
  }
  
  return;
} /* ds_profile_remote_create_profile_cmd_hdlr() */


/*===========================================================================
FUNCTION DS_PROFILE_REMOTE_MODIFY_PROFILE_CMD_HDLR

DESCRIPTION
  This API sends request to remote modem to modify the profile information 
  for given profile index 

PARAMETERS
  profile_index :  Profile number
  
DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
 
===========================================================================*/
void ds_profile_remote_modify_profile_cmd_hdlr
(
  ds_profile_num_type    profile_index
)
{
  wds_modify_profile_settings_req_msg_v01  * 
    wds_modify_profile_req_msg = NULL;
  wds_modify_profile_settings_resp_msg_v01 * 
    wds_modify_profile_resp_msg = NULL;
  ds_profile_status_etype           profile_status;
  uint8                             index;
  uint8                             array_size;
  ds_profile_hndl_type              profile_hndl;
  ds_profile_identifier_type        profile_identifier;
  ds_profile_info_type              param_info;
  int16                             errval = DS_PROFILE_REG_RESULT_SUCCESS;
  unsigned char                     value[DS_PROFILE_REMOTE_MAX_PARAM_VAL_LEN];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO1_1("ds_profile_remote_modify_profile_cmd_hdlr() profile index %d ",
                  profile_index);
  
  if( !ds_profile_remote_client_handle )
  {
     LOG_MSG_ERROR_0("ds_profile_remote_modify_profile_cmd_hdlr(): QMI WDS client"
                     " handle null");
     return;
  }
  
  if(profile_index < 0)
  {
    LOG_MSG_ERROR_0("ds_profile_remote_modify_profile_cmd_hdlr(): Incorrect profile "
                    "number");
    return;                  
  }

  /*----------------------------------------------------------------------- 
    Allocate dynamic memory for response message. Memory will be freed
    during response message callback handler
  ------------------------------------------------------------------------*/
  wds_modify_profile_req_msg 
    = (wds_modify_profile_settings_req_msg_v01 *)
        DS_PROFILE_MEM_ALLOC(sizeof(wds_modify_profile_settings_req_msg_v01),
                             MODEM_MEM_CLIENT_DATA_CRIT);

  if(wds_modify_profile_req_msg == NULL)
  {
    LOG_MSG_ERROR_0("ds_profile_remote_modify_profile_cmd_hdlr(): Couldn't get "
                    "data_block_ptr memory." );
    errval = DS_PROFILE_REG_RESULT_FAIL;
    goto bail;
  }

  LOG_MSG_INFO2_1("ds_profile_remote_modify_profile_cmd_hdlr() Allocated mem for set "
                  "event report resp message 0x%x",
                  wds_modify_profile_req_msg);

  memset(wds_modify_profile_req_msg,
         0,
         sizeof(wds_modify_profile_settings_req_msg_v01));
         
  wds_modify_profile_req_msg->profile.profile_index = profile_index;
  wds_modify_profile_req_msg->profile.profile_type = 
    WDS_PROFILE_TYPE_3GPP_V01;
    
 
  LOG_MSG_INFO2_1("ds_profile_remote_modify_profile_cmd_hdlr() Profile index %d",
                  wds_modify_profile_req_msg->profile.profile_index);
         
  memset(&profile_hndl, 0, sizeof(profile_hndl));
  memset(&param_info, 0, sizeof(param_info));
  
  array_size = sizeof(ds_profile_remote_3gpp_profile_params) / 
                 sizeof(ds_profile_remote_3gpp_profile_params[0]);
                
  /*-------------------------------------------------------------------------
    Begin transaction
  -------------------------------------------------------------------------*/
  profile_status = 
    ds_profile_begin_transaction(DS_PROFILE_TRN_READ, 
                                 DS_PROFILE_TECH_3GPP,
                                 (ds_profile_num_type)profile_index,
                                 &profile_hndl);

  if (profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    LOG_MSG_ERROR_1 ("ds_profile_remote_modify_profile_cmd_hdlr() : Begin Transaction "
                     "failed. Error %d", profile_status);
    errval = DS_PROFILE_REG_RESULT_FAIL;
    goto bail;
  }     

  index = 0;

  while (index < array_size)
  {
    profile_identifier = ds_profile_remote_3gpp_profile_params[index];
    
    memset(value, 0, sizeof(value));
    param_info.len = sizeof(value);
    param_info.buf = (void*)value;
    
    /*-----------------------------------------------------------------------
      Retrieve profile_identifier and param info
    -----------------------------------------------------------------------*/
    profile_status = ds_profile_get_param(profile_hndl, 
                                          profile_identifier, 
                                          &param_info);

    /*-----------------------------------------------------------------------
      Do not send the profile parameter to remote modem when get operation 
      fails on local modem
    -----------------------------------------------------------------------*/                                          
    if(profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
    {
      LOG_MSG_ERROR_2 ("ds_profile_remote_modify_profile_cmd_hdlr() : Get operation "
                       "failed. Profile Ident %d Error %d",
                       profile_identifier, profile_status);
      index++;
      continue;
    }
    
    switch (profile_identifier)
    {
      case DS_PROFILE_3GPP_PROFILE_PARAM_PROFILE_NAME:
        if(ds_profile_remote_is_set_profile_identifier
          (DS_PROFILE_3GPP_PROFILE_PARAM_PROFILE_NAME
          ))
        {
          wds_modify_profile_req_msg->profile_name_valid = TRUE;
          memscpy( &wds_modify_profile_req_msg->profile_name,
            param_info.len, param_info.buf, param_info.len );
                           
          ds_profile_remote_profile_identifier_set
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_PROFILE_NAME,
            FALSE
          );
        }
        break;
        
      case DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE:
        if(ds_profile_remote_is_set_profile_identifier
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE
          ))
        {
          wds_modify_profile_req_msg->pdp_type_valid = TRUE;
          /* For SGLTE, the requirement is that profile on QSC should always
             be ipv4. So no matter what the profile on 8960 is, profile 
             created on QSC would be ipv4 type profile only
          */
          wds_modify_profile_req_msg->pdp_type = WDS_PDP_TYPE_PDP_IPV4_V01;
                  
          ds_profile_remote_profile_identifier_set
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE,
            FALSE
          );
        }        
        break;

      case DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_H_COMP:
        if(ds_profile_remote_is_set_profile_identifier
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_H_COMP
          ))
        {      
          wds_modify_profile_req_msg->pdp_hdr_compression_type_valid 
            = TRUE;
          memscpy( &wds_modify_profile_req_msg->pdp_hdr_compression_type,
            param_info.len, param_info.buf, param_info.len );

          ds_profile_remote_profile_identifier_set
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_H_COMP,
            FALSE
          );
        }                   
        break;

      case DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_D_COMP :
        if(ds_profile_remote_is_set_profile_identifier
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_D_COMP
          ))
        {       
          wds_modify_profile_req_msg->pdp_data_compression_type_valid 
            = TRUE;
          memscpy( &wds_modify_profile_req_msg->pdp_data_compression_type,
            param_info.len, param_info.buf, param_info.len );  

          ds_profile_remote_profile_identifier_set
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_D_COMP,
            FALSE
          );
        }                  
        break;

      case DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN :
        if(ds_profile_remote_is_set_profile_identifier
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN
          ))
        {       
          wds_modify_profile_req_msg->apn_name_valid = TRUE;
          memscpy( &wds_modify_profile_req_msg->apn_name,
            param_info.len, param_info.buf, param_info.len );     

          ds_profile_remote_profile_identifier_set
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN,
            FALSE
          );
        }                     
        break;

      case DS_PROFILE_3GPP_PROFILE_PARAM_DNS_ADDR_V4_PRIMARY :
        if(ds_profile_remote_is_set_profile_identifier
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_DNS_ADDR_V4_PRIMARY
          ))
        {      
          wds_modify_profile_req_msg->primary_DNS_IPv4_address_preference_valid 
            = TRUE;
          memscpy ( 
            &wds_modify_profile_req_msg->primary_DNS_IPv4_address_preference,
            param_info.len, param_info.buf, param_info.len ); 

          ds_profile_remote_profile_identifier_set
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_DNS_ADDR_V4_PRIMARY,
            FALSE
          );
        }                      
        break;

      case DS_PROFILE_3GPP_PROFILE_PARAM_DNS_ADDR_V4_SECONDARY :
        if(ds_profile_remote_is_set_profile_identifier
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_DNS_ADDR_V4_SECONDARY
          ))
        {      
          wds_modify_profile_req_msg->secondary_DNS_IPv4_address_preference_valid 
            = TRUE;
          memscpy ( 
            &wds_modify_profile_req_msg->secondary_DNS_IPv4_address_preference,
            param_info.len, param_info.buf, param_info.len );  

          ds_profile_remote_profile_identifier_set
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_DNS_ADDR_V4_SECONDARY,
            FALSE
          );
        }         
        break;

      case DS_PROFILE_3GPP_PROFILE_PARAM_GPRS_REQ_QOS :
        if(ds_profile_remote_is_set_profile_identifier
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_GPRS_REQ_QOS
          ))
        {       
          wds_modify_profile_req_msg->gprs_requested_qos_valid = TRUE;
          memscpy( &wds_modify_profile_req_msg->gprs_requested_qos,
            param_info.len, param_info.buf, param_info.len );  

          ds_profile_remote_profile_identifier_set
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_GPRS_REQ_QOS,
            FALSE
          );
        }                
        break;

      case DS_PROFILE_3GPP_PROFILE_PARAM_GPRS_MIN_QOS :
        if(ds_profile_remote_is_set_profile_identifier
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_GPRS_MIN_QOS
          ))
        {       
          wds_modify_profile_req_msg->gprs_minimum_qos_valid = TRUE;
          memscpy( &wds_modify_profile_req_msg->gprs_minimum_qos,
            param_info.len, param_info.buf, param_info.len );  

          ds_profile_remote_profile_identifier_set
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_GPRS_MIN_QOS,
            FALSE
          );
        }        
        break;    

      case DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_USERNAME :
        if(ds_profile_remote_is_set_profile_identifier
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_USERNAME
          ))
        {      
          wds_modify_profile_req_msg->username_valid = TRUE;
          memscpy( &wds_modify_profile_req_msg->username,
            param_info.len, param_info.buf, param_info.len );   

          ds_profile_remote_profile_identifier_set
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_USERNAME,
            FALSE
          );
        }                  
        break;

      case DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_PASSWORD :
        if(ds_profile_remote_is_set_profile_identifier
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_PASSWORD
          ))
        {      
          wds_modify_profile_req_msg->password_valid = TRUE;
          memscpy( &wds_modify_profile_req_msg->password,
            param_info.len, param_info.buf, param_info.len );      

          ds_profile_remote_profile_identifier_set
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_PASSWORD,
            FALSE
          );
        }                 
        break;

      case DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_TYPE :
        if(ds_profile_remote_is_set_profile_identifier
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_TYPE
          ))
        {      
          wds_modify_profile_req_msg->authentication_preference_valid = TRUE;
          memscpy( &wds_modify_profile_req_msg->authentication_preference,
            param_info.len, param_info.buf, param_info.len );        

          ds_profile_remote_profile_identifier_set
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_TYPE,
            FALSE
          );
        }                
        break;

      case DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_ADDR_V4 :
        if(ds_profile_remote_is_set_profile_identifier
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_ADDR_V4
          ))
        {      
          wds_modify_profile_req_msg->ipv4_address_preference_valid = TRUE;
          memscpy( &wds_modify_profile_req_msg->ipv4_address_preference,
            param_info.len, param_info.buf, param_info.len );     

          ds_profile_remote_profile_identifier_set
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_ADDR_V4,
            FALSE
          );
        }                  
        break;

      case DS_PROFILE_3GPP_PROFILE_PARAM_PCSCF_REQ_FLAG :
        if(ds_profile_remote_is_set_profile_identifier
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_PCSCF_REQ_FLAG
          ))
        {      
          wds_modify_profile_req_msg->pcscf_addr_using_pco_valid = TRUE;
          memscpy( &wds_modify_profile_req_msg->pcscf_addr_using_pco,
            param_info.len, param_info.buf, param_info.len ); 

          ds_profile_remote_profile_identifier_set
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_PCSCF_REQ_FLAG,
            FALSE
          );
        }                
        break;   

      case DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_TE_MT_ACCESS_CTRL_FLAG :
        if(ds_profile_remote_is_set_profile_identifier
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_TE_MT_ACCESS_CTRL_FLAG
          ))
        {      
          wds_modify_profile_req_msg->pdp_access_control_flag_valid = TRUE;
          memscpy( &wds_modify_profile_req_msg->pdp_access_control_flag,
            param_info.len, param_info.buf, param_info.len ); 

          ds_profile_remote_profile_identifier_set
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_TE_MT_ACCESS_CTRL_FLAG,
            FALSE
          );
        }                
        break;

      case DS_PROFILE_3GPP_PROFILE_PARAM_PCSCF_DHCP_REQ_FLAG :
        if(ds_profile_remote_is_set_profile_identifier
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_PCSCF_DHCP_REQ_FLAG
          ))
        {      
          wds_modify_profile_req_msg->pcscf_addr_using_dhcp_valid = TRUE;
          memscpy( &wds_modify_profile_req_msg->pcscf_addr_using_dhcp,
            param_info.len, param_info.buf, param_info.len );   

          ds_profile_remote_profile_identifier_set
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_PCSCF_DHCP_REQ_FLAG,
            FALSE
          );
        }                
        break;

      case DS_PROFILE_3GPP_PROFILE_PARAM_IM_CN_FLAG :
        if(ds_profile_remote_is_set_profile_identifier
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_IM_CN_FLAG
          ))
        {      
          wds_modify_profile_req_msg->im_cn_flag_valid = TRUE;
          memscpy( &wds_modify_profile_req_msg->im_cn_flag,
            param_info.len, param_info.buf, param_info.len );        

          ds_profile_remote_profile_identifier_set
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_IM_CN_FLAG,
            FALSE
          );
        }                
        break;

      case DS_PROFILE_3GPP_PROFILE_PARAM_TFT_FILTER_ID1 :
        if(ds_profile_remote_is_set_profile_identifier
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_TFT_FILTER_ID1
          ))
        {      
          wds_modify_profile_req_msg->tft_id1_params_valid = TRUE;
          memscpy( &wds_modify_profile_req_msg->tft_id1_params,
            param_info.len, param_info.buf, param_info.len );    

          ds_profile_remote_profile_identifier_set
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_TFT_FILTER_ID1,
            FALSE
          );
        }                
        break;

      case DS_PROFILE_3GPP_PROFILE_PARAM_TFT_FILTER_ID2 :
        if(ds_profile_remote_is_set_profile_identifier
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_TFT_FILTER_ID2
          ))
        {      
          wds_modify_profile_req_msg->tft_id2_params_valid = TRUE;
          memscpy( &wds_modify_profile_req_msg->tft_id2_params,
            param_info.len, param_info.buf, param_info.len );   

          ds_profile_remote_profile_identifier_set
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_TFT_FILTER_ID2,
            FALSE
          );
        }                
        break;

      case DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_NUMBER :
        if(ds_profile_remote_is_set_profile_identifier
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_NUMBER
          ))
        {      
          wds_modify_profile_req_msg->pdp_context_valid = TRUE;
          memscpy( &wds_modify_profile_req_msg->pdp_context,
            param_info.len, param_info.buf, param_info.len );   

          ds_profile_remote_profile_identifier_set
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_NUMBER,
            FALSE
          );
        }                
        break;

      case DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_SECONDARY_FLAG :
        if(ds_profile_remote_is_set_profile_identifier
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_SECONDARY_FLAG
          ))
        {      
          wds_modify_profile_req_msg->secondary_flag_valid = TRUE;
          memscpy( &wds_modify_profile_req_msg->secondary_flag, 
            param_info.len, param_info.buf, param_info.len );      

          ds_profile_remote_profile_identifier_set
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_SECONDARY_FLAG,
            FALSE
          );
        }                
        break;    

      case DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PRIMARY_ID :
        if(ds_profile_remote_is_set_profile_identifier
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PRIMARY_ID
          ))
        {      
          wds_modify_profile_req_msg->primary_id_valid = TRUE;
          memscpy( &wds_modify_profile_req_msg->primary_id, 
            param_info.len, param_info.buf, param_info.len );   

          ds_profile_remote_profile_identifier_set
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PRIMARY_ID,
            FALSE
          );
        }                
        break; 

      case DS_PROFILE_3GPP_PROFILE_PARAM_IPV4_ADDR_ALLOC :
        if(ds_profile_remote_is_set_profile_identifier
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_IPV4_ADDR_ALLOC
          ))
        {      
          wds_modify_profile_req_msg->addr_allocation_preference_valid = TRUE;
          memscpy( &wds_modify_profile_req_msg->addr_allocation_preference, 
            param_info.len, param_info.buf, param_info.len );     

          ds_profile_remote_profile_identifier_set
          (
            DS_PROFILE_3GPP_PROFILE_PARAM_IPV4_ADDR_ALLOC,
            FALSE
          );
        }                
        break; 
        
      default:      
        LOG_MSG_ERROR_1 ("ds_profile_remote_modify_profile_cmd_hdlr() : Invalid profile "
                         "param %d", profile_identifier);
        break;
    }
  
    index++;  
  }  
  
  /*-------------------------------------------------------------------------
    End transaction
  -------------------------------------------------------------------------*/
  profile_status = ds_profile_end_transaction(profile_hndl, 
                                              DS_PROFILE_ACTION_COMMIT);
  if(profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    LOG_MSG_ERROR_1 ("ds_profile_remote_modify_profile_cmd_hdlr() : "
                     "End Transaction failed. Error %d", profile_status);
    errval = DS_PROFILE_REG_RESULT_FAIL;
    goto bail;
  }
  /*----------------------------------------------------------------------- 
    Allocate dynamic memory for response message. Memory will be freed
    during response message callback handler
  ------------------------------------------------------------------------*/
  wds_modify_profile_resp_msg 
    = (wds_modify_profile_settings_resp_msg_v01*)
        DS_PROFILE_MEM_ALLOC(sizeof(wds_modify_profile_settings_resp_msg_v01),
                             MODEM_MEM_CLIENT_DATA_CRIT);

  if(wds_modify_profile_resp_msg == NULL)
  {
    LOG_MSG_ERROR_0("Couldn't get data_block_ptr memory. ");
    errval = DS_PROFILE_REG_RESULT_FAIL;
    goto bail;
  }

  LOG_MSG_INFO2_1("ds_profile_remote_modify_profile_cmd_hdlr(): Allocated mem for "
                  "set event report resp message 0x%x",
                  wds_modify_profile_resp_msg);

  memset(wds_modify_profile_resp_msg,
         0,
         sizeof(wds_modify_profile_settings_resp_msg_v01));
  
  /*----------------------------------------------------------------------- 
    Send request QMI_WDS_SET_EVENT_REPORT_REQ_MSG to QMI-WDS service
  ------------------------------------------------------------------------*/
  if(QMI_NO_ERR != qmi_client_send_msg_sync
                   (
                     ds_profile_remote_client_handle,
                     QMI_WDS_MODIFY_PROFILE_SETTINGS_REQ_V01,
                     wds_modify_profile_req_msg,
                     sizeof(wds_modify_profile_settings_req_msg_v01),
                     wds_modify_profile_resp_msg,
                     sizeof(wds_modify_profile_settings_resp_msg_v01),
                     500
                   ))
  {
    LOG_MSG_ERROR_0("ds_profile_remote_modify_profile_cmd_hdlr(): Send msg sync"
                    "failed.");
    errval = DS_PROFILE_REG_RESULT_FAIL;
    goto bail;
  }
  
  if(wds_modify_profile_resp_msg->resp.error != QMI_ERR_NONE_V01)
  {
    LOG_MSG_ERROR_1("ds_profile_remote_modify_profile_cmd_hdlr() "
                    "Response error=%d.",
                    wds_modify_profile_resp_msg->resp.error);
    errval = DS_PROFILE_REG_RESULT_FAIL;
    goto bail;
  }

bail:
  /*-------------------------------------------------------------------------
     Release the memory 
  -------------------------------------------------------------------------*/ 
  LOG_MSG_INFO2_0("ds_profile_remote_modify_profile_cmd_hdlr() "
                  "Freeing mem allocated "); 

  if(wds_modify_profile_req_msg != NULL)
  {  
    DS_PROFILE_MEM_FREE((void *)wds_modify_profile_req_msg,MODEM_MEM_CLIENT_DATA);       
  }    
  if(wds_modify_profile_resp_msg != NULL)
  {
    DS_PROFILE_MEM_FREE((void *)wds_modify_profile_resp_msg,MODEM_MEM_CLIENT_DATA);
  }
  
  if(errval != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    LOG_MSG_ERROR_0("ds_profile_remote_modify_profile_cmd_hdlr() "
                    "Remote operation for modify profile failed ");
  }
  
  return;  
} /* ds_profile_remote_modify_profile_cmd_hdlr() */


/*===========================================================================
FUNCTION DS_PROFILE_REMOTE_DELETE_PROFILE_CMD_HDLR

DESCRIPTION
  This API sends request to remote modem to delete the profile information
  for given profile index

PARAMETERS
  profile_type  :  Technology type like 3gpp or 3gpp profile
  profile_index :  Profile number
  
DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
 
===========================================================================*/
void ds_profile_remote_delete_profile_cmd_hdlr
(
  uint8           profile_type,
  uint8           profile_index
)
{
  wds_delete_profile_req_msg_v01  * wds_delete_profile_req_msg = NULL;
  wds_delete_profile_resp_msg_v01 * wds_delete_profile_resp_msg = NULL;
  int                               errval = DS_PROFILE_REG_RESULT_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO1_2("ds_profile_remote_delete_profile_cmd_hdlr() profile type %d "
                  "profile index %d", profile_type, profile_index);
  
  if( !ds_profile_remote_client_handle )
  {
     LOG_MSG_ERROR_0("ds_profile_remote_delete_profile_cmd_hdlr(): QMI WDS client"
                     " handle null");
     return;
  }

  /*----------------------------------------------------------------------- 
    Allocate dynamic memory for response message. Memory will be freed
    during response message callback handler
  ------------------------------------------------------------------------*/
  wds_delete_profile_req_msg 
    = (wds_delete_profile_req_msg_v01 *)
        DS_PROFILE_MEM_ALLOC(sizeof(wds_delete_profile_req_msg_v01),
                             MODEM_MEM_CLIENT_DATA_CRIT);

  if(wds_delete_profile_req_msg == NULL)
  {
    LOG_MSG_ERROR_1("ds_profile_remote_delete_profile_cmd_hdlr() : Couldn't get "
                    "data_block_ptr memory. Free mem 0x%x",
                    wds_delete_profile_req_msg );
    errval = DS_PROFILE_REG_RESULT_FAIL;
    goto bail;
  }

  LOG_MSG_INFO2_1("ds_profile_remote_delete_profile_cmd_hdlr() : Allocated mem"
                  "  for set event report resp message 0x%x",
                  wds_delete_profile_req_msg);

  memset(wds_delete_profile_req_msg,
         0,
         sizeof(wds_delete_profile_req_msg_v01));
 
  wds_delete_profile_req_msg->profile.profile_type = profile_type;
  wds_delete_profile_req_msg->profile.profile_index = profile_index;
  
  /*----------------------------------------------------------------------- 
    Allocate dynamic memory for response message. Memory will be freed
    during response message callback handler
  ------------------------------------------------------------------------*/
  wds_delete_profile_resp_msg 
    = (wds_delete_profile_resp_msg_v01*)
        DS_PROFILE_MEM_ALLOC(sizeof(wds_delete_profile_resp_msg_v01),
                             MODEM_MEM_CLIENT_DATA_CRIT);

  if(wds_delete_profile_resp_msg == NULL)
  {
    LOG_MSG_ERROR_1("ds_profile_remote_delete_profile_cmd_hdlr() Couldn't get "
                    "data_block_ptr memory. Free mem 0x%x",
                    wds_delete_profile_resp_msg );
    errval = DS_PROFILE_REG_RESULT_FAIL;
    goto bail;
  }

  LOG_MSG_INFO2_1("ds_profile_remote_delete_profile_cmd_hdlr() : Allocated mem "
                  "for set event report resp message 0x%x",
                  wds_delete_profile_resp_msg);

  memset(wds_delete_profile_resp_msg,
         0,
         sizeof(wds_delete_profile_resp_msg_v01));
  
  /*----------------------------------------------------------------------- 
    Send request to QMI-WDS service
  ------------------------------------------------------------------------*/
  if(QMI_NO_ERR != qmi_client_send_msg_sync
                   (
                     ds_profile_remote_client_handle,
                     QMI_WDS_DELETE_PROFILE_REQ_V01,
                     wds_delete_profile_req_msg,
                     sizeof(wds_delete_profile_req_msg_v01),
                     wds_delete_profile_resp_msg,
                     sizeof(wds_delete_profile_resp_msg_v01),
                     500
                   ))
  {
    LOG_MSG_ERROR_0("ds_profile_remote_delete_profile_cmd_hdlr() : Send msg async "
                    "failed. ");
    errval = DS_PROFILE_REG_RESULT_FAIL;
    goto bail;    
  }                                             

  if(wds_delete_profile_resp_msg->resp.error != QMI_ERR_NONE_V01)
  {
    LOG_MSG_ERROR_1("ds_profile_remote_delete_profile_cmd_hdlr() "
                    "Response error=%d.",
                    wds_delete_profile_resp_msg->resp.error);
    errval = DS_PROFILE_REG_RESULT_FAIL;
    goto bail;
  }
  
bail:
  /*-------------------------------------------------------------------------
     Release the memory 
  -------------------------------------------------------------------------*/ 
  LOG_MSG_INFO2_0("ds_profile_remote_delete_profile_cmd_hdlr() "
                  "Freeing mem allocated "); 

  if(wds_delete_profile_req_msg != NULL)
  {  
    DS_PROFILE_MEM_FREE((void *)wds_delete_profile_req_msg,MODEM_MEM_CLIENT_DATA);       
  }    
  if(wds_delete_profile_resp_msg != NULL)
  {
    DS_PROFILE_MEM_FREE((void *)wds_delete_profile_resp_msg,MODEM_MEM_CLIENT_DATA);
  }
  
  if(errval != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    LOG_MSG_ERROR_0("ds_profile_remote_delete_profile_cmd_hdlr() "
                    "Remote operation for delete profile failed ");
  }
  
  return;  
} /* ds_profile_remote_delete_profile_cmd_hdlr() */


/*===========================================================================
FUNCTION DS_PROFILE_REMOTE_RESET_PROFILE_TO_DEFAULT_CMD_HDLR

DESCRIPTION
  This API sends request to remote modem to reset the profile information 
  to default values

PARAMETERS
  profile_index :  Profile number
  
DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
 
===========================================================================*/
void ds_profile_remote_reset_profile_to_default_cmd_hdlr
(
  uint8  profile_index
)
{
  wds_reset_profile_to_default_req_msg_v01  * 
    wds_reset_profile_to_default_req_msg = NULL;
  wds_reset_profile_to_default_resp_msg_v01 * 
    wds_reset_profile_to_default_resp_msg = NULL;
  int16                             errval = DS_PROFILE_REG_RESULT_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO1_0("ds_profile_remote_reset_profile_to_default_cmd_hdlr(): ");
  
  if( !ds_profile_remote_client_handle )
  {
     LOG_MSG_ERROR_0("ds_profile_remote_reset_profile_to_default_cmd_hdlr(): "
                     "QMI WDS client handle null");
     return;
  }

  /*----------------------------------------------------------------------- 
    Allocate dynamic memory for response message. Memory will be freed
    during response message callback handler
  ------------------------------------------------------------------------*/
  wds_reset_profile_to_default_req_msg 
    = (wds_reset_profile_to_default_req_msg_v01 *)
        DS_PROFILE_MEM_ALLOC(sizeof(wds_reset_profile_to_default_req_msg_v01),
                             MODEM_MEM_CLIENT_DATA_CRIT);

  if(wds_reset_profile_to_default_req_msg == NULL)
  {
    LOG_MSG_ERROR_0("Couldn't get data_block_ptr memory." );
    errval = DS_PROFILE_REG_RESULT_FAIL;
    goto bail;
  }

  LOG_MSG_INFO2_1("Allocated mem for set event report resp message 0x%x",
                  wds_reset_profile_to_default_req_msg);

  memset(wds_reset_profile_to_default_req_msg,
         0,
         sizeof(wds_reset_profile_to_default_req_msg_v01));
         
  wds_reset_profile_to_default_req_msg->profile_identifier.profile_index = 
    profile_index;

  /*----------------------------------------------------------------------- 
    Allocate dynamic memory for response message. Memory will be freed
    during response message callback handler
  ------------------------------------------------------------------------*/
  wds_reset_profile_to_default_resp_msg 
    = (wds_reset_profile_to_default_resp_msg_v01*)
        DS_PROFILE_MEM_ALLOC(sizeof(wds_reset_profile_to_default_resp_msg_v01),
                             MODEM_MEM_CLIENT_DATA_CRIT);

  if(wds_reset_profile_to_default_resp_msg == NULL)
  {
    LOG_MSG_ERROR_0("ds_profile_remote_reset_profile_to_default_cmd_hdlr(): Couldn't get "
                    "data_block_ptr memory." ); 
    errval = DS_PROFILE_REG_RESULT_FAIL;
    goto bail;
  }

  LOG_MSG_INFO2_1("ds_profile_remote_reset_profile_to_default_cmd_hdlr(): Allocated mem "
                  "for set event report resp message 0x%x",
                  wds_reset_profile_to_default_resp_msg);

  memset(wds_reset_profile_to_default_resp_msg,
         0,
         sizeof(wds_reset_profile_to_default_resp_msg_v01));
  
  /*----------------------------------------------------------------------- 
    Send request QMI_WDS_SET_EVENT_REPORT_REQ_MSG to QMI-WDS service
  ------------------------------------------------------------------------*/
  if(QMI_NO_ERR != qmi_client_send_msg_sync
                   (
                     ds_profile_remote_client_handle,
                     QMI_WDS_RESET_PROFILE_TO_DEFAULT_REQ_V01,
                     wds_reset_profile_to_default_req_msg,
                     sizeof(wds_reset_profile_to_default_req_msg_v01),
                     wds_reset_profile_to_default_resp_msg,
                     sizeof(wds_reset_profile_to_default_resp_msg_v01),
                     500
                   ))
  {
    LOG_MSG_ERROR_0("ds_profile_remote_reset_profile_to_default_cmd_hdlr(): Send msg "
                    "async failed.");
    errval = DS_PROFILE_REG_RESULT_FAIL;
    goto bail;
  }
  
  if(wds_reset_profile_to_default_resp_msg->resp.error != QMI_ERR_NONE_V01)
  {
    LOG_MSG_ERROR_1("ds_profile_remote_reset_profile_to_default_cmd_hdlr() "
                    "Response error=%d.",
                    wds_reset_profile_to_default_resp_msg->resp.error);
    errval = DS_PROFILE_REG_RESULT_FAIL;
    goto bail;
  }
  
  LOG_MSG_INFO2_0("ds_profile_remote_reset_profile_to_default_cmd_hdlr(): Profile on "
                  "remote modem set to default");
  
bail:
  /*-------------------------------------------------------------------------
     Release the memory 
  -------------------------------------------------------------------------*/ 
  LOG_MSG_INFO2_0("ds_profile_remote_reset_profile_to_default_cmd_hdlr() "
                  "Freeing mem allocated "); 

  if(wds_reset_profile_to_default_req_msg != NULL)
  {  
    DS_PROFILE_MEM_FREE((void *)wds_reset_profile_to_default_req_msg,
                   MODEM_MEM_CLIENT_DATA);       
  }    
  if(wds_reset_profile_to_default_resp_msg != NULL)
  {
    DS_PROFILE_MEM_FREE((void *)wds_reset_profile_to_default_resp_msg,
                   MODEM_MEM_CLIENT_DATA);
  }
  
  if(errval != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    LOG_MSG_ERROR_0("ds_profile_remote_reset_profile_to_default_cmd_hdlr() Remote"
                    "  operation for reset profile to default failed ");
  }
  
  return;
} /* ds_profile_remote_reset_profile_to_default_cmd_hdlr() */


/*===========================================================================
FUNCTION DS_PROFILE_REMOTE_GET_PROFILE_LIST

DESCRIPTION
  This API sends request to remote modem to retrieve all the profile list

PARAMETERS
  profile_list  :  List of profile retrieved from remote modem
  profile_index :  Profile number
  
DEPENDENCIES 
  
RETURN VALUE 
  DS_PROFILE_REG_RESULT_SUCCESS : On Success
  DS_PROFILE_REG_RESULT_FAIL    : On Failure
  
SIDE EFFECTS 
 
===========================================================================*/
int16 ds_profile_remote_get_profile_list
(
  ds_profile_remote_profile_list_type * profile_list,
  ds_profile_tech_etype                profile_type
)
{
  wds_get_profile_list_req_msg_v01  * wds_get_profile_list_req_msg = NULL;
  wds_get_profile_list_resp_msg_v01 * wds_get_profile_list_resp_msg = NULL;
  int                                 ret_val = DS_PROFILE_REG_RESULT_SUCCESS;
  int                                 cnt_profiles = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO1_1("ds_profile_remote_get_profile_list() profile type %d",
                  profile_type);
  
  if( !ds_profile_remote_client_handle )
  {
     LOG_MSG_ERROR_0("ds_profile_remote_get_profile_list(): QMI WDS client"
                     " handle null");
     return DS_PROFILE_REG_RESULT_FAIL;
  }
  
  if(profile_list == NULL)
  {
     LOG_MSG_ERROR_0("ds_profile_remote_get_profile_list(): Invalid input"
                     " parameter profile_list null");
     return DS_PROFILE_REG_RESULT_FAIL;  
  }
  
  if(profile_type >= DS_PROFILE_TECH_MAX)
  {
     LOG_MSG_ERROR_1("ds_profile_remote_get_profile_list(): Invalid input"
                     " parameter profile type %d", profile_type);
     return DS_PROFILE_REG_RESULT_FAIL;  
  }

  /*----------------------------------------------------------------------- 
    Allocate dynamic memory for response message. Memory will be freed
    during response message callback handler
  ------------------------------------------------------------------------*/
  wds_get_profile_list_req_msg 
    = (wds_get_profile_list_req_msg_v01 *)
        DS_PROFILE_MEM_ALLOC(sizeof(wds_get_profile_list_req_msg_v01),
                             MODEM_MEM_CLIENT_DATA_CRIT);

  if(wds_get_profile_list_req_msg == NULL)
  {
    LOG_MSG_ERROR_1("ds_profile_remote_get_profile_list() : Couldn't get "
                    "data_block_ptr memory. Free mem 0x%x",
                    wds_get_profile_list_req_msg );
    ret_val = DS_PROFILE_REG_RESULT_FAIL;
    goto bail;
  }

  LOG_MSG_INFO2_1("ds_profile_remote_get_profile_list() : Allocated mem"
                  "  for set event report resp message 0x%x",
                  wds_get_profile_list_req_msg);

  memset(wds_get_profile_list_req_msg,
         0,
         sizeof(wds_get_profile_list_req_msg_v01));
 
  wds_get_profile_list_req_msg->profile_type_valid = TRUE;
  wds_get_profile_list_req_msg->profile_type = profile_type;
  
  /*----------------------------------------------------------------------- 
    Allocate dynamic memory for response message. Memory will be freed
    during response message callback handler
  ------------------------------------------------------------------------*/
  wds_get_profile_list_resp_msg 
    = (wds_get_profile_list_resp_msg_v01*)
        DS_PROFILE_MEM_ALLOC(sizeof(wds_get_profile_list_resp_msg_v01),
                             MODEM_MEM_CLIENT_DATA_CRIT);

  if(wds_get_profile_list_resp_msg == NULL)
  {
    LOG_MSG_ERROR_1("Couldn't get data_block_ptr memory. Free mem 0x%x",
                    wds_get_profile_list_resp_msg );
    ret_val = DS_PROFILE_REG_RESULT_FAIL;
    goto bail;
  }

  LOG_MSG_INFO2_1("ds_profile_remote_get_profile_list() : Allocated mem "
                  "for set event report resp message 0x%x",
                  wds_get_profile_list_resp_msg);

  memset(wds_get_profile_list_resp_msg,
         0,
         sizeof(wds_get_profile_list_resp_msg_v01));
  
  /*----------------------------------------------------------------------- 
    Send request to QMI-WDS service
  ------------------------------------------------------------------------*/
  if(QMI_NO_ERR != qmi_client_send_msg_sync
                   (
                     ds_profile_remote_client_handle,
                     QMI_WDS_GET_PROFILE_LIST_REQ_V01,
                     wds_get_profile_list_req_msg,
                     sizeof(wds_get_profile_list_req_msg_v01),
                     wds_get_profile_list_resp_msg,
                     sizeof(wds_get_profile_list_resp_msg_v01),
                     500
                   ))
  {
    LOG_MSG_ERROR_0("ds_profile_remote_get_profile_list() : Send msg async "
                    "failed. ");
    ret_val = DS_PROFILE_REG_RESULT_FAIL;
    goto bail;    
  }                                             

  if(wds_get_profile_list_resp_msg->resp.error != QMI_ERR_NONE_V01)
  {
    LOG_MSG_ERROR_1("ds_profile_remote_get_profile_list() "
                    "Response error=%d.",
                    wds_get_profile_list_resp_msg->resp.error);
    ret_val = DS_PROFILE_REG_RESULT_FAIL;
    goto bail;
  }
  
  if(wds_get_profile_list_resp_msg->profile_list_len >= 
      DS_PROFILE_REMOTE_NUM_PROFILE_MAX)
  {
    LOG_MSG_ERROR_2("ds_profile_remote_get_profile_list() "
                    "Number of profile %d exceeds max value %d",
                    wds_get_profile_list_resp_msg->profile_list_len,
                    DS_PROFILE_REMOTE_NUM_PROFILE_MAX);
    ret_val = DS_PROFILE_REG_RESULT_FAIL;
    goto bail;
  }
  
  profile_list->profile_list_len = 
    wds_get_profile_list_resp_msg->profile_list_len;    

  LOG_MSG_INFO2_1 ("dsprofile_remote_get_profile_list() remote Profile retreived "
                   " len %d", profile_list->profile_list_len);
                 
  
  /*-------------------------------------------------------------------------
     Loop through all profiles and retrieve the profile index for each 
     profiles. We just need profile index wrt to making sure that profile
     on local and remote modem are same.
  -------------------------------------------------------------------------*/ 
  for(cnt_profiles=0; 
      cnt_profiles < wds_get_profile_list_resp_msg->profile_list_len; 
      cnt_profiles++)
  {
    profile_list->profile_list[cnt_profiles].profile_index
      = wds_get_profile_list_resp_msg->profile_list[cnt_profiles].profile_index;
    
    LOG_MSG_INFO2_1 ("dsprofile_remote_get_profile_list() remote Profile retreived "
                     " %d",
                     profile_list->profile_list[cnt_profiles].profile_index);
  }
           
bail:
  /*-------------------------------------------------------------------------
     Release the memory 
  -------------------------------------------------------------------------*/ 
  LOG_MSG_INFO2_0("ds_profile_remote_get_profile_list() "
                  "Freeing mem allocated "); 

  if(wds_get_profile_list_req_msg != NULL)
  {  
    DS_PROFILE_MEM_FREE((void *)wds_get_profile_list_req_msg,MODEM_MEM_CLIENT_DATA);       
  }    
  if(wds_get_profile_list_resp_msg != NULL)
  {
    DS_PROFILE_MEM_FREE((void *)wds_get_profile_list_resp_msg,MODEM_MEM_CLIENT_DATA);
  }
  
  return ret_val;  
} /* ds_profile_remote_get_profile_list() */


/*===========================================================================
FUNCTION DS_PROFILE_REMOTE_PERFORM_SYNC

DESCRIPTION
  This API sends request to remote modem to peform the necessary sync 
  operation. Sync operation would make the profile on local and remote modem
  same wrt number of profiles and profile information

PARAMETERS
  
DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
 
===========================================================================*/
void ds_profile_remote_perform_sync(void)
{
  ds_profile_remote_profile_list_type * profile_list;
  ds_profile_itr_type                  itr;
  ds_profile_list_info_type            list_info;
  ds_profile_status_etype              profile_status;
  ds_profile_list_type                 search_list_info;
  ds_profile_info_type                 profile_info;
  byte                                 value[100];
  uint8     num_profiles = 0;
  uint8     cnt_profiles_local;
  uint8     cnt_profiles_remote;
  boolean   profile_found;
  int       errval = DS_PROFILE_REG_RESULT_SUCCESS;
  uint16    profile_index[DS_PROFILE_REMOTE_NUM_PROFILE_MAX];
  uint8     index;
  uint8     array_size;
  ds_profile_identifier_type           profile_identifier;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO1_0("ds_profile_remote_perform_sync() ");
  
  /*----------------------------------------------------------------------- 
    Allocate dynamic memory for response message. Memory will be freed
    during response message callback handler
  ------------------------------------------------------------------------*/
  profile_list 
    = (ds_profile_remote_profile_list_type*)
        DS_PROFILE_MEM_ALLOC(sizeof(ds_profile_remote_profile_list_type),
                             MODEM_MEM_CLIENT_DATA_CRIT);

  if(profile_list == NULL)
  {
    LOG_MSG_ERROR_1("ds_profile_remote_perform_sync() : Couldn't get "
                    "data_block_ptr memory. Free mem 0x%x", profile_list );
    errval = DS_PROFILE_REG_RESULT_FAIL;
    goto bail;
  }

  LOG_MSG_INFO2_1("ds_profile_remote_perform_sync() : Allocated mem "
                  "for set event report resp message 0x%x", profile_list);

  memset(profile_list,
         0,
         sizeof(ds_profile_remote_profile_list_type));
         
  ds_profile_remote_get_profile_list( profile_list, DS_PROFILE_TECH_3GPP );
  
  memset(&search_list_info, 0, sizeof(search_list_info));
  /* Default operation - Get all profiles */
  search_list_info.dfn = DS_PROFILE_LIST_ALL_PROFILES;
  
  profile_status = ds_profile_get_list_itr( DS_PROFILE_TECH_3GPP, 
                                            &search_list_info, 
                                            &itr); 
  if(profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    if(profile_status == DS_PROFILE_REG_RESULT_LIST_END)
    {
      LOG_MSG_INFO1_0("ds_profile_remote_perform_sync() No Profile entry found"
                      " matching the request");
      profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
    } else
    {
      LOG_MSG_ERROR_1 ("ds_profile_remote_perform_sync() Getting profile list "
                       "iterator failed. Error %d", profile_status);
      errval = DS_PROFILE_REG_RESULT_FAIL;
    }
    goto bail;
  }
  
  while(profile_status != DS_PROFILE_REG_RESULT_LIST_END)
  {
    memset(&list_info, 0, sizeof (ds_profile_list_info_type));
    memset(&profile_info, 0, sizeof (ds_profile_info_type));
    memset(value, 0, sizeof(value));
    
    list_info.name = &profile_info;
    list_info.name->len  = sizeof(value);
    list_info.name->buf  = (void*)value;
    
    profile_status = ds_profile_get_info_by_itr(itr, &list_info);
    if(profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
    {
      LOG_MSG_ERROR_1 ("ds_profile_remote_perform_sync() Getting profile info by "
                       "iterator failed. Error %d", profile_status);
      (void) ds_profile_itr_destroy(itr);
      errval = -1;
      goto bail;
    }
    
    profile_index[num_profiles] = list_info.num;

    num_profiles++;

    profile_status = ds_profile_itr_next(itr);
    if ( (profile_status != DS_PROFILE_REG_RESULT_SUCCESS) &&
         (profile_status != DS_PROFILE_REG_RESULT_LIST_END) )
    {
      LOG_MSG_ERROR_1 ("ds_profile_remote_perform_sync() Getting Next iterator "
                       "failed. Error %d", profile_status);
      (void) ds_profile_itr_destroy(itr);
      errval = DS_PROFILE_REG_RESULT_FAIL;
      goto bail;
    }    
  }
  
  /*-------------------------------------------------------------------------
    Destroy the iterator
  -------------------------------------------------------------------------*/
  profile_status = ds_profile_itr_destroy(itr);
  if(profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    LOG_MSG_ERROR_1 ("ds_profile_remote_perform_sync() Destroying iterator "
                     "failed. Error %d", profile_status);
    errval = DS_PROFILE_REG_RESULT_FAIL;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Delete all profiles on remote modem that doesnt exist on local modem
  -------------------------------------------------------------------------*/
  for(cnt_profiles_remote=0; cnt_profiles_remote < profile_list->profile_list_len;
      cnt_profiles_remote++)  
  {
    profile_found = FALSE;
    for(cnt_profiles_local=0; cnt_profiles_local < num_profiles; 
        cnt_profiles_local++)
    {
      if(profile_list->profile_list[cnt_profiles_remote].profile_index == 
          profile_index[cnt_profiles_local] )
      {
        profile_found = TRUE;
      }
    }
    
    if(profile_found == FALSE)
    {
      ds_profile_remote_delete_profile
      (  
        DS_PROFILE_TECH_3GPP,
        profile_list->profile_list[cnt_profiles_remote].profile_index
      );
    }
  }  

  /*-------------------------------------------------------------------------
    Create all profiles on remote modem that exist on local modem and not 
    on remote modem
  -------------------------------------------------------------------------*/  
  for(cnt_profiles_local=0; cnt_profiles_local < num_profiles; 
      cnt_profiles_local++)
  {
    profile_found = FALSE;
    for(cnt_profiles_remote=0; cnt_profiles_remote < profile_list->profile_list_len;
        cnt_profiles_remote++)
    {
      if(profile_list->profile_list[cnt_profiles_remote].profile_index == 
          profile_index[cnt_profiles_local] )
      {
        profile_found = TRUE;
      }
    }
    
    if(profile_found == FALSE)
    {
      ds_profile_remote_create_profile(DS_PROFILE_TECH_3GPP);
    }
  }

  /*-------------------------------------------------------------------------
    Modify all the profiles on remote modem with values from local modem
  -------------------------------------------------------------------------*/  
  for(cnt_profiles_local=0; cnt_profiles_local < num_profiles; 
      cnt_profiles_local++)
  {
    array_size = sizeof(ds_profile_remote_3gpp_profile_params) / 
                   sizeof(ds_profile_remote_3gpp_profile_params[0]);   

    index = 0;

    while (index < array_size)
    {
      profile_identifier = ds_profile_remote_3gpp_profile_params[index]; 
      ds_profile_remote_profile_identifier_set(profile_identifier,TRUE);
      index++;  
    }
    
    ds_profile_remote_modify_profile(profile_index[cnt_profiles_local]);
  }
  
bail:
  /*-------------------------------------------------------------------------
     Release the memory 
  -------------------------------------------------------------------------*/ 
  LOG_MSG_INFO2_0("ds_profile_remote_get_profile_list() "
                  "Freeing mem allocated "); 

  if(profile_list != NULL)
  {  
    DS_PROFILE_MEM_FREE((void *)profile_list,MODEM_MEM_CLIENT_DATA);       
  }
  
  if(errval != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    LOG_MSG_ERROR_0("ds_profile_remote_perform_sync() Remote operation"
                    " for syncing profile during bootup failed ");
  }
  
} /* ds_profile_remote_perform_sync() */

/*===========================================================================
FUNCTION       DS_PROFILE_REMOTE_INIT

DESCRIPTION    Called during powerup to register handler with dcc task to 
               perform remote operation

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_profile_remote_init(void)
{
  LOG_MSG_INFO1_0("ds_profile_remote_init() " );
  
  dcc_set_cmd_handler(DCC_DS_PROFILE_REMOTE_PERFORM_OPERATION_CMD,
                      ds_profile_remote_perform_operaton_cmd_handler);
}/* ds_profile_remote_init() */


/*===========================================================================
FUNCTION       DS_PROFILE_REMOTE_MODIFY_PROFILE

DESCRIPTION    Sends cmd to dcc task to modify profile on remote modem

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_profile_remote_modify_profile(int profile_num)
{
  ds_profile_remote_perform_operation_info_type * op_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  LOG_MSG_INFO1_0("ds_profile_remote_modify_profile() " );
  
  op_info = 
    (ds_profile_remote_perform_operation_info_type *)
      DS_PROFILE_MEM_ALLOC
       (sizeof(ds_profile_remote_perform_operation_info_type),
         MODEM_MEM_CLIENT_DATA_CRIT);   
          
  op_info->operation_type = DS_PROFILE_REMOTE_OPERATION_PROFILE_MODIFY;
  op_info->profile_index = profile_num;    
  dcc_send_cmd_ex (DCC_DS_PROFILE_REMOTE_PERFORM_OPERATION_CMD, op_info);
  
}/* ds_profile_remote_modify_profile() */

/*===========================================================================
FUNCTION       DS_PROFILE_REMOTE_DELETE_PROFILE

DESCRIPTION    Sends cmd to dcc task to delete profile on remote modem

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_profile_remote_delete_profile
(  
  uint8           profile_tech_type,
  uint8           profile_index
)
{
  ds_profile_remote_perform_operation_info_type * op_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  LOG_MSG_INFO1_0("ds_profile_remote_delete_profile() " );
  
  op_info = 
    (ds_profile_remote_perform_operation_info_type *)
      DS_PROFILE_MEM_ALLOC
       (sizeof(ds_profile_remote_perform_operation_info_type),
         MODEM_MEM_CLIENT_DATA_CRIT);   
          
  op_info->operation_type = DS_PROFILE_REMOTE_OPERATION_PROFILE_DELETE;
  op_info->profile_index = profile_index;    
  op_info->tech_type = profile_tech_type;   
  dcc_send_cmd_ex (DCC_DS_PROFILE_REMOTE_PERFORM_OPERATION_CMD, op_info);
  
}/* ds_profile_remote_delete_profile() */

/*===========================================================================
FUNCTION       DS_PROFILE_REMOTE_CREATE_PROFILE

DESCRIPTION    Sends cmd to dcc task to create profile on remote modem

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_profile_remote_create_profile
(  
  uint8           profile_tech_type
)
{
  ds_profile_remote_perform_operation_info_type * op_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
  LOG_MSG_INFO1_0("ds_profile_remote_create_profile() " );
  
  op_info = 
  (ds_profile_remote_perform_operation_info_type *)
    DS_PROFILE_MEM_ALLOC
     (sizeof(ds_profile_remote_perform_operation_info_type),
       MODEM_MEM_CLIENT_DATA_CRIT);   
          
  op_info->operation_type = DS_PROFILE_REMOTE_OPERATION_PROFILE_CREATE; 
  op_info->tech_type = profile_tech_type;   
  dcc_send_cmd_ex (DCC_DS_PROFILE_REMOTE_PERFORM_OPERATION_CMD, op_info);
  
  return;
  
}/* ds_profile_remote_create_profile() */

/*===========================================================================
FUNCTION       DS_PROFILE_REMOTE_RESET_PROFILE_TO_DEFAULT

DESCRIPTION    Sends cmd to dcc task to reset profile to default on remote 
               modem

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_profile_remote_reset_profile_to_default
(
  uint8           profile_index
)
{
  ds_profile_remote_perform_operation_info_type * op_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  LOG_MSG_INFO1_0("ds_profile_remote_reset_profile_to_default() " );
  
  op_info = 
    (ds_profile_remote_perform_operation_info_type *)
      DS_PROFILE_MEM_ALLOC
       (sizeof(ds_profile_remote_perform_operation_info_type),
         MODEM_MEM_CLIENT_DATA_CRIT);   
          
  op_info->operation_type = DS_PROFILE_REMOTE_OPERATION_PROFILE_RESET;
  op_info->profile_index = profile_index;   
  dcc_send_cmd_ex (DCC_DS_PROFILE_REMOTE_PERFORM_OPERATION_CMD, op_info);
  
}/* ds_profile_remote_reset_profile_to_default() */

#else /*#ifndef FEATURE_8960_SGLTE_FUSION*/

/*===========================================================================
FUNCTION DS_PROFILE_REMOTE_PROFILE_INDENTIFIER_SET

DESCRIPTION
  This API lets the local modem know that profile identifer Is set and it has
  to send it to remote modem.

PARAMETERS
  profile_identifier :  profile identifier
  is_set             :  Profile identifier is set or unset
  
DEPENDENCIES 
  
RETURN VALUE 

SIDE EFFECTS 
 
===========================================================================*/
void ds_profile_remote_profile_identifier_set
(
  int      profile_identifier, 
  boolean  is_set
)
{
  LOG_MSG_INFO1_0("ds_profile_remote_profile_identifier_set(): Not supported "); 
  return;
}

/*===========================================================================
FUNCTION DS_PROFILE_REMOTE_IS_SET_PROFILE_INDENTIFIER

DESCRIPTION
  This API lets local modem know that profile identifier has not been set 
  and hence it neednt send it to remote modem

PARAMETERS
  profile_identifier :  profile identifier
  
DEPENDENCIES 
  
RETURN VALUE 
  TRUE   :   Profile identifier is set.
  FALSE  :   Profile identifier is not set.
  
SIDE EFFECTS 
 
===========================================================================*/
boolean ds_profile_remote_is_set_profile_identifier
(
  int profile_identifier
)
{
  LOG_MSG_INFO1_0("ds_profile_remote_is_set_profile_identifier(): Not supported"); 
  return FALSE;
}

/*===========================================================================
FUNCTION DS_PROFILE_REMOTE_CREATE_PROFILE_CMD_HDLR

DESCRIPTION
  This API sends request to remote modem to create the profile for given 
  tech type.

PARAMETERS
  tech :  Technology type
  
DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
 
===========================================================================*/
void ds_profile_remote_create_profile_cmd_hdlr
(
  ds_profile_tech_etype  tech
)
{
  LOG_MSG_INFO1_0("ds_profile_remote_create_profile_cmd_hdlr(): Not supported"); 
  return;
}

/*===========================================================================
FUNCTION DS_PROFILE_REMOTE_MODIFY_PROFILE_CMD_HDLR

DESCRIPTION
  This API sends request to remote modem to modify the profile information 
  for given profile index 

PARAMETERS
  profile_index :  Profile number
  
DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
 
===========================================================================*/
void ds_profile_remote_modify_profile_cmd_hdlr
(
  ds_profile_num_type    profile_index
)
{
  LOG_MSG_INFO1_0("ds_profile_remote_modify_profile_cmd_hdlr(): Not supported"); 
  return;
}

/*===========================================================================
FUNCTION DS_PROFILE_REMOTE_DELETE_PROFILE_CMD_HDLR

DESCRIPTION
  This API sends request to remote modem to delete the profile information
  for given profile index

PARAMETERS
  profile_type  :  Technology type like 3gpp or 3gpp profile
  profile_index :  Profile number
  
DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
 
===========================================================================*/
void ds_profile_remote_delete_profile_cmd_hdlr
(
  uint8           profile_type,
  uint8           profile_index
)
{
  LOG_MSG_INFO1_0("ds_profile_remote_delete_profile_cmd_hdlr(): Not supported"); 
  return;
}

/*===========================================================================
FUNCTION DS_PROFILE_REMOTE_RESET_PROFILE_TO_DEFAULT_CMD_HDLR

DESCRIPTION
  This API sends request to remote modem to reset the profile information 
  to default values

PARAMETERS
  profile_index :  Profile number
  
DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
 
===========================================================================*/
void ds_profile_remote_reset_profile_to_default_cmd_hdlr
(
  uint8  profile_index
)
{
  LOG_MSG_INFO1_0("ds_profile_remote_reset_profile_to_default_cmd_hdlr(): Not supported"); 
  return;
}

/*===========================================================================
FUNCTION DS_PROFILE_REMOTE_GET_PROFILE_LIST

DESCRIPTION
  This API sends request to remote modem to retrieve all the profile list

PARAMETERS
  profile_list  :  List of profile retrieved from remote modem
  profile_index :  Profile number
  
DEPENDENCIES 
  
RETURN VALUE 
  DS_PROFILE_REG_RESULT_SUCCESS  : On Success
  DS_PROFILE_REG_RESULT_FAIL     : On Failure
  
SIDE EFFECTS 
 
===========================================================================*/
int16 ds_profile_remote_get_profile_list
(
  ds_profile_remote_profile_list_type * profile_list,
  ds_profile_tech_etype                profile_type
)
{
  LOG_MSG_INFO1_0("ds_profile_remote_get_profile_list(): Not supported"); 
  return DS_PROFILE_REG_RESULT_FAIL;
}

/*===========================================================================
FUNCTION DS_PROFILE_REMOTE_PERFORM_SYNC

DESCRIPTION
  This API sends request to remote modem to peform the necessary sync 
  operation. Sync operation would make the profile on local and remote modem
  same wrt number of profiles and profile information

PARAMETERS
  
DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
 
===========================================================================*/
void ds_profile_remote_perform_sync(void)
{
  LOG_MSG_INFO1_0("ds_profile_remote_perform_sync(): Not supported"); 
  return;
}

/*===========================================================================
FUNCTION       DS_PROFILE_REMOTE_INIT

DESCRIPTION    Called during powerup to register handler with dcc task to 
               perform remote operation

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_profile_remote_init(void)
{
  LOG_MSG_INFO1_0("ds_profile_remote_init() " );
  
  return;
}/* ds_profile_remote_init() */

/*===========================================================================
FUNCTION       DS_PROFILE_REMOTE_MODIFY_PROFILE

DESCRIPTION    Sends cmd to dcc task to modify profile on remote modem

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_profile_remote_modify_profile(int profile_num)
{  
  LOG_MSG_INFO1_0("ds_profile_remote_modify_profile() " );

  return;  
}/* ds_profile_remote_modify_profile() */

/*===========================================================================
FUNCTION       DS_PROFILE_REMOTE_DELETE_PROFILE

DESCRIPTION    Sends cmd to dcc task to delete profile on remote modem

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_profile_remote_delete_profile
(  
  uint8           profile_tech_type,
  uint8           profile_index
)
{  
  LOG_MSG_INFO1_0("ds_profile_remote_delete_profile() " );
  
  return;
  
}/* ds_profile_remote_delete_profile() */

/*===========================================================================
FUNCTION       DS_PROFILE_REMOTE_CREATE_PROFILE

DESCRIPTION    Sends cmd to dcc task to create profile on remote modem

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_profile_remote_create_profile
(  
  uint8           profile_tech_type
)
{  
  LOG_MSG_INFO1_0("ds_profile_remote_create_profile() " );
  
  return;
  
}/* ds_profile_remote_create_profile() */

/*===========================================================================
FUNCTION       DS_PROFILE_REMOTE_RESET_PROFILE_TO_DEFAULT

DESCRIPTION    Sends cmd to dcc task to reset profile to default on remote 
               modem

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_profile_remote_reset_profile_to_default
(
  uint8           profile_index
)
{  
  LOG_MSG_INFO1_0("ds_profile_remote_reset_profile_to_default() " );
  
  return;
  
}/* ds_profile_remote_reset_profile_to_default() */

#endif /* FEATURE_8960_SGLTE_FUSION */


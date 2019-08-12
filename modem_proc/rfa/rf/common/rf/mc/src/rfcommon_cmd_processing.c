/*!
  @file
  rfcommon_cmd_processing.c

  @brief
  This file contains the definitions for RFcommon APIs to register, deregister RFM UMIDs with MSGR as well as call the dispatcher for the MSGR cmds.

*/

/*=============================================================================
Copyright (c) 2008, 2009, 2010, 2011, 2012, 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/mc/src/rfcommon_cmd_processing.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/02/13   sb     Initial version 
10/03/13   sb      Add changes to receive MSGR cmd from MCS to switch to filtered/unfiltered 
                   path for SGLTE coexistence (GSM DCS +LTE B39/TDS B39/B34) 

============================================================================*/
#include "rfcom.h"
#include "rfcommon_core.h"
#include "rfcommon_msg.h"
//#include "rfcommon_locks.h"
//#include "rfcommon_mc.h"  /* common to all technologies */
#include "rfcommon_cmd_processing.h"
#include "rf_cmd_dispatcher.h"
#include "stringl.h"
#include "rf_dispatch.h"
#include "rf_apps_task.h"

static msgr_umid_type rfcommon_umid_req_list[] = 
{
  RFA_RF_COMMON_BYPASS_FILTERED_PATH_REQ,

};


//static msgr_umid_type rfm_common_umid_rsp_list[] = 
//{
//  RFA_RF_COMMON_BYPASS_FILTERED_PATH_RSP,
//
//};


/*---------------------------------------------------------------------------*/
/*
  @brief
  This API is called by RF Task to register for any message router commands
  that it intends to recieve.  
 
  @details
  During rf_rex_msgr_interface_init, MSGR gives the RF task a client ID and mailbox.
  When this API is called to register the UMID list with MSGR, the same client ID and mailbox are used.
 
  @params
  Client ID and Mailbox ID
 
  @retval

*/

boolean rfcommon_msgr_register( msgr_client_t *client_id, msgr_id_t id )
{
  boolean         rc         = TRUE;
  errno_enum_type ret_status = E_SUCCESS;


  /* Register for the RF GSM L1 Commands that the RF task is interested in receiving */
  ret_status = msgr_register_block(MSGR_RFA_RF_COMMON,
                                   client_id,
                                   id,
                                   rfcommon_umid_req_list,
                                   RFCOMMON_REQ_CNT(rfcommon_umid_req_list));


  if (ret_status != E_SUCCESS)
  {

    RF_MSG_1(RF_ERROR, "rfcommon_msgr_register: Unable to register for RFM COMMON message router register block, %d",ret_status);
    rc = FALSE;
  }


  return(rc);
}


/*---------------------------------------------------------------------------*/
/*
  @brief
  During RF Message Router Tear down, this API deregisters the UMID list with MSGR (for commands
  that RF intends to recieve)   
 
  @details

  @params
  Client ID
 
  @retval

*/


boolean rfcommon_msgr_deregister( msgr_client_t *client_id )
{
  boolean         rc         = TRUE;
  errno_enum_type ret_status = E_SUCCESS;

  /* Deregister all the RF GSM L1 message RF task has registered for. */
  ret_status = msgr_deregister_block(MSGR_RFA_RF_COMMON,
                                   client_id,
                                   rfcommon_umid_req_list,
                                   RFCOMMON_REQ_CNT(rfcommon_umid_req_list));

  if (ret_status != E_SUCCESS)
  {

    RF_MSG_1(RF_ERROR, "rfcommon_msgr_deregister: Unable to deregister RF message router register block, %d",ret_status);
    rc = FALSE;
  }
  return(rc);
}

/*---------------------------------------------------------------------------*/
/*
  @brief
  This API is a common location to register rf apps task commands with rf dispatch.
 
  @details
  This API is called by rfcommon mc init to register any rf apps task commands
  with the rf dispatcher.
  
  @params
  
  @retval
  TRUE if all rf dispatch registration completed successfully
*/

boolean rfcommon_rf_apps_task_register(void)
{
  boolean ret_val = TRUE;

  ret_val &= rf_dispatch_register_command(rf_apps_task_get_dispatch_handle(),       // dispatch handle
                                          MSGR_RFA_RF_COMMON,                       // module self
                                          RFA_RF_COMMON_MSG_ID_RFFE_SCAN_EFS_DUMP,  // cid
                                          rfc_common_rffe_scan_efs_dump,            // cid handler
                                          NULL,                                     // cb data
                                          0);                                       // reg action

  return ret_val;
}


/*---------------------------------------------------------------------------*/
/*
  @brief
  This API is a common location to deregister rf apps task commands with rf dispatch.
 
  @details
  This API deregisters rf apps task commands with the rf dispatcher. 
  It is called from rfcommon_mc_deinit()

  @params
 
  @retval
  TRUE if all rf dispatch deregistration completed successfully
*/

boolean rfcommon_rf_apps_task_deregister(void)
{
  boolean ret_val = TRUE;
  
  /* Deregister rffe_scan_efs_dump */
  ret_val &= rf_dispatch_deregister_command( rf_apps_task_get_dispatch_handle(),
                                            MSGR_RFA_RF_COMMON,                       
                                            RFA_RF_COMMON_MSG_ID_RFFE_SCAN_EFS_DUMP);
  
  return ret_val;
}


/*---------------------------------------------------------------------------*/
/*
  @brief
  This API is a dispatcher to handle the MSGR commands. It receives the MSGR cmd and based on the cmd ID calls the appropriate RF API  
 
  @details

  @params
  pointer to the MSGR message structure
 
  @retval

*/


rfm_dispatch_outcome_type rfcommon_req_dispatch(rfm_msg_u* msg_ptr)
{
  rfm_dispatch_outcome_type req_cnf = RFM_DISPATCH_UNSUCCESSFUL;
  msgr_hdr_s   *msgr_hdr_ptr = (msgr_hdr_s*) msg_ptr; 

  if (msg_ptr != NULL)
  {
    switch ( msgr_hdr_ptr->id )
    {
       case RFA_RF_COMMON_BYPASS_FILTERED_PATH_REQ:
           req_cnf = rfcommon_core_use_filtered_path( (rfm_rf_cmn_bypass_filtered_path_req_s *)msg_ptr, RFA_RF_COMMON_BYPASS_FILTERED_PATH_RSP);
          break;

       default :
          RF_MSG_1(RF_ERROR, "rfcommon_req_dispatch, Undefined RFM common REQ, %d",msgr_hdr_ptr->id);
          req_cnf = RFM_DISPATCH_UNSUCCESSFUL;
          break;
    }
  }

  return req_cnf;

}




void rfcommon_req_dispatcher( rf_cmd_type* req_ptr )
{
  rfm_dispatch_outcome_type dispatch_result = RFM_DISPATCH_SUCCESSFUL;
  rfm_msg_u rfm_msgr_req;
  uint8 * cpy_dest;
  void * cpy_src;
  size_t cpy_size;

  if( req_ptr != NULL )
  {
    /*Limit memcpy message copy size to max allowable size*/
    cpy_size = MIN(sizeof(rfm_msg_u)-sizeof(rf_msgr_hdr_struct_type), RF_MAX_CMD_PAYLOAD_SIZE);

    cpy_src = (void *) &req_ptr->msgr_hdr;
    cpy_dest = (uint8 *) &rfm_msgr_req;

    /*Copy the MSGR request packet header */
    memscpy( (void *)cpy_dest, sizeof(rfm_msg_u), cpy_src, sizeof(rf_msgr_hdr_struct_type));

    /* Set the destination and source pointers to the payload */
    cpy_src = (void *) &req_ptr->payload;
    cpy_dest += sizeof(rf_msgr_hdr_struct_type);

    /* Copy the payload */
    memscpy( (void *)cpy_dest, (sizeof(rfm_msg_u)-sizeof(rf_msgr_hdr_struct_type)), cpy_src, cpy_size );

    /* Dispatch the message */
    dispatch_result = rfcommon_req_dispatch(&rfm_msgr_req);

    if (dispatch_result != RFM_DISPATCH_SUCCESSFUL)
    {

      RF_MSG_1(RF_ERROR, "RF command received could not process the command. Exited with error code, %d",dispatch_result);
     
      dispatch_result = RFM_DISPATCH_UNSUCCESSFUL;
    }
  }
}


/*---------------------------------------------------------------------------*/
/*
  @brief
  This API is called by a RF task to register the MSGR dispatcher using RFA common module.  
 
  @details

  @params
  pointer to the MSGR message structure
 
  @retval

*/


void rfcommon_dispatch_init(rf_task_num_type task)
{

   MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "rfcommon_dispatch_init called" );
  /*Register with rf module dispatch table using RF RFA as the module.*/
  if(rf_cmd_dispatch_register_tech( RF_TASK_MAIN, 
                                    MSGR_TECH_RFA, 
                                    MSGR_MODULE_RF_COMMON, 
                                    rfcommon_req_dispatcher ) != RF_CMD_REGISTRATION_SUCCESS)
  {

     RF_MSG_1(RF_ERROR, "rfcommon_dispatch_init:Failed MSGR_TECH_RFA / MSGR_MODULE_RF_COMMON registration %d",0);
     return;
  }
}







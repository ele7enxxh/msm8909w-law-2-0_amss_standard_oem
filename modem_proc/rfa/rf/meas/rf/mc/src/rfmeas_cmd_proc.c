

/*!
  @file rfmeas_cmd_processing.c 

  @brief
  This file dispacthes and sends responses for any REQ that MEAS RF driver receives.

*/

/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/


/*===========================================================================


                EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/meas/rf/mc/src/rfmeas_cmd_proc.c#1 $
$DateTime: 2016/12/13 07:59:52 $
$Author: mplcsds1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/31/13   sc      Initial version
  

===========================================================================*/
/*!@ TODO Need to add changes required for Triton iRAT support */
/*===========================================================================

                  INCLUDE FILES

===========================================================================*/
#include "amssassert.h"
#include "appmgr.h"
#include "msgr.h"
#include "rf_cap.h" /*Contains FEATURE_RF_TASK */
#include "rfa_msgr.h"
#include "rf_task.h"
#include "rf_cmd_interface.h"
#include "rf_cmd_dispatcher.h"
#include "rfm.h"
#include "rfm_types.h"
#include "rfmeas_types.h"
#include "rfmeas_msg.h"
#include "rfmeas_mc.h"
#include "rf_hal_buffer.h"
#include "rfm_internal.h"
#include "rf_task_common_services.h"
#include "stringl.h" /* for memscpy */

/*===========================================================================

                                DEFINITIONS

===========================================================================*/
/*! @brief Outcome of the dispatch. 
    @details Outcomes of the dispatch to RF task used by MEAS MC Dispatcher 
*/
typedef enum
{
  RFMEAS_DISPATCH_SUCCESSFUL=1,
  RFMEAS_DISPATCH_UNSUCCESSFUL=2,
  RFMEAS_DISPATCH_NULL_PTR=3,
  RFMEAS_DISPATCH_INVALID_PARAM =4,
  RFMEAS_DISPATCH_MAX
}rfmeas_dispatch_outcome_t;

typedef union
{
  rfa_rf_meas_get_irat_info_param_req_s   rfa_rf_meas_get_irat_info_param;
  rfa_rf_meas_get_time_constant_req_s     rfa_rf_meas_get_time_constant;
  rfa_rf_meas_script_enter_req_s          rfa_rf_meas_script_enter;
  rfa_rf_meas_script_build_scripts_req_s  rfa_rf_meas_script_build_scripts;
  rfa_rf_meas_script_exit_req_s           rfa_rf_meas_script_exit;
} rfmeas_msg_u;

/*! @brief This array holds the request (REQ) command list used to register
 *  the whole list with MSGR.
 */
static msgr_umid_type rfmeas_umid_req_list[] = 
{
    RFA_RF_MEAS_GET_IRAT_INFO_PARAM_REQ,
    RFA_RF_MEAS_GET_TIME_CONSTANT_REQ,
    RFA_RF_MEAS_SCRIPT_ENTER_REQ,
    RFA_RF_MEAS_SCRIPT_BUILD_SCRIPTS_REQ,
    RFA_RF_MEAS_SCRIPT_EXIT_REQ,
};

/*===========================================================================

                               Macros

===========================================================================*/
/*! @brief Definitions of the RF MEAS L1 message count for the mailboxes.
*/
#define RF_MEAS_REQ_CNT \
    (sizeof(rfmeas_umid_req_list)/sizeof(msgr_umid_type))



/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*! @fn rfmeas_req_dispatch
 *  @brief Function prototype for the dispatching functionality.
 */
rfmeas_dispatch_outcome_t rfmeas_req_dispatch(rfmeas_msg_u* msg_ptr);

/*! @brief Function prototypes for the dispatching functionality.          */
rfmeas_dispatch_outcome_t rfmeas_dispatch_get_irat_info_param_req( 
           rfa_rf_meas_get_irat_info_param_req_s* rfmeas_msg_ptr );

rfmeas_dispatch_outcome_t rfmeas_dispatch_get_time_constant_req( 
           rfa_rf_meas_get_time_constant_req_s* rfmeas_msg_ptr );

rfmeas_dispatch_outcome_t rfmeas_dispatch_script_enter_req( 
           rfa_rf_meas_script_enter_req_s* rfmeas_msg_ptr );

rfmeas_dispatch_outcome_t rfmeas_dispatch_script_build_scripts_req( 
           rfa_rf_meas_script_build_scripts_req_s* rfmeas_msg_ptr );

rfmeas_dispatch_outcome_t rfmeas_dispatch_script_exit_req( 
           rfa_rf_meas_script_exit_req_s* rfmeas_msg_ptr );



/*===========================================================================

                      FUNCTIONS

===========================================================================*/
/*===========================================================================
  FUNCTION:  rfmeas_send_msgr_cnf()
===========================================================================*/
/*! @fn rfmeas_send_msgr_cnf(msgr_hdr_struct_type* msg_ptr, uint32 umid, uint32 size){}
 *  @brief This function is called by any function that requires to send a 
 *  confirmation (CNF) MSG.
 *  @param msg_ptr a pointer to message router header structure type
 *  @param umid a uint32 of the Unique Message ID
 *  @param size the payload size
 *  @return errno_enum_type.
 */
errno_enum_type rfmeas_send_msgr_cnf( msgr_hdr_struct_type* msg_ptr, 
                                      uint32 umid, 
                                      uint32 size )
{
  errno_enum_type msgr_send_status = E_SUCCESS;
  
  if( msg_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "MSGR Send NULL Message UMID 0x%X, error_no %d",umid);
    return (E_BAD_ADDRESS);
  }

  /* Send the CNF MSG back here */
  msgr_init_hdr( msg_ptr, MSGR_RFA_RF_MEAS, umid );
    
  msgr_send_status = msgr_send( msg_ptr, size );
  return msgr_send_status;
}



/*===========================================================================

  FUNCTION:  rfmeas_dispatch_get_irat_info_param_req

===========================================================================*/
/*! @fn rfmeas_dispatch_get_irat_info_param_req( rfa_rf_meas_get_irat_info_param_req_s* rfmeas_msg_ptr )
 *  @brief This function dispatches the initialisation of the RF Driver for 
 *  a specific radio transceiver for MEAS.
 *  @param rfmeas_msg_ptr a pointer to the ... structure variable
 *  payload.
 *  @return rfmeas_dispatch_outcome_t enum variable describing the success/
 *  failure of the function.
 */
rfmeas_dispatch_outcome_t rfmeas_dispatch_get_irat_info_param_req( rfa_rf_meas_get_irat_info_param_req_s* rfmeas_msg_ptr )
{
  /* Initialise Response and Result Variables */
  rfmeas_dispatch_outcome_t     req_cnf           = RFMEAS_DISPATCH_UNSUCCESSFUL;
  rfm_meas_result_type          rfmeas_rfm        = RFM_MEAS_COMMON_FAILURE;
  errno_enum_type               msgr_send_status  = E_FAILURE;
  rfa_rf_meas_get_irat_info_param_cnf_s  rfmeas_resp_msg;
  
  /* Verify payload ptr is not NULL */
  if( rfmeas_msg_ptr == NULL )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "Pointer argument into rfmeas_dispatch_get_irat_info_param_req is NULL");
    req_cnf = RFMEAS_DISPATCH_NULL_PTR;
    return req_cnf;
  }

  /* Echo REQ message info to CNF message info for functionality usage. */
  rfmeas_resp_msg.hdr           =     rfmeas_msg_ptr->hdr;
  rfmeas_resp_msg.cnf_required  =     rfmeas_msg_ptr->cnf_required;
  rfmeas_resp_msg.cb_handler    =     rfmeas_msg_ptr->cb_handler;
  rfmeas_resp_msg.req_result    =     RFA_RF_MEAS_EXECUTION_ERROR;

  /* Verify payload parameters. */
  if( rfmeas_msg_ptr->rfm_meas_irat_info_param == NULL )
  {
    /* Payload is bad and will not achieve RFM functionality dispatch. */
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "Pointer argument for get_irat_info_param is NULL");
    rfmeas_resp_msg.req_result  = RFA_RF_MEAS_L1_PARAM_ERROR;
    req_cnf                     = RFMEAS_DISPATCH_NULL_PTR;
    return req_cnf;
  }
  else
  {
    /* Payload is good and has achieved RFM functionality dispatch. */
    req_cnf = RFMEAS_DISPATCH_SUCCESSFUL;
    rfmeas_rfm = rfmeas_mc_irat_info_get( rfmeas_msg_ptr->rfm_meas_irat_info_param,
                                          rfmeas_msg_ptr->cb_handler );
  }

  /*Set the payload result to the result of the above functionality*/
  if( rfmeas_rfm == RFM_MEAS_COMMON_SUCCESS )
  {
    /* RF MC function was successful and therefore send a success response. */
    rfmeas_resp_msg.req_result  =  RFA_RF_MEAS_SUCCESS;
  }

  if( rfmeas_resp_msg.cnf_required )
  {
    /* Assign the iRAT info to confirmation member */
    rfmeas_resp_msg.rfm_meas_irat_info_param = rfmeas_msg_ptr->rfm_meas_irat_info_param;

    /* Send a confirmation message */
    msgr_send_status = rfmeas_send_msgr_cnf((msgr_hdr_struct_type *)&rfmeas_resp_msg,
                                            RFA_RF_MEAS_GET_IRAT_INFO_PARAM_CNF,
                                            sizeof(rfa_rf_meas_get_irat_info_param_cnf_s));
  
    if (msgr_send_status != E_SUCCESS)
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RFMEAS get iRAT info MSGR Send UNSUCCESSFUL, error_no %d",msgr_send_status);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "RFMEAS get iRAT info MSGR Send SUCCESSFUL, status %d",msgr_send_status);
    }
  }
  return(req_cnf);
}



/*===========================================================================

  FUNCTION:  rfmeas_dispatch_get_time_constant_req

===========================================================================*/
/*! @fn rfmeas_dispatch_get_time_constant_req( rfa_rf_meas_get_time_constant_req_s* rfmeas_msg_ptr )
 *  @brief This function gets the time constants based on source and target tech
 *  @param rfmeas_msg_ptr a pointer to the rfa_rf_meas_get_time_constant_req_s structure variable
 *  @return rfmeas_dispatch_outcome_t enum variable describing the success/failure of the function.
 */
/* NOTE: This API is a placeholder if required as currently get_time_constant API does not go through mc layer */
rfmeas_dispatch_outcome_t rfmeas_dispatch_get_time_constant_req( rfa_rf_meas_get_time_constant_req_s* rfmeas_msg_ptr )
{
  /* Initialise Response and Result Variables */
  rfmeas_dispatch_outcome_t     req_cnf           = RFMEAS_DISPATCH_UNSUCCESSFUL;
  rfm_meas_result_type          rfmeas_rfm        = RFM_MEAS_COMMON_FAILURE;
  errno_enum_type               msgr_send_status  = E_FAILURE;
  rfa_rf_meas_get_time_constant_cnf_s   rfmeas_resp_msg;
  
  /* Verify payload ptr is not NULL */
  if( rfmeas_msg_ptr == NULL )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "Pointer argument into rfmeas_dispatch_get_time_constant_req is NULL");
    req_cnf = RFMEAS_DISPATCH_NULL_PTR;
    return req_cnf;
  }

  /* Echo REQ message info to CNF message info for functionality usage. */
  rfmeas_resp_msg.hdr           =     rfmeas_msg_ptr->hdr;
  rfmeas_resp_msg.cnf_required  =     rfmeas_msg_ptr->cnf_required;
  rfmeas_resp_msg.source        =     rfmeas_msg_ptr->source;
  rfmeas_resp_msg.target        =     rfmeas_msg_ptr->target;
  rfmeas_resp_msg.req_result    =     RFA_RF_MEAS_EXECUTION_ERROR;

  /* Verify payload parameters. */
  if( rfmeas_msg_ptr->time_const == NULL )
  {
    /* Payload is bad and will not achieve RFM functionality dispatch. */
    rfmeas_resp_msg.req_result  = RFA_RF_MEAS_L1_PARAM_ERROR;
    req_cnf                     = RFMEAS_DISPATCH_NULL_PTR;
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "Pointer argument for time_const is NULL");
    return req_cnf;
  }
  else
  {
    /* Payload is good and has achieved RFM functionality dispatch. */
    //req_cnf = RFMEAS_DISPATCH_SUCCESSFUL;
    //rfmeas_rfm = rfmeas_mc_get_time_constant( rfmeas_msg_ptr->source,
    //                                          rfmeas_msg_ptr->target,
    //                                          rfmeas_msg_ptr->time_const );
  }

  /*Set the payload result to the result of the above functionality*/
  if( rfmeas_rfm == RFM_MEAS_COMMON_SUCCESS )
  {
    /* RF MC function was successful and therefore send a success response. */
    rfmeas_resp_msg.req_result  =  RFA_RF_MEAS_SUCCESS;
  }

  if( rfmeas_resp_msg.cnf_required )
  {
    /* Assign the iRAT info to confirmation member */
    rfmeas_resp_msg.time_const = rfmeas_msg_ptr->time_const;

    /* Send a confirmation message */
    msgr_send_status = rfmeas_send_msgr_cnf((msgr_hdr_struct_type *)&rfmeas_resp_msg,
                                            RFA_RF_MEAS_GET_TIME_CONSTANT_CNF,
                                            sizeof(rfa_rf_meas_get_time_constant_cnf_s));
  
    if (msgr_send_status != E_SUCCESS)
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RFMEAS iRAT get time constant MSGR Send UNSUCCESSFUL, error_no %d",msgr_send_status);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "RFMEAS iRAT get time constant MSGR Send SUCCESSFUL, status %d",msgr_send_status);
    }
  }
  return(req_cnf);
}



/*===========================================================================

  FUNCTION:  rfmeas_dispatch_script_enter_req

===========================================================================*/
/*! @fn rfmeas_dispatch_script_enter_req( rfa_rf_meas_script_enter_req_s* rfmeas_msg_ptr )
 *  @brief This function dispatches the initialisation of the RF Driver for 
 *  a specific radio transceiver for MEAS.
 *  @param rfmeas_msg_ptr a pointer to the ... structure variable
 *  payload.
 *  @return rfmeas_dispatch_outcome_t enum variable describing the success/
 *  failure of the function.
 */
rfmeas_dispatch_outcome_t rfmeas_dispatch_script_enter_req( rfa_rf_meas_script_enter_req_s* rfmeas_msg_ptr )
{
  /* Initialise Response and Result Variables */
  rfmeas_dispatch_outcome_t     req_cnf           = RFMEAS_DISPATCH_UNSUCCESSFUL;
  rfm_meas_result_type          rfmeas_rfm        = RFM_MEAS_COMMON_FAILURE;
  errno_enum_type               msgr_send_status  = E_FAILURE;
  rfa_rf_meas_script_enter_cnf_s  rfmeas_resp_msg;
  
  /* Verify payload ptr is not NULL */
  if( rfmeas_msg_ptr == NULL )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "Pointer argument into rfmeas_dispatch_script_enter_req is NULL");
    req_cnf = RFMEAS_DISPATCH_NULL_PTR;
    return req_cnf;
  }

  /* Echo REQ message info to CNF message info for functionality usage. */
  rfmeas_resp_msg.hdr           =     rfmeas_msg_ptr->hdr;
  rfmeas_resp_msg.cnf_required  =     rfmeas_msg_ptr->cnf_required;
  rfmeas_resp_msg.cb_handler    =     rfmeas_msg_ptr->cb_handler;
  rfmeas_resp_msg.req_result    =     RFA_RF_MEAS_EXECUTION_ERROR;

  /* Verify payload parameters. */
  if( rfmeas_msg_ptr->rfm_meas_enter_param == NULL )
  {
    /* Payload is bad and will not achieve RFM functionality dispatch. */
    rfmeas_resp_msg.req_result  = RFA_RF_MEAS_L1_PARAM_ERROR;
    req_cnf                     = RFMEAS_DISPATCH_NULL_PTR;
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "Pointer argument for rfm_meas_enter_param is NULL");
    return req_cnf;
  }
  else
  {
    /* Payload is good and has achieved RFM functionality dispatch. */
    req_cnf = RFMEAS_DISPATCH_SUCCESSFUL;
    rfmeas_rfm = rfmeas_mc_enter( rfmeas_msg_ptr->rfm_meas_enter_param, 
                                  rfmeas_msg_ptr->cb_handler );
  }

  /*Set the payload result to the result of the above functionality*/
  if( rfmeas_rfm == RFM_MEAS_COMMON_SUCCESS )
  {
    /* RF MC function was successful and therefore send a success response. */
    rfmeas_resp_msg.req_result  =  RFA_RF_MEAS_SUCCESS;
  }

  if( rfmeas_resp_msg.cnf_required )
  {
    /* Assign the iRAT info to confirmation member */
    rfmeas_resp_msg.rfm_meas_enter_param = rfmeas_msg_ptr->rfm_meas_enter_param;

    /* Send a confirmation message */
    msgr_send_status = rfmeas_send_msgr_cnf((msgr_hdr_struct_type *)&rfmeas_resp_msg,
                                            RFA_RF_MEAS_SCRIPT_ENTER_CNF,
                                            sizeof(rfa_rf_meas_script_enter_cnf_s));
  
    if (msgr_send_status != E_SUCCESS)
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RFMEAS enter MSGR Send UNSUCCESSFUL, error_no %d",msgr_send_status);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "RFMEAS enter MSGR Send SUCCESSFUL, status %d",msgr_send_status);
    }
  }
  return(req_cnf);
}



/*===========================================================================

  FUNCTION:  rfmeas_dispatch_script_build_scripts_req

===========================================================================*/
/*! @fn rfmeas_dispatch_script_build_scripts_req( rfa_rf_meas_script_build_scripts_req_s* rfmeas_msg_ptr )
 *  @brief This function dispatches the initialisation of the RF Driver for 
 *  a specific radio transceiver for MEAS.
 *  @param rfmeas_msg_ptr a pointer to the ... structure variable
 *  payload.
 *  @return rfmeas_dispatch_outcome_t enum variable describing the success/
 *  failure of the function.
 */
rfmeas_dispatch_outcome_t rfmeas_dispatch_script_build_scripts_req( rfa_rf_meas_script_build_scripts_req_s* rfmeas_msg_ptr )
{
  /* Initialise Response and Result Variables */
  rfmeas_dispatch_outcome_t     req_cnf           = RFMEAS_DISPATCH_UNSUCCESSFUL;
  rfm_meas_result_type          rfmeas_rfm        = RFM_MEAS_COMMON_FAILURE;
  errno_enum_type               msgr_send_status  = E_FAILURE;
  rfa_rf_meas_script_build_scripts_cnf_s  rfmeas_resp_msg;
  
  /* Verify payload ptr is not NULL */
  if( rfmeas_msg_ptr == NULL )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "Pointer argument into rfmeas_dispatch_script_build_scripts_req is NULL");
    req_cnf = RFMEAS_DISPATCH_NULL_PTR;
    return req_cnf;
  }

  /* Echo REQ message info to CNF message info for functionality usage. */
  rfmeas_resp_msg.hdr           =     rfmeas_msg_ptr->hdr;
  rfmeas_resp_msg.cnf_required  =     rfmeas_msg_ptr->cnf_required;
  rfmeas_resp_msg.cb_handler    =     rfmeas_msg_ptr->cb_handler;
  rfmeas_resp_msg.req_result    =     RFA_RF_MEAS_EXECUTION_ERROR;

  /* Verify payload parameters. */
  if( rfmeas_msg_ptr->rfm_meas_setup_param == NULL )
  {
    /* Payload is bad and will not achieve RFM functionality dispatch. */
    rfmeas_resp_msg.req_result  = RFA_RF_MEAS_L1_PARAM_ERROR;
    req_cnf                     = RFMEAS_DISPATCH_NULL_PTR;
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "Pointer argument rfm_meas_setup_param is NULL");
    return req_cnf;
  }
  else
  {
    /* Payload is good and has achieved RFM functionality dispatch. */
    req_cnf = RFMEAS_DISPATCH_SUCCESSFUL;
    rfmeas_rfm = rfmeas_build_scripts( rfmeas_msg_ptr->rfm_meas_setup_param, 
                                       rfmeas_msg_ptr->cb_handler );
  }

  /*Set the payload result to the result of the above functionality*/
  if( rfmeas_rfm == RFM_MEAS_COMMON_SUCCESS )
  {
    /* RF MC function was successful and therefore send a success response. */
    rfmeas_resp_msg.req_result  =  RFA_RF_MEAS_SUCCESS;
  }

  if( rfmeas_resp_msg.cnf_required )
  {
    /* Assign the iRAT info to confirmation member */
    rfmeas_resp_msg.rfm_meas_setup_param = rfmeas_msg_ptr->rfm_meas_setup_param;

    /* Send a confirmation message */
    msgr_send_status = rfmeas_send_msgr_cnf((msgr_hdr_struct_type *)&rfmeas_resp_msg,
                                            RFA_RF_MEAS_SCRIPT_BUILD_SCRIPTS_CNF,
                                            sizeof(rfa_rf_meas_script_build_scripts_cnf_s));
  
    if (msgr_send_status != E_SUCCESS)
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RFMEAS build scripts MSGR Send UNSUCCESSFUL, error_no %d",msgr_send_status);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "RFMEAS build scripts MSGR Send SUCCESSFUL, status %d",msgr_send_status);
    }
  }
  return(req_cnf);
}



/*===========================================================================

  FUNCTION:  rfmeas_dispatch_script_exit_req

===========================================================================*/
/*! @fn rfmeas_dispatch_script_exit_req( rfa_rf_meas__req_s* rfmeas_msg_ptr )
 *  @brief This function dispatches the initialisation of the RF Driver for 
 *  a specific radio transceiver for MEAS.
 *  @param rfmeas_msg_ptr a pointer to the ... structure variable
 *  payload.
 *  @return rfmeas_dispatch_outcome_t enum variable describing the success/
 *  failure of the function.
 */
rfmeas_dispatch_outcome_t rfmeas_dispatch_script_exit_req( rfa_rf_meas_script_exit_req_s* rfmeas_msg_ptr )
{
  /* Initialise Response and Result Variables */
  rfmeas_dispatch_outcome_t     req_cnf           = RFMEAS_DISPATCH_UNSUCCESSFUL;
  rfm_meas_result_type          rfmeas_rfm        = RFM_MEAS_COMMON_FAILURE;
  errno_enum_type               msgr_send_status  = E_FAILURE;
  rfa_rf_meas_script_exit_cnf_s   rfmeas_resp_msg;
  
  /* Verify payload ptr is not NULL */
  if( rfmeas_msg_ptr == NULL )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "Pointer argument into rfmeas_dispatch_script_exit_req is NULL");
    req_cnf = RFMEAS_DISPATCH_NULL_PTR;
    return req_cnf;
  }

  /* Echo REQ message info to CNF message info for functionality usage. */
  rfmeas_resp_msg.hdr           =     rfmeas_msg_ptr->hdr;
  rfmeas_resp_msg.cnf_required  =     rfmeas_msg_ptr->cnf_required;
  rfmeas_resp_msg.cb_handler    =     rfmeas_msg_ptr->cb_handler;
  rfmeas_resp_msg.req_result    =     RFA_RF_MEAS_EXECUTION_ERROR;

  /* Verify payload parameters. */
  if( rfmeas_msg_ptr->rfm_meas_exit_param == NULL )
  {
    /* Payload is bad and will not achieve RFM functionality dispatch. */
    rfmeas_resp_msg.req_result  = RFA_RF_MEAS_L1_PARAM_ERROR;
    req_cnf                     = RFMEAS_DISPATCH_NULL_PTR;
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "Pointer argument rfm_meas_exit_param is NULL");
    return req_cnf;
  }
  else
  {
    /* Payload is good and has achieved RFM functionality dispatch. */
    req_cnf = RFMEAS_DISPATCH_SUCCESSFUL;
    rfmeas_rfm = rfmeas_mc_exit( rfmeas_msg_ptr->rfm_meas_exit_param,
                                 rfmeas_msg_ptr->cb_handler );
  }

  /*Set the payload result to the result of the above functionality*/
  if( rfmeas_rfm == RFM_MEAS_COMMON_SUCCESS )
  {
    /* RF MC function was successful and therefore send a success response. */
    rfmeas_resp_msg.req_result  =  RFA_RF_MEAS_SUCCESS;
  }

  if( rfmeas_resp_msg.cnf_required )
  {
    /* Assign the iRAT info to confirmation member */
    rfmeas_resp_msg.rfm_meas_exit_param = rfmeas_msg_ptr->rfm_meas_exit_param;

    /* Send a confirmation message */
    msgr_send_status = rfmeas_send_msgr_cnf((msgr_hdr_struct_type *)&rfmeas_resp_msg,
                                            RFA_RF_MEAS_SCRIPT_EXIT_CNF,
                                            sizeof(rfa_rf_meas_script_exit_cnf_s));
  
    if (msgr_send_status != E_SUCCESS)
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RFMEAS exit MSGR Send UNSUCCESSFUL, error_no %d",msgr_send_status);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "RFMEAS exit MSGR Send SUCCESSFUL, status %d",msgr_send_status);
    }
  }
  return(req_cnf);
}



#ifdef FEATURE_RF_MEAS
/*===========================================================================

  FUNCTION:  rfmeas_msgr_register

===========================================================================*/
/*!
  @fn boolean rfmeas_msgr_register( msgr_client_t *client_id, msgr_id_t id )
  @brief This function is used by RF Task to register for commands sent by the
         L1 task via the message router. This function is called from
         rf_task_msgr_register function which is called from RF Task itself
  @param client_id a pointer to the RF task control block to regsiter
  @param id the RF TASK command signal RF_CMD_Q_SIG flagging the TASK that there
            are commands waiting in the queue
  @return boolean TRUE if all registrations were successful, FALSE otherwise
*/
/*=========================================================================*/
boolean rfmeas_msgr_register( msgr_client_t *client_id, msgr_id_t id )
{
  boolean         rc         = TRUE;
  errno_enum_type ret_status = E_SUCCESS;

  /* Register for the RF MEAS L1 Commands that the RF task is interested in receiving */
  ret_status = msgr_register_block(MSGR_RFA_RF_MEAS,
                                   client_id,
                                   id,
                                   rfmeas_umid_req_list,
                                   RF_MEAS_REQ_CNT);
  if (ret_status != E_SUCCESS)
  {
    MSG_ERROR("rfmeas_msgr_register: Unable to register for RF MEAS L1 message router register block.",0,0,0);
    rc = FALSE;
  }
  return(rc);
}



/*===========================================================================

  FUNCTION:  rfmeas_msgr_deregister

===========================================================================*/
/*!
  @fn boolean rfmeas_msgr_register( msgr_client_t *client_id, msgr_id_t id )
  @brief This function is used by RF Task to de-register commands sent by the
         L1 task via the message router. This function is called from
         rf_task_msgr_register function which is called from RF Task itself
  @param client_id a pointer to the RF task control block to de-regsiter
  @return boolean TRUE if all de-registrations were successful, FALSE otherwise
*/
/*=========================================================================*/
boolean rfmeas_msgr_deregister( msgr_client_t *client_id )
{
  boolean         rc         = TRUE;
  errno_enum_type ret_status = E_SUCCESS;

  /* Deregister all the RF MEAS L1 message RF task has registered for. */
  ret_status = msgr_deregister_block(MSGR_RFA_RF_MEAS,
                                     client_id,
                                     rfmeas_umid_req_list,
                                     RF_MEAS_REQ_CNT);
  if (ret_status != E_SUCCESS)
  {
    MSG_ERROR("rfmeas_msgr_deregister: Unable to deregister RF message router register block.",0,0,0);
    rc = FALSE;
  }
  return(rc);
}



/*===========================================================================

  FUNCTION:  rfmeas_req_dispatcher

===========================================================================*/
/*!
  @brief This is the common command handler for rf meas RFM commands
  @param req_ptr A pointer to a structure containing tcb and msgr details
  @return void
*/
/*=========================================================================*/
void rfmeas_req_dispatcher( rf_cmd_type* req_ptr )
{
  rfmeas_dispatch_outcome_t req_cnf = RFMEAS_DISPATCH_SUCCESSFUL;
  rfmeas_msg_u rfmeas_msgr_req;
  uint8 * cpy_dest;
  void * cpy_src;
  size_t cpy_size;

  if( req_ptr != NULL )
  {
    /*Limit memcpy message copy size to max allowable size*/
    cpy_size = MIN(sizeof(rfmeas_msg_u)-sizeof(rf_msgr_hdr_struct_type), RF_MAX_CMD_PAYLOAD_SIZE);

    cpy_src = (void *) &req_ptr->msgr_hdr;
    cpy_dest = (uint8 *) &rfmeas_msgr_req;

    /*Copy the MSGR request packet header */
    memscpy( (void *)cpy_dest, sizeof(rf_msgr_hdr_struct_type), cpy_src, sizeof(rf_msgr_hdr_struct_type));

    /* Set the destination and source pointers to the payload */
    cpy_src = (void *) &req_ptr->payload;
    cpy_dest += sizeof(rf_msgr_hdr_struct_type);

    /* Copy the payload */
    memscpy( (void *)cpy_dest, cpy_size, cpy_src, cpy_size );

    /*This dispatcher only services subscriber one which is indicated by RFM_DEVICE_0*/
    req_cnf = rfmeas_req_dispatch(&rfmeas_msgr_req);
  	if (req_cnf != RFMEAS_DISPATCH_SUCCESSFUL)
  	{
      MSG_ERROR("MEAS command received from rf command dispatcher. MEAS Command"
                "processor could not process the command. Exited with error code"
                "%d",req_cnf,0,0);
      req_cnf = RFMEAS_DISPATCH_UNSUCCESSFUL;
  	}
  }
}



/*===========================================================================

  FUNCTION:  rfmeas_dispatch_init

===========================================================================*/
/*!
  @fn void rfmeas_dispatch_init(void)
  @brief Regsiters the MEAS module and dispatch handlers with the RF task dispatcher.
         This function is called from rfmeas_mc_init before any RF MEAS L1 command is
         sent to the RF Driver Task.
*/
/*=========================================================================*/
void rfmeas_dispatch_init(void)
{
  /*Register with rf module dispatch table using RF RFA as the module.*/
  if(rf_cmd_dispatch_register_tech( RF_TASK_MAIN, MSGR_TECH_RFA, MSGR_MODULE_RF_MEAS, rfmeas_req_dispatcher) != RF_CMD_REGISTRATION_SUCCESS)
  {
     MSG_ERROR( "rfmeas_dispatch_init:Failed MSGR_TECH_RFA / MSGR_MODULE_RF_MEAS "
               "registration",0,0,0 );
     return;
  }
}



/*===========================================================================

  FUNCTION:  rfmeas_dispatch_deinit

===========================================================================*/
/*!
  @fn void rfmeas_dispatch_deinit(void)
  @brief This funtion has to be called whenever the RF task is killed. Frees allocated
         memory and registration tables.
  @return void
  @TODO this function is not called at the moment but need verify if required because
        rfm_init is only called once. Need to understand if is is possible to create 
        multiple instances of the same module and therefore not deregistering could 
        be a problem if the case where multiple inits occurs. (Shouldn't occur)
*/
/*=========================================================================*/
void rfmeas_dispatch_deinit(void)
{
   /*Clean up the rf command dispatch table which contains all the pointers to 
     individual command handler. The rf module dispatch table is cleaned by 
     rf_req_dispatcher when rf task is killed.
   */
   /*! @TODO the module dispatch de-registration returns a success/failure flag 
             which needs to be handled here.
   */
  rf_cmd_dispatch_deregister_tech( RF_TASK_MAIN, MSGR_TECH_RFA, MSGR_MODULE_RF_MEAS);
}



/*===========================================================================
  FUNCTION:  rfmeas_req_dispatch
===========================================================================*/
/*!
  @fn rfmeas_dispatch_outcome_t rfmeas_req_dispatch(rfmeass_msg_u* msg_ptr)
  @brief This function is the main dispatch function for all the REQs the RF
         task will receive. Once it receives a command, it basically dispatches
         it to the appropriate function again in this file. That function will
         basically send the command to the repective MC function and then once that 
         is executed, it will send back a the reponse. That function will also echo 
         the arguments of the original command.
  @param msg_ptr a pointer to the structure variable of the payload
  @return rfmeas_dispatch_outcome_t 
*/
/*=========================================================================*/
rfmeas_dispatch_outcome_t rfmeas_req_dispatch(rfmeas_msg_u* msg_ptr)
{
  rfmeas_dispatch_outcome_t req_cnf;
  msgr_hdr_s   *msgr_hdr_ptr = (msgr_hdr_s*) msg_ptr; 

  if (msg_ptr != NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "RF MEAS req dispatch, MSG ID=0x%x", msgr_hdr_ptr->id);

    switch ( msgr_hdr_ptr->id )
    {
    case RFA_RF_MEAS_GET_IRAT_INFO_PARAM_REQ:
      req_cnf = rfmeas_dispatch_get_irat_info_param_req((rfa_rf_meas_get_irat_info_param_req_s*) msg_ptr);
      break;
    case RFA_RF_MEAS_GET_TIME_CONSTANT_REQ:
      req_cnf = rfmeas_dispatch_get_time_constant_req((rfa_rf_meas_get_time_constant_req_s*) msg_ptr);
      break;
    case RFA_RF_MEAS_SCRIPT_ENTER_REQ:
      req_cnf = rfmeas_dispatch_script_enter_req((rfa_rf_meas_script_enter_req_s*) msg_ptr);
      break;
    case RFA_RF_MEAS_SCRIPT_BUILD_SCRIPTS_REQ:
      req_cnf = rfmeas_dispatch_script_build_scripts_req((rfa_rf_meas_script_build_scripts_req_s*) msg_ptr);
      break;
    case RFA_RF_MEAS_SCRIPT_EXIT_REQ:
      req_cnf = rfmeas_dispatch_script_exit_req((rfa_rf_meas_script_exit_req_s*) msg_ptr);
      break;
    default :
      MSG_ERROR("Undefined MEAS REQ",0,0,0);
      req_cnf = RFMEAS_DISPATCH_UNSUCCESSFUL;
      break;
    }
  } 
  else
  {
    MSG_ERROR("Pointer NULL",0,0,0);
    req_cnf = RFMEAS_DISPATCH_NULL_PTR;
  }
  return(req_cnf);
}
#endif /*FEATURE_RF_MEAS*/


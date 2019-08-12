/*!
  @file
  	sar_dsi.c

  @brief
        Initializes the specific_absorption_rate service to clients for
        getting the device state index (DSI).
*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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
/*==========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/sar/src/sar_dsi.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/27/14   rj      Cleanup SAR related processing
06/19/14   rj      Added support for CFCM module
02/12/14   rj      Added support for GPIO polarity and BootUp BackOff
10/18/13   rj      Initialize sar_dsi_msg, to avoid wrong initialization of 
                   is_gpio_valid as random value
05/29/13   jk      Added check for the compatibility key in rf_set_state request.
05/20/13   rj      mcs_limitsmgr.conf EFS file optimization
05/10/13   rj      Corrected DSI valid check when DSI is received through QMI
04/05/13   rj      GPIO Based SAR Control added
03/24/13   sk      support for diag command to set SAR DSI
03/07/13   rj      sar_rf_state missing in Qmi_sar_rf_get_state_req_msg response.
01/10/12   rj      Fixed Compilation error
08/14/12   ag      Initial implementation

===========================================================================*/
/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "lmtsmgr_task.h"
#include "lmtsmgr_msgr.h"
#include "mqcsi_conn_mgr.h"
#include "sar_i.h"
#include "specific_absorption_rate_v01.h"
#include "wwan_coex_mgr.h"

/* QMI Files */
#include "qmi_si.h"
#include "qmi_idl_lib.h"
#include "specific_absorption_rate_impl_v01.h"

/* diag command files */
#include "diagcmd.h"

#ifdef FEATURE_MODEM_CFCM_DIAG_TEST
#error code not present
#endif /* FEATURE_MODEM_CFCM_DIAG_TEST */
/*=============================================================================

                       TYEPEDEFS

=============================================================================*/

/* Global SAR GPIO DSI Interface structure */
typedef struct
{
  /* Declare a handle to the TLMM object */
  DalDeviceHandle * lmtsmgr_gpio_hTlmm;
  DALGpioSignalType lmtsmgr_dsi_gpio_config;
  DALGpioValueType  gpio_value;

  /* Timer for GPIO polling */
  timer_type  lmtsmgr_gpio_timer;

  /* Data read from EFS wrt to GPIO */
  sar_gpio_dsi_efs_data gpio_efs;
}sar_gpio_dsi_type;

/* Global SAR DSI Interface structure */
typedef struct
{
  qmi_csi_service_handle     *service_handle;
  qmi_idl_service_object_type service_object;
  cxm_sar_dsi_type            current;
  sar_gpio_dsi_type           sar_gpio_dsi;
} sar_dsi_iface_type;

/*=============================================================================

                         INTERNAL VARIABLES

=============================================================================*/
sar_dsi_iface_type sar_dsi;

/* declaration of diag command handler function */ 
static PACK(void *) sar_diag_set_current_dsi (PACK(void*) req_pkt, uint16 pkt_len);

/* table containing diag commands and their callback function
   this table should be registered to diag services before use */
static const diagpkt_user_table_entry_type limitsmgr_diag_tbl[] =
{
  /* start id, end id, callback function */
  {(word)SAR_SET_DSI,
   (word)SAR_SET_DSI,
   sar_diag_set_current_dsi}

};

#ifdef FEATURE_MODEM_CFCM_DIAG_TEST
#error code not present
#endif /* FEATURE_MODEM_CFCM_DIAG_TEST */
/*=============================================================================

                           FUNCTIONS DEFINITIONS

=============================================================================*/
/*===========================================================================

  FUNCTION:  sar_dsi_map_qmi_to_rf

===========================================================================*/  
/*!
  @brief
		Maps qmi_sar_rf_state_enum_type_v01 to cxm_sar_dsi_type

  @return
    cxm_sar_dsi_type
*/
/*=========================================================================*/

static cxm_sar_dsi_type sar_dsi_map_qmi_to_rf
(
	qmi_sar_rf_state_enum_type_v01 qmi_dsi 
)
{
  cxm_sar_dsi_type dsi = RF_SAR_STATE_MAX;

  /*-----------------------------------------------------------------------*/

  /* switch statement mapping QMI enums to RF enums */
  switch(qmi_dsi)
  {
    case QMI_SAR_RF_STATE_DEFAULT_V01:
      dsi = RF_SAR_STATE_DEFAULT;
      break;

    case QMI_SAR_RF_STATE_1_V01:
      dsi = RF_SAR_STATE_1;
      break;

    case QMI_SAR_RF_STATE_2_V01:
      dsi = RF_SAR_STATE_2;
      break;

    case QMI_SAR_RF_STATE_3_V01:
      dsi = RF_SAR_STATE_3;
      break;

    case QMI_SAR_RF_STATE_4_V01:
      dsi = RF_SAR_STATE_4;
      break;

    case QMI_SAR_RF_STATE_5_V01:
      dsi = RF_SAR_STATE_5;
      break;

    case QMI_SAR_RF_STATE_6_V01:
      dsi = RF_SAR_STATE_6;
      break;

    case QMI_SAR_RF_STATE_7_V01:
      dsi = RF_SAR_STATE_7;
      break;

    case QMI_SAR_RF_STATE_8_V01:
      dsi = RF_SAR_STATE_8;
      break;

    default:
	    /* if we reach this case the input was invalid */
	    LMTSMGR_MSG_0(ERROR, "Invalid dsi type mapping\n");
	    break;
  }

  return dsi;
}

/*===========================================================================

  FUNCTION:  sar_dsi_map_rf_to_qmi

===========================================================================*/  
/*!
  @brief
		Maps cxm_sar_dsi_type to qmi_sar_rf_state_enum_type_v01

  @return
    qmi_sar_rf_state_enum_type_v01
*/
/*=========================================================================*/

static qmi_sar_rf_state_enum_type_v01 sar_dsi_map_rf_to_qmi
(
   cxm_sar_dsi_type dsi 
)
{
  qmi_sar_rf_state_enum_type_v01 qmi_dsi=QMI_SAR_RF_STATE_DEFAULT_V01;

  /*-----------------------------------------------------------------------*/

  /* switch statement mapping RF enums to QMI enums */
  switch(dsi)
  {
    case RF_SAR_STATE_DEFAULT:
      qmi_dsi = QMI_SAR_RF_STATE_DEFAULT_V01;
      break;

    case RF_SAR_STATE_1:
      qmi_dsi = QMI_SAR_RF_STATE_1_V01;
      break;

    case RF_SAR_STATE_2:
      qmi_dsi = QMI_SAR_RF_STATE_2_V01;
      break;

    case RF_SAR_STATE_3:
      qmi_dsi = QMI_SAR_RF_STATE_3_V01;
      break;

    case RF_SAR_STATE_4:
      qmi_dsi = QMI_SAR_RF_STATE_4_V01;
      break;

    case RF_SAR_STATE_5:
      qmi_dsi = QMI_SAR_RF_STATE_5_V01;
      break;

    case RF_SAR_STATE_6:
      qmi_dsi = QMI_SAR_RF_STATE_6_V01;
      break;

    case RF_SAR_STATE_7:
      qmi_dsi = QMI_SAR_RF_STATE_7_V01;
      break;

    case RF_SAR_STATE_8:
      qmi_dsi = QMI_SAR_RF_STATE_8_V01;
      break;

    default:
	    /* if we reach this case the input was invalid */
	    LMTSMGR_MSG_1(ERROR, "Invalid dsi %d returning default\n",dsi);
	    break;
  }

  return qmi_dsi;
}

/*=============================================================================

  FUNCTION:  sar_dsi_process_req

=============================================================================*/
/*!
    @brief
        Callback function called by QCSI infrastructure when a REQ message to
        SAR DSI is received
 
    @note
    	QCSI infrastructure decodes the data before forwarding it to this layer
 
    @return
   	 qmi_csi_cb_error
*/
/*===========================================================================*/

qmi_csi_cb_error sar_dsi_process_req (
  void           *connection_handle,
  qmi_req_handle  req_handle,
  unsigned int    msg_id,
  void           *req_c_struct,
  unsigned int    req_c_struct_len,
  void           *service_cookie
)
{
  qmi_csi_cb_error      req_cb_retval    = QMI_CSI_CB_NO_ERR;
  qmi_response_type_v01 response;
  cxm_sar_dsi_type      old_dsi, new_dsi;
  qmi_sar_rf_state_enum_type_v01 rcvd_qmi_dsi;
  sar_rf_get_state_resp_msg_v01 get_resp;
  sar_rf_get_compatibility_key_resp_msg_v01 get_comp_key;
  uint32 rcvd_qmi_comp_key;
  uint8 rcvd_qmi_validity_field;
  /*-----------------------------------------------------------------------*/

  /* Process message appropriately based on QMI msg_id */
  switch( msg_id )
  {
    /* This request provides the current DSI */
    case QMI_SAR_RF_SET_STATE_REQ_MSG_V01:
      {
        /* Store current dsi locally */
        old_dsi = sar_dsi.current;

        /* Store the new DSI locally */
        rcvd_qmi_dsi = 
          ((sar_rf_set_state_req_msg_v01 *)req_c_struct)->sar_rf_state;

        /*Store the received compatibility key locally*/
        rcvd_qmi_comp_key = 
          ((sar_rf_set_state_req_msg_v01 *)req_c_struct)->compatibility_key;

        /*Store the validity field locally*/
         rcvd_qmi_validity_field = 
          ((sar_rf_set_state_req_msg_v01 *)req_c_struct)->compatibility_key_valid;

         /*Check if we have received a key*/
         if (rcvd_qmi_validity_field)
         {
           /*Check if the received QMI Message has a key, and whether it matches the key*/
           if ((!sar_efs_check_if_key_present()) || (rcvd_qmi_comp_key!=sar_efs_get_comp_key()))
           {
             /*The client passed a invalid key or the key is not present in EFS*/
             /*Send the appropriate error message */
             response.result = QMI_RESULT_FAILURE_V01;
             response.error = QMI_ERR_INVALID_ID_V01;
             /* send request prior to initiating further indications */
             req_cb_retval   = mqcsi_conn_mgr_send_resp_from_cb( 
               connection_handle,
               req_handle,
               QMI_SAR_RF_SET_STATE_RESP_MSG_V01,
               &response,
               sizeof( sar_rf_set_state_resp_msg_v01 )
               );
             if( req_cb_retval == QMI_CSI_CB_NO_ERR )
             {
               LMTSMGR_MSG_3(MED, "Sent resp msg %d res %d err %d for Set RF SAR req msg",
                             msg_id, response.result, response.error);
             }
             else
             {
               LMTSMGR_MSG_3(ERROR, "Could not send Set RF SAR resp msg %d res %d err %d",
                             msg_id, response.result, response.error);
             }
             break;  /*Exit the switch case*/
           }
           else
           {
             LMTSMGR_MSG_1(MED, "Received Comp key is %d",
                             rcvd_qmi_comp_key);
           }
         }
         /* Convert to RF DSI */
         new_dsi = sar_dsi_map_qmi_to_rf(rcvd_qmi_dsi);
         /* Initialize structure holding response */
         memset( &response, 0, sizeof( sar_rf_set_state_resp_msg_v01 ) );

         /* If the DSI is not within limits, respond with failure... */
         if (!IS_SAR_DSI_VALUE_VALID(new_dsi))
         {
           response.result = QMI_RESULT_FAILURE_V01;
           response.error = QMI_ERR_INVALID_ARG_V01;
         }
         else if (sar_dsi.sar_gpio_dsi.gpio_efs.is_gpio_dsi_active)
         {
           /* Apply the new dsi */
           sar_dsi.current = RF_SAR_STATE_4*sar_dsi.sar_gpio_dsi.gpio_value + new_dsi;

           /* Send a success response */
           response.result = QMI_RESULT_SUCCESS_V01;
           response.error = QMI_ERR_NONE_V01;
         }
         else
         {
           /* Apply the new dsi */
           sar_dsi.current = new_dsi;

           /* Send a success response */
           response.result = QMI_RESULT_SUCCESS_V01;
           response.error = QMI_ERR_NONE_V01;
         }

         LMTSMGR_MSG_3(HIGH, "Received new DSI %d, current DSI %d, applied DSI %d",
                       rcvd_qmi_dsi, old_dsi, sar_dsi.current);

         /* send request prior to initiating further indications */
         req_cb_retval   = mqcsi_conn_mgr_send_resp_from_cb( 
           connection_handle,
           req_handle,
           QMI_SAR_RF_SET_STATE_RESP_MSG_V01,
           &response,
           sizeof( sar_rf_set_state_resp_msg_v01 )
           );
         if( req_cb_retval == QMI_CSI_CB_NO_ERR )
         {
           LMTSMGR_MSG_3(MED, "Sent resp msg %d res %d err %d for Set RF SAR req msg",
                         msg_id, response.result, response.error);
         }
         else
         {
           LMTSMGR_MSG_3(ERROR, "Could not send Set RF SAR resp msg %d res %d err %d",
                         msg_id, response.result, response.error);
         }
        
         /* Send messages to interested parties only if the dsi has changed */
         if( sar_dsi.current != old_dsi )
         {
           /* Process the change in dsi */
           lmtsmgr_process_sar_input();

           /* Inform RF about it */
           rfm_common_sar_set_state(sar_dsi.current);
         }
      }
      break;

          /* This request is used to query sar_dsi for the current DSI */
     case QMI_SAR_RF_GET_STATE_REQ_MSG_V01:
     {
        /* Initialize structure holding response */
        memset( &get_resp, 0, sizeof( sar_rf_get_state_resp_msg_v01 ) );

        /* Set the DSI in the response */
        if (!sar_dsi.sar_gpio_dsi.gpio_efs.is_gpio_dsi_active)
        {
          get_resp.sar_rf_state = sar_dsi_map_rf_to_qmi(sar_dsi.current);
        }
        else 
        {
          get_resp.sar_rf_state = sar_dsi_map_rf_to_qmi((sar_dsi.current - (RF_SAR_STATE_4*sar_dsi.sar_gpio_dsi.gpio_value)));
        }
        get_resp.sar_rf_state_valid = TRUE;

        /* send out response */
        req_cb_retval   = mqcsi_conn_mgr_send_resp_from_cb( 
                              connection_handle,
                              req_handle,
                              QMI_SAR_RF_GET_STATE_RESP_MSG_V01,
                              &get_resp,
                              sizeof( sar_rf_get_state_resp_msg_v01 )
                          );

        if( req_cb_retval == QMI_CSI_CB_NO_ERR )
        {
          LMTSMGR_MSG_3(MED, "Sent Get SAR State %d resp msg %d handle %d", 
                    get_resp.sar_rf_state, msg_id, req_handle);
        }
        else
        {
          LMTSMGR_MSG_2(ERROR, "Could not send Get SAR State resp msg %d handle %d",
                    msg_id, req_handle);
        }
      }
      break;
      
      /* This request is used by the client to get the current key */
     case QMI_SAR_GET_COMPATIBILITY_KEY_REQ_MSG_V01:
     {

        /* Initialize structure holding response */
        memset( &get_comp_key, 0, sizeof( sar_rf_get_compatibility_key_resp_msg_v01 ) );

        /* Check if the key is present */
        if (sar_efs_check_if_key_present())
        {
          get_comp_key.compatibility_key=sar_efs_get_comp_key();
          get_comp_key.compatibility_key_valid= 1;

          /* Send a success response */
          get_comp_key.resp.result = QMI_RESULT_SUCCESS_V01;
          get_comp_key.resp.error  = QMI_ERR_NONE_V01;
        }
        else 
        {
          get_comp_key.compatibility_key=0;
          get_comp_key.compatibility_key_valid= 0;

          /* Send a failure response */
          get_comp_key.resp.result = QMI_RESULT_FAILURE_V01;
          get_comp_key.resp.error  = QMI_ERR_NOT_PROVISIONED_V01; 
          LMTSMGR_MSG_0(MED, "Compatibility Key is not present in modem EFS");
        }
        
        /* send out response */
        req_cb_retval   = mqcsi_conn_mgr_send_resp_from_cb( 
          connection_handle,
          req_handle,
          QMI_SAR_GET_COMPATIBILITY_KEY_RESP_MSG_V01,
          &get_comp_key,
          sizeof( sar_rf_get_compatibility_key_resp_msg_v01 )
          );
        if( req_cb_retval == QMI_CSI_CB_NO_ERR )
        {
          LMTSMGR_MSG_0(MED, "Sent Compatibility Key Response to the Client ");
        }
        else
        {
          LMTSMGR_MSG_0(ERROR, "Could not send Compatibility Key Response to the Client ");
        }
     }
     break;
      
      
 

    /* error case */
    default:
      /* Initialize structure holding response */
      memset( &response, 0, sizeof( qmi_response_type_v01 ) );			

      response.result = QMI_RESULT_FAILURE_V01;
      response.error  = QMI_ERR_INVALID_MESSAGE_ID_V01;

      req_cb_retval   = mqcsi_conn_mgr_send_resp_from_cb( 
                          connection_handle,
                          req_handle,
                          msg_id,
                          &response,
                          sizeof( qmi_response_type_v01 )
                        );
      break;
  } /* switch ( msg_id ) */

  /*-----------------------------------------------------------------------*/
  return req_cb_retval;
}

/*=============================================================================

  FUNCTION:  sar_dsi_get_ind_offset

=============================================================================*/
/*!
    @brief
    This method maintains the internal mapping between the indication method
    id and offset of the message in the message map.

    @return
    int32 offset
*/
/*===========================================================================*/
int32 sar_dsi_get_ind_offset 
(
  const uint32   ind_msg_id  /*!< Indication whose internal offset is needed */
)
{
  /*-----------------------------------------------------------------------*/

  return -1;
}

/*=============================================================================

  FUNCTION:  sar_read_current_gpio_value

=============================================================================*/
/*!
    @brief
    	read Current GPIO value 
 
    @return
    	None
*/
/*===========================================================================*/
static void sar_read_current_gpio_value ( timer_cb_data_type data )
{
  /* GPIO pin value for SAR */
  DALGpioValueType	 new_gpio_value = -1, old_gpio_value = -1;
  cxm_sar_set_dsi_s     sar_dsi_msg;

  DALResult nResult = DAL_ERROR;

  NOTUSED( data );

  if (sar_dsi.sar_gpio_dsi.gpio_efs.is_gpio_dsi_active != TRUE)
  {
    LMTSMGR_MSG_1(ERROR, "GPIO DSI is NOT active %d", sar_dsi.sar_gpio_dsi.gpio_efs.is_gpio_dsi_active);
    return;
  }

  // Call the TLMM DAL API to configure the GPIO.
  nResult = DalTlmm_GpioIn( sar_dsi.sar_gpio_dsi.lmtsmgr_gpio_hTlmm, sar_dsi.sar_gpio_dsi.lmtsmgr_dsi_gpio_config, &new_gpio_value );

  old_gpio_value = sar_dsi.sar_gpio_dsi.gpio_value;
  if((DAL_SUCCESS == nResult) && (IS_SAR_DSI_GPIO_VALUE_VALID(new_gpio_value)))
  {
    LMTSMGR_MSG_4(HIGH, "GPIO: Read SUCCESS DSI new_gpio %d, old %d, sar_dsi.current %d, is_gpio_active_low %d", 
                             new_gpio_value, old_gpio_value, sar_dsi.current, sar_dsi.sar_gpio_dsi.gpio_efs.is_gpio_active_low);

    memset(&sar_dsi_msg, 0, sizeof(cxm_sar_set_dsi_s));

    /* Check whether GPIO Active-Low polarity is set --> invert DAL GPIO read value */
    if (sar_dsi.sar_gpio_dsi.gpio_efs.is_gpio_active_low)
    {
      new_gpio_value = SWITCH_SAR_DSI_GPIO_VALUE(new_gpio_value);
    }

	/* Send messages to interested parties only if the GPIO-dsi has changed */
    if( old_gpio_value != new_gpio_value )
    {
      /* populate the msgr structure with new GPIO value */
      sar_dsi_msg.is_gpio_valid = TRUE;
      sar_dsi_msg.gpio_value    = new_gpio_value;

      /* populate the dsi_value with MAX invalid value */
      sar_dsi_msg.dsi_val       = RF_SAR_STATE_MAX;
	  
      /* send message to msgr with the entered dsi value*/
      lmtsmgr_msgr_send_msg(&sar_dsi_msg.msg_hdr,
                             (msgr_umid_type)MCS_CXM_SAR_SET_DSI_IND,
                              sizeof(cxm_sar_set_dsi_s));
    }

  }
  else
  {
    LMTSMGR_MSG_2(HIGH, "GPIO: Read FAILED DSI %d, nResult %d", new_gpio_value, nResult);
  }

  /* Reset the timer */
  timer_set( &(sar_dsi.sar_gpio_dsi.lmtsmgr_gpio_timer), (timetick_type) sar_dsi.sar_gpio_dsi.gpio_efs.polling_time_ms, 0, T_MSEC );
  
}


/*=============================================================================

  FUNCTION:  sar_dsi_service_init

=============================================================================*/
/*!
    @brief
    	Initializes SAR DSI layer's functionalities
 
    @return
    	None
*/
/*===========================================================================*/
void sar_dsi_service_init 
(
  void
)
{
  DALResult nResult = DAL_ERROR;

  /* Setup SAR DSI's QCSI service interface */

  /* 1. Set the current DSI to default */
  sar_dsi.current = RF_SAR_STATE_DEFAULT;

  /* 2. Setup service object */
  sar_dsi.service_object = sar_get_service_object_v01();

  /* 3. Register object with QMI */
  (void) qmi_si_register_object ( sar_dsi.service_object,
                                  0, /* Service Instance */
                                  sar_get_service_impl_v01() );

  /* 4. Register/Open/Initiate service with Connection Manager infrastructure */
  sar_dsi.service_handle = mqcsi_conn_mgr_service_open (
                                               MQCSI_DSI_SERVICE_ID,
                                               "DSI_SERVICE",
                                               &lmtsmgr_tcb,
                                               LMTSMGR_SAR_QMI_SIG,
                                               sar_dsi.service_object, 
                                               sar_dsi_process_req,
                                               sar_dsi_get_ind_offset,
                                               NULL,
                                               SAR_V01_IDL_MAJOR_VERS,
                                               SAR_V01_IDL_MINOR_VERS );
  LMTSMGR_ASSERT( NULL != sar_dsi.service_handle );

  /* register the limitsmgr diag commands table to the diag command service */
   DIAGPKT_DISPATCH_TABLE_REGISTER ((diagpkt_subsys_id_type)DIAG_SUBSYS_LIMITSMGR,
                                   limitsmgr_diag_tbl);

  /*-----------------------------------------------------------------------*/

  /* Default values for GPIO DSI EFS data */
  memset(&(sar_dsi.sar_gpio_dsi.gpio_efs), 0, sizeof(sar_gpio_dsi_efs_data));

  /* Read GPIO based DSI value */
  sar_dsi.sar_gpio_dsi.gpio_efs = sar_read_gpio_dsi_efs_data(LIMITSMGR_SAR_DSI_GPIO_EFS_FILE);
  
  if (sar_dsi.sar_gpio_dsi.gpio_efs.is_gpio_dsi_active == TRUE)
  {
    /* initialized GPIO to LOW and polarity doesn't matter here since 
            internally we are always keeping DAL GPIO values as Active HIGH */
    sar_dsi.sar_gpio_dsi.gpio_value = DAL_GPIO_LOW_VALUE;

    // Call upon the DAL to retrieve a handle to the DALTLMM
    nResult = DAL_DeviceAttach(DALDEVICEID_TLMM, &(sar_dsi.sar_gpio_dsi.lmtsmgr_gpio_hTlmm));
  
    if( DAL_ERROR == nResult ||
        sar_dsi.sar_gpio_dsi.lmtsmgr_gpio_hTlmm == NULL )
    {
      // This is a fatal error.
      ERR_FATAL("Failed to acquire a handle to TLMM.",0,0,0);
    }
  
    // create a GPIO configuration
    sar_dsi.sar_gpio_dsi.lmtsmgr_dsi_gpio_config = DAL_GPIO_CFG(sar_dsi.sar_gpio_dsi.gpio_efs.gpio_num,
                                                                0,
                                                                DAL_GPIO_INPUT,
                                                                DAL_GPIO_NO_PULL,
                                                                DAL_GPIO_2MA);
  
    nResult = DAL_ERROR;
    nResult = DalTlmm_ConfigGpio( sar_dsi.sar_gpio_dsi.lmtsmgr_gpio_hTlmm, 
                              sar_dsi.sar_gpio_dsi.lmtsmgr_dsi_gpio_config, DAL_TLMM_GPIO_ENABLE );

    if (nResult == DAL_SUCCESS)
    {
      LMTSMGR_MSG_1(HIGH, "gpio for DSI configured successfully %d", nResult);

    /* Initialize timers */
    timer_def(&(sar_dsi.sar_gpio_dsi.lmtsmgr_gpio_timer), NULL, NULL, 0,
                                  sar_read_current_gpio_value, 0);

    /* Set/start the timer */
    timer_set( &(sar_dsi.sar_gpio_dsi.lmtsmgr_gpio_timer), 
               (timetick_type) sar_dsi.sar_gpio_dsi.gpio_efs.polling_time_ms, 0, T_MSEC );

    }
    else
    {
      LMTSMGR_MSG_1(ERROR, "gpio for DSI not configured successfully %d", nResult);
    }

    if (sar_dsi.sar_gpio_dsi.gpio_efs.is_init_backoff == TRUE)
    {
      /* Inform RF about Backoff during Bootup */
      rfm_common_sar_set_state(RF_SAR_STATE_4);
      /* Update current DSI value */
      sar_dsi.current = RF_SAR_STATE_4;
      /* If Tech is Backoff during intialization, we should have GPIO value as HIGH */
      sar_dsi.sar_gpio_dsi.gpio_value = DAL_GPIO_HIGH_VALUE;
    }

  }

/*-----------------------------------------------------------------------*/

  return;
}

/*=============================================================================

  FUNCTION:  sar_get_current_dsi

=============================================================================*/
/*!
    @brief
    	Returns the current DSI
 
    @return
    	None
*/
/*===========================================================================*/
cxm_sar_dsi_type sar_get_current_dsi ( void )
{
  return sar_dsi.current;
}

/*=============================================================================

  FUNCTION:   sar_diag_set_current_dsi

=============================================================================*/
/*!
    @brief
        Callback function that gets called when the diag command to set
        the dsi value is entered.
        It then sends the received dsi value to limitsmgr task via msgr
 
    @return
    	None
*/
/*===========================================================================*/

static PACK(void *) sar_diag_set_current_dsi
(
  PACK(void*) req_pkt,  /* request packet */
  uint16 pkt_len        /* request packet length */
)
{

  /* response pkt */
  DIAG_SUBSYS_LIMITSMGR_SAR_SET_DSI_rsp_type       *rsp_ptr;
  cxm_sar_dsi_type                                 new_dsi;
  cxm_sar_set_dsi_s                                sar_dsi_msg;
  /*-----------------------------------------------------------------------*/

  /* Check if the request is valid */
  if(pkt_len != sizeof(DIAG_SUBSYS_LIMITSMGR_SAR_SET_DSI_req_type))
  {
    LMTSMGR_MSG_0(ERROR, "Invalid diag request to set DSI value (%d)");
    return(NULL);
  }

  memset(&sar_dsi_msg, 0, sizeof(cxm_sar_set_dsi_s));

  /* get the new dsi value from Diag packet */
  new_dsi = (cxm_sar_dsi_type) (((DIAG_SUBSYS_LIMITSMGR_SAR_SET_DSI_req_type *)req_pkt)->dsi_val);

#ifdef FEATURE_MODEM_CFCM_DIAG_TEST
  #error code not present
#endif /* FEATURE_MODEM_CFCM_DIAG_TEST */
    if (new_dsi >= RF_SAR_STATE_MAX)
    {
      /* invalid value received */
      LMTSMGR_MSG_2(ERROR, "Invalid DSI: Expected less than: %d Entered: %d ",
                          RF_SAR_STATE_MAX, new_dsi);
    }
    else if( sar_dsi.current != new_dsi )
    {
      /* populate the msgr structure with new_dsi */
      sar_dsi_msg.dsi_val = new_dsi;
      /* send message to msgr with the entered dsi value*/
      lmtsmgr_msgr_send_msg(&sar_dsi_msg.msg_hdr,
  						(msgr_umid_type)MCS_CXM_SAR_SET_DSI_IND,
  						sizeof(cxm_sar_set_dsi_s));
    }
#ifdef FEATURE_MODEM_CFCM_DIAG_TEST
  #error code not present
#endif /* FEATURE_MODEM_CFCM_DIAG_TEST */

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_LIMITSMGR_SAR_SET_DSI_rsp_type *)
            diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_LIMITSMGR,
            (diagpkt_subsys_cmd_code_type)SAR_SET_DSI,
            sizeof(DIAG_SUBSYS_LIMITSMGR_SAR_SET_DSI_rsp_type));

  return(rsp_ptr);

} /* sar_diag_set_current_dsi */

/*=============================================================================

  FUNCTION:  sar_handle_set_dsi_ind

=============================================================================*/
/*!
    @brief
    Sets the current dsi value using value obtained from MSGR msg
 
    @return
    None
*/
/*===========================================================================*/
void sar_handle_set_dsi_ind
(
  void *sar_rcvd_msg /* struct we have recieved and are updating for */
)
{
  /* void recieved message will be cast to this */
  cxm_sar_set_dsi_s   *sar_dsi_msg = NULL;

  /* this will hold the new value of DSI, for quick access */
  cxm_sar_dsi_type              new_dsi;

  /* if pointer is null, print error message do nothing */
  if(sar_rcvd_msg == NULL)  
  {
	LMTSMGR_MSG_0(ERROR, "Set DSI value was passed a NULL pointer\n");
	return;
  }

  /*-----------------------------------------------------------------------*/

  /* retrieve the dsi value from msg received */
  sar_dsi_msg = (cxm_sar_set_dsi_s *) sar_rcvd_msg;
  /* update new DSI value base don GPIO valid flag */
  if (sar_dsi_msg->is_gpio_valid)
  {
    new_dsi = RF_SAR_STATE_4*sar_dsi_msg->gpio_value+ (sar_dsi.current - (RF_SAR_STATE_4*sar_dsi.sar_gpio_dsi.gpio_value));
    sar_dsi.sar_gpio_dsi.gpio_value = sar_dsi_msg->gpio_value;
  }
  else
  {
    new_dsi = sar_dsi_msg->dsi_val;
  }

  LMTSMGR_MSG_2(HIGH, "Current dsi: %d new value: %d",
                      sar_dsi.current, new_dsi);

  /* check the limits just to make sure data didnt get corrupted */
  if((new_dsi != sar_dsi.current) && IS_SAR_DSI_VALUE_VALID(new_dsi))
  {
    /* update the current dsi with new value */
    sar_dsi.current = new_dsi;
    /* Process the change in dsi */
    lmtsmgr_process_sar_input();
    /* Inform RF about it */
    rfm_common_sar_set_state(sar_dsi.current);
  }

}

/*=============================================================================

  FUNCTION:  sar_dsi_service_deinit

=============================================================================*/
/*!
    @brief
        De-Initializes SAR DSI layer's functionalities
 
    @return
        None
*/
/*===========================================================================*/
void sar_dsi_service_deinit 
(
  void
)
{
  /* Turn off the service */
  mqcsi_conn_mgr_service_close(MQCSI_DSI_SERVICE_ID);
}

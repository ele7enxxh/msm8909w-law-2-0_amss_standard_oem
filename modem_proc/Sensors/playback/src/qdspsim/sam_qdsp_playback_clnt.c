/******************************************************************************
  @file    sns_qdspsim_playback_clnt.c
  @brief   SAM client for QDSP SIM Playback


  ---------------------------------------------------------------------------
  Copyright (c) 2013, 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
 *******************************************************************************/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-03-26  ps   Updated license header
  2013-06-11  ae   Initial version

============================================================================*/

#include "comdef.h"
#include "msg.h"
#include "qurt_anysignal.h"
#include "qmi_client.h"
#include "qmi_idl_lib.h"
#include "qmi_cci_target.h"
#include "qmi_cci_common.h"
#include "sns_sam_amd_v01.h"
#include "sns_sam_qmd_v01.h"
#include "sns_sam_rmd_v01.h"
#include "sns_sam_bte_v01.h"
#include "sns_sam_basic_gestures_v01.h"
#include "sns_sam_facing_v01.h"
#include "sns_sam_filtered_mag_v01.h"
#include "sns_sam_fns_v01.h"
#include "sns_sam_tap_v01.h"
#include "sns_sam_game_rotation_vector_v01.h"
#include "sns_sam_gravity_vector_v01.h"
#include "sns_sam_gyrobuf_v01.h"
#include "sns_sam_gyroint_v01.h"
#include "sns_sam_gyro_tap2_v01.h"
#include "sns_sam_integ_angle_v01.h"
#include "sns_sam_mag_cal_v01.h"
#include "sns_sam_orientation_v01.h"
#include "sns_sam_pam_v01.h"
#include "sns_sam_ped_v01.h"
#include "sns_sam_quaternion_v01.h"
#include "sns_sam_rotation_vector_v01.h"
#include "sns_sam_smd_v01.h"
#include "sns_sam_cmc_v01.h"
#include "sns_sam_ped_v01.h"

#include "qurt.h"
#include "qurt_sclk.h"
#include "sam_qdsp_playback_clnt.h"



#define QMI_CLNT_WAIT_SIG        0x00010000
#define QMI_CLNT_TIMER_SIG       0x00000001
#define QMI_CLNT_IND_DONE_SIG    0x00000002
#define QMI_CLNT_ASYNC_DONE_SIG  0x00000004


extern void  qmi_fw_init(void);

uint8 algo_instance_id;
/*=========================================================================
  FUNCTION: sam_ind_cb
=========================================================================*/
/*!
    @brief
    Callback function registered for QMI Indication messages.
*/
/*=======================================================================*/
void sam_ind_cb (qmi_client_type    user_handle,
                 unsigned int       msg_id,
                 void               *ind_buf,
                 unsigned int       ind_buf_len,
                 void               *ind_cb_data)
{
    switch (msg_id)
    {
    case SNS_QDSP_SIM_SAM_REPORT_IND:
      {
        printf("\n Algo report IND recieved");
      }
      break;

    case SNS_QDSP_SIM_SAM_ERROR_IND:
      {
        printf("\n Algo error IND recieved");
      }
      break;
    default:
      break;
    }
}





static void sam_rx_cb (qmi_client_type   user_handle,
                       unsigned int      msg_id,
                       void             *buf,
                       unsigned int      len,
                       void             *resp_cb_data,
                       qmi_client_error_type  transp_err)
{
  /* Print the appropriate message based on the message ID */
    switch (msg_id)
    {
     /* All algo enable resp msg id's are defined to be the same value 0x0002 */
      case SNS_QDSP_SIM_SAM_ENABLE_RESP:
       {
         sns_common_resp_s_v01 *resp_msg = (sns_common_resp_s_v01*)buf;

         printf("\n SAM Algo Enable Resp: Result: %d   Err: %d\n", 
                resp_msg->sns_result_t, resp_msg->sns_err_t);
		 if(((sam_qdsp_common_resp_msg*)(buf))->instance_id_valid)
		 {
			algo_instance_id = ((sam_qdsp_common_resp_msg*)(buf))->instance_id;
		 }
       }
       break;
     default:
         printf("\n sam_rx_cb unhandled message recieved\n");
      break;
   }   
}


static void sam_qmd_rx_cb (qmi_client_type   user_handle,
                           unsigned int      msg_id,
                           void             *buf,
                           unsigned int      len,
                           void             *resp_cb_data,
                           qmi_client_error_type  transp_err)
{
  /* Print the appropriate message based on the message ID */
    switch (msg_id)
    {
     /* All algo enable resp msg id's are defined to be the same value 0x0002 */
      case SNS_SAM_AMD_ENABLE_RESP_V01:
       {
          sns_sam_qmd_enable_resp_msg_v01 *resp_msg = (sns_sam_qmd_enable_resp_msg_v01*)buf;

          printf("\n SAM QMD EN RESP: Result: %d   Err: %d", 
                 resp_msg->resp.sns_result_t, resp_msg->resp.sns_err_t);
		  if(!(resp_msg->resp.sns_err_t))
	      {
		    algo_instance_id = resp_msg->instance_id;
	      }
       }
	   break;
     default:
         printf("\n sam_qmd_rx_cb unhandled message recieved\n");
      break;
   }   
}


/*=========================================================================
  FUNCTION: sns_plbk_sam_amd_enable
=========================================================================*/
/*!
    @brief
    Enable's a SAM AMD Algorithm by sending enable message.
*/
/*=======================================================================*/
int sns_plbk_sam_amd_enable(int *algo_config, 
                            qmi_client_type *clnt)
{
    qmi_txn_handle txn;
    qmi_client_type notifier;
    unsigned int num_services, num_entries=10;
    qmi_cci_os_signal_type os_params;
    qmi_service_info service_info;
    qmi_idl_service_object_type service_object = 
                                      SNS_SAM_AMD_SVC_get_service_object_v01();
    sns_sam_qmd_enable_req_msg_v01 *data_req;
    sns_sam_qmd_enable_resp_msg_v01 *data_resp;
    int rc;

    if (!service_object) {
        printf("AMD CLNT: get_serivce_object failed\n");
    }

    memset(&os_params,0,sizeof(os_params));
    os_params.ext_signal = NULL;
    os_params.sig = QMI_CLNT_WAIT_SIG;
    os_params.timer_sig = QMI_CLNT_TIMER_SIG;

    rc = qmi_client_notifier_init(service_object, &os_params, &notifier);
    
    QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
    QMI_CCI_OS_SIGNAL_CLEAR(&os_params);
    
    /* The server has come up, store the information in info variable */
    rc = qmi_client_get_service_list(service_object, &service_info,
                                     &num_entries, &num_services);

    printf("AMD CLNT: qmi_client_get_service_list() returned %d num_entries=%d"
           " num_services = %d\n", rc, num_entries, num_services);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    rc = qmi_client_init(&service_info, service_object, sam_ind_cb, NULL, 
                         &os_params, clnt);
    printf("AMD CLNT: qmi_client_init returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    /* Prepare and send the request */
    data_req = (sns_sam_qmd_enable_req_msg_v01*)
                                MALLOC(sizeof(sns_sam_qmd_enable_req_msg_v01));
    data_resp = (sns_sam_qmd_enable_resp_msg_v01*)
                               MALLOC(sizeof(sns_sam_qmd_enable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }

    memset( data_req, 0, sizeof(sns_sam_qmd_enable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_qmd_enable_resp_msg_v01) );

    /* sam AMD config */
    data_req->report_period = algo_config[0];
    data_req->config_valid = FALSE;
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_AMD_ENABLE_REQ_V01, data_req,
              sizeof(sns_sam_qmd_enable_req_msg_v01), data_resp,
              sizeof(sns_sam_qmd_enable_resp_msg_v01), sam_qmd_rx_cb, NULL, &txn);
    printf("AMD CLNT: qmi_client_send_msg_async returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}

/*=========================================================================
  FUNCTION: sns_plbk_sam_amd_disable
=========================================================================*/
/*!
    @brief
    Disabble's a SAM AMD Algorithm by sending disable message.
*/
/*=======================================================================*/
int sns_plbk_sam_amd_disable(qmi_client_type *clnt)
{
    sns_sam_qmd_disable_req_msg_v01 *data_req;
    sns_sam_qmd_disable_resp_msg_v01 *data_resp;
    qmi_txn_handle txn;
    int rc;

    /* Prepare and send the request */
    data_req = (sns_sam_qmd_disable_req_msg_v01*)
                               MALLOC(sizeof(sns_sam_qmd_disable_req_msg_v01));
    data_resp = (sns_sam_qmd_disable_resp_msg_v01*)
                              MALLOC(sizeof(sns_sam_qmd_disable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }
    memset( data_req, 0, sizeof(sns_sam_qmd_disable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_qmd_disable_resp_msg_v01) );
  
    data_req->instance_id = algo_instance_id;

    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_AMD_DISABLE_REQ_V01, data_req,
             sizeof(sns_sam_qmd_disable_req_msg_v01), data_resp,
             sizeof(sns_sam_qmd_disable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    if (rc != QMI_NO_ERR) {
        return -1;
    }
    return 0;
}

/*=========================================================================
  FUNCTION: sns_plbk_sam_rmd_enable
=========================================================================*/
/*!
    @brief
    Enable's a SAM RMD Algorithm by sending enable message.
*/
/*=======================================================================*/
int sns_plbk_sam_rmd_enable(int *algo_config, 
                            qmi_client_type *clnt)
{
    qmi_txn_handle txn;
    qmi_client_type notifier;
    unsigned int num_services, num_entries=10;
    qmi_cci_os_signal_type os_params;
    qmi_service_info service_info;
    qmi_idl_service_object_type service_object = 
                                      SNS_SAM_RMD_SVC_get_service_object_v01();
    sns_sam_qmd_enable_req_msg_v01 *data_req;
    sns_sam_qmd_enable_resp_msg_v01 *data_resp;
    int rc;


    if (!service_object) {
        printf("RMD CLNT: get_serivce_object failed\n");
    }

    memset(&os_params,0,sizeof(os_params));
    os_params.ext_signal = NULL;
    os_params.sig = QMI_CLNT_WAIT_SIG;
    os_params.timer_sig = QMI_CLNT_TIMER_SIG;

    rc = qmi_client_notifier_init(service_object, &os_params, &notifier);
    
    QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
    QMI_CCI_OS_SIGNAL_CLEAR(&os_params);
    
    /* The server has come up, store the information in info variable */
    rc = qmi_client_get_service_list(service_object, &service_info,
                                     &num_entries, &num_services);

    printf("RMD CLNT: qmi_client_get_service_list() returned %d "
           "num_entries=%d num_services=%d\n", rc, num_entries, num_services);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    rc = qmi_client_init(&service_info, service_object, sam_ind_cb,
                         NULL, &os_params, clnt);
    printf("RMD CLNT: qmi_client_init returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    /* Prepare and send the request */
    data_req = (sns_sam_qmd_enable_req_msg_v01*)
                                MALLOC(sizeof(sns_sam_qmd_enable_req_msg_v01));
    data_resp = (sns_sam_qmd_enable_resp_msg_v01*)
                               MALLOC(sizeof(sns_sam_qmd_enable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }
    memset( data_req, 0, sizeof(sns_sam_qmd_enable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_qmd_enable_resp_msg_v01) );

    /* sam RMD config */
    data_req->report_period = algo_config[0];
    data_req->config_valid = FALSE;
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_RMD_ENABLE_REQ_V01, data_req,
               sizeof(sns_sam_qmd_enable_req_msg_v01), data_resp, 
               sizeof(sns_sam_qmd_enable_resp_msg_v01), sam_qmd_rx_cb, NULL, &txn);
    printf("RMD CLNT: qmi_client_send_msg_async returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}


/*=========================================================================
  FUNCTION: sns_plbk_sam_rmd_disable
=========================================================================*/
/*!
    @brief
    Disabble's a SAM RMD Algorithm by sending disable message.
*/
/*=======================================================================*/
int sns_plbk_sam_rmd_disable(qmi_client_type *clnt)
{
    sns_sam_qmd_disable_req_msg_v01 *data_req;
    sns_sam_qmd_disable_resp_msg_v01 *data_resp;
    qmi_txn_handle txn;
    int rc;

    /* Prepare and send the request */
    data_req = (sns_sam_qmd_disable_req_msg_v01*)
                               MALLOC(sizeof(sns_sam_qmd_disable_req_msg_v01));
    data_resp = (sns_sam_qmd_disable_resp_msg_v01*)
                              MALLOC(sizeof(sns_sam_qmd_disable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }
    memset( data_req, 0, sizeof(sns_sam_qmd_disable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_qmd_disable_resp_msg_v01) );

    data_req->instance_id = algo_instance_id;
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_RMD_DISABLE_REQ_V01, data_req,
               sizeof(sns_sam_qmd_disable_req_msg_v01), data_resp,
               sizeof(sns_sam_qmd_disable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}

/*=========================================================================
  FUNCTION: sns_plbk_sam_fns_enable
=========================================================================*/
/*!
    @brief
    Enable's a SAM FNS Algorithm by sending enable message.
*/
/*=======================================================================*/
int sns_plbk_sam_fns_enable(int *algo_config, 
                            qmi_client_type *clnt)
{
    qmi_txn_handle txn;
    qmi_client_type notifier;
    unsigned int num_services, num_entries=10;
    qmi_cci_os_signal_type os_params;
    qmi_service_info service_info;
    qmi_idl_service_object_type service_object = 
                                      SNS_SAM_FNS_SVC_get_service_object_v01();
    sns_sam_fns_enable_req_msg_v01 *data_req;
    sns_sam_fns_enable_resp_msg_v01 *data_resp;
    int rc;

    if (!service_object) {
        printf("FNS CLNT: get_serivce_object failed\n");
    }

    memset(&os_params,0,sizeof(os_params));
    os_params.ext_signal = NULL;
    os_params.sig = QMI_CLNT_WAIT_SIG;
    os_params.timer_sig = QMI_CLNT_TIMER_SIG;

    rc = qmi_client_notifier_init(service_object, &os_params, &notifier);
    
    QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
    QMI_CCI_OS_SIGNAL_CLEAR(&os_params);
    
    /* The server has come up, store the information in info variable */
    rc = qmi_client_get_service_list(service_object, &service_info,
                                     &num_entries, &num_services);

    printf("FNS CLNT: qmi_client_get_service_list() returned %d num_entries=%d"
           " num_services = %d\n", rc, num_entries, num_services);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    rc = qmi_client_init(&service_info, service_object, sam_ind_cb, NULL, 
                         &os_params, clnt);
    printf("FNS CLNT: qmi_client_init returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    /* Prepare and send the request */
    data_req = (sns_sam_fns_enable_req_msg_v01*)
                                MALLOC(sizeof(sns_sam_fns_enable_req_msg_v01));
    data_resp = (sns_sam_fns_enable_resp_msg_v01*)
                               MALLOC(sizeof(sns_sam_fns_enable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }

    memset( data_req, 0, sizeof(sns_sam_fns_enable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_fns_enable_resp_msg_v01) );

    /* sam FNS config */
    // NO Mandatory config
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_FNS_ENABLE_REQ_V01, data_req,
              sizeof(sns_sam_fns_enable_req_msg_v01), data_resp,
              sizeof(sns_sam_fns_enable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    printf("FNS CLNT: qmi_client_send_msg_async returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}

/*=========================================================================
  FUNCTION: sns_plbk_sam_fns_disable
=========================================================================*/
/*!
    @brief
    Disabble's a SAM FNS Algorithm by sending disable message.
*/
/*=======================================================================*/
int sns_plbk_sam_fns_disable(qmi_client_type *clnt)
{
    sns_sam_fns_disable_req_msg_v01 *data_req;
    sns_sam_fns_disable_resp_msg_v01 *data_resp;
    qmi_txn_handle txn;
    int rc;

    /* Prepare and send the request */
    data_req = (sns_sam_fns_disable_req_msg_v01*)
                               MALLOC(sizeof(sns_sam_fns_disable_req_msg_v01));
    data_resp = (sns_sam_fns_disable_resp_msg_v01*)
                              MALLOC(sizeof(sns_sam_fns_disable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }
    memset( data_req, 0, sizeof(sns_sam_fns_disable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_fns_disable_resp_msg_v01) );

    data_req->instance_id = algo_instance_id;
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_FNS_DISABLE_REQ_V01, data_req,
               sizeof(sns_sam_fns_disable_req_msg_v01), data_resp,
               sizeof(sns_sam_fns_disable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}


/*=========================================================================
  FUNCTION: sns_plbk_sam_bte_enable
=========================================================================*/
/*!
    @brief
    Enable's a SAM BTE Algorithm by sending enable message.
*/
/*=======================================================================*/
int sns_plbk_sam_bte_enable(int *algo_config, 
                            qmi_client_type *clnt)
{
    qmi_txn_handle txn;
    qmi_client_type notifier;
    unsigned int num_services, num_entries=10;
    qmi_cci_os_signal_type os_params;
    qmi_service_info service_info;
    qmi_idl_service_object_type service_object = 
                                      SNS_SAM_BTE_SVC_get_service_object_v01();
    sns_sam_bte_enable_req_msg_v01 *data_req;
    sns_sam_bte_enable_resp_msg_v01 *data_resp;
    int rc;

    if (!service_object) {
        printf("BTE CLNT: get_serivce_object failed\n");
    }

    memset(&os_params,0,sizeof(os_params));
    os_params.ext_signal = NULL;
    os_params.sig = QMI_CLNT_WAIT_SIG;
    os_params.timer_sig = QMI_CLNT_TIMER_SIG;

    rc = qmi_client_notifier_init(service_object, &os_params, &notifier);
    
    QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
    QMI_CCI_OS_SIGNAL_CLEAR(&os_params);
    
    /* The server has come up, store the information in info variable */
    rc = qmi_client_get_service_list(service_object, &service_info,
                                     &num_entries, &num_services);

    printf("BTE CLNT: qmi_client_get_service_list() returned %d num_entries=%d"
           " num_services = %d\n", rc, num_entries, num_services);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    rc = qmi_client_init(&service_info, service_object, sam_ind_cb, NULL, 
                         &os_params, clnt);
    printf("BTE CLNT: qmi_client_init returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    /* Prepare and send the request */
    data_req = (sns_sam_bte_enable_req_msg_v01*)
                                MALLOC(sizeof(sns_sam_bte_enable_req_msg_v01));
    data_resp = (sns_sam_bte_enable_resp_msg_v01*)
                               MALLOC(sizeof(sns_sam_bte_enable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }

    memset( data_req, 0, sizeof(sns_sam_bte_enable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_bte_enable_resp_msg_v01) );

    /* sam BTE config */
    // NO Mandatory config
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_BTE_ENABLE_REQ_V01, data_req,
              sizeof(sns_sam_bte_enable_req_msg_v01), data_resp,
              sizeof(sns_sam_bte_enable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    printf("BTE CLNT: qmi_client_send_msg_async returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}


/*=========================================================================
  FUNCTION: sns_plbk_sam_bte_disable
=========================================================================*/
/*!
    @brief
    Disabble's a SAM BTE Algorithm by sending disable message.
*/
/*=======================================================================*/
int sns_plbk_sam_bte_disable(qmi_client_type *clnt)
{
    sns_sam_bte_disable_req_msg_v01 *data_req;
    sns_sam_bte_disable_resp_msg_v01 *data_resp;
    qmi_txn_handle txn;
    int rc;

    /* Prepare and send the request */
    data_req = (sns_sam_bte_disable_req_msg_v01*)
                               MALLOC(sizeof(sns_sam_bte_disable_req_msg_v01));
    data_resp = (sns_sam_bte_disable_resp_msg_v01*)
                              MALLOC(sizeof(sns_sam_bte_disable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }
    memset( data_req, 0, sizeof(sns_sam_bte_disable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_bte_disable_resp_msg_v01) );

    data_req->instance_id = algo_instance_id;
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_BTE_DISABLE_REQ_V01, data_req,
               sizeof(sns_sam_bte_disable_req_msg_v01), data_resp,
               sizeof(sns_sam_bte_disable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}

/*=========================================================================
  FUNCTION: sns_plbk_sam_mag_cal_enable
=========================================================================*/
/*!
    @brief
    Enable's a SAM MAG_CAL Algorithm by sending enable message.
*/
/*=======================================================================*/
int sns_plbk_sam_mag_cal_enable(int *algo_config, 
                            qmi_client_type *clnt)
{
    qmi_txn_handle txn;
    qmi_client_type notifier;
    unsigned int num_services, num_entries=10;
    qmi_cci_os_signal_type os_params;
    qmi_service_info service_info;
    qmi_idl_service_object_type service_object = 
                                      SNS_SAM_MAG_CAL_SVC_get_service_object_v01();
    sns_sam_mag_cal_enable_req_msg_v01 *data_req;
    sns_sam_mag_cal_enable_resp_msg_v01 *data_resp;
    int rc;

    if (!service_object) {
        printf("MAG_CAL CLNT: get_serivce_object failed\n");
    }

    memset(&os_params,0,sizeof(os_params));
    os_params.ext_signal = NULL;
    os_params.sig = QMI_CLNT_WAIT_SIG;
    os_params.timer_sig = QMI_CLNT_TIMER_SIG;

    rc = qmi_client_notifier_init(service_object, &os_params, &notifier);
    
    QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
    QMI_CCI_OS_SIGNAL_CLEAR(&os_params);
    
    /* The server has come up, store the information in info variable */
    rc = qmi_client_get_service_list(service_object, &service_info,
                                     &num_entries, &num_services);

    printf("MAG_CAL CLNT: qmi_client_get_service_list() returned %d num_entries=%d"
           " num_services = %d\n", rc, num_entries, num_services);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    rc = qmi_client_init(&service_info, service_object, sam_ind_cb, NULL, 
                         &os_params, clnt);
    printf("MAG_CAL CLNT: qmi_client_init returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    /* Prepare and send the request */
    data_req = (sns_sam_mag_cal_enable_req_msg_v01*)
                                MALLOC(sizeof(sns_sam_mag_cal_enable_req_msg_v01));
    data_resp = (sns_sam_mag_cal_enable_resp_msg_v01*)
                               MALLOC(sizeof(sns_sam_mag_cal_enable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }

    memset( data_req, 0, sizeof(sns_sam_mag_cal_enable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_mag_cal_enable_resp_msg_v01) );

    /* sam MAG_CAL config */
    data_req->report_period = algo_config[0];
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_MAG_CAL_ENABLE_REQ_V01, data_req,
              sizeof(sns_sam_mag_cal_enable_req_msg_v01), data_resp,
              sizeof(sns_sam_mag_cal_enable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    printf("MAG_CAL CLNT: qmi_client_send_msg_async returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}


/*=========================================================================
  FUNCTION: sns_plbk_sam_mag_cal_disable
=========================================================================*/
/*!
    @brief
    Disabble's a SAM MAG_CAL Algorithm by sending disable message.
*/
/*=======================================================================*/
int sns_plbk_sam_mag_cal_disable(qmi_client_type *clnt)
{
    sns_sam_mag_cal_disable_req_msg_v01 *data_req;
    sns_sam_mag_cal_disable_resp_msg_v01 *data_resp;
    qmi_txn_handle txn;
    int rc;

    /* Prepare and send the request */
    data_req = (sns_sam_mag_cal_disable_req_msg_v01*)
                               MALLOC(sizeof(sns_sam_mag_cal_disable_req_msg_v01));
    data_resp = (sns_sam_mag_cal_disable_resp_msg_v01*)
                              MALLOC(sizeof(sns_sam_mag_cal_disable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }
    memset( data_req, 0, sizeof(sns_sam_mag_cal_disable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_mag_cal_disable_resp_msg_v01) );

    data_req->instance_id = algo_instance_id;
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_MAG_CAL_DISABLE_REQ_V01, data_req,
               sizeof(sns_sam_mag_cal_disable_req_msg_v01), data_resp,
               sizeof(sns_sam_mag_cal_disable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}


/*=========================================================================
  FUNCTION: sns_plbk_sam_filtered_mag_enable
=========================================================================*/
/*!
    @brief
    Enable's a SAM FILTERED_MAG Algorithm by sending enable message.
*/
/*=======================================================================*/
int sns_plbk_sam_filtered_mag_enable(int *algo_config, 
                            qmi_client_type *clnt)
{
    qmi_txn_handle txn;
    qmi_client_type notifier;
    unsigned int num_services, num_entries=10;
    qmi_cci_os_signal_type os_params;
    qmi_service_info service_info;
    qmi_idl_service_object_type service_object = 
                                      SNS_SAM_FILTERED_MAG_SVC_get_service_object_v01();
    sns_sam_filtered_mag_enable_req_msg_v01 *data_req;
    sns_sam_filtered_mag_enable_resp_msg_v01 *data_resp;
    int rc;

    if (!service_object) {
        printf("FILTERED_MAG CLNT: get_serivce_object failed\n");
    }

    memset(&os_params,0,sizeof(os_params));
    os_params.ext_signal = NULL;
    os_params.sig = QMI_CLNT_WAIT_SIG;
    os_params.timer_sig = QMI_CLNT_TIMER_SIG;

    rc = qmi_client_notifier_init(service_object, &os_params, &notifier);
    
    QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
    QMI_CCI_OS_SIGNAL_CLEAR(&os_params);
    
    /* The server has come up, store the information in info variable */
    rc = qmi_client_get_service_list(service_object, &service_info,
                                     &num_entries, &num_services);

    printf("FILTERED_MAG CLNT: qmi_client_get_service_list() returned %d num_entries=%d"
           " num_services = %d\n", rc, num_entries, num_services);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    rc = qmi_client_init(&service_info, service_object, sam_ind_cb, NULL, 
                         &os_params, clnt);
    printf("FILTERED_MAG CLNT: qmi_client_init returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    /* Prepare and send the request */
    data_req = (sns_sam_filtered_mag_enable_req_msg_v01*)
                                MALLOC(sizeof(sns_sam_filtered_mag_enable_req_msg_v01));
    data_resp = (sns_sam_filtered_mag_enable_resp_msg_v01*)
                               MALLOC(sizeof(sns_sam_filtered_mag_enable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }

    memset( data_req, 0, sizeof(sns_sam_filtered_mag_enable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_filtered_mag_enable_resp_msg_v01) );

    /* sam FILTERED_MAG config */
    data_req->report_period = algo_config[0];
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_FILTERED_MAG_ENABLE_REQ_V01, data_req,
              sizeof(sns_sam_filtered_mag_enable_req_msg_v01), data_resp,
              sizeof(sns_sam_filtered_mag_enable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    printf("FILTERED_MAG CLNT: qmi_client_send_msg_async returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}


/*=========================================================================
  FUNCTION: sns_plbk_sam_filtered_mag_disable
=========================================================================*/
/*!
    @brief
    Disabble's a SAM FILTERED_MAG Algorithm by sending disable message.
*/
/*=======================================================================*/
int sns_plbk_sam_filtered_mag_disable(qmi_client_type *clnt)
{
    sns_sam_filtered_mag_disable_req_msg_v01 *data_req;
    sns_sam_filtered_mag_disable_resp_msg_v01 *data_resp;
    qmi_txn_handle txn;
    int rc;

    /* Prepare and send the request */
    data_req = (sns_sam_filtered_mag_disable_req_msg_v01*)
                               MALLOC(sizeof(sns_sam_filtered_mag_disable_req_msg_v01));
    data_resp = (sns_sam_filtered_mag_disable_resp_msg_v01*)
                              MALLOC(sizeof(sns_sam_filtered_mag_disable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }
    memset( data_req, 0, sizeof(sns_sam_filtered_mag_disable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_filtered_mag_disable_resp_msg_v01) );

    data_req->instance_id = algo_instance_id;
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_FILTERED_MAG_DISABLE_REQ_V01, data_req,
               sizeof(sns_sam_filtered_mag_disable_req_msg_v01), data_resp,
               sizeof(sns_sam_filtered_mag_disable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}
/*=========================================================================
  FUNCTION: sns_plbk_sam_rotation_vector_enable
=========================================================================*/
/*!
    @brief
    Enable's a SAM ROTATION_VECTOR Algorithm by sending enable message.
*/
/*=======================================================================*/
int sns_plbk_sam_rotation_vector_enable(int *algo_config, 
                            qmi_client_type *clnt)
{
    qmi_txn_handle txn;
    qmi_client_type notifier;
    unsigned int num_services, num_entries=10;
    qmi_cci_os_signal_type os_params;
    qmi_service_info service_info;
    qmi_idl_service_object_type service_object = 
                                      SNS_SAM_ROTATION_VECTOR_SVC_get_service_object_v01();
    sns_sam_rotation_vector_enable_req_msg_v01 *data_req;
    sns_sam_rotation_vector_enable_resp_msg_v01 *data_resp;
    int rc;

    if (!service_object) {
        printf("ROTATION_VECTOR CLNT: get_serivce_object failed\n");
    }

    memset(&os_params,0,sizeof(os_params));
    os_params.ext_signal = NULL;
    os_params.sig = QMI_CLNT_WAIT_SIG;
    os_params.timer_sig = QMI_CLNT_TIMER_SIG;

    rc = qmi_client_notifier_init(service_object, &os_params, &notifier);
    
    QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
    QMI_CCI_OS_SIGNAL_CLEAR(&os_params);
    
    /* The server has come up, store the information in info variable */
    rc = qmi_client_get_service_list(service_object, &service_info,
                                     &num_entries, &num_services);

    printf("ROTATION_VECTOR CLNT: qmi_client_get_service_list() returned %d num_entries=%d"
           " num_services = %d\n", rc, num_entries, num_services);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    rc = qmi_client_init(&service_info, service_object, sam_ind_cb, NULL, 
                         &os_params, clnt);
    printf("ROTATION_VECTOR CLNT: qmi_client_init returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    /* Prepare and send the request */
    data_req = (sns_sam_rotation_vector_enable_req_msg_v01*)
                                MALLOC(sizeof(sns_sam_rotation_vector_enable_req_msg_v01));
    data_resp = (sns_sam_rotation_vector_enable_resp_msg_v01*)
                               MALLOC(sizeof(sns_sam_rotation_vector_enable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }

    memset( data_req, 0, sizeof(sns_sam_rotation_vector_enable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_rotation_vector_enable_resp_msg_v01) );

    /* sam ROTATION_VECTOR config */
    data_req->report_period = algo_config[0];
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_ROTATION_VECTOR_ENABLE_REQ_V01, data_req,
              sizeof(sns_sam_rotation_vector_enable_req_msg_v01), data_resp,
              sizeof(sns_sam_rotation_vector_enable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    printf("ROTATION_VECTOR CLNT: qmi_client_send_msg_async returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}

/*=========================================================================
  FUNCTION: sns_plbk_sam_rotation_vector_disable
=========================================================================*/
/*!
    @brief
    Disabble's a SAM ROTATION_VECTOR Algorithm by sending disable message.
*/
/*=======================================================================*/
int sns_plbk_sam_rotation_vector_disable(qmi_client_type *clnt)
{
    sns_sam_rotation_vector_disable_req_msg_v01 *data_req;
    sns_sam_rotation_vector_disable_resp_msg_v01 *data_resp;
    qmi_txn_handle txn;
    int rc;

    /* Prepare and send the request */
    data_req = (sns_sam_rotation_vector_disable_req_msg_v01*)
                               MALLOC(sizeof(sns_sam_rotation_vector_disable_req_msg_v01));
    data_resp = (sns_sam_rotation_vector_disable_resp_msg_v01*)
                              MALLOC(sizeof(sns_sam_rotation_vector_disable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }
    memset( data_req, 0, sizeof(sns_sam_rotation_vector_disable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_rotation_vector_disable_resp_msg_v01) );

    data_req->instance_id = algo_instance_id;
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_ROTATION_VECTOR_DISABLE_REQ_V01, data_req,
               sizeof(sns_sam_rotation_vector_disable_req_msg_v01), data_resp,
               sizeof(sns_sam_rotation_vector_disable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}

/*=========================================================================
  FUNCTION: sns_plbk_sam_quaternion_enable
=========================================================================*/
/*!
    @brief
    Enable's a SAM QUATERNION Algorithm by sending enable message.
*/
/*=======================================================================*/
int sns_plbk_sam_quaternion_enable(int *algo_config, 
                            qmi_client_type *clnt)
{
    qmi_txn_handle txn;
    qmi_client_type notifier;
    unsigned int num_services, num_entries=10;
    qmi_cci_os_signal_type os_params;
    qmi_service_info service_info;
    qmi_idl_service_object_type service_object = 
                                      SNS_SAM_QUATERNION_SVC_get_service_object_v01();
    sns_sam_quat_enable_req_msg_v01 *data_req;
    sns_sam_quat_enable_resp_msg_v01 *data_resp;
    int rc;

    if (!service_object) {
        printf("QUATERNION CLNT: get_serivce_object failed\n");
    }

    memset(&os_params,0,sizeof(os_params));
    os_params.ext_signal = NULL;
    os_params.sig = QMI_CLNT_WAIT_SIG;
    os_params.timer_sig = QMI_CLNT_TIMER_SIG;

    rc = qmi_client_notifier_init(service_object, &os_params, &notifier);
    
    QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
    QMI_CCI_OS_SIGNAL_CLEAR(&os_params);
    
    /* The server has come up, store the information in info variable */
    rc = qmi_client_get_service_list(service_object, &service_info,
                                     &num_entries, &num_services);

    printf("QUATERNION CLNT: qmi_client_get_service_list() returned %d num_entries=%d"
           " num_services = %d\n", rc, num_entries, num_services);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    rc = qmi_client_init(&service_info, service_object, sam_ind_cb, NULL, 
                         &os_params, clnt);
    printf("QUATERNION CLNT: qmi_client_init returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    /* Prepare and send the request */
    data_req = (sns_sam_quat_enable_req_msg_v01*)
                                MALLOC(sizeof(sns_sam_quat_enable_req_msg_v01));
    data_resp = (sns_sam_quat_enable_resp_msg_v01*)
                               MALLOC(sizeof(sns_sam_quat_enable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }

    memset( data_req, 0, sizeof(sns_sam_quat_enable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_quat_enable_resp_msg_v01) );

    /* sam QUATERNION config */
    data_req->report_period = algo_config[0];
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_QUAT_ENABLE_REQ_V01, data_req,
              sizeof(sns_sam_quat_enable_req_msg_v01), data_resp,
              sizeof(sns_sam_quat_enable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    printf("QUATERNION CLNT: qmi_client_send_msg_async returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}


/*=========================================================================
  FUNCTION: sns_plbk_sam_quaternion_disable
=========================================================================*/
/*!
    @brief
    Disabble's a SAM QUATERNION Algorithm by sending disable message.
*/
/*=======================================================================*/
int sns_plbk_sam_quaternion_disable(qmi_client_type *clnt)
{
    sns_sam_quat_disable_req_msg_v01 *data_req;
    sns_sam_quat_disable_resp_msg_v01 *data_resp;
    qmi_txn_handle txn;
    int rc;

    /* Prepare and send the request */
    data_req = (sns_sam_quat_disable_req_msg_v01*)
                               MALLOC(sizeof(sns_sam_quat_disable_req_msg_v01));
    data_resp = (sns_sam_quat_disable_resp_msg_v01*)
                              MALLOC(sizeof(sns_sam_quat_disable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }
    memset( data_req, 0, sizeof(sns_sam_quat_disable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_quat_disable_resp_msg_v01) );

    data_req->instance_id = algo_instance_id;
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_QUAT_DISABLE_REQ_V01, data_req,
               sizeof(sns_sam_quat_disable_req_msg_v01), data_resp,
               sizeof(sns_sam_quat_disable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}

/*=========================================================================
  FUNCTION: sns_plbk_sam_gravity_vector_enable
=========================================================================*/
/*!
    @brief
    Enable's a SAM Gravity Vector Algorithm by sending enable message.
*/
/*=======================================================================*/
int sns_plbk_sam_gravity_vector_enable(int *algo_config, 
                            qmi_client_type *clnt)
{
    qmi_txn_handle txn;
    qmi_client_type notifier;
    unsigned int num_services, num_entries=10;
    qmi_cci_os_signal_type os_params;
    qmi_service_info service_info;
    qmi_idl_service_object_type service_object = 
                                      SNS_SAM_GRAVITY_VECTOR_SVC_get_service_object_v01();
    sns_sam_gravity_enable_req_msg_v01 *data_req;
    sns_sam_gravity_enable_resp_msg_v01 *data_resp;
    int rc;

    if (!service_object) {
        printf("GRAVITY VEC CLNT: get_serivce_object failed\n");
    }

    memset(&os_params,0,sizeof(os_params));
    os_params.ext_signal = NULL;
    os_params.sig = QMI_CLNT_WAIT_SIG;
    os_params.timer_sig = QMI_CLNT_TIMER_SIG;

    rc = qmi_client_notifier_init(service_object, &os_params, &notifier);
    
    QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
    QMI_CCI_OS_SIGNAL_CLEAR(&os_params);
    
    /* The server has come up, store the information in info variable */
    rc = qmi_client_get_service_list(service_object, &service_info,
                                     &num_entries, &num_services);

    printf("GRAVITY VEC CLNT: qmi_client_get_service_list() returned %d num_entries=%d"
           " num_services = %d\n", rc, num_entries, num_services);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    rc = qmi_client_init(&service_info, service_object, sam_ind_cb, NULL, 
                         &os_params, clnt);
    printf("GRAVITY VEC CLNT: qmi_client_init returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    /* Prepare and send the request */
    data_req = (sns_sam_gravity_enable_req_msg_v01*)
                                MALLOC(sizeof(sns_sam_gravity_enable_req_msg_v01));
    data_resp = (sns_sam_gravity_enable_resp_msg_v01*)
                               MALLOC(sizeof(sns_sam_gravity_enable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }

    memset( data_req, 0, sizeof(sns_sam_gravity_enable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_gravity_enable_resp_msg_v01) );

    /* sam GRAVITY VEC config */
    data_req->report_period = algo_config[0];
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_GRAVITY_ENABLE_REQ_V01, data_req,
              sizeof(sns_sam_gravity_enable_req_msg_v01), data_resp,
              sizeof(sns_sam_gravity_enable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    printf("GRAVITY VEC CLNT: qmi_client_send_msg_async returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}

/*=========================================================================
  FUNCTION: sns_plbk_sam_gravity_vector_disable
=========================================================================*/
/*!
    @brief
    Disabble's a SAM GRAVITY Algorithm by sending disable message.
*/
/*=======================================================================*/
int sns_plbk_sam_gravity_vector_disable(qmi_client_type *clnt)
{
    sns_sam_gravity_disable_req_msg_v01 *data_req;
    sns_sam_gravity_disable_resp_msg_v01 *data_resp;
    qmi_txn_handle txn;
    int rc;

    /* Prepare and send the request */
    data_req = (sns_sam_gravity_disable_req_msg_v01*)
                               MALLOC(sizeof(sns_sam_gravity_disable_req_msg_v01));
    data_resp = (sns_sam_gravity_disable_resp_msg_v01*)
                              MALLOC(sizeof(sns_sam_gravity_disable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }
    memset( data_req, 0, sizeof(sns_sam_gravity_disable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_gravity_disable_resp_msg_v01) );

    data_req->instance_id = algo_instance_id;
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_GRAVITY_DISABLE_REQ_V01, data_req,
               sizeof(sns_sam_gravity_disable_req_msg_v01), data_resp,
               sizeof(sns_sam_gravity_disable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}


/*=========================================================================
  FUNCTION: sns_plbk_sam_orientation_enable
=========================================================================*/
/*!
    @brief
    Enable's a SAM ORIENTATION Algorithm by sending enable message.
*/
/*=======================================================================*/
int sns_plbk_sam_orientation_enable(int *algo_config, 
                            qmi_client_type *clnt)
{
    qmi_txn_handle txn;
    qmi_client_type notifier;
    unsigned int num_services, num_entries=10;
    qmi_cci_os_signal_type os_params;
    qmi_service_info service_info;
    qmi_idl_service_object_type service_object = 
                                      SNS_SAM_ORIENTATION_SVC_get_service_object_v01();
    sns_sam_orientation_enable_req_msg_v01 *data_req;
    sns_sam_orientation_enable_resp_msg_v01 *data_resp;
    int rc;

    if (!service_object) {
        printf("ORIENTATION CLNT: get_serivce_object failed\n");
    }

    memset(&os_params,0,sizeof(os_params));
    os_params.ext_signal = NULL;
    os_params.sig = QMI_CLNT_WAIT_SIG;
    os_params.timer_sig = QMI_CLNT_TIMER_SIG;

    rc = qmi_client_notifier_init(service_object, &os_params, &notifier);
    
    QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
    QMI_CCI_OS_SIGNAL_CLEAR(&os_params);
    
    /* The server has come up, store the information in info variable */
    rc = qmi_client_get_service_list(service_object, &service_info,
                                     &num_entries, &num_services);

    printf("ORIENTATION CLNT: qmi_client_get_service_list() returned %d num_entries=%d"
           " num_services = %d\n", rc, num_entries, num_services);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    rc = qmi_client_init(&service_info, service_object, sam_ind_cb, NULL, 
                         &os_params, clnt);
    printf("ORIENTATION CLNT: qmi_client_init returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    /* Prepare and send the request */
    data_req = (sns_sam_orientation_enable_req_msg_v01*)
                                MALLOC(sizeof(sns_sam_orientation_enable_req_msg_v01));
    data_resp = (sns_sam_orientation_enable_resp_msg_v01*)
                               MALLOC(sizeof(sns_sam_orientation_enable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }

    memset( data_req, 0, sizeof(sns_sam_orientation_enable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_orientation_enable_resp_msg_v01) );

    /* sam ORIENTATION config */
    data_req->report_period = algo_config[0];
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_ORIENTATION_ENABLE_REQ_V01, data_req,
              sizeof(sns_sam_orientation_enable_req_msg_v01), data_resp,
              sizeof(sns_sam_orientation_enable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    printf("ORIENTATION CLNT: qmi_client_send_msg_async returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}

/*=========================================================================
  FUNCTION: sns_plbk_sam_orientation_disable
=========================================================================*/
/*!
    @brief
    Disabble's a SAM ORIENTATION Algorithm by sending disable message.
*/
/*=======================================================================*/
int sns_plbk_sam_orientation_disable(qmi_client_type *clnt)
{
    sns_sam_orientation_disable_req_msg_v01 *data_req;
    sns_sam_orientation_disable_resp_msg_v01 *data_resp;
    qmi_txn_handle txn;
    int rc;

    /* Prepare and send the request */
    data_req = (sns_sam_orientation_disable_req_msg_v01*)
                               MALLOC(sizeof(sns_sam_orientation_disable_req_msg_v01));
    data_resp = (sns_sam_orientation_disable_resp_msg_v01*)
                              MALLOC(sizeof(sns_sam_orientation_disable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }
    memset( data_req, 0, sizeof(sns_sam_orientation_disable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_orientation_disable_resp_msg_v01) );

    data_req->instance_id = algo_instance_id;
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_ORIENTATION_DISABLE_REQ_V01, data_req,
               sizeof(sns_sam_orientation_disable_req_msg_v01), data_resp,
               sizeof(sns_sam_orientation_disable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}

/*=========================================================================
  FUNCTION: sns_plbk_sam_basic_gestures_enable
=========================================================================*/
/*!
    @brief
    Enable's a SAM Basic Gestures Algorithm by sending enable message.
*/
/*=======================================================================*/
int sns_plbk_sam_basic_gestures_enable(int *algo_config, 
                            qmi_client_type *clnt)
{
    qmi_txn_handle txn;
    qmi_client_type notifier;
    unsigned int num_services, num_entries=10;
    qmi_cci_os_signal_type os_params;
    qmi_service_info service_info;
    qmi_idl_service_object_type service_object = 
                                      SNS_SAM_BASIC_GESTURES_SVC_get_service_object_v01();
    sns_sam_basic_gestures_enable_req_msg_v01 *data_req;
    sns_sam_basic_gestures_enable_resp_msg_v01 *data_resp;
    int rc;

    if (!service_object) {
        printf("BASIC_GESTURES CLNT: get_serivce_object failed\n");
    }

    memset(&os_params,0,sizeof(os_params));
    os_params.ext_signal = NULL;
    os_params.sig = QMI_CLNT_WAIT_SIG;
    os_params.timer_sig = QMI_CLNT_TIMER_SIG;

    rc = qmi_client_notifier_init(service_object, &os_params, &notifier);
    
    QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
    QMI_CCI_OS_SIGNAL_CLEAR(&os_params);
    
    /* The server has come up, store the information in info variable */
    rc = qmi_client_get_service_list(service_object, &service_info,
                                     &num_entries, &num_services);

    printf("BASIC_GESTURES CLNT: qmi_client_get_service_list() returned %d num_entries=%d"
           " num_services = %d\n", rc, num_entries, num_services);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    rc = qmi_client_init(&service_info, service_object, sam_ind_cb, NULL, 
                         &os_params, clnt);
    printf("BASIC_GESTURES CLNT: qmi_client_init returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    /* Prepare and send the request */
    data_req = (sns_sam_basic_gestures_enable_req_msg_v01*)
                                MALLOC(sizeof(sns_sam_basic_gestures_enable_req_msg_v01));
    data_resp = (sns_sam_basic_gestures_enable_resp_msg_v01*)
                               MALLOC(sizeof(sns_sam_basic_gestures_enable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }

    memset( data_req, 0, sizeof(sns_sam_basic_gestures_enable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_basic_gestures_enable_resp_msg_v01) );

    /* sam Basic Gestures config */
     // NO Mandatory config
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_BASIC_GESTURES_ENABLE_REQ_V01, data_req,
              sizeof(sns_sam_basic_gestures_enable_req_msg_v01), data_resp,
              sizeof(sns_sam_basic_gestures_enable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    printf("BASIC_GESTURES CLNT: qmi_client_send_msg_async returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}


/*=========================================================================
  FUNCTION: sns_plbk_sam_basic_gestures_disable
=========================================================================*/
/*!
    @brief
    Disabble's a SAM BASIC_GESTURES Algorithm by sending disable message.
*/
/*=======================================================================*/
int sns_plbk_sam_basic_gestures_disable(qmi_client_type *clnt)
{
    sns_sam_basic_gestures_disable_req_msg_v01 *data_req;
    sns_sam_basic_gestures_disable_resp_msg_v01 *data_resp;
    qmi_txn_handle txn;
    int rc;

    /* Prepare and send the request */
    data_req = (sns_sam_basic_gestures_disable_req_msg_v01*)
                               MALLOC(sizeof(sns_sam_basic_gestures_disable_req_msg_v01));
    data_resp = (sns_sam_basic_gestures_disable_resp_msg_v01*)
                              MALLOC(sizeof(sns_sam_basic_gestures_disable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }
    memset( data_req, 0, sizeof(sns_sam_basic_gestures_disable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_basic_gestures_disable_resp_msg_v01) );

    data_req->instance_id = algo_instance_id;
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_BASIC_GESTURES_DISABLE_REQ_V01, data_req,
               sizeof(sns_sam_basic_gestures_disable_req_msg_v01), data_resp,
               sizeof(sns_sam_basic_gestures_disable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}


/*=========================================================================
  FUNCTION: sns_plbk_sam_tap_enable
=========================================================================*/
/*!
    @brief
    Enable's a SAM TAP Algorithm by sending enable message.
*/
/*=======================================================================*/
int sns_plbk_sam_tap_enable(int *algo_config, 
                            qmi_client_type *clnt)
{
    qmi_txn_handle txn;
    qmi_client_type notifier;
    unsigned int num_services, num_entries=10;
    qmi_cci_os_signal_type os_params;
    qmi_service_info service_info;
    qmi_idl_service_object_type service_object = 
                                      SNS_SAM_TAP_SVC_get_service_object_v01();
    sns_sam_tap_enable_req_msg_v01 *data_req;
    sns_sam_tap_enable_resp_msg_v01 *data_resp;
    int rc;

    if (!service_object) {
        printf("TAP CLNT: get_serivce_object failed\n");
    }

    memset(&os_params,0,sizeof(os_params));
    os_params.ext_signal = NULL;
    os_params.sig = QMI_CLNT_WAIT_SIG;
    os_params.timer_sig = QMI_CLNT_TIMER_SIG;

    rc = qmi_client_notifier_init(service_object, &os_params, &notifier);
    
    QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
    QMI_CCI_OS_SIGNAL_CLEAR(&os_params);
    
    /* The server has come up, store the information in info variable */
    rc = qmi_client_get_service_list(service_object, &service_info,
                                     &num_entries, &num_services);

    printf("TAP CLNT: qmi_client_get_service_list() returned %d num_entries=%d"
           " num_services = %d\n", rc, num_entries, num_services);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    rc = qmi_client_init(&service_info, service_object, sam_ind_cb, NULL, 
                         &os_params, clnt);
    printf("TAP CLNT: qmi_client_init returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    /* Prepare and send the request */
    data_req = (sns_sam_tap_enable_req_msg_v01*)
                                MALLOC(sizeof(sns_sam_tap_enable_req_msg_v01));
    data_resp = (sns_sam_tap_enable_resp_msg_v01*)
                               MALLOC(sizeof(sns_sam_tap_enable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }

    memset( data_req, 0, sizeof(sns_sam_tap_enable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_tap_enable_resp_msg_v01) );

    /* sam TAP config */
    data_req->report_period = algo_config[0];
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_TAP_ENABLE_REQ_V01, data_req,
              sizeof(sns_sam_tap_enable_req_msg_v01), data_resp,
              sizeof(sns_sam_tap_enable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    printf("TAP CLNT: qmi_client_send_msg_async returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}


/*=========================================================================
  FUNCTION: sns_plbk_sam_tap_disable
=========================================================================*/
/*!
    @brief
    Disabble's a SAM TAP Algorithm by sending disable message.
*/
/*=======================================================================*/
int sns_plbk_sam_tap_disable(qmi_client_type *clnt)
{
    sns_sam_tap_disable_req_msg_v01 *data_req;
    sns_sam_tap_disable_resp_msg_v01 *data_resp;
    qmi_txn_handle txn;
    int rc;

    /* Prepare and send the request */
    data_req = (sns_sam_tap_disable_req_msg_v01*)
                               MALLOC(sizeof(sns_sam_tap_disable_req_msg_v01));
    data_resp = (sns_sam_tap_disable_resp_msg_v01*)
                              MALLOC(sizeof(sns_sam_tap_disable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }
    memset( data_req, 0, sizeof(sns_sam_tap_disable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_tap_disable_resp_msg_v01) );

    data_req->instance_id = algo_instance_id;
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_TAP_DISABLE_REQ_V01, data_req,
               sizeof(sns_sam_tap_disable_req_msg_v01), data_resp,
               sizeof(sns_sam_tap_disable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}

/*=========================================================================
  FUNCTION: sns_plbk_sam_facing_enable
=========================================================================*/
/*!
    @brief
    Enable's a SAM FACING Algorithm by sending enable message.
*/
/*=======================================================================*/
int sns_plbk_sam_facing_enable(int *algo_config, 
                            qmi_client_type *clnt)
{
    qmi_txn_handle txn;
    qmi_client_type notifier;
    unsigned int num_services, num_entries=10;
    qmi_cci_os_signal_type os_params;
    qmi_service_info service_info;
    qmi_idl_service_object_type service_object = 
                                      SNS_SAM_FACING_SVC_get_service_object_v01();
    sns_sam_facing_enable_req_msg_v01 *data_req;
    sns_sam_facing_enable_resp_msg_v01 *data_resp;
    int rc;

    if (!service_object) {
        printf("FACING CLNT: get_serivce_object failed\n");
    }

    memset(&os_params,0,sizeof(os_params));
    os_params.ext_signal = NULL;
    os_params.sig = QMI_CLNT_WAIT_SIG;
    os_params.timer_sig = QMI_CLNT_TIMER_SIG;

    rc = qmi_client_notifier_init(service_object, &os_params, &notifier);
    
    QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
    QMI_CCI_OS_SIGNAL_CLEAR(&os_params);
    
    /* The server has come up, store the information in info variable */
    rc = qmi_client_get_service_list(service_object, &service_info,
                                     &num_entries, &num_services);

    printf("FACING CLNT: qmi_client_get_service_list() returned %d num_entries=%d"
           " num_services = %d\n", rc, num_entries, num_services);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    rc = qmi_client_init(&service_info, service_object, sam_ind_cb, NULL, 
                         &os_params, clnt);
    printf("FACING CLNT: qmi_client_init returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    /* Prepare and send the request */
    data_req = (sns_sam_facing_enable_req_msg_v01*)
                                MALLOC(sizeof(sns_sam_facing_enable_req_msg_v01));
    data_resp = (sns_sam_facing_enable_resp_msg_v01*)
                               MALLOC(sizeof(sns_sam_facing_enable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }

    memset( data_req, 0, sizeof(sns_sam_facing_enable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_facing_enable_resp_msg_v01) );

    /* sam FACING config */
    data_req->report_period = algo_config[0];
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_FACING_ENABLE_REQ_V01, data_req,
              sizeof(sns_sam_facing_enable_req_msg_v01), data_resp,
              sizeof(sns_sam_facing_enable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    printf("FACING CLNT: qmi_client_send_msg_async returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}


/*=========================================================================
  FUNCTION: sns_plbk_sam_facing_disable
=========================================================================*/
/*!
    @brief
    Disabble's a SAM FACING Algorithm by sending disable message.
*/
/*=======================================================================*/
int sns_plbk_sam_facing_disable(qmi_client_type *clnt)
{
    sns_sam_facing_disable_req_msg_v01 *data_req;
    sns_sam_facing_disable_resp_msg_v01 *data_resp;
    qmi_txn_handle txn;
    int rc;

    /* Prepare and send the request */
    data_req = (sns_sam_facing_disable_req_msg_v01*)
                               MALLOC(sizeof(sns_sam_facing_disable_req_msg_v01));
    data_resp = (sns_sam_facing_disable_resp_msg_v01*)
                              MALLOC(sizeof(sns_sam_facing_disable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }
    memset( data_req, 0, sizeof(sns_sam_facing_disable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_facing_disable_resp_msg_v01) );

    data_req->instance_id = algo_instance_id;
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_FACING_DISABLE_REQ_V01, data_req,
               sizeof(sns_sam_facing_disable_req_msg_v01), data_resp,
               sizeof(sns_sam_facing_disable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}

/*=========================================================================
  FUNCTION: sns_plbk_sam_integ_angle_enable
=========================================================================*/
/*!
    @brief
    Enable's a SAM INTEG_ANGLE Algorithm by sending enable message.
*/
/*=======================================================================*/
int sns_plbk_sam_integ_angle_enable(int *algo_config, 
                            qmi_client_type *clnt)
{
    qmi_txn_handle txn;
    qmi_client_type notifier;
    unsigned int num_services, num_entries=10;
    qmi_cci_os_signal_type os_params;
    qmi_service_info service_info;
    qmi_idl_service_object_type service_object = 
                                      SNS_SAM_INTEG_ANGLE_SVC_get_service_object_v01();
    sns_sam_integ_angle_enable_req_msg_v01 *data_req;
    sns_sam_integ_angle_enable_resp_msg_v01 *data_resp;
    int rc;

    if (!service_object) {
        printf("INTEG_ANGLE CLNT: get_serivce_object failed\n");
    }

    memset(&os_params,0,sizeof(os_params));
    os_params.ext_signal = NULL;
    os_params.sig = QMI_CLNT_WAIT_SIG;
    os_params.timer_sig = QMI_CLNT_TIMER_SIG;

    rc = qmi_client_notifier_init(service_object, &os_params, &notifier);
    
    QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
    QMI_CCI_OS_SIGNAL_CLEAR(&os_params);
    
    /* The server has come up, store the information in info variable */
    rc = qmi_client_get_service_list(service_object, &service_info,
                                     &num_entries, &num_services);

    printf("INTEG_ANGLE CLNT: qmi_client_get_service_list() returned %d num_entries=%d"
           " num_services = %d\n", rc, num_entries, num_services);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    rc = qmi_client_init(&service_info, service_object, sam_ind_cb, NULL, 
                         &os_params, clnt);
    printf("INTEG_ANGLE CLNT: qmi_client_init returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    /* Prepare and send the request */
    data_req = (sns_sam_integ_angle_enable_req_msg_v01*)
                                MALLOC(sizeof(sns_sam_integ_angle_enable_req_msg_v01));
    data_resp = (sns_sam_integ_angle_enable_resp_msg_v01*)
                               MALLOC(sizeof(sns_sam_integ_angle_enable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }

    memset( data_req, 0, sizeof(sns_sam_integ_angle_enable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_integ_angle_enable_resp_msg_v01) );

    /* sam INTEG_ANGLE config */
    data_req->report_period = algo_config[0];
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_INTEG_ANGLE_ENABLE_REQ_V01, data_req,
              sizeof(sns_sam_integ_angle_enable_req_msg_v01), data_resp,
              sizeof(sns_sam_integ_angle_enable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    printf("INTEG_ANGLE CLNT: qmi_client_send_msg_async returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}

/*=========================================================================
  FUNCTION: sns_plbk_sam_integ_angle_disable
=========================================================================*/
/*!
    @brief
    Disabble's a SAM INTEG_ANGLE Algorithm by sending disable message.
*/
/*=======================================================================*/
int sns_plbk_sam_integ_angle_disable(qmi_client_type *clnt)
{
    sns_sam_integ_angle_disable_req_msg_v01 *data_req;
    sns_sam_integ_angle_disable_resp_msg_v01 *data_resp;
    qmi_txn_handle txn;
    int rc;

    /* Prepare and send the request */
    data_req = (sns_sam_integ_angle_disable_req_msg_v01*)
                               MALLOC(sizeof(sns_sam_integ_angle_disable_req_msg_v01));
    data_resp = (sns_sam_integ_angle_disable_resp_msg_v01*)
                              MALLOC(sizeof(sns_sam_integ_angle_disable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }
    memset( data_req, 0, sizeof(sns_sam_integ_angle_disable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_integ_angle_disable_resp_msg_v01) );

    data_req->instance_id = algo_instance_id;
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_INTEG_ANGLE_DISABLE_REQ_V01, data_req,
               sizeof(sns_sam_integ_angle_disable_req_msg_v01), data_resp,
               sizeof(sns_sam_integ_angle_disable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}


/*=========================================================================
  FUNCTION: sns_plbk_sam_gyro_tap2_enable
=========================================================================*/
/*!
    @brief
    Enable's a SAM GYRO TAP2 Algorithm by sending enable message.
*/
/*=======================================================================*/
int sns_plbk_sam_gyro_tap2_enable(int *algo_config, 
                            qmi_client_type *clnt)
{
    qmi_txn_handle txn;
    qmi_client_type notifier;
    unsigned int num_services, num_entries=10;
    qmi_cci_os_signal_type os_params;
    qmi_service_info service_info;
    qmi_idl_service_object_type service_object = 
                                      SNS_SAM_GYRO_TAP2_SVC_get_service_object_v01();
    sns_sam_gyro_tap2_enable_req_msg_v01 *data_req;
    sns_sam_gyro_tap2_enable_resp_msg_v01 *data_resp;
    int rc;

    if (!service_object) {
        printf("GYRO_TAP2 CLNT: get_serivce_object failed\n");
    }

    memset(&os_params,0,sizeof(os_params));
    os_params.ext_signal = NULL;
    os_params.sig = QMI_CLNT_WAIT_SIG;
    os_params.timer_sig = QMI_CLNT_TIMER_SIG;

    rc = qmi_client_notifier_init(service_object, &os_params, &notifier);
    
    QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
    QMI_CCI_OS_SIGNAL_CLEAR(&os_params);
    
    /* The server has come up, store the information in info variable */
    rc = qmi_client_get_service_list(service_object, &service_info,
                                     &num_entries, &num_services);

    printf("GYRO_TAP2 CLNT: qmi_client_get_service_list() returned %d num_entries=%d"
           " num_services = %d\n", rc, num_entries, num_services);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    rc = qmi_client_init(&service_info, service_object, sam_ind_cb, NULL, 
                         &os_params, clnt);
    printf("GYRO_TAP2 CLNT: qmi_client_init returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    /* Prepare and send the request */
    data_req = (sns_sam_gyro_tap2_enable_req_msg_v01*)
                                MALLOC(sizeof(sns_sam_gyro_tap2_enable_req_msg_v01));
    data_resp = (sns_sam_gyro_tap2_enable_resp_msg_v01*)
                               MALLOC(sizeof(sns_sam_gyro_tap2_enable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }

    memset( data_req, 0, sizeof(sns_sam_gyro_tap2_enable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_gyro_tap2_enable_resp_msg_v01) );

    /* sam GYRO_TAP2 config */
    // NO mandatory config
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_GYRO_TAP2_ENABLE_REQ_V01, data_req,
              sizeof(sns_sam_gyro_tap2_enable_req_msg_v01), data_resp,
              sizeof(sns_sam_gyro_tap2_enable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    printf("GYRO_TAP2 CLNT: qmi_client_send_msg_async returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}


/*=========================================================================
  FUNCTION: sns_plbk_sam_gyro_tap2_disable
=========================================================================*/
/*!
    @brief
    Disabble's a SAM GYRO_TAP2 Algorithm by sending disable message.
*/
/*=======================================================================*/
int sns_plbk_sam_gyro_tap2_disable(qmi_client_type *clnt)
{
    sns_sam_gyro_tap2_disable_req_msg_v01 *data_req;
    sns_sam_gyro_tap2_disable_resp_msg_v01 *data_resp;
    qmi_txn_handle txn;
    int rc;

    /* Prepare and send the request */
    data_req = (sns_sam_gyro_tap2_disable_req_msg_v01*)
                               MALLOC(sizeof(sns_sam_gyro_tap2_disable_req_msg_v01));
    data_resp = (sns_sam_gyro_tap2_disable_resp_msg_v01*)
                              MALLOC(sizeof(sns_sam_gyro_tap2_disable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }
    memset( data_req, 0, sizeof(sns_sam_gyro_tap2_disable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_gyro_tap2_disable_resp_msg_v01) );

    data_req->instance_id = algo_instance_id;
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_GYRO_TAP2_DISABLE_REQ_V01, data_req,
               sizeof(sns_sam_gyro_tap2_disable_req_msg_v01), data_resp,
               sizeof(sns_sam_gyro_tap2_disable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}



/*=========================================================================
  FUNCTION: sns_plbk_sam_ped_enable
=========================================================================*/
/*!
    @brief
    Enable's a SAM Pedometer Algorithm by sending enable message.
*/
/*=======================================================================*/
int sns_plbk_sam_ped_enable(int *algo_config, 
                            qmi_client_type *clnt)
{
    qmi_txn_handle txn;
    qmi_client_type notifier;
    unsigned int num_services, num_entries=10;
    qmi_cci_os_signal_type os_params;
    qmi_service_info service_info;
    qmi_idl_service_object_type service_object = 
                                      SNS_SAM_PED_SVC_get_service_object_v01();
    sns_sam_ped_enable_req_msg_v01 *data_req;
    sns_sam_ped_enable_resp_msg_v01 *data_resp;
    int rc;

    if (!service_object) {
        printf("PED CLNT: get_serivce_object failed\n");
    }

    memset(&os_params,0,sizeof(os_params));
    os_params.ext_signal = NULL;
    os_params.sig = QMI_CLNT_WAIT_SIG;
    os_params.timer_sig = QMI_CLNT_TIMER_SIG;

    rc = qmi_client_notifier_init(service_object, &os_params, &notifier);
    
    QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
    QMI_CCI_OS_SIGNAL_CLEAR(&os_params);
    
    /* The server has come up, store the information in info variable */
    rc = qmi_client_get_service_list(service_object, &service_info,
                                     &num_entries, &num_services);

    printf("PED CLNT: qmi_client_get_service_list() returned %d num_entries=%d"
           " num_services = %d\n", rc, num_entries, num_services);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    rc = qmi_client_init(&service_info, service_object, sam_ind_cb, NULL, 
                         &os_params, clnt);
    printf("PED CLNT: qmi_client_init returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    /* Prepare and send the request */
    data_req = (sns_sam_ped_enable_req_msg_v01*)
                                MALLOC(sizeof(sns_sam_ped_enable_req_msg_v01));
    data_resp = (sns_sam_ped_enable_resp_msg_v01*)
                               MALLOC(sizeof(sns_sam_ped_enable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }

    memset( data_req, 0, sizeof(sns_sam_ped_enable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_ped_enable_resp_msg_v01) );

    /* sam Pedometer config */
    data_req->report_period = algo_config[0];
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_PED_ENABLE_REQ_V01, data_req,
              sizeof(sns_sam_ped_enable_req_msg_v01), data_resp,
              sizeof(sns_sam_ped_enable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    printf("PED CLNT: qmi_client_send_msg_async returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}


/*=========================================================================
  FUNCTION: sns_plbk_sam_ped_disable
=========================================================================*/
/*!
    @brief
    Disabble's a SAM PED Algorithm by sending disable message.
*/
/*=======================================================================*/
int sns_plbk_sam_ped_disable(qmi_client_type *clnt)
{
    sns_sam_ped_disable_req_msg_v01 *data_req;
    sns_sam_ped_disable_resp_msg_v01 *data_resp;
    qmi_txn_handle txn;
    int rc;

    /* Prepare and send the request */
    data_req = (sns_sam_ped_disable_req_msg_v01*)
                               MALLOC(sizeof(sns_sam_ped_disable_req_msg_v01));
    data_resp = (sns_sam_ped_disable_resp_msg_v01*)
                              MALLOC(sizeof(sns_sam_ped_disable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }
    memset( data_req, 0, sizeof(sns_sam_ped_disable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_ped_disable_resp_msg_v01) );

    data_req->instance_id = algo_instance_id;
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_PED_DISABLE_REQ_V01, data_req,
               sizeof(sns_sam_ped_disable_req_msg_v01), data_resp,
               sizeof(sns_sam_ped_disable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}


/*=========================================================================
  FUNCTION: sns_plbk_sam_pam_enable
=========================================================================*/
/*!
    @brief
    Enable's a SAM PAM Algorithm by sending enable message.
*/
/*=======================================================================*/
int sns_plbk_sam_pam_enable(int *algo_config, 
                            qmi_client_type *clnt)
{
    qmi_txn_handle txn;
    qmi_client_type notifier;
    unsigned int num_services, num_entries=10;
    qmi_cci_os_signal_type os_params;
    qmi_service_info service_info;
    qmi_idl_service_object_type service_object = 
                                      SNS_SAM_PAM_SVC_get_service_object_v01();
    sns_sam_pam_enable_req_msg_v01 *data_req;
    sns_sam_pam_enable_resp_msg_v01 *data_resp;
    int rc;

    if (!service_object) {
        printf("PAM CLNT: get_serivce_object failed\n");
    }

    memset(&os_params,0,sizeof(os_params));
    os_params.ext_signal = NULL;
    os_params.sig = QMI_CLNT_WAIT_SIG;
    os_params.timer_sig = QMI_CLNT_TIMER_SIG;

    rc = qmi_client_notifier_init(service_object, &os_params, &notifier);
    
    QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
    QMI_CCI_OS_SIGNAL_CLEAR(&os_params);
    
    /* The server has come up, store the information in info variable */
    rc = qmi_client_get_service_list(service_object, &service_info,
                                     &num_entries, &num_services);

    printf("PAM CLNT: qmi_client_get_service_list() returned %d num_entries=%d"
           " num_services = %d\n", rc, num_entries, num_services);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    rc = qmi_client_init(&service_info, service_object, sam_ind_cb, NULL, 
                         &os_params, clnt);
    printf("PAM CLNT: qmi_client_init returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    /* Prepare and send the request */
    data_req = (sns_sam_pam_enable_req_msg_v01*)
                                MALLOC(sizeof(sns_sam_pam_enable_req_msg_v01));
    data_resp = (sns_sam_pam_enable_resp_msg_v01*)
                               MALLOC(sizeof(sns_sam_pam_enable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }

    memset( data_req, 0, sizeof(sns_sam_pam_enable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_pam_enable_resp_msg_v01) );

    /* sam PAM config */
    data_req->measurement_period = algo_config[0];
    data_req->step_count_threshold = algo_config[1];
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_PAM_ENABLE_REQ_V01, data_req,
              sizeof(sns_sam_pam_enable_req_msg_v01), data_resp,
              sizeof(sns_sam_pam_enable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    printf("PAM CLNT: qmi_client_send_msg_async returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}


/*=========================================================================
  FUNCTION: sns_plbk_sam_pam_disable
=========================================================================*/
/*!
    @brief
    Disabble's a SAM PAM Algorithm by sending disable message.
*/
/*=======================================================================*/
int sns_plbk_sam_pam_disable(qmi_client_type *clnt)
{
    sns_sam_pam_disable_req_msg_v01 *data_req;
    sns_sam_pam_disable_resp_msg_v01 *data_resp;
    qmi_txn_handle txn;
    int rc;

    /* Prepare and send the request */
    data_req = (sns_sam_pam_disable_req_msg_v01*)
                               MALLOC(sizeof(sns_sam_pam_disable_req_msg_v01));
    data_resp = (sns_sam_pam_disable_resp_msg_v01*)
                              MALLOC(sizeof(sns_sam_pam_disable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }
    memset( data_req, 0, sizeof(sns_sam_pam_disable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_pam_disable_resp_msg_v01) );

    data_req->instance_id = algo_instance_id;
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_PAM_DISABLE_REQ_V01, data_req,
               sizeof(sns_sam_pam_disable_req_msg_v01), data_resp,
               sizeof(sns_sam_pam_disable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}



/*=========================================================================
  FUNCTION: sns_plbk_sam_smd_enable
=========================================================================*/
/*!
    @brief
    Enable's a SAM smd Algorithm by sending enable message.
*/
/*=======================================================================*/
int sns_plbk_sam_smd_enable(int *algo_config, 
                            qmi_client_type *clnt)
{
    qmi_txn_handle txn;
    qmi_client_type notifier;
    unsigned int num_services, num_entries=10;
    qmi_cci_os_signal_type os_params;
    qmi_service_info service_info;
    qmi_idl_service_object_type service_object = 
                                      SNS_SAM_SMD_SVC_get_service_object_v01();
    sns_sam_smd_enable_req_msg_v01 *data_req;
    sns_sam_smd_enable_resp_msg_v01 *data_resp;
    int rc;

    if (!service_object) {
        printf("SMD CLNT: get_serivce_object failed\n");
    }

    memset(&os_params,0,sizeof(os_params));
    os_params.ext_signal = NULL;
    os_params.sig = QMI_CLNT_WAIT_SIG;
    os_params.timer_sig = QMI_CLNT_TIMER_SIG;

    rc = qmi_client_notifier_init(service_object, &os_params, &notifier);
    
    QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
    QMI_CCI_OS_SIGNAL_CLEAR(&os_params);
    
    /* The server has come up, store the information in info variable */
    rc = qmi_client_get_service_list(service_object, &service_info,
                                     &num_entries, &num_services);

    printf("SMD CLNT: qmi_client_get_service_list() returned %d num_entries=%d"
           " num_services = %d\n", rc, num_entries, num_services);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    rc = qmi_client_init(&service_info, service_object, sam_ind_cb, NULL, 
                         &os_params, clnt);
    printf("SMD CLNT: qmi_client_init returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    /* Prepare and send the request */
    data_req = (sns_sam_smd_enable_req_msg_v01*)
                                MALLOC(sizeof(sns_sam_smd_enable_req_msg_v01));
    data_resp = (sns_sam_smd_enable_resp_msg_v01*)
                               MALLOC(sizeof(sns_sam_smd_enable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }

    memset( data_req, 0, sizeof(sns_sam_smd_enable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_smd_enable_resp_msg_v01) );

    /* sam SMD config */
    //No mandatory config for this algo
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_SMD_ENABLE_REQ_V01, data_req,
              sizeof(sns_sam_smd_enable_req_msg_v01), data_resp,
              sizeof(sns_sam_smd_enable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    printf("SMD CLNT: qmi_client_send_msg_async returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}


/*=========================================================================
  FUNCTION: sns_plbk_sam_smd_disable
=========================================================================*/
/*!
    @brief
    Disabble's a SAM SMD Algorithm by sending disable message.
*/
/*=======================================================================*/
int sns_plbk_sam_smd_disable(qmi_client_type *clnt)
{
    sns_sam_smd_disable_req_msg_v01 *data_req;
    sns_sam_smd_disable_resp_msg_v01 *data_resp;
    qmi_txn_handle txn;
    int rc;

    /* Prepare and send the request */
    data_req = (sns_sam_smd_disable_req_msg_v01*)
                               MALLOC(sizeof(sns_sam_smd_disable_req_msg_v01));
    data_resp = (sns_sam_smd_disable_resp_msg_v01*)
                              MALLOC(sizeof(sns_sam_smd_disable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }
    memset( data_req, 0, sizeof(sns_sam_smd_disable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_smd_disable_resp_msg_v01) );

    data_req->instance_id = algo_instance_id;
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_SMD_DISABLE_REQ_V01, data_req,
               sizeof(sns_sam_smd_disable_req_msg_v01), data_resp,
               sizeof(sns_sam_smd_disable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}


/*=========================================================================
  FUNCTION: sns_plbk_sam_cmc_enable
=========================================================================*/
/*!
    @brief
    Enable's a SAM CMC Algorithm by sending enable message.
*/
/*=======================================================================*/
int sns_plbk_sam_cmc_enable(int *algo_config, 
                            qmi_client_type *clnt)
{
    qmi_txn_handle txn;
    qmi_client_type notifier;
    unsigned int num_services, num_entries=10;
    qmi_cci_os_signal_type os_params;
    qmi_service_info service_info;
    qmi_idl_service_object_type service_object = 
                                      SNS_SAM_CMC_SVC_get_service_object_v01();
    sns_sam_cmc_enable_req_msg_v01 *data_req;
    sns_sam_cmc_enable_resp_msg_v01 *data_resp;
    int rc;

    if (!service_object) {
        printf("CMC CLNT: get_serivce_object failed\n");
    }

    memset(&os_params,0,sizeof(os_params));
    os_params.ext_signal = NULL;
    os_params.sig = QMI_CLNT_WAIT_SIG;
    os_params.timer_sig = QMI_CLNT_TIMER_SIG;

    rc = qmi_client_notifier_init(service_object, &os_params, &notifier);
    
    QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
    QMI_CCI_OS_SIGNAL_CLEAR(&os_params);
    
    /* The server has come up, store the information in info variable */
    rc = qmi_client_get_service_list(service_object, &service_info,
                                     &num_entries, &num_services);

    printf("CMC CLNT: qmi_client_get_service_list() returned %d num_entries=%d"
           " num_services = %d\n", rc, num_entries, num_services);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    rc = qmi_client_init(&service_info, service_object, sam_ind_cb, NULL, 
                         &os_params, clnt);
    printf("CMC CLNT: qmi_client_init returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    /* Prepare and send the request */
    data_req = (sns_sam_cmc_enable_req_msg_v01*)
                                MALLOC(sizeof(sns_sam_cmc_enable_req_msg_v01));
    data_resp = (sns_sam_cmc_enable_resp_msg_v01*)
                               MALLOC(sizeof(sns_sam_cmc_enable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }

    memset( data_req, 0, sizeof(sns_sam_cmc_enable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_cmc_enable_resp_msg_v01) );

    /* sam CMC config */
    //No mandatory config for this algo
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_CMC_ENABLE_REQ_V01, data_req,
              sizeof(sns_sam_cmc_enable_req_msg_v01), data_resp,
              sizeof(sns_sam_cmc_enable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    printf("CMC CLNT: qmi_client_send_msg_async returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}


/*=========================================================================
  FUNCTION: sns_plbk_sam_cmc_disable
=========================================================================*/
/*!
    @brief
    Disabble's a SAM CMC Algorithm by sending disable message.
*/
/*=======================================================================*/
int sns_plbk_sam_cmc_disable(qmi_client_type *clnt)
{
    sns_sam_cmc_disable_req_msg_v01 *data_req;
    sns_sam_cmc_disable_resp_msg_v01 *data_resp;
    qmi_txn_handle txn;
    int rc;

    /* Prepare and send the request */
    data_req = (sns_sam_cmc_disable_req_msg_v01*)
                               MALLOC(sizeof(sns_sam_cmc_disable_req_msg_v01));
    data_resp = (sns_sam_cmc_disable_resp_msg_v01*)
                              MALLOC(sizeof(sns_sam_cmc_disable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }
    memset( data_req, 0, sizeof(sns_sam_cmc_disable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_cmc_disable_resp_msg_v01) );

    data_req->instance_id = algo_instance_id;
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_CMC_DISABLE_REQ_V01, data_req,
               sizeof(sns_sam_cmc_disable_req_msg_v01), data_resp,
               sizeof(sns_sam_cmc_disable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}


/*=========================================================================
  FUNCTION: sns_plbk_sam_GYROBUF_enable
=========================================================================*/
/*!
    @brief
    Enable's a SAM GYROBUF Algorithm by sending enable message.
*/
/*=======================================================================*/
int sns_plbk_sam_gyrobuf_enable(int *algo_config, 
                            qmi_client_type *clnt)
{
    qmi_txn_handle txn;
    qmi_client_type notifier;
    unsigned int num_services, num_entries=10;
    qmi_cci_os_signal_type os_params;
    qmi_service_info service_info;
    qmi_idl_service_object_type service_object = 
                                      SNS_SAM_GYROBUF_SVC_get_service_object_v01();
    sns_sam_gyrobuf_enable_req_msg_v01 *data_req;
    sns_sam_gyrobuf_enable_resp_msg_v01 *data_resp;
    int rc;

    if (!service_object) {
        printf("GYROBUF CLNT: get_serivce_object failed\n");
    }

    memset(&os_params,0,sizeof(os_params));
    os_params.ext_signal = NULL;
    os_params.sig = QMI_CLNT_WAIT_SIG;
    os_params.timer_sig = QMI_CLNT_TIMER_SIG;

    rc = qmi_client_notifier_init(service_object, &os_params, &notifier);
    
    QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
    QMI_CCI_OS_SIGNAL_CLEAR(&os_params);
    
    /* The server has come up, store the information in info variable */
    rc = qmi_client_get_service_list(service_object, &service_info,
                                     &num_entries, &num_services);

    printf("GYROBUF CLNT: qmi_client_get_service_list() returned %d num_entries=%d"
           " num_services = %d\n", rc, num_entries, num_services);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    rc = qmi_client_init(&service_info, service_object, sam_ind_cb, NULL, 
                         &os_params, clnt);
    printf("GYROBUF CLNT: qmi_client_init returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    /* Prepare and send the request */
    data_req = (sns_sam_gyrobuf_enable_req_msg_v01*)
                                MALLOC(sizeof(sns_sam_gyrobuf_enable_req_msg_v01));
    data_resp = (sns_sam_gyrobuf_enable_resp_msg_v01*)
                               MALLOC(sizeof(sns_sam_gyrobuf_enable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }

    memset( data_req, 0, sizeof(sns_sam_gyrobuf_enable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_gyrobuf_enable_resp_msg_v01) );

    /* sam GYROBUF config */
    data_req->sample_rate =  algo_config[0];
    data_req->extra_sample = algo_config[1];
    //No mandatory config for this algo
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_GYROBUF_ENABLE_REQ_V01, data_req,
              sizeof(sns_sam_gyrobuf_enable_req_msg_v01), data_resp,
              sizeof(sns_sam_gyrobuf_enable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    printf("GYROBUF CLNT: qmi_client_send_msg_async returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}

/*=========================================================================
  FUNCTION: sns_plbk_sam_gyrobuf_disable
=========================================================================*/
/*!
    @brief
    Disabble's a SAM GYROBUF Algorithm by sending disable message.
*/
/*=======================================================================*/
int sns_plbk_sam_gyrobuf_disable(qmi_client_type *clnt)
{
    sns_sam_gyrobuf_disable_req_msg_v01 *data_req;
    sns_sam_gyrobuf_disable_resp_msg_v01 *data_resp;
    qmi_txn_handle txn;
    int rc;

    /* Prepare and send the request */
    data_req = (sns_sam_gyrobuf_disable_req_msg_v01*)
                               MALLOC(sizeof(sns_sam_gyrobuf_disable_req_msg_v01));
    data_resp = (sns_sam_gyrobuf_disable_resp_msg_v01*)
                              MALLOC(sizeof(sns_sam_gyrobuf_disable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }
    memset( data_req, 0, sizeof(sns_sam_gyrobuf_disable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_gyrobuf_disable_resp_msg_v01) );

    data_req->instance_id = algo_instance_id;
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_GYROBUF_DISABLE_REQ_V01, data_req,
               sizeof(sns_sam_gyrobuf_disable_req_msg_v01), data_resp,
               sizeof(sns_sam_gyrobuf_disable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}


/*=========================================================================
  FUNCTION: sns_plbk_sam_gyroint_enable
=========================================================================*/
/*!
    @brief
    Enable's a SAM GYROINT Algorithm by sending enable message.
*/
/*=======================================================================*/
int sns_plbk_sam_gyroint_enable(int *algo_config, 
                            qmi_client_type *clnt)
{
    qmi_txn_handle txn;
    qmi_client_type notifier;
    unsigned int num_services, num_entries=10;
    qmi_cci_os_signal_type os_params;
    qmi_service_info service_info;
    qmi_idl_service_object_type service_object = 
                                      SNS_SAM_GYROINT_SVC_get_service_object_v01();
    sns_sam_gyroint_enable_req_msg_v01 *data_req;
    sns_sam_gyroint_enable_resp_msg_v01 *data_resp;
    int rc;

    if (!service_object) {
        printf("GYROINT CLNT: get_serivce_object failed\n");
    }

    memset(&os_params,0,sizeof(os_params));
    os_params.ext_signal = NULL;
    os_params.sig = QMI_CLNT_WAIT_SIG;
    os_params.timer_sig = QMI_CLNT_TIMER_SIG;

    rc = qmi_client_notifier_init(service_object, &os_params, &notifier);
    
    QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
    QMI_CCI_OS_SIGNAL_CLEAR(&os_params);
    
    /* The server has come up, store the information in info variable */
    rc = qmi_client_get_service_list(service_object, &service_info,
                                     &num_entries, &num_services);

    printf("GYROINT CLNT: qmi_client_get_service_list() returned %d num_entries=%d"
           " num_services = %d\n", rc, num_entries, num_services);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    rc = qmi_client_init(&service_info, service_object, sam_ind_cb, NULL, 
                         &os_params, clnt);
    printf("GYROINT CLNT: qmi_client_init returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    /* Prepare and send the request */
    data_req = (sns_sam_gyroint_enable_req_msg_v01*)
                                MALLOC(sizeof(sns_sam_gyroint_enable_req_msg_v01));
    data_resp = (sns_sam_gyroint_enable_resp_msg_v01*)
                               MALLOC(sizeof(sns_sam_gyroint_enable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }

    memset( data_req, 0, sizeof(sns_sam_gyroint_enable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_gyroint_enable_resp_msg_v01) );

    /* sam GYROINT config */
    data_req->enable_angle = algo_config[0];
    data_req->enable_sample = algo_config[1];
    data_req->sample_rate =  algo_config[2];
    data_req->extra_sample = algo_config[3];
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_GYROINT_ENABLE_REQ_V01, data_req,
              sizeof(sns_sam_gyroint_enable_req_msg_v01), data_resp,
              sizeof(sns_sam_gyroint_enable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    printf("GYROINT CLNT: qmi_client_send_msg_async returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}


/*=========================================================================
  FUNCTION: sns_plbk_sam_gyroint_disable
=========================================================================*/
/*!
    @brief
    Disabble's a SAM GYROINT Algorithm by sending disable message.
*/
/*=======================================================================*/
int sns_plbk_sam_gyroint_disable(qmi_client_type *clnt)
{
    sns_sam_gyroint_disable_req_msg_v01 *data_req;
    sns_sam_gyroint_disable_resp_msg_v01 *data_resp;
    qmi_txn_handle txn;
    int rc;

    /* Prepare and send the request */
    data_req = (sns_sam_gyroint_disable_req_msg_v01*)
                               MALLOC(sizeof(sns_sam_gyroint_disable_req_msg_v01));
    data_resp = (sns_sam_gyroint_disable_resp_msg_v01*)
                              MALLOC(sizeof(sns_sam_gyroint_disable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }
    memset( data_req, 0, sizeof(sns_sam_gyroint_disable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_gyroint_disable_resp_msg_v01) );

    data_req->instance_id = algo_instance_id;
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_GYROINT_DISABLE_REQ_V01, data_req,
               sizeof(sns_sam_gyroint_disable_req_msg_v01), data_resp,
               sizeof(sns_sam_gyroint_disable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}
/*=========================================================================
  FUNCTION: sns_plbk_sam_game_rotation_vector_enable
=========================================================================*/
/*!
    @brief
    Enable's a SAM Game Rot Vec Algorithm by sending enable message.
*/
/*=======================================================================*/
int sns_plbk_sam_game_rotation_vector_enable(int *algo_config, 
                            qmi_client_type *clnt)
{
    qmi_txn_handle txn;
    qmi_client_type notifier;
    unsigned int num_services, num_entries=10;
    qmi_cci_os_signal_type os_params;
    qmi_service_info service_info;
    qmi_idl_service_object_type service_object = 
                                      SNS_SAM_GAME_ROTATION_VECTOR_SVC_get_service_object_v01();
    sns_sam_game_rotation_vector_enable_req_msg_v01 *data_req;
    sns_sam_game_rotation_vector_enable_resp_msg_v01 *data_resp;
    int rc;

    if (!service_object) {
        printf("GAME RV CLNT: get_serivce_object failed\n");
    }

    memset(&os_params,0,sizeof(os_params));
    os_params.ext_signal = NULL;
    os_params.sig = QMI_CLNT_WAIT_SIG;
    os_params.timer_sig = QMI_CLNT_TIMER_SIG;

    rc = qmi_client_notifier_init(service_object, &os_params, &notifier);
    
    QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
    QMI_CCI_OS_SIGNAL_CLEAR(&os_params);
    
    /* The server has come up, store the information in info variable */
    rc = qmi_client_get_service_list(service_object, &service_info,
                                     &num_entries, &num_services);

    printf("GAME RV CLNT: qmi_client_get_service_list() returned %d num_entries=%d"
           " num_services = %d\n", rc, num_entries, num_services);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    rc = qmi_client_init(&service_info, service_object, sam_ind_cb, NULL, 
                         &os_params, clnt);
    printf("GAME RV CLNT: qmi_client_init returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    }

    /* Prepare and send the request */
    data_req = (sns_sam_game_rotation_vector_enable_req_msg_v01*)
                                MALLOC(sizeof(sns_sam_game_rotation_vector_enable_req_msg_v01));
    data_resp = (sns_sam_game_rotation_vector_enable_resp_msg_v01*)
                               MALLOC(sizeof(sns_sam_game_rotation_vector_enable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }

    memset( data_req, 0, sizeof(sns_sam_game_rotation_vector_enable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_game_rotation_vector_enable_resp_msg_v01) );

    /* sam GAME RV config */
    data_req->report_period = algo_config[0];
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_GAME_ROTATION_VECTOR_ENABLE_REQ_V01, data_req,
              sizeof(sns_sam_game_rotation_vector_enable_req_msg_v01), data_resp,
              sizeof(sns_sam_game_rotation_vector_enable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    printf("GAME RV CLNT: qmi_client_send_msg_async returned %d\n", rc);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}



/*=========================================================================
  FUNCTION: sns_plbk_sam_rotation_vector_disable
=========================================================================*/
/*!
    @brief
    Disabble's a SAM GAME_ROTATION_VECTOR Algorithm by sending disable message.
*/
/*=======================================================================*/
int sns_plbk_sam_game_rotation_vector_disable(qmi_client_type *clnt)
{
    sns_sam_game_rotation_vector_disable_req_msg_v01 *data_req;
    sns_sam_game_rotation_vector_disable_resp_msg_v01 *data_resp;
    qmi_txn_handle txn;
    int rc;

    /* Prepare and send the request */
    data_req = (sns_sam_game_rotation_vector_disable_req_msg_v01*)
                               MALLOC(sizeof(sns_sam_game_rotation_vector_disable_req_msg_v01));
    data_resp = (sns_sam_game_rotation_vector_disable_resp_msg_v01*)
                              MALLOC(sizeof(sns_sam_game_rotation_vector_disable_resp_msg_v01));

    if (!data_req || !data_resp) {
        return -1;
    }
    memset( data_req, 0, sizeof(sns_sam_game_rotation_vector_disable_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_sam_game_rotation_vector_disable_resp_msg_v01) );

    data_req->instance_id = algo_instance_id;
  
    rc = qmi_client_send_msg_async(*clnt, SNS_SAM_GAME_ROTATION_VECTOR_DISABLE_REQ_V01, data_req,
               sizeof(sns_sam_game_rotation_vector_disable_req_msg_v01), data_resp,
               sizeof(sns_sam_game_rotation_vector_disable_resp_msg_v01), sam_rx_cb, NULL, &txn);
    if (rc != QMI_NO_ERR) {
        return -1;
    } 
    return 0;
}




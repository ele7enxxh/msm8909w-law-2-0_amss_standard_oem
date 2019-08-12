/******************************************************************************

                       N E T M G R _ Q M I _W D S . H

******************************************************************************/

/******************************************************************************

  @file    netmgr_qmi_wds.h
  @brief   Network Manager QMI wireless data service header

  DESCRIPTION
  Network Manager QMI wireless data service header

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#ifndef __NETMGR_QMI_WDS_H__
#define __NETMGR_QMI_WDS_H__
#include <stdint.h>
#include "qmi_client.h"
#include "wireless_data_service_v01.h"

typedef union
{
  wds_pkt_srvc_status_ind_msg_v01                   pkt_srvc_status;
  wds_event_report_ind_msg_v01                      event_report;
  wds_reverse_ip_transport_connection_ind_msg_v01   rev_ip_conn_ind;
  wds_extended_ip_config_ind_msg_v01                ext_ip_ind;
  wds_configured_throughput_info_ind_msg_v01        tput_info_ind;
} netmgr_qmi_wds_indication_data_type;

typedef enum
{
  NETMGR_WDS_REV_IP_CONFIG_COMPLETE_FAILURE,
  NETMGR_WDS_REV_IP_CONFIG_COMPLETE_SUCCESS
} netmgr_qmi_wds_rev_ip_config_result_type;

extern boolean netmgr_qmi_wds_err_cb_reg;

/*===========================================================================
  FUNCTION  netmgr_qmi_wds_init_client
===========================================================================*/
/*!
@brief
  Initialize QMI WDS handle

@return
  int - NETMGR_SUCCESS on successful operation,
        NETMGR_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int netmgr_qmi_wds_init_client
(
  int                      link,
  const char               *dev_str,
  wds_ip_family_enum_v01   ip_family,
  qmi_client_type          *clnt_hndl
);

/*===========================================================================
  FUNCTION  netmgr_qmi_wds_release_client
===========================================================================*/
/*!
@brief
  Release QMI WDS handle

@arg *dev_id Name of device to release the QMI WDS client

@return
  void

*/
/*=========================================================================*/
int netmgr_qmi_wds_release_client
(
  int                      link,
  qmi_client_type          clnt_hndl
);

/*===========================================================================
  FUNCTION  netmgr_qmi_wds_register_notifier
===========================================================================*/
/*!
@brief
  Release QMI WDS notifier handle

@return
  none
*/
/*=========================================================================*/
void netmgr_qmi_wds_register_notifier
(
  int                      link
);

/*===========================================================================
  FUNCTION  netmgr_qmi_wds_release_notifier
===========================================================================*/
/*!
@brief
  Release QMI WDS notifier handle

@return
  none
*/
/*=========================================================================*/
void netmgr_qmi_wds_release_notifier
(
  int                      link
);



/*===========================================================================
  FUNCTION  netmgr_qmi_wds_ext_ip_ind_reg
===========================================================================*/
/*!
@brief
  Register for extended IP indication

@return
  int - NETMGR_QMI_WDS_SUCCESS on successful operation,
        NETMGR_QMI_WDS_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int netmgr_qmi_wds_ext_ip_ind_reg
(
  int                      link,
  qmi_client_type          clnt_hndl
);

/*===========================================================================
  FUNCTION  netmgr_qmi_wds_config_tput_ind_reg
===========================================================================*/
/*!
@brief
  Register for configured throughput indications

@return
  int - NETMGR_SUCCESS on successful operation,
        NETMGR_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int netmgr_qmi_wds_config_tput_ind_reg
(
  int                      link,
  qmi_client_type          clnt_hndl
);


/*===========================================================================
  FUNCTION  netmgr_qmi_wds_bind_subscription
===========================================================================*/
/*!
@brief
  Bind the subscription to WDS_DONT_CARE_SUBS_V01

@return
  int - NETMGR_QMI_WDS_SUCCESS on successful operation,
        NETMGR_QMI_WDS_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int netmgr_qmi_wds_bind_subscription
(
  int                      link,
  qmi_client_type          clnt_hndl
);

/*===========================================================================
  FUNCTION  netmgr_qmi_wds_bearer_tech_ex_ind_reg
===========================================================================*/
/*!
@brief
  Register for bearer tech ex indications

@return
  int - NETMGR_QMI_WDS_SUCCESS on successful operation,
        NETMGR_QMI_WDS_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int netmgr_qmi_wds_bearer_tech_ex_ind_reg
(
  int                      link,
  qmi_client_type          clnt_hndl
);

/*===========================================================================
  FUNCTION  netmgr_qmi_wds_get_runtime_settings
===========================================================================*/
/*!
@brief
  Query the Modem for runtime settings

@return
  int - NETMGR_QMI_WDS_SUCCESS on successful operation,
        NETMGR_QMI_WDS_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int netmgr_qmi_wds_get_runtime_settings
(
  int                                    link,
  qmi_client_type                        clnt_hndl,
  uint32_t                               req_mask,
  wds_get_runtime_settings_resp_msg_v01  *runtime_set
);

#ifdef FEATURE_DATA_IWLAN
/*===========================================================================
  FUNCTION  netmgr_qmi_wds_rev_ip_ind_reg
===========================================================================*/
/*!
@brief
  Register for reverse IP transport indications

@return
  int - NETMGR_QMI_WDS_SUCCESS on successful operation,
        NETMGR_QMI_WDS_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int netmgr_qmi_wds_rev_ip_ind_reg
(
  int                      link,
  qmi_client_type          clnt_hndl
);

/*===========================================================================
  FUNCTION  netmgr_qmi_wds_rev_ip_config_complete
===========================================================================*/
/*!
@brief
  Report reverse adapter configuration result to the Modem

@return
  int - NETMGR_QMI_WDS_SUCCESS on successful operation,
        NETMGR_QMI_WDS_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int netmgr_qmi_wds_rev_ip_config_complete
(
  int                                       link,
  qmi_client_type                           clnt_hndl,
  uint32_t                                  txn_id,
  netmgr_qmi_wds_rev_ip_config_result_type  result
);

/*===========================================================================
  FUNCTION  netmgr_qmi_wds_get_ipsec_sa_config
===========================================================================*/
/*!
@brief
  Query the IPSec config from the Modem

@return
  int - NETMGR_QMI_WDS_SUCCESS on successful operation,
        NETMGR_QMI_WDS_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int netmgr_qmi_wds_get_ipsec_sa_config
(
  int                                          link,
  qmi_client_type                              clnt_hndl,
  wds_get_ipsec_static_sa_config_resp_msg_v01  *result
);

/*===========================================================================
  FUNCTION  netmgr_qmi_wds_initiate_esp_rekey
===========================================================================*/
/*!
@brief
  Initiate ESP re-key for the iWLAN call corresponding to the client handle

@return
  int - NETMGR_QMI_WDS_SUCCESS on successful operation,
        NETMGR_QMI_WDS_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int netmgr_qmi_wds_initiate_esp_rekey
(
  int              link,
  qmi_client_type  clnt_hndl
);
#endif /* FEATURE_DATA_IWLAN */

#endif /* __NETMGR_QMI_WDS_H__ */

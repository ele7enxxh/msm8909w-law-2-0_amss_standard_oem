/*===========================================================================

                         D S _ Q M I _ W D S _ U M T S . C

DESCRIPTION

 The Data Services Qualcomm Wireless Data Services MSM Interface source file.

EXTERNALIZED FUNCTIONS

  QMI_WDS_UMTS_GET_RUNTIME_SETTINGS()
    Get the runtime settings for a given UMTS profile.
    
Copyright (c) 2005-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE
                      
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/ds_qmi_wds_umts.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/10/12    sb     Added Len checks for QMI TLVs
10/09/09    ba     Win7 compliance modification.
05/22/09    pp     PDPREG API - CMI De-featurization impacts.
02/08/08    mct    Added support for IPv6 over UMTS.
11/02/07    ks     Added PCSCF addr using PCO flag to profiles.
10/15/07    ks     Added gateway address and mask to runtime settings.
05/28/07    ks     Added support for get_runtime_settings api
07/06/06    ks     Replaces qmi_wds_umtsi_get_tl() with qmi_svc_get_tl().
04/18/06    ks     Changed format of profile_list tlv to include 
                   num_instances.
04/10/06    ks     Fix profile name during create.
03/15/06    ks     Mapping auth preference got in Start Network Interface to 
                   umtsps auth type.
03/13/06    ks     Changed parsing of requests to return error for fixed 
                   length tlvs when tlv lengh passed is incorrect.
03/13/06    ks     Changed QMI_ERR_INVALID_ARG to QMI_ERR_MALFORMED_MSG.
03/09/06    ks     Removed header and data compression tlvs from the spec 
                   and code as umts doesn't read it from profiles currently.
03/06/06    ks     Expedite parsing in qmi_wds_umtsi_parse_profile_params() 
                   if a malformed TLV is seen.
01/17/05    mct    Fixed typo.
12/14/05    ks     Made changes in UMTS profile defn if IPV6 is defined.
11/30/05    ks     Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM) || defined (FEATURE_LTE) || defined(FEATURE_TDSCDMA)
#include "amssassert.h"
#include "dsm.h"

#include "ds_qmi_wds_umts.h"
#include "dsumtspdpreg.h"
#include "dsumtsps.h"
#include "ps_byte.h"
#include "ps_iface.h"
#include "ps_ifacei.h"
#include "ps_phys_link.h"
#include "ps_iface_ioctl.h"
#include "wireless_data_service_v01.h"
#include "ds_qmi_wds.h"
#include "ds_qmi_svc.h"
#include "ds_qmi_defs.h"

#include <stringl/stringl.h>


/*===========================================================================
            
                        EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_WDS_UMTS_GET_RUNTIME_SETTINGS()

  DESCRIPTION
    Get the runtime settings for a given UMTS profile.

  PARAMETERS
    req_settings     : mask of requesting settings
    resp_msg         : output buffer to fill the response
    iface_ptr        : the ps_iface pointer
    bearer_tech_type : the bearer technology type

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_wds_umts_get_runtime_settings
(
  uint32                                 req_settings,
  wds_get_runtime_settings_resp_msg_v01* resp_msg,
  ps_iface_type *                        iface_ptr,
#ifdef FEATURE_DATA_WLAN_MAPCON
  ds_sys_system_status_info_type         bearer_tech_ex_type
#else
  ps_iface_bearer_technology_type        bearer_tech_type
#endif /* FEATURE_DATA_WLAN_MAPCON */
)
{
  ps_iface_ioctl_3gpp_session_params_info_type  session_info;
  uint16 len;
  int return_val;
  sint15 ps_errno;
  boolean im_cn_flag;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(resp_msg == NULL)
  {
    return QMI_ERR_INTERNAL_V01;
  }
  if ( req_settings & QMI_WDS_MASK_IM_CN_FLAG )
  {
    return_val = ps_iface_ioctl(iface_ptr,
                                PS_IFACE_IOCTL_UMTS_GET_IM_CN_FLAG,
                                &im_cn_flag,
                                &ps_errno);
    if (return_val == -1) 
    {
      LOG_MSG_INFO1_0("Failed PS_IFACE_IOCTL_UMTS_GET_IM_CN_FLAG. Return rest of the available runtime settings");
    }
    else
    {
      resp_msg->im_cn_flag_valid = TRUE;
      resp_msg->im_cn_flag = im_cn_flag;
    }
  }

  return_val = ps_iface_ioctl(iface_ptr,
                                PS_IFACE_IOCTL_UMTS_GET_SESSION_PARAMS,
                                &session_info,
                                &ps_errno);
  if (return_val == -1) 
  {
    LOG_MSG_INFO1_0("Failed PS_IFACE_IOCTL_UMTS_GET_SESSION_PARAMS. Return rest of the available runtime settings");
  }
  else
  {
    if(req_settings & QMI_WDS_MASK_PCSCF_PCO)
    {
      resp_msg->pcscf_addr_using_pco_valid = TRUE;
      resp_msg->pcscf_addr_using_pco = session_info.request_pcscf_address_flag;
    }

    if(req_settings & QMI_WDS_MASK_GRANTED_QOS)
    {
#ifdef FEATURE_DATA_WLAN_MAPCON
      if(DS_SYS_RAT_EX_NULL_BEARER != bearer_tech_ex_type.rat_value)
      {
        if((DS_SYS_RAT_EX_3GPP_GERAN == bearer_tech_ex_type.rat_value) &&
           ((bearer_tech_ex_type.so_mask & DS_SYS_SO_EX_3GPP_GPRS) ||
            (bearer_tech_ex_type.so_mask & DS_SYS_SO_EX_3GPP_EDGE)))//GPRS
        {
          if(session_info.gprs_qos.valid_flg)
          {
            resp_msg->gprs_requested_qos_valid = TRUE;
            resp_msg->gprs_requested_qos.precedence_class = session_info.gprs_qos.precedence;
            resp_msg->gprs_requested_qos.delay_class = session_info.gprs_qos.delay;
            resp_msg->gprs_requested_qos.reliability_class = session_info.gprs_qos.reliability;
            resp_msg->gprs_requested_qos.mean_throughput_class = session_info.gprs_qos.mean;
            resp_msg->gprs_requested_qos.peak_throughput_class = session_info.gprs_qos.peak;
          }
        }
        else if (( bearer_tech_ex_type.rat_value == DS_SYS_RAT_EX_3GPP_LTE) ||
                 (bearer_tech_ex_type.rat_value == DS_SYS_RAT_EX_3GPP_WLAN))
        {
          LOG_MSG_ERROR_0("LTE/WLAN QoS not supported currently");
        }
#else
      if(PS_IFACE_3GPP_NULL_BEARER != bearer_tech_type.data.umts_type.rat_mask)
      {
        if ((bearer_tech_type.data.umts_type.rat_mask == PS_IFACE_UMTS_GPRS) ||
            (bearer_tech_type.data.umts_type.rat_mask == PS_IFACE_UMTS_EDGE)) //GPRS
        {
          if(session_info.gprs_qos.valid_flg)
          {
            resp_msg->gprs_requested_qos_valid = TRUE;
            resp_msg->gprs_requested_qos.precedence_class = session_info.gprs_qos.precedence;
            resp_msg->gprs_requested_qos.delay_class = session_info.gprs_qos.delay;
            resp_msg->gprs_requested_qos.reliability_class = session_info.gprs_qos.reliability;
            resp_msg->gprs_requested_qos.mean_throughput_class = session_info.gprs_qos.mean;
            resp_msg->gprs_requested_qos.peak_throughput_class = session_info.gprs_qos.peak;
          }
        }
        else if ( bearer_tech_type.data.umts_type.rat_mask == PS_IFACE_3GPP_LTE)
        {
          LOG_MSG_ERROR_0("LTE QoS not supported currently");
        }
#endif /* FEATURE_DATA_WLAN_MAPCON */
        else //UMTS/TDSCDMA
        {
          if(session_info.umts_qos.valid_flg)
          {
            resp_msg->umts_requested_qos_valid = TRUE;
            resp_msg->umts_requested_qos.traffic_class = 
                      (wds_traffic_class_enum_v01)session_info.umts_qos.traffic_class;
            resp_msg->umts_requested_qos.max_uplink_bitrate = 
                      session_info.umts_qos.max_ul_bitrate;
            resp_msg->umts_requested_qos.max_downlink_bitrate = 
                      session_info.umts_qos.max_dl_bitrate;
            resp_msg->umts_requested_qos.guaranteed_uplink_bitrate = 
                      session_info.umts_qos.gtd_ul_bitrate;
            resp_msg->umts_requested_qos.guaranteed_downlink_bitrate =
                      session_info.umts_qos.gtd_dl_bitrate;
            resp_msg->umts_requested_qos.qos_delivery_order = 
                      (wds_qos_delivery_order_enum_v01)session_info.umts_qos.dlvry_order;			
            resp_msg->umts_requested_qos.max_sdu_size = 
                      session_info.umts_qos.max_sdu_size;
            resp_msg->umts_requested_qos.sdu_error_ratio = 
                      (wds_sdu_error_ratio_enum_v01)session_info.umts_qos.sdu_err;
            resp_msg->umts_requested_qos.residual_bit_error_ratio = 
                      (wds_residual_bit_error_ratio_enum_v01)session_info.umts_qos.res_biterr;
            resp_msg->umts_requested_qos.delivery_erroneous_SDUs = 
                     (wds_delivery_erroneous_SDUs_enum_v01)session_info.umts_qos.dlvr_err_sdu;
            resp_msg->umts_requested_qos.transfer_delay = 
                      session_info.umts_qos.trans_delay;
            resp_msg->umts_requested_qos.traffic_handling_priority = 
                      session_info.umts_qos.thandle_prio;
          }
        }
      }
    }

    if( req_settings & QMI_WDS_MASK_USERNAME)
    {
      /* Make sure that the string is NULL terminated */
      session_info.auth.username[sizeof(session_info.auth.username) -1] = '\0';
      len = strlen((char *)session_info.auth.username);
      if(len != 0)
      {
        resp_msg->username_valid = TRUE;
        memscpy((void*)resp_msg->username,sizeof(resp_msg->username),
                 (void *)session_info.auth.username,len);
      }
    }

    if(req_settings & QMI_WDS_MASK_AUTH_PROTO)
    {
      resp_msg->authentication_preference_valid = TRUE;
      resp_msg->authentication_preference = session_info.auth.auth_type;
    }

    if(req_settings & QMI_WDS_MASK_APN)
    {
      /* Make sure that the string is NULL terminated */
      session_info.apn[sizeof(session_info.apn) - 1] = '\0';
      len = strlen((char *)session_info.apn);
      if(0 != len)
      {
        resp_msg->apn_name_valid = TRUE;
        memscpy((void*)resp_msg->apn_name,sizeof(resp_msg->apn_name),
                (void *)session_info.apn,len);
      }
    }

    if(req_settings & QMI_WDS_MASK_PDP_TYPE)
    {
      resp_msg->pdp_type_valid = TRUE;
      resp_msg->pdp_type = (wds_pdp_type_enum_v01)session_info.pdp_type;
    }

    if(req_settings & QMI_WDS_MASK_PROFILE_NAME)
    {
      /* Make sure the string is NULL terminated */ 
      session_info.profile_name[sizeof(session_info.profile_name) - 1] = '\0';
      len = strlen((char *)session_info.profile_name);
      if(0 != len)
      {
        resp_msg->profile_name_valid = TRUE;
        memscpy((void*)resp_msg->profile_name,sizeof(resp_msg->profile_name),
                (void *)session_info.profile_name,len);
      }
    }

    if(req_settings & QMI_WDS_MASK_PROFILE_ID)
    {
      resp_msg->profile_valid = TRUE;
      resp_msg->profile.profile_type = WDS_PROFILE_TYPE_3GPP;
      /* Remove the upper byte (profile family) from the profile number */
      resp_msg->profile.profile_index = (uint8)session_info.profile_number & 0x00FF;
    }
  } /* if (return_val == -1) */
  return QMI_ERR_NONE_V01;
  }/* qmi_wds_umts_get_runtime_settings */

#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined (FEATURE_LTE) || defined(FEATURE_TDSCDMA)*/

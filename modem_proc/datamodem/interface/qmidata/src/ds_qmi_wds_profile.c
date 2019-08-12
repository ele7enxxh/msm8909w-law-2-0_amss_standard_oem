/*===========================================================================
 
                         D S _ Q M I _ W D S _ P R O F I L E . C
 
DESCRIPTION
 
 The Data Services Qualcomm Wireless Data Services MSM Interface source file.
 
EXTERNALIZED FUNCTIONS
 
  QMI_WDS_PROFILE_MODIFY_SETTINGS()
    Modify profile settings 
  
  QMI_WDS_PROFILE_GET_SETTINGS()
    Get the settings for a given profile.
  
  QMI_WDS_PROFILE_GET_DEFAULT_SETTINGS()
    Get the settings of default profile.
  
  QMI_WDS_PROFILE_GET_LIST()
    Get the Profile list
  
  QMI_WDS_PROFILE_GET_DEFAULT_PROFILE_NUM()
    Get the default profile number.
  
  QMI_WDS_PROFILE_SET_DEFAULT_PROFILE_NUM()
    Set the default profile number.
  
  QMI_WDS_PROFILE_RESET_PROFILE_TO_DEFAULT()
    Reset the profile parameters to default.
  
  QMI_WDS_PROFILE_RESET_PARAM_TO_INVALID()
    Reset a given profile parameter to invalid.
  
  QMI_WDS_PROFILE_CREATE()
    Create a new Profile.
  
  QMI_WDS_PROFILE_DELETE()
    Delete a profile.
  
  QMI_WDS_PROFILE_UMTS_VALIDATE_PDP_TYPE()
    Validate PDP Type for a UMTS Profile.
  
 QMI_WDS_PROFILE_RETRIEVE_DEFAULT_PROFILE_NUM()
    Get the default profile number value. 
  
Copyright (c) 2009-2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/ds_qmi_wds_profile.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
4/15/12     ab     Convert IPV4 addresses to big endian for 3GPP/3GPP2 profiles
12/19/12    ab     Add PDN Throttling Timer 3GPP2 profile parameters  
12/19/12    ab     Add Disallow Timer 3GPP2 profile parameters 
12/04/12    svj    Add PCO related 3gpp and 3gpp2 profile parameters 
10/25/12    svj    Add support emergency calls 3GPP profile parameter 
09/10/12    sb     Added Len checks for QMI TLVs
04/16/12    wc     Add APN Beare 3GPP profile parameter
11/29/11    wc     Add PDN level auth/PDN label 3GPP2 profile parameters
05/25/11    wc     Support persistent/non-persistent profile creation
12/17/10    kk     Fix to update the valid fields mask after updating 
                   parameters from the default profile.
06/21/10    kk     Fix to validate parameters in MODIFY_PROFILE request.
04/26/09    rt     Created Module.
===========================================================================*/

/*===========================================================================
 
  INCLUDE FILES FOR MODULE
 
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "amssassert.h"

#include "ds_qmi_wds_profile.h"

#include "ps_byte.h"

#include "ds_qmi_wds.h"
#include "ds_qmi_svc.h"

#include "ds_profile.h"
#include "qmi_idl_lib.h"
#include "common_v01.h"
#include "wireless_data_service_v01.h"

//#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM) || defined(FEATURE_LTE)
  #include "ds_profile_3gpp.h"
//#endif /* defined (FEATURE_WCDMA) || defined (FEATURE_GSM) || defined(FEATURE_LTE)*/

//#ifdef FEATURE_CDMA
#include "ds_profile_3gpp2.h"
//#endif /* FEATURE_CDMA */
#include "ds_Utils_DebugMsg.h"
#include "ps_system_heap.h"

#include <stringl/stringl.h>
#include "ps_utils.h"

/*===========================================================================
                              INTERNAL MACROS                                
===========================================================================*/

/*---------------------------------------------------------------------------
  Maximum length of any profile parameter value.
---------------------------------------------------------------------------*/
#define   WDS_MAX_PROFILE_PARAM_VAL_LEN    150

#define WDSI_PRM_TYPE_EXT_ERROR_INFO_LEN         (0x02)

/*---------------------------------------------------------------------------
 Length of Header for Profile Lists. 
 Type (3GPP, 3GPP2), Profile Number, Length of name.
---------------------------------------------------------------------------*/
#define   PROFILE_LIST_PARAM_HDR_LENGTH  3

/*---------------------------------------------------------------------------
  Macro used for error handling.
---------------------------------------------------------------------------*/
#define CHECK_RETVAL()  if (FALSE == retval) { dsm_free_packet(&response); \
                                               return NULL; }

/*===========================================================================
                                 DEFINITIONS                                 
===========================================================================*/

//#ifdef FEATURE_CDMA
typedef struct
{
  ds_profile_3gpp2_param_enum_type   param_type;
  uint16                            param_len;  
}qmi_profile_3gpp2_param_tl_list_type;

/* Param length for Strings is 0*/

static const qmi_profile_3gpp2_param_tl_list_type qmi_profile_3gpp2_param_tl_list[] = 
{
  /*0x90*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_NEGOTIATE_DNS_SERVER, 
    sizeof(boolean) },
  
  /*0x91*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_SESSION_CLOSE_TIMER_DO, 
    sizeof(uint32) },
  
  /*0x92*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_SESSION_CLOSE_TIMER_1X, 
    sizeof(uint32) },
  
  /*0x93*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_ALLOW_LINGER, 
    sizeof(boolean) },
  
  /*0x94*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_LCP_ACK_TIMEOUT, 
    sizeof(uint16) },
  
  /*0x95*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_IPCP_ACK_TIMEOUT, 
    sizeof(uint16) },
  
  /*0x96*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_AUTH_TIMEOUT, 
    sizeof(uint16) },
  
  /*0x97*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_LCP_CREQ_RETRY_COUNT, 
    sizeof(uint8) },
  
  /*0x98*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_IPCP_CREQ_RETRY_COUNT, 
    sizeof(uint8) },
  
  /*0x99*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_AUTH_RETRY_COUNT, 
    sizeof(uint8) },
  
  /*0x9A*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_AUTH_PROTOCOL, 
    sizeof(uint8) },
  
  /*0x9B*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_USER_ID,
    0 },
  
  /*0x9C*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_AUTH_PASSWORD,
    0 },
  
  /*0x9D*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_DATA_RATE, 
    sizeof(uint8) },
  
  /*0x9E*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_DATA_MODE, 
    sizeof(uint8) },
  
  /*0x9F*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_APP_TYPE, 
    sizeof(uint32) },
  
  /*0xA0*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_APP_PRIORITY, 
    sizeof(uint8) },
  
  /*0xA1*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_APN_STRING, 
    0 },
  
  /*0xA2*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_TYPE, 
    sizeof(ds_profile_3gpp2_pdn_type_enum_type) },
  
  /*0xA3*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_IS_PCSCF_ADDR_NEEDED, 
    sizeof(boolean) },
  
  /*0xA4*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_V4_DNS_ADDR_PRIMARY,
    sizeof(ds_profile_3gpp2_in_addr_type) },
  
  /*0xA5*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_V4_DNS_ADDR_SECONDARY, 
    sizeof(ds_profile_3gpp2_in_addr_type) },
  
  /*0xA6*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_V6_DNS_ADDR_PRIMARY,
    sizeof(ds_profile_3gpp2_in6_addr_type) },
  
  /*0xA7*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_V6_DNS_ADDR_SECONDARY, 
    sizeof(ds_profile_3gpp2_in6_addr_type) },
  
  /*0xA8*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_RAT_TYPE, 
    sizeof(ds_profile_3gpp2_rat_type_enum_type) },

  /*0xA9*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_APN_ENABLED, 
    sizeof(boolean) },

  /*0xAA*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_INACTIVITY_TIMEOUT, 
    sizeof(uint32) },

  /*0xAB*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_APN_CLASS, 
    sizeof(uint8) },

  /*0xAD*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_LEVEL_AUTH_PROTOCOL, 
    sizeof(uint8) },

  /*0xAE*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_LEVEL_USER_ID, 
    0 },

  /*0xAF*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_LEVEL_AUTH_PASSWORD, 
    0 },

  /*0xB0*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_LABEL, 
    0 },

  /*0xBD*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_OP_PCO_ID, 
    sizeof(uint16) },

  /*0xBE*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_MCC, 
    sizeof(uint16) },

  /*0xBF*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_MNC, 
    sizeof(ds_profile_3gpp2_mnc_type) },

  /*0xC0*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_FAILURE_TIMERS,
    sizeof(ds_profile_3gpp2_failure_timers_type) },

  /*0xC1*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_DISALLOW_TIMERS,
    sizeof(ds_profile_3gpp2_failure_timers_type) },

  /*0xC2*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_USER_APP_DATA,
    sizeof(ds_profile_3gpp2_user_app_data) },

  /*0xC3*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_PCSCF_DHCP_REQ_FLAG,
    sizeof(ds_profile_3gpp2_request_pcscf_address_using_dhcp_flag_type) },

  /*0xC4*/
  { DS_PROFILE_3GPP2_PROFILE_PARAM_DNS_DHCP_REQ_FLAG,
    sizeof(ds_profile_3gpp2_request_dns_address_using_dhcp_flag_type) }
};
//#endif /* FEATURE_CDMA */

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM) || defined(FEATURE_LTE) || defined(FEATURE_TDSCDMA)
typedef struct
{
  ds_profile_3gpp_param_enum_type   param_type;
  uint16                            param_len;  
}qmi_profile_3gpp_param_tl_list_type;

typedef struct
{
  ds_profile_tech_common_param_enum_type   param_type;
  uint16                                   param_len;  
}qmi_profile_common_param_tl_list_type;

typedef struct
{
  ds_profile_tech_general_param_enum_type   param_type;
  uint16                                    param_len;  
}qmi_profile_general_param_tl_list_type;

typedef PACKED struct PACKED_POST
{
  byte                                 filter_id;    
  byte                                 eval_prec_id; 
  ds_profile_3gpp_ip_version_enum_type ip_version; 
  ds_profile_3gpp_tft_addr_type        address;
  uint8                                mask;       
  byte                                 prot_num;    
  ds_profile_3gpp_port_range_type      dest_port_range; 
  ds_profile_3gpp_port_range_type      src_port_range;  
  uint32                               ipsec_spi;  
  uint16                               tos_mask;    
  uint32                               flow_label;      
}qmi_profile_tft_params_type;

/* Param length for Strings is 0*/
static const qmi_profile_3gpp_param_tl_list_type qmi_profile_3gpp_param_tl_list[] = 
{
  /*0x10*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_PROFILE_NAME, 
    0 },
  
  /*0x11*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE,
    sizeof (ds_profile_3gpp_pdp_type_enum_type) },
  
  /*0x12*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_H_COMP,
    sizeof (ds_profile_3gpp_pdp_header_comp_e_type) },
  
  /*0x13*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_D_COMP,
    sizeof(ds_profile_3gpp_pdp_data_comp_e_type) },
  
  /*0x14*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN,
    0 },
  
  /*0x15*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_DNS_ADDR_V4_PRIMARY,
    sizeof(uint32) },
  
  /*0x16*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_DNS_ADDR_V4_SECONDARY,
    sizeof(uint32) },
  
  /*0x17*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_REQ_QOS_EXTENDED,
    33 },
  
  /*0x18*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_MIN_QOS_EXTENDED,
    33 },
  
  /*0x19*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_GPRS_REQ_QOS,
    sizeof(ds_profile_3gpp_gprs_qos_params_type) },
  
  /*0x1A*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_GPRS_MIN_QOS,
    sizeof(ds_profile_3gpp_gprs_qos_params_type) },

  /*0x1B*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_USERNAME,
    0},
  
  /*0x1C*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_PASSWORD,
    0},

  /*0x1D*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_TYPE,
    sizeof (ds_profile_3gpp_auth_pref_type) },

  /*0x1E*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_ADDR_V4,
    sizeof(uint32) },
  
  /*0x1F*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_PCSCF_REQ_FLAG,
    sizeof(ds_profile_3gpp_request_pcscf_address_flag_type) },

  /*0x20*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_TE_MT_ACCESS_CTRL_FLAG,
    sizeof(ds_profile_3gpp_pdp_access_control_e_type) },
  
  /*0x21*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_PCSCF_DHCP_REQ_FLAG,
    sizeof(ds_profile_3gpp_request_pcscf_address_using_dhcp_flag_type) },
  
  /*0x22*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_IM_CN_FLAG,
    sizeof(ds_profile_3gpp_im_cn_flag_type) },
  
  /*0x23*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_TFT_FILTER_ID1,
    sizeof(qmi_profile_tft_params_type) },
  
  /*0x24*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_TFT_FILTER_ID2,
    sizeof(qmi_profile_tft_params_type) },
  
  /*0x25*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_NUMBER,
    sizeof(byte) },
  
  /*0x26*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_SECONDARY_FLAG,
    sizeof(boolean) },
  
  /*0x27*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PRIMARY_ID,
    sizeof(byte) },
  
  /*0x28*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_ADDR_V6,
    sizeof(ds_profile_3gpp_pdp_addr_type_ipv6) },
  
  /*0x29*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_REQ_QOS,
    34 },
  
  /*0x2A*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_MIN_QOS,
    34 },
  
  /*0x2B*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_DNS_ADDR_V6_PRIMARY,
    sizeof(ds_profile_3gpp_pdp_addr_type_ipv6) },
  
  /*0x2C*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_DNS_ADDR_V6_SECONDARY,
    sizeof(ds_profile_3gpp_pdp_addr_type_ipv6) },

  /*0x2D*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_IPV4_ADDR_ALLOC,
    sizeof(ds_profile_3gpp_pdp_ipv4_addr_alloc_e_type) },

  /*0x2E*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_LTE_REQ_QOS,
    sizeof(ds_profile_3gpp_lte_qos_params_type) },

  /*0x2F*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_APN_DISABLE_FLAG,
    sizeof(boolean) },

  /*0x30*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_INACTIVITY_TIMER_VAL,
    sizeof(uint32) },

  /*0x31*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_APN_CLASS,
    sizeof(uint8) },

  /*0x35*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_APN_BEARER,
    sizeof(uint64) },

  /*0x36*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_EMERGENCY_CALLS_SUPPORTED,
    sizeof(boolean) },

  /*0x37*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_OPERATOR_RESERVED_PCO,
    sizeof(uint16) },

  /*0x38*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_MCC,
    sizeof(uint16) },

  /*0x39*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_MNC,
    sizeof(ds_profile_3gpp_mnc_type) },

  /*0x3A*/
  { DS_PROFILE_3GPP_PROFILE_MAX_PDN_CONN_PER_BLOCK,
    sizeof(uint16) },
  
  /*0x3B*/
  { DS_PROFILE_3GPP_PROFILE_MAX_PDN_CONN_TIMER,
    sizeof(uint16) },
  
  /*0x3C*/
  { DS_PROFILE_3GPP_PROFILE_PDN_REQ_WAIT_TIMER,
    sizeof(uint16) },

  /*0x3D*/
  { DS_PROFILE_3GPP_PROFILE_USER_APP_DATA,
    sizeof(ds_profile_3gpp_user_app_data) },

  /*0x3E*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_ROAMING_DISALLOWED,
    sizeof(boolean) },

  /*0x3F*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_PDN_DISCON_WAIT_TIME,
    sizeof(uint8) },

  /*0x40*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_DNS_DHCP_REQ_FLAG,
    sizeof(ds_profile_3gpp_request_dns_address_using_dhcp_flag_type) },

  /*0x41*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_LTE_ROAMING_PDP_TYPE,
    sizeof(uint32) },

  /*0x42*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_ROAMING_PDP_TYPE,
    sizeof(uint32) },

  /*0x43*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_IWLAN_TO_LTE_ROAMING_HO_FLAG,
    sizeof(boolean) },

  /*0x44*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_LTE_TO_IWLAN_ROAMING_HO_FLAG,
    sizeof(boolean) },

  /*0x45*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_FAILURE_TIMERS,
    sizeof(ds_profile_3gpp_failure_timers_type) },

  /*0x46*/
  { DS_PROFILE_3GPP_PROFILE_PARAM_OVERRIDE_HOME_PDP_TYPE,
    sizeof(uint32) }
};
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined(FEATURE_LTE) || defined(FEATURE_TDSCDMA)*/


/* Param length for Strings is 0. Common params start from 0x8F and go backwards up.*/
static const qmi_profile_common_param_tl_list_type qmi_profile_common_param_tl_list[] = 
{
   /*0x7D*/
   { DS_PROFILE_TECH_COMMON_PROFILE_PARAM_PCSCF_DHCP_REQ_FLAG,
     sizeof(ds_profile_req_dhcp_pcscf_address_flag_type) },

   /*0x7E*/
   { DS_PROFILE_TECH_COMMON_PROFILE_PARAM_DNS_DHCP_REQ_FLAG,
     sizeof(ds_profile_req_dhcp_dns_address_flag_type) },

  /*0x7F*/
  { DS_PROFILE_TECH_COMMON_PROFILE_PARAM_PDP_TYPE,
    sizeof(uint32) },
  
  /*0x80*/
  { DS_PROFILE_TECH_COMMON_PROFILE_PARAM_USER_APP_DATA,
    sizeof(ds_profile_3gpp_user_app_data) },

  /*0x81*/
  { DS_PROFILE_TECH_COMMON_PROFILE_PARAM_MNC,
    sizeof(ds_profile_mnc_type) },


  /*0x82*/
  { DS_PROFILE_TECH_COMMON_PROFILE_PARAM_MCC,
    sizeof(ds_profile_mcc_type) },

  /*0x83*/
  { DS_PROFILE_TECH_COMMON_PROFILE_PARAM_OPERATOR_RESERVED_PCO,
    sizeof(ds_profile_op_pco_id_type) },

  /*0x84*/
  { DS_PROFILE_TECH_COMMON_PROFILE_PARAM_AUTH_PASSWORD,
    0},

  /*0x85*/
  { DS_PROFILE_TECH_COMMON_PROFILE_PARAM_USER_ID,
    0},

  /*0x86*/
  { DS_PROFILE_TECH_COMMON_PROFILE_PARAM_AUTH_PROTOCOL, 
    sizeof(ds_profile_auth_pref_type) },

  /*0x87*/
  { DS_PROFILE_TECH_COMMON_PROFILE_PARAM_REQUEST_PCSCF_REQ_FLAG,
    sizeof(ds_profile_is_pcscf_req_flag_type) },

  /*0x88*/
  { DS_PROFILE_TECH_COMMON_PROFILE_PARAM_LINGER_PARAMS, 
    sizeof(ds_profile_linger_params_type) },

  /*0x89*/
  { DS_PROFILE_TECH_COMMON_PROFILE_PARAM_V6_DNS_ADDR_SECONDARY, 
    sizeof(ds_profile_in6_addr_type) },

  /*0x8A*/
  { DS_PROFILE_TECH_COMMON_PROFILE_PARAM_V6_DNS_ADDR_PRIMARY,
    sizeof(ds_profile_in6_addr_type) },

  /*0x8B*/
  { DS_PROFILE_TECH_COMMON_PROFILE_PARAM_V4_DNS_ADDR_SECONDARY, 
    sizeof(ds_profile_in_addr_type) },

  /*0x8C*/
  { DS_PROFILE_TECH_COMMON_PROFILE_PARAM_V4_DNS_ADDR_PRIMARY,
    sizeof(ds_profile_in_addr_type) },

  /*0x8D*/
  { DS_PROFILE_TECH_COMMON_PROFILE_PARAM_APN_CLASS,
    sizeof(ds_profile_apn_class_type) },

  /*0x8E*/
  { DS_PROFILE_TECH_COMMON_PROFILE_PARAM_APN_DISABLED,
    sizeof(ds_profile_apn_disable_flag_type) }
};

/* Param length for Strings is 0*/
static const qmi_profile_general_param_tl_list_type qmi_profile_general_param_tl_list[] = 
{
  /*0XDE*/
  { DS_PROFILE_GENERAL_PROFILE_PARAM_CLAT_ENABLE_FLAG, 
   sizeof(boolean) },

  /*0XDF*/
  { DS_PROFILE_GENERAL_PROFILE_PARAM_IPV6_PREFIX_DELEGATION_FLAG, 
   sizeof(boolean) }
};

/*===========================================================================
 
                    INTERNAL FUNCTION DEFINITIONS
 
===========================================================================*/

/*===========================================================================
  FUNCTION QMI_WDS_PROFILE_GET_PARAM_LEN()
 
  DESCRIPTION
    Get the max length of the parameter value 
 
  PARAMETERS
    profile_param_id : Parameter identifier.
    tech_type        : Tech type of the parameter (3GPP/3GPP2)
    param_len        : Maximum Length of the param.
 
  RETURN VALUE
    0 (QMI_ERR_NONE_V01) : Success.
    Any other value  : Failure
  
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_wds_profile_get_param_len
(
 ds_profile_identifier_type   profile_param_id,
 uint8                        tech_type,
 uint16                     * param_len
)
{
  uint8     array_size;
  uint8     idx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  array_size = 0;

  /* Check if this is a general param */
  array_size = sizeof(qmi_profile_general_param_tl_list)/
                 sizeof(qmi_profile_general_param_tl_list_type);
  for(idx=0; idx < array_size; idx++ )
  {
    if(qmi_profile_general_param_tl_list[idx].param_type == 
        profile_param_id )
    {
      *param_len = qmi_profile_general_param_tl_list[idx].param_len;
      return QMI_ERR_NONE_V01;
    }
  }

/* Param length for Strings is 0*/

  if(tech_type == WDS_PROFILE_TYPE_EPC)
  {
    array_size = sizeof(qmi_profile_common_param_tl_list)/
                   sizeof(qmi_profile_common_param_tl_list_type);
    for(idx=0; idx < array_size; idx++ )
    {
      if(qmi_profile_common_param_tl_list[idx].param_type == 
          profile_param_id )
      {
        *param_len = qmi_profile_common_param_tl_list[idx].param_len;
        return QMI_ERR_NONE_V01;
      }
    }
  }

  if((tech_type == WDS_PROFILE_TYPE_3GPP) || (tech_type == WDS_PROFILE_TYPE_EPC))
  {
    #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined(FEATURE_LTE) || defined(FEATURE_TDSCDMA)
      array_size = sizeof(qmi_profile_3gpp_param_tl_list)/
                     sizeof(qmi_profile_3gpp_param_tl_list_type);
      for(idx=0; idx < array_size; idx++ )
      {
        if(qmi_profile_3gpp_param_tl_list[idx].param_type == 
            profile_param_id )
        {
          *param_len = qmi_profile_3gpp_param_tl_list[idx].param_len;
          return QMI_ERR_NONE_V01;
        }
      }
    #endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined(FEATURE_LTE) || defined(FEATURE_TDSCDMA)*/
  }

  if((tech_type == WDS_PROFILE_TYPE_3GPP2) || tech_type == WDS_PROFILE_TYPE_EPC)
  {
      array_size = sizeof(qmi_profile_3gpp2_param_tl_list)/
                     sizeof(qmi_profile_3gpp2_param_tl_list_type);
      for(idx=0; idx < array_size; idx++ )
      {
        if(qmi_profile_3gpp2_param_tl_list[idx].param_type == 
            profile_param_id )
        {
          *param_len = qmi_profile_3gpp2_param_tl_list[idx].param_len;
          return QMI_ERR_NONE_V01;
        }
      }
  }

  else
  {
    LOG_MSG_ERROR("Invalid Technology Type %d", tech_type, 0, 0);
    return QMI_ERR_INVALID_PROFILE_TYPE_V01;
  }

  LOG_MSG_ERROR_1("No match found for param id %d", profile_param_id);
  return QMI_ERR_MALFORMED_MSG_V01;
  
} /* ds_qmi_wds_profile_get_param_len() */


/*===========================================================================
  FUNCTION QMI_WDS_PROFILE_GET_PARAM_ID_INFO()
 
  DESCRIPTION
    Get the settings for a parameter for a given profile.
 
  PARAMETERS
    profile_type     : profile type (3GPP/3GPP2)
    profile_index    : profile whose settings are to be retrieved
 
  RETURN VALUE
    ptr to response
 
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
 ==========================================================================*/
qmi_error_type_v01 qmi_wds_profile_get_param_id_info
(
  uint8                        profile_type,
  uint8                        profile_index,
  ds_profile_identifier_type   profile_identifier,
  uint32                       subs_id,
  ds_profile_info_type        *param_info
)
{
  qmi_error_type_v01                errval;
  ds_profile_status_etype           profile_status;
  ds_profile_hndl_type              profile_hndl;
  uint16                            param_len;
  boolean                           cancel_txn;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  memset(&profile_hndl, 0, sizeof(profile_hndl));
  errval      = QMI_ERR_NONE_V01;
  profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
  cancel_txn = FALSE;
  param_len = 0;

  /*-------------------------------------------------------------------------
    Begin transaction
  -------------------------------------------------------------------------*/
  profile_status = 
    ds_profile_begin_transaction_per_sub(DS_PROFILE_TRN_READ,
                                 (ds_profile_tech_etype)profile_type,
                                 (ds_profile_num_type)profile_index,
                                 (ds_profile_subs_etype)subs_id,
                                 &profile_hndl);
  if (profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    LOG_MSG_ERROR_1 ("Begin Transaction failed. Error %d", profile_status);
    return QMI_ERR_INTERNAL_V01;
  }

  /*-----------------------------------------------------------------------
    Retrieve profile_identifier param info
  -----------------------------------------------------------------------*/
  profile_status = ds_profile_get_param(profile_hndl, 
                                        profile_identifier, 
                                        param_info);
  if (profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    LOG_MSG_ERROR_3("Cannot get Profile param %d for profile number %d Error %d",
                    profile_identifier, profile_index, profile_status);
    cancel_txn = TRUE;
    goto send_result;
  }

  /*-----------------------------------------------------------------------
    Verify that the return length is same as expected length for fixed
    length identifiers.
  -----------------------------------------------------------------------*/
  errval = qmi_wds_profile_get_param_len(profile_identifier, profile_type, 
                                         &param_len);
  if (errval != QMI_ERR_NONE_V01)
  {
    LOG_MSG_ERROR_1("Cannot get the Length for profile param %d",
                    profile_identifier);
    ASSERT(0);
  }
  else if ((param_len != 0 && param_len != param_info->len))
  {
    LOG_MSG_ERROR_3("Invalid Length. Identifier(%d) Expected(%d) Got(%d)",
                    profile_identifier, param_len, param_info->len);
    cancel_txn = TRUE;
    goto send_result;
  }
    
  /*-------------------------------------------------------------------------
    End transaction
  -------------------------------------------------------------------------*/
  profile_status = ds_profile_end_transaction(profile_hndl, 
                                              DS_PROFILE_ACTION_COMMIT);
  if(profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    LOG_MSG_ERROR_1 ("End Transaction failed. Error %d", profile_status);
    return QMI_ERR_INTERNAL_V01;
  }

send_result:
  if (cancel_txn == TRUE)
  {
    (void)ds_profile_end_transaction(profile_hndl, 
                                     DS_PROFILE_ACTION_CANCEL);
    return QMI_ERR_INTERNAL_V01;
  }
  
  return QMI_ERR_NONE_V01;
} /* qmi_wds_profile_get_param_id_info() */


/*===========================================================================
 
                      EXTERNAL FUNCTION DEFINITIONS
 
===========================================================================*/

/*===========================================================================
  FUNCTION QMI_WDS_PROFILE_MODIFY_SETTINGS()
 
  DESCRIPTION
    Modify an existing profile 
 
  PARAMETERS
    sdu_in       : incoming request
    profile_type : profile type (3GPP/3GPP2)
    profile_index: profile index to be modified.
    subs_id      : subscription identifier   
    error_val_ptr: Pointer containing the error code in case this API fails 
 
  RETURN VALUE
    response
  
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
===========================================================================*/
dsm_item_type * qmi_wds_profile_modify_settings
(
  dsm_item_type **    sdu_in,
  uint8               profile_type,
  uint8               profile_index,
  uint32              subs_id,  
  qmi_error_type_v01   *error_val_ptr,
  ds_profile_status_etype* modify_status
)
{
  ds_profile_hndl_type              profile_hndl;
  ds_profile_info_type              param_info;
  uint8                             type;
  uint16                            len;
  uint16                            param_len;
  byte                              value[WDS_MAX_PROFILE_PARAM_VAL_LEN];
  qmi_error_type_v01                errval;
  qmi_result_type_v01                 result;
  boolean                           retval;
  dsm_item_type *                   response;
  ds_profile_status_etype           profile_status;
  boolean                           cancel_txn;
  uint32                            *ipv4_value;
  qmi_profile_tft_params_type       tft_qmi_param;
  ds_profile_3gpp_tft_params_type   tft_profile_param;
  ds_profile_status_etype           temp_profile_status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  ASSERT(sdu_in);

  *error_val_ptr = QMI_ERR_NONE_V01;

  memset(&profile_hndl, 0, sizeof(profile_hndl));
  memset(&param_info, 0, sizeof(param_info));
  memset(&tft_qmi_param, 0, sizeof(qmi_profile_tft_params_type));
  memset(&tft_profile_param, 0, sizeof(ds_profile_3gpp_tft_params_type));
  response = NULL;
  errval = QMI_ERR_NONE_V01;
  profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
  temp_profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
  cancel_txn = FALSE;

  /*-------------------------------------------------------------------------
    Begin transaction
  -------------------------------------------------------------------------*/
  profile_status = ds_profile_begin_transaction_per_sub(DS_PROFILE_TRN_RW,
                                                (ds_profile_tech_etype)profile_type,
                                                (ds_profile_num_type)profile_index,
                                                (ds_profile_subs_etype)subs_id,
                                                &profile_hndl);

  if(profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    LOG_MSG_ERROR_1 ("Profile begin Tx returns error %d", profile_status);
    errval = QMI_ERR_EXTENDED_INTERNAL_V01;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Extract one TLV from the SDU at a time and process it.
  -------------------------------------------------------------------------*/
  while (*sdu_in)
  {
    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    // Bypass the mandatory TLV and the persistence TLV
    if ( (type == QMI_TYPE_REQUIRED_PARAMETERS) || (type == WDSI_PRM_TYPE_PROFILE_PERSISTENT) )
    {
      if (len > WDS_MAX_PROFILE_PARAM_VAL_LEN)
      {
        LOG_MSG_INFO2_2("T (%d) Invalid L (%d bytes) V - discarding",
                        type, len);
        cancel_txn = TRUE;
        errval = QMI_ERR_ARG_TOO_LONG_V01;
        goto send_result;
      }
      else if(len != PS_UTILS_DSM_PULLUP( sdu_in, (void*)value, len ))
      {
        cancel_txn = TRUE;
        errval = QMI_ERR_MALFORMED_MSG_V01;
        goto send_result;
      }
      continue;
    }

    errval = qmi_wds_profile_get_param_len(type, profile_type, &param_len);

    /*-----------------------------------------------------------------------
      Check for the length of the profile parameter. For fixed length params
      verify the exact length. For variable lenght params make sure they 
      are not longer than the maximum allowed length. 
    -----------------------------------------------------------------------*/
    if (errval != QMI_ERR_NONE_V01)
    {
      LOG_MSG_INFO1_1("Cannot get the Length for profile param type %d", type);
      cancel_txn = TRUE;
      goto send_result;
    }
    else if(!qmi_svc_validate_type_length( type,
                                           len,
                                           param_len,
                                           FILE_ID_DS_QMI_WDS_PROFILE,__LINE__)  )
    {
      errval = QMI_ERR_MALFORMED_MSG_V01;
      cancel_txn = TRUE;
      goto send_result;
    }
    else if ((param_len == 0 && len > WDS_MAX_PROFILE_PARAM_VAL_LEN))
    {
      LOG_MSG_ERROR_2("T (%d) Invalid L (%d bytes) V - discarding", type, len);
      errval = QMI_ERR_ARG_TOO_LONG_V01;
      cancel_txn = TRUE;
      goto send_result;
    }

    /* memset the local place holder for the value. */
    memset(value, 0, sizeof(value));

    /*-----------------------------------------------------------------------
      Get profile_identifier and param info from TLV
    -----------------------------------------------------------------------*/
    param_info.len     = len;
    param_info.buf     = (void*)value;

    /*-----------------------------------------------------------------------
      if value is NULL, skip past the current TLV
    -----------------------------------------------------------------------*/
    if(len != PS_UTILS_DSM_PULLUP( sdu_in, (void*)value, len ))
    {
      cancel_txn = TRUE;
      errval = QMI_ERR_MALFORMED_MSG_V01;
      goto send_result;
    }

    /*-----------------------------------------------------------------------
      Handling APN bearer param
    -----------------------------------------------------------------------*/
    if (type == DS_PROFILE_3GPP_PROFILE_PARAM_APN_BEARER)
    {
      param_info.len = sizeof(uint8);
      if ( (*(uint64 *)value) & 0x8000000000000000ULL ) 
      {
        *(uint8 *)value = 0xFF;
      }
    }

    /*-----------------------------------------------------------------------
      Handling IPV4 address. Converting them to Big Endian format before
      calling DS Profile API's
    -----------------------------------------------------------------------*/

    if ((type == DS_PROFILE_3GPP_PROFILE_PARAM_DNS_ADDR_V4_PRIMARY)  || 
        (type == DS_PROFILE_3GPP_PROFILE_PARAM_DNS_ADDR_V4_SECONDARY)|| 
        (type == DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_ADDR_V4) || 
        (type == DS_PROFILE_3GPP2_PROFILE_PARAM_V4_DNS_ADDR_PRIMARY) || 
        (type == DS_PROFILE_3GPP2_PROFILE_PARAM_V4_DNS_ADDR_SECONDARY) ||
        (type == DS_PROFILE_TECH_COMMON_PROFILE_PARAM_V4_DNS_ADDR_PRIMARY) ||
        (type == DS_PROFILE_TECH_COMMON_PROFILE_PARAM_V4_DNS_ADDR_SECONDARY))
    {
      ipv4_value = (uint32 *) param_info.buf;
      *ipv4_value = ps_htonl(*ipv4_value);
    }

    if ((type == DS_PROFILE_TECH_COMMON_PROFILE_PARAM_PDP_TYPE)
        || (type == DS_PROFILE_3GPP_PROFILE_PARAM_OVERRIDE_HOME_PDP_TYPE)
        || (type == DS_PROFILE_3GPP_PROFILE_PARAM_LTE_ROAMING_PDP_TYPE)
        || (type == DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_ROAMING_PDP_TYPE))
    {
      param_info.len = sizeof(uint8);
    }

    if ((type == DS_PROFILE_3GPP_PROFILE_PARAM_TFT_FILTER_ID1) ||
        (type == DS_PROFILE_3GPP_PROFILE_PARAM_TFT_FILTER_ID2))
    {
      param_info.len                    = sizeof(ds_profile_3gpp_tft_params_type);
      tft_qmi_param                     = *((qmi_profile_tft_params_type*)value);
      tft_profile_param.filter_id       = tft_qmi_param.filter_id;
      tft_profile_param.eval_prec_id    = tft_qmi_param.eval_prec_id;
      tft_profile_param.ip_version      = tft_qmi_param.ip_version;
      tft_profile_param.prot_num        = tft_qmi_param.prot_num;
      tft_profile_param.dest_port_range = tft_qmi_param.dest_port_range;
      tft_profile_param.src_port_range  = tft_qmi_param.src_port_range;
      tft_profile_param.ipsec_spi       = tft_qmi_param.ipsec_spi;
      tft_profile_param.tos_mask        = tft_qmi_param.tos_mask;
      tft_profile_param.flow_label      = tft_qmi_param.flow_label;

      if (tft_profile_param.ip_version == DS_PROFILE_3GPP_IP_V4)
      {
        tft_profile_param.src_addr.address.ds_profile_3gpp_tft_addr_ipv4 = 
          ps_htonl(tft_qmi_param.address.ds_profile_3gpp_tft_addr_ipv4);
        tft_profile_param.src_addr.mask.mask = (uint32)ps_htonl((0xFFFFFF00) |
                                                         tft_qmi_param.mask);
      }
      else if (tft_profile_param.ip_version == DS_PROFILE_3GPP_IP_V6)
      {
        tft_profile_param.src_addr.address   = tft_qmi_param.address;
        tft_profile_param.src_addr.mask.mask = (uint32)tft_qmi_param.mask;
      }
      
      param_info.buf = (void*)&tft_profile_param;
    }

    /*-----------------------------------------------------------------------
      Set the Profile with the value of profile identifier.
    -----------------------------------------------------------------------*/
    profile_status = ds_profile_set_param(profile_hndl, (uint32)type, 
                                          &param_info);

    if(profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
    {
      if (profile_status == DS_PROFILE_REG_SET_IDENT_NOT_SUPPORTED)
      {
        LOG_MSG_ERROR_1("Set Profile return error as the ident is not supported %d",type);
        temp_profile_status = DS_PROFILE_REG_SET_IDENT_NOT_SUPPORTED;
        profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
        continue;
                       
      }
      else
      {
        LOG_MSG_ERROR_2 ("Set Profile returns error. Profile_identifier %d, Error %d",
                         type, profile_status);
        cancel_txn = TRUE;
        errval = QMI_ERR_EXTENDED_INTERNAL_V01;
        goto send_result;
      }
    }
  } /* end while(sdu_in) */
    
  /*-------------------------------------------------------------------------
    End transaction
  -------------------------------------------------------------------------*/
  profile_status = ds_profile_end_transaction(profile_hndl, 
                                              DS_PROFILE_ACTION_COMMIT);
  if(profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    LOG_MSG_ERROR_1 ("End Tx Profile returns error %d", profile_status);
    errval = QMI_ERR_EXTENDED_INTERNAL_V01;
    goto send_result;
  }

send_result:
  
  dsm_free_packet(sdu_in);
  
  if (cancel_txn == TRUE)
  {
    (void)ds_profile_end_transaction(profile_hndl, 
                                     DS_PROFILE_ACTION_CANCEL);
  }

  if (profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    retval = qmi_svc_put_param_tlv(&response,
                                   QMI_TYPE_EXT_ERROR_INFO,
                                   WDSI_PRM_TYPE_EXT_ERROR_INFO_LEN,
                                   (void*)&profile_status);
    CHECK_RETVAL();
  }

  if (profile_status == DS_PROFILE_REG_RESULT_SUCCESS)
  {
    if (temp_profile_status == DS_PROFILE_REG_SET_IDENT_NOT_SUPPORTED)
    {
      if(modify_status)
      {
        *modify_status = temp_profile_status;
      }
      retval = qmi_svc_put_param_tlv(&response,
                                     QMI_TYPE_EXT_ERROR_INFO,
                                     WDSI_PRM_TYPE_EXT_ERROR_INFO_LEN,
                                     (void*)&temp_profile_status);
      CHECK_RETVAL();
    }
  }

  *error_val_ptr = errval;
  
  result = (errval == QMI_ERR_NONE_V01? QMI_RESULT_SUCCESS_V01 
                                   : QMI_RESULT_FAILURE_V01);
  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;
} /* qmi_wds_profile_modify_settings */


/*===========================================================================
  FUNCTION QMI_WDS_PROFILE_GET_SETTINGS()
 
  DESCRIPTION
    Get the settings for a given profile.
 
  PARAMETERS
    profile_type     : profile type (3GPP/3GPP2)
    profile_index    : profile whose settings are to be retrieved
 
  RETURN VALUE
    ptr to response
 
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
 ==========================================================================*/
#define HANDLE_FAILURE(pkt)  dsm_free_packet(pkt); \
                             errval = QMI_ERR_NO_MEMORY_V01; \
                             goto send_result 
dsm_item_type * qmi_wds_profile_get_settings
(
  uint8           profile_type,
  uint8           profile_index,
  uint32          subs_id
)
{
  dsm_item_type *                   response;
  qmi_error_type_v01                errval;
  qmi_result_type_v01                 result;
  boolean                           retval;
  ds_profile_status_etype           profile_status;
  uint8                             index;
  uint8                             array_size;
  uint8                             array_size_pp;
  uint8                             array_size_pp2;
  uint8                             array_size_common;
  uint8                             array_size_general;
  ds_profile_hndl_type              profile_hndl;
  ds_profile_identifier_type        profile_identifier;
  ds_profile_info_type              param_info;
  byte                              value[WDS_MAX_PROFILE_PARAM_VAL_LEN];
  uint16                            param_len;
  boolean                           cancel_txn;
  uint32                            *ipv4_value;
  ds_profile_3gpp_tft_params_type   tft_profile_param;
  qmi_profile_tft_params_type       tft_qmi_param;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  memset(&profile_hndl, 0, sizeof(profile_hndl));
  memset(&param_info, 0, sizeof(param_info));
  memset(value, 0, sizeof(value));
  memset(&tft_qmi_param, 0, sizeof(qmi_profile_tft_params_type));
  memset(&tft_profile_param, 0, sizeof(ds_profile_3gpp_tft_params_type));
  profile_identifier  = 0;
  response    = NULL;
  errval      = QMI_ERR_NONE_V01;

  array_size  = 0;
  array_size_pp = 0;
  array_size_pp2 = 0;
  array_size_common = 0;
  array_size_general = 0;

  profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
  cancel_txn = FALSE;

  if(profile_type == WDS_PROFILE_TYPE_3GPP || 
     profile_type == WDS_PROFILE_TYPE_EPC)
  {
    #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined(FEATURE_LTE) || defined(FEATURE_TDSCDMA)
      array_size_pp = sizeof(qmi_profile_3gpp_param_tl_list)/
                     sizeof(qmi_profile_3gpp_param_tl_list_type);
    #endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined(FEATURE_LTE) || defined(FEATURE_TDSCDMA)*/
      array_size = array_size_pp;
  }

  if(profile_type == WDS_PROFILE_TYPE_3GPP2 || 
     profile_type == WDS_PROFILE_TYPE_EPC)
  {
//  #ifdef FEATURE_CDMA
      array_size_pp2 = sizeof(qmi_profile_3gpp2_param_tl_list)/
                     sizeof(qmi_profile_3gpp2_param_tl_list_type);
      array_size = array_size_pp2;
//  #endif /* FEATURE_CDMA */
  }

  if(profile_type == WDS_PROFILE_TYPE_EPC)
  {
    array_size_common = sizeof(qmi_profile_common_param_tl_list)/
                     sizeof(qmi_profile_common_param_tl_list_type);
    array_size = array_size_pp + array_size_pp2 + array_size_common;
  }


  if (!array_size)
  {
    errval = QMI_ERR_INVALID_PROFILE_TYPE_V01;
    goto send_result;
  }

  array_size_general = sizeof(qmi_profile_general_param_tl_list)/
                     sizeof(qmi_profile_general_param_tl_list_type);
  array_size = array_size + array_size_general;


  /*-------------------------------------------------------------------------
    Begin transaction
  -------------------------------------------------------------------------*/
  profile_status = 
    ds_profile_begin_transaction_per_sub(DS_PROFILE_TRN_READ, 
                                 (ds_profile_tech_etype)profile_type,
                                 (ds_profile_num_type)profile_index,
                                         (ds_profile_subs_etype)subs_id,
                                 &profile_hndl);
  if (profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    LOG_MSG_ERROR_1 ("Begin Transaction failed. Error %d", profile_status);
    errval = QMI_ERR_EXTENDED_INTERNAL_V01;
    goto send_result;
  }

  index = 0;
  while (index < array_size)
  {
    /*-----------------------------------------------------------------------
      Get profile_identifier.
      Exception. We don't return password TLV for 3GPP2.
    -----------------------------------------------------------------------*/
    if (profile_type == WDS_PROFILE_TYPE_3GPP2)
    {
//  #ifdef FEATURE_CDMA
      if(index < array_size_pp2)
      {
      profile_identifier = qmi_profile_3gpp2_param_tl_list[index].param_type;
      if (profile_identifier == DS_PROFILE_3GPP2_PROFILE_PARAM_AUTH_PASSWORD)
      {
        index++;
        continue;
      }
      }
      else
      {
        profile_identifier = qmi_profile_general_param_tl_list[index-array_size_pp2].param_type;
      }
//  #endif /* FEATURE_CDMA */
    }
    else if (profile_type == WDS_PROFILE_TYPE_3GPP)
    {
    #if defined (FEATURE_WCDMA) || defined (FEATURE_GSM) || defined(FEATURE_LTE) || defined(FEATURE_TDSCDMA)
     if(index < array_size_pp)
     {
      profile_identifier = qmi_profile_3gpp_param_tl_list[index].param_type;
     }
     else
     {
       profile_identifier = qmi_profile_general_param_tl_list[index-array_size_pp].param_type; 
     }
    #endif /* defined (FEATURE_WCDMA) || defined (FEATURE_GSM) || defined(FEATURE_LTE) || defined(FEATURE_TDSCDMA) */
    }

    else if (profile_type == WDS_PROFILE_TYPE_EPC)
    {
      if(index < array_size_pp)
      {
      #if defined (FEATURE_WCDMA) || defined (FEATURE_GSM) || defined(FEATURE_LTE) || defined(FEATURE_TDSCDMA)
        profile_identifier = qmi_profile_3gpp_param_tl_list[index].param_type;
      #endif /* defined (FEATURE_WCDMA) || defined (FEATURE_GSM) || defined(FEATURE_LTE) || defined(FEATURE_TDSCDMA) */
      }
      else if((index >= array_size_pp) && (index < (array_size_pp + array_size_common)))
      {
        profile_identifier = qmi_profile_common_param_tl_list[index-array_size_pp].param_type;
      }
      else if((index >= array_size_pp + array_size_common) && (index < (array_size_pp + array_size_common + array_size_pp2)))
      {
//    #ifdef FEATURE_CDMA
        profile_identifier = qmi_profile_3gpp2_param_tl_list[index-array_size_pp-array_size_common].param_type;
        if (profile_identifier == DS_PROFILE_3GPP2_PROFILE_PARAM_AUTH_PASSWORD)
        {
          index++;
          continue;
        }
//    #endif /* FEATURE_CDMA */
      }
      else
      {
        profile_identifier = qmi_profile_general_param_tl_list[index-array_size_pp-array_size_common-array_size_pp2].param_type;
      }
    }

    memset(value, 0, sizeof(value));
    param_info.len = sizeof(value);
    param_info.buf = (void*)value;
    
    /*-----------------------------------------------------------------------
      Retrieve profile_identifier and param info
    -----------------------------------------------------------------------*/
    profile_status = ds_profile_get_param(profile_hndl, 
                                          profile_identifier, 
                                          &param_info);
    if (profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
    {
      /*-------------------------------------------------------------------
      Not all parameters are available for each profile. Get whatever is 
      available. Set the profile_status to Success as it is used later
      to send the extended error code.
      -------------------------------------------------------------------*/
      profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
      index++;
      continue;
    }

    /*-----------------------------------------------------------------------
      Handling APN bearer param
    -----------------------------------------------------------------------*/
    if (profile_identifier == DS_PROFILE_3GPP_PROFILE_PARAM_APN_BEARER)
    {
      param_info.len = sizeof(uint64);
      if (*(uint8 *)value == 0xFF)
      {
        *(uint64 *)value = 0x8000000000000000ULL;
      }
    }

    /*-----------------------------------------------------------------------
      Handling IPV4 address. Converting them back to Little Endian format since
      they are stored as Big Endian after conversion
     -----------------------------------------------------------------------*/

    if ((profile_identifier == DS_PROFILE_3GPP_PROFILE_PARAM_DNS_ADDR_V4_PRIMARY)  || 
        (profile_identifier == DS_PROFILE_3GPP_PROFILE_PARAM_DNS_ADDR_V4_SECONDARY)|| 
        (profile_identifier == DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_ADDR_V4) || 
        (profile_identifier == DS_PROFILE_3GPP2_PROFILE_PARAM_V4_DNS_ADDR_PRIMARY) || 
        (profile_identifier == DS_PROFILE_3GPP2_PROFILE_PARAM_V4_DNS_ADDR_SECONDARY) ||
        (profile_identifier == DS_PROFILE_TECH_COMMON_PROFILE_PARAM_V4_DNS_ADDR_PRIMARY) ||
        (profile_identifier == DS_PROFILE_TECH_COMMON_PROFILE_PARAM_V4_DNS_ADDR_SECONDARY))
    {
      ipv4_value = (uint32 *) param_info.buf;
      *ipv4_value = ps_ntohl(*ipv4_value);
    }

    if ((profile_identifier == DS_PROFILE_3GPP_PROFILE_PARAM_TFT_FILTER_ID1) ||
        (profile_identifier == DS_PROFILE_3GPP_PROFILE_PARAM_TFT_FILTER_ID2))
    {
      param_info.len                = sizeof(qmi_profile_tft_params_type);
      tft_profile_param             = *((ds_profile_3gpp_tft_params_type*)param_info.buf);
      tft_qmi_param.filter_id       = tft_profile_param.filter_id;
      tft_qmi_param.eval_prec_id    = tft_profile_param.eval_prec_id;
      tft_qmi_param.ip_version      = tft_profile_param.ip_version;
      tft_qmi_param.prot_num        = tft_profile_param.prot_num;
      tft_qmi_param.dest_port_range = tft_profile_param.dest_port_range;
      tft_qmi_param.src_port_range  = tft_profile_param.src_port_range;
      tft_qmi_param.ipsec_spi       = tft_profile_param.ipsec_spi;
      tft_qmi_param.tos_mask        = tft_profile_param.tos_mask;
      tft_qmi_param.flow_label      = tft_profile_param.flow_label;

      if (tft_profile_param.ip_version == DS_PROFILE_3GPP_IP_V4)
      {
        tft_qmi_param.address.ds_profile_3gpp_tft_addr_ipv4 = 
          ps_ntohl(tft_profile_param.src_addr.address.ds_profile_3gpp_tft_addr_ipv4);
        tft_qmi_param.mask = (uint8)(0xFF & ps_ntohl(tft_profile_param.src_addr.mask.mask));
      }
      else if (tft_profile_param.ip_version == DS_PROFILE_3GPP_IP_V6)
      {
        tft_qmi_param.address = tft_profile_param.src_addr.address;
        tft_qmi_param.mask = (uint8)tft_profile_param.src_addr.mask.mask;
      }
      param_info.buf = (void*)&tft_qmi_param;
    }
    if ((profile_identifier == DS_PROFILE_TECH_COMMON_PROFILE_PARAM_PDP_TYPE)
         || (profile_identifier == DS_PROFILE_3GPP_PROFILE_PARAM_OVERRIDE_HOME_PDP_TYPE)
         || (profile_identifier == DS_PROFILE_3GPP_PROFILE_PARAM_LTE_ROAMING_PDP_TYPE)
         || (profile_identifier == DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_ROAMING_PDP_TYPE))
    {
      //new enums are 32 bit
      param_info.len = sizeof(uint32);
    }
    /*-----------------------------------------------------------------------
      Verify that the return length is same as expected length for fixed
      length identifiers.
    -----------------------------------------------------------------------*/
    errval = qmi_wds_profile_get_param_len(profile_identifier, profile_type, 
                                           &param_len);
    if (errval != QMI_ERR_NONE_V01)
    {
      LOG_MSG_ERROR_1("Cannot get the Length for profile param %d",
                      profile_identifier);
      ASSERT(0);
    }
    else if ((param_len != 0 && param_len != param_info.len))
    {
      LOG_MSG_ERROR_3("Invalid Length. Identifier(%d) Expected(%d) Got(%d)",
                      profile_identifier, param_len, param_info.len);
      cancel_txn = TRUE;
      errval = QMI_ERR_MALFORMED_MSG_V01;
      goto send_result;
    }
    
    /*-----------------------------------------------------------------------
      Populate the response, containing all profile settings
    -----------------------------------------------------------------------*/
    if( FALSE == qmi_svc_put_param_tlv(&response,
                                       (uint8)profile_identifier,
                                       (uint16)param_info.len,
                                       (void*) param_info.buf))
    {
      cancel_txn = TRUE;
      HANDLE_FAILURE(&response);
    }
    index++;
  } /* while loop end */

  /*-------------------------------------------------------------------------
    End transaction
  -------------------------------------------------------------------------*/
  profile_status = ds_profile_end_transaction(profile_hndl, 
                                              DS_PROFILE_ACTION_COMMIT);
  if(profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    LOG_MSG_ERROR_1 ("End Transaction failed. Error %d", profile_status);
    errval = QMI_ERR_EXTENDED_INTERNAL_V01;
    goto send_result;
  }

send_result:
  if (cancel_txn == TRUE)
  {
    (void)ds_profile_end_transaction(profile_hndl, 
                                     DS_PROFILE_ACTION_CANCEL);
  }
  if (profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    retval = qmi_svc_put_param_tlv(&response,
                                   QMI_TYPE_EXT_ERROR_INFO,
                                   WDSI_PRM_TYPE_EXT_ERROR_INFO_LEN,
                                   (void*)&profile_status);
    CHECK_RETVAL();
  }

  result = (errval == QMI_ERR_NONE_V01? QMI_RESULT_SUCCESS_V01 
                                     : QMI_RESULT_FAILURE_V01);
  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;
} /* qmi_wds_profile_get_settings */


/*===========================================================================
  FUNCTION QMI_WDS_PROFILE_GET_DEFAULT_SETTINGS()
 
  DESCRIPTION
    For a given technology get the settings for default profile.
 
  PARAMETERS
    profile_type     : profile type (3GPP/3GPP2)
 
  RETURN VALUE
    ptr to response
 
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
 ==========================================================================*/
dsm_item_type * qmi_wds_profile_get_default_settings
(
  uint8           profile_type,
  uint32          subs_id
)
{
  dsm_item_type *                   response;
  wds_get_default_settings_resp_msg_v01* resp_msg = NULL;
  qmi_error_type_v01                errval;
  uint16                            default_profile_num;
  uint32                            profile_family;
  ds_profile_status_etype           profile_status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  default_profile_num   = 0;
  response    = NULL;
  errval      = QMI_ERR_NONE_V01;
  profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
  profile_family = 0;
  PS_SYSTEM_HEAP_MEM_ALLOC(resp_msg,
                           sizeof(wds_get_default_settings_resp_msg_v01),
                           wds_get_default_settings_resp_msg_v01*);
  /*Sizeof wds_get_default_settings_resp_msg_v01 is big if we dont get requested memory then
       we are sending result tlv with ERROR*/
  if(resp_msg == NULL)
  {
    qmi_svc_put_result_tlv(&response, (qmi_result_e_type)QMI_RESULT_FAILURE_V01, 
                           (qmi_error_e_type)QMI_ERR_NO_MEMORY_V01);
    return response;
  }
  else
  {
    memset(resp_msg,0,sizeof(wds_get_default_settings_resp_msg_v01));
  }
  switch(profile_type)
  {
    case WDS_PROFILE_TYPE_3GPP:
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM) || defined(FEATURE_LTE) || defined(FEATURE_TDSCDMA)
      profile_family = DS_PROFILE_3GPP_RMNET_PROFILE_FAMILY;
#endif /*defined (FEATURE_WCDMA) || defined (FEATURE_GSM) || defined(FEATURE_LTE) || defined(FEATURE_TDSCDMA)*/
      break;

    case WDS_PROFILE_TYPE_3GPP2:
      /* No Profile Family on CDMA side */
      break;

    default:
      errval = QMI_ERR_INVALID_PROFILE_TYPE_V01;
      goto send_result;
  }

  /*-------------------------------------------------------------------------
    Call the API to get the default profile number.
  -------------------------------------------------------------------------*/
  profile_status = 
    ds_profile_get_default_profile_num_per_subs((ds_profile_tech_etype)profile_type,
                                        profile_family,
                                                (ds_profile_subs_etype)subs_id,
                                        &default_profile_num);
  if (profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    LOG_MSG_ERROR_2 ("Get Default Profile for family %d failed. Error %d",
                     profile_type, profile_status);
    errval = QMI_ERR_EXTENDED_INTERNAL_V01;
    goto send_result;
  }
  
  /* Call the API to get the profile settings. */
  PS_SYSTEM_HEAP_MEM_FREE(resp_msg);
  return qmi_wds_profile_get_settings(profile_type,
                                      (uint8)default_profile_num,
                                      subs_id);

send_result:

  if (profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    resp_msg->extended_error_code_valid = TRUE;
    resp_msg->extended_error_code = (wds_ds_extended_error_code_enum_v01)profile_status;
  }  
  QMI_WDSI_ENCODE_RESPONSE_MSG(QMI_WDS_GET_DEFAULT_SETTINGS_RESP_V01,
                               resp_msg,
                               sizeof(wds_get_default_settings_resp_msg_v01),
                               errval,
                               &response);
  PS_SYSTEM_HEAP_MEM_FREE(resp_msg);
  return response;
} /* qmi_wds_profile_get_default_settings */


/*===========================================================================
  FUNCTION QMI_WDS_PROFILE_GET_LIST()

  DESCRIPTION
    Get a list of profiles.

  PARAMETERS
    sdu_in         : incoming request
    profile_type   : profile type
    subs_id        : subscription identifier

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsm_item_type * qmi_wds_profile_get_list
(
  uint8               profile_type,
  uint32              subs_id
)
{
  qmi_error_type_v01              errval;
  dsm_item_type *                 response;
  byte                            value[WDS_MAX_PROFILE_PARAM_VAL_LEN];
  #define WDS_MAX_PROFILE_LIST_LEN  1000
  ds_profile_itr_type             itr;
  ds_profile_list_info_type       list_info;
  ds_profile_info_type            profile_info;
  ds_profile_status_etype         profile_status;
  ds_profile_list_type            search_list_info;
  wds_get_profile_list_resp_msg_v01* resp_msg = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&itr,0,sizeof(itr));
  
  profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
  memset(&search_list_info, 0, sizeof(search_list_info));
  
  errval = QMI_ERR_NONE_V01;
  response = NULL;
  PS_SYSTEM_HEAP_MEM_ALLOC(resp_msg,
                           sizeof(wds_get_profile_list_resp_msg_v01),
                           wds_get_profile_list_resp_msg_v01*);
  if(resp_msg == NULL)
  {
    qmi_svc_put_result_tlv(&response, (qmi_result_e_type)QMI_RESULT_FAILURE_V01,
                          (qmi_error_e_type)QMI_ERR_NO_MEMORY_V01);
    return response;
  }
  else
  {
    memset(resp_msg,0,sizeof(wds_get_profile_list_resp_msg_v01));
  }
  
  /* Default operation - Get all profiles */
  search_list_info.dfn = DS_PROFILE_LIST_ALL_PROFILES;
  
  /*-------------------------------------------------------------------------
    Get the iterator. If there is no entry then the DS Profile will return
    LIST_END error code. Fill number of instances as 0 and return success.
  -------------------------------------------------------------------------*/
  profile_status = ds_profile_get_list_itr_per_sub((ds_profile_tech_etype)profile_type,
                                                   &search_list_info, 
                                                   &itr,
                                                   (ds_profile_subs_etype)subs_id);
  if(profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    if(profile_status == DS_PROFILE_REG_RESULT_LIST_END)
    {
      LOG_MSG_INFO1_0("No Profile entry found matching the request");
      resp_msg->profile_list_len = 0;
      profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
    }
    else
    {
      LOG_MSG_ERROR_1 ("Getting profile list iterator failed. Error %d",
                       profile_status);
      errval = QMI_ERR_EXTENDED_INTERNAL_V01;
    }
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Reserve 1 byte for the num_instances field to be filled in later
    increment ptr by 1 and decrement empty space in list 1 byte
  -------------------------------------------------------------------------*/
  while(profile_status != DS_PROFILE_REG_RESULT_LIST_END &&
        resp_msg->profile_list_len < QMI_WDS_PROFILE_LIST_MAX_V01)
  {
    memset(&list_info, 0, sizeof (ds_profile_list_info_type));
    memset(value, 0, sizeof(value));

    list_info.name = &profile_info;
    list_info.name->len  = sizeof(value);
    list_info.name->buf  = (void*)value;

    /*-----------------------------------------------------------------------
      Call the profile registry API to get the profile info
    -----------------------------------------------------------------------*/
    profile_status = ds_profile_get_info_by_itr(itr, &list_info);
    if(profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
    {
      LOG_MSG_ERROR_1 ("Getting profile info by iterator failed. Error %d",
                       profile_status);
      (void) ds_profile_itr_destroy(itr);
      errval = QMI_ERR_EXTENDED_INTERNAL_V01;
      goto send_result;
    }

    /*-----------------------------------------------------------------------
      Put the result TLV in the profile list.
      1 - Profile Type (3GPP/3GPP2).
      2 - Profile Number.
      3 - Profile Name Length. (Would be 0 for 3GPP2)
      4 - Profile Name. (Would be NULL for 3GPP2)
    -----------------------------------------------------------------------*/

    /* Add Profile Type field */
    resp_msg->profile_list[resp_msg->profile_list_len].profile_type = profile_type;

    /* Add Profile Number field */
    resp_msg->profile_list[resp_msg->profile_list_len].profile_index = (uint8)list_info.num;

    /* Add Profile Name, if any */
    if(list_info.name->len != 0)
    {
      memscpy( (char *)resp_msg->profile_list[resp_msg->profile_list_len].profile_name, 
               sizeof(resp_msg->profile_list[resp_msg->profile_list_len].profile_name),
              (char *)list_info.name->buf,
              list_info.name->len );
    }

    resp_msg->profile_list_len++;

    profile_status = ds_profile_itr_next(itr);
    if ( (profile_status != DS_PROFILE_REG_RESULT_SUCCESS) &&
         (profile_status != DS_PROFILE_REG_RESULT_LIST_END) )
    {
      LOG_MSG_ERROR_1 ("Getting Next iterator failed. Error %d",
                       profile_status);
      (void) ds_profile_itr_destroy(itr);
      errval = QMI_ERR_EXTENDED_INTERNAL_V01;
      goto send_result;
    }
  } /* while (profile_status != DS_PROFILE_REG_RESULT_LIST_END) */

  /*-------------------------------------------------------------------------
    Destroy the iterator
  -------------------------------------------------------------------------*/
  profile_status = ds_profile_itr_destroy(itr);
  if(profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    LOG_MSG_ERROR_1 ("Destroying iterator failed. Error %d", profile_status);
    errval = QMI_ERR_EXTENDED_INTERNAL_V01;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Populate the first byte reserved earlier with number of profiles
  -------------------------------------------------------------------------*/

send_result:
  if (profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    resp_msg->extended_error_code_valid = TRUE;
    resp_msg->extended_error_code = (wds_ds_extended_error_code_enum_v01)profile_status;
  }
  QMI_WDSI_ENCODE_RESPONSE_MSG(QMI_WDS_GET_PROFILE_LIST_RESP_V01,
                               resp_msg,
                               sizeof(wds_get_profile_list_resp_msg_v01),
                               errval,
                               &response);
  PS_SYSTEM_HEAP_MEM_FREE(resp_msg);
  return response;
} /* qmi_wds_profile_get_list */


/*===========================================================================
  FUNCTION QMI_WDS_PROFILE_GET_DEFAULT_PROFILE_NUM()
 
  DESCRIPTION
    For a given technology get the default profile number.
 
  PARAMETERS
    profile_type     : profile type (3GPP/3GPP2)
    profile_family   : profile family
    subs_id          : Subscription identifier
 
  RETURN VALUE
    ptr to response
 
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
 ==========================================================================*/
dsm_item_type * qmi_wds_profile_get_default_profile_num
(
  uint8           profile_type,
  uint8           profile_family,
  uint32          subs_id
)
{
  dsm_item_type *                   response;
  wds_get_default_profile_num_resp_msg_v01 resp_msg;
  qmi_error_type_v01                errval;
  uint16                            default_profile_num;
  ds_profile_status_etype           profile_status;
  ds_profile_subs_etype             profile_subs;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
  response       = NULL;
  errval         = QMI_ERR_NONE_V01;
  default_profile_num = 0;
  profile_subs = (ds_profile_subs_etype)subs_id;
  memset(&resp_msg,0,sizeof(resp_msg));
  /*-------------------------------------------------------------------------
    Call the API to get the default profile number.
  -------------------------------------------------------------------------*/
  profile_status = 
    ds_profile_get_default_profile_num_per_subs ((ds_profile_tech_etype)profile_type, 
                                        (uint32)profile_family,
                                        profile_subs,
                                        &default_profile_num);
  if (profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    LOG_MSG_ERROR_2 ("Get Default Profile for family %d failed. Error %d",
                     profile_type, profile_status);
    errval = QMI_ERR_EXTENDED_INTERNAL_V01;
    goto send_result;
  }
  resp_msg.profile_index = (uint8)default_profile_num;
send_result:
  if (profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    resp_msg.extended_error_code_valid = TRUE;
    resp_msg.extended_error_code = (wds_ds_extended_error_code_enum_v01)profile_status;
  }  
  QMI_WDSI_ENCODE_RESPONSE_MSG(QMI_WDS_GET_DEFAULT_PROFILE_NUM_RESP_V01,
                               &resp_msg,
                               sizeof(resp_msg),
                               errval,
                               &response);
  return response;
} /* qmi_wds_profile_get_default_profile_num */


/*===========================================================================
  FUNCTION QMI_WDS_PROFILE_SET_DEFAULT_PROFILE_NUM()
 
  DESCRIPTION
    For a given technology set the default profile number.
 
  PARAMETERS
    profile_type     : profile type (3GPP/3GPP2)
    profile_family   : profile family
    profile_index    : profile number that should be set to default
    subs_id          : subscription identifier 
 
  RETURN VALUE
    ptr to response
 
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
 ==========================================================================*/
dsm_item_type * qmi_wds_profile_set_default_profile_num
(
  uint8           profile_type,
  uint8           profile_family,
  uint8           profile_index,
  uint32          subs_id
)
{
  dsm_item_type *                   response;
  wds_set_default_profile_num_resp_msg_v01 resp_msg;
  qmi_error_type_v01                errval;
  ds_profile_status_etype           profile_status;
  ds_profile_subs_etype             profile_subs;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
  response       = NULL;
  errval         = QMI_ERR_NONE_V01;
  profile_subs = (ds_profile_subs_etype)subs_id;
  memset(&resp_msg,0,sizeof(resp_msg));
  /*-------------------------------------------------------------------------
    Call the API to set the default profile number.
  -------------------------------------------------------------------------*/
  profile_status = 
    ds_profile_set_default_profile_num_per_subs((ds_profile_tech_etype)profile_type, 
                                       (uint32)profile_family,
                                                 profile_subs,
                                       (ds_profile_num_type)profile_index);
  if (profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    LOG_MSG_ERROR_2 ("Set Default Profile for family %d failed. Error %d",
                     profile_type, profile_status);
    errval = QMI_ERR_EXTENDED_INTERNAL_V01;
    goto send_result;
  }

send_result:
  if (profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    resp_msg.extended_error_code_valid = TRUE;
    resp_msg.extended_error_code = (wds_ds_extended_error_code_enum_v01) profile_status;
  }  
  QMI_WDSI_ENCODE_RESPONSE_MSG(QMI_WDS_SET_DEFAULT_PROFILE_NUM_RESP_V01,
                               &resp_msg,
                               sizeof(resp_msg),
                               errval,
                               &response);
  return response;
} /* qmi_wds_profile_set_default_profile_num */


/*===========================================================================
  FUNCTION QMI_WDS_PROFILE_RESET_PROFILE_TO_DEFAULT()
 
  DESCRIPTION
    For a given technology resets the specified profile to default values.
 
  PARAMETERS
    profile_type     : profile type (3GPP/3GPP2)
    profile_index    : profile number that should be reset to use 
                       default values
 
  RETURN VALUE
    ptr to response
 
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
 ==========================================================================*/
dsm_item_type * qmi_wds_profile_reset_profile_to_default
(
  uint8           profile_type,
  uint8           profile_index,
  uint32          subs_id
)
{
  dsm_item_type *                   response;
  wds_reset_profile_to_default_resp_msg_v01 resp_msg;
  qmi_error_type_v01                errval;
  ds_profile_status_etype           profile_status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
  response       = NULL;
  errval         = QMI_ERR_NONE_V01;
  memset(&resp_msg,0,sizeof(resp_msg));

  /*-------------------------------------------------------------------------
    Call the DS Profile API to reset the profile to default values
  -------------------------------------------------------------------------*/
  profile_status = 
    ds_profile_reset_profile_to_default_per_sub((ds_profile_tech_etype)profile_type,
                                                (ds_profile_num_type)profile_index,
                                                (ds_profile_subs_etype)subs_id);
  if (profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    LOG_MSG_ERROR_3 ("Reset Profile %d to default for family %d failed. Error %d",
                     profile_index, profile_type, profile_status);
    errval = QMI_ERR_EXTENDED_INTERNAL_V01;
    goto send_result;
  }

send_result:
  if (profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    resp_msg.extended_error_code_valid = TRUE;
    resp_msg.extended_error_code = (wds_ds_extended_error_code_enum_v01)profile_status;
  }  
  QMI_WDSI_ENCODE_RESPONSE_MSG(QMI_WDS_RESET_PROFILE_TO_DEFAULT_RESP_V01,
                               &resp_msg,
                               sizeof(resp_msg),
                               errval,
                               &response);
  return response;
} /* qmi_wds_profile_reset_profile_to_default() */


/*===========================================================================
  FUNCTION QMI_WDS_PROFILE_RESET_PARAM_TO_INVALID()
 
  DESCRIPTION
    Set the specified parameter in the specified profile to invalid
 
  PARAMETERS
    profile_type     : profile type (3GPP/3GPP2)
    profile_index    : profile number that should be reset to use 
                       default values
    param_id         : param id that should be reset to invalid
 
  RETURN VALUE
    ptr to response
 
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
 ==========================================================================*/
dsm_item_type * qmi_wds_profile_reset_param_to_invalid
(
  uint8           profile_type,
  uint8           profile_index,
  uint32          param_id,
  uint32          subs_id
)
{
  dsm_item_type *                   response;
  wds_reset_profile_param_to_invalid_resp_msg_v01 resp_msg;
  qmi_error_type_v01                errval;
  ds_profile_status_etype           profile_status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
  response       = NULL;
  errval         = QMI_ERR_NONE_V01;
  memset(&resp_msg,0,sizeof(resp_msg));

  /*-------------------------------------------------------------------------
    Call the DS Profile API to reset the profile param to invalid
  -------------------------------------------------------------------------*/
  profile_status = 
      ds_profile_reset_param_to_invalid_per_sub((ds_profile_tech_etype)profile_type,
                                         (ds_profile_num_type)profile_index,
                                                param_id,
                                                (ds_profile_subs_etype)subs_id);
  if (profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    LOG_MSG_ERROR_3 ("Invalidate profile param %d profile %d failed. Error %d",
                     param_id, profile_index, profile_status);
    errval = QMI_ERR_EXTENDED_INTERNAL_V01;
    goto send_result;
  }

send_result:
  if (profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    resp_msg.extended_error_code_valid = TRUE;
    resp_msg.extended_error_code = (wds_ds_extended_error_code_enum_v01)profile_status;
  }  
  QMI_WDSI_ENCODE_RESPONSE_MSG(QMI_WDS_RESET_PROFILE_PARAM_TO_INVALID_RESP_V01,
                               &resp_msg,
                               sizeof(resp_msg),
                               errval,
                               &response);
  return response;
} /* qmi_wds_profile_reset_param_to_invalid() */


/*===========================================================================
  FUNCTION QMI_WDS_PROFILE_CREATE()
 
  DESCRIPTION
    Create a new profile.
 
  PARAMETERS
    sdu_in       : incoming request
    subs_id      : subscription identifier
 
  RETURN VALUE
    response
  
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
===========================================================================*/
dsm_item_type * qmi_wds_profile_create
(
  dsm_item_type **    sdu_in,
  uint32              subs_id
)
{
  dsm_item_type *                   response;
  wds_create_profile_resp_msg_v01   resp_msg;
  wds_create_profile_req_msg_v01*   req_msg = NULL;
  qmi_error_type_v01                errval;
  ds_profile_status_etype           profile_status;
  uint16                            profile_num;
  ds_profile_config_type            profile_config = {0};
  dsm_item_type *    sdu_dup;
  int32              temp;
  uint32             sdu_len;
  ds_profile_status_etype           temp_profile_status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
  temp_profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
  response       = NULL;
  errval         = QMI_ERR_NONE_V01;
  profile_num    = 0;
  memset(&resp_msg,0,sizeof(resp_msg));

  PS_SYSTEM_HEAP_MEM_ALLOC(req_msg,
                           sizeof(wds_create_profile_req_msg_v01),
                           wds_create_profile_req_msg_v01*);
  if(req_msg == NULL)
  {
    qmi_svc_put_result_tlv(&response, (qmi_result_e_type)QMI_RESULT_FAILURE_V01, 
                           (qmi_error_e_type)QMI_ERR_NO_MEMORY_V01);
    return response;
  }
  else
  {
    memset(req_msg,0,sizeof(wds_create_profile_req_msg_v01));
  }

  sdu_dup = NULL;
  sdu_len = dsm_length_packet(*sdu_in);

  temp = PS_UTILS_DSM_DUP_PACKET( &sdu_dup, *sdu_in, 0, (uint16)sdu_len);
  if (0 == temp && sdu_len > 0)
  {
    dsm_free_packet(sdu_in);
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_error;
  }
  errval = QMI_WDSI_DECODE_REQUEST_MSG(QMI_WDS_CREATE_PROFILE_REQ_V01,
                                       sdu_in,
                                       req_msg,
                                       sizeof(wds_create_profile_req_msg_v01));
  if(errval != QMI_ERR_NONE_V01)
  {
    goto send_error;
  }
  if (req_msg->persistent_valid)
  {
    LOG_MSG_INFO2_1 ( "Persistence TLV (%d)", req_msg->persistent );
  }
  /*-------------------------------------------------------------------------
    Call the DS Profile API to create a profile
  -------------------------------------------------------------------------*/
  if (req_msg->persistent_valid)
  {
    profile_config.config_mask |= DS_PROFILE_CONFIG_MASK_PERSISTENCE;
    profile_config.is_persistent = req_msg->persistent;
  }

  profile_config.config_mask |= DS_PROFILE_CONFIG_MASK_SUBS_ID;
  profile_config.subs_id = subs_id;
  
  profile_status = ds_profile_create_ex( (ds_profile_tech_etype)req_msg->profile_type, 
                                        &profile_config,
                                        &profile_num );

  if (profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    LOG_MSG_ERROR_2 ("Cannot create profile for tech type %d. Error %d",
                     req_msg->profile_type, profile_status);
    errval = QMI_ERR_EXTENDED_INTERNAL_V01;
    dsm_free_packet(&sdu_dup);
    goto send_error;
  }

  /*-------------------------------------------------------------------------
    Modify the newly created profile with the input parameters. If the 
    modify returns fail delete the created profile. The sdu_in is freed
    by modify API in success as well as fail case.
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO1_1("calling modify set %d",req_msg->profile_type);
  response = qmi_wds_profile_modify_settings(&sdu_dup,
                                             req_msg->profile_type,
                                             (uint8)profile_num,
                                             subs_id,
                                             &errval,
                                             &temp_profile_status);

  if (response == NULL || errval != QMI_ERR_NONE_V01)
  {
    (void) ds_profile_delete_per_sub((ds_profile_tech_etype)req_msg->profile_type,
                                     profile_num,
                                     subs_id);
    PS_SYSTEM_HEAP_MEM_FREE(req_msg);
    return response;
  }
  
  /*Free the response and create a new one with profile type and number*/
  dsm_free_packet(&response);
  response = NULL;
  resp_msg.profile.profile_type = req_msg->profile_type;
  resp_msg.profile.profile_index = (uint8)profile_num;

send_error:
  if (profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    resp_msg.extended_error_code_valid = TRUE;
    resp_msg.extended_error_code = 
           (wds_ds_extended_error_code_enum_v01)profile_status;
  }
  else if (temp_profile_status == DS_PROFILE_REG_SET_IDENT_NOT_SUPPORTED)
  {
    resp_msg.extended_error_code_valid = TRUE;
    resp_msg.extended_error_code = 
           (wds_ds_extended_error_code_enum_v01)temp_profile_status;
  }
  PS_SYSTEM_HEAP_MEM_FREE(req_msg);
  QMI_WDSI_ENCODE_RESPONSE_MSG(QMI_WDS_CREATE_PROFILE_RESP_V01,
                               &resp_msg,
                               sizeof(resp_msg),
                               errval,
                               &response);
  return response;
}/* qmi_wds_profile_create() */


/*===========================================================================
  FUNCTION QMI_WDS_PROFILE_DELETE()
 
  DESCRIPTION
  Deletes a specified profile for the specified technology.
 
  PARAMETERS
    profile_type     : profile type (3GPP/3GPP2)
    profile_index    : profile number that should be deleted.
 
  RETURN VALUE
    ptr to response
 
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
 ==========================================================================*/
dsm_item_type * qmi_wds_profile_delete
(
  uint8           profile_type,
  uint8           profile_index,
  uint32          subs_id
)
{
  dsm_item_type *                   response;
  wds_delete_profile_resp_msg_v01   resp_msg;
  qmi_error_type_v01                errval;
  ds_profile_status_etype           profile_status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
  response       = NULL;
  errval         = QMI_ERR_NONE_V01;
  memset(&resp_msg,0,sizeof(resp_msg));
  /*-------------------------------------------------------------------------
    Call the DS Profile API to reset the profile to default values
  -------------------------------------------------------------------------*/
  profile_status = ds_profile_delete_per_sub((ds_profile_tech_etype)profile_type,
                                             (ds_profile_num_type)profile_index,
                                             subs_id);
  if (profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    LOG_MSG_ERROR_3 ("Cannot delete profile (%d) Tech (%d) Error (%d)",
                     profile_index, profile_type, profile_status);
    errval = QMI_ERR_EXTENDED_INTERNAL_V01;
    goto send_result;
  }

send_result:
  if (profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    resp_msg.extended_error_code_valid = TRUE;
    resp_msg.extended_error_code = (wds_ds_extended_error_code_enum_v01)profile_status;
  }  
  QMI_WDSI_ENCODE_RESPONSE_MSG(QMI_WDS_DELETE_PROFILE_RESP_V01,
                               &resp_msg,
                               sizeof(resp_msg),
                               errval,
                               &response);
  return response;
} /* qmi_wds_profile_delete() */


/*===========================================================================
  FUNCTION QMI_WDS_PROFILE_UMTS_VALIDATE_PDP_TYPE()

  DESCRIPTION
    Validate the pdp_type set in the profile is IP

  PARAMETERS
    profile_num

  RETURN VALUE
    TRUE   - pdp_type is PDP-IP
    FALSE  - otherwise
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_wds_profile_umts_validate_pdp_type
(
  uint8  profile_num,
  uint32 subs_id
)
{

  ds_profile_info_type                param_info;
  byte                                value[WDS_MAX_PROFILE_PARAM_VAL_LEN];
  qmi_error_type_v01                  errval;
  ds_profile_3gpp_pdp_type_enum_type  pdp_type;

  errval = QMI_ERR_NONE_V01;
  pdp_type = DS_PROFILE_3GPP_PDP_MAX;
  memset(&param_info, 0, sizeof(param_info));
  memset(value, 0, sizeof(value));

  param_info.len = sizeof(value);
  param_info.buf = (void*)value;

  errval = qmi_wds_profile_get_param_id_info
                     ( WDS_PROFILE_TYPE_3GPP, 
                       profile_num, 
                       DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE, 
                       subs_id,
                       &param_info );
  
  if(errval == QMI_ERR_NONE_V01)
  {
    pdp_type = *((ds_profile_3gpp_pdp_type_enum_type*)param_info.buf);
    if((pdp_type != DS_PROFILE_3GPP_PDP_IP)
        && (pdp_type != DS_PROFILE_3GPP_PDP_IPV4V6)
#ifdef FEATURE_DATA_PS_IPV6
       && (pdp_type != DS_PROFILE_3GPP_PDP_IPV6)
#endif /* FEATURE_DATA_PS_IPV6 */
      )
    {
      errval = QMI_ERR_INVALID_PDP_TYPE_V01;
    }
  }
  return errval;
} /* qmi_wds_profile_umts_validate_pdp_type() */


/*===========================================================================
  FUNCTION QMI_WDS_PROFILE_RETRIEVE_DEFAULT_PROFILE_NUM()
  
  DESCRIPTION
  Get the default profile number for tech type.
  
  PARAMETERS
   profile_type (IN)  : Profile Type (3GPP/3GPP2)
   profile_num  (OUT) : Default Profile Number
  
  RETURN VALUE
  None
  
  DEPENDENCIES
    None
  
  SIDE EFFECTS
    None
===========================================================================*/
void qmi_wds_profile_retrieve_default_profile_num
(
  uint8                        profile_type,
  uint8                        profile_family,
  uint16                      *default_profile_num,
  uint32                       subs_id
)
{
  ds_profile_status_etype           profile_status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
  *default_profile_num = 0;

  /*-------------------------------------------------------------------------
    Call the API to get the default profile number.
  -------------------------------------------------------------------------*/
  profile_status = 
    ds_profile_get_default_profile_num_per_subs((ds_profile_tech_etype)profile_type, 
                                        (uint32)profile_family,
                                                (ds_profile_subs_etype)subs_id,
                                        default_profile_num);
  if (profile_status != DS_PROFILE_REG_RESULT_SUCCESS && 
      profile_status != DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE)
  {
    LOG_MSG_ERROR_2 ("Get Default Profile for type %d failed. Error %d",
                     profile_type, profile_status);
  }

  return;
  
} /* qmi_wds_profile_retrieve_default_profile_num() */

#ifdef FEATURE_DATA_LTE
/*===========================================================================
  FUNCTION QMI_WDS_PROFILE_UPDATE_LTE_ATTACH_PDN_LIST_PARAMS ()
 
  DESCRIPTION
  Updates the profile parameters in the lte attach pdn list of the 
  specified technology.
 
  PARAMETERS
    profile_type     : profile type (3GPP/3GPP2)
 
  RETURN VALUE
    ptr to response
 
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
 ==========================================================================*/
dsm_item_type * qmi_wds_profile_update_lte_attach_pdn_list_params
(
  uint8           profile_type,
  uint32          subs_id
)
{
  dsm_item_type *                   response;
  wds_update_lte_attach_pdn_list_profiles_resp_msg_v01 resp_msg;
  qmi_error_type_v01                errval;
  ds_profile_status_etype           profile_status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
  response       = NULL;
  errval         = QMI_ERR_NONE_V01;
  memset(&resp_msg,0,sizeof(resp_msg));
  /*-------------------------------------------------------------------------
    Call the DS Profile API to update the LTE attach PDN list profile params
  -------------------------------------------------------------------------*/
  profile_status = ds_profile_update_lte_attach_pdn_list_profiles_per_subs(
                     (ds_profile_tech_etype)profile_type,
                     (ds_profile_subs_etype)subs_id);

  if (profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    LOG_MSG_ERROR_2 ("Cannot update LTE attach PDN profiles Tech(%d) Error(%d)",
                     profile_type, profile_status);
    errval = QMI_ERR_EXTENDED_INTERNAL_V01;
    
    goto send_result;
  }

send_result:
  QMI_WDSI_ENCODE_RESPONSE_MSG(QMI_WDS_UPDATE_LTE_ATTACH_PDN_LIST_PROFILES_RESP_V01,
                               &resp_msg,
                               sizeof(resp_msg),
                               errval,
                               &response);
  return response;
} /* qmi_wds_update_lte_attach_pdn_list_profiles() */
#endif /* FEATURE_DATA_LTE */



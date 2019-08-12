/*===========================================================================
  @file ds_Profile_Profile.cpp

  This file defines Params3GPPTech class which implements 3GPP Technology Params 
  subcomponent of Profile class.

  DEPENDENCIES: None

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-03-02 am Created module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_Profile_Params3GPPTech.h"
#include "ds_Profile_ParamsCommonTech.h"
#include "ds_Profile_Params.h"
#include "ds_profile_db.h"
#include <stringl/stringl.h>

extern "C"
{
#include "ds_3gpp_profile_api.h"
#include "datamodem_variation.h"
}

using namespace ds::Profile;

#define DB_FILL_PARAM_MAP_3GPP_INIT() DB_FILL_PARAM_MAP_INIT(ds_profile_db_3gpp_tech_params_type);
/*===========================================================================

                    STATIC MEMBER INIT

===========================================================================*/
validate_fn_type Params3GPPTech::validate_fn = NULL;
/*===========================================================================

                     INTERNAL HELPER FUNCTIONS

===========================================================================*/
static ds_profile_db_result_type Ident_Tech_To_Common(ds_profile_db_ident_type tech_ident,
                                                      ds_profile_db_ident_type* common_ident)
{
  switch (tech_ident)
  {
    case(TECH_3GPP_PROFILE_PARAM_APN_DISABLE_FLAG):
      *common_ident = COMMON_TECH_PROFILE_PARAM_APN_DISABLE_FLAG;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP_PROFILE_PARAM_AUTH_TYPE):
      *common_ident = COMMON_TECH_PROFILE_PARAM_AUTH_TYPE;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP_PROFILE_PARAM_AUTH_PASSWORD):
      *common_ident = COMMON_TECH_PROFILE_PARAM_AUTH_PASSWORD;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP_PROFILE_PARAM_AUTH_USERNAME):
      *common_ident = COMMON_TECH_PROFILE_PARAM_AUTH_USERNAME;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP_PROFILE_PARAM_DNS_ADDR_V4_PRIMARY):
      *common_ident = COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V4_PRIMARY;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP_PROFILE_PARAM_DNS_ADDR_V4_SECONDARY):
      *common_ident = COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V4_SECONDARY;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP_PROFILE_PARAM_DNS_ADDR_V6_PRIMARY):
      *common_ident = COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V6_PRIMARY;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP_PROFILE_PARAM_DNS_ADDR_V6_SECONDARY):
      *common_ident = COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V6_SECONDARY;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP_PROFILE_PARAM_PCSCF_REQ_FLAG):
      *common_ident = COMMON_TECH_PROFILE_PARAM_PCSCF_REQ_FLAG;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP_PROFILE_PARAM_APN_CLASS):
      *common_ident = COMMON_TECH_PROFILE_PARAM_APN_CLASS;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP_PROFILE_PARAM_LINGER_PARAMS):
      *common_ident = COMMON_TECH_PROFILE_PARAM_LINGER_PARAMS;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP_PROFILE_PARAM_OPERATOR_RESERVED_PCO):
      *common_ident = COMMON_TECH_PROFILE_PARAM_OPERATOR_RESERVED_PCO;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP_PROFILE_PARAM_MCC):
      *common_ident = COMMON_TECH_PROFILE_PARAM_MCC;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP_PROFILE_PARAM_MNC):
      *common_ident = COMMON_TECH_PROFILE_PARAM_MNC;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP_PROFILE_PARAM_USER_APP_DATA):
      *common_ident = COMMON_TECH_PROFILE_PARAM_USER_APP_DATA;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE):
      *common_ident = COMMON_TECH_PROFILE_PARAM_PDP_TYPE;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP_PROFILE_PARAM_DNS_DHCP_REQ_FLAG):
      *common_ident = COMMON_TECH_PROFILE_PARAM_DNS_DHCP_REQ_FLAG;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP_PROFILE_PARAM_PCSCF_DHCP_REQ_FLAG):
      *common_ident = COMMON_TECH_PROFILE_PARAM_PCSCF_DHCP_REQ_FLAG;
      return DB_RESULT_SUCCESS;
    default:
      return DB_RESULT_ERR_INVAL_IDENT;
  }      
}
/*===========================================================================

                     INTERNAL VALIDATION FUNCTIONS

===========================================================================*/
ds_profile_db_result_type 
Params3GPPTech::ValidateGeneral(const ds_profile_db_3gpp_tech_params_type* profile) 
{
  return ds_3gpp_profile_validation_gen(profile);
}
/*===========================================================================

                     INTERNAL SPECIAL SET FUNCTIONS

===========================================================================*/
static void SpecialSetPdpContext(
  const ds_profile_db_info_type* info,
  void*                          params)
{
  ds_profile_db_3gpp_tech_params_type* prf_3gpp = 
    (ds_profile_db_3gpp_tech_params_type*)params;

  prf_3gpp->is_valid.pdp = true;
}

static void SpecialSetQosReqUmts(
  const ds_profile_db_info_type* info,
  void*                          params,
  boolean is_set) 
{
  ds_profile_db_3gpp_tech_params_type* prf_3gpp = 
    (ds_profile_db_3gpp_tech_params_type*)params;
  
  prf_3gpp->is_valid.qos_req_umts = is_set;
}

static void SpecialSetQosMinUMTS(
  const ds_profile_db_info_type* info,
  void*                          params,
  boolean is_set) 
{
  ds_profile_db_3gpp_tech_params_type* prf_3gpp = 
    (ds_profile_db_3gpp_tech_params_type*)params;

  prf_3gpp->is_valid.qos_min_umts = is_set;
}

static void SpecialSetQosReqGPRS(
  const ds_profile_db_info_type* info,
  void*                          params,
  boolean is_set) 
{
  ds_profile_db_3gpp_tech_params_type* prf_3gpp = 
    (ds_profile_db_3gpp_tech_params_type*)params;

  prf_3gpp->is_valid.qos_req_gprs = is_set;
}

static void SpecialSetQosMinGPRS(
  const ds_profile_db_info_type* info,
  void*                          params,
  boolean is_set) 
{
  ds_profile_db_3gpp_tech_params_type* prf_3gpp = 
    (ds_profile_db_3gpp_tech_params_type*)params;

  prf_3gpp->is_valid.qos_min_gprs = is_set;
}

static void SpecialSetTftFilter1(
  const ds_profile_db_info_type* info,
  void*                          params,
  boolean is_set) 
{
  ds_profile_db_3gpp_tech_params_type* prf_3gpp = 
    (ds_profile_db_3gpp_tech_params_type*)params;

  prf_3gpp->is_valid.tft[DS_PROFILE_DB_3GPP_TFT_FILTER_ID1] = is_set;
}

static void SpecialSetTftFilter2(
  const ds_profile_db_info_type* info,
  void*                          params,
  boolean is_set) 
{
  ds_profile_db_3gpp_tech_params_type* prf_3gpp = 
    (ds_profile_db_3gpp_tech_params_type*)params;

  prf_3gpp->is_valid.tft[DS_PROFILE_DB_3GPP_TFT_FILTER_ID2] = is_set;
}

static void SpecialSetQosReqLTE(
  const ds_profile_db_info_type* info,
  void*                          params,
  boolean is_set) 
{
  ds_profile_db_3gpp_tech_params_type* prf_3gpp = 
    (ds_profile_db_3gpp_tech_params_type*)params;

  prf_3gpp->is_valid.qos_req_lte = is_set;
}

static void SpecialSetPdpAddrV4(
  const ds_profile_db_info_type* info,
  void*                          params)
{
  ds_profile_db_3gpp_tech_params_type* prf_3gpp = 
    (ds_profile_db_3gpp_tech_params_type*)params;

  if (DB_IP_V6 == prf_3gpp->pdp_context.pdp_addr.ip_vsn)
  {
    prf_3gpp->pdp_context.pdp_addr.ip_vsn = DB_IP_V4V6;
  }

  prf_3gpp->is_valid.pdp = true;
}

static void SpecialSetPdpAddrV6(
  const ds_profile_db_info_type* info,
  void*                          params)
{
  ds_profile_db_3gpp_tech_params_type* prf_3gpp = 
    (ds_profile_db_3gpp_tech_params_type*)params;

  if (DB_IP_V4 == prf_3gpp->pdp_context.pdp_addr.ip_vsn)
  {
    prf_3gpp->pdp_context.pdp_addr.ip_vsn = DB_IP_V4V6;
  }

  prf_3gpp->is_valid.pdp = true;
}

void Params3GPPTech::SpecialSet(
  ds_profile_db_ident_type ident, 
  const ds_profile_db_info_type* info,
  void*              params,
  boolean is_set
  ) 
{
  ds_profile_db_ident_type common_ident;
  if (DB_RESULT_SUCCESS == Ident_Tech_To_Common(ident, &common_ident))
  {
    return ParamsCommonTech::SpecialSet(common_ident, info, params, is_set);
  }

  switch (ident)
  {
    case(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE):
    case(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_H_COMP):
    case(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_D_COMP):
    case(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_TE_MT_ACCESS_CTRL_FLAG):
    case(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_NUMBER):
    case(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_SECONDARY_FLAG):
    case(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_PRIMARY_ID):
    case(TECH_3GPP_PROFILE_PARAM_IPV4_ADDR_ALLOC):
      return SpecialSetPdpContext(info, params);
    case(TECH_3GPP_PROFILE_PARAM_UMTS_REQ_QOS_EXTENDED):
    case(TECH_3GPP_PROFILE_PARAM_UMTS_REQ_QOS):
      return SpecialSetQosReqUmts(info, params, is_set);
    case(TECH_3GPP_PROFILE_PARAM_UMTS_MIN_QOS_EXTENDED):
    case(TECH_3GPP_PROFILE_PARAM_UMTS_MIN_QOS):
      return SpecialSetQosMinUMTS(info, params, is_set);
    case(TECH_3GPP_PROFILE_PARAM_GPRS_REQ_QOS):
      return SpecialSetQosReqGPRS(info, params, is_set);
    case(TECH_3GPP_PROFILE_PARAM_GPRS_MIN_QOS):
      return SpecialSetQosMinGPRS(info, params, is_set);
    case(TECH_3GPP_PROFILE_PARAM_LTE_REQ_QOS):
      return SpecialSetQosReqLTE(info, params, is_set);
    case(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_ADDR_V4):
      return SpecialSetPdpAddrV4(info, params);
    case(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_ADDR_V6):
      return SpecialSetPdpAddrV6(info, params);
    case(TECH_3GPP_PROFILE_PARAM_TFT_FILTER_ID1):
      return SpecialSetTftFilter1(info, params, is_set);
    case(TECH_3GPP_PROFILE_PARAM_TFT_FILTER_ID2):
      return SpecialSetTftFilter2(info, params, is_set);
    default:
      return;
  }
}

/*===========================================================================

                     INTERNAL SPECIAL GET FUNCTIONS

===========================================================================*/
static ds_profile_db_result_type SpecialGetPdpContext(
  ds_profile_db_info_type*       info,
  const void*                    params)
{
  ds_profile_db_3gpp_tech_params_type* prf_3gpp = 
    (ds_profile_db_3gpp_tech_params_type*)params;

  if (!prf_3gpp->is_valid.pdp)
  {
    return DB_RESULT_ERR_VALID_FLAG_NOT_SET; 
  }

  return DB_RESULT_SUCCESS;
}

static ds_profile_db_result_type SpecialGetQosReqUmts(
  ds_profile_db_info_type*       info,
  const void*                    params)
{
  ds_profile_db_3gpp_tech_params_type* prf_3gpp = 
    (ds_profile_db_3gpp_tech_params_type*)params;

  if (!prf_3gpp->is_valid.qos_req_umts)
  {
    return DB_RESULT_ERR_VALID_FLAG_NOT_SET; 
  }

  return DB_RESULT_SUCCESS;
}

static ds_profile_db_result_type SpecialGetQosMinUMTS(
  ds_profile_db_info_type*       info,
  const void*                    params)
{
  ds_profile_db_3gpp_tech_params_type* prf_3gpp = 
    (ds_profile_db_3gpp_tech_params_type*)params;

  if (!prf_3gpp->is_valid.qos_min_umts)
  {
    return DB_RESULT_ERR_VALID_FLAG_NOT_SET; 
  }

  return DB_RESULT_SUCCESS;
}

static ds_profile_db_result_type SpecialGetQosReqGPRS(
  ds_profile_db_info_type*       info,
  const void*                    params)
{
  ds_profile_db_3gpp_tech_params_type* prf_3gpp = 
    (ds_profile_db_3gpp_tech_params_type*)params;

  if (!prf_3gpp->is_valid.qos_req_gprs)
  {
    return DB_RESULT_ERR_VALID_FLAG_NOT_SET; 
  }

  return DB_RESULT_SUCCESS;
}

static ds_profile_db_result_type SpecialGetQosMinGPRS(
  ds_profile_db_info_type*       info,
  const void*                    params)
{
  ds_profile_db_3gpp_tech_params_type* prf_3gpp = 
    (ds_profile_db_3gpp_tech_params_type*)params;

  if (!prf_3gpp->is_valid.qos_min_gprs)
  {
    return DB_RESULT_ERR_VALID_FLAG_NOT_SET; 
  }

  return DB_RESULT_SUCCESS;
}

static ds_profile_db_result_type SpecialGetTftFilter1(
  ds_profile_db_info_type*       info,
  const void*                    params)
{
  ds_profile_db_3gpp_tech_params_type* prf_3gpp = 
    (ds_profile_db_3gpp_tech_params_type*)params;

  if (!prf_3gpp->is_valid.tft[DS_PROFILE_DB_3GPP_TFT_FILTER_ID1])
  {
    return DB_RESULT_ERR_VALID_FLAG_NOT_SET; 
  }

  return DB_RESULT_SUCCESS;
}

static ds_profile_db_result_type SpecialGetTftFilter2(
  ds_profile_db_info_type*       info,
  const void*                    params)
{
  ds_profile_db_3gpp_tech_params_type* prf_3gpp = 
    (ds_profile_db_3gpp_tech_params_type*)params;

  if (!prf_3gpp->is_valid.tft[DS_PROFILE_DB_3GPP_TFT_FILTER_ID2])
  {
    return DB_RESULT_ERR_VALID_FLAG_NOT_SET; 
  }

  return DB_RESULT_SUCCESS;
}

static ds_profile_db_result_type SpecialGetQosReqLTE(
  ds_profile_db_info_type*       info,
  const void*                    params)
{
  ds_profile_db_3gpp_tech_params_type* prf_3gpp = 
    (ds_profile_db_3gpp_tech_params_type*)params;

  if (!prf_3gpp->is_valid.qos_req_lte)
  {
    return DB_RESULT_ERR_VALID_FLAG_NOT_SET; 
  }

  return DB_RESULT_SUCCESS;
}

static ds_profile_db_result_type SpecialGetPdpAddrV4(
  ds_profile_db_info_type*       info,
  const void*                    params)
{
  ds_profile_db_3gpp_tech_params_type* prf_3gpp = 
    (ds_profile_db_3gpp_tech_params_type*)params;

  if (!prf_3gpp->is_valid.pdp)
  {
    return DB_RESULT_ERR_VALID_FLAG_NOT_SET; 
  }

  if (prf_3gpp->pdp_context.pdp_addr.ip_vsn == DB_IP_V6)
  {
    return DB_RESULT_ERR_INVAL;
  }

  return DB_RESULT_SUCCESS;
}

static ds_profile_db_result_type SpecialGetPdpAddrV6(
  ds_profile_db_info_type*       info,
  const void*                    params)
{
  ds_profile_db_3gpp_tech_params_type* prf_3gpp = 
    (ds_profile_db_3gpp_tech_params_type*)params;

  if (!prf_3gpp->is_valid.pdp)
  {
    return DB_RESULT_ERR_VALID_FLAG_NOT_SET; 
  }

  if (prf_3gpp->pdp_context.pdp_addr.ip_vsn == DB_IP_V4)
  {
    return DB_RESULT_ERR_INVAL;
  }

  return DB_RESULT_SUCCESS;

}

ds_profile_db_result_type Params3GPPTech::SpecialGet(
  ds_profile_db_ident_type ident, 
  ds_profile_db_info_type* info,
  const void*              params) 
{
  ds_profile_db_ident_type common_ident;

  /* for 3GPP profiles some features are disabled */
  if (validate_fn)
  {
    ds_profile_db_result_type res = validate_fn(ident, info, params);
    if (DB_RESULT_NOT_SUPPORTED == res ||
        DB_RESULT_NO_EMERGENCY_PDN_SUPPORT == res)
    {
      return res;
    }
  }

  if (DB_RESULT_SUCCESS == Ident_Tech_To_Common(ident, &common_ident))
  {
    return ParamsCommonTech::SpecialGet(common_ident, info, params);
  }

  switch (ident)
  {
    case(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE):
    case(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_H_COMP):
    case(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_D_COMP):
    case(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_TE_MT_ACCESS_CTRL_FLAG):
    case(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_NUMBER):
    case(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_SECONDARY_FLAG):
    case(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_PRIMARY_ID):
    case(TECH_3GPP_PROFILE_PARAM_IPV4_ADDR_ALLOC):
      return SpecialGetPdpContext(info, params);
    case(TECH_3GPP_PROFILE_PARAM_UMTS_REQ_QOS_EXTENDED):
    case(TECH_3GPP_PROFILE_PARAM_UMTS_REQ_QOS):
      return SpecialGetQosReqUmts(info, params);
    case(TECH_3GPP_PROFILE_PARAM_UMTS_MIN_QOS_EXTENDED):
    case(TECH_3GPP_PROFILE_PARAM_UMTS_MIN_QOS):
      return SpecialGetQosMinUMTS(info, params);
    case(TECH_3GPP_PROFILE_PARAM_GPRS_REQ_QOS):
      return SpecialGetQosReqGPRS(info, params);
    case(TECH_3GPP_PROFILE_PARAM_GPRS_MIN_QOS):
      return SpecialGetQosMinGPRS(info, params);
    case(TECH_3GPP_PROFILE_PARAM_LTE_REQ_QOS):
      return SpecialGetQosReqLTE(info, params);
    case(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_ADDR_V4):
      return SpecialGetPdpAddrV4(info, params);
    case(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_ADDR_V6):
      return SpecialGetPdpAddrV6(info, params);
    case(TECH_3GPP_PROFILE_PARAM_TFT_FILTER_ID1):
      return SpecialGetTftFilter1(info, params);
    case(TECH_3GPP_PROFILE_PARAM_TFT_FILTER_ID2):
      return SpecialGetTftFilter2(info, params);
    default:
      return DB_RESULT_SUCCESS;
  }
}

/*===========================================================================

                     INTERNAL COMPARE FUNCTIONS

===========================================================================*/
bool Params3GPPTech::CompareIdent(
  ds_profile_db_ident_type ident, 
  const ds_profile_db_info_type* info1,
  const ds_profile_db_info_type* info2)
{
  ds_profile_db_ident_type common_ident;
  if (DB_RESULT_SUCCESS == Ident_Tech_To_Common(ident, &common_ident))
  {
    return ParamsCommonTech::CompareIdent(common_ident, info1, info2);
  }

  return (memcmp(info1->buf, info2->buf, info1->len) == 0) ? true : false ;
}
/*===========================================================================

                    MEMBER FUNCTIONS

===========================================================================*/
ds_profile_db_result_type Params3GPPTech::Init(Params::ParamInfo* mapParamsInfo)
{
  DB_FILL_PARAM_MAP_3GPP_INIT();

  /* common tech params */
  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_APN_DISABLE_FLAG,
                    tech_common_params.apn_disable_flag);

  DB_FILL_PARAM_MAP_WITH_SPECIAL_SIZE(TECH_3GPP_PROFILE_PARAM_AUTH_TYPE,
                                      tech_common_params.auth_info.auth_type,
                                      1);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_AUTH_PASSWORD,
                    tech_common_params.auth_info.password);
 
  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_AUTH_USERNAME,
                    tech_common_params.auth_info.username);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_DNS_ADDR_V4_PRIMARY,
                    tech_common_params.dns_addr.primary_addr.v4);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_DNS_ADDR_V4_SECONDARY,
                    tech_common_params.dns_addr.secondary_addr.v4);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_DNS_ADDR_V6_PRIMARY,
                    tech_common_params.dns_addr.primary_addr.v6);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_DNS_ADDR_V6_SECONDARY,
                    tech_common_params.dns_addr.secondary_addr.v6);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_PCSCF_REQ_FLAG,
                    tech_common_params.request_pcscf_address_flag);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_APN_CLASS,
                    tech_common_params.apn_class);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_LINGER_PARAMS,
                    tech_common_params.iface_linger_params.linger_params);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_OPERATOR_RESERVED_PCO,
                    tech_common_params.op_pco_id);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_MCC,
                    tech_common_params.mcc);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_MNC,
                    tech_common_params.mnc);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_USER_APP_DATA,
                    tech_common_params.user_app_data);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_DNS_DHCP_REQ_FLAG,
                    tech_common_params.request_dns_address_using_dhcp_flag);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_PCSCF_DHCP_REQ_FLAG,
                    tech_common_params.request_pcscf_address_using_dhcp_flag);
 
  /* tech specific params */
  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE,
                    pdp_context.pdp_type);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_H_COMP,
                    pdp_context.h_comp);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_D_COMP,
                    pdp_context.d_comp);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_UMTS_REQ_QOS,
                    qos_request_umts); 

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_UMTS_MIN_QOS,
                    qos_minimum_umts);                 

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_GPRS_REQ_QOS,
                    qos_request_gprs);                

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_GPRS_MIN_QOS,
                    qos_minimum_gprs);                 

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_ADDR_V4,
                    pdp_context.pdp_addr.addr.v4);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_TE_MT_ACCESS_CTRL_FLAG,
                    pdp_context.access_ctrl_flag);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_IM_CN_FLAG,
                    im_cn_flag);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_TFT_FILTER_ID1,
                    tft[DS_PROFILE_DB_3GPP_TFT_FILTER_ID1]);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_TFT_FILTER_ID2,
                    tft[DS_PROFILE_DB_3GPP_TFT_FILTER_ID2]);                

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_NUMBER,
                    pdp_context.pdp_context_number);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_SECONDARY_FLAG,
                    pdp_context.secondary_flag);       

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_PRIMARY_ID,
                    pdp_context.primary_id);                                

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_ADDR_V6,
                    pdp_context.pdp_addr.addr.v6);

  DB_FILL_PARAM_MAP_WITH_SPECIAL_SIZE(TECH_3GPP_PROFILE_PARAM_UMTS_REQ_QOS_EXTENDED,
                                      qos_request_umts,
                                      sizeof(params.qos_request_umts)-1);

  DB_FILL_PARAM_MAP_WITH_SPECIAL_SIZE(TECH_3GPP_PROFILE_PARAM_UMTS_MIN_QOS_EXTENDED,
                                      qos_minimum_umts,
                                      sizeof(params.qos_minimum_umts)-1);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_IPV4_ADDR_ALLOC,
                    pdp_context.ipv4_addr_alloc);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_LTE_REQ_QOS,
                    qos_request_lte);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_INACTIVITY_TIMER_VAL,
                    inactivity_timer_val);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_SRC_STAT_DESC_REQ,
                    src_stat_desc_req);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_SRC_STAT_DESC_MIN,
                    src_stat_desc_min);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_APN_BEARER,
                    apn_bearer);  

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_EMERGENCY_CALLS_SUPPORTED,
                    emergency_calls_are_supported);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_MAX_PDN_CONN_PER_BLOCK,
                    max_pdn_conn_per_blk);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_MAX_PDN_CONN_TIMER,
                    max_pdn_conn_time);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_PDN_REQ_WAIT_TIMER,
                    pdn_req_wait_time);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_ROAMING_DISALLOWED,
                    roaming_disallowed);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_PDN_DISCON_WAIT_TIME,
                    pdn_discon_wait_time);
					
  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_LTE_ROAMING_PDP_TYPE,
                    lte_pdp_type);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_UMTS_ROAMING_PDP_TYPE,
                    umts_pdp_type);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_IWLAN_TO_LTE_ROAMING_HO_FLAG,
                    roaming_ho_iwlan_to_lte);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_LTE_TO_IWLAN_ROAMING_HO_FLAG,
                    roaming_ho_lte_to_iwlan);
					
  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_FAILURE_TIMERS,
                    failure_timers);

  DB_FILL_PARAM_MAP(TECH_3GPP_PROFILE_PARAM_OVERRIDE_HOME_PDP_TYPE,
                    override_home_ip_type);
  return DB_RESULT_SUCCESS;
}


void Params3GPPTech::GetDefaultCParams(void* params)
{
  tech_func_table_type *tech_table_3gpp = ds_profile_db_get_tech_tbl(DB_TECH_3GPP);

  if (!params)
  {
    return;
  }

  if (tech_table_3gpp)
  {
    if (tech_table_3gpp->get_default_profile)
    {
      tech_table_3gpp->get_default_profile(params);
    }
  } 
  else
  {
    memset(params, 0, sizeof(ds_profile_db_3gpp_tech_params_type));
  }
  return;
}

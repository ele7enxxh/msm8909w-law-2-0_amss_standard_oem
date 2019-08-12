/*===========================================================================
  @file ds_Profile_Profile.cpp

  This file implements ParamsCommonTech class which implements Common Technology Params 
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
#include "ds_Profile_ParamsCommonTech.h"
#include "ds_profile_db.h"
#include <stringl/stringl.h>

using namespace ds::Profile;

#define DB_FILL_PARAM_MAP_COMMON_INIT() DB_FILL_PARAM_MAP_INIT(ds_profile_db_common_tech_params_type);
/*===========================================================================

                    STATIC MEMBER INIT

===========================================================================*/
/*===========================================================================

                     INTERNAL HELPER FUNCTIONS

===========================================================================*/

/*===========================================================================

                     INTERNAL VALIDATION FUNCTIONS

===========================================================================*/
static ds_profile_db_result_type 
ValidateAuthProtocol(
  const ds_profile_db_info_type*  info,
  const void*                     profile_params,
  ...)
{
  ds_profile_db_auth_protocol auth_protocol;

  if (!info->len)
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  memset(&auth_protocol, 0, sizeof(auth_protocol));
  memscpy(&auth_protocol, sizeof(auth_protocol), info->buf, info->len);
  
  switch (auth_protocol)
  {
    case DB_DATA_SESS_AUTH_PROTOCOL_NONE:
    case DB_DATA_SESS_AUTH_PROTOCOL_PAP:
    case DB_DATA_SESS_AUTH_PROTOCOL_CHAP:
    case DB_DATA_SESS_AUTH_PROTOCOL_PAP_CHAP:
    case DB_DATA_SESS_AUTH_PROTOCOL_EAP:
      break;
    default:
      return DB_RESULT_ERR_INVAL;
  }

  return DB_RESULT_SUCCESS;
}

static ds_profile_db_result_type ValidateIpVersion(ds_profile_db_ip_version version) 
{
  switch (version)
  {
    case DB_IP_V4:
    case DB_IP_V6:
    case DB_IP_V4V6:
      break;
    default:
      return DB_RESULT_ERR_INVAL;
  }

  return DB_RESULT_SUCCESS;
}

ds_profile_db_result_type 
ParamsCommonTech::ValidateGeneral(const ds_profile_db_common_tech_params_type* profile) 
{
  ds_profile_db_result_type res;
  if (DB_RESULT_SUCCESS != (res = ValidateIpVersion(profile->dns_addr.primary_ip_version)))
  {
    return res;
  }
  
  if (DB_RESULT_SUCCESS != (res = ValidateIpVersion(profile->dns_addr.secondary_ip_version)))
  {
    return res;
  }

  return DB_RESULT_SUCCESS;
}


static ds_profile_db_result_type 
ValidateIdent(
  ds_profile_db_ident_type        ident, 
  const ds_profile_db_info_type*  info,
  const void*                     profile_params,
  ...)
{
  switch (ident)
  {
    case(COMMON_TECH_PROFILE_PARAM_AUTH_TYPE):
      return ValidateAuthProtocol(info, profile_params);
    default:
      return DB_RESULT_SUCCESS;    
  }
}

validate_fn_type ParamsCommonTech::validate_fn = &ValidateIdent;
/*===========================================================================

                     INTERNAL SPECIAL SET FUNCTIONS

===========================================================================*/
static void SpecialSetDnsV4Primary(
  const ds_profile_db_info_type* info,
  void*                          params)
{
  ds_profile_db_common_tech_params_type* profile =
    (ds_profile_db_common_tech_params_type*)params;

  if (profile->dns_addr.primary_ip_version == DB_IP_V6)
  {
    profile->dns_addr.primary_ip_version = DB_IP_V4V6;
  }
}

static void SpecialSetDnsV6Primary(
  const ds_profile_db_info_type* info,
  void*                          params)
{
  ds_profile_db_common_tech_params_type* profile =
    (ds_profile_db_common_tech_params_type*)params;

  if (profile->dns_addr.primary_ip_version == DB_IP_V4)
  {
    profile->dns_addr.primary_ip_version = DB_IP_V4V6;
  }
}

static void SpecialSetDnsV4Secondary(
  const ds_profile_db_info_type* info,
  void*                          params)
{
  ds_profile_db_common_tech_params_type* profile =
    (ds_profile_db_common_tech_params_type*)params;

  if (profile->dns_addr.secondary_ip_version == DB_IP_V6)
  {
    profile->dns_addr.secondary_ip_version = DB_IP_V4V6;
  }
}

static void SpecialSetDnsV6Secondary(
  const ds_profile_db_info_type* info,
  void*                          params)
{
  ds_profile_db_common_tech_params_type* profile =
    (ds_profile_db_common_tech_params_type*)params;

  if (profile->dns_addr.secondary_ip_version == DB_IP_V4)
  {
    profile->dns_addr.secondary_ip_version = DB_IP_V4V6;
  }
}

static void SpecialSetLingerParams(
  const ds_profile_db_info_type* info,
  void*                          params)
{
  ds_profile_db_common_tech_params_type* profile =
    (ds_profile_db_common_tech_params_type*)params;

  profile->iface_linger_params.is_valid = true;
}

void ParamsCommonTech::SpecialSet(
  ds_profile_db_ident_type       ident, 
  const ds_profile_db_info_type* info,
  void*                          params,
  boolean is_set)
{  
  switch (ident)
  {
    case(COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V4_PRIMARY):
      return SpecialSetDnsV4Primary(info, params);
    case(COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V4_SECONDARY):
      return SpecialSetDnsV4Secondary(info, params);
    case(COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V6_PRIMARY):
      return SpecialSetDnsV6Primary(info, params);
    case(COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V6_SECONDARY):
      return SpecialSetDnsV6Secondary(info, params);
    case(COMMON_TECH_PROFILE_PARAM_LINGER_PARAMS):
      return SpecialSetLingerParams(info, params);
    default:
      return;
  }
}

/*===========================================================================

                     INTERNAL SPECIAL GET FUNCTIONS

===========================================================================*/
static ds_profile_db_result_type SpecialGetAuthPass(
  ds_profile_db_info_type*       info,
  const void*                    params)
{
  ds_profile_db_common_tech_params_type* profile =
    (ds_profile_db_common_tech_params_type*)params;

  info->len = strnlen(profile->auth_info.password, sizeof(profile->auth_info.password));

  return DB_RESULT_SUCCESS;
}

static ds_profile_db_result_type SpecialGetAuthName(
  ds_profile_db_info_type*       info,
  const void*                    params)
{
  ds_profile_db_common_tech_params_type* profile =
    (ds_profile_db_common_tech_params_type*)params;

  info->len = strnlen(profile->auth_info.username, sizeof(profile->auth_info.username));

  return DB_RESULT_SUCCESS;
}

static ds_profile_db_result_type SpecialGetDnsV4Primary(
  ds_profile_db_info_type*       info,
  const void*                    params)
{
  ds_profile_db_common_tech_params_type* profile =
    (ds_profile_db_common_tech_params_type*)params;

  if (profile->dns_addr.primary_ip_version == DB_IP_V6)
  {
    return DB_RESULT_ERR_INVAL;
  }

  return DB_RESULT_SUCCESS;
}

static ds_profile_db_result_type SpecialGetDnsV6Primary(
  ds_profile_db_info_type*       info,
  const void*                    params)
{
  ds_profile_db_common_tech_params_type* profile =
    (ds_profile_db_common_tech_params_type*)params;

  if (profile->dns_addr.primary_ip_version == DB_IP_V4)
  {
    return DB_RESULT_ERR_INVAL;
  }

  return DB_RESULT_SUCCESS;
}

static ds_profile_db_result_type SpecialGetDnsV4Secondary(
  ds_profile_db_info_type*       info,
  const void*                    params)
{
  ds_profile_db_common_tech_params_type* profile =
    (ds_profile_db_common_tech_params_type*)params;

  if (profile->dns_addr.secondary_ip_version == DB_IP_V6)
  {
    return DB_RESULT_ERR_INVAL;
  }

  return DB_RESULT_SUCCESS;
}

static ds_profile_db_result_type SpecialGetDnsV6Secondary(
  ds_profile_db_info_type*       info,
  const void*                    params)
{
  ds_profile_db_common_tech_params_type* profile =
    (ds_profile_db_common_tech_params_type*)params;

  if (profile->dns_addr.secondary_ip_version == DB_IP_V4)
  {
    return DB_RESULT_ERR_INVAL;
  }

  return DB_RESULT_SUCCESS;
}

static ds_profile_db_result_type SpecialGetLingerParams(
  ds_profile_db_info_type*       info,
  const void*                    params)
{
  ds_profile_db_common_tech_params_type* profile =
    (ds_profile_db_common_tech_params_type*)params;

  if (!profile->iface_linger_params.is_valid)
  {
    return DB_RESULT_ERR_VALID_FLAG_NOT_SET; 
  }

  return DB_RESULT_SUCCESS;
}


ds_profile_db_result_type ParamsCommonTech::SpecialGet(
  ds_profile_db_ident_type ident, 
  ds_profile_db_info_type* info,
  const void*              params)
{  
  switch (ident)
  {     
    case(COMMON_TECH_PROFILE_PARAM_AUTH_PASSWORD):
      return SpecialGetAuthPass(info, params); 
    case(COMMON_TECH_PROFILE_PARAM_AUTH_USERNAME):
      return SpecialGetAuthName(info, params);
    case(COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V4_PRIMARY):
      return SpecialGetDnsV4Primary(info, params);
    case(COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V4_SECONDARY):
      return SpecialGetDnsV4Secondary(info, params);
    case(COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V6_PRIMARY):
      return SpecialGetDnsV6Primary(info, params);
    case(COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V6_SECONDARY):
      return SpecialGetDnsV6Secondary(info, params);
    case(COMMON_TECH_PROFILE_PARAM_LINGER_PARAMS):
      return SpecialGetLingerParams(info, params);
    default:
      return DB_RESULT_SUCCESS;
  }
}

/*===========================================================================

                     INTERNAL COMPARE FUNCTIONS

===========================================================================*/
bool ParamsCommonTech::CompareIdent(
  ds_profile_db_ident_type ident, 
  const ds_profile_db_info_type* info1,
  const ds_profile_db_info_type* info2)
{
  return (memcmp(info1->buf, info2->buf, info1->len) == 0) ? true : false ;
}
/*===========================================================================

                    MEMBER FUNCTIONS

===========================================================================*/

ds_profile_db_result_type ParamsCommonTech::Init(Params::ParamInfo* mapParamsInfo)
{
  DB_FILL_PARAM_MAP_COMMON_INIT();

  DB_FILL_PARAM_MAP(COMMON_TECH_PROFILE_PARAM_APN_DISABLE_FLAG,
                    apn_disable_flag);

  DB_FILL_PARAM_MAP_WITH_SPECIAL_SIZE(COMMON_TECH_PROFILE_PARAM_AUTH_TYPE,
                                      auth_info.auth_type,
                                      1);

  DB_FILL_PARAM_MAP(COMMON_TECH_PROFILE_PARAM_AUTH_USERNAME,
                    auth_info.username);

  DB_FILL_PARAM_MAP(COMMON_TECH_PROFILE_PARAM_AUTH_PASSWORD,
                    auth_info.password);

  DB_FILL_PARAM_MAP(COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V4_PRIMARY,
                    dns_addr.primary_addr.v4);

  DB_FILL_PARAM_MAP(COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V4_SECONDARY,
                    dns_addr.secondary_addr.v4);
                 
  DB_FILL_PARAM_MAP(COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V6_PRIMARY,
                    dns_addr.primary_addr.v6);

  DB_FILL_PARAM_MAP(COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V6_SECONDARY,
                    dns_addr.secondary_addr.v6);                    

  DB_FILL_PARAM_MAP(COMMON_TECH_PROFILE_PARAM_PCSCF_REQ_FLAG,
                    request_pcscf_address_flag);

  DB_FILL_PARAM_MAP(COMMON_TECH_PROFILE_PARAM_APN_CLASS,
                    apn_class);
           
  DB_FILL_PARAM_MAP(COMMON_TECH_PROFILE_PARAM_LINGER_PARAMS,
                    iface_linger_params.linger_params);

  DB_FILL_PARAM_MAP(COMMON_TECH_PROFILE_PARAM_OPERATOR_RESERVED_PCO,
                    op_pco_id);

  DB_FILL_PARAM_MAP(COMMON_TECH_PROFILE_PARAM_MCC,
                    mcc);

  DB_FILL_PARAM_MAP(COMMON_TECH_PROFILE_PARAM_MNC,
                    mnc);

  DB_FILL_PARAM_MAP(COMMON_TECH_PROFILE_PARAM_USER_APP_DATA,  
                    user_app_data);

  DB_FILL_PARAM_MAP(COMMON_TECH_PROFILE_PARAM_PDP_TYPE,
                    pdp_type);

  DB_FILL_PARAM_MAP(COMMON_TECH_PROFILE_PARAM_PCSCF_DHCP_REQ_FLAG,  
                    request_pcscf_address_using_dhcp_flag);

  DB_FILL_PARAM_MAP(COMMON_TECH_PROFILE_PARAM_DNS_DHCP_REQ_FLAG,  
                    request_dns_address_using_dhcp_flag);

  return DB_RESULT_SUCCESS;
}

void ParamsCommonTech::GetDefaultCParams(void* params)
{
  if (!params)
  {
    return;
  }

  ds_profile_db_common_tech_params_type* common_params = (ds_profile_db_common_tech_params_type*)params;

  memset(params, 0, sizeof(ds_profile_db_common_tech_params_type));

  common_params->dns_addr.primary_ip_version   = DB_IP_V4;
  common_params->dns_addr.secondary_ip_version = DB_IP_V4;
}

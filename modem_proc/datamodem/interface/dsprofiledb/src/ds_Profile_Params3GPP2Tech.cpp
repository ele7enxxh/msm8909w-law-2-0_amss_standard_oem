/*===========================================================================
  @file ds_Profile_Profile.cpp

  This file defines Params3GPP2Tech class which implements 3GPP2 Technology Params 
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
#include "ds_Profile_Params3GPP2Tech.h"
#include "ds_Profile_ParamsCommonTech.h"
#include "ds_Profile_Params.h"
#include "ds_Profile_Log.h"
#include "ds_profile_db.h"
#include <stringl/stringl.h>

extern "C"
{
#include "ds_profile_3gpp2_mgr.h"
}

using namespace ds::Profile;


/*===========================================================================

                    Various Globals definitions

===========================================================================*/
#define DB_FILL_PARAM_MAP_3GPP2_INIT() DB_FILL_PARAM_MAP_INIT(ds_profile_db_3gpp2_tech_params_type);
/*===========================================================================

                    STATIC MEMBER INIT

===========================================================================*/
validate_fn_type Params3GPP2Tech::validate_fn = NULL;
/*===========================================================================

                     INTERNAL HELPER FUNCTIONS

===========================================================================*/
static ds_profile_db_result_type Ident_Tech_To_Common(ds_profile_db_ident_type tech_ident,
                                                      ds_profile_db_ident_type* common_ident)
{
  switch (tech_ident)
  {
    case(TECH_3GPP2_PROFILE_PARAM_APN_ENABLED):
      *common_ident = COMMON_TECH_PROFILE_PARAM_APN_DISABLE_FLAG;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP2_PROFILE_PARAM_AUTH_PROTOCOL):
      *common_ident = COMMON_TECH_PROFILE_PARAM_AUTH_TYPE;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP2_PROFILE_PARAM_AUTH_PASSWORD):
      *common_ident = COMMON_TECH_PROFILE_PARAM_AUTH_PASSWORD;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP2_PROFILE_PARAM_USER_ID):
      *common_ident = COMMON_TECH_PROFILE_PARAM_AUTH_USERNAME;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP2_PROFILE_PARAM_V4_DNS_ADDR_PRIMARY):
      *common_ident = COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V4_PRIMARY;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP2_PROFILE_PARAM_V4_DNS_ADDR_SECONDARY):
      *common_ident = COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V4_SECONDARY;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP2_PROFILE_PARAM_V6_DNS_ADDR_PRIMARY):
      *common_ident = COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V6_PRIMARY;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP2_PROFILE_PARAM_V6_DNS_ADDR_SECONDARY):
      *common_ident = COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V6_SECONDARY;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP2_PROFILE_PARAM_IS_PCSCF_ADDR_NEEDED):
      *common_ident = COMMON_TECH_PROFILE_PARAM_PCSCF_REQ_FLAG;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP2_PROFILE_PARAM_APN_CLASS):
      *common_ident = COMMON_TECH_PROFILE_PARAM_APN_CLASS;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP2_PROFILE_PARAM_LINGER_PARAMS):
      *common_ident = COMMON_TECH_PROFILE_PARAM_LINGER_PARAMS;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP2_PROFILE_PARAM_ALLOW_LINGER):
      *common_ident = COMMON_TECH_PROFILE_PARAM_LINGER_PARAMS;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP2_PROFILE_PARAM_OP_PCO_ID):
      *common_ident = COMMON_TECH_PROFILE_PARAM_OPERATOR_RESERVED_PCO;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP2_PROFILE_PARAM_MCC):
      *common_ident = COMMON_TECH_PROFILE_PARAM_MCC;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP2_PROFILE_PARAM_MNC):
      *common_ident = COMMON_TECH_PROFILE_PARAM_MNC;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP2_PROFILE_PARAM_USER_APP_DATA):
      *common_ident = COMMON_TECH_PROFILE_PARAM_USER_APP_DATA;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP2_PROFILE_PARAM_PDN_TYPE):
      *common_ident = COMMON_TECH_PROFILE_PARAM_PDP_TYPE;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP2_PROFILE_PARAM_DNS_DHCP_REQ_FLAG):
      *common_ident = COMMON_TECH_PROFILE_PARAM_DNS_DHCP_REQ_FLAG;
      return DB_RESULT_SUCCESS;
    case(TECH_3GPP2_PROFILE_PARAM_PCSCF_DHCP_REQ_FLAG):
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
Params3GPP2Tech::ValidateGeneral(const ds_profile_db_3gpp2_tech_params_type* profile) 
{
  tech_func_table_type *tech_table_3gpp2 = ds_profile_db_get_tech_tbl(DB_TECH_3GPP2);
  if (tech_table_3gpp2)
  {
    if (tech_table_3gpp2->validate_general)
    {
      return tech_table_3gpp2->validate_general((void*)profile);
    }
  }
  return DB_RESULT_SUCCESS;
}
/*===========================================================================

                     INTERNAL SPECIAL SET FUNCTIONS

===========================================================================*/
void Params3GPP2Tech::SpecialSet(
  ds_profile_db_ident_type ident, 
  const ds_profile_db_info_type* info,
  void*              params,
  boolean is_set) 
{

  ds_profile_db_ident_type common_ident;
  if (DB_RESULT_SUCCESS == Ident_Tech_To_Common(ident, &common_ident))
  {
    return ParamsCommonTech::SpecialSet(common_ident, info, params, is_set);
  }
}

/*===========================================================================

                     INTERNAL SPECIAL GET FUNCTIONS

===========================================================================*/
static ds_profile_db_result_type SpecialGetPdnLabel(
  ds_profile_db_info_type*       info,
  const void*                    params)
{
  ds_profile_db_3gpp2_tech_params_type* prf_3gpp2 = 
    (ds_profile_db_3gpp2_tech_params_type*)params;

  info->len = strnlen(prf_3gpp2->pdn_label, sizeof(prf_3gpp2->pdn_label));
  return DB_RESULT_SUCCESS;
}

static ds_profile_db_result_type SpecialGetPdnLevelUserId(
  ds_profile_db_info_type*       info,
  const void*                    params)
{
  ds_profile_db_3gpp2_tech_params_type* prf_3gpp2 = 
    (ds_profile_db_3gpp2_tech_params_type*)params;

  info->len = strnlen(prf_3gpp2->pdn_level_auth_info.username, 
                      sizeof(prf_3gpp2->pdn_level_auth_info.username));
  return DB_RESULT_SUCCESS;
}

static ds_profile_db_result_type SpecialGetPdnLevelAuthPassword(
  ds_profile_db_info_type*       info,
  const void*                    params)
{
  ds_profile_db_3gpp2_tech_params_type* prf_3gpp2 = 
    (ds_profile_db_3gpp2_tech_params_type*)params;

  info->len = strnlen(prf_3gpp2->pdn_level_auth_info.password, 
                      sizeof(prf_3gpp2->pdn_level_auth_info.password));
  return DB_RESULT_SUCCESS;
}


ds_profile_db_result_type Params3GPP2Tech::SpecialGet(
  ds_profile_db_ident_type ident, 
  ds_profile_db_info_type* info,
  const void*              params) 
{

  ds_profile_db_ident_type common_ident;

  /* for 3GPP2 profiles it is forbidden to get parameters that do not belong to your
     profile sub-type (KDDI / OMH / etc. ) */
  if (validate_fn)
  {
    ds_profile_db_result_type res = validate_fn(ident, info, params);
    if (DB_RESULT_NOT_SUPPORTED == res ||
        DB_RESULT_ERR_3GPP2_INVALID_IDENT_FOR_PROFILE == res)
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
    case(TECH_3GPP2_PROFILE_PARAM_PDN_LEVEL_USER_ID):
      return SpecialGetPdnLevelUserId(info, params);
    case(TECH_3GPP2_PROFILE_PARAM_PDN_LEVEL_AUTH_PASSWORD):
      return SpecialGetPdnLevelAuthPassword(info, params);
    case(TECH_3GPP2_PROFILE_PARAM_PDN_LABEL):
      return SpecialGetPdnLabel(info, params);
    default:
      return DB_RESULT_SUCCESS;
  }
}

/*===========================================================================

                     INTERNAL COMPARE FUNCTIONS

===========================================================================*/
bool Params3GPP2Tech::CompareIdent(
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
ds_profile_db_result_type Params3GPP2Tech::Init(Params::ParamInfo* mapParamsInfo)
{
  DB_FILL_PARAM_MAP_3GPP2_INIT();


  /* common tech params */
  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_APN_ENABLED,
                    tech_common_params.apn_disable_flag);

  DB_FILL_PARAM_MAP_WITH_SPECIAL_SIZE(TECH_3GPP2_PROFILE_PARAM_AUTH_PROTOCOL,
                                      tech_common_params.auth_info.auth_type,
                                      1);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_AUTH_PASSWORD,
                    tech_common_params.auth_info.password);
 
  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_USER_ID,
                    tech_common_params.auth_info.username);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_V4_DNS_ADDR_PRIMARY,
                    tech_common_params.dns_addr.primary_addr.v4);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_V4_DNS_ADDR_SECONDARY,
                    tech_common_params.dns_addr.secondary_addr.v4);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_V6_DNS_ADDR_PRIMARY,
                    tech_common_params.dns_addr.primary_addr.v6);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_V6_DNS_ADDR_SECONDARY,
                    tech_common_params.dns_addr.secondary_addr.v6);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_IS_PCSCF_ADDR_NEEDED,
                    tech_common_params.request_pcscf_address_flag);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_APN_CLASS,
                    tech_common_params.apn_class);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_LINGER_PARAMS,
                    tech_common_params.iface_linger_params.linger_params);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_ALLOW_LINGER,
                    tech_common_params.iface_linger_params.linger_params.allow_linger_flag);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_OP_PCO_ID,
                    tech_common_params.op_pco_id);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_MCC,
                    tech_common_params.mcc);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_MNC,
                    tech_common_params.mnc);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_USER_APP_DATA,
                    tech_common_params.user_app_data);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_DNS_DHCP_REQ_FLAG,
                    tech_common_params.request_dns_address_using_dhcp_flag);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_PCSCF_DHCP_REQ_FLAG,
                    tech_common_params.request_pcscf_address_using_dhcp_flag);

  /* tech specific params */
  
  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_PDN_TYPE,
                    pdn_type);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_NEGOTIATE_DNS_SERVER,
                    negotiate_dns_server);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_SESSION_CLOSE_TIMER_DO,
                    ppp_session_close_timer_DO);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_SESSION_CLOSE_TIMER_1X,
                    ppp_session_close_timer_1X); 

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_LCP_ACK_TIMEOUT,
                    lcp_ack_timeout);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_IPCP_ACK_TIMEOUT,
                    ipcp_ack_timeout);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_AUTH_TIMEOUT,
                    auth_timeout);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_LCP_CREQ_RETRY_COUNT,
                    lcp_creq_retry_count);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_IPCP_CREQ_RETRY_COUNT,
                    ipcp_creq_retry_count);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_AUTH_RETRY_COUNT,
                    auth_retry_count);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_DATA_RATE,
                    data_rate);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_DATA_MODE,
                    data_mode);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_APP_TYPE,
                    app_type);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_APP_PRIORITY,
                    app_priority);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_RAT_TYPE,
                    rat_type);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_PDN_INACTIVITY_TIMEOUT,
                    pdn_inactivity_timeout);

  DB_FILL_PARAM_MAP_WITH_SPECIAL_SIZE(TECH_3GPP2_PROFILE_PARAM_PDN_LEVEL_AUTH_PROTOCOL,
                                      pdn_level_auth_info.auth_type,
                                      1);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_PDN_LEVEL_USER_ID,
                    pdn_level_auth_info.username);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_PDN_LEVEL_AUTH_PASSWORD,
                    pdn_level_auth_info.password);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_PDN_LABEL,
                    pdn_label);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_FAILURE_TIMER_1,
                    failure_timer[0]);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_FAILURE_TIMER_2,
                    failure_timer[1]);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_FAILURE_TIMER_3,
                    failure_timer[2]);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_FAILURE_TIMER_4,
                    failure_timer[3]);
  
  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_FAILURE_TIMER_5,
                    failure_timer[4]);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_FAILURE_TIMER_6,
                    failure_timer[5]);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_1,
                    disallow_timer[0]);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_2,
                    disallow_timer[1]);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_3,
                    disallow_timer[2]);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_4,
                    disallow_timer[3]);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_5,
                    disallow_timer[4]);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_6,
                    disallow_timer[5]);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_DISALLOW_TIMERS,
                    disallow_timer);

  DB_FILL_PARAM_MAP(TECH_3GPP2_PROFILE_PARAM_FAILURE_TIMERS,
                    failure_timer);

  return DB_RESULT_SUCCESS;
}

void Params3GPP2Tech::GetDefaultCParams(void* params)
{
  tech_func_table_type *tech_table_3gpp2 = ds_profile_db_get_tech_tbl(DB_TECH_3GPP2);

  if (!params)
  {
    return;
  }

  if (tech_table_3gpp2)
  {
    if (tech_table_3gpp2->get_default_profile)
    {
      tech_table_3gpp2->get_default_profile(params);
    }
  }
  else
  {
    memset(params, 0 , sizeof(ds_profile_db_3gpp2_tech_params_type));
  }
  return;
}

ds_profile_db_result_type Params3GPP2Tech::ValidateCParams(
  validate_fn_type         validation_fn,
  ds_profile_db_ident_type start_range,
  ParamInfo* mapParamsInfo,
  unsigned int map_number_of_elements,
  const void *params,
  const ds_profile_db_gen_params_type* gen_params /* = NULL */)
{ 
  ds_profile_db_info_type validate_info;
  ds_profile_db_result_type res;
  ds_profile_db_3gpp2_tech_params_type* params_3gpp2 = (ds_profile_db_3gpp2_tech_params_type*)params;
  ds_profile_db_ident_type ident;
  
  PROFILE_DB_LOG_MED_1("ValidateCParams(): for params 0x%x", params);

  /* there is no validation fn, nothing to check */
  if (!validation_fn)
  {
    return DB_RESULT_SUCCESS;
  }

  for (unsigned int i = 0; i < map_number_of_elements; i++)
  {
    ParamInfo* info = &(mapParamsInfo[i]);

    /* there is no param here */
    if (!info->len)
    {
      continue;
    }

    ident = (ds_profile_db_ident_type)(start_range + i);

    /* For 3GPP2 validation may fail on parameters that do not belong to profile
       tech sub-type (KDDI / OMH / etc). For those DB_RESULT_ERR_3GPP2_INVALID_IDENT_FOR_PROFILE
       is returned from 3gpp2 MH validation function. This should be ignored */
    validate_info.buf = (char*)params + info->offset;
    validate_info.len = info->len;
    res = validation_fn(ident, &validate_info, params, gen_params);
    if (DB_RESULT_SUCCESS != res)
    {
      if (DB_RESULT_ERR_3GPP2_INVALID_IDENT_FOR_PROFILE == res)
      {
        PROFILE_DB_LOG_MED_2("ValidateCParams(): ignoring ident %d as it is inconsistent with 3GPP2 profile subtype %d",
                             ident, params_3gpp2->profile_type);
        continue;
      }
      PROFILE_DB_LOG_ERROR_2("ValidateCParams(): ident %d is not valid, error %d", ident, res);
      return res;
    }    
  }

  return DB_RESULT_SUCCESS;
}
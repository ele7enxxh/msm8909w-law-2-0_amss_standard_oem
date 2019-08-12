/*==============================================================================

                        ds_http_dsnet_iface_info.cpp

GENERAL DESCRIPTION
  Internal iface info structure and functions for DS HTTP

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/29/15    ml     Resolve profile id if it is default before attempting bringup
07/28/15    ml     Created file/Initial version.
==============================================================================*/
#include "ds_http_dsnet_iface_info.h"

#include <stringl/stringl.h>
extern "C"
{
  #include "ds3gmmgsdiif.h"
  #include "ds_profile.h"
}


ds_http_dsnet_iface_info::ds_http_dsnet_iface_info(const ds_http_iface_info_s_type *iface)
: iface_type(DS_HTTP_IFACE_MAX), profile_id(0), subs_id(DS_HTTP_DEFAULT_SUBS)
{
  if(NULL == iface)
    return;

  iface_type = iface->iface_type;
  profile_id = iface->profile_id;
  subs_id    = iface->subs_id;
}



bool ds_http_dsnet_iface_info::operator==(const ds_http_dsnet_iface_info &rhs) const
{
  if(subs_id != rhs.subs_id || iface_type != rhs.iface_type)
    return false;

  switch(iface_type)
  {
    case DS_HTTP_IFACE_3GPP:  // Fallthrough
    case DS_HTTP_IFACE_IWLAN: // Fallthrough
    case DS_HTTP_IFACE_ANY:
      return (profile_id == rhs.profile_id);

    case DS_HTTP_IFACE_WLAN_LB:
      return true;

    default:
      return false;
  }
}


uint32 ds_http_dsnet_iface_info::get_subs_id() const
{
  if(DS_HTTP_DEFAULT_SUBS == subs_id)
  {
    return static_cast<uint32>(ds_http_get_default_subscription());
  }

  return static_cast<uint32>(subs_id);
}



uint16 ds_http_dsnet_iface_info::get_profile_id() const
{
  if(DS_HTTP_IFACE_WLAN_LB == iface_type)
    return 0;

  // Resolve default profile if profile num is 0
  if(0 == profile_id)
  {
    // Get default profile number
    uint16                  prof_num = 0;
    ds_profile_status_etype result   = ds_profile_get_default_profile_num_per_subs(
                                                                                   DS_PROFILE_TECH_3GPP,
                                                                                   DS_PROFILE_EMBEDDED_PROFILE_FAMILY,
                                                                                   (ds_profile_subs_etype)get_subs_id(),
                                                                                   &prof_num
                                                                                   );

    if(DS_PROFILE_REG_RESULT_SUCCESS == result)
    {
      return prof_num;
    }
  }
  return profile_id;
}



ds_http_subscription_e_type ds_http_get_default_subscription()
{
  return ps_sys_subs_id_to_ds_http_subs_id( ps_sys_get_default_data_subscription() );
}



ds_http_subscription_e_type ps_sys_subs_id_to_ds_http_subs_id(ps_sys_subscription_enum_type subs_id)
{
  switch(subs_id)
  {
    case PS_SYS_DEFAULT_SUBS:
      return DS_HTTP_DEFAULT_SUBS;

    case PS_SYS_PRIMARY_SUBS:
      return DS_HTTP_PRIMARY_SUBS;

    case PS_SYS_SECONDARY_SUBS:
      return DS_HTTP_SECONDARY_SUBS;

    case PS_SYS_TERTIARY_SUBS:
      return DS_HTTP_TERTIARY_SUBS;

    case PS_SYS_SUBS_MAX: // Fallthrough
    default:
      return DS_HTTP_SUBS_MAX;
  }
}



ds_http_subscription_e_type sys_subs_id_to_ds_http_subs_id(sys_modem_as_id_e_type subs_id)
{
  switch(subs_id)
  {
    case SYS_MODEM_AS_ID_1:
      return DS_HTTP_PRIMARY_SUBS;

    case SYS_MODEM_AS_ID_2:
      return DS_HTTP_SECONDARY_SUBS;

    case SYS_MODEM_AS_ID_3:
      return DS_HTTP_TERTIARY_SUBS;

    default:
      return DS_HTTP_SUBS_MAX;
  }
}



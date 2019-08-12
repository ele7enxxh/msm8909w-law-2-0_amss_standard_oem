/*==============================================================================

                        ds_http_dsnet_iface_info.h

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
#ifndef DS_HTTP_DSNET_IFACE_INFO_H
#define DS_HTTP_DSNET_IFACE_INFO_H

#include "ds_http_types.h"
#include "ps_sys.h"


/* Iface info needed for dsnet */
struct ds_http_dsnet_iface_info
{
  ds_http_iface_e_type          iface_type;
  uint16                        profile_id; // Ignored if WLAN_LB
  ds_http_subscription_e_type   subs_id;

  ds_http_dsnet_iface_info(const ds_http_iface_info_s_type *iface);
  bool operator==(const ds_http_dsnet_iface_info& rhs) const;

  // If value is default, these functions will resolve
  uint32 get_subs_id() const;
  uint16 get_profile_id() const;
};


ds_http_subscription_e_type ds_http_get_default_subscription();
ds_http_subscription_e_type ps_sys_subs_id_to_ds_http_subs_id(ps_sys_subscription_enum_type subs_id);


#endif /* DS_HTTP_DSNET_IFACE_INFO_H */

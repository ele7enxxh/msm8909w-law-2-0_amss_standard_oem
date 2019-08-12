/*===========================================================================                        ds_andsf_query_data_manager.cpp

                        ds_andsf_utility.h

GENERAL DESCRIPTION
  ANDSF Rule Priority Query API

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*===========================================================================

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who                            what, where, why
--------    ---    ----------------------------------------------------------
04/15/15    ml     Created file/Initial version.
===========================================================================*/
#ifndef DS_ANDSF_UTILITY_H
#define DS_ANDSF_UTILITY_H

#include "datamodem_variation.h"
#ifdef FEATURE_DATA_ANDSF

#include "ds_andsf_internal_defs.h"
#include "ds_sys.h"


#ifdef __cplusplus
extern "C"
{
#endif

boolean ds_andsf_module_enabled();

boolean ds_andsf_is_ds_subs_id_valid(ds_sys_subscription_enum_type subs_id);
boolean ds_andsf_is_andsf_subs_idx_valid(ds_andsf_subs_idx_e_type subs_idx);

ds_andsf_subs_idx_e_type ds_subs_id_to_andsf_index(ds_sys_subscription_enum_type subs_id);
ds_sys_subscription_enum_type andsf_index_to_ds_subs_id(ds_andsf_subs_idx_e_type subs_id);


#ifdef __cplusplus
}
#endif

#endif /* FEATURE_DATA_ANDSF */
#endif /* DS_ANDSF_UTILITY_H */

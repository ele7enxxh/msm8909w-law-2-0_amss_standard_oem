/*===========================================================================
  @file ds_Profile_Params.h

  This file defines abstract ParamsTech class which provides interface for all
  Technology Param classes
  

                Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-04-22 am Created module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_profile_db.h"
#include "ds_Profile_Params.h"
#include "ds_Profile_ParamsTech.h"

using namespace ds::Profile;
/*===========================================================================

                    MEMBER FUNCTIONS

===========================================================================*/
const ds_profile_db_gen_params_type* ParamsTech::GetGeneralCParamsPtr(const ParamsGeneral* paramsGeneral)
{
  if (paramsGeneral)
  {
    return (const ds_profile_db_gen_params_type*)(paramsGeneral->GetCParamsPtr());
  }
  else
  {
    return NULL;
  }
} 


ds_profile_db_result_type ParamsTech::SetParam(
  ds_profile_db_ident_type   ident,
  const ds_profile_db_info_type *set_info,
  boolean is_set)
{
  return Params::SetParam(GetMapParamsPtr(),
                          (void*)GetCParamsPtr(), 
                          ident, 
                          set_info,
                          GetGeneralCParamsPtr(paramsGeneral),
                          is_set);
}


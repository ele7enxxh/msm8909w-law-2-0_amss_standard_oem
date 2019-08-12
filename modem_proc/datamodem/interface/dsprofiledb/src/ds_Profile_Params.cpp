/*===========================================================================
  @file ds_Profile_Params.h

  This file implements abstract Params class which is a general interface for 
  all various param subcomponents of Profile class
  Each Params subcomponent implement a certain set of Profile parameters, it 
  should be completely independent on others
  

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
#include "ds_Profile_Log.h"
#include <stringl/stringl.h>
#include <stdarg.h>
#include "ps_system_heap.h"

extern "C"
{
#include "ds_profile_os.h"
}


using namespace ds::Profile;

/*===========================================================================

                    STATIC MEMBER INIT

===========================================================================*/

/*===========================================================================

                    MEMBER FUNCTIONS

===========================================================================*/
ds_profile_db_result_type Params::InitInstance(void)
{
  if (changed_idents_list)
  {
    /* already inited */
    return DB_RESULT_ERR_INVAL;
  }

  if (ds_util_list_get_hndl(&changed_idents_list) != DS_SUCCESS)
  {
    changed_idents_list = NULL;
    return DB_RESULT_ERR_OUT_OF_MEMORY;
  }

  return DB_RESULT_SUCCESS;
}

ds_profile_db_result_type Params::InitInstance(ds_util_list_hndl_type list)
{
  if (changed_idents_list)
  {
    /* already inited */
    return DB_RESULT_ERR_INVAL;
  }

  if (!list)
  {
    return DB_RESULT_ERR_INVAL;
  }

  if (DS_SUCCESS != ds_util_list_copy(list, &changed_idents_list))
  {
    changed_idents_list = NULL;
    return DB_RESULT_ERR_OUT_OF_MEMORY;
  }

  return DB_RESULT_SUCCESS;
}

Params::~Params(void)
{
  if (changed_idents_list)
  {
    ds_util_list_rel_hndl(changed_idents_list);
  }
}


ds_profile_db_result_type Params::ValidateCParams(
  validate_fn_type         validation_fn,
  ds_profile_db_ident_type start_range,
  ParamInfo *mapParamsInfo,
  unsigned int map_number_of_elements,
  const void *params,
  const ds_profile_db_gen_params_type *gen_params /* = NULL */)
{
  ds_profile_db_info_type validate_info;
  ds_profile_db_result_type res;

  /* there is no validation fn, nothing to check */
  if (!validation_fn)
  {
    return DB_RESULT_SUCCESS;
  }

  for (unsigned int i = 0; i < map_number_of_elements; i++)
  {
    ParamInfo *info = &(mapParamsInfo[i]);

    /* there is no param here */
    if (!info->len)
    {
      continue;
    }

    validate_info.buf = (char *)params + info->offset;
    validate_info.len = info->len;
    res = validation_fn((ds_profile_db_ident_type)(start_range + i), &validate_info, params, gen_params);
    if (DB_RESULT_SUCCESS != res)
    {
      PROFILE_DB_LOG_ERROR_2("ValidateCParams(): ident %d is not valid, error %d", start_range + i, res);
      return res;
    }
  }

  return DB_RESULT_SUCCESS;
}

ds_profile_db_result_type Params::SetParam(
  ds_profile_db_ident_type   ident,
  const ds_profile_db_info_type *info,
  boolean is_set)
{
  return SetParam(GetMapParamsPtr(),
                  (void *)GetCParamsPtr(),
                  ident,
                  info,
                  NULL,
                  is_set);
}

ds_profile_db_result_type Params::GetParam(
  ds_profile_db_ident_type   ident,
  ds_profile_db_info_type *info) const
{
  return GetParam(GetMapParamsPtr(),
                  GetCParamsPtr(),
                  ident,
                  info);
}

bool Params::CompareParam(
  ds_profile_db_ident_type         ident,
  const ds_profile_db_info_type    *info) const
{
  return CompareParam(GetMapParamsPtr(),
                      ident,
                      info);
}

ds_profile_db_result_type Params::GetFieldInfo(
  ds_profile_db_ident_type   ident,
  unsigned int *offset,
  uint16 *len,
  void **buf) const
{
  return GetFieldInfo(GetMapParamsPtr(),
                      GetCParamsPtr(),
                      ident,
                      offset,
                      len,
                      buf);
}

void Params::GetCParams(void *_params) const
{
  if (!_params)
  {
    return;
  }

  const void *params = GetCParamsPtr();
  memscpy(_params, GetCStructSize(), params, GetCStructSize());
}

ds_profile_db_result_type Params::GetParam(
  ParamInfo *mapParamsInfo,
  const void *params,
  ds_profile_db_ident_type ident,
  ds_profile_db_info_type *get_info) const
{
  ParamInfo *info;
  ds_profile_db_result_type res;

  if (!get_info)
  {
    return DB_RESULT_ERR_INVAL;
  }

  if (DB_RESULT_SUCCESS != (res = GetParamInfo(mapParamsInfo, ident, &info)))
  {
    return res;
  }

  /* buffer should be valid and length should be no less than required */
  if (!get_info->buf ||
      get_info->len < info->len)
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  memset(get_info->buf, 0, get_info->len);

  /* update buf len */
  get_info->len = info->len;

  if (GetSpecialGetFn())
  {
    if (DB_RESULT_SUCCESS != (res = GetSpecialGetFn()(ident, get_info, params)))
    {
      return res;
    }
  }

  void *profile_offset_ptr = (char *)params + info->offset;
  memscpy(get_info->buf, get_info->len,  profile_offset_ptr, get_info->len);
  return DB_RESULT_SUCCESS;
}

bool Params::CompareParam(
  ParamInfo *mapParamsInfo,
  ds_profile_db_ident_type ident,
  const ds_profile_db_info_type *compare_info) const
{
  bool res = false;
  ds_profile_db_result_type res2;

  if (!compare_info)
  {
    return false;
  }

  ParamInfo *paramInfo;
  if (DB_RESULT_SUCCESS != (res2 = GetParamInfo(mapParamsInfo, ident, &paramInfo)))
  {
    return res;
  }

  ds_profile_db_info_type temp_info;
  temp_info.len = paramInfo->len;
  PS_SYSTEM_HEAP_MEM_ALLOC ( temp_info.buf, temp_info.len, void*);
  if (!temp_info.buf) 
  {
    return false;
  }

  if (DB_RESULT_SUCCESS != (res2 = GetParam(ident, &temp_info)))
  {
    PROFILE_DB_LOG_ERROR_1("CompareParam(): GetParam() failed with error %d", res2);
    res = false;
    goto bail;
  }

  if (temp_info.len != compare_info->len)
  {
    res = false;
    goto bail;
  }

  if (GetCompareFn())
  {
    res = GetCompareFn()(ident, compare_info, &temp_info);
  } else
  {
    res = (memcmp(compare_info->buf, temp_info.buf, compare_info->len) == 0) ? true : false;
  }

  /* fall through */

bail:
  PS_SYSTEM_HEAP_MEM_FREE(temp_info.buf);

  return res;
}


ds_profile_db_result_type Params::SetParam(
  ParamInfo *mapParamsInfo,
  void *params,
  ds_profile_db_ident_type   ident,
  const ds_profile_db_info_type *set_info,
  const ds_profile_db_gen_params_type *gen_params /* = NULL */,
  boolean is_set /*=true*/)
{
  ds_profile_db_result_type res;
  ParamInfo *info;

  if (!set_info)
  {
    return DB_RESULT_ERR_INVAL;
  }

  if (DB_RESULT_SUCCESS != (res = GetParamInfo(mapParamsInfo, ident, &info)))
  {
    return res;
  }

  if (IsParamReadOnly(ident))
  {
    return DB_RESULT_ERR_INVAL_OP;
  }

  /* provided len should be less or equal to the param actual len
     len can be 0
     if len is non-zero, buffer should be valid */
  if (set_info->len > info->len)
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }
  if (set_info->len && !(set_info->buf))
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  if (GetValidateFn())
  {
    if (DB_RESULT_SUCCESS != (res = GetValidateFn()(ident, set_info, params, gen_params)))
    {
      PROFILE_DB_LOG_ERROR_2("SetParam(): ident %d is not valid, error %d", ident, res);
      return res;
    }
  }

  if (DB_RESULT_SUCCESS != (res = TrackParamChange(ident)))
  {
    return res;
  }

  void *profile_offset_ptr = (char *)params + info->offset;
  memset(profile_offset_ptr, 0, info->len);

  /* if the len is 0, then the param is set to 0 */
  if (set_info->len)
  {
    memscpy(profile_offset_ptr, info->len, set_info->buf, set_info->len);
    if (GetSpecialSetFn())
    {
      GetSpecialSetFn()(ident, set_info, params, is_set);
    }
  }

  return DB_RESULT_SUCCESS;
}


ds_profile_db_result_type Params::GetFieldInfo(
  ParamInfo *mapParamsInfo,
  const void *params,
  ds_profile_db_ident_type   ident,
  unsigned int *offset,
  uint16 *len,
  void **buf) const
{
  ds_profile_db_result_type res;
  ParamInfo *paramInfo;

  if (!offset || !len || !buf)
  {
    return DB_RESULT_ERR_INVAL;
  }

  if (DB_RESULT_SUCCESS != (res = GetParamInfo(mapParamsInfo, ident, &paramInfo)))
  {
    return res;
  }

  *offset = paramInfo->offset;
  *len = paramInfo->len;

  *buf = (char *)params + paramInfo->offset;

  return DB_RESULT_SUCCESS;
}


ds_profile_db_result_type Params::GetParamInfo(
  ParamInfo *mapParamsInfo,
  ds_profile_db_ident_type   ident,
  ParamInfo **paramInfo) const
{
  int index = IdentToIndex(ident);
  if (-1 == index)
  {
    return DB_RESULT_ERR_INVAL_IDENT;
  }

  if (!(mapParamsInfo[index].len))
  {
    return DB_RESULT_ERR_INVAL_IDENT;
  }

  *paramInfo = &(mapParamsInfo[index]);

  return DB_RESULT_SUCCESS;
}

static int track_param(void *data, va_list args)
{
  ds_profile_db_ident_type *ident;
  int res = -1;

  ident = va_arg(args, ds_profile_db_ident_type *);
  if ((*(ds_profile_db_ident_type *)data) == *ident)
  {
    res = 0;
  }

  return res;
}

ds_profile_db_result_type Params::TrackParamChange(ds_profile_db_ident_type ident)
{
  int res;
  /* todo optimize logic here */

  /* not inited */
  if (!changed_idents_list)
  {
    return DB_RESULT_FAIL;
  }

  boolean ident_found;
  res = ds_util_list_do_for_every_node(changed_idents_list,
                                       track_param,
                                       true,
                                       &ident_found,
                                       &ident);

  if (ident_found)
  {
    return DB_RESULT_SUCCESS;
  }

  if (DS_SUCCESS != res)
  {
    return DB_RESULT_FAIL;
  }

  if (ds_util_list_add(changed_idents_list, &ident, sizeof(ident)) != DS_SUCCESS)
  {
    return DB_RESULT_FAIL;
  }

  return DB_RESULT_SUCCESS;
}

void Params::ResetParamTracking(void)
{
  /* not inited */
  if (!changed_idents_list)
  {
    return;
  }

  ds_util_list_reset(changed_idents_list);
}

ds_util_list_hndl_type Params::GetChangedIdentsList(void) const
{
  return changed_idents_list;
}

bool Params::IsParamReadOnly(ds_profile_db_ident_type ident) const
{
  return false;
}

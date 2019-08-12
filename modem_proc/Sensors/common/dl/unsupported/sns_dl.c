/*==============================================================================
  @file sns_dl.c

  @brief
  Empty implementation of the SNS Dynamic Loading service (DL svc).  Used on
  targets and configurations where dynamic loading is not enabled or not
  available.

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
==============================================================================*/
#include <string.h>
#include <stringl.h>
#include "sns_init.h"
#include "sns_dl.h"

typedef struct sns_dl_svc_s sns_dl_svc_t;

//------------------------------------------------------------------------------
//  FUNCTIONS
//------------------------------------------------------------------------------
sns_err_code_e sns_dl_set_attr_name(sns_dl_attr_t *p_attr, const char *name,
    size_t name_sz)
{
  return SNS_ERR_NOTSUPPORTED;
}

sns_err_code_e sns_dl_get_attr_name(sns_dl_attr_t *p_attr, char *name,
    size_t name_sz)
{
  return SNS_ERR_NOTSUPPORTED;
}

sns_err_code_e sns_dl_init(void)
{
  return SNS_ERR_NOTSUPPORTED;
}

sns_err_code_e sns_dl_deinit(void)
{
  return SNS_SUCCESS;
}

sns_err_code_e sns_dl_svc_destroy(sns_dl_svc_handle_t svc_handle)
{
  return SNS_ERR_NOTSUPPORTED;
}

sns_err_code_e sns_dl_svc_is_initialized(sns_dl_svc_handle_t svc_handle,
    pfn_is_initialized_cb pfn_init_cb, void *init_cb_param, uint32_t timeout_us)
{
  return SNS_ERR_NOTSUPPORTED;
}

sns_err_code_e sns_dl_reg_for_each_algorithm(sns_dl_svc_handle_t svc_handle,
    pfn_sns_reg_for_each pfn, sns_reg_dyn_cfg_t *p_cfg, uint32_t timeout_us)

{
  return SNS_ERR_NOTSUPPORTED;
}

sns_err_code_e sns_dl_reg_for_each_driver(sns_dl_svc_handle_t svc_handle,
    pfn_sns_reg_for_each pfn, sns_reg_dyn_cfg_t *p_cfg, uint32_t timeout_us)
{
  return SNS_ERR_NOTSUPPORTED;
}

sns_err_code_e sns_dl_reg_search_uuid(sns_dl_svc_handle_t svc_handle,
    void* id, size_t id_sz, sns_reg_dyn_cfg_t *p_cfg, uint32_t timeout_us)
{
  return SNS_ERR_NOTSUPPORTED;
}

void sns_dl_svc_print_state(sns_dl_svc_handle_t svc_handle)
{
}

void sns_dl_svc_handle_error(sns_dl_svc_t *p_svc, sns_err_code_e err_code)
{
}

sns_err_code_e sns_dl_svc_check_and_initialize(sns_dl_svc_t *p_svc)
{
  return SNS_ERR_NOTSUPPORTED;
}

void sns_dl_svc_reg_cmd_unlock_svc(sns_dl_svc_t *p_svc)
{
}

sns_err_code_e sns_dl_reg_test_hook_timing_for_each_algo(
    sns_dl_svc_handle_t svc_handle, pfn_sns_reg_for_each pfn,
    sns_reg_dyn_cfg_t *p_cfg, uint32_t timeout_us)
{
  return SNS_ERR_NOTSUPPORTED;
}

sns_err_code_e sns_dl_reg_test_hook_timing_for_each_driver(
    sns_dl_svc_handle_t svc_handle, pfn_sns_reg_for_each pfn,
    sns_reg_dyn_cfg_t *p_cfg, uint32_t timeout_us)
{
  return SNS_ERR_NOTSUPPORTED;
}

sns_err_code_e sns_dl_reg_test_hook_timing_search_uuid(
    sns_dl_svc_handle_t svc_handle, void* id, size_t id_sz,
    sns_reg_dyn_cfg_t *p_cfg, uint32_t timeout_us)
{
  return SNS_ERR_NOTSUPPORTED;
}

sns_err_code_e sns_dl_attr_init(sns_dl_attr_t *p_attr)
{
  return SNS_ERR_NOTSUPPORTED;
}

sns_err_code_e sns_dl_attr_deinit(sns_dl_attr_t *p_attr)
{
  return SNS_ERR_NOTSUPPORTED;
}

sns_err_code_e sns_dl_set_attr_allocator(sns_dl_attr_t *p_attr,
    pfn_sns_dl_malloc pfn_malloc, pfn_sns_dl_free pfn_free)
{
  return SNS_ERR_NOTSUPPORTED;
}

sns_err_code_e sns_dl_set_attr_blocking_info(sns_dl_attr_t *p_attr,
    bool should_block, uint32_t timeout_us, pfn_is_initialized_cb pfn,
    void *init_cb_param)
{
  return SNS_ERR_NOTSUPPORTED;
}

sns_err_code_e sns_dl_attr_use_local_registry(sns_dl_attr_t *p_attr)
{
  return SNS_ERR_NOTSUPPORTED;
}

sns_err_code_e sns_dl_svc_create(sns_dl_svc_handle_t *p_handle,
    sns_dl_attr_t *p_attr)
{
  return SNS_ERR_NOTSUPPORTED;
}

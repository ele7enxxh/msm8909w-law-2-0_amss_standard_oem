/*============================================================================

 Copyright (c) 2012-2013,2015 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/
#ifndef __CHROMATIX_SUB_MODULE_H__
#define __CHROMATIX_SUB_MODULE_H__

#include "chromatix.h"
#include "chromatix_common.h"
#include "chromatix_adrc.h"

typedef struct {
  void                      *chromatix_lib_handle;
  void                      *common_chromatix_lib_handle;
  void                      *liveshot_chromatix_lib_handle;
  void                      *snapshot_chromatix_lib_handle;
  void                      *adrc_chromatix_handle;
  chromatix_parms_type      *chromatix_ptr;
  chromatix_VFE_common_type *common_chromatix_ptr;
  chromatix_parms_type      *liveshot_chromatix_ptr;
  chromatix_parms_type      *snapshot_chromatix_ptr;
  chromatix_adrc_type       *adrc_chromatix_ptr;
  chromatix_parms_type       chromatixData;
  chromatix_VFE_common_type  common_chromatixData;
  chromatix_parms_type       liveshot_chromatixData;
  chromatix_parms_type       snapshot_chromatixData;
  char                      *cur_chromatix_name;
  char                      *cur_chromatix_common_name;
  int                        reloaded_chromatix;
  int                        reloaded_chromatix_common;
} sensor_chromatix_data_t;

#endif

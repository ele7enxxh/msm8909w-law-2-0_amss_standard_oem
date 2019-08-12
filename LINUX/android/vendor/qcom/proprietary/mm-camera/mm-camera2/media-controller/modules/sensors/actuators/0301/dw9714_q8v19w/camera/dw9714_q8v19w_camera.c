/*============================================================================

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/

#include "af_algo_tuning.h"

static af_algo_ctrl_t af_algo_data = {
#include "dw9714_q8v19w_camera.h"
};

void *dw9714_q8v19w_camera_af_algo_open_lib(void)
{
  return &af_algo_data;
}

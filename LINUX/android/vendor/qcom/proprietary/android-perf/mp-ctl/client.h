/******************************************************************************
  @file    client.h
  @brief   Android performance PerfLock library

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

int perf_lock_acq(int, int, int[], int);
int perf_lock_rel(int);
void perf_lock_reset(void);
int perf_lock_use_profile(int, int);

#ifdef __cplusplus
}
#endif

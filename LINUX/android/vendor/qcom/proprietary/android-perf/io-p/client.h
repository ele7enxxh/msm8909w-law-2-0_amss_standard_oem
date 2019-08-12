/******************************************************************************
  @file    client.h
  @brief   Android performance iop library

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

int perf_io_prefetch_start(int pid, const char *pkg_name);
int perf_io_prefetch_stop();


#ifdef __cplusplus
}
#endif

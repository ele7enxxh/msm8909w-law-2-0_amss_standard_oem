/******************************************************************************
  @file    client.c
  @brief   Android performance iop library

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_TAG         "ANDR-IOP"
#include <cutils/log.h>

#include "client.h"
#include "io-p.h"

#include "io-prefetch/list_capture.h"

#if QC_DEBUG
#  define QLOGE(...)    ALOGE(__VA_ARGS__)
#  define QLOGW(...)    ALOGW(__VA_ARGS__)
#  define QLOGI(...)    ALOGI(__VA_ARGS__)
#  define QLOGV(...)    ALOGV(__VA_ARGS__)
#else
#  define QLOGE(...)
#  define QLOGW(...)
#  define QLOGI(...)
#  define QLOGV(...)
#endif

int perf_io_prefetch_start(int pid, const char *pkg_name)
{
    int rc = -1;
    rc = iop_send(IOP_CMD_PERFLOCK_IOPREFETCH_START, pid, pkg_name);
    return rc;
}


int perf_io_prefetch_stop()
{
    int rc = -1;
    rc = iop_send(IOP_CMD_PERFLOCK_IOPREFETCH_STOP);
    return rc;
}

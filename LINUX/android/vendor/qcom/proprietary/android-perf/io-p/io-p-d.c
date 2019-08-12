/******************************************************************************
 *
 *  IO-P-D.C
 *
 * GENERAL DESCRIPTION
 *  IOP Control Daemon

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
 ******************************************************************************/

extern int  iop_server_init(void);
extern void iop_server_exit(void);

#define LOG_TAG           "ANDR-PERF-IOP-D"
#include <cutils/log.h>

#if QC_DEBUG
#define QLOGE(...)    ALOGE(__VA_ARGS__)
#define QLOGW(...)    ALOGW(__VA_ARGS__)
#define QLOGI(...)    ALOGI(__VA_ARGS__)
#define QLOGV(...)    ALOGV(__VA_ARGS__)
#define printf(...)   ALOGE(__VA_ARGS__)
#else
#define QLOGE(...)
#define QLOGW(...)
#define QLOGI(...)
#define QLOGV(...)
#define printf(...)
#endif

/* Main entry to io-p-d
   * argc - number of command-line arguments
   * argv - command-line argument array
   Return value - 0 if successful, negative otherwise */
int main(int argc, char *argv[])
{
    iop_server_init();

    iop_server_exit();
    return 0;
}

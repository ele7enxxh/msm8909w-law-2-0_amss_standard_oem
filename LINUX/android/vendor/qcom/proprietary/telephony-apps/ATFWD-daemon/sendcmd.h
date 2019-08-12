/*!
  @file
  sendcmd.h

  @brief
	Contains functions required to send commands to android through the
	AtCmdFwd Service
*/

/*===========================================================================

Copyright (c) 2015, Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who      what, where, why
--------   ---      ---------------------------------------------------------
04/11/11   jaimel    First cut.


===========================================================================*/

#ifndef __ATFWDDAEMON_H
#define __ATFWDDAEMON_H

#include "AtCmdFwd.h"

#define ATFWD_RETRY_DELAY                 5  /* Retry delay in sec */
#define ATFWD_MAX_RETRY_ATTEMPTS          5

#ifdef __cplusplus
extern "C" {
#endif

int initializeAtFwdService();
AtCmdResponse *sendit(const AtCmd *cmd);
void millisecondSleep(int milliseconds);

#ifdef __cplusplus
}
#endif

#endif /* __ATFWDDAEMON_H */

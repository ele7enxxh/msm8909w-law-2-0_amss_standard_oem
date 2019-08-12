/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#include "drStd.h"

#include "drCommon.h"
#include "drUtils.h"

/**
 * Exchange registers of current thread or another thread.
 *
 * @retval E_OK or relevant error code.
 */
drApiResult_t drUtilsRestartThread(
    threadno_t threadno,
    addr_t ip,
    addr_t sp)
{
    drApiResult_t ret  = E_INVALID;
    uint32_t      ctrl = THREAD_EX_REGS_IP | THREAD_EX_REGS_SP;

    ret= drApiThreadExRegs(threadno, ctrl, ip, sp);

    if (ret == E_OK)
    {
        ret = drApiResumeThread(threadno);
    }

    return ret;
}

/**
 * Makes request to RTM to update notification thread
 *
 * @retval E_OK or relevant error code.
 */
drApiResult_t drUtilsUpdateNotificationThread( threadno_t threadno )
{
    drApiResult_t ret =  E_INVALID;
    threadid_t  notificationThread;

    /* Retrieve task id */
    taskid_t task = drApiGetTaskid();
    if (task == NILTASK)
    {
        return ret;
    }

    /* Retrieve thread id based on task id and DCI handler thread no */
    notificationThread = drApiTaskidGetThreadid(task, threadno);

    /* IPC data for setting notification handler */
    threadid_t  ipcClient = notificationThread;
    message_t   ipcMsg    = MSG_SET_NOTIFICATION_HANDLER;
    uint32_t    ipcData   = 0;

    /* Make IPC call to IPCH and wait for a response */
    ret = drApiIpcCallToIPCH(&ipcClient, &ipcMsg, &ipcData);

    return ret;
}

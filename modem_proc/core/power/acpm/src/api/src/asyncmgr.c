/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * asyncmgr.c
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */
#include "adsppm.h"
#include "ULog.h"
#include "ULogFront.h"
#include "adsppm_utils.h"
#include "core.h"
#include <qurt_pipe.h>
#include <qurt_error.h>
#include "asyncmgr.h"
#include "rcinit.h"
#include "api.h"


uint32 API_doClientCallback(AsyncMgrPipeDataType* receiveData);

qurt_pipe_data_t reqPipeBuffer[MAX_ADSPPM_REQ_Q_ELEMENTS];
qurt_pipe_t  amReqQ;
qurt_pipe_data_t eventPipeBuffer[MAX_ADSPPM_REQ_Q_ELEMENTS];
qurt_pipe_t  amEventQ;

Adsppm_Status AM_AsyncInit(void)
{
    AdsppmStatusType sts = Adsppm_Status_Success;
    // create pipe for asyncmgr req
    qurt_pipe_attr_t attr1, attr2;
    ADSPPM_LOG_FUNC_ENTER;
    memset(reqPipeBuffer, 0, sizeof(qurt_pipe_data_t)*MAX_ADSPPM_REQ_Q_ELEMENTS);
    memset(eventPipeBuffer, 0, sizeof(qurt_pipe_data_t)*MAX_ADSPPM_REQ_Q_ELEMENTS);

    //init pipes
    qurt_pipe_attr_init(&attr1);
    qurt_pipe_attr_set_buffer(&attr1, reqPipeBuffer);
    qurt_pipe_attr_set_elements(&attr1, MAX_ADSPPM_REQ_Q_ELEMENTS);
    if(QURT_EOK != qurt_pipe_init(&(amReqQ), &attr1))
    {
        sts = Adsppm_Status_Failed;
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "req pipe init failed");
    }
    //create pipe for asyncmgr complete event
    qurt_pipe_attr_init(&attr2);
    qurt_pipe_attr_set_buffer(&attr2, eventPipeBuffer);
    qurt_pipe_attr_set_elements(&attr2, MAX_ADSPPM_REQ_Q_ELEMENTS);
    if(QURT_EOK != qurt_pipe_init(&(amEventQ), &attr2))
    {
        sts = Adsppm_Status_Failed;
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "event pipe init failed");
    }

    ADSPPM_LOG_FUNC_EXIT(sts);

    return sts;

}
/**
 * @fn AM_AsyncRequest - schedules request for asynchronous execution.
 *
 * @param clientId - client ID obtained during registration with ADSPPM
 * @param rscMap - resource map - a bitmap encoding all the resources requested as a part of this bundle.
 *                 Each bit is 1<<resourceID
 * @param reqTag - user defined identificator of the request.
 *
 * @return completion status
 */
Adsppm_Status AM_AsyncRequest(uint32 clientId, uint32 rscMap, uint32 reqTag)
{
    AdsppmStatusType sts = Adsppm_Status_Success;
    AsyncMgrMessageType AMMessage;
    ADSPPM_LOG_FUNC_ENTER;

    memset(&AMMessage, 0, sizeof(AsyncMgrMessageType));

    //setup pipe data
    AMMessage.asyncPipeData.reqQMask = rscMap;
    AMMessage.asyncPipeData.clientId = clientId;
    AMMessage.asyncPipeData.reqTag = reqTag;
    AMMessage.asyncPipeData.result = Adsppm_Status_Success;
    qurt_pipe_send(&(amReqQ), AMMessage.pipeData);

    ADSPPM_LOG_FUNC_EXIT(sts);
    return sts;
}   

void AM_WorkLoopThread(void)
{
    AdsppmStatusType sts = Adsppm_Status_Success;
    AsyncMgrMessageType receiveData;
    AsyncMgrMessageType returnData;
    uint32 index = 0;


    ADSPPM_LOG_FUNC_ENTER;

    /* Start the task */
    rcinit_handshake_startup();
    if(ADSPPM_IsInitialized())
    {
        // Enter forever loop
        for(;;)
        {
            memset(&receiveData, 0, sizeof(qurt_pipe_data_t));
            memset(&returnData, 0, sizeof(qurt_pipe_data_t));

            receiveData.pipeData = qurt_pipe_receive(&(amReqQ));
            returnData = receiveData;

            for(index = Adsppm_Rsc_Id_Power; index < Adsppm_Rsc_Id_Max; index++)
            {
                if( receiveData.asyncPipeData.reqQMask & 0x1<<index)
                {
                    sts = RM_RequestResource(index);
                    if(Adsppm_Status_Success != sts)
                    {
                        returnData.asyncPipeData.result = sts;
                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "RM_RequestResource failed for resource %d \n", index);
                        break;
                    }
                }
            }
            qurt_pipe_send(&(amEventQ), returnData.pipeData);

        }
    }
    ADSPPM_LOG_FUNC_EXIT(0);
}

/**
 * Async API completion callback thread
 */
void AM_callBackThread(void)
{
    AsyncMgrMessageType receiveData;
    uint32 result = 0;
    ADSPPM_LOG_FUNC_ENTER;

    /* Start the task */
    rcinit_handshake_startup();

    if(ADSPPM_IsInitialized())
    {
        // Enter forever loop
        for(;;)
        {
            memset(&receiveData, 0, sizeof(qurt_pipe_data_t));

            receiveData.pipeData = qurt_pipe_receive(&(amEventQ));

            result = API_doClientCallback(&(receiveData.asyncPipeData));
        }
    }
    ADSPPM_LOG_FUNC_EXIT(0);
}



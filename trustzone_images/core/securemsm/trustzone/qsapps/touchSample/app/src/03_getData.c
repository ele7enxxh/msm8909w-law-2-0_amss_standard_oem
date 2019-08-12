/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include "sselog.h"
#include <stdint.h>
#include "touchSampleApi.h"
#include "TouchApi.h"

/** GetData
 *
 * @return SSE_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdGetData(
  touchSampleCommandPayload_t* command,
  touchSampleResponsePayload_t* response
)
{
  ENTERRV;
  fingerData_t *data = NULL;
  uint32_t result = SSE_OK;
  
  do {
    result = touchGetData(command->getData.sid,
                          (struct tsFingerData *)&response->getData.data);
    if (!IS_OK(result)) {
      LOG_E("Error getting data from driver",result);
      rv = E_SSE_UNKNOWN;
      break;
    }
    data = &response->getData.data;
    for (uint8_t i = 0; i < MAX_FINGER_NUM; i++) {
      if (data->finger[i].event) {
        LOG_I("[%u]: %u, (%u, %u)",i, 
          data->finger[i].event, 
          data->finger[i].x, 
          data->finger[i].y);
      }
    }
    //DUMPHEX("Data returned from driver",&response->getData.data, sizeof(response->getData.data));
  } while (0);
  EXITRV;
}

/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * ahbmgr.h
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */

#ifndef AHBMGR_H_
#define AHBMGR_H_

#include "adsppm.h"
#include "busmgr.h"

/**
*@struct ahb management context
*/

typedef struct
{
   AdsppmBusBWDataIbAbType    curAHBBwData; //!< Current aggregated AHB BW
   AdsppmBusBWDataIbAbType    nonSuppressibleAhbBW; //!< Non-suppressible AHB BW
   uint32    preAHBClock; //!< Clock currently set
   uint32    activeAHBClock; //!< Clock calculated based on aggregated clock accounting for execution state (periodic vs non-periodic)
   uint32    aggregatedAHBClock; //!< Clock calculated based on BW requirements only for all clients
   uint32    nonSuppressibleAHBClock; //!< Clock calculated based on BW requirements only for non-suppressible clients

   DALSYSSyncHandle       AHBMgrCtxLock;
}AHBMCtxType;

Adsppm_Status AHBM_Init(void);

Adsppm_Status AHBM_ProcessRequest(int numOfBw, busMgrInterBwReqType *pBusBwReq);

extern inline Adsppm_Status AHBM_SetActiveAHBClock(void);

extern inline Adsppm_Status AHBM_SetSleepAHBClock(void);

#endif /* BUSMGR_H_ */

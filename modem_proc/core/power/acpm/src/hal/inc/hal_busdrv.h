/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * hal_busdrv.h
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */

#ifndef HAL_BUSDRV_H_
#define HAL_BUSDRV_H_
#include "adsppm.h"

//! Definition of the number of MIPS routes exposed in Bus_IssueBWRequest_MIPS
#define BUS_NUMBER_MIPS_ROUTES 1

#define ICBARB_NODE_NAME "/icb/arbiter"

/******************************************************************************/
/*              Functions definitions                                         */
/******************************************************************************/
AdsppmStatusType Bus_Init(void);

AdsppmStatusType Bus_ReleaseBWRequest_MIPS(void);

AdsppmStatusType Bus_IssueBWRequest_MIPS(AdsppmBusBWRequestValueType *pRequest);

AdsppmStatusType Bus_IssueBWRequest( int num, AdsppmBusBWRequestValueType *pRequest);

AdsppmStatusType Bus_ReleaseBWRequest( int num, AdsppmBusBWRequestValueType *pRequest);

/**
 * Get last requested BW value for a specific route
 * @param pRoute - external BW route
 * @return BW value
 */
AdsppmBusBWDataIbAbType Bus_GetRequestedBw(AdsppmBusRouteType *pRoute);

#endif /* HAL_BUSDRV_H_ */

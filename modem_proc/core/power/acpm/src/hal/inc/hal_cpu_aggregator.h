/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * hal_sleep.h
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */

#ifndef HAL_AGGREGATOR_H_
#define HAL_AGGREGATOR_H_

#include "adsppm.h"

typedef struct{
	uint32 mpps;
	uint32 floorClock;
	uint32 cpp;
}CpuAggregatorRequestDataType;


AdsppmStatusType CA_Init(void);

AdsppmStatusType CA_RequestCpuPerformanceLevel(AdsppmMipsAggregateDataType* pCpuData);

#endif /* HAL_AGGREGATOR_H_ */

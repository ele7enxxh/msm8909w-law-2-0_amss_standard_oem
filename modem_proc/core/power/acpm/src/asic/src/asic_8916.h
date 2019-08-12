/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * asic_8916.h - contains ASIC configuration definitions required for 8916
 *
 *  Created on: Apr 27, 2012
 *      Author: yrusakov
 */

#ifndef ASIC_8916_H_
#define ASIC_8916_H_
#include "adsppm.h"
#include "asic_internal.h"

Adsppm_Status aggregateMips_8916(AdsppmMipsAggregateDataType *pMipsAggregateData, AdsppmMipsRequestType *pMipsReqData, AdsppmBusBWDataUsageType* pMipsBW);

Adsppm_Status setDspPerformanceLevel_8916(AdsppmMipsAggregateDataType* pMipsAggregateData);

Adsppm_Status getClockFromBW_8916(uint32 *pClock, AdsppmBusBWDataIbAbType *pAHBBwData);

Adsppm_Status aggregateBusBW_8916(AdsppmBusBWDataIbAbType *pAggregateBwIbAbValue, AdsppmBusBWDataType *pBwValue);

Adsppm_Status getCppFromMips_8916(AdsppmMipsAggregateDataType *pMipsAggregateData);


#endif /* ASIC_8916_H_ */

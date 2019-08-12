/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/**
 *  @file: asic_internal.h contains internal Asic Config Manager's definitions
 *
 *  Created on: Mar 23, 2012
 *  @author: yrusakov
 */

#ifndef ASIC_INTERNAL_H_
#define ASIC_INTERNAL_H_

#include "asic.h"


#define PERCENTS_IN_A_WHOLE  256

#define ASIC_CONFIG_DEVICE_NAME "/core/power/adsppm"

typedef struct{
    int numElements;
    AsicClkDescriptorType *pClocks;
} AsicClockDescriptorArrayType;

typedef struct{
    int numElements;
    AsicBusPortDescriptorType *pPorts;
} AsicBusPortDescriptorArrayType;

typedef struct{
    int numElements;
    AsicPowerDomainDescriptorType *pPwrDomains;
} AsicPwrDescriptorArrayType;

typedef struct{
    int numElements;
    AsicCoreDescType *pCores;
} AsicCoreDescriptorArrayType;

typedef struct{
    int numElements;
    AsicMemDescriptorFullType pMemories[Adsppm_Mem_Max];
} AsicMemDescriptorFullArrayType;


typedef struct{
    const int numElements;
    const AsicMemDescType *pPwrDomainDesc;
} AsicMemDescriptorArrayType;

typedef struct{
    int numElements;
    AdsppmBusRouteType *pRoutes;
} AsicBusRouteArrayType;

typedef struct{
	uint32 mpps; //!< MPPS level limiting the CPP
	uint32 cpp; //!< CPP*1000 value for MPPS level below the one specified by mpps field
}AsicCpuAggregatorMppsCppType;

typedef struct{
	uint32 numberElements; //!< Number of elements in CPP array
	AsicCpuAggregatorMppsCppType *pData; //!< Pointer to CPP array
}AsicCpuAggregatorMppsCppArrayType;

typedef struct{
	uint32 footprintFactor; //!< footprint indicator (BW/MPPS)level limiting the CPP adjustment
	uint32 cppFudgeFactor; //!< CPP*1000 value for MPPS level below the one specified by mpps field
}AsicCpuAggregatorCppAdjustmentType;

typedef struct{
	uint32 numberElements; //!< Number of elements in CPP array
	AsicCpuAggregatorCppAdjustmentType *pData; //!< Pointer to CPP array
}AsicCpuAggregatorCppAdjustmentArrayType;

/**
 *  Enumeration to indicate function set used for this chip
 */
typedef enum{
    AsicFnSetId_Default = 0//!< AsicFnID_Default
} AsicFnSetIdType;

typedef struct{
    AsicFnSetIdType functionSet;
    AsicCpuAggregatorTypeType cpuAggregatorType;
    uint32  debugLevel;
    uint32  adspHwThreadNumber;
    uint32  adspCacheSize; //!< L2 size in KB
    AsicHwioAddrRangeType*  lpassBaseAddr; //!< Base address for LPASS CSR
    AsicCoreDescriptorArrayType* cores;
    AsicMemDescriptorFullArrayType memories;
    AsicClockDescriptorArrayType* clocks;
    AsicBusPortDescriptorArrayType* busPorts;
    AsicBusRouteArrayType* extBusRoutes;
    AsicBusRouteArrayType* mipsBusRoutes;
    AsicPwrDescriptorArrayType* pwrDomains;
    AsicCpuAggregatorMppsCppArrayType* cppFactors; //!< Array of CPP factors per MPPS level
    AsicCpuAggregatorCppAdjustmentArrayType* cppFudgeFactors; //!< Array of CPP adjustment factors based on memory footprint factor
    Adsppm_Status (*pFn_AggregateMips)(AdsppmMipsAggregateDataType *pMipsAggregateData, AdsppmMipsRequestType *pMipsReqData, AdsppmBusBWDataUsageType* pMipsBW);
    Adsppm_Status (*pFn_SetDspPerformanceLevel)(AdsppmMipsAggregateDataType *pMipsAggregateData);
    Adsppm_Status (*pFn_GetClockFromBW)(uint32 *pClocks, AdsppmBusBWDataIbAbType *pAHBBwData);
    Adsppm_Status (*pFn_BusBWAggregate)(AdsppmBusBWDataIbAbType *pAggregateBwIbAbValue, AdsppmBusBWDataType *pBwValue);
    Adsppm_Status (*pFn_GetCppFromMips)(AdsppmMipsAggregateDataType *pMipsAggregateData);
    AsicFeatureDescType features[AsicFeatureId_enum_max]; //!< Feature enablement status
} AsicConfigType;


typedef struct{
    uint32 numClients;
    uint32 factor;
} AsicFactorType;


/**
 * @fn asicGetAsicConfig - fill in ASIC configuration from Device Config image
 * @param pConfig - [OUT] pointer to ASIC configuration structure
 * @return completion status
 */
Adsppm_Status asicGetAsicConfig(AsicConfigType *pConfig);

#endif /* ASIC_INTERNAL_H_ */

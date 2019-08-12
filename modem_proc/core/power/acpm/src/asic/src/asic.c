/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * asic.c - File contains definitions of all generic ASIC Config Manager's functions
 *
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */

#include "adsppm.h"
#include "ULog.h"
#include "ULogFront.h"
#include "adsppm_utils.h"
#include "asic.h"
#include "asic_internal.h"
#include "ULogFront.h"
#include "DALSys.h"
#include "DDIChipInfo.h"
#include "DALDeviceId.h"
#include "asic_8916.h"
#include "qurt_memory.h"


static AsicConfigType g_AsicConfig; //!< global structure holding ASIC configuration

#define ASIC_SCLK_FREQ_KHZ 32
#define ASIC_QCLK_FREQ_KHZ 19200

/** 
*  this global variable is used for change debug level on
*  runtime to get adsppm ulog default ulog level is 3. #define
*  ADSPPM_LOG_LEVEL_PROFILE 0   profiling log message, set gDEBUGLEVEL = ADSPPM_LOG_LEVEL_MAX to get this debug level
* #define ADSPPM_LOG_LEVEL_ERROR 1      Error log messages
* #define ADSPPM_LOG_LEVEL_WARNING 2    Warnings 
* #define ADSPPM_LOG_LEVEL_INFO 3       Critical info messages 
* #define ADSPPM_LOG_LEVEL_INFO_EXT 4   Extended info messages 
* #define ADSPPM_LOG_LEVEL_FN_ENTRY 5   Function entry/exit messages 
* #define ADSPPM_LOG_LEVEL_DEBUG 6      All debug messages 
* #define ADSPPM_LOG_LEVEL_MAX 7 
*/ 

static volatile uint32 gDEBUGLEVEL = 0;

/**
 * this global variable is used for enable/disable ADSPPM power 
 * feature before MMPM_Init 
 *  gADSPPMPOWERFEATURE = 0xff  get feature definition from
 *  config file
 *  gADSPPMPOWERFEATURE = 0, disable all adsppm power feature
 *  gADSPPMPOWERFEATURE = 1, enable all adsppm power feature
 *  except some features not supported.
 * 
 * @author alisonc (3/1/2013)
 */
static volatile uint32 gADSPPMPOWERFEATURE = 0xff;


Adsppm_Status asicInitConfigData(AsicConfigType *pConfig)
{
    Adsppm_Status result = Adsppm_Status_Success;

    //Get Chip configuration from Device Config image
    result = asicGetAsicConfig(pConfig);

    //Fill in ASIC specific functions
    // Only default functions are used at the moment
    switch (pConfig->functionSet)
    {
    case AsicFnSetId_Default:
        pConfig->pFn_AggregateMips = aggregateMips_8916;
        pConfig->pFn_SetDspPerformanceLevel = setDspPerformanceLevel_8916;
        pConfig->pFn_GetCppFromMips = getCppFromMips_8916;
        pConfig->pFn_GetClockFromBW = getClockFromBW_8916;
        pConfig->pFn_BusBWAggregate = aggregateBusBW_8916;
        break;
    default:
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "ERROR: Unsupported function set ID %d", pConfig->functionSet);
        result = Adsppm_Status_Failed;
        break;
    }
    return result;
}

/**
 * @fn ACM_Init - initializes global ASIC configuration structure
 *
 * Function gets ASIC information (Family, ID, version) from ChipInfo.
 * The ASIC information is then used to populate the global configuration with
 * ASIC-specific data.
 *
 * @return completion status
 */
Adsppm_Status ACM_Init(){
    Adsppm_Status result = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_ENTER;
    memset(&g_AsicConfig, 0, sizeof(g_AsicConfig));

    // Populate ASIC config with chip-specific data
    // All properties will be versioned in the device config XML,
    // so no need to know about chip family and revision
    result = asicInitConfigData(&g_AsicConfig);

    ADSPPM_LOG_FUNC_EXIT(result);
    return result;
}

AsicHwRscIdType ACM_GetHwRscId(AdsppmCoreIdType core, AdsppmRscIdType rsc)
{
    AsicHwRscIdType resType = AsicRsc_None;

    if ((core > Adsppm_Core_Id_None) && (core < g_AsicConfig.cores->numElements) &&
            (rsc > Adsppm_Rsc_Id_None) && (rsc < Adsppm_Rsc_Id_Max))
    {
        resType = g_AsicConfig.cores->pCores[core].hwResourceId[rsc];
    }
    return resType;
}

/* TODO switch to using this instead of direct function calls
Asic_Rsc_AggregationFnType ACM_GetAggregationFn(AsicHwRscIdType, AdsppmCoreIdType)
{
    return NULL;
}
 */

/**
 * Request number of supported routes (master/slave pairs) for external BW requests
 * @return number of supported routes (master/slave pairs) for external BW requests
 */
__inline int ACMBus_GetNumberOfExtRoutes(void)
{
    return g_AsicConfig.extBusRoutes->numElements;
}

/**
 * Request supported routes (master/slave pairs) for external BW requests
 * @param pExtRoutes - [OUT] array to be filled with supported routes (master/slave pairs) for external BW requests
 */
void ACMBus_GetExtRoutes(AsicBusExtRouteType* pExtRoutes)
{
    int i = 0;
    AsicBusPortDescriptorType *pMasterBusPortDescriptor = NULL;
	AsicBusPortDescriptorType *pSlaveBusPortDescriptor = NULL;
    if(NULL != pExtRoutes)
    {
        for(i=0; i < g_AsicConfig.extBusRoutes->numElements; i++)
        {
            pExtRoutes[i].masterPort.adsppmMaster = g_AsicConfig.extBusRoutes->pRoutes[i].masterPort;
			pMasterBusPortDescriptor = ACMBus_GetPortDescriptor(pExtRoutes[i].masterPort.adsppmMaster);
            pExtRoutes[i].slavePort.adsppmSlave = g_AsicConfig.extBusRoutes->pRoutes[i].slavePort;
			pSlaveBusPortDescriptor = ACMBus_GetPortDescriptor(pExtRoutes[i].slavePort.adsppmSlave);
			if(( NULL != pMasterBusPortDescriptor) && (NULL != pSlaveBusPortDescriptor))
			{
				pExtRoutes[i].masterPort.icbarbMaster = pMasterBusPortDescriptor->icbarbId.icbarbMaster;
				pExtRoutes[i].slavePort.icbarbSlave = pSlaveBusPortDescriptor->icbarbId.icbarbSlave;
			}
        }
    }
}

/**
 * Request number of supported routes (master/slave pairs) for MIPS BW requests
 * @return number of supported routes (master/slave pairs) for MIPS BW requests
 */
__inline int ACMBus_GetNumberOfMipsBwRoutes(void)
{
    return g_AsicConfig.mipsBusRoutes->numElements;
}

/**
 * Request list supported routes (master/slave pairs) for MIPS BW requests
 * @return pointer to array of supported routes (master/slave pairs) for MIPS BW requests
 */
__inline AdsppmBusRouteType* ACMBus_GetMipsBwRoutes(void)
{
    return g_AsicConfig.mipsBusRoutes->pRoutes;
}

/**
 * Returns pointer to the the descriptor for the specified bus port.
 * @param port
 * @return pointer to the bus port descriptor
 */
__inline AsicBusPortDescriptorType* ACMBus_GetPortDescriptor(AdsppmBusPortIdType port)
{

    AsicBusPortDescriptorType* pDesc = NULL;
    if ((port >= AdsppmBusPort_None) && (port < (AdsppmBusPortIdType)g_AsicConfig.busPorts->numElements))
    {
        pDesc = &(g_AsicConfig.busPorts->pPorts[port]);
    }
    return pDesc;

}

/**
 * Get type of the clock
 * @param Clock Id
 * @return Type of the clock (enable/disable, frequency set, npa)
 */
__inline AsicClkTypeType ACMClk_GetClockType(AdsppmClkIdType clk)
{
    AsicClkTypeType clkType = AsicClk_TypeDalEnable;
    if ((clk >= AdsppmClk_None) && (clk < (AdsppmClkIdType)g_AsicConfig.clocks->numElements))
    {
        clkType = g_AsicConfig.clocks->pClocks[clk].clkType;
    }
    return clkType;
}

/**
 * Get descriptor the clock
 * @param Clock Id
 * @return Type of the clock (enable/disable, frequency set, npa)
 */
__inline AsicClkDescriptorType* ACMClk_GetClockDescriptor(AdsppmClkIdType clk)
{
    AsicClkDescriptorType* pDesc = NULL;
    if ((clk >= AdsppmClk_None) && (clk < (AdsppmClkIdType)g_AsicConfig.clocks->numElements))
    {
        pDesc = &(g_AsicConfig.clocks->pClocks[clk]);
    }
    return pDesc;
}

/**
 * Get info about the power domain
 * @param Domain Id
 * @return Power Domain Info
 */
__inline AsicPowerDomainDescriptorType* ACMClk_GetPwrDomainDescriptor(AsicPowerDomainType domain)
{
    AsicPowerDomainDescriptorType* pDesc = NULL;
    if ((domain >= AsicPowerDomain_AON) && (domain < (AsicPowerDomainType)g_AsicConfig.pwrDomains->numElements))
    {
        pDesc = &(g_AsicConfig.pwrDomains->pPwrDomains[domain]);
    }
    return pDesc;
}

/**
 * Get power domain for the specified core
 * @param Core ID
 * @return Power Domain
 */
__inline AsicPowerDomainType ACMPwr_GetPowerDomain(AdsppmCoreIdType core)
{
    AsicPowerDomainType domain = AsicPowerDomain_AON;
    if ((core > Adsppm_Core_Id_None) && (core < (AdsppmCoreIdType)g_AsicConfig.cores->numElements))
    {
        domain = g_AsicConfig.cores->pCores[core].pwrDomain;
    }
    return domain;
}

/**
 * Get info about the memory
 * @param Memory Id
 * @return Memory Info
 */
AsicMemDescriptorFullType *ACM_GetMemoryDescriptor(AdsppmMemIdType mem)
{
    AsicMemDescriptorFullType *pDesc = NULL;
    if((mem > Adsppm_Mem_None) && (mem < g_AsicConfig.memories.numElements))
    {
        pDesc = &(g_AsicConfig.memories.pMemories[mem]);
    }
    return pDesc;
}


/**
 * Get address range for the specified memory
 * @param Memory ID
 * @return address range
 */
AsicAddressRangeType *ACM_GetVirtMemAddressRange(AdsppmMemIdType mem)
{
    AsicAddressRangeType *addrRange = NULL;
    if((mem > Adsppm_Mem_None) && (mem < g_AsicConfig.memories.numElements))
    {
        addrRange = &g_AsicConfig.memories.pMemories[mem].virtAddr;
    }
    return addrRange;
}


/**
 * Set address range for the specified memory
 * @param mem - Memory ID
 * @param addr - start address
 * @param size - address range size
 * @return operation status
 */
AdsppmStatusType ACM_SetVirtMemAddressRange(AdsppmMemIdType mem, uint64 addr, uint32 size)
{
    AdsppmStatusType result = Adsppm_Status_BadParm;
    if((mem > Adsppm_Mem_None) && (mem < g_AsicConfig.memories.numElements))
    {
        g_AsicConfig.memories.pMemories[mem].virtAddr.startAddr = addr;
        g_AsicConfig.memories.pMemories[mem].virtAddr.size = size;
        result = Adsppm_Status_Success;
    }
    return result;
}

/** 
 * Get power domain for the specified memory
 * @param Memory ID
 * @return Power Domain
 */
__inline AsicPowerDomainType ACMPwr_GetMemPowerDomain(AdsppmMemIdType mem)
{
    AsicPowerDomainType domain = AsicPowerDomain_AON;
    if((mem > Adsppm_Mem_None) && (mem < g_AsicConfig.memories.numElements))
    {
        domain = g_AsicConfig.memories.pMemories[mem].pDescriptor->pwrDomain;
    }
    return domain;
}

/**
 * Get power resource type  for the specified memory
 * @param Memory ID
 * @return Power Domain
 */
__inline AsicHwRscIdType ACMPwr_GetMemPowerType(AdsppmMemIdType mem)
{
    AsicHwRscIdType rsc = AsicRsc_None;
    if((mem > Adsppm_Mem_None) && (mem < g_AsicConfig.memories.numElements))
    {
        const AsicPowerDomainDescriptorType *pPwrDesc = ACMClk_GetPwrDomainDescriptor(g_AsicConfig.memories.pMemories[mem].pDescriptor->pwrDomain);
        if (NULL != pPwrDesc)
        {
            rsc = pPwrDesc->pwrDomainType;
        }
    }
    return rsc;
}

/**
 * Get descriptor for the specified core
 * @param Core ID
 * @return pointer to the descriptor for this core
 */
__inline AsicCoreDescType* ACM_GetCoreDescriptor(AdsppmCoreIdType core)
{
    AsicCoreDescType* pDesc = NULL;
    if ((core > Adsppm_Core_Id_None) && (core < (AdsppmCoreIdType)g_AsicConfig.cores->numElements))
    {
        pDesc = &(g_AsicConfig.cores->pCores[core]);
    }
    return pDesc;
}

__inline Adsppm_Status ACM_AggregateMips(AdsppmMipsAggregateDataType *pMipsAggregateData, AdsppmMipsRequestType *pMipsReqData, AdsppmBusBWDataUsageType* pMipsBW)
{
	Adsppm_Status result = Adsppm_Status_Failed;
	if(NULL != g_AsicConfig.pFn_AggregateMips)
	{
		result = g_AsicConfig.pFn_AggregateMips(pMipsAggregateData, pMipsReqData, pMipsBW);
	}
	return result;
}

__inline Adsppm_Status ACM_GetCppFromMips(AdsppmMipsAggregateDataType *pMipsAggregateData)
{
    Adsppm_Status result = Adsppm_Status_Failed;
    if(NULL != g_AsicConfig.pFn_GetCppFromMips)
    {
        result = g_AsicConfig.pFn_GetCppFromMips(pMipsAggregateData);
    }
    return result;
}

__inline Adsppm_Status ACM_SetDspPerformanceLevel(AdsppmMipsAggregateDataType* pMipsAggregateData)
{
    Adsppm_Status result = Adsppm_Status_Failed;
    if(NULL != g_AsicConfig.pFn_SetDspPerformanceLevel)
    {
        result = g_AsicConfig.pFn_SetDspPerformanceLevel(pMipsAggregateData);
    }
    return result;
}

__inline Adsppm_Status ACM_GetClockFromBW(uint32 *pClock, AdsppmBusBWDataIbAbType *pAHBBwData)
{
    Adsppm_Status result = Adsppm_Status_Failed;
    if(NULL != g_AsicConfig.pFn_GetClockFromBW)
    {
        result = g_AsicConfig.pFn_GetClockFromBW(pClock, pAHBBwData);
    }
    return result;
}

/**
 * @fn ACM_BusBWAggregate - Aggregate bw. So far the function is common for all chips
 * @param input: AdsppmBusBWDataType *pBwValue
 * @param output: AdsppmBusBWDataIbAbType *pAggregateBwIbAbValue
 * @return Adsppm_Status
 */

Adsppm_Status ACM_BusBWAggregate(AdsppmBusBWDataIbAbType *pAggregateBwIbAbValue, AdsppmBusBWDataType *pBwValue)
{
    Adsppm_Status result = Adsppm_Status_Failed;
    if(NULL != g_AsicConfig.pFn_BusBWAggregate)
    {
        result = g_AsicConfig.pFn_BusBWAggregate(pAggregateBwIbAbValue, pBwValue);
    }
    return result;
}


/**
 * Get feature enablement status for a particular feature
 */
__inline AsicFeatureStateType ACM_GetFeatureStatus(AsicFeatureIdType featureId)
{
    return g_AsicConfig.features[featureId].state;
}

/**
 * Get feature descriptor for a particular feature
 */
__inline AsicFeatureDescType* ACM_GetFeatureDescriptor(AsicFeatureIdType featureId)
{
    return &g_AsicConfig.features[featureId];
}

/**
 * Adjust parameter value based on feature enablement and configuration data
 *
 * @param featureId - feature ID.
 * @param value - value, which needs to be adjusted
 *
 * @return - adjusted value
 */
uint64 ACM_AdjustParamValue (AsicFeatureIdType featureId, uint64 value)
{
    AsicFeatureDescType *pDesc = ACM_GetFeatureDescriptor(featureId);
    uint64 result = value;

    switch(pDesc->state)
    {
    case AsicFeatureState_Enabled:
    case AsicFeatureState_Limited:
        //apply min/max limitation
        // Limit from the top
        result = MIN(value, pDesc->max);
        // Limit from bottom
        result = MAX(result, pDesc->min);
        break;
    case AsicFeatureState_Disabled:
        // use max for the parameter
        result =  pDesc->max;
        break;
    default:

        break;
    }

    return result;
}

__inline uint32 ACM_GetDebugLevel(void)
{
    return g_AsicConfig.debugLevel;
}

/**
 * Convert time in ms to sclk
 *
 * @param ms - time in ms.
 *
 * @return - number of sclks
 */
__inline uint64 ACM_ms_to_sclk (uint64 ms)
{
    return ms * ASIC_SCLK_FREQ_KHZ;
}

/**
 * Convert time in us to qclk (QTimer clock ticks)
 *
 * @param us - time in us.
  *
 * @return - number of qclks
 */
__inline uint64 ACM_us_to_qclk (uint64 us)
{
    return (us * ASIC_QCLK_FREQ_KHZ)/1000;
}

__inline uint32 ACM_GetHwThreadNumber(void)
{
    return g_AsicConfig.adspHwThreadNumber;
}


__inline AsicCpuAggregatorTypeType ACM_GetCpuAggregatorType(void)
{
	return g_AsicConfig.cpuAggregatorType;
}

/**
 * Get CPU cache size
* @return CPU cache size in KB
*/
__inline uint32 ACM_GetCacheSize(void)
{
   return g_AsicConfig.adspCacheSize;
}

/**
 * Get LPASS HWIO base address
* @return HWIO base address
*/
__inline AsicHwioAddrRangeType* ACM_GetLpassBaseAddr(void)
{
   return g_AsicConfig.lpassBaseAddr;
}

/**
 * Get CPP factor based on MPPS
 * Function assumes that CPP array is sorted by MPPS
 * @return CPP*1000
 */
__inline uint32 ACM_GetCppforMpps(uint32 mpps)
{
	uint32 i=0, cpp = 1;
	if(NULL != g_AsicConfig.cppFactors)
	{
		for (i=0; i < g_AsicConfig.cppFactors->numberElements; i++)
		{
			if (mpps < g_AsicConfig.cppFactors->pData[i].mpps)
			{
				cpp = g_AsicConfig.cppFactors->pData[i].cpp;
				break;
			}
		}
	}
	return cpp;
}

/**
 * Get CPP fudge factor based on footprint indicator
 * Function assumes that CPP array is sorted by footprint value
 * @return CPP_fudge factor*1000
 */
__inline uint32 ACM_GetCppFudgeFactor(uint32 footprint)
{
	uint32 i=0, cpp = 1000;
	if(NULL != g_AsicConfig.cppFudgeFactors)
	{
		for (i=0; i < g_AsicConfig.cppFudgeFactors->numberElements; i++)
		{
			if (footprint < g_AsicConfig.cppFudgeFactors->pData[i].footprintFactor)
			{
				cpp = g_AsicConfig.cppFudgeFactors->pData[i].cppFudgeFactor;
				break;
			}
		}
	}
	return cpp;
}

static AsicHwioAddrRangeType defaultHwioBase = {0,0,0};

Adsppm_Status asicGetAsicConfig(AsicConfigType *pConfig)
{
    Adsppm_Status result = Adsppm_Status_Success;
    DALSYSPropertyVar system_property_var;
    DALResult dalresult = DAL_SUCCESS;
    AsicFeatureStateType featureState = AsicFeatureState_Disabled;
    AsicFeatureIdType featureIndex = AsicFeatureId_Adsp_Clock_Scaling;
    int i = 0;

    //Set default values
    pConfig->debugLevel = ADSPPM_LOG_LEVEL;
    pConfig->adspHwThreadNumber = ADSP_HW_THREAD_NUMBER_DEFAULT;
    pConfig->functionSet = AsicFnSetId_Default;
    pConfig->cpuAggregatorType = AsicCpuAggregatorType_Internal;
    pConfig->adspCacheSize = ADSP_CACHE_SIZE_DEFAULT;
    pConfig->lpassBaseAddr = &defaultHwioBase;

    /**
     * declare property handle variable.
     */
    DALSYS_PROPERTY_HANDLE_DECLARE(adsppm_property);
    /** Get the property handle */
    DALSYS_GetDALPropertyHandleStr(ASIC_CONFIG_DEVICE_NAME, adsppm_property);

    dalresult = DALSYS_GetPropertyValue(adsppm_property, "FUNCTION_SET", 0, &system_property_var);
    if(dalresult == DAL_SUCCESS)
    {
        // No checking for NULL since we are passing pointer to a static variable.
        pConfig->functionSet = system_property_var.Val.dwVal;
    }
    else
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "FUNCTION_SET is not specified in config, using default\n");
    }

    dalresult = DALSYS_GetPropertyValue(adsppm_property, "MCA_TYPE", 0, &system_property_var);
    if(dalresult == DAL_SUCCESS)
    {
    	// No checking for NULL since we are passing pointer to a static variable.
    	pConfig->cpuAggregatorType = system_property_var.Val.dwVal;
    }
    else
    {
    	ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "MCA_TYPE is not specified in config, using default\n");
    }

    dalresult = DALSYS_GetPropertyValue(adsppm_property, "DEBUG_LEVEL", 0, &system_property_var);
    if (!gDEBUGLEVEL)
    {
    	if(dalresult == DAL_SUCCESS)
    	{
    		// No checking for NULL since we are passing pointer to a static variable.
    		pConfig->debugLevel = system_property_var.Val.dwVal;
    	}
    	else
    	{
    		ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "DEBUG_LEVEL is not specified in config, using default\n");
    	}
    }
    else
    {
    	if (gDEBUGLEVEL < ADSPPM_LOG_LEVEL_MAX)
    	{
    		pConfig->debugLevel = gDEBUGLEVEL;
    		ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "DEBUG_LEVEL is set to %d \n", pConfig->debugLevel);
    	}
    	else if(ADSPPM_LOG_LEVEL_MAX == gDEBUGLEVEL)
    	{
    		pConfig->debugLevel = 0;
    		ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "DEBUG_LEVEL is set to %d, profiling\n", pConfig->debugLevel);
    	}
    	else
    	{
    		ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "gDEBUGLEVEL setting is wrong = %d \n", gDEBUGLEVEL);
    	}
    }
    dalresult = DALSYS_GetPropertyValue(adsppm_property, "THREAD_NUMBER", 0, &system_property_var);
    if(dalresult == DAL_SUCCESS)
    {
    	// No checking for NULL since we are passing pointer to a static variable.
    	pConfig->adspHwThreadNumber = system_property_var.Val.dwVal;
    }
    else
    {
    	ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "THREAD_NUMBER is not specified in config, using default\n");
    }

    dalresult = DALSYS_GetPropertyValue(adsppm_property, "CACHE_SIZE_KB", 0, &system_property_var);
    if(dalresult == DAL_SUCCESS)
    {
    	// No checking for NULL since we are passing pointer to a static variable.
    	pConfig->adspCacheSize = system_property_var.Val.dwVal;
    }
    else
    {
    	ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "CACHE_SIZE_KB is not specified in config, using default\n");
    }

    dalresult = DALSYS_GetPropertyValue(adsppm_property, "LPASS_BASE_ADDR", 0, &system_property_var);
    if(dalresult == DAL_SUCCESS)
    {
    	// No checking for NULL since we are passing pointer to a static variable.
    	pConfig->lpassBaseAddr = (AsicHwioAddrRangeType *) system_property_var.Val.pStruct;
    }
    else
    {
    	ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "LPASS_BASE_ADDR is not specified in config, using default\n");
    }



    /**
     *    Getting Device Config for CPP factors
     */
    dalresult = DALSYS_GetPropertyValue(adsppm_property, "CPP_FACTORS", 0, &system_property_var);
    if(dalresult == DAL_SUCCESS)
    {
    	pConfig->cppFactors = (AsicCpuAggregatorMppsCppArrayType *) system_property_var.Val.pStruct;
    	/**
    	 *    Getting Device Config for CPP fudge factors
    	 */
    	dalresult = DALSYS_GetPropertyValue(adsppm_property, "CPP_FUDGE_FACTORS", 0, &system_property_var);
    	if(dalresult == DAL_SUCCESS)
    	{
    		pConfig->cppFudgeFactors = (AsicCpuAggregatorCppAdjustmentArrayType *) system_property_var.Val.pStruct;

    		/**
    		 *    Getting Device Config for Cores
    		 */
    		dalresult = DALSYS_GetPropertyValue(adsppm_property, "CORE_DESCRIPTORS", 0, &system_property_var);
    		if(dalresult == DAL_SUCCESS)
    		{
    			pConfig->cores = (AsicCoreDescriptorArrayType *) system_property_var.Val.pStruct;
    			//pConfig->cores.numElements = Adsppm_Core_Id_Max;
    			/**
    			 *    Getting Device Config for Memories
    			 */
    			dalresult = DALSYS_GetPropertyValue(adsppm_property,"MEMORY_DESCRIPTORS",0,&system_property_var);
    			if(dalresult == DAL_SUCCESS)
    			{
    				AsicMemDescriptorArrayType *pMemories = (AsicMemDescriptorArrayType *)system_property_var.Val.pStruct;
					pConfig->memories.numElements = pMemories->numElements;
					//get addresses for each memory
					for(i = Adsppm_Mem_None; i < Adsppm_Mem_Max; i++)
					{
						AsicMemDescriptorFullType *pMem = &pConfig->memories.pMemories[i];
						pMem->pDescriptor = &pMemories->pPwrDomainDesc[i];
						//Initialize address range with zeroes
						pMem->virtAddr.startAddr = 0;
						pMem->virtAddr.size = 0; //We will not flush cache for memories with size of 0
					}
    				//pConfig->memories.numElements = Adsppm_Mem_Max;
    				/**
    				 *    Getting Device Config for Clocks
    				 */
    				dalresult = DALSYS_GetPropertyValue(adsppm_property,"CLOCK_DESCRIPTORS",0,&system_property_var);
    				if(dalresult == DAL_SUCCESS)
    				{
    					pConfig->clocks = (AsicClockDescriptorArrayType *) system_property_var.Val.pStruct;
    					//pConfig->clocks.numElements = AdsppmClk_EnumMax;
    					/**
    					 *    Getting Device Config for Bus Ports
    					 */
    					dalresult = DALSYS_GetPropertyValue(adsppm_property,"BUS_PORT_DESCRIPTORS",0,&system_property_var);
    					if(dalresult == DAL_SUCCESS)
    					{
    						pConfig->busPorts = (AsicBusPortDescriptorArrayType*) system_property_var.Val.pStruct;
    						//pConfig->busPorts.numElements = AdsppmBusPort_EnumMax;
    					}
    					else //Bus port descriptors failed
    					{
    						ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Unable to retrieve BUS_PORT_DESCRIPTORS from config\n");
    						result = Adsppm_Status_Failed;
    					}
    				}
    				else //Clock descriptors failed
    				{
    					ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Unable to retrieve CLOCK_DESCRIPTORS from config\n");
    					result = Adsppm_Status_Failed;
    				}
    			}
    			else //Memory descriptors failed
    			{
    				ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Unable to retrieve MEMORY_DESCRIPTORS from config\n");
    				result = Adsppm_Status_Failed;
    			}
    		}
    		else //Core descriptors failed
    		{
    			ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Unable to retrieve CORE_DESCRIPTORS from config\n");
    			result = Adsppm_Status_Failed;
    		}
    	}
    	else //CPP factors failed
    	{
    		ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Unable to retrieve CPP_FUDGE_FACTORS from config\n");
    		result = Adsppm_Status_Failed;
    	}
    }
    else //CPP fudge factors failed
    {
    	ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Unable to retrieve CPP_FACTORS from config\n");
    	result = Adsppm_Status_Failed;
    }
    if(result != Adsppm_Status_Failed)
    {
    	/**
    	 *    Getting Device Config for External Bus Routes
    	 */
    	dalresult = DALSYS_GetPropertyValue(adsppm_property,"EXTERNAL_BUS_ROUTES",0,&system_property_var);
    	if(dalresult == DAL_SUCCESS)
    	{
    		pConfig->extBusRoutes = (AsicBusRouteArrayType*) system_property_var.Val.pStruct;

    		dalresult = DALSYS_GetPropertyValue(adsppm_property,"MIPS_BUS_ROUTES",0,&system_property_var);
    		if(dalresult == DAL_SUCCESS)
    		{
    			pConfig->mipsBusRoutes = (AsicBusRouteArrayType*) system_property_var.Val.pStruct;

    			/**
    			 *    Getting Device Config for Power Domains
    			 */
    			dalresult = DALSYS_GetPropertyValue(adsppm_property,"POWER_DOMAIN_DESCRIPTORS",0,&system_property_var);
    			if(dalresult == DAL_SUCCESS)
    			{
    				pConfig->pwrDomains = (AsicPwrDescriptorArrayType *) system_property_var.Val.pStruct;
    				//pConfig->pwrDomains-numElements = AsicPowerDomain_EnumMax;
    				/*
    				 *    Getting Device Config for Features
    				 */
    				if (0xff == gADSPPMPOWERFEATURE)
    				{
    					dalresult = DALSYS_GetPropertyValue(adsppm_property,"FEATURE_DESCRIPTORS",0,&system_property_var);
    					if(dalresult == DAL_SUCCESS)
    					{
    						memscpy(pConfig->features, AsicFeatureId_enum_max*sizeof(AsicFeatureDescType), system_property_var.Val.pStruct, AsicFeatureId_enum_max*sizeof(AsicFeatureDescType));
    					}
    					else //Features failed
    					{
    						ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Unable to retrieve FEATURE_DESCRIPTORS from config\n");
    						result = Adsppm_Status_Failed;
    					}
    				}
    				else
    				{
    					if(0 == gADSPPMPOWERFEATURE)
    					{
    						featureState = AsicFeatureState_Disabled;
    					}
    					else if (1 == gADSPPMPOWERFEATURE)
    					{
    						featureState = AsicFeatureState_Enabled;
    					}
    					for (featureIndex = AsicFeatureId_Adsp_Clock_Scaling; featureIndex <AsicFeatureId_enum_max; featureIndex++)
    					{
    						pConfig->features[featureIndex].state = featureState;
    					}
    					/* some specal feature need to disable now*/
    					pConfig->features[AsicFeatureId_Ahb_DCG].state = AsicFeatureState_Disabled;
    					pConfig->features[AsicFeatureId_LpassCore_PC].state = AsicFeatureState_Disabled;
    					pConfig->features[AsicFeatureId_LpassCore_PC_TZ_Handshake].state = AsicFeatureState_Disabled;
    				}
    			}
    			else //Power domain descriptors failed
    			{
    				ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Unable to retrieve POWER_DOMAIN_DESCRIPTORS from config\n");
    				result = Adsppm_Status_Failed;
    			}
    		}
    		else //External bus routes failed
    		{
    			ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "MIPS_BUS_ROUTES  are corrupt");
    			result = Adsppm_Status_Failed;
    		}

    	}
    	else //External bus routes failed
    	{
    		ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Unable to retrieve EXTERNAL_BUS_ROUTES from config\n");
    		result = Adsppm_Status_Failed;
    	}

    }
    return result;
}


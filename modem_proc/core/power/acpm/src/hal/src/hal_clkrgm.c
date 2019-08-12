/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * hal_clkrgm.c
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */

#include "adsppm.h"
#include "ULog.h"
#include "ULogFront.h"
#include "adsppm_utils.h"
#include "hal_clkrgm.h"
#include "asic.h"
#include "DDIClock.h"
#include "DALStdDef.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "ClockDefs.h"
#include "ddiaxicfg.h"
#include "npa.h"
#include "hal_hwio_clkctrl.h"
#include "string.h"
#include "stdlib.h"
#include "qurt_memory.h"

extern boolean gULTAudioResetDone;
/*
 * Data type definitions
 */
typedef struct{
    AsicClkDescriptorType* pClkDescriptor; //!< Pointer to the clock descriptor from ACM
    union{
        ClockIdType clkId; //!< Clock Id for DAL clocks
        npa_client_handle npaHandle; //!< NPA handle for NPA clocks
    }clkHandle;
    uint32 currentFreq; //!< Clock frequency currently set
    Hal_ClkRgmEnableDisableType currentState; //!< Clock enablement state
    uint32 requestCount; //!< Number of clients requesting this clock
} HalClk_ClockDescriptorType;

typedef struct{
    AsicPowerDomainDescriptorType* pPwrDescriptor;
    union{
        ClockPowerDomainIdType domainId;
        npa_client_handle npaHandle;
    }pwrHandle;
} HalClk_PwrDomainDescriptorType;

typedef struct{
    DALSYSSyncHandle ctxLock;
    DalDeviceHandle  *pHandle;
    HalClk_ClockDescriptorType clockDesc[AdsppmClk_EnumMax];
    HalClk_PwrDomainDescriptorType pwrDesc[AsicPowerDomain_EnumMax];
}Hal_ClkRgmCtxType;

static Hal_ClkRgmCtxType gClkRgmCtx;

static void NpaADSPPowerAvailableCb( void         *context,
        unsigned int  event_type,
        void         *data,
        unsigned int  data_size );

Adsppm_Status clkrgm_MemCache_Flush(AdsppmMemIdType mem)
{
    Adsppm_Status result = Adsppm_Status_Success;
    AsicAddressRangeType *pAddrRange = ACM_GetVirtMemAddressRange(mem);
    if(NULL != pAddrRange)
    {
        if(pAddrRange->size > 0)
        {
            //flush memory
            int res = QURT_EOK;
            ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO, "Flushing cache for memory %u (addr = 0x%X, size = %u)",
                                mem, (uint32)pAddrRange->startAddr, pAddrRange->size);
            res = qurt_mem_cache_clean(pAddrRange->startAddr, pAddrRange->size, QURT_MEM_CACHE_FLUSH, QURT_MEM_DCACHE);

            if(res != QURT_EOK)
            {
                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "Cannot flush cache for memory %u", mem);
                result = Adsppm_Status_Failed;
            }
        }
        else
        {
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Skipping cache flush for memory %u (mapping not specified)", mem);
        }
    }
    else
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "NULL descriptor for memory ID %u", mem);
        result = Adsppm_Status_ResourceNotFound;
    }
    return result;
}

Adsppm_Status clkrgm_EnableDalClock(HalClk_ClockDescriptorType* pClk)
{
    Adsppm_Status result = Adsppm_Status_Success;
    //Call clock regime to enable clock
    DALResult dalResult = DalClock_EnableClock(gClkRgmCtx.pHandle,
            pClk->clkHandle.clkId);
    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Enabled %s",
            pClk->pClkDescriptor->clkInfo.clkName);
    if (DAL_SUCCESS != dalResult)
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                "ClkRegime returned failure");
        result = Adsppm_Status_Failed;
    }
    return result;
}

Adsppm_Status clkrgm_DisableDalClock(HalClk_ClockDescriptorType* pClk)
{
    DALResult dalResult = DAL_SUCCESS;
    Adsppm_Status result = Adsppm_Status_Success;
    //Call clock regime to disable clock
    dalResult = DalClock_DisableClock(gClkRgmCtx.pHandle,
            pClk->clkHandle.clkId);
    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Disabled %s",
            pClk->pClkDescriptor->clkInfo.clkName);
    if (DAL_SUCCESS != dalResult)
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                "ClkRegime returned failure");
        result = Adsppm_Status_Failed;
    }
    return result;
}

Adsppm_Status clkrgm_EnableDalClockHwCtl(HalClk_ClockDescriptorType* pClk)
{
    Adsppm_Status result = Adsppm_Status_Success;
    DALResult dalResult = DAL_SUCCESS;
    // Enable HW control
    //TODO: 09/10/2013, Clock Regime calls disabled until Clock Regime implements necessary functionality
    /*

     ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Enabled DCG for %s",
     pClk->pClkDescriptor->clkInfo.clkName);
     dalResult = DalClock_ConfigClock(gClkRgmCtx.pHandle, pClk->clkHandle.clkId,
     CLOCK_CONFIG_LPASS__HW_CTL_ON);
     }*/
    if (DAL_SUCCESS != dalResult)
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                "ClkRegime returned failure");
        result = Adsppm_Status_Failed;
    }
    return result;
}

Adsppm_Status clkrgm_DisableDalClockHwCtl(HalClk_ClockDescriptorType* pClk)
{
    Adsppm_Status result = Adsppm_Status_Success;
    DALResult dalResult = DAL_SUCCESS;
    // Enable HW control
    //TODO: 09/10/2013, Clock Regime calls disabled until Clock Regime implements necessary functionality
    /*

     ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Disabled DCG for %s",
     pClk->pClkDescriptor->clkInfo.clkName);
     dalResult = DalClock_ConfigClock(gClkRgmCtx.pHandle, pClk->clkHandle.clkId,
     CLOCK_CONFIG_LPASS__HW_CTL_OFF);
     }*/
    if (DAL_SUCCESS != dalResult)
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                "ClkRegime returned failure");
        result = Adsppm_Status_Failed;
    }
    return result;
}

__inline Adsppm_Status clk_EnableClock(AdsppmClkIdType clk, HalClk_ClockDescriptorType* pClk)
{
    Adsppm_Status result = Adsppm_Status_UnSupported;
    // disable clock
    switch (pClk->pClkDescriptor->clkType)
    {
    case AsicClk_TypeDalFreqSet:
    case AsicClk_TypeDalEnable:
        result = clkrgm_EnableDalClock(pClk);
        break;
    case AsicClk_TypeInternalCGC:
        result = HalHwIo_EnableClock(clk);
        break;
    default:
        break;
    }
    return result;
}

__inline Adsppm_Status clk_DisableClock(AdsppmClkIdType clk, HalClk_ClockDescriptorType* pClk)
{
    Adsppm_Status result = Adsppm_Status_UnSupported;
    // disable clock
    switch (pClk->pClkDescriptor->clkType)
    {
    case AsicClk_TypeDalFreqSet:
    case AsicClk_TypeDalEnable:
        result = clkrgm_DisableDalClock(pClk);
        break;
    case AsicClk_TypeInternalCGC:
        result = HalHwIo_DisableClock(clk);
        break;
    default:
        break;
    }
    return result;
}

__inline Adsppm_Status clk_DisableClockHwCtl(AdsppmClkIdType clk, HalClk_ClockDescriptorType* pClk)
{
    Adsppm_Status result = Adsppm_Status_UnSupported;
    // disable clock
    switch (pClk->pClkDescriptor->clkType)
    {
    case AsicClk_TypeDalFreqSet:
    case AsicClk_TypeDalEnable:
        result = clkrgm_DisableDalClockHwCtl(pClk);
        break;
    case AsicClk_TypeInternalCGC:
        result = HalHwIo_DisableHWClockCntl(clk);
        break;
    default:
        break;
    }
    return result;
}

__inline Adsppm_Status clk_EnableClockHwCtl(AdsppmClkIdType clk, HalClk_ClockDescriptorType* pClk)
{
    Adsppm_Status result = Adsppm_Status_UnSupported;
    // disable clock
    switch (pClk->pClkDescriptor->clkType)
    {
    case AsicClk_TypeDalFreqSet:
    case AsicClk_TypeDalEnable:
        result = clkrgm_EnableDalClockHwCtl(pClk);
        break;
    case AsicClk_TypeInternalCGC:
        result = HalHwIo_EnableHWClockCntl(clk);
        break;
    default:
        break;
    }
    return result;
}

Adsppm_Status clkrgm_EnableClockBranch(AdsppmClkIdType clk)
{
    Adsppm_Status result = Adsppm_Status_Success;
    HalClk_ClockDescriptorType *pClk = &gClkRgmCtx.clockDesc[clk];

    //Call clock regime to enable clock
    switch (pClk->pClkDescriptor->clkCntlType)
    {
    case AsicClk_CntlOff: //don't enable Always OFF and HW controlled clocks
        break;
    case AsicClk_CntlHW:
        pClk->currentState = Hal_ClkRgmEnable;
        break;
    default://enable clock and disable HW control for all other clocks
        result = clk_EnableClock(clk, pClk);
        if (Adsppm_Status_Success == result)
        {
            //Disable HW gating for SW and DCG clocks unconditionally to force the clock On.
            result = clk_DisableClockHwCtl(clk, pClk);
        }
        if (Adsppm_Status_Success == result)
        {
            ADSPPM_QDSS_EVENT_1(ADSPPM_CLK_ENABLE, clk);
            pClk->currentState = Hal_ClkRgmEnable;
        }
        break;
    }

    return result;
}



Adsppm_Status clkrgm_DisableClockBranch(AdsppmClkIdType clk)
{
    Adsppm_Status result = Adsppm_Status_Success;
    HalClk_ClockDescriptorType *pClk =&gClkRgmCtx.clockDesc[clk];

    if((Adsppm_Mem_None != pClk->pClkDescriptor->memoryId))
    {
        //flush cache before disabling memory clock
        result = clkrgm_MemCache_Flush(pClk->pClkDescriptor->memoryId);
    }

    switch (pClk->pClkDescriptor->clkCntlType)
    {
    case AsicClk_CntlOff:
    case AsicClk_CntlSW:
    case AsicClk_CntlSW_SHARED:
        // disable clock
        result = clk_DisableClock(clk, pClk);
        if (Adsppm_Status_Success == result)
        {
            ADSPPM_QDSS_EVENT_1(ADSPPM_CLK_DISABLE, clk);
            pClk->currentState = Hal_ClkRgmDisable;
        }
        break;
    case AsicClk_CntlAlwaysON:
        break;
    case AsicClk_CntlHW:
        //Don't disable AON clocks and the ones in HW control
        pClk->currentState = Hal_ClkRgmDisable;
        break;
    case AsicClk_CntlSW_DCG:
        //Put clock under HW control if DCG is Enabled
        // Or disable it if DCG is not enabled
        if (AsicFeatureState_Enabled
                == ACM_GetFeatureStatus(AsicFeatureId_Ahb_DCG))
        {
            //Enable  clock
            result = clk_EnableClock(clk, pClk);
            if (Adsppm_Status_Success == result)
            {
                //Enable HW control
                result = clk_EnableClockHwCtl(clk, pClk);
            }
        }
        else
        {
            result = clk_DisableClock(clk, pClk);
        }
        if (Adsppm_Status_Success == result)
        {
            ADSPPM_QDSS_EVENT_1(ADSPPM_CLK_DISABLE, clk);
            pClk->currentState = Hal_ClkRgmDisable;
        }
        break;
    case AsicClk_CntlAlwaysON_DCG:
        //Enable  clock
        result = clk_EnableClock(clk, pClk);
        //Put clock under HW control if DCG is Enabled
        if ((Adsppm_Status_Success == result)
                && AsicFeatureState_Enabled
                        == ACM_GetFeatureStatus(AsicFeatureId_Ahb_DCG))
        {
            //Enable HW control
            result = clk_EnableClockHwCtl(clk, pClk);
        }
        break;
    default:
        break;
    }

    return result;
}


Adsppm_Status clkrgm_MemCore_Retention(AdsppmClkIdType clk, Hal_ClkRgmMemStateType pwrState)
{
    DALResult dalResult = DAL_SUCCESS;
    Adsppm_Status result = Adsppm_Status_Success;
    HalClk_ClockDescriptorType *pClk = NULL;
    Hal_ClkRgmEnableDisableType currentClkState= Hal_ClkRgmEnable;
    
    if(( AdsppmClk_None < clk) && ( AdsppmClk_EnumMax > clk))
    {
        pClk = &gClkRgmCtx.clockDesc[clk];
        currentClkState= pClk->currentState;
        if(Hal_ClkRgmEnable != currentClkState)
        {
            //Call clock driver to enable the clock
            dalResult = clkrgm_EnableClockBranch(clk);
            if (DAL_SUCCESS != dalResult)
            {
                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                        "ClkRegime returned failure");
                result = Adsppm_Status_Failed;
            }
        }
        //Call clock regime to enable/disable mem core retention
        if (Hal_ClkRgmMemRetain == pwrState)
        {
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Memory retention for %s",
                    gClkRgmCtx.clockDesc[clk].pClkDescriptor->clkInfo.clkName);
            dalResult = DalClock_ConfigClock(gClkRgmCtx.pHandle,
                    gClkRgmCtx.clockDesc[clk].clkHandle.clkId, CLOCK_CONFIG_CORE_MEM_ON);
            ADSPPM_QDSS_EVENT_2(ADSPPM_MEM_PWR, clk, pwrState);
        }
        else
        {
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                    "Memory collapse enabled for %s",
                    gClkRgmCtx.clockDesc[clk].pClkDescriptor->clkInfo.clkName);
            dalResult = DalClock_ConfigClock(gClkRgmCtx.pHandle,
                    gClkRgmCtx.clockDesc[clk].clkHandle.clkId , CLOCK_CONFIG_CORE_MEM_OFF);
            ADSPPM_QDSS_EVENT_2(ADSPPM_MEM_PWR, clk, pwrState);
        }

        if (DAL_SUCCESS != dalResult)
        {
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                    "ClkRegime returned failure");
            result = Adsppm_Status_Failed;
        }

        if(Hal_ClkRgmEnable != currentClkState)
        {
            //restore previous clock state
            dalResult = clkrgm_DisableClockBranch(clk);
            if (DAL_SUCCESS != dalResult)
            {
                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                        "ClkRegime returned failure");
                result = Adsppm_Status_Failed;
            }
        }
    }
    else
    {
        result = Adsppm_Status_Failed;  
    }
    return result;
}


void clkrgm_SharedClkInit(void)
{
    AdsppmClkIdType i;
    for(i = AdsppmClk_None; i < AdsppmClk_EnumMax; i++)
    {
        HalClk_ClockDescriptorType *pClk = &gClkRgmCtx.clockDesc[i];
        if((NULL != pClk->pClkDescriptor) && (pClk->pClkDescriptor->clkType != AsicClk_TypeNone))
        {
            //Only proceed for clocks that are present in the chip.
            //Otherwise just skip the setting
            if( AsicClk_CntlSW_SHARED == pClk->pClkDescriptor->clkCntlType)
            {
                pClk->currentState = Hal_ClkRgmDisable;
            }
        }
    }
}


Adsppm_Status clkrgm_SharedClkControl(Hal_ClkRgmEnableDisableType stateRequested)
{
    Adsppm_Status result = Adsppm_Status_Success;
    AdsppmClkIdType i; 
    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_FUNC_PROFILESTART;

    //enable or disable rest of shared clocks
    for(i = AdsppmClk_None; i < AdsppmClk_EnumMax; i++)
    {
        HalClk_ClockDescriptorType *pClk = &gClkRgmCtx.clockDesc[i];
        if((NULL != pClk->pClkDescriptor) && (pClk->pClkDescriptor->clkType != AsicClk_TypeNone))
        {
            //Only proceed for clocks that are present in the chip.
            //Otherwise just skip the setting
            if((AsicClk_CntlSW_SHARED == pClk->pClkDescriptor->clkCntlType) && (pClk->currentState != stateRequested))
            {
                result = ClkRgm_EnableClock(i, stateRequested);
                ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO, "turn on/off shared clockid %d, state=%d", i, stateRequested);
            }
            if (Adsppm_Status_Success != result)
            {
                ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR, "Failed to turn on/off shared clockid %d, state=%d", i, stateRequested);
                break;
            }
        }
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    ADSPPM_LOG_FUNC_EXIT(result);
    return result;    
}

Adsppm_Status ClkRgm_Init(void)
{
    Adsppm_Status result = Adsppm_Status_Success;
    int i = 0;
    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_FUNC_PROFILESTART;

    //Create mutex
    if(DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, (DALSYSSyncHandle *)&(gClkRgmCtx.ctxLock), NULL ))
    {
        result = Adsppm_Status_NotInitialized;
    }
    else
    {
        //Register handle for ClkRegime
        if (DAL_SUCCESS == DAL_ClockDeviceAttach(DALDEVICEID_CLOCK, &(gClkRgmCtx.pHandle)))
        {
            //Get information for each power domain from ClkRegime
            for(i =AsicPowerDomain_AON; (Adsppm_Status_Success == result) && (i < AsicPowerDomain_EnumMax); i++)
            {
                HalClk_PwrDomainDescriptorType *pPwr = &gClkRgmCtx.pwrDesc[i];
                pPwr->pPwrDescriptor = ACMClk_GetPwrDomainDescriptor((AsicPowerDomainType)i);
                if (NULL != pPwr->pPwrDescriptor)
                {
                    switch (pPwr->pPwrDescriptor->pwrDomainType)
                    {
                    case AsicRsc_Power_ADSP:
                        //Register with NPA node for vdd voting
                        //Require ClkRegime to be initialized before ADSPPM, so the npa clocks have to be available at this point
                        npa_resource_available_cb(pPwr->pPwrDescriptor->pwrDomainName, NpaADSPPowerAvailableCb, NULL);
                        break;
                    case AsicRsc_Power_Mem:
                        break;
                    case AsicRsc_Power_Core:
                        //Get domain Id from ClkRegime based on clock name
                        //Ignore domains with empty name
                        if(strncmp(pPwr->pPwrDescriptor->pwrDomainName, "", MAX_ADSPPM_CLIENT_NAME))
                        {
                            if (DAL_SUCCESS != DalClock_GetPowerDomainId(gClkRgmCtx.pHandle, pPwr->pPwrDescriptor->pwrDomainName, &(pPwr->pwrHandle.domainId)))
                            {
                                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "Failed to get Power domain ID for %s", pPwr->pPwrDescriptor->pwrDomainName);
                                result = Adsppm_Status_Failed;
                            }
                        }
                        break;
                    default:
                        break;
                    }
                }
                else
                {
                    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "Failed to get Pwr descriptor for power domain %d", i);
                    result = Adsppm_Status_Failed;
                }
                if (Adsppm_Status_Success != result)
                {
                    break;
                }
            }

            //Get information for each clock from ClkRegime
            for(i =AdsppmClk_None; (Adsppm_Status_Success == result) && (i < AdsppmClk_EnumMax); i++)
            {
                HalClk_ClockDescriptorType *pClk = &gClkRgmCtx.clockDesc[i];
                pClk->pClkDescriptor = ACMClk_GetClockDescriptor((AdsppmClkIdType)i);
                pClk->currentFreq = 0;
                pClk->requestCount = 0;
                if(NULL != pClk->pClkDescriptor)
                {
                    switch (pClk->pClkDescriptor->clkType)
                    {
                    case AsicClk_TypeNpa:
                        //Register with NPA node
                        //Require ClkRegime to be initialized before ADSPPM, so the npa clocks have to be available at this point
                        //npa_resource_available_cb(pClk->pClkDescriptor->clkInfo.clkName, NpaClkAvailableCb, NULL);
                        pClk->clkHandle.npaHandle = npa_create_sync_client( pClk->pClkDescriptor->clkInfo.clkName,
                                "ADSPPM",
                                NPA_CLIENT_REQUIRED);
                        if (NULL == pClk->clkHandle.npaHandle)
                        {
                            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "Failed to create NPA client for %s", pClk->pClkDescriptor->clkInfo.clkName);
                            result = Adsppm_Status_Failed;
                        }
                        break;
                    case AsicClk_TypeDalFreqSet:
                    case AsicClk_TypeDalEnable:
                    case AsicClk_TypeDalDomainSrc:
                        //Get clock Id from ClkRegime based on clock name
                        if (DAL_SUCCESS != DalClock_GetClockId(gClkRgmCtx.pHandle, pClk->pClkDescriptor->clkInfo.clkName, &(pClk->clkHandle.clkId)))
                        {
                            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "Failed to get ClkId for %s", pClk->pClkDescriptor->clkInfo.clkName);
                            result = Adsppm_Status_Failed;
                        }
                        break;
                    default:
                        break;
                    }
                }
                else
                {
                    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "Failed to get Clk descriptor for clk %d", i);
                    result = Adsppm_Status_Failed;
                }
            }

            //set init state for shared clock
            clkrgm_SharedClkInit();
            //turn ON AsicClk_CntlSW_SHARE clocks first
            result = clkrgm_SharedClkControl(Hal_ClkRgmEnable); 

            //Set initial state for each clock
            for(i =AdsppmClk_None; (Adsppm_Status_Success == result) && (i < AdsppmClk_EnumMax); i++)
            {
                HalClk_ClockDescriptorType *pClk = &gClkRgmCtx.clockDesc[i];

                if((NULL != pClk->pClkDescriptor) && (pClk->pClkDescriptor->clkType != AsicClk_TypeNone))
                {
                    //Only proceed for clocks that are present in the chip.
                    //Otherwise just skip the setting
                    switch(pClk->pClkDescriptor->clkCntlType)
                    {
                    case AsicClk_CntlAlwaysON:
                        //Enable AON clock
                        pClk->currentState = Hal_ClkRgmDisable;
                        result = ClkRgm_EnableClock(i, Hal_ClkRgmEnable);
                        break;
                    case AsicClk_CntlOff:
                        //Disable AOFF clock
                        pClk->currentState = Hal_ClkRgmEnable;
                        result = ClkRgm_EnableClock(i, Hal_ClkRgmDisable);
                        break;
                    case AsicClk_CntlAlwaysON_DCG:
                        if(AsicFeatureState_Enabled == ACM_GetFeatureStatus(AsicFeatureId_Ahb_DCG))
                        {
                            //Enable clock and enable HW control
                            //That will be handled by ClkRgm_EnableClock internally if clock supports DCG and is disabled
                            pClk->currentState = Hal_ClkRgmEnable;
                            result = ClkRgm_EnableClock(i, Hal_ClkRgmDisable);
                        }
                        else
                        {
                            //Enable AON clock
                            pClk->currentState = Hal_ClkRgmDisable;
                            result = ClkRgm_EnableClock(i, Hal_ClkRgmEnable);
                        }
                        break;
                    case AsicClk_CntlSW:
                    case AsicClk_CntlSW_DCG:
                        //Disable clock or Enable clock and enable HW control
                        //That will be handled by ClkRgm_EnableClock internally if clock supports DCG and is disabled
                        pClk->currentState = Hal_ClkRgmEnable;
                        result = ClkRgm_EnableClock(i, Hal_ClkRgmDisable);
                        break;
                    case AsicClk_CntlHW:
                        pClk->currentState = Hal_ClkRgmDisable;
                        result = ClkRgm_EnableHWClockCntl(i, Hal_ClkRgmEnable);
                        break;
                    default: //Don't touch other clocks here
                        break;
                    }
                    if (Adsppm_Status_Success == result)
                    {
                        switch(pClk->pClkDescriptor->clkMemCntlType)
                        {
                        case AsicClk_MemCntlAlwaysRetain:
                            //Call clock regime to enable mem core retention
                            result = clkrgm_MemCore_Retention(i, Hal_ClkRgmMemRetain);
                            break;
                        default: //Don't touch other clocks here
                            break;
                        }
                    }
                }
            }
        }
        //turn off AsicClk_CntlSW_SHARED clocks
        if (Adsppm_Status_Success == result)
        {
            result = clkrgm_SharedClkControl(Hal_ClkRgmDisable); 
        }
        if (Adsppm_Status_Success != result)
        {
            DALSYS_DestroyObject((DALSYSSyncHandle *)&(gClkRgmCtx.ctxLock));
            gClkRgmCtx.ctxLock = NULL;
        }
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    ADSPPM_LOG_FUNC_EXIT(result);
    return result;
}


static void NpaADSPPowerAvailableCb( void         *context,
        unsigned int  event_type,
        void         *data,
        unsigned int  data_size )
{
    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_FUNC_PROFILESTART;
    HalClk_PwrDomainDescriptorType *pPwr = &gClkRgmCtx.pwrDesc[AsicPowerDomain_Adsp];

    pPwr->pwrHandle.npaHandle = npa_create_sync_client( pPwr->pPwrDescriptor->pwrDomainName,
            "adsppmadsppower",
            NPA_CLIENT_REQUIRED);
    if (NULL == pPwr->pwrHandle.npaHandle)
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Failed to create client for ADSP power");
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    ADSPPM_LOG_FUNC_EXIT(0);
}

/**
 * @fn ClkRgm_CalcClockFrequency
 * get clock frequency through clock regime calculate from
 * oscillator 
 * @param pFreq - out put frequency in Hz
 */


Adsppm_Status ClkRgm_CalcClockFrequency(AdsppmClkIdType clk, uint32 *pFreq)
{
    Adsppm_Status result = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_FUNC_PROFILESTART;
    if(( AdsppmClk_None < clk) && ( AdsppmClk_EnumMax > clk) && (NULL != pFreq ))
    {
        if(NULL != gClkRgmCtx.ctxLock )
        { 
            //TODO: USE calculate clock now, later on will use getfreq api from clock regime
            HalClk_ClockDescriptorType *pClk = &gClkRgmCtx.clockDesc[clk];
            DALResult dalResult = DAL_SUCCESS;
            if((NULL != pClk->pClkDescriptor) && (AdsppmClk_None != pClk->pClkDescriptor->clkId))
            {
                if(pClk->currentState == Hal_ClkRgmEnable)
                {
                    if(pClk->clkHandle.clkId != NULL)
                    {
                    dalResult = DalClock_CalcClockFrequency(gClkRgmCtx.pHandle, pClk->clkHandle.clkId, pFreq);
                    ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO, "get clock frequency %d for %s. currentset freq=%d ",
                            *pFreq,
                            pClk->pClkDescriptor->clkInfo.clkName,
                            pClk->currentFreq);
                    if(DAL_SUCCESS != dalResult)
                    {
                        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "ClkRegime returned failure");
                        result = Adsppm_Status_Failed;
                    }
                    }
                    else
                    {
                        if(AdsppmClk_None != pClk->pClkDescriptor->clkSrcId)
                        {
                            // Recursive call here.
                            // We are relying on the lock to work in recursive cases as it is stated in the API description
                            result = ClkRgm_CalcClockFrequency(pClk->pClkDescriptor->clkSrcId, pFreq);

                        }
                    }
                }
                else
                {
                    *pFreq = 0;
                }

            }
            else
            {
                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO, "bad pClk->pClkDescriptor->clkId ");
                result = Adsppm_Status_BadParm;
            }        
        }
        else
        {
            result = Adsppm_Status_NotInitialized;
        }
    }
    else
    {
        result = Adsppm_Status_BadParm;
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO, "bad clk or pFreq ");
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    ADSPPM_LOG_FUNC_EXIT(result);
    return result;
}


Adsppm_Status ClkRgm_GetClockFrequency(AdsppmClkIdType clk, uint32 *pFreq)
{
    Adsppm_Status result = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_FUNC_PROFILESTART;

    if(( AdsppmClk_None < clk) && ( AdsppmClk_EnumMax > clk) && (NULL != pFreq ))
    {
        if(NULL != gClkRgmCtx.ctxLock )
        {
            HalClk_ClockDescriptorType *pClk = &gClkRgmCtx.clockDesc[clk];
            if(pClk->currentState)
            {
                *pFreq = pClk->currentFreq;
            }
            else
            {
                *pFreq = 0;
            }
        }
        else
        {
            result = Adsppm_Status_NotInitialized;
        }
    }
    else
    {
        result = Adsppm_Status_NullPointer;
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    ADSPPM_LOG_FUNC_EXIT(result);
    return result;
}
/**
 * @fn ClkRgm_SetClock
 * Set clock frequency via ClkRegime
 * @param freq - frequency in Hz
 */
Adsppm_Status ClkRgm_SetClock(AdsppmClkIdType clk, uint32 freq)
{
    Adsppm_Status result = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_FUNC_PROFILESTART;
    
    if(( AdsppmClk_None < clk) && ( AdsppmClk_EnumMax > clk))
    {
        if( NULL != gClkRgmCtx.ctxLock )
        {
            HalClk_ClockDescriptorType *pClk = &gClkRgmCtx.clockDesc[clk];
            DALResult dalResult = DAL_SUCCESS;
            if (NULL != pClk->pClkDescriptor)
            {
                // Lock HW layer
                adsppm_lock(gClkRgmCtx.ctxLock);
                if(pClk->currentFreq != freq)
                {
                    switch (pClk->pClkDescriptor->clkType)
                    {
                    case AsicClk_TypeNpa:
                        //Use NPA node
                        if (NULL != pClk->clkHandle.npaHandle)
                        {
                            if(0 < freq)
                            {
                                ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO, "NPA request for %s, state: %d",pClk->pClkDescriptor->clkInfo.clkName, freq);
                                ADSPPM_QDSS_EVENT_3(ADSPPM_CLK_SET, clk, freq, freq);
                                //issue NPA request for specified frequency
                                //Translate from Hz to kHz first
                                npa_issue_required_request(pClk->clkHandle.npaHandle, freq/1000);
                                pClk->currentState = Hal_ClkRgmEnable;
                            }
                            else
                            {
                                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Complete NPA request for %s",pClk->pClkDescriptor->clkInfo.clkName);
                                ADSPPM_QDSS_EVENT_3(ADSPPM_CLK_SET, clk, 0, 0);
                                //release NPA request
                                npa_complete_request(pClk->clkHandle.npaHandle);
                                pClk->currentState = Hal_ClkRgmDisable;
                            }
                            pClk->currentFreq = freq;
                        }
                        else
                        {
                            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,"NULL NPA handle");
                            result = Adsppm_Status_Failed;
                        }
                        break;

                    case AsicClk_TypeDalEnable:
                    case AsicClk_TypeInternalCGC:
                        //Disable clock branch if 0 frequency is requested
                        if(0 < freq)
                        {
                            if(AdsppmClk_None != pClk->pClkDescriptor->clkSrcId)
                            {
                                //Use the source clock if available for enable/disable only clocks
                                result = ClkRgm_SetClock(pClk->pClkDescriptor->clkSrcId, freq);
                            }

                            if((Adsppm_Status_Success == result) && (pClk->currentState != Hal_ClkRgmEnable))
                            {
                                result = ClkRgm_EnableClock(clk, Hal_ClkRgmEnable);
                            }
                        }
                        else
                        {
                            if(pClk->currentState != Hal_ClkRgmDisable)
                            {
                                result = ClkRgm_EnableClock(clk, Hal_ClkRgmDisable);
                            }
                        }

                        if (DAL_SUCCESS != dalResult)
                        {
                            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "ClkRegime returned failure");
                            result = Adsppm_Status_Failed;
                        }
                        else
                        {
                            pClk->currentFreq = freq;
                        }
                        break;

                    case AsicClk_TypeDalFreqSet:
                    case AsicClk_TypeDalDomainSrc:
                        //Call clock regime to set at least requested frequency
                        //Disable clock branch if 0 frequency is requested
                        if(0 < freq)
                        {
                            //attempt to set frequency at least as requested
                            dalResult = DalClock_SetClockFrequency(gClkRgmCtx.pHandle, pClk->clkHandle.clkId, freq, CLOCK_FREQUENCY_HZ_AT_LEAST, &pClk->currentFreq);
                            if (DAL_SUCCESS != dalResult)
                            {
                                //Try to request cap the frequency if the previous request has failed because of exceeding max for this clock
                                dalResult = DalClock_SetClockFrequency(gClkRgmCtx.pHandle, pClk->clkHandle.clkId, freq, CLOCK_FREQUENCY_HZ_CLOSEST, &pClk->currentFreq);
                                ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO, "Failed to set frequency %d for %s. Capped freq=%d ",
                                        freq,
                                        pClk->pClkDescriptor->clkInfo.clkName,
                                        pClk->currentFreq);
                            }
                            else
                            {
                                ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO, "Requested frequency %d for %s. Set freq=%d ",
                                        freq,
                                        pClk->pClkDescriptor->clkInfo.clkName,
                                        pClk->currentFreq);
                                ADSPPM_QDSS_EVENT_3(ADSPPM_CLK_SET, clk, freq, pClk->currentFreq);
                            }

                            if((DAL_SUCCESS == dalResult) && (pClk->currentState != Hal_ClkRgmEnable))
                            {
                                result = ClkRgm_EnableClock(clk, Hal_ClkRgmEnable);

                            }
                        }
                        else
                        {
                            if(pClk->currentState != Hal_ClkRgmDisable)
                            {
                                result = ClkRgm_EnableClock(clk, Hal_ClkRgmDisable);
                            }
                        }
                        if (DAL_SUCCESS != dalResult)
                        {
                            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "ClkRegime returned failure");
                            result = Adsppm_Status_Failed;
                        }
                        else
                        {
                            pClk->currentFreq = freq;
                        }
                        break;
                    default:
                        break;
                    }
                }
            // Unlock HW layer
            adsppm_unlock(gClkRgmCtx.ctxLock);
            }
            else
            {
                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "invalid HalClk_ClockDescriptor");
                result = Adsppm_Status_BadParm;
            }
        }
        else
        {
            result = Adsppm_Status_NotInitialized;
        }
    }
    else
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "AdsppmClkId out of range");
        result = Adsppm_Status_BadParm;
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    ADSPPM_LOG_FUNC_EXIT(result);
    return result;
}


Adsppm_Status ClkRgm_EnableClock(AdsppmClkIdType clk, Hal_ClkRgmEnableDisableType stateRequested)
{
    Adsppm_Status result = Adsppm_Status_Success;
    Hal_ClkRgmEnableDisableType state= stateRequested;
    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_FUNC_PROFILESTART;
    if(( AdsppmClk_None < clk) && ( AdsppmClk_EnumMax > clk))
    {
        if(NULL != gClkRgmCtx.ctxLock )
        {
            HalClk_ClockDescriptorType *pClk = &gClkRgmCtx.clockDesc[clk];
            if (NULL != pClk->pClkDescriptor)
            {
                // Lock HW layer
                adsppm_lock(gClkRgmCtx.ctxLock);
                if(Hal_ClkRgmEnable == stateRequested)
                {
                    pClk->requestCount++;
                }
                else
                {
                    if(pClk->requestCount > 0) pClk->requestCount--;
                    //Override the requested state if the request counter is not 0
                    if(pClk->requestCount > 0) state = Hal_ClkRgmEnable;
                }
                ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_DEBUG, "Current Request Count for clk %d, count: %d",pClk->pClkDescriptor->clkId, pClk->requestCount);
                if(pClk->currentState != state)
                {
                    switch (pClk->pClkDescriptor->clkType)
                    {
                    case AsicClk_TypeNpa:
                        //Use NPA node
                        //Require ClkRegime to be initialized before ADSPPM, so the npa clocks have to be available at this point
                        if (NULL != pClk->clkHandle.npaHandle)
                        {
                            if(Hal_ClkRgmEnable == state)
                            {
                                npa_query_type query;
                                query.type = NPA_QUERY_TYPE_VALUE;
                                //Get min frequency for this clock
                                if(NPA_QUERY_SUCCESS == npa_query_by_client(pClk->clkHandle.npaHandle, CLOCK_NPA_QUERY_MIN_FREQ_KHZ, &query))
                                {
                                    ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO, "NPA request for %s, state: %d",pClk->pClkDescriptor->clkInfo.clkName, query.data.value);
                                    //issue NPA request for min frequency
                                    npa_issue_required_request(pClk->clkHandle.npaHandle, query.data.value);
                                    pClk->currentFreq = query.data.value;
                                }
                                else
                                {
                                    ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "ClkRegime query returned failure");
                                    result = Adsppm_Status_Failed;
                                }
                            }
                            else
                            {
                                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Complete NPA request for %s",pClk->pClkDescriptor->clkInfo.clkName);
                                //release NPA request
                                npa_complete_request(pClk->clkHandle.npaHandle);
                                pClk->currentFreq = 0;
                            }
                        }
                        else
                        {
                            result = Adsppm_Status_Failed;
                        }
                        break;
                    case AsicClk_TypeInternalCGC:
                        //enable clock source
                        if(AdsppmClk_None != pClk->pClkDescriptor->clkSrcId)
                        {
                            // Recursive call here.
                            // We are relying on the lock to work in recursive cases as it is stated in the API description
                            result = ClkRgm_EnableClock(pClk->pClkDescriptor->clkSrcId, state);

                            if(Adsppm_Status_Success != result)
                            {
                                //only break in the case of failure
                                break;
                            };
                        }
                        //no break statement here as we need to continue and enable/disable the clock branch
                    case AsicClk_TypeDalFreqSet:
                    case AsicClk_TypeDalEnable:
                    case AsicClk_TypeDalDomainSrc:
                        switch(state)
                        {
                        case Hal_ClkRgmEnable:
                            //Call clock regime to enable clock
                            result = clkrgm_EnableClockBranch(clk);
                            break;
                        case Hal_ClkRgmDisable:
                        default:
                            result = clkrgm_DisableClockBranch(clk);
                            break;
                        }
                        break;
                    default:
                        break;
                    }
                }
                // Unlock HW layer
                adsppm_unlock(gClkRgmCtx.ctxLock);
            }
            else
            {
                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "invalid HalClk_ClockDescriptor");
                result = Adsppm_Status_BadParm;
            }
        }
        else
        {
            result = Adsppm_Status_NotInitialized;
        }
    }
    else
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "AdsppmClkId out of range");
        result = Adsppm_Status_BadParm;
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    ADSPPM_LOG_FUNC_EXIT(result);
    return result;
}

/**
 * @fn ClkRgm_EnableHWClockCntl - enables/disables HW clock control for the specified clock
 * Function will return success if the clock doesn't support DCG
 * @param clk - clock ID
 * @param state - specifies whether or not HW control should be enabled
 * @return completion status
 */
Adsppm_Status ClkRgm_EnableHWClockCntl(AdsppmClkIdType clk, Hal_ClkRgmEnableDisableType state)
{
    Adsppm_Status result = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_FUNC_PROFILESTART;
    if(( AdsppmClk_None < clk) && ( AdsppmClk_EnumMax > clk))
    {
        if(NULL != gClkRgmCtx.ctxLock )
        {
            HalClk_ClockDescriptorType *pClk = &gClkRgmCtx.clockDesc[clk];
            if (NULL != pClk->pClkDescriptor)
            {
            // Lock HW layer
            adsppm_lock(gClkRgmCtx.ctxLock);

            switch (pClk->pClkDescriptor->clkType)
            {
            case AsicClk_TypeNpa:
                break;
            default:
                switch (pClk->pClkDescriptor->clkCntlType)
                {
                case AsicClk_CntlSW_DCG:
                case AsicClk_CntlAlwaysON_DCG:
                case AsicClk_CntlHW:
                    //Call clock regime to enable/disable HW control for the clock
                    if(Hal_ClkRgmEnable == state)
                    {
                        result = clk_EnableClockHwCtl(clk, pClk);
                    }
                    else
                    {
                        result = clk_DisableClockHwCtl(clk, pClk);
                    }
                    break;
                default:
                    break;
                }
                break;
            }
            // Unlock HW layer
            adsppm_unlock(gClkRgmCtx.ctxLock);
            }
            else
            {
                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "invalid HalClk_ClockDescriptor");
                result = Adsppm_Status_BadParm;
            }
        }
        else
        {
            result = Adsppm_Status_NotInitialized;
        }
    }
    else
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "AdsppmClkId out of range");
        result = Adsppm_Status_BadParm;
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    ADSPPM_LOG_FUNC_EXIT(result);
    return result;
}

Adsppm_Status clkrgm_TurnOnPowerDomain(HalClk_PwrDomainDescriptorType *pPwr)
{
    DALResult dalResult= DAL_SUCCESS;
    Adsppm_Status result = Adsppm_Status_Success;
    //Call clock regime to enable power domain
    if(NULL != pPwr->pwrHandle.domainId)
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Enabled power domain %s",
                pPwr->pPwrDescriptor->pwrDomainName);
        dalResult = DalClock_EnablePowerDomain(gClkRgmCtx.pHandle,
                pPwr->pwrHandle.domainId);
        ADSPPM_QDSS_EVENT_1(ADSPPM_PWR_REQ, pPwr->pPwrDescriptor->pwrDomain);

        if (DAL_SUCCESS != dalResult)
        {
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                    "ClkRegime returned failure");
            result = Adsppm_Status_Failed;
        }
    }
    else
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Skipping power domain operation for %d", pPwr->pPwrDescriptor->pwrDomain);
    }
    return result;
}

Adsppm_Status clkrgm_TurnOffPowerDomain(HalClk_PwrDomainDescriptorType *pPwr)
{
    DALResult dalResult= DAL_SUCCESS;
    Adsppm_Status result = Adsppm_Status_Success;
    //Call clock regime to disable power domain
    if(NULL != pPwr->pwrHandle.domainId)
    {

        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Disabled power domain %s",
                pPwr->pPwrDescriptor->pwrDomainName);
        dalResult = DalClock_DisablePowerDomain(gClkRgmCtx.pHandle,
                pPwr->pwrHandle.domainId);
        ADSPPM_QDSS_EVENT_1(ADSPPM_PWR_REL, pPwr->pPwrDescriptor->pwrDomain);

        if (DAL_SUCCESS != dalResult)
        {
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                    "ClkRegime returned failure");
            result = Adsppm_Status_Failed;
        }
    }
    else
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Skipping power domain operation for %d", pPwr->pPwrDescriptor->pwrDomain);
    }
    return result;
}

Adsppm_Status clkrgm_ReinitDomainClocks(AsicPowerDomainType pwrDomain)
{
    Adsppm_Status result = Adsppm_Status_Success;
    AdsppmClkIdType i = AdsppmClk_None;
    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_FUNC_PROFILESTART;
    //Set initial state for each clock
    for(i = AdsppmClk_None; (Adsppm_Status_Success == result) && (i < AdsppmClk_EnumMax); i++)
    {
        HalClk_ClockDescriptorType *pClk = &gClkRgmCtx.clockDesc[i];

        if((NULL != pClk->pClkDescriptor) &&
                (pClk->pClkDescriptor->clkType != AsicClk_TypeNone) &&
                (pClk->pClkDescriptor->pwrDomain == pwrDomain))
        {
            //Only proceed for clocks that are present in the chip and belong to the specified domain.
            //Otherwise just skip the setting
            pClk->currentFreq = 0;
            pClk->requestCount = 0;
            switch(pClk->pClkDescriptor->clkCntlType)
            {
            case AsicClk_CntlAlwaysON:
                //Enable AON clock
                pClk->currentState = Hal_ClkRgmDisable;
                result = ClkRgm_EnableClock(i, Hal_ClkRgmEnable);
                break;
            case AsicClk_CntlOff:
                //Disable AOFF clock
                pClk->currentState = Hal_ClkRgmEnable;
                result = ClkRgm_EnableClock(i, Hal_ClkRgmDisable);
                break;
            case AsicClk_CntlAlwaysON_DCG:
                if(AsicFeatureState_Enabled == ACM_GetFeatureStatus(AsicFeatureId_Ahb_DCG))
                {
                    //Enable clock and enable HW control
                    //That will be handled by ClkRgm_EnableClock internally if clock supports DCG and is disabled
                    pClk->currentState = Hal_ClkRgmEnable;
                    result = ClkRgm_EnableClock(i, Hal_ClkRgmDisable);
                }
                else
                {
                    //Enable AON clock
                    pClk->currentState = Hal_ClkRgmDisable;
                    result = ClkRgm_EnableClock(i, Hal_ClkRgmEnable);
                }
                break;
            case AsicClk_CntlSW:
            case AsicClk_CntlSW_DCG:
                //Disable clock or Enable clock and enable HW control
                //That will be handled by ClkRgm_EnableClock internally if clock supports DCG and is disabled
                pClk->currentState = Hal_ClkRgmEnable;
                result = ClkRgm_EnableClock(i, Hal_ClkRgmDisable);
                break;
            case AsicClk_CntlHW:
                pClk->currentState = Hal_ClkRgmDisable;
                result = ClkRgm_EnableHWClockCntl(i, Hal_ClkRgmEnable);
                break;
            default: //Don't touch other clocks here
                break;
            }
            if (Adsppm_Status_Success == result)
            {
                switch(pClk->pClkDescriptor->clkMemCntlType)
                {
                case AsicClk_MemCntlAlwaysRetain:
                    //Call clock regime to enable mem core retention
                    result = clkrgm_MemCore_Retention(i, Hal_ClkRgmMemRetain);
                    break;
                default: //Don't touch other clocks here
                    break;
                }
            }
        }
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    ADSPPM_LOG_FUNC_EXIT(result);
    return result;
}

Adsppm_Status clkrgm_ResetPowerDomain(HalClk_PwrDomainDescriptorType *pPwr)
{
    DALResult dalResult = DAL_SUCCESS;
    Adsppm_Status result = Adsppm_Status_Success;
    AxiCfg_ReturnCodeType axiresult = AXICFG_SUCCESS;
    HalClk_ClockDescriptorType* pClk = &gClkRgmCtx.clockDesc[pPwr->pPwrDescriptor->clkId];
    AsicBusPortDescriptorType* pPortDesc= ACMBus_GetPortDescriptor(pPwr->pPwrDescriptor->masterBusPort);
    if (pPortDesc !=NULL)
    {
        if(pPwr->pPwrDescriptor->masterBusPort != AdsppmBusPort_None)
        {
            //do reset only for domains where both clock and bus port are specified

            //Halt bus traffic
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Halted bus port %d", pPortDesc->port);
            axiresult = AxiCfg_HaltMasterPort(pPortDesc->icbarbId.icbarbMaster);
            if (AXICFG_SUCCESS != axiresult)
            {
                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                        "Bus driver returned failure %d", axiresult);
                result = Adsppm_Status_Failed;
            }
        }
        if(pPwr->pPwrDescriptor->clkId != AdsppmClk_None)
        {
            //Assert reset
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Asserting reset for %s",
                                pClk->pClkDescriptor->clkInfo.clkName);
            dalResult = DalClock_ResetClock(gClkRgmCtx.pHandle, pClk->clkHandle.clkId, CLOCK_RESET_ASSERT);
            if (DAL_SUCCESS != dalResult)
            {
                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                        "ClkRegime returned failure");
                result = Adsppm_Status_Failed;
            }

            //Need at least 3 sleep clocks delay
            DALSYS_BusyWait(100);
        }
        if(pPwr->pPwrDescriptor->masterBusPort != AdsppmBusPort_None)
        {
            //Unhalt bus traffic
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Unhalted bus port %d", pPortDesc->port);
            axiresult = AxiCfg_UnhaltMasterPort(pPortDesc->icbarbId.icbarbMaster);
            if (AXICFG_SUCCESS != axiresult)
            {
                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                        "Bus driver returned failure %d", axiresult);
                result = Adsppm_Status_Failed;
            }
        }
        else
        {
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Skipping power domain bus halt/unhalt for %d", pPwr->pPwrDescriptor->pwrDomain);
        }
        if(pPwr->pPwrDescriptor->clkId != AdsppmClk_None)
        {
            //Deassert reset
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Deasserting reset for %s",
                    pClk->pClkDescriptor->clkInfo.clkName);
            dalResult = DalClock_ResetClock(gClkRgmCtx.pHandle, pClk->clkHandle.clkId, CLOCK_RESET_DEASSERT);
            if (DAL_SUCCESS != dalResult)
            {
                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                        "ClkRegime returned failure");
                result = Adsppm_Status_Failed;
            }
        }
        else
        {
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Skipping power domain reset for %d", pPwr->pPwrDescriptor->pwrDomain);
        }

    }
    else
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Invalid domain configuration (clock or bus port is NULL)");
    }
    return result;
}

Adsppm_Status clkrgm_EnablePowerDomain(Hal_ClkRgmEnableDisableType state, HalClk_PwrDomainDescriptorType *pPwr)
{
    Adsppm_Status result = Adsppm_Status_Success;

    if (Hal_ClkRgmEnable == state)
    {
        result = clkrgm_TurnOnPowerDomain(pPwr);
        if(result == Adsppm_Status_Success)
        {
            if ( FALSE == gULTAudioResetDone )
            {
            //Reset power domain after enabling it
            result = clkrgm_ResetPowerDomain(pPwr);
            if(result == Adsppm_Status_Success)
            {
                result = clkrgm_ReinitDomainClocks(pPwr->pPwrDescriptor->pwrDomain);
            }
        }
    }
    }
    else
    {
        result = clkrgm_TurnOffPowerDomain(pPwr);
        gULTAudioResetDone = FALSE;
    }
    return result;
}

Adsppm_Status ClkRgm_RequestPower(AsicPowerDomainType blk, Hal_ClkRgmEnableDisableType state)
{
    Adsppm_Status result = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_FUNC_PROFILESTART;
    if(blk < AsicPowerDomain_EnumMax)
    {
        if(NULL != gClkRgmCtx.ctxLock )
        {
            HalClk_PwrDomainDescriptorType *pPwr = &gClkRgmCtx.pwrDesc[blk];

            if (NULL != pPwr->pPwrDescriptor)
            {
            // Lock HW layer
            adsppm_lock(gClkRgmCtx.ctxLock);

            switch (pPwr->pPwrDescriptor->pwrDomainType)
            {
            case AsicRsc_Power_ADSP:
                //Use NPA node
                //Require ClkRegime to be initialized before ADSPPM, so the npa clocks have to be available at this point
                /* Disabling voting now as the node is not available*/
                if (NULL != pPwr->pwrHandle.npaHandle)
                {
                    if(Hal_ClkRgmEnable == state)
                    {
                        //issue NPA request for power
                        npa_issue_required_request(pPwr->pwrHandle.npaHandle, 1);
                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Enabled power domain %s",pPwr->pPwrDescriptor->pwrDomainName);
                        ADSPPM_QDSS_EVENT_1(ADSPPM_PWR_REQ, blk);

                    }
                    else
                    {
                        //release NPA request
                        npa_complete_request(pPwr->pwrHandle.npaHandle);
                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Disabled power domain %s",pPwr->pPwrDescriptor->pwrDomainName);
                        ADSPPM_QDSS_EVENT_1(ADSPPM_PWR_REL, blk);
                    }
                }
                else
                {
                    //    result = Adsppm_Status_Failed;
                    ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "ADSP power NPA handle is NULL");
                }
                break;

            case AsicRsc_Power_Mem:
                //Call clock regime to enable/disable mem core retention
                result = clkrgm_MemCore_Retention(pPwr->pPwrDescriptor->clkId, (state == Hal_ClkRgmEnable)?Hal_ClkRgmMemRetain:Hal_ClkRgmMemCollapse);
                break;

            case AsicRsc_Power_Core:
                //Call clock regime to enable/disable power domain
                //only call disable if the LPASS_Core PC feature is enabled
                //otherwise keep LPASS Core power enabled
                result = clkrgm_EnablePowerDomain(state, pPwr);

                break;
            default:
                break;
            }
            // Unlock HW layer
            adsppm_unlock(gClkRgmCtx.ctxLock);
            }
            else
            {
                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "invalid HalClk_PwrDomainDescriptor\n");
                result = Adsppm_Status_BadParm;
            }
        }
        else
        {
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Not initialized");
            result = Adsppm_Status_NotInitialized;
        }
    }
    else
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "AsicPowerDomainid out of range\n");
        result = Adsppm_Status_BadParm;
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    ADSPPM_LOG_FUNC_EXIT(result);
    return result;
}

Adsppm_Status ClkRgm_MemoryControl(AdsppmMemIdType memCore, Hal_ClkRgmMemStateType pwrState)
{
    Adsppm_Status result = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_FUNC_PROFILESTART;
    if((Adsppm_Mem_None < memCore) && (Adsppm_Mem_Max > memCore))
    {
        if(NULL != gClkRgmCtx.ctxLock )
        {
            AsicPowerDomainType pwrDomain = ACMPwr_GetMemPowerDomain(memCore);
            HalClk_PwrDomainDescriptorType *pPwr = &gClkRgmCtx.pwrDesc[pwrDomain];
            adsppm_lock(gClkRgmCtx.ctxLock);
            //Call clock regime to enable/disable mem core retention
            result = clkrgm_MemCore_Retention(pPwr->pPwrDescriptor->clkId, pwrState);
            adsppm_unlock(gClkRgmCtx.ctxLock);
        }
        else
        {
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Not initialized");
            result = Adsppm_Status_NotInitialized;
        }
    }
    else
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "AdsppmMemId out of range\n");
        result = Adsppm_Status_BadParm;
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    ADSPPM_LOG_FUNC_EXIT(result);
    return result;
}

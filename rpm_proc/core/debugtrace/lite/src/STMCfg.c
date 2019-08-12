/*=============================================================================

FILE:         STMCfg.c

DESCRIPTION:  

================================================================================
                Copyright 2011 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/rpm.bf/2.1.1/core/debugtrace/lite/src/STMCfg.c#2 $
=============================================================================*/

#include "STMCfg.h"
#include "halqdss_stm_config.c"  //lite_inline 
#include "qdss_stm_config.h"

#define QDSS_STM_ATID  16

__inline void STMCfgPreInit(uint32 ts_counter_freq)
{
   HAL_qdss_stm_SetConfigPhysAddr(QDSS_STM_BASE_CONST);
   HAL_qdss_stm_AccessUnLock();
   HAL_qdss_stm_SetTimeStampFreq(ts_counter_freq);
   HAL_qdss_stm_AccessLock();
}


__inline void STMCfgInit()
{
  HAL_qdss_stm_SetConfigPhysAddr(QDSS_STM_BASE_CONST);
  HAL_qdss_stm_AccessUnLock();
   if(!HAL_qdss_stm_isEnabled()) 
   { 
      HAL_qdss_stm_MasterSelectCfgAll();
      HAL_qdss_stm_PortEnableMask(0xFFFFFFFF); 
      HAL_qdss_stm_PortSelectCfgAll();
      HAL_qdss_stm_TimeStampEnable(); 
      HAL_qdss_stm_TraceIdCfg(QDSS_STM_ATID); 
      HAL_qdss_stm_SetDefaultSyncPeriod();

#ifdef QDSS_CTI_ENABLE
         HAL_qdss_stm_HWEventEnable();
         HAL_qdss_stm_HWEventSelectAll();
         HAL_qdss_stm_HWEventTriggerEnableAll();
#endif

      HAL_qdss_stm_Enable(); 

   } 
   HAL_qdss_stm_AccessLock();
}

__inline void STMSetTimestampFreq(uint32 ts_counter_freq)
{
   HAL_qdss_stm_AccessUnLock();
   HAL_qdss_stm_SetTimeStampFreq(ts_counter_freq);
   HAL_qdss_stm_AccessLock();
}

__inline boolean STMAccessStatusNSNID()
{
	return HAL_qdss_stm_AuthStatusNSNID();
}

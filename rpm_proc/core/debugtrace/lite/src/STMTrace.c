/*=============================================================================

FILE:         STMTrace.c

DESCRIPTION:  

================================================================================
                Copyright 2011 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/rpm.bf/2.1.1/core/debugtrace/lite/src/STMTrace.c#1 $
==============================================================================*/
#include "STMTrace.h"
#include "STMCfg.c"
#include "qdss_stm_config.h"


#define QDSS_STM_START_STIMULUS_PORT     1024
#define QDSS_STM_NUM_STIMULUS_PORTS      16

//TBD This needs to be mutex protected.
//It is left out now because the immediate use cases
//do not require it.
static uint8 _stm_ports[QDSS_STM_NUM_STIMULUS_PORTS];



__inline int STMTrace_NewPort(uint32 * uPort)
{
   uint8 i = 0;

   while ((i < QDSS_STM_NUM_STIMULUS_PORTS) && (_stm_ports[i] != 0))
   {
      i++;
   }
   if(i == QDSS_STM_NUM_STIMULUS_PORTS){
      return -1;  
         
   }


   _stm_ports[i] = 1;
   *uPort = QDSS_STM_STIMULUS_PORT_BASE_PHYS + 
            256 * (QDSS_STM_START_STIMULUS_PORT + i);
 
   return 0;
}


__inline void STMTrace_FreePort(uint32  uPort)
{
   uint8 i = (uint8)(((uPort - QDSS_STM_STIMULUS_PORT_BASE_PHYS)/256)-
                     QDSS_STM_START_STIMULUS_PORT);
   _stm_ports[i] = 0;
}

__inline void STMTrace_Data32(uint32 uPort, STMTraceDataType uType, uint32 uData)
{       
   *(volatile uint32 *)((uPort)|(0x98&(~uType)))=uData;
}


__inline void STMTrace_Data16(uint32 uPort, STMTraceDataType uType, uint16 uData)
{
   *(volatile uint16 *)((uPort)|(0x98&(~uType)))=uData;
}


__inline void STMTrace_Data8(uint32 uPort, STMTraceDataType uType, uint8 uData)
{
   *(volatile uint8 *)((uPort)|(0x98&(~uType)))=uData;
}


__inline void STMTrace_Flag(uint32 uPort, STMTraceFlagType uType)
{
   *(volatile uint32 *)((uPort)|(0xE8&(~uType)))=0;
}

__inline void STMTrace_Trigger(uint32 uPort, STMTraceTriggerType uType)
{
   *(volatile uint32 *)((uPort)|(0xF8&(~uType)))=0;
}

/*=============================================================================

FILE:         ETR.c

DESCRIPTION:  

================================================================================
              Copyright (c) 2011 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/rpm.bf/2.1.1/core/debugtrace/lite/src/ETR.c#1 $
==============================================================================*/

#include "ETR.h"
#include "halqdss_etr.c"   //lite_inline
#include "qdss_tmc_config.h"



void ConfigureETRToRAM(uint32 etr_ram_base_addr, uint32 etr_ram_size)
{
   //a. Set ramsize
   HAL_qdss_etr_SetRAMSize(etr_ram_size);

   //b. Program the MODE Register for Circular Buffer mode.
   HAL_qdss_etr_SetMode(ETR_MODE_CIRCULAR_BUFFER);

   //c. Program the WrBurstLen field of the AXI Control Register
   HAL_qdss_etr_SetWriteBurstLength(0xF);//recommended value in HPG


   //d. Program the ScatterGatherMode field of the AXI Control Register (AXICTL) to 0
   HAL_qdss_etr_DisableScatterGather();

   //Set cache and prot ctrl to HPG recommendations
   HAL_qdss_etr_SetCacheProtCtrl();


   HAL_qdss_etr_SetDataBufferAddress(etr_ram_base_addr);

   HAL_qdss_etr_EnableFormatting();

   HAL_qdss_etr_EnableTraceCapture();

}


void ETRPreInit(void)
{
   HAL_qdss_etr_HalConfigInit(QDSS_ETR_BASE_PHYS);
}


void ETRInit(void)
{

#ifdef QDSS_INIT_ETR
   HAL_qdss_etr_AccessUnLock();
   ConfigureETRToRAM(QDSS_ETR_RAM_BASE, QDSS_ETR_RAM_SIZE);
   HAL_qdss_etr_AccessLock();
#endif

}



void ETRFlush(void) 
{
   HAL_qdss_etr_AccessUnLock();
   HAL_qdss_etr_Flush();
#ifdef QDSS_DO_ETR_FLUSH_WAIT
   HAL_qdss_etr_FlushWait();
#endif
   HAL_qdss_etr_AccessLock();
}


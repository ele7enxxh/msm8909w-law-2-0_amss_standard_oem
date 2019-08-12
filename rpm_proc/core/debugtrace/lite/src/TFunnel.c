/*=============================================================================

FILE:         TFunnel.c

DESCRIPTION:  

================================================================================
                Copyright 2011 QUALCOMM Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/rpm.bf/2.1.1/core/debugtrace/lite/src/TFunnel.c#1 $
==============================================================================*/
#include "TFunnel.h"
#include "halqdss_tfunnel.c"  //lite_inline
#include "qdss_tfunnel_config.h"

//Funnel configuration is platform specific.
//However as of now for the B-family we need only one configuration below
//to just enable STM traffic. If need to manage multiple configurations
//the below code could be separated in to platform/chipset specific directories

#define QDSS_NUM_FUNNELS  1
static uint32  qdss_funnels[QDSS_NUM_FUNNELS] = 
   {
      QDSS_IN_FUN0_BASE
   };

#define IN_FUN0  0
#define IN_FUN1  1
#define MERG_FUN 2


__inline void TFunnelPreInit(void)
{
   HAL_qdss_tfunnel_NumFunnels(QDSS_NUM_FUNNELS); // set num funnels
   HAL_qdss_tfunnel_MemAllocTable((uint32)qdss_funnels);
}

__inline void TFunnelInit(void)
{
   HAL_qdss_tfunnel_UnlockAccessAll();

   //enable STM port of the in funnel for STM
   HAL_qdss_tfunnel_EnablePort(QDSS_IN_FUN_STM_PORT,QDSS_STM_IN_FUN);

   HAL_qdss_tfunnel_LockAccessAll();
}

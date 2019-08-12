/*=============================================================================

FILE:         ETFETB.c

DESCRIPTION:  

================================================================================
              Copyright (c) 2011 QUALCOMM Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/rpm.bf/2.1.1/core/debugtrace/lite/src/ETFETB.c#1 $
==============================================================================*/

#include "ETFETB.h"
#include "halqdss_etfetb.c"   //lite_inline
#include "qdss_tmc_config.h"

__inline void  ETFETBPreInit(void)
{
   HAL_qdss_etfetb_HalConfigInit(QDSS_ETFETB_BASE_PHYS);
}

__inline void  ETFETBInit(void)
{
   HAL_qdss_etfetb_AccessUnLock();

#ifdef QDSS_INIT_ETR
      HAL_qdss_etfetb_SetMode(QDSS_TMC_MODE_HWFIFO);
#else
      HAL_qdss_etfetb_SetMode(QDSS_TMC_MODE_CBUF);
#endif

   HAL_qdss_etfetb_SetFFCRETBDefault();
   HAL_qdss_etfetb_EnableTraceCapture();

   HAL_qdss_etfetb_AccessLock();
}



__inline void ETFETBFlush(void) 
{
   HAL_qdss_etfetb_AccessUnLock();
   HAL_qdss_etfetb_Flush();
   HAL_qdss_etfetb_FlushWait();
   HAL_qdss_etfetb_AccessLock();
}



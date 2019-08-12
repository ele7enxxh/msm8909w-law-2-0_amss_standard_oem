/*=============================================================================

FILE:         CTI.c

DESCRIPTION:  

================================================================================
              Copyright (c) 2011 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/

#include "halqdss_cti.c"
#include "qdss_cti_config.h"

#define HWE_ECT_CHANNEL_NUM    0

//Default CTI programming for
//routing hwevents to halt all processors
//

//WARNING: This would halt all processors on all selected hardware
//events. We have to be very certain that the selected
//set of hw events are dog bites and bus hang summary events only
//

void CTIInit(void)
{
   //Configure CTI 0
   HAL_qdss_cti_HalConfigInit(QDSS_CTI0_BASE_PHYS);
   HAL_qdss_cti_AccessUnLock();
   HAL_qdss_cti_Enable();
   //map hardware event on CTI 0 to channel 0
   HAL_qdss_cti_MapHWEventTrigger(HWE_ECT_CHANNEL_NUM); 
   HAL_qdss_cti_AccessLock();

   //The following halts all the processors on 
   //a trigger on channel 0
   {
      uint32 CoreCTIs[] = CPU_DSP_CTIS;

      int num_cores = sizeof(CoreCTIs) >> 2; //divide by 4
      int i = 0;

      for (i=0; i < num_cores; i++) {
            HAL_qdss_cti_HalConfigInit(CoreCTIs[i]);
            HAL_qdss_cti_AccessUnLock();
            HAL_qdss_cti_Enable();
            HAL_qdss_cti_MapDbgReq(HWE_ECT_CHANNEL_NUM); 
            HAL_qdss_cti_AccessLock();
      }
   }
}


/*=============================================================================

FILE:         CSR.c

DESCRIPTION:  QDSS CSR releated

================================================================================
              Copyright (c) 2011 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/rpm.bf/2.1.1/core/debugtrace/lite/src/CSR.c#1 $
==============================================================================*/

#include "halqdss_csr.c"
#include "qdss_csr_config.h"

//STM Burst Translator 
//Default programmed to enable Modem and BAMDMA
void STMBurstTransalatorInit()
{
   HAL_qdss_csr_SetAWIDs(QDSS_CSR_STMAWID_ASSIGNMENTS);

   //DMA port range is 128-255 for all masters
   HAL_qdss_csr_SetModemChannelOffset(0x1); 
   HAL_qdss_csr_SetBamDMAChannelOffset(0x1);  

   //Only a subset suitable for this target is enabled
   HAL_qdss_csr_EnableBurstMasters(QDSS_CSR_STM_BURST_TRANSALATORS);
}

void STMHWMuxInit(void)
{
   HAL_qdss_csr_mux_ctrl_0( HWEVENT_MUX_CTRL_0);
   HAL_qdss_csr_mux_ctrl_1( HWEVENT_MUX_CTRL_1);
   HAL_qdss_csr_mux_ctrl_2( HWEVENT_MUX_CTRL_2);
   HAL_qdss_csr_mux_ctrl_3( HWEVENT_MUX_CTRL_3);
   
}

void CSRInit()
{
   HAL_qdss_csr_HalConfigInit(QDSS_QDSSCSR_REG_BASE_PHYS);

   HAL_qdss_csr_AccessUnLock();

   HAL_qdss_csr_PwrReqIgnore();
   
   STMBurstTransalatorInit();

#ifdef QDSS_CTI_ENABLE   
      STMHWMuxInit();
#endif

   HAL_qdss_csr_AccessLock();

}

void USBFlush() 
{
   HAL_qdss_csr_AccessUnLock();
   HAL_qdss_csr_ManualEOT();
   HAL_qdss_csr_AccessLock();
}

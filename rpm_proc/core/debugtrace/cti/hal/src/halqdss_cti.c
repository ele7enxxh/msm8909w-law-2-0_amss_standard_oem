/*=============================================================================

FILE:         halqdss_cti.c

DESCRIPTION:  

================================================================================
                Copyright 2011 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/rpm.bf/2.1.1/core/debugtrace/cti/hal/src/halqdss_cti.c#1 $
==============================================================================*/
#include "halqdss_cti.h"
#include "qdss_cti_regs.h"

volatile uint32 QDSS_CTI_CSCTI_REG_BASE;


void HAL_qdss_cti_HalConfigInit(uint32 nBaseAddr)
{
   QDSS_CTI_CSCTI_REG_BASE = nBaseAddr;
}

void  HAL_qdss_cti_AccessUnLock(void)
{
   HWIO_OUT(QDSS_CTI_LAR,0xC5ACCE55);   
}

void  HAL_qdss_cti_AccessLock(void)
{ 
   HWIO_OUT(QDSS_CTI_LAR,(~0xC5ACCE55));   
}

void  HAL_qdss_cti_Enable(void)
{ 
   HWIO_OUTF(QDSS_CTICONTROL,GLBEN,1);
}

//Maps STM HW Event Trigger on CTI 0 TIGIN[6] to a channel
void HAL_qdss_cti_MapHWEventTrigger(uint8 channel) 
{
   HWIO_OUTF(QDSS_CTIINEN6,TRIGINEN, (1 << channel));
}

//DbgReq/EDBGRQ for CPUs comes in on TRIGOUT[0]
void HAL_qdss_cti_MapDbgReq(uint8 channel) 
{
   HWIO_OUTF(QDSS_CTIOUTEN0,TRIGOUTEN, (1 << channel));
}







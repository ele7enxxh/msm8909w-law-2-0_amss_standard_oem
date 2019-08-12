#ifndef HALQDSS_CTI_H
#define HALQDSS_CTI_H

/*=============================================================================

FILE:         halqdss_cti.h

DESCRIPTION:  

================================================================================
                Copyright 2011 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/rpm.bf/2.1.1/core/debugtrace/cti/hal/inc/halqdss_cti.h#1 $
==============================================================================*/
#include "HALcomdef.h"


void HAL_qdss_cti_HalConfigInit(uint32 nBaseAddr);
void  HAL_qdss_cti_AccessUnLock(void);
void  HAL_qdss_cti_AccessLock(void);
void  HAL_qdss_cti_Enable(void);
void HAL_qdss_cti_MapHWEventTrigger(uint8 channel); 
void HAL_qdss_cti_MapDbgReq(uint8 channel);


#endif //HALQDSS_CTI_H

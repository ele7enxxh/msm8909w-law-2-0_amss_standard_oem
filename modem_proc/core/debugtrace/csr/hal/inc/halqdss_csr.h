#ifndef HALQDSS_CSR_H
#define HALQDSS_CSR_H

/*=============================================================================

FILE:         halqdss_csr.h

DESCRIPTION:  

================================================================================
                Copyright 2011 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/core.mpss/3.5.c12.3/debugtrace/csr/hal/inc/halqdss_csr.h#1 $
==============================================================================*/
#include "HALcomdef.h"

#define USBBAMCTRL_BLKSIZE_256   0x0
#define USBBAMCTRL_BLKSIZE_512   0x1
#define USBBAMCTRL_BLKSIZE_1024  0x2
#define USBBAMCTRL_BLKSIZE_2048  0x3

void HAL_qdss_csr_HalConfigInit(uint32 nBaseAddr);
void HAL_qdss_csr_SetBlkSize(uint8 blksize); 
void HAL_qdss_csr_SetPeriodicFlushThreshold(uint16 nETRBAMCycles); 
void HAL_qdss_csr_EnablePeriodicFlush(void); 
void HAL_qdss_csr_EnableUSB(void); 
boolean  HAL_qdss_csr_IsUSBEnabled(void);
void HAL_qdss_csr_ManualEOT(void);



#endif //HALQDSS_CSR_H

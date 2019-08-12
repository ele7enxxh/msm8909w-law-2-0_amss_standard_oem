/*=============================================================================

FILE:         STMCfg.h

DESCRIPTION:  

================================================================================
                Copyright 2011 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/rpm.bf/2.1.1/core/debugtrace/lite/src/STMCfg.h#2 $
==============================================================================*/

#ifndef STMCFG_H
#define STMCFG_H

#include "HALcomdef.h"

void STMCfgPreInit(uint32 ts_counter_freq);
void STMCfgInit(void);
void STMSetTimestampFreq(uint32 dwFreq);
boolean STMAccessStatusNSNID(void);

#endif

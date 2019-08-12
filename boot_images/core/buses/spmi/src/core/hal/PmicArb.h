/**
 * @file:  PmicArb.h
 * @brief: 
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2015/09/01 00:30:35 $
 * $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/buses/spmi/src/core/hal/PmicArb.h#1 $
 * $Change: 8935858 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 10/1/13  Initial Version
 */
#ifndef PMICARB_H
#define	PMICARB_H

#include "SpmiTypes.h"
#include "SpmiUtils.h"
#include "SpmiHal.h"

typedef struct 
{
    uint32 irqStatus;
    uint32 geniStatus;
    uint32 geniCtrl;
} PmicArbHwStatus;

typedef void (*PmicArbIsrCallback)(uint8 channel, uint8 mask);

Spmi_Result PmicArb_Init(PmicArbIsrCallback isrCb);
Spmi_Result PmicArb_ConfigHW(SpmiCfg_ConfigType cfgType);
Spmi_Result PmicArb_FindChannel(uint8 slaveId, uint8 periph, uint8* channel);
Spmi_Result PmicArb_UpdateCache(void);
Spmi_Result PmicArb_InvalidateCache(void);
Spmi_Result PmicArb_AddCacheEntry(uint32 chan, uint8 sid, uint8 periph);
Spmi_Result PmicArb_SetInterruptEnabled(uint8 channel, boolean enabled);
Spmi_Result PmicArb_ExecuteCommand(PmicArbCmd cmd,
                                   Spmi_AccessPriority priority,
                                   uint8 slaveId,
                                   uint16 address,
                                   uint8* data,
                                   uint32 dataLen,
                                   uint32* bytesTransacted);

void PmicArb_GetHwStatus(PmicArbHwStatus* status);
boolean PmicArb_HasInterruptSupport(void);
uint32 PmicArb_GetMaxChannelSupport(void);

#endif

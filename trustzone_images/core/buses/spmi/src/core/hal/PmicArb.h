/**
 * @file:  PmicArb.h
 * @brief: 
 * 
 * Copyright (c) 2013-2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2016/06/17 14:31:11 $
 * $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/spmi/src/core/hal/PmicArb.h#1 $
 * $Change: 10727476 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 1/20/15  Multiple bus support
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

typedef void (*PmicArbIsrCallback)(uint8 bid, uint8 channel, uint8 mask);

Spmi_Result PmicArb_Init(PmicArbIsrCallback isrCb);
Spmi_Result PmicArb_ConfigHW(SpmiBusCfg_ConfigType cfgType);
Spmi_Result PmicArb_FindChannel(uint8 bid, uint8 slaveId, uint8 periph, uint8* channel);
Spmi_Result PmicArb_UpdateCache(void);
Spmi_Result PmicArb_AddCacheEntry(uint8 bid, uint32 chan, uint8 sid, uint8 periph);
Spmi_Result PmicArb_SetInterruptEnabled(uint8 bid, uint8 channel, boolean enabled);
Spmi_Result PmicArb_ExecuteCommand(PmicArbCmd cmd,
                                   Spmi_AccessPriority priority,
                                   uint8 bid,
                                   uint8 slaveId,
                                   uint16 address,
                                   uint8* data,
                                   uint32 dataLen,
                                   uint32* bytesTransacted);

void PmicArb_GetHwStatus(uint8 bid, PmicArbHwStatus* status);
boolean PmicArb_HasInterruptSupport(void);
uint32 PmicArb_GetMaxChannelSupport(void);
uint32 PmicArb_GetNumBusesSupported(void);

#endif

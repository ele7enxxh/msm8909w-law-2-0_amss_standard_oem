/**
 * @file:  SpmiMaster.h
 * @brief: Configures the Spmi Master Controller
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2016/06/17 14:31:11 $
 * $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/spmi/src/core/hal/badger/SpmiMaster.h#1 $
 * $Change: 10727476 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 10/1/13  Initial Version
 */
#ifndef SPMIMASTER_H
#define	SPMIMASTER_H

#include "SpmiCfg.h"
#include "SpmiUtils.h"

Spmi_Result SpmiMaster_ConfigHW(uint8 masterId, SpmiCfg_ConfigType cfgType);
void SpmiMaster_LoadFirmware(void);
void SpmiMaster_LoadRadixTree(SpmiCfg_ChannelCfg* entries, uint32 numEntries);

#endif


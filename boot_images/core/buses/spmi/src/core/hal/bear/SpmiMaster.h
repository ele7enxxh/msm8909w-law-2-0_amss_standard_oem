/**
 * @file:  SpmiMaster.h
 * @brief: Configures the Spmi Master Controller
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2015/09/01 00:30:35 $
 * $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/buses/spmi/src/core/hal/bear/SpmiMaster.h#1 $
 * $Change: 8935858 $ 
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


/**
 * @file:  SpmiMaster.h
 * @brief: Configures the Spmi Master Controller
 * 
 * Copyright (c) 2013-2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2016/06/17 14:31:11 $
 * $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/spmi/src/core/hal/bear/SpmiMaster.h#1 $
 * $Change: 10727476 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 10/1/13  Initial Version
 */
#ifndef SPMIMASTER_H
#define	SPMIMASTER_H

#include "SpmiBusCfg.h"
#include "SpmiUtils.h"

Spmi_Result SpmiMaster_ConfigHW(uint8 bid, uint8 masterId, SpmiBusCfg_ConfigType cfgType);
void SpmiMaster_LoadFirmware(uint8 bid);
void SpmiMaster_LoadRadixTree(uint8 bid, SpmiBusCfg_ChannelCfg* entries, uint32 numEntries);

#endif


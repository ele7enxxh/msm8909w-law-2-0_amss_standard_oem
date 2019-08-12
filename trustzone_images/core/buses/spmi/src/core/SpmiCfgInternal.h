/**
 * @file:  SpmiCfg.h
 * @brief: This module provides configuration options for the SPMI controller
 * 
 * Copyright (c) 2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2016/06/17 14:31:11 $
 * $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/spmi/src/core/SpmiCfgInternal.h#1 $
 * $Change: 10727476 $ 
 * 
 *                              Edit History
 * Date      Description
 * --------  -------------------------------------------------------------------
 * 1/20/15  Multiple bus support
 * 11/14/14  Initial Version
 */
#ifndef SPMICFGINTERNAL_H
#define	SPMICFGINTERNAL_H

#include "SpmiBusCfg.h"

//******************************************************************************
// Public API Functions
//******************************************************************************

boolean SpmiBusCfg_InDynamicChannelMode(uint8 busId);
Spmi_Result SpmiBusCfg_ConfigureChannel(uint8 busId, SpmiBusCfg_ChannelCfg* chanCfg, boolean autoAssignChan);

#endif

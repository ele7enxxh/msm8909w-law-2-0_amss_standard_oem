/**
 * @file:  SpmiCfg.h
 * @brief: This module provides configuration options for the SPMI controller
 * 
 * Copyright (c) 2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2015/09/01 00:30:35 $
 * $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/buses/spmi/src/core/SpmiCfgInternal.h#1 $
 * $Change: 8935858 $ 
 * 
 *                              Edit History
 * Date      Description
 * --------  -------------------------------------------------------------------
 * 11/14/14  Initial Version
 */
#ifndef SPMICFGINTERNAL_H
#define	SPMICFGINTERNAL_H

#include "SpmiCfg.h"

//******************************************************************************
// Public API Functions
//******************************************************************************

boolean SpmiCfg_InDynamicChannelMode(void);
Spmi_Result SpmiCfg_ConfigureChannel(SpmiCfg_ChannelCfg* chanCfg, boolean autoAssignChan);

#endif

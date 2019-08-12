/**
 * @file:  SpmiCfg.h
 * @brief: This module provides configuration options for the SPMI controller
 * 
 * Copyright (c) 2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2016/12/13 07:59:23 $
 * $Header: //components/rel/core.mpss/3.5.c12.3/buses/spmi/bear/src/core/SpmiCfgInternal.h#1 $
 * $Change: 11985146 $ 
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

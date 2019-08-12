/**
 * @file:  SpmiOsLogs.c
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2015/09/01 00:30:35 $
 * $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/buses/spmi/src/platform/os/sbl/SpmiOsLogs.c#1 $
 * $Change: 8935858 $
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 10/1/13  Initial Version
 */

#include "SpmiLogs.h"

//******************************************************************************
// Global Data
//******************************************************************************

char spmiLogBuf[SPMI_MAX_LOG_LEN];

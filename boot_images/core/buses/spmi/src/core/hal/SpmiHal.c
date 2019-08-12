/**
 * @file:  SpmiHal.c
 * 
 * Copyright (c) 2013-2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2015/09/01 00:30:35 $
 * $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/buses/spmi/src/core/hal/SpmiHal.c#1 $
 * $Change: 8935858 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 10/1/13  Initial Version
 */

#include "SpmiHal.h"
#include "SpmiOs.h"

//******************************************************************************
// Global Data
//******************************************************************************

uint8* pmicArbHwioBase = NULL;

//******************************************************************************
// Public API Functions
//******************************************************************************

Spmi_Result SpmiHal_Init()
{
    if(pmicArbHwioBase != NULL) {
        return SPMI_SUCCESS;
    }

    return SpmiOs_GetPmicArbBaseAddr( (void**)&pmicArbHwioBase );
}

uint8* SpmiHal_GetPmicArbBase()
{
    return pmicArbHwioBase;
}

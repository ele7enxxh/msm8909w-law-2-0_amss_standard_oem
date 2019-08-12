/**
 * @file:  SpmiHal.c
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2015/02/25 02:53:43 $
 * $Header: //components/rel/rpm.bf/2.1.1/core/buses/spmi/src/core/hal/SpmiHal.c#1 $
 * $Change: 7538753 $ 
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

void* pmicArbHwioBase = NULL;

//******************************************************************************
// Public API Functions
//******************************************************************************

Spmi_Result SpmiHal_Init()
{
    if(pmicArbHwioBase != NULL) {
        return SPMI_SUCCESS;
    }

    return SpmiOs_GetPmicArbBaseAddr( &pmicArbHwioBase );
}

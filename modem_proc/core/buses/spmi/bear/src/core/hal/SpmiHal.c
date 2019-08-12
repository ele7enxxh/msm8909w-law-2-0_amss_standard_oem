/**
 * @file:  SpmiHal.c
 * 
 * Copyright (c) 2013-2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2016/12/13 07:59:23 $
 * $Header: //components/rel/core.mpss/3.5.c12.3/buses/spmi/bear/src/core/hal/SpmiHal.c#1 $
 * $Change: 11985146 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 1/20/15  Multiple bus support
 * 10/1/13  Initial Version
 */

#include "SpmiHal.h"
#include "SpmiLogs.h"
#include "SpmiOs.h"

//******************************************************************************
// Global Data
//******************************************************************************

uint8* pmicArbBaseAddrs[SWIO_MAX_BUSES_SUPPORTED] = {NULL};

//******************************************************************************
// Public API Functions
//******************************************************************************

Spmi_Result SpmiHal_Init()
{
    uint32 i;
    Spmi_Result rslt;

    for(i = 0; i < SWIO_MAX_BUSES_SUPPORTED; i++)
    {
        if((rslt = SpmiOs_GetPmicArbBaseAddr( i, (void**)&pmicArbBaseAddrs[i] )) != SPMI_SUCCESS) {
            return rslt;
        }
        SPMI_LOG_INFO( "Bus #%d base addr: %p", i, pmicArbBaseAddrs[i] );
    }
    
    return SPMI_SUCCESS;
}

uint8* SpmiHal_GetBaseAddr(uint8 bid)
{
    return pmicArbBaseAddrs[bid];
}

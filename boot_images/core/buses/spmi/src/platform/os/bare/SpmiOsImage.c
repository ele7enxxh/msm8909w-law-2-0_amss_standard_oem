/**
 * @file:  SpmiOsEmmc.c
 * 
 * Copyright (c) 2013-2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2015/09/01 00:30:35 $
 * $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/buses/spmi/src/platform/os/bare/SpmiOsImage.c#1 $
 * $Change: 8935858 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 11/3/14  Automatic channel assignment
 * 10/1/13  Initial Version
 */

#include "SpmiOs.h"
#include "SpmiBare.h"
#include "busywait.h"

//******************************************************************************
// Public API Functions
//******************************************************************************

boolean SpmiOs_HandleTransactionError(Spmi_Result* rslt, PmicArbCmd cmd, uint8 slaveId, uint16 address, uint8 tries)
{
    return FALSE;
}

Spmi_Result SpmiOs_GetOwnerId(uint8* owner)
{
    *owner = SPMI_BARE_OWNER_NUMBER;
    return SPMI_SUCCESS;
}

Spmi_Result SpmiOs_GetPmicArbBaseAddr(void** addr)
{
    *addr = (void*) SPMI_BARE_PMIC_ARB_ADDRESS;
    return SPMI_SUCCESS;
}

void SpmiOs_Wait(uint32 us)
{
    busywait( us );
}

Spmi_Result SpmiOs_Malloc(uint32 size, void** buf)
{
    return SPMI_FAILURE_FUNCTIONALITY_NOT_SUPPORTED;
}

Spmi_Result SpmiOs_Free(void* buf)
{
    return SPMI_FAILURE_FUNCTIONALITY_NOT_SUPPORTED;
}

Spmi_Result SpmiOs_RegisterISR(SpmiOs_IsrPtr isr, void* ctx)
{
    return SPMI_FAILURE_FUNCTIONALITY_NOT_SUPPORTED;
}

uint64 SpmiOs_GetTimeTick(void)
{
    return 0;
}

SpmiOs_ClkStatus SpmiOs_GetSerialClkState(void)
{
    return SPMI_CLK_STATUS_NOT_SUPPORTED;
}

SpmiOs_ClkStatus SpmiOs_GetAhbClkState(void)
{
    return SPMI_CLK_STATUS_NOT_SUPPORTED;
}

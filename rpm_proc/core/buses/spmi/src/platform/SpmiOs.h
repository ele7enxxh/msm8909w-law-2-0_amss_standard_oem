/**
 * @file:  SpmiOs.h
 * @brief: Implements platform specific functionality
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2015/02/25 02:53:43 $
 * $Header: //components/rel/rpm.bf/2.1.1/core/buses/spmi/src/platform/SpmiOs.h#1 $
 * $Change: 7538753 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 10/1/13  Initial Version
 */
#ifndef SPMIOS_H
#define	SPMIOS_H

#include "SpmiTypes.h"

typedef enum 
{
    SPMI_CLK_OFF,
    SPMI_CLK_ON,
    SPMI_CLK_STATUS_NOT_SUPPORTED
} SpmiOs_ClkStatus;

typedef void* (*SpmiOs_IsrPtr)(void* ctx);

//******************************************************************************
// Required Functionality
//******************************************************************************

/**
 * Gets the owner number of this EE
 */
Spmi_Result SpmiOs_GetOwnerId(uint8* owner);

/**
 * Gets the base address of the PMIC Arbiter on this EE (PMIC_ARB_BASE)
 */
Spmi_Result SpmiOs_GetPmicArbBaseAddr(void** addr);

/**
 * Wait for 'us' microseconds
 */
void SpmiOs_Wait(uint32 us);

//******************************************************************************
// Optional / Debug Functionality
//******************************************************************************

/**
 * Sets the pointer 'buf' to a buffer of size 'size'
 * This function must zero out the new buffer
 */
Spmi_Result SpmiOs_Malloc(uint32 size, void** buf);

/**
 * Frees the buf previously allocated by SpmiOs_Malloc
 */
Spmi_Result SpmiOs_Free(void* buf);

/**
 * Registers the given ISR to service all SPMI interrupts
 */
Spmi_Result SpmiOs_RegisterISR(SpmiOs_IsrPtr isr, void* ctx);

/**
 * Return some relative time value
 */
uint64 SpmiOs_GetTimeTick(void);

/**
 * Return the state of the SPMI serial clock
 */
SpmiOs_ClkStatus SpmiOs_GetSerialClkState(void);

/**
 * Return the state of the AHB clock
 */
SpmiOs_ClkStatus SpmiOs_GetAhbClkState(void);

#endif

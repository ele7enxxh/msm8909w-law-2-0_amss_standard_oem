/**
 * @file:  SpmiOs.h
 * @brief: Implements platform specific functionality
 * 
 * Copyright (c) 2013-2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2016/06/17 14:31:11 $
 * $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/spmi/src/platform/SpmiOs.h#1 $
 * $Change: 10727476 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 07/10/15 Multiple bus support
 * 11/3/14  Automatic channel assignment
 * 10/1/13  Initial Version
 */
#ifndef SPMIOS_H
#define	SPMIOS_H

#include "SpmiTypes.h"
#include "SpmiUtils.h"

typedef enum 
{
    SPMI_CLK_OFF,
    SPMI_CLK_ON,
    SPMI_CLK_STATUS_NOT_SUPPORTED
} SpmiOs_ClkStatus;

typedef struct 
{
    uint32 numReserved;
    uint16* reserved;
} SpmiOs_ReservedChannels;

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
Spmi_Result SpmiOs_GetPmicArbBaseAddr(uint8 busId, void** addr);

/**
 * Wait for 'us' microseconds
 */
void SpmiOs_Wait(uint32 us);

/**
 * Get a list of channels to reserve.  Can return NULL to ignore.
 */
SpmiOs_ReservedChannels* SpmiOs_GetReservedChannels(uint8 busId);

/**
 * Allows custom handling of transaction errors.
 * Return True to retry the transaction. False to stop and 
 * return the error to the client.
 */
boolean SpmiOs_HandleTransactionError(Spmi_Result* rslt, PmicArbCmd cmd, 
                                      uint8 busId, uint8 slaveId, uint16 address, uint8 tries);

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
Spmi_Result SpmiOs_RegisterISR(SpmiOs_IsrPtr isr, uint8 busId);

/**
 * Return some relative time value
 */
uint64 SpmiOs_GetTimeTick(void);

/**
 * Return the state of the SPMI serial clock
 */
SpmiOs_ClkStatus SpmiOs_GetSerialClkState(uint8 busId);

/**
 * Return the state of the AHB clock
 */
SpmiOs_ClkStatus SpmiOs_GetAhbClkState(uint8 busId);

#endif

/**
 * @file:  SpmiOsImage.c
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2015/02/25 02:53:43 $
 * $Header: //components/rel/rpm.bf/2.1.1/core/buses/spmi/src/platform/os/rpm/SpmiOsImage.c#1 $
 * $Change: 7538753 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 10/1/13  Initial Version
 */

#include "SpmiOs.h"
#include "SpmiOsNhlos.h"
#include "SpmiLogs.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "Clock.h"
#include "timetick.h"
#include "cortex-m3.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

#define INTERRUPT_NUM_PROP "interrupt"

// Ids expected by the Clk driver
#define SERIAL_CLK_ID "gcc_spmi_ser_clk"
#define AHB_CLK_ID    "gcc_spmi_ahb_clk"

//******************************************************************************
// Global Data
//******************************************************************************

static boolean timeInit = FALSE;

static ClockIdType serClkId;
static boolean serClkIdInited = FALSE;

static ClockIdType ahbClkId;
static boolean ahbClkIdInited = FALSE;

static SpmiOs_IsrPtr spmiIsr = NULL;
static void* spmiCtx = NULL;
static uint32 interruptNum;

//******************************************************************************
// Local Helper Functions
//******************************************************************************

static void spmiIsrWrapper(void) __irq
{
    spmiIsr( spmiCtx );
    interrupt_clear( interruptNum );
}

//******************************************************************************
// Public API Functions
//******************************************************************************

Spmi_Result SpmiOs_Malloc(uint32 size, void** buf)
{
    if(DALSYS_Malloc( size, buf ) == DAL_SUCCESS) {
        DALSYS_memset( *buf, 0, size );
        return SPMI_SUCCESS;
    }    
    
    return SPMI_FAILURE_FUNCTIONALITY_NOT_SUPPORTED;
}

Spmi_Result SpmiOs_Free(void* buf)
{
    if(DALSYS_Free( buf ) == DAL_SUCCESS) {
        return SPMI_SUCCESS;
    }
    
    return SPMI_FAILURE_FUNCTIONALITY_NOT_SUPPORTED;
}

Spmi_Result SpmiOs_RegisterISR(SpmiOs_IsrPtr isr, void* ctx)
{
    DALSYSPropertyVar val;
    
    if(isr == NULL) {
        return SPMI_FAILURE_INVALID_PARAMETER;
    }
    
    if(SpmiOsNhlos_GetDalProp( INTERRUPT_NUM_PROP, &val ) == SPMI_SUCCESS) 
    {
        interruptNum = val.Val.dwVal;
        spmiIsr = isr;
        spmiCtx = ctx;
        
        interrupt_set_isr( interruptNum, spmiIsrWrapper );
        interrupt_configure( interruptNum, LEVEL_HIGH );
        interrupt_enable( interruptNum );
        
        return SPMI_SUCCESS;
    }
    
    return SPMI_FAILURE_FUNCTIONALITY_NOT_SUPPORTED;
}

SpmiOs_ClkStatus SpmiOs_GetSerialClkState()
{
    if(!serClkIdInited)
    {
        if(Clock_GetClockId(SERIAL_CLK_ID, &serClkId) == DAL_SUCCESS) {
            serClkIdInited = TRUE;
        }
    }
    
    if(serClkIdInited)
    {
        boolean isOn;
        if(Clock_IsClockOn(serClkId, &isOn) == DAL_SUCCESS) {
            return isOn ? SPMI_CLK_ON : SPMI_CLK_OFF;
        }
    }
    
    return SPMI_CLK_STATUS_NOT_SUPPORTED;
}

SpmiOs_ClkStatus SpmiOs_GetAhbClkState()
{
    if(!ahbClkIdInited)
    {
        if(Clock_GetClockId(AHB_CLK_ID, &ahbClkId) == DAL_SUCCESS) {
            ahbClkIdInited = TRUE;
        }
    }
    
    if(ahbClkIdInited)
    {
        boolean isOn;
        if(Clock_IsClockOn(ahbClkId, &isOn) == DAL_SUCCESS) {
            return isOn ? SPMI_CLK_ON : SPMI_CLK_OFF;
        }
    }
    
    return SPMI_CLK_STATUS_NOT_SUPPORTED;
}

uint64 SpmiOs_GetTimeTick() 
{
    if(!timeInit) {
        timetick_init();
        timeInit = TRUE;
    }
    
    return timetick_get64();
}

/**
 * @file:  SpmiOsImage.c
 * 
 * Copyright (c) 2013-2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2016/12/13 07:59:23 $
 * $Header: //components/rel/core.mpss/3.5.c12.3/buses/spmi/bear/src/platform/os/mpss/SpmiOsImage.c#1 $
 * $Change: 11985146 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 09/09/14  Hold Off Spmi From Attaching To ClkDriver Until Post Initialization
 * 10/1/13  Initial Version
 */

#include "SpmiOs.h"
#include "SpmiOsNhlos.h"
#include "SpmiLogs.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "DDIClock.h"
#include "DDITimetick.h"
#include "DDIInterruptController.h"

#include "stdint.h"

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

static boolean timeHdlInited = FALSE;
static DalDeviceHandle* timeDev = NULL;

static boolean clkHdlInited = FALSE;
static boolean clkHdAllowedToInit = FALSE;
static DalDeviceHandle* clkDev = NULL;
static ClockIdType ahbClkId;
static ClockIdType serClkId;

static DalDeviceHandle* interruptDev = NULL;
static uint32 interruptNum;

//******************************************************************************
// Local Helper Functions
//******************************************************************************

void SpmiOs_ClkStatusInit(void)
{
    // Allow attachment to DAL Clock Driver now that it has initialized.
    clkHdAllowedToInit = TRUE;
}

static void initClkDev(void)
{
    // Do not init if Clock Driver is not initialized yet.
    if (!clkHdAllowedToInit)
    {
        return;
    }
    
    if(DAL_ClockDeviceAttach( DALDEVICEID_CLOCK, &clkDev ) != DAL_SUCCESS) {
        clkDev = NULL;
    }
    else 
    {
        if(DalClock_GetClockId( clkDev, SERIAL_CLK_ID, &serClkId ) != DAL_SUCCESS) {
            SPMI_LOG_WARNING( "Could not get handle to serial clock" );
        }
        
        if(DalClock_GetClockId( clkDev, AHB_CLK_ID, &ahbClkId ) != DAL_SUCCESS) {
            SPMI_LOG_WARNING( "Could not get handle to ahb clock" );
        }
    }
    
    clkHdlInited = TRUE;
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

Spmi_Result SpmiOs_RegisterISR(SpmiOs_IsrPtr isr, uint8 busId)
{
    DALResult rslt;
    
    if(interruptDev == NULL)
    {
        DALSYSPropertyVar val;
        
        if(SpmiOsNhlos_GetDalProp( INTERRUPT_NUM_PROP, &val ) == SPMI_SUCCESS) 
        {
            interruptNum = val.Val.dwVal;
            
            if((rslt = DAL_DeviceAttach( DALDEVICEID_INTERRUPTCONTROLLER, &interruptDev )) != DAL_SUCCESS)
            {
                SPMI_LOG_ERROR( "Could not get handle to interrupt controller.  Result: %d", rslt );
                return SPMI_FAILURE_FUNCTIONALITY_NOT_SUPPORTED;
            }
        }
    }
    
    if(interruptDev != NULL)
    {
        if((rslt = DalInterruptController_RegisterISR( interruptDev,
                                                       interruptNum,
                                                       isr,
                                                       (void *)((uintptr_t)busId),
                                                       DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER )) == DAL_SUCCESS) 
        {
            return SPMI_SUCCESS;            
        }
        else 
        {
            SPMI_LOG_ERROR( "Could not register for interrupt number %d.  Result: %d",
                            interruptNum,
                            rslt );
        }
    }
        
    return SPMI_FAILURE_FUNCTIONALITY_NOT_SUPPORTED;
}

SpmiOs_ClkStatus SpmiOs_GetSerialClkState(uint8 busId)
{
    if(!clkHdlInited) {
        initClkDev();
    }
    
    if(clkDev != NULL) {
        return DalClock_IsClockOn( clkDev, serClkId ) ? SPMI_CLK_ON : SPMI_CLK_OFF;
    }
    
    return SPMI_CLK_STATUS_NOT_SUPPORTED;
}

SpmiOs_ClkStatus SpmiOs_GetAhbClkState(uint8 busId)
{
    if(!clkHdlInited) {
        initClkDev();
    }
    
    if(clkDev != NULL) {
        return DalClock_IsClockOn( clkDev, ahbClkId ) ? SPMI_CLK_ON : SPMI_CLK_OFF;
    }
    
    return SPMI_CLK_STATUS_NOT_SUPPORTED;
}

boolean SpmiOs_HandleTransactionError(Spmi_Result* rslt, PmicArbCmd cmd, uint8 busId, 
                                      uint8 slaveId, uint16 address, uint8 tries)
{
    return FALSE;
}

uint64 SpmiOs_GetTimeTick() 
{
    uint64 tick = 0;
    
    if(!timeHdlInited) 
    {
        if(DalTimetick_Attach( "SystemTimer", &timeDev ) != DAL_SUCCESS) {
            timeDev = NULL;
        }
        else if(DalTimetick_InitTimetick64( timeDev ) != DAL_SUCCESS)
        {
            DAL_DeviceDetach( timeDev );
            timeDev = NULL;
        }
        timeHdlInited = TRUE;
    }
    
    if(timeDev != NULL) {
        DalTimetick_GetTimetick64( timeDev, &tick );
    }
    
    return tick;
}

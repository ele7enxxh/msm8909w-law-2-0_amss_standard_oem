/**
 * @file:  SpmiOsImage.c
 * 
 * Copyright (c) 2013-2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2015/09/01 00:30:35 $
 * $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/buses/spmi/src/platform/os/sbl/SpmiOsImage.c#1 $
 * $Change: 8935858 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 11/3/14  Automatic channel assignment
 * 10/1/13  Initial Version
 */

#include "SpmiOs.h"
#include "SpmiOsNhlos.h"
#include "SpmiLogs.h"
#include "SpmiCfgInternal.h"
#include "PmicArb.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "DDIClock.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

// Ids expected by the Clk driver
#define SERIAL_CLK_ID "gcc_spmi_ser_clk"
#define AHB_CLK_ID    "gcc_spmi_ahb_clk"

//******************************************************************************
// Global Data
//******************************************************************************

static boolean clkHdlInited = FALSE;
static DalDeviceHandle* clkDev = NULL;
static ClockIdType ahbClkId;
static ClockIdType serClkId;

//******************************************************************************
// Local Helper Functions
//******************************************************************************

static void initClkDev(void)
{
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
// Implemented API Functions
//******************************************************************************

boolean SpmiOs_HandleTransactionError(Spmi_Result* rslt, PmicArbCmd cmd, uint8 slaveId, uint16 address, uint8 tries)
{
    boolean rtn = FALSE;

    if(SpmiCfg_InDynamicChannelMode() && (*rslt == SPMI_FAILURE_CHANNEL_NOT_FOUND))
    {
        Spmi_Result cfgRslt;
        SpmiCfg_ChannelCfg chanCfg;
        uint8 periph = REG_ADDR_TO_PERIPH_ID( address );
        
        chanCfg.slaveId = slaveId;
        chanCfg.periphId = periph;

        if((cfgRslt = SpmiCfg_ConfigureChannel( &chanCfg, TRUE )) != SPMI_SUCCESS) {
            *rslt = cfgRslt;
        }
        else if((cfgRslt = PmicArb_AddCacheEntry( chanCfg.channel, slaveId, periph )) != SPMI_SUCCESS) {
            *rslt = cfgRslt;
        }
        else {
            rtn = TRUE;
        }
    }

    SPMI_LOG_VERBOSE( "OS Error %d (%s)", *rslt, rtn ? "retry" : "done" );
    return rtn;
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

SpmiOs_ClkStatus SpmiOs_GetSerialClkState()
{
    if(!clkHdlInited) {
        initClkDev();
    }
    
    if(clkDev != NULL) {
        return DalClock_IsClockOn( clkDev, serClkId ) ? SPMI_CLK_ON : SPMI_CLK_OFF;
    }
    
    return SPMI_CLK_STATUS_NOT_SUPPORTED;
}

SpmiOs_ClkStatus SpmiOs_GetAhbClkState()
{
    if(!clkHdlInited) {
        initClkDev();
    }
    
    if(clkDev != NULL) {
        return DalClock_IsClockOn( clkDev, ahbClkId ) ? SPMI_CLK_ON : SPMI_CLK_OFF;
    }
    
    return SPMI_CLK_STATUS_NOT_SUPPORTED;
}

uint64 SpmiOs_GetTimeTick() 
{
    return 0;
}

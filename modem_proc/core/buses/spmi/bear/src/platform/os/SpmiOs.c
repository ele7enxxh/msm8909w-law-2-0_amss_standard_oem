/**
 * @file:  SpmiOs.c
 * 
 * Copyright (c) 2013-2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2016/12/13 07:59:23 $
 * $Header: //components/rel/core.mpss/3.5.c12.3/buses/spmi/bear/src/platform/os/SpmiOs.c#1 $
 * $Change: 11985146 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 1/20/15  Multiple bus support
 * 10/1/13  Initial Version
 */

#include "SpmiOs.h"
#include "SpmiOsNhlos.h"
#include "SpmiLogs.h"
#include "SpmiSwio.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "DDISpmi.h"
#include "DDIHWIO.h"
#include "DDIInterruptController.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

// Hard coded pmic arb base address (property)
#define PMIC_ARB_BASE_ADDR_PROP "pmic_arb_base_addr"

// Debug channel (property)
#define DEBUG_CHAN_PROP "useDebugChan"

// This owner number for the EE this driver is running on (property)
#define OWNER_PROP "owner"

// Name expected by the HWIO driver for the pmic arb base address
#define PMIC_ARB_BASE_ID "PMIC_ARB"

//******************************************************************************
// Global Data
//******************************************************************************

static boolean propHdlInited = FALSE;
static DALSYS_PROPERTY_HANDLE_DECLARE(dalPropDev);
static uint16 dbgChannels[] = {SWIO_MAX_CHANNELS_SUPPORTED - 1};

//******************************************************************************
// Private API Helper Functions
//******************************************************************************

Spmi_Result SpmiOsNhlos_GetDalProp(char* propId, DALSYSPropertyVar* val)
{
    if(!propHdlInited)
    {
        if(DALSYS_GetDALPropertyHandleStr( DALSPMI_DEVICE_ID, dalPropDev ) != DAL_SUCCESS ) {
            SPMI_LOG_ERROR( "Unable to get SPMI property handle" );
            return SPMI_FAILURE_INIT_FAILED;
        }
        
        propHdlInited = TRUE;
    }
    
    return DALSYS_GetPropertyValue( dalPropDev, propId, 0, val ) == DAL_SUCCESS ? 
           SPMI_SUCCESS : SPMI_FAILURE_FUNCTIONALITY_NOT_SUPPORTED;
}

//******************************************************************************
// Public API Functions
//******************************************************************************

SpmiOs_ReservedChannels* SpmiOs_GetReservedChannels(uint8 busId)
{
    DALSYSPropertyVar val;
    static SpmiOs_ReservedChannels debugChannels = {
        .numReserved = sizeof(dbgChannels) / sizeof(dbgChannels[0]),
        .reserved = dbgChannels
    };
    
    if((SpmiOsNhlos_GetDalProp( DEBUG_CHAN_PROP, &val ) == SPMI_SUCCESS) && val.Val.dwVal) {
        return &debugChannels;
    }
    
    return NULL;
}

Spmi_Result SpmiOs_GetOwnerId(uint8* owner)
{
    DALSYSPropertyVar val;
    Spmi_Result rslt = SpmiOsNhlos_GetDalProp( OWNER_PROP, &val );

    if(rslt == SPMI_SUCCESS) {
        *owner = val.Val.dwVal;
    } 
    
    return rslt;
}

void SpmiOs_Wait(uint32 us)
{
    DALSYS_BusyWait( us );
}

Spmi_Result SpmiOs_GetPmicArbBaseAddr(uint8 busId, void** addr)
{
    Spmi_Result rslt;
    DALSYSPropertyVar dalProp;
    DalDeviceHandle* handle;

    // Try to use the HWIO driver to get the base address
    if(DAL_HWIODeviceAttach( DALDEVICEID_HWIO, &handle ) == DAL_SUCCESS && handle != NULL)
    {
        if(DalDevice_Open( handle, DAL_OPEN_SHARED ) != DAL_SUCCESS) {
            rslt = SPMI_FAILURE_INIT_FAILED;
        }
        else if(DalHWIO_MapRegion( handle, PMIC_ARB_BASE_ID, (uint8**) addr ) != DAL_SUCCESS) {
            rslt = SPMI_FAILURE_INIT_FAILED;
        } 
        else if(DalDevice_Close( handle ) != DAL_SUCCESS) {
            rslt = SPMI_FAILURE_INIT_FAILED;
        }
        else {
            rslt = SPMI_SUCCESS;
        }
    }
    else 
    {
        // Fall back to reading a hard coded value from the prop file
        if((rslt = SpmiOsNhlos_GetDalProp( PMIC_ARB_BASE_ADDR_PROP, &dalProp )) == SPMI_SUCCESS) {
            *addr = (void*)dalProp.Val.dwVal;
        }
    }
    
    return rslt;
}

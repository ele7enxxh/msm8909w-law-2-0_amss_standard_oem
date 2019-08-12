/**
 * @file:  SpmiOs.c
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2015/09/01 00:30:35 $
 * $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/buses/spmi/src/platform/os/SpmiOs.c#1 $
 * $Change: 8935858 $ 
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
#include "DDISpmi.h"
#include "DDIHWIO.h"
#include "DDIInterruptController.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

// Hard coded pmic arb base address (property)
#define PMIC_ARB_BASE_ADDR_PROP "pmic_arb_base_addr"

// This owner number for the EE this driver is running on (property)
#define OWNER_PROP "owner"

// Name expected by the HWIO driver for the pmic arb base address
#define PMIC_ARB_BASE_ID "PMIC_ARB"

//******************************************************************************
// Global Data
//******************************************************************************

static boolean propHdlInited = FALSE;
static DALSYS_PROPERTY_HANDLE_DECLARE(dalPropDev);

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

Spmi_Result SpmiOs_GetPmicArbBaseAddr(void** addr)
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

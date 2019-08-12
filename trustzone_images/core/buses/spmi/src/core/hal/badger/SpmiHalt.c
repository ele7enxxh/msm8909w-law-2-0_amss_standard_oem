/**
 * @file:  SpmiHalt.c
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2016/06/17 14:31:11 $
 * $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/spmi/src/core/hal/badger/SpmiHalt.c#1 $
 * $Change: 10727476 $
 */

#include "SpmiHalt.h"
#include "busywait.h"

//******************************************************************************
// Constants
//******************************************************************************

// This base should work for all Badger chips
#define PMIC_ARB_BASE 0xFC4CF000
#include "SpmiHal.h"
#include "SpmiUtils.h"

//******************************************************************************
// Public API Functions
//******************************************************************************

Spmi_Result SpmiHalt_DisableBus(void)
{
    uint32 timeout = 0;
    
    SPMI_HWIO_OUT_SET( HWIO_PMIC_ARB_SPMI_GENI_CTRL_ADDR, HWIO_PMIC_ARB_SPMI_GENI_CTRL_M_GENI_DISABLE_BMSK );
 
    while(SPMI_HWIO_IN_FIELD( HWIO_PMIC_ARB_SPMI_GENI_STATUS_ADDR, HWIO_PMIC_ARB_SPMI_GENI_STATUS_M_GENI_CMD_ACTIVE ))
    {
        if(timeout++ == SPMI_TIMEOUT_USEC) {
            return SPMI_FAILURE_TRANSACTION_TIMEOUT;
        }
 
        busywait( 1 );
    }
 
    return SPMI_SUCCESS;
}

/**
 * @file:  SpmiIntrCtrl.c
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2016/06/17 14:31:11 $
 * $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/spmi/src/core/hal/badger/SpmiIntrCtrl.c#1 $
 * $Change: 10727476 $
 */

#include "SpmiIntrCtrl.h"
#include "PmicArb.h"

//******************************************************************************
// Public API Functions
//******************************************************************************

Spmi_Result SpmiIntrCtlr_SetInterruptEnabled(uint8 slaveId, uint8 periph, boolean enabled)
{
    uint8 channel;
    Spmi_Result rslt;
    
    if((rslt = PmicArb_FindChannel( slaveId, periph, &channel )) != SPMI_SUCCESS) {
        return rslt;
    }
    
    SPMI_HWIO_OUT_FIELD( HWIO_PMIC_ARB_SPMI_PIC_ACC_ENABLEn_ADDR( channel ), HWIO_PMIC_ARB_SPMI_PIC_ACC_ENABLEn_INT_ACC_ENABLE, enabled );
    
    return SPMI_SUCCESS;
}

Spmi_Result SpmiIntrCtlr_IsInterruptEnabled(uint8 slaveId, uint8 periph, boolean* enabled)
{
    uint8 channel;
    Spmi_Result rslt;
    
    if((rslt = PmicArb_FindChannel( slaveId, periph, &channel )) != SPMI_SUCCESS) {
        return rslt;
    }
    
    *enabled = SPMI_HWIO_IN_FIELD( HWIO_PMIC_ARB_SPMI_PIC_ACC_ENABLEn_ADDR( channel ), HWIO_PMIC_ARB_SPMI_PIC_ACC_ENABLEn_INT_ACC_ENABLE );
    
    return SPMI_SUCCESS;
}

Spmi_Result SpmiIntrCtlr_GetAccumulatedIntrStatus(uint8 owner, const uint32** accumStatus, uint32* accStatusCount)
{
    static const uint32 count = HWIO_PMIC_ARB_SPMI_PIC_OWNERm_ACC_STATUSn_MAXn + 1;
    static uint32 status[count];
    
    for(uint32 i = 0; i < count; i++) {
        status[i] = SPMI_HWIO_IN( HWIO_PMIC_ARB_SPMI_PIC_OWNERm_ACC_STATUSn_ADDR( owner, i ) );
    }
    
    *accumStatus = status;
    *accStatusCount = count;
    
    return SPMI_SUCCESS;
}

Spmi_Result SpmiIntrCtlr_ExtendedInterruptStatus(uint8 idx, uint32* statusMask)
{
    if(idx > HWIO_PMIC_ARB_SPMI_PIC_IRQ_STATUSn_MAXn) {
        return SPMI_FAILURE_INVALID_CHAN_NUM;
    }
    
    *statusMask = SPMI_HWIO_IN_FIELD( HWIO_PMIC_ARB_SPMI_PIC_IRQ_STATUSn_ADDR( idx ), HWIO_PMIC_ARB_SPMI_PIC_IRQ_STATUSn_INT_STATUS );
    
    return SPMI_SUCCESS;
}

Spmi_Result SpmiIntrCtlr_ExtendedInterruptClear(uint8 idx, uint32 statusMask)
{
    if(idx > HWIO_PMIC_ARB_SPMI_PIC_IRQ_CLEARn_MAXn) {
        return SPMI_FAILURE_INVALID_CHAN_NUM;
    }
    
    SPMI_HWIO_OUT_FIELD( HWIO_PMIC_ARB_SPMI_PIC_IRQ_CLEARn_ADDR( idx ), HWIO_PMIC_ARB_SPMI_PIC_IRQ_CLEARn_INT_CLEAR, statusMask );
    
    return SPMI_SUCCESS;
}

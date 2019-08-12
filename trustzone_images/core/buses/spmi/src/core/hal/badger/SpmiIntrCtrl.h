/**
 * @file:  SpmiIntrCtrl.h
 * @brief: This module provides raw access to SPMI interrupts
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2016/06/17 14:31:11 $
 * $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/spmi/src/core/hal/badger/SpmiIntrCtrl.h#1 $
 * $Change: 10727476 $
 */
#ifndef SPMIINTRCTRL_H
#define	SPMIINTRCTRL_H

#include "SpmiTypes.h"

/**
 * @brief Enables or disables an interrupt
 * 
 * This function enables or disables an interrupt for the given peripheral
 * 
 * @param[in] slaveId  Slave Id of the device
 * @param[in] periph   Peripheral address
 * @param[in] enabled  True to enable the interrupt, False otherwise.
 * 
 * @return SPMI_SUCCESS on success, error code on error
 * 
 * @see SpmiIntrCtlr_InterruptDisable()
 */
Spmi_Result SpmiIntrCtlr_SetInterruptEnabled(uint8 slaveId, uint8 periph, boolean enabled);


/**
 * @brief Get the accumulated interrupt status
 * 
 * This function return the interrupt status for all peripherals
 * 
 * @param[in] owner        - The owner number of this EE
 * @param[out] accumStatus - Pointer to an array which will hold the the 
 *                           accumulated IntrStatus. Each bit set in this 
 *                           parameter will indicates that the corresponding
 *                           peripheral number interrupt is set.  Interrupt 
 *                           bit will be set if the interrupt is asserted 
 *                           and if the peripheral is owned by this processor.
 * 
 * @param[out] accumStatusCount - Will be set to the number of words returned
 * 
 * @return SPMI_SUCCESS on success, error code on error
 */
Spmi_Result SpmiIntrCtlr_GetAccumulatedIntrStatus(uint8 owner, const uint32** accumStatus, uint32* accStatusCount);


/**
 * @brief Get the extended interrupt status
 * 
 * This function returns the extended interrupt status of the index.
 * The index is obtained by querying the accumulated interrupt status.
 * 
 * @param[in] idx The index of the peripheral who's accumulated interrupt
 *                status bit is set.
 * 
 * @param[out] statusMask The status of the extended interrupts. Each set 
 *                        bit in this parameter represents one of the peripherals
 *                        interrupts.
 * 
 * @return SPMI_SUCCESS on success, error code on error
 * 
 * @see SpmiIntrCtlr_GetAccumulatedIntrStatus
 */
Spmi_Result SpmiIntrCtlr_ExtendedInterruptStatus(uint8 idx, uint32* statusMask);


/**
 * @brief Clear the extended interrupt status
 * 
 * This function clears the extended interrupt status associated
 * with the intrId
 * 
 * @param[in] idx The index of the peripheral who's accumulated interrupt
 *                status bit is set.
 * 
 * @param[in] statusMask This is a mask of bits which represent the interrupts
 *                       to be cleared.
 * 
 * @return SPMI_SUCCESS on success, error code on error
 * 
 * @see SpmiIntrCtlr_GetAccumulatedIntrStatus,
 *      SpmiIntrCtlr_ExtendedInterruptStatus
 */
Spmi_Result SpmiIntrCtlr_ExtendedInterruptClear(uint8 idx, uint32 statusMask);


#endif

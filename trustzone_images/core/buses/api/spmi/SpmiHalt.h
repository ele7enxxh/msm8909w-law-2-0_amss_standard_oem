/**
 * @file: SpmiHalt.h
 * @brief: A standalone API for disabling the SPMI bus
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2016/06/17 14:31:11 $
 * $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/api/spmi/SpmiHalt.h#1 $
 * $Change: 10727476 $
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 07/10/15  Multiple bus support
 * 10/1/13  Initial Version
 */
#ifndef SPMIHALT_H
#define	SPMIHALT_H

#include "SpmiTypes.h"

/**
 * @brief Gracefully halts the SPMI Bus
 * 
 * This function disables all SPMI controllers so no future bus
 * requests are processed.  If this function returns successfully,
 * all buses will have completed any in progress transaction and will be
 * disabled.
 * 
 * @return  SPMI_SUCCESS on success, error code on failure
*/
Spmi_Result SpmiHalt_DisableAllBuses(void);

/**
 * @brief Gracefully halts the SPMI Bus
 * 
 * This function disables the SPMI controller so no future bus
 * requests are processed.  If this function returns successfully,
 * the bus will have completed any in progress transaction and will be
 * disabled.
 * 
 * @param[in] busId  The bus to disable
 * 
 * @return  SPMI_SUCCESS on success, error code on failure
*/
Spmi_Result SpmiHalt_DisableBus(uint8 busId);

#endif

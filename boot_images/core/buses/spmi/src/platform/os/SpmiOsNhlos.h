/**
 * @file:  SpmiOsNhlos.h
 * @brief: Implements platform specific functionality
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2015/09/01 00:30:35 $
 * $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/buses/spmi/src/platform/os/SpmiOsNhlos.h#1 $
 * $Change: 8935858 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 10/1/13  Initial Version
 */
#ifndef SPMIOSNHLOS_H
#define	SPMIOSNHLOS_H

#include "SpmiTypes.h"
#include "DALSysTypes.h"

Spmi_Result SpmiOsNhlos_GetDalProp(char* propId, DALSYSPropertyVar* val);

#endif

/**
 * @file:  SpmiOsNhlos.h
 * @brief: Implements platform specific functionality
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2015/02/25 02:53:43 $
 * $Header: //components/rel/rpm.bf/2.1.1/core/buses/spmi/src/platform/os/SpmiOsNhlos.h#1 $
 * $Change: 7538753 $ 
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

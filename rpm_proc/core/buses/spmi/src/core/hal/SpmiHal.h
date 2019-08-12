/**
 * @file:  SpmiHal.h
 * @brief: 
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2015/02/25 02:53:43 $
 * $Header: //components/rel/rpm.bf/2.1.1/core/buses/spmi/src/core/hal/SpmiHal.h#1 $
 * $Change: 7538753 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 10/1/13  Initial Version
 */
#ifndef SPMIHAL_H
#define	SPMIHAL_H

#ifndef PMIC_ARB_BASE
    extern void* pmicArbHwioBase;
    #define PMIC_ARB_BASE ((unsigned long)pmicArbHwioBase)
#endif

#include "HALhwio.h"
#include "SpmiTypes.h"
#include "SpmiCoreHwio.h"
#include "PmicArbCoreHwio.h"

#define SPMI_HWIO_IN(addr)        __inpdw(addr)
#define SPMI_HWIO_OUT(addr, val)  __outpdw(addr, val)

#define SPMI_HWIO_OUT_SET(addr, mask)    SPMI_HWIO_OUT(addr, ((SPMI_HWIO_IN(addr)) | (mask)))
#define SPMI_HWIO_OUT_CLEAR(addr, mask)  SPMI_HWIO_OUT(addr, ((SPMI_HWIO_IN(addr)) & (~(mask))))

#define SPMI_HWIO_FIELD_VALUE(value, field)  (((value) & field##_BMSK) >> field##_SHFT)

#define SPMI_HWIO_IN_MASK(addr, mask)         (SPMI_HWIO_IN(addr) & (mask))
#define SPMI_HWIO_IN_FIELD(addr, field)       (SPMI_HWIO_IN_MASK( addr, field##_BMSK ) >> field##_SHFT)
#define SPMI_HWIO_OUT_MASK(addr, mask, val)    SPMI_HWIO_OUT(addr, ((SPMI_HWIO_IN(addr) & (~(mask))) | ((val) & (mask))))
#define SPMI_HWIO_OUT_FIELD(addr, field, val)  SPMI_HWIO_OUT_MASK( addr, field##_BMSK, (val) << field##_SHFT )

Spmi_Result SpmiHal_Init(void);

#endif

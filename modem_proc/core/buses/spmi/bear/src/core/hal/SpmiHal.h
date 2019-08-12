/**
 * @file:  SpmiHal.h
 * @brief: 
 * 
 * Copyright (c) 2013-2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2016/12/13 07:59:23 $
 * $Header: //components/rel/core.mpss/3.5.c12.3/buses/spmi/bear/src/core/hal/SpmiHal.h#1 $
 * $Change: 11985146 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 1/20/15  Multiple bus support
 * 10/1/13  Initial Version
 */
#ifndef SPMIHAL_H
#define	SPMIHAL_H

#include "HALhwio.h"
#include "SpmiTypes.h"

extern uint8* pmicArbBaseAddrs[];
#define PMIC_ARB_BASE_ARRAY pmicArbBaseAddrs

#include "SpmiSwio.h"
    
#define SPMI_HWIO_IN(addr)        __inpdw(addr)
#define SPMI_HWIO_OUT(addr, val)  __outpdw(addr, val)

#define SPMI_HWIO_OUT_SET(addr, mask)    SPMI_HWIO_OUT(addr, ((SPMI_HWIO_IN(addr)) | (mask)))
#define SPMI_HWIO_OUT_CLEAR(addr, mask)  SPMI_HWIO_OUT(addr, ((SPMI_HWIO_IN(addr)) & (~(mask))))

#define SPMI_HWIO_IN_MASK(addr, mask)         (SPMI_HWIO_IN(addr) & (mask))
#define SPMI_HWIO_OUT_MASK(addr, mask, val)    SPMI_HWIO_OUT(addr, ((SPMI_HWIO_IN(addr) & (~(mask))) | ((val) & (mask))))

#define SPMI_HWIO_GET_FIELD_VALUE(value, field)  (((value) & field##_BMSK) >> field##_SHFT)
#define SPMI_HWIO_SET_FIELD_VALUE(value, field)  (((value) << field##_SHFT) & field##_BMSK)
#define SPMI_HWIO_IN_FIELD(addr, field)       (SPMI_HWIO_IN_MASK( addr, field##_BMSK ) >> field##_SHFT)
#define SPMI_HWIO_OUT_FIELD(addr, field, val)  SPMI_HWIO_OUT_MASK( addr, field##_BMSK, (val) << field##_SHFT )

#define SPMI_SWIO_GET_FIELD_VALUE(bid, value, field)  (((value) & field##_BMSK(bid)) >> field##_SHFT(bid))
#define SPMI_SWIO_SET_FIELD_VALUE(bid, value, field)  (((value) << field##_SHFT(bid)) & field##_BMSK(bid))
#define SPMI_SWIO_IN_FIELD(bid, addr, field)  (SPMI_HWIO_IN_MASK( addr, field##_BMSK(bid) ) >> field##_SHFT(bid))
#define SPMI_SWIO_OUT_FIELD(bid, addr, field, val)  SPMI_HWIO_OUT_MASK( addr, field##_BMSK(bid), (val) << field##_SHFT(bid) )

Spmi_Result SpmiHal_Init(void);
uint8* SpmiHal_GetBaseAddr(uint8 bid);

#endif

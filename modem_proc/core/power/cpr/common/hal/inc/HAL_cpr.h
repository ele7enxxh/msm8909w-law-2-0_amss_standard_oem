/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/


#ifndef HAL_CPR_H
#define HAL_CPR_H

#include "HALhwio.h"
#include "comdef.h"

#if defined(WINSIM) | defined(MPSS_CPR_BUILD)
#define uintnt uint32
#endif

#define CPR_HWIO_IN(addr)        __inpdw(addr)
#define CPR_HWIO_OUT(addr, val)  __outpdw(addr, val)

#define CPR_HWIO_OUT_SET(addr, mask)    CPR_HWIO_OUT(addr, ((CPR_HWIO_IN(addr)) | (mask)))
#define CPR_HWIO_OUT_CLEAR(addr, mask)  CPR_HWIO_OUT(addr, ((CPR_HWIO_IN(addr)) & (~(mask))))

#define CPR_HWIO_IN_MASK(addr, mask)         (CPR_HWIO_IN(addr) & (mask))
#define CPR_HWIO_OUT_MASK(addr, mask, val)    CPR_HWIO_OUT(addr, ((CPR_HWIO_IN(addr) & (~(mask))) | ((val) & (mask))))

#define CPR_HWIO_GET_FIELD_VALUE(value, field)  (((value) & field##_BMSK) >> field##_SHFT)
#define CPR_HWIO_SET_FIELD_VALUE(value, field)  (((value) << field##_SHFT) & field##_BMSK)
#define CPR_HWIO_IN_FIELD(addr, field)       (CPR_HWIO_IN_MASK( addr, field##_BMSK ) >> field##_SHFT)
#define CPR_HWIO_OUT_FIELD(addr, field, val)  CPR_HWIO_OUT_MASK( addr, field##_BMSK, (val) << field##_SHFT )


typedef struct HAL_cpr_rail_s const * HAL_cpr_rail_t;

typedef struct HAL_cpr_rail_s
{
  uintnt hw_base_address;
  uint32 thread;
} HAL_cpr_rail_s;


typedef struct HAL_cpr_dump_s* HAL_cpr_dump_t ;

HAL_cpr_dump_t HAL_cpr_alloc_cpr_dump_buffer(void);

void HAL_cpr_save_hw_state(HAL_cpr_rail_t rail, HAL_cpr_dump_t core_dump);

#endif //HAL_CPR_H

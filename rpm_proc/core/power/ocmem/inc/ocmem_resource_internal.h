/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef OCMEM_RESOURCE_INTERNAL_H
#define OCMEM_RESOURCE_INTERNAL_H

#define OCMEM_REGIONS 3
#define OCMEM_BANKS_PER_REGION 4


typedef struct {
    unsigned ctl_addr;
    unsigned msk;
    unsigned shft;
    unsigned rmsk;
} ocmem_hwio_param;


#endif //OCMEM_RESOURCE_INTERNAL_H

#ifndef _TZMEMARRAY_H_
#define _TZMEMARRAY_H_
/*===========================================================================
  Copyright (c) 2011 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsapps/tzdrm/tzcommon/common/shared/inc/tzmemarray.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/11/14    ss     Declared a macro for max size of unint32
04/03/14    tp     Modified by running "atyle --style=allman" command.
06/28/13    dm     Modified TZ_OUT_BUF_MAX macro defintion
11/20/12    hw     add debug functions for non-contiguous support
10/09/12    hw     init version, define operations for memory array in
                   non-contiguous output buffer
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#define UINT32_MAXSIZE           0xFFFFFFFF
#define TZ_OUT_BUF_MAX           512

/* Structure to hold the pointers to multiple buffer space*/
struct tz_mem_array_seg
{
    uint32 _address;
    uint32 _size;
} __attribute__ ((packed));


/* Structure to hold the pointers to multiple buffer space*/
typedef struct tz_mem_array_s
{
    struct tz_mem_array_seg _tz_mem_seg[TZ_OUT_BUF_MAX];
    uint32 _seg_num;
} __attribute__ ((packed)) tz_mem_array_s_t;


uint32 tzmemArray_getSegSize(tz_mem_array_s_t* memArray, uint32 index);

uint8* tzmemArray_getSegAddress(tz_mem_array_s_t* memArray, uint32 index);

uint32 tzmemArray_getSegNumber(tz_mem_array_s_t* memArray);


boolean tzmemArray_checkSizeAndNum(tz_mem_array_s_t *ptr, uint32 input_size);

// testing methods
void tzmemArray_evenlydivided(tz_mem_array_s_t *ptr);
void tzmemArray_randomdivided(tz_mem_array_s_t *ptr);

void tzmemArray_printInfo(tz_mem_array_s_t *ptr);


#endif /* _TZMEMARRAY_H_*/

#ifndef __Q6ZIP_UNCOMPRESS_H__
#define __Q6ZIP_UNCOMPRESS_H__

/*===========================================================================
 * FILE:         q6zip_uncompress.h
 *
 * SERVICES:     DL PAGER
 *
 * DESCRIPTION:  q6zip uncompressor header
 *
 * Copyright (c) 2014 Qualcomm Technologies Incorporated.
 * All Rights Reserved. QUALCOMM Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

  EDIT HISTORY FOR MODULE
  
  $Header: //components/rel/core.mpss/3.5.c12.3/kernel/dlpager/inc/q6zip_uncompress.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/17/14   ao      Initial revision (collaboration with Ric Senior)
03/18/14   rs      Updated to version q6zip3v3
04/25/14   rs      Updated to version q6zip3v4
01/01/15   ao      Updated to version q6zip3v6
===========================================================================*/
#include <stdint.h>

#include "../compressor/q6zip_constants.py"
#define Q6ZIP_DICT_SIZE ((1<<DICT1_BITS)+(1<<DICT2_BITS))

typedef struct {
    uint32_t hold_low;
    uint32_t hold_high;
    int32_t  bits_in_hold;
    int32_t  last_sequencial_offset;
} q6zip_state_t;

int q6zip_uncompress(void* out_buf, void* in_buf, void* dict, q6zip_state_t* p_state);

#endif

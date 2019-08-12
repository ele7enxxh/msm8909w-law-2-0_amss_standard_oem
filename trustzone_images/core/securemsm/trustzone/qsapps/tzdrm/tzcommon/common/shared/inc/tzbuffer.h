#ifndef _TZBUFFER_H_
#define _TZBUFFER_H_
/*===========================================================================
  Copyright (c) 2011 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsapps/tzdrm/tzcommon/common/shared/inc/tzbuffer.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/03/14   tp      Modified by running "atyle --style=allman" command.
10/09/12   hw      init, create for operations on non-contiguous buffers
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "tzmemarray.h"

typedef enum
{
    TZBUF_SUCCESS = 0,
    TZBUF_FAIL = 1
} TZBUF_STATUS;

/***************************************************************************
 *  _bufs   : the list of contiguous memory pointer
 *  _size   : the total memory size
 *  _offset : the offset for the entire TzBuffer structure. TzBuffer is seen
 *            contiguous by its ops. _offset will be used by its ops on every
 *            function.
 * *************************************************************************/
typedef struct TzBuffer
{
    tz_mem_array_s_t * _bufs;
    uint32 _size;
    uint32 _offset;
    boolean _isCacheFlushed;
    //uint32* _sizeCacheflush;
} __attribute__ ((packed)) TzBuffer_t;

typedef int32 (*TZBUFOPS1)(uint8*, uint32*, void*);

typedef int32 (*TZBUFOPS2)(uint8*, uint8*, uint32*, void*);

TZBUF_STATUS tzbuf_init(struct TzBuffer* buffer, tz_mem_array_s_t* abufs, uint32 offset);

TZBUF_STATUS tzbuf_ops(struct TzBuffer* buffer, uint32 *size, uint32 offset, TZBUFOPS1 ops, void* carryover);

TZBUF_STATUS tzbuf_ops_onsrc(struct TzBuffer* buffer, uint8 * src_buf, uint32 *size, uint32 offset, TZBUFOPS2 ops, void* carryover);

/*
 * copy to TzBuffer
 * parm[in, out] : buffer
 *                 src_buf - source buffer address
 * parm[in]:       size - size of the source buffer
 *                 offset - offset to the TzBuffer (1-based)
 */
TZBUF_STATUS tzbuf_memcpy(struct TzBuffer* buffer, uint8 * src_buf, uint32 size, uint32 offset);

/*
 * return size of the TzBuffer object
 */
uint32 tzbuf_getSize(struct TzBuffer* buffer);

/*
 * return offset of the TzBuffer object
 */
uint32 tzbuf_getOffset(struct TzBuffer* buffer);

/*
 * convert the segmented address to physical address.
 * parm[in, out] : buffer
 *                 address - address to TzBuffer
 *                 space - space left for this address
 */
TZBUF_STATUS tzbuf_convertAddress(struct TzBuffer* buffer, uint8** address, uint32* space);

void tzbuf_printInfo(struct TzBuffer* buffer);

#endif /* _TZBUFFER_H_*/

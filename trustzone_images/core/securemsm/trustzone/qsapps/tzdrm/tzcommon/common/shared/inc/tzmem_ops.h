#ifndef TZ_MEM_OPS_H
#define TZ_MEM_OPS_H

/** @file tzmem_ops.h
 * @brief
 * This file contains the definitions of the constants, data structures and
 * and functions for OEM Crypto Decryption implementation on TZ Service Side
 */
/*===========================================================================
  Copyright (c) 2011-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsapps/tzdrm/tzcommon/common/shared/inc/tzmem_ops.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/18/14   rk      Renamed tzcommon_entry.h to app_common_svc_handler.h
04/03/14   tp      Modified by running "atyle --style=allman" command.
07/01/13   rk      Added support copy data from secure to non-secure buffer for internal testing
11/20/12   hw      add new ops for single contiguous buffer. these ops are used
                   by non-contiguous buffer framework as well.
09/18/12   hw      Initial Version. define operations for tz memory

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/** @addtogroup TZMEM_Svc
  @} */
#include "comdef.h"
#include "app_common_svc_handler.h"
#include "qsee_core.h"
#include "qsee_cipher.h"

#define CAST(type, ptr) ((type)(ptr))
extern boolean qsee_is_s_tag_area(uint32  tag, uint32  start, uint32  end);


typedef struct TzClass
{
    const char * name;
    uint32 size;
} __attribute__ ((packed)) TZCLASS;

void * tzmem_new(const void * tzclass);
void tzmem_delete(void * self);

int32 tzmem_copy(uint8* dest, uint8* src, uint32* size, void* carryover);

/* This API is for internal testing use only.  It fails on external builds */
int32 tzmem_internal_copy(uint8* dest, uint8* src, uint32* size, void* carryover);

int32 tzmem_is_s_area_tag(uint8* ptr, uint32* size, void* carryover);

/****************************************************
 * decrypt only one contiguous memory chunk
 * before the decryption happens, it
 *     1) validate the decypt size
 *     2) cacheflush the memory
 *     3) validate the memory protection
 * after the decryption finishes, it
 *     4) stop cacheflushing the memory
 * *************************************************/
int32 tzmem_decrypt(uint8* dest, uint8* src, uint32* size, qsee_cipher_ctx* ctx);

int32 tzmem_cacheflush_register(uint8*  input, uint32*  size);
int32 tzmem_cacheflush_deregister(uint8* input, uint32 size);
void tzmem_reverseByteOrder(unsigned char* ptr, unsigned int size);

#endif //TZ_MEM_OPS_H

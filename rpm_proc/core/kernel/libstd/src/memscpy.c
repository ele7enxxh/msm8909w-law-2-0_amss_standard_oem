/*===============================================================================================
 * FILE:        libstd_std_scanul.c
 *
 * DESCRIPTION: Implementation of a secure API memscpy - Size bounded memory copy. 
 *
 *              Copyright (c) 2012 Qualcomm Technologies Incorporated.
 *              All Rights Reserved. QUALCOMM Proprietary and Confidential.
 *===============================================================================================*/
 
/*===============================================================================================
 *
 *                            Edit History
 * $Header: //components/rel/rpm.bf/2.1.1/core/kernel/libstd/src/memscpy.c#1 $ 
 * $DateTime: 2015/02/25 02:53:43 $
 *
 *===============================================================================================*/

#include <stringl/stringl.h>

size_t  memscpy(
          void        *dst,
          size_t      dst_size,
          const void  *src,
          size_t      src_size
          )
{
  size_t  copy_size = (dst_size <= src_size)? dst_size : src_size;

  memcpy(dst, src, copy_size);

  return copy_size;
}

#ifndef TZBSP_UIO_H
#define TZBSP_UIO_H

/**
@file tzbsp_uio.h
@brief Process scatter/gather data
*/

/*===========================================================================
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/armv7/include/tzbsp_uio.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
03/08/12   th       Initial version.

===========================================================================*/

#include <comdef.h>

/*------------------------------------------------------------------------
                            PUBLIC TYPES
------------------------------------------------------------------------*/

typedef PACKED struct 
{
  void             *iov_base;  /* Base address. */
  uint32            iov_len;   /* Length. */
} tzbsp_iovec_t;

typedef PACKED struct 
{
  tzbsp_iovec_t    *iov;                 
  uint32            iov_cnt;                 
} tzbsp_uio_t;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/**
 * Takes a virtual memory buffer and converts it in to an iov list of
 * physical memory pointers
 *
 * @param [in]  a virtual address
 *
 * @param [in]  the length of the virtual buffer to examine
 *
 * @param [out] Returns the virtual address as an array of type uio. The 
 *              calling function needs to have allocated the uio for 
 *              population by this function.
 *
 * @return E_SUCCESS if successful
 *         E_INVALID_ARG if uio_out is invalid
 *         E_FAILURE otherwise
 */
int tzbsp_uio(uint32 buf_addr, uint32 buf_len, const void *uio_out);

#endif

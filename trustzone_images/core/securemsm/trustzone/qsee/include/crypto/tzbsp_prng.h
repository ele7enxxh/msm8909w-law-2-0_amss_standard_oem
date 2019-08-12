#ifndef TZBSP_PRNG_H
#define TZBSP_PRNG_H

/**
@file tzbsp_prng.h
@brief Provide prng API wrappers
*/

/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/crypto/tzbsp_prng.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
04/05/10   cap      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define TZBSP_MAX_PRNG    512  /* Maximum number of PRNG bytes read */

/**
 * @brief Release all resources with a given prng context.
 *
 * @param[in] out     The output data buffer
 * @param[in] out_len The output data length. The out_len
 *                    must be at most TZBSP_MAX_PRNG bytes.
 *
 * @return number of bytes read
 *
 */
uint32 tzbsp_prng_getdata(uint8 *out, uint32 out_len);

#endif

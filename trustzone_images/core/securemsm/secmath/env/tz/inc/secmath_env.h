#ifndef SECMATH_ENV_H
#define SECMATH_ENV_H

/**
@file secmath_env.h
@brief This file is used to definen environment specific functionality.
*/

/*===========================================================================
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/secmath/env/tz/inc/secmath_env.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
01/03/12   cap      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
extern uint32 tzbsp_prng_getdata(uint8 *out, uint32 out_len);

/* Random number generator function */
#define secmath_prng_getdata(seed, seedlen)\
            tzbsp_prng_getdata(seed, seedlen)

#endif /* SECRSA_ENV_H */

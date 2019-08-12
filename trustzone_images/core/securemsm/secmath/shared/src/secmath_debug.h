/**
  @file secmath_debug.h

  @brief General debugging
*/

/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/secmath/shared/src/secmath_debug.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
05/14/11   vg       changed the secmath_assert macro to return error instead spinning
                    infinitely.
05/11/11   vg       Initial Revision

===========================================================================*/

#ifndef SECMATH_DEBUG_H
#define SECMATH_DEBUG_H


#ifdef __cplusplus
extern "C"
{
#endif


#define DO_SPIN_FOREVER(x) do { while (x); } while (0)


#ifndef SECMATH_CHECK_ARGS
#define SECMATH_ASSERT(x) do { if (!(x)) return E_SECMATH_FAILURE; }while (0)
#else
/* Something bad happens so we really just spin */
#define SECMATH_ASSERT(x) do { if (!(x)) \
   { SECMATH_OUT("Assertion #x failed, on line %d, in file %s\n",  __LINE__, __FILE__); \
     DO_SPIN_FOREVER(1); } while(0)
#endif

#ifndef SECMATH_DEBUG_OUT
static void secmath_debug_do_nothing(const char *str, ...)
{
}
#define SECMATH_OUT secmath_debug_do_nothing
#else
#include <stdio.h>
   /* Place the printf style output function here */
#define SECMATH_OUT
#endif


#ifdef __cplusplus
}
#endif

#endif

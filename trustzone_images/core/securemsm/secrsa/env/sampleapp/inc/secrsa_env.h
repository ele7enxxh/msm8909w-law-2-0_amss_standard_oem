#ifndef SECRSA_ENV_H
#define SECRSA_ENV_H

/**
@file secrsa_env.h
@brief This file is used to define the Hash functions, random 
       number genrator function & Memory management functions
       that are provided by the environment
*/

/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/secrsa/env/sampleapp/inc/secrsa_env.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
08/20/10    hv      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qsee_hash.h"
#include "qsee_prng.h"
#include "qsee_heap.h"
#ifndef TZ_APP_LEGACY
#include "cmnlib_cntxt.h"
#endif

// hash functions
#define ce_hash_ctx        qsee_hash_ctx

#define ce_hash(hash_idx, lparam, lparamlen, lhash, hashlen)\
            qsee_hash((QSEE_HASH_ALGO_ET) hash_idx, lparam, lparamlen, lhash, hashlen)

#define ce_hash_init(hash_idx, hash_ctx)\
            qsee_hash_init((QSEE_HASH_ALGO_ET)hash_idx, hash_ctx)

#define ce_hash_update(hash_ctx, msg, msglen)\
            qsee_hash_update(hash_ctx,msg, msglen)

#define ce_hash_final(hash_ctx, msg, msglen)\
            qsee_hash_final(hash_ctx, msg, msglen)

#define ce_hash_free_ctx(hash_ctx)\
            qsee_hash_free_ctx(hash_ctx)

// Random number generator function
#define ce_prng_getdata(seed, seedlen)\
            qsee_prng_getdata(seed, seedlen)

// Memory managment functions
#undef malloc
#undef free

#ifndef TZ_APP_LEGACY

#define malloc cmnlib_malloc
#define free   cmnlib_free

#else

#define malloc qsee_malloc
#define free qsee_free

#endif

#define SUPPORT_RSA_CRT

#endif /* SECRSA_ENV_H */

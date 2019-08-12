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

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/secrsa/env/tz/inc/secrsa_env.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
01/14/15    cz      Added "#define SUPPORT_RSA_CRT"
08/20/10    hv      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "crypto/tzbsp_hash.h"
#include "crypto/tzbsp_prng.h"
#include "tzbsp_mem.h"
#ifndef TZ_APP_LEGACY
#include "cmnlib_cntxt.h"
#endif

// hash functions
#define ce_hash_ctx        tzbsp_hash_ctx

#define ce_hash(hash_idx, lparam, lparamlen, lhash, hashlen)\
            tzbsp_hash((TZBSP_HASH_ALGO_ET) hash_idx, lparam, lparamlen, lhash, hashlen)

#define ce_hash_init(hash_idx, hash_ctx)\
            tzbsp_hash_init((TZBSP_HASH_ALGO_ET)hash_idx, hash_ctx)

#define ce_hash_update(hash_ctx, msg, msglen)\
            tzbsp_hash_update(hash_ctx,msg, msglen)

#define ce_hash_final(hash_ctx, msg, msglen)\
            tzbsp_hash_final(hash_ctx, msg, msglen)

#define ce_hash_free_ctx(hash_ctx)\
            tzbsp_hash_free_ctx(hash_ctx)

// Random number generator function
#define ce_prng_getdata(seed, seedlen)\
            tzbsp_prng_getdata(seed, seedlen)

// Memory managment functions
#ifndef TZ_APP_LEGACY
extern CMNLIB_CNTXT *pgAppCntxt;
#endif

#undef malloc
#undef free

#define malloc tzbsp_malloc
#define free tzbsp_free

#define SUPPORT_RSA_CRT

#endif /* SECRSA_ENV_H */

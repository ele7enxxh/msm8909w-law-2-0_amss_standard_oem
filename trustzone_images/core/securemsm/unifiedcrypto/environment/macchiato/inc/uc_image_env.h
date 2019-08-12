#ifndef UC_IMAGE_ENV_H
#define UC_IMAGE_ENV_H
/*===========================================================================

                             Unified Crypto 

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE
 
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/unifiedcrypto/environment/macchiato/inc/uc_image_env.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
 10/10/12   yk     Initial version
 ===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "uc_comdef.h"
#include "qsee_heap.h"
/*===========================================================================
                 DEFINITIONS AND TYPE DECLARATIONS
===========================================================================*/
/* Forward declare tzbsp functions */
//uint32 tzbsp_prng_getdata(uint8 *out, uint32 out_len);
uint32 qsee_prng_getdata(uint8 *out, uint32 out_len);

#if 0
// Memory managment functions
#undef malloc
#undef free
#endif

#define malloc qsee_malloc
#define free qsee_free


/* end of forward declare */


#define uc_internal_malloc(ptr, ptrLen) \
       {*ptr = malloc(ptrLen);}
//       {*ptr = tzbsp_malloc(ptrLen);}

#define uc_internal_free(ptr) \
	   {free (ptr);}
//	   {tzbsp_free (ptr);}


#define uc_internal_prng_getdata(returnvar, ptr, ptrLen) \
         {returnvar = qsee_prng_getdata(ptr, ptrLen);}

//sw_crypto_errno_enum_type uc_internal_malloc(void** ptr, uint32 ptrLen);

//sw_crypto_errno_enum_type uc_internal_free(void* ptr);

#endif //UC_IMAGE_ENV_H

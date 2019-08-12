#ifndef _NLS_CSPRNG_DEFINED
#define _NLS_CSPRNG_DEFINED
/*===========================================================================

                      EDIT HISTORY FOR FILE
 
  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/shared/src/seccsprng.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/10   nk      Shared Library Framework    	
===========================================================================*/

/* Use stream cipher NLS as a Pseudo-Random Number Generator (PRNG)
 * Copyright C 2005, Qualcomm Technologies Inc. 
 * Created on 05/26/2005 by lxiao
 */

#ifdef errno
#undef errno
#endif

#include "comdef.h"
#include "secnls.h"	/* type definition */
#include "secerrno.h"	/* error message definition */

/* seed/reseed function */
secerrno_enum_type csprng_seed(uint8 *seed, int seed_length);

/* random sequence output function */
secerrno_enum_type csprng_rand(uint8 *rand_out, int rand_length);

#endif


#ifndef SEC_SNOW_3G_H
#define SEC_SNOW_3G_H
/*===========================================================================

                      S E C U R I T Y    S E R V I C E S
      
                  S E C   S N O W    3 G   R O U T I N E S


DESCRIPTION
  This is the header file for the SNOW3G ciphering module.

EXTERNALIZED FUNCTIONS
  snow3g_init()
  snow3g_update()
  snow3g_keystream_word()
  snow3g_final()

 Copyright (c) 2009-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/shared/src/secsnow3g.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/10   nk      Shared Library Framework 
02/11/10   ejt     Added LFSR and FSM to function prototypes 
09/04/09   qxu     create file.    	
===========================================================================*/

/*===========================================================================

                              INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "secapi.h"

/*typedef uint8 u8;
typedef uint32 u32;
typedef uint32 uint32 uint64;*/

/* Initialization.
* Input k[4]: Four 32-bit words making up 128-bit key.
* Input IV[4]: Four 32-bit words making 128-bit initialization variable.
* Output: All the LFSRs and FSM are initialized for key generation.
* See Section 4.1.
*/
void Initialize(uint32 k[4], uint32 IV[4], uint32 LFSR[SECAPI_HSH_SNOW3G_LFSR_SIZE], 
                uint32 FSM[SECAPI_HSH_SNOW3G_FSM_SIZE]);
/* Generation of Keystream.
* input n: number of 32-bit words of keystream.
* input z: space for the generated keystream, assumes
* memory is allocated already.
* output: generated keystream which is filled in z
* See section 4.2.
*/
void GenerateKeystream(uint32 n, uint32 *z, uint32 LFSR[SECAPI_HSH_SNOW3G_LFSR_SIZE], 
                       uint32 FSM[SECAPI_HSH_SNOW3G_FSM_SIZE]);

#endif /* SEC_SNOW_3G_H */

#ifndef SECAPIUMBUTILS_H
#define SECAPIUMBUTILS_H
/*===========================================================================

                    S E C U R I T Y    S E R V I C E S

                 C R Y P T O G R A P H I C   L I B R A R Y

         E X T E R N A L   A P P L I C A T I O N   I N T E R F A C E
                              
                      U T I L I T Y   F U N C T I O N S
DESCRIPTION
  This file defines certain cryptographic utility functions that use the 
  Security Services API's
  The utility functions implemented are:

EHMAC-SHA-256
  - as defined in the document 3GPP2 S.S0078-B (Version 1)
  <make sure we have a permanent place holder for this document inside Qualnet>

AES-CMAC Algorithm
  - as defined in the document RFC 4493


EXTERNALIZED FUNCTIONS
  - secapiutils_init()
  - secapiutils_ehmac_sha256()
  - secapiutils_aes128_cmac()


INITIALIZATION AND SEQUENCING REQUIREMENTS
  The cryptographic library is initialized by calling the secapiutils_init().


 Copyright (c) 2002-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
  ===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/environment/inc/secapiutils.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/10   nk      Shared Library Framework 
01/28/08   rv      Created file.

===========================================================================*/

#include "comdef.h"
#include "secapi.h"
/*===========================================================================

                        DEFINITIONS AND TYPES

===========================================================================*/

/* Padding types */
#define SECAPI_EHMAC_IPAD 0x36
#define SECAPI_EHMAC_OPAD 0x5c


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION 
  SECAPIUMB_SHA256_EHMAC()

DESCRIPTION
  For detailed descripton of this function refer to document 3GPP2 
  S.S0078-B (Version 1)

DEPENDENCIES
  Any application using Crypto Utility functions must first initialize
  the UMB crypto library by first calling secapiutils_init().

PARAMETERS
  key             -[I/P] HMAC key
  l_key           -[I/P] Key Length
  message         -[I/P] pointer to input mesage stream
  message_offset  -[I/P] pointer to start of message
  message_length  -[I/P] input message length in bits
  ehmac           -[O/P] pointer to EHMAC digest 
                          ( memory should be allocated by caller )
  l_ehmac         -[I/P] expected length of EHMAC in bytes 
                          ( has to be less than SECAPI_HSH_SHA256_DIGEST_SIZE ) 

RETURN VALUE
  Status of request:
    AEE_SUCCESS   if successful
    AEE_ENOMEMORY if running out of the free space
    AEE_EFAILED   general failure case

SIDE EFFECTS
  None.
===========================================================================*/
extern int secapiutils_sha256_ehmac
(
  unsigned char       key[],
  int                 l_key,
  unsigned char       *message,
  unsigned long       message_offset,
  unsigned long       message_length,
  unsigned char       *ehmac,
  int                 l_ehmac
);

/*===========================================================================

FUNCTION 
  SECAPIUMB_AES128_CMAC()

DESCRIPTON
  For detailed description of this API, refer to document RFC4493.

DEPENDENCIES
  Any application using Crypto Utility functions must first initialize
  the UMB crypto library by first calling secapiumb_init().

PARAMETERS
  Key             -[I/P] AES key 
  l_key           -[I/P] Key Length ( should be 16B )( 128 bit key ) 
  message         -[I/P] pointer to input mesage 
  message_length  -[I/P] input message length in bytes
  cmac           -[O/P] pointer to CMAC digest 
                          ( memory should be allocated by caller )
  l_cmac         -[I/P] expected length of CMAC in bytes 
                          ( has to be less than SECAPI_ENC_AES128_BLOCK_BYTE_LEN ) 

RETURN VALUE
  Status of request:
    AEE_SUCCESS   if successful
    AEE_ENOMEMORY if running out of the free space
    AEE_EFAILED   general failure case

SIDE EFFECTS
  None.
===========================================================================*/
extern secerrno_enum_type secapiutils_aes128_cmac
(
  uint8         *key,
  uint8         l_key,
  uint8         *message,
  uint16        l_message,
  uint8         *cmac,
  uint8         l_cmac
);

#endif /* SECAPIUMBUTILS_H */
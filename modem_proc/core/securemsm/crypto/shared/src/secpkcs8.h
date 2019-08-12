/*==========================================================================

         PKCS #8: Private-Key Information Syntax Standard V1.2

GENERAL DESCRIPTION
  This software module contains the source code for PKCS#8 private key 
  parsing. Note that the module only parses the private key in plaintext. 
  The encrypted private key (Section 7, PKCS#8) is not supported at this
  moment, which requires implementation of PKCS#5 V1.5 and PKCS#12.

  Only RSA private key is supported in this version. 
      
EXTERNALIZED FUNCTIONS

  secpkcs8_parse()     Parse a single private key file

Copyright (c) 2002-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/shared/src/secpkcs8.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/10   nk      Shared Library Framework 
05/11/06   lx      Created
===========================================================================*/


#include <stdlib.h>
#include "comdef.h"

#include "secerrno.h"
#include "secx509.h"
#include "secx509asntypes.h"


/* Public Key algorithms in pkcs8 */
typedef enum
{
    PKCS8_RSA_ENCRYPTION = 0,
    PKCS8_ALGO_MAX               /* Last one, for error checking */

}   pkcs8_algo_type;

/* RSA private key parameters */
/* refer to Sect7.2, PKCS#1: RSA Encryption Standard */
typedef struct 
{
    uint8   version;
    uint16  mod_len;        /* modulus: n               */
    uint8*  mod_data;
    uint16  pub_exp_len;    /* public exponent: e       */
    uint8*  pub_exp_data;
    uint16  priv_exp_len;   /* private exponent: d      */
    uint8*  priv_exp_data;
    uint16  prime1_len;     /* prime 1: p               */
    uint8*  prime1_data;
    uint16  prime2_len;     /* prime 2: q               */
    uint8*  prime2_data;
    uint16  exp1_len;       /* exponent 1: d mod (p-1)  */
    uint8*  exp1_data;
    uint16  exp2_len;       /* exponent 1: d mod (q-1)  */
    uint8*  exp2_data;
    uint16  coef_len ;      /* coefficient: q**{-1} mod p   */
    uint8*  coef_data;

}   pkcs8_rsa_privkey_type;

/* DSA private key parameters */
typedef struct
{
    uint16  dummy_len;
    uint8*  dummy;

}   pkcs8_dsa_privkey_type;

/* DH private key parameters */
typedef struct
{
    uint16  mod_len;
    uint8*  mod_data;
    uint16  exp_len;
    uint8*  exp_data;

}   pkcs8_dh_privkey_type;


/* Union of all the private key types */
typedef struct
{
    uint8              ver;
    pkcs8_algo_type    algo;
    union
    {
        pkcs8_rsa_privkey_type  rsa;
        pkcs8_dsa_privkey_type  dsa;
        pkcs8_dh_privkey_type   dh;
     
    }   key;

}   pkcs8_privkey_type;

/*===========================================================================

FUNCTION PKCS8_PARSE

DESCRIPTION
  Parses a private key in PKCS#8 format. The private key contains multiple 
  items pointing to the memory where raw PKCS#8 data is held. 

DEPENDENCIES
  None
  
PARAMETERS
  data_ptr - pointer to the raw PKCS#8  data
  data_len - length of the PKCS#8 data
  privkey  - pointer to the private key extracted from the raw data

RETURN VALUE
  E_SUCCESS if the private key is parsed properly
  E_DATA_INVALID if the key cannot be parsed properly
  E_NOT_SUPPORTED if an algorithm or version is not supported
  E_INVALID_ARG if a pointer argument is NULL
  E_FAILURE if input data length does not match the length parsed
  
SIDE EFFECTS
  None
===========================================================================*/

secerrno_enum_type secpkcs8_parse
(
    uint8*                 data_ptr,
    uint16                 data_len,
    pkcs8_privkey_type*    privkey
);


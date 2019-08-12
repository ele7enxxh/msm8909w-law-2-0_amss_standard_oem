#ifndef SIMLOCK_CRYPTO_H
#define SIMLOCK_CRYPTO_H
/*===========================================================================


            S I M L O C K   C R Y P T O   H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2014 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_crypto.h#1 $$ $DateTime: 2016/12/13 08:00:05 $ 

when       who     what, where, why
--------   ---     -----------------------------------------------------------
01/20/16   stv     Fix to validate the RSA signature in the blob request
01/06/16   stv     Remote simlock support
05/15/15   vv      Support for RSU set configuration in simlock
05/15/15   vv      Support for RSU in simlock
02/02/14   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifdef FEATURE_SIMLOCK

#include "simlock.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#define SIMLOCK_HMAC_SHA256_LEN                  32
#define SIMLOCK_RSA_LEN                         256

/* ----------------------------------------------------------------------------
   DATA TYPE:      simlock_hmac_signature_type
------------------------------------------------------------------------------*/
typedef uint8  simlock_hmac_signature_type[SIMLOCK_HMAC_SHA256_LEN];

/* ----------------------------------------------------------------------------
   DATA TYPE:      simlock_rsa_signature_type
------------------------------------------------------------------------------*/
typedef uint8  simlock_rsa_signature_type[SIMLOCK_RSA_LEN];

/* ----------------------------------------------------------------------------
   ENUM TYPE:      simlock_crypto_rsa_operation_type
------------------------------------------------------------------------------*/
typedef enum
{
  SIMLOCK_CRYPTO_RSA_ENCRYPT            = 0,
  SIMLOCK_CRYPTO_RSA_SIGNATURE_VALIDATE
}simlock_crypto_rsa_operation_type;

/*===========================================================================
FUNCTION SIMLOCK_CRYPTO_GENERATE_SALT

DESCRIPTION
  This function generates the random digits of request length

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_crypto_generate_salt
(
  simlock_salt_type   salt
);

/*===========================================================================
FUNCTION SIMLOCK_CRYPTO_RUN_PBKDF2_ALGORITHM

DESCRIPTION
  This function generates the hash of the control key (HCK) using the PBKDF2
  algorithm. It takes password (control key), salt and iterations as the
  input and outputs the hashed control key (key_data_ptr). The PBKDF2 uses
  the HMAC_SHA256 as the pseudo random function

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_crypto_run_pbkdf2_algorithm
(
  const uint8            * password_data_ptr,
  uint16                   password_data_len,
  simlock_salt_type        salt_data,
  uint16                   salt_data_len,
  uint32                   iterations,
  simlock_hck_type         key_data,
  uint32                   key_data_len
);

/*===========================================================================
FUNCTION SIMLOCK_CRYPTO_GET_RANDOM_DATA

DESCRIPTION
  This function generates the random data of requested length

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_crypto_get_random_data
(
  uint8      * data_ptr,
  uint16       data_len
);

/*===========================================================================
FUNCTION SIMLOCK_CRYPTO_RSA

DESCRIPTION
  This function performs rsa operation specified in the input parameter rsa_operation

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_crypto_rsa(
  simlock_crypto_rsa_operation_type    rsa_operation,
  uint8                             *  in_data_ptr,
  uint16                               in_len,
  uint8                             *  mod_ptr,
  uint16                               mod_len,
  uint8                             *  exp_ptr,
  uint16                               exp_len,
  simlock_data_type                 *  out_data_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_CRYPTO_GENERATE_HMAC_SHA256

DESCRIPTION
  This function generates the mac for the message to be authenticated.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_crypto_generate_hmac_sha256(
  uint8                        * msg_ptr,
  uint16                         msg_len,
  uint8                        * key_ptr,
  uint16                         key_len,
  simlock_hmac_signature_type    hmac_data
);

/*===========================================================================
FUNCTION SIMLOCK_CRYPTO_GENERATE_DIGEST_SHA256

DESCRIPTION
  This function generates the digest for the message to be authenticated.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_crypto_generate_digest_sha256(
  uint8                        * msg_ptr,
  uint16                         msg_len,
  simlock_data_type            * digest_info_ptr
);
#endif /* FEATURE_SIMLOCK */

#endif /* SIMLOCK_CRYPTO_H */
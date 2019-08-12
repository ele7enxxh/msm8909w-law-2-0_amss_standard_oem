#ifndef SECSSLKEYGEN_H
#define SECSSLKEYGEN_H


/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

                S E C U R E   S O C K E T S   L A Y E R

                  S S L   K E Y   G E N E R A T I O N

GENERAL DESCRIPTION
  Header file for key generation

EXTERNALIZED FUNCTIONS
  secsslkeygen_create_keys
  secsslkeygen_create_master_secret
  secsslkeygen_tlsv1_prf


INITIALIZATION AND SEQUENCING REQUIREMENTS
  The required SSL session parameters have to be set.  

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/sec/ssl/shared/src/secsslkeygen.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/11/15   nm      TLSv1.2 support
06/01/10   ml      Removed seccrypt.h from include definitions.
09/15/03   jay     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "sec_variation.h"
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_SEC_SSL
#include "secapi.h"
#include "secerrno.h"

#include "secssldef.h"
#include "secssl.h"
#include "secssli.h"


/*===========================================================================

FUNCTION SECSSLKEYGEN_CREATE_KEYS

DESCRIPTION
  This function derives the MAC secrets, cipher keys, and cipher
  initialization vectors based on the master secret and the server
  and client random numbers.

DEPENDENCIES
  This function has to be called _before_ the ciphersuite is activated.
  
PARAMETERS
  session_ptr - Pointer to session for which to derive the keys

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secsslkeygen_create_keys
(
  secssl_session_type*  session_ptr
);


/*===========================================================================

FUNCTION SECSSLKEYGEN_CREATE_MASTER_SECRET

DESCRIPTION
  This function derives the master secret from the premaster secret
  and the server and client random numbers.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr - Pointer to session for which to derive the master secret

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secsslkeygen_create_master_secret
(
  secssl_session_type*  session_ptr
);


/*===========================================================================

FUNCTION SECSSLKEYGEN_TLSV1_PRF

DESCRIPTION
  This function computes the requested amount of pseudorandom output
  according to the procedure specified by TLS V1.
  
DEPENDENCIES
  None
  
PARAMETERS
  seed_ptr    - seed data (input for hashes)
  seed_len    - length of seed data
  secret_ptr  - secret data (input for hashes)
  secret_len  - length of secret data
  out_ptr     - address of output buffer
  req_len     - requested number of bytes of output
  
RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secsslkeygen_tlsv1_prf
(
  dsm_item_type*   seed_ptr, 
  uint16           seed_len,
  dsm_item_type*   secret_ptr, 
  uint16           secret_len,
  dsm_item_type**  out_ptr,
  uint16           req_len
);


/*===========================================================================

FUNCTION SECSSLKEYGEN_TLSV1_CORE

DESCRIPTION
  This function does the P_Hash functions based on the hash algorithm
  according to the procedure specified by TLS V1.
  
DEPENDENCIES
  None
  
PARAMETERS
  seed_ptr    - seed data (input for hashes)
  seed_len    - length of seed data
  secret_ptr  - secret data (input for hashes)
  secret_len  - length of secret data
  out_ptr     - address of output buffer 
  req_len     - requested number of bytes of output

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secsslkeygen_tlsv1_core
(
  secapi_algo_enum_type  hmac_algo,
  dsm_item_type*         seed_ptr,
  uint16                 seed_len,
  dsm_item_type*         secret_ptr, 
  uint16                 secret_len,
  dsm_item_type**        out_ptr,
  uint16                 req_len
);



/*===========================================================================

FUNCTION SECSSLKEYGEN_TLSV12_PRF

DESCRIPTION
  This function computes the requested amount of pseudorandom output
  according to the procedure specified by TLS V12.
  
DEPENDENCIES
  None
  
PARAMETERS
  seed_ptr    - seed data (input for hashes)
  seed_len    - length of seed data
  secret_ptr  - secret data (input for hashes)
  secret_len  - length of secret data
  out_ptr     - address of output buffer
  req_len     - requested number of bytes of output
  
RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secsslkeygen_tlsv12_prf
(
  dsm_item_type*   seed_ptr, 
  uint16           seed_len,
  dsm_item_type*   secret_ptr, 
  uint16           secret_len,
  dsm_item_type**  out_ptr,
  uint16           req_len
);

#endif /* FEATURE_SEC_SSL */

#endif /* SECSSLKEYGEN_H */

      

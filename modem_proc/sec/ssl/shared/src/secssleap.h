#ifndef SECSSLEAP_H
#define SECSSLEAP_H


/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

          E A P   E N C R Y P T I O N   H E L P   F U N C T I O N S

GENERAL DESCRIPTION
  Header file for EAP/EAP-TLV/PEAP/TTLS encryption

EXTERNALIZED FUNCTIONS
  secssl_create_eap_tls_key
  secssl_create_eap_SHA

INITIALIZATION AND SEQUENCING REQUIREMENTS
  secssl_open_session() must be called

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/sec/ssl/shared/src/secssleap.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/11/09   yh      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "sec_variation.h"
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_SEC_SSL
#include "secerrno.h"
#include "secssl.h"

/*===========================================================================

FUNCTION SECSSL_CREATE_EAP_TLS_KEY

DESCRIPTION
  This function derives EAP-TLS KEY or cipher initialization vectors based on  
  seed (label key + SSL session's server and client random numbers )
  secret (the SSL session's master secret) in iput_ptr

DEPENDENCIES
  This function has to be called after secssl_open_session.
  
PARAMETERS
  input_ptr   - Pointer to SSL session handle and label key
  output_ptr  - Place holder for the encryption output


RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssl_create_eap_tls_key
(
  secssl_eap_session_key_type*     input_ptr,
  secssl_eap_encrypt_output_type*  out_ptr
);

/*===========================================================================

FUNCTION SECSSL_CREATE_EAP_SHA

DESCRIPTION
  seed (Intermediate combined key) and secret (from EAP session in input_ptr

DEPENDENCIES
  None
  
PARAMETERS
  input_ptr   - Pointer to data for which to derive SHA
  output_ptr  - Place holder for the encryption output


RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssl_create_eap_SHA
(
  secssl_eap_intermediate_combined_key_type*  input_ptr,
  secssl_eap_encrypt_output_type*             out_ptr
);

#endif /* FEATURE_SEC_SSL */

#endif /* SECSSLEAP_H */
     

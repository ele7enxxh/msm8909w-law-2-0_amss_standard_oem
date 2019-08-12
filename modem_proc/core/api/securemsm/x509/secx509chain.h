#ifndef SECX509CHAIN_H
#define SECX509CHAIN_H

/** 
  @file secx509chain.h
  @brief
  This file contains the definitions for verifcation of certificates against
  a given root.
**/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The secx509_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      secx509_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the secx509chain_apis group 
      description in the secx509_mainpage.dox file. 
=============================================================================*/
/*=============================================================================
   Copyright (c) 2000-2012 Qualcomm Technologies Incorporated.  
   All rights reserved.
   Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================
  Below is an example of how to verify a certificate chain

\endcode

INITIALIZATION AND SEQUENCING REQUIREMENTS

  secx509_init() must be called before using the other functions.

=============================================================================*/
/*=============================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/x509/secx509chain.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/05/11   sd      (Tech Pubs) Edited/added Doxygen markup and comments.
08/10/10   ml      added mainpage, dscriptions for functions, constants and data types
                   to doxygenate API document.
08/10/06   lx      Add current_time as an argument for 
                   secx509chain_validate_cert_list
04/12/06   lx      Added secx509chain_init()
10/23/00   om      Created Module

===========================================================================*/

/*==========================================================================

             Include Files for Module 

==========================================================================*/
#include "comdef.h"
#include "secerrno.h"

#include "secx509.h"


/**
   @addtogroup secx509chain_apis
   @{
*/

/*==========================================================================

             Local Constant Declaration

==========================================================================*/
/**
  Maximum chain depth.
*/
#define SECX509CHAIN_MAX_CHAIN_LEN    10

/** @} */  /* end_addtogroup secx509chain_apis */


/**
  @addtogroup secx509chain_apis
  @{
*/
/*==========================================================================

             Structure Definition

==========================================================================*/
/**
  @brief Certificate chain structure.
*/
typedef struct
{
  /** Comment needed here. */
  struct
  {
    secx509_cert_type    cert;             /**< Certificate info data holder. */
    uint8*               cert_ptr;         /**< Certificate file buffer. */
    uint32               cert_len;         /**< Certificate file length. */
  } element[SECX509CHAIN_MAX_CHAIN_LEN];

  secx509_cert_list_type root_cert_list;   /**< Parsed root certificate data holder. */
  secx509_cert_ctx_type  cert_ctx;         /**< Chained certificates information. */

  
  uint8*        root_cert_ptr[ SECX509_NUM_CERTS ]; 
    /**< Stores the pointers to the root certificates. */

} secx509chain_ctx_type;

/** @} */  /* end_addtogroup secx509chain_apis */

/*=============================================================================

                      FUNCTION DECLARATIONS

=============================================================================*/

/**
  @addtogroup secx509chain_apis
  @{
*/

/*=============================================================================
   FUNCTION : SECX509CHAIN_INIT
=============================================================================*/

/**
  Initializes the chain context.

  @param[out] context_ptr   Pointer to the secx509 chain context to be
                            initialized.
  @param[in] purpose        Purpose of the certificate usage. Currently supports
                            SSL Server authentication and OMA-DRM Rights Issuer
                            authentication.

  @return
  E_SUCCESS -- Chain context is initialized properly.\n
  E_NOT_SUPPORTED -- Purpose is not supported.\n
  E_INVALID_ARG -- Pointer argument is NULL.

  @dependencies
  None.
*/
secerrno_enum_type secx509chain_init
(
  secx509chain_ctx_type* context_ptr,
  uint16                 purpose
);

/*=============================================================================
   FUNCTION : SECX509CHAIN_ADD_ROOT_CERT
=============================================================================*/

/**
  Adds a root certificate to the chain context.

  This list of root certificates is used for all verifications made with this 
  chain context.

  This function copy the certificate data buffer and parse it.
  secx509chain_clear_root_cert_list() must be called later in order to free the memory
  allocated by this function.

  @param[out] context_ptr  secx509chain context to which the root certificate is added.
  @param[in] cert_ptr      Raw root certificate data.
  @param[in] cert_len      Length of the raw root certificate data.
  
  @return
  E_SUCCESS -- A root cert is added to chain context successfully.\n
  E_NO_MEMORY -- System can't allocate memory to hold root cert.\n
  E_INVALID_ARG -- Pointer argument is NULL or chain context met its root cert capability.\n
  E_FAILURE -- Parse of root cert failed.

  @dependencies
  This function uses the secasn() and ossl_asn() functionality.
*/
secerrno_enum_type secx509chain_add_root_cert
(
  secx509chain_ctx_type* context_ptr,
  uint8*                 cert_ptr,
  uint32                 cert_len
);

/*=============================================================================
   FUNCTION : SECX509CHAIN_ADD_PARSED_ROOT_CERT
=============================================================================*/

/**
  Adds a parsed root certificate to the chain context.

  This list of root certificates is used for all verifications made with this 
  chain context.

  This function does _not_ copy secx509_cert_type* cert_ptr itself. The memory pointed 
  to must stay valid for the lifetime of the context.

  @param[out] context_ptr  secx509chain context to which the root certificate is added.
  @param[in] cert_ptr      Parsed root certificate data.
  
  @return
  E_SUCCESS -- A root cert is added to chain context successfully.\n
  E_INVALID_ARG -- Pointer argument is NULL or chain context met its root cert capability.\n

  @dependencies
  This function uses the secasn() and ossl_asn() functionality.
*/
secerrno_enum_type secx509chain_add_parsed_root_cert
(
  secx509chain_ctx_type* context_ptr,
  secx509_cert_type*     cert_ptr
);

/*=============================================================================
   FUNCTION : SECX509CHAIN_ADD_CERT
=============================================================================*/

/**
  Adds a certificate to the list to be verified.

  This function copy the certificate data buffer and parse it.
  secx509chain_clear_cert_list() must be called later in order to free the memory
  allocated by this function.


  @param[out] context_ptr  secx509chain context to which the certificate is 
                           added.
  @param[in] cert_ptr      Raw certificate data.
  @param[in] cert_len      Length of the raw certificate data.

  @return
  E_SUCCESS -- A cert is added to chain context successfully.\n
  E_DATA_INVALID -- A cert cannot be parsed properly. \n
  E_NOT_SUPPORTED -- An algorithm found in the cert is not supported
  E_NO_MEMORY -- System can't allocate memory to hold a cert.\n
  E_INVALID_ARG -- Pointer argument is NULL.\n
  E_FAILURE -- Parse of root cert failed.

  @dependencies
  This function uses the secasn() and ossl_asn() functionality.
*/
secerrno_enum_type secx509chain_add_cert
(
  secx509chain_ctx_type* context_ptr,
  uint8*                 cert_ptr,
  uint32                 cert_len
);

/*=============================================================================
   FUNCTION : SECX509CHAIN_CLEAR_CERT_LIST
=============================================================================*/

/**
  Frees all certificates listed in the current context except the root 
  certificates.

  @param[out] context_ptr Context for which the certificate list is freed.
  
  @return
  None.

  @dependencies
  This function uses the secasn() and ossl_asn() functionality.

  @sideeffects
  This function frees the memory pointed to by the certificate list elements.
*/
secerrno_enum_type secx509chain_clear_cert_list
(
  secx509chain_ctx_type* context_ptr
);

/*=============================================================================
   FUNCTION : SECX509CHAIN_CLEAR_ROOT_CERT_LIST
=============================================================================*/

/**
  Frees all the root certificate structures and the buffers holding the root 
  certificate.

  @param[out] context_ptr Context for which the root certificate list is freed.
  
  @return
  None.

  @dependencies
  None.

  @sideeffects
  This function frees the memory pointed to by the root certificate elements.
*/
secerrno_enum_type secx509chain_clear_root_cert_list
(
  secx509chain_ctx_type* context_ptr
);

/*=============================================================================
   FUNCTION : SECX509CHAIN_VALIDATE_CERT_LIST
=============================================================================*/

/**
  Parses a list of certificates.

  @param[out] context_ptr      Pointer to secx509chain_ctx_type.
  @param[in] current_time      Current time in seconds since the CDMA epoch time 
                               (January 6, 1980).
  @param[in] alert_mask_ptr    Alert mask for verification errors from 
                               secx509_verify_cert_list().
  @param[in] ignore_expiration Specifies whether the validity period is to be checked:\n
                               TRUE -- Ignore the validity period.\n
                               FALSE -- Do not ignore the validity period.
  
  @return
  E_SUCCESS -- Verification is successful.\n
  E_DATA_INVALID -- Certificate purpose checking failed.\n
  E_DATA_INVALID -- Certificate path length checking failed.\n
  E_DATA_EXPIRED -- Certificate is not yet valid.\n
  E_DATA_EXPIRED -- Certificate is expired.\n
  E_DATA_TOO_LARGE -- Verification failed; too many certificates in the chain.\n
  E _INVALID_ARG -- Pointer argument is set to NULL.

  @dependencies
  This function uses the secasn() and ossl_asn() functionality.
*/
secerrno_enum_type secx509chain_validate_cert_list
(
  secx509chain_ctx_type*  context_ptr,
  uint32                  current_time,
  uint32*                 alert_mask_ptr,
  boolean                 ignore_expiration
);

/*=============================================================================
   FUNCTION : SECX509CHAIN_CLEAR_CONTEXT
=============================================================================*/

/**
  Frees all memory associated with the context.

  @param[out] context_ptr  secx509chain context to clear.
  
  @return
   None.

  @dependencies
  This function uses the secasn() and ossl_asn() functionality.
*/
void secx509chain_clear_context
(
  secx509chain_ctx_type*  context_ptr
);

/** @} */  /* end_addtogroup secx509chain_apis */


#endif /* SECX509CHAIN_H */

#ifndef SECX509_H
#define SECX509_H

/** 
   @file secx509.h
   @brief Certificate parsing and verification.
   This file contains the definitions of the constants, data structures
   and interfaces that provide certificate parsing and verification.
**/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The secx509_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      secx509_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the secx509_apis group 
      description in the secx509_mainpage.dox file. 
===========================================================================*/ 
/*===========================================================================
  Copyright (c) 2002 - 2012 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/ 

/*==========================================================================
  Below is an example of how to parse a certificate, display and get the name
  of the certificate
  uint8 *cert_data                            // Unparsed certificate data
  uint16 cert_len                             // Length of the certificate
  secx509_cert_type *cert = NULL;             // Certificate pointer
  secerrno_enum_type err;
  uint8 name[NAME_SIZE];
  
  // Initialize the secx509 module
  err = secx509_init();
  
  if ( err != E_SUCCESS ) 
  {
    // Error has occurred
  }
  
  // Parse the certificate
  err = secx509_parse_certificate( cert_data, cert_len, &cert );
  
  if ( err == E_DATA_INVALID ) 
  {
    // Certificate has invalid data, handle error
  }
  else if ( err != E_SUCCESS ) 
  {
    // Error has occurred
  }
  
  // Verify the certificate against its own public key
  err = secx509_verify_cert( cert, &cert->pkey );
  
  if ( err != E_SUCCESS ) 
  {
    // Error has occurred
  }
    
  // Log the certificate to the F3 messages
  err = secx509_log_certificate( cert );
  
  if ( err != E_SUCCESS ) 
  {
    // Error has occurred
  }
  
  // Get the name of the certificate
  err = secx509_name_certificate( cert, &name, NAME_SIZE );
  
  if ( err != E_SUCCESS ) 
  {
    // Error has occurred
  }
  
  // Use the name of the certificate
  
  // Free the memory used by the certificate
  err = secx509_free_certificate( cert );
  
  if ( err != E_SUCCESS ) 
  {
    // Error has occurred
  }  

   
EXTERNALIZED FUNCTIONS

  secx509_init()                 Initializes certificate processing

  secx509_parse_certificate()    Parses an X.509 certificate

  secx509_log_certificate()      Displays a parsed certificate through 
                                 F3 Messages

  secx509_name_certificate()     Finds the name of a parsed certificate

  secx509_free_certificate()     Release memory occupied by the parsed
                                 certificate

  secx509_verify_certificate()   Verifies that a parsed certificate is
                                 valid by checking the content of fields

  secx509_verify_cert_list()     Verifies that a list of parsed certificates
                                 is valid

  secx509_process_cert_list()    Processes a lists of certificates, by
                                 parsing and verifying them
                                 
  secx509_process_certificate()  Process a single certificate, by parsing 
                                 and verifying it

INITIALIZATION AND SEQUENCING REQUIREMENTS

  secx509_init() must be called before using the other functions.

==========================================================================*/

/*==========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/x509/secx509.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $


when       who     what, where, why
--------   ---     ---------------------------------------------------------
01/05/11   sd      (Tech Pubs) Updated the copyright date and mainpage reference.
10/07/10   sd      (Tech Pubs) Edited/added Doxygen markup and comments.
07/09/10   ml      Deprecated secx509_process_cert_list() and added secx509_get_dn
06/18/10   yh      bring secx509_process_cert_list() back to header file
06/10/10   yh      ignore_expiration interface change
06/06/10   MSh     added mainpage, dscriptions for functions, constants and data types
                   to doxygenate API document.
01/25/07   df      fix for SR890833 Auth Key Id parsing bug
                   fix for verify cert chain bug
11/01/06   avm     Renamed secssl_ses_certificate_pdata_type;
                   to secx509_ses_certificate_pdata_type.
                   Included the ASN.1 definition structs
                   from: secx509asntypes.h.
                   Moved secx509_ses_certificate_pdata_type from secssl.h
10/30/06   rv      removed SEC_L4_SINGLE_PD
08/10/06   lx      Add current_time as an argument for time validity check
07/10/06   lx      Add SECX509_PURPOSE_OCSP_RSP and extension field
                   ocsp_nocheck_set
03/15/06   lx      Changes to support DRM certificate chain verification
06/11/05   sv/ssm  SSL changes to support EAP-TLS
11/12/04   rv      changes to correct LINT errors
10/21/03   jay     Cleaned up the file for better readibility
08/07/02   djb     initial creation

==========================================================================*/

/*==========================================================================

             Include Files for Module

==========================================================================*/
#include "comdef.h"
#include "secasn1.h"
#include "secapi.h"
#include "secx509asntypes.h"
#include "secerrno.h"

/*==========================================================================

             Local Constant Declaration

==========================================================================*/
/**
  @addtogroup secx509_apis
  @{
*/

/**
  @name Alert Mask Definitions
  The following alert masks can be returned by the SECX509 library.
@{
*/
#define SECX509_ALERT_BAD_CERTIFICATE            0x00000080   
  /**< Bad certificate. */
#define SECX509_ALERT_BAD_CERTIFICATE_SIGN       0x00000100   
  /**< Bad signature. */
#define SECX509_ALERT_BAD_CERTIFICATE_ISSUER     0x00000200   
  /**< Bad certificate issuer. */
#define SECX509_ALERT_UNSUPPORTED_CERTIFICATE    0x00000400   
  /**< Unsupported certificate. */
#define SECX509_ALERT_CERTIFICATE_REVOKED        0x00000800   
  /**< Certificate revoked. */
#define SECX509_ALERT_CERTIFICATE_EXPIRED        0x00001000   
  /**< Certificate expired. */
#define SECX509_ALERT_CERTIFICATE_UNKNOWN        0x00002000   
  /**< Unknown certificate. */
#define SECX509_ALERT_ILLEGAL_PARAMETER          0x00004000   
  /**< Illegal parameters in the certificate. */
#define SECX509_ALERT_DECODE_ERROR               0x00040000   
  /**< Decoding error. */
#define SECX509_ALERT_BAD_CERTIFICATE_NAME       0x00080000   
  /**< Bad certificate name. */
#define SECX509_ALERT_BAD_CERTIFICATE_SERIALNUM  0x00100000   
  /**< Bad certificate serial number. */

/** @} */  /* end_name_Alert Mask Definitions */

/**
  @name X.509 Operations
  The following defines provide the maximum supported operand byte size for 
  X.509 operations. 
  @{
*/
#define SECX509_MAX_DIGEST_SIZE   MAX( SECAPI_HSH_MD5_DIGEST_SIZE,  \
                                        SECAPI_HSH_SHA256_DIGEST_SIZE )
  /**< X.509 maximum digest size. */
#define SECX509_CERT_CHAIN_LEN_SIZ          3  
  /**< Certificate chain length size. */
#define SECX509_CERT_LEN_SIZ                3  
  /**< Certificate chain length size. */
#define SECX509_MAX_CERT_LEN                4000 
  /**< Maximum certificate length. */
#define SECX509_ASN1_MAX_HASH_OBJ_LEN       19 
  /**< Maximum hash object length. */
#define SECX509_MAX_VRFY_SIG_LEN      \
        ( SECX509_ASN1_MAX_HASH_OBJ_LEN + SECX509_MAX_DIGEST_SIZE )
  /**< Maximum signature length. */

#define SECX509_MAX_DER_ALGORITHM_ID_LEN    64 
  /**< Maximum DER algorithm length. */
#define SECX509_MAX_DER_PUBKEY_INFO_LEN     \
        ( SECAPI_PKX_MAX_KEY_SIZE * 2 + SECX509_MAX_DER_ALGORITHM_ID_LEN )
  /**< Maximum DER public key length. */
#define SECX509_MAX_STRING_LEN              256 
  /**< Maximum string length for the X.509 certificate. */

#define SECX509_NUM_CERTS           50 /**< Maximum number of CA certificates. */

#define SECX509_MD2_BLOCK           16 /**< X.509 MD2 block size. */
#define SECX509_MD2_DIGEST_LENGTH   16 /**< X.509 MD2 digest size. */
/** @} */  /* end_name_X.509 Operations */

/* #define SECX509_CERT_LIST_MAX_DEPTH     9 */
#define SECX509_CERT_LIST_MAX_DEPTH     ( SECX509_NUM_CERTS - 1 )
  /**< Certificate list depth for processing. */

#define SECX509_TRUST_SSL_SERVER        3 /**< SSL server signature. */

/**
@name Certificate Purpose Values
@{
*/
/** @cond 
*/
#define SECX509_PURPOSE_SSL_CLIENT      1  /**< Not supported. */
/** @endcond 
*/

#define SECX509_PURPOSE_SSL_SERVER      2  /**< SSL server certificate. */

/** @cond 
*/
#define SECX509_PURPOSE_NS_SSL_SERVER   3  /**< Not supported. */
#define SECX509_PURPOSE_SMIME_SIGN      4  /**< Not supported. */
#define SECX509_PURPOSE_SMIME_ENCRYPT   5  /**< Not supported. */
#define SECX509_PURPOSE_CRL_SIGN        6  /**< Not supported. */
#define SECX509_PURPOSE_ANY             7  /**< Not supported. */
/** @endcond 
*/

#define SECX509_PURPOSE_DRM_RI          8  /**< DRM rights issuer certificate. */
#define SECX509_PURPOSE_OCSP_RSP        9  /**< Online certificate status protocol 
                                                response. */
#define SECX509_PURPOSE_IKEV2           10 /**< Internet Protocol Security (IPSec)
                                                Internet Key Exchange Version 2. */
/** @} */  /* end_name_Certificate Purpose Values */

/** @} */  /* end_addtogroup secx509_apis */
/*==========================================================================

             Structure Definition

==========================================================================*/
/**
  @addtogroup secx509_apis
  @{
*/

/**
 Public key algorihms in the certificate.
*/
typedef enum
{
  SECX509_PUBKEY_RSA = 0, /**< RSA public key. */
  SECX509_PUBKEY_DSA = 1, /**< DSA public key. */
  SECX509_PUBKEY_DH  = 2, /**< DH public key. */
  SECX509_PUBKEY_MAX      /**< Last key; used for error checking. */

} secx509_pubkey_algo_type;

/**
 Certificate signature algorithm type.
*/
typedef enum
{
  SECX509_md5WithRSAEncryption    = 0, /**< MD5 hash and an RSA public key. */
  SECX509_md2WithRSAEncryption    = 1, /**< MD2 hash and an RSA public key. */
  SECX509_sha1WithRSAEncryption   = 2, /**< SHA-1 hash and an RSA public key. */
  SECX509_sha256WithRSAEncryption = 3, /**< SHA256 hash and an RSA public key. */
  SECX509_SIG_ALGO_MAX                 /**< Last key, for error checking. */

} secx509_sig_algo_type;

/**
  Relative distinguished name types.
*/
typedef enum
{
   SECX509_ISSUER             = 1,              
     /**< Select issuer distinguished name. */
   SECX509_SUBJECT            = 2,              
     /**< Select object distinguished name. */
   SECX509_TARGET_RESERVED    = 0x7FFFFFFF      
     /**< Default behavior. */
} secx509_dn_target_type;

/**
  Distinguished name attribute type.
*/
typedef enum
{
   SECX509_NAME_TEXT             = 1,  /**< Entire comma-separated name text. */
   SECX509_COMMON_NAME           = 2,  /**< Common name for the certificate. */
   SECX509_COUNTRY               = 3,  /**< Country name for the certificate. */
   SECX509_STATE                 = 4,  /**< State or province name for the 
                                            certificate. */
   SECX509_LOCATION              = 5,  /**< Locality name for the certificate. */
   /** @cond 
   */
   SECX509_STREET                = 6,  /**< Not supported. */
   /** @endcond 
   */
   SECX509_ORGANIZATION          = 7,  /**< Organization name for the 
                                            certificate. */
   /** @cond 
   */
   SECX509_TITLE                 = 8,  /**< Not supported. */
   /** @endcond 
   */
   SECX509_SUBJECT_ALT_NAME      = 9,  /**< Subject alternate name for the 
                                            certificate. */
   /** @cond 
   */
   SECX509_INITIALS              = 10, /**< Not supported. */
   SECX509_GENERATIONQUALIFIER   = 11, /**< Not supported. */
   SECX509_DNQUALIFIER           = 12, /**< Not supported. */
   SECX509_EMAILADDRESS          = 13, /**< Not supported. */
   /** @endcond 
   */
   SECX509_ORG_UNIT_1            = 14, /**< Organizational unit name for the 
                                            certificate. */
   /** @cond 
   */
   SECX509_ORG_UNIT_2            = 15, /**< Not supported. */
   SECX509_ORG_UNIT_3            = 16, /**< Not supported. */
   SECX509_ORG_UNIT_4            = 17, /**< Not supported. */
   SECX509_ORG_UNIT_5            = 18, /**< Not supported. */
   SECX509_ORG_UNIT_6            = 19, /**< Not supported. */
   SECX509_ORG_UNIT_7            = 20, /**< Not supported. */
   /** @endcond 
   */
   SECX509_SERIAL_NUM            = 21, /**< Serial number for the certificate. */
   SECX509_VERSION               = 22, /**< Version for the certificate. */
   SECX509_NOT_VALID_BEFORE      = 23, /**< Validity start for the certificate. */
   SECX509_NOT_VALID_AFTER       = 24, /**< Validity end for the certificate. */
   SECX509_DN_ALL_ATTR           = 0x7FFFFFFF 
                                       /**< Entire colon-separated distinguished 
                                            name. */
} secx509_dn_attr_type;


/** @} */  /* end_addtogroup_secx509_apis */

/**
  @addtogroup secx509_apis
  @{
*/

/**
  @brief Rivest, Shamir and Adleman (RSA) algorithm public key parameters.
*/
typedef struct
{
  uint16  mod_len;    /**< Modulus length.  */
  uint8*  mod_data;   /**< Modulus data. */
  uint16  exp_e_len;  /**< Exponent length. */
  uint8*  exp_e_data; /**< Exponent data. */

} secx509_rsa_pubkey_type;

/**
  @brief Digital Signature Algorithm (DSA) public key parameters.
*/
typedef struct
{
  uint16  dummy_len; /**< DSA public key length. */
  uint8*  dummy;     /**< DSA public key data. */

} secx509_dsa_pubkey_type;

/**
  @brief Diffie-Hellman (DH) algorithm public key parameters.
*/
typedef struct
{
  uint16  mod_len;   /**< Modulus length. */
  uint8*  mod_data;  /**< Modulus data. */
  uint16  exp_len;   /**< Exponent length. */
  uint8*  exp_data;  /**< Exponent data. */

} secx509_dh_pubkey_type;

/** @} */  /* end_addtogroup secx509_apis */

/**
  @addtogroup secx509_apis
  @{
*/

/**
  @brief Union of all the public key types.
*/
typedef struct
{
  secx509_pubkey_algo_type  algo; /**< Public key algorithm: RSA, DSA, DH. */

  /** Key value holder. */
  union
  {
    secx509_rsa_pubkey_type  rsa;  /**< RSA algorithm. */
    secx509_dsa_pubkey_type  dsa;  /**< Digital signature algorithm. */
    secx509_dh_pubkey_type   dh;   /**< Diffie-Hellman algorithm. */

  }key; 

} secx509_pubkey_type;

/** @} */  /* end_addtogroup secx509_apis */

/**
  @addtogroup secx509_apis
  @{
*/

/**
  @brief Signature structure.
*/
typedef struct
{
  secx509_sig_algo_type  algo_id; /**< Certificate signature algorithm. */
  secasn1_data_type      val;     /**< Certificate signature raw data. */

} secx509_signature_type;

/**
  @brief Distinguished name structure.
*/
typedef struct
{
  uint8              num_attrib; /**< Number of attributes inside this distinguished name. */
  secasn1_data_type  data;       /**< Certificate distinguished name raw data. */

} secx509_dn_type;

/**
  @brief Version structure.
*/
typedef struct
{
  uint8              ver; /**< Certificate version. */
  secasn1_data_type  val; /**< Certificate version raw data. */

} secx509_version_type;

/**
  @brief Time structure.
*/
typedef struct
{
  uint32             time; /**< Certificate time. */
  secasn1_data_type  data; /**< Certificate time raw data. */

} secx509_time_type;

/**
  @brief Authority key identifier structure.
*/
typedef struct
{
  boolean            set;           /**< Issuer unique identifier value set flag. */
  secasn1_data_type  key_id;        /**< Issuer key ID. */
  secx509_dn_type    name;          /**< Issuer distinguished name. */
  secasn1_data_type  serial_number; /**< Issuer serial number. */

} secx509_auth_key_id_type;

/**
  @brief Subject key identifier structure.
*/
typedef struct
{
  boolean            set;      /**< Subject unique identifier value set flag.  */
  secasn1_data_type  key_id;   /**< Subject key ID. */

} secx509_subject_key_id_type;

/**
  @brief Key usage structure.
*/
typedef struct
{
  uint16   val;   /**< Key usage value; see RFC 2459. */
  boolean  set;   /**< Key usage value set flag. */

} secx509_key_usage_type;

/**
  @brief Certificate Authority (CA) structure.
*/
typedef struct
{
  boolean  set;   /**< CA cerificate value set flag. */
  boolean  val;   /**< Indicates whether the type is a CA. */

} secx509_ca_type;

/**
  @brief Extension structure type, introduced in Security X.509 version 3.
*/
typedef struct
{
  boolean                      set;              /**< Indicates whether the 
                                                      extension type is set. */
  secx509_auth_key_id_type     auth_key_id;      /**< Authority key identifier. */
  secx509_subject_key_id_type  subject_key_id;   /**< Subject key identifier. */
  secx509_key_usage_type       key_usage;        /**< Key useage. */
  secx509_key_usage_type       ex_key_usage;     /**< Extended key usage field. */
  int                          path_len;         /**< Only CA certificates can 
                                                      have a path length. */
  secx509_ca_type              ca;               /**< CA certificate flag.  */
  boolean                      ocsp_nocheck_set; /**< Online certificate status 
                                                      protocol check flag. */

} secx509_ext_type;

/**
  @brief DER-encoded public key information of the subject used for Open Mobile 
  Alliance Digital Rights Management (OMA-DRM).
*/
typedef struct
{
  uint8   data[SECX509_MAX_DER_PUBKEY_INFO_LEN]; /**< Certificate data buffer. */
  uint16  len;                                   /**< Certificate length. */
} secx509_der_pubkey_info_type;


/**
  @brief Certificate information structure.
*/
typedef struct
{
  secx509_version_type     version;           
    /**< Certificate version. */
  secasn1_data_type        serial_number;     
    /**< Certificate serial number. */
  secx509_signature_type   algorithm;         
    /**< Signature algorithm of the certificate. */
  secx509_dn_type          issuer;            
    /**< Issuer of the certificate. */
  secx509_time_type        not_before;        
    /**< Certificate validity begin time. */
  secx509_time_type        not_after;         
    /**< Certificate validity end time. */
  secx509_dn_type          subject;           
    /**< Subject name of the certificate. */
  secx509_pubkey_type      pub_key;           
    /**< Public key information of the certificate. */
  secasn1_bit_string_type  issuer_unique_id;  
    /**< Unique identification of the certificate issuer. */
  secasn1_bit_string_type  subject_unique_id; 
    /**< Unique identification of the certificate subject. */
  secx509_ext_type         extension;         
    /**< Extensions in the certificate. */
  secasn1_data_type        subject_alt_name;  
    /**< Subject name of the certificate. */

} secx509_cert_info_type;

/**
  @brief Certificate structure.
*/
typedef struct
{
  secx509_cert_info_type  cert_info; /**< secx509 certificate information. */

  /* Signature info on the cert */
  secx509_pubkey_type     pkey;          /**< Public key. */
  secx509_sig_algo_type   sig_algo;      /**< Signature algorithm. */
  uint32                  cinf_offset;   /**< Offset of cert_info. */
  uint16                  cinf_byte_len; /**< Length of cert_info. */
  uint8                   sig[SECX509_MAX_VRFY_SIG_LEN]; /**< Signature data. */
  uint16                  sig_len;       /**< Signature data length. */

  secx509_der_pubkey_info_type pubkey_info; 
    /**< DER-encoded subject public key information used for the OMA-DRM only. */


  boolean                 __used; 
    /**< Bookkeeping to indicate whether pubkey_info is used for the OMA-DRM. */

} secx509_cert_type;

/**
  @brief Certificate list structure.
*/
typedef struct secx509_cert_list_struct
{
  secx509_cert_type*  cert[SECX509_NUM_CERTS]; 
    /**< Certificate list data holder. */
  uint8               size;                    
    /**< Number of certificates in the list. */

} secx509_cert_list_type;

/**
  @brief Certificate list context type.
*/
typedef struct
{
  int                      purpose;  /**< Purpose of the certificate validation 
                                          (SSL, IPSec, DRM). */
  int                      trust;    /**< Trust anchor index. */
  uint16                   depth;    /**< Depth of the chained certificate. */
  secx509_cert_list_type*  ca_list;  /**< Certificate list. */

} secx509_cert_ctx_type;

/**
  @brief Message-digest algorithm 2 (MD2) context type structure.
*/
typedef struct
{
  uint16  num;                      /**< Index to the array. */
  uint8   data[SECX509_MD2_BLOCK];  /**< MD2 data holder. */
  int     cksm[SECX509_MD2_BLOCK];  /**< MD2 checksum data holder. */
  int     state[SECX509_MD2_BLOCK]; /**< MD2 state data holder. */

} secx509_md2_ctx_type;

/**
  @brief Certificate information using the ASN.1 standard.
*/
typedef struct
{
  SECASN1_INTEGER  version;          /**< Version number.      */
  SECASN1_INTEGER  serial_no;        /**< Serial number.      */
  SECASN1_STRING   issuer;           /**< Issuer name.         */
  SECASN1_UTCTIME  not_before;       /**< Validity start date. */
  SECASN1_UTCTIME  not_after;        /**< Validity end date.   */
  SECASN1_STRING   object_organization; /**< Object organization value. */
  SECASN1_STRING   object_common_name;  /**< Object common name value. */
  SECASN1_STRING   subject_alt_name; /**< Subject Alternate name value */
} secx509_ses_certificate_pdata_type;

/** @} */  /* end_addtogroup secx509_apis */
 /*  End of Data Structures */

/*==========================================================================

                      PUBLIC FUNCTION DECLARATIONS

==========================================================================*/

/**
   @addtogroup secx509_apis
   @{ */

/**
  Initializes the X.509 library.

  This function initializes the global data structures for use. It initializes 
  the certificate buffer and the certificate list along with the context structure. 

  There is no callback function invocation upon the completion of this operation.

  @return
  E_SUCCESS -- Request was successful. \n
  E_FAILURE -- Initialization of the library failed.

  @dependencies
  Must be called before any functions are called.

  @sideeffects
  If the function is called for a second time, any other applications holding
  a certificate using the certificate buffer are wiped out.
*/
extern secerrno_enum_type secx509_init( void );

/**
  Parses an X.509 certificate from data_ptr, if cert equals NULL. The 
  certificate is then allocated from the memory pool. If the certificate is
  already allocated, the function uses the pre-allocated memory.

  @param[in] cert_data Pointer to the raw certificate data.
  @param[in] cert_len  Length of the certificate data.
  @param[in,out] cert  Pointer to the certificate.

  @return
  E_SUCCESS       -- Certificate is parsed properly. \n
  E_DATA_INVALID  -- Certificate cannot be parsed properly. \n
  E_NO_MEMORY     -- No more memory slots are available for the certificates. \n
  E_NOT_SUPPORTED -- Algorithm found in the certificate is not supported. \n
  E_INVALID_ARG   -- Pointer argument is NULL. \n
  E_FAILURE       -- Cert parameter is pre-allocated, but not the cert field in 
                     the cert parameter; or the certificate data length does 
                     not match cert_len.

  @dependencies
  None.
*/
extern secerrno_enum_type secx509_parse_certificate
(
  uint8                         *cert_data,
  uint16                        cert_len,  
  secx509_cert_type             **cert     
);

/**
  Gets the subject name for a certificate. The certificate name is decided as 
  follows:

  -# If the subject's organization name is valid, the name is the
     organization's name.
  -# Otherwise, if the common name is valid, the name is the common name.
  -# Otherwise, return the subject name in one line.

  This function parses an X.509 certificate from the data pointer. If cert_ptr equals NULL, 
  the certificate is allocated from the memory pool. If the certificate is
  already allocated, the function uses the pre-allocated memory.

  @param[in] cert_ptr Pointer to the certificate to be named.
  @param[in] buf      Location to place the certificate name.
  @param[in] len      Maximum length of the buffer for the certificate name.

  @return
  E_SUCCESS -- Valid name. \n
  E_FAILURE -- Otherwise.

  @dependencies
  None.
*/
extern secerrno_enum_type secx509_name_certificate
(
  secx509_cert_type     *cert_ptr,
  char                  *buf,
  uint16                len
);

/**
  Logs certificate information.

  @param[in] cert_ptr Pointer to the certificate to be printed.

  @return
  E_SUCCESS -- Function executes successfully. \n
  E_FAILURE -- Otherwise.

  @dependencies
  This function uses MSG_STR_LOW to print out the string information. This is 
  very CPU intensive and is used only when absolutely necessary.
*/
extern secerrno_enum_type secx509_log_certificate
(
  secx509_cert_type     *cert_ptr
);

/**
  Verifies the certificate signature  by decrypting the digital signature
  and comparing it with the calculated hashing over the signed certificate
  information.

  @param cert_ptr Pointer to the certificate to be verified.
  @param pkey_ptr Pointer to the public key.

  @return
  E_SUCCESS       -- Certificate verification was successful. \n
  E_DATA_INVALID  -- Verification failed. \n
  E_NOT_SUPPORTED -- Algorithm is not supported. \n
  E_INVALID_ARG   -- Either argument pointer is NULL. \n
  E_FAILURE       -- Otherwise.

  @dependencies
  None.
*/
extern secerrno_enum_type secx509_verify_certificate
(
  secx509_cert_type             *cert_ptr,
  secx509_pubkey_type           *pkey_ptr
);

/**
  Validates and parses a single certificate. This function checks the validity 
  time, and, if it is a self-signed certificate, verifies the certificate with 
  its own key. If successful, a valid, parsed certificate is returned.

  @param[in] cert_data         Pointer to the raw certificate data.
  @param[in] cert_len          Length of the certificate data.
  @param[in] cert              Certificate pointer.
  @param[in] current_time      Current time in seconds since the CDMA epoch
                               time (January 6, 1980).
  @param[in] ignore_expiration Specifies whether the time validity of the 
                               certificate is to be checked. \n
                               TRUE -- Do not check. \n
                               FALSE -- Check.

  @return
  E_SUCCESS     -- Certificate verification was successful. \n
  E_INVALID_ARG -- Pointer argument is NULL. \n
  E_NO_DATA     -- Certificate cannot be verified because the phone has no 
                   system time. \n
  E_FAILURE     -- Otherwise.

  @dependencies
  None.
*/
extern secerrno_enum_type secx509_process_certificate
(
  uint8                 *cert_data,
  uint16                cert_len,
  secx509_cert_type     **cert,
  uint32                current_time,
  boolean               ignore_expiration
);

/**
  Validates a list of certificates.

  This function starts from the server's certificate and verifies each 
  certificate until the function reaches the root certificate or the 
  certificate from the CA. The verification involves checking the depth of the 
  certificate, the purpose, the trust, the validity, and finally verifying the 
  signature of the certificate.

  @param[in] cert_list               List of parsed certificates to be 
                                     verified.
  @param[in] ctx                     Certificate list context.
  @param[in] current_time            Current time in seconds since the CDMA 
                                     epoch time (January 6, 1980).
  @param[in] security_alert_mask_ptr Alert mask for verification errors.
  @param[in] ignore_expiration       Specifies whether the time validity of the 
                                     certificate must be checked. \n
                                     TRUE -- Do not check. \n
                                     FALSE -- Check.

  @return
  E_SUCCESS        -- Verification is successful. \n
  E_DATA_INVALID   -- Certificate checking failed for the purpose or path length. \n
  E_DATA_EXPIRED   -- Certificate is not valid yet or has expired. \n
  E_DATA_TOO_LARGE -- Verification failed because of too many certificates in the 
                      chain. \n
  E_INVALID_ARG    -- Pointer argument is set to NULL.

  @dependencies
  None.
*/
extern secerrno_enum_type secx509_verify_cert_list
(
  secx509_cert_list_type        *cert_list,
  secx509_cert_ctx_type         *ctx,
  uint32                        current_time,
  uint32                        *security_alert_mask_ptr,
  boolean                       ignore_expiration
);

/**
  This function is deprecated. Use the X.509 chain interface to process a 
  certificate list.

  Processes a certificate list.

  This function processes a certificate list by parsing the certificate
  from the server up to the root and verifying that each certificate 
  is signed properly by either the CA or the issuing certificate. 

  @param[in] msg_ptr                 Pointer to the message containing the 
                                     certificate list.
  @param[in] msg_len                 Length of the certificate list message.
  @param[out] pubkey                 Server's public key.
  @param[out] srv_cert               Server certificate.
  @param[in] security_alert_mask_ptr Alert mask for processing errors.
  @param[in] ignore_expiration       Specifies whether the time validity of the certificate is 
                                     to be checked. \n
                                     TRUE -- Do not check. \n
                                     FALSE -- Check.
  @return
  E_DATA_INVALID -- Certificate is not valid. \n
  E_DATA_TOO_LARGE -- Certificate is too large for the buffer. \n
  E_FAILURE -- Failed to process the certificate list due to other reasons. \n
  E_INVALID_ARG -- Invalid argument is passed in. \n
  E_SUCCESS -- Processing is successful; this function also returns: \n
  - The public key from the server certificate. 
  - Simple information from the server certificate. 

  @dependencies
  This function frees the certificate data memory from the Data Services Memory (DSM) item
  and places it in secx509_cert_chain_buf. \n 
  This memory is also erased when secx509_cert_list_init() is called at cleanup.
*/
extern secerrno_enum_type secx509_process_cert_list
(
  dsm_item_type**                     msg_ptr,
  uint16                              msg_len,
  secx509_cert_ctx_type*              custom_ctx_ptr,
  secx509_pubkey_type*                pubkey,
  secx509_ses_certificate_pdata_type* srv_cert,
  uint32*                             security_alert_mask_ptr,
  uint32                              current_time,
  boolean                             ignore_expiration
);

/**
  Frees the certificate structure. The certificate memory is returned to the 
  buffer by adding it to the end of the secx509_free_cert_list list. The end 
  marker of the list, secx509_free_cert_end, is incremented. 

  When the certificate is successfully freed, the number of certificates 
  allocated (secx509_num_cert_alloc) is decremented.

  @param[in] cert_ptr Pointer to the certificate being freed.

  @return
  E_SUCCESS -- Certificate was successfully freed. \n
  E_FAILURE -- Error freeing the certificate; e.g., the cert_ptr points to a 
               certificate that is not allocated, or the cert_ptr points to 
               memory that is not in the certificate buffer.

  @dependencies
  None.
*/
extern secerrno_enum_type secx509_free_certificate
(
  secx509_cert_type             *cert_ptr
);

/**
  Gets the relative distinguished name.

  Provides an interface to a certificate distinguished name.
   
  @param[in] cert      Pointer to the parsed certificate structure.
  @param[in] dn_type   Type of distinguished name (i.e. issuer or subject)
  @param[in] dn_attr   Attribute to retrieve based on dn_type. If
                       SECX509_DN_ALL_ATTR is used, a colon-separated list of 
                       all attributes is returned up to len.
  @param[out] attr     Pointer to which the attribute type is copied. This is a
                       NULL-terminated string with a length of attr_len minus 1, at most. 
                       Data beyond the size of the buffer is truncated.
  @param[in] attr_len  Length of the data plus 1 for NULL character. Data is truncated
                       to attr_len minus 1, at most.
  
  @return
  E_SUCCESS     -- A X509 certificate¡¯s Distinguished Name attribute value is found. \n
  E_INVALID_ARG -- Bad arguments are passed. \n
  E_FAILURE     -- Otherwise.

  @dependencies
  Requires a certificate parsed by secx509_parse_certificate to get meaningful data.
*/
extern secerrno_enum_type secx509_get_dn
(
  secx509_cert_type*      cert,
  secx509_dn_target_type  dn_type,
  secx509_dn_attr_type    dn_attr,
  uint8*                  attr,
  uint16                  attr_len
);

/** @} */  /* end_addtogroup_secx509_apis */

#endif /* #ifdef SECX509_H */

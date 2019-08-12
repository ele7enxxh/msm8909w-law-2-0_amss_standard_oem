#ifndef SECSSLDEF_H
#define SECSSLDEF_H

/*===========================================================================*/
  
/**  
  @file secssldef.h
  @brief Contains all SSL v3 types and definitions derived from the Netscape
  specification and TLS values from RFC2246 to The TLS Protocol Version 1.0.

  All specification-derived types and definitions start with ssl_v3_... to
  distinguish them from implementation-specific types and definitions starting
  with secssl_....
 */
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The SECSSL_API_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the SECSSL_API_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "ssl_api" group 
      description in the SECSSL_API_mainpage.dox file. 
===========================================================================*/
/*=============================================================================
    Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
    Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
/*=============================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/COMMON/vcs/secssldef.h_v   1.15   03 Sep 2003 11:08:22   omichael  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/sec/api/ssl/secssldef.h#1 $ $DateTime: 2016/12/13 07:58:24 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/11/15   nm      TLSv1.2 support
06/22/15   nm      Removing support for Insecure CS 
01/21/11   lo      (Tech Pubs) Edited/added Doxygen comments and markup.
07/15/10   lo      (Tech Pubs) Edited/added Doxygen comments and markup.
06/24/05   ssm     Bug fixes for EAP-TLS
06/11/05   sv/ssm  Added support for EAP-TLS
01/14/03   om      Removed commented code (obsolete).
11/20/02   rwh     Changed SSL_V3_HDR_SPACE value to get 32-bit alignment.
06/04/02   om      Added SSL_V3_SVR_KEY_X_SIG_SIZE definition.
05/17/02   rwh     Added SSL_V3_MAC_MAX_LENGTH to add to the spec-compliant
                   SSL_V3_MAX_RECORD_LENGTH.
02/12/02   rwh     Increased value of SSL_V3_MAX_RECORD_LENGTH to be 
                   spec-compliant.
10/23/00   om      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "ps_in.h"
/**
@addtogroup cc
@{ 
*/

/* All values are in network order */
#define SSL_V3_CS_NULL_WITH_NULL_NULL             ps_htons( 0x0000 )
#define SSL_V3_CS_RSA_WITH_NULL_MD5               ps_htons( 0x0001 )
#define SSL_V3_CS_RSA_WITH_NULL_SHA               ps_htons( 0x0002 )
#define SSL_V3_CS_RSA_WITH_RC4_128_MD5            ps_htons( 0x0004 )
#define SSL_V3_CS_RSA_WITH_RC4_128_SHA            ps_htons( 0x0005 )
#define SSL_V3_CS_RSA_WITH_DES_CBC_SHA            ps_htons( 0x0009 )
#define SSL_V3_CS_DH_RSA_WITH_DES_CBC_SHA         ps_htons( 0x000f )
#define SSL_V3_CS_DHE_RSA_WITH_DES_CBC_SHA        ps_htons( 0x0015 )
#define SSL_V3_CS_DH_ANON_WITH_RC4_128_MD5        ps_htons( 0x0018 )
#define SSL_V3_CS_DH_ANON_WITH_DES_CBC_SHA        ps_htons( 0x001a )
#define SSL_V3_CS_CLIENT_PENDING_2_ACTIVE         ps_htons( 0xfffe )
#define SSL_V3_CS_SERVER_PENDING_2_ACTIVE         ps_htons( 0xffff )

/* rfc 3268 ciphersuites */
#define TLS_RSA_WITH_AES_128_CBC_SHA              ps_htons( 0x002F )
 /**< Supported. */
#define TLS_DH_DSS_WITH_AES_128_CBC_SHA           ps_htons( 0x0030 )
#define TLS_DH_RSA_WITH_AES_128_CBC_SHA           ps_htons( 0x0031 )
#define TLS_DHE_DSS_WITH_AES_128_CBC_SHA          ps_htons( 0x0032 )
#define TLS_DHE_RSA_WITH_AES_128_CBC_SHA          ps_htons( 0x0033 )
 /**< Supported by SecSSL client session only. */
#define TLS_DH_ANON_WITH_AES_128_CBC_SHA          ps_htons( 0x0034 )
#define TLS_RSA_WITH_AES_256_CBC_SHA              ps_htons( 0x0035 )
/**< Supported by SecSSL client session only. */
#define TLS_DH_DSS_WITH_AES_256_CBC_SHA           ps_htons( 0x0036 )
#define TLS_DH_RSA_WITH_AES_256_CBC_SHA           ps_htons( 0x0037 )
#define TLS_DHE_DSS_WITH_AES_256_CBC_SHA          ps_htons( 0x0038 )
#define TLS_DHE_RSA_WITH_AES_256_CBC_SHA          ps_htons( 0x0039 )
/**< Supported by SecSSL client session only. */
#define TLS_DH_anon_WITH_AES_256_CBC_SHA          ps_htons( 0x003A )
#define SSL_V3_CS_RSA_WITH_3DES_CBC_SHA           ps_htons( 0x000a )
 /**< Supported. */
#define SSL_V3_CS_DH_RSA_WITH_3DES_CBC_SHA        ps_htons( 0x0010 )
#define SSL_V3_CS_DHE_RSA_WITH_3DES_CBC_SHA       ps_htons( 0x0016 )
 /**< Supported by SecSSL client session only. */
#define SSL_V3_CS_DH_ANON_WITH_3DES_CBC_SHA       ps_htons( 0x001b )

#define TLS_RSA_WITH_AES_128_CBC_SHA256           ps_htons( 0x003C )
/**< Supported by SecSSL client session only. */
#define TLS_RSA_WITH_AES_256_CBC_SHA256           ps_htons( 0x003D )
/**< Supported by SecSSL client session only. */
#define TLS_DHE_RSA_WITH_AES_128_CBC_SHA256       ps_htons( 0x0067 )
/**< Supported by SecSSL client session only. */
#define TLS_DHE_RSA_WITH_AES_256_CBC_SHA256       ps_htons( 0x006B )
/**< Supported by SecSSL client session only. */

/**
@}
*/
/* end_addtogroup cc */

/** @cond
*/
/**
@addtogroup hpd
@{ 
*/

/* SSL header size */
#define SSL_V3_HDR_SIZE             5   /**< SSL header size in bytes. */
#define SSL_V3_HDR_SPACE            8   /**< SSL header size for 32-bit
                                             alignment. */

/* SSL common session data sizes */
#define SSL_V3_CLIENT_RANDOM_SIZE     32 /**< Common session client random data
                                              size. */
#define SSL_V3_SERVER_RANDOM_SIZE     32 /**< Common session server random data
                                              size. */
#define SSL_V3_PREMASTER_SECRET_SIZE  48 /**< Common session premaster secret
                                              data size. */
#define SSL_V3_MASTER_SECRET_SIZE     48 /**< Common session master secret data
                                              size. */

/* SSL version definitions */
#define SSL_V1_MAJOR                 1   /**< SSL version 1.0 major. */
#define SSL_V1_MINOR                 0   /**< SSL version 1.0 minor. */
#define SSL_V2_MAJOR                 2   /**< SSL version 2.0 major. */
#define SSL_V2_MINOR                 0   /**< SSL version 2.0 minor. */
#define SSL_V3_MAJOR                 3   /**< SSL version 3.0 major. */
#define SSL_V3_MINOR                 0   /**< SSL version 3.0 minor. */
#define SSL_TLS_V1_MAJOR             3   /**< TLS version 1.0 major. */
#define SSL_TLS_V1_MINOR             1   /**< TLS version 1.0 minor. */
#define SSL_TLS_V2_MINOR             2   /**< TLS version 1.1 minor. */
#define SSL_TLS_V3_MINOR             3   /**< TLS version 1.2 minor. */

/** SSL sub-protocols.
*/
typedef enum
{
  SSL_V3_CIPHSPEC_PROT_ID    = 20,   /**< SSL change cipherspec protocol. */
  SSL_V3_ALERT_PROT_ID       = 21,   /**< SSL alert protocol. */
  SSL_V3_HANDSHAKE_PROT_ID   = 22,   /**< SSL handshake protocol. */
  SSL_V3_APPLICATION_DATA_ID = 23    /**< SSL application data. */
} ssl_v3_protocol_id_enum_type;

#define SSL_V3_HSP_MSG_HEADER_SIZE       4
#define SSL_V3_HSP_MSG_TYPE_SIZE         1
#define SSL_V3_HSP_MSG_LENGTH_SIZE       3

#define SSL_V3_SESSION_ID_LEN_SIZE       1
#define SSL_V3_SESSION_ID_MAX_LEN       32
#define SSL_V3_CIPHERSUITE_ID_LEN_SIZE   2
#define SSL_V3_CIPHERSUITE_ID_SIZE       2
#define SSL_V3_COMPRESSION_ID_LEN_SIZE   1
#define SSL_V3_COMPRESSION_ID_SIZE       1
#define SSL_V3_PUB_KEY_LEN_SIZE          2

#define SSL_V3_CERT_CHAIN_LEN_SIZE       3

#define SSL_V3_SVR_KEY_X_SIG_SIZE       36

#define SSL_V3_HSP_CLIENT_ROLE_TOKEN     { 0x43, 0x4c, 0x4e, 0x54 }
#define SSL_V3_HSP_SERVER_ROLE_TOKEN     { 0x53, 0x52, 0x56, 0x52 }

#define TLS_V1_MASTER_LABEL_SIZE        13
#define TLS_V1_KEYEXP_LABEL_SIZE        13
#define TLS_V1_CLIENT_LABEL_SIZE        15
#define TLS_V1_SERVER_LABEL_SIZE        15
#define TLS_V1_VERIFY_DATA_SIZE         12
#define TLS_V12_VERIFY_DATA_SIZE        12
#define TLS_V12_HELLO_EXT_ALGO_SIZE      2
#define TLS_V12_HELLO_EXT_ALGO_LEN_SIZE  2

/* RFC4366 TLS extensions */
#define TLS_V1_HELLO_EXT_LEN_SIZE   2
#define TLS_V1_HELLO_EXT_HDR_SIZE   4
#define TLS_V1_HELLO_EXT_SERVER_NAME_LIST_LEN_SIZE 2
#define TLS_V1_HELLO_EXT_SERVER_NAME_TYPE_SIZE 1
#define TLS_V1_HELLO_EXT_SERVER_HOSTNAME_LEN_SIZE  2

/* Message ID definitions */
typedef enum
{
  SSL_V3_HSP_HELLO_REQUEST        =  0,
  SSL_V3_HSP_CLIENT_HELLO         =  1,
  SSL_V3_HSP_SERVER_HELLO         =  2,
  SSL_V3_HSP_CERTIFICATE          = 11,
  SSL_V3_HSP_SERVER_KEY_EXCHANGE  = 12,
  SSL_V3_HSP_CERTIFICATE_REQUEST  = 13,
  SSL_V3_HSP_SERVER_HELLO_DONE    = 14,
  SSL_V3_HSP_CERTIFICATE_VERIFIY  = 15,
  SSL_V3_HSP_CLIENT_KEY_EXCHANGE  = 16,
  SSL_V3_HSP_FINISHED             = 20,
  SSL_V3_ALP_MESSAGE              = 0xFF 
} ssl_v3_hsp_msg_id_enum_type;

/* RFC4366 TLS hello extension definitions.
*/
typedef enum 
{
  TLS_V1_HELLO_EXT_SERVER_NAME            = 0, 
  TLS_V1_HELLO_EXT_MAX_FRAGMENT_LENGTH    = 1,
  TLS_V1_HELLO_EXT_CLIENT_CERTIFICATE_URL = 2,
  TLS_V1_HELLO_EXT_TRUSTED_CA_KEYS        = 3,
  TLS_V1_HELLO_EXT_TRUNCATED_HMAC         = 4,
  TLS_V1_HELLO_EXT_STATUS_REQUEST         = 5,
  TLS_v1_HELLO_EXT_SIG_HASH_ALGO          =13,
  TLS_V1_HELLO_EXT_MAX                    = 0xFFFF
} tls_v1_hello_ext_id_enum_type;

typedef enum
{
  TLS_V12_HELLO_EXT_HASH_NONE     = 0,
  TLS_V12_HELLO_EXT_HASH_MD5      = 1, 
  TLS_V12_HELLO_EXT_HASH_SHA1     = 2, 
  TLS_V12_HELLO_EXT_HASH_SHA224   = 3,
  TLS_V12_HELLO_EXT_HASH_SHA256   = 4,
  TLS_V12_HELLO_EXT_HASH_SHA384   = 5,
  TLS_V12_HELLO_EXT_HASH_SAH512   = 6,
  TLS_V12_HELLO_EXT_HASH_MAX      = 255
} tls_v12_hello_ext_hash_enum_type;

typedef enum
{
  TLS_V12_HELLO_EXT_SIG_ANON     = 0,
  TLS_V12_HELLO_EXT_SIG_RSA      = 1,
  TLS_V12_HELLO_EXT_SIG_DSA      = 2,
  TLS_V12_HELLO_EXT_SIG_ECDSA    = 3,
  TLS_V12_HELLO_EXT_SIG_MAX      = 255
}tls_v12_hello_ext_signature_enum_type;


/**
@}
*/
/* end_addtogroup hpd */

/**
@addtogroup cpd
@{ 
*/

#define SSL_V3_CSP_MSG_TYPE_SIZE      1
#define SSL_V3_CSP_MSG_LENGTH_SIZE    3

/**
@}
*/
/* end_addtogroup cpd */

/**
@addtogroup apd
@{ 
*/

#define SSL_V3_ALP_ALERT_LEVEL_SIZE   1
#define SSL_V3_ALP_ALERT_ID_SIZE      1

/** Alert level definitions.
*/
typedef enum
{
  SSL_V3_ALP_WARNING  = 1,
  SSL_V3_ALP_FATAL    = 2
} ssl_v3_alp_alert_level_enum_type;

/** Alert ID definitions.
*/
typedef enum
{
  SSL_V3_ALP_CLOSE_NOTIFY             = 0,  
  /**< Session was closed by either side. */
  SSL_V3_ALP_UNEXPECTED_MESSAGE       = 10, 
  /**< SSL protocol received an unexpected message. */
  SSL_V3_ALP_BAD_RECORD_MAC           = 20, 
  /**< SSL record failed authentication. */
  TLS_V1_ALP_DECRYPTION_FAILED        = 21, 
  /**< Decrypted message not a multiple of block size, or invalid padding. */
  TLS_V1_ALP_RECORD_OVERFLOW          = 22, 
  /**< Record too large. */
  SSL_V3_ALP_DECOMPRESSION_FAILURE    = 30, 
  /**< SSL record failed decompression. */
  SSL_V3_ALP_HANDSHAKE_FAILURE        = 40, 
  /**< SSL handshake protocol failed. */
  SSL_V3_ALP_NO_CERTIFICATE           = 41, 
  /**< Deleted in TLS. */
  SSL_V3_ALP_BAD_CERTIFICATE          = 42, 
  /**< Certificate failed authentication. */
  SSL_V3_ALP_UNSUPPORTED_CERTIFICATE  = 43, 
  /**< Certificate type not understood. */
  SSL_V3_ALP_CERTIFICATE_REVOKED      = 44, 
  /**< Certificate invalid (was revoked). */
  SSL_V3_ALP_CERTIFICATE_EXPIRED      = 45, 
  /**< Certificate invalid (expired). */
  SSL_V3_ALP_CERTIFICATE_UNKNOWN      = 46, 
  /**< Certificate invalid (unknown error). */
  SSL_V3_ALP_ILLEGAL_PARAMETER        = 47, 
  /**< A field in the handshake was out of range or inconsistent with
  other fields.\ This is always fatal. */
  TLS_V1_ALP_UNKNOWN_CA               = 48, 
  /**< Cannot identify certificate authority. */
  TLS_V1_ALP_ACCESS_DENIED            = 49, 
  /**< No access rights to continue negotiation. */
  TLS_V1_ALP_DECODE_ERROR             = 50, 
  /**< Field value out of range. */
  TLS_V1_ALP_DECRYPT_ERROR            = 51, 
  /**< Crypto operation failed. */
  TLS_V1_ALP_EXPORT_RESTRICTION       = 60, 
  /**< Not used in TLS1.1 and later. */
  TLS_V1_ALP_PROTOCOL_VERSION         = 70, 
  /**< Cannot support requested version. */
  TLS_V1_ALP_INSUFFICIENT_SECURITY    = 71, 
  /**< Server requires more secure ciphersuites than the client can support. */
  TLS_V1_ALP_INTERNAL_ERROR           = 80, 
  /**< Local error independent of protocol. */
  TLS_V1_ALP_USER_CANCELED            = 90, 
  /**< Cancel handshake for other reasons. */
  TLS_V1_ALP_NO_RENEGOTIATION         = 100, 
  /**< Cannot renegotiate the handshake. */
  TLS_V1_ALP_UNRECOGNIZED_NAME        = 112  
  /**< RFC4366 server name extension. */
} ssl_v3_alp_alert_id_enum_type;

/**
@}
*/
/* end_addtogroup apd */

/**
@addtogroup rpd
@{ 
*/

#define SSL_V3_VERSION_SIZE          2
#define SSL_V3_SEQUENCE_SIZE         3
#define SSL_V3_MAX_RECORD_LENGTH     16384
#define SSL_V3_MAC_MAX_LENGTH        32  /**< 8-byte pad + 20-byte MAC + x. */
/**
@}
*/
/* end_addtogroup rpd */

/** @endcond
*/

#endif  /* SECSSLDEF_H */

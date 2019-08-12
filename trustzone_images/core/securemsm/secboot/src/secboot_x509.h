#ifndef SECBOOT_X509_H
#define SECBOOT_X509_H
/*===========================================================================

                       S E C U R I T Y    S E R V I C E S

                  S E C U R E  B O O T   X.5 0 9  P A R S E R

                          H E A D E R   F I L E

FILE:  secboot_x509.h

DESCRIPTION:
    Secure Boot X.509 PARSER header file

EXTERNALIZED FUNCTIONS


  Copyright (c) 2013 by Qualcomm Technologies, Inc. All Rights Reserved.
===========================================================================*/

/*=========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/secboot/src/secboot_x509.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $

when       who                  what, where, why
--------   ----                 -------------------------------------------
08/28/13   hw                   Added retail unlock feature code
02/27/13   hw                   add support for multiple root selection
04/12/10   sm                   Added support for flexible chain size
09/10/09   sm                   Added pbl_secx509_get_asn1_cert_size to 
                                support regional security
03/01/08   ANB                  Added const qualifiers.
05/02/06   AMW                  Initial SC2X port.
08/25/04   SJD                  Initial revision.  Original SW from the
                                security team.

===========================================================================*/


/*==========================================================================

                          Include Files

==========================================================================*/
#include "comdef.h"
#include "secboot_asn1.h"
#include "secboot_i.h"
/*==========================================================================

                          Constant Declaration

==========================================================================*/
/* Key Usage Masks */
#define SECX509_KEY_USAGE_DIG_SIG   (0x0100) /* digital signature */
#define SECX509_KEY_USAGE_NON_REP   (0x0080) /* non-repudiation   */
#define SECX509_KEY_USAGE_KEY_ENC   (0x0040) /* key encipherment  */
#define SECX509_KEY_USAGE_DAT_ENC   (0x0020) /* data encipherment */
#define SECX509_KEY_USAGE_KEY_ARG   (0x0010) /* key agreement     */
#define SECX509_KEY_USAGE_KEY_CRT   (0x0008) /* key cert sign     */
#define SECX509_KEY_USAGE_CRL_SIG   (0x0004) /* CRL sign          */
#define SECX509_KEY_USAGE_ENC_OLY   (0x0002) /* encipher only     */
#define SECX509_KEY_USAGE_DEC_OLY   (0x0001) /* decipher only     */
/* Extended Key Usage Masks */
#define SECX509_EX_KEY_USAGE_SAUTH  (0x0001) /* TLS Web Server Authentication*/
#define SECX509_EX_KEY_USAGE_CAUTH  (0x0002) /* TLS Web Client Authentication*/
#define SECX509_EX_KEY_USAGE_CODE   (0x0004) /* Downloadable Code Signing    */
#define SECX509_EX_KEY_USAGE_EMAIL  (0x0008) /* Email Protection             */
#define SECX509_EX_KEY_USAGE_TIME   (0x0010) /* Time Stamping                */
#define SECX509_EX_KEY_USAGE_SGC    (0x0020) /* Secured Gated Crypto         */

/*==========================================================================

                       Type Definitions

==========================================================================*/
/* Public Key algorithms in the certificate */
typedef enum
{
  SECX509_PUBKEY_RSA = 0,
  SECX509_PUBKEY_DSA = 1,
  SECX509_PUBKEY_DH  = 2,
  SECX509_PUBKEY_MAX,               /* Last one, for error checking */
  SECX509_RESERVED_1 = 0x7FFFFFFF
} pbl_secx509_pubkey_algo_type;

/* Certificate signature algorithm type */
typedef enum
{
  SECX509_md5WithRSAEncryption    = 0,
  SECX509_md2WithRSAEncryption    = 1,
  SECX509_sha1WithRSAEncryption   = 2,
  SECX509_sha256WithRSAEncryption = 3,  
  SECX509_SIG_ALGO_MAX,                 /* Last one, for error checking */
  SECX509_RESERVED_2            = 0x7FFFFFFF
} pbl_secx509_sig_algo_type;


/* RSA public key parameters */
typedef struct pbl_secx509_rsa_pubkey_type
{
  uint32  mod_len;
  const uint8   *mod_data;
  uint32  exp_e_len;
  const uint8   *exp_e_data;

} pbl_secx509_rsa_pubkey_type;

/* Union of all the public key types */
typedef struct pbl_secx509_pubkey_type
{
  pbl_secx509_pubkey_algo_type  algo;
  union
  {
    pbl_secx509_rsa_pubkey_type  rsa;
  }key;

} pbl_secx509_pubkey_type;

/* Signature Structure */
typedef struct pbl_secx509_signature_type
{
  pbl_secx509_sig_algo_type   algo_id;
  secasn1_data_type           val;

} pbl_secx509_signature_type;

/* Distinguished name structure */
typedef struct pbl_secx509_dn_type
{
  uint32             num_attrib;
  secasn1_data_type  data;

} pbl_secx509_dn_type;

/* Version structure */
typedef struct pbl_secx509_version_type
{
  uint32             ver;
  secasn1_data_type  val;

} pbl_secx509_version_type;

/* Time structure */
typedef struct pbl_secx509_time_type
{
  uint32             time;
  secasn1_data_type  data;

} pbl_secx509_time_type;

/* Authority Key Identifier structure */
typedef struct pbl_secx509_auth_key_id_type
{
  boolean            set;
  secasn1_data_type  key_id;
  pbl_secx509_dn_type    name;
  secasn1_data_type  serial_number;

} pbl_secx509_auth_key_id_type;

/* Subject Key Identifier structure */
typedef struct pbl_secx509_subject_key_id_type
{
  boolean            set;
  secasn1_data_type  key_id;

} pbl_secx509_subject_key_id_type;

/* Key Usage structure */
typedef struct pbl_secx509_key_usage_type
{
  uint32   val;
  boolean  set;

} pbl_secx509_key_usage_type;

/* CA structure */
typedef struct pbl_secx509_ca_type
{
  boolean  set;
  boolean  val;

} pbl_secx509_ca_type;

/* Extension structure type */
typedef struct pbl_secx509_ext_type
{
  boolean                          set;
  pbl_secx509_auth_key_id_type     auth_key_id;
  pbl_secx509_subject_key_id_type  subject_key_id;
  pbl_secx509_key_usage_type       key_usage;
  pbl_secx509_key_usage_type       ex_key_usage;
  int32                            path_len;
  pbl_secx509_ca_type              ca;

} pbl_secx509_ext_type;

/* Certificate information structure */
typedef struct pbl_secx509_cert_info_type
{
  pbl_secx509_version_type     version;
  secasn1_data_type            serial_number;
  pbl_secx509_signature_type   algorithm;
  pbl_secx509_dn_type          issuer;
  pbl_secx509_time_type        not_before;
  pbl_secx509_time_type        not_after;
  pbl_secx509_dn_type          subject;
  secasn1_bit_string_type      issuer_unique_id;
  secasn1_bit_string_type      subject_unique_id;
  pbl_secx509_ext_type         extension;

} pbl_secx509_cert_info_type;

/* Certificate structure */
typedef struct pbl_secx509_cert_type
{
  /* The cert_info needs to be the first member */
  pbl_secx509_cert_info_type  cert_info;

  uint32                      cinf_offset; //where the certificate actually starts -
                                           //after the initial tag/len
  uint32                      cinf_byte_len; //length of where the certificate actually starts
                                             //upto (but not including) the certificate signature
  uint32                      asn1_size_in_bytes; //size of the entire certificate (including the initial tag/len)

  /* Signature info on the cert */
  pbl_secx509_pubkey_type     pkey;
  pbl_secx509_sig_algo_type   sig_algo;
  const uint8                *sig;
  uint32                      sig_len;
  
  /*For verification */
  uint8                       cert_hash[CEML_HASH_DIGEST_SIZE_SHA256];
} pbl_secx509_cert_type;



/* Certificate list struct */
typedef struct pbl_secx509_cert_list_struct
{
  pbl_secx509_cert_type  cert[SECBOOT_MAX_NUM_CERTS];
  uint32               size;

} pbl_secx509_cert_list_type;

/* certificate list context type */
typedef struct
{
  uint32                       purpose;
  uint32                       trust;
  uint32                       depth;
  pbl_secx509_cert_list_type*  ca_list;

} pbl_secx509_cert_ctx_type;


typedef enum
{
  E_X509_SUCCESS = 0,
  E_X509_FAILURE,
  E_X509_NO_DATA,
  E_X509_DATA_INVALID,
  E_X509_BAD_DATA,
  E_X509_DATA_TOO_LARGE,
  E_X509_DATA_EXPIRED,
  E_X509_NO_MEMORY,
  E_X509_INVALID_ARG,
  E_X509_NOT_SUPPORTED,
  E_X509_RESERVED       = 0x7FFFFFFF
} secx509_errno_enum_type;

typedef enum
{
  E_X509_CODE_HASH_NOT_SPECIFIED = 0,
  E_X509_CODE_HASH_SHA1,
  E_X509_CODE_HASH_SHA256,
  E_X509_CODE_HASH_RESERVED       = 0x7FFFFFFF
}secx509_code_hash_algo_type;

typedef struct secx509_ou_field_info_type
{
  uint64                      debug_enable;
  uint64                      sw_id;  
  uint64                      hw_id;
  secx509_code_hash_algo_type code_hash_algo;
  uint64                      crash_dump_enable;
} secx509_ou_field_info_type;

#ifdef IMEM_BYTE_ACCESS
/*===================================================================
 Inserted here by sid ... used now and should be useful later on,
 will want to see if unless memcmp would work in this scenario ....
====================================================================*/

extern uint16 pbl_secutil_byte_compare
(
  const uint8*  value1_ptr,
  const uint8*  value2_ptr,
  const uint16  byte_len
);
#endif

#define PBL_MIN(x,y) (((x) <= (y)) ? (x) : (y))


/*==========================================================================

                      GLOBAL FUNCTION DECLARATIONS

==========================================================================*/

/*===========================================================================

FUNCTION SECX509_PARSE_CERTIFICATE

DESCRIPTION
  Parses an X509 certificate from the data_ptr, if *cert == NULL then the
  certificate is allocated from the memory pool, if the certificate is
  already allocated then the function just uses the pre-allocated memory

DEPENDENCIES
  None

PARAMETERS
  data_ptr - pointer to the raw certificate data
  data_bound - cert must end before here
  cert - pointer to the certificate
  ou_field_info_ptr - pointer to store OU Field values

RETURN VALUE
  E_SUCCESS if the certificate is parsed properly
  E_DATA_INVALID if the certificate cannot be parsed properly
  E_NO_MEMORY if no more memory slots are available for the certs
  E_NOT_SUPPORTED if an algorithm found in the cert is not supported
  E_INVALID_ARG if a pointer argument is NULL
  E_FAILURE if the *cert is pre-allocated but not *cert->cert or if the
            certificate data length does not match cert_len

SIDE EFFECTS
  None
===========================================================================*/
extern secx509_errno_enum_type pbl_secx509_parse_certificate
(
  const uint8                *cert_data,  /* cert data in the DER format */
  const uint8                *data_bound, /* parsing must stop before this address */
  pbl_secx509_cert_type      *cert,       /* pointer to the parsed struct*/
  secx509_ou_field_info_type *ou_field_info_ptr
);

/*===========================================================================
FUNCTION pbl_secx509_parse_cert_buffer

DESCRIPTION
  Parses binary x509 certificates from memory into the buffer.
  parsing is done in order (attest cert first, CA cert next and
  root cert last).

DEPENDENCIES
  None

PARAMETERS
  cert_chain_ptr          - points to x509 certificate chain
                            CA cert and Attestation cert are here
  cert_chain_len         - size of the certificate chain
  pbl_secx509_cert_list   - destination for all the certificate
                            after they have been parsed in order
                            of chaining.
  ou_field_info_ptr       - pointer to store OU Field values                            
  root_sel_info           - struct to store info about the root cert to select among
                            list of OEM provisioned root certs

RETURN VALUE
  E_SUCCESS on successful parsing of all certs
  E_FAILURE on unsuccessful parsing of all certs

SIDE EFFECTS
  None
===========================================================================*/
secx509_errno_enum_type pbl_secx509_parse_cert_buffer
(
  const uint8                 *cert_chain_ptr,
  uint32                       cert_chain_len, /* buffer len, to check for bogus internal len */
  pbl_secx509_cert_list_type  *secx509_cert_list,
  secx509_ou_field_info_type  *ou_field_info_ptr,
  secboot_root_cert_fuse_info_type   root_sel_info
);

/*===========================================================================
FUNCTION pbl_secx509_check_cert_list

DESCRIPTION
  Simpy checks if the certificate information, over the whole chain
  is valid and that the issuer and subject ID's are chained
  consecutively. Cryptographic signature verification down the chain
  is _NOT_ done here.

DEPENDENCIES
  None

PARAMETERS
  *cert_list_ptr  - Pointer to array of parsed certificates
  *check_ctx              - Sets of the parameters for the checking process

RETURN VALUE
  E_SUCCESS       - if no error
  E_DATA_INVALID  - Invalid Data
  E_NOT_SUPPORTED - Unknown Data

SIDE EFFECTS
  None
===========================================================================*/
secx509_errno_enum_type pbl_secx509_check_cert_list
(
  const pbl_secx509_cert_list_type *cert_list_ptr,
  const pbl_secx509_cert_ctx_type  *check_ctx
);


/*===========================================================================

FUNCTION SECX509_SKIP_NUM_CERTS

DESCRIPTION
  Skips over number of certificates and sets the chain ptr to point to the
  next certificate

DEPENDENCIES
  None

PARAMETERS
  cert_chain_pptr         - points to ptr of x509 certificate chain
  cert_chain_len          - upper bound on length of chain (parse fails
                            if the length is crossed when skipping
                            certs)
  num_certs_to_skip       - number of certs to skip in the chain

RETURN VALUE
  E_X509_SUCCESS on successful skipping of certs
  E_X509_FAILURE on unsuccessful skipping of all certs

SIDE EFFECTS
  None
===========================================================================*/
secx509_errno_enum_type pbl_secx509_skip_num_certs(
  const uint8**                 cert_chain_ptr,
  const uint8*                  cert_chain_bound,
  uint8                         num_certs_to_skip
);

/*===========================================================================

FUNCTION SECX509_CAL_CERT_NUM

DESCRIPTION
  calculate the total number of certificates

DEPENDENCIES
  None

PARAMETERS
  cert_chain_pptr         - points to ptr of x509 certificate chain
  cert_chain_len          - upper bound on length of chain (parse fails
                            if the length is crossed when skipping
                            certs)
  fuse_info_ptr           - pointer to the fuse info

RETURN VALUE
  E_X509_SUCCESS on successful skipping of certs
  E_X509_FAILURE on unsuccessful skipping of all certs

SIDE EFFECTS
  None
===========================================================================*/
secx509_errno_enum_type secboot_cal_cert_num
(
  const uint8*                 cert_chain_ptr,
  uint32                       cert_chain_len,
  uint32                      *total_cert_num
);
#endif /* SECBOOT_X509_H */


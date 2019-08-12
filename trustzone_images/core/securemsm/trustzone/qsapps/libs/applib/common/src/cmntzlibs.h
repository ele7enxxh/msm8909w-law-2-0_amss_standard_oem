/*
@file cmntzlibs.h
@brief Common Security Library. This library is shared between all TZ apps.

*/
/*===========================================================================
   Copyright (c) 2012 - 2014 by QUALCOMM Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
05/08/14     tp      Added remove function to link prxy services
04/25/14     tp      Changed DRM sconscripts to use the prxy_services 
                     library instead of linking to qsee_services.
03/10/14     rk      Removed including tzcommon_entry.h
02/26/14     rk      Added fs rename, opendir, readdir
02/04/14     rk      Removed fs.h and added qsee_fs.h
10/28/13     ib      Add rmdir (from fs) to common lib

===========================================================================*/
#include "IxErrno.h"
#include "qsee_heap.h"
#include "secmath.h"
#include "secmath_extended.h"
#include "cmnlib_syscall_wrapper.h"
#include "syscall.h"
#include "memheap_lite.h"
#include "secasn1.h"
#include "secerrno.h"
#include "secx509asntypes.h"
#include "secpkcs8.h"
#include "secrsa.h"
#include "secrsa_err.h"
#include "secrsa_keygen.h"
#include "secrsa_math.h"
#include "secrsa_math2.h"
#include "secrsa_utils.h"
#include "secrsa_exptmod.h"
#include "qsee_fs.h"
#include "qsee_time.h"
#include "sfs.h"
#include "sfs_idx.h"
#include "qsee_kdf.h"
#include "sfs_ll_funcs.h"
#include "sfs_sec.h"
#include "qsee_sfs.h"
#include "qsee_log.h"
#include "ecc_shared.h"
#include "ecc_generic_shared.h"
#include "aes_shared.h"
#include "sha_shared.h"
#include "des_shared.h"
#include "cmnlib_apps.h"
#include "cmnlib_cntxt.h"
#include "cmnlib_wrapper.h"


#define CMNLIB_IF_VERSION_MAJOR       1
#define CMNLIB_IF_VERSION_MINOR       0
#define CMNLIB_IF_VERSION             (CMNLIB_IF_VERSION_MAJOR<<16 + CMNLIB_IF_VERSION_MINOR)

#define CMNLIB_SECMATH_VERSION_MAJOR  1
#define CMNLIB_SECMATH_VERSION_MINOR  0
#define CMNLIB_SECMATH_VERSION        (CMNLIB_SECMATH_VERSION_MAJOR<<16 + CMNLIB_SECMATH_VERSION_MINOR)

#define CMNLIB_SECRSA_VERSION_MAJOR   1
#define CMNLIB_SECRSA_VERSION_MINOR   0
#define CMNLIB_SECRSA_VERSION         (CMNLIB_SECRSA_VERSION_MAJOR<<16 + CMNLIB_SECRSA_VERSION_MINOR)

#define CMNLIB_QSEE_SVC_VERSION_MAJOR  1
#define CMNLIB_QSEE_SVC_VERSION_MINOR  0
#define CMNLIB_QSEE_SVC_VERSION        (CMNLIB_QSEE_SVC_VERSION_MAJOR<<16 + CMNLIB_QSEE_SVC_VERSION_MINOR)

#define CMNLIB_ECC_VERSION_MAJOR  1
#define CMNLIB_ECC_VERSION_MINOR  0
#define CMNLIB_ECC_VERSION        (CMNLIB_ECC_VERSION_MAJOR<<16 + CMNLIB_ECC_VERSION_MINOR)

#define CMNLIB_UF_AES_VERSION_MAJOR  1
#define CMNLIB_UF_AES_VERSION_MINOR  0
#define CMNLIB_UF_AES_VERSION        (CMNLIB_UF_AES_VERSION_MAJOR<<16 + CMNLIB_UF_AES_VERSION_MINOR)

#define CMNLIB_UF_SHA_VERSION_MAJOR  1
#define CMNLIB_UF_SHA_VERSION_MINOR  0
#define CMNLIB_UF_SHA_VERSION        (CMNLIB_UF_SHA_VERSION_MAJOR<<16 + CMNLIB_UF_SHA_VERSION_MINOR)

#define CMNLIB_UF_DES_VERSION_MAJOR  1
#define CMNLIB_UF_DES_VERSION_MINOR  0
#define CMNLIB_UF_DES_VERSION        (CMNLIB_UF_DES_VERSION_MAJOR<<16 + CMNLIB_UF_DES_VERSION_MINOR)

/* Common Library malloc & free function pointers*/
typedef void* (*CMNLIB_QSEE_MALLOC)(size_t);
typedef void (*CMNLIB_QSEE_FREE)(void *);

/* secmath function pointers */
typedef SECMATH_ERRNO_ET (*CMNLIB_SECMATH_BIGINT_READ_UNSIGNED_BIN)(void *, BigInt *, const uint8 *, uint32);
typedef SECMATH_ERRNO_ET (*CMNLIB_SECMATH_BIGINT_READ_RADIX)(void *, BigInt *, const char *, uint32);


/* secrsa function pointers */
typedef secerrno_enum_type (*CMNLIB_SECRSA_SECPKCS8_PARSE)(void *, uint8 *,uint16 ,pkcs8_privkey_type *);
typedef int (*CMNLIB_SECRSA_CE_RSA_KEY_GEN)(void *, CE_RSA_KEY *, int, unsigned char *, int);
typedef int (*CMNLIB_SECRSA_CE_RSA_SIGN_HASH)(void *, CE_RSA_KEY *, CE_RSA_PADDING_TYPE ,void *,int ,const unsigned char *,int ,unsigned char *,int *);
typedef int (*CMNLIB_SECRSA_CE_RSA_VERIFY_SIGNATURE)(void *, CE_RSA_KEY *,CE_RSA_PADDING_TYPE ,void *,int ,unsigned char *,int ,unsigned char *,int);
typedef int (*CMNLIB_SECRSA_CE_RSA_ENCRYPT)(void *, CE_RSA_KEY *,CE_RSA_PADDING_TYPE ,void *,const unsigned char *,int ,unsigned char *,int *);
typedef int (*CMNLIB_SECRSA_CE_RSA_DECRYPT)(void *, CE_RSA_KEY *,CE_RSA_PADDING_TYPE ,void *,unsigned char *,int ,unsigned char *,int *);
typedef int (*CMNLIB_SECRSA_CE_UTIL_INIT_S_BIGINT)(void *, S_BIGINT **); 
typedef void (*CMNLIB_SECRSA_CE_UTIL_FREE_S_BIGINT)(void *, S_BIGINT *);
typedef int (*CMNLIB_SECRSA_CE_UTIL_COUNT_BYTES)(void *, S_BIGINT *);
typedef int (*CMNLIB_SECRSA_CE_RSA_EXPTMOD)(void *, CE_RSA_KEY *, const unsigned char *, int, unsigned char *, int *, int);

/* QSEE services function pointers */
typedef int (*CMNLIB_QSEESVC_SFS_KDF)(void *, const void *, size_t, const void *, size_t, const void *, size_t, void*, size_t);
typedef int (*CMNLIB_QSEESVC_QSEE_SFS_GETSIZE)(void *, int, uint32*);
typedef int (*CMNLIB_QSEESVC_QSEE_SFS_OPEN)(void *, const char*, int);
typedef int (*CMNLIB_QSEESVC_QSEE_SFS_CLOSE)(void *, int);
typedef int (*CMNLIB_QSEESVC_QSEE_SFS_READ)(void *, int, char *, int);
typedef int (*CMNLIB_QSEESVC_QSEE_SFS_WRITE)(void *, int, const char *, int);
typedef int (*CMNLIB_QSEESVC_QSEE_SFS_SEEK)(void *, int, int32, int);
typedef int (*CMNLIB_QSEESVC_QSEE_SFS_MKDIR)(void *, const char *);
typedef int (*CMNLIB_QSEESVC_QSEE_SFS_RMDIR)(void *, const char *);
typedef int (*CMNLIB_QSEESVC_QSEE_SFS_RM)(void *, const char *);
typedef int (*CMNLIB_QSEESVC_QSEE_SFS_ERROR)(void *, int);
typedef void (*CMNLIB_QSEESVC_QSEE_LOG)(void *, uint8, char*);
typedef int (*CMNLIB_QSEESVC_OPEN)(void *,const char* path, int mode, ...);
typedef int32 (*CMNLIB_QSEESVC_READ)(void *,int fd, void *buf, uint32 count);
typedef int32 (*CMNLIB_QSEESVC_WRITE)(void *,int fd, const void *buf, uint32 count);
typedef int (*CMNLIB_QSEESVC_CLOSE)(void *,int fd);
typedef int (*CMNLIB_QSEESVC_LSTAT)(void *,const char *path, fs_stat *buf);
typedef int (*CMNLIB_QSEESVC_MKDIR)(void *, const char *pathname, uint32 mode);
typedef int (*CMNLIB_QSEESVC_RENAME)(void *, const char *oldfilename, const char *newfilename);
typedef DIR* (*CMNLIB_QSEESVC_OPEN_DIR)(void *, const char *path);
typedef struct tzDirent* (*CMNLIB_QSEESVC_READ_DIR)(void *, DIR* pdir, struct tzDirent* entry);
typedef int (*CMNLIB_QSEESVC_RMDIR)(void *, const char *path);
typedef int (*CMNLIB_QSEESVC_TESTDIR)(void *, const char *pathname);
typedef int (*CMNLIB_QSEESVC_FILE_GET_PARTITION_FREE_SIZE)(void *, const char * path, uint64 * size);
typedef int (*CMNLIB_QSEESVC_FSTAT)(void *, int filedes, fs_stat *buf);
typedef int32 (*CMNLIB_QSEESVC_LSEEK)(void *, int fildes, int32 offset, int whence);
typedef int (*CMNLIB_QSEESVC_GETSYSTIME)(void *, struct tztm *tzTime);
typedef int (*CMNLIB_QSEESVC_GETUTCSEC)(void *, struct tztimespec *tzTimeSpec);
typedef int (*CMNLIB_QSEESVC_IS_LEAP_YEAR)(void *, unsigned int year); 
typedef int (*CMNLIB_QSEESVC_REMOVE)(void *, const char *pathname);
/* ECC library function pointers */
typedef sw_crypto_errno_enum_type (*CMNLIB_SW_ECC_PUBPRIVATE_KEY_GENERATE)(void *, affine_point_t *, bigval_t *);
typedef sw_crypto_errno_enum_type (*CMNLIB_SW_ECDH_SHARED_KEY_DERIVE)(void *, affine_point_t *, bigval_t *,affine_point_t *);
typedef sw_crypto_errno_enum_type (*CMNLIB_SW_ECDSA_SIGN)(void *, bigval_t const *, bigval_t const *, ECDSA_sig_t *);
typedef sw_crypto_errno_enum_type (*CMNLIB_SW_ECDSA_VERIFY)(void *, bigval_t const *, affine_point_t const *, ECDSA_sig_t const *);
typedef sw_crypto_errno_enum_type (*CMNLIB_SW_ECDH_MULT_BASEPOINT)(void *, affine_point_t *, const bigval_t *);
typedef void (*CMNLIB_ECC_HASH_TO_BIGVAL)(void *, bigval_t *, void const *, unsigned int);
typedef int (*CMNLIB_TOGGLE_RANDOM_BIT)(void *, void *, size_t);
typedef boolean_t (*CMNLIB_IN_CURVEP)(void *, affine_point_t const *);
typedef int (*CMNLIB_GET_RANDOM_BYTES)(void *, void *, int);
typedef int (*CMNLIB_SW_ECDH_GENERATE)(void *, affine_point_t *, bigval_t *);
typedef boolean (*CMNLIB_SW_ECDH_DERIVE)(void *, bigval_t *, bigval_t const *, affine_point_t const *);
typedef sw_crypto_errno_enum_type (*CMNLIB_SW_GENERIC_ECC_INIT)    (void *, qrlbn_ecc_domain_t *, char *, char *, char *, char *, char *, char *, unsigned);
typedef sw_crypto_errno_enum_type (*CMNLIB_SW_GENERIC_ECDSA_SIGN)  (void *, uint8_t *, uint32_t, qrlbn_ecc_bigval_t *, qrlbn_ECDSA_sig_t *, qrlbn_ecc_domain_t *);
typedef sw_crypto_errno_enum_type (*CMNLIB_SW_GENERIC_ECDSA_VERIFY)(void *, uint8_t *, uint32_t, qrlbn_ecc_affine_point_t *, qrlbn_ECDSA_sig_t *, qrlbn_ecc_domain_t *);
typedef sw_crypto_errno_enum_type (*CMNLIB_SW_GENERIC_ECDH_SHARED_KEY_DERIVE)(void *, qrlbn_ecc_bigval_t *, qrlbn_ecc_bigval_t *, qrlbn_ecc_affine_point_t *, qrlbn_ecc_domain_t *); 
typedef sw_crypto_errno_enum_type (*CMNLIB_SW_GENERIC_ECC_KEYPAIR_GENERATE)(void *, qrlbn_ecc_bigval_t *, qrlbn_ecc_affine_point_t *, qrlbn_ecc_domain_t *);
typedef sw_crypto_errno_enum_type (*CMNLIB_SW_GENERIC_ECC_BINARY_TO_BIGVAL)(void *, qrlbn_ecc_bigval_t *, const void *, size_t, const qrlbn_ecc_domain_t *, qrlbn_field_tag_t);
typedef sw_crypto_errno_enum_type (*CMNLIB_SW_GENERIC_ECC_BIGVAL_TO_BINARY)(void *, uint8_t *, size_t, const qrlbn_ecc_bigval_t *, const qrlbn_ecc_domain_t *, qrlbn_field_tag_t);
typedef sw_crypto_errno_enum_type (*CMNLIB_SW_GENERIC_ECC_COMPARE)(void *, const uint32_t *, const uint32_t *, const qrlbn_modulus_data_t *);

/* UF SHA library function pointers */
typedef sw_crypto_errno_enum_type (*CMNLIB_SW_HASH_INIT)(void *, CryptoCntxHandle **, SW_Auth_Alg_Type);
typedef sw_crypto_errno_enum_type (*CMNLIB_SW_HASH_FINAL)(void *, CryptoCntxHandle *,IovecListType *);
typedef sw_crypto_errno_enum_type (*CMNLIB_SW_HASH_UPDATE)(void *, CryptoCntxHandle *,IovecListType);
typedef sw_crypto_errno_enum_type (*CMNLIB_SW_HMAC)(void *, uint8 *, uint32, IovecListType, IovecListType *, SW_Auth_Alg_Type);

/* UF AES library function pointers */
typedef sw_crypto_errno_enum_type (*CMNLIB_SW_CIPHER_INIT)(void *, CryptoCntxHandle **, SW_Cipher_Alg_Type);
typedef sw_crypto_errno_enum_type (*CMNLIB_SW_CIPHER_DENIT)(void *, CryptoCntxHandle **, SW_Cipher_Alg_Type);
typedef sw_crypto_errno_enum_type (*CMNLIB_SW_CIPHERDATA)(void *, CryptoCntxHandle *, IovecListType,IovecListType *);
typedef sw_crypto_errno_enum_type (*CMNLIB_SW_CIPHER_SETPARAM)(void *, CryptoCntxHandle *,SW_CipherParam,const void *,uint32);
typedef sw_crypto_errno_enum_type (*CMNLIB_SW_CIPHER_GETPARAM)(void *, CryptoCntxHandle *,SW_CipherParam,void *,uint32);
typedef sw_crypto_errno_enum_type (*CMNLIB_SW_CIPHER_RESET)(void *, CryptoCntxHandle *);
typedef sw_crypto_errno_enum_type (*CMNLIB_SW_AE_UPDATEAAD)(void *, CryptoCntxHandle *, void *,uint32);
typedef sw_crypto_errno_enum_type (*CMNLIB_SW_AE_UPDATEDATA)(void *, CryptoCntxHandle *, IovecListType,IovecListType *);
typedef sw_crypto_errno_enum_type (*CMNLIB_SW_AE_FINALDATA)(void *, CryptoCntxHandle *, IovecListType,IovecListType *);

/* UF DES library function pointers */
typedef sw_crypto_errno_enum_type (*CMNLIB_SW_CIPHER_DES_INIT)(void *, CryptoCntxHandle **, SW_Cipher_DES_Alg_Type);
typedef sw_crypto_errno_enum_type (*CMNLIB_SW_CIPHER_DES_DENIT)(void *, CryptoCntxHandle **);
typedef sw_crypto_errno_enum_type (*CMNLIB_SW_CIPHERDESDATA)(void *, CryptoCntxHandle *, IovecListType, IovecListType *);
typedef sw_crypto_errno_enum_type (*CMNLIB_SW_CIPHER_DES_SETPARAM)(void *, CryptoCntxHandle *, SW_CipherDESParam, const void *, uint32);

/**
 * secmath library interface
 *
 * version  : secmath libarry interface version
 */
typedef struct CMNLIB_SECMATH_LIB_S {
  uint32 version;  
  
  CMNLIB_SECMATH_BIGINT_READ_UNSIGNED_BIN p_secmath_BIGINT_read_unsigned_bin;
  CMNLIB_SECMATH_BIGINT_READ_RADIX        p_secmath_BIGINT_read_radix;

  /* Add more API's here as needed */
  
}CMNLIB_SECMATH_LIB, *PCMNLIB_SECMATH_LIB;

/**
 * secrsa library interface
 *
 * version  : secrsa libarry interface version
 */
typedef struct CMNLIB_SECRSA_LIB_S {
  uint32 version;  

  CMNLIB_SECRSA_SECPKCS8_PARSE                p_secpkcs8_parse;
  CMNLIB_SECRSA_CE_RSA_KEY_GEN                p_ce_rsa_key_gen;
  CMNLIB_SECRSA_CE_RSA_SIGN_HASH              p_ce_rsa_sign_hash;
  CMNLIB_SECRSA_CE_RSA_VERIFY_SIGNATURE       p_ce_rsa_verify_signature;
  CMNLIB_SECRSA_CE_RSA_ENCRYPT                p_ce_rsa_encrypt;
  CMNLIB_SECRSA_CE_RSA_DECRYPT                p_ce_rsa_decrypt;
  CMNLIB_SECRSA_CE_UTIL_INIT_S_BIGINT         p_ce_util_init_s_bigint; 
  CMNLIB_SECRSA_CE_UTIL_FREE_S_BIGINT         p_ce_util_free_s_bigint;
  CMNLIB_SECRSA_CE_UTIL_COUNT_BYTES           p_ce_util_count_bytes;
  CMNLIB_SECRSA_CE_RSA_EXPTMOD                p_ce_rsa_exptmod;

  /* Add more API's here as needed */

}CMNLIB_SECRSA_LIB, *PCMNLIB_SECRSA_LIB;

/**
 * QSEE services library interface
 *
 * version  : QSEE services library interface version
 */
typedef struct CMNLIB_QSEE_SVC_LIB_S {
  uint32 version;  

  CMNLIB_QSEESVC_SFS_KDF                p_sfs_kdf;
  CMNLIB_QSEESVC_QSEE_SFS_GETSIZE       p_qsee_sfs_getSize;
  CMNLIB_QSEESVC_QSEE_SFS_OPEN          p_qsee_sfs_open;
  CMNLIB_QSEESVC_QSEE_SFS_CLOSE         p_qsee_sfs_close;
  CMNLIB_QSEESVC_QSEE_SFS_READ          p_qsee_sfs_read;
  CMNLIB_QSEESVC_QSEE_SFS_WRITE         p_qsee_sfs_write;
  CMNLIB_QSEESVC_QSEE_SFS_SEEK          p_qsee_sfs_seek;
  CMNLIB_QSEESVC_QSEE_SFS_MKDIR         p_qsee_sfs_mkdir;
  CMNLIB_QSEESVC_QSEE_SFS_RMDIR         p_qsee_sfs_rmdir;
  CMNLIB_QSEESVC_QSEE_SFS_RM            p_qsee_sfs_rm;
  CMNLIB_QSEESVC_QSEE_SFS_ERROR         p_qsee_sfs_error;
  CMNLIB_QSEESVC_QSEE_LOG               p_qsee_log;
  CMNLIB_QSEESVC_OPEN                   p_open;
  CMNLIB_QSEESVC_READ                   p_read;
  CMNLIB_QSEESVC_WRITE                  p_write;
  CMNLIB_QSEESVC_CLOSE                  p_close;
  CMNLIB_QSEESVC_LSTAT                  p_lstat;
  CMNLIB_QSEESVC_MKDIR                  p_mkdir;
  CMNLIB_QSEESVC_RENAME                 p_rename;
  CMNLIB_QSEESVC_OPEN_DIR               p_opendir;
  CMNLIB_QSEESVC_READ_DIR               p_readdir;
  CMNLIB_QSEESVC_RMDIR                  p_rmdir;
  CMNLIB_QSEESVC_TESTDIR                p_testdir;
  CMNLIB_QSEESVC_FILE_GET_PARTITION_FREE_SIZE         p_file_get_partition_free_size;
  CMNLIB_QSEESVC_FSTAT                  p_fstat;
  CMNLIB_QSEESVC_LSEEK                  p_lseek;
  CMNLIB_QSEESVC_GETSYSTIME             p_time_getsystime;
  CMNLIB_QSEESVC_GETUTCSEC              p_time_getutcsec;
  CMNLIB_QSEESVC_IS_LEAP_YEAR           p_time_is_leap_year;
  CMNLIB_QSEESVC_REMOVE                 p_remove;
  /* Add more API's here as needed */

}CMNLIB_QSEE_SVC_LIB, *PCMNLIB_QSEE_SVC_LIB;

/**
 * ECC library interface
 *
 * version  : ECC libarry interface version
 */
typedef struct CMNLIB_ECC_LIB_S {
  uint32 version;  

  CMNLIB_SW_ECC_PUBPRIVATE_KEY_GENERATE p_SW_ECC_PubPrivate_Key_generate;
  CMNLIB_SW_ECDH_SHARED_KEY_DERIVE      p_SW_ECDH_Shared_Key_Derive;
  CMNLIB_SW_ECDSA_SIGN                  p_SW_ECDSA_Sign;
  CMNLIB_SW_ECDSA_VERIFY                p_SW_ECDSA_Verify;
  CMNLIB_SW_ECDH_MULT_BASEPOINT         p_SW_ECDH_mult_basepoint;
  CMNLIB_ECC_HASH_TO_BIGVAL             p_ECC_hash_to_bigval;
  CMNLIB_TOGGLE_RANDOM_BIT              p_toggle_random_bit;
  CMNLIB_IN_CURVEP                      p_in_curveP;
  CMNLIB_GET_RANDOM_BYTES               p_get_random_bytes;
  CMNLIB_SW_ECDH_GENERATE               p_SW_ECDH_generate;
  CMNLIB_SW_ECDH_DERIVE                 p_SW_ECDH_derive;
  CMNLIB_SW_GENERIC_ECC_INIT                     p_SW_GENERIC_ECC_init; 
  CMNLIB_SW_GENERIC_ECDSA_SIGN                   p_SW_GENERIC_ECDSA_sign; 
  CMNLIB_SW_GENERIC_ECDSA_VERIFY                 p_SW_GENERIC_ECDSA_verify; 
  CMNLIB_SW_GENERIC_ECDH_SHARED_KEY_DERIVE       p_SW_GENERIC_ECDH_shared_key_derive; 
  CMNLIB_SW_GENERIC_ECC_KEYPAIR_GENERATE         p_SW_GENERIC_ECC_keypair_generate;
  CMNLIB_SW_GENERIC_ECC_BINARY_TO_BIGVAL         p_SW_GENERIC_ECC_binary_to_bigval;
  CMNLIB_SW_GENERIC_ECC_BIGVAL_TO_BINARY         p_SW_GENERIC_ECC_bigval_to_binary;
  CMNLIB_SW_GENERIC_ECC_COMPARE                  p_SW_GENERIC_ECC_compare;

  /* Add more API's here as needed */

}CMNLIB_ECC_LIB, *PCMNLIB_ECC_LIB;


/**
 * UF Crypto AES library interface
 *
 * version  : AES library interface version
 */
typedef struct CMNLIB_UFAES_LIB_S {
  uint32 version;  

  CMNLIB_SW_CIPHER_INIT                 p_SW_Cipher_Init;
  CMNLIB_SW_CIPHER_DENIT                p_SW_Cipher_DeInit;
  CMNLIB_SW_CIPHERDATA                  p_SW_CipherData;
  CMNLIB_SW_CIPHER_SETPARAM             p_SW_Cipher_SetParam;
  CMNLIB_SW_CIPHER_GETPARAM             p_SW_Cipher_GetParam;
  CMNLIB_SW_CIPHER_RESET                p_SW_Cipher_Reset;
  CMNLIB_SW_AE_UPDATEAAD                p_SW_AE_UpdateAAD;
  CMNLIB_SW_AE_UPDATEDATA               p_SW_AE_UpdateData;
  CMNLIB_SW_AE_FINALDATA                p_SW_AE_FinalData;

  /* Add more API's here as needed */

}CMNLIB_UFAES_LIB, *PCMNLIB_UFAES_LIB;

/**
 * UF Crypto DES library interface
 *
 * version  : DES library interface version
 */
typedef struct CMNLIB_UFDES_LIB_S {
  uint32 version;  

  CMNLIB_SW_CIPHER_DES_INIT                p_SW_Cipher_DES_Init;
  CMNLIB_SW_CIPHER_DES_DENIT               p_SW_Cipher_DES_DeInit;
  CMNLIB_SW_CIPHERDESDATA                  p_SW_CipherDESData;
  CMNLIB_SW_CIPHER_DES_SETPARAM            p_SW_Cipher_DES_SetParam;

  /* Add more API's here as needed */

}CMNLIB_UFDES_LIB, *PCMNLIB_UFDES_LIB;

/**
 * UF Crypto SHA library interface
 *
 * version  : SHA library interface version
 */
typedef struct CMNLIB_UFSHA_LIB_S {
  uint32 version;  

  CMNLIB_SW_HASH_INIT                 p_SW_Hash_Init;
  CMNLIB_SW_HASH_UPDATE               p_SW_Hash_Update;
  CMNLIB_SW_HASH_FINAL                p_SW_Hash_Final;
  CMNLIB_SW_HMAC                      p_SW_Hmac;

  /* Add more API's here as needed */

}CMNLIB_UFSHA_LIB, *PCMNLIB_UFSHA_LIB;

/**
 * application callback interface
 *
 * version  :       Library interface version
 * pSecmathLib:     Pointer to secmath library API's
 * pSecrsaLib:      Pointer to secrsa library API's
 * pQseeSvcLib:     Pointer to qsee service library API's
 * pTzappEntLib:    Pointer to tzapp entry lib library API's
 */
typedef struct CMNLIB_LIB_ENTRY_S {
  CMNLIB_TZAPP_LIB *pTzappLib; /* !!! Has to be the first member !!! */

  uint32 version;  /* library interface version */

  CMNLIB_SECMATH_LIB   *pSecmathLib;  /* filled at runtime by the library */
  CMNLIB_SECRSA_LIB    *pSecrsaLib;   /* filled at runtime by the library */
  CMNLIB_QSEE_SVC_LIB  *pQseeSvcLib;  /* filled at runtime by the library */
  CMNLIB_ECC_LIB       *pEccLib;
  CMNLIB_UFAES_LIB     *pUfAESLib;
  CMNLIB_UFSHA_LIB     *pUfSHALib;
  CMNLIB_UFDES_LIB     *pUfDESLib;

  /* more libraries will be added here as needed */

}CMNLIB_LIB_ENTRY, *PCMNLIB_LIB_ENTRY;

/* Macro */
#define get_cmnlib_ptr()        ((CMNLIB_LIB_ENTRY *)pgCmnlib)
#define get_ver(p)              p->version
#define get_tzapp_lib(p)        p->pTzappLib
#define get_secmath_lib(p)      p->pSecmathLib
#define get_secrsa_lib(p)       p->pSecrsaLib
#define get_services_lib(p)     p->pQseeSvcLib
#define get_ecc_lib(p)          p->pEccLib
#define get_ufaes_lib(p)        p->pUfAESLib
#define get_ufdes_lib(p)        p->pUfDESLib
#define get_ufsha_lib(p)        p->pUfSHALib

#ifdef _TZ_PROXY_LIB

/* Each app will keep a global copy of the return value in gcsl*/
extern void *pgCmnlib;
extern CMNLIB_CNTXT gAppCntxt;

#endif

/*
@file cmnlib_wrapper.h
@brief Common TZ App Library wrapper.

*/
/*===========================================================================
 Copyright (c) 2012-2014 QUALCOMM Technologies Incorporated.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
  04/25/14   tp      Changed DRM sconscripts to use the prxy_services 
                     library instead of linking to qsee_services.
  02/26/14   rk      Added rename, opendir, readdir to TZ common lib
  02/04/14   rk      Replaced tzStat with fs_stat
===========================================================================*/
#ifndef _CMNLIB_WRAPPER_H
#define _CMNLIB_WRAPPER_H

#include "qsee_time.h"

/* SECMATH Library Wrapper Prototypes */
int mod_exp_wrapper(void *pAppCntxt, uint32 *result, const uint32 *base, 
                    const uint32 *exponent, const uint32 *modulus,
                    const size_t N, const size_t expN, uint32 *work[4]);
SECMATH_ERRNO_ET secmath_BIGINT_modexp_wrapper(void *pAppCntxt, BigInt * r, BigInt * b, BigInt * e, BigInt * m);
SECMATH_ERRNO_ET secmath_BIGINT_to_unsigned_bin_wrapper(void *pAppCntxt, const BigInt * a, uint8 * buf, uint32 size);
SECMATH_ERRNO_ET secmath_BIGINT_to_radix_wrapper(void *pAppCntxt, const BigInt * a, char *str, uint32 * len, uint32 radix);
SECMATH_ERRNO_ET secmath_BIGINT_read_unsigned_bin_wrapper(void *pAppCntxt, BigInt * a, const uint8 * buf, uint32 len);
SECMATH_ERRNO_ET secmath_BIGINT_read_radix_wrapper(void *pAppCntxt, BigInt * a, const char *str, uint32 radix);
SECMATH_ERRNO_ET secmath_BIGINT_prime_test_wrapper(void *pAppCntxt, BigInt *n, uint32 t, int *result);
SECMATH_ERRNO_ET secmath_BIGINT_is_prime_wrapper(void *pAppCntxt, const BigInt *n, uint32 t, boolean *prime);
int mult_base_mod_m_wrapper(void *pAppCntxt, uint32 *z, const uint32 *m, const uint32 *a, uint32 qi, size_t N);
int mult_R_mod_m_wrapper(void *pAppCntxt, uint32 *z, const uint32 *a, const uint32 *m, const size_t N);

/* SECRSA Library Wrapper Prototypes */
secerrno_enum_type secpkcs8_parse_wrapper(void *pAppCntxt, uint8 *data_ptr, uint16 data_len, pkcs8_privkey_type *privkey);
secasn1_err_type secasn1_start_wrapper(void *pAppCntxt, uint8 *data, secasn1_data_type *ret_ptr);
secasn1_err_type secasn1_end_wrapper(void *pAppCntxt, secasn1_data_type *data_ptr);
secasn1_err_type secasn1_close_sequence_wrapper(void *pAppCntxt, secasn1_data_type *outer, secasn1_data_type *inner);
secasn1_err_type secasn1_next_field_wrapper(void *pAppCntxt, secasn1_data_type *data_ptr, secasn1_data_type *ret_ptr, uint8 verify_tag_id);
secasn1_err_type secasn1_decode_bit_string_wrapper(void *pAppCntxt, secasn1_data_type *data_ptr, secasn1_bit_string_type *bit_ptr);
secasn1_err_type secasn1_decode_boolean_wrapper(void *pAppCntxt, secasn1_data_type *data_ptr, boolean *b_val);
int ce_rsa_key_gen_wrapper(void *pAppCntxt, CE_RSA_KEY *key, int keylen, unsigned char *pub_exp, int pub_exp_len);
int ce_rsa_encrypt_wrapper(void *pAppCntxt, CE_RSA_KEY *key, CE_RSA_PADDING_TYPE padding_type, void *padding_info, 
                   const unsigned char *msg, int msglen, unsigned char *cipher, int *cipherlen);
int ce_rsa_decrypt_wrapper(void *pAppCntxt, CE_RSA_KEY *key, CE_RSA_PADDING_TYPE padding_type, void *padding_info,
                   unsigned char *cipher, int cipherlen, unsigned char *msg, int *msglen);
int ce_rsa_sign_hash_wrapper(void *pAppCntxt, CE_RSA_KEY *key, CE_RSA_PADDING_TYPE padding_type, void *padding_info,
                     int hashidx, const unsigned char *hash, int hashlen, unsigned char *signature, int *siglen);
int ce_rsa_verify_signature_wrapper(void *pAppCntxt, CE_RSA_KEY *key, CE_RSA_PADDING_TYPE padding_type, void *padding_info,
                            int hashidx, unsigned char *hash, int hashlen, unsigned char *sig, int siglen);
int ce_util_add_wrapper(void *pAppCntxt, S_BIGINT *a, S_BIGINT *b, S_BIGINT *c);
int ce_util_sub_wrapper(void *pAppCntxt, S_BIGINT *a, S_BIGINT *b, S_BIGINT *c);
int ce_util_mul_wrapper(void *pAppCntxt, S_BIGINT *A, S_BIGINT *B, S_BIGINT *C);
int ce_util_div_wrapper(void *pAppCntxt, S_BIGINT *a, S_BIGINT *b, S_BIGINT *c, S_BIGINT *d);
int ce_util_mul_d_wrapper(void *pAppCntxt, S_BIGINT *a, BLONG b, S_BIGINT *c);
int ce_util_mul_2d_wrapper(void *pAppCntxt, S_BIGINT *a, int b, S_BIGINT *c);
int ce_util_div_2d_wrapper(void *pAppCntxt, S_BIGINT *a, unsigned int b, S_BIGINT *c);
int ce_util_mod_wrapper(void *pAppCntxt, S_BIGINT *a, S_BIGINT *b, S_BIGINT *c);
int ce_util_lcm_wrapper(void *pAppCntxt, S_BIGINT *a, S_BIGINT *b, S_BIGINT *c);
int ce_util_invmod_wrapper(void *pAppCntxt, S_BIGINT *a, S_BIGINT *b, S_BIGINT *c);
uint32 ce_bignum_add_wrapper(void *pAppCntxt, BigNum *a, BigNum *b, BigNum *c);
uint32 ce_bignum_sub_wrapper(void *pAppCntxt, BigNum *a, BigNum *b, BigNum *c);
int ce_bignum_modular_reduction_wrapper(void *pAppCntxt, BigNum *a, BigNum *b, BigNum *c);
int ce_bignum_util_mul_wrapper(void *pAppCntxt, BigNum *A, BigNum *B, BigNum *C);
int ce_util_count_bits_wrapper(void *pAppCntxt, S_BIGINT * s);
int ce_util_count_bytes_wrapper(void *pAppCntxt, S_BIGINT * s);
int ce_util_init_s_bigint_wrapper(void *pAppCntxt, S_BIGINT **s);
void ce_util_free_s_bigint_wrapper(void *pAppCntxt, S_BIGINT *s);
void ce_util_reverse_str_wrapper(void *pAppCntxt, unsigned char *s, int len);
int ce_util_cmp_wrapper(void *pAppCntxt, S_BIGINT *s1, S_BIGINT *s2);
int ce_util_cmp_d_wrapper(void *pAppCntxt, S_BIGINT *a, BLONG b);
int ce_util_to_unsigned_bin_wrapper(void *pAppCntxt, S_BIGINT *a, unsigned char *b, int *c);
int ce_util_read_unsigned_bin_wrapper(void *pAppCntxt, S_BIGINT *a, unsigned char *b, int c);
void ce_util_replace_zeros_wrapper(void *pAppCntxt, uint8 *buff, uint32 buff_len);
SECMATH_ERRNO_ET secmath_BIGNUM_modexp_wrapper(void *pAppCntxt, BigNum * r, BigNum * b, BigNum * e, BigNum * m);
SECMATH_ERRNO_ET RSA_CRT_exponent_wrapper(void *pAppCntxt, CE_RSA_KEY *key, S_BIGINT *DataIn, S_BIGINT *ResultOut);
int ce_util_count_bytes_wrapper(void *pAppCntxt, S_BIGINT *bi);
int ce_rsa_exptmod_wrapper(void *pAppCntxt, CE_RSA_KEY *key, const unsigned char *in, int inlen, unsigned char *out, 
                           int *outlen, int which);

/* QSEE Service Library Wrapper Prototypes */
int open_wrapper(void *pAppCntxt, const char* path, int mode, ...);
int openat_wrapper(void *pAppCntxt, int fd, const char* path, int mode, ...);
int unlinkat_wrapper(void *pAppCntxt, int dirfd, const char *pathname, int flags);
int fcntl_wrapper(void *pAppCntxt, int fd, int command, ...);
int creat_wrapper(void *pAppCntxt, const char*  path, uint32  mode);
int32 read_wrapper(void *pAppCntxt, int fd, void *buf, uint32 count);
int32 write_wrapper(void *pAppCntxt, int fd, const void *buf, uint32 count);
int close_wrapper(void *pAppCntxt, int fd);
int32 lseek_wrapper(void *pAppCntxt, int fildes, int32 offset, int whence);
int link_wrapper(void *pAppCntxt, const char *oldpath, const char *newpath);
int unlink_wrapper(void *pAppCntxt, const char *pathname);
int mkdir_wrapper(void *pAppCntxt, const char *pathname, uint32 mode);
int rmdir_wrapper(void *pAppCntxt, const char *path);
int testdir_wrapper(void *pAppCntxt, const char *pathname);
int32 telldir_wrapper(void *pAppCntxt, const char *pathname);
int remove_wrapper(void *pAppCntxt, const char *pathname);
int fstat_wrapper(void *pAppCntxt, int filedes, fs_stat *buf);
int lstat_wrapper(void *pAppCntxt, const char *path, fs_stat *buf);
int file_end_wrapper(void *pAppCntxt);
int file_dir_chown_chmod_wrapper(void *pAppCntxt, char*  path, uint32 path_len, char* word, unsigned long int word_len, char* owner, unsigned long int owner_len, char* mod, unsigned long int mod_len, unsigned long int level);
int rename_wrapper(void *pAppCntxt, const char *oldfilename, const char *newfilename);
DIR* opendir_wrapper(void *pAppCntxt, const char *path);
struct tzDirent* readdir_wrapper(void *pAppCntxt, DIR* pdir, struct tzDirent* entry);
int file_get_partition_free_size_wrapper(void *pAppCntxt, const char * path, uint64 * size);
int time_getsystime_wrapper(void *pAppCntxt, struct tztm *tzTime);
int time_getutcsec_wrapper(void *pAppCntxt, struct tztimespec *tzTimeSpec);
int time_is_leap_year_wrapper(void *pAppCntxt, unsigned int year);
int sfs_kdf_wrapper(void *pAppCntxt, const void *key, size_t key_len, const void *label, size_t label_len, const void *context, size_t context_len, void* output, size_t output_len);
int qsee_sfs_getSize_wrapper(void *pAppCntxt, int fd, unsigned long int* size);
int qsee_sfs_open_wrapper(void *pAppCntxt, const char* path, int flags);
int qsee_sfs_close_wrapper(void *pAppCntxt, int fd);
int qsee_sfs_read_wrapper(void *pAppCntxt, int fd, char *buf, int nbytes);
int qsee_sfs_write_wrapper(void *pAppCntxt, int fd, const char *buf, int nbytes);
int qsee_sfs_seek_wrapper(void *pAppCntxt, int fd, int32 offset, int whence);
int qsee_sfs_mkdir_wrapper(void *pAppCntxt, const char *path);
int qsee_sfs_rmdir_wrapper(void *pAppCntxt, const char *path);
int qsee_sfs_rm_wrapper(void *pAppCntxt, const char *path);
int qsee_sfs_error_wrapper(void *pAppCntxt, int fd);
void qsee_log_wrapper(void *pAppCntxt, uint8 pri, char* fmt);

void set_cmnlib_rw_base(uint32 rw_base);

/* ECC Library API's wrapper */
sw_crypto_errno_enum_type SW_ECC_PubPrivate_Key_generate_wrapper(void *pAppCntxt, affine_point_t *pubkey, bigval_t *privkey);
sw_crypto_errno_enum_type SW_ECDH_Shared_Key_Derive_wrapper(void *pAppCntxt, affine_point_t *shared_key, bigval_t *privkey,affine_point_t *pubkey);
sw_crypto_errno_enum_type SW_ECDSA_Sign_wrapper(void *pAppCntxt, bigval_t const *msgdgst, bigval_t const *privkey, ECDSA_sig_t *sig);
sw_crypto_errno_enum_type SW_ECDSA_Verify_wrapper(void *pAppCntxt, bigval_t const *msgdgst, affine_point_t const *pubkey, ECDSA_sig_t const *sig);
sw_crypto_errno_enum_type SW_ECDH_mult_basepoint_wrapper(void *pAppCntxt, affine_point_t *P1, const bigval_t *k);
void ECC_hash_to_bigval_wrapper(void *pAppCntxt, bigval_t *tgt, void const *hashp, unsigned int hashlen);
int toggle_random_bit_wrapper(void *pAppCntxt, void *buf, size_t len);
boolean_t in_curveP_wrapper(void *pAppCntxt, affine_point_t const *P);
int get_random_bytes_wrapper(void *pAppCntxt, void *buf, int len);
int SW_ECDH_generate_wrapper(void *pAppCntxt, affine_point_t *P1, bigval_t *k);
boolean SW_ECDH_derive_wrapper(void *pAppCntxt, bigval_t *tgt, bigval_t const *k, affine_point_t const *Q);
sw_crypto_errno_enum_type SW_GENERIC_ECC_init_wrapper(void *pAppCntxt, qrlbn_ecc_domain_t * dp, char *modulus, char *a, 
	                                                  char* b, char *x, char *y, char *n, unsigned cofactor);
sw_crypto_errno_enum_type SW_GENERIC_ECDSA_sign_wrapper(void *pAppCntxt, uint8_t *msgdgst, uint32_t msgdgst_len, qrlbn_ecc_bigval_t *privkey,                                   
                                                        qrlbn_ECDSA_sig_t *sig, qrlbn_ecc_domain_t *dp);
sw_crypto_errno_enum_type SW_GENERIC_ECDSA_verify_wrapper(void *pAppCntxt, uint8_t *msgdgst, uint32_t msgdgst_len, qrlbn_ecc_affine_point_t *pubkey,                                   
                                                          qrlbn_ECDSA_sig_t *sig, qrlbn_ecc_domain_t *dp);
sw_crypto_errno_enum_type SW_GENERIC_ECDH_shared_key_derive_wrapper(void *pAppCntxt, qrlbn_ecc_bigval_t *shared_key, qrlbn_ecc_bigval_t *privkey, 
                                                                    qrlbn_ecc_affine_point_t *pubkey, qrlbn_ecc_domain_t *dp);
sw_crypto_errno_enum_type SW_GENERIC_ECC_keypair_generate_wrapper(void *pAppCntxt, qrlbn_ecc_bigval_t *privkey, qrlbn_ecc_affine_point_t *pubkey, 
  	                                                              qrlbn_ecc_domain_t *dp);
sw_crypto_errno_enum_type SW_GENERIC_ECC_binary_to_bigval_wrapper(void *pAppCntxt, qrlbn_ecc_bigval_t *dst, const void *src, size_t srclen,
		                                                          const qrlbn_ecc_domain_t *dp, qrlbn_field_tag_t tag);
sw_crypto_errno_enum_type SW_GENERIC_ECC_bigval_to_binary_wrapper(void *pAppCntxt, uint8_t *dst, size_t dstlen, const qrlbn_ecc_bigval_t *src,
		                                                          const qrlbn_ecc_domain_t *dp, qrlbn_field_tag_t tag); 
sw_crypto_errno_enum_type SW_GENERIC_ECC_compare_wrapper(void *pAppCntxt, const uint32_t *a, const uint32_t *b, const qrlbn_modulus_data_t *mdp);

/* UFCrypto AES Library API's wrapper */
sw_crypto_errno_enum_type SW_Cipher_Init_wrapper(void *pAppCntxt, CryptoCntxHandle **handle, SW_Cipher_Alg_Type pAlgo);
sw_crypto_errno_enum_type SW_Cipher_DeInit_wrapper(void *pAppCntxt, CryptoCntxHandle **handle, SW_Cipher_Alg_Type pAlgo);
sw_crypto_errno_enum_type SW_CipherData_wrapper(void *pAppCntxt, CryptoCntxHandle *handle, IovecListType ioVecIn,
                                                IovecListType *ioVecOut);
sw_crypto_errno_enum_type SW_Cipher_SetParam_wrapper(void *pAppCntxt, CryptoCntxHandle *handle, SW_CipherParam nParamID, 
                                                     const void *pParam, uint32 cParam);
sw_crypto_errno_enum_type SW_Cipher_GetParam_wrapper(void *pAppCntxt, CryptoCntxHandle *handle, SW_CipherParam nParamID, 
                                                     void *pParam, uint32 cParam);
sw_crypto_errno_enum_type SW_AE_UpdateAAD_wrapper(void *pAppCntxt, CryptoCntxHandle *handle, 
                                                      void *aad, uint32 aad_len);
sw_crypto_errno_enum_type SW_AE_UpdateData_wrapper(void *pAppCntxt, CryptoCntxHandle *handle, IovecListType ioVecIn,
                                                   IovecListType *ioVecOut);
sw_crypto_errno_enum_type SW_AE_FinalData_wrapper(void *pAppCntxt, CryptoCntxHandle *handle, IovecListType ioVecIn,
                                                  IovecListType *ioVecOut);

/* UFCrypto SHA Library API's wrapper */
sw_crypto_errno_enum_type SW_Hash_Init_wrapper(void *pAppCntxt, CryptoCntxHandle **handle, SW_Auth_Alg_Type auth_alg);
sw_crypto_errno_enum_type SW_Hash_Update_wrapper(void *pAppCntxt, CryptoCntxHandle *handle,IovecListType ioVecIn);
sw_crypto_errno_enum_type SW_Hash_Final_wrapper(void *pAppCntxt, CryptoCntxHandle *handle,IovecListType *ioVecOut);
sw_crypto_errno_enum_type SW_Hmac_wrapper(void *pAppCntxt, uint8 * key_ptr, uint32 keylen, IovecListType ioVecIn, 
                                          IovecListType * ioVecOut, SW_Auth_Alg_Type  pAlgo);

/* UFCrypto DES Library API's wrapper */
sw_crypto_errno_enum_type SW_Cipher_DES_Init_wrapper(void *pAppCntxt, CryptoCntxHandle **handle, SW_Cipher_DES_Alg_Type pAlgo);
sw_crypto_errno_enum_type SW_Cipher_DES_DeInit_wrapper(void *pAppCntxt, CryptoCntxHandle **handle);
sw_crypto_errno_enum_type SW_CipherDESData_wrapper(void *pAppCntxt, CryptoCntxHandle *handle, IovecListType ioVecIn,
                                                   IovecListType *ioVecOut);
sw_crypto_errno_enum_type SW_Cipher_DES_SetParam_wrapper(void *pAppCntxt, CryptoCntxHandle *handle, SW_CipherDESParam nParamID, 
                                                         const void *pParam, uint32 cParam);

#endif /* _CMNLIB_WRAPPER_H */


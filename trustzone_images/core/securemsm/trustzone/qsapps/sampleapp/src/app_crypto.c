/*
@file app_crypto.c
@brief This file is a sample code that show how QSEE crypto API's can be used.

*/
/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsapps/sampleapp/src/app_crypto.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------

===========================================================================*/
#include "IxErrno.h"
#include "qsee_cipher.h"
#include "qsee_hash.h"
#include "qsee_hmac.h"
#include "qsee_cmac.h"
#include "qsee_log.h"
#include "qsee_heap.h"
#include "qsee_timer.h"
#include "qsee_ecc.h"
#include "qsee_kdf.h"
#include "qsee_uf_aes.h"
#include "qsee_uf_des.h"
#include "qsee_uf_sha.h"

#define NEED_TO_REENABLE 0

#define ECC_TEST_COUNT 10
#define TEST_CRYPTO_PERF_MAX_ITERATIONS      100
#define TEST_CRYPTO_GCM_PERF_MAX_ITERATIONS  2
#define TZBSP_TEST_CRYPTO_PERF_SIZE_8K       8*1024
#define TZBSP_TEST_CRYPTO_PERF_SIZE_32K      32*1024

#define TZBSP_TEST_CRYPTO_PERF_START_SIZE TZBSP_TEST_CRYPTO_PERF_SIZE_8K
#define TZBSP_TEST_CRYPTO_PERF_END_SIZE   TZBSP_TEST_CRYPTO_PERF_SIZE_32K

#define UINT8_A uint8 __attribute__ ((aligned(128)))

#define ECC_MAX_MODULUS_BITS (521 + 8)
#define _ECC_BIGNUM_WORDS ((ECC_MAX_MODULUS_BITS + 24 + 31) / 32)
#define QRLBN_ECC_MODULUS_CHECK_FAIL(modulus_struct_p)      \
((modulus_struct_p) == NULL ||          \
 (modulus_struct_p)->md.modulus != (modulus_struct_p)->modulus_storage.data || \
 (modulus_struct_p)->md.montgomery_R != (modulus_struct_p)->montgomery_R_storage.data || \
 (modulus_struct_p)->md.montgomery_R2 != (modulus_struct_p)->montgomery_R2_storage.data || \
 (modulus_struct_p)->md.allocated_words != _ECC_BIGNUM_WORDS)

UINT8_A aes_large_plain_text_1[1024];
UINT8_A aes_large_cipher_text_1[1024];
UINT8_A aes_large_plain_text2[16*1024];
UINT8_A aes_large_cipher_text2[16*1024];
UINT8_A aes_large_cipher_text3[16*1024];
UINT8_A aes_performance[32*1024];

//CE CCM Vectors 
UINT8_A aesccm_iv_1[] = 
{
  0x07, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
};

UINT8_A aesccm128_key_1[] = 
{
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,  
  0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f 
};

UINT8_A aesccm128_nonce_1[] = 
{
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16
};

//8 bytes of Associated header data with 8 bytes of padding, 4 bytes of PT = payload
UINT8_A aesccm128_plain_text_1[] = 
{
  0x00, 0x00,                                     //Padding
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, //Associated header data
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             //Padding
  0x20, 0x21, 0x22, 0x23,                         //PT = payload
};

//8 bytes of Associated header data with 8 bytes of padding, 4 bytes of CT, 4 bytes of MAC
UINT8_A aesccm128_cipher_text_1[] = 
{
  0x00, 0x00,                                     //Padding
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, //Associated header data
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             //Padding
  0x71, 0x62, 0x01, 0x5b,                         //CT 
  0x4d, 0xac, 0x25, 0x5d                          //MAC  
}; 

UINT8_A aesccm_iv_2[] = 
{
  0x06, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 
  0x17, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
};

UINT8_A aesccm128_key_2[] = 
{
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,  
  0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f 
};

UINT8_A aesccm128_nonce_2[] = 
{
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17
};

//16 bytes of Associated header data with 16 bytes of padding, 16 bytes of PT = payload
UINT8_A aesccm128_plain_text_2[] = 
{
  0x00, 0x00,                                     //Padding
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, //Associated header data
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //Padding
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, //PT = payload
  0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f   
};

//16 bytes of Associated header data with 16 bytes of padding, 16 bytes of CT, 6 bytes of MAC
UINT8_A aesccm128_cipher_text_2[] = 
{
  0x00, 0x00,                                     //Padding
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, //Associated header data
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //Padding
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             
  0xd2, 0xa1, 0xf0, 0xe0, 0x51, 0xea, 0x5f, 0x62, //CT
  0x08, 0x1a, 0x77, 0x92, 0x07, 0x3d, 0x59, 0x3d,  
  0x1f, 0xc6, 0x4f, 0xbf, 0xac, 0xcd              //MAC
};

UINT8_A aesccm_iv_3[] = 
{
  0x02, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 
  0x17, 0x18, 0x19, 0x1a, 0x1b, 0x00, 0x00, 0x00
};

UINT8_A aesccm128_key_3[] = 
{
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,  
  0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f 
};

UINT8_A aesccm128_nonce_3[] = 
{
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
  0x18, 0x19, 0x1a, 0x1b, 
};

//20 bytes of Associated header data with 12 bytes of padding, 24 bytes of PT = payload
UINT8_A aesccm128_plain_text_3[] = 
{
  0x00, 0x00,                                     //Padding
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, //Associated header data
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
  0x10, 0x11, 0x12, 0x13,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //Padding
  0x00, 0x00, 
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, //PT = payload
  0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
};

//20 bytes of Associated header data with 12 bytes of padding, 24 bytes of CT, 8 bytes of MAC
UINT8_A aesccm128_cipher_text_3[] = 
{
  0x00, 0x00,                                     //Padding
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, //Associated header data
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
  0x10, 0x11, 0x12, 0x13,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //Padding
  0x00, 0x00, 
  0xe3, 0xb2, 0x01, 0xa9, 0xf5, 0xb7, 0x1a, 0x7a, //CT
  0x9b, 0x1c, 0xea, 0xec, 0xcd, 0x97, 0xe7, 0x0b, 
  0x61, 0x76, 0xaa, 0xd9, 0xa4, 0x42, 0x8a, 0xa5,
  0x48, 0x43, 0x92, 0xfb, 0xc1, 0xb0, 0x99, 0x51, //MAC  
};

typedef struct
{
  QSEE_CIPHER_ALGO_ET  alg;
  QSEE_CIPHER_MODE_ET  mode;
  uint8 *pt;
  uint32 pt_len;
  uint8 *key;
  uint32 key_len;
  uint8 *iv;
  uint8 *nonce;
  uint32 nonce_len;
  uint32 payload_len;
  uint8 *ct;
  uint32 ct_len;
  uint8 *ct2;
  uint32 header_len;
  uint32 sw_verify;
}__attribute__ ((packed)) aes_ccm_vector_type;

aes_ccm_vector_type aes_ccm_test_vectors[] = 
{ 
  {QSEE_CIPHER_ALGO_AES_128, QSEE_CIPHER_MODE_CCM,
   aesccm128_plain_text_1, sizeof(aesccm128_plain_text_1), aesccm128_key_1,
   sizeof(aesccm128_key_1), aesccm_iv_1, aesccm128_nonce_1, sizeof(aesccm128_nonce_1),
   4, aesccm128_cipher_text_1, sizeof(aesccm128_cipher_text_1), aes_large_cipher_text3, 8, 0},

  {QSEE_CIPHER_ALGO_AES_128, QSEE_CIPHER_MODE_CCM,
   aesccm128_plain_text_2, sizeof(aesccm128_plain_text_2), aesccm128_key_2, 
   sizeof(aesccm128_key_2), aesccm_iv_2, aesccm128_nonce_2, sizeof(aesccm128_nonce_2),
   16, aesccm128_cipher_text_2, sizeof(aesccm128_cipher_text_2), aes_large_cipher_text3, 16, 0},

  {QSEE_CIPHER_ALGO_AES_128, QSEE_CIPHER_MODE_CCM,
   aesccm128_plain_text_3, sizeof(aesccm128_plain_text_3), aesccm128_key_3, 
   sizeof(aesccm128_key_3), aesccm_iv_3, aesccm128_nonce_3, sizeof(aesccm128_nonce_3),
   24, aesccm128_cipher_text_3, sizeof(aesccm128_cipher_text_3), aes_large_cipher_text3, 20, 0}
};

aes_ccm_vector_type aes_ccm_test_vectors_1 = 
{
  QSEE_CIPHER_ALGO_AES_128, QSEE_CIPHER_MODE_CCM,
  aes_large_plain_text2, 0, aesccm128_key_1, sizeof(aesccm128_key_1), 
  aesccm_iv_1, aesccm128_nonce_1, sizeof(aesccm128_nonce_1),
  0, aes_large_cipher_text2, 0, aes_large_cipher_text3, 0, 1
};

/* Simultaneous HASH/AES CBC test vectors */
UINT8_A hash_aes_key_128[] = 
{
  0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};

UINT8_A hash_aes_key_256[] = 
{
  0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
  0x3c, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x2b
};

UINT8_A hash_aes_iv_1[] = 
{
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};

typedef struct
{
  QSEE_HASH_ALGO_ET     hash_algo; 
  QSEE_CIPHER_ALGO_ET   cipher_algo; 
  QSEE_CIPHER_MODE_ET   cipher_mode; 
  uint8*                pt;
  uint32                pt_len;
  uint8*                key;
  uint32                key_len;
  uint8*                iv;
  uint8*                ct;
  uint8*                ct2;
} __attribute__ ((packed)) hash_aes_vector_type;  

/* Simultaneous Hash/AES AES-128/256 and SHA1/SHA256 test vectors array */
hash_aes_vector_type hash_aes_test_vectors[] = 
{
#if 0 // TonyB
  {QSEE_HASH_SHA1, QSEE_CIPHER_ALGO_AES_128, QSEE_CIPHER_MODE_CTS, 
   aes_large_plain_text2, sizeof(aes_large_plain_text2), 
   hash_aes_key_128, sizeof(hash_aes_key_128),
   hash_aes_iv_1, aes_large_cipher_text2, aes_large_cipher_text3} 
  , 
#endif
  {QSEE_HASH_SHA1, QSEE_CIPHER_ALGO_AES_128, QSEE_CIPHER_MODE_CBC, 
   aes_large_plain_text2, sizeof(aes_large_plain_text2), 
   hash_aes_key_128, sizeof(hash_aes_key_128),
   hash_aes_iv_1, aes_large_cipher_text2, aes_large_cipher_text3}
  ,
  {QSEE_HASH_SHA1, QSEE_CIPHER_ALGO_AES_256, QSEE_CIPHER_MODE_CBC, 
   aes_large_plain_text2, sizeof(aes_large_plain_text2),  
   hash_aes_key_256, sizeof(hash_aes_key_256),
   hash_aes_iv_1, aes_large_cipher_text2, aes_large_cipher_text3}
  ,
  {QSEE_HASH_SHA256, QSEE_CIPHER_ALGO_AES_128,QSEE_CIPHER_MODE_CBC, 
   aes_large_plain_text2, sizeof(aes_large_plain_text2), 
   hash_aes_key_128, sizeof(hash_aes_key_128),
   hash_aes_iv_1, aes_large_cipher_text2, aes_large_cipher_text3}
  ,
  {QSEE_HASH_SHA256, QSEE_CIPHER_ALGO_AES_256, QSEE_CIPHER_MODE_CBC, 
   aes_large_plain_text2, sizeof(aes_large_plain_text2), 
   hash_aes_key_256, sizeof(hash_aes_key_256),
   hash_aes_iv_1, aes_large_cipher_text2, aes_large_cipher_text3}
  ,
};

/* Generic ECC test vectors */
uint8 ecc_160_public_key_x[] =
{
 0x51,0xb4,0x49,0x6f,0xec,0xc4,0x06,0xed,0x0e,0x75,0xa2,0x4a,0x3c,0x03,0x20,0x62,0x51,0x41,0x9d,0xc0
};

uint8 ecc_160_public_key_y[] =
{
 0xc2,0x8d,0xcb,0x4b,0x73,0xa5,0x14,0xb4,0x68,0xd7,0x93,0x89,0x4f,0x38,0x1c,0xcc,0x17,0x56,0xaa,0x6c
};

uint8 ecc_160_private_key[] =
{
 0xAA,0x37,0x4F,0xFC,0x3C,0xE1,0x44,0xE6,0xB0,0x73,0x30,0x79,0x72,0xCB,0x6D,0x57,0xB2,0xA4,0xE9,0x82
};

uint8 ecc_160_signature[] =
{
 0xce,0x28,0x73,0xe5,0xbe,0x44,0x95,0x63,0x39,0x1f,0xeb,0x47,0xdd,0xcb,0xa2,0xdc,0x16,0x37,0x91,0x91,  //signature r
 0x34,0x80,0xec,0x13,0x71,0xa0,0x91,0xa4,0x64,0xb3,0x1c,0xe4,0x7d,0xf0,0xcb,0x8a,0xa2,0xd9,0x8b,0x54   //signature s
};

uint8 ecc_160_hash[] =
{
 0xA9,0x99,0x3E,0x36,0x47,0x06,0x81,0x6A,0xBA,0x3E,0x25,0x71,0x78,0x50,0xC2,0x6C,0x9C,0xD0,0xD8,0x9D
};

uint8 ecc_192_public_key_x[] =
{
 0x8d,0x80,0x96,0xd2,0xac,0x42,0x0e,0x8d,0x6e,0x7d,0xc7,0x44,0x53,0xc7,0xf9,0x4d,0x2a,0x6c,0x3f,0x01,0x98,0x8c,0x64,0xf7
};

uint8 ecc_192_public_key_y[] =
{
 0x32,0xe4,0x25,0xb9,0xe1,0x3c,0xd4,0x32,0x1d,0x03,0x7f,0xf9,0xb4,0x37,0x82,0x0f,0xf8,0xac,0xb3,0xdc,0x89,0x53,0x92,0xeb
};

uint8 ecc_192_private_key[] =
{
 0x5d,0x32,0xdc,0x03,0x42,0x20,0x03,0xd3,0x01,0xc9,0xd7,0x19,0x8b,0x85,0x9e,0x88,0x20,0xbd,0xa6,0xca,0x61,0x0d,0xe5,0xa6
};

uint8 ecc_192_signature[] =
{
 0x0b,0x42,0xa6,0x59,0x64,0xc1,0xb2,0xcb,0x5f,0xa9,0xa3,0xb9,0x5a,0x2d,0x90,0xa6,0x50,0x1a,0x4c,0xa7,0xd3,0x4c,0xf9,0x5f, //signature r 
 0x45,0x85,0x0f,0xe1,0xd2,0xda,0x50,0xa7,0xea,0x97,0xa8,0x16,0x9c,0x97,0xde,0x37,0x57,0x4f,0x7e,0x04,0x92,0x7e,0x2a,0xba  //signature s
};

uint8 ecc_192_hash[] =
{
 0x85,0x60,0xe2,0xca,0x33,0x9a,0x80,0xf4,0x22,0xce,0xe3,0x0c,0x70,0x5f,0xf6,0x4c,0x52,0x32,0xcc,0x9d,0x9e,0x75,0xcb,0x82,0x2d,0x2a,0x34,0xc6,0x3d,0x81,0x8d,0x5a
}; 

uint8 ecc_256_public_key_x[] =
{
 0x59,0x63,0x75,0xe6,0xce,0x57,0xe0,0xf2,0x02,0x94,0xfc,0x46,0xbd,0xfc,0xfd,0x19,0xa3,0x9f,0x81,0x61,0xb5,0x86,0x95,0xb3,0xec,0x5b,0x3d,0x16,0x42,0x7c,0x27,0x4d
};

uint8 ecc_256_public_key_y[] =
{
 0x42,0x75,0x4d,0xfd,0x25,0xc5,0x6f,0x93,0x9a,0x79,0xf2,0xb2,0x04,0x87,0x6b,0x3a,0x3a,0xb1,0xce,0xb2,0xe4,0xff,0x57,0x1a,0xbf,0x4f,0xbf,0x36,0x32,0x6c,0x8b,0x27
};

uint8 ecc_256_private_key[] =
{
 0x2c,0xa1,0x41,0x1a,0x41,0xb1,0x7b,0x24,0xcc,0x8c,0x3b,0x08,0x9c,0xfd,0x03,0x3f,0x19,0x20,0x20,0x2a,0x6c,0x0d,0xe8,0xab,0xb9,0x7d,0xf1,0x49,0x8d,0x50,0xd2,0xc8
};

uint8 ecc_256_signature[] =
{
 0xd7,0x3c,0xd3,0x72,0x2b,0xae,0x6c,0xc0,0xb3,0x90,0x65,0xbb,0x40,0x03,0xd8,0xec,0xe1,0xef,0x2f,0x7a,0x8a,0x55,0xbf,0xd6,0x77,0x23,0x4b,0x0b,0x3b,0x90,0x26,0x50,//signature r 
 0xd9,0xc8,0x82,0x97,0xfe,0xfe,0xd8,0x44,0x1e,0x08,0xdd,0xa6,0x95,0x54,0xa6,0x45,0x2b,0x8a,0x0b,0xd4,0xa0,0xea,0x1d,0xdb,0x75,0x04,0x99,0xf0,0xc2,0x29,0x8c,0x2f //signature s
};

uint8 ecc_256_hash[] =
{
 0x1B,0xD4,0xED,0x43,0x0B,0x0F,0x38,0x4B,0x4E,0x8D,0x45,0x8E,0xFF,0x1A,0x8A,0x55,0x32,0x86,0xD7,0xAC,0x21,0xCB,0x2F,0x68,0x06,0x17,0x2E,0xF5,0xF9,0x4A,0x06,0xAD
};

/* Test vectors from NIST: http://csrc.nist.gov/groups/STM/cavp/index.html */

uint8 ecc_224_public_key_x[] = {0x4C,0x74,0x1E,0x4D,0x20,0x10,0x36,0x70,0xB7,0x16,0x1A,0xE7,0x22,0x71,0x08,0x21,0x55,0x83,0x84,0x18,0x08,0x43,0x35,0x33,0x8A,0xC3,0x8F,0xA4}; // "4c741e4d20103670b7161ae72271082155838418084335338ac38fa4";

uint8 ecc_224_public_key_y[] = {0xDB,0x79,0x19,0x15,0x1A,0xC2,0x85,0x87,0xB7,0x2B,0xAD,0x7A,0xB1,0x80,0xEC,0x8E,0x95,0xAB,0x9E,0x2C,0x8D,0x81,0xD9,0xB9,0xD7,0xE2,0xE3,0x83}; // "db7919151ac28587b72bad7ab180ec8e95ab9e2c8d81d9b9d7e2e383";

uint8 ecc_224_private_key[] = {0x88,0x8F,0xC9,0x92,0x89,0x3B,0xDD,0x8A,0xA0,0x2C,0x80,0x76,0x88,0x32,0x60,0x5D,0x02,0x0B,0x81,0xAE,0x0B,0x25,0x47,0x41,0x54,0xEC,0x89,0xAA}; // "888fc992893bdd8aa02c80768832605d020b81ae0b25474154ec89aa";

uint8 ecc_224_signature[] = {0x09,0x09,0xC9,0xB9,0xCA,0xE8,0xD2,0x79,0x0E,0x29,0xDB,0x6A,0xFD,0xB4,0x5C,0x04,0xF5,0xB0,0x72,0xC4,0xC2,0x04,0x10,0xC7,0xDC,0x9B,0x67,0x72, // r "0909c9b9cae8d2790e29db6afdb45c04f5b072c4c20410c7dc9b6772
                             0x29,0x8F,0x4F,0xCA,0xE1,0xFE,0x27,0x1D,0xA1,0xE0,0x34,0x5D,0x11,0xD0,0x7A,0x1F,0xCA,0x43,0xF5,0x8A,0xF4,0xC1,0x13,0xB9,0x09,0xEE,0xDE,0xA0}; // s  298f4fcae1fe271da1e0345d11d07a1fca43f58af4c113b909eedea0";

// SHA256 digest of 2b49de971bb0f705a3fb5914eb7638d72884a6c3550667dbfdf301adf26bde02f387fd426a31be6c9ff8bfe8690c8113c88576427f1466508458349fc86036afcfb66448b947707e791e71f558b2bf4e7e7507773aaf4e9af51eda95cbce0a0f752b216f8a54a045d47801ff410ee411a1b66a516f278327df2462fb5619470e
uint8 ecc_224_hash[] = {0xF0,0x05,0xCE,0xD6,0x19,0x95,0x8B,0xE9,0x56,0xFA,0xA2,0xF6,0x21,0xD1,0x0C,0xB2,0xBF,0x83,0xE4,0xBD,0x26,0xDD,0xBE,0x2B,0xC1,0xB8,0x35,0x83,0xDC,0x00,0xC8,0x53}; // "f005ced619958be956faa2f621d10cb2bf83e4bd26ddbe2bc1b83583dc00c853";

uint8 ecc_384_public_key_x[] = "0400193b21f07cd059826e9453d3e96dd145041c97d49ff6b7047f86bb0b0439e909274cb9c282bfab88674c0765bc75";

uint8 ecc_384_public_key_y[] = "f70d89c52acbc70468d2c5ae75c76d7f69b76af62dcf95e99eba5dd11adf8f42ec9a425b0c5ec98e2f234a926b82a147";

uint8 ecc_384_private_key[] = "c602bc74a34592c311a6569661e0832c84f7207274676cc42a89f058162630184b52f0d99b855a7783c987476d7f9e6b";

uint8 ecc_384_signature[] = "b11db00cdaf53286d4483f38cd02785948477ed7ebc2ad609054551da0ab0359978c61851788aa2ec3267946d440e878\
                             16007873c5b0604ce68112a8fee973e8e2b6e3319c683a762ff5065a076512d7c98b27e74b7887671048ac027df8cbf2"; // r \ s

uint8 ecc_384_hash[] = "4baa1fa97be584cc38d66191407e85eb8ae01aff9d277fafea09195040ee8824"; // SHA256 digest of 663b12ebf44b7ed3872b385477381f4b11adeb0aec9e0e2478776313d536376dc8fd5f3c715bb6ddf32c01ee1d6f8b731785732c0d8441df636d8145577e7b3138e43c32a61bc1242e0e73d62d624cdc924856076bdbbf1ec04ad4420732ef0c53d42479a08235fcfc4db4d869c4eb2828c73928cdc3e3758362d1b770809997

uint8 ecc_521_public_key_x[] = "007d042ca19408524e68b981f1419351e3b84736c77fe58fee7d11317df2e850d960c7dd10d10ba714c8a609d163502b79d682e8bbecd4f52591d2748533e45a867a";

uint8 ecc_521_public_key_y[] = "0197ac6416111ccf987d290459ebc8ad9ec56e49059c992155539a36a626631f4a2d89164b985154f2dddc0281ee5b5178271f3a76a0914c3fcd1f97be8e8376efb3";

uint8 ecc_521_private_key[] = "01e8c05996b85e6f3f875712a09c1b40672b5e7a78d5852de01585c5fb990bf3812c3245534a714389ae9014d677a449efd658254e610da8e6cad33414b9d33e0d7a";

uint8 ecc_521_signature[] = "009dd1f2a716843eedec7a6645ac834d4336e7b18e35701f06cae9d6b290d41491424735f3b57e829ad5de055eaeef1778f051c1ee152bf2131a081e53df2a567a8a\
                             002148e8428d70a72bc9fa986c38c2c97deda0420f222f9dc99d32c0acba699dc7ba0a2b79ce5999ff61bd0b233c744a893bc105bca5c235423e531612da65d72e62"; // r \ s

uint8 ecc_521_hash[] = "3ba859e1d48b57b704fae35788755a9dd8b808c85323dc9c44c0c6ac799687ad"; // SHA256 digest of 8ab8176b16278db54f84328ae0b75ef8f0cd18afdf40c04ad0927ed0f6d9e47470396c8e87cde7a9be2ffbfe6c9658c88b7de4d582111119c433b2e4a504493f0a1166e3a3ea0d7b93358f4a297d63f65a5e752f94e2ee7f49ebcc742fa3eb03a617d00c574245b77a20033854d82964b2949e2247637239ab00baf4d170d97c

typedef struct
{
   char   *m; 
   char   *a;
   char   *b;
   char   *G_x;
   char   *G_y;
   char   *n;
   uint8  *public_key_x;
   uint8  *public_key_y;
   uint8  *private_key;   
   uint32 key_size;         //size in bits
   uint8  *signature;       //signature r followed by sig s
   uint32 sig_size;         //length of signature r + sig s in bytes
   uint8  *hash;   
   uint32 hash_size;        //size in bytes
} __attribute__ ((packed)) generic_ecc_vector_type;

generic_ecc_vector_type generic_ecc_test_vectors[] =
{
  { //Key length 160 bits
    "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7FFFFFFF",     //m
    "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7FFFFFFC",     //a
    "1C97BEFC54BD7A8B65ACF89F81D4D4ADC565FA45",     //b
    "4A96B5688EF573284664698968C38BB913CBFC82",     //G_x
    "23A628553168947D59DCC912042351377AC5FB32",     //G_y
    "0100000000000000000001F4C8F927AED3CA752257",   //n
    ecc_160_public_key_x,                           //public_key_x
    ecc_160_public_key_y,                           //public_key_y
    ecc_160_private_key,                            //private_key
    160,                                            //key size in bits
    ecc_160_signature,                              //sig_r followed by sig_s
    40,                                             //sig len r + s in bytes
    ecc_160_hash,                                   //hash
    20,                                             //hash size in bytes
  },

  { //Key length 192 bits
    "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFF",     //m                      
    "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFC",     //a                      
    "64210519E59C80E70FA7E9AB72243049FEB8DEECC146B9B1",     //b                      
    "188DA80EB03090F67CBF20EB43A18800F4FF0AFD82FF1012",     //G_x                    
    "07192B95FFC8DA78631011ED6B24CDD573F977A11E794811",     //G_y                    
    "FFFFFFFFFFFFFFFFFFFFFFFF99DEF836146BC9B1B4D22831",     //n
    ecc_192_public_key_x,                                   //public_key_x           
    ecc_192_public_key_y,                                   //public_key_y           
    ecc_192_private_key,                                    //private_key            
    192,                                                    //key size in bits
    ecc_192_signature,                                      //sig_r followed by sig_s
    48,                                                     //sig len r + s in bytes 
    ecc_192_hash,                                           //hash                   
    32,                                                     //hash size in bytes
  },
#if 0
  { //Key length 224 bits
    "ffffffffffffffffffffffffffffffff000000000000000000000001",     //m
    "fffffffffffffffffffffffffffffffefffffffffffffffffffffffe",     //a
    "b4050a850c04b3abf54132565044b0b7d7bfd8ba270b39432355ffb4",     //b
    "b70e0cbd6bb4bf7f321390b94a03c1d356c21122343280d6115c1d21",     //G_x
    "bd376388b5f723fb4c22dfe6cd4375a05a07476444d5819985007e34",     //G_y
    "ffffffffffffffffffffffffffff16a2e0b8f03e13dd29455c5c2a3d",     //n
    ecc_224_public_key_x,                                   //public_key_x
    ecc_224_public_key_y,                                   //public_key_y
    ecc_224_private_key,                                    //private_key
    224,                                                    //key size in bits
    ecc_224_signature,                                      //sig_r followed by sig_s
    56,                                                     //sig len r + s in bytes
    ecc_224_hash,                                           //hash
    32,                                                     //hash size in bytes
  },
#endif
  { //Key length 256 bits
    "ffffffff00000001000000000000000000000000ffffffffffffffffffffffff",    //m                      
    "ffffffff00000001000000000000000000000000fffffffffffffffffffffffc",    //a                      
    "5ac635d8aa3a93e7b3ebbd55769886bc651d06b0cc53b0f63bce3c3e27d2604b",    //b                      
    "6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296",    //G_x                    
    "4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5",    //G_y                    
    "ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551",    //n 
    ecc_256_public_key_x,                                                  //public_key_x           
    ecc_256_public_key_y,                                                  //public_key_y           
    ecc_256_private_key,                                                   //private_key            
    256,                                                                   //key size in bits
    ecc_256_signature,                                                     //sig_r followed by sig_s
    64,                                                                    //sig len r + s in bytes 
    ecc_256_hash,                                                          //hash                   
    32,                                                                    //hash size in bytes
  },
#if 0
  { //Key length 384 bits
    "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffeffffffff0000000000000000ffffffff",    //m
    "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffeffffffff0000000000000000fffffffc",    //a
    "b3312fa7e23ee7e4988e056be3f82d19181d9c6efe8141120314088f5013875ac656398d8a2ed19d2a85c8edd3ec2aef",    //b
    "aa87ca22be8b05378eb1c71ef320ad746e1d3b628ba79b9859f741e082542a385502f25dbf55296c3a545e3872760ab7",    //G_x
    "3617DE4A96262C6F5D9E98BF9292DC29F8F41DBD289A147CE9DA3113B5F0B8C00A60B1CE1D7E819D7A431D7C90EA0E5F",    //G_y
    "ffffffffffffffffffffffffffffffffffffffffffffffffc7634d81f4372ddf581a0db248b0a77aecec196accc52973",    //n
    ecc_384_public_key_x,                                                  //public_key_x
    ecc_384_public_key_y,                                                  //public_key_y
    ecc_384_private_key,                                                   //private_key
    384,                                                                   //keysize in bits
    ecc_384_signature,                                                     //sig_r followed by sig_s
    96,                                                                    //sig len r + s in bytes
    ecc_384_hash,                                                          //hash
    32,                                                                    //hash size in bytes
  },
  { //Key length 521 bits (will be 528 to be multiple of 8)
    "01ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff",    //m
    "01fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffc",    //a
    "0051953eb9618e1c9a1f929a21a0b68540eea2da725b99b315f3b8b489918ef109e156193951ec7e937b1652c0bd3bb1bf073573df883d2c34f1ef451fd46b503f00",    //b
    "00c6858e06b70404e9cd9e3ecb662395b4429c648139053fb521f828af606b4d3dbaa14b5e77efe75928fe1dc127a2ffa8de3348b3c1856a429bf97e7e31c2e5bd66",    //G_x
    "011839296a789a3bc0045c8a5fb42c7d1bd998f54449579b446817afbd17273e662c97ee72995ef42640c550b9013fad0761353c7086a272c24088be94769fd16650",    //G_y
    "01fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffa51868783bf2f966b7fcc0148f709a5d03bb5c9b8899c47aebb6fb71e91386409",    //n
    ecc_521_public_key_x,                                                  //public_key_x
    ecc_521_public_key_y,                                                  //public_key_y
    ecc_521_private_key,                                                   //private_key
    521,                                                                   //keysize in bits
    ecc_521_signature,                                                     //sig_r followed by sig_s
    132,                                                                   //sig len r + s in bytes
    ecc_521_hash,                                                          //hash
    32,                                                                    //hash size in bytes
  },
#endif
};
 
//Set up array of pointers to test vectors
#if 0 
generic_ecc_vector_type *generic_ecc_test_vectors_ptr[] = {&generic_ecc_test_vectors[0], &generic_ecc_test_vectors[1], &generic_ecc_test_vectors[2], &generic_ecc_test_vectors[3], &generic_ecc_test_vectors[4] , &generic_ecc_test_vectors[5]};
#endif
generic_ecc_vector_type *generic_ecc_test_vectors_ptr[] = {&generic_ecc_test_vectors[0], &generic_ecc_test_vectors[1], &generic_ecc_test_vectors[2]};

uint8 hash_binary[40];

//SW AES - CCM Vectors 
//Key data
UINT8_A ccm128_key_1[] = 
{
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f 
};

//IV/Nonce
UINT8_A ccm128_iv_1[] = 
{
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16
};

//Header data
UINT8_A ccm128_header_data_1[] = 
{
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
};

//Plain text
UINT8_A ccm128_plain_text_1[] = 
{
  0x20, 0x21, 0x22, 0x23 
};

//Cipher text
UINT8_A ccm128_cipher_text_1[] = 
{
  0x71, 0x62, 0x01, 0x5b,   
};

//MAC Data
UINT8_A ccm128_mac_data_1[] = 
{  
  0x4d, 0xac, 0x25, 0x5d
};

//Key data
UINT8_A ccm128_key_2[] = 
{
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f 
};

//IV/Nonce
UINT8_A ccm128_iv_2[] = 
{
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17
};

//Header data
UINT8_A ccm128_header_data_2[] = 
{
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
};

//Plain text 
UINT8_A ccm128_plain_text_2[] = 
{
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 
};

//Cipher text
UINT8_A ccm128_cipher_text_2[] = 
{
  0xd2, 0xa1, 0xf0, 0xe0, 0x51, 0xea, 0x5f, 0x62, 0x08, 0x1a, 0x77, 0x92, 0x07, 0x3d, 0x59, 0x3d,  
};

//MAC Data
UINT8_A ccm128_mac_data_2[] = 
{
  0x1f, 0xc6, 0x4f, 0xbf, 0xac, 0xcd
};
  
//Key data
UINT8_A ccm128_key_3[] = 
{
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f 
};

//IV/Nonce
UINT8_A ccm128_iv_3[] = 
{
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 
};

//Header data
UINT8_A ccm128_header_data_3[] = 
{
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13
};

//Plain text
UINT8_A ccm128_plain_text_3[] = 
{ 
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
};

//Cipher text
UINT8_A ccm128_cipher_text_3[] = 
{
  0xe3, 0xb2, 0x01, 0xa9, 0xf5, 0xb7, 0x1a, 0x7a, 0x9b, 0x1c, 0xea, 0xec, 0xcd, 0x97, 0xe7, 0x0b, 
  0x61, 0x76, 0xaa, 0xd9, 0xa4, 0x42, 0x8a, 0xa5, 
};

//MAC Data
UINT8_A ccm128_mac_data_3[] = 
{  
  0x48, 0x43, 0x92, 0xfb, 0xc1, 0xb0, 0x99, 0x51
}; 
 
typedef struct
{
  uint8*               pt;
  uint32               pt_len;
  uint8*               key;
  uint32               key_len;
  uint8*               iv;
  uint32               iv_len;
  uint8*               ct;
  uint8*               header;
  uint32               header_len;
  uint8*               mac;
  uint32               mac_len;
} __attribute__ ((packed)) uf_aes_ccm_vector_type; 

/* AES CCM test vectors array */
uf_aes_ccm_vector_type aes_ccm_test_vectors_uf[] =
{
  { ccm128_plain_text_1, sizeof(ccm128_plain_text_1), ccm128_key_1, sizeof(ccm128_key_1), 
    ccm128_iv_1, sizeof(ccm128_iv_1), ccm128_cipher_text_1, ccm128_header_data_1, 
    sizeof(ccm128_header_data_1), ccm128_mac_data_1, sizeof(ccm128_mac_data_1) }
  ,

  { ccm128_plain_text_2, sizeof(ccm128_plain_text_2), ccm128_key_2, sizeof(ccm128_key_2), 
    ccm128_iv_2, sizeof(ccm128_iv_2), ccm128_cipher_text_2, ccm128_header_data_2, 
    sizeof(ccm128_header_data_2), ccm128_mac_data_2, sizeof(ccm128_mac_data_2) }
  ,

  { ccm128_plain_text_3, sizeof(ccm128_plain_text_3), ccm128_key_3, sizeof(ccm128_key_3), 
    ccm128_iv_3, sizeof(ccm128_iv_3), ccm128_cipher_text_3, ccm128_header_data_3, 
    sizeof(ccm128_header_data_3), ccm128_mac_data_3, sizeof(ccm128_mac_data_3) }
  ,
};

/* AES-128 ECB test vectors */
UINT8_A aes_plain_text_1[] = 
{
  0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 
  0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
};

UINT8_A aes_key_1[] = 
{
  0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 
  0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};

UINT8_A aes_cipher_text_1[] = 
{
  0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60, 
  0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97
};

/* AES-128 CBC test vectors */
UINT8_A aes_plain_text_2[] = 
{
  0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 
  0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
};

UINT8_A aes_key_2[] = 
{
  0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 
  0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};

UINT8_A aes_iv_2[] = 
{
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};

UINT8_A aes_cipher_text_2[] = {
  0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46, 
  0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d
};

/* AES-128 CTR test vectors */
UINT8_A aes_plain_text_3[] = 
{
  0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 
  0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
};

UINT8_A aes_key_3[] = {
  0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 
  0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};

UINT8_A aes_iv_3[] = 
{
  0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 
  0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

UINT8_A aes_cipher_text_3[] = {
  0x87, 0x4d, 0x61, 0x91, 0xb6, 0x20, 0xe3, 0x26, 
  0x1b, 0xef, 0x68, 0x64, 0x99, 0x0d, 0xb6, 0xce
};

UINT8_A aes_iv_4[] = 
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* AES-256 ECB test vectors */
UINT8_A aes256_plain_text_1[] =
{
  0x06, 0x9A, 0x00, 0x7F, 0xC7, 0x6A, 0x45, 0x9F,
  0x98, 0xBA, 0xF9, 0x17, 0xFE, 0xDF, 0x95, 0x21
};

UINT8_A aes256_key_1[] =
{
  0x08, 0x09, 0x0A, 0x0B, 0x0D, 0x0E, 0x0F, 0x10,
  0x12, 0x13, 0x14, 0x15, 0x17, 0x18, 0x19, 0x1A,
  0x1C, 0x1D, 0x1E, 0x1F, 0x21, 0x22, 0x23, 0x24,
  0x26, 0x27, 0x28, 0x29, 0x2B, 0x2C, 0x2D, 0x2E
};

UINT8_A aes256_cipher_text_1[] =
{
  0x08, 0x0e, 0x95, 0x17, 0xeb, 0x16, 0x77, 0x71, 
  0x9a, 0xcf, 0x72, 0x80, 0x86, 0x04, 0x0a, 0xe3
};

/* AES-256 CBC test vectors */
UINT8_A aes256_plain_text_2[] =
{
  0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
  0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
};

UINT8_A aes256_key_2[] =
{
  0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
  0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
  0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
  0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
};

UINT8_A aes256_iv_2[] =
{
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};

UINT8_A aes256_cipher_text_2[] =
{
  0xf5, 0x8c, 0x4c, 0x04, 0xd6, 0xe5, 0xf1, 0xba,
  0x77, 0x9e, 0xab, 0xfb, 0x5f, 0x7b, 0xfb, 0xd6
};

/* AES-256 CTR test vectors */
UINT8_A aes256_plain_text_3[] =
{
  0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
  0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
};

UINT8_A aes256_key_3[] =
{
  0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
  0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
  0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
  0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4

};

UINT8_A aes256_iv_3[] =
{
  0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
  0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

UINT8_A aes256_cipher_text_3[] =
{
  0x60, 0x1e, 0xc3, 0x13, 0x77, 0x57, 0x89, 0xa5,
  0xb7, 0xa7, 0xf5, 0x04, 0xbb, 0xf3, 0xd2, 0x28
};

typedef struct aes_vector_type
{
  QSEE_CIPHER_ALGO_ET  alg;
  QSEE_CIPHER_MODE_ET  mode;
  uint8*               pt;
  uint32               pt_len;
  uint8*               key;
  uint32               key_len;
  uint8*               iv;
  uint8*               ct;
} __attribute__ ((packed)) aes_vector_type_t;

typedef struct
{
  QSEE_CIPHER_ALGO_ET  alg;
  QSEE_CIPHER_MODE_ET  mode;
  uint8*               pt;
  uint32               pt_len;
  uint8*               key;
  uint32               key_len;
  uint8*               iv;
  uint8*               ct;
  uint8*               ct2;
} __attribute__ ((packed)) aes_vector_t_2;

/* AES-128 and AES-256 test vectors array */
aes_vector_type_t aes_test_vectors[] = 
{
#if 0 // TonyB
  {QSEE_CIPHER_ALGO_AES_128, QSEE_CIPHER_MODE_CTS, aes_plain_text_2,
   sizeof(aes_plain_text_2), aes_key_2, 16,
   aes_iv_2, aes_cipher_text_2}
  ,
#endif		
  {QSEE_CIPHER_ALGO_AES_128, QSEE_CIPHER_MODE_ECB, aes_plain_text_1,
   sizeof(aes_plain_text_1), aes_key_1, 16, NULL,
   aes_cipher_text_1}
  ,
  {QSEE_CIPHER_ALGO_AES_128, QSEE_CIPHER_MODE_CBC, aes_plain_text_2,
   sizeof(aes_plain_text_2), aes_key_2, 16,
   aes_iv_2, aes_cipher_text_2}
  ,
  {QSEE_CIPHER_ALGO_AES_128, QSEE_CIPHER_MODE_CTR, aes_plain_text_3,
   sizeof(aes_plain_text_3), aes_key_3, 16,
   aes_iv_3, aes_cipher_text_3}
  ,
  {QSEE_CIPHER_ALGO_AES_256, QSEE_CIPHER_MODE_ECB, aes256_plain_text_1,
   sizeof(aes256_plain_text_1), aes256_key_1, 32,
   NULL, aes256_cipher_text_1}
  ,
  {QSEE_CIPHER_ALGO_AES_256, QSEE_CIPHER_MODE_CBC, aes256_plain_text_2,
   sizeof(aes256_plain_text_2), aes256_key_2, 32,
   aes256_iv_2, aes256_cipher_text_2}
  ,
  {QSEE_CIPHER_ALGO_AES_256, QSEE_CIPHER_MODE_CTR, aes256_plain_text_3,
   sizeof(aes256_plain_text_3), aes256_key_3, 32,
   aes256_iv_3, aes256_cipher_text_3}
  ,
};

aes_vector_t_2 aes_test_vectors_2 = 
{
  QSEE_CIPHER_ALGO_AES_128, QSEE_CIPHER_MODE_CBC, NULL, NULL, 
  aes_key_1, sizeof(aes_key_1), aes_iv_2, NULL, NULL
}; 

typedef struct uf_aes_vector_type
{
  SW_Cipher_Alg_Type   alg;
  SW_CipherModeType    mode;
  uint8*               pt;
  uint32               pt_len;
  uint8*               key;
  uint32               key_len;
  uint8*               iv;
  uint8*               ct;
} __attribute__ ((packed)) uf_aes_vector_type_t;

/* AES-128 test vectors array */
uf_aes_vector_type_t sw_aes_test_vectors[] = 
{
  {SW_CIPHER_ALG_AES128, SW_CIPHER_MODE_ECB, aes_plain_text_1,
   sizeof(aes_plain_text_1), aes_key_1, 16, NULL,
   aes_cipher_text_1}
  ,
  {SW_CIPHER_ALG_AES128, SW_CIPHER_MODE_CBC, aes_plain_text_2,
   sizeof(aes_plain_text_2), aes_key_2, 16,
   aes_iv_2, aes_cipher_text_2}
  ,
  {SW_CIPHER_ALG_AES128, SW_CIPHER_MODE_CTR, aes_plain_text_3,
   sizeof(aes_plain_text_3), aes_key_3, 16,
   aes_iv_3, aes_cipher_text_3}
  ,
};

/* AES GCM test vectors */
#define MAX_TESTS   6

int key_index_gcm[MAX_TESTS] =
    { 0, 0, 1, 1, 1, 1 };

unsigned char key_gcm[MAX_TESTS][32] =
{
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c,
      0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08,
      0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c,
      0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08 },
};

size_t iv_len_gcm[MAX_TESTS] =
    { 12, 12, 12, 12, 8, 60 };

int iv_index_gcm[MAX_TESTS] =
    { 0, 0, 1, 1, 1, 2 };

unsigned char iv_gcm[MAX_TESTS][64] =
{
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00 },
    { 0xca, 0xfe, 0xba, 0xbe, 0xfa, 0xce, 0xdb, 0xad,
      0xde, 0xca, 0xf8, 0x88 },
    { 0x93, 0x13, 0x22, 0x5d, 0xf8, 0x84, 0x06, 0xe5,
      0x55, 0x90, 0x9c, 0x5a, 0xff, 0x52, 0x69, 0xaa,
      0x6a, 0x7a, 0x95, 0x38, 0x53, 0x4f, 0x7d, 0xa1,
      0xe4, 0xc3, 0x03, 0xd2, 0xa3, 0x18, 0xa7, 0x28,
      0xc3, 0xc0, 0xc9, 0x51, 0x56, 0x80, 0x95, 0x39,
      0xfc, 0xf0, 0xe2, 0x42, 0x9a, 0x6b, 0x52, 0x54,
      0x16, 0xae, 0xdb, 0xf5, 0xa0, 0xde, 0x6a, 0x57,
      0xa6, 0x37, 0xb3, 0x9b },
};

size_t add_len_gcm[MAX_TESTS] =
    { 0, 0, 0, 20, 20, 20 };

int add_index_gcm[MAX_TESTS] =
    { 0, 0, 0, 1, 1, 1 };

unsigned char additional_gcm[MAX_TESTS][64] =
{
    { 0x00 },
    { 0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef,
      0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef,
      0xab, 0xad, 0xda, 0xd2 },
};

size_t pt_len_gcm[MAX_TESTS] =
    { 0, 16, 64, 60, 60, 60 };

int pt_index_gcm[MAX_TESTS] =
    { 0, 0, 1, 1, 1, 1 };

unsigned char pt_gcm[MAX_TESTS][64] =
{
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0xd9, 0x31, 0x32, 0x25, 0xf8, 0x84, 0x06, 0xe5,
      0xa5, 0x59, 0x09, 0xc5, 0xaf, 0xf5, 0x26, 0x9a,
      0x86, 0xa7, 0xa9, 0x53, 0x15, 0x34, 0xf7, 0xda,
      0x2e, 0x4c, 0x30, 0x3d, 0x8a, 0x31, 0x8a, 0x72,
      0x1c, 0x3c, 0x0c, 0x95, 0x95, 0x68, 0x09, 0x53,
      0x2f, 0xcf, 0x0e, 0x24, 0x49, 0xa6, 0xb5, 0x25,
      0xb1, 0x6a, 0xed, 0xf5, 0xaa, 0x0d, 0xe6, 0x57,
      0xba, 0x63, 0x7b, 0x39, 0x1a, 0xaf, 0xd2, 0x55 },
};

unsigned char ct_gcm[MAX_TESTS * 3][64] =
{
    { 0x00 },
    { 0x03, 0x88, 0xda, 0xce, 0x60, 0xb6, 0xa3, 0x92,
      0xf3, 0x28, 0xc2, 0xb9, 0x71, 0xb2, 0xfe, 0x78 },
    { 0x42, 0x83, 0x1e, 0xc2, 0x21, 0x77, 0x74, 0x24,
      0x4b, 0x72, 0x21, 0xb7, 0x84, 0xd0, 0xd4, 0x9c,
      0xe3, 0xaa, 0x21, 0x2f, 0x2c, 0x02, 0xa4, 0xe0,
      0x35, 0xc1, 0x7e, 0x23, 0x29, 0xac, 0xa1, 0x2e,
      0x21, 0xd5, 0x14, 0xb2, 0x54, 0x66, 0x93, 0x1c,
      0x7d, 0x8f, 0x6a, 0x5a, 0xac, 0x84, 0xaa, 0x05,
      0x1b, 0xa3, 0x0b, 0x39, 0x6a, 0x0a, 0xac, 0x97,
      0x3d, 0x58, 0xe0, 0x91, 0x47, 0x3f, 0x59, 0x85 },
    { 0x42, 0x83, 0x1e, 0xc2, 0x21, 0x77, 0x74, 0x24,
      0x4b, 0x72, 0x21, 0xb7, 0x84, 0xd0, 0xd4, 0x9c,
      0xe3, 0xaa, 0x21, 0x2f, 0x2c, 0x02, 0xa4, 0xe0,
      0x35, 0xc1, 0x7e, 0x23, 0x29, 0xac, 0xa1, 0x2e,
      0x21, 0xd5, 0x14, 0xb2, 0x54, 0x66, 0x93, 0x1c,
      0x7d, 0x8f, 0x6a, 0x5a, 0xac, 0x84, 0xaa, 0x05,
      0x1b, 0xa3, 0x0b, 0x39, 0x6a, 0x0a, 0xac, 0x97,
      0x3d, 0x58, 0xe0, 0x91 },
    { 0x61, 0x35, 0x3b, 0x4c, 0x28, 0x06, 0x93, 0x4a,
      0x77, 0x7f, 0xf5, 0x1f, 0xa2, 0x2a, 0x47, 0x55,
      0x69, 0x9b, 0x2a, 0x71, 0x4f, 0xcd, 0xc6, 0xf8,
      0x37, 0x66, 0xe5, 0xf9, 0x7b, 0x6c, 0x74, 0x23,
      0x73, 0x80, 0x69, 0x00, 0xe4, 0x9f, 0x24, 0xb2,
      0x2b, 0x09, 0x75, 0x44, 0xd4, 0x89, 0x6b, 0x42,
      0x49, 0x89, 0xb5, 0xe1, 0xeb, 0xac, 0x0f, 0x07,
      0xc2, 0x3f, 0x45, 0x98 },
    { 0x8c, 0xe2, 0x49, 0x98, 0x62, 0x56, 0x15, 0xb6,
      0x03, 0xa0, 0x33, 0xac, 0xa1, 0x3f, 0xb8, 0x94,
      0xbe, 0x91, 0x12, 0xa5, 0xc3, 0xa2, 0x11, 0xa8,
      0xba, 0x26, 0x2a, 0x3c, 0xca, 0x7e, 0x2c, 0xa7,
      0x01, 0xe4, 0xa9, 0xa4, 0xfb, 0xa4, 0x3c, 0x90,
      0xcc, 0xdc, 0xb2, 0x81, 0xd4, 0x8c, 0x7c, 0x6f,
      0xd6, 0x28, 0x75, 0xd2, 0xac, 0xa4, 0x17, 0x03,
      0x4c, 0x34, 0xae, 0xe5 },
    { 0x00 },
    { 0x98, 0xe7, 0x24, 0x7c, 0x07, 0xf0, 0xfe, 0x41,
      0x1c, 0x26, 0x7e, 0x43, 0x84, 0xb0, 0xf6, 0x00 },
    { 0x39, 0x80, 0xca, 0x0b, 0x3c, 0x00, 0xe8, 0x41,
      0xeb, 0x06, 0xfa, 0xc4, 0x87, 0x2a, 0x27, 0x57,
      0x85, 0x9e, 0x1c, 0xea, 0xa6, 0xef, 0xd9, 0x84,
      0x62, 0x85, 0x93, 0xb4, 0x0c, 0xa1, 0xe1, 0x9c,
      0x7d, 0x77, 0x3d, 0x00, 0xc1, 0x44, 0xc5, 0x25,
      0xac, 0x61, 0x9d, 0x18, 0xc8, 0x4a, 0x3f, 0x47,
      0x18, 0xe2, 0x44, 0x8b, 0x2f, 0xe3, 0x24, 0xd9,
      0xcc, 0xda, 0x27, 0x10, 0xac, 0xad, 0xe2, 0x56 },
    { 0x39, 0x80, 0xca, 0x0b, 0x3c, 0x00, 0xe8, 0x41,
      0xeb, 0x06, 0xfa, 0xc4, 0x87, 0x2a, 0x27, 0x57,
      0x85, 0x9e, 0x1c, 0xea, 0xa6, 0xef, 0xd9, 0x84,
      0x62, 0x85, 0x93, 0xb4, 0x0c, 0xa1, 0xe1, 0x9c,
      0x7d, 0x77, 0x3d, 0x00, 0xc1, 0x44, 0xc5, 0x25,
      0xac, 0x61, 0x9d, 0x18, 0xc8, 0x4a, 0x3f, 0x47,
      0x18, 0xe2, 0x44, 0x8b, 0x2f, 0xe3, 0x24, 0xd9,
      0xcc, 0xda, 0x27, 0x10 },
    { 0x0f, 0x10, 0xf5, 0x99, 0xae, 0x14, 0xa1, 0x54,
      0xed, 0x24, 0xb3, 0x6e, 0x25, 0x32, 0x4d, 0xb8,
      0xc5, 0x66, 0x63, 0x2e, 0xf2, 0xbb, 0xb3, 0x4f,
      0x83, 0x47, 0x28, 0x0f, 0xc4, 0x50, 0x70, 0x57,
      0xfd, 0xdc, 0x29, 0xdf, 0x9a, 0x47, 0x1f, 0x75,
      0xc6, 0x65, 0x41, 0xd4, 0xd4, 0xda, 0xd1, 0xc9,
      0xe9, 0x3a, 0x19, 0xa5, 0x8e, 0x8b, 0x47, 0x3f,
      0xa0, 0xf0, 0x62, 0xf7 },
    { 0xd2, 0x7e, 0x88, 0x68, 0x1c, 0xe3, 0x24, 0x3c,
      0x48, 0x30, 0x16, 0x5a, 0x8f, 0xdc, 0xf9, 0xff,
      0x1d, 0xe9, 0xa1, 0xd8, 0xe6, 0xb4, 0x47, 0xef,
      0x6e, 0xf7, 0xb7, 0x98, 0x28, 0x66, 0x6e, 0x45,
      0x81, 0xe7, 0x90, 0x12, 0xaf, 0x34, 0xdd, 0xd9,
      0xe2, 0xf0, 0x37, 0x58, 0x9b, 0x29, 0x2d, 0xb3,
      0xe6, 0x7c, 0x03, 0x67, 0x45, 0xfa, 0x22, 0xe7,
      0xe9, 0xb7, 0x37, 0x3b },
    { 0x00 },
    { 0xce, 0xa7, 0x40, 0x3d, 0x4d, 0x60, 0x6b, 0x6e,
      0x07, 0x4e, 0xc5, 0xd3, 0xba, 0xf3, 0x9d, 0x18 },
    { 0x52, 0x2d, 0xc1, 0xf0, 0x99, 0x56, 0x7d, 0x07,
      0xf4, 0x7f, 0x37, 0xa3, 0x2a, 0x84, 0x42, 0x7d,
      0x64, 0x3a, 0x8c, 0xdc, 0xbf, 0xe5, 0xc0, 0xc9,
      0x75, 0x98, 0xa2, 0xbd, 0x25, 0x55, 0xd1, 0xaa,
      0x8c, 0xb0, 0x8e, 0x48, 0x59, 0x0d, 0xbb, 0x3d,
      0xa7, 0xb0, 0x8b, 0x10, 0x56, 0x82, 0x88, 0x38,
      0xc5, 0xf6, 0x1e, 0x63, 0x93, 0xba, 0x7a, 0x0a,
      0xbc, 0xc9, 0xf6, 0x62, 0x89, 0x80, 0x15, 0xad },
    { 0x52, 0x2d, 0xc1, 0xf0, 0x99, 0x56, 0x7d, 0x07,
      0xf4, 0x7f, 0x37, 0xa3, 0x2a, 0x84, 0x42, 0x7d,
      0x64, 0x3a, 0x8c, 0xdc, 0xbf, 0xe5, 0xc0, 0xc9,
      0x75, 0x98, 0xa2, 0xbd, 0x25, 0x55, 0xd1, 0xaa,
      0x8c, 0xb0, 0x8e, 0x48, 0x59, 0x0d, 0xbb, 0x3d,
      0xa7, 0xb0, 0x8b, 0x10, 0x56, 0x82, 0x88, 0x38,
      0xc5, 0xf6, 0x1e, 0x63, 0x93, 0xba, 0x7a, 0x0a,
      0xbc, 0xc9, 0xf6, 0x62 },
    { 0xc3, 0x76, 0x2d, 0xf1, 0xca, 0x78, 0x7d, 0x32,
      0xae, 0x47, 0xc1, 0x3b, 0xf1, 0x98, 0x44, 0xcb,
      0xaf, 0x1a, 0xe1, 0x4d, 0x0b, 0x97, 0x6a, 0xfa,
      0xc5, 0x2f, 0xf7, 0xd7, 0x9b, 0xba, 0x9d, 0xe0,
      0xfe, 0xb5, 0x82, 0xd3, 0x39, 0x34, 0xa4, 0xf0,
      0x95, 0x4c, 0xc2, 0x36, 0x3b, 0xc7, 0x3f, 0x78,
      0x62, 0xac, 0x43, 0x0e, 0x64, 0xab, 0xe4, 0x99,
      0xf4, 0x7c, 0x9b, 0x1f },
    { 0x5a, 0x8d, 0xef, 0x2f, 0x0c, 0x9e, 0x53, 0xf1,
      0xf7, 0x5d, 0x78, 0x53, 0x65, 0x9e, 0x2a, 0x20,
      0xee, 0xb2, 0xb2, 0x2a, 0xaf, 0xde, 0x64, 0x19,
      0xa0, 0x58, 0xab, 0x4f, 0x6f, 0x74, 0x6b, 0xf4,
      0x0f, 0xc0, 0xc3, 0xb7, 0x80, 0xf2, 0x44, 0x45,
      0x2d, 0xa3, 0xeb, 0xf1, 0xc5, 0xd8, 0x2c, 0xde,
      0xa2, 0x41, 0x89, 0x97, 0x20, 0x0e, 0xf8, 0x2e,
      0x44, 0xae, 0x7e, 0x3f },
};

unsigned char tag_gcm[MAX_TESTS * 3][16] =
{
    { 0x58, 0xe2, 0xfc, 0xce, 0xfa, 0x7e, 0x30, 0x61,
      0x36, 0x7f, 0x1d, 0x57, 0xa4, 0xe7, 0x45, 0x5a },
    { 0xab, 0x6e, 0x47, 0xd4, 0x2c, 0xec, 0x13, 0xbd,
      0xf5, 0x3a, 0x67, 0xb2, 0x12, 0x57, 0xbd, 0xdf },
    { 0x4d, 0x5c, 0x2a, 0xf3, 0x27, 0xcd, 0x64, 0xa6,
      0x2c, 0xf3, 0x5a, 0xbd, 0x2b, 0xa6, 0xfa, 0xb4 },
    { 0x5b, 0xc9, 0x4f, 0xbc, 0x32, 0x21, 0xa5, 0xdb,
      0x94, 0xfa, 0xe9, 0x5a, 0xe7, 0x12, 0x1a, 0x47 },
    { 0x36, 0x12, 0xd2, 0xe7, 0x9e, 0x3b, 0x07, 0x85,
      0x56, 0x1b, 0xe1, 0x4a, 0xac, 0xa2, 0xfc, 0xcb },
    { 0x61, 0x9c, 0xc5, 0xae, 0xff, 0xfe, 0x0b, 0xfa,
      0x46, 0x2a, 0xf4, 0x3c, 0x16, 0x99, 0xd0, 0x50 },
    { 0xcd, 0x33, 0xb2, 0x8a, 0xc7, 0x73, 0xf7, 0x4b,
      0xa0, 0x0e, 0xd1, 0xf3, 0x12, 0x57, 0x24, 0x35 },
    { 0x2f, 0xf5, 0x8d, 0x80, 0x03, 0x39, 0x27, 0xab,
      0x8e, 0xf4, 0xd4, 0x58, 0x75, 0x14, 0xf0, 0xfb },
    { 0x99, 0x24, 0xa7, 0xc8, 0x58, 0x73, 0x36, 0xbf,
      0xb1, 0x18, 0x02, 0x4d, 0xb8, 0x67, 0x4a, 0x14 },
    { 0x25, 0x19, 0x49, 0x8e, 0x80, 0xf1, 0x47, 0x8f,
      0x37, 0xba, 0x55, 0xbd, 0x6d, 0x27, 0x61, 0x8c },
    { 0x65, 0xdc, 0xc5, 0x7f, 0xcf, 0x62, 0x3a, 0x24,
      0x09, 0x4f, 0xcc, 0xa4, 0x0d, 0x35, 0x33, 0xf8 },
    { 0xdc, 0xf5, 0x66, 0xff, 0x29, 0x1c, 0x25, 0xbb,
      0xb8, 0x56, 0x8f, 0xc3, 0xd3, 0x76, 0xa6, 0xd9 },
    { 0x53, 0x0f, 0x8a, 0xfb, 0xc7, 0x45, 0x36, 0xb9,
      0xa9, 0x63, 0xb4, 0xf1, 0xc4, 0xcb, 0x73, 0x8b },
    { 0xd0, 0xd1, 0xc8, 0xa7, 0x99, 0x99, 0x6b, 0xf0,
      0x26, 0x5b, 0x98, 0xb5, 0xd4, 0x8a, 0xb9, 0x19 },
    { 0xb0, 0x94, 0xda, 0xc5, 0xd9, 0x34, 0x71, 0xbd,
      0xec, 0x1a, 0x50, 0x22, 0x70, 0xe3, 0xcc, 0x6c },
    { 0x76, 0xfc, 0x6e, 0xce, 0x0f, 0x4e, 0x17, 0x68,
      0xcd, 0xdf, 0x88, 0x53, 0xbb, 0x2d, 0x55, 0x1b },
    { 0x3a, 0x33, 0x7d, 0xbf, 0x46, 0xa7, 0x92, 0xc4,
      0x5e, 0x45, 0x49, 0x13, 0xfe, 0x2e, 0xa8, 0xf2 },
    { 0xa4, 0x4a, 0x82, 0x66, 0xee, 0x1c, 0x8e, 0xb0,
      0xc8, 0xb5, 0xd4, 0xcf, 0x5a, 0xe9, 0xf1, 0x9a },
};
//Triple DES vectors 

//CBC vectors  
UINT8_A triple_des_iv_1_cbc[] = 
{
 0x21, 0xC1, 0xB0, 0xA1, 0xD8, 0x50, 0x6C, 0x28 
};

UINT8_A triple_des_key_1_cbc[] = 
{
  0x9A, 0x72, 0xED, 0x27, 0x5C, 0x8E, 0x8B, 0x3F,
  0x17, 0xAF, 0x0D, 0x56, 0xC9, 0x14, 0x42, 0x9A,
  0x9A, 0x72, 0xED, 0x27, 0x5C, 0x8E, 0x8B, 0x3F
};

UINT8_A triple_des_plain_text_1_cbc[] = 
{
  0x3F, 0xD5, 0xBF, 0xAB, 0xFF, 0x94, 0x5F, 0x8B,
  0x8F, 0x84, 0x67, 0x83, 0x93, 0x80, 0x69, 0x81,
  0x94, 0x81, 0xCA, 0x40, 0x65, 0x20, 0x12, 0xD1,
  0x89, 0x68, 0x64, 0xF5, 0xB2, 0x7A, 0x59, 0x3D,
  0x8C, 0xDF, 0xC6, 0x6F, 0xE3, 0x37, 0xD1, 0xDA,
  0x48, 0xAC, 0x24, 0x56, 0x12, 0x2B, 0x89, 0x15,
  0xE4, 0xCB, 0xF2, 0x65, 0xF9, 0x32, 0x5C, 0xD8,
  0x2E, 0x6C, 0x17, 0x36, 0x2B, 0xDA, 0x35, 0xAC
};

UINT8_A triple_des_cipher_text_1_cbc[] = 
{
  0xF5, 0xEB, 0x5A, 0x83, 0x52, 0x97, 0x21, 0xAC,
  0x07, 0x60, 0x6B, 0x58, 0x2D, 0xF7, 0x9F, 0x2B,
  0x5B, 0x68, 0x20, 0x53, 0x2C, 0x4F, 0xD7, 0x0F,
  0x5B, 0xDD, 0x23, 0xE7, 0x69, 0xA7, 0x55, 0x17,
  0x62, 0x94, 0x51, 0x31, 0x24, 0xCE, 0x84, 0x35,
  0x45, 0x16, 0xB9, 0x80, 0xBE, 0x7C, 0x8B, 0xF0,
  0x70, 0x3E, 0x8C, 0x63, 0x6E, 0x52, 0x83, 0xD9,
  0xBC, 0x57, 0xFF, 0x5A, 0xFF, 0x41, 0x89, 0xB8  
};

UINT8_A triple_des_iv_2_cbc[] =
{
  0x6E, 0xAB, 0x13, 0xB8, 0x36, 0xEE, 0xF5, 0xC3
};

UINT8_A triple_des_key_2_cbc[] =
{
  0x21, 0x28, 0xCA, 0x88, 0xFC, 0xB8, 0x3C, 0xB7,
  0x22, 0xF3, 0xDB, 0x35, 0x74, 0xA9, 0x5C, 0xC5,
  0x50, 0x99, 0x48, 0x99, 0xE8, 0x21, 0xD4, 0x8B 
};

UINT8_A triple_des_plain_text_2_cbc[] =
{
  0x4E, 0xBD, 0x88, 0xB7, 0xBA, 0xBC, 0xEF, 0x94, 
  0x51, 0x07, 0x81, 0x35, 0x1A, 0x9B, 0x74, 0x6C, 
  0x04, 0xDA, 0x15, 0x4C, 0x33, 0x42, 0x4C, 0x00, 
  0xB6, 0x20, 0x9B, 0x6C, 0x6B, 0x9D, 0x21, 0x25, 
  0x65, 0x8B, 0xB4, 0xE4, 0x2B, 0x46, 0x32, 0x85, 
  0xFC, 0xFA, 0xDB, 0x3A, 0x91, 0x76, 0x18, 0x09, 
  0x5C, 0xA4, 0xC9, 0xE9, 0x46, 0xC4, 0x8F, 0xE5, 
  0xCB, 0x79, 0x24, 0x0D, 0xEA, 0x9D, 0xAC, 0x7B, 
  0xBE, 0x06, 0x8B, 0xE9, 0x49, 0x99, 0x71, 0x10, 
  0x7E, 0x6F, 0x25, 0x01, 0x33, 0x79, 0xE8, 0x22, 
  0x58, 0xCB, 0x35, 0x7E, 0xEC, 0xEE, 0x55, 0x41, 
  0x51, 0xC7, 0xC0, 0xAE, 0x15, 0xE4, 0xA8, 0xED, 
  0x3A, 0x16, 0xBE, 0x85, 0x71, 0x7B, 0xC2, 0x1C, 
  0xC2, 0x7E, 0x5E, 0xB4, 0x98, 0xA0, 0x61, 0xDF,
  0xB6, 0x5B, 0x03, 0x62, 0x8B, 0xAF, 0x44, 0xB8,
  0xE8, 0x68, 0xAE, 0x27, 0xCB, 0x5F, 0x0B, 0x87
}; 

UINT8_A triple_des_cipher_text_2_cbc[] =
{
  0x4F, 0x76, 0x86, 0xDA, 0x38, 0x4D, 0xAA, 0x3B,
  0x0C, 0x20, 0x04, 0x76, 0x00, 0x94, 0x22, 0x69,
  0xE0, 0x16, 0x4E, 0x72, 0x9C, 0xBB, 0x44, 0x6E,
  0x33, 0x0B, 0x40, 0x49, 0xC2, 0x93, 0x96, 0xC2,
  0xA1, 0x17, 0x7F, 0x78, 0xF3, 0xC9, 0x54, 0x89,
  0xC5, 0x2D, 0x18, 0x07, 0x45, 0xBC, 0x09, 0x85,
  0xC3, 0xB4, 0x17, 0xCB, 0x2C, 0x24, 0xC1, 0xE8,
  0xFA, 0x4D, 0x59, 0x95, 0x7B, 0x62, 0x7D, 0x9A,
  0x53, 0xD2, 0x41, 0x8F, 0x2F, 0x0A, 0x8F, 0x36,
  0x13, 0xDB, 0xFC, 0xAA, 0x8F, 0x09, 0x01, 0x04,
  0x3B, 0x2E, 0x92, 0xDD, 0xF8, 0x56, 0x3B, 0xD4,
  0x3D, 0xCC, 0xEA, 0xAB, 0xEF, 0x6C, 0x26, 0xAA,
  0x4E, 0x1C, 0x6C, 0xBD, 0x9E, 0x0E, 0x70, 0xC2,
  0xF7, 0x45, 0x41, 0x35, 0xE2, 0x70, 0x34, 0x08,
  0xD7, 0x01, 0x27, 0x75, 0xC9, 0x09, 0xE2, 0x62,
  0x8C, 0x03, 0x04, 0xFB, 0x4A, 0xDE, 0xAA, 0x11
};

//ECB vectors 
UINT8_A triple_des_key_1_ecb[] = 
{
  0x21, 0x28, 0xCA, 0x88, 0xFC, 0xB8, 0x3C, 0xB7,
  0x22, 0xF3, 0xDB, 0x35, 0x74, 0xA9, 0x5C, 0xC5,
  0x50, 0x99, 0x48, 0x99, 0xE8, 0x21, 0xD4, 0x8B
};

UINT8_A triple_des_plain_text_1_ecb[] = 
{
  0x69, 0xA7, 0xEF, 0x44, 0x63, 0x07, 0x1C, 0x13,
  0xA4, 0x47, 0x70, 0x82, 0xBE, 0x36, 0x3B, 0x65,
  0x52, 0x9F, 0xD5, 0xFD, 0x46, 0xE8, 0x2B, 0xE0,
  0x0A, 0xA6, 0xC9, 0x79, 0x19, 0x8A, 0x62, 0x53,
  0x92, 0x11, 0x66, 0x4D, 0x10, 0x21, 0xB4, 0xEB,
  0xDB, 0xCD, 0x6F, 0x15, 0xA5, 0x91, 0x27, 0x2B,
  0x96, 0xD7, 0xC8, 0xE0, 0x00, 0xD7, 0xA3, 0xFB,
  0x47, 0x49, 0xE8, 0xE2, 0x74, 0x33, 0x92, 0xD8,
  0x43, 0xDC, 0x1A, 0xEB, 0x82, 0x72, 0x4A, 0x15,
  0x7D, 0xFF, 0xCC, 0x71, 0x87, 0x51, 0x5E, 0x5C,
  0x86, 0x12, 0xE3, 0x13, 0x8F, 0x53, 0x84, 0xD9,
  0x84, 0x41, 0x5E, 0xCA, 0xFA, 0xB8, 0x16, 0x7C,
  0x40, 0xCD, 0x76, 0xAA, 0xFD, 0x12, 0x0E, 0xC4,
  0x93, 0x13, 0xBC, 0xD3, 0xE9, 0xA4, 0xB1, 0xE9,
  0x1C, 0xB7, 0xB5, 0xF5, 0x38, 0x84, 0x99, 0x75,
  0xD9, 0xA5, 0x95, 0x4E, 0xC4, 0xC5, 0xB8, 0xA6
};

UINT8_A triple_des_cipher_text_1_ecb[] = 
{
  0xDA, 0x72, 0x1D, 0x0F, 0x23, 0x6F, 0xDD, 0x0B,
  0x3A, 0xD0, 0x0C, 0xC7, 0x09, 0x02, 0xAE, 0x3F,
  0x37, 0xDE, 0x22, 0xBB, 0x5C, 0x89, 0x49, 0x85,
  0x1C, 0xB0, 0x6D, 0x80, 0x78, 0xC6, 0x56, 0xFC,
  0x31, 0x9D, 0xDF, 0xBB, 0xD9, 0xBA, 0x65, 0x99,
  0x47, 0xCB, 0xA7, 0x61, 0x9A, 0xE5, 0x5B, 0xAD,
  0x01, 0xAD, 0xA7, 0x07, 0xA3, 0x8F, 0xE8, 0xE3,
  0xA4, 0xE0, 0x0B, 0x97, 0xE8, 0xBB, 0x0A, 0x46,
  0x7C, 0xD5, 0x16, 0x17, 0xAD, 0x6E, 0x4E, 0x46,
  0x37, 0xA2, 0x08, 0xAF, 0xBF, 0xF1, 0x64, 0xB7,
  0x17, 0xC6, 0xDA, 0xA4, 0x1F, 0x29, 0xC2, 0xB8,
  0x24, 0xAE, 0x4B, 0xA7, 0x5F, 0x06, 0x10, 0x3C,
  0x85, 0x98, 0xE9, 0xF1, 0xEA, 0xF7, 0xF2, 0x17,
  0xA7, 0x2D, 0x80, 0xFF, 0xF3, 0xF7, 0x11, 0x8D,
  0x0D, 0xF8, 0xAD, 0x2D, 0x0E, 0x69, 0x4B, 0x2D,
  0x57, 0x5E, 0xD4, 0x14, 0x2C, 0xBF, 0xA6, 0x34
};

UINT8_A triple_des_key_2_ecb[] =
{  
  0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
  0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01,
  0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01, 0x23
};

UINT8_A triple_des_plain_text_2_ecb[] =
{
  0x54, 0x68, 0x65, 0x20, 0x71, 0x75, 0x66, 0x63,
  0x6B, 0x20, 0x62, 0x72, 0x6F, 0x77, 0x6E, 0x20,
  0x66, 0x6F, 0x78, 0x20, 0x6A, 0x75, 0x6D, 0x70
};

UINT8_A triple_des_cipher_text_2_ecb[] =
{ 
  0xA8, 0x26, 0xFD, 0x8C, 0xE5, 0x3B, 0x85, 0x5F,
  0xCC, 0xE2, 0x1C, 0x81, 0x12, 0x25, 0x6F, 0xE6,
  0x68, 0xD5, 0xC0, 0x5D, 0xD9, 0xB6, 0xB9, 0x00
};  

UINT8_A triple_des_key_3_ecb[] = 
{
  0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 
  0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01, 
  0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01, 0x23,
};

UINT8_A triple_des_plain_text_3_ecb[] = 
{
  0x54, 0x68, 0x65, 0x20, 0x71, 0x75, 0x66, 0x63, 
};

UINT8_A triple_des_cipher_text_3_ecb[] = 
{
  0xa8, 0x26, 0xfd, 0x8c, 0xe5, 0x3b, 0x85, 0x5f,   
};  

typedef struct
{
  QSEE_CIPHER_ALGO_ET  alg;
  QSEE_CIPHER_MODE_ET  mode;
  uint8*               pt;
  uint32               pt_len;
  uint8*               key;
  uint8*               iv;
  uint8*               ct;
} __attribute__ ((packed)) triple_des_vector_t;

typedef struct
{
  QSEE_CIPHER_ALGO_ET  alg;
  QSEE_CIPHER_MODE_ET  mode;
  uint8*               pt;
  uint32               pt_len;
  uint8*               key;
  uint8*               iv;
  uint8*               ct;
  uint8*               ct2;
} __attribute__ ((packed)) triple_des_vector_t_2;

triple_des_vector_t triple_des_test_vectors[] = 
{
  {QSEE_CIPHER_ALGO_TRIPLE_DES, QSEE_CIPHER_MODE_CBC, triple_des_plain_text_1_cbc, sizeof(triple_des_plain_text_1_cbc), 
   triple_des_key_1_cbc, triple_des_iv_1_cbc, triple_des_cipher_text_1_cbc},
  {QSEE_CIPHER_ALGO_TRIPLE_DES, QSEE_CIPHER_MODE_CBC, triple_des_plain_text_2_cbc, sizeof(triple_des_plain_text_2_cbc), 
   triple_des_key_2_cbc, triple_des_iv_2_cbc, triple_des_cipher_text_2_cbc}, 
  {QSEE_CIPHER_ALGO_TRIPLE_DES, QSEE_CIPHER_MODE_ECB, triple_des_plain_text_1_ecb, sizeof(triple_des_plain_text_1_ecb), 
   triple_des_key_1_ecb, NULL, triple_des_cipher_text_1_ecb},
  {QSEE_CIPHER_ALGO_TRIPLE_DES, QSEE_CIPHER_MODE_ECB, triple_des_plain_text_2_ecb, sizeof(triple_des_plain_text_2_ecb), 
   triple_des_key_2_ecb, NULL, triple_des_cipher_text_2_ecb},
  {QSEE_CIPHER_ALGO_TRIPLE_DES, QSEE_CIPHER_MODE_ECB, triple_des_plain_text_3_ecb, sizeof(triple_des_plain_text_3_ecb), 
   triple_des_key_3_ecb, NULL, triple_des_cipher_text_3_ecb},
};

triple_des_vector_t_2 triple_des_test_vectors_2 = 
{
  QSEE_CIPHER_ALGO_TRIPLE_DES, QSEE_CIPHER_MODE_CBC, triple_des_plain_text_1_cbc, sizeof(triple_des_plain_text_1_cbc), 
  triple_des_key_1_cbc, triple_des_iv_1_cbc, triple_des_cipher_text_1_cbc, triple_des_cipher_text_1_cbc
};

/* AES-XTS-128 test vectors */
//Vector 1 in "IEEE Std 1619-2007"
UINT8_A aesxts128_plain_text_1[] = 
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

UINT8_A aesxts128_key1_1[] = 
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

UINT8_A aesxts128_key2_1[] = 
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

UINT8_A aesxts128_iv_1[] = 
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

UINT8_A aesxts128_cipher_text_1[] = 
{
  0x91, 0x7c, 0xf6, 0x9e, 0xbd, 0x68, 0xb2, 0xec,
  0x9b, 0x9f, 0xe9, 0xa3, 0xea, 0xdd, 0xa6, 0x92,
  0xcd, 0x43, 0xd2, 0xf5, 0x95, 0x98, 0xed, 0x85,
  0x8c, 0x02, 0xc2, 0x65, 0x2f, 0xbf, 0x92, 0x2e
};

//Vector 2 in "IEEE Std 1619-2007"
UINT8_A aesxts128_plain_text_2[] = 
{
  0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
  0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
  0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
  0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44
};

UINT8_A aesxts128_key1_2[] = 
{
  0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11
};

UINT8_A aesxts128_key2_2[] = 
{
  0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
  0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22
};

UINT8_A aesxts128_iv_2[] = 
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x33
};

UINT8_A aesxts128_cipher_text_2[] = 
{
  0xc4, 0x54, 0x18, 0x5e, 0x6a, 0x16, 0x93, 0x6e,
  0x39, 0x33, 0x40, 0x38, 0xac, 0xef, 0x83, 0x8b,
  0xfb, 0x18, 0x6f, 0xff, 0x74, 0x80, 0xad, 0xc4,
  0x28, 0x93, 0x82, 0xec, 0xd6, 0xd3, 0x94, 0xf0
};

//Vector 5 in "IEEE Std 1619-2007"
UINT8_A aesxts128_plain_text_3[] = 
{
  0x27, 0xa7, 0x47, 0x9b, 0xef, 0xa1, 0xd4, 0x76, 0x48, 0x9f, 0x30, 0x8c, 0xd4, 0xcf, 0xa6, 0xe2,
  0xa9, 0x6e, 0x4b, 0xbe, 0x32, 0x08, 0xff, 0x25, 0x28, 0x7d, 0xd3, 0x81, 0x96, 0x16, 0xe8, 0x9c,
  0xc7, 0x8c, 0xf7, 0xf5, 0xe5, 0x43, 0x44, 0x5f, 0x83, 0x33, 0xd8, 0xfa, 0x7f, 0x56, 0x00, 0x00,
  0x05, 0x27, 0x9f, 0xa5, 0xd8, 0xb5, 0xe4, 0xad, 0x40, 0xe7, 0x36, 0xdd, 0xb4, 0xd3, 0x54, 0x12,
  0x32, 0x80, 0x63, 0xfd, 0x2a, 0xab, 0x53, 0xe5, 0xea, 0x1e, 0x0a, 0x9f, 0x33, 0x25, 0x00, 0xa5,
  0xdf, 0x94, 0x87, 0xd0, 0x7a, 0x5c, 0x92, 0xcc, 0x51, 0x2c, 0x88, 0x66, 0xc7, 0xe8, 0x60, 0xce,
  0x93, 0xfd, 0xf1, 0x66, 0xa2, 0x49, 0x12, 0xb4, 0x22, 0x97, 0x61, 0x46, 0xae, 0x20, 0xce, 0x84,
  0x6b, 0xb7, 0xdc, 0x9b, 0xa9, 0x4a, 0x76, 0x7a, 0xae, 0xf2, 0x0c, 0x0d, 0x61, 0xad, 0x02, 0x65,
  0x5e, 0xa9, 0x2d, 0xc4, 0xc4, 0xe4, 0x1a, 0x89, 0x52, 0xc6, 0x51, 0xd3, 0x31, 0x74, 0xbe, 0x51,
  0xa1, 0x0c, 0x42, 0x11, 0x10, 0xe6, 0xd8, 0x15, 0x88, 0xed, 0xe8, 0x21, 0x03, 0xa2, 0x52, 0xd8,
  0xa7, 0x50, 0xe8, 0x76, 0x8d, 0xef, 0xff, 0xed, 0x91, 0x22, 0x81, 0x0a, 0xae, 0xb9, 0x9f, 0x91,
  0x72, 0xaf, 0x82, 0xb6, 0x04, 0xdc, 0x4b, 0x8e, 0x51, 0xbc, 0xb0, 0x82, 0x35, 0xa6, 0xf4, 0x34,
  0x13, 0x32, 0xe4, 0xca, 0x60, 0x48, 0x2a, 0x4b, 0xa1, 0xa0, 0x3b, 0x3e, 0x65, 0x00, 0x8f, 0xc5,
  0xda, 0x76, 0xb7, 0x0b, 0xf1, 0x69, 0x0d, 0xb4, 0xea, 0xe2, 0x9c, 0x5f, 0x1b, 0xad, 0xd0, 0x3c,
  0x5c, 0xcf, 0x2a, 0x55, 0xd7, 0x05, 0xdd, 0xcd, 0x86, 0xd4, 0x49, 0x51, 0x1c, 0xeb, 0x7e, 0xc3,
  0x0b, 0xf1, 0x2b, 0x1f, 0xa3, 0x5b, 0x91, 0x3f, 0x9f, 0x74, 0x7a, 0x8a, 0xfd, 0x1b, 0x13, 0x0e,
  0x94, 0xbf, 0xf9, 0x4e, 0xff, 0xd0, 0x1a, 0x91, 0x73, 0x5c, 0xa1, 0x72, 0x6a, 0xcd, 0x0b, 0x19,
  0x7c, 0x4e, 0x5b, 0x03, 0x39, 0x36, 0x97, 0xe1, 0x26, 0x82, 0x6f, 0xb6, 0xbb, 0xde, 0x8e, 0xcc,
  0x1e, 0x08, 0x29, 0x85, 0x16, 0xe2, 0xc9, 0xed, 0x03, 0xff, 0x3c, 0x1b, 0x78, 0x60, 0xf6, 0xde,
  0x76, 0xd4, 0xce, 0xcd, 0x94, 0xc8, 0x11, 0x98, 0x55, 0xef, 0x52, 0x97, 0xca, 0x67, 0xe9, 0xf3,
  0xe7, 0xff, 0x72, 0xb1, 0xe9, 0x97, 0x85, 0xca, 0x0a, 0x7e, 0x77, 0x20, 0xc5, 0xb3, 0x6d, 0xc6,
  0xd7, 0x2c, 0xac, 0x95, 0x74, 0xc8, 0xcb, 0xbc, 0x2f, 0x80, 0x1e, 0x23, 0xe5, 0x6f, 0xd3, 0x44,
  0xb0, 0x7f, 0x22, 0x15, 0x4b, 0xeb, 0xa0, 0xf0, 0x8c, 0xe8, 0x89, 0x1e, 0x64, 0x3e, 0xd9, 0x95,
  0xc9, 0x4d, 0x9a, 0x69, 0xc9, 0xf1, 0xb5, 0xf4, 0x99, 0x02, 0x7a, 0x78, 0x57, 0x2a, 0xee, 0xbd,
  0x74, 0xd2, 0x0c, 0xc3, 0x98, 0x81, 0xc2, 0x13, 0xee, 0x77, 0x0b, 0x10, 0x10, 0xe4, 0xbe, 0xa7,
  0x18, 0x84, 0x69, 0x77, 0xae, 0x11, 0x9f, 0x7a, 0x02, 0x3a, 0xb5, 0x8c, 0xca, 0x0a, 0xd7, 0x52,
  0xaf, 0xe6, 0x56, 0xbb, 0x3c, 0x17, 0x25, 0x6a, 0x9f, 0x6e, 0x9b, 0xf1, 0x9f, 0xdd, 0x5a, 0x38,
  0xfc, 0x82, 0xbb, 0xe8, 0x72, 0xc5, 0x53, 0x9e, 0xdb, 0x60, 0x9e, 0xf4, 0xf7, 0x9c, 0x20, 0x3e,
  0xbb, 0x14, 0x0f, 0x2e, 0x58, 0x3c, 0xb2, 0xad, 0x15, 0xb4, 0xaa, 0x5b, 0x65, 0x50, 0x16, 0xa8,
  0x44, 0x92, 0x77, 0xdb, 0xd4, 0x77, 0xef, 0x2c, 0x8d, 0x6c, 0x01, 0x7d, 0xb7, 0x38, 0xb1, 0x8d,
  0xeb, 0x4a, 0x42, 0x7d, 0x19, 0x23, 0xce, 0x3f, 0xf2, 0x62, 0x73, 0x57, 0x79, 0xa4, 0x18, 0xf2,
  0x0a, 0x28, 0x2d, 0xf9, 0x20, 0x14, 0x7b, 0xea, 0xbe, 0x42, 0x1e, 0xe5, 0x31, 0x9d, 0x05, 0x68
};

UINT8_A aesxts128_key1_3[] = 
{
  0x27, 0x18, 0x28, 0x18, 0x28, 0x45, 0x90, 0x45,
  0x23, 0x53, 0x60, 0x28, 0x74, 0x71, 0x35, 0x26
};

UINT8_A aesxts128_key2_3[] = 
{
  0x31, 0x41, 0x59, 0x26, 0x53, 0x58, 0x97, 0x93,
  0x23, 0x84, 0x62, 0x64, 0x33, 0x83, 0x27, 0x95
};

UINT8_A aesxts128_iv_3[] = 
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
};

UINT8_A aesxts128_cipher_text_3[] = 
{
  0x26, 0x4d, 0x3c, 0xa8, 0x51, 0x21, 0x94, 0xfe, 0xc3, 0x12, 0xc8, 0xc9, 0x89, 0x1f, 0x27, 0x9f,
  0xef, 0xdd, 0x60, 0x8d, 0x0c, 0x02, 0x7b, 0x60, 0x48, 0x3a, 0x3f, 0xa8, 0x11, 0xd6, 0x5e, 0xe5,
  0x9d, 0x52, 0xd9, 0xe4, 0x0e, 0xc5, 0x67, 0x2d, 0x81, 0x53, 0x2b, 0x38, 0xb6, 0xb0, 0x89, 0xce,
  0x95, 0x1f, 0x0f, 0x9c, 0x35, 0x59, 0x0b, 0x8b, 0x97, 0x8d, 0x17, 0x52, 0x13, 0xf3, 0x29, 0xbb,
  0x1c, 0x2f, 0xd3, 0x0f, 0x2f, 0x7f, 0x30, 0x49, 0x2a, 0x61, 0xa5, 0x32, 0xa7, 0x9f, 0x51, 0xd3,
  0x6f, 0x5e, 0x31, 0xa7, 0xc9, 0xa1, 0x2c, 0x28, 0x60, 0x82, 0xff, 0x7d, 0x23, 0x94, 0xd1, 0x8f,
  0x78, 0x3e, 0x1a, 0x8e, 0x72, 0xc7, 0x22, 0xca, 0xaa, 0xa5, 0x2d, 0x8f, 0x06, 0x56, 0x57, 0xd2,
  0x63, 0x1f, 0xd2, 0x5b, 0xfd, 0x8e, 0x5b, 0xaa, 0xd6, 0xe5, 0x27, 0xd7, 0x63, 0x51, 0x75, 0x01,
  0xc6, 0x8c, 0x5e, 0xdc, 0x3c, 0xdd, 0x55, 0x43, 0x5c, 0x53, 0x2d, 0x71, 0x25, 0xc8, 0x61, 0x4d,
  0xee, 0xd9, 0xad, 0xaa, 0x3a, 0xca, 0xde, 0x58, 0x88, 0xb8, 0x7b, 0xef, 0x64, 0x1c, 0x4c, 0x99,
  0x4c, 0x80, 0x91, 0xb5, 0xbc, 0xd3, 0x87, 0xf3, 0x96, 0x3f, 0xb5, 0xbc, 0x37, 0xaa, 0x92, 0x2f,
  0xbf, 0xe3, 0xdf, 0x4e, 0x5b, 0x91, 0x5e, 0x6e, 0xb5, 0x14, 0x71, 0x7b, 0xdd, 0x2a, 0x74, 0x07,
  0x9a, 0x50, 0x73, 0xf5, 0xc4, 0xbf, 0xd4, 0x6a, 0xdf, 0x7d, 0x28, 0x2e, 0x7a, 0x39, 0x3a, 0x52,
  0x57, 0x9d, 0x11, 0xa0, 0x28, 0xda, 0x4d, 0x9c, 0xd9, 0xc7, 0x71, 0x24, 0xf9, 0x64, 0x8e, 0xe3,
  0x83, 0xb1, 0xac, 0x76, 0x39, 0x30, 0xe7, 0x16, 0x2a, 0x8d, 0x37, 0xf3, 0x50, 0xb2, 0xf7, 0x4b,
  0x84, 0x72, 0xcf, 0x09, 0x90, 0x20, 0x63, 0xc6, 0xb3, 0x2e, 0x8c, 0x2d, 0x92, 0x90, 0xce, 0xfb,
  0xd7, 0x34, 0x6d, 0x1c, 0x77, 0x9a, 0x0d, 0xf5, 0x0e, 0xdc, 0xde, 0x45, 0x31, 0xda, 0x07, 0xb0,
  0x99, 0xc6, 0x38, 0xe8, 0x3a, 0x75, 0x59, 0x44, 0xdf, 0x2a, 0xef, 0x1a, 0xa3, 0x17, 0x52, 0xfd,
  0x32, 0x3d, 0xcb, 0x71, 0x0f, 0xb4, 0xbf, 0xbb, 0x9d, 0x22, 0xb9, 0x25, 0xbc, 0x35, 0x77, 0xe1,
  0xb8, 0x94, 0x9e, 0x72, 0x9a, 0x90, 0xbb, 0xaf, 0xea, 0xcf, 0x7f, 0x78, 0x79, 0xe7, 0xb1, 0x14,
  0x7e, 0x28, 0xba, 0x0b, 0xae, 0x94, 0x0d, 0xb7, 0x95, 0xa6, 0x1b, 0x15, 0xec, 0xf4, 0xdf, 0x8d,
  0xb0, 0x7b, 0x82, 0x4b, 0xb0, 0x62, 0x80, 0x2c, 0xc9, 0x8a, 0x95, 0x45, 0xbb, 0x2a, 0xae, 0xed,
  0x77, 0xcb, 0x3f, 0xc6, 0xdb, 0x15, 0xdc, 0xd7, 0xd8, 0x0d, 0x7d, 0x5b, 0xc4, 0x06, 0xc4, 0x97,
  0x0a, 0x34, 0x78, 0xad, 0xa8, 0x89, 0x9b, 0x32, 0x91, 0x98, 0xeb, 0x61, 0xc1, 0x93, 0xfb, 0x62,
  0x75, 0xaa, 0x8c, 0xa3, 0x40, 0x34, 0x4a, 0x75, 0xa8, 0x62, 0xae, 0xbe, 0x92, 0xee, 0xe1, 0xce,
  0x03, 0x2f, 0xd9, 0x50, 0xb4, 0x7d, 0x77, 0x04, 0xa3, 0x87, 0x69, 0x23, 0xb4, 0xad, 0x62, 0x84,
  0x4b, 0xf4, 0xa0, 0x9c, 0x4d, 0xbe, 0x8b, 0x43, 0x97, 0x18, 0x4b, 0x74, 0x71, 0x36, 0x0c, 0x95,
  0x64, 0x88, 0x0a, 0xed, 0xdd, 0xb9, 0xba, 0xa4, 0xaf, 0x2e, 0x75, 0x39, 0x4b, 0x08, 0xcd, 0x32,
  0xff, 0x47, 0x9c, 0x57, 0xa0, 0x7d, 0x3e, 0xab, 0x5d, 0x54, 0xde, 0x5f, 0x97, 0x38, 0xb8, 0xd2,
  0x7f, 0x27, 0xa9, 0xf0, 0xab, 0x11, 0x79, 0x9d, 0x7b, 0x7f, 0xfe, 0xfb, 0x27, 0x04, 0xc9, 0x5c,
  0x6a, 0xd1, 0x2c, 0x39, 0xf1, 0xe8, 0x67, 0xa4, 0xb7, 0xb1, 0xd7, 0x81, 0x8a, 0x4b, 0x75, 0x3d,
  0xfd, 0x2a, 0x89, 0xcc, 0xb4, 0x5e, 0x00, 0x1a, 0x03, 0xa8, 0x67, 0xb1, 0x87, 0xf2, 0x25, 0xdd
};

//Vector 11 in "IEEE Std 1619-2007"
UINT8_A aesxts256_plain_text_4[] = 
{
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
  0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
  0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
  0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
  0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
  0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
  0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
  0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
  0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
  0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
  0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
  0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
  0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
  0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
  0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
};

UINT8_A aesxts256_key1_4[] = 
{
  0x27, 0x18, 0x28, 0x18, 0x28, 0x45, 0x90, 0x45,
  0x23, 0x53, 0x60, 0x28, 0x74, 0x71, 0x35, 0x26,
  0x62, 0x49, 0x77, 0x57, 0x24, 0x70, 0x93, 0x69,
  0x99, 0x59, 0x57, 0x49, 0x66, 0x96, 0x76, 0x27
};

UINT8_A aesxts256_key2_4[] = 
{
  0x31, 0x41, 0x59, 0x26, 0x53, 0x58, 0x97, 0x93,
  0x23, 0x84, 0x62, 0x64, 0x33, 0x83, 0x27, 0x95,
  0x02, 0x88, 0x41, 0x97, 0x16, 0x93, 0x99, 0x37,
  0x51, 0x05, 0x82, 0x09, 0x74, 0x94, 0x45, 0x92
};

UINT8_A aesxts256_iv_4[] = 
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff
};

UINT8_A aesxts256_cipher_text_4[] = 
{
  0x77, 0xa3, 0x12, 0x51, 0x61, 0x8a, 0x15, 0xe6, 0xb9, 0x2d, 0x1d, 0x66, 0xdf, 0xfe, 0x7b, 0x50,
  0xb5, 0x0b, 0xad, 0x55, 0x23, 0x05, 0xba, 0x02, 0x17, 0xa6, 0x10, 0x68, 0x8e, 0xff, 0x7e, 0x11,
  0xe1, 0xd0, 0x22, 0x54, 0x38, 0xe0, 0x93, 0x24, 0x2d, 0x6d, 0xb2, 0x74, 0xfd, 0xe8, 0x01, 0xd4,
  0xca, 0xe0, 0x6f, 0x20, 0x92, 0xc7, 0x28, 0xb2, 0x47, 0x85, 0x59, 0xdf, 0x58, 0xe8, 0x37, 0xc2,
  0x46, 0x9e, 0xe4, 0xa4, 0xfa, 0x79, 0x4e, 0x4b, 0xbc, 0x7f, 0x39, 0xbc, 0x02, 0x6e, 0x3c, 0xb7,
  0x2c, 0x33, 0xb0, 0x88, 0x8f, 0x25, 0xb4, 0xac, 0xf5, 0x6a, 0x2a, 0x98, 0x04, 0xf1, 0xce, 0x6d,
  0x3d, 0x6e, 0x1d, 0xc6, 0xca, 0x18, 0x1d, 0x4b, 0x54, 0x61, 0x79, 0xd5, 0x55, 0x44, 0xaa, 0x77,
  0x60, 0xc4, 0x0d, 0x06, 0x74, 0x15, 0x39, 0xc7, 0xe3, 0xcd, 0x9d, 0x2f, 0x66, 0x50, 0xb2, 0x01,
  0x3f, 0xd0, 0xee, 0xb8, 0xc2, 0xb8, 0xe3, 0xd8, 0xd2, 0x40, 0xcc, 0xae, 0x2d, 0x4c, 0x98, 0x32,
  0x0a, 0x74, 0x42, 0xe1, 0xc8, 0xd7, 0x5a, 0x42, 0xd6, 0xe6, 0xcf, 0xa4, 0xc2, 0xec, 0xa1, 0x79,
  0x8d, 0x15, 0x8c, 0x7a, 0xec, 0xdf, 0x82, 0x49, 0x0f, 0x24, 0xbb, 0x9b, 0x38, 0xe1, 0x08, 0xbc,
  0xda, 0x12, 0xc3, 0xfa, 0xf9, 0xa2, 0x11, 0x41, 0xc3, 0x61, 0x3b, 0x58, 0x36, 0x7f, 0x92, 0x2a,
  0xaa, 0x26, 0xcd, 0x22, 0xf2, 0x3d, 0x70, 0x8d, 0xae, 0x69, 0x9a, 0xd7, 0xcb, 0x40, 0xa8, 0xad,
  0x0b, 0x6e, 0x27, 0x84, 0x97, 0x3d, 0xcb, 0x60, 0x56, 0x84, 0xc0, 0x8b, 0x8d, 0x69, 0x98, 0xc6,
  0x9a, 0xac, 0x04, 0x99, 0x21, 0x87, 0x1e, 0xbb, 0x65, 0x30, 0x1a, 0x46, 0x19, 0xca, 0x80, 0xec,
  0xb4, 0x85, 0xa3, 0x1d, 0x74, 0x42, 0x23, 0xce, 0x8d, 0xdc, 0x23, 0x94, 0x82, 0x8d, 0x6a, 0x80,
  0x47, 0x0c, 0x09, 0x2f, 0x5b, 0xa4, 0x13, 0xc3, 0x37, 0x8f, 0xa6, 0x05, 0x42, 0x55, 0xc6, 0xf9,
  0xdf, 0x44, 0x95, 0x86, 0x2b, 0xbb, 0x32, 0x87, 0x68, 0x1f, 0x93, 0x1b, 0x68, 0x7c, 0x88, 0x8a,
  0xbf, 0x84, 0x4d, 0xfc, 0x8f, 0xc2, 0x83, 0x31, 0xe5, 0x79, 0x92, 0x8c, 0xd1, 0x2b, 0xd2, 0x39,
  0x0a, 0xe1, 0x23, 0xcf, 0x03, 0x81, 0x8d, 0x14, 0xde, 0xdd, 0xe5, 0xc0, 0xc2, 0x4c, 0x8a, 0xb0,
  0x18, 0xbf, 0xca, 0x75, 0xca, 0x09, 0x6f, 0x2d, 0x53, 0x1f, 0x3d, 0x16, 0x19, 0xe7, 0x85, 0xf1,
  0xad, 0xa4, 0x37, 0xca, 0xb9, 0x2e, 0x98, 0x05, 0x58, 0xb3, 0xdc, 0xe1, 0x47, 0x4a, 0xfb, 0x75,
  0xbf, 0xed, 0xbf, 0x8f, 0xf5, 0x4c, 0xb2, 0x61, 0x8e, 0x02, 0x44, 0xc9, 0xac, 0x0d, 0x3c, 0x66,
  0xfb, 0x51, 0x59, 0x8c, 0xd2, 0xdb, 0x11, 0xf9, 0xbe, 0x39, 0x79, 0x1a, 0xbe, 0x44, 0x7c, 0x63,
  0x09, 0x4f, 0x7c, 0x45, 0x3b, 0x7f, 0xf8, 0x7c, 0xb5, 0xbb, 0x36, 0xb7, 0xc7, 0x9e, 0xfb, 0x08,
  0x72, 0xd1, 0x70, 0x58, 0xb8, 0x3b, 0x15, 0xab, 0x08, 0x66, 0xad, 0x8a, 0x58, 0x65, 0x6c, 0x5a,
  0x7e, 0x20, 0xdb, 0xdf, 0x30, 0x8b, 0x24, 0x61, 0xd9, 0x7c, 0x0e, 0xc0, 0x02, 0x4a, 0x27, 0x15,
  0x05, 0x52, 0x49, 0xcf, 0x3b, 0x47, 0x8d, 0xdd, 0x47, 0x40, 0xde, 0x65, 0x4f, 0x75, 0xca, 0x68,
  0x6e, 0x0d, 0x73, 0x45, 0xc6, 0x9e, 0xd5, 0x0c, 0xdc, 0x2a, 0x8b, 0x33, 0x2b, 0x1f, 0x88, 0x24,
  0x10, 0x8a, 0xc9, 0x37, 0xeb, 0x05, 0x05, 0x85, 0x60, 0x8e, 0xe7, 0x34, 0x09, 0x7f, 0xc0, 0x90,
  0x54, 0xfb, 0xff, 0x89, 0xee, 0xae, 0xea, 0x79, 0x1f, 0x4a, 0x7a, 0xb1, 0xf9, 0x86, 0x82, 0x94,
  0xa4, 0xf9, 0xe2, 0x7b, 0x42, 0xaf, 0x81, 0x00, 0xcb, 0x9d, 0x59, 0xce, 0xf9, 0x64, 0x58, 0x03
};

/* AES-XTS test vectors */
typedef struct
{
  QSEE_CIPHER_ALGO_ET  alg;
  QSEE_CIPHER_MODE_ET  mode;
  uint8 *pt;
  uint32 pt_len;
  uint8 *key;
  uint32 key_len;
  uint8 *xts_key;
  uint32 xts_key_len;
  uint32 xts_du_size;
  uint8 *iv;
  uint8 *ct;
}__attribute__ ((packed)) aes_xts_vector_t;

aes_xts_vector_t aes_xts_test_vectors[] = 
{
  { QSEE_CIPHER_ALGO_AES_128, QSEE_CIPHER_MODE_XTS, aesxts128_plain_text_1, sizeof(aesxts128_plain_text_1),
    aesxts128_key1_1, sizeof(aesxts128_key1_1), aesxts128_key2_1, sizeof(aesxts128_key2_1),
    32, aesxts128_iv_1, aesxts128_cipher_text_1 },
  { QSEE_CIPHER_ALGO_AES_128, QSEE_CIPHER_MODE_XTS, aesxts128_plain_text_1, sizeof(aesxts128_plain_text_1),
    aesxts128_key1_1, sizeof(aesxts128_key1_1), aesxts128_key2_1, sizeof(aesxts128_key2_1),
    0, aesxts128_iv_1, aesxts128_cipher_text_1 },
  { QSEE_CIPHER_ALGO_AES_128, QSEE_CIPHER_MODE_XTS, aesxts128_plain_text_2, sizeof(aesxts128_plain_text_2),
    aesxts128_key1_2, sizeof(aesxts128_key1_2), aesxts128_key2_2, sizeof(aesxts128_key2_2),
    32, aesxts128_iv_2, aesxts128_cipher_text_2 },
  { QSEE_CIPHER_ALGO_AES_128, QSEE_CIPHER_MODE_XTS, aesxts128_plain_text_2, sizeof(aesxts128_plain_text_2),
    aesxts128_key1_2, sizeof(aesxts128_key1_2), aesxts128_key2_2, sizeof(aesxts128_key2_2),
    0, aesxts128_iv_2, aesxts128_cipher_text_2 },
  { QSEE_CIPHER_ALGO_AES_128, QSEE_CIPHER_MODE_XTS, aesxts128_plain_text_3, sizeof(aesxts128_plain_text_3),
    aesxts128_key1_3, sizeof(aesxts128_key1_3), aesxts128_key2_3, sizeof(aesxts128_key2_3),
    512, aesxts128_iv_3, aesxts128_cipher_text_3 },
  { QSEE_CIPHER_ALGO_AES_128, QSEE_CIPHER_MODE_XTS, aesxts128_plain_text_3, sizeof(aesxts128_plain_text_3),
    aesxts128_key1_3, sizeof(aesxts128_key1_3), aesxts128_key2_3, sizeof(aesxts128_key2_3),
    0, aesxts128_iv_3, aesxts128_cipher_text_3 },
  { QSEE_CIPHER_ALGO_AES_256, QSEE_CIPHER_MODE_XTS,aesxts256_plain_text_4, sizeof(aesxts256_plain_text_4),
    aesxts256_key1_4, sizeof(aesxts256_key1_4), aesxts256_key2_4, sizeof(aesxts256_key2_4),
    512, aesxts256_iv_4, aesxts256_cipher_text_4 },
  { QSEE_CIPHER_ALGO_AES_256, QSEE_CIPHER_MODE_XTS, aesxts256_plain_text_4, sizeof(aesxts256_plain_text_4),
    aesxts256_key1_4, sizeof(aesxts256_key1_4), aesxts256_key2_4, sizeof(aesxts256_key2_4),
    0, aesxts256_iv_4, aesxts256_cipher_text_4 }
};

/* SHA 1 test vectors */
UINT8_A sha_plain_text_1[] = 
{
  "Test vector from febooti.com"
};

UINT8_A sha1_digest_text_1[] = 
{
  0xa7, 0x63, 0x17, 0x95, 0xf6, 0xd5, 0x9c, 0xd6,
  0xd1, 0x4e, 0xbd, 0x00, 0x58, 0xa6, 0x39, 0x4a,
  0x4b, 0x93, 0xd8, 0x68
};

/* SHA 256 test vectors */
UINT8_A sha256_digest_text_1[] = 
{
  0x07, 0x7b, 0x18, 0xfe, 0x29, 0x03, 0x6a, 0xda,
  0x48, 0x90, 0xbd, 0xec, 0x19, 0x21, 0x86, 0xe1,
  0x06, 0x78, 0x59, 0x7a, 0x67, 0x88, 0x02, 0x90,
  0x52, 0x1d, 0xf7, 0x0d, 0xf4, 0xba, 0xc9, 0xab
};

typedef struct sha_vector_type
{
  QSEE_HASH_ALGO_ET  alg;
  uint8*             msg;
  uint32             msg_len;
  uint8*             digest;
} __attribute__ ((packed)) sha_vector_type_t;

/* SHA test vectors array */
sha_vector_type_t sha_test_vectors[] =
{
  {QSEE_HASH_SHA1, sha_plain_text_1,
   sizeof(sha_plain_text_1) - 1, sha1_digest_text_1} ,
  {QSEE_HASH_SHA256, sha_plain_text_1,
   sizeof(sha_plain_text_1) - 1, sha256_digest_text_1} ,
};

typedef struct uf_sha_vector_type
{
  SW_Auth_Alg_Type   alg;
  uint8*             msg;
  uint32             msg_len;
  uint8*             digest;
} __attribute__ ((packed)) uf_sha_vector_type_t;

/* SHA test vectors array */
uf_sha_vector_type_t uf_sha_test_vectors[] =
{
  {SW_AUTH_ALG_SHA1, sha_plain_text_1,
   sizeof(sha_plain_text_1) - 1, sha1_digest_text_1} ,
  {SW_AUTH_ALG_SHA256, sha_plain_text_1,
   sizeof(sha_plain_text_1) - 1, sha256_digest_text_1} ,
};

/* SHA 256 HMAC test vectors */
UINT8_A sha_hmac_msg_1[2] = {0x5f, 0xd4};

UINT8_A sha_hmac_key_1[4] = 
{
  0x4a, 0x65, 0x66, 0x65
};

UINT8_A sha_hmac_msg_1_hmac1[20] = 
{
  0xc4, 0x8b, 0xbf, 0xb4, 0x82, 0x37, 0xe0, 0x8c,
  0xeb, 0x54, 0x7e, 0x6a, 0x91, 0x0d, 0x62, 0x54,
  0xd2, 0x5b, 0x23, 0x9d
};

UINT8_A sha_hmac_msg_1_hmac256[32] = 
{
  0xA0, 0xCC, 0x63, 0x97, 0x95, 0x25, 0x59, 0xB6,
  0x0E, 0x31, 0x83, 0x00, 0xD3, 0x94, 0x69, 0x85,
  0xCD, 0xD4, 0x5A, 0xA9, 0xCF, 0xF8, 0xAB, 0x0D,
  0xA2, 0xA5, 0x05, 0x71, 0x07, 0xC8, 0x08, 0x9C
};

UINT8_A sha_hmac_msg_2[8] = 
{
  0x48, 0x69, 0x20, 0x54, 0x68, 0x65, 0x72, 0x65
};

UINT8_A sha_hmac_key_2[20] = {
  0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
  0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
  0x0B, 0x0B, 0x0B, 0x0B
};

UINT8_A sha_hmac_msg_2_hmac1[20] = 
{
  0xb6, 0x17, 0x31, 0x86, 0x55, 0x05, 0x72, 0x64,
  0xe2, 0x8b, 0xc0, 0xb6, 0xfb, 0x37, 0x8c, 0x8e,
  0xf1, 0x46, 0xbe, 0x00
};

UINT8_A sha_hmac_msg_2_hmac256[32] = 
{
  0xB0, 0x34, 0x4C, 0x61, 0xD8, 0xDB, 0x38, 0x53,
  0x5C, 0xA8, 0xAF, 0xCE, 0xAF, 0x0B, 0xF1, 0x2B,
  0x88, 0x1D, 0xC2, 0x00, 0xC9, 0x83, 0x3D, 0xA7,
  0x26, 0xE9, 0x37, 0x6C, 0x2E, 0x32, 0xCF, 0xF7
};

UINT8_A sha_hmac_msg_3[28] = 
{
  0x77, 0x68, 0x61, 0x74, 0x20, 0x64, 0x6F, 0x20,
  0x79, 0x61, 0x20, 0x77, 0x61, 0x6E, 0x74, 0x20,
  0x66, 0x6F, 0x72, 0x20, 0x6E, 0x6F, 0x74, 0x68,
  0x69, 0x6E, 0x67, 0x3F
};

UINT8_A sha_hmac_key_3[4] = 
{
  0x4A, 0x65, 0x66, 0x65
};
UINT8_A sha_hmac_msg_3_hmac1[20] = 
{
  0xef, 0xfc, 0xdf, 0x6a, 0xe5, 0xeb, 0x2f, 0xa2,
  0xd2, 0x74, 0x16, 0xd5, 0xf1, 0x84, 0xdf, 0x9c,
  0x25, 0x9a, 0x7c, 0x79
};

UINT8_A sha_hmac_msg_3_hmac256[32] = 
{
  0x5B, 0xDC, 0xC1, 0x46, 0xBF, 0x60, 0x75, 0x4E,
  0x6A, 0x04, 0x24, 0x26, 0x08, 0x95, 0x75, 0xC7,
  0x5A, 0x00, 0x3F, 0x08, 0x9D, 0x27, 0x39, 0x83,
  0x9D, 0xEC, 0x58, 0xB9, 0x64, 0xEC, 0x38, 0x43
};

UINT8_A sha_hmac_msg_4[50] = 
{
  0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
  0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
  0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
  0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
  0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
  0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
  0xDD, 0xDD
};

UINT8_A sha_hmac_key_4[20] = 
{
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA
};

UINT8_A sha_hmac_msg_4_hmac1[20] = 
{
  0x12, 0x5d, 0x73, 0x42, 0xb9, 0xac, 0x11, 0xcd,
  0x91, 0xa3, 0x9a, 0xf4, 0x8a, 0xa1, 0x7b, 0x4f,
  0x63, 0xf1, 0x75, 0xd3
};

UINT8_A sha_hmac_msg_4_hmac256[32] = 
{
  0x77, 0x3E, 0xA9, 0x1E, 0x36, 0x80, 0x0E, 0x46,
  0x85, 0x4D, 0xB8, 0xEB, 0xD0, 0x91, 0x81, 0xA7,
  0x29, 0x59, 0x09, 0x8B, 0x3E, 0xF8, 0xC1, 0x22,
  0xD9, 0x63, 0x55, 0x14, 0xCE, 0xD5, 0x65, 0xFE
};

UINT8_A sha_hmac_msg_5[50] = 
{
  0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD,
  0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD,
  0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD,
  0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD,
  0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD,
  0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD,
  0xCD, 0xCD
};

UINT8_A sha_hmac_key_5[25] = 
{
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
  0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
  0x19
};

UINT8_A sha_hmac_msg_5_hmac1[20] = 
{
  0x4c, 0x90, 0x07, 0xf4, 0x02, 0x62, 0x50, 0xc6,
  0xbc, 0x84, 0x14, 0xf9, 0xbf, 0x50, 0xc8, 0x6c,
  0x2d, 0x72, 0x35, 0xda
};

UINT8_A sha_hmac_msg_5_hmac256[32] = 
{
  0x82, 0x55, 0x8A, 0x38, 0x9A, 0x44, 0x3C, 0x0E,
  0xA4, 0xCC, 0x81, 0x98, 0x99, 0xF2, 0x08, 0x3A,
  0x85, 0xF0, 0xFA, 0xA3, 0xE5, 0x78, 0xF8, 0x07,
  0x7A, 0x2E, 0x3F, 0xF4, 0x67, 0x29, 0x66, 0x5B
};

UINT8_A sha_hmac_msg_6[54] = 
{
  0x54, 0x65, 0x73, 0x74, 0x20, 0x55, 0x73, 0x69,
  0x6E, 0x67, 0x20, 0x4C, 0x61, 0x72, 0x67, 0x65,
  0x72, 0x20, 0x54, 0x68, 0x61, 0x6E, 0x20, 0x42,
  0x6C, 0x6F, 0x63, 0x6B, 0x2D, 0x53, 0x69, 0x7A,
  0x65, 0x20, 0x4B, 0x65, 0x79, 0x20, 0x2D, 0x20,
  0x48, 0x61, 0x73, 0x68, 0x20, 0x4B, 0x65, 0x79,
  0x20, 0x46, 0x69, 0x72, 0x73, 0x74
};

UINT8_A sha_hmac_key_6[131] = 
{
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA
};

UINT8_A sha_hmac_msg_6_hmac1[20] = 
{
  0x90, 0xd0, 0xda, 0xce, 0x1c, 0x1b, 0xdc, 0x95,
  0x73, 0x39, 0x30, 0x78, 0x03, 0x16, 0x03, 0x35,
  0xbd, 0xe6, 0xdf, 0x2b
};

UINT8_A sha_hmac_msg_6_hmac256[32] = 
{
  0x60, 0xE4, 0x31, 0x59, 0x1E, 0xE0, 0xB6, 0x7F,
  0x0D, 0x8A, 0x26, 0xAA, 0xCB, 0xF5, 0xB7, 0x7F,
  0x8E, 0x0B, 0xC6, 0x21, 0x37, 0x28, 0xC5, 0x14,
  0x05, 0x46, 0x04, 0x0F, 0x0E, 0xE3, 0x7F, 0x54
};

UINT8_A sha_hmac_msg_7[147] = 
{
  0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20,
  0x61, 0x20, 0x74, 0x65, 0x73, 0x74, 0x20, 0x75,
  0x73, 0x69, 0x6E, 0x67, 0x20, 0x61, 0x20, 0x6C,
  0x61, 0x72, 0x67, 0x65, 0x72, 0x20, 0x74, 0x68,
  0x61, 0x6E, 0x20, 0x62, 0x6C, 0x6F, 0x63, 0x6B,
  0x2D, 0x73, 0x69, 0x7A, 0x65, 0x20, 0x6B, 0x65,
  0x79, 0x20, 0x61, 0x6E, 0x64, 0x20, 0x61, 0x20,
  0x6C, 0x61, 0x72, 0x67, 0x65, 0x72, 0x20, 0x74,
  0x68, 0x61, 0x6E, 0x20, 0x62, 0x6C, 0x6F, 0x63,
  0x6B, 0x2D, 0x73, 0x69, 0x7A, 0x65, 0x20, 0x64,
  0x61, 0x74, 0x61, 0x2E, 0x20, 0x54, 0x68, 0x65,
  0x20, 0x6B, 0x65, 0x79, 0x20, 0x6E, 0x65, 0x65,
  0x64, 0x73, 0x20, 0x74, 0x6F, 0x20, 0x62, 0x65,
  0x20, 0x68, 0x61, 0x73, 0x68, 0x65, 0x64, 0x20,
  0x62, 0x65, 0x66, 0x6F, 0x72, 0x65, 0x20, 0x62,
  0x65, 0x69, 0x6E, 0x67, 0x20, 0x75, 0x73, 0x65,
  0x64, 0x20, 0x62, 0x79, 0x20, 0x48, 0x4D, 0x41,
  0x43, 0x20, 0x61, 0x6C, 0x67, 0x6F, 0x72, 0x69,
  0x74, 0x68, 0x6D
};

UINT8_A sha_hmac_key_7[131] = 
{
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
  0xAA, 0xAA, 0xAA
};

UINT8_A sha_hmac_msg_7_hmac1[20] = 
{
  0x34, 0xed, 0x59, 0x30, 0x63, 0x1c, 0x91, 0x48,
  0x1b, 0x7a, 0x0e, 0xbd, 0x47, 0x84, 0x58, 0x82,
  0xe2, 0x39, 0xce, 0xe9
}; 

UINT8_A sha_hmac_msg_7_hmac256[32] = 
{
  0x3C, 0xB1, 0x7A, 0xDD, 0x8A, 0x41, 0x81, 0x60,
  0xE5, 0x1C, 0x0D, 0x43, 0x9F, 0xE9, 0x8B, 0x2C,
  0xE5, 0x46, 0x4D, 0xEF, 0xB6, 0x80, 0x89, 0x71,
  0x09, 0xED, 0x70, 0x0B, 0x8D, 0xB6, 0x2C, 0xF9
};

typedef struct sha_hmac_vector_type
{
  uint8 *msg;
  uint32 msg_len;
  uint8 *hmac_key;
  uint32 hmac_key_len;
  uint8 *msg_hmac1;
  uint8 *msg_hmac256;
} __attribute__ ((packed)) sha_hmac_vector_type_t;

sha_hmac_vector_type_t sha_hmac_test_vectors[] = 
{
  {sha_hmac_msg_1, sizeof(sha_hmac_msg_1), sha_hmac_key_1,
   sizeof(sha_hmac_key_1), sha_hmac_msg_1_hmac1, sha_hmac_msg_1_hmac256}
  ,
  {sha_hmac_msg_2, sizeof(sha_hmac_msg_2), sha_hmac_key_2,
   sizeof(sha_hmac_key_2), sha_hmac_msg_2_hmac1, sha_hmac_msg_2_hmac256}
  ,
  {sha_hmac_msg_3, sizeof(sha_hmac_msg_3), sha_hmac_key_3,
   sizeof(sha_hmac_key_3), sha_hmac_msg_3_hmac1, sha_hmac_msg_3_hmac256}
  ,
  {sha_hmac_msg_4, sizeof(sha_hmac_msg_4), sha_hmac_key_4,
   sizeof(sha_hmac_key_4), sha_hmac_msg_4_hmac1, sha_hmac_msg_4_hmac256}
  ,
  {sha_hmac_msg_5, sizeof(sha_hmac_msg_5), sha_hmac_key_5,
   sizeof(sha_hmac_key_5), sha_hmac_msg_5_hmac1, sha_hmac_msg_5_hmac256}
  ,
  {sha_hmac_msg_6, sizeof(sha_hmac_msg_6), sha_hmac_key_6,
   sizeof(sha_hmac_key_6), sha_hmac_msg_6_hmac1, sha_hmac_msg_6_hmac256}
  ,
  {sha_hmac_msg_7, sizeof(sha_hmac_msg_7), sha_hmac_key_7,
   sizeof(sha_hmac_key_7), sha_hmac_msg_7_hmac1, sha_hmac_msg_7_hmac256}
};

int tz_app_ufcrypto_aes_gcm_func(uint32 *pt,
                                 uint32 pt_len,
                                 uint8 *key,
                                 uint32 key_len,
                                 uint8 *iv,
                                 uint32 iv_len,
                                 uint32 *ptr_tmp,
                                 uint32 *header_data,
                                 uint32 *auth_struct);
// CMAC vectors 
// AES 128 vectors
UINT8_A aescmac128_key_1[] = 
{
  0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
  0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};

UINT8_A aescmac128_key_2[] = 
{
  0x36, 0x5d, 0xf1, 0x49, 0x77, 0xf5, 0x56, 0xd6, 
  0xdd, 0xe6, 0x5f, 0x66, 0x70, 0xa3, 0x05, 0x18
};

// Mlen = 16
UINT8_A aescmac128_plain_text_1[] = 
{
  0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
  0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
};

// Mlen = 40
UINT8_A aescmac128_plain_text_2[] = 
{
  0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
  0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
  0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
  0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
  0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11
};

// Mlen = 64
uint8 aescmac128_plain_text_3[] = 
{
  0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
  0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
  0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
  0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
  0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
  0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
  0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
  0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10 
};

// Mlen = 32
uint8 aescmac128_plain_text_4[] = 
{
  0x1b, 0x4e, 0x4c, 0x7f, 0x91, 0xc6, 0x98, 0xbf, 
  0xa7, 0x47, 0x0f, 0xad, 0x1d, 0x60, 0x92, 0xfc, 
  0xef, 0xed, 0x1d, 0x02, 0x2a, 0x9a, 0x41, 0x5e, 
  0xff, 0xa2, 0xd4, 0xdd, 0x16, 0xdd, 0xe7, 0x9f
};

UINT8_A aescmac128_cmac_text_0[] = 
{
  0xbb, 0x1d, 0x69, 0x29, 0xe9, 0x59, 0x37, 0x28,
  0x7f, 0xa3, 0x7d, 0x12, 0x9b, 0x75, 0x67, 0x46
};

UINT8_A aescmac128_cmac_text_1[] = 
{
  0x07, 0x0a, 0x16, 0xb4, 0x6b, 0x4d, 0x41, 0x44,
  0xf7, 0x9b, 0xdd, 0x9d, 0xd0, 0x4a, 0x28, 0x7c
};

UINT8_A aescmac128_cmac_text_2[] = 
{
  0xdf, 0xa6, 0x67, 0x47, 0xde, 0x9a, 0xe6, 0x30,
  0x30, 0xca, 0x32, 0x61, 0x14, 0x97, 0xc8, 0x27
};

UINT8_A aescmac128_cmac_text_3[] = 
{
  0x51, 0xf0, 0xbe, 0xbf, 0x7e, 0x3b, 0x9d, 0x92,
  0xfc, 0x49, 0x74, 0x17, 0x79, 0x36, 0x3c, 0xfe
};   

UINT8_A aescmac128_cmac_text_4[] = 
{
  0x64, 0x4e, 0xc8, 0x02, 0x28, 0xc9, 0xc6, 0x43
};

// AES 256 vectors
UINT8_A aescmac256_key_1[] = 
{
  0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
  0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
  0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
  0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
}; 

UINT8_A aescmac256_cmac_text_1[] = 
{
  0x28, 0xa7, 0x02, 0x3f, 0x45, 0x2e, 0x8f, 0x82,
  0xbd, 0x4b, 0xf2, 0x8d, 0x8c, 0x37, 0xc3, 0x5c
};

UINT8_A aescmac256_cmac_text_2[] = 
{
  0xaa, 0xf3, 0xd8, 0xf1, 0xde, 0x56, 0x40, 0xc2,
  0x32, 0xf5, 0xb1, 0x69, 0xb9, 0xc9, 0x11, 0xe6
};

UINT8_A aescmac256_cmac_text_3[] = 
{
  0xe1, 0x99, 0x21, 0x90, 0x54, 0x9f, 0x6e, 0xd5,
  0x69, 0x6a, 0x2c, 0x05, 0x6c, 0x31, 0x54, 0x10
};

typedef struct
{
  uint8 *msg;
  uint32 msg_len;
  uint8 *key;
  uint32 key_len;
  uint8 *cmac;
  uint32 cmac_len;
} __attribute__ ((packed)) aes_cmac_vector_t;  

aes_cmac_vector_t aes_cmac_test_vectors[] = 
{
  {aescmac128_plain_text_1, sizeof(aescmac128_plain_text_1), aescmac128_key_1,   //AES-CMAC128
   16, aescmac128_cmac_text_1, sizeof(aescmac128_cmac_text_1)},
  {aescmac128_plain_text_2, sizeof(aescmac128_plain_text_2), aescmac128_key_1,   //AES-CMAC128
   16, aescmac128_cmac_text_2, sizeof(aescmac128_cmac_text_2)},
  {aescmac128_plain_text_3, sizeof(aescmac128_plain_text_3), aescmac128_key_1,   //AES-CMAC128
   16, aescmac128_cmac_text_3, sizeof(aescmac128_cmac_text_3)},
  {aescmac128_plain_text_4, sizeof(aescmac128_plain_text_4), aescmac128_key_2,   //AES-CMAC128
   16, aescmac128_cmac_text_4, sizeof(aescmac128_cmac_text_4)},
  {aescmac128_plain_text_1, sizeof(aescmac128_plain_text_1), aescmac256_key_1,   //AES-CMAC256
   32, aescmac256_cmac_text_1, sizeof(aescmac256_cmac_text_1)},
  {aescmac128_plain_text_2, sizeof(aescmac128_plain_text_2), aescmac256_key_1,   //AES-CMAC256
   32, aescmac256_cmac_text_2, sizeof(aescmac256_cmac_text_2)},
  {aescmac128_plain_text_3, sizeof(aescmac128_plain_text_3), aescmac256_key_1,   //AES-CMAC256
   32, aescmac256_cmac_text_3, sizeof(aescmac256_cmac_text_3)}
};

/**
  @brief 
    Sample code for how to call QSEE AES crypto API's
*/
int tz_app_crypto_aes_func(QSEE_CIPHER_ALGO_ET alg,
                           QSEE_CIPHER_MODE_ET mode,
                           uint8 *pt,
                           uint32 pt_len,
                           uint8 *key,
                           uint32 key_len,
                           uint8 *iv,
                           uint8 *ct,
                           uint8 verify)
{
  uint32 ct_len = pt_len;
  qsee_cipher_ctx *ctx = 0;
  uint8 *ct_tmp = 0;
  uint8 *pt_tmp = 0;
  int status = E_SUCCESS;
  QSEE_CIPHER_MODE_ET mode2;
  uint32 mode2_len = sizeof(mode2);

  /*------------------------------------------------------------------
    Init ctx
    ------------------------------------------------------------------*/
  if (qsee_cipher_init(alg, &ctx) < 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_init API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set key for encryption
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, key_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set AES mode
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set IV only if not NULL
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      NULL != iv &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_AES128_IV_SIZE) < 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*----------------------------------------------------------------------
  Allocate temp ct buffer
  ------------------------------------------------------------------------*/
  if (E_SUCCESS == status && (verify == 1) &&
      (ct_tmp = qsee_malloc(pt_len)) == NULL)
  {
    status = -E_FAILURE;
  }

  /*-----------------------------------------------------------------------
  Clear temp buffer
  ------------------------------------------------------------------------*/
  if ((E_SUCCESS == status) && (verify == 1))
  {
    memset(ct_tmp, 0, pt_len);
  }

  /*-----------------------------------------------------------------------
   If verify is 0,  it means we are not using the predefined test case and
   cannot verify the encrypted packet
  -------------------------------------------------------------------------*/
  if (verify == 0)
    ct_tmp = ct;

  /*-----------------------------------------------------------------------
  Now encrypt the data
  -------------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_encrypt(ctx, pt, pt_len, ct_tmp, &ct_len) < 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_encrypt API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------------
  If NULL key pointer then we are using HW key so don't compare encrypted result
  -----------------------------------------------------------------------------*/
  if ((E_SUCCESS == status) && (verify == 1) && (NULL != key) &&
      0 != memcmp(ct, ct_tmp, pt_len))
  {
    status = -E_FAILURE;
  }

  /*------------------------------------------------------------------------------
  Allocate temp pt buffer
  --------------------------------------------------------------------------------*/
  if ((E_SUCCESS == status) && (verify == 1) && 
      (pt_tmp = qsee_malloc(pt_len)) == NULL)
  {
    status = -E_FAILURE;
  }

  if ((E_SUCCESS == status)&& (verify == 1))
  {
    memset(pt_tmp, 0, pt_len);
  }

  /*-----------------------------------------------------------------------
   If verify is 0,  it means we are not using the predefined test case and
   cannot verify the encrypted packet
  -------------------------------------------------------------------------*/
  if (verify == 0)
    pt_tmp = pt;

  /*--------------------------------------------------------------------------------
  We must set parameters again so we can do the decrypt
  ---------------------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, key_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*----------------------------------------------------------------------------------
  Set IV if not NULL
  ------------------------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      NULL != iv &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_AES128_IV_SIZE) < 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*----------------------------------------------------------------------------------
  Now decrypt the data
  ------------------------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_decrypt(ctx, ct_tmp, pt_len, pt_tmp, &ct_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_decrypt API failed");
    status = -E_FAILURE;
  }

  /*-------------------------------------------------------------------------------------
  Now compare decrypted results
  ---------------------------------------------------------------------------------------*/
  if ((E_SUCCESS == status) && (verify == 1) &&
      0 != memcmp(pt, pt_tmp, pt_len))
  {  
    status = -E_FAILURE;
  }

  /*-------------------------------------------------------------------------------------
  Now read back Cipher Mode parameter
  ---------------------------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_get_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode2, &mode2_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_get_parm API failed");
    status = -E_FAILURE;
  }

  /*-------------------------------------------------------------------------------
  Free ctx
  --------------------------------------------------------------------------------*/
  if (ctx)
  {
    if (qsee_cipher_free_ctx(ctx) < 0)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_free_ctx API failed");
      status = -E_FAILURE;
    }
    ctx = 0;
  }

  /*--------------------------------------------------------------------------------
  Free malloc data
  ---------------------------------------------------------------------------------*/
  if ((ct_tmp) && (verify == 1))
  {
    qsee_free(ct_tmp);
    ct_tmp = 0;
  }

  if ((pt_tmp)&& (verify == 1))
  {
    qsee_free(pt_tmp);
    pt_tmp = 0;
  }

  return status;
}

/**
  @brief 
    Sample code for how to call QSEE Triple DES crypto API's
*/
int tz_app_crypto_triple_des_func(QSEE_CIPHER_ALGO_ET alg,
                                  QSEE_CIPHER_MODE_ET mode,
                                  uint8 *pt,
                                  uint32 pt_len,
                                  uint8 *key,
                                  uint8 *iv,       
                                  uint8 *ct,
                                  uint8 verify)
{
  uint32 ct_len = pt_len;
  qsee_cipher_ctx *ctx = 0;
  uint8 *ct_tmp = 0;
  uint8 *pt_tmp = 0;
  int status = E_SUCCESS;

  /*------------------------------------------------------------------
    Init ctx
    ------------------------------------------------------------------*/
  if (qsee_cipher_init(alg, &ctx) < 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_init API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set key for encryption
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, QSEE_TRIPLE_DES_KEY_SIZE) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set Cipher mode
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set IV only if not NULL
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      NULL != iv &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_TRIPLE_DES_IV_SIZE) < 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }
  
  /*----------------------------------------------------------------------
  Allocate temp ct buffer
  ------------------------------------------------------------------------*/
  if (E_SUCCESS == status && (verify == 1) &&
      (ct_tmp = qsee_malloc(pt_len)) == NULL)
  {
    status = -E_FAILURE;
  }

  /*-----------------------------------------------------------------------
  Clear temp buffer
  ------------------------------------------------------------------------*/
  if ((E_SUCCESS == status) && (verify == 1))
  {
    memset(ct_tmp, 0, pt_len);
  }

  /*-----------------------------------------------------------------------
   If verify is 0,  it means we are not using the predefined test case and
   cannot verify the encrypted packet
  -------------------------------------------------------------------------*/
  if (verify == 0)
    ct_tmp = ct;

  /*-----------------------------------------------------------------------
  Now encrypt the data
  -------------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_encrypt(ctx, pt, pt_len, ct_tmp, &ct_len) < 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_encrypt API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------------
  If NULL key pointer then we are using HW key so don't compare encrypted result
  -----------------------------------------------------------------------------*/
  if ((E_SUCCESS == status) && (verify == 1) && (NULL != key) &&
      0 != memcmp(ct, ct_tmp, pt_len))
  {
    status = -E_FAILURE;
  }

  /*------------------------------------------------------------------------------
  Allocate temp pt buffer
  --------------------------------------------------------------------------------*/
  if ((E_SUCCESS == status) && (verify == 1) && 
      (pt_tmp = qsee_malloc(pt_len)) == NULL)
  {
    status = -E_FAILURE;
  }

  if ((E_SUCCESS == status)&& (verify == 1))
  {
    memset(pt_tmp, 0, pt_len);
  }

  /*-----------------------------------------------------------------------
   If verify is 0,  it means we are not using the predefined test case and
   cannot verify the encrypted packet
  -------------------------------------------------------------------------*/
  if (verify == 0)
    pt_tmp = pt;

  /*--------------------------------------------------------------------------------
  We must set parameters again so we can do the decrypt
  ---------------------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, QSEE_TRIPLE_DES_KEY_SIZE) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*----------------------------------------------------------------------------------
  Set IV if not NULL
  ------------------------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      NULL != iv &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_TRIPLE_DES_IV_SIZE) < 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*----------------------------------------------------------------------------------
  Now decrypt the data
  ------------------------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_decrypt(ctx, ct_tmp, pt_len, pt_tmp, &ct_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_decrypt API failed");
    status = -E_FAILURE;
  }

  /*-------------------------------------------------------------------------------------
  Now compare decrypted results
  ---------------------------------------------------------------------------------------*/
  if ((E_SUCCESS == status) && (verify == 1) &&
      0 != memcmp(pt, pt_tmp, pt_len))
  {  
    status = -E_FAILURE;
  }

  /*-------------------------------------------------------------------------------
  Free ctx
  --------------------------------------------------------------------------------*/
  if (ctx)
  {
    if (qsee_cipher_free_ctx(ctx) < 0)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_free_ctx API failed");
      status = -E_FAILURE;
    }
    ctx = 0;
  }

  /*--------------------------------------------------------------------------------
  Free malloc data
  ---------------------------------------------------------------------------------*/
  if ((ct_tmp) && (verify == 1))
  {
    qsee_free(ct_tmp);
    ct_tmp = 0;
  }

  if ((pt_tmp)&& (verify == 1))
  {
    qsee_free(pt_tmp);
    pt_tmp = 0;
  }

  return status;
}

/**
  @brief 
    Sample code for how to call QSEE Triple DES crypto API's
*/
int tz_app_crypto_triple_des_large_func(QSEE_CIPHER_ALGO_ET alg,
                                        QSEE_CIPHER_MODE_ET mode,
                                        uint8 *pt,
                                        uint32 pt_len,
                                        uint8 *key,
                                        uint8 *iv,
                                        uint8 *ct,
                                        uint8 *ct2)
{
  uint32 ct_len = pt_len;
  qsee_cipher_ctx *ctx = 0;
  int status = E_SUCCESS;
  CryptoCntxHandle *cntx = NULL;
  IovecListType   ioVecIn;
  IovecListType   ioVecOut;
  IovecType       IovecIn;
  IovecType       IovecOut;
  SW_CipherDESEncryptDir sw_dir  = SW_CIPHER_DES_ENCRYPT;
  SW_CipherDESModeType   sw_mode = SW_CIPHER_DES_MODE_CBC;

  //Set SW mode
  if (mode == QSEE_CIPHER_MODE_ECB)
  {
    sw_mode = SW_CIPHER_DES_MODE_ECB;
  }

  /*------------------------------------------------------------------
    Init ctx
    ------------------------------------------------------------------*/
  if (qsee_cipher_init(alg, &ctx) < 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_init API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set key for encryption
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, QSEE_TRIPLE_DES_KEY_SIZE) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set Cipher mode
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set IV only if not NULL
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      NULL != iv &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_TRIPLE_DES_IV_SIZE) < 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }
  
  /*-----------------------------------------------------------------------
  Now encrypt the data
  -------------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_encrypt(ctx, pt, pt_len, ct, &ct_len) < 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_encrypt API failed");
    status = -E_FAILURE;
  }

  /*-----------------------------------------------------------------------
  Now encrypt the data using SW algo
  -------------------------------------------------------------------------*/

  //Now perform encryption using SW
  if(E_SUCCESS == status && qsee_SW_Cipher_DES_Init(&cntx, SW_CIPHER_ALG_DES3) != 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_DES_Init API failed");
    status = -E_FAILURE;
  }

  //Set parameters so we can do the encrypt
  if(E_SUCCESS == status && 
     qsee_SW_Cipher_DES_SetParam(cntx, SW_CIPHER_DES_PARAM_DIRECTION, &sw_dir, sizeof(SW_CipherDESEncryptDir)) != 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_DES_SetParm API failed");
    status = -E_FAILURE;
  }
 
  if(E_SUCCESS == status &&
     qsee_SW_Cipher_DES_SetParam(cntx, SW_CIPHER_DES_PARAM_MODE, &sw_mode, sizeof(SW_CipherDESModeType)) != 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_DES_SetParm API API failed");
    status = -E_FAILURE;
  }

  if(E_SUCCESS == status &&
     qsee_SW_Cipher_DES_SetParam(cntx, SW_CIPHER_DES_PARAM_KEY, key, SW_3DES_KEY_BYTE_LEN) != 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_DES_SetParm API API failed");
    status = -E_FAILURE;
  }
  
  //Check for NULL IV pointer
  if (iv != NULL)
  {
    if(E_SUCCESS == status &&
       qsee_SW_Cipher_DES_SetParam(cntx, SW_CIPHER_DES_PARAM_IV, iv, SW_DES_IV_SIZE) != 0)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_DES_SetParm API API failed");
      status = -E_FAILURE;
    }
  }
 
  /* Input IOVEC */
  ioVecIn.size = 1;
  ioVecIn.iov = &IovecIn;
  ioVecIn.iov[0].dwLen  = pt_len;
  ioVecIn.iov[0].pvBase = pt; 

  /* Output IOVEC */
  ioVecOut.size = 1;
  ioVecOut.iov = &IovecOut;
  ioVecOut.iov[0].dwLen  = pt_len;
  ioVecOut.iov[0].pvBase = ct2;

  //Now encrypt the data
  if(E_SUCCESS == status && qsee_SW_CipherDESData(cntx, ioVecIn, &ioVecOut) != 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_CipherDESData API failed");
    status = -E_FAILURE;
  }
  
  if(E_SUCCESS == status && qsee_SW_Cipher_DES_DeInit(&cntx) !=0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_DES_DeInit API failed");
    status = -E_FAILURE;
  }
  
  //Now compare encrypted results from SW and HW
  if (E_SUCCESS == status && memcmp(ct, ct2, pt_len) != 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"SW and HW encrypted results compare failed");
    status = -E_FAILURE;
  }
  
  /*--------------------------------------------------------------------------------
  We must set parameters again so we can do the decrypt
  ---------------------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, QSEE_TRIPLE_DES_KEY_SIZE) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*----------------------------------------------------------------------------------
  Set IV if not NULL
  ------------------------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      NULL != iv &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_TRIPLE_DES_IV_SIZE) < 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*----------------------------------------------------------------------------------
  Now decrypt the data
  ------------------------------------------------------------------------------------*/
  if (E_SUCCESS == status && qsee_cipher_decrypt(ctx, ct, pt_len, ct, &ct_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_decrypt API failed");
    status = -E_FAILURE;
  }

  /*-------------------------------------------------------------------------------------
  Now compare decrypted results
  ---------------------------------------------------------------------------------------*/
  if ((E_SUCCESS == status) && memcmp(pt, ct, pt_len)!= 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Decrypted results compare failed");
    status = -E_FAILURE;
  }

  /*-------------------------------------------------------------------------------
  Free ctx
  --------------------------------------------------------------------------------*/
  if (ctx)
  {
    if (qsee_cipher_free_ctx(ctx) < 0)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_free_ctx API failed");
      status = -E_FAILURE;
    }
    ctx = 0;
  }

  return status;
}

/**
  @brief 
    Sample code for how to call QSEE Triple DES crypto API's
*/
int tz_app_crypto_aes_large_func(QSEE_CIPHER_ALGO_ET alg,
                                 QSEE_CIPHER_MODE_ET mode,
                                 uint8 *pt,
                                 uint32 pt_len,
                                 uint8 *key,
                                 uint32 key_len,
                                 uint8 *iv,
                                 uint8 *ct,
                                 uint8 *ct2)
{
  uint32              ct_len = pt_len;
  qsee_cipher_ctx     *ctx = 0;
  int                 status = E_SUCCESS;
  CryptoCntxHandle    *cntx = NULL;
  IovecListType       ioVecIn;
  IovecListType       ioVecOut;
  IovecType           IovecIn;
  IovecType           IovecOut;
  SW_CipherEncryptDir sw_dir  = SW_CIPHER_ENCRYPT;
  SW_CipherModeType   sw_mode;
  SW_Cipher_Alg_Type  sw_alg;

  //Determine SW algorithm
  if (alg == QSEE_CIPHER_ALGO_AES_128)
  {
    sw_alg = SW_CIPHER_ALG_AES128;
  }
  else
  {
    sw_alg = SW_CIPHER_ALG_AES256;
  }

  //Set SW mode
  if (mode == QSEE_CIPHER_MODE_ECB)
  {
    sw_mode = SW_CIPHER_MODE_ECB;
  }
  else if (mode == QSEE_CIPHER_MODE_CBC)
  {
    sw_mode = SW_CIPHER_MODE_CBC;
  }
  else
  {
    sw_mode = SW_CIPHER_MODE_CTR;
  }

  /*------------------------------------------------------------------
    Init ctx
    ------------------------------------------------------------------*/
  if (qsee_cipher_init(alg, &ctx) < 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_init API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set key for encryption
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, key_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set Cipher mode
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set IV only if not NULL
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      NULL != iv &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_AES128_IV_SIZE) < 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }
  
  /*-----------------------------------------------------------------------
  Now encrypt the data
  -------------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_encrypt(ctx, pt, pt_len, ct, &ct_len) < 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_encrypt API failed");
    status = -E_FAILURE;
  }

  /*-----------------------------------------------------------------------
  Now encrypt the data using SW algo
  -------------------------------------------------------------------------*/

  //Now perform encryption using SW
  if(E_SUCCESS == status && qsee_SW_Cipher_Init(&cntx, sw_alg) != 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_Init API failed");
    status = -E_FAILURE;
  }

  //Set parameters so we can do the encrypt
  if(E_SUCCESS == status && 
     qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_DIRECTION, &sw_dir, sizeof(SW_CipherEncryptDir)) != 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_SetParm API failed");
    status = -E_FAILURE;
  }
 
  if(E_SUCCESS == status &&
     qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_MODE, &sw_mode, sizeof(SW_CipherModeType)) != 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_SetParm API API failed");
    status = -E_FAILURE;
  }

  if(E_SUCCESS == status &&
     qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_KEY, key, key_len) != 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_SetParm API API failed");
    status = -E_FAILURE;
  }
  
  //Check for NULL IV pointer
  if (iv != NULL)
  {
    if(E_SUCCESS == status &&
       qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_IV, iv, SW_AES_IV_SIZE) != 0)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_SetParm API API failed");
      status = -E_FAILURE;
    }
  }
 
  /* Input IOVEC */
  ioVecIn.size = 1;
  ioVecIn.iov = &IovecIn;
  ioVecIn.iov[0].dwLen  = pt_len;
  ioVecIn.iov[0].pvBase = pt; 

  /* Output IOVEC */
  ioVecOut.size = 1;
  ioVecOut.iov = &IovecOut;
  ioVecOut.iov[0].dwLen  = pt_len;
  ioVecOut.iov[0].pvBase = ct2;

  //Now encrypt the data
  if(E_SUCCESS == status && qsee_SW_CipherData(cntx, ioVecIn, &ioVecOut) != 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_CipherData API failed");
    status = -E_FAILURE;
  }
  
  if(E_SUCCESS == status && qsee_SW_Cipher_DeInit(&cntx, sw_alg) !=0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_DeInit API failed");
    status = -E_FAILURE;
  }
  
  //Now compare encrypted results from SW and HW
  if (E_SUCCESS == status && memcmp(ct, ct2, pt_len) != 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"SW and HW encrypted results compare failed");
    status = -E_FAILURE;
  }
  
  /*--------------------------------------------------------------------------------
  We must set parameters again so we can do the decrypt
  ---------------------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, key_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*----------------------------------------------------------------------------------
  Set IV if not NULL
  ------------------------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      NULL != iv &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_AES128_IV_SIZE) < 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*----------------------------------------------------------------------------------
  Now decrypt the data
  ------------------------------------------------------------------------------------*/
  if (E_SUCCESS == status && qsee_cipher_decrypt(ctx, ct, pt_len, ct, &ct_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_decrypt API failed");
    status = -E_FAILURE;
  }

  /*-------------------------------------------------------------------------------------
  Now compare decrypted results
  ---------------------------------------------------------------------------------------*/
  if ((E_SUCCESS == status) && memcmp(pt, ct, pt_len)!= 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Decrypted results compare failed");
    status = -E_FAILURE;
  }

  /*-------------------------------------------------------------------------------
  Free ctx
  --------------------------------------------------------------------------------*/
  if (ctx)
  {
    if (qsee_cipher_free_ctx(ctx) < 0)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_free_ctx API failed");
      status = -E_FAILURE;
    }
    ctx = 0;
  }

  return status;
}

/**
  @brief 
    Sample code for how to call QSEE AES XTS crypto API's
*/
int tz_app_crypto_aes_xts_func(QSEE_CIPHER_ALGO_ET alg,
                               QSEE_CIPHER_MODE_ET mode,
                               uint8 *pt,
                               uint32 pt_len,
                               uint8 *key,
                               uint32 key_len,
                               uint8 *xts_key,
                               uint32 xts_key_len,
                               uint32 xts_du_size,
                               uint8 *iv,
                               uint8 *ct,
                               uint8 verify)
{
  uint32 ct_len = pt_len;
  qsee_cipher_ctx *ctx = 0;
  uint8 *ct_tmp = 0;
  uint8 *pt_tmp = 0;
  int status = E_SUCCESS;

  /*------------------------------------------------------------------
    Init ctx
    ------------------------------------------------------------------*/
  if (qsee_cipher_init(alg, &ctx) < 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_init API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set key for encryption
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, key_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set AES mode
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
    Set XTS Key 
   ----------------------------------------------------------------------*/  
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_XTS_KEY, xts_key, xts_key_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
    Set XTS DU Size 
  ----------------------------------------------------------------------*/  
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_XTS_DU_SIZE, &xts_du_size, sizeof(uint32)) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set IV only if not NULL
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      NULL != iv &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_AES128_IV_SIZE) < 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }
  
  /*----------------------------------------------------------------------
  Allocate temp ct buffer
  ------------------------------------------------------------------------*/
  if (E_SUCCESS == status && (verify == 1) &&
      (ct_tmp = qsee_malloc(pt_len)) == NULL)
  {
    status = -E_FAILURE;
  }

  /*-----------------------------------------------------------------------
  Clear temp buffer
  ------------------------------------------------------------------------*/
  if ((E_SUCCESS == status) && (verify == 1))
  {
    memset(ct_tmp, 0, pt_len);
  }

  /*-----------------------------------------------------------------------
   If verify is 0,  it means we are not using the predefined test case and
   cannot verify the encrypted packet
  -------------------------------------------------------------------------*/
  if (verify == 0)
    ct_tmp = ct;

  /*-----------------------------------------------------------------------
  Now encrypt the data
  -------------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_encrypt(ctx, pt, pt_len, ct_tmp, &ct_len) < 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_encrypt API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------------
  If NULL key pointer then we are using HW key so don't compare encrypted result
  -----------------------------------------------------------------------------*/
  if ((E_SUCCESS == status) && (verify == 1) && (NULL != key) &&
      0 != memcmp(ct, ct_tmp, pt_len))
  {
    status = -E_FAILURE;
  }

  /*------------------------------------------------------------------------------
  Allocate temp pt buffer
  --------------------------------------------------------------------------------*/
  if ((E_SUCCESS == status) && (verify == 1) && 
      (pt_tmp = qsee_malloc(pt_len)) == NULL)
  {
    status = -E_FAILURE;
  }

  if ((E_SUCCESS == status)&& (verify == 1))
  {
    memset(pt_tmp, 0, pt_len);
  }

  /*-----------------------------------------------------------------------
   If verify is 0,  it means we are not using the predefined test case and
   cannot verify the encrypted packet
  -------------------------------------------------------------------------*/
  if (verify == 0)
    pt_tmp = pt;

  /*--------------------------------------------------------------------------------
  We must set parameters again so we can do the decrypt
  ---------------------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, key_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_XTS_KEY, xts_key, xts_key_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_XTS_DU_SIZE, &xts_du_size, sizeof(uint32)) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*----------------------------------------------------------------------------------
  Set IV if not NULL
  ------------------------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      NULL != iv &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_AES128_IV_SIZE) < 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*----------------------------------------------------------------------------------
  Now decrypt the data
  ------------------------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_decrypt(ctx, ct_tmp, pt_len, pt_tmp, &ct_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_decrypt API failed");
    status = -E_FAILURE;
  }

  /*-------------------------------------------------------------------------------------
  Now compare decrypted results
  ---------------------------------------------------------------------------------------*/
  if ((E_SUCCESS == status) && (verify == 1) &&
      0 != memcmp(pt, pt_tmp, pt_len))
  {  
    status = -E_FAILURE;
  }

  /*-------------------------------------------------------------------------------
  Free ctx
  --------------------------------------------------------------------------------*/
  if (ctx)
  {
    if (qsee_cipher_free_ctx(ctx) < 0)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_free_ctx API failed");
      status = -E_FAILURE;
    }
    ctx = 0;
  }

  /*--------------------------------------------------------------------------------
  Free malloc data
  ---------------------------------------------------------------------------------*/
  if ((ct_tmp) && (verify == 1))
  {
    qsee_free(ct_tmp);
    ct_tmp = 0;
  }

  if ((pt_tmp)&& (verify == 1))
  {
    qsee_free(pt_tmp);
    pt_tmp = 0;
  }

  return status;
} 

/**
  @brief 
    Sample code for how to call QSEE AES CCM crypto API's
*/
int tz_app_crypto_aes_ccm_func(QSEE_CIPHER_ALGO_ET alg,
                               QSEE_CIPHER_MODE_ET mode,
                               uint8 *pt,
                               uint32 pt_len,
                               uint8 *key,
                               uint32 key_len,
                               uint8 *iv,
                               uint8 *nonce,
                               uint32 nonce_len,
                               uint32 payload_len,
                               uint8 *ct,
                               uint32 ct_len,
                               uint8* ct2,
                               uint32 header_len,
                               uint32 sw_verify)
{
  qsee_cipher_ctx *ctx = 0;
  uint32 mac_len = ct_len - pt_len;
  //uint32 sw_ct_len = ct_len - mac_len;
  uint32 cmp_offset;
  int status = E_SUCCESS;
  IovecListType   ioVecIn;
  IovecListType   ioVecOut;
  IovecType       IovecIn;
  IovecType       IovecOut;
  uint8           tag_auth[16];
  uint32          header_data = 0;
  SW_CipherEncryptDir sw_dir = SW_CIPHER_ENCRYPT;
  SW_CipherModeType   sw_mode  = SW_CIPHER_MODE_CCM;
  SW_Cipher_Alg_Type  sw_alg;
  CryptoCntxHandle *  sw_cntx = 0;
  uint32 sw_ct_len = ct_len - mac_len;

  /*------------------------------------------------------------------
  Init ctx
  ------------------------------------------------------------------*/
  if (qsee_cipher_init(alg, &ctx) < 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_init API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set key for encryption
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, key_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set AES mode
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set IV only if not NULL
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      NULL != iv &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_AES128_IV_SIZE) < 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set CCM Payload len
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_CCM_PAYLOAD_LEN, &payload_len, sizeof(payload_len)) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set CCM Nonce
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_NONCE, nonce, nonce_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set CCM Header len
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_CCM_HDR_LEN, &header_len, sizeof(header_len)) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set CCM MAC len
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_CCM_MAC_LEN, &mac_len, sizeof(mac_len)) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*-----------------------------------------------------------------------
  Now encrypt the data
  -------------------------------------------------------------------------*/
  if (E_SUCCESS == status && 
      qsee_cipher_encrypt(ctx, pt, pt_len, ct2, &ct_len) < 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_encrypt API failed");
    status = -E_FAILURE;
  }

  //Check if we need to do SW verify
  if (sw_verify)
  {
    //Determine Algo
    if (alg == QSEE_CIPHER_ALGO_AES_128)
    {
      sw_alg = SW_CIPHER_ALG_AES128; 
    }
    else
    {
      sw_alg = SW_CIPHER_ALG_AES256;
    }

    /* Input IOVEC */
    ioVecIn.size = 1;
    ioVecIn.iov = &IovecIn;
    ioVecIn.iov[0].dwLen = pt_len;
    ioVecIn.iov[0].pvBase = (void*)pt; 

    /* Output IOVEC */
    ioVecOut.size = 1;
    ioVecOut.iov = &IovecOut;
    ioVecOut.iov[0].dwLen = sw_ct_len;
    ioVecOut.iov[0].pvBase = (void*)ct;

    /*--------------------------------------------------------------------
    Init context
    ----------------------------------------------------------------------*/
    if(0 != qsee_SW_Cipher_Init(&sw_cntx, sw_alg))
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_Init API failed");
      status = -E_FAILURE;
    }

    /*--------------------------------------------------------------------
    Set direction to encryption
    ----------------------------------------------------------------------*/
    if (E_SUCCESS == status &&
        qsee_SW_Cipher_SetParam(sw_cntx, SW_CIPHER_PARAM_DIRECTION, &sw_dir, sizeof(SW_CipherEncryptDir)) != 0)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_SetParam API failed");
      status = -E_FAILURE;
    }

    /*--------------------------------------------------------------------
    Set AES mode
    ----------------------------------------------------------------------*/
    if (E_SUCCESS == status &&
        qsee_SW_Cipher_SetParam(sw_cntx, SW_CIPHER_PARAM_MODE, &sw_mode, sizeof(sw_mode)) != 0)
    {  
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_SetParam API failed");
      status = -E_FAILURE;
    }

    /*--------------------------------------------------------------------
    Set key for encryption
    ----------------------------------------------------------------------*/
    if (E_SUCCESS == status &&
        qsee_SW_Cipher_SetParam(sw_cntx, SW_CIPHER_PARAM_KEY, key, key_len) != 0)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
    }

    /*--------------------------------------------------------------------
    Set IV
    ----------------------------------------------------------------------*/
    if(mode != SW_CIPHER_MODE_ECB)
    {
      if(E_SUCCESS == status &&
         qsee_SW_Cipher_SetParam(sw_cntx, SW_CIPHER_PARAM_IV, nonce, nonce_len) != 0)
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_SetParam API failed");
        status = -E_FAILURE;
      }
    }

    /*--------------------------------------------------------------------
    Set AAD 
    ----------------------------------------------------------------------*/
    if (E_SUCCESS == status && 
       qsee_SW_Cipher_SetParam(sw_cntx, SW_CIPHER_PARAM_AAD, &header_data, 0) != 0)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_SetParam API failed");
      status = -E_FAILURE;
    }

    /*--------------------------------------------------------------------
    Set TAG 
    ----------------------------------------------------------------------*/
    if(E_SUCCESS == status && 
       qsee_SW_Cipher_SetParam(sw_cntx, SW_CIPHER_PARAM_TAG, tag_auth, mac_len) != 0)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_SetParam API failed");
      status = -E_FAILURE;
    }

    /*-----------------------------------------------------------------------
    Now encrypt the data
    -------------------------------------------------------------------------*/
    if (E_SUCCESS == status &&
        qsee_SW_CipherData(sw_cntx, ioVecIn, &ioVecOut) != 0)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_CipherData encrypt API failed");
      status = -E_FAILURE;
    }

    /*-----------------------------------------------------------------------
    Get the auth tag
    -------------------------------------------------------------------------*/
    if (E_SUCCESS == status &&
        0 != qsee_SW_Cipher_GetParam(sw_cntx, SW_CIPHER_PARAM_TAG, tag_auth, mac_len))
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_GetParam API failed");
      status = -E_FAILURE;
    }

    //Now compare the encrypted results HW vs SW
    if (memcmp(ct, ct2, sw_ct_len) != 0)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"SW vs HW cipher text compare failed");
      status = -E_FAILURE;
    }

    //Now compare the MAC results HW vs SW
    if(memcmp(tag_auth, ct2 + sw_ct_len, mac_len) != 0)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"SW vs HW MAC compare failed");
      status = -E_FAILURE;
    }

    /*--------------------------------------------------------------------
    Free context
    ----------------------------------------------------------------------*/
    if(0 != qsee_SW_Cipher_DeInit(&sw_cntx, sw_alg)) 
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_DeInit API failed");
      status = -E_FAILURE;
    }
  }
  else
  {
    /*--------------------------------------------------------------------------
    Now compare encrypted results
    -----------------------------------------------------------------------------*/
    cmp_offset = pt_len - payload_len;
    if (E_SUCCESS == status &&
        0 != memcmp(ct + cmp_offset, ct2 + cmp_offset, pt_len - cmp_offset))
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"HW cipher text & MAC compare failed");
      status = -E_FAILURE;
    }
  }

  /*-------------------------------------------------------------------------------
  Free ctx
  --------------------------------------------------------------------------------*/
  if (ctx)
  {
    if (qsee_cipher_free_ctx(ctx) < 0)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_free_ctx API failed");
      status = -E_FAILURE;
    }
    ctx = 0;
  }
  /*------------------------------------------------------------------
  Init ctx
  ------------------------------------------------------------------*/
  if (qsee_cipher_init(alg, &ctx) < 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_init API failed");
    status = -E_FAILURE;
  }
  /*--------------------------------------------------------------------------------
  We must set parameters again so we can do the decrypt
  ---------------------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, key_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status &&
      NULL != iv &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_AES128_IV_SIZE) < 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_CCM_PAYLOAD_LEN, &payload_len, sizeof(payload_len)) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_NONCE, nonce, nonce_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_CCM_HDR_LEN, &header_len, sizeof(header_len)) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_CCM_MAC_LEN, &mac_len, sizeof(mac_len)) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }  

  /*----------------------------------------------------------------------------------
  Now decrypt the data
  ------------------------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_decrypt(ctx, ct2, ct_len, ct2, &pt_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_decrypt API failed");
    status = -E_FAILURE;
  }

  /*-------------------------------------------------------------------------------------
  Now compare decrypted results
  ---------------------------------------------------------------------------------------*/
  cmp_offset = pt_len - payload_len;
  if ((E_SUCCESS == status) &&
      0 != memcmp(pt + cmp_offset, ct2 + cmp_offset, pt_len - cmp_offset))
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"ccm memcompare failed");
    status = -E_FAILURE;
  }

  /*-------------------------------------------------------------------------------
  Free ctx
  --------------------------------------------------------------------------------*/
  if (ctx)
  {
    if (qsee_cipher_free_ctx(ctx) < 0)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_free_ctx API failed");
      status = -E_FAILURE;
    }
    ctx = 0;
  }

  return status;
} 

/**
  @brief 
    Sample code for how to call QSEE AES crypto API's
*/
int tz_app_crypto_aes_block_func(QSEE_CIPHER_ALGO_ET alg,
                                 QSEE_CIPHER_MODE_ET mode,
                                 uint8 *pt,
                                 uint32 pt_len,
                                 uint8 *key,
                                 uint32 key_len,
                                 uint8 *iv,
                                 uint8 *ct)
{
  uint32 ct_len = pt_len;
  uint32 pt_len2 = pt_len / 2;
  uint32 ct_len2 = pt_len2;
  qsee_cipher_ctx *ctx = 0;
  uint8 *pt_tmp = 0;
  int status = E_SUCCESS;

  /*------------------------------------------------------------------
    Init ctx
    ------------------------------------------------------------------*/
  if (qsee_cipher_init(alg, &ctx) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_init API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set key for decryption
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, key_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set AES mode
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set IV only if not NULL
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      NULL != iv &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_AES128_IV_SIZE) < 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*-----------------------------------------------------------------------
  Now decrypt the entire data buffer
  -------------------------------------------------------------------------*/
  if (E_SUCCESS == status && 
      qsee_cipher_decrypt(ctx, ct, pt_len, pt, &ct_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_decrypt API failed");
    status = -E_FAILURE;
  }

  /*-------------------------------------------------------------------------------
  Free ctx
  --------------------------------------------------------------------------------*/
  if (ctx)
  {
    if (qsee_cipher_free_ctx(ctx) < 0)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_free_ctx API failed");
      status = -E_FAILURE;
    }
    ctx = 0;
  } 

  /*------------------------------------------------------------------------------
  Allocate temp pt buffer
  --------------------------------------------------------------------------------*/
  if ((E_SUCCESS == status) && (pt_tmp = qsee_malloc(pt_len)) == NULL)
  {
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status)
  {
    memset(pt_tmp, 0, pt_len);
  } 

  /*------------------------------------------------------------------
  Init ctx
  ------------------------------------------------------------------*/
  if ((E_SUCCESS == status) && qsee_cipher_init(alg, &ctx) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_init API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set key for decryption
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, key_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set AES mode
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set IV only if not NULL
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      NULL != iv &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_AES128_IV_SIZE) < 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*-----------------------------------------------------------------------
  Now decrypt the the first half of the data buffer 
  -------------------------------------------------------------------------*/
  if (E_SUCCESS == status && 
      qsee_cipher_decrypt(ctx, ct, pt_len2, pt_tmp, &ct_len2) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_decrypt API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set key for again for another decryption
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, key_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*-----------------------------------------------------------------------
  Now decrypt the the second half of the data buffer 
  -------------------------------------------------------------------------*/
  if (E_SUCCESS == status && 
      qsee_cipher_decrypt(ctx, ct + pt_len2, pt_len2, pt_tmp + pt_len2, &ct_len2) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_decrypt API failed");
    status = -E_FAILURE;
  } 

  /*--------------------------------------------------------------------------
  Now compare decrypted results
  -----------------------------------------------------------------------------*/
  if ((E_SUCCESS == status) && (0 != memcmp(pt, pt_tmp, pt_len)))
  {
    status = -E_FAILURE;
  }

  /*-------------------------------------------------------------------------------
  Free ctx
  --------------------------------------------------------------------------------*/
  if (ctx)
  {
    if (qsee_cipher_free_ctx(ctx) < 0)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_free_ctx API failed");
      status = -E_FAILURE;
    }
    ctx = 0;
  } 

  /*--------------------------------------------------------------------------------
  Free malloc data
  ---------------------------------------------------------------------------------*/
  if (pt_tmp)
  {
    qsee_free(pt_tmp);
    pt_tmp = 0;
  }

  return status;
}

/**
  @brief 
    Sample code for how to call QSEE HASH crypto API's
*/
int tz_app_crypto_hash_func(QSEE_HASH_ALGO_ET alg,
                            uint8 *msg,
                            uint32 msg_len,
                            uint8 *digest)
{
  int status = E_SUCCESS;
  uint8 result_digest[QSEE_SHA256_HASH_SZ];
  uint32 digest_len = 0;

  //Clear result digest
  memset(result_digest, 0, sizeof(result_digest));

  //Set digest length
  if (alg == QSEE_HASH_SHA1)
    digest_len = QSEE_SHA1_HASH_SZ;
  else
    digest_len = QSEE_SHA256_HASH_SZ;

  //Calculate HASH
  if (qsee_hash(alg, msg, msg_len, result_digest, digest_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_hash API failed");
    status = -E_FAILURE;
  }

  //Compare rsults 
  if ((E_SUCCESS == status) && (0 != memcmp(result_digest, digest, digest_len)))
  {
    status = -E_FAILURE;
  }

  return status;
}

/**
  @brief 
    Sample code for how to call QSEE HASH crypto API's
*/
int tz_app_crypto_hash_block_func(QSEE_HASH_ALGO_ET alg,
                                  uint8 *msg,
                                  uint32 msg_len)
{
  int status = E_SUCCESS;
  uint8 result_digest[QSEE_SHA256_HASH_SZ];
  uint8 result_digest2[QSEE_SHA256_HASH_SZ];
  uint32 digest_len = 0;
  uint32 msg_len2 = msg_len / 2;
  qsee_hash_ctx *hash_ctx = NULL;

  //Clear result digest
  memset(result_digest, 0, sizeof(result_digest));
  memset(result_digest2, 0, sizeof(result_digest2));

  //Set digest length
  if (alg == QSEE_HASH_SHA1)
    digest_len = QSEE_SHA1_HASH_SZ;
  else
    digest_len = QSEE_SHA256_HASH_SZ;

  //Calculate HASH of the entire data buffer
  if (qsee_hash(alg, msg, msg_len, result_digest, digest_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_hash API failed");
    status = -E_FAILURE;
  }

  //Create hash context
  if (E_SUCCESS == status && qsee_hash_init(alg, &hash_ctx) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_hash_init API failed");
    status = -E_FAILURE;
  }

  //HASH first half of the data buffer
  if (E_SUCCESS == status && qsee_hash_update(hash_ctx, msg, msg_len2) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_hash_update API failed");
    status = -E_FAILURE;
  }

  //HASH second half of data buffer
  if (E_SUCCESS == status && qsee_hash_update(hash_ctx, msg + msg_len2, msg_len2) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_hash_update API failed");
    status = -E_FAILURE;
  }

  //Call HASH final to complete hash operation
  if (E_SUCCESS == status && qsee_hash_final(hash_ctx, result_digest2, digest_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_hash_final API failed");
    status = -E_FAILURE;
  }

  //Compare rsults 
  if ((E_SUCCESS == status) && (0 != memcmp(result_digest, result_digest2, digest_len)))
  {
    status = -E_FAILURE;
  }

  //Free HASH context
  if (hash_ctx != NULL)
  {
    if (qsee_hash_free_ctx(hash_ctx) < 0)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_hash_free_ctx API failed");
      status = -E_FAILURE;
    }
  }
  return status;
}


/**
  @brief 
    Sample code for how to call QSEE HMAC crypto API's
*/
int tz_app_crypto_hmac_func(uint8 *msg,
                            uint32 msg_len,
                            uint8* key,
                            uint32 key_len,
                            uint8* hmac1,
                            uint8* hmac256)
{
  int status = E_SUCCESS;
  uint8 result_digest[QSEE_SHA256_HASH_SZ];

  //Clear result digest
  memset(result_digest, 0, sizeof(result_digest));

  //Calculate HASH SHA1
  if (qsee_hmac(QSEE_HMAC_SHA1, msg, msg_len, key, key_len, result_digest) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_hmac API failed");
    status = -E_FAILURE;
  }

  //Compare rsults 
  if ((E_SUCCESS == status) && (0 != memcmp(result_digest, hmac1, QSEE_SHA1_HASH_SZ)))
  {
    status = -E_FAILURE;
  }

  //Clear result digest
  memset(result_digest, 0, sizeof(result_digest));

  //Calculate HASH SHA256
  if ((E_SUCCESS == status) && 
      qsee_hmac(QSEE_HMAC_SHA256, msg, msg_len, key, key_len, result_digest) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_hmac API failed");
    status = -E_FAILURE;
  }

  //Compare rsults 
  if ((E_SUCCESS == status) && (0 != memcmp(result_digest, hmac256, QSEE_SHA256_HASH_SZ)))
  {
    status = -E_FAILURE;
  }

  return status;
}

/**
  @brief 
    Sample code for how to call QSEE CMAC crypto API's
*/
int tz_app_crypto_cmac_func(uint8 *msg,
                            uint32 msg_len,
                            uint8* key,
                            uint32 key_len,
                            uint8* cmac,
                            uint32 cmac_len)
{
  int status = E_SUCCESS;
  uint8 cmac_digest[QSEE_CMAC_DIGEST_SIZE];
  QSEE_CMAC_ALGO_ET alg;
 
  //Determine algorithm
  if (key_len == 16)
  {
    alg = QSEE_CMAC_ALGO_AES_128;
  }
  else
  {
    alg = QSEE_CMAC_ALGO_AES_256;
  }

  //Clear result digest
  memset(cmac_digest, 0, sizeof(cmac_digest));

  //Calculate CMAC
  if (qsee_cmac(alg, msg, msg_len, key, key_len, cmac_digest, cmac_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cmac API failed");
    status = -E_FAILURE;
  }

  //Compare rsults 
  if ((E_SUCCESS == status) && (0 != memcmp(cmac_digest, cmac, cmac_len)))
  {
    status = -E_FAILURE;
  }

  return status;
}

/**
  @brief 
    Sample code for how to call QSEE AES crypto API's
*/
int tz_app_crypto_aes_perf(QSEE_CIPHER_ALGO_ET alg,
                           QSEE_CIPHER_MODE_ET mode,
                           uint8 *pt,
                           uint32 pt_len,
                           uint8 *key,
                           uint32 key_len,
                           uint8 *iv)
{
  uint32 ct_len = pt_len;
  uint8 *ct = pt;
  qsee_cipher_ctx *ctx = 0;
  int status = E_SUCCESS;

  /*------------------------------------------------------------------
    Init ctx
    ------------------------------------------------------------------*/
  if (qsee_cipher_init(alg, &ctx) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_init API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set key for encryption
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, key_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set AES mode
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set IV only if not NULL
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      NULL != iv &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_AES128_IV_SIZE) < 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  } 
  
  /*-----------------------------------------------------------------------
  Now encrypt the data
  -------------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_encrypt(ctx, pt, pt_len, ct, &ct_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_encrypt API failed");
    status = -E_FAILURE;
  }

  /*-------------------------------------------------------------------------------
  Free ctx
  --------------------------------------------------------------------------------*/
  if (ctx)
  {
    if (qsee_cipher_free_ctx(ctx) < 0)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_free_ctx API failed");
      status = -E_FAILURE;
    }
    ctx = 0;
  }

  return status;
}

/**
  @brief 
    Sample code for how to call QSEE Triple DES crypto API's
*/
int tz_app_crypto_triple_des_perf(QSEE_CIPHER_ALGO_ET alg,
                                  QSEE_CIPHER_MODE_ET mode,
                                  uint8 *pt,
                                  uint32 pt_len,
                                  uint8 *key,
                                  uint8 *iv)
{
  uint32 ct_len = pt_len;
  uint8 *ct = pt;
  qsee_cipher_ctx *ctx = 0;
  int status = E_SUCCESS;

  /*------------------------------------------------------------------
    Init ctx
    ------------------------------------------------------------------*/
  if (qsee_cipher_init(alg, &ctx) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_init API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set key for encryption
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, QSEE_TRIPLE_DES_KEY_SIZE) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set AES mode
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  } 

  /*--------------------------------------------------------------------
  Set IV only if not NULL
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      NULL != iv &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_TRIPLE_DES_IV_SIZE) < 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  } 
  
  /*-----------------------------------------------------------------------
  Now encrypt the data
  -------------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_encrypt(ctx, pt, pt_len, ct, &ct_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_encrypt API failed");
    status = -E_FAILURE;
  }

  /*-------------------------------------------------------------------------------
  Free ctx
  --------------------------------------------------------------------------------*/
  if (ctx)
  {
    if (qsee_cipher_free_ctx(ctx) < 0)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_free_ctx API failed");
      status = -E_FAILURE;
    }
    ctx = 0;
  }

  return status;
}

/**
  @brief 
    Sample code for how to call QSEE AES XTS crypto API's
*/
int tz_app_crypto_aes_xts_perf(QSEE_CIPHER_ALGO_ET alg,
                               QSEE_CIPHER_MODE_ET mode,
                               uint8 *pt,
                               uint32 pt_len,
                               uint8 *key,
                               uint32 key_len,
                               uint8 *xts_key,
                               uint32 xts_key_len,
                               uint32 xts_du_size,
                               uint8 *iv)
{
  uint32 ct_len = pt_len;
  uint8 *ct = pt;
  qsee_cipher_ctx *ctx = 0;
  int status = E_SUCCESS;

  /*------------------------------------------------------------------
    Init ctx
    ------------------------------------------------------------------*/
  if (qsee_cipher_init(alg, &ctx) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_init API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set key for encryption
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, key_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set AES mode
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  } 

  /*--------------------------------------------------------------------
    Set XTS Key 
   ----------------------------------------------------------------------*/  
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_XTS_KEY, xts_key, xts_key_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
    Set XTS DU Size 
  ----------------------------------------------------------------------*/  
  if (E_SUCCESS == status &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_XTS_DU_SIZE, &xts_du_size, sizeof(uint32)) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set IV only if not NULL
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      NULL != iv &&
      qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_AES128_IV_SIZE) < 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  } 
  
  /*-----------------------------------------------------------------------
  Now encrypt the data
  -------------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_cipher_encrypt(ctx, pt, pt_len, ct, &ct_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_encrypt API failed");
    status = -E_FAILURE;
  }

  /*-------------------------------------------------------------------------------
  Free ctx
  --------------------------------------------------------------------------------*/
  if (ctx)
  {
    if (qsee_cipher_free_ctx(ctx) < 0)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_free_ctx API failed");
      status = -E_FAILURE;
    }
    ctx = 0;
  }

  return status;
}

/**
  @brief 
    Sample code for how to call QSEE HASH crypto API's
*/
int tz_app_crypto_hash_perf(QSEE_HASH_ALGO_ET alg,
                            uint8 *msg,
                            uint32 msg_len)
{
  int status = E_SUCCESS;
  uint8 result_digest[QSEE_SHA256_HASH_SZ];
  uint32 digest_len = 0;

  //Set digest length
  if (alg == QSEE_HASH_SHA1)
    digest_len = QSEE_SHA1_HASH_SZ;
  else
    digest_len = QSEE_SHA256_HASH_SZ;

  //Calculate HASH
  if (qsee_hash(alg, msg, msg_len, result_digest, digest_len) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_hash API failed");
    status = -E_FAILURE;
  }

  return status;
}

/*
	Sample code for how to call QSEE HMAC crypto API's
*/
int tz_app_crypto_hmac_perf(QSEE_HMAC_ALGO_ET alg,
                            uint8 *msg,
                            uint32 msg_len,
                            uint8* key,
                            uint32 key_len)
{
  int status = E_SUCCESS;
  uint8 result_digest[QSEE_SHA256_HASH_SZ];

  //Calculate HMAC
  if (qsee_hmac(alg, msg, msg_len, key, key_len, result_digest) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_hmac API failed");
    status = -E_FAILURE;
  }

  return status;
}

/**
  @brief 
    Sample code for how to call QSEE CMAC crypto API's
*/
int tz_app_crypto_cmac_perf(QSEE_CMAC_ALGO_ET alg,
                            uint8 *msg,
                            uint32 msg_len,
                            uint8* key,
                            uint32 key_len)
{
  int status = E_SUCCESS;
  uint8 cmac_digest[QSEE_CMAC_DIGEST_SIZE];

  //Calculate CMAC
  if (qsee_cmac(alg, msg, msg_len, key, key_len, cmac_digest, sizeof(cmac_digest)) < 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cmac API failed");
    status = -E_FAILURE;
  }

  return status;
}

/**
  @brief 
    Sample code for AES functional tests
*/
int tz_app_aes_func(void)
{
  int idx=0, jdx=0;
  int num=0;
  int status = E_SUCCESS;
  uint32 plain_text_len = sizeof(aes_large_plain_text2);
  num = sizeof(aes_test_vectors) / sizeof(aes_vector_type_t);

  /*---------------------------------------------------------------
  Run through AES128/256 ECB/CBC/CTR test vectors
  -------------------------------------------------------------------*/
  for (idx = 0; idx < num; idx++)
  {
    status = tz_app_crypto_aes_func(aes_test_vectors[idx].alg,
                                    aes_test_vectors[idx].mode,
                                    aes_test_vectors[idx].pt,
                                    aes_test_vectors[idx].pt_len,
                                    aes_test_vectors[idx].key,
                                    aes_test_vectors[idx].key_len,
                                    aes_test_vectors[idx].iv,
                                    aes_test_vectors[idx].ct, 1);

    if (status == E_SUCCESS)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto AES Functional Test - Vect Num:%u Passed", idx);
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto AES Functional Test - Vect Num:%u Failed", idx);
      return status;
    }
  }  

  /*---------------------------------------------------------------
  Run AES multiple block test vector
  -------------------------------------------------------------------*/ 
  memset(aes_large_plain_text_1, 0, sizeof(aes_large_plain_text_1));
  memset(aes_large_cipher_text_1, 0xed, sizeof(aes_large_cipher_text_1));

  status = tz_app_crypto_aes_block_func(QSEE_CIPHER_ALGO_AES_128,
                                        QSEE_CIPHER_MODE_CBC,
                                        aes_large_plain_text_1,
                                        sizeof(aes_large_plain_text_1),
                                        aes_key_2,
                                        sizeof(aes_key_2),
                                        aes_iv_2,
                                        aes_large_cipher_text_1);

  if (status == E_SUCCESS)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto AES Functional Block Test Passed");
  }
  else
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto AES Functional Block Test Failed");
  }

 /*---------------------------------------------------------------
  Run through AES ECB, CBC and CTR large test vectors
  ----------------------------------------------------------------*/   

  memset(aes_large_plain_text2, 0xED, plain_text_len);

  aes_test_vectors_2.pt  = aes_large_plain_text2;
  aes_test_vectors_2.ct  = aes_large_cipher_text2;
  aes_test_vectors_2.ct2 = aes_performance; 

  //Test AES CBC with large vectors
  for (idx = 16; idx <= plain_text_len; idx *= 2) 
  { 
    aes_test_vectors_2.pt_len = idx;

    status = tz_app_crypto_aes_large_func(aes_test_vectors_2.alg,
                                          QSEE_CIPHER_MODE_CBC,
                                          aes_test_vectors_2.pt,
                                          aes_test_vectors_2.pt_len,
                                          aes_test_vectors_2.key,
                                          aes_test_vectors_2.key_len,
                                          aes_test_vectors_2.iv,
                                          aes_test_vectors_2.ct,
                                          aes_test_vectors_2.ct2);

    if (status == E_SUCCESS)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto AES CBC Large Vector Functional Test - Vect Size:%u Passed", idx);
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto AES CBC Large Vector Functional Test - Vect Size:%u Failed", idx);
      return status;
    }
  }

  //Test AES ECB with large vectors
  for (idx = 16; idx <= plain_text_len; idx *= 2) 
  { 
    aes_test_vectors_2.pt_len = idx;

    status = tz_app_crypto_aes_large_func(aes_test_vectors_2.alg,
                                          QSEE_CIPHER_MODE_ECB,
                                          aes_test_vectors_2.pt,
                                          aes_test_vectors_2.pt_len,
                                          aes_test_vectors_2.key,
                                          aes_test_vectors_2.key_len,
                                          NULL,
                                          aes_test_vectors_2.ct,
                                          aes_test_vectors_2.ct2);

    if (status == E_SUCCESS)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto AES ECB Large Vector Functional Test - Vect Size:%u Passed", idx);
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto AES ECB Large Vector Functional Test - Vect Size:%u Failed", idx);
      return status;
    }
  }

  //Test AES CTR with large vectors
  for (idx = 8; idx <= plain_text_len; idx *= 2) 
  { 
    aes_test_vectors_2.pt_len = idx;

    status = tz_app_crypto_aes_large_func(aes_test_vectors_2.alg,
                                          QSEE_CIPHER_MODE_CTR,
                                          aes_test_vectors_2.pt,
                                          aes_test_vectors_2.pt_len,
                                          aes_test_vectors_2.key,
                                          aes_test_vectors_2.key_len,
                                          aes_test_vectors_2.iv,
                                          aes_test_vectors_2.ct,
                                          aes_test_vectors_2.ct2);

    if (status == E_SUCCESS)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto AES CTR Large Vector Functional Test - Vect Size:%u Passed", idx);
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto AES CTR Large Vector Functional Test - Vect Size:%u Failed", idx);
      return status;
    }
  }  

  /*---------------------------------------------------------------
  Run through AES CTR large vector test with rotating IV vector
  ----------------------------------------------------------------*/

  //Zero IV vector
  memset(aes_iv_4, 0, sizeof(aes_iv_4));

  //Loop over different IVs starting with IV = all zeros to IV = all 0xFF
  for (jdx = 0; jdx < sizeof(aes_iv_4) - 1; jdx++)
  {
    //Test AES CTR with large vectors
    for (idx = 8; idx <= plain_text_len; idx *= 2) 
    { 
      aes_test_vectors_2.pt_len = idx;

      status = tz_app_crypto_aes_large_func(aes_test_vectors_2.alg,
                                            QSEE_CIPHER_MODE_CTR,
                                            aes_test_vectors_2.pt,
                                            aes_test_vectors_2.pt_len,
                                            aes_test_vectors_2.key,
                                            aes_test_vectors_2.key_len,
                                            aes_iv_4,
                                            aes_test_vectors_2.ct,
                                            aes_test_vectors_2.ct2);

      if (status == E_SUCCESS)
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto AES CTR Large Vector Rotating IV Functional Test - Vect Size:%u Passed",idx);
      }
      else
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto AES CTR Large Vector Rotating IV Functional Test - Vect Size:%u Failed",idx);
        return status;
      }
    }

    //Increment IV vector 
    aes_iv_4[jdx] = 0xff;
  }

  return status;
}

/**
  @brief 
    Sample code for DES functional tests
*/
int tz_app_des_func(void)
{
  int idx=0;
  int num=0;
  int status = E_SUCCESS;
  uint32 plain_text_len = sizeof(aes_large_plain_text2);

  num = sizeof(triple_des_test_vectors) / sizeof(triple_des_vector_t);

  /*---------------------------------------------------------------
  Run through Triple DES ECB and CBC test vectors
  -------------------------------------------------------------------*/
  for (idx = 0; idx < num; idx++)
  {
    status = tz_app_crypto_triple_des_func(triple_des_test_vectors[idx].alg,
                                           triple_des_test_vectors[idx].mode,
                                           triple_des_test_vectors[idx].pt,
                                           triple_des_test_vectors[idx].pt_len,
                                           triple_des_test_vectors[idx].key,
                                           triple_des_test_vectors[idx].iv,
                                           triple_des_test_vectors[idx].ct, 1);

    if (status == E_SUCCESS)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto Triple DES Functional Test - Vect Num:%u Passed", idx);
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto Triple DES Functional Test - Vect Num:%u Failed", idx);
      return status;
    }
  }
  
  /*---------------------------------------------------------------
  Run through Triple DES ECB and CBC large test vectors
  -------------------------------------------------------------------*/   

  memset(aes_large_plain_text2, 0xED, plain_text_len);

  triple_des_test_vectors_2.pt  = aes_large_plain_text2;
  triple_des_test_vectors_2.ct  = aes_large_cipher_text2;
  triple_des_test_vectors_2.ct2 = aes_performance; 

  //Test Triple DES CBC with large vectors
  for (idx = 8; idx <= plain_text_len; idx *= 2) 
  { 
    triple_des_test_vectors_2.pt_len = idx;

    status = tz_app_crypto_triple_des_large_func(triple_des_test_vectors_2.alg,
                                                 QSEE_CIPHER_MODE_CBC,
                                                 triple_des_test_vectors_2.pt,
                                                 triple_des_test_vectors_2.pt_len,
                                                 triple_des_test_vectors_2.key,
                                                 triple_des_test_vectors_2.iv,
                                                 triple_des_test_vectors_2.ct,
                                                 triple_des_test_vectors_2.ct2);

    if (status == E_SUCCESS)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto Triple DES CBC Large Vector Functional Test - Vect Size:%u Passed", idx);
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto Triple DES CBC Large Vector Functional Test - Vect Size:%u Failed", idx);
      return status;
    }
  }

  //Test Triple DES ECB with large vectors
  for (idx = 8; idx <= plain_text_len; idx *= 2) 
  { 
    triple_des_test_vectors_2.pt_len = idx;

    status = tz_app_crypto_triple_des_large_func(triple_des_test_vectors_2.alg,
                                                 QSEE_CIPHER_MODE_ECB,
                                                 triple_des_test_vectors_2.pt,
                                                 triple_des_test_vectors_2.pt_len,
                                                 triple_des_test_vectors_2.key,
                                                 NULL,
                                                 triple_des_test_vectors_2.ct,
                                                 triple_des_test_vectors_2.ct2);

    if (status == E_SUCCESS)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto Triple DES ECB Large Vector Functional Test - Vect Size:%u Passed", idx);
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto Triple DES ECB Large Vector Functional Test - Vect Size:%u Failed", idx);
      return status;
    }
  }

  return status;
}

/**
  @brief 
    Sample code for DES functional tests
*/
int tz_app_aes_xts_func(void)
{
  int idx=0;
  int num=0;
  int status = E_SUCCESS;
  num = sizeof(aes_xts_test_vectors) / sizeof(aes_xts_vector_t);

  /*---------------------------------------------------------------
  Run through AES 126 & 256 XTS test vectors
  -------------------------------------------------------------------*/
  for (idx = 0; idx < num; idx++)
  {
    status = tz_app_crypto_aes_xts_func(aes_xts_test_vectors[idx].alg,
                                        aes_xts_test_vectors[idx].mode,
                                        aes_xts_test_vectors[idx].pt,
                                        aes_xts_test_vectors[idx].pt_len,
                                        aes_xts_test_vectors[idx].key,
                                        aes_xts_test_vectors[idx].key_len,
                                        aes_xts_test_vectors[idx].xts_key,
                                        aes_xts_test_vectors[idx].xts_key_len,
                                        aes_xts_test_vectors[idx].xts_du_size,
                                        aes_xts_test_vectors[idx].iv,
                                        aes_xts_test_vectors[idx].ct, 1);

    if (status == E_SUCCESS)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto AES XTS Functional Test - Vect Num:%u Passed", idx);
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto AES XTS Functional Test - Vect Num:%u Failed", idx);
      return status;
    }
  } 

  return status;
}

/**
  @brief 
    Sample code for AES CCM functional tests
*/
int tz_app_aes_ccm_func(void)
{
 int status = E_SUCCESS;
#if 0
  int idx=0;
  int num=0;
  uint32 plain_text_len = sizeof(aes_large_plain_text2);

  num = sizeof(aes_ccm_test_vectors) / sizeof(aes_ccm_vector_type);

  /*---------------------------------------------------------------
  Run through AES 126 & 256 XTS test vectors
  -------------------------------------------------------------------*/
  for (idx = 0; idx < num; idx++)
  {
    status = tz_app_crypto_aes_ccm_func(aes_ccm_test_vectors[idx].alg,
                                        aes_ccm_test_vectors[idx].mode,
                                        aes_ccm_test_vectors[idx].pt,
                                        aes_ccm_test_vectors[idx].pt_len,
                                        aes_ccm_test_vectors[idx].key,
                                        aes_ccm_test_vectors[idx].key_len,
                                        aes_ccm_test_vectors[idx].iv,
                                        aes_ccm_test_vectors[idx].nonce,
                                        aes_ccm_test_vectors[idx].nonce_len,
                                        aes_ccm_test_vectors[idx].payload_len,
                                        aes_ccm_test_vectors[idx].ct,
                                        aes_ccm_test_vectors[idx].ct_len,
                                        aes_ccm_test_vectors[idx].ct2,
                                        aes_ccm_test_vectors[idx].header_len,
                                        aes_ccm_test_vectors[idx].sw_verify);

    if (status == E_SUCCESS)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto AES CCM Functional Test - Vect Num:%u Passed", idx);
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto AES CCM Functional Test - Vect Num:%u Failed", idx);
      return status;
    }
  } 

  //Test AES CCM with large vectors
  for (idx = 16; idx <= plain_text_len; idx *= 2) 
  { 
    //Set the length
    aes_ccm_test_vectors_1.pt_len      = idx;
    aes_ccm_test_vectors_1.payload_len = idx;
    aes_ccm_test_vectors_1.ct_len      = idx + 8;

    status = tz_app_crypto_aes_ccm_func(aes_ccm_test_vectors_1.alg,
                                        aes_ccm_test_vectors_1.mode,
                                        aes_ccm_test_vectors_1.pt,
                                        aes_ccm_test_vectors_1.pt_len,
                                        aes_ccm_test_vectors_1.key,
                                        aes_ccm_test_vectors_1.key_len,
                                        aes_ccm_test_vectors_1.iv,
                                        aes_ccm_test_vectors_1.nonce,
                                        aes_ccm_test_vectors_1.nonce_len,
                                        aes_ccm_test_vectors_1.payload_len,
                                        aes_ccm_test_vectors_1.ct,
                                        aes_ccm_test_vectors_1.ct_len,
                                        aes_ccm_test_vectors_1.ct2,
                                        aes_ccm_test_vectors_1.header_len,
                                        aes_ccm_test_vectors_1.sw_verify);
    if (status == E_SUCCESS)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto AES CCM Large Vector Functional Test - Vect Size:%u Passed", idx);
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto AES CCM Large Vector Functional Test - Vect Size:%u Failed", idx);
      return status;
    }
  }
#endif

  QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto AES CCM : Test case commented out", 0);
  QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto AES CCM : AES CCM mode is not supported in hardware driver", 0);
  return status;
}

/**
  @brief 
    Sample code for HASH functional tests
*/
int tz_app_hash_func(void)
{
  int idx=0;
  int num=0;
  int status = E_SUCCESS;
  num = sizeof(sha_test_vectors) / sizeof(sha_vector_type_t);

  /*---------------------------------------------------------------
  Run through HASH test vectors
  -------------------------------------------------------------------*/
  for (idx = 0; idx < num; idx++)
  {
    status = tz_app_crypto_hash_func(sha_test_vectors[idx].alg,
                                     sha_test_vectors[idx].msg,
                                     sha_test_vectors[idx].msg_len,
                                     sha_test_vectors[idx].digest);

    if (status == E_SUCCESS)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto Hash Functional Test - Vect Num:%u Passed", idx);
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto Hash Functional Test - Vect Num:%u Failed", idx);
      return status;
    }
  }
  
  /*---------------------------------------------------------------
  Run HASH multiple block test vector
  -------------------------------------------------------------------*/ 
  memset(aes_large_plain_text_1, 0xed, sizeof(aes_large_plain_text_1));
  
  status = tz_app_crypto_hash_block_func(QSEE_HASH_SHA1,
                                         aes_large_plain_text_1,
                                         sizeof(aes_large_plain_text_1));

  if (status == E_SUCCESS)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto Hash Functional Block Test Passed");
  }
  else
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto Hash Functional Block Test Failed");
  }

  return status;
}


/*
	Sample code for HMAC functional tests
*/
int tz_app_hmac_func(void)
{
  int idx=0;
  int num=0;
  int status = E_SUCCESS;
  num = sizeof(sha_hmac_test_vectors) / sizeof(sha_hmac_vector_type_t);

  /*---------------------------------------------------------------
  Run through HMAC test vectors
  -------------------------------------------------------------------*/
  for (idx = 0; idx < num; idx++)
  {
    status = tz_app_crypto_hmac_func(sha_hmac_test_vectors[idx].msg,
                                     sha_hmac_test_vectors[idx].msg_len,
                                     sha_hmac_test_vectors[idx].hmac_key,
                                     sha_hmac_test_vectors[idx].hmac_key_len,
                                     sha_hmac_test_vectors[idx].msg_hmac1,
                                     sha_hmac_test_vectors[idx].msg_hmac256);

    if (status == E_SUCCESS)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto Hmac Functional Test - Vect Num:%u Passed", idx);
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto Hmac Functional Test - Vect Num:%u Failed", idx);
    }
  }
  
  return status;    
}

/**
  @brief 
    Sample code for CMAC functional tests
*/
int tz_app_cmac_func(void)
{
  int idx=0;
  int num=0;
  int status = E_SUCCESS;
  num = sizeof(aes_cmac_test_vectors) / sizeof(aes_cmac_vector_t);

  /*---------------------------------------------------------------
  Run through CMAC test vectors
  -------------------------------------------------------------------*/
  for (idx = 0; idx < num; idx++)
  {
    status = tz_app_crypto_cmac_func(aes_cmac_test_vectors[idx].msg,
                                     aes_cmac_test_vectors[idx].msg_len,
                                     aes_cmac_test_vectors[idx].key,
                                     aes_cmac_test_vectors[idx].key_len,
                                     aes_cmac_test_vectors[idx].cmac,
                                     aes_cmac_test_vectors[idx].cmac_len);

    if (status == E_SUCCESS)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto CMAC Functional Test - Vect Num:%u Passed", idx);
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto CMAC Functional Test - Vect Num:%u Failed", idx);
    }
  }
  
  return status;    
}

/**
  @brief 
    Sample code for AES performance tests
*/
int tz_app_aes_perf(void)
{
  uint32 size;
  uint32 num  = TEST_CRYPTO_PERF_MAX_ITERATIONS;
  uint32 i = 0;
  int    tput;
  int    status = E_SUCCESS;
  unsigned long long total_time = 0;
  total_time = qsee_get_uptime() - total_time;
  QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for turning on clocks: %u ms", total_time);

  for (size = TZBSP_TEST_CRYPTO_PERF_START_SIZE; size <= TZBSP_TEST_CRYPTO_PERF_END_SIZE; size = size*2)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Performance AES128 Test - AES Algo: ECB Data Size: %u", size);

    total_time = qsee_get_uptime();

    for (i = 0; i < num; i++)
    {
      status = tz_app_crypto_aes_perf(QSEE_CIPHER_ALGO_AES_128,
                                      QSEE_CIPHER_MODE_ECB,
                                      aes_performance,
                                      size,
                                      aes_key_1,
                                      sizeof(aes_key_1),
                                      NULL);
      if (status != E_SUCCESS) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"tz_app_crypto_aes_perf test FAILED for iteration: %u", i);
        return status;
      }
    }
  
    total_time = qsee_get_uptime() - total_time;

    /*
     * average_time_in_milli_sec = total_time/num_of_iterations
     * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
     * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
     */
    tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);

    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Performance AES128 Test - AES Algo: CBC Data Size: %u", size);

    total_time = qsee_get_uptime();

    for (i = 0; i < num; i++)
    {
      status = tz_app_crypto_aes_perf(QSEE_CIPHER_ALGO_AES_128,
                                      QSEE_CIPHER_MODE_CBC,
                                      aes_performance,
                                      size,
                                      aes_key_2,
                                      sizeof(aes_key_2),
                                      aes_iv_2);
      if (status != E_SUCCESS) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"tz_app_crypto_aes_perf test FAILED for iteration: %u", i);
        return status;
      }
    }
    
    total_time = qsee_get_uptime() - total_time;
    
    /*
     * average_time_in_milli_sec = total_time/num_of_iterations
     * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
     * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
     */
    tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);

    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Performance AES128 Test - AES Algo: CTR Data Size: %u", size);

    total_time = qsee_get_uptime();

    for (i = 0; i < num; i++)
    {
      status = tz_app_crypto_aes_perf(QSEE_CIPHER_ALGO_AES_128,
                                      QSEE_CIPHER_MODE_CTR,
                                      aes_performance,
                                      size,
                                      aes_key_2,
                                      sizeof(aes_key_2),
                                      aes_iv_2);
      if (status != E_SUCCESS) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"tz_app_crypto_aes_perf test FAILED for iteration: %u", i);
        return status;
      }
    }
  
    total_time = qsee_get_uptime() - total_time;

    /*
     * average_time_in_milli_sec = total_time/num_of_iterations
     * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
     * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
     */
    tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);
    
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Performance AES256 Test - AES Algo: ECB Data Size: %u", size);
    
    total_time = qsee_get_uptime();

    for (i = 0; i < num; i++)
    {
      status = tz_app_crypto_aes_perf(QSEE_CIPHER_ALGO_AES_256,
                                      QSEE_CIPHER_MODE_ECB,
                                      aes_performance,
                                      size,
                                      aes256_key_1,
                                      sizeof(aes256_key_1),
                                      NULL);
      if (status != E_SUCCESS) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"tz_app_crypto_aes_perf test FAILED for iteration: %u", i);
        return status;
      }
    }
  
    total_time = qsee_get_uptime() - total_time;
    
    /*
     * average_time_in_milli_sec = total_time/num_of_iterations
     * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
     * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
     */
    tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);
    
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Performance AES256 Test - AES Algo: CBC Data Size: %u", size);

    total_time = qsee_get_uptime();

    for (i = 0; i < num; i++)
    {
      status = tz_app_crypto_aes_perf(QSEE_CIPHER_ALGO_AES_256,
                                      QSEE_CIPHER_MODE_CBC,
                                      aes_performance,
                                      size,
                                      aes256_key_2,
                                      sizeof(aes256_key_2),
                                      aes256_iv_2);
      if (status != E_SUCCESS) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"tz_app_crypto_aes_perf test FAILED for iteration: %u", i);
        return status;
      }
    }
  
    total_time = qsee_get_uptime() - total_time;

    /*
     * average_time_in_milli_sec = total_time/num_of_iterations
     * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
     * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
     */
    tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);
    
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Performance AES256 Test - AES Algo: CTR Data Size: %u", size);
    
    total_time = qsee_get_uptime();

    for (i = 0; i < num; i++)
    {
      status = tz_app_crypto_aes_perf(QSEE_CIPHER_ALGO_AES_256,
                                      QSEE_CIPHER_MODE_CTR,
                                      aes_performance,
                                      size,
                                      aes256_key_2,
                                      sizeof(aes256_key_2),
                                      aes256_iv_2);
      if (status != E_SUCCESS) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"tz_app_crypto_aes_perf test FAILED for iteration: %u", i);
        return status;
      }
    }
  
    total_time = qsee_get_uptime() - total_time;

    /*
     * average_time_in_milli_sec = total_time/num_of_iterations
     * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
     * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
     */
    tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);
  }

  total_time = qsee_get_uptime() - total_time;
  QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for turning off clocks: %u ms", total_time);
  return status;
}

/**
  @brief 
    Sample code for DES performance tests
*/
int tz_app_des_perf(void)
{
  uint32 size;
  uint32 num  = TEST_CRYPTO_PERF_MAX_ITERATIONS;
  uint32 i = 0;
  int    tput;
  int    status = E_SUCCESS;
  unsigned long long total_time = 0;
  total_time = qsee_get_uptime() - total_time;
  QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for turning on clocks: %u ms", total_time);

  for (size = TZBSP_TEST_CRYPTO_PERF_START_SIZE; size <= TZBSP_TEST_CRYPTO_PERF_END_SIZE; size = size*2)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Performance Triple DES Test - DES Algo: ECB Data Size: %u", size);

    total_time = qsee_get_uptime();

    for (i = 0; i < num; i++)
    {
      status = tz_app_crypto_triple_des_perf(QSEE_CIPHER_ALGO_TRIPLE_DES,
                                             QSEE_CIPHER_MODE_ECB,
                                             aes_performance,
                                             size,
                                             triple_des_key_1_ecb,
                                             NULL);
      if (status != E_SUCCESS) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"tz_app_crypto_triple_des_perf test FAILED for iteration: %u", i);
        return status;
      }
    }
  
    total_time = qsee_get_uptime() - total_time;

    /*
     * average_time_in_milli_sec = total_time/num_of_iterations
     * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
     * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
     */
    tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);

    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Performance Triple DES Test - DES Algo: CBC Data Size: %u", size);

    total_time = qsee_get_uptime();

    for (i = 0; i < num; i++)
    {
      status = tz_app_crypto_triple_des_perf(QSEE_CIPHER_ALGO_TRIPLE_DES,
                                             QSEE_CIPHER_MODE_CBC,
                                             aes_performance,
                                             size,
                                             triple_des_key_1_cbc,
                                             triple_des_iv_1_cbc);
      if (status != E_SUCCESS) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"tz_app_crypto_triple_des_perf test FAILED for iteration: %u", i);
        return status;
      }
    }
    
    total_time = qsee_get_uptime() - total_time;
    
    /*
     * average_time_in_milli_sec = total_time/num_of_iterations
     * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
     * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
     */
    tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput); 
  }

  total_time = qsee_get_uptime() - total_time;
  QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for turning off clocks: %u ms", total_time);
  return status;
}

/**
  @brief 
    Sample code for AES XTS performance tests
*/
int tz_app_aes_xts_perf(void)
{
  uint32 size;
  uint32 num  = TEST_CRYPTO_PERF_MAX_ITERATIONS;
  uint32 i = 0;
  int    tput;
  int    status = E_SUCCESS;
  unsigned long long total_time = 0;
  total_time = qsee_get_uptime() - total_time;
  QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for turning on clocks: %u ms", total_time);

  for (size = TZBSP_TEST_CRYPTO_PERF_START_SIZE; size <= TZBSP_TEST_CRYPTO_PERF_END_SIZE; size = size*2)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Performance AES128 Test - AES Algo: XTS Data Size: %u", size);

    total_time = qsee_get_uptime();

    for (i = 0; i < num; i++)
    {
      status = tz_app_crypto_aes_xts_perf(QSEE_CIPHER_ALGO_AES_128,
                                          QSEE_CIPHER_MODE_XTS,
                                          aes_performance,
                                          size,
                                          aesxts128_key1_1,
                                          sizeof(aesxts128_key1_1),
                                          aesxts128_key2_1,
                                          sizeof(aesxts128_key2_1),
                                          0,
                                          aesxts128_iv_1);
      if (status != E_SUCCESS) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"tz_app_crypto_aes_perf test FAILED for iteration: %u", i);
        return status;
      }
    }
  
    total_time = qsee_get_uptime() - total_time;

    /*
     * average_time_in_milli_sec = total_time/num_of_iterations
     * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
     * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
     */
    tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput); 
    
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Performance AES256 Test - AES Algo: XTS Data Size: %u", size);
    
    total_time = qsee_get_uptime();

    for (i = 0; i < num; i++)
    {
      status = tz_app_crypto_aes_xts_perf(QSEE_CIPHER_ALGO_AES_256,
                                          QSEE_CIPHER_MODE_XTS,
                                          aes_performance,
                                          size,
                                          aesxts256_key1_4,
                                          sizeof(aesxts256_key1_4),
                                          aesxts256_key2_4,
                                          sizeof(aesxts256_key2_4),
                                          0,
                                          aesxts256_iv_4);
      if (status != E_SUCCESS) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"tz_app_crypto_aes_perf test FAILED for iteration: %u", i);
        return status;
      }
    }
  
    total_time = qsee_get_uptime() - total_time;
    
    /*
     * average_time_in_milli_sec = total_time/num_of_iterations
     * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
     * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
     */
    tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);  
  }

  total_time = qsee_get_uptime() - total_time;
  QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for turning off clocks: %u ms", total_time);
  return status;
}

/**
  @brief 
    Sample code for HASH performance tests
*/
int tz_app_hash_perf(void)
{
  uint32 size;
  uint32 num  = TEST_CRYPTO_PERF_MAX_ITERATIONS;
  uint32 i = 0;
  int    tput;
  int    status = E_SUCCESS;
  unsigned long long total_time = 0;
  total_time = qsee_get_uptime() - total_time;
  QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for turning on clocks: %u ms", total_time);

  for (size = TZBSP_TEST_CRYPTO_PERF_START_SIZE; size <= TZBSP_TEST_CRYPTO_PERF_END_SIZE; size = size*2)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Performance HASH Test - HASH Algo: SHA128 Data Size: %u", size);

    total_time = qsee_get_uptime();

    for (i = 0; i < num; i++)
    {
      status = tz_app_crypto_hash_perf(QSEE_HASH_SHA1,
                                       aes_performance,
                                       size);

      if (status != E_SUCCESS) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"tz_app_crypto_hash_perf test FAILED for iteration: %u", i);
        return status;
      }
    }
  
    total_time = qsee_get_uptime() - total_time;

    /*
     * average_time_in_milli_sec = total_time/num_of_iterations
     * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
     * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
     */
    tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);
 
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Performance HASH Test - HASH Algo: SHA256 Data Size: %u", size);

    total_time = qsee_get_uptime();

    for (i = 0; i < num; i++)
    {
      status = tz_app_crypto_hash_perf(QSEE_HASH_SHA256,
                                       aes_performance,
                                       size);

      if (status != E_SUCCESS) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"tz_app_crypto_hash_perf test FAILED for iteration: %u", i);
        return status;
      }
    }
  
    total_time = qsee_get_uptime() - total_time;

    /*
     * average_time_in_milli_sec = total_time/num_of_iterations
     * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
     * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
     */
    tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);
  }

  return status;
}

/**
	Sample code for HMAC performance tests
*/
int tz_app_hmac_perf(void)
{
  uint32 size;
  uint32 num  = TEST_CRYPTO_PERF_MAX_ITERATIONS;
  uint32 i = 0;
  int    tput;
  int    status = E_SUCCESS;
  unsigned long long total_time = 0;
  total_time = qsee_get_uptime() - total_time;
  QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for turning on clocks: %u ms", total_time);

  for (size = TZBSP_TEST_CRYPTO_PERF_START_SIZE; size <= TZBSP_TEST_CRYPTO_PERF_END_SIZE; size = size*2)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Performance HMAC Test - HMAC Algo: SHA128 Data Size: %u", size);

    total_time = qsee_get_uptime();

    for (i = 0; i < num; i++)
    {
      status = tz_app_crypto_hmac_perf(QSEE_HMAC_SHA1,
                                       aes_performance,
                                       size,
                                       sha_hmac_key_1,
                                       sizeof(sha_hmac_key_1));

      if (status != E_SUCCESS) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"tz_app_crypto_hmac_perf test FAILED for iteration: %u", i);
        return status;
      }
    }
  
    total_time = qsee_get_uptime() - total_time;

    /*
     * average_time_in_milli_sec = total_time/num_of_iterations
     * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
     * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
     */
    tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);

    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Performance HMAC Test - HMAC Algo: SHA256 Data Size: %u", size);

    total_time = qsee_get_uptime();

    for (i = 0; i < num; i++)
    {
      status = tz_app_crypto_hmac_perf(QSEE_HMAC_SHA256,
                                       aes_performance,
                                       size,
                                       sha_hmac_key_1,
                                       sizeof(sha_hmac_key_1));

      if (status != E_SUCCESS) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"tz_app_crypto_hmac_perf test FAILED for iteration: %u", i);
        return status;
      }
    }
  
    total_time = qsee_get_uptime() - total_time;

    /*
     * average_time_in_milli_sec = total_time/num_of_iterations
     * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
     * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
     */
    tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);
  }
  total_time = qsee_get_uptime() - total_time;
  QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for turning off clocks: %u ms", total_time);
  return status;
}

/**
  @brief 
    Sample code for CMAC performance tests
*/
int tz_app_cmac_perf(void)
{
  uint32 size;
  uint32 num  = TEST_CRYPTO_PERF_MAX_ITERATIONS;
  uint32 i = 0;
  int    tput;
  int    status = E_SUCCESS;
  unsigned long long total_time = 0;
  total_time = qsee_get_uptime() - total_time;
  QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for turning on clocks: %u ms", total_time);

  for (size = TZBSP_TEST_CRYPTO_PERF_START_SIZE; size <= TZBSP_TEST_CRYPTO_PERF_END_SIZE; size = size*2)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Performance CMAC Test - CMAC Algo: AES128 Data Size: %u", size);

    total_time = qsee_get_uptime();

    for (i = 0; i < num; i++)
    {
      status = tz_app_crypto_cmac_perf(QSEE_CMAC_ALGO_AES_128,
                                       aes_performance,
                                       size,
                                       aescmac128_key_1,
                                       sizeof(aescmac128_key_1));

      if (status != E_SUCCESS) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"tz_app_crypto_cmac_perf test FAILED for iteration: %u", i);
        return status;
      }
    }
  
    total_time = qsee_get_uptime() - total_time;

    /*
     * average_time_in_milli_sec = total_time/num_of_iterations
     * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
     * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
     */
    tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);

    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Performance CMAC Test - CMAC Algo: AES256 Data Size: %u", size);

    total_time = qsee_get_uptime();

    for (i = 0; i < num; i++)
    {
      status = tz_app_crypto_cmac_perf(QSEE_CMAC_ALGO_AES_256,
                                       aes_performance,
                                       size,
                                       aescmac256_key_1,
                                       sizeof(aescmac256_key_1));

      if (status != E_SUCCESS) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"tz_app_crypto_cmac_perf test FAILED for iteration: %u", i);
        return status;
      }
    }
  
    total_time = qsee_get_uptime() - total_time;

    /*
     * average_time_in_milli_sec = total_time/num_of_iterations
     * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
     * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
     */
    tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);
  }
  total_time = qsee_get_uptime() - total_time;
  QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for turning off clocks: %u ms", total_time);
  return status;
}

/**
  @brief 
    Sample code for KDF performance tests
*/
int tz_app_kdf_perf(void)
{
  uint32 size;
  uint32 num  = TEST_CRYPTO_PERF_MAX_ITERATIONS;
  uint32 i = 0;
  int    tput;
  int    status = E_SUCCESS;
  unsigned long long total_time = 0;
  uint8 *hmac_key = NULL;
  uint32 key_sizes[] = {16,32};
  uint32 data_set = sizeof(key_sizes)/4;
  static char key_label[] = {"I am deriving a key"};
  static char key_context[] = {"To verify the kdf working"};
  uint8 key_input[] = {0x12,0x23,0x56,0x73,
                       0xA3,0xff,0xEf,0xAB,
                       0xCD,0x1F,0x56,0xEF,
                       0xA3,0xff,0xEf,0xAB,
                       0xCD,0x1F,0x56,0xEF,
                       0x12,0x23,0x56,0x73,
                       0xCD,0x1F,0x56,0xEF,
                       0xCD,0x1F,0x56,0xEF,
                       0x12,0x23,0x56,0x73,
                       0xCD,0x1F,0x56,0xEF,
                       0xCD,0x1F,0x56,0xEF,
                       0xA3,0xff,0xEf,0xAB};
  total_time = qsee_get_uptime() - total_time;
  QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for turning on clocks: %u ms", total_time);

  for(size = 0; size < data_set; size++)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Performance KDF Test, NULL key - Data Size: %u", key_sizes[size]);

    hmac_key = qsee_malloc(key_sizes[size]);
    if(NULL == hmac_key)
    {
      status = E_NO_MEMORY;
      return status;
    }

    total_time = qsee_get_uptime();

    for (i = 0; i < num; i++)
    {
      status = qsee_kdf(NULL, key_sizes[size],                           
                        (void*)(&key_label), strlen(key_label),
                        (void*)(&key_context), strlen(key_context),
                        hmac_key, key_sizes[size]);
      if (status != E_SUCCESS) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"tz_app_crypto_kdf_perf test FAILED for iteration for NULL key: %u", i);
        qsee_free(hmac_key);
        return status;
      }
    }
  
    total_time = qsee_get_uptime() - total_time;

    /*
     * average_time_in_milli_sec = total_time/num_of_iterations
     * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
     * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
     */
    tput = (((float)key_sizes[size] / ((float)total_time / (float)num)) * 1000.00);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Throughput: %u Bps", tput);
 
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Performance KDF Test, user input key - Data Size: %u", key_sizes[size]);

    total_time = qsee_get_uptime();

    for (i = 0; i < num; i++)
    {
      status = qsee_kdf((void*)key_input, key_sizes[size],                           
                        (void*)(&key_label), strlen(key_label),
                        (void*)(&key_context), strlen(key_context),
                        hmac_key, key_sizes[size]);
      if (status != E_SUCCESS) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"tz_app_crypto_kdf_perf test FAILED for iteration for user input key: %u", i);
        qsee_free(hmac_key);
        return status;
      }
    }
  
    total_time = qsee_get_uptime() - total_time;

    /*
     * average_time_in_milli_sec = total_time/num_of_iterations
     * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
     * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
     */
    tput = (((float)key_sizes[size] / ((float)total_time / (float)num)) * 1000.00);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Throughput: %u Bps", tput);
    qsee_free(hmac_key);
  }

  total_time = qsee_get_uptime() - total_time;
  QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for turning off clocks: %u ms", total_time);
  return status;
}

/**
 * @brief 
 *  Sample code for KDF 
 */

int tz_app_crypto_kdf(void)
{
  uint32 status = E_FAILURE;
  uint8 *derived_key = NULL, *derived_key_ver = NULL;
  uint32 i = 0;
  uint32 key_sizes[] = {16,32};
  uint32 num = sizeof(key_sizes)/4;
  static char key_label[] = {"I am deriving a key"};
  static char key_context[] = {"To verify the kdf working"};
  uint8 key_input[] = {0x12,0x23,0x56,0x73,
                       0xA3,0xff,0xEf,0xAB,
                       0xCD,0x1F,0x56,0xEF,
                       0xA3,0xff,0xEf,0xAB,
                       0xCD,0x1F,0x56,0xEF,
                       0x12,0x23,0x56,0x73,
                       0xCD,0x1F,0x56,0xEF,
                       0xCD,0x1F,0x56,0xEF,
                       0x12,0x23,0x56,0x73,
                       0xCD,0x1F,0x56,0xEF,
                       0xCD,0x1F,0x56,0xEF,
                       0xA3,0xff,0xEf,0xAB};
  uint32 expected_derived_key[2][8] = { {0x9295C5BA, 0xD1524116, 0x25E77AC1, 0x20BE4A59,
                                         0x0       , 0x0       , 0x0       , 0x0       },
                                        {0x132540A1, 0xEA6D8712, 0x54D50521, 0x485544CB,
                                         0x8286E798, 0xD1A2E1DD, 0x52EBEB97, 0x780CDAE9},
                                      };


  for(i = 0; i < num; i++)
  {
    derived_key = qsee_malloc(key_sizes[i]);
    if(NULL == derived_key)
    {
      status = E_NO_MEMORY;
      return status;
    }

    derived_key_ver = qsee_malloc(key_sizes[i]);
    if(NULL == derived_key_ver)
    {
      qsee_free(derived_key_ver);
      status = E_NO_MEMORY;
      return status;
    }

    status = qsee_kdf(NULL, key_sizes[i],                           
                      (void*)(&key_label), strlen(key_label),
                      (void*)(&key_context), strlen(key_context),
                      derived_key, key_sizes[i]);
    if (status != E_SUCCESS) 
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"round1 : create1 KDF test FAILED for iteration: %u", i);
      goto err;
    }

    status = qsee_kdf(NULL, key_sizes[i],                           
                      (void*)(&key_label), strlen(key_label),
                      (void*)(&key_context), strlen(key_context),
                      derived_key_ver, key_sizes[i]);
    if (status != E_SUCCESS) 
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"round1 : create2 KDF test FAILED for iteration: %u", i);
      goto err;
    }

    if(E_SUCCESS != memcmp(derived_key,derived_key_ver,key_sizes[i]))
    {
      status = E_FAILURE;
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"round1 : KDF verify test FAILED for iteration: %u", i);
      goto err;
    }

    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"KDF key\n");
    {
      uint32 j = 0;
      for(j = 0; j < key_sizes[i]; j++)
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"0x%x,",derived_key[j]);
      }
		}
    if(E_SUCCESS != memcmp(expected_derived_key[i], derived_key, key_sizes[i]))
		{
      status = E_FAILURE;
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"\nFuses are blown, key is not same as unblown device");
      goto err;
    }


    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"KDF round1 passed for iteration: %u", i);

    qsee_free(derived_key);
    qsee_free(derived_key_ver);
  }


  for(i = 0; i < num; i++)
  {
    derived_key = qsee_malloc(key_sizes[i]);
    if(NULL == derived_key)
    {
      status = E_NO_MEMORY;
      return status;
    }

    derived_key_ver = qsee_malloc(key_sizes[i]);
    if(NULL == derived_key_ver)
    {
      qsee_free(derived_key_ver);
      status = E_NO_MEMORY;
      return status;
    }

    status = qsee_kdf((void*)key_input, key_sizes[i],                           
                      (void*)(&key_label), strlen(key_label),
                      (void*)(&key_context), strlen(key_context),
                      derived_key, key_sizes[i]);
    if (status != E_SUCCESS) 
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"round2 : create1 KDF test FAILED for iteration: %u", i);
      goto err;
    }

    status = qsee_kdf((void*)key_input, key_sizes[i],                           
                      (void*)(&key_label), strlen(key_label),
                      (void*)(&key_context), strlen(key_context),
                      derived_key_ver, key_sizes[i]);
    if (status != E_SUCCESS) 
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"round2 : create2 KDF test FAILED for iteration: %u", i);
      goto err;
    }

    if(E_SUCCESS != memcmp(derived_key,derived_key_ver,key_sizes[i]))
    {
      status = E_FAILURE;
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"round2 : KDF verify test FAILED for iteration: %u", i);
      goto err;
    }

    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"KDF round2 passed for iteration: %u", i);

    qsee_free(derived_key);
    qsee_free(derived_key_ver);
  }
  return status;
err:
  qsee_free(derived_key);
  qsee_free(derived_key_ver);
  return status;
}


/**
  @brief 
    Sample code for how to call ECDH crypto API's
*/
int tz_app_crypto_ecdh_func(boolean injecterror)
{
   uint32 j;
   boolean error_occurred = FALSE;
   int rv;
   QSEE_bigval_t alicePrivate, bobPrivate;
   QSEE_affine_point_t alicePublic, bobPublic, aliceFinal, bobFinal;


   rv = qsee_SW_ECC_PubPrivate_Key_generate(&alicePublic, &alicePrivate );
   if (rv) {
      return -E_FAILURE;
   }
   
   rv = qsee_SW_ECC_PubPrivate_Key_generate(&bobPublic, &bobPrivate);
   if (rv) {
      return -E_FAILURE;
   }

   rv = qsee_SW_ECDH_Shared_Key_Derive (&aliceFinal, &alicePrivate, &bobPublic);
   if (rv) {
      return -E_FAILURE;
   }

   if (injecterror) {
      if (qsee_toggle_random_bit(&bobPrivate, sizeof(bobPrivate) - sizeof(int))) {
        return -E_FAILURE;
      }
   }

   rv = qsee_SW_ECDH_Shared_Key_Derive(&bobFinal, &bobPrivate, &alicePublic);
   if (rv) {
      return -E_FAILURE;
   }
   
   if (!qsee_in_curveP(&aliceFinal)) {
      return -E_FAILURE;
   }
   if (!qsee_in_curveP(&bobFinal)) {
      return -E_FAILURE;
   }
   
   for (j = 0; j < sizeof (QSEE_affine_point_t) / sizeof (int); ++j) {
      if (((int *)&aliceFinal)[j] != ((int *)&bobFinal)[j]) {
        error_occurred = TRUE;
      }
   }

   if (injecterror) {
      if (!error_occurred) {
        return -E_FAILURE;
      }      
   } else {
      if (error_occurred) {
        return -E_FAILURE;
      }
   }

   return E_SUCCESS;
}

/**
  @brief 
    Sample code for how to call ECDSA crypto API's
*/
int tz_app_crypto_ecdsa_func(boolean injecterror, int dgstlen)
{
   uint8 *dgst;
   int rv;
   int rv_2;
   QSEE_affine_point_t dmjpubkey;
   QSEE_bigval_t       msgdgst;
   QSEE_bigval_t       dmjprivkey;
   QSEE_ECDSA_sig_t    dmjsig;

   rv = qsee_SW_ECC_PubPrivate_Key_generate(&dmjpubkey, &dmjprivkey);

   if (rv) {
      return -E_FAILURE;
   }
   
   /* generate message hash */
   dgst = (uint8 *)qsee_malloc(dgstlen);

   if (dgst == NULL) {
      return -E_NO_MEMORY;
   }
   
   rv_2 = qsee_get_random_bytes(dgst, dgstlen);
   if (rv_2 != 0) {
      return -E_FAILURE;
   }
      
   qsee_ECC_hash_to_bigval(&msgdgst, dgst, dgstlen);

   rv = qsee_SW_ECDSA_Sign(&msgdgst, &dmjprivkey, &dmjsig);   
   if (rv) {
     return -E_FAILURE;
  }

   if (injecterror) {
      qsee_toggle_random_bit(dgst, MIN(dgstlen, 4*(QSEE_BIGLEN-1)-1));
      qsee_ECC_hash_to_bigval(&msgdgst, dgst, dgstlen);
   }

   rv = qsee_SW_ECDSA_Verify(&msgdgst, &dmjpubkey, &dmjsig);   

   if ((rv == -1)^injecterror) {
     if (!injecterror) {
       return -E_FAILURE;
     } else {
       return -E_FAILURE;
     }
   }

   qsee_free(dgst);

   return E_SUCCESS;
}

/**
  @brief
    Sample code for how to call Generic ECDSA crypto API's
*/
int tz_app_crypto_generic_ecdsa_func(generic_ecc_vector_type *data_ptr)
{
  QSEE_qrlbn_ecc_affine_point_t  public_key;
  QSEE_qrlbn_ecc_bigval_t        private_key;
  QSEE_qrlbn_ECDSA_sig_t         signature_data;
  QSEE_qrlbn_ecc_domain_t        *domain = NULL;
  int                            rv  = 0;
  int                            rv2 = 0;
  int                            rv3 = 0;
  uint32                         temp; 
  uint32                         i;
  uint32                         t;
  generic_ecc_vector_type        *binary_to_compare = NULL;

  QSEE_LOG(QSEE_LOG_MSG_DEBUG, "ECDSA with keylen: %u", data_ptr->key_size);

  do
  {

    domain = (QSEE_qrlbn_ecc_domain_t *)qsee_malloc(sizeof(QSEE_qrlbn_ecc_domain_t));
    if (domain == NULL)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "domain malloc failed");
      rv = -E_FAILURE;
      break;
    }

    //Init ECC curve parameters
    rv = qsee_SW_GENERIC_ECC_init(domain, data_ptr->m, data_ptr->a, data_ptr->b, data_ptr->G_x, data_ptr->G_y, data_ptr->n, 1);
    if (rv) 
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_GENERIC_ECC_init API failed %d", rv);
      rv = -E_FAILURE;
      break;
    }

    // Validate what is in the domain
    if (QRLBN_ECC_MODULUS_CHECK_FAIL(&domain->base_point_order))
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_SW_GENERIC_ECC_init returned domain failed validation");
      rv = -E_FAILURE;
      break;
    }
   
    //Convert public_key_x binary data to public_key.x Bigval
    rv = qsee_SW_GENERIC_ECC_binary_to_bigval(&public_key.x, data_ptr->public_key_x, (data_ptr->key_size + 7) / 8, 
                                              domain, QSEE_qrlbn_tag_X);
    if (rv)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_GENERIC_ECC_Binary_to_Bigval API failed on x");
      rv = -E_FAILURE;
      break;
    }

    //Convert public_key_y binary binary to public_key.y Bigval
    rv = qsee_SW_GENERIC_ECC_binary_to_bigval(&public_key.y, data_ptr->public_key_y, (data_ptr->key_size + 7) / 8, domain, QSEE_qrlbn_tag_Y);
    if (rv) 
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_GENERIC_ECC_Binary_to_Bigval API failed on y");
      rv = -E_FAILURE;
      break;
    }

    binary_to_compare = (generic_ecc_vector_type *) qsee_malloc(sizeof(generic_ecc_vector_type));
    if (binary_to_compare == NULL)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_SW_GENERIC_ECC_bigval_to_binary failed to malloc test comparison");
      rv = -E_FAILURE;
      break;
    }

    binary_to_compare->key_size = data_ptr->key_size;
    binary_to_compare->public_key_y = qsee_malloc((binary_to_compare->key_size + 7) / 8);
    if (binary_to_compare->public_key_y == NULL)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_SW_GENERIC_ECC_bigval_to_binary failed to malloc test comparison y");
      rv = -E_FAILURE;
      break;
    }
    
    // Convert Bigval back to binary and compare
    rv = qsee_SW_GENERIC_ECC_bigval_to_binary(binary_to_compare->public_key_y, (binary_to_compare->key_size + 7) / 8, &public_key.y, domain, QSEE_qrlbn_tag_Y);
    if (rv)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_SW_GENERIC_ECC_bigval_to_binary API failed on key y");
      rv = -E_FAILURE;
      break;
    }

    if(binary_to_compare->public_key_y == NULL || memcmp(binary_to_compare->public_key_y, data_ptr->public_key_y, (data_ptr->key_size + 7) / 8))
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_SW_GENERIC_ECC_bigval_to_binary API failed on key y binary comparison");
      rv = -E_FAILURE;
      break;
    }

    //Convert signature binary data to signature_data.r Bigval. Signature r in first half of signature input data
    rv = qsee_SW_GENERIC_ECC_binary_to_bigval(&signature_data.r, data_ptr->signature, data_ptr->sig_size / 2, domain, QSEE_qrlbn_tag_r);
    if (rv) 
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_GENERIC_ECC_Binary_to_Bigval API failed first half");
      rv = -E_FAILURE;
      break;
    }

    //Convert signature binary data to signature_data.s Bigval. Signature s in second half of signature input data
    rv = qsee_SW_GENERIC_ECC_binary_to_bigval(&signature_data.s, data_ptr->signature + (data_ptr->sig_size / 2), 
                                              data_ptr->sig_size / 2, domain, QSEE_qrlbn_tag_s);
    if (rv) 
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_GENERIC_ECC_Binary_to_Bigval API failed second half");
      rv = -E_FAILURE;
      break;
    }

    //Execute verify routine and verify signature
    rv = qsee_SW_GENERIC_ECDSA_verify(data_ptr->hash, data_ptr->hash_size, &public_key, &signature_data, domain);
    if (rv) 
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_GENERIC_ECDSA_Verify API failed to verify vector signature");
      rv = -E_FAILURE;
      break;
    }

    // Now twiddle one bit in the hash value. This should cause a failure.
    temp = data_ptr->hash[0];
    data_ptr->hash[0] ^= 1; 
    
    //Verify vector data again. This time is should fail
    rv2 = qsee_SW_GENERIC_ECDSA_verify(data_ptr->hash, data_ptr->hash_size, &public_key, &signature_data, domain);
    if (!rv2) 
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_GENERIC_ECDSA_Verify API passed where it should have failed");
      rv = -E_FAILURE;
      break;
    }

    //Restore hash data
    data_ptr->hash[0] = temp;

    //Convert private_key binary data to private.key.data Bigval
    rv = qsee_SW_GENERIC_ECC_binary_to_bigval(&private_key, data_ptr->private_key, (data_ptr->key_size + 7) / 8, 
                                              domain, QSEE_qrlbn_tag_privkey);
    if (rv) 
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_GENERIC_ECC_Binary_to_Bigval API failed");
      rv = -E_FAILURE;
      break;
    }

    //Generate signature
    rv = qsee_SW_GENERIC_ECDSA_sign(data_ptr->hash, data_ptr->hash_size, &private_key, &signature_data, domain); 
    if (rv) 
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_GENERIC_ECDSA_Sign API failed to generate signature");
      rv = -E_FAILURE;
      break;
    }

    //Now verify the previous generated signature
    rv = qsee_SW_GENERIC_ECDSA_verify(data_ptr->hash, data_ptr->hash_size, &public_key, &signature_data, domain);
    if (rv) 
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_GENERIC_ECDSA_Verify API failed to verify signature");
      rv = -E_FAILURE;
      break;
    }

    // Now twiddle one bit in the hash value.
    data_ptr->hash[0] ^= 1;

    //Verify the previous signnature again. This time it should fail
    rv2 = qsee_SW_GENERIC_ECDSA_verify(data_ptr->hash, data_ptr->hash_size, &public_key, &signature_data, domain);
    if (!rv2) 
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_GENERIC_ECDSA_Verify API passed when it should have failed to verify signature");
      rv = -E_FAILURE;
      break;
    }

    //Restore hash data
    data_ptr->hash[0] = temp;
    
    //Run ECC Random Hash Vector Test 
    //Loop over number of test
    for (i=0; i < 10; i++)
    { 
      //Generate random hash -- using current hash size to mix it up each run -- make sure buffer is big enough
      rv3 = qsee_get_random_bytes(hash_binary, data_ptr->hash_size);
      if (rv3) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_get_random_bytes API failed pass %u", i);
        rv = -E_FAILURE;
        break;
      }

      //Generate Public/Private key pair
      rv = qsee_SW_GENERIC_ECC_keypair_generate(&private_key, &public_key, domain);
      if (rv) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_GENERIC_ECC_PubPrivate_Key_generate API failed pass %u", i);
        rv = -E_FAILURE;
        break;
      }

      //Generate signature using Random hash vecotr
      rv = qsee_SW_GENERIC_ECDSA_sign(hash_binary, data_ptr->hash_size, &private_key, &signature_data, domain); 
      if (rv) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_GENERIC_ECDSA_Sign API failed pass %u", i);
        rv = -E_FAILURE;
        break;
      }

      //Now verify the previous generated signature
      rv = qsee_SW_GENERIC_ECDSA_verify(hash_binary, data_ptr->hash_size, &public_key, &signature_data, domain);
      if (rv) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_GENERIC_ECDSA_Verify API failed pass %u", i);
        rv = -E_FAILURE;
        break;
      }

      // Now twiddle one bit in the hash value.
      temp = hash_binary[0];
      hash_binary[0] ^= 1;

      //Now verify the previous generated signature. This time it should fail
      rv2 = qsee_SW_GENERIC_ECDSA_verify(hash_binary, data_ptr->hash_size, &public_key, &signature_data, domain);
      if (!rv2) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_GENERIC_ECDSA_Verify API failed pass %u", i);
        rv = -E_FAILURE;
        break;
      }

      // twiddle back
      hash_binary[0] = temp;
      for(t = 0; t < QSEE_ECC_BIGNUM_WORDS; ++t) {
        signature_data.r.data[t] = 0x00;
      }
      // Now mess with the signature
      // Now verify the previous generated signature. This time it should fail
      rv2 = qsee_SW_GENERIC_ECDSA_verify(hash_binary, data_ptr->hash_size, &public_key, &signature_data, domain);
      if (!rv2) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_GENERIC_ECDSA_Verify API failed pass %u", i);
        rv = -E_FAILURE;
        break;
      }
    }
  } while(0);

  if (binary_to_compare)
  {
    if (binary_to_compare->public_key_y)
    {
      qsee_free(binary_to_compare->public_key_y);
    }

    qsee_free(binary_to_compare);
  }
  if (domain)
  {
    qsee_free(domain);
  }
   
  return rv;
}

/**
  @brief 
    Sample code for how to call Generic ECDH crypto API's
*/
int tz_app_crypto_generic_ecdh_func(generic_ecc_vector_type *data_ptr)
{
  QSEE_qrlbn_ecc_domain_t        domain;
  QSEE_qrlbn_ecc_affine_point_t  bob_public_key, alice_public_key;
  QSEE_qrlbn_ecc_bigval_t        bob_private_key, alice_private_key;
  QSEE_qrlbn_ecc_bigval_t        bob_shared_key, alice_shared_key;
  int                            rv  = 0;

  //Init ECC curve paramters
  rv = qsee_SW_GENERIC_ECC_init(&domain, data_ptr->m, data_ptr->a, data_ptr->b, data_ptr->G_x, data_ptr->G_y, data_ptr->n, 1);
  if (rv) 
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_GENERIC_ECC_init API failed");
    return -E_FAILURE;
  }

  //Generate Public/Private key pair b
  rv = qsee_SW_GENERIC_ECC_keypair_generate(&bob_private_key, &bob_public_key, &domain);
  if (rv) 
  {
     QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_GENERIC_ECC_keypair_generate API failed to generate pair b");
     return -E_FAILURE;
  }

  //Generate Public/Private key pair a
  rv = qsee_SW_GENERIC_ECC_keypair_generate(&alice_private_key, &alice_public_key, &domain);
  if (rv) 
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_GENERIC_ECC_keypair_generate API failed to generate pair a");
    return -E_FAILURE;
  }
 
  rv = qsee_SW_GENERIC_ECDH_shared_key_derive(&alice_shared_key, &alice_private_key, &bob_public_key, &domain);
  if(rv)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_GENERIC_ECDH_shared_key_derive API failed shared a");
    return -E_FAILURE;
  }

  rv = qsee_SW_GENERIC_ECDH_shared_key_derive(&bob_shared_key, &bob_private_key, &alice_public_key, &domain);
  if(rv)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_GENERIC_ECDH_shared_key_derive API failed shared b");
     return -E_FAILURE;
  }

  if(qsee_SW_GENERIC_ECC_compare(alice_shared_key.data, bob_shared_key.data, &((&domain)->modulus.md)) != 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_GENERIC_ECC_compare API failed");
    return -E_FAILURE;
  }

  return rv;

}
/**
	Sample code for ECDH functional tests
*/
int tz_app_ecdh_func(void)
{
  int idx=0;
  int status = E_SUCCESS;

  /*---------------------------------------------------------------
  Run through ECDH test 
  -------------------------------------------------------------------*/
  for (idx = 0; idx < ECC_TEST_COUNT; idx++)
  {
    status = tz_app_crypto_ecdh_func(FALSE);

    if (status == E_SUCCESS)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto ECDH Functional Test - Vect Num:%u Passed", idx);
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto ECDH Functional Test - Vect Num:%u Failed", idx);
      return status;
    }

    status = tz_app_crypto_ecdh_func(TRUE);

    if (status == E_SUCCESS)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto ECDH Functional Test - Vect Num:%u Passed", idx);
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto ECDH Functional Test - Vect Num:%u Failed", idx);
      return status;
    }
  }
  
  return status;
}

/**
  @brief 
    Sample code for ECDH functional tests
*/
int tz_app_ecdsa_func(void)
{
  int status = E_SUCCESS;
  int i = 0;
  int len;

  /*---------------------------------------------------------------
  Run through ECDH test 
  -------------------------------------------------------------------*/
  for (i = 0; i < ECC_TEST_COUNT; i++)
  {

      switch (i % 17) {
      case 1:
        /* test lengths shorter than full size */
        len = sizeof (uint32) * (QSEE_BIGLEN - 1) - i % 5;
        break;
      case 2: 
        /* test lengths longer than full size */
        len = sizeof (uint32) * (QSEE_BIGLEN - 1) + i % 5;
        break;
      case 3: 
        /* test shortest length */
        len = 1;
        break;
      default:
        len = sizeof (uint32) * (QSEE_BIGLEN - 1);
      }

      status = tz_app_crypto_ecdsa_func(FALSE, len);
 
      if (status == E_SUCCESS)
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto ECDSA Functional Test - Vect Num:%u Passed", i);
      }
      else
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto ECDSA Functional Test - Vect Num:%u Failed", i);
        return status;
      }

      status = tz_app_crypto_ecdsa_func(TRUE, len);

      if (status == E_SUCCESS)
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto ECDSA Functional Test - Vect Num:%u Passed", i);
      }
      else
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto ECDSA Functional Test - Vect Num:%u Failed", i);
        return status;
      }

  }
  
  return status;
}

/**
  @brief 
    Sample code for Generic ECDH functional tests
*/
int tz_app_generic_ecdh_func(void)
{
  int status = E_SUCCESS;
  int i = 0;
  int num = 0;

  /*---------------------------------------------------------------
  Run through Generic ECDH test 
  -------------------------------------------------------------------*/
  num = sizeof(generic_ecc_test_vectors) / sizeof(generic_ecc_vector_type); 

  //Set up loop over known ECDSA test vectors and test ECDH
  for (i=0; i < num; i++)
  { 
    status = tz_app_crypto_generic_ecdh_func(generic_ecc_test_vectors_ptr[i]);

    if (status == E_SUCCESS)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto Generic ECDH Functional Test - Vect Num:%u Passed", i);
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto Generic ECDH Functional Test - Vect Num:%u Failed", i);
      return status;
    } 
  } 

  return status;
}

/**
  @brief 
    Sample code for Generic ECDSA functional tests
*/
int tz_app_generic_ecdsa_func(void)
{
  int status = E_SUCCESS;
  int i = 0;
  int num = 0;
  uint32 passed = 0;

  /*---------------------------------------------------------------
  Run through Generic ECDSA test 
  -------------------------------------------------------------------*/
  num = sizeof(generic_ecc_test_vectors) / sizeof(generic_ecc_vector_type); 

  QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto Generic ECDSA Functional Test has %u Vectors", num);

  //Set up loop over known ECDSA test vectors
  for (i=0; i < num; i++)
  { 
    status = tz_app_crypto_generic_ecdsa_func(generic_ecc_test_vectors_ptr[i]);

    if (status == E_SUCCESS)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto Generic ECDSA Functional Test - Vect Num:%u Passed", i);
      ++passed;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto Generic ECDSA Functional Test - Vect Num:%u Failed", i);
    } 
  }

  // 3 known to fail p224, p394 & p521
  if (passed >= (num - 3))
  {
    status = E_SUCCESS;
  } else {
    status = -E_FAILURE;
  }

  return status;
}
/**
	Sample code for how to call QSEE AES software crypto API's
*/
int tz_app_ufcrypto_aes_func(SW_Cipher_Alg_Type alg,
                             SW_CipherModeType mode,
                             uint8 *pt,
                             uint32 pt_len,
                             uint8 *key,
                             uint32 key_len,
                             uint8 *iv,
                             uint8 *ct,
                             uint8 verify)
{
  CryptoCntxHandle *cntx = 0;
  uint8 *ct_tmp = 0;
  uint8 *pt_tmp = 0;
  int status = E_SUCCESS;

  IovecListType   ioVecIn;
  IovecListType   ioVecOut;
  IovecType       IovecIn;
  IovecType       IovecOut;
  SW_CipherEncryptDir dir = SW_CIPHER_ENCRYPT;

  /*----------------------------------------------------------------------
  Allocate temp ct buffer
  ------------------------------------------------------------------------*/
  if (E_SUCCESS == status && (verify == 1) &&
      (ct_tmp = qsee_malloc(pt_len)) == NULL)
  {
    status = -E_FAILURE;
  }

  /*-----------------------------------------------------------------------
   If verify is 0,  it means we are not using the predefined test case and
   cannot verify the encrypted packet
  -------------------------------------------------------------------------*/
  if (verify == 0)
    ct_tmp = ct;

  /*------------------------------------------------------------------
    Init ctx
    ------------------------------------------------------------------*/

  /* Input IOVEC */
  ioVecIn.size = 1;
  ioVecIn.iov = &IovecIn;
  ioVecIn.iov[0].dwLen = pt_len;
  ioVecIn.iov[0].pvBase = pt; 

  /* Output IOVEC */
  ioVecOut.size = 1;
  ioVecOut.iov = &IovecOut;
  ioVecOut.iov[0].dwLen = pt_len;
  ioVecOut.iov[0].pvBase = ct_tmp;

  //Determine algorithm
  if(key_len == 16)
  {
    if(0 != qsee_SW_Cipher_Init(&cntx, SW_CIPHER_ALG_AES128))
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_Init API failed");
      status = -E_FAILURE;
    }
  }
  else
  {
    if(0 != qsee_SW_Cipher_Init(&cntx, SW_CIPHER_ALG_AES256))
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_Init API failed");
      status = -E_FAILURE;
    } 
  }

  /*--------------------------------------------------------------------
  Set key for encryption
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_KEY, key, key_len) != 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set direction to encryption
  ----------------------------------------------------------------------*/

  if (E_SUCCESS == status &&
      qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_DIRECTION, &dir, sizeof(SW_CipherEncryptDir)) != 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_SetParam API failed");
    status = -E_FAILURE;
  }
  /*--------------------------------------------------------------------
  Set AES mode
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_MODE, &mode, sizeof(mode)) != 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_SetParam API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set IV
  ----------------------------------------------------------------------*/
  if(mode != SW_CIPHER_MODE_ECB)
  {
    if(E_SUCCESS == status &&
       qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_IV, iv, SW_AES_IV_SIZE) != 0)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_SetParam API failed");
      status = -E_FAILURE;
    }
  }
  
  /*-----------------------------------------------------------------------
  Clear temp buffer
  ------------------------------------------------------------------------*/
  if ((E_SUCCESS == status) && (verify == 1))
  {
    memset(ct_tmp, 0, pt_len);
  }

  /*-----------------------------------------------------------------------
  Now encrypt the data
  -------------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_SW_CipherData(cntx, ioVecIn, &ioVecOut) != 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_CipherData encrypt API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------------
  If NULL key pointer then we are using HW key so don't compare encrypted result
  -----------------------------------------------------------------------------*/
  if ((E_SUCCESS == status) && (verify == 1) && 0 != memcmp(ct, ct_tmp, pt_len))
  {
    status = -E_FAILURE;
  }

  /*------------------------------------------------------------------------------
  Allocate temp pt buffer
  --------------------------------------------------------------------------------*/
  if ((E_SUCCESS == status) && (verify == 1) && 
      (pt_tmp = qsee_malloc(pt_len)) == NULL)
  {
    status = -E_FAILURE;
  }

  if ((E_SUCCESS == status)&& (verify == 1))
  {
    memset(pt_tmp, 0, pt_len);
  }

  /*-----------------------------------------------------------------------
   If verify is 0,  it means we are not using the predefined test case and
   cannot verify the encrypted packet
  -------------------------------------------------------------------------*/
  if (verify == 0)
    pt_tmp = pt;


  /* Input IOVEC */
  ioVecIn.size = 1;
  ioVecIn.iov = &IovecIn;
  ioVecIn.iov[0].dwLen = pt_len;
  ioVecIn.iov[0].pvBase = ct_tmp;

  /* Output IOVEC */
  ioVecOut.size = 1;
  ioVecOut.iov = &IovecOut;
  ioVecOut.iov[0].dwLen = pt_len;
  ioVecOut.iov[0].pvBase = pt_tmp;

  /*--------------------------------------------------------------------
  Set key for encryption
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_KEY, key, key_len) != 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set direction to decryption
  ----------------------------------------------------------------------*/

  dir = SW_CIPHER_DECRYPT;
  if (E_SUCCESS == status &&
      qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_DIRECTION, &dir, sizeof(SW_CipherEncryptDir)) != 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_SetParam API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set AES mode
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_MODE, &mode, sizeof(mode)) != 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_SetParam API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set IV
  ----------------------------------------------------------------------*/
  if(mode != SW_CIPHER_MODE_ECB)
  {
    if(E_SUCCESS == status &&
       qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_IV, iv, SW_AES_IV_SIZE) != 0)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_SetParam API failed");
      status = -E_FAILURE;
    }
  }

  /*----------------------------------------------------------------------------------
  Now decrypt the data
  ------------------------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_SW_CipherData(cntx, ioVecIn, &ioVecOut) != 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_CipherData decrypt API failed");
    status = -E_FAILURE;
  }

  /*-------------------------------------------------------------------------------------
  Now compare decrypted results
  ---------------------------------------------------------------------------------------*/
  if ((E_SUCCESS == status) && (verify == 1) && 0 != memcmp(pt, pt_tmp, pt_len))
  {  
    status = -E_FAILURE;
  }

  /*-------------------------------------------------------------------------------
  Free ctx
  --------------------------------------------------------------------------------*/
  if (cntx)
  {
    if(key_len == 16) 
    {
      if(0 != qsee_SW_Cipher_DeInit(&cntx, SW_CIPHER_ALG_AES128)) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_DeInit API failed");
        status = -E_FAILURE;
      }
    }
    else
    {
      if(0 != qsee_SW_Cipher_DeInit(&cntx, SW_CIPHER_ALG_AES256)) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_DeInit API failed");
        status = -E_FAILURE;
      } 
    }
    cntx = 0;
  }

  /*--------------------------------------------------------------------------------
  Free malloc data
  ---------------------------------------------------------------------------------*/
  if ((ct_tmp) && (verify == 1))
  {
    qsee_free(ct_tmp);
    ct_tmp = 0;
  }

  if ((pt_tmp)&& (verify == 1))
  {
    qsee_free(pt_tmp);
    pt_tmp = 0;
  }

  return status;
}


/**
  @brief
    Sample code for how to call QSEE AES GCM software crypto
    API's
*/
int tz_app_ufcrypto_aes_gcm_func(uint32 *pt,
                                 uint32 pt_len,
                                 uint8 *key,
                                 uint32 key_len,
                                 uint8 *iv,
                                 uint32 iv_len,
                                 uint32 *ptr_tmp,
                                 uint32 *header_data,
                                 uint32 *auth_struct)
{

  CryptoCntxHandle *cntx = 0;
  int status = E_SUCCESS;

  IovecListType   ioVecIn;
  IovecListType   ioVecOut;
  IovecType       IovecIn;
  IovecType       IovecOut;
  uint8          tag_auth[16];

  uint32 ct_len  = pt_len;
  SW_CipherEncryptDir dir = SW_CIPHER_ENCRYPT;
  SW_CipherModeType mode = SW_CIPHER_MODE_GCM;

  /*------------------------------------------------------------------
    Init ctx
    ------------------------------------------------------------------*/

  /* Input IOVEC */
  ioVecIn.size = 1;
  ioVecIn.iov = &IovecIn;
  ioVecIn.iov[0].dwLen = pt_len;
  ioVecIn.iov[0].pvBase = (void*)pt[0];

  /* Output IOVEC */
  ioVecOut.size = 1;
  ioVecOut.iov = &IovecOut;
  ioVecOut.iov[0].dwLen = ct_len;
  ioVecOut.iov[0].pvBase = (void*)ptr_tmp[1];

  //Determine algorithm
  if(key_len == 16)
  {
    if(0 != qsee_SW_Cipher_Init(&cntx, SW_CIPHER_ALG_AES128))
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_Init API failed");
      status = -E_FAILURE;
    }
  }
  else
  {
    if(0 != qsee_SW_Cipher_Init(&cntx, SW_CIPHER_ALG_AES256))
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_Init API failed");
      status = -E_FAILURE;
    }
  }

  /*--------------------------------------------------------------------
  Set direction to encryption
  ----------------------------------------------------------------------*/

  if (E_SUCCESS == status &&
      qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_DIRECTION, &dir, sizeof(SW_CipherEncryptDir)) != 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_SetParam direction API failed");
    status = -E_FAILURE;
  }
  /*--------------------------------------------------------------------
  Set AES mode
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_MODE, &mode, sizeof(mode)) != 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_SetParam mode API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set key for encryption
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_KEY, key, key_len) != 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm key API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set IV
  ----------------------------------------------------------------------*/
  if(mode != SW_CIPHER_MODE_ECB)
  {
    if(E_SUCCESS == status &&
       qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_IV, iv, iv_len) != 0)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_SetParam iv API failed");
      status = -E_FAILURE;
    }
  }

  /*--------------------------------------------------------------------
  Set AAD if present
  ----------------------------------------------------------------------*/
  if(E_SUCCESS == status &&
     qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_AAD, (void*)header_data[0], header_data[1]) != 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_SetParam AAD API failed");
    status = -E_FAILURE;
  }

  /*-----------------------------------------------------------------------
  Now encrypt the data
  -------------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_SW_CipherData(cntx, ioVecIn, &ioVecOut) != 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_CipherData encrypt API failed");
    status = -E_FAILURE;
  }

  /*-----------------------------------------------------------------------
  Get the auth tag
  -------------------------------------------------------------------------*/
  if(E_SUCCESS == status &&
      0 != qsee_SW_Cipher_GetParam(cntx, SW_CIPHER_PARAM_TAG, tag_auth, 16))
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_GetParam auth tag API failed");
    status = -E_FAILURE;
  }


  if (cntx)
  {
    if(key_len == 16)
    {
      if(0 != qsee_SW_Cipher_DeInit(&cntx, SW_CIPHER_ALG_AES128))
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_DeInit API failed");
        status = -E_FAILURE;
      }
    }
    else
    {
      if(0 != qsee_SW_Cipher_DeInit(&cntx, SW_CIPHER_ALG_AES256))
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_DeInit API failed");
        status = -E_FAILURE;
      }
    }
    cntx = 0;
  }

  /*--------------------------------------------------------------------------
  If NULL key pointer then we are using HW key so don't compare encrypted result
  -----------------------------------------------------------------------------*/
  if ((E_SUCCESS == status))
  {
    if(pt[2] == 0 || pt[3] == 1)
    {
      if(pt[3] == 0)
      {
        //Now compare encrypted results
        if (memcmp((void*)pt[1], (void*)ptr_tmp[1], ct_len) != 0)
        {
          QSEE_LOG(QSEE_LOG_MSG_DEBUG,"encrypted data compare failed");
          status = -E_FAILURE;
        }

        if(memcmp(tag_auth, (void*)auth_struct[0], auth_struct[1]) != 0)
        {
          QSEE_LOG(QSEE_LOG_MSG_DEBUG,"tag compare failed");
          status = -E_FAILURE;
        }
      }

      /* Input IOVEC */
      ioVecIn.size = 1;
      ioVecIn.iov = &IovecIn;
      ioVecIn.iov[0].dwLen = ct_len;
      ioVecIn.iov[0].pvBase = (void*)ptr_tmp[1];

      /* Output IOVEC */
      ioVecOut.size = 1;
      ioVecOut.iov = &IovecOut;
      ioVecOut.iov[0].dwLen = pt_len;
      ioVecOut.iov[0].pvBase = (void*)ptr_tmp[0];


      //Determine algorithm
      if(key_len == 16)
      {
        if(0 != qsee_SW_Cipher_Init(&cntx, SW_CIPHER_ALG_AES128))
        {
          QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_Init API failed");
          status = -E_FAILURE;
        }
      }
      else
      {
        if(0 != qsee_SW_Cipher_Init(&cntx, SW_CIPHER_ALG_AES256))
        {
          QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_Init API failed");
          status = -E_FAILURE;
        }
      }

      /*--------------------------------------------------------------------
    Set direction to decryption
    ----------------------------------------------------------------------*/

      dir = SW_CIPHER_DECRYPT;
      if (E_SUCCESS == status &&
          qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_DIRECTION, &dir, sizeof(SW_CipherEncryptDir)) != 0)
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_SetParam direction API failed");
        status = -E_FAILURE;
      }

      /*--------------------------------------------------------------------
    Set AES mode
    ----------------------------------------------------------------------*/
      if (E_SUCCESS == status &&
          qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_MODE, &mode, sizeof(mode)) != 0)
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_SetParam mode API failed");
        status = -E_FAILURE;
      }

      /*--------------------------------------------------------------------
    Set key for encryption
    ----------------------------------------------------------------------*/
      if (E_SUCCESS == status &&
          qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_KEY, key, key_len) != 0)
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_param key API failed");
        status = -E_FAILURE;
      }

      /*--------------------------------------------------------------------
    Set IV
    ----------------------------------------------------------------------*/
      if(mode != SW_CIPHER_MODE_ECB)
      {
        if(E_SUCCESS == status &&
           qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_IV, iv, iv_len) != 0)
        {
          QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_SetParam iv API failed");
          status = -E_FAILURE;
        }
      }

      /*--------------------------------------------------------------------
      Set AAD
      ----------------------------------------------------------------------*/
      if(E_SUCCESS == status &&
          0 != qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_AAD, (void*)header_data[0], header_data[1]))
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_SetParam AAD API failed");
        status = -E_FAILURE;
      }

      /*--------------------------------------------------------------------
      Set Tag
      ----------------------------------------------------------------------*/
      if(E_SUCCESS == status &&
          0 != qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_TAG, tag_auth, 16))
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_SetParam auth tag API failed");
        status = -E_FAILURE;
      }

      /*----------------------------------------------------------------------------------
        Now decrypt the data
      ------------------------------------------------------------------------------------*/
      if (E_SUCCESS == status &&
          qsee_SW_CipherData(cntx, ioVecIn, &ioVecOut) != 0)
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_CipherData decrypt API failed");
        status = -E_FAILURE;
      }

      /*-------------------------------------------------------------------------------------
    Now compare decrypted results
    ---------------------------------------------------------------------------------------*/
      if ((E_SUCCESS == status))
      {
        //Now compare decrypted results
        if (memcmp((void*)pt[0], (void*)ptr_tmp[0], pt_len) != 0)
        {
          QSEE_LOG(QSEE_LOG_MSG_DEBUG,"decrypted data memcompare failed");
          status = -E_FAILURE;
        }
      }

      /*-------------------------------------------------------------------------------
    Free ctx
    --------------------------------------------------------------------------------*/
      if (cntx)
      {
        if(key_len == 16)
        {
          if(0 != qsee_SW_Cipher_DeInit(&cntx, SW_CIPHER_ALG_AES128))
          {
            QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_DeInit API failed");
            status = -E_FAILURE;
          }
        }
        else
        {
          if(0 != qsee_SW_Cipher_DeInit(&cntx, SW_CIPHER_ALG_AES256))
          {
            QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_DeInit API failed");
            status = -E_FAILURE;
          }
        }
        cntx = 0;
      }
    }
  }

  return status;
}
/**
  @brief
    Sample code for how to call QSEE HASH software crypto API's
*/
int tz_app_ufcrypto_hash_func(SW_Auth_Alg_Type alg,
                              uint8 *msg,
                              uint32 msg_len,
                              uint8 *digest)
{
  int status = E_SUCCESS;
  uint8 result_digest[SW_SHA256_DIGEST_SIZE];
  uint32 digest_len = 0;
  uint32 len;

  IovecListType   ioVecIn;
  IovecListType   ioVecOut;
  IovecType       IovecIn;
  IovecType       IovecOut;

  CryptoCntxHandle *cntx = NULL;

  //Clear result digest
  memset(result_digest, 0, sizeof(result_digest));

  if (alg == SW_AUTH_ALG_SHA1)
    len = SW_SHA1_DIGEST_SIZE;
  else
    len = SW_SHA256_DIGEST_SIZE;

    /* Input IOVEC */
  ioVecIn.size = 1;
  ioVecIn.iov = &IovecIn;
  ioVecIn.iov[0].dwLen = msg_len;
  ioVecIn.iov[0].pvBase = msg; 

  /* Output IOVEC */
  ioVecOut.size = 1;
  ioVecOut.iov = &IovecOut;
  ioVecOut.iov[0].dwLen = len;
  ioVecOut.iov[0].pvBase = result_digest;


  if(0 != qsee_SW_Hash_Init(&cntx, alg))
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_sw_hash_initAPI failed");
    status = -E_FAILURE;
  }
    
  if((E_SUCCESS == status) && (0 != qsee_SW_Hash_Update(cntx, ioVecIn)))
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Hash_Update API failed");
    status = -E_FAILURE;
  }


  if((E_SUCCESS == status) && (0 != qsee_SW_Hash_Final(cntx, &ioVecOut)))
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Hash_Final API failed");
    status = -E_FAILURE;
  }

  //Compare rsults 
  if ((E_SUCCESS == status) && (0 != memcmp(result_digest, digest, digest_len)))
  {
    status = -E_FAILURE;
  }

  return status;
}



/**
  @brief 
    Sample code for how to call QSEE HMAC software crypto API's
*/
int tz_app_ufcrypto_hmac_func(uint8 *msg,
                            uint32 msg_len,
                            uint8* key,
                            uint32 key_len,
                            uint8* hmac1,
                            uint8* hmac256)
{
  int status = E_SUCCESS;
  uint8 result_digest[SW_SHA256_DIGEST_SIZE];

  IovecListType   ioVecIn;
  IovecListType   ioVecOut;
  IovecType       IovecIn;
  IovecType       IovecOut;

  //Clear result digest
  memset(result_digest, 0, sizeof(result_digest));

  /* Input IOVEC */
  ioVecIn.size = 1;
  ioVecIn.iov = &IovecIn;
  ioVecIn.iov[0].dwLen = msg_len;
  ioVecIn.iov[0].pvBase = msg; 


  if(NULL != hmac1)
  {
    /* Output IOVEC */
    ioVecOut.size = 1;
    ioVecOut.iov = &IovecOut;
    ioVecOut.iov[0].dwLen = SW_SHA1_DIGEST_SIZE;
    ioVecOut.iov[0].pvBase = result_digest;

    if(0 != qsee_SW_Hmac(key, key_len, ioVecIn, &ioVecOut, SW_AUTH_ALG_SHA1))
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Hmac API failed");
      status = -E_FAILURE;
    }

    //Compare rsults 
    if ((E_SUCCESS == status) && (0 != memcmp(result_digest, hmac1, QSEE_SHA1_HASH_SZ)))
    {
      status = -E_FAILURE;
    }
  }

  //Clear result digest
  memset(result_digest, 0, sizeof(result_digest));

  //Calculate HASH SHA256
  if (NULL != hmac256)
  {
    /* Output IOVEC */
    ioVecOut.size = 1;
    ioVecOut.iov = &IovecOut;
    ioVecOut.iov[0].dwLen = SW_SHA256_DIGEST_SIZE;
    ioVecOut.iov[0].pvBase = result_digest;

    if ((E_SUCCESS == status) && 
        (0 != qsee_SW_Hmac(key, key_len, ioVecIn, &ioVecOut, SW_AUTH_ALG_SHA256)))
    {  
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Hmac API failed");
      status = -E_FAILURE;
    }

    //Compare rsults 
    if ((E_SUCCESS == status) && (0 != memcmp(result_digest, hmac256, QSEE_SHA256_HASH_SZ)))
    {
      status = -E_FAILURE;
    }
  }

  return status;
}


/**
  @brief 
    Sample code for SW AES functional tests
*/
int tz_app_sw_aes_func(void)
{
  //int idx=0;
  //int num=0;
  int status = E_SUCCESS;

#if NEED_TO_REENABLE
  int idx=0;
  int num=0;
  num = sizeof(sw_aes_test_vectors) / sizeof(uf_aes_vector_type_t);


  /*---------------------------------------------------------------
  Run through AES128/256 ECB/CBC/CTR test vectors
  -------------------------------------------------------------------*/

  {
    status = tz_app_ufcrypto_aes_func(sw_aes_test_vectors[idx].alg,
                                      sw_aes_test_vectors[idx].mode,
                                      sw_aes_test_vectors[idx].pt,
                                      sw_aes_test_vectors[idx].pt_len,
                                      sw_aes_test_vectors[idx].key,
                                      sw_aes_test_vectors[idx].key_len,
                                      sw_aes_test_vectors[idx].iv,
                                      sw_aes_test_vectors[idx].ct, 1);

    if (status == E_SUCCESS)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"SW AES Functional Test - Vect Num:%u Passed", idx);
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"SW AES Functional Test - Vect Num:%u Failed", idx);
      return status;
    }
  }


  /* AES GCM test */
  {
    int i;
    int j;
    uint32 ptr_struct[4] = {0,0,0,0};
    uint32 header_struct[2];
    uint32 tag_struct[2];
    uint32 ptr_tmp_struct[2];
    uint32 key_len = 16;

    ptr_tmp_struct[0] = (uint32)aes_large_plain_text_1;
    ptr_tmp_struct[1] = (uint32)aes_large_cipher_text_1;
    for( j = 0; j < 3; j+=2 )
    {
      key_len = 16 + 8 * j;

      for( i = 0; i < MAX_TESTS; i++ )
      {
        ptr_struct[0] = (uint32)&pt_gcm[pt_index_gcm[i]];
        ptr_struct[1] = (uint32)ct_gcm[j * 6 + i];
        header_struct[0] = (uint32)&additional_gcm[add_index_gcm[i]];
        header_struct[1] = (uint32)add_len_gcm[i];
        tag_struct[0] = (uint32)&tag_gcm[j * 6 + i];
        tag_struct[1] = 16;

        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Crypto AES GCM Vect Test - Vect Num: %u", i);
        status = tz_app_ufcrypto_aes_gcm_func(ptr_struct, pt_len_gcm[i], key_gcm[key_index_gcm[i]], key_len,
                                              iv_gcm[iv_index_gcm[i]], iv_len_gcm[i], ptr_tmp_struct,
                                              header_struct, tag_struct);
        if (status == E_SUCCESS)
        {
          QSEE_LOG(QSEE_LOG_MSG_DEBUG,"SW AES GCM Functional Test - Vect Num:%u Passed", i);
        }
        else
        {
          QSEE_LOG(QSEE_LOG_MSG_DEBUG,"SW AES GCM Functional Test - Vect Num:%u Failed", i);
          return status;
        }
      }
    }
  }
#endif  
  return status;
}


/**
  @brief 
    Sample code for SW HASH functional tests
*/
int tz_app_sw_hash_func(void)
{
  int idx=0;
  int num=0;
  int status = E_SUCCESS;
  num = sizeof(uf_sha_test_vectors) / sizeof(uf_sha_vector_type_t);

  /*---------------------------------------------------------------
  Run through HASH test vectors
  -------------------------------------------------------------------*/
  for (idx = 0; idx < num; idx++)
  {
    status = tz_app_ufcrypto_hash_func(uf_sha_test_vectors[idx].alg,
                                       uf_sha_test_vectors[idx].msg,
                                       uf_sha_test_vectors[idx].msg_len,
                                       uf_sha_test_vectors[idx].digest);

    if (status == E_SUCCESS)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"SW Hash Functional Test - Vect Num:%u Passed", idx);
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"SW Hash Functional Test - Vect Num:%u Failed", idx);
      return status;
    }
  }
  return status;
}


/**
  @brief 
    Sample code for SW HMAC functional tests
*/
int tz_app_sw_hmac_func(void)
{
  int idx=0;
  int num=0;
  int status = E_SUCCESS;
  num = sizeof(sha_hmac_test_vectors) / sizeof(sha_hmac_vector_type_t);

  /*---------------------------------------------------------------
  Run through HMAC test vectors
  -------------------------------------------------------------------*/
  for (idx = 0; idx < num; idx++)
  {
    status = tz_app_ufcrypto_hmac_func(sha_hmac_test_vectors[idx].msg,
                                       sha_hmac_test_vectors[idx].msg_len,
                                       sha_hmac_test_vectors[idx].hmac_key,
                                       sha_hmac_test_vectors[idx].hmac_key_len,
                                       sha_hmac_test_vectors[idx].msg_hmac1,
                                       sha_hmac_test_vectors[idx].msg_hmac256);

    if (status == E_SUCCESS)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"SW Hmac Functional Test - Vect Num:%u Passed", idx);
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"SW Hmac Functional Test - Vect Num:%u Failed", idx);
    }
  }
  
  return status;    
}



/**
  @brief 
    Sample code for how to call QSEE AES software crypto API's
*/
int tz_app_ufcrypto_aes_perf(SW_Cipher_Alg_Type alg,
                             SW_CipherModeType mode,
                             uint8 *pt,
                             uint32 pt_len,
                             uint8 *key,
                             uint32 key_len,
                             uint8 *iv)
{
  CryptoCntxHandle *cntx = 0;
  uint8 *ct_tmp = pt;
  int status = E_SUCCESS;

  IovecListType   ioVecIn;
  IovecListType   ioVecOut;
  IovecType       IovecIn;
  IovecType       IovecOut;
  SW_CipherEncryptDir dir = SW_CIPHER_ENCRYPT;

  /*------------------------------------------------------------------
    Init ctx
    ------------------------------------------------------------------*/

  /* Input IOVEC */
  ioVecIn.size = 1;
  ioVecIn.iov = &IovecIn;
  ioVecIn.iov[0].dwLen = pt_len;
  ioVecIn.iov[0].pvBase = pt; 

  /* Output IOVEC */
  ioVecOut.size = 1;
  ioVecOut.iov = &IovecOut;
  ioVecOut.iov[0].dwLen = pt_len;
  ioVecOut.iov[0].pvBase = ct_tmp;

  //Determine algorithm
  if(key_len == 16)
  {
    if(0 != qsee_SW_Cipher_Init(&cntx, SW_CIPHER_ALG_AES128))
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_Init API failed");
      status = -E_FAILURE;
    }
  }
  else
  {
    if(0 != qsee_SW_Cipher_Init(&cntx, SW_CIPHER_ALG_AES256))
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_Init API failed");
      status = -E_FAILURE;
    } 
  }

  /*--------------------------------------------------------------------
  Set key for encryption
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_KEY, key, key_len) != 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_cipher_set_parm API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set direction to encryption
  ----------------------------------------------------------------------*/

  if (E_SUCCESS == status &&
      qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_DIRECTION, &dir, sizeof(SW_CipherEncryptDir)) != 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_SetParam API failed");
    status = -E_FAILURE;
  }
  /*--------------------------------------------------------------------
  Set AES mode
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_MODE, &mode, sizeof(mode)) != 0)
  {  
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_SetParam API failed");
    status = -E_FAILURE;
  }

  /*--------------------------------------------------------------------
  Set IV
  ----------------------------------------------------------------------*/
  if(mode != SW_CIPHER_MODE_ECB)
  {
    if(E_SUCCESS == status &&
       qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_IV, iv, SW_AES_IV_SIZE) != 0)
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_SetParam API failed");
      status = -E_FAILURE;
    }
  }
  
  /*-----------------------------------------------------------------------
  Now encrypt the data
  -------------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_SW_CipherData(cntx, ioVecIn, &ioVecOut) != 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_CipherData encrypt API failed");
    status = -E_FAILURE;
  }

  if (cntx)
  {
    if(key_len == 16) 
    {
      if(0 != qsee_SW_Cipher_DeInit(&cntx, SW_CIPHER_ALG_AES128)) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_DeInit API failed");
        status = -E_FAILURE;
      }
    }
    else
    {
      if(0 != qsee_SW_Cipher_DeInit(&cntx, SW_CIPHER_ALG_AES256)) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher_DeInit API failed");
        status = -E_FAILURE;
      } 
    }
    cntx = 0;
  }

  return status;
}


/**
  @brief 
    Sample code for how to call QSEE HASH software crypto API's
*/
int tz_app_ufcrypto_hash_perf(SW_Auth_Alg_Type alg,
                              uint8 *msg,
                              uint32 msg_len)
{
  int status = E_SUCCESS;
  uint8 result_digest[SW_SHA256_DIGEST_SIZE];
  uint32 len;

  IovecListType   ioVecIn;
  IovecListType   ioVecOut;
  IovecType       IovecIn;
  IovecType       IovecOut;

  CryptoCntxHandle *cntx = NULL;

  //Clear result digest
  memset(result_digest, 0, sizeof(result_digest));

  if (alg == SW_AUTH_ALG_SHA1)
    len = SW_SHA1_DIGEST_SIZE;
  else
    len = SW_SHA256_DIGEST_SIZE;

    /* Input IOVEC */
  ioVecIn.size = 1;
  ioVecIn.iov = &IovecIn;
  ioVecIn.iov[0].dwLen = msg_len;
  ioVecIn.iov[0].pvBase = msg; 

  /* Output IOVEC */
  ioVecOut.size = 1;
  ioVecOut.iov = &IovecOut;
  ioVecOut.iov[0].dwLen = len;
  ioVecOut.iov[0].pvBase = result_digest;


  if(0 != qsee_SW_Hash_Init(&cntx, alg))
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_sw_hash_initAPI failed");
    status = -E_FAILURE;
  }
    
  if((E_SUCCESS == status) && (0 != qsee_SW_Hash_Update(cntx, ioVecIn)))
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Hash_Update API failed");
    status = -E_FAILURE;
  }


  if((E_SUCCESS == status) && (0 != qsee_SW_Hash_Final(cntx, &ioVecOut)))
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Hash_Final API failed");
    status = -E_FAILURE;
  }

  return status;
}


/**
  @brief 
    Sample code for how to call QSEE HMAC software crypto API's
*/
int tz_app_ufcrypto_hmac_perf(SW_Auth_Alg_Type alg,
                              uint8 *msg,
                              uint32 msg_len,
                              uint8* key,
                              uint32 key_len)
{
  int status = E_SUCCESS;
  uint8 result_digest[SW_SHA256_DIGEST_SIZE];
  uint8 result_len;

  IovecListType   ioVecIn;
  IovecListType   ioVecOut;
  IovecType       IovecIn;
  IovecType       IovecOut;

  //Clear result digest
  memset(result_digest, 0, sizeof(result_digest));

  if (alg == SW_AUTH_ALG_SHA1)
    result_len = SW_SHA1_DIGEST_SIZE;
  else if (alg == SW_AUTH_ALG_SHA256)
    result_len = SW_SHA256_DIGEST_SIZE;
  else
    return -E_FAILURE;

  /* Input IOVEC */
  ioVecIn.size = 1;
  ioVecIn.iov = &IovecIn;
  ioVecIn.iov[0].dwLen = msg_len;
  ioVecIn.iov[0].pvBase = msg; 

  /* Output IOVEC */
  ioVecOut.size = 1;
  ioVecOut.iov = &IovecOut;
  ioVecOut.iov[0].dwLen = result_len;
  ioVecOut.iov[0].pvBase = result_digest;

  if(0 != qsee_SW_Hmac(key, key_len, ioVecIn, &ioVecOut, alg))
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"qsee_SW_Hmac API failed");
    status = -E_FAILURE;
  }

  return status;
}

/**
  @brief 
    Sample code for AES performance tests
*/
int tz_app_sw_aes_perf(void)
{
  uint32 size;
  uint32 num  = TEST_CRYPTO_PERF_MAX_ITERATIONS;
  uint32 i = 0;
  int    tput;
  int    status = E_SUCCESS;
  unsigned long long total_time = 0;

  for (size = TZBSP_TEST_CRYPTO_PERF_START_SIZE; size <= TZBSP_TEST_CRYPTO_PERF_END_SIZE; size = size*2)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Performance Software AES128 Test - AES Algo: ECB Data Size: %u", size);

    total_time = qsee_get_uptime();

    for (i = 0; i < num; i++)
    {
      status = tz_app_ufcrypto_aes_perf(SW_CIPHER_ALG_AES128,
                                        SW_CIPHER_MODE_ECB,
                                        aes_performance,
                                        size,
                                        aes_key_1,
                                        sizeof(aes_key_1),
                                        NULL);
      if (status != E_SUCCESS) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"tz_app_ufcrypto_aes_perf test FAILED for iteration: %u", i);
        return status;
      }
    }
  
    total_time = qsee_get_uptime() - total_time;

    /*
     * average_time_in_milli_sec = total_time/num_of_iterations
     * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
     * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
     */
    tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);

    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Performance Software AES128 Test - AES Algo: CBC Data Size: %u", size);

    total_time = qsee_get_uptime();

    for (i = 0; i < num; i++)
    {
      status = tz_app_ufcrypto_aes_perf(SW_CIPHER_ALG_AES128,
                                        SW_CIPHER_MODE_CBC,
                                        aes_performance,
                                        size,
                                        aes_key_2,
                                        sizeof(aes_key_2),
                                        aes_iv_2);
      if (status != E_SUCCESS) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"tz_app_ufcrypto_aes_perf test FAILED for iteration: %u", i);
        return status;
      }
    }
    
    total_time = qsee_get_uptime() - total_time;
    
    /*
     * average_time_in_milli_sec = total_time/num_of_iterations
     * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
     * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
     */
    tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);

    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Performance Software AES128 Test - AES Algo: CTR Data Size: %u", size);

    
    total_time = qsee_get_uptime();

    for (i = 0; i < num; i++)
    {
      status = tz_app_ufcrypto_aes_perf(SW_CIPHER_ALG_AES128,
                                        SW_CIPHER_MODE_CTR,
                                        aes_performance,
                                        size,
                                        aes_key_2,
                                        sizeof(aes_key_2),
                                        aes_iv_2);
      if (status != E_SUCCESS) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"tz_app_ufcrypto_aes_perf test FAILED for iteration: %u", i);
        return status;
      }
    }
    
    total_time = qsee_get_uptime() - total_time;
    
    /*
     * average_time_in_milli_sec = total_time/num_of_iterations
     * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
     * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
     */
    tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);

  }

  return status;
}

/**
  @brief
    Sample code for AES GCM performance tests
*/

int tz_app_sw_aes_gcm_perf(void)
{

  uint32 size;
  uint32 num1  = TEST_CRYPTO_GCM_PERF_MAX_ITERATIONS;
  uint32 i = 0, j=0;
  int    tput;
  int    status = E_SUCCESS;
  unsigned long long total_time = 0;
  uint32 ptr_struct[5] = {0,0,1,0,0};
  uint32 header_struct[2];
  uint32 tag_struct[2];
  uint32 ptr_tmp_struct[2];
  int key_len = 16;

  /* Performance for AES128, AES-GCM */
  for (size = TZBSP_TEST_CRYPTO_PERF_START_SIZE; size <= TZBSP_TEST_CRYPTO_PERF_END_SIZE; size = size*2)
  {
    for( j = 0; j < MAX_TESTS; j++ )
    {
      ptr_tmp_struct[0] = (uint32)aes_performance;
      ptr_tmp_struct[1] = (uint32)aes_performance;

      ptr_struct[0] = (uint32)aes_performance;
      ptr_struct[1] = (uint32)aes_performance;
      header_struct[0] = (uint32)&additional_gcm[add_index_gcm[j]];
      header_struct[1] = (uint32)add_len_gcm[j];
      tag_struct[0] = (uint32)&tag_gcm[j];
      tag_struct[1] = 16;
      /* AES128 */
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Performance for GCM-128 ; Data Size %u", size);
      total_time = 0;
      ptr_struct[4] = 0x0;
      memset(aes_performance, 0x1, size);
      total_time = qsee_get_uptime();
      for (i = 0; i < num1; i++)
      {
        status = tz_app_ufcrypto_aes_gcm_func(ptr_struct, size, key_gcm[key_index_gcm[i]], key_len, iv_gcm[iv_index_gcm[i]],
                                     iv_len_gcm[i], ptr_tmp_struct,header_struct, tag_struct);
        if (status != E_SUCCESS)
        {
          QSEE_LOG(QSEE_LOG_MSG_DEBUG,"tz_app_crypto_aes_perf test FAILED for gcm iteration: %u", i);
          return status;
        }
      }
      total_time = qsee_get_uptime() - total_time;
      /*
       * average_time_in_milli_sec = total_time/num_of_iterations
       * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
       * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
       */
      tput = ( ((float)size/((float)total_time/(float)num1)) * 1000.00 ) / (1024.00);
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for %u iterations  %u ms", num1, total_time);
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Throughput: %u KBps", tput);
    }
  }

  return status;

}

/**
  @brief
    Sample code for HASH performance tests
*/
int tz_app_sw_hash_perf(void)
{
  uint32 size;
  uint32 num  = TEST_CRYPTO_PERF_MAX_ITERATIONS;
  uint32 i = 0;
  int    tput;
  int    status = E_SUCCESS;
  unsigned long long total_time = 0;

  for (size = TZBSP_TEST_CRYPTO_PERF_START_SIZE; size <= TZBSP_TEST_CRYPTO_PERF_END_SIZE; size = size*2)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Performance Software HASH Test - HASH Algo: SHA128 Data Size: %u", size);

    total_time = qsee_get_uptime();

    for (i = 0; i < num; i++)
    {
      status = tz_app_ufcrypto_hash_perf(SW_AUTH_ALG_SHA1,
                                         aes_performance,
                                         size);

      if (status != E_SUCCESS) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"tz_app_ufcrypto_hash_perf test FAILED for iteration: %u", i);
        return status;
      }
    }
  
    total_time = qsee_get_uptime() - total_time;

    /*
     * average_time_in_milli_sec = total_time/num_of_iterations
     * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
     * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
     */
    tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);
 
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Performance Software HASH Test - HASH Algo: SHA256 Data Size: %u", size);

    total_time = qsee_get_uptime();

    for (i = 0; i < num; i++)
    {
      status = tz_app_ufcrypto_hash_perf(SW_AUTH_ALG_SHA256,
                                         aes_performance,
                                         size);

      if (status != E_SUCCESS) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"tz_app_ufcrypto_hash_perf test FAILED for iteration: %u", i);
        return status;
      }
    }
  
    total_time = qsee_get_uptime() - total_time;

    /*
     * average_time_in_milli_sec = total_time/num_of_iterations
     * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
     * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
     */
    tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);
  }

  return status;
}

/**
  @brief 
    Sample code for HMAC performance tests
*/
int tz_app_sw_hmac_perf(void)
{
  uint32 size;
  uint32 num  = TEST_CRYPTO_PERF_MAX_ITERATIONS;
  uint32 i = 0;
  int    tput;
  int    status = E_SUCCESS;
  unsigned long long total_time = 0;

  for (size = TZBSP_TEST_CRYPTO_PERF_START_SIZE; size <= TZBSP_TEST_CRYPTO_PERF_END_SIZE; size = size*2)
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Performance Software HMAC Test - HMAC Algo: SHA128 Data Size: %u", size);

    total_time = qsee_get_uptime();

    for (i = 0; i < num; i++)
    {
      status = tz_app_ufcrypto_hmac_perf(SW_AUTH_ALG_SHA1,
                                         aes_performance,
                                         size,
                                         sha_hmac_key_1,
                                         sizeof(sha_hmac_key_1));

      if (status != E_SUCCESS) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"tz_app_ufcrypto_hmac_perf test FAILED for iteration: %u", i);
        return status;
      }
    }
  
    total_time = qsee_get_uptime() - total_time;

    /*
     * average_time_in_milli_sec = total_time/num_of_iterations
     * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
     * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
     */
    tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);

    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Performance Software HMAC Test - HMAC Algo: SHA256 Data Size: %u", size);

    total_time = qsee_get_uptime();

    for (i = 0; i < num; i++)
    {
      status = tz_app_ufcrypto_hmac_perf(SW_AUTH_ALG_SHA256,
                                         aes_performance,
                                         size,
                                         sha_hmac_key_1,
                                         sizeof(sha_hmac_key_1));

      if (status != E_SUCCESS) 
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG,"tz_app_ufcrypto_hmac_perf test FAILED for iteration: %u", i);
        return status;
      }
    }
  
    total_time = qsee_get_uptime() - total_time;

    /*
     * average_time_in_milli_sec = total_time/num_of_iterations
     * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
     * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
     */
    tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
    QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);
  }
  return status;
}


/**
  @brief 
    Sample code for AES functional tests
*/
int tz_app_crypto_test(uint32 src, uint32 dst, uint32 length)
{
  QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Start tz_app_crypto_test packet size = 0x%x", length);
  return tz_app_crypto_aes_func(aes_test_vectors[0].alg,
                                aes_test_vectors[0].mode,
                                (uint8 *)src,
                                length,
                                aes_test_vectors[0].key,
                                aes_test_vectors[0].key_len,
                                aes_test_vectors[0].iv,
                                (uint8 *)dst, 0);
}

/**
  @brief 
    Sample code to call crypto functional tests
*/
int tz_app_crypto_func_test(void)
{
  int status = E_SUCCESS;

  QSEE_LOG(QSEE_LOG_MSG_DEBUG,"----------Start Sample App Crypto Funtional Tests ------------");


  if (tz_app_aes_func() < 0)
  {  
    status = -E_FAILURE;
  } 

  if (E_SUCCESS == status && tz_app_des_func() < 0)
  {  
    status = -E_FAILURE;
  } 

  if (E_SUCCESS == status && tz_app_aes_xts_func() < 0)
  {  
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status && tz_app_hash_func() < 0)
  {  
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status && tz_app_hmac_func() < 0)
  {  
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status && tz_app_cmac_func() < 0)
  {  
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status && tz_app_aes_ccm_func() < 0)
  {
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status && tz_app_ecdh_func() < 0)
  {  
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status && tz_app_ecdsa_func() < 0)
  {  
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status && tz_app_crypto_kdf() < 0)
  {  
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status && tz_app_sw_aes_func() < 0)
  {
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status && tz_app_sw_hash_func() < 0)
  {
     status = -E_FAILURE;
  }

  if (E_SUCCESS == status && tz_app_sw_hmac_func() < 0)
  {
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status && tz_app_generic_ecdsa_func() < 0)
  {  
    status = -E_FAILURE;
  }
  
  if (E_SUCCESS == status && tz_app_generic_ecdh_func() < 0)
  {  
    status = -E_FAILURE;
  }
  QSEE_LOG(QSEE_LOG_MSG_DEBUG,"----------End Sample App Crypto Funtional Tests --------------");

  return status;
}

/**
  @brief 
    Sample code to call crypto performance test
*/
int tz_app_crypto_perf_test(void)
{
  int status = E_SUCCESS;

  //Init the data buffer
  memset(aes_performance, 0xed, sizeof(aes_performance));

  QSEE_LOG(QSEE_LOG_MSG_DEBUG,"----------Start Sample App Crypto Performance Tests ------------");

  if (tz_app_aes_perf() < 0)
  {  
    status = -E_FAILURE;
  } 

  if (E_SUCCESS == status && tz_app_des_perf() < 0)
  {  
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status && tz_app_aes_xts_perf() < 0)
  {  
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status && tz_app_hash_perf() < 0)
  {  
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status && tz_app_hmac_perf() < 0)
  {  
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status && tz_app_cmac_perf() < 0)
  {  
    status = -E_FAILURE;
  }
  if (E_SUCCESS == status && tz_app_sw_aes_perf() < 0)
  {
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status && tz_app_sw_aes_gcm_perf() < 0)
  {
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status && tz_app_sw_hash_perf() < 0)
  {
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status && tz_app_sw_hmac_perf() < 0)
  {
    status = -E_FAILURE;
  }

  if (E_SUCCESS == status && tz_app_kdf_perf() < 0)
  {  
    status = -E_FAILURE;
  }

  QSEE_LOG(QSEE_LOG_MSG_DEBUG,"----------End Sample App Crypto Performance Tests --------------");

  return status;
}

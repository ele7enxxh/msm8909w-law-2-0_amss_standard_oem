#ifndef SECENC_H
#define SECENC_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  S E C E N C   H E A D E R

GENERAL DESCRIPTION
  This header file defines the interface for the Security Services
  Crypto-Interface layer for encrypt and decrypt operations for various
  stream and block ciphers for bulk data encryption.

EXTERNAL FUNCTIONS

  secenc_initialize_instance() - Init a cipher instance
  secenc_set_param()           - Set the parameter for a cipher instance
  secenc_get_param()           - Get the parameter for a cipher instance
  secenc_encrypt()             - Encrypt the message
  secenc_decrypt()             - Decrypt the message
  secenc_add_padding()         - Pad message to cipher block size
  secenc_remove_padding()      - Removes block cipher padding

Copyright (c) 2000-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/shared/src/secenc.h#1 $
  $DateTime: 2016/12/13 07:59:23 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/21/13   nk      Kasumi algorithm ported. 
04/30/12   nk      ZUC algorithm ported. 
07/18/10   nk      Shared Library framework - Removed FEATURE_SEC
02/11/10   ejt     Moved SNOW3G LFSR and FSM from global data to crypto instance 
06/10/09   ejt     Updated code to use secapi APIs
08/31/05   df      Changes to support IOVEC
02/25/05   sb      Added support for MSM device key based AES128 enc/dec
11/12/04   rv      changes to correct LINT errors
02/24/04   rwh     Removed redundant #define's, use SECCRYPT_ENC_ ones.
12/16/03   yw      Added AES128 support.
12/09/03   jay     Aligining the ARC4 context structs for the DSP
10/29/03   jay     Deleted the hmac_len field
10/28/03   jay     Added an outer_msg_ptr field for hmac
06/05/03   om      Added cipher input mode, fixed mac_secret names.
06/02/03   om      Named cipherinfo structures for debug.
04/04/03   om      Created module.

===========================================================================*/

#include "comdef.h"
#include "secencarm.h"
#include "secenchw.h"

/*===========================================================================

                                CONSTANTS

===========================================================================*/

/*---------------------------------------------------------------------------
               GLOBAL CONSTANTS - FEATURES
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Bit masks that represent each ENC parameter. These are used to
  set and determine the status of a ENC instance.
---------------------------------------------------------------------------*/
#define SECENC_ENC_KEY_PARAM_MASK   0x00000001
#define SECENC_DEC_KEY_PARAM_MASK   0x00000002
#define SECENC_MAC_MODE_PARAM_MASK  0x00000004
#define SECENC_DEVKEY_PARAM_MASK    SECCRYPT_DEVKEY_PARAM_MASK //0x8

#define SECENC_REQUIRED_PARAMS_MASK (SECENC_ENC_KEY_PARAM_MASK \
                                     | SECENC_DEC_KEY_PARAM_MASK)

#define SECENC_AES_MAX_SUBKEY_SIZE  240 //60*4 
#define SECENC_AES_PAD_SIZE         128 //416-288 -- Ctx size of 3DES - 
                                            //AES Ctx size

/* TLS specific changes SECAPI_HMAC_TLS_12_TYPE needs support upto 32bytes of digest*/
#define SECENC_MAX_MAC_SECRET_SIZE  SECAPI_HSH_LARGEST_DIGEST_SIZE 


#define SECENC_SHA_HMAC_PAD_LEN  40
#define SECENC_MD5_HMAC_PAD_LEN  48

#define SECENC_INNER_HMAC_PAD_VALUE 0x36
#define SECENC_OUTER_HMAC_PAD_VALUE 0x5C

#define SECENC_HMAC_SEQUENCE_LEN  8

typedef enum
{
  SECENC_MAC_CREATE = 0,
  SECENC_MAC_VERIFY = 1
} secenc_mac_enum_type;

/* Tho following mimics the context structure from aes.h */
typedef struct
{
  uint32  sub_key[4 * SECAPI_ENC_AES_BLOCK_BYTE_LEN]; /* the key schedule  */
  uint32  Nkey;               /* the number of words in the key input block */
  uint32  Nrnd;               /* the number of cipher rounds                */
} secenc_aes_ctx_type;

/*---------------------------------------------------------------------------
  Type for a Cipher instance, included by the generic seccrypt instance
---------------------------------------------------------------------------*/
struct secenc_instance_s
{
  struct __enc_common_info_s
  {
    /* Keep track whether this instance is currently executing */
    boolean enc_busy;
    boolean dec_busy;

    /* Keep track of execution time */
    //seccrypt_exec_time_type  enc_time;
    //seccrypt_exec_time_type  dec_time;

    /* Cipher input mode (Continue or Final)*/
    secapi_enc_input_mode_enum_type input_mode;

    /* Common data for block ciphers only */
    struct
    {
      /* Indicates CBC or ECB mode (CBC is default) */
      secapi_enc_op_mode_enum_type  op_mode;

      /*counter size. currently support 16-bit or 128 bit */
      secapi_enc_counter_width_type counter_width;

      /*input plaintext bit length. */
      secapi_enc_input_bit_length_type input_bit_len;

      /* The DSP and hardware accelerators expect data im block multi-   */
      /* ples. This array stores leftover bytes from commands executed   */
      /* in continuous operation.                                        */
      struct
      {
        struct
        {
          uint8   current[SECAPI_ENC_MAX_BLOCK_BYTE_LEN];
          uint16  current_size;
          uint8   newbytes[SECAPI_ENC_MAX_BLOCK_BYTE_LEN];
          uint16  new_size;
        } enc;

        struct
        {
          uint8   current[SECAPI_ENC_MAX_BLOCK_BYTE_LEN];
          uint16  current_size;
          uint8   newbytes[SECAPI_ENC_MAX_BLOCK_BYTE_LEN];
          uint16  new_size;
        } dec;

      } leftover;

      /* Synchronized IV's allow to use the CBC state of the last ope-   */
      /* ration for the next operation independent of enc- / decryption. */
      boolean  synchronize_ivs;

      /* Reference to the CBC state of block ciphers. Thses pointers are */
      /* specific to the cipher selected (set up at instantiation time). */
      struct
      {
        struct
        {
          uint8*  ptr;
          uint16  size;
        } enc;
        
        struct
        {
          uint8*  ptr;
          uint16  size;
        } dec;

      } cbc_state;

      /* Padding type required */
      struct                                           
      {
        secapi_enc_padding_enum_type type;
        uint8                        value;
      } padding;

    } block;

    /* The size of the cipher key is implicit to the algorithm selected. */
    /* These size fields are set up at instantiation to avoid looking up */
    /* the algorithm type every time you need the key and IV sizes.      */
    struct
    {
      uint16  key;
      uint16  iv;
      uint16  block_size;
    } cipher_info_size;

    /* Pad info - supports SSLv3, TLS 1.0 and IPSec MAC's    */
    struct secenc_pad_info_s
    {
      uint16 pad_len;
      uint8  pad[SECAPI_ENC_MAX_BLOCK_BYTE_LEN];
    }pad_info;

    /* MAC mode information - supports SSLv3, TLS 1.0 and IPSec MAC's    */
    struct secenc_mac_info_s
    {
      /* Mac mode and handle executing the hashing */
      /* If cipher runs on multiple platforms these need to be duplicated 
         for enc and dec */
      secapi_hsh_hmac_enum_type  mode;
      seccrypt_instance_type*    hash_instance_ptr;
      IOVEC_T                    outer_msg;  // Keys
      IOVEC                      ov;         // Keys
      uint8                      inner_hash_payload[100];  // should be 80?
      uint8                      outer_hash_payload[100];  // should be 88?
      uint8                      hmac[SECAPI_HSH_LARGEST_DIGEST_SIZE];
      uint8                      decrypted_mac_buf[SECENC_MAX_MAC_SECRET_SIZE];

      /* Mac specific configuration data */
      union
      {
        struct
        {
          struct
          {
            uint8   mac_secret[SECENC_MAX_MAC_SECRET_SIZE];
          } enc;
          
          struct
          {
            uint8   mac_secret[SECENC_MAX_MAC_SECRET_SIZE];
          } dec;

        } ssl;

        union
        {
          struct
          {
            uint8   ddd;
          } v4;

          struct
          {
            uint8   ddd;
          } v6;
        } ipsec;

      } config;
    } mac_info;
 
    /* ---------------------------------------------------------------- */
    /* Shared cipher data starts here                                   */
    /* ---------------------------------------------------------------- */

    /* Each cipher has an encryption and decryption info struct         */
    /* associated to hold keys, IV's, and the current CBC / SBox state. */

    /* The ARM implementations need this to be 32-bit aligned */
    uint32 alignment_dummy;
    union
    {
      /* DES info - needs to be same size as 3DES */
      struct __enc_des_cipherinfo_s
      {
        struct
        {
          uint8  iv[SECAPI_ENC_DES_BLOCK_BYTE_LEN];
          uint8  key[SECAPI_ENC_3DES_KEY_BYTE_LEN];
          uint8  subkey[16][SECAPI_ENC_DES_KEY_BYTE_LEN];

          uint8  __reserved_key_space[2][16][SECAPI_ENC_DES_KEY_BYTE_LEN];
        } enc;

        struct
        {
          uint8  iv[SECAPI_ENC_DES_BLOCK_BYTE_LEN];
          uint8  key[SECAPI_ENC_3DES_KEY_BYTE_LEN];
          uint8  subkey[16][SECAPI_ENC_DES_KEY_BYTE_LEN];

          /* Pad here? */

        } dec;

      } des;
      
      /* 3DES info */
      struct __enc_des3_cipherinfo_s
      {
        struct
        {
          uint8  iv[SECAPI_ENC_DES_BLOCK_BYTE_LEN];
          uint8  key[SECAPI_ENC_3DES_KEY_BYTE_LEN];
          uint8  subkey[3][16][SECAPI_ENC_DES_KEY_BYTE_LEN];
        } enc;

        struct
        {
          uint8  iv[SECAPI_ENC_DES_BLOCK_BYTE_LEN];
          uint8  key[SECAPI_ENC_3DES_KEY_BYTE_LEN];
          uint8  subkey[3][16][SECAPI_ENC_DES_KEY_BYTE_LEN];
        } dec;

      } des3;
      
      /* AES info - needs to be same size as 3DES */
      struct __enc_aes_cipherinfo_s
      {
        struct
        {
          uint8 iv[SECAPI_ENC_AES_BLOCK_BYTE_LEN];
          uint8 key[SECAPI_ENC_AES256_KEY_BYTE_LEN];
          secenc_aes_ctx_type ctx;
          uint8 aes_pad[ 416 - (SECAPI_ENC_AES_BLOCK_BYTE_LEN
                             +  SECAPI_ENC_AES256_KEY_BYTE_LEN
                             +  sizeof(secenc_aes_ctx_type)) ];
        } enc;

        struct
        {
          uint8 iv[SECAPI_ENC_AES_BLOCK_BYTE_LEN];
          uint8 key[SECAPI_ENC_AES256_KEY_BYTE_LEN];
          secenc_aes_ctx_type ctx;

          /* Pad here? */

        } dec;
      } aes;

      /* ARC4 info - needs to be same size as 3DES */
      struct __enc_arc4_cipherinfo_s
      {
        struct
        {
          uint8  key[16];
          uint8  sbox_i;
          uint8  sbox_j;
          uint8  sbox[256];
          uint8  pad[142];  /* Just to make the struct size same as 3DES */
        } enc;

        struct
        {
          uint8  key[16];
          uint8  sbox_i;
          uint8  sbox_j;
          uint8  sbox[256];

          /* Pad here? */

        } dec;

      } arc4;
      
      /* SNOW3G info - needs to be same size as 3DES */
      struct __enc_snow3g_cipherinfo_s
      { 
        uint8  iv[SECAPI_ENC_SNOW3G_IV_SIZE];
        uint8  key[SECAPI_ENC_SNOW3G_KEY_SIZE];
        uint32 LFSR[SECAPI_ENC_SNOW3G_LFSR_SIZE];
        uint32 FSM[SECAPI_ENC_SNOW3G_FSM_SIZE];
      } snow3g;

      /* ZUC info - needs to be same size as 3DES */
      struct __enc_zuc_cipherinfo_s
      { 
        uint8  iv[SECAPI_ENC_ZUC_IV_SIZE];
        uint8  key[SECAPI_ENC_ZUC_KEY_SIZE];
      } zuc;

      /* Kasumi info - needs to be same size as 3DES */
      struct __enc_kasumi_cipherinfo_s
      { 
        uint8  iv[SECAPI_ENC_KASUMI_IV_SIZE];
        uint8  key[SECAPI_ENC_KASUMI_KEY_SIZE];
      } kasumi;

    } cipher_info;

  } common;

  /* All implementation-specific data is in here */
  union
  {
    secencarm_instance_type  arm;
    secenchw_instance_type   hw;
  } platform;

};


/*===========================================================================

                            DATA DECLARATIONS

===========================================================================*/

/* ---------------------------------------------------------------------- */
/* Ciphering State Machine transition function tables                     */
/* ---------------------------------------------------------------------- */
extern seccrypts_state_x_funcs_table_type    secenc_x_func_table;
extern seccrypts_state_x_allowed_table_type  secenc_x_allowed_table;


/*===========================================================================

                          FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION SECENC_INITIALIZE_INSTANCE

DESCRIPTION
  This function sets the default parameters for the passed-in 
  cipher instance.

DEPENDENCIES
  None

PARAMETERS
  instance_ptr  - Pointer to hash instance

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secenc_initialize_instance
(
  seccrypt_instance_type*  instance_ptr
);

/*===========================================================================

FUNCTION SECENC_SET_PARAM()

DESCRIPTION
  This function will set the parameters for the cipher instance specified.

DEPENDENCIES
  None.

RETURN VALUE
  Status of request.
    E_SUCCESS if successful
    E_FAILURE if failed to process the request.

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type secenc_set_param
(
  seccrypt_instance_type*      instance_ptr,
  secapi_enc_param_enum_type   param,
  secapi_enc_param_data_type*  param_data_ptr
);

/*===========================================================================

FUNCTION SECENC_GET_PARAM()

DESCRIPTION
  This function will retrieve the cipher parameters for the instance 
  specified.

DEPENDENCIES
  None.

RETURN VALUE
  Status of request.
    E_SUCCESS if successful
    E_FAILURE if failed to process the request.

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type secenc_get_param
(
  seccrypt_instance_type*      instance_ptr,
  secapi_enc_param_enum_type   param,
  secapi_enc_param_data_type*  param_data_ptr
);

/*===========================================================================

FUNCTION SECENC_ENCRYPT()

DESCRIPTION
  This function will encrypt the passed message using the instance-specific
  algorithm.
  Based on the MAC mode, it will create the specified MAC if necessary.

DEPENDENCIES
  None

RETURN VALUE
  Status of request.
    E_SUCCESS        on sucessful completion
    E_BAD_DATA       Data verification failed
    E_NOT_SUPPORTED  Option hasn't been coded yet
    E_FAILURE        Any other failure to process the request

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type secenc_encrypt
(
  seccrypt_instance_type*  instance_ptr
);

/*===========================================================================

FUNCTION SECENC_DECRYPT()

DESCRIPTION
  This function will decrypt the passed data using the instace-specific
  algorithm.
  Based on the MAC mode, it will verify the specified MAC if necessary.

DEPENDENCIES
  None

RETURN VALUE
  Status of request.
    E_SUCCESS        on sucessful completion
    E_BAD_DATA       Data verification failed
    E_NOT_SUPPORTED  Option hasn't been coded yet
    E_FAILURE        Any other failure to process the request

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type secenc_decrypt
(
  seccrypt_instance_type*  instance_ptr
);




/*===========================================================================

FUNCTION SECENC_FINISH_OP()

DESCRIPTION
  Common cleanup work for ciphering operations.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void secenc_finish_op
(
  seccrypt_instance_type* instance_ptr
);

#endif /* SECENC_H */

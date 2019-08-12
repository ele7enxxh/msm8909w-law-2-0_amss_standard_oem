/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      TrustZone Keystore

General Description
  Implement a keystore used for SSD

Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/services/ssd/src/tz_ks.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when         who     what, where, why
--------     ---     -------------------------------------------------------
01/30/13     pre     Initial Revision
===========================================================================*/

/* TODO: Make keystore mem cached */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "tz_ks.h"
#include "tzbsp_cipher.h"
#include "tzbsp_hmac.h"
#include "tzbsp_kdf.h"
#include "tzbsp_memory_map.h"
#include "tzbsp_prng.h"
#include "tz_ssd.h" /* just for OVERFLOWS macro */
#include "tz_ssd_errno.h"
#include "tzbsp_mpu.h"
#include "tzbsp_mem.h"
#include <string.h>

/*----------------------------------------------------------------------------
 * Definitions And Type Declarations
 * -------------------------------------------------------------------------*/
#define MIN_DIR_ENTRIES 10

typedef enum
{
  KS_ENCRYPT,
  KS_DECRYPT
} ks_cipher_direction_t;

typedef enum
{
  KS_EMV_RESERVED         = 0,

  KS_EMV_HMAC256_AES128   = 1,

  KS_KS_EM_MAX_USAGE      = 0x7FFFFFFF
} ks_em_ver_t;

typedef struct
{
  uint8 iv[TZBSP_AES128_IV_SIZE];
} ks_ent_iv_t;

typedef struct
{
  ks_ent_md_t md;
  ks_ent_id_t id;
} __attribute__((packed)) ks_dir_key_info_t;

typedef struct
{
  uint32            version;
  uint32            num_keys;
  ks_dir_key_info_t key_info[];
} __attribute__((packed)) ks_dir_t;

typedef struct
{
  ks_em_ver_t    em_version;
  ks_auth_tag_t  auth_tag;
  uint32         msg_len;
  ks_ent_iv_t    iv;
  uint8          msg[];
} __attribute__((packed)) ks_entry_info_t;

typedef struct
{
  ks_em_ver_t em_version;
  ks_ent_md_t md;
  ks_ent_id_t id;
  uint32      len;
  ks_ent_iv_t iv;
  uint8       enc_msg[];
} __attribute__((packed)) ent_em_t;

typedef struct
{
  uint32           ii;
  uint32           num_keys;
  ks_entry_info_t* cur_entry_i_ptr;
  ks_entry_info_t* dir_entry_ptr;
} ks_iter_t;

static uint8 ks_key[TZBSP_AES128_KEY_SIZE];
static boolean ks_key_init = FALSE;

static uint8 hmac_key[TZBSP_AES256_KEY_SIZE];
static boolean hmac_key_init = FALSE;

static ks_entry_info_t* g_ks_ptr =
  (ks_entry_info_t *) TZBSP_SSD_REGION_BASE;
static uint32 g_ks_len = 0;

/*-------------------------------------------------------------------------
 * Private Functions
 *-------------------------------------------------------------------------*/
static int tz_ks_validate_entry_len(ks_entry_info_t* ent_i_ptr, uint32 val_len)
{
  uint32 info_base = (uint32) ent_i_ptr;
  uint32 msg_len = ent_i_ptr->msg_len;
  uint32 ent_ovhd = tz_ks_get_entry_overhead_size();

  /* Check specific entry overflow and length.  This overflow check
     is not covered in the generic length overflow test required
     before calling. */
  if (OVERFLOWS(info_base, msg_len + ent_ovhd) || 0 == msg_len)
  {
    TZBSP_ERROR_CODE(KS_VEL_INVALID);
    return -E_FAILURE;
  }

  /* The length from the end of the current entry must fit within the
     given length. */
  if (msg_len + ent_ovhd  > val_len)
  {
    TZBSP_ERROR_CODE(KS_VEL_BOUNDS);
    return -E_FAILURE;
  }

  return E_SUCCESS;
}

/**
   Encrypt or decrypt the entry data from a given entry info pointer
   using a key derived from the hardware key

   @pre ent_i_ptr must point to a valid pointer
   @pre direction may only be KS_ENCRYPT or KS_DECRYPT
 */
static int tz_ks_enc_dec_entry
(
  ks_entry_info_t*      ent_i_ptr,
  ks_cipher_direction_t direction
)
{
  tzbsp_cipher_ctx*   ctx;
  TZBSP_CIPHER_MODE_ET mode = TZBSP_CIPHER_MODE_CTR;
  int (*cipher_fp)(const tzbsp_cipher_ctx *, const uint8 *,
                   uint32, uint8*, uint32*);
  uint32 len = ent_i_ptr->msg_len;

  /* Initialize crypto engine */
  if (E_SUCCESS != tzbsp_cipher_init(TZBSP_CIPHER_ALGO_AES_128, &ctx))
  {
    TZBSP_ERROR_CODE(KS_EDE_CIPHER_INIT);
    return -E_FAILURE;
  }

  /* setup CBC mode */
  if (E_SUCCESS != tzbsp_cipher_set_param(ctx, TZBSP_CIPHER_PARAM_MODE,
                                          &mode, sizeof(mode)))
  {
    TZBSP_ERROR_CODE(KS_EDE_MODE);
    tzbsp_cipher_free_ctx(ctx);
    return -E_FAILURE;
  }

  /* Set up IV */
  if (E_SUCCESS !=
      tzbsp_cipher_set_param(ctx, TZBSP_CIPHER_PARAM_IV,
                             &ent_i_ptr->iv, sizeof(ent_i_ptr->iv)))
  {
    TZBSP_ERROR_CODE(KS_EDE_IV);
    tzbsp_cipher_free_ctx(ctx);
    return -E_FAILURE;
  }

  if (!ks_key_init)
  {
    char* label   = "SSD:Keystore Entry Encryption Key";
    char* context = "7Tt3QFpJX46qNUqzExqX";

    if (E_SUCCESS != tzbsp_kdf(NULL, 0,
                               label, strlen(label),
                               context, strlen(context),
                               ks_key, sizeof(ks_key)))
    {
      TZBSP_ERROR_CODE(KS_EDE_KDF);
      tzbsp_cipher_free_ctx(ctx);
      return -E_FAILURE;
    }

    ks_key_init = TRUE;
  }

  if (E_SUCCESS != tzbsp_cipher_set_param(ctx, TZBSP_CIPHER_PARAM_KEY,
                                          ks_key, sizeof(ks_key)))
  {
    TZBSP_ERROR_CODE(KS_EDE_KEY);
    tzbsp_cipher_free_ctx(ctx);
    return -E_FAILURE;
  }

  if (KS_ENCRYPT == direction)
  {
    cipher_fp = tzbsp_cipher_encrypt;
  }
  else
  {
    cipher_fp = tzbsp_cipher_decrypt;
  }


  if (E_SUCCESS != cipher_fp(ctx, ent_i_ptr->msg, len, ent_i_ptr->msg, &len))
  {
    TZBSP_ERROR_CODE(KS_EDE_CIPHER_OP);
    tzbsp_cipher_free_ctx(ctx);
    return -E_FAILURE;
  }

  tzbsp_cipher_free_ctx(ctx);

  if (len != ent_i_ptr->msg_len)
  {
    TZBSP_ERROR_CODE(KS_EDE_OUT_LEN);
    return -E_FAILURE;
  }

  return E_SUCCESS;
}

/* TODO: request HMAC in parts so no malloc/memcpy needed */
static int tz_ks_auth_entry(ks_entry_info_t* ent_i_ptr,
                            ks_ent_id_t*     id,
                            ks_ent_md_t      md,
                            ks_auth_tag_t*   auth_tag)
{
  ent_em_t* em_ptr =
    (ent_em_t*) tzbsp_malloc(sizeof(*em_ptr) + ent_i_ptr->msg_len);
  int retval;

  if (NULL == em_ptr)
  {
    TZBSP_ERROR_CODE(KS_AE_MALLOC);
    return -E_FAILURE;
  }

  /* Fill in EM for HMAC */
  if (id)
  {
    memcpy(&em_ptr->id, id, sizeof(em_ptr->id));
  }
  else
  {
    memset(&em_ptr->id, 0, sizeof(em_ptr->id));
  }

  em_ptr->md = md;
  em_ptr->len = ent_i_ptr->msg_len;
  em_ptr->em_version = ent_i_ptr->em_version;
  memcpy(&em_ptr->iv, &ent_i_ptr->iv, sizeof(em_ptr->iv));
  memcpy(em_ptr->enc_msg, ent_i_ptr->msg, ent_i_ptr->msg_len);

  if (!hmac_key_init)
  {
    char* label   = "SSD:Key Entry Auth Key";
    char* context = "tPMYHBWHwsgT4RjcqzWA";

    if (E_SUCCESS != tzbsp_kdf(NULL, 0,
                               label, strlen(label),
                               context, strlen(context),
                               hmac_key, sizeof(hmac_key)))
    {
      TZBSP_ERROR_CODE(KS_AE_KDF);
      return -E_FAILURE;
    }

    hmac_key_init = TRUE;
  }

  /* Compute MAC for EM */
  retval = tzbsp_hmac(TZBSP_HMAC_SHA256,
                      (const uint8*) em_ptr,
                      sizeof(*em_ptr) + ent_i_ptr->msg_len,
                      hmac_key,
                      sizeof(hmac_key),
                      (uint8*) auth_tag);

  tzbsp_free(em_ptr);

  if (E_SUCCESS != retval)
  {
    TZBSP_ERROR_CODE(KS_AE_HMAC);
    return -E_FAILURE;
  }

  return E_SUCCESS;
}

/**
   Checks the directory entry in the keystore for length and decrypts.
   This is done separately from other entries because the keystore
   iterator can validate the lengths of entries only after the
   directory is decrypted.
 */
static int tz_ks_decrypt_auth_entry(ks_entry_info_t* ent_i_ptr,
                                    ks_ent_id_t*     id,
                                    ks_ent_md_t      md)
{
  ks_auth_tag_t auth_tag;

  if (KS_EMV_HMAC256_AES128 != ent_i_ptr->em_version)
  {
    TZBSP_ERROR_CODE(KS_DAE_EM_VER);
    return -E_FAILURE;
  }

  if (E_SUCCESS != tz_ks_auth_entry(ent_i_ptr, id, md, &auth_tag))
  {
    TZBSP_ERROR_CODE(KS_DAE_AUTH);
    return -E_FAILURE;
  }

  if (memcmp(&ent_i_ptr->auth_tag, &auth_tag, sizeof(auth_tag)))
  {
    TZBSP_ERROR_CODE(KS_DAE_AUTH_CMP);
    return -E_FAILURE;
  }

  /* decrypt the dir with the derived key */
  if (E_SUCCESS != tz_ks_enc_dec_entry(ent_i_ptr, KS_DECRYPT))
  {
    TZBSP_ERROR_CODE(KS_DAE_CIPHER);
    return -E_FAILURE;
  }

  /* Create some zeros between entries for debug */
  memset(&ent_i_ptr->auth_tag, 0, sizeof(ent_i_ptr->auth_tag));
  memset(&ent_i_ptr->iv, 0, sizeof(ent_i_ptr->iv));

  return E_SUCCESS;
}

static ks_entry_info_t* tz_ks_iter_get_entry_info(const ks_iter_t* iter)
{
  return iter->cur_entry_i_ptr;
}

static ks_ent_id_t* tz_ks_iter_get_entry_id(const ks_iter_t* iter)
{
  return &((ks_dir_t*) iter->dir_entry_ptr->msg)->key_info[iter->ii].id;
}

static ks_ent_md_t tz_ks_iter_get_entry_md(const ks_iter_t* iter)
{
  return ((ks_dir_t*) iter->dir_entry_ptr->msg)->key_info[iter->ii].md;
}


/**
   @pre iter points to valid-length iterator
   @pre directory pointer should point to memory with enough length to
        fit a directory header
   @pre directory entry of keystore must be the first entry
   @pre directory entry in keystore must be decrypted for num_keys to be
        valid.
 */
static void tz_ks_iter_init
(
  ks_iter_t*       iter,
  ks_entry_info_t* ks
)
{
  /* Make sure iter is ZI in case caller ignores return status */
  memset(iter, 0, sizeof(*iter));

  iter->cur_entry_i_ptr = ks;
  iter->dir_entry_ptr   = ks;
  iter->num_keys        = ((ks_dir_t*) ks->msg)->num_keys;
}

/**
   @pre Iterator must first be initialized using tz_ks_iter_init
   @pre keystore length has been validated using
        tz_ks_iter_validate_len in order to ensure the iteration
        points to a valid key
   @pre directory entry in keystore must be plaintext at the time of
        init for the next_entry to work properly.
*/
static int tz_ks_iter_next_entry(ks_iter_t* iter)
{
  ks_entry_info_t* temp;

  /* iter index includes keystore */
  if (iter->ii >= iter->num_keys)
  {
    /* This will be hit commonly, so no error is desired. */
    return -E_FAILURE;
  }

  /* Entries are contiguous in memory */
  temp = (ks_entry_info_t*)((uint32) iter->cur_entry_i_ptr +
                            sizeof(*temp) + iter->cur_entry_i_ptr->msg_len);


  iter->ii++;
  iter->cur_entry_i_ptr = temp;

  return E_SUCCESS;
}

/**
   @pre Iterator must first be initialized using tz_ks_iter_init
   @pre The length of the entire keystore has been validated to not
        overflow and not go into secure memory before calling

   @sideeffect On failure leaves the iterator at the key that caused
               overflow with given length
 */
static int tz_ks_iter_validate_ks_len(ks_iter_t* iter, uint32 ks_len)
{
  ks_entry_info_t* backup    = iter->cur_entry_i_ptr;
  uint32           backup_ii = iter->ii;

  do
  {
    ks_entry_info_t* ci_ptr = tz_ks_iter_get_entry_info(iter);

    /* The length from the end of the current entry must fit within the
       given length of the keystore. */
    if (E_SUCCESS !=
        tz_ks_validate_entry_len(ci_ptr, ((uint32) iter->dir_entry_ptr +
                                          ks_len - (uint32) ci_ptr)))
    {
      TZBSP_ERROR_CODE(KS_IVKSL_ENT_LEN);
      return -E_FAILURE;
    }

    if (ci_ptr == iter->dir_entry_ptr)
    {
      uint32 dir_entries = (iter->num_keys < MIN_DIR_ENTRIES) ?
        MIN_DIR_ENTRIES : iter->num_keys;

      /* Check to make sure there's room for the minimum directory
         entries or the number of directory entries. */
      if (ci_ptr->msg_len <
          dir_entries * sizeof(ks_dir_key_info_t) + sizeof(ks_dir_t))
      {
        TZBSP_ERROR_CODE(KS_IVKSL_DIR_LEN);
        return -E_FAILURE;
      }
    }

  } while (E_SUCCESS == tz_ks_iter_next_entry(iter));

  iter->cur_entry_i_ptr = backup;
  iter->ii              = backup_ii;

  return E_SUCCESS;
}

static int tz_ks_iter_encrypt_auth_entry(ks_iter_t* iter)
{
  ks_entry_info_t* ent_i_ptr = tz_ks_iter_get_entry_info(iter);
  ks_ent_id_t* ent_id;
  ks_ent_md_t  ent_md;

  /* ID and MD are ignored for directory */
  if (iter->cur_entry_i_ptr == iter->dir_entry_ptr)
  {
    ent_id = NULL;
    ent_md = KS_SSD_INVALID_MD;
  }
  else
  {
    ent_id = tz_ks_iter_get_entry_id(iter);
    ent_md = tz_ks_iter_get_entry_md(iter);
  }

  if (KS_EMV_HMAC256_AES128 != ent_i_ptr->em_version)
  {
    TZBSP_ERROR_CODE(KS_EAE_EM_VER);
    return -E_FAILURE;
  }

  if (tzbsp_prng_getdata((uint8*) &ent_i_ptr->iv, sizeof(ent_i_ptr->iv)) !=
      sizeof(ent_i_ptr->iv))
  {
    TZBSP_ERROR_CODE(KS_EAE_PRNG);
    return -E_FAILURE;
  }

  /* Encrypt entry with the derived key */
  if (E_SUCCESS != tz_ks_enc_dec_entry(ent_i_ptr, KS_ENCRYPT))
  {
    TZBSP_ERROR_CODE(KS_EAE_CIPHER);
    return -E_FAILURE;
  }

  if (E_SUCCESS != tz_ks_auth_entry(ent_i_ptr, ent_id, ent_md,
                                    &ent_i_ptr->auth_tag))
  {
    TZBSP_ERROR_CODE(KS_EAE_AUTH);
    return -E_FAILURE;
  }

  return E_SUCCESS;
}

/*-------------------------------------------------------------------------
 * Public Functions
 *-------------------------------------------------------------------------*/

/**
   @copydoc tz_ks_get_key
 */
int tz_ks_get_key
(
  const ks_ent_id_t* id,
  ks_ent_md_t        md,
  const uint8**      key,
  uint32*            key_len
)
{
  ks_iter_t iter;

  if (g_ks_len == 0)
  {
    TZBSP_ERROR_CODE(KS_GET_KEY_NO_KS_PROVISIONED);
    return -E_FAILURE;
  }

  tz_ks_iter_init(&iter, g_ks_ptr);

  /* Skips the first entry, which is the directory. */
  while (E_SUCCESS == tz_ks_iter_next_entry(&iter))
  {
    if (!memcmp(tz_ks_iter_get_entry_id(&iter), id, sizeof(*id)))
    {
      if (tz_ks_iter_get_entry_md(&iter) == md)
      {
        if (key && key_len)
        {
          *key     = tz_ks_iter_get_entry_info(&iter)->msg;
          *key_len = tz_ks_iter_get_entry_info(&iter)->msg_len;
        }

        return E_SUCCESS;
      }
    }
  }

  TZBSP_ERROR_CODE(KS_GET_KEY_NOT_FOUND);
  return -E_FAILURE;
}

/**
   @copydoc tz_ks_encrypt
 */
int tz_ks_encrypt(uint8* keystore, uint32 keystore_len)
{
  ks_iter_t iter;
  int retval = E_SUCCESS;

  /* This malloc can use up to 8 KB, but it should only be used in a
     factory environment and by fastboot, so not many other runtime TZ
     features should be using heap at this time. */
  ks_entry_info_t* s_ks_ptr = (ks_entry_info_t*) tzbsp_malloc(keystore_len);

  if (!s_ks_ptr)
  {
    TZBSP_ERROR_CODE(KS_ENC_MALLOC);
    return -E_FAILURE;
  }

  memcpy(s_ks_ptr, keystore, keystore_len);

  tz_ks_iter_init(&iter, s_ks_ptr);

  /* Two keys are the minimum for current use cases */
  if (((ks_dir_t*) s_ks_ptr->msg)->num_keys < 2)
  {
    TZBSP_ERROR_CODE(KS_ENC_KEYNUM);
    return -E_NO_DATA;
  }

  retval = tz_ks_iter_validate_ks_len(&iter, keystore_len);

  if (E_SUCCESS != retval)
  {
    TZBSP_ERROR_CODE(KS_ENC_VAL_LEN);
    tzbsp_free(s_ks_ptr);
    return -E_OUT_OF_RANGE;
  }

  /* Encrypt the directory last because it contains the key entrys'
     metadata and IDs. */
  while (E_SUCCESS == tz_ks_iter_next_entry(&iter))
  {
    if (E_SUCCESS != tz_ks_iter_encrypt_auth_entry(&iter))
    {
      TZBSP_ERROR_CODE(KS_ENC_AUTH_KEY);
      tzbsp_free(s_ks_ptr);
      return -E_FAILURE;
    }
  }

  /* Re-init iter to reset position to the ks directory */
  tz_ks_iter_init(&iter, s_ks_ptr);

  /* encrypt directory */
  if (E_SUCCESS != tz_ks_iter_encrypt_auth_entry(&iter))
  {
    TZBSP_ERROR_CODE(KS_ENC_AUTH_DIR);
    tzbsp_free(s_ks_ptr);
    return -E_FAILURE;
  }

  memcpy(keystore, s_ks_ptr, keystore_len);

  tzbsp_free(s_ks_ptr);

  return E_SUCCESS;
}

/**
   @copydoc tz_ks_init_and_decrypt
 */
int tz_ks_init_and_decrypt(const uint32* keystore_ptr, uint32 keystore_len)
{
  ks_iter_t iter;

  g_ks_len = keystore_len;

  /* Now we can use g_ks_len */

  /* Copy the keystore to secure memory to prevent TOCTOU */
  memcpy(g_ks_ptr, keystore_ptr, g_ks_len);

  memset((void *) ((uint32) g_ks_ptr + keystore_len), 0,
         TZBSP_SSD_REGION_SIZE - g_ks_len);

  /* Now we can use g_ks_ptr */

  /* Must start by validating the length of only the first entry: the
     keystore directory. */
  if (E_SUCCESS != tz_ks_validate_entry_len(g_ks_ptr, g_ks_len))
  {
    g_ks_len = 0;
    TZBSP_ERROR_CODE(KS_IAD_DIR_LEN);
    return -E_OUT_OF_RANGE;
  }

  /* Now decrypt the keystore directory */
  if (E_SUCCESS != tz_ks_decrypt_auth_entry(g_ks_ptr, NULL, KS_SSD_INVALID_MD))
  {
    g_ks_len = 0;
    TZBSP_ERROR_CODE(KS_IAD_DEC_AUTH_DIR);
    return -E_FAILURE;
  }

  /* Two keys are the minimum for current use cases */
  if (((ks_dir_t*) g_ks_ptr->msg)->num_keys < 2)
  {
    g_ks_len = 0;
    TZBSP_ERROR_CODE(KS_IAD_NUM_KEYS);
    return -E_NO_DATA;
  }

  /* Now that the keystore is decrypted, an iterator may be used and
     full keystore length validation will work. */
  tz_ks_iter_init(&iter, g_ks_ptr);

  if (E_SUCCESS != tz_ks_iter_validate_ks_len(&iter, g_ks_len))
  {
    g_ks_len = 0;
    TZBSP_ERROR_CODE(KS_IAD_KS_LEN);
    return -E_OUT_OF_RANGE;
  }

  /* Skip the first entry (directory), since it was decrypted first. */
  while (E_SUCCESS == tz_ks_iter_next_entry(&iter))
  {
    if (E_SUCCESS != tz_ks_decrypt_auth_entry(tz_ks_iter_get_entry_info(&iter),
                                              tz_ks_iter_get_entry_id(&iter),
                                              tz_ks_iter_get_entry_md(&iter)))
    {
      g_ks_len = 0;
      TZBSP_ERROR_CODE(KS_IAD_DEC_AUTH_KEY);
      return -E_FAILURE;
    }
  }

  return E_SUCCESS;
}

/**
   @copydoc tz_ks_get_dir_size
 */
uint32 tz_ks_get_dir_entry_size(void)
{
  /* The minimum sized keystore will have two keys for SSD. */
  return tz_ks_get_entry_overhead_size() + sizeof(ks_dir_t) +
    MIN_DIR_ENTRIES * sizeof(ks_dir_key_info_t);
}

/**
   @copydoc tz_ks_get_entry_overhead_size
 */
uint32 tz_ks_get_entry_overhead_size(void)
{
  return sizeof(ks_entry_info_t);
}

/**
   @copydoc tz_ks_get_max_ks_len
 */
uint32 tz_ks_get_max_ks_len(void)
{
  return TZBSP_SSD_REGION_SIZE;
}


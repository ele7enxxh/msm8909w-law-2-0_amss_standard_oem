#ifndef TZ_KS_H
#define TZ_KS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      TrustZone Keystore

General Description
  Implement a keystore used for SSD

Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/services/ssd/inc/tz_ks.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when         who     what, where, why
--------     ---     -------------------------------------------------------
01/30/13     pre     Initial Revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "tzbsp_hash.h"


/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/

typedef enum
{
  KS_SSD_INVALID_MD       = 0,   /**< This may not be used */

  KS_SSD_OEM_PUB_TEST_KEY = 1,
  KS_SSD_OEM_PUB_KEY      = 2,

  KS_SSD_DVC_PRI_TEST_KEY = 3,
  KS_SSD_DVC_PRI_KEY      = 4,

  KS_SSD_DVC_AES_TEST_KEY = 5,
  KS_SSD_DVC_AES_KEY      = 6,

  KS_ENT_MD_MAX_USAGE     = 0x7FFFFFFF
} ks_ent_md_t;

typedef struct
{
  uint8 auth_tag[TZBSP_SHA256_HASH_SZ];
} ks_auth_tag_t;

typedef struct
{
  uint8 key_id[TZBSP_SHA256_HASH_SZ];
} ks_ent_id_t;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/**
   Retrieves a key from the keystore given the key's ID and metadata.
   Alternatively, this function may be called with NULL output
   parameters to query whether a key exists in the keystore.

   @param[out] key      The key, if found
   @param[out] key_len  The length of the key

   @return  E_SUCCESS    On success
           -E_FAILURE    Key with given ID and MD does not exist
                         in keystore

   @pre This function may only be called after the keystore has been
        initialized using tz_ks_init_and_decrypt
   @pre id parameter must point to a valid-length key id
 */
int tz_ks_get_key(const ks_ent_id_t*  id,
                  ks_ent_md_t         md,
                  const uint8**       key,
                  uint32*             key_len);

/**
   Take a pointer to memory that is a keystore and encrypt it in
   place.  This is meant to be used in the factory environment.

   @todo Consider only allowing this API based on a fuse value

   @return  E_SUCCESS  On success
           -E_FAILURE All failures

   @pre keystore_len must be at least the size of the keystore header

   @pre keystore input pointer and length do not fall into any secure
        memory area, is less than the SSD region size in DDR and will
        not cause an overflow
 */
int tz_ks_encrypt(uint8* keystore, uint32 keystore_len);

/**
   Initializes the TrustZone keystore to the given pointer.  Part of
   the initialization is decryption and authentication of the keys.

   @return  E_SUCCESS      On success
           -E_NO_DATA      If the keystore has no keys
           -E_OUT_OF_RANGE If there is a key whose length is out of the
                           given keystore length or causes an overflow
           -E_FAILURE      All other failures

   @pre Fixed TrustZone keystore area in DDR must be XPU protected
        before calling into this function

   @pre keystore_len must be at least the size of the keystore header

   @pre keystore input pointer and length do not fall into any secure
        memory area, is less than the SSD region size in DDR and will
        not cause an overflow
 */
int tz_ks_init_and_decrypt(const uint32* keystore_ptr, uint32 keystore_len);

/**
   Returns the size of the keystore directory entry in the keystore.
   The keystore directory is currently a fixed length, so this does
   not require a keystore to be initialized.
 */
uint32 tz_ks_get_dir_entry_size(void);

/**
   Returns the size of the data that must accompany the raw data in
   the keystore
 */
uint32 tz_ks_get_entry_overhead_size(void);

/**
   Returns the maximum supported size for any keystore
 */
uint32 tz_ks_get_max_ks_len(void);

#endif /* #define TZ_KS_H */

#ifndef KS_H
#define KS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      TrustZone Keystore

General Description
  Implement generic keystore functionality used within TZ

Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/services/ks/inc/ks.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when         who     what, where, why
--------     ---     -------------------------------------------------------
04/05/13     pre     Initial Revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "tzbsp_hash.h"
#include "tzbsp_cipher.h"

/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/
#define KS_DEFAULT_DIR_ENTRIES 10
#define KS_LEGACY_NAME         "SSD"

typedef enum
{
  KS_DIR_MD           = 0,

  KS_FREE_MD_VAL      = 1,

  KS_MD_MAX_USAGE     = 0x7FFFFFFF
} ks_ent_md_t;

typedef struct
{
  uint8 auth_tag[TZBSP_SHA256_HASH_SZ];
} ks_auth_tag_t;

typedef struct
{
  uint8 usr_pwd[32];
} ks_ent_pwd_t;

typedef struct
{
  uint8 key_id[TZBSP_SHA256_HASH_SZ];
} ks_ent_id_t;

typedef tz_aes_128_key_t ks_aes_128_key_t;

typedef struct
{
  uint8*      ks_ptr;
  uint32      ks_len;
  uint32      max_ks_len;
  const char* ks_name;
  boolean     b_data_initialized;
} ks_obj_t;

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

   @pre The keystore passed to this function must be a valid keystore.
   @pre id parameter must point to a valid-length key id
 */
int ks_get_key(const ks_obj_t*    ks_obj_ptr,
               const ks_ent_id_t* id,
               const ks_ent_md_t* md,
               const uint8**      key,
               uint32*            key_len);

/**
   Adds a new entry to the given keystore.

   @param[in/out] ks_obj_ptr       The keystore to add a key to.
   @param[in]     key              The key to add to the keystore.
   @param[in]     key_len          The length of the key to be added.
   @param[in]     id               The ID for the key to add.
   @param[in]     md               The MD for the key to add.

   @return  E_SUCCESS    On success
           -E_FAILURE    Key with given ID and MD already exists
                         in keystore or there is not enough space to add

   @pre The keystore passed to this function must be a valid keystore.
   @pre id parameter must point to a valid-length key id

   @warn Requires keystore that supports RB protection
 */
int ks_add_key
(
  ks_obj_t*           ks_obj_ptr,
  const uint8*        key,
  uint32              key_len,
  const ks_ent_id_t*  id,
  const ks_ent_md_t*  md
);

/**
   Removes an entry from the given keystore.

   @param[in/out] ks_obj_ptr       The keystore to remove a key from.
   @param[in]     id               The ID for the key to remove.
   @param[in]     md               The MD for the key to remove.

   @return  E_SUCCESS    On success
           -E_FAILURE    Key with given ID and MD does not exist
                         in keystore.

   @pre The keystore passed to this function must be a valid keystore.
   @pre id parameter must point to a valid-length key id.
   @pre md parameter must point to a valid-length key md.

   @warn Requires keystore that supports RB protection
 */
int ks_remove_key
(
  ks_obj_t*          ks_obj_ptr,
  const ks_ent_id_t* id,
  const ks_ent_md_t* md
);

/**
   Take a pointer to memory that is a keystore and encrypt it in
   place.  This is meant to be used in the factory environment.

   @todo Consider only allowing this API based on a fuse value

   @return  E_SUCCESS  On success
           -E_FAILURE All failures

   @pre keystore_len must be at least the size of the keystore header

   @pre keystore input pointer and length do not fall into any secure
        memory area, is less than the QSEE storage region size in DDR
        and will not cause an overflow
 */
int ks_encrypt(uint8* keystore, uint32 keystore_len, uint32 version);

/**
   Initializes the TrustZone keystore to the given pointer.  Part of
   the initialization is decryption and authentication of the keys.

   @sa Also updates the keystore length to the length of all the
       entries in the keystore.

   @return  E_SUCCESS      On success
           -E_NO_DATA      If the keystore is uninitialized.  This is not
                           the same thing as an empty keystore.
           -E_OUT_OF_RANGE If the contents of the keystore exceed the
                           length of the keystore or if there is a key
                           with a length that's invalid because it is too
                           large and causes an overflow or invalid because
                           it's zero.
           -E_FAILURE      All other failures

   @pre Fixed TrustZone keystore area in DDR must be XPU protected
        before calling into this function

   @pre keystore_len must be at least the size of the keystore header

   @pre keystore input pointer and length do not fall into any secure
        memory area, is less than the QSEE storage region size in DDR
        and will not cause an overflow
 */
int ks_decrypt(ks_obj_t* ks_obj_ptr, uint32 version);

/**
   Returns the minimum size of the keystore directory entry in the
   keystore.  The keystore directory must currently be a minimum
   length, so this does not require a keystore.
 */
uint32 ks_get_min_dir_entry_size(void);

/**
   Returns the size of the data that must accompany the raw data in a
   keystore.
 */
uint32 ks_get_entry_overhead_size(void);

/**
   Generates an empty directory for a given keystore.  Will update the
   keystore's length to reflect the length of the default empty
   keystore.

   @warn Requires keystore that supports RB protection
 */
int ks_create_empty_ks(ks_obj_t* ks_obj_ptr);

/**
   Returns whether or not the given keystore has been initialized.
 */
boolean ks_is_init(const ks_obj_t* ks_obj_ptr);

/**
   Returns the current length for a given keystore object
 */
uint32 ks_get_len(const ks_obj_t* ks_obj_ptr);

/**
   Returns the maximum length for a given keystore object
 */
uint32 ks_get_max_len(const ks_obj_t* ks_obj_ptr);

/**
   Initializes a keystore object.
 */
void ks_init(ks_obj_t*   ks_obj_ptr,
             uint8*      ks_ptr,
             uint32      ks_len,
             uint32      max_ks_len,
             const char* ks_name);

/**
   Deinitializes a keystore object
 */
void ks_deinit(ks_obj_t* ks_obj_ptr);

/**
   Copies keystore data into a keystore object.  The amount of data
   copied from src is determined by the current keystore's size.  The
   difference between the maximum keystore size and the current
   keystore size will be zeroed.

   @pre length validation of src must be done outside of function
 */
void* ks_copy_in(ks_obj_t* ks_obj_ptr, const void* src);

/**
   Copies keystore data out of a keystore object.  The amount of data
   copied to dst is determined by the current keystore's size.

   @pre length validation of dst must be done outside of function
 */
void* ks_copy_out(ks_obj_t* ks_obj_ptr, void* dst);


#endif /* #ifndef KS_H */

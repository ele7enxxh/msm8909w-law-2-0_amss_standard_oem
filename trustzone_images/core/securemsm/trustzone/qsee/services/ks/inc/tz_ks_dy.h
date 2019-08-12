#ifndef TZ_KS_DY_H
#define TZ_KS_DY_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      TrustZone Keystore

General Description
  This is an extension of the generic keystore defined in ks.h/ks.c

Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/services/ks/inc/tz_ks_dy.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when         who     what, where, why
--------     ---     -------------------------------------------------------
01/30/13     pre     Initial Revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ks.h"
#include "tz_ks_common.h"
#include "IxErrno.h"

/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/

#define SET_KEY_USAGE    1
#define UPDATE_KEY_USAGE 2


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/**
   Returns the QSEE keystore
 */
ks_obj_t* tz_ks_dy_get_ks(void);

/**
   Initializes the QSEE keystore object.  This call must be made prior
   to decrypting the keystore.  The length of the keystore may be
   updated after decrypting.
 */
int tz_ks_dy_copy_init(uint32 dy_ks_off);

/**
   Initializes the QSEE keystore object.  This call must be made prior
   to decrypting the keystore.  The length of the keystore may be
   updated after decrypting.
 */
int tz_ks_dy_init(uint8* dy_ks_ptr, uint32 dy_ks_len, uint32 max_dy_ks_len);

/**
   Adds a key to the dynamic keystore
 */
int tz_ks_dy_add_key(const uint8*          key,
                     uint32                key_len,
                     const ks_ent_id_t*    id,
                     const ks_ent_pwd_t*   user_pwd,
                     const tz_ks_ent_md_t* md);

/**
   Removes a key from the dynamic keystore
 */
int tz_ks_dy_remove_key(const ks_ent_id_t*    id,
                        const tz_ks_ent_md_t* md);

/**
   Gets the current keystore version
 */
uint32 tz_ks_dy_get_version(void);

/**
   Loads the keystore version in RPMB for rollback protection
 */
int tz_ks_dy_load_version_from_rpmb(void);

/**
   Saves the keystore version in RPMB for rollback protection
 */
int tz_ks_dy_save_version_in_rpmb(void);

/**
   @copydoc ks_get_key
 */
int tz_ks_dy_get_key(const ks_ent_id_t*    id,
                     const ks_ent_pwd_t*   user_pwd,
                                   const tz_ks_ent_md_t* md,
                     uint8*                key,
                     uint32*               key_len,
                     const uint32          usage);

/**
   @copydoc ks_decrypt
 */
static inline int tz_ks_dy_decrypt(void)
{
  return ks_decrypt(tz_ks_dy_get_ks(), tz_ks_dy_get_version());
}

/**
   @copydoc ks_create_empty_ks
 */
static inline int tz_ks_dy_create_empty_ks(void)
{
  return ks_create_empty_ks(tz_ks_dy_get_ks());
}

/**
   Deinitializes the QSEE keystore data, but leaves rollback info untouched
 */
static inline void tz_ks_dy_deinit(void)
{
  ks_deinit(tz_ks_dy_get_ks());
}

static inline boolean tz_ks_dy_is_init(void)
{
  return ks_is_init(tz_ks_dy_get_ks());
}

static inline uint32 tz_ks_dy_get_len(void)
{
  return ks_get_len(tz_ks_dy_get_ks());
}

int tz_ks_dy_update_key(const ks_ent_id_t*    id,
                        const ks_ent_pwd_t*   pwd_curr,
                        const ks_ent_pwd_t*   pwd_new,
                        const tz_ks_ent_md_t* md);


#endif /* #define TZ_KS_DY_H */

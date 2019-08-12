#ifndef TZ_KS_PP_H
#define TZ_KS_PP_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      TrustZone Keystore

General Description
  This is an extension of the generic keystore defined in ks.h/ks.c

Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/services/ks/inc/tz_ks_pp.h#1 $
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

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/**
   Returns the QSEE keystore
 */
ks_obj_t* tz_ks_pp_get_ks(void);

/**
   Copies what's in the current dynamic keystore object into TZ's
   pre-defined region for keystores and reinitializes the keystore
   object to use that copy.
 */
int tz_ks_pp_copy_init(void);

/**
   Initializes the QSEE pre-provisioned keystore object.  This call
   must be made prior to decrypting the keystore.  The length of the
   keystore may be decreased after decrypting.
 */
void tz_ks_pp_init(uint8* ks_ptr, uint32 ks_len);

/**
   Initializes the QSEE keystore object.  This call must be made prior
   to decrypting the keystore.  The length of the keystore may be
   decreased after decrypting.
 */
int tz_ks_pp_copy_init(void);

/**
   @copydoc ks_get_key
 */
static inline int tz_ks_pp_get_key(const ks_ent_id_t*    id,
                                   const tz_ks_ent_md_t* md,
                                   const uint8**         key,
                                   uint32*               key_len)
{
  return ks_get_key(tz_ks_pp_get_ks(),
                    id, (const ks_ent_md_t*) md,
                    key, key_len);
}

/**
   @copydoc ks_decrypt
 */
static inline int tz_ks_pp_decrypt(void)
{
  /* pre-provisioned keystore uses version zero since it does not
     support rollback-protection */
  return ks_decrypt(tz_ks_pp_get_ks(), 0);
}

static inline void tz_ks_pp_init(uint8* ks_ptr, uint32 ks_len)
{
  /* This keystore must be initialized with the legacy keystore name
     for backward compatability with previously provisioned
     SSD keystores. */
  ks_init(tz_ks_pp_get_ks(), ks_ptr, ks_len, ks_len, KS_LEGACY_NAME);
}

/**
   Deinitializes the pre-provisioned keystore.
 */
static inline void tz_ks_pp_deinit(void)
{
  ks_deinit(tz_ks_pp_get_ks());
}

static inline boolean tz_ks_pp_is_init(void)
{
  return ks_is_init(tz_ks_pp_get_ks());
}

static inline uint32 tz_ks_pp_get_len(void)
{
  return ks_get_len(tz_ks_pp_get_ks());
}

#endif /* #define TZ_KS_PP_H */

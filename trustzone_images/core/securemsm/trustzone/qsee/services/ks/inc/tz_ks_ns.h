#ifndef TZ_KS_NS_H
#define TZ_KS_NS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      TrustZone Keystore

General Description
  These are the APIs to access the QSEE extension to the generic keystore
  defined in ks.h/ks.c

Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/services/ks/inc/tz_ks_ns.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when         who     what, where, why
--------     ---     -------------------------------------------------------
05/02/13     pre     Initial Revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ks.h"

/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/



/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/**
   Adds a randomly generated key to QSEE storage thats value will not be
   exposed back to the non-secure side.

   @param flags        TBD
   @param key_id_ptr   Key ID data used to access the generated key in the
                       future. Associate with a specific usecase
   @param user_pwd_ptr User password to associate with the use case

   @retval E_SUCCESS                   on success
           QSEE_RESULT_FAIL_LOAD_KS    if the keystore could not be loaded
                                       from EMMC or if the version could
                                       not be verified from RPMB
           QSEE_RESULT_FAIL_KS_OP      if the keystore operation failed
           QSEE_RESULT_FAIL_SAVE_KS    if the keystore update isn't saved
*/
int tz_ks_ns_generate_key_ns(uint32 flags, ks_ent_id_t* key_id_ptr, ks_ent_pwd_t* user_pwd_ptr);

/**
   Removes a key from QSEE storage

   @param flags        TBD
   @param key_id_ptr   Key ID data used to access the generated 
                       key in the future. Associate with a
                       specific usecase
   @param user_pwd_ptr User password to associate with the use 
                       case(ignored, added for future)

   @retval E_SUCCESS                   on success
           QSEE_RESULT_FAIL_LOAD_KS    if the keystore could not be loaded
                                       from EMMC or if the version could
                                       not be verified from RPMB
           QSEE_RESULT_FAIL_KEY_ID_DNE if the key did not exist in the
                                       keystore
           QSEE_RESULT_FAIL_KS_OP      if the keystore operation failed
           QSEE_RESULT_FAIL_SAVE_KS    if the keystore update isn't saved
*/
int tz_ks_ns_delete_key_ns(uint32 flags, ks_ent_id_t* key_id_ptr, ks_ent_pwd_t* user_pwd_ptr);

/**
   Returns the maximum number of keys supported by QSEE.

   @retval E_SUCCESS always
 */
int tz_ks_ns_max_key_count_query_ns(uint32 flags, uint32* rsp);

/** 
   Replaces the existing key in keystore pointed by current key 
   id and replaces with key created using new key id 

   @param flags                TBD 
   @param key_id_ptr            Key ID data used to access the 
                                generated key in the future.
                                Associate
                                with a specific usecase
   @param user_pwd_ptr_current  User password to associated with 
                                the use case
    
   @param user_pwd_ptr_new      New User password to associate 
                                with the use case

   @retval E_SUCCESS                   on success
           QSEE_RESULT_FAIL_LOAD_KS    if the keystore could not be loaded
                                       from EMMC or if the version could
                                       not be verified from RPMB
           QSEE_RESULT_FAIL_KS_OP      if the keystore operation failed
           QSEE_RESULT_FAIL_SAVE_KS    if the keystore update isn't saved
    
           QSEE_RESULT_FAIL_KEY_ID_DNE if the key did not exist in the
                                       keystore
           QSEE_RESULT_FAIL_SAVE_KS    if the keystore update isn't saved
*/
int tz_ks_ns_update_key_ns(uint32 flags, ks_ent_id_t* key_id_ptr, 
                           ks_ent_pwd_t* user_pwd_ptr_current, ks_ent_pwd_t* user_pwd_ptr_new);

#endif /* #define TZ_KS_NS_H */

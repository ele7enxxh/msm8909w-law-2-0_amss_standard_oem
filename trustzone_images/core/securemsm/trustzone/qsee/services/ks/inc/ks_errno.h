#ifndef KS_ERRNO_H
#define KS_ERRNO_H

/**
   @file ks_errno.h
   @brief extend TZ error codes
*/

/*===========================================================================
   Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/services/ks/inc/ks_errno.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
05/08/13   pre      Initial version.

===========================================================================*/


/*===========================================================================

                              DEFINES

===========================================================================*/

#include "tz_ssd_errno.h"

#define KS_B TZ_EC_SSD_MAX_ERRNO /* 600 */

enum
{
  /* *************************************************** */
  /* ks.c */
  /* *************************************************** */
  /* ks_generate_ks_key */
  TZBSP_EC_KS_GKSK_STLCPY               = KS_B,
  TZBSP_EC_KS_GKSK_STLCAT,
  TZBSP_EC_KS_GKSK_KDF,

  /* ks_generate_hmac_key */
  TZBSP_EC_KS_GHMACK_STLCPY             = KS_B + 10,
  TZBSP_EC_KS_GHMACK_STLCAT,
  TZBSP_EC_KS_GHMACK_KDF,

  /* ks_validate_entry_len */
  TZBSP_EC_KS_VEL_ZERO_LEN_ENTRY        = KS_B + 20,
  TZBSP_EC_KS_VEL_INVALID,
  TZBSP_EC_KS_VEL_BOUNDS,

  /* ks_enc_dec_entry */
  TZBSP_EC_KS_EDE_CIPHER_INIT           = KS_B + 30,
  TZBSP_EC_KS_EDE_MODE,
  TZBSP_EC_KS_EDE_IV,
  TZBSP_EC_KS_EDE_KEY,
  TZBSP_EC_KS_EDE_CIPHER_OP,
  TZBSP_EC_KS_EDE_OUT_LEN,

  /* ks_auth_entry */
  TZBSP_EC_KS_AE_MALLOC                 = KS_B + 40,
  TZBSP_EC_KS_AE_HMAC,

  /* ks_decrypt_auth_entry */
  TZBSP_EC_KS_DAE_EM_VER                = KS_B + 50,
  TZBSP_EC_KS_DAE_AUTH,
  TZBSP_EC_KS_DAE_AUTH_CMP,
  TZBSP_EC_KS_DAE_CIPHER,

  /* ks_iter_validate_ks_len */
  TZBSP_EC_KS_IVKSL_DIR_ENT_LEN         = KS_B + 60,
  TZBSP_EC_KS_IVKSL_MIN_DIR_LEN,
  TZBSP_EC_KS_IVKSL_ENT_LEN,

  /* ks_iter_encrypt_auth_entry */
  TZBSP_EC_KS_EAE_EM_VER                = KS_B + 70,
  TZBSP_EC_KS_EAE_PRNG,
  TZBSP_EC_KS_EAE_CIPHER,
  TZBSP_EC_KS_EAE_AUTH,

  /* ks_find_key */
  TZBSP_EC_KS_FK_NO_KEYS                = KS_B + 80,
  TZBSP_EC_KS_FK_NOT_FOUND,

  /* ks_check_dir_vacancy */
  TZBSP_EC_KS_CDV_NO_VACANCY            = KS_B + 90,

  /* ks_get_key */
  TZBSP_EC_KS_GET_KEY_NO_KS_PROVISIONED = KS_B + 100,
  TZBSP_EC_KS_GET_KEY_NOT_FOUND,

  /* ks_add_key / ks_remove_key */
  TZBSP_EC_KS_ADD_KEY_OBJ_NOT_INIT      = KS_B + 110,
  TZBSP_EC_KS_ADD_KEY_ALREADY_EXISTS,
  TZBSP_EC_KS_ADD_KEY_INSUFFICIENT_SPACE,
  TZBSP_EC_KS_ADD_KEY_NO_DIR_VACANCY,
  TZBSP_EC_KS_REM_KEY_KS_UNINIT,
  TZBSP_EC_KS_REM_KEY_KEY_NOT_FOUND,

  /* ks_encrypt */
  TZBSP_EC_KS_ENC_VAL_LEN               = KS_B + 120,
  TZBSP_EC_KS_ENC_GEN_KS_KEY,
  TZBSP_EC_KS_ENC_GEN_HMAC_KEY,
  TZBSP_EC_KS_ENC_AUTH_KEY,
  TZBSP_EC_KS_ENC_AUTH_DIR,

  /* ks_decrypt */
  TZBSP_EC_KS_D_KS_UNINIT               = KS_B + 130,
  TZBSP_EC_KS_D_DIR_LEN,
  TZBSP_EC_KS_D_GEN_KS_KEY,
  TZBSP_EC_KS_D_GEN_HMAC_KEY,
  TZBSP_EC_KS_D_DEC_AUTH_DIR,
  TZBSP_EC_KS_D_KS_LEN,
  TZBSP_EC_KS_D_DEC_AUTH_KEY,

  /* ks_create_empty_ks */
  TZBSP_EC_KS_CEK_NULL_PTR              = KS_B + 140,

  /* *************************************************** */
  /* tz_ks_common.c */
  /* *************************************************** */
  /* tz_ks_save_in_emmc */
  TZBSP_EC_TZ_KS_SIE_BUFSZ              = KS_B + 150,
  TZBSP_EC_TZ_KS_SIE_NO_LISTENER_BUF,
  TZBSP_EC_TZ_KS_SIE_ENCRYPT_PP_FAILED,
  TZBSP_EC_TZ_KS_SIE_ENCRYPT_DY_FAILED,
  TZBSP_EC_TZ_KS_SIE_LISTENER_FAILED,

  /* tz_ks_load_from_emmc */
  TZBSP_EC_TZ_KS_LFE_BUFSZ              = KS_B + 160,
  TZBSP_EC_TZ_KS_LFE_NO_LISTENER_BUF,
  TZBSP_EC_TZ_KS_LFE_LISTENER_FAILURE,
  TZBSP_EC_TZ_KS_LFE_KS_INIT_DY_FAILED,
  TZBSP_EC_TZ_KS_ADD_KEY_DIR_CREATE_FAILED,

  /* tz_ks_load */
  TZBSP_EC_TZ_KS_LOAD_VERSION_FAILED    = KS_B + 170,
  TZBSP_EC_TZ_KS_LOAD_DATA_FAILED,

  /* tz_ks_save */
  TZBSP_EC_TZ_KS_SAVE_VERSION_FAILED    = KS_B + 180,
  TZBSP_EC_TZ_KS_SAVE_DATA_FAILED,


  /* *************************************************** */
  /* tz_ks_pp.c and tz_ks_dy.c */
  /* *************************************************** */
  /* tz_ks_pp_init and tz_ks_dy_init */
  TZBSP_EC_TZ_KS_PP_INIT_INVALID_LEN    = KS_B + 190,
  TZBSP_EC_TZ_KS_DY_INIT_INVALID_LEN,

  /* tz_ks_dy_save_version_in_rpmb and tz_ks_dy_load_version_from_rpmb */
  TZBSP_EC_TZ_KS_SVR_SAVE_FAILED        = KS_B + 200,
  TZBSP_EC_TZ_KS_SVR_VER_UNINIT,
  TZBSP_EC_TZ_KS_LVR_LOAD_FAILED,

  /* tz_ks_dy_remove_key / tz_ks_dy_add_key / tz_ks_dy_update_key*/
  TZBSP_EC_TZ_KS_REM_KEY_VER_MAXED      = KS_B + 210,
  TZBSP_EC_TZ_KS_REM_KEY_NO_EXIST,
  TZBSP_EC_TZ_KS_ADD_KEY_VER_MAXED,
  TZBSP_EC_TZ_KS_INSUFF_MEM_MD,
  TZBSP_EC_TZ_KS_INSUFF_MEM_MD_MALLOC,
  TZBSP_EC_TZ_KS_GEN_KEY_ALREADY_DONE,
  TZBSP_EC_TZ_KS_GEN_KEY_HASH_FAIL,
  TZBSP_EC_TZ_KS_GEN_SALT_FAILED,
  TZBSP_EC_TZ_KS_GEN_HMAC_FAILED,
  TZBSP_EC_TZ_KS_GEN_IV_FAILED,
  TZBSP_EC_TZ_KS_GCM_ENC_FAIL,
  TZBSP_EC_TZ_KS_UPDATE_ADD_FAIL,
  TZBSP_EC_TZ_KS_UPDATE_GET_KEY_FAIL,
  TZBSP_EC_TZ_KS_MAX_ATTEMPT_REM_FAIL,
  TZBSP_EC_TZ_KS_MAX_ATTEMPT,

  /* *************************************************** */
  /* ks_ns.c */
  /* *************************************************** */
  /* ks_ns_encrypt_keystore_ns */
  TZBSP_EC_KS_NS_EKSNS_OVERFLOW         = KS_B + 230,
  TZBSP_EC_KS_NS_EKSNS_SPTR,
  TZBSP_EC_KS_NS_EKSNS_ALIGN_OVERFLOW,
  TZBSP_EC_KS_NS_EKSNS_SPTR2,
  TZBSP_EC_KS_NS_EKSNS_NO_HDR,
  TZBSP_EC_KS_NS_EKSNS_KS_LEN,
  TZBSP_EC_KS_NS_EKSNS_MALLOC,
  TZBSP_EC_KS_NS_EKSNS_ENC_FAILED,

  /* *************************************************** */
  /* tz_ks_ns.c */
  /* *************************************************** */
  /* tz_ks_ns_generate_key_ns */
  TZBSP_EC_TZ_KS_NS_GEN_KEY_LOAD_FAILED = KS_B + 240,
  TZBSP_EC_TZ_KS_NS_GEN_KEY_FAILED,
  TZBSP_EC_TZ_KS_NS_GEN_KEY_EXISTS,
  TZBSP_EC_TZ_KS_NS_GEN_KEY_MAX_KEYS,
  TZBSP_EC_TZ_KS_NS_GEN_KEY_NO_SPACE,
  TZBSP_EC_TZ_KS_NS_GEN_KEY_ADD_KEY_FAILED,
  TZBSP_EC_TZ_KS_NS_GEN_KEY_SAVE_FAILED,

  /* tz_ks_ns_delete_key_ns */
  TZBSP_EC_TZ_KS_NS_DEL_KEY_LOAD_FAILED = KS_B + 250,
  TZBSP_EC_TZ_KS_NS_DEL_KEY_NOT_FOUND,
  TZBSP_EC_TZ_KS_NS_DEL_KEY_FAILED,
  TZBSP_EC_TZ_KS_NS_DEL_KEY_SAVE_FAILED,

  /* tz_ks_ns_update_key_ns */
  TZBSP_EC_TZ_KS_NS_UP_KEY_LOAD_FAILED = KS_B + 260,
  TZBSP_EC_TZ_KS_NS_UP_KEY_FAILED,
  TZBSP_EC_TZ_KS_NS_UP_KEY_SAVE_FAILED,

  /* tz_ce_gen_random_key_for_pipe */
  TZBSP_EC_TZ_KS_CE_GEN_RKEY_MALLOC_FAIL  = KS_B + 290,
  TZBSP_EC_TZ_KS_CE_GEN_RKEY_PRNG_FAIL,
};

#endif

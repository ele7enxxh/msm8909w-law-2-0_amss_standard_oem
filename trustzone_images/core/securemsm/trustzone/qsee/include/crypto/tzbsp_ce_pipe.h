#ifndef TZBSP_CE_PIPE_H
#define TZBSP_CE_PIPE_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      TrustZone Keystore

General Description
  This file uses the QSEE keystore to select a key for a given key ID
  and place it into a crypto engine pipe register for use by a NS client.

Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/crypto/tzbsp_ce_pipe.h#1 $
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
#define QSEE_NS_KEY_SELECT_USE_HASH   0x1

/* TODO: target dependent */
typedef enum
{
  TZ_PIPE_ENC            = 0,
  TZ_PIPE_ENC_XTS        = 1,
  TZ_PIPE_AUTH           = 2,
  TZ_PIPE_NUM_PIPE_DESCS = 3,
  TZ_PIPE_ENUM_FILL      = 0x7FFFFFFF
} tz_pipe_desc_t;

typedef enum
{
  TZ_PIPE_NONE_MSK      = 0,
  TZ_PIPE_ENC_MSK       = 1 << TZ_PIPE_ENC,
  TZ_PIPE_ENC_XTS_MSK   = 1 << TZ_PIPE_ENC_XTS,
  TZ_PIPE_AUTH_MSK      = 1 << TZ_PIPE_AUTH,
  TZ_PIPE_MSK_ENUM_FILL = 0x7FFFFFFF
} tz_pipe_desc_msk_t;

typedef struct
{
  ks_ent_id_t usr_key;
  ks_ent_pwd_t usr_pwd;
} tz_pipe_key_param_t;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/**
 */
int tz_ce_pipe_key_select_ns(uint32 ce, uint32 pipe,
                             tz_pipe_desc_msk_t pipe_desc_msk,
                             uint32 flags, tz_pipe_key_param_t* key_info_ptr);

#endif /* #define TZBSP_CE_PIPE_H */

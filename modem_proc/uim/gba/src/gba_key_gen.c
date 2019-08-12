/*===========================================================================


            G B A   K E Y  G E N E R A T I O N   F U N C T I O N S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2015 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/22/16   vdc     Remove F3 messages for memory allocation failure
05/05/15   lxu     Fixed various issues
04/01/15   tkl     Fixed Ks generation
11/31/14   tkl     Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "secapi.h"
#include <stringl/stringl.h>
#include "amssassert.h"

#include "gba.h"
#include "gba_key_gen.h"
#include "gba_platform.h"
#include "gba_util.h"

/*=============================================================================

                       TYPE DECLARATIONS

=============================================================================*/

#define GBA_MAX_NAF_ID_STR_LEN               256
#define GBA_KEY_FC_VAULE                     1    /* Spec 33.220 Annex B */
#define GBA_KEY_P0_GBA_ME_STR                "gba-me"

typedef uint8 gba_naf_id_str_type[GBA_MAX_NAF_ID_STR_LEN];

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/
/*===========================================================================
FUNCTION GBA_KEY_GEN_KS

DESCRIPTION
  This function takes the IK and CK as input, and generates Ks.
  Caller takes care of the memory allocation for Ks pointer.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_key_gen_ks (
  const gba_ik_type         * ikey_ptr,
  const gba_ck_type         * ckey_ptr,
  gba_ks_type               * ks_ptr
)
{
  if(ikey_ptr == NULL || ckey_ptr == NULL || ks_ptr == NULL ||
     (ikey_ptr->data_len + ckey_ptr->data_len) != GBA_KS_LEN)
  {
    return GBA_INCORRECT_PARAMS;
  }

  memset(ks_ptr, 0x00, GBA_KS_LEN);

  /* Spec 33.220 Annex B3. Ks = CK || IK */
  memscpy((uint8 *)(*ks_ptr), GBA_KS_LEN, ckey_ptr->data, ckey_ptr->data_len);
  memscpy((uint8 *)(*ks_ptr) + ckey_ptr->data_len, (GBA_KS_LEN - ckey_ptr->data_len),
           ikey_ptr->data, ikey_ptr->data_len);

  return GBA_SUCCESS;
} /* gba_key_gen_ks */


/*===========================================================================
FUNCTION GBA_KEY_GEN_KSNAF

DESCRIPTION
  This function takes Ks, Rand, IMPI, Naf ID as input, and generates KsNAF.
  Caller takes care of the memory allocation for KsNAF pointer.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_key_gen_ksnaf (
  const gba_ks_type                * ks_ptr,
  const gba_rand_type              * rand_ptr,
  const gba_impi_type              * impi_ptr,
  const gba_naf_id_type            * naf_id_ptr,
  gba_ks_naf_type                  * ksnaf_ptr
)
{
  uint16                      msg_len       = 0;
  uint8                     * msg_ptr       = NULL;
  uint8                     * ks_temp_ptr   = NULL;
  uint16                      str_len       = 0;
  uint16                      offset        = 0;
  secerrno_enum_type          result        = E_SUCCESS;
  secapi_handle_type          hsh_handle    = NULL;
  secapi_hsh_param_data_type  hsh_param;

  if(ks_ptr == NULL || rand_ptr == NULL || impi_ptr == NULL || naf_id_ptr == NULL || ksnaf_ptr == NULL)
  {
    return GBA_INCORRECT_PARAMS;
  }

  ks_temp_ptr = GBA_MALLOC(GBA_KS_LEN);

  /* Follows Spec 33.220
     High Level Function Overview -
     Calcualte S String = FC || P0 || L0 || P1 || L1 || P2 || L2 || P3 || L3
     where FC = 0x01
     L0/L1/L2/L3 are two octet fields containing length of P0/P1/P2/P3
     P0 = gba-me/gba-u
     P1 = RAND
     P2 = IMPI,
     P3 = NAF_Id/FQDN concatenated with security protocol

     For Ks_NAF Ua security protocol = (0x01,0x00,0x00,0x00,0x02) -- HTTP digest authentication. */

  str_len = strlen(GBA_KEY_P0_GBA_ME_STR);
  msg_len = 1 + str_len + 2 + GBA_RAND_LEN + 2 +
           impi_ptr->data_len + 2 + naf_id_ptr->fqdn.data_len + GBA_NAF_SECURITY_PROTOCOL_LEN + 2;
  msg_ptr = GBA_MALLOC(msg_len);

  if(ks_temp_ptr == NULL || msg_ptr == NULL)
  {
    GBA_FREE(ks_temp_ptr);
    GBA_FREE(msg_ptr);
    return GBA_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  memset(msg_ptr, 0x00, msg_len);
  memset(ks_temp_ptr, 0x00, GBA_KS_LEN);
  memset(ksnaf_ptr, 0x00, GBA_KS_NAF_LEN);

  offset = 0;
  msg_ptr[offset++] = GBA_KEY_FC_VAULE;

  /* P0 */
  ASSERT((offset + str_len + 2) <= msg_len);

  memscpy(&msg_ptr[offset], (msg_len - offset) , GBA_KEY_P0_GBA_ME_STR, str_len);
  offset = offset + str_len;
  msg_ptr[offset++] = (uint8)((str_len & 0xFF00) >> 8);
  msg_ptr[offset++] = (uint8)(str_len & 0x00FF);

  /* P1 */
  ASSERT((offset + GBA_RAND_LEN + 2) <= msg_len);

  memscpy(&msg_ptr[offset], (msg_len - offset), (const uint8 *)rand_ptr, GBA_RAND_LEN);
  offset = offset + GBA_RAND_LEN;
  msg_ptr[offset++] = (uint8)((GBA_RAND_LEN & 0xFF00) >> 8);
  msg_ptr[offset++] = (uint8)(GBA_RAND_LEN & 0x00FF);

  /* P2 */
  ASSERT((offset + impi_ptr->data_len + 2) <= msg_len);

  memscpy(&msg_ptr[offset], (msg_len - offset), (const uint8 *)impi_ptr->data, impi_ptr->data_len);
  offset = offset + impi_ptr->data_len;
  msg_ptr[offset++] = (uint8)((impi_ptr->data_len & 0xFF00) >> 8);
  msg_ptr[offset++] = (uint8)(impi_ptr->data_len & 0x00FF);

  /* P3 */
  ASSERT((offset + naf_id_ptr->fqdn.data_len + GBA_NAF_SECURITY_PROTOCOL_LEN + 2) <= msg_len);

  memscpy(&msg_ptr[offset], (msg_len - offset),
          (const uint8 *)naf_id_ptr->fqdn.data, naf_id_ptr->fqdn.data_len);
  offset = offset + naf_id_ptr->fqdn.data_len;
  memscpy(&msg_ptr[offset], (msg_len - offset),
          (const uint8 *)naf_id_ptr->security_protocol, GBA_NAF_SECURITY_PROTOCOL_LEN);
  offset = offset + GBA_NAF_SECURITY_PROTOCOL_LEN;
  str_len = naf_id_ptr->fqdn.data_len + GBA_NAF_SECURITY_PROTOCOL_LEN;
  msg_ptr[offset++] = (uint8)((str_len & 0xFF00) >> 8);
  msg_ptr[offset++] = (uint8)(str_len & 0x00FF);

  memscpy(ks_temp_ptr, GBA_KS_LEN, ks_ptr, GBA_KS_LEN);

  /* setup SHA256 hashing */
  result = secapi_new(&hsh_handle, SECAPI_SHA256);
  if(result != E_SUCCESS)
  {
    GBA_MSG_ERR_1("failed to get handle, result: 0x%x", result);
    GBA_FREE(ks_temp_ptr);
    GBA_FREE(msg_ptr);
    return GBA_GENERIC_ERROR;
  }

  memset(&hsh_param, 0x00, sizeof(secapi_hsh_param_data_type));
  hsh_param.mode = SECAPI_HSH_INPUT_MODE_ATOMIC;
  result = secapi_hsh_set_param(hsh_handle, SECAPI_SHA256,
                                SECAPI_HSH_INPUT_MODE,
                                &hsh_param);
  if(result != E_SUCCESS)
  {
    GBA_MSG_ERR_1("failed to set param, result: 0x%x", result);
    (void)secapi_delete(&hsh_handle);
    GBA_FREE(ks_temp_ptr);
    GBA_FREE(msg_ptr);
    return GBA_GENERIC_ERROR;
  }

  hsh_param.common.exec_platform = SECAPI_EXEC_PLATFORM_ARM;
  result = secapi_hsh_set_param(hsh_handle, SECAPI_SHA256,
                                SECAPI_EXEC_PLATFORM,
                                &hsh_param);

  if(result != E_SUCCESS)
  {
    (void)secapi_delete(&hsh_handle);
    GBA_FREE(ks_temp_ptr);
    GBA_FREE(msg_ptr);
    return GBA_GENERIC_ERROR;
  }

  result = secapi_hsh_create_hmac(hsh_handle,
                                  SECAPI_SHA256,
                                  msg_ptr,
                                  msg_len,
                                  ks_temp_ptr,
                                  GBA_KS_LEN,
                                  (uint8 *)(*ksnaf_ptr));

  GBA_FREE(ks_temp_ptr);
  GBA_FREE(msg_ptr);
  (void)secapi_delete(&hsh_handle);

  if(result != E_SUCCESS)
  {
    GBA_MSG_ERR_0("Failed to create HMAC");
    return GBA_GENERIC_ERROR;
  }

  return GBA_SUCCESS;
} /* gba_key_gen_ksnaf */

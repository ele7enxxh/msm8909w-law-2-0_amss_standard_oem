/*===========================================================================


            G B A   T M P I   F U N C T I O N S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/20/15   tkl      Added HTTP Cookies support
06/10/15   tkl      Add support of IPv6 BSF address
05/12/15   tkl      Integrated review comment
03/12/15   tkl      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include <stringl/stringl.h>
#include "amssassert.h"

#include "gba.h"
#include "gba_key_gen.h"
#include "gba_platform.h"
#include "gba_storage.h"
#include "gba_util.h"

/*=============================================================================

                       TYPE DECLARATIONS

=============================================================================*/

#define GBA_TMPI_ADDRESS_STR               "@tmpi.bsf.3gppnetwork.org"
#define GBA_TMPI_OCTETS_OF_NAF_FOR_TEMP    24

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/
/*===========================================================================
FUNCTION GBA_TMPI_GET_BSF_ID

DESCRIPTION
  This function computes BSF ID from BSF address and NAF ID

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  update bsf_id data
===========================================================================*/
gba_result_enum_type gba_tmpi_get_bsf_id (
   const gba_bsf_address_type           * realm_ptr,
   gba_naf_id_type                      * bsf_id_ptr
)
{
  /* TMPI generation security protocol as specified by TS 33.220 Annex H.3 */
  uint8                 tmpi_security_protocol[] = {0x1, 0x0, 0x0, 0x1, 0x0};

  if(realm_ptr == NULL ||
     realm_ptr->data_len == 0 ||
     bsf_id_ptr == NULL)
  {
    return GBA_INCORRECT_PARAMS;
  }

  memset(bsf_id_ptr, 0x00, sizeof(gba_naf_id_type));

  bsf_id_ptr->fqdn.data_len = realm_ptr->data_len;
  memscpy(bsf_id_ptr->fqdn.data, sizeof(bsf_id_ptr->fqdn.data),
          realm_ptr->data, sizeof(realm_ptr->data));
  memscpy(bsf_id_ptr->security_protocol, sizeof(bsf_id_ptr->security_protocol),
          tmpi_security_protocol, sizeof(tmpi_security_protocol));

  return GBA_SUCCESS;
} /* gba_tmpi_get_bsf_id */


/*===========================================================================
FUNCTION GBA_TMPI_CALCULATE_TMPI

DESCRIPTION
  This function construct the TMPI string from NAF (TEMP)

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  update bsf_id data
===========================================================================*/
gba_result_enum_type gba_tmpi_calculate_tmpi (
  const gba_ks_naf_type           * naf_ptr,
  gba_impi_type                   * tmpi_ptr)
{
  char                  *tmpi_temp_ptr = NULL;
  uint16                 tmpi_temp_len = 0;

  if(naf_ptr == NULL ||
     tmpi_ptr == NULL)
  {
    return GBA_INCORRECT_PARAMS;
  }

  memset(tmpi_ptr, 0x00, sizeof(gba_impi_type));

  /* Compute TEMP = the base64-encoding [12] of the 24 most significant octets of the NAF */
  tmpi_temp_ptr = gba_util_alloc_bin_to_base64string(*naf_ptr, GBA_TMPI_OCTETS_OF_NAF_FOR_TEMP);
  if(tmpi_temp_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  tmpi_temp_len = strlen(tmpi_temp_ptr);

  if(tmpi_temp_len + strlen(GBA_TMPI_ADDRESS_STR) > sizeof(tmpi_ptr->data))
  {
    GBA_FREE(tmpi_temp_ptr);
    return GBA_GENERIC_ERROR;
  }

  tmpi_ptr->data_len = tmpi_temp_len + strlen(GBA_TMPI_ADDRESS_STR);
  memscpy(tmpi_ptr->data, sizeof(tmpi_ptr->data), tmpi_temp_ptr, tmpi_temp_len);
  memscpy((tmpi_ptr->data + tmpi_temp_len), sizeof(tmpi_ptr->data) - tmpi_temp_len,
          GBA_TMPI_ADDRESS_STR, strlen(GBA_TMPI_ADDRESS_STR));

  GBA_FREE(tmpi_temp_ptr);
  return GBA_SUCCESS;
} /* gba_tmpi_calculate_tmpi */


/*===========================================================================
FUNCTION GBA_TMPI_COMPUTE_FOR_GBA_ME

DESCRIPTION
  This function computes the TMPI for GBA_ME

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_tmpi_compute_for_gba_me(
  const gba_ks_type                       * ks_ptr,
  const gba_impi_type                     * impi_ptr,
  const gba_rand_type                     * rand_ptr,
  const gba_naf_id_type                   * bsf_id_ptr,
  gba_impi_type                           * tmpi_ptr)
{
  gba_result_enum_type                gba_status           = GBA_SUCCESS;
  gba_ks_naf_type                     naf;

  if(ks_ptr == NULL ||
     impi_ptr == NULL ||
     rand_ptr == NULL ||
     bsf_id_ptr == NULL ||
     tmpi_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  gba_status = gba_key_gen_ksnaf(ks_ptr,
                                 rand_ptr,
                                 impi_ptr,
                                 (const gba_naf_id_type *)bsf_id_ptr,
                                 &naf);
  if(gba_status != GBA_SUCCESS)
  {
    return gba_status;
  }

  return gba_tmpi_calculate_tmpi(&naf, tmpi_ptr);
} /* gba_tmpi_compute_for_gba_me */


/*===========================================================================
FUNCTION GBA_TMPI_CLEAR_TMPI_STORAGE

DESCRIPTION
  This function clear the TMPI from the storage

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_tmpi_clear_tmpi_storage(
  gba_session_type                session_type
)
{
  gba_result_enum_type             gba_status              = GBA_SUCCESS;
  gba_storage_profile_data_type  * identity_profile_ptr    = NULL;

  /* If TMPI is not available return success as if it were successfully cleared */
  identity_profile_ptr = gba_storage_get_profile(session_type);
  if(identity_profile_ptr != NULL)
  {
    memset(&identity_profile_ptr->tmpi, 0x00, sizeof(gba_impi_type));

    gba_status = gba_storage_put_profile(session_type, identity_profile_ptr);

    GBA_FREE(identity_profile_ptr);
  }

  return gba_status;
} /* gba_tmpi_clear_tmpi_storage */


/*===========================================================================
FUNCTION GBA_TMPI_GET_IMPI_TMPI_PAIR

DESCRIPTION
  This function gets the TMPI from storage

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_tmpi_get_impi_tmpi_pair(
  gba_session_type                session_type,
  const gba_impi_type           * impi_ptr,
  gba_impi_type                 * tmpi_ptr
)
{
  gba_result_enum_type             gba_status              = GBA_GENERIC_ERROR;
  gba_storage_profile_data_type  * identity_profile_ptr    = NULL;

  if(impi_ptr == NULL ||
     tmpi_ptr == NULL)
  {
    return GBA_INCORRECT_PARAMS;
  }

  memset(tmpi_ptr, 0x00, sizeof(gba_impi_type));

  identity_profile_ptr = gba_storage_get_profile(session_type);
  if(identity_profile_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  if(identity_profile_ptr->impi.data_len == impi_ptr->data_len &&
     identity_profile_ptr->impi.data_len <= GBA_MAX_IMPI_LEN &&
     (memcmp(identity_profile_ptr->impi.data,
             impi_ptr->data,
             impi_ptr->data_len) == 0) &&
     identity_profile_ptr->tmpi.data_len > 0 &&
     identity_profile_ptr->tmpi.data_len <= GBA_MAX_IMPI_LEN)
  {
    tmpi_ptr->data_len = identity_profile_ptr->tmpi.data_len;
    memscpy(tmpi_ptr->data,
            tmpi_ptr->data_len,
            identity_profile_ptr->tmpi.data,
            identity_profile_ptr->tmpi.data_len);
    gba_status = GBA_SUCCESS;
  }

  GBA_FREE(identity_profile_ptr);

  return gba_status;
} /* gba_tmpi_get_impi_tmpi_pair */



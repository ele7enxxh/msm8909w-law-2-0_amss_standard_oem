/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            M M G S D I   P E R S I S T E N T  C A C H E   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the processing of all MMGSDI persistent cache
  related functions

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2014, 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_persistent_cache.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/07/16   dd      Increase max cache profile count to support 3rd operator
05/22/16   vdc     Remove F3 messages for memory allocation failure
10/08/14   tkl     Fix cache activate with imsi_m
09/29/14   tkl     Initial version
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "intconv.h"
#include "uim_msg.h"
#include "err.h"
#include "mmgsdiutil.h"
#include "mmgsdi_file.h"
#include "mmgsdi_persistent_cache.h"

#ifdef FEATURE_UIM_PERSISTENT_CACHE
/*===========================================================================

                        DEFINITION DEPENDANCIES

===========================================================================*/

#define MMGSDI_ACTIVE_PROFILE_USIM 0
#define MMGSDI_ACTIVE_PROFILE_CSIM 1
#define MMGSDI_MAX_ACTIVE_PROFILES 2    /* one for each app type, USIM / CSIM */

#define MMGSDI_MAX_CACHE_PROFILES  6    /* 2 apps, USIM & CSIM, for each operator
                                           profile */

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* Internal storage of persistent cache */
typedef struct
{
  mmgsdi_file_enum_type                  mmgsdi_file;
  mmgsdi_len_type                        data_len;
  uint8                                 *data_ptr;
  uint16                                 record_num;
} mmgsdi_persistent_cache_element_type;

typedef struct mmgsdi_persistent_cache_node
{
  struct mmgsdi_persistent_cache_node   *next_ptr;
  mmgsdi_persistent_cache_element_type   data;
} mmgsdi_persistent_cache_node_type;

typedef struct
{
  mmgsdi_slot_id_enum_type               slot_id;
  mmgsdi_iccid_info_type                 iccid;
  mmgsdi_session_type_enum_type          session_type;
  mmgsdi_data_type                       imsi;
  mmgsdi_persistent_cache_node_type     *cache_list_ptr;
} mmgsdi_persistent_profile_type;

/* List of cache profiles */
static mmgsdi_persistent_profile_type          *mmgsdi_cache_profiles[MMGSDI_MAX_CACHE_PROFILES];

/* Pointers to active cache profile */
static mmgsdi_persistent_profile_type          *mmgsdi_active_cache_ptr[MMGSDI_MAX_NUM_SLOTS][MMGSDI_MAX_ACTIVE_PROFILES];

/* MMGSDI USIM files for persistent cache */
static mmgsdi_file_enum_type mmgsdi_usim_persistent_cache_enums[] =
{
  MMGSDI_USIM_ACC,                      MMGSDI_USIM_LOCI,
  MMGSDI_USIM_PSLOCI,                   MMGSDI_USIM_EPSLOCI,
  MMGSDI_USIM_KEYS,                     MMGSDI_USIM_KEYSPS,
  MMGSDI_USIM_EPSNSC,                   MMGSDI_USIM_START_HFN,
  MMGSDI_USIM_THRESHOLD,                MMGSDI_USIM_KC,
  MMGSDI_USIM_KCGPRS,                   MMGSDI_USIM_GID1,
  MMGSDI_USIM_GID2,                     MMGSDI_USIM_RPLMNACT,
  MMGSDI_USIM_HPLMN,                    MMGSDI_USIM_HPLMNWACT,
  MMGSDI_USIM_PLMNWACT,                 MMGSDI_USIM_OPLMNWACT,
  MMGSDI_USIM_FPLMN,                    MMGSDI_USIM_CBMID,
  MMGSDI_USIM_ACM,                      MMGSDI_USIM_ACM_MAX,
  MMGSDI_USIM_EHPLMN,                   MMGSDI_USIM_EST,
  MMGSDI_USIM_EHPLMNPI,                 MMGSDI_USIM_LRPLMNSI,
  MMGSDI_USIM_HIDDENKEY,                MMGSDI_USIM_SPN,
  MMGSDI_USIM_SPDI,                     MMGSDI_USIM_NASCONFIG,
  MMGSDI_USIM_PNN,                      MMGSDI_USIM_OPL,
  MMGSDI_USIM_ACSGL,                    MMGSDI_USIM_CSGT,
  MMGSDI_USIM_HNBN,                     MMGSDI_USIM_OCSGL,
  MMGSDI_USIM_OCSGT,                    MMGSDI_USIM_OHNBN,
  MMGSDI_USIM_UST,                      MMGSDI_USIM_CPHSI,
  MMGSDI_USIM_CSP,                      MMGSDI_USIM_ONS,
  MMGSDI_USIM_ONS_SHORT
};

/* MMGSDI CSIM files for persistent cache */
static mmgsdi_file_enum_type mmgsdi_csim_persistent_cache_enums[] =
{
  MMGSDI_CSIM_IMSI_T,                   MMGSDI_CSIM_CST,
  MMGSDI_CSIM_EST,                      MMGSDI_CSIM_AD,
  MMGSDI_CSIM_RUIM_ID,                  MMGSDI_CSIM_CDMA_DIST_BASED_REGN_IND,
  MMGSDI_CSIM_CDMA_DIST_BASED_REGN_IND, MMGSDI_CSIM_CDMA_SYS_REGN_IND,
  MMGSDI_CSIM_OTAPA_SPC_ENABLE,         MMGSDI_CSIM_NAM_LOCK,
  MMGSDI_CSIM_SERVICE_PREF,             MMGSDI_CSIM_ACCOLC,
  MMGSDI_CSIM_CDMA_HOME_SID_NID,        MMGSDI_CSIM_CDMA_ZONE_BASED_REGN_IND,
  MMGSDI_CSIM_PRL,                      MMGSDI_CSIM_EPRL,
  MMGSDI_CSIM_MAXIMUM_PRL,              MMGSDI_CSIM_HIDDEN_KEY,
  MMGSDI_CSIM_SPN,                      MMGSDI_CSIM_HRPDUPP
};

/*===========================================================================
         F U N C T I O N S
============================================================================*/

/*===========================================================================
FUNCTION: MMGSDI_PERSISTENT_CACHE_IS_CACHING_SUPPORTED

DESCRIPTION
  Utility function for finding out whether persistent cache is supported for the
  given file enum

DEPENDENCIES
  none

RETURN VALUE
  Success/failure

SIDE EFFECTS

===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_persistent_cache_is_caching_supported
(
  mmgsdi_file_enum_type                  file_enum
)
{
  uint8                     mmgsdi_file_base = 0;
  mmgsdi_file_enum_type    *cache_table_ptr  = NULL;
  uint8                     num_of_files     = 0;
  uint8                     i                = 0;

  mmgsdi_file_base = (uint8) MMGSDIFILE_GET_TECH_TYPE_FROM_FILE(file_enum);
  switch(mmgsdi_file_base)
  {
    case MMGSDI_USIM_EF_START:
      cache_table_ptr = &mmgsdi_usim_persistent_cache_enums[0];
      num_of_files = sizeof(mmgsdi_usim_persistent_cache_enums) /
                            sizeof(mmgsdi_usim_persistent_cache_enums[0]);
      break;

    case MMGSDI_CSIM_EF_START:
      cache_table_ptr = &mmgsdi_csim_persistent_cache_enums[0];
      num_of_files = sizeof(mmgsdi_csim_persistent_cache_enums) /
                            sizeof(mmgsdi_csim_persistent_cache_enums[0]);;
      break;

    default:
      UIM_MSG_MED_1("no persistent cache file_enum 0x%x", file_enum);
      return MMGSDI_ERROR;
  }

  for( i = 0; i < num_of_files; i++)
  {
    if(cache_table_ptr[i] == file_enum)
    {
      return MMGSDI_SUCCESS;
    }
  }
  return MMGSDI_ERROR;
} /* mmgsdi_persistent_cache_is_caching_supported */


/*===========================================================================
FUNCTION: MMGSDI_PERSISTENT_CACHE_GET_PROFILE_INDEX_FROM_SESSION_TYPE

DESCRIPTION
  Retrieves active profile index from session type

DEPENDENCIES
  none

RETURN VALUE
  Success/failure

SIDE EFFECTS

===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_persistent_cache_get_profile_index_from_session_type
(
  mmgsdi_session_type_enum_type          session_type,
  uint8                                 *profile_index
)
{
  switch (session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      *profile_index = MMGSDI_ACTIVE_PROFILE_USIM;
      break;

    case MMGSDI_1X_PROV_PRI_SESSION:
      *profile_index = MMGSDI_ACTIVE_PROFILE_CSIM;
      break;

    default:
      UIM_MSG_MED_1("persistent cache doesn't support session type 0x%x",
                    session_type);
      *profile_index = MMGSDI_MAX_ACTIVE_PROFILES;
      return MMGSDI_ERROR;
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_persistent_cache_get_profile_index_from_session_type */


/*===========================================================================
FUNCTION: MMGSDI_PERSISTENT_CACHE_GET_PROFILE_INDEX_FROM_FILE

DESCRIPTION
  Retrieves active profile index from file

DEPENDENCIES
  none

RETURN VALUE
  Success/failure

SIDE EFFECTS

===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_persistent_cache_get_profile_index_from_file
(
  mmgsdi_file_enum_type          file_enum,
  uint8                         *profile_index
)
{
  uint8  mmgsdi_file_base = 0;

  mmgsdi_file_base = (uint8) MMGSDIFILE_GET_TECH_TYPE_FROM_FILE(file_enum);

  switch (mmgsdi_file_base)
  {
    case MMGSDI_USIM_EF_START:
      *profile_index = MMGSDI_ACTIVE_PROFILE_USIM;
      break;

    case MMGSDI_CSIM_EF_START:
      *profile_index = MMGSDI_ACTIVE_PROFILE_CSIM;
      break;

    default:
      UIM_MSG_MED_1("persistent cache doesn't support file base 0x%x",
                    mmgsdi_file_base);
      *profile_index = MMGSDI_MAX_ACTIVE_PROFILES;
      return MMGSDI_ERROR;
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_persistent_cache_get_profile_index_from_session_type */


/*===========================================================================
FUNCTION: MMGSDI_PERSISTENT_CACHE_DELETE_CACHE_PROFILE

DESCRIPTION
  Delete cache profile per index

DEPENDENCIES
  none

RETURN VALUE
  Success/failure

SIDE EFFECTS

===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_persistent_cache_delete_cache_profile
(
  uint8                    index
)
{
  mmgsdi_persistent_cache_node_type *cache_list_ptr    = NULL;
  mmgsdi_persistent_cache_node_type *temp_ptr          = NULL;
  mmgsdi_return_enum_type            mmgsdi_status     = MMGSDI_SUCCESS;

  if(index >= MMGSDI_MAX_CACHE_PROFILES)
  {
    return MMGSDI_ERROR;
  }

  if(mmgsdi_cache_profiles[index] == NULL)
  {
    UIM_MSG_MED_1("mmgsdi_cache_profiles [0x%x] is already NULL", index);
    return MMGSDI_SUCCESS;
  }

  cache_list_ptr = mmgsdi_cache_profiles[index]->cache_list_ptr;
  while(cache_list_ptr != NULL)
  {
    /* free data in cache node */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cache_list_ptr->data.data_ptr);

    temp_ptr = cache_list_ptr;
    cache_list_ptr = cache_list_ptr->next_ptr;

    /* free cache list node*/
    temp_ptr->next_ptr = NULL;

    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(temp_ptr);
  }
  /* free cache profile */
  mmgsdi_cache_profiles[index]->cache_list_ptr = NULL;

  /* free imsi data */
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_cache_profiles[index]->imsi.data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_cache_profiles[index]);

  return MMGSDI_SUCCESS;
} /* mmgsdi_persistent_cache_delete_cache_profile */


/*===========================================================================
FUNCTION: MMGSDI_PERSISTENT_CACHE_ACTIVATE_PROFILE

DESCRIPTION
  Switches internal cache to the one specified, persistent cache is only available
  for GW_PRI or 1X_PRI session.

DEPENDENCIES
  none

RETURN VALUE
  Success/failure

SIDE EFFECTS

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_persistent_cache_activate_profile
(
  mmgsdi_slot_id_enum_type           slot_id,
  mmgsdi_iccid_info_type             iccid,
  mmgsdi_session_type_enum_type      session_type,
  mmgsdi_data_type                   imsi
)
{
  mmgsdi_return_enum_type            mmgsdi_status = MMGSDI_SUCCESS;
  uint8                              slot_index    = MMGSDI_SLOT_1_INDEX;
  uint8                              profile_index = MMGSDI_MAX_ACTIVE_PROFILES;
  uint8                              i             = 0;
  uint8                              free_index    = MMGSDI_MAX_CACHE_PROFILES;

  if(imsi.data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_slot_index(slot_id, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_persistent_cache_get_profile_index_from_session_type(session_type,
                                                                              &profile_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if(mmgsdi_active_cache_ptr[slot_index][profile_index] != NULL )
  {
    UIM_MSG_MED_2("mmgsdi_active_cache_ptr[0x%x][0x%x] already activated",
                  slot_index, profile_index);
    return MMGSDI_ERROR;
  }

  for(i = 0; i < MMGSDI_MAX_CACHE_PROFILES; i++)
  {
    if(mmgsdi_cache_profiles[i] == NULL)
    {
      /* store first free profile index */
      if(free_index == MMGSDI_MAX_CACHE_PROFILES)
      {
        free_index = i;
      }
      continue;
    }
    if(mmgsdi_cache_profiles[i]->slot_id == slot_id &&
       (mmgsdi_cache_profiles[i]->iccid.length == iccid.length &&
        (memcmp(mmgsdi_cache_profiles[i]->iccid.data, iccid.data, iccid.length) == 0)) &&
       mmgsdi_cache_profiles[i]->session_type == session_type &&
       (mmgsdi_cache_profiles[i]->imsi.data_len == imsi.data_len &&
        memcmp(mmgsdi_cache_profiles[i]->imsi.data_ptr,
               imsi.data_ptr,
               imsi.data_len) == 0))
    {
      mmgsdi_active_cache_ptr[slot_index][profile_index] = mmgsdi_cache_profiles[i];
      return MMGSDI_SUCCESS;
    }
  }

  if(free_index == MMGSDI_MAX_CACHE_PROFILES)
  {
    UIM_MSG_ERR_0("profile not found and unable to create a new profile");
    return MMGSDI_ERROR;
  }

  /* Create cache profile if not found in cache profile list */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_cache_profiles[free_index],
                                   sizeof(mmgsdi_persistent_profile_type));
  if(mmgsdi_cache_profiles[free_index] == NULL)
  {
    return MMGSDI_ERROR;
  }

  memset(mmgsdi_cache_profiles[free_index], 0, sizeof(mmgsdi_persistent_profile_type));

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_cache_profiles[free_index]->imsi.data_ptr,
                                     sizeof(uint8) * imsi.data_len);
  if(mmgsdi_cache_profiles[free_index]->imsi.data_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_cache_profiles[free_index]);
    return MMGSDI_ERROR;
  }
  mmgsdi_memscpy(mmgsdi_cache_profiles[free_index]->imsi.data_ptr, imsi.data_len,
                 imsi.data_ptr, imsi.data_len);

  mmgsdi_cache_profiles[free_index]->imsi.data_len = imsi.data_len;
  mmgsdi_cache_profiles[free_index]->slot_id       = slot_id;
  mmgsdi_cache_profiles[free_index]->session_type  = session_type;
  mmgsdi_cache_profiles[free_index]->iccid.length  = iccid.length;
  mmgsdi_memscpy(mmgsdi_cache_profiles[free_index]->iccid.data,
                 mmgsdi_cache_profiles[free_index]->iccid.length,
                 iccid.data,
                 iccid.length);

  mmgsdi_active_cache_ptr[slot_index][profile_index] = mmgsdi_cache_profiles[free_index];

  return MMGSDI_SUCCESS;
} /* mmgsdi_persistent_cache_activate_profile */


/*===========================================================================
FUNCTION: MMGSDI_PERSISTENT_CACHE_DEACTIVATE_PROFILE

DESCRIPTION
  Deactive internal cache to per slot id and app type

DEPENDENCIES
  none

RETURN VALUE
  Success/failure

SIDE EFFECTS

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_persistent_cache_deactivate_profile
(
  mmgsdi_slot_id_enum_type           slot_id,
  mmgsdi_session_type_enum_type      session_type
)
{
  mmgsdi_return_enum_type            mmgsdi_status = MMGSDI_SUCCESS;
  uint8                              slot_index    = MMGSDI_SLOT_1_INDEX;
  uint8                              profile_index = MMGSDI_ACTIVE_PROFILE_USIM;
  uint8                              i             = 0;

  mmgsdi_status = mmgsdi_util_get_slot_index(slot_id, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_persistent_cache_get_profile_index_from_session_type(session_type,
                                                                              &profile_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if(mmgsdi_active_cache_ptr[slot_index][profile_index] != NULL)
  {
    mmgsdi_active_cache_ptr[slot_index][profile_index] = NULL;
  }

  UIM_MSG_MED_2("persistent cache for slot_id 0x%x, session_type 0x%x deactivated",
                 slot_id, session_type);

  return MMGSDI_SUCCESS;
} /* mmgsdi_persistent_cache_deactivate_profile */


/*===========================================================================
FUNCTION: MMGSDI_PERSISTENT_CACHE_READ

DESCRIPTION
  Reads a specific file from active profile, the function takes care of
  allocating the memory for data_ptr.

DEPENDENCIES
  none

RETURN VALUE
  Success/failure

SIDE EFFECTS

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_persistent_cache_read
(
  mmgsdi_slot_id_enum_type           slot_id,
  mmgsdi_file_enum_type              file_enum,
  uint16                             record_num,
  mmgsdi_len_type                   *data_len,
  uint8                            **data_ptr
)
{
  mmgsdi_persistent_cache_node_type *cache_list_ptr = NULL;
  mmgsdi_return_enum_type            mmgsdi_status  = MMGSDI_SUCCESS;
  uint8                              slot_index     = MMGSDI_SLOT_1_INDEX;
  uint8                              profile_index  = MMGSDI_MAX_ACTIVE_PROFILES;

  if(data_len == NULL || data_ptr == NULL)
  {
    UIM_MSG_ERR_0("Invalid input parameters");
    return MMGSDI_INCORRECT_PARAMS;
  }

  mmgsdi_status = mmgsdi_util_get_slot_index(slot_id, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_persistent_cache_get_profile_index_from_file(file_enum,
                                                                      &profile_index);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = MMGSDI_NOT_FOUND;

  /* check if cache is activated*/
  if(mmgsdi_active_cache_ptr[slot_index][profile_index] == NULL)
  {
    UIM_MSG_MED_3("persistent cache is not activated for slot 0x%x, profile_index 0x%x, file_enum 0x%x",
                  slot_index, profile_index, file_enum);
    return MMGSDI_NOT_FOUND;
  }

  cache_list_ptr = mmgsdi_active_cache_ptr[slot_index][profile_index]->cache_list_ptr;
  while(cache_list_ptr != NULL)
  {
    if(cache_list_ptr->data.mmgsdi_file == file_enum &&
       cache_list_ptr->data.record_num  == record_num)
    {
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*data_ptr,
                                         sizeof(uint8)*cache_list_ptr->data.data_len);
      if((*data_ptr) == NULL)
      {
        return MMGSDI_ERROR;
      }

      *data_len = cache_list_ptr->data.data_len;

      /* Copy from cache */
      mmgsdi_memscpy((*data_ptr),
                     int32touint32(*data_len),
                     cache_list_ptr->data.data_ptr,
                     int32touint32(cache_list_ptr->data.data_len));

      UIM_MSG_MED_2("Found persistent cache for file_enum 0x%x, data_len 0x%x", file_enum, *data_len);
      return MMGSDI_SUCCESS;
    }
    cache_list_ptr = cache_list_ptr->next_ptr;
  }
  return mmgsdi_status;
} /* mmgsdi_persistent_cache_read */


/*===========================================================================
FUNCTION: MMGSDI_PERSISTENT_CACHE_WRITE


DESCRIPTION
  Writes to a specific file in active profile

DEPENDENCIES
  none

RETURN VALUE
  Success/failure

SIDE EFFECTS

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_persistent_cache_write
(
  mmgsdi_slot_id_enum_type           slot_id,
  mmgsdi_file_enum_type              file_enum,
  uint16                             record_num,
  mmgsdi_len_type                    data_len,
  const uint8                       *data_ptr
)
{
  mmgsdi_persistent_cache_node_type *cache_list_ptr      = NULL;
  mmgsdi_return_enum_type            mmgsdi_status       = MMGSDI_SUCCESS;
  uint8                              slot_index          = MMGSDI_SLOT_1_INDEX;
  uint8                              profile_index       = MMGSDI_ACTIVE_PROFILE_USIM;
  mmgsdi_persistent_profile_type    *active_profile_ptr  = NULL;

  if(data_ptr == NULL)
  {
    UIM_MSG_ERR_0("Invalid input parameters");
    return MMGSDI_INCORRECT_PARAMS;
  }

  mmgsdi_status = mmgsdi_persistent_cache_is_caching_supported(file_enum);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_get_slot_index(slot_id, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_persistent_cache_get_profile_index_from_file(file_enum,
                                                                      &profile_index);

  UIM_MSG_ERR_2("slot 0x%x; profile_index 0x%x", slot_index, profile_index);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* check if cache is activated*/
  if(mmgsdi_active_cache_ptr[slot_index][profile_index] == NULL)
  {
    UIM_MSG_MED_0("mmgsdi_active_cache_ptr == NULL");
    return MMGSDI_NOT_INIT;
  }

  active_profile_ptr = mmgsdi_active_cache_ptr[slot_index][profile_index];

  if(active_profile_ptr->cache_list_ptr != NULL)
  {
    /* Search thru cache list */
    cache_list_ptr = active_profile_ptr->cache_list_ptr;
    while(cache_list_ptr != NULL)
    {
      if(cache_list_ptr->data.mmgsdi_file == file_enum &&
         cache_list_ptr->data.record_num  == record_num)
      {
        /* free the data_ptr from cache node */
        UIM_MSG_MED_2("found cache for file enum 0x%x, rec 0x%x: free data_ptr",
                      file_enum, record_num);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cache_list_ptr->data.data_ptr);
        break;
      }
      cache_list_ptr = cache_list_ptr->next_ptr;
    }
  }

  if(cache_list_ptr == NULL)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(cache_list_ptr,
                                       sizeof(mmgsdi_persistent_cache_node_type));
    if(cache_list_ptr == NULL)
    {
      return MMGSDI_ERROR;
    }
    memset(cache_list_ptr, 0, sizeof(mmgsdi_persistent_cache_node_type));

    /* append new cache node at the head of cache list */
    cache_list_ptr->next_ptr = active_profile_ptr->cache_list_ptr;
    active_profile_ptr->cache_list_ptr = cache_list_ptr;
  }

  /* update cache node data */
  cache_list_ptr->data.mmgsdi_file = file_enum;
  cache_list_ptr->data.record_num  = record_num;
  cache_list_ptr->data.data_len    = data_len;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(cache_list_ptr->data.data_ptr,
                                     sizeof(uint8)*data_len);

  if(cache_list_ptr->data.data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }
  mmgsdi_memscpy(cache_list_ptr->data.data_ptr, cache_list_ptr->data.data_len,
                 data_ptr, data_len);

  return MMGSDI_SUCCESS;
} /* mmgsdi_persistent_cache_write */


/*===========================================================================
FUNCTION: MMGSDI_PERSISTENT_CACHE_DELETE


DESCRIPTION
  Deletes the entire or active only persistent cache for the slot

DEPENDENCIES
  none

RETURN VALUE
  Success/failure

SIDE EFFECTS

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_persistent_cache_delete
(
  mmgsdi_slot_id_enum_type           slot_id,
  mmgsdi_session_type_enum_type      session_type,
  boolean                            active_only
)
{
  mmgsdi_return_enum_type            mmgsdi_status     = MMGSDI_SUCCESS;
  uint8                              slot_index        = MMGSDI_SLOT_1_INDEX;
  uint8                              profile_index     = MMGSDI_ACTIVE_PROFILE_USIM;
  uint8                              i                 = 0;
  uint8                              j                 = 0;

  mmgsdi_status = mmgsdi_util_get_slot_index(slot_id, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if(active_only)
  {
    mmgsdi_status = mmgsdi_persistent_cache_get_profile_index_from_session_type(session_type,
                                                                                &profile_index);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }

    if(mmgsdi_active_cache_ptr[slot_index][profile_index] == NULL)
    {
      UIM_MSG_MED_0("no active profile to delete");
      return MMGSDI_ERROR;
    }

    /* find index of active profile */
    for(i = 0; i < MMGSDI_MAX_CACHE_PROFILES; i++)
    {
      if(mmgsdi_cache_profiles[i] == mmgsdi_active_cache_ptr[slot_index][profile_index])
      {
        break;
      }
    }

    if(i >= MMGSDI_MAX_CACHE_PROFILES)
    {
      UIM_MSG_MED_0("active cache ptr not pointing to mmgsdi_cache_profiles list");
      return MMGSDI_ERROR;
    }

    /* Delete active cache profile*/
    (void)mmgsdi_persistent_cache_delete_cache_profile(i);

    /* Deactivate active cache pointer */
    mmgsdi_active_cache_ptr[slot_index][profile_index] = NULL;
    return MMGSDI_SUCCESS;
  }

  /* delete entire persistent cache for the slot */
  for(i = 0; i < MMGSDI_MAX_CACHE_PROFILES; i++)
  {
    if(mmgsdi_cache_profiles[i] != NULL &&
       mmgsdi_cache_profiles[i]->slot_id == slot_id)
    {
      mmgsdi_status = mmgsdi_persistent_cache_delete_cache_profile(i);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        return mmgsdi_status;
      }
    }
  }

  /* Deactivate active cache pointers for the slot */
  for(i = 0; i < MMGSDI_MAX_ACTIVE_PROFILES; i++)
  {
    mmgsdi_active_cache_ptr[slot_index][i] = NULL;
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_persistent_cache_delete */

#endif /* FEATURE_UIM_PERSISTENT_CACHE */

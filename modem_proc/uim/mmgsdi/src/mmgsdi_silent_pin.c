/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 MMGSDI - SILENT PIN RELATED FUNCTIONS


GENERAL DESCRIPTION

  This source file contains functions for managing and utilizing silent
  pin data.

                        COPYRIGHT INFORMATION

Copyright (c) 2014, 2016 QUALCOMM Techologies, Inc (QTI) and its licensors.  All Rights
Reserved.  QUALCOMM Techologies Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_silent_pin.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/22/16   vdc     Remove F3 messages for memory allocation failure
04/04/16   tl      Remove unnecessary traces
11/14/14   hh      Fix compilation warnings
10/27/14   hh      Fix KW error
08/14/14   tl      Initial version
===========================================================================*/

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "mmgsdi_silent_pin.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdi.h"
#include "mmgsdicache.h"
#include "mmgsdisessionlib.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SILENT_PIN_DATA_TYPE

   DESCRIPTION:
     This structure stores the data corresponding to one instance of
     pin data.
-------------------------------------------------------------------------------*/
typedef struct{
  mmgsdi_key_ref_values_enum_type   key_ref;
  mmgsdi_pin_status_enum_type       status;
  mmgsdi_static_data_type           pin_value;
  mmgsdi_aid_type                   app_data;
} mmgsdi_silent_pin_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SILENT_PIN_INFO_TYPE

   DESCRIPTION:
     This structure contains all the information to regarding the current
     status of the silent pin cache as well as stores the data stored.
-------------------------------------------------------------------------------*/
typedef struct{
  mmgsdi_iccid_info_type            iccid_data;
  mmgsdi_silent_pin_data_type      *pin1_table_ptr[MMGSDI_MAX_PIN1_INFO];
} mmgsdi_silent_pin_info_type;

static mmgsdi_silent_pin_info_type    *mmgsdi_silent_pin_info_table_ptr[MMGSDI_MAX_NUM_SLOTS];

rex_tcb_type *  sam_mmgsdi_current_task_0 = NULL;
rex_tcb_type *  sam_mmgsdi_current_task_1 = NULL;


/*=============================================================================

                          FUNCTIONS

=============================================================================*/

/*===========================================================================
  FUNCTION MMGSDI_SILENT_PIN_INIT()

  DESCRIPTION
    Initalizes the mmgsdi_silent_pin_info_table_ptr.

    This should only be called once when MMGSDI is initalizing
    all global variables.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    This should only be called when MMGSDI is being initalized

  SIDE EFFECTS
    None
===========================================================================*/
void mmgsdi_silent_pin_init(
  void
)
{
  if(mmgsdi_util_get_current_task() == MMGSDI_TASK_MAX)
  {
    return;
  }
  UIM_MSG_HIGH_0("mmgsdi_silent_pin_init()");

  memset(mmgsdi_silent_pin_info_table_ptr,
         0x00,
         sizeof(mmgsdi_silent_pin_info_table_ptr));
} /* mmgsdi_silent_pin_init */


/*===========================================================================
  FUNCTION MMGSDI_SILENT_PIN_PURGE_INFO()

  DESCRIPTION
    Purge the PIN information saved for silent PIN verification

  PARAMETERS
    slot_id : slot on which to purge pin info

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void mmgsdi_silent_pin_purge_info(
  mmgsdi_slot_id_enum_type    slot_id
)
{
  uint8                       slot_index    = 0;
  uint8                       i             = 0;

  if(mmgsdi_util_get_current_task() == MMGSDI_TASK_MAX ||
     mmgsdi_util_get_slot_index(slot_id, &slot_index) != MMGSDI_SUCCESS ||
     mmgsdi_silent_pin_info_table_ptr[slot_index] == NULL)
  {
    return;
  }

  UIM_MSG_HIGH_1("mmgsdi_silent_pin_purge_info, slot_id: 0x%x", slot_id);

  for (i = 0; i < MMGSDI_MAX_PIN1_INFO; i++)
  {
    if(NULL != mmgsdi_silent_pin_info_table_ptr[slot_index]->pin1_table_ptr[i])
    {
      /* Need to clear the heap PIN data in order to protect against an attacker
       reading this memory after it has been freed */
      memset(&mmgsdi_silent_pin_info_table_ptr[slot_index]->pin1_table_ptr[i],
           0x00,
             sizeof(mmgsdi_silent_pin_info_table_ptr[slot_index]->pin1_table_ptr[i]));

      MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_silent_pin_info_table_ptr[slot_index]->pin1_table_ptr[i]);
    }
  }

  memset(&mmgsdi_silent_pin_info_table_ptr[slot_index],
         0x00,
         sizeof(mmgsdi_silent_pin_info_table_ptr[slot_index]));

  MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_silent_pin_info_table_ptr[slot_index]);
} /* mmgsdi_silent_pin_purge_info */


/*===========================================================================
  FUNCTION MMGSDI_SILENT_PIN_CACHE_DATA()

  DESCRIPTION
    Save the PIN information to be used for silent PIN verification

  PARAMETERS
    slot_id : slot on which to save pin info

  RETURN VALUE
    mmgsdi_return_enum_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_silent_pin_cache_data(
  mmgsdi_slot_id_enum_type    slot_id
)
{
  mmgsdi_return_enum_type     mmgsdi_status = MMGSDI_SUCCESS;
  uint8                       slot_index    = 0;
  uint8                       app_index     = 0;
  uint8                       pin_index     = 0;
  mmgsdi_slot_data_type      *slot_data_ptr = NULL;
  mmgsdi_data_type            iccid_data;
  uint8                       iccid[MMGSDI_ICCID_LEN] = {0};
  mmgsdi_access_type          access_info;

  UIM_MSG_HIGH_1("mmgsdi_silent_pin_cache_data, slot: 0x%x", slot_id);

  memset(&access_info, 0x00, sizeof(access_info));

  if(mmgsdi_util_get_current_task() == MMGSDI_TASK_MAX)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_slot_index(slot_id, &slot_index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);
  if (slot_data_ptr == NULL ||
      slot_data_ptr->app_info_pptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if (mmgsdi_silent_pin_info_table_ptr[slot_index] != NULL)
  {
    /* This should one be reached in the case a temporary error occurs between the time
       a preceeding temporary error caches the PIN and the following PIN verification
       after power up occurs where there was no need to purge the PIN data.*/
    UIM_MSG_HIGH_1("Silent PIN info is already cached on slot: 0x%x", slot_id);
    return MMGSDI_SUCCESS;
  }

  access_info.access_method = MMGSDI_EF_ENUM_ACCESS;
  access_info.file.file_enum = MMGSDI_ICCID;

  iccid_data.data_len = MMGSDI_ICCID_LEN;
  iccid_data.data_ptr = iccid;

  /* Save the ICCID to ensure that, upon power-up, the card being
     silently verified is still matching. */
  mmgsdi_status = mmgsdi_cache_read(mmgsdi_generic_data_ptr->slot_session_id[slot_index],
                                    &access_info,
                                    &iccid_data,
                                    0,
                                    NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_silent_pin_info_table_ptr[slot_index],
                                     sizeof(mmgsdi_silent_pin_info_type));
  if (mmgsdi_silent_pin_info_table_ptr[slot_index] == NULL)
  {
    return MMGSDI_ERROR;
  }

  memset(mmgsdi_silent_pin_info_table_ptr[slot_index], 0x00,
         sizeof(mmgsdi_silent_pin_info_type));

  memscpy(mmgsdi_silent_pin_info_table_ptr[slot_index]->iccid_data.data,
          iccid_data.data_len,
          iccid_data.data_ptr,
          iccid_data.data_len);
  mmgsdi_silent_pin_info_table_ptr[slot_index]->iccid_data.length =
    iccid_data.data_len;

  for (app_index = 0, pin_index = 0;
       app_index < MMGSDI_MAX_EXT_APP_INFO && pin_index < MMGSDI_MAX_PIN1_INFO;
       app_index++)
  {
    if (slot_data_ptr->app_info_pptr[app_index] == NULL ||
        slot_data_ptr->app_info_pptr[app_index]->pin1_ptr == NULL ||
        slot_data_ptr->app_info_pptr[app_index]->pin1_ptr->cached == FALSE ||
        slot_data_ptr->app_info_pptr[app_index]->pin1_ptr->valid_pin == FALSE)
    {
      continue;
    }

    if (mmgsdi_silent_pin_info_table_ptr[slot_index]->pin1_table_ptr[pin_index] != NULL)
    {
      /* Pin1 table is already allocated. This should never happen. */
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      mmgsdi_silent_pin_info_table_ptr[slot_index]->pin1_table_ptr[pin_index],
      sizeof(mmgsdi_silent_pin_data_type));
    if (mmgsdi_silent_pin_info_table_ptr[slot_index]->pin1_table_ptr[pin_index] == NULL)
    {
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    mmgsdi_silent_pin_info_table_ptr[slot_index]->pin1_table_ptr[pin_index]->key_ref =
      slot_data_ptr->app_info_pptr[app_index]->pin1_ptr->key_ref;
    mmgsdi_silent_pin_info_table_ptr[slot_index]->pin1_table_ptr[pin_index]->status =
      slot_data_ptr->app_info_pptr[app_index]->pin1_ptr->status;
    mmgsdi_silent_pin_info_table_ptr[slot_index]->pin1_table_ptr[pin_index]->pin_value =
      slot_data_ptr->app_info_pptr[app_index]->pin1_ptr->pin_data;
    mmgsdi_silent_pin_info_table_ptr[slot_index]->pin1_table_ptr[pin_index]->app_data =
      slot_data_ptr->app_info_pptr[app_index]->app_data;

    pin_index++;
  }

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_silent_pin_purge_info(slot_id);
  }

  return mmgsdi_status;
} /* mmgsdi_silent_pin_cache_data */


/*===========================================================================
  FUNCTION MMGSDI_SILENT_PIN_MATCH_ICCID()

  DESCRIPTION
    Checks that the input ICCID data matches the data stored in the
    silent pin info table which is cached when an extended recovery
    results in a card reset.

  PARAMETERS
    iccid_data_ptr : pointer to iccid data
    slot_id : identifies the slot corresponding to the pin info cache

  RETURN VALUE
    boolean

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean mmgsdi_silent_pin_match_iccid(
  const mmgsdi_iccid_info_type     * iccid_data_ptr,
  mmgsdi_slot_id_enum_type           slot_id
)
{
  uint8 slot_index = 0;
  boolean   match      = FALSE;

  do
  {
    if (mmgsdi_util_get_current_task() == MMGSDI_TASK_MAX)
    {
      break;
    }

    if (mmgsdi_util_get_slot_index(slot_id, &slot_index) != MMGSDI_SUCCESS ||
        mmgsdi_silent_pin_info_table_ptr[slot_index] == NULL)
    {
      break;
    }

    if (iccid_data_ptr == NULL ||
        iccid_data_ptr->length == 0 ||
        iccid_data_ptr->length > MMGSDI_ICCID_LEN ||
        iccid_data_ptr->length != mmgsdi_silent_pin_info_table_ptr[slot_index]->iccid_data.length)
    {
      break;
    }

    if (memcmp(iccid_data_ptr->data,
               mmgsdi_silent_pin_info_table_ptr[slot_index]->iccid_data.data,
               iccid_data_ptr->length) != 0)
    {
      break;
    }

    match = TRUE;
  }
  while(0);

  UIM_MSG_HIGH_1("Silent PIN - ICCID match: 0x%x", match);
  return match;
} /* mmgsdi_silent_pin_match_iccid */


/*===========================================================================
  FUNCTION MMGSDI_SILENT_PIN_IS_CACHED()

  DESCRIPTION
    Returns whether the silent pin information is cached on the slot

  PARAMETERS
    slot_id : identifies the slot corresponding to the pin info cache

  RETURN VALUE
    boolean

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean mmgsdi_silent_pin_is_cached(
  mmgsdi_slot_id_enum_type           slot_id
)
{
  uint8 slot_index = 0;

  if (mmgsdi_util_get_current_task() == MMGSDI_TASK_MAX)
  {
    return FALSE;
  }

  if (mmgsdi_util_get_slot_index(slot_id, &slot_index) != MMGSDI_SUCCESS ||
      mmgsdi_silent_pin_info_table_ptr[slot_index] == NULL)
  {
    return FALSE;
  }

  UIM_MSG_HIGH_1("silent PIN is cached, slot: 0x%x", slot_id);

  return TRUE;
} /* mmgsdi_silent_pin_is_cached */


/*===========================================================================
  FUNCTION MMGSDI_SILENT_PIN_VERIFY_PIN()

  DESCRIPTION
    This function will match the application AID with the AID stored
    in the silent PIN cache and if a match is found, send the PIN
    verification request with the cached PIN data.

  PARAMETERS
    session_id : id of the current session
    app_info_ptr : application information to match with the cached data
    slot_id : identifies the slot corresponding to the pin info cache

  RETURN VALUE
    boolean

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean mmgsdi_silent_pin_verify_pin(
  mmgsdi_session_id_type             session_id,
  const mmgsdi_int_app_info_type    *app_info_ptr,
  mmgsdi_slot_id_enum_type           slot_id
)
{
  uint8                       i             = 0;
  uint8                       slot_index    = 0;
  mmgsdi_data_type            pin_data;

  if (mmgsdi_util_get_current_task() == MMGSDI_TASK_MAX)
  {
    return FALSE;
  }

  UIM_MSG_HIGH_1("mmgsdi_silent_pin_verify_pin, slot: 0x%x", slot_id);

  if (NULL == app_info_ptr           ||
      NULL == app_info_ptr->pin1_ptr ||
      app_info_ptr->pin1_ptr->status != MMGSDI_PIN_ENABLED_NOT_VERIFIED)
  {
    return FALSE;
  }

  if (mmgsdi_util_get_slot_index(slot_id, &slot_index) != MMGSDI_SUCCESS ||
      mmgsdi_silent_pin_info_table_ptr[slot_index] == NULL)
  {
    return FALSE;
  }

  for (i = 0; i < MMGSDI_MAX_PIN1_INFO; i++)
  {
    if(mmgsdi_silent_pin_info_table_ptr[slot_index]->pin1_table_ptr[i] == NULL ||
       mmgsdi_silent_pin_info_table_ptr[slot_index]->pin1_table_ptr[i]->key_ref !=
         app_info_ptr->pin1_ptr->key_ref ||
       mmgsdi_silent_pin_info_table_ptr[slot_index]->pin1_table_ptr[i]->status !=
         MMGSDI_PIN_ENABLED_VERIFIED ||
       mmgsdi_silent_pin_info_table_ptr[slot_index]->pin1_table_ptr[i]->app_data.app_type !=
         app_info_ptr->app_data.app_type ||
       mmgsdi_silent_pin_info_table_ptr[slot_index]->pin1_table_ptr[i]->app_data.aid.data_len !=
         app_info_ptr->app_data.aid.data_len)
    {
      continue;
    }

    if(memcmp(mmgsdi_silent_pin_info_table_ptr[slot_index]->pin1_table_ptr[i]->app_data.aid.data_ptr,
              app_info_ptr->app_data.aid.data_ptr,
              app_info_ptr->app_data.aid.data_len) != 0)
    {
      continue;
    }

    pin_data.data_len =
      mmgsdi_silent_pin_info_table_ptr[slot_index]->pin1_table_ptr[i]->pin_value.data_len;
    pin_data.data_ptr =
      mmgsdi_silent_pin_info_table_ptr[slot_index]->pin1_table_ptr[i]->pin_value.data_ptr;

    UIM_MSG_HIGH_2("Silently verifying PIN1, slot index: 0x%x, app_type: 0x%x",
                   slot_index, app_info_ptr->app_data.app_type);

    if(mmgsdi_session_verify_pin(session_id,
                                 MMGSDI_PIN1,
                                 pin_data,
                                 mmgsdi_util_internal_cb,
                                 0) != MMGSDI_SUCCESS)
    {
      return FALSE;
    }

    /* Need to clear the heap PIN data in order to protect against an attacker
       reading this memory after it has been freed */
    memset(&mmgsdi_silent_pin_info_table_ptr[slot_index]->pin1_table_ptr[i]->pin_value,
           0x00,
           sizeof(mmgsdi_silent_pin_info_table_ptr[slot_index]->pin1_table_ptr[i]->pin_value));

    /* After the silent PIN has been verified, it will be either be valid and recached
       in a separately location along with other useful PIN data or invalid in which
       case this data is useless anyways. Either way this data will not longer be
       needed so we can free. */
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_silent_pin_info_table_ptr[slot_index]->pin1_table_ptr[i]);
    return TRUE;
  }

  return FALSE;
} /* mmgsdi_silent_pin_verify_pin */


/*===========================================================================
  FUNCTION MMGSDI_CLEAR_SILENT_PIN_DATA()

  DESCRIPTION
    This function will clear the silent PIN info stored on the heap

  PARAMETERS
    None

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void mmgsdi_clear_silent_pin_data(
  void
)
{
  uint8                        slot_index    = 0;
  mmgsdi_slot_id_enum_type     slot_id       = MMGSDI_SLOT_1;

  if(mmgsdi_util_get_current_task() == MMGSDI_TASK_MAX)
  {
    return;
  }

  for (slot_index = 0; slot_index < MMGSDI_MAX_NUM_SLOTS; slot_index++)
  {
    if (mmgsdi_util_get_slot_id(slot_index, &slot_id) == MMGSDI_SUCCESS)
    {
      mmgsdi_silent_pin_purge_info(slot_id);
    }
  }
} /* mmgsdi_clear_silent_pin_data */


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 M M G S D I   S I M L O C K  F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains functions to perform MMGSDI specific SIM Lock
  tasks.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2014 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_simlock.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/08/16   bcho    Added support for ICCID simlock
08/22/16   vdc     Set appropriate status/feature, during ready to perso state
06/17/16   vdc     Added support for SPN based SIMLOCK feature
05/22/16   vdc     Remove F3 messages for memory allocation failure
01/07/16   kv      UIM PSM handling updates
12/25/15   bcho    Send Card and session events based on SFS data in PSM mode
11/16/15   vdc     Send app state as PERSO when device is set in emergency mode
11/05/15   kv      Check for mmgsdi task state before posting simlock evt
09/07/15   bcho    Migrate to mcfg EFS APIs
09/08/15   bcho    Run SIMLOCK Algo in Onchip SIM 
08/10/15   stv     To send temporary unlock status indication
06/04/15   bcho    Run algorithm after a lock operation
05/04/15   vv      Support for more than 1 bytes of GID1/GID2
03/11/15   vv      Added support for new perso status
02/17/15   vv      Indicate the reason for invalid subscription
11/24/14   vv      Send simlock run algo request if pre-perso caching is done
11/14/14   hh      Fix compilation warnings
11/05/14   am      Changes to eliminate memory leak
10/21/14   vv      Add support for late simlock config registration
09/29/14   vv      Added support for emergency only mode
08/29/14   vv      Added support for temporary unlock
08/28/14   vv      Added support for unlock via REFRESH
08/27/14   tl      Introduce network related SIM Lock slot policies
08/22/14   tl      Change client notification when app state is brought
                   down from SUB_READY TO PERSO_READY
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
07/16/14   tl      Fix potential memory leak
07/08/14   vv      Fix for compiler warnings
06/30/14   vv      Remove Perso code
05/12/14   vv      Fix compiler errors
04/29/14   vv      Added check for halt subscription
04/15/14   vv      Added support for quad SIM
04/09/14   tl      Notify all session associated with application
03/25/14   tl      Secondary revisions
02/25/14   tl      Initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "comdef.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdi_simlock.h"
#include "mmgsdilib_common.h"
#include "mmgsdicache.h"
#include "mmgsdi_card_init.h"
#include "simlock_modem_lib.h"
#include "fs_lib.h"
#include "uim_common_efs.h"
#include "mmgsdi_psm.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

#ifdef FEATURE_SIMLOCK
/*===========================================================================
                  F U N C T I O N   P R O T O T Y P E S
============================================================================*/
/* ==========================================================================
   FUNCTION:      MMGSDI_SIMLOCK_CONVERT_STATUS

   DESCRIPTION:
     Converts the SIM Lock status to an MMGSDI status enum type.

   DEPENDENCIES:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_simlock_convert_status(
  simlock_result_enum_type           simlock_status
)
{
  switch (simlock_status)
  {
    case SIMLOCK_SUCCESS:
      return MMGSDI_SUCCESS;
      break;
    case SIMLOCK_GENERIC_ERROR:
      return MMGSDI_ERROR;
      break;
    case SIMLOCK_INCORRECT_KEY:
      return MMGSDI_PERSO_INVALID_CK;
      break;
    case SIMLOCK_INCORRECT_PARAMS:
      return MMGSDI_INCORRECT_PARAMS;
      break;
    case SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED:
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      break;
    case SIMLOCK_MAX_RETRIES_REACHED:
      return MMGSDI_PERSO_CK_BLOCKED;
      break;
    case SIMLOCK_INVALID_STATE:
      return MMGSDI_APP_STATE_DENIED;
      break;
    case SIMLOCK_UNSUPPORTED:
      return MMGSDI_NOT_SUPPORTED;
      break;
    default:
      UIM_MSG_ERR_0("Invalid SIM Lock status");
      return MMGSDI_ERROR;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_simlock_convert_status */


/* ==========================================================================
   FUNCTION:      MMGSDI_SIMLOCK_CONVERT_SLOT_ID

   DESCRIPTION:
     Converts the SIM Lock slot id to an MMGSDI slot id.

   DEPENDENCIES:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_simlock_convert_slot_id(
  simlock_slot_enum_type             simlock_slot_id,
  mmgsdi_slot_id_enum_type         * mmgsdi_slot_id_ptr
)
{
  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_slot_id_ptr);

  switch (simlock_slot_id)
  {
    case SIMLOCK_SLOT_1:
      *mmgsdi_slot_id_ptr = MMGSDI_SLOT_1;
      break;
    case SIMLOCK_SLOT_2:
      *mmgsdi_slot_id_ptr = MMGSDI_SLOT_2;
      break;
    case SIMLOCK_SLOT_3:
      *mmgsdi_slot_id_ptr = MMGSDI_SLOT_3;
      break;
    case SIMLOCK_SLOT_4:
    default:
      UIM_MSG_ERR_0("Invalid SIM Lock status");
      return MMGSDI_ERROR;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_simlock_convert_slot_id */


/* ==========================================================================
FUNCTION MGSDI_SIMLOCK_CONVERT_MMGSDI_SLOT_ID

DESCRIPTION:
  This is an utility function that converts MMGSDI slot id
  to SIMLOCK slot id

DEPENDENCIES:
  None

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS:
  None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_simlock_convert_mmgsdi_slot_id(
  mmgsdi_slot_id_enum_type             mmgsdi_slot_id,
  simlock_slot_enum_type             * simlock_slot_id_ptr
)
{
  MMGSDIUTIL_RETURN_IF_NULL(simlock_slot_id_ptr);

  switch (mmgsdi_slot_id)
  {
    case MMGSDI_SLOT_1:
      *simlock_slot_id_ptr = SIMLOCK_SLOT_1;
      break;

    case MMGSDI_SLOT_2:
      *simlock_slot_id_ptr = SIMLOCK_SLOT_2;
      break;

    case MMGSDI_SLOT_3:
      *simlock_slot_id_ptr = SIMLOCK_SLOT_3;
      break;

    default:
      UIM_MSG_ERR_0("Invalid slot id");
      return MMGSDI_ERROR;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_simlock_convert_mmgsdi_slot_id */


/* ==========================================================================
FUNCTION:      MMGSDI_SIMLOCK_CONVERT_CATEGORY

DESCRIPTION:
  Converts the SIM Lock category to an MMGSDI enum type.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS:
  None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_simlock_convert_category(
  simlock_category_enum_type            simlock_category,
  mmgsdi_perso_feature_enum_type      * mmgsdi_category_ptr
)
{
  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_category_ptr);

  switch (simlock_category)
  {
    case SIMLOCK_CATEGORY_3GPP_NW:
      *mmgsdi_category_ptr = MMGSDI_PERSO_NW;
      break;
    case SIMLOCK_CATEGORY_3GPP_NS:
      *mmgsdi_category_ptr = MMGSDI_PERSO_NS;
      break;
    case SIMLOCK_CATEGORY_3GPP_SP:
      *mmgsdi_category_ptr = MMGSDI_PERSO_SP;
      break;
    case SIMLOCK_CATEGORY_3GPP_CP:
      *mmgsdi_category_ptr = MMGSDI_PERSO_CP;
      break;
    case SIMLOCK_CATEGORY_3GPP_SIM:
      *mmgsdi_category_ptr = MMGSDI_PERSO_SIM;
      break;
    case SIMLOCK_CATEGORY_3GPP_SPN:
      *mmgsdi_category_ptr = MMGSDI_PERSO_SPN;
      break;
    case SIMLOCK_CATEGORY_3GPP_ICCID:
      *mmgsdi_category_ptr = MMGSDI_PERSO_ICCID;
      break;
    case SIMLOCK_CATEGORY_3GPP2_NW_TYPE1:
      *mmgsdi_category_ptr = MMGSDI_PERSO_RUIM_NW1;
      break;
    case SIMLOCK_CATEGORY_3GPP2_NW_TYPE2:
      *mmgsdi_category_ptr = MMGSDI_PERSO_RUIM_NW2;
      break;
    case SIMLOCK_CATEGORY_3GPP2_HRPD:
      *mmgsdi_category_ptr = MMGSDI_PERSO_RUIM_HRPD;
      break;
    case SIMLOCK_CATEGORY_3GPP2_SP:
      *mmgsdi_category_ptr = MMGSDI_PERSO_RUIM_SP;
      break;
    case SIMLOCK_CATEGORY_3GPP2_CP:
      *mmgsdi_category_ptr = MMGSDI_PERSO_RUIM_CP;
      break;
    case SIMLOCK_CATEGORY_3GPP2_RUIM:
      *mmgsdi_category_ptr = MMGSDI_PERSO_RUIM_RUIM;
      break;
    default:
      UIM_MSG_ERR_1("Invalid SIM Lock category: 0x%x", simlock_category);
      *mmgsdi_category_ptr = MMGSDI_MAX_PERSO_FEATURE_ENUM;
      return MMGSDI_ERROR;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_simlock_convert_category */


/* ==========================================================================
FUNCTION MGSDI_SIMLOCK_CHECK_CATEGORY_STATUS

DESCRIPTION:
  This function checks whether a simlock category is enabled or not

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE  : If the category is enabled
  FALSE : Otherwise

SIDE EFFECTS:
  None
==========================================================================*/
boolean mmgsdi_simlock_check_category_status(
  mmgsdi_slot_id_enum_type             mmgsdi_slot_id,
  simlock_category_enum_type           category
)
{
  mmgsdi_return_enum_type  mmgsdi_status     = MMGSDI_SUCCESS;
  uint8                    slot_index        = 0;
  uint8                    category_index    = 0;

  mmgsdi_status = mmgsdi_util_get_slot_index(mmgsdi_slot_id,
                                             &slot_index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return TRUE;
  }

  for(category_index = 0; category_index < SIMLOCK_CATEGORY_COUNT_MAX; category_index++)
  {
    if(mmgsdi_simlock_status_table[slot_index][category_index].category == category)
    {
      return mmgsdi_simlock_status_table[slot_index][category_index].category_status;
    }
  }

  /* The default return value is TRUE, in case
     the status is not determined. This way the
     behavior of the calling function is less impactful */
  return TRUE;
} /* mmgsdi_simlock_check_category_status */


/* ==========================================================================
   FUNCTION:      MMGSDI_SIMLOCK_READ_BINARY_FILE

   DESCRIPTION:
     This function will read the input file from cache.

   DEPENDENCIES:
     None

   RETURN VALUE:
     uint8

   SIDE EFFECTS:
     None
==========================================================================*/
static uint8 mmgsdi_simlock_read_binary_file(
  mmgsdi_file_enum_type         file,
  mmgsdi_app_enum_type          app_type,
  mmgsdi_session_type_enum_type session_type,
  uint8                         offset,
  uint8                         buffer_size,
  uint8*                        buffer_ptr)
{
  mmgsdi_return_enum_type    mmgsdi_status   = MMGSDI_SUCCESS;
  mmgsdi_len_type            data_len        = 0;

  if(buffer_ptr == NULL || buffer_size == 0)
  {
    return 0;
  }

  mmgsdi_status = mmgsdi_cache_read_item_len(
                    file,
                    app_type,
                    session_type,
                    0,
                    &data_len,
                    NULL);
  
  if (mmgsdi_status != MMGSDI_SUCCESS || data_len == 0)
  {
    return 0;
  }

  if(data_len > buffer_size)
  {
    data_len = buffer_size;
  }

  mmgsdi_status = mmgsdi_cache_read_item(file,
                                         app_type,
                                         session_type,
                                         data_len,
                                         offset,
                                         &data_len,
                                         buffer_ptr,
                                         NULL);

  if(mmgsdi_status != MMGSDI_SUCCESS || data_len > buffer_size)
  {
    UIM_MSG_MED_4("Read for file: 0x%x failed with status: 0x%x, data_len: 0x%x, buffer_size: 0x%x",
                  file, mmgsdi_status, data_len, buffer_size);
    return 0;
  }
  return (uint8)data_len;
} /* mmgsdi_simlock_read_binary_file */


/* ==========================================================================
   FUNCTION:      MMGSDI_SIMLOCK_BUILD_ALGO_DATA

   DESCRIPTION:
     This function will read all relevant SIM Lock files, EF-IMSI, EF-GID1,
     EF-GID2 and for 3GPP applications, EF-AD, and package them in a SIM
     Lock request to be sent to the SIM Lock engine.

     It will also save the session IDs of the valid sessions as user data
     to be when the response is processed.

   DEPENDENCIES:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     None
==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_simlock_build_algo_data(
  simlock_run_algorithm_msg_req_type   * simlock_algo_data_ptr,
  mmgsdi_simlock_session_id_table_type * session_id_table_ptr
)
{
  mmgsdi_return_enum_type       mmgsdi_status        = MMGSDI_SUCCESS;
  mmgsdi_file_enum_type         imsi                 = MMGSDI_NO_FILE_ENUM;
  mmgsdi_file_enum_type         ad                   = MMGSDI_NO_FILE_ENUM;
  mmgsdi_file_enum_type         gid1                 = MMGSDI_NO_FILE_ENUM;
  mmgsdi_file_enum_type         gid2                 = MMGSDI_NO_FILE_ENUM;
  mmgsdi_slot_id_enum_type      slot_id              = MMGSDI_MAX_SLOT_ID_ENUM;
  uint8                         slot                 = 0;
  uint8                         app                  = 0;
  uint8                         sub                  = 0;
  mmgsdi_session_id_type        session_id           = 0;
  mmgsdi_session_type_enum_type session_type         = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_file_enum_type         spn                  = MMGSDI_NO_FILE_ENUM;
  simlock_iccid_file_type       iccid;

  MMGSDIUTIL_RETURN_IF_NULL_2(session_id_table_ptr, simlock_algo_data_ptr);

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  for (slot = 0; slot < MMGSDI_MAX_NUM_SLOTS && slot < SIMLOCK_SLOT_COUNT_MAX; slot++)
  {
    if (mmgsdi_data_slot_ptr[slot] == NULL ||
        mmgsdi_data_slot_ptr[slot]->mmgsdi_state != MMGSDI_STATE_CARD_INSERTED)
    {
      continue;
    }

    memset(&iccid, 0x00, sizeof(iccid));

    mmgsdi_status = mmgsdi_util_get_slot_id(slot, &slot_id);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      continue;
    }

    mmgsdi_status = mmgsdi_util_get_non_prov_session_type_from_slot(slot_id, &session_type);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      continue;
    }

    /* Retrieve ICCID value for the slot */
    iccid.iccid_len = mmgsdi_simlock_read_binary_file(MMGSDI_ICCID,
                             MMGSDI_APP_NONE,
                             session_type,
                             0,
                             sizeof(iccid.iccid_data),
                             iccid.iccid_data);

    for(app = 0, simlock_algo_data_ptr->card_data[slot].num_subscriptions = 0;
        app < MMGSDI_MAX_EXT_APP_INFO && simlock_algo_data_ptr->card_data[slot].num_subscriptions < SIMLOCK_SUBS_COUNT_MAX;
        app++)
    {
      if(mmgsdi_data_slot_ptr[slot]->app_info_pptr[app] == NULL ||
         mmgsdi_data_slot_ptr[slot]->app_info_pptr[app]->app_selected == FALSE)
      {
        continue;
      }

      if(!mmgsdi_data_slot_ptr[slot]->app_info_pptr[app]->pre_perso_files_cached)
      {
        /* Applications whose pre-perso cache is not complete will not proceed
           with algorithm. This is done to avoid sending the perso evt prematurely
           (i.e. before pre-perso caching is complete). Sending perso evt
           prematurely might result in clients reading files from our cache,
           which may not be ready */
        continue;
      }

      sub = simlock_algo_data_ptr->card_data[slot].num_subscriptions;

      switch (mmgsdi_data_slot_ptr[slot]->app_info_pptr[app]->app_data.app_type)
      {
        case MMGSDI_APP_SIM:
          simlock_algo_data_ptr->card_data[slot].subscription_data[sub].sub_type = SIMLOCK_SUBSCRIPTION_3GPP;
          imsi = MMGSDI_GSM_IMSI;
          ad   = MMGSDI_GSM_AD;
          gid1 = MMGSDI_GSM_GID1;
          gid2 = MMGSDI_GSM_GID2;
          spn  = MMGSDI_GSM_SPN;
          break;
        case MMGSDI_APP_USIM:
          simlock_algo_data_ptr->card_data[slot].subscription_data[sub].sub_type = SIMLOCK_SUBSCRIPTION_3GPP;
          imsi = MMGSDI_USIM_IMSI;
          ad   = MMGSDI_USIM_AD;
          gid1 = MMGSDI_USIM_GID1;
          gid2 = MMGSDI_USIM_GID2;
          spn  = MMGSDI_USIM_SPN;
          break;
        case MMGSDI_APP_RUIM:
          simlock_algo_data_ptr->card_data[slot].subscription_data[sub].sub_type = SIMLOCK_SUBSCRIPTION_3GPP2;
          imsi = MMGSDI_CDMA_IMSI_M;
          gid1 = MMGSDI_CDMA_GID1;
          gid2 = MMGSDI_CDMA_GID2;
          break;
        case MMGSDI_APP_CSIM:
          simlock_algo_data_ptr->card_data[slot].subscription_data[sub].sub_type = SIMLOCK_SUBSCRIPTION_3GPP2;
          imsi = MMGSDI_CSIM_IMSI_M;
          gid1 = MMGSDI_CSIM_GID1;
          gid2 = MMGSDI_CSIM_GID2;
          break;
        default:
          UIM_MSG_ERR_1("Invalid app type: 0x%x",
                        mmgsdi_data_slot_ptr[slot]->app_info_pptr[app]->app_data.app_type);
          continue;
      }

      mmgsdi_status = mmgsdi_util_get_prov_session_id_for_app(slot_id,
                                                              &mmgsdi_data_slot_ptr[slot]->app_info_pptr[app]->app_data,
                                                              &session_id);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        continue;
      }

      mmgsdi_status = mmgsdi_util_get_session_type(session_id, &session_type, NULL);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_1("Invalid session id 0x%x", session_id);
        continue;
      }

      /* Store the cached EF-IMSI/EF-IMSI_M/T value */
      simlock_algo_data_ptr->card_data[slot].subscription_data[sub].imsi.imsi_len = 0;
      memset(simlock_algo_data_ptr->card_data[slot].subscription_data[sub].imsi.imsi,
             0xFF,
             SIMLOCK_IMSI_FILE_LEN);

      simlock_algo_data_ptr->card_data[slot].subscription_data[sub].imsi.imsi_len = 
          mmgsdi_simlock_read_binary_file(
            imsi,
            mmgsdi_data_slot_ptr[slot]->app_info_pptr[app]->app_data.app_type,
            session_type,
            0,
            sizeof(simlock_algo_data_ptr->card_data[slot].subscription_data[sub].imsi.imsi),
            simlock_algo_data_ptr->card_data[slot].subscription_data[sub].imsi.imsi);

      if (simlock_algo_data_ptr->card_data[slot].subscription_data[sub].imsi.imsi_len == 0)
      {
        UIM_MSG_ERR_0("Failed to read cached IMSI");
        /* Applications where the IMSI has not been initalized will
           not proceed with algorithm */
        continue;
      }

      /* Store the cached EF-GID1 value */
      simlock_algo_data_ptr->card_data[slot].subscription_data[sub].gid1.gid_len = 0;
      memset(simlock_algo_data_ptr->card_data[slot].subscription_data[sub].gid1.gid_data,
             0xFF,
             sizeof(simlock_algo_data_ptr->card_data[slot].subscription_data[sub].gid1.gid_data));

      simlock_algo_data_ptr->card_data[slot].subscription_data[sub].gid1.gid_len = 
          mmgsdi_simlock_read_binary_file(
            gid1,
            mmgsdi_data_slot_ptr[slot]->app_info_pptr[app]->app_data.app_type,
            session_type,
            0,
            sizeof(simlock_algo_data_ptr->card_data[slot].subscription_data[sub].gid1.gid_data),
            simlock_algo_data_ptr->card_data[slot].subscription_data[sub].gid1.gid_data);

      /* Store the cached EF-GID2 value */
      simlock_algo_data_ptr->card_data[slot].subscription_data[sub].gid2.gid_len = 0;
      memset(simlock_algo_data_ptr->card_data[slot].subscription_data[sub].gid2.gid_data,
             0xFF,
             sizeof(simlock_algo_data_ptr->card_data[slot].subscription_data[sub].gid2.gid_data));

      simlock_algo_data_ptr->card_data[slot].subscription_data[sub].gid2.gid_len = 
          mmgsdi_simlock_read_binary_file(
            gid2,
            mmgsdi_data_slot_ptr[slot]->app_info_pptr[app]->app_data.app_type,
            session_type,
            0,
            sizeof(simlock_algo_data_ptr->card_data[slot].subscription_data[sub].gid2.gid_data),
            simlock_algo_data_ptr->card_data[slot].subscription_data[sub].gid2.gid_data);

      /* EF-AD is only used for SIM lock in 3GPP applications */
      if (mmgsdi_data_slot_ptr[slot]->app_info_pptr[app]->app_data.app_type == MMGSDI_APP_SIM ||
          mmgsdi_data_slot_ptr[slot]->app_info_pptr[app]->app_data.app_type == MMGSDI_APP_USIM)
      {
        /* initalize data to invalid */
        simlock_algo_data_ptr->card_data[slot].subscription_data[sub].ad.ad_len = 0;

        simlock_algo_data_ptr->card_data[slot].subscription_data[sub].ad.ad_len =
                 mmgsdi_simlock_read_binary_file(ad,
                                                 mmgsdi_data_slot_ptr[slot]->app_info_pptr[app]->app_data.app_type,
                                                 session_type,
                                                 0,
                                                 sizeof(simlock_algo_data_ptr->card_data[slot].subscription_data[sub].ad.ad),
                                                 simlock_algo_data_ptr->card_data[slot].subscription_data[sub].ad.ad);
      }

      /* EF-SPN is only required for 3GPP subscriptions */
      if (simlock_algo_data_ptr->card_data[slot].subscription_data[sub].sub_type == SIMLOCK_SUBSCRIPTION_3GPP)
      {
        uint8    spn_read_len = 0;

        /* Store the cached EF-SPN value */
        simlock_algo_data_ptr->card_data[slot].subscription_data[sub].spn.spn_valid = FALSE;
        memset(simlock_algo_data_ptr->card_data[slot].subscription_data[sub].spn.spn_data,
               0xFF,
               SIMLOCK_SPN_LEN);

        spn_read_len = mmgsdi_simlock_read_binary_file(spn,
                                                       mmgsdi_data_slot_ptr[slot]->app_info_pptr[app]->app_data.app_type,
                                                       session_type,
                                                       1,
                                                       sizeof(simlock_algo_data_ptr->card_data[slot].subscription_data[sub].spn.spn_data),
                                                       simlock_algo_data_ptr->card_data[slot].subscription_data[sub].spn.spn_data);
        if (spn_read_len != 0)
        {
          simlock_algo_data_ptr->card_data[slot].subscription_data[sub].spn.spn_valid = TRUE;
        }
      }

      /* Copy ICCID value for the subscription */
      simlock_algo_data_ptr->card_data[slot].subscription_data[sub].iccid = iccid;

      simlock_algo_data_ptr->card_data[slot].subscription_data[sub].service_status =
        mmgsdi_data_slot_ptr[slot]->app_info_pptr[app]->service_status;

      session_id_table_ptr->session_id[slot][sub] = session_id;
      simlock_algo_data_ptr->card_data[slot].num_subscriptions++;
    }
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_simlock_build_algo_data */


/* ==========================================================================
FUNCTION MMGSDI_SIMLOCK_EVT_CB

DESCRIPTION
  Callback for all MMGSDI related events that are triggered by the SIM Lock
  engine. This will be called in the SIM Lock context and the function will
  post the message back to MMGSDI.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_simlock_evt_cb (
  const simlock_result_enum_type              status,
  const simlock_event_data_type               event_data
)
{
  mmgsdi_task_cmd_type     * task_cmd_ptr        = NULL;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr,
                                     sizeof(mmgsdi_task_cmd_type));
  if (task_cmd_ptr == NULL)
  {
    return;
  }

  MMGSDIUTIL_ENTER_TASK_STATE_CRIT_SECT;

  if(mmgsdi_task_state[MMGSDI_TASK_MAIN] == MMGSDI_TASK_ACTIVE)
  {
    task_cmd_ptr->cmd.cmd_enum = MMGSDI_SIMLOCK_EVT;
    task_cmd_ptr->cmd.cmd.simlock_evt.status = status;
    task_cmd_ptr->cmd.cmd.simlock_evt.event_data.evt_type = event_data.evt_type;

    if(task_cmd_ptr->cmd.cmd.simlock_evt.event_data.evt_type == SIMLOCK_UNLOCK_EVENT)
    {
      task_cmd_ptr->cmd.cmd.simlock_evt.event_data.message.unlock.num_retries_max
        = event_data.message.unlock.num_retries_max;
      task_cmd_ptr->cmd.cmd.simlock_evt.event_data.message.unlock.curr_retries
        = event_data.message.unlock.curr_retries;
      task_cmd_ptr->cmd.cmd.simlock_evt.event_data.message.unlock.category
        = event_data.message.unlock.category;
      task_cmd_ptr->cmd.cmd.simlock_evt.event_data.message.unlock.slot
        = event_data.message.unlock.slot;
    }
    else if(task_cmd_ptr->cmd.cmd.simlock_evt.event_data.evt_type == SIMLOCK_TEMPORARY_UNLOCK_EVENT)
    {
      task_cmd_ptr->cmd.cmd.simlock_evt.event_data.message.temporary_unlock.slot
                        = event_data.message.temporary_unlock.slot;
      task_cmd_ptr->cmd.cmd.simlock_evt.event_data.message.temporary_unlock.category
                        = event_data.message.temporary_unlock.category;
    }
    else if (task_cmd_ptr->cmd.cmd.simlock_evt.event_data.evt_type == SIMLOCK_DEVICE_MODE_EVENT)
    {
      task_cmd_ptr->cmd.cmd.simlock_evt.event_data.message.device_mode.mode
                        = event_data.message.device_mode.mode;
    }
    else if(task_cmd_ptr->cmd.cmd.simlock_evt.event_data.evt_type == SIMLOCK_REGISTER_REMOTE_SFS_EVENT)
    {
      /* No event data */
    }
    else if(task_cmd_ptr->cmd.cmd.simlock_evt.event_data.evt_type == SIMLOCK_LOCK_EVENT)
    {
      task_cmd_ptr->cmd.cmd.simlock_evt.event_data.message.lock.slot
                        = event_data.message.lock.slot;
      task_cmd_ptr->cmd.cmd.simlock_evt.event_data.message.lock.category
                        = event_data.message.lock.category;
    }

    (void)q_link(task_cmd_ptr, &task_cmd_ptr->link);

    q_put(&mmgsdi_simlock_rsp_q, &task_cmd_ptr->link);

    (void)rex_set_sigs(UIM_GSDI_TCB, MMGSDI_SIMLOCK_MSG_SIG);
  }
  else
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
  }

  MMGSDIUTIL_LEAVE_TASK_STATE_CRIT_SECT;
} /* mmgsdi_simlock_evt_cb */


/* ==========================================================================
FUNCTION MMGSDI_SIMLOCK_DUMMY_CB

DESCRIPTION
  Dummy callback provided to SIM Lock engine if Onchip SIM is activated.
  This will be called in the SIM Lock context.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_simlock_dummy_cb (
  simlock_result_enum_type                    status,
  const simlock_message_response_data_type  * resp_ptr,
  const void                                * user_data_ptr
)
{
  (void)status;
  (void)resp_ptr;
  MMGSDIUTIL_TMC_MEM_FREE(user_data_ptr);
} /* mmgsdi_simlock_dummy_cb */


/* ==========================================================================
FUNCTION MMGSDI_SIMLOCK_PSM_INIT_CB

DESCRIPTION
  Callback for sim lock run algo when in psm init mode

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_simlock_psm_init_cb (
  simlock_result_enum_type                    status,
  const simlock_message_response_data_type  * resp_ptr,
  const void                                * user_data_ptr
)
{
  /* Signal the main task to proceed with 
     triggering of early card and session events */
  MMGSDIUTIL_ENTER_TASK_STATE_CRIT_SECT;
  if(mmgsdi_task_state[MMGSDI_TASK_MAIN] == MMGSDI_TASK_ACTIVE)
  {
    (void)rex_set_sigs(UIM_GSDI_TCB, MMGSDI_SYNC_WAIT_SIG);
  }
  MMGSDIUTIL_LEAVE_TASK_STATE_CRIT_SECT;

  (void)status;
  (void)resp_ptr;
  MMGSDIUTIL_TMC_MEM_FREE(user_data_ptr);
}/* mmgsdi_simlock_psm_init_cb */


/* ==========================================================================
FUNCTION MMGSDI_SIMLOCK_CB

DESCRIPTION
  Callback for all MMGSDI commands that interact with the SIM Lock engine.
  This will be called in the SIM Lock context and the function will post
  the message back to MMGSDI.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_simlock_cb (
  simlock_result_enum_type                    status,
  const simlock_message_response_data_type  * resp_ptr,
  const void                                * user_data_ptr
)
{
  mmgsdi_task_cmd_type     * task_cmd_ptr        = NULL;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr,
                                     sizeof(mmgsdi_task_cmd_type));

  MMGSDIUTIL_ENTER_TASK_STATE_CRIT_SECT;

  if((mmgsdi_task_state[MMGSDI_TASK_MAIN] == MMGSDI_TASK_ACTIVE) &&
     (resp_ptr != NULL) &&
     (task_cmd_ptr != NULL))
  {
    task_cmd_ptr->cmd.cmd_enum = MMGSDI_SIMLOCK_RSP;
    task_cmd_ptr->cmd.cmd.simlock_rsp.user_data_ptr = (const mmgsdi_simlock_session_id_table_type*)user_data_ptr;
    task_cmd_ptr->cmd.cmd.simlock_rsp.status = status;
    mmgsdi_memscpy(&task_cmd_ptr->cmd.cmd.simlock_rsp.resp_data,
                   sizeof(simlock_message_response_data_type),
                   resp_ptr,
                   sizeof(simlock_message_response_data_type));

    (void)q_link(task_cmd_ptr, &task_cmd_ptr->link);

    q_put(&mmgsdi_simlock_rsp_q, &task_cmd_ptr->link);

    (void)rex_set_sigs(UIM_GSDI_TCB, MMGSDI_SIMLOCK_MSG_SIG);
  }
  else
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(user_data_ptr);
  }

  MMGSDIUTIL_LEAVE_TASK_STATE_CRIT_SECT;
} /* mmgsdi_simlock_cb */


/* ==========================================================================
FUNCTION MMGSDI_SIMLOCK_GET_LOCKED_FEATURE_FOR_APP

DESCRIPTION
  This function retrieves the feature that is locked for an application
  on a slot.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_simlock_get_locked_feature_for_app (
  mmgsdi_slot_id_enum_type           slot,
  mmgsdi_app_enum_type               app_type,
  mmgsdi_perso_feature_enum_type  *  feature_ptr
)
{
  uint8                      app_index         = 0;
  mmgsdi_slot_data_type    * slot_data_ptr     = NULL;

  if(feature_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot);
  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  for(app_index = 0; app_index < MMGSDI_MAX_EXT_APP_INFO; app_index++)
  {
    if(slot_data_ptr->app_info_pptr == NULL ||
       slot_data_ptr->app_info_pptr[app_index] == NULL ||
       slot_data_ptr->app_info_pptr[app_index]->app_selected == FALSE ||
       slot_data_ptr->app_info_pptr[app_index]->app_data.app_type != app_type)
    {
      continue;
    }

    if(slot_data_ptr->app_info_pptr[app_index]->perso_state != MMGSDI_SIMLOCK_UNLOCK_REQUIRED)
    {
      break;
    }

    if(slot_data_ptr->app_info_pptr[app_index]->perso_info.feature == MMGSDI_MAX_PERSO_FEATURE_ENUM)
    {
      break;
    }

    *feature_ptr = slot_data_ptr->app_info_pptr[app_index]->perso_info.feature;
    return MMGSDI_SUCCESS;
  }

  return MMGSDI_ERROR;
} /* mmgsdi_simlock_get_locked_feature_for_app */


/* ==========================================================================
FUNCTION MMGSDI_SIMLOCK_UPDATE_STATUS_OR_FEATURE_IF_NEEDED

DESCRIPTION
  This function update the perso status or the feature or both based on
  the failure reason.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_simlock_update_status_or_feature_if_needed (
  mmgsdi_slot_id_enum_type                         mmgsdi_slot,
  mmgsdi_app_enum_type                             update_app_type,
  simlock_subscription_invalid_reason_type         failure_reason,
  mmgsdi_perso_status_enum_type                *   perso_status_ptr,
  mmgsdi_perso_feature_enum_type               *   feature_ptr
)
{
  uint8       slot_index  = 0;

  if((perso_status_ptr == NULL) ||
     (feature_ptr == NULL))
  {
    return;
  }

  switch (failure_reason)
  {
    case SIMLOCK_SUBSCRIPTION_INVALID_REASON_APP_REQUIRED_BUT_NOT_PRESENT:
      *perso_status_ptr = MMGSDI_PERSO_STATUS_IN_PROGRESS;
      *feature_ptr      = MMGSDI_MAX_PERSO_FEATURE_ENUM;
      break;

    case SIMLOCK_SUBSCRIPTION_INVALID_REASON_APP_REQUIRED_BUT_LOCKED:
      /* Nothing to do in this case because the SimLock
         engine already takes care of setting the correct failure category */
      break;

    case SIMLOCK_SUBSCRIPTION_INVALID_REASON_APP_ON_SLOT1_REQUIRED_BUT_NOT_PRESENT:
      if((mmgsdi_slot == MMGSDI_SLOT_2) ||
         (mmgsdi_slot == MMGSDI_SLOT_3))
      {
        *perso_status_ptr = MMGSDI_PERSO_STATUS_IN_PROGRESS;
        *feature_ptr      = MMGSDI_MAX_PERSO_FEATURE_ENUM;
      }
      break;

    case SIMLOCK_SUBSCRIPTION_INVALID_REASON_APP_ON_SLOT1_REQUIRED_BUT_LOCKED:
      if((mmgsdi_slot == MMGSDI_SLOT_2) ||
         (mmgsdi_slot == MMGSDI_SLOT_3))
      {
        if(update_app_type == MMGSDI_APP_USIM)
        {
          /* If the USIM app is locked on the slot-1, then use the same
             feature to indicate the USIM app on the slot_2 or slot_3, as locked.
             If not, then use the feature which is locked for CSIM */
          if(mmgsdi_simlock_get_locked_feature_for_app(MMGSDI_SLOT_1, MMGSDI_APP_USIM, feature_ptr) !=
               MMGSDI_SUCCESS)
          {
            if(mmgsdi_simlock_get_locked_feature_for_app(MMGSDI_SLOT_1, MMGSDI_APP_CSIM, feature_ptr) !=
                 MMGSDI_SUCCESS)
            {
              /* Nothing else to do, if the function returns a failure */
            }
          }
        }
        else if(update_app_type == MMGSDI_APP_CSIM)
        {
          /* If the CSIM app is locked on the slot-1, then use the same
             feature to indicate the USIM app on the slot_2 or slot_3, as locked.
             If not, then use the feature which is locked for USIM */
          if(mmgsdi_simlock_get_locked_feature_for_app(MMGSDI_SLOT_1, MMGSDI_APP_CSIM, feature_ptr) !=
               MMGSDI_SUCCESS)
          {
            if(mmgsdi_simlock_get_locked_feature_for_app(MMGSDI_SLOT_1, MMGSDI_APP_USIM, feature_ptr) !=
                 MMGSDI_SUCCESS)
            {
              /* Nothing else to do, if the function returns a failure */
            }
          }
        }
      }
      break;

    case SIMLOCK_SUBSCRIPTION_INVALID_REASON_APP_ON_ANY_SLOT_REQUIRED_BUT_LOCKED:
      for(slot_index = 0; slot_index < MMGSDI_MAX_NUM_SLOTS; slot_index++)
      {
        if(mmgsdi_util_get_slot_id(slot_index, &mmgsdi_slot) != MMGSDI_SUCCESS)
        {
          continue;
        }
        if(update_app_type == MMGSDI_APP_USIM)
        {
          /* If the USIM app is locked on any slot, then use the same
             feature to indicate the USIM app on the other slots, as locked.
             If not, then use the feature which is locked for CSIM */
          if(mmgsdi_simlock_get_locked_feature_for_app(mmgsdi_slot, MMGSDI_APP_USIM, feature_ptr) ==
               MMGSDI_SUCCESS)
          {
            break;
          }
          if(mmgsdi_simlock_get_locked_feature_for_app(mmgsdi_slot, MMGSDI_APP_CSIM, feature_ptr) ==
                 MMGSDI_SUCCESS)
          {
            break;
          }
        }
        else if(update_app_type == MMGSDI_APP_CSIM)
        {
          /* If the CSIM app is locked on any slot, then use the same
             feature to indicate the USIM app on the other slots, as locked.
             If not, then use the feature which is locked for USIM */
          if(mmgsdi_simlock_get_locked_feature_for_app(mmgsdi_slot, MMGSDI_APP_CSIM, feature_ptr) ==
               MMGSDI_SUCCESS)
          {
            break;
          }
          if(mmgsdi_simlock_get_locked_feature_for_app(mmgsdi_slot, MMGSDI_APP_USIM, feature_ptr) ==
                 MMGSDI_SUCCESS)
          {
            break;
          }
        }
      }
      break;

    case SIMLOCK_SUBSCRIPTION_INVALID_REASON_SUB_ON_SLOT1_EXPIRED:
      if((mmgsdi_slot == MMGSDI_SLOT_2) ||
         (mmgsdi_slot == MMGSDI_SLOT_3))
      {
        *feature_ptr = MMGSDI_MAX_PERSO_FEATURE_ENUM;
      }
      break;

    case SIMLOCK_SUBSCRIPTION_INVALID_REASON_WAIT_FOR_FULL_SRV_ON_SLOT1:
      if((mmgsdi_slot == MMGSDI_SLOT_2) ||
         (mmgsdi_slot == MMGSDI_SLOT_3))
      {
        *perso_status_ptr = MMGSDI_PERSO_STATUS_IN_PROGRESS;
        *feature_ptr      = MMGSDI_MAX_PERSO_FEATURE_ENUM;
      }
      break;

    default:
      break;
  }
} /* mmgsdi_simlock_update_status_or_feature_if_needed */


/* ==========================================================================
FUNCTION MMGSDI_SIMLOCK_CHECK_HALT_SUBSCRIPTION_STATUS

DESCRIPTION
  This function checks status of the halt subscription EFS item

DEPENDENCIES
  None

RETURN VALUE
  TRUE  : if the halt subscription item is enabled
  FALSE : otherwise

SIDE EFFECTS
  None
===========================================================================*/
static boolean mmgsdi_simlock_check_halt_subscription_status (
  void
)
{
  boolean                           simlock_halt_sub_setting  = FALSE;
  uim_common_efs_status_enum_type   efs_read_status           = UIM_COMMON_EFS_ERROR;

  /* Get halt subscription config value from efs */
  efs_read_status = uim_common_efs_read(UIM_COMMON_EFS_MMGSDI_HALT_SUBSCRIPTION,
                                        UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                        UIM_COMMON_EFS_DEVICE,
                                        (uint8*) (&simlock_halt_sub_setting),
                                        sizeof(boolean));

  UIM_MSG_HIGH_2("efs get status, 0x%x. Halt subscription setting, 0x%x",
                  efs_read_status, simlock_halt_sub_setting);

  return (simlock_halt_sub_setting ? TRUE : FALSE);
} /* mmgsdi_simlock_check_halt_subscription_status */


/* ==========================================================================
  FUNCTION:      MMGSDI_SIMLOCK_NOTIFY_SESSION_SUB_READY_TO_PERSO_READY

  DESCRIPTION:
    This function finds all sessions associated with the input slot and
    application and indicates to those sessions that the application has
    moved from subscription ready state to perso ready state.

    For clients running inside the GSDI task, we only need to send a PERSO
    Locked Event to indicate subscription has been brought down however for
    clients outside of the GSDI task, this scenario may not be handled correctly
    so we will need to send session changed events to indicate to clients that
    subscription ready has been brought down.

    Note: The reason this is sent to all sessions except for MMGSDI is because
          this is essentially just to notify to the clients in a straightforward
          way that the app state has changed so no need to clear any caches in
          MMGSDI.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
==========================================================================*/
static void mmgsdi_simlock_notify_session_sub_ready_to_perso_ready(
  mmgsdi_slot_id_enum_type                   slot_id,
  simlock_subscription_invalid_reason_type   failure_reason,
  const mmgsdi_int_app_info_type           * app_info_ptr
)
{
  int32                                   client_index        = 0;
  int32                                   session_index       = 0;
  mmgsdi_session_info_type              * session_info_ptr    = NULL;
  mmgsdi_perso_status_enum_type           perso_status        = MMGSDI_PERSO_STATUS_LOCKED;
  mmgsdi_evt_session_notify_type          notify_info;
  char                                    rex_task_name[REX_TASK_NAME_LEN + 1] = {0};

  if(app_info_ptr == NULL)
  {
    return;
  }

  /* If the reason for sub not valid is due to device set in emergency 
     mode then set the perso status as in progress */
  if(failure_reason == SIMLOCK_SUBSCRIPTION_INVALID_REASON_EMERGENCY_ONLY)
  {
    perso_status = MMGSDI_PERSO_STATUS_IN_PROGRESS;
  }

  memset(&notify_info, 0x00, sizeof(notify_info));

  notify_info.notify_type = MMGSDI_EVT_NOTIFY_SINGLE_SESSION;
  notify_info.slot_id = slot_id;

  (void)rex_get_task_name(rex_self(), rex_task_name, sizeof(rex_task_name),NULL);

  for (client_index = 0; client_index < MMGSDI_MAX_CLIENT_INFO; client_index++)
  {
    if (mmgsdi_client_id_reg_table[client_index] == NULL)
    {
      continue;
    }

    for (session_index = 0; session_index < MMGSDI_MAX_SESSION_INFO; session_index++)
    {
      session_info_ptr =
        mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index];
      if ((session_info_ptr != NULL) &&
          (session_info_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
          (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] != NULL) &&
          (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id == slot_id) &&
          (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr != NULL) &&
          (mmgsdi_util_cmp_aid(&app_info_ptr->app_data,
                               &mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_data,
                               FALSE) == MMGSDI_SUCCESS))
      {
        notify_info.session_id = session_info_ptr->session_id;

        /* Send only PERSO Locked event to clients running in the GSDI task because
           we can handle this case internally without session changed events. */
        if(strcmp(rex_task_name, mmgsdi_client_id_reg_table[client_index]->client_name) == 0)
        {
          mmgsdi_evt_build_and_send_perso(
            notify_info,
            app_info_ptr,
            perso_status,
            app_info_ptr->perso_info.feature,
            app_info_ptr->perso_info.num_retries,
            app_info_ptr->perso_info.num_unblock_retries);
        }
        else
        {
          /* Send the session deactivate to clients since we are no longer in SUB READY */
          mmgsdi_evt_build_and_send_session_changed(
            notify_info,
            FALSE,
            app_info_ptr);

          mmgsdi_evt_build_and_notify_session_evt_from_state(session_info_ptr->session_id);
        }
      }
    }
  }
}/* mmgsdi_simlock_notify_session_sub_ready_to_perso_ready */


/* ==========================================================================
FUNCTION MMGSDI_SIMLOCK_PROCESS_RUN_ALGO_RESP

DESCRIPTION
  This function processes the response returned from the SIM Lock engine
  after it has run the algorithm and determines how to appropriately
  indicate the SIM Lock status for each application to the clients.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_simlock_process_run_algo_resp (
  mmgsdi_simlock_rsp_type           * simlock_rsp_ptr
)
{
  uint8                                   slot                 = 0;
  uint8                                   sub                  = 0;
  mmgsdi_return_enum_type                 mmgsdi_status        = MMGSDI_ERROR;
  simlock_run_algorithm_msg_resp_type   * run_algo_data_ptr    = NULL;
  mmgsdi_slot_id_enum_type                slot_id              = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_evt_session_notify_type          notify_info;
  mmgsdi_int_app_info_type              * app_info_ptr         = NULL;
  boolean                                 halt_subs            = FALSE;
  mmgsdi_perso_status_enum_type           status               = MMGSDI_PERSO_STATUS_NONE;

  MMGSDIUTIL_RETURN_IF_NULL_2(simlock_rsp_ptr, simlock_rsp_ptr->user_data_ptr);

  run_algo_data_ptr = &simlock_rsp_ptr->resp_data.message.run_algo_resp;

  UIM_MSG_HIGH_0("mmgsdi_simlock_process_run_algo_resp");

  /* Retrieve the halt subscription status */
  halt_subs = mmgsdi_simlock_check_halt_subscription_status();

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  for (slot = 0; slot < MMGSDI_MAX_NUM_SLOTS && slot < SIMLOCK_SLOT_COUNT_MAX; slot++)
  {
    if (mmgsdi_data_slot_ptr[slot] == NULL ||
        mmgsdi_data_slot_ptr[slot]->mmgsdi_state != MMGSDI_STATE_CARD_INSERTED ||
        run_algo_data_ptr->card_validity_data[slot].num_subscriptions == 0 ||
        run_algo_data_ptr->card_validity_data[slot].num_subscriptions > SIMLOCK_SUBS_COUNT_MAX)
    {
      continue;
    }

    for (sub = 0; sub < run_algo_data_ptr->card_validity_data[slot].num_subscriptions; sub++)
    {

      if (simlock_rsp_ptr->user_data_ptr->session_id[slot][sub] == MMGSDI_SESSION_ID_ZERO ||
          simlock_rsp_ptr->user_data_ptr->session_id[slot][sub] == MMGSDI_INVALID_SESSION_ID)
      {
        continue;
      }

      mmgsdi_status = mmgsdi_util_get_prov_session_info(simlock_rsp_ptr->user_data_ptr->session_id[slot][sub],
                                                        NULL,
                                                        &slot_id,
                                                        &app_info_ptr);

      if (mmgsdi_status != MMGSDI_SUCCESS || app_info_ptr == NULL)
      {
        continue;
      }

      memset(&notify_info, 0x00, sizeof(notify_info));
      notify_info.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
      notify_info.slot_id = slot_id;
      notify_info.session_id = simlock_rsp_ptr->user_data_ptr->session_id[slot][sub];

      if (run_algo_data_ptr->card_validity_data[slot].subscription_data[sub].num_retries_max == 0)
      {
        app_info_ptr->perso_info.num_retries = MMGSDI_SIMLOCK_MAX_RETRIES;
      }
      else if (run_algo_data_ptr->card_validity_data[slot].subscription_data[sub].curr_retries >=
               run_algo_data_ptr->card_validity_data[slot].subscription_data[sub].num_retries_max)
      {
        app_info_ptr->perso_info.num_retries = 0;
      }
      else
      {
        app_info_ptr->perso_info.num_retries =
          run_algo_data_ptr->card_validity_data[slot].subscription_data[sub].num_retries_max -
          run_algo_data_ptr->card_validity_data[slot].subscription_data[sub].curr_retries;
      }

      app_info_ptr->perso_info.num_unblock_retries = 0;

      if (run_algo_data_ptr->card_validity_data[slot].subscription_data[sub].is_subscription_valid)
      {
        app_info_ptr->perso_state = MMGSDI_SIMLOCK_VERIFIED;
        /* Set perso feature to MAX to indicate no feature failed verification */
        app_info_ptr->perso_info.feature = MMGSDI_MAX_PERSO_FEATURE_ENUM;

        if (halt_subs)
        {
          UIM_MSG_HIGH_0("Halting subscription. Waiting for SUB OK request");
          status = MMGSDI_PERSO_STATUS_WAIT_FOR_EXT_PERSO;
        }
        else
        {
          status = MMGSDI_PERSO_STATUS_DONE;
        }

        if (app_info_ptr->app_state == MMGSDI_APP_STATE_READY_FOR_PERSO)
        {
          mmgsdi_evt_build_and_send_perso(
            notify_info,
            app_info_ptr,
            status,
            MMGSDI_MAX_PERSO_FEATURE_ENUM,
            0,
            0);
        }
      }
      else
      {
        mmgsdi_perso_status_enum_type   perso_status = MMGSDI_PERSO_STATUS_LOCKED;

        app_info_ptr->perso_state = MMGSDI_SIMLOCK_UNLOCK_REQUIRED;

        (void)mmgsdi_simlock_convert_category(
          run_algo_data_ptr->card_validity_data[slot].subscription_data[sub].failed_category,
          &app_info_ptr->perso_info.feature);

        /* In case the subscription is marked as invalid due to
           a dependency on another subscription in the same slot or
           a different slot, the perso status or feature or both
           needs to be updated accordingly. The failure reason will
           help determine if the update is needed */
        mmgsdi_simlock_update_status_or_feature_if_needed(
                                   notify_info.slot_id,
                                   app_info_ptr->app_data.app_type,
                                   run_algo_data_ptr->card_validity_data[slot].subscription_data[sub].failure_reason,
                                   &perso_status,
                                   &app_info_ptr->perso_info.feature);

        if(perso_status == MMGSDI_PERSO_STATUS_IN_PROGRESS)
        {
          app_info_ptr->perso_state = MMGSDI_SIMLOCK_NOT_VERIFIED;
        }

        if (app_info_ptr->app_state == MMGSDI_APP_STATE_READY_FOR_PERSO)
        {
          /* Send the perso event only if the SIM is locked
             with a CK. In case the SIM is locked without CK or
             the device is in emergency only mode, sending the perso
             event with status as 'locked' might result in an unexpected
             UI behavior*/
          if(run_algo_data_ptr->card_validity_data[slot].subscription_data[sub].failure_reason !=
                            SIMLOCK_SUBSCRIPTION_INVALID_REASON_EMERGENCY_ONLY)
          {
           mmgsdi_evt_build_and_send_perso(
              notify_info,
              app_info_ptr,
              perso_status,
              app_info_ptr->perso_info.feature,
              app_info_ptr->perso_info.num_retries,
              app_info_ptr->perso_info.num_unblock_retries);
          }
        }
        /* If the app state is current in subscription ready and the
           perso status moves back to locked then the app needs to move
           back to perso ready state and the clients need to be notified */
        else if(app_info_ptr->app_state == MMGSDI_APP_STATE_READY)
        {
          /* Move the app state to PERSO_READY */
          mmgsdi_state_update_app_state_for_prov_app(
            simlock_rsp_ptr->user_data_ptr->session_id[slot][sub],
            MMGSDI_APP_STATE_READY,
            MMGSDI_APP_STATE_READY_FOR_PERSO,
            NULL);

          /* Set the session closed cause to PERSO_OPERATION_FAILED so that the algorithm
             is not unnecessarily rerun */
          app_info_ptr->session_closed_cause = MMGSDI_SESSION_CLOSED_CAUSE_PERSO_OPERATION_FAIL;

          mmgsdi_simlock_notify_session_sub_ready_to_perso_ready(
            slot_id,
            run_algo_data_ptr->card_validity_data[slot].subscription_data[sub].failure_reason,
            app_info_ptr);

          /* Reset the session closed cause since the session closed indication
             is really just to notify clients that we are falling back the PERSO
             READY state. MMGSDI is not notified of this session closed event so
             no reason to maintain the session close cause as PERSO_OPERATION_FAIL */
          app_info_ptr->session_closed_cause = MMGSDI_SESSION_CLOSED_CAUSE_UNKNOWN;
        }
      }
    }
  }

  MMGSDIUTIL_TMC_MEM_FREE(simlock_rsp_ptr->user_data_ptr);
  return MMGSDI_SUCCESS;
} /* mmgsdi_simlock_process_run_algo_resp */


/* ==========================================================================
FUNCTION MMGSDI_SIMLOCK_PROCESS_GET_STATUS_RESP

DESCRIPTION
  This function processes the response returned from the SIM Lock engine
  after we retrieving the current status information.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_simlock_process_get_status_resp (
  mmgsdi_simlock_rsp_type           * simlock_rsp_ptr
)
{
  uint8                                slot                  = 0;
  uint8                                category              = 0;
  simlock_get_status_msg_resp_type   * simlock_status_ptr    = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(simlock_rsp_ptr);

  simlock_status_ptr = &simlock_rsp_ptr->resp_data.message.get_status_resp;

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  for (slot = 0; slot < SIMLOCK_SLOT_COUNT_MAX; slot++)
  {
    for (category = 0; category < SIMLOCK_CATEGORY_COUNT_MAX; category++)
    {
      mmgsdi_simlock_status_table[slot][category].category =
        simlock_status_ptr->lock_info[slot].category_info[category].category;

      mmgsdi_simlock_status_table[slot][category].category_status =
        simlock_status_ptr->lock_info[slot].category_info[category].category_status;
    }
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_simlock_process_get_status_resp */


/* ==========================================================================
FUNCTION MMGSDI_SIMLOCK_PROCESS_RESPONSE

DESCRIPTION
  This function handles the response returned from the SIM Lock task.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_simlock_process_response (
  mmgsdi_simlock_rsp_type           * simlock_rsp_ptr
)
{
  if (simlock_rsp_ptr == NULL)
  {
    return;
  }

  if(simlock_rsp_ptr->status != SIMLOCK_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(simlock_rsp_ptr->user_data_ptr);
    UIM_MSG_ERR_0("SIM Lock response message not supported");
    return;
  }

  switch (simlock_rsp_ptr->resp_data.msg_type)
  {
    case SIMLOCK_RUN_ALGORITHM_MSG:
      (void)mmgsdi_simlock_process_run_algo_resp(simlock_rsp_ptr);
      break;

    case SIMLOCK_GET_STATUS_MSG:
      (void)mmgsdi_simlock_process_get_status_resp(simlock_rsp_ptr);
      break;

    case SIMLOCK_INIT_MSG:
    case SIMLOCK_SET_SIMLOCK_POLICY_MSG:
    case SIMLOCK_SET_LOCK_HCK_MSG:
    case SIMLOCK_SET_LOCK_CK_MSG:
    case SIMLOCK_UNLOCK_DEVICE_MSG:
    case SIMLOCK_GET_CATEGORY_DATA_MSG:
    case SIMLOCK_TEMPORARY_UNLOCK_MSG:
    default:
      UIM_MSG_ERR_0("SIM Lock response message not supported");
      break;
  }
} /* mmgsdi_simlock_process_response */


/* ==========================================================================
FUNCTION MMGSDI_SIMLOCK_RUN_ALGO_IF_NEEDED

DESCRIPTION
  This function checks if any app is in a perso ready state. If there is
  at least one app in the perso ready state, it runs the simlock algorithm.
  This might happen in case the remote simlock configuration registration
  happened after the app reached the ready for perso state.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_simlock_run_algo_if_needed (
  void
)
{
  uint8          slot_index = 0;
  uint8          app_index  = 0;

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  for(slot_index = 0; slot_index < MMGSDI_MAX_NUM_SLOTS; slot_index++)
  {
    if(mmgsdi_data_slot_ptr[slot_index] == NULL ||
       mmgsdi_data_slot_ptr[slot_index]->mmgsdi_state != MMGSDI_STATE_CARD_INSERTED)
    {
      continue;
    }

    for(app_index = 0; app_index < MMGSDI_MAX_EXT_APP_INFO; app_index++)
    {
      if(mmgsdi_data_slot_ptr[slot_index]->app_info_pptr[app_index] == NULL ||
         mmgsdi_data_slot_ptr[slot_index]->app_info_pptr[app_index]->app_selected == FALSE)
      {
        continue;
      }

      /* If the app is in ready for perso state, run the simlock algo */
      if(mmgsdi_data_slot_ptr[slot_index]->app_info_pptr[app_index]->app_state == MMGSDI_APP_STATE_READY_FOR_PERSO)
      {
        mmgsdi_simlock_build_and_run_algo(FALSE);
        return;
      }
    }
  }
} /* mmgsdi_simlock_run_algo_if_needed */


/* ==========================================================================
FUNCTION MMGSDI_SIMLOCK_PROCESS_EVENT_TYPE

DESCRIPTION
  This function handles the response returned from the SIM Lock task.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_simlock_process_event_type (
  mmgsdi_simlock_evt_type           * simlock_evt_ptr
)
{
  mmgsdi_slot_data_type            * slot_data_ptr     = NULL;
  mmgsdi_perso_feature_enum_type     perso_feature     = MMGSDI_MAX_PERSO_FEATURE_ENUM;
  uint32                             i                 = 0;
  mmgsdi_slot_id_enum_type           mmgsdi_slot_id    = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_perso_status_enum_type      perso_status      = MMGSDI_PERSO_STATUS_NONE;
  mmgsdi_evt_session_notify_type     notify_info;

  if (simlock_evt_ptr == NULL)
  {
    return;
  }

  UIM_MSG_HIGH_2("processing SIM Lock event, evt_type: 0x%x, status: 0x%x",
                 simlock_evt_ptr->event_data.evt_type, simlock_evt_ptr->status);

  /* If the remote registration event is received, check if there is
     a need to run the algorithm */
  if(simlock_evt_ptr->event_data.evt_type == SIMLOCK_REGISTER_REMOTE_SFS_EVENT)
  {
    mmgsdi_simlock_run_algo_if_needed();
    return;
  }

  /* If the evt_type is either lock_event or unlock_event, then perform a get
     status request. This will update the global mmgsdi_simlock_status_table,
     which maintains the status of each lock */
  if((simlock_evt_ptr->event_data.evt_type == SIMLOCK_LOCK_EVENT) ||
     (simlock_evt_ptr->event_data.evt_type == SIMLOCK_UNLOCK_EVENT))
  {
    (void)simlock_get_status(mmgsdi_simlock_cb,
                             NULL);
  }

  if(simlock_evt_ptr->status == SIMLOCK_SUCCESS)
  {
    mmgsdi_simlock_build_and_run_algo(FALSE);
  }
  else if(simlock_evt_ptr->status == SIMLOCK_INCORRECT_KEY ||
          simlock_evt_ptr->status == SIMLOCK_MAX_RETRIES_REACHED)
  {
    (void)mmgsdi_simlock_convert_slot_id(
            simlock_evt_ptr->event_data.message.unlock.slot,
            &mmgsdi_slot_id);

    slot_data_ptr = mmgsdi_util_get_slot_data_ptr(mmgsdi_slot_id);

    if (slot_data_ptr == NULL)
    {
      UIM_MSG_ERR_1("Invalid SIM Lock slot: 0x%x",
                    simlock_evt_ptr->event_data.message.unlock.slot);
      return;
    }

    for (i = 0; i < MMGSDI_MAX_EXT_APP_INFO; i++)
    {
      if (slot_data_ptr->mmgsdi_state != MMGSDI_STATE_CARD_INSERTED ||
          slot_data_ptr->app_info_pptr[i] == NULL)
      {
        continue;
      }

      (void)mmgsdi_simlock_convert_category(
        simlock_evt_ptr->event_data.message.unlock.category,
        &perso_feature);

      /* The prov app perso information needs to match the current
         information in the SIM Lock engine. The feature parameter will match
         when the application that is blocked from reaching PERSO ready is the
         same as the feature that failed to unlock */
      if (slot_data_ptr->app_info_pptr[i]->app_state == MMGSDI_APP_STATE_READY_FOR_PERSO &&
          slot_data_ptr->app_info_pptr[i]->perso_info.feature == perso_feature)
      {
        memset(&notify_info, 0x00, sizeof(notify_info));
        notify_info.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
        notify_info.slot_id = mmgsdi_slot_id;

        if(simlock_evt_ptr->status == SIMLOCK_INCORRECT_KEY)
        {
          perso_status = MMGSDI_PERSO_STATUS_LOCKED;
        }
        else
        {
          perso_status = MMGSDI_PERSO_STATUS_BLOCKED;
        }

        if (simlock_evt_ptr->event_data.message.unlock.num_retries_max == 0)
        {
          slot_data_ptr->app_info_pptr[i]->perso_info.num_retries = MMGSDI_SIMLOCK_MAX_RETRIES;
        }
        else if (simlock_evt_ptr->event_data.message.unlock.curr_retries >=
                 simlock_evt_ptr->event_data.message.unlock.num_retries_max)
        {
          slot_data_ptr->app_info_pptr[i]->perso_info.num_retries = 0;
        }
        else
        {
          slot_data_ptr->app_info_pptr[i]->perso_info.num_retries =
            simlock_evt_ptr->event_data.message.unlock.num_retries_max -
            simlock_evt_ptr->event_data.message.unlock.curr_retries;
        }

        slot_data_ptr->app_info_pptr[i]->perso_info.num_unblock_retries = 0;

        /* Send an event to clients to notify them that retries has changed */
        mmgsdi_evt_build_and_send_perso(
          notify_info,
          slot_data_ptr->app_info_pptr[i],
          perso_status,
          slot_data_ptr->app_info_pptr[i]->perso_info.feature,
          slot_data_ptr->app_info_pptr[i]->perso_info.num_retries,
          slot_data_ptr->app_info_pptr[i]->perso_info.num_unblock_retries);
      }
    }
  }

  return;
} /* mmgsdi_simlock_process_event_type */


/* ==========================================================================
FUNCTION MMGSDI_SIMLOCK_PROCESS_EVENT

DESCRIPTION
  This function handles events from the SIM Lock task.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_simlock_process_event (
  mmgsdi_simlock_evt_type           * simlock_evt_ptr
)
{
  if (simlock_evt_ptr == NULL)
  {
    return;
  }

  switch (simlock_evt_ptr->event_data.evt_type)
  {
    /* After SIM Lock has successfully completed an unlock or temporary unlock
       command we need to rerun the SIM Lock algorithm to see if an application
       can now proceed to SUB READY*/
    case SIMLOCK_UNLOCK_EVENT:
    case SIMLOCK_DEVICE_MODE_EVENT:
    case SIMLOCK_REGISTER_REMOTE_SFS_EVENT:
    case SIMLOCK_LOCK_EVENT:
      mmgsdi_simlock_process_event_type(simlock_evt_ptr);
      break;

    case SIMLOCK_TEMPORARY_UNLOCK_EVENT:
      if(simlock_evt_ptr->status == SIMLOCK_SUCCESS)
      {
        mmgsdi_slot_id_enum_type        slot_id = MMGSDI_MAX_SLOT_ID_ENUM;
        mmgsdi_perso_feature_enum_type  feature = MMGSDI_MAX_PERSO_FEATURE_ENUM;

        if((mmgsdi_simlock_convert_slot_id(simlock_evt_ptr->event_data.message.temporary_unlock.slot,
                                           &slot_id) == MMGSDI_SUCCESS) && 
            (mmgsdi_simlock_convert_category(simlock_evt_ptr->event_data.message.temporary_unlock.category,
                                             &feature) == MMGSDI_SUCCESS))
        {
          mmgsdi_perso_temp_unlock_evt_info_type  temp_unlock_info;

          memset(&temp_unlock_info, 0x00, sizeof(temp_unlock_info));

          /* Copy the data received from SIMLOCK to MMGSDI so that
             temp unlock event can be built */
          temp_unlock_info.slot_id = slot_id;
          temp_unlock_info.feature = feature;

          /* Send MMGSDI_PERSO_TEMPORARY_UNLOCK_EVT to clients */
          mmgsdi_evt_build_and_send_temp_unlock_evt(TRUE, temp_unlock_info, 0);
        }
      }
      mmgsdi_simlock_process_event_type(simlock_evt_ptr);
      break;

    default:
      UIM_MSG_ERR_0("Invalid SIM Lock event type");
      break;
  }
} /* mmgsdi_simlock_process_event */


/* ==========================================================================
FUNCTION MMGSDI_SIMLOCK_BUILD_AND_RUN_ALGO

DESCRIPTION
  This function reads the relevant files from the MMGSDI cache, creates
  a table with the current provisioning session ids and passes the information
  to the SIM Lock engine to begin running the algorithm.
  If Onchip SIM is activated, SIMLOCK module only requires to update valid IMSI
  list so algorithm result will be ignored. 

DEPENDENCIES
  The SIM Lock task and command queue must be up and running

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_simlock_build_and_run_algo (
  boolean ignore_result
)
{
  mmgsdi_return_enum_type                mmgsdi_status           = MMGSDI_SUCCESS;
  simlock_result_enum_type               simlock_status          = SIMLOCK_SUCCESS;
  mmgsdi_simlock_session_id_table_type * session_id_table_ptr    = NULL;
  simlock_run_algorithm_msg_req_type     simlock_algo_data;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(session_id_table_ptr,
    sizeof(mmgsdi_simlock_session_id_table_type));
  if(session_id_table_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  memset(&simlock_algo_data, 0x00, sizeof(simlock_algo_data));
  memset(session_id_table_ptr, 0x00, sizeof(mmgsdi_simlock_session_id_table_type));

  mmgsdi_status = mmgsdi_simlock_build_algo_data(&simlock_algo_data,
                                                 session_id_table_ptr);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Failed to save SIM Lock algo data: 0x%x", mmgsdi_status);
    MMGSDIUTIL_TMC_MEM_FREE(session_id_table_ptr);
    return mmgsdi_status;
  }

  if (mmgsdi_psm_get_status_early_init() &&
      ignore_result)
  {
    rex_sigs_type                       sigs_rcvd     = 0x00;

    (void)rex_clr_sigs(UIM_GSDI_TCB, MMGSDI_SYNC_WAIT_SIG);

    /* Pass PSM init callback if SIMLOCK algorithm is being run psm init sequence */
    simlock_status = simlock_run_algo(&simlock_algo_data,
                                      mmgsdi_simlock_psm_init_cb,
                                      session_id_table_ptr);

    /* Handle async response from simlock before proceeding with psm init */
    sigs_rcvd = mmgsdi_wait(MMGSDI_SYNC_WAIT_SIG);
    if (sigs_rcvd & MMGSDI_SYNC_WAIT_SIG)
    {
      UIM_MSG_HIGH_0("received MMGSDI_SYNC_WAIT_SIG");
      (void)rex_clr_sigs(UIM_GSDI_TCB, MMGSDI_SYNC_WAIT_SIG);
      simlock_status = SIMLOCK_SUCCESS;
    }
    else
    {
      /* if the signal received is not sync wait, then dont proceed */
      simlock_status = SIMLOCK_GENERIC_ERROR;
    }
  }
  else
  {
    /* Pass dummy callback if SIMLOCK algorithm is being run in Onchip SIM */
    simlock_status = simlock_run_algo(&simlock_algo_data,
                                      ignore_result ? mmgsdi_simlock_dummy_cb : mmgsdi_simlock_cb,
                                      session_id_table_ptr);
  }
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(session_id_table_ptr);
  }

  return mmgsdi_simlock_convert_status(simlock_status);
} /* mmgsdi_simlock_build_and_run_algo */
#endif /* FEATURE_SIMLOCK */


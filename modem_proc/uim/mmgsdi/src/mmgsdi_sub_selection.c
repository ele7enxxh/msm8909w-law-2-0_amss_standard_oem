/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


             M M G S D I  S U B S C R I P T I O N  S E L E C T I O N  F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains routines used for subscription provisioning
  from MMGSDI EFS.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2013 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_sub_selection.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/02/16   gm      Support for CDMALess Device
09/07/15   av      Migrate to mcfg EFS APIs
03/24/15   kk      Subs prov EFS error handling
07/28/14   vv      Remove unused data types
06/16/14   kk      Fixed usim/csim activation sequence
01/11/14   df      Fix off-target compilation error
01/10/14   df      Use v2.0 diag macros
10/16/13   df      Convert gsdi_efs functions to return mmgsdi type
10/04/13   rp      F3 message reduction
08/30/13   tl      Convert channel info table from static to dynamic allocation
08/21/13   spo     Resolving build error
08/20/13   spo     Initial version


===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdi_session.h"
#include "mmgsdi_evt.h"
#include "mmgsdicache.h"
#include "mmgsdi_efs.h"
#include "mmgsdi_nv.h"
#include "mmgsdi_sub_selection.h"

/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/* SIM/USIM app data in MMGSDI EFS is stored at Index 0 for corresponding slot */
#define MMGSDI_SUBS_AUTO_PROV_USIM_INDEX  0

/* RUIM/CSIM app data in MMGSDI EFS is stored at Index 1 for corresponding slot */
#define MMGSDI_SUBS_AUTO_PROV_CSIM_INDEX  1

static mmgsdi_return_enum_type mmgsdi_write_to_subs_prov_efs(
  void
);

static boolean mmgsdi_check_subs_prov_info_update_needed
(
  uint8                          slot_index,
  mmgsdi_data_type               iccid,
  mmgsdi_session_type_enum_type  session_type,
  mmgsdi_aid_type                aid_info
);

static void mmgsdi_provision_app_from_efs_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr
);

/* These globals are specific to Automatic Subscription provisioning in MMGSDI */
static mmgsdi_subs_prov_info_type     mmgsdi_subs_prov_info = {0} ;

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#else
char* mmgsdi_efs_subs_prov_root_dir = "mmgsdi";
char* mmgsdi_efs_subs_prov_mgr      = "mmgsdi/subs_prov_mgr";
char* mmgsdi_efs_subs_prov_info     = "mmgsdi/subs_prov_mgr/subs_prov_info.txt";
#endif

/*===========================================================================
FUNCTION MMGSDI_WRITE_TO_SUBS_PROV_EFS

DESCRIPTION
  This function will write the content in global mmgsdi_subs_prov_info to
  MMGSDI EFS "subs_prov_info.txt"

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
============================================================================ */
static mmgsdi_return_enum_type mmgsdi_write_to_subs_prov_efs(
  void
)
{
  mmgsdi_return_enum_type   mmgsdi_status              = MMGSDI_ERROR;

  if(MMGSDI_SUCCESS !=
     gsdi_efs_name_test((const char *)mmgsdi_efs_subs_prov_root_dir,
                        UIM_COMMON_EFS_DEVICE))
  {
    /* mmgsdi dir is not available, create the directory */
    if(MMGSDI_SUCCESS !=
       gsdi_efs_mkdir((const char *)mmgsdi_efs_subs_prov_root_dir))
    {
      /* Error creating mmgsdi directory */
      return MMGSDI_ERROR;
    }
  }
  if(MMGSDI_SUCCESS !=
       gsdi_efs_name_test((const char *)mmgsdi_efs_subs_prov_mgr, UIM_COMMON_EFS_DEVICE))
  {
    /* subs_prov_mgr directory is not available, create the directory */
    if(MMGSDI_SUCCESS !=
       gsdi_efs_mkdir((const char *)mmgsdi_efs_subs_prov_mgr))
    {
      /* Error creating subs_prov_mgr directory */
      return MMGSDI_ERROR;
    }
  }
  if(MMGSDI_SUCCESS !=
       gsdi_efs_name_test((const char *)mmgsdi_efs_subs_prov_info, UIM_COMMON_EFS_DEVICE))
  {
    /* subs_prov_info.txt is not available, create the file */
    if(MMGSDI_SUCCESS !=
         gsdi_efs_create_file((const char *)mmgsdi_efs_subs_prov_info, UIM_COMMON_EFS_DEVICE))
    {
      /* Error creating subs_prov_info.txt */
      return MMGSDI_ERROR;
    }
  }
  if(MMGSDI_SUCCESS == gsdi_efs_write_file(
                     (const char *)mmgsdi_efs_subs_prov_info ,
                     sizeof(mmgsdi_subs_prov_data_type) * MMGSDI_MAX_NUM_SLOTS,
                     (uint8 *)mmgsdi_subs_prov_info.subs_prov_data,
                     UIM_COMMON_EFS_DEVICE))
  {
    mmgsdi_subs_prov_info.is_efs_content_valid = TRUE;
    mmgsdi_status = MMGSDI_SUCCESS;
  }
  else
  {
    mmgsdi_status = MMGSDI_ERROR;
  }
  UIM_MSG_HIGH_1("Write file status for subs_prov_info.txt is 0x%x",
             mmgsdi_status);
  return mmgsdi_status;
} /* mmgsdi_write_to_subs_prov_efs */


/*===========================================================================
FUNCTION MMGSDI_CHECK_SUBS_PROV_INFO_UPDATE_NEEDED

DESCRIPTION
  This is used to check if we need to update the mmgsdi EFS subs_prov_info.txt
  with latest subscription information

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
static boolean mmgsdi_check_subs_prov_info_update_needed
(
  uint8                          slot_index,
  mmgsdi_data_type               iccid,
  mmgsdi_session_type_enum_type  session_type,
  mmgsdi_aid_type                aid_info
)
{
  mmgsdi_sub_app_specific_info_type   *sub_app_specific_info_ptr = NULL;
  uint8                                app_index                 = 0;
  mmgsdi_subs_prov_data_type          *subs_prov_data_ptr        = NULL;
  boolean                              iccid_match               = FALSE;

  switch(slot_index)
  {
    case MMGSDI_SLOT_1_INDEX:
    case MMGSDI_SLOT_2_INDEX:
    case MMGSDI_SLOT_3_INDEX:
      break;
    default:
      UIM_MSG_HIGH_1("Invalid slot index 0x%x", slot_index);
      return FALSE;
  }

  subs_prov_data_ptr = &mmgsdi_subs_prov_info.subs_prov_data[slot_index];

  if(aid_info.app_type == MMGSDI_APP_SIM ||
     aid_info.app_type == MMGSDI_APP_USIM)
  {
    app_index = MMGSDI_SUBS_AUTO_PROV_USIM_INDEX;
  }
  else if(aid_info.app_type == MMGSDI_APP_RUIM ||
          aid_info.app_type == MMGSDI_APP_CSIM)
  {
    app_index = MMGSDI_SUBS_AUTO_PROV_CSIM_INDEX;
  }
  else
  {
    return FALSE;
  }

  /* Check if ICCID does not match */
  if(subs_prov_data_ptr->iccid.length > 0)
  {
    if((subs_prov_data_ptr->iccid.length == iccid.data_len) &&
       (memcmp(subs_prov_data_ptr->iccid.data,
               iccid.data_ptr, iccid.data_len) == 0))
    {
      iccid_match = TRUE;
    }
  }

  if(!iccid_match)
  {
    memset(&mmgsdi_subs_prov_info.subs_prov_data[slot_index],
           0x00, sizeof(mmgsdi_subs_prov_data_type));
    UIM_MSG_HIGH_0("ICCID/Card Type doesn't match with stored EFS ");
    return TRUE;
  }

  sub_app_specific_info_ptr =
    &mmgsdi_subs_prov_info.subs_prov_data[slot_index].subs_app_data[app_index];

  if(sub_app_specific_info_ptr->app_data_valid == FALSE)
  {
    UIM_MSG_HIGH_0("New app activation");
    return TRUE;
  }
  /* Check if session type does not match */
  if(sub_app_specific_info_ptr->session_type != session_type)
  {
    UIM_MSG_HIGH_0("Session Type doesn't match with stored Session type");
    return TRUE;
  }

  /* Check if aid does not match with stored aid in EFS */
  if(aid_info.app_type == MMGSDI_APP_USIM ||
     aid_info.app_type == MMGSDI_APP_CSIM)
  {
    if(sub_app_specific_info_ptr->app_data.data_len > 0)
    {
      if((sub_app_specific_info_ptr->app_data.data_len != aid_info.aid.data_len) ||
         (memcmp(sub_app_specific_info_ptr->app_data.data_ptr,
                 aid_info.aid.data_ptr,
                 aid_info.aid.data_len)) != 0)
      {
        UIM_MSG_HIGH_0("Aid mismatches with stored aid in EFS " );
        return TRUE;
      }
    }
    else
    {
      /* No valid AID found in EFS, update the EFS with latest subscription info */
      return TRUE;
    }
  }

  /* Check if the EFS contains invalid AID for SIM/RUIM type.
     Update the EFS with new subscription information available */
  if(aid_info.app_type == MMGSDI_APP_SIM ||
     aid_info.app_type == MMGSDI_APP_RUIM )
  {
    if(sub_app_specific_info_ptr->app_data.data_len != 0)
    {
      return TRUE;
    }
  }
  return FALSE;
} /* mmgsdi_check_subs_prov_info_update_needed */


/*===========================================================================
FUNCTION MMGSDI_UPDATE_EFS_SUBS_PROV

DESCRIPTION
  This function will check if any new application is activated compared to
  the one stored in EFS subs_prov_info.txt. It will update the EFS in case of
  new application activation.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_update_efs_subs_prov(
  const mmgsdi_session_changed_evt_info_type *event_ptr,
  mmgsdi_session_id_type                      session_id
)
{
  mmgsdi_return_enum_type              mmgsdi_status              = MMGSDI_ERROR;
  mmgsdi_session_type_enum_type        session_type               = MMGSDI_MAX_SESSION_TYPE_ENUM;
  boolean                              is_update_needed           = TRUE;
  mmgsdi_access_type                   access;
  mmgsdi_data_type                     iccid;
  mmgsdi_slot_id_enum_type             slot_id                    = MMGSDI_MAX_SLOT_ID_ENUM;
  uint8                                slot_index                 = 0;
  mmgsdi_subs_prov_data_type          *subs_prov_data_ptr         = NULL;
  mmgsdi_slot_data_type               *slot_data_ptr              = NULL;
  mmgsdi_sub_app_specific_info_type   *sub_app_specific_info_ptr  = NULL;
  uint8                                app_index                  = 0;
  mmgsdi_app_enum_type                 app_type                   = MMGSDI_APP_NONE;
  uint8                                iccid_buf[MMGSDI_ICCID_LEN];

  if(mmgsdi_subs_prov_info.is_efs_content_valid == FALSE)
  {
    mmgsdi_status = gsdi_efs_read_file_with_len(
                            (const char *)mmgsdi_efs_subs_prov_info,
                            sizeof(mmgsdi_subs_prov_data_type) * MMGSDI_MAX_NUM_SLOTS,
                            (uint8 *)mmgsdi_subs_prov_info.subs_prov_data,
                            UIM_COMMON_EFS_DEVICE);
    if(mmgsdi_status == MMGSDI_SUCCESS)
    {
      mmgsdi_subs_prov_info.is_efs_content_valid = TRUE;
    }
  }

  slot_id = event_ptr->app_info.slot;
  mmgsdi_status = mmgsdi_util_get_slot_index(slot_id, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);
  if(slot_data_ptr == NULL)
  {
    return;
  }

  memset(&access, 0x00, sizeof(mmgsdi_access_type));
  memset(&iccid, 0x00, sizeof(mmgsdi_data_type));
  access.access_method  = MMGSDI_EF_ENUM_ACCESS;
  access.file.file_enum = MMGSDI_ICCID;

  iccid.data_ptr = iccid_buf;
  mmgsdi_status = mmgsdi_cache_read(session_id,
                                    &access,
                                    &iccid,
                                    0,
                                    NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Failed to read cache for ICCID: mmgsdi_status 0x%x",
                mmgsdi_status);

   /* If we fail to get iccid clear data in EFS for that slot */
    memset(&mmgsdi_subs_prov_info.subs_prov_data[slot_index],
           0x00, sizeof(mmgsdi_subs_prov_data_type));
    mmgsdi_write_to_subs_prov_efs();
    return;
  }

  if(MMGSDI_SUCCESS != mmgsdi_util_get_session_type(session_id,
                                                    &session_type, NULL))
  {
    memset(&mmgsdi_subs_prov_info.subs_prov_data[slot_index],
           0x00, sizeof(mmgsdi_subs_prov_data_type));
    mmgsdi_write_to_subs_prov_efs();
    return;
  }

  app_type = event_ptr->app_info.app_data.app_type;
  is_update_needed = mmgsdi_check_subs_prov_info_update_needed(
                       slot_index,
                       iccid,
                       session_type,
                       event_ptr->app_info.app_data);

  /* Update EFS with latest subscription information if needed */
  if(!is_update_needed)
  {
    return;
  }

  subs_prov_data_ptr = &mmgsdi_subs_prov_info.subs_prov_data[slot_index];

  /* Fill ICCID len and value */
  subs_prov_data_ptr->iccid.length = iccid.data_len;
  memscpy(subs_prov_data_ptr->iccid.data,
          sizeof(subs_prov_data_ptr->iccid.data),
          iccid.data_ptr,
          iccid.data_len);

  /* Fill slot id */
  subs_prov_data_ptr->slot_id = slot_id;

  /*Fill application specific information */
  if(app_type == MMGSDI_APP_SIM || app_type == MMGSDI_APP_USIM)
  {
    app_index = MMGSDI_SUBS_AUTO_PROV_USIM_INDEX;
  }
  else if(app_type == MMGSDI_APP_RUIM || app_type == MMGSDI_APP_CSIM)
  {
    app_index = MMGSDI_SUBS_AUTO_PROV_CSIM_INDEX;
  }

  sub_app_specific_info_ptr =
    &mmgsdi_subs_prov_info.subs_prov_data[slot_index].subs_app_data[app_index];

  sub_app_specific_info_ptr->app_data_valid = TRUE;
  sub_app_specific_info_ptr->session_type   = session_type;
  sub_app_specific_info_ptr->app_type       = app_type;
  sub_app_specific_info_ptr->app_data.data_len =
    event_ptr->app_info.app_data.aid.data_len;
  memscpy(sub_app_specific_info_ptr->app_data.data_ptr,
          sizeof(sub_app_specific_info_ptr->app_data.data_ptr),
          event_ptr->app_info.app_data.aid.data_ptr,
          event_ptr->app_info.app_data.aid.data_len);

  mmgsdi_write_to_subs_prov_efs();

} /* mmgsdi_update_efs_subs_prov */


/*===========================================================================
FUNCTION MMGSDI_CLEAR_SUBS_PROV_APP_INFO

DESCRIPTION
  This function clears the EFS content if application has been
  deactivated by client request

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
============================================================================ */
void mmgsdi_clear_subs_prov_app_info(
  const mmgsdi_session_changed_evt_info_type *event_ptr
)
{
  uint8                              app_index;
  mmgsdi_sub_app_specific_info_type *app_specific_ptr     = NULL;
  uint8                              slot_index;

  if(event_ptr == NULL)
  {
    UIM_MSG_HIGH_0("Event ptr is NULL");
    return;
  }

  if(MMGSDI_SUCCESS != mmgsdi_util_get_slot_index(
                         event_ptr->app_info.slot, &slot_index))
  {
    return;
  }

  if(mmgsdi_subs_prov_info.is_efs_content_valid == FALSE)
  {
    if(MMGSDI_SUCCESS != gsdi_efs_read_file_with_len(
                             (const char *)mmgsdi_efs_subs_prov_info,
                             sizeof(mmgsdi_subs_prov_data_type) * MMGSDI_MAX_NUM_SLOTS,
                             (uint8 *)mmgsdi_subs_prov_info.subs_prov_data,
                             UIM_COMMON_EFS_DEVICE))
    {
      UIM_MSG_HIGH_0("EFS read failed while trying to update Session deactivation information");
      return;
    }
    mmgsdi_subs_prov_info.is_efs_content_valid = TRUE;
  }

  for(app_index = 0; app_index < MMGSDI_MAX_SUPPORTED_AUTO_PROV_APPS ;
      app_index++)
  {
    app_specific_ptr =
      &mmgsdi_subs_prov_info.subs_prov_data[slot_index].subs_app_data[app_index];
    if(app_specific_ptr->app_data_valid)
    {
      if(app_specific_ptr->app_type ==
         event_ptr->app_info.app_data.app_type )
      {
        memset(app_specific_ptr,
               0x00,
               sizeof(mmgsdi_sub_app_specific_info_type));
        if(MMGSDI_SUCCESS == gsdi_efs_write_file(
                             (const char *)mmgsdi_efs_subs_prov_info ,
                             sizeof(mmgsdi_subs_prov_data_type) * MMGSDI_MAX_NUM_SLOTS,
                             (uint8 *)mmgsdi_subs_prov_info.subs_prov_data,
                             UIM_COMMON_EFS_DEVICE))
        {
          UIM_MSG_HIGH_1("Updated subs_prov_data.txt with session deactivation information for App_Type 0x%x",
                         app_specific_ptr->app_type);
        }
        break;
      }
    }
  }
} /* mmgsdi_clear_subs_prov_app_info */


/* ==========================================================================
FUNCTION MMGSDI_TRIGGER_PROV_APP_ACT_FROM_EFS

DESCRIPTION
  This function goes through the list of stored prov app data from EFS
  and trigger activation of prov applicaitons.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
----------------------------------------------------------------------------*/
static void mmgsdi_trigger_prov_app_act_from_efs(
  mmgsdi_slot_id_enum_type slot_id,
  uint8                    app_start_index
)
{
  mmgsdi_return_enum_type    mmgsdi_status       = MMGSDI_ERROR;
  uint8                      index               = 0;
  mmgsdi_subs_prov_data_type *subs_prov_data_ptr = NULL;
  uint8                      slot_index          = 0;
  mmgsdi_slot_data_type      *slot_data_ptr      = NULL;
  mmgsdi_data_type           aid;
  mmgsdi_session_id_type     session_id          = 0;
  mmgsdi_nv_context_type     sub_nv_context      = MMGSDI_NV_CONTEXT_INVALID;

  memset(&aid, 0x00, sizeof(mmgsdi_data_type));

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);
  if(slot_data_ptr == NULL)
  {
    return;
  }

  if(MMGSDI_SUCCESS != mmgsdi_util_get_slot_index(slot_id, &slot_index))
  {
    return;
  }

  switch(slot_index)
  {
    case MMGSDI_SLOT_1_INDEX:
    case MMGSDI_SLOT_2_INDEX:
    case MMGSDI_SLOT_3_INDEX:
      break;
    default:
      return;
  }

  subs_prov_data_ptr = &mmgsdi_subs_prov_info.subs_prov_data[slot_index];

  for(index = app_start_index; index < MMGSDI_MAX_SUPPORTED_AUTO_PROV_APPS;
      index++)
  {
    mmgsdi_sub_app_specific_info_type      *app_specific_info_ptr;

    app_specific_info_ptr = &subs_prov_data_ptr->subs_app_data[index];
    if(app_specific_info_ptr->app_data_valid)
    {
      if(mmgsdi_util_is_aid_valid_in_slot(slot_data_ptr,
           app_specific_info_ptr->app_type,
           app_specific_info_ptr->app_data) == FALSE)
      {
        continue;
      }

      if(mmgsdi_util_get_mmgsdi_session_id_from_type(
           app_specific_info_ptr->session_type,
           &session_id) != MMGSDI_SUCCESS)
      {
        continue;
      }

      /*Activate 1x session only when CDMALess NV is disbaled */
      if((mmgsdi_util_is_1x_prov_session(app_specific_info_ptr->session_type)) &&
         (mmgsdi_util_get_efs_item_index(session_id, &sub_nv_context) == MMGSDI_SUCCESS) &&
         (mmgsdi_nv_get_external_feature_status(
            MMGSDI_EXT_FEAT_CDMA_LESS_DEVICE, sub_nv_context) == MMGSDI_FEATURE_ENABLED))
      {
        UIM_MSG_HIGH_2("CDMALess NV is enabled for 1x subscription context 0x%x slot 0x%x; not activating",
                        sub_nv_context, slot_id);
        continue;
      }

        aid.data_len = app_specific_info_ptr->app_data.data_len;
        aid.data_ptr = app_specific_info_ptr->app_data.data_ptr;
        mmgsdi_status = mmgsdi_session_activate_or_switch_provisioning(
                          session_id,
                          slot_id,
                          aid,
                          mmgsdi_provision_app_from_efs_cb,
                          (mmgsdi_client_data_type)index);
        UIM_MSG_HIGH_3("Auto provisioning from MMGSDI EFS for App_type = 0x%x, Session_type = 0x%x mmgsdi_status = 0x%x",
                       app_specific_info_ptr->app_type,
                       app_specific_info_ptr->session_type,
                       mmgsdi_status);
        if(mmgsdi_status == MMGSDI_SUCCESS)
        {
        /* Do not post the next activate request immediately - process next
           entry in the call back */
          return;
        }
      }
    }
}/*mmgsdi_trigger_prov_app_act_from_efs*/


/* ==========================================================================
FUNCTION MMGSDI_PROVISION_APP_FROM_EFS_CB

DESCRIPTION
  This function handles the cnf for EFS based provisioning. Based on the
  clientdata, the provisining of remaining applications are triggered.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
----------------------------------------------------------------------------*/
static void mmgsdi_provision_app_from_efs_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr)
{
  uint8                      app_index           = 0;

  (void)status;

  if(cnf != MMGSDI_SESSION_ACTIVATE_OR_SWITCH_PROVISIONING_CNF ||
     cnf_ptr == NULL)
  {
    return;
  }

  /* EFS content is already read - so it is expected to be valid, if not exit */
  if(mmgsdi_subs_prov_info.is_efs_content_valid == FALSE)
  {
    return;
  }

  /* activate the remaining apps from EFS based on index stored in client data*/
  app_index = (uint8)cnf_ptr->response_header.client_data;

  UIM_MSG_HIGH_1("mmgsdi_provision_app_from_efs_cb: client_data:0x%x",app_index);

  /* if we have processed all the apps dont proceed */
  if(app_index+1 >= MMGSDI_MAX_SUPPORTED_AUTO_PROV_APPS)
  {
    return;
  }

  mmgsdi_trigger_prov_app_act_from_efs(
    cnf_ptr->response_header.slot_id,
    app_index+1);
}/*mmgsdi_provision_app_from_efs_cb*/


/*===========================================================================
FUNCTION:  MMGSDI_FIND_DUPLICATE_AID_IN_SLOT

DESCRIPTION
  This function will check if there are any duplicate AID entries on one slot.

DEPENDENCIES
  None

RETURN VALUE
  boolean - invalid data found or not

SIDE EFFECTS
  None
===========================================================================*/
static boolean mmgsdi_find_duplicate_aid_in_slot(
  mmgsdi_subs_prov_data_type *slot_prov_data_ptr)
{
  uint8 i = 0;
  uint8 j = 0;

  for(i = 0; i < MMGSDI_MAX_SUPPORTED_AUTO_PROV_APPS - 1; i++)
  {
    if (!slot_prov_data_ptr->subs_app_data[i].app_data_valid)
    {
      continue;
    }

    for (j = i + 1; j < MMGSDI_MAX_SUPPORTED_AUTO_PROV_APPS; j++)
    {
      if((slot_prov_data_ptr->subs_app_data[j].app_data_valid) &&
         (slot_prov_data_ptr->subs_app_data[i].app_data.data_len ==
            slot_prov_data_ptr->subs_app_data[j].app_data.data_len ) &&
         (slot_prov_data_ptr->subs_app_data[i].app_data.data_len > 0) &&
         (memcmp(slot_prov_data_ptr->subs_app_data[i].app_data.data_ptr,
                 slot_prov_data_ptr->subs_app_data[j].app_data.data_ptr,
                 slot_prov_data_ptr->subs_app_data[i].app_data.data_len ) == 0 ))
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}/*mmgsdi_find_duplicate_aid_in_slot*/


/*===========================================================================
FUNCTION:  MMGSDI_CHECK_EFS_DATA_IS_INVALID

DESCRIPTION
  This function will check if EFS is valid before triggering provisining. 
  The following checks are in place
  - Same session type is reused across slots
  - Same AID is not expected to be present on a particular slot
  - Different session types (pri or sec) of GW or 1X cannot be configured on same slot
  - Same session types of GW or 1X cannot be configured across different slots

DEPENDENCIES
  None

RETURN VALUE
  boolean - invalid data found or not

SIDE EFFECTS
  None
===========================================================================*/
static boolean mmgsdi_check_efs_data_is_invalid(
  void
)
{
  uint8                       slot_count         = 0;
  uint8                       app_count          = 0;
  uint8                       session_cat_count  = 0;
  boolean                     invalid_mapping    = FALSE;
  mmgsdi_subs_prov_data_type *slot_prov_data_ptr = NULL;
  uint8                       session_index      = 0;
  mmgsdi_attribute_enum_type  session_category   = MMGSDI_ATTR_UNKNOWN;
  boolean                     session_used[MMGSDI_MAX_PROV_APPS];
  boolean                     session_category_inuse[MMGSDI_NUM_ATTRIBUTES];
  uint8                       session_category_inuse_slot[MMGSDI_NUM_ATTRIBUTES];
  mmgsdi_technology_enum_type tech;

  memset(session_category_inuse_slot, 0xFF, MMGSDI_NUM_ATTRIBUTES);
  memset(session_used, FALSE, MMGSDI_MAX_PROV_APPS);
  memset(session_category_inuse, FALSE, MMGSDI_NUM_ATTRIBUTES);

  for (slot_count = 0; (slot_count < MMGSDI_MAX_NUM_SLOTS) && (invalid_mapping == FALSE); slot_count++)
  {
    slot_prov_data_ptr = &mmgsdi_subs_prov_info.subs_prov_data[slot_count];

    /* check if same aid is present more than once in a particular slot */
    invalid_mapping = mmgsdi_find_duplicate_aid_in_slot(slot_prov_data_ptr);
    if(invalid_mapping)
    {
      break;
    }

    for (app_count = 0; (app_count < MMGSDI_MAX_SUPPORTED_AUTO_PROV_APPS) && (invalid_mapping == FALSE); app_count++)
    {
      UIM_MSG_MED_4("Subs prov info Validity, app: 0x%x, slot: 0x%x, app_data_valid: 0x%x, session_type: 0x%x",
                      app_count, slot_count,
                      slot_prov_data_ptr->subs_app_data[app_count].app_data_valid, 
                      slot_prov_data_ptr->subs_app_data[app_count].session_type);

      if (slot_prov_data_ptr->subs_app_data[app_count].app_data_valid)
      {
        session_index = mmgsdi_util_get_prov_index_from_session(
                          slot_prov_data_ptr->subs_app_data[app_count].session_type);

        /* Make sure same session type not used across any slots */
        if(session_index >= MMGSDI_MAX_PROV_APPS ||
           session_used[session_index])
        {
          invalid_mapping = TRUE;
          break;
        }

        session_used[session_index] = TRUE;

        /* Make sure mix and match of PRI and SEC session types together are not used on same slot*/
        (void)mmgsdi_util_get_tech_and_attr_from_session_type(
          slot_prov_data_ptr->subs_app_data[app_count].session_type,
          &tech, &session_category);
        if(session_category == MMGSDI_ATTR_UNKNOWN)
        {
          invalid_mapping = TRUE;
          continue;
        }

        for(session_cat_count = 0; 
            (session_cat_count < MMGSDI_NUM_ATTRIBUTES) && (invalid_mapping == FALSE); 
            session_cat_count++)
        {
          /* if same session category is mapped to different slot, mark invalid */
          if(session_cat_count == session_category)
          {
            if(session_category_inuse[session_category])
            {
              if(session_category_inuse_slot[session_category] != slot_count)
              {
                invalid_mapping = TRUE;
                continue;
              }
            }
            else
            {
              session_category_inuse[session_category] = TRUE;
              session_category_inuse_slot[session_category] = slot_count;
            }
          }
          /* if other session category is mapped to this slot, mark invalid */
          else
          {
            if(session_category_inuse[session_cat_count])
            {
              if(session_category_inuse_slot[session_cat_count] == slot_count)
              {
                invalid_mapping = TRUE;
                continue;
              }
            }
          }
        }
      }
    }
  }

  /* If mapping is invalid, delete the EFS content and wait for AP to trigger activation */
  if(invalid_mapping)
  {
    memset(&mmgsdi_subs_prov_info,
           0x00,
           sizeof(mmgsdi_subs_prov_info_type));

    UIM_MSG_HIGH_0("Subs prov info cleaned up in subs_prov_data.txt due to session mismatch");

    (void)gsdi_efs_write_file(
            (const char *)mmgsdi_efs_subs_prov_info ,
            sizeof(mmgsdi_subs_prov_data_type) * MMGSDI_MAX_NUM_SLOTS,
            (uint8 *)mmgsdi_subs_prov_info.subs_prov_data,
            UIM_COMMON_EFS_DEVICE);
  }
  return invalid_mapping;
}/* mmgsdi_check_efs_data_is_invalid */


/*===========================================================================
FUNCTION:  MMGSDI_PROVISION_APPLICATIONS_FROM_EFS

DESCRIPTION
  This function will try to provision applications stored in EFS if
  ICCID stored is matching with current ICCID.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_provision_applications_from_efs(
  mmgsdi_slot_id_enum_type slot_id
)
{
  mmgsdi_return_enum_type                 mmgsdi_status          = MMGSDI_SUCCESS;
  mmgsdi_access_type                      access;
  mmgsdi_data_type                        iccid;
  mmgsdi_session_id_type                  card_session_id        = 0;
  mmgsdi_subs_prov_data_type             *subs_prov_data_ptr     = NULL;
  uint8                                   slot_index             = 0;
  uint8                                   iccid_buf[MMGSDI_ICCID_LEN];

  if(MMGSDI_SUCCESS != mmgsdi_util_get_slot_index(slot_id, &slot_index))
  {
    return;
  }
  switch(slot_index)
  {
    case MMGSDI_SLOT_1_INDEX:
    case MMGSDI_SLOT_2_INDEX:
    case MMGSDI_SLOT_3_INDEX:
      break;
    default:
      return;
  }

  /* Try to read the EFS and provision applications from EFS if ICCID matches */
  if(mmgsdi_subs_prov_info.is_efs_content_valid == FALSE)
  {
    mmgsdi_status = gsdi_efs_read_file_with_len(
                             (const char *)mmgsdi_efs_subs_prov_info,
                             sizeof(mmgsdi_subs_prov_data_type) * MMGSDI_MAX_NUM_SLOTS,
                             (uint8 *)mmgsdi_subs_prov_info.subs_prov_data,
                             UIM_COMMON_EFS_DEVICE);
    UIM_MSG_HIGH_2("Reading file status 0x%x for EFS subs_prov_info.txt for slot 0x%x",
                   mmgsdi_status, slot_id);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      return;
    }
    mmgsdi_subs_prov_info.is_efs_content_valid = TRUE;
  }

  subs_prov_data_ptr = &mmgsdi_subs_prov_info.subs_prov_data[slot_index];
  memset(&access, 0x00, sizeof(mmgsdi_access_type));
  memset(&iccid, 0x00, sizeof(mmgsdi_data_type));
  access.access_method  = MMGSDI_EF_ENUM_ACCESS;
  access.file.file_enum = MMGSDI_ICCID;

  iccid.data_ptr = iccid_buf;
  card_session_id = mmgsdi_generic_data_ptr->slot_session_id[slot_index];
  mmgsdi_status = mmgsdi_cache_read(card_session_id,
                                    &access,
                                    &iccid,
                                    0,
                                    NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {

    UIM_MSG_ERR_1("Failed to read ICCID for slot 0x%x", slot_id);
    return;
  }

  if(subs_prov_data_ptr->iccid.length == 0 )
  {
    return;
  }

  if((subs_prov_data_ptr->iccid.length != iccid.data_len) ||
     (memcmp(subs_prov_data_ptr->iccid.data,
             iccid.data_ptr,
             iccid.data_len) != 0 ))
  {
    return;
  }

  /* perform sanity check on eFS - if invalid do not proceed to prov*/
  if(mmgsdi_check_efs_data_is_invalid())
  {
    return;
  }

  /* start activaiton of app at the 0th index */
  mmgsdi_trigger_prov_app_act_from_efs(slot_id,0);
} /* mmgsdi_provision_applications_from_efs */


/*===========================================================================
FUNCTION:  MMGSDI_DELETE_EFS_SUBS_PROV

DESCRIPTION
  This function will delete efs subs prov data when nv is disabled
  and efs file still present.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_delete_efs_subs_prov(
  void
)
{
  /* delete the EFS file - file will be deleted if present */
  (void)gsdi_efs_delete_file(
          (const char *)mmgsdi_efs_subs_prov_info,
          UIM_COMMON_EFS_DEVICE);
}/* mmgsdi_delete_efs_subs_prov */


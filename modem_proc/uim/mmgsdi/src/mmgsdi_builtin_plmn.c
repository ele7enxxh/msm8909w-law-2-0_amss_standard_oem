/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            M M G S D I B U I L T I N  P L M N  D E F I N I T I O N S

                      A N D   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the main GSDI task along with supporting functions.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2014 - 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_builtin_plmn.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/07/15   av      Migrate to mcfg EFS APIs
09/09/14   kk      Enhancements in NV handling
07/28/14   vv      Remove unused data types
07/15/14   yt      Support for NV refresh
02/25/14   bcho    initial version


===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "mmgsdi_builtin_plmn.h"
#include "mmgsdi_cnf.h"
#include "mmgsdi_efs.h"
#include "mmgsdi_nv.h"
#include "mmgsdi_refresh.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* OPLMNWACT record size */
#define MMGSDI_OPLMNWACT_RECORD_SIZE               5


/*===========================================================================
FUNCTION MMGSDI_PROCESS_SET_BUILTIN_PLMN_LIST

DESCRIPTION
  This function queues a response for handling set builtin plmn list in
  the cnf

PARAMETERS:
  mmgsdi_set_builtin_plmn_list_req_type req_ptr: request pointer

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_process_set_builtin_plmn_list(
  mmgsdi_set_builtin_plmn_list_req_type * req_ptr
)
{
  mmgsdi_sw_status_type status_word = {0};

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  return mmgsdi_cnf_build_and_queue(MMGSDI_SUCCESS,
                                             &req_ptr->request_header,
                                             NULL,
                                             (const void *)req_ptr,
                                             FALSE,
                                             status_word);
}/* mmgsdi_process_set_builtin_plmn_list */


/*================================================================
FUNCTION  MMGSDI_GET_BUILTIN_PLMN_LIST

DESCRIPTION:
  This function is called to read builtin PLMN list stored in EFS.

  INPUT PARAMETERS:
    int32 file_size_ptr    : file size pointer
    uint8 file_buffer_pptr : address of file buffer pointer

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS:
  None
===============================================================*/
mmgsdi_return_enum_type mmgsdi_get_builtin_plmn_list(
  int32 * file_size_ptr,
  uint8 ** file_buffer_pptr)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  char * file_handle_ptr                = MMGSDI_BUILTIN_PLMN_LIST_FILE;

  MMGSDIUTIL_RETURN_IF_NULL_2(file_size_ptr, file_buffer_pptr);
  *file_buffer_pptr = NULL;

  if(MMGSDI_SUCCESS != gsdi_efs_name_test((const char *)file_handle_ptr,
                                          UIM_COMMON_EFS_DEVICE))
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = gsdi_fs_get_file_size(file_handle_ptr,(int32*)file_size_ptr,
                                        UIM_COMMON_EFS_DEVICE);

  if(mmgsdi_status != MMGSDI_SUCCESS ||
     *file_size_ptr == 0)
  {
    MMGSDI_DEBUG_MSG_ERROR_2("Builtin PLMN List file Status: 0x%x File Size: 0x%x",
                             mmgsdi_status, *file_size_ptr);
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*file_buffer_pptr, *file_size_ptr);

  if(*file_buffer_pptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status =  gsdi_efs_read_file((const char *)file_handle_ptr,
                                      (int32*)file_size_ptr,
                                      *file_buffer_pptr,
                                      UIM_COMMON_EFS_DEVICE);

  if ( mmgsdi_status != MMGSDI_SUCCESS ||
       *file_size_ptr   == 0 )
  {
    MMGSDIUTIL_TMC_MEM_FREE(*file_buffer_pptr);
    return MMGSDI_ERROR;
  }

  if((*file_size_ptr) % MMGSDI_OPLMNWACT_RECORD_SIZE != 0)
  {
    MMGSDIUTIL_TMC_MEM_FREE(*file_buffer_pptr);
    mmgsdi_status = MMGSDI_ERROR;
  }
  return mmgsdi_status;
}/* mmgsdi_get_builtin_plmn_list */


/*================================================================
FUNCTION  MMGSDI_BUILTIN_PLMN_REFRESH_CB

DESCRIPTION:
  This function is used as callback function for refresh triggered after
  storing builtin PLMN List.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===============================================================*/
static void mmgsdi_builtin_plmn_refresh_cb(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
)
{
  UIM_MSG_MED_1("mmgsdi_builtin_plmn_refresh_cb, status: 0x%x", status);
  (void)cnf;
  (void)cnf_ptr;
}/* mmgsdi_builtin_plmn_refresh_cb */


/*================================================================
FUNCTION  MMGSDI_BUILTIN_PLMN_TRIGGER_REFRESH

DESCRIPTION:
  This function is called when MMGSDI receives OPLMN List from network.
  It triggers Refresh FCN on EF-OPLMNwACT .

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS:
  None
===============================================================*/
static mmgsdi_return_enum_type mmgsdi_builtin_plmn_trigger_refresh(
  mmgsdi_slot_id_enum_type slot_id
)
{
  mmgsdi_refresh_req_type  req;
  uint8                    index                   = 0;
  mmgsdi_slot_data_type   *slot_data_ptr           = NULL;
  uint8                    usim_oplmnwact_data[]   = {0x01,0x3F,0x00,0x7F,0xFF,0x6F,0x61};
  uint8                    sim_oplmnwact_data[]    = {0x01,0x3F,0x00,0x7F,0x20,0x6F,0x61};
  mmgsdi_return_enum_type  mmgsdi_status           = MMGSDI_ERROR;

  memset(&req, 0x0, sizeof(mmgsdi_refresh_req_type));

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);

  if(slot_data_ptr == NULL)
  {
    return MMGSDI_SUCCESS;
  }

  /* Scan through the apps list to find if any USIMs/SIMs
     in ILLEGAL or READY state. We check only first availabe USIM/SIM app */
  for(index = 0; index < MMGSDI_MAX_EXT_APP_INFO; index ++)
  {
    if(slot_data_ptr->app_info_pptr[index] == NULL)
    {
      continue;
    }

    if((slot_data_ptr->app_info_pptr[index]->app_data.app_type != MMGSDI_APP_USIM) &&
       (slot_data_ptr->app_info_pptr[index]->app_data.app_type != MMGSDI_APP_SIM))
    {
      continue;
    }

    if(slot_data_ptr->app_info_pptr[index]->app_state == MMGSDI_APP_STATE_READY ||
       slot_data_ptr->app_info_pptr[index]->app_state == MMGSDI_APP_STATE_ILLEGAL)
    {
      break;
    }
  }

  if(index != MMGSDI_MAX_EXT_APP_INFO)
  {
    mmgsdi_memscpy(&req.refresh.card_req.aid,
                   sizeof(req.refresh.card_req.aid),
                   &slot_data_ptr->app_info_pptr[index]->app_data,
                   sizeof(mmgsdi_aid_type));

    if(slot_data_ptr->app_info_pptr[index]->app_data.app_type ==
         MMGSDI_APP_USIM)
    {
      req.refresh.card_req.refresh_files.data_ptr = usim_oplmnwact_data;
      req.refresh.card_req.refresh_files.data_len = sizeof(usim_oplmnwact_data);
    }
    else
    {
      req.refresh.card_req.refresh_files.data_ptr = sim_oplmnwact_data;
      req.refresh.card_req.refresh_files.data_len = sizeof(sim_oplmnwact_data);
    }
  }
  else
  {
    /* No USIM/SIM app present in READY state on slot */
    UIM_MSG_HIGH_1("No USIM/SIM app present in READY state on slot 0x%x",
                   slot_id);
    return MMGSDI_SUCCESS;
  }

  req.refresh_req_type              = MMGSDI_REFRESH_ORIG_REFRESH_REQ;
  req.refresh.card_req.refresh_mode = MMGSDI_REFRESH_NAA_FCN;
  req.refresh.card_req.num_files    = 1;
  req.refresh.card_req.source       = MMGSDI_REFRESH_SOURCE_BUILTIN_PLMN;

  if(MMGSDI_SUCCESS != mmgsdi_copy_info_to_refresh_data_info(&req,
                                                             slot_id))
  {
    UIM_MSG_HIGH_0("Could not initialize refresh_data_info_ptr");
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_client_build_refresh_req(
                    mmgsdi_generic_data_ptr->client_id,
                    slot_id,
                    &req,
                    FALSE,
                    mmgsdi_builtin_plmn_refresh_cb,
                    0,
                    FALSE);
  UIM_MSG_HIGH_1("Build Refresh req status: 0x%x", mmgsdi_status);
  return mmgsdi_status;
}/* mmgsdi_builtin_plmn_trigger_refresh */


/*================================================================
FUNCTION  MMGSDI_STORE_BUILTIN_PLMN_LIST

DESCRIPTION:
  This function is called to store builtin plmn list in EFS provided by QMI NAS
  interface.

  INPUT PARAMETERS:
    mmgsdi_set_builtin_plmn_list_req_type set_builtin_plmn_list_req_ptr: request pointer

DEPENDENCIES:
  It triggers Refresh FCN on OPLMNwACT after storing builtin PLMN list only if
  USIM/SIM app has reached SUB READY.

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS:
  None
===============================================================*/
mmgsdi_return_enum_type mmgsdi_store_builtin_plmn_list(
  const mmgsdi_set_builtin_plmn_list_req_type      *set_builtin_plmn_list_req_ptr
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  char  * file_handle_ptr               = MMGSDI_BUILTIN_PLMN_LIST_FILE;
  char  * file_path_handle_ptr          = MMGSDI_NV_EFS_PATH;
  uint8 * builtin_plmn_data_ptr         = NULL;
  uint16  num_of_plmn_ids               = 0;
  boolean trigger_refresh               = FALSE;
  uint16  loop_cnt                      = 0;
  uint16  buffer_size                   = 0;

  MMGSDIUTIL_RETURN_IF_NULL(set_builtin_plmn_list_req_ptr);

  if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_OPLMN_LIST_CONFIGURABLE_OTA, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) ==
       MMGSDI_FEATURE_ENABLED)
  {
    boolean ct_card_on_slot1 = mmgsdi_util_is_imsi_plmn_cust_plmn(MMGSDI_SLOT_1);
    boolean ct_card_on_slot2 = mmgsdi_util_is_imsi_plmn_cust_plmn(MMGSDI_SLOT_2);
    boolean ct_card_on_slot3 = mmgsdi_util_is_imsi_plmn_cust_plmn(MMGSDI_SLOT_3);

    /* Return if no card in slots is a specific customer card */
    if(FALSE == ct_card_on_slot1 &&
       FALSE == ct_card_on_slot2 &&
       FALSE == ct_card_on_slot3 )
    {
      return MMGSDI_ERROR;
    }

    num_of_plmn_ids = set_builtin_plmn_list_req_ptr->builtin_plmn_list.num_of_plmn_ids;
    if(num_of_plmn_ids > 0)
    {
      if(MMGSDI_SUCCESS != gsdi_efs_name_test((const char *)file_path_handle_ptr,
                                              UIM_COMMON_EFS_DEVICE))
      {
        /* Return ERROR as Builtin PLMN list file path is also not
           present in EFS */
        return MMGSDI_ERROR;
      }

      /* Create a buffer to copy OPLMN list from builtin plmn list req and store
         it in EFS */
      buffer_size = num_of_plmn_ids * MMGSDI_OPLMNWACT_RECORD_SIZE;

      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(builtin_plmn_data_ptr, buffer_size);

      if(builtin_plmn_data_ptr == NULL)
      {
        return MMGSDI_ERROR;
      }

      for(loop_cnt = 0; loop_cnt < num_of_plmn_ids; loop_cnt++)
      {
        mmgsdi_memscpy(
          &builtin_plmn_data_ptr[loop_cnt * MMGSDI_OPLMNWACT_RECORD_SIZE],
          buffer_size - (loop_cnt * MMGSDI_OPLMNWACT_RECORD_SIZE),
          set_builtin_plmn_list_req_ptr->builtin_plmn_list.plmn_list_ptr[loop_cnt].plmn_id_val,
          MMGSDI_PLMN_ID_SIZE);

        mmgsdi_memscpy(&builtin_plmn_data_ptr[(loop_cnt * MMGSDI_OPLMNWACT_RECORD_SIZE) + MMGSDI_PLMN_ID_SIZE],
          buffer_size - MMGSDI_PLMN_ID_SIZE - (loop_cnt * MMGSDI_OPLMNWACT_RECORD_SIZE),
          &set_builtin_plmn_list_req_ptr->builtin_plmn_list.plmn_list_ptr[loop_cnt].access_tech,
          sizeof(uint16));
      }

      if(MMGSDI_SUCCESS == gsdi_efs_write_file(
                             (const char *)file_handle_ptr,
                             buffer_size,
                             (uint8 *)builtin_plmn_data_ptr,
                             UIM_COMMON_EFS_DEVICE))
      {
        mmgsdi_status = MMGSDI_SUCCESS;
        trigger_refresh = TRUE;
      }

      UIM_MSG_MED_1("Write status for builtin PLMN List file: 0x%x", mmgsdi_status);

      MMGSDIUTIL_TMC_MEM_FREE(builtin_plmn_data_ptr);
    }
    else
    {
      /* Delete Stored builtin PLMN List file if file is present in EFS */
      if(MMGSDI_SUCCESS == gsdi_efs_name_test((const char *)file_handle_ptr,
                                              UIM_COMMON_EFS_DEVICE))
      {
        if(MMGSDI_SUCCESS == gsdi_efs_delete_file((const char *)file_handle_ptr,
                                                  UIM_COMMON_EFS_DEVICE))
        {
          mmgsdi_status = MMGSDI_SUCCESS;
          trigger_refresh = TRUE;
        }
      }
      else
      {
        /* Return SUCCESS as Builtin PLMN list is empty and file is also not
            present in EFS */
        mmgsdi_status = MMGSDI_SUCCESS;
      }
    }

    if(trigger_refresh)
    {
      if(ct_card_on_slot1)
      {
        (void)mmgsdi_builtin_plmn_trigger_refresh(MMGSDI_SLOT_1);
      }

      if(ct_card_on_slot2)
      {
        (void)mmgsdi_builtin_plmn_trigger_refresh(MMGSDI_SLOT_2);
      }

      if(ct_card_on_slot3)
      {
        (void)mmgsdi_builtin_plmn_trigger_refresh(MMGSDI_SLOT_3);
      }
    }
  }
  return mmgsdi_status;
}/* mmgsdi_store_builtin_plmn_list */


/*================================================================
FUNCTION  MMGSDI_BUILTIN_PLMN_LIST_IS_CONFIGURED

DESCRIPTION:
  This function is called to check biltin PLMN list is configured in EFS.

INPUT PARAMETERS:
  None

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS:
  None
===============================================================*/
mmgsdi_return_enum_type mmgsdi_builtin_plmn_list_is_configured(
  void
)
{
  return gsdi_efs_name_test((const char *)MMGSDI_BUILTIN_PLMN_LIST_FILE,
                            UIM_COMMON_EFS_DEVICE);
}/* mmgsdi_builtin_plmn_list_is_configured */

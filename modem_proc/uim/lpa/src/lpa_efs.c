/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


     L P A   F I L E S Y S T E M   R E L A T E D   O P E R A T I O N S


GENERAL DESCRIPTION

  This source files contains the LPA wrapper functions for carrying out
  EFS operations.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS

                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/lpa/src/lpa_efs.c#1 $ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/23/16    ar     Featurizing the code for storing LPA data to EFS
04/11/16    av     Initial Revision

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "intconv.h"
#include "customer.h"
#include "string.h"
#include "err.h"
#include "lpa_efs.h"
#include "fs_errno.h"
#include "lpa_platform.h"
#include "mcfg_fs.h"

#if defined(FEATURE_UIM_TEST_FRAMEWORK)
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*===========================================================================

                                    FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION LPA_EFS_INIT

DESCRIPTION
  Function creates the LPA EFS directory.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lpa_efs_init (
  void
)
{
  /* If the feature is defined, then only we write the data in EFS. This EFS
     logic is added for testing purpose only so not defined by default. In case
     we need it for debug with eUICC cards, we can define this feature. */
#ifdef FEATURE_LPA_STORE_EFS
  if(mcfg_mkdir(LPA_DIRECTORY, 0777, MCFG_FS_TYPE_EFS) < 0)
  {
    LPA_MSG_ERR_0("efs mkdir procedure failed");
  }
#endif /* FEATURE_LPA_STORE_EFS */
  return;
} /* lpa_efs_init */


/*===========================================================================
FUNCTION LPA_EFS_CLEAN

DESCRIPTION
  Deletes all the LPA EFS files

DEPENDENCIES
  None

RETURN VALUE
  lpa_result_enum_type:
           LPA_SUCCESS:          File was written
           LPA_GENERIC_ERROR:    Failure occurred

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_efs_cleanup(
  lpa_slot_id_enum_type        slot_id
)
{
#ifdef FEATURE_LPA_STORE_EFS
  uim_common_efs_context_type context = (uim_common_efs_context_type) slot_id;

  if(slot_id >= LPA_NUM_SLOTS )
  {
    LPA_MSG_ERR_1("Invalid slot_id=0x%x", slot_id);
    return LPA_GENERIC_ERROR;
  }

  /* Delete the files one by one */
  if(mcfg_fs_delete(LPA_FILE_JSON_ENCODED_INIT_AUTH, MCFG_FS_TYPE_EFS,
                    uim_common_convert_to_mcfg_context(context))
         != MCFG_FS_STATUS_OK)
  {
    LPA_MSG_ERR_0("Failure to remove file:");
    LPA_MSG_SPRINTF_1("%s", LPA_FILE_JSON_ENCODED_INIT_AUTH);
  }

  if(mcfg_fs_delete(LPA_FILE_JSON_ENCODED_GET_BPP, MCFG_FS_TYPE_EFS,
                    uim_common_convert_to_mcfg_context(context))
         != MCFG_FS_STATUS_OK)
  {
    LPA_MSG_ERR_0("Failure to remove file:");
    LPA_MSG_SPRINTF_1("%s", LPA_FILE_JSON_ENCODED_GET_BPP);
  }

  if(mcfg_fs_delete(LPA_FILE_JSON_ENCODED_HANDLE_INST_RSLT, MCFG_FS_TYPE_EFS,
                    uim_common_convert_to_mcfg_context(context))
         != MCFG_FS_STATUS_OK)
  {
    LPA_MSG_ERR_0("Failure to remove file:");
    LPA_MSG_SPRINTF_1("%s", LPA_FILE_JSON_ENCODED_HANDLE_INST_RSLT);
  }
  if(mcfg_fs_delete(LPA_FILE_JSON_INIT_AUTH_FROM_DS, MCFG_FS_TYPE_EFS,
                    uim_common_convert_to_mcfg_context(context))
         != MCFG_FS_STATUS_OK)
  {
    LPA_MSG_ERR_0("Failure to remove file:");
    LPA_MSG_SPRINTF_1("%s", LPA_FILE_JSON_INIT_AUTH_FROM_DS);
  }

  if(mcfg_fs_delete(LPA_FILE_JSON_GET_BPP_FROM_DS, MCFG_FS_TYPE_EFS,
                    uim_common_convert_to_mcfg_context(context))
         != MCFG_FS_STATUS_OK)
  {
    LPA_MSG_ERR_0("Failure to remove file:");
    LPA_MSG_SPRINTF_1("%s", LPA_FILE_JSON_GET_BPP_FROM_DS);
  }

  if(mcfg_fs_delete(LPA_FILE_ASN1_ENCODED_PREP_DOWNLOAD, MCFG_FS_TYPE_EFS,
                    uim_common_convert_to_mcfg_context(context))
         != MCFG_FS_STATUS_OK)
  {
    LPA_MSG_ERR_0("Failure to remove file:");
    LPA_MSG_SPRINTF_1("%s", LPA_FILE_ASN1_ENCODED_PREP_DOWNLOAD);
  }

  if(mcfg_fs_delete(LPA_FILE_PREP_DOWNLOAD_RESP_FROM_CARD, MCFG_FS_TYPE_EFS,
                    uim_common_convert_to_mcfg_context(context))
         != MCFG_FS_STATUS_OK)
  {
    LPA_MSG_ERR_0("Failure to remove file:");
    LPA_MSG_SPRINTF_1("%s", LPA_FILE_PREP_DOWNLOAD_RESP_FROM_CARD);
  }

  if(mcfg_fs_delete(LPA_FILE_EUICC_INFO_FROM_CARD, MCFG_FS_TYPE_EFS,
                    uim_common_convert_to_mcfg_context(context))
         != MCFG_FS_STATUS_OK)
  {
    LPA_MSG_ERR_0("Failure to remove file:");
    LPA_MSG_SPRINTF_1("%s", LPA_FILE_EUICC_INFO_FROM_CARD);
  }

  if(mcfg_fs_delete(LPA_FILE_LOAD_BPP_FROM_CARD, MCFG_FS_TYPE_EFS,
                    uim_common_convert_to_mcfg_context(context))
         != MCFG_FS_STATUS_OK)
  {
    LPA_MSG_ERR_0("Failure to remove file:");
    LPA_MSG_SPRINTF_1("%s", LPA_FILE_LOAD_BPP_FROM_CARD);
  }

  if(mcfg_fs_delete(LPA_FILE_JSON_DECODED_GET_BPP, MCFG_FS_TYPE_EFS,
                    uim_common_convert_to_mcfg_context(context))
         != MCFG_FS_STATUS_OK)
  {
    LPA_MSG_ERR_0("Failure to remove file:");
    LPA_MSG_SPRINTF_1("%s", LPA_FILE_JSON_DECODED_GET_BPP);
  }

  if(mcfg_fs_delete(LPA_FILE_EUICC_CHAL_FROM_CARD, MCFG_FS_TYPE_EFS,
                    uim_common_convert_to_mcfg_context(context))
         != MCFG_FS_STATUS_OK)
  {
    LPA_MSG_ERR_0("Failure to remove file:");
    LPA_MSG_SPRINTF_1("%s", LPA_FILE_EUICC_CHAL_FROM_CARD);
  }

  if(mcfg_fs_delete(LPA_FILE_ACTIVATION_CODE_FROM_USER, MCFG_FS_TYPE_EFS,
                    uim_common_convert_to_mcfg_context(context))
         != MCFG_FS_STATUS_OK)
  {
    LPA_MSG_ERR_0("Failure to remove file:");
    LPA_MSG_SPRINTF_1("%s", LPA_FILE_ACTIVATION_CODE_FROM_USER);
  }

  if(mcfg_fs_delete(LPA_FILE_STORE_DATA_RESP_FROM_CARD, MCFG_FS_TYPE_EFS,
                    uim_common_convert_to_mcfg_context(context))
         != MCFG_FS_STATUS_OK)
  {
    LPA_MSG_ERR_0("Failure to remove file:");
    LPA_MSG_SPRINTF_1("%s", LPA_FILE_STORE_DATA_RESP_FROM_CARD);
  }

  return LPA_SUCCESS;
#else
  (void)slot_id;
  return LPA_SUCCESS;
#endif /* FEATURE_LPA_STORE_EFS */
}/* lpa_efs_cleanup */


/*===========================================================================
FUNCTION LPA_EFS_WRITE

DESCRIPTION
  Generic Function called by LPA to write to the file in EFS. If the file does
  note exist, it creates it. If the file exists, it furst truncates it and
  then writes to it.

DEPENDENCIES
  None

RETURN VALUE
  lpa_result_enum_type:
           LPA_SUCCESS:          File was written
           LPA_GENERIC_ERROR:    Failure occurred

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_efs_write(
  const char                  *file_path_ptr,
  uint32                       data_buffer_len,
  uint8                       *data_buffer_ptr,
  lpa_slot_id_enum_type        slot_id
)
{
#ifdef FEATURE_LPA_STORE_EFS
  lpa_result_enum_type        lpa_status   = LPA_SUCCESS;
  int                         file_descriptor = 0;
  uim_common_efs_context_type context = (uim_common_efs_context_type) slot_id;

  if(file_path_ptr == NULL ||
     data_buffer_ptr == NULL ||
     slot_id >= LPA_NUM_SLOTS)
  {
    LPA_MSG_ERR_0("NULL File path or data buffer");
    return LPA_GENERIC_ERROR;
  }

  LPA_MSG_HIGH_2("For slot_id=0x%x, attempting to write 0x%x bytes to file:",
                 slot_id, data_buffer_len);
  LPA_MSG_SPRINTF_1("%s", file_path_ptr);

  /* Open the File in EFS for Create as a New File */
  file_descriptor = mcfg_fopen(file_path_ptr,
                               MCFG_FS_O_WRONLY|MCFG_FS_O_CREAT|MCFG_FS_O_TRUNC,
                               MCFG_FS_ALLPERMS,
                               MCFG_FS_TYPE_EFS,
                               uim_common_convert_to_mcfg_context(context));
  if(file_descriptor < 0)
  {
    LPA_MSG_ERR_0("efs open Procedure Failed ");
    return LPA_GENERIC_ERROR;
  }

  /* Write the data to the file */
  if(mcfg_fwrite(file_descriptor,
                 (void *)data_buffer_ptr,
                 data_buffer_len,
                 MCFG_FS_TYPE_EFS) != (int)data_buffer_len)
  {
    LPA_MSG_ERR_0("efs write failed ");
    lpa_status = LPA_GENERIC_ERROR;
  }

  /* Data has been successfully written to EFS, close it */
  if(mcfg_fclose(file_descriptor, MCFG_FS_TYPE_EFS) != 0)
  {
    LPA_MSG_HIGH_1("efs close procedure failed 0x%x", file_descriptor);
  }

  return lpa_status;
#else
  (void)file_path_ptr;
  (void)data_buffer_len;
  (void)data_buffer_ptr;
  (void)slot_id;
  return LPA_GENERIC_ERROR;
#endif /* FEATURE_LPA_STORE_EFS */
} /* lpa_efs_write_file */


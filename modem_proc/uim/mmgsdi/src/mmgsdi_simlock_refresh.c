/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 M M G S D I   S I M L O C K   R E F R E S H   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains functions to perform REFRESH specific
  SIM Lock operations.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2014 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_simlock_refresh.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/09/14   vv      Fixed compiler warnings
08/28/14   vv      Initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "comdef.h"
#include "uim_msg.h"

#include "mmgsdilib_common.h"
#include "mmgsdi_simlock_refresh.h"
#include "mmgsdiutil.h"
#include "mmgsdi_card_init.h"
#include "mmgsdi_simlock.h"
#include "mmgsdi_uim_uicc.h"
#include "mmgsdi_uim_icc.h"
#include "simlock_modem_lib.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

#ifdef FEATURE_SIMLOCK

/*===========================================================================

                        DEFINITIONS

===========================================================================*/
#define MMGSDI_SIMLOCK_3GPP_DCK_FILE_LEN        16
#define MMGSDI_SIMLOCK_3GPP2_DCK_FILE_LEN       20

#define MMGSDI_SIMLOCK_REFRESH_NW_CK_INDEX      0
#define MMGSDI_SIMLOCK_REFRESH_NS_CK_INDEX      4
#define MMGSDI_SIMLOCK_REFRESH_SP_CK_INDEX      8
#define MMGSDI_SIMLOCK_REFRESH_CP_CK_INDEX      12
#define MMGSDI_SIMLOCK_REFRESH_HRPD_CK_INDEX    16

/* ck length in EF-DCK file */
#define MMGSDI_SIMLOCK_DCK_CK_LEN               4


/* ==========================================================================
FUNCTION MMGSDI_SIMLOCK_REFRESH_UNLOCK_CB

DESCRIPTION
  This is the callback function, that MMGSDI uses in the unlock
  request to the SIMLock

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_simlock_refresh_unlock_cb (
  simlock_result_enum_type                    status,
  const simlock_message_response_data_type  * resp_ptr,
  const void                                * user_data_ptr
)
{
  (void)status;
  (void)resp_ptr;
  (void)user_data_ptr;
} /* mmgsdi_simlock_refresh_unlock_cb */


/* ==========================================================================
FUNCTION MMGSDI_SIMLOCK_REFRESH_UPDATE_DCK_FILE

DESCRIPTION
  This function updates the EF-DCK file with the default ck values

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_simlock_refresh_update_dck_file
(
  mmgsdi_session_id_type     session_id,
  mmgsdi_slot_id_enum_type   slot_id,
  mmgsdi_file_enum_type      file,
  mmgsdi_len_type            file_data_len
)
{
  mmgsdi_write_req_type         * write_req_ptr      = NULL;
  mmgsdi_write_cnf_type         * write_cnf_ptr      = NULL;
  mmgsdi_client_id_type           client_id          = 0;
  mmgsdi_return_enum_type         mmgsdi_status      = MMGSDI_ERROR;
  uint8                           mmgsdi_default_dck_data[MMGSDI_SIMLOCK_3GPP2_DCK_FILE_LEN];

  mmgsdi_status = mmgsdi_util_get_client_id(session_id, &client_id);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  if((file_data_len <= 0) ||
     (file_data_len > MMGSDI_SIMLOCK_3GPP2_DCK_FILE_LEN))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  UIM_MSG_MED_0("mmgsdi_simlock_refresh_update_dck_file");

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(write_req_ptr,
                                     sizeof(mmgsdi_write_req_type));
  if(write_req_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  write_req_ptr->request_header.client_id         = client_id;
  write_req_ptr->request_header.session_id        = session_id;
  write_req_ptr->request_header.request_type      = MMGSDI_WRITE_REQ;
  write_req_ptr->request_header.orig_request_type = MMGSDI_WRITE_REQ;
  write_req_ptr->request_header.slot_id           = slot_id;
  write_req_ptr->request_header.client_data       = 0;
  write_req_ptr->request_header.response_cb       = 0;
  write_req_ptr->access.access_method             = MMGSDI_EF_ENUM_ACCESS;
  write_req_ptr->int_client_data                  = 0;
  write_req_ptr->access.file.file_enum            = file;
  write_req_ptr->data.data_len                    = file_data_len;

  /* update with the default ck */
  memset(mmgsdi_default_dck_data, 0xFF, MMGSDI_SIMLOCK_3GPP2_DCK_FILE_LEN);

  write_req_ptr->data.data_ptr = mmgsdi_default_dck_data;

  if((file == MMGSDI_USIM_DCK) ||
     (file == MMGSDI_CSIM_DCK))
  {
    mmgsdi_status = mmgsdi_uim_uicc_write_transparent(
                     write_req_ptr,
                     TRUE,
                     &write_cnf_ptr);
  }
  else if(file == MMGSDI_GSM_DCK)
  {
    mmgsdi_status = mmgsdi_uim_icc_write_transparent(
                     write_req_ptr,
                     TRUE,
                     &write_cnf_ptr);
  }

  if (write_cnf_ptr)
  {
    mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)write_cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(write_cnf_ptr);
  }

  MMGSDIUTIL_TMC_MEM_FREE(write_req_ptr);

  return mmgsdi_status;
} /* mmgsdi_simlock_refresh_update_dck_file */


/* ==========================================================================
FUNCTION MMGSDI_SIMLOCK_REFRESH_SEND_UNLOCK

DESCRIPTION
  This function sends an unlock request to the SIMLock task

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_simlock_refresh_send_unlock
(
  const simlock_ck_data_type  * ck_data_ptr,
  mmgsdi_slot_id_enum_type      slot,
  simlock_category_enum_type    category
)
{
  simlock_result_enum_type              simlock_status = SIMLOCK_SUCCESS;
  simlock_unlock_device_msg_req_type    unlock_req;

  if(ck_data_ptr == NULL)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  memset(&unlock_req, 0x00, sizeof(simlock_unlock_device_msg_req_type));

  if(mmgsdi_simlock_convert_mmgsdi_slot_id(slot, &unlock_req.slot) !=
                        MMGSDI_SUCCESS)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  UIM_MSG_MED_0("mmgsdi_simlock_refresh_send_unlock");

  unlock_req.category             = category;
  unlock_req.ck.simlock_ck_length = ck_data_ptr->simlock_ck_length;

  mmgsdi_memscpy(unlock_req.ck.simlock_ck_data,
                 SIMLOCK_CK_MAX,
                 ck_data_ptr->simlock_ck_data,
                 ck_data_ptr->simlock_ck_length);

  simlock_status = simlock_unlock(unlock_req,
                                  mmgsdi_simlock_refresh_unlock_cb,
                                  NULL);

  return mmgsdi_simlock_convert_status(simlock_status);
} /* mmgsdi_simlock_refresh_send_unlock */


/* ==========================================================================
FUNCTION MMGSDI_SIMLOCK_REFRESH_GET_CK

DESCRIPTION
  This function gets the control key from the EF-DCK file data

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_simlock_refresh_get_ck
(
  const mmgsdi_data_type         * file_data_ptr,
  simlock_category_enum_type       category,
  simlock_ck_data_type           * ck_data_ptr
)
{
  uint8   mmgsdi_ck_default[MMGSDI_SIMLOCK_DCK_CK_LEN] = {0xFF,0xFF,0xFF,0xFF};
  uint8   ck_index                                     = 0;
  uint8   index                                        = 0;

  if((file_data_ptr == NULL) ||
     (ck_data_ptr == NULL))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  switch(category)
  {
    case SIMLOCK_CATEGORY_3GPP_NW:
    case SIMLOCK_CATEGORY_3GPP2_NW_TYPE1:
      ck_index = MMGSDI_SIMLOCK_REFRESH_NW_CK_INDEX;
      break;

    case SIMLOCK_CATEGORY_3GPP_NS:
    case SIMLOCK_CATEGORY_3GPP2_NW_TYPE2:
      ck_index = MMGSDI_SIMLOCK_REFRESH_NS_CK_INDEX;
      break;

    case SIMLOCK_CATEGORY_3GPP_SP:
    case SIMLOCK_CATEGORY_3GPP2_SP:
      ck_index = MMGSDI_SIMLOCK_REFRESH_SP_CK_INDEX;
      break;

    case SIMLOCK_CATEGORY_3GPP_CP:
    case SIMLOCK_CATEGORY_3GPP2_CP:
      ck_index = MMGSDI_SIMLOCK_REFRESH_CP_CK_INDEX;
      break;

    case SIMLOCK_CATEGORY_3GPP2_HRPD:
      ck_index = MMGSDI_SIMLOCK_REFRESH_HRPD_CK_INDEX;
      break;

    default:
      return MMGSDI_ERROR;
  }

  if((ck_index + MMGSDI_SIMLOCK_DCK_CK_LEN) <=
         file_data_ptr->data_len)
  {
    if (memcmp(&file_data_ptr->data_ptr[ck_index],
               mmgsdi_ck_default,
               MMGSDI_SIMLOCK_DCK_CK_LEN) == 0)
    {
      /* EF-DCK has the default CK for the category */
      ck_data_ptr->simlock_ck_length = 0;
      return MMGSDI_SUCCESS;
    }
  }

  for(index = ck_index; index < (ck_index + MMGSDI_SIMLOCK_DCK_CK_LEN);
      index++)
  {
    if(index >= file_data_ptr->data_len)
    {
      return MMGSDI_ERROR;
    }

    if((ck_data_ptr->simlock_ck_length + 2) < SIMLOCK_CK_MAX)
    {
      ck_data_ptr->simlock_ck_data[ck_data_ptr->simlock_ck_length++] =
             (char)((file_data_ptr->data_ptr[index] & 0x0F) + '0');
      ck_data_ptr->simlock_ck_data[ck_data_ptr->simlock_ck_length++] =
             (char)(((file_data_ptr->data_ptr[index] & 0xF0) >> 4) + '0');
    }
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_simlock_refresh_get_ck */


/* ==========================================================================
FUNCTION MMGSDI_SIMLOCK_REFRESH_PROCESS_UNLOCK

DESCRIPTION
  This function reads the EF-DCK file and extracts the control key. If a
  valid control key exists, then it sends an unlock request to the SIMLock.
  It then updates the EF-DCK file with the default control key

DEPENDENCIES
  The SIM Lock task and command queue must be up and running

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_simlock_refresh_process_unlock
(
  mmgsdi_session_id_type     session_id
)
{
  mmgsdi_return_enum_type     mmgsdi_status     = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type    slot_id           = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_file_enum_type       file_name         = MMGSDI_MAX_FILE_ENUM;
  mmgsdi_len_type             expected_file_len = 0;
  mmgsdi_int_app_info_type   *int_app_info_ptr  = NULL;
  simlock_category_enum_type  category          = SIMLOCK_CATEGORY_3GPP_NW;
  simlock_category_enum_type  category_begin    = SIMLOCK_CATEGORY_3GPP_NW;
  simlock_category_enum_type  category_end      = SIMLOCK_CATEGORY_3GPP_CP;
  boolean                     write_ef_dck      = FALSE;
  mmgsdi_data_type            file_data_buf;

  UIM_MSG_MED_0("mmgsdi_simlock_refresh_process_unlock");

  memset(&file_data_buf, 0x00, sizeof(mmgsdi_data_type));

  mmgsdi_status = mmgsdi_util_get_prov_session_info(session_id,
                                                    NULL,
                                                    &slot_id,
                                                    &int_app_info_ptr);
  if ((mmgsdi_status != MMGSDI_SUCCESS) || (int_app_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }

  switch(int_app_info_ptr->app_data.app_type)
  {
    case MMGSDI_APP_USIM:
      file_name          = MMGSDI_USIM_DCK;
      expected_file_len  = MMGSDI_SIMLOCK_3GPP_DCK_FILE_LEN;
      category_begin     = SIMLOCK_CATEGORY_3GPP_NW;
      category_end       = SIMLOCK_CATEGORY_3GPP_CP;
      break;

    case MMGSDI_APP_SIM:
      file_name          = MMGSDI_GSM_DCK;
      expected_file_len  = MMGSDI_SIMLOCK_3GPP_DCK_FILE_LEN;
      category_begin     = SIMLOCK_CATEGORY_3GPP_NW;
      category_end       = SIMLOCK_CATEGORY_3GPP_CP;
      break;

    case MMGSDI_APP_CSIM:
      file_name          = MMGSDI_CSIM_DCK;
      expected_file_len  = MMGSDI_SIMLOCK_3GPP2_DCK_FILE_LEN;
      category_begin     = SIMLOCK_CATEGORY_3GPP2_NW_TYPE1;
      category_end       = SIMLOCK_CATEGORY_3GPP2_CP;
      break;

    case MMGSDI_APP_RUIM:
      return MMGSDI_NOT_SUPPORTED;

    default:
      return MMGSDI_ERROR;
  }

  /* read the EF-DCK file */
  mmgsdi_status = mmgsdi_card_init_cache_binary(session_id,
                                                int_app_info_ptr->app_data.app_type,
                                                slot_id,
                                                file_name,
                                                &file_data_buf);
  if((mmgsdi_status != MMGSDI_SUCCESS) ||
     (file_data_buf.data_len == 0) ||
     (file_data_buf.data_ptr == NULL))
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(file_data_buf.data_ptr);
    return mmgsdi_status;
  }

  /* check the length of EF-DCK file */
  if(file_data_buf.data_len != expected_file_len)
  {
    UIM_MSG_MED_1("invalid file len, 0x%x", file_data_buf.data_len);
    MMGSDIUTIL_TMC_MEM_FREE(file_data_buf.data_ptr);
    return MMGSDI_ERROR;
  }

  /* check if a valid CK is present in EF-DCK for any category */
  for(category = category_begin; category <= category_end;
      category++)
  {
    simlock_ck_data_type          ck_data;
    memset(&ck_data, 0x00, sizeof(simlock_ck_data_type));

    /* get the ck from the EF-DCK data */
    mmgsdi_status = mmgsdi_simlock_refresh_get_ck(&file_data_buf,
                                                  category,
                                                  &ck_data);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_1("unable to extract CK for category, 0x%x", category);
      MMGSDIUTIL_TMC_MEM_FREE(file_data_buf.data_ptr);
      return MMGSDI_ERROR;
    }

    if(ck_data.simlock_ck_length == 0)
    {
      continue;
    }

    /* a valid control key is present */
    write_ef_dck = TRUE;

    /* call the engine to unlock the category */
    mmgsdi_status = mmgsdi_simlock_refresh_send_unlock(&ck_data, slot_id,
                                                       category);
  }

  MMGSDIUTIL_TMC_MEM_FREE(file_data_buf.data_ptr);

  /* if a valid control key was found for atleast one category, then
     after unlocking it, the file needs to be updated with the 
     default control key */
  if(write_ef_dck)
  {
    mmgsdi_status = mmgsdi_simlock_refresh_update_dck_file(session_id, slot_id,
                                                           file_name, expected_file_len);
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_simlock_refresh_process_unlock */

#endif /* FEATURE_SIMLOCK */


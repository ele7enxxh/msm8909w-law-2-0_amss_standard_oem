/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            M M G S D I   1 X   U P P   F U N C T I O N S

GENERAL DESCRIPTION

  This source file contains the functions related to 1X UPP files

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_1x_upp.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/29/14   hh      Fix compliation warnings
08/19/14   hh      Initial versoin
===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdi_1x_upp.h"
#include "mmgsdi_cnf.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
#ifdef FEATURE_MMGSDI_3GPP2

/* Length of default MDN pattern */
#define MMGSDI_MDN_PATTERN_LEN                  3

/* Data offset in EF-MDN */
#define MMGSDI_MDN_OFFSET                       1

/* Total length of MDN */
#define MMGSDI_MDN_LENGTH                       11

/* File ID for EF MDN under CSIM */
#define MMGSDI_MDN_FILE_ID                      0x6F44

/* Length of Network Access Identifier */
#define MMGSDI_NAI_LENGTH                       10

/* Data offset in EF-SIPUPP */
#define MMGSDI_SIPUPP_DATA_OFFSET               3

/* Data offset in EF-MIPUPP */
#define MMGSDI_MIPUPP_DATA_OFFSET               4

/* Data offset in EF-HRPDUPP */
#define MMGSDI_HRPDUPP_DATA_OFFSET              2

/* Default data in UPP files (ASCII 0) */
#define MMGSDI_UPP_DATA_DEFAULT_VALUES          0x30

/* Default data in EF-MIPUPP (ASCII 0 left shifted by 1 nibble) */
#define MMGSDI_MIPUPP_DATA_DEFAULT_VALUES       0x03

/* First data byte in EF-MIPUPP */
#define MMGSDI_MIPUPP_DATA_FIRST_BYTE           0x83

/* Last data byte in EF-MIPUPP */
#define MMGSDI_MIPUPP_DATA_LAST_BYTE            0x04

/* Digit '0' encoded in DTMF format */
#define MMGSDI_DIGIT_ZERO_DTMF                  0x0A


/*===========================================================================
         S T A T I C    F U N C T I O N S
============================================================================*/

/*================================================================
FUNCTION  MMGSDI_BUILD_AND_QUEUE_OTASP_COMMIT_CNF

DESCRIPTION:
  This function queues the confirmation for OTASP COMMIT that was
  delayed in order to read MDN or write user profile parameters.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===============================================================*/
static mmgsdi_return_enum_type mmgsdi_build_and_queue_otasp_commit_cnf
(
  mmgsdi_uim_report_rsp_type * otasp_commit_rsp_ptr
)
{
  mmgsdi_request_header_type           req_header;
  mmgsdi_return_enum_type              mmgsdi_status    = MMGSDI_SUCCESS;
  uim_cmd_type                         *uim_cmd_ptr     = NULL;
  mmgsdi_client_req_extra_info_type    *extra_param_ptr = NULL;
  int32                                index            = 0;

  if (otasp_commit_rsp_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  memset(&req_header, 0, sizeof(req_header));

  index = otasp_commit_rsp_ptr->rsp_data_index;
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_REQ_INFO - 1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(index,
                                                            &req_header,
                                                            &uim_cmd_ptr,
                                                            &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Error in getting info from client req table");
    return MMGSDI_ERROR;
  }

  if(req_header.orig_request_type != MMGSDI_SESSION_OTASP_COMMIT_REQ )
  {
    UIM_MSG_HIGH_0("Not an OTASP COMMIT request; no need to send CNF");
    return MMGSDI_ERROR;
  }

  /* If a confirmation has already been sent for this command as part of
     CARD_ERROR handling, free only the client_req_table index. */
  if(mmgsdi_client_req_table_info_ptr[index]->uim_rsp_status == MMGSDI_UIM_RSP_NOT_RECEIVED)
  {
    UIM_MSG_HIGH_2("Free client_req_table index 0x%x for cmd 0x%x",
                   index,
                   mmgsdi_client_req_table_info_ptr[index]->request_type);

    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(otasp_commit_rsp_ptr->rsp_data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(otasp_commit_rsp_ptr);

    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(MMGSDI_SUCCESS,
                                             &req_header,
                                             extra_param_ptr,
                                             (void*)&otasp_commit_rsp_ptr->rsp_data,
                                             FALSE, /* status word not required
                                                     for OTASP Commit operation */
                                             otasp_commit_rsp_ptr->status_word);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Unable to build proper CNF for OTASP COMMIT, status 0x%x , using static cnf buffer",
                  mmgsdi_status);

    mmgsdi_status = mmgsdi_build_and_queue_static_cnf(index, mmgsdi_status);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_0("Impossible to return cnf back to client");
    }
  }
  else
  {
    UIM_MSG_HIGH_0("Queued confirmation for OTASP COMMIT Request");
  }

  /* Free the copy of response that was cached before starting the user
     profile parameter update. */
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(otasp_commit_rsp_ptr->rsp_data.data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(otasp_commit_rsp_ptr);

  mmgsdi_util_free_client_request_table_index(index);

  return mmgsdi_status;
} /* mmgsdi_build_and_queue_otasp_commit_cnf */


/*================================================================
FUNCTION  MMGSDI_1X_UPP_WRITE_CB

DESCRIPTION:
  This is a response callback for write request for EFs
  SIPUPP, MIPUPP, or HRPDUPP.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===============================================================*/
static void mmgsdi_1x_upp_write_cb
(
  mmgsdi_return_enum_type cnf_status,
  mmgsdi_cnf_enum_type    cnf_type,
  const mmgsdi_cnf_type * cnf_ptr
)
{
  mmgsdi_slot_data_type   * slot_data_ptr = NULL;

  if(cnf_ptr == NULL || cnf_type != MMGSDI_WRITE_CNF)
  {
    return;
  }

  UIM_MSG_HIGH_2("mmgsdi_1x_upp_write_cb(): file_enum=0x%x,  cnf_status=0x%x",
                 cnf_ptr->write_cnf.access.file.file_enum, cnf_status);

  /* HRPDUPP is the last file that gets updated after OTASP_COMMIT_REQ succeeds
     Per Call flow in 80-ND946-1 A Cricket UICC Provisioning Overview, cnf is
     sent after write update ends.
   */
  if(cnf_ptr->write_cnf.access.file.file_enum == MMGSDI_CSIM_HRPDUPP)
  {
    if(MMGSDI_SUCCESS == cnf_status)
    {
      slot_data_ptr = mmgsdi_util_get_slot_data_ptr(cnf_ptr->response_header.slot_id);
      if (slot_data_ptr)
      {
        slot_data_ptr->upp_update_required = FALSE;
      }
    }

    /* Confirmation is needed regardless of the result of UPP file update */
    if(mmgsdi_build_and_queue_otasp_commit_cnf(
         (mmgsdi_uim_report_rsp_type *)cnf_ptr->response_header.client_data) !=
       MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_0("Unable to queue CNF for OTASP COMMIT in mmgsdi_1x_upp_write_cb");
    }
  }
} /* mmgsdi_1x_upp_write_cb */


/*================================================================
FUNCTION  MMGSDI_1X_CONVERT_MDN_TO_UPP

DESCRIPTION:
  This function is called to convert NAI value from EF-MDN in
  correct format for writting to EFs SIPUPP, MIPUPP and HRPDUPP.

  INPUT PARAMETERS:
    mdn_data_ptr    : Pointer to the data read from EF-MDN
    mdn_len         : Length of MDN
  OUTPUT PARAMETERS:
    upp_data        : Pointer to the array of length
                      MMGSDI_NAI_LENGTH to be written to SIPUPP
                      and HRPDUPP files
    mipupp_data    : Pointer to the array of length
                      MMGSDI_NAI_LENGTH+1 to be written to MIPUPP

DEPENDENCIES:
  None

LIMITATIONS:
  Data allocated for UPP files needs to be freed by the caller of
  this function.

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===============================================================*/
static mmgsdi_return_enum_type mmgsdi_1x_convert_mdn_to_upp
(
  const uint8      * mdn_data_ptr,
  uint8              mdn_len,
  mmgsdi_data_type * upp_data,
  mmgsdi_data_type * mipupp_data
)
{
  uint8 i         = 0;
  uint8 temp_data = 0;

  MMGSDIUTIL_RETURN_IF_NULL(mdn_data_ptr);

  if(mdn_len < MMGSDI_MDN_LENGTH)
  {
    UIM_MSG_ERR_1("Invalid length for EF-MDN 0x%x", mdn_len);
    return MMGSDI_ERROR;
  }

  if(upp_data != NULL)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(upp_data->data_ptr, MMGSDI_NAI_LENGTH);
    if (upp_data->data_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    memset(upp_data->data_ptr, MMGSDI_UPP_DATA_DEFAULT_VALUES, MMGSDI_NAI_LENGTH);
    upp_data->data_len = MMGSDI_NAI_LENGTH;
  }

  if(mipupp_data != NULL)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mipupp_data->data_ptr, MMGSDI_NAI_LENGTH+1);
    if (mipupp_data->data_ptr == NULL)
    {
      if (upp_data != NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(upp_data->data_ptr);
      }
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    memset(mipupp_data->data_ptr,
           MMGSDI_MIPUPP_DATA_DEFAULT_VALUES,
           MMGSDI_NAI_LENGTH+1);
    mipupp_data->data_ptr[0] = MMGSDI_MIPUPP_DATA_FIRST_BYTE;
    mipupp_data->data_ptr[MMGSDI_NAI_LENGTH] = MMGSDI_MIPUPP_DATA_LAST_BYTE;
    mipupp_data->data_len = MMGSDI_NAI_LENGTH+1;
  }

  for(i = 0; i < MMGSDI_NAI_LENGTH/2; i++)
  {
    temp_data = mdn_data_ptr[i+MMGSDI_MDN_OFFSET];

    /* In EF-MDN where digits are encoded in DTMF, digit '0' is encoded as 0xA.
       Convert it back to 0x0 before writing it to UPP files. */
    if((temp_data & 0x0F) == MMGSDI_DIGIT_ZERO_DTMF)
    {
      temp_data &= 0xF0;
    }
    if(((temp_data >> 4) & 0x0F) == MMGSDI_DIGIT_ZERO_DTMF)
    {
      temp_data &= 0x0F;
    }

    if(upp_data != NULL)
    {
      upp_data->data_ptr[2*i] |= temp_data & 0x0F;
      upp_data->data_ptr[2*i+1] |= (temp_data >> 4) & 0x0F;
    }

    if(mipupp_data != NULL)
    {
      mipupp_data->data_ptr[2*i+1] |= (temp_data << 4) & 0xF0;
      mipupp_data->data_ptr[2*i+2] |= temp_data & 0xF0;
    }
  }

  return MMGSDI_SUCCESS;
}/* mmgsdi_1x_convert_mdn_to_upp */


/*================================================================
FUNCTION  MMGSDI_1X_WRITE_UPP

DESCRIPTION:
  This function is called to queue a write requests for CSIM EFs
  SIPUPP, MIPUPP, and HRPDUPP.

  INPUT PARAMETERS:
    session_id     :  Id for a 1x primary session
    read_data_ptr :   Pointer to the data read from EF-MDN
    read_data_len :   Length of the data read from EF-MDN
    client_index   :  Index of OTASP COMMIT request in client req
                      table


DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===============================================================*/
static mmgsdi_return_enum_type mmgsdi_1x_write_upp
(
  mmgsdi_session_id_type       session_id,
  uint8                      * read_data_ptr,
  uint32                       read_data_len,
  mmgsdi_uim_report_rsp_type * otasp_commit_rsp_ptr
)
{
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_data_type          upp_data;
  mmgsdi_data_type          mipupp_data;
  mmgsdi_write_data_type    write_data;
  mmgsdi_access_type        file_access;

  memset(&file_access, 0, sizeof(file_access));
  memset(&write_data, 0, sizeof(write_data));
  memset(&upp_data, 0, sizeof(upp_data));
  memset(&mipupp_data, 0, sizeof(mipupp_data));

  if (read_data_ptr == NULL || read_data_len < MMGSDI_MDN_LENGTH )
  {
    UIM_MSG_ERR_0("read_data_ptr NULL or read_data_len invalid");
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_1x_convert_mdn_to_upp(read_data_ptr,
                                               (uint8)read_data_len,
                                               &upp_data,
                                               &mipupp_data);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* MDN has been updated. Write the MDN value to CSIM EFs SIPUPP, MIPUPP,
     and HRPDUPP.
     The client_data (which is a pointer to the copy of OTASP
     Commit response) is passed only for the third WRITE operation so that
     OTASP Commit cnf can be queued after receeiving response for the third
     WRITE. */
  file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
  file_access.file.file_enum = MMGSDI_CSIM_3GPD_SIPUPP;
  write_data.data_len = upp_data.data_len;
  write_data.data_ptr = upp_data.data_ptr;
  mmgsdi_status = mmgsdi_session_write_transparent(session_id,
                                                   file_access,
                                                   MMGSDI_SIPUPP_DATA_OFFSET,
                                                   write_data,
                                                   mmgsdi_1x_upp_write_cb,
                                                   0);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Unable to queue write for EF-SIPUPP");
    MMGSDIUTIL_TMC_MEM_FREE(upp_data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(mipupp_data.data_ptr);
    return mmgsdi_status;
  }

  file_access.file.file_enum = MMGSDI_CSIM_3GPD_MIPUPP;
  write_data.data_len = mipupp_data.data_len;
  write_data.data_ptr = mipupp_data.data_ptr;
  mmgsdi_status = mmgsdi_session_write_transparent(session_id,
                                                   file_access,
                                                   MMGSDI_MIPUPP_DATA_OFFSET,
                                                   write_data,
                                                   mmgsdi_1x_upp_write_cb,
                                                   0);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Unable to queue write for EF-MIPUPP");
    MMGSDIUTIL_TMC_MEM_FREE(upp_data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(mipupp_data.data_ptr);
    return mmgsdi_status;
  }

  /* Pass the client data from MDN read operation as the client_ref. This
     client_index is the index of OTASP_COMMIT request in
     mmgsdi_client_req_table and will be used in the write callback to queue
     OTASP confirmation */
  file_access.file.file_enum = MMGSDI_CSIM_HRPDUPP;
  write_data.data_len = upp_data.data_len;
  write_data.data_ptr = upp_data.data_ptr;
  mmgsdi_status = mmgsdi_session_write_transparent(session_id,
                                                   file_access,
                                                   MMGSDI_HRPDUPP_DATA_OFFSET,
                                                   write_data,
                                                   mmgsdi_1x_upp_write_cb,
                                                   (mmgsdi_client_data_type)otasp_commit_rsp_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Unable to queue write for EF-HRPDUPP");
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(upp_data.data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mipupp_data.data_ptr);
  return mmgsdi_status;
} /* mmgsdi_1x_write_upp */


/*================================================================
FUNCTION  MMGSDI_1X_READ_UPP_CB

DESCRIPTION:
  This is a response callback for read request for EFs SIPUPP,
  MIPUPP, and HRPDUPP. If the contents of any of these files do
  not correspond to the MDN value, then the MDN is written to that
  UPP file.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===============================================================*/
static void mmgsdi_1x_read_upp_cb
(
  mmgsdi_return_enum_type cnf_status,
  mmgsdi_cnf_enum_type    cnf_type,
  const mmgsdi_cnf_type * cnf_ptr
)
{
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_ERROR;
  uint8                   * mdn_data_ptr  = NULL;
  mmgsdi_offset_type        offset          = 0;
  mmgsdi_data_type          upp_data;
  mmgsdi_data_type          mipupp_data;
  mmgsdi_write_data_type    write_data;
  mmgsdi_access_type        file_access;

  if(cnf_ptr == NULL || cnf_type != MMGSDI_READ_CNF ||
     cnf_ptr->response_header.client_data == 0)
  {
    return;
  }

  /* client_data has the pointer to the cached data from EF-MDN */
  mdn_data_ptr = (uint8 *)cnf_ptr->response_header.client_data;

  if(cnf_status != MMGSDI_SUCCESS)
  {
    /* Free the memory allocated in mmgsdi_1x_read_upp for storing MDN value */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mdn_data_ptr);
    return;
  }

  memset(&file_access, 0, sizeof(file_access));
  memset(&write_data, 0, sizeof(write_data));
  memset(&upp_data, 0, sizeof(upp_data));
  memset(&mipupp_data, 0, sizeof(mipupp_data));

  if(cnf_ptr->read_cnf.access.file.file_enum == MMGSDI_CSIM_3GPD_SIPUPP)
  {
    mmgsdi_status = mmgsdi_1x_convert_mdn_to_upp(mdn_data_ptr,
                                                 MMGSDI_MDN_LENGTH,
                                                 &upp_data,
                                                 NULL);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      /* Free the memory allocated in mmgsdi_1x_read_upp for storing MDN value */
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mdn_data_ptr);
      return;
    }
    /* Free the memory allocated in mmgsdi_1x_read_upp for storing MDN value */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mdn_data_ptr);

    if(cnf_ptr->read_cnf.read_data.data_len == upp_data.data_len &&
       memcmp(upp_data.data_ptr, cnf_ptr->read_cnf.read_data.data_ptr, upp_data.data_len) != 0)
    {
      UIM_MSG_HIGH_0("Updating EF-SIPUPP with MDN value");
      write_data.data_len = upp_data.data_len;
      write_data.data_ptr = upp_data.data_ptr;
      file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
      file_access.file.file_enum = MMGSDI_CSIM_3GPD_SIPUPP;
      offset = MMGSDI_SIPUPP_DATA_OFFSET;
    }
    else
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(upp_data.data_ptr);
      return;
    }
  }
  else if(cnf_ptr->read_cnf.access.file.file_enum == MMGSDI_CSIM_3GPD_MIPUPP)
  {
    mmgsdi_status = mmgsdi_1x_convert_mdn_to_upp(mdn_data_ptr,
                                                 MMGSDI_MDN_LENGTH,
                                                 NULL,
                                                 &mipupp_data);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      /* Free the memory allocated in mmgsdi_1x_read_upp for storing MDN value */
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mdn_data_ptr);
      return;
    }
    /* Free the memory allocated in mmgsdi_1x_read_upp for storing MDN value */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mdn_data_ptr);

    if(cnf_ptr->read_cnf.read_data.data_len == mipupp_data.data_len &&
       memcmp(mipupp_data.data_ptr, cnf_ptr->read_cnf.read_data.data_ptr, mipupp_data.data_len) != 0)
    {
      UIM_MSG_HIGH_0("Updating EF-MIPUPP with MDN value");
      write_data.data_len = mipupp_data.data_len;
      write_data.data_ptr = mipupp_data.data_ptr;
      file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
      file_access.file.file_enum = MMGSDI_CSIM_3GPD_MIPUPP;
      offset = MMGSDI_MIPUPP_DATA_OFFSET;
    }
    else
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mipupp_data.data_ptr);
      return;
    }
  }
  else if(cnf_ptr->read_cnf.access.file.file_enum == MMGSDI_CSIM_HRPDUPP)
  {
    mmgsdi_status = mmgsdi_1x_convert_mdn_to_upp(mdn_data_ptr,
                                                 MMGSDI_MDN_LENGTH,
                                                 &upp_data,
                                                 NULL);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      /* Free the memory allocated in mmgsdi_1x_read_upp for storing MDN value */
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mdn_data_ptr);
      return;
    }
    /* Free the memory allocated in mmgsdi_1x_read_upp for storing MDN value */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mdn_data_ptr);

    if(cnf_ptr->read_cnf.read_data.data_len == upp_data.data_len &&
       memcmp(upp_data.data_ptr, cnf_ptr->read_cnf.read_data.data_ptr, upp_data.data_len) != 0)
    {
      UIM_MSG_HIGH_0("Updating EF-HRPDUPP with MDN value");
      write_data.data_len = upp_data.data_len;
      write_data.data_ptr = upp_data.data_ptr;
      file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
      file_access.file.file_enum = MMGSDI_CSIM_HRPDUPP;
      offset = MMGSDI_HRPDUPP_DATA_OFFSET;
    }
    else
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(upp_data.data_ptr);
      return;
    }
  }
  else
  {
    return;
  }

  mmgsdi_status = mmgsdi_session_write_transparent(cnf_ptr->response_header.session_id,
                                                   file_access,
                                                   offset,
                                                   write_data,
                                                   mmgsdi_1x_upp_write_cb,
                                                   0);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Unable to queue write for EF 0x%x",
                  file_access.file.file_enum);
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(upp_data.data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mipupp_data.data_ptr);
} /* mmgsdi_1x_read_upp_cb */


/*================================================================
FUNCTION  MMGSDI_1X_READ_UPP

DESCRIPTION:
  This function is called to queue read requests for CSIM EFs
  SIPUPP, MIPUPP, and HRPDUPP.

INPUT PARAMETERS:
    session_id   :  Id for a 1x primary session
    mdn_data_ptr :  Pointer to the data read from EF-MDN

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS:
  None
===============================================================*/
static mmgsdi_return_enum_type mmgsdi_1x_read_upp
(
  mmgsdi_session_id_type   session_id,
  const uint8            * mdn_data_ptr
)
{
  mmgsdi_return_enum_type   mmgsdi_status   = MMGSDI_ERROR;
  mmgsdi_access_type        file_access;
  uint8                   * mdn_data_sipupp_ptr = NULL;
  uint8                   * mdn_data_mipupp_ptr = NULL;
  uint8                   * mdn_data_hrpdupp_ptr = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(mdn_data_ptr);

  memset(&file_access, 0, sizeof(file_access));

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mdn_data_sipupp_ptr, MMGSDI_MDN_LENGTH);
  if(mdn_data_sipupp_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  mmgsdi_memscpy((void *)mdn_data_sipupp_ptr,
                 MMGSDI_MDN_LENGTH,
                 (void *)mdn_data_ptr,
                 MMGSDI_MDN_LENGTH);

  file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
  file_access.file.file_enum = MMGSDI_CSIM_3GPD_SIPUPP;
  mmgsdi_status = mmgsdi_session_read_transparent(session_id,
                                                  file_access,
                                                  MMGSDI_SIPUPP_DATA_OFFSET,
                                                  MMGSDI_NAI_LENGTH,
                                                  mmgsdi_1x_read_upp_cb,
                                                  (mmgsdi_client_data_type)mdn_data_sipupp_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mdn_data_sipupp_ptr);
    return mmgsdi_status;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mdn_data_mipupp_ptr, MMGSDI_MDN_LENGTH);
  if(mdn_data_mipupp_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  mmgsdi_memscpy((void *)mdn_data_mipupp_ptr,
                 MMGSDI_MDN_LENGTH,
                 (void *)mdn_data_ptr,
                 MMGSDI_MDN_LENGTH);

  file_access.file.file_enum = MMGSDI_CSIM_3GPD_MIPUPP;
  mmgsdi_status = mmgsdi_session_read_transparent(session_id,
                                                  file_access,
                                                  MMGSDI_MIPUPP_DATA_OFFSET,
                                                  MMGSDI_NAI_LENGTH+1,
                                                  mmgsdi_1x_read_upp_cb,
                                                  (mmgsdi_client_data_type)mdn_data_mipupp_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mdn_data_mipupp_ptr);
    return mmgsdi_status;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mdn_data_hrpdupp_ptr, MMGSDI_MDN_LENGTH);
  if(mdn_data_hrpdupp_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  mmgsdi_memscpy((void *)mdn_data_hrpdupp_ptr,
                 MMGSDI_MDN_LENGTH,
                 (void *)mdn_data_ptr,
                 MMGSDI_MDN_LENGTH);

  file_access.file.file_enum = MMGSDI_CSIM_HRPDUPP;
  mmgsdi_status = mmgsdi_session_read_transparent(session_id,
                                                  file_access,
                                                  MMGSDI_HRPDUPP_DATA_OFFSET,
                                                  MMGSDI_NAI_LENGTH,
                                                  mmgsdi_1x_read_upp_cb,
                                                  (mmgsdi_client_data_type)mdn_data_hrpdupp_ptr);

  return mmgsdi_status;
} /* mmgsdi_1x_read_upp */


/*================================================================
FUNCTION  MMGSDI_1X_READ_MDN_AND_VALIDATE_UPP_FILES_CB

DESCRIPTION:
  This is a response callback for read request for EF MDN.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===============================================================*/
static void mmgsdi_1x_read_mdn_and_validate_upp_files_cb
(
  mmgsdi_return_enum_type cnf_status,
  mmgsdi_cnf_enum_type    cnf_type,
  const mmgsdi_cnf_type * cnf_ptr
)
{
  /* Default value when EF-MDN in CSIM is not provisioned */
  const uint8                  mmgsdi_default_mdn_value[] = {0x99, 0x99, 0x99};
  mmgsdi_slot_data_type       *slot_data_ptr              = NULL;
  mmgsdi_return_enum_type      mmgsdi_status              = MMGSDI_ERROR;
  boolean                      mdn_provisioned            = FALSE;
  mmgsdi_uim_report_rsp_type  *otasp_commit_rsp_ptr       = NULL;

  if(cnf_ptr == NULL || cnf_type != MMGSDI_READ_CNF)
  {
    return;
  }

  otasp_commit_rsp_ptr = (mmgsdi_uim_report_rsp_type *)
                           cnf_ptr->response_header.client_data;

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(cnf_ptr->response_header.slot_id);
  if (slot_data_ptr == NULL)
  {
    return;
  }

  do
  {
    if(cnf_status != MMGSDI_SUCCESS ||
       cnf_ptr->read_cnf.read_data.data_ptr == NULL ||
       cnf_ptr->read_cnf.read_data.data_len != MMGSDI_MDN_LENGTH)
    {
      UIM_MSG_ERR_0("Error in reading CSIM MDN");
      break;
    }

    if(memcmp(mmgsdi_default_mdn_value,
              &cnf_ptr->read_cnf.read_data.data_ptr[MMGSDI_MDN_OFFSET],
              MMGSDI_MDN_PATTERN_LEN) != 0)
    {
      mdn_provisioned = TRUE;
    }

    if (!slot_data_ptr->upp_update_required)
    {
      if (!mdn_provisioned)
      {
        /* At power up with unprovisioned card, just set the
           upp_update_required flag to TRUE and return. The UPP update logic
           will be carried out after OTASP when the MDN gets provisioned. */
        slot_data_ptr->upp_update_required = TRUE;
        return;
      }
      else
      {
        /* This will be reached at power up with a provisioned card. UPP files
           are read to ensure that the data in EF-MDN matches the NAI value in
           EFs SIPUPP, MIPUPP, and HRPDUPP. */
        mmgsdi_status = mmgsdi_1x_read_upp(cnf_ptr->response_header.session_id,
                                           cnf_ptr->read_cnf.read_data.data_ptr);
        break;
      }
    }
    else
    {
      if (!mdn_provisioned)
      {
        /* If the MDN is still not provisioned after an OTASP, queue the
           confirmation for OTASP COMMIT */
        break;
      }
    }

    mmgsdi_status = mmgsdi_1x_write_upp(
                      cnf_ptr->response_header.session_id,
                      cnf_ptr->read_cnf.read_data.data_ptr,
                      cnf_ptr->read_cnf.read_data.data_len,
                      otasp_commit_rsp_ptr);

  }while(0);

  /* Queue the OTASP_COMMIT confirmation if read/write to UPP files was not
     successful and pointer to the cached OTASP_COMMIT response is available.
     otasp_commit_rsp_ptr (passed as client_data) is valid when MDN read
     request is queued during processing of OTASP COMMIT response. When MDN
     is read at power up during CSIM Post PIN1 init, no client_data is passed
     and hence otasp_commit_rsp_ptr would be NULL. */
  if (mmgsdi_status != MMGSDI_SUCCESS &&
      otasp_commit_rsp_ptr != NULL)
  {
    if(mmgsdi_build_and_queue_otasp_commit_cnf(otasp_commit_rsp_ptr) !=
       MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_0("Did not queue CNF for OTASP COMMIT in mmgsdi_1x_upp_write_cb");
    }
  }
} /* mmgsdi_1x_read_mdn_and_validate_upp_files_cb */


/*===========================================================================
         E X T E R N A L    F U N C T I O N S
============================================================================*/

/*================================================================
FUNCTION  MMGSDI_1X_READ_MDN_AND_VALIDATE_UPP_FILES

DESCRIPTION:
  This function is called to queue a read request for EF-MDN using
  1x PRI session.

  INPUT PARAMETERS:
    session_id :             Id for a 1x primary session
    otasp_commit_rsp_ptr:    Pointer to OTASP commit response from
                             UIM. It is passed as client_data and
                             will be valid only when this function
                             is called during processing of OTASP
                             Commit response.


DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===============================================================*/
mmgsdi_return_enum_type mmgsdi_1x_read_mdn_and_validate_upp_files
(
  mmgsdi_session_id_type         session_id,
  mmgsdi_uim_report_rsp_type   * otasp_commit_rsp_ptr
)
{
  mmgsdi_access_type        file_access;

  memset(&file_access, 0, sizeof(file_access));

  file_access.access_method = MMGSDI_BY_PATH_ACCESS;
  file_access.file.path_type.path_len = 3;
  file_access.file.path_type.path_buf[0] = MMGSDI_MF_LEVEL;
  file_access.file.path_type.path_buf[1] = MMGSDI_ADF_LEVEL;
  file_access.file.path_type.path_buf[2] = MMGSDI_MDN_FILE_ID;

  return mmgsdi_session_read_record(session_id,
                                    file_access,
                                    1,
                                    MMGSDI_MDN_LENGTH,
                                    mmgsdi_1x_read_mdn_and_validate_upp_files_cb,
                                    (mmgsdi_client_data_type)otasp_commit_rsp_ptr);
} /* mmgsdi_1x_read_mdn_and_validate_upp_files */
#endif /* FEATURE_MMGSDI_3GPP2 */

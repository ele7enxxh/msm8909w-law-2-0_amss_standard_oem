/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                        M M G S D I   L I B R A R Y


GENERAL DESCRIPTION

  This file contains jcdma ruim functions


                        COPYRIGHT INFORMATION

Copyright (c) 2007-2012, 2014-2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_ruim_jcdma.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/17/16   tkl     F3 log prints cleanup
08/10/15   vdc     Add length checks to prevent invalid dereference
10/13/14   hh      Fix KW error
10/12/14   hh      Support slot refresh for NV 69687 JCDMA Mode
09/09/14   kk      Enhancements in NV handling
08/11/14   bcho    Optimize srv req by using cached srv table in client context
07/15/14   yt      Support for NV refresh
06/03/14   tl      Remove additional parameter variables
03/27/14   yt      Add processing for slot 3
01/11/14   df      Fix off-target compilation error
01/10/14   df      Use v2.0 diag macros
10/04/13   vdc     F3 message reduction
01/09/14   ar      Replace old Macros with MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE
10/04/13   vdc     F3 message reduction
10/04/13   vdc     F3 message reduction
06/24/13   vdc     Added support for triple SIM
05/16/13   vdc     Replace memcpy with safer version memscpy
10/11/12   spo     Removed unnecessary F3 messages
09/28/12   abg     Updated ERR to MSG_ERROR
09/11/12   tl      Fix KW error
03/16/12   yt      Fix compiler warning
02/18/12   av      Cleaned up remaining ALWAYS ON features
12/21/11   bcho    Legacy GSDI removal updates
11/28/11   nmb     Check for NULL ptr before freeing
08/01/11   av      Slot-specific handling of JCDMA apis
05/17/11   shr     Updates for Thread safety
01/31/11   ssr     Decoupled FEATURE_UIM_JCDMA_RUIM_SUPPORT feature
01/27/11   ssr     Fixed compilation warning
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
11/30/10   yt      Including private UIMDRV header
07/01/10   nmb     Updates for JCDMA CSIM support
09/26/09   kp      ZI memory reduction changes
04/22/09   sun     Fixed Includes
09/17/08   nb      Resolved compiler warning
06/04/08   tml     Added sw to cnf header
01/24/08   nk      Added nv write if jcdma nv items are not active
11/20/07   vs      Fixed warning, lint error due to empty translation unit
11/15/07   sun     Lint Fixes
06/20/07   nk      Fixed mem free issue.
05/31/07   nk      Fixed sending and building of cnf
05/30/07   nk      Resolved compiler warning
05/24/07   nk      Resolved lint error
05/17/07   nk      Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "mmgsdi.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "intconv.h"
#include "mmgsdilib_common.h"
#include "mmgsdisessionlib.h"
#include "mmgsdilib.h"
#include "uim.h"
#include "uim_p.h"
#include "nv_items.h"
#include "mmgsdi_nv.h"
#include "mmgsdi_uim_icc.h"
#include "mmgsdi_uim_uicc.h"
#include "mmgsdi_ruim_jcdma.h"
#include "mmgsdi_cnf.h"
#include "mmgsdi_uicc.h"



/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
static mmgsdi_jcdma_uicc_info_type* mmgsdi_jcdma_get_uicc_info_ptr_from_slot(
  mmgsdi_slot_id_enum_type  slot_id);

static mmgsdi_return_enum_type mmgsdi_jcdma_card_get_attr(
  mmgsdi_session_id_type    session_id);

static mmgsdi_return_enum_type mmgsdi_jcdma_card_get_color_and_locks(
  mmgsdi_jcdma_card_attr_type *card_attr_ptr,
  mmgsdi_session_id_type       session_id);

static void mmgsdi_jcdma_evt_cb (
  const mmgsdi_event_data_type *event_ptr
);

static void mmgsdi_jcdma_session_open_cb (
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
);

static void mmgsdi_jcdma_session_close_cb (
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
);

/* For jcdma card in each slot */
static mmgsdi_jcdma_uicc_info_type *mmgsdi_jcdma_uicc_info_slot_ptr[MMGSDI_MAX_NUM_SLOTS] = {NULL};

static const uint8 mmgsdi_jcdma_signature[MMGSDI_JCDMA_SIG_SIZE] = {
  0xAF, 0xC4, 0x59, 0xFF,0x53, 0x70, 0xD9, 0x4E,
  0xC0, 0xAF, 0x47, 0xF0,0xBC, 0x07, 0xC1, 0xEB
};


/*===========================================================================
FUNCTION MMGSDI_PROCESS_JCDMA_CARD_GET_INFO_CMD

DESCRIPTION
  This function, called in mgmsdi_process_command.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE:
  MMGSDI_SUCCESS: upon succssesful processing of command.
  MMGSDI_ERROR: upon error

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_process_jcdma_card_get_info_cmd (
  const mmgsdi_jcdma_get_card_info_req_type *req_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;
  int32                              index           = 0;

  /* Parameter checks.*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr->request_header.response_cb);

  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("JCDMA get client request failed w/ status=0x%x",
                  mmgsdi_status);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                    index, &req_ptr->request_header,NULL,NULL);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("JCDMA populate client request failed w/ status=0x%x",
                  mmgsdi_status);
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  /* Get card info */
  mmgsdi_status = mmgsdi_jcdma_card_get_attr(
                    req_ptr->request_header.session_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("JCDMA get card info failed w/ status=0x%x", mmgsdi_status);
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                    index, MMGSDI_JCDMA_GET_CARD_INFO_REQ, MMGSDI_SUCCESS);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }
  return mmgsdi_status;
} /* mmgsdi_process_jcdma_card_get_info_cmd() */


/*===========================================================================
FUNCTION MMGSDI_JCDMA_RSP_GET_CARD_INFO

DESCRIPTION
  This function sends response

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE:
   MMGSDI_SUCCESS: upon successful queueing of the conf.
   MMGSDI_ERROR: upon error

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_jcdma_rsp_get_card_info (
  const mmgsdi_uim_report_rsp_type*   uim_rsp_ptr
)
{
  mmgsdi_request_header_type         req_header;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  uim_cmd_type*                      uim_cmd_ptr      = NULL;
  mmgsdi_client_req_extra_info_type* extra_param_ptr  = NULL;
  mmgsdi_return_enum_type            report_status    = MMGSDI_SUCCESS;

  /*
   * Check input pointers.
   */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(uim_rsp_ptr->rsp_data_index,
                                    0, MMGSDI_MAX_CLIENT_INFO-1);
  /*
   * Get the request header.
   */
  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                    uim_rsp_ptr->rsp_data_index,&req_header,&uim_cmd_ptr,
                                                             &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Get the MMGSDI command processing Status */
  report_status = uim_rsp_ptr->mmgsdi_error_status;

  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                             &req_header,
                                             NULL,
                                             NULL,
                                             FALSE,
                                             uim_rsp_ptr->status_word);

  return mmgsdi_status;
} /* mmgsdi_jcdma_rsp_get_card_info() */


/*===========================================================================
FUNCTION:      MMGSDI_JCDMA_CARD_GET_COLOR_AND_LOCKS

DESCRIPTION:
  This function reads files from card and decides card color. It also reads
  UIM lock indicator to decide if lock checks are needed.
  Files read from card: MMGSDI_CDMA_IMSI_M
                        MMGSDI_CDMA_IMSI_STATUS
                        MMGSDI_CDMA_UIM_VERSION
                        MMGSDI_CDMA_RUIM_ID
                        MMGSDI_CDMA_GID1
                        MMGSDI_CDMA_GID2

DEPENDENCIES:

LIMITATIONS:

RETURN VALUE:
  MMGSDI_SUCCESS:  Upon successfuly parsing read file data
  MMGSDI_ERROR:    Upon error

SIDE EFFECTS:
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_jcdma_card_get_color_and_locks (
  mmgsdi_jcdma_card_attr_type *card_attr_ptr,
  mmgsdi_session_id_type       req_session_id
)
{
  mmgsdi_return_enum_type             mmgsdi_status       = MMGSDI_ERROR;
  mmgsdi_read_req_type               *read_req_ptr        = NULL;
  mmgsdi_read_cnf_type               *read_cnf_ptr        = NULL;
  mmgsdi_protocol_enum_type           protocol            = MMGSDI_NO_PROTOCOL;
  mmgsdi_slot_id_enum_type            slot_id             = MMGSDI_MAX_SLOT_ID_ENUM;
  uint8                               slot_index          = MMGSDI_SLOT_1_INDEX;
  mmgsdi_int_app_info_type           *app_info_ptr        = NULL;
  mmgsdi_jcdma_uicc_info_type        *jcdma_uicc_info_ptr = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(card_attr_ptr);

  /* Initial value for card type */
  card_attr_ptr->card_type = MMGSDI_JCDMA_NON_JCDMA_CARD;

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    req_session_id,&slot_id,NULL,&app_info_ptr,NULL,NULL,NULL);

  if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    UIM_MSG_ERR_2("JCDMA: Get color/lock, No info for session id =0x%x,status=0x%x",
                  req_session_id, mmgsdi_status);
    return MMGSDI_ERROR;
  }

  if (MMGSDI_SUCCESS != mmgsdi_util_get_slot_index(slot_id, &slot_index))
  {
    return MMGSDI_ERROR;
  }

  jcdma_uicc_info_ptr = mmgsdi_jcdma_get_uicc_info_ptr_from_slot(slot_id);
  if(NULL == jcdma_uicc_info_ptr)
  {
    return MMGSDI_ERROR;
  }

  protocol = mmgsdi_util_determine_protocol(slot_id);

  if (protocol == MMGSDI_NO_PROTOCOL)
  {
    UIM_MSG_ERR_1("JCDMA: Get color/lock, Cannot determine protocol for slot=0x%x",
                  slot_id);
    return MMGSDI_ERROR;
  }
  else if (protocol == MMGSDI_UICC)
  {
    if ((jcdma_uicc_info_ptr->jcsim1.attr_avail == TRUE) &&
        (jcdma_uicc_info_ptr->jcsim2.attr_avail == TRUE))
    {
      UIM_MSG_HIGH_1("JCDMA:Copy UICC color/lock info collected at power up for slot=0x%x",
                     slot_id);
      mmgsdi_memscpy(card_attr_ptr,
                     sizeof(card_attr_ptr),
                     &jcdma_uicc_info_ptr->card_attr,
                     sizeof(mmgsdi_jcdma_card_attr_type));
    }
    else
    {
      UIM_MSG_HIGH_3("JCDMA:color/lock unavailable.JCSIM1=0x%x,JCSIM2=0x%x,card=0x%x",
                     jcdma_uicc_info_ptr->jcsim1.attr_avail,
                     jcdma_uicc_info_ptr->jcsim2.attr_avail,
                     jcdma_uicc_info_ptr->card_attr.card_type);
      card_attr_ptr->card_type =
        jcdma_uicc_info_ptr->card_attr.card_type;
    }
    return MMGSDI_SUCCESS;
  }
  else
  {
    /* Free any memory allocated for UICC card attributes as
       we are in ICC mode */
    mmgsdi_jcdma_free(slot_index);
  }

  /* Allocate memory space */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(read_req_ptr,
    sizeof(mmgsdi_read_req_type));
  if (read_req_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*--------------------------------------
    Read JCDMA version and signature EF
  --------------------------------------*/
  read_req_ptr->access.access_method      = MMGSDI_EF_ENUM_ACCESS;
  read_req_ptr->data_len                  = 0; /* read entire file */
  read_req_ptr->rec_num                   = 0;
  read_req_ptr->offset                    = 0;
  read_req_ptr->file_type                 = MMGSDI_TRANSPARENT_FILE;
  read_req_ptr->request_header.session_id = req_session_id;
  read_req_ptr->request_header.slot_id    = slot_id;
  read_req_ptr->access.file.file_enum     = MMGSDI_CDMA_UIM_VERSION;
  mmgsdi_status = mmgsdi_uim_icc_read_transparent(read_req_ptr, TRUE,
                                                  &read_cnf_ptr);
  if(read_cnf_ptr == NULL)
  {
    UIM_MSG_ERR_0("Read Cnf Ptr is NULL");
    MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
    return MMGSDI_ERROR;
  }

  if ( (mmgsdi_status == MMGSDI_SUCCESS) &&
       (read_cnf_ptr->response_header.mmgsdi_status == MMGSDI_SUCCESS))
  {
    if (read_cnf_ptr->read_data.data_ptr == NULL ||
        read_cnf_ptr->read_data.data_len < 18)
    {
      UIM_MSG_ERR_0("JCDMA: Version read data ptr NULL");
      MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_cnf_ptr->read_data.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
      return MMGSDI_ERROR;
    }

    /* Check the version. If the 1st byte of the signature file is set
       to the same value as a test card then the card is marked as
       an unknown card */
    if (MMGSDI_JCDMA_UIM_TEST_VERSION == read_cnf_ptr->read_data.data_ptr[0])
    {
      UIM_MSG_ERR_0("Invalid. JCDMA test card");
      card_attr_ptr->card_type = MMGSDI_JCDMA_UNKNOWN_CARD;
      MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr->read_data.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
      /* JCDMA test card. No need to proceed */
      return MMGSDI_SUCCESS;
    }

    /* Compare the signature which starts from the 2nd byte of version EF */
    if (memcmp (&read_cnf_ptr->read_data.data_ptr[1],
                mmgsdi_jcdma_signature, MMGSDI_JCDMA_SIG_SIZE))
    {
       UIM_MSG_ERR_0("Invalid. JCDMA signature failed");
       card_attr_ptr->card_type = MMGSDI_JCDMA_NON_JCDMA_CARD;
       MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr->read_data.data_ptr);
       MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
       MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
       /* JCDMAsignature mismatch. No need to proceed */
       return MMGSDI_SUCCESS;
    }

    /* Check skip UIM lock bit which is the 1st bit of the 18th byte of the
       version EF */
    if (read_cnf_ptr->read_data.data_ptr[17] & MMGSDI_JCDMA_UIM_LOCK_IND)
    {
       /* Lock is disabled */
       card_attr_ptr->lock_enabled = FALSE;
    }
    else
    {
       /* Lock is enabled */
      card_attr_ptr->lock_enabled = TRUE;
    }
  }
  /* handling if read on MMGSDI_CDMA_UIM_VERSION file failed */
  else
  {
    card_attr_ptr->card_type = MMGSDI_JCDMA_NON_JCDMA_CARD;
    UIM_MSG_ERR_0("Invalid EF. JCDMA signature EF read failed");
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_cnf_ptr->read_data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
    /* Failed to read JCDMA signature EF */
    return MMGSDI_SUCCESS;
  }

  MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr->read_data.data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);

  /*--------------------------------------
   It is JCDMA card. Read card color
  --------------------------------------*/
  read_req_ptr->access.file.file_enum = MMGSDI_CDMA_IMSI_M;
  mmgsdi_status = mmgsdi_uim_icc_read_transparent(read_req_ptr, TRUE,
                                                  &read_cnf_ptr);
  if(read_cnf_ptr == NULL)
  {
    UIM_MSG_ERR_0("Read Cnf Ptr is NULL");
    MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
    return MMGSDI_ERROR;
  }

  if ( (mmgsdi_status != MMGSDI_SUCCESS) ||
       (read_cnf_ptr->response_header.mmgsdi_status != MMGSDI_SUCCESS) ||
       (read_cnf_ptr->read_data.data_ptr == NULL) ||
       (read_cnf_ptr->read_data.data_len < 10))
  {
    UIM_MSG_ERR_1("Invalid. JCDMA imsi_m EF read failed w/ status=0x%x",
                  read_cnf_ptr->response_header.mmgsdi_status);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_cnf_ptr->read_data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
    return MMGSDI_ERROR;
  }

  /* Check the 8th bit of byte 8 of the IMSI_M EF to determine if IMSI_M
     is programmed or not */
  if( (read_cnf_ptr->read_data.data_ptr[7] &
       MMGSDI_JCDMA_UIM_IMSI_PROGRAMMED) == 0)
  {
    card_attr_ptr->card_type = MMGSDI_JCDMA_WHITE_CARD;
    UIM_MSG_HIGH_0("JCDMA white card");
    MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr->read_data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
    /* JCDMA white card. */
    return MMGSDI_SUCCESS;
  }

  /* Get NW lock code, i.e MCC, byte 9 and byte 10 from IMSI_M */
  card_attr_ptr->imsi_mcc = (read_cnf_ptr->read_data.data_ptr[9] <<
                             MMGSDI_JCDMA_8BITS) |
                             read_cnf_ptr->read_data.data_ptr[8];

  MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr->read_data.data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);

  /* read JCDMA IMSI STATUS for card color */
  read_req_ptr->access.file.file_enum = MMGSDI_CDMA_IMSI_STATUS;
  mmgsdi_status = mmgsdi_uim_icc_read_transparent(read_req_ptr, TRUE,
                                                  &read_cnf_ptr);
  if(read_cnf_ptr == NULL)
  {
    UIM_MSG_ERR_0("Read Cnf Ptr is NULL");
    MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
    return MMGSDI_ERROR;
  }

  if ( (mmgsdi_status != MMGSDI_SUCCESS) ||
       (read_cnf_ptr->response_header.mmgsdi_status != MMGSDI_SUCCESS) ||
       (read_cnf_ptr->read_data.data_ptr == NULL) ||
       (read_cnf_ptr->read_data.data_len < 1))
  {
    UIM_MSG_ERR_1("Invalid. JCDMA imsi status EF read failed w/ status=0x%x",
                  read_cnf_ptr->response_header.mmgsdi_status);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_cnf_ptr->read_data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
    return MMGSDI_ERROR;
  }

  /* Check if byte 1 of the IMSI_STATUS EF is set to 0 or 1 to determine
     the color */
  if( (read_cnf_ptr->read_data.data_ptr[0] & MMGSDI_JCDMA_UIM_IMSI_STATUS)
       == 0)
  {
    /* JCDMA black card. */
    card_attr_ptr->card_type = MMGSDI_JCDMA_BLACK_CARD;
    UIM_MSG_HIGH_0("JCDMA black card");
  }
  else
  {
    /* JCDMA gray card. */
    card_attr_ptr->card_type = MMGSDI_JCDMA_GRAY_CARD;
    UIM_MSG_HIGH_0("JCDMA gray card");
  }

  MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr->read_data.data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);

  /*--------------------------------------
   Read Lock codes from card
  --------------------------------------*/
  if (card_attr_ptr->lock_enabled)
  {
    /* read UIM ID for line lock */
    read_req_ptr->access.file.file_enum           = MMGSDI_CDMA_RUIM_ID;
    mmgsdi_status = mmgsdi_uim_icc_read_transparent(read_req_ptr, TRUE,
                                                    &read_cnf_ptr);
    if(read_cnf_ptr == NULL)
    {
      UIM_MSG_ERR_0("Read Cnf Ptr is NULL");
      MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
      return MMGSDI_ERROR;
    }

    if ( (mmgsdi_status == MMGSDI_SUCCESS) &&
         (read_cnf_ptr->response_header.mmgsdi_status == MMGSDI_SUCCESS))
    {
      if (read_cnf_ptr->read_data.data_ptr == NULL ||
          read_cnf_ptr->read_data.data_len < 2 ||
          read_cnf_ptr->read_data.data_ptr[0] > (read_cnf_ptr->read_data.data_len - 1))
      {
        UIM_MSG_ERR_0("JCDMA: RUIMID read data ptr NULL");
        MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_cnf_ptr->read_data.data_ptr);
        MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
        return MMGSDI_ERROR;
      }

      /* Copy the RUIM ID which starts from byte 2 onwards with
         the size listed in byte 1 */
      mmgsdi_memscpy(&card_attr_ptr->ruim_id,
                     sizeof(card_attr_ptr->ruim_id),
                     &read_cnf_ptr->read_data.data_ptr[1],
                     read_cnf_ptr->read_data.data_ptr[0]);
    }
    else
    {
      /* Failed to read RUIM ID. Set to defalt */
      card_attr_ptr->ruim_id = 0xFFFFFFFF;
      UIM_MSG_ERR_1("JCDMA ruim id EF read failed w/ status=0x%x",
                    read_cnf_ptr->response_header.mmgsdi_status);
    }

    MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr->read_data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);

    /* read GID1 for SP lock     */
    read_req_ptr->access.file.file_enum = MMGSDI_CDMA_GID1;
    mmgsdi_status = mmgsdi_uim_icc_read_transparent(read_req_ptr,TRUE,
                                                    &read_cnf_ptr);
    if(read_cnf_ptr == NULL)
    {
      UIM_MSG_ERR_0("Read Cnf Ptr is NULL");
      MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
      return MMGSDI_ERROR;
    }

    if ( (mmgsdi_status == MMGSDI_SUCCESS) &&
         (read_cnf_ptr->response_header.mmgsdi_status == MMGSDI_SUCCESS))
    {
       if (read_cnf_ptr->read_data.data_ptr == NULL ||
           read_cnf_ptr->read_data.data_len < 1)
       {
         UIM_MSG_ERR_0("JCDMA: GID1 read data ptr NULL");
         MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
         MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_cnf_ptr->read_data.data_ptr);
         MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
         return MMGSDI_ERROR;
       }

       /* Copy the SP Id from byte 1 of the GID1 EF contents */
       card_attr_ptr->sp_id = read_cnf_ptr->read_data.data_ptr[0];
    }
    else
    {
      /* Failed to read SP code. Set to defalt */
      card_attr_ptr->sp_id = MMGSDI_JCDMA_UIM_SP_UG_DEFAULT;
      UIM_MSG_ERR_1("JCDMA gid1 EF read failed w/ status=0x%x",
                    read_cnf_ptr->response_header.mmgsdi_status);
    }

    MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr->read_data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);

    /* read GID2 for UG lock */
    read_req_ptr->access.file.file_enum = MMGSDI_CDMA_GID2;
    mmgsdi_status = mmgsdi_uim_icc_read_transparent(read_req_ptr,TRUE,
                                                    &read_cnf_ptr);
    if(read_cnf_ptr == NULL)
    {
      UIM_MSG_ERR_0("Read Cnf Ptr is NULL");
      MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
      return MMGSDI_ERROR;
    }

    if ( (mmgsdi_status == MMGSDI_SUCCESS) &&
         (read_cnf_ptr->response_header.mmgsdi_status == MMGSDI_SUCCESS))
    {
      if (read_cnf_ptr->read_data.data_ptr == NULL ||
          read_cnf_ptr->read_data.data_len < 1)
      {
        UIM_MSG_ERR_0("JCDMA: GID2 read data ptr NULL");
        MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_cnf_ptr->read_data.data_ptr);
        MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
        return MMGSDI_ERROR;
      }

      /* Copy the UG Id from byte 1 of the GID2 EF contents */
      card_attr_ptr->ug_id = read_cnf_ptr->read_data.data_ptr[0];
    }
    else
    {
      /* Failed to read UG code. Set to defalt */
      card_attr_ptr->ug_id = MMGSDI_JCDMA_UIM_SP_UG_DEFAULT;
      UIM_MSG_ERR_1("JCDMA gid2 EF read failed w/ status=0x%x",
                    read_cnf_ptr->response_header.mmgsdi_status);
    }

    MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr->read_data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
  }  /* if(card_attr_ptr->lock_enabled) */

  MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
  return MMGSDI_SUCCESS;
} /* mmgsdi_jcdma_card_get_color_and_locks() */


/*===========================================================================
FUNCTION:      MMGSDI_JCDMA_CARD_GET_ATTR

DESCRIPTION:
  This function checks card color (white/gray/black), UIM lock staus, and card
  insertion history.

DEPENDENCIES:
   PIN1 verification is done before calling this function.

LIMITATIONS:

RETURN VALUE:
  MMGSDI_SUCCESS: If succssesful in getting card attributes
  MMGSDI_ERROR : If a error in getting card attributes

SIDE EFFECTS:
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_jcdma_card_get_attr (
  mmgsdi_session_id_type       session_id
)
{
  mmgsdi_jcdma_card_attr_type   *card_data_ptr;
  nv_item_type                   lock_nv_data;
  mmgsdi_nv_item_cache_type      mmgsdi_lock_nv_data    = {0};
  nv_item_type                   history_nv_data;
  mmgsdi_nv_item_cache_type      mmgsdi_history_nv_data = {0};
  nv_stat_enum_type              nv_read_status;
  nv_stat_enum_type              nv_write_status;
  mmgsdi_slot_id_enum_type       slot_id           = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_slot_data_type         *slot_data_ptr     = NULL;
  mmgsdi_nv_context_type         nv_context        = MMGSDI_NV_CONTEXT_INVALID;

  /* Get slot_id from session_id in order to get the corresponding slot
     data ptr */
  if(MMGSDI_SUCCESS != mmgsdi_util_get_session_app_info(
       session_id,&slot_id,NULL,NULL,NULL,NULL,NULL))
  {
    return MMGSDI_ERROR;
  }
  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);
  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* Allocate memory */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(card_data_ptr,
    sizeof(mmgsdi_jcdma_card_attr_type));
  if (card_data_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /* Initialized card_info to unknown */
  slot_data_ptr->jcdma_card_info = MMGSDI_JCDMA_UNKNOWN_CARD;

  /*-----------------------------------------------------
    Read card color and lock info from card
  -----------------------------------------------------*/
  if(MMGSDI_SUCCESS !=
       mmgsdi_jcdma_card_get_color_and_locks(card_data_ptr,session_id))
  {
     MMGSDIUTIL_TMC_MEM_FREE(card_data_ptr);
     UIM_MSG_ERR_0("JCDMA get card color failed");
     return MMGSDI_ERROR;
  }

  /*-----------------------------------------------------
    Read lock info from NV
  -----------------------------------------------------*/
  nv_context = mmgsdi_util_get_efs_item_index_for_slot(slot_id);

  nv_read_status =  mmgsdi_nv_get_item(NV_JCDMA_UIM_LOCK_I,
                                       &mmgsdi_lock_nv_data,
                                       nv_context);
  if(nv_read_status == NV_DONE_S)
  {
     UIM_MSG_HIGH_0("NV_JCDMA_UIM_LOCK_I read ");
  }
  else if (nv_read_status == NV_NOTACTIVE_S)
  {
    /* Write Default Value */

    lock_nv_data.jcdma_uim_lock.ruim_id   = MMGSDI_JCDMA_RUIMID_DEFAULT;
    lock_nv_data.jcdma_uim_lock.imsi_mcc1 = MMGSDI_JCDMA_IMSI_MCC1_DEFAULT;
    lock_nv_data.jcdma_uim_lock.imsi_mcc2 = MMGSDI_JCDMA_IMSI_MCC2_DEFAULT;
    lock_nv_data.jcdma_uim_lock.sp_id = MMGSDI_JCDMA_SP_ID_DEFAULT;
    lock_nv_data.jcdma_uim_lock.ug_id = MMGSDI_JCDMA_UG_ID_DEFAULT;
    lock_nv_data.jcdma_uim_lock.indicator[MMGSDI_JCDMA_LINE_LOCK] = MMGSDI_JCDMA_INDICATOR_DEFAULT;
    lock_nv_data.jcdma_uim_lock.indicator[MMGSDI_JCDMA_NW_LOCK]   = MMGSDI_JCDMA_INDICATOR_DEFAULT;
    lock_nv_data.jcdma_uim_lock.indicator[MMGSDI_JCDMA_SP_LOCK]   = MMGSDI_JCDMA_INDICATOR_DEFAULT;
    lock_nv_data.jcdma_uim_lock.indicator[MMGSDI_JCDMA_UG_LOCK]   = MMGSDI_JCDMA_INDICATOR_DEFAULT;

    nv_write_status =  mmgsdi_nv_put_item(NV_JCDMA_UIM_LOCK_I,
                                          (nv_item_type *) &lock_nv_data, nv_context);
    if (nv_write_status != NV_DONE_S)
    {
      MMGSDIUTIL_TMC_MEM_FREE(card_data_ptr);
      UIM_MSG_ERR_1("NV NV_JCDMA_UIM_LOCK_I Write Error, status returned 0x%x",
                    nv_write_status);
      return MMGSDI_ERROR;
    }
    (void)mmgsdi_nv_get_item(NV_JCDMA_UIM_LOCK_I,
                             &mmgsdi_lock_nv_data,
                             nv_context);

  }
  else
  {
    MMGSDIUTIL_TMC_MEM_FREE(card_data_ptr);
    UIM_MSG_ERR_0("NV read error");
    return MMGSDI_ERROR;
  }

  /* copy into lock_nv_data for writing into nv further */
  mmgsdi_memscpy(&(lock_nv_data.jcdma_uim_lock),sizeof(nv_jcdma_uim_lock_data),
                 &(mmgsdi_lock_nv_data.item_value).jcdma_uim_lock,sizeof(nv_jcdma_uim_lock_data));

  /*-----------------------------------------------------
     Read insertion history info from NV
  -----------------------------------------------------*/
  if(NV_DONE_S !=
     mmgsdi_nv_get_item(NV_JCDMA_RUIM_ID_I, &mmgsdi_history_nv_data, nv_context))
  {
    MMGSDIUTIL_TMC_MEM_FREE(card_data_ptr);
    UIM_MSG_ERR_0("NV read error");
    return MMGSDI_ERROR;
  }
  else
  {
    UIM_MSG_HIGH_0("NV_JCDMA_RUIM_ID_I read ");
  }

  /* copy into history_nv_data for writing into nv further */
  mmgsdi_memscpy(&(history_nv_data.jcdma_ruim_id),sizeof(nv_jcdma_ruimid),
                 &(mmgsdi_history_nv_data.item_value.jcdma_ruim_id),sizeof(nv_jcdma_ruimid));

  /*-----------------------------------------------------
    Parse the data from card
  -----------------------------------------------------*/
  slot_data_ptr->jcdma_card_info = card_data_ptr->card_type;
  switch (card_data_ptr->card_type) {
    case MMGSDI_JCDMA_BLACK_CARD:
      if (card_data_ptr->lock_enabled)
      {
        /* Check line lock enabled */
        if (mmgsdi_lock_nv_data.item_value.jcdma_uim_lock.indicator[MMGSDI_JCDMA_LINE_LOCK])
        {
          /* Compare UIM ID */
          if ((mmgsdi_lock_nv_data.item_value.jcdma_uim_lock.ruim_id != card_data_ptr->ruim_id) &&
              (mmgsdi_lock_nv_data.item_value.jcdma_uim_lock.ruim_id != MMGSDI_JCDMA_UIMID_DEFAULT))
          {
             slot_data_ptr->jcdma_card_info = MMGSDI_JCDMA_BLACK_CARD_LINE_LOCKED;
             break;
          }
        }
        /* Check NW lock  */
        if (mmgsdi_lock_nv_data.item_value.jcdma_uim_lock.indicator[MMGSDI_JCDMA_NW_LOCK])
        {
          /* Compare NW */
          if ((mmgsdi_lock_nv_data.item_value.jcdma_uim_lock.imsi_mcc1 != card_data_ptr->imsi_mcc) &&
              (mmgsdi_lock_nv_data.item_value.jcdma_uim_lock.imsi_mcc2 != card_data_ptr->imsi_mcc))
          {
            slot_data_ptr->jcdma_card_info = MMGSDI_JCDMA_BLACK_CARD_NW_LOCKED;
            break;
          }
        }
        /* Check SP lock  */
        if (mmgsdi_lock_nv_data.item_value.jcdma_uim_lock.indicator[MMGSDI_JCDMA_SP_LOCK])
        {
          /* Compare SP */
          if (mmgsdi_lock_nv_data.item_value.jcdma_uim_lock.sp_id != card_data_ptr->sp_id)
          {
            slot_data_ptr->jcdma_card_info = MMGSDI_JCDMA_BLACK_CARD_SP_LOCKED;
            break;
          }
        }
        /* Check UG lock  */
        if (mmgsdi_lock_nv_data.item_value.jcdma_uim_lock.indicator[MMGSDI_JCDMA_UG_LOCK])
        {
          /* Compare UG */
          if (mmgsdi_lock_nv_data.item_value.jcdma_uim_lock.ug_id!= card_data_ptr->ug_id)
          {
            slot_data_ptr->jcdma_card_info = MMGSDI_JCDMA_BLACK_CARD_UG_LOCKED;
            break;
          }
        }
      }
      break;
    case MMGSDI_JCDMA_GRAY_CARD:
    case MMGSDI_JCDMA_GRAY_BLACK_CARD:
      if (card_data_ptr->lock_enabled)
      {
        /* Only need to check line lock for gray card */
        if (mmgsdi_lock_nv_data.item_value.jcdma_uim_lock.indicator[MMGSDI_JCDMA_LINE_LOCK])
        {
          /* Compare UIM ID and see if NV is initialized */
          if ((mmgsdi_lock_nv_data.item_value.jcdma_uim_lock.ruim_id != card_data_ptr->ruim_id) &&
              (mmgsdi_lock_nv_data.item_value.jcdma_uim_lock.ruim_id != MMGSDI_JCDMA_UIMID_DEFAULT))
          {
              /* Only lock ME when ruim id does not match and NV is not
                 initialized */
          if (card_data_ptr->card_type == MMGSDI_JCDMA_GRAY_CARD)
          {
            slot_data_ptr->jcdma_card_info =
              MMGSDI_JCDMA_GRAY_CARD_LINE_LOCKED;
          }
          else
          {
            slot_data_ptr->jcdma_card_info =
              MMGSDI_JCDMA_GRAY_BLACK_CARD_LINE_LOCKED;
          }
          break;
          }
        }
      }
      break;
    default:
      break;
  } /* switch */

  /*--------------------------------------------------------------
   Check line lock code initialization and card insertion history
  --------------------------------------------------------------*/

  if (slot_data_ptr->jcdma_card_info == MMGSDI_JCDMA_BLACK_CARD)
  {
     /* NV line lock code initialized? */
     if (mmgsdi_lock_nv_data.item_value.jcdma_uim_lock.ruim_id == MMGSDI_JCDMA_UIMID_DEFAULT)
     {
        /* This is first black card for this ME */
        slot_data_ptr->jcdma_card_info = MMGSDI_JCDMA_BLACK_CARD_NEW_CARD;
        lock_nv_data.jcdma_uim_lock.ruim_id = card_data_ptr->ruim_id;
        UIM_MSG_HIGH_0("JCDMA New card ");
        if(NV_DONE_S !=
          mmgsdi_nv_put_item(NV_JCDMA_UIM_LOCK_I, (nv_item_type *) &lock_nv_data, nv_context))
        {
          MMGSDIUTIL_TMC_MEM_FREE(card_data_ptr);
          UIM_MSG_ERR_0("NV write error");
          return MMGSDI_ERROR;
        }
        else
        {
          UIM_MSG_HIGH_0("NV_JCDMA_RUIM_ID_I write ");
        }
     }

     /* Compare the UIM ID with history data */
     if (mmgsdi_history_nv_data.item_value.jcdma_ruim_id.ruim_id != card_data_ptr->ruim_id)
     {
        /* it is different card */
        slot_data_ptr->jcdma_card_info = MMGSDI_JCDMA_BLACK_CARD_NEW_CARD;
        /* update the history data to NV */
        history_nv_data.jcdma_ruim_id.ruim_id = card_data_ptr->ruim_id;
        UIM_MSG_HIGH_0("JCDMA New card ");
        if(NV_DONE_S !=
          mmgsdi_nv_put_item(NV_JCDMA_RUIM_ID_I, (nv_item_type *) &history_nv_data, nv_context))
        {
          MMGSDIUTIL_TMC_MEM_FREE(card_data_ptr);
          UIM_MSG_ERR_0("NV write error");
          return MMGSDI_ERROR;
        }
        else
        {
          UIM_MSG_HIGH_0("NV_JCDMA_RUIM_ID_I write ");
        }
     }
  }
  /* Release memory */
  MMGSDIUTIL_TMC_MEM_FREE(card_data_ptr);
  return MMGSDI_SUCCESS;
} /* mmgsdi_jcdma_card_get_attr() */

/* ============================================================================
FUNCTION MMGSDI_JCDMA_GET_UICC_INFO_PTR_FROM_SLOT

DESCRIPTION
  This function retrieves the appropriate jcdma_uicc_info_ptr based on the
  slot id

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_jcdma_uicc_info_slot1_ptr or mmgsdi_jcdma_uicc_info_slot2_ptr or NULL if it is
  invalid slot id

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_jcdma_uicc_info_type* mmgsdi_jcdma_get_uicc_info_ptr_from_slot(
  mmgsdi_slot_id_enum_type mmgsdi_slot)
{
  switch(mmgsdi_slot)
  {
    case MMGSDI_SLOT_1:
      return mmgsdi_jcdma_uicc_info_slot_ptr[0];
    case MMGSDI_SLOT_2:
      return mmgsdi_jcdma_uicc_info_slot_ptr[1];
    case MMGSDI_SLOT_3:
      return mmgsdi_jcdma_uicc_info_slot_ptr[2];
    default:
      UIM_MSG_ERR_1("Invalid mmgsdi slot 0x%x", mmgsdi_slot);
      return NULL;
  }
} /* mmgsdi_jcdma_get_uicc_info_ptr_from_slot*/

/*===========================================================================
FUNCTION MMGSDI_JCDMA_OPEN_NON_PROV_SESSION

DESCRIPTION
  This function openss non provisioning sessions to the JCDMA CSIM Apps

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE:
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_jcdma_open_non_prov_session (
  const mmgsdi_app_info_type*      app_info_ptr
)
{
  mmgsdi_non_prov_app_info_type   jcdma_app_data;
  mmgsdi_return_enum_type         mmgsdi_status       = MMGSDI_ERROR;
  mmgsdi_jcdma_uicc_info_type    *jcdma_uicc_info_ptr = NULL;
  mmgsdi_session_type_enum_type   session_id_type;
  mmgsdi_slot_id_enum_type        slot_id             = MMGSDI_MAX_SLOT_ID_ENUM;

  if (app_info_ptr->app_data.app_type != MMGSDI_APP_CSIM)
  {
    /* Open the non-prov sessions to the JCSIM1 and JCSIM2 apps only
       after the 1st CSIM app is selected */
    return;
  }

  slot_id = app_info_ptr->slot;
  switch(slot_id)
  {
    case MMGSDI_SLOT_1:
      session_id_type = MMGSDI_NON_PROV_SESSION_SLOT_1;
      break;
    case MMGSDI_SLOT_2:
      session_id_type = MMGSDI_NON_PROV_SESSION_SLOT_2;
      break;
    case MMGSDI_SLOT_3:
      session_id_type = MMGSDI_NON_PROV_SESSION_SLOT_3;
      break;
    default:
      /* Invalid mmgsdi slot id*/
      return;
  }

  jcdma_uicc_info_ptr =
    mmgsdi_jcdma_get_uicc_info_ptr_from_slot(slot_id);
  if(jcdma_uicc_info_ptr == NULL)
  {
    return;
  }

  if ((jcdma_uicc_info_ptr->jcsim1.attr_avail == TRUE) &&
      (jcdma_uicc_info_ptr->jcsim2.attr_avail == TRUE))
  {
    /* Return if attributes have already been read */
    return;
  }

  if ((jcdma_uicc_info_ptr->jcsim1.session_opened != TRUE) &&
      (jcdma_uicc_info_ptr->jcsim1.aid.data_ptr != NULL))
  {
    memset(&jcdma_app_data,0,sizeof(mmgsdi_non_prov_app_info_type));
    UIM_MSG_HIGH_1("JCDMA opening non prov session to JCSIM1 app on slot=0x%x",
                   slot_id);
    jcdma_app_data.app_id_data.data_len =
      jcdma_uicc_info_ptr->jcsim1.aid.data_len;
    jcdma_app_data.app_id_data.data_ptr =
      jcdma_uicc_info_ptr->jcsim1.aid.data_ptr;
    jcdma_app_data.exclusive_channel    = FALSE;
    mmgsdi_status = mmgsdi_session_open(mmgsdi_generic_data_ptr->client_id,
                      session_id_type,&jcdma_app_data,
                      mmgsdi_jcdma_evt_cb,FALSE,
                      mmgsdi_jcdma_session_open_cb,
                      (mmgsdi_client_data_type)MMGSDI_JCDMA_JCSIM1_ID);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_2("JCSIM1 non-prov session open failed on slot=0x%x.status 0x%x",
                    slot_id, mmgsdi_status);
    }
    else
    {
      jcdma_uicc_info_ptr->jcsim1.session_opened = TRUE;
    }
  }

  if ((jcdma_uicc_info_ptr->jcsim2.session_opened != TRUE) &&
      (jcdma_uicc_info_ptr->jcsim2.aid.data_ptr != NULL))
  {
    memset(&jcdma_app_data,0,sizeof(mmgsdi_non_prov_app_info_type));
    UIM_MSG_HIGH_1("JCDMA opening non prov session to JCSIM2 app on slot=0x%x",
                   slot_id);
    jcdma_app_data.app_id_data.data_len =
      jcdma_uicc_info_ptr->jcsim2.aid.data_len;
    jcdma_app_data.app_id_data.data_ptr =
      jcdma_uicc_info_ptr->jcsim2.aid.data_ptr;
    jcdma_app_data.exclusive_channel    = FALSE;
    mmgsdi_status = mmgsdi_session_open(mmgsdi_generic_data_ptr->client_id,
                      session_id_type,&jcdma_app_data,
                      mmgsdi_jcdma_evt_cb,FALSE,
                      mmgsdi_jcdma_session_open_cb,
                      (mmgsdi_client_data_type)MMGSDI_JCDMA_JCSIM2_ID);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_2("JCSIM2 non-prov session open failed on slot=0x%x.status 0x%x",
                    slot_id, mmgsdi_status);
    }
    else
    {
      jcdma_uicc_info_ptr->jcsim2.session_opened = TRUE;
    }
  }
} /* mmgsdi_jcdma_open_non_prov_session */


/*===========================================================================

FUNCTION MMGSDI_JCDMA_SESSION_OPEN_CB

DESCRIPTION
  This function will be called by MMGSDI when the requeseted JCDMA sessions are
  open and a session id is available

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void mmgsdi_jcdma_session_open_cb (
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
)
{
  mmgsdi_jcdma_uicc_info_type *jcdma_uicc_info_ptr = NULL;
  mmgsdi_slot_id_enum_type     slot_id             = MMGSDI_MAX_SLOT_ID_ENUM;

  /* Get slot_id from session_id in order to get the corresponding
     mmgsdi_jcdma_uicc_info_slot1_ptr */
  if(MMGSDI_SUCCESS != mmgsdi_util_get_session_app_info(
       cnf_ptr->session_open_cnf.session_id,&slot_id,NULL,NULL,NULL,NULL,NULL))
  {
    return;
  }
  jcdma_uicc_info_ptr =
    mmgsdi_jcdma_get_uicc_info_ptr_from_slot(slot_id);
  if(jcdma_uicc_info_ptr == NULL)
  {
    return;
  }

  if (cnf_ptr->session_open_cnf.response_header.client_data ==
      (mmgsdi_client_data_type)MMGSDI_JCDMA_JCSIM1_ID)
  {
    UIM_MSG_HIGH_2("JCSIM1 non-prov session open for slot: 0x%x, status: 0x%x",
                   slot_id,
                   cnf_ptr->session_open_cnf.response_header.mmgsdi_status);
    if (cnf_ptr->session_open_cnf.response_header.mmgsdi_status ==
       MMGSDI_SUCCESS)
    {
       jcdma_uicc_info_ptr->jcsim1.session_id =
         cnf_ptr->session_open_cnf.session_id;
    }
  }

  if (cnf_ptr->session_open_cnf.response_header.client_data ==
      (mmgsdi_client_data_type)MMGSDI_JCDMA_JCSIM2_ID)
  {
    UIM_MSG_HIGH_2("JCSIM2 non-prov session open for slot: 0x%x, status: 0x%x",
                   slot_id,
                   cnf_ptr->session_open_cnf.response_header.mmgsdi_status);
    if (cnf_ptr->session_open_cnf.response_header.mmgsdi_status ==
       MMGSDI_SUCCESS)
    {
       jcdma_uicc_info_ptr->jcsim2.session_id =
         cnf_ptr->session_open_cnf.session_id;
    }
  }
} /* mmgsdi_jcdma_session_open_cb */


/*===========================================================================

FUNCTION MMGSDI_JCDMA_SESSION_CLOSE_CB

DESCRIPTION
  This function will be called by MMGSDI when the JCDMA sessions are closed

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void mmgsdi_jcdma_session_close_cb (
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
)
{
  mmgsdi_jcdma_uicc_info_type *jcdma_uicc_info_ptr = NULL;
  mmgsdi_slot_id_enum_type     slot_id             = MMGSDI_MAX_SLOT_ID_ENUM;

  /* Get slot_id from the response header in order to get the corresponding
     mmgsdi_jcdma_uicc_info_ptr */
  slot_id = (mmgsdi_slot_id_enum_type)
             cnf_ptr->session_close_cnf.response_header.slot_id;
  jcdma_uicc_info_ptr =
    mmgsdi_jcdma_get_uicc_info_ptr_from_slot(slot_id);
  if(jcdma_uicc_info_ptr == NULL)
  {
    return;
  }

  if (cnf_ptr->session_close_cnf.response_header.client_data ==
      (mmgsdi_client_data_type)MMGSDI_JCDMA_JCSIM1_ID)
  {
    UIM_MSG_HIGH_2("JCSIM1 non-prov session close on slot: 0x%x, status: 0x%x",
                   slot_id,
                   cnf_ptr->session_close_cnf.response_header.mmgsdi_status);
    if (cnf_ptr->session_close_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS)
    {
      jcdma_uicc_info_ptr->jcsim1.session_id = 0x0;
    }
  }

  if (cnf_ptr->session_close_cnf.response_header.client_data ==
      (mmgsdi_client_data_type)MMGSDI_JCDMA_JCSIM2_ID)
  {
     UIM_MSG_HIGH_2("JCSIM2 non-prov session close on slot: 0x%x status: 0x%x",
                    slot_id,
                    cnf_ptr->session_close_cnf.response_header.mmgsdi_status);
    if (cnf_ptr->session_close_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS)
    {
      jcdma_uicc_info_ptr->jcsim2.session_id = 0x0;
    }
  }
} /* mmgsdi_jcdma_session_close_cb */


/*===========================================================================

FUNCTION MMGSDI_JCDMA_EVT_CB

DESCRIPTION
  This function will be called when events related to the JCDMA session occur

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void mmgsdi_jcdma_evt_cb (
  const mmgsdi_event_data_type *event_ptr
)
{
  mmgsdi_jcdma_uicc_info_type *jcdma_uicc_info_ptr = NULL;
  mmgsdi_slot_id_enum_type     slot_id             = MMGSDI_MAX_SLOT_ID_ENUM;

  /* Get slot_id from session_id in order to get the corresponding
     mmgsdi_jcdma_uicc_info_slot1_ptr */
  if(MMGSDI_SUCCESS != mmgsdi_util_get_session_app_info(
       event_ptr->session_id,&slot_id,NULL,NULL,NULL,NULL,NULL))
  {
    return;
  }
  jcdma_uicc_info_ptr =
    mmgsdi_jcdma_get_uicc_info_ptr_from_slot(slot_id);
  if(jcdma_uicc_info_ptr == NULL)
  {
    return;
  }

  if ((event_ptr->evt == MMGSDI_SESSION_CLOSE_EVT) &&
      (event_ptr->session_id == jcdma_uicc_info_ptr->jcsim1.session_id))
  {
    UIM_MSG_HIGH_1("JCDMA: Session close event for JCSIM1 non-prov session on slot=0x%x",
                   slot_id);
    jcdma_uicc_info_ptr->jcsim1.session_id     = 0x0;
  }
  else if ((event_ptr->evt == MMGSDI_SESSION_CLOSE_EVT) &&
           (event_ptr->session_id ==
              jcdma_uicc_info_ptr->jcsim2.session_id))
  {
    UIM_MSG_HIGH_1("JCDMA: Session close event for JCSIM2 non-prov session on slot=0x%x",
                   slot_id);
    jcdma_uicc_info_ptr->jcsim2.session_id     = 0x0;
  }
} /* mmgsdi_jcdma_evt_cb */


/*===========================================================================
FUNCTION MMGSDI_JCDMA_FIND_CSIM_APP_FOR_COLOR_AND_LOCK

DESCRIPTION
  This function determines if a CSIM APP is present to be used for
  color and lock status check

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_jcdma_find_csim_app_for_color_and_lock (
  const mmgsdi_read_cnf_type  *read_cnf_ptr
)
{
  uint8    app_search_count;
  mmgsdi_jcdma_uicc_info_type *jcdma_uicc_info_ptr = NULL;
  mmgsdi_slot_id_enum_type     slot_id             = MMGSDI_MAX_SLOT_ID_ENUM;

  MMGSDIUTIL_RETURN_IF_NULL(read_cnf_ptr);

  slot_id = read_cnf_ptr->response_header.slot_id;
  jcdma_uicc_info_ptr =
    mmgsdi_jcdma_get_uicc_info_ptr_from_slot(slot_id);

  MMGSDIUTIL_RETURN_IF_NULL(jcdma_uicc_info_ptr);

  if ((jcdma_uicc_info_ptr->jcsim1.aid.data_ptr != NULL) &&
      (jcdma_uicc_info_ptr->jcsim2.aid.data_ptr != NULL))
  {
    /* JCSIM1 and JCSIM2 AIDs have been found so no further processing */
    return MMGSDI_SUCCESS;
  }

  /* Get a handle on the Discrentionary data segment that holds the App type in
     the following way:
     + 2 bytes for size of Template Tag and Length
     + 2 bytes for AID Tag and Length
     + Size of AID
     + 1 byte for size of Label Tag */
  app_search_count =  2*(MMGSDI_JCDMA_TAG_SIZE + MMGSDI_JCDMA_LEN_SIZE) +
    read_cnf_ptr->read_data.data_ptr[MMGSDI_JCDMA_AID_LEN_INDEX] + MMGSDI_JCDMA_TAG_SIZE;

  if (app_search_count >= read_cnf_ptr->read_data.data_len)
  {
    return MMGSDI_ERROR;
  }

  /* Once we have a pointer to the Label Length then we determine location of
     app type byte as follows:
     + 1 byte for size of Label Length
     + Size of Label */
  app_search_count += (MMGSDI_JCDMA_LEN_SIZE +
                        read_cnf_ptr->read_data.data_ptr[app_search_count]);

  /* Ensure Discretionary Data Tag is good */
  if ((app_search_count >= read_cnf_ptr->read_data.data_len) ||
      (read_cnf_ptr->read_data.data_ptr[app_search_count]
       != MMGSDI_JCDMA_CSIM_DISC_DATA_TAG))
  {
    return MMGSDI_ERROR;
  }

  /* Move past the Discretionary tag and length byte */
  app_search_count +=  (MMGSDI_JCDMA_TAG_SIZE + MMGSDI_JCDMA_LEN_SIZE);

  /* Check 1st byte of the discretionary data value to determine app type */
  if ((app_search_count < read_cnf_ptr->read_data.data_len) &&
      (read_cnf_ptr->read_data.data_ptr[app_search_count] ==
        MMGSDI_JCDMA_JCSIM1_ID))
  {
    /* If JCSIM1 primary CSIM App is found copy over the AID whose
       length is stored in the 4th byte of the App entry and whose
       value is stored from the 5th byte onwards */
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      jcdma_uicc_info_ptr->jcsim1.aid.data_ptr,
      read_cnf_ptr->read_data.data_ptr[MMGSDI_JCDMA_AID_LEN_INDEX]);
    if (jcdma_uicc_info_ptr->jcsim1.aid.data_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    jcdma_uicc_info_ptr->jcsim1.aid.data_len =
     read_cnf_ptr->read_data.data_ptr[MMGSDI_JCDMA_AID_LEN_INDEX];

    mmgsdi_memscpy(jcdma_uicc_info_ptr->jcsim1.aid.data_ptr,
                   jcdma_uicc_info_ptr->jcsim1.aid.data_len,
                   &read_cnf_ptr->read_data.data_ptr[MMGSDI_JCDMA_AID_START_INDEX],
                   int32touint32(jcdma_uicc_info_ptr->jcsim1.aid.data_len));
    return MMGSDI_SUCCESS;
  }
  else if ((app_search_count < read_cnf_ptr->read_data.data_len) &&
           (read_cnf_ptr->read_data.data_ptr[app_search_count] ==
             MMGSDI_JCDMA_JCSIM2_ID))
  {
    /* If JCSIM2 App is found copy over the AID whose
       length is stored in the 4th byte of the App entry and whose
       value is stored from the 5th byte onwards */
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      jcdma_uicc_info_ptr->jcsim2.aid.data_ptr,
      read_cnf_ptr->read_data.data_ptr[MMGSDI_JCDMA_AID_LEN_INDEX]);
    if (jcdma_uicc_info_ptr->jcsim2.aid.data_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    jcdma_uicc_info_ptr->jcsim2.aid.data_len =
      read_cnf_ptr->read_data.data_ptr[MMGSDI_JCDMA_AID_LEN_INDEX];

    mmgsdi_memscpy(jcdma_uicc_info_ptr->jcsim2.aid.data_ptr,
                   int32touint32(jcdma_uicc_info_ptr->jcsim2.aid.data_len),
                   &read_cnf_ptr->read_data.data_ptr[MMGSDI_JCDMA_AID_START_INDEX],
                   int32touint32(jcdma_uicc_info_ptr->jcsim2.aid.data_len));
    return MMGSDI_SUCCESS;
  }
  else
  {
    return MMGSDI_ERROR;
  }
} /* mmgsdi_jcdma_find_csim_app_for_color_and_lock */


/*===========================================================================
FUNCTION MMGSDI_JCDMA_INIT

DESCRIPTION
  This function is called from MMGSDI init to allocated memory for the jcdma
  data structure and initialize the members

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_jcdma_init (
  uint8  slot_index
)
{
  if (slot_index >= MMGSDI_MAX_NUM_SLOTS ||
      slot_index >= mmgsdi_generic_data_ptr->num_slots)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* Free memory if already allocated */
  if (NULL != mmgsdi_jcdma_uicc_info_slot_ptr[slot_index])
  {
    mmgsdi_jcdma_free(slot_index);
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_jcdma_uicc_info_slot_ptr[slot_index],
                                     sizeof(mmgsdi_jcdma_uicc_info_type));
  if (NULL == mmgsdi_jcdma_uicc_info_slot_ptr[slot_index])
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  mmgsdi_jcdma_uicc_info_slot_ptr[slot_index]->jcsim1.session_attr.lock_state  =
    MMGSDI_JCDMA_MAX_LOCK_STATE_ENUM;
  mmgsdi_jcdma_uicc_info_slot_ptr[slot_index]->jcsim1.session_attr.imsi_status =
    MMGSDI_JCDMA_MAX_IMSI_STATUS_ENUM;
  mmgsdi_jcdma_uicc_info_slot_ptr[slot_index]->jcsim2.session_attr.lock_state  =
    MMGSDI_JCDMA_MAX_LOCK_STATE_ENUM;
  mmgsdi_jcdma_uicc_info_slot_ptr[slot_index]->jcsim2.session_attr.imsi_status =
    MMGSDI_JCDMA_MAX_IMSI_STATUS_ENUM;

  return MMGSDI_SUCCESS;
} /* mmgsdi_jcdma_init */


/*===========================================================================
FUNCTION MMGSDI_JCDMA_FREE

DESCRIPTION
  This function is called to free memory previously allocated to keep track
  of JCDMA info data

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE:
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_jcdma_free (
  uint8  slot_index
)
{
  if (slot_index <  MMGSDI_MAX_NUM_SLOTS &&
      slot_index <  mmgsdi_generic_data_ptr->num_slots &&
      NULL       != mmgsdi_jcdma_uicc_info_slot_ptr[slot_index])
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
      mmgsdi_jcdma_uicc_info_slot_ptr[slot_index]->jcsim1.aid.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
      mmgsdi_jcdma_uicc_info_slot_ptr[slot_index]->jcsim2.aid.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_jcdma_uicc_info_slot_ptr[slot_index]);
  }
} /* mmgsdi_jcdma_free */


/*===========================================================================
FUNCTION MMGSDI_JCDMA_GET_UICC_CARD_ATTR

DESCRIPTION
  This function gets the info from the CSIM Apps for color and lock
  determination

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_jcdma_get_uicc_card_attr (
  mmgsdi_app_enum_type      app_type,
  mmgsdi_slot_id_enum_type  slot
)
{
  mmgsdi_return_enum_type             mmgsdi_status       = MMGSDI_ERROR;
  mmgsdi_read_req_type               *read_req_ptr        = NULL;
  mmgsdi_read_cnf_type               *read_cnf_ptr        = NULL;
  boolean                             gid1_available      = FALSE;
  boolean                             gid2_available      = FALSE;
  mmgsdi_jcdma_uicc_info_type        *jcdma_uicc_info_ptr =  NULL;

  jcdma_uicc_info_ptr =
    mmgsdi_jcdma_get_uicc_info_ptr_from_slot(slot);

  MMGSDIUTIL_RETURN_IF_NULL(jcdma_uicc_info_ptr);

  if (app_type != MMGSDI_APP_CSIM)
  {
    /* Return if provisioning app is not of type CSIM  */
    return MMGSDI_SUCCESS;
  }

  if ((jcdma_uicc_info_ptr->jcsim1.session_id == 0x0) ||
      (jcdma_uicc_info_ptr->jcsim2.session_id == 0x0))
  {
    /* Return if sessions have not been established with JCSIM2 and JCSIM1 apps */
    return MMGSDI_SUCCESS;
  }

  if ((jcdma_uicc_info_ptr->jcsim1.attr_avail == TRUE) &&
      (jcdma_uicc_info_ptr->jcsim2.attr_avail == TRUE))
  {
    /* Return if attributes have already been read */
    return MMGSDI_SUCCESS;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(read_req_ptr,
    sizeof(mmgsdi_read_req_type));
  if (read_req_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  read_req_ptr->access.access_method      = MMGSDI_EF_ENUM_ACCESS;
  read_req_ptr->data_len                  = 0; /* read entire file */
  read_req_ptr->rec_num                   = 0;
  read_req_ptr->offset                    = 0;
  read_req_ptr->file_type                 = MMGSDI_TRANSPARENT_FILE;
  read_req_ptr->request_header.slot_id    = slot;

  read_req_ptr->request_header.session_id =
    jcdma_uicc_info_ptr->jcsim2.session_id;
  read_req_ptr->access.file.file_enum     = MMGSDI_CSIM_UIM_VERSION;
  mmgsdi_status = mmgsdi_uim_uicc_read_transparent(read_req_ptr,
                                                   TRUE,
                                                   &read_cnf_ptr);
  if(read_cnf_ptr == NULL)
  {
    UIM_MSG_ERR_0("JCSIM2 version read cnf ptr NULL");
    MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
    return MMGSDI_ERROR;
  }

  if ((mmgsdi_status == MMGSDI_SUCCESS) &&
      (read_cnf_ptr->response_header.mmgsdi_status == MMGSDI_SUCCESS))
  {
    if (read_cnf_ptr->read_data.data_ptr == NULL ||
        read_cnf_ptr->read_data.data_len < 18)
    {
      UIM_MSG_ERR_0("JCSIM2 version read data ptr NULL");
      MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_cnf_ptr->read_data.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
      return MMGSDI_ERROR;
    }

    /* Check the version. If the 1st byte of the signature file is set
       to the same value as a test card then the card is marked as
       an unknown card */
    if (MMGSDI_JCDMA_UIM_TEST_VERSION == read_cnf_ptr->read_data.data_ptr[0])
    {
      UIM_MSG_HIGH_0("JCSIM2 invalid test version");
      jcdma_uicc_info_ptr->card_attr.card_type =
         MMGSDI_JCDMA_UNKNOWN_CARD;
      MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr->read_data.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
      /* JCDMA test card. No need to proceed */
      return MMGSDI_SUCCESS;
    }

    /* Compare the signature which starts from the 2nd byte of version EF */
    if (memcmp (&read_cnf_ptr->read_data.data_ptr[1],
                mmgsdi_jcdma_signature, MMGSDI_JCDMA_SIG_SIZE))
    {
      UIM_MSG_HIGH_0("JCSIM2 invalid signature failure");
      jcdma_uicc_info_ptr->card_attr.card_type =
        MMGSDI_JCDMA_NON_JCDMA_CARD;
      MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr->read_data.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
      /* JCDMAsignature mismatch. No need to proceed */
      return MMGSDI_SUCCESS;
    }

    /* Check skip UIM lock bit which is the 1st bit of the 18th byte of the
       version EF */
    if (read_cnf_ptr->read_data.data_ptr[17] & MMGSDI_JCDMA_UIM_LOCK_IND)
    {
      /* Lock is disabled */
      jcdma_uicc_info_ptr->card_attr.lock_enabled = FALSE;
      jcdma_uicc_info_ptr->jcsim2.session_attr.lock_state =
        MMGSDI_JCDMA_LOCK_DISABLED;
    }
    else
    {
      /* Lock is enabled */
      jcdma_uicc_info_ptr->card_attr.lock_enabled = TRUE;
      jcdma_uicc_info_ptr->jcsim2.session_attr.lock_state =
        MMGSDI_JCDMA_LOCK_ENABLED;
    }
  }
  /* handling if read on MMGSDI_CDMA_UIM_VERSION file failed */
  else
  {
    jcdma_uicc_info_ptr->card_attr.card_type =
      MMGSDI_JCDMA_NON_JCDMA_CARD;
    UIM_MSG_HIGH_1("JCSIM2 signature EF read failed w/ status=0x%x",
                   read_cnf_ptr->response_header.mmgsdi_status);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_cnf_ptr->read_data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
    /* Failed to read JCDMA signature EF */
    return MMGSDI_SUCCESS;
  }

  MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr->read_data.data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);

  read_req_ptr->request_header.session_id =
    jcdma_uicc_info_ptr->jcsim2.session_id;
  read_req_ptr->access.file.file_enum = MMGSDI_CSIM_IMSI_M;
  mmgsdi_status = mmgsdi_uim_uicc_read_transparent(read_req_ptr,
                                                   TRUE,
                                                   &read_cnf_ptr);
  if(read_cnf_ptr == NULL)
  {
    UIM_MSG_ERR_0("JCSIM2 IMSI_M read cnf ptr is NULL");
    MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
    return MMGSDI_ERROR;
  }

  if ((mmgsdi_status != MMGSDI_SUCCESS) ||
      (read_cnf_ptr->response_header.mmgsdi_status != MMGSDI_SUCCESS) ||
      (read_cnf_ptr->read_data.data_ptr == NULL) ||
      (read_cnf_ptr->read_data.data_len < 10))
  {
    UIM_MSG_ERR_1("JCSIM2 IMSI_M read failed w/ status=0x%x",
                  read_cnf_ptr->response_header.mmgsdi_status);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_cnf_ptr->read_data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
    return MMGSDI_ERROR;
  }

  /* Check the 8th bit of byte 8 of the IMSI_M EF to determine if IMSI_M
     is programmed or not */
  if(read_cnf_ptr->read_data.data_ptr[7] & MMGSDI_JCDMA_UIM_IMSI_PROGRAMMED)
  {
    jcdma_uicc_info_ptr->jcsim2.session_attr.imsi_programmed = TRUE;
  }
  else
  {
    jcdma_uicc_info_ptr->jcsim2.session_attr.imsi_programmed = FALSE;
  }

  UIM_MSG_HIGH_1("JCSIM2 app IMSI_M programming status: 0x%x",
                 jcdma_uicc_info_ptr->jcsim2.session_attr.imsi_programmed);

  MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr->read_data.data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);

  jcdma_uicc_info_ptr->jcsim2.attr_avail = TRUE;

  read_req_ptr->request_header.session_id =
    jcdma_uicc_info_ptr->jcsim1.session_id;
  read_req_ptr->access.file.file_enum = MMGSDI_CSIM_IMSI_M;
  mmgsdi_status = mmgsdi_uim_uicc_read_transparent(read_req_ptr,
                                                   TRUE,
                                                   &read_cnf_ptr);
  if(read_cnf_ptr == NULL)
  {
    UIM_MSG_ERR_0("JCSIM1 IMSI_M read cnf ptr is NULL");
    MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
    return MMGSDI_ERROR;
  }

  if ((mmgsdi_status != MMGSDI_SUCCESS) ||
      (read_cnf_ptr->response_header.mmgsdi_status != MMGSDI_SUCCESS) ||
      (read_cnf_ptr->read_data.data_ptr == NULL) ||
      (read_cnf_ptr->read_data.data_len < 10))
  {
    UIM_MSG_ERR_1("JCSIM1 IMSI_M read failed w/ status=0x%x",
                  read_cnf_ptr->response_header.mmgsdi_status);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_cnf_ptr->read_data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
    return MMGSDI_ERROR;
  }

  /* Check the 8th bit of byte 8 of the IMSI_M EF to determine if IMSI_M
     is programmed or not */
  if(read_cnf_ptr->read_data.data_ptr[7] & MMGSDI_JCDMA_UIM_IMSI_PROGRAMMED)
  {
    jcdma_uicc_info_ptr->jcsim1.session_attr.imsi_programmed = TRUE;
  }
  else
  {
    jcdma_uicc_info_ptr->jcsim1.session_attr.imsi_programmed = FALSE;
  }
  UIM_MSG_HIGH_1("JCSIM1 app IMSI_M programming status: 0x%x",
                 jcdma_uicc_info_ptr->jcsim1.session_attr.imsi_programmed);

  /* Get NW lock code, i.e MCC, byte 9 and byte 10 from IMSI_M */
  jcdma_uicc_info_ptr->jcsim1.session_attr.imsi_mcc =
    (read_cnf_ptr->read_data.data_ptr[9] << MMGSDI_JCDMA_8BITS) |
    read_cnf_ptr->read_data.data_ptr[8];

  jcdma_uicc_info_ptr->card_attr.imsi_mcc =
    jcdma_uicc_info_ptr->jcsim1.session_attr.imsi_mcc;

  MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr->read_data.data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);

  read_req_ptr->access.file.file_enum = MMGSDI_CSIM_IMSI_STATUS;
  mmgsdi_status = mmgsdi_uim_uicc_read_transparent(read_req_ptr,
                                                   TRUE,
                                                   &read_cnf_ptr);
  if(read_cnf_ptr == NULL)
  {
    UIM_MSG_ERR_0("JCSIM1 IMSI Status read cnf ptr NULL");
    MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
    return MMGSDI_ERROR;
  }

  if ((mmgsdi_status != MMGSDI_SUCCESS) ||
      (read_cnf_ptr->response_header.mmgsdi_status != MMGSDI_SUCCESS) ||
      (read_cnf_ptr->read_data.data_ptr == NULL) ||
      (read_cnf_ptr->read_data.data_len < 1))
  {
    UIM_MSG_ERR_1("JCSIM1 IMSI Status read failed w/ status=0x%x",
                  read_cnf_ptr->response_header.mmgsdi_status);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_cnf_ptr->read_data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
    return MMGSDI_ERROR;
  }

  /* Check if byte 1 of the IMSI_STATUS EF is set to 0 or 1 to determine
     the color */
  if(read_cnf_ptr->read_data.data_ptr[0] & MMGSDI_JCDMA_UIM_IMSI_STATUS)
  {
    jcdma_uicc_info_ptr->jcsim1.session_attr.imsi_status =
      MMGSDI_JCDMA_IMSI_STATUS_1;
  }
  else
  {
    jcdma_uicc_info_ptr->jcsim1.session_attr.imsi_status =
      MMGSDI_JCDMA_IMSI_STATUS_0;
  }
  UIM_MSG_HIGH_1("JCSIM1 IMSI Status: 0x%x",
                 jcdma_uicc_info_ptr->jcsim1.session_attr.imsi_status);

  MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr->read_data.data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);

  switch (jcdma_uicc_info_ptr->jcsim2.session_attr.imsi_programmed)
  {
    case TRUE: /* JCSIM2 IMSI is programmed */
      switch (jcdma_uicc_info_ptr->jcsim1.session_attr.imsi_programmed)
      {
        case TRUE: /* JCSIM1 IMSI is programmed */
          switch (jcdma_uicc_info_ptr->jcsim1.session_attr.imsi_status)
          {
            case MMGSDI_JCDMA_IMSI_STATUS_0:
              jcdma_uicc_info_ptr->card_attr.card_type =
                MMGSDI_JCDMA_BLACK_CARD;
              break;
            case MMGSDI_JCDMA_IMSI_STATUS_1:
              jcdma_uicc_info_ptr->card_attr.card_type =
                MMGSDI_JCDMA_GRAY_BLACK_CARD;
              break;
            default:
              break;
          }
          break;
        case FALSE: /* JCSIM1 IMSI is not programmed */
          jcdma_uicc_info_ptr->card_attr.card_type =
            MMGSDI_JCDMA_WHITE_BLACK_CARD;
          break;
        default:
          break;
      }
      break;
    case FALSE: /* JCSIM2 IMSI is not programmed */
      switch (jcdma_uicc_info_ptr->jcsim1.session_attr.imsi_programmed)
      {
        case TRUE: /* JCSIM1 IMSI is programmed */
          switch (jcdma_uicc_info_ptr->jcsim1.session_attr.imsi_status)
          {
            case MMGSDI_JCDMA_IMSI_STATUS_0:
              jcdma_uicc_info_ptr->card_attr.card_type =
                MMGSDI_JCDMA_BLACK_WHITE_CARD;
              break;
            case MMGSDI_JCDMA_IMSI_STATUS_1:
              jcdma_uicc_info_ptr->card_attr.card_type =
                MMGSDI_JCDMA_GRAY_WHITE_CARD;
              break;
            default:
              break;
          }
          break;
        case FALSE: /* JCSIM1 IMSI is not programmed */
          jcdma_uicc_info_ptr->card_attr.card_type =
            MMGSDI_JCDMA_WHITE_CARD;
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
  UIM_MSG_HIGH_1("JCDMA: Card color is 0x%x",
                 jcdma_uicc_info_ptr->card_attr.card_type);

  if ((jcdma_uicc_info_ptr->card_attr.card_type !=
        MMGSDI_JCDMA_BLACK_CARD) &&
      (jcdma_uicc_info_ptr->card_attr.card_type !=
        MMGSDI_JCDMA_GRAY_BLACK_CARD))
  {
    MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
    return MMGSDI_SUCCESS;
  }

  read_req_ptr->access.file.file_enum = MMGSDI_CSIM_RUIM_ID;
  mmgsdi_status = mmgsdi_uim_uicc_read_transparent(read_req_ptr,
                                                   TRUE,
                                                   &read_cnf_ptr);
  if(read_cnf_ptr == NULL)
  {
    UIM_MSG_ERR_0("JCSIM1 RUIMID read cnf ptr NULL");
    MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
    return MMGSDI_ERROR;
  }
  if ((mmgsdi_status == MMGSDI_SUCCESS) &&
      (read_cnf_ptr->response_header.mmgsdi_status == MMGSDI_SUCCESS))
  {
    if (read_cnf_ptr->read_data.data_ptr == NULL ||
        read_cnf_ptr->read_data.data_len < 2 ||
        read_cnf_ptr->read_data.data_ptr[0] > (read_cnf_ptr->read_data.data_len - 1))
    {
      UIM_MSG_ERR_0("JCSIM1 RUIMID read data ptr NULL");
      MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_cnf_ptr->read_data.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
      return MMGSDI_ERROR;
    }
    else
    {
      /* Copy the RUIM ID which starts from byte 2 onwards with
         the size listed in byte 1 */
      mmgsdi_memscpy(&jcdma_uicc_info_ptr->jcsim1.session_attr.ruim_id,
                     sizeof(jcdma_uicc_info_ptr->jcsim1.session_attr.ruim_id),
                     &read_cnf_ptr->read_data.data_ptr[1],
                     read_cnf_ptr->read_data.data_ptr[0]);
    }
  }
  else
  {
    /* Failed to read RUIM ID. Set to defalt */
    jcdma_uicc_info_ptr->card_attr.ruim_id = 0xFFFFFFFF;
    UIM_MSG_ERR_1("JCSIM1 RUIMID read failed w/ status=0x%x",
                  read_cnf_ptr->response_header.mmgsdi_status);
  }

  jcdma_uicc_info_ptr->card_attr.ruim_id =
    jcdma_uicc_info_ptr->jcsim1.session_attr.ruim_id;

  MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr->read_data.data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);

  mmgsdi_status = mmgsdi_uicc_chk_srv_available(
                    mmgsdi_generic_data_ptr->client_id,
                    jcdma_uicc_info_ptr->jcsim1.session_id,
                    MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                    MMGSDI_CSIM_SRV_GID1,
                    &gid1_available);

  if (gid1_available)
  {
    read_req_ptr->access.file.file_enum = MMGSDI_CSIM_GID1;
    mmgsdi_status = mmgsdi_uim_uicc_read_transparent(read_req_ptr,
                                                     TRUE,
                                                     &read_cnf_ptr);
    if(read_cnf_ptr == NULL)
    {
      UIM_MSG_ERR_0("JCSIM1 GID1 read cnf NULL");
      MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
      return MMGSDI_ERROR;
    }

    if ((mmgsdi_status == MMGSDI_SUCCESS) &&
        (read_cnf_ptr->response_header.mmgsdi_status == MMGSDI_SUCCESS))
    {
      if (read_cnf_ptr->read_data.data_ptr == NULL ||
          read_cnf_ptr->read_data.data_len < 1)
      {
        UIM_MSG_ERR_0("JCSIM1 GID1 read data ptr NULL");
        MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_cnf_ptr->read_data.data_ptr);
        MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
        return MMGSDI_ERROR;
      }

      /* Copy the SP Id from byte 1 of the GID1 EF contents */
      jcdma_uicc_info_ptr->jcsim1.session_attr.sp_id =
        read_cnf_ptr->read_data.data_ptr[0];
    }
    else
    {
      /* Failed to read SP code. Set to defalt */
      jcdma_uicc_info_ptr->jcsim1.session_attr.sp_id =
        MMGSDI_JCDMA_UIM_SP_UG_DEFAULT;
      UIM_MSG_ERR_1("JCSIM1 GID1 read failed w/ status=0x%x",
                    read_cnf_ptr->response_header.mmgsdi_status);
    }

    MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr->read_data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
  }
  else
  {
    UIM_MSG_ERR_0("JCDMA: GID1 disabled in CST. Setting default SP ID");
    jcdma_uicc_info_ptr->jcsim1.session_attr.sp_id =
      MMGSDI_JCDMA_UIM_SP_UG_DEFAULT;
  }

  jcdma_uicc_info_ptr->card_attr.sp_id =
    jcdma_uicc_info_ptr->jcsim1.session_attr.sp_id;

  mmgsdi_status = mmgsdi_uicc_chk_srv_available(
                    mmgsdi_generic_data_ptr->client_id ,
                    jcdma_uicc_info_ptr->jcsim1.session_id,
                    MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                    MMGSDI_CSIM_SRV_GID2,
                    &gid2_available);

  if (gid2_available)
  {
    read_req_ptr->access.file.file_enum = MMGSDI_CSIM_GID2;
    mmgsdi_status = mmgsdi_uim_uicc_read_transparent(read_req_ptr,
                                                     TRUE,
                                                     &read_cnf_ptr);

    if(read_cnf_ptr == NULL)
    {
      UIM_MSG_ERR_0("JCSIM1 GID2 read cnf ptr NULL");
      MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
      return MMGSDI_ERROR;
    }

    if ((mmgsdi_status == MMGSDI_SUCCESS) &&
        (read_cnf_ptr->response_header.mmgsdi_status == MMGSDI_SUCCESS))
    {
      if (read_cnf_ptr->read_data.data_ptr == NULL ||
          read_cnf_ptr->read_data.data_len < 1)
      {
        UIM_MSG_ERR_0("JCSIM1 GID2 read data ptr NULL");
        MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_cnf_ptr->read_data.data_ptr);
        MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
        return MMGSDI_ERROR;
      }

      /* Copy the UG Id from byte 1 of the GID2 EF contents */
      jcdma_uicc_info_ptr->jcsim1.session_attr.ug_id =
        read_cnf_ptr->read_data.data_ptr[0];
    }
    else
    {
      /* Failed to read UG code. Set to defalt */
      jcdma_uicc_info_ptr->jcsim1.session_attr.ug_id =
         MMGSDI_JCDMA_UIM_SP_UG_DEFAULT;
      UIM_MSG_ERR_1("JCDMA: GID2 EF read failed w/ status=0x%x",
                    read_cnf_ptr->response_header.mmgsdi_status);
    }

    MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr->read_data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
  }
  else
  {
    UIM_MSG_ERR_0("JCDMA: GID2 disabled in CST. Setting default UG ID");
    jcdma_uicc_info_ptr->jcsim1.session_attr.ug_id =
       MMGSDI_JCDMA_UIM_SP_UG_DEFAULT;
  }

  jcdma_uicc_info_ptr->card_attr.ug_id =
    jcdma_uicc_info_ptr->jcsim1.session_attr.ug_id;

  jcdma_uicc_info_ptr->jcsim1.attr_avail = TRUE;
  MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
  return MMGSDI_SUCCESS;
} /* mmgsdi_jcdma_get_uicc_card_attr */


/*===========================================================================
FUNCTION MMGSDI_JCDMA_CLOSE_NON_PROV_SESSION

DESCRIPTION
  This function closes the non-prov sessions to the CSIM apps

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE:
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_jcdma_close_non_prov_session (
  const mmgsdi_app_info_type*      app_info_ptr
)
{
  mmgsdi_jcdma_uicc_info_type   *jcdma_uicc_info_ptr = NULL;
  mmgsdi_slot_id_enum_type       slot_id           = MMGSDI_MAX_SLOT_ID_ENUM;

  slot_id = app_info_ptr->slot;
  jcdma_uicc_info_ptr =
    mmgsdi_jcdma_get_uicc_info_ptr_from_slot(slot_id);

  if((jcdma_uicc_info_ptr == NULL) ||
     (app_info_ptr->app_data.app_type != MMGSDI_APP_CSIM))
  {
    return;
  }

  /* Close the sessions if session ids are non-zero, i.e. session
     was established in the first place */
  if (jcdma_uicc_info_ptr->jcsim1.session_id)
  {
  (void)mmgsdi_session_close(jcdma_uicc_info_ptr->jcsim1.session_id,
                             mmgsdi_jcdma_session_close_cb,
                             (mmgsdi_client_data_type)MMGSDI_JCDMA_JCSIM1_ID);
  }
  if (jcdma_uicc_info_ptr->jcsim2.session_id)
  {
  (void)mmgsdi_session_close(jcdma_uicc_info_ptr->jcsim2.session_id,
                             mmgsdi_jcdma_session_close_cb,
                             (mmgsdi_client_data_type)MMGSDI_JCDMA_JCSIM2_ID);
  }
} /* mmgsdi_jcdma_close_non_prov_session */


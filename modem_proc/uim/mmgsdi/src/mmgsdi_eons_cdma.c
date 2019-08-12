/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                  M M G S D I   E O N S   C D M A   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains routines related to finding ONS for CDMA sessions

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2013 - 2014, 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_eons_cdma.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/04/16   ar      EONS selective logging
05/22/16   vdc     Remove F3 messages for memory allocation failure
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
01/11/14   df      Fix off-target compilation error
01/10/14   df      Use v2.0 diag macros
01/09/14   ar      Replace old Macros with MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE
12/18/13   hh      For API calls, populate SPN and ONS only when condition allows
07/15/13   vs      EONS updates to support single baseband SGLTE

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "comdef.h"
#include "intconv.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdi_session.h"
#include "mmgsdi_evt.h"
#include "mmgsdi_eons.h"
#include "mmgsdicache.h"
#include "uim_selective_logging.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/* ----------------------------------------------------------------------------

   ENUM:      MMGSDI_EONS_CDMA_SPN_ENC_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the encoding types for cdma service provider name.
     Enum members are derived by referring 3GPP2 C.S0001 standard documents.
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_EONS_CDMA_SPN_ENC_INVALID            = 0x00, /**< Invalid encoding. */
  MMGSDI_EONS_CDMA_SPN_ENC_EXT_PROTOCOL_MSG,
  MMGSDI_EONS_CDMA_SPN_ENC_7BIT_ASCII,
  MMGSDI_EONS_CDMA_SPN_ENC_IA5,
  MMGSDI_EONS_CDMA_SPN_ENC_UNICODE,
  MMGSDI_EONS_CDMA_SPN_ENC_SHIFT_JIS,
  MMGSDI_EONS_CDMA_SPN_ENC_KOREAN,
  MMGSDI_EONS_CDMA_SPN_ENC_LATIN_HEBREW,
  MMGSDI_EONS_CDMA_SPN_ENC_LATIN,
  MMGSDI_EONS_CDMA_SPN_ENC_GSM_7BIT_DEF,
  MMGSDI_EONS_CDMA_SPN_ENC_GSM_DCS
 } mmgsdi_eons_cdma_spn_enc_enum_type;

/*==========================================================================
FUNCTION MMGSDI_EONS_GET_CDMA_SPN

DESCRIPTION
  Function to get service provide name from cached CDMA SPN file

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_eons_get_cdma_spn(
  mmgsdi_session_id_type                   session_id,
  mmgsdi_eons_ignore_disp_cond_enum_type   ignore_display_condition,
  mmgsdi_app_enum_type                     app_type,
  mmgsdi_eons_info_type                  * eons_info_ptr
)
{
  mmgsdi_return_enum_type           mmgsdi_status         = MMGSDI_ERROR;
  mmgsdi_access_type                cdma_spn_file_access;
  mmgsdi_data_type                  cdma_spn_data;
  uint8                             spn_buff_len          = 0;
  unsigned char                   * spn_buff_ptr          = NULL;
  mmgsdi_eons_encoding_enum_type    eons_encoding         = MMGSDI_EONS_ENC_INVALID;

  if(eons_info_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  cdma_spn_data.data_len                    = 0;
  cdma_spn_data.data_ptr                    = NULL;
  cdma_spn_file_access.access_method        = MMGSDI_EF_ENUM_ACCESS;

  if(app_type == MMGSDI_APP_CSIM)
  {
    cdma_spn_file_access.file.file_enum = MMGSDI_CSIM_SPN;
  }
  else if(app_type == MMGSDI_APP_RUIM)
  {
    cdma_spn_file_access.file.file_enum = MMGSDI_CDMA_HOME_SVC_PVDR_NAME;
  }

  mmgsdi_status = mmgsdi_cache_read_len(
                    session_id,
                    &cdma_spn_file_access,
                    &cdma_spn_data.data_len,
                    0,
                    NULL);

  UIM_SELECTIVE_MSG_HIGH_2(UIM_LOG_EONS, 
                           "cdma spn read cache len status: 0x%x cdma spn data len: 0x%x",
                           mmgsdi_status, cdma_spn_data.data_len);

  if(mmgsdi_status == MMGSDI_SUCCESS && cdma_spn_data.data_len > 0)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(cdma_spn_data.data_ptr,
                                       cdma_spn_data.data_len);
    if (cdma_spn_data.data_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    mmgsdi_status = mmgsdi_cache_read(
                      session_id,
                      &cdma_spn_file_access,
                      &cdma_spn_data,
                      0,
                      NULL);

    UIM_SELECTIVE_MSG_HIGH_1(UIM_LOG_EONS,
                             "cdma spn cache read status: 0x%x",
                             mmgsdi_status);

    if(mmgsdi_status == MMGSDI_SUCCESS)
    {
        eons_info_ptr->spn_display_bit =
          (cdma_spn_data.data_ptr[MMGSDI_EONS_SPN_DISP_COND_POS] &
             MMGSDI_EONS_CDMA_SPN_DISP_COND) ?
               MMGSDI_EONS_SPN_DISPLAY_BIT_VAL_1: MMGSDI_EONS_SPN_DISPLAY_BIT_VAL_0;
      }
    }

  if(eons_info_ptr->rplmn_info.first_stack_rplmn.is_home_network)
  {
    eons_info_ptr->display_spn = TRUE;
  }

  if(cdma_spn_data.data_ptr != NULL && mmgsdi_status == MMGSDI_SUCCESS)
  {
    switch (cdma_spn_data.data_ptr[MMGSDI_EONS_CDMA_SPN_ENC_POS] &
            MMGSDI_EONS_CDMA_SPN_ENC_VALID)
    {
      case MMGSDI_EONS_CDMA_SPN_ENC_EXT_PROTOCOL_MSG: /*Extended Protocol Message*/
      {
        eons_encoding = MMGSDI_EONS_ENC_EXT_PROTOCOL_MSG;
      }
      break;

      case MMGSDI_EONS_CDMA_SPN_ENC_7BIT_ASCII: /*7-bit ASCII*/
      {
         eons_encoding = MMGSDI_EONS_ENC_7BIT_ASCII;
      }
      break;

      case MMGSDI_EONS_CDMA_SPN_ENC_IA5: /*IA5*/
      {
        eons_encoding = MMGSDI_EONS_ENC_IA5;
      }
      break;

      case MMGSDI_EONS_CDMA_SPN_ENC_UNICODE: /*Unicode Encoding*/
      {
        eons_encoding = MMGSDI_EONS_ENC_UCS2;
      }
      break;

      case MMGSDI_EONS_CDMA_SPN_ENC_SHIFT_JIS: /*shift JIS*/
      {
        eons_encoding = MMGSDI_EONS_ENC_SHIFT_JIS;
      }
      break;

      case MMGSDI_EONS_CDMA_SPN_ENC_KOREAN: /*korean*/
      {
        eons_encoding = MMGSDI_EONS_ENC_KOREAN;
      }
      break;

      case MMGSDI_EONS_CDMA_SPN_ENC_LATIN_HEBREW: /*latin hebrew*/
      {
        eons_encoding = MMGSDI_EONS_ENC_LATIN_HEBREW;
      }
      break;

      case MMGSDI_EONS_CDMA_SPN_ENC_LATIN: /*Latin*/
      {
        eons_encoding = MMGSDI_EONS_ENC_LATIN;
      }
      break;

      case MMGSDI_EONS_CDMA_SPN_ENC_GSM_7BIT_DEF: /*GSM 7 BIT default alphabet*/
      {
        eons_encoding = MMGSDI_EONS_ENC_GSM_7BIT_DEF_ALPHA_PACKED;
      }
      break;

      case MMGSDI_EONS_CDMA_SPN_ENC_GSM_DCS: /*GSM DCS*/
      {
        eons_encoding = MMGSDI_EONS_ENC_GSM_DCS;
      }
      break;

      default:
        break;
    }

    /*get first occurrence of 0xFF in CDMA SPN Buffer*/
    spn_buff_ptr =
      (unsigned char *)memchr(
         &cdma_spn_data.data_ptr[MMGSDI_EONS_CDMA_SPN_DATA_BUF_POS],
         0xFF,
         MMGSDI_EONS_CDMA_SPN_DATA_LEN);

    /*if pointer is not NULL, an occurrence of 0xFF is found*/
    if(spn_buff_ptr)
    {
      /*Calculate length of CDAM SPN buffer before ocurrence of 0xFF*/
      spn_buff_len = (uint8)(spn_buff_ptr -
                       (&cdma_spn_data.data_ptr[MMGSDI_EONS_CDMA_SPN_DATA_BUF_POS]));
    }
    else
    {
      /*CDMA SPN Buffer does not have 0xFF*/
      spn_buff_len = MMGSDI_EONS_CDMA_SPN_DATA_LEN;
    }

    if(spn_buff_len)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(eons_info_ptr->spn.eons_data.data_ptr);
      memset(&(eons_info_ptr->spn), 0x0, sizeof(mmgsdi_eons_name_type));

      eons_info_ptr->spn.eons_encoding = eons_encoding;
      eons_info_ptr->spn.eons_data.data_len = spn_buff_len;

      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
        eons_info_ptr->spn.eons_data.data_ptr,
        spn_buff_len);

      if(eons_info_ptr->spn.eons_data.data_ptr != NULL)
      {
        mmgsdi_memscpy(eons_info_ptr->spn.eons_data.data_ptr,
          spn_buff_len,
          &cdma_spn_data.data_ptr[MMGSDI_EONS_CDMA_SPN_DATA_BUF_POS],
          spn_buff_len);

        if (uim_selective_logging[UIM_LOG_EONS])
        {
          /* print spn data buffer*/
          mmgsdi_util_print_data_to_diag("cdma spn data buffer",
            eons_info_ptr->spn.eons_data.data_ptr,
            (uint16)spn_buff_len);
        }
      }
      else
      {
        mmgsdi_status = MMGSDI_ERROR;
      }
    }

    eons_info_ptr->lang_ind =
      cdma_spn_data.data_ptr[MMGSDI_EONS_CDMA_SPN_LANG_IND_POS];

  }
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cdma_spn_data.data_ptr);

  UIM_SELECTIVE_MSG_HIGH_3(UIM_LOG_EONS,
                           "CDMA spn length: 0x%x, encoding: 0x%x, lang ind 0x%x",
                           eons_info_ptr->spn.eons_data.data_len,
                           eons_info_ptr->spn.eons_encoding,
                           eons_info_ptr->lang_ind);

  return mmgsdi_status;
}/* mmgsdi_eons_get_cdma_spn */


/*===========================================================================
  FUNCTION MMGSDI_EONS_UPDATE_CDMA_SERVING_SYS_INFO

DESCRIPTION
  Function to update serving system related info.

DEPENDENCIES
  This function can only be called if there is an SS event trigger

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_eons_update_cdma_serving_sys_info(
  mmgsdi_eons_info_type   * eons_info_ptr
)
{
  if (eons_info_ptr == NULL)
  {
    return;
  }

  eons_info_ptr->rplmn_info.first_stack_rplmn.rplmn_available = TRUE;

  eons_info_ptr->rplmn_info.first_stack_rplmn.srv_status =
    eons_info_ptr->ss_info.first_stack_ss.srv_status;

  /* Set cdma home network status based on roaming status */
  /* Note: 2nd stack processing is only for 3GPP so no need to update
           home network status for 2nd stack in case of CDMA */
  if (eons_info_ptr->ss_info.first_stack_ss.roam_status == SYS_ROAM_STATUS_OFF)
  {
    eons_info_ptr->rplmn_info.first_stack_rplmn.is_home_network = TRUE;
  }
  else
  {
    eons_info_ptr->rplmn_info.first_stack_rplmn.is_home_network = FALSE;
  }
} /* mmgsdi_eons_update_cdma_serving_sys_info */


/*===========================================================================
  FUNCTION MMGSDI_EONS_RUN_ALGO_FOR_CDMA

DESCRIPTION
  Function to run algorithm for 1x primary or 1x secondary session ids.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_run_algo_for_cdma(
  mmgsdi_session_id_type                         session_id,
  mmgsdi_eons_info_type                        * eons_info_ptr,
  mmgsdi_app_enum_type                           app_type,
  mmgsdi_eons_ignore_disp_cond_enum_type         ignore_display_condition,
  mmgsdi_eons_name_type                        * spn_ptr,
  uint8                                        * lang_ind_ptr,
  mmgsdi_eons_display_info_type                * display_info_ptr
)
{
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_SUCCESS;

  if(eons_info_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /*Get service provider name from EF-SPN, copy it into the APP info along
    with language indication, spn display bit, roaming status and
    spn display rule */
  mmgsdi_status = mmgsdi_eons_get_cdma_spn(
                    session_id,
                    ignore_display_condition,
                    app_type,
                    eons_info_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (display_info_ptr)
  {
    memset(display_info_ptr, 0x0, sizeof(mmgsdi_eons_display_info_type));

    display_info_ptr->roaming_status =
      (eons_info_ptr->rplmn_info.first_stack_rplmn.is_home_network) ?
        MMGSDI_EONS_IN_HOME_NW: MMGSDI_EONS_IN_ROAMING_NW;

    display_info_ptr->spn_display_bit = eons_info_ptr->spn_display_bit;
  }

  if (lang_ind_ptr)
  {
    *lang_ind_ptr =   eons_info_ptr->lang_ind;
  }

  UIM_SELECTIVE_MSG_HIGH_3(UIM_LOG_EONS,
                           "is_home_network=0x%x, ignore_display_condition=0x%x, spn_display_bit=0x%x",
                           eons_info_ptr->rplmn_info.first_stack_rplmn.is_home_network,
                           ignore_display_condition,
                           eons_info_ptr->spn_display_bit);

  /* To determine whether to populate registered sysytem name, check
     1. If client wants to ignore SPN display condition, populate
     2. Otherwise, if UE is not on home network, populate
     3. Otherwise, if b1 == 1, populate
   */
  if (spn_ptr &&
      ((MMGSDI_EONS_IGNORE_SPN_DISPLAY_COND       == ignore_display_condition) ||
       (MMGSDI_EONS_IGNORE_SPN_RPLMN_DISPLAY_COND == ignore_display_condition) ||
       (!eons_info_ptr->rplmn_info.first_stack_rplmn.is_home_network)          ||
       (MMGSDI_EONS_SPN_DISPLAY_BIT_VAL_1 == eons_info_ptr->spn_display_bit)))
  {
    memset(spn_ptr, 0x0, sizeof(mmgsdi_eons_name_type));

    spn_ptr->eons_encoding      = eons_info_ptr->spn.eons_encoding;
    spn_ptr->eons_data.data_len = eons_info_ptr->spn.eons_data.data_len;

    if(spn_ptr->eons_data.data_len > 0)
    {
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
        spn_ptr->eons_data.data_ptr,
        spn_ptr->eons_data.data_len);

      if(spn_ptr->eons_data.data_ptr == NULL)
      {
        return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }
      mmgsdi_memscpy(spn_ptr->eons_data.data_ptr,
                     spn_ptr->eons_data.data_len,
                     eons_info_ptr->spn.eons_data.data_ptr,
                     eons_info_ptr->spn.eons_data.data_len);
    }
  }
  return mmgsdi_status;
} /* mmgsdi_eons_run_algo_for_cdma */


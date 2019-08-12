/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                   M M G S D I   C S G   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the CSG related functions

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2012-2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_csg.c#1 $$ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/04/16   ar      EONS selective logging
05/22/16   vdc     Remove F3 messages for memory allocation failure
05/16/16   tkl     F3 log prints cleanup
05/16/16   ar      F3 reduction
10/06/15   rp      Fix usage of wrong size in memset
08/10/15   vdc     Add length checks to prevent invalid dereference
05/04/15   bcho    Skip unused byte in CSG List TLV
01/19/15   tkl     Fixed compilation error with FFEATURE_UIM_PERSISTENT_CACHE
01/19/15   kr      Fixed Compilation warnings
09/29/14   tkl     Added support for persistent cache
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
07/15/14   yt      Support for NV refresh
06/03/14   tl      Remove additional parameter variables
05/09/14   vv      Perform search for empty records in EF-OCSGL/ACSGL
04/22/14   at      Critical KW fixes
01/11/14   df      Fix off-target compilation error
01/10/14   df      Use v2.0 diag macros
01/09/14   ar      Replace old Macros with MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE
12/05/13   bcho    Re-run the EONS algorithm when PLMNID/LAC/RAT is changed
11/22/13   vv      Added input parameter check for the CSG ID
10/04/13   rp      F3 message reduction
07/15/13   vs      EONS updates to support single baseband SGLTE
07/03/13   bcho    PLMN compare fn modified according to Annex A 3GPP TS 23122
06/24/13   vdc     Added support for triple SIM
06/18/13   vv      Reset the status if HNBN record is not found
06/18/13   vv      Fix for copying the HNB name length
05/28/13   spo     Replaced instances of memmove with memsmove
05/16/13   vdc     Replace memcpy with safer version memscpy
05/06/13   vv      Fixed error when trying to delete the only available CSG ID
04/24/12   vv      Reset CSG eons info in the app. info
01/20/13   vv      Fixed critical KW warnings
01/20/13   vv      Introduced new UCS2_PACKED encoding scheme
01/20/13   vv      Check if RPLM name info is available
01/20/13   vv      Replace memcpy() with memmove()
01/20/13   vv      Added support for updating EF-ACSGL record
10/15/12   vv      Added EONS support for CSG

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "intconv.h"
#include "mmgsdi.h"
#include "mmgsdi_uim_uicc.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdicache.h"
#include "mmgsdi_eons.h"
#include "mmgsdi_csg.h"
#include "mmgsdi_nv.h"
#include "uim_selective_logging.h"

#ifdef FEATURE_UIM_PERSISTENT_CACHE
#include "mmgsdi_persistent_cache.h"
#endif /* FEATURE_UIM_PERSISTENT_CACHE */

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Length of CSG Info in CSG Info TLV:
   CSG Type Indication 1 byte, 
   HNB Name Indication 1 byte
   CSG ID size         4 byte */
#define MMGSDI_CSG_INFO_SIZE  MMGSDI_CSG_ID_SIZE + 2

/* Length of CSG Display Indicator  */
#define MMGSDI_CSG_DISP_IND_SIZE  1 

static void mmgsdi_eons_csg_file_attr_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type  * cnf_ptr
);


/*=============================================================================

                       FUNCTION IMPLEMENTATIONS

=============================================================================*/


/*==========================================================================
FUNCTION MMGSDI_EONS_SEND_LAST_KNOWN_RPLMN_INFO

DESCRIPTION
  This is an utility function that provides the last known RPLMN info from
  the app info table

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_eons_send_last_known_rplmn_info(
  mmgsdi_eons_call_stack_enum_type   stack_type,
  mmgsdi_list_plmn_name_type       * plmn_name_list_ptr,
  mmgsdi_int_app_info_type         * int_app_info_ptr,
  uint32                             index
)
{
  mmgsdi_return_enum_type                   mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_eons_per_stack_rplmn_info_type   * stack_name_ptr   = NULL;

  if((plmn_name_list_ptr == NULL) ||
     (plmn_name_list_ptr->plmn_info_ptr == NULL) ||
     (int_app_info_ptr == NULL) ||
     (stack_type != MMGSDI_EONS_FIRST_CALL_STACK &&
      stack_type != MMGSDI_EONS_SEC_CALL_STACK))
  {
    return MMGSDI_ERROR;
  }

  if (stack_type == MMGSDI_EONS_FIRST_CALL_STACK)
  {
    stack_name_ptr = &(int_app_info_ptr->eons_info.rplmn_info.first_stack_rplmn);
  }
  else
  {
    stack_name_ptr = &(int_app_info_ptr->eons_info.rplmn_info.sec_stack_rplmn);
  }

  mmgsdi_status = mmgsdi_eons_copy_plmn_name_info(
                    &(stack_name_ptr->long_name),
                    &(plmn_name_list_ptr->plmn_info_ptr[index].plmn_long_name),
                    TRUE);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    mmgsdi_status = mmgsdi_eons_copy_plmn_name_info(
                      &(stack_name_ptr->short_name),
                      &(plmn_name_list_ptr->plmn_info_ptr[index].plmn_short_name),
                      TRUE);
  }

  return mmgsdi_status;
} /* mmgsdi_eons_send_last_known_rplmn_info */


/*==========================================================================
FUNCTION MMGSDI_CSG_PARSE_HNBN_TLV

DESCRIPTION
  This function parses the HNB TLV and provides the length and offset
  to the HNB name

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_csg_parse_hnbn_tlv(
  mmgsdi_data_type   hnbn_data,
  uint8 *            offset_ptr,
  mmgsdi_len_type *  hnbn_length_ptr
)
{
  if((hnbn_data.data_len <= 0)    ||
     (hnbn_data.data_ptr == NULL) ||
     (offset_ptr == NULL)         ||
     (hnbn_length_ptr == NULL))
  {
    UIM_MSG_ERR_0("HNB record data buffer is null");
    return MMGSDI_ERROR;
  }

  /* The TLV object length is coded according to ISO/IEC 8825.
     As per the spec., if the length is less than 0x7F, it is coded
     in one byte, otherwise it is coded in two bytes. The offset
     to the next valid tag is calculated accordingly */

  /* HNB name TLV begins with tag 0x80 */
  if ((hnbn_data.data_len > 1) &&
      (hnbn_data.data_ptr[0] == 0x80))
  {
    if(hnbn_data.data_ptr[1] <= 0x7F &&
       (hnbn_data.data_ptr[1] + 2) <= hnbn_data.data_len)
    {
      *hnbn_length_ptr = hnbn_data.data_ptr[1];
      *offset_ptr      = 2;
      return MMGSDI_SUCCESS;
    }
    else if ((hnbn_data.data_ptr[1] == 0x81) &&
             (hnbn_data.data_len > 2) &&
             (hnbn_data.data_ptr[2] + 3) <= hnbn_data.data_len)
    {
      *hnbn_length_ptr = hnbn_data.data_ptr[2];
      *offset_ptr      = 3;
      return MMGSDI_SUCCESS;
    }
    else
    {
      UIM_MSG_ERR_0("Incorrect HNB name length in TLV");
      return MMGSDI_ERROR;
    }
  }
  else
  {
    UIM_MSG_ERR_0("Incorrect HNB name TLV");
    return MMGSDI_ERROR;
  }
} /* mmgsdi_csg_parse_hnbn_tlv */


/*==========================================================================
FUNCTION MMGSDI_CSG_GET_HNB_NAME

DESCRIPTION
  This function gets the HNB name from the EF-HNBN or the EF-OHNBN. The
  file content is retrieved from the cache

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_csg_get_hnb_name(
  mmgsdi_session_id_type session_id,
  mmgsdi_file_enum_type  file,
  mmgsdi_rec_num_type    hnbn_rec,
  mmgsdi_data_type  *    hnbn_name_data
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
  uint8                   offset        = 0;
  uint32                  data_len      = 0;
  mmgsdi_access_type      access;
  mmgsdi_data_type        hnbn_rec_data;

  memset(&access, 0x00, sizeof(mmgsdi_access_type));

  access.access_method  = MMGSDI_EF_ENUM_ACCESS;
  access.file.file_enum = file;

  hnbn_rec_data.data_len = 0;
  hnbn_rec_data.data_ptr = NULL;

  /* Get the record length from cache */
  mmgsdi_status = mmgsdi_cache_get_record_len(session_id, access,
                                              hnbn_rec,
                                              &hnbn_rec_data.data_len);
  if((mmgsdi_status != MMGSDI_SUCCESS) ||
     (hnbn_rec_data.data_len == 0))
  {
    UIM_SELECTIVE_MSG_ERR_2(UIM_LOG_EONS,
                            "Rec 0x%x of file 0x%x not found in cache",
                            hnbn_rec, file);
    return MMGSDI_ERROR;
  }

  data_len = int32touint32(hnbn_rec_data.data_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(hnbn_rec_data.data_ptr, data_len);
  if(hnbn_rec_data.data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* Get the record data from cache */
  mmgsdi_status = mmgsdi_cache_get_record(session_id, access,
                                          hnbn_rec, &hnbn_rec_data);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_SELECTIVE_MSG_ERR_2(UIM_LOG_EONS,
                            "Unable to get record data for file 0x%x"
                            " rec num 0x%x", file, hnbn_rec);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(hnbn_rec_data.data_ptr);
    return mmgsdi_status;
  }

  /* Parse the HNBN TLV */
  mmgsdi_status = mmgsdi_csg_parse_hnbn_tlv(hnbn_rec_data, &offset,
                                             &hnbn_name_data->data_len);
  if((mmgsdi_status == MMGSDI_SUCCESS) &&
     (offset != 0) &&
     (hnbn_name_data->data_len > 0))
  {
    data_len = hnbn_name_data->data_len;

    /* This memory is free'd by the caller*/
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(hnbn_name_data->data_ptr, data_len);
    if(hnbn_name_data->data_ptr != NULL)
    {
      if(offset + int32touint8(hnbn_name_data->data_len)
          <= int32touint8(hnbn_rec_data.data_len))
      {
        mmgsdi_memscpy(hnbn_name_data->data_ptr,
                       data_len,
                       &hnbn_rec_data.data_ptr[offset],
                       hnbn_name_data->data_len);
      }
      else
      {
        UIM_SELECTIVE_MSG_ERR_1(UIM_LOG_EONS, "HNB name offset is incorrect, 0x%x",
                                offset);
        mmgsdi_status = MMGSDI_ERROR;
      }
    }
    else
    {
      mmgsdi_status = MMGSDI_ERROR;
    }
  }
  else
  {
    UIM_SELECTIVE_MSG_ERR_2(UIM_LOG_EONS, "Failed to parse HNB TLV for file 0x%x, rec num 0x%x",
                            file, hnbn_rec);
    mmgsdi_status = MMGSDI_ERROR;
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(hnbn_rec_data.data_ptr);
  return mmgsdi_status;
} /* mmgsdi_csg_get_hnb_name */


/*==========================================================================
FUNCTION MMGSDI_EONS_GET_CSG_ID

DESCRIPTION
  This function constructs the CSG ID from a data buffer. The CSG ID
  is exactly 27 bits.

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
==========================================================================*/
static mmgsdi_csg_id_type mmgsdi_csg_get_csg_id(
  uint8  * csg_buf,
  uint8    csg_buf_len
)
{
  mmgsdi_csg_id_type csg_id = 0x0FFFFFFF;

  if((csg_buf == NULL) || (csg_buf_len == 0))
  {
    UIM_MSG_ERR_0("CSG id buffer is NULL");
    return csg_id;
  }

  if(csg_buf_len >= 4)
  {
    csg_id = (((uint32)csg_buf[0] << 19) +
              ((uint32)csg_buf[1] << 11) +
              ((uint32)csg_buf[2] << 3)  +
              ((uint32)csg_buf[3] >> 5));
  }
  return csg_id;
} /* mmgsdi_csg_get_csg_id */


/*==========================================================================
FUNCTION MMGSDI_CSG_PARSE_CSGL_TLV

DESCRIPTION
  This function provides the offset to the next vaild tag (0x80 or 0x81 or
  0xA0) in the CSGL TLV data.

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_csg_parse_csgl_tlv(
  uint8               * data_buf_ptr,
  mmgsdi_len_type       data_len,
  uint8               * tlv_tag_ptr,
  uint8               * offset_ptr,
  uint8               * tlv_len_ptr,
  uint8              ** tlv_data_pptr)
{
  if((data_buf_ptr == NULL)  ||
     (tlv_tag_ptr  == NULL)  ||
     (offset_ptr   == NULL)  ||
     (tlv_len_ptr  == NULL)  ||
     (data_len <= 0))
  {
    return MMGSDI_ERROR;
  }

  /* The TLV object length is coded according to ISO/IEC 8825.
     As per the spec., if the length is less than 0x7F, it is coded
     in one byte, otherwise it is coded in two bytes. The offset
     to the tlv data is calculated accordingly */

  *tlv_tag_ptr = data_buf_ptr[0];
  if((data_len >= 2) && (data_buf_ptr[1] <= 0x7F))
  {
    *tlv_len_ptr   = data_buf_ptr[1];
    *offset_ptr    = 2;
    *tlv_data_pptr = data_buf_ptr + 2;
  }
  else if ((data_len >= 3) && (data_buf_ptr[1] == 0x81))
  {
    *tlv_len_ptr   = data_buf_ptr[2];
    *offset_ptr    = 3;
    *tlv_data_pptr = data_buf_ptr + 3;
  }
  else
  {
    UIM_MSG_ERR_0("Invalid TLV Length");
    return MMGSDI_ERROR;
  }

  if (data_len < *tlv_len_ptr + *offset_ptr)
  {
    return MMGSDI_ERROR;
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_csg_parse_csgl_tlv */


/*==========================================================================
FUNCTION MMGSDI_CSG_PARSE_CSGL_AND_POPULATE_TABLE

DESCRIPTION
  This function parsers the CSGL TLV data (from EF-ACSGL or EF-OCSGL) and
  populates the table with PLMN, CSG ID, CSG type and HNB name record
  numbers and CSG display indicator (for EF-OCSGL).

  In a record, the TLV data of EF-ACSGL can be,
  A0 _ 80 _ _ _ _ 81 _ _ _ _ _ _ _
  A0 _ 80 _ _ _ _ 81 _ _ _ _ _ _ _ 81 _ _ _ _ _ _ _
  A0 _ 80 _ _ _ _ 81 _ _ _ _ _ _ _ A0 _ 80 _ _ _ _ 81 _ _ _ _ _ _ _

  In a record, the TLV data of EF-OCSGL can be,
  A0 _ 80 _ _ _ _ 81 _ _ _ _ _ _ _82 _ _
  A0 _ 80 _ _ _ _ 81 _ _ _ _ _ _ _ 81 _ _ _ _ _ _ _
  A0 _ 80 _ _ _ _ 81 _ _ _ _ _ _ _ A0 _ 80 _ _ _ _ 81 _ _ _ _ _ _ _82 _ _

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_csg_parse_csgl_and_populate_table(
  mmgsdi_file_enum_type         file,
  mmgsdi_data_type              rec_data,
  uint16                     *  highest_index,
  mmgsdi_csg_plmn_info_type  *  mmgsdi_csg_plmn_table_ptr,
  uint8                         num_csg_plmn_table_entries
)
{
  mmgsdi_return_enum_type       mmgsdi_status             = MMGSDI_SUCCESS;
  mmgsdi_csg_id_type            csg_id_from_rec           = MMGSDI_EONS_INVALID_CSG_ID;
  uint8                     *   tlv_data_ptr              = NULL;
  uint8                     *   csgl_tlv_data_ptr         = NULL;
  uint8                         offset                    = 0;
  uint8                         length                    = 0;
  uint8                         tag                       = 0;
  uint16                        index                     = 0;
  mmgsdi_plmn_id_type           temp_plmn_id;
  uint8                         csgl_tlv_length           = 0;
  uint8                         tlv_temp_length           = 0;

  if((rec_data.data_len <= 0)    ||
     (rec_data.data_ptr == NULL) ||
     (mmgsdi_csg_plmn_table_ptr == NULL))
  {
    UIM_MSG_ERR_0("CSGL record data buffer is null");
    return MMGSDI_ERROR;
  }

  memset(temp_plmn_id.plmn_id_val, 0xFF, MMGSDI_PLMN_ID_SIZE);

  /* Begin parsing the TLV data */
  tlv_data_ptr = rec_data.data_ptr;

  /* Begin looping through the record data */
  while((tlv_data_ptr < rec_data.data_ptr + rec_data.data_len) &&
        (MMGSDI_SUCCESS == mmgsdi_csg_parse_csgl_tlv(tlv_data_ptr,
                                     rec_data.data_len - (tlv_data_ptr - rec_data.data_ptr),
                                     &tag, &offset, &csgl_tlv_length, &tlv_data_ptr)))
  {
    tlv_temp_length = 0;

    /* The TLV begins with tag A0 */
    if (tag != 0xA0)
    {
      /* Stop parsing rest of record data as unknown TLV is found; it is
         possible that we reach end of valid data in record and unused bytes
         are not set to 0xFF */
      UIM_MSG_HIGH_0("Unknown TLV");
      break;
    }

    csgl_tlv_data_ptr = tlv_data_ptr;
    UIM_MSG_HIGH_1("CSGL TLV length, 0x%x", csgl_tlv_length);

    /* Begin parsing the PLMN TLV data */
    if((tlv_data_ptr + csgl_tlv_length <= rec_data.data_ptr + rec_data.data_len) &&
       (MMGSDI_SUCCESS == mmgsdi_csg_parse_csgl_tlv(tlv_data_ptr,
                                   rec_data.data_len - (tlv_data_ptr - rec_data.data_ptr),
                                   &tag, &offset, &length, &tlv_data_ptr)))
    {
      /* Check if PLMN TLV is greater than length in CSG List TLV */
      tlv_temp_length = offset + length;
      if (csgl_tlv_length < tlv_temp_length)
      {
        UIM_MSG_ERR_0("CSG List TLV data is greater than length");
        return MMGSDI_ERROR;
      }

      /* The PLMN TLV tag is 80 */
      if (tag != 0x80)
      {
        UIM_MSG_ERR_0("Incorrect PLMN tag in TLV");
        return MMGSDI_ERROR;
      }

      /* Check the PLMN size */
      if(length != MMGSDI_PLMN_ID_SIZE)
      {
        UIM_MSG_ERR_1("Incorrect PLMN length, 0x%x", length);
        return MMGSDI_ERROR;
      }

      /* Save the PLMN */
      mmgsdi_memscpy(temp_plmn_id.plmn_id_val,
        sizeof(temp_plmn_id.plmn_id_val),
        tlv_data_ptr,
        MMGSDI_PLMN_ID_SIZE);

      /* Move the pointer to next tag location */
      tlv_data_ptr += length;

      /* Begin parsing the CSG info TLV data */
      while((tlv_data_ptr < csgl_tlv_data_ptr + csgl_tlv_length) &&
            (MMGSDI_SUCCESS == mmgsdi_csg_parse_csgl_tlv(tlv_data_ptr,
                                    rec_data.data_len - (tlv_data_ptr - rec_data.data_ptr),
                                    &tag, &offset, &length, &tlv_data_ptr)) &&
            (index < num_csg_plmn_table_entries))
      {
        /* Check if CSG Information TLV(or CSG Display indicator TLV) is
           greater than length in CSG List TLV */
        tlv_temp_length = tlv_temp_length + offset + length;
        if(csgl_tlv_length < tlv_temp_length)
        {
          UIM_MSG_ERR_0("CSG List TLV data is greater than length");
          return MMGSDI_ERROR;
        }

        if((tag == 0x81) &&
           (length == MMGSDI_CSG_INFO_SIZE))
        {
          /* Populate the global table */
          mmgsdi_memscpy(mmgsdi_csg_plmn_table_ptr[index].plmn_id_val,
                         sizeof(mmgsdi_csg_plmn_table_ptr[index].plmn_id_val),
                         temp_plmn_id.plmn_id_val,
                         MMGSDI_PLMN_ID_SIZE);

          csg_id_from_rec = mmgsdi_csg_get_csg_id(&tlv_data_ptr[2], MMGSDI_CSG_ID_SIZE);

          mmgsdi_csg_plmn_table_ptr[index].csg_id = csg_id_from_rec;
          mmgsdi_csg_plmn_table_ptr[index].csgt_record_num = tlv_data_ptr[0];
          mmgsdi_csg_plmn_table_ptr[index].hnbn_record_num = tlv_data_ptr[1];

          UIM_MSG_HIGH_1("CSG ID in the record is 0x%x", csg_id_from_rec);

          /* Keep track of highest index */
          *highest_index = index;

          /* Move the pointer to next tag location */
          tlv_data_ptr += length;

          /* In case a PLMN has more than one CSG ID associated
             copy the PLMN and CSG info at the next index */
          index++;
        }
        else if((file == MMGSDI_USIM_OCSGL) &&
                (tag == 0x82) &&
                (index > 0) &&
                (length == MMGSDI_CSG_DISP_IND_SIZE))
        {
          /* The optional display indicator tag is present.
             This belongs at the previous index in the table */
          mmgsdi_csg_plmn_table_ptr[index - 1].csg_display_indicator = tlv_data_ptr[0];

          /* Move the pointer to next tag location */
          tlv_data_ptr += length;
        }
        else
        {
          UIM_MSG_ERR_0("Incorrect CSG list tag in TLV");
          return MMGSDI_ERROR;
        }
      }
    } /* End parsing the PLMN TLV data */
    else
    {
      UIM_MSG_ERR_0("Incorrect CSGL TLV");
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }
  } /* End parsing a CSGL TLV */

  return mmgsdi_status;
} /* mmgsdi_csg_parse_csgl_and_populate_table */


/*==========================================================================
FUNCTION MMGSDI_CSG_LOOKUP_HNBN_FROM_CSGL

DESCRIPTION
  This function looks up for the HNB name's record number for a PLMN and
  CSG ID from a CSGL file (EF-ACGSL or EF-OCSGL).

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_csg_lookup_hnbn_from_csgl(
  mmgsdi_session_id_type session_id,
  mmgsdi_file_enum_type  file,
  mmgsdi_plmn_id_type    plmn_id,
  mmgsdi_csg_id_type     csg_id,
  mmgsdi_rec_num_type  * hnbn_rec_num)
{
  mmgsdi_return_enum_type     mmgsdi_status             = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type  * int_app_info_ptr          = NULL;
  mmgsdi_session_info_type  * session_info_ptr          = NULL;
  mmgsdi_rec_num_type         rec_num                   = 0;
  mmgsdi_rec_num_type         total_records             = 0;
  uint32                      data_len                  = 0;
  uint16                      index                     = 0;
  uint16                      highest_index_in_table    = 0;
  mmgsdi_access_type          access;
  mmgsdi_data_type            csgl_data;
  mmgsdi_csg_plmn_info_type   mmgsdi_csg_plmn_table[MMGSDI_MAX_CSG_INFO];
  mmgsdi_plmn_id_type         plmn_id_from_tbl;

  memset(plmn_id_from_tbl.plmn_id_val, 0xFF, MMGSDI_PLMN_ID_SIZE);
  memset(mmgsdi_csg_plmn_table, 0x00,
         MMGSDI_MAX_CSG_INFO * sizeof(mmgsdi_csg_plmn_info_type));

  memset(&access, 0x00, sizeof(mmgsdi_access_type));
  access.access_method  = MMGSDI_EF_ENUM_ACCESS;
  access.file.file_enum = file;

  csgl_data.data_len = 0;
  csgl_data.data_ptr = NULL;

  /* Check whether session type is either GW PRI or GW SEC*/
  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    session_id,
                    NULL,
                    NULL,
                    &int_app_info_ptr,
                    NULL,
                    &session_info_ptr,
                    NULL);
  if((MMGSDI_SUCCESS != mmgsdi_status) ||
     (int_app_info_ptr == NULL) ||
     (session_info_ptr == NULL))
  {
    UIM_SELECTIVE_MSG_ERR_0(UIM_LOG_EONS, "Session app info error");
    return MMGSDI_ERROR;
  }

  if (file == MMGSDI_USIM_ACSGL)
  {
    total_records = int_app_info_ptr->eons_info.num_of_acsgl_rec;
  }
  else if (file == MMGSDI_USIM_OCSGL)
  {
    total_records = int_app_info_ptr->eons_info.num_of_ocsgl_rec;
  }
  else
  {
    UIM_SELECTIVE_MSG_ERR_1(UIM_LOG_EONS, "Unexpected file to read, file: 0x%x",
                            file);
    return MMGSDI_ERROR;
  }

  if(total_records > 0)
  {
    mmgsdi_status = mmgsdi_cache_get_record_len(session_id,
                                                access, 1,
                                                &csgl_data.data_len);
    if((mmgsdi_status != MMGSDI_SUCCESS) ||
       (csgl_data.data_len == 0))
    {
      UIM_SELECTIVE_MSG_HIGH_2(UIM_LOG_EONS, "Rec 0x%x of file 0x%x not found in cache",
                               1, file);
      return MMGSDI_ERROR;
    }

    data_len = int32touint32(csgl_data.data_len);

    /* Malloc here as all the records are of same length */
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(csgl_data.data_ptr, data_len);
    if(csgl_data.data_ptr == NULL)
    {
      return MMGSDI_ERROR;
    }
  }

  for (rec_num = 1; rec_num <= total_records; rec_num++)
  {

    /*Reset the index and table data */
    index = 0;
    memset(mmgsdi_csg_plmn_table, 0x00,
         MMGSDI_MAX_CSG_INFO * sizeof(mmgsdi_csg_plmn_info_type));

    mmgsdi_status = mmgsdi_cache_get_record(session_id, access,
                                            rec_num, &csgl_data);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_SELECTIVE_MSG_HIGH_2(UIM_LOG_EONS,
                               "Rec 0x%x data of file 0x%x not found in cache",
                               rec_num, file);
      continue;
    }

    /* Skip if it is empty record */
    if(csgl_data.data_ptr[0] != 0xA0)
    {
      continue;
    }

    /* Parse CSGL record*/
    mmgsdi_status = mmgsdi_csg_parse_csgl_and_populate_table(file,
                                                             csgl_data,
                                                             &highest_index_in_table,
                                                             mmgsdi_csg_plmn_table,
                                                             MMGSDI_MAX_CSG_INFO);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      continue;
    }

    /* Loop through the table */
    while((index <= highest_index_in_table) &&
          (index < MMGSDI_MAX_CSG_INFO))
    {
      mmgsdi_memscpy(plmn_id_from_tbl.plmn_id_val,
                     sizeof(plmn_id_from_tbl.plmn_id_val),
                     mmgsdi_csg_plmn_table[index].plmn_id_val,
                     MMGSDI_PLMN_ID_SIZE);

      if(mmgsdi_eons_compare_sim_plmn_with_nw_plmn(plmn_id_from_tbl, plmn_id))
      {
        if(csg_id == mmgsdi_csg_plmn_table[index].csg_id)
        {
          UIM_SELECTIVE_MSG_HIGH_1(UIM_LOG_EONS, "Found CSG ID in file 0x%x", file);
          *hnbn_rec_num = mmgsdi_csg_plmn_table[index].hnbn_record_num;
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(csgl_data.data_ptr);
          return MMGSDI_SUCCESS;
        }
      }
      index++;
    }
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(csgl_data.data_ptr);
  return MMGSDI_ERROR;
} /* mmgsdi_csg_lookup_hnbn_from_csgl */


/*==========================================================================
FUNCTION MMGSDI_EONS_RUN_ALGO_CSG

DESCRIPTION
  Function provides the home node(b) name.
  For the home node(b) it runs the below name lookup algorithm.
  EONS algorithm for HNB name source has the following priority,
  1. Provide the name from EF-HNBN or EF-OHNBN
  2. NITZ information Look up, NITZ info is received in CM SS Event
  3. If the HNB name is not retrieved, it provides the last known
     RPLMN info for the SPN and Operator name string

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_run_algo_csg(
  mmgsdi_eons_call_stack_enum_type  stack_type,
  mmgsdi_session_id_type            session_id,
  mmgsdi_eons_trigger_enum_type     eons_trigger,
  mmgsdi_plmn_id_type             * plmn_csg_id_ptr,
  mmgsdi_list_plmn_name_type      * plmn_name_list_ptr,
  uint32                            index,
  boolean                           plmn_id_lac_rat_changed
)
{
  mmgsdi_return_enum_type                 mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_int_app_info_type              * int_app_info_ptr = NULL;
  mmgsdi_session_info_type              * session_info_ptr = NULL;
  mmgsdi_csg_id_type                      csg_id           = MMGSDI_EONS_INVALID_CSG_ID;
  mmgsdi_rec_num_type                     hnbn_rec         = 0;
  mmgsdi_data_type                        hnbn_data;
  mmgsdi_plmn_id_type                     plmn_id;
  mmgsdi_eons_nitz_data_type            * nitz_info_ptr    = NULL;
  mmgsdi_eons_per_stack_rplmn_info_type * rplmn_info_ptr   = NULL;
  mmgsdi_nv_context_type                  efs_index        = MMGSDI_NV_CONTEXT_INVALID;

  if((MMGSDI_SESSION_ID_UNASSIGNED == mmgsdi_util_is_session_id_valid(session_id)) ||
     (plmn_csg_id_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }

  if(eons_trigger != MMGSDI_EONS_TRIGGER_REFRESH_FCN &&
     eons_trigger != MMGSDI_EONS_TRIGGER_SS_EVENT &&
     eons_trigger != MMGSDI_EONS_TRIGGER_API_CALL)
  {
    return MMGSDI_ERROR;
  }

  memset(plmn_id.plmn_id_val, 0xFF, MMGSDI_PLMN_ID_SIZE);

  /* Check whether session type is either GW PRI or GW SEC*/
  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    session_id,
                    NULL,
                    NULL,
                    &int_app_info_ptr,
                    NULL,
                    &session_info_ptr,
                    NULL);
  if((MMGSDI_SUCCESS != mmgsdi_status) ||
     (int_app_info_ptr == NULL) ||
     (int_app_info_ptr->app_data.app_type != MMGSDI_APP_USIM))
  {
    return MMGSDI_ERROR;
  }

  /* If the CSG feature is disable, return */
  if((mmgsdi_util_get_efs_item_index(session_id, &efs_index) ==
        MMGSDI_SUCCESS) &&
     (mmgsdi_nv_get_external_feature_status(
        MMGSDI_EXT_FEAT_NAS_CFG_FEATURE_CSG, efs_index) == MMGSDI_FEATURE_DISABLED))
  {
    return MMGSDI_NOT_SUPPORTED;
  }

  /* PLMN and CSG IDs are passed in */
  mmgsdi_memscpy(plmn_id.plmn_id_val,
                 sizeof(plmn_id.plmn_id_val),
                 plmn_csg_id_ptr->plmn_id_val,
                 MMGSDI_PLMN_ID_SIZE);
  csg_id = plmn_csg_id_ptr->csg_id;

  hnbn_data.data_len = 0;
  hnbn_data.data_ptr = NULL;

  UIM_SELECTIVE_MSG_HIGH_1(UIM_LOG_EONS, "In mmgsdi_eons_run_algo_csg, CSG ID: 0x%x",
                           csg_id);

  /* Look up in EFs: ACSGL/HNBN or OCSGL/OHNBN */

  /* First check in EF-ACSGL */
  mmgsdi_status = mmgsdi_csg_lookup_hnbn_from_csgl(
                           session_id,
                           MMGSDI_USIM_ACSGL,
                           plmn_id,
                           csg_id,
                           &hnbn_rec);
  if ((mmgsdi_status == MMGSDI_SUCCESS) &&
      (hnbn_rec != 0))
  {
    /* PLMN and CSG ID are found in EF-ACSGL. Get the HBN name */
    UIM_SELECTIVE_MSG_HIGH_1(UIM_LOG_EONS, "Retrieving HNB name from the rec, 0x%x",
                             hnbn_rec);
    mmgsdi_status = mmgsdi_csg_get_hnb_name(session_id, MMGSDI_USIM_HNBN,
                                              hnbn_rec, &hnbn_data);
  }
  else if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Next check in EF-OCSGL */
    mmgsdi_status = mmgsdi_csg_lookup_hnbn_from_csgl(
                             session_id,
                             MMGSDI_USIM_OCSGL,
                             plmn_id,
                             csg_id,
                             &hnbn_rec);
    if ((mmgsdi_status == MMGSDI_SUCCESS) &&
        (hnbn_rec != 0))
    {
      UIM_SELECTIVE_MSG_HIGH_1(UIM_LOG_EONS, "Retrieving OHNB name from the rec, 0x%x",
                               hnbn_rec);
      mmgsdi_status = mmgsdi_csg_get_hnb_name(session_id, MMGSDI_USIM_OHNBN,
                                                hnbn_rec, &hnbn_data);
    }
  }

  if(eons_trigger == MMGSDI_EONS_TRIGGER_SS_EVENT)
  {
    if (stack_type == MMGSDI_EONS_FIRST_CALL_STACK)
    {
      nitz_info_ptr  = &(int_app_info_ptr->eons_info.ss_info.first_stack_ss.nitz_info);
      rplmn_info_ptr = &(int_app_info_ptr->eons_info.rplmn_info.first_stack_rplmn);
    }
    else
    {
      nitz_info_ptr  = &(int_app_info_ptr->eons_info.ss_info.sec_stack_ss.nitz_info);
      rplmn_info_ptr = &(int_app_info_ptr->eons_info.rplmn_info.sec_stack_rplmn);
    }
  }

  /* HNB name is retrieved from the card */
  if ((hnbn_data.data_len > 0)     &&
      (hnbn_data.data_ptr != NULL) &&
      (mmgsdi_status == MMGSDI_SUCCESS))
  {
    UIM_SELECTIVE_MSG_HIGH_1(UIM_LOG_EONS,
                   "HNB name is retrieved from the file, length 0x%x",
                   hnbn_data.data_len);

    /* Save it in CSG info, if SS event was received */
    if(eons_trigger == MMGSDI_EONS_TRIGGER_SS_EVENT)
    {
      if((hnbn_data.data_len > 0) &&
         (hnbn_data.data_len <= SYS_HNB_NAME_LEN))
      {
        rplmn_info_ptr->csg_info.hnb_name.length =
          int32touint8(hnbn_data.data_len);

        mmgsdi_memscpy(rplmn_info_ptr->csg_info.hnb_name.name,
                       rplmn_info_ptr->csg_info.hnb_name.length,
                       hnbn_data.data_ptr,
                       rplmn_info_ptr->csg_info.hnb_name.length);
      }
    }

    if (plmn_name_list_ptr->plmn_info_ptr != NULL)
    {
      plmn_name_list_ptr->plmn_info_ptr[index].plmn_long_name.plmn_name.eons_data.data_len =
        hnbn_data.data_len;
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
        plmn_name_list_ptr->plmn_info_ptr[index].plmn_long_name.plmn_name.eons_data.data_ptr,
        hnbn_data.data_len);
      if(plmn_name_list_ptr->plmn_info_ptr[index].plmn_long_name.plmn_name.eons_data.data_ptr == NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(hnbn_data.data_ptr);
        return MMGSDI_ERROR;
      }

      mmgsdi_memscpy(plmn_name_list_ptr->plmn_info_ptr[index].plmn_long_name.plmn_name.eons_data.data_ptr,
                     hnbn_data.data_len,
                     hnbn_data.data_ptr,
                     hnbn_data.data_len);
      /* Encoding type: UCS2_PACKED or UCS2 normal */
      if(hnbn_data.data_ptr[MMGSDI_EONS_CSG_DATA_ENC_BYTE_POS] == MMGSDI_UCS2_ENCODING_TYPE_1 ||
         hnbn_data.data_ptr[MMGSDI_EONS_CSG_DATA_ENC_BYTE_POS] == MMGSDI_UCS2_ENCODING_TYPE_2 ||
         hnbn_data.data_ptr[MMGSDI_EONS_CSG_DATA_ENC_BYTE_POS] == MMGSDI_UCS2_ENCODING_TYPE_3)
      {
        plmn_name_list_ptr->plmn_info_ptr[index].plmn_long_name.plmn_name.eons_encoding =
          MMGSDI_EONS_ENC_UCS2_PACKED;
      }
      else
      {
        plmn_name_list_ptr->plmn_info_ptr[index].plmn_long_name.plmn_name.eons_encoding =
          MMGSDI_EONS_ENC_UCS2;
      }
      if(eons_trigger == MMGSDI_EONS_TRIGGER_SS_EVENT)
      {
        rplmn_info_ptr->eons_method = MMGSDI_EONS_METHOD_CSG;
      }
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(hnbn_data.data_ptr);
      return MMGSDI_SUCCESS;
    }

    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(hnbn_data.data_ptr);
  }
  else
  {
    /* Reset the status, in case the corresponding HNBN records
       are not mapped */
    mmgsdi_status = MMGSDI_ERROR;
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(hnbn_data.data_ptr);
  }

  /* HNB name is not retrieved from the card, so look up
     for the name in SS event */

  /* If the SS event is received with valid CSG info,
     look up for the HNB name from the SS event data.
     The SS event data was saved in NITZ info */
  if (eons_trigger == MMGSDI_EONS_TRIGGER_SS_EVENT)
  {
    /* Check if the ss event has the hnbn name */
    if(nitz_info_ptr->csg_info.csg_id != MMGSDI_EONS_INVALID_CSG_ID   &&
       nitz_info_ptr->csg_info.hnb_name.length > 0                    &&
       plmn_name_list_ptr->plmn_info_ptr != NULL)
    {
      plmn_name_list_ptr->plmn_info_ptr[index].plmn_long_name.plmn_name.eons_data.data_len =
        (int32)(nitz_info_ptr->csg_info.hnb_name.length);

      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
        plmn_name_list_ptr->plmn_info_ptr[index].plmn_long_name.plmn_name.eons_data.data_ptr,
        nitz_info_ptr->csg_info.hnb_name.length);
      if(plmn_name_list_ptr->plmn_info_ptr[index].plmn_long_name.plmn_name.eons_data.data_ptr == NULL)
      {
        return MMGSDI_ERROR;
      }

      mmgsdi_memscpy(
        plmn_name_list_ptr->plmn_info_ptr[index].plmn_long_name.plmn_name.eons_data.data_ptr,
        nitz_info_ptr->csg_info.hnb_name.length,
        nitz_info_ptr->csg_info.hnb_name.name,
        nitz_info_ptr->csg_info.hnb_name.length);
      /* Understanding is that NITZ csg data doesn't contain encoding info
         in its first byte. So, always set the encoding type to normal UCS2
         for CSG PLMN */
      plmn_name_list_ptr->plmn_info_ptr[index].plmn_long_name.plmn_name.eons_encoding =
        MMGSDI_EONS_ENC_UCS2;
      rplmn_info_ptr->eons_method = MMGSDI_EONS_METHOD_CSG;

      return MMGSDI_SUCCESS;
    }

    /* WE tried everything, still no Hnb name. Fall back to last RPLMN info */
    if(plmn_id_lac_rat_changed == FALSE              &&
       nitz_info_ptr->long_name_available == FALSE   &&
       nitz_info_ptr->short_name_available == FALSE)
    {
      UIM_SELECTIVE_MSG_HIGH_0(UIM_LOG_EONS,
                               "HNB name is not got from the nitz or card. Use RPLMN info");
      mmgsdi_status = mmgsdi_eons_send_last_known_rplmn_info(stack_type,
                                                             plmn_name_list_ptr,
                                                             int_app_info_ptr,
                                                             index);
    }
  }
  else
  {
    mmgsdi_status = MMGSDI_ERROR;
  }
  return mmgsdi_status;
} /* mmgsdi_eons_run_algo_csg */


/* ==========================================================================
FUNCTION: MMGSDI_CSG_READ_RECORD_SYNC

DESCRIPTION:
  This function performs a synchronous read of a record in file

DEPENDENCIES:
  NONE

LIMITATIONS:
  NONE

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS:
  NONE
==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_csg_read_record_sync(
  mmgsdi_session_id_type  session_id,
  mmgsdi_file_enum_type   file_enum,
  mmgsdi_rec_num_type     rec_num
)
{
  mmgsdi_return_enum_type               mmgsdi_status     = MMGSDI_SUCCESS;
  mmgsdi_client_id_type                 client_id         = MMGSDI_CLIENT_ID_ZERO;
  mmgsdi_slot_id_enum_type              slot_id           = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_read_req_type               *  read_req_ptr      = NULL;
  mmgsdi_read_cnf_type               *  read_cnf_ptr      = NULL;

  UIM_MSG_MED_0("mmgsdi_csg_read_record_sync");

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    session_id,
                    &slot_id,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL);
  if((mmgsdi_status != MMGSDI_SUCCESS) ||
     (slot_id == MMGSDI_MAX_SLOT_ID_ENUM))
  {
    UIM_MSG_ERR_0("Unable to get the slot");
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_client_id(session_id, &client_id);
  if((mmgsdi_status != MMGSDI_SUCCESS) ||
     (client_id == MMGSDI_CLIENT_ID_ZERO))
  {
    UIM_MSG_ERR_0("Unable to get the client id");
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(read_req_ptr,
                                     sizeof(mmgsdi_read_req_type));
  if(read_req_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  read_req_ptr->request_header.client_id         = client_id;
  read_req_ptr->request_header.session_id        = session_id;
  read_req_ptr->request_header.request_type      = MMGSDI_READ_REQ;
  read_req_ptr->request_header.orig_request_type = MMGSDI_READ_REQ;
  read_req_ptr->request_header.request_len       = sizeof(mmgsdi_read_req_type);
  read_req_ptr->request_header.payload_len       = read_req_ptr->request_header.request_len -
                                                     uint32toint32(sizeof(mmgsdi_read_req_type));
  read_req_ptr->request_header.slot_id           = slot_id;
  read_req_ptr->request_header.client_data       = 0;
  read_req_ptr->request_header.response_cb       = NULL;
  read_req_ptr->access.access_method             = MMGSDI_EF_ENUM_ACCESS;
  read_req_ptr->access.file.file_enum            = file_enum;
  read_req_ptr->file_type                        = MMGSDI_LINEAR_FIXED_FILE;
  read_req_ptr->rec_num                          = rec_num;
  read_req_ptr->data_len                         = 0;/*Entire Record*/

  mmgsdi_status = mmgsdi_uim_uicc_read_record(read_req_ptr,
                                              TRUE,
                                              &read_cnf_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS || read_cnf_ptr == NULL)
  {
    UIM_MSG_ERR_2("Read failed for record 0x%x of file 0x%x",
                  rec_num, file_enum);
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_req_ptr);

  /* Free the content of the cnf before freeing the cnf_ptr itself */
  mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)((void*)read_cnf_ptr));
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_cnf_ptr);

  return mmgsdi_status;
} /* mmgsdi_csg_read_record_sync */


/* ==========================================================================
FUNCTION: MMGSDI_CSG_SELECT_SYNC

DESCRIPTION:
  This function performs a synchronous select of a file and provides
  the total records count and length of each record.

DEPENDENCIES:
  NONE

LIMITATIONS:
  NONE

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS:
  NONE
==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_csg_select_sync(
  mmgsdi_session_id_type   session_id,
  mmgsdi_file_enum_type    file_enum,
  mmgsdi_rec_num_type    * rec_cnt_ptr,
  mmgsdi_len_type        * rec_len_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status       = MMGSDI_SUCCESS;
  mmgsdi_client_id_type              client_id           = MMGSDI_CLIENT_ID_ZERO;
  mmgsdi_slot_id_enum_type           slot_id             = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_get_file_attr_req_type  *   get_file_attr_ptr   = NULL;
  mmgsdi_get_file_attr_cnf_type  *   cnf_ptr             = NULL;

  if((rec_cnt_ptr == NULL) ||
     (rec_len_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }

  UIM_MSG_MED_0("mmgsdi_csg_select_sync");

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    session_id,
                    &slot_id,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL);
  if((mmgsdi_status != MMGSDI_SUCCESS) ||
     (slot_id == MMGSDI_MAX_SLOT_ID_ENUM))
  {
    UIM_MSG_ERR_0("Unable to get the slot");
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_client_id(session_id, &client_id);
  if((mmgsdi_status != MMGSDI_SUCCESS) ||
     (client_id == MMGSDI_CLIENT_ID_ZERO))
  {
    UIM_MSG_ERR_0("Unable to get the client id");
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(get_file_attr_ptr,
                                     sizeof(mmgsdi_get_file_attr_req_type));
  if(get_file_attr_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  get_file_attr_ptr->request_header.client_id         = client_id;
  get_file_attr_ptr->request_header.session_id        = session_id;
  get_file_attr_ptr->request_header.request_type      = MMGSDI_GET_FILE_ATTR_REQ;
  get_file_attr_ptr->request_header.orig_request_type = MMGSDI_GET_FILE_ATTR_REQ;
  get_file_attr_ptr->request_header.request_len       = sizeof(mmgsdi_get_file_attr_req_type);
  get_file_attr_ptr->request_header.payload_len       = 0;
  get_file_attr_ptr->request_header.slot_id           = slot_id;
  get_file_attr_ptr->request_header.client_data       = 0;
  get_file_attr_ptr->request_header.response_cb       = NULL;
  get_file_attr_ptr->access.access_method             = MMGSDI_EF_ENUM_ACCESS;
  get_file_attr_ptr->access.file.file_enum            = file_enum;
  get_file_attr_ptr->int_client_data                  = 0;
  get_file_attr_ptr->activate_aid                     = TRUE;

  mmgsdi_status = mmgsdi_uim_uicc_select(get_file_attr_ptr,
                                         TRUE,
                                         &cnf_ptr,
                                         TRUE);

  MMGSDIUTIL_TMC_MEM_FREE(get_file_attr_ptr);

  if(cnf_ptr == NULL)
  {
    UIM_MSG_ERR_0("Select conf. ptr is NULL");
    return MMGSDI_ERROR;
  }

  if(mmgsdi_status != MMGSDI_SUCCESS ||
     cnf_ptr->file_attrib.file_type != MMGSDI_LINEAR_FIXED_FILE ||
     cnf_ptr->file_attrib.file_info.linear_fixed_file.num_of_rec == 0)
  {
    UIM_MSG_ERR_1("Select failed for file, 0x%x", file_enum);

    /* Free the content of the cnf before freeing the cnf_ptr itself */
    mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(cnf_ptr);
    return MMGSDI_ERROR;
  }

  *rec_cnt_ptr = cnf_ptr->file_attrib.file_info.linear_fixed_file.num_of_rec;
  *rec_len_ptr = cnf_ptr->file_attrib.file_info.linear_fixed_file.rec_len;

  /* Free the content of the cnf before freeing the cnf_ptr itself */
  mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cnf_ptr);
  return MMGSDI_SUCCESS;
} /* mmgsdi_csg_select_sync */


/* ==========================================================================
FUNCTION: MMGSDI_CSG_SEARCH_RECORDS_SYNC

DESCRIPTION:
  This function performs a synchronous search in a file and provides
  the record numbers that are empty.
  The function allocates the memory if empty records are found in the
  file. In that case, the caller of the function is responsible to
  free it.

DEPENDENCIES:
  NONE

LIMITATIONS:
  NONE

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS:
  NONE
==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_csg_search_records_sync(
  mmgsdi_session_id_type      session_id,
  mmgsdi_file_enum_type       file_enum,
  mmgsdi_len_type             rec_len,
  mmgsdi_data_type          * search_resp_data_ptr
)
{
  mmgsdi_return_enum_type             mmgsdi_status  = MMGSDI_SUCCESS;
  mmgsdi_client_id_type               client_id      = MMGSDI_CLIENT_ID_ZERO;
  mmgsdi_slot_id_enum_type            slot_id        = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_search_req_type             *search_req_ptr = NULL;
  mmgsdi_search_cnf_type             *search_cnf_ptr = NULL;

  if((search_resp_data_ptr == NULL) ||
     (rec_len <= 0))
  {
    UIM_MSG_ERR_0("Incorrect input parameters");
    return MMGSDI_ERROR;
  }

  UIM_MSG_MED_0("mmgsdi_csg_search_records_sync");

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    session_id,
                    &slot_id,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL);
  if((mmgsdi_status != MMGSDI_SUCCESS) ||
     (slot_id == MMGSDI_MAX_SLOT_ID_ENUM))
  {
    UIM_MSG_ERR_0("Unable to get the slot");
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_client_id(session_id, &client_id);
  if(mmgsdi_status != MMGSDI_SUCCESS ||
     client_id == MMGSDI_CLIENT_ID_ZERO)
  {
    UIM_MSG_ERR_0("Unable to get the client id");
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(search_req_ptr,
                                     sizeof(mmgsdi_search_req_type));

  /* memory allocation failed for search req ptr */
  if (search_req_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* Determine record numbers which are empty */
  search_req_ptr->request_header.client_id           = client_id;
  search_req_ptr->request_header.session_id          = session_id;
  search_req_ptr->request_header.request_type        = MMGSDI_SEARCH_REQ;
  search_req_ptr->request_header.orig_request_type   = MMGSDI_SEARCH_REQ;
  search_req_ptr->request_header.request_len         = sizeof(mmgsdi_search_req_type);
  search_req_ptr->request_header.payload_len         = 0;
  search_req_ptr->request_header.slot_id             = slot_id;
  search_req_ptr->request_header.client_data         = 0;
  search_req_ptr->request_header.response_cb         = NULL;
  search_req_ptr->access.access_method               = MMGSDI_EF_ENUM_ACCESS;
  search_req_ptr->access.file.file_enum              = file_enum;
  search_req_ptr->search_type                        = MMGSDI_UICC_SIMPLE_SEARCH;

  search_req_ptr->rec_num                            = 1;
  search_req_ptr->search_direction                   = MMGSDI_SEARCH_FORWARD_FROM_REC_NUM;
  search_req_ptr->seek_direction                     = MMGSDI_SEEK_BEGINNING_FORWARDS;
  search_req_ptr->enhanced_search_offset.offset_data = 0;
  search_req_ptr->enhanced_search_offset.offset_type = MMGSDI_SEARCH_OPTION_NONE;
  search_req_ptr->data.data_len                      = rec_len;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(search_req_ptr->data.data_ptr,
                                     search_req_ptr->data.data_len);
  if(search_req_ptr->data.data_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE(search_req_ptr);
    return MMGSDI_ERROR;
  }

  memset(search_req_ptr->data.data_ptr, 0xFF, search_req_ptr->data.data_len);

  mmgsdi_status = mmgsdi_uim_uicc_search_record(search_req_ptr,
                                                TRUE,
                                                &search_cnf_ptr);

  MMGSDIUTIL_TMC_MEM_FREE(search_req_ptr->data.data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(search_req_ptr);

  if(search_cnf_ptr == NULL)
  {
    UIM_MSG_ERR_0("search cnf ptr is NULL");
    return MMGSDI_ERROR;
  }

  if(search_cnf_ptr->access.access_method != MMGSDI_EF_ENUM_ACCESS ||
     search_cnf_ptr->access.file.file_enum != file_enum)
  {
    UIM_MSG_ERR_0("search failed: access method or file enum does not match");
    mmgsdi_status = MMGSDI_ERROR;
  }

  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    search_resp_data_ptr->data_len =
              search_cnf_ptr->searched_record_nums.data_len;

    if(search_resp_data_ptr->data_len > 0)
    {
      /* copy the search response */
      /* caller is reponsible to free this data */
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(search_resp_data_ptr->data_ptr,
                                         search_resp_data_ptr->data_len);

      /* memory allocation failed for search req ptr */
      if (search_resp_data_ptr->data_ptr != NULL)
      {
        mmgsdi_memscpy(search_resp_data_ptr->data_ptr,
                       search_resp_data_ptr->data_len,
                       search_cnf_ptr->searched_record_nums.data_ptr,
                       search_cnf_ptr->searched_record_nums.data_len);
      }
      else
      {
        mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }
    }
  }

  /* free the content of the cnf before freeing the cnf_ptr itself */
  mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)search_cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(search_cnf_ptr);
  return mmgsdi_status;
} /* mmgsdi_csg_search_records_sync */


/*==========================================================================
FUNCTION MMGSDI_CSG_REFRESH_CSGL_CACHE

DESCRIPTION
  This function clears cache and reads the CSG EF which got refreshed. It
  then updates the cache of the EF refreshed.

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_csg_refresh_csgl_cache(
  mmgsdi_session_id_type session_id,
  mmgsdi_client_id_type  client_id,
  mmgsdi_file_enum_type  file
)
{
  mmgsdi_return_enum_type            mmgsdi_status             = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type       *   int_app_info_ptr          = NULL;
  mmgsdi_session_info_type       *   session_info_ptr          = NULL;
  mmgsdi_rec_num_type                total_recs                = 0;
  mmgsdi_rec_num_type                rec_cnt                   = 0;
  mmgsdi_len_type                    rec_len                   = 0;
  mmgsdi_len_type                    empty_rec                 = 0;
  boolean                            cache_updated             = FALSE;
  mmgsdi_access_type                 file_access;
  mmgsdi_data_type                   invalid_record_data;
  mmgsdi_data_type                   empty_data_buf;

  memset(&file_access, 0x00, sizeof(mmgsdi_access_type));
  memset(&invalid_record_data, 0x00, sizeof(mmgsdi_data_type));
  memset(&empty_data_buf, 0x00, sizeof(mmgsdi_data_type));

  UIM_MSG_MED_0("mmgsdi_csg_refresh_csgl_cache");

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    session_id,
                    NULL,
                    NULL,
                    &int_app_info_ptr,
                    NULL,
                    &session_info_ptr,
                    NULL);
  if((mmgsdi_status != MMGSDI_SUCCESS) ||
     (int_app_info_ptr == NULL) ||
     (session_info_ptr == NULL) ||
     (session_info_ptr->session_type != MMGSDI_GW_PROV_PRI_SESSION &&
      session_info_ptr->session_type != MMGSDI_GW_PROV_SEC_SESSION &&
      session_info_ptr->session_type != MMGSDI_GW_PROV_TER_SESSION))
  {
    UIM_MSG_ERR_0("Unable to get session app info");
    return MMGSDI_ERROR;
  }

  file_access.access_method  = MMGSDI_EF_ENUM_ACCESS;
  file_access.file.file_enum = file;

  switch(file)
  {
    case MMGSDI_USIM_ACSGL:
      if(int_app_info_ptr->eons_info.num_of_acsgl_rec > 0)
      {
        total_recs = int_app_info_ptr->eons_info.num_of_acsgl_rec;
      }
      int_app_info_ptr->eons_info.is_get_file_attr_acsgl_done = FALSE;
      int_app_info_ptr->eons_info.len_of_acsgl_rec = 0;
      int_app_info_ptr->eons_info.num_of_acsgl_rec = 0;
      break;

    case MMGSDI_USIM_CSGT:
      if(int_app_info_ptr->eons_info.num_of_csgt_rec > 0)
      {
        total_recs = int_app_info_ptr->eons_info.num_of_csgt_rec;
      }
      int_app_info_ptr->eons_info.is_get_file_attr_csgt_done = FALSE;
      int_app_info_ptr->eons_info.len_of_csgt_rec = 0;
      int_app_info_ptr->eons_info.num_of_csgt_rec = 0;
      break;

    case MMGSDI_USIM_HNBN:
      if(int_app_info_ptr->eons_info.num_of_hnbn_rec > 0)
      {
        total_recs = int_app_info_ptr->eons_info.num_of_hnbn_rec;
      }
      int_app_info_ptr->eons_info.is_get_file_attr_hnbn_done = FALSE;
      int_app_info_ptr->eons_info.len_of_hnbn_rec = 0;
      int_app_info_ptr->eons_info.num_of_hnbn_rec = 0;
      break;

    case MMGSDI_USIM_OCSGL:
      if(int_app_info_ptr->eons_info.num_of_ocsgl_rec > 0)
      {
        total_recs = int_app_info_ptr->eons_info.num_of_ocsgl_rec;
      }
      int_app_info_ptr->eons_info.is_get_file_attr_ocsgl_done = FALSE;
      int_app_info_ptr->eons_info.len_of_ocsgl_rec = 0;
      int_app_info_ptr->eons_info.num_of_ocsgl_rec = 0;
      break;

    case MMGSDI_USIM_OCSGT:
      if(int_app_info_ptr->eons_info.num_of_ocsgt_rec > 0)
      {
        total_recs = int_app_info_ptr->eons_info.num_of_ocsgt_rec;
      }
      int_app_info_ptr->eons_info.is_get_file_attr_ocsgt_done = FALSE;
      int_app_info_ptr->eons_info.len_of_ocsgt_rec = 0;
      int_app_info_ptr->eons_info.num_of_ocsgt_rec = 0;
      break;

    case MMGSDI_USIM_OHNBN:
      if(int_app_info_ptr->eons_info.num_of_ohnbn_rec > 0)
      {
        total_recs = int_app_info_ptr->eons_info.num_of_ohnbn_rec ;
      }
      int_app_info_ptr->eons_info.is_get_file_attr_ohnbn_done = FALSE;
      int_app_info_ptr->eons_info.len_of_ohnbn_rec = 0;
      int_app_info_ptr->eons_info.num_of_ohnbn_rec = 0;
      break;

    default:
      UIM_MSG_HIGH_1("File 0x%x not expected", file);
      return MMGSDI_ERROR;
  }

  if(total_recs > 0)
  {
    for(rec_cnt = 1; rec_cnt <= total_recs; rec_cnt++)
    {
      mmgsdi_status = mmgsdi_cache_delete_record(session_id, file_access, rec_cnt);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_HIGH_2("Failed to delete rec 0x%x of file 0x%x from cache",
                       rec_cnt, file);
      }
    }
  }

  /* get the attributes of the file */
  mmgsdi_status = mmgsdi_csg_select_sync(session_id,
                                         file,
                                         &total_recs,
                                         &rec_len);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_2("select failed, status: 0x%x for file, 0x%x",
                  mmgsdi_status, file);
    return MMGSDI_ERROR;
  }

  if((total_recs == 0) ||
     (rec_len == 0))
  {
    UIM_MSG_ERR_2("total_rec: 0x%x, rec_len: 0x%x",
                  total_recs, rec_len);
    return MMGSDI_ERROR;
  }

  switch(file)
  {
    case MMGSDI_USIM_ACSGL:
      int_app_info_ptr->eons_info.is_get_file_attr_acsgl_done = TRUE;
      int_app_info_ptr->eons_info.len_of_acsgl_rec = rec_len;
      int_app_info_ptr->eons_info.num_of_acsgl_rec = total_recs;
      break;

    case MMGSDI_USIM_CSGT:
      int_app_info_ptr->eons_info.is_get_file_attr_csgt_done = TRUE;
      int_app_info_ptr->eons_info.len_of_csgt_rec = rec_len;
      int_app_info_ptr->eons_info.num_of_csgt_rec = total_recs;
      break;

    case MMGSDI_USIM_HNBN:
      int_app_info_ptr->eons_info.is_get_file_attr_acsgl_done = TRUE;
      int_app_info_ptr->eons_info.len_of_hnbn_rec = rec_len;
      int_app_info_ptr->eons_info.num_of_hnbn_rec = total_recs;
      break;

    case MMGSDI_USIM_OCSGL:
      int_app_info_ptr->eons_info.is_get_file_attr_ocsgl_done = TRUE;
      int_app_info_ptr->eons_info.len_of_ocsgl_rec = rec_len;
      int_app_info_ptr->eons_info.num_of_ocsgl_rec = total_recs;
      break;

    case MMGSDI_USIM_OCSGT:
      int_app_info_ptr->eons_info.is_get_file_attr_ocsgt_done = TRUE;
      int_app_info_ptr->eons_info.len_of_ocsgt_rec = rec_len;
      int_app_info_ptr->eons_info.num_of_ocsgt_rec = total_recs;
      break;

    case MMGSDI_USIM_OHNBN:
      int_app_info_ptr->eons_info.is_get_file_attr_ohnbn_done = TRUE;
      int_app_info_ptr->eons_info.len_of_ohnbn_rec = rec_len;
      int_app_info_ptr->eons_info.num_of_ohnbn_rec = total_recs;
      break;

    default:
      break;
  }

  /* perform the search for empty records */
  mmgsdi_status = mmgsdi_csg_search_records_sync(session_id,
                                                 file,
                                                 rec_len,
                                                 &invalid_record_data);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_2("search in file, 0x%x failed, status: 0x%x",
                   file, mmgsdi_status);
  }

  if((mmgsdi_status == MMGSDI_SUCCESS) &&
     (invalid_record_data.data_len != 0) &&
     (invalid_record_data.data_len <= total_recs))
  {
    UIM_MSG_HIGH_3("file 0x%x has total rec: 0x%x, valid records: 0x%x",
                   file, total_recs, (total_recs - invalid_record_data.data_len));

    /* there is atleast one empty record. so need to update
       the cache with empty data */
    empty_data_buf.data_len = rec_len;
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(empty_data_buf.data_ptr,
                                        empty_data_buf.data_len);
    if(empty_data_buf.data_ptr != NULL)
    {
      memset(empty_data_buf.data_ptr, 0xFF,
             empty_data_buf.data_len);
    }
  }

  for(rec_cnt = 1; rec_cnt <= total_recs; rec_cnt++)
  {
    /* reset the variable */
    cache_updated = FALSE;

    if((invalid_record_data.data_len > 0) &&
       (invalid_record_data.data_ptr != NULL) &&
       (empty_data_buf.data_len > 0) &&
       (empty_data_buf.data_ptr != NULL))
    {
      for(empty_rec = 0; empty_rec < invalid_record_data.data_len;
          empty_rec++)
      {
        if(rec_cnt == invalid_record_data.data_ptr[empty_rec])
        {
          (void)mmgsdi_cache_add_record(session_id,
                                        file_access,
                                        rec_cnt,
                                        &empty_data_buf);
          cache_updated = TRUE;
          break;
        }
      }

      /* cache is updated. no need to read
         the record */
      if(cache_updated)
      {
        continue;
      }
    }

    mmgsdi_status = mmgsdi_csg_read_record_sync(session_id,
                                                file,
                                                rec_cnt);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_1("read record, 0x%x failed", rec_cnt);
      break;
    }
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(empty_data_buf.data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(invalid_record_data.data_ptr);

  UIM_MSG_HIGH_1("csg refresh process status, 0x%x", mmgsdi_status);
  return mmgsdi_status;
} /* mmgsdi_csg_refresh_csgl_cache */


/*==============================================================================
  FUNCTION MMGSDI_CSG_CONSTRUCT_ACSGL_TLV_FROM_TABLE

  DESCRIPTION:
    This function constructs the ACSGL TLV from the table data

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    MMGSDI_RETURN_ENUM_TYPE

  SIDE EFFECTS:
    None
==============================================================================*/
static mmgsdi_return_enum_type mmgsdi_csg_construct_acsgl_tlv_from_table
(
  const mmgsdi_csg_plmn_info_type *  mmgsdi_csg_plmn_table_ptr,
  uint16                             highest_index,
  mmgsdi_data_type                *  acsgl_tlv_ptr
)
{
  uint16      index                     = 0;
  uint16      acsgl_buf_len             = 0;
  uint16      acsgl_tlv_len             = 0;
  uint8       temp_csg_tlv[MMGSDI_MAX_REC_LEN];

  if((acsgl_tlv_ptr == NULL) ||
     (mmgsdi_csg_plmn_table_ptr == NULL) ||
     (acsgl_tlv_ptr->data_ptr == NULL) ||
     (highest_index >= MMGSDI_MAX_CSG_INFO))
  {
    UIM_MSG_ERR_0("Input parameters error");
    return MMGSDI_ERROR;
  }

  memset(temp_csg_tlv, 0x00, MMGSDI_MAX_REC_LEN);
  memset(acsgl_tlv_ptr->data_ptr, 0xFF, acsgl_tlv_ptr->data_len);

  /* Loop through the table */
  for(index = 0; index <= highest_index; index++)
  {
    if(index == 0 || memcmp(mmgsdi_csg_plmn_table_ptr[index - 1].plmn_id_val,
                            mmgsdi_csg_plmn_table_ptr[index].plmn_id_val,
                            MMGSDI_PLMN_ID_SIZE))
    {
      if(index != 0)
      {
        if(acsgl_tlv_len + acsgl_buf_len <= acsgl_tlv_ptr->data_len)
        {
          acsgl_tlv_ptr->data_ptr[acsgl_tlv_len++] = 0xA0;
          if(acsgl_buf_len > 0x7F)
          {
            acsgl_tlv_ptr->data_ptr[acsgl_tlv_len++] = 0x81;
            acsgl_tlv_ptr->data_ptr[acsgl_tlv_len++] = (uint8)acsgl_buf_len;
          }
          else
          {
            acsgl_tlv_ptr->data_ptr[acsgl_tlv_len++] = (uint8)acsgl_buf_len;
          }
          UIM_MSG_HIGH_1("Constructed TLV length, 0x%x", acsgl_buf_len);
          mmgsdi_memscpy(&acsgl_tlv_ptr->data_ptr[acsgl_tlv_len],
                         acsgl_tlv_ptr->data_len - acsgl_tlv_len,
                         temp_csg_tlv,
                         acsgl_buf_len);
          acsgl_tlv_len += acsgl_buf_len;
          memset(temp_csg_tlv, 0x00, MMGSDI_MAX_REC_LEN);

          /* Reset the buffer index */
          acsgl_buf_len = 0;
        }
        else
        {
          UIM_MSG_ERR_3("TLV size exceeded rec length, tlv_len: 0x%x, buf_len: 0x%x"
                        " rec_len: 0x%x",
                        acsgl_tlv_len, acsgl_buf_len, acsgl_tlv_ptr->data_len);
          return MMGSDI_ERROR;
        }
      }

      if(acsgl_buf_len + MMGSDI_PLMN_TLV_SIZE <= MMGSDI_MAX_REC_LEN)
      {
        temp_csg_tlv[acsgl_buf_len++] = 0x80;
        temp_csg_tlv[acsgl_buf_len++] = MMGSDI_PLMN_ID_SIZE;
        mmgsdi_memscpy(&temp_csg_tlv[acsgl_buf_len],
                       sizeof(temp_csg_tlv) - acsgl_buf_len,
                       mmgsdi_csg_plmn_table_ptr[index].plmn_id_val,
                       MMGSDI_PLMN_ID_SIZE);
        acsgl_buf_len += MMGSDI_PLMN_ID_SIZE;
      }
      else
      {
        UIM_MSG_ERR_1("TLV size exceeded buffer size, buf_len: 0x%x",
                      acsgl_buf_len);
        return MMGSDI_ERROR;
      }
    }

    if(acsgl_buf_len + MMGSDI_CSG_INFO_TLV_SIZE <= MMGSDI_MAX_REC_LEN)
    {
      temp_csg_tlv[acsgl_buf_len++] = 0x81;
      temp_csg_tlv[acsgl_buf_len++] = 0x6;
      temp_csg_tlv[acsgl_buf_len++] = int32touint8(mmgsdi_csg_plmn_table_ptr[index].csgt_record_num);
      temp_csg_tlv[acsgl_buf_len++] = int32touint8(mmgsdi_csg_plmn_table_ptr[index].hnbn_record_num);
      temp_csg_tlv[acsgl_buf_len++] = (uint8)(mmgsdi_csg_plmn_table_ptr[index].csg_id >> 19);
      temp_csg_tlv[acsgl_buf_len++] = (uint8)(mmgsdi_csg_plmn_table_ptr[index].csg_id >> 11);
      temp_csg_tlv[acsgl_buf_len++] = (uint8)(mmgsdi_csg_plmn_table_ptr[index].csg_id >> 3);
      temp_csg_tlv[acsgl_buf_len++] = (((uint8)(mmgsdi_csg_plmn_table_ptr[index].csg_id) << 5) | 0x1F);
    }
    else
    {
      UIM_MSG_ERR_1("TLV size exceeded buffer size, buf_len: 0x%x",
                    acsgl_buf_len);
      return MMGSDI_ERROR;
    }

    if(index == highest_index)
    {
      if(acsgl_tlv_len + acsgl_buf_len < acsgl_tlv_ptr->data_len)
      {
        acsgl_tlv_ptr->data_ptr[acsgl_tlv_len++] = 0xA0;
        if(acsgl_buf_len > 0x7F)
        {
          acsgl_tlv_ptr->data_ptr[acsgl_tlv_len++] = 0x81;
          acsgl_tlv_ptr->data_ptr[acsgl_tlv_len++] = (uint8)acsgl_buf_len;
        }
        else
        {
          acsgl_tlv_ptr->data_ptr[acsgl_tlv_len++] = (uint8)acsgl_buf_len;
        }
        UIM_MSG_HIGH_1("Constructed TLV length, 0x%x", acsgl_buf_len);
        mmgsdi_memscpy(&acsgl_tlv_ptr->data_ptr[acsgl_tlv_len],
                       acsgl_tlv_ptr->data_len - acsgl_tlv_len,
                       temp_csg_tlv,
                       acsgl_buf_len);
        acsgl_tlv_len += acsgl_buf_len;
        memset(temp_csg_tlv, 0x00, MMGSDI_MAX_REC_LEN);
      }
      else
      {
        UIM_MSG_ERR_3("TLV size exceeded rec length, tlv_len: 0x%x, buf_len: 0x%x"
                      " rec_len: 0x%x",
                      acsgl_tlv_len, acsgl_buf_len, acsgl_tlv_ptr->data_len);
        return MMGSDI_ERROR;
      }
    }
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_csg_construct_acsgl_tlv_from_table */


/*==============================================================================
  FUNCTION MMGSDI_CSG_UPDATE_EF_RECORD_CACHE

  DESCRIPTION:
    This function updates the cache of a given record and file

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    MMGSDI_RETURN_ENUM_TYPE

  SIDE EFFECTS:
    None
==============================================================================*/
static mmgsdi_return_enum_type mmgsdi_csg_update_ef_record_cache
(
  mmgsdi_session_id_type session_id,
  mmgsdi_access_type     access,
  mmgsdi_rec_num_type    rec_num,
  mmgsdi_data_type       rec_data
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;

  if((rec_data.data_len <= 0) ||
     (rec_data.data_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_cache_delete_record(session_id,
                                             access, rec_num);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_2("Failed to delete rec 0x%x of file 0x%x from cache",
                   rec_num, access.file.file_enum);
    return mmgsdi_status;
  }

  UIM_MSG_HIGH_2("Add rec 0x%x of file 0x%x to cache",
                 rec_num, access.file.file_enum);

  mmgsdi_status = mmgsdi_cache_add_record(session_id, access, rec_num,
                                          &rec_data);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_2("Failed to add rec 0x%x of file 0x%x to cache",
                   rec_num, access.file.file_enum);
  }
  return mmgsdi_status;
} /* mmgsdi_csg_update_ef_record_cache */


/*==============================================================================
  FUNCTION MMGSDI_CSG_ACSGL_TLV_SIZE_IN_REC

  DESCRIPTION:
    This function calculates the size of the ACSGL TLV in the record

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    The size of the tlv.

  SIDE EFFECTS:
    None
==============================================================================*/
static mmgsdi_len_type mmgsdi_csg_acsgl_tlv_size_in_rec
(
  const mmgsdi_csg_plmn_info_type *  mmgsdi_csg_plmn_table_ptr,
  uint16                             highest_index
)
{
  uint16           index                           = 0;
  uint8            num_of_unique_plmns             = 0;
  uint8            num_csg_tlvs_per_plmn           = 0;
  uint8            num_tlvs_with_two_bytes_for_len = 0;
  mmgsdi_len_type  tlv_len                         = 0;

  if(mmgsdi_csg_plmn_table_ptr == NULL)
  {
    UIM_MSG_ERR_0("CSG and PLMN info table is NULL");
    return MMGSDI_MAX_REC_LEN;
  }

  while((index <= highest_index) &&
        (index < MMGSDI_MAX_CSG_INFO))
  {
    if (index > 0)
    {
      if(memcmp(mmgsdi_csg_plmn_table_ptr[index - 1].plmn_id_val,
                mmgsdi_csg_plmn_table_ptr[index].plmn_id_val,
                MMGSDI_PLMN_ID_SIZE))
      {
        num_of_unique_plmns++;
        if(((num_csg_tlvs_per_plmn * MMGSDI_CSG_INFO_TLV_SIZE) + MMGSDI_PLMN_TLV_SIZE) > 0x7F)
        {
          num_tlvs_with_two_bytes_for_len++;
        }
        num_csg_tlvs_per_plmn = 0;
      }
      else
      {
        num_csg_tlvs_per_plmn++;
      }
    }
    index++;
  }

  tlv_len = (((num_of_unique_plmns + 1) * MMGSDI_PLMN_TLV_SIZE) +
             ((highest_index + 1) *  MMGSDI_CSG_INFO_TLV_SIZE)  +
             ((num_of_unique_plmns + 1) * 2) +
             (num_tlvs_with_two_bytes_for_len));

  UIM_MSG_HIGH_1("Total TLV size in record, 0x%x", tlv_len);

  return tlv_len;
} /* mmgsdi_csg_acsgl_tlv_size_in_rec */


/*==============================================================================
  FUNCTION MMGSDI_CSG_CHECK_ACSGL_RECORD_FOR_NEW_CSGID

  DESCRIPTION:
    This function checks if a record has sufficient space to fit a csg tlv or
    both the plmn and csg tlvs

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    TRUE  - If the record has space for the tlv
    FALSE - If the record has no space for the tlv

  SIDE EFFECTS:
    None
==============================================================================*/
static boolean mmgsdi_csg_check_acsgl_record_for_new_csgid
(
  const mmgsdi_csg_plmn_info_type *  mmgsdi_csg_plmn_table_ptr,
  uint16                             highest_index,
  mmgsdi_len_type                    rec_len,
  boolean                            add_plmn_tlv_to_rec
)
{
  mmgsdi_len_type  tlv_len_in_rec = 0;

  if(mmgsdi_csg_plmn_table_ptr == NULL)
  {
    UIM_MSG_ERR_0("CSG and PLMN info table is NULL");
    return FALSE;
  }

  tlv_len_in_rec = mmgsdi_csg_acsgl_tlv_size_in_rec(mmgsdi_csg_plmn_table_ptr,
                                                    highest_index);
  if((!add_plmn_tlv_to_rec) &&
     (rec_len >= (tlv_len_in_rec + MMGSDI_CSG_INFO_TLV_SIZE)))
  {
    return TRUE;
  }
  else if((add_plmn_tlv_to_rec) &&
          (rec_len >= (tlv_len_in_rec + MMGSDI_ACSGL_TLV_SIZE)))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* mmgsdi_csg_check_acsgl_record_for_new_csgid */


/*==============================================================================
  FUNCTION MMGSDI_CSG_FIND_HNBN_RECORD

  DESCRIPTION:
    This function finds the record with the requested home node(b) name. If the
    name is not found in any of the records, it checks if an empty record of
    sufficient length is available.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    record number
    0 - if hnbn is not found or no empty records

  SIDE EFFECTS:
    None
==============================================================================*/
static mmgsdi_rec_num_type mmgsdi_csg_find_hnbn_record
(
  mmgsdi_session_id_type  session_id,
  mmgsdi_data_type        hnb_name_data,
  boolean               * is_new_record
)
{
  mmgsdi_return_enum_type      mmgsdi_status             = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type  *  int_app_info_ptr          = NULL;
  mmgsdi_session_info_type  *  session_info_ptr          = NULL;
  mmgsdi_rec_num_type          rec_num                   = 0;
  mmgsdi_rec_num_type          total_records             = 0;
  mmgsdi_rec_num_type          first_empty_rec           = 0;
  mmgsdi_len_type              rec_len                   = 0;
  uint32                       data_len                  = 0;
  mmgsdi_access_type           access;
  mmgsdi_data_type             hnbn_data;

  memset(&access, 0x00, sizeof(mmgsdi_access_type));
  access.access_method  = MMGSDI_EF_ENUM_ACCESS;
  access.file.file_enum = MMGSDI_USIM_HNBN;

  hnbn_data.data_len = 0;
  hnbn_data.data_ptr = NULL;

  if(is_new_record == NULL)
  {
    UIM_MSG_ERR_0("Input parameters error");
    return 0;
  }

  /* Get the session app info */
  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    session_id,
                    NULL,
                    NULL,
                    &int_app_info_ptr,
                    NULL,
                    &session_info_ptr,
                    NULL);
  if((MMGSDI_SUCCESS != mmgsdi_status) ||
     (int_app_info_ptr == NULL) ||
     (session_info_ptr == NULL))
  {
    UIM_MSG_ERR_0("Session app info error");
    return 0;
  }

  total_records = int_app_info_ptr->eons_info.num_of_hnbn_rec;
  rec_len       = int_app_info_ptr->eons_info.len_of_hnbn_rec;

  if(total_records <= 0)
  {
    UIM_MSG_ERR_0("EF_HNBN has no records");
    return 0;
  }

  /* Check if the record is of sufficient length
     2 additional bytes for TLV Tag and Length byte */
  if(rec_len < (hnb_name_data.data_len + 2))
  {
    UIM_MSG_ERR_0("EF_HNBN record is of insufficient size");
    return 0;
  }

  mmgsdi_status = mmgsdi_cache_get_record_len(session_id,
                                              access, 1,
                                              &hnbn_data.data_len);
  if (mmgsdi_status != MMGSDI_SUCCESS ||
      hnbn_data.data_len == 0)
  {
    UIM_MSG_HIGH_2("Rec 0x%x of file 0x%x not found in cache",
                             1, MMGSDI_USIM_HNBN);
    return 0;
  }

  data_len = int32touint32(hnbn_data.data_len);
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(hnbn_data.data_ptr, data_len);
  if(hnbn_data.data_ptr == NULL)
  {
    return 0;
  }

  for (rec_num = 1; rec_num <= total_records; rec_num++)
  {
    mmgsdi_status = mmgsdi_cache_get_record(session_id, access,
                                            rec_num, &hnbn_data);
    if (mmgsdi_status != MMGSDI_SUCCESS ||
        hnbn_data.data_ptr == NULL ||
        hnbn_data.data_len < (2 + hnb_name_data.data_len))
    {
      UIM_MSG_HIGH_1("Rec 0x%x data of file MMGSDI_USIM_HNBN not found in cache",
                     rec_num);
      continue;
    }

    if((hnbn_data.data_ptr[0] == 0x80) &&
       (hnbn_data.data_ptr[1] == hnb_name_data.data_len) &&
       !(memcmp(&hnbn_data.data_ptr[2], hnb_name_data.data_ptr,
                hnb_name_data.data_len)))
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(hnbn_data.data_ptr);
      hnbn_data.data_ptr = NULL;
      *is_new_record     = FALSE;
      return rec_num;
    }
    else if(hnbn_data.data_ptr[0] == 0xFF)
    {
      if(first_empty_rec == 0)
      {
        first_empty_rec = rec_num;
      }
    }
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(hnbn_data.data_ptr);
  hnbn_data.data_ptr = NULL;

  /* HNB name not present in records.
     Check if there is an empty record */
  if(first_empty_rec != 0)
  {
    *is_new_record = TRUE;
    return first_empty_rec;
  }
  return 0;
} /* mmgsdi_csg_find_hnbn_record */


/*==============================================================================
  FUNCTION MMGSDI_CSG_WRITE_RECORD_DATA

  DESCRIPTION:
    This function writes the data to a record of a file

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          Write record was successful.
    MMGSDI_ERROR:            Write record was not successful.

  SIDE EFFECTS:
    None
==============================================================================*/
static mmgsdi_return_enum_type mmgsdi_csg_write_record_data
(
  mmgsdi_session_update_acsgl_record_req_type * req_ptr,
  mmgsdi_rec_num_type                           rec_num,
  mmgsdi_access_type                            access,
  mmgsdi_data_type                              write_data
)
{
  mmgsdi_return_enum_type             mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_write_req_type             * write_req_ptr = NULL;
  uint32                              data_len      = 0;

  if((req_ptr == NULL) ||
     (write_data.data_ptr == NULL) ||
     (write_data.data_len <= 0))
  {
    UIM_MSG_ERR_0("Input parameter error");
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(write_req_ptr,
                                     sizeof(mmgsdi_write_req_type));
  if(write_req_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  write_req_ptr->request_header.session_id = req_ptr->request_header.session_id;
  write_req_ptr->request_header.slot_id    = req_ptr->request_header.slot_id;
  write_req_ptr->access.access_method      = MMGSDI_EF_ENUM_ACCESS;
  write_req_ptr->access.file.file_enum     = access.file.file_enum;
  write_req_ptr->rec_num                   = rec_num;
  write_req_ptr->offset                    = 0;
  write_req_ptr->file_type                 = MMGSDI_LINEAR_FIXED_FILE;
  write_req_ptr->data.data_len             = write_data.data_len;

  data_len = int32touint32(write_data.data_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(write_req_ptr->data.data_ptr, data_len);
  if(write_req_ptr->data.data_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE(write_req_ptr);
    return MMGSDI_ERROR;
  }

  /* Copy the data to write */
  mmgsdi_memscpy(write_req_ptr->data.data_ptr,
                 data_len,
                 write_data.data_ptr,
                 write_data.data_len);

  UIM_MSG_HIGH_2("Writing to rec 0x%x of file 0x%x",
                 rec_num, access.file.file_enum);

  mmgsdi_status = mmgsdi_uim_uicc_write_record(write_req_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_2("Failed to write to file 0x%x, status = 0x%x",
                  access.file.file_enum, mmgsdi_status);
  }

  MMGSDIUTIL_TMC_MEM_FREE(write_req_ptr->data.data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(write_req_ptr);

  return mmgsdi_status;
} /* mmgsdi_csg_write_record_data */


/*==============================================================================
  FUNCTION MMGSDI_CSG_WRITE_RECORD_AND_UPDATE_CACHE

  DESCRIPTION:
    This function writes the data to a record of a file and updates the
    corresponding cache

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          Write and update cache were successful.
    MMGSDI_ERROR:            Write and update cache were not successful.

  SIDE EFFECTS:
    None
==============================================================================*/
mmgsdi_return_enum_type mmgsdi_csg_write_record_and_update_cache
(
  mmgsdi_session_update_acsgl_record_req_type * req_ptr,
  mmgsdi_access_type                            access,
  mmgsdi_data_type                              rec_data,
  mmgsdi_rec_num_type                           rec_num
)
{
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  /* Write the record to file */
  mmgsdi_status = mmgsdi_csg_write_record_data(req_ptr,
                                               rec_num,
                                               access,
                                               rec_data);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_2("Failed to write rec 0x%x of file 0x%x to card",
                   rec_num, access.file.file_enum);
    return mmgsdi_status;
  }

  /* Update the cache */
  mmgsdi_status = mmgsdi_csg_update_ef_record_cache(req_ptr->request_header.session_id,
                                                    access, rec_num, rec_data);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_2("Failed to add rec 0x%x of file 0x%x to cache",
                   rec_num, access.file.file_enum);
    return mmgsdi_status;
  }

  return mmgsdi_status;
} /* mmgsdi_csg_write_record_and_update_cache */


#ifdef FEATURE_UIM_PERSISTENT_CACHE
/*==============================================================================
  FUNCTION MMGSDI_CSG_POPULATE_FROM_PERSISTENT_CACHE

  DESCRIPTION:
    This function populate CSG record data from persistent cache

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:

  SIDE EFFECTS:
    None
==============================================================================*/
static void mmgsdi_csg_populate_from_persistent_cache
(
  mmgsdi_session_id_type     session_id,
  mmgsdi_slot_id_enum_type   slot_id,
  mmgsdi_int_app_info_type * int_app_info_ptr
)
{
  mmgsdi_return_enum_type             mmgsdi_status     = MMGSDI_ERROR;
  uint8                               i                 = 0;
  uint16                              j                 = 0;
  uint8                               total_rec         = 0;
  mmgsdi_len_type                     rec_len           = 0;
  mmgsdi_rec_num_type                *num_of_rec_ptr    = NULL;
  mmgsdi_len_type                    *len_of_rec_ptr    = NULL;
  boolean                            *attr_done_ptr     = NULL;
  mmgsdi_access_type                  access;
  mmgsdi_data_type                    cache_data;

  mmgsdi_file_enum_type               csg_files_table[] =
  {
    MMGSDI_USIM_ACSGL, MMGSDI_USIM_CSGT, MMGSDI_USIM_HNBN, MMGSDI_USIM_OCSGL,
    MMGSDI_USIM_OCSGT, MMGSDI_USIM_OHNBN
  };

  memset(&access, 0, sizeof(mmgsdi_access_type));
  access.access_method = MMGSDI_EF_ENUM_ACCESS;

  for(i = 0; i < sizeof(csg_files_table) / sizeof(csg_files_table[0]); i++)
  {
    /* if CSG file is being process, skip reading from persistent cache */
    if(csg_files_table[i] == MMGSDI_USIM_ACSGL)
    {
      attr_done_ptr = &int_app_info_ptr->eons_info.is_get_file_attr_acsgl_done;
      len_of_rec_ptr = &int_app_info_ptr->eons_info.len_of_acsgl_rec;
      num_of_rec_ptr = &int_app_info_ptr->eons_info.num_of_acsgl_rec;
    }
    else if(csg_files_table[i] == MMGSDI_USIM_CSGT)
    {
      attr_done_ptr = &int_app_info_ptr->eons_info.is_get_file_attr_csgt_done;
      len_of_rec_ptr = &int_app_info_ptr->eons_info.len_of_csgt_rec;
      num_of_rec_ptr = &int_app_info_ptr->eons_info.num_of_csgt_rec;
    }
    else if(csg_files_table[i] == MMGSDI_USIM_HNBN)
    {
      attr_done_ptr = &int_app_info_ptr->eons_info.is_get_file_attr_hnbn_done;
      len_of_rec_ptr = &int_app_info_ptr->eons_info.len_of_hnbn_rec;
      num_of_rec_ptr = &int_app_info_ptr->eons_info.num_of_hnbn_rec;
    }
    else if(csg_files_table[i] == MMGSDI_USIM_OCSGL)
    {
      attr_done_ptr = &int_app_info_ptr->eons_info.is_get_file_attr_ocsgl_done;
      len_of_rec_ptr = &int_app_info_ptr->eons_info.len_of_ocsgl_rec;
      num_of_rec_ptr = &int_app_info_ptr->eons_info.num_of_ocsgl_rec;
    }
    else if(csg_files_table[i] == MMGSDI_USIM_OCSGT)
    {
      attr_done_ptr = &int_app_info_ptr->eons_info.is_get_file_attr_ocsgt_done;
      len_of_rec_ptr = &int_app_info_ptr->eons_info.len_of_ocsgt_rec;
      num_of_rec_ptr = &int_app_info_ptr->eons_info.num_of_ocsgt_rec;
    }
    else if(csg_files_table[i] == MMGSDI_USIM_OHNBN)
    {
      attr_done_ptr = &int_app_info_ptr->eons_info.is_get_file_attr_ohnbn_done;
      len_of_rec_ptr = &int_app_info_ptr->eons_info.len_of_ohnbn_rec;
      num_of_rec_ptr = &int_app_info_ptr->eons_info.num_of_ohnbn_rec;
    }

    if(attr_done_ptr == NULL || *attr_done_ptr)
    {
      continue;
    }

    for(j = 1; j <= MMGSDI_MAX_REC_NUM; j++)
    {
      memset(&cache_data, 0, sizeof(mmgsdi_data_type));
      mmgsdi_status = mmgsdi_persistent_cache_read(slot_id,
                                                   csg_files_table[i],
                                                   j,
                                                   &(cache_data.data_len),
                                                   &(cache_data.data_ptr));
      if(mmgsdi_status == MMGSDI_NOT_FOUND)
      {
        UIM_MSG_ERR_2("Failed to read persistent for 0x%x, rec num 0x%x", access.file.file_enum, i);
        break;
      }
      else if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        continue;
      }

      rec_len = cache_data.data_len;
      access.file.file_enum = csg_files_table[i];
      mmgsdi_status = mmgsdi_cache_add_record(session_id,
                                              access,
                                              (mmgsdi_len_type)j,
                                              (const mmgsdi_data_type*)&cache_data);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cache_data.data_ptr);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_2("Failed to add record for 0x%x, rec num 0x%x", access.file.file_enum, i);
        break;
      }
      total_rec++;
    }

    /* update eons data in int_app_info_ptr */
    if(total_rec > 0)
    {
      *attr_done_ptr = TRUE;
      *len_of_rec_ptr = rec_len;
      *num_of_rec_ptr = total_rec;
    }
  }
} /* mmgsdi_csg_populate_from_persistent_cache */


/*==============================================================================
  FUNCTION MMGSDI_CSG_BUILD_PERSISTENT_CACHE

  DESCRIPTION:
    This function build up persistent cache from CSG record data

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          Write record was successful.
    MMGSDI_ERROR:            Write record was not successful.

  SIDE EFFECTS:
    None
==============================================================================*/
static void mmgsdi_csg_build_persistent_cache
(
  mmgsdi_session_id_type     session_id,
  mmgsdi_slot_id_enum_type   slot_id,
  mmgsdi_int_app_info_type * int_app_info_ptr
)
{
  mmgsdi_return_enum_type             mmgsdi_status     = MMGSDI_ERROR;
  uint8                               i                 = 0;
  uint8                               j                 = 0;
  uint8                               total_rec         = 0;
  mmgsdi_len_type                     rec_len           = 0;
  boolean                             attr_done         = FALSE;
  mmgsdi_access_type                  access;
  mmgsdi_data_type                    cache_data;

  mmgsdi_file_enum_type               csg_files_table[] =
  {
    MMGSDI_USIM_ACSGL, MMGSDI_USIM_CSGT, MMGSDI_USIM_HNBN, MMGSDI_USIM_OCSGL,
    MMGSDI_USIM_OCSGT, MMGSDI_USIM_OHNBN
  };

  memset(&cache_data, 0, sizeof(mmgsdi_data_type));
  memset(&access, 0, sizeof(mmgsdi_access_type));
  access.access_method = MMGSDI_EF_ENUM_ACCESS;

  for(i = 0; i < sizeof(csg_files_table) / sizeof(csg_files_table[0]); i++)
  {
    switch(csg_files_table[i])
    {
      case MMGSDI_USIM_ACSGL:
        attr_done = int_app_info_ptr->eons_info.is_get_file_attr_acsgl_done;
        total_rec = int_app_info_ptr->eons_info.num_of_acsgl_rec;
        rec_len   = int_app_info_ptr->eons_info.len_of_acsgl_rec;
        break;

      case MMGSDI_USIM_CSGT:
        attr_done = int_app_info_ptr->eons_info.is_get_file_attr_csgt_done;
        total_rec = int_app_info_ptr->eons_info.num_of_csgt_rec;
        rec_len   = int_app_info_ptr->eons_info.len_of_csgt_rec;
        break;

      case MMGSDI_USIM_HNBN:
        attr_done = int_app_info_ptr->eons_info.is_get_file_attr_hnbn_done;
        total_rec = int_app_info_ptr->eons_info.num_of_hnbn_rec;
        rec_len   = int_app_info_ptr->eons_info.len_of_hnbn_rec;
        break;

      case MMGSDI_USIM_OCSGL:
        attr_done = int_app_info_ptr->eons_info.is_get_file_attr_ocsgl_done;
        total_rec = int_app_info_ptr->eons_info.num_of_ocsgl_rec;
        rec_len   = int_app_info_ptr->eons_info.len_of_ocsgl_rec;
        break;

      case MMGSDI_USIM_OCSGT:
        attr_done = int_app_info_ptr->eons_info.is_get_file_attr_ocsgt_done;
        total_rec = int_app_info_ptr->eons_info.num_of_ocsgt_rec;
        rec_len   = int_app_info_ptr->eons_info.len_of_ocsgt_rec;
        break;

      case MMGSDI_USIM_OHNBN:
        attr_done = int_app_info_ptr->eons_info.is_get_file_attr_ohnbn_done;
        total_rec = int_app_info_ptr->eons_info.num_of_ohnbn_rec;
        rec_len   = int_app_info_ptr->eons_info.len_of_ohnbn_rec;
        break;

      default:
      UIM_MSG_ERR_1("file attr not done for file 0x%x", csg_files_table[i]);
      break;
    }

    if(!attr_done || rec_len == 0)
    {
      UIM_MSG_ERR_3("file 0x%x attr done 0x%x rec len 0x%x",
                    csg_files_table[i], attr_done, rec_len);
      break;
    }

    access.file.file_enum = csg_files_table[i];
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(cache_data.data_ptr,
                                       rec_len);
    if(cache_data.data_ptr == NULL)
    {
      break;
    }

    cache_data.data_len = rec_len;

    for(j = 1; j <= total_rec; j++)
    {
      mmgsdi_status = mmgsdi_cache_get_record(session_id,
                                              access,
                                              j,
                                              &cache_data);

      if (mmgsdi_status != MMGSDI_SUCCESS ||
          cache_data.data_len != rec_len)
      {
        UIM_MSG_HIGH_3("Faile to get record for file, 0x%x, rec 0x%x, data len",
                        csg_files_table[i], j, cache_data.data_len);
        continue;
      }
      mmgsdi_status = mmgsdi_persistent_cache_write(slot_id,
                                                    csg_files_table[i],
                                                    j,
                                                    cache_data.data_len,
                                                    cache_data.data_ptr);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_2("Failed to write persistent for 0x%x, rec num 0x%x",
                      access.file.file_enum, i);
        continue;
      }
    } /* for loop : total_rec for presistent update*/
    MMGSDIUTIL_TMC_MEM_FREE(cache_data.data_ptr);
  } /* for loop : csg_table enums */
} /* mmgsdi_csg_build_persistent_cache */
#endif /* FEATURE_UIM_PERSISTENT_CACHE */


/*==============================================================================
  FUNCTION MMGSDI_CSG_CONSTRUCT_HNBN_TLV_AND_UPDATE_RECORD

  DESCRIPTION:
    This function constructs the HNBN data tlv and writes the data to
    a record of the EF-HNBN. It also updates the cache of the record.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          Write record was successful.
    MMGSDI_ERROR:            Write record was not successful.

  SIDE EFFECTS:
    None
==============================================================================*/
static mmgsdi_return_enum_type mmgsdi_csg_construct_hnbn_tlv_and_udpate_record
(
  mmgsdi_session_update_acsgl_record_req_type * req_ptr,
  mmgsdi_len_type                               hnbn_rec_len,
  mmgsdi_rec_num_type                           hnbn_rec_num
)
{
  mmgsdi_return_enum_type    mmgsdi_status = MMGSDI_SUCCESS;
  uint32                     data_len      = 0;
  mmgsdi_data_type           hnbn_tlv_data;
  mmgsdi_access_type         access;

  if((req_ptr == NULL) ||
     (req_ptr->hnb_name.data_len <= 0) ||
     (req_ptr->hnb_name.data_ptr == NULL))
  {
    UIM_MSG_ERR_0("Input parameters error");
    return MMGSDI_ERROR;
  }

  if(hnbn_rec_len < (req_ptr->hnb_name.data_len + 2))
  {
    UIM_MSG_ERR_0("EF_HNBN record is of insufficient size");
    return MMGSDI_ERROR;
  }

  memset(&access, 0x00, sizeof(mmgsdi_access_type));

  access.access_method  = MMGSDI_EF_ENUM_ACCESS;
  access.file.file_enum = MMGSDI_USIM_HNBN;

  hnbn_tlv_data.data_len  = 0;
  hnbn_tlv_data.data_ptr  = NULL;

  data_len = int32touint32(hnbn_rec_len);

  /* Malloc the buffer same as record length */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(hnbn_tlv_data.data_ptr, data_len);
  if(hnbn_tlv_data.data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  hnbn_tlv_data.data_len = int32touint8(data_len);

  memset(hnbn_tlv_data.data_ptr, 0xFF, data_len);

  /* Begin TLV construction */
  hnbn_tlv_data.data_ptr[0] = 0x80;

  /* As per 3GPP TS 23.003 the max. length for HNB name is 48 bytes
     (< 0x7F). So construct the TLV with one byte for length */
  if(req_ptr->hnb_name.data_len <= SYS_HNB_NAME_LEN)
  {
    hnbn_tlv_data.data_ptr[1] = int32touint8(req_ptr->hnb_name.data_len);
  }
  else
  {
    hnbn_tlv_data.data_ptr[1] = SYS_HNB_NAME_LEN;
  }

  mmgsdi_memscpy(&hnbn_tlv_data.data_ptr[2],
                 hnbn_tlv_data.data_len - 2,
                 req_ptr->hnb_name.data_ptr,
                 hnbn_tlv_data.data_ptr[1]);

  mmgsdi_status = mmgsdi_csg_write_record_and_update_cache(req_ptr,
                                                           access,
                                                           hnbn_tlv_data,
                                                           hnbn_rec_num);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Failed to update EF_HNBN record, status 0x%x", mmgsdi_status);
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(hnbn_tlv_data.data_ptr);
  return mmgsdi_status;
} /* mmgsdi_csg_construct_hnbn_tlv_and_udpate_record */


/*==============================================================================
  FUNCTION MMGSDI_CSG_ADD_TO_ACSGL_RECORD

  DESCRIPTION:
    This function handles adding a csg_id to the EF-ACSGL record.
    1. If the corresponding plmn id is found in a record and the record has
       sufficient space to add the new csg tlv, it is added to that record.
    2. If the record with the corresponding plmn id has no space, the new
       csg tlv (along with the PLMN) is added to an empty record.
    3. If no empty record is available, then PLMN and CSG tlvs are added
       to a record which fits both the tlvs.
    4. If the corresponding plmn is not found, both the plmn and csg tlvs are
       added to an empty record. If no empty records are available, then they are
       added to a record which fits both the tlvs.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:                     Adding the csg_id was successful.
    MMGSDI_ERROR:                       Adding the csg_id was not successful.
    MMGSDI_CSG_UPDATE_FAILED_FILE_FULL: No empty record is available or no
                                        record has sufficient space for the
                                        new csg_id.

  SIDE EFFECTS:
    None
==============================================================================*/
static mmgsdi_return_enum_type mmgsdi_csg_add_to_acsgl_record
(
  mmgsdi_session_update_acsgl_record_req_type * req_ptr
)
{
  mmgsdi_return_enum_type      mmgsdi_status             = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type  *  int_app_info_ptr          = NULL;
  mmgsdi_session_info_type  *  session_info_ptr          = NULL;
  uint16                       index                     = 0;
  uint16                       highest_index             = 0;
  mmgsdi_rec_num_type          rec_num                   = 0;
  mmgsdi_rec_num_type          total_records             = 0;
  mmgsdi_rec_num_type          hnbn_rec_num              = 0;
  mmgsdi_rec_num_type          acsgl_rec_num             = 0;
  mmgsdi_rec_num_type          first_empty_rec_num       = 0;
  mmgsdi_rec_num_type          first_vacant_rec_num      = 0;
  mmgsdi_len_type              rec_len                   = 0;
  mmgsdi_len_type              hnbn_rec_len              = 0;
  uint32                       data_len                  = 0;
  boolean                      is_new_hnbn_record        = FALSE;
  mmgsdi_access_type           access;
  mmgsdi_data_type             acsgl_data;
  mmgsdi_plmn_id_type          plmn_id_from_tbl;
  mmgsdi_plmn_id_type          plmn_id_from_req;
  mmgsdi_csg_plmn_info_type    mmgsdi_csg_plmn_table[MMGSDI_MAX_CSG_INFO];

  memset(&plmn_id_from_tbl, 0x00, sizeof(mmgsdi_plmn_id_type));
  memset(&plmn_id_from_req, 0x00, sizeof(mmgsdi_plmn_id_type));
  memset(mmgsdi_csg_plmn_table, 0x00,
              sizeof(mmgsdi_csg_plmn_info_type) * MMGSDI_MAX_CSG_INFO);

  memset(plmn_id_from_tbl.plmn_id_val, 0xFF, MMGSDI_PLMN_ID_SIZE);
  memset(plmn_id_from_req.plmn_id_val, 0xFF, MMGSDI_PLMN_ID_SIZE);

  memset(&access, 0x00, sizeof(mmgsdi_access_type));
  access.access_method  = MMGSDI_EF_ENUM_ACCESS;
  access.file.file_enum = MMGSDI_USIM_ACSGL;

  acsgl_data.data_len = 0;
  acsgl_data.data_ptr = NULL;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  UIM_MSG_HIGH_1("Add CSG ID 0x%x", req_ptr->plmn_csg_id.csg_id);

  /* Get the session app info */
  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    req_ptr->request_header.session_id,
                    NULL,
                    NULL,
                    &int_app_info_ptr,
                    NULL,
                    &session_info_ptr,
                    NULL);
  if((MMGSDI_SUCCESS != mmgsdi_status) ||
     (int_app_info_ptr == NULL) ||
     (session_info_ptr == NULL))
  {
    UIM_MSG_ERR_0("Get session app info error");
    return MMGSDI_ERROR;
  }

  /* Copy the PLMN from request */
  mmgsdi_memscpy(plmn_id_from_req.plmn_id_val,
                 sizeof(plmn_id_from_req.plmn_id_val),
                 req_ptr->plmn_csg_id.plmn_id_val,
                 MMGSDI_PLMN_ID_SIZE);

  total_records = int_app_info_ptr->eons_info.num_of_acsgl_rec;
  rec_len       = int_app_info_ptr->eons_info.len_of_acsgl_rec;
  hnbn_rec_len  = int_app_info_ptr->eons_info.len_of_hnbn_rec;

  if((total_records <= 0) ||
     (rec_len < MMGSDI_ACSGL_TLV_SIZE))
  {
    UIM_MSG_ERR_0("EF_ACSGL has no space for the new CSG ID");
    return MMGSDI_ERROR;
  }

  /* If HNBN is provided in the request, find the
     corresponding record */
  if(req_ptr->hnb_name.data_len > 0)
  {
    /* If the HNB is not present in the existing records, the function
       returns first available empty record which has sufficient space
       to fit the new HNB name. If there is an empty record but doesn't
       fit the new HNB name it is skipped (is_new_hnbn_record = FALSE) */
    hnbn_rec_num = mmgsdi_csg_find_hnbn_record(req_ptr->request_header.session_id,
                                               req_ptr->hnb_name,
                                               &is_new_hnbn_record);

    UIM_MSG_HIGH_1("HNB name rec 0x%x", hnbn_rec_num);
  }

  /* Get the record length in cache */
  mmgsdi_status = mmgsdi_cache_get_record_len(req_ptr->request_header.session_id,
                                              access, 1,
                                              &acsgl_data.data_len);
  if((mmgsdi_status != MMGSDI_SUCCESS) ||
     (acsgl_data.data_len == 0))
  {
    UIM_MSG_HIGH_2("Rec 0x%x of file 0x%x not found in cache",
                   1, MMGSDI_USIM_ACSGL);
    return MMGSDI_ERROR;
  }

  data_len = int32touint32(acsgl_data.data_len);

  /* Malloc one time as all the records are of same length */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(acsgl_data.data_ptr, data_len);
  if(acsgl_data.data_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* Loop through all the records to find the record with
     the PLMN */
  for (rec_num = 1; rec_num <= total_records; rec_num++)
  {
    boolean  plmn_found = FALSE;
    /*Reset table data */
    memset(mmgsdi_csg_plmn_table, 0x00,
         MMGSDI_MAX_CSG_INFO * sizeof(mmgsdi_csg_plmn_info_type));

    mmgsdi_status = mmgsdi_cache_get_record(req_ptr->request_header.session_id,
                                            access, rec_num, &acsgl_data);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_HIGH_2("Rec 0x%x data of file 0x%x not found in cache",
                     rec_num, MMGSDI_USIM_ACSGL);
      continue;
    }

    /* Check if it is empty record, save it */
    if(acsgl_data.data_ptr[0] == 0xFF)
    {
      if(first_empty_rec_num == 0)
      {
        first_empty_rec_num = rec_num;
      }
      continue;
    }

    /* Parse ACSGL record*/
    mmgsdi_status = mmgsdi_csg_parse_csgl_and_populate_table(MMGSDI_USIM_ACSGL,
                                                             acsgl_data,
                                                             &highest_index,
                                                             mmgsdi_csg_plmn_table,
                                                             MMGSDI_MAX_CSG_INFO);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_1("Failed to parse rec 0x%x", rec_num);
      continue;
    }

    /* Loop through the table */
    for(index = 0; index <= highest_index; index++)
    {
      mmgsdi_memscpy(plmn_id_from_tbl.plmn_id_val,
                     sizeof(plmn_id_from_tbl.plmn_id_val),
                     mmgsdi_csg_plmn_table[index].plmn_id_val,
                     MMGSDI_PLMN_ID_SIZE);

      if(mmgsdi_eons_compare_sim_plmn_with_nw_plmn(plmn_id_from_tbl,
                                                   plmn_id_from_req))
      {
        plmn_found = TRUE;
        break;
      }
    }

    if(plmn_found)
    {
      /* Check if the record fits the new CSG TLV */
      if(mmgsdi_csg_check_acsgl_record_for_new_csgid(mmgsdi_csg_plmn_table,
                                                     highest_index,
                                                     rec_len,
                                                     FALSE))
      {
        acsgl_rec_num = rec_num;

        /* Rec with PLMN is found and it fits new CSG TLV */
        break;
      }

      continue;
    }

    /* Check if the record has space for both PLMN and CSG TLVs */
    if((first_vacant_rec_num == 0) &&
       (mmgsdi_csg_check_acsgl_record_for_new_csgid(mmgsdi_csg_plmn_table,
                                                    highest_index,
                                                    rec_len,
                                                    TRUE)))
    {
      first_vacant_rec_num = rec_num;
    }
  }

  /* Rec with PLMN to update has space */
  if(acsgl_rec_num != 0)
  {
    UIM_MSG_HIGH_1("Update record 0x%x with new CSG ID", acsgl_rec_num);

    /* Check to ensure table is not over-written */
    if((highest_index < (MMGSDI_MAX_CSG_INFO - 1)) &&
       (index < (MMGSDI_MAX_CSG_INFO - 1)))
    {
      /* Add a new row with PLMN and CSG IDs to the table at the
         index - 1 */
      /* Move the current data to the index + 1 before adding */
      mmgsdi_memsmove(&mmgsdi_csg_plmn_table[index + 1],
                      sizeof(mmgsdi_csg_plmn_info_type) * (MMGSDI_MAX_CSG_INFO - index - 1),
                      &mmgsdi_csg_plmn_table[index],
                      sizeof(mmgsdi_csg_plmn_info_type) * ((highest_index - index) + 1));

      mmgsdi_csg_plmn_table[index].csg_id = req_ptr->plmn_csg_id.csg_id;

      mmgsdi_csg_plmn_table[index].hnbn_record_num = hnbn_rec_num;

      /* Update the index */
      highest_index++;
      rec_num = acsgl_rec_num;
    }
  }
  else if(first_empty_rec_num != 0)
  {
    UIM_MSG_HIGH_1("Update empty record 0x%x with new CSG ID",
                   first_empty_rec_num);

    memset(mmgsdi_csg_plmn_table, 0x00,
       MMGSDI_MAX_CSG_INFO * sizeof(mmgsdi_csg_plmn_info_type));

    /* Since it is an empty record being populated with
       a plmn and csg tlv, set index in the table to 0 */
    highest_index = 0;
    mmgsdi_memscpy(mmgsdi_csg_plmn_table[highest_index].plmn_id_val,
                   sizeof(mmgsdi_csg_plmn_table[highest_index].plmn_id_val),
                   plmn_id_from_req.plmn_id_val,
                   MMGSDI_PLMN_ID_SIZE);
    mmgsdi_csg_plmn_table[highest_index].csg_id = req_ptr->plmn_csg_id.csg_id;
    /* Use the default CSGT record */
    mmgsdi_csg_plmn_table[highest_index].csgt_record_num = 0;
    mmgsdi_csg_plmn_table[highest_index].hnbn_record_num = hnbn_rec_num;
    rec_num = first_empty_rec_num;
  }
  else if(first_vacant_rec_num != 0)
  {
    UIM_MSG_HIGH_1("Update vacant record 0x%x with new CSG ID",
                   first_vacant_rec_num);
    memset(mmgsdi_csg_plmn_table, 0x00,
       MMGSDI_MAX_CSG_INFO * sizeof(mmgsdi_csg_plmn_info_type));

    /* Get the vacant record data from cache as current data in acsgl_data
       might be from a different record */
    mmgsdi_status = mmgsdi_cache_get_record(req_ptr->request_header.session_id,
                                            access, first_vacant_rec_num, &acsgl_data);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_HIGH_2("Rec 0x%x data of file 0x%x not found in cache",
                     first_vacant_rec_num, MMGSDI_USIM_ACSGL);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(acsgl_data.data_ptr);
      return mmgsdi_status;
    }

    /* Parse ACSGL record*/
    mmgsdi_status = mmgsdi_csg_parse_csgl_and_populate_table(MMGSDI_USIM_ACSGL,
                                                             acsgl_data,
                                                             &highest_index,
                                                             mmgsdi_csg_plmn_table,
                                                             MMGSDI_MAX_CSG_INFO);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_HIGH_1("Failed to parse rec 0x%x", first_vacant_rec_num);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(acsgl_data.data_ptr);
      return mmgsdi_status;
    }

    if (highest_index < (MMGSDI_MAX_CSG_INFO - 1))
    {
      /* Add PLMN and CSG info at the next index */
      highest_index++;
      mmgsdi_memscpy(mmgsdi_csg_plmn_table[highest_index].plmn_id_val,
                     sizeof(mmgsdi_csg_plmn_table[highest_index].plmn_id_val),
                     plmn_id_from_req.plmn_id_val,
                     MMGSDI_PLMN_ID_SIZE);
      mmgsdi_csg_plmn_table[highest_index].csg_id = req_ptr->plmn_csg_id.csg_id;
      /* Use the same CSGT record */
      mmgsdi_csg_plmn_table[highest_index].csgt_record_num = 0;
      mmgsdi_csg_plmn_table[highest_index].hnbn_record_num = hnbn_rec_num;
      rec_num = first_vacant_rec_num;
    }
  }
  else
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(acsgl_data.data_ptr);
    return MMGSDI_CSG_UPDATE_FAILED_FILE_FULL;
  }

  /* Construct the TLV from the updated table */
  mmgsdi_status = mmgsdi_csg_construct_acsgl_tlv_from_table(mmgsdi_csg_plmn_table,
                                                            highest_index,
                                                            &acsgl_data);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(acsgl_data.data_ptr);
    return mmgsdi_status;
  }

  /* Write the updated acsgl tlv to record and update cache */
  mmgsdi_status = mmgsdi_csg_write_record_and_update_cache(req_ptr,
                                                           access,
                                                           acsgl_data,
                                                           rec_num);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_2("Failed to update rec 0x%x of file 0x%x",
                   rec_num, MMGSDI_USIM_ACSGL);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(acsgl_data.data_ptr);
    return mmgsdi_status;
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(acsgl_data.data_ptr);

  if((is_new_hnbn_record) &&
     (hnbn_rec_num != 0))
  {
    /* Construct the HNB Name TLV and update cache, EF */
    mmgsdi_status = mmgsdi_csg_construct_hnbn_tlv_and_udpate_record(req_ptr,
                                                                    hnbn_rec_len,
                                                                    hnbn_rec_num);
  }
  return mmgsdi_status;
} /* mmgsdi_csg_add_to_acsgl_record */


/*==============================================================================
  FUNCTION MMGSDI_CSG_DELETE_FROM_ACSGL_RECORD

  DESCRIPTION:
    This function handles deleting a csg_id from the EF-ACSGL record

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:                  Deleting csg_id was successful.
    MMGSDI_ERROR:                    Deleting csg_id was not successful.
    MMGSDI_CSG_UPDATE_FAILED:        The csg_id to delete is not found.

  SIDE EFFECTS:
    None
==============================================================================*/
static mmgsdi_return_enum_type mmgsdi_csg_delete_from_acsgl_record
(
  mmgsdi_session_update_acsgl_record_req_type * req_ptr
)
{
  mmgsdi_return_enum_type      mmgsdi_status             = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type  *  int_app_info_ptr          = NULL;
  mmgsdi_session_info_type  *  session_info_ptr          = NULL;
  uint16                       index                     = 0;
  uint16                       highest_index             = 0;
  mmgsdi_rec_num_type          rec_num                   = 0;
  mmgsdi_rec_num_type          total_records             = 0;
  uint32                       data_len                  = 0;
  boolean                      csg_found                 = FALSE;
  mmgsdi_access_type           access;
  mmgsdi_data_type             acsgl_data;
  mmgsdi_plmn_id_type          plmn_id_from_tbl;
  mmgsdi_plmn_id_type          plmn_id_from_req;
  mmgsdi_csg_plmn_info_type    mmgsdi_csg_plmn_table[MMGSDI_MAX_CSG_INFO];

  memset(&plmn_id_from_tbl, 0x00, sizeof(mmgsdi_plmn_id_type));
  memset(&plmn_id_from_req, 0x00, sizeof(mmgsdi_plmn_id_type));
  memset(mmgsdi_csg_plmn_table, 0x00,
              sizeof(mmgsdi_csg_plmn_info_type) * MMGSDI_MAX_CSG_INFO);

  memset(plmn_id_from_tbl.plmn_id_val, 0xFF, MMGSDI_PLMN_ID_SIZE);
  memset(plmn_id_from_req.plmn_id_val, 0xFF, MMGSDI_PLMN_ID_SIZE);

  memset(&access, 0x00, sizeof(mmgsdi_access_type));
  access.access_method  = MMGSDI_EF_ENUM_ACCESS;
  access.file.file_enum = MMGSDI_USIM_ACSGL;

  acsgl_data.data_len = 0;
  acsgl_data.data_ptr = NULL;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  UIM_MSG_HIGH_1("Delete CSG ID 0x%x", req_ptr->plmn_csg_id.csg_id);

  /* Get the session app info */
  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    req_ptr->request_header.session_id,
                    NULL,
                    NULL,
                    &int_app_info_ptr,
                    NULL,
                    &session_info_ptr,
                    NULL);
  if((MMGSDI_SUCCESS != mmgsdi_status) ||
     (int_app_info_ptr == NULL) ||
     (session_info_ptr == NULL))
  {
    UIM_MSG_ERR_0("Session app info error");
    return MMGSDI_ERROR;
  }

  mmgsdi_memscpy(plmn_id_from_req.plmn_id_val,
                 sizeof(plmn_id_from_req.plmn_id_val),
                 req_ptr->plmn_csg_id.plmn_id_val,
                 MMGSDI_PLMN_ID_SIZE);

  total_records = int_app_info_ptr->eons_info.num_of_acsgl_rec;

  if(total_records > 0)
  {
    /* Get the record len from cache */
    mmgsdi_status = mmgsdi_cache_get_record_len(req_ptr->request_header.session_id,
                                                access, 1,
                                                &acsgl_data.data_len);
    if((mmgsdi_status != MMGSDI_SUCCESS) ||
       (acsgl_data.data_len == 0))
    {
      UIM_MSG_HIGH_2("Rec 0x%x of file 0x%x not found in cache",
                     1, MMGSDI_USIM_ACSGL);
      return MMGSDI_ERROR;
    }

    data_len = int32touint32(acsgl_data.data_len);

    /* Malloc one time as all the records are of same length */
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(acsgl_data.data_ptr, data_len);
    if(acsgl_data.data_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
  }

  for (rec_num = 1; rec_num <= total_records; rec_num++)
  {
    /* Reset the index and table data */
    index = 0;
    memset(mmgsdi_csg_plmn_table, 0x00,
         MMGSDI_MAX_CSG_INFO * sizeof(mmgsdi_csg_plmn_info_type));

    mmgsdi_status = mmgsdi_cache_get_record(req_ptr->request_header.session_id, access,
                                            rec_num, &acsgl_data);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_HIGH_2("Rec 0x%x data of file 0x%x not found in cache",
                     rec_num, MMGSDI_USIM_ACSGL);
      continue;
    }

    /* If empty record, skip it */
    if(acsgl_data.data_ptr[0] != 0xA0)
    {
      continue;
    }

    /* Parse ACSGL record */
    mmgsdi_status = mmgsdi_csg_parse_csgl_and_populate_table(MMGSDI_USIM_ACSGL,
                                                             acsgl_data,
                                                             &highest_index,
                                                             mmgsdi_csg_plmn_table,
                                                             MMGSDI_MAX_CSG_INFO);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_HIGH_2("Unable to parse rec 0x%x of file 0x%x",
                     rec_num, MMGSDI_USIM_ACSGL);
      continue;
    }

    /* Loop through the table */
    while((index <= highest_index) &&
          (index < MMGSDI_MAX_CSG_INFO))
    {
      mmgsdi_memscpy(plmn_id_from_tbl.plmn_id_val,
                     sizeof(plmn_id_from_tbl.plmn_id_val),
                     mmgsdi_csg_plmn_table[index].plmn_id_val,
                     MMGSDI_PLMN_ID_SIZE);

      if(mmgsdi_eons_compare_sim_plmn_with_nw_plmn(plmn_id_from_tbl,
                                                   plmn_id_from_req))
      {
        if(req_ptr->plmn_csg_id.csg_id ==
             mmgsdi_csg_plmn_table[index].csg_id)
        {
          UIM_MSG_HIGH_2("Found CSG ID in rec 0x%x at index 0x%x", rec_num, index);
          memset(&mmgsdi_csg_plmn_table[index], 0x00,
                     sizeof(mmgsdi_csg_plmn_table[0]));
          csg_found = TRUE;
          break;
        }
      }
      index++;
    }

    if(csg_found)
    {
      break;
    }
  }

  if (csg_found == FALSE)
  {
    UIM_MSG_ERR_0("CSG ID to delete, not found in records");
    MMGSDIUTIL_TMC_MEM_FREE(acsgl_data.data_ptr);
    return MMGSDI_CSG_NOT_FOUND;
  }

  if(index < (MMGSDI_MAX_CSG_INFO - 1))
  {
    /* Remove the corresponding info from the table */
    mmgsdi_memsmove(&mmgsdi_csg_plmn_table[index],
                     sizeof(mmgsdi_csg_plmn_table) - (sizeof(mmgsdi_csg_plmn_info_type) * index),
                     &mmgsdi_csg_plmn_table[index + 1],
                     sizeof(mmgsdi_csg_plmn_info_type) * (MMGSDI_MAX_CSG_INFO - 1 - index));
  }

  if(highest_index > 0)
  {
    highest_index--;

    /* If csg_id is found, acsgl_data.data_ptr has corresponding record data.
       Construct the new acsgl tlv after deleting the requested csg_id */
    mmgsdi_status = mmgsdi_csg_construct_acsgl_tlv_from_table(mmgsdi_csg_plmn_table,
                                                              highest_index,
                                                              &acsgl_data);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MMGSDIUTIL_TMC_MEM_FREE(acsgl_data.data_ptr);
      return mmgsdi_status;
    }
  }
  else
  {
    /* highest_index will be 0 in case there is one
       valid record with only one PLMN and CSG ID */
    memset(acsgl_data.data_ptr, 0xFF, acsgl_data.data_len);
  }

  /* Write the updated acsgl tlv to record and update cache */
  mmgsdi_status = mmgsdi_csg_write_record_and_update_cache(req_ptr,
                                                           access,
                                                           acsgl_data,
                                                           rec_num);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_2("Failed to update rec 0x%x of file 0x%x",
                   rec_num, MMGSDI_USIM_ACSGL);
  }

  MMGSDIUTIL_TMC_MEM_FREE(acsgl_data.data_ptr);
  return mmgsdi_status;
} /* mmgsdi_csg_delete_from_acsgl_record */


/*==============================================================================
  FUNCTION MMGSDI_CSG_PROCESS_UPDATE_ACSGL_RECORD_REQUEST

  DESCRIPTION:
    This function handles adding or deleting a csg_id to the EF-ACSGL record

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:           The command processing was successful.
    MMGSDI_ERROR:             The command processing was not successful.
    MMGSDI_CSG_UPDATE_FAILED: The new csg_id cannot be added or the
                              csg_id to delete is not found.

  SIDE EFFECTS:
    None
==============================================================================*/
mmgsdi_return_enum_type mmgsdi_csg_process_update_acsgl_record_reqest
(
  mmgsdi_session_update_acsgl_record_req_type * req_ptr
)
{
  mmgsdi_return_enum_type              mmgsdi_status   = MMGSDI_SUCCESS;
  mmgsdi_client_req_extra_info_type *  extra_param_ptr = NULL;
  int32                                index           = 0;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
                    req_ptr->request_header.request_type, (void*)req_ptr, &extra_param_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    return mmgsdi_status;
  }

  /*Fill the client table*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                    index,
                    &req_ptr->request_header,
                    NULL,
                    extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  if(req_ptr->update_req_type == MMGSDI_UPDATE_ADD_CSG_ID)
  {
    mmgsdi_status = mmgsdi_csg_add_to_acsgl_record(req_ptr);
  }
  else if(req_ptr->update_req_type == MMGSDI_UPDATE_DELETE_CSG_ID)
  {
    mmgsdi_status = mmgsdi_csg_delete_from_acsgl_record(req_ptr);
  }
  else
  {
    UIM_MSG_ERR_1("Unknown acsgl update request 0x%x", req_ptr->update_req_type);
    mmgsdi_status = MMGSDI_ERROR;
  }

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                     index, MMGSDI_SESSION_UPDATE_ACSGL_RECORD_REQ,
                     mmgsdi_status);
  }
  else
  {
    mmgsdi_util_free_client_request_table_index(index);
  }

  return mmgsdi_status;
} /* mmgsdi_csg_process_update_acsgl_record_reqest */


/* =============================================================================
  FUNCTION:      MMGSDI_CSG_RESET_EONS_FLAGS_IN_APP_INFO

  DESCRIPTION:
    This function resets the CSG eons info in the global application information

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
=============================================================================*/
void mmgsdi_csg_reset_eons_flags_in_app_info
(
  mmgsdi_session_id_type  session_id
)
{
  mmgsdi_return_enum_type    mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_int_app_info_type * int_app_info_ptr = NULL;

  UIM_MSG_HIGH_0("mmgsdi_csg_reset_eons_flags_in_app_info");

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                        session_id,
                        NULL,
                        NULL,
                        &int_app_info_ptr,
                        NULL,
                        NULL,
                        NULL);
  if((mmgsdi_status != MMGSDI_SUCCESS) ||
     (int_app_info_ptr == NULL))
  {
    UIM_MSG_ERR_0("Unable to get session app info");
    return;
  }

  /* Reset EFs ACSGL/CSGT/HNBN */
  int_app_info_ptr->eons_info.is_get_file_attr_acsgl_done = FALSE;
  int_app_info_ptr->eons_info.is_get_file_attr_csgt_done  = FALSE;
  int_app_info_ptr->eons_info.is_get_file_attr_hnbn_done  = FALSE;

  /* Reset EFs OCSGL/OCSGT/OHNBN */
  int_app_info_ptr->eons_info.is_get_file_attr_ocsgl_done = FALSE;
  int_app_info_ptr->eons_info.is_get_file_attr_ocsgt_done = FALSE;
  int_app_info_ptr->eons_info.is_get_file_attr_ohnbn_done = FALSE;

  /* Initialize EF-ACSGL record info */
  int_app_info_ptr->eons_info.len_of_acsgl_rec = 0;
  int_app_info_ptr->eons_info.num_of_acsgl_rec = 0;

  /* Initialize EF-CSGT record info */
  int_app_info_ptr->eons_info.len_of_csgt_rec = 0;
  int_app_info_ptr->eons_info.num_of_csgt_rec = 0;

  /* Initialize EF-HNBN record info */
  int_app_info_ptr->eons_info.len_of_hnbn_rec = 0;
  int_app_info_ptr->eons_info.num_of_hnbn_rec = 0;

  /* Initialize EF-OCSGL record info */
  int_app_info_ptr->eons_info.len_of_ocsgl_rec = 0;
  int_app_info_ptr->eons_info.num_of_ocsgl_rec = 0;

  /* Initialize EF-OCSGT record info */
  int_app_info_ptr->eons_info.len_of_ocsgt_rec = 0;
  int_app_info_ptr->eons_info.num_of_ocsgt_rec = 0;

  /* Initialize EF-OHNBN record info */
  int_app_info_ptr->eons_info.len_of_ohnbn_rec = 0;
  int_app_info_ptr->eons_info.num_of_ohnbn_rec = 0;

  /* Reset the HNBN in EONS info. Irrespective of the
     length, memset the whole buffer */
  memset(&(int_app_info_ptr->eons_info.ss_info.first_stack_ss.nitz_info.csg_info.hnb_name), 0x00,
         sizeof(sys_home_nodeb_name_type));
  int_app_info_ptr->eons_info.ss_info.first_stack_ss.nitz_info.csg_info.csg_id = MMGSDI_EONS_INVALID_CSG_ID;
  int_app_info_ptr->eons_info.ss_info.first_stack_ss.nitz_info.rplmn_id.csg_id = MMGSDI_EONS_INVALID_CSG_ID;

  memset(&(int_app_info_ptr->eons_info.rplmn_info.first_stack_rplmn.csg_info.hnb_name), 0x00,
         sizeof(sys_home_nodeb_name_type));
  int_app_info_ptr->eons_info.rplmn_info.first_stack_rplmn.csg_info.csg_id = MMGSDI_EONS_INVALID_CSG_ID;
  int_app_info_ptr->eons_info.rplmn_info.first_stack_rplmn.rplmn_id.csg_id = MMGSDI_EONS_INVALID_CSG_ID;

  memset(&(int_app_info_ptr->eons_info.ss_info.sec_stack_ss.nitz_info.csg_info.hnb_name.name), 0x00,
         sizeof(int_app_info_ptr->eons_info.ss_info.sec_stack_ss.nitz_info.csg_info.hnb_name.name));
  int_app_info_ptr->eons_info.ss_info.sec_stack_ss.nitz_info.csg_info.csg_id = MMGSDI_EONS_INVALID_CSG_ID;
  int_app_info_ptr->eons_info.ss_info.sec_stack_ss.nitz_info.rplmn_id.csg_id = MMGSDI_EONS_INVALID_CSG_ID;

  memset(&(int_app_info_ptr->eons_info.rplmn_info.sec_stack_rplmn.csg_info.hnb_name), 0x00,
         sizeof(sys_home_nodeb_name_type));
  int_app_info_ptr->eons_info.rplmn_info.sec_stack_rplmn.csg_info.csg_id = MMGSDI_EONS_INVALID_CSG_ID;
  int_app_info_ptr->eons_info.rplmn_info.sec_stack_rplmn.rplmn_id.csg_id = MMGSDI_EONS_INVALID_CSG_ID;

} /* mmgsdi_csg_reset_eons_flags_in_app_info */


/*===========================================================================
FUNCTION MMGSDI_EONS_TRIGGER_OCSGL_OCSGT_OHNBN_CACHE

DESCRIPTION
  Function to trigger caching OCSGL/OCSGT/OHNBN records

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_eons_trigger_ocsgl_ocsgt_ohnbn_cache(
  mmgsdi_session_id_type session_id)
{
  mmgsdi_return_enum_type    mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type * int_app_info_ptr = NULL;
  mmgsdi_session_info_type * session_info_ptr = NULL;
  mmgsdi_access_type         file_access;

  memset(&file_access, 0x00, sizeof(mmgsdi_access_type));

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    session_id,
                    NULL,
                    NULL,
                    &int_app_info_ptr,
                    NULL,
                    &session_info_ptr,
                    NULL);
  if((mmgsdi_status != MMGSDI_SUCCESS) ||
     (int_app_info_ptr == NULL) ||
     (session_info_ptr == NULL))
  {
    UIM_MSG_ERR_0("Failed to retrieve session app info");
    return;
  }

  /* Check whether session type is either GW PRI or GW SEC*/
  if((session_info_ptr->session_type != MMGSDI_GW_PROV_PRI_SESSION) &&
     (session_info_ptr->session_type != MMGSDI_GW_PROV_SEC_SESSION))
  {
    UIM_MSG_ERR_1("Unexpected session type: 0x%x, not performing OCSGL/OHNBN caching",
                session_info_ptr->session_type);
    return;
  }

  if(int_app_info_ptr->app_data.app_type != MMGSDI_APP_USIM)
  {
    UIM_MSG_HIGH_1("App type is: 0x%x, operator CSG caching is not needed",
                   int_app_info_ptr->app_data.app_type);
    return;
  }

  if(!int_app_info_ptr->eons_info.is_get_file_attr_ocsgl_done)
  {
    UIM_MSG_HIGH_0("Trigger OCSGL Cache");
    file_access.access_method  = MMGSDI_EF_ENUM_ACCESS;
    file_access.file.file_enum = MMGSDI_USIM_OCSGL;
  }
  else if(!int_app_info_ptr->eons_info.is_get_file_attr_ocsgt_done)
  {
    UIM_MSG_HIGH_0("Trigger OCSGT Cache");
    file_access.access_method  = MMGSDI_EF_ENUM_ACCESS;
    file_access.file.file_enum = MMGSDI_USIM_OCSGT;
  }
  else if(!int_app_info_ptr->eons_info.is_get_file_attr_ohnbn_done)
  {
    UIM_MSG_HIGH_0("Trigger OHNBN Cache");
    file_access.access_method  = MMGSDI_EF_ENUM_ACCESS;
    file_access.file.file_enum = MMGSDI_USIM_OHNBN;
  }
  else
  {
    return;
  }

  mmgsdi_status = mmgsdi_session_get_file_attr(
                    session_id,
                    file_access,
                    mmgsdi_eons_csg_file_attr_cb,
                    0);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_2("Get file attr failed, status: 0x%x, file enum: 0x%x",
                  mmgsdi_status, file_access.file.file_enum);
  }
} /* mmgsdi_eons_trigger_ocsgl_ocsgt_ohnbn_cache */


/*===========================================================================
FUNCTION MMGSDI_CSG_GET_RECORD_NOT_CACHED

DESCRIPTION
  This function checks the record cache starting from the current record
  provided as the input and gets the next record not in cache.

DEPENDENCIES
  NONE

RETURN VALUE
  - record number not in cache
  - 0, if all records are in cache or if there is an error

SIDE EFFECTS
  NONE
===========================================================================*/
static mmgsdi_rec_num_type mmgsdi_csg_get_record_not_cached(
  mmgsdi_session_id_type  session_id,
  mmgsdi_access_type      access,
  mmgsdi_rec_num_type     current_rec_num,
  mmgsdi_rec_num_type     total_recs
)
{
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_rec_num_type      rec_cnt       = 0;
  mmgsdi_len_type          rec_len       = 0;

  if((current_rec_num <=0) ||
     (total_recs <= 0)  ||
     (current_rec_num >= total_recs))
  {
    return 0;
  }

  for(rec_cnt = current_rec_num + 1; rec_cnt <= total_recs; rec_cnt++)
  {
    mmgsdi_status = mmgsdi_cache_get_record_len(session_id, access,
                                                rec_cnt, &rec_len);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_MED_1("record 0x%x not in cache", rec_cnt);
      return rec_cnt;
    }
  }

  return 0;
} /* mmgsdi_csg_get_record_not_cached */


/*===========================================================================
FUNCTION MMGSDI_EONS_CSG_READ_REC_CB

DESCRIPTION
  Read record callback function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_eons_csg_read_rec_cb(
  mmgsdi_return_enum_type     status,
  mmgsdi_cnf_enum_type        cnf,
  const mmgsdi_cnf_type     * cnf_ptr
)
{
  mmgsdi_return_enum_type    mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_int_app_info_type * int_app_info_ptr = NULL;
  mmgsdi_session_info_type * session_info_ptr = NULL;
  mmgsdi_file_enum_type      file_enum        = MMGSDI_MAX_FILE_ENUM;
  mmgsdi_rec_num_type        rec_number       = 0;
  mmgsdi_rec_num_type        next_rec_num     = 0;
  mmgsdi_len_type            next_rec_len     = 0;
  boolean                    read_next_record = FALSE;

  if((status != MMGSDI_SUCCESS) ||
     (cnf_ptr == NULL)          ||
     (cnf != MMGSDI_READ_CNF)   ||
     (cnf_ptr->read_cnf.access.access_method != MMGSDI_EF_ENUM_ACCESS))
  {
    UIM_MSG_ERR_0("Error in read record confirmation");
    return;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                        cnf_ptr->read_cnf.response_header.session_id,
                        NULL,
                        NULL,
                        &int_app_info_ptr,
                        NULL,
                        &session_info_ptr,
                        NULL);
  if((mmgsdi_status != MMGSDI_SUCCESS) ||
     (int_app_info_ptr == NULL) ||
     (session_info_ptr == NULL))
  {
    UIM_MSG_ERR_0("Unable to get session app info");
    return;
  }

  rec_number = cnf_ptr->read_cnf.accessed_rec_num;
  file_enum  = cnf_ptr->read_cnf.access.file.file_enum;

  switch(file_enum)
  {
    case MMGSDI_USIM_ACSGL:
      /* get the next record to read */
      next_rec_num = mmgsdi_csg_get_record_not_cached(cnf_ptr->read_cnf.response_header.session_id,
                                                      cnf_ptr->read_cnf.access,
                                                      rec_number,
                                                      int_app_info_ptr->eons_info.num_of_acsgl_rec);
      if(next_rec_num == 0)
      {
        rec_number = int_app_info_ptr->eons_info.num_of_acsgl_rec;
      }
      else if(next_rec_num > 1)
      {
        rec_number = next_rec_num - 1;
      }

      /* Last record of ACSGL is read, need to start caching CSGT */
      if(rec_number == int_app_info_ptr->eons_info.num_of_acsgl_rec)
      {
        UIM_MSG_HIGH_1("Last record 0x%x of ACSGL is read, start caching CSGT",
                       rec_number);

        read_next_record = FALSE;
        mmgsdi_eons_trigger_acsgl_csgt_hnbn_cache(
          cnf_ptr->response_header.session_id);
      }
      else if(rec_number < int_app_info_ptr->eons_info.num_of_acsgl_rec)
      {
        /* Read the next ACSGL record */
        next_rec_len = int_app_info_ptr->eons_info.len_of_acsgl_rec;
        read_next_record = TRUE;
      }
      break;

    case MMGSDI_USIM_CSGT:
      /* Last record of CSGT is read, need to start caching HNBN */
      if(rec_number == int_app_info_ptr->eons_info.num_of_csgt_rec)
      {
        UIM_MSG_HIGH_1("Last record 0x%x of CSGT is read, start caching HNBN",
                       rec_number);

        read_next_record = FALSE;
        mmgsdi_eons_trigger_acsgl_csgt_hnbn_cache(
          cnf_ptr->response_header.session_id);
      }
      else if(rec_number < int_app_info_ptr->eons_info.num_of_csgt_rec)
      {
        /* Read the next CSGT record */
        next_rec_len = int_app_info_ptr->eons_info.len_of_csgt_rec;
        read_next_record = TRUE;
      }
      break;

    case MMGSDI_USIM_HNBN:
      if(rec_number == int_app_info_ptr->eons_info.num_of_hnbn_rec)
      {
        read_next_record = FALSE;

        /* Trigger OCSGL EFs caching */
        mmgsdi_eons_trigger_ocsgl_ocsgt_ohnbn_cache(
          cnf_ptr->response_header.session_id);
      }
      if(rec_number < int_app_info_ptr->eons_info.num_of_hnbn_rec)
      {
        /* Read the next HNBN record */
        next_rec_len = int_app_info_ptr->eons_info.len_of_hnbn_rec;
        read_next_record = TRUE;
      }
      break;

    case MMGSDI_USIM_OCSGL:
      next_rec_num = mmgsdi_csg_get_record_not_cached(cnf_ptr->read_cnf.response_header.session_id,
                                                      cnf_ptr->read_cnf.access,
                                                      rec_number,
                                                      int_app_info_ptr->eons_info.num_of_ocsgl_rec);
      if(next_rec_num == 0)
      {
        rec_number = int_app_info_ptr->eons_info.num_of_ocsgl_rec;
      }
      else if(next_rec_num > 1)
      {
        rec_number = next_rec_num - 1;
      }

      /* Last record of OCSGL is read, need to start caching OCSGT */
      if(rec_number == int_app_info_ptr->eons_info.num_of_ocsgl_rec)
      {
        UIM_MSG_HIGH_1("Last record 0x%x of OCSGL is read, start caching OCSGT",
                       rec_number);

        read_next_record = FALSE;
        mmgsdi_eons_trigger_ocsgl_ocsgt_ohnbn_cache(
          cnf_ptr->response_header.session_id);
      }
      else if(rec_number < int_app_info_ptr->eons_info.num_of_ocsgl_rec)
      {
        /* Read the next OCSGL record */
        next_rec_len = int_app_info_ptr->eons_info.len_of_ocsgl_rec;
        read_next_record = TRUE;
      }
      break;

    case MMGSDI_USIM_OCSGT:
      /* Last record of OCSGT is read, need to start caching OHNBN */
      if(rec_number == int_app_info_ptr->eons_info.num_of_ocsgt_rec)
      {
        UIM_MSG_HIGH_1("Last record 0x%x of OCSGT is read, start caching OHNBN",
                       rec_number);

        read_next_record = FALSE;
        mmgsdi_eons_trigger_ocsgl_ocsgt_ohnbn_cache(
          cnf_ptr->response_header.session_id);
      }
      else if(rec_number < int_app_info_ptr->eons_info.num_of_ocsgt_rec)
      {
        /* Read the next OCSGL record */
        next_rec_len = int_app_info_ptr->eons_info.len_of_ocsgt_rec;
        read_next_record = TRUE;
      }
      break;

    case MMGSDI_USIM_OHNBN:
      if(rec_number == int_app_info_ptr->eons_info.num_of_ohnbn_rec)
      {
        read_next_record = FALSE;
      }
      if(rec_number < int_app_info_ptr->eons_info.num_of_ohnbn_rec)
      {
        /* Read the next HNBN record */
        next_rec_len = int_app_info_ptr->eons_info.len_of_ohnbn_rec;
        read_next_record = TRUE;
      }
      break;

    default:
      UIM_MSG_ERR_1("Unexpected EF, 0x%x", file_enum);
      return;
  }

  if(read_next_record)
  {
    mmgsdi_status = mmgsdi_session_read_record(
                      cnf_ptr->response_header.session_id,
                      cnf_ptr->read_cnf.access,
                      rec_number + 1,
                      next_rec_len,
                      mmgsdi_eons_csg_read_rec_cb,
                      0);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_3("Read record failed, status 0x%x, file enum 0x%x, rec_num 0x%x",
                    mmgsdi_status,
                    cnf_ptr->read_cnf.access.file.file_enum,
                    (rec_number + 1));
    }
  }
} /* mmgsdi_eons_csg_read_rec_cb */


/*===========================================================================
FUNCTION MMGSDI_CSG_SEARCH_RECORDS_AND_UPDATE_CACHE

DESCRIPTION
  This function performs search to determine if there are any invalid
  records in a file. If there are invalid records, it updates the cache
  for those records with the empty data. The function then queues a
  read request for the first valid record.

DEPENDENCIES
  NONE

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_csg_search_records_and_update_cache(
   mmgsdi_session_id_type      session_id,
   mmgsdi_file_enum_type       file_enum,
   mmgsdi_len_type             rec_len,
   mmgsdi_rec_num_type         total_recs
)
{
  mmgsdi_return_enum_type      mmgsdi_status  = MMGSDI_SUCCESS;
  mmgsdi_rec_num_type          rec_cnt        = 0;
  mmgsdi_len_type              empty_rec_cnt  = 0;
  mmgsdi_rec_num_type          read_rec_num   = 1;
  mmgsdi_access_type           file_access;
  mmgsdi_data_type             invalid_record_data;
  mmgsdi_data_type             empty_data_buf;

  memset(&invalid_record_data, 0x00, sizeof(mmgsdi_data_type));
  memset(&empty_data_buf, 0x00, sizeof(mmgsdi_data_type));
  memset(&file_access, 0x00, sizeof(mmgsdi_access_type));

  if((total_recs <= 0) ||
     (rec_len <= 0))
  {
    return MMGSDI_ERROR;
  }

  UIM_MSG_MED_0("mmgsdi_csg_search_records_and_update_cache");

  /* perform search for invalid records */
  mmgsdi_status = mmgsdi_csg_search_records_sync(session_id,
                                                 file_enum,
                                                 rec_len,
                                                 &invalid_record_data);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_2("Search file, 0x%x failed, status: 0x%x",
                   file_enum, mmgsdi_status);
  }

  if((mmgsdi_status == MMGSDI_SUCCESS) &&
     (invalid_record_data.data_len != 0) &&
     (invalid_record_data.data_len <= total_recs))
  {
    UIM_MSG_HIGH_3("In file 0x%x, total recs: 0x%x, valid recs: 0x%x",
             file_enum, total_recs, (total_recs - invalid_record_data.data_len));

    empty_data_buf.data_len = rec_len;

    /* atleast one empty record is found, so need to update
       the cache */
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(empty_data_buf.data_ptr,
                                       empty_data_buf.data_len);
    if(empty_data_buf.data_ptr != NULL)
    {
      memset(empty_data_buf.data_ptr, 0xFF,
             empty_data_buf.data_len);
    }
  }

  file_access.access_method  = MMGSDI_EF_ENUM_ACCESS;
  file_access.file.file_enum = file_enum;

  for(rec_cnt = 1; rec_cnt <= total_recs; rec_cnt++)
  {
    if((invalid_record_data.data_len > 0) &&
       (invalid_record_data.data_ptr != NULL) &&
       (empty_data_buf.data_len > 0) &&
       (empty_data_buf.data_ptr != NULL))
    {
      for(empty_rec_cnt = 0; empty_rec_cnt < invalid_record_data.data_len;
          empty_rec_cnt++)
      {
        if(rec_cnt == invalid_record_data.data_ptr[empty_rec_cnt])
        {
          (void)mmgsdi_cache_add_record(session_id,
                                        file_access,
                                        rec_cnt,
                                        &empty_data_buf);

          if(rec_cnt == read_rec_num)
          {
            /* If the invalid record and current valid record numbers match,
               move to next record as the current valid record to read */
            read_rec_num++;
          }
          break;
        }
      }
    }
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(empty_data_buf.data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(invalid_record_data.data_ptr);

  /* read the records starting from read_rec_num */
  if(read_rec_num <= total_recs)
  {
    mmgsdi_status = mmgsdi_session_read_record(
       session_id,
       file_access,
       read_rec_num,
       rec_len,
       mmgsdi_eons_csg_read_rec_cb,
       0);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_3("read record failed, status 0x%x, file enum 0x%x, rec_num 0x%x",
                    mmgsdi_status, file_enum, read_rec_num);
    }
  }
  else
  {
    /* If there are no valid records in EF-ACSGL, then
       trigger EF-OCSGL access/cache. It is not needed to 
       access HNBN/CSGT files */
    UIM_MSG_HIGH_1("No valid records in file, 0x%x", file_enum);
    if(file_enum == MMGSDI_USIM_ACSGL)
    {
      mmgsdi_eons_trigger_ocsgl_ocsgt_ohnbn_cache(session_id);
    }
    mmgsdi_status = MMGSDI_SUCCESS;
  }

  return mmgsdi_status;
} /* mmgsdi_csg_search_records_and_update_cache */


/*===========================================================================
FUNCTION MMGSDI_EONS_CSG_FILE_ATTR_CB

DESCRIPTION
  Function to read file attributes and initiate a read request

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_eons_csg_file_attr_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type  * cnf_ptr
)
{
  mmgsdi_return_enum_type    mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_int_app_info_type * int_app_info_ptr = NULL;
  mmgsdi_session_info_type * session_info_ptr = NULL;
  mmgsdi_file_enum_type      file_enum        = MMGSDI_MAX_FILE_ENUM;
  mmgsdi_rec_num_type        num_rec          = 0;
  mmgsdi_len_type            len_of_rec       = 0;

  if((cnf_ptr == NULL) ||
     (cnf != MMGSDI_GET_FILE_ATTR_CNF) ||
     (cnf_ptr->get_file_attr_cnf.file_attrib.file_type != MMGSDI_LINEAR_FIXED_FILE))
  {
    UIM_MSG_ERR_0("Error in get file attributes confirmation");
    return;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    cnf_ptr->get_atr_cnf.response_header.session_id,
                    NULL,
                    NULL,
                    &int_app_info_ptr,
                    NULL,
                    &session_info_ptr,
                    NULL);

  if((mmgsdi_status != MMGSDI_SUCCESS) ||
     (int_app_info_ptr == NULL) ||
     (session_info_ptr == NULL))
  {
    UIM_MSG_ERR_0("Unable to get session app info");
    return;
  }

  file_enum = cnf_ptr->get_file_attr_cnf.access.file.file_enum;

  /* In case ACSGL or OCSGL is present but CSGT/HNBN or OCSGT/OHNBN
     are not, continue with caching the other EFs */
  if(status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_1("Failed to read file attributes for file: 0x%x", file_enum);
    switch(file_enum)
    {
      case MMGSDI_USIM_ACSGL:
        /* Set the flag */
        int_app_info_ptr->eons_info.is_get_file_attr_acsgl_done = TRUE;

        /* Continue to cache CSGT */
        mmgsdi_eons_trigger_acsgl_csgt_hnbn_cache(cnf_ptr->response_header.session_id);
        break;

      case MMGSDI_USIM_CSGT:
        /* Set the flag */
        int_app_info_ptr->eons_info.is_get_file_attr_csgt_done = TRUE;

        /* Continue to cache HNBN */
        mmgsdi_eons_trigger_acsgl_csgt_hnbn_cache(cnf_ptr->response_header.session_id);
        break;

      case MMGSDI_USIM_HNBN:
        /* Set the flag */
        int_app_info_ptr->eons_info.is_get_file_attr_hnbn_done = TRUE;

        /* Continue to cache OCSGL */
        mmgsdi_eons_trigger_ocsgl_ocsgt_ohnbn_cache(cnf_ptr->response_header.session_id);
        break;

      case MMGSDI_USIM_OCSGL:
        /* Set the flag */
        int_app_info_ptr->eons_info.is_get_file_attr_ocsgl_done = TRUE;

        /* Continue to cache OCSGT */
        mmgsdi_eons_trigger_ocsgl_ocsgt_ohnbn_cache(cnf_ptr->response_header.session_id);
        break;

      case MMGSDI_USIM_OCSGT:
        /* Set the flag */
        int_app_info_ptr->eons_info.is_get_file_attr_ocsgt_done = TRUE;

        /* Continue to cache OHNBN */
        mmgsdi_eons_trigger_ocsgl_ocsgt_ohnbn_cache(cnf_ptr->response_header.session_id);
        break;

      case MMGSDI_USIM_OHNBN:
        /* Set the flag */
        int_app_info_ptr->eons_info.is_get_file_attr_ohnbn_done = TRUE;

#ifdef FEATURE_UIM_PERSISTENT_CACHE
        /* at this point all CSG files are read from card, start to build
           the persistent cache */
        mmgsdi_csg_build_persistent_cache(cnf_ptr->response_header.session_id,
                                          cnf_ptr->response_header.slot_id,
                                          int_app_info_ptr);
#endif /* FEATURE_UIM_PERSISTENT_CACHE */
        break;

      default:
        UIM_MSG_ERR_1("Unexpected EF, 0x%x", file_enum);
        return;
    }
  }
  else
  {
    num_rec =
      cnf_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.num_of_rec;

    len_of_rec =
      cnf_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.rec_len;

    switch(file_enum)
    {
      case MMGSDI_USIM_ACSGL:
        if(!int_app_info_ptr->eons_info.is_get_file_attr_acsgl_done)
        {
          /* Save the len, num of recs and set the file attr status */
          int_app_info_ptr->eons_info.len_of_acsgl_rec = len_of_rec;
          int_app_info_ptr->eons_info.num_of_acsgl_rec = num_rec;
          int_app_info_ptr->eons_info.is_get_file_attr_acsgl_done = TRUE;

          UIM_MSG_HIGH_0("EF-ACSGL file attr done! search records now...");

          mmgsdi_status = mmgsdi_csg_search_records_and_update_cache(
                                    cnf_ptr->response_header.session_id,
                                    MMGSDI_USIM_ACSGL,
                                    len_of_rec,
                                    num_rec);
          if(mmgsdi_status == MMGSDI_SUCCESS)
          {
            /* In case of EF-ACSGL, the search function starts the
               read record operation. So if it is a success, return.
               The select of other CSG EFs is triggered in
               mmgsdi_eons_csg_read_rec_cb(), when the last record of EF-ACSGL
               is read */
            return;
          }
        }
        else
        {
          UIM_MSG_ERR_1("Get file attr. for ACSGL done: 0x%x",
                        int_app_info_ptr->eons_info.is_get_file_attr_acsgl_done);
        }
        break;

      case MMGSDI_USIM_CSGT:
        if(!int_app_info_ptr->eons_info.is_get_file_attr_csgt_done)
        {
          /* Save the len, num of recs and set the file attr status */
          int_app_info_ptr->eons_info.len_of_csgt_rec = len_of_rec;
          int_app_info_ptr->eons_info.num_of_csgt_rec = num_rec;
          int_app_info_ptr->eons_info.is_get_file_attr_csgt_done = TRUE;
        }
        else
        {
          UIM_MSG_ERR_1("Get file attr. for CSGT done: 0x%x",
                        int_app_info_ptr->eons_info.is_get_file_attr_csgt_done);
        }
        break;

      case MMGSDI_USIM_HNBN:
        if(!int_app_info_ptr->eons_info.is_get_file_attr_hnbn_done)
        {
         int_app_info_ptr->eons_info.len_of_hnbn_rec = len_of_rec;
         int_app_info_ptr->eons_info.num_of_hnbn_rec = num_rec;
         int_app_info_ptr->eons_info.is_get_file_attr_hnbn_done = TRUE;
        }
        else
        {
          UIM_MSG_ERR_1("Get file attr. for HNBN done: 0x%x",
                        int_app_info_ptr->eons_info.is_get_file_attr_hnbn_done);
        }
        break;

      case MMGSDI_USIM_OCSGL:
        if(!int_app_info_ptr->eons_info.is_get_file_attr_ocsgl_done)
        {
          /* Save the len, num of recs and set the file attr status */
          int_app_info_ptr->eons_info.len_of_ocsgl_rec = len_of_rec;
          int_app_info_ptr->eons_info.num_of_ocsgl_rec = num_rec;
          int_app_info_ptr->eons_info.is_get_file_attr_ocsgl_done = TRUE;

          UIM_MSG_HIGH_0("EF-OCSGL file attr done! search records now...");

          mmgsdi_status = mmgsdi_csg_search_records_and_update_cache(
                                     cnf_ptr->response_header.session_id,
                                     MMGSDI_USIM_OCSGL,
                                     len_of_rec,
                                     num_rec);
          if(mmgsdi_status == MMGSDI_SUCCESS)
          {
            /* In case of EF-OCSGL, the search function starts the
               read record operation. So if it is a success, return.
               The select of other CSG EFs is triggered in
               mmgsdi_eons_csg_read_rec_cb(), when the last record of EF-OCSGL
               is read */
            return;
          }
        }
        else
        {
          UIM_MSG_ERR_1("Get file attr. for OCSGL done: 0x%x",
                        int_app_info_ptr->eons_info.is_get_file_attr_ocsgl_done);
        }
        break;

      case MMGSDI_USIM_OCSGT:
        if(!int_app_info_ptr->eons_info.is_get_file_attr_ocsgt_done)
        {
          /* Save the len, num of recs and set the file attr status */
          int_app_info_ptr->eons_info.len_of_ocsgt_rec = len_of_rec;
          int_app_info_ptr->eons_info.num_of_ocsgt_rec = num_rec;
          int_app_info_ptr->eons_info.is_get_file_attr_ocsgt_done = TRUE;
        }
        else
        {
          UIM_MSG_ERR_1("Get file attr. for OCSGT done: 0x%x",
                        int_app_info_ptr->eons_info.is_get_file_attr_ocsgt_done);
        }
        break;

      case MMGSDI_USIM_OHNBN:
        if(!int_app_info_ptr->eons_info.is_get_file_attr_ohnbn_done)
        {
         int_app_info_ptr->eons_info.len_of_ohnbn_rec = len_of_rec;
         int_app_info_ptr->eons_info.num_of_ohnbn_rec = num_rec;
         int_app_info_ptr->eons_info.is_get_file_attr_ohnbn_done = TRUE;
        }
        else
        {
          UIM_MSG_ERR_1("Get file attr. for OHNBN done: 0x%x",
                        int_app_info_ptr->eons_info.is_get_file_attr_ohnbn_done);
        }
        break;

      default:
        UIM_MSG_ERR_1("Unexpected EF, 0x%x", file_enum);
        return;
    }

    /* Start reading records */
    if(num_rec > 0)
    {
      mmgsdi_rec_num_type rec_num = 1;
      mmgsdi_status = mmgsdi_session_read_record(
                        cnf_ptr->response_header.session_id,
                        cnf_ptr->get_file_attr_cnf.access,
                        rec_num,
                        len_of_rec,
                        mmgsdi_eons_csg_read_rec_cb,
                        0);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_3("Read record failed, status 0x%x, file enum 0x%x, rec_num 0x%x",
                      mmgsdi_status,
                      cnf_ptr->get_file_attr_cnf.access.file.file_enum,
                      rec_num);
      }
    }
    else
    {
      /* One of the CSG EFs has no records */
      if(cnf_ptr->get_file_attr_cnf.access.file.file_enum == MMGSDI_USIM_ACSGL ||
         cnf_ptr->get_file_attr_cnf.access.file.file_enum == MMGSDI_USIM_HNBN  ||
         cnf_ptr->get_file_attr_cnf.access.file.file_enum == MMGSDI_USIM_OCSGT )
      {
        mmgsdi_eons_trigger_ocsgl_ocsgt_ohnbn_cache(cnf_ptr->response_header.session_id);
      }
      else if(cnf_ptr->get_file_attr_cnf.access.file.file_enum == MMGSDI_USIM_CSGT)
      {
        mmgsdi_eons_trigger_acsgl_csgt_hnbn_cache(cnf_ptr->response_header.session_id);
      }
      else
      {
        return;
      }
    }
  }
} /* mmgsdi_eons_csg_file_attr_cb */


/*===========================================================================
FUNCTION MMGSDI_EONS_TRIGGER_ACSGL_CSGT_HNBN_CACHE

DESCRIPTION
  Function to trigger caching ACSGL/CSGT/HNBN records

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_eons_trigger_acsgl_csgt_hnbn_cache(
  mmgsdi_session_id_type session_id)
{
  mmgsdi_return_enum_type    mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type * int_app_info_ptr = NULL;
  mmgsdi_session_info_type * session_info_ptr = NULL;
  mmgsdi_access_type         file_access;
  mmgsdi_slot_id_enum_type   slot_id          = MMGSDI_MAX_SLOT_ID_ENUM;

  memset(&file_access, 0x00, sizeof(mmgsdi_access_type));

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    session_id,
                    &slot_id,
                    NULL,
                    &int_app_info_ptr,
                    NULL,
                    &session_info_ptr,
                    NULL);
  if((mmgsdi_status != MMGSDI_SUCCESS) ||
     (int_app_info_ptr == NULL) ||
     (session_info_ptr == NULL) ||
     (slot_id == MMGSDI_MAX_SLOT_ID_ENUM))
  {
    UIM_MSG_ERR_0("Failed to retrieve session app info");
    return;
  }

  /* Check whether session type is either GW PRI or GW SEC*/
  if((session_info_ptr->session_type != MMGSDI_GW_PROV_PRI_SESSION) &&
     (session_info_ptr->session_type != MMGSDI_GW_PROV_SEC_SESSION))
  {
    UIM_MSG_ERR_1("Unexpected session type: 0x%x, not performing ACSGL/HNBN caching",
                  session_info_ptr->session_type);
    return;
  }

  if(int_app_info_ptr->app_data.app_type != MMGSDI_APP_USIM)
  {
    UIM_MSG_HIGH_1("App type is: 0x%x, accepted CSG caching is not needed",
                   int_app_info_ptr->app_data.app_type);
    return;
  }

#ifdef FEATURE_UIM_PERSISTENT_CACHE
  /* populate persistent cache*/
  mmgsdi_csg_populate_from_persistent_cache(session_id,
                                            slot_id,
                                            int_app_info_ptr);
#endif /* FEATURE_UIM_PERSISTENT_CACHE */

  file_access.file.file_enum = MMGSDI_NO_FILE_ENUM;
  if(!int_app_info_ptr->eons_info.is_get_file_attr_acsgl_done)
  {
    UIM_MSG_HIGH_0("Trigger ACSGL Cache");
    file_access.access_method  = MMGSDI_EF_ENUM_ACCESS;
    file_access.file.file_enum = MMGSDI_USIM_ACSGL;
  }
  else if(!int_app_info_ptr->eons_info.is_get_file_attr_csgt_done)
  {
    UIM_MSG_HIGH_0("Trigger CSGT Cache");
    file_access.access_method  = MMGSDI_EF_ENUM_ACCESS;
    file_access.file.file_enum = MMGSDI_USIM_CSGT;
  }
  else if(!int_app_info_ptr->eons_info.is_get_file_attr_hnbn_done)
  {
    UIM_MSG_HIGH_0("Trigger HNBN Cache");
    file_access.access_method  = MMGSDI_EF_ENUM_ACCESS;
    file_access.file.file_enum = MMGSDI_USIM_HNBN;
  }
  else
  {
    /* Trigger caching OCSGL/OCSGT/OHNBN */
    mmgsdi_eons_trigger_ocsgl_ocsgt_ohnbn_cache(session_id);
    return;
  }

  mmgsdi_status = mmgsdi_session_get_file_attr(
                    session_id,
                    file_access,
                    mmgsdi_eons_csg_file_attr_cb,
                    0);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    if((mmgsdi_status == MMGSDI_NOT_SUPPORTED) &&
       (file_access.file.file_enum == MMGSDI_USIM_ACSGL))
    {
      UIM_MSG_HIGH_1("File enum: 0x%x not supported", file_access.file.file_enum);

      /* ACSGL is not present. No need to continue to cache CSGT/HNBN.
         Trigger caching OCSGL/OCSGT/OHNBN */
      mmgsdi_eons_trigger_ocsgl_ocsgt_ohnbn_cache(session_id);
    }
    else
    {
      UIM_MSG_ERR_2("Get file attr failed, status: 0x%x, file enum: 0x%x",
                    mmgsdi_status, file_access.file.file_enum);
    }
  }
} /* mmgsdi_eons_trigger_acsgl_csgt_hnbn_cache */


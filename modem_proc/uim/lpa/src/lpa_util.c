/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            L P A   U T I L I T Y  F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the utility functions related to critical sections.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/lpa/src/lpa_util.c#1 $ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/15/16   av      Fix protocol check for SMDP address
05/15/16   ll      ASN1 decoder enhancement
05/12/16   av      Fix confirmation code parsing in activation code
04/11/16   av      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "rex.h"
#include <stringl/stringl.h>

#include "lpa.h"
#include "lpa_platform.h"
#include "lpa_util.h"
#include "secapi.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/
#define LPA_SHA256_DIGEST_LEN                                32
#define LPA_ACTIVATION_CODE_AC_FORMAT_VALUE                 0x31
#define LPA_ACTIVATION_CODE_DELIMITER_VALUE                  '$'
#define LPA_ACTIVATION_CODE_PREFIX                        "LPA:"

#define LPA_UTIL_ONE_BYTE_LEN                               0x80
#define LPA_UTIL_NUM_LEN_BYTES_MASK                         0x7F

#define LPA_UTIL_INITIALIZE_SECURE_CHANNEL_TAG_0            0xBF
#define LPA_UTIL_INITIALIZE_SECURE_CHANNEL_TAG_1            0x23
#define LPA_UTIL_INITIALIZE_SECURE_CHANNEL_OFFSET_IN_BPP    0x00

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION LPA_UTIL_CRIT_SECT_INIT

DESCRIPTION
  Initializes the critical sections.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lpa_util_crit_sect_init (
  void
)
{
  rex_init_crit_sect(&lpa_task_state_crit_sect);
  rex_init_crit_sect(&lpa_data_crit_sect);
} /* lpa_crit_sect_init */


/*===========================================================================
FUNCTION LPA_UTIL_HASH_SHA256

DESCRIPTION
  This function generates the SHA256 digest for the input data.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
lpa_result_enum_type lpa_util_hash_sha256(
  uint8                         *data_ptr,
  uint32                         data_len,
  lpa_data_type                 *hashed_data_ptr
)
{
  secerrno_enum_type          result        = E_SUCCESS;
  secapi_handle_type          hsh_handle    = NULL;
  secapi_hsh_param_data_type  hsh_param;

  if((data_ptr == NULL) ||
     (data_len == 0) ||
     (hashed_data_ptr == NULL))
  {
    return LPA_ERROR_INCORRECT_PARAMS;
  }

  memset(&hsh_param, 0x00, sizeof(secapi_hsh_param_data_type));

  hashed_data_ptr->data_ptr = LPA_MALLOC(LPA_SHA256_DIGEST_LEN);
  if(hashed_data_ptr->data_ptr == NULL)
  {
    return LPA_ERROR_HEAP_EXHAUSTED;
  }

  result = secapi_new(&hsh_handle, SECAPI_SHA256);
  if(result != E_SUCCESS)
  {
    LPA_FREE(hashed_data_ptr->data_ptr);
    return LPA_GENERIC_ERROR;
  }

  hsh_param.mode = SECAPI_HSH_INPUT_MODE_ATOMIC;
  result = secapi_hsh_set_param(hsh_handle, SECAPI_SHA256,
                                SECAPI_HSH_INPUT_MODE,
                                &hsh_param);
  if(result != E_SUCCESS)
  {
    LPA_MSG_HIGH_1("lpa_util_hash_sha256: secapi_hsh_set_param returned 0x%x", result);
    LPA_FREE(hashed_data_ptr->data_ptr);
    (void)secapi_delete(&hsh_handle);
    return LPA_GENERIC_ERROR;
  }

  hsh_param.common.exec_platform = SECAPI_EXEC_PLATFORM_ARM;
  result = secapi_hsh_set_param(hsh_handle, SECAPI_SHA256,
                                SECAPI_HSH_INPUT_MODE,
                                &hsh_param);
  if(result != E_SUCCESS)
  {
    LPA_MSG_HIGH_1("lpa_util_hash_sha256: secapi_hsh_set_param returned 0x%x", result);
    LPA_FREE(hashed_data_ptr->data_ptr);
    (void)secapi_delete(&hsh_handle);
    return LPA_GENERIC_ERROR;
  }

  result = secapi_hsh_create_digest(hsh_handle,
                                    SECAPI_SHA256,
                                    data_ptr,
                                    data_len,
                                    hashed_data_ptr->data_ptr);

  (void)secapi_delete(&hsh_handle);

  if(result != E_SUCCESS)
  {
    LPA_FREE(hashed_data_ptr->data_ptr);
    LPA_MSG_ERR_1("SECAPI for digest creation returned: 0x%x", result);
    return LPA_GENERIC_ERROR;
  }

  hashed_data_ptr->data_len = LPA_SHA256_DIGEST_LEN;

  return LPA_SUCCESS;
} /* lpa_util_hash_sha256 */


/*===========================================================================
   FUNCTION:      LPA_UTIL_CONVERT_LPA_SLOT_TO_MMGSDI_SLOT

   DESCRIPTION:


   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     lpa_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
lpa_result_enum_type lpa_util_convert_lpa_slot_to_mmgsdi_slot(
  lpa_slot_id_enum_type          lpa_slot,
  mmgsdi_slot_id_enum_type      *mmgsdi_slot
)
{
  lpa_result_enum_type         lpa_status = LPA_GENERIC_ERROR;

  if(mmgsdi_slot != NULL)
  {
    if(lpa_slot == LPA_SLOT_1)
    {
      lpa_status = LPA_SUCCESS;
      *mmgsdi_slot = MMGSDI_SLOT_1;
    }
    else if(lpa_slot == LPA_SLOT_2)
    {
      lpa_status = LPA_SUCCESS;
      *mmgsdi_slot = MMGSDI_SLOT_2;
    }
  }

 return lpa_status;
} /* lpa_util_convert_lpa_slot_to_mmgsdi_slot */


/*===========================================================================
   FUNCTION:      LPA_UTIL_CONVERT_MMGSDI_SLOT_TO_LPA_SLOT

   DESCRIPTION:


   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     lpa_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
lpa_result_enum_type lpa_util_convert_mmgsdi_slot_to_lpa_slot(
  mmgsdi_slot_id_enum_type      mmgsdi_slot,
  lpa_slot_id_enum_type        *lpa_slot
)
{
  lpa_result_enum_type        lpa_status = LPA_GENERIC_ERROR;

  if(lpa_slot != NULL)
  {
    if(mmgsdi_slot == MMGSDI_SLOT_1)
    {
      lpa_status = LPA_SUCCESS;
      *lpa_slot = LPA_SLOT_1;
    }
    else if(mmgsdi_slot == MMGSDI_SLOT_2)
    {
      lpa_status = LPA_SUCCESS;
      *lpa_slot = LPA_SLOT_2;
    }
  }

 return lpa_status;
} /* lpa_util_convert_mmgsdi_slot_to_lpa_slot */


/*===========================================================================
   FUNCTION:      LPA_UTIL_CONVERT_MMGSDI_STATUS_TO_LPA_STATUS

   DESCRIPTION:
     Utility to convert MMGSDI Status to LPA status

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     lpa_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
lpa_result_enum_type lpa_util_convert_mmgsdi_status_to_lpa_status(
  mmgsdi_return_enum_type                          mmgsdi_status
)
{
  switch (mmgsdi_status)
  {
    case MMGSDI_SUCCESS:
      return LPA_SUCCESS;

    case MMGSDI_ERROR:
      return LPA_GENERIC_ERROR;

    case MMGSDI_INCORRECT_PARAMS:
      return LPA_ERROR_INCORRECT_PARAMS;

    case MMGSDI_SELECT_AID_FAILED:
      return LPA_ERROR_SELECT_ISDR_FAILED;

    default:
      break;
  }

  return LPA_GENERIC_ERROR;
} /* lpa_convert_mmgsdi_status_to_lpa */


/*===========================================================================
   FUNCTION:      LPA_UTIL_IS_SLOT_VALID

   DESCRIPTION:
     Utility to find out if the slot is a valid LPA slot

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     boolean

   SIDE EFFECTS:
     None
===========================================================================*/
boolean lpa_util_is_slot_valid(
  lpa_slot_id_enum_type             slot_id
)
{
  if(slot_id != LPA_SLOT_1 &&
     slot_id != LPA_SLOT_2)
  {
    return FALSE;
  }

   return TRUE;
} /* lpa_util_is_slot_valid */


/*===========================================================================
   FUNCTION:      LPA_UTIL_IS_ICCID_VALID

   DESCRIPTION:
     Utility to find out if the ICCID is of valid length or not

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     boolean

   SIDE EFFECTS:
     None
===========================================================================*/
boolean lpa_util_is_iccid_valid(
  lpa_iccid_type             iccid
)
{
  if(iccid.data_len < LPA_ICCID_LEN)
  {
    return FALSE;
  }

  return TRUE;
} /* lpa_util_is_iccid_valid */


/*===========================================================================
   FUNCTION:      LPA_UTIL_FREE_LPA_RESPONSE_PTR

   DESCRIPTION:
     Utility to free the members of response pointer malloc'd for sending
     LPA response to the LPA client via client's callback

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
void lpa_util_free_lpa_response_ptr(
  lpa_response_data_type                *lpa_response_ptr
)
{
  uint8                        index     = 0;

  if(lpa_response_ptr == NULL)
  {
    return;
  }

  switch(lpa_response_ptr->msg_type)
  {
    case LPA_ENABLE_PROFILE_MSG:
    case LPA_DISABLE_PROFILE_MSG:
    case LPA_DELETE_PROFILE_MSG:
    case LPA_CARD_MEMORY_RESET_MSG:
    case LPA_ADD_PROFILE_MSG:
    case LPA_ADD_OR_UPDATE_PROFILE_NICKNAME_MSG:
    case LPA_GET_EID_MSG:
      /* Nothing to free... all the items in the response pointer for these
         requests, if any, are static items */
      break;
    case LPA_GET_PROFILES_INFO_MSG:
      while(index < lpa_response_ptr->message.get_profiles_info_resp.num_profiles &&
            index < (sizeof(lpa_response_ptr->message.get_profiles_info_resp.profile_info)/sizeof(lpa_response_ptr->message.get_profiles_info_resp.profile_info[0])))
      {
        /* Only Icon is dynamically allocated. All other items in the get
           profiles info resp are static arrays...no need to worry about them */
        LPA_FREE(lpa_response_ptr->message.get_profiles_info_resp.profile_info[index].icon.data_ptr);
        index++;
      }
      break;
    default:
       break;
  }
} /* lpa_util_free_lpa_response_ptr */


/*===========================================================================
   FUNCTION:      LPA_UTIL_MAP_SW1_SW2_TO_LPA_STATUS

   DESCRIPTION:
     Utility to create lpa status from SW1/SW2 returned by the card

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     lpa_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
lpa_result_enum_type lpa_util_map_sw1_sw2_to_lpa_status(
  mmgsdi_sw_type                   sw
)
{
  lpa_result_enum_type        lpa_status = LPA_GENERIC_ERROR;

  if((sw.sw1 == 0x90 && sw.sw2 == 0x00) ||
     (sw.sw1 == 0x00 && sw.sw2 == 0x00))
  {
    return LPA_SUCCESS;
  }

  switch (sw.sw1)
  {
    case 0x6A:
      switch (sw.sw2)
      {
        case 0x88:
          lpa_status = LPA_ERROR_REFERENCE_DATA_NOT_FOUND;
          break;
        case 0x80:
          lpa_status = LPA_GENERIC_ERROR;
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }

  return lpa_status;
} /* lpa_util_map_sw1_sw2_to_lpa_status */


/*===========================================================================
FUNCTION LPA_UTIL_GET_LEN_FROM_TLV

DESCRIPTION
  Function parses the input buffer that contains the length bytes of a TLV
  and returns back a single uint32 with length derived from those bytes
  (For length values greater than 127, the length is encoded in a multiple
  bytes).

DEPENDENCIES
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_util_get_len_from_tlv (
  uint32                              *out_len_ptr,
  uint8                               *out_num_len_bytes_ptr,
  uint32                               in_data_len,
  uint8                               *in_data_ptr
)
{
  uint8                     offset = 0;

  if(in_data_ptr == NULL ||
     in_data_len == 0 ||
     out_len_ptr == NULL)
  {
    return LPA_GENERIC_ERROR;
  }

  if(in_data_ptr[offset] < LPA_UTIL_ONE_BYTE_LEN)
  {
    /* Length is encoded in just one byte */
    *out_len_ptr = in_data_ptr[offset];
    if(out_num_len_bytes_ptr)
    {
      *out_num_len_bytes_ptr = 1;
    }
  }
  else if(in_data_ptr[offset] > LPA_UTIL_ONE_BYTE_LEN &&
          in_data_len > (offset + 1))
  {
    uint8  len_of_len              = 0;
    uint8  num_len_bytes           = 0;
    uint32 temp_len                = 0;

    /* Length is encoded in multiple bytes */
    num_len_bytes = (in_data_ptr[offset++] & LPA_UTIL_NUM_LEN_BYTES_MASK);
    if(num_len_bytes == 0 ||
       num_len_bytes > 4 || /* We don't allow more than 4
                               bytes long (uint32) length field */
       in_data_len < (offset + num_len_bytes))
    {
      return LPA_GENERIC_ERROR;
    }
    while(len_of_len < num_len_bytes)
    {
      temp_len = in_data_ptr[offset++];
      *out_len_ptr |=
        (temp_len << (8*(num_len_bytes - len_of_len - 1)));
      len_of_len ++;
    }
    if(out_num_len_bytes_ptr)
    {
      *out_num_len_bytes_ptr = num_len_bytes + 1;
    }
  }
  else
  {
    /* Length = 0x80 indicating indefinite length format(we don't support that)
       OR
       in_data_len is invalid */
    return LPA_GENERIC_ERROR;
  }

  return LPA_SUCCESS;
} /* lpa_util_get_len_from_tlv */


/*===========================================================================
FUNCTION LPA_UTIL_PARSE_ACTIVATION_CODE

DESCRIPTION
  Function parses an activation code and stores the parsed data in a struct

DEPENDENCIES
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_util_parse_activation_code(
  const lpa_data_type              *raw_activation_code,
  lpa_activation_code_type         *parsed_activation_code
)
{
  uint8 offset      = 0;   /* uint8 is enough as max length of raw activation
                              code is 255 bytes per spec */
  uint8 smdp_offset = 0;

  if(raw_activation_code == NULL ||
     parsed_activation_code == NULL ||
     raw_activation_code->data_len > LPA_ACTIVATION_CODE_LEN_MAX ||
     raw_activation_code->data_ptr == NULL ||
     raw_activation_code->data_len < LPA_ACTIVATION_CODE_LEN_MIN)
  {
    return LPA_GENERIC_ERROR;
  }

  /* Activation code *may* be prefixed with "LPA:". */
  if(raw_activation_code->data_len >= strlen(LPA_ACTIVATION_CODE_PREFIX) &&
     !memcmp(raw_activation_code->data_ptr,
             LPA_ACTIVATION_CODE_PREFIX,
             (uint8) strlen(LPA_ACTIVATION_CODE_PREFIX)))
  {
    /* Activation code starts with "LPA:" */
    offset = (uint8) strlen(LPA_ACTIVATION_CODE_PREFIX);
  }

  /* After the optional "LPA:" prefix, the first byte must be 1 and
     the second byte must be the delimiter */
  if(raw_activation_code->data_ptr[offset++] !=
       LPA_ACTIVATION_CODE_AC_FORMAT_VALUE ||
     raw_activation_code->data_ptr[offset] !=
       LPA_ACTIVATION_CODE_DELIMITER_VALUE)
  {
    /* Invalid AC_Format or Delimiter */
    LPA_MSG_ERR_0("lpa_util_parse_activation_code: Invalid data after LPA: prefix");
    return LPA_GENERIC_ERROR;
  }

  offset++; /* Move to the SMDP address */
  while (offset < raw_activation_code->data_len &&
         raw_activation_code->data_ptr[offset] !=
           LPA_ACTIVATION_CODE_DELIMITER_VALUE &&
         smdp_offset < LPA_SMDP_ADDRESS_LEN_MAX)
  {
    parsed_activation_code->smdp_address.data[smdp_offset++] =
      raw_activation_code->data_ptr[offset++];
  }
  parsed_activation_code->smdp_address.data_len = smdp_offset;
  /* Make sure a non-null SMDP address is present in the activation code */
  if(parsed_activation_code->smdp_address.data_len == 0)
  {
    LPA_MSG_ERR_0("lpa_util_parse_activation_code: Null SMDP address");
    return LPA_GENERIC_ERROR;
  }

  offset++; /* Move to AC Token */
  smdp_offset = 0;
  while (offset < raw_activation_code->data_len &&
         raw_activation_code->data_ptr[offset] !=
           LPA_ACTIVATION_CODE_DELIMITER_VALUE  &&
         smdp_offset < LPA_ACTIVATION_TOKEN_LEN_MAX)
  {
    /* Note that AC_Token can be blank... in that case, we will not enter
       this loop */
    parsed_activation_code->activation_token.data[smdp_offset] =
      raw_activation_code->data_ptr[offset++];
    smdp_offset++;
  }
  parsed_activation_code->activation_token.data_len = smdp_offset;

  offset++; /* Move to SMDPID */
  smdp_offset = 0;
  while (offset < raw_activation_code->data_len &&
         raw_activation_code->data_ptr[offset] !=
           LPA_ACTIVATION_CODE_DELIMITER_VALUE  &&
         smdp_offset < LPA_SMDP_ID_LEN_MAX)
  {
    /* This is Optional SMDPid */
    parsed_activation_code->smdp_id.data[smdp_offset] =
      raw_activation_code->data_ptr[offset++];
    smdp_offset++;
  }
  parsed_activation_code->smdp_id.data_len = smdp_offset;

  if(++offset < raw_activation_code->data_len)
  {
    if (raw_activation_code->data_ptr[offset] == 0x31)
    {
      parsed_activation_code->confirmation_code_required = TRUE;
    }
    else
    {
      /* Invalid value of confirmation code required flag */
      LPA_MSG_ERR_1("lpa_util_parse_activation_code: Invalid value of conf code required flag:0x%x",
                    parsed_activation_code->confirmation_code_required);
      return LPA_GENERIC_ERROR;
    }
  }

  if(++offset < raw_activation_code->data_len)
  {
    /* Looks like there is more data in the activation code, beyond the
       confirmation-code required byte, which is wrong. Confirmation-code-
       required is supposed to be the last byte of data in the activation
       code... error out */
    return LPA_GENERIC_ERROR;
  }

  return LPA_SUCCESS;
} /* lpa_util_parse_activation_code */


/*===========================================================================
FUNCTION LPA_UTIL_GET_DEVICE_INFO

DESCRIPTION
  Function retrieves the device info which comprises of the TAC and
  rat capabilities.

DEPENDENCIES
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_util_get_device_info(
  lpa_util_device_info_type                *device_info
)
{
  uint8                        i = 0;

  if(device_info == NULL)
  {
    return LPA_GENERIC_ERROR;
  }

  /* For now we are hardcoding TAC and RAT support. We can potentially
     determine which RATs are supported based on some policyman/mcs
     APIs.
     The release numbers for the RATs supported however have to be hardcoded -
     there is no way to determine them dynamically. */
  device_info->tac.tac_len = 0x08;
  for(i = 0; i < device_info->tac.tac_len; i++)
  {
    device_info->tac.tac[i]  = 0x01;
  }

  device_info->device_capability.gsm_supported_release_present           = 1;
  device_info->device_capability.gsm_supported_release                   =
    LPA_UTIL_GSM_SUPPORTED_RELEASE;                         /* Release 11 */
  device_info->device_capability.utran_supported_release_present         = 1;
  device_info->device_capability.utran_supported_release                 =
    LPA_UTIL_UTRAN_SUPPORTED_RELEASE;                       /* Release 11 */
  device_info->device_capability.cdma2000onex_supported_release_present  = 1;
  device_info->device_capability.cdma2000onex_supported_release          =
    LPA_UTIL_CDMA2000_ONEX_SUPPORTED_RELEASE;               /* Per 1x team  */
  device_info->device_capability.cdma2000hrpd_supported_release_present  = 1;
  device_info->device_capability.cdma2000hrpd_supported_release          =
    LPA_UTIL_CDMA2000_HRPD_SUPPORTED_RELEASE;               /* Per 1x team */
  device_info->device_capability.cdma2000ehrpd_supported_release_present = 1;
  device_info->device_capability.cdma2000ehrpd_supported_release         =
    LPA_UTIL_CDMA2000_EHRPD_SUPPORTED_RELEASE;              /* Per 1x team */
  device_info->device_capability.eutran_supported_release_present        = 1;
  device_info->device_capability.eutran_supported_release                =
    LPA_UTIL_EUTRAN_SUPPORTED_RELEASE;                      /* Release 11 */
  device_info->device_capability.contactless_supported_release_present   = 1;
  device_info->device_capability.contactless_supported_release           =
    LPA_UTIL_CONTACTLESS_SUPPORTED_RELEASE;                 /* Release 7 */

  return LPA_SUCCESS;
} /* lpa_util_get_device_info */


/*===========================================================================
FUNCTION LPA_UTIL_PARSE_BPP

DESCRIPTION
  Function processes the http response for GET BPP and retrieves the TLVs
  from it. The output is a bunch of buffers containing those individual TLVs.

DEPENDENCIES
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_util_parse_bpp(
  lpa_data_type                     in_bpp,
  lpa_bpp_tlvs_data_type           *out_bpp_tlvs
)
{
  lpa_result_enum_type              lpa_status    = LPA_SUCCESS;
  uint32                            offset        = 0;

  if(in_bpp.data_len < 3 ||
     in_bpp.data_ptr == NULL ||
     out_bpp_tlvs == NULL)
  {
    return LPA_GENERIC_ERROR;
  }

  out_bpp_tlvs->tlvs_num = 0;

  while(offset < in_bpp.data_len &&
        out_bpp_tlvs->tlvs_num <
          sizeof(out_bpp_tlvs->tlvs_data)/sizeof(out_bpp_tlvs->tlvs_data[0]))
  {
    uint32        total_tlv_len         = 0;
    uint32        tlv_len               = 0;
    uint32        num_tlv_tag_bytes     = 1;
    uint8         num_tlv_len_bytes     = 0;

    if(offset == LPA_UTIL_INITIALIZE_SECURE_CHANNEL_OFFSET_IN_BPP)
    {
      if(in_bpp.data_ptr[offset] != LPA_UTIL_INITIALIZE_SECURE_CHANNEL_TAG_0 &&
         in_bpp.data_ptr[offset + 1] != LPA_UTIL_INITIALIZE_SECURE_CHANNEL_TAG_1)
      {
        /* First TLV in the input buffer must always be the InitializeSecureChannel
           TLV, with BF23 as the tag */
        lpa_status = LPA_GENERIC_ERROR;
        break;
      }
      num_tlv_tag_bytes = 2;
    }

    if(in_bpp.data_len <= (offset + num_tlv_tag_bytes))
    {
      lpa_status = LPA_GENERIC_ERROR;
      break;
    }

    lpa_status = lpa_util_get_len_from_tlv(&tlv_len,
                                           &num_tlv_len_bytes,
                                           in_bpp.data_len - (offset + num_tlv_tag_bytes),
                                           &in_bpp.data_ptr[offset + num_tlv_tag_bytes]);
    if(lpa_status != LPA_SUCCESS)
    {
      break;
    }

    /* Compute the number of bytes of T+L+V for the current TLV */
    total_tlv_len = num_tlv_tag_bytes + num_tlv_len_bytes + tlv_len;

    /* Make sure the number of bytes of T+L+V of all the TLVs parsed so far
       is less than the entire input buffer length */
    if(in_bpp.data_len < (total_tlv_len + offset))
    {
      lpa_status = LPA_GENERIC_ERROR;
      break;
    }

    out_bpp_tlvs->tlvs_data[out_bpp_tlvs->tlvs_num].data_len = total_tlv_len;

    out_bpp_tlvs->tlvs_data[out_bpp_tlvs->tlvs_num].data_ptr =
      LPA_MALLOC(out_bpp_tlvs->tlvs_data[out_bpp_tlvs->tlvs_num].data_len);
    if(out_bpp_tlvs->tlvs_data[out_bpp_tlvs->tlvs_num].data_ptr == NULL)
    {
      lpa_status = LPA_GENERIC_ERROR;
      break;
    }
    /* Copy the entire T+L+V for the current iteration over to the
       output structure */
    memscpy(out_bpp_tlvs->tlvs_data[out_bpp_tlvs->tlvs_num].data_ptr,
            out_bpp_tlvs->tlvs_data[out_bpp_tlvs->tlvs_num].data_len,
            &in_bpp.data_ptr[offset],
            total_tlv_len);
    out_bpp_tlvs->tlvs_num++;
    offset += total_tlv_len;
  }

  if(lpa_status != LPA_SUCCESS)
  {
    uint8     i = 0;
    while(i < (sizeof(out_bpp_tlvs->tlvs_data)/sizeof(out_bpp_tlvs->tlvs_data[0])) &&
          i < out_bpp_tlvs->tlvs_num &&
          out_bpp_tlvs->tlvs_data[i].data_ptr)
    {
      LPA_FREE(out_bpp_tlvs->tlvs_data[i].data_ptr);
      i++;
    }
    out_bpp_tlvs->tlvs_num = 0;
  }

  return lpa_status;
} /* lpa_util_parse_bpp */

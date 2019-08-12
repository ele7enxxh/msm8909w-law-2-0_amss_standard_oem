/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            L P A   A S N 1   D E C O D E R S


GENERAL DESCRIPTION

  This source file contains the ASN1 content decoders.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/lpa/src/lpa_asn1_decoder.c#1 $ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/22/16   ll      Peek into profile installation result
09/07/16   av      Do not return error if no data returned for GET STATUS
06/21/16   av      Add SVN check for supporting phase1, phase1.1 and phase2
05/12/16   ll      ASN1 decoder enhancement
04/11/16   av      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "rex.h"
#include <stringl/stringl.h>
#include "lpa.h"
#include "lpalib.h"
#include "lpa_util.h"
#include "lpa_platform.h"
#include "lpa_lpd.h"
#include "lpa_asn1_decoder.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/
#define LPA_ASN1_GET_PROFILES_INFO_APDU_MIN_LEN                       0x02
#define LPA_ASN1_GET_PROFILES_INFO_EMPTY_LIST_LEN                     0x00
#define LPA_ASN1_GET_PROFILES_INFO_TAG                                0xE3
#define LPA_ASN1_GET_PROFILES_INFO_ICCID_TAG                          0x5A
#define LPA_ASN1_GET_PROFILES_INFO_ISDP_AID_TAG                       0x4F
#define LPA_ASN1_GET_PROFILES_INFO_STATE_BYTE1_TAG                    0x9F
#define LPA_ASN1_GET_PROFILES_INFO_STATE_BYTE2_TAG                    0x70
#define LPA_ASN1_GET_PROFILES_INFO_STATE_LEN                          0x01
#define LPA_ASN1_GET_PROFILES_INFO_NICKNAME_TAG                       0x90
#define LPA_ASN1_GET_PROFILES_INFO_SERVICE_PROVIDER_NAME_TAG          0x91
#define LPA_ASN1_GET_PROFILES_INFO_NAME_TAG                           0x92
#define LPA_ASN1_GET_PROFILES_INFO_ICON_TYPE_TAG                      0x93
#define LPA_ASN1_GET_PROFILES_INFO_ICON_TAG                           0x94
#define LPA_ASN1_GET_PROFILES_INFO_ICON_LEN                           0x01

#define LPA_ASN1_GET_EUICC_INFO_BYTE1_TAG                             0xBF
#define LPA_ASN1_GET_EUICC_INFO_BYTE2_TAG                             0x20
#define LPA_ASN1_GET_EUICC_INFO_SVN_TAG                               0x82
#define LPA_ASN1_GET_EUICC_INFO_CERT_INFO_TAG                         0x85

#define LPA_ASN1_GET_EUICC_INFO_BYTE1_TAG                             0xBF
#define LPA_ASN1_GET_EUICC_INFO_BYTE2_TAG                             0x20

#define LPA_ASN1_PROFILE_INSTALLATION_RESULT_BYTE1_TAG                0xBF
#define LPA_ASN1_PROFILE_INSTALLATION_RESULT_BYTE2_TAG                0x28
#define LPA_ASN1_INITIALISE_SECURE_CHANNEL_BYTE1_TAG                  0xBF
#define LPA_ASN1_INITIALISE_SECURE_CHANNEL_BYTE2_TAG                  0x23
#define LPA_ASN1_INITIALISE_SECURE_CHANNEL_SIGNATURE_BYTE1_TAG        0x5F
#define LPA_ASN1_INITIALISE_SECURE_CHANNEL_SIGNATURE_BYTE2_TAG        0x37
#define LPA_ASN1_LOAD_BPP_87_TAG                                      0x87
#define LPA_ASN1_LOAD_BPP_86_TAG                                      0x86
#define LPA_ASN1_PROFILE_INSTALLATION_RECEIPT_BYTE1_TAG               0xBF
#define LPA_ASN1_PROFILE_INSTALLATION_RECEIPT_BYTE2_TAG               0x27
#define LPA_ASN1_PROFILE_INSTALLATION_RECEIPT_TRANSACTION_ID_TAG      0x80
#define LPA_ASN1_PROFILE_INSTALLATION_RECEIPT_RESULT_CODE_TAG         0x81

#define LPA_ASN1_ONE_BYTE_LEN                                         0x80
#define LPA_ASN1_NUM_LEN_BYTES_MASK                                   0x7F

#define LPA_ASN1_PROFILE_INSTALLATION_RECEIPT_RESULT_CODE_LEN         LPA_INSTALLATION_RESULT_CODE_LEN

/*=============================================================================

                       GLOBAL VARIABLES

=============================================================================*/

/*=============================================================================

                              FUNCTIONS

=============================================================================*/
/*===========================================================================
FUNCTION LPA_ASN1_DECODE_PROFILE_INSTALLATION_RECEIPT

DESCRIPTION
  Function parses the ASN1 formatted profile installation receipt as part of
  profile installation result.
 
DEPENDENCIES
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static lpa_result_enum_type lpa_asn1_decode_profile_installation_receipt (
  uint32                                               data_len,
  uint8                                               *data_ptr,
  lpa_lpd_profile_installation_result_code_array_type  result_code_array
)
{
  uint32 offset                        = 0;
  uint8  current_tlv_len_len           = 0;
  uint32 current_tlv_len               = 0;

  if(NULL == result_code_array || NULL == data_ptr)
  {
    /* NULL result_code_array or data_ptr */
    return LPA_GENERIC_ERROR;
  }
  
  if(data_len < 1 ||
     LPA_ASN1_PROFILE_INSTALLATION_RECEIPT_TRANSACTION_ID_TAG != data_ptr[offset++])
  {
    return LPA_GENERIC_ERROR;
  }

  if((data_len <= offset) ||
      LPA_SUCCESS != lpa_util_get_len_from_tlv(&current_tlv_len,
                                               &current_tlv_len_len,
                                               (data_len - offset),
                                               (data_ptr  + offset)) ||
      0 == current_tlv_len)
  {
    /* Any of the following error happened: 
       1.Length field not present
       2.Unable to decode length field
       3.No data in the 1-16bytes transaction ID value field.*/
    return LPA_GENERIC_ERROR;
  }
  offset += current_tlv_len_len;
  /* Skip the value field */
  offset += current_tlv_len;

  /* Parse Result code */
  if ((data_len - 2) < offset || 
       data_ptr[offset++] != LPA_ASN1_PROFILE_INSTALLATION_RECEIPT_RESULT_CODE_TAG ||
       data_ptr[offset++] != LPA_ASN1_PROFILE_INSTALLATION_RECEIPT_RESULT_CODE_LEN ||
      (data_len - LPA_ASN1_PROFILE_INSTALLATION_RECEIPT_RESULT_CODE_LEN) < offset)
  {
    /* Any of the following error happened: 
       1.Not enough data for result code tag and len
       2.Incorrect tag or len value for result code
       3.Not enough data left for value field of the result code.*/
    return LPA_GENERIC_ERROR;   
  }

  (void)memscpy((void*)result_code_array,
                 sizeof(lpa_lpd_profile_installation_result_code_array_type),
                (void*)(data_ptr + offset),
                 LPA_ASN1_PROFILE_INSTALLATION_RECEIPT_RESULT_CODE_LEN);

  LPA_MSG_HIGH_2("LPA profile installation receipt result code[0] = 0x%x, [1] = 0x%x", 
                  result_code_array[0], result_code_array[1]); 
  return LPA_SUCCESS;
}/*lpa_asn1_decode_profile_installation_receipt*/


/*===========================================================================
FUNCTION LPA_ASN1_DECODE_PROFILE_INSTALLATION_RESULT

DESCRIPTION
  Function parses the ASN1 formatted APDU response received as a result of
  LoadBoundProfilePackage or GetProfileInstallationResult request.

DEPENDENCIES
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_asn1_decode_profile_installation_result (
  uint32                                               data_len,
  uint8                                               *data_ptr,
  lpa_lpd_profile_installation_result_code_array_type  result_code_array
)
{
  uint32 offset                        = 0;
  uint8  current_tlv_len_len           = 0;
  uint32 current_tlv_len               = 0;

  if(NULL == result_code_array || NULL == data_ptr)
  {
    /* NULL result_code_array or data_ptr */
    return LPA_GENERIC_ERROR;
  }

  /* Parse profile installation result */
  if(data_len < 2 ||
     LPA_ASN1_PROFILE_INSTALLATION_RESULT_BYTE1_TAG != data_ptr[offset++] ||
     LPA_ASN1_PROFILE_INSTALLATION_RESULT_BYTE2_TAG != data_ptr[offset++])
  {
    /* First tag bytes must be the profile installation result tag */
    return LPA_GENERIC_ERROR;
  }

  if(data_len <= offset ||
     LPA_SUCCESS != lpa_util_get_len_from_tlv(&current_tlv_len,
                                              &current_tlv_len_len,
                                              (data_len - offset),
                                              (data_ptr  + offset)) ||
     0 == current_tlv_len)
  {
    /* Any of the following error happened: 
       1.Length field not present
       2.Unable to decode length field
       3.No data in the manditory profile installation result.*/
    return LPA_GENERIC_ERROR;
  }
  offset += current_tlv_len_len;

  /* Parse the data of profile installation result */
  /* Parse eUICC’s InitialiseSecureChannel */
  if ((data_len - 2) < offset || 
       LPA_ASN1_INITIALISE_SECURE_CHANNEL_BYTE1_TAG != data_ptr[offset++] ||
       LPA_ASN1_INITIALISE_SECURE_CHANNEL_BYTE2_TAG != data_ptr[offset++])
  {
    /* Second tag bytes must be the initialise secure channel tag */
    return LPA_GENERIC_ERROR;
  }

  if((data_len <= offset) || 
      LPA_SUCCESS != lpa_util_get_len_from_tlv(&current_tlv_len,
                                               &current_tlv_len_len,
                                               (data_len - offset),
                                               (data_ptr  + offset)) ||
     0 == current_tlv_len)
  {
    /* Any of the following error happened: 
       1.Length field not present
       2.Unable to decode length field
       3.No data in the manditory Initialise Secure Channel response.*/
    return LPA_GENERIC_ERROR;
  }
  offset += current_tlv_len_len;
  /* Skip the value field */
  offset += current_tlv_len;

  /* Parse eUICC’s signature for InitialiseSecureChannel */
  if ((data_len - 2) < offset || 
       LPA_ASN1_INITIALISE_SECURE_CHANNEL_SIGNATURE_BYTE1_TAG != data_ptr[offset++] ||
       LPA_ASN1_INITIALISE_SECURE_CHANNEL_SIGNATURE_BYTE2_TAG != data_ptr[offset++])
  {
    /* Second tag bytes must be the initialise secure channel signature tag */
    return LPA_GENERIC_ERROR;
  }

  if((data_len <= offset) || 
      LPA_SUCCESS != lpa_util_get_len_from_tlv(&current_tlv_len,
                                               &current_tlv_len_len,
                                               (data_len - offset),
                                               (data_ptr  + offset)) ||
     0 == current_tlv_len)
  {
    /* Any of the following error happened: 
       1.Length field not present
       2.Unable to decode length field
       3.No data in the manditory Initialise Secure Channel response signature.*/
    return LPA_GENERIC_ERROR;
  }
  offset += current_tlv_len_len;
  /* Skip the value field */
  offset += current_tlv_len;

  /* Parse 87 TLVs if we have we have 87 tag byte */
  /* The first 2 87 TLVs are mandatory however we don't put restriction here for now */
  while (data_len > offset &&
         LPA_ASN1_LOAD_BPP_87_TAG == data_ptr[offset])
  {
    /* Move to next byte after tag */
    offset++;
    if((data_len <= offset) ||
        LPA_SUCCESS != lpa_util_get_len_from_tlv(&current_tlv_len,
                                                 &current_tlv_len_len,
                                                 (data_len - offset),
                                                 (data_ptr  + offset)) ||
       0 == current_tlv_len)
    {
      /* Any of the following error happened: 
         1.Length field not present
         2.Unable to decode length field
         3.No data in 87 value field is not allowed.R-MAC should be present*/
      return LPA_GENERIC_ERROR;
    }
    offset += current_tlv_len_len;
    /* Skip the value field */
    offset += current_tlv_len;
  }

  /* Parse optional 86 TLVs if we have we have 86 tag byte */
  /* There should be one 1 mandatory 86 TLV, however we don't put restriction here for now */
  while (data_len > offset &&
         LPA_ASN1_LOAD_BPP_86_TAG == data_ptr[offset])
  {
    /*move to next byte after tag*/
    offset++;
    if((data_len <= offset) ||
        LPA_SUCCESS != lpa_util_get_len_from_tlv(&current_tlv_len,
                                                 &current_tlv_len_len,
                                                 (data_len - offset),
                                                 (data_ptr  + offset)) ||
        0 == current_tlv_len)
    {
      /* Any of the following error happened: 
         1.Length field not present
         2.Unable to decode length field
         3.No data in 86 value field is not allowed, R-MAC should be present.*/
      return LPA_GENERIC_ERROR;
    }
    offset += current_tlv_len_len;
    /* Skip the value field */
    offset += current_tlv_len;
  }

  /* Parse profile installation receipt */
  if ((data_len - 2) < offset || 
       data_ptr[offset++] != LPA_ASN1_PROFILE_INSTALLATION_RECEIPT_BYTE1_TAG ||
       data_ptr[offset++] != LPA_ASN1_PROFILE_INSTALLATION_RECEIPT_BYTE2_TAG)
  {
    /* Second tag bytes must be the receipt tag */
    return LPA_GENERIC_ERROR;
  }

  if((data_len <= offset) || 
      LPA_SUCCESS != lpa_util_get_len_from_tlv(&current_tlv_len,
                                               &current_tlv_len_len,
                                               (data_len - offset),
                                               (data_ptr  + offset)) ||
      0 == current_tlv_len)
  {
    /* Any of the following error happened: 
       1.Length field not present
       2.Unable to decode length field
       3.No data in receipt value field is not allowed due to mandatory data.*/
    return LPA_GENERIC_ERROR;
  }
  offset += current_tlv_len_len;

  /* Decode profile installation receipt */
  if(LPA_SUCCESS != lpa_asn1_decode_profile_installation_receipt(current_tlv_len,
                                                                (data_ptr + offset),
                                                                 result_code_array))
  {
    return LPA_GENERIC_ERROR;
  }

  return LPA_SUCCESS;
}/*lpa_asn1_decode_profile_installation_result*/


/*===========================================================================
FUNCTION LPA_ASN1_DECODE_GET_PROFILES_INFO_APDU

DESCRIPTION
  Function parses the ASN formatted APDU response received as a result of
  Get Profiles Info request. This function might MALLOC
  resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].icon.
  data_ptr (where profile_index ranges from 0 to LPA_NUM_PROFILES_MAX) which
  should be freed by the caller.

DEPENDENCIES
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_asn1_decode_get_profiles_info_apdu (
  uint32                                 data_len,
  uint8                                 *data_ptr,
  lpa_response_data_type                *resp_ptr
)
{
  uint32                       offset              = 0;
  uint32                       total_len           = 0;
  uint8                        profile_index       = 0;
  lpa_result_enum_type         lpa_status          = LPA_GENERIC_ERROR;
  uint8                        num_len_bytes = 0;

  if(resp_ptr == NULL)
  {
    return LPA_GENERIC_ERROR;
  }

  if(data_len == LPA_ASN1_GET_PROFILES_INFO_EMPTY_LIST_LEN)
  {
    /* When the eUICC does not have any profile, get_profiles_info returns
       empty list on some eUICCs, which is ok */
    return LPA_SUCCESS;
  }

  if(data_len < LPA_ASN1_GET_PROFILES_INFO_APDU_MIN_LEN ||
     data_ptr == NULL)
  {
    /* If the profile list is not empty (that is, data_len is not zero), at the
       very least GetProfilesInfo Tag and Length fields should be present */
    return LPA_GENERIC_ERROR;
  }

  while(profile_index < LPA_NUM_PROFILES_MAX)
  {
    uint32 profile_info_tag_len    = 0;

    if (data_ptr[offset++] != LPA_ASN1_GET_PROFILES_INFO_TAG)
    {
      /* First byte must be the GetProfilesInfo tag */
      lpa_status = LPA_GENERIC_ERROR;
      break;
    }

    if (data_ptr[offset] == 0x00)
    {
      /* This means none of the optional fields in the GetProfilesInfo structure
         are present. This is not a failure case. */
      lpa_status = LPA_SUCCESS;
      break;
    }
    num_len_bytes = 0;
    lpa_status = lpa_util_get_len_from_tlv(&profile_info_tag_len,
                                           &num_len_bytes,
                                           data_len - (offset),
                                           &data_ptr[offset]);
    if(lpa_status != LPA_SUCCESS)
    {
      break;
    }

    /* Move beyond the Length field of the GetProfilesInfo TLV */
    offset += num_len_bytes;

    total_len = profile_info_tag_len + offset;

    if (total_len > data_len)
    {
      lpa_status = LPA_GENERIC_ERROR;
    }

    if (profile_info_tag_len == 0x00)
    {
      LPA_MSG_MED_0("No profile info returned from the card");
      lpa_status = LPA_SUCCESS;
    }

    /* ICCID Tag 0x5A */
    if (total_len > (offset + 1) &&
        data_ptr[offset] == LPA_ASN1_GET_PROFILES_INFO_ICCID_TAG)
    {
      offset++;
      if (data_ptr[offset] != LPA_ICCID_LEN && data_ptr[offset] != 0)
      {
        /* ICCID length found not to be equal to 10 */
        LPA_MSG_ERR_1("Invalid ICCID length(0x%x) returned by the card", data_ptr[offset]);
        lpa_status = LPA_GENERIC_ERROR;
        break;
      }

      if (data_ptr[offset] == 0)
      {
        offset++;
      }
      else
      {
        resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].iccid.data_len =
        data_ptr[offset];
        offset++;
        if (total_len < (offset + LPA_ICCID_LEN))
        {
          lpa_status = LPA_GENERIC_ERROR;
          break;
        }
        memscpy(resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].iccid.data,
                sizeof(resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].iccid.data),
                &data_ptr[offset],
                resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].iccid.data_len);
        offset += LPA_ICCID_LEN;
      }
    }

    /* ISDPAID tag 0x4F */
    if (total_len > (offset + 1) &&
        data_ptr[offset] == LPA_ASN1_GET_PROFILES_INFO_ISDP_AID_TAG) /* T */
    {
      offset++;
      if (data_ptr[offset] > LPA_ISDP_AID_LEN)
      {
        /* ISDPAID length found to be greater than max allowed 16 bytes */
        LPA_MSG_ERR_1("Invalid ISDP AID length(0x%x) returned by the card", data_ptr[offset]);
        lpa_status = LPA_GENERIC_ERROR;
        break;
      }
      resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].isdp_aid.data_len =
        data_ptr[offset]; /* L */
      offset++;

      if (total_len <
            (offset + resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].isdp_aid.data_len))
      {
        lpa_status = LPA_GENERIC_ERROR;
        break;
      }

      memscpy(resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].isdp_aid.data,
              sizeof(resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].isdp_aid.data),
              &data_ptr[offset],
              resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].isdp_aid.data_len); /* V */
      offset += resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].isdp_aid.data_len;
    }

    /* ProfileState tag 0x9F70 */
    if (total_len > (offset + 2) &&
        data_ptr[offset] == LPA_ASN1_GET_PROFILES_INFO_STATE_BYTE1_TAG &&
        data_ptr[++offset] == LPA_ASN1_GET_PROFILES_INFO_STATE_BYTE2_TAG) /* T */
    {
      offset++;
      if(data_ptr[offset] != 0)
      {
        if (data_ptr[offset] != LPA_ASN1_GET_PROFILES_INFO_STATE_LEN) /* L */
        {
          /* ProfileState should be one byte long */
          LPA_MSG_ERR_1("Invalid Profile State length(0x%x) returned by the card", data_ptr[offset]);
          lpa_status = LPA_GENERIC_ERROR;
          break;
        }
        offset++;

        if (total_len < (offset + LPA_ASN1_GET_PROFILES_INFO_STATE_LEN))
        {
          lpa_status = LPA_GENERIC_ERROR;
          break;
        }

        if (data_ptr[offset] == 0x01)
        {
          resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].state =
            LPA_PROFILE_STATE_ENABLED;
        }
        else if (data_ptr[offset] == 0x00)
        {
          resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].state =
            LPA_PROFILE_STATE_DISABLED;
        }
        else
        {
          lpa_status = LPA_GENERIC_ERROR;
          break;
        }
      }
      offset++;
    }

    /* nickName tag 0x50 */
    if (total_len > (offset + 1) &&
        data_ptr[offset] == LPA_ASN1_GET_PROFILES_INFO_NICKNAME_TAG) /* T */
    {
      offset++;
      if (data_ptr[offset] > LPA_NAME_LEN_MAX)
      {
        /* nickName length found to be greater than max allowed 64 bytes */
        LPA_MSG_ERR_1("Invalid Nickname length(0x%x) returned by the card", data_ptr[offset]);
        lpa_status = LPA_GENERIC_ERROR;
        break;
      }
      resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].nickname.name_len =
        data_ptr[offset]; /* L */
      offset++;

      if (total_len <
            (offset + resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].nickname.name_len))
      {
        lpa_status = LPA_GENERIC_ERROR;
        break;
      }

      if (resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].nickname.name_len > 0)
      {
        /* Nickname may be 0, hence this if-check */
        memscpy(resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].nickname.name,
                sizeof(resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].nickname.name),
                &data_ptr[offset],
                resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].nickname.name_len); /* V */
        offset += resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].nickname.name_len;
      }
    }

    /* serviceProviderName tag 0x51 */
    if (total_len > (offset + 1) &&
        (data_ptr[offset] ==
           LPA_ASN1_GET_PROFILES_INFO_SERVICE_PROVIDER_NAME_TAG)) /* T */
    {
      offset++;
      if (data_ptr[offset] > LPA_SPN_LEN_MAX)
      {
        /* serviceProviderName length found to be greater than max allowed 32 bytes */
        LPA_MSG_ERR_1("Invalid SPN length(0x%x) returned by the card", data_ptr[offset]);
        lpa_status = LPA_GENERIC_ERROR;
        break;
      }
      resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].spn.name_len =
        data_ptr[offset]; /* L */
      offset++;

      if (total_len <
            (offset + resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].spn.name_len))
      {
        lpa_status = LPA_GENERIC_ERROR;
        break;
      }

      if (resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].spn.name_len > 0)
      {
        /* serviceProviderName may be 0, hence this if-check */
        memscpy(resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].spn.name,
                sizeof(resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].spn.name),
                &data_ptr[offset],
                resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].spn.name_len); /* V */
        offset += resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].spn.name_len;
      }
    }

    /* profileName tag 0x52 */
    if (total_len > (offset + 1) &&
        data_ptr[offset] == LPA_ASN1_GET_PROFILES_INFO_NAME_TAG) /* T */
    {
      offset++;
      if (data_ptr[offset] > LPA_NAME_LEN_MAX)
      {
        /* profileName length found to be greater than max allowed 64 bytes */
        LPA_MSG_ERR_1("Invalid ProfileName length(0x%x) returned by the card", data_ptr[offset]);
        lpa_status = LPA_GENERIC_ERROR;
        break;
      }
      resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].name.name_len =
        data_ptr[offset]; /* L */
      offset++;
      if (total_len < (offset + resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].name.name_len))
      {
        lpa_status = LPA_GENERIC_ERROR;
        break;
      }

      if (resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].name.name_len > 0)
      {
        /* profileName may be 0, hence this if-check */
        memscpy(resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].name.name,
                sizeof(resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].name.name),
                &data_ptr[offset],
                resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].name.name_len); /* V */
        offset += resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].name.name_len;
      }
    }

    /* iconType tag 0x53 */
    if (total_len > (offset + 1) &&
        data_ptr[offset] == LPA_ASN1_GET_PROFILES_INFO_ICON_TYPE_TAG) /* T */
    {
      offset++;
      if(data_ptr[offset] != 0)
      {
        if (data_ptr[offset] != LPA_ASN1_GET_PROFILES_INFO_ICON_LEN)
        {
          /* iconType length found to be greater than 1 byte */
          LPA_MSG_ERR_1("Invalid Icon Type length(0x%x) returned by the card", data_ptr[offset]);
          lpa_status = LPA_GENERIC_ERROR;
          break;
        }
        offset++;
        if (total_len < (offset + LPA_ASN1_GET_PROFILES_INFO_ICON_LEN))
        {
          lpa_status = LPA_GENERIC_ERROR;
          break;
        }

        resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].icon_type =
          data_ptr[offset]; /* V */
      }
      offset++;
    }

    /* icon tag 0x54... this can be at max 1024 bytes... so, length may be
       coded up in at max 2 bytes */
    if (total_len > (offset + 1) &&
        data_ptr[offset] == LPA_ASN1_GET_PROFILES_INFO_ICON_TAG) /* T */
    {
      uint32 icon_tag_len = 0;

      offset++;
      num_len_bytes = 0;
      lpa_status = lpa_util_get_len_from_tlv(&icon_tag_len,
                                             &num_len_bytes,
                                             data_len - (offset),
                                             &data_ptr[offset]);
      if(lpa_status != LPA_SUCCESS)
      {
        break;
      }

      /* Move beyond the Length field of the GetProfilesInfo TLV */
      offset += num_len_bytes;

      if (total_len < (offset + icon_tag_len))
      {
        lpa_status = LPA_GENERIC_ERROR;
        break;
      }

      if(icon_tag_len != 0)
      {
        if (icon_tag_len > LPA_ICON_LEN_MAX)
        {
          LPA_MSG_ERR_1("Invalid Icon length(0x%x) returned by the card", icon_tag_len);
          lpa_status = LPA_GENERIC_ERROR;
          break;
        }

        resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].icon.data_len = icon_tag_len;
        resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].icon.data_ptr =
        LPA_MALLOC(icon_tag_len);

        if (resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].icon.data_ptr == NULL)
        {
          return LPA_ERROR_HEAP_EXHAUSTED;
        }

        memscpy(resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].icon.data_ptr,
                resp_ptr->message.get_profiles_info_resp.profile_info[profile_index].icon.data_len,
              &data_ptr[offset],
              icon_tag_len); /* V */
        offset += icon_tag_len;
      }
    }

    profile_index++;

    if(!(data_len > (offset + 1) &&
          data_ptr[offset] == LPA_ASN1_GET_PROFILES_INFO_TAG)) /* T */
    {
      break;
    }
  }

  resp_ptr->message.get_profiles_info_resp.num_profiles = profile_index;

  return lpa_status;
} /* lpa_asn1_decode_get_profiles_info_apdu */


/*===========================================================================
FUNCTION LPA_ASN1_DECODE_EUICC_INFO_APDU

DESCRIPTION
  Function parses the ASN1 formatted APDU response of Get EUICC Info and
  fetches/decodes the SVN and certificate info from it.

DEPENDENCIES
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_asn1_decode_euicc_info_apdu (
  uint32                                 data_len,
  uint8                                 *data_ptr,
  lpa_lpd_get_euicc_info_resp_type      *resp_ptr
)
{
  uint32                       offset                        = 0;
  uint32                       total_len                     = 0;
  uint32                       euicc_info_tag_len            = 0;
  uint32                       temp_euicc_info_tag_len       = 0;

  if(resp_ptr == NULL)
  {
    return LPA_GENERIC_ERROR;
  }

  /* Lets fetch SVN from the asn1 formatted EUICC Info */
  if(data_len < 8 ||
     data_ptr == NULL)
  {
    /* We should at least have the SVN as the first element of the EUICC INFO.
       So, the apdu len should at least be 8 bytes... 2 bytes for the EUICC
       Info Tag, one byte for the EUICC Info Len, one byte for the SVN Tag,
       one byte for the SVN Len, 3 bytes for the SVN */
    return LPA_GENERIC_ERROR;
  }

  if(data_ptr[offset++] != LPA_ASN1_GET_EUICC_INFO_BYTE1_TAG ||
     data_ptr[offset++] != LPA_ASN1_GET_EUICC_INFO_BYTE2_TAG)
  {
    /* First 2 bytes byte must be the Get EUICC Info tag */
    return LPA_GENERIC_ERROR;
  }

  if(data_ptr[offset] == 0x00)
  {
    /* This means that there is no field in the EUICC Info structure. This
       is a failure. */
    return LPA_GENERIC_ERROR;
  }

  if(data_ptr[offset] < LPA_ASN1_ONE_BYTE_LEN)
  {
    /* Length is encoded in just one byte */
    euicc_info_tag_len = data_ptr[offset++];
  }
  else if (data_ptr[offset] > LPA_ASN1_ONE_BYTE_LEN)
  {
    uint8 len_of_len = 0;
    uint8 num_len_bytes = 0;

    /* Length is encoded in multiple bytes */
    num_len_bytes = (data_ptr[offset++] & LPA_ASN1_NUM_LEN_BYTES_MASK);
    if (num_len_bytes == 0 ||
        num_len_bytes > 4 ||
        data_len < (offset + num_len_bytes)) /* We don't allow more than 4 bytes long (uint32) length field */
    {
      return LPA_GENERIC_ERROR;
    }
    while (len_of_len < num_len_bytes)
    {
      temp_euicc_info_tag_len = data_ptr[offset++];
      euicc_info_tag_len |= (temp_euicc_info_tag_len << (8*(num_len_bytes - len_of_len - 1)));
      len_of_len ++;
    }
  }
  else
  {
    /* Length = 0x80... indicating indefinite length format...
       we don't support that yet!! */
    return LPA_GENERIC_ERROR;
  }

  total_len = euicc_info_tag_len + offset;

  if(total_len > data_len ||
     euicc_info_tag_len == 0x00)
  {
    return LPA_GENERIC_ERROR;
  }

  /* SVN Tag 0x82 */
  if (total_len > (offset + 1) &&
      data_ptr[offset] == LPA_ASN1_GET_EUICC_INFO_SVN_TAG)
  {
    offset++;
    if (data_ptr[offset] != LPA_SVN_LEN)
    {
      /* SVN length found not to be equal to 3 */
      return LPA_GENERIC_ERROR;
    }
    offset++;
    if (total_len < (offset + LPA_SVN_LEN))
    {
      return LPA_GENERIC_ERROR;
    }
    memscpy(&resp_ptr->svn,
            sizeof(resp_ptr->svn),
            &data_ptr[offset],
            LPA_SVN_LEN);
    offset += LPA_SVN_LEN;
  }

  /* certificate Info Tag 0x85 */
  if (total_len > (offset + 1) &&
      data_ptr[offset] == LPA_ASN1_GET_EUICC_INFO_CERT_INFO_TAG)
  {
    offset++;
    if (total_len < (offset + 1 + data_ptr[offset]))
    {
      return LPA_GENERIC_ERROR;
    }

    resp_ptr->cert_info.data_ptr = LPA_MALLOC(data_ptr[offset]);
    if (resp_ptr->cert_info.data_ptr == NULL)
    {
      return LPA_ERROR_HEAP_EXHAUSTED;
    }
    resp_ptr->cert_info.data_len = data_ptr[offset];

    memscpy(resp_ptr->cert_info.data_ptr,
            resp_ptr->cert_info.data_len,
            &data_ptr[offset+1],
            data_ptr[offset]);
  }

  return LPA_SUCCESS;
} /* lpa_asn1_decode_euicc_info_apdu */

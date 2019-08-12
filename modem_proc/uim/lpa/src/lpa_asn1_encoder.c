/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            L P A   A S N 1   E N C O D E R S   D E C O D E R S


GENERAL DESCRIPTION

  This source file contains LPA ASN1 encoder APIs and utility functions.

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


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/lpa/src/lpa_asn1_encoder.c#1 $ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/20/16   ll      Treat prepareDownload SMDP data as TLV instead raw data
05/15/16   ll      ASN1 encoder code clean up 
03/28/16   ll      Initial revision
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
#include "lpa_asn1_encoder.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/    
/* ASN1 len field */
#define LPA_ASN1_MAX_LEN_FIELD_LEN                    10
#define LPA_ASN1_GEN_TAG_LEN_SIMPLE                   1
#define LPA_ASN1_GEN_TAG_LEN_LOW_TAG_NUMBER_FORM      1
#define LPA_ASN1_GEN_TAG_LEN_HIGH_TAG_NUMBER_FORM     2
#define LPA_ASN1_SINGLE_LEN_BYTE_MAX_LEN_VALUE        127
#define LPA_ASN1_HIGHEST_BYTE_IN_32BIT_MASK           0xFF000000
#define LPA_ASN1_LOWEST_BYTE_IN_32BIT_MASK            0x000000FF
#define LPA_ASN1_NUMBER_OF_BIT_IN_BYTE                8
#define LPA_ASN1_LEN_LEN_BYTE_80_MASK                 0x80      
#define LPA_ASN1_DEVICE_CAPABILITIES_MAX_LEN          127

/* ASN1 tag field */
#define LPA_ASN1_MAX_TAG_FIELD_LEN                    2
/* Simple type tags -- reference: X.208 */
#define LPA_ASN1_GEN_TAG_OCTET                        0x4
#define LPA_ASN1_GEN_TAG_OBJECT_IDENTIFIER            0x6
#define LPA_ASN1_GEN_TAG_UTF8STRING                   0xC
/* Structured type tags */
#define LPA_ASN1_GEN_TAG_SEQUENCE                     0x30
/* Explicitly tagged type tags -- Context specific */
/* NOTE: low tag number(1 byte) for tag number <= 30, high tag number(2 byte) for tag number > 30*/
#define LPA_ASN1_GEN_TAG_CONTEXT_SPECIFIC0            0x80  
#define LPA_ASN1_GEN_TAG_CONTEXT_SPECIFIC1            0x81
#define LPA_ASN1_GEN_TAG_CONTEXT_SPECIFIC2            0x82
#define LPA_ASN1_GEN_TAG_CONTEXT_SPECIFIC3            0x83
#define LPA_ASN1_GEN_TAG_CONTEXT_SPECIFIC4            0x84
#define LPA_ASN1_GEN_TAG_CONTEXT_SPECIFIC5            0x85
#define LPA_ASN1_GEN_TAG_CONTEXT_SPECIFIC6            0x86
#define LPA_ASN1_GEN_TAG_CONTEXT_SPECIFIC16           0x90        
#define LPA_ASN1_GEN_TAG_CONTEXT_SPECIFIC33_FIRST     0xBF
#define LPA_ASN1_GEN_TAG_CONTEXT_SPECIFIC33_SECOND    0x21        
#define LPA_ASN1_GEN_TAG_CONTEXT_SPECIFIC41_FIRST     0xBF 
#define LPA_ASN1_GEN_TAG_CONTEXT_SPECIFIC41_SECOND    0x29  
/* Explicitly tagged type tags -- Application */
#define LPA_ASN1_GEN_TAG_APPLICATION26                0x5A        
/* Device Info Tag (per objSYS ASN1 encoder instead of 0x30) */
#define LPA_ASN1_TAG_DEVICE_CAPABILITY                0xA1

/* LPA ASN1 type Macro */
/* Number of oid should be less than number of char in the oid char array */
#define LPA_ASN1_MAX_SMDPOID_INT_LEN                  LPA_SMDP_ID_LEN_MAX

/* LPA ASN1 internal types */
/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_ASN1_CONST_DATA_TYPE

   DESCRIPTION:
     This structure is used for all the LPA ASN1 tags defined in SGP22
-----------------------------------------------------------------------------*/
typedef struct
{
  uint16         tag_len;
  uint8          tag[LPA_ASN1_MAX_TAG_FIELD_LEN];
} lpa_asn1_tag_field_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_ASN1_LEN_FIELD_TYPE

   DESCRIPTION:
     This structure is used for all the LPA ASN1 tags defined in SGP22
-----------------------------------------------------------------------------*/
typedef struct
{
  uint8          len_len;
  uint8          len[LPA_ASN1_MAX_LEN_FIELD_LEN];
} lpa_asn1_len_field_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_ASN1_OID_TYPE

   DESCRIPTION:
     This structure is used for the parsed OID(Object Identifier) from OID string.
     The OID data type consists of a sequence of one or more non-negative integers.
     The formal definition of OIDs comes from ITU-T recommendation X.208 (ASN.1)
     [Ed. Note: ISO/IEC 8824:1994]
-----------------------------------------------------------------------------*/
typedef struct
{
  uint16         oid_len;
  uint32         oid[LPA_ASN1_MAX_SMDPOID_INT_LEN];
}lpa_asn1_oid_type;

/*=============================================================================

                              FUNCTIONS

=============================================================================*/
/*===========================================================================
FUNCTION LPA_ASN1_GET_LEN_FIELD

DESCRIPTION
  Utility function to populate a TLV to an provided buffer TLV_array
  For multiple length field, it will be in format 0x8X, XX, XX...
  where the first X in the 0x8X indicate the number of byte of the rest of
  length fields.

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static lpa_result_enum_type lpa_asn1_util_get_len_field(
  uint32                     len,
  lpa_asn1_len_field_type*   len_ptr
)
{
  uint32 i        = 0;
  uint32 j        = 0;
  uint32 temp_len = len;

  if (NULL == len_ptr)
  {
    LPA_MSG_ERR_0("len_field_ptr is NULL");
    return LPA_GENERIC_ERROR;
  }

  if(len <= LPA_ASN1_SINGLE_LEN_BYTE_MAX_LEN_VALUE)
  {
    /* 1 byte length field */
    len_ptr->len_len    = 1;
    len_ptr->len[0]     = (uint8)(len & 0xFF);
  }
  else
  {
    /* multi byte length fields */
    for(i = sizeof(uint32); i > 0; i--)
    {
      /* Find the highest byte for number of length byte */
      if((len & LPA_ASN1_HIGHEST_BYTE_IN_32BIT_MASK) > 0)
      {
        len_ptr->len_len = i + 1;
        /* 0x8i where i indicate the length of length field */
        len_ptr->len[0]  = (uint8)(LPA_ASN1_LEN_LEN_BYTE_80_MASK | i);
        /* populate the each length field value */
        for(j = i; j > 0 ; j--)
        {
          len_ptr->len[j] = temp_len & LPA_ASN1_LOWEST_BYTE_IN_32BIT_MASK;
          temp_len = temp_len >> LPA_ASN1_NUMBER_OF_BIT_IN_BYTE;
        }
        break;
      }
      len = len << 8;
    }
  }

  return LPA_SUCCESS;
}/*lpa_asn1_util_get_len_field*/


/*===========================================================================
FUNCTION LPA_ASN1_POPULATE_TLV

DESCRIPTION
  Utility function to populate a TLV to an provided buffer TLV_array

DEPENDENCIES
  Caller is responsible to not call this function if the optional TLV
  is not present.
  Caller should ensure tlv_array_ptr is not NULL before calling. No return
  code added due to trying to aviod too many return value checks for the caller. 

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static void lpa_asn1_util_populate_tlv(
  lpa_data_type*             tlv_array_ptr,
  uint32                     max_tlv_len,
  lpa_asn1_tag_field_type*   tag_ptr,
  lpa_asn1_len_field_type*   len_ptr,
  lpa_data_type*             data_ptr
)
{
  if(NULL == tlv_array_ptr)
  {
    LPA_MSG_ERR_0("lpa_asn1_util_populate_tlv failed due to tlv_array_ptr is NULL");
    return;
  }

  if(NULL != tag_ptr)
  {
    memscpy((uint8*)&tlv_array_ptr->data_ptr[tlv_array_ptr->data_len],
             max_tlv_len,
             tag_ptr->tag,
             tag_ptr->tag_len);

   tlv_array_ptr->data_len += tag_ptr->tag_len;
  }

  if(NULL != len_ptr)
  {
    memscpy((uint8*)&tlv_array_ptr->data_ptr[tlv_array_ptr->data_len],
             max_tlv_len,
             (const uint8*)len_ptr->len,
             len_ptr->len_len);

    tlv_array_ptr->data_len += len_ptr->len_len;
  }

  if(NULL != data_ptr)
  {
    memscpy((uint8*)&tlv_array_ptr->data_ptr[tlv_array_ptr->data_len],
             max_tlv_len,
             (const uint8*)data_ptr->data_ptr,
             data_ptr->data_len);

   tlv_array_ptr->data_len += data_ptr->data_len;
  }
}/*lpa_asn1_util_populate_tlv*/


/*===========================================================================
FUNCTION LPA_ASN1_INIT_TAGS_SET_NICKNAME_REQUEST

DESCRIPTION
  Initialize the tags that will be used when encode SetNicknameRequest
  This function can be used to change the tag value easily e.g. per spec change

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void lpa_asn1_init_tags_set_nickname_request(lpa_asn1_tag_field_type* iccid_tag_ptr,
                                                    lpa_asn1_tag_field_type* profile_nickname_tag_ptr,
                                                    lpa_asn1_tag_field_type* set_nick_name_request_tag_ptr
)
{
  if ( NULL == iccid_tag_ptr ||
       NULL == profile_nickname_tag_ptr ||
       NULL == set_nick_name_request_tag_ptr ) 
  {
    LPA_MSG_ERR_0("Either iccid_tag_ptr, profile_nickname_tag_ptr or set_nick_name_request_tag_ptr is NULL");
    return;
  }

  iccid_tag_ptr->tag_len                        = LPA_ASN1_GEN_TAG_LEN_LOW_TAG_NUMBER_FORM;
  iccid_tag_ptr->tag[0]                         = LPA_ASN1_GEN_TAG_APPLICATION26;

  profile_nickname_tag_ptr->tag_len             = LPA_ASN1_GEN_TAG_LEN_LOW_TAG_NUMBER_FORM;
  profile_nickname_tag_ptr->tag[0]              = LPA_ASN1_GEN_TAG_CONTEXT_SPECIFIC16;

  set_nick_name_request_tag_ptr->tag_len        = LPA_ASN1_GEN_TAG_LEN_HIGH_TAG_NUMBER_FORM;
  set_nick_name_request_tag_ptr->tag[0]         = LPA_ASN1_GEN_TAG_CONTEXT_SPECIFIC41_FIRST;
	set_nick_name_request_tag_ptr->tag[1]   = LPA_ASN1_GEN_TAG_CONTEXT_SPECIFIC41_SECOND;
}/*lpa_asn1_init_tags_set_nickname_request*/


/*===========================================================================
FUNCTION LPA_ASN1_INIT_TAGS_DEVICE_CAPABILITIES

DESCRIPTION
  Initialize the tags that will be used when encode DeviceCapabilities
  This function can be used to change the tag value easily e.g. per spec change

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void lpa_asn1_init_tags_device_capabilities(lpa_asn1_tag_field_type* octet1_context_specific_tag_ptr,
                                                   lpa_asn1_tag_field_type* device_capabilities_tag_ptr
)
{
  if ( NULL == octet1_context_specific_tag_ptr ||
       NULL == device_capabilities_tag_ptr) 
  {
    LPA_MSG_ERR_0("Either octet1_context_specific_tag_ptr or device_capabilities_tag_ptr is NULL");
    return;
  }
  /* for the total technologies are less than 31, so use low tag number form */
  octet1_context_specific_tag_ptr->tag_len = LPA_ASN1_GEN_TAG_LEN_LOW_TAG_NUMBER_FORM;
  device_capabilities_tag_ptr->tag_len     = LPA_ASN1_GEN_TAG_LEN_LOW_TAG_NUMBER_FORM;
  device_capabilities_tag_ptr->tag[0]      = LPA_ASN1_TAG_DEVICE_CAPABILITY;
}/*lpa_asn1_init_tags_device_capabilities*/


/*===========================================================================
FUNCTION LPA_ASN1_INIT_TAGS_DEVICE_INFO

DESCRIPTION
  Initialize the tags that will be used when encode DeviceInfo
  This function can be used to change the tag value easily e.g. per spec change

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void lpa_asn1_init_tags_device_info(lpa_asn1_tag_field_type* tac_tag_ptr,
                                           lpa_asn1_tag_field_type* device_info_tag_ptr
)
{
  if ( NULL == tac_tag_ptr ||
       NULL == device_info_tag_ptr) 
  {
    LPA_MSG_ERR_0("Either tac_tag_ptr, device_info_tag_ptr or tac_tag_ptr is NULL");
    return;
  }
  /* Using 0x80 instead of octet_tag 0x04 for tac_tag;
     if change back to 0x04 remove the tac_tag and use common_octet_tag */
  tac_tag_ptr->tag_len              = LPA_ASN1_GEN_TAG_LEN_SIMPLE;
  tac_tag_ptr->tag[0]               = LPA_ASN1_GEN_TAG_CONTEXT_SPECIFIC0;

  device_info_tag_ptr->tag_len      = LPA_ASN1_GEN_TAG_LEN_SIMPLE;
  device_info_tag_ptr->tag[0]       = LPA_ASN1_GEN_TAG_SEQUENCE;
}/*lpa_asn1_init_tags_device_info*/


/*===========================================================================
FUNCTION LPA_ASN1_INIT_TAGS_PREPARE_DOWNLOAD_REQUEST

DESCRIPTION
  Initialize the tags that will be used when encode PrepareDownloadRequest
  This function can be used to change the tag value easily e.g. per spec change

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void lpa_asn1_init_tags_prepare_download_request(lpa_asn1_tag_field_type* activation_code_token_raw_tag_ptr,
                                                        lpa_asn1_tag_field_type* smdpOID_raw_tag_ptr,
                                                        lpa_asn1_tag_field_type* hashed_confirmation_code_raw_tag_ptr,
                                                        lpa_asn1_tag_field_type* prepare_download_request_tag_ptr
)
{
  if ( NULL == activation_code_token_raw_tag_ptr ||
       NULL == smdpOID_raw_tag_ptr ||
       NULL == hashed_confirmation_code_raw_tag_ptr ||
       NULL == prepare_download_request_tag_ptr ) 
  {
    LPA_MSG_ERR_0("Some tags of prepare download is NULL");
    return;
  }

  activation_code_token_raw_tag_ptr->tag_len    = LPA_ASN1_GEN_TAG_LEN_SIMPLE;
  activation_code_token_raw_tag_ptr->tag[0]     = LPA_ASN1_GEN_TAG_UTF8STRING;

  smdpOID_raw_tag_ptr->tag_len                  = LPA_ASN1_GEN_TAG_LEN_SIMPLE;
  smdpOID_raw_tag_ptr->tag[0]                   = LPA_ASN1_GEN_TAG_OBJECT_IDENTIFIER;

  hashed_confirmation_code_raw_tag_ptr->tag_len = LPA_ASN1_GEN_TAG_LEN_SIMPLE;
  hashed_confirmation_code_raw_tag_ptr->tag[0]  = LPA_ASN1_GEN_TAG_OCTET;

  prepare_download_request_tag_ptr->tag_len     = LPA_ASN1_GEN_TAG_LEN_HIGH_TAG_NUMBER_FORM;
  prepare_download_request_tag_ptr->tag[0]      = LPA_ASN1_GEN_TAG_CONTEXT_SPECIFIC33_FIRST;
  prepare_download_request_tag_ptr->tag[1]      = LPA_ASN1_GEN_TAG_CONTEXT_SPECIFIC33_SECOND;
}/*lpa_asn1_init_tags_prepare_download_request*/


/*===========================================================================
FUNCTION LPA_ASN1_ENCODE_DEVICE_CAPABILITIES

DESCRIPTION
  Encode DeviceCapabilities

DEPENDENCIES
  caller is responsible to free the the data pointer inside of parsed_smdpOID

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static lpa_result_enum_type lpa_asn1_encode_device_capabilities(
  lpa_util_device_capability_type             device_capability,
  lpa_data_type*                              device_capabilities_encoded_data_buff_ptr
)
{
  uint32                   device_capabilities_encoded_data_buff_total_len   = 0;
  uint32                   device_capabilities_encoded_data_buff_payload_len = 0;
  uint32                   number_of_capabilities                            = 0;
  /* tag fields */ 
  /* this is a temporary tag that is going to have fix length 1 and tag value from 0x80 ~ 0x86  */
  lpa_asn1_tag_field_type  octet1_context_specific_tag;
  lpa_asn1_tag_field_type  device_capabilities_tag;
  /* length field types -- the use of the common one is due to the length is fix for multiple TLV length */
  lpa_asn1_len_field_type  common_octet1_len;
  lpa_asn1_len_field_type  device_capabilities_len;
  /* value field types -- the use of the common one is due to the length is fix for multiple TLV value */
  lpa_data_type            common_capability;

  /* initialize tag fields */
  lpa_asn1_init_tags_device_capabilities(&octet1_context_specific_tag,
                                         &device_capabilities_tag);
  /* initialize len fields */
  memset(&common_octet1_len, 0, sizeof(lpa_asn1_len_field_type));
  memset(&device_capabilities_len, 0, sizeof(lpa_asn1_len_field_type));
  common_octet1_len.len_len    = 1;
  common_octet1_len.len[0]     = 0x1;
  /* initialize value fields */
  /* the common_capability structure will hold the value of release support for each technology*/
  memset(&common_capability, 0, sizeof(lpa_data_type));
  common_capability.data_len   = 1;

  /* get total length and malloc device_info_encoded_data_buff_ptr */
  if(device_capability.gsm_supported_release_present == TRUE)
  {
    number_of_capabilities++;
  }
  if(device_capability.utran_supported_release_present == TRUE)
  {
    number_of_capabilities++;
  }
  if(device_capability.cdma2000onex_supported_release_present == TRUE)
  {
    number_of_capabilities++;
  }
  if(device_capability.cdma2000hrpd_supported_release_present == TRUE)
  {
    number_of_capabilities++;
  }
  if(device_capability.cdma2000ehrpd_supported_release_present == TRUE)
  {
    number_of_capabilities++;
  }
  if(device_capability.eutran_supported_release_present == TRUE)
  {
    number_of_capabilities++;
  }
  if(device_capability.contactless_supported_release_present == TRUE)
  {
    number_of_capabilities++;
  }

  device_capabilities_encoded_data_buff_payload_len = number_of_capabilities*
                                                      (octet1_context_specific_tag.tag_len +
                                                      common_octet1_len.len_len +
                                                      common_octet1_len.len[0]);

  /* populate device_capabilities_len with the total paload data */
  device_capabilities_len.len_len = 1;
  /* The max total payload len should be less than 128, so one byte for len is enough */
  if(device_capabilities_encoded_data_buff_payload_len > LPA_ASN1_DEVICE_CAPABILITIES_MAX_LEN)
  {
    return LPA_GENERIC_ERROR;
  }

  device_capabilities_len.len[0] = (uint8)device_capabilities_encoded_data_buff_payload_len;

  /* get the total length with sequence header */
  device_capabilities_encoded_data_buff_total_len = device_capabilities_tag.tag_len +
                                                    device_capabilities_len.len_len +
                                                    device_capabilities_encoded_data_buff_payload_len;

  device_capabilities_encoded_data_buff_ptr->data_ptr = (uint8 *)LPA_MALLOC(
                                                        device_capabilities_encoded_data_buff_total_len);
  if (NULL == device_capabilities_encoded_data_buff_ptr->data_ptr)
  {
    LPA_MSG_ERR_1("device_capabilities_encoded_data_buff_ptr->data_ptr NULL malloc len = 0x%x",
                  device_capabilities_encoded_data_buff_total_len);
    return LPA_ERROR_HEAP_EXHAUSTED;
  }

  /* populate device_capabilities_encoded_data_buff_ptr */
  device_capabilities_encoded_data_buff_ptr->data_len = 0;
  /* populate header */
  lpa_asn1_util_populate_tlv(device_capabilities_encoded_data_buff_ptr,
                             device_capabilities_encoded_data_buff_total_len,
                             &device_capabilities_tag,
                             &device_capabilities_len,
                             NULL);

  /* populate capabilities fields */
  /* assume 0 means release is not information is not present */
  if(device_capability.gsm_supported_release_present == TRUE)
  {
    octet1_context_specific_tag.tag[0]        = LPA_ASN1_GEN_TAG_CONTEXT_SPECIFIC0;
    common_capability.data_ptr                = &device_capability.gsm_supported_release;

    lpa_asn1_util_populate_tlv(device_capabilities_encoded_data_buff_ptr,
                               device_capabilities_encoded_data_buff_total_len,
                               &octet1_context_specific_tag,
                               &common_octet1_len,
                               &common_capability);
  }

  if(device_capability.utran_supported_release_present == TRUE)
  {
    octet1_context_specific_tag.tag[0]        = LPA_ASN1_GEN_TAG_CONTEXT_SPECIFIC1;
    common_capability.data_ptr                = &device_capability.utran_supported_release;
    lpa_asn1_util_populate_tlv(device_capabilities_encoded_data_buff_ptr,
                               device_capabilities_encoded_data_buff_total_len,
                               &octet1_context_specific_tag,
                               &common_octet1_len,
                               &common_capability);
  }

  if(device_capability.cdma2000onex_supported_release_present == TRUE)
  {
    octet1_context_specific_tag.tag[0]        = LPA_ASN1_GEN_TAG_CONTEXT_SPECIFIC2;
    common_capability.data_ptr                = &device_capability.cdma2000onex_supported_release;
    lpa_asn1_util_populate_tlv(device_capabilities_encoded_data_buff_ptr,
                               device_capabilities_encoded_data_buff_total_len,
                               &octet1_context_specific_tag,
                               &common_octet1_len,
                               &common_capability);
  }

  if(device_capability.cdma2000hrpd_supported_release_present == TRUE)
  {
    octet1_context_specific_tag.tag[0]        = LPA_ASN1_GEN_TAG_CONTEXT_SPECIFIC3;
    common_capability.data_ptr                = &device_capability.cdma2000hrpd_supported_release;
    lpa_asn1_util_populate_tlv(device_capabilities_encoded_data_buff_ptr,
                               device_capabilities_encoded_data_buff_total_len,
                               &octet1_context_specific_tag,
                               &common_octet1_len,
                               &common_capability);
  }

  if(device_capability.cdma2000ehrpd_supported_release_present == TRUE)
  {
    octet1_context_specific_tag.tag[0]        = LPA_ASN1_GEN_TAG_CONTEXT_SPECIFIC4;
    common_capability.data_ptr                = &device_capability.cdma2000ehrpd_supported_release;
    lpa_asn1_util_populate_tlv(device_capabilities_encoded_data_buff_ptr,
                               device_capabilities_encoded_data_buff_total_len,
                               &octet1_context_specific_tag,
                               &common_octet1_len,
                               &common_capability);
  }

  if(device_capability.eutran_supported_release_present == TRUE)
  {
    octet1_context_specific_tag.tag[0]        = LPA_ASN1_GEN_TAG_CONTEXT_SPECIFIC5;
    common_capability.data_ptr                = &device_capability.eutran_supported_release;
    lpa_asn1_util_populate_tlv(device_capabilities_encoded_data_buff_ptr,
                               device_capabilities_encoded_data_buff_total_len,
                               &octet1_context_specific_tag,
                               &common_octet1_len,
                               &common_capability);
  }

  if(device_capability.contactless_supported_release_present == TRUE)
  {
    octet1_context_specific_tag.tag[0]        = LPA_ASN1_GEN_TAG_CONTEXT_SPECIFIC6;
    common_capability.data_ptr                = &device_capability.contactless_supported_release;
    lpa_asn1_util_populate_tlv(device_capabilities_encoded_data_buff_ptr,
                               device_capabilities_encoded_data_buff_total_len,
                               &octet1_context_specific_tag,
                               &common_octet1_len,
                               &common_capability);
  }

  return LPA_SUCCESS;
}/*lpa_asn1_encode_device_capabilities*/


/*===========================================================================
FUNCTION LPA_ASN1_ENCODE_DEVICE_INFO

DESCRIPTION
  Encode device info

DEPENDENCIES
  caller is responsible to free the the data pointer inside of parsed_smdpOID

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static lpa_result_enum_type lpa_asn1_encode_device_info(
  lpa_util_device_info_type        device_info,
  lpa_data_type*                   device_info_encoded_data_buff_ptr
)
{
  /* intermediate encoding results */
  lpa_data_type            device_capability_encoded_data_buff;
  /* tag fields */
  lpa_asn1_tag_field_type  tac_tag;
  lpa_asn1_tag_field_type  device_info_tag;
  /* length fields */
  lpa_asn1_len_field_type  tac_len;
  lpa_asn1_len_field_type  device_info_len;
  /* value fields, all none lpa_data_type input parameters are held in temp lpa_data_type */
  lpa_data_type            tac;
  /* temporary total lenth, for calculating total malloc size */
  uint32                   device_info_encoded_data_buff_total_len = 0;
  /* encoding status */
  lpa_result_enum_type     status                                  = LPA_SUCCESS;

  /* init intermediate buff */
  memset(&device_capability_encoded_data_buff, 0, sizeof(lpa_data_type));
  /* initialize tag fields */
  lpa_asn1_init_tags_device_info(&tac_tag, &device_info_tag);
  /* initialize len fields */
  memset(&tac_len,0, sizeof(lpa_asn1_len_field_type));
  memset(&device_info_len,0, sizeof(lpa_asn1_len_field_type));
  /* initialize value fields */
  tac.data_len = device_info.tac.tac_len;
  tac.data_ptr = device_info.tac.tac;

  /* Encode device capability */
  status = lpa_asn1_encode_device_capabilities(device_info.device_capability,
                                               &device_capability_encoded_data_buff);
  if(LPA_SUCCESS != status)
  {
    /* caller of lpa_asn1_encode_device_capabilities is responsible for FREE.
       This might be a defensive FREE */
    LPA_FREE(device_capability_encoded_data_buff.data_ptr);
    return status;
  }

  /* Get total length and malloc device_info_encoded_data_buff_ptr */
  if(LPA_SUCCESS != lpa_asn1_util_get_len_field(tac.data_len, &tac_len))
  {
    LPA_FREE(device_capability_encoded_data_buff.data_ptr);
    return LPA_GENERIC_ERROR;
  }
  /* Add total payload length -- tac TLV and the device capability ASN1 data*/
  device_info_encoded_data_buff_total_len     = tac_tag.tag_len + tac_len.len_len + tac.data_len +
                                                device_capability_encoded_data_buff.data_len;
  /* Add the device info header len */
  if (LPA_SUCCESS != lpa_asn1_util_get_len_field(device_info_encoded_data_buff_total_len, &device_info_len))
  {
    LPA_FREE(device_capability_encoded_data_buff.data_ptr);
    return LPA_GENERIC_ERROR;
  }
  /* Use 0xA1 instead of common sequence tag per ObjSys */
  device_info_encoded_data_buff_total_len    += device_info_tag.tag_len + device_info_len.len_len;


  device_info_encoded_data_buff_ptr->data_ptr = (uint8*)LPA_MALLOC(device_info_encoded_data_buff_total_len);
  if (NULL == device_info_encoded_data_buff_ptr->data_ptr)
  {
    LPA_MSG_ERR_1("device_info_encoded_data_buff_total_len->data_ptr malloc len=0x%x", 
                  device_info_encoded_data_buff_total_len);

    LPA_FREE(device_capability_encoded_data_buff.data_ptr);
    return LPA_ERROR_HEAP_EXHAUSTED;
  }

  device_info_encoded_data_buff_ptr->data_len = 0;
  /* Header LPA_ASN1_GEN_TAG_SEQUENCE */
  lpa_asn1_util_populate_tlv(device_info_encoded_data_buff_ptr,
                             device_info_encoded_data_buff_total_len,
                             &device_info_tag,
                             &device_info_len,
                             NULL);

  /* tac LPA_ASN1_GEN_TAG_OCTET */
  lpa_asn1_util_populate_tlv(device_info_encoded_data_buff_ptr,
                             device_info_encoded_data_buff_total_len,
                             &tac_tag,
                             &tac_len,
                             &tac);

  /* Concatenate device_capabilities LPA_ASN1_GEN_TAG_SEQUENCE*/
  lpa_asn1_util_populate_tlv(device_info_encoded_data_buff_ptr,
                             device_info_encoded_data_buff_total_len,
                             NULL,
                             NULL,
                             &device_capability_encoded_data_buff);

  /* Memory cleanup */
  LPA_FREE(device_capability_encoded_data_buff.data_ptr);

  return LPA_SUCCESS;
} /* lpa_asn1_encode_device_info */ 


/*===========================================================================
FUNCTION LPA_PARSE_OID

DESCRIPTION
  Parse the OID string from activation code e.g.XX.XX.XXXXX to int array

DEPENDENCIES
  caller is responsible to free the the data pointer inside of parsed_oid

RETURN VALUE
  NONE

SIDE EFFECTS
  None
===========================================================================*/
static lpa_result_enum_type lpa_asn1_parse_oid(
  lpa_smdp_id_type     oid_string,
  lpa_asn1_oid_type*   parsed_oid_ptr
)
{
  uint32 i = 0;
  uint32 j = 0;
  uint32 number_of_int_in_oid = 1;   /*at least 1 int in OID*/

  if (parsed_oid_ptr == NULL) 
  {
    LPA_MSG_ERR_0("parsed_oid_ptr is NULL, check caller function");
    return LPA_ERROR_INCORRECT_PARAMS;
  }

  /* get the total number of OID int */
  for(i = 0; i < oid_string.data_len; i++)
  {
    if(oid_string.data[i] == '.')
    {
      number_of_int_in_oid++;
    }
  }

  /* Note: number_of_int_in_oid > LPA_ASN1_MAX_SMDPOID_INT_LEN */
  /* Set OID array len */
  parsed_oid_ptr->oid_len = number_of_int_in_oid;

  /* Populate OID int array */
  j = 0;
  for( i = 0; i < oid_string.data_len; i++ )
  {
    if(oid_string.data[i] != '.')
    {
      if ( j >=  LPA_ASN1_MAX_SMDPOID_INT_LEN) 
      {
        /* this should never happen */
        LPA_MSG_ERR_0("Too many OID digits, array out of range");
        return LPA_GENERIC_ERROR;
      }
      parsed_oid_ptr->oid[j] = parsed_oid_ptr->oid[j]*10 + (oid_string.data[i] - '0');
    }
    else
    {
      j++;
    }
  }

  return LPA_SUCCESS;
}/*lpa_asn1_parse_OID*/


/*===========================================================================
FUNCTION LPA_ASN1_ENCODE_OID

DESCRIPTION
  Encode the OID int array to ASN1 byte array and malloc and populate the
  encoded_OID data_ptr.
  OBJECT IDENTIFIER encoding rules:
  1) The first two nodes of the OID are encoded onto a single byte.
     The first node is multiplied by the decimal 40 and the result is added to the value of the second node.
  2) Node values less than or equal to 127 are encoded on one byte.
  3) Node values greater than or equal to 128 are encoded on multiple bytes.
     Bit 7 of the leftmost byte is set to one. Bits 0 through 6 of each byte contains the encoded value.

DEPENDENCIES
  caller is responsible to free the the data pointer inside of parsed_smdpOID

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static lpa_result_enum_type lpa_asn1_encode_oid(
  lpa_data_type*    encoded_oid_ptr,
  lpa_asn1_oid_type parsed_oid
)
{
  uint32 i                           = 0;
  uint32 j                           = 0;
  uint32 k                           = 0;
  uint32 numb_of_byte_for_single_int = 0;
  uint32 total_encoded_data_len      = 1;   /* At least 1 byte */
  uint32 temp_int_leftshift          = 0;
  uint32 temp_int_rightshift         = 0;

  /* Get length */
  if(parsed_oid.oid_len > 2)
  {
    /* Loop through and try to get the total length */
    for(i = 2; i < parsed_oid.oid_len; i++)
    {
      /* Node values less than or equal to 127 are encoded on one byte */
      if(parsed_oid.oid[i] <= 127)
      {
        total_encoded_data_len++;
      }
      else
      {
        /* Multiple byte coding */
        temp_int_leftshift = parsed_oid.oid[i];
        for(j = sizeof(uint32)*8; j > 0; j--)
        {
          /* Finding the highest byte */
          if((temp_int_leftshift & 0x80000000) > 0)
          {
            /* Found the highest non-zero bit and add the needed bytes to the total len*/
            total_encoded_data_len += j/7;
            if(0 != j%7)
            {
              total_encoded_data_len++;
            }
            break;
          }
          temp_int_leftshift = temp_int_leftshift << 1;
        }/*end of finding highest non-zero bit loop*/
      }
    }/* end of find toto_encoded_data_len loop */
  }

  encoded_oid_ptr->data_len = total_encoded_data_len;
  encoded_oid_ptr->data_ptr = (uint8*)LPA_MALLOC(encoded_oid_ptr->data_len);
  if (NULL ==encoded_oid_ptr->data_ptr)
  {
    LPA_MSG_ERR_1("encoded_oid_ptr->data_ptr malloc len= 0x%x", 
                  encoded_oid_ptr->data_len);
    return LPA_ERROR_HEAP_EXHAUSTED;
  }

  if(1 == parsed_oid.oid_len)
  {
    encoded_oid_ptr->data_ptr[0] = (uint8)parsed_oid.oid[0];
  }

  else if(parsed_oid.oid_len >1)
  {
    encoded_oid_ptr->data_ptr[0] = (uint8)(parsed_oid.oid[0]*40 + parsed_oid.oid[1]);
    j = 1; /* Encode the 2nd and up OID int */

    for(i = 2; i < parsed_oid.oid_len; i++)
    {
      if(parsed_oid.oid[i] <= 127)
      {
        encoded_oid_ptr->data_ptr[j]  = (uint8)parsed_oid.oid[i];
        j++;
      }
      else
      {
        temp_int_leftshift = parsed_oid.oid[i];
        /* looping through each bit 8 bit in a byte * 4 byte in uint32(for oid) */
        for(k = sizeof(uint32)*8; k >0 ; k--)
        {
          if((temp_int_leftshift&0x80000000) > 0)
          {
            /* Found the highest bit*/
            numb_of_byte_for_single_int = k/7;
            if((k%7) > 0)
            {
              numb_of_byte_for_single_int++;
            }
            break;
          }
          else
          {
            temp_int_leftshift = temp_int_leftshift << 1;
          }
        }
        /* populate */
        temp_int_rightshift = parsed_oid.oid[i];
        for(k = numb_of_byte_for_single_int; k > 0 ; k--)
        {
          encoded_oid_ptr->data_ptr[j + k -1] = (temp_int_rightshift & 0x0000007F)|0x80;
          temp_int_rightshift = temp_int_rightshift >> 7;
        }
        break;
      }
    }/* end of encoding all OID int loop */
  }

  return LPA_SUCCESS;
}/*lpa_asn1_encode_oid*/


/*===========================================================================
FUNCTION LPA_ASN1_ENCODE_PREPARE_DOWNLOAD_REQUEST

DESCRIPTION
  this function encodes PrepareDownloadRequest

DEPENDENCIES
  caller is responsible to free the the data pointer inside of
  prepare_download_encoded_data_buff_ptr

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_asn1_encode_prepare_download_request(
  lpa_asn1_prepare_download_request_type    prepare_download_request,
  lpa_data_type*                            prepare_download_encoded_data_buff_ptr
)
{
  /* intermediate encoding results */
  lpa_asn1_oid_type        parsed_smdpoid;
  lpa_data_type            device_info_encoded_data_buff;
  lpa_data_type            smdpoid_info_encoded_data_buff;
  /* tag fields */
  lpa_asn1_tag_field_type  dp_signed1_raw_tag;
  lpa_asn1_tag_field_type  smdp_signature1_raw_tag;
  lpa_asn1_tag_field_type  activation_code_token_raw_tag;
  lpa_asn1_tag_field_type  smdpoid_raw_tag;
  lpa_asn1_tag_field_type  hashed_confirmation_code_raw_tag;
  lpa_asn1_tag_field_type  cert_format_to_be_used_raw_tag;
  lpa_asn1_tag_field_type  curve_to_be_used_raw_tag;
  lpa_asn1_tag_field_type  smdp_certificate_raw_tag;
  lpa_asn1_tag_field_type  prepare_download_request_tag;	
  /* lenth fields */
  lpa_asn1_len_field_type  prepare_download_request_len;
  lpa_asn1_len_field_type  smdp_signature1_len;
  lpa_asn1_len_field_type  activation_code_token_len;
  lpa_asn1_len_field_type  smdpoid_len;
  lpa_asn1_len_field_type  hashed_confirmation_code_len;
  lpa_asn1_len_field_type  cert_format_to_be_used_len;
  lpa_asn1_len_field_type  curve_to_be_used_len;
  /* value fields, all none lpa_data_type input parameters are held in temp lpa_data_type*/
  lpa_data_type            smdpoid;
  /* temporary total lenth */
  uint32                   prepare_download_encoded_data_buff_total_len = 0;
  /* encoding status */
  lpa_result_enum_type     status                                       = LPA_GENERIC_ERROR;


  /* Pre-encoding preparation */
  /* Input data validation -- check for maditory data field */
  if( NULL == prepare_download_request.dp_signed1_raw.data_ptr ||
      NULL == prepare_download_request.smdp_signature1_raw.data_ptr ||
      NULL == prepare_download_request.activation_code_token_raw.data_ptr ||
      NULL == prepare_download_request.cert_format_to_be_used_raw.data_ptr ||
      NULL == prepare_download_request.curve_to_be_used_raw.data_ptr ||
      NULL == prepare_download_request.smdp_certificate_raw.data_ptr)
  {
    return LPA_GENERIC_ERROR;
  }

  memset(&parsed_smdpoid, 0, sizeof(lpa_asn1_oid_type));
  memset(&device_info_encoded_data_buff, 0, sizeof(lpa_data_type));
  memset(&smdpoid_info_encoded_data_buff, 0, sizeof(lpa_data_type));
  memset(&prepare_download_request_len, 0, sizeof(lpa_asn1_len_field_type));
  memset(&smdp_signature1_len, 0, sizeof(lpa_asn1_len_field_type));
  memset(&activation_code_token_len, 0, sizeof(lpa_asn1_len_field_type));
  memset(&smdpoid_len, 0, sizeof(lpa_asn1_len_field_type));
  memset(&hashed_confirmation_code_len, 0, sizeof(lpa_asn1_len_field_type));
  memset(&cert_format_to_be_used_len, 0, sizeof(lpa_asn1_len_field_type));
  memset(&curve_to_be_used_len, 0, sizeof(lpa_asn1_len_field_type));

  /* init tag fields */
  lpa_asn1_init_tags_prepare_download_request(&activation_code_token_raw_tag,
                                              &smdpoid_raw_tag,
                                              &hashed_confirmation_code_raw_tag,
                                              &prepare_download_request_tag);
  /* init value fields */
  smdpoid.data_len = prepare_download_request.smdpoid_string.data_len;
  smdpoid.data_ptr = (uint8*)prepare_download_request.smdpoid_string.data;

  /* Get total malloc length: add passing length/fixed length */
  prepare_download_encoded_data_buff_total_len = 0;

  /* dp_signed1_raw -- Already in TLV format from SMDP*/
  prepare_download_encoded_data_buff_total_len += prepare_download_request.dp_signed1_raw.data_len;

  /*smdp_signature1_raw -- Already in TLV format from SMDP*/
  prepare_download_encoded_data_buff_total_len += prepare_download_request.smdp_signature1_raw.data_len;

  /*activation_code_token_raw TAG_UTF8STRING*/
  if(LPA_SUCCESS != lpa_asn1_util_get_len_field(prepare_download_request.activation_code_token_raw.data_len,
                                                &activation_code_token_len))
  {
    return LPA_GENERIC_ERROR;
  }
  prepare_download_encoded_data_buff_total_len += (activation_code_token_raw_tag.tag_len +
                                                   activation_code_token_len.len_len +
                                                   prepare_download_request.activation_code_token_raw.data_len);
  /*device_info LPA_ASN1_GEN_TAG_SEQUENCE*/
  status = lpa_asn1_encode_device_info(prepare_download_request.device_info,
                                       &device_info_encoded_data_buff);
  if(LPA_SUCCESS != status)
  {
    LPA_FREE(device_info_encoded_data_buff.data_ptr);
    return status;
  }
  prepare_download_encoded_data_buff_total_len += device_info_encoded_data_buff.data_len;

  /*smdpOID_raw LPA_ASN1_GEN_TAG_OBJECT_IDENTIFIER OPTIONAL*/
  if(prepare_download_request.smdpoid_string.data_len > 0)
  {
    if(LPA_SUCCESS != lpa_asn1_parse_oid(prepare_download_request.smdpoid_string, &parsed_smdpoid))
    {
      LPA_FREE(device_info_encoded_data_buff.data_ptr);
      return LPA_GENERIC_ERROR;
    }
    if( LPA_SUCCESS != lpa_asn1_encode_oid(&smdpoid_info_encoded_data_buff, parsed_smdpoid) )
    {
      /* currently the return of the failure is due to heap exhausted, hence this free is just a defensive code */
      LPA_FREE(device_info_encoded_data_buff.data_ptr);
      return LPA_GENERIC_ERROR;
    }
    if (LPA_SUCCESS != lpa_asn1_util_get_len_field(smdpoid_info_encoded_data_buff.data_len,
                                                   &smdpoid_len))
    {
      LPA_FREE(device_info_encoded_data_buff.data_ptr);
      return LPA_GENERIC_ERROR;
    }
    prepare_download_encoded_data_buff_total_len += (smdpoid_raw_tag.tag_len +
                                                     smdpoid_len.len_len +
                                                     smdpoid_info_encoded_data_buff.data_len);
  }

  /*hashed_confirmation_code_raw LPA_ASN1_GEN_TAG_OCTET OPTIONAL*/
  if(prepare_download_request.hashed_confirmation_code_raw.data_len > 0)
  {
    if(LPA_SUCCESS != lpa_asn1_util_get_len_field(
                               prepare_download_request.hashed_confirmation_code_raw.data_len,
                               &hashed_confirmation_code_len))
    {
      LPA_FREE(device_info_encoded_data_buff.data_ptr);
      return LPA_GENERIC_ERROR;
    }
    prepare_download_encoded_data_buff_total_len +=
                                  (hashed_confirmation_code_raw_tag.tag_len +
                                  hashed_confirmation_code_len.len_len +
                                  prepare_download_request.hashed_confirmation_code_raw.data_len);
  }

  /*cert_format_to_be_used_raw -- Already in TLV format from SMDP*/
  prepare_download_encoded_data_buff_total_len += prepare_download_request.cert_format_to_be_used_raw.data_len;

  /*curve_to_be_used_raw -- Already in TLV format from SMDP*/
  prepare_download_encoded_data_buff_total_len += prepare_download_request.curve_to_be_used_raw.data_len;

  /*smdp_certificate_raw -- Already in TLV format from SMDP*/
  prepare_download_encoded_data_buff_total_len += prepare_download_request.smdp_certificate_raw.data_len;

  /*prepare_download_request LPA_ASN1_GEN_TAG_SEQUENCE*/
  if(LPA_SUCCESS != lpa_asn1_util_get_len_field(prepare_download_encoded_data_buff_total_len,
                                                &prepare_download_request_len))
  {
    LPA_FREE(device_info_encoded_data_buff.data_ptr);
    return LPA_GENERIC_ERROR;
  }
  prepare_download_encoded_data_buff_total_len += (prepare_download_request_tag.tag_len +
                                                   prepare_download_request_len.len_len);

  LPA_MSG_ERR_1("prepare_download_encoded_data_buff_total_len=0x%x", prepare_download_encoded_data_buff_total_len);
  prepare_download_encoded_data_buff_ptr->data_ptr = (uint8 *)LPA_MALLOC(prepare_download_encoded_data_buff_total_len);
  if (NULL == prepare_download_encoded_data_buff_ptr->data_ptr)
  {
    LPA_MSG_ERR_1("prepare_download_encoded_data_buff_ptr->data_ptr malloc len= 0x%x",
                  prepare_download_encoded_data_buff_total_len);
    LPA_FREE(device_info_encoded_data_buff.data_ptr);
    return LPA_ERROR_HEAP_EXHAUSTED;
  }

  /*populate prepare_download_encoded_data_buff_ptr*/
  prepare_download_encoded_data_buff_ptr->data_len = 0;
  /*T and L for prepare_download_encoded_data_buff_ptr LPA_ASN1_GEN_TAG_PREPARE_DOWNLOAD_REQUEST*/
  lpa_asn1_util_populate_tlv(prepare_download_encoded_data_buff_ptr,
                             prepare_download_encoded_data_buff_total_len,
                             &prepare_download_request_tag,
                             &prepare_download_request_len,
                             NULL);

  /*dp_signed1_raw LPA_ASN1_GEN_TAG_SEQUENCE*/
  lpa_asn1_util_populate_tlv(prepare_download_encoded_data_buff_ptr,
                             prepare_download_encoded_data_buff_total_len,
                             NULL, NULL,
                             &prepare_download_request.dp_signed1_raw);

  /*smdp_signature1_raw*/
  lpa_asn1_util_populate_tlv(prepare_download_encoded_data_buff_ptr,
                             prepare_download_encoded_data_buff_total_len,
                             NULL, NULL,
                             &prepare_download_request.smdp_signature1_raw);

  /*activation_code_token_raw LPA_ASN1_GEN_TAG_SMDPSIGNATURE1*/
  lpa_asn1_util_populate_tlv(prepare_download_encoded_data_buff_ptr,
                             prepare_download_encoded_data_buff_total_len,
                             &activation_code_token_raw_tag,
                             &activation_code_token_len,
                             &prepare_download_request.activation_code_token_raw);

  /*device_info LPA_ASN1_GEN_TAG_SEQUENCE*/
  lpa_asn1_util_populate_tlv(prepare_download_encoded_data_buff_ptr,
                             prepare_download_encoded_data_buff_total_len,
                             NULL, NULL,
                             &device_info_encoded_data_buff);

  if(prepare_download_request.smdpoid_string.data_len > 0)
  {
     /*smdpOID_raw LPA_ASN1_GEN_TAG_OBJECT_IDENTIFIER*/
     lpa_asn1_util_populate_tlv(prepare_download_encoded_data_buff_ptr,
                                prepare_download_encoded_data_buff_total_len,
                                &smdpoid_raw_tag, &smdpoid_len,
                                &smdpoid);
  }

  if(prepare_download_request.hashed_confirmation_code_raw.data_len > 0)
  {
     /*hashed_confirmation_code_raw LPA_ASN1_GEN_TAG_OCTET*/
     lpa_asn1_util_populate_tlv(prepare_download_encoded_data_buff_ptr,
                                prepare_download_encoded_data_buff_total_len,
                                &hashed_confirmation_code_raw_tag,
                                &hashed_confirmation_code_len,
                                &prepare_download_request.hashed_confirmation_code_raw);
  }

  /*cert_format_to_be_used_raw*/
  lpa_asn1_util_populate_tlv(prepare_download_encoded_data_buff_ptr,
                             prepare_download_encoded_data_buff_total_len,
                             NULL, NULL,
                             &prepare_download_request.cert_format_to_be_used_raw);

  /*curve_to_be_used_raw*/
  lpa_asn1_util_populate_tlv(prepare_download_encoded_data_buff_ptr,
                             prepare_download_encoded_data_buff_total_len,
                             NULL, NULL,
                             &prepare_download_request.curve_to_be_used_raw);

  /*smdp_certificate_raw*/
  lpa_asn1_util_populate_tlv(prepare_download_encoded_data_buff_ptr,
                             prepare_download_encoded_data_buff_total_len,
                             NULL, NULL,
                             &prepare_download_request.smdp_certificate_raw);

  /*memory cleanup*/
  if(prepare_download_request.smdpoid_string.data_len > 0)
  {
    LPA_FREE(smdpoid_info_encoded_data_buff.data_ptr);
  }
  LPA_FREE(device_info_encoded_data_buff.data_ptr);

  return LPA_SUCCESS;
}/*lpa_asn1_encode_prepare_download_request*/


/*===========================================================================
FUNCTION LPA_ASN1_ENCODE_SET_NICKNAME_REQUEST

DESCRIPTION
  This function encodes the SetNicknameRequest

DEPENDENCIES
  caller is responsible to free the the data pointer inside of
  set_nickname_request_encoded_data_buff_ptr

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_asn1_encode_set_nickname_request(
  lpa_asn1_set_nickname_request_type set_nickname_request,
  lpa_data_type*                     set_nickname_request_encoded_data_buff_ptr
)
{
   uint32                  set_nickname_request_data_buff_total_len   = 0;
   uint32                  set_nickname_request_data_buff_payload_len = 0;
   /* tag fields */
   lpa_asn1_tag_field_type iccid_tag;
   lpa_asn1_tag_field_type profile_nickname_tag;
   lpa_asn1_tag_field_type set_nick_name_request_tag;
   /* lenth fields */
   lpa_asn1_len_field_type set_nickname_request_len;
   lpa_asn1_len_field_type iccid_len;
   lpa_asn1_len_field_type profile_nickname_len;
   /* local asn1 format data type */
   lpa_data_type           asn1_iccid;
   lpa_data_type           asn1_profile_nickname;

  /* Pre-encoding preparation */
  /* Input data validation -- check for maditory data field */
   memset(&set_nickname_request_len, 0, sizeof(lpa_asn1_len_field_type));
   memset(&iccid_len, 0, sizeof(lpa_asn1_len_field_type));
   memset(&profile_nickname_len, 0, sizeof(lpa_asn1_len_field_type));
   memset(&asn1_iccid, 0, sizeof(lpa_data_type));
   memset(&asn1_profile_nickname, 0, sizeof(lpa_data_type));
   /* init tag field */
   lpa_asn1_init_tags_set_nickname_request(&iccid_tag,
                                           &profile_nickname_tag,
                                           &set_nick_name_request_tag);
   /* populate input data in ASN1 object format */
   asn1_iccid.data_len            = set_nickname_request.iccid.data_len;
   asn1_iccid.data_ptr            = set_nickname_request.iccid.data;
   asn1_profile_nickname.data_len = set_nickname_request.profile_nickname.name_len;
   asn1_profile_nickname.data_ptr = set_nickname_request.profile_nickname.name;

   /* Get total malloc length: add passing length/fixed length */
   if(LPA_SUCCESS != lpa_asn1_util_get_len_field((uint32)set_nickname_request.iccid.data_len, &iccid_len))
   {
     return LPA_GENERIC_ERROR;
   }
   set_nickname_request_data_buff_payload_len += iccid_tag.tag_len +
                                                 iccid_len.len_len +
                                                 set_nickname_request.iccid.data_len;

   if(LPA_SUCCESS != lpa_asn1_util_get_len_field((uint32)set_nickname_request.profile_nickname.name_len,
                                                 &profile_nickname_len))
   {
     return LPA_GENERIC_ERROR;
   }
   set_nickname_request_data_buff_payload_len += profile_nickname_tag.tag_len +
                                                 profile_nickname_len.len_len +
                                                 set_nickname_request.profile_nickname.name_len;

   if(LPA_SUCCESS != lpa_asn1_util_get_len_field(set_nickname_request_data_buff_payload_len,
                                                 &set_nickname_request_len))
   {
     return LPA_GENERIC_ERROR;
   }


   /* malloc for set_nickname_request_encoded_data_buff_ptr */
   set_nickname_request_data_buff_total_len = set_nick_name_request_tag.tag_len +
                                              set_nickname_request_len.len_len +
                                              set_nickname_request_data_buff_payload_len;

   set_nickname_request_encoded_data_buff_ptr->data_ptr = (uint8 *)LPA_MALLOC(set_nickname_request_data_buff_total_len);
   if (NULL == set_nickname_request_encoded_data_buff_ptr->data_ptr)
   {
     return LPA_ERROR_HEAP_EXHAUSTED;
   }

   /* populate set_nickname_request_encoded_data_buff_ptr */
   set_nickname_request_encoded_data_buff_ptr->data_len = 0;
   /* header LPA_ASN1_GEN_TAG_PREPARE_DOWNLOAD_REQUEST*/
   lpa_asn1_util_populate_tlv(set_nickname_request_encoded_data_buff_ptr,
                              set_nickname_request_data_buff_total_len,
                              &set_nick_name_request_tag,
                              &set_nickname_request_len,
                              NULL);

   /* iccid LPA_ASN1_GEN_TAG_APPLICATION26*/
   lpa_asn1_util_populate_tlv(set_nickname_request_encoded_data_buff_ptr,
                              set_nickname_request_data_buff_total_len,
                              &iccid_tag,
                              &iccid_len,
                              &asn1_iccid);
   /*profileNickname LPA_ASN1_GEN_TAG_UTF8STRING*/
   lpa_asn1_util_populate_tlv(set_nickname_request_encoded_data_buff_ptr,
                              set_nickname_request_data_buff_total_len,
                              &profile_nickname_tag,
                              &profile_nickname_len,
                              &asn1_profile_nickname);

   return LPA_SUCCESS;
}/*lpa_asn1_encode_set_nickname_request*/

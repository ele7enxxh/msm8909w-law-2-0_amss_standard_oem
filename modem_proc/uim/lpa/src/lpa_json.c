/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


       U I M   JSON encoder and decoder IMPLEMENTATION

GENERAL DESCRIPTION
  This module contains the procedures for the BTSAP in client mode implementation.
  The module receives the event from the QMI REMOTE CLIENT. The module handles the
  power up and power down sequence of the remote sim card.

Copyright (c) 2016 by QUALCOMM Technologies, Inc. All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/lpa/src/lpa_json.c#1 $ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/21/16    av     Add SVN check for supporting phase1, phase1.1 and phase2
04/11/16    ll     Initial version
===========================================================================*/
#include "lpa_json.h"
#include "lpa_platform.h"
#include "lpa_lpd.h"


#define LPA_JSON_SVN_FORMATED_STR_LEN                            6
#define LPA_JSON_EUICC_CHANLLENGE_SIZE                           16
#define LPA_JSON_STRING_MAX_LEN                                  100000

#define LPA_JSON_NUM_BITS_BASE64_CHAR                            6
#define LPA_JSON_NUM_BITS_CHAR                                   8

#define LPA_JSON_ASCII_HEXADICIMALBASE                           48
#define LPA_JSON_ASCII_DOT                                       46
#define LPA_JSON_ASCII_NULL                                      0

#define JSON_HTTP_RSP_HEADER_STATUS_EXECUTED_SUCCESS             "Executed-Success"
#define JSON_HTTP_RSP_HEADER_STATUS_PATH                         "header.functionExecutionStatus.status"

#define JSON_INIT_AUTH_CMD_EUICCCHANGE_PATH                      "euiccChallenge"
#define JSON_INIT_AUTH_CMD_EUICCCHANGE_PATH_PH2                  "eUiccChallenge"
#define JSON_INIT_AUTH_CMD_SVN_PATH_PATH                         "svn"
#define JSON_INIT_AUTH_CMD_CERTINFO_PATH                         "certificateInfo"
#define JSON_INIT_AUTH_CMD_EUICCINFO_PATH                        "euiccInfo1"
#define JSON_INIT_AUTH_CMD_EUICCINFO_PATH_PH2                    "eUICCInfo1"
#define JSON_INIT_AUTH_CMD_SMDPADDRESS_PATH                      "smdpAddress"

#define JSON_INIT_AUTH_RSP_TRANSACTIONID_PATH                    "transactionId"
#define JSON_INIT_AUTH_RSP_DPSIGNED1_PATH                        "dpSigned1"
#define JSON_INIT_AUTH_RSP_SMDPSIGNATURE1_PATH                   "smdpSignature1"
#define JSON_INIT_AUTH_RSP_CERTFORMATTOBEUSED_PATH               "certFormatTobeUsed"
#define JSON_INIT_AUTH_RSP_CURVETOBEUSED_PATH                    "curveTobeUsed"
#define JSON_INIT_AUTH_RSP_SMDPCERTIFICATE_PATH                  "smdpCertificate"

#define JSON_GET_BPP_CMD_TRANSACTIONID_PATH                      "transactionId"
#define JSON_GET_BPP_CMD_EUICCPREPAREDOWNLOADRESPONSE_PATH_PH1   "euiccPrepareDownloadResponse"
#define JSON_GET_BPP_CMD_EUICCPREPAREDOWNLOADRESPONSE_PATH_PH1_1 "prepareDownloadResponse"
#define JSON_GET_BPP_CMD_EUICCPREPAREDOWNLOADRESPONSE_PATH_PH2   "PrepareDownloadResponse"

#define JSON_GET_BPP_RSP_BOUNDPROFILEPACKAGE_PATH                "boundProfilePackage"

#define JSON_HANDLE_INSTALL_RESULT_RESULT_PATH                   "result"

/* ==========================================================================
FUNCTION LPA_JSON_UTIL_FORMAT_SVN_TO_STRING

DESCRIPTION:
  Add '.' seperater for SVN

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
==========================================================================*/
static void lpa_json_util_format_svn_to_string (
  const uint8*       svn_number_array_ptr,
  char*              svn_formatted_string_ptr
)
{
  if (NULL != svn_number_array_ptr && NULL != svn_formatted_string_ptr)
  {
    svn_formatted_string_ptr[0] = (char)(svn_number_array_ptr[0] + LPA_JSON_ASCII_HEXADICIMALBASE);
    svn_formatted_string_ptr[1] = (char)LPA_JSON_ASCII_DOT;
    svn_formatted_string_ptr[2] = (char)(svn_number_array_ptr[1] + LPA_JSON_ASCII_HEXADICIMALBASE);
    svn_formatted_string_ptr[3] = (char)LPA_JSON_ASCII_DOT;
    svn_formatted_string_ptr[4] = (char)(svn_number_array_ptr[2] + LPA_JSON_ASCII_HEXADICIMALBASE);
    svn_formatted_string_ptr[5] = (char)LPA_JSON_ASCII_NULL;
  }
}/* lpa_json_util_format_svn_to_string */


/* ==========================================================================
FUNCTION LPA_JSON_UTIL_ALLOC_ASCIISTRING_TO_BIN

DESCRIPTION:
  Allocates memory and converts a ASCII string to a binary
  datastream.

DEPENDENCIES:
  Caller needs to free the return pointer

RETURN VALUE:
  Decoded binary array

SIDE EFFECTS:
  None
==========================================================================*/
static uint8* lpa_json_util_alloc_ascii_string_to_bin (
  const char                *input_ptr,
  uint32                    *output_len_ptr
)
{
  uint32 i = 0;
  uint8* decode_buffer;

  if (NULL == input_ptr || NULL == output_len_ptr)
  {
    return NULL;
  }

  *output_len_ptr = strlen(input_ptr);
  decode_buffer   = LPA_MALLOC(sizeof(uint8) * (*output_len_ptr));
  if (NULL == decode_buffer)
  {
    /* malloc failed */
    return NULL;
  }

  while( i < strlen(input_ptr) &&
         input_ptr[i] >= LPA_JSON_ASCII_HEXADICIMALBASE)
  {
    decode_buffer[i] = (uint8)((uint8)input_ptr[i] - LPA_JSON_ASCII_HEXADICIMALBASE);
    i++;
  }

  return decode_buffer;
}/*lpa_json_util_alloc_ASCIIstring_to_bin*/


/* ==========================================================================
FUNCTION LPA_JSON_UTIL_ALLOC_BIN_TO_ASCIISTRING

DESCRIPTION:
  Allocates memory and converts a binary datastream to a Hexadecimal string format.
  The output string is null terminated

DEPENDENCIES:
  Caller needs to free the return pointer

RETURN VALUE:
  encoded ASCII

SIDE EFFECTS:
  None
==========================================================================*/
char* lpa_json_util_alloc_bin_to_ascii_string ( const uint8 *input_ptr, uint32 input_len )
{
   char*    encode_buffer;
   uint32   i = 0;

   if (NULL == input_ptr || 0 == input_len)
   {
     return NULL;
   }

   encode_buffer = LPA_MALLOC(sizeof(uint8) * (input_len + 1));

   if (NULL == encode_buffer)
   {
     return NULL;
   }

   while( i < input_len)
   {
     encode_buffer[i] = (char)(input_ptr[i] + LPA_JSON_ASCII_HEXADICIMALBASE);
     i++;
   }

   encode_buffer[i]    = '\0';

   return encode_buffer;
}/*lpa_json_util_alloc_bin_to_ascii_string*/


/* ==========================================================================
FUNCTION LPA_JSON_UTIL_FIND_BASE64_VALUES

DESCRIPTION:
  This is an utility function which convert a base64 string to bin

DEPENDENCIES:
  None

RETURN VALUE:
  base64 value

SIDE EFFECTS:
  None
==========================================================================*/
static uint8 lpa_json_util_find_base64_values(
  boolean  find_char,
  char     input_char,
  uint8    input_index
)
{
  const char base64_table[] =
             {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};
  const char * value_ptr = NULL;

  if (find_char)
  {
    value_ptr = strchr(base64_table, input_char);
    if (value_ptr)
    {
      return (uint8)(value_ptr - base64_table);
    }
  }
  else
  {
    if (input_index < (sizeof(base64_table)/sizeof(base64_table[0])))
    {
      return (uint8)base64_table[input_index];
    }
  }
  return 0;
} /* lpa_json_util_find_base64_values */


/* ==========================================================================
FUNCTION LPA_JSON_UTIL_ALLOC_BASE64STRING_TO_BIN

DESCRIPTION:
  Allocates memory and converts a Base64 encoded ASCII string to a binary
  datastream.

DEPENDENCIES:
  Caller needs to free the return pointer

RETURN VALUE:
  decoded binary array

SIDE EFFECTS:
  None
==========================================================================*/
static uint8* lpa_json_util_alloc_base64string_to_bin (
  const char                *input_ptr,
  uint32                    *output_len_ptr
)
{
  uint32   i          = 0;
  uint32   j          = 0;
  uint32  input_len   = 0;
  uint32  output_len  = 0;
  uint8  *output_ptr  = NULL;

  if (input_ptr == NULL || output_len_ptr == NULL)
  {
    return NULL;
  }

  /* Base64 string have to be multiples of 4 byte-blocks and
     possibly padded to a 4 bytes block */
  input_len = (uint32) strnlen(input_ptr, LPA_JSON_STRING_MAX_LEN + 1);

  if ((input_len == 0) || (input_len > LPA_JSON_STRING_MAX_LEN) ||
      (input_len % 4) != 0)
  {
    LPA_MSG_ERR_1("Invalid Base64 string length: %d", input_len);
    return NULL;
  }

  output_len = (input_len / 4) * 3;

  LPA_MSG_ERR_2("input_len=%d, output_len=%d", input_len, output_len);

  /* Update output buffer size if input was padded with '='s
     Only 2 '=' padded bytes are allowed per 4 byte-block */
  if (input_ptr[input_len-1] == '=')
  {
    output_len--;
    if (input_ptr[input_len-2] == '=')
    {
      output_len--;
    }
  }

  output_ptr = LPA_MALLOC(sizeof(uint8) * (output_len));
  if (output_ptr == NULL)
  {
    return NULL;
  }

  memset(output_ptr, 0x00, output_len);

  /* Decode the string & convert to binary */
  while ((i+3) < input_len)
  {
    uint8 binary_byte = 0;

    /* Decode each block of 4 Base64 bytes to 3 binary bytes */
    uint32 first  = lpa_json_util_find_base64_values(TRUE, input_ptr[i++], 0);
    uint32 second = lpa_json_util_find_base64_values(TRUE, input_ptr[i++], 0);
    uint32 third  = lpa_json_util_find_base64_values(TRUE, input_ptr[i++], 0);
    uint32 fourth = lpa_json_util_find_base64_values(TRUE, input_ptr[i++], 0);

    uint32 all_three = (first  << (3 * LPA_JSON_NUM_BITS_BASE64_CHAR)) +
                       (second << (2 * LPA_JSON_NUM_BITS_BASE64_CHAR)) +
                       (third  << (1 * LPA_JSON_NUM_BITS_BASE64_CHAR)) +
                        fourth;

    /* 3 binary bytes */
    if(j >= output_len)
    {
      break;
    }
    binary_byte = j < output_len ? ((all_three >> (2 * LPA_JSON_NUM_BITS_CHAR)) & 0xFF) : 0;
    output_ptr[j++] = binary_byte;
    if(j >= output_len)
    {
      break;
    }
    binary_byte = j < output_len ? ((all_three >> (1 * LPA_JSON_NUM_BITS_CHAR)) & 0xFF) : 0;
    output_ptr[j++] = binary_byte;
    if(j >= output_len)
    {
      break;
    }
    binary_byte = j < output_len ? (all_three & 0xFF) : 0;
    output_ptr[j++] = binary_byte;
  }

  *output_len_ptr = output_len;

  return output_ptr;
} /* lpa_json_util_alloc_base64string_to_bin */


/* ==========================================================================
FUNCTION LPA_JSON_UTIL_ALLOC_BIN_TO_BASE64STRING

DESCRIPTION:
  Allocates memory and converts a binary datastream to a Base64 encoded
  ASCII string format. the output string is null terminated

DEPENDENCIES:
  Caller needs to free the returned pointer

RETURN VALUE:
  encoded base64string

SIDE EFFECTS:
  None
==========================================================================*/
static char* lpa_json_util_alloc_bin_to_base64string (
  const uint8                       *input_ptr,
  uint32                             input_len
)
{
  uint32   i                = 0;
  uint32   j                = 0;
  uint32   extra_bytes      = 0;
  uint32   output_len       = 0;
  char  * output_ptr       = NULL;

  if ((input_ptr == NULL) || (input_len == 0))
  {
    LPA_MSG_ERR_1("Invalid input parameters: input_len 0x%x", input_len);
    return NULL;
  }

  /* Calculate the max buffer size needed for the encoded Base64 string,
     3 binary bytes make 4 Base64 bytes */
  output_len =  (uint32) (sizeof(char) + ((((input_len % 3 > 0) ? 1 : 0) +
                                         (input_len / 3 )) * 4));

  output_ptr = LPA_MALLOC(sizeof(char) * (output_len + 1));
  if (output_ptr == NULL)
  {
    return NULL;
  }

  memset(output_ptr, 0, output_len);

  /* Now encode the ASCII string to Base64 string */
  while (i < input_len)
  {
    /* Encode each block of 4 bytes from 3 ASCII bytes */
    uint32 first  = i < input_len ? input_ptr[i++] : 0;
    uint32 second = i < input_len ? input_ptr[i++] : 0;
    uint32 third  = i < input_len ? input_ptr[i++] : 0;

    uint32 all_three = (first  << (2 * LPA_JSON_NUM_BITS_CHAR)) +
                       (second << (1 * LPA_JSON_NUM_BITS_CHAR)) +
                        third;

    /* 4 Base64 bytes */
    if ((j+3) < output_len)
    {
      output_ptr[j++] = lpa_json_util_find_base64_values(
                          FALSE, 0, (all_three >> (3 * LPA_JSON_NUM_BITS_BASE64_CHAR)) & 0x3F);
      output_ptr[j++] = lpa_json_util_find_base64_values(
                          FALSE, 0, (all_three >> (2 * LPA_JSON_NUM_BITS_BASE64_CHAR)) & 0x3F);
      output_ptr[j++] = lpa_json_util_find_base64_values(
                          FALSE, 0, (all_three >> (1 * LPA_JSON_NUM_BITS_BASE64_CHAR)) & 0x3F);
      output_ptr[j++] = lpa_json_util_find_base64_values(
                          FALSE, 0, all_three & 0x3F);
    }
  }

  /* Update pading if required. It is needed if ASCII string's
     last group has either 1 or 2 bytes */
  extra_bytes = input_len % 3;
  if (extra_bytes)
  {
    uint32 bytes_to_fill = (extra_bytes == 1) ? 2 : 1;
    for (i = 1; i < 3 && bytes_to_fill; i++, --bytes_to_fill)
    {
      output_ptr[output_len - 1 - i] = '=';
    }
  }
  return output_ptr;
} /* lpa_json_util_alloc_bin_to_base64string */


/*===========================================================================
FUNCTION LPA_JSON_INITIATE_AUTH_DECODER

DESCRIPTION
  This function parse the ES9+.InitiateAuthentication response JSON string.
  Then populate the json_initiate_auth_ptr passed in from LPA caller

DEPENDENCIES
  Caller need to free all inter pointers in json_initiate_auth_ptr

RETURN VALUE
  Decoding status

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_json_initiate_auth_decoder(
  lpa_data_type                       * json_data_ptr,
  lpa_lpd_initiate_auth_resp_type     * json_initiate_auth_ptr
)
{
  ds_json_handler*     json_handler_ptr   = NULL;
  ds_json_value*       json_value_obj_ptr = NULL;
  ds_json_value_info_s json_value_info;

  if( NULL == json_initiate_auth_ptr ||
      NULL == json_data_ptr)
  {
    return LPA_GENERIC_ERROR;
  }

  memset(&json_value_info, 0, sizeof(ds_json_value_info_s));

  /* Create a JSON handler */
  json_handler_ptr = ds_json_create_handler();

  if(NULL == json_handler_ptr)
  {
    return LPA_GENERIC_ERROR;
  }

  if( DS_JSON_SUCCESS != ds_json_parse_from_content(
                              json_handler_ptr, (const char*)json_data_ptr->data_ptr,
                              json_data_ptr->data_len))
  {
    ds_json_destroy_handler(json_handler_ptr);
    return LPA_GENERIC_ERROR;
  }

  /* Get root node */
  json_value_obj_ptr = ds_json_get_root(json_handler_ptr);
  if(NULL == json_value_obj_ptr)
  {
    ds_json_destroy_handler(json_handler_ptr);
    return LPA_GENERIC_ERROR;
  }

  memset(&json_value_info, 0, sizeof(ds_json_value_info_s));
  ds_json_get_value_info_at_path(json_value_obj_ptr, JSON_HTTP_RSP_HEADER_STATUS_PATH, &json_value_info);
  if (strcmp(JSON_HTTP_RSP_HEADER_STATUS_EXECUTED_SUCCESS, json_value_info.value.string_value.json_str))
  {
     json_initiate_auth_ptr->http_response = LPA_SUCCESS;
  }
  else
  {
     json_initiate_auth_ptr->http_response = LPA_ERROR_SMDP_RESPONSE;
  }

  memset(&json_value_info, 0, sizeof(ds_json_value_info_s));
  if(DS_JSON_SUCCESS != ds_json_get_value_info_at_path(json_value_obj_ptr, JSON_INIT_AUTH_RSP_TRANSACTIONID_PATH, &json_value_info))
  {
    ds_json_destroy_handler(json_handler_ptr);
    return LPA_GENERIC_ERROR;
  }
  json_initiate_auth_ptr->transaction_id.data_ptr = lpa_json_util_alloc_ascii_string_to_bin(json_value_info.value.string_value.json_str,
                                                                                           &(json_initiate_auth_ptr->transaction_id.data_len));

  memset(&json_value_info, 0, sizeof(ds_json_value_info_s));
  if(DS_JSON_SUCCESS != ds_json_get_value_info_at_path(json_value_obj_ptr, JSON_INIT_AUTH_RSP_DPSIGNED1_PATH, &json_value_info))
  {
    ds_json_destroy_handler(json_handler_ptr);
    return LPA_GENERIC_ERROR;
  }
  json_initiate_auth_ptr->dp_signed1.data_ptr = lpa_json_util_alloc_base64string_to_bin(json_value_info.value.string_value.json_str,
                                                                                   &(json_initiate_auth_ptr->dp_signed1.data_len));

  memset(&json_value_info, 0, sizeof(ds_json_value_info_s));
  if(DS_JSON_SUCCESS != ds_json_get_value_info_at_path(json_value_obj_ptr, JSON_INIT_AUTH_RSP_SMDPSIGNATURE1_PATH, &json_value_info))
  {
    ds_json_destroy_handler(json_handler_ptr);
    return LPA_GENERIC_ERROR;
  }
  json_initiate_auth_ptr->smdp_signature1.data_ptr = lpa_json_util_alloc_base64string_to_bin(json_value_info.value.string_value.json_str,
                                                                                        &(json_initiate_auth_ptr->smdp_signature1.data_len));

  memset(&json_value_info, 0, sizeof(ds_json_value_info_s));
  if(DS_JSON_SUCCESS != ds_json_get_value_info_at_path(json_value_obj_ptr, JSON_INIT_AUTH_RSP_CERTFORMATTOBEUSED_PATH, &json_value_info))
  {
    ds_json_destroy_handler(json_handler_ptr);
    return LPA_GENERIC_ERROR;
  }
  json_initiate_auth_ptr->cert_format_to_be_used.data_ptr = lpa_json_util_alloc_base64string_to_bin(json_value_info.value.string_value.json_str,
                                                                                               &(json_initiate_auth_ptr->cert_format_to_be_used.data_len));

  memset(&json_value_info, 0, sizeof(ds_json_value_info_s));
  if(DS_JSON_SUCCESS != ds_json_get_value_info_at_path(json_value_obj_ptr, JSON_INIT_AUTH_RSP_CURVETOBEUSED_PATH, &json_value_info))
  {
    ds_json_destroy_handler(json_handler_ptr);
    return LPA_GENERIC_ERROR;
  }
  json_initiate_auth_ptr->curve_to_be_used.data_ptr = lpa_json_util_alloc_base64string_to_bin(json_value_info.value.string_value.json_str,
                                                                                         &(json_initiate_auth_ptr->curve_to_be_used.data_len));

  memset(&json_value_info, 0, sizeof(ds_json_value_info_s));
  if(DS_JSON_SUCCESS != ds_json_get_value_info_at_path(json_value_obj_ptr, JSON_INIT_AUTH_RSP_SMDPCERTIFICATE_PATH, &json_value_info))
  {
    ds_json_destroy_handler(json_handler_ptr);
    return LPA_GENERIC_ERROR;
  }
  json_initiate_auth_ptr->smdp_certificate.data_ptr = lpa_json_util_alloc_base64string_to_bin(json_value_info.value.string_value.json_str,
                                                                                         &(json_initiate_auth_ptr->smdp_certificate.data_len));

  /* cleanup */
  ds_json_destroy_handler(json_handler_ptr);
  return LPA_SUCCESS;
} /* lpa_json_initiate_auth_decoder */


/*===========================================================================
FUNCTION LPA_JSON_GET_BOUND_PROFILE_PACKAGE_DECODER

DESCRIPTION
  This function parse the ES9+.GetBoundProfilePackage response JSON string.
  Then populate the json_initiate_auth_ptr passed in from LPA caller

DEPENDENCIES
  None

RETURN VALUE
  Decoding status

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_json_get_bound_profile_package_decoder(
  lpa_data_type*             json_data_ptr,
  lpa_lpd_get_bpp_resp_type* json_get_bound_profile_package_ptr
)
{
  ds_json_handler*     json_handler_ptr         = NULL;
  ds_json_value*       json_value_obj_ptr       = NULL;
  ds_json_value_info_s json_value_info;

  if (NULL == json_get_bound_profile_package_ptr)
  {
    /* No data return pointer provided */
    return LPA_GENERIC_ERROR;
  }

  memset(&json_value_info, 0, sizeof(ds_json_value_info_s));

  /* Create a JSON handler */
  json_handler_ptr = ds_json_create_handler();

  if(NULL == json_handler_ptr)
  {
    return LPA_GENERIC_ERROR;
  }

  if( DS_JSON_SUCCESS != ds_json_parse_from_content(json_handler_ptr, (const char*)json_data_ptr->data_ptr, json_data_ptr->data_len))
  {
    ds_json_destroy_handler(json_handler_ptr);
    return LPA_GENERIC_ERROR;
  }

  /* Get root node */
  json_value_obj_ptr = ds_json_get_root(json_handler_ptr);
  if(NULL == json_value_obj_ptr)
  {
    ds_json_destroy_handler(json_handler_ptr);
    return LPA_GENERIC_ERROR;
  }

  if(DS_JSON_SUCCESS != ds_json_get_value_info_at_path(json_value_obj_ptr, JSON_HTTP_RSP_HEADER_STATUS_PATH, &json_value_info))
  {
    ds_json_destroy_handler(json_handler_ptr);
    return LPA_GENERIC_ERROR;
  }

  if (strcmp(JSON_HTTP_RSP_HEADER_STATUS_EXECUTED_SUCCESS, json_value_info.value.string_value.json_str))
  {
     json_get_bound_profile_package_ptr->http_response = LPA_SUCCESS;
  }
  else
  {
     json_get_bound_profile_package_ptr->http_response = LPA_ERROR_SMDP_RESPONSE;
  }

  if(DS_JSON_SUCCESS != ds_json_get_value_info_at_path(json_value_obj_ptr, JSON_GET_BPP_RSP_BOUNDPROFILEPACKAGE_PATH, &json_value_info))
  {
    ds_json_destroy_handler(json_handler_ptr);
    return LPA_GENERIC_ERROR;
  }
  json_get_bound_profile_package_ptr->bpp.data_ptr = lpa_json_util_alloc_base64string_to_bin(json_value_info.value.string_value.json_str,
                                                                                   &(json_get_bound_profile_package_ptr->bpp.data_len));

  /* cleanup */
  ds_json_destroy_handler(json_handler_ptr);

  return LPA_SUCCESS;
} /* lpa_json_get_bound_profile_package_decoder */


/*===========================================================================
FUNCTION LPA_JSON_INITIATE_AUTH_ENCODER

DESCRIPTION
  This function encode the ES9+.InitiateAuthentication command to JSON string.

DEPENDENCIES
  Caller is responsible to free the data_ptr in json_data_ptr

RETURN VALUE
  Encoding status

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_json_initiate_auth_encoder(
  lpa_data_type*                  json_data_ptr,
  lpa_lpd_initiate_auth_req_type* json_initiate_auth_ptr
)
{
  ds_json_handler*               json_handler_ptr       = NULL;
  ds_json_value*                 json_value_obj_ptr     = NULL;
  ds_json_value_info_s           json_value_info;
  ds_json_serialize_content_s    json_serialize_content;
  char                           svn_formatted_string[LPA_JSON_SVN_FORMATED_STR_LEN];
  char*                          temp_base64string      = NULL;
  lpa_lpd_euicc_phase_enum_type  euicc_phase            = LPA_LPD_PHASE1_EUICC;
  int16                          json_response          = DS_JSON_SUCCESS;

  if (NULL == json_initiate_auth_ptr)
  {
    /* No input pointer provided */
    return LPA_GENERIC_ERROR;
  }

  memset(&json_serialize_content, 0, sizeof(ds_json_serialize_content_s));
  memset(svn_formatted_string, 0, LPA_JSON_SVN_FORMATED_STR_LEN*sizeof(char));

  /* Retrieve the eUICC phase */
  if(LPA_SUCCESS != lpa_lpd_retrieve_euicc_phase(json_initiate_auth_ptr->slot_id,
                                                 &euicc_phase))
  {
    LPA_MSG_ERR_1("Invalid or unsupported phase conformance, phase=0x%x", euicc_phase);
    return LPA_GENERIC_ERROR;
  }

  /* Create a JSON handler */
  json_handler_ptr = ds_json_create_handler();

  if(NULL == json_handler_ptr)
  {
    return LPA_GENERIC_ERROR;
  }

  /* Get root node */
  json_value_obj_ptr = ds_json_get_root(json_handler_ptr);
  if(NULL == json_value_obj_ptr)
  {
    LPA_MSG_ERR_0("ds_json_get_root failed");
    ds_json_destroy_handler(json_handler_ptr);
    return LPA_GENERIC_ERROR;
  }

  memset(&json_value_info, 0, sizeof(ds_json_value_info_s));

  temp_base64string = lpa_json_util_alloc_bin_to_base64string(json_initiate_auth_ptr->euicc_challenge, LPA_JSON_EUICC_CHANLLENGE_SIZE);
  if (NULL == temp_base64string)
  {
    return LPA_GENERIC_ERROR;
  }
  json_value_info.type = DS_JSON_TYPE_STRING;
  json_value_info.value.string_value.json_str        = temp_base64string;
  json_value_info.value.string_value.json_str_length = strlen(temp_base64string);

  if(euicc_phase == LPA_LPD_PHASE1_EUICC ||
     euicc_phase == LPA_LPD_PHASE1_1_EUICC)
  {
    json_response = 
      ds_json_set_value_at_path(json_value_obj_ptr, JSON_INIT_AUTH_CMD_EUICCCHANGE_PATH, &json_value_info);
  }
  else 
  {
    json_response = 
      ds_json_set_value_at_path(json_value_obj_ptr, JSON_INIT_AUTH_CMD_EUICCCHANGE_PATH_PH2, &json_value_info);
  }

  if(DS_JSON_SUCCESS != json_response)
  {
    LPA_MSG_ERR_0("ds_json_set_value_at_path failed");
    ds_json_destroy_handler(json_handler_ptr);
    LPA_FREE(temp_base64string);
    return LPA_GENERIC_ERROR;
  }
  LPA_FREE(temp_base64string);

  memset(&json_value_info, 0, sizeof(ds_json_value_info_s));
  lpa_json_util_format_svn_to_string(json_initiate_auth_ptr->svn, svn_formatted_string);
  json_value_info.type = DS_JSON_TYPE_STRING;
  json_value_info.value.string_value.json_str        = svn_formatted_string;
  json_value_info.value.string_value.json_str_length = strlen(svn_formatted_string);
  if(DS_JSON_SUCCESS !=  ds_json_set_value_at_path(json_value_obj_ptr, JSON_INIT_AUTH_CMD_SVN_PATH_PATH, &json_value_info))
  {
    LPA_MSG_ERR_0("ds_json_set_value_at_path failed");
    ds_json_destroy_handler(json_handler_ptr);
    return LPA_GENERIC_ERROR;
  }

  memset(&json_value_info, 0, sizeof(ds_json_value_info_s));
  json_value_info.type = DS_JSON_TYPE_STRING;
  if(euicc_phase == LPA_LPD_PHASE1_EUICC) 
  {
    /* For PHASE 1, encode certificate info */
    temp_base64string = lpa_json_util_alloc_bin_to_base64string(json_initiate_auth_ptr->cert_info.data_ptr,
                                                                json_initiate_auth_ptr->cert_info.data_len);
  }
  else
  {
    /* For PHASE 1.1 or PHASE 2, encode entire eUICCInfo*/
    temp_base64string = lpa_json_util_alloc_bin_to_base64string(json_initiate_auth_ptr->euicc_info.data_ptr,
                                                                json_initiate_auth_ptr->euicc_info.data_len);
  }
  if (NULL == temp_base64string)
  {
    return LPA_GENERIC_ERROR;
  }
  json_value_info.value.string_value.json_str = temp_base64string;
  json_value_info.value.string_value.json_str_length = strlen(temp_base64string);
  if (euicc_phase == LPA_LPD_PHASE1_EUICC)
  {
    json_response = 
      ds_json_set_value_at_path(json_value_obj_ptr, JSON_INIT_AUTH_CMD_CERTINFO_PATH, &json_value_info);
  }
  else if (euicc_phase == LPA_LPD_PHASE1_1_EUICC) 
  {
    json_response = 
      ds_json_set_value_at_path(json_value_obj_ptr, JSON_INIT_AUTH_CMD_EUICCINFO_PATH, &json_value_info);
  }
  else
  {
    json_response = 
      ds_json_set_value_at_path(json_value_obj_ptr, JSON_INIT_AUTH_CMD_EUICCINFO_PATH_PH2, &json_value_info);
  }
  if(DS_JSON_SUCCESS != json_response)
  {
    LPA_FREE(temp_base64string);
    LPA_MSG_ERR_0("ds_json_set_value_at_path failed");
    ds_json_destroy_handler(json_handler_ptr);
    return LPA_GENERIC_ERROR;
  }
  LPA_FREE(temp_base64string);

  memset(&json_value_info, 0, sizeof(ds_json_value_info_s));
  json_value_info.type = DS_JSON_TYPE_STRING;
  json_value_info.value.string_value.json_str = json_initiate_auth_ptr->smdp_address.data;
  json_value_info.value.string_value.json_str_length = json_initiate_auth_ptr->smdp_address.data_len;     /* is NULL terminated? */
  ds_json_set_value_at_path(json_value_obj_ptr, JSON_INIT_AUTH_CMD_SMDPADDRESS_PATH, &json_value_info);

  if(DS_JSON_SUCCESS != ds_json_serialize_to_content(json_handler_ptr, &json_serialize_content))
  {
    LPA_MSG_ERR_0("ds_json_serialize_to_content failed");
    ds_json_destroy_handler(json_handler_ptr);
    return LPA_GENERIC_ERROR;
  }

  json_data_ptr->data_ptr = LPA_MALLOC(json_serialize_content.content_size);
  if(json_data_ptr->data_ptr == NULL)
  {
    ds_json_destroy_handler(json_handler_ptr);
    return LPA_ERROR_HEAP_EXHAUSTED;
  }
  json_data_ptr->data_len = json_serialize_content.content_size;
  memscpy(json_data_ptr->data_ptr,
          json_data_ptr->data_len,
          json_serialize_content.content,
          json_serialize_content.content_size);

  /* cleanup */
  ds_json_destroy_handler(json_handler_ptr);

  return LPA_SUCCESS;
} /* lpa_json_initiate_auth_encoder */


/*===========================================================================
FUNCTION LPA_JSON_GET_BOUND_PROFILE_PACKAGE_ENCODER

DESCRIPTION
  This function encode the ES9+.GetBoundProfilePackage command to JSON string.

DEPENDENCIES
  None

RETURN VALUE
  Encoding status

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_json_get_bound_profile_package_encoder(
  lpa_data_type*              json_data_ptr,
  lpa_lpd_get_bpp_req_type*   json_get_bpp_ptr
)
{
  ds_json_handler*               json_handler_ptr         = NULL;
  ds_json_value*                 json_value_obj_ptr       = NULL;
  ds_json_value_info_s           json_value_info;
  ds_json_serialize_content_s    json_serialize_content;
  int16                          json_response            = DS_JSON_SUCCESS;
  char*                          temp_ascii_string        = NULL;
  char*                          temp_base64string        = NULL;
  lpa_lpd_euicc_phase_enum_type  euicc_phase              = LPA_LPD_PHASE1_EUICC;

  if (NULL == json_get_bpp_ptr)
  {
    /* No input pointer provided */
    return LPA_GENERIC_ERROR;
  }

  /* Retrieve the eUICC phase */
  if(LPA_SUCCESS != lpa_lpd_retrieve_euicc_phase(json_get_bpp_ptr->slot_id,
                                                 &euicc_phase))
  {
    LPA_MSG_ERR_1("Invalid or unsupported phase conformance, phase=0x%x", euicc_phase);
    return LPA_GENERIC_ERROR;
  }

  /* Create a JSON handler */
  json_handler_ptr = ds_json_create_handler();

  if(NULL == json_handler_ptr)
  {
    ds_json_destroy_handler(json_handler_ptr);
    return LPA_GENERIC_ERROR;
  }

  /* Get root node */
  json_value_obj_ptr = ds_json_get_root(json_handler_ptr);
  if(NULL == json_value_obj_ptr)
  {
    ds_json_destroy_handler(json_handler_ptr);
    return LPA_GENERIC_ERROR;
  }

  memset(&json_value_info, 0, sizeof(ds_json_value_info_s));
  json_value_info.type = DS_JSON_TYPE_STRING;
  temp_ascii_string = lpa_json_util_alloc_bin_to_ascii_string(json_get_bpp_ptr->transaction_id_data.data_ptr, json_get_bpp_ptr->transaction_id_data.data_len);
  if (NULL == temp_ascii_string)
  {
    return LPA_GENERIC_ERROR;
  }
  json_value_info.value.string_value.json_str        = temp_ascii_string;
  json_value_info.value.string_value.json_str_length = strlen(temp_ascii_string);
  if(DS_JSON_SUCCESS != ds_json_set_value_at_path(json_value_obj_ptr, JSON_GET_BPP_CMD_TRANSACTIONID_PATH, &json_value_info))
  {
    ds_json_destroy_handler(json_handler_ptr);
    return LPA_GENERIC_ERROR;
  }

  memset(&json_value_info, 0, sizeof(ds_json_value_info_s));
  temp_base64string = lpa_json_util_alloc_bin_to_base64string(json_get_bpp_ptr->prepare_download_data.data_ptr, json_get_bpp_ptr->prepare_download_data.data_len);
  if (NULL == temp_base64string)
  {
    return LPA_GENERIC_ERROR;
  }
  json_value_info.type = DS_JSON_TYPE_STRING;
  json_value_info.value.string_value.json_str        = temp_base64string;
  json_value_info.value.string_value.json_str_length = strlen(temp_base64string);

  if (euicc_phase == LPA_LPD_PHASE1_EUICC)
  {
    json_response = 
      ds_json_set_value_at_path(json_value_obj_ptr, JSON_GET_BPP_CMD_EUICCPREPAREDOWNLOADRESPONSE_PATH_PH1, &json_value_info);
  }
  else if (euicc_phase == LPA_LPD_PHASE1_1_EUICC) 
  {
    json_response = 
      ds_json_set_value_at_path(json_value_obj_ptr, JSON_GET_BPP_CMD_EUICCPREPAREDOWNLOADRESPONSE_PATH_PH1_1, &json_value_info);
  }
  else
  {
    json_response = 
      ds_json_set_value_at_path(json_value_obj_ptr, JSON_GET_BPP_CMD_EUICCPREPAREDOWNLOADRESPONSE_PATH_PH2, &json_value_info);
  }
  if(DS_JSON_SUCCESS != json_response)
  {
    ds_json_destroy_handler(json_handler_ptr);
    return LPA_GENERIC_ERROR;
  }

  if(DS_JSON_SUCCESS != ds_json_serialize_to_content(json_handler_ptr, &json_serialize_content))
  {
    ds_json_destroy_handler(json_handler_ptr);
    return LPA_GENERIC_ERROR;
  }

  json_data_ptr->data_len = json_serialize_content.content_size;
  json_data_ptr->data_ptr = LPA_MALLOC(json_data_ptr->data_len);
  if(json_data_ptr->data_ptr == NULL)
  {
    ds_json_destroy_handler(json_handler_ptr);
    return LPA_ERROR_HEAP_EXHAUSTED;
  }

  memscpy(json_data_ptr->data_ptr,
          json_data_ptr->data_len,
          json_serialize_content.content,
          json_serialize_content.content_size);

  /* cleanup */
  ds_json_destroy_handler(json_handler_ptr);

  return LPA_SUCCESS;
} /* lpa_json_get_bound_profile_package_encoder */


/*===========================================================================
FUNCTION LPA_JSON_HANDLE_PROFILE_INSTALL_RESULT_ENCODER

DESCRIPTION
  This function encode the ES9+.HandleProfileInstallationResult command to JSON string.

DEPENDENCIES
  None

RETURN VALUE
  Encoding status

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_json_handle_profile_install_result_encoder(
  lpa_data_type*                               json_data_ptr,
  lpa_lpd_handle_installation_result_req_type* json_handle_profile_install_result_ptr
)
{
  ds_json_handler*               json_handler_ptr         = NULL;
  ds_json_value*                 json_value_obj_ptr       = NULL;
  ds_json_value_info_s           json_value_info;
  ds_json_serialize_content_s    json_serialize_content;
  char*                          temp_base64string        = NULL;

  if (NULL == json_handle_profile_install_result_ptr)
  {
    /* No input pointer provided */
    return LPA_GENERIC_ERROR;
  }

  /* Create a JSON handler */
  json_handler_ptr = ds_json_create_handler();

  if(NULL == json_handler_ptr)
  {
    return LPA_GENERIC_ERROR;
  }

  /* Get root node */
  json_value_obj_ptr = ds_json_get_root(json_handler_ptr);
  if(NULL == json_value_obj_ptr)
  {
    ds_json_destroy_handler(json_handler_ptr);
    return LPA_GENERIC_ERROR;
  }

  memset(&json_value_info, 0, sizeof(ds_json_value_info_s));
  json_value_info.type = DS_JSON_TYPE_STRING;
  temp_base64string = lpa_json_util_alloc_bin_to_base64string( json_handle_profile_install_result_ptr->profile_installation_result.data_ptr,
                                                          json_handle_profile_install_result_ptr->profile_installation_result.data_len );
  if (NULL == temp_base64string)
  {
    return LPA_GENERIC_ERROR;
  }
  json_value_info.value.string_value.json_str        = temp_base64string;
  json_value_info.value.string_value.json_str_length = strlen(temp_base64string);
  if(DS_JSON_SUCCESS != ds_json_set_value_at_path(json_value_obj_ptr, JSON_HANDLE_INSTALL_RESULT_RESULT_PATH, &json_value_info))
  {
    ds_json_destroy_handler(json_handler_ptr);
    return LPA_GENERIC_ERROR;
  }

  if(DS_JSON_SUCCESS != ds_json_serialize_to_content(json_handler_ptr, &json_serialize_content))
  {
    ds_json_destroy_handler(json_handler_ptr);
    return LPA_GENERIC_ERROR;
  }
  json_data_ptr->data_ptr = LPA_MALLOC(json_serialize_content.content_size);
  if(json_data_ptr->data_ptr == NULL)
  {
    ds_json_destroy_handler(json_handler_ptr);
    return LPA_ERROR_HEAP_EXHAUSTED;
  }
  json_data_ptr->data_len = json_serialize_content.content_size;
  memscpy(json_data_ptr->data_ptr,
          json_data_ptr->data_len,
          json_serialize_content.content,
          json_serialize_content.content_size);

  /* cleanup */
  ds_json_destroy_handler(json_handler_ptr);

  return LPA_SUCCESS;
} /* lpa_json_handle_profile_install_result_encoder */

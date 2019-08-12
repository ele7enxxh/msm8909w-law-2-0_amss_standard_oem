/*==============================================================================

                        ds_http_auth_generator.cpp

GENERAL DESCRIPTION
  Generates HTTP header for HTTP Authentication

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/20/15    ml     Use memory from modem heap
04/15/15    ml     Created file/Initial version.
==============================================================================*/
#include "ds_http_auth_generator.h"

#include "ds_http_utility.h"
#include "ds_ASStringStream.h"
#include "ds_appsrv_mem.h"

#include "data_msg.h"
extern "C"
{
  #include "md5.h"
}

#include <cctype>
#include <cstdlib>
#include <stringl/stringl.h>


namespace
{

/*==============================================================================
                              Static functions
==============================================================================*/
/*===========================================================================
* FUNCTION: IxBase64::EncodeBase64()
*
* DESCRIPTION:
*   This function encodes the given string.
*
*   It extracts char by char and extract 6 bits from the sequece of bits and
*   converting them using Base64 alphabet. In this conversion 3 chars are
*   represented as 4 chars in Base64. While regrouping above few bytes left
*   over. We add base64 padding char '='to Base64 representation of left
*   over bytes.
*
* INPUT/OUTPUT PARAMETERS:
* @param   cpcIn     - Input string to be Encoded
* @param   nInLen    - Length of the Input String
* @param   pcOut     - Output string to place the encoded string
*
* RETURN VALUE:
* @return  char    - Return a pointer to the output string OR
*                      NULL if output string pointer is not valid.
*
* SIDE EFFECTS:
*
===========================================================================*/
char* EncodeBase64(const char *cpcIn, int32  nInLen, char *pcOut);
uint32 base64len(uint32 len);


const char MD5_RANDOM_STR[]            = "asdfasdfqwer123132"; // used for creating client nonce
const char MD5_DEFAULT_NONCE_COUNTER[] = "00000001";
// Base64 alphabet
const char basis_64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

} /* namespace */










ds_http_auth_generator::ds_http_auth_generator()
: is_proxy(false), auth_type(DS_HTTP_AUTH_BASIC)
{ }


void ds_http_auth_generator::clear()
{
  is_proxy  = false;
  auth_type = DS_HTTP_AUTH_BASIC;
  ds_http_auth::clear();
}


bool ds_http_auth_generator::parse_auth_header(const char* auth_str, uint32 http_status)
{
  clear();

  switch(http_status)
  {
    case 401:
      is_proxy = false;
      break;

    case 407:
      is_proxy = true;
      break;

    default:
      DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "decode_auth_header - not auth status %d", http_status);
      return false;
  }


  ASStringStream iss(auth_str);
  ASString token;


  iss.get_next(token, ' ');

  if(token == "Basic")
  {
    auth_type = DS_HTTP_AUTH_BASIC;
  }
  else if(token == "Digest")
  {
    auth_type = DS_HTTP_AUTH_DIGEST;
  }
  else
  {
    DATA_APPSRV_MSG_SPRINTF_1(MSG_LEGACY_ERROR, "decode_auth_header - Unknown auth type : %s", token.c_str());
    return false;
  }

  iss.get_next(token, '\0');
  return ds_http_auth::parse_auth_header(token.c_str());
}


bool ds_http_auth_generator::generate_response(
                                               const ds_http_credential_s_type* credential,
                                               const char*                      method,
                                               const char*                      uri_path,
                                               uint8*                           entity_body,
                                               uint32                           entity_size,
                                               ASBuffer&                        auth_header_str
                                               )
{
  auth_header_str.clear();

  if(DS_HTTP_AUTH_BASIC == auth_type)
  {
    return generate_basic_auth_response(credential, auth_header_str);
  }
  else // if(DS_HTTP_AUTH_DIGEST == auth_type)
  {
    return generate_digest_auth_response(
                                         credential,
                                         method,
                                         uri_path,
                                         entity_body,
                                         entity_size,
                                         auth_header_str
                                         );
  }
}


bool ds_http_auth_generator::generate_basic_auth_response(
                                                          const ds_http_credential_s_type* credential,
                                                          ASBuffer&                        auth_header_str
                                                          )
{
  uint32 uname_len   = credential->username_len;
  uint32 pass_len    = credential->password_len;
  uint32 auth_strlen = 0;
  char*  encode_str  = NULL;
  char*  response    = NULL;

  if(uname_len >= DS_HTTP_USERNAME_MAX_LEN || pass_len >= DS_HTTP_USERNAME_MAX_LEN)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "generate_basic_auth_response - username/password len error");
    return false;
  }

  auth_strlen = uname_len + pass_len + 1; // 1 colon
  encode_str  = (char*)ds_appsrv_alloc(sizeof(char), auth_strlen+1);
  if(NULL == encode_str)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "generate_basic_auth_response - failed to alloc encode_str");
    return false;
  }

  response = (char*)ds_appsrv_alloc(sizeof(char), base64len(auth_strlen)+1);
  if(NULL == response)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "generate_basic_auth_response - failed to alloc response");
    ds_appsrv_free(encode_str);
    return false;
  }

  memset(encode_str, 0, auth_strlen+1);
  memset(response, 0, base64len(auth_strlen)+1);

  memscpy(encode_str, uname_len, credential->username, uname_len);
  encode_str[uname_len] = ':';
  memscpy(encode_str + uname_len + 1, pass_len, credential->password, pass_len);

  EncodeBase64(encode_str, auth_strlen, response);

  if(is_proxy)
    auth_header_str.append("\r\nProxy-Authorization: Basic ");
  else
    auth_header_str.append("\r\nAuthorization: Basic ");

  auth_header_str.append(response);

  ds_appsrv_free(encode_str);
  ds_appsrv_free(response);

  if(auth_header_str.error())
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "generate_basic_auth_response - buffer error");
  }

  return !auth_header_str.error();
}



bool ds_http_auth_generator::generate_digest_auth_response(
                                                           const ds_http_credential_s_type* credential,
                                                           const char*                      method,
                                                           const char*                      uri_path,
                                                           uint8*                           entity_body,
                                                           uint32                           entity_size,
                                                           ASBuffer&                        auth_header_str
                                                           )
{
  MD5_CTX       context;
  unsigned char md5_result[MD5_RES_SIZE] = {0};
  char          cnonce[MD5_STR_SIZE+1]   = {0};
  char          response[MD5_STR_SIZE+1] = {0};

  // create cnonce
  // TODO: Better way to generate cnonce?
  MD5Init(&context);
  MD5Update(&context, (unsigned char*)MD5_RANDOM_STR, strlen(MD5_RANDOM_STR));
  MD5Update(&context, (unsigned char*)credential->password, credential->password_len);
  MD5Update(&context, (unsigned char*)nonce.c_str(), nonce.size());
  MD5Final(md5_result, &context);
  hex_to_hex_str(md5_result, cnonce);
  DATA_APPSRV_MSG_SPRINTF_1(MSG_LEGACY_LOW, "auth_digest - Client nonce : %s", cnonce);


  // Create response hash
  if(!generate_auth_hash(
                         response,
                         credential,
                         method,
                         uri_path,
                         cnonce,
                         MD5_DEFAULT_NONCE_COUNTER,
                         entity_body,
                         entity_size
                         ))
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "generate_digest_auth_response - Failed to generate auth hash");
    return false;
  }


  // Create response auth header
  auth_header_str.clear();

  if(is_proxy)
    auth_header_str.append("\r\nProxy-Authorization: Digest realm=\"");
  else
    auth_header_str.append("\r\nAuthorization: Digest realm=\"");

  auth_header_str.append(realm);
  auth_header_str.append("\", uri=\"");
  auth_header_str.append(uri_path);
  auth_header_str.append("\", nonce=\"");
  auth_header_str.append(nonce);
  auth_header_str.append("\", username=\"");
  auth_header_str.append(credential->username);
  auth_header_str.append("\"");

  if(!opaque.empty())
  {
    auth_header_str.append(", opaque=\"");
    auth_header_str.append(opaque);
    auth_header_str.append("\"");
  }


  switch(qop)
  {
    case DS_HTTP_AUTH_QOP_AUTH:
    {
      auth_header_str.append(", qop=auth, cnonce=\"");
      auth_header_str.append(cnonce);
      auth_header_str.append("\", nc=");
      auth_header_str.append(MD5_DEFAULT_NONCE_COUNTER);
    }
    break;

    case DS_HTTP_AUTH_QOP_AUTH_INT:
    {
      auth_header_str.append(", qop=auth-int, cnonce=\"");
      auth_header_str.append(cnonce);
      auth_header_str.append("\", nc=");
      auth_header_str.append(MD5_DEFAULT_NONCE_COUNTER);
    }
    break;

    case DS_HTTP_AUTH_QOP_UNSPECIFIED:
    default:
    break;
  }

  switch(algorithm)
  {
    case DS_HTTP_AUTH_ALGORITHM_MD5:
    {
      auth_header_str.append(", algorithm=MD5");
    }
    break;

    case DS_HTTP_AUTH_ALGORITHM_MD5_SESS:
    {
      auth_header_str.append(", algorithm=MD5-sess");
    }
    break;

    case DS_HTTP_AUTH_ALGORITHM_AKAV1_MD5:
    {
      auth_header_str.append(", algorithm=AKAv1-MD5");
    }
    break;

    case DS_HTTP_AUTH_ALGORITHM_UNSPECIFIED: // fallthrough
    default:
      break;
  }

  auth_header_str.append(", response=\"");
  auth_header_str.append(response);
  auth_header_str.append("\"");

  return !auth_header_str.error();
}





bool ds_http_auth_generator::parse_auth_header_helper(const ASString& name, const ASString& value)
{
  return true;
}




namespace
{

/*==============================================================================
                        Static function definitions
==============================================================================*/
char* EncodeBase64(const char *cpcIn, int32 nInLen, char *pcOut)
{
  char *pDest;
  char *pSrc;
  byte c1,c2,c3;
  int32   nOutLen = ((nInLen + 2) / 3) * 4;

  if(pcOut == NULL) {
    return NULL;
  }

  /* move the data to the end */
  pSrc = pcOut + nOutLen - nInLen;
  memscpy(pSrc, (size_t)nInLen, cpcIn, (size_t)nInLen);
  pDest = pcOut;

  while (nInLen) {

    // extract first char
    c1 = *pSrc++;
    *pDest++ = basis_64[c1>>2];  // encode first char
    nInLen--;

    if (!nInLen){
      c2 = 0;
    } else {
      c2 = *pSrc++;  // second char
    }

    // encode second char
    *pDest++ = basis_64[((c1 & 0x3) << 4) | ((c2 & 0xF0) >> 4)];

    // append '=' to make encoded string size is multiple of 4
    if (!nInLen){
      *pDest++ = '=';
      *pDest++ = '=';
      break;
    }

    nInLen--;

    if (!nInLen){
      c3 = 0;
    } else {
      c3 = *pSrc++;    // third char
    }

    // encode third char
    *pDest++ = basis_64[((c2 & 0xF) << 2) | ((c3 & 0xC0) >> 6)];

    if (!nInLen){
      *pDest++ = '=';
      break;
    }
    --nInLen;

    // fourth char
    *pDest++ = basis_64[c3 & 0x3F];
  }

  *pDest = '\0';

  return pcOut;
}



uint32 base64len(uint32 len)
{
  return (((len + 2) / 3) * 4) + 1;
}


} /* namespace */

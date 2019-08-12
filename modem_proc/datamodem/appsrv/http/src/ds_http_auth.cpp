/*==============================================================================

                              ds_http_auth.cpp

GENERAL DESCRIPTION
  HTTP Authentication base class

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/12/15    ml     Fix NULL check condition for for HA2 generation.
05/20/15    ml     Use memory from modem heap
04/15/15    ml     Created file/Initial version.
==============================================================================*/
#include "ds_http_auth.h"

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
                              Static variables
==============================================================================*/

const char   MD5_DELIM_STR[]          = ":";
const char   MD5_QOP_AUTH_STR[]       = "auth";
const char   MD5_QOP_AUTH_INT_STR[]   = "auth-int";
const char   DS_HTTP_MD5_EMPTY_STR[]  = "d41d8cd98f00b204e9800998ecf8427e";
const uint32 MD5_QOP_AUTH_STR_LEN     = 4;
const uint32 MD5_QOP_AUTH_INT_STR_LEN = 8;

} /* namespace */



ds_http_auth::ds_http_auth()
: stale(false),
  algorithm(DS_HTTP_AUTH_ALGORITHM_UNSPECIFIED),
  qop(DS_HTTP_AUTH_QOP_UNSPECIFIED)
{ }


void ds_http_auth::clear()
{
  realm     = "";
  domain    = "";
  nonce     = "";
  opaque    = "";
  stale     = false;
  algorithm = DS_HTTP_AUTH_ALGORITHM_UNSPECIFIED;
  qop       = DS_HTTP_AUTH_QOP_UNSPECIFIED;
}


bool ds_http_auth::parse_auth_header(const char* auth_str)
{
  ASStringStream iss(auth_str);
  ASString name;
  ASString value;
  bool     result = true;

  while(!iss.get_next(name, "="))
  {
    if(name == "realm")
    {
      iss.next_ptr(); // skip "
      iss.get_next(realm, "\"");
    }
    else if(name == "nonce")
    {
      iss.next_ptr(); // skip "
      iss.get_next(nonce, "\"");
    }
    else if(name == "opaque")
    {
      iss.next_ptr(); // skip "
      iss.get_next(opaque, "\"");
    }
    else if(name == "stale")
    {
      iss.next_ptr(); // skip "
      iss.get_next(value, "\"");
      if(value == "true")
      {
        stale = true;
      }
      else if(value == "false")
      {
        stale = false;
      }
      else
      {
        result = false;
        break;
      }
    }
    else if(name == "algorithm")
    {
      iss.get_next(value, ",");
      if(value == "MD5")
      {
        algorithm = DS_HTTP_AUTH_ALGORITHM_MD5;
      }
      else if(value == "MD5-sess")
      {
        algorithm = DS_HTTP_AUTH_ALGORITHM_MD5_SESS;
      }
      else if(value == "AKAv1-MD5")
      {
        algorithm = DS_HTTP_AUTH_ALGORITHM_AKAV1_MD5;
      }
      else
      {
        result = false;
        break;
      }
    }
    else if(name == "qop")
    {
      if('"' == iss.peek())
      {
        iss.next_ptr(); // skip "
        iss.get_next(value, "\"");
      }
      else
      {
        iss.get_next(value, ",");
      }
      if(value == "auth")
      {
        qop = DS_HTTP_AUTH_QOP_AUTH;
      }
      else if(value == "auth-int" || value == "auth,auth-int")
      {
        qop = DS_HTTP_AUTH_QOP_AUTH_INT;
      }
      else
      {
        result = false;
        break;
      }
    }
    else
    {
      if('"' == iss.peek())
      {
        iss.next_ptr(); // skip "
        iss.get_next(value, "\"");
      }
      else
      {
        iss.get_next(value, ",");
      }

      if(!parse_auth_header_helper(name, value))
      {
        result = false;
        break;
      }
    }
    if(',' == iss.peek())
      iss.next_ptr(); // skip ,

    while(isspace(iss.peek()))
      iss.next_ptr(); // skip whitespaces
  }

  if(!result)
  {
    DATA_APPSRV_MSG_SPRINTF_2(MSG_LEGACY_ERROR, "parse_auth_header - invalid auth name or value %s : %s", name.c_str(), value.c_str());
  }

  return result;
}


bool ds_http_auth::generate_auth_hash(
                                      char*                            hash_result,
                                      const ds_http_credential_s_type* credential,
                                      const char*                      method,
                                      const char*                      uri_path,
                                      const char*                      cnonce,
                                      const char*                      nonce_counter,
                                      uint8*                           entity_body,
                                      uint32                           entity_size
                                      )
{
  char ha1[MD5_STR_SIZE+1] = {0};
  char ha2[MD5_STR_SIZE+1] = {0};

  return (generate_ha1_md5(ha1, credential, cnonce)                         &&
          generate_ha2_md5(ha2, method, uri_path, entity_body, entity_size) &&
          generate_hash_result(hash_result, ha1, ha2, cnonce, nonce_counter));
}


bool ds_http_auth::generate_ha1_md5(
                                    char*                            ha1,
                                    const ds_http_credential_s_type* credential,
                                    const char*                      cnonce
                                    )
{
  MD5_CTX       context;
  unsigned char md5_result[MD5_RES_SIZE] = {0};

  if(NULL == ha1 || NULL == credential || NULL == cnonce)
  {
    DATA_APPSRV_MSG3(MSG_LEGACY_ERROR, "generate_ha1_md5 - NULL input %d %d %d", ha1, credential, cnonce);
    return false;
  }

  if( (DS_HTTP_USERNAME_MAX_LEN < credential->username_len || 0 == credential->username_len) ||
      (DS_HTTP_PASSWORD_MAX_LEN < credential->password_len) )
  {
    DATA_APPSRV_MSG2(MSG_LEGACY_ERROR, "generate_ha1_md5 - invalid credential info len ulen:%d plen:%d", credential->username_len, credential->password_len);
    return false;
  }

  // ====== HA1 ======
  MD5Init(&context);
  MD5Update(&context, (unsigned char*)credential->username, credential->username_len);
  MD5Update(&context, (unsigned char*)MD5_DELIM_STR, 1);
  MD5Update(&context, (unsigned char*)realm.c_str(), realm.size());
  MD5Update(&context, (unsigned char*)MD5_DELIM_STR, 1);
  MD5Update(&context, (unsigned char*)credential->password, credential->password_len);
  MD5Final(md5_result, &context);
  hex_to_hex_str(md5_result, ha1);

  if(DS_HTTP_AUTH_ALGORITHM_MD5_SESS == algorithm)
  {
    MD5Init(&context);
    MD5Update(&context, (unsigned char*)ha1, MD5_STR_SIZE);

    MD5Update(&context, (unsigned char*)MD5_DELIM_STR, 1);
    MD5Update(&context, (unsigned char*)nonce.c_str(), nonce.size());

    MD5Update(&context, (unsigned char*)MD5_DELIM_STR, 1);
    MD5Update(&context, (unsigned char*)cnonce, strlen(cnonce));

    MD5Final(md5_result, &context);
    hex_to_hex_str(md5_result, ha1);
  }
  DATA_APPSRV_MSG_SPRINTF_1(MSG_LEGACY_LOW, "generate_ha1_md5 - HA1 : %s", ha1);

  // Util to print username/password. Uncomment if needed for debugging
  // uint32 pslen = credential->password_len;
  // char* pshex = (char*)ds_appsrv_alloc(sizeof(char), (pslen*2 + 1));
  // if(NULL != pshex)
  // {
  //   memset(pshex, 0, pslen*2 + 1);
  //   for(uint32 i = 0; i < pslen; ++i)
  //   {
  //     static const char hex_str[] = "0123456789abcdef";
  //     unsigned int a = 0, b = 0;

  //     a = credential->password[i]>>4;
  //     a = a & 0x0f;
  //     b = credential->password[i] & 0x0f;

  //     pshex[i*2] = hex_str[a];
  //     pshex[i*2+1] = hex_str[b];
  //   }
  //   DATA_APPSRV_MSG_SPRINTF_3(MSG_LEGACY_LOW, "generate_ha1_md5 - name:%s pass:%s phex:%s", credential->username, credential->password, pshex);
  //   ds_appsrv_free(pshex);
  // }

  return true;
}


bool ds_http_auth::generate_ha2_md5(
                                    char*       ha2,
                                    const char* method,
                                    const char* uri_path,
                                    uint8*      entity_body,
                                    uint32      entity_size
                                    )
{
  MD5_CTX       context;
  unsigned char md5_result[MD5_RES_SIZE] = {0};

  if(NULL == ha2 || NULL == method || NULL == uri_path)
  {
    DATA_APPSRV_MSG3(MSG_LEGACY_ERROR, "generate_ha2_md5 - invalid input %p %p %p", ha2, method, uri_path);
    return false;
  }

  // ===== HA2 =====
  MD5Init(&context);
  MD5Update(&context, (unsigned char*)method, strlen(method));
  MD5Update(&context, (unsigned char*)MD5_DELIM_STR, 1);
  MD5Update(&context, (unsigned char*)uri_path, strlen(uri_path));

  if(DS_HTTP_AUTH_QOP_AUTH_INT == qop)
  {
    MD5_CTX tmp_context;
    char tmp[MD5_STR_SIZE+1] = {0};

    if(0 == entity_size || NULL == entity_body)
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_LOW, "empty entity");
      memscpy(tmp, MD5_STR_SIZE, DS_HTTP_MD5_EMPTY_STR, MD5_STR_SIZE);
    }
    else
    {
      MD5Init(&tmp_context);
      MD5Update(&tmp_context, (unsigned char*)entity_body, entity_size);
      MD5Final(md5_result, &tmp_context);
      hex_to_hex_str(md5_result, tmp);
    }

    MD5Update(&context, (unsigned char*)MD5_DELIM_STR, 1);
    MD5Update(&context, (unsigned char*)tmp, MD5_STR_SIZE);
  }

  MD5Final(md5_result, &context);
  hex_to_hex_str(md5_result, ha2);
  DATA_APPSRV_MSG_SPRINTF_1(MSG_LEGACY_LOW, "generate_ha2_md5 - HA2 : %s", ha2);

  return true;
}


bool ds_http_auth::generate_hash_result(
                                        char*       hash_result,
                                        char*       ha1,
                                        char*       ha2,
                                        const char* cnonce,
                                        const char* nonce_counter
                                        )
{
  MD5_CTX       context;
  unsigned char md5_result[MD5_RES_SIZE] = {0};

  if(NULL == ha1 || NULL == ha2 || NULL == cnonce)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "generate_hash_result - invalid input");
    return false;
  }


  // ===== Res =====
  MD5Init(&context);
  MD5Update(&context, (unsigned char*)ha1, MD5_STR_SIZE);
  MD5Update(&context, (unsigned char*)MD5_DELIM_STR, 1);
  MD5Update(&context, (unsigned char*)nonce.c_str(), nonce.size());

  if(DS_HTTP_AUTH_QOP_UNSPECIFIED != qop)
  {
    MD5Update(&context, (unsigned char*)MD5_DELIM_STR, 1);
    MD5Update(&context, (unsigned char*)nonce_counter, strlen(nonce_counter));
    MD5Update(&context, (unsigned char*)MD5_DELIM_STR, 1);
    MD5Update(&context, (unsigned char*)cnonce, strlen(cnonce));
    MD5Update(&context, (unsigned char*)MD5_DELIM_STR, 1);

    if(DS_HTTP_AUTH_QOP_AUTH == qop)
    {
      MD5Update(&context, (unsigned char*)MD5_QOP_AUTH_STR, MD5_QOP_AUTH_STR_LEN);
    }
    else // if(DS_HTTP_AUTH_QOP_AUTH_INT == qop)
    {
      MD5Update(&context, (unsigned char*)MD5_QOP_AUTH_INT_STR, MD5_QOP_AUTH_INT_STR_LEN);
    }
  }

  MD5Update(&context, (unsigned char*)MD5_DELIM_STR, 1);
  MD5Update(&context, (unsigned char*)ha2, MD5_STR_SIZE);
  MD5Final(md5_result, &context);
  hex_to_hex_str(md5_result, hash_result);
  DATA_APPSRV_MSG_SPRINTF_1(MSG_LEGACY_MED, "generate_hash_result - hash_result : %s", hash_result);

  return true;
}


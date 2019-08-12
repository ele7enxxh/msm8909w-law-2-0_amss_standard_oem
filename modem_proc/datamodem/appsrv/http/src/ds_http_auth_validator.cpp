/*==============================================================================

                      ds_http_auth_validator.cpp

GENERAL DESCRIPTION
  Validates HTTP response containing authentication information

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/15/15    ml     Created file/Initial version.
==============================================================================*/
#include "ds_http_auth_validator.h"

#include "data_msg.h"

namespace
{

const char DS_HTTP_AUTH_EMPTY_METHOD[] = "";

}


ds_http_auth_validator::ds_http_auth_validator()
{ }


void ds_http_auth_validator::clear()
{
  cnonce        = "";
  nonce_counter = "";
  rspauth       = "";
  ds_http_auth::clear();
}


bool ds_http_auth_validator::parse_auth_header(const char* auth_str)
{
  clear();
  return ds_http_auth::parse_auth_header(auth_str);
}


bool ds_http_auth_validator::validate_response(
                                               const ds_http_credential_s_type* credential,
                                               const char*                      uri_path,
                                               uint8*                           entity_body,
                                               uint32                           entity_size
                                               )
{
  if(0 == rspauth.length())
    return true;

  if(DS_HTTP_AUTH_QOP_UNSPECIFIED != qop &&
     (cnonce.empty() || nonce_counter.empty() || rspauth.empty()))
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "validate_response - missing value for auth calculation");
    return false;
  }

  char response[MD5_STR_SIZE+1] = {0};

  // Create response hash
  if(!generate_auth_hash(
                         response,
                         credential,
                         DS_HTTP_AUTH_EMPTY_METHOD,
                         uri_path,
                         cnonce.c_str(),
                         nonce_counter.c_str(),
                         entity_body,
                         entity_size
                         ))
  {
    return false;
  }

  DATA_APPSRV_MSG_SPRINTF_2(
                            MSG_LEGACY_LOW,
                            "validate_response - rspauth: %s res: %s",
                            rspauth.c_str(),
                            response
                            );
  return (rspauth == response);
}


bool ds_http_auth_validator::parse_auth_header_helper(const ASString& name, const ASString& value)
{
  if("cnonce" == name)
  {
    cnonce = value;
  }
  else if("nc" == name)
  {
    nonce_counter = value;
  }
  else if("rspauth" == name)
  {
    rspauth = value;
  }
  // else ignore

  return true;
}



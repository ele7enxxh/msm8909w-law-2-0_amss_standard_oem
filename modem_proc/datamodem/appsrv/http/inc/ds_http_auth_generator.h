/*==============================================================================

                        ds_http_auth_generator.h

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
04/15/15    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_HTTP_AUTH_GENERATOR_H
#define DS_HTTP_AUTH_GENERATOR_H

#include "ds_http_auth.h"
#include "ds_ASBuffer.h"



class ds_http_auth_generator : public ds_http_auth
{
public:
  ds_http_auth_generator();
  void clear();

  bool parse_auth_header(const char* auth_str, uint32 http_status);
  bool generate_response(
                         const ds_http_credential_s_type* credential,
                         const char*                      method,
                         const char*                      uri_path,
                         uint8*                           entity_body,
                         uint32                           entity_size,
                         ASBuffer&                        auth_header_str
                         );

private:
  bool parse_auth_header_helper(const ASString& name, const ASString& value);
  using ds_http_auth::parse_auth_header;

  bool generate_basic_auth_response(
                                    const ds_http_credential_s_type* credential,
                                    ASBuffer&                        auth_header_str
                                    );
  bool generate_digest_auth_response(
                                     const ds_http_credential_s_type* credential,
                                     const char*                      method,
                                     const char*                      uri_path,
                                     uint8*                           entity_body,
                                     uint32                           entity_size,
                                     ASBuffer&                        auth_header_str
                                     );

public:
  bool              is_proxy;
  ds_http_auth_type auth_type;
};








#endif /* DS_HTTP_AUTH_GENERATOR_H */

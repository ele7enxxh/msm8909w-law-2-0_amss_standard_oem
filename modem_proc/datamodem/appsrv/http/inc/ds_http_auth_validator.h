/*==============================================================================

                      ds_http_auth_validator.h

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
#ifndef DS_HTTP_AUTH_VALIDATOR_H
#define DS_HTTP_AUTH_VALIDATOR_H

#include "ds_http_auth.h"

class ds_http_auth_validator : public ds_http_auth
{
public:
  ds_http_auth_validator();
  void clear();

  bool parse_auth_header(const char* auth_str);
  bool validate_response(
                         const ds_http_credential_s_type* credential,
                         const char*                      uri_path,
                         uint8*                           entity_body,
                         uint32                           entity_size
                         );

private:
  bool parse_auth_header_helper(const ASString& name, const ASString& value);


private:
  ASString cnonce;
  ASString nonce_counter;
  ASString rspauth;
};

#endif /* DS_HTTP_AUTH_VALIDATOR_H */

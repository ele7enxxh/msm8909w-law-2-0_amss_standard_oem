/*==============================================================================

                              ds_http_auth.h

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
04/15/15    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_HTTP_AUTH_H
#define DS_HTTP_AUTH_H

#include "ds_http_types.h"
#include "ds_ASString.h"

const uint32 MD5_RES_SIZE = 16;
const uint32 MD5_STR_SIZE = 32;
const uint32 MD5_MAX_LEN  = 65472;


/* Authentication challenge type */
enum ds_http_auth_type
{
  DS_HTTP_AUTH_BASIC,
  DS_HTTP_AUTH_DIGEST
};


/* Digest algorithm type */
enum ds_http_auth_algorithm_type
{
  DS_HTTP_AUTH_ALGORITHM_UNSPECIFIED,
  DS_HTTP_AUTH_ALGORITHM_MD5,
  DS_HTTP_AUTH_ALGORITHM_MD5_SESS,
  DS_HTTP_AUTH_ALGORITHM_AKAV1_MD5 /* Same algorithm as MD5 */
};


/* Digest qop type */
enum ds_http_auth_qop_type
{
  DS_HTTP_AUTH_QOP_UNSPECIFIED,
  DS_HTTP_AUTH_QOP_AUTH,
  DS_HTTP_AUTH_QOP_AUTH_INT
};


/* Decoded authentication challenge */
class ds_http_auth
{
public:
  ds_http_auth();
  virtual void clear();

protected:
  virtual bool parse_auth_header(const char* auth_str);
  bool generate_auth_hash(
                          char*                            hash_result,
                          const ds_http_credential_s_type* credential,
                          const char*                      method,
                          const char*                      uri_path,
                          const char*                      cnonce,
                          const char*                      nonce_counter,
                          uint8*                           entity_body,
                          uint32                           entity_size
                          );

  bool generate_ha1_md5(
                        char*                            ha1,
                        const ds_http_credential_s_type* credential,
                        const char*                      cnonce
                        );
  bool generate_ha2_md5(
                        char*       ha2,
                        const char* method,
                        const char* uri_path,
                        uint8*      entity_body,
                        uint32      entity_size
                        );
  bool generate_hash_result(
                            char*       hash_result,
                            char*       ha1,
                            char*       ha2,
                            const char* cnonce,
                            const char* nonce_counter
                            );

  virtual bool parse_auth_header_helper(const ASString& name, const ASString& value) = 0;

public:
  ASString                    realm;
  ASString                    domain;
  ASString                    nonce;
  ASString                    opaque;
  bool                        stale;
  ds_http_auth_algorithm_type algorithm;
  ds_http_auth_qop_type       qop;
};




#endif /* DS_HTTP_AUTH_H */

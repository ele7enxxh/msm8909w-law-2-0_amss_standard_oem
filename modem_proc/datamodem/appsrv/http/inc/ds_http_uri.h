/*==============================================================================

                            ds_http_uri.h

GENERAL DESCRIPTION
  URI info manager
    - DNS resolution result cache
    - Permanent redirect cache
    - Permanent redirect file cache management
    - URI utilities (e.g. parsing)

  Copyright (c) 2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/02/15    ml     Permanent redirect
07/28/15    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_HTTP_URI_H
#define DS_HTTP_URI_H

#include "ds_http_types.h"

extern "C"
{
  #include "time_jul.h"
}


const uint32 DS_HTTP_IPV6_SIZE = sizeof(uint32) * 4;

const uint32 DS_HTTP_HOSTNAME_MAX_LEN = 256;
const uint32 DS_HTTP_PATH_MAX_LEN     = 256;
const uint32 DS_HTTP_DEFAULT_PORT     = 80;
const uint32 DS_HTTPS_DEFAULT_PORT    = 443;



/* HTTP Protocols */
enum ds_http_protocol
{
  DS_HTTP_PROTOCOL_HTTP,
  DS_HTTP_PROTOCOL_HTTPS
};


/* DNS resolve IP type */
enum ds_http_ip_version
{
  DS_HTTP_IPV4 = 0,
  DS_HTTP_IPV6
};




/* URI information */
struct ds_http_request_uri
{
  ds_http_protocol protocol;
  sint15           port; // HTTP default - 80, HTTPS default - 443
  char             hostname[DS_HTTP_HOSTNAME_MAX_LEN+1];
  char             path[DS_HTTP_PATH_MAX_LEN+1];

  ds_http_request_uri();
  ds_http_request_uri(const ds_http_request_uri& other);

  ds_http_request_uri& operator=(const ds_http_request_uri& other);

  bool operator==(const ds_http_request_uri& other);

  // bool parse_uri(const char* uri_str);
};



struct ds_http_ip_addr_info
{
  ds_http_ip_version ip_addr_type;
  uint32             ip_addr;
  uint32             ip_addr6[4];

  ds_http_ip_addr_info();
  ds_http_ip_addr_info(const ds_http_ip_addr_info& other);
  ds_http_ip_addr_info& operator=(const ds_http_ip_addr_info& other);
  bool operator==(const ds_http_ip_addr_info& rhs) const;

  void clear();
};


bool parse_uri(const char* uri_str, ds_http_request_uri& uri_info);


void add_redirect_cache(
                        const ds_http_request_uri& origin_uri,
                        const ds_http_request_uri& redirect_uri,
                        const time_julian_type&    expiration_date,
                        bool                       expiration_set
                        );

/* If uri is in redirect cache, it will overwrite the query object with the redirect uri */
void check_and_update_redirect_uri(ds_http_request_uri& query_uri);


void ds_http_init_uri();

#endif /* DS_HTTP_URI_H */

/*==============================================================================

                              ds_http_dns.cpp

GENERAL DESCRIPTION
  DNS query object

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/02/15    ml     Cancel request
11/02/15    ml     HTTP Post chunked support
09/29/15    ml     IPv6 URI support
09/22/15    ml     IPv6 support
09/11/15    ml     DSDS support
05/20/15    ml     Use memory from modem heap
07/21/14    ml     Created file/Initial version.
==============================================================================*/
#include "ds_http_dns.h"

#include "ds_http_request.h"
#include "ds_http_types.h"
#include "ds_appsrv_mem.h"

#include "dssocket.h"
#include "dssocket_defs.h"

#include "data_msg.h"
#include <stringl/stringl.h>


typedef ASList<ds_http_request*>::iterator       request_iterator;
typedef ASList<ds_http_request*>::const_iterator request_const_iterator;


/*==============================================================================
ds_http_dns_query
==============================================================================*/
ds_http_dns_query::ds_http_dns_query(ds_http_request* request)
: dns_query_handle(DSS_ERROR)
{
  request_queue.push_back(request);
}



ds_http_dns_query::~ds_http_dns_query()
{
  DATA_APPSRV_MSG0(MSG_LEGACY_LOW, "~ds_http_dns_query - destructor");
  clear_request(DS_HTTP_ERROR_DNS);
}



bool ds_http_dns_query::start_query(const dss_dns_session_mgr_handle_type session_handle, int ip_family)
{
  sint15            err  = 0;
  dss_dns_addrinfo* hint = ds_appsrv_alloc<dss_dns_addrinfo>();
  char              hostname[DS_HTTP_HOSTNAME_MAX_LEN+1];

  if(NULL == hint)
  {
    DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "ds_http_dns_query - memory allocation error %d", dns_query_handle);
    clear_request(DS_HTTP_ERROR_MEMALLOC);
    return false;
  }

  DATA_APPSRV_MSG_SPRINTF_2(
                            MSG_LEGACY_LOW,
                            "ds_http_dns_query - request id: %d hostname: %s",
                            request_queue.front()->request_id,
                            request_queue.front()->get_hostname()
                            );

  memset(hint, 0, sizeof(dss_dns_addrinfo));
  memset(hostname, 0, sizeof(DS_HTTP_HOSTNAME_MAX_LEN+1));

  hint->ai_family   = ip_family;
  hint->ai_socktype = DSS_SOCK_STREAM;
  hint->ai_protocol = PS_IPPROTO_TCP;

  const char* hname = request_queue.front()->get_hostname();
  if('[' == hname[0])
  {
    // IPv6 address; 4 <= len <= 47
    // Sample min: [::]
    // Sample max: [ABCD:ABCD:ABCD:ABCD:ABCD:ABCD:123.123.123.123]
    uint32 hname_len = strlen(hname);
    if( hname_len < 4 || hname_len > 47 || ']' != hname[hname_len-1] )
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_dns_query - Invalid IPv6 address");
      clear_request(DS_HTTP_ERROR_DNS);
      ds_appsrv_free(hint);
      return false;
    }

    // Don't copy brackets in beginning and end
    memscpy(hostname, DS_HTTP_HOSTNAME_MAX_LEN, hname+1, hname_len-2);
  }
  else
  {
    // IPv4 or domain name
    memscpy(hostname, DS_HTTP_HOSTNAME_MAX_LEN, hname, DS_HTTP_HOSTNAME_MAX_LEN);
  }

  dns_query_handle = dss_dns_get_addrinfo(
                                          session_handle,
                                          hostname,
                                          NULL,
                                          hint,
                                          &err
                                          );
  ds_appsrv_free(hint);

  if (dns_query_handle == DSS_ERROR && DS_EWOULDBLOCK != err)
  {
    DATA_APPSRV_MSG2(MSG_LEGACY_ERROR, "ds_http_dns_query - dns request error %d %d", dns_query_handle, err);
    clear_request(DS_HTTP_ERROR_DNS);
    return false;
  }

  return true;
}



void ds_http_dns_query::add_request(ds_http_request* request)
{
  request_queue.push_back(request);
}



bool ds_http_dns_query::remove_request(const uint32 request_id)
{
  request_iterator request_it = request_queue.begin();
  while(request_queue.end() != request_it)
  {
    if(request_id == (*request_it)->request_id)
    {
      // No need to notify client
      ds_appsrv_free(*request_it);
      request_queue.erase(request_it);
      return true;
    }

    // else
    ++request_it;
  }

  return false;
}



void ds_http_dns_query::remove_requests_for_session(const uint32 session_id)
{
  request_iterator request_it = request_queue.begin();
  while(request_queue.end() != request_it)
  {
    if(session_id == (*request_it)->session_id)
    {
      // No need to notify client
      ds_appsrv_free(*request_it);
      request_it = request_queue.erase(request_it);
    }
    else
    {
      ++request_it;
    }
  }
}



bool ds_http_dns_query::append_chunk_to_request(
                                                const uint32                   request_id,
                                                const ds_http_chunked_content& chunked_content
                                                )
{
  for(request_iterator request_it = request_queue.begin(); request_queue.end() != request_it; ++request_it)
  {
    if(request_id == (*request_it)->request_id)
    {
      (*request_it)->append_chunked_content(chunked_content);
      return true;
    }
  }
  return false;
}



void ds_http_dns_query::clear_request(const sint15 error)
{
  while (!request_queue.empty())
  {
    request_queue.front()->notify_error(error);
    ds_appsrv_free(request_queue.front());
    request_queue.pop_front();
  }
}



void ds_http_dns_query::net_down_event()
{
  clear_request(DS_HTTP_ERROR_NETWORK_DOWN);
}


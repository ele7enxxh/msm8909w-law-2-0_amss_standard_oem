/*==============================================================================

                              ds_http_dns.h

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
07/21/14    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_HTTP_DNS_H
#define DS_HTTP_DNS_H

#include "ds_ASList.h"

extern "C"
{
  #include "dssdns.h"
}

class  ds_http_request;
struct ds_http_chunked_content;


class ds_http_dns_query
{
public:
  /*============================================================================
  FUNCTION ds_http_dns_query

  DESCRIPTION
    Constructor

  PARAMETERS
    [In] request - New request, must not be NULL

  RETURN VALUE
    None

  DEPENDENCIES
    None
  ============================================================================*/
  ds_http_dns_query(ds_http_request* request);


  /*============================================================================
  FUNCTION ~ds_http_dns_query

  DESCRIPTION
    Destructor. If it holds any requests, it will notify a DNS error and free
    the requests as well

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None
  ============================================================================*/
  ~ds_http_dns_query();


  /*============================================================================
  FUNCTION start_query

  DESCRIPTION
    Starts the DNS query

  PARAMETERS
    [In] session_handle - DNS session handle

  RETURN VALUE
    None

  DEPENDENCIES
    None
  ============================================================================*/
  bool start_query(const dss_dns_session_mgr_handle_type session_handle, int ip_family);


  /*============================================================================
  FUNCTION start_query

  DESCRIPTION
    Add another request to its internal queue

  PARAMETERS
    [In] request - Request to add to the queue

  RETURN VALUE
    None

  DEPENDENCIES
    None
  ============================================================================*/
  void add_request(ds_http_request* request);

  bool remove_request(const uint32 request_id);
  void remove_requests_for_session(const uint32 session_id);

  bool append_chunk_to_request(
                               const uint32                   request_id,
                               const ds_http_chunked_content& chunk_content
                               );

  /*============================================================================
  FUNCTION start_query

  DESCRIPTION
    Notifies the given error to all the requests it holds and then clear/free them

  PARAMETERS
    [In] error - The error to notify

  RETURN VALUE
    None

  DEPENDENCIES
    None
  ============================================================================*/
  void clear_request(const sint15 error);


  /*============================================================================
  FUNCTION net_down_event

  DESCRIPTION
    Notify network is down to all the requests it holds and then clear/free them

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None
  ============================================================================*/
  void net_down_event();

  /* Public Members */
private:
  dss_dns_query_handle_type dns_query_handle;
  ASList<ds_http_request*>  request_queue;

  friend class ds_http_dsnet;
};


#endif /* DS_HTTP_DNS_H */

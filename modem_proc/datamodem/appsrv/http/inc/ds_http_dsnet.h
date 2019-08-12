/*==============================================================================

                              ds_http_dsnet.h

GENERAL DESCRIPTION
  DSNet object manager. One should be created for each pdp id

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/22/15    ml     IPv6 support
09/11/15    ml     DSDS support
04/20/15    ml     Session deregistration
03/10/15    ml     IWLAN / WLAN LB Support
02/19/15    ml     Redirect requests to a new socket if current socket is in close wait status.
12/02/14    ml     Added features for more detailed authentication requests.
07/21/14    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_HTTP_DSNET_H
#define DS_HTTP_DSNET_H

#include "ds_http_types.h"
#include "ds_http_dsnet_iface_info.h"
#include "ds_http_uri.h"

#include "ds_ASList.h"
#include "ds_ASQueue.h"
#include "ds_ASString.h"

extern "C"
{
  #include "dssdns.h"
}
#include "dssocket.h"


class  ds_http_request;
class  ds_http_socket;
class  ds_http_dns_query;
struct ds_http_chunked_content;


static const uint32 DS_HTTP_DNS_CACHE_MAX_SIZE = 20;


class ds_http_dsnet
{
public:
  /* Public Functions */
  ds_http_dsnet(const ds_http_dsnet_iface_info& iface);
  ~ds_http_dsnet();

  /* Status info */
  bool net_down() const;       /* Returns true if dsnet is not connected */
  bool dsnet_inactive() const; /* Returns true if no activity is pending in this object */
  bool get_sys_status() const; /* Returns true if system is available for the iface */

  /* Request management */
  void add_request(ds_http_request* request);
  void append_chunk_to_request(
                               const uint32                   request_id,
                               const ds_http_chunked_content& chunk_content
                               );
  void remove_request(const uint32 request_id);
  void remove_requests_for_session(const uint32 session_id);


  /* Event handlers */
  void handle_dsnet_event(const dss_iface_id_type iface, const sint15 error);
  void handle_socket_event(const sint15 sockfd, const sint15 event_mask);
  void handle_dns_event(
                        const dss_dns_query_handle_type query_id,
                        const uint16 num_records,
                        const int16 dss_errno
                        );
  void handle_timeout_event(const uint16 socket_id);

  /* Resolve handlers */
  void resolve_auth(const uint32 request_id, const ds_http_auth_info_s_type* auth_info);
  void resolve_ssl_suspension(const uint32 request_id, const bool cont);


private:
  /* Private Functions */
  /* General */
  void handle_request(ds_http_request* request);
  void dsnet_down_clear_all();

  /* Dsnet */
  bool ds_http_init_dsnet();
  bool ds_http_connect_dsnet();


  /* DNS */
  bool ds_http_open_dns_session();
  void ds_http_close_dns_session();

  void ds_http_create_dns_query(ds_http_request* request);
  void ds_http_add_request_to_dns(ds_http_request* request);

  void ds_http_cache_dns_info(const char* hostname, const ds_http_ip_addr_info& addr_info);
  bool ds_http_dns_find_ip(const char* hostname, ds_http_ip_addr_info& addr_info) const;


  /* sockets */
  void ds_http_add_request_to_socket(ds_http_request* request, const ds_http_ip_addr_info& addr_info);
  void check_and_handle_socket_status(ds_http_socket* http_socket);

  ds_http_socket* create_http_socket(ds_http_request* request, const ds_http_ip_addr_info& addr_info);
  ds_http_socket* find_http_socket_by_sockfd(const sint15 sockfd) const;
  ds_http_socket* find_http_socket_by_socket_id(const uint16 socket_id) const;
  ds_http_socket* find_http_socket_by_request_id(const uint32 request_id) const;
  void remove_http_socket(const uint32 socket_id);
  void remove_http_socket(ds_http_socket* http_socket);



/* Internal structs */
private:
  enum ds_http_dsnet_status
  {
    DS_HTTP_NET_STATUS_NOHANDLE,
    DS_HTTP_NET_STATUS_DOWN_IN_PROGRESS,
    DS_HTTP_NET_STATUS_DOWN,
    DS_HTTP_NET_STATUS_UP_IN_PROGRESS,
    DS_HTTP_NET_STATUS_UP
  };

  struct ds_http_dns_cache
  {
    ASString hostname;
    ds_http_ip_addr_info  ip_addr_info;
  };


// Class members
public:
  /* Search Keys */
  const ds_http_dsnet_iface_info    iface_info;
  sint15                            dsnet_handle_id;
  dss_dns_session_mgr_handle_type   dns_session_handle;

private:
  /* DSNet */
  dss_iface_id_type          iface_id;
  ds_http_dsnet_status       dsnet_status;

  /* DNS */
  uint32                     cache_counter;
  ds_http_dns_cache          dns_cache[DS_HTTP_DNS_CACHE_MAX_SIZE];
  ds_http_dns_query*         active_query;
  ASList<ds_http_dns_query*> dns_wait_queue_list;

  /* Socket */
  ASList<ds_http_socket*>    socket_list;
  ASList<ds_http_request*>   dsnet_wait_queue; // Waiting for net up

  /* Number of sessions referencing this dsnet object. It should only change by
      ds_http_session_info object */
  uint32       reference_counter;

  /* IP type */
  int          ip_family;

  friend class ds_http_session_info;
};





#endif /* DS_HTTP_DSNET_H */

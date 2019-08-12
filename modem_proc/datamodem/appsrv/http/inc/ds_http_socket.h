/*==============================================================================

                              ds_http_socket.h

GENERAL DESCRIPTION
  HTTP socket manager

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/20/16    ml     Multiple SSL version support
11/02/15    ml     Cancel request
11/02/15    ml     HTTP Post chunked support
09/22/15    ml     IPv6 support
02/19/15    ml     Redirect requests to a new socket if current socket is in close wait status.
01/02/15    ml     Added timeout to handle unresolved block status.
12/02/14    ml     Added features for more detailed authentication requests.
07/21/14    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_HTTP_SOCKET_H
#define DS_HTTP_SOCKET_H

#include "ds_http_types.h"
#include "ds_ssl_def.h"
#include "ds_http_uri.h"

#include "ds_ASList.h"

#include "dssocket.h"
#include "dssocket_defs.h"
#include "rex.h"

/* Forward Declaration */
class  ds_http_request;
class  ds_http_response;
struct ds_http_chunked_content;


class ds_http_socket
{
public:
  ds_http_socket(
                 ds_http_request*            request,
                 const sint15                dsnet_handle,
                 const dss_iface_id_type     iface,
                 const ds_http_ip_addr_info& addr_info,
                 sint15*                     err
                 );
  ds_http_socket(
                 ds_http_socket* prev_socket,
                 sint15*         err
                 );
  virtual ~ds_http_socket();

  /* Returns true if waiting for close event */
  bool socket_closing() const;
  /* Returns true if socket is closed and there are no requests remaining */
  bool socket_closed() const;
  /* Returns true if the socket has requests to handle */
  bool is_active() const;
  bool is_ssl_protocol() const;
  const ds_http_ip_addr_info& get_ip_addr_info() const;
  sint15 get_nethandle() const;
  uint32 get_port() const;
  uint32 get_active_request_id() const;

  void clear_connection_info();

  virtual void add_request(ds_http_request* request);
  bool remove_request(const uint32 request_id);
  void remove_requests_for_session(const uint32 session_id);

  // chunked transfer
  bool append_chunk_to_request(
                               const uint32                   request_id,
                               const ds_http_chunked_content& chunked_content
                               );

  void resolve_auth(const ds_http_auth_info_s_type* auth_info);
  virtual void resolve_ssl_suspension(const bool cont);

  // event handlers
  virtual void timeout_event();
  void net_down_event();
  virtual void socket_event(const uint32 event_mask);


protected:
  // Utility Functions
  void socket_init();
  virtual void   init_values();
  virtual sint15 http_write(sint15 sockfd, const uint8* buffer, uint32 buffer_size, sint15* err);
  virtual sint15 http_read(sint15 sockfd, uint8* buffer, uint32 buffer_size, sint15* err);
  virtual bool   check_socket_error(sint15 retval, sint15 sock_err, sint31 event_mask, sint15 internal_err);

  // Flow
  void http_create_socket();
  void http_connect();
  virtual void socket_connected();
  void http_ssl_connect();
  void http_send();
  void http_receive();
  void http_connected();
  void http_close();
  virtual bool close_socket();
  void http_close_complete();

  // Http status handlers
  void handle_response_status();
  void status_success();
  void status_redirect();
  void status_authenticate(uint32 status);
  void update_permanent_redirect(const ds_http_request_uri& redirect_uri);

  // Error handlers
  void socket_error(sint15 err);
  void status_error(sint15 err, sint15 http_status);
  void notify_error(sint15 error, sint15 http_status);



  /*============================================================================
                            Internal structs & enums
  ============================================================================*/
protected:
  enum socket_status_e_type
  {
    SOCKET_STATUS_SOCKET_CONNECT,
    SOCKET_STATUS_SSL_CONNECT, // Used by SSL Only
    SOCKET_STATUS_SEND,
    SOCKET_STATUS_SEND_WAIT, // For chunked-transfer
    SOCKET_STATUS_RECEIVE,
    SOCKET_STATUS_AUTH,
    SOCKET_STATUS_CONNECTED,
    SOCKET_STATUS_CLOSE,
    SOCKET_STATUS_CLOSED
  };

  friend class ds_http_dsnet;



  /*============================================================================
                                Class members
  ============================================================================*/
public:
  // Keys
  const uint16 socket_id;
  sint15       sockfd;


protected:
  uint16                    port;
  socket_status_e_type      socket_status;

  ds_http_request*          active_request;
  ds_http_response*         active_response;
  rex_timer_type            timeout_timer;
  uint32                    timeout_length;

private:
  sint15                    dsnet_handle_id;
  dss_iface_id_type         iface_id;
  ds_http_ip_addr_info      ip_addr_info;
  bool                      persistent;
  bool                      filter_set;
  bool                      waiting_ds_close_event;

  ASList<ds_http_request*>  request_queue;
};


// used by socket timeout timer, defined at dsnet
void ds_http_timeout_timer_cb(uint32 http_socket_id);

#endif /* DS_HTTP_SOCKET_H */

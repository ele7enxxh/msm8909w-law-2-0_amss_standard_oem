/*==============================================================================

                              ds_ssl_socket.h

GENERAL DESCRIPTION
  HTTP ssl socket manager

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/20/16    ml     Multiple SSL version support
09/22/15    ml     IPv6 support
02/19/15    ml     Redirect requests to a new socket if current socket is in close wait status.
01/02/15    ml     Added timeout to handle unresolved block status.
07/21/14    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_SSL_SOCKET_H
#define DS_SSL_SOCKET_H

#include "ds_http_socket.h"



class ds_ssl_socket : public ds_http_socket
{
public:
  ds_ssl_socket(
                ds_http_request*            request,
                const sint15                dsnet_handle,
                const dss_iface_id_type     iface,
                const ds_http_ip_addr_info& addr_info,
                sint15*                     err
                );
  ds_ssl_socket(
                ds_http_socket* prev_socket,
                sint15*         err
                );
  virtual ~ds_ssl_socket();

  sint15 get_ssl_session_id() const;
  virtual void add_request(ds_http_request* request);
  virtual void timeout_event();
  virtual void socket_event(const uint32 event_mask);
  virtual void resolve_ssl_suspension(const bool cont);
  void ssl_event(const ds_ssl_status_enum_type status, void* status_info);


protected:
  // Utility
  virtual void   init_values();
  virtual sint15 http_write(const sint15 sockfd, const uint8* buffer, const uint32 buffer_size, sint15* err);
  virtual sint15 http_read(const sint15 sockfd, uint8* buffer, const uint32 buffer_size, sint15* err);
  virtual bool check_socket_error(
                                  const sint15 retval,
                                  const sint15 sock_err,
                                  const sint31 event_mask,
                                  const sint15 internal_err
                                  );

  // Flow
  virtual void socket_connected();
  virtual bool close_socket();
  void         ssl_connect();
  void         ssl_close();


private:
  enum ssl_status_e_type
  {
    SSL_STATUS_DISABLED,
    SSL_STATUS_ENABLED,
    SSL_STATUS_CONNECTED,
    SSL_STATUS_DISCONNECT_WAIT
  };


  sint15                     ssl_session_id;
  ssl_status_e_type          ssl_status;
  ds_http_ssl_version_e_type ssl_version;
};


void ds_http_init_ssl_session();

#endif /* DS_SSL_SOCKET_H */

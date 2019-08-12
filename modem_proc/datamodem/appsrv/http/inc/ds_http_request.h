/*==============================================================================

                              ds_http_request.h

GENERAL DESCRIPTION
  HTTP request information object

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/20/16    ml     Multiple SSL version support
11/02/15    ml     Permanent redirect
11/02/15    ml     Cancel request
11/02/15    ml     HTTP Post chunked support
09/11/15    ml     DSDS support
06/11/15    ml     Cookie support
04/16/15    ml     Response authentication support.
12/02/14    ml     Added features for more detailed authentication requests.
07/21/14    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_HTTP_REQUEST_H
#define DS_HTTP_REQUEST_H

#include "ds_http_types.h"
#include "ds_http_uri.h"
#include "ds_http_auth_generator.h"
#include "ds_ASString.h"
#include "ds_ASBuffer.h"


struct ds_http_chunked_content
{
  bool     is_last_chunk;
  ASBuffer chunked_content;

  ds_http_chunked_content(const ds_http_chunked_content_info_s_type* chunked_content_info);
};


class ds_http_request
{
public:
  ds_http_request(
                  const uint32                 sid,
                  const uint32                 rid,
                  const char*                  uri_str,
                  const ds_http_header_s_type* header,
                  uint16                       reserved_src_port,
                  bool                         is_chunked,
                  sint15*                      request_errno
                  );
  virtual ~ds_http_request() = 0;

  const ds_http_request_uri& get_uri_info() const
  { return uri_info; }

  /*============================================================================
  FUNCTION ds_http_request::get_hostname

  PARAMETERS
    None

  RETURN VALUE
    The hostname of the request

  DEPENDENCIES
    None
  ============================================================================*/
  const char* get_hostname() const;


  /*============================================================================
  FUNCTION ds_http_request::get_port

  PARAMETERS
    None

  RETURN VALUE
    The port to connect.

  DEPENDENCIES
    None
  ============================================================================*/
  sint15 get_port() const;


  /*============================================================================
  FUNCTION ds_http_request::is_ssl_protocol

  PARAMETERS
    None

  RETURN VALUE
    true if request needs SSL protocol (HTTPS), else false.

  DEPENDENCIES
    None
  ============================================================================*/
  bool is_ssl_protocol() const;


  /*============================================================================
  FUNCTION ds_http_request::is_cancelled

  PARAMETERS
    None

  RETURN VALUE
    true if request has been cancelled by the client, else false

  DEPENDENCIES
    None
  ============================================================================*/
  bool is_cancelled() const;


  /*============================================================================
  FUNCTION ds_http_request::get_send_buffer_content

  PARAMETERS
    None

  RETURN VALUE
    A pointer to the buffer that needs to be sent to the server.

  DEPENDENCIES
    None
  ============================================================================*/
  const uint8* get_send_buffer_content() const;


  /*============================================================================
  FUNCTION ds_http_request::get_send_buffer_size

  PARAMETERS
    None

  RETURN VALUE
    Size of the buffer that needs to be sent to the server.

  DEPENDENCIES
    None
  ============================================================================*/
  uint32 get_send_buffer_size() const;


  /*============================================================================
  FUNCTION ds_http_request::reset_send_status

  DESCRIPTION
    Resets the current request send status.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None
  ============================================================================*/
  void reset_send_status();


  /*============================================================================
  FUNCTION ds_http_request::update_send_status

  DESCRIPTION
    Updates the send status to the next status based on bytes sent.

  PARAMETERS
    [In] written - Size of bytes sent to the server.

  RETURN VALUE
    None

  DEPENDENCIES
    None
  ============================================================================*/
  void update_send_status(const uint32 written);


  /*============================================================================
  FUNCTION ds_http_request::send_complete

  DESCRIPTION
    Check if the HTTP request send status is complete.

  PARAMETERS
    None

  RETURN VALUE
    True if send status is complete, otherwise false.

  DEPENDENCIES
    None
  ============================================================================*/
  bool send_complete() const;


  /*============================================================================
  FUNCTION ds_http_request::get_auth_info

  DESCRIPTION
    Fills auth_data with the decoded authentication response header information.

  PARAMETERS
    [Out] auth_data - auth info to be filled.

  RETURN VALUE
    None

  DEPENDENCIES
    decode_auth_header must have been called and successful.
  ============================================================================*/
  void get_auth_info(ds_http_block_auth_info_type* auth_data) const;


  const ds_http_credential_s_type* get_credential() const;
  const ds_http_auth_generator* get_request_auth_info() const;
  const char* get_uri_path() const;

  /*============================================================================
  FUNCTION ds_http_request::create_auth_header_str

  DESCRIPTION
    Add credentials to the request buffer.

  PARAMETERS
    [In] credential - The credential provided by the client for the request/realm

  RETURN VALUE
    True if authentication header creation is successful, otherwise false.

  DEPENDENCIES
    None
  ============================================================================*/
  bool create_auth_header_str(const ds_http_auth_info_s_type* client_auth_info);
  void create_cookie_header_str(
                                const ds_http_cookie_name_s_type cookie[],
                                uint16                           num_cookies
                                );


  /*============================================================================
  FUNCTION ds_http_request::decode_auth_header

  DESCRIPTION
    Decode the authentication header given by HTTP 401/407 status.

  PARAMETERS
    [In] auth_str - Authentication header string

  RETURN VALUE
    True if auth header decoding is successful, otherwise false.

  DEPENDENCIES
    None
  ============================================================================*/
  bool decode_auth_header(const char* auth_str, uint32 http_status);


  /*============================================================================
  FUNCTION ds_http_request::update_redirect

  DESCRIPTION
    Updates the request URI.

  PARAMETERS
    [In]  redirect_uri - The URI to redirect to
    [Out] err          - Error status to notify

  RETURN VALUE
    False if URI is invalid or redirect counter reaches max allowed,
    otherwise will return true

  DEPENDENCIES
    None
  ============================================================================*/
  bool update_redirect(const ds_http_request_uri& redirect_uri);


  /*============================================================================
  FUNCTION ds_http_request::notify_error

  DESCRIPTION
    Notifies the client that the request failed.

  PARAMETERS
    [In] http_error  - HTTP error status
    [In] http_status - HTTP response status

  RETURN VALUE
    None

  DEPENDENCIES
    None
  ============================================================================*/
  void notify_error(const sint15 http_error) const;

  void cancel_request();


  // chunked transfer
  void append_chunked_content(const ds_http_chunked_content& chunked_content);
  bool is_chunked_transfer() const;
  bool is_waiting_chunk() const;
  bool chunked_transfer_complete() const;

protected:
  /*============================================================================
  FUNCTION ds_http_request::create_request_str

  DESCRIPTION
    Derived functions must override this function and call create_request_str(const char*)

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None
  ============================================================================*/
  virtual void create_request_str() = 0;


  virtual const char* get_request_method_str() = 0;


  /*============================================================================
  FUNCTION ds_http_request::create_request_str

  DESCRIPTION
    Creates the base request buffer.

  PARAMETERS
    [In] request_method - The request method (e.g. Get, Post, Put, etc.)

  RETURN VALUE
    None

  DEPENDENCIES
    None
  ============================================================================*/
  void create_request_str(const char* request_method);


  /*============================================================================
  FUNCTION ds_http_request::append_header

  DESCRIPTION
    Adds the header to the current header request buffer.

  PARAMETERS
    [In] header_name  - Name of the header to add
    [In] header_value - Value of the header to add

  RETURN VALUE
    None

  DEPENDENCIES
    None
  ============================================================================*/
  void append_header(const char* header_name, const char* header_value);


  /*============================================================================
  FUNCTION ds_http_request::append_header

  DESCRIPTION
    Adds the header to the current header request buffer.

  PARAMETERS
    [In] content_data - Content buffer to send
    [In] content_size - Size of the content buffer

  RETURN VALUE
    None

  DEPENDENCIES
    None
  ============================================================================*/
  void append_content(const uint8* content_data, const uint32 content_size);


  /*============================================================================
  FUNCTION ds_http_request::check_buffer_error

  DESCRIPTION
    Check if the buffer has any errors.

  PARAMETERS
    None

  RETURN VALUE
    True if the buffer has an error status, else false

  DEPENDENCIES
    None
  ============================================================================*/
  bool check_buffer_error() const;


private:
  void create_header_str(const ds_http_header_s_type* header_info);
  bool add_cust_auth_param(const ds_http_auth_info_s_type* client_auth_info);

  void notify_ssl_alert(uint8 alert_level, uint32 alert_mask);



  /*============================================================================
                            Internal structs & enums
  ============================================================================*/
private:
  /* Request send status */
  enum ds_http_send_status
  {
    REQUEST_SEND_REQUEST,
    REQUEST_SEND_HEADER,
    REQUEST_SEND_AUTH_HEADER,
    REQUEST_SEND_COOKIE_HEADER,
    REQUEST_SEND_HEADER_DELIM,
    REQUEST_SEND_CONTENT,
    REQUEST_SEND_CONTENT_WAIT,
    REQUEST_SEND_COMPLETE
  };



  /*============================================================================
                                Class members
  ============================================================================*/
public:
  const uint32 session_id;
  const uint32 request_id;
  const uint16 reserved_src_port;
  const bool   chunked_transfer;

private:
  ds_http_request_uri       uri_info;
  ds_http_send_status       send_status;
  ds_http_auth_generator    auth_generator;
  ds_http_credential_s_type credential;
  ds_http_ssl_config_s_type ssl_config_info;

  // Data in header_str and content must be initialized in constructor and
  // should never change after construction
  ASBuffer header_str;
  ASBuffer content;

  // Could change based on http response status (redirect, auth)
  ASBuffer request_str; // contains method, path, hostname, http ver.
  ASBuffer auth_header_str;
  ASBuffer cookie_header_str;

  uint16   redirect_counter;
  uint32   bytes_written;
  bool     cancelled;
  bool     last_chunk_received;

  friend class ds_ssl_socket; // for ssl related data/functions
};


#endif /* DS_HTTP_REQUEST_H */

/*==============================================================================

                              ds_http_types.h

GENERAL DESCRIPTION
  Type declarations required for ds_http users

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/20/16    ml     Session option to handle response size > 100k
02/20/16    ml     Multiple SSL version support
11/02/15    ml     HTTP Post chunked support
09/11/15    ml     DSDS support
06/11/15    ml     Cookie support
04/10/15    ml     Iface any support
03/10/15    ml     IWLAN / WLAN LB Support
12/02/14    ml     Added features for more detailed authentication requests.
07/21/14    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_HTTP_TYPES_H
#define DS_HTTP_TYPES_H

#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "time_jul.h"

#ifdef __cplusplus
}
#endif

/*==============================================================================
                               DS HTTP Error Codes
==============================================================================*/
#define DS_HTTP_ERROR 0xFFFFFFFF /* uint32 max value */

/*============================ sint15 error codes ============================*/
/*======================= Create session/request error =======================*/
/* No Errors */
#define DS_HTTP_ERROR_NONE                 0
/* Invalid session given */
#define DS_HTTP_ERROR_INVALID_SESSION      -1
/* Network down */
#define DS_HTTP_ERROR_NETDOWN              -2
/* URI not provided in parameter */
#define DS_HTTP_ERROR_NO_URI_INFO          -3
/* Invalid URI format */
#define DS_HTTP_ERROR_INVALID_URI          -4
/* Unsupported HTTP method */
#define DS_HTTP_ERROR_UNSUPPORTED_METHOD   -5
/* Memory allocation failure */
#define DS_HTTP_ERROR_MEMALLOC             -6
/* Session container is full */
#define DS_HTTP_ERROR_MAX_SESSIONS         -7
/* Invalid/missing required value in request parameter */
#define DS_HTTP_ERROR_INVALID_PARAM        -8


/*========================= Error codes for callback =========================*/
/*=============================== Notification ===============================*/
/* DS HTTP is ready to handle requests */
#define DS_HTTP_READY       0
/* HTTP request successful */
#define DS_HTTP_STATUS_OK   200

/*  */
#define DS_HTTP_NOTIFY_PARTIAL_BODY 1



/*============================== General errors ==============================*/
/* Error in dsnet */
#define DS_HTTP_ERROR_DSNET             -200
/* Error in DNS query */
#define DS_HTTP_ERROR_DNS               -202
/* Error in ds_socket */
#define DS_HTTP_ERROR_SOCKET            -203
/* Error in SSL */
#define DS_HTTP_ERROR_SSL               -204
/* Error in response header */
#define DS_HTTP_ERROR_HTTP_HEADER       -205
/* HTTP Status Error */
#define DS_HTTP_ERROR_HTTP_STATUS       -206
/* Network is down */
#define DS_HTTP_ERROR_NETWORK_DOWN      -207
/* HTTP response authentication mismatch */
#define DS_HTTP_ERROR_RSPAUTH_MISMATCH  -208
/* HTTP timeout (keep-alive timeout or no response for block cb) */
#define DS_HTTP_ERROR_TIMEOUT           -209
/*  */
#define DS_HTTP_ERROR_CONFIG            -210



#define DS_HTTP_HEADER_NAME_MAX_LEN   32
#define DS_HTTP_HEADER_VALUE_MAX_LEN  256
#define DS_HTTP_USERNAME_MAX_LEN      64
#define DS_HTTP_PASSWORD_MAX_LEN      32
#define DS_HTTP_CUST_HEADER_MAX_SIZE  10
#define DS_HTTP_CUST_PARAM_MAX_SIZE   5

#define DS_HTTP_COOKIE_NAME_MAX_LEN  32
#define DS_HTTP_COOKIE_VALUE_MAX_LEN 256
#define DS_HTTP_COOKIE_MAX           20

/* 100kb */
#define DS_HTTP_CONTENT_MAX_SIZE      102400

/* SSL alert notification mask */
#define DS_HTTP_SSL_ALERT_WARNING    0x01
#define DS_HTTP_SSL_ALERT_SUSPENSION 0x02
#define DS_HTTP_SSL_ALERT_FATAL      0x04

/* For SSL continue/abort block events */
#define DS_HTTP_SSL_CONTINUE TRUE
#define DS_HTTP_SSL_ABORT    FALSE



/*==============================================================================
                            DS HTTP Structures
==============================================================================*/
/* Iface type to connect */
typedef enum
{
  DS_HTTP_IFACE_3GPP = 0,
  DS_HTTP_IFACE_IWLAN,
  DS_HTTP_IFACE_WLAN_LB,
  DS_HTTP_IFACE_ANY,
  DS_HTTP_IFACE_MAX
} ds_http_iface_e_type;



/* Subscription ID for iface */
typedef enum
{
  DS_HTTP_DEFAULT_SUBS   = 0x0000, /* Default Data Subscription */
  DS_HTTP_PRIMARY_SUBS   = 0x0001, /* Primary */
  DS_HTTP_SECONDARY_SUBS = 0x0002, /* Secondary */
  DS_HTTP_TERTIARY_SUBS  = 0x0003, /* Tertiary */
  DS_HTTP_SUBS_MAX
} ds_http_subscription_e_type;



/* Iface info to use for dsnet session */
typedef struct
{
  ds_http_iface_e_type        iface_type;
  uint16                      profile_id; // Ignored if WLAN_LB
  ds_http_subscription_e_type subs_id;
} ds_http_iface_info_s_type;



/* SSL version to use for HTTPS */
/* Note: TLS 1.3 not supported yet; placeholder for future. Will fall back to default version if specified. */
typedef enum
{
  DS_HTTP_SSL_VER_DEFAULT = 0, /* Current default is DS_HTTP_SSL_VER_TLS11 */
  DS_HTTP_SSL_VER_10,
  DS_HTTP_SSL_VER_20,
  DS_HTTP_SSL_VER_30,
  DS_HTTP_SSL_VER_TLS10,
  DS_HTTP_SSL_VER_TLS11,
  DS_HTTP_SSL_VER_TLS12,
  DS_HTTP_SSL_VER_TLS13
} ds_http_ssl_version_e_type;



typedef struct
{
  uint8  alert_level; // Alert level (warning, suspension, fatal)
  uint32 alert_mask;  // Alert mask given from SSL callback
} ds_http_ssl_alert_info_s_type;


/* Callback function for SSL alert notifications */
typedef void (*ds_http_ssl_alert_cb_fcn)(
                                         uint32                         session_id,
                                         uint32                         request_id,
                                         ds_http_ssl_alert_info_s_type* ssl_alert_info
                                         );


/* SSL settings */
typedef struct
{
  ds_http_ssl_version_e_type ssl_version;
  uint8                      ssl_alert_notification_mask;
  ds_http_ssl_alert_cb_fcn   ssl_alert_cb_fcn;
} ds_http_ssl_config_s_type;




/* Session configuration value */
typedef union
{
  ds_http_ssl_config_s_type ssl_config;
  boolean                   forward_oversize_response;
} ds_http_session_config_value_u_type;



/* List of session configurations */
typedef enum
{
  /* SSL configuration */
  DS_HTTP_SESSION_CONFIG_SSL = 0,
  /* Enable multiple cb if size exceeds 100k limit */
  DS_HTTP_SESSION_CONFIG_FORWARD_OVERSIZE_RESPONSE
} ds_http_session_config_option_e_type; // TODO: name???



/* Authentication information */
/* Note: Username MUST be a valid, non-empty string with no null character in the middle. */
typedef struct
{
  char   username[DS_HTTP_USERNAME_MAX_LEN+1];
  uint16 username_len;
  char   password[DS_HTTP_PASSWORD_MAX_LEN+1];
  uint16 password_len;
} ds_http_credential_s_type;



/* Cookie name-value pair */
typedef struct
{
  char name [DS_HTTP_COOKIE_NAME_MAX_LEN+1];
  char value[DS_HTTP_COOKIE_VALUE_MAX_LEN+1];
} ds_http_cookie_name_s_type;



/* Cookie information */
typedef struct
{
  /* Cookie name value pair */
  ds_http_cookie_name_s_type      name_value;
  /* Cookie attributes */
  const char*                     domain;
  const char*                     path;
  /* Cookie expiration. Ignore if expiration is unset. Both Expires and Max-Age
      attribute will be converted */
  boolean                         expiration_set;
  time_julian_type                expiration;
  /* Cookie flags */
  boolean                         http_only;
  boolean                         security;
} ds_http_cookie_info_s_type;



/* The content data to be sent */
typedef struct
{
  const uint8* content;
  uint32       content_size; // 0 <= size < DS_HTTP_CONTENT_MAX_SIZE
} ds_http_content_info_s_type;



/* Chunk content data to be sent. Request will complete only after the last
    chunk is received from client */
typedef struct
{
  ds_http_content_info_s_type content_info;
  boolean                     is_last_chunk;
} ds_http_chunked_content_info_s_type;



/* Custom headers that can be specified by the client */
typedef struct
{
  char name [DS_HTTP_HEADER_NAME_MAX_LEN+1];
  char value[DS_HTTP_HEADER_VALUE_MAX_LEN+1];
} ds_http_cust_header_s_type;


/* HTTP response header information */
typedef ds_http_cust_header_s_type ds_http_response_header;

/* Custom HTTP header param (e.g. name=value) */
typedef ds_http_cust_header_s_type ds_http_cust_param_s_type;


/* Info to use when creating HTTP Authentication response header */
typedef struct
{
  /* Credentials needed for authentication. */
  ds_http_credential_s_type  credential;
  /* Cookies to set for authentication resolve request. This will replace cookies set from initial request. Keep this field
      empty if cookies from initial request should be used. */
  /* !!!: These fields will be removed once cookie is supported internally */
  ds_http_cookie_name_s_type cookie[DS_HTTP_COOKIE_MAX];
  uint16                     num_cookies;
  /* Will append to Authentication response header as name=value. If value
     requires quotation marks, client is responsible to add them in each end. */
  ds_http_cust_param_s_type  cust_auth_param[DS_HTTP_CUST_PARAM_MAX_SIZE];
  /* Number of parameters specified. The size must not exceed DS_HTTP_CUST_PARAM_MAX_SIZE */
  uint16                     num_cust_auth_param;
} ds_http_auth_info_s_type;



/* Header information needed to create a request header. Please note that the
    size of this object will be large (6000+ bytes). */
typedef struct
{
  /* Header information to add to the request. */
  char                       accept          [DS_HTTP_HEADER_VALUE_MAX_LEN+1];
  char                       accept_charset  [DS_HTTP_HEADER_VALUE_MAX_LEN+1];
  char                       accept_encoding [DS_HTTP_HEADER_VALUE_MAX_LEN+1];
  char                       accept_language [DS_HTTP_HEADER_VALUE_MAX_LEN+1];
  char                       user_agent      [DS_HTTP_HEADER_VALUE_MAX_LEN+1];
  char                       ua_profile      [DS_HTTP_HEADER_VALUE_MAX_LEN+1];
  char                       referer         [DS_HTTP_HEADER_VALUE_MAX_LEN+1];
  char                       authorization   [DS_HTTP_HEADER_VALUE_MAX_LEN+1];
  /* Cookies */
  ds_http_cookie_name_s_type cookie          [DS_HTTP_COOKIE_MAX];
  uint16                     num_cookies;
  /* File type that will be sent to the server. This field must not be empty for
      POST and PUT requests */
  char                       content_type    [DS_HTTP_HEADER_VALUE_MAX_LEN+1];
  /* Custom headers not included in the structure to add in the request */
  ds_http_cust_header_s_type cust_header_list[DS_HTTP_CUST_HEADER_MAX_SIZE];
  /* Number of custom headers specified. The size must not exceed DS_HTTP_CUST_HEADER_MAX_SIZE */
  uint16                     num_cust_headers;
} ds_http_header_s_type;


// ***Note***: Placeholder; might be needed for PUT/DELETE request
typedef struct
{
  char if_match            [DS_HTTP_HEADER_VALUE_MAX_LEN+1];
  char if_none_match       [DS_HTTP_HEADER_VALUE_MAX_LEN+1];
  char if_modified_since   [DS_HTTP_HEADER_VALUE_MAX_LEN+1];
  char if_unmodified_since [DS_HTTP_HEADER_VALUE_MAX_LEN+1];
  char if_range            [DS_HTTP_HEADER_VALUE_MAX_LEN+1];
} ds_http_header_condition_s_type;




/* Structs for HTTP block events */
/* HTTP block event types */
typedef enum
{
  DS_HTTP_BLOCK_AUTHENTICATION,
  DS_HTTP_BLOCK_SSL_CERTIFICATE_ALERT
} ds_http_block_event_type;


/* Server authenticaiton request information */
typedef struct
{
  const char*                 realm; /* The realm that the server requested authentication for */
  const char*                 nonce; /* The nonce given in the response header for authentication */
  const char*                 auth_response_header; /* WWW-Authenticate or Proxy-Authenticate response header content */
  /* Array set-cookie headers in response */
  ds_http_cookie_info_s_type* cookie_info;
  uint16                      num_cookies;
} ds_http_block_auth_info_type;



/* SSL alert information */
typedef enum
{
  DS_HTTP_SSL_NO_CERT            = 0x00000040,        /* Missing certificate*/
  DS_HTTP_SSL_BAD_CERT           = 0x00000080,        /* Bad certificate*/
  DS_HTTP_SSL_BAD_CERT_SIGN      = 0x00000100,        /* Bad certificate signature*/
  DS_HTTP_SSL_BAD_CERT_ISSUER    = 0x00000200,        /* Bad certificate issuer*/
  DS_HTTP_SSL_UNSUPPORTED_CERT   = 0x00000400,        /* Unsupported certificate*/
  DS_HTTP_SSL_CERT_REVOKED       = 0x00000800,        /* Certificate invalid (was revoked)*/
  DS_HTTP_SSL_CERT_EXPIRED       = 0x00001000,        /* Certificate invalid (expired)*/
  DS_HTTP_SSL_CERT_UNKNOWN       = 0x00002000,        /* certifiate invalid (unknown error)*/
  DS_HTTP_SSL_UNKNOWN_CA         = 0x00010000,        /* can not identify certificate authority*/
  DS_HTTP_SSL_HOST_MISMATCH      = 0x04000000,        /* Host mismatch (Common Name)*/
  DS_HTTP_SSL_CERT_INFO          = 0x08000000         /* Alert for viewing the certificate*/
} ds_http_ssl_cert_alert_e_type;


typedef union
{
  ds_http_block_auth_info_type  auth_event_info;
  ds_http_ssl_cert_alert_e_type ssl_event_info;
} ds_http_block_event_info_type;



typedef struct
{
  uint16                            http_status;  /* HTTP response status */
  const uint8*                      content;      /* HTTP response content */
  uint32                            content_size; /* Size of the response content */
  const ds_http_response_header*    header_info;  /* Response header information */
  uint32                            num_headers;  /* Number of response headers */
  const ds_http_cookie_info_s_type* cookie_info;  /* Cookie information */
  uint16                            num_cookies;  /* Number of cookies */
} ds_http_response_info_s_type;


/*==============================================================================
                          DS HTTP Callback Functions
==============================================================================*/
/*==============================================================================
  Callback function for request successful, request failed, and http api ready
  notifications.

  Parameter description:
    session_id    - Session ID of the request
    request_id    - Request ID of the request
    error         - Error status. Possible values are
                      DS_HTTP_READY
                      DS_HTTP_STATUS_OK
                      DS_HTTP_ERROR_DSNET
                      DS_HTTP_ERROR_DNS
                      DS_HTTP_ERROR_SOCKET
                      DS_HTTP_ERROR_SSL
                      DS_HTTP_ERROR_HTTP_HEADER
                      DS_HTTP_ERROR_HTTP_STATUS
                      DS_HTTP_ERROR_NETWORK_DOWN
                      DS_HTTP_ERROR_RSPAUTH_MISMATCH
    http_status   - HTTP response status; only valid if error status is
                      DS_HTTP_STATUS_OK
                      DS_HTTP_ERROR_HTTP_STATUS
    content       - HTTP response content
    content_size  - Size of the response content
    header_info   - Response header information
    num_headers   - Number of response headers
==============================================================================*/
typedef void (*ds_http_cb_fcn)(
                               uint32                         session_id,
                               uint32                         request_id,
                               sint15                         error,
                               uint16                         http_status,
                               const uint8*                   content,
                               uint32                         content_size,
                               const ds_http_response_header* header_info,
                               uint32                         num_headers
                               );


/*==============================================================================
  Callback function for request successful, request failed, and http api ready
  notifications.

  Parameter description:
    session_id    - Session ID of the request
    request_id    - Request ID of the request
    error         - Error status. Possible values are
                      DS_HTTP_READY
                      DS_HTTP_STATUS_OK
                      DS_HTTP_ERROR_DSNET
                      DS_HTTP_ERROR_DNS
                      DS_HTTP_ERROR_SOCKET
                      DS_HTTP_ERROR_SSL
                      DS_HTTP_ERROR_HTTP_HEADER
                      DS_HTTP_ERROR_HTTP_STATUS
                      DS_HTTP_ERROR_NETWORK_DOWN
                      DS_HTTP_ERROR_RSPAUTH_MISMATCH
    response_info - HTTP response information; only valid if error status is
                      DS_HTTP_STATUS_OK
                      DS_HTTP_ERROR_HTTP_STATUS
                    Otherwise value will be NULL
==============================================================================*/
typedef void (*ds_http_cb_fcn_ex)(
                                  uint32                              session_id,
                                  uint32                              request_id,
                                  sint15                              error,
                                  const ds_http_response_info_s_type* response_info
                                  );




/*==============================================================================
  Callback function to notify block events that need input from the client.
  If the client does not respond to the block event within a given timeframe,
  the request will fail and notify the registered ds_http_cb_fcn.
  The two types of blocks are :
    SSL Certificate - Client needs to update the API to either continue or abort
                      the request.
    Authentication  - Client needs to provide credentials for the given realm.
                      Providing NULL or empty credentials will fail the request.

  Parameter description:
    session_id  - Session ID of the request
    request_id  - Request ID of the request
    event       - Block event type
    event_info  - Detailed information for the block event
==============================================================================*/
typedef void (*ds_http_block_event_cb_fcn)(
                                           uint32                         session_id,
                                           uint32                         request_id,
                                           ds_http_block_event_type       event,
                                           ds_http_block_event_info_type* event_info
                                           );




#endif /* DS_HTTP_TYPES_H */

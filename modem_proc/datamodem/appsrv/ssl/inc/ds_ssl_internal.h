/******************************************************************************
  @file    ds_ssl_internal.h
  @brief   Internal header file for DS SSL

  DESCRIPTION
  Internal header file for DS SSL

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A
******************************************************************************/

/*=============================================================================
   Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/


/*=============================================================================
                        EDIT HISTORY FOR MODULE

  # $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/appsrv/ssl/inc/ds_ssl_internal.h#1 $
  # $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who                    what, where, why
--------   ---       ----------------------------------------------------------
5/20/14   youjunf                      Created
===========================================================================*/


#ifndef DS_SSL_INTERNAL_H
#define DS_SSL_INTERNAL_H

#include "datamodem_variation.h"

#ifndef TEST_FRAMEWORK
#include "secssl.h"
#include "ds_ssl_def.h"

typedef enum
{
  /** Unconnected State */
  DS_SSL_STATE_IDLE,
  /** TLS Session establishing */
  DS_SSL_STATE_OPENING,
  /** TLS Session established */
  DS_SSL_STATE_OPENED,
  /** TLS Session suspended */
  DS_SSL_STATE_SUSPENDED,
  /** initiated clossing */
  DS_SSL_STATE_CLOSING,
  /** TLS Session manually terminated */
  DS_SSL_STATE_CLOSED
} ds_ssl_state_type;


typedef enum
{
  DS_SSL_OPEN_IND,
  DS_SSL_CLOSE_IND,
  DS_SSL_CONTINUE_IND,
  DS_SSL_EVENT_IND,
  DS_SSL_REPORT_ERROR_IND,
  DS_SSL_REPORT_ALERT_IND
} ds_ssl_msg_id_enum_type;



typedef struct
{
  ds_ssl_cb_fcn             ssl_cb;
  void                      *user_data_ptr;
} ds_ssl_cb_fcn_type;




typedef struct
{
  sint15                   client_id;
  sint15                   sock_fd;
  sint15                   ssl_session_id;
  secssl_handle_type       sec_session_hdl;
  ds_ssl_config_info_type  ssl_config_info;
  ds_ssl_state_type        state;
  ds_ssl_cb_fcn            ds_ssl_status_notify_cb;
  void                    *user_data_ptr;
} ds_ssl_instance_type;



typedef enum
{
  OP_SUCCESS,
  OP_FAIL
} async_op_result_type;


typedef struct
{
  async_op_result_type      result;
  ds_ssl_instance_type      *ds_ssl_instance_ptr;
} ds_ssl_async_op_msg_type;


typedef struct
{
  secssl_ses_event_enum_type ssl_event;
  ds_ssl_instance_type      *ds_ssl_instance_ptr;
} ds_ssl_event_msg_type;


typedef struct
{
  secerrno_enum_type        secsslerrno;
  ds_ssl_instance_type      *ds_ssl_instance_ptr;
} ds_ssl_error_msg_type;



typedef struct
{
  uint8  ssl_alert_level;                  /*alert level*/
  uint32 ssl_alert_mask;                   /*alert mask*/
} ds_ssl_alert_content_type;


typedef struct
{
  ds_ssl_alert_content_type    alert_content;
  ds_ssl_instance_type         *ds_ssl_instance_ptr;
} ds_ssl_alert_msg_type;


typedef struct
{
  ds_ssl_msg_id_enum_type      ssl_msg_id;
  union
  {
    ds_ssl_async_op_msg_type  async_op_msg;
    ds_ssl_event_msg_type     event_msg;
    ds_ssl_error_msg_type     error_msg;
    ds_ssl_alert_msg_type     alert_msg;
  }msg_content;
} ds_ssl_msg_type;


#define DS_MAP_SSL_VERSION(version)                             \
  (((version) == DS_SSL_VER_DEFAULT) ? SECSSL_SSL_VER_UNKNOWN : \
   ((version) == DS_SSL_VER_10)      ? SECSSL_SSL_VER_10 :      \
   ((version) == DS_SSL_VER_20)      ? SECSSL_SSL_VER_20 :      \
   ((version) == DS_SSL_VER_30)      ? SECSSL_SSL_VER_30 :      \
   ((version) == DS_SSL_VER_TLS10)   ? SECSSL_SSL_VER_TLS10 :   \
   ((version) == DS_SSL_VER_TLS11)   ? SECSSL_SSL_VER_TLS11 :   \
   ((version) == DS_SSL_VER_TLS12)   ? SECSSL_SSL_VER_TLS12 : SECSSL_SSL_VER_UNKNOWN)





typedef struct
{
  sint15                         client_id;
  ds_ssl_cb_fcn_type             sslCallback;
} ds_ssl_client_type;


void ds_ssl_open_callback
(
  void                *user_data_ptr,
  secerrno_enum_type   secsslerrno
);

void ds_ssl_close_callback
(
  void                *user_data_ptr,
  secerrno_enum_type   secsslerrno
);


void ds_ssl_continue_callback
(
  void                 *user_data_ptr,
  secerrno_enum_type   secsslerrno
);

void ds_ssl_error_callback
(
  void*                user_data_ptr,
  secerrno_enum_type   secsslerrno
);



void ds_ssl_allert_callback
(
  void*                user_data_ptr,
  /*Type of alert being reported*/
  secssl_ses_alert_level_enum_type  alert_level,
  /*Bit mask containing the sum of all of the current alerts*/
  secssl_ses_alert_mask_enum_type   alert_mask
);


void ds_ssl_event_callback
(
  secssl_handle_type                sec_session_hdl,
  secssl_ses_event_enum_type        secsslevent,
  void                              *user_data_ptr
);




void ds_ssl_process_open_session_result(ds_ssl_msg_type *msg_ptr);
void ds_ssl_process_continue_session_result(ds_ssl_msg_type *msg_ptr);
void ds_ssl_process_close_session_result(ds_ssl_msg_type *msg_ptr);
void ds_ssl_process_event(ds_ssl_msg_type *msg_ptr);
void ds_ssl_process_error(ds_ssl_msg_type *msg_ptr);
void ds_ssl_process_alert(ds_ssl_msg_type *msg_ptr);

void ds_ssl_msg_post(ds_ssl_msg_id_enum_type msg_id, void *msg_content_ptr);

#endif /*TEST_FRAMEWORK*/

#endif /* DS_SSL_INTERNAL_H */



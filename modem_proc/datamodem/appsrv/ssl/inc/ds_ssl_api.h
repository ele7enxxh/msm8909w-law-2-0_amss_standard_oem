/******************************************************************************
  @file    ds_ssl_api.h
  @brief   ds ssl interface header file

  DESCRIPTION
  Definitions for DS SSL API functions

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A
******************************************************************************/

/*=============================================================================
   Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/



/*=============================================================================
                        EDIT HISTORY FOR MODULE

  # $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/appsrv/ssl/inc/ds_ssl_api.h#1 $
  # $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who                    what, where, why
--------   ---       ----------------------------------------------------------
5/20/14   youjunf                      Created
===========================================================================*/

#ifndef DS_SSL_API_H
#define DS_SSL_API_H

#include "ds_ssl_def.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*===========================================================================
FUNCTION     ds_ssl_register_status_cback

DESCRIPTION

  ds_ssl_status_cb  --callback function ptr for ds ssl to notify client of status

  user_data_ptr     --userdata used to invoke status cback to client, can be null

DEPENDENCIES  None

RETURN VALUE  client ID

SIDE EFFECTS  None
===========================================================================*/
sint15 ds_ssl_register_status_cback(
                                    ds_ssl_cb_fcn ds_ssl_status_cb,
                                    void          *user_data_ptr
                                    );




/*===========================================================================
FUNCTION     ds_ssl_session_open

DESCRIPTION
  Create and async open a ssl session.  upon successful connetion, calls the ssl callback function
  that the ssl status is DS_SSL_STATUS_CONNECTED.

  client_id:
  sock_fd:
  ss_config_info_ptr:

DEPENDENCIES
  An external socket is already connected

RETURN VALUE
  On successful operation, this function returns ssl session ID
  which is a sint15 value between 0x0000and 0x7FFF.

  On error, retuns:
  DS_SSL_INVALID_ARG
  DS_SSL_NOT_AVAILABLE
  DS_SSL_ERROR_UNKNOW



SIDE EFFECTS  None
===========================================================================*/
sint15 ds_ssl_session_open(
                           sint15 client_id,
                           sint15 sock_fd,
                           ds_ssl_config_info_type *ssl_config_info_ptr
                           );





/*===========================================================================
FUNCTION     ds_ssl_session_close

DESCRIPTION
  Async close a ssl session. upon the session is successfully colsed, calls the ssl callback function
  that the ssl status is DS_SSL_STATUS_DISCONNECTED.

  ssl_session_id:

DEPENDENCIES
  An external socket is already connected

RETURN VALUE
  On successful operation, this function returns DS_SSL_SUCCESS

  On error, retuns:
    DS_SSL_INVALID_ARG
    DS_SSL_NOT_AVAILABLE
    DS_SSL_ERROR_UNKNOW

SIDE EFFECTS  None
===========================================================================*/
sint15 ds_ssl_session_close(sint15 ssl_session_id);




/*===========================================================================

DESCRIPTION
  This function allow client to control the SSL session when get a suspend alert callback
  during handshake, client can choose to continue the session or abort the session

  ssl_session_id:
  session_control:

DEPENDENCIES
  A suspend alert callback is triggered

RETURN VALUE
  On successful operation, this function returns DS_SSL_SUCCESS

  On error, retuns:
    DS_SSL_INVALID_ARG
    DS_SSL_NOT_AVAILABLE
    DS_SSL_ERROR_UNKNOW

SIDE EFFECTS  None
===========================================================================*/
sint15 ds_ssl_session_user_control(sint15 ssl_session_id, ds_ssl_control_type session_control);






/*===========================================================================

DESCRIPTION
  This function writes the given byte array

  ssl_session_id  - vaild session id to an SSL session
  buffer          - Byte pointer to the data in network order
  nbytes          - Numbder of bytes to write

DEPENDENCIES
  The SSL session is connected

RETURN VALUE
  Total number of bytes written, which could be less than the number of bytes specified.

  possible error conditions are:
    DS_SSL_WOULDBLOCK
    DS_SSL_INVALID_ARG
    DS_SSL_NOT_AVAILABLE
    DS_SSL_ERROR_UNKNOW

SIDE EFFECTS  None
===========================================================================*/
sint15 ds_ssl_session_write(sint15 ssl_session_id, const uint8 *buffer, uint16 nbytes);







/*===========================================================================
FUNCTION     ds_ssl_session_read

DESCRIPTION
  This function reads the given byte array

  ssl_session_id  - vaild session id to an SSL session
  buffer          - Byte pointer to the data in network order
  nbytes          - Numbder of bytes to read

DEPENDENCIES
  The SSL session is connected

RETURN VALUE
  Total number of bytes read, which could be less than the number of bytes specified.

  possible error conditions are:
    DS_SSL_WOULDBLOCK
    DS_SSL_INVALID_ARG
    DS_SSL_NOT_AVAILABLE
    DS_SSL_ERROR_UNKNOW

SIDE EFFECTS  None
===========================================================================*/
sint15 ds_ssl_session_read(sint15 ssl_session_id, uint8 *buffer, uint16 nbytes);





/*===========================================================================
DESCRIPTION
  This function fowards external DS socket event to SSL engine.
  sock_fd    : socket descriptor
  event_mask : socket event mask

DEPENDENCIES
  An external socket is already connected

RETURN VALUE
  On successful operation, this function returns
    DS_SSL_SUCCESS

  On error, retuns:
    DS_SSL_INVALID_ARG
    DS_SSL_NOT_AVAILABLE

SIDE EFFECTS  None
===========================================================================*/
sint15  ds_ssl_foward_sockevent(sint15 sock_fd, uint32 event_mask);


void ds_ssl_init(void);
void ds_ssl_msg_process(uint32 *data_ptr);


#ifdef __cplusplus
}
#endif


#endif /* DS_SSL_API_H */


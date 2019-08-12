#ifndef A2_REMOTE_EVENT_H
#define A2_REMOTE_EVENT_H
/******************************************************************************
  @file  a2_remote_event.h
  @brief A2 RPC program interface

  DESCRIPTION
    Interface definition for handshake by RPC between A2 on QDSP6 and clients on ARM9.
  
  INITIALIZATION AND SEQUENCING REQUIREMENTS

  
  -----------------------------------------------------------------------------
  Copyright (c) 2011 QUALCOMM Technologies Incorporated. 
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  -----------------------------------------------------------------------------
******************************************************************************/

/*===========================================================================

                             EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/public/a2_remote_event.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/05/11    yuw    Initial version.
===========================================================================*/

/*============================================================================

             TYPE DEFINITIONS

============================================================================*/
#include "comdef.h"

/*
    List of request events from client to A2
 */
typedef enum
{
  A2_REMOTE_EVENT_SUSPEND_A2_REQUEST,
  A2_REMOTE_EVENT_RESUME_A2_REQUEST,
  A2_REMOTE_EVENT_RESET_A2_BAM_REQUEST,
}a2_remote_event_client_req_e;

/*
    List of confirm events from client to A2
 */
typedef enum
{
  A2_REMOTE_EVENT_WAKEUP_HSIC_CONFIRMED,
  A2_REMOTE_EVENT_FLUSH_HSIC_BAM_CONFIRMED,
}a2_remote_event_client_confirm_e;

/*
    List of request events from A2 to client
 */
typedef enum
{
  A2_REMOTE_EVENT_WAKEUP_HSIC_REQUEST,
  A2_REMOTE_EVENT_FLUSH_HSIC_BAM_REQUEST,
}a2_remote_event_req_e;

/*
    List of confirm events from A2 to client
 */
typedef enum
{
  A2_REMOTE_EVENT_SUSPEND_A2_CONFIRMED,
  A2_REMOTE_EVENT_RESUME_A2_CONFIRMED,
  A2_REMOTE_EVENT_RESET_A2_BAM_CONFIRMED,
}a2_remote_event_confirm_e;

/*============================================================================

             CALLBACK TYPE DEFINITIONS

============================================================================*/

/**
   a2_remote_event_req_cb_type
   Callback function which is called when A2 sends request event to client.
 */
typedef void (*a2_remote_event_req_cb_type) (a2_remote_event_req_e event);
/*~ CALLBACK a2_remote_event_req_cb_type */

/**
   a2_remote_event_confirm_cb_type
   Callback function which is called when A2 sends confirm event to client.
 */
typedef void (*a2_remote_event_confirm_cb_type) (a2_remote_event_confirm_e event);
/*~ CALLBACK a2_remote_event_confirm_cb_type */

/*============================================================================

             FUNCTION DECLARATIONS

============================================================================*/


/*===========================================================================
  FUNCTION  a2_remote_event_register_cb
===========================================================================*/
/*!
@brief
Register the callback functions to be called when A2 sends event to clients

@param[in]  
event_req_cb:  the callback function needs to be registered for all requests from A2
event_confirm_cb:  the callback function needs to be registered for all confirms from A2

@dependencies
none

@sideeffects 
none

@return
None.
*/
void a2_remote_event_register_cb
(
  a2_remote_event_req_cb_type event_req_cb,
  a2_remote_event_confirm_cb_type event_confirm_cb
);
/*~ FUNCTION a2_remote_event_register_cb 
    RELEASE_FUNC a2_remote_event_deregister_cb(event_req_cb, event_confirm_cb) */

/*===========================================================================
  FUNCTION  a2_remote_event_deregister_cb
===========================================================================*/
/*!
@brief
Deregister the callback functions to be called when A2 sends event to clients

@param[in]  
event_req_cb:  the callback function needs to be deregistered for all requests from A2
event_confirm_cb:  the callback function needs to be deregistered for all confirms from A2

@dependencies
none

@sideeffects 
none

@return
None.
*/
void a2_remote_event_deregister_cb
(
  a2_remote_event_req_cb_type event_req_cb,
  a2_remote_event_confirm_cb_type event_confirm_cb
);
/*~ FUNCTION a2_remote_event_deregister_cb */


/*===========================================================================
  FUNCTION  a2_remote_event_client_send_req
===========================================================================*/
/*!
@brief
Function to send request event to A2 from clients

@param[in]     
request event to be sent to A2

@dependencies 
none

@sideeffect 
none

@return
None.
*/
void a2_remote_event_client_send_req(a2_remote_event_client_req_e event_req);
/*~ FUNCTION a2_remote_event_client_send_req */

/*===========================================================================
  FUNCTION  a2_remote_event_client_send_confirm
===========================================================================*/
/*!
@brief
Function to send confirm event to A2 from clients

@param[in]     
confirm event to be sent to A2

@dependencies 
none

@sideeffect 
none

@return
None.
*/
void a2_remote_event_client_send_confirm(a2_remote_event_client_confirm_e event_confirm);
/*~ FUNCTION a2_remote_event_client_send_confirm */

#endif /* A2_REMOTE_EVENT_H */


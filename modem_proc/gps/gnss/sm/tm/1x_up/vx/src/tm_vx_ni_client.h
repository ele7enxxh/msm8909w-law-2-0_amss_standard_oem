#ifndef TM_VX_NI_CLIENT_H
#define TM_VX_NI_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                  TM Vx NI Client Header File

DESCRIPTION
  This header file contains the definition of API and data structures for
  Vx NI client module.

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/1x_up/vx/src/tm_vx_ni_client.h#1 $  
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/8/06    cl      Initial Release
===========================================================================*/

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/
#include "tm_vx.h"
#include "tm_pdapi_iface.h"
#include "pdapiclient.h"

/* Enum for session state */
typedef enum
{
  TM_VX_NI_CLIENT_SESS_STATE_INIT = 0,
  TM_VX_NI_CLIENT_SESS_STATE_PD_IN_PROGRESS,
  TM_VX_NI_CLIENT_SESS_STATE_PD_DONE
} tm_vx_ni_client_sess_state_e_type;

/* Struct for keeping track of singleshot/tracking information */
typedef struct
{
  tm_vx_mt_sms_pos_tech_ind_e_type   pos_tech;
  tm_vx_ni_client_sess_state_e_type  sess_state;
  tm_pdapi_get_pos_param_s_type      get_pos_param;
  pdsm_pd_option_s_type              pd_option_param;
  pdsm_pd_qos_type                   pd_qos_param;
  /* NI tracking session timer */
  os_TimerMsgType                    *tracking_timer;
  uint32                             current_fix_count;
} tm_vx_ni_client_session_info_s_type;

extern tm_vx_ni_client_session_info_s_type tm_vx_ni_session_info;

/*===========================================================================
FUNCTION tm_vx_ni_client_init

DESCRIPTION
  This function initialize tm vx ni client module

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
extern void tm_vx_ni_client_init(void);

/*===========================================================================
FUNCTION tm_vx_ni_client_tracking_timer_handler

DESCRIPTION
  This function handles ni tracking timer expiry event
DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
extern void tm_vx_ni_client_tracking_timer_handler(void);

/*===========================================================================
FUNCTION tm_vx_ni_client_start_session

DESCRIPTION
  This function is called to start Vx NI session.
  It is called in following 2 scenarios
  1. NI req without user verification needed.
  2. NI req with user verificcation needed and user accept the ni req.

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
extern boolean tm_vx_ni_client_start_session(void);

/*===========================================================================
FUNCTION tm_vx_ni_client_stop_session

DESCRIPTION
  This function is called to stop Vx NI session.
  It can be called in following scenarios
  1. User aborts NI session.
  2. MPC discontiues NI tracking session.

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
extern void tm_vx_ni_client_stop_session(void);

/*===========================================================================
FUNCTION tm_vx_ni_client_xlate_op_mode

DESCRIPTION
  This function translates V2 SMS is801 mode to corresponding PD API op mode.

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
extern pdsm_pd_session_operation_e_type tm_vx_ni_client_xlate_op_mode(uint8 is801_mode);

/*===========================================================================
FUNCTION tm_vx_ni_client_sess_param_check

DESCRIPTION
  This function is called to check if an NI session is allowed by TM-Core

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
boolean tm_vx_ni_client_sess_param_check(void);

/*===========================================================================
FUNCTION tm_vx_ni_handle_pd_event

DESCRIPTION
  This function is called to pass PD event to NI client

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
extern void tm_vx_ni_handle_pd_event(pdsm_pd_event_type  pd_event);

#ifdef __cplusplus
}
#endif

#endif /* TM_VX_NI_CLIENT_H */


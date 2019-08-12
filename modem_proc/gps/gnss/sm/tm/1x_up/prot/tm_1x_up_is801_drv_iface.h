#ifndef TM_1X_UP_IS801_DRV_IFACE_H
#define TM_1X_UP_IS801_DRV_IFACE_H

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                  TM IS801 Session Driver Interface Header File

DESCRIPTION
  This header file contains interface definition for IS801 session driver.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/1x_up/prot/tm_1x_up_is801_drv_iface.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/27/07   rw      Add support for JGPS (KDDI) module
===========================================================================*/

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/
#include "tm_prtl_iface.h"
#include "pd_comms_api.h"

typedef enum 
{
  TM_IS801_DRV_REQ_NONE = -1,	
  TM_IS801_DRV_REQ_START,
  TM_IS801_DRV_REQ_CONT,
  TM_IS801_DRV_REQ_STOP,
  TM_IS801_DRV_REQ_START_NODWLD
} tm_1x_up_is801_drv_req_e_type;

typedef struct
{
  boolean                           wake_tm_core;
  prtl_start_sess_req_param_s_type  *start_sess_req_param;
  tm_sess_req_start_param_s_type    *actions;
} tm_1x_up_is801_drv_start_param_s_type;

typedef struct
{
  prtl_sess_stop_type           stop_type;
  tm_sess_stop_reason_e_type    stop_reason;
} tm_1x_up_is801_drv_stop_param_s_type;

typedef union
{
  tm_1x_up_is801_drv_start_param_s_type  start_param;
  tm_1x_up_is801_drv_stop_param_s_type   stop_param;
} tm_1x_up_is801_drv_req_param_u_type;

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_init

DESCRIPTION
  This function is called by Vx or KDDI module to initialize IS801 driver module

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
extern void tm_1x_up_is801_drv_init(void);


/*===========================================================================

FUNCTION tm_1x_up_is801_drv_req

DESCRIPTION
  This function is called by Vx or KDDI module to start or stop a IS801 session

  For example, KDDI module can call this function to inform IS-801 driver
  that it has successfully requested a 

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
extern boolean tm_1x_up_is801_drv_req
(
  tm_sess_handle_type            sess_handle,
  tm_1x_up_is801_drv_req_e_type        req_type,
  tm_1x_up_is801_drv_req_param_u_type  req_param
);

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_sess_info

DESCRIPTION
  This function is called by Vx or KDDI module to request aiding data download

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
extern boolean tm_1x_up_is801_drv_req_aiding_data
(
  tm_sess_handle_type  session_handle,
  prtl_data_req_type   req_data
);


/*===========================================================================

FUNCTION: tm_1x_up_is801_drv_sess_info_handler

DESCRIPTION:
  This function is called by Vx or KDDI module to provide session
  info data (PPM/PRM for example).
   
PARAMETERS:
  
===========================================================================*/
extern boolean tm_1x_up_is801_drv_sess_info_handler
(
  tm_sess_handle_type          session_handle,
  prtl_sess_info_e_type        sess_info_type,
  prtl_sess_info_param_u_type  *sess_info_param
);

/*===========================================================================

FUNCTION: tm_1x_up_is801_drv_data_pipe_handler

DESCRIPTION:
  This function is called by Vx or KDDI to relay transport layer information to 
  protocol sub-module 
  
  Use msg_type to decode msg_data_ptr (can be one of the following)  
    pd_comms_ipc_write_ack_hdr_type;
    pd_comms_ipc_read_data_hdr_type;
    pd_comms_ipc_notification_hdr_type;
    pd_comms_ipc_ioctl_ack_hdr_type;
    pd_comms_ipc_hash_resp_hdr_type;  
   
PARAMETERS:
  
===========================================================================*/
extern boolean tm_1x_up_is801_drv_data_pipe_handler
(
  pd_comms_client_ipc_message_type  msg_type,
  void                              *msg_data_ptr
);

/*===========================================================================

FUNCTION: tm_1x_up_is801_drv_timer_handler

DESCRIPTION:
  This function is called by Vx or KDDI to notify IS801 session module that a
  paticular timer has expired.
   
PARAMETERS:

NOTE:    
===========================================================================*/
extern void tm_1x_up_is801_drv_timer_handler
(
  void *timerData
);

/*===========================================================================

FUNCTION: tm_1x_up_is801_drv_event_handler

DESCRIPTION:
  This function is called by Vx or KDDI to notify IS801 session module that an
  particular event (phone event, user response) has happened. 
   
PARAMETERS:
    
===========================================================================*/
extern void tm_1x_up_is801_drv_event_handler
(
  prtl_event_e_type  event_type,
  const prtl_event_u_type  *event_payload
);


/*===========================================================================

FUNCTION: tm_1x_up_is801_drv_send_data

DESCRIPTION:
  This function is called by Vx or KDDI to tell IS801 session module to send 
  the data on the current TCP/IP connection
  
PARAMETERS:

RETURN VALUE:
  TRUE - If packet is sent successfully to PD COMM
  FALSE - All other error cases.
    
===========================================================================*/
extern boolean tm_1x_up_is801_drv_send_data
(
  byte   *send_data_ptr,
  int16  send_data_len
);

/*===========================================================================

FUNCTION: tm_1x_up_is801_drv_read_data

DESCRIPTION:
  This function is called by Vx or KDDI to tell IS801 session module to read 
  the data on the current TCP/IP connection
  
PARAMETERS:

RETURN VALUE:
  TRUE - If packet is sent successfully to PD COMM
  FALSE - All other error cases.
    
===========================================================================*/
extern boolean tm_1x_up_is801_drv_read_data
(
  uint16    len_to_read,
  boolean   wait_all_data
);

/*===========================================================================

FUNCTION: tm_1x_up_is801_drv_disconnect

DESCRIPTION:
  This function is called by Vx or KDDI to tell IS801 session module to 
  disconnect the any PDCOMM connections
  
PARAMETERS:

RETURN VALUE:
  TRUE - If packet is sent successfully to PD COMM
  FALSE - All other error cases.
    
===========================================================================*/
extern boolean tm_1x_up_is801_drv_disconnect
(
  boolean abort
);


/*===========================================================================

FUNCTION: tm_1x_up_is801_drv_app_ioctl

DESCRIPTION:
  This function is called by Vx or KDDI to send an IOCTL
  
PARAMETERS:

RETURN VALUE:
  TRUE - If packet is sent successfully to PD COMM
  FALSE - All other error cases.
    
===========================================================================*/
extern boolean tm_1x_up_is801_drv_app_ioctl
(
  pd_comms_ioctl_cmd_type cmd
);


/*===========================================================================

FUNCTION tm_1x_up_is801_drv_is_optimal_mode

DESCRIPTION
  This function is used to indicate if mode is optimal or not.

RETURN VALUE
  TRUE if Optimal, else FALSE.

DEPENDENCIES

===========================================================================*/
extern boolean tm_1x_up_is801_drv_is_optimal_mode(void);


/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_pde_connected

DESCRIPTION
  This function is used to handle the state when we are connected
  with PDE.

   For Call flow 1, 2, 3: Connected to PDE is trigger to REQ_PPM to TM-Core 
   For Call flow 7: Conencted to PDE is trigger to REQ_LR to TM-Core
   
RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
extern void tm_1x_up_is801_drv_handle_pde_connected(boolean sess_start);


/*===========================================================================

FUNCTION tm_1x_up_is801_drv_check_msb_throttling

DESCRIPTION
  This function is used to check if the next connection to PDE should
  be throttled or not.

RETURN VALUE
  TRUE: Yes, throttle this conneciton
  FALSE: No, don't throttle this connection.

DEPENDENCIES

===========================================================================*/
extern boolean tm_1x_up_is801_drv_check_msb_throttling(boolean bConnectToMPC);


/*===========================================================================

FUNCTION tm_1x_up_is801_drv_set_mpc_throttle_check

DESCRIPTION
  This function is used to set the msb_throttling_info.mpc_throttle_check flag

RETURN VALUE
  NONE

DEPENDENCIES

===========================================================================*/
extern void tm_1x_up_is801_drv_set_mpc_throttle_check(boolean bMPCThrottleCheck);


/*===========================================================================

FUNCTION tm_1x_up_set_throttling_backoff_time

DESCRIPTION
  This function is used to set pdsm throttling backoff time

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
extern void tm_1x_up_mpc_set_throttling_backoff_time(void);

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_v2_lr_resp_msbased

DESCRIPTION
  This function is used to report position to user before Vx connection is completed

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
extern boolean tm_1x_up_is801_drv_handle_v2_lr_resp_msbased(sm_GnssFixRptStructType* lr_ptr);

#ifdef __cplusplus
}
#endif

#endif /* TM_1X_UP_IS801_DRV_IFACE_H */

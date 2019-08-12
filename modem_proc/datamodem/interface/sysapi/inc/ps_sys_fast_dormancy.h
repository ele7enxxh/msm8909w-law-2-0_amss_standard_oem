/**
@file ps_sys_fast_dormancy.h
@brief
This file defines functions, enums and structs used for fast dormancy feature implementation
*/

#ifndef PS_SYSI_FAST_DORMANCY_H
#define PS_SYSI_FAST_DORMANCY_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ S Y S _ FAST_DORMANCY . H


GENERAL DESCRIPTION
  This file defines functions, enums and structs used for fast dormancy  
  implementation.

Copyright (c) 2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/sysapi/inc/ps_sys_fast_dormancy.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when                who           what, where, why
--------    ---    ---------------------------------------------------------- 
 4/29/2014      tyk           .created module
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "ps_svc.h"
#include "pstimer.h"
#include "ps_sys_ioctl.h"
#include "ps_iface_fast_dormancy.h"

/*===========================================================================

                         EXTERNAL DATA DECLARATIONS

===========================================================================*/

#define TRUE 1
#define FALSE 0

#define PS_SYS_FAST_DORMANCY_MAX_TIMERS 2

/* enum used to refer fast formancy timers */
typedef enum
{
  PS_SYS_FAST_DORMANCY_TIMER_1,
  PS_SYS_FAST_DORMANCY_TIMER_2,
  PS_SYS_FAST_DORMANCY_TIMER_MAX = PS_SYS_FAST_DORMANCY_TIMER_2
  
}ps_sys_fast_dormancy_enum_timer_type;

/* structure to take timer values from Mode Handler */
typedef struct
{
  uint32 timer_value_1;
  uint32 timer_value_2;
}ps_sys_fast_dormancy_timer_values_s;

/* Structure to hold Fast Dormancy Timers related information */
typedef struct 
{
  ps_timer_handle_type  timer_hdl;  /* Holds timer handle for Timer_1 */
  uint32                timer_value;      /*Holds Timer Value for Timer:1  */

} ps_sys_fast_dorancy_timer_info_s;

/* structure to hold Common Fast Dormancy and timer Information  */
typedef struct 
{
   /* Holds Timer Info */
  ps_sys_fast_dorancy_timer_info_s   timer_info[PS_SYS_FAST_DORMANCY_MAX_TIMERS]; 
 
   /* Indicates if dropped Indication is received */
  boolean                            pkt_dropd_ind_rcvd;
  
}ps_sys_fast_dormancy_info_s;


/*===========================================================================

                         FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION PS_SYS_FAST_DORMANCY_START()

DESCRIPTION
  This function processes start fast dormancy command and starts timer_1. 
  It updates statistics if Timer_1 is already running  else (first trigger) 
  Initializes IFACE Statistics before starting Timer

PARAMETERS
event_info_p: contains timer values in milliseconds

RETURN VALUE
  0 -  Success
 -1 - Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_sys_fast_dormancy_start
(
  void* event_info_p
);

/*===========================================================================
FUNCTION PS_SYS_FAST_DORMANCY_STOP()

DESCRIPTION
  This function clears Fast Dormancy Timers and also fress the memory allocated
  for all IFACE Stats

PARAMETERS
None

RETURN VALUE
 None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_sys_fast_dormancy_stop(void);

/*===========================================================================
FUNCTION PS_SYS_FAST_DORMANCY_PKT_DROPPED_IND()

DESCRIPTION
  This function handles  packet dropped indication from Framework 
  and starts  Fast Dormancy Timer_2 

PARAMETERS
iface_ptr: IFACE on which packet drop occured

RETURN VALUE
 None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_sys_fast_dormancy_pkt_dropped_ind
(
  ps_iface_type     *iface_ptr   /* for later use or enhancement */
);


/*===========================================================================
FUNCTION PS_SYS_FAST_DORMANCY_TIMER_EXPIRY_CB() 

DESCRIPTION
  This function will get called when timer is expired.
  checks for data activity and calls mode handler  IOCTL to release
  signalling connection if activity is not present

PARAMETERS
cb_data: carrys information on what timer expired

RETURN VALUE
 None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_sys_fast_dormancy_timer_expiry_cb
(
  void*  cb_data /* callback data contains the timer identity here */
);

/*===========================================================================
FUNCTION PS_SYS_FAST_DORMANCY_IS_TIMER_RUNNING()

DESCRIPTION
  This function gives information on whether  timer is running or not currently

PARAMETERS
None

RETURN VALUE
TRUE -  If running
FALSE - if Not

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint8 ps_sys_fast_dormancy_is_timer_running
(
  ps_sys_fast_dormancy_enum_timer_type    timer_type
);

#endif /*PS_SYSI_FAST_DORMANCY_H*/

#ifndef DHCP_CLIENT_TIMER_H
#define DHCP_CLIENT_TIMER_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  D H C P _ C L I E N T _ T I M E R . H

GENERAL DESCRIPTION
  DMSS Dynamic Host Configuration Protocol client timer header file.  
  This header file provides the shim layer between the core and the 
  timer services routines in both the DHCPv4 and DHCPv6 clients.
  
Copyright (c) 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                           EDIT HISTORY FOR MODULE
                           
$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/dhcp_client_timer.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/09/14    ash    Added Prefix Delegation support.
09/26/08    am     Changed malloc/free names for off target support.
06/29/06    es     Initial development work.
===========================================================================*/
#include "comdef.h"

/* Modes for the timer start function. */
typedef enum {
  DHCP_CLIENT_TIMER_NORM                = 0,
  DHCP_CLIENT_TIMER_RAND_BACKOFF        = 1,
  DHCP_CLIENT_TIMER_RAND_INIT           = 2,
  DHCP6_CLIENT_TIMER_RAND_BACKOFF_START = 3,
  DHCP6_CLIENT_TIMER_RAND_BACKOFF_NORM  = 4,
  DHCP6_CLIENT_TIMER_RAND_INIT          = 5
} dhcp_client_timer_mode;

/* The init structure contains the information that is supplied by the init
 * call. */
typedef struct {
  void * (*d_malloc)(uint32 size);
  void (*d_free)(void *);
} dhcp_client_timer_init;

/* The config structure contains information that is supplied on start.  The
 * information describes how to interface with the other modules in the DHCP
 * client.  */
typedef struct { 
  void (*timer_expire)(void * timer_expire_handle);
  void * timer_expire_handle;
} dhcp_client_timer_config;

/*===========================================================================
FUNCTION DHCP_CLIENTI_TIMER_NEW

Description:
  This function initalizes a new timer shim layer object.  A handle to
  the object will be returned. 

Arguements:
  dhcp_client_timer_init * - the information needed to get initialized. 

Return value:
  Handle to timer shim layer object

Dependencies:
  The init must be valid.
  The dhcp_clienti_timer_start() function must be called before the
  timer layer is used. 
===========================================================================*/
void * 
dhcp_clienti_timer_new
(
  dhcp_client_timer_init * init
);

/*===========================================================================
FUNCTION DHCP_CLIENTI_TIMER_START

Description:
  This function starts the timer layer for use. 

Arguements:
  void * handle - Handle to the timer object.
  dhcp_client_timer_configure * - Configuration infomation to
    interface with other systems. 

Return value:
  boolean - TRUE if started ok, otherwise false.

Dependencies:
  The dhcp_clienti_timer_new must have initialized the handle.
===========================================================================*/
boolean
dhcp_clienti_timer_start
(
  void * handle,
  dhcp_client_timer_config * config
);

/*===========================================================================
FUNCTION DHCP6_CLIENTI_TIMER_FREE

Description:
  This function frees the timer layer.

Arguements:
  void * handle - Handle to the timer object.

Return value:
  boolean - TRUE if started ok, otherwise false.

Dependencies:
  The dhcp_clienti_timer_new must have initialized the handle.
===========================================================================*/
void
dhcp_clienti_timer_free
(
  void ** handle
);

/*===========================================================================
FUNCTION DHCP_CLIENTI_TIMER_SET

Description:
  This function starts a timer set to expire with the specified time,
  in seconds.  If there is already a timer running associated with
  this instance, it will be cancelled, and the current timer set in
  its place.

Arguements:
  void * handle - Handle to the timer object.
  uint32 delta - The amount of time to set the timer for in seconds.
  dhcp_client_timer_mode mode - indicates the mode of the timer: 
     - Norm - no randomization to the number.
     - V4 Backoff - +/- 1 second randomization to the delta value.
     - V4 Init - 1 - delta second randomization. 
     - V6 Backoff_Start - initial backoff retransmission time randomization.
                          See RFC 3315, Section 14. Delta should be passed
                          as the IRT (initial retransmission time) value.
     - V6 Backoff_Norm  - initial backoff retransmission time randomization.
                          See RFC 3315, Section 14. Delta should be passed
                          as the MRT (max retransmission time) value.
     - V6 Init - 0 to delta second randomization.
     
Return value:
  None.

Dependencies:
  The dhcp_clienti_timer_new must have initialized the handle.
  The timer layer must have been started. 
===========================================================================*/
void
dhcp_clienti_timer_set
(
  void * handle,
  uint32 delta,
  dhcp_client_timer_mode mode
);

/*===========================================================================
FUNCTION DHCP6_CLIENTI_TIMER_CLEAR

Description:
  This function clears the timer associated with this instance, if
  there is one.

Arguements:
  void * handle - Handle to the timer object.

Return value:
  None.

Dependencies:
  The dhcp_clienti_timer_new must have initialized the handle.
  The timer layer must have been started. 
===========================================================================*/
void
dhcp_clienti_timer_clear
(
  void * handle
);

#endif /* DHCP_CLIENT_TIMER_H */

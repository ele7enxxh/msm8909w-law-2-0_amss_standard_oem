#ifndef PS_PCB_CLOSE_REASON_CODES_H
#define PS_PCB_CLOSE_REASON_CODES_H
/*===========================================================================
  @file ps_pcb_close_reason_codes.h

  This file defines the reasons why a PCB would be closed. One of these
  reasons is sent along with PS_SOCKET_PCB_CLOSED_EV to the platform socket
  and it reports an appropriate error to the application.

  Copyright (c) 2008 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/ps_pcb_close_reason_codes.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/



/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/**
  @brief Normal close.

  PCB is closed normally either locally or by peer.
*/
#define PS_PCB_CLOSE_REASON_CODE_NORMAL         0

/**
  @brief Reset by peer.

  PCB is closed because peer reset the connection.
*/
#define PS_PCB_CLOSE_REASON_CODE_RESET          1

/**
  @brief Re-transmission timeout.

  PCB is closed because peer is not responding and re-transmission timeout
  elapsed.
*/
#define PS_PCB_CLOSE_REASON_CODE_TIMEOUT        2

/**
  @brief Network issue.

  PCB is closed because of some network issue.
*/
#define PS_PCB_CLOSE_REASON_CODE_NETWORK        3

#define PS_PCB_CLOSE_REASON_CODE_MAX            4


#endif /* PS_PCB_CLOSE_REASON_CODES_H */

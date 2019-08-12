#ifndef PS707_ASYNC_H
#define PS707_ASYNC_H
/*===========================================================================

                        P S 7 0 7 _ A S Y N C 

DESCRIPTION
  This file contains functionality for IS707 async/fax mode-specific
  handlers that deals mainly deals with call control.

EXTERNALIZED FUNCTIONS
  ps707_async_in_rawdata_mode()
    Returns True/False depending on whether a raw data call is in progress.

  ps707_async_process_tcp_send_q()
    This function processes all the items application layers want TCP to 
    transmit.The items sent by the serial port are escaped for 617 and Telnet 
    characters, while the 617 cmds or responses are enqueued as is into the TCP 
    send queue.The argument ack_piggyback means that this function is called
    when a TCP ack has to be sent. If this argument is TRUE, only one segment is
    transmitted, otherwise segments are transmitted till the tcp_send_q is empty.

Copyright (c) 2009-2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE
    $PVCSPath: L:/src/asw/MM_DATA/vcs/ps707_async.h_v   1.4   02 Dec 2002 12:59:50   ajithp  $
    $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/api/ps707_async.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/27/10    ps     Added code changes to fix the broken Async/Fax support.
10/28/09    vs     Removed the inclusion of customer.h
06/30/09    ss     New file created for CMI SU level changes.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "ps_tcp_async.h"

#ifdef FEATURE_DATA_IS707_ASYNC
/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================

FUNCTION      PS707_ASYNC_IN_RAWDATA_MODE

DESCRIPTION   Returns True/False depending on whether a raw data call is in
              progress.

DEPENDENCIES  None.

RETURN VALUE  TRUE/FALSE

SIDE EFFECTS  None.

===========================================================================*/
boolean ps707_async_in_rawdata_mode
(
  void
);

/*===========================================================================
FUNCTION      PS707_ASYNC_PROCESS_TCP_SEND_Q

DESCRIPTION   This function processes all the items application layers
              want TCP to transmit.  The items sent by the serial port are
              escaped for 617 and Telnet characters, while the 617 cmds
              or responses are enqueued as is into the TCP send queue.
              The argument ack_piggyback means that this function is called
              when a TCP ack has to be sent. If this argument is TRUE, only
              one segment is transmitted, otherwise segments are transmitted
              till the tcp_send_q is empty.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_process_tcp_send_q
(
  boolean ack_piggyback                           /*If TCP ACK piggybacked */
);

#endif /* FEATURE_DATA_IS707_ASYNC */
#endif /* PS707_ASYNC_H   */

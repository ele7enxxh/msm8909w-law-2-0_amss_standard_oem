#ifndef DS707_ASYNC_DEFS_H
#define DS707_ASYNC_DEFS_H
/*===========================================================================

                        D S 7 0 7 _ A S Y N C _ D E F S
DESCRIPTION
  This file contains definitions of data structures, enumerations, etc 
  that are used in an IS707 circuit switched data call.

EXTERNALIZED FUNCTIONS

 Copyright (c) 2002-2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE
  $PVCSPath: MM_DATA/ds707_async_defs.h_v 1.3 02 Oct 2002 13:22:16 ajithp  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_async_defs.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/11/11    ack    Global Variable Cleanup
05/18/06    sq     removed pkt_um_iface_ptr from struct ds707_async_state_type
01/03/06    ksu    EMPA: hook up forward link with new phys link architecture
12/15/05    TMR    Changed RLP RX queue to watermark and added parameter to 
                   dsrlp_reg_srvc() and RX callback function
03/16/04    atp    Lint fixes.
08/06/03    mvl    Updates for separation of phys_link.
10/01/02    atp    Cleanup.
08/22/02    atp    Cleanup for shipping.
08/19/02    atp    Support for Um flow control for low-baud SIO links.
07/15/02    atp    Added header.
07/13/02    atp    First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707_ASYNC
#include "cm.h"
#include "ds707.h"
#include "dsm.h"
#include "ps_iface.h"
#include "sio.h"


/*===========================================================================
                            TYPEDEFS
===========================================================================*/


/*---------------------------------------------------------------------------
  Enum that represents the state of the async TCP connection from DS 
  point of view.
---------------------------------------------------------------------------*/
typedef enum
{
  DS707_ASYNC_PROTOCOL_CLOSED,
  DS707_ASYNC_PROTOCOL_OPENING,
  DS707_ASYNC_PROTOCOL_OPEN,
  DS707_ASYNC_PROTOCOL_CLOSING

} ds707_async_protocol_state_enum_type;


/*---------------------------------------------------------------------------
  Traffic channel state info.  Contains info specific to one CM call.
---------------------------------------------------------------------------*/
typedef struct
{
  ds707_so_type            so;                       /* SO of this call    */
  cm_call_id_type          call_id;                  /* CM call id         */
  dsm_watermark_type      *tx_watermark_ptr;         /* RLP Tx watermark   */
  void                    (*post_rx_func_ptr)(uint32); /* RLP Rx callback  */
  dsm_watermark_type      *post_rx_wm_ptr;            /* RLP Rx watermark  */

} ds707_async_tc_state_type;

/*---------------------------------------------------------------------------
  Structure that holds overall state info for async call. Includes info
  about the traffic channel and the ps_iface.
---------------------------------------------------------------------------*/
typedef struct
{
  /*-------------------------------------------------------------------------
    The PS iface and phys link for IS-707 async calls 
  -------------------------------------------------------------------------*/
  ps_iface_type             ps_iface;
  ps_phys_link_type         ps_phys_link;
  /*-------------------------------------------------------------------------
    State of traffic channel.
  -------------------------------------------------------------------------*/
  ds707_async_tc_state_type tc_state;
  /*-------------------------------------------------------------------------
    State of async TCP connection.
  -------------------------------------------------------------------------*/
  ds707_async_protocol_state_enum_type protocol_state;
  /*-------------------------------------------------------------------------
    Semaphores to remember if NO CARRIER, OK were sent to the serial port.
  -------------------------------------------------------------------------*/
  boolean no_carrier_txed;
  boolean ok_result_txed;
  boolean ps_closed_properly;

  /*-------------------------------------------------------------------------
    For enabling/disabling Um flow. Applicable for low-baud SIO links.
  -------------------------------------------------------------------------*/
  boolean um_flow_enable;

} ds707_async_state_type;

/*===========================================================================
FUNCTION      DS707_GET_ASYNC_STATE_PTR

DESCRIPTION   Accessor method for ds707_async_state structure

DEPENDENCIES  None.

RETURN VALUE  Returns pointer to ds707_async_state structure.

SIDE EFFECTS  None.
===========================================================================*/
ds707_async_state_type* ds707_get_async_state_ptr
(
  void
);

#endif /* FEATURE_DATA_IS707_ASYNC */
#endif /* DS707_ASYNC_DEFS_H   */

#ifndef _DSDVRLP_RX_H
#define _DSDVRLP_RX_H

/*===========================================================================

  D A T A   S E R V I C E S   R L P   E X T E R N A L   H E A D E R  F I L E

DESCRIPTION
   This file contains external variable and function declarations for the
   Radio Link Protocol implentation that supports EVDV.

   This should be included by files which interface with RLP, like the
   MuxLayer and Call Processing for Forward link Rx processing.

EXTERNALIZED FUNCTIONS
   dsdvrlp_alloc_fwd_frames( n )
     Called by Mux to allocate n elements when it has received n pdus 
     at that point on Forward Link which will later be enqueued to RLP.

   dsdvrlp_enqueue_fwd_frames()
     Called by Mux to enqueue a certain number of frames received on Forward
     Link to RLP after it has filled up info regarding rate, muxpdutype, etc
     for each pdu.

   dsdvrlp_process_fwd_frames()
     Called by Mux at fixed intervals for RLP to process enqueued frames.    

   dsdvrlp_process_20ms_tick()
     Called by Mux once in 20 ms.
   
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1999-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/dsdvrlp_rx.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/12/11   sn      Removed internal header files.
10/28/09   vs      Removed the inclusion of customer.h
07/22/04   atp     After lint.
06/08/04   atp     Support for upper bound on DDW and REXMIT_TIMER.
05/07/04   atp     Initial version. First cut at supporting EVDV (IS707-B).

===========================================================================*/

/*===========================================================================
                      INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"

#include "dsm.h"
#include "queue.h"
#include "dsrlp_api.h"


/*===========================================================================
                      EXTERNAL DATA DECLARATIONS
===========================================================================*/


/*---------------------------------------------------------------------------
  Channels on which data can arrive.
---------------------------------------------------------------------------*/
#define DSDVRLP_RX_FCH_ASSIGNED_MASK  1
#define DSDVRLP_RX_DCCH_ASSIGNED_MASK 2
#define DSDVRLP_RX_SCH_ASSIGNED_MASK  4
#define DSDVRLP_RX_PDCH_ASSIGNED_MASK 8

typedef enum
{
  DSDVRLP_RX_FCH = 0,
  DSDVRLP_RX_DCCH,
  DSDVRLP_RX_SCH,
  DSDVRLP_RX_PDCH
} dsdvrlp_fwd_channel_enum_type;

/*---------------------------------------------------------------------------
  Structure of RLP info received on the forward link. Contains a ptr to
  the actual frame. Also has info fields, some filled up by mux, others by
  RLP. This structure is itself embedded in a dsm_item and can be enqueued
  in RLP's input queue.
---------------------------------------------------------------------------*/
typedef struct dsdvrlp_fwd_fr_info
{
  dsm_item_type  *this_dsm_item_ptr;    /* Ptr to dsm item this struct  in */

  /*-------------------------------------------------------------------------
    To be filled by Mux
  -------------------------------------------------------------------------*/
  dsm_item_type  *actual_fr_item_ptr;   /* Ptr to frame payload            */
  dsrlp_srvc_enum_type srvc_type;       /* Primary/Sec./NA                 */
  dsrlp_mux_pdu_enum_type mux_pdu_type; /* MuxPDU type                     */
  dsrlp_rate_enum_type    fr_rate;      /* Received frame rate             */
  byte sr_id;                           /* Service ID - identifies service */
  dsdvrlp_fwd_channel_enum_type chan;   /* FCH/DCCH/SCH/PDCH               */

  /*-------------------------------------------------------------------------
    To be filled by RLP 
  -------------------------------------------------------------------------*/
  dsrlpi_fr_det_enum_type  fr_det_enum; /* New/Rexmit/Nak/idle/fill/etc    */
  uint32 seq_num;                       /* Frame sequence number           */
  uint32 fr_cat;                        /* Frame category bitmask          */
  uint16 time_to_expire_ddw;            /* Time left for DDW expiry        */
  uint16 pkts_after_counter;            /* Num pkts received after this    */

} dsdvrlp_fwd_fr_info_type;


/*===========================================================================
                      EXTERNAL FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================

FUNCTION DSDVRLP_ALLOC_FWD_FRAMES

DESCRIPTION
  Allocates  num forward frame element items, where num is a parameter
  passed by the caller (mux).  Note that each element is embedded in a
  dsm item. The items are linked together in the form of a linked list. 

DEPENDENCIES
  None

RETURN VALUE
  Ptr to first item in the linked list.

SIDE EFFECTS
  None
===========================================================================*/
dsm_item_type * dsdvrlp_alloc_fwd_frames 
( 
  uint8   num_frames                             /* Num items required     */
); 



/*===========================================================================

FUNCTION DSDVRLP_ENQUEUE_FWD_FRAMES

DESCRIPTION
  This function is called by Mux Rx to pass on a linked list of frame 
  elements received on Forward Link over to RLP's input queue. Along with 
  the pointer to the payload, the relevant info fields should have been 
  filled up by Mux. The parameter passed is a pointer to the first item in
  the list, and the number of frames (pdus) in the list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsdvrlp_enqueue_fwd_frames
(
  dsm_item_type * first_fwd_fr_item_ptr, /* Ptr to first dsm item in the
                                              linked list of items containing
                                              forward frame elements       */
  uint8  num_frames                        /* Number of frames in list     */
);


/*===========================================================================

FUNCTION DSDVRLP_PROCESS_FWD_FRAMES

DESCRIPTION
  This function is called by Mux Rx at periodic intervals (currently 5ms)
  for RLP to process the queued up frames.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsdvrlp_process_fwd_frames
(
  void
);

/*===========================================================================

FUNCTION DSDVRLP_PROCESS_20MS_TICK

DESCRIPTION
  This function is called by Mux Rx once in 20 ms.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsdvrlp_process_20ms_tick
(
  uint8  chan_assign,          /* Channels assigned at this time           */
  uint16 num_pkts_in_last_20ms /* Num encoder pkts rxed by mux in last 20ms*/
);

#endif  /* _DSDVRLP_RX_H */


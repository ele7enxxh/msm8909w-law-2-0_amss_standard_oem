/*===========================================================================

      H D R   P A C K E T   C O N S O L I D A T I O N   P R O T O C O L
                 S L P   P A C K E T   A P P   M O D U L E

DESCRIPTION
  This file contains the implementation of the 1x-EVDO Release B Packet 
  Consolidation Protocol's SLP Packet Application.

  Copyright (c) 2007 - 2016 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrpcp_slp_pkt_app.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/16/16   vko     Reduce excessive F3
10/29/13   sat     Reduced F3 messages (or/and) ASSERTs
07/27/11   cnx     Merged Optimized Handoff changes.
03/12/10   wsh     ConnectClose not sending out without early termination 
02/09/10   wsh     New API to notify RLP RX buffer empty
12/18/08   wsh     Fixed: some informative log message marked as errors
12/10/08   smd     Replaced assert.h with amssassert.h
10/01/08   wsh     Fixed critical/high lint warnings
09/03/07   wsh     Fixed SLP retransmit before TSLPWaitAck expires
08/27/07   sju     Fixed incompatible parameter type error
08/16/07   hal     Fixed the issue of leaking SLP ReTX items
08/14/07   hal     Updated get_data() callbacks from code review comments
08/06/07   hal     Added temporary F3 messaging for debugging purposes
07/30/07   hal     Fixed bug by changing iterate to dequeue
06/27/07   hal     Initial version

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*-----------------------------------------------------
  Common AMSS include files
-----------------------------------------------------*/

#include "hdr_variation.h"
#ifndef FEATURE_HDRPCP_UNITTEST
#include "amssassert.h"
#endif /* FEATURE_HDRPCP_UNITTEST */

#include "comdef.h"

#ifndef FEATURE_HDRPCP_UNITTEST
#include "dsm.h"
#endif /* FEATURE_HDRPCP_UNITTEST */

/*-----------------------------------------------------
  HDR specific include files
-----------------------------------------------------*/

#ifndef FEATURE_HDRPCP_UNITTEST
#include "hdrdebug.h"
#endif /* FEATURE_HDRPCP_UNITTEST */

#include "hdrpcp.h"
#include "hdrpcp_slp_pkt_app.h"
#include "hdrpcp_queue.h"

#ifndef FEATURE_HDRPCP_UNITTEST
#include "hdrslp.h"
#endif /* FEATURE_HDRPCP_UNITTEST */


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*-----------------------------------------------------
  Definitions and macros
-----------------------------------------------------*/

#define HDRPCP_SLP_MAX_RETX_ITEMS (8 * HDRPCP_MAX_COMBOS)

/*-----------------------------------------------------
  Data structures and types
-----------------------------------------------------*/

/*
 * hdrpcp_slp_pkt_app_retx_item_type
 *
 * The SLP packet application's internal retransmit item
 * type. This item holds the SLP packet as well as the
 * packet size for quick access.
 *
 */
typedef struct
{
  dsm_item_type *pkt_ptr;
  uint16 size;

} hdrpcp_slp_pkt_app_retx_item_type;

/*
 * hdrpcp_slp_pkt_app_retx_buffer_type
 *
 * The SLP packet application's internal retransmit buffer
 * type. This buffer holds a queue of retransmit items as
 * well as total byte and total packet counts.
 *
 */
typedef struct
{
  /* The item_q holds the retransmit items */
  hdrpcp_queue_type item_q;

  /* Stored for quick access without needing to iterate
     through the entire queue of stored retransmit items */
  uint32 num_bytes;
  uint32 num_pkts;

  boolean in_use;

} hdrpcp_slp_pkt_app_retx_buffer_type;

/*
 * hdrpcp_slp_pkt_app_ctrl_blk_info_type
 *
 * The SLP packet application's control block info structure.
 * This structure is initialized at the beginning of every
 * subframe.
 *
 */
typedef struct
{
  hdrhai_channel_enum_type channel;
  uint16 time_since_prev_req;
  boolean first_call_to_slp;
  boolean carrier_id_known;
  uint8 carrier_id;
  uint8 combo_id;

} hdrpcp_slp_pkt_app_ctrl_blk_info_type;

/*
 * hdrpcp_slp_pkt_app_static_data_type
 *
 * The SLP packet application's internal data structure. This
 * structure is the SLP packet application.
 *
 */
typedef struct
{
  hdrpcp_slp_pkt_app_retx_item_type retx_items[HDRPCP_SLP_MAX_RETX_ITEMS];
  
  hdrpcp_queue_item_type q_items[HDRPCP_SLP_MAX_RETX_ITEMS];
  hdrpcp_queue_type free_q;

  hdrpcp_slp_pkt_app_retx_buffer_type retx_buffer;
  hdrpcp_slp_pkt_app_retx_buffer_type sent_pkt_q[HDRPCP_MAX_COMBOS];

  hdrpcp_slp_pkt_app_ctrl_blk_info_type ctrl_blk;

} hdrpcp_slp_pkt_app_static_data_type;


/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

static hdrpcp_slp_pkt_app_static_data_type hdrpcp_slp_pkt_app;



/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/* -- None -- */



/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRPCP_SLP_PKT_APP_NEW_RETX_QUEUE_ITEM

DESCRIPTION
  This function dequeues a new retransmit item from the free queue.

DEPENDENCIES
  None.

PARAMETERS
  pkt_ptr   - The SLP packet that should be copied
  pkt_size  - The size of the SLP packet

RETURN VALUE
  hdrpcp_queue_item_type *  - A pointer to the new retransmit queue item

SIDE EFFECTS
  None.

===========================================================================*/
static hdrpcp_queue_item_type *hdrpcp_slp_pkt_app_new_retx_queue_item
(
  dsm_item_type *pkt_ptr,
  uint16 pkt_size
)
{
  hdrpcp_queue_item_type *retx_q_item_ptr = NULL;
  hdrpcp_slp_pkt_app_retx_item_type *retx_item_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  retx_q_item_ptr = hdrpcp_queue_get_item( &hdrpcp_slp_pkt_app.free_q );
  if( retx_q_item_ptr )
  {
    retx_item_ptr = 
      (hdrpcp_slp_pkt_app_retx_item_type *) retx_q_item_ptr->data;

    /* Dupe the DSM packet and store the size; symmetrically, the 
       free_rext_queue_item() routine will call dsm_free_packet()
       on the packet pointer */
    (void) dsm_dup_packet( &retx_item_ptr->pkt_ptr, pkt_ptr, 0, pkt_size );
    retx_item_ptr->size = pkt_size;
  }

  return retx_q_item_ptr;

} /* hdrpcp_slp_pkt_app_new_retx_queue_item() */

/*===========================================================================

FUNCTION HDRPCP_SLP_PKT_APP_FREE_RETX_QUEUE_ITEM

DESCRIPTION
  This function frees a retransmit queue item back to the free queue.

DEPENDENCIES
  None.

PARAMETERS
  retx_q_item_ptr  - A pointer to the retransmit queue item to be freed

RETURN VALUE
  None.

SIDE EFFECTS
  This function will also call DSM_free_packet() on the SLP packet 
  referenced by this retransmit queue item.

===========================================================================*/
static void hdrpcp_slp_pkt_app_free_retx_queue_item
(
  hdrpcp_queue_item_type **retx_q_item_ptr
)
{
  hdrpcp_slp_pkt_app_retx_item_type *retx_item_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( retx_q_item_ptr && *retx_q_item_ptr )
  {
    retx_item_ptr = 
      (hdrpcp_slp_pkt_app_retx_item_type *) ((*retx_q_item_ptr)->data);

    if( retx_item_ptr->pkt_ptr )
    {
      dsm_free_packet( &retx_item_ptr->pkt_ptr );
    }

    retx_item_ptr->pkt_ptr = NULL;
    retx_item_ptr->size = 0;

    /* Return the retx item back to the retx item free queue */
    hdrpcp_queue_put_item( &hdrpcp_slp_pkt_app.free_q, *retx_q_item_ptr );

    /* NULL out the reference so it can't be reused */
    *retx_q_item_ptr = NULL;
  }

} /* hdrpcp_slp_pkt_app_free_retx_queue_item() */

/*===========================================================================

FUNCTION HDRPCP_SLP_PKT_APP_TX_STATUS_CB

DESCRIPTION
  This callback is used to inform the SLP packet application of the previous
  subframe's transmission status.

DEPENDENCIES
  None.

PARAMETERS
  combo_id            - The combo ID of the chosen combo
  carrier_tx_outcome  - A list of transmitted sets and their respective statuses
  num_sets            - The number of transmitted sets
  user_data_ptr       - Pointer to the control block

RETURN VALUE
  None.

SIDE EFFECTS
  If the tranmission was not successful, the sent packets will be moved
  into the retransmit buffer for the upcoming subframe.

===========================================================================*/
static void hdrpcp_slp_pkt_app_tx_status_cb
(
  uint8 combo_id,
  hdrpcp_carrier_tx_outcome_type carrier_tx_outcome[],
  uint8 num_outcomes,
  void *user_data_ptr
)
{
  hdrpcp_slp_pkt_app_ctrl_blk_info_type *cb_ptr = 
    (hdrpcp_slp_pkt_app_ctrl_blk_info_type *) user_data_ptr;

  hdrpcp_queue_item_type *retx_q_item_ptr = NULL;
  hdrpcp_slp_pkt_app_retx_buffer_type *retx_buf_ptr = 
    &hdrpcp_slp_pkt_app.retx_buffer;

  int combo = 0;
  int i = 0; /* The i-th carrier tx outcome */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_2( MSG_LEGACY_MED,
    "SLP tx status w/ combo_id %d, %d sets", combo_id, num_outcomes );

  if( combo_id == HDRPCP_NO_COMBO_ID_ACCEPTED )
  {
    /* [0] contains all of the messages that were attempted in the
       last subframe; calling queue_link effectively empties sent packet q[0] 
       and moves all of the messages into the retx buffer */
    hdrpcp_queue_link( 
      &retx_buf_ptr->item_q, &hdrpcp_slp_pkt_app.sent_pkt_q[0].item_q );

    /* Update the additional bytes/packets pending transmission */
    retx_buf_ptr->num_bytes += hdrpcp_slp_pkt_app.sent_pkt_q[0].num_bytes;
    retx_buf_ptr->num_pkts += hdrpcp_slp_pkt_app.sent_pkt_q[0].num_pkts;
  }
  else
  {
    /* The SLP packet app does not use packet handles and must cycle through
       all of the carrier tx outcomes to find the carrier on which it transmitted
       data; the status of the this carrier will determine if the SLP data
       was successful or canceled */
    for( i = 0; i < num_outcomes; i++ )
    {
      hdrpcp_carrier_tx_outcome_type *crr_tx_outcome = &carrier_tx_outcome[i];

      /* If the carrier is the one on which SLP sent data */
      if( crr_tx_outcome->carrier_id == cb_ptr->carrier_id )
      {
        if( crr_tx_outcome->tx_set_accepted != E_SUCCESS )
        {
          HDR_MSG_PROT_2( MSG_LEGACY_MED, "combo: %d, carrier: %d, slp packet canceled!",
            combo_id, cb_ptr->carrier_id );

          HDR_MSG_PROT_1( MSG_LEGACY_MED, "sent pkt q: %d", 
            hdrpcp_slp_pkt_app.sent_pkt_q[combo_id].item_q.count );

          /* [combo_id] contains the messages that were attempted in 
             the last subframe; calling queue_link effectively empties 
             the sent packet q[combo_id] and moves the messages into the 
             retx buffer */
          hdrpcp_queue_link(
            &retx_buf_ptr->item_q, 
            &hdrpcp_slp_pkt_app.sent_pkt_q[combo_id].item_q );

          /* Update the additional bytes/packets pending transmission */
          retx_buf_ptr->num_bytes += 
            hdrpcp_slp_pkt_app.sent_pkt_q[combo_id].num_bytes;

          retx_buf_ptr->num_pkts += 
            hdrpcp_slp_pkt_app.sent_pkt_q[combo_id].num_pkts;

          HDR_MSG_PROT_3( MSG_LEGACY_MED, "retxbytes: %d, retxpkts: %d, qcnt: %d",
            retx_buf_ptr->num_bytes, retx_buf_ptr->num_pkts, retx_buf_ptr->item_q.count );
        }

        /* No need to go through the rest of the sets */
        break;
      }
    }
  }

  /* Notify PCP is the SLP packet app has retransmit data */
  if( retx_buf_ptr->num_bytes > 0 )
  {
    hdrpcp_notify_app_has_hp_data( 
      HDRPCP_SLP_FLOW_ID, HDRPCP_SLP_FLOW_ID, user_data_ptr );
  }

  /* Reset and free the sent packet queues for each combo */
  for( combo = 0; combo < HDRPCP_MAX_COMBOS; combo++ )
  {
    hdrpcp_slp_pkt_app.sent_pkt_q[combo].num_bytes = 0;
    hdrpcp_slp_pkt_app.sent_pkt_q[combo].num_pkts = 0;

    /* For each sent packet queue, start at the first packet ..*/
    retx_q_item_ptr = 
      hdrpcp_queue_get_item( &hdrpcp_slp_pkt_app.sent_pkt_q[combo].item_q );

    /* .. and walk through the queue, freeing each individual packet */
    while( retx_q_item_ptr )
    {
      hdrpcp_slp_pkt_app_free_retx_queue_item( &retx_q_item_ptr );
      retx_q_item_ptr = 
        hdrpcp_queue_get_item( &hdrpcp_slp_pkt_app.sent_pkt_q[combo].item_q );
    }
  }

} /* hdrpcp_slp_pkt_app_tx_status_cb() */

/*===========================================================================

FUNCTION HDRPCP_SLP_PKT_APP_GET_DATA_SIZE_CB

DESCRIPTION
  This function returns the number of bytes pending transmission for this
  subframe.

DEPENDENCIES
  None.

PARAMETERS
  num_pkts_ptr   - Pointer to the number of packets pending
  user_data_ptr  - Pointer to the control block

RETURN VALUE
  uint32  - The number of bytes pending

SIDE EFFECTS
  None.

===========================================================================*/
static uint32 hdrpcp_slp_pkt_app_get_data_size_cb
(
  uint32 *num_pkts_ptr,
  void *user_data_ptr
)
{
  hdrpcp_slp_pkt_app_ctrl_blk_info_type *cb_ptr = 
    (hdrpcp_slp_pkt_app_ctrl_blk_info_type *) user_data_ptr;

  uint32 num_bytes = 0;
  uint32 num_pkts = 0;
  uint16 time_since_prev_req = cb_ptr->time_since_prev_req;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  
  /* Check the SLP retx buffer next; if it is in use then we
     should also include the number of packets and bytes pending in the
     retx buffer */
  if( hdrpcp_slp_pkt_app.retx_buffer.num_pkts > 0 )
  {
    num_bytes += hdrpcp_slp_pkt_app.retx_buffer.num_bytes;
    num_pkts += hdrpcp_slp_pkt_app.retx_buffer.num_pkts;
    HDR_MSG_PROT_2(MSG_LEGACY_MED, "DEBUG: slp HP data %d, Shouldn't we upate slp timere here? data pkt size returned %d",
                   hdrpcp_slp_pkt_app.retx_buffer.num_pkts, num_bytes);
  }
  else
  {
    /* Call into SLP to get the current pending information */
    num_bytes = hdrslp_get_tx_queue_size( &num_pkts, time_since_prev_req );
    HDR_MSG_PROT_3(MSG_LEGACY_MED, "=TX= slp HP data %d, now checking LP size %d data pkt size returned %d",
                   hdrpcp_slp_pkt_app.retx_buffer.num_pkts,
                   time_since_prev_req,
                   num_bytes); 
  }

  *num_pkts_ptr = num_pkts;
  return num_bytes;

} /* hdrpcp_slp_pkt_app_get_data_size_cb() */

/*===========================================================================

FUNCTION HDRPCP_SLP_PKT_APP_GET_LP_DATA_CB

DESCRIPTION
  This callback is used to collect the low priority (or new transmit) data
  from SLP.

DEPENDENCIES
  None.

PARAMETERS
  combo_id       - The combo ID for which this transmission is intended
  carrier_id     - The carrier ID on which this transmission will be sent
  max_data_size  - The maximum data size allowed
  tx_data_ptr    - A pointer to the SLP packet
  tx_set_id      - A pointer to the transmit handle (unused by SLP)
  user_data_ptr  - Pointer to the control block

RETURN VALUE
  uint16  - The size of the SLP packet

SIDE EFFECTS
  None.

===========================================================================*/
static uint16 hdrpcp_slp_pkt_app_get_lp_data_cb
(
  uint8 combo_id, // assume 0-3
  uint8 carrier_id, // assume 0-2
  uint16 max_data_size,
  dsm_item_type **tx_data_ptr,
  void **tx_set_id,
  void *user_data_ptr
)
{
  hdrpcp_slp_pkt_app_ctrl_blk_info_type *cb_ptr = 
    (hdrpcp_slp_pkt_app_ctrl_blk_info_type *) user_data_ptr;
  
  hdrhai_channel_enum_type channel = cb_ptr->channel;
  uint16 time_since_prev_req = cb_ptr->time_since_prev_req;
  boolean first_call_to_slp = cb_ptr->first_call_to_slp;
  boolean carrier_id_known = cb_ptr->carrier_id_known;

  dsm_item_type *pkt_ptr = NULL;
  hdrpcp_queue_item_type *retx_q_item_ptr = NULL;
  hdrpcp_slp_pkt_app_retx_item_type *retx_item_ptr = NULL;

  uint16 bytes_to_tx = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_2( MSG_LEGACY_LOW, 
    "=TX= SLP new tx on carrier %d, combo %d", carrier_id, combo_id );

  ASSERT(( combo_id < HDRPCP_MAX_COMBOS ) &&
         ( carrier_id < HDRPCP_MAX_CARRIERS ));

  /* Store the carrier ID if unknown */
  if( !carrier_id_known )
  {
    cb_ptr->carrier_id = carrier_id;
    cb_ptr->carrier_id_known = TRUE;
  }
  else /* assert that we are using the same carrier ID */
  {
    ASSERT( carrier_id == cb_ptr->carrier_id );
  }

  /* A combo ID of 0 is the first combination to be built */
  if( combo_id == 0 )
  {
    /* Store the the first combo ID given */
    cb_ptr->combo_id = combo_id;

	HDR_MSG_PROT_6( MSG_LEGACY_MED,
		"=TX= max size: %d, time_since: %d, first: %d, channel: %d carrier %d combo %d",
		max_data_size, time_since_prev_req, first_call_to_slp, channel, carrier_id, combo_id);

    /* This function doubles as a wrapper to SLP's get_pkt() call; for
       combo 0, retrieve the message from SLP; for all successive combos,
       refer to combo 0's sent packet queue. This behavior assumes that
       combo 0 can hold the most messages. Time info has already been 
       passed down to SLP in hdrpcp_slp_pkt_app_get_data_size_cb() */
    bytes_to_tx = hdrslp_get_pkt( 
      first_call_to_slp, max_data_size, 0, &pkt_ptr, &channel );
  }
  else /* each combo i will refer to combo 0 for data from SLP */
  {
   HDR_MSG_PROT_2( MSG_LEGACY_MED, 
                   "=TX= SLP new tx on carrier %d, combo %d", carrier_id, combo_id );
    if( cb_ptr->combo_id == combo_id )
    {
      /* The combo ID is the same so continue the iteration through
         combo 0's sent packet queue */
      retx_q_item_ptr = hdrpcp_queue_walk( 
        &hdrpcp_slp_pkt_app.sent_pkt_q[0].item_q, HDRPCP_QUEUE_CONTINUE );
    }
    else
    {
      /* The combo ID is different so restart the iteration of
         combo 0's sent packet queue at the beginning */
      retx_q_item_ptr = hdrpcp_queue_walk( 
        &hdrpcp_slp_pkt_app.sent_pkt_q[0].item_q, HDRPCP_QUEUE_BEGIN );

      /* Store this combo ID for the next call */
      cb_ptr->combo_id = combo_id;
    }
   
    if( retx_q_item_ptr )
    {
      retx_item_ptr = 
        (hdrpcp_slp_pkt_app_retx_item_type *) (retx_q_item_ptr->data);

      if( max_data_size >= retx_item_ptr->size )
      {
        pkt_ptr = retx_item_ptr->pkt_ptr;
        bytes_to_tx = retx_item_ptr->size;
      }
      else
      {
        HDR_MSG_PROT_2( 
          MSG_LEGACY_HIGH, "%d bytes not enough for %d bytes pending",
          max_data_size, retx_item_ptr->size );

        bytes_to_tx = 0;
      }
    }
    else
    {
      bytes_to_tx = 0;
    }
  }

  /* Only if SLP returns data */
  if( bytes_to_tx > 0 )
  {
    HDR_MSG_PROT_2( MSG_LEGACY_MED,
      "giving %d bytes to PCP, first: %d", bytes_to_tx, first_call_to_slp );

    /* Provide the original packet to RMAC */
    *tx_data_ptr = pkt_ptr; // revisit

    /* Create a new retx item with by duping original DSM item; RMAC 
       will free any DSM items regardless of succesful or cancelled frames  */
    retx_q_item_ptr = hdrpcp_slp_pkt_app_new_retx_queue_item( pkt_ptr, bytes_to_tx );
    if( retx_q_item_ptr )
    {
      /* Put the retx item into the sent packet queue for this combo */
      hdrpcp_queue_put_item( 
        &hdrpcp_slp_pkt_app.sent_pkt_q[combo_id].item_q, retx_q_item_ptr );

      hdrpcp_slp_pkt_app.sent_pkt_q[combo_id].num_bytes += bytes_to_tx;
      hdrpcp_slp_pkt_app.sent_pkt_q[combo_id].num_pkts ++;
    }
    else
    {
      HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
        "Out of SLP retx items! %d not enough", HDRPCP_SLP_MAX_RETX_ITEMS );
      *tx_data_ptr = NULL;
      bytes_to_tx = 0;
    }
  }
  else /* there was no data to send */
  {
    *tx_data_ptr = NULL;
  }

  if( first_call_to_slp )
  {
    /* Any successive calls to SLP this frame will no longer be the first; the
       'first' call status is reset on every get_packet_info() update */
    cb_ptr->first_call_to_slp = FALSE;

    /* The time_since_prev_req is in units of 26.6ms, so for all successive calls
       to SLP this frame the value must be set to 0; similar to the 'first' call
      status, this value will be reset on the next get_packet_info() update */
    cb_ptr->time_since_prev_req = 0;
  }

  /* Return a NULL data handle to PCP; the always_notify_status flag should
     be set to TRUE for SLP so PCP handles to data are unnecessary */
  *tx_set_id = NULL;

  return bytes_to_tx;

} /* hdrpcp_slp_pkt_app_get_lp_data_cb() */

/*===========================================================================

FUNCTION HDRPCP_SLP_PKT_APP_GET_HP_DATA_CB

DESCRIPTION
  This callback is used to collect the high priority (or retransmit) data
  from SLP.

DEPENDENCIES
  None.

PARAMETERS
  combo_id       - The combo ID for which this transmission is intended
  carrier_id     - The carrier ID on which this transmission will be sent
  max_data_size  - The maximum data size allowed
  tx_data_ptr    - A pointer to the SLP packet
  tx_set_id      - A pointer to the transmit handle (unused by SLP)
  user_data_ptr  - Pointer to the control block

RETURN VALUE
  uint16  - The size of the SLP packet

SIDE EFFECTS
  None.

===========================================================================*/
static uint16 hdrpcp_slp_pkt_app_get_hp_data_cb
(
  uint8 combo_id,
  uint8 carrier_id,
  uint16 max_data_size,
  dsm_item_type **tx_data_ptr,
  void **tx_set_id,
  void *user_data_ptr 
)
{
  hdrpcp_slp_pkt_app_ctrl_blk_info_type *cb_ptr = 
    (hdrpcp_slp_pkt_app_ctrl_blk_info_type *) user_data_ptr;

  hdrpcp_slp_pkt_app_retx_buffer_type *retx_buf_ptr = 
    &hdrpcp_slp_pkt_app.retx_buffer;

  boolean carrier_id_known = cb_ptr->carrier_id_known;

  dsm_item_type *pkt_ptr = NULL;
  hdrpcp_queue_item_type *retx_q_item_ptr = NULL;
  hdrpcp_slp_pkt_app_retx_item_type *retx_item_ptr = NULL;

  uint16 bytes_to_tx = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_4( MSG_LEGACY_MED, 
    "=TX= SLP retx on carrier %d, combo %d; %d tot pkts w/ %d tot bytes pending",
    carrier_id, combo_id, retx_buf_ptr->num_pkts, retx_buf_ptr->num_bytes );

  ASSERT(( combo_id < HDRPCP_MAX_COMBOS ) &&
         ( carrier_id < HDRPCP_MAX_CARRIERS ));

  /* Store the carrier ID if unknown */
  if( !carrier_id_known )
  {
    cb_ptr->carrier_id = carrier_id;
    cb_ptr->carrier_id_known = TRUE;
  }
  else /* assert that we are using the same carrier ID */
  {
    ASSERT( carrier_id == cb_ptr->carrier_id );
  }

  /* Combo ID of 0 indicates the first combination to be built */
  if( combo_id == 0 )
  {
    /* Store the the first combo ID given */
    cb_ptr->combo_id = combo_id;

    /* Check the first item in the retransmit queue */
    retx_q_item_ptr = hdrpcp_queue_check_front_item( &retx_buf_ptr->item_q );
    if( retx_q_item_ptr )
    {
      /* Assert that there exists at least one item in the retx queue */
      retx_item_ptr = (hdrpcp_slp_pkt_app_retx_item_type *) retx_q_item_ptr->data;
      ASSERT( retx_item_ptr != NULL );

      /* If there is enough space to send the item */
      if( max_data_size >= retx_item_ptr->size )
      {
        /* Remove the item from the retx item queue */
        (void) hdrpcp_queue_get_item( &retx_buf_ptr->item_q );
  
        /* Copy the SLP packet information to local variables to prep 
           for transmission */
        /* Dupe this packet into the pkt_ptr because we will free the
           containing ReTX item */
        (void) dsm_dup_packet( &pkt_ptr, retx_item_ptr->pkt_ptr, 0, retx_item_ptr->size );
        bytes_to_tx = retx_item_ptr->size;

        /* Free the ReTX item */
        hdrpcp_slp_pkt_app_free_retx_queue_item( &retx_q_item_ptr );
    
        /* Update the retx buffer to reflect transmitting this packet */
        retx_buf_ptr->num_bytes -= bytes_to_tx;
        retx_buf_ptr->num_pkts -= 1;
  
        /* The number of packets in the retx buffer must equal the number 
           of items remaining in the sent packet queue */
        ASSERT( retx_buf_ptr->num_pkts == retx_buf_ptr->item_q.count );
  
        if( retx_buf_ptr->num_bytes == 0 && retx_buf_ptr->num_pkts == 0 )
        {
          retx_buf_ptr->in_use = FALSE;
        }
      }
      else
      {
        HDR_MSG_PROT_2( MSG_LEGACY_HIGH,
          "SLP retx error; %d size provided not enough for %d bytes to tx",
          max_data_size, retx_item_ptr->size );

        bytes_to_tx = 0;
      }
    }
  }
  else /* each combo i will refer to combo 0 for data from SLP */
  {
    if( cb_ptr->combo_id == combo_id )
    {
      /* The combo ID is the same so continue the iteration through
         combo 0's sent packet queue */
      retx_q_item_ptr = hdrpcp_queue_walk( 
        &hdrpcp_slp_pkt_app.sent_pkt_q[0].item_q, HDRPCP_QUEUE_CONTINUE );
    }
    else
    {
      /* The combo ID is different so restart the iteration of
         combo 0's sent packet queue at the beginning */
      retx_q_item_ptr = hdrpcp_queue_walk( 
        &hdrpcp_slp_pkt_app.sent_pkt_q[0].item_q, HDRPCP_QUEUE_BEGIN );

      /* Store this combo ID for the next call */
      cb_ptr->combo_id = combo_id;
    }
   
    if( retx_q_item_ptr )
    {
      retx_item_ptr = 
        (hdrpcp_slp_pkt_app_retx_item_type *) (retx_q_item_ptr->data);

      if( max_data_size >= retx_item_ptr->size )
      {
        /* Dupe this packet into the pkt_ptr because we will be freeing
           the ReTX item */
        (void) dsm_dup_packet( &pkt_ptr, retx_item_ptr->pkt_ptr, 0, retx_item_ptr->size );
        bytes_to_tx = retx_item_ptr->size;

        /* Free the ReTX item */
        hdrpcp_slp_pkt_app_free_retx_queue_item( &retx_q_item_ptr );
      }
      else
      {
        HDR_MSG_PROT_2( 
          MSG_LEGACY_HIGH, "%d bytes not enough for %d bytes pending",
          max_data_size, retx_item_ptr->size );

        bytes_to_tx = 0;
      }
    }
    else
    {
      bytes_to_tx = 0;
    }
  }

  /* Only if SLP returns data */
  if( bytes_to_tx > 0 )
  {
    /* Provide the original packet to RMAC */
    *tx_data_ptr = pkt_ptr; // revisit

    /* Create a new retx item by duping the original DSM item; RMAC 
       will free any DSM items regardless of succesful or cancelled frames */
    retx_q_item_ptr = hdrpcp_slp_pkt_app_new_retx_queue_item( pkt_ptr, bytes_to_tx );
    if( retx_q_item_ptr )
    {
      /* Put the retx item into the sent packet queue for this combo */
      hdrpcp_queue_put_item( 
        &hdrpcp_slp_pkt_app.sent_pkt_q[combo_id].item_q, retx_q_item_ptr );

      hdrpcp_slp_pkt_app.sent_pkt_q[combo_id].num_bytes += bytes_to_tx;
      hdrpcp_slp_pkt_app.sent_pkt_q[combo_id].num_pkts ++;
    }
    else
    {
      HDR_MSG_PROT_1( MSG_LEGACY_ERROR, 
        "Out of SLP retx items! %d not enough", HDRPCP_SLP_MAX_RETX_ITEMS );

      /* The high priority/retransmit path will dupe the packet before 
         sending; we will need to free this dupe if we ran out of ReTX items */
      dsm_free_packet( tx_data_ptr );

      *tx_data_ptr = NULL;
      bytes_to_tx = 0;
    }
  }
  else /* there was no data to send */
  {
    *tx_data_ptr = NULL;
  }

  /* Return a NULL data handle to PCP; the always_notify_status flag should
     be set to TRUE for SLP so PCP handles to data are unnecessary */
  *tx_set_id = NULL;

  return bytes_to_tx;

} /* hdrpcp_slp_pkt_app_get_hp_data_cb() */

/*===========================================================================

FUNCTION HDRPCP_SLP_PKT_APP_PUT_DATA_CB

DESCRIPTION
  This callback is used to pass the received connection layer packet up to
  the SLP layer.

DEPENDENCIES
  None.

PARAMETERS
  rx_data_ptr    - The SLP packet to give
  user_data_ptr  - Pointer to the control block

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrpcp_slp_pkt_app_put_data_cb
(
  dsm_item_type *rx_data_ptr,
  void *user_data_ptr
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDRPCP_ARG_NOT_USED( user_data_ptr );
  hdrslp_put_pkt( rx_data_ptr );

} /* hdrpcp_slp_pkt_app_put_data_cb() */

/*===========================================================================

FUNCTION HDRPCP_SLP_PKT_APP_INIT

DESCRIPTION
  This function performs one-time initialization of the SLP Packet Application
  and registers all necessary callbacks with PCP.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrpcp_slp_pkt_app_init
(
  void /* no arguments */
)
{
  int q_item = 0;
  int combo = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  hdrpcp_queue_init( &hdrpcp_slp_pkt_app.free_q );

  /* Populate the SLP packet app's free queue with retx items */
  for( q_item = 0; q_item < HDRPCP_SLP_MAX_RETX_ITEMS; q_item++ )
  {
    hdrpcp_slp_pkt_app.q_items[q_item].next = NULL;
    
    /* Initialize each queue item's data to point to an SLP retx item */
    hdrpcp_slp_pkt_app.q_items[q_item].data = 
      &hdrpcp_slp_pkt_app.retx_items[q_item];

    /* Put each item into the free queue */
    hdrpcp_queue_put_item( 
      &hdrpcp_slp_pkt_app.free_q, 
      &hdrpcp_slp_pkt_app.q_items[q_item] );
  }

  /* Initialize the sent packet queues */
  for( combo = 0; combo < HDRPCP_MAX_COMBOS; combo++ )
  {
    hdrpcp_slp_pkt_app.sent_pkt_q[combo].num_bytes = 0;
    hdrpcp_slp_pkt_app.sent_pkt_q[combo].num_pkts = 0;
  }

  /* Register forward link callbacks with PCP */
  (void) hdrpcp_register_receive_callbacks( 
    HDRPCP_SLP_FLOW_ID, 
    hdrpcp_slp_pkt_app_put_data_cb, 
    NULL, /* no end of slot notification callback */
    NULL,
    (void *) &hdrpcp_slp_pkt_app.ctrl_blk );

  /* Register reverse link callbacks with PCP */
  (void) hdrpcp_register_transmit_callbacks(
    HDRPCP_SLP_FLOW_ID,
    HDRPCP_SLP_FLOW_ID,
    HDRPCP_SLP_FLOW_ID,
    hdrpcp_slp_pkt_app_get_data_size_cb,
    hdrpcp_slp_pkt_app_get_hp_data_cb,
    hdrpcp_slp_pkt_app_get_lp_data_cb,
    hdrpcp_slp_pkt_app_tx_status_cb,
    NULL, /* no tx failed callback */
    TRUE, /* always notify tx status */
    (void *) &hdrpcp_slp_pkt_app.ctrl_blk );

} /* hdrpcp_slp_pkt_app_init() */

/*===========================================================================

FUNCTION HDRPCP_SLP_PKT_APP_INIT_CTRL_BLK

DESCRIPTION
  This function should be called at the beginning of every subframe to 
  properly initialize and dependent instance variables for the SLP
  Packet Application.

DEPENDENCIES
  Must be called by PCP at the beginning of every subframe.

PARAMETERS
  channel              - The channel on which SLP will transmit data
  time_since_prev_req  - The number of PN ROLLs that have triggered

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrpcp_slp_pkt_app_init_ctrl_blk
( 
  hdrhai_channel_enum_type channel,
  uint16 time_since_prev_req
)
{
  hdrpcp_slp_pkt_app_ctrl_blk_info_type *cb_ptr = &hdrpcp_slp_pkt_app.ctrl_blk;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the control block values */
  cb_ptr->channel = channel;
  cb_ptr->first_call_to_slp = TRUE;
  cb_ptr->time_since_prev_req = time_since_prev_req;

  cb_ptr->combo_id = 0xFF;
  cb_ptr->carrier_id = 0xFF;
  cb_ptr->carrier_id_known = FALSE;
  
} /* hdrpcp_slp_pkt_app_init_ctrl_blk() */

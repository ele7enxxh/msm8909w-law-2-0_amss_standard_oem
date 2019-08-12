#ifndef HDRRLPRTX_H
#define HDRRLPRTX_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
              H D R  R A D I O   L I N K   P R O T O C O L
              R E X M I T   Q U E U E   P R O C E S S I N G

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  
DESCRIPTION
  

 Copyright (c) 2007 - 2012 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrrlprtx.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ 
  $Author: mplcsds1 $

when       who        what, where, why
--------   ---        -------------------------------------------------------
03/14/12   msh        Del hdrrlprtx_init. Functionality shifted to dsrlp_init 
                       in order to initialize queues only once on power up.
11/18/11   vpk        HDR SU API cleanup
06/11/07   spn        Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"

#ifdef FEATURE_HDR

#include "dsrlpi.h"
#include "hdrrlptxq.h"


/*===========================================================================

             PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*
 *  Enum to differentiate the reason for creating this Retransmit Item
 *  NULL:  Invalid Enum
 *  NAK:   Entry because of a NAK
 *  MARQ:  Because of MARQ'ed Data
 *  FALSE: Because of a higher priority returning CANCEL on pkt status. 
 */
typedef enum
{
  HDRRLPRTX_NULL   = 0,
  HDRRLPRTX_NAK_RTX    = 1,
  HDRRLPRTX_MARQ_RTX   = 2,
  HDRRLPRTX_FALSE_RTX  = 3  
} hdrrlprtx_reason_enum_type;

/*
 * The info for each frame to rexmit is stored in the following struct. 
 * 
 * link_next --> To make it part of the queue
 * first_seq --> first seq num to rexmit ( inclusive )
 * last_seq --> (last seq num + 1) to rexmit
 * next_seq_tx_offset --> This is the offset from the first seq, 
 *          that has to be transmitted on the next opportunity. 
 *          Maintained per combo.  
 * tx_queue_ptr --> Pointer to entry in transmitted queue 
 * reason  --> Reason for creating this entry.
 */
typedef struct 
{
  q_link_type   link_next;
  uint32        first_seq;        
  uint32        last_seq;       
  uint32        next_seq_tx_offset[HDRCP_PCP_MAX_COMBOS];      
  hdrrlprtx_reason_enum_type reason;                               
  void*         tx_queue_item_ptr;   
} hdrrlprtx_type;


/*
 * This is a typedef is used to store information required to identify a
 * particular retransmit element that has been given to PCP for transmission.
 *
 * first_seq -->  Sequence number of the first byte in this handle
 * pkt_copy_len --> length of the the packet given to PCP
 * head_item_ptr --> This dsm item is added to facilitate the RLP loggig 
 *                   mechanism. The dsm item will be duped and saved on 
 *                   head_item_ptr, which will be freed when PCP calls 
 *                   packet status callback returning the handle
 * set_id --> used to store the set id that gives information about the
 *            combo id, carrier_id, marq_id and marq_index
 */
typedef struct
{
  uint32   first_seq;
  uint16   pkt_copy_len; 
  hdrrlprtx_type *retx_item_ptr;
  dsm_item_type   *head_item_ptr;
  uint32 set_id; /*cannot use the type hdrrlptxsh_handle_type because it is defined in a .h file in src folder*/
} hdrrlprtx_pkt_handle_type;

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================

FUNCTION       HDRRLPRTX_HAS_REXMITS

DESCRIPTION    Returns TRUE if there are retransmit bytes to be sent.
                                           
DEPENDENCIES   NONE

RETURN VALUE   TRUE if naks to xmit, else FALSE

SIDE EFFECTS   None.

===========================================================================*/
boolean hdrrlprtx_has_rexmits
(
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr,
  uint8 combo_index
);

/*===========================================================================

FUNCTION       HDRRLPRTX_POWERUP_INIT

DESCRIPTION    Called at initialization
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void hdrrlprtx_powerup_init
(
  void
);

/*===========================================================================

FUNCTION       HDRRLPRTX_RESET

DESCRIPTION    Called when an RLP instance is reset.  This takes any rexmit
               queue entries and puts them back into the global free q.
               
DEPENDENCIES   rrcb_ptr != NULL

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void hdrrlprtx_reset
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr         /* current RLP ctl blk        */
);

/*===========================================================================

FUNCTION       HDRRLPRTX_PUT_REXMIT_INFO

DESCRIPTION    Called by the Rx/Tx task when a Retransmit needs to happen.  
               This function creates an entry for the rexmit list.
               
               Retransmit entries can be put because of the following reasons
               1. NAKs received
               2. MARQs happening in the TX task
               3. FALSE RETX's happening because of 3 carriers 
               
               Items are not sorted; they are always put at the end of the
               list.
               
DEPENDENCIES   rrcb_ptr != NULL

RETURN VALUE   TRUE  - if the element could be added to rexmit list.
               FALSE - if the element could not be added to the rexmit list.

SIDE EFFECTS   NONE
===========================================================================*/
boolean hdrrlprtx_put_rexmit_info
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,        /* current RLP ctl blk        */
  uint32                    first_seq,       /* first seq num to xmit      */
  uint32                    last_seq,        /* last seq num to xmit       */
  hdrrlprtx_reason_enum_type reason,
  void*                     tx_queue_ptr   /* Pointer to entry in 
                                                       transmitted queue   */
);

/*===========================================================================

FUNCTION       HDRRLPRTX_GET_DATA_SIZE

DESCRIPTION    Gets data size from RLP retransmission list.
               
DEPENDENCIES   NONE

RETURN VALUE   RLP Retransmission list data size.
               Also returns the number of separate retransmission packet pending

SIDE EFFECTS   NONE
===========================================================================*/
uint32 hdrrlprtx_get_data_size
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,        /* current RLP ctl blk        */
  uint32 *num_retx_pkts
);

/*===========================================================================

FUNCTION       HDRRLPRTX_PACKET_STATUS

DESCRIPTION    Update packet status of the retransmission list.
               TRUE if the packet was accepted.
               FALSE if the packet was cancelled.
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void hdrrlprtx_packet_status
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,       /* current RLP ctl blk         */
  hdrrlprtx_pkt_handle_type   *tx_data_handle, /* handle to determine packet  */
  boolean                  status,          /* packet status               */
  uint8 combo_index
);

/*===========================================================================

FUNCTION       HDRRLPRTX_GET_INFO_NEXTFIRST_RETX_FRAME

DESCRIPTION    Returns information for the retransmit frame, that can be 
               built next.
               
DEPENDENCIES   NONE

RETURN VALUE   TRUE is there are elements in the txq and info is returned
               FALSE if there is no information returned. 

SIDE EFFECTS   NONE
===========================================================================*/
hdrrlprtx_type* hdrrlprtx_get_info_next_retx_frame
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,
  uint8  combo_index
);

/*===========================================================================

FUNCTION       HDRRLPRTX_REMOVE_ITEM

DESCRIPTION    Removes the specified item from the retransmit queue               
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void hdrrlprtx_remove_item
(
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr,
  hdrrlprtx_type *retx_ptr
);

/*===========================================================================

FUNCTION       HDRRLPRTX_GET_HANDLE_INFO

DESCRIPTION    Parses the RETX handle info to return the first and last 
               seq numbers that is pointed to by this handle. Also returns
               the number of data units represented by the handle.
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void hdrrlprtx_get_handle_info
(
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr,
  hdrrlprtx_pkt_handle_type *retx_handle_info,
  uint32 *first_seq,
  uint32 *last_seq,
  uint32 *num_data_units
);

/*===========================================================================

FUNCTION HDRRLPRTX_GET_RTXQ_SIZE

DESCRIPTION
  Called to return the current size of the Retransmit Queue

DEPENDENCIES
  None.

RETURN VALUE
  Number of Items in the retransmit Queue

SIDE EFFECTS
===========================================================================*/
uint16 hdrrlprtx_get_rtxq_size
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr
);

#endif /* FEATURE_HDR */
#endif /* HDRRLPRTX_H */


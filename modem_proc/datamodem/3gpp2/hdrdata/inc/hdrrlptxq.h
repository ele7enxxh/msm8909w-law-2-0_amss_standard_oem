#ifndef HDRRLPTXQ_H
#define HDRRLPTXQ_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               H D R   R A D I O   L I N K   P R O T O C O L  
               
      T R A N S M I T T E D   Q U E U E   M A N A G E M E N T   M O D U L E

GENERAL DESCRIPTION
  
EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  
 Copyright (c) 2007-2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrrlptxq.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when         who        what, where, why
--------     ---        -------------------------------------------------------
07/04/14     sc         Optimize the code to reduce the static memory
09/04/09     ss         CMI SU level modifications.
08/18/08     spn        Added support to MARQ flush packets
02/14/07     sy         Created module.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

#include "dsrlpi.h"
#include "rex.h"
#include "hdrrlp_api.h"

#ifdef FEATURE_HDR

#define HDRRLPTXQ_TOTAL_NUM_ITEMS  2728 

/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/
typedef struct hdrrlptxq_fr_param_type
{
  q_link_type  link_next;                   /*Next pointer to the link list*/
  uint32 timestamp;                         /*Timestamp noted when the packet is
                                              placed in the transmitted queue.*/
  dsm_item_type *dsm_chain_ptr;             /*Pointer to the DSM chain that 
                                              contains the RLP transmitted pkt*/
  uint32 seq_num;                           /*32 bit seq number of the RLP pkt*/  
}hdrrlptxq_type;

/*===========================================================================

                        FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION hdrrlptxq_powerup_init

DESCRIPTION
  Called to initialize the queue and place all the free items of the pool in
  the transmitted queue.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void hdrrlptxq_powerup_init
(
  void
);

/*===========================================================================

FUNCTION HDRRLPTXQ_GET_NEW_TXQ_ITEM

DESCRIPTION  Gets an item from the global txq pool.

  The timestamp on each RLP transmitted frame is saved.
  The timer value is set to value of the expiry time based on the NAK Abort 
  timer negotiated for that RLP.
  timestamp_saved_in the_RLP_tranmsitted_frame = current_timestamp 
                                    + Nak_Abort_timer_negotiated_for_that_RLP.

  RLP Nak Abort timer is different for each RLP from RevA onwards. The default
  value is 500msec.

  If there is atleast one item transmitted then the timer need to be started.
  The timer can be started for the least value of the timer needed for all 
  the RLP's.

  TODO: Just start the timer for the smallest timer value negotiated for the 
        RLP abort timer. See which one is better interms of CPU.


  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
hdrrlptxq_type* hdrrlptxq_new_txq_item
(
   dsrlpi_rlp_ctl_blk_type *rrcb_ptr
);

/*===========================================================================

FUNCTION HDRRLPTXQ_FREE_TXQ_ITEM

DESCRIPTION  Placws the item back in the global txq pool.
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void hdrrlptxq_free_txq_item
(
 hdrrlptxq_type *free_item
);

/*===========================================================================

FUNCTION      HDRRLPTXQ_ADD_FRAME

DESCRIPTION   This function adds a new frame into the transmit queue. This
              function is called whenever RLP sends a new frame. It runs in 
              the context of the task that does RLP TX.
              
DEPENDENCIES  Called from Tx task.

RETURN VALUE  None

SIDE EFFECTS    
===========================================================================*/
void hdrrlptxq_add_frame
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,    /* current RLP ctl blk        */
  hdrrlptxq_type               *fr_param_ptr /* Info. of fr. to be inserted*/
);

/*===========================================================================

FUNCTION      HDRRLPTXQ_LOCATE_FRAME

DESCRIPTION   Given a sequence number, this returns a pointer to the
              requested RLP frame.  The user can than access the internal
              contents of the RLP frame.  It is assumed that the following
              remains unchanged:
              
                RLP frame's sequence number
                The dsm item's seq_num field is unchanged
                
              The function is passed three important params:
              
                rscb_ptr    - pointer to the session control block
                
                fr_info_ptr - the seq_num field needs to contain the
                              requested sequence number.  All other fields
                              are ignored, and are filled in when the
                              appropriate RLP frame is found.
                              
                next_seq_num - filled in if a frame is found.  no input
                               value required.
                
DEPENDENCIES  Called from Tx task.  Also see conditions stated above.

RETURN VALUE  If the packet was found, then returns a non-NULL value
              which contains the relevant RLP frame. 
              
              next_seq_num is loaded with the seq_num of the next element
              in the txq.  If there is no next element, then next_seq_num
              is set equal to the requested sequence number.
              
              If the packet was not found, it returns NULL.  
              
              If the queue is in fact empty, then next_seq_num is unchanged.

SIDE EFFECTS  NONE
===========================================================================*/
hdrrlptxq_type* hdrrlptxq_locate_frame
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,    /* Ptr to RLP ctl block       */
  uint32                       req_seq_num, /* points to req'd frame info */
  uint32                       *next_seq_num, /* filled with next seq num in*/
                                             /* list                       */
  boolean                      *use_next_seq_num /* if the next_seq_num is valid */
);

/*===========================================================================

FUNCTION      HDRRLPTXQ_ACCESS_LAST_FRAME

DESCRIPTION   Returns a pointer to the last RLP frame transmitted.  
              The user can than access the internal contents of the RLP 
              frame.  It is assumed that the following remains unchanged:
              
                RLP frame's sequence number
                The dsm item's seq_num field is unchanged
              
DEPENDENCIES  Called from Tx task.  Also see conditions stated above.
              Caller does not change the frame that is returned

RETURN VALUE  If the packet was found, then returns a non-zero value, 
              which indicates the length of the payload in the returned
              dsm packet chain (which contains the relevant RLP frame). 
              
              The passed fr_info_ptr is completely filled out with the info
              from the found dsm packet chain. The fr_info_ptr struct 
              contains a pointer to the head of the dsm pkt chain.
              
              If the packet was not found, the function returns zero.  

SIDE EFFECTS  
 None.
===========================================================================*/
uint16 hdrrlptxq_access_last_frame
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,    /* Ptr to RLP ctl block       */
  hdrrlptxq_type               **fr_info_ptr  /* points to req'd frame info */
);

/*===========================================================================

FUNCTION      HDRRLPTXQ_RESET
             
DESCRIPTION   This goes ahead and clears out a transmit q of all of its
              items.  Called from dsrlpi_reset() if RLP is reset or gotten
              rid of.  

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void hdrrlptxq_reset
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr         /* current RLP ctl blk        */
);

/*===========================================================================

FUNCTION HDRRLPTXQ_GET_INFO

DESCRIPTION
  Called to return the information first and last seq numbers from a 
  txq item  

DEPENDENCIES
  None.

RETURN VALUE
  None.  

SIDE EFFECTS
===========================================================================*/
void hdrrlptxq_get_info
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,
  hdrrlptxq_type *txq_ptr,
  uint32 *first_seq,
  uint32 *last_seq,
  uint32 *length
);

/*===========================================================================

FUNCTION HDRRLPTXQ_GET_TXQ_SIZE

DESCRIPTION
  Called to return the current size of the TX Queue

DEPENDENCIES
  None.

RETURN VALUE
  Number of Items in the TX Queue

SIDE EFFECTS
===========================================================================*/
uint16 hdrrlptxq_get_txq_size
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr
);

#endif /* FEATURE_HDR */

#endif

#ifndef HDRRLPRSQ_H
#define HDRRLPRSQ_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  R A D I O   L I N K   P R O T O C O L  R E S E Q U E N C I N G  Q U E U E  
  
                          P R O C E S S I N G  

GENERAL DESCRIPTION
  
  This header file contains HDR-specific RSQ-list processing functions for the 
  RLP software.

  It is assumed that the user will not directly access any RSQ information
  contained in the RSQ structure.  Instead, all RSQ access can be done
  via the externalized functions or macros provided in this file and the
  dsrlprseq.h file.

  The  resequencing queue is simply a queue of small dsm items.  This takes
  advantage of the embedded q_link_type inside each dsm item.
  
  This module consists of four functions:

  hdrrlprsq_reset() is called from within dsrlpi_reset() each time an RLP
  instance resets. It will take any resequencing queue entries with sequence
  numbers up until LV(N) and convey them to upper layers. Any remaining items
  in the queue will be freed.
 
  hdrrlprsq_remove() removes as many items as possible from the
  resequencing queue, subject to the sequence numbers of items
  relative to LV(N). So it will keep removing until it finds a dsm
  item chain with sequence number greater than LV(N).
 
  It is called from within hdrrlp_process_rx_frames(),
  hdrrlpi_process_rexmit_frame(), hdrrlprsq_reset(), and hdrrlprsq_insert(). 
  Users may optimize the value of maximum chains to remove with
  DSRLPI_MAX_CHAINS_TO_REMOVE or set it to zero which means the only
  restriction to removing items from the resequencing queue is their sequence
  number relative to LV(N). 
  
  hdrrlprsq_insert() is the function that inserts out-of-sequence frames into
  the resequencing queue. Inserts are made according to the sequence numbers
  of the items to be inserted as long as the sequence numbers are equal or
  greater to LV(N). Whenever possible this function will attempt to append to
  an existing dsm item chain in the resequencing queue instead of inserting a
  new dsm item in order to minimize the size of the queue. For an append to
  happen though, the sequence numbers need to be sequential. This function
  also makes sure that there is no repetition and it can discard an incoming
  frame if it is already in the queue and also replace an existing dsm item
  chain or even a combination of dsm item chains if they can be replaced by a
  new incoming frame.

EXTERNALIZED FUNCTIONS

  void hdrrlprsq_reset (dsrlpi_rlp_ctl_blk_type *rrcb_ptr);

  boolean hdrrlprsq_insert(dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,     
                           uint32                    seq_num,
                           boolean                   first_data_unit,
                           boolean                   last_data_unit,
                           dsm_item_type            *fwd_pkt_ptr
                          );

  void hdrrlprsq_remove(dsrlpi_rlp_ctl_blk_type *rrcb_ptr);

  void hdrrlprsq_update_vn(dsrlpi_rlp_ctl_blk_type *rrcb_ptr);

INITIALIZATION AND SEQUENCING REQUIREMENTS
  -- RLP configuration and initialization procedures.
  
  -- Resequencing queue initialization through call to hdrrlprsq_init() for
  each separate rlp session.

 Copyright (c) 2005-2012 by Qualcomm Technologies Incorporated.  All Rights Reserved.
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrrlprsq.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        -------------------------------------------------------
03/14/12   msh        Deleted hdrrlprsq_init() function. reseq_q is now 
                      initialzed in dsrlp_init()
04/08/08   hs         Fixed the problem with improper logging for packet 0x12A5
06/06/06   ksu        EMPA: RLP Rx / Sig code review comments
11/15/05   ksu        EMPA: packet stream, octet sequencing (forward link also)

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"

#ifdef FEATURE_HDR
#include "dsrlpi.h"



/*===========================================================================

             PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/


/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                      FUNCTION DEFINITIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/

/*===========================================================================

FUNCTION       HDRRLPRSQ_RESET

DESCRIPTION    Called when an RLP instance is reset.  This takes any Reseq Q
               entries up until LV(N) and dsm_enqueue() then  into then rx
               watermark structure. From that point on it sets free any other
               remaining dsm item chains.

PARAMETERS     rrcb_ptr: pointer to rlp control block containing RSQ
               
DEPENDENCIES   rrcb_ptr != NULL

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
extern void hdrrlprsq_reset
(
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr                 /* current RLP ctl blk */
);

/*===========================================================================

FUNCTION       HDRRLPRSQ_INSERT

DESCRIPTION    Insert this new entry into the ReSeq Queue. Inserted based on
               sequence number. It also takes care of the cases there is 
               overlap or partial overlap in the sequence number ranges.
               
               If entry already exists, then nothing is done.

PARAMETERS     rrcb_ptr:         pointer to RLP control block
               seq_num:          current sequence number
               first_data_unit:  this is the first data unit
               last_data_unit:   this is the last data unit
               fwd_pkt_ptr:      dsm item to insert
               qn_seq_included   to indicate if Qn sequence was included           
               
DEPENDENCIES   Inputs rscb_ptr, frame_info_ptr, fr_ptr  are  non-null.
               
RETURN VALUE   TRUE if the packet was inserted; FALSE if discarded (e.g. duplicate)

SIDE EFFECTS   NONE
===========================================================================*/
extern boolean hdrrlprsq_insert
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,          /* curr RLP ctl blk         */
  uint32                    seq_num,           /* current sequence number  */
  boolean                   first_data_unit,   /* first data unit          */
  boolean                   last_data_unit,    /* last data unit           */
  dsm_item_type            *fwd_pkt_ptr,        /* the frame to insert      */
  boolean                   qn_seq_included,    /* To see if Qn seq was included 
                                                to be able to update the log packet
                                                statistics correctly*/
  boolean                   is_recursive
);

/*===========================================================================

FUNCTION HDRRLPRSQ_REMOVE

DESCRIPTION
  Removes chains of DSM items from the resequencing queue.  Calls registered 
  callback function to perform the handing of RLP packets to the higher layers.
  
DEPENDENCIES
  None

PARAMETERS
  rrcb_ptr      - pointer to current RLP control block

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrrlprsq_remove
(
  dsrlpi_rlp_ctl_blk_type     *rrcb_ptr                /* curr RLP ctl blk */
);

/*===========================================================================

FUNCTION HDRRLPRSQ_UPDATE_VN_UPON_NAK_ABORT

DESCRIPTION
  Updates V(n) beyond the last contiguous data unit available in the reseq_q
  
DEPENDENCIES
  None

PARAMETERS
  rrcb_ptr      - pointer to current RLP control block

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrrlprsq_update_vn_upon_nak_abort
(
  dsrlpi_rlp_ctl_blk_type     *rrcb_ptr                /* curr RLP ctl blk */
);

/*===========================================================================

FUNCTION HDRRLPRSQ_GET_SEQ_NUM

DESCRIPTION
  Returns the starting seq number of an element of the reseq_q
  
DEPENDENCIES
  None

PARAMETERS
  Pointer to dsm_item stored in reseq_q

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern uint32 hdrrlprsq_get_seq_num
(
  dsm_item_type *reseq_item_ptr
);

/*===========================================================================

FUNCTION HDRRLPRSQ_GET_NUM_DATA_UNITS

DESCRIPTION
  Returns number of data units in an element of the reseq_q
  
DEPENDENCIES
  None

PARAMETERS
  Pointer to dsm_item stored in reseq_q

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern uint32 hdrrlprsq_get_num_data_units
(
  dsm_item_type *reseq_item_ptr
);

/*===========================================================================

FUNCTION      HDRRLPRSQ_COMPUTE_NUM_DATA_UNITS 

DESCRIPTION    Computes the number of data units based on RLP version and config
           
PARAMETERS     cfg_ptr: Configuration of RLP
               
DEPENDENCIES   None.

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/

uint32 HDRRLPRSQ_COMPUTE_NUM_DATA_UNITS
(
  const dsrlpi_cfg_info_type* cfg_ptr,  /* reference config info           */
  dsm_item_type *rx_item_ptr
);

#endif /* FEATURE_HDR */
#endif /* HDRRLPRSQ_H */

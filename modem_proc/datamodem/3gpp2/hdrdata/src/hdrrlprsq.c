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
 
  hdrrlprsq_init() is called once from within dslrp_init() to initialize the
  resequencing queue at the beginning of an RLP session. 
 
  hdrrlprsq_reset() is called from within dsrlpi_reset() each time an RLP
  instance resets. It will take any resequencing queue entries with sequence
  numbers up until LV(N) and convey them to upper layers. Any remaining items
  in the queue will be freed.
 
  hdrrlprsq_insert() is the function that inserts frames into
  the resequencing queue. Inserts are made according to the sequence numbers
  of the items to be inserted as long as the sequence numbers are equal or
  greater to V(N). Whenever possible this function will attempt to append to
  an existing dsm item chain in the resequencing queue instead of inserting a
  new dsm item in order to minimize the size of the queue. For an append to
  happen though, the sequence numbers need to be sequential. This function
  also makes sure that there is no repetition and it can discard an incoming
  frame if it is already in the queue.

  hdrrlprsq_remove() removes as many items as possible from the
  resequencing queue, subject to the sequence numbers of items
  relative to LV(N). So it will keep removing until it finds a dsm
  item chain with sequence number greater than LV(N).
 

EXTERNALIZED FUNCTIONS
  
  void hdrrlprsq_init  (dsrlpi_sess_ctl_blk_type *rscb_ptr);
  
  void hdrrlprsq_reset (dsrlpi_rlp_ctl_blk_type *rrcb_ptr);

  boolean hdrrlprsq_insert(dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,
                           uint32                    seq_num,
                           boolean                   first_data_unit,
                           boolean                   last_data_unit,
                           dsm_item_type            *fwd_pkt_ptr
                          );

  void hdrrlprsq_remove(dsrlpi_rlp_ctl_blk_type *rrcb_ptr);

  void hdrrlprsq_update_vn_upon_nak_abort(dsrlpi_rlp_ctl_blk_type *rrcb_ptr);

INITIALIZATION AND SEQUENCING REQUIREMENTS
  
  --RLP configuration and initialization procedures.
  
  --Resequencing queue initialization through call to hdrrlprsq_init() for
    each separate rlp session.

 Copyright (c) 2005-2012 by Qualcomm Technologies Incorporated.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/src/hdrrlprsq.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        -------------------------------------------------------
03/14/12   msh        Deleted hdrrlprsq_init() function. reseq_q is now 
                      initialzed in dsrlp_init(). 
07/26/11   vpk        Remove unused HDR headers
02/08/11   ls         Global variable cleanup
11/10/09   as         Cleared Windows Compiler warnings for MOB builds
06/02/09   spn        Lint fix
04/23/09   spn        Re-act to the return value of dsm_split() instead of 
                      ignoring it.
03/19/09   ms         Fixed Lint errors.
06/26/08   spn        Fixed removal of duplicate/excess bytes when inserting
                      an item in the resequencing queue.
06/26/08   spn        Changed dsm_length_packet() and used a RSQ MACRO to 
                      count the number of data units, so that it works for 
                      segment sequenced flows.
04/22/08   spn        Capped the DSM Packet length, when it is enqueued to 
                      the Watermark on the Forward link
04/08/08   hs         Fixed the problem with improper logging for packet 0x12A5
03/26/08   hs         Made changes for Coalesce to function correctly.
03/07/08   da         MMPA: Changed num_data_units from uint16 to uint32
12/17/07   da         MMPA: Fixed Retx bytes stats
05/23/07   spn        Lint Fixes
05/18/07   ac         EMPA: RSP debug message enhancement
05/08/07   da         EMPA: change to avoid going through nak-q for every packet
03/28/07   ac         EMPA: fix the error for rx_total_frames 
03/06/07   ac         EMPA: fix the app_ptr clean up issue for out of order delivery
06/22/06   ksu        EMPA: RLP Rx (again) code review comments
06/06/06   ksu        EMPA: RLP Rx / Sig code review comments
05/30/06   ksu        EMPA: RLP init code review comments
02/07/06   ksu        EMPA: forward link RoHC testing
01/03/06   ksu        EMPA: hook up forward link with new phys link architecture
12/23/05   ksu        EMPA RSP: two fwd rx queues
12/04/05   ksu        EMPA segment sequencing: don't fragment for Format B
11/22/05   ksu        EMPA packet stream: segment sequencing (forward link only)
11/21/05   ksu        EMPA packet stream: out of order delivery to route protocol forward
11/15/05   ksu        EMPA: packet stream, octet sequencing (forward link also)

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "data_msg.h"

#if defined(FEATURE_HDR)
#include "hdrrlprsq.h"
#include "hdrrlpnak.h"
#include "hdrrlpdnak.h"
#include "hdrrlpmlnak.h"
#include "hdrrlpi.h"
#include "hdrds.h"
#include "dsrlp_v.h"
#include "hdrrlpseq.h"
#include "hdrrlp.h"


/*===========================================================================

             PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/



/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

#define HDRRLPRSQ_MAX_LEN_DSM_PACKET_ENQ 150000

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                        MACRO DEFINITIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/


/*===========================================================================
  Gets the sequence number from the app_field member of the dsm item
  ===========================================================================*/
INLINE uint32 HDRRLPRSQ_GET_SEQ_NUM(dsm_item_type *reseq_item_ptr)
{
  return reseq_item_ptr->app_field;
}

/*===========================================================================
  Store the sequence number in the app_field member of the dsm item
  ===========================================================================*/
INLINE void HDRRLPRSQ_SET_SEQ_NUM(dsm_item_type *reseq_item_ptr,
                                  uint32 seq_num)
{
  reseq_item_ptr->app_field = seq_num;
}

/*===========================================================================
  Get the number of data units in the app_ptr member of the dsm item
  ===========================================================================*/
INLINE uint32 HDRRLPRSQ_GET_NUM_DATA_UNITS(dsm_item_type *reseq_item_ptr)
{
  return (uint32)reseq_item_ptr->app_ptr;
}

/*===========================================================================
  Store the number of data units in the app_ptr member of the dsm item
  ===========================================================================*/
INLINE void HDRRLPRSQ_SET_NUM_DATA_UNITS(dsm_item_type *reseq_item_ptr, 
                                         uint32 num_data_units)
{
  reseq_item_ptr->app_ptr = (void*)(uint32)num_data_units;
}

/*===========================================================================
  Store the flags in the kind member of the dsm item
  ===========================================================================*/
INLINE boolean HDRRLPRSQ_GET_FIRST_DATA_UNIT(dsm_item_type *reseq_item_ptr)
{
  return (reseq_item_ptr->kind & 0x01) ? TRUE : FALSE;
}
INLINE void HDRRLPRSQ_SET_FIRST_DATA_UNIT(dsm_item_type *reseq_item_ptr, 
                                          boolean first_data_unit)
{
  if (first_data_unit)
  {
    reseq_item_ptr->kind |= 0x01;
  }
  else
  {
    reseq_item_ptr->kind &= ~0x01;
  }
}
INLINE boolean HDRRLPRSQ_GET_LAST_DATA_UNIT(dsm_item_type *reseq_item_ptr)
{
  return (reseq_item_ptr->kind & 0x02) ? TRUE : FALSE;
}
INLINE void HDRRLPRSQ_SET_LAST_DATA_UNIT(dsm_item_type *reseq_item_ptr, 
                                         boolean last_data_unit)
{
  if (last_data_unit)
  {
    reseq_item_ptr->kind |= 0x02;
  }
  else
  {
    reseq_item_ptr->kind &= ~0x02;
  }
}
INLINE boolean HDRRLPRSQ_GET_ALREADY_PASSED_UP_OUT_OF_ORDER(dsm_item_type *reseq_item_ptr)
{
  return (reseq_item_ptr->kind & 0x04) ? TRUE : FALSE;
}
INLINE void HDRRLPRSQ_SET_ALREADY_PASSED_UP_OUT_OF_ORDER(dsm_item_type *reseq_item_ptr, 
                                                         boolean already_passed_up_out_of_order)
{
  if (already_passed_up_out_of_order)
  {
    reseq_item_ptr->kind |= 0x04;
  }
  else
  {
    reseq_item_ptr->kind &= ~0x04;
  }
}

/*===========================================================================
  Determine if two packets are adjacent in sequence number space.
  Adjacent implies that the two packets could be combined, which does
  not work for certain combinations of RLP framing flags (EMPA packet stream).
  ===========================================================================*/
INLINE boolean HDRRLPRSQ_ADJACENT(dsrlpi_rlp_ctl_blk_type *rrcb_ptr,
                                  dsm_item_type *first_item_ptr,
                                  dsm_item_type *second_item_ptr)
{
  uint32 end_seq_num;

  /* EMPA/MMPA packet stream: packets can't be combined */
  if ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA || 
       rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) &&
       rrcb_ptr->cfg_ptr->packet_stream) 
  {
    /* If the first packet has LDU flag set, then can't be adjacent */
    if (HDRRLPRSQ_GET_LAST_DATA_UNIT(first_item_ptr))
    {
      return FALSE;
    }

    /* If the second packet has FDU flag set, then can't be adjacent */
    if (HDRRLPRSQ_GET_FIRST_DATA_UNIT(second_item_ptr))
    {
      return FALSE;
    }
  }

  /* If the second sequence number immediately follows the first packet, then they are adjacent */
  end_seq_num = HDRRLPSEQ_ADD_SEQ(HDRRLPRSQ_GET_SEQ_NUM(first_item_ptr),
                                   HDRRLPRSQ_GET_NUM_DATA_UNITS(first_item_ptr),
                                   rrcb_ptr->cfg_ptr->seq_modulus);
  return (HDRRLPRSQ_GET_SEQ_NUM(second_item_ptr) == end_seq_num);
}

/*===========================================================================
  Determine if two packets are overlapping in sequence number space.
  ===========================================================================*/
INLINE boolean HDRRLPRSQ_OVERLAPPING(dsrlpi_rlp_ctl_blk_type *rrcb_ptr,
                                     dsm_item_type *first_item_ptr,
                                     dsm_item_type *second_item_ptr,
                                     uint16 *overlapping_data_units_ptr,
                                     boolean *need_split_ptr,
                                     uint32 *split_offset_ptr
)
{
  uint32 first_pkt_start_seq_num;
  uint32 sec_pkt_start_seq_num;
  uint32 end_first_pkt_seq_num;
  uint32 end_sec_pkt_seq_num;
  boolean ret_val = FALSE;
  uint16 overlap = 0;
  *need_split_ptr = FALSE;
  *split_offset_ptr = 0;
  /* If the second sequence number is before than end the first packet, then they are overlapping */
  first_pkt_start_seq_num = HDRRLPRSQ_GET_SEQ_NUM(first_item_ptr);
  end_first_pkt_seq_num = HDRRLPSEQ_ADD_SEQ(first_pkt_start_seq_num,
                                   HDRRLPRSQ_GET_NUM_DATA_UNITS(first_item_ptr),
                                   rrcb_ptr->cfg_ptr->seq_modulus);

  sec_pkt_start_seq_num = HDRRLPRSQ_GET_SEQ_NUM(second_item_ptr);
  end_sec_pkt_seq_num = HDRRLPSEQ_ADD_SEQ(sec_pkt_start_seq_num,
                                   HDRRLPRSQ_GET_NUM_DATA_UNITS(second_item_ptr),
                                   rrcb_ptr->cfg_ptr->seq_modulus);

  /* If End Seq of first packet is greater than the 
     start of the first packet */
  if (HDRRLPSEQ_GT_SEQ_CHECK(end_first_pkt_seq_num,
                            sec_pkt_start_seq_num,
                            rrcb_ptr->cfg_ptr->seq_modulus))
  {
    /* If end of first packet is greater than the end of the 
       second packet, then the entire second packet is contained in 
       the first packet, overlap is equal to the length of 
       the second packet */
    if ( HDRRLPSEQ_GT_SEQ_CHECK( end_first_pkt_seq_num,
                                 end_sec_pkt_seq_num,
                                 rrcb_ptr->cfg_ptr->seq_modulus ) )
    {
      overlap = (uint16)HDRRLPRSQ_GET_NUM_DATA_UNITS(second_item_ptr);
      *need_split_ptr = TRUE;
      *split_offset_ptr = HDRRLPSEQ_DIFF_SEQ( sec_pkt_start_seq_num,
                                              first_pkt_start_seq_num, 
                                              rrcb_ptr->cfg_ptr->seq_modulus);
    }
    else
    {
      overlap = (uint16)HDRRLPSEQ_DIFF_SEQ( end_first_pkt_seq_num,
                                            sec_pkt_start_seq_num,
                                            rrcb_ptr->cfg_ptr->seq_modulus);
    }
    ret_val = TRUE;
  }
  *overlapping_data_units_ptr = overlap;
  return ret_val;
}

/*===========================================================================
  Combine two adjacent list items.
  ===========================================================================*/
INLINE void HDRRLPRSQ_COALESCE(dsrlpi_rlp_ctl_blk_type *rrcb_ptr,
                               dsm_item_type *first_item_ptr)
{
  dsm_item_type *second_item_ptr;
  hdrrlp_debug_ctl *hdrrlp_debug_ptr = NULL;

  if (first_item_ptr == NULL)
  {
    return;
  }

  hdrrlp_debug_ptr = get_hdrrlp_debug_ptr();

  second_item_ptr = q_next(&rrcb_ptr->state.reseq_q, &first_item_ptr->link);
  if (second_item_ptr == NULL)
  {
    return;
  }

  /* This if check has been included because the Kind field checked inside the method
     HDRRLPRSQ_GET_ALREADY_PASSED_UP_OUT_OF_ORDER was being used to carry the packet size info.
     As a result of this, the functions always returned TRUE and the return was executed. Hence
     no coalescing happened. This is still a problem if we have a packet stream on MMPA or EMPA */
  if (rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA || 
      rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA)
  {
    if (rrcb_ptr->cfg_ptr->packet_stream)
    {

      if (HDRRLPRSQ_GET_ALREADY_PASSED_UP_OUT_OF_ORDER(first_item_ptr) ||
	      HDRRLPRSQ_GET_ALREADY_PASSED_UP_OUT_OF_ORDER(second_item_ptr))
      {
        return;
      }
    }
  }

  if (HDRRLPRSQ_ADJACENT(rrcb_ptr, first_item_ptr, second_item_ptr))
  {

    if (hdrrlp_debug_ptr->enable_rx_debug) 
    {
      DATA_RLP_MSG8( MSG_LEGACY_MED, 
                     "RLP Rx[%d,%d] V[%d,%d] Coalescing Seq#0x%x len:0x%x and next Seq#0x%x len:0x%x",
                      rrcb_ptr->cfg_ptr->sr_id,
                      "AB"[(rrcb_ptr->route)],
                      rrcb_ptr->cfg_ptr->ver,
                      rrcb_ptr->cfg_ptr->enhanced,
                      HDRRLPRSQ_GET_SEQ_NUM(first_item_ptr), 
                      HDRRLPRSQ_GET_NUM_DATA_UNITS(first_item_ptr),
                      HDRRLPRSQ_GET_SEQ_NUM(second_item_ptr), 
                      HDRRLPRSQ_GET_NUM_DATA_UNITS(second_item_ptr) );
    }                        

    /* Merge meta-data from second packet to first */
    HDRRLPRSQ_SET_NUM_DATA_UNITS(first_item_ptr,
                                 HDRRLPRSQ_GET_NUM_DATA_UNITS(first_item_ptr) +
                                 HDRRLPRSQ_GET_NUM_DATA_UNITS(second_item_ptr));

    /* Set the app_ptr of the second item to NULL, since it won't be used anymore
       in DS and RLP. This is required by PS */
    HDRRLPRSQ_SET_NUM_DATA_UNITS(second_item_ptr, 0);

    /* EMPA/MMPA packet stream: merge data unit flags */
    if ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA || 
         rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) &&
         rrcb_ptr->cfg_ptr->packet_stream) 
    {
      if (HDRRLPRSQ_GET_LAST_DATA_UNIT(second_item_ptr))
      {
        HDRRLPRSQ_SET_LAST_DATA_UNIT(first_item_ptr, TRUE);
      }
    }

    /* Merge data from second packet to first */
    q_delete(&rrcb_ptr->state.reseq_q, &second_item_ptr->link);
    dsm_append(&first_item_ptr, &second_item_ptr);
  }
}


/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                     INTERNAL FUNCTION DEFINITIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/
/*===========================================================================
  
===========================================================================*/
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
)
{
  /* EMPA/MMPA segment sequencing: 1 data unit per rx'd RLP packet. 
     However, there could have been multiple RLP packets coalesced in the 
     reseq_q. This info is stored in the app_ptr as a final count of total 
     packets coalesced. */
  if ((cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA ||
       cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA)&&
       cfg_ptr->segment_sequencing)
  {
    return (uint16)(uint32)rx_item_ptr->app_ptr;
  }
  else
  {
    return dsm_length_packet(rx_item_ptr);
  }
}

/*===========================================================================

FUNCTION       HDRRLPRSQ_RESET

DESCRIPTION    Called when an RLP instance is reset.  This takes any Reseq Q
               entries up until LV(N), and puts  them  into the rx q. From 
               that point on it sets free any other remaining dsm item 
               chains.

PARAMETERS     rscb_ptr: pointer to session control block containing RSQ
               
DEPENDENCIES   rscb_ptr != NULL

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void hdrrlprsq_reset
(
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr         /* current RLP session ctl blk*/
)
{
  q_type         *reseq_q_ptr = &(rrcb_ptr->state.reseq_q);     /* reseq q */
  dsm_item_type  *reseq_entry_ptr;               /*   reseq  list entry    */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    First call hdrrlprsq_remove() to make sure we pass up what remains.
    -------------------------------------------------------------------------*/
  hdrrlprsq_remove(rrcb_ptr);

  /*-------------------------------------------------------------------------
    Now we can free up the rest of the contents of the resequencing Q.
    -------------------------------------------------------------------------*/
  while ((reseq_entry_ptr = (dsm_item_type*)q_get(reseq_q_ptr)) != NULL)
  {
    dsm_free_packet(&reseq_entry_ptr);
  }
}

/*===========================================================================

FUNCTION       HDRRLPRSQ_INSERT

DESCRIPTION    This function performs the INSERT operation for the 
               Resequencing queue of the RLP software. It inserts a dsm item
               representing an RLP frame into the Resequencing queue. Every 
               time there is a hole in the data, the frames end up in this
               queue. Each item is inserted in order based on its sequence 
               number. When the missing data is received items are sent to
               upper layer.
                
               It also takes care of the cases there is overlap or partial
               overlap in the sequence number ranges of the dsm items and 
               replaces, discards or chops as required.
               
               If an entry already exists in the queue, then it is discarded.

PARAMETERS     rrcb_ptr:         pointer to RLP control block
               seq_num:          current sequence number
               first_data_unit:  this is the first data unit
               last_data_unit:   this is the last data unit
               fwd_pkt_ptr:      dsm item to insert
               qn_seq_included   Flag to determine if Qn sequence was included           

RETURN VALUE   TRUE if the packet was inserted; FALSE if discarded (e.g. duplicate)
===========================================================================*/
boolean hdrrlprsq_insert
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,          /* curr RLP ctl blk         */
  uint32                    seq_num,           /* current sequnce number   */
  boolean                   first_data_unit,
  boolean                   last_data_unit,
  dsm_item_type            *fwd_pkt_ptr,        /* the frame to insert      */
  boolean                   qn_seq_included,     /* used to update statistics in the log packet correctly*/
  boolean                   is_recursive
)
{
  q_type        *reseq_q_ptr = &(rrcb_ptr->state.reseq_q);      /* reseq q */
  uint32         num_data_units;
  dsm_item_type *prev_item_ptr = NULL;
  dsm_item_type *next_item_ptr = NULL;
  uint32         next_seq_num;
  uint16         overlapping_data_units;
  uint16         overlapping_octets;
  uint32         data_len;                     /* RLP packet length        */
  uint32         old_vn;
  uint32         end_seq_num;
  uint32         tmp_seq_num;
  uint32         tmp_packet_len;
  uint32         seq_num_tail;
  boolean        need_split;
  uint32         split_offset;
  boolean        need_recursive = FALSE;
  dsm_item_type  *fwd_pkt_tail_ptr = NULL;
  hdrrlp_debug_ctl *hdrrlp_debug_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DSM_TOUCH_ITEM(fwd_pkt_ptr);

  hdrrlp_debug_ptr = get_hdrrlp_debug_ptr();

  /* Store meta-data in various elements of the dsm item */
  HDRRLPRSQ_SET_SEQ_NUM(fwd_pkt_ptr, seq_num); 

  /* EMPA segment sequencing: 1 data unit per rx'd RLP packet */
  if ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA || 
       rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA)&&
       rrcb_ptr->cfg_ptr->segment_sequencing)
  {
    num_data_units = 1;
  }
  else
  {
    num_data_units = HDRRLPRSQ_COMPUTE_NUM_DATA_UNITS(rrcb_ptr->cfg_ptr,
                                                      fwd_pkt_ptr);
  }
  HDRRLPRSQ_SET_NUM_DATA_UNITS(fwd_pkt_ptr, num_data_units);
  
  /* EMPA/MMPA packet stream: remember data unit flags, clear out of order flag */
  if ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA || 
       rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) && (is_recursive == FALSE))
  {
    if (rrcb_ptr->cfg_ptr->packet_stream)
    {
      rrcb_ptr->stats.rx_total_frames++;

      HDRRLPRSQ_SET_FIRST_DATA_UNIT(fwd_pkt_ptr, first_data_unit);
		  HDRRLPRSQ_SET_LAST_DATA_UNIT(fwd_pkt_ptr, last_data_unit);
		  HDRRLPRSQ_SET_ALREADY_PASSED_UP_OUT_OF_ORDER(fwd_pkt_ptr, 0);
      if (first_data_unit == TRUE)
      {
        rrcb_ptr->stats.rx_first_data_unit++;
      }
      if (last_data_unit == TRUE)
      {
        rrcb_ptr->stats.rx_last_data_unit++;
      }
    }
  }

  /*=========================================================================
    Process incoming RLP packets according to the procedures outlined
    in the IS856-A standard.  Note that all operations and comparisons
    performed on the RLP packet sequence numbers are carried out in modulo
    2^s arithmetic, where for HDR, s=seq len bits.  For any sequence number
    N, the sequence numbers in the range [N+1, N+2^(s-1) - 1] shall be
    considered greater than N, and the sequence numbers in the range
    [N-2^(s-1), N-1] shall be considered smaller than N.
    ========================================================================*/

  /* If the packet contains only new data (X >= V(R)), bypass the
     complex cases 1 and/or 2; i.e. perform a fast insert */
  if (HDRRLPSEQ_GT_SEQ_CHECK(rrcb_ptr->state.expected_seq_num, seq_num,
                            rrcb_ptr->cfg_ptr->seq_modulus))
  {

    /*---------------------------------------------------------------------
      Case 1: fairly common case of X < V(N).  This is regularly
      expected for flush packets.  Silently discard and free the
      duplicate(s).  If packet overlaps V(N), fall through to case 2.
      No change to V(N).  Since V(N) <= V(R) always, V(R) is not 
      involved in this case.

      A) packet < V(N)

        Before:   |---|---|---|---|
                                 V(N)

        Packet:               |---|
                              X 

        Return:

      B) packet overlaps V(N)

        Before:   |---|---|---|---|
                                 V(N)

        Packet:                 |-----|
                                X 

        Fall through:             |---|
                                  X

      --------------------------------------------------------------------*/
    if (HDRRLPSEQ_GT_SEQ_CHECK(rrcb_ptr->state.needed_seq_num, seq_num,
                              rrcb_ptr->cfg_ptr->seq_modulus))
    {
      /* EMPA/MMPA segment sequencing: 1 data unit per rx'd RLP packet */
      if ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA || 
          rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) &&
          rrcb_ptr->cfg_ptr->segment_sequencing)
      {
        overlapping_data_units = 1;
        overlapping_octets = (uint16)dsm_length_packet(fwd_pkt_ptr);
      }
      else
      {
        tmp_seq_num = HDRRLPSEQ_DIFF_SEQ(rrcb_ptr->state.needed_seq_num,
                                         seq_num, rrcb_ptr->cfg_ptr->seq_modulus);
        tmp_packet_len = dsm_length_packet(fwd_pkt_ptr);

        overlapping_data_units = (uint16) MIN(tmp_packet_len, tmp_seq_num );
        overlapping_octets = overlapping_data_units;
      }

      rrcb_ptr->stats.rx_dup_fr_cnt += overlapping_octets;
      rrcb_ptr->stats.rx_dup_frames++;
      rrcb_ptr->stats.rx_total_bytes += overlapping_octets;
      rrcb_ptr->stats.rx_total_frames++;

      DATA_RLP_MSG8 (MSG_LEGACY_MED, "RLP Rx[%d,%d] V[%d,%d] dup:0x%x Vn:0x%x Vr:0x%x len:0x%x",
                      rrcb_ptr->cfg_ptr->sr_id,
                      "AB"[(rrcb_ptr->route)],
                      rrcb_ptr->cfg_ptr->ver,
                      rrcb_ptr->cfg_ptr->enhanced,
                      seq_num,
                      rrcb_ptr->state.needed_seq_num,
                      rrcb_ptr->state.expected_seq_num,
                      overlapping_data_units);

      (void)dsm_pullup(&fwd_pkt_ptr, NULL, overlapping_octets);
      if (fwd_pkt_ptr == NULL) 
      {
        /* A) packet < V(N) */
        return FALSE;
      }

      /* Update meta-data for new length */
      seq_num = HDRRLPSEQ_ADD_SEQ(seq_num,
                                   overlapping_data_units,
                                   rrcb_ptr->cfg_ptr->seq_modulus);
      HDRRLPRSQ_SET_SEQ_NUM(fwd_pkt_ptr, seq_num);
      num_data_units = HDRRLPRSQ_COMPUTE_NUM_DATA_UNITS(rrcb_ptr->cfg_ptr,
                                                        fwd_pkt_ptr);
      HDRRLPRSQ_SET_NUM_DATA_UNITS(fwd_pkt_ptr, num_data_units);

      /* EMPA packet stream: can't be FDU any longer */
      if ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA || 
          rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) &&
          rrcb_ptr->cfg_ptr->packet_stream) 
      {
        first_data_unit = FALSE;
        HDRRLPRSQ_SET_FIRST_DATA_UNIT(fwd_pkt_ptr, first_data_unit);
      }

      /* B) packet overlaps V(N) */
      seq_num = rrcb_ptr->state.needed_seq_num;

      /* Fall through to case 2; now X == V(N) */
    } 

    /*---------------------------------------------------------------------
      Case 2: V(N) <= X < V(R).  This is a retransmission.  Silently
      discard and free any leading or trailing duplicate(s).  Remove
      the corresponding Nak entry.  Insert retx data into correct
      location in the resequencing queue.  Coalesce with surrounding
      data units.  If packet overlaps V(R), fall through to case 3.

      A) packet < V(R)

        Before:   |---|---|   |---|   |---|
                         V(N)            V(R)

        Packet:                  |-----|
                                 X 

        After:    |---|---|   |---|---|---|
                         V(N)            V(R)

      B) packet overlaps V(R)

        Before:   |---|---|   |---|
                         V(N)    V(R)

        Packet:                 |-----|
                                X 

        Fall through:             |---|
                                  X 

      --------------------------------------------------------------------*/
    if (HDRRLPSEQ_GE_SEQ_CHECK(seq_num, rrcb_ptr->state.needed_seq_num,
                              rrcb_ptr->cfg_ptr->seq_modulus) && 
        HDRRLPSEQ_GT_SEQ_CHECK(rrcb_ptr->state.expected_seq_num, seq_num,
                              rrcb_ptr->cfg_ptr->seq_modulus))
    {
      /* Find the first item with sequence number greater than ours */
      prev_item_ptr = NULL; 
      next_item_ptr = (dsm_item_type*)q_check(reseq_q_ptr);
      while (next_item_ptr != NULL)
      {
        next_seq_num = HDRRLPRSQ_GET_SEQ_NUM(next_item_ptr);
        if (HDRRLPSEQ_GT_SEQ_CHECK(next_seq_num,
                                  seq_num,
                                  rrcb_ptr->cfg_ptr->seq_modulus))
        {
          break;
        }

        prev_item_ptr = next_item_ptr;
        next_item_ptr = (dsm_item_type*)q_next(reseq_q_ptr, &next_item_ptr->link);
      }

      /* Drop any leading duplicate data units */
      if (prev_item_ptr != NULL &&
          HDRRLPRSQ_OVERLAPPING(rrcb_ptr, prev_item_ptr, fwd_pkt_ptr,
                                &overlapping_data_units, &need_split, &split_offset))
      {
        /* EMPA/MMPA segment sequencing: 1 data unit per rx'd RLP packet */
        if ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA || 
            rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) &&
            rrcb_ptr->cfg_ptr->segment_sequencing)
        {
          overlapping_octets = (uint16)dsm_length_packet(fwd_pkt_ptr);
        }
        else
        {
          overlapping_octets = overlapping_data_units;
        }

        rrcb_ptr->stats.rx_dup_fr_cnt += overlapping_octets;
        rrcb_ptr->stats.rx_dup_frames++;
        rrcb_ptr->stats.rx_total_bytes += overlapping_octets;
        rrcb_ptr->stats.rx_total_frames++;


        DATA_RLP_MSG9( MSG_LEGACY_MED, 
                       "RLP Rx[%d,%d] V[%d,%d] dup:0x%x Vn:0x%x Vr:0x%x overlap:0x%x Dropped:0x%x",
                        rrcb_ptr->cfg_ptr->sr_id,
                        "AB"[(rrcb_ptr->route)],
                        rrcb_ptr->cfg_ptr->ver,
                        rrcb_ptr->cfg_ptr->enhanced,
                        seq_num,
                        rrcb_ptr->state.needed_seq_num,
                        rrcb_ptr->state.expected_seq_num,
                        overlapping_data_units,
                        overlapping_octets);

        (void)dsm_pullup(&fwd_pkt_ptr, NULL, overlapping_octets);
        if (fwd_pkt_ptr == NULL)
        {
          return FALSE;
        }

        /* Update meta-data for new length */
        seq_num = HDRRLPSEQ_ADD_SEQ(seq_num, 
                                     overlapping_octets,
                                     rrcb_ptr->cfg_ptr->seq_modulus);
        HDRRLPRSQ_SET_SEQ_NUM(fwd_pkt_ptr, seq_num);
        num_data_units = HDRRLPRSQ_COMPUTE_NUM_DATA_UNITS(rrcb_ptr->cfg_ptr,
                                                          fwd_pkt_ptr);
        HDRRLPRSQ_SET_NUM_DATA_UNITS(fwd_pkt_ptr, num_data_units);


        /* EMPA/MMPA packet stream: can't be FDU any longer */
        if ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA || 
            rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) &&
            rrcb_ptr->cfg_ptr->packet_stream) 
        {
          first_data_unit = FALSE;
          HDRRLPRSQ_SET_FIRST_DATA_UNIT(fwd_pkt_ptr, first_data_unit);
        }
      }

      /* Drop any trailing duplicate data units.  Note that this does
         _not_ cover the difficult case where there are non-duplicate data
         units after the trailing duplicate data units.  We will rely on
         the Nak or higher layer recovery mechanisms to request these
         bytes again. This is an acceptable compromise since a
         well-behaved AN should never send data in this fashion. */
      if (next_item_ptr != NULL &&
          HDRRLPRSQ_OVERLAPPING(rrcb_ptr, fwd_pkt_ptr, next_item_ptr,
                                &overlapping_data_units, &need_split, &split_offset))
      {
        /* EMPA/MMPA segment sequencing: 1 data unit per rx'd RLP packet */
        if ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA || 
            rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) &&
            rrcb_ptr->cfg_ptr->segment_sequencing)
        {
          overlapping_octets = (uint16)dsm_length_packet(fwd_pkt_ptr);
        }
        else
        {
          overlapping_octets = overlapping_data_units;
        }

        rrcb_ptr->stats.rx_dup_fr_cnt += overlapping_octets;
        rrcb_ptr->stats.rx_dup_frames++;
        rrcb_ptr->stats.rx_total_bytes += overlapping_octets;
        rrcb_ptr->stats.rx_total_frames++;

        DATA_RLP_MSG9( MSG_LEGACY_MED, 
                       "RLP Rx[%d,%d] V[%d,%d] dup:0x%x Vn:0x%x Vr:0x%x overlap:0x%x dropped:0x%x",
                        rrcb_ptr->cfg_ptr->sr_id,
                        "AB"[(rrcb_ptr->route)],
                        rrcb_ptr->cfg_ptr->ver,
                        rrcb_ptr->cfg_ptr->enhanced,
                        HDRRLPRSQ_GET_SEQ_NUM(next_item_ptr),
                        rrcb_ptr->state.needed_seq_num,
                        rrcb_ptr->state.expected_seq_num,
                        overlapping_data_units,
                        overlapping_octets);

        if (need_split == FALSE)
        {
           (void)dsm_pullup_tail(&fwd_pkt_ptr, NULL, overlapping_octets);
        }
        else
        {
          if (dsm_split_packet( &fwd_pkt_ptr, 
                                 &fwd_pkt_tail_ptr, 
                                 split_offset, 
                                 DSM_DUP_ITEM_POOL ) != split_offset)
          {
            DATA_RLP_MSG3( MSG_LEGACY_MED,"Cannot Split the DSM Item Freeing the DSM Items. p1 len:%d p2 len:%d offset:%d",
                           dsm_length_packet(fwd_pkt_ptr),
                           dsm_length_packet(fwd_pkt_tail_ptr),
                           split_offset );
            dsm_free_packet(&fwd_pkt_ptr);                        
            dsm_free_packet(&fwd_pkt_tail_ptr);                        
            return FALSE;
          }
          else
          { /* after split */
            (void)dsm_pullup(&fwd_pkt_tail_ptr, NULL, overlapping_octets);
            if (fwd_pkt_tail_ptr == NULL)
            {
              DATA_RLP_MSG8 (MSG_LEGACY_MED, "RLP Rx[%d,%d] V[%d,%d] dup:0x%x Vn:0x%x Vr:0x%x len:0x%x tail packet after split is freed ",
                             rrcb_ptr->cfg_ptr->sr_id,
                             "AB"[(rrcb_ptr->route)],
                             rrcb_ptr->cfg_ptr->ver,
                             rrcb_ptr->cfg_ptr->enhanced,
                             HDRRLPRSQ_GET_SEQ_NUM(next_item_ptr),
                             rrcb_ptr->state.needed_seq_num,
                             rrcb_ptr->state.expected_seq_num,
                             overlapping_octets);
            }
            else
            {
              need_recursive = TRUE;
              seq_num_tail = HDRRLPSEQ_ADD_SEQ(HDRRLPRSQ_GET_SEQ_NUM(next_item_ptr), 
                                               overlapping_octets,
                                               rrcb_ptr->cfg_ptr->seq_modulus);
              HDRRLPRSQ_SET_SEQ_NUM(fwd_pkt_tail_ptr, seq_num_tail);
              num_data_units = HDRRLPRSQ_COMPUTE_NUM_DATA_UNITS(rrcb_ptr->cfg_ptr,
                                                                fwd_pkt_tail_ptr);
              HDRRLPRSQ_SET_NUM_DATA_UNITS(fwd_pkt_tail_ptr, num_data_units);
              if ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA || 
                    rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) &&
                    rrcb_ptr->cfg_ptr->packet_stream) 
              {
                first_data_unit = FALSE;
                HDRRLPRSQ_SET_FIRST_DATA_UNIT(fwd_pkt_tail_ptr, first_data_unit);
              }
            }
          } /* after split */
        } /* (need_split == TRUE) */
       

        if (fwd_pkt_ptr == NULL)
        {
          DATA_RLP_MSG8 (MSG_LEGACY_MED, "RLP Rx[%d,%d] V[%d,%d] dup:0x%x Vn:0x%x Vr:0x%x len:0x%x incoming packet is freed ",
                          rrcb_ptr->cfg_ptr->sr_id,
                          "AB"[(rrcb_ptr->route)],
                          rrcb_ptr->cfg_ptr->ver,
                          rrcb_ptr->cfg_ptr->enhanced,
                          HDRRLPRSQ_GET_SEQ_NUM(next_item_ptr),
                          rrcb_ptr->state.needed_seq_num,
                          rrcb_ptr->state.expected_seq_num,
                          overlapping_data_units);


          return FALSE;
        }

        /* Update meta-data for new length */
        num_data_units = HDRRLPRSQ_COMPUTE_NUM_DATA_UNITS(rrcb_ptr->cfg_ptr, fwd_pkt_ptr);
        HDRRLPRSQ_SET_NUM_DATA_UNITS(fwd_pkt_ptr, num_data_units);

        /* EMPA/MMPA packet stream: can't be LDU any longer */
        if ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA || 
            rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) &&
            rrcb_ptr->cfg_ptr->packet_stream) 
        {
          last_data_unit = FALSE;
          HDRRLPRSQ_SET_LAST_DATA_UNIT(fwd_pkt_ptr, last_data_unit);
        }
      }

      data_len = dsm_length_packet(fwd_pkt_ptr);
      /* These are retransmit bytes only if this is not MMPA.
         For MMPA, we need to see if QNSeq is included or not to determine
         if the bytes are new or retransmits. Retx stats for MMPA are 
         updated in hdrrlp_process_rx_pkt for this reason. */
      if (rrcb_ptr->cfg_ptr->enhanced != DSRLP_ENHANCED_VER_MMPA) 
      {
        rrcb_ptr->stats.rx_rexmit_fr_cnt += data_len;
        rrcb_ptr->stats.rx_rexmit_frames++;
      }
      else
      {
        /*
        This else part has been added to catch cases where we have new transmissions
        combined with the case where we have processed a packet with higher sequence
        number before we do the ones with the lower sequence numbers.
        The else part is entered if it is MMPA. In MMPA there are lots of chances that
        we get out of order RLP packets due to the multi carrier scenario. 
        If for an RLP packet, the seq number is available, we count the received bytes
        towards new transmissions even if we have processed an RLP packet with a higher
        sequence number.
        In the absence of this else part, those bytes are considered as retransmitted byes and
        the log packet statistics are computed wrongly.
        */
        if(TRUE == qn_seq_included)
        {
          rrcb_ptr->stats.rx_new_data_fr_cnt += data_len;
          rrcb_ptr->stats.rx_new_data_frames++;
        }
      }
      rrcb_ptr->stats.rx_total_bytes += data_len;
      rrcb_ptr->stats.rx_total_frames++;

      if (hdrrlp_debug_ptr->enable_rx_debug) 
      {
        if ( qn_seq_included == TRUE )
        {
          DATA_RLP_MSG8 (MSG_LEGACY_MED, "RLP Rx[%d,%d] V[%d,%d] new out-of-order:0x%x Vn:0x%x Vr:0x%x len:0x%x",
                        rrcb_ptr->cfg_ptr->sr_id,
                        "AB"[(rrcb_ptr->route)],
                        rrcb_ptr->cfg_ptr->ver,
                        rrcb_ptr->cfg_ptr->enhanced,
                        seq_num,
                        rrcb_ptr->state.needed_seq_num,
                        rrcb_ptr->state.expected_seq_num,
                        num_data_units);
        }
        else
        {
          DATA_RLP_MSG8 (MSG_LEGACY_MED, "RLP Rx[%d,%d] V[%d,%d] rexmit:0x%x Vn:0x%x Vr:0x%x len:0x%x",
                        rrcb_ptr->cfg_ptr->sr_id,
                        "AB"[(rrcb_ptr->route)],
                        rrcb_ptr->cfg_ptr->ver,
                        rrcb_ptr->cfg_ptr->enhanced,
                        seq_num,
                        rrcb_ptr->state.needed_seq_num,
                        rrcb_ptr->state.expected_seq_num,
                        num_data_units);
        }
      }

      /*-----------------------------------------------------------------------
        We do not need to check against Nak entries at this point anymore. Let 
        the Nak timer run till it expires and that point check if the hole was 
        filled or not. If it is filled, do nothing, else abort pending data.

        If X==V(N), update V(N) to move past the received data units
        ----------------------------------------------------------------------*/

      /* Insert new before next */
      if (next_item_ptr != NULL) 
      {
        /* A) packet < V(R) */
        q_insert(reseq_q_ptr, &(fwd_pkt_ptr->link), &(next_item_ptr->link));

        /* Combine new and next if they are adjacent */
        HDRRLPRSQ_COALESCE(rrcb_ptr, fwd_pkt_ptr);
      }
      else
      {
        /*-----------------------------------------------------------------------
          next_item_ptr can be null here in MMPA due to the flush message.
          When we get FLUSH message, V(R) is set to the seq num in FLUSH and a 
          NAK is generated. When the response to NAK is received, it will hit
          this case with next_item_ptr==NULL.
          In this case, place new item at end of q.
          ----------------------------------------------------------------------*/
        q_put(reseq_q_ptr, &(fwd_pkt_ptr)->link);

        /* This packet could contain data past V(R) too. If so, update V(R) */
        end_seq_num = HDRRLPSEQ_ADD_SEQ(seq_num,
                                        HDRRLPRSQ_COMPUTE_NUM_DATA_UNITS(rrcb_ptr->cfg_ptr, fwd_pkt_ptr),
                                        rrcb_ptr->cfg_ptr->seq_modulus);

        if (HDRRLPSEQ_GT_SEQ_CHECK(end_seq_num, 
                                   rrcb_ptr->state.expected_seq_num,
                                   rrcb_ptr->cfg_ptr->seq_modulus))
        {
          rrcb_ptr->state.expected_seq_num = end_seq_num;

          if (hdrrlp_debug_ptr->enable_rx_debug) 
          {
            DATA_RLP_MSG5( MSG_LEGACY_MED, 
                            "RLP Rx[%d,%d] V[%d,%d] Pkt has rexmit along with new data. New Vr: 0x%x",
                            rrcb_ptr->cfg_ptr->sr_id,
                            "AB"[(rrcb_ptr->route)],
                            rrcb_ptr->cfg_ptr->ver,
                            rrcb_ptr->cfg_ptr->enhanced,
                            rrcb_ptr->state.expected_seq_num );
          }
        }
      }
      /* Now check to see how far V(n) can be moved ahead based on the
          contiguous data units available in the reseq_q. Don't coalesce
          with prev item yet as that will change the seq_num of the 
          item at the head of the queue, and it might become less than V(n) */
      
      if (rrcb_ptr->state.needed_seq_num == seq_num)
      {
        /* Update the V(n) to beyond the next available contiguous data units. 
           All continuous data units beyond V(n) have already been coalesced 
           into a single dsm entry in the reseq_q. */

        old_vn = rrcb_ptr->state.needed_seq_num;

        num_data_units = HDRRLPRSQ_COMPUTE_NUM_DATA_UNITS(rrcb_ptr->cfg_ptr, fwd_pkt_ptr);

        rrcb_ptr->state.needed_seq_num = 
        HDRRLPSEQ_ADD_SEQ( rrcb_ptr->state.needed_seq_num,
                             num_data_units, 
                             rrcb_ptr->cfg_ptr->seq_modulus);

        if (hdrrlp_debug_ptr->enable_rx_debug) 
        {
          DATA_RLP_MSG7 (MSG_LEGACY_MED, "RLP Rx[%d,%d] V[%d,%d] V(n) moved 0x%x to 0x%x next_item[0x%x]",
                          rrcb_ptr->cfg_ptr->sr_id,
                          "AB"[(rrcb_ptr->route)],
                          rrcb_ptr->cfg_ptr->ver,
                          rrcb_ptr->cfg_ptr->enhanced,
                          old_vn, 
                          rrcb_ptr->state.needed_seq_num, 
                          next_item_ptr);
        }
      }
      
      /* Now combine prev and new if they are adjacent */
      HDRRLPRSQ_COALESCE(rrcb_ptr, prev_item_ptr);   
      if (need_recursive == TRUE)
      {
        (void)hdrrlprsq_insert(rrcb_ptr, 
                         HDRRLPRSQ_GET_SEQ_NUM(fwd_pkt_tail_ptr),
                         HDRRLPRSQ_GET_FIRST_DATA_UNIT(fwd_pkt_tail_ptr),
                         HDRRLPRSQ_GET_LAST_DATA_UNIT(fwd_pkt_tail_ptr),
                         fwd_pkt_tail_ptr, 
                         qn_seq_included,
                         TRUE);
      }
      return TRUE;
    }
  }

  /*---------------------------------------------------------------------
    Case 3: the most common case of X == V(R).  This is a new and in
    order packet.  There may or may not be a hole earlier in the
    resequencing queue.  Append new packet and increment V(R);
    increment V(N) only if V(N) == V(R) (no hole already).

    A) V(N) == V(R)

      Before:   |---|---|---|---|
                               V(R)
                               V(N)

      Packet:                   |---|
                                X 

      After:    |---|---|---|---|---|
                                   V(R)
                                   V(N)
                     
    B) V(N) != V(R)

      Before:   |---|---|   |---|
                       V(N)    V(R)

      Packet:                   |---|
                                X 

      After:    |---|---|   |---|---|
                       V(N)        V(R)

    --------------------------------------------------------------------*/
  if (seq_num == rrcb_ptr->state.expected_seq_num)
  {
    data_len = dsm_length_packet(fwd_pkt_ptr);
    rrcb_ptr->stats.rx_new_data_fr_cnt += data_len;
    rrcb_ptr->stats.rx_new_data_frames++;
    rrcb_ptr->stats.rx_total_bytes += data_len;
    rrcb_ptr->stats.rx_total_frames++;

    if (hdrrlp_debug_ptr->enable_rx_debug) 
    {
      DATA_RLP_MSG8 (MSG_LEGACY_MED, "RLP Rx[%d,%d] V[%d,%d] new in-order:0x%x Vn:0x%x Vr:0x%x len:0x%x",
                      rrcb_ptr->cfg_ptr->sr_id,
                      "AB"[(rrcb_ptr->route)],
                      rrcb_ptr->cfg_ptr->ver,
                      rrcb_ptr->cfg_ptr->enhanced,
                      seq_num, 
                      rrcb_ptr->state.needed_seq_num,
                      rrcb_ptr->state.expected_seq_num, 
                      num_data_units);
    }

    /* Retrieve the current last item */
    prev_item_ptr = (dsm_item_type*)q_last_check(reseq_q_ptr);

    /* Place new item at end of q */
    q_put(reseq_q_ptr, &(fwd_pkt_ptr)->link);

    
    if (prev_item_ptr)
    {
      /* Combine last two items if they are adjacent */
      HDRRLPRSQ_COALESCE(rrcb_ptr, prev_item_ptr);
    }

    /* Increment V(N) if there are no holes.  Always increment V(R). */
    if (rrcb_ptr->state.needed_seq_num == rrcb_ptr->state.expected_seq_num)
    {
      old_vn = rrcb_ptr->state.needed_seq_num;

      rrcb_ptr->state.needed_seq_num = 
        HDRRLPSEQ_ADD_SEQ( rrcb_ptr->state.needed_seq_num,
                           num_data_units, 
                           rrcb_ptr->cfg_ptr->seq_modulus);
      rrcb_ptr->state.expected_seq_num = rrcb_ptr->state.needed_seq_num;

      if (hdrrlp_debug_ptr->enable_rx_debug) 
      {
        DATA_RLP_MSG6 (MSG_LEGACY_MED, "RLP Rx[%d,%d] V[%d,%d] V(n) moved 0x%x to 0x%x",
                        rrcb_ptr->cfg_ptr->sr_id,
                        "AB"[(rrcb_ptr->route)],
                        rrcb_ptr->cfg_ptr->ver,
                        rrcb_ptr->cfg_ptr->enhanced,
                        old_vn, 
                        rrcb_ptr->state.needed_seq_num);
      }
    }
    else
    {
      rrcb_ptr->state.expected_seq_num = 
        HDRRLPSEQ_ADD_SEQ( rrcb_ptr->state.expected_seq_num,
                         num_data_units, 
                         rrcb_ptr->cfg_ptr->seq_modulus);
    }

    return TRUE;
  }

  /*---------------------------------------------------------------------
    Case 4: V(R) < X.  This is a new but out of order packet.  There
    may or may not be a hole earlier in the resequencing queue.  We
    need to create a new Nak list entry for the newly formed
    hole. Append new packet and increment only V(R).

    A) V(N) == V(R)

      Before:   |---|---|---|---|
                               V(R)
                               V(N)

      Packet:                       |---|
                                    X 

      After:    |---|---|---|---|   |---|
                               V(N)    V(R)

    B) V(N) != V(R)

      Before:   |---|---|   |---|
                       V(N)    V(R)

      Packet:                       |---|
                                    X 

      After:    |---|---|   |---|   |---|
                       V(N)     (nak)  V(R)
                                 
    --------------------------------------------------------------------*/
  if (HDRRLPSEQ_GT_SEQ_CHECK(seq_num, rrcb_ptr->state.expected_seq_num,
                            rrcb_ptr->cfg_ptr->seq_modulus))
  {
    data_len = dsm_length_packet(fwd_pkt_ptr);
    rrcb_ptr->stats.rx_new_data_fr_cnt += data_len;
    rrcb_ptr->stats.rx_new_data_frames++;
    rrcb_ptr->stats.rx_total_bytes += data_len;
    rrcb_ptr->stats.rx_total_frames++;

    if (hdrrlp_debug_ptr->enable_rx_debug) 
    {
      DATA_RLP_MSG8 (MSG_LEGACY_MED, "RLP Rx[%d,%d] V[%d,%d] new out-of-order:0x%x Vn:0x%x Vr:0x%x len:0x%x",
                      rrcb_ptr->cfg_ptr->sr_id,
                      "AB"[(rrcb_ptr->route)],
                      rrcb_ptr->cfg_ptr->ver,
                      rrcb_ptr->cfg_ptr->enhanced,
                      seq_num, 
                      rrcb_ptr->state.needed_seq_num,
                      rrcb_ptr->state.expected_seq_num, 
                      num_data_units);
    }

    /*----------------------------------------------------------------------
      A NAK list entry may be required, if Rx'ed seq > V(R).  Generate NAK list 
      entries for missing RLP data units from V(R) to X-1 inclusive.

      This needs to be done even if naks are disabled, so that the
      abort timer will advance V(N).
      ----------------------------------------------------------------------*/
    if (!hdrrlpnak_insert(rrcb_ptr, rrcb_ptr->state.expected_seq_num, seq_num)) 
    {
      DATA_RLP_MSG4 (MSG_LEGACY_ERROR, "RLP Rx[%d,%d] V[%d,%d] Could not insert into NAK list.",
                      rrcb_ptr->cfg_ptr->sr_id,
                      "AB"[(rrcb_ptr->route)],
                      rrcb_ptr->cfg_ptr->ver,
                      rrcb_ptr->cfg_ptr->enhanced);
      hdrrlpi_mobile_initiated_reset(rrcb_ptr);
      /* Free the DSM Item for further use */
      dsm_free_packet(&fwd_pkt_ptr);
      return FALSE;
    }

    /* Place new item at end of q */
    q_put(reseq_q_ptr, &(fwd_pkt_ptr)->link);

    /* Don't increment V(N).  Always increment V(R). */
    rrcb_ptr->state.expected_seq_num = HDRRLPSEQ_ADD_SEQ(seq_num, num_data_units,
                                                          rrcb_ptr->cfg_ptr->seq_modulus);

    return TRUE;
  }

  /* This should never be reached */
  DATA_RLP_MSG8 (MSG_LEGACY_ERROR, "RLP Rx[%d,%d] V[%d,%d] unknown:0x%x Vn:0x%x Vr:0x%x len:0x%x",
                  rrcb_ptr->cfg_ptr->sr_id,
                  "AB"[(rrcb_ptr->route)],
                  rrcb_ptr->cfg_ptr->ver,
                  rrcb_ptr->cfg_ptr->enhanced,
                  seq_num,  
                  rrcb_ptr->state.needed_seq_num,
                  rrcb_ptr->state.expected_seq_num, 
                  num_data_units);
  return FALSE;
}


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
void hdrrlprsq_remove
(
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr                    /* curr RLP ctl blk */
)
{
  q_type          *reseq_q_ptr = &(rrcb_ptr->state.reseq_q);    /* reseq q */
  dsm_item_type   *reseq_item_ptr;                    /* local working ptr */
  dsm_item_type   *tmp_item_ptr;                          /* temp loop ptr */
  boolean          first_data_unit;
  boolean          last_data_unit;
  boolean          already_passed_up_out_of_order;
  uint32           next_seq_num;
  uint32           tmp_seq_num;

  dsm_item_type   *pkt_ptr;
  uint32           num_data_units;
  uint32           end_seq_num;
  uint32           dsm_dup_len = 0;
  hdrrlp_debug_ctl *hdrrlp_debug_ptr = NULL;
                                                   
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  hdrrlp_debug_ptr = get_hdrrlp_debug_ptr();

  /* EMPA/MMPA packet stream: whack any packets < V(N) that should not be
     passed up to the higher layers.  This code is relevant
     particularly after an abort timer expiration. */
  if ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA || 
       rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) &&
       rrcb_ptr->cfg_ptr->packet_stream)
  {
    while (1)
    {
      reseq_item_ptr = (dsm_item_type*)q_check(reseq_q_ptr);
      if (reseq_item_ptr == NULL)
      {
        return;
      }

      tmp_seq_num = HDRRLPRSQ_GET_SEQ_NUM(reseq_item_ptr);
      /* If this packet it not < V(N), get out of this loop */
      if (!HDRRLPSEQ_GT_SEQ_CHECK(rrcb_ptr->state.needed_seq_num,
                                 tmp_seq_num,
                                 rrcb_ptr->cfg_ptr->seq_modulus))
      {
        break;
      }

      /* EMPA out of order: discard any packet < V(N) that has */
      /* already been passed up to the Route Protocol. */
      if (rrcb_ptr->cfg_ptr->out_of_order_allowed)
      {
        already_passed_up_out_of_order = HDRRLPRSQ_GET_ALREADY_PASSED_UP_OUT_OF_ORDER(reseq_item_ptr);
        if (already_passed_up_out_of_order)
        {
          DATA_RLP_MSG6 (MSG_LEGACY_MED, "RLP Rx[%d,%d] V[%d,%d] RSQ delete old packet seq:0x%x len:0x%x",
                          rrcb_ptr->cfg_ptr->sr_id,
                          "AB"[(rrcb_ptr->route)],
                          rrcb_ptr->cfg_ptr->ver,
                          rrcb_ptr->cfg_ptr->enhanced,
                          HDRRLPRSQ_GET_SEQ_NUM(reseq_item_ptr),
                          dsm_length_packet(reseq_item_ptr));

          q_delete(reseq_q_ptr, &reseq_item_ptr->link);
          dsm_free_packet(&reseq_item_ptr);
          continue;
        }
      }

      /* Discard any packet < V(N) that does not have FDU set. */
      /* This is likely due to an aborted FDU segment, which means */
      /* the packet can never be reassembled. */
      first_data_unit = HDRRLPRSQ_GET_FIRST_DATA_UNIT(reseq_item_ptr);
      if (!first_data_unit)
      {
        DATA_RLP_MSG6 (MSG_LEGACY_MED, "RLP Rx[%d,%d] V[%d,%d] RSQ discard non-FDU packet seq:0x%x len:0x%x",
                        rrcb_ptr->cfg_ptr->sr_id,
                        "AB"[(rrcb_ptr->route)],
                        rrcb_ptr->cfg_ptr->ver,
                        rrcb_ptr->cfg_ptr->enhanced,
                        HDRRLPRSQ_GET_SEQ_NUM(reseq_item_ptr),
                        dsm_length_packet(reseq_item_ptr));

        q_delete(reseq_q_ptr, &reseq_item_ptr->link);
        dsm_free_packet(&reseq_item_ptr);
        continue;
      }
      
      /* Discard any FDU segment < V(N) with an aborted segment after it. */
      last_data_unit = HDRRLPRSQ_GET_LAST_DATA_UNIT(reseq_item_ptr);
      if (!last_data_unit)
      {
        /* Compute the next sequence number immediately following FDU segment */
        next_seq_num = HDRRLPSEQ_ADD_SEQ(HDRRLPRSQ_GET_SEQ_NUM(reseq_item_ptr),
                                          HDRRLPRSQ_GET_NUM_DATA_UNITS(reseq_item_ptr),
                                          rrcb_ptr->cfg_ptr->seq_modulus);
        
        /* If V(N) is beyond the next sequence number, then the next */
        /* segment has been aborted, so discard the FDU segment as */
        /* the packet can never be reassembled. */
        if (HDRRLPSEQ_GT_SEQ_CHECK(rrcb_ptr->state.needed_seq_num,
                                  next_seq_num,
                                  rrcb_ptr->cfg_ptr->seq_modulus)) 
        {
          DATA_RLP_MSG6 (MSG_LEGACY_MED, 
                          "RLP Rx[%d,%d] V[%d,%d] RSQ discard non-LDU packet seq:0x%x len:0x%x",
                          rrcb_ptr->cfg_ptr->sr_id,
                          "AB"[(rrcb_ptr->route)],
                          rrcb_ptr->cfg_ptr->ver,
                          rrcb_ptr->cfg_ptr->enhanced,
                          HDRRLPRSQ_GET_SEQ_NUM(reseq_item_ptr),
                          dsm_length_packet(reseq_item_ptr));

          q_delete(reseq_q_ptr, &reseq_item_ptr->link);
          dsm_free_packet(&reseq_item_ptr);
          continue;
        }
      }

      /* If we get here, then the current leading packet is good */
      break;
    }
  } /* Done discarding old packets */

  /* Pass up new packets to the upper layers */
  tmp_item_ptr = (dsm_item_type*)q_check(reseq_q_ptr);
  while (tmp_item_ptr != NULL)
  {
    /*---------------------------------------------------------------------
      Within the loop, it is possible for the current reseq entry to be
      deleted. Therefore, make a copy of the ptr to the current entry, and
      then immediately get the next entry.  This way, the current entry can
      be deleted safely.  (Otherwise, once the current entry is deleted, it
      becomes impossible to find the next entry).
      ---------------------------------------------------------------------*/ 
    reseq_item_ptr = tmp_item_ptr;
    tmp_item_ptr = (dsm_item_type*)q_next(reseq_q_ptr, &reseq_item_ptr->link);

    /* EMPA/MMPA packet stream: only complete packets can be passed up */
    if ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA || 
        rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) &&
        rrcb_ptr->cfg_ptr->packet_stream)
    {
      if (!HDRRLPRSQ_GET_FIRST_DATA_UNIT(reseq_item_ptr) ||
          !HDRRLPRSQ_GET_LAST_DATA_UNIT(reseq_item_ptr))
      {
        /* Skip this partial packet */
        continue;
      }

      /* EMPA out of order: skip packets that have */
      /* already been passed up to the Route Protocol. */
      if (rrcb_ptr->cfg_ptr->out_of_order_allowed)
      {
        already_passed_up_out_of_order = HDRRLPRSQ_GET_ALREADY_PASSED_UP_OUT_OF_ORDER(reseq_item_ptr);
        if (already_passed_up_out_of_order)
        {
          /* Skip this old packet */
          continue;
        }
      }

    }

    /* V(N) check */
    tmp_seq_num = HDRRLPRSQ_GET_SEQ_NUM(reseq_item_ptr);
    if (HDRRLPSEQ_GT_SEQ_CHECK(rrcb_ptr->state.needed_seq_num,
                              tmp_seq_num,
                              rrcb_ptr->cfg_ptr->seq_modulus))
    {

      /* Delete this item before sending up to higher layer */
      q_delete(reseq_q_ptr, &reseq_item_ptr->link);

      /* Sanity check to see Vn was updated correctly */
      end_seq_num = HDRRLPSEQ_ADD_SEQ( tmp_seq_num, 
                                       HDRRLPRSQ_GET_NUM_DATA_UNITS(reseq_item_ptr), 
                                       rrcb_ptr->cfg_ptr->seq_modulus );

      if( HDRRLPSEQ_GT_SEQ_CHECK(end_seq_num,
                           rrcb_ptr->state.needed_seq_num,
                           rrcb_ptr->cfg_ptr->seq_modulus) )
      {
        DATA_RLP_MSG6( MSG_LEGACY_ERROR, "RLP Rx[%d,%d] V[%d,%d] POSSIBLE ERROR IN V(N) COMPUTATION !!! Vn:%d EndSeq:%d",
                        rrcb_ptr->cfg_ptr->sr_id,
                        "AB"[(rrcb_ptr->route)],
                        rrcb_ptr->cfg_ptr->ver,
                        rrcb_ptr->cfg_ptr->enhanced,
                        rrcb_ptr->state.needed_seq_num,
                        end_seq_num );

        ASSERT(0);
      }

    } 
    else 
    {
      /* EMPA/MMPA out of order: don't stop at V(N) */
      if ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA || 
          rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) &&
          rrcb_ptr->cfg_ptr->packet_stream &&
          rrcb_ptr->cfg_ptr->out_of_order_allowed) 
      {
        /* Leave this item in the queue for duplicate detection */
        /* purposes, but mark it as sent up out of order */
        HDRRLPRSQ_SET_ALREADY_PASSED_UP_OUT_OF_ORDER(reseq_item_ptr, TRUE);
        num_data_units = HDRRLPRSQ_GET_NUM_DATA_UNITS(reseq_item_ptr);
        dsm_dup_len = dsm_length_packet(reseq_item_ptr);
        if (dsm_dup_packet(&reseq_item_ptr, reseq_item_ptr, 0, (uint16)dsm_dup_len) != dsm_dup_len)
        {
          ERR_FATAL("Dup did not succeed!",0,0,0);
        }
        
        if(reseq_item_ptr != NULL)
        {
          HDRRLPRSQ_SET_NUM_DATA_UNITS(reseq_item_ptr, num_data_units);
          DSM_TOUCH_ITEM(reseq_item_ptr);
        }
        else
        {
          DATA_RLP_MSG1( MSG_LEGACY_ERROR, 
                         "RLP - NULL dup reseq item: 0x%x", 
                         reseq_item_ptr);
        }

      }
      else
      {
        /* All done ;) */
        break;
      }
    }

    if ( reseq_item_ptr == NULL )
    {
      DATA_RLP_MSG1( MSG_LEGACY_ERROR, 
                     "RLP - NULL reseq item ptr: 0x%x", 
                     reseq_item_ptr);
      continue;
    }

    if (hdrrlp_debug_ptr->enable_rx_debug) 
    {
      DATA_RLP_MSG6 (MSG_LEGACY_MED, "RLP Rx[%d,%d] V[%d,%d] RSQ put pkt on WM seq:0x%x len:0x%x",
                      rrcb_ptr->cfg_ptr->sr_id,
                      "AB"[(rrcb_ptr->route)],
                      rrcb_ptr->cfg_ptr->ver,
                      rrcb_ptr->cfg_ptr->enhanced,
                      HDRRLPRSQ_GET_SEQ_NUM(reseq_item_ptr),
                      dsm_length_packet(reseq_item_ptr));
    }

    num_data_units = HDRRLPRSQ_GET_NUM_DATA_UNITS(reseq_item_ptr);
    pkt_ptr = NULL;

    /*-------------------------------------------------------------------
      Make the app_ptr in the received packet as NULL as PS code expects 
      it to bo NULL
    ------------------------------------------------------------------*/
    HDRRLPRSQ_SET_NUM_DATA_UNITS(reseq_item_ptr, 0);

    /*-------------------------------------------------------------------
      The While loop below is to Cap the DSM Chain enqueued to the 
      Watermark to be capped @ HDRRLPRSQ_MAX_LEN_DSM_PACKET_ENQ bytes.
      This is because of a limitation of the SHIM layer to accept higher 
      packet lenghts. 
    ------------------------------------------------------------------*/
    while (num_data_units > HDRRLPRSQ_MAX_LEN_DSM_PACKET_ENQ)
    {
      if ( dsm_split_packet( &reseq_item_ptr, 
                        &pkt_ptr, 
                        HDRRLPRSQ_MAX_LEN_DSM_PACKET_ENQ, 
                        DSM_DUP_ITEM_POOL ) != HDRRLPRSQ_MAX_LEN_DSM_PACKET_ENQ )
      {
        DATA_RLP_MSG3( MSG_LEGACY_MED,"Cannot Split the DSM Item Freeing the DSM Items",
                        num_data_units,
                        dsm_length_packet( reseq_item_ptr ),
                        dsm_length_packet( pkt_ptr ) );

         dsm_free_packet(&reseq_item_ptr);                        
         dsm_free_packet(&pkt_ptr);                        
         break;
      }

      DATA_RLP_MSG3( MSG_LEGACY_MED,"Splitting DSM item [%d] into smaller chunks [%d, %d]",
                      num_data_units,
                      dsm_length_packet( reseq_item_ptr ),
                      dsm_length_packet( pkt_ptr ) );

      /*-------------------------------------------------------------------
        Put data on to receive queue and call registered callback fcn.
      ------------------------------------------------------------------*/
      dsm_enqueue(rrcb_ptr->cfg_ptr->rx_wm_ptr[rrcb_ptr->route], &reseq_item_ptr);
      reseq_item_ptr = pkt_ptr;
      pkt_ptr = NULL;

      num_data_units -= HDRRLPRSQ_MAX_LEN_DSM_PACKET_ENQ;
    }

    /*-------------------------------------------------------------------
      After splitting bytes in sizes of HDRRLPRSQ_MAX_LEN_DSM_PACKET_ENQ
      if some more bytes are left
      NOTE: Reseq Item Ptr can be NULL when num_data_units > 0, if there are 
      no DSM_DUP_ITEMS available.
    ------------------------------------------------------------------*/
    if ( num_data_units > 0 &&
         reseq_item_ptr != NULL)
    {
      dsm_enqueue(rrcb_ptr->cfg_ptr->rx_wm_ptr[rrcb_ptr->route], &reseq_item_ptr);
    }

    if (hdrrlp_debug_ptr->enable_rx_debug) 
    {
      DATA_RLP_MSG5 (MSG_LEGACY_MED, "RLP Rx[%d,%d] V[%d,%d] RSQ Processed a data packet from WM %p",
                      rrcb_ptr->cfg_ptr->sr_id,
                      "AB"[(rrcb_ptr->route)],
                      rrcb_ptr->cfg_ptr->ver,
                      rrcb_ptr->cfg_ptr->enhanced,
                      rrcb_ptr->cfg_ptr->rx_wm_ptr[rrcb_ptr->route]);
    }

    rrcb_ptr->cfg_ptr->rx_func_ptr(rrcb_ptr->cfg_ptr->rx_data);
  }
}

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
void hdrrlprsq_update_vn_upon_nak_abort
(
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr
)
{
  dsrlpi_state_info_type       *state_ptr;     /*  curr state              */ 
  q_type                       *reseq_q_ptr;   /*  ptr to rlp's reseq list */
  dsm_item_type                *reseq_item_ptr;/*  reseq list entry        */
  uint32                        start_seq_num;
  uint32                        end_seq_num;
  uint32                        modulus;       /* sequence modulus         */
  uint32                        old_vn;
  hdrrlp_debug_ctl             *hdrrlp_debug_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  state_ptr = &(rrcb_ptr->state);
  reseq_q_ptr = &(state_ptr->reseq_q);
  modulus   = rrcb_ptr->cfg_ptr->seq_modulus;
  hdrrlp_debug_ptr = get_hdrrlp_debug_ptr();

  reseq_item_ptr = (dsm_item_type*)q_check(reseq_q_ptr);
  while (reseq_item_ptr != NULL)
  {
    start_seq_num = HDRRLPRSQ_GET_SEQ_NUM(reseq_item_ptr);
    end_seq_num = HDRRLPSEQ_ADD_SEQ(start_seq_num,
                                      HDRRLPRSQ_GET_NUM_DATA_UNITS(reseq_item_ptr),
                                      modulus);
    if (HDRRLPSEQ_GT_SEQ_CHECK(start_seq_num,
                              state_ptr->needed_seq_num,
                              modulus))
    {
      break;
    }
    
    /* If V(n) >= start_seq_num && V(n) < end_seq_num, 
        then update V(n) to end_seq_num */
    if (HDRRLPSEQ_GE_SEQ_CHECK(state_ptr->needed_seq_num,
                              start_seq_num,
                              modulus)
        && HDRRLPSEQ_GT_SEQ_CHECK(end_seq_num,
                                 state_ptr->needed_seq_num,
                                 modulus))
    {
      old_vn = state_ptr->needed_seq_num;
      
      state_ptr->needed_seq_num = end_seq_num;

      if (hdrrlp_debug_ptr->enable_rx_debug) 
      {
        DATA_RLP_MSG6 (MSG_LEGACY_MED, "RLP Rx[%d,%d] V[%d,%d] V(n) moved 0x%x to 0x%x",
                        rrcb_ptr->cfg_ptr->sr_id,
                        "AB"[(rrcb_ptr->route)],
                        rrcb_ptr->cfg_ptr->ver,
                        rrcb_ptr->cfg_ptr->enhanced,
                        old_vn, 
                        rrcb_ptr->state.needed_seq_num);
      }
    }
    reseq_item_ptr = (dsm_item_type*)q_next(reseq_q_ptr, &(reseq_item_ptr->link));
  }

  return;
}

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
uint32 hdrrlprsq_get_seq_num
(
  dsm_item_type *reseq_item_ptr
)
{
  return HDRRLPRSQ_GET_SEQ_NUM( reseq_item_ptr );
}

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
uint32 hdrrlprsq_get_num_data_units
(
  dsm_item_type *reseq_item_ptr
)
{
  return HDRRLPRSQ_GET_NUM_DATA_UNITS( reseq_item_ptr );
}

#endif /* FEATURE_HDR */

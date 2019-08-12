/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                                              
      R A D I O   L I N K   P R O T O C O L   T X Q   P R O C E S S I N G

GENERAL DESCRIPTION
  
  This file contains generic Transmit Queue (Sent Frame Queue) processing 
  functions. The Transmit Queue is a queue of all the frames the mobile RLP
  has sent. The Transmit Queue is used to buffer sent frames, that might
  be used if the other side wants a retransmission. 
  
  The transmit queue is not  specific to any one RLP type, but is a general
  purpose data structure.
  
  It is assumed that the user will not directly access any tx_q information
  contained in the tx_q structure.  Instead, all tx_q access can be done
  via the externalized functions or macros provided in this file and the
  dsrlptxq.h file.
  
  The transmit queue is implemented as a variable size queue. 
    
  The transmit queue assumes that each stored element is a chain of
  dsm items, which make up one unsegmented RLP frame.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  dsrlptxq_init() must be called before any other TX_Q functions can be
  called. 
  
DESCRIPTION
  

 Copyright (c) 1999-2012 by Qualcomm Technologies Incorporated.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsrlptxq.c_v   1.11   02 Dec 2002 21:56:20   sarithay  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/1xrlp/src/dsrlptxq.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        -------------------------------------------------------
12/03/12   msh        Initialize the queues only once to avoid memory leak
01/18/11   op         Migrated to MSG 2.0 Macros
11/22/10   sn         Removed REX premption macros as it is not required in 
                      dsrlptxq_reset().
12/17/09   ms         Compiler Warning fix.
09/22/09   sn         Fixed klocwork errors.
05/10/07   ac         Lint fix
05/30/06   ksu        EMPA: RLP init code review comments
12/04/05   ksu        EMPA segment sequencing: don't fragment for Format B
10/27/05   ksu        EMPA: octet stream, octet sequencing
11/17/05   sy         Handling the sequence number wrap around in the TXQ.
09/20/05   ksu/sy     Don't wrap sequence numbers in Tx queue.
09/20/04   vas        QOS related changes
12/02/02   sy         Added macros for footer as the first 4 bits of the 
                      app_field are used by the footer along with the kind.
10/15/02   js         Added RLP identifier to differentiate different RLP 
                      instances. 
10/26/01   ap         Reduced num ack frames to be searched for puma non-iram
09/25/01   hcg        Fixed memory leak for 1xEV flush packets in 
                      dsrlptxq_access_last_frame() to use q_last_check().
05/24/01   hcg        Made changes to support HDR (1xEV).
04/16/01   ak         Integrate new pointer which allows faster access to 
                      txq members for re-transmissions.
02/27/01   ak         Added remove_acked() and access_frame() to IRAM
02/21/01   ak         Increased queue size to allow for 3 rounds of NAKs
                      when R-SCH is 16X.
02/05/01   ak         Func remove_acked is now executed in Tx task, so big
                      INTLOCK'd section no longer needed.  Also limit # of
                      frames that are removed in one call to function.
01/16/01   igt        Removed a couple compiler warnings.
11/14/00   igt        Changed the implementation of the data structure to a 
                      queue instead of an array of pointers to dsm items.
06/15/00   ak         Fixed bug where the end_seq_num was not being correctly
                      calculated, access_frame().
06/07/00   rc         Replaced the variable txq_seq_num with app_field (a new
                      user defined field in dsm_item_type).
01/19/00   ak         Fixed some ADD_S_SEQ issues with adding the correct
                      amount.
01/10/00   na         Reduced RLPTXQ_ARRAY_SIZE to 128.
01/07/00   ak         Added function to remove last element from q.  Also
                      adjusted code to correctly use INTLOCKs.
08/05/99 na/hcg/ak    Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "data_msg.h"

#if defined( FEATURE_DS_RLP3)
#include "msg.h"
#include "dsrlp_v.h"
#include "dsrlpi.h"
#include "dsrlpseq.h"
#include "dsrlptxq.h"
#include "amssassert.h"
#include "err.h"
#include "dsm.h"


/*===========================================================================

             PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/*---------------------------------------------------------------------------
  The transmitted RLP frames are stored in a queue of dsm items. The DSM 
  items contain the completely formatted RLP frame.
  
  The DSM item's link field will be union'ized to also be a 4-byte seq num.
  This will allow for quick seq number searches.
 
  The DSM item's kind field will be overloaded to store the muxpdu type 
  of the frame, number of bytes in header and number of bytes in the footer.
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Max number of items to remove when we receive a peer_vn from the BS.  This
  is so that the remove_acked() function doesn't run for a long time.  Make
  it a multiple of the max number of frames which could be generated in
  one frame time.
  For Puma (MSM5100), non-IRAM build, we reduce the number to be searched in one 
  frame time to shorten the overall time for RLP to build a frame
---------------------------------------------------------------------------*/

#if defined(T_MSM_5100) && !defined(FEATURE_IRAM)  
  #define DSRLPTXQ_MAX_REMOVED 15 
#else
  #define DSRLPTXQ_MAX_REMOVED  (RLP_MAX_FWD_PDU * 3)
#endif /* #if defined(T_MSM_5100) && defined(FEATURE_IRAM) */

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                        MACRO DEFINITIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/
/*===========================================================================

MACRO         RLPTXQ_PUT_HDR_LEN

DESCRIPTION   This macro puts the length of the frame header into the 
              4 bottom, unused bits of the sequence number (which is a 32-
              bit number).

PARAMETERS    the 32-bit seq number and the header length.

DEPENDENCIES  None

RETURN VALUE  the modified seq_number

SIDE EFFECTS  None
===========================================================================*/
#define RLPTXQ_PUT_HDR_LEN(seq,len ) \
  (seq | (((uint32)len) & 0x0F))


/*==========================================================================

MACRO         RLPTXQ_PUT_FTR_LEN

DESCRIPTION   This macro puts the length of the frame footer into the 
              4 top, unused bits of the sequence number (which is a 32-
              bit number).

PARAMETERS    the 32-bit seq number and the header length.

DEPENDENCIES  None

RETURN VALUE  the modified seq_number

SIDE EFFECTS  None
===========================================================================*/
#define RLPTXQ_PUT_FTR_LEN( item, footer_len )    \
   {                                              \
     item->kind = (footer_len & 0xFF);            \
     item->app_field = ( item->app_field          \
	                     | ((((uint32)footer_len) << 20) & 0xF0000000 ) ); \
   }


/*===========================================================================

MACRO         RLPTXQ_GET_FTR_LEN

DESCRIPTION   This macro gets the length of the frame header from the 
              4 top bits of the sequence number (which is a 32-
              bit number).

PARAMETERS    the 32-bit seq number

DEPENDENCIES  None

RETURN VALUE  the footer length

SIDE EFFECTS  None
===========================================================================*/
#define RLPTXQ_GET_FTR_LEN( item ) \
   ((uint16)((item->app_field & 0xF0000000) >> 20) | ((uint16)item->kind))


/*===========================================================================

MACRO         RLPTXQ_GET_HDR_LEN

DESCRIPTION   This macro gets the length of the frame header from the 
              4 bottom bits of the sequence number (which is a 32-
              bit number).

PARAMETERS    the 32-bit seq number

DEPENDENCIES  None

RETURN VALUE  the header length

SIDE EFFECTS  None
===========================================================================*/
#define RLPTXQ_GET_HDR_LEN( seq ) \
   (byte)(seq & 0x0F)



/*===========================================================================

MACRO         RLPTXQ_GET_SEQ

DESCRIPTION   This macro gets the sequence number from the 32-bit
              value which contains both the seq. number and the header
              length.

PARAMETERS    the 32-bit seq number

DEPENDENCIES  None

RETURN VALUE  the 32-bit seq number.

SIDE EFFECTS  None
===========================================================================*/
#define RLPTXQ_GET_SEQ( seq ) \
   (seq & 0x0FFFFFF0)

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                    INTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/


/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                    PUBLIC FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/

/*===========================================================================

FUNCTION      DSRLPTXQ_INIT

DESCRIPTION   This function is called up from dsrlp_init() during the RLP 
              session initialization.  It initializes the rlp TX Queue.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dsrlptxq_init
( 
  dsrlpi_sess_ctl_blk_type *rscb_ptr        /* current RLP session ctl blk */
)
{
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr;       /* current RLP ctl blk         */
  int p;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  ASSERT( rscb_ptr != NULL);
  
  for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
  {
    rrcb_ptr = &rscb_ptr->rlp_list[p];
    rrcb_ptr->state.tx_q_recent = NULL;
  }

}/* dsrlptxq_init() */


/*===========================================================================

FUNCTION      DSRLPTXQ_RESET
             
DESCRIPTION   This goes ahead and clears out a transmit q of all of its
              items.  Called from dsrlpi_reset() if RLP is reset or gotten
              rid of.  

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dsrlptxq_reset
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr         /* current RLP ctl blk        */
)
{
  q_type             *tx_q_ptr;              /*   ptr to rlp's reseq q     */
  dsm_item_type      *tx_entry_ptr;          /*   reseq  list entry        */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  ASSERT(rrcb_ptr != NULL);
  tx_q_ptr = &(rrcb_ptr->state.tx_q);
   
  /*-------------------------------------------------------------------------
    Now you can free up the rest of the contents of the transmit Q.
    Make sure the app_field is set to 0, before calling dsm_free_packet.

    Removed disable and enable premption as it is not required while 
    freeing DSM packet. RLP code in many places frees the DSM item without 
    mutual exclusion and hence removing the mutual exclusion here as well.
  -------------------------------------------------------------------------*/
  while ((tx_entry_ptr = (dsm_item_type *)q_get(tx_q_ptr)) != NULL)
  {
    tx_entry_ptr->app_field = 0;
    dsm_free_packet( &tx_entry_ptr);
  }/* while() */

  rrcb_ptr->state.tx_q_recent = NULL;

}/* dsrlptxq_reset() */


/*===========================================================================

FUNCTION      DSRLPTXQ_ADD_FRAME

DESCRIPTION   This function adds a new frame into the transmit queue. This
              function is called whenever RLP sends a new frame. It runs in 
              the context of the task that does RLP TX.
              
              If the txq is full, then the oldest frame is removed from the
              list to make room for the newest frame.

DEPENDENCIES  Called from Tx task.

RETURN VALUE  None

SIDE EFFECTS  The input RLP frame is stored in a potential chain of dsm 
              items.  The first dsm item's q_link field will be used to
              hold the sequence number of the frame.  
===========================================================================*/
void dsrlptxq_add_frame
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,    /* current RLP ctl blk        */
  dsrlptxq_fr_param_type       *fr_param_ptr /* Info. of fr. to be inserted*/
)
{
  q_type                       *tx_q_ptr;     /* ptr to the tx q           */
  uint16                        q_items;      /* holds # of items in the q */
  dsm_item_type                *free_item_ptr;/* points to  item to free   */ 
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- -*/

  ASSERT(rrcb_ptr != NULL);
  tx_q_ptr = &(rrcb_ptr->state.tx_q);

  /*-------------------------------------------------------------------------
    This assumes no chains. In other words it assumes that the dsm item
    size is enough to hold a frame. Otherwise the max size of the
    q will not necessarily be limited to the number of version-specific TXQ 
    size dsm items.
    -------------------------------------------------------------------------*/
  q_items = q_cnt( tx_q_ptr);
  if ( q_items  >= rrcb_ptr->cfg_ptr->ver_ptr->txq_size ) 
  {
    /*-----------------------------------------------------------------------
      In this case we need to take an item out of the head of queue
      before inserting the new one at the tail of the queue
      -----------------------------------------------------------------------*/
    free_item_ptr = (dsm_item_type *)q_get( tx_q_ptr);
    ASSERT(free_item_ptr != NULL);

    free_item_ptr->app_field = 0;
    if (free_item_ptr == rrcb_ptr->state.tx_q_recent)
    {
      rrcb_ptr->state.tx_q_recent = NULL;
    }
    dsm_free_packet(&free_item_ptr);
  }

  /*-------------------------------------------------------------------------
    This file assumes that sequence numbers do not wrap within the Tx queue.
    -------------------------------------------------------------------------*/
  while ((free_item_ptr = (dsm_item_type *)q_check(tx_q_ptr)) != NULL) 
  {

    /*-------------------------------------------------------------------------
      Remove all the items in the TX-Q that have a sequence number that can cause
      wrap around confusion. So any sequence numbers that are greater than the
      sequence number that we are adding are considered to be stale items and need
      to be removed.
      -------------------------------------------------------------------------*/
    if( DSRLPSEQ_GT_SEQ_CHECK(RLPTXQ_GET_SEQ(free_item_ptr->app_field),
                              fr_param_ptr->seq_num,
                              rrcb_ptr->cfg_ptr->seq_modulus))
    {
      /*-----------------------------------------------------------------------
        In this case we need to take an item out of the head of queue
        before inserting the new one at the tail of the queue
        -----------------------------------------------------------------------*/
      free_item_ptr = (dsm_item_type *)q_get( tx_q_ptr);
      ASSERT(free_item_ptr != NULL);

      free_item_ptr->app_field = 0;
      if (free_item_ptr == rrcb_ptr->state.tx_q_recent)
      {
        rrcb_ptr->state.tx_q_recent = NULL;
      }
      dsm_free_packet(&free_item_ptr);
    }
    else
    {
      /*-------------------------------------------------------------------------
        The first element in the transmit queue has a sequence number less than
        the seq number of element being added. Thus, break
        -------------------------------------------------------------------------*/
      break;
    }
  }


  /*-------------------------------------------------------------------------
    NOTE: Do not change the order of assignmnet of header and footer.

    First four bits of the app_field have been used for storing the
    footer information once the footer value goes over 255. The order
    is important because the header info needs to be placed before the
    footer.
    -------------------------------------------------------------------------*/
  fr_param_ptr->fr_ptr->app_field = 
    RLPTXQ_PUT_HDR_LEN(fr_param_ptr->seq_num,fr_param_ptr->header);
  RLPTXQ_PUT_FTR_LEN( fr_param_ptr->fr_ptr, fr_param_ptr->footer );

  /*-------------------------------------------------------------------------
    Add a new frame, just insert it at the tail  of the queue 
    -------------------------------------------------------------------------*/
  q_put(tx_q_ptr, &((fr_param_ptr->fr_ptr)->link) );

} /* dsrlptxq_add_frame () */


#ifndef MSM5000_IRAM_REV
/*===========================================================================

FUNCTION      DSRLPTXQ_REMOVE_ACKED

DESCRIPTION   Given the peer's L_V(N), this function removes from the array
              any frames whose sequence number is less than the input L_V(N).
              
              Only remove a certain number of items at a certain time, as
              the removal of a lot of items could take a long time.
              
DEPENDENCIES  Called from Tx task.  It is called from their so that there 
              no need to do INTLOCKs, as this could take a long time.

RETURN VALUE  None

SIDE EFFECTS  NONE
===========================================================================*/
void dsrlptxq_remove_acked
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,   /* Ptr to RLP ctl block.       */
  uint32                        l_vn        /* Peer's L_V(N), formatted to */
                                            /* internal 32-bit format.     */
)
{
  uint32                     seq_num;        /* holds frame's seq num      */
  byte                       hdr;            /* header length              */
  uint16                     ftr;            /* footer length              */
  word                       payload_len;    /* length of stored payload   */
  uint32                     end_seq_num;    /* last seq num in a pkt      */
  q_type                    *tx_q_ptr;       /* ptr to rlp's tx q          */
  dsm_item_type             *free_item_ptr;  /* ptr to item to free        */
  dsm_item_type             *walk_item_ptr;  /* ptr to walk the queue      */
  word                       frames_removed; /* # frames taken out of list */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- -*/

  ASSERT(rrcb_ptr != NULL);
  tx_q_ptr = &(rrcb_ptr->state.tx_q);
  
  /*-------------------------------------------------------------------------
    get a pointer to the head of the queue
    -------------------------------------------------------------------------*/
  walk_item_ptr = (dsm_item_type *)q_check(tx_q_ptr);

  /*-------------------------------------------------------------------------
    If queue is empty, then return immediately.
    -------------------------------------------------------------------------*/
  if ( walk_item_ptr == NULL ) 
  {
    return;
  }
 
  /*-------------------------------------------------------------------------
    Queue is non-empty.  Walk through the q and delete any elements
    whose sequence number is less than the l_vn.  Be sure *not* to delete if
    the sequence number is equal to l_vn.  
    -------------------------------------------------------------------------*/
  frames_removed = 0;

  do
  {
    /*-----------------------------------------------------------------------
      Compare l_vn against stored element's seq_num, and delete that element
      if necessary.
      -----------------------------------------------------------------------*/
    seq_num = RLPTXQ_GET_SEQ( walk_item_ptr->app_field);

    if (DSRLPSEQ_GT_SEQ_CHECK(l_vn,
                              seq_num,
                              rrcb_ptr->cfg_ptr->seq_modulus
                              ))
    {
      /*--------------------------------------------------------------------
        See if whole chain can be removed, or if only a small part.
        --------------------------------------------------------------------*/
      hdr = RLPTXQ_GET_HDR_LEN(seq_num);
      ftr = RLPTXQ_GET_FTR_LEN(walk_item_ptr);

      payload_len = (word)dsm_length_packet(walk_item_ptr);
      payload_len = payload_len - (hdr + ftr);

      if (payload_len > 0)
      {
        /* EMPA/MMPA segment sequencing: 1 data unit per retx'd RLP packet */
        if ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA ||
            rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA) &&
            rrcb_ptr->cfg_ptr->segment_sequencing)
        {
          end_seq_num = seq_num;
        }
        else
        {
          end_seq_num = DSRLPSEQ_ADD_S_SEQ(seq_num,
                                           (payload_len - 1),
                                           rrcb_ptr->cfg_ptr->seq_modulus);
        }

      }
      else
      {
        DATA_RLP_MSG0(MSG_LEGACY_ERROR,"Transmitted a zero length rlp frame?");
        end_seq_num = seq_num;
      }

      if (DSRLPSEQ_GE_SEQ_CHECK(l_vn,
                                end_seq_num,
                                rrcb_ptr->cfg_ptr->seq_modulus))
      {
        
        /*------------------------------------------------------------------
          Can remove whole chain. This is in a queue, so you can not just 
          free the packet. First you have to take it out of the queue.
          ------------------------------------------------------------------*/
        free_item_ptr = (dsm_item_type *)q_get(tx_q_ptr);
        ASSERT(free_item_ptr != NULL);

        free_item_ptr->app_field = 0;
        if (free_item_ptr == rrcb_ptr->state.tx_q_recent)
        {
          rrcb_ptr->state.tx_q_recent = NULL;
        }
        dsm_free_packet(&free_item_ptr);
        frames_removed++;
      }
      else
      {
        /*------------------------------------------------------------------
          Can only remove part of chain...leave it alone, and exit from 
          loop.
          ------------------------------------------------------------------*/
        break;
      }
    }
    else
    {
      /*--------------------------------------------------------------------
        Remaining elements have seq_num >= l_vn.  Therefore, loop is 
        finished
        --------------------------------------------------------------------*/
      break;
    }
  }
  while (((walk_item_ptr = (dsm_item_type *)q_check(tx_q_ptr) ) != NULL) &&
         (frames_removed < DSRLPTXQ_MAX_REMOVED)
         );
}/* dsrlptxq_remove_acked() */
#endif /* MSM5000_IRAM_REV */

#ifndef MSM5000_IRAM_REV
/*===========================================================================

FUNCTION      DSRLPTXQ_ACCESS_FRAME

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

RETURN VALUE  If the packet was found, then returns a non-zero value, 
              which indicates the length of the payload in the returned
              dsm packet chain (which contains the relevant RLP frame). 
              
              The passed fr_info_ptr is completely filled out with the info
              from the found dsm packet chain. The fr_info_ptr struct 
              contains a pointer to the head of the dsm pkt chain.
              
              next_seq_num is loaded with the seq_num of the next element
              in the txq.  If there is no next element, then next_seq_num
              is set equal to the requested sequence number.
              
              If the packet was not found, it returns zero.  The input
              fr_info_ptr is unchanged.  next_seq_num is loaded with the 
              seq num of the first (i.e., oldest) element in the queue.
              
              If the queue is in fact empty, then next_seq_num is unchanged.

SIDE EFFECTS  NONE
===========================================================================*/
word dsrlptxq_access_frame
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,    /* Ptr to RLP ctl block       */
  dsrlptxq_fr_param_type       *fr_info_ptr, /* points to req'd frame info */
  uint32                       *next_seq_num /* filled with next seq num in*/
                                             /* list                       */
)
{
  uint32                     req_seq_num;    /* requested seq num          */
  uint32                     ith_seq_num;    /* holds frame's seq num      */
  uint32                     jth_seq_num;    /* holds frame's seq num      */
  byte                       hdr;            /* header length              */
  uint16                     ftr;            /* footer length              */
  word                       payload_len;    /* length of stored payload   */
  uint32                     end_seq_num;    /* last seq num in a pkt      */
  q_type                    *tx_q_ptr;       /* ptr to rlp's reseq q       */
  uint16                     q_items;        /* hold # of items in queue   */
  uint16                     examined;       /* keep track of # "examined" */
  dsm_item_type             *walk_item_ptr;  /* ptr to walk the queue      */
  dsm_item_type             *next_item_ptr;  /* ptr to next item           */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- -*/

  ASSERT(rrcb_ptr != NULL);
  tx_q_ptr = &(rrcb_ptr->state.tx_q);
  
  q_items     = q_cnt(tx_q_ptr);
  req_seq_num = fr_info_ptr->seq_num;
  payload_len = 0;

  /*-------------------------------------------------------------------------
    If queue is empty, then return immediately.
    -------------------------------------------------------------------------*/
  if (q_items == 0) 
  {
    return(payload_len);
  }
  
  /*-------------------------------------------------------------------------
    do initializations before the do{}while()
    -------------------------------------------------------------------------*/
  examined = 0;
  
  /*-------------------------------------------------------------------------
    get a pointer to the begining of the queue
    we know it has at least one element
    -------------------------------------------------------------------------*/
  walk_item_ptr = (dsm_item_type *)q_check(tx_q_ptr);
  ASSERT( walk_item_ptr != NULL);

  /*-------------------------------------------------------------------------
    If had previously accessed the queue, and the info prev pointed at is
    more efficient, start search there.
    -------------------------------------------------------------------------*/
  if (rrcb_ptr->state.tx_q_recent != NULL)
  {
    if (DSRLPSEQ_GE_SEQ_CHECK(req_seq_num,
                              RLPTXQ_GET_SEQ(rrcb_ptr->state.tx_q_recent->app_field),
                              rrcb_ptr->cfg_ptr->seq_modulus))
    {
      walk_item_ptr = rrcb_ptr->state.tx_q_recent;
    }
  }

  /*-------------------------------------------------------------------------
    Loop through transmit queue, until the element is found.  Only continue
    through loop if the requested sequence number is greater than the current
    one in the list.  Since q_items != 0, there is at least one element in
    the queue.
    -------------------------------------------------------------------------*/
  do 
  {
    examined++;
    /*-----------------------------------------------------------------------
      point to the next element in the queue. It may be NULL
      -----------------------------------------------------------------------*/
    next_item_ptr = (dsm_item_type *)q_next(tx_q_ptr, &(walk_item_ptr->link));
    
    /*-----------------------------------------------------------------------
      To make the code readable, extract the sequence numbers and place
      them in the local vars.
      -----------------------------------------------------------------------*/
    ith_seq_num = RLPTXQ_GET_SEQ( walk_item_ptr->app_field);
    
    /*-----------------------------------------------------------------------
      if this is the very last item we are looking at right now
      -----------------------------------------------------------------------*/
    if ( next_item_ptr == NULL )
    {
      /*---------------------------------------------------------------------
        Either one element in list or no other elements.  If the req'd 
        seq_num is >= i'th element's seq_num,then check against that element.
        ---------------------------------------------------------------------*/
      hdr = RLPTXQ_GET_HDR_LEN(walk_item_ptr->app_field);
      ftr = RLPTXQ_GET_FTR_LEN(walk_item_ptr);

      payload_len = (word)dsm_length_packet(walk_item_ptr);
      payload_len = payload_len - (hdr + ftr);

      if (payload_len > 0)
      {
        /* EMPA/MMPA segment sequencing: 1 data unit per retx'd RLP packet */
        if ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA ||
            rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA) &&
            rrcb_ptr->cfg_ptr->segment_sequencing)
        {
          end_seq_num = ith_seq_num;
        }
        else
        {
          end_seq_num = DSRLPSEQ_ADD_S_SEQ(ith_seq_num,
                                           (payload_len - 1),
                                           rrcb_ptr->cfg_ptr->seq_modulus);
        }
      }
      else
      {
        DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Accessing a zero length rlp frame?");
        end_seq_num = ith_seq_num;
      }
       
      if (DSRLPSEQ_GE_SEQ_CHECK(req_seq_num,
                                ith_seq_num,
                                rrcb_ptr->cfg_ptr->seq_modulus)     &&
          DSRLPSEQ_GE_SEQ_CHECK(end_seq_num,
                                req_seq_num,
                                rrcb_ptr->cfg_ptr->seq_modulus))
      {
        fr_info_ptr->seq_num     = ith_seq_num;
        fr_info_ptr->fr_ptr      = walk_item_ptr;
        fr_info_ptr->header      = hdr;
        fr_info_ptr->footer      = ftr;

        *next_seq_num = req_seq_num;
      }
      else
      {
        /*-------------------------------------------------------------------
          reset payload_len to indicate the frame was not found
          -------------------------------------------------------------------*/
        payload_len = 0;
      }
      break;
    }
    else /* more items in queue */
    {
      /*---------------------------------------------------------------------
        this should never be NULL if the code got here
        ---------------------------------------------------------------------*/
      ASSERT(next_item_ptr != NULL);

      /*---------------------------------------------------------------------
        There is at least one element after the i'th.  See if the requested
        sequence number falls in between the i'th and j'th frames
        ---------------------------------------------------------------------*/
      jth_seq_num = RLPTXQ_GET_SEQ(next_item_ptr->app_field);

      if (DSRLPSEQ_GE_SEQ_CHECK(req_seq_num,
                                ith_seq_num,
                                rrcb_ptr->cfg_ptr->seq_modulus)     &&
          DSRLPSEQ_GT_SEQ_CHECK(jth_seq_num,
                                req_seq_num,
                                rrcb_ptr->cfg_ptr->seq_modulus))
      {
        /*-------------------------------------------------------------------
          Requested seq_num lies between this and the next seq num.  See if
          the i'th packet contains the correct number of bytes, such that
          it in fact does contain the seq num requested.
          -------------------------------------------------------------------*/
        hdr = RLPTXQ_GET_HDR_LEN(walk_item_ptr->app_field);
        ftr = RLPTXQ_GET_FTR_LEN(walk_item_ptr);

        payload_len = (word)dsm_length_packet(walk_item_ptr);
        payload_len = payload_len - (hdr + ftr);

        if (payload_len > 0)
        {
          /* EMPA/MMPA segment sequencing: 1 data unit per retx'd RLP packet */
          if ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA ||
              rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA) &&
              rrcb_ptr->cfg_ptr->segment_sequencing)
          {
            end_seq_num = ith_seq_num;
          }
          else
          {
            end_seq_num = DSRLPSEQ_ADD_S_SEQ(ith_seq_num,
                                             (payload_len - 1),
                                             rrcb_ptr->cfg_ptr->seq_modulus);
          }
        }
        else
        {
          DATA_RLP_MSG0(MSG_LEGACY_ERROR,"Accessing a zero length rlp frame?");
          end_seq_num = ith_seq_num;
        }

        if (DSRLPSEQ_GE_SEQ_CHECK(end_seq_num,
                                  req_seq_num,
                                  rrcb_ptr->cfg_ptr->seq_modulus))
        {
          /*-----------------------------------------------------------------
            The i'th element does contain the correct info.  Fill out the
            return struct with proper info.
            -----------------------------------------------------------------*/
          fr_info_ptr->seq_num     = ith_seq_num;
          fr_info_ptr->fr_ptr      = walk_item_ptr;
          fr_info_ptr->header      = hdr;
          fr_info_ptr->footer      = ftr;

          *next_seq_num = jth_seq_num;
        }
        else
        {
          /*-----------------------------------------------------------------
            reset payload_len to indicate the frame was not found 
            -----------------------------------------------------------------*/
          payload_len = 0;
        } 
        break;
      }
      else if (DSRLPSEQ_GE_SEQ_CHECK(ith_seq_num,
                                     req_seq_num,
                                     rrcb_ptr->cfg_ptr->seq_modulus
                                     ))
      {
        /*-------------------------------------------------------------------
          requested seq_num is now less than remaining stored frames.  time
          to get out of the loop.
          -------------------------------------------------------------------*/
        break;
      }
    }
  } while((walk_item_ptr = (dsm_item_type *)q_next
           (tx_q_ptr, &(walk_item_ptr->link))) != NULL);
  

  DATA_RLP_MSG2(MSG_LEGACY_LOW, "TXQ Examined = %d RLP: %d",examined,rrcb_ptr->cfg_ptr->sr_id);

  if (payload_len == 0)
  {
    /*-----------------------------------------------------------------------
      The requested frame was not found.  Load next_seq_num with the value
      of the first (oldest) seq_num in the queue.
      -----------------------------------------------------------------------*/
    rrcb_ptr->state.tx_q_recent = NULL;
    walk_item_ptr = (dsm_item_type *)q_check(tx_q_ptr);
    ASSERT(walk_item_ptr != NULL);
    *next_seq_num = RLPTXQ_GET_SEQ(walk_item_ptr->app_field);
  }
  else
  {
    rrcb_ptr->state.tx_q_recent = fr_info_ptr->fr_ptr;
  }

  return (payload_len);
} /* dsrlptxq_access_frame() */
#endif /* MSM5000_IRAM_REV */

#endif /* any of the RLPs defined */

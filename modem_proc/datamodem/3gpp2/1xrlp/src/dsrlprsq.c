/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  R A D I O   L I N K   P R O T O C O L  R E S E Q U E N C I N G  Q U E U E  
  
                          P R O C E S S I N G  

GENERAL DESCRIPTION
  
  This file contains generic RSQ-list processing functions for the RLP
  software. It is not specific to any one RLP type.

  It is assumed that the user will not directly access any RSQ information
  contained in the RSQ structure.  Instead, all RSQ access can be done
  via the externalized functions or macros provided in this file and the
  dsrlprsq.h file.
  
  The  resequencing queue is simply a queue of small dsm items.  This takes
  advantage of the embedded q_link_type inside each dsm item.
  
  This module consists of four functions:
 
  dsrlprsq_init() is called once from within dslrp_init() to initialize the
  resequencing queue at the beginning of an RLP session. 
 
  dsrlprsq_reset() is called from within dsrlpi_reset() each time an RLP
  instance resets. It will take any resequencing queue entries with sequence
  numbers up until LV(N) and convey them to upper layers. Any remaining items
  in the queue will be freed.
 
  dsrlprsq_remove() removes as many items as possible from the resequencing
  queue, subject to limitations from the max_chain_cnt parameter and the
  sequence  numbers of items relative to LV(N). So it will keep removing
  until it removes max_chain_cnt chains or finds a dsm item chain with
  sequence number greater than LV(N). 
 
  It is called from within dsrlp_process_rx_frames(),
  dsrlpi_process_rexmit_frame(), dsrlprsq_reset(), and dsrlprsq_insert(). 
  Users may optimize the value of maximum chains to remove with
  DSRLPI_MAX_CHAINS_TO_REMOVE or set it to zero which means the only
  restriction to removing items from the resequencing queue is their sequence
  number relative to LV(N). 
  
  dsrlprsq_insert() is the function that inserts out-of-sequence frames into
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
  
  void dsrlprsq_init  (dsrlpi_sess_ctl_blk_type     *rscb_ptr);
  
  void dsrlprsq_reset (dsrlpi_rlp_ctl_blk_type      *rrcb_ptr);

  void dsrlprsq_insert(dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,
                       dsrlpi_curr_fr_info_type     *frame_info_ptr,
                       dsm_item_type                *fr_ptr         
                      );

  void dsrlprsq_remove(dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,
                       uint32                        max_chain_cnt 
                      );

INITIALIZATION AND SEQUENCING REQUIREMENTS
  
  --RLP configuration and initialization procedures.
  
  --Resequencing queue initialization through call to dsrlprsq_init() for
    each separate rlp session.

 Copyright (c) 1999 - 2012 by Qualcomm Technologies Incorporated.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsrlprsq.c_v   1.24   31 Jan 2003 11:07:56   kdevara  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/1xrlp/src/dsrlprsq.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        -------------------------------------------------------
03/21/12   msh        TCB changes due to Core changes  
03/14/12   msh        Initialize the queues only once to avoid memory leak
01/04/12   ack        Fixed Klocwork errors
02/02/10   ack        Added Critsect to guard reseq Q insertion
01/18/11   op         Migrated to MSG 2.0 Macros
06/15/10   sn         Added interrupt lock when processing resequencing 
                      queue and unlock after processing it. This is to avoid 
                      SCR interrupt freeing resequencing queue as part of RLP 
                      reset.
09/22/09   sn         Fixed klocwork errors.
05/14/09   ls         Fix the lint errors
09/13/07   sn         Increased DSRLPRSQ_MAX_OVERLAP_ITEMS from 10 to 50.
05/10/07   ac         Lint fix
05/30/06   ksu        EMPA: RLP init code review comments
04/26/06   sy         Fixed multiple issues: 1. Macros DSRLPRSQ_ADD_32BIT_SEQ()
                      and DSRLPRSQ_GET_LAST_SEQ_NUM() now work for seq_modulus 
                      rollover scenarios.
                      2. Received data(dsm_chain) that needs to be inserted in
                      the resequencing queue is updated with sequence number 
                      appropriately in each dsm_item->app_field.
                      3.Incase of overlap with the resequencing queue, the data 
                      is pulled up from the received packet and the sequencing
                      number is calculated correctly.

01/03/06   ksu        EMPA: hook up forward link with new phys link architecture
12/23/05   ksu        EMPA RSP: two fwd rx queues
12/15/05   TMR        Changed RLP RX queue to watermark and added callback data
                      to RLP RX function callback.
11/15/05   ksu        EMPA: packet stream, octet sequencing (forward link also)
09/29/05   ksu/sy     MFPA: short sequence numbers work for flow with 
                      naks disabled
08/29/05   ksu/sy     Remove dsrlp_reg_hdlc() in favor of uint32 app_field
                      parameter to dsrlp_reg_srvc().
11/29/04   gr         Merged the PZID hyst changes into mainline
09/20/04   vas        QOS related changes
09/08/04   vr         Notify the PZID hysterisis engine if a PPP packet is
                      exchanged first time after dormancy
07/30/04   gr         Replaced a few ASSERTs in dsrlprsq_regular_insert
                      function with graceful handling of error condition
04/21/03   ik/js      Modified to work with the defeaturized queues. 
01/31/03   kvd        Modified 4 dsrlpseq macros DSRLPSEQ_GET_NEXT_SEQ,
                      DSRLPSEQ_INCREMENT_SEQ, DSRLPSEQ_INCREMENT_S_SEQ,
                      DSRLPSEQ_INCREMENT_32BIT_SEQ to include modulus as
                      an argument
10/15/02   js         Added RLP stream identifier to f3 messages in order to
                      distinguish different RLP streams. 
06/12/02   snn        Removed the fast insert function out of IRAM.
02/11/02   ss         Modified parameters of hdrfmac_enable_flow_control.
12/18/01   rsl        Updated tracer field.
12/09/01   vr         Check for HDR system before flow controlling FL
12/09/01   vr         Flow control HDR FL is PS PPP queue too large.
09/04/01   hcg        Added dsrlprsq_remove_chain() to pass chains of DSM
                      items to upper layer.  Also, added code to support
                      1xEV data path optimizations.
08/02/01   ak         Removed \n's from error statements.
07/31/01   igt        Add sw to prevent two types of fast insert ptrs to 
                      ever point to the same item to prevent ASSERT() fail. 
07/25/01   igt        Allow insert when LV(N) != LV(R) and the queue is empty
07/19/01   igt        Add sw to handle fast insert ptrs in rare append case.
07/02/01   ak         Remove call to dsrlprsq_remove under error conditions
06/19/01   snn        removed the variable walk_seq_num from regular_insert
                      function to remove compiler warnings.
06/06/01   igt        created dsrlprsq_fast_walk() to reside in IRAM.
06/05/01   igt        Arranged the sw into several functions to save IRAM
                      space while maintaining performance.
06/04/01   igt        Added fast insert pointers for the case we insert 
                      supplementals somewhere other than the end of the
                      resequencing queue.
05/24/01   hcg        Made changes to support HDR (1xEV).
05/11/01   ak         FEATURE cleanup.
02/22/01   ak         dsrlprsq_insert now in IRAM
01/04/01   igt        added fast insert when incoming seq num >= LV(r)
11/13/00   igt        fixed a couple compiler warnings
11/01/00   igt        incorporated sw review comments into the code
09/29/00   ak/igt     Fixed DIFF_SEQ comparison error when both operands are
                      equal.
06/08/00   ak         Added MSM5000_IRAM_FWD #ifndef's for IRAM support.
06/05/00   ak         Changed dsrlprsq_remove to use an rx_func_ptr callback
                      instead of the rx_watermark.
05/16/00   igt        Added append capability instead of insert when seq#s 
                      permit to append to a dsm item chain. Keeps q shorter.
04/10/00   ak         Added braces to a while() clause.
04/28/00   igt        Update the rx_total_bytes statistic
04/26/00   igt        Added code to set dup_ptr to NULL for dsm chains 
03/21/00   igt        Fixed a compiler warning
01/11/00   na         Fixed bug in dsrlprsq_insert, where 2 seq. numbers were
                      being compared without the macros.
01/05/00   na         Fixed dsrlprsq_remove(), so that the dup_ptr is set
                      to NULL.
11/09/99   igt        Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "data_msg.h"

#if defined (FEATURE_DS_RLP3)

#include "dsrlp_v.h"
#include "dsrlpi.h"
#include "dsrlprsq.h"
#include "amssassert.h"
#include "memory.h"
#include "err.h"
#include "queue.h"
#include "dsutil.h"
#include "rex.h"

/*===========================================================================

             PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*---------------------------------------------------------------------------
Number of overlap array entries. This array is used as an array of pointers
to items that are between "last" and "truelast" and are to be removed from Q
in case a big dsm item that replaces them is received.
---------------------------------------------------------------------------*/
#define DSRLPRSQ_MAX_OVERLAP_ITEMS   50

/*---------------------------------------------------------------------------
  These are used to set the MSB in the kind field of a dsm_item 
---------------------------------------------------------------------------*/
#define SET_LST_ON_MASK              (0x80)
#define SET_LST_OFF_MASK             (0x7f)

/*------------------------------------------------------------------------- 
  this holds pointers to items between "last" and "truelast" so they can
  easily be taken out of the resequencing queue in case of an overlap.           
-------------------------------------------------------------------------*/
typedef struct  
{
  uint32          overlap_count; /* # of dsm items overlapping with "new"*/
  dsm_item_type  *overlap_arr[DSRLPRSQ_MAX_OVERLAP_ITEMS];
} overlap_info_type;

/*------------------------------------------------------------------------- 
  this structure keeps information about the "first", "last" and 
  "truelast" items in the resequencing queue, as well as the "new_item".            
-------------------------------------------------------------------------*/
typedef struct                      /*keep info for overlap checks       */
{
  dsm_item_type   *ptr;             /*points to the dsm item chain       */
  uint32          seq_num;          /*first seq# of new dsm item chain   */
  uint32          last_seq_num;     /*last seq# for the dsm item chain   */
  uint32          last_plus_seq_num;/*last seq num + 1 for the new item  */
  uint32          length;           /*the length of the frame            */
  uint32          overlap_bytes;    /*# bytes to be chopped from "new"   */
  boolean         discard;          /*TRUE if needs to be  discarded     */
  boolean         replace;          /*TRUE is needs to be  replaced      */
  boolean         lst_set;          /*hold LST information for the item  */
  boolean         found;            /*keep track if "first" was found    */
} dsrlpi_rsq_item_info_type;

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                        MACRO DEFINITIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/

/*===========================================================================

MACRO          DSRLPRSQ_GET_NEXT_ENTRY

DESCRIPTION    Gets the next entry from the reseq Q, given the current
               entry.  This is here so the code in the function is
               readable.
               
               2 params:   q_ptr     = ptr to queue
                           entry_ptr = ptr to current entry
               
DEPENDENCIES   NONE

RETURN VALUE   pointer to  a dsm item chain

SIDE EFFECTS   NONE
===========================================================================*/
#define DSRLPRSQ_GET_NEXT_ENTRY(q_ptr,entry_ptr) \
     ((dsm_item_type *)q_next(q_ptr,&(entry_ptr->link)))


/*===========================================================================

MACRO          DSRLPRSQ_GET_SEQ_NUM

DESCRIPTION    Get the first sequence # for this dsm item chain
               
               1 params:   walk_item_ptr     = ptr to dsm item
                        
               
DEPENDENCIES   NONE

RETURN VALUE   The first sequence # for the given dsm item chain

SIDE EFFECTS   NONE
===========================================================================*/
#define DSRLPRSQ_GET_SEQ_NUM(walk_item_ptr)\
        ((uint32)walk_item_ptr->app_field)


/*===========================================================================

MACRO          DSRLPRSQ_GET_LST_STATUS

DESCRIPTION    Get the status of the LST bit for this specific dsm item chain
               
               1 params:   fr_ptr     = ptr to dsm item
                        
               
DEPENDENCIES   NONE

RETURN VALUE   True or False depending on the LST bit status

SIDE EFFECTS   NONE
===========================================================================*/
#define DSRLPRSQ_GET_LST_STATUS(fr_ptr)\
 ((fr_ptr->kind & SET_LST_ON_MASK ) ? (TRUE) : (FALSE))



/*===========================================================================

MACRO          DSRLPRSQ_Q_DELETE

DESCRIPTION    Take a DSM item chain out of the Resequencing Q. Also make 
               sure you set the app_field to 0 to not cause any 
               problems later.
               
               2 params:   reseq_q_ptr     = ptr to Resequencing Q
                           item_ptr        = link
                           
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
#ifdef FEATURE_Q_NO_SELF_QPTR
#define  DSRLPRSQ_Q_DELETE(reseq_q_ptr, item_ptr)\
{\
  item_ptr->app_field = 0;\
  q_delete(reseq_q_ptr, &(item_ptr->link));\
}
#else
#define  DSRLPRSQ_Q_DELETE(reseq_q_ptr, item_ptr)\
{\
  item_ptr->app_field = 0;\
  q_delete(&(item_ptr->link));\
}
#endif


/*===========================================================================

MACRO          DSRLPRSQ_Q_INSERT

DESCRIPTION    Insert to the Q according to the FEATURE_Q               

PARAMETERS     reseq_q_ptr: pointer to the queue  
               new_link:    link of the new item to insert
               old_link:    link of the item already in the q
                                           
DEPENDENCIES   None

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
#ifdef FEATURE_Q_NO_SELF_QPTR
#define DSRLPRSQ_Q_INSERT(reseq_q_ptr, new_link, old_link)\
          q_insert(reseq_q_ptr, new_link, old_link);
#else
#define DSRLPRSQ_Q_INSERT(reseq_q_ptr, new_link, old_link)\
          q_insert( new_link, old_link);
#endif


    

/*===========================================================================

MACRO          DSRLPRSQ_DISCARD_NEW_FRAME

DESCRIPTION    Checks to see if the old_item includes all of the new_item               

PARAMETERS     the 2 items to compare and the current sequence modulus
                                           
DEPENDENCIES   The "new" item has to be in the first parameter

RETURN VALUE   TRUE or FALSE

SIDE EFFECTS   None.

===========================================================================*/
#define DSRLPRSQ_DISCARD_NEW_FRAME(dnew, dold, seq_mod)\
( ((DSRLPSEQ_GE_SEQ_CHECK(dnew.seq_num,      dold.seq_num,     seq_mod)) &&\
   (DSRLPSEQ_GE_SEQ_CHECK(dold.last_seq_num, dnew.last_seq_num,seq_mod))) ?\
TRUE : FALSE)


/*===========================================================================

MACRO          DSRLPRSQ_REPLACE_OLD_FRAME

DESCRIPTION    Checks to see if the new_item includes all of the old_item               

PARAMETERS     the 2 items to compare 
                                           
DEPENDENCIES   The "new" item info has to be in the first parameter

RETURN VALUE   TRUE or FALSE

SIDE EFFECTS   None.

===========================================================================*/
#define DSRLPRSQ_REPLACE_OLD_FRAME(dnew, dold, seq_mod)\
( ((DSRLPSEQ_GE_SEQ_CHECK(dold.seq_num,      dnew.seq_num,      seq_mod)) &&\
   (DSRLPSEQ_GE_SEQ_CHECK(dnew.last_seq_num, dold.last_seq_num, seq_mod))) ?\
TRUE : FALSE)



/*===========================================================================

MACRO          DSRLPRSQ_ADD_SEQ_NUM

DESCRIPTION    Adds the second parameter to the first seq number               

PARAMETERS     The first sequence #, the # to add, the sequence modulus
                                           
DEPENDENCIES   None

RETURN VALUE   The new value of the first sequence number goes into the
               first parameter.

SIDE EFFECTS   None.

===========================================================================*/
#define DSRLPRSQ_ADD_32BIT_SEQ(fseq, bytes, seq_mod)\
{\
  uint32 temp;\
  ASSERT((bytes <= ((seq_mod >> 4)-1)) && (fseq < seq_mod));\
  temp = ((uint32)(fseq) + ((uint32)bytes <<4));\
  if (temp < seq_mod)\
  {\
    fseq = temp;\
  }\
  else\
  {\
    fseq = (temp - seq_mod);\
  }\
}



/*===========================================================================

MACRO    DSRLPRSQ_GET_LAST_SEQ_NUM

DESCRIPTION
  This macro will calculate the last sequence # of the range for the rlp 
  frame in hand. It will do this by adding the length of the frame to the
  very first sequence #  of that frame. It returns the calculated number. If
  the LST bit is set to 1 this is the last segmented frame and thus you need
  to calculate 1 past the range of sequence numbers used for the segment. 
  
PARAMETERS
  fseq:          The first sequence number of the range for the frame.
  lseq:          To be SET to the last sequence number of the range.
  fr_len:        This is the length of the frame
  seq_mod:       The modulus of the rlp version
  
DEPENDENCIES
  None

RETURN VALUE
  The calculated 32-bit last seq # for the frame is returned in l_seq

SIDE EFFECTS
  None
===========================================================================*/
#define DSRLPRSQ_GET_LAST_SEQ_NUM(fseq, lseq, fr_len, seq_mod)\
{\
  uint32 temp;\
  ASSERT((fr_len <= ((seq_mod >> 4)-1)) && (fseq < seq_mod));\
  temp =((uint32)(fseq) + ((uint32)(fr_len - 1) <<4));\
  if (temp < seq_mod)\
  {\
    lseq = temp;\
  }\
  else\
  {\
    lseq = (temp - seq_mod);\
  }\
}



/*===========================================================================

MACRO    DSRLPRSQ_GET_LAST_PLUS_SEQ_NUM

DESCRIPTION
  This function will calculate the last sequence # + 1 of the range for the rlp 
  frame in hand. It will do this by adding the length of the frame to the
  very first sequence #  of that frame. It returns the calculated number. If
  the LST bit is set to 1 this is the last segmented frame and thus you need
  to calculate 1 past the range of sequence numbers used for the segment. 
  
PARAMETERS
  rscb_ptr:      current RLP session ctl blk
  rsq_item_ptr:  RSQ item to operate upon
  
DEPENDENCIES
  None

RETURN VALUE
  The calculated 32-bit last seq # + 1  is returned in lseq

SIDE EFFECTS
  None
===========================================================================*/
static void dsrlprsq_get_last_plus_seq_num
(
  dsrlpi_rlp_ctl_blk_type   *rrcb_ptr,      /* current RLP ctl blk         */
  dsrlpi_rsq_item_info_type *rsq_item_ptr   /* RSQ item to operate upon    */   
)
{
  DSRLPRSQ_GET_LAST_SEQ_NUM(rsq_item_ptr->seq_num, 
                            rsq_item_ptr->last_plus_seq_num,
                            rsq_item_ptr->length, 
                            rrcb_ptr->cfg_ptr->seq_modulus);

    if (rsq_item_ptr->lst_set)
    {
      rsq_item_ptr->last_plus_seq_num = DSRLPSEQ_GET_NEXT_SEQ(rsq_item_ptr->last_plus_seq_num,
                                                            rrcb_ptr->cfg_ptr->seq_modulus);
    }
    else
    {
      rsq_item_ptr->last_plus_seq_num = DSRLPSEQ_INCREMENT_32BIT_SEQ(rsq_item_ptr->last_plus_seq_num,
                                                                   rrcb_ptr->cfg_ptr->seq_modulus);
    }
}



/*===========================================================================

MACRO    DSRLPRSQ_GET_LAST_CHAIN_ITEM

DESCRIPTION
  This will traverse a dsm item chain and set the first parameter
  to point to the last dsm item in that chain.   
  
PARAMETERS
  fr_ptr:        pointer to the first dsm item in the chain 
  last_item_ptr: pointer to the last item in the chain    
    
DEPENDENCIES
  Assumes the fr_ptr is not NULL

RETURN VALUE
  Sets the value of the first parameter last_item_ptr

SIDE EFFECTS
  None
===========================================================================*/
#define DSRLPRSQ_GET_LAST_CHAIN_ITEM(last_item_ptr, fr_ptr)\
{\
  ASSERT(fr_ptr != NULL);\
  last_item_ptr = fr_ptr;\
  while(last_item_ptr->pkt_ptr != NULL)\
  {\
    last_item_ptr = last_item_ptr->pkt_ptr;\
  }\
}




/*===========================================================================

MACRO    DSRLPRSQ_REESTABLISH_FAST_PTRS

DESCRIPTION
  This will re-establish pointers to the last dsm item chain in the 
  resequencing queue as well as a pointer to the last item of the
  last chain. It is used in cases the value of these pointers are
  forced to NULL when the items they are pointing to have to be 
  removed from the resequencing queue.     
  
PARAMETERS
  rscb_ptr:    pointer to the session control block 
  reseq_q_ptr: pointer to the resequencing queue    
    
DEPENDENCIES
  Assumes the parameters are not NULL

RETURN VALUE
  None

SIDE EFFECTS
  Sets the values of the reseq_last_item_ptr  and reseq_last_chain_ptr
  fields of the state structure in the session control block
===========================================================================*/
#define DSRLPRSQ_REESTABLISH_FAST_PTRS(rrcb_ptr, reseq_q_ptr)\
{\
  rrcb_ptr->state.reseq_last_chain_ptr =\
    (dsm_item_type *)q_last_check( reseq_q_ptr );\
  DSRLPRSQ_GET_LAST_CHAIN_ITEM(rrcb_ptr->state.reseq_last_item_ptr,\
                               rrcb_ptr->state.reseq_last_chain_ptr);\
}

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                     INTERNAL FUNCTION DEFINITIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/


/*===========================================================================

FUNCTION       DSRLPRSQ_INIT

DESCRIPTION    Called at initialization of the resequencing queue. 
           
PARAMETERS     rscb_ptr: helps distinguish among multiple RLP sessions
               
DEPENDENCIES   Needs to be called before using the resequencing queue
               The rscb_ptr passed in should be non-NULL

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void dsrlprsq_init
( 
  dsrlpi_sess_ctl_blk_type *rscb_ptr        /* current RLP session ctl blk */)
{
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr;       /* current RLP ctl blk         */
  int p;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  ASSERT( rscb_ptr != NULL);
  
  for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
  {
    rrcb_ptr = &rscb_ptr->rlp_list[p];

  /*-------------------------------------------------------------------------
    initialize the boolean to show no data in the resequencing queue 
    -------------------------------------------------------------------------*/
    rrcb_ptr->state.reseq_q_more_data = FALSE;

    rrcb_ptr->state.reseq_last_chain_ptr = NULL;
    rrcb_ptr->state.reseq_last_item_ptr  = NULL;

    rrcb_ptr->state.reseq_last_insert_chain_ptr = NULL;
    rrcb_ptr->state.reseq_last_insert_item_ptr  = NULL;
  } 
}

/*===========================================================================

FUNCTION       DSRLPRSQ_RESET

DESCRIPTION    Called when an RLP instance is reset.  This takes any Reseq Q
               entries up until LV(N), and puts  them  into the rx q. From 
               that point on it sets free any other remaining dsm item 
               chains. It is not subject to max chain count limitations as
               the dsrlprsq_remove() may be.

PARAMETERS     rscb_ptr: pointer to session control block containing RSQ
               
DEPENDENCIES   rscb_ptr != NULL

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void dsrlprsq_reset
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr         /* current RLP ctl blk        */
)
{
  q_type                      *reseq_q_ptr;      /*   ptr to rlp's reseq q */
  dsm_item_type               *reseq_entry_ptr;  /*   reseq  list entry    */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  ASSERT(rrcb_ptr != NULL);

  reseq_q_ptr = &(rrcb_ptr->state.reseq_q);

  /*-------------------------------------------------------------------------
    First call the dsrlprsq_remove() to make sure you pass up what remains.
    The second parameter is ZERO as this call is not subject to max # of dsm
    item chains that can be dsm_enqueue() at one time.
  -------------------------------------------------------------------------*/
  dsrlprsq_remove(rrcb_ptr, 0);

  /*-------------------------------------------------------------------------
    now you can free up the rest of the contents of the resequencing Q.
    Make sure the app_field is set to 0, before calling dsm_free_packet.
  -------------------------------------------------------------------------*/
  while ((reseq_entry_ptr = (dsm_item_type *)q_get(reseq_q_ptr)) != NULL)
  {
    reseq_entry_ptr->app_field = 0;
    dsm_free_packet( &reseq_entry_ptr);
  }

  rrcb_ptr->state.reseq_last_chain_ptr = NULL;
  rrcb_ptr->state.reseq_last_item_ptr  = NULL;
  
  rrcb_ptr->state.reseq_last_insert_chain_ptr = NULL;
  rrcb_ptr->state.reseq_last_insert_item_ptr  = NULL;

  rrcb_ptr->state.reseq_q_more_data = FALSE;

}


/*===========================================================================

FUNCTION       DSRLPRSQ_FAST_INSERT

DESCRIPTION    This function performs the optimized INSERT operation for the 
               Resequencing queue of the RLP software. It takes care of 3 
               fast insert cases. When the queue is empty and this is the 
               very first item to be inserted, when the item belongs to the 
               very end of the re-sequencing queue, and finnaly when the new
               item belongs right next to the one we inserted last somewhere
               in the iddle of the re-sequencing queue. 
               
               It inserts a dsm item representing an RLP frame into the 
               Resequencing queue. Every time there is a hole in the data, 
               the frames end up in this queue. Each item is inserted in
               order based on its sequence number. 
               
               
PARAMETERS     rscb_ptr:       pointer to session control block
               frame_info_ptr: pointer to frame information type
               fr_ptr:         pointer to dsm item to insert           
  

DEPENDENCIES   -- Inputs rscb_ptr, frame_info_ptr, fr_ptr  are  non-null.
               -- Need calculated full 32 bit seq # in frame_info_ptr.
               -- Uses "app_field" of incoming dsm item *fr_ptr to store
                  the sequence number
               -- Uses "kind" field of incoming dsm item *fr_ptr to store
                  the LST state

RETURN VALUE   NONE

SIDE EFFECTS   Sets frame_info_ptr to NULL upon exit
===========================================================================*/
static boolean dsrlprsq_fast_insert
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,      /* curr RLP ctl blk         */
  dsrlpi_curr_fr_info_type     *frame_info_ptr,/* current frame info       */
  dsm_item_type                *fr_ptr         /* the frame to insert      */
)
{
  
  /*------------------------------------------------------------------------- 
    keep information about the "first", "last" and "truelast" items in the
    resequencing queue, as well as the "new_item".            
  -------------------------------------------------------------------------*/
  dsrlpi_rsq_item_info_type   new_item;     /* keep info about "new" item  */ 
  dsrlpi_rsq_item_info_type   fast_item;    /* keep info about "fast" item */

  q_type         *reseq_q_ptr       = NULL;/* convenience ptr to reseq Q   */
  
  dsm_item_type  *walk_item_ptr     = NULL;/* auxiliary pointer for Q ops  */ 
  dsm_item_type  *last_item_ptr     = NULL;/* auxiliary pointer for Q ops  */

  uint32          rsq_item_count    = 0;   /* # dsm item chains in Reseq Q */
  uint32          seq_mod           = 0;   /* the curent sequence modulus  */
  uint32          needed_seq_num    = 0;   /* keeps LV(N) for fast insert  */
  uint32          expected_seq_num  = 0;   /* keeps LV(R) for fast insert  */
  boolean         result = FALSE; 
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  memset( &new_item,      0, sizeof(dsrlpi_rsq_item_info_type));
  memset( &fast_item,     0, sizeof(dsrlpi_rsq_item_info_type));
    
  /*-------------------------------------------------------------------------
    get some fields for fast access to the resequencing queue throughout
  -------------------------------------------------------------------------*/
  reseq_q_ptr     = &(rrcb_ptr->state.reseq_q);/* pointer to the reseq Q   */
  seq_mod         =  rrcb_ptr->cfg_ptr->seq_modulus ;

  /*-------------------------------------------------------------------------
    In the rare case both types of fast insert ptrs are pointing to the same
    item, set the "middle" fast ptrs to NULL as they do not cotribute and the 
    sw assumes they are not ever the same as the pointers to the end of Q.
  -------------------------------------------------------------------------*/
  if ((rrcb_ptr->state.reseq_last_chain_ptr     == 
       rrcb_ptr->state.reseq_last_insert_chain_ptr ) &&
      (rrcb_ptr->state.reseq_last_chain_ptr != NULL)
     ) 
  {
    DATA_RLP_MSG1(MSG_LEGACY_HIGH, "Fast pointers pointing to same item found. RLP: %d", 
             rrcb_ptr->cfg_ptr->sr_id);
    rrcb_ptr->state.reseq_last_insert_chain_ptr = NULL;
    rrcb_ptr->state.reseq_last_insert_item_ptr  = NULL;
  }

  /*-------------------------------------------------------------------------
    Make sure if the q is not empty the "last_chain_ptr always points to the
    last dsm item chain of the re-sequencing queue.  
  -------------------------------------------------------------------------*/
  if ( (walk_item_ptr = q_check(reseq_q_ptr)) != NULL) 
  {
     if ( rrcb_ptr->state.reseq_last_chain_ptr != 
          (dsm_item_type *)q_last_check( reseq_q_ptr ))
     {
       DSRLPRSQ_REESTABLISH_FAST_PTRS(rrcb_ptr, reseq_q_ptr);
       DATA_RLP_MSG0(MSG_LEGACY_ERROR,
                     "Fast pointers found not pointing to last element.");
     }
  }
 
  /*-------------------------------------------------------------------------
    read the LV(N) into needed_seq_num. Prepare all the information fields
    in the dsrlpi_rsq_item_info structure for the "new item" 
  -------------------------------------------------------------------------*/
  needed_seq_num  = rrcb_ptr->state.needed_seq_num;

  /*-------------------------------------------------------------------------
    read the LV(R), it will help with optimized fast insert
  -------------------------------------------------------------------------*/
  expected_seq_num= rrcb_ptr->state.expected_seq_num;

  new_item.ptr    = fr_ptr;
  new_item.length = dsm_length_packet(fr_ptr); /* length of frame to insert*/

  /*-------------------------------------------------------------------------
    get the first sequence # and store in the dsm item chain. In this 
    function the "app_field" is being used to store the sequence number 
  -------------------------------------------------------------------------*/
  new_item.seq_num= fr_ptr->app_field;   

  /*-------------------------------------------------------------------------
    get LST , store in the MSB of "kind" field in dsm  item     
  -------------------------------------------------------------------------*/
  if (frame_info_ptr->fr_cat & CURR_FRAME_LST_BIT_MASK ) 
  {
      new_item.lst_set   = TRUE;
      fr_ptr->kind      |= SET_LST_ON_MASK;
  } 
  else  
  {
      new_item.lst_set   = FALSE;
      fr_ptr->kind      &= SET_LST_OFF_MASK; 
  }
   
  /*-------------------------------------------------------------------------
    get value for "new_item.last_seq_num"      
  -------------------------------------------------------------------------*/
  DSRLPRSQ_GET_LAST_SEQ_NUM(new_item.seq_num,
                            new_item.last_seq_num,
                            new_item.length,
                            seq_mod); 

  /*-------------------------------------------------------------------------
    get value for "new_item.last_plus_seq_num"       
  -------------------------------------------------------------------------*/
  dsrlprsq_get_last_plus_seq_num(rrcb_ptr,
                                 &new_item); 
 

  /*-------------------------------------------------------------------------
    check the location of the new frame in relation to the LV(N) and see if
    it needs to be chopped or ignored. Recalculate any numbers if needed.
  -------------------------------------------------------------------------*/
  if (DSRLPSEQ_GT_SEQ_CHECK(needed_seq_num, new_item.last_seq_num, seq_mod))
  {
    /*-----------------------------------------------------------------------
      (LV(N) > last sequence number) so this new frame does not contribute.
      So free the dsm item and return. Make sure to set the app_field to 0.
    -----------------------------------------------------------------------*/
    DATA_RLP_MSG3(MSG_LEGACY_ERROR, "LV(N)= 0x%x > than last "
                  "seq.= 0x%x, seq_mod= 0x%x. Frame discarded ", 
                  needed_seq_num, new_item.last_seq_num, seq_mod);

    fr_ptr->app_field = 0;
    dsm_free_packet(&fr_ptr);
    
    /*-----------------------------------------------------------------------
      Return TRUE as we do not need to run the dsrlprsq_regular_insert()  
    -----------------------------------------------------------------------*/
    return(TRUE);
  } 
  else if (DSRLPSEQ_GT_SEQ_CHECK(needed_seq_num, new_item.seq_num, seq_mod)) 
  {
    /*-----------------------------------------------------------------------
      (LV(N) > first sequence number) so we need to chop part of the frame 
      and also recalculate the first seq number and the length.
      This is best left to be handled by the dsrlprsq_regular_insert().
      Return FALSE so that dsrlprsq_regular_insert() will run after this.
    -----------------------------------------------------------------------*/
    return(FALSE);
  }
  
  /*-------------------------------------------------------------------------
    now go ahead and insert, making sure there will be no duplication
  -------------------------------------------------------------------------*/
  rsq_item_count = q_cnt( reseq_q_ptr);
  if ( rsq_item_count == 0  ) 
  {
    
    /*-----------------------------------------------------------------------
      at this point these pointers should be NULL as the q is empty
    -----------------------------------------------------------------------*/
    ASSERT((rrcb_ptr->state.reseq_last_item_ptr == NULL) &&
           (rrcb_ptr->state.reseq_last_chain_ptr== NULL) &&
           (rrcb_ptr->state.reseq_last_insert_item_ptr == NULL) &&
           (rrcb_ptr->state.reseq_last_insert_chain_ptr== NULL));

    /*----------------------------------------------------------------------- 
      In this case we know the new chain is going to be the last chain.
      This will update all the related pointers.
    -----------------------------------------------------------------------*/
    DSRLPRSQ_GET_LAST_CHAIN_ITEM(last_item_ptr, new_item.ptr);
    rrcb_ptr->state.reseq_last_item_ptr  = last_item_ptr;
    rrcb_ptr->state.reseq_last_chain_ptr = new_item.ptr;
    

    /*----------------------------------------------------------------------- 
      No items in the q at this time, just insert as a first item. 
      The  fr_ptr->link has been initialized and is valid at this point.
    -----------------------------------------------------------------------*/
    q_put( reseq_q_ptr, &(new_item.ptr->link) );

    /*-----------------------------------------------------------------------
      Now your job is done just get out of here !!
    -----------------------------------------------------------------------*/
    frame_info_ptr = NULL;

    result = TRUE;
  }
  else
  {
    /*-------------------------------------------------------------------------
      In this case the resequencing Q is not empty. We need to insert at the
      proper location and also make sure there is no duplication.
    -------------------------------------------------------------------------*/
     
    /*-------------------------------------------------------------------------
      Make sure the pointers are not NULL as the q is not empty
    -------------------------------------------------------------------------*/
    ASSERT( ( rrcb_ptr->state.reseq_last_chain_ptr != NULL) &&
           ( rrcb_ptr->state.reseq_last_item_ptr  != NULL) &&
           ( (rrcb_ptr->state.reseq_last_item_ptr)->pkt_ptr == NULL) );
 
    /*-------------------------------------------------------------------------
      See if we can do  fast insert to the end of the resequencing queue
    -------------------------------------------------------------------------*/
    if (DSRLPSEQ_GE_SEQ_CHECK(new_item.seq_num, expected_seq_num, seq_mod)) 
    {
    /*-------------------------------------------------------------------------
      get field values of the last item in the resequencing queue
    -------------------------------------------------------------------------*/
    fast_item.ptr     = rrcb_ptr->state.reseq_last_chain_ptr; 
    fast_item.seq_num = fast_item.ptr->app_field;
    fast_item.lst_set = DSRLPRSQ_GET_LST_STATUS(fast_item.ptr);
    fast_item.length  = dsm_length_packet(fast_item.ptr);
    
    dsrlprsq_get_last_plus_seq_num( rrcb_ptr,
                                    &fast_item
                                  );

    DSRLPRSQ_GET_LAST_SEQ_NUM( fast_item.seq_num, 
                               fast_item.last_seq_num,
                               fast_item.length,
                               seq_mod
                             );
  
    /*-------------------------------------------------------------------------
      This should also be true. It may be replaced with an ASSERT as it should
      always be true. If it is not TRUE and we insert the fast way we are not
      avoiding duplicates.
    -------------------------------------------------------------------------*/
    if(DSRLPSEQ_GT_SEQ_CHECK(expected_seq_num, fast_item.last_seq_num,seq_mod))
    {
    
      /*-----------------------------------------------------------------------
        In this case there is no risk of duplication as the new item is equal
        or beyond the LV(R) and the LV(R) is beyond the last seq number of the
        last item in the queue.     
      -----------------------------------------------------------------------*/
      DSRLPRSQ_GET_LAST_CHAIN_ITEM(last_item_ptr, new_item.ptr);

    
      if (( new_item.seq_num            == expected_seq_num) &&
          (!fast_item.lst_set)                               &&
          ( fast_item.last_plus_seq_num == new_item.seq_num)
         ) 
      {
        /*-------------------------------------------------------------------
          This is the fast APPEND
        -------------------------------------------------------------------*/
        (rrcb_ptr->state.reseq_last_item_ptr)->pkt_ptr = new_item.ptr;
        rrcb_ptr->state.reseq_last_item_ptr            = last_item_ptr;

        if (new_item.lst_set) 
        {
          fast_item.lst_set = TRUE;
          fast_item.ptr->kind |= SET_LST_ON_MASK;
        }
        
      }
      else
      {
        /*-------------------------------------------------------------------
          Update fast insert pointers and insert at the tail of the queue
        -------------------------------------------------------------------*/
        rrcb_ptr->state.reseq_last_item_ptr     = last_item_ptr;
        rrcb_ptr->state.reseq_last_chain_ptr    = new_item.ptr;


        q_put(reseq_q_ptr, &(new_item.ptr->link));
      }

      /*-----------------------------------------------------------------------
        Now your job is done just get out of here !!
      -----------------------------------------------------------------------*/
      frame_info_ptr = NULL;

      result = TRUE;  
    }/* if expected_seq_num >  fast_item.last_seq_num */
    else
    { 
      /*-----------------------------------------------------------------------
        Though unlikely, if you are here there is no more hope so just get out
      -----------------------------------------------------------------------*/
      result = FALSE;
    }
  
    }/* if new_item.seq_num >= expected_seq_num */
    else if (rrcb_ptr->state.reseq_last_insert_chain_ptr != NULL) 
    {
      /*-------------------------------------------------------------------------
        If we are here then the fast insert to the end of the resequencing queue
        was not appropriate. Try to see if we should insert right after the last
        inserted item somewhere in the middle of the q using the last_insert_ptr 
      -------------------------------------------------------------------------*/
    
      /*-----------------------------------------------------------------------
        get field values of the most recently inserted item. This excludes any
	    item inserted to the very end of the queue.
      -----------------------------------------------------------------------*/
      fast_item.ptr     = rrcb_ptr->state.reseq_last_insert_chain_ptr; 
      fast_item.seq_num = fast_item.ptr->app_field;
      fast_item.lst_set = DSRLPRSQ_GET_LST_STATUS(fast_item.ptr);
      fast_item.length  = dsm_length_packet(fast_item.ptr);

      dsrlprsq_get_last_plus_seq_num( rrcb_ptr,
                                      &fast_item
                                    );

      DSRLPRSQ_GET_LAST_SEQ_NUM( fast_item.seq_num, 
                                 fast_item.last_seq_num,
                                 fast_item.length,
                                 seq_mod
                               );

      /*-------------------------------------------------------------------------
        Try to get a pointer to the next chain in q if any. 
      -------------------------------------------------------------------------*/
      walk_item_ptr = DSRLPRSQ_GET_NEXT_ENTRY(reseq_q_ptr, fast_item.ptr);

      /*-------------------------------------------------------------------------
        If the q non-empty and we are here there must be at least 2 chains.   
      -------------------------------------------------------------------------*/
      ASSERT( walk_item_ptr != NULL);

      /*-------------------------------------------------------------------------
        This should also be true, to avoid risk of duplication in data. 
      -------------------------------------------------------------------------*/
      if(DSRLPSEQ_GT_SEQ_CHECK(new_item.seq_num,fast_item.last_seq_num,seq_mod) 
         &&
         DSRLPSEQ_GT_SEQ_CHECK(
           DSRLPRSQ_GET_SEQ_NUM(walk_item_ptr), new_item.last_seq_num, seq_mod)
        )
      {
        /*-----------------------------------------------------------------------
          In this case there is no risk of duplication.     
        -----------------------------------------------------------------------*/
        DSRLPRSQ_GET_LAST_CHAIN_ITEM(last_item_ptr, new_item.ptr);

        if ( (!fast_item.lst_set)                               &&
             ( fast_item.last_plus_seq_num == new_item.seq_num)
           ) 
        {
        /*-------------------------------------------------------------------
          This is the fast APPEND
        -------------------------------------------------------------------*/
          (rrcb_ptr->state.reseq_last_insert_item_ptr)->pkt_ptr = new_item.ptr;
           rrcb_ptr->state.reseq_last_insert_item_ptr           = last_item_ptr;

          if (new_item.lst_set) 
          {
            fast_item.lst_set = TRUE;
            fast_item.ptr->kind |= SET_LST_ON_MASK;
          }
        
        }
        else 
        {
          /*-------------------------------------------------------------------
            This is the fast INSERT. Update fast insert pointers and insert at
            the appropriate location
          -------------------------------------------------------------------*/
          rrcb_ptr->state.reseq_last_insert_item_ptr  = last_item_ptr;
          rrcb_ptr->state.reseq_last_insert_chain_ptr = new_item.ptr;

          /*-------------------------------------------------------------------
            insert in the Q right before the chain "walk_item_ptr" points to
          -------------------------------------------------------------------*/
          DSRLPRSQ_Q_INSERT(reseq_q_ptr, 
                             &(new_item.ptr->link), &(walk_item_ptr->link));
        }

        /*-----------------------------------------------------------------------
          Now your job is successfuly done, time to get out of here !
        -----------------------------------------------------------------------*/
        frame_info_ptr = NULL;

        result = TRUE;  
      }/* if */
      else
      { 
        /*-----------------------------------------------------------------------
          None of the optimized inserts can be used at this time.
        -----------------------------------------------------------------------*/
        result = FALSE;
      }
 
    }/* if{} else {}  */
    else
    {
      /*-----------------------------------------------------------------------
        None of the optimized inserts can be used at this time.
      -----------------------------------------------------------------------*/
      result = FALSE;
    }
  }
  
  return result;  
}/* dsrlprsq_fast_insert() */

#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION       DSRLPRSQ_FAST_WALK

DESCRIPTION    This function finds the appropriate location in the 
               re-sequencing queue to insert a new frame.                
                              
              
PARAMETERS     rscb_ptr:       pointer to session control block
               new_item_ptr:   pointer to the "new_item"       
               first_item_ptr: pointer to the "new_item"
               last_item_ptr:  pointer to the "new_item"
               
DEPENDENCIES   -- Inputs rscb_ptr  non-null.
               
RETURN VALUE   pointer to a dsm item chain in re-sequencing queue

SIDE EFFECTS   NONE   
===========================================================================*/
static dsm_item_type *dsrlprsq_fast_walk(
  dsrlpi_rlp_ctl_blk_type    *rrcb_ptr,      /* curr RLP ctl blk            */
  dsrlpi_rsq_item_info_type  *new_item_ptr,  /* keep info about "new" item  */
  dsrlpi_rsq_item_info_type  *first_item_ptr,/* keep info about "new" item  */
  dsrlpi_rsq_item_info_type  *last_item_ptr  /* keep info about "new" item  */
)
{
  
  uint32          seq_mod           = 0;   /* the curent sequence modulus  */
  uint32          walk_seq_num      = 0;   /* seq# of the walk ptr item    */
  q_type         *reseq_q_ptr       = NULL;/* convenience ptr to reseq Q   */
  dsm_item_type  *walk_item_ptr     = NULL;/* this holds the return value  */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /*-------------------------------------------------------------------------
    get some fields for fast access to the resequencing queue throughout
  -------------------------------------------------------------------------*/
  reseq_q_ptr     = &(rrcb_ptr->state.reseq_q);/* pointer to the reseq Q   */
  seq_mod         =  rrcb_ptr->cfg_ptr->seq_modulus;

  /*-------------------------------------------------------------------------
    See if you can start from the point the middle insert pointers were
    pointing to. Depends on why the optimized inserts failed
    -- because the seq number of the new item is before that point.
    -- because there is a data overlap
    -- because the seq number of the new is after that point. 
  -------------------------------------------------------------------------*/
  if ((rrcb_ptr->state.reseq_last_insert_chain_ptr != NULL) &&
      (DSRLPSEQ_GT_SEQ_CHECK(
          new_item_ptr->seq_num, 
		  DSRLPRSQ_GET_SEQ_NUM(rrcb_ptr->state.reseq_last_insert_chain_ptr), 
		  seq_mod)
      )
     )
  {
    walk_item_ptr = rrcb_ptr->state.reseq_last_insert_chain_ptr;   
  }
  else
  {
    /*-------------------------------------------------------------------------
      Get a pointer to the first dsm item chain in the Q    
    -------------------------------------------------------------------------*/
    walk_item_ptr = (dsm_item_type *)q_check(reseq_q_ptr);
  }
          
  /*-------------------------------------------------------------------------
    Should not have NULL value as we now know there is at least 1 Q entry
  -------------------------------------------------------------------------*/
  ASSERT(walk_item_ptr != NULL);


  /*-------------------------------------------------------------------------
    Walk through the reseq Q and find the two elements between which you 
    want to insert the new dsm item chain. Lets call them "first" and
    "last". You may not be able to find 2 if there is only one in the Q. 
    In that case the "first" or the "last" may be NULL. Same if we need to
    insert before the first element or after the last element in the Q.
    The "first" and "last" are calculated based on the first sequence #.
    We also find the "truelast" which is the first item that has a last 
    seq # that is greater than that of the new item we want to insert.
    The requirement for the "first" is first->first_seq# < new->first_seq#
    Requirement for "truelast" is   truelast->last_seq#  >= new->last_seq#
  -------------------------------------------------------------------------*/
  do 
  {

    /*-----------------------------------------------------------------------
      the "last" pointer becomes equal to THIS one no matter what, unless we 
      need to insert after the end of the Q, then it should be NULL and the 
      "first" pointer should point to the last element of the Q.
    -----------------------------------------------------------------------*/
    last_item_ptr->ptr = walk_item_ptr;
        
    /*-----------------------------------------------------------------------
          get the sequence # for this dsm item chain
    -----------------------------------------------------------------------*/
    walk_seq_num  = DSRLPRSQ_GET_SEQ_NUM(walk_item_ptr);
    
    /*-----------------------------------------------------------------------
      see if this is the one we should insert AFTER  
    -----------------------------------------------------------------------*/
    if( DSRLPSEQ_GT_SEQ_CHECK( new_item_ptr->seq_num, walk_seq_num, seq_mod))
    {   /* we need to WALK some more */
                
        /*-------------------------------------------------------------------
          this may become the "first" when we stop walking
        -------------------------------------------------------------------*/
        first_item_ptr->ptr = walk_item_ptr; 
        continue;
        
    } 
    else 
    {                          /* we need to insert BEFORE this item chain */
        
    /*-----------------------------------------------------------------------
      note that here we may have  new_seq_num <= walk_seq_num 
    -----------------------------------------------------------------------*/

        last_item_ptr->found    = TRUE;
                 
        /*--------------------------------------------------------------
          if there is a "first" then inform "found" field 
        --------------------------------------------------------------*/
        if (first_item_ptr->ptr != NULL) first_item_ptr->found = TRUE;
                         
        /*--------------------------------------------------------------
          get out of the while, found location to insert 
        --------------------------------------------------------------*/
        break;
    }
    
  } while( (walk_item_ptr = 
              DSRLPRSQ_GET_NEXT_ENTRY(reseq_q_ptr, walk_item_ptr)) != NULL);
  
  /*-------------------------------------------------------------------------
    if here, then either the end of the Q has been reached OR we did a break;
  -------------------------------------------------------------------------*/

  if (walk_item_ptr == NULL) 
  {
    /*-----------------------------------------------------------------------
      This is the case when we have exceeded the end of the Q. We want to 
      insert at the TAIL of the Q. Values for first_found, last_found bools
      are incorrect so fix them to know we want to insert at the tail of Q.
    -----------------------------------------------------------------------*/
    ASSERT(last_item_ptr->ptr != NULL);
    first_item_ptr->found      = TRUE;
    first_item_ptr->ptr        = last_item_ptr->ptr;
    last_item_ptr->ptr         = NULL;
    last_item_ptr->found       = FALSE;
  }

  return(walk_item_ptr);

}/* dsrlprsq_fast_walk() */
#endif /* MSM5000_IRAM_FWD */


/*===========================================================================

FUNCTION       DSRLPRSQ_REGULAR_INSERT

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
               
               If an entry already exists in the queue, then is is discarded.

PARAMETERS     rscb_ptr:       pointer to session control block
               frame_info_ptr: pointer to frame information type
               fr_ptr:         pointer to dsm item to insert           
  

DEPENDENCIES   -- Inputs rscb_ptr, frame_info_ptr, fr_ptr  are  non-null.
               -- Need calculated full 32 bit seq # in frame_info_ptr.
               -- Needs the frame to at least have last seq number greater
                  than the value of the LV(N).
               -- Uses "app_field" of incoming dsm item *fr_ptr to store
                  the sequence number
               -- Uses "kind" field of incoming dsm item *fr_ptr to store
                  the LST state

RETURN VALUE   NONE

SIDE EFFECTS   Sets frame_info_ptr to NULL upon exit
===========================================================================*/
static void dsrlprsq_regular_insert
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,      /* curr RLP ctl blk         */
  dsrlpi_curr_fr_info_type     *frame_info_ptr,/* current frame info       */
  dsm_item_type                *fr_ptr         /* the frame to insert      */
)
{
  /*------------------------------------------------------------------------- 
    this holds pointers to items between "last" and "truelast" so they can
    easily be taken out of the resequencing queue in case of an overlap.           
  -------------------------------------------------------------------------*/
  overlap_info_type  overlap_items;
  
  /*------------------------------------------------------------------------- 
    keep information about the "first", "last" and "truelast" items in the
    resequencing queue, as well as the "new_item".            
  -------------------------------------------------------------------------*/
  dsrlpi_rsq_item_info_type   new_item;     /* keep info about "new" item  */ 
  dsrlpi_rsq_item_info_type   first_item;   /* keep info about "first" item*/
  dsrlpi_rsq_item_info_type   last_item;    /* keep info about "last" item */
  dsrlpi_rsq_item_info_type   truelast_item;/* keep info about "truelast"  */
 
  q_type         *reseq_q_ptr       = NULL;/* convenience ptr to reseq Q   */
  
  dsm_item_type  *temp_ptr          = NULL;/* auxiliary pointer for Q ops  */
  dsm_item_type  *append_ptr        = NULL;/* auxiliary pointer for Q ops  */
  dsm_item_type  *truewalk_item_ptr = NULL;/* auxiliary pointer for Q ops  */
  dsm_item_type  *delete_item_ptr   = NULL;/* auxiliary pointer for Q ops  */
  dsm_item_type  *last_item_ptr     = NULL;/* auxiliary pointer for Q ops  */

  uint32          i                 = 0;   /* auxiliary counter            */
  uint32          truelast_distance = 0;   /* auxiliary counter            */
  uint32          rsq_item_count    = 0;   /* # dsm item chains in Reseq Q */
  uint32          seq_mod           = 0;   /* the curent sequence modulus  */
  uint32          needed_seq_num    = 0;   /* keeps the LV(N)              */
  
  boolean         discard_flag      = FALSE;/* is "new" to be discarded ?  */
  boolean         start_distance_cnt= FALSE;/* auxiliary counter           */
  boolean         establish_ptrs    = FALSE;
  /* keeps track if we need to recalculate the fast insert pointers        */

  uint32          pullup_bytes      = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  memset( &new_item,      0, sizeof(dsrlpi_rsq_item_info_type));
  memset( &first_item,    0, sizeof(dsrlpi_rsq_item_info_type));
  memset( &last_item,     0, sizeof(dsrlpi_rsq_item_info_type));
  memset( &truelast_item, 0, sizeof(dsrlpi_rsq_item_info_type));
  memset( &overlap_items, 0, sizeof(overlap_items));
  
  /*-------------------------------------------------------------------------
    get some fields for fast access to the resequencing queue throughout
  -------------------------------------------------------------------------*/
  reseq_q_ptr     = &(rrcb_ptr->state.reseq_q);/* pointer to the reseq Q   */
  seq_mod         =  rrcb_ptr->cfg_ptr->seq_modulus;

  /*-------------------------------------------------------------------------
    read the LV(N) into needed_seq_num. Prepare all the information fields
    in the dsrlpi_rsq_item_info structure for the "new item" 
  -------------------------------------------------------------------------*/
  needed_seq_num  = rrcb_ptr->state.needed_seq_num;

  new_item.ptr    = fr_ptr;
  new_item.length = dsm_length_packet(fr_ptr); /* length of frame to insert*/

  /*-------------------------------------------------------------------------
    get the first sequence # and store in the dsm item chain. In this 
    function the "app_field" is being used to store the sequence number 
  -------------------------------------------------------------------------*/
  new_item.seq_num= fr_ptr->app_field;   

  /*-------------------------------------------------------------------------
    get LST , store in the MSB of "kind" field in dsm  item     
  -------------------------------------------------------------------------*/
  if (frame_info_ptr->fr_cat & CURR_FRAME_LST_BIT_MASK ) 
  {
      new_item.lst_set   = TRUE;
      fr_ptr->kind      |= SET_LST_ON_MASK;
  } 
  else  
  {
      new_item.lst_set   = FALSE;
      fr_ptr->kind      &= SET_LST_OFF_MASK; 
  }
   
  /*-------------------------------------------------------------------------
    get value for "new_item.last_seq_num"      
  -------------------------------------------------------------------------*/
  DSRLPRSQ_GET_LAST_SEQ_NUM(new_item.seq_num,
                            new_item.last_seq_num,
                            new_item.length,
                            seq_mod); 

  /*-------------------------------------------------------------------------
    get value for "new_item.last_plus_seq_num"       
  -------------------------------------------------------------------------*/
  dsrlprsq_get_last_plus_seq_num(rrcb_ptr,
                                 &new_item); 
  /*-------------------------------------------------------------------------
    check the location of the new frame in relation to the LV(N) and see if
    it needs to be chopped. Recalculate any numbers if needed.
  -------------------------------------------------------------------------*/
  if (DSRLPSEQ_GT_SEQ_CHECK(needed_seq_num, new_item.seq_num, seq_mod)) 
  {
    /*-----------------------------------------------------------------------
      (LV(N) > first sequence number) so we need to chop part of the frame 
      and also recalculate the first seq number and the length
    -----------------------------------------------------------------------*/
    pullup_bytes =(uint16)DSRLPSEQ_DIFF_SEQ(needed_seq_num,new_item.seq_num,seq_mod);
  
    (void) dsm_pullup( &(new_item.ptr), NULL, (uint16)pullup_bytes);
    ASSERT(new_item.ptr != NULL);
  
    DSRLPRSQ_ADD_32BIT_SEQ( new_item.seq_num, 
                             pullup_bytes, 
                             seq_mod); 

    (new_item.ptr)->app_field = new_item.seq_num;
    ASSERT(new_item.seq_num == rrcb_ptr->state.needed_seq_num);
    new_item.length  = dsm_length_packet(new_item.ptr);

    
  }

  /*-------------------------------------------------------------------------
    In this case the resequencing Q is either not empty or we needed to chop 
    the incoming frame and we could not insert during dsrlprsq_fast_insert().
    We need to insert at the proper location and also make sure there is no 
    duplication.
  -------------------------------------------------------------------------*/
   
  /*-------------------------------------------------------------------------
    now go ahead and insert, making sure there will be no duplication.
    If the q were empty the insert would be taken care in the fast insert
    function UNLESS chopping the incoming frame was required. Say its first
    seq# was less than LV(N). So make sure the resequencing q is 
    not empty ONLY when no chopping was required.
  -------------------------------------------------------------------------*/
  rsq_item_count = q_cnt( reseq_q_ptr);
  if (rsq_item_count == 0)
  {
  
    /*-----------------------------------------------------------------------
      at this point these pointers should be NULL as the q is empty
    -----------------------------------------------------------------------*/
    ASSERT((rrcb_ptr->state.reseq_last_item_ptr == NULL) &&
           (rrcb_ptr->state.reseq_last_chain_ptr== NULL) &&
           (rrcb_ptr->state.reseq_last_insert_item_ptr == NULL) &&
           (rrcb_ptr->state.reseq_last_insert_chain_ptr== NULL));

    /*----------------------------------------------------------------------- 
      In this case we know the new chain is going to be the last chain.
      This will update all the related pointers.
    -----------------------------------------------------------------------*/
    DSRLPRSQ_GET_LAST_CHAIN_ITEM(last_item_ptr, new_item.ptr);
    rrcb_ptr->state.reseq_last_item_ptr  = last_item_ptr;
    rrcb_ptr->state.reseq_last_chain_ptr = new_item.ptr;
    

    /*----------------------------------------------------------------------- 
      No items in the q at this time, just insert as a first item. 
      The  fr_ptr->link has been initialized and is valid at this point.
    -----------------------------------------------------------------------*/
    q_put( reseq_q_ptr, &(new_item.ptr->link) );

    /*-----------------------------------------------------------------------
      Now your job is done just get out of here !!
    -----------------------------------------------------------------------*/
    frame_info_ptr = NULL;

    return;
  
  }
  
    
  /*-------------------------------------------------------------------------
    Make sure the pointers are not NULL as the q is not empty
  -------------------------------------------------------------------------*/
  ASSERT(( rrcb_ptr->state.reseq_last_chain_ptr != NULL) &&
         ( rrcb_ptr->state.reseq_last_item_ptr  != NULL) &&
         ( (rrcb_ptr->state.reseq_last_item_ptr)->pkt_ptr == NULL));

  /*-------------------------------------------------------------------------
    now find the appropriate location to insert
  -------------------------------------------------------------------------*/
  (void) dsrlprsq_fast_walk
                    (rrcb_ptr, &new_item, &first_item, &last_item); 
  /*-------------------------------------------------------------------------
    We still need to find the "truelast" and build an array of pointers to 
    the items in between "last" and "truelast" so we may easily remove items
    that overlap with the most recent retransmission.
  -------------------------------------------------------------------------*/
 
  /*-------------------------------------------------------------------------   
    start from "first" or if NULL start from "last" and try to find
    truelast. It may be "truelast" is equal to "first" and we know 
    we will always have at least one of the "first" or "last" in a 
    non-empty Q.
  -------------------------------------------------------------------------*/
  if(first_item.ptr != NULL)
  {
    truewalk_item_ptr = first_item.ptr;
  }
  else if (last_item.ptr  != NULL)  
  {
    truewalk_item_ptr = last_item.ptr;
  }
  ASSERT(truewalk_item_ptr != NULL);

  truelast_distance      = 0;
  start_distance_cnt     = FALSE;
  
  /*-------------------------------------------------------------------------
    now walk in the Q and try to find "truelast" with last seq # GREATER or 
    EQUAL to the last seq # of the new dsm item. At the same time count the
    distance between the "last" and "truelast"
  -------------------------------------------------------------------------*/
  do 
  {        
    if (start_distance_cnt)  
    {    
      /*---------------------------------------------------------------------
        this distance will only be valid if we suceed in finding "truelast"
      ---------------------------------------------------------------------*/   
      truelast_distance           += 1;
      
      ASSERT((truelast_distance-1)<DSRLPRSQ_MAX_OVERLAP_ITEMS);

      /*---------------------------------------------------------------------
        this holds pointers to items between "last" and "truelast"   
      ---------------------------------------------------------------------*/
      overlap_items.overlap_count                    = truelast_distance;
      overlap_items.overlap_arr[truelast_distance-1] = truewalk_item_ptr;
    }
        
    /*-----------------------------------------------------------------------
      this quarantees that we have already found "last"
    -----------------------------------------------------------------------*/
    if (truewalk_item_ptr == last_item.ptr) 
    {
        start_distance_cnt = TRUE;
    }

    truelast_item.seq_num  = truewalk_item_ptr->app_field;
    truelast_item.length   = dsm_length_packet(truewalk_item_ptr);
        
    DSRLPRSQ_GET_LAST_SEQ_NUM(truelast_item.seq_num,
                              truelast_item.last_seq_num,
                              truelast_item.length,
                              seq_mod);
        
    if (DSRLPSEQ_GE_SEQ_CHECK
             (truelast_item.last_seq_num, new_item.last_seq_num, seq_mod))
    {
      truelast_item.found   = TRUE;
      truelast_item.ptr     = truewalk_item_ptr;
      truelast_item.lst_set = DSRLPRSQ_GET_LST_STATUS(truewalk_item_ptr);
          
      break;
    }
        
  } while((truewalk_item_ptr = 
           DSRLPRSQ_GET_NEXT_ENTRY(reseq_q_ptr, truewalk_item_ptr)) != NULL);
  
  /*-------------------------------------------------------------------------
    if code is here, you either found what you want or exhausted the Q   
  -------------------------------------------------------------------------*/
  
  /*-------------------------------------------------------------------------
    now go ahead and remove the dsm item chains that need to be removed
    because the distance between truelast and last is more than 1 item   
  -------------------------------------------------------------------------*/
  if ((truelast_item.found) && (truelast_distance > 1)) 
  {
        
    /*-----------------------------------------------------------------------
      the truelast_distance > 1 quarantees that "truelast" appears after 
      "last" in the Q. Now DELETE (truelast_distance-1) dsm items in between    
    -----------------------------------------------------------------------*/
    for ( i=0; i < truelast_distance-1; i++)
    {
      /*---------------------------------------------------------------------
        Make sure we are not taking out of the q any item the fast insert
        pointer is pointing to. If we are, set the pointer as NULL
      ---------------------------------------------------------------------*/
      if(overlap_items.overlap_arr[i] ==rrcb_ptr->state.reseq_last_chain_ptr) 
      {
        rrcb_ptr->state.reseq_last_item_ptr  = NULL;
        rrcb_ptr->state.reseq_last_chain_ptr = NULL;
        establish_ptrs                       = TRUE;
      }/* if */

      if(overlap_items.overlap_arr[i] ==rrcb_ptr->state.reseq_last_insert_chain_ptr) 
      {
        rrcb_ptr->state.reseq_last_insert_item_ptr  = NULL;
        rrcb_ptr->state.reseq_last_insert_chain_ptr = NULL;
      }/* if */


      DSRLPRSQ_Q_DELETE(reseq_q_ptr, overlap_items.overlap_arr[i]);
      dsm_free_packet(&(overlap_items.overlap_arr[i]));
    }
  
    /*---------------------------------------------------------------------
      If we removed an item the fast insert pointers were pointing to then 
      go ahead and reestablish them.
    ---------------------------------------------------------------------*/
    if (establish_ptrs) 
    {
      establish_ptrs = FALSE;
      DSRLPRSQ_REESTABLISH_FAST_PTRS(rrcb_ptr, reseq_q_ptr);
    }

  }/* IF */
 
  
  if ((truelast_item.found == FALSE) && (last_item.found == TRUE)) 
  {
    /*-----------------------------------------------------------------------
      If we do not have a "truelast" but we have "last", need to delete all
      items after the "last" (if any )
    -----------------------------------------------------------------------*/
   	do{
        delete_item_ptr =last_item.ptr;
        delete_item_ptr =DSRLPRSQ_GET_NEXT_ENTRY(reseq_q_ptr, delete_item_ptr);
            
        if (delete_item_ptr != NULL)
        {
          /*-------------------------------------------------------------------
            Make sure we are not taking out of the q any item the fast insert
            pointer is pointing to. If we are, set the pointer as NULL
          -------------------------------------------------------------------*/
          if (delete_item_ptr == rrcb_ptr->state.reseq_last_chain_ptr) 
          {
            rrcb_ptr->state.reseq_last_item_ptr  = NULL;
            rrcb_ptr->state.reseq_last_chain_ptr = NULL;
            establish_ptrs                       = TRUE;
          }/* if */
   
          if (delete_item_ptr == rrcb_ptr->state.reseq_last_insert_chain_ptr) 
          {
            rrcb_ptr->state.reseq_last_insert_item_ptr  = NULL;
            rrcb_ptr->state.reseq_last_insert_chain_ptr = NULL;
          }/* if */
   
          DSRLPRSQ_Q_DELETE(reseq_q_ptr, delete_item_ptr);
          dsm_free_packet(&(delete_item_ptr));
        }
	} 
	while(delete_item_ptr != NULL);
    
    /*---------------------------------------------------------------------
      If we removed an item the fast insert pointers were pointing to then 
      go ahead and reestablish them.
    ---------------------------------------------------------------------*/
    if (establish_ptrs) 
    {
      establish_ptrs = FALSE;
      DSRLPRSQ_REESTABLISH_FAST_PTRS(rrcb_ptr, reseq_q_ptr);
    }

  }/* If */

  /*-------------------------------------------------------------------------
    calculate some values for the "first" and "last" and "truelast" items, 
    and check for overlaps. At the same time make sure we have found "first"
    and "last" e.g. to guard against the case we want to insert at the head 
  -------------------------------------------------------------------------*/
  if (first_item.found) 
  {
    ASSERT(first_item.ptr != NULL);
    first_item.seq_num           = first_item.ptr->app_field;
    first_item.length            = dsm_length_packet(first_item.ptr);
    first_item.lst_set           = DSRLPRSQ_GET_LST_STATUS(first_item.ptr);
        
    dsrlprsq_get_last_plus_seq_num( rrcb_ptr,
                                    &first_item); 
    
    DSRLPRSQ_GET_LAST_SEQ_NUM( first_item.seq_num,
                               first_item.last_seq_num,
                               first_item.length,
                               seq_mod); 
    
    /*-----------------------------------------------------------------------
      check for OVERLAPS and calculate # bytes affected
    -----------------------------------------------------------------------*/
    if (DSRLPSEQ_GT_SEQ_CHECK(new_item.seq_num, first_item.last_seq_num, 
                                     seq_mod)) 
    {
      first_item.overlap_bytes = 0;  
    } 
    else 
    {                     /* we have an overlap, so calculate some numbers */
                
      /*---------------------------------------------------------------------
        now we know that new_item.seq_num is a smaller number. The bytes 
        calculated here need to be chopped off the HEAD of the new item.
      ---------------------------------------------------------------------*/
      first_item.overlap_bytes =  1 + 
        DSRLPSEQ_DIFF_SEQ(first_item.last_seq_num,new_item.seq_num,seq_mod);
  
      
      /*---------------------------------------------------------------------
        make sure this will never happen normally. To call a dsm item "first"
        its first sequence number has to be smaller than the first sequence
        number of the new dsm item chain to be inserted.   
      ---------------------------------------------------------------------*/
      if(first_item.overlap_bytes >= first_item.length)
      {
        /*-----------------------------------------------------------------------
          overlap_bytes >= first item length. This should never happen.
          So free the dsm item and return. Make sure to set the app_field to 0.
        -----------------------------------------------------------------------*/
        DATA_RLP_MSG2(MSG_LEGACY_ERROR,
                      "Overlap bytes %d >= first item length %d",
                      first_item.overlap_bytes, first_item.length);

        fr_ptr->app_field = 0;
        dsm_free_packet(&fr_ptr);
        return;
      }
    } 
  } 

  /*-------------------------------------------------------------------------
    Make sure we have found "last" and if yes then calculate its length, seq
    number, lst_set bit, last seq number and last plus one seq number. 
    Also check for any overlaps.   
  -------------------------------------------------------------------------*/
  if (last_item.found) 
  {

    ASSERT(last_item.ptr != NULL);
    last_item.seq_num            = last_item.ptr->app_field;
    last_item.length             = dsm_length_packet(last_item.ptr);
    last_item.lst_set            = DSRLPRSQ_GET_LST_STATUS(last_item.ptr);
    
    dsrlprsq_get_last_plus_seq_num( rrcb_ptr,
                                    &last_item);     

    DSRLPRSQ_GET_LAST_SEQ_NUM( last_item.seq_num,
                               last_item.last_seq_num,
                               last_item.length,
                               seq_mod);     

    /*-----------------------------------------------------------------------
      check for OVERLAPS and calculate # of bytes affected 
    -----------------------------------------------------------------------*/
    if (DSRLPSEQ_GT_SEQ_CHECK(last_item.seq_num, new_item.last_seq_num, 
         seq_mod)) 
    {
      last_item.overlap_bytes = 0;  
    }
    else 
    { 
      /*---------------------------------------------------------------------
        we have an overlap, so calculate some numbers 
      ---------------------------------------------------------------------*/   

      /*---------------------------------------------------------------------
        these bytes need to be chpped off the TAIL of the new item
      ---------------------------------------------------------------------*/
      last_item.overlap_bytes = 1 + DSRLPSEQ_DIFF_SEQ
                                      (new_item.last_seq_num,
                                       last_item.seq_num,
                                       seq_mod);
        
    } 
  }


  /*-------------------------------------------------------------------------
    Make sure we have found "truelast" and if yes then calculate its length,
    seq number, lst_set bit, last seq number and last plus one seq number. 
    Also check for any overlaps.   
  -------------------------------------------------------------------------*/  
  if (truelast_item.found) 
  {

    ASSERT(truelast_item.ptr != NULL);
    truelast_item.seq_num = truelast_item.ptr->app_field;
    truelast_item.length  = dsm_length_packet(truelast_item.ptr);
    truelast_item.lst_set = DSRLPRSQ_GET_LST_STATUS(truelast_item.ptr);
        
    dsrlprsq_get_last_plus_seq_num( rrcb_ptr,
                                    &truelast_item);     

    DSRLPRSQ_GET_LAST_SEQ_NUM( truelast_item.seq_num,
                               truelast_item.last_seq_num,
                               truelast_item.length,
                               seq_mod);

    /*-----------------------------------------------------------------------
      check for OVERLAPS and calculate # bytes affected
    -----------------------------------------------------------------------*/
    if (DSRLPSEQ_GT_SEQ_CHECK(truelast_item.seq_num, 
                              new_item.last_seq_num, 
                              seq_mod)) 
    {
      truelast_item.overlap_bytes = 0;  
    } 
    else
    { 
      /*---------------------------------------------------------------------
        we have an overlap, so calculate some numbers 
      ---------------------------------------------------------------------*/   

      /*---------------------------------------------------------------------
        the first parameter passed into DSRLPI_DIFF_SEQ is the bigger #
      ---------------------------------------------------------------------*/
      truelast_item.overlap_bytes = 1 + DSRLPSEQ_DIFF_SEQ
                                          (new_item.last_seq_num,
                                           truelast_item.seq_num,
                                           seq_mod);
        
    }

  }

  /*-------------------------------------------------------------------------
    After you have found the "first" and "last" all you have to do is 
    check for any overlaps,  F bytes with "first" and L bytes with "last" 
    Then see what kind of Q operations you are going to need. The cases 
    that can happen are: (assume "first" and "last" point to different
    dsm item chains)

      -- no overlap at all

      -- overlap with multiple elements !! keep track of dsm item chains 
         between "first" and "last", if over 0, then  this is the case
          
      -- partial  overlap with "first" only
      -- complete overlap with "first" only 
          
      -- partial  overlap with "last"  only
      -- complete overlap with "last"  only 
          
      -- partial  overlap with "first" and "last"
      -- complete overlap with "first" and "last"   
      -- partial  with "first" and full with "last" 
      -- partial  with "last"  and full with "first"
          
      If the "first" and "last" are the same dsm item it must be the case 
      that there is only one dsm item in the chain at this time and we need
      to insert before it, so we really do not have "first". So the "first"
      will be NULL and only the "last" will have a value.
  -------------------------------------------------------------------------*/
  if (first_item.found) 
  {
   
    /*-----------------------------------------------------------------------
      check for overlaps with "first" and chop if required. If there is no
      overlap with "first" there can not be DISCARD,REPLACE or CHOP. 
    -----------------------------------------------------------------------*/
    if (first_item.overlap_bytes > 0)
    {
                  
      if (DSRLPRSQ_DISCARD_NEW_FRAME(new_item, first_item, seq_mod))
      {
        discard_flag     = TRUE; 
        new_item.discard = TRUE; 
          
      } 
      else if (DSRLPRSQ_REPLACE_OLD_FRAME(new_item, first_item, seq_mod)) 
      {
        first_item.replace = TRUE;
                  
      } 
      else 
      {
                          
        /*-------------------------------------------------------------
          we know it is OVERLAP and it is not DISCARD or REPLACE so it 
          has to be CHOP the new frame from the head with dsm_pullup()
        -------------------------------------------------------------*/
        pullup_bytes = dsm_pullup( &(new_item.ptr), NULL, (uint16)first_item.overlap_bytes);
                  
        /*-------------------------------------------------------------
          make sure we have pulled up as many as we think we should
        -------------------------------------------------------------*/
        ASSERT((pullup_bytes == first_item.overlap_bytes) &&
          (new_item.ptr != NULL));
                          
        /*-------------------------------------------------------------
          if we pullup the whole item the pointer will be NULL but it
          should never happen as we check for the DISCARD right above
        -------------------------------------------------------------*/
   

        /*-------------------------------------------------------------
          Update fields that changed as a result of the bytes chopped
          -- the first seq num have changed
          -- length has changed
        -------------------------------------------------------------*/
        DSRLPRSQ_ADD_32BIT_SEQ( new_item.seq_num,
                                 first_item.overlap_bytes, 
                                 seq_mod); 
                          
        (new_item.ptr)->app_field = new_item.seq_num;
        ASSERT(new_item.seq_num < seq_mod);

        new_item.length  = dsm_length_packet(new_item.ptr); 
                  
      }/* if{}else{} */

    }/* overlap */ 
  
  }/* if found */

 

  if(truelast_item.found && (truelast_item.ptr!=first_item.ptr) && 
         !discard_flag) 
  {
    /*-----------------------------------------------------------------------
      check for overlaps with "truelast" and chop if required 
    -----------------------------------------------------------------------*/
  
    /*---------------------------------------------------------------------
      if there is no overlap with "truelast" there can not be DISCARD
      or REPLACE or CHOP required.
    ---------------------------------------------------------------------*/
    if (truelast_item.overlap_bytes > 0)
    {

      /*-----------------------------------------------------------------
        check for any DISCARD
      -----------------------------------------------------------------*/
      if (DSRLPRSQ_DISCARD_NEW_FRAME(new_item, truelast_item, seq_mod))
      {
        discard_flag     = TRUE;
        new_item.discard = TRUE;
          
      } 
      else if(DSRLPRSQ_REPLACE_OLD_FRAME(new_item, truelast_item, seq_mod)) 
      {
        truelast_item.replace = TRUE;
                  
      } 
      else 
      {
                               
        /*-------------------------------------------------------------
          it should never be the case the overlap is more than the 
          length of the new_item even if we chopped bytes from the head
          because of an overlap with "first". In here we are guarding 
          with the "discard" flag.
        -------------------------------------------------------------*/
        ASSERT(truelast_item.overlap_bytes <= new_item.length);

                          
        /*-------------------------------------------------------------
          we know we have OVERLAP and it is not DISCARD or REPLACE so 
          it has to be CHOP new frame from the tail with dsm_pullup()
        --------------------------------------------------------------*/
        pullup_bytes = dsm_pullup_tail
            ( &(new_item.ptr), NULL, (word)truelast_item.overlap_bytes );

        /*-------------------------------------------------------------
          if we pullup the whole item the pointer will be NULL but it
          should never happen as we check for the DISCARD right above
        --------------------------------------------------------------*/
                  
        ASSERT(( pullup_bytes == truelast_item.overlap_bytes) &&
               ( new_item.ptr != NULL));
        
        
        

              
        /*-------------------------------------------------------------
          Update fields that have changed as a result of bytes chopped
          -- the last  seq num have changed
          -- the last plus 1 seq num has changed
          -- length has changed
        --------------------------------------------------------------*/
        new_item.length  = dsm_length_packet(new_item.ptr); 
        ASSERT( new_item.length > 0);

                          
        dsrlprsq_get_last_plus_seq_num( rrcb_ptr,
                                        &new_item);     
              
        DSRLPRSQ_GET_LAST_SEQ_NUM( new_item.seq_num,
                                   new_item.last_seq_num,
                                   new_item.length,
                                   seq_mod);
                                          
      }/* if{}else{} */

    }/* overlap */ 
  
  }

  if (last_item.found && (last_item.ptr!=truelast_item.ptr) && !discard_flag) 
  {
    /*-----------------------------------------------------------------------
      Check for overlaps with "last" and chop if required. If there is no 
      overlap with "truelast" it can not be DISCARD,REPLACE or CHOP required.
    -----------------------------------------------------------------------*/
          
    if (last_item.overlap_bytes > 0)
    {
      /*-----------------------------------------------------------------
        check for any DISCARD     
      -----------------------------------------------------------------*/
      if (DSRLPRSQ_DISCARD_NEW_FRAME(new_item, last_item, seq_mod))
      {
        /*-------------------------------------------------------------
          indicates the new frame may be discarded 
        -------------------------------------------------------------*/
        discard_flag     = TRUE;
        new_item.discard = TRUE;
          
      } 
      else if(DSRLPRSQ_REPLACE_OLD_FRAME(new_item, last_item, seq_mod)) 
      {
        /*-------------------------------------------------------------
          indicates at least one existing item needs to be replaced 
        -------------------------------------------------------------*/
        last_item.replace = TRUE;
                  
      } 
      else 
      {

        /*-------------------------------------------------------------------
          it should never be the case the overlap is more than the length
          of the new_item even if we chopped bytes from the head because
          of an overlap with "first" 
        -------------------------------------------------------------------*/
        if(last_item.overlap_bytes >= new_item.length)
        {
          /*-----------------------------------------------------------------------
            overlap_bytes >= new item length. This should never happen.
            So free the dsm item and return. Make sure to set the app_field to 0.
          -----------------------------------------------------------------------*/
          DATA_RLP_MSG2(MSG_LEGACY_ERROR,
                        "Overlap bytes %d >= new item length %d",
                        last_item.overlap_bytes, new_item.length);

          fr_ptr->app_field = 0;
          dsm_free_packet(&fr_ptr);
          return;
        }

        /*-------------------------------------------------------------------
          we know we have OVERLAP and it is not DISCARD or REPLACE so it 
          has to be CHOP the new frame from the head with dsm_pullup() 
        -------------------------------------------------------------------*/
        pullup_bytes = dsm_pullup_tail
                        (&(new_item.ptr),NULL,(word)last_item.overlap_bytes);
                  
        ASSERT(pullup_bytes == last_item.overlap_bytes);
                          
        /*-------------------------------------------------------------------
          Update fields that have changed as a result of the bytes chopped
          -- the last  seq num have changed
          -- the last plus 1 seq num has changed
          -- length has changed
        -------------------------------------------------------------------*/
        new_item.length  = dsm_length_packet(new_item.ptr); 
        ASSERT( new_item.length > 0);
                          
        dsrlprsq_get_last_plus_seq_num( rrcb_ptr,
                                        &new_item);     
              
        DSRLPRSQ_GET_LAST_SEQ_NUM( new_item.seq_num,
                                   new_item.last_seq_num,
                                   new_item.length,
                                   seq_mod);
                                          
      }/* if {} else {} */

    }/* overlap */

  }

  /*---------------------------------------------------------------------
    Get a pointer to the very last item of the new chain
  ---------------------------------------------------------------------*/
  DSRLPRSQ_GET_LAST_CHAIN_ITEM(last_item_ptr, new_item.ptr);

  /*-------------------------------------------------------------------------
    You now have 3 cases: either  need to insert at the HEAD of the Q or at 
    the TAIL of the Q or need to insert in the MIDDLE of the Q. You can 
    distinguish among these cases by simply checking the values of the bools.

    In this case we are not checking the state of "truelast" as it will not 
    affect the location we want to insert into the Q.
  -------------------------------------------------------------------------*/
  if ( !discard_flag )
  { 
  
    if (first_item.found && !last_item.found) 
    {
      /*---------------------------------------------------------------------
        in this case we need to insert at the TAIL of the Q as we can not 
        find a dsm item chain with a bigger first seq # than the new one. 
        This will be the most common case. Use the first_item_ptr to insert
      ---------------------------------------------------------------------*/
      
      /*---------------------------------------------------------------------
        Make sure the last chain ptr is the same as the "first" item
      ---------------------------------------------------------------------*/
      ASSERT( first_item.ptr == rrcb_ptr->state.reseq_last_chain_ptr);

      /*---------------------------------------------------------------------
        in order to APPEND you need the seq# to be sequential, the LST bit
        indicator to not be on,  and the "first" not about to be replaced.        
      ---------------------------------------------------------------------*/
      if ((!first_item.replace)                               &&
          ( first_item.last_plus_seq_num == new_item.seq_num) &&
          (!first_item.lst_set))
      {
      
        /*-----------------------------------------------------------------
          Append at the end of "first" and update LST bit of the chain.
          This is the fast APPEND    
        -----------------------------------------------------------------*/
        (rrcb_ptr->state.reseq_last_item_ptr)->pkt_ptr= new_item.ptr;
        rrcb_ptr->state.reseq_last_item_ptr           = last_item_ptr;
        
        if (new_item.lst_set)
        {
         /*---------------------------------------------------------------
           update the LST bit state for "first" in first_item.lst_set and 
           first_item.ptr->kind
         ---------------------------------------------------------------*/
         first_item.lst_set    = TRUE;/* this will probably not be used */
         first_item.ptr->kind |= SET_LST_ON_MASK;/* set the LST bit     */

         /*---------------------------------------------------------------
           also should update last seq numbers e.t.c. in the "first" 
           except we know they will not be used any more in this function
           in this call.    
         ---------------------------------------------------------------*/
        }

      } 
      else
      {
        /*-----------------------------------------------------------------
          in this case do not append, just update the fast insert pointers
          and insert at the TAIL.    
        -----------------------------------------------------------------*/
        rrcb_ptr->state.reseq_last_chain_ptr    = new_item.ptr;
        rrcb_ptr->state.reseq_last_item_ptr     = last_item_ptr;

        q_put(reseq_q_ptr, &(new_item.ptr->link));

      }
              

    } 
    else if (last_item.found && (!first_item.found)) 
    {
  
      /*---------------------------------------------------------------------
        in this case we need to insert at the HEAD of a non-empty Q.
        The "last" item happens to be the very first one in the queue. 
      ---------------------------------------------------------------------*/

      /*---------------------------------------------------------------------
        in order to APPEND you need the seq# to be sequential, the LST bit
        indicator to not be on,  and the "last" not about to be replaced.        
      ---------------------------------------------------------------------*/
      if ((!last_item.replace)                               &&
          ( new_item.last_plus_seq_num == last_item.seq_num) &&
          (!new_item.lst_set))
      {
        
        /*-------------------------------------------------------------------
          dequeue "last" item from the head of the queue    
        -------------------------------------------------------------------*/
        temp_ptr = (dsm_item_type *)q_get(reseq_q_ptr);
        ASSERT(temp_ptr == last_item.ptr);  
        
        /*-------------------------------------------------------------------
          append "last" at the end of "new" and update LST bit for the chain    
        -------------------------------------------------------------------*/
        append_ptr = last_item.ptr;
        dsm_append( &(new_item.ptr), &(append_ptr));

                  
        if (last_item.lst_set)
        {
          /*------------------------------------------------------------------
            update the LST bit state for "new" in new_item.lst_set and 
            new_item.ptr->kind
          ------------------------------------------------------------------*/
          new_item.lst_set    = TRUE;/* this will probably not be used      */
          new_item.ptr->kind |= SET_LST_ON_MASK;/* set the LST bit          */

          /*------------------------------------------------------------------
            also should update last seq numbers e.t.c. in the "new" 
          ------------------------------------------------------------------*/
        }

        temp_ptr = (dsm_item_type *)q_check(reseq_q_ptr);
        if (temp_ptr != NULL) 
        {
          if( rrcb_ptr->state.reseq_last_insert_chain_ptr == last_item.ptr)
          {
            /*---------------------------------------------------------------
              If we have appended to a chain the middle fast insert pointers
              were pointing to, then we need to update the insert_chain_ptr 
            ---------------------------------------------------------------*/
            rrcb_ptr->state.reseq_last_insert_chain_ptr = new_item.ptr;

          }

          /*-----------------------------------------------------------------
            just insert at the head of the  Q before everything else. In this
            case no need to update fast insert pointers as they point to a
            different dsm item chain than this, unless if we removed them 
            before... 
          -----------------------------------------------------------------*/
          DSRLPRSQ_Q_INSERT(reseq_q_ptr,  
                &(new_item.ptr->link), &(temp_ptr->link));

          
        } 
        else
        {
          /*-----------------------------------------------------------------
            Update the fast insert pointer  
          -----------------------------------------------------------------*/
          rrcb_ptr->state.reseq_last_chain_ptr = new_item.ptr;
          
          /*-----------------------------------------------------------------
            in this case the Q is empty  after we took out the one item 
            to append. This case requires fast insert pointer udate. The 
            last_item_ptr does not change but the last_chain_ptr is.
          -----------------------------------------------------------------*/

          q_put(reseq_q_ptr, &(new_item.ptr->link));

        }
      }
      else 
      {

        /*-------------------------------------------------------------------
          just insert in the Q before "last".  
        -------------------------------------------------------------------*/
        DSRLPRSQ_Q_INSERT(reseq_q_ptr,  
                &(new_item.ptr->link), &(last_item.ptr->link));
      
      }/* If */
    
    } 
    else 
    {
      /*---------------------------------------------------------------------
        in this case we need to insert in the MIDDLE of the Q. 
      ---------------------------------------------------------------------*/
 
      /*---------------------------------------------------------------------
        make sure that we actually have "first" and "last"
      ---------------------------------------------------------------------*/
      ASSERT((first_item.ptr   != NULL) &&
             (last_item.ptr    != NULL) &&
             (last_item.found  == TRUE) &&
             (first_item.found == TRUE));
      
      /*---------------------------------------------------------------------
        update the fast insert pointers that are based on last chain inserted
        to the "middle" of re-sequencing queue.
      ---------------------------------------------------------------------*/
      rrcb_ptr->state.reseq_last_insert_chain_ptr = new_item.ptr;
      rrcb_ptr->state.reseq_last_insert_item_ptr  = last_item_ptr;
      
      /*---------------------------------------------------------------------
        Here we have both "first" and "last" and "truelast" is a "don't care" 
        We want to insert in the MIDDLE of the q before "last" so just insert
        in the Q before "last"
      ---------------------------------------------------------------------*/
      DSRLPRSQ_Q_INSERT(reseq_q_ptr,
                 &(new_item.ptr->link), &(last_item.ptr->link));
 
    }/* if {} else {} */

    /*-----------------------------------------------------------------------
      This is a safe way to do the REPLACE as we now have already inserted.
      Before inserting or appending we also check to see that the replace
      flag is not set. If it is we will not append even if we could, but 
      insert instead. If the "replace" flag is set this quarantees "found"
    -----------------------------------------------------------------------*/
    if (first_item.replace) 
    {
      ASSERT(first_item.ptr != NULL);

      if (first_item.ptr == truelast_item.ptr)
      {
        truelast_item.replace = FALSE;
      }
    
      if (first_item.ptr == rrcb_ptr->state.reseq_last_chain_ptr) 
      {
        establish_ptrs = TRUE;
      }
      
      if (first_item.ptr == rrcb_ptr->state.reseq_last_insert_chain_ptr) 
      {
        rrcb_ptr->state.reseq_last_insert_chain_ptr = NULL;
        rrcb_ptr->state.reseq_last_insert_item_ptr  = NULL;
      }
      
        DSRLPRSQ_Q_DELETE(reseq_q_ptr, first_item.ptr);
        dsm_free_packet( &(first_item.ptr)); 
      }     
  
  
    if (last_item.replace) 
    { 
      if (last_item.ptr == truelast_item.ptr)
      {
        truelast_item.replace = FALSE;
      }
    
      if (last_item.ptr == rrcb_ptr->state.reseq_last_chain_ptr) 
      {
        establish_ptrs = TRUE;
      }
      
      if (last_item.ptr == rrcb_ptr->state.reseq_last_insert_chain_ptr) 
      {
        rrcb_ptr->state.reseq_last_insert_chain_ptr = NULL;
        rrcb_ptr->state.reseq_last_insert_item_ptr  = NULL;
      }

        DSRLPRSQ_Q_DELETE(reseq_q_ptr, last_item.ptr);
        dsm_free_packet( &(last_item.ptr)); 
      }

  
    if (truelast_item.replace)  
    {
      
      if (truelast_item.ptr == rrcb_ptr->state.reseq_last_chain_ptr) 
      {
        establish_ptrs = TRUE;
      }
      
      if (truelast_item.ptr == rrcb_ptr->state.reseq_last_insert_chain_ptr) 
      {
         rrcb_ptr->state.reseq_last_insert_chain_ptr = NULL;
         rrcb_ptr->state.reseq_last_insert_item_ptr  = NULL;
      }
      if( NULL != truelast_item.ptr )
      {
        DSRLPRSQ_Q_DELETE(reseq_q_ptr, truelast_item.ptr);
        dsm_free_packet( &(truelast_item.ptr));   
      }
    }
    
    if (establish_ptrs) 
    {
      establish_ptrs = FALSE;
      DSRLPRSQ_REESTABLISH_FAST_PTRS(rrcb_ptr, reseq_q_ptr);
    }

  }/* IF !discarded */


  
 
  /*-------------------------------------------------------------------------
    do not want anyone to use this any longer as we may have changed the 
    length of the frame. If we need to chop off any bytes of the frame, we
    do not pass that info to the outside world by updating the length or the
    first seq#.
  -------------------------------------------------------------------------*/
  frame_info_ptr = NULL;

}/* dsrlprsq_regular_insert() */


#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION       DSRLPRSQ_INSERT

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
               
               If an entry already exists in the queue, then is is discarded.

PARAMETERS     rscb_ptr:       pointer to session control block
               frame_info_ptr: pointer to frame information type
               fr_ptr:         pointer to dsm item to insert           
  

DEPENDENCIES   -- Inputs rscb_ptr, frame_info_ptr, fr_ptr  are  non-null.
               -- Need calculated full 32 bit seq # in frame_info_ptr.
               -- Needs the frame to at least have last seq number greater
                  than the value of the LV(N).
               -- Uses "app_field" of incoming dsm item *fr_ptr to store
                  the sequence number
               -- Uses "kind" field of incoming dsm item *fr_ptr to store
                  the LST state

RETURN VALUE   NONE

SIDE EFFECTS   Sets frame_info_ptr to NULL upon exit
===========================================================================*/
void dsrlprsq_insert
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,      /* curr RLP ctl blk         */
  dsrlpi_curr_fr_info_type     *frame_info_ptr,/* current frame info       */
  dsm_item_type                *fr_ptr         /* the frame to insert      */
)
{
  dsm_item_type                *tmp_fr_ptr;
  /*------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    make sure none of the pointers being passed in is NULL
  -------------------------------------------------------------------------*/
  ASSERT(( rrcb_ptr       != NULL ) &&
         ( frame_info_ptr != NULL ) &&
         ( fr_ptr         != NULL ));

  fr_ptr->app_field = frame_info_ptr->seq_num;   


  /*---------------------------------------------------------------------- 
    Go through the chain of DSM items and set the app_field with the 
    appropriated sequence number calculated.
  -----------------------------------------------------------------------*/
  tmp_fr_ptr = fr_ptr;
  while (tmp_fr_ptr->pkt_ptr != NULL)
  {
    DSRLPRSQ_GET_LAST_SEQ_NUM(tmp_fr_ptr->app_field,
                              tmp_fr_ptr->pkt_ptr->app_field,
                              tmp_fr_ptr->used,
                              rrcb_ptr->cfg_ptr->seq_modulus);
    tmp_fr_ptr = tmp_fr_ptr->pkt_ptr;

  }

  rex_enter_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));
  if (dsrlprsq_fast_insert(rrcb_ptr, frame_info_ptr, fr_ptr)) 
  {
    rex_leave_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));
    return;
  }
  else
  {
    /*-------------------------------------------------------------------------
      In case fast insert can not be used call the regular insert.
    -------------------------------------------------------------------------*/
    dsrlprsq_regular_insert(rrcb_ptr, frame_info_ptr, fr_ptr);
  }
  rex_leave_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));
}
#endif /* MSM5000_IRAM_FWD */

 

#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION       DSRLPRSQ_REMOVE

DESCRIPTION    This function will keep removing dsm item chains from the
               resequencing queue as long as their sequence numers are
               less that LV(N) i.e. no data holes. The second parameter 
               max_chain_cnt is the max number of chains that may be removed
               from the resequence q per call of this function.

                              
PARAMETERS     rscb_ptr         : pointer to the dsrlpi_sess_ctl_blk_type

               max_dsm_chain_cnt: indicates the max number of dsm item 
                                  chains to be removed from the Q. If set
                                  to 0 as many chains as possible will be 
                                  removed.
               
DEPENDENCIES   Input rscb_ptr is non-null.  

               
RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void dsrlprsq_remove
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,      /* curr RLP ctl blk         */
  uint32                        max_chain_cnt
)
{
  q_type         *reseq_q_ptr       = NULL;/* convenience ptr to reseq Q   */
  
  dsm_item_type  *walk_item_ptr     = NULL;/* auxiliary dsm item pointer   */ 
  dsm_item_type  *insert_item_ptr   = NULL;/* auxiliary dsm item pointer   */
  dsm_item_type  *chain_walk_ptr    = NULL;/* auxiliary dsm item pointer   */
  dsm_item_type  *temp_item_ptr     = NULL;/* auxiliary dsm item pointer   */

  uint32          walk_seq_num      = 0;   /* seq# of the walk ptr item    */
  uint32          rsq_item_count    = 0;   /* # dsm item chains in Reseq Q */
  uint32          seq_mod           = 0;   /* the curent sequence modulus  */
  uint32          chain_cnt         = 0;   /* keep # of dsm chains removed */
  uint32          needed_seq_num    = 0;   /* keep the LV(N)               */

  boolean         chain_cnt_imposed = FALSE;
  boolean         LVN_exceeded      = FALSE;
 
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    make sure none of the pointers being passed in is NULL
  -------------------------------------------------------------------------*/
  ASSERT( rrcb_ptr       != NULL );
 
  /*-------------------------------------------------------------------------
    get some fields for fast access to the resequencing queue throughout
  -------------------------------------------------------------------------*/
  reseq_q_ptr     = &(rrcb_ptr->state.reseq_q);/* pointer to the reseq Q   */

  rsq_item_count = q_cnt(reseq_q_ptr);
  switch (rsq_item_count) 
  {

  case 0: 
    /*-----------------------------------------------------------------------
      if the Q has no dsm item chains to begin with, do not waste time
    -----------------------------------------------------------------------*/
      rrcb_ptr->state.reseq_q_more_data = FALSE;
    
    /*-----------------------------------------------------------------------
      If the queue is empty then we should have set these pointers to NULL
      when removing the very last remaining item.
    -----------------------------------------------------------------------*/
      ASSERT((rrcb_ptr->state.reseq_last_chain_ptr == NULL) &&
             (rrcb_ptr->state.reseq_last_item_ptr  == NULL) &&
             (rrcb_ptr->state.reseq_last_insert_chain_ptr == NULL) &&
             (rrcb_ptr->state.reseq_last_insert_item_ptr  == NULL));

    break;
  
  default:
  /*-------------------------------------------------------------------------
    In this case the resequencing Q is not empty. We can walk and remove at 
    least one dsm item chain (if before LV(N)) and may be more if contiguous.
  -------------------------------------------------------------------------*/
     
  /*-------------------------------------------------------------------------
    Get a pointer to the first dsm item chain in the Q.
    Should not have NULL value,  as we know  there is at least 1 Q entry
  -------------------------------------------------------------------------*/
  walk_item_ptr = (dsm_item_type *)q_check(reseq_q_ptr);
  ASSERT(walk_item_ptr != NULL);

  /*-------------------------------------------------------------------------
    get the LV(N)  
  -------------------------------------------------------------------------*/
  needed_seq_num  = rrcb_ptr->state.needed_seq_num;
  seq_mod         = rrcb_ptr->cfg_ptr->seq_modulus;       

  /*-------------------------------------------------------------------------
    increase by 1 for convenient use in the code for the case of ==0  
  -------------------------------------------------------------------------*/
  if (max_chain_cnt++ > 0) 
  {
    chain_cnt_imposed = TRUE; 
  }
  else
  {
    chain_cnt_imposed = FALSE;
  }
  
  /*-------------------------------------------------------------------------
    as long as the Q is non-empty, and the dsm item chains sequential, then
    continue putting them in the watermark structure until you hit the max 
    dsm chains count, or you empty the Q, or encouter a data  discontinuity.
  -------------------------------------------------------------------------*/
  do 
  { 
    /*-----------------------------------------------------------------------
      get the sequence # for this dsm item chain  
    -----------------------------------------------------------------------*/
    walk_seq_num     = DSRLPRSQ_GET_SEQ_NUM(walk_item_ptr);

    if (DSRLPSEQ_GE_SEQ_CHECK(needed_seq_num, walk_seq_num, seq_mod))
    {
      /*---------------------------------------------------------------------
        get the first dsm item chain out of the Resequencing Q and set the
        app_field (which stored the sequence number) to 0.
      ---------------------------------------------------------------------*/
      insert_item_ptr = (dsm_item_type *)q_get(reseq_q_ptr);
      ASSERT(insert_item_ptr != NULL);
      
      /*---------------------------------------------------------------------
        Here need to set the middle insert pointers to NULL in case we 
        remove the item they are pointing to.    
      ---------------------------------------------------------------------*/
      if (insert_item_ptr == rrcb_ptr->state.reseq_last_insert_chain_ptr) 
      {
        rrcb_ptr->state.reseq_last_insert_chain_ptr = NULL;
        rrcb_ptr->state.reseq_last_insert_item_ptr  = NULL;
      }
      
      insert_item_ptr->app_field = 0;

      /*---------------------------------------------------------------------
         go down the whole chain and set app_field to 0.
      ---------------------------------------------------------------------*/
      chain_walk_ptr = insert_item_ptr;
      while ((chain_walk_ptr = chain_walk_ptr->pkt_ptr) != NULL) 
      {
        chain_walk_ptr->app_field = 0;
      }
      rrcb_ptr->stats.rx_total_bytes += dsm_length_packet(insert_item_ptr);

      /*---------------------------------------------------------------------
        put the item onto the rx queue, and call the callback.  Callback
        assumes non-chained items, so enqueue each item in the pkt list
        separately.
      ---------------------------------------------------------------------*/
      do
      {
        temp_item_ptr          = insert_item_ptr;
        insert_item_ptr        = insert_item_ptr->pkt_ptr;
        temp_item_ptr->pkt_ptr = NULL;


            dsm_enqueue(rrcb_ptr->cfg_ptr->rx_wm_ptr[rrcb_ptr->route], &temp_item_ptr);

      } while (insert_item_ptr != NULL);
      
      rrcb_ptr->cfg_ptr->rx_func_ptr(rrcb_ptr->cfg_ptr->rx_data);
   
      /*---------------------------------------------------------------------
        Set the flag if a PPP packet is received first time after dormancy
      ---------------------------------------------------------------------*/
      rrcb_ptr->state.ppp_data_transfer_after_dormancy = TRUE;
   
      if (chain_cnt_imposed)
      {
        chain_cnt++;
      }
    } 
    else 
    {        
      /*---------------------------------------------------------------------
        fails the LV(N) test,  get out of do{}while() !!   
      ---------------------------------------------------------------------*/   
      LVN_exceeded = TRUE;
          rrcb_ptr->state.reseq_q_more_data = FALSE;

      break;
    }
        
    walk_item_ptr   =  (dsm_item_type *)q_check(reseq_q_ptr);
    
    /*---------------------------------------------------------------------
      Here need to set the fast insert pointers to NULL in case we 
      completely empty the resequencing q    
    ---------------------------------------------------------------------*/
    if (walk_item_ptr == NULL) 
    {
      rrcb_ptr->state.reseq_last_chain_ptr = NULL;
      rrcb_ptr->state.reseq_last_item_ptr  = NULL;
      rrcb_ptr->state.reseq_last_insert_chain_ptr = NULL;
      rrcb_ptr->state.reseq_last_insert_item_ptr  = NULL;
    }

  } while ((walk_item_ptr != NULL) && (chain_cnt < max_chain_cnt));
    
  /*-------------------------------------------------------------------------
    if here, then either max_chain_cnt was reached or the end of the Q. If 
    we exceeded max_chain_cnt send a signal to indicate there is more data.
    The conditions below can only be true if chain_cnt_imposed==TRUE.
  -------------------------------------------------------------------------*/
  if ((chain_cnt     >= max_chain_cnt) && 
      (walk_item_ptr != NULL)          &&
      (LVN_exceeded  == FALSE))
  {
    
    /*-----------------------------------------------------------------------
      if the code is here it can only be because of the max chain
      count being exceeded         
    -----------------------------------------------------------------------*/
    walk_seq_num    = DSRLPRSQ_GET_SEQ_NUM(walk_item_ptr);
   
    /*-----------------------------------------------------------------------
      make sure it meets the LV(N) test 
    -----------------------------------------------------------------------*/
    if ( DSRLPSEQ_GT_SEQ_CHECK(needed_seq_num, walk_seq_num, seq_mod))
    {
      /*---------------------------------------------------------------------
        SET the more_reseq_Q_data = TRUE; if no data discontinuity
        with the new dsm item chain walk_item_ptr is pointing   
      ---------------------------------------------------------------------*/
      rrcb_ptr->state.reseq_q_more_data = TRUE;
    }
  } 
  else 
  {    
        rrcb_ptr->state.reseq_q_more_data = FALSE;
  }
  
  break;
  }/* Switch */

}/* FUNC */
#endif /* MSM5000_IRAM_FWD */
#endif /* RLP 3 || HDR */


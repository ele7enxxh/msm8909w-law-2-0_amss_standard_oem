#ifndef DSRLPRSQ_H
#define DSRLPRSQ_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  R A D I O   L I N K   P R O T O C O L  R E S E Q U E N C I N G  Q U E U E  
  
                          P R O C E S S I N G  

GENERAL DESCRIPTION
  
  This header file contains generic RSQ-list processing functions for the 
  RLP software. It is not specific to any one RLP type.

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
  -- RLP configuration and initialization procedures.
  
  -- Resequencing queue initialization through call to dsrlprsq_init() for
  each separate rlp session.

 Copyright (c) 1999 - 2003 by Qualcomm Technologies Incorporated.  All Rights Reserved.
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsrlprsq.h_v   1.4   04 Sep 2001 12:09:26   hgilkey  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/1xrlp/inc/dsrlprsq.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        -------------------------------------------------------
11/15/05   ksu        EMPA: packet stream, octet sequencing (forward link also)
04/21/03   ik/js      Modified to work defeaturized queues. 
09/04/01   hcg        Added dsrlprsq_remove_chain() to pass chains of DSM
                      items to upper layer.  
07/02/01   ak         Added macro to return num chains in reseq queue.
11/01/00   igt        incorporated sw review comments into the code
10/10/00   igt        incorporated code review comments.
05/16/00   igt        Add SET_LST_ON and _OFF masks.
11/01/99   igt        Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"

#ifdef FEATURE_DS

#include "dsrlp_v.h"
#include "dsrlpi.h"
#include "dsrlpseq.h"


/*===========================================================================

             PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/


/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                       MACRO DEFINITIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/
/*===========================================================================
MACRO          DSRLPRSQ_CNT

DESCRIPTION    Returns number of chains in resequencing queue.
           
PARAMETERS     rscb_ptr: helps distinguish among multiple RLP sessions
               
DEPENDENCIES   The rscb_ptr passed in should be non-NULL

RETURN VALUE   Number of chains in resequencing queue.

SIDE EFFECTS   NONE
===========================================================================*/
#define DSRLPRSQ_CNT(rrcb_ptr)\
  (q_cnt( &rrcb_ptr->state.reseq_q ))

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                      FUNCTION DEFINITIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/

/*===========================================================================

FUNCTION       DSRLPRSQ_INIT

DESCRIPTION    Called during initialization of the resequencing queue. 
           
PARAMETERS     rscb_ptr: helps distinguish among multiple RLP sessions
               
DEPENDENCIES   Needs to be called before using the resequencing queue
               The rscb_ptr passed in should be non-NULL

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void dsrlprsq_init
( 
    dsrlpi_sess_ctl_blk_type *rscb_ptr      /* current RLP session ctl blk */
);      


/*===========================================================================

FUNCTION       DSRLPRSQ_RESET

DESCRIPTION    Called when an RLP instance is reset.  This takes any Reseq Q
               entries up until LV(N) and dsm_enqueue() then  into then rx
               watermark structure. From that point on it sets free any other
               remaining dsm item chains. It is not subject to max chain 
               count limitations as  the dsrlprsq_remove() may be.

PARAMETERS     rscb_ptr: pointer to session control block containing RSQ
               
DEPENDENCIES   rscb_ptr != NULL

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void dsrlprsq_reset
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr         /* current RLP ctl blk        */
);



/*===========================================================================

FUNCTION       DSRLPRSQ_INSERT

DESCRIPTION    Insert this new entry into the ReSeq Queue. Inserted based on
               sequence number. It also takes care of the cases there is 
               overlap or partial overlap in the sequence number ranges.
               
               If entry already exists, then nothing is done.

PARAMETERS     rscb_ptr:       pointer to session control block
               frame_info_ptr: pointer to frame information type
               fr_ptr:         pointer to dsm item to insert           
               
DEPENDENCIES   Inputs rscb_ptr, frame_info_ptr, fr_ptr  are  non-null.
               

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void dsrlprsq_insert
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,      /* curr RLP ctl blk         */
  dsrlpi_curr_fr_info_type     *frame_info_ptr,/* current frame info       */
  dsm_item_type                *fr_ptr         /* the frame to insert      */
);



/*===========================================================================

FUNCTION       DSRLPRSQ_REMOVE

DESCRIPTION    Removes as many Resesquencing Q entries as possible and 
               places then in the rx watermark structure. It is limited
               by max_chain_cnt. It will not check for data continuity
               from dsm item chain to dsm item chain in the Q but it will
               make sure that the first sequence number of a dsm item chain
               is less than LV(N) (start of the first hole) before removing
               from the Q.

PARAMETERS     rscb_ptr : pointer to the session control block.

               max_chain_cnt: max number of dsm item chains to be removed at
               once. Time sensitive applications need to use a smaller 
               number for this, as dsm_enqueue() operations into the rx 
               watermark structure take some time to complete. If called 
               with ZERO then no limit is imposed.

DEPENDENCIES   Input ptrs are non-null.
                              
RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void dsrlprsq_remove
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,      /* curr RLP ctl blk         */
  uint32                        max_chain_cnt  /* first missing seq num    */
);
#endif /* FEATURE_DS */
#endif /* DSRLPRSQ_H */


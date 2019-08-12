#ifndef DSRLPNAK_H
#define DSRLPNAK_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      R A D I O   L I N K   P R O T O C O L  N A K  P R O C E S S I N G

GENERAL DESCRIPTION
  This file contains 1x nak-list processing functions.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  RLP configuration and initialization procedures.

 Copyright (c) 1999 - 2016 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsrlpnak.h_v   1.2   12 Feb 2003 17:35:52   kdevara  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/1xrlp/src/dsrlpnak.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        -------------------------------------------------------
01/02/12   msh        Coan: Feature cleanup
09/28/05   ksu/sy     HDR: use one-shot abort timer instead of 20ms ticks
02/12/03   kvd        Added new function prototype, dsrlpnak_get_freenak_count()
08/07/02   vr         Added functgion dsrlp_get_all_nak for HDR
01/07/00   ak         Fixed _insert and _remove to return boolean values.
08/05/99 na/hcg/ak    Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"

#ifdef FEATURE_DS

#include "dsrlp_v.h"
#include "dsrlpi.h"


/*===========================================================================

             PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                       MACRO DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/

/*===========================================================================

FUNCTION       DSRLPNAK_GET_FIRST_SEQ

DESCRIPTION    Returns the first seq for a nak entry.

               PARAMS:  nak_id = id returned by DSRLPNAK_GET_NAKS.

DEPENDENCIES   Only called by the TX TASK
               
RETURN VALUE   The first_seq in the nak entry.

SIDE EFFECTS   NONE
===========================================================================*/
#define DSRLPNAK_GET_FIRST_SEQ(nak_id) \
       (((dsrlpi_nak_list_entry_type *)nak_id)->first_seq)

/*===========================================================================

FUNCTION       DSRLPNAK_GET_LAST_SEQ

DESCRIPTION    Returns the last seq for a nak entry.

               PARAMS:  nak_id = id returned by DSRLPNAK_GET_NAKS.

DEPENDENCIES   Only called by the TX TASK
               
RETURN VALUE   The last_seq in the nak entry.

SIDE EFFECTS   NONE
===========================================================================*/
#define DSRLPNAK_GET_LAST_SEQ(nak_id) \
       (((dsrlpi_nak_list_entry_type *)nak_id)->last_seq)


/*===========================================================================

MACRO          DSRLPNAK_GET_NEXT_ENTRY

DESCRIPTION    Gets the next entry from the nak list, given the current
               entry.  This is here so the code in the function is
               readable.
               
               2 params:   q_ptr     = ptr to queue
                           entry_ptr = ptr to current entry
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
#define DSRLPNAK_GET_NEXT_ENTRY(q_ptr,entry_ptr) \
     ((dsrlpi_nak_list_entry_type *)q_next(q_ptr,&(entry_ptr->link)))



/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                      FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/

/*===========================================================================

FUNCTION       DSRLPNAK_INIT

DESCRIPTION    Called at initialization, this takes all nak entries from
               the array and puts them onto the free queue.
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void dsrlpnak_init(void);

/*===========================================================================

FUNCTION       DSRLPNAK_RESET

DESCRIPTION    Called when an RP instance is reset.  This takes any nak list
               entries from that RLP instance and puts them onto the 
               global free queue.
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void dsrlpnak_reset
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr         /* current RLP ctl blk        */
);


/*===========================================================================

FUNCTION       DSRLPNAK_AGE_LIST

DESCRIPTION    Go through the list and age the nak list entries.  If any
               of the timers have expired, then set a flag so that the
               RLP Tx routine knows to send more naks out.
               
DEPENDENCIES   rscb_ptr != NULL

RETURN VALUE   TRUE if V(N) has changed.

SIDE EFFECTS   NONE
===========================================================================*/
boolean dsrlpnak_age_list
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr         /* current RLP ctl blk        */
);

/*===========================================================================

FUNCTION       DSRLPNAK_INSERT

DESCRIPTION    Insert this new entry into the nak list.  Inserted according
               to sequence number.  Nak entry is filled out with first nak
               round information.
               
               If entry already exists, then nothing is done.
               
               Insert(a,b):  a is the first missing frame, inclusive.
               b is the 32-bit sequence number of the frame just received,
               as calculated by frame determination.  This means that the
               second number is exclusive to the set of missing frames.
               
               Possible insertions are:
               
               Insert            Results in these being inserted
               3  -> 5                3 -> 5
               
               3  -> 3f               3 -> 3f
               
               3  -> 4a               3 -> 4, 4 -> 4a
               
               3a -> 4                3a -> 4
               
               3a -> 4g               3a -> 4, 4 -> 4g
               
               3a -> 6b               3a -> 4, 4 -> 6, 6 -> 6b
               
               3  -> 6b               3 -> 6, 6 -> 6b
               
               This maintains the rule that if segmented frames are being
               NAK'd, then an entry contains the NAK info for the bytes
               within one 8-bit SEQ space.
               
DEPENDENCIES   Input rscb_ptr is non-null.

RETURN VALUE   TRUE if the information could be inserted, else FALSE.

SIDE EFFECTS   NONE
===========================================================================*/
boolean dsrlpnak_insert
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,      /* curr RLP ctl blk         */
  uint32                        first_num,     /* first missing seq num    */
  uint32                        last_num       /* last missing seq num     */
);

/*===========================================================================

FUNCTION       DSRLPNAK_REMOVE

DESCRIPTION    Removes a nak entry from the nak list.

DEPENDENCIES   Input ptrs are non-null.
               
               This can only be called by the RX task.  Must not be called
               by any other task, as then mutual exclusion issues will
               arise.  Assumed that TX task has higher priority than the
               RX task.
               
               nak_id must be the value returned from dsrlpnak_get_seq_hi,
               which returned TRUE.
               
RETURN VALUE   TRUE if the remove was successful.  TRUE is returned even
               when the nak item was not in the list.  FALSE is
               returned when there was an internal memory error with
               allocating nak items.  This indicates the calling fcn
               should reset RLP.

SIDE EFFECTS   V(N) may be updated.
===========================================================================*/
boolean dsrlpnak_remove
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,      /* curr RLP ctl blk         */
  dsrlpi_curr_fr_info_type     *fr_info_ptr,   /* ptr to curr frame info   */
  word                          length,        /* byte-length of data      */
  uint32                        nak_id         /* ref pt from get_seq_hi   */
);

/*===========================================================================

FUNCTION       DSRLPNAK_NAK_WAS_SENT

DESCRIPTION    Tells the nak list if a particular nak was sent over the air.
               Results in decrementing the naks_left counter.  In the case
               where naks_left counter == 0, nothing is done.  The nak_entry
               is "reset" in the Rx Task, in the dsrlpnak_age_list function.
               This scheme prevents any mutual exclusion issues between the
               RX and TX task. 

               The input paramenter is the id given by DSRLP_NAK_GET_NAKS().

DEPENDENCIES   Input ptrs are non-null.
               
               This can only be called by the TX task.  Must not be called
               by any other task, as then mutual exclusion issues will
               arise.  Assumed that TX task has higher priority than the
               RX task.
               
RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void dsrlpnak_nak_was_sent
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,      /* curr RLP ctl blk         */
  uint32                        nak_id         /* nak id of the nak that   */
                                               /* was sent                 */
);

/*===========================================================================

FUNCTION       DSRLPNAK_GET_NAK

DESCRIPTION    Returns an id for the next nak entry which requires a NAK to
               be sent (i.e, age == 0, and naks_left > 0).
               
               The entry is returned as a uint32's.  The TX task is assumed
               to only use functions/macros found in dsrlpnak.c/.h to
               access nak entry info.  As such, it is not important for the
               calling task to know what type the nak_id is.  
               
               Input requires the user to input "curr_nak_id".  If this
               value is zero, then the very first nak entry (requiring
               nak transmission) is returned.  Else, this id refers to a
               nak list entry; in this case the id of nak entry after the
               referenced entry is returned.

DEPENDENCIES   Input ptrs are non-null.
               
               This can only be called by the TX task.  Must not be called
               by any other task, as then mutual exclusion issues will
               arise.  Assumed that TX task has higher priority than the
               RX task.  
               
RETURN VALUE   An id for the next nak entry which requires a nak xmission.
               If the return value is zero, then there are no more nak
               entries requiring transmission.

SIDE EFFECTS   NONE
===========================================================================*/
uint32 dsrlpnak_get_nak
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,      /* curr RLP ctl blk         */
  uint32                        curr_nak_id,   /* current nak id           */
  boolean                       segmented      /* TRUE = next segmented    */
);

/*===========================================================================

FUNCTION       DSRLPNAK_GET_SEQ_HI

DESCRIPTION    Given an input sequence number (with unknown SEQ_HI), 
               finds the SEQ_HI for that number.
               
               The fr_info_ptr's seq_num is updated with the found seq
               number (if it is found).  The input nak_id is updated with
               a reference point, so that if dsrlpnak_remove is called
               with it, it can be more efficient.

DEPENDENCIES   Input ptrs are non-null.
               
               This can only be called by the RX task.  Must not be called
               by any other task, as then mutual exclusion issues will
               arise.  Assumed that TX task has higher priority than the
               RX task.
               
RETURN VALUE   TRUE if a SEQ_HI could be found, else FALSE.  If the input
               fr_info_ptr already contains a sequence number with a known 
               SEQ_HI, then still return TRUE.

SIDE EFFECTS   V(N) may be updated.
===========================================================================*/
boolean dsrlpnak_get_seq_hi
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,      /* curr RLP ctl blk         */
  dsrlpi_curr_fr_info_type     *fr_info_ptr,   /* ptr to curr frame info   */
  word                          length,        /* byte-length of data      */
  uint32                       *nak_id         /* filled with ref pt       */
);
#endif /* FEATURE_DS */

/*===========================================================================

FUNCTION       DSRLPNAK_GET_FREENAK_COUNT

DESCRIPTION    returns the no. of items in the nak_free Q. This number is
               useful to print in a debug message at the end of rlp cleanup
               to detect nak item leaks

DEPENDENCIES   None

RETURN VALUE   returns the no. if items in nak_free Q

SIDE EFFECTS   None
===========================================================================*/

uint8 dsrlpnak_get_freenak_count(void);

/*===========================================================================

FUNCTION       DSRLPNAKI_NEXT_NAK_RND

DESCRIPTION    Update the current nak entry to the next round's nak info.
               If this is already the last round, then may result in the
               entry being removed from the list.
               
DEPENDENCIES   both input ptrs are non-NULL.

RETURN VALUE   TRUE  - If the NAK entry was aborted
               FALSE - If the NAK entry is still valid

SIDE EFFECTS   The nak_entry_ptr can be deleted from the nak list.
===========================================================================*/
boolean rlpnaki_next_nak_rnd
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,      /* curr RLP ctl blk         */
  dsrlpi_nak_list_entry_type   *nak_entry_ptr  /* nak list entry           */
);
#endif /* DSRLPNAK_H */


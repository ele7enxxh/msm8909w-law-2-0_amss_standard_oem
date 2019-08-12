#ifndef HDRRLPNAK_H
#define HDRRLPNAK_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      R A D I O   L I N K   P R O T O C O L  N A K  P R O C E S S I N G

GENERAL DESCRIPTION
  This file contains HDR nak-list processing functions.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  RLP configuration and initialization procedures.

 Copyright (c) 2005-2012 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/hdrrlpnak.h_v   1.2   12 Feb 2003 17:35:52   kdevara  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrrlpnak.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        -------------------------------------------------------
09/12/12   sd         Memory Reduction modification
09/04/09   ss         CMI SU level modifications.
05/08/07   da         Removed function hdrrlpnak_remove as its no longer used
06/06/06   ksu        EMPA: RLP Rx / Sig code review comments
11/28/05   ksu        CR 81596: Rev 0 data call fails all the time with dsrlpsq
                      crash during bi-directional data transfer
11/17/05   ksu        fix reentrancy problem in hdrrlpnak_age_list()
11/15/05   ksu        EMPA: packet stream, octet sequencing (forward link also)
09/28/05   ksu        HDR: use one-shot abort timer instead of 20ms ticks

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"

#ifdef FEATURE_HDR

#include "dsrlpi.h"
#include "hdrrlp_api.h"

/*===========================================================================

             PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                       MACRO DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                      FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/


/*===========================================================================

FUNCTION       HDRRLPNAK_RESET

DESCRIPTION    Called when an RP instance is reset.  This takes any nak list
               entries from that RLP instance and puts them onto the 
               global free queue.
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
extern void hdrrlpnak_reset
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr         /* current RLP ctl blk        */
);

/*===========================================================================

FUNCTION       HDRRLPNAK_INSERT

DESCRIPTION    Insert this new entry into the nak list.  Inserted according
               to sequence number.  Nak entry is filled out with first nak
               round information.
               
               If entry already exists, then nothing is done.
               
DEPENDENCIES   Input rrcb_ptr is non-null.

RETURN VALUE   TRUE if the information could be inserted, else FALSE.

SIDE EFFECTS   NONE
===========================================================================*/
extern boolean hdrrlpnak_insert
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,      /* curr RLP ctl blk         */
  uint32                        first_num,     /* first missing seq num    */
  uint32                        last_num       /* last missing seq num     */
);

/*===========================================================================

FUNCTION       HDRRLPNAK_INSERT_ABORT

DESCRIPTION    Insert this new entry into the nak list.  Inserted according
               to sequence number. This marks the entry as nak_sent and is
               used only as a nak-abort node. To be used when 
               we need to start abort timer for missing bytes without sending 
               a nak message. eg. Flush msg processing in MMPA.

               If entry already exists, then nothing is done.
               
               Insert(a,b):  a is the first missing frame, inclusive.
               b is the sequence number of the frame just received.
               
DEPENDENCIES   Input rscb_ptr is non-null.

RETURN VALUE   TRUE if the information could be inserted, else FALSE.

SIDE EFFECTS   NONE
===========================================================================*/
extern boolean hdrrlpnak_insert_abort
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,      /* curr RLP ctl blk         */
  uint32                        first_num,     /* first missing seq num    */
  uint32                        last_num       /* last missing seq num     */
);

/*===========================================================================

FUNCTION       HDRRLPNAK_NAK_WAS_SENT

DESCRIPTION    Tells the nak list if a particular nak was sent over the air.

               The input paramenter is the id given by DSRLP_NAK_GET_NAKS().

DEPENDENCIES   Input ptrs are non-null.
               
               This can only be called by the TX task.  Must not be called
               by any other task, as then mutual exclusion issues will
               arise.  Assumed that TX task has higher priority than the
               RX task.
               
RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
extern void hdrrlpnak_nak_was_sent
(
  const dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,      /* curr RLP ctl blk         */
  uint32                        nak_id         /* nak id of the nak that   */
                                               /* was sent                 */
);

/*===========================================================================

FUNCTION       HDRRLPNAK_GET_NEXT

DESCRIPTION    Returns an id for the next nak entry which requires a NAK to
               be sent.
               
               The entry is returned as a uint32's.  The TX task is assumed
               to only use functions/macros found in hdrrlpnak.c/.h to
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
extern uint32 hdrrlpnak_get_next
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,      /* curr RLP ctl blk         */
  uint32                        curr_nak_id,   /* current nak id           */
  uint32                       *first_seq_ptr, /* first seq num            */
  uint32                       *last_seq_ptr   /* last seq num             */
);

/*===========================================================================

FUNCTION       HDRRLPNAK_GET_ENTRY

DESCRIPTION    Given an input sequence number, finds the SEQ for that number.
               
               The seq_num_ptr is updated with the found seq
               number (if it is found).  The input nak_id is updated with
               a reference point, so that if hdrrlpnak_remove is called
               with it, it can be more efficient.

DEPENDENCIES   Input ptrs are non-null.
               
               This can only be called by the RX task.  Must not be called
               by any other task, as then mutual exclusion issues will
               arise.  Assumed that TX task has higher priority than the
               RX task.
               
RETURN VALUE   TRUE if a seq_num could be found, else FALSE.  If the input
               seq_num_ptr already contains a sequence number with a known 
               seq_num, then still return TRUE.

SIDE EFFECTS   V(N) may be updated.
===========================================================================*/
extern boolean hdrrlpnak_get_entry
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,      /* curr RLP ctl blk         */
  uint32                        seq_num,       /* current sequence number  */
  word                          length,        /* byte-length of data      */
  uint32                       *nak_id         /* filled with ref pt       */
);

#endif /* FEATURE_HDR */

#endif /* HDRRLPNAK_H */


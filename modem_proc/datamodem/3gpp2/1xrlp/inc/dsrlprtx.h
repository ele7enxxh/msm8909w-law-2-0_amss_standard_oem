#ifndef _DSRLPRTX_H
#define _DSRLPRTX_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                   R A D I O   L I N K   P R O T O C O L
              R E X M I T   Q U E U E   P R O C E S S I N G

GENERAL DESCRIPTION
  This file contains generic Re-Transmit Queue processing functions. The 
  Re-Transmit Queue is a queue that holds structures which define the
  frames/parts of frames that the other side has NAK'd.
  
  The re-transmit queue is not  specific to any one RLP type, but is a
  general purpose data structure.
  
  It is assumed that the user will not directly access any retxq information
  contained in the retxq structure.  Instead, all retxq access can be done
  via the externalized functions or macros provided in this file and the
  rlpretxq.h file.

  The retxq is a doubly-linked list, where each element is an element from
  an array of re-txq structs.  The prev and next pointers are really just
  indices into the array.  

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  rlprtxq_init() must be called before any other RETXQ functions can be
  called. 
  
  rlprtxq_init_ptr() must be called when an RLP session needs to set up
  its state variables.  This must be called after rlprtxq_init, but before
  any rtxq other functions.
  
DESCRIPTION
  

 Copyright (c) 1999 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/COMMON/vcs/dsrlprtx.h_v   1.0   18 Oct 2000 18:07:02   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/1xrlp/inc/dsrlprtx.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        -------------------------------------------------------
09/14/05   sy         Changed DSRLPRTX_HAS_REXMITS macro to 
                      support EV DO/1X .
07/27/05   sy         DSRLPRTX_HAS_REXMITS is changed to use
                      local_rtxq.
04/29/05   tc         Added code changes for DO Rev A .
05/02/00   na         Modified prototype for dsrlp_put_rexmit_info()
00/07/00   ak         Removed extraneous page breaks.
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
#include "dsm.h"


/*===========================================================================

             PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
#define   DSRLPRTX_NO_Q            0     /* indicates no items in rtx q    */




/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                       MACRO/INLINE FUNCTION
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/
/*===========================================================================
 DSRLPPRTX_HAS_REXMIT Macro has changed because earlier the condtion check 
 was simple but with DO Rev A code changes the Macro has to be changed into
 a function. 
===========================================================================*/
#define DSRLPRTX_HAS_REXMITS(rrcb_ptr)  dsrlprtx_has_rexmits(rrcb_ptr)


/*===========================================================================

FUNCTION       DSRLPRTX_HAS_REXMITS

DESCRIPTION    Returns TRUE if there are naks to be transmitted.

PARAMETERS     A pointer to the RLP session control block.
                                           
DEPENDENCIES   NONE

RETURN VALUE   TRUE if naks to xmit, else FALSE

SIDE EFFECTS   None.

===========================================================================*/

static __inline boolean dsrlprtx_has_rexmits(dsrlpi_rlp_ctl_blk_type *rrcb_ptr)
{
  if(rrcb_ptr->cfg_ptr->ver == DSRLP_VER_1X_3)
  {
    if(rrcb_ptr->state.rtxq != DSRLPRTX_NO_Q)
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }                                                               
  
  return FALSE;
}

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                      FUNCTION DECLARATIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/
/*===========================================================================

FUNCTION       DSRLPRTX_INIT

DESCRIPTION    Called at initialization, this takes all rexmit entries from
               the array and puts them onto the free queue.
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void dsrlprtx_init(void);

/*===========================================================================

FUNCTION       DSRLPRTX_RESET

DESCRIPTION    Called when an RLP instance is reset.  This takes any rexmit
               queue entries and puts them back into the global free q.
               
DEPENDENCIES   rrcb_ptr != NULL

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void dsrlprtx_reset
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr         /* current RLP ctl blk        */
);

/*===========================================================================

FUNCTION       DSRLPRTX_INIT_PTR

DESCRIPTION    Called when at the start of an RLP session.  Required to 
               set up the rtx ptr in the rrcb.
               
DEPENDENCIES   rrcb_ptr != NULL

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void dsrlprtx_init_ptr
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr         /* current RLP ctl blk        */
);

/*===========================================================================

FUNCTION       DSRLPRTX_GET_REXMIT_FIRST_SEQ

DESCRIPTION    Called by the Tx task when it needs to send a retransmission.
               Will get the first seq of the first frame requiring re-
               transmsission.
               
DEPENDENCIES   rrcb_ptr != NULL

RETURN VALUE   TRUE if the list is not empty, else returns FALSE.  If it 
               returns TRUE, then the input first_seq is loaded with the
               appropriate value.

SIDE EFFECTS   NONE
===========================================================================*/
boolean dsrlprtx_get_rexmit_first_seq
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,        /* current RLP ctl blk        */
  uint32                   *first_seq        /* will be loaded with 1st seq*/      
);

/*===========================================================================

FUNCTION       DSRLPRTX_GET_REXMIT_LAST_SEQ

DESCRIPTION    Called by the Tx task when it needs to send a retransmission.
               Will get the last seq of the first frame requiring re-
               transmsission.
               
DEPENDENCIES   rrcb_ptr != NULL

RETURN VALUE   TRUE if the list is not empty, else returns FALSE.  If it 
               returns TRUE, then the input last_seq is loaded with the
               appropriate value.

SIDE EFFECTS   NONE
===========================================================================*/
boolean dsrlprtx_get_rexmit_last_seq
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,       /* current RLP ctl blk         */
  uint32                   *last_seq        /* will be loaded with last seq*/      
);

/*===========================================================================

FUNCTION       DSRLPRTX_REMOVE_FIRST_ElEMENT

DESCRIPTION    Called by the Tx task when it has fully re-transmitted the
               frame(s) described by the first element in the rtx queue.
               
               Removes that element from the queue and places it in the
               free queue.
               
DEPENDENCIES   rrcb_ptr != NULL

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void dsrlprtx_remove_first_element
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr         /* current RLP ctl blk        */
);

/*===========================================================================

FUNCTION       DSRLPRTX_REMOVE_LAST_ElEMENT

DESCRIPTION    Called by the Rx task when it finds out that there is a 
               memory shortage in the system.  Then the most recent NAK
               from the peer is dumped.
               
               Removes the newest element from the queue and places it in
               the free queue.
               
DEPENDENCIES   rrcb_ptr != NULL

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void dsrlprtx_remove_last_element
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr         /* current RLP ctl blk        */
);

/*===========================================================================

FUNCTION       DSRLPRTX_UPDATE_FIRST_ElEMENT

DESCRIPTION    Called by the Tx task when it has partially re-transmitted the
               frame(s) described by the first element in the rtx queue.
               This then updates the first/last info in the element so that
               on the next pass, the Tx task can continue to send info 
               described by this element.
               
DEPENDENCIES   rrcb_ptr != NULL

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void dsrlprtx_update_first_element
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,        /* current RLP ctl blk        */
  uint32                    first_seq,       /* new first seq for element  */
  uint32                    last_seq         /* new last seq for element   */
);
/*===========================================================================

FUNCTION       DSRLPRTX_PUT_REXMIT_INFO

DESCRIPTION    Called by the Rx task when an incoming NAK is received.  This
               function creates an entry for the rexmit list.
               
               Items are not sorted; they are always put at the end of the
               list.
               
DEPENDENCIES   rrcb_ptr != NULL

RETURN VALUE   TRUE  - if the element could be added to rexmit list.
               FALSE - if the element could not be added to the rexmit list.

SIDE EFFECTS   NONE
===========================================================================*/
boolean dsrlprtx_put_rexmit_info
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,        /* current RLP ctl blk        */
  uint32                    first_seq,       /* first seq num to xmit      */
  uint32                    last_seq         /* last seq num to xmit       */
);


#endif /* FEATURE_DS */
#endif /* _DSRLPRTX_H */


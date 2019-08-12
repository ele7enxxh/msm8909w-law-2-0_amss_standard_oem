#ifndef _DSDVRLPI_RX_PREQ_H
#define _DSDVRLPI_RX_PREQ_H

/*===========================================================================

  D A T A   S E R V I C E S   R L P   I N T E R N A L   H E A D E R  F I L E

DESCRIPTION
  This file contains internal variable and function declarations specific to 
  the Radio Link Protocol implementation that supports EVDV.

EXTERNALIZED FUNCTIONS
  dsdvrlpi_rx_pre_q_init()
    This function initializes RLP's preQ.

  dsdvrlpi_rx_pre_q_cleanup()
    This function empties and cleans up RLP's preQ.

  dsdvrlpi_rx_pre_q_put()
    This function inserts elements into RLP's preQ.
  
  dsdvrlpi_rx_pre_q_get()
    This function gets and removes next element from RLP's preQ.

  dsdvrlpi_rx_pre_q_get_max()
    This function gets and removes maximum elements possible
    (contiguous, inorder and needed) from RLP's preQ.
   
  dsdvrlpi_rx_pre_q_age_list()
    This function processes RLP's preQ - ages the preQ
    entries, removes the ones expired, etc.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1999-2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/1xrlp/src/dsdvrlpi_rx_preq.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/22/04   atp     After lint.
06/08/04   atp     Support for upper bound on DDW and REXMIT_TIMER.
05/07/04   atp     Initial version. First cut at supporting EVDV (IS707-B).

===========================================================================*/

/*===========================================================================
                      INCLUDE FILES FOR MODULE
===========================================================================*/
#include "customer.h"
#include "comdef.h"

#include "dsm.h"
#include "dsrlpi.h"
#include "queue.h"



/*===========================================================================
                      EXTERNAL DATA DECLARATIONS
===========================================================================*/

/*===========================================================================
                      EXTERNAL FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================

FUNCTION DSDVRLPI_RX_PRE_Q_INIT

DESCRIPTION
  This function initializes RLP's preQ.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsdvrlpi_rx_pre_q_init
( 
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr      /* Ptr to RLP ctl block     */
); 

/*===========================================================================

FUNCTION DSDVRLPI_RX_PRE_Q_CLEANUP

DESCRIPTION
  This function empties and cleans up RLP's preQ.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsdvrlpi_rx_pre_q_cleanup
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr      /* Ptr to RLP ctl block     */
);

/*===========================================================================

FUNCTION DSDVRLPI_RX_PRE_Q_PUT

DESCRIPTION
  This function inserts elements into RLP's preQ.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsdvrlpi_rx_pre_q_put
( 
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,     /* Ptr to RLP ctl block     */
  dsm_item_type *insrt_elem_ptr           /* Ptr to element to be inserted */
); 

/*===========================================================================

FUNCTION DSDVRLPI_RX_PRE_Q_GET

DESCRIPTION
  This function gets and removes next element from RLP's preQ. 
  (No matter whether it's the needed one or not).

DEPENDENCIES
  None

RETURN VALUE
  Ptr to next element

SIDE EFFECTS
  None.
===========================================================================*/
dsm_item_type *dsdvrlpi_rx_pre_q_get
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr         /* Ptr to RLP ctl block      */
);

/*===========================================================================

FUNCTION DSDVRLPI_RX_PRE_Q_GET_MAX

DESCRIPTION
  This function gets and removes as many elements as possible from RLP's
  preQ, provided they are contiguous, in-order and start with the needed
  seq number.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void dsdvrlpi_rx_pre_q_get_max
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr         /* Ptr to RLP ctl block      */
);


/*===========================================================================

FUNCTION DSDVRLPI_RX_PRE_Q_AGE_LIST

DESCRIPTION
  This function processes RLP's preQ - ages the preQ entries, removes the
  ones expired, etc.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  May result in Naks, due to entries being removed from PreQ.
===========================================================================*/
void dsdvrlpi_rx_pre_q_age_list
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,        /* Ptr to RLP ctl block      */
  uint16 num_pkts_in_last_20ms /* Num encoder pkts rxed by mux in last 20ms*/  
);

/*===========================================================================

FUNCTION DSDVRLPI_RX_PRE_Q_PROCESS_ENTRIES_TILL_EXPIRED_ONE

DESCRIPTION
  This function processes  entries in preQ until the one which has actually
  expired (exclusive).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsdvrlpi_rx_pre_q_process_entries_till_expired_one
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,        /* Ptr to RLP ctl block      */
  dsm_item_type            *expired_elem_ptr /* expired elem               */
);

/*===========================================================================

FUNCTION DSDVRLPI_RX_PRE_Q_PROCESS_EXPIRED_ENTRY

DESCRIPTION
  This function processes  entries in preQ whose DDW has expired.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  May result in Naks, due to entries being removed from PreQ.
===========================================================================*/
void dsdvrlpi_rx_pre_q_process_expired_entry
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,        /* Ptr to RLP ctl block      */
  dsm_item_type            *exp_elem_ptr
);

#endif  /* _DSDVRLPI_RX_PREQ_H */


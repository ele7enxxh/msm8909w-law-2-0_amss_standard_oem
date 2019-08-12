#ifndef _HDRMRLPSCHED_H
#define _HDRMRLPSCHED_H

/*===========================================================================
                         H D R M R L P   S C H E D
GENERAL DESCRIPTION
  This file contains the configuration for the scheduling of watermarks 
  belonging to a particular RLP. Each MRLP can bind multiple IP flows,
  each IP flow has one watermark. When the new data needs to get transmitted,
  the RLP should take the data out of these watermarks in a round robin fashion.
  
EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS
   

 Copyright (c) 2005 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrmrlpsched.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when         who        what, where, why
--------     ---        -------------------------------------------------------
07/20/06     kvd        7500 re-arch changes.
12/23/05   ksu        EMPA RSP: two rev watermarks per ip flow
07/06/2005   sy         Initial revision.

===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#ifdef FEATURE_HDR_QOS
#include "dsrlpi.h"

/*===========================================================================
                        EXTERNAL FUNCTIONS
===========================================================================*/

/*===========================================================================

FUNCTION HDRMRLPSCHED_BIND_WM_RLP

DESCRIPTION  Binds a watermark to the given RLP
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void  hdrmrlpsched_bind_wm_rlp
(
  byte rscb_index,
  dsm_watermark_type     *add_wm_ptr,
  byte                    route
);

/*===========================================================================

FUNCTION HDRMRLPSCHED_UNBIND_WM_RLP

DESCRIPTION Unbinds watermark from the RLP.
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/

void hdrmrlpsched_unbind_wm_rlp
(
  byte rscb_index,
   dsm_watermark_type *del_wm_ptr,
   byte                route
);

/*===========================================================================

FUNCTION HDRMRLPSCHED_GET_NEW_DATA_CNT

DESCRIPTION  Dequeues elements from the watermark belonging to the rlp in
             a round robin fashion.
  
DEPENDENCIES
  None.

RETURN VALUE
  Returns a pointer to the dsm item that has been dequeued

SIDE EFFECTS
===========================================================================*/
uint32 hdrmrlpsched_get_new_data_cnt
( 
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr       /* ptr to RLP specific ctl blk  */
);

/*===========================================================================

FUNCTION HDRMRLPSCHED_DEQUEUE

DESCRIPTION  Dequeues elements from the watermark belonging to the rlp in
             a round robin fashion.
  
DEPENDENCIES
  None.

RETURN VALUE
  Returns a pointer to the dsm item that has been dequeued

SIDE EFFECTS
===========================================================================*/

dsm_item_type * hdrmrlpsched_dequeue
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr       /* ptr to RLP specific ctl blk  */
);

/*===========================================================================

FUNCTION HDRMRLPSCHED_STARTUP

DESCRIPTION
  Called to initialize the MRLP scheduler module at powerup  

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/

void hdrmrlpsched_startup(void);

#endif /* FEATURE_HDR_QOS */
#endif /* _HDRMRLPSCHED_H*/

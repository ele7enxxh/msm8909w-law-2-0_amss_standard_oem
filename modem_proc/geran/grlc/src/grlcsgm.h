/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC UPLINK SEGMENT MODULE HEADER FILE
***
***
*** DESCRIPTION
***
***  This module contains definitions of data and functions relating to 
***  segmentation of LL PDU into RLC data blocks
*** 
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlcsgm.h#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 24/11/03    hv     Created
*** 22/04/04    hv     Added EGPRS fn definitions
*****************************************************************************/

#ifndef INC_GRLCSGM_H
#define INC_GRLCSGM_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "grlcutil.h"
#include "grlcultypes.h"
#include "grlcgrp.h"
#include "geran_multi_sim.h"


/* MSC tables defined in grlcsgm.c */
extern const uint8 E_GRLC_MCS_RESEGMENT_SET[9][11];
extern const uint8 E_GRLC_MCS_RESEGMENT_CLR[9][11];


/* Current rlc uplink data block being worked on. This may already contain a 
** number of short LLC pdu(s)
*/
extern grlc_ul_data_block_t   cur_blk[NUM_GERAN_DATA_SPACES];

/*===========================================================================
===
===  FUNCTION      SEGMENT_PDU
===
===  DESCRIPTION
===
===    Segment the current PDU to produce a number of uplink data blocks
===    for transmission. The TLLI field will be included if IS_TLLI_REQUIRED()
===    is tested TRUE.
===
===    Padding of a partially-filled data block (where rem_cnt > 0) is done
===    elsewhere for efficientcy reasons.
===
===  PARAMETERS
===
===    blks_to_segment: maximum number of blocks to segment. The fn returns
===    if 0.
===    
===    cs: coding scheme to apply during segmentation.
===
===  DEPENDENCIES
===
===    inputs: cur_pdu -- PDU being processed, pointed to by 'bp'
===            IS_TLLI_REQUIRED() determines if TLLI is required.
===
===    output: cur_blk
===
===  RETURN VALUE
===
===    segment_status_t
===
===  SIDE EFFECTS
===
===    xxx.
===
=== ! to send off block (for padding and transmission) if the CV is 0.
===
===========================================================================*/
extern segment_status_t  segment_pdu( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      E_SEGMENT_PDU
===
===  DESCRIPTION
===
===    Segment the current PDU to produce a number of uplink data blocks
===    for transmission. The TLLI field will be included if IS_TLLI_REQUIRED()
===    is tested TRUE.
===
===    Padding of a partially-filled data block (where rem_cnt > 0) is done
===    elsewhere for efficientcy reasons.
===
===  PARAMETERS
===    
===
===  DEPENDENCIES
===
===    inputs: cur_pdu -- PDU being processed, pointed to by 'bp'
===            IS_TLLI_REQUIRED() determines if TLLI is required.
===
===    output: cur_blk
===
===  RETURN VALUE
===
===    segment_status_t
===
===  SIDE EFFECTS
===
===========================================================================*/
extern segment_status_t  e_segment_pdu( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      SEGMENT_LOOP
===
===  DESCRIPTION
===
===    Call segment_pdu() again when the length of the 1st PDU is shorter
===    than the capacity of a block. When this happens the 'cur_blk' is partial
===    and un-attached and the next PDU needs to be loaded and segmented
===    to make up enough blocks to transmit with. However, we should only do
===    this when vs == vsf, ie. no more blocks to send to FIFO
===
===  PARAMS
===
===
===  DEPENDENCIES
===
===    ul struct
===  
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    xxx.
===  
===========================================================================*/
extern void segment_loop
(
  gas_id_t  gas_id,
  boolean   b_reprocessing
);
/*===========================================================================
===
===  FUNCTION      GRLC_UL_UNSEGMENT()
===
===  DESCRIPTION
===
===    Free the segmented blocks from BSN to segment index, free the 
===    segmented blocks and adjusted the segment index and segment state
===
===  DEPENDENCIES
===
===  PARAMETERS
===  
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===  
===========================================================================*/
extern void grlc_ul_unsegment (gas_id_t gas_id, uint16 bsn);

/*===========================================================================
===
===  FUNCTION      E_GRLC_UL_UNSEGMENT()
===
===  DESCRIPTION
===
===    Free the segmented blocks from BSN to segment index, free the 
===    segmented blocks and adjusted the segment index and segment state
===
===  DEPENDENCIES
===
===  PARAMETERS
===  
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===  
===========================================================================*/
extern void e_grlc_ul_unsegment ( gas_id_t gas_id, uint16 bsn);

/*===========================================================================
===
===  FUNCTION      RECALC_CV_RELATED_VALUES()
===
===  DESCRIPTION
===
===     Recalculate the total number of blocks etc..during reallocation.
===     These variables effect the current CV value.
===
===
===  PARAMS
===    repro_cause 
===
===  DEPENDENCIES
===
===    Requires ul.octets_rem
===  
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    Alter ul.blocks_rem, ul.abs_bsn, ul.tbc, ul.cv
===  
===========================================================================*/
extern void recalc_cv_related_values
(
  gas_id_t                gas_id,
  grlc_reprocess_cause_t  repro_cause
);

#endif /* INC_GRLCSGM_H */

/*** EOF: don't remove! ***/

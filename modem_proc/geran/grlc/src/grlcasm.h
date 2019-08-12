/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC ASSEMBLY MODULE
***
***
*** DESCRIPTION
***
***  Definitions of data re-assembly functions
***
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlcasm.h#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 03/29/01    hv     Created
***
*****************************************************************************/

#ifndef INC_GRLCASM_H
#define INC_GRLCASM_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */
#include "grlcdl.h"
#include "geran_multi_sim.h"

/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/

/*---------------------------------------------------------
** Current downlink pdu.
** Holds the pdu currently being processed.
** Note the dsm item must be created and held in 'dsm_ptr'
**---------------------------------------------------------
*/
extern grlc_dl_pdu_t  cur_dl_pdu[NUM_GERAN_DATA_SPACES];

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION    GRLC_DL_INIT_LLC_PDU
===
===  DESCRIPTION
===
===     input: cur_dl_pdu of type grlc_dl_pdu_t
===            rlc_mode
===     output: none.
===
===  DEPENDENCIES
===
===  
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_dl_init_llc_pdu
(
  gas_id_t      gas_id,
  grlc_dl_pdu_t *pdu_ptr, 
  const uint8   rlc_mode
);

/*===========================================================================
===
===  FUNCTION      GRLC_DL_ASSEMBLE
===
===  DESCRIPTION
===
===    Assemble the downlink data block in ack mode or un-ack mode into
===    one or more pdus. When a complete pdu is formed it is sent off to
===    LLC and a new pdu is created for the next assembly.
===
===    When a pdu exceeds its 1536 octets limit, it is truncated and sent 
===    off. A flag is set to ensure subsequent data blocks are discarded
===    until one containing a pdu boundary (with ext octet). The data 
===    following the pdu boundary will then be assembled into pdus again.
===
===    This fn should be called when a dl data block is ready in cur_dl_blk[gas_id].
===    In ack mode, another shell fn should call this function.
===
===  PARAMETERS
===
===    blk_ptr: points to an internal DL data block which could be the
===             current dl block or a filled block in un-ack mode or an
===             element of the DL array in ack mode.
===
===  DEPENDENCIES
===
===    cur_dl_pdu: input dl data block
===    cur_dl_pdu: current working pdu which may contain a partial pdu
===    or empty.
===
===    When this function is called cur_dl_blk[gas_id] is expected to hold a data 
===    block whose BSN is contiguous with the one before.
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    b_pdu_truncated
===    
===  
===========================================================================*/
void grlc_dl_assemble
(
  gas_id_t              gas_id,
  grlc_dl_data_block_t  *blk_ptr
);

/*===========================================================================
===
===  FUNCTION      GRLC_DL_E_ASSEMBLE
===
===  DESCRIPTION
===
===    Assemble the downlink data block in ack mode or un-ack mode into
===    one or more pdus. When a complete pdu is formed it is sent off to
===    LLC and a new pdu is created for the next assembly.
===
===    When a pdu exceeds its 1536 octets limit, it is truncated and sent 
===    off. A flag is set to ensure subsequent data blocks are discarded
===    until one containing a pdu boundary (with ext octet). The data 
===    following the pdu boundary will then be assembled into pdus again.
===
===    This fn should be called when a dl data block is ready in cur_dl_blk[gas_id].
===    In ack mode, another shell fn should call this function.
===
===  PARAMETERS
===
===    blk_ptr: points to an internal DL data block which could be the
===             current dl block or a filled block in un-ack mode or an
===             element of the DL array in ack mode.
===
===  DEPENDENCIES
===
===    cur_dl_pdu: input dl data block
===    cur_dl_pdu: current working pdu which may contain a partial pdu
===    or empty.
===
===    When this function is called cur_dl_blk[gas_id] is expected to hold a data 
===    block whose BSN is contiguous with the one before.
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    b_pdu_truncated
===    
===  
===========================================================================*/
void grlc_dl_e_assemble
(
  gas_id_t              gas_id,
  grlc_dl_data_block_t  *blk_ptr
);


#endif /* INC_GRLCASM_H */

/*** EOF: don't remove! ***/

/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC GROUP
***
***
*** DESCRIPTION
***
***  This module contains definitions supporting PDU grouping operations
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlcgrp.h#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 11/24/03    hv     Created
***
*****************************************************************************/

#ifndef INC_GRLCGRP_H
#define INC_GRLCGRP_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "grlci.h"
#include "grlcutil.h"
#include "grlcultypes.h"
#include "grlcultst.h"
#include "grlculproc.h"


/* Data structure to manage grouping of PDUs
*/
extern grlc_ul_pdu_group_t   pg[NUM_GERAN_DATA_SPACES];

/*===========================================================================
===
===  FUNCTION      GET_NUM_BLOCKS()
===
===  DESCRIPTION
===
===   Return the number of data blocks required to hold the specified number
===   of octets given the coding scheme, TLLI indicator. Partial block is
===   counted as one whole block and extension octet is accounted for.
===
===  DEPENDENCIES
===
===   RLC_DATA_BLOCK_SIZE[]
===   RM_TLLI_LEN
===  
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    also returns the number of remanining octets in the last data block
===
===========================================================================*/
uint16 get_num_blks 
(
  gas_id_t gas_id,
  uint16  octets,
  uint8   cs,
  uint8   tlli_cs,
  uint8   ti,
  uint16  b_next_pdu_avail
);

/*===========================================================================
===
===  FUNCTION      E_GET_NUM_BLOCKS()
===
===  DESCRIPTION
===
===   Return the number of data blocks required to hold the specified number
===   of octets given the MCS, TLLI indicator. Partial block is
===   counted as one whole block and extension octet is accounted for.
===
===  DEPENDENCIES
===  
===  RETURN VALUE
===
===    Number of blocks
===
===  SIDE EFFECTS
===
===    also returns the number of remanining octets in the last data block
===
===========================================================================*/
uint16 e_get_num_blks 
(
  gas_id_t  gas_id,
  uint16  octets,
  uint8   mcs,
  uint8   tlli_cs,
  uint8   ti
);

/*===========================================================================
===
===  FUNCTION      GET_NUM_BLKS_REQ()
===
===  DESCRIPTION
===
===   Calculate the number of data blocks given coding scheme, tlli indicator,
===   and the number of octets  for GPRS and EGPRS. 
===
===   Return Extra params such as the number of partial octets, the pad octets 
===   and the b_ext_octet flag to allow precise calculation of the extra blocks 
===   required when reallocating.
===
===  PARAMETERS
===
===   INPUT
===
===   octets: number of octets to store in the blocks
===   b_next_pdu_avail: when TRUE indicates there is a next PDU to follow
===   
===   OUTPUT
===
===   ul.num_blks: total number of blocks required to hold the specified 
===     octets given the block size
===
===   ul.b_ext_octet: where a perfect fit is in force this flag indicates
===     whether the an extension octet is already present and accounted for
===     in the final data block.
===
===   ul.partial_octets: number of (data) octets in the (last) partial block
===
===   ul.pad_octets: number of octets remained free in the partial block 
===     This value may or may not already account for any extension octet.
===
===  DEPENDENCIES
===  
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
void get_num_blks_req
(
  gas_id_t  gas_id,
  uint16  octets,
  uint8   ti,
  uint16  b_next_pdu_avail
);
/*===========================================================================
===
===  FUNCTION      GET_NUM_BLKS_EX()
===
===  DESCRIPTION
===
===   Calculate the number of data blocks given coding scheme, tlli indicator,
===   and the number of octets as per get_num_blks(). 
===
===   Return Extra params such as the number of partial octets, the pad octets 
===   and the b_ext_octet flag to allow precise calculation of the extra blocks 
===   required when reallocating.
===
===  PARAMETERS
===
===   INPUT
===
===   octets: number of octets to store in the blocks
===   cs,ti:  coding scheme and TLLI indicator to determine the block size
===   b_next_pdu_avail: when TRUE indicates there is a next PDU to follow
===   
===   OUTPUT
===
===   ul.num_blks: total number of blocks required to hold the specified 
===     octets given the block size
===
===   ul.b_ext_octet: where a perfect fit is in force this flag indicates
===     whether the an extension octet is already present and accounted for
===     in the final data block.
===
===   ul.partial_octets: number of (data) octets in the (last) partial block
===
===   ul.pad_octets: number of octets remained free in the partial block 
===     This value may or may not already account for any extension octet.
===
===  DEPENDENCIES
===
===   RLC_DATA_BLOCK_SIZE[]
===   RM_RLC_HDR_LEN
===   RM_TLLI_LEN
===  
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    also returns the number of remanining octets in the last data block
===
===========================================================================*/
void get_num_blks_ex 
(
  gas_id_t  gas_id,
  uint16  octets,
  uint8   cs,
  uint8   tlli_cs,
  uint8   ti,
  uint16  b_next_pdu_avail
);

void e_get_num_blks_ex 
(
  gas_id_t  gas_id,
  uint16  octets,
  uint8   cs,
  uint8   tlli_cs,
  uint8   ti,
  uint16  b_next_pdu_avail
);

/*===========================================================================
===
===  FUNCTION      PG_CLR_TBC_INFO()
===
===  DESCRIPTION
===
===   This function clears the PDU group info struct to 0. This structure holds
===   the current partial octets, pad octets, extention octet etc.. of the 
===   current PDU group. If a new group starts this structure should be cleared
===
===  PARAMETERS
===
===   tbci_ptr: pointer to 'total-block-count-info' struct
===
===   OUTPUT
===
===   tbci_ptr: returns the altered content
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
void pg_clr_tbc_info (gas_id_t gas_id, grlc_ul_tbc_info_t *tbci_ptr);

/*===========================================================================
===
===  FUNCTION      PG_CLR_GROUP_INFO()
===
===  DESCRIPTION
===
===   Inititialise PDU-group information structure
===
===  PARAMETERS
===
===   INPUT
===
===
===   OUTPUT
===
===  DEPENDENCIES
===
===  
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
void pg_clr_group_info ( gprs_grr_ul_mes_t*  pdu_ptr );

/*===========================================================================
===
===  FUNCTION      PG_XFER_TO_TBC()
===
===  DESCRIPTION
===
===   This function takes the number of PDUs in the PDU group and calculates
===   the number of blocks required to hold these octets and store the value in
===   tbci.num_blks field. This value is to load into TBC
===
===   This function should only be called when ul.tbc is 0. The pg_octets field
===   not actually used. Instead the current PDU is first added to the TBC-info
===   structure of the PDU group. The next PDU in the queue is added one by one
===   to this structure. This is to make sure the 
===
===  PARAMETERS
===
===   INPUT
===
===
===   OUTPUT
===
===  DEPENDENCIES
===
===  
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    also returns the number of remanining octets in the last data block
===
===========================================================================*/
void pg_calc_xfer_value ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      E_PG_XFER_TO_TBC()
===
===  DESCRIPTION
===
===   This function takes the number of PDUs in the PDU group and calculates
===   the number of blocks required to hold these octets and store the value in
===   tbci.num_blks field. This value is to load into TBC
===
===   This function should only be called when ul.tbc is 0. The pg_octets field
===   not actually used. Instead the current PDU is first added to the TBC-info
===   structure of the PDU group. The next PDU in the queue is added one by one
===   to this structure. This is to make sure the 
===
===  PARAMETERS
===
===   INPUT
===
===
===   OUTPUT
===
===  DEPENDENCIES
===
===  
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    also returns the number of remanining octets in the last data block
===
===========================================================================*/
void e_pg_calc_xfer_value ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      GROUP_PDU_INIT()
===
===  DESCRIPTION
===
===    PDU is being inserted in the queue for the first time so put a PSN 
===    value on the PDU and set its grouping state to PGS_INIT.
===
===    This function should be called in enqueue_pdu()
===
===  PARAMS
===
===    in_pdu_ptr:  pointer to incoming PDU
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    none
===   
===  SIDE EFFECTS
===
===    ul.llc_group_pdus, ul.llc_group_octets
===  
===========================================================================*/
void group_pdu_init (gas_id_t gas_id, gprs_grr_ul_mes_t* pdu_ptr);

/*===========================================================================
===
===  FUNCTION      GROUP_PDUS()
===
===  DESCRIPTION
===
===    Search the PDU queue to find a next PDU, if one exists, and compare
===    its radio priority and peak-throughput class with the current PDU group.
===    If they match, then increment the number of PDUs in the group, the total
===    group octets count and update the group status. If the next PDU type
===    does not match the current type then terminate the current PDU group.
===
===  PARAMS
===
===    in_pdu_ptr:  pointer to incoming PDU
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    none
===   
===  SIDE EFFECTS
===
===    ul.llc_group_pdus, ul.llc_group_octets
===  
===========================================================================*/
void group_pdus ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      UNGROUP_PDU()
===
===  DESCRIPTION
===
===    This function is used when the current PDU runs out and the next PDU is 
===    picked up. This function transfer the PDU Grouping status and counters
===    to the next PDU if one exists. It also subtracts the PDU count and 
===    octets count of the finished (current PDU) from the record.
===
===  PARAMS
===
===    pdu_ptr: points to the PDU to be taken from the PDU group.
===             Normally this will be 'cur_pdu_ptr'
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void ungroup_pdu ( gas_id_t gas_id, gprs_grr_ul_mes_t*  pdu_ptr );

/*===========================================================================
===
===  FUNCTION      GROUP_PDUS_FOR_REPROCESSING()
===
===  DESCRIPTION
===
===    This function performs grouping of PDUs without considering reallocation
===    actions
===
===  PARAMS
===
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    none
===   
===  SIDE EFFECTS
===
===    ul.llc_group_pdus, ul.llc_group_octets
===  
===========================================================================*/
void group_pdus_for_reprocessing ( gas_id_t gas_id );

#endif /* INC_GRLCGRP_H */

/*** EOF: don't remove! ***/

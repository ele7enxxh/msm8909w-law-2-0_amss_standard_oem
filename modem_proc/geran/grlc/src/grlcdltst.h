/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC DOWNLINK HOST TEST HEADER FILE
***
***
*** DESCRIPTION
***
***  This module contains definitions of functions that support the host test
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlcdltst.h#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 11/24/03    hv     Created
***
*****************************************************************************/

#ifndef INC_GRLCDLTST_H
#define INC_GRLCDLTST_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */


/*===========================================================================
===
===  FUNCTION      SHOW_DL_RBB()
===
===  DESCRIPTION
===
===    Show SSN and bitmap of PDAN
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
void show_dl_rbb (gas_id_t gas_id, const rm_acknack_desc_t *desc_ptr, uint8 fai);

/*===========================================================================
===
===  FUNCTION      SHOW_DL_VARS()
===
===  DESCRIPTION
===
===    Show values of internal variables of Downlink State Machine
===
===  DEPENDENCIES
===    
===    dl
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
void show_dl_vars ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      SHOW_EPDAN()
===
===  DESCRIPTION
===
===    Show the values of the current EDGE Packet Downlink Acknowledgement Message
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
void show_epdan (gas_id_t gas_id, const rm_dl_pkt_acknack_req_t  *dl_acknack_ptr);

#ifdef  ERLD_SHOW_BLOCK
/*===========================================================================
===
===  FUNCTION      SHOW_DL_BLOCK()
===
===  DESCRIPTION
===
===    Show the values of the EDGE data block
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
void show_dl_block
(
  gas_id_t              gas_id,
  e_pl1_radio_block_t   *radio_blk_ptr,
  grlc_dl_data_block_t  *data_blk_1_ptr,
  grlc_dl_data_block_t  *data_blk_2_ptr
);
#endif  /* ERLD_SHOW_BLOCK */

#ifdef ERLD_DUMP_12BYTES_PLD
/*===========================================================================
===
===  FUNCTION      E_DUMP_DL_PLD()
===
===  DESCRIPTION
===
===   Dumps the first 12 octets of the dl payloads in hex format in a F3 message.
===
===  DEPENDENCIES
===
===   None.
===  
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===   None.
===  
===========================================================================*/
void e_dump_dl_pld( void );
#endif /* ERLD_DUMP_12BYTES_PLD  */

/*===========================================================================
===
===  FUNCTION      GRLC_DIAG_SHOW_DL()
===
===  DESCRIPTION
===
===    This function tests the set of Diag functions provided by RLC DL to
===    the Diag module
===
===  DEPENDENCIES
===  
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_diag_show_dl (void);

/*===========================================================================
===
===  FUNCTION      RLC_GET_PL1_RADIO_BLOCK_SIZE()
===
===  DESCRIPTION
===
===    Return the size of the dl data block given the coding scheme
===    the Diag module
===
===  DEPENDENCIES
===  
===  RETURN VALUE
===
===    size of dl data block in octets
===
===  SIDE EFFECTS
===
===========================================================================*/
uint16 rlc_get_pl1_radio_block_size (uint8 cs);

/*===========================================================================
===
===  FUNCTION      RLC_DL_SHOW_STATE_VARS()
===
===  DESCRIPTION
===
===    Show the current dl state and event
===
===  DEPENDENCIES
===  
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void rlc_dl_show_state_vars(void);

/*===========================================================================
===
===  FUNCTION      RLC_DL_SHOW_RX_VARS()
===
===  DESCRIPTION
===
===    Shows 
===
===  DEPENDENCIES
===  
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void rlc_dl_show_rx_vars (void);

/*===========================================================================
===
===  FUNCTION      RLC_DL_CHECK_RBB()
===
===  DESCRIPTION
===
===    Return TRUE if the 'rbb' array contains a bit representing the given
===    'bsn' and 'ssn' values
===
===  DEPENDENCIES
===  
===  RETURN VALUE
===
===    boolean
===
===  SIDE EFFECTS
===
===========================================================================*/
boolean rlc_dl_check_rbb (uint8 *rbb, uint8 bsn, uint8 ssn);

/*===========================================================================
===
===  FUNCTION      RLC_DL_FORCE_RESET()
===
===  DESCRIPTION
===
===    Force a dl reset and clear the dl fifo
===
===  DEPENDENCIES
===  
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void rlc_dl_force_reset (void);

/*===========================================================================
===
===  FUNCTION      GRLC_DL_INC_MISSYNC_BLK_CNT()
===
===  DESCRIPTION
===
===    Return the number of number of data blocks received with mis-synced 
===    (un-expected) pdu number during back-to-back test
===
===  DEPENDENCIES
===  
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_dl_inc_missync_blk_cnt (void);

extern void grlc_dl_diag_pdu_stat_add_entry(uint32 pdu_num, grlc_dl_pdu_t *ptr, const gas_id_t gas_id);
extern grlc_diag_dl_pdu_t* grlc_dl_diag_pdu_stat_remove_entry(const gas_id_t gas_id);
extern uint8 grlc_dl_diag_pdu_stat_get_count(const gas_id_t gas_id);
extern void grlc_dl_diag_rx_stat_add_entry(grlc_dl_data_block_t *ptr, const gas_id_t gas_id);
extern grlc_diag_dl_rx_t* grlc_dl_diag_rx_stat_remove_entry(const gas_id_t gas_id);
extern uint8 grlc_dl_diag_rx_stat_get_count(const gas_id_t gas_id);
extern void grlc_dl_diag_clear_stat(const gas_id_t gas_id);

#endif /* INC_GRLCDLTST_H */

/*** EOF: don't remove! ***/

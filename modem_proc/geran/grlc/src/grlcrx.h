/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC RECEIVE MODULE
***
***
*** DESCRIPTION
***
***  Definitions of Receive Functions
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlcrx.h#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 03/29/01    hv     Created
***
*****************************************************************************/

#ifndef INC_GRLCRX_H
#define INC_GRLCRX_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */
#include "grlcdl.h"

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/

/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/

/*-------------------------------------------
** current downlink data block for gprs and egprs.
** Temporary downlink data block workspace.
**-------------------------------------------
*/
extern grlc_dl_data_block_t     cur_dl_blk[NUM_GERAN_DATA_SPACES];

extern e_grlc_dl_header_t       e_cur_hdr[NUM_GERAN_DATA_SPACES];

extern mac_hdr_t                cur_mac_hdr[NUM_GERAN_DATA_SPACES];

/*-----------------------------------------------------------------------
** Pointer to an arrray of structures, each representing a downlink 
** GPRS/EGPRS data block
**-----------------------------------------------------------------------
*/
extern grlc_dl_data_block_t  (*(p_rx_arr[NUM_GERAN_DATA_SPACES]))[];
///HV-extern grlc_dl_data_block_t   (*p_rx_arr)[];

/*-----------------------------------------------------------------------
** Pointer to the DSM item representing the portable layer 1 radio block
** This is used to retrieve a block from the PL1/RLC-DL FIFO
**-----------------------------------------------------------------------
*/
extern pl1_radio_block_header_t *pl1_blk_hdr_ptr[NUM_GERAN_DATA_SPACES];
extern dsm_item_type            *pl1_blk_dsm_ptr[NUM_GERAN_DATA_SPACES];


/*-----------------------------------------------------------------------
** Pointer to a EGPRS portable layer 1 radio block.
** This is used to retrieve a block from the PL1/RLC-DL FIFO
**-----------------------------------------------------------------------
*/
extern e_pl1_radio_block_t e_pl1_dl_blk[NUM_GERAN_DATA_SPACES];

#ifdef FEATURE_GPRS_PS_HANDOVER
/*-----------------------------------------------
** EGPRS temporary downlink data block for PSHO
**-----------------------------------------------
*/
extern dl_radio_block_psho_t dl_psho_blind_arr[NUM_GERAN_DATA_SPACES][DL_BLIND_DATA_BLKS];

#endif /* FEATURE_GPRS_PS_HANDOVER */
/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      CLEAR_RECEIVE_STATUS()
===
===  DESCRIPTION
===
===    Clear receive status of data blocks in the receive array from 'start' to
===    'end' - 1 for both GPRS and EGPRS modes
===
===  DEPENDENCIES
===    
===    'start' and 'end' represent the start and end points within the RX array
===    MODULO SNS
===  
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
extern void clear_receive_status
(
  gas_id_t  gas_id,
  uint16    start,
  uint16    end
);

/*===========================================================================
===
===  FUNCTION     E_UPDATE_DL_UNACK_STATE_VARS()
===
===  DESCRIPTION
===
===    Update the V(R) and V(Q) state variables after a new block of
===    given BSN arrives. V(R) is updated first before V(Q) because the 
===    conditions for updating V(Q) involves V(R).
===
===    input: Latest DL BSN, global dl (dl_ass, vr, vq, bsn).
===    output: dl (vr, vq) and Vn array
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
===    All vars involved are module variables
===
===========================================================================*/
extern void e_update_dl_unack_state_vars ( gas_id_t gas_id, uint16 bsn );

/*===========================================================================
===
===  FUNCTION    E_FORMAT_DL_ACKNACK_DESC()
===
===  DESCRIPTION
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
extern void e_format_dl_acknack_desc
(
  gas_id_t                   gas_id,
  rm_dl_pkt_acknack_req_t    *dl_acknack_ptr,
  const pl1_rlc_msg_t        *epdan_req_ptr
);

/*===========================================================================
===
===  FUNCTION    GRLC_DL_REQUEST_PKT_ACK_NACK_TRANSMISSION()
===
===  DESCRIPTION
===
===    Prepare a RM_DL_PKT_ACKNACK_REQ message for each queued SP (Poll)
===    and send this to MAC.
===
===    Also monitor FBI indication and enter into pending release state if
===    all blocks have been received.
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
extern void grlc_dl_request_pkt_ack_nack_transmission( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      DS_RX_ACK_STATE_PL1_PH_DATA_IND_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Handler of PL1_PH_DATA_IND event in DS_RX_ACK state.
===
===  DEPENDENCIES
===    
===   None
===  
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===========================================================================*/
extern void ds_rx_ack_state_pl1_ph_data_ind_event_handler( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      E_DS_RX_ACK_STATE_PL1_PH_DATA_IND_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Handler of PL1_PH_DATA_IND event in DS_RX_ACK state in EGPRS mode.
===
===  DEPENDENCIES
===    
===   None
===  
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===========================================================================*/
extern void e_ds_rx_ack_state_pl1_ph_data_ind_event_handler( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      E_DS_RX_ACK_STATE_PL1_EPDAN_REQ_EVENT_HANDLER
===
===  DESCRIPTION
===
===   Handler of PL1_EPDAN_REQ event in DS_RX_ACK state in EGPRS mode 
===   when DL fifo is empty.
===
===  DEPENDENCIES
===    
===   None
===  
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===========================================================================*/
extern void e_ds_rx_ack_state_pl1_epdan_req_event_handler( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      DS_RX_ACK_PEND_REL_STATE_PL1_PH_DATA_IND_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Handler of PL1_PH_DATA_IND event in DS_RX_ACK_PEND_REL state.
===
===  DEPENDENCIES
===    
===   None
===  
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===========================================================================*/
extern void ds_rx_ack_pend_rel_state_pl1_ph_data_ind_event_handler( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      E_DS_RX_ACK_PEND_REL_STATE_PL1_PH_DATA_IND_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Handler of PL1_PH_DATA_IND event in DS_RX_ACK_PEND_REL state.
===
===  DEPENDENCIES
===    
===   None
===  
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===========================================================================*/
extern void e_ds_rx_ack_pend_rel_state_pl1_ph_data_ind_event_handler( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      DS_RX_UNACK_STATE_PL1_PH_DATA_IND_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Handler of PL1_PH_DATA_IND event in DS_RX_UNACK state in GPRS mode.
===
===  DEPENDENCIES
===    
===   None
===  
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===========================================================================*/
extern void ds_rx_unack_state_pl1_ph_data_ind_event_handler( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      E_DS_RX_UNACK_STATE_PL1_PH_DATA_IND_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Handler of PL1_PH_DATA_IND event in DS_RX_UNACK state in EGPRS mode.
===
===  DEPENDENCIES
===    
===   None
===  
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===========================================================================*/
extern void e_ds_rx_unack_state_pl1_ph_data_ind_event_handler( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      E_DS_RX_UNACK_STATE_PL1_EPDAN_REQ_EVENT_HANDLER
===
===  DESCRIPTION
===
===   Handler of PL1_EPDAN_REQ event in DS_RX_UNACK state in EGPRS mode 
===   when DL fifo is empty.
===
===  DEPENDENCIES
===    
===   None
===  
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===========================================================================*/
extern void e_ds_rx_unack_state_pl1_epdan_req_event_handler( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      DS_RX_UNACK_PEND_REL_STATE_PL1_PH_DATA_IND_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Handler of PL1_PH_DATA_IND event in DS_RX_UNACK_PEND_REL state in GPRS mode.
===
===  DEPENDENCIES
===    
===   None
===  
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===========================================================================*/
extern void ds_rx_unack_pend_rel_state_pl1_ph_data_ind_event_handler( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      E_DS_RX_UNACK_PEND_REL_STATE_PL1_PH_DATA_IND_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Handler of PL1_PH_DATA_IND event in DS_RX_UNACK_PEND_REL state in EGPRS mode.
===
===  DEPENDENCIES
===    
===   None
===  
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===========================================================================*/
extern void e_ds_rx_unack_pend_rel_state_pl1_ph_data_ind_event_handler( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION    GRLC_DL_SCHEDULE_PKT_ACK_NACK_TRANSMISSION()
===
===  DESCRIPTION
===
===    Processes the SP indication of the current received radio block to
===    determine whether the Packet ACK/NACK Description Request should be
===    requested immediately, or held pending until the last BSN in the
===    current radio block period has been received.
===
===    This decision is essential when interacting with a BSS that does not
===    order it's BSNs in line with the time slot number (eg Motorola BSS).
===    Failure to deal with this results in false NAKed block indications,
===    unnecessary retransmission, and reduced throughput.
===
===  DEPENDENCIES
===
===    dl
===  
===  RETURN VALUE
===
===    TRUE if PKT ACK/NACK transmission is required, FALSE otherwise.
===
===  SIDE EFFECTS
===
===========================================================================*/
extern boolean grlc_dl_schedule_pkt_ack_nack_transmission( gas_id_t gas_id );


#ifdef FEATURE_GPRS_PS_HANDOVER

/*===========================================================================
===
===  FUNCTION      grlc_handle_dl_psho_complete_ind_null_and_release_state()
===
===  DESCRIPTION
===
===  On Reception of DL_PSHO_COMPLETE_IND in DS_NULL/DS_PENDING_REL
===  if the PSHO is FAILURE -  Blind Data blocks are deleted.
===  if the PSHO is SUCCESS ,  TBF is inialized and blind data blocks processed.
===
===
===
===  DEPENDENCIES
===
===    dl_ass_ind_present when new TBF and when T3192 running and complete_ind
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
extern void grlc_handle_dl_psho_complete_ind_null_and_release_state
            (
              gas_id_t                        gas_id,
              const mr_dl_psho_complete_ind_t *dl_psho_complete_ind
            );

/*===========================================================================
===
===  FUNCTION      grlc_handle_dl_psho_complete_ind_xfer()
===
===  DESCRIPTION
===
===  On Reception of DL_PSHO_COMPLETE_IND in ACK/ UNACK XFER.
===  if the PSHO is FAILURE -  Blind Data blocks are deleted.
===  if the PSHO is success from GtoW - DL is released Locally.
===  if the PSHO is GtoG and Reset - 1 , DL TBF is initialised.
===  if the PSHO is GtoG and Reset - 0 , DL TBF is continued.
===
===  DEPENDENCIES
===
===    dl_ass_ind_present when new TBF and when T3192 running and complete_ind
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
extern void grlc_handle_dl_psho_complete_ind_xfer
            (
              gas_id_t                        gas_id,
              const mr_dl_psho_complete_ind_t *dl_psho_complete_ind
            );

/*===========================================================================
===
===  FUNCTION      grlc_dl_delete_blind_data_blocks()
===
===  DESCRIPTION
===
===  When there is PSHO FAILURE or PANIC RESET , it is called  to free
===  DSM items.
===
===  DEPENDENCIES
===
===    none
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
extern void grlc_dl_delete_blind_data_blocks ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      grlc_dl_store_blind_data_blocks()
===
===  DESCRIPTION
===
===   The function read the DL FIFO and store the block as per TBF_MODE
===   for blind data transmission
===
===  DEPENDENCIES
===
===    none
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
extern void grlc_dl_store_blind_data_blocks ( gas_id_t gas_id );

#endif /* FEATURE_GPRS_PS_HANDOVER */

#endif /* INC_GRLCRX_H */

/*** EOF: don't remove! ***/

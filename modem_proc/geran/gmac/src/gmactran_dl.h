/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC TRANSFER MODE MODULE
***
***
*** DESCRIPTION
***
***  This module administrates the signal reception and the associated actions
***  for transfer mode states.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/src/gmactran_dl.h#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 05/12/01   sb      Created
***
*****************************************************************************/

#ifndef INC_GMACTRANSFERDL_H
#define INC_GMACTRANSFERDL_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "geran_variation.h"
#include "customer.h"

/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/

/* PACKET MOBILE TBF STATUS message CAUSE values */
#define MOBILE_TBF_STATUS_NORMAL_EVENT_CAUSE          0
#define MOBILE_TBF_STATUS_STATUS_UNSPECIFIED          1
#define MOBILE_TBF_STATUS_NON_DIST_PART_ERR           2
#define MOBILE_TBF_STATUS_MESS_ESCAPE_CAUSE           3
#define MOBILE_TBF_STATUS_PROTOCOL_ERROR_CAUSE        4

/*****************************************************************************
***
***     Public Typedefs
***
*****************************************************************************/
typedef struct
{
  q_link_type link;
  rm_dl_pkt_acknack_req_t pdan_elem;
}gmac_dl_pdan_store_t;

typedef struct
{
  /* queue for PDAN information from RLC waiting to be compiled and sent */
  q_type   gmac_local_pdan_info_store;
  q_type  *gmac_local_pdan_info_store_ptr;

  /* PMR cannot be sent until 2 x PDAN sent, so count up to 2 */
  uint8 gmac_transfer_dl_ack_nack_count_max_2;

  /*flag is set when a pending UL tbf est chan req is to be included in the EGPRS pdan*/
  /*and reset when the EGPRS PDAN with the chan req is sent or the DL tbf is released*/
  boolean egprs_pdan_with_chan_req_pending;

  boolean i_level_meas;

  boolean gprs_can_send_pmr_pccf;

  boolean egprs_can_send_pmr_pccf;
} gmac_transfer_dl_data_t;

/*****************************************************************************
***
***     Public Function Prototypes
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      gmac_dl_transfer_init 
===
===  DESCRIPTION
===  Initialise information applicable to downlink transfer
===
===  DEPENDENCIES
===  gmac_local_pdan_info_store_ptr - local queue for PDAN handling
===  gmac_local_ul_tbf_est_pending - if UL request pending
=== 
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
void gmac_dl_transfer_init(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_transfer_dl_normal_release_to_idle 
===
===  DESCRIPTION
===  Inform L1 and GRR of DL release
===
===  DEPENDENCIES
===
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
void gmac_transfer_dl_normal_release_to_idle(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_transfer_dl_normal_release_with_ul 
===
===  DESCRIPTION
===  Inform L1 and GRR of DL release
===
===  DEPENDENCIES
===
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
void gmac_transfer_dl_normal_release_with_ul(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_transfer_send_tbf_status 
===
===  DESCRIPTION
===  Compile and send PACKET MOBILE TBF STATUS to the network by
===  sending MAC_PH_DATA_REQ to L1 with message content.
===
===  DEPENDENCIES
===  gmac_local_tfi_store - provides TFI
===  csn_encoder_ul_msg - store message data prior to CSN.1 encoder
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
void gmac_transfer_send_tbf_status(uint8 cause, uint8 mess_type,
                                   gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_transfer_send_stored_dl_config_to_l1 
===
===  DESCRIPTION
===  MAC_L1_DL_TBF_CONFIG data has been stored waiting to send to L1.
===  Copy stored data into message and send.
===
===  DEPENDENCIES
===  gmac_transfer_store_dl_tbf_msg - message data is stored
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
void gmac_transfer_send_stored_dl_config_to_l1(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_transfer_send_stored_ts_reconfig_to_l1
===
===  DESCRIPTION
===  MAC_L1_TS_RECONFIG data has been stored waiting to send to L1.
===  Copy stored data into message and send.
===
===  DEPENDENCIES
===  gmac_transfer_store_ts_reconfig - message data is stored
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
void gmac_transfer_send_stored_ts_reconfig_to_l1(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_transfer_dl_pkt_acknack 
===
===  DESCRIPTION
===  Complete downlink packet ACK/NACK and send to L1           
===  Data for message is stored in local queue for PDAN data which
===  is retrieved and added to message content. CSN.1 encoder is used.
===
===  The mobile station shall transmit a PDAN message in the UL RB specified 
===  by the RRBP field, unless another RLC/MAC control message is waiting to 
===  be transmitted, in which case the other RLC/MAC control message shall be 
===  sent. 
===
===  However, the mobile station shall transmit an RLC/MAC control message 
===  other than a PDAN at most every second time it is polled. 
===
===  NOTE: The MS shall not transmit an RLC/MAC control message other than a 
===        PDAN if it contains a FAI or Channel Request Description IE.
===
===  DEPENDENCIES
===  gmac_local_pdan_info_store_ptr - pointer to local queue for PDAN data
===  gmac_local_ul_tbf_est_pending - indication if UL pending
===  gmac_local_stored_ul_chan_req - stored pending UL channel description
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
void gmac_transfer_dl_pkt_acknack(l1_mac_qual_measurements_t  *rx_qual_meas_ptr, gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_transfer_dl_pdan_handler 
===
===  DESCRIPTION
===  The mobile station shall transmit a PDAN message in the UL RB specified 
===  by the RRBP field, unless another RLC/MAC control message is waiting to 
===  be transmitted, in which case the other RLC/MAC control message shall be 
===  sent. 
===
===  However, the mobile station shall transmit an RLC/MAC control message 
===  other than a PDAN at most every second time it is polled. 
===
===  NOTE: The MS shall not transmit an RLC/MAC control message other than a 
===        PDAN if it contains a FAI or Channel Request Description IE.
===
===  DEPENDENCIES
===  gmac_local_pdan_info_store_ptr - store PDAN data from RLC
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
void gmac_transfer_dl_pdan_handler(rm_dl_pkt_acknack_req_t *acknack_msg_ptr, boolean ul_present,
                                   gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_transfer_dl_data_ind 
===
===  DESCRIPTION
===  Handles received logical channel messages during downlink transfer
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header                       
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
void gmac_transfer_dl_data_ind(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_transfer_dl_reassign_data_ind 
===
===  DESCRIPTION
===  Handles received logical channel messages during downlink
===  reassignment
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header                       
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
void gmac_transfer_dl_reassign_data_ind(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_transfer_dl_dynamic_alloc_data_ind 
===
===  DESCRIPTION
===  Handles received logical channel messages during downlink transfer
===  with UL dynamic allocation
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header                       
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
void gmac_transfer_dl_dynamic_alloc_data_ind(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_transfer_dl_reassign_dynamic_alloc_data_ind 
===
===  DESCRIPTION
===  Handles received logical channel messages during downlink
===  reassignment with UL dynamic allocation
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header                       
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
void gmac_transfer_dl_reassign_dynamic_alloc_data_ind(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION    gmac_get_transfer_dl_data_ptr()
===
===  DESCRIPTION
===  Returns a pointer to transfer dl data space using the current gas_id
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===    gmac_null_data_t*
=== 
===  SIDE EFFECTS
===
===
===========================================================================*/
gmac_transfer_dl_data_t* gmac_get_transfer_dl_data_ptr(gas_id_t gas_id);


/*===========================================================================
===
===  FUNCTION      gmac_tran_check_dl_tbf_status
===
===  DESCRIPTION
===
===  Check to see if MAC is in the process ofsending an GPRS PDAN
===  with the FBI bit set and is waiting on the reception of 
===  L1_MAC_QUAL_MEASURMENTS when NO PS ACCESS is rcvd. 
===  Ie MAC has not yet sent L1 the PDAN for tx-ing.
===
===  Under these conditions use the previously rcvd L1_MAC_QUAL_MEASURMENTS
===  measurements 
===                                               
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/

void gmac_tran_check_dl_tbf_status(gas_id_t gas_id);

#endif /* INC_GMACTRANSFERDL_H */
/*** EOF: don't remove! ***/

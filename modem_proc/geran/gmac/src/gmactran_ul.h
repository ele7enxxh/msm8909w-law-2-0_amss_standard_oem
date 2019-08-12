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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/src/gmactran_ul.h#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 04/12/01   sb      Created
***
*****************************************************************************/

#ifndef INC_GMACTRANSFERUL_H
#define INC_GMACTRANSFERUL_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "geran_variation.h"
#include "customer.h"

/*****************************************************************************
***
***     Public Function Prototypes
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      gmac_transfer_ul_handle_pda
===
===  DESCRIPTION
===  Handles PACKET DOWNLINK ASSIGNMENT received during packet transfer
===
===  - if invalid frequency parameters perform abnormal release with SI
===  - if frequency parameters are different from what 
===    is currently in use for uplink, ignore PDA
===  - if does not define PDCH satisfactorily, ignore PDA
===  - failure for any other reason, abort downlink, continue with uplink
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header                       
===  csn_dec_data_ptr - global pointer to decoded CSN.1 data                       
===
===  RETURN VALUE
===  - TRUE if PDA successful
===  - FALSE if PDA unsuccessful
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
boolean gmac_transfer_ul_handle_pda(uint8 current_mac_mode,
                                    gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_tdynamic_handle_ptr
===
===  DESCRIPTION
===  Handles PACKET TIMESLOT RECONFIGURE received during 
===  uplink dynamic packet transfer
===
===  - if invalid frequency parameters perform abnormal release with SI
===  - if fixed allocation, abnormal release with retry
===  - if does not define UL and DL PDCH, abnormal release with retry
===  - violates multislot capabilities, abnormal release with retry
===  - failure for any other reason, abnormal release with retry
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header                       
===  csn_dec_data_ptr - global pointer to decoded CSN.1 data                       
===
===  RETURN VALUE
===  TRUE if PTR accepted
===  FALSE if PTR rejected
===
===  SIDE EFFECTS
===
===
===========================================================================*/
boolean gmac_tdynamic_handle_ptr(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_tdynamic_handle_pua 
===
===  DESCRIPTION
===  Handles packet uplink assignment during uplink dynamic TBF
===  This leads to uplink dynamic reassignment
===
===  - if invalid frequency parameters perform abnormal release with SI
===  - if fixed allocation assigned, abnormal release with retry
===  - if does not properly define UL PDCH, abnormal release with retry
===  - violates multislot class, abnormal release with retry
===  - failure for any other reason, abnormal release with retry
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header                       
===  csn_dec_data_ptr - global pointer to decoded CSN.1 data                       
===  
===  RETURN VALUE
===  TRUE if PUA accepted
===  FALSE if PUA rejected
===
===  SIDE EFFECTS
===
===
===========================================================================*/
boolean gmac_tdynamic_handle_pua(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_tdynamic_handle_puan 
===
===  DESCRIPTION
===  Handles packet uplink ACK/NACK during uplink dynamic TBF.
===  If PUAN contains fixed allocation, abnormal release with access retry
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header                       
===  csn_dec_data_ptr - global pointer to decoded CSN.1 data                       
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
void gmac_tdynamic_handle_puan(boolean dl_present, gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_transfer_ul_dynamic_release_clean 
===
===  DESCRIPTION
===  When UL dynamic released, clean up any outstanding UL memory.
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
void gmac_transfer_ul_dynamic_release_clean(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_transfer_ul_tbf_abort_alloc 
===
===  DESCRIPTION
===  Called when an error occurs in the PUA assignment.
===  
===  If MAC is establishing an UL from a DL only state then block/unblock 
===  RLC and move to the DL state.
=== 
===  If the UL is being reallocated then perform an abnormal release with retry.
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
void gmac_transfer_ul_tbf_abort_alloc(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_transfer_ul_tbf_abnormal_abort 
===
===  DESCRIPTION
===  This function is called when a DL TBF only is active in order to
===  abort any pending requests for UL TBF est by RLC.  RLC is always
===  blocked regardless of whether it was requesting access or not.
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
void gmac_transfer_ul_tbf_abnormal_abort(delete_pdu_cause_t delete_pdu_cause,
                                         gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_transfer_handle_ppcta 
===
===  DESCRIPTION
===  Handles packet power control/timing advance message during transfer.
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header                       
===  csn_dec_data_ptr - global pointer to decoded CSN.1 data                       
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
void gmac_transfer_handle_ppcta(boolean ul_present, gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_transfer_handle_par
===
===  DESCRIPTION
===  Handles PACKET ACCESS REJECT received during uplink TBF. If none of
===  the given addresses matchs the MS then the PAR is ignored. If an
===  address match is made then a reject is sent to RLC indicating whether
===  WAIT_INDICATOR is present or not.
===
===  DEPENDENCIES
===  csn_dec_data_ptr - global pointer to decoded CSN.1 data                       
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
void gmac_transfer_handle_par(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_transfer_qual_meas_req_to_L1
===
===  DESCRIPTION
===  Send MAC_L1_QUALITY_RPT to L1
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
void gmac_transfer_qual_meas_req_to_L1(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_tdynamic_usf_detected 
===
===  DESCRIPTION
===  Handles USF detection indication from L1. If final ack indication set
===  then do not start T3180, otherwise re-start timer.
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
void gmac_tdynamic_usf_detected(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_tdynamic_data_ind 
===
===  DESCRIPTION
===  Handles received logical channel messages during uplink dynamic
===  packet transfer           
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
void gmac_tdynamic_data_ind(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_tdynamic_realloc_data_ind 
===
===  DESCRIPTION
===  Handles received logical channel messages during uplink dynamic
===  reallocation
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
void gmac_tdynamic_realloc_data_ind(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_tdynamic_dl_assign_data_ind 
===
===  DESCRIPTION
===
===  Handles received logical channel messages during uplink dynamic
===  packet transfer with DL assignment
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
void gmac_tdynamic_dl_assign_data_ind(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_tdynamic_realloc_dl_assign_data_ind 
===
===  DESCRIPTION
===
===  Handles received logical channel messages during uplink dynamic
===  reallocation with DL assignment in packet transfer
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
void gmac_tdynamic_realloc_dl_assign_data_ind(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_tdynamic_handle_egprs_cont_res_pua 
===
===  DESCRIPTION
===
===  Handles packet uplink assignment during uplink dynamic TBF
===  This leads to uplink dynamic reassignment
===
===
===  DEPENDENCIES                      
===  
===  RETURN VALUE
===  TRUE if PUA accepted
===  FALSE if PUA rejected
===
===  SIDE EFFECTS
===
===========================================================================*/
boolean gmac_tdynamic_handle_egprs_cont_res_pua(packet_uplink_assignment_t *pua_ptr,
                                                gas_id_t gas_id);

#endif /* INC_GMACTRANSFERUL_H */
/*** EOF: don't remove! ***/

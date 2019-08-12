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
***  for transfer mode states.  This file is of transfer mode only and should be
***  included by gmactransfer.c gmactran_ul.c gmactran_dl.c gmactran_c.c
***
*** 
*** Copyright (c) 2001,2002,2003,2004,2005-2013 Qualcomm Technologies, Inc.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/inc/gmactransfer.h#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 20/11/01   sb      Created
***
*****************************************************************************/

#ifndef INC_GMACTRANSFER_H
#define INC_GMACTRANSFER_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "geran_variation.h"
#include "customer.h"

#include "gmactransfer_if.h"

/*****************************************************************************
***
***     Transfer Public Typedefs
***
*****************************************************************************/

typedef enum
{
  REL_RLC_DL,
  DONT_REL_RLC_DL,
  NO_RLC_DL_TBF_PRESENT_TO_RELEASE
} gmac_rlc_dl_rel_cause_t;

typedef struct
{
  uint8 csn_encoded_pts[SIZE_OF_CONTROL_RADIO_BLOCK];
  boolean pts_pending;
} gmac_pts_params_t;

typedef enum
{
  GMAC_TRANSFER_TBF_REL_NORMAL,
  GMAC_TRANSFER_TBF_REL_GRR_NO_SERVICE
} gmac_transfer_tbf_rel_t;

typedef struct
{
  /* indicates FAI for transfer */
  uint8 gmac_transfer_glob_final_ack_ind;

  /* reallocation PRR retry count */
  uint8 gmac_transfer_ul_request_count;

  /* store config data destines for L1 */
  mac_l1_dl_tbf_config_t  gmac_transfer_store_dl_tbf_msg;
  mac_l1_ts_reconfig_t    gmac_transfer_store_ts_reconfig;

  /* uplink allocation confirm */
  mac_l1_sig_t gmac_transfer_store_l1_dyn_realloc;

  /* Store data in PUA for RLC */
  mr_ul_alloc_cnf_t gmac_transfer_ul_rlc_info_store;

  /* store coding scheme setting for transfer when rcvd in PUA or PTR*/
  uint8 pua_ptr_cs_change;

  gprs_mac_transfer_substates_t mac_transfer_substate;

  transfer_flags_t   t_flags;

  gmac_pts_params_t  gmac_local_pts_params;

  mac_l1_sig_t mac_l1_sig_cs_reconfig;
  mac_rlc_msg_t *mr_ul_alloc_cnf_ptr;
  mac_rlc_msg_t *mr_dl_test_mode_ind_ptr;
  uint8 t3168_time;

  /* PCCF parameters saved during transfer */
  gmac_pccf_params_t gmac_transfer_local_pccf_params;

  gmac_transfer_tbf_rel_t gmac_transfer_tbf_rel_cause;

  char *mac_tran_substate_str;        /* String representation of the current tran state */

#ifdef FEATURE_GSM_EDTM
  mac_l1_edtm_cs_rel_tbf_alloc_t mac_l1_edtm_cs_rel_alloc_msg; /* MAC L1 CS REL ALLOC msg store */
#endif /*FEATURE_GSM_EDTM*/

  uint32                     gmac_tran_last_dl_qual_meas_rrbp_value;
  l1_mac_qual_measurements_t gmac_tran_saved_qual_meas;

} gmac_transfer_data_t;

/*****************************************************************************
***
***     MAC Transfer Data Declarations
***
***     These externs are for transfer mode only (gmactran_dl.c gnactran_ul.c
***     gmactran_c.c gmactransfer.c)
***
*****************************************************************************/

/*****************************************************************************
***
***     Public Function Prototypes
***
*****************************************************************************/
/*===========================================================================
===
===  FUNCTION    gmac_abnormal_release_procedure() 
===
===  DESCRIPTION
===  Handles all abnormal release of UL & DL TBF.  This function handles both
===  MAC initiated abnormal releases and GRR (GRR_MAC_NO_PS_ACCESS_IND) releases.
===
===  PARAMS
===    mac_release_cause:  Indicates if GRR needs an abnormal release
===    release_rlc_dl:     Indicates if RLC DL should be released    
===    rlc_del_ul_pdu:     Type of RLC UL PDU deletion used when blocking RLC
===    dtm_chan_rel        Indicates if the ab rel is from a DTM release.
===
===  RETURN VALUE
===    None
===    
===  SIDE EFFECTS
===    c_flags.tbf_release_pending_flg write
===    mac_transfer_substate read
===    t_flags.abnormal_ul_and_dl_rel_flg write
=== 
===========================================================================*/
void gmac_abnormal_release_procedure
(
    rr_abnormal_release_cause_t  mac_release_cause,
    gmac_rlc_dl_rel_cause_t      release_rlc_dl,
    delete_pdu_cause_t           rlc_del_ul_pdu,
    boolean                      dtm_chan_rel,
    gas_id_t                     gas_id
);


/*===========================================================================
===
===  FUNCTION      gmac_tran_pcco_handler() 
===
===  DESCRIPTION
===  Sends GRR PCCO msg if its correctly addressed.
===
===  DEPENDENCIES
===  ptr_csn_decode_dl_msg - global pointer to decoded CSN.1 data 
=== 
===  RETURN VALUE
===
===  SIDE EFFECTS
===  
===========================================================================*/
void gmac_tran_pcco_handler(gprs_mac_transfer_substates_t mac_tran_substate,
                            gas_id_t gas_id);


#ifdef FEATURE_GSM_EDTM
/*===========================================================================
===
===  FUNCTION      gmac_transfer_edtm_pkt_cs_cmd_handler() 
===
===  DESCRIPTION
===  Sends GRR Packet CS Command msg if its correctly addressed.
===
===  DEPENDENCIES
===  csn_dec_data_ptr - global pointer to decoded CSN.1 data 
=== 
===  RETURN VALUE
===
===  SIDE EFFECTS
===  
===========================================================================*/
void gmac_transfer_edtm_pkt_cs_cmd_handler(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_tran_edtm_cs_rel_ind_handler() 
===
===  DESCRIPTION
===
===  DEPENDENCIES    
=== 
===  RETURN VALUE
===    NONE
===
===  SIDE EFFECTS
===  
===========================================================================*/
gprs_mac_transfer_substates_t gmac_tran_edtm_cs_rel_ind_handler( 
                                       gprs_mac_transfer_substates_t transfer_state,
                                       active_tbf_t                  active_tbfs,
                                       gas_id_t gas_id );

#endif /*FEATURE_GSM_EDTM*/


/*===========================================================================
===
===  FUNCTION      gmac_transfer_simulate_freq_cnf 
===
===  DESCRIPTION
===
===  Called when simulating frequency confirm.  Determines which config messages
===  should be sent depending on which transfer state MAC is in.
===
===  DEPENDENCIES
===  mac_transfer_substate - Determines which config messages should be sent.
===
===  PARAMETERS
===    none
=== 
===  RETURN VALUE
===    none
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_transfer_simulate_freq_cnf( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      gmac_convert_transfer_substate_to_string() 
===
===  DESCRIPTION
===  Converts the supplied transfer state (of type gprs_mac_transfer_substates_t) into a string.
===
===  DEPENDENCIES  
=== 
===  RETURN VALUE
===    char *  - transfer substate string
===
===  SIDE EFFECTS
===  
===========================================================================*/
char * gmac_convert_transfer_substate_to_string( gprs_mac_transfer_substates_t transfer_state,
                                                 gas_id_t gas_id );


#ifdef FEATURE_GPRS_PS_HANDOVER
/*===========================================================================
===
===  FUNCTION      gmac_tran_psho_handler() 
===
===  DESCRIPTION
===  This function validates PSHO command message and if validation successful
===  this function forwards the message to GRR. This function also ignores the 
===  pending tbf and sends MAC_L1_PSHO_CANCEL_PENDING_CONFIG to GL1
===
===  DEPENDENCIES
===
===   
===  RETURN VALUE
===  void
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_tran_psho_handler(gas_id_t gas_id);
#endif /* FEATURE_GPRS_PS_HANDOVER */

/*===========================================================================
===
===  FUNCTION      gmac_tran_handle_pdch_release 
===
===  DESCRIPTION
===  Send PDCH release to L1 with signal MAC_L1_PDCH_REL_TS.
===
===  DEPENDENCIES
===  timeslot_available - timeslot bitmap
===  
===  RETURN VALUE
===
===    
===  SIDE EFFECTS
===
===  
===========================================================================*/
void gmac_tran_handle_pdch_release(uint8 timeslot_available,active_tbf_t active_tbf,
                                   gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION    gmac_transfer_send_pts_on_pacch_rrbp() 
===
===  DESCRIPTION
===  Function used by transfer to send a pkt ms tbf_status during an active
===  downlink tbf. The pkt ms tbf_status is sent in a PACCH RRBP message to
===  the network
===
===
===  DEPENDENCIES
===
===  RETURN VALUE
===    
===  SIDE EFFECTS
=== 
=== 
===========================================================================*/
void gmac_transfer_send_pts_on_pacch_rrbp(uint8 ts, uint8 rrbp, uint32 fn,
                                          gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION    gmac_get_transfer_data_ptr()
===
===  DESCRIPTION
===  Returns a pointer to transfer data space using the current gas_id
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===    gmac_transfer_data_t*
=== 
===  SIDE EFFECTS
===
===
===========================================================================*/
gmac_transfer_data_t* gmac_get_transfer_data_ptr(gas_id_t gas_id);

#endif /* INC_GMACTRANSFER_H */
/*** EOF: don't remove! ***/

/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC TRANSFER MODE MODULE
***
***
*** DESCRIPTION
***
***  This module is the public interface of Transfer mode functionality.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/inc/gmactransfer_if.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** 10-03-30   tjw     Explicitly include dependent headers to make SU API files
***                    stand alone
*** 20/11/01   sb      Created
***
*****************************************************************************/

#ifndef INC_GMACTRANSFER_IF_H
#define INC_GMACTRANSFER_IF_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif


#include "gprsdef.h"
#include "gprsdef_g.h"
#include "gmacsigl1.h"
#include "gmacsigl1_g.h"
#include "grlcmac.h"
#include "gmacrlc.h"
#include "gmacl1sig.h"
#include "gmacl1sig_g.h"

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

/********* TRANSFER mode sub states ************/
typedef enum
{
  TRANSFER_INACTIVE,

  TRANSFER_DL,
  TRANSFER_DL_REASSIGN,
  TRANSFER_DL_WITH_DYNAMIC_ALLOC,
  TRANSFER_DL_REASSIGN_DYNAMIC_ALLOC,

  TRANSFER_UL_DYNAMIC,
  TRANSFER_UL_DYNAMIC_REALLOC,
  TRANSFER_UL_DYNAMIC_DL_ASSIGN,
  TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN,

  TRANSFER_CONCURRENT_DYNAMIC_TRANSFER,
  TRANSFER_CONCURRENT_DYNAMIC_DL_REASSIGN,
  TRANSFER_CONCURRENT_DYNAMIC_REALLOC,
  TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC
#ifdef FEATURE_GPRS_PS_HANDOVER
  ,
  TRANSFER_PS_HANDOVER
#endif /*FEATURE_GPRS_PS_HANDOVER*/
}gprs_mac_transfer_substates_t;

#ifdef FEATURE_GSM_DTM
typedef enum
{
  NO_REL,
  UL_NORM,
  DL_NORM,
  UL_AB,
  DL_AB,
  UL_DL_NORM,
  UL_DL_AB
} gmac_tran_rel_state_t;
#endif /*FEATURE_GSM_DTM*/

typedef struct
{
  /***** ul_tbf_normal_release_ind *****************************************************
   *
   *    Indicates that an UL is being released normally by RLC in a pending DL
   *    assignment state.  This ensures that when the DL Connect is received,
   *    the state is changed to DL and not Concurrent.
   *
   *************************************************************************************/
  boolean ul_tbf_normal_release_ind;


  /***** dl_tbf_normal_release_ind ****************************************************
   *
   *    Indicates that a DL is being released normally by RLC in a pending DL assignment
   *    state.  This ensures that when the DL Connect is received, the state is changed
   *    to DL and not Concurrent.
   *
   ************************************************************************************/
  boolean dl_tbf_normal_release_ind;

  /***** gmac_local_ul_tbf_est_pending ************************************************
   *    Indicates that an RLC UL TBF req or Realloc is waiting to be estabalished.
   *
   ************************************************************************************/
  boolean gmac_local_ul_tbf_est_pending;


  /***** gmac_transfer_dl_fai_set *****************************************************
   *
   *    Indicates Last DL Packet ACK/NAK (with FAI set)
   *
   *************************************************************************************/
  boolean gmac_transfer_dl_fai_set;


  /***** gmac_transfer_ul_tbf_realloc_pending *******************************************
   *
   *    Indicates if UL realloc pending during DL reassign
   *
   *************************************************************************************/
  boolean gmac_transfer_ul_tbf_realloc_pending;


  /***** gmac_transfer_cs_reconfig_flag *************************************************
   *
   *    CS reconfig change during realloc
   *
   *************************************************************************************/
  boolean gmac_transfer_cs_reconfig_flag;


  /***** waiting_for_ph_con_for_ul *****************************************************
   *
   *    Indicates that a PUA has been received in a DL state
   *
   ************************************************************************************/
  boolean waiting_for_ph_con_for_ul;


  /***** waiting_for_ph_con_for_dl ******************************************************
   *
   *    Indicates that a PDA has been received in a DL state
   *
   *************************************************************************************/
  boolean waiting_for_ph_con_for_dl;

  /***** failed_to_send_pdan_chan_req ***************************************************
   *
   *
   *************************************************************************************/
  boolean failed_to_send_pdan_chan_req;


  /***** abnormal_ul_and_dl_rel_flg *****************************************************
   *
   * Flag set when an abnormal release of the UL and DL is rcvd and reset once the UL and
   * DL rel confirm is rcvd form L1. The flag is used when an abnormal release for the
   * UL+DL occurs during a normal release procedure and will causes TRANSFER to wait in
   * state till the UL+DL  rel confirm is rcvd form L1
   *
   **************************************************************************************/
  boolean abnormal_ul_and_dl_rel_flg;

  /***** gmac_transfer_ul_req **********************************************************
   *
   *  Indicates if an UL TBF request has been made by RLC.
   *  Needs to be set when pdan sends ul tbf req and  RM_UL_EST_TBF_REQ was not
   *  received in gmactransfer.c .
   *
   *************************************************************************************/
  boolean gmac_transfer_ul_req;

  /***** block_dl_msg_during_abnormal_ul_release ***************************************
   *
   *
   *************************************************************************************/
  boolean block_dl_msg_during_abnormal_ul_release;

  /***** block_dl_msg_during_normal_ul_release ***************************************
   *
   *
   *************************************************************************************/
  boolean block_dl_msg_during_normal_ul_release;

  /***** block_dl_msg_during_abnormal_dl_release ***************************************
   *
   *
   *************************************************************************************/
  boolean block_dl_msg_during_abnormal_dl_release;

  /***** dtm_active *********************************************************************
   *
   *  Indicates when MAC is currently in DTM Mode.
   *
   *************************************************************************************/
  boolean dtm_active;

  /***** gmac_rcvd_dl_assignment_in_concurrency **********************************************************
  *  Usage:  In DL TBF re-assignment states in concurrency on reception of L1_MAC_TBF_REL_CONFIRM
  *  the flag is checked to determine if there is a DL TBF pending connection. If there is the DL is released
  *  and state change to TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN where the pending DL TBF will
  *  eventually be connected

  *  Only in set T_CON when a DL assignment is rcvd( PDA or PTR.)
  *  Reset when the pending DL TBF is connected (PH_CONNECT_IND) in concurrent states
  *  OR when L1_MAC_TBF_REL_CONFIRM is rcvd while waiting for a pending DL
  *  to connect in concurrent states.
  *************************************************************************************/
  boolean gmac_rcvd_dl_assignment_in_concurrency;

#ifdef FEATURE_GSM_DTM
  /***** dtm_realloc_in_progress ********************************************************
   *
   *  Indicates when MAC is reallocating DTM resources.
   *  This will be set when a DTM Channel Ass is received in DTm and reset when
   *  the PH_CONNECT_IND for the DTM reallocation assignment is received.
   *
   *************************************************************************************/
  boolean dtm_realloc_in_progress;

  /***** gmac_tran_rel_state ********************************************************
   *
   *  Indicates when MAC is releasing a TBF.  It is set when a MAC_L1_TBF_REL is sent
   *  to L1.  It is reset when PH_CONNECT_IND or a L1_MAC_TBF_REL_CNF is received.
   *
   *************************************************************************************/
  gmac_tran_rel_state_t gmac_tran_rel_state;

#endif /*FEATURE_GSM_DTM*/

#ifdef FEATURE_GSM_EDTM
  /***** edtm_realloc_in_progress ********************************************************
   *
   *  Indicates when MAC is performing CS establishment using EDTM or performing a DTM
   *  rellocation when EDTM is enabled.
   *  This will be set when a DTM Channel Ass is received. It is reset when a
   *  L1_MAC_PH_CS_CONNECT_IND is received.
   *
   *************************************************************************************/
  boolean edtm_realloc_in_progress;

  /***** edtm_pcri_rel_dl_tbf_for_ctrl_ack **********************************************
   *
   *  Indicates that the CTRL_ACK bit was set for a valid DL TBF in the Packet CS Release
   *  Indication message.  MAC should release the DL immediately in this case.  If it is
   *  not possible to release the TBF immediately then this flag shall be set to indicate
   *  that the DL is to be released.
   *
   *************************************************************************************/
  boolean edtm_pcri_rel_dl_tbf_for_ctrl_ack;
#endif /*FEATURE_GSM_EDTM*/

  boolean gmac_tran_saved_qual_meas_valid;

}transfer_flags_t;

typedef enum
{
 UL_AND_DL,
 DL_ONLY,
 UL_ONLY
} active_tbf_t;

typedef enum
{
  GRR_CNF_NOT_NEEDED,
  GRR_CNF_NEEDED
} gmac_grr_cnf_needed_t;

typedef struct
{
  uint8 csn_encoded_pccf[SIZE_OF_CONTROL_RADIO_BLOCK];
  boolean pccf_pending;
#ifdef FEATURE_GPRS_PS_HANDOVER
  gmac_grr_cnf_needed_t pccf_cnf_needed;
#endif /*FEATURE_GPRS_PS_HANDOVER*/
} gmac_pccf_params_t;

#ifdef FEATURE_GPRS_PS_HANDOVER

typedef enum
{
 PSHO_NOT_ACTIVE,
 PSHO_WAITING_FOR_L1_CNF,
 PSHO_CONNECTING_TARGET_CHANNELS,
 PSHO_RECONNECTING_OLD_CHANNELS
} psho_mac_status_t;

typedef struct
{
uint8              psho_ul_alloc_ts;  /* This will holds the number of ul timeslots
                                         allocated in PSHO COMMAND*/

uint8              psho_dl_assign_ts;/* This will holds the number of dl timeslots
                                        allocated in PSHO command*/

uint8              psho_ul_alloc_dl_ctrl_ts;/* This holds the dl ctrl timeslot allocated
                                               in PSHO command. This flag is not required*/

mac_rlc_mode_T     psho_dl_rlc_mode;/* This holds the dl rlc mode needs to be sent in dl ass ind*/

mac_rlc_mode_T     psho_ul_rlc_mode;/* This holds the ul rlc mode needs to be sent in ul alloc cnf*/

uint8              psho_assign_ul_tfi;/* This holds the ul tfi assigned in PSHO command*/

uint8              psho_assign_dl_tfi;/* This holds the dl tbf assigned in PSHO command*/

mr_ul_alloc_cnf_t  psho_ul_alloc_info;/* This holds the ul alloc cnf structure to be
                                         passed to RLC after PSHO completion*/

uint8              psho_dl_ass_present; /* This will tell whether dl ass needs to pass it to
                                           RLC or not*/

mr_dl_ass_ind_t    psho_dl_ass_info; /* This holds the dl assign ind structure to
                                        be passed to RLC after PSHO completion*/

uint8              psho_reset_flag;/*This holds the reset flag received in PSHO COMMAND*/

uint8              psho_nas_container_flag;/* This will store whether NAS container is
                                              present in the PSHO message or not*/
}gmac_psho_config_params_t;

typedef struct
{
psho_mac_status_t  psho_substate;/* This will track the PSHO status in GMAC*/

psho_type_t        psho_type;/* This holds the type PSHO i.e GTOG/GTOW/ WTOG*/

psho_sync_type_t   psho_sync_type;/* This holds the type PSHO at L1.
                                    i.e SYNC/NONSYNC/PRESYNC*/

psho_tbf_type_t    psho_tbf_type;/* This holds type of tbf rcvd in PSHO*/

rlc_mac_msg_t      *ul_release_ind_ptr; /* This will store ul release signal when PS
                                       HO going on*/

rlc_mac_msg_t      *dl_release_ind_ptr; /* This will store dl release signal when PS
                                       HO going on*/

l1_mac_sig_t       *tbf_rel_cnf_ptr; /* This will store tbf rel cnf signal when PS
                                    HO going on*/

uint8              control_ack_set; /* This will set when control ack is set in PSHO*/

gprs_mac_transfer_substates_t psho_initiated_tran_substate;/* This will store the
                              transfer state when GMAC receives PSHO command from network*/

}gmac_psho_control_params_t;

typedef struct
{
/* PCCF parameters */
CSN_ARFCN_TYPE  psho_arfcn;
uint8   psho_bsic;
uint8   cause;
}gmac_psho_pccf_params_t;

typedef struct
{
gmac_psho_config_params_t       psho_config_params;
gmac_psho_control_params_t      psho_control_params;
gmac_psho_pccf_params_t         psho_pccf_params;

}gmac_ps_ho_t;
#endif /*FEATURE_GPRS_PS_HANDOVER*/

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION    gmac_tran_get_transfer_state()
===
===  DESCRIPTION
===  This is an access function for external entities to get the mac_transfer_substate.
===
===  PARAMS
===
===
===  RETURN VALUE
===  gprs_mac_transfer_substates_t
===
===  SIDE EFFECTS
===
===
===========================================================================*/
gprs_mac_transfer_substates_t gmac_tran_get_transfer_state(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION    gmac_tran_get_t_flags()
===
===  DESCRIPTION
===  This is an access function for external entities to get t_flags.
===
===  PARAMS
===
===
===  RETURN VALUE
===  transfer_flags_t
===
===  SIDE EFFECTS
===
===
===========================================================================*/
transfer_flags_t gmac_tran_get_t_flags(gas_id_t gas_id);

#ifdef FEATURE_GPRS_PS_HANDOVER
/*===========================================================================
===
===  FUNCTION      gmac_tran_check_psho_addressing()
===
===  DESCRIPTION
===  Sends GRR PSHO msg if its correctly addressed.
===
===  DEPENDENCIES
===  ptr_csn_decode_dl_msg - global pointer to decoded CSN.1 data
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
boolean gmac_tran_check_psho_addressing(gas_id_t gas_id);
#endif /*FEATURE_GPRS_PS_HANDOVER*/

#ifdef FEATURE_GSM_DTM
/*===========================================================================
===
===  FUNCTION    gmac_tran_rel_state_handle_rel_req()
===
===  DESCRIPTION
===    Updates the GMAC rel state for the MAC_L1_RELEASE signal.
===
===  PARAMS
===    tbf_rel_type - Indication of which TBF is being released
===
===  RETURN VALUE
===    NONE
===
===  SIDE EFFECTS
===    sets t_flags.gmac_tran_rel_state
===
===
===========================================================================*/
void gmac_tran_rel_state_handle_rel_req( release_tbf_t tbf_rel_type, gas_id_t gas_id );
#endif /*FEATURE_GSM_DTM*/

/*===========================================================================
===
===  FUNCTION      gmac_tran_get_tbfs_active()
===
===  DESCRIPTION
===   This function can be called externally to transfer in
===   order to determine which TBFs are active.  Note: A TBF is only
===   active when L1 has indicated that it has been connected.
===   This function should only be called when in Transfer
===   mode.  It will send an ERROR F3 if not.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===    boolean - TRUE DL TBF only is active
===              FALSE UL or UL+DL TBFs are active
===
===  SIDE EFFECTS
===  NONE
===
===========================================================================*/
active_tbf_t gmac_tran_get_tbfs_active( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      transfer_mode_handler
===
===  DESCRIPTION
===  Handles the signal reception and the associated actions
===  for transfer mode states.
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
void transfer_mode_handler(uint8 task_id, uint8 signal_id, gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION    gmac_init_transfer_flags()
===
===  DESCRIPTION
===  Initialises the transfer flags, should only be called once at power on.
===
===  PARAMS
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_init_transfer_flags( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      gmac_transfer_dl_release_clean
===
===  DESCRIPTION
===  When DL released, clean up any outstanding DL memory.
===
===  DEPENDENCIES
===  gmac_local_pdan_info_store_ptr - if not NULL, free pointer
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_transfer_dl_release_clean(gas_id_t gas_id);


/*===========================================================================
===
===  FUNCTION    gmac_init_tran_dl_data()
===
===  DESCRIPTION
===  Initialises gmac transfer dl data.  Used for Lower layer resets.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_init_tran_dl_data( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION    gmac_init_tran_ul_data()
===
===  DESCRIPTION
===  Initialises gmac transfer ul data.  Used for Lower layer resets.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_init_tran_ul_data( gas_id_t gas_id );


/*===========================================================================
===
===  FUNCTION    gmac_init_transfer_data()
===
===  DESCRIPTION
===  Initialises the transfer data, used for lower layer resets.
===
===  PARAMS
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_init_transfer_data( gas_id_t gas_id );
/*===========================================================================
===
===  FUNCTION    gmac_transfer_get_transfer_state()
===
===  DESCRIPTION
===  This is an access function for external entities to get the mac_transfer_substate.
===
===  PARAMS
===
===
===  RETURN VALUE
===  gprs_mac_transfer_substates_t
===
===  SIDE EFFECTS
===
===
===========================================================================*/
gprs_mac_transfer_substates_t gmac_transfer_get_transfer_state(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION    gmac_init_transfer_data_non_zero()
===
===  DESCRIPTION
===  Initialises non zero (including all enums) transfer data.
===
===  PARAMS
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_init_transfer_data_non_zero( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION    gmac_init_tran_dl_data_non_zero()
===
===  DESCRIPTION
===  Initialises non zero (including all enums) transfer dl data.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_init_tran_dl_data_non_zero( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION    gmac_tran_send_mac_l1_release() 
===
===  DESCRIPTION
===  Send MAC_L1_RELEASE to L1 to release its physical resources 
===
===  PARAMS
===
===    tbf_release_desc: UL or|and DL TBF release indicator
===
===  RETURN VALUE
===
===    
===  SIDE EFFECTS
=== 
=== 
===========================================================================*/
void gmac_tran_send_mac_l1_release(release_tbf_t tbf_release_desc,
                                   gas_id_t gas_id);

#endif /* INC_GMACTRANSFER_IF_H */
/*** EOF: don't remove! ***/

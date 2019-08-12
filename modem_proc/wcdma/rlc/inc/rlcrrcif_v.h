#ifndef RLCRRCIF_V_H
#define RLCRRCIF_V_H

/*===========================================================================
              R L C - R R C  I N T E R F A C E  H E A D E R  F I L E

DESCRIPTION
  This file contains data types and declarations associated with the
  RLC - RRC Interface.
  
Copyright (c) 2001 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

Copyright (c) 2002-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rlc/inc/rlcrrcif_v.h#1 $

when      who     what, where, why
--------  ---     ----------------------------------------------------------
04/30/15  kv      To enable BFI if data is present in WM after Reset Ack is received
12/10/13  kc      Send status PDU for 3 TTIs to optimize DCH to FACH transition
03/21/13  kc      Added support to indicate CTRL info availability in rlc_check_pending_sdu api
07/24/12  geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
04/26/12  grk     Mainlined code under FEATURE_INACTIVITY_HANDLING.
02/01/12  grk     Feature cleanup
01/18/11  vp      Removed the definition of rlc_pending_hfn_req_type
01/17/11  vp      Moved the Wcdma only declarations from rlcrrcif.h to _v.h
12/02/10  geg     Created file

===========================================================================*/

/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/

#include "wcdma_variation.h"
#include "rlcrrcif.h"

/*===========================================================================

FUNCTION RLC_RRC_GET_HFN_REQ

DESCRIPTION
   This function is called by RRC to get latest HFN's for all established
   RLC UM and AM radio-bearers.  RLC populates the * to struct 
   rlc_hfn_req_type.

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS:  If HFN's for all UM and AM rb's are populated.
  FAILURE:  If no UM and AM rb's are present.

SIDE EFFECTS
  None.

===========================================================================*/
uecomdef_status_e_type rlc_rrc_get_hfn_req
(
  rlc_hfn_req_type* hfn_req             /*ptr to hfn request type          */
);

/*===========================================================================

FUNCTION RLC_RRC_GET_COUNT_C_REQ

DESCRIPTION
   This function is called by RRC to get latest COUNT_C's for all established
   RLC UM and AM radio-bearers.  RLC populates the * to struct 
   rlc_count_c_req_type.

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS:  If COUNT_C's for all UM and AM rb's are populated.
  FAILURE:  If no UM and AM rb's are present.

SIDE EFFECTS
  None.

===========================================================================*/
uecomdef_status_e_type rlc_rrc_get_count_c_req
(
  rlc_count_c_req_type* count_c_req             /*ptr to hfn request type          */
);

/*===========================================================================

FUNCTION RLC_QUERY_DATA_CHANNEL_ACTIVITY

DESCRIPTION
   This function is called by RRC to check if there is any activity going
   on in the user plane.

DEPENDENCIES
  None.

RETURN VALUE
  DATA_TRANSFERRED: If there is any activity going on.
  NO_DATA_TRANSFERRED: Whenever RRC resets the flag.

SIDE EFFECTS
  None.

===========================================================================*/

rlc_data_activity_type rlc_query_data_channel_activity
(
  void
);

/*===========================================================================

FUNCTION RLC_RESET_ACTIVITY_FLAG

DESCRIPTION
   This function is called by RRC to set the activity flag to 
   NO_DATA_TRANSFERRED.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void rlc_reset_activity_flag
(
  void
);


/*===========================================================================

FUNCTION RLC_RRC_UL_RLC_SN_REQ

DESCRIPTION
   This function is called by RRC to get Sequence numbers for a 
   particular RB or for a set of RB's.  The sn_req_type * is
   populated by RLC for all the RB's requested by RRC.

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS:  If SN's for all requested RB's are populated.
  FAILURE:  If unable to populate SN for requested RB's.

SIDE EFFECTS
  None.

===========================================================================*/
uecomdef_status_e_type rlc_rrc_ul_rlc_sn_req
(
  rlc_ul_sn_req_type* sn_req            /*ptr to SN request type           */
);

/*===========================================================================

FUNCTION RLC_RRC_GET_WM_INFO_REQ

DESCRIPTION
   This function is called by RRC to get number of bytes in RLC Watermark queue.

DEPENDENCIES
  None.

RETURN VALUE
  RLC returns a uint32 which is the number of bytes in the Watermark for this
  (num_bytes_in_wm)+(LI*NUM_OF_SDU)

SIDE EFFECTS
  None.

===========================================================================*/
uint32 rlc_rrc_get_wm_info_req
(
  rlc_lc_id_type lc_id             /* logical channel ID for which length of
                                      WM is needed                          */
);

#ifdef FEATURE_SMC_SRB2_SUSPENSION
/*===========================================================================

FUNCTION  RLC_GET_SRB_UL_CIPH_STATUS

DESCRIPTION

  Response to RRC's query for ciphering status.

DEPENDENCIES
  None

RETURN VALUE
  RLC_NO_CIPHER_CFG_PENDING, no ciphering config pending.
  RLC_CIPHER_CFG_PENDING, yes, there is at least one.
  RLC_CIPHER_STATUS_UNSPECIFIED, can not find the LC.
  

SIDE EFFECTS
  None.

===========================================================================*/
rlc_cipher_cfg_pending_e_type rlc_get_srb_ul_ciph_status
(
  rlc_lc_id_type  rlc_id,       /* RLC LC ID for SRB2                     */
  uint32          *vt_s,        /* Current VT(S) of UL SRB2               */
  uint32          *act_sn,      /* Act SN of pendinf config,if there is   */
                                /* something pending                      */ 
  uint32          *wm_size      /* WM size in terms of number of RLC PDUs */
);
#endif //FEATURE_SMC_SRB2_SUSPENSION

/*===========================================================================

FUNCTION RLC_RRC_GET_LATEST_PENDING_HFN_REQ

DESCRIPTION
   This function is called by RRC to get latest pending HFN for indicated 
   logical channel ID.  
   RLC populates the info to the struct rlc_pending_hfn_req_type.
   If there is a pending configuration for UL alone then 
   dl_pending_hfn_present flag will be set to FALSE and vice-versa.

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS/FAILURE:  If either dl_pending_hfn_present or ul_pending_hfn_present
                    flag is set to TRUE then function will return SUCCESS.

SIDE EFFECTS
  None.

===========================================================================*/
uecomdef_status_e_type rlc_rrc_get_latest_pending_hfn
(
  rlc_lc_id_type rlc_id,                /* queried RLC logical channel ID */
  rlc_pending_hfn_req_type *pending_hfn
);

/*===========================================================================

FUNCTION RLC_RRC_OVERWRITE_HFN

DESCRIPTION
   This function is called by RRC to overwrite the HFN of RLC currently using.

   This function will be called after RRC re-establish RLC to overwrite the 
   new HFN that RRC just comes up before the traffic can be resumed.

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS/FAILURE:  Done successful / no such LC or no ciphering at all for
                    this LC.

SIDE EFFECTS
  None.

===========================================================================*/
uecomdef_status_e_type rlc_rrc_overwrite_hfn
(
  rlc_lc_id_type rlc_id,                /* queried RLC logical channel ID */
  uint32 hfn,
  rlc_reestablish_e_type direction
);

/*===========================================================================

FUNCTION crlc_ul_perform_suspend_or_stop_functionality

DESCRIPTION
  Suspends or stop one or more uplink unacknowledged or acknowledged mode channels.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void crlc_ul_perform_suspend_or_stop_functionality
( 
  rlc_ul_stop_suspend_request_type *suspend_stop_ptr,
  rlc_ul_stop_suspend_cnf_type *crlc_suspend_stop_cnf
);

/*===========================================================================

FUNCTION crlc_dl_perform_stop_functionality

DESCRIPTION
  Stops one or more downlink unacknowledged or acknowledged mode channels.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/

void crlc_dl_perform_stop_functionality
(
  rlc_dl_stop_request_type *rrcsmc_dl_stop_req
);

/*===========================================================================

FUNCTION rlc_rrc_req_to_register_for_data_ind

DESCRIPTION
  Informs RRC about the list of logical channels registered for watermark 
  in Cell_PCH/Ura_PCH states.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlc_rrc_req_to_register_for_data_ind
(
  rlc_ul_register_srvc_type *ul_ptr
);

/*===========================================================================

FUNCTION RLC_CHECK_PENDING_SDU

DESCRIPTION
  Checks if there is any data pending for a given LC, and if there are
  any pending ACKs awaited.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if data is available or ACK is awaited
  otherwise FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean rlc_check_pending_sdu
(
  rlc_lc_id_type lc_id,
  boolean *l2_int_data
);

#ifdef FEATURE_DUAL_SIM_WCDMA_TUNEAWAY
/*===========================================================================

FUNCTION RLC_CHECK_PENDING_DL_SDU

DESCRIPTION
  Checks if there are any PDUs yet to be received or processed
  for a complete SDU.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if more PDUs are expected for a SDU
  FALSE - otherwise 
  
SIDE EFFECTS
  None.

===========================================================================*/

boolean rlc_check_pending_dl_sdu
(
  rlc_lc_id_type lc_id
);
#endif

boolean rlc_rrc_set_status_pdu_tx_cnt(rlc_lc_id_type lc_id, uint8 set_count);

#endif /* RLCRRCIF_V_H */


#ifndef RRCLLC_H
#define RRCLLC_H
/*===========================================================================

                    L3 - RRC Lower Layer Controller Interface

DESCRIPTION
  This file contains global declarations and external references
  for the RRC Lower Layer Controller (LLC) interface.
  

  The functions that are to be called from external modules are:
  
  - rrcllc_init(void)

    This function initializes the RRC LLC command queues, the BCH data base,
    the CCTrCH Ids, the Lower Layer confirm/indication pending flag, and the 
    ordered config data base.
  
    This function should only be called when RRC is being initialized,
    i.e. after a power up reset, or during an RRC reset.
  
    There is no return value.
  
    See the comments in the function for more detailed information.

  - rrcllc_process_cmd(rrc_cmd_type *cmd_ptr)

    This function is called whenever there is a command for LLC to 
    process. A list of command types that can be processed by LLC are:
    
    - All *_req and *_rsp types going to L1, MAC, and RLC that are generated
      internally by other RRC procedures.
      
    - All *_cnf and *_ind types coming from L1, MAC, and RLC that are
      required by other procedures.
      
    - The general flow (in psuedo code) of processing is as follows:
    
      // This code in rrcllc_init()
      lower_layer_confirm_is_pending = FALSE;
      
      rrcllc_process_cmd(cmd_ptr)
      {
        if(cmd_ptr == Internal_command)
        {
          llc_cmd_q_ptr->contents = cmd_ptr->contents;
        }

        else if (cmd_ptr->cmd_id == Lower_Layer_cnf_or_ind)
        {
          // process can change lower_layer_confirm_is_pending to FALSE
          // if a *_cnf is received.
          rrcllc_process_*_cnf_or_ind(cmd_ptr);
        }    
        
        while (llc_cmd_q_ptr != NULL && !lower_layer_confirm_is_pending)
        {
            // process can change lower_layer_confirm_is_pending to TRUE
            rrcllc_process_int_cmd();

        } // end while

      } // end rrcllc_process_cmd()

    There is no return value.
    
    See the comments in the function for more detailed information.

   - rrcllc_set_ordered_config(rrc_state_e_type next_state, 
                              uint32 dl_sdu_num, void *msg_ptr)
  
    This function takes a pointer to the ASN.1 decoded message, the next
    state, and the downlink SDU number as parameters. It then fills in the 
    ordered config database. After the order config data base is filled in,
    status is then returned to the calling function as to the success
    or failure to fill in the ordered config database.

    The return type is of type rrcllc_oc_status_e_type. This indicates
    if ORDERED_CONFIG is set and if not, what the current failure
    status is. The calling procedure then needs to act accordingly.
    
    Once ORDERED_CONFIG is set, rrcllc_channel_config() is called with
    LLC using the newly programed data to configure the lower layers with
    ORDERED_CONFIG.
    
    ORDERED_CONFIG is created.....

    * Idle-Disconnected to Idle-Connecting

    ************ Procedure calls rrcllc_chan_config_req() *************
    1) Copy SIB5 data to CURRENT_CONFIG (Note SIB5 data is already in
       CURRENT_CONFIG, courtesy the PCH setup), unless a SIB change
       has occurred.
    2) Config Lower Layers (via primitives with CURRENT_CONFIG_PTR),
       note this is for PRACH/RACH/CCCH Uplink only, while enabling
       FACH Transport blocks, i.e. RLC-UL, MAC-UL, and PHYCHAN-PRACH.
    3) Wait for Lower Layer confirms
    4) Send Channel Confirm to RRC Connection Establishment

    * Idle-Connecting to CELL_DCH

    ************ Procedure calls rrcllc_set_ordered_config() **********
    1) ORDERED_CONFIG = CURRENT_CONFIG
    2) Extract OTA Msg IEs and overwrite those values included in Msg.
    3) ordered_config_set = TRUE
    ************ Procedure calls rrcllc_chan_config_req() *************
    1) Config Lower Layers (via primitives with ORDERED_CONFIG_PTR)
    2) Wait for Lower Layer confirms
    3) Change State (to CELL_DCH)
    4) Send Channel Confirm to RRC Connection Establishment
    *********** Procedure calls rrcllc_clear_ordered_config() *********
    1) ordered_config_set = FALSE
    2) CURRENT_CONFIG_PTR = ORDERED_CONFIG_PTR
    ************ Receive an RB Setup msg & stay in CELL_DCH ***********
    ************ Procedure calls rrcllc_set_ordered_config() **********
    1) ORDERED_CONFIG = CURRENT_CONFIG
    2) Extract OTA Msg IEs and overwrite those values included in Msg.
    3) ordered_config_set = TRUE
    ************ Procedure calls rrcllc_chan_config_req() *************
    1) Config Lower Layers (via primitives with ORDERED_CONFIG_PTR)
    2) Wait for Lower Layer confirms
    3) Change State (to CELL_DCH)
    4) Send Channel Confirm to RB Setup
    *********** Procedure calls rrcllc_clear_ordered_config() *********
    1) ordered_config_set = FALSE
    2) CURRENT_CONFIG_PTR = ORDERED_CONFIG_PTR

    * End CELL_DCH transitions with RB setup.
    
    * Idle-Connecting to CELL_FACH (Not implemented or complete, just
      for reference now).

    ************ Procedure calls rrcllc_set_ordered_config() **********
    1) ORDERED_CONFIG = CURRENT_CONFIG
    2) If SIB change Overwrite ORDERED_CONFIG with SIB5 updates.
    3) Extract OTA Msg IEs and overwrite those values included in Msg.
    4) ordered_config_set = TRUE
    ************ Procedure calls rrcllc_chan_config_req() *************
    1) Config Lower Layers (via primitives with ORDERED_CONFIG_PTR)
    2) Wait for Lower Layer confirms
    3) Change State (to CELL_FACH)
    4) Send Channel Confirm to RRC Connection Establishment
    *********** Procedure calls rrcllc_clear_ordered_config() *********
    1) ordered_config_set = FALSE
    2) CURRENT_CONFIG_PTR = ORDERED_CONFIG_PTR

  - rrcllc_clear_ordered_config(void)

    This function will clear(reset) the ordered_config_set flag. It also
    swaps the ordered_config_ptr and current_config_ptr.

    The following steps occur:
    1) ordered_config_set = FALSE;
    2) temp_ptr = current_config_ptr
    3) current_config_ptr = ordered_config_ptr
    4) ordered_config_ptr = temp_ptr
   
  Functions that can be called from other rrcllc modules are:

  - rrcllc_init_ordered_config()
    
    This function is called by rrcllc_init(). This function initializes
    the ordered config and current config pointers and data inside 
    the ordered config and current config data bases.

  - rrcllc_channel_config_req(rrc_cmd_type *cmd_ptr)
  
    This function will take a pointer to an RRC command packet, ensure that
    it is an RRC_CHAN_CONFIG_REQ command, and then configure the requested
    radio bearers for the next state.

Copyright (c) 2000-2004, 2006-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcllc.h_v   1.24   13 Jun 2002 16:06:04   vnagpal  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/inc/rrcllc.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/16   nr      Made changes to remove F3 in rrcllc_get_ordered_config_state_and_proc 
10/16/15   as      Added code to support LTA durting activation time
08/08/14   sg      Made changes to check URNTI in TOC also if it is not 
                   present on config ptr in use
06/17/14   sp      Made changes to mainline DCH-FACH transition optimization changes
06/04/14   as      Made changes for DCH-FACH transition time optimization
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
05/30/13   db      Made changes to ignore Cu-Cnf when RAB release is in progress
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
04/06/12   ad      Changes for HS-RACH feature
09/27/11   ad      Fixed compilation warnings
09/16/11   ad      Added code to include the reconfig_status indicator in a 
                   corner scenario where TOC is reset from TOC_FOR_OOS to TOC_FOR_DCH
04/26/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.96.50
04/20/11   vg      added support for mdsp halt recovery
03/14/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.93.50
03/09/11   ssg     MAC-ehs support on Apollo.
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
04/26/10   ss      Added code changes for Fast Dormancy feature under feature flag
                   FEATURE_3GPP_FAST_DORMANCY
11/26/09   dm      Added function to check whether S-BCCH is up.
06/23/09   ps      Made changes to ignore L2 ack for CRLC REL all req
06/03/09   rm      Removed prototype for rrcllc_get_substate
05/11/09   sks     Made changes to supress compilation errors for RRC UTF.
05/11/09   rm      Added prototype for rrcllc_get_substate
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
11/28/08   gkg     Made Changes for new MVS-RRC Interface to (de)configure vocoder
                   for AMR Voice Calls.
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7. Removed
                   feature flag FEATURE_CELLPCH_URAPCH
09/15/08   ns      Made changes to calculate the activation time using the CFN of the
                   new cell, and thereby ensure that the activation time sent is at least 200
                   frames ahead of CFN. 
07/21/08   rm      Added Prototype for rrcllc_process_chan_config_request
01/04/08   gkg     Added support for asymmetric AMR call by adding dl_amr_mode in
                   rrcllc_amr_mapping_info_type. 
20/03/08   gkg     To support rrcllc_derive_dl_amr_mode() moved MAX_DCH_VOICE_CHANNEL
                   out of WB AMR feature.
03/19/08   rm      Added prototype for rrcllc_check_if_chan_config_in_progress_for_sib5_change
03/06/08   da      Changed FEATURE_MBMS to FEATURE_MODEM_MBMS
02/13/08   rm      Added prototype for rrcllc_get_rach_cctrch_ptr
12/21/07   da      Support for FEATURE_MBMS
09/10/07   ps      Added support for Frequency redirection received in 
                   FACH->FACH and FACH->PCH cases under feature flag
                   FEATURE_RRC_FREQ_REDIRECTION_THRU_RB_AND_CU
07/17/07   rm      Added support for FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING feature.
06/04/07   kp      Added a function which will return status of N-BCCH setup.
                   It looks at semi permanent data & LCM status.   
03/02/06   ps      Added support for Wb-AMR under the feature flag
                   FEATURE_VOC_AMR_WB.
11/23/06   kp      SMC suspend request & confirmation will be through call back function 
                   with RLC, Cleaning up code for command interface.
10/13/06   sgk     Added 7600 support under feature
                   FEATURE_UMTS_1X_HANDOVER_UMTSMSM
08/28/06   da      Added support for OOS area handling during transition from DCH.
02/15/06   sgk     Added functions to check remaining activation time, going
                   back to old config and return llc substate. Included llc 
                   substate typedef in the file.
12/04/04   svk     Added prototype for rrcllc_update_lbt_mode_for_fachs() under
                   FEATURE_TC
08/20/04   jh      Added data members for enhanced AMR processing.
07/14/04    sk     Added prototypes for new function to reset OC and 
                   to get the proc that set oc
02/20/04   bu      Added prototype for rrcllc_error_recovery_in_progress().
02/06/04   jh      Added PCH-related values to rrcllc_oc_set_status_e_type.
12/11/03   vn      Added prototype for rrcllc_set_ordered_config_for_rl_failure
11/13/03   jh      Add prototypes for rrcllc_get_ordered_config_process_state()
                   and rrcllc_reconfig_in_progress().
05/01/03   kc      Added prototype for rrcllc_update_ciphering_config_for_handover().
04/21/03   vn      Added prototype for new fn rrcllc_can_oc_be_cleared_now.
03/18/03   vn      Removed OC_SET_FOR_FACH_DCH_TRANS as it is no longer needed.
02/28/03   vn      Removed declaration for rrc_ul_tx_pwr_ind() as that is 
                   declared now in l1rrcif.h
02/18/03   vn      Added new functions rrcllc_return_psc_for_fach_pch and
                   rrcllc_return_freq_for_fach_pch for supported directed cell
                   transitions.
02/05/03   vn      Merged in changes dropped during linting.
01/10/03   vn      Added new parameter proc_id in rrcllc_set_ordered_config.
                   Used to maintain procedure ID of the proc setting OC status.
09/24/02   vn      Added prototype for rrcllc_swap_oc_and_cc. Added new types
                   to the enum rrcllc_oc_set_status_e_type for DCH->FACH
                   transitions.
09/25/02   xfg     Added support for WVS to MVS migration. The change was under
                   FEATURE_MVS.
09/20/02   vn      Support for cell reselection in Connecting and CELL_FACH.
                   Added rrcllc_oc_set_status_e_type and prototype for
                   rrcllc_clear_reselection_ordered_config(). Changed
                   rrcllc_is_ordered_config_set() to rrcllc_get_ordered_config_status().                                    
06/12/02   vn      Merged in Dummy signalling support for LBT.
06/12/02   rj      Added prototype for rrcllc_get_current_urnti to provide
                   Current URNTI.
06/03/02   vn      Added function prototype for rrcllc_update_sib1_data to
                   update SIB1 data in Current and Ordered Config.
03/04/02   vn/bu   Added prototype for fn rrcllc_get_current_crnti_status. 
                   Defined rrcllc_crnti_status_e_type enum.
02/07/02   vn      Added prototype for rrcllc_update_lbt_mode_for_dchs for
                   Loopback Test mode support.
01/15/02   vn      Modified rrcllc_get_amr_mapping to return a status.
12/05/01   rj      Added new enum to rrcllc_oc_status_e_type for invalid
                   configuration for June conversion.
10/26/01   vn      Added a new RRCLLC_RE_INIT_FREE_Q to init_e_types. Needed 
                   for Deactivateion support.
08/27/01   kmp     Added prototype for rrcllc_get_drx_info().
06/15/01   vn      Added rrcllc_init_e_type to define type of LLC init. Changed
                   prototype of function rrcllc_init to accept rrcllc_init_e_type
                   as a parameter.
05/14/01   kmp/ram Added prototype for rrcllc_get_amr_mapping(). Added
                   structure rrcllc_amr_mapping_info_type.
05/04/01   ram     Added prototype rrcllc_is_ordered_config_set which checks
                   if ordered_config is set or not.
04/02/01   kmp     Updated to add uplink power data structure to Ordered
                   Config.
03/30/01   rj      Changed dl_sdu_num to uint32 from uint8 to be compatible
                   with latest LED based rrcasn1.h
03/21/01   kmp     Updated to add prototypes for rrcllc_rest_l1_req() and
                   rrcllc_get_tm_ccch_rlc_size(). Added
                   #define RRCLLC_INVALID_RLC_SIZE 0xFFFFFFFF.
02/02/01   kmp     Updated to add the next_state to rrcllc_set_ordered_config().
                   Added new function rrcllc_clear_ordered_config();
01/23/01   kmp     Updated to new interface for LLC to funnel all commands to
                   L1, MAC and RLC through LLC.
01/09/01   ram     Removed duplicate definitions of BCCH_S_RADIO_BEARER_ID,
                   BCCH_N_RADIO_BEARER_ID and PCCH_RADIO_BEARER_ID. These are
                   now defined in uecomdef.h.
12/12/00   kmp     Changed names of rrcllc_connected_mode_chan_config()
                   to rrcllc_cm_channel_config(). Also added structure for 
                   pcch data.
12/08/00   ram     Updated to the new command queue interface for RRC. Moved
                   command queue related data structures to rrcintif.h.
12/01/00   kmp     Updated for rex++. Changed Events to commands. Added
                   pch_setup() and pch_release() functions. Updated for new
                   ORDERED_CONFIG prototypes and enumerated values.
11/09/00   kmp     Updated for review comments.
11/07/00   kmp     Updated to change the init_bch_data_base() to rrcllc_init_
                   bch_data_base(). Removed rrc_config_type and added
                   #include "uecomdef.h".
10/30/00   kmp     Initial Release was on 10/27/00. Updated now to include
                   new improved MAC, RLC, L1, and RRC interfaces.

===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "rrccmd_v.h"
#include "uecomdef.h"

/*===================================================================
                        DATA DECLARATIONS
=====================================================================*/


/*===================================================================
                        CONSTANTS
=====================================================================*/
/*-------------------------------------------------------------------
This define is an error condition when no valid RLC PDU size is 
availble for TM UL transmissions.
--------------------------------------------------------------------*/
#define RRCLLC_INVALID_RLC_SIZE     0xFFFFFFFFUL


/*------------------------------------------------------------------
This enumerates all possibile use types for TOC
-------------------------------------------------------------------*/
typedef enum
{
  TOC_FOR_DCH_AND_FACH,
  TOC_FOR_DCH,
  TOC_FOR_OOS,
/* This is applicable for OOS triggered during Frequency redirection 
  * for FACH->FACH and FACH->PCH/URA PCH
  */
  TOC_FOR_OOS_WITH_DCH_INFO, 
  TOC_FOR_OOS_WITHOUT_DCH_INFO,
  /*This is applicable for Frequency redirection for FACH->FACH 
      and FACH->PCH*/
  TOC_FOR_FACH_AND_PCH,
  TOC_FOR_FACH_AND_PCH_WITH_DCH_INFO,
  
  TOC_INVALID
}rrcllc_toc_usage_e_type;


/*-------------------------------------------------------------------
ENUM: rrcllc_oc_status_e_type

This enum defines the return status for rrcllc_set_ordered_config().
--------------------------------------------------------------------*/
typedef enum
{
  ORDERED_CONFIG_SET,
  ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG,
  ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED,
  ORDERED_CONFIG_NOT_SET_OTHER,
  ORDERED_CONFIG_CONFIGURATION_INVALID,
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
  ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED,
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
  MAX_ORDERED_CONFIG_STATUS_TYPES
} rrcllc_oc_status_e_type;

/*-------------------------------------------------------------------
ENUM: rrcllc_oc_set_status_e_type

This enum defines the various status values that OC can be set to.
--------------------------------------------------------------------*/
typedef enum
{
  OC_SET_FOR_CELL_FACH,
  OC_SET_FOR_CELL_DCH,
  OC_SET_FOR_URA_PCH,
  OC_SET_FOR_CELL_PCH,
  OC_SET_FOR_DCH_FACH_TRANS,
  OC_SET_FOR_DCH_CELL_PCH_TRANS,
  OC_SET_FOR_DCH_URA_PCH_TRANS,
  OC_SET_FOR_FACH_CELL_PCH_TRANS,
  OC_SET_FOR_FACH_URA_PCH_TRANS,
  OC_NOT_SET
}rrcllc_oc_set_status_e_type;

#define rrcllc_oc_set_status_e_type_value0 OC_SET_FOR_CELL_FACH,
#define rrcllc_oc_set_status_e_type_value1 OC_SET_FOR_CELL_DCH,
#define rrcllc_oc_set_status_e_type_value2 OC_SET_FOR_URA_PCH,
#define rrcllc_oc_set_status_e_type_value3 OC_SET_FOR_CELL_PCH,
#define rrcllc_oc_set_status_e_type_value4 OC_SET_FOR_DCH_FACH_TRANS,
#define rrcllc_oc_set_status_e_type_value5 OC_SET_FOR_DCH_CELL_PCH_TRANS,
#define rrcllc_oc_set_status_e_type_value6 OC_SET_FOR_DCH_URA_PCH_TRANS,
#define rrcllc_oc_set_status_e_type_value7 OC_SET_FOR_FACH_CELL_PCH_TRANS,
#define rrcllc_oc_set_status_e_type_value8 OC_SET_FOR_FACH_URA_PCH_TRANS,
#define rrcllc_oc_set_status_e_type_value9 OC_NOT_SET

/*-------------------------------------------------------------------
ENUM: rrcllc_oc_process_state_e_type

This enum defines the various process states for Ordered Config.
--------------------------------------------------------------------*/
typedef enum
{
  LL_CONFIG_WITH_OC,
  LL_NOT_CONFIG_WITH_OC
}rrcllc_oc_process_state_e_type;

/*-------------------------------------------------------------------
ENUM: rrcllc_init_e_type

This enum defines the possible RRC-LLC initialization types.
--------------------------------------------------------------------*/
typedef enum
{
  RRCLLC_STARTUP_INIT,
  RRCLLC_RE_INIT,
  RRCLLC_RE_INIT_FREE_Q,
  RRCLLC_INIT_PHY_CHL_DATA
} rrcllc_init_e_type;

#define RRC_DCH_FACH_TRANISITION_CHANNEL_INVALID 0x1
/*CCCH and bcch on fach/HS setup*/
#define RRC_DCH_FACH_TRANISITION_CHANNEL_SETUP 0x2

#define RRC_DCH_FACH_TRANISITION_SKIP_CHANNEL_SETUP 0x4


/*-------------------------------------------------------------------
ENUM: rrcllc_crnti_status_e_type

This enum defines the possible status for C_RNTI
--------------------------------------------------------------------*/
typedef enum
{
  C_RNTI_VALID,
  C_RNTI_NOT_VALID
}rrcllc_crnti_status_e_type;

/*-------------------------------------------------------------------
ENUM: rrcllc_urnti_status_e_type

This enum defines the possible status for U_RNTI
--------------------------------------------------------------------*/
typedef enum
{
  U_RNTI_PRESENT,
  U_RNTI_NOT_PRESENT
}rrcllc_urnti_status_e_type;

/*-------------------------------------------------------------------
This is an enumerator that lists all possible states of LLC state
machine.
--------------------------------------------------------------------*/
typedef enum
{
  /* LLC Idle state - accepts all RRC internal commands */
  LLC_IDLE,
  LLC_WAIT_CPHY_SETUP_CNF,
  LLC_WAIT_CPHY_CHANNEL_IND,
  LLC_WAIT_CPHY_IN_SYNC_IND,
  LLC_WAIT_CPHY_IDLE_CNF,
  LLC_WAIT_CPHY_DEACTIVATE_CNF,
  LLC_WAIT_CPHY_ACQ_CNF,
  LLC_WAIT_CPHY_FREQ_SCAN_CNF,
  LLC_WAIT_CPHY_CAPABILITY_CNF,
  LLC_WAIT_CPHY_CELL_SELECTION_CNF,
  LLC_WAIT_CPHY_CELL_TRANSITION_CNF,
  LLC_WAIT_CPHY_IMMEDIATE_MEAS_CNF,
  LLC_WAIT_CPHY_INTEGRITY_KEY_MAUC_CNF,
  LLC_WAIT_CPHY_IMMEDIATE_RESELECTION_CNF,
  LLC_WAIT_MAC_UL_HFN_CNF,
  LLC_WAIT_MAC_DL_HFN_CNF,
  LLC_WAIT_CPHY_MEAS_CTRL_CNF,
  /* Waiting for CRLC_CONFIG_TM_UL_CNF */
  LLC_WAIT_RLC_TM_UL_CNF,
  LLC_WAIT_RLC_TM_DL_CNF,
  LLC_WAIT_RLC_UM_UL_CNF,
  LLC_WAIT_RLC_UM_DL_CNF,
  LLC_WAIT_RLC_AM_CNF,
  LLC_WAIT_RLC_UL_HFN_CNF,
  LLC_WAIT_RLC_DL_HFN_CNF,
  LLC_WAIT_RLC_REL_ALL_CNF,
  LLC_WAIT_NEW_CELL_IND,

  /* Waiting for CPHY_STOP_WCDMA_MODE_CNF */
  LLC_WAIT_CPHY_STOP_WCDMA_MODE_CNF,
  /* Waiting for RRC_CPHY_SUSPEND_WCDMA_MODE_CNF */
  LLC_WAIT_CPHY_SUSPEND_WCDMA_MODE_CNF,
  /* Waiting for RRC_CPHY_RESUME_WCDMA_MODE_CNF */
  LLC_WAIT_CPHY_RESUME_WCDMA_MODE_CNF,

  LLC_WAIT_FOR_STATE_CHG_IND,
  /* New states for Ciphering */
  LLC_WAIT_RRC_RESUME_RLC_RB_REQ,
  LLC_WAIT_RRC_CRLC_UL_RESUME_CNF,
  LLC_WAIT_CPHY_DRX_CNF,
  /* Waiting for CPHY_START_WCDMA_MODE_CNF */
  LLC_WAIT_CPHY_START_WCDMA_MODE_CNF,
  LLC_WAIT_CPHY_ACT_TIME_CANCEL_CNF ,
#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
  #error code not present
#endif
#ifdef FEATURE_WCDMA_HANDLE_MDSP_HALT
  LLC_WAIT_CPHY_RECOVER_CNF,
#endif
  LLC_STATES_MAX
}rrcllc_substate_e_type;
/* Below #defines are added for logging purpose only */
  #define rrcllc_substate_e_type_value0 LLC_IDLE,
  #define rrcllc_substate_e_type_value1 LLC_WAIT_CPHY_SETUP_CNF,
  #define rrcllc_substate_e_type_value2 LLC_WAIT_CPHY_CHANNEL_IND,
  #define rrcllc_substate_e_type_value3 LLC_WAIT_CPHY_IN_SYNC_IND,
  #define rrcllc_substate_e_type_value4 LLC_WAIT_CPHY_IDLE_CNF,
  #define rrcllc_substate_e_type_value5 LLC_WAIT_CPHY_DEACTIVATE_CNF,
  #define rrcllc_substate_e_type_value6 LLC_WAIT_CPHY_ACQ_CNF,
  #define rrcllc_substate_e_type_value7 LLC_WAIT_CPHY_FREQ_SCAN_CNF,
  #define rrcllc_substate_e_type_value8 LLC_WAIT_CPHY_CAPABILITY_CNF,
  #define rrcllc_substate_e_type_value9 LLC_WAIT_CPHY_CELL_SELECTION_CNF,
  #define rrcllc_substate_e_type_value10 LLC_WAIT_CPHY_CELL_TRANSITION_CNF,
  #define rrcllc_substate_e_type_value11 LLC_WAIT_CPHY_IMMEDIATE_MEAS_CNF,
  #define rrcllc_substate_e_type_value12 LLC_WAIT_CPHY_INTEGRITY_KEY_MAUC_CNF,
  #define rrcllc_substate_e_type_value13 LLC_WAIT_CPHY_IMMEDIATE_RESELECTION_CNF,
  #define rrcllc_substate_e_type_value14 LLC_WAIT_MAC_UL_HFN_CNF,
  #define rrcllc_substate_e_type_value15 LLC_WAIT_MAC_DL_HFN_CNF,
  #define rrcllc_substate_e_type_value16 LLC_WAIT_CPHY_MEAS_CTRL_CNF,
  /* Waiting for CRLC_CONFIG_TM_UL_CNF */
  #define rrcllc_substate_e_type_value17 LLC_WAIT_RLC_TM_UL_CNF,
  #define rrcllc_substate_e_type_value18 LLC_WAIT_RLC_TM_DL_CNF,
  #define rrcllc_substate_e_type_value19 LLC_WAIT_RLC_UM_UL_CNF,
  #define rrcllc_substate_e_type_value20 LLC_WAIT_RLC_UM_DL_CNF,
  #define rrcllc_substate_e_type_value21 LLC_WAIT_RLC_AM_CNF,
  #define rrcllc_substate_e_type_value22 LLC_WAIT_RLC_UL_HFN_CNF,
  #define rrcllc_substate_e_type_value23 LLC_WAIT_RLC_DL_HFN_CNF,
  #define rrcllc_substate_e_type_value24 LLC_WAIT_RLC_REL_ALL_CNF,
  #define rrcllc_substate_e_type_value25 LLC_WAIT_NEW_CELL_IND,

  /* Waiting for CPHY_STOP_WCDMA_MODE_CNF */
  #define rrcllc_substate_e_type_value26 LLC_WAIT_CPHY_STOP_WCDMA_MODE_CNF,
  /* Waiting for RRC_CPHY_SUSPEND_WCDMA_MODE_CNF */
  #define rrcllc_substate_e_type_value27 LLC_WAIT_CPHY_SUSPEND_WCDMA_MODE_CNF,
  /* Waiting for RRC_CPHY_RESUME_WCDMA_MODE_CNF */
  #define rrcllc_substate_e_type_value28 LLC_WAIT_CPHY_RESUME_WCDMA_MODE_CNF,

  #define rrcllc_substate_e_type_value29 LLC_WAIT_FOR_STATE_CHG_IND,
  /* New states for Ciphering */
  #define rrcllc_substate_e_type_value30 LLC_WAIT_RRC_RESUME_RLC_RB_REQ,
  #define rrcllc_substate_e_type_value31 LLC_WAIT_RRC_CRLC_UL_RESUME_CNF,
  #define rrcllc_substate_e_type_value32 LLC_WAIT_CPHY_DRX_CNF,
  /* Waiting for CPHY_START_WCDMA_MODE_CNF */
  #define rrcllc_substate_e_type_value33 LLC_WAIT_CPHY_START_WCDMA_MODE_CNF,
  #define rrcllc_substate_e_type_value34 LLC_WAIT_CPHY_ACT_TIME_CANCEL_CNF ,
#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
  #error code not present
#endif
#ifdef FEATURE_WCDMA_HANDLE_MDSP_HALT
  #define rrcllc_substate_e_type_value36 LLC_WAIT_CPHY_RECOVER_CNF,
#endif
  #define rrcllc_substate_e_type_value37 LLC_STATES_MAX

/*===================================================================
                        DATA STRUCTURES
====================================================================*/

/*===================================================================
*                       FUNCTION PROTOTYPES
====================================================================*/

/*====================================================================
FUNCTION: rrcllc_init()

DESCRIPTION:
  This function initializes all RRCLLC data including BCCH, PCCH and
  the ORDERED_CONFIG databases. This calls a number of other
  initialization functions.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init(rrcllc_init_e_type  init_type);

/*====================================================================
FUNCTION: rrcllc_process_cmd()

DESCRIPTION:
  This is called for the following conditions:
  
  - when an internal command is received by the RRC dispatcher and
    the commands final destination is L1, MAC or RLC. 

  - when a confirmation or indication from L1, MAC or RLC is received
    by the RRC dispatcher.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_process_cmd
(
  rrc_cmd_type   *cmd_ptr
);

/*====================================================================
FUNCTION: rrcllc_set_ordered_config

DESCRIPTION:
  This is called when the ORDERED_CONFIG variable needs to be set.
  It checks to see whether or not it is set or not, and returns
  the status of the check.

  It also takes a pointer to the asn1 decoded message stream and
  places it into the ordered_config database.

  The procedure name is required so that the message asn1 decoded
  message can be properly typecast so as to populate the 
  ORDERED_CONFIG database.

DEPENDENCIES:
  None.

RETURN VALUE:
  rrcllc_oc_status_e_type - indicates whether the ORDERED_CONFIG
                            variable could be successfully set or
                            what the failure was.

SIDE EFFECTS:
  None.
====================================================================*/
rrcllc_oc_status_e_type rrcllc_set_ordered_config
(
  /*
  * This variable indicates the PDU number as defined in the ASN.1
  * header file. This information can be used to map the to correct
  * logical channel received on and then down to the message type.
  * The message contents are in msg_ptr, and need to be typecasted.
  */
  rrc_proc_e_type   proc_id,
  rrc_state_e_type  next_state,
  uint32            dl_sdu_num,
  void             *msg_ptr
);

/*====================================================================
FUNCTION: rrcllc_swap_oc_and_cc

DESCRIPTION:
  This function swaps ordered_config_ptr and current_config_ptrs
  without altering the ordered_config_status.
  This could be called in place of rrcllc_clear_ordered_config
  when the ordered_config_status needs to be preserved.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void  rrcllc_swap_oc_and_cc(void);

/*====================================================================
FUNCTION: rrcllc_clear_ordered_config

DESCRIPTION:
  This is called when the ORDERED_CONFIG variable needs to be cleared.

  This function puts control of ORDERED_CONFIG in the hands of the 
  individual procedures. Each procedure can then decide whether or not
  they wish to wait for a layer 2 ack before clearing or just clearing
  after reciept of an lower layer configuration message.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_clear_ordered_config
(
  void
);

/*====================================================================
FUNCTION: rrcllc_clear_reselection_ordered_config

DESCRIPTION:
  This is called when the RESELECTION ORDERED_CONFIG variable needs
  to be cleared.

  This function puts control of ORDERED_CONFIG in the hands of the 
  individual procedures. Each procedure can then decide whether or not
  they wish to wait for a layer 2 ack before clearing or just clearing
  after reciept of an lower layer configuration message.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_clear_reselection_ordered_config
(
  void
);


/*====================================================================
FUNCTION: rrcllc_channel_config_req()

DESCRIPTION:
  This is called when an RRC_CHANNEL_CONFIG_REQ is received.
  This function adds, drops, or and performs an add/drop of idle 
  or connected mode channels.

DEPENDENCIES:
  None.

RETURN VALUE:
  uecomdef_status_e_type - indicates whether or not the config
                           parameters were correctly set.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_channel_config_req
(
  rrc_cmd_type   *cmd_ptr
);

/*====================================================================
FUNCTION: rrcllc_reset_l1_req()

DESCRIPTION:
  This is called when an RRC_RESET_L1_REQ is received.
  This function is called when an RRC procedure needs to send Layer
  1 to the Idle state.

DEPENDENCIES:
  None.

RETURN VALUE:
  uecomdef_status_e_type - indicates whether or not the config
                           parameters were correctly set.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_reset_l1_req
(
  rrc_cmd_type  *cmd_ptr
);

/*====================================================================
FUNCTION: rrcllc_get_ul_tm_ccch_rlc_size()

DESCRIPTION:
  This is called when the RLC size for an uplink Transparent Mode
  Channel is required. This is needed by RRC procedures (specifically
  the RRC Connection Establishment procedure) to fill in the missing
  number of bits to fit the given RLC size.

  How this works is the following way:
  * Ordered config processing takes the size of the encoded pdu
  * Ordered config then looks at the RLC sizes for all UL TM Transport
    Formats.
  * Ordered config then returns the Transport Format RLC size of the
    closest (without going under) of the encoded PDU size.
    
  * For example if the encoded PDU size is 129 bits, and the UL TM
    RLC sizes for all Transport Formats are 128 and 166, this function
    will return the RLC size of the Transport Format which equals 166.

  * If no RLC size is found that can accomidate the given encoded PDU
    size RRCLLC_INVALID_RLC_SIZE.
    
DEPENDENCIES:
  None.

RETURN VALUE:
  uint32  - The size of the RLC Transparent Mode buffer.  

SIDE EFFECTS:
  None.
====================================================================*/
uint32 rrcllc_get_ul_tm_ccch_rlc_size
(
  uint32   pdu_size
);



/*====================================================================
FUNCTION: rrcllc_calculate_ul_tm_ccch_rlc_size()

DESCRIPTION:
  This is called when the RLC size for an uplink Transparent Mode
  Channel is required. 
  This function returns the rlc size removing the MAC header size from the tansport block size

DEPENDENCIES:
  None.

RETURN VALUE:
  uint32  - The size of the RLC Transparent Mode buffer.

SIDE EFFECTS:
  Note that currently all logical channels mapped to a Transport
  channel must have the same TrBLK size. Therefore, by extracting
  the RLC size of the first Transport Format, this will apply to
  all Transport Formats for all logical channels and types mapped to
  this TrCH.
====================================================================*/
uint32 rrcllc_calculate_ul_tm_ccch_rlc_size(void);


/*====================================================================
FUNCTION: rrcllc_get_ordered_config_status()

DESCRIPTION:
  This function checks if ORDERED_CONFIG is set. If so, it
  returns TRUE. If it's not set it returns FALSE.

DEPENDENCIES:
  None.

RETURN VALUE:
  Returns a value that indicates if Ordered Config is set for
  an RRC state or not set.
  
SIDE EFFECTS:
  None.
====================================================================*/
rrcllc_oc_set_status_e_type rrcllc_get_ordered_config_status( void );

/*====================================================================
FUNCTION: rrcllc_get_ordered_config_status_wo_f3()

DESCRIPTION:
  This function checks if ORDERED_CONFIG is set. If so, it
  returns TRUE. If it's not set it returns FALSE.

DEPENDENCIES:
  None.

RETURN VALUE:
  Returns a value that indicates if Ordered Config is set for
  an RRC state or not set.

SIDE EFFECTS:
  None.
====================================================================*/
rrcllc_oc_set_status_e_type rrcllc_get_ordered_config_status_wo_f3( void );


/*====================================================================
FUNCTION: rrcllc_get_toc_usage()

DESCRIPTION:
  This function checks returns the toc_usage status

DEPENDENCIES:
  None.

RETURN VALUE:
  toc_usage status

SIDE EFFECTS:
  None.
====================================================================*/
rrcllc_toc_usage_e_type rrcllc_get_toc_usage(void);
/*====================================================================
FUNCTION: rrcllc_get_ordered_config_process_state()

DESCRIPTION:
  This function returns info on whether or not lower layers have been
  configured with the information in odered config.

DEPENDENCIES:
  None.

RETURN VALUE:
  Returns a value that indicates if lower layers have been set with the
  info in ordered config.
  
SIDE EFFECTS:
  None.
====================================================================*/
rrcllc_oc_process_state_e_type rrcllc_get_ordered_config_process_state( void );

/*====================================================================
FUNCTION: rrcllc_reconfig_in_progress()

DESCRIPTION:
  This function returns info on whether or not a reconfiguration is
  currently in progress.

DEPENDENCIES:
  None.

RETURN VALUE:
  Returns a value that indicates if a reconfiguration is in progress.
  
SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_reconfig_in_progress( void );



 /*====================================================================
FUNCTION: rrcllc_get_ordered_config_state_and_proc()

DESCRIPTION:
  This function returns oc status. It also indicates which process
  has set the OC and process status.

DEPENDENCIES:
  None.

RETURN VALUE:
  Returns a value that indicates if Ordered Config is set for
  an RRC state or not set.
  
SIDE EFFECTS:
  None.
====================================================================*/
rrcllc_oc_set_status_e_type rrcllc_get_ordered_config_state_and_proc(
                                                   rrc_proc_e_type *proc_ptr,
                                                   rrcllc_oc_process_state_e_type *process_status_ptr
                                                    );

  /*====================================================================
 FUNCTION: rrcllc_get_ordered_config_state_and_proc_wo_f3()
 
 DESCRIPTION:
   This function returns oc status. It also indicates which process
   has set the OC and process status.
 
 DEPENDENCIES:
   None.
 
 RETURN VALUE:
   Returns a value that indicates if Ordered Config is set for
   an RRC state or not set.
 
 SIDE EFFECTS:
   None.
 ====================================================================*/
 rrcllc_oc_set_status_e_type rrcllc_get_ordered_config_state_and_proc_wo_f3(
                                                    rrc_proc_e_type *proc_ptr,
                                                    rrcllc_oc_process_state_e_type *process_status_ptr
                                                     );

 /*====================================================================
FUNCTION: rrcllc_reset_ordered_config()

DESCRIPTION:
  This function is used to reset the OC

DEPENDENCIES:
  None.

RETURN VALUE:
  Returns a value that indicates if Ordered Config is set for
  an RRC state or not set.
  
SIDE EFFECTS:
  None.
====================================================================*/
rrcllc_oc_set_status_e_type rrcllc_reset_ordered_config(
                                                   rrc_proc_e_type proc_id
                                                   ,boolean reset_toc

                                                    );

/*====================================================================
FUNCTION: rrcllc_get_drx_info()

DESCRIPTION:
  This function returns the shorter of the PS or CS CN DRX Cycle
  Length.

DEPENDENCIES:
  None.

RETURN VALUE:
  uint32 of the DRX Cycle length, per 25.304.

SIDE EFFECTS:
  This function should only be called after the following function
  calls:
  - rrcllc_copy_sib_to_oc() or
  - rrcllc_set_ordered_config().
====================================================================*/
uint32 rrcllc_get_drx_info( void );

/*====================================================================
FUNCTION: rrcllc_get_drx_info_values()

DESCRIPTION:
  This function returns the shorter of the PS or CS CN DRX Cycle
  Length.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
  This function should only be called after the following function
  calls:
  - rrcllc_copy_sib_to_oc() or
  - rrcllc_set_ordered_config().
====================================================================*/
void rrcllc_get_drx_info_values
(
  uint32 *cs_drx_ptr,
  uint32 *ps_drx_ptr,
  uint32 *utran_drx_ptr
);

/*====================================================================
FUNCTION: rrcllc_set_utran_drx_info()

DESCRIPTION:
  This function sets the utran_drx_coef in to ordered_config
  data structure. This api is used when a procedure does not have
  to set ordered_config but needs to configure it with utran_drx_info

DEPENDENCIES:
  None.

RETURN VALUE:
  void

SIDE EFFECTS:
====================================================================*/
void rrcllc_set_utran_drx_info( uint32 utran_drx_coef );

/*====================================================================
FUNCTION: rrcllc_get_current_crnti_status()

DESCRIPTION:
  This function is responsible for returning the C-RNTI status as it
  exists in the Current Config. 
  
DEPENDENCIES:
  None

RETURN VALUE:
  rrcllc_crnti_status_e_type
  
SIDE EFFECTS:
  None.
====================================================================*/
rrcllc_crnti_status_e_type
rrcllc_get_current_crnti_status(void);

/*====================================================================
FUNCTION: rrcllc_get_current_urnti()

DESCRIPTION:
  This function is responsible for returning the U-RNTI  as it
  exists in the Current Config. 
  
DEPENDENCIES:
  None

RETURN VALUE:
  rrcllc_crnti_status_e_type
  
SIDE EFFECTS:
  None.
====================================================================*/
rrcllc_urnti_status_e_type
rrcllc_get_current_urnti(uint32 *urnti);
/*====================================================================
FUNCTION: rrcllc_get_urnti_from_toc()

DESCRIPTION:
  This function is responsible for returning the U-RNTI  if present on TOC
  
DEPENDENCIES:
  None

RETURN VALUE:
 rrcllc_urnti_status_e_type
  
SIDE EFFECTS:
  None.
====================================================================*/
rrcllc_urnti_status_e_type
rrcllc_get_urnti_from_toc(uint32 *urnti);

/*====================================================================
FUNCTION: rrcllc_update_lbt_mode_for_dchs()

DESCRIPTION:
  This function updates the Loopback Test mode in CCTrCH Info for
  L1 in the Ordered Config.
  It updates the mode for those transport channels that have a DTCH
  logical channel mapped. 
  
DEPENDENCIES:
  None.

RETURN VALUE:
  None.
  
SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_update_lbt_mode_for_dchs(
  l1_loop_back_mode_type_enum_type loopback_test_mode
#ifdef FEATURE_DUMMY_SIGNALING   
  #error code not present
#endif
);

/*====================================================================
FUNCTION: rrcllc_update_lbt_mode_for_fachs()

DESCRIPTION:
  This function updates the Loopback Test mode in CCTrCH Info for
  L1 in the Ordered Config.
  It updates the mode for those transport channels that have a DTCH
  logical channel mapped. 
  
DEPENDENCIES:
  None.

RETURN VALUE:
  None.
  
SIDE EFFECTS:
  None.
====================================================================*/

void rrcllc_update_lbt_mode_for_fachs(
  l1_loop_back_mode_type_enum_type loopback_test_mode
);


/*====================================================================
FUNCTION: rrcllc_update_sib1_data()

DESCRIPTION:
  This function reads SIB1 for the Current Serving Cell and updates
  both Current and Ordered Config databases with DRX cycle values
  for all available CN domains.
  If a domain is not present (CS or PS), the DRX cycle length for
  that domain is initialized to the one for the other domain to
  prevent any wrong calculations elsewhere.
  
DEPENDENCIES:
  The caller of this function has to make sure Cell Reselection is
  not in progress when this is called. Otherwise it may lead to
  storing DRX values of the old cell.

RETURN VALUE:
  Success/Failure to update.

SIDE EFFECTS:
  Look under dependency.
====================================================================*/
uecomdef_status_e_type  rrcllc_update_sib1_data(void);


/*============================================================================
FUNCTION: rrcllc_return_freq_for_fach_pch()

DESCRIPTION:
  This function returns the frequency that UE is directed to when going to 
  either one of Cell_FACH, Cell_PCH or URA_PCH
         
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE
  FAILURE indicates redirection frequency is NOT available.

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type
rrcllc_return_freq_for_fach_pch(rrc_freq_type  *freq);

/*============================================================================
FUNCTION: rrcllc_return_psc_for_fach_pch()

DESCRIPTION:
  This function returns the Primary Scvrambling Code that UE is directed to 
  when going from to either one of Cell_FACH, Cell_PCH or URA_PCH
         
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE
  FAILURE indicates redirection PSC is NOT available.

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type
rrcllc_return_psc_for_fach_pch(rrc_scr_code_type  *psc);

/*====================================================================
FUNCTION: rrcllc_can_oc_be_cleared_now

DESCRIPTION:
  This returns a boolean depending on if the LLC state permits
  a call to clear_ordered_config_now or not.
  If a valid LLC operation is under process, OC can not be
  cleared and this function returns a FALSE. Before returning
  a FALSE, it registers a clear_ordered_config operation so 
  that it is called immeditaley after a Command Engine is 
  finished.

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE = OC can be cleared now
  FALSE = OC can not be cleared.

SIDE EFFECTS:
  A clear ordered config event is registered.
====================================================================*/
boolean  rrcllc_can_oc_be_cleared_now(void);


/*====================================================================
FUNCTION: rrcllc_update_ciphering_config_info_for_first_tm_rb()

DESCRIPTION:
  This function updates the ciphering information in MAC.

DEPENDENCIES:
  None

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_update_ciphering_config_info_for_first_tm_rb(void);

/*====================================================================
FUNCTION: rrcllc_update_ciphering_config_info_for_handover()

DESCRIPTION:
  This function updates the ciphering information in RLC and MAC.

DEPENDENCIES:
  None
           
RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_update_ciphering_config_info_for_handover(void);


/*====================================================================
FUNCTION: rrcllc_set_ordered_config_for_rl_failure

DESCRIPTION:
  This is called when the ORDERED_CONFIG variable needs to be set.
  It checks to see whether or not it is set or not, and returns
  the status of the check.

  It also takes a pointer to the asn1 decoded message stream and
  places it into the ordered_config database.

  The procedure name is required so that the message asn1 decoded
  message can be properly typecast so as to populate the 
  ORDERED_CONFIG database.

DEPENDENCIES:
  CELL_DCH is the only supported state.

RETURN VALUE:
  rrcllc_oc_status_e_type - indicates whether the ORDERED_CONFIG
                            variable could be successfully set or
                            what the failure was.

SIDE EFFECTS:
  None.
====================================================================*/
rrcllc_oc_status_e_type rrcllc_set_ordered_config_for_rl_failure
(
  /*
  * This variable indicates the PDU number as defined in the ASN.1
  * header file. This information can be used to map the to correct
  * logical channel received on and then down to the message type.
  * The message contents are in msg_ptr, and need to be typecasted.
  */
  rrc_proc_e_type  proc_id
);


#ifdef FEATURE_RRC_L1_ERROR_RECOVERY_P1
/*===========================================================================
FUNCTION rrcllc_error_recovery_in_progress

DESCRIPTION
  This function triggers the error recovery mechanism instead of doing an
  ERR_FATAL when the delay timer expires.
 
DEPENDENCIES
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean rrcllc_error_recovery_in_progress(void);

#endif /* FEATURE_RRC_L1_ERROR_RECOVERY_P1 */

 /*====================================================================
FUNCTION: rrcllc_return_current_substate()

DESCRIPTION:
  This function returns the current llc substate.

DEPENDENCIES:
  None.

RETURN VALUE:
  current llc substate.

SIDE EFFECTS:
  None.

=====================================================================*/
rrcllc_substate_e_type rrcllc_return_current_substate(void);

/*====================================================================
FUNCTION: rrcllc_is_rab_release_in_progress()

DESCRIPTION:
  This function returns true if rrcllc is processing rab release due to T314/315 timer expiry. 
  This function was introduced to handle a corner case where Cell Update Confirm was received while 
  RAB release was in progress
  Scenario: 
1.CS call is going on
2.RLC RESET, UE moves to FACH and does CU
3.CS RAB Re-establishment timer expires and we release the RAB
4.While releasing the RAB(waiting for RRC_CRLC_DL_TM_CONFIG_CNF), UE receives CU-CNF to DCH state
5.CU-CNF updates OC, UE now receives RRC_CRLC_DL_TM_CONFIG_CNF
6.OC and CC are Swapped as rab-release is completed
7.LLC does Channel config to DCH 

DEPENDENCIES:
  None.

RETURN VALUE:
  current llc substate.

SIDE EFFECTS:
  None.

=====================================================================*/
boolean rrcllc_is_rab_release_in_progress(void);



/*====================================================================
FUNCTION: rrcllc_check_going_to_old_config()

DESCRIPTION:
  This function checks if going back to old config is ongoing.

DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE   going back to old confing in progress.
  FALSE  going back to old confing is not in progress

SIDE EFFECTS:
  None.

=====================================================================*/
boolean rrcllc_check_going_to_old_config(void);

/*====================================================================
FUNCTION: rrcllc_check_sufficient_act_time()

DESCRIPTION:
  This function checks if there is sufficient activation time to kill
  the pending configuration.

DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE   Sufficient activation time .
  FALSE  Insufficient activation time.

SIDE EFFECTS:
  None.

=====================================================================*/
boolean rrcllc_check_sufficient_act_time(void);

/*====================================================================
FUNCTION: rrcllc_get_n_bcch_setup_status()

DESCRIPTION:
  This function will return N-BCCH set up status.
  It will return TRUE if N-BCCH is set up FALSE other wise

DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE : If N-BCCH is set up
  FALSE otherwise
SIDE EFFECTS:
  None.

=====================================================================*/
boolean rrcllc_get_n_bcch_setup_status
(
void
);
/*====================================================================
FUNCTION: rrcllc_get_s_bcch_setup_status()

DESCRIPTION:
  This function will return S-BCCH set up status.
  It will return TRUE if S-BCCH is set up FALSE other wise

DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE : If S-BCCH is set up
  FALSE otherwise
SIDE EFFECTS:
  None.

=====================================================================*/
boolean rrcllc_get_s_bcch_setup_status
(
void
);

/*====================================================================
FUNCTION: rrcllc_get_rach_cctrch_ptr()

DESCRIPTION:
  This function returns rach cctrch ptr from current config

DEPENDENCIES:
  None.

RETURN VALUE:
  This function returns rach cctrch ptr from current config
SIDE EFFECTS:
  None.

=====================================================================*/
mac_ul_rach_cctrch_config_type* rrcllc_get_rach_cctrch_ptr
(
  void
);

/*====================================================================
FUNCTION: rrcllc_check_if_chan_config_in_progress_for_sib5_change()

DESCRIPTION:
  This function will return TRUE if channels are being configured for sib5 change(Requested by CSP)
  It will return FALSE other wise

DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE : if channels are being configured for sib change
  FALSE otherwise
SIDE EFFECTS:
  None.

=====================================================================*/
boolean rrcllc_check_if_chan_config_in_progress_for_sib5_change
(
  void
);

/*====================================================================
FUNCTION: rrcllc_process_chan_config_request()

DESCRIPTION:
  This function will process the channel config request

DEPENDENCIES:
  None.

RETURN VALUE:
  Next LLC State
  
SIDE EFFECTS:
  None.

=====================================================================*/
rrcllc_substate_e_type rrcllc_process_chan_config_request
(
  rrc_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION   rrcllc_cphy_ciphering_key_req

DESCRIPTION
  This function sends a CPHY_CIPHERING_KEY_REQ to L1. It gets a L1 command buffer,
  fills it up with the data, pointer to which is passed as a parameter to
  this function. It then sends the command over to L1.

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS or FAILURE

SIDE EFFECTS
  None.
===========================================================================*/
uecomdef_status_e_type rrcllc_cphy_ciphering_key_req
(
void
);

/*===========================================================================
FUNCTION RRCLLC_QUEUE_INIT

DESCRIPTION
  This function initializes the RRC LLC queues. This function needs
  to be called once at the time of the RRC task start-up.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rrcllc_queue_init
(
  void 
);

/*===========================================================================
FUNCTION: rrcllc_curr_cmd_rel_all_req

DESCRIPTION:
  This  function returns if the current cmd is CRLC REL all req. 
  In that state L2 ack can be ignored.

DEPENDENCIES:
  None.

RETURN VALUE:
TRUE/FALSE

SIDE EFFECTS:
  None
===========================================================================*/
boolean rrcllc_curr_cmd_rel_all_req
(
  void
);
/*===========================================================================

FUNCTION: rrcllc_check_dch_fach_reconfig_status

DESCRIPTION:


DEPENDENCIES:
  None.

RETURN VALUE:
TRUE/FALSE

SIDE EFFECTS:
  None
===========================================================================*/
boolean rrcllc_check_dch_fach_reconfig_status
(
  void
);
/*===========================================================================

FUNCTION: rrcllc_dch_fach_check_channel_setup_skip_status

DESCRIPTION:


DEPENDENCIES:
  None.

RETURN VALUE:
TRUE/FALSE

SIDE EFFECTS:
  None
===========================================================================*/
uint8 rrcllc_dch_fach_check_channel_setup_skip_status
(
  void
);

/*===========================================================================

FUNCTION: rrcllc_setup_common_logical_channels_for_fach

DESCRIPTION:


DEPENDENCIES:
  None.

RETURN VALUE:
TRUE/FALSE

SIDE EFFECTS:
  None
===========================================================================*/
uecomdef_status_e_type rrcllc_setup_common_logical_channels_for_fach
(
  void
);
/*===========================================================================

FUNCTION: rrcllc_setup_physical_channels_for_dch_to_fach

DESCRIPTION:


DEPENDENCIES:
  None.

RETURN VALUE:
TRUE/FALSE

SIDE EFFECTS:
  None
===========================================================================*/
uecomdef_status_e_type rrcllc_setup_physical_channels_for_dch_to_fach
(
  void
);


/*====================================================================
FUNCTION: rrcllc_reset_toc_usage_change_oos_to_dch()

DESCRIPTION:
  This function resets the value of toc_usage_change_oos_to_dch

DEPENDENCIES:
  None.

RETURN VALUE:
 boolean

SIDE EFFECTS:
  None.

=====================================================================*/
void rrcllc_reset_toc_usage_change_oos_to_dch
(
  void
);
/*====================================================================
FUNCTION: rrcllc_get_toc_usage_change_oos_to_dch()

DESCRIPTION:
  This function returns the value of toc_usage_change_oos_to_dch

DEPENDENCIES:
  None.

RETURN VALUE:
 boolean

SIDE EFFECTS:
  None.

=====================================================================*/
boolean rrcllc_get_toc_usage_change_oos_to_dch
(
  void
);

/*====================================================================
FUNCTION: rrcllc_get_n_bcch_setup_status()

DESCRIPTION:
  This function will return N-BCCH set up status.
  It will return TRUE if N-BCCH is set up FALSE other wise

DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE : If N-BCCH is set up
  FALSE otherwise
SIDE EFFECTS:
  None.

=====================================================================*/
uecomdef_status_e_type rrcllc_get_bcch_psc
(
  uint32* psc
);

#ifdef FEATURE_WCDMA_HS_RACH
/*====================================================================
FUNCTION: rrcllc_get_hsrach_config_ptr()

DESCRIPTION:
  This function returns hsrach config ptr from current config

DEPENDENCIES:
  None.

RETURN VALUE:
  This function returns hsrach config ptr  from current config
SIDE EFFECTS:
  None.

=====================================================================*/
mac_hs_rach_config_s_type* rrcllc_get_hsrach_config_ptr
(
  void
);

#endif

#endif /* RRCLLC_H */

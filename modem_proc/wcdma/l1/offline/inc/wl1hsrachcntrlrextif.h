#ifndef WL1HSRACHCNTRLREXTIF_H
#define WL1HSRACHCNTRLREXTIF_H

/*===========================================================================
                 WCDMA L1 HS RACH Controller (HEADER)

GENERAL DESCRIPTION
  This is a header file that has the APIs with which the HS RACH controller
  can interface with the different modules within WCDMA L1.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2006 - 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/wl1hsrachcntrlrextif.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who    what, where, why
--------   ---   ----------------------------------------------------------
10/01/14   gv     Code cleanup. Removal of stale and unused code. 
12/18/13   ash    Fix for HSRACH corner case scenario where SS tables are not ready
05/22/13   oh     Adding DC-HSUPA framework support
08/31/12   vr     Fix for 0x4309 logging specific to HSRACH seamless transition.
07/06/12   vr     HSRACH fixes
06/04/12   gv     Feature HS RACH Initial code check in -- File Creation
*/

#include "wcdma_variation.h"
#ifdef FEATURE_WCDMA_HS_RACH

#include "l1rrcif.h"
#include "eul.h"
#include "mcalwcdma_hsrach.h"
#include "wl1hsrachcntrlr.h"
 /*==============================================
                                       Header Inclusions
 **==============================================*/



/*==============================================
                                        MACROS
**==============================================*/
#define WL1_HS_RACH_VALID_FDPCH_SLOT_FMT 0

#define WL1_EUL_DL_AGCH_OVSF_CODE_MIN 0
#define WL1_EUL_DL_AGCH_OVSF_CODE_MAX 255

#define WL1_EUL_DL_GRANT_STEP_IDX_MIN 0
#define WL1_EUL_DL_GRANT_STEP_IDX_MAX 37

/* Index of minimum grant  in SG table. The value Minimum_Grant corresponds to
   the minimum E-DPDCH to DPCCH power ratio that the UE considers. This value
   is (5/15)^2.
*/
#define      WL1_EUL_MINIMUM_GRANT_IDX              0

/*The max valid SG index for Scheduling Grant table shown in Table 9.2.5.2.1.1
  of 25.321.
*/
#define      WL1_EUL_MAX_SG_IDX                     37

/* Define for default start and final sub fn for EUL/DL channels for config
** when the channels are added using JUDC*/
#define EUL_START_SUB_FN_CHANNEL_ADD_DFLT_VAL 1
#define EUL_FINAL_SUB_FN_CHANNEL_ADD_DFLT_VAL 0

#define WL1_HS_RACH_EUL_2MS_DPCCH_ONLY_TX_OFFSET     2
#define WL1_HS_RACH_EUL_10MS_DPCCH_ONLY_TX_OFFSET     1
#define WL1_HS_RACH_EUL_DPCCH_ONLY_TX_INVALID_VAL 0xFFFF

#define WL1_HS_RACH_EUL_FINAL_SUN_FN_OFFSET 3

#define WL1_HS_RACH_COMPUTE_APPLY_SFN_REFERENCE_2MS_TTI(cfg_cfn,delta_sfn,div_by_30,apply_sfn_rfrnce) \
  apply_sfn_rfrnce = ((cfg_cfn*EUL_2MS_MAX_NUM_SUB_FR_IN_10MS) + (delta_sfn - EUL_2MS_MAX_NUM_SUB_FR_IN_10MS)-div_by_30)%EUL_MAX_SUB_FN_COUNT

#define WL1_HS_RACH_COMPUTE_APPLY_SFN_REFERENCE_10MS_TTI(cfg_cfn,delta_sfn,apply_sfn_rfrnce) \
  apply_sfn_rfrnce = ((cfg_cfn*EUL_2MS_MAX_NUM_SUB_FR_IN_10MS) + (delta_sfn - EUL_2MS_MAX_NUM_SUB_FR_IN_10MS))%EUL_MAX_SUB_FN_COUNT

#define WL1_HS_RACH_COMPUTE_FINAL_SFN_VAL(final_sfn, curr_cfn, fdpch_cfg_cfn, sub_fn_offset) \
	final_sfn = ((curr_cfn - fdpch_cfg_cfn + MAX_CFN_COUNT) % MAX_CFN_COUNT)*EUL_2MS_MAX_NUM_SUB_FR_IN_10MS + sub_fn_offset

#define WL1_HS_RACH_IS_RGCH_CFGED()((EDL_GET_RGCH_CUR_PAGE(EDL_GET_HS_RACH_E_DL_RL_IDX()))!= INVALID_PAGE_INDEX)

#define WL1_HS_RACH_GET_AGCH_APPLY_SUBFN_REFERENCE() wl1_hs_rach_config_params.agch_cfg_info.apply_subfn_reference

#define WL1_HS_RACH_GET_HICH_APPLY_SUBFN_REFERENCE() wl1_hs_rach_config_params.hich_cfg_info.apply_subfn_reference

#define WL1_HS_RACH_GET_RGCH_APPLY_SUBFN_REFERENCE() wl1_hs_rach_config_params.rgch_cfg_info.apply_subfn_reference

/* ===============================================
**                                   ENUMERATED TYPES DEFINTIONS
** ===============================================*/


/* ===============================================
**                                   STRUCTURE TYPES DEFINTIONS
** ===============================================*/
/* ===============================================
**                                   EXTERN of GLOBAL VARIABLES
** ===============================================*/
extern wl1_eul_chan_setup_enum_type wl1_eul_chan_setup_type;

/* ===============================================
**                                   FUNCTION PROTOTYPES
** ===============================================*/
 extern void wl1_hs_rach_eul_ul_setup_drop(void);

 /*===========================================================================
 FUNCTION		 WL1_HS_RACH_DEACTIVATE_E_UL_CFG

 DESCRIPTION

 DEPENDENCIES	 None

 RETURN VALUE	 None

 SIDE EFFECTS
 ===========================================================================*/
 extern void wl1_hs_rach_deactivate_e_ul_cfg(void);

 /*===========================================================================
 FUNCTION		  WL1_HS_RACH_UPDATE_SG_CFG

 DESCRIPTION	  This function is used to singal a SG change to EUL.
				  The function is once the validation is done and so that the
				  at the activation time CFN, the SG can be activated.

 DEPENDENCIES	  None

 RETURN VALUE	  None

 SIDE EFFECTS	  None

 ===========================================================================*/

 extern void wl1_hs_rach_update_sg_cfg(void);

 /*===========================================================================
 FUNCTION		  E_DL_UPDATE_NUM_E_RL

 DESCRIPTION	  This function updates the number of RL in the EDCH
				  ASET in the e_dl_config_db.

 DEPENDENCIES	  None

 RETURN VALUE	  None

 SIDE EFFECTS	  Updates the number of RLs in the EDCH ASET

 ===========================================================================*/
 void e_dl_update_num_e_rl(uint8 carr_idx);

 /*===========================================================================

  FUNCTION		  E_DL_ADD_RADIO_LINK

  DESCRIPTION

  DEPENDENCIES

  RETURN VALUE

  SIDE EFFECTS	  Unknown

  ===========================================================================*/
  void e_dl_add_radio_link(uint16 rl_idx, l1_e_dl_info_struct_type *e_dl_info_ptr,
                                wl1_ul_carr_id_enum_type carr_idx);

 /*===========================================================================

   FUNCTION 	   E_DL_INIT_RLS_TPC_SET_INFO_BEFORE_CFG

   DESCRIPTION

   DEPENDENCIES    Unknown

   RETURN VALUE    None

   SIDE EFFECTS    Unknown

   ===========================================================================*/
 void e_dl_init_rls_tpc_set_info_before_cfg(void);

 /*===========================================================================

FUNCTION                WL1_HS_RACH_UPDATE_APPLY_SFN_REFERENCE

DESCRIPTION      This function updates the apply SFN reference. THis will
                               be used in coming up with the apply sub FN after reading the
                               EUL DL resutls.


DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS    Impacts the apply sub FN as to wherethe results will be
                            applied

===========================================================================*/
void wl1_hs_rach_update_apply_sfn_reference(void);

/*===========================================================================

FUNCTION        WL1_HS_RACH_DL_FDPCH_SETUP_ADD

DESCRIPTION     This function is called from the wl1_hs_rach_channel_setup_add()
                and this function would interface with the DL module to setup the F-DPCH


DEPENDENCIES    CPHY_SETUP_REQ command

RETURN VALUE    Boolean indicating whether the F-DPCH setup was
                success or not

SIDE EFFECTS    None

===========================================================================*/
extern boolean wl1_hs_rach_dl_fdpch_setup_add(dl_phychan_add_type * add_dl_phychan_ptr);

/*===========================================================================

 FUNCTION        WL1_HS_RACH_DL_FDPCH_SETUP_RESUME

 DESCRIPTION     This function is called from the wl1_hs_rach_channel_resume_proc()
                 and this function would interface with the DL module to resume the F-DPCH


 DEPENDENCIES    Triggered on receipt of Tx Indication from MAC

 RETURN VALUE    Boolean indicating whether the F-DPCH setup was
                 success or not

 SIDE EFFECTS    None

 ===========================================================================*/
 extern boolean wl1_hs_rach_dl_fdpch_setup_resume(void);

 /*===========================================================================

FUNCTION        WL1_HS_RACH_EUL_DL_SETUP_ADD

DESCRIPTION     This function is called from the wl1_hs_rach_channel_setup_add()
                and this function would interface with the EUL DL module to setup
                AGCH, HICH and RGCH


DEPENDENCIES    CPHY_SETUP_REQ command

RETURN VALUE    Boolean indicating whether the EUL DL setup was
                success or not

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_hs_rach_eul_dl_setup_add(l1_e_dl_info_struct_type *hs_rach_edl_cfg_ptr);

/*===========================================================================

FUNCTION		WL1_HS_RACH_EUL_DL_SETUP_SUSPEND

DESCRIPTION		This function is called from the wl1_hs_rach_channel_suspend_proc()
				and this function would interface with the EUL DL module to suspend/stop
				from the FW perspective  AGCH, HICH and RGCH


DEPENDENCIES	  Triggered on receipt of local command to release EDCH resource

RETURN VALUE	  None

SIDE EFFECTS	  None

===========================================================================*/
extern void wl1_hs_rach_eul_dl_setup_suspend(void);

 /*===========================================================================

   FUNCTION        WL1_HS_RACH_EUL_DL_SETUP_RESUME

   DESCRIPTION     This function is called from the wl1_hs_rach_channel_resume_proc()
                   and this function would interface with the EUL DL module to resume
                   AGCH, HICH and RGCH


   DEPENDENCIES    Triggered on receipt of Tx Indication from MAC

   RETURN VALUE    Boolean indicating whether the EUL DL resume was
                   success or not

   SIDE EFFECTS    None

   ===========================================================================*/
   extern void wl1_hs_rach_eul_dl_setup_resume(void);

 /*===========================================================================

FUNCTION        WL1_HS_RACH_EUL_UL_SETUP_ADD

DESCRIPTION     This function is called from the wl1_hs_rach_channel_setup_add()
                and this function would interface with the EUL UL module.


DEPENDENCIES    CPHY_SETUP_REQ command

RETURN VALUE    Boolean indicating whether the EUL UL setup was
                success or not

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_hs_rach_eul_ul_setup_add(void);

 /*===========================================================================

   FUNCTION        WL1_HS_RACH_EUL_UL_SETUP_RESUME

   DESCRIPTION     This function is called from the wl1_hs_rach_channel_resume_proc()
                   and this function would interface with the EUL UL module to resume
                   E-DPDCH


   DEPENDENCIES    Triggered on receipt of Tx Indication from MAC

   RETURN VALUE    Boolean indicating whether the EUL ULresume was
                   success or not

   SIDE EFFECTS    None

   ===========================================================================*/
   extern void wl1_hs_rach_eul_ul_setup_resume(void);

  /*===========================================================================

FUNCTION 	   WL1_HS_RACH_EUL_UL_SETUP_SUSPEND

DESCRIPTION	   This function is called from the wl1_hs_rach_channel_suspend_proc()
			   and this function would interface with the EUL DL module to suspend
			   the EUL UL channels.


DEPENDENCIES    Triggered on receipt of Local command for EDCH release

RETURN VALUE    Boolean indicating whether the EUL DL suspend was
			   success or not

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_hs_rach_eul_ul_setup_suspend(void);

  /*===========================================================================

 FUNCTION		 WL1_HS_RACH_VALIDATE_EUL_UL_PARAMS

 DESCRIPTION	 This function is called from the HS RACH validate function. This
				 function validates the EUL UL parameters configured for HS RACH setup.


 DEPENDENCIES	 CPHY_SETUP_REQ command

 RETURN VALUE	 Boolean indicating whether the EUL UL validation was success

 SIDE EFFECTS	 None

 ===========================================================================*/
extern boolean wl1_hs_rach_validate_eul_ul_params(l1_e_hs_rach_info_struct_type *hs_rach_eul_info_ptr,
                                                  uint8 hs_rach_req_mask);

/*===========================================================================
FUNCTION        WL1_HS_RACH_ACTIVATE_E_UL_CFG

DESCRIPTION     This function is called from the F-DPCH setup interrupt
                event handler. This interrupt from the FW indicates that
                the channels setup procedure has been initiated in FW.
                This interrupt would be treated as channel setup success
                from L1 side. This function internally performs the activation
                of the configuration at the EUL side

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS   This function activates the databases of EUL UL and
               performs the functions of activate config params
===========================================================================*/
extern void wl1_hs_rach_activate_e_ul_cfg(void);
/*===========================================================================
FUNCTION        WL1_HS_RACH_ACTIVATE_E_DL_CFG

DESCRIPTION     This function is called from the F-DPCH setup interrupt
                event handler. This interrupt from the FW indicates that
                the channels setup procedure has been initiated in FW.
                This interrupt would be treated as channel setup success
                from L1 side. This function internally performs the activation
                of the configuration at the EUL DL side

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS   This function activates the databases of EUL DL and
               performs the functions of activate config params
===========================================================================*/
extern void wl1_hs_rach_activate_e_dl_cfg(void);

/*===========================================================================

 FUNCTION        WL1_HS_RACH_EDL_UPDATE_CHANNEL_INFO

 DESCRIPTION     This function is called from the HS RACH cntrlr
                    function to process the E-AI status. If an ACK is received
                    this function is called to update the EUL DL config databases
                    with the AGCH,HICH and RGCH information

 DEPENDENCIES    Unknown

 RETURN VALUE    None

 SIDE EFFECTS    Updates the e_dl_config_db for AGCH,HICH and RGCH

 ===========================================================================*/
extern void wl1_hs_rach_edl_update_channel_info(uint16 tau_fdpch,
 	         mcalwcdma_hsrach_rsrc_idx_channel_param_struct_type *edch_rsrc_idx_chan_params);

/*===========================================================================

 FUNCTION        WL1_HS_RACH_EDL_UPDATE_CONFIG_DB_INFO

 DESCRIPTION     This function is called from the HS RACH cntrlr
                    function to process the E-AI status. If an ACK is received
                    this function is called to update the EUL DL config databases
                    with the HICH and RGCH information

 DEPENDENCIES    Unknown

 RETURN VALUE    None

 SIDE EFFECTS    Updates the e_dl_config_db for AGCH,HICH and RGCH

 ===========================================================================*/
extern void wl1_hs_rach_edl_update_config_db_info(void);
/*===========================================================================

 FUNCTION        WL1_HS_RACH_FLUSH_LOG_PKTS_POST_SEAMLESS_TRANSITION

 DESCRIPTION     This function is called from the e_ul_activate_cfg_params to flush the
                 log packets after the seamless reconfig from HS RACH to EUL is deemed as complete

 DEPENDENCIES    Unknown

 RETURN VALUE    None

 SIDE EFFECTS    Submits, EUL and EUL DL log packets to diag

 ===========================================================================*/
extern void wl1_hs_rach_flush_log_pkts_post_seamless_transition(void);


#endif /*FEATURE_WCDMA_HS_RACH*/
#endif


/*========================================================================================

                                    R R C H S R A C H C F G . C

DESCRIPTION
  This file contains the definitions of the functions that handles the configuration of HS RACH.

  Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
========================================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrchsrachcfg.c#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $
  
 when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/20/16   sg      Made changes to reduce F3s
10/22/14   sp      Made changes to remove redundant F3s
10/22/14   gv      Made changes to have connected mode feature set independent of TA status 
08/04/14   sp      Made changes to fix compilation errors
06/09/14   db      RRC Logging Changes for CMAC and CRLC
05/20/14   rmn     Made changes to save DS Tuneaway Status Change indication in connected state based on the concurrency NV
05/16/14   vi      Made changes to support MOB framework
04/10/14   db      Made changes to enable/disable feature support in DSDS/DSDA/TSTS modes through NV
03/25/14   db      Corrected validation checks in rrchsrach_process_meas_feedback_info()
02/13/14   ad      Made changes to remove redundant F3s
09/20/13   ad      Made changes to split NV control of FDPCH and CPC DTX/DRX features 
12.24/13   sp      Made changes to use config ptr instead of Oredered config ptr for reselection cases.
12/24/13   db      Introduced new NV to allow CPC,HSFACH,HSRACH,DC-HSDPA in DSDS and DSDA mode
11/15/13   sp      Added Nv support for boost feature
10/07/13   db      Made changes to configure downlink properly upon PCH to FACH thorugh MR
09/13/13   ad       Made changes to pick the prach parameters from first prach index
                    if optional hsrach parameters are not provided 
07/01/13   vg      Do not support Common EDCH if UE category is 1,3,5.
06/27/13   as      Made chagnes to disable HS-RACH using NV
06/14/13   db      Added backup and restore ERNTI logic for transition to(out of) PCH state
06/10/13   vg      HSRACH: Ignore HSRACH in SIB5 if 2ms EDCH-TTI is configured for UE HSUPA/EDCH CAT 1,3,5
06/07/13   db      Disable HSRACH when CPC-FDPCH NV is set(67333)
04/22/13   db      Multiple mapping support for EFACH
03/26/13   db      Changes for unification of Cell Transition and CPHY setup handling.
01/03/13   ad      Made changes to reduce the redundant F3s
12/17/12   mp       Fixing KW errors
12/12/12   mp       Fixing KW errors
10/23/12   pm       Made changes to correct enable HSRACH NV only when MACI and HSFACH
                    NVs are also ON
10/18/12   gv       Replace FEATURE_DUAL_SIM_WCDMA_TUNEAWAY with FEATURE_DUAL_SIM
08/25/12   sa       Made changes to support HSRACH NV
08/22/12   ad       Made changes to handle HSCFG if N/W configures HS-DPCCH 
                    without CQI Meas FB info
07/20/12   ad       Made changes to do rlc-reestablishment during reselections.
01/03/12   ad/as    Support for FEATURE_WCDMA_HS_RACH
===========================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "wcdma_variation.h"
#include "uecomdef.h"

#ifdef FEATURE_WCDMA_HS_RACH

#include "string.h"
#include "fs_fcntl.h"
#include "fs_public.h"
#include "fs_sys_types.h"
#include "string.h"
#include "fs_lib.h"

#include "rrcasn1_ie.h"
#include "rrcsibproci.h"
#include "rrchsrachcfg.h"
#include "rrcsibdb.h"
#include "rrcrce.h"
#include "rrcenhstatecfg.h"


extern uint16 rrc_feature_nv_item_value;
extern uint32 DEBUG_EUL_SI_ONLY_POWER_OFFSET;
extern uint32 RRCEUL_SET_NO_GRANT_DEFAULT_PERIODICITY ;
extern uint32 RRCEUL_NO_GRANT_PERIODICITY_DEFAULT_VALUE ;
extern boolean DEBUG_EUL_RRC_VALIDATE_STEP_INDEX_THRESHOLD ;

/*====================================================================
FUNCTION: rrcllc_init_l1_hsrach_prach_info()

DESCRIPTION:
  This function initializes hs-rach l1 prach variables.

DEPENDENCIES:

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_l1_hsrach_prach_info
(
  l1_ul_prach_cfg_struct_type *hs_prach_cfg_ptr  
)
{
    
  hs_prach_cfg_ptr->avail_sig_mask = 0;
  
  hs_prach_cfg_ptr->sf_min = L1_NUM_SF;
  
  hs_prach_cfg_ptr->sc_num = 0xFF;
  
  hs_prach_cfg_ptr->avail_sub_chan_mask = 0;
  
  hs_prach_cfg_ptr->trch_id = 0xFF;
  
  hs_prach_cfg_ptr->asc_cfg.num_asc= 0;
  
  hs_prach_cfg_ptr->pwr_offset_P0= 0xFF;
  
  hs_prach_cfg_ptr->hs_rach_info.e_ai_indication = FALSE;
  
  hs_prach_cfg_ptr->hs_rach_info.powerOffsetPp_e = L1_HS_RACH_INVALID_POWER_OFFSET_PP_E;
      
}

/*====================================================================
FUNCTION: rrcllc_init_l1_hsrach_ul_dpch_info()

DESCRIPTION:
  This function initializes hs-rach ul dpch structure.

DEPENDENCIES:


RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_l1_hsrach_ul_dpch_info
(
  l1_ul_dpch_cfg_struct_type *ul_dpch_cfg_ptr  
)
{
  
  ul_dpch_cfg_ptr->dpch_pwr_ctrl_info.valid = FALSE;
  
  ul_dpch_cfg_ptr->dpch_pwr_ctrl_info.dpcch_pwr_offset = 0;
  
  ul_dpch_cfg_ptr->dpch_pwr_ctrl_info.pcp_len = 0xFF;
  
  ul_dpch_cfg_ptr->dpch_pwr_ctrl_info.srb_delay = 0xFF;
  
  ul_dpch_cfg_ptr->dpch_pwr_ctrl_info.pca= L1_UL_NUM_PCA;
  
  ul_dpch_cfg_ptr->dpch_pwr_ctrl_info.tpc_step_size= L1_UL_NUM_TPC_STEP_SIZE;
  
  ul_dpch_cfg_ptr->scrambling_code_type= L1_UL_SC_SHORT;
  
  ul_dpch_cfg_ptr->scrambling_code= 0xFFFFFFFF;
  
  ul_dpch_cfg_ptr->num_dpdch= MAX_DPDCH+1;
  
  ul_dpch_cfg_ptr->sf_min= L1_NUM_SF;
  
  ul_dpch_cfg_ptr->tfci_present= FALSE;
  
  ul_dpch_cfg_ptr->fbi_len= L1_UL_NUM_FBI_BITS;
  
  ul_dpch_cfg_ptr->punct_lim= 0xFF;
  
  ul_dpch_cfg_ptr->numtpc= L1_UL_NUM_TPC_BITS_MAX;
  
  ul_dpch_cfg_ptr->dpcch_slot_format= L1_UL_DPCCH_SLOT_FORMAT_MAX;
    
}

/*====================================================================
FUNCTION: rrcllc_init_l1_hsrach_common_edch_resource_list()

DESCRIPTION:
  This function initializes hs-rach common edch resource list .

DEPENDENCIES:


RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_l1_hsrach_common_edch_resource_list
(
  l1_e_dch_common_list_info_struct_type *common_e_dch_resource_list_ptr  
)
{
  uint8 count = 0;

  common_e_dch_resource_list_ptr->num_e_dch_common_info = 0;

  for (count=0; count<L1_MAX_E_DCH_COMMON_LIST; count++)
  {	

  common_e_dch_resource_list_ptr->e_dch_common_info[count].s_offset = 0xFF;

  common_e_dch_resource_list_ptr->e_dch_common_info[count].e_rgch_info_present = FALSE;

  common_e_dch_resource_list_ptr->e_dch_common_info[count].e_rgch_info.rg_idx = 0xFF;
  common_e_dch_resource_list_ptr->e_dch_common_info[count].e_rgch_info.signature_idx= 0xFF;

  common_e_dch_resource_list_ptr->e_dch_common_info[count].e_hich_info.ch_code= 0xFF;
  common_e_dch_resource_list_ptr->e_dch_common_info[count].e_hich_info.signature_idx= 0xFF;

  common_e_dch_resource_list_ptr->e_dch_common_info[count].l1_ul_dpch_code_info.scrambling_code_type = L1_UL_SC_SHORT;
  common_e_dch_resource_list_ptr->e_dch_common_info[count].l1_ul_dpch_code_info.scrambling_code = 0xFFFFFFFF;

  }
  
}


/*====================================================================
FUNCTION: rrcllc_init_l1_hsrach_aich_info()

DESCRIPTION:
  This function initializes hs-rach l1 aich variables.

DEPENDENCIES:


RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_l1_hsrach_aich_info
(
  l1_aich_parms_struct_type *hs_rach_aich_ptr  
)
{

  hs_rach_aich_ptr->tx_div_ind = FALSE;
  hs_rach_aich_ptr->tt = L1_DL_NUM_AICH_TT;
  hs_rach_aich_ptr->pwr_offset = 0xFF;

}
/*====================================================================
FUNCTION: rrcllc_init_l1_hsrach_fdpch_info()

DESCRIPTION:
  This function initializes hs-rach l1 fdpch variables.

DEPENDENCIES:


RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_l1_hsrach_fdpch_info
(
  l1_hs_rach_f_dpch_info_struct_type *hs_rach_dl_fdpch_info_ptr  
)
{
  hs_rach_dl_fdpch_info_ptr->dpc_info = L1_DPC_MODE_SINGLE_TPC;
  hs_rach_dl_fdpch_info_ptr->tpc_cmd_err_rate_target = 0;
  hs_rach_dl_fdpch_info_ptr->fdpch_slot_format = 0;
}
/*====================================================================
FUNCTION: rrcllc_init_l1_hsrach_hsdpcch_info()

DESCRIPTION:
  This function initializes l1 hs dpcch variables.

DEPENDENCIES:


RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_l1_hsrach_hsdpcch_info
(
  l1_ul_hs_dpcch_info_struct_type *hs_dpcch_info_ptr,
  rrc_state_e_type next_rrc_state
)
{
 
  hs_dpcch_info_ptr->info_present_bitmask = 0;

  hs_dpcch_info_ptr->beta_hs_info.delta_ack = RRCLLC_DELTA_ACK_MAX + 1;

  hs_dpcch_info_ptr->beta_hs_info.delta_nack = RRCLLC_DELTA_NACK_MAX + 1;

  if((next_rrc_state == RRC_STATE_CELL_FACH) ||(next_rrc_state == RRC_STATE_CONNECTING))
  {
    /*
    Based on the discussion with L1 folks RRC will set the default values to parameters so that 
    the desired effect of no CQI reporting is observed.
    
    Confirmed with Fred after the meeting that if deltaCQI is set to 0, it will have the effect of 'no CQI reporting' on HS-DPCCH.
    
    For the cases where SIB does not carry the optional elements of meas_feedback_info structure, RRC can use the following default values.
    
    po_hs_dsch = -12
    cqi_feedback_cycle = 0 ' This also provides the effect of DTXing CQI reports
    cqi_rep_factor = 1
    ack_nack_rep_factor = 1
    harq_pmbl_mode would always be FALSE
    
    For delta_cqi, RRC should set it to 0 when the IE is missing.
    */
    
      hs_dpcch_info_ptr->beta_hs_info.delta_cqi = 0;
    
      hs_dpcch_info_ptr->meas_feedback_info.ack_nack_rep_factor = 1;
    
      hs_dpcch_info_ptr->meas_feedback_info.cqi_rep_factor = 1;
      
      hs_dpcch_info_ptr->meas_feedback_info.cqi_feedback_cycle = 0;
      
      hs_dpcch_info_ptr->meas_feedback_info.po_hs_dsch = RRCLLC_MIN_MEAS_POWER_OFFSET;
  
  }
  else
  {
      
    hs_dpcch_info_ptr->beta_hs_info.delta_cqi = RRCLLC_DELTA_CQI_MAX + 1;
  
    hs_dpcch_info_ptr->meas_feedback_info.ack_nack_rep_factor = 
          RRCLLC_DELTA_ACK_NACK_REPITITION_FACTOR_MAX +1;
    
    hs_dpcch_info_ptr->meas_feedback_info.cqi_rep_factor = 0;
    
    hs_dpcch_info_ptr->meas_feedback_info.cqi_feedback_cycle = 0xff;
    
    hs_dpcch_info_ptr->meas_feedback_info.po_hs_dsch = RRCLLC_MAX_MEAS_POWER_OFFSET + 1;  
  }

 #ifdef FEATURE_HSDPA_HARQ_PREAMBLE
  hs_dpcch_info_ptr->meas_feedback_info.harq_pmbl_mode = FALSE;
 #endif

}
/*====================================================================
FUNCTION: rrcllc_init_l1_hsrach_eul_info()

DESCRIPTION:
  This function initializes hs-rach l1_e_ul_info variables.

DEPENDENCIES:


RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_l1_hsrach_eul_info
(
  l1_e_ul_info_struct_type *e_ul_info_ptr  
)
{

  e_ul_info_ptr->e_tti = NUM_E_TTI;

  e_ul_info_ptr->harq_rv_config = L1_RV_NOT_PRESENT;	
  
  e_ul_info_ptr->e_dpcch_info.e_dpcch_pwr_offset = 0xFF;
#ifdef FEATURE_WCDMA_REL7
#ifdef FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST
  e_ul_info_ptr->e_dpcch_info.e_tfc_boost_info.e_tfci_boost_val =0xff;
  e_ul_info_ptr->e_dpcch_info.e_tfc_boost_info.delta_t2tp =0xff;
#endif
  e_ul_info_ptr->e_dpcch_info.e_dpdch_pwr_interpolation = FALSE;
#endif/*FEATURE_WCDMA_REL7*/

  e_ul_info_ptr->e_dpdch_info.e_max_ch_codes = 
    L1_NUM_L1_E_MAX_CH_CODES_SF;  

  e_ul_info_ptr->e_dpdch_info.num_ref_e_tfci = 0;

  e_ul_info_ptr->e_dpdch_info.e_dpdch_pl_non_max = 0xFF;

  e_ul_info_ptr->e_dpdch_info.e_tfci_table_idx = 0xFF;

  e_ul_info_ptr->e_dpdch_info.min_beta_ed = NUM_L1_E_MIN_BED;

}

/*====================================================================
FUNCTION: rrcllc_init_l1_hsrach_edl_info()

DESCRIPTION:
  This function initializes hs-rach l1_e_dl_info variables.
  
DEPENDENCIES:


RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_l1_hsrach_edl_info
(
  l1_e_dl_hs_rach_info_struct_type *e_dl_info_ptr
)
{

  e_dl_info_ptr->e_tti = NUM_E_TTI;
  
  e_dl_info_ptr->grant_config.serving_grant =  L1_SERVING_GRANT_NOT_PRESENT;
  
  e_dl_info_ptr->grant_config.grant_selector =   L1_E_GRANT_SELECTOR_NOT_PRESENT;
  
  e_dl_info_ptr->grant_config.three_index_step_threshold = L1_SG_STEP_THRESHOLD_NOT_PRESENT;
  
  e_dl_info_ptr->grant_config.two_index_step_threshold = L1_SG_STEP_THRESHOLD_NOT_PRESENT;

}

/*====================================================================
FUNCTION: rrcllc_init_mac_hsrach_edch_info()

DESCRIPTION:
  This function initializes hs-rach mac e_dch_info structure variables (i.e. UL-EDCH
  physical layer parameters).

DEPENDENCIES:


RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_mac_hsrach_edch_info
(
  mac_e_dch_info_s_type *e_dch_info_ptr
)
{
  e_dch_info_ptr->e_dpcch_info.happy_bit_delay_in_ms = 0;

  e_dch_info_ptr->e_dpdch_info.e_tfci_min_set = MAC_E_TFCI_MIN_SET_NOT_PRESENT;

  e_dch_info_ptr->e_dpdch_info.e_tfci_table_index = 0;

  e_dch_info_ptr->e_dpdch_info.sched_info_config.grant_periodicity_of_si = 0;

  e_dch_info_ptr->e_dpdch_info.sched_info_config.no_grant_periodicity_of_si = 0;

  e_dch_info_ptr->e_dpdch_info.sched_info_config.power_offset_of_si = 0;

  e_dch_info_ptr->e_dpdch_info.sched_tx_config.sched_tx_grant_harq_allocation_2ms = 0xFF;

}

/*====================================================================
FUNCTION: rrcllc_init_mac_hsrach_info()

DESCRIPTION:
  This function initializes hs-rach mac structures in rrc data-base
  
DEPENDENCIES:

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_mac_hsrach_info
(
  mac_hs_rach_config_s_type *mac_hs_rach_config_ptr
)
{
  uint32 count, idx;
  
  mac_hs_rach_config_ptr->add_e_dch_tx_backoff = 0xFF;

  mac_hs_rach_config_ptr->max_collision_resol = 0xFF;

  mac_hs_rach_config_ptr->max_e_dch_res_alloc_ccch = 0xFF;

  mac_hs_rach_config_ptr->l1_e_dch_tx_cont_backoff = MAC_HS_RACH_TX_BACKOFF_INVALID;

  mac_hs_rach_config_ptr->ccch_asc = 0xFF;
  
  mac_hs_rach_config_ptr->mac_ul_rach_txcntl.mmax = RRCLLC_MAX_MMAX+1;
  
  mac_hs_rach_config_ptr->mac_ul_rach_txcntl.nbo1min= RRCLLC_MAX_NB01+1;	
  
  mac_hs_rach_config_ptr->mac_ul_rach_txcntl.nbo1max= RRCLLC_MAX_NB01+1;		

//  config_ptr->mac_hs_rach_config.mac_ul_rach_txcntl.asc= ;		

  mac_hs_rach_config_ptr->mac_ul_rach_txcntl.nASC= 0;			

  //for (i=0;i<MAX_ASC;i++)
    mac_hs_rach_config_ptr->mac_ul_rach_txcntl.asc->pval = 0;

  mac_hs_rach_config_ptr->hs_rach_config.e_tti= NUM_E_TTI;	

  rrcllc_init_mac_hsrach_edch_info(&mac_hs_rach_config_ptr->hs_rach_config.e_dch_info);

  mac_hs_rach_config_ptr->hs_rach_config.num_e_mac_d_flow = 0;

  for (count=0; count<MAX_E_DCH_MAC_D_FLOW; count++)
  {
    mac_hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].
          num_logch = 0;

        for (idx=0; idx < MAX_DED_LOGCHAN_PER_TRCH; idx++)
      {
    mac_hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].
          e_ul_logch_info[idx].rb_id = 0xFF;

    mac_hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].
          e_ul_logch_info[idx].rlc_id = 0xF;

    mac_hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].
          e_ul_logch_info[idx].lc_id = 0xF;

    mac_hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].
          e_ul_logch_info[idx].rlc_mode = UE_MODE_MAX_NUM;

    mac_hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].
          e_ul_logch_info[idx].priority = 0xF;

        mac_hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].
          e_ul_logch_info[idx].chan_type = UE_LOGCHAN_MAX_NUM;

      mac_hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].
        e_ul_logch_info[idx].include_in_scheduling_info = FALSE;
      mac_hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].
        e_ul_logch_info[idx].num_rlc_size_ddi_info = 0;
#ifdef FEATURE_MAC_I
      mac_hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].
        e_ul_logch_info[idx].rlc_pdu_type = RLC_INVALID_PDU_SIZE;
      mac_hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].
        e_ul_logch_info[idx].ul_li_size = LI_UNSPECIFIED;
      mac_hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].
        e_ul_logch_info[idx].ul_flex_min_pdu_size = RRC_UL_INVALID_RLC_SIZE;
      mac_hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].
        e_ul_logch_info[idx].ul_flex_max_pdu_size = RRC_UL_INVALID_RLC_SIZE;
#endif /* FEATURE_MAC_I */	
    }

  
    mac_hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].
      e_mac_d_flow_id = INVALID_DFLOW_ID;

    mac_hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].
      e_mac_d_flow_power_offset = 0xFF;

    mac_hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].
      e_mac_d_flow_max_num_of_retx = 0xFF;

    mac_hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].
      e_mac_d_flow_mux_list = 0xFF;

    mac_hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].
      e_tx_grant_info = MAC_E_TX_GRANT_NOT_DEFINED;

    mac_hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].
      non_sched_tx_grant.non_sched_tx_grant_harq_allocation_2ms = 0xFF;

    mac_hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].
      non_sched_tx_grant.max_mac_e_pdu_size_in_bits = 0;
  }
}

/*====================================================================
FUNCTION: rrcllc_init_hsrach_info()

DESCRIPTION:
  This function initializes all the HS-RACH variables.

DEPENDENCIES:


RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_hsrach_info
(
  ordered_config_type *config_ptr,
  rrc_state_e_type next_rrc_state
)
{

  config_ptr->ack_nack_support_on_hs_dpcch = FALSE;  
    /*Initialize MAC structure*/
  rrcllc_init_mac_hsrach_info(&config_ptr->mac_hs_rach_config);

    /*Initialize L1 EDL structure*/  
  rrcllc_init_l1_hsrach_edl_info(&config_ptr->l1_hs_rach_e_info.e_dl_info);

    /*Initialize L1 EUL structure*/    
  rrcllc_init_l1_hsrach_eul_info(&config_ptr->l1_hs_rach_e_info.e_ul_info);

/*   initialize l1 hs dpcch structure*/
  rrcllc_init_l1_hsrach_hsdpcch_info(&config_ptr->l1_hsdpa_info.hs_dpcch_info,next_rrc_state);

/*   initialize l1 fdpch structure */  
  rrcllc_init_l1_hsrach_fdpch_info(&config_ptr->l1_dl_chan_parms.hs_rach_dl_fdpch_info);

/*   initialize l1 aich structure */    
  rrcllc_init_l1_hsrach_aich_info(&config_ptr->l1_dl_chan_parms.hs_rach_aich);

/*   initialize l1 common edch resource list*/      
  rrcllc_init_l1_hsrach_common_edch_resource_list(&config_ptr->common_e_dch_resource_list);

/*   initialize l1 ul dpch structure */        
  rrcllc_init_l1_hsrach_ul_dpch_info(&config_ptr->l1_ul_chan_parms.hs_ul_dpch_cfg);

/*   initialize l1 prach structure */          
  rrcllc_init_l1_hsrach_prach_info(&config_ptr->l1_ul_chan_parms.hs_prach_cfg);

  if(next_rrc_state == RRC_STATE_DISCONNECTED)
  {
    config_ptr->l1_hs_rach_e_info.e_dl_info.primary_e_rnti_present = FALSE;
  }

}
/*====================================================================
FUNCTION: rrchsrach_nv_supports_hs_rach()

DESCRIPTION:
This function checks whether the NV supports HSRACH operation or not

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrchsrach_nv_supports_hs_rach
(
  void
)
{
  /* Check to see NV HSUPA Category to ascertain UE has such HSUPA category which can support HSRACH as per spec quote below -.
   *  ==== TS25.306 ====
   *
   *   Support of common E-DCH
   *   Defines whether the UE supports E-DCH enhanced random access in CELL_FACH state and Idle mode. If the UE supports common E-DCH then the UE shall also support
   *  o	 MAC-i/is
   *  o	 FDD E-DCH physical layer category 2, 4, 6 or 7
   *  o	 Enhanced F-DPCH
   *  o	 HS-PDSCH in CELL_FACH
   *
   *   It is further confirmed by Sam/Alan that UE supporting Category 8/9 should also mean that UE supports category 6
   *   ==> So UE which supports category 8/9 MAY support HSRACH.
   */

#ifdef FEATURE_DUAL_SIM
  uint16 nv_value = rrc_get_concurrency_nv();
#endif

  if (
      (wcdma_nv_hsupa_category == 0)||
      (wcdma_nv_hsupa_category == 1)||
      (wcdma_nv_hsupa_category == 3)||
      (wcdma_nv_hsupa_category == 5)
     )
  {
    WRRC_MSG1_MED("RRCHSRACH:hsupa_category %d does not allow HSRACH",wcdma_nv_hsupa_category);
    return FALSE;
  }

  if(!(RRC_IS_HSRACH_TURNED_ON) )
  {
    WRRC_MSG3_MED("RRCHSRACH NV disabled HSFACH NV %d MACI NV %d HSRACH NV %d",
                ((rrc_feature_nv_item_value & RRC_EFACH_ENABLED)?1:0),
                ((rrc_feature_nv_item_value & RRC_MAC_I_ENABLED)?1:0),
                ((rrc_feature_nv_item_value & RRC_HSRACH_ENABLED)?1:0));
    return FALSE;
  }

#ifdef FEATURE_DUAL_SIM
  if(CHECK_CONCURRENCY_FOR_HSRACH(nv_value))
  {
    WRRC_MSG1_MED("RRCHSRACH : HSRACH NV is disabled for device_mode %d", wcdma_device_mode);
    return FALSE;
  }
#endif

  return TRUE;

}
/*====================================================================
FUNCTION: rrchsrach_camped_cell_supports_hsrach()

DESCRIPTION:
This function checks whether the currently camped cell supports HSRACH or not

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrchsrach_camped_cell_supports_hsrach
(
  void
)
{
  rrc_SysInfoType5    *sib5_ptr = NULL;

  if(rrchsrach_nv_supports_hs_rach() == FALSE)
  {
    WRRC_MSG0_HIGH("RRCHSRACH:HSRACH NV doesnt suppport HSRACH");
    return FALSE;
  }
  
  if(FALSE == rrcenhstate_camped_cell_supports_e_fach())
  {
    return FALSE;
  }
  else
  {
    sib5_ptr = (rrc_SysInfoType5 *)rrc_sibdb_return_sib_for_srv_cell(rrc_SIB5);
  
    if (sib5_ptr == NULL)
    {
       WRRC_MSG0_ERROR("RRCHSRACH:SIB 5 not present in SIB DB for ");
       return FALSE;
    }
  
    if(COMMON_EDCH_SYS_INFO_PRESENT_IN_SIB5(sib5_ptr))
    {
      WRRC_MSG0_MED("RRCHSRACH:EDCH Common Sys Info is present");
      return TRUE;
    }
    else
    {
      WRRC_MSG0_MED("RRCHSRACH:EDCH Common Sys Info not present");
      return FALSE;
    }
  }
}
/*====================================================================
FUNCTION: rrchsrach_find_e_mac_d_flow_idx()

DESCRIPTION:
  Find a EDCH MAC-d flow Id in common flow id list

DEPENDENCIES:
  None.

RETURN VALUE:
  uint8 -> A EDCH MAC-d flow Index.
  UE_MAX_MAC_D_FLOW -> Given EDCH Mac-d flow does not
           have an allocated DCH transport channel index.

SIDE EFFECTS:
====================================================================*/
uint8  rrchsrach_find_e_mac_d_flow_idx(ordered_config_type *config_ptr, uint8 e_mac_d_flow_id)
{
  uint8 cnt = MAX_E_DCH_MAC_D_FLOW;

  for (cnt =0; cnt < MAX_E_DCH_MAC_D_FLOW; cnt++)
  {
    if (config_ptr->mac_hs_rach_config.hs_rach_config.e_mac_d_flow_info[cnt].e_mac_d_flow_id == e_mac_d_flow_id)
    {
      break;
    }
  }

  return cnt;
}
/*====================================================================
FUNCTION: rrchsrach_allocate_e_mac_d_flow_index()

DESCRIPTION:
  Allocates a EDCH MAC-d flow Id.

DEPENDENCIES:
  None.

RETURN VALUE:
  uint8 -> A EDCH MAC-d flow Index.
  UE_MAX_MAC_D_FLOW -> Given EDCH Mac-d flow does not
           have an allocated DCH transport channel index.

SIDE EFFECTS:
====================================================================*/
uint8  rrchsrach_allocate_e_mac_d_flow_index(ordered_config_type *config_ptr)
{
  uint8 cnt = MAX_E_DCH_MAC_D_FLOW;

  for (cnt =0; cnt < MAX_E_DCH_MAC_D_FLOW; cnt++)
  {
    if (config_ptr->mac_hs_rach_config.hs_rach_config.e_mac_d_flow_info[cnt].e_mac_d_flow_id == INVALID_DFLOW_ID)
    {
      break;
    }
  }
  return cnt;
}

/*============================================================================
FUNCTION: rrchsrach_convert_asn1_edch_transmit_cont_offset()

DESCRIPTION:
  This function converts the ENUM type received in reconfig message to actual ms
  delay in happyBitDelayCondition.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  happy bit delay condition in ms

SIDE EFFECTS:
=============================================================================*/
static uint8 rrchsrach_convert_asn1_edch_max_ccch_resource_alloc
(
    rrc_CommonEDCHSystemInfoFDD_max_CCCH_ResourceAllocation max_ccch_resourcealloc
)
{
   
  uint8 max_ccch_resource_alloc = 8;

  switch(max_ccch_resourcealloc)
  {
    case rrc_CommonEDCHSystemInfoFDD_max_CCCH_ResourceAllocation_tti8:
      max_ccch_resource_alloc = 8;
      break;

    case rrc_CommonEDCHSystemInfoFDD_max_CCCH_ResourceAllocation_tti12:
      max_ccch_resource_alloc = 12;
      break;

    case rrc_CommonEDCHSystemInfoFDD_max_CCCH_ResourceAllocation_tti16:
      max_ccch_resource_alloc = 16;
      break;

    case rrc_CommonEDCHSystemInfoFDD_max_CCCH_ResourceAllocation_tti20:
      max_ccch_resource_alloc = 20;
      break;

    case rrc_CommonEDCHSystemInfoFDD_max_CCCH_ResourceAllocation_tti24:
      max_ccch_resource_alloc = 24;
      break;

    case rrc_CommonEDCHSystemInfoFDD_max_CCCH_ResourceAllocation_tti32:
      max_ccch_resource_alloc = 32;
      break;

    case rrc_CommonEDCHSystemInfoFDD_max_CCCH_ResourceAllocation_tti40:
      max_ccch_resource_alloc = 40;
      break;

    case rrc_CommonEDCHSystemInfoFDD_max_CCCH_ResourceAllocation_tti80:
      max_ccch_resource_alloc = 80;
      break;

    default:
      WRRC_MSG1_ERROR("RRCHSRACH UnKnown Happy bit delay condition: %d", max_ccch_resource_alloc);
      break;
  }

  return(max_ccch_resource_alloc);
}
/*====================================================================
FUNCTION: rrchsrach_get_e_mac_dflow_idx()

DESCRIPTION:
  This function gets a common EDCH mac-d flow Index for the specified mac-d flow. 
  If the the mac-d flow already existed, its index is returned. Otherwise a 
  new index is allocated for the mac-d ID. If a new mac-d flowindex is allocated,
  e_mac_d_flow count in MAC structure is updated.

DEPENDENCIES:
  None.

RETURN VALUE:
  Index value if successful else
  UE_MAX_MAC_D_FLOW if no index is available

SIDE EFFECTS:

====================================================================*/
uint8 rrchsrach_get_e_mac_dflow_idx
(
  ordered_config_type *config_ptr,
  uint8  e_mac_d_flow_id
)
{
  uint8 index = MAX_E_DCH_MAC_D_FLOW;

  index =  rrchsrach_find_e_mac_d_flow_idx(config_ptr,e_mac_d_flow_id);

  /* E-MAC-d flow index not found, Allocate new index*/
  index = rrchsrach_allocate_e_mac_d_flow_index(config_ptr);

  if(MAX_E_DCH_MAC_D_FLOW == index)
  {
    WRRC_MSG0_ERROR("RRCHSRACH:No of Mac-d dlows more than MAX_E_DCH_MAC_D_FLOW ");  
    return MAX_E_DCH_MAC_D_FLOW;
  }
  else
  {
    /* Now Save the MAC-d flow ID in MAC structure */
    config_ptr->mac_hs_rach_config.hs_rach_config.e_mac_d_flow_info[index].e_mac_d_flow_id = e_mac_d_flow_id;
    
    /* Increment no of DFLOWs */
    config_ptr->mac_hs_rach_config.hs_rach_config.num_e_mac_d_flow++;
    
  }
  return(index);   
} /* rrcllc_get_dflow_index */

/*===========================================================================
FUNCTION rrchsrach_process_common_edch_flowlist

DESCRIPTION
This function processes the common edch flowlist in common edch sys info  and stores in
the MAC database
  
DEPENDENCIES

RETURN VALUE
SUCCESS if everything is valid, FAILURE otherwise

SIDE EFFECTS
  None
===========================================================================*/
static uecomdef_status_e_type rrchsrach_process_common_edch_flowlist
(
  ordered_config_type *config_ptr,
  rrc_Common_E_DCH_MAC_d_FlowList* edch_flow_info
)
{
  uint8 edch_flow_idx, idx=0;  

  mac_e_config_s_type * hs_rach_config_ptr = &config_ptr->mac_hs_rach_config.hs_rach_config;
   
  while ((edch_flow_info->n > idx) && (idx < MAX_E_DCH_MAC_D_FLOW))
  {
    if(edch_flow_info->elem[idx].mac_d_FlowIdentity > (INVALID_DFLOW_ID-1))
    {
      WRRC_MSG1_ERROR("RRCHSRACH:MAC I Flow Id %d, Range 0..7",
          edch_flow_info->elem[idx].mac_d_FlowIdentity);
      return FAILURE;
    }
  
    /* If flow id is already present then return it else allocate a new index */
    if ((edch_flow_idx = rrchsrach_get_e_mac_dflow_idx(config_ptr,(uint8)edch_flow_info->elem[idx].mac_d_FlowIdentity)) 
      == MAX_E_DCH_MAC_D_FLOW)
    {
      WRRC_MSG0_HIGH("RRCHSRACH  Unable to allocate index in EDCH flow list  ");
      return FAILURE;
    }
    else
    {
      /*Fill the configured parameters for the flow id*/
      hs_rach_config_ptr->e_mac_d_flow_info[edch_flow_idx].e_mac_d_flow_power_offset =edch_flow_info->elem[idx].mac_d_FlowPowerOffset;
      hs_rach_config_ptr->e_mac_d_flow_info[edch_flow_idx].e_mac_d_flow_max_num_of_retx = edch_flow_info->elem[idx].mac_d_FlowMaxRetrans;

    /* 8.6.5.24
    	  1> if the IE "E-DCH MAC-d flow multiplexing list" is included:
    2> only multiplex MAC-d PDUs from the E-DCH MAC-d flow indicated in the IE "E-DCH MAC-d flow
    identity" with MAC-d PDUs from E-DCH MAC-d flows with which multiplexing in the same MAC-i PDU
    is allowed in accordance to the IE "E-DCH MAC-d flow multiplexing list".
    1> else (IE "E-DCH MAC-d flow multiplexing list" not included in the system information):
    2> allow multiplexing of MAC-d PDUs from the E-DCH MAC-d flow indicated in the IE "E-DCH MAC-d flow
    identity" with MAC-d PDUs from any other E-DCH MAC-d flow in the same MAC-i PDU.
    2> for FDD, prohibit multiplexing of MAC-c PDUs from the E-DCH MAC-c flow indicated in the IE "E-DCH
    MAC-d flow identity" with MAC-d PDUs from any other E-DCH MAC-d flow in the same MAC-i PDU.
    */

      if (HS_RACH_CCCH_D_FLOW_ID == hs_rach_config_ptr->e_mac_d_flow_info[edch_flow_idx].e_mac_d_flow_id)
      {
        hs_rach_config_ptr->e_mac_d_flow_info[edch_flow_idx].e_mac_d_flow_mux_list = 0x1;
      }
      else
      {
        if(edch_flow_info->elem[idx].m.mac_d_FlowMultiplexingListPresent)
        {
          hs_rach_config_ptr->e_mac_d_flow_info[edch_flow_idx].e_mac_d_flow_mux_list = 
                       rrcllcpcie_convert_asn1_oss_bitstring_to_uint8(&edch_flow_info->elem[idx].mac_d_FlowMultiplexingList);
        }
        else
        {
          /*Bit 0 is the first/leftmost bit of the bit string. bit0 corresponds to mac-d flow 0*/
            hs_rach_config_ptr->e_mac_d_flow_info[edch_flow_idx].e_mac_d_flow_mux_list = 0xFE;
       }
      }
      /* HS-RACH is always with scheduled grant */
      hs_rach_config_ptr->e_mac_d_flow_info[edch_flow_idx].e_tx_grant_info = MAC_E_TX_GRANT_SCHEDULED;	  
    }
    idx++;
  }
  return SUCCESS;
}


/*===========================================================================
FUNCTION rrchsrach_check_for_edch_flow_in_sib5

DESCRIPTION
Checks for flow-ID presense in SIB5

DEPENDENCIES

RETURN VALUE
TRUE if if present
SIDE EFFECTS
  None
===========================================================================*/

boolean rrchsrach_check_for_edch_flow_in_sib5
(
  uint8 mac_e_flow_id
)
{

  rrc_CommonEDCHSystemInfoFDD *common_edch_sys_info_ptr = NULL;
  rrc_SysInfoType5    *sib5_ptr = NULL;
  int idx =0;
  rrc_Common_E_DCH_MAC_d_FlowList *edch_flow_info ;
  sib5_ptr = (rrc_SysInfoType5 *) rrc_sibdb_return_sib_for_srv_cell(rrc_SIB5);

  if (sib5_ptr == NULL)
  {
     WRRC_MSG0_ERROR("RRCHSRACH: SIB 5 not present in SIB DB for serving cell ");
     return FALSE;
  }
  common_edch_sys_info_ptr = &sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.\
      v690NonCriticalExtensions.v770NonCriticalExtensions.v860NonCriticalExtensions.v890NonCriticalExtensions.v8b0NonCriticalExtensions.\
      v8d0NonCriticalExtensions.sysInfoType5_v8d0ext.commonEDCHSystemInfoFDD;

  edch_flow_info = &(common_edch_sys_info_ptr->common_E_DCH_MAC_d_FlowList);

  for(idx =0;edch_flow_info->n > idx;idx++)
  {
    if(edch_flow_info->elem[idx].mac_d_FlowIdentity == mac_e_flow_id)
    {
      return TRUE;
    }
  }

  return FALSE;

}



/*===========================================================================
FUNCTION rrchsrach_set_edch_common_queue_info

DESCRIPTION
This function fills the MAC database for SRB 0
  
DEPENDENCIES

RETURN VALUE
  SUCCESS/FAILURE

SIDE EFFECTS
  None
===========================================================================*/
static uecomdef_status_e_type rrchsrach_update_rb_mapping_info_for_ul_ccch_srb0
(
  ordered_config_type *config_ptr
)
{
  mac_e_config_s_type *hs_rach_config_ptr = &(config_ptr->mac_hs_rach_config.hs_rach_config);
  uint8 idx=INVALID_DFLOW_ID;  

  if((idx = rrchsrach_find_e_mac_d_flow_idx(config_ptr,HS_RACH_CCCH_D_FLOW_ID))
  	== MAX_E_DCH_MAC_D_FLOW)
  {
    WRRC_MSG0_HIGH("RRCHSRACH MAC-D Flow ID 7 not configured in SIB5");
    return FAILURE;
  }
  	
    
/*Only 1 logical channel mapped onto Flow ID 7 */
hs_rach_config_ptr->e_mac_d_flow_info[idx].num_logch = 1;
hs_rach_config_ptr->e_mac_d_flow_info[idx].e_ul_logch_info[hs_rach_config_ptr->e_mac_d_flow_info[idx].num_logch-1].rb_id = 
	CCCH_RADIO_BEARER_ID;

  /*rlc_id will be filled later while adding CCCh */

hs_rach_config_ptr->e_mac_d_flow_info[idx].e_ul_logch_info[hs_rach_config_ptr->e_mac_d_flow_info[idx].num_logch-1].lc_id = 
HS_RACH_CCCH_LCH_ID;

hs_rach_config_ptr->e_mac_d_flow_info[idx].e_ul_logch_info[hs_rach_config_ptr->e_mac_d_flow_info[idx].num_logch-1].rlc_mode = 
	UE_MODE_TRANSPARENT;

//Maximum priority for CCCH 
hs_rach_config_ptr->e_mac_d_flow_info[idx].e_ul_logch_info[0].priority = 1;

hs_rach_config_ptr->e_mac_d_flow_info[idx].e_ul_logch_info[hs_rach_config_ptr->e_mac_d_flow_info[idx].num_logch-1].chan_type = 
UE_LOGCHAN_CCCH;

/*Anju agreed that MAC would ignore the rlc_pdu_type, ul_li_size, ul_flex_min_pdu_size,ul_flex_max_pdu_size. Response from Anju Yes - MAC ignores these for CCCH/TM*/
hs_rach_config_ptr->e_mac_d_flow_info[idx].e_ul_logch_info[hs_rach_config_ptr->e_mac_d_flow_info[idx].num_logch-1].rlc_pdu_type = 
RLC_FIXED_PDU_SIZE;

  return SUCCESS;
}
/*====================================================================
FUNCTION        rrchsrach_copy_prach_partitioning_from_prachsysinfo()

DESCRIPITON     This function takes a pointer to the PRACH Partitioning
                info IE and copies it to ORDERED_CONFIG.

DEPENDENCIES    None

RETURN VALUE    SUCCESS if everything is valid, FAILURE otherwise

SIDE EFFECTS    The current Ordered Config data is overwritten.
                The num_of_asc is updated to reflect the number of
                asc's rcv'd over the air.
====================================================================*/
uecomdef_status_e_type rrchsrach_copy_prach_partitioning_from_prachsysinfo
(
  l1_ul_asc_struct_type  *hsrach_asc_info,
  l1_ul_prach_cfg_struct_type * prach_cfg_ptr,
  l1_ul_asc_struct_type  *asc_info,
  uint8 num_of_asc
)
{
  /* Loop counter */
  uint8 n_index;

  n_index = 0;
  
  while((n_index < MAX_ASC) && (n_index <num_of_asc))
  {
    /* save the pointer */
      prach_cfg_ptr->asc_cfg.asc_info_ptr[n_index] =
        &hsrach_asc_info[n_index];

    /* get all of the signature and sub-channel indicies */
    hsrach_asc_info[n_index].avail_sig_end_idx = asc_info[n_index].avail_sig_end_idx;
  
    hsrach_asc_info[n_index].avail_sig_start_idx = asc_info[n_index].avail_sig_start_idx;
  
  
    hsrach_asc_info[n_index].assigned_sub_chan_num = asc_info[n_index].assigned_sub_chan_num;

    n_index++;

  }
  /* We now have the number of ASCs supported in num_of_asc */
  return SUCCESS;

} /* end get_prach_partitioning */

/*====================================================================
FUNCTION        rrchsrach_get_prach_partitioning()

DESCRIPITON     This function takes a pointer to the PRACH Partitioning
                info IE and copies it to HSRACH CONFIG.
DEPENDENCIES    None

RETURN VALUE    SUCCESS if everything is valid, FAILURE otherwise

SIDE EFFECTS    The current Ordered Config data is overwritten.
                The num_of_asc is updated to reflect the number of
                asc's rcv'd over the air.
====================================================================*/
static uecomdef_status_e_type rrchsrach_get_prach_partitioning
(
  l1_ul_asc_struct_type  *asc_info,
  l1_ul_prach_cfg_struct_type * prach_cfg_ptr,
  rrc_PRACH_Partitioning_r7   *ie_ptr,
  uint8                    *num_of_asc
)
{
  /* Loop counter */
  uint32 i, n_index, n_value;

  /* Pointer to the ASC data when extracting Partitioning Info */
  struct rrc_PRACH_Partitioning_r7_fdd     *asc_ptr;

  if(RRC_CHECK_COMMON_MSG_TYPE_PTR(ie_ptr,
              rrc_PRACH_Partitioning_r7_fdd))
  {
    /* Assign pointer to FDD values */
    asc_ptr = ie_ptr->u.fdd;
  }
  else
  {
    /* Don't support TDD */
    WRRC_MSG0_ERROR("RRCHSRACH No FDD values for PRACH partitioning");
    return(FAILURE);
  }
  
  /* Get all the Partitioning Data */
  if(asc_ptr == NULL)
  {
    ERR_FATAL("RRCHSRACH Invalid ASC PTR received",0,0,0);
  }
  //Added check to fix the KW error
  if(*num_of_asc == 0)
  {
    ERR_FATAL("RRCHSRACH Invalid num of ASC received",0,0,0);
  }

  n_index = 0;
  n_value = asc_ptr->n;
  
  while((n_value != 0) && (*num_of_asc <= MAX_ASC))
  {
    /* save the pointer */
      prach_cfg_ptr->asc_cfg.asc_info_ptr[(*num_of_asc)-1] =
        &asc_info[(*num_of_asc)-1];

    if (RRC_CHECK_MSG_TYPE_IE(asc_ptr->elem[n_index], 
                accessServiceClass_FDDPresent))
    {
      /* get all of the signature and sub-channel indicies */
      asc_info[(*num_of_asc)-1].avail_sig_end_idx = (uint8)
        asc_ptr->elem[n_index].accessServiceClass_FDD.availableSignatureEndIndex;

      asc_info[(*num_of_asc)-1].avail_sig_start_idx = (uint8)
        asc_ptr->elem[n_index].accessServiceClass_FDD.availableSignatureStartIndex;

      /* get the assigned subchannel number. L1 will do some magic
         to get the correct value */
      asc_info[(*num_of_asc)-1].assigned_sub_chan_num = (uint8)
       ((*asc_ptr->elem[n_index].accessServiceClass_FDD.assignedSubChannelNumber.data) >> 4);
    
    } /* end if ASC present */
    else
    {
      /* Access Service Class data is absent for this ASC */
      /* Set default values for the 1st Access Service Class */
      if(*num_of_asc == 1)
      {
        /* For the 1st ASC, set it to default value based on the available
         * signatures listed in PRACH info (for RACH).  This is done by simply
         * counting the bits in the available signature mask.  The low
         * available signature index in this case is always zero, and the high
         * available signature index is the count of available indices minus
         * one.
         */
        uint16 sig_index;
        uint8 avail_sig_count = 0;
        uint16 avail_sig_mask =
          prach_cfg_ptr->avail_sig_mask;

        for(sig_index = 0; sig_index < RRCLLC_MAX_AVAILABLE_SIGNATURE; sig_index++)
        {
          if (avail_sig_mask & (1 << sig_index)) {
            avail_sig_count++;
          }
        }

        /* Complain and fail if there were no signatures available. */
        if(avail_sig_count == 0)
        {
          WRRC_MSG0_ERROR("RRCHSRACH No signatures available for default processing.");
          return(FAILURE);
        }

        /* Indicate availability of signatures as represented by the bit string
         * above.
         */
        asc_info[(*num_of_asc)-1].avail_sig_end_idx =
                          avail_sig_count - 1;
        asc_info[(*num_of_asc)-1].avail_sig_start_idx =
                          0;

        /* Set the mask for assigned subchannels to default value of
         * all available sub-channels.
         */
        asc_info[(*num_of_asc)-1].assigned_sub_chan_num =
                          RRCLLC_ASC0_DEFAULT_ASSIGNED_SUBCHAN_MASK;

      }
      else
      {
        /* For any ASC other than ASC0, copy data from previous Access
         * Service Class */
        asc_info[(*num_of_asc)-1].avail_sig_end_idx =
                asc_info[(*num_of_asc)-2].avail_sig_end_idx;
        asc_info[(*num_of_asc)-1].avail_sig_start_idx =
                asc_info[(*num_of_asc)-2].avail_sig_start_idx;
        asc_info[(*num_of_asc)-1].assigned_sub_chan_num =
                asc_info[(*num_of_asc)-2].assigned_sub_chan_num;

      }
    }

    /* get the next element */
    (*num_of_asc)++;
    n_value--;
    n_index++;
  } /* end while getting all Partioning Mode info */

  /* Reset the number of Access Service class back to correct value */
  (*num_of_asc)--;

  /* Fill out to the end of the array with copies of the last value */
  for(i = *num_of_asc; i < MAX_ASC; i++)
  {
    asc_info[i].avail_sig_end_idx =
            asc_info[(*num_of_asc)-1].avail_sig_end_idx;
    asc_info[i].avail_sig_start_idx =
            asc_info[(*num_of_asc)-1].avail_sig_start_idx;
    asc_info[i].assigned_sub_chan_num =
            asc_info[(*num_of_asc)-1].assigned_sub_chan_num;

  }

  /* We now have the number of ASCs supported in num_of_asc */
  return SUCCESS;

} /* end get_prach_partitioning */


/*====================================================================
FUNCTION        rrchsrach_get_prach_pwr_offset()

DESCRIPITON     This function takes a pointer to the PRACH Power Offset
                info IE and copies it to HSRACH ORDERED_CONFIG.

DEPENDENCIES    None

RETURN VALUE    SUCCESS if everything is valid, FAILURE otherwise

SIDE EFFECTS    Previous values in Ordered Config are overwritten.
====================================================================*/
static uecomdef_status_e_type rrchsrach_get_prach_pwr_offset
(
  ordered_config_type  *config_ptr,
  rrc_PRACH_PowerOffset *prach_pwr_offset
)
{
  /* Get the Power Ramp Step */
  if((prach_pwr_offset->powerRampStep >= RRCLLC_MIN_PWR_RAMP_STEP) &&
     (prach_pwr_offset->powerRampStep <= RRCLLC_MAX_PWR_RAMP_STEP)
    )
  {
    config_ptr->l1_ul_chan_parms.hs_prach_cfg.pwr_offset_P0 = (int8)
      prach_pwr_offset->powerRampStep;
  }
  else
  {
    WRRC_MSG1_ERROR("RRCHSRACH Power Ramp Step %d is out of range", prach_pwr_offset->powerRampStep);
    return FAILURE;
  }

  /* Get the Preamble Retrans Max */
  if((prach_pwr_offset->preambleRetransMax >= RRCLLC_MIN_PREAMBLE_RETRANS_MAX) &&
     (prach_pwr_offset->preambleRetransMax <= RRCLLC_MAX_PREAMBLE_RETRANS_MAX)
    )
  {
    config_ptr->l1_ul_chan_parms.hs_prach_cfg.max_preamble_tx = (uint8)
      prach_pwr_offset->preambleRetransMax;
  }
  else
  {
    WRRC_MSG1_ERROR("RRCHSRACH Preamble Retrans Max %d is out of range",
      prach_pwr_offset->preambleRetransMax);
    return FAILURE;
  }

  return SUCCESS;
} /* end get_get_prach_pwr_offset() */

/*====================================================================
FUNCTION        rrchsrach_get_rach_xmit_parms()

DESCRIPITON     This function takes a pointer to the RACH Xmit parms
                info IE and copies it to HSRACH ORDERED_CONFIG.

DEPENDENCIES    None

RETURN VALUE    SUCCESS if everything is valid, FAILURE otherwise

SIDE EFFECTS    Previous values in Ordered Config are overwritten.
====================================================================*/
static uecomdef_status_e_type rrchsrach_get_rach_xmit_parms
(
  ordered_config_type  *config_ptr,
  rrc_RACH_TransmissionParameters *rach_xmit_parms
)
{

  mac_ul_rach_txcntl_config_type *rach_txcntl_config_ptr = &config_ptr->mac_hs_rach_config.mac_ul_rach_txcntl;
  
  /* Get the Max # of RACH preamble ramping cycles */
  if((rach_xmit_parms->mmax >= RRCLLC_MIN_MMAX) &&
     (rach_xmit_parms->mmax <= RRCLLC_MAX_MMAX)
    )
  {
    rach_txcntl_config_ptr->mmax = (uint8)
      rach_xmit_parms->mmax;
  }
  else
  {
    WRRC_MSG1_ERROR("RRCHSRACH Max # of Preamble Cyles %d is out of range", rach_xmit_parms->mmax);
    return FAILURE;
  }

  /* Get the lower bound of backoff time (units of 10ms) */
  if(rach_xmit_parms->nb01Min <= RRCLLC_MAX_NB01)
  {
    rach_txcntl_config_ptr->nbo1min = (uint8)
      rach_xmit_parms->nb01Min;
  }
  else
  {
    WRRC_MSG1_ERROR("RRCHSRACH Backoff Time lower bound %d is out of range", rach_xmit_parms->nb01Min);
    return FAILURE;
  }

  /* Get the upper bound of backoff time (units of 10ms) */
  if(rach_xmit_parms->nb01Max <= RRCLLC_MAX_NB01)
  {
    rach_txcntl_config_ptr->nbo1max = (uint8)
      rach_xmit_parms->nb01Max;
  }
  else
  {
    WRRC_MSG1_ERROR("RRCHSRACH Backoff Time upper bound %d is out of range", rach_xmit_parms->nb01Max);
    return FAILURE;
  }

  return SUCCESS;
} /* end get_rach_xmit_parms() */


/*====================================================================
FUNCTION        rrchsrach_get_aich_info()

DESCRIPITON     This function takes a pointer to the AICH
                info IE and copies it to HSRACH ORDERED_CONFIG.

DEPENDENCIES    None

RETURN VALUE    SUCCESS if everything is valid, FAILURE otherwise

SIDE EFFECTS    Previous values in Ordered Config are overwritten.
====================================================================*/
static uecomdef_status_e_type rrchsrach_get_aich_info
(
  ordered_config_type  *config_ptr,
  rrc_AICH_Info *aich_info_ptr
)
{

l1_aich_parms_struct_type *hs_rach_aich_ptr = &config_ptr->l1_dl_chan_parms.hs_rach_aich;
  
  /* Get the channelization code, same as orthoganol variable spreading factor (ovsf)  */
  hs_rach_aich_ptr->ovsf = (uint8)
  aich_info_ptr->channelisationCode256;

  /* Get the Standard tranmist Time Diversity (STTD) indicator status */
  hs_rach_aich_ptr->tx_div_ind = aich_info_ptr->sttd_Indicator;

  /* Get AICH Transmission Timing */
  switch(aich_info_ptr->aich_TransmissionTiming)
  {
    case rrc_AICH_TransmissionTiming_e0:
      hs_rach_aich_ptr->tt = L1_DL_AICH_TT_0;
      break;

    case rrc_AICH_TransmissionTiming_e1:
      hs_rach_aich_ptr->tt = L1_DL_AICH_TT_1;
      break;

    default:
      WRRC_MSG1_ERROR("RRCHSRACH AICH Transmission Time %d is out of range",
        aich_info_ptr->aich_TransmissionTiming);
      return FAILURE;
  } /* end switch AICH Transmission Timing */

 hs_rach_aich_ptr->pwr_offset = config_ptr->l1_dl_chan_parms.aich.pwr_offset;

  return SUCCESS;
} /* end get_aich_info() */

/*====================================================================
FUNCTION        rrchsrach_get_primary_cpich_tx_pwr()

DESCRIPITON     This function takes a pointer to the primary CPICH Tx
                Power info IE and copies it to ORDERED_CONFIG.

DEPENDENCIES    None

RETURN VALUE    SUCCESS if everything is valid, FAILURE otherwise

SIDE EFFECTS    Previous values in Ordered Config are overwritten.
====================================================================*/
static uecomdef_status_e_type rrchsrach_get_primary_cpich_tx_pwr
(
  ordered_config_type *config_ptr,
  rrc_PrimaryCPICH_TX_Power pri_cpich_tx_pwr
)
{
  if((pri_cpich_tx_pwr >= RRCLLC_MIN_PRI_CPICH_TX_PWR) &&
     (pri_cpich_tx_pwr <= RRCLLC_MAX_PRI_CPICH_TX_PWR)
    )
  {
    config_ptr->rrc_ul_pwr_init_parms.cpich_tx_pwr = (int8) pri_cpich_tx_pwr;
    return SUCCESS;
  }
  else
  {
    WRRC_MSG1_ERROR("RRCHSRACH Primary CPICH TX Power %d is out of range", pri_cpich_tx_pwr);
    return FAILURE;
  }
} /* end get_primary_cpich_tx_pwr() */

/*====================================================================
FUNCTION        rrchsrach_get_constant_value()

DESCRIPITON     This function takes a pointer to the Constant Value
                info IE and copies it to ORDERED_CONFIG.

DEPENDENCIES    None

RETURN VALUE    SUCCESS if everything is valid, FAILURE otherwise

SIDE EFFECTS    Previous values in Ordered Config are overwritten.
====================================================================*/
static uecomdef_status_e_type rrchsrach_get_constant_value
(
  ordered_config_type *config_ptr,
  rrc_ConstantValue const_val
)
{
  if((const_val >= RRCLLC_MIN_CONSTANT_VALUE) &&
     (const_val <= RRCLLC_MAX_CONSTANT_VALUE)
    )
  {
    config_ptr->rrc_ul_pwr_init_parms.const_val = (int8) const_val;
    return (SUCCESS);
  }
  else
  {
    WRRC_MSG1_ERROR("RRCHSRACH Constant Value %d is out of range", const_val);
    return (FAILURE);
  }
} /* end get_constant_value() */

/*====================================================================
FUNCTION        rrchsrach_select_asc_from_ac()

DESCRIPITON     This function selects the ASC from the UE's access
                service class. It uses the AC to ASC mapping to come up
                with the chosen ASC.

DEPENDENCIES    AC to ASC mapping should be present.
                UE's Access class should be available.

RETURN VALUE    None

SIDE EFFECTS    None
====================================================================*/
static void  rrchsrach_select_asc_from_ac(ordered_config_type *config_ptr)
{
  /* loop index variables */
  uint8 ac_map_index;
  uint8 ac_index;

  /* mask to test which hi access classes are set */
  rrc_hi_access_class_type ac_mask;

  if(rrcrce_get_rrc_est_cause() == RRC_EST_EMERGENCY_CALL) 
  {
    config_ptr->mac_hs_rach_config.ccch_asc =
      0;
  }
  /* If we have no low access class, then we use the second mapping in the
     table.  This is the special "Access Class 10" case referred to in
     section 4.4 of 22.011. */
  else if(rrc_lo_access_class == RRC_LO_ACCESS_CLASS_ABSENT)
  {
    config_ptr->mac_hs_rach_config.ccch_asc  =
      config_ptr->rrc_hsrach_info.ac_to_asc_mapping[1];
  }
  /* If we only have a low access class (0-9), then we always use the first
     mapping in the table, as per section 8.5.13 of 25.331. */
  else if(rrc_hi_access_class == RRC_HI_ACCESS_CLASS_ABSENT)
  {
    config_ptr->mac_hs_rach_config.ccch_asc  =
      config_ptr->rrc_hsrach_info.ac_to_asc_mapping[0];
  }
  /* Otherwise, we use the mapping for the highest-numbered access class, as
     per section 8.5.13 of 25.331. */
  else
  {
    ac_map_index = MAX_ASC_MAP - 1;
    ac_mask = RRC_ACCESS_CLASS_15_BIT;
    for(ac_index = (uint8)RRC_INT_ACCESS_CLASS_15_INDEX;
        ac_index >= (uint8)RRC_INT_ACCESS_CLASS_11_INDEX;
        ac_index--, ac_map_index--, ac_mask >>= 1)
    {
      if(rrc_hi_access_class & ac_mask)
      {
        config_ptr->mac_hs_rach_config.ccch_asc  =
          config_ptr->rrc_hsrach_info.ac_to_asc_mapping[ac_map_index];
        break;
      }
    }
  }
}

/*====================================================================
FUNCTION        rrchsrach_get_persist_scaling_factors()

DESCRIPITON     This function takes a pointer to the Persistence Scaling
                Factor info IE and copies it to HSRACH ORDERED_CONFIG.

DEPENDENCIES    None

RETURN VALUE    SUCCESS if everything is valid, FAILURE otherwise

SIDE EFFECTS    Dynamic Persistence Values from SIB 7 are then needed
                to correctly calculate the Persistence Value needed
                by MAC.
====================================================================*/
static uecomdef_status_e_type rrchsrach_get_persist_scaling_factors
(
  ordered_config_type  *config_ptr,
  rrc_PersistenceScalingFactorList  *ie_ptr
)
{
  /* local variables */
  uint32 index = 0, n_index = 0, n_value = 0;
  rrc_PersistenceScalingFactorList  *psflist_ptr = ie_ptr;
  	

  if(psflist_ptr == NULL)
  {
    ERR_FATAL("RRCHSRACH Null pointer received",0,0,0);
  }
  
  n_value = psflist_ptr->n;
  n_index = 0;
  while((n_value !=0) && (index < MAX_ASC_PERSIST))
  {
    /*
    * All the Persistence scaling factors a real numbers in the range
    * from 0.2 to 0.9. The stored values are stored in the ordered config
    * data base according to the following equation:
    *
    * ordered_config psf values = (rcv'd values * 10).
    *
    */
    /* Since this is sent as an enumerated type, time to do the mapping */
    switch(psflist_ptr->elem[n_index])
    {
      case rrc_PersistenceScalingFactor_psf0_2:
        config_ptr->rrc_hsrach_info.psf[index] = (uint32) 2;
        break;
      case rrc_PersistenceScalingFactor_psf0_3:
        config_ptr->rrc_hsrach_info.psf[index] = (uint32) 3;
        break;
      case rrc_PersistenceScalingFactor_psf0_4:
        config_ptr->rrc_hsrach_info.psf[index] = (uint32) 4;
        break;
      case rrc_PersistenceScalingFactor_psf0_5:
        config_ptr->rrc_hsrach_info.psf[index] = (uint32) 5;
        break;
      case rrc_PersistenceScalingFactor_psf0_6:
        config_ptr->rrc_hsrach_info.psf[index] = (uint32) 6;
        break;
      case rrc_PersistenceScalingFactor_psf0_7:
        config_ptr->rrc_hsrach_info.psf[index] = (uint32) 7;
        break;
      case rrc_PersistenceScalingFactor_psf0_8:
        config_ptr->rrc_hsrach_info.psf[index] = (uint32) 8;
        break;
      case rrc_PersistenceScalingFactor_psf0_9:
        config_ptr->rrc_hsrach_info.psf[index] = (uint32) 9;
        break;
      default:
        WRRC_MSG1_ERROR("RRCHSRACH Invalid Persistance Scaling Factor enum %d",psflist_ptr->elem[n_index]);
        return FAILURE;
    } /* end switch */

    /*
    * Note that the first 2 values are calculated outside of this loop, since they
    * are known values.
    */
    /* get the next element */
    n_value--;
    n_index++;
    index++;
  } /* end while MAX_ASC_PERSIST */

  /* Set the default value for the rest of ASCs */
  if(index < MAX_ASC_PERSIST)
  {
    do
    {
      config_ptr->rrc_hsrach_info.psf[index] = RRCLLC_DEFAULT_PERSISTENCE_SCALING_FACTOR_VAL;
      index++;
    }while(index < MAX_ASC_PERSIST);
  }

  return SUCCESS;
} /* end get_persist_scaling_factors() */

/*====================================================================
FUNCTION        rrchsrach_get_ac_to_asc_mapping()

DESCRIPITON     This function takes a pointer to the AC to ASC mapping
                info IE and copies it to ORDERED_CONFIG.

DEPENDENCIES    None

RETURN VALUE    SUCCESS if everything is valid, FAILURE otherwise

SIDE EFFECTS    Previous values in Ordered Config are overwritten.
====================================================================*/
static uecomdef_status_e_type rrchsrach_get_ac_to_asc_mapping
(
  ordered_config_type  *config_ptr,
  rrc_AC_To_ASC_MappingTable        *ie_ptr
)
{
  /* local variables */
  uint32 index = 0, n_value = 0, n_index = 0;
  rrc_AC_To_ASC_MappingTable   *ascmplist_ptr = ie_ptr;

  /**********************************************************************/
  /* The Access service class mapping follows the table below:          */
  /* ------------------------------------------------------------------ */
  /* |   AC   | 0 - 9 |   10  |   11  |   12  |   13  |   14  |   15  | */
  /* |--------|-------|-------|-------|-------|-------|-------|-------| */
  /* |  ASC   | 1stIE | 2ndIE | 3rdIE | 4thIE | 5thIE | 6thIE | 7thIE | */
  /* ------------------------------------------------------------------ */
  /**********************************************************************/
  /* In other words if the UE supports only AC 0-9 then it selects the  */
  /* Access Service Class given in the first array element.             */
  /* If the UE only supports Access Class 15, then the UE selects the   */
  /* Access Service Class given in the last array element.              */
  /* The IE values are just stored here, and the RRC Connection         */
  /* Establishment procedure must do this mapping correctly.            */
  /**********************************************************************/
  if(ascmplist_ptr == NULL)
  {
    ERR_FATAL("RRCHSRACH NULL pointer received",0,0,0);
  } 

  n_value = ascmplist_ptr->n;
  n_index = 0;
  
  while((n_value != 0) && (index < MAX_ASC_MAP))
  {
    if(ascmplist_ptr->elem[n_index] <= RRCLLC_MAX_AC_TO_ASC_MAPPING)
    {
      config_ptr->rrc_hsrach_info.ac_to_asc_mapping[index] = (uint8)ascmplist_ptr->elem[n_index];
    }
    else
    {
      WRRC_MSG1_ERROR("RRCHSRACH AC to ASC Mapping value %d is out of range",ascmplist_ptr->elem[n_index]);
      return(FAILURE);
    }

    /* Get the next element */
    n_value--;
    n_index++;
    index++;
  } /* end while */

  return SUCCESS;
} /* end get_ac_to_asc_mapping() */

/*===========================================================================
FUNCTION rrchsrach_process_prach_preamble_enhanceduplink

DESCRIPTION
This function processes the IEs present in prach preamble . If not present 
fill the elements from legacy sib 5 structures
  
DEPENDENCIES
When this function is called legacy SIB5/SIB7 should have been processed

RETURN VALUE
SUCCESS if everything is valid, FAILURE otherwise

SIDE EFFECTS
  None
===========================================================================*/
static uecomdef_status_e_type rrchsrach_process_prach_preamble_enhanceduplink
(
  ordered_config_type *config_ptr,
  rrc_PRACH_PreambleForEnhancedUplink* prach_preamble_info_ptr,
  rrc_SysInfoType5 *sib5_ptr
)
{

  uint16 available_subchannels = 0;

  /* count of the number of Access Service Classes (ASC) included */
  uint8                               num_of_asc = 1;
      /* index of ASCs for calculation of Persistence values */
  uint8                               asc_idx;
	    /* calculated Persistence value */
  uint32                              pval = 0;

  uint16 available_signature   = 0;

  uint8 ix=0;

  rrc_PRACH_SystemInformationList *prach_sys_info_ptr=NULL;
  
  rrc_PRACH_SystemInformation* prach_sys_info_first_ptr = NULL;
  

  l1_ul_prach_cfg_struct_type *hs_prach_cfg_ptr = &config_ptr->l1_ul_chan_parms.hs_prach_cfg;
	
  /* Init the ASC data pointers */
  for(ix = 0; ix < MAX_ASC; ix ++)
  { 
    config_ptr->l1_ul_chan_parms.hs_prach_cfg.asc_cfg.asc_info_ptr[ix] =
      &config_ptr->rrc_hsrach_info.asc_info[ix];
  } /* end ASC initialization */


  prach_sys_info_ptr = &sib5_ptr->prach_SystemInformationList;

  /*select the first prach index*/
  if((prach_sys_info_ptr != NULL) && (prach_sys_info_ptr->n >0))
  {
    prach_sys_info_first_ptr = prach_sys_info_ptr->elem;
  }

  if (prach_sys_info_first_ptr == NULL)
  {
    WRRC_MSG0_HIGH("First prach sysinfo list is not present in SIB data base");
    return(FAILURE);
  }


	
/*Process the available signature */
  if(prach_preamble_info_ptr->m.availableSignaturesPresent)
  {
    available_signature = (uint16)((*prach_preamble_info_ptr->availableSignatures.data) << 8);
    available_signature |= (uint16)(*(prach_preamble_info_ptr->availableSignatures.data + 1));
    hs_prach_cfg_ptr->avail_sig_mask = available_signature;
  }
  else 
  {
    /* As per 10.3.6.54a PRACH preamble control parameters (for Enhanced Uplink) The default value is
    the inverse of the bitstring indicated in the IE "Available Signature" in the IE "PRACH Info (for RACH).*/
    if(T_rrc_PRACH_RACH_Info_modeSpecificInfo_fdd == prach_sys_info_first_ptr->prach_RACH_Info.modeSpecificInfo.t)
    {
      available_signature = (uint16)((*prach_sys_info_first_ptr->prach_RACH_Info.modeSpecificInfo.u.fdd->availableSignatures.data) << 8);
      available_signature |= (uint16)(*(prach_sys_info_first_ptr->prach_RACH_Info.modeSpecificInfo.u.fdd->availableSignatures.data + 1));
      hs_prach_cfg_ptr->avail_sig_mask = ~(available_signature);
    }
  }

  if (0 == hs_prach_cfg_ptr->avail_sig_mask)
  {
    WRRC_MSG0_ERROR("RRCHSRACH: avail_sig_mask 0 for a HS-RACH cell in  SIB5");
    return FAILURE;
  }
  
/*Fill the e ai indication */
  hs_prach_cfg_ptr->hs_rach_info.e_ai_indication= prach_preamble_info_ptr->e_ai_Indication;


/*Next element sf_min not applicable */
/*Fill PSCWN */
  if(prach_preamble_info_ptr->m.preambleScramblingCodeWordNumberPresent)
  {
    hs_prach_cfg_ptr->sc_num = prach_preamble_info_ptr->preambleScramblingCodeWordNumber;
  }
  else
  {
    if((T_rrc_PRACH_RACH_Info_modeSpecificInfo_fdd == prach_sys_info_first_ptr->prach_RACH_Info.modeSpecificInfo.t) && 
      (prach_sys_info_first_ptr->prach_RACH_Info.modeSpecificInfo.u.fdd !=NULL))
    {
      hs_prach_cfg_ptr->sc_num = prach_sys_info_first_ptr->prach_RACH_Info.modeSpecificInfo.u.fdd->preambleScramblingCodeWordNumber;
  }
  }


/*Next element punct_lim not applicable */
/*Fill ASCN */
  if(prach_preamble_info_ptr->m.availableSubChannelNumbersPresent)
  {
    available_subchannels = (uint16)((*prach_preamble_info_ptr->availableSubChannelNumbers.data) << 8);
    available_subchannels |= (uint16)(*(prach_preamble_info_ptr->availableSubChannelNumbers.data + 1));
    available_subchannels = (uint16) (available_subchannels >> 4);
    hs_prach_cfg_ptr->avail_sub_chan_mask = available_subchannels;
  }
  else
  {
    if((T_rrc_PRACH_RACH_Info_modeSpecificInfo_fdd == prach_sys_info_first_ptr->prach_RACH_Info.modeSpecificInfo.t) && 
      (prach_sys_info_first_ptr->prach_RACH_Info.modeSpecificInfo.u.fdd !=NULL))
    {
      available_subchannels = (uint16)((*prach_sys_info_first_ptr->prach_RACH_Info.modeSpecificInfo.u.fdd->availableSubChannelNumbers.data) << 8);
      available_subchannels |= (uint16)(*(prach_sys_info_first_ptr->prach_RACH_Info.modeSpecificInfo.u.fdd->availableSubChannelNumbers.data + 1));
      available_subchannels = (uint16) (available_subchannels >> 4);
      hs_prach_cfg_ptr->avail_sub_chan_mask  = available_subchannels;
    }
  }


/*Process preamble partitioning IE*/
  if(prach_preamble_info_ptr->m.prach_PartitioningPresent)
  {
    if( FAILURE == rrchsrach_get_prach_partitioning(config_ptr->rrc_hsrach_info.asc_info,
                          &config_ptr->l1_ul_chan_parms.hs_prach_cfg,
                          &prach_preamble_info_ptr->prach_Partitioning,&num_of_asc))
    {
      WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting PRACH preamble info from SIB5");
      return FAILURE;
    }
    /* Give L1 the number of ASCs rcv'd over the air */
    config_ptr->l1_ul_chan_parms.hs_prach_cfg.asc_cfg.num_asc = (uint8)num_of_asc;	
  }
  else
  {
    if(prach_sys_info_first_ptr->m.prach_PartitioningPresent)
    {
        if( FAILURE == rrchsrach_get_prach_partitioning(config_ptr->rrc_hsrach_info.asc_info,
                            &config_ptr->l1_ul_chan_parms.hs_prach_cfg,
                                                        (rrc_PRACH_Partitioning_r7 *)&prach_sys_info_first_ptr->prach_Partitioning,&num_of_asc))
      {
        WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting PRACH preamble info from SIB5");
        return FAILURE;
      }
      /* Give L1 the number of ASCs rcv'd over the air */
      config_ptr->l1_ul_chan_parms.hs_prach_cfg.asc_cfg.num_asc = (uint8)num_of_asc;	
    }
  }


/*Process persistance scaling factor IE*/
  if(prach_preamble_info_ptr->m.persistenceScalingFactorListPresent)
  {
    if( FAILURE == rrchsrach_get_persist_scaling_factors(config_ptr,
                          &prach_preamble_info_ptr->persistenceScalingFactorList))
    {
      WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting PRACH preamble info from SIB5");
      return FAILURE;
    }
  }
  else
  {
    if(prach_sys_info_first_ptr->m.persistenceScalingFactorListPresent)
    {
      if( FAILURE == rrchsrach_get_persist_scaling_factors(config_ptr,
                            &prach_sys_info_first_ptr->persistenceScalingFactorList))
      {
        WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting PRACH preamble info from SIB5");
        return FAILURE;
      }
    }
  }


  /*Fill MAC ASC and ASC structure*/
    /* Setup the Persistence values for all ASCs */
    /*ASC_IDX ranges from 0 to 7 which needs to be mapped to persitence
      value Pi(1 to 0) according to spec: 8.5.12. Pi is maintained internally
      as (0xFFFF to 0) so ASC_IDX(0 to 7) -> pval(0xFFFF to 0)*/
    for(asc_idx = 0; ((asc_idx < num_of_asc)&& (asc_idx < MAX_ASC)); asc_idx++)
    {
      switch(asc_idx)
      {
        case 0:
          pval = RRCLLC_PVAL_MULTIPLICATION_FACTOR;
          break;
        case 1:
          pval = RRCLLC_PVAL_MULTIPLICATION_FACTOR>>(config_ptr->dpl - 1);
          break;
        default:
          /*psf from spec goes from 0.9..0.2, we have scaled it to 9...2(factor of 10), this is why we 
              divide by RRCLLC_DEFAULT_PERSISTENCE_SCALING_FACTOR_VAL*/
          pval = ((RRCLLC_PVAL_MULTIPLICATION_FACTOR>>(config_ptr->dpl - 1)) * config_ptr->rrc_hsrach_info.psf[asc_idx - 2])/
                  RRCLLC_DEFAULT_PERSISTENCE_SCALING_FACTOR_VAL; 
          break;
      }
      config_ptr->mac_hs_rach_config.mac_ul_rach_txcntl.asc[asc_idx].pval = pval;
      if(asc_idx >= 2)
      {
         MSG_9(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"asc_idx: %d  pval: %d mul factor: %d dpl: %d psf: %d scaling factor: %d",
                                           asc_idx,
                                           pval,
                                           RRCLLC_PVAL_MULTIPLICATION_FACTOR,
                                           config_ptr->dpl,
                                           config_ptr->rrc_hsrach_info.psf[asc_idx - 2],
                                           RRCLLC_DEFAULT_PERSISTENCE_SCALING_FACTOR_VAL,
                                           0,
                                           0,
                                           0
                                           );
      }
    }
    
    /* Indicate the number of Access Service Classes supported */
    config_ptr->mac_hs_rach_config.mac_ul_rach_txcntl.nASC = num_of_asc;


    /* Process the AC to ASC mapping table IE */
    if(prach_preamble_info_ptr->m.ac_To_ASC_MappingTablePresent)
    {
      if( FAILURE == rrchsrach_get_ac_to_asc_mapping(config_ptr,
                            &prach_preamble_info_ptr->ac_To_ASC_MappingTable))
      {
        WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting PRACH preamble info from SIB5");
        return FAILURE;
      }
    }
    else
    {
      if(prach_sys_info_first_ptr->m.ac_To_ASC_MappingTablePresent)
      {
        if( FAILURE == rrchsrach_get_ac_to_asc_mapping(config_ptr,
                              &prach_sys_info_first_ptr->ac_To_ASC_MappingTable))
        {
          WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting PRACH preamble info from SIB5");
          return FAILURE;
        }
      }
    }

    rrchsrach_select_asc_from_ac(config_ptr);

    /* Process the CPICH Tx power IE */
    if(prach_preamble_info_ptr->m.primaryCPICH_TX_PowerPresent)
    {
      if( FAILURE == rrchsrach_get_primary_cpich_tx_pwr(config_ptr,
                            prach_preamble_info_ptr->primaryCPICH_TX_Power))
      {
        WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting PRACH preamble info from SIB5");
        return FAILURE;
      }
    }
    else
    {
      if((prach_sys_info_first_ptr->modeSpecificInfo.t == T_rrc_PRACH_SystemInformation_modeSpecificInfo_fdd) &&
        (prach_sys_info_first_ptr->modeSpecificInfo.u.fdd!=NULL) &&
        (prach_sys_info_first_ptr->modeSpecificInfo.u.fdd->m.primaryCPICH_TX_PowerPresent))
      {
        if( FAILURE == rrchsrach_get_primary_cpich_tx_pwr(config_ptr,
                              prach_sys_info_first_ptr->modeSpecificInfo.u.fdd->primaryCPICH_TX_Power))
        {
          WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting PRACH preamble info from SIB5");
          return FAILURE;
        }
      }
    }

    /* Process the Constant value IE */
    if(prach_preamble_info_ptr->m.constantValuePresent)
    {
      if( FAILURE == rrchsrach_get_constant_value(config_ptr,
                            prach_preamble_info_ptr->constantValue))
      {
        WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting PRACH preamble info from SIB5");
        return FAILURE;
      }
    }
    else
    {
      if((prach_sys_info_first_ptr->modeSpecificInfo.t == T_rrc_PRACH_SystemInformation_modeSpecificInfo_fdd) &&
        (prach_sys_info_first_ptr->modeSpecificInfo.u.fdd !=NULL) &&
        (prach_sys_info_first_ptr->modeSpecificInfo.u.fdd->m.constantValuePresent))
      {
        if( FAILURE == rrchsrach_get_constant_value(config_ptr,
                              prach_sys_info_first_ptr->modeSpecificInfo.u.fdd->constantValue))
        {
          WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting PRACH preamble info from SIB5");
          return FAILURE;
        }
      }
    }

    /* Process the prach PO IE */
    if(prach_preamble_info_ptr->m.prach_PowerOffsetPresent)
    {
      if( FAILURE == rrchsrach_get_prach_pwr_offset(config_ptr,
                            &prach_preamble_info_ptr->prach_PowerOffset))
      {
        WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting PRACH preamble info from SIB5");
        return FAILURE;
      }
    }
    else
    {
      if((prach_sys_info_first_ptr->modeSpecificInfo.t == T_rrc_PRACH_SystemInformation_modeSpecificInfo_fdd) &&
        (prach_sys_info_first_ptr->modeSpecificInfo.u.fdd !=NULL) &&
        (prach_sys_info_first_ptr->modeSpecificInfo.u.fdd->m.prach_PowerOffsetPresent))
      {
        if( FAILURE == rrchsrach_get_prach_pwr_offset(config_ptr,
                              &prach_sys_info_first_ptr->modeSpecificInfo.u.fdd->prach_PowerOffset))
        {
          WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting PRACH preamble info from SIB5");
          return FAILURE;
    }
      }
    }
	

    /* Process the rach tx params IE */
    if(prach_preamble_info_ptr->m.rach_TransmissionParametersPresent)
    {
      if( FAILURE == rrchsrach_get_rach_xmit_parms(config_ptr,
                            &prach_preamble_info_ptr->rach_TransmissionParameters))
      {
        WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting PRACH preamble info from SIB5");
        return FAILURE;
      }
    }
    else
    {
       if((prach_sys_info_first_ptr->modeSpecificInfo.t == T_rrc_PRACH_SystemInformation_modeSpecificInfo_fdd) &&
         (prach_sys_info_first_ptr->modeSpecificInfo.u.fdd!=NULL) &&
         (prach_sys_info_first_ptr->modeSpecificInfo.u.fdd->m.rach_TransmissionParametersPresent))
      {
        if( FAILURE == rrchsrach_get_rach_xmit_parms(config_ptr,
                              &prach_sys_info_first_ptr->modeSpecificInfo.u.fdd->rach_TransmissionParameters))
        {
          WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting PRACH preamble info from SIB5");
          return FAILURE;
        }
      }
    }


    /* Process the AICH info IE */
    if(prach_preamble_info_ptr->m.aich_InfoPresent)
    {
      if( FAILURE == rrchsrach_get_aich_info(config_ptr,
                            &prach_preamble_info_ptr->aich_Info))
      {
        WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting PRACH preamble info from SIB5");
        return FAILURE;
      }
    }
    else
    {
       if((prach_sys_info_first_ptr->modeSpecificInfo.t == T_rrc_PRACH_SystemInformation_modeSpecificInfo_fdd) &&
         (prach_sys_info_first_ptr->modeSpecificInfo.u.fdd!=NULL) &&
         (prach_sys_info_first_ptr->modeSpecificInfo.u.fdd->m.aich_InfoPresent))
      {
        if( FAILURE == rrchsrach_get_aich_info(config_ptr,
                             &prach_sys_info_first_ptr->modeSpecificInfo.u.fdd->aich_Info))
        {
          WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting PRACH preamble info from SIB5");
          return FAILURE;
        }
      }
    }


    hs_prach_cfg_ptr->hs_rach_info.powerOffsetPp_e = prach_preamble_info_ptr->powerOffsetPp_e;
	
  return SUCCESS;
}

/*============================================================================
FUNCTION: rrchsrach_process_e_dpcch_info()

DESCRIPTION:
  This function updates IE "rrc_E_DPCCH_Info_r7" in HSRACH ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
static uecomdef_status_e_type rrchsrach_process_e_dpcch_info
(
  ordered_config_type *config_ptr,
  rrc_E_DPCCH_Info_r7*e_dpcch_info_ptr
)
{

  l1_e_ul_info_struct_type *e_ul_info_ptr = &config_ptr->l1_hs_rach_e_info.e_ul_info;
  mac_hs_rach_config_s_type *mac_hs_rach_config_ptr = &config_ptr->mac_hs_rach_config;

  e_ul_info_ptr->e_dpcch_info.e_dpcch_pwr_offset =  e_dpcch_info_ptr->e_DPCCH_DPCCH_PowerOffset;

  if(e_dpcch_info_ptr->m.e_DPDCH_PowerInterpolationPresent)
  {
    e_ul_info_ptr->e_dpcch_info.e_dpdch_pwr_interpolation =  e_dpcch_info_ptr->e_DPDCH_PowerInterpolation;
  }

#ifdef FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST
    if(TRUE == rrcueci_nv_supports_edpcch_pwr_boost())
    { 
      if(e_dpcch_info_ptr->m.e_TFC_Boost_InfoPresent)
      {
        e_ul_info_ptr->e_dpcch_info.e_tfc_boost_info.e_tfci_boost_val =  e_dpcch_info_ptr->e_TFC_Boost_Info.e_TFCI_Boost;
  	  
        if(e_dpcch_info_ptr->e_TFC_Boost_Info.m.delta_T2TPPresent)
        {
          e_ul_info_ptr->e_dpcch_info.e_tfc_boost_info.delta_t2tp  = e_dpcch_info_ptr->e_TFC_Boost_Info.delta_T2TP;
        }
        else
        {
           if(e_dpcch_info_ptr->e_TFC_Boost_Info.e_TFCI_Boost != 127 )
           {
              WRRC_MSG0_ERROR("RRCHSRACH :delta_T2TP mandatory if e_TFCI_Boost not 127");
              return FAILURE;
           }
           /*Invalid value. L1 shld not look into it.*/
           e_ul_info_ptr->e_dpcch_info.e_tfc_boost_info.delta_t2tp = 0xFF;
         }
     
       }
    }
#else
      WRRC_MSG0_ERROR("RRCHSRACH: e_TFCI_Boost_present not supported ");
#endif

    mac_hs_rach_config_ptr->hs_rach_config.e_dch_info.e_dpcch_info.happy_bit_delay_in_ms = 
      rrcllcpcie_convert_asn1_happy_bit_delay_condn_to_ms(
      e_dpcch_info_ptr->happyBit_DelayCondition);

  return SUCCESS;
}


/*============================================================================
FUNCTION: rrchsrach_process_reference_e_tfcis()

DESCRIPTION:
  This function processes reference E-TFCIs and store the same in e_ul_info for L1
      
DEPENDENCIES:
  NONE

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
static uecomdef_status_e_type rrchsrach_process_reference_e_tfcis
(
  rrc_E_DPDCH_Reference_E_TFCIList_r7 *reference_etfcis_ptr,
  l1_e_dpdch_info_struct_type *e_dpdch_info_ptr,
    ordered_config_type *config_ptr
)
{
  rrc_E_DPDCH_Reference_E_TFCIList_r7 *temp_ref_etfci_ptr;
  boolean sort_ref_tfci_list = FALSE;
  uint32 n_value = 0;
  uint32 n_index = 0;

  temp_ref_etfci_ptr = reference_etfcis_ptr;
  /* reset the number of reference e-tfci's to 0 */
  e_dpdch_info_ptr->num_ref_e_tfci = 0;

  n_value = temp_ref_etfci_ptr->n;
  while (n_value != 0)
  {
    if (L1_MAX_REF_E_TFCI_LIST <= e_dpdch_info_ptr->num_ref_e_tfci)
    {
      return(FAILURE);
    }
    
    e_dpdch_info_ptr->ref_e_tfci_list[
      e_dpdch_info_ptr->num_ref_e_tfci].ref_e_tfci = 
      temp_ref_etfci_ptr->elem[n_index].reference_E_TFCI;

    e_dpdch_info_ptr->ref_e_tfci_list[
      e_dpdch_info_ptr->num_ref_e_tfci].ref_e_tfci_pwr_offset = 
      temp_ref_etfci_ptr->elem[n_index].reference_E_TFCI_PO_r7;
 #ifdef FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST
    /*
     * According to 25.331 section 8.3.4.3
     * If E-TFCI boost is signalled to the UE and a reference E-TFCI <= E-TFCI Boost 
     * is signalled to the UE with a Reference E-TFCI PO of value 30 or 31, the UE behaviour is unspecified.
     */
    if(config_ptr->l1_hs_rach_e_info.e_ul_info.e_dpcch_info.e_tfc_boost_info.e_tfci_boost_val  != 0xff)
    {
      if(( e_dpdch_info_ptr->ref_e_tfci_list[e_dpdch_info_ptr->num_ref_e_tfci].ref_e_tfci
            <= config_ptr->l1_hs_rach_e_info.e_ul_info.e_dpcch_info.e_tfc_boost_info.e_tfci_boost_val)
            && (e_dpdch_info_ptr->ref_e_tfci_list[e_dpdch_info_ptr->num_ref_e_tfci].ref_e_tfci_pwr_offset > 29))
      {
        MSG_ERROR("RRCHSRACH :ref etfci %d more than etfci boost val %d and po value more than > 29",
                e_dpdch_info_ptr->ref_e_tfci_list[e_dpdch_info_ptr->num_ref_e_tfci].ref_e_tfci,
                config_ptr->l1_hs_rach_e_info.e_ul_info.e_dpcch_info.e_tfc_boost_info.e_tfci_boost_val,
                e_dpdch_info_ptr->ref_e_tfci_list[e_dpdch_info_ptr->num_ref_e_tfci].ref_e_tfci_pwr_offset);
        return(FAILURE);
      }    
    }
 #endif
    if(e_dpdch_info_ptr->num_ref_e_tfci > 0 )
    {
      if( e_dpdch_info_ptr->ref_e_tfci_list[e_dpdch_info_ptr->num_ref_e_tfci].ref_e_tfci < 
         e_dpdch_info_ptr->ref_e_tfci_list[e_dpdch_info_ptr->num_ref_e_tfci-1].ref_e_tfci)
      {
        MSG_ERROR("RRCHSRACH Ref ETFCI not in ascending order in OTA RefTFCI %d <  Next RefTFCI %d num ETFCI %d",
             e_dpdch_info_ptr->ref_e_tfci_list[e_dpdch_info_ptr->num_ref_e_tfci-1].ref_e_tfci,
             e_dpdch_info_ptr->ref_e_tfci_list[e_dpdch_info_ptr->num_ref_e_tfci].ref_e_tfci,
             e_dpdch_info_ptr->num_ref_e_tfci);
        sort_ref_tfci_list = TRUE;
      }
    }
    e_dpdch_info_ptr->num_ref_e_tfci++;

    n_value--;
    n_index++;
  }

  if(sort_ref_tfci_list == TRUE )
  {
    rrcllc_sort_ref_e_tfci_list(e_dpdch_info_ptr);
  }
  return(SUCCESS);
}

/*============================================================================
FUNCTION: rrchsrach_convert_asn1_periodicity_of_sched_info_grant()

DESCRIPTION:
  This function converts the ENUM type of rrc_E_DPDCH_PeriodicyOfSchedInfo in the 
  received in reconfig message to actual 'ms'.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  happy bit delay condition in ms

SIDE EFFECTS:
=============================================================================*/
static uecomdef_status_e_type rrchsrach_convert_asn1_periodicity_of_sched_info_grant
(
  rrc_E_DPDCH_PeriodicyOfSchedInfo rcvd_periodicity_of_sched_info,
  uint16 *periodicity_in_ms_ptr,
  ordered_config_type *config_ptr
)
{
  uint16 computed_periodicity_in_ms = 0;
  
  switch (rcvd_periodicity_of_sched_info)
  {
    case rrc_E_DPDCH_PeriodicyOfSchedInfo_everyEDCHTTI:
      if (E_TTI_10MS == config_ptr->mac_hs_rach_config.hs_rach_config.e_tti)
      {
        computed_periodicity_in_ms = 10;
      }
      else /* E_TTI_2MS */
      {
        computed_periodicity_in_ms = 2;
      }
      break;


    case rrc_E_DPDCH_PeriodicyOfSchedInfo_ms4:
      if (E_TTI_10MS == config_ptr->mac_hs_rach_config.hs_rach_config.e_tti)
      {
        computed_periodicity_in_ms = 10;
      }
      else
      {
        computed_periodicity_in_ms = 4;
      }
      break;

    case rrc_E_DPDCH_PeriodicyOfSchedInfo_ms10:
      computed_periodicity_in_ms = 10;
      break;

    case rrc_E_DPDCH_PeriodicyOfSchedInfo_ms20:
      computed_periodicity_in_ms = 20;
      break;

    case rrc_E_DPDCH_PeriodicyOfSchedInfo_ms50:
      computed_periodicity_in_ms = 50;
      break;

    case rrc_E_DPDCH_PeriodicyOfSchedInfo_ms100:
      computed_periodicity_in_ms = 100;
      break;

    case rrc_E_DPDCH_PeriodicyOfSchedInfo_ms200:
      computed_periodicity_in_ms = 200;
      break;

    case rrc_E_DPDCH_PeriodicyOfSchedInfo_ms500:
      computed_periodicity_in_ms = 500;
      break;

    case rrc_E_DPDCH_PeriodicyOfSchedInfo_ms1000:
      computed_periodicity_in_ms = 1000;
      break;

    default:
      WRRC_MSG1_ERROR("RRCHSRACH: received wrong periodicity of sched info: %d",
        rcvd_periodicity_of_sched_info);
      return(FAILURE);
  }

  *periodicity_in_ms_ptr = computed_periodicity_in_ms;
  return(SUCCESS);
}

/*============================================================================
FUNCTION: rrchsrach_process_sched_info_config()

DESCRIPTION:
  This function process scheduling information config and store the same in e_mac_info for MAC
      
DEPENDENCIES:
  NONE

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
static uecomdef_status_e_type rrchsrach_process_sched_info_config
(
  rrc_E_DPDCH_SchedulingInfoConfiguration *sched_info_config_ptr,
  mac_e_dpdch_info_s_type *mac_e_dpdch_info_ptr,
    ordered_config_type *config_ptr
  
)
{

  if (DEBUG_EUL_SI_ONLY_POWER_OFFSET != 0)
  {
    mac_e_dpdch_info_ptr->sched_info_config.power_offset_of_si = (uint8)
      DEBUG_EUL_SI_ONLY_POWER_OFFSET;
    WRRC_MSG1_ERROR("RRCHSRACH: DEBUG: hard coding si onlypo to %d", DEBUG_EUL_SI_ONLY_POWER_OFFSET);
  }
  else
  {
    mac_e_dpdch_info_ptr->sched_info_config.power_offset_of_si = 
      sched_info_config_ptr->powerOffsetForSchedInfo;
  }

  /* its a MD IE */
  mac_e_dpdch_info_ptr->sched_info_config.no_grant_periodicity_of_si = 0;
  mac_e_dpdch_info_ptr->sched_info_config.grant_periodicity_of_si = 0;

  if (RRC_CHECK_MSG_TYPE_IE_PTR(sched_info_config_ptr,
            periodicityOfSchedInfo_NoGrantPresent))
  {
    if (FAILURE == rrchsrach_convert_asn1_periodicity_of_sched_info_grant(
      sched_info_config_ptr->periodicityOfSchedInfo_NoGrant,
      &mac_e_dpdch_info_ptr->sched_info_config.no_grant_periodicity_of_si,
      config_ptr))
    {
      return(FAILURE);
    }
  }
  else
  {
      if(E_TTI_2MS == config_ptr->mac_hs_rach_config.hs_rach_config.e_tti)
      {
        mac_e_dpdch_info_ptr->sched_info_config.no_grant_periodicity_of_si = (uint16)
            RRCEUL_NO_GRANT_PERIODICITY_DEFAULT_VALUE_2MS;
      }
      else
      {
        mac_e_dpdch_info_ptr->sched_info_config.no_grant_periodicity_of_si = (uint16)
          RRCEUL_NO_GRANT_PERIODICITY_DEFAULT_VALUE;
      }
  }

  if (RRC_CHECK_MSG_TYPE_IE_PTR(sched_info_config_ptr,
               periodicityOfSchedInfo_GrantPresent))
  {
    if (FAILURE == rrchsrach_convert_asn1_periodicity_of_sched_info_grant(
      sched_info_config_ptr->periodicityOfSchedInfo_Grant,
      &mac_e_dpdch_info_ptr->sched_info_config.grant_periodicity_of_si,config_ptr))
    {
      return(FAILURE);
    }
  }

  return(SUCCESS);
}

/*============================================================================
FUNCTION: rrchsrach_process_two_and_three_step_threshold_info()

DESCRIPTION:
  This function process rcvd two and three step threshold and store the same in 
  e_dl_info for L1
      
DEPENDENCIES:
  NONE

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
static uecomdef_status_e_type rrchsrach_process_two_and_three_step_threshold_info
(
  OSS_UINT32 three_index_step_threshold_present,
  OSS_UINT32 three_index_step_threshold,
  OSS_UINT32 two_index_step_threshold_present,
  OSS_UINT32 two_index_step_threshold,
  l1_e_dl_hs_rach_info_struct_type *e_dl_info_ptr
)
{
  if (three_index_step_threshold_present || two_index_step_threshold_present)
  {
    if (three_index_step_threshold_present)
    {
      e_dl_info_ptr->grant_config.three_index_step_threshold = 
        three_index_step_threshold;
    }

    if (two_index_step_threshold_present)
    {
      e_dl_info_ptr->grant_config.two_index_step_threshold = 
        two_index_step_threshold;
    }

    if (TRUE == DEBUG_EUL_RRC_VALIDATE_STEP_INDEX_THRESHOLD)
    {
      /* 
         As per 25.321:
         -     if SGLUPR < "3-index-step threshold":
           -     Serving_Grant = SG[MIN(SGLUPR + 3 , 37)].
         -     if "3-index-step threshold" <= SGLUPR < "2-index-step threshold":
           -     Serving_Grant = SG[MIN(SGLUPR + 2 , 37)].
         -     if "2-index-step threshold" <= SGLUPR::
           -     Serving_Grant = SG[MIN(SGLUPR + 1 , 37)].
         From above algorithm, 3-index has to be less than or equal to 2-index step threshold
      */
      if ((e_dl_info_ptr->grant_config.three_index_step_threshold > 
           e_dl_info_ptr->grant_config.two_index_step_threshold) &&
          /* 2-index or 3-index step threshold value range is 0..37. 0 is the default value 
             hence ignore this value as well. Below check is only for 3-index because we
             are concenred about 3-index step threshold only */
          (e_dl_info_ptr->grant_config.three_index_step_threshold < 38) &&
          (e_dl_info_ptr->grant_config.three_index_step_threshold > 0) &&
          /* there is a possibility that 3-index is received and 2-index is not received from the NW */
          (e_dl_info_ptr->grant_config.two_index_step_threshold > 0))
      {
        WRRC_MSG2_ERROR("RRCHSRACH: Invalid 3-Index(%d) and 2-IndexStep(%d) combination. 3-index should be less than 2-index",
          e_dl_info_ptr->grant_config.three_index_step_threshold,
          e_dl_info_ptr->grant_config.two_index_step_threshold
          );
        return(FAILURE);
      }
    }

  }

  return(SUCCESS);
}


/*============================================================================
FUNCTION: rrchsrach_process_e_dpdch_info()

DESCRIPTION:
  This function updates IE "dl per rl info" in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
static uecomdef_status_e_type rrchsrach_process_e_dpdch_info
(
  ordered_config_type *config_ptr,
  rrc_E_DPDCH_Info_r8 *e_dpdch_info_ptr
)
{
  l1_e_ul_info_struct_type *e_ul_info_ptr = &config_ptr->l1_hs_rach_e_info.e_ul_info;
  mac_hs_rach_config_s_type *mac_hs_rach_config_ptr = &config_ptr->mac_hs_rach_config;
  l1_e_dl_hs_rach_info_struct_type *e_dl_info_ptr = &config_ptr->l1_hs_rach_e_info.e_dl_info;
  
	
    e_ul_info_ptr->e_dpdch_info.e_tfci_table_idx = 
      e_dpdch_info_ptr->e_TFCI_TableIndex;

    mac_hs_rach_config_ptr->hs_rach_config.e_dch_info.e_dpdch_info.e_tfci_table_index = 
      e_dpdch_info_ptr->e_TFCI_TableIndex;    

    if (FAILURE == rrcllcpcie_convert_asn1_e_dch_max_ch_codes(
      e_dpdch_info_ptr->maxChannelisationCodes,
      &e_ul_info_ptr->e_dpdch_info))
    {
      return(FAILURE);
    }

    /* multiply received value by 4 - as per rrc-l1 interface requirements */
    e_ul_info_ptr->e_dpdch_info.e_dpdch_pl_non_max = 
      e_dpdch_info_ptr->pl_NonMax << 2;

    if (FAILURE == rrchsrach_process_reference_e_tfcis(
        &e_dpdch_info_ptr->reference_E_TFCIs,
        &e_ul_info_ptr->e_dpdch_info,config_ptr))
    {
      return(FAILURE);
    }

    if (FAILURE == rrchsrach_process_sched_info_config(
        &e_dpdch_info_ptr->schedulingInfoConfiguration,
        &mac_hs_rach_config_ptr->hs_rach_config.e_dch_info.e_dpdch_info,config_ptr))
    {
      return(FAILURE);
    }

    if (RRC_CHECK_MSG_TYPE_IE_PTR(e_dpdch_info_ptr, 
                 e_DCH_MinimumSet_E_TFCIPresent))
    {
      mac_hs_rach_config_ptr->hs_rach_config.e_dch_info.e_dpdch_info.e_tfci_min_set =
        e_dpdch_info_ptr->e_DCH_MinimumSet_E_TFCI;
    }
    else
    {
      mac_hs_rach_config_ptr->hs_rach_config.e_dch_info.e_dpdch_info.e_tfci_min_set = 
        MAC_E_TFCI_MIN_SET_NOT_PRESENT;
    }

    /* reset the threshold because its a MD IE */
    e_dl_info_ptr->grant_config.three_index_step_threshold = 
      L1_SG_STEP_THRESHOLD_NOT_PRESENT;
    e_dl_info_ptr->grant_config.two_index_step_threshold = 
      L1_SG_STEP_THRESHOLD_NOT_PRESENT;
	
    if (FAILURE == rrchsrach_process_two_and_three_step_threshold_info(
        RRC_CHECK_MSG_TYPE_IE_PTR(e_dpdch_info_ptr, 
                  threeIndexStepThresholdPresent),
        e_dpdch_info_ptr->threeIndexStepThreshold,
        RRC_CHECK_MSG_TYPE_IE_PTR(e_dpdch_info_ptr, 
                 twoIndexStepThresholdPresent),
        e_dpdch_info_ptr->twoIndexStepThreshold,
        e_dl_info_ptr)
       )
    {
      return(FAILURE);
    }

    if (RRC_CHECK_MSG_TYPE_IE_PTR(e_dpdch_info_ptr,minReduced_E_DPDCH_GainFactorPresent))
    {
      e_ul_info_ptr->e_dpdch_info.min_beta_ed =
      rrcllcpcie_convert_asn1_minreduced_e_dpdch_gainfactor(
      e_dpdch_info_ptr->minReduced_E_DPDCH_GainFactor);
    }
    else
    {
       e_ul_info_ptr->e_dpdch_info.min_beta_ed = L1_E_MIN_BED_08; /* 8/15 default */
    }

    
  return(SUCCESS);
}

/*============================================================================
FUNCTION: rrchsrach_update_edl_info()

DESCRIPTION:
  This function updates the remaining elements in EDL structure
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
static uecomdef_status_e_type rrchsrach_update_edl_info
(
  l1_e_dl_hs_rach_info_struct_type *e_dl_info_ptr,
  rrc_CommonEDCHSystemInfoFDD *common_edch_sys_info_ptr
)
{

    e_dl_info_ptr->grant_config.grant_selector = L1_E_PRIMARY_GRANT;

  return(SUCCESS);
}

/*============================================================================
FUNCTION: rrchsrach_process_uldpch_powerctrl_info()

DESCRIPTION:
  This function updates IE "rrc_UL_DPCHpowerControlInfoForCommonEDCH" in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
static uecomdef_status_e_type rrchsrach_process_uldpch_powerctrl_info
(
  ordered_config_type *config_ptr,
  rrc_UL_DPCHpowerControlInfoForCommonEDCH *ul_dpch_pwrctrl_info_ptr
)
{
l1_ul_hs_dpcch_info_struct_type *hs_dpcch_info_ptr= &config_ptr->l1_hsdpa_info.hs_dpcch_info;
l1_ul_dpch_pwr_ctl_info_struct_type * ul_dpch_pwr_ctl_info_ptr = &config_ptr->l1_ul_chan_parms.hs_ul_dpch_cfg.dpch_pwr_ctrl_info;

ul_dpch_pwr_ctl_info_ptr->valid = TRUE;
  
  if(T_rrc_PowerControlAlgorithm_algorithm1==ul_dpch_pwrctrl_info_ptr->powerControlAlgorithm.t)
  {
        ul_dpch_pwr_ctl_info_ptr->pca =
        L1_UL_PCA_ONE;
      if(ul_dpch_pwrctrl_info_ptr->powerControlAlgorithm.u.algorithm1 == 0)
      {
        ul_dpch_pwr_ctl_info_ptr->tpc_step_size =
          L1_UL_TPC_STEP_ONE;
      }
      else if(ul_dpch_pwrctrl_info_ptr->powerControlAlgorithm.u.algorithm1 == 1)
      {
        ul_dpch_pwr_ctl_info_ptr->tpc_step_size =
          L1_UL_TPC_STEP_TWO;
      }
      else
      {
        WRRC_MSG1_ERROR("RRCHSRACH TPC Step size %d is out of range",
          ul_dpch_pwrctrl_info_ptr->powerControlAlgorithm.u.algorithm1);
        return FAILURE;
      } /* end if-else check for Algorithm 1 and TPC step size */
  }
  else if (T_rrc_PowerControlAlgorithm_algorithm2 == ul_dpch_pwrctrl_info_ptr->powerControlAlgorithm.t)
  {
    ul_dpch_pwr_ctl_info_ptr->pca =
      L1_UL_PCA_TWO;
  }
  else
  {
    WRRC_MSG0_ERROR("RRCHSRACH Invalid UL Power Control Algorithm specified");
    return FAILURE;
  } /* end if-else check for UL power control algorithm check */

  /* Spec requirement
    8.5.45 Enhanced Uplink in CELL_FACH state and Idle mode
    1> if IE "ACK/NACK support on HS-DPCCH" is set to TRUE:
    2> if any of the IE "ACK", "NACK" or "Ack-Nack repetition factor" is not included in the IE "Uplink DPCH
    power control info":
    3> the UE behaviour is not specified.
    2> else:
    3> configure the HS-DPCCH in accordance with

    8.5.46 Actions related to COMMON_E_DCH_TRANSMISSION variable
    1> if transmitting DCCH or DTCH data:
    2> if IE "ACK/NACK support on HS-DPCCH" is set to TRUE:
    3> after collision resolution provide ACK/NACK feedback in accordance with [29] in the physical layer on
    the serving HS-DSCH radio link, using the information in the IE "Uplink DPCH power control info";
    3> after collision resolution use the information for the channel quality indication (CQI) procedure in the
    physical layer on the serving HS-DSCH radio link if the IE "Measurement Feedback Info" has been
    provided with the common E-DCH resource.

    Always set both the bitmasks but in initialization RRC will configure valid values 
    so that L1 validation doesnt fail. The valid values are initialized such that no CQI reporting will happen
    unless meas feedback info IE is provided*/
    hs_dpcch_info_ptr->info_present_bitmask = HS_DPCCH_BETA_HS_INFO_PRESENT_BITMASK|HS_DPCCH_MEAS_FEEDBACK_INFO_PRESENT_BITMASK;
   
if(ul_dpch_pwrctrl_info_ptr->m.deltaACKPresent)
{
  hs_dpcch_info_ptr->beta_hs_info.delta_ack = ul_dpch_pwrctrl_info_ptr->deltaACK;
}

if(ul_dpch_pwrctrl_info_ptr->m.deltaNACKPresent)
{
  hs_dpcch_info_ptr->beta_hs_info.delta_nack = ul_dpch_pwrctrl_info_ptr->deltaNACK;
}

if(ul_dpch_pwrctrl_info_ptr->m.ack_NACK_repetition_factorPresent)
{
  hs_dpcch_info_ptr->meas_feedback_info.ack_nack_rep_factor = ul_dpch_pwrctrl_info_ptr->ack_NACK_repetition_factor;
}

/*Slot format fixed for DPCCH confirm*/
config_ptr->l1_ul_chan_parms.hs_ul_dpch_cfg.dpcch_slot_format = L1_UL_DPCCH_SLOT_FORMAT_1;

  return(SUCCESS);
}

/*============================================================================
FUNCTION: rrchsrach_process_fdpch_info()

DESCRIPTION:
  This function updates IE "rrc_CommonEDCHSystemInfoFDD" in HSRACH ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
static uecomdef_status_e_type rrchsrach_process_fdpch_info
(
  ordered_config_type *config_ptr,
  rrc_CommonEDCHSystemInfoFDD *common_edch_sys_info_ptr
)
{
	
  config_ptr->l1_dl_chan_parms.hs_rach_dl_fdpch_info.tpc_cmd_err_rate_target = 
      common_edch_sys_info_ptr->dl_FDPCH_TPCcommandErrorRate;
	
  config_ptr->l1_dl_chan_parms.hs_rach_dl_fdpch_info.dpc_info = L1_DPC_MODE_SINGLE_TPC;

  /*FDPCH slot format 0 to be used for HS-RACH*/
  config_ptr->l1_dl_chan_parms.hs_rach_dl_fdpch_info.fdpch_slot_format =0;

  return(SUCCESS);
}

/*============================================================================
FUNCTION: rrchsrach_update_eul_info()

DESCRIPTION:
  This function updates IE "dl per rl info" in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
static uecomdef_status_e_type rrchsrach_update_common_edch_resource_list
(
  ordered_config_type *config_ptr,
  rrc_CommonEDCHSystemInfoFDD_common_E_DCH_ResourceInfoList *common_e_dch_resourceinfolist
)
{
  
  rrc_CommonEDCHSystemInfoFDD_common_E_DCH_ResourceInfoList *temp_common_e_dch_resourceinfolist;
  uint8 n_value = 0;
  uint8 n_index = 0;

    /*  Last received Scrambling Code  */
  uint32  last_received_scrambling_code = 0xFFFFFFFF; 
  uint8 last_s_offset = 0;
  uint8 last_f_dpch_chan_code_num = 0xFF;
  uint8  last_hich_ch_code = 0xFF;
  uint8  last_hich_signature_idx = 0;
  uint8  last_ergch_signature_idx = 0;
  
  temp_common_e_dch_resourceinfolist = common_e_dch_resourceinfolist;

  config_ptr->common_e_dch_resource_list.num_e_dch_common_info = 0;

  n_value = common_e_dch_resourceinfolist->n;
  
  while (n_value != 0)
  {
    if (L1_MAX_E_DCH_COMMON_LIST <= config_ptr->common_e_dch_resource_list.num_e_dch_common_info)
    {
      return(FAILURE);
    }

    if(temp_common_e_dch_resourceinfolist->elem[n_index].m.s_offsetPresent)
    {
      last_s_offset = config_ptr->common_e_dch_resource_list.e_dch_common_info[config_ptr->common_e_dch_resource_list.num_e_dch_common_info].s_offset = 
        temp_common_e_dch_resourceinfolist->elem[n_index].s_offset;
    }
    else
    {
          /*	10.3.6.9a Common E-DCH system info
      NOTE3: The default value is defined by:
      Soffset = (offset+common E-DCH resource list position) mod 10
      where "offset" is the value of the last instance of the IE "Soffset". If the IE "Soffset" was not present in the
      first occurrence, "offset" is equal to zero.
      "common E-DCH resource list position" indicates the Common E-DCH resource Configuration Information
      number by order of appearance. The value of the "common E-DCH resource list position" is zero for the
      first occurrence.
      */
      if(0xFF == last_s_offset)
      {
        WRRC_MSG0_ERROR("RRCHSRACH No valid value of soffset in any index ");
        return(FAILURE);
      }
      else
      {
        config_ptr->common_e_dch_resource_list.e_dch_common_info[config_ptr->common_e_dch_resource_list.num_e_dch_common_info].s_offset =
        	(last_s_offset + config_ptr->common_e_dch_resource_list.num_e_dch_common_info)%10;
      }
    }

    if(temp_common_e_dch_resourceinfolist->elem[n_index].m.f_dpch_ChannelisationCodeNumberPresent)
    {
      last_f_dpch_chan_code_num = config_ptr->common_e_dch_resource_list.e_dch_common_info[config_ptr->common_e_dch_resource_list.num_e_dch_common_info].f_dpch_chan_code_num = 
        temp_common_e_dch_resourceinfolist->elem[n_index].f_dpch_ChannelisationCodeNumber;
    }
    else
    {
      /*10.3.6.9a Common E-DCH system info 
      If this IE is not present, the value is equal to the last occurrence of this IE*/
      if(0xFF == last_f_dpch_chan_code_num)
      {
        WRRC_MSG0_ERROR("RRCHSRACH No valid value of last_f_dpch_chan_code_num in any index ");
        return(FAILURE);
      }
      else
      {
        config_ptr->common_e_dch_resource_list.e_dch_common_info[config_ptr->common_e_dch_resource_list.num_e_dch_common_info].f_dpch_chan_code_num=
          last_f_dpch_chan_code_num ;
      }
    }	

    if(temp_common_e_dch_resourceinfolist->elem[n_index].m.e_RGCH_InformationPresent)
    {
      config_ptr->common_e_dch_resource_list.e_dch_common_info[config_ptr->common_e_dch_resource_list.num_e_dch_common_info].e_rgch_info_present = TRUE;
      if(temp_common_e_dch_resourceinfolist->elem[n_index].e_RGCH_Information.m.signatureSequencePresent)
      {
        last_ergch_signature_idx = 
        config_ptr->common_e_dch_resource_list.e_dch_common_info[config_ptr->common_e_dch_resource_list.num_e_dch_common_info].e_rgch_info.signature_idx=
          temp_common_e_dch_resourceinfolist->elem[n_index].e_RGCH_Information.signatureSequence;
      }
      else
      {
      
        /* 10.3.6.115 E-RGCH Info for Common E-DCH
        NOTE1: The default value is defined by:
        Signature Sequence = (offset + common E-DCH resource list position) mod 40
        where "offset" is the value of the last occurrence of the IE "Signature Sequence". If "Signature
        Sequence" is not present in the first occurrence, "offset" is equal to zero.
        "common E-DCH resource list position" indicates the Common E-DCH resource Configuration
        Information number by order of appearance. The value of the "common E-DCH resource list
        position" is zero for the first occurrence.
        */

          config_ptr->common_e_dch_resource_list.e_dch_common_info[config_ptr->common_e_dch_resource_list.num_e_dch_common_info].e_rgch_info.signature_idx=
          (last_ergch_signature_idx+config_ptr->common_e_dch_resource_list.num_e_dch_common_info)%40;

      }
    }
	

  if(temp_common_e_dch_resourceinfolist->elem[n_index].e_hich_Info.m.channelisationCodePresent)
  {
    last_hich_ch_code = 
    config_ptr->common_e_dch_resource_list.e_dch_common_info[config_ptr->common_e_dch_resource_list.num_e_dch_common_info].e_hich_info.ch_code=
      temp_common_e_dch_resourceinfolist->elem[n_index].e_hich_Info.channelisationCode;
  }
  else
  {
    if(0xFF == last_hich_ch_code)
    {
      WRRC_MSG0_ERROR("RRCHSRACH No valid value of e_hich_info channelisation code in any index ");
      return(FAILURE);
    }
    else
    {
      config_ptr->common_e_dch_resource_list.e_dch_common_info[config_ptr->common_e_dch_resource_list.num_e_dch_common_info].e_hich_info.ch_code=
        last_hich_ch_code;
    }
  }
  
  if(temp_common_e_dch_resourceinfolist->elem[n_index].e_hich_Info.m.signatureSequencePresent)
  {
    last_hich_signature_idx = 
    config_ptr->common_e_dch_resource_list.e_dch_common_info[config_ptr->common_e_dch_resource_list.num_e_dch_common_info].e_hich_info.signature_idx =
      temp_common_e_dch_resourceinfolist->elem[n_index].e_hich_Info.signatureSequence;
  }
  else
  {
  
    /* 10.3.6.116 E-HICH Info for Common E-DCH
      NOTE1: The default value is defined by:
    Signature Sequence = (offset + common E-DCH resource list position) mod 40
    where "offset" is the value of the last occurrence of the IE "Signature Sequence". If "Signature
    Sequence" is not present in the first occurrence, "offset" is equal to zero.
    "common E-DCH resource list position" indicates the Common E-DCH resource Configuration
    Information number by order of appearance. The value of the "common E-DCH resource list
    position" is zero for the first occurrence.
    */
      config_ptr->common_e_dch_resource_list.e_dch_common_info[config_ptr->common_e_dch_resource_list.num_e_dch_common_info].e_hich_info.signature_idx=
      (last_hich_signature_idx+config_ptr->common_e_dch_resource_list.num_e_dch_common_info)%40;

  }

  config_ptr->common_e_dch_resource_list.e_dch_common_info[config_ptr->common_e_dch_resource_list.num_e_dch_common_info].l1_ul_dpch_code_info.scrambling_code_type=
  	temp_common_e_dch_resourceinfolist->elem[n_index].ul_DPCH_CodeInfoForCommonEDCH.ul_DPCCHscramblingCodeType;


/* 10.3.6.87b
NOTE1: If this IE is not included, the value is given by:
Scrambling code number = (offset + common E-DCH resource list position) mod 16777216
where
"offset" is the value of the last occurrence of the IE "scrambling code number".
"common E-DCH resource list position" indicates the Common E-DCH resource Configuration
Information number by order of appearance. The value of the "common E-DCH resource list
position" is zero for the first occurrence.
*/

if(temp_common_e_dch_resourceinfolist->elem[n_index].ul_DPCH_CodeInfoForCommonEDCH.m.ul_DPCCHscramblingCodePresent)
{
  last_received_scrambling_code = 
  config_ptr->common_e_dch_resource_list.e_dch_common_info[config_ptr->common_e_dch_resource_list.num_e_dch_common_info].l1_ul_dpch_code_info.scrambling_code=
  temp_common_e_dch_resourceinfolist->elem[n_index].ul_DPCH_CodeInfoForCommonEDCH.ul_DPCCHscramblingCode;
}
else
{
  if(0xFFFFFFFF == last_received_scrambling_code)
  {
     WRRC_MSG0_ERROR("RRCHSRACH No valid value of scrambling code number in any index ");
    return(FAILURE);
  }
  else
  {
    config_ptr->common_e_dch_resource_list.e_dch_common_info[config_ptr->common_e_dch_resource_list.num_e_dch_common_info].l1_ul_dpch_code_info.scrambling_code = 
      (last_received_scrambling_code+config_ptr->common_e_dch_resource_list.num_e_dch_common_info)%16777216;
  }
}

  config_ptr->common_e_dch_resource_list.num_e_dch_common_info++;
    n_value--;
    n_index++;
  }

  
  return(SUCCESS);
}

/*============================================================================
FUNCTION: rrchsrach_convert_asn1_edch_transmit_cont_offset()

DESCRIPTION:
  This function converts the ENUM type received in reconfig message to actual ms
  delay in happyBitDelayCondition.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  happy bit delay condition in ms

SIDE EFFECTS:
=============================================================================*/
static l1_e_dch_tx_cont_backoff_enum_type rrchsrach_convert_asn1_edch_transmit_cont_offset
(
    rrc_CommonEDCHSystemInfoFDD_e_dch_TransmitContinuationOffset edch_tx_cont_offset
)
{

  l1_e_dch_tx_cont_backoff_enum_type edch_transmit_cont_offset = MAC_HS_RACH_TX_BACKOFF_INFINITY;

  switch(edch_tx_cont_offset)
  {
    case rrc_CommonEDCHSystemInfoFDD_e_dch_TransmitContinuationOffset_tti0:
      edch_transmit_cont_offset = MAC_HS_RACH_TX_BACKOFF_0;
      break;

    case rrc_CommonEDCHSystemInfoFDD_e_dch_TransmitContinuationOffset_tti4:
      edch_transmit_cont_offset = MAC_HS_RACH_TX_BACKOFF_4;
      break;

    case rrc_CommonEDCHSystemInfoFDD_e_dch_TransmitContinuationOffset_tti8:
      edch_transmit_cont_offset = MAC_HS_RACH_TX_BACKOFF_8;
      break;

    case rrc_CommonEDCHSystemInfoFDD_e_dch_TransmitContinuationOffset_tti16:
      edch_transmit_cont_offset = MAC_HS_RACH_TX_BACKOFF_16;
      break;

    case rrc_CommonEDCHSystemInfoFDD_e_dch_TransmitContinuationOffset_tti24:
      edch_transmit_cont_offset = MAC_HS_RACH_TX_BACKOFF_24;
      break;

    case rrc_CommonEDCHSystemInfoFDD_e_dch_TransmitContinuationOffset_tti40:
      edch_transmit_cont_offset = MAC_HS_RACH_TX_BACKOFF_40;
      break;

    case rrc_CommonEDCHSystemInfoFDD_e_dch_TransmitContinuationOffset_tti80:
      edch_transmit_cont_offset = MAC_HS_RACH_TX_BACKOFF_80;
      break;

    case rrc_CommonEDCHSystemInfoFDD_e_dch_TransmitContinuationOffset_infinity:
      edch_transmit_cont_offset = MAC_HS_RACH_TX_BACKOFF_INFINITY;
      break;

    default:
      WRRC_MSG1_ERROR("RRCHSRACH UnKnown Happy bit delay condition: %d", edch_transmit_cont_offset);
      break;
  }

  return(edch_transmit_cont_offset);
}

/*====================================================================
FUNCTION      rrchsrach_process_meas_feedback_info()

DESCRIPTION   This function sets the Meas Feedback Information in
              L1 structure in OC.

DEPENDENCIES  None

RETURN VALUE  SUCCESS if the entire operation is successful,
              FAILURE otherwise

SIDE EFFECTS  None.
====================================================================*/

static uecomdef_status_e_type rrchsrach_process_meas_feedback_info
(
  ordered_config_type *config_ptr,
  rrc_Measurement_Feedback_Info_r7* meas_feedback_ptr
)
{
  l1_ul_hs_dpcch_info_struct_type *hs_dpcch_info_ptr= &config_ptr->l1_hsdpa_info.hs_dpcch_info;
  
  if (RRC_CHECK_MSG_TYPE(meas_feedback_ptr->modeSpecificInfo,
                 T_rrc_Measurement_Feedback_Info_r7_modeSpecificInfo_fdd))
  {
    hs_dpcch_info_ptr->info_present_bitmask = HS_DPCCH_BETA_HS_INFO_PRESENT_BITMASK|HS_DPCCH_MEAS_FEEDBACK_INFO_PRESENT_BITMASK;

    if ((meas_feedback_ptr->modeSpecificInfo.u.fdd->cqi_RepetitionFactor < RRCLLC_CQI_MIN_REP_FACTOR) || 
        (meas_feedback_ptr->modeSpecificInfo.u.fdd->cqi_RepetitionFactor > RRCLLC_CQI_MAX_REP_FACTOR))
    {
      WRRC_MSG1_ERROR("RRCHSRACH:Invalid CQI Rfactor %d, 1..4",
        meas_feedback_ptr->modeSpecificInfo.u.fdd->cqi_RepetitionFactor);
      return FAILURE;
    }

    hs_dpcch_info_ptr->meas_feedback_info.cqi_rep_factor =
      (uint8) meas_feedback_ptr->modeSpecificInfo.u.fdd->cqi_RepetitionFactor;

    if (meas_feedback_ptr->modeSpecificInfo.u.fdd->deltaCQI > RRCLLC_DELTA_CQI_MAX)
    {
      WRRC_MSG1_ERROR("RRCHSRACH:Invalid DELTA CQI %d, 0..8",
        meas_feedback_ptr->modeSpecificInfo.u.fdd->deltaCQI);
      return FAILURE;
    }
	
    hs_dpcch_info_ptr->beta_hs_info.delta_cqi = (uint8)
      meas_feedback_ptr->modeSpecificInfo.u.fdd->deltaCQI;

    switch(meas_feedback_ptr->modeSpecificInfo.u.fdd->feedback_cycle)
    {
      case rrc_Feedback_cycle_r7_fc0:
        hs_dpcch_info_ptr->meas_feedback_info.cqi_feedback_cycle = 0;
        break;
  
      case rrc_Feedback_cycle_r7_fc2:
        hs_dpcch_info_ptr->meas_feedback_info.cqi_feedback_cycle = 2;
        break;
  
      case rrc_Feedback_cycle_r7_fc4:
        hs_dpcch_info_ptr->meas_feedback_info.cqi_feedback_cycle = 4;
  
        break;
  
      case rrc_Feedback_cycle_r7_fc8:
        hs_dpcch_info_ptr->meas_feedback_info.cqi_feedback_cycle = 8;
        break;
  
      case rrc_Feedback_cycle_r7_fc10:
        hs_dpcch_info_ptr->meas_feedback_info.cqi_feedback_cycle = 10;
        break;
  
      case rrc_Feedback_cycle_r7_fc20:
        hs_dpcch_info_ptr->meas_feedback_info.cqi_feedback_cycle = 20;
        break;
  
      case rrc_Feedback_cycle_r7_fc40:
        hs_dpcch_info_ptr->meas_feedback_info.cqi_feedback_cycle = 40;
        break;
  
      case rrc_Feedback_cycle_r7_fc80:
        hs_dpcch_info_ptr->meas_feedback_info.cqi_feedback_cycle = 80;
        break;
  
      case rrc_Feedback_cycle_r7_fc160:
        hs_dpcch_info_ptr->meas_feedback_info.cqi_feedback_cycle = 160;
        break;
  
      case rrc_Feedback_cycle_r7_fc16:
        hs_dpcch_info_ptr->meas_feedback_info.cqi_feedback_cycle = 16;
        break;
  
      case rrc_Feedback_cycle_r7_fc32:
        hs_dpcch_info_ptr->meas_feedback_info.cqi_feedback_cycle = 32;
        break;
  
      case rrc_Feedback_cycle_r7_fc64:
        hs_dpcch_info_ptr->meas_feedback_info.cqi_feedback_cycle = 64;
        break;
  
      default:
        WRRC_MSG1_ERROR("RRCHSRACH:Incorrect Value",
          meas_feedback_ptr->modeSpecificInfo.u.fdd->feedback_cycle);
        return FAILURE;
    }


    if ((meas_feedback_ptr->modeSpecificInfo.u.fdd->measurementPowerOffset < RRCLLC_MIN_MEAS_POWER_OFFSET) ||
        (meas_feedback_ptr->modeSpecificInfo.u.fdd->measurementPowerOffset > RRCLLC_MAX_MEAS_POWER_OFFSET))
    {
      WRRC_MSG1_ERROR("RRCHSRACH:Invalid MeasPowerOffset %d, -12..26",
        meas_feedback_ptr->modeSpecificInfo.u.fdd->measurementPowerOffset);
      return FAILURE;
    }

    hs_dpcch_info_ptr->meas_feedback_info.po_hs_dsch = (int8)
      meas_feedback_ptr->modeSpecificInfo.u.fdd->measurementPowerOffset;

    /* rrc_UL_DPCH_PowerControlInfo_r5 : ACK/NACK/REPITITON factor from Power Control */
  }
  else
  {
    WRRC_MSG0_ERROR("RRCHSRACH:Invalid Meas Feedback Info");
    return FAILURE;
  }
  return SUCCESS;

}


/*============================================================================
FUNCTION: rrchsrach_process_ul_interference_commonedch()

DESCRIPTION:
  This function updates ul interference from SIB 5 Edch common sys info or SIB 7 
      
DEPENDENCIES:
  None.

RETURN VALUE:
SUCCESS if everything is valid, FAILURE otherwise

SIDE EFFECTS:
=============================================================================*/
static uecomdef_status_e_type rrchsrach_process_ul_interference_commonedch
(
  ordered_config_type *config_ptr,
  rrc_CommonEDCHSystemInfoFDD *common_edch_sys_info_ptr
)
{

  if(common_edch_sys_info_ptr->m.ul_InterferenceForCommonEDCHPresent)
  {
    config_ptr->rrc_ul_pwr_init_parms.ul_interference_commonedch = common_edch_sys_info_ptr->ul_InterferenceForCommonEDCH;
  }
  else
  {
    config_ptr->rrc_ul_pwr_init_parms.ul_interference_commonedch  = config_ptr->rrc_ul_pwr_init_parms.ul_interference;
  }	
  
  return(SUCCESS);
}

/*============================================================================
FUNCTION: rrchsrach_process_edch_tti()

DESCRIPTION:
  This function updates IE EDCH TTI  in HSRACH ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
SUCCESS if everything is valid, FAILURE otherwise

SIDE EFFECTS:
=============================================================================*/
static uecomdef_status_e_type rrchsrach_process_edch_tti
(
  ordered_config_type *config_ptr,
  rrc_CommonEDCHSystemInfoFDD *common_edch_sys_info_ptr
)
{

  config_ptr->mac_hs_rach_config.hs_rach_config.e_tti = common_edch_sys_info_ptr->e_dch_TTI;
  	
  config_ptr->l1_hs_rach_e_info.e_ul_info.e_tti = common_edch_sys_info_ptr->e_dch_TTI;

  config_ptr->l1_hs_rach_e_info.e_dl_info.e_tti = common_edch_sys_info_ptr->e_dch_TTI;
	
  return(SUCCESS);
}
/*============================================================================
FUNCTION: rrchsrach_process_initial_serving_grant()

DESCRIPTION:
  This function updates IE "initialServingGrantValue" in HSRACH ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
SUCCESS if everything is valid, FAILURE otherwise

SIDE EFFECTS:
=============================================================================*/
static uecomdef_status_e_type rrchsrach_process_initial_serving_grant
(
  ordered_config_type *config_ptr,
  rrc_CommonEDCHSystemInfoFDD *common_edch_sys_info_ptr
)
{
  config_ptr->l1_hs_rach_e_info.e_dl_info.grant_config.serving_grant = (uint8)common_edch_sys_info_ptr->initialServingGrantValue;
	
  return(SUCCESS);
}

/*============================================================================
FUNCTION: rrchsrach_process_eagch_info()

DESCRIPTION:
  This function updates IE "e_agch_Information" in HSRACH ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
SUCCESS if everything is valid, FAILURE otherwise

SIDE EFFECTS:
=============================================================================*/
static uecomdef_status_e_type rrchsrach_process_eagch_info
(
  ordered_config_type *config_ptr,
  rrc_CommonEDCHSystemInfoFDD *common_edch_sys_info_ptr
)
{
  config_ptr->l1_hs_rach_e_info.e_dl_info.e_agch_info.ch_code = (uint8)common_edch_sys_info_ptr->e_agch_Information.e_AGCH_ChannelisationCode;
	
  return(SUCCESS);
}
/*============================================================================
FUNCTION: rrchsrach_process_harq_info()

DESCRIPTION:
  This function updates IE "harq_Info" in HSRACH ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
static uecomdef_status_e_type rrchsrach_process_harq_info
(
  ordered_config_type *config_ptr,
  rrc_CommonEDCHSystemInfoFDD *common_edch_sys_info_ptr
)
{
	
  if(rrc_CommonEDCHSystemInfoFDD_harq_Info_rv0 == common_edch_sys_info_ptr->harq_Info)
  {
    config_ptr->l1_hs_rach_e_info.e_ul_info.harq_rv_config = L1_RV_0;
  }
  else if (rrc_CommonEDCHSystemInfoFDD_harq_Info_rvtable == common_edch_sys_info_ptr->harq_Info)
  {
    config_ptr->l1_hs_rach_e_info.e_ul_info.harq_rv_config = L1_RV_TABLE;
  }
	
  return(SUCCESS);
}
/*====================================================================
FUNCTION      rrchsrach_copy_sib5_to_config_db()

DESCRIPTION 

DEPENDENCIES  Before calling this function, you should have done the check whether the cell supports HSRACH

RETURN VALUE  
  SUCCESS if the entire operation is successful
  FAILURE otherwise

SIDE EFFECTS  None.
====================================================================*/
static uecomdef_status_e_type rrchsrach_copy_sib5_to_config_db
(
  ordered_config_type *config_ptr,
  rrc_state_e_type next_rrc_state
)
{
  rrc_CommonEDCHSystemInfoFDD *common_edch_sys_info_ptr = NULL;

  rrc_SysInfoType5    *sib5_ptr = NULL;

  sib5_ptr = (rrc_SysInfoType5 *) rrc_sibdb_return_sib_for_srv_cell(rrc_SIB5);
  
  if (sib5_ptr == NULL)
  {
     WRRC_MSG0_ERROR("RRCHSRACH: SIB 5 not present in SIB DB for serving cell ");
     return FAILURE;
  }

  common_edch_sys_info_ptr = &sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.\
      v690NonCriticalExtensions.v770NonCriticalExtensions.v860NonCriticalExtensions.v890NonCriticalExtensions.v8b0NonCriticalExtensions.\
      v8d0NonCriticalExtensions.sysInfoType5_v8d0ext.commonEDCHSystemInfoFDD;

    /* Process the UL interference for common edch IE */
    if( FAILURE == rrchsrach_process_ul_interference_commonedch(config_ptr,common_edch_sys_info_ptr))
    {
      WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting PRACH preamble info from SIB5");
      return FAILURE;
    }
	
    /* Process the common edch flowlist IE */
    if( FAILURE == rrchsrach_process_common_edch_flowlist(config_ptr,
                          &common_edch_sys_info_ptr->common_E_DCH_MAC_d_FlowList))
    {
      WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting MAC D Flow info from SIB5");
      return FAILURE;
    }

    /* Process the prach preamble IE */
    if( FAILURE == rrchsrach_process_prach_preamble_enhanceduplink(config_ptr,
                          &common_edch_sys_info_ptr->prach_PreambleForEnhancedUplink,sib5_ptr))
    {
      WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting PRACH preamble info from SIB5");
      return FAILURE;
    }

    /* Process the initial SG IE */
    if( FAILURE == rrchsrach_process_initial_serving_grant(config_ptr,
                          common_edch_sys_info_ptr))
    {
      WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting PRACH preamble info from SIB5");
      return FAILURE;
    }
	
    if( FAILURE == rrchsrach_process_edch_tti(config_ptr,common_edch_sys_info_ptr))
    {
      WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting PRACH preamble info from SIB5");
      return FAILURE;
    }

    /* Process the AGCH info IE */
    if( FAILURE == rrchsrach_process_eagch_info(config_ptr,common_edch_sys_info_ptr))
    {
      WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting PRACH preamble info from SIB5");
      return FAILURE;
    }

    if( FAILURE == rrchsrach_process_harq_info(config_ptr,common_edch_sys_info_ptr))
    {
      WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting PRACH preamble info from SIB5");
      return FAILURE;
    }

    /* Process the ul_DPCHpowerControlInfoForCommonEDCH IE */
    if( FAILURE == rrchsrach_process_uldpch_powerctrl_info(config_ptr,&common_edch_sys_info_ptr->ul_DPCHpowerControlInfoForCommonEDCH))
    {
      WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting PRACH preamble info from SIB5");
      return FAILURE;
    }

    if( FAILURE == rrchsrach_process_e_dpcch_info(config_ptr,
                          &common_edch_sys_info_ptr->e_dpcch_Info))
    {
      WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting EDPCCH info from SIB5");
      return FAILURE;
    }

    /*updating the e dpdch  structure */
    if( FAILURE == rrchsrach_process_e_dpdch_info(config_ptr,
                          &common_edch_sys_info_ptr->e_dpdch_Info))
    {
      WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting EDPDCH info from SIB5");
      return FAILURE;
    }

    if( FAILURE == rrchsrach_process_fdpch_info(config_ptr,common_edch_sys_info_ptr))
    {
      WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting PRACH preamble info from SIB5");
      return FAILURE;
    }


  config_ptr->mac_hs_rach_config.add_e_dch_tx_backoff = common_edch_sys_info_ptr->additional_E_DCH_TransmitBackoff;
  
  config_ptr->mac_hs_rach_config.max_collision_resol = common_edch_sys_info_ptr->max_PeriodForCollisionResolution;
	
  config_ptr->mac_hs_rach_config.l1_e_dch_tx_cont_backoff = 
		rrchsrach_convert_asn1_edch_transmit_cont_offset(common_edch_sys_info_ptr->e_dch_TransmitContinuationOffset);

  config_ptr->mac_hs_rach_config.max_e_dch_res_alloc_ccch = 
		rrchsrach_convert_asn1_edch_max_ccch_resource_alloc(common_edch_sys_info_ptr->max_CCCH_ResourceAllocation);
  

  if(common_edch_sys_info_ptr->ack_nack_support_on_HS_DPCCH)
  {
    /* 8.5.45    Enhanced Uplink in CELL_FACH state and Idle mode (FDD only)
1>  if IE "ACK/NACK support on HS-DPCCH" is set to TRUE:
2>  if any of the IE "ACK", "NACK" or "Ack-Nack repetition factor" is not included in the IE "Uplink DPCH power control info":
3>  the UE behaviour is not specified.
*/
    
    if(!((common_edch_sys_info_ptr->ul_DPCHpowerControlInfoForCommonEDCH.m.deltaACKPresent)||
      (common_edch_sys_info_ptr->ul_DPCHpowerControlInfoForCommonEDCH.m.deltaNACKPresent) ||
      (common_edch_sys_info_ptr->ul_DPCHpowerControlInfoForCommonEDCH.m.ack_NACK_repetition_factorPresent)))
    {
      WRRC_MSG0_ERROR("RRCHSRACH: IEs ACK, NACK or Ack-Nack repetition factor not present in SIB5");
      return FAILURE;
    }
    else
    {
      config_ptr->ack_nack_support_on_hs_dpcch = TRUE;
//    config_ptr->l1_hsdpa_info.hs_req_mask|=  CPHY_HS_DPCCH_CFG_INCL;
    }
  }

  if(common_edch_sys_info_ptr->m.measurement_Feedback_InfoPresent)
  {
    if( FAILURE == rrchsrach_process_meas_feedback_info(config_ptr,&common_edch_sys_info_ptr->measurement_Feedback_Info))
    {
      WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting Meas FB info from SIB5");
      return FAILURE;
    }
    
  }

  if( FAILURE == rrchsrach_update_common_edch_resource_list(config_ptr,&common_edch_sys_info_ptr->common_E_DCH_ResourceInfoList))
  {
    WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting Common EDCH resource info from SIB5");
    return FAILURE;
  }

    /*Fill the SRB0 CCCH params in mac database */
    if( FAILURE == rrchsrach_update_rb_mapping_info_for_ul_ccch_srb0(config_ptr))
    {
      WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting MAC D Flow info from SIB5");
      return FAILURE;
    }

    if( FAILURE == rrchsrach_update_edl_info(&config_ptr->l1_hs_rach_e_info.e_dl_info,
                          common_edch_sys_info_ptr))
    {
      WRRC_MSG0_ERROR("RRCHSRACH: Failure in setting PRACH preamble info from SIB5");
      return FAILURE;
    }	
  
  return SUCCESS;
}

/*====================================================================
FUNCTION: rrchsrach_get_edch_rach_info_from_sib5()

DESCRIPTION:
  Update config pointer with HSRACH info from SIB5
  
DEPENDENCIES:


RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
  ====================================================================*/
uecomdef_status_e_type rrchsrach_get_edch_rach_info_from_sib5
(
  ordered_config_type *config_ptr,
  rrc_state_e_type next_rrc_state
)
{
/*Initialize the HS-RACH structures*/
  rrcllc_init_hsrach_info(config_ptr,next_rrc_state);

  /*The check whether the cell supports HSRACH or not must have been done by now*/
  if(rrchsrach_copy_sib5_to_config_db(config_ptr, next_rrc_state) == FAILURE)
  {
    WRRC_MSG0_ERROR("HSRACH:Copying SIB5 HS params to Config DB failed.");
    return FAILURE;
  }
  return SUCCESS;
}

void rrchsrach_print_cell_transition_req(l1_cell_transition_cmd_type *l1_cmd_ptr)
{
  
  WRRC_MSG2_HIGH("RRCHSRACH :CPHY_SETUP CphySetup 0x%x L1 HS_RACH_REQ_MASK:0x%x [1-START,2- RECFG,4-STOP] ", l1_cmd_ptr->setup_cmd.req_mask, l1_cmd_ptr->setup_cmd.hs_rach_req_mask);

  if((l1_cmd_ptr->setup_cmd.hs_rach_req_mask == L1_HS_RACH_NO_OP) || (l1_cmd_ptr->setup_cmd.hs_rach_req_mask == L1_HS_RACH_STOP))
    return;

  /*
  WRRC_MSG1_HIGH("RRCHSRACH Number of Common resources %d ",l1_cmd_ptr->setup_cmd.common_e_dch_resource_list->num_e_dch_common_info);

  for (count=0; count<l1_cmd_ptr->setup_cmd.common_e_dch_resource_list->num_e_dch_common_info; count++)
  {
    MSG_9(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCHSRACH :s_offset %d, f_dpch_chan_code_num %d, e_rgch_info_present %d, rg_idx %d signature_idx %d ch_code %d signature_idx %d scrambling_code_type %d scrambling_code %d",
    l1_cmd_ptr->setup_cmd.common_e_dch_resource_list->e_dch_common_info[count].s_offset,
    l1_cmd_ptr->setup_cmd.common_e_dch_resource_list->e_dch_common_info[count].f_dpch_chan_code_num,
    l1_cmd_ptr->setup_cmd.common_e_dch_resource_list->e_dch_common_info[count].e_rgch_info_present,
    l1_cmd_ptr->setup_cmd.common_e_dch_resource_list->e_dch_common_info[count].e_rgch_info.rg_idx,
    l1_cmd_ptr->setup_cmd.common_e_dch_resource_list->e_dch_common_info[count].e_rgch_info.signature_idx,
    l1_cmd_ptr->setup_cmd.common_e_dch_resource_list->e_dch_common_info[count].e_hich_info.ch_code,
    l1_cmd_ptr->setup_cmd.common_e_dch_resource_list->e_dch_common_info[count].e_hich_info.signature_idx,
    l1_cmd_ptr->setup_cmd.common_e_dch_resource_list->e_dch_common_info[count].l1_ul_dpch_code_info.scrambling_code_type,
    l1_cmd_ptr->setup_cmd.common_e_dch_resource_list->e_dch_common_info[count].l1_ul_dpch_code_info.scrambling_code);
  }
  */
  MSG_9(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCHSRACH :EDL Info p_ernti_prst %d, p_ernti 0x%d, tti %d, srvg_grant %d, 3_idx_step_thres %d 2_idx_step_thres %d grant_selec %d ch_code %d",
  l1_cmd_ptr->setup_cmd.hs_rach_e_info_ptr->e_dl_info.primary_e_rnti_present,
  l1_cmd_ptr->setup_cmd.hs_rach_e_info_ptr->e_dl_info.primary_e_rnti,
  l1_cmd_ptr->setup_cmd.hs_rach_e_info_ptr->e_dl_info.e_tti,
  l1_cmd_ptr->setup_cmd.hs_rach_e_info_ptr->e_dl_info.grant_config.serving_grant,
  l1_cmd_ptr->setup_cmd.hs_rach_e_info_ptr->e_dl_info.grant_config.three_index_step_threshold,
  l1_cmd_ptr->setup_cmd.hs_rach_e_info_ptr->e_dl_info.grant_config.two_index_step_threshold,
  l1_cmd_ptr->setup_cmd.hs_rach_e_info_ptr->e_dl_info.grant_config.grant_selector,
  l1_cmd_ptr->setup_cmd.hs_rach_e_info_ptr->e_dl_info.e_agch_info.ch_code,
  l1_cmd_ptr->setup_cmd.common_e_dch_resource_list->num_e_dch_common_info
  );
/*
  MSG_6(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCHSRACH :EUL Info harq_rv_config %d, DPDCH e_tfci_table_idx %d, num_ref_e_tfci %d, min_beta_ed %d, e_max_ch_codes %d e_dpdch_pl_non_max %d ",
  l1_cmd_ptr->setup_cmd.hs_rach_e_info_ptr->e_ul_info.harq_rv_config,
  l1_cmd_ptr->setup_cmd.hs_rach_e_info_ptr->e_ul_info.e_dpdch_info.e_tfci_table_idx,
  l1_cmd_ptr->setup_cmd.hs_rach_e_info_ptr->e_ul_info.e_dpdch_info.num_ref_e_tfci,
  l1_cmd_ptr->setup_cmd.hs_rach_e_info_ptr->e_ul_info.e_dpdch_info.min_beta_ed,
  l1_cmd_ptr->setup_cmd.hs_rach_e_info_ptr->e_ul_info.e_dpdch_info.e_max_ch_codes,
  l1_cmd_ptr->setup_cmd.hs_rach_e_info_ptr->e_ul_info.e_dpdch_info.e_dpdch_pl_non_max
  );

  WRRC_MSG2_HIGH("RRCHSRACH :EUL EDPCCH info e_dpcch_pwr_offset %d, e_dpdch_pwr_interpolation %d",
    l1_cmd_ptr->setup_cmd.hs_rach_e_info_ptr->e_ul_info.e_dpcch_info.e_dpcch_pwr_offset,
    l1_cmd_ptr->setup_cmd.hs_rach_e_info_ptr->e_ul_info.e_dpcch_info.e_dpdch_pwr_interpolation);
 
*/
}


void rrchsrach_print_cphy_setup(l1_setup_cmd_type *l1_cmd_ptr)
{

  MSG_HIGH("RRCHSRACH CPHY_SETUP CphySetup 0x%x L1 HS_RACH_REQ_MASK:0x%x [1-START,2- RECFG,4-STOP], transition_to_dch:%d", 
                   l1_cmd_ptr->req_mask,
                   l1_cmd_ptr->hs_rach_req_mask,
                   l1_cmd_ptr->transition_to_dch);

    if((l1_cmd_ptr->hs_rach_req_mask == L1_HS_RACH_NO_OP) || (l1_cmd_ptr->hs_rach_req_mask == L1_HS_RACH_STOP))
    return;

/*
    MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCHSRACH  hsdpa_l1_ops 0x%d hsdpa_l1_info hs_req_mask %d transition_to_dch:%d Number of Common resources  %d",
    l1_cmd_ptr->hsdpa_l1_ops, l1_cmd_ptr->hsdpa_l1_info->hs_req_mask, 
    l1_cmd_ptr->transition_to_dch, l1_cmd_ptr->common_e_dch_resource_list->num_e_dch_common_info);

  for (count=0; count<l1_cmd_ptr->common_e_dch_resource_list->num_e_dch_common_info; count++)
  { 
	MSG_9(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCHSRACH :s_offset %d, f_dpch_chan_code_num %d, e_rgch_info_present %d, rg_idx %d signature_idx %d ch_code %d signature_idx %d scrambling_code_type %d scrambling_code %d",
	l1_cmd_ptr->common_e_dch_resource_list->e_dch_common_info[count].s_offset,
	l1_cmd_ptr->common_e_dch_resource_list->e_dch_common_info[count].f_dpch_chan_code_num,
	l1_cmd_ptr->common_e_dch_resource_list->e_dch_common_info[count].e_rgch_info_present,
	l1_cmd_ptr->common_e_dch_resource_list->e_dch_common_info[count].e_rgch_info.rg_idx,
	l1_cmd_ptr->common_e_dch_resource_list->e_dch_common_info[count].e_rgch_info.signature_idx,
	l1_cmd_ptr->common_e_dch_resource_list->e_dch_common_info[count].e_hich_info.ch_code,
	l1_cmd_ptr->common_e_dch_resource_list->e_dch_common_info[count].e_hich_info.signature_idx,
	l1_cmd_ptr->common_e_dch_resource_list->e_dch_common_info[count].l1_ul_dpch_code_info.scrambling_code_type,
	l1_cmd_ptr->common_e_dch_resource_list->e_dch_common_info[count].l1_ul_dpch_code_info.scrambling_code);
  }
  */
	MSG_9(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCHSRACH :EDL Info p_ernti_prst %d, p_ernti 0x%d, tti %d, srvg_grant %d, 3_idx_step_thres %d 2_idx_step_thres %d grant_selec %d ch_code %d, #Common resources  %d",
	l1_cmd_ptr->hs_rach_e_info_ptr->e_dl_info.primary_e_rnti_present,
	l1_cmd_ptr->hs_rach_e_info_ptr->e_dl_info.primary_e_rnti,
	l1_cmd_ptr->hs_rach_e_info_ptr->e_dl_info.e_tti,
	l1_cmd_ptr->hs_rach_e_info_ptr->e_dl_info.grant_config.serving_grant,
	l1_cmd_ptr->hs_rach_e_info_ptr->e_dl_info.grant_config.three_index_step_threshold,
	l1_cmd_ptr->hs_rach_e_info_ptr->e_dl_info.grant_config.two_index_step_threshold,
	l1_cmd_ptr->hs_rach_e_info_ptr->e_dl_info.grant_config.grant_selector,
	l1_cmd_ptr->hs_rach_e_info_ptr->e_dl_info.e_agch_info.ch_code,
	l1_cmd_ptr->common_e_dch_resource_list->num_e_dch_common_info
	);
/*
	MSG_6(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCHSRACH :EUL Info harq_rv_config %d, DPDCH e_tfci_table_idx %d, num_ref_e_tfci %d, min_beta_ed %d, e_max_ch_codes %d e_dpdch_pl_non_max %d ",
	l1_cmd_ptr->hs_rach_e_info_ptr->e_ul_info.harq_rv_config,
	l1_cmd_ptr->hs_rach_e_info_ptr->e_ul_info.e_dpdch_info.e_tfci_table_idx,
	l1_cmd_ptr->hs_rach_e_info_ptr->e_ul_info.e_dpdch_info.num_ref_e_tfci,
	l1_cmd_ptr->hs_rach_e_info_ptr->e_ul_info.e_dpdch_info.min_beta_ed,
	l1_cmd_ptr->hs_rach_e_info_ptr->e_ul_info.e_dpdch_info.e_max_ch_codes,
	l1_cmd_ptr->hs_rach_e_info_ptr->e_ul_info.e_dpdch_info.e_dpdch_pl_non_max
	);

   WRRC_MSG2_HIGH("RRCHSRACH :EUL EDPCCH info e_dpcch_pwr_offset %d, e_dpdch_pwr_interpolation %d",
     l1_cmd_ptr->hs_rach_e_info_ptr->e_ul_info.e_dpcch_info.e_dpcch_pwr_offset,
     l1_cmd_ptr->hs_rach_e_info_ptr->e_ul_info.e_dpcch_info.e_dpdch_pwr_interpolation);
	
#ifdef FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST
	WRRC_MSG2_HIGH("RRCHSRACH :EUL EDPCCH info e_tfci_boost_val %d, delta_t2tp %d ",
    	l1_cmd_ptr->hs_rach_e_info_ptr->e_ul_info.e_dpcch_info.e_tfc_boost_info.e_tfci_boost_val,
    	l1_cmd_ptr->hs_rach_e_info_ptr->e_ul_info.e_dpcch_info.e_tfc_boost_info.delta_t2tp);
	
#endif

  if((l1_cmd_ptr->dl_phychan_db != NULL) && (l1_cmd_ptr->ul_phychan_db != NULL))
   {
   MSG_HIGH("RRCHSRACH :FDPCH info for start/reconfig dpc_info %d, tpc_cmd_err_rate_target %d, fdpch_slot_format %d ",
   l1_cmd_ptr->dl_phychan_db->hs_rach_dl_fdpch_info.dpc_info,
   l1_cmd_ptr->dl_phychan_db->hs_rach_dl_fdpch_info.tpc_cmd_err_rate_target,
   l1_cmd_ptr->dl_phychan_db->hs_rach_dl_fdpch_info.fdpch_slot_format
   );
   
   MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCHSRACH :AICH info for start ovsf %d, tt %d, tx_div_ind %d, pwr_offset %d ",
   l1_cmd_ptr->dl_phychan_db->hs_rach_aich.ovsf,
   l1_cmd_ptr->dl_phychan_db->hs_rach_aich.tt,
   l1_cmd_ptr->dl_phychan_db->hs_rach_aich.tx_div_ind,
   l1_cmd_ptr->dl_phychan_db->hs_rach_aich.pwr_offset
   );
   
   MSG_7(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCHSRACH :UL DPCCH slot format %d, power ctrl info valid %d, dpcch_pwr_offset %d, pcp_len %d, srb_delay %d, pca %d, tpc_step_size %d",
   l1_cmd_ptr->ul_phychan_db->hs_ul_dpch_cfg.dpcch_slot_format,
   l1_cmd_ptr->ul_phychan_db->hs_ul_dpch_cfg.dpch_pwr_ctrl_info.valid,
   l1_cmd_ptr->ul_phychan_db->hs_ul_dpch_cfg.dpch_pwr_ctrl_info.dpcch_pwr_offset,
   l1_cmd_ptr->ul_phychan_db->hs_ul_dpch_cfg.dpch_pwr_ctrl_info.pcp_len,
   l1_cmd_ptr->ul_phychan_db->hs_ul_dpch_cfg.dpch_pwr_ctrl_info.srb_delay,
   l1_cmd_ptr->ul_phychan_db->hs_ul_dpch_cfg.dpch_pwr_ctrl_info.pca,
   l1_cmd_ptr->ul_phychan_db->hs_ul_dpch_cfg.dpch_pwr_ctrl_info.tpc_step_size
   );
   
   MSG_5(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCHSRACH :PRACH info avail_sig_mask %d, sf_min %d, sc_num %d, punct_lim %d, avail_sub_chan_mask %d",
   l1_cmd_ptr->ul_phychan_db->hs_prach_cfg.avail_sig_mask,
   l1_cmd_ptr->ul_phychan_db->hs_prach_cfg.sf_min,
   l1_cmd_ptr->ul_phychan_db->hs_prach_cfg.sc_num,
   l1_cmd_ptr->ul_phychan_db->hs_prach_cfg.punct_lim,
   l1_cmd_ptr->ul_phychan_db->hs_prach_cfg.avail_sub_chan_mask
   );
   
   MSG_5(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCHSRACH :PRACH info num_asc %d, pwr_offset_P0 %d, max_preamble_tx %d, e_ai_indication %d, powerOffsetPp_e %d",
   l1_cmd_ptr->ul_phychan_db->hs_prach_cfg.asc_cfg.num_asc,
   l1_cmd_ptr->ul_phychan_db->hs_prach_cfg.pwr_offset_P0,
   l1_cmd_ptr->ul_phychan_db->hs_prach_cfg.max_preamble_tx,
   l1_cmd_ptr->ul_phychan_db->hs_prach_cfg.hs_rach_info.e_ai_indication,
   l1_cmd_ptr->ul_phychan_db->hs_prach_cfg.hs_rach_info.powerOffsetPp_e
   );
   
   }*/
}

void rrchsrach_print_cmac_ul_config(mac_ul_config_type *l2_cmd_ptr)
{

uint8 count=0,count2=0;

  WRRC_MSG1_HIGH("RRCHSRACH : CMAC_UL_CONFIG:MAC hs_rach_action:%d [1-START, 2- RECFG, 4 -STOP]", l2_cmd_ptr->hs_rach_action);

  if((l2_cmd_ptr->hs_rach_action == MAC_HS_RACH_NOOP) || (l2_cmd_ptr->hs_rach_action == MAC_HS_RACH_STOP))
  return;
  
	MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCHSRACH :hs_rach_to_eul_trans:%d mac_hsrach_e_reset_indicator: %d, ERNTI info primary_e_rnti_present  %d primary_e_rnti %d  ",	
    l2_cmd_ptr->hs_rach_to_eul_trans, l2_cmd_ptr->mac_hsrach_e_reset_indicator,
    l2_cmd_ptr->hs_rach_config_ptr->primary_e_rnti_present, l2_cmd_ptr->hs_rach_config_ptr->primary_e_rnti);

	MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCHSRACH :HSRACH new IEs max_e_dch_res_alloc_ccch %d, l1_e_dch_tx_cont_backoff %d max_collision_resol %d, add_e_dch_tx_backoff %d ",	
	l2_cmd_ptr->hs_rach_config_ptr->max_e_dch_res_alloc_ccch,
	l2_cmd_ptr->hs_rach_config_ptr->l1_e_dch_tx_cont_backoff,
	l2_cmd_ptr->hs_rach_config_ptr->max_collision_resol,
	l2_cmd_ptr->hs_rach_config_ptr->add_e_dch_tx_backoff);

	MSG_5(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCHSRACH :PRACH info ccch_asc %d mmax %d nASC %d, nbo1max %d, nbo1min %d",
	l2_cmd_ptr->hs_rach_config_ptr->ccch_asc,
	l2_cmd_ptr->hs_rach_config_ptr->mac_ul_rach_txcntl.mmax,
	l2_cmd_ptr->hs_rach_config_ptr->mac_ul_rach_txcntl.nASC,
	l2_cmd_ptr->hs_rach_config_ptr->mac_ul_rach_txcntl.nbo1max,
	l2_cmd_ptr->hs_rach_config_ptr->mac_ul_rach_txcntl.nbo1min);

  WRRC_MSG2_HIGH("RRCHSRACH :TTI %d num_e_mac_d_flow %d ",
	l2_cmd_ptr->hs_rach_config_ptr->hs_rach_config.e_tti , l2_cmd_ptr->hs_rach_config_ptr->hs_rach_config.num_e_mac_d_flow);

for (count=0; count<l2_cmd_ptr->hs_rach_config_ptr->hs_rach_config.num_e_mac_d_flow;count++)
  { 
	MSG_5(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCHSRACH :Flows info e_mac_d_flow_id %d, e_mac_d_flow_power_offset %d e_mac_d_flow_max_num_of_retx %d, e_mac_d_flow_mux_list %d num_logch %d",
	l2_cmd_ptr->hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].e_mac_d_flow_id,
	l2_cmd_ptr->hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].e_mac_d_flow_power_offset,
	l2_cmd_ptr->hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].e_mac_d_flow_max_num_of_retx,
	l2_cmd_ptr->hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].e_mac_d_flow_mux_list,
	l2_cmd_ptr->hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].num_logch
	);

	for (count2=0; count2<l2_cmd_ptr->hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].num_logch; count2++)	
	{
	  MSG_6(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCHSRACH :LC info rb_id %d, rlc_id %d lc_id %d, rlc_mode %d chan_type %d, rlc_pdu_type %d ",
	  l2_cmd_ptr->hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].e_ul_logch_info[count2].rb_id,
	  l2_cmd_ptr->hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].e_ul_logch_info[count2].rlc_id,
	  l2_cmd_ptr->hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].e_ul_logch_info[count2].lc_id,
	  l2_cmd_ptr->hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].e_ul_logch_info[count2].rlc_mode,
	  l2_cmd_ptr->hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].e_ul_logch_info[count2].chan_type,
	  l2_cmd_ptr->hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].e_ul_logch_info[count2].rlc_pdu_type
	  );
	
	  if(l2_cmd_ptr->hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].e_ul_logch_info[count2].rlc_pdu_type==RLC_FLEXIBLE_PDU_SIZE)
	  {
		MSG_HIGH("RRCHSRACH :Flexible size params ul_li_size%d ul_flex_min_pdu_size %d ul_flex_max_pdu_size %d",
		l2_cmd_ptr->hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].e_ul_logch_info[count2].ul_li_size, 
		l2_cmd_ptr->hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].e_ul_logch_info[count2].ul_flex_min_pdu_size, 
		l2_cmd_ptr->hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].e_ul_logch_info[count2].ul_flex_max_pdu_size 
		);
	  }
	  else if(l2_cmd_ptr->hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].e_ul_logch_info[count2].rlc_pdu_type==RLC_FIXED_PDU_SIZE)
	  {
		WRRC_MSG1_HIGH("RRCHSRACH :Fixed params num_rlc_size_ddi_info%d ",
		l2_cmd_ptr->hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[count].e_ul_logch_info[count2].num_rlc_size_ddi_info);
	  }
	
	}
  }

	MSG_7(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCHSRACH :EDCH info hppybit_delay %d e_tfci_table_index %d e_tfci_min_set %d, no_grant_period_of_si %d, grant_period_of_si %d po_si%d sch_tx_grant_harq_allo_2ms %d",
	l2_cmd_ptr->hs_rach_config_ptr->hs_rach_config.e_dch_info.e_dpcch_info.happy_bit_delay_in_ms,
	l2_cmd_ptr->hs_rach_config_ptr->hs_rach_config.e_dch_info.e_dpdch_info.e_tfci_table_index,
	l2_cmd_ptr->hs_rach_config_ptr->hs_rach_config.e_dch_info.e_dpdch_info.e_tfci_min_set,
	l2_cmd_ptr->hs_rach_config_ptr->hs_rach_config.e_dch_info.e_dpdch_info.sched_info_config.no_grant_periodicity_of_si,
	l2_cmd_ptr->hs_rach_config_ptr->hs_rach_config.e_dch_info.e_dpdch_info.sched_info_config.grant_periodicity_of_si,
	l2_cmd_ptr->hs_rach_config_ptr->hs_rach_config.e_dch_info.e_dpdch_info.sched_info_config.power_offset_of_si,
	l2_cmd_ptr->hs_rach_config_ptr->hs_rach_config.e_dch_info.e_dpdch_info.sched_tx_config.sched_tx_grant_harq_allocation_2ms 
	);

}

void rrchsrach_print_crlc_am_config(rlc_am_config_req_type *l2_cmd_ptr)
{

uint8 count=0;

  for (count=0; count<l2_cmd_ptr->nentity; count++)
  {
    if(l2_cmd_ptr->am_info[count]->rlc_ul_am_config.ul_flex_pdu_size == RLC_FIXED_PDU_SIZE)
    {
        	MSG_6(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCHSRACH :CRLC_AM_CONFIG:action UL %d, rb id %d ul_nchan %d, reestab_entity %d ul_flex_pdu_size %d Fixed size params ul_rlc_size%d",
        l2_cmd_ptr->am_act[count].action,
        l2_cmd_ptr->am_info[count]->common_config.rb_id,
        l2_cmd_ptr->am_info[count]->ul_nchan,
        l2_cmd_ptr->am_info[count]->reestab_entity,
      	l2_cmd_ptr->am_info[count]->rlc_ul_am_config.ul_flex_pdu_size,
      	l2_cmd_ptr->am_info[count]->rlc_ul_am_config.ul_rlc_size
        );
    }
    else if(l2_cmd_ptr->am_info[count]->rlc_ul_am_config.ul_flex_pdu_size == RLC_FLEXIBLE_PDU_SIZE)
    {
        MSG_8(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCHSRACH :CRLC_AM_CONFIG:action UL %d, rb id %d ul_nchan %d, reestab_entity %d ul_flex_pdu_size %d Flexible size params ul_li_size: %d ul_flex_min_pdu_size %d ul_flex_max_pdu_size %d",
        l2_cmd_ptr->am_act[count].action,
        l2_cmd_ptr->am_info[count]->common_config.rb_id,
        l2_cmd_ptr->am_info[count]->ul_nchan,
        l2_cmd_ptr->am_info[count]->reestab_entity,
        l2_cmd_ptr->am_info[count]->rlc_ul_am_config.ul_flex_pdu_size,
          l2_cmd_ptr->am_info[count]->rlc_ul_am_config.ul_li_size,
          l2_cmd_ptr->am_info[count]->rlc_ul_am_config.ul_flex_min_pdu_size,
        l2_cmd_ptr->am_info[count]->rlc_ul_am_config.ul_flex_max_pdu_size
      );
    }
  }

}

void rrchsrach_print_crlc_dl_tm_config(rlc_dl_tm_config_type *l2_cmd_ptr)
{
  int i=0;
    WRRC_MSG9_MED("RRCHSRACH :CRLC_DL_TM_CONFIG:nchan %d lc_id %d, lc_type %d seg_ind %d action %d act_incl %d act_time %d dl_cfg_mask %d ul_cfg_mask %d",	
    l2_cmd_ptr->nchan,l2_cmd_ptr->chan_info->lc_id,
    l2_cmd_ptr->chan_info->lc_type,
    l2_cmd_ptr->chan_info->seg_ind,
    l2_cmd_ptr->dl_tm_act->action,
    l2_cmd_ptr->dl_tm_act->act_incl,
    l2_cmd_ptr->dl_tm_act->act_time,
    l2_cmd_ptr->dl_tm_act->dl_cfg_mask,
    l2_cmd_ptr->dl_tm_act->ul_cfg_mask);

    for(;i<l2_cmd_ptr->nchan;i++)
  {
    WRRC_MSG5_HIGH("CRLC_DL_TM_CONFIG: LC-ID: %d LC-TYPE:%d ACTION:%d dl_cfg_mask:%d, ul_cfg_mask:%d ",
                               l2_cmd_ptr->chan_info[i].lc_id,
                               l2_cmd_ptr->chan_info[i].lc_type,
                               l2_cmd_ptr->dl_tm_act[i].action,
                               l2_cmd_ptr->dl_tm_act[i].dl_cfg_mask,
                               l2_cmd_ptr->dl_tm_act[i].ul_cfg_mask);
  }
}
void rrchsrach_print_crlc_ul_tm_config(rlc_ul_tm_config_type *l2_cmd_ptr)
{

uint8 count=0;
  for (count=0; count<l2_cmd_ptr->nchan; count++)
  {
	MSG_HIGH("RRCHSRACH :CRLC_UL_TM_CONFIG:action UL %d, lc_type%d lc_id %d",
	l2_cmd_ptr->ul_tm_act[count].action,
	l2_cmd_ptr->chan_info[count].lc_type,
	l2_cmd_ptr->chan_info[count].lc_id);
  }

}

void rrchsrach_print_crlc_dl_um_config(rlc_dl_um_config_type *l2_cmd_ptr)
{
    MSG_8(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCHSRACH:CRLC_DL_UM_CONFIG:nchan %d lc_id %d, lc_type %d hfn: %d rlc_size %d ciph_algo %d alternate_e_bit %d li_size %d",
    l2_cmd_ptr->nchan,l2_cmd_ptr->chan_info->lc_id,
    l2_cmd_ptr->chan_info->lc_type,
    l2_cmd_ptr->chan_info->rlc_size,
    l2_cmd_ptr->chan_info->hfn,
    l2_cmd_ptr->chan_info->ciph_algo,
    l2_cmd_ptr->chan_info->alternate_e_bit,
    l2_cmd_ptr->chan_info->li_size);
    
    MSG_5(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCHSRACH CRLC_DL_UM_CONFIG:action %d act_incl %d act_time %d dl_cfg_mask %d ul_cfg_mask %d",	
    l2_cmd_ptr->dl_um_act->action,
    l2_cmd_ptr->dl_um_act->act_incl,
    l2_cmd_ptr->dl_um_act->act_time,
    l2_cmd_ptr->dl_um_act->dl_cfg_mask,
    l2_cmd_ptr->dl_um_act->ul_cfg_mask);
}
void rrchsrach_print_crlc_ul_um_config(rlc_ul_um_config_type *l2_cmd_ptr)
{
uint8 count=0;

  for (count=0; count<l2_cmd_ptr->nchan; count++)
  {
	if(l2_cmd_ptr->chan_info[count].ul_flex_pdu_size == RLC_FIXED_PDU_SIZE)
	{
      MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCHSRACH CRLC_UL_UM_CONFIG:action UL %d, lc_type%d lc_id %d Fixed size params ul_rlc_size%d ",	
      l2_cmd_ptr->ul_um_act[count].action,
      l2_cmd_ptr->chan_info[count].lc_type,
      l2_cmd_ptr->chan_info[count].lc_id,
      l2_cmd_ptr->chan_info[count].rlc_size);
	}
	else if(l2_cmd_ptr->chan_info[count].ul_flex_pdu_size == RLC_FLEXIBLE_PDU_SIZE)
	{
      MSG_5(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCHSRACH CRLC_UL_UM_CONFIG:action UL %d, lc_type%d lc_id %d Flexible size params ul_flex_min_pdu_size %d ul_flex_max_pdu_size %d",	
      l2_cmd_ptr->ul_um_act[count].action,
      l2_cmd_ptr->chan_info[count].lc_type,
      l2_cmd_ptr->chan_info[count].lc_id,
	  l2_cmd_ptr->chan_info[count].ul_flex_min_pdu_size,
      l2_cmd_ptr->chan_info[count].ul_flex_max_pdu_size);
	}
  }
}

/*====================================================================
FUNCTION: rrchsrach_get_previous_cell_supports_hsrach()

DESCRIPTION:
Checks if the previous camped cell support HS-RACH or not

DEPENDENCIES:
  None
  
RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrchsrach_get_previous_cell_supports_hsrach
(
  void
)
{
  boolean status = FALSE;
  rrc_plmn_identity_type prev_plmn_id;
  /* Cell ID */
  rrc_int_cell_id_type prev_cell_id = 0;
  rrc_SysInfoType5 *sib5_ptr = NULL;

  /* Initialize PLMN to dummy value to keep lint happy */
  prev_plmn_id.mcc[0] = 0;
  prev_plmn_id.mcc[1] = 0;
  prev_plmn_id.mcc[2] = 0;
  prev_plmn_id.num_mnc_digits = 3;
  prev_plmn_id.mnc[0] = 0;
  prev_plmn_id.mnc[1] = 0;
  prev_plmn_id.mnc[2] = 0;


  if(rrchsrach_nv_supports_hs_rach() == FALSE)
  {
    WRRC_MSG0_HIGH("RRCHSRACH:HSRACH NV doesnt suppport HSRACH");
    return FALSE;
  }


  rrc_ccm_get_previous_camped_cell_info(&prev_plmn_id,&prev_cell_id);
  PRINT_PLMN_ID(prev_plmn_id )
  WRRC_MSG1_HIGH("RRCHSRACH:prev cell id=%d",prev_cell_id);

  sib5_ptr = (rrc_SysInfoType5 *) rrc_sibdb_return_sib_for_cell(
                            rrc_SIB5, 
                            prev_plmn_id, prev_cell_id);

  if (sib5_ptr == NULL)
  {
     WRRC_MSG0_ERROR("RRCHSRACH:SIB 5 not present in SIB DB for ");
     status = FALSE;
     return status;
  }

  if(FALSE == rrcllc_get_previous_cell_hs_status_in_e_fach())
  {
    return FALSE;
  }
  else
  {
    if(COMMON_EDCH_SYS_INFO_PRESENT_IN_SIB5(sib5_ptr))
    {
      WRRC_MSG0_HIGH("RRCHSRACH:EDCH Common Sys Info is present");
      return TRUE;
    }
    else
    {
      WRRC_MSG0_HIGH("RRCHSRACH:EDCH Common Sys Info not present");
      return FALSE;
    }
  }

  WRRC_MSG1_HIGH("RRCHSRACH:Previous cell HSRACH Status (0:F/1:T) = %d ",status);

  return status;
}

#endif /*FEATURE_WCDMA_HS_RACH*/

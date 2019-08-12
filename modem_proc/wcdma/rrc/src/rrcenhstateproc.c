/*========================================================================================

                                    R R C E N H S T A T E P R O C

DESCRIPTION
  This file contains the definition of the functions that handles the configuration of Enhanced FACH state.

  Copyright (c) 2010 by Qualcomm Technologies, Inc.  All Rights Reserved.
========================================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcenhstateproc.c#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $
  
 when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/02/15   sp      Made changes to configure proper LC ID to MAC during dch to fach state transition on HSRACH cell.
04/06/15   aa      Made changes to provide RB Id and one-sided reestablishment flag status in RLC config procedures
07/21/14   as      Made changes to check-in F3 reduction chnages
03/13/14   sp      Removed code under FEATURE_WCDMA_HS_PCH feature flag
26/12/13   sp      Made changes to set efach action as noop for non-efach to non-efach reselections
07/29/13   ad      Made Changes to reset hs req mask if channel config cnf is not required.
                   Changes to reset the hs req mask before sending cell transition req
03/26/13   db     Changes for unification of Cell Transition and CPHY setup handling.
01/03/13   ad      Made changes to reduce the redundant F3s
11/02/12   db      EFACH Redesign changes to handle SysInofChange Indication
10/26/12   db      EFACH Redesign changes
09/26/12   pm      Made changes to update TOC with BCCH_FACH RLC id
08/13/12   as      Made changes to reset cctrch-id for sccpch after
                   reselection from non-efach to efach cell
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
07/11/12   ad      Changes for including the hsdpcch bitmask in 
                   cell_transition_req for a hsrach cell.Made changes 
                   to handle ERNTI correctly. Changes to avoid starting MAC
                   for a HSRACH to HSRACH SIB5 change.Increased the handing rb size.
04/06/12   ad      Changes for HS-RACH feature
09/08/11   as      Added E-DRX changes for R8
04/26/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.96.50
04/18/11   as      Added code to support feature EFACH-DRX 
03/28/11   rl      Added the file in Osys line due to HS-FACH/PCH changes. 
03/02/11   kp       Added debug code in fun that fills cell trans_req
01/31/11   as       Made changes to update hs_req_mask in efach mode during 
                    cell reselection in connecting state
12/10/10   prk     Added checks for reselection in connecting state.
11/12/10   prk     Added checks for reselection in enh pch state.
10/26/10   prk     Skip update hs_req_mask when HSDPA action is STOP.
10/25/10   prk     Give PSC of HS RL in CELL_TRANS_REQ.
10/21/10   prk     ADD enhPCH bitmask in CELL_TRANS_REQ in PCH state.
10/04/10   prk     Added code to support FEATURE_WCDMA_HS_PCH.
06/01/10   dm/gkg  Support for FEATURE_WCDMA_HS_FACH

===========================================================================*/

#include "wcdma_variation.h"
#include "rrcenhstateproc.h"
#include "rrcenhstatecfg.h"

#ifdef FEATURE_WCDMA_HS_FACH
/*--------------------------------------------------------------------------
       Macro to convert scrambling code to primary scrambling code
--------------------------------------------------------------------------*/
#define RRC_SCR_CODE_TO_PRI_SCR_CODE(scr) ((scr) >> 4)

/*====================================================================
FUNCTION: rrcenhstate_process_add_ccch()

DESCRIPTION:
  This function updates Ordered Config data structures and LLC internal
  data (rrcllc_cmd_under_process) on addition of a CCCH.
  Note that both the Uplink and Downlink data structures are updated.

DEPENDENCIES:
  None
RETURN VALUE:
  Status
                SUCCESS: All data structures needed to establish CCCH
                updated successfully.
SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type  rrcenhstate_process_add_ccch
(
  void
)
{
  /* RLC Uplink logical channel ID */
  rlc_lc_id_type        rlc_ul_log_chl_id;
  /* RLC downlink logical channel ID */
  rlc_lc_id_type        rlc_dl_log_chl_id;
  /* DL watermark pointer assigned for logical channel */
  dsm_watermark_type    *dl_log_chl_queue;
  /* UL watermark pointer assigned for logical channel */
  dsm_watermark_type    *ul_log_chl_queue;

  uint32   num_chl;
#ifdef FEATURE_WCDMA_HS_RACH
  boolean rb_found = FALSE;
  uint8 ix, iy;
#endif
  /* Allocate an Uplink logical channel ID */
  rlc_ul_log_chl_id = rrclcm_allocate_ul_rlc_lc_id
  (
    UE_LOGCHAN_CCCH,
    CCCH_RADIO_BEARER_ID,
    UE_MODE_TRANSPARENT,
    &ul_log_chl_queue
  );


    /* Allocate a downlink logical channel ID */
    rlc_dl_log_chl_id = rrclcm_allocate_dl_rlc_lc_id
    (
      UE_LOGCHAN_CCCH,
      CCCH_RADIO_BEARER_ID,
      UE_MODE_UNACKNOWLEDGED,
      &dl_log_chl_queue
    );

  /* Check if able to successfully allocate UL and DL logical channel IDs */
  if((rlc_ul_log_chl_id != RRCLCM_RLC_LC_ID_ERROR) &&
     (rlc_dl_log_chl_id != RRCLCM_RLC_LC_ID_ERROR))
  {
    /* 2. NOW UPDATE RLC AND MAC ORDERD CONFIG DATA STRUCTURES */
    /* Update RLC Uplink TM channel data */
    rrcllc_cmd_under_process.chan_config.rlc_config_req_list.ul_tm.nchan ++;
    /* Also update the count and logical channel ID in Ordered Config */
    ordered_config_ptr->rlc_ul_tm_parms.chan_info[
      ordered_config_ptr->rlc_ul_tm_parms.nchan].lc_id =
      rlc_ul_log_chl_id;
    ordered_config_ptr->rlc_ul_tm_parms.nchan ++;
    WRRC_MSG1_MED("nchan: Add CCCH UL TM %d after increment",
        ordered_config_ptr->rlc_ul_tm_parms.nchan);

    num_chl = rrcllc_cmd_under_process.chan_config.rlc_config_req_list.ul_tm.nchan;
    if(num_chl < UE_MAX_UL_TM_CHANNEL)
    {
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.ul_tm.chan_info[num_chl-1].lc_id = rlc_ul_log_chl_id;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.ul_tm.chan_info[num_chl-1].lc_type = UE_LOGCHAN_CCCH;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.ul_tm.ul_tm_act[num_chl-1].action = RLC_ESTABLISH;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.ul_tm.ul_tm_act[num_chl-1].act_incl = FALSE;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.ul_tm.chan_info[num_chl-1].seg_ind = FALSE;
      /* TBD: Interface with OC needed to get timer discard value */
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.ul_tm.chan_info[num_chl-1].timer_discard = 0;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.ul_tm.chan_info[num_chl-1].rb_id = CCCH_RADIO_BEARER_ID;
    }
    else
    {
      WRRC_MSG1_ERROR("Number of UL TM Log Chl %d", num_chl);
      return(FAILURE);
    }

    /* Update RLC Downlink UM channel data */
    rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_um.nchan ++;
	
    /* Also update the count and logical channel ID in Ordered Config */
    if(get_rlc_dl_um_info(CCCH_RADIO_BEARER_ID, TRUE, ordered_config_ptr) == FAILURE)
    {
      WRRC_MSG0_ERROR("Config DB could not be updated for DL UM RB CTCH_RADIO_BEARER_ID");
      return(FAILURE);		
    }

    num_chl = rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_um.nchan;
    if(num_chl < UE_MAX_DL_UM_CHANNEL)
    {
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_um.chan_info[num_chl-1].lc_id = rlc_dl_log_chl_id;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_um.chan_info[num_chl-1].lc_type = UE_LOGCHAN_CCCH;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_um.dl_um_act[num_chl-1].action = RLC_ESTABLISH;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_um.dl_um_act[num_chl-1].act_incl = FALSE;
      /* Get RLC size from Ordered Config */
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_um.chan_info[num_chl-1].rlc_size =
        rrcllc_get_dl_um_ccch_rlc_size();//CRC_DEEPTI - Need to check

      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_um.chan_info[num_chl-1].li_size = LI_7_BITS;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_tm.chan_info[num_chl-1].rb_id = CCCH_RADIO_BEARER_ID;
    }
    else
    {
      WRRC_MSG1_ERROR("Number of DL UM Log Chl %d", num_chl);
      return(FAILURE);
    }

    /* Update MAC Ordered Config data with Logical Channel IDs */

    /* Update MAC data in Downlink */
    for(num_chl=0;num_chl < ordered_config_ptr->mac_dl_parms.efach_ndlchans;num_chl++)
    {
      if(ordered_config_ptr->efach_dl_logchan_info[num_chl].rb_id == CCCH_RADIO_BEARER_ID)
      {
        ordered_config_ptr->efach_dl_logchan_info[num_chl].rlc_id = rlc_dl_log_chl_id;
        WRRC_MSG1_HIGH("EFACH: Updating mac rlc_id %d",rlc_dl_log_chl_id);
        break;
      }
    }
    /* Update MAC data in Uplink */
    ordered_config_ptr->mac_ul_parms.cctrch_ptr->cctrch_type = RACH;
    ordered_config_ptr->mac_ul_parms.cctrch_ptr->rach_info.ccch_enable = TRUE;
    ordered_config_ptr->mac_ul_parms.cctrch_ptr->rach_info.ccch_id = rlc_ul_log_chl_id;
    /* OC sets the number of Dedicated logical channels mapped to RACH to 0 */

#ifdef FEATURE_WCDMA_HS_RACH
/*If cell supports HS-RACH set the LC ID for CCCH 
  All other elements for CCCH should have been filled while extracting the common edch info from SIB5 */
    if(TRUE == rrchsrach_get_ready_for_common_edch(ordered_config_ptr)
      ||
      ((TRUE == rrcllc_check_dch_fach_reconfig_status()) && (TRUE == rrchsrach_camped_cell_supports_hsrach())))
    {
      for(ix = 0; (!rb_found) && (ix < ordered_config_ptr->mac_hs_rach_config.hs_rach_config.num_e_mac_d_flow); ix++)
      {
        if(ordered_config_ptr->mac_hs_rach_config.hs_rach_config.e_mac_d_flow_info[ix].e_mac_d_flow_id != INVALID_DFLOW_ID)
        {
          for(iy = 0; (!rb_found) && (iy < ordered_config_ptr->mac_hs_rach_config.hs_rach_config.e_mac_d_flow_info[ix].num_logch); iy++)
          {
            if (ordered_config_ptr->mac_hs_rach_config.hs_rach_config.e_mac_d_flow_info[ix].e_ul_logch_info[iy].rb_id == CCCH_RADIO_BEARER_ID) 
            {
              WRRC_MSG2_HIGH("RRCHSRACH: rb_id %d lc_id %d", CCCH_RADIO_BEARER_ID, rlc_ul_log_chl_id);
              rb_found = TRUE;
              ordered_config_ptr->mac_hs_rach_config.hs_rach_config.e_mac_d_flow_info[ix].e_ul_logch_info[iy].rlc_id = rlc_ul_log_chl_id;
              ordered_config_ptr->mac_ul_parms.ul_pdu_type = MAC_I_PDU_HDR;
              break;
            }
          }
        }
      }
    }

#endif /* FEATURE_WCDMA_HS_RACH */

  } /* Successfully allocated Uplink and Downlink Logical channel IDs */
  else
  {
    WRRC_MSG0_ERROR("Could not get Log Chl for UL/DL CCCH");
    return(FAILURE);
  }

  return(SUCCESS);
} /* rrcllc_process_add_ccch */

/*====================================================================
FUNCTION: rrcenhstate_process_reconfig_ccch()

DESCRIPTION:
  This function updates Ordered Config data structures and LLC internal
  data (rrcllc_cmd_under_process) on addition of a CCCH.
  Note that both the Uplink and Downlink data structures are updated.

DEPENDENCIES:
  None
RETURN VALUE:
  Status
                SUCCESS: All data structures needed to establish CCCH
                updated successfully.
SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type  rrcenhstate_process_reconfig_ccch
(
  void
)
{
  /* RLC Uplink logical channel ID */
  rlc_lc_id_type        rlc_ul_log_chl_id;
  /* RLC downlink logical channel ID */
  rlc_lc_id_type        rlc_dl_log_chl_id;

  uint32   num_chl;
#ifdef FEATURE_WCDMA_HS_RACH
  boolean rb_found = FALSE;
  uint8 ix, iy;
#endif

  /* Get the Uplink logical channel ID already allocated for CCCH*/
  rlc_ul_log_chl_id = rrclcm_check_ul_rlc_lc_id
  (
    UE_LOGCHAN_CCCH,
    CCCH_RADIO_BEARER_ID,
    UE_MODE_TRANSPARENT
  );

  /* Get the downlink logical channel ID already allocated for CCCH */
  rlc_dl_log_chl_id = rrclcm_check_dl_rlc_lc_id
  (
    UE_LOGCHAN_CCCH,
    CCCH_RADIO_BEARER_ID,
    UE_MODE_UNACKNOWLEDGED
  );

  /* Check if able to successfully get UL and DL logical channel IDs */
  if((rlc_ul_log_chl_id != RRCLCM_RLC_LC_ID_ERROR) &&
     (rlc_dl_log_chl_id != RRCLCM_RLC_LC_ID_ERROR))
  {
    /* 2. NOW UPDATE RLC AND MAC ORDERD CONFIG DATA STRUCTURES */

    /* Update RLC Uplink TM channel data */
    rrcllc_cmd_under_process.chan_config.rlc_config_req_list.ul_tm.nchan ++;
    num_chl = rrcllc_cmd_under_process.chan_config.rlc_config_req_list.ul_tm.nchan;
    if(num_chl < UE_MAX_UL_TM_CHANNEL)
    {
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.ul_tm.chan_info[num_chl-1].lc_id = rlc_ul_log_chl_id;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.ul_tm.chan_info[num_chl-1].lc_type = UE_LOGCHAN_CCCH;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.ul_tm.ul_tm_act[num_chl-1].action = RLC_MODIFY;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.ul_tm.ul_tm_act[num_chl-1].ul_cfg_mask = RLC_MODIFY_CFG;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.ul_tm.ul_tm_act[num_chl-1].dl_cfg_mask = 0;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.ul_tm.ul_tm_act[num_chl-1].act_incl = FALSE;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.ul_tm.chan_info[num_chl-1].seg_ind = FALSE;
      /* TBD: Interface with OC needed to get timer discard value */
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.ul_tm.chan_info[num_chl-1].timer_discard = 0;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.ul_tm.chan_info[num_chl-1].rb_id = CCCH_RADIO_BEARER_ID;
    }
    else
    {
      WRRC_MSG1_ERROR("Number of UL TM Log Chl %d", num_chl);
      return(FAILURE);
    }

    /* Update RLC Downlink UM channel data */
    rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_um.nchan ++;
    num_chl = rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_um.nchan;
    if(num_chl < UE_MAX_DL_UM_CHANNEL)
    {
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_um.chan_info[num_chl-1].lc_id = rlc_dl_log_chl_id;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_um.chan_info[num_chl-1].lc_type = UE_LOGCHAN_CCCH;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_um.dl_um_act[num_chl-1].action = RLC_MODIFY;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_um.dl_um_act[num_chl-1].dl_cfg_mask = RLC_MODIFY_CFG;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_um.dl_um_act[num_chl-1].ul_cfg_mask = 0;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_um.dl_um_act[num_chl-1].act_incl = FALSE;
      /* Get RLC size from Ordered Config */
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_um.chan_info[num_chl-1].rlc_size =
        rrcllc_get_dl_um_ccch_rlc_size(); //CRC_DEEPTI - Need to check

      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_um.chan_info[num_chl-1].li_size = LI_7_BITS;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_um.chan_info[num_chl-1].rb_id = CCCH_RADIO_BEARER_ID ;
    }
    else
    {
      WRRC_MSG1_ERROR("Number of DL UM Log Chl %d", num_chl);
      return(FAILURE);
    }
  }


  /* Update MAC Ordered Config data with Logical Channel IDs */
  /* Update MAC data in Downlink */
  for(num_chl=0;num_chl < ordered_config_ptr->mac_dl_parms.efach_ndlchans;num_chl++)
  {
    if(ordered_config_ptr->efach_dl_logchan_info[num_chl].rb_id == CCCH_RADIO_BEARER_ID)
    {
      ordered_config_ptr->efach_dl_logchan_info[num_chl].rlc_id = rlc_dl_log_chl_id;
      WRRC_MSG1_HIGH("EFACH: Updating mac rlc_id %d",rlc_dl_log_chl_id);
      break;
    }
  }
  /* Update MAC data in Uplink */
  ordered_config_ptr->mac_ul_parms.cctrch_ptr->cctrch_type = RACH;
  ordered_config_ptr->mac_ul_parms.cctrch_ptr->rach_info.ccch_enable = TRUE;
  ordered_config_ptr->mac_ul_parms.cctrch_ptr->rach_info.ccch_id = rlc_ul_log_chl_id;
  /* OC sets the number of Dedicated logical channels mapped to RACH to 0 */

  
#ifdef FEATURE_WCDMA_HS_RACH
  /*If cell supports HS-RACH set the LC ID for CCCH 
  All other elements for CCCH should have been filled while extracting the common edch info from SIB5 */
  if(TRUE == rrchsrach_get_ready_for_common_edch(ordered_config_ptr))
  {
    for(ix = 0; (!rb_found) && (ix < ordered_config_ptr->mac_hs_rach_config.hs_rach_config.num_e_mac_d_flow); ix++)
    {
      if(ordered_config_ptr->mac_hs_rach_config.hs_rach_config.e_mac_d_flow_info[ix].e_mac_d_flow_id != INVALID_DFLOW_ID)
      {
        for(iy = 0; (!rb_found) && (iy < ordered_config_ptr->mac_hs_rach_config.hs_rach_config.e_mac_d_flow_info[ix].num_logch); iy++)
        {
          if (ordered_config_ptr->mac_hs_rach_config.hs_rach_config.e_mac_d_flow_info[ix].e_ul_logch_info[iy].rb_id == CCCH_RADIO_BEARER_ID) 
          {
            WRRC_MSG2_HIGH("RRCHSRACH: rb_id %d lc_id %d", CCCH_RADIO_BEARER_ID, rlc_ul_log_chl_id);
            rb_found = TRUE;
            ordered_config_ptr->mac_hs_rach_config.hs_rach_config.e_mac_d_flow_info[ix].e_ul_logch_info[iy].rlc_id = rlc_ul_log_chl_id;
            ordered_config_ptr->mac_ul_parms.ul_pdu_type = MAC_I_PDU_HDR;
            break;
          }
        }
      }
    }
  }

#endif /* FEATURE_WCDMA_HSUPA */
    return(SUCCESS);
}
 /* rrcllc_process_reconfig_ccch */
 
/*====================================================================
FUNCTION: rrcenhstate_process_add_bcch_on_fach

DESCRIPTION:

DEPENDENCIES:
  None
RETURN VALUE:
  Status

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type  rrcenhstate_process_add_bcch_on_hsdsch
(
  void
)
{

  uint8   num_chl;
  /* RLC downlink logical channel ID */
  rlc_lc_id_type        rlc_dl_log_chl_id;
  /* DL watermark pointer assigned for logical channel */
  dsm_watermark_type    *dl_log_chl_queue;

  rlc_dl_log_chl_id = rrclcm_get_dl_rlc_lc_id (BCCH_FACH_RADIO_BEARER_ID);
  
  if(rlc_dl_log_chl_id == RRCLCM_RLC_LC_ID_NOT_FOUND)
  {
    /* Allocate a downlink logical channel ID */
    rlc_dl_log_chl_id = rrclcm_allocate_dl_rlc_lc_id
    (
      UE_LOGCHAN_BCCH,
      BCCH_FACH_RADIO_BEARER_ID,
      UE_MODE_TRANSPARENT,
      &dl_log_chl_queue
    );
  };

  /* Check if able to successfully allocate DL logical channel ID */
  if(rlc_dl_log_chl_id != RRCLCM_RLC_LC_ID_ERROR)
  {
    WRRC_MSG0_HIGH("EFACH:Bcch on HS-DSCH setup");

    /* Update RLC Downlink TM channel data */
    rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_tm.nchan ++;
    /* Also update the count in Ordered Config */
    ordered_config_ptr->rlc_dl_tm_parms.nchan ++;

    num_chl = rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_tm.nchan;

    if(num_chl < UE_MAX_DL_TM_CHANNEL)
    {
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_tm.chan_info[num_chl-1].lc_id = rlc_dl_log_chl_id;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_tm.chan_info[num_chl-1].lc_type = UE_LOGCHAN_BCCH;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_tm.dl_tm_act[num_chl-1].action = RLC_ESTABLISH;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_tm.dl_tm_act[num_chl-1].act_incl = FALSE;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_tm.chan_info[num_chl-1].seg_ind = FALSE;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_tm.chan_info[num_chl-1].rb_id = BCCH_FACH_RADIO_BEARER_ID;
    }
    else
    {
      WRRC_MSG1_ERROR("EFACH:Number of DL TM Log Chl %d", num_chl);
      return(FAILURE);
    }
  } /* Logical Channel ID was allocated */
  else
  {
    WRRC_MSG0_ERROR("EFACH:Log Chl for BCCH_FACH failed");
    return(FAILURE);
  }

  return(SUCCESS);
} /* rrcllc_process_add_bcch_on_fach */

/*====================================================================
FUNCTION: rrcllc_process_reconfig_bcch_on_fach

DESCRIPTION:

DEPENDENCIES:
  None
RETURN VALUE:
  Status

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type  rrcenhstate_process_reconfig_bcch_on_hsdsch
(
  void
)
{

  uint8   num_chl;
  /* RLC downlink logical channel ID */
  rlc_lc_id_type        rlc_dl_log_chl_id;


  /* Allocate a downlink logical channel ID */
  rlc_dl_log_chl_id = rrclcm_check_dl_rlc_lc_id
  (
    UE_LOGCHAN_BCCH,
    BCCH_FACH_RADIO_BEARER_ID,
    UE_MODE_TRANSPARENT
  );

  /* Check if able to successfully allocate DL logical channel ID */
  if(rlc_dl_log_chl_id != RRCLCM_RLC_LC_ID_ERROR)
  {
    WRRC_MSG0_HIGH("EFACH::Bcch-HS DSCH reconfig");

    /* Update RLC Downlink TM channel data */
    rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_tm.nchan ++;
    /* Also update the count in Ordered Config */
    ordered_config_ptr->rlc_dl_tm_parms.nchan ++;

    num_chl = rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_tm.nchan;

    if(num_chl < UE_MAX_DL_TM_CHANNEL)
    {
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_tm.chan_info[num_chl-1].lc_id = rlc_dl_log_chl_id;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_tm.chan_info[num_chl-1].lc_type = UE_LOGCHAN_BCCH;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_tm.dl_tm_act[num_chl-1].action = RLC_MODIFY;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_tm.dl_tm_act[num_chl-1].ul_cfg_mask = 0;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_tm.dl_tm_act[num_chl-1].dl_cfg_mask = RLC_MODIFY_CFG;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_tm.dl_tm_act[num_chl-1].act_incl = FALSE;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_tm.chan_info[num_chl-1].seg_ind = FALSE;
      rrcllc_cmd_under_process.chan_config.rlc_config_req_list.dl_tm.chan_info[num_chl-1].rb_id = BCCH_FACH_RADIO_BEARER_ID;
    }
    else
    {
      WRRC_MSG1_ERROR("EFACH:Number of DL TM Log Chl %d", num_chl);
      return(FAILURE);
    }
  } /* Logical Channel ID was allocated */
  else
  {
    WRRC_MSG0_ERROR("EFACH:Log Chl for BCCH_FACH failed");
    return(FAILURE);
  }

  return(SUCCESS);
} /* rrcllc_process_reconfig_bcch_on_fach */

/*====================================================================
FUNCTION: rrcenhstate_set_hsdpa_l1_info_in_cmd_under_process()

DESCRIPTION:
  This function sets HSDPA Information in L1 struct in rrcllc_cmd_under_process

DEPENDENCIES:
  None.

RETURN VALUE:
  Success/Failure.

SIDE EFFECTS:
  None.
====================================================================*/

uecomdef_status_e_type rrcenhstate_set_hsdpa_l1_info_in_cmd_under_process
(
  ordered_config_type *config_ptr
)
{
  hsdpa_action_enum_type hsdpa_action;

  mac_hs_e_type rrc_machs_ehs;
  rrc_machs_ehs = config_ptr->mac_dl_parms.mac_hs_or_ehs;
 
  if (config_ptr->mac_dl_parms.mac_hs_transition_type == MAC_HS_INVALID_TRANSITION)
  {
   WRRC_MSG0_ERROR("Invalid Transition type");
  }  
  else if(config_ptr->mac_dl_parms.mac_hs_transition_type ==MAC_HS_TO_EHS)
  {
      rrcllc_cmd_under_process.chan_config.l1_req_list.l1_req[0].cmd.setup.hsdpa_l1_ops = 
          HSDPA_SETUP_OPS_RECFG;


      rrcllc_cmd_under_process.chan_config.l1_req_list.l1_req[0].cmd.setup.hsdpa_l1_info =
              &config_ptr->l1_hsdpa_info;
      rrcllc_cmd_under_process.chan_config.l1_req_list.l1_req[0].cmd.setup.hsdpa_l1_info->hs_dsch_info.
        mac_hs_or_ehs = MAC_EHS;
      rrcllc_cmd_under_process.chan_config.hsdpa_hs_to_ehs = TRUE;
      WRRC_MSG1_HIGH("EFACH:HS_EHS transition type is HS to EHS, l1hsaction RECONFIG. BitMask %x",
               rrcllc_cmd_under_process.chan_config.l1_req_list.l1_req[0].cmd.setup.hsdpa_l1_info->hs_req_mask);
  }
  else if(config_ptr->mac_dl_parms.mac_hs_transition_type ==MAC_EHS_TO_HS)
  {
    WRRC_MSG0_ERROR("EFACH: HS Transition type can't be EHS->HS");
  }
  else
  {
    hsdpa_action = rrcllc_get_hs_action_in_e_fach(config_ptr);
    switch(hsdpa_action)
    {
    case  HSDPA_NOOP:
      if ((config_ptr->dest_freq_present == TRUE) &&
            (config_ptr->rrc_state_indicator == RRC_STATE_CELL_DCH) &&
          (rrc_get_hsdpa_status() == HSDPA_ACTIVE))
      {
          /* Change action to RECONFIG as per L1 request */
        rrcllc_cmd_under_process.chan_config.l1_req_list.l1_req[0].cmd.setup.hsdpa_l1_ops =
                HSDPA_SETUP_OPS_RECFG;
    
        rrcllc_cmd_under_process.chan_config.l1_req_list.l1_req[0].
            cmd.setup.hsdpa_l1_info = NULL;

        rrcllc_cmd_under_process.chan_config.l1_req_list.l1_req[0].cmd.setup.hsdpa_l1_info =
                  &config_ptr->l1_hsdpa_info;
        
        rrcllc_cmd_under_process.chan_config.l1_req_list.l1_req[0].cmd.setup.hsdpa_l1_info->hs_dsch_info.
              mac_hs_or_ehs = rrc_machs_ehs;
    
        WRRC_MSG1_HIGH("EFACH:SyncA with HS ACTIVE, l1hsaction is RECONFIG. hs_req_mask %x", 
                 config_ptr->l1_hsdpa_info.hs_req_mask);
      }
      else
      {
        rrcllc_cmd_under_process.chan_config.l1_req_list.l1_req[0].cmd.setup.hsdpa_l1_ops = 
          HSDPA_SETUP_OPS_NOOP;
   
        rrcllc_cmd_under_process.chan_config.l1_req_list.l1_req[0].cmd.setup.hsdpa_l1_info = NULL;
  
      }
      break;
  
    case HSDPA_START:
  
      /* To Add preventive check Later: All the params required to setup HSDPA are present */
  
      rrcllc_cmd_under_process.chan_config.l1_req_list.l1_req[0].cmd.setup.hsdpa_l1_ops =
              HSDPA_SETUP_OPS_START; //crc:index hardcoded to 0?
  
      rrcllc_cmd_under_process.chan_config.l1_req_list.l1_req[0].cmd.setup.hsdpa_l1_info =
              &config_ptr->l1_hsdpa_info;

        rrcllc_cmd_under_process.chan_config.l1_req_list.l1_req[0].cmd.setup.hsdpa_l1_info->hs_dsch_info.
            mac_hs_or_ehs = rrc_machs_ehs;
  
        WRRC_MSG2_HIGH("EFACH:l1hsaction START. BitMask %x for MACHS:1/MACEHS:2 : %d",
                 rrcllc_cmd_under_process.chan_config.l1_req_list.l1_req[0].cmd.setup.hsdpa_l1_info->hs_req_mask,rrc_machs_ehs);

 //     rrcllc_cmd_under_process.chan_config.hsdpa_inactive_to_active = TRUE;
  
      break;
  
    case HSDPA_RECONFIG:
      rrcllc_cmd_under_process.chan_config.l1_req_list.l1_req[0].cmd.setup.hsdpa_l1_ops =
        HSDPA_SETUP_OPS_RECFG;
  
      rrcllc_cmd_under_process.chan_config.l1_req_list.l1_req[0].cmd.setup.hsdpa_l1_info =
        &config_ptr->l1_hsdpa_info;
  
      rrcllc_cmd_under_process.chan_config.l1_req_list.l1_req[0].cmd.setup.hsdpa_l1_info->hs_dsch_info.
          mac_hs_or_ehs = rrc_machs_ehs;
  
  
      break;
  
    case HSDPA_STOP:
      rrcllc_cmd_under_process.chan_config.l1_req_list.l1_req[0].cmd.setup.hsdpa_l1_ops =
        HSDPA_SETUP_OPS_STOP;
  
      rrcllc_cmd_under_process.chan_config.l1_req_list.l1_req[0].cmd.setup.hsdpa_l1_info = NULL;
  
        // GKG - HACK
//      rrcllc_cmd_under_process.chan_config.hsdpa_active_to_inactive = TRUE;
  
      break;
  
    default:
  
  
      rrcllc_cmd_under_process.chan_config.l1_req_list.l1_req[0].cmd.setup.hsdpa_l1_ops =
        HSDPA_SETUP_OPS_NOOP;
  
      rrcllc_cmd_under_process.chan_config.l1_req_list.l1_req[0].cmd.setup.hsdpa_l1_info = NULL;
      break;
    }
  }

  if(rrcllc_cmd_under_process.chan_config.l1_req_list.l1_req[0].cmd.setup.hsdpa_l1_info != NULL)
  {
    MSG_HIGH("EFACH: mac_hs_transition_type %d l1hsaction %d. BitMask %d",
        config_ptr->mac_dl_parms.mac_hs_transition_type, 
        rrcllc_cmd_under_process.chan_config.l1_req_list.l1_req[0].cmd.setup.hsdpa_l1_ops, 
        rrcllc_cmd_under_process.chan_config.l1_req_list.l1_req[0].cmd.setup.hsdpa_l1_info->hs_req_mask);
  }
  return SUCCESS;

}

/*====================================================================
FUNCTION: rrcllc_process_add_ccch()

DESCRIPTION:
  This function updates Ordered Config data structures and LLC internal
  data (rrcllc_cmd_under_process) on addition of a CCCH.
  Note that both the Uplink and Downlink data structures are updated.

DEPENDENCIES:
  None
RETURN VALUE:
  Status
                SUCCESS: All data structures needed to establish CCCH
                updated successfully.
SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type  rrcenhstate_setup_phychan
(
  void
)
{
  if(rrcllc_semi_permanent_data.dl_phy_chan.sccpch_is_up == TRUE) //CRC_DEEPTI: Check behaviour for reentry case
  {
    if(rrcllc_process_drop_sccpch() == FAILURE)
    {
      return FAILURE;
    }
  }
  if(rrcenhstate_add_hs_pdsch() == FAILURE)
  {
    return FAILURE;
  }

  rrcllc_set_hs_action_in_e_fach(ordered_config_ptr, HSDPA_START);

  if (rrcenhstate_set_hsdpa_l1_info_in_cmd_under_process(ordered_config_ptr) == FAILURE)
  {
    WRRC_MSG0_ERROR("EFACH:HSDPA Action Setup FAILURE");
    rrcllc_set_hs_action_in_e_fach(ordered_config_ptr, HSDPA_NOOP);
    return FAILURE;
  }
  else
  {
    ordered_config_ptr->mac_dl_parms.mac_hsdpa_action = HSDPA_START;
    ordered_config_ptr->mac_dl_parms.mac_hs_reset_indicator = FALSE;
  }

  return(SUCCESS);
}

/*====================================================================
FUNCTION: rrcenhstate_reconfig_phychan()

DESCRIPTION:

DEPENDENCIES:
  None
RETURN VALUE:

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type  rrcenhstate_reconfig_phychan
(
  rrc_e_fach_operation_mode_e_type mode
)
{
  if(mode == RRC_E_FACH_COMMON)
  {
    ordered_config_ptr->mac_dl_parms.mac_hs_reset_indicator = FALSE;  
  }

  if(rrcenhstate_reconfig_hs_pdsch() == FAILURE)
  {
    return FAILURE;
  }

  rrcllc_set_hs_action_in_e_fach(ordered_config_ptr, HSDPA_RECONFIG);
    
/*CRC_DEEPTI: Not indicating hs action as reconfig since procedure would be doing it. What about the case when SIB5 changes in 
connecting*/
  if (rrcenhstate_set_hsdpa_l1_info_in_cmd_under_process(ordered_config_ptr) == FAILURE)
  {
    WRRC_MSG0_ERROR("EFACH:HSDPA Action Setup FAILURE");
    rrcllc_set_hs_action_in_e_fach(ordered_config_ptr,HSDPA_NOOP);
    return FAILURE;
  }
  else
  {
    ordered_config_ptr->mac_dl_parms.mac_hsdpa_action = HSDPA_RECONFIG;
  }
  return(SUCCESS);

}

/*====================================================================
FUNCTION: rrcenhstate_release_phychan()

DESCRIPTION:

DEPENDENCIES:
  None
RETURN VALUE:

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type  rrcenhstate_release_phychan
(
  rrc_e_fach_operation_mode_e_type mode
)
{
  if(rrcenhstate_release_hs_pdsch() == FAILURE)
  {
    return FAILURE;
  }

  if (rrcenhstate_set_hsdpa_l1_info_in_cmd_under_process(ordered_config_ptr) == FAILURE)
  {
    WRRC_MSG0_ERROR("EFACH:HSDPA Action Setup FAILURE");
    rrcllc_set_hs_action_in_e_fach(ordered_config_ptr,HSDPA_NOOP);
    return FAILURE;
  }
  else
  {
    ordered_config_ptr->mac_dl_parms.mac_hsdpa_action = HSDPA_STOP;
    ordered_config_ptr->mac_dl_parms.mac_hs_reset_indicator = TRUE;
  }
  return(SUCCESS);

}

/*====================================================================
FUNCTION: rrcenhstate_add_hs_pdsch()

DESCRIPTION:

DEPENDENCIES:
  None
RETURN VALUE:

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcenhstate_add_hs_pdsch
(
  void
)
{
  uint8     dl_cctrch_id;
  int cnt;

    /*mode as an argument and update queue strcture accrodingly */

 /* Get a new CCTrCH ID */
   dl_cctrch_id = rrcllc_get_cctrch_id();
 
   if(dl_cctrch_id != RRCLLC_INVALID_CCTRCH_ID)
   {
     /*Set CCtrchId for both queues  irrespective of mode*/
     for(cnt=0;cnt < ordered_config_ptr->mac_dl_parms.efach_num_queue;cnt++)
     {
       ordered_config_ptr->mac_ehs_queue_info[cnt].cctrch_id = dl_cctrch_id;
     }
     for(cnt=0;cnt < UE_MAX_COMMON_HS_QUEUE;cnt++)
     {
       ordered_config_ptr->efach_common_queue[cnt].cctrch_id = dl_cctrch_id;
     }
   }
   else
   {
     WRRC_MSG1_ERROR("EFACH: Invalid CCTrCH ID %d assigned --> HS PDSCH setup failed", dl_cctrch_id);
     return(FAILURE);
   }
 
   WRRC_MSG1_HIGH("EFACH:Set CCTrCh Id %d in L1 structure",dl_cctrch_id);
   
   /* Set CCTRCH in L1 HSDSCH information */
   ordered_config_ptr->l1_hsdpa_info.hs_dsch_info.cctrch_id = dl_cctrch_id;
   return(SUCCESS);
}

/*====================================================================
FUNCTION: rrcenhstate_config_phychan_for_resel()

DESCRIPTION:
  This function updates Ordered Config data structures and LLC internal
  data (rrcllc_cmd_under_process) on addition of a CCCH.
  Note that both the Uplink and Downlink data structures are updated.

DEPENDENCIES:
  None
RETURN VALUE:
  Status
                SUCCESS: All data structures needed to establish CCCH
                updated successfully.
SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcenhstate_config_phychan_for_resel
(
  rrc_state_e_type  curr_rrc_state
)
{
  uint8     dl_cctrch_id;
  int cnt;
  if( ( (rrcenhstate_camped_cell_supports_e_fach() == TRUE) && 
          (curr_rrc_state == RRC_STATE_CELL_FACH || curr_rrc_state == RRC_STATE_CONNECTING) ))
  {
    if (rrcllc_get_hs_status_in_e_fach(reselection_config_ptr) == TRUE)//CRC_DEEPTI: What if resel ptr is CC?
    {
      WRRC_MSG0_HIGH("EFACH:Reselection eFACH -> eFACH cell");
      if(reselection_config_ptr->l1_hsdpa_info.hs_dsch_info.cctrch_id !=
           RRCLLC_INVALID_CCTRCH_ID)
      {
        RRCLLC_ADD_CCTRCH_ID_TO_RELEASE_LIST(reselection_config_ptr->l1_hsdpa_info.hs_dsch_info.cctrch_id);
        rrcllc_set_hs_action_in_e_fach(reselection_config_ptr,HSDPA_RECONFIG);
      }
      else
      {
        ERR_FATAL("EFACH: CCTrCH ID not assigned to HS PDSCH", 0, 0, 0);
      }
    }
    else
    {
      WRRC_MSG0_HIGH("EFACH:non eFACH -> eFACH cell");
      if(rrcllc_semi_permanent_data.dl_phy_chan.sccpch.cctrch_id !=
       RRCLLC_INVALID_CCTRCH_ID)
      {
        RRCLLC_ADD_CCTRCH_ID_TO_RELEASE_LIST(rrcllc_semi_permanent_data.
                                             dl_phy_chan.sccpch.cctrch_id);
      }
      rrcllc_semi_permanent_data.dl_phy_chan.sccpch.trch_ref_type_bf = 0;
      rrcllc_semi_permanent_data.dl_phy_chan.sccpch.trch_enable_bf = 0;
      rrcllc_set_hs_status_in_e_fach(reselection_config_ptr,TRUE);
      rrcllc_set_hs_action_in_e_fach(reselection_config_ptr,HSDPA_START);
      rrcllc_semi_permanent_data.dl_phy_chan.sccpch.cctrch_id = RRCLLC_INVALID_CCTRCH_ID;
    }
  
   /* Get a new CCTrCH ID */
    dl_cctrch_id = rrcllc_get_cctrch_id();
   
    if(dl_cctrch_id != RRCLLC_INVALID_CCTRCH_ID)
    {
    /*Need mode here to update correct structure, for now keep it common*/
      for(cnt=0;cnt < reselection_config_ptr->mac_dl_parms.efach_num_queue;cnt++)
      {
        reselection_config_ptr->efach_common_queue[cnt].cctrch_id = dl_cctrch_id;
      }
    }
    else
    {
      WRRC_MSG1_ERROR("EFACH: Invalid CCTrCH ID %d assigned", dl_cctrch_id);
      return(FAILURE);
    }
     
    WRRC_MSG1_HIGH("EFACH:Set HSDPA L1 Info and CCTrCh Id %d in L1 structure",dl_cctrch_id);
    
    /* Set CCTRCH in L1 HSDSCH information */
    reselection_config_ptr->l1_hsdpa_info.hs_dsch_info.cctrch_id = dl_cctrch_id;
    
    reselection_config_ptr->mac_dl_parms.mac_hsdpa_action = rrcllc_get_hs_action_in_e_fach(reselection_config_ptr);
      if(reselection_config_ptr->mac_dl_parms.mac_hsdpa_action == HSDPA_RECONFIG)
      {
        reselection_config_ptr->mac_dl_parms.mac_hs_reset_indicator = TRUE;
      }

  }
  else
  {
    if(rrcllc_get_hs_status_in_e_fach(reselection_config_ptr) == TRUE)
    {
      WRRC_MSG0_HIGH("EFACH:Reselection eFACH -> non eFACH cell");
      if(reselection_config_ptr->l1_hsdpa_info.hs_dsch_info.cctrch_id !=
           RRCLLC_INVALID_CCTRCH_ID)
      {
        RRCLLC_ADD_CCTRCH_ID_TO_RELEASE_LIST(reselection_config_ptr->l1_hsdpa_info.hs_dsch_info.cctrch_id);
        rrcllc_set_hs_action_in_e_fach(reselection_config_ptr, HSDPA_STOP);
        reselection_config_ptr->mac_dl_parms.mac_hsdpa_action = HSDPA_STOP;
        reselection_config_ptr->mac_dl_parms.mac_hs_reset_indicator = TRUE;
        rrcllc_set_hs_status_in_e_fach(reselection_config_ptr,FALSE); 
      }
    }
    else
    {
      WRRC_MSG0_HIGH("EFACH:Reselection non eFACH -> non eFACH cell");
      rrcllc_set_hs_action_in_e_fach(reselection_config_ptr, HSDPA_NOOP);
      reselection_config_ptr->mac_dl_parms.mac_hsdpa_action = HSDPA_NOOP;
    }
  }
  return(SUCCESS);
}

/*====================================================================
FUNCTION: rrcenhstate_reconfig_hs_pdsch()

DESCRIPTION:
  This function updates Ordered Config data structures and LLC internal
  data (rrcllc_cmd_under_process) on addition of a CCCH.
  Note that both the Uplink and Downlink data structures are updated.

DEPENDENCIES:
  None
RETURN VALUE:
  Status
                SUCCESS: All data structures needed to establish CCCH
                updated successfully.
SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcenhstate_reconfig_hs_pdsch
(
  void
)
{
  uint8     dl_cctrch_id;
  boolean hs_status;
  int cnt;
  /*mode as an argument and update queue strcture accrodingly */

  hs_status = rrcllc_get_hs_status_in_e_fach(current_config_ptr);
  if (hs_status == TRUE)
  {
    if(ordered_config_ptr->l1_hsdpa_info.hs_dsch_info.cctrch_id !=
       RRCLLC_INVALID_CCTRCH_ID)
    {
      RRCLLC_ADD_CCTRCH_ID_TO_RELEASE_LIST(ordered_config_ptr->l1_hsdpa_info.hs_dsch_info.cctrch_id);
    }
    
    /* Get a new CCTrCH ID */
    dl_cctrch_id = rrcllc_get_cctrch_id();
  
    if(dl_cctrch_id != RRCLLC_INVALID_CCTRCH_ID)
    {
      /*Set CCtrchId for both queues  irrespective of mode*/
      for(cnt=0;cnt < ordered_config_ptr->mac_dl_parms.efach_num_queue;cnt++)
      {
        ordered_config_ptr->mac_ehs_queue_info[cnt].cctrch_id = dl_cctrch_id;
      }
      for(cnt=0;cnt < UE_MAX_COMMON_HS_QUEUE;cnt++)
      {
        ordered_config_ptr->efach_common_queue[cnt].cctrch_id = dl_cctrch_id;
      }
    }
    else
    {
      WRRC_MSG1_ERROR("EFACH:Invalid CCTrCH ID %d assigned --> HS PDSCH reconfig failed", dl_cctrch_id);
      return(FAILURE);
    }
  
    WRRC_MSG1_HIGH("EFACH:Set CCTrCh Id %d in L1 structure",dl_cctrch_id);
    
    /* Set CCTRCH in L1 HSDSCH information */
    ordered_config_ptr->l1_hsdpa_info.hs_dsch_info.cctrch_id = dl_cctrch_id;
  } 
  else
  {
      WRRC_MSG1_ERROR("EFACH:Invalid HS status %d", hs_status);
  }
  return SUCCESS;
}

/*====================================================================
FUNCTION: rrcenhstate_release_hs_pdsch()

DESCRIPTION:
  This function updates Ordered Config data structures and LLC internal
  data (rrcllc_cmd_under_process) on addition of a CCCH.
  Note that both the Uplink and Downlink data structures are updated.

DEPENDENCIES:
  None
RETURN VALUE:
  Status
                SUCCESS: All data structures needed to establish CCCH
                updated successfully.
SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcenhstate_release_hs_pdsch
(
  void
)
{
  boolean hs_status;
  hsdpa_action_enum_type hs_action;

  hs_status = rrcllc_get_hs_status_in_e_fach(current_config_ptr);
  
  hs_action = rrcllc_get_hs_action_in_e_fach(current_config_ptr);

  if (hs_status == TRUE)
  {
    if(ordered_config_ptr->l1_hsdpa_info.hs_dsch_info.cctrch_id !=
       RRCLLC_INVALID_CCTRCH_ID)
    {
      RRCLLC_ADD_CCTRCH_ID_TO_RELEASE_LIST(ordered_config_ptr->l1_hsdpa_info.hs_dsch_info.cctrch_id);
    }
  } 
  else
  {
    ERR_FATAL("EFACH:Invalid HS status %d hs_action %d --> HSPDSCH release failed", hs_status,hs_action,0);
  }
  return SUCCESS;
}

/*====================================================================
FUNCTION: rrcenhstate_add_cell_trans_info()

DESCRIPTION:
  This function updates Ordered Config data structures and LLC internal
  data (rrcllc_cmd_under_process) on addition of a CCCH.
  Note that both the Uplink and Downlink data structures are updated.

DEPENDENCIES:
  None
RETURN VALUE:
  Status
                SUCCESS: All data structures needed to establish CCCH
                updated successfully.
SIDE EFFECTS:
  None.
====================================================================*/
void rrcenhstate_add_cell_trans_info
(
  l1_cell_transition_cmd_type *cell_trans
)
{
  hsdpa_action_enum_type hs_action = rrcllc_get_hs_action_in_e_fach(reselection_config_ptr);
  rrc_state_e_type  curr_rrc_state =rrc_get_state();
      
  WRRC_MSG2_HIGH("EFACH:l1hsaction is %d, Cell trans BitMask %d", hs_action,cell_trans->setup_cmd.req_mask);
  switch(hs_action)
  {
    case  HSDPA_NOOP:
      cell_trans->setup_cmd.hsdpa_l1_ops = HSDPA_SETUP_OPS_NOOP;
      cell_trans->setup_cmd.hsdpa_l1_info = NULL;
      break;
  
    case HSDPA_START:
      cell_trans->setup_cmd.hsdpa_l1_ops = HSDPA_SETUP_OPS_START;
      cell_trans->setup_cmd.hsdpa_l1_info = &reselection_config_ptr->l1_hsdpa_info;
      break;
  
    case HSDPA_RECONFIG:
      cell_trans->setup_cmd.hsdpa_l1_ops = HSDPA_SETUP_OPS_RECFG;
      cell_trans->setup_cmd.hsdpa_l1_info = &reselection_config_ptr->l1_hsdpa_info;
      break;
  
    case HSDPA_STOP:
      cell_trans->setup_cmd.hsdpa_l1_ops = HSDPA_SETUP_OPS_STOP;
      cell_trans->setup_cmd.hsdpa_l1_info = NULL;
      break;
  
    default:
      cell_trans->setup_cmd.hsdpa_l1_ops = HSDPA_SETUP_OPS_NOOP;
      cell_trans->setup_cmd.hsdpa_l1_info = NULL;
      WRRC_MSG0_ERROR("EFACHEFACH:L1 HSDPA action is NOOP");
      break;
  }
  if(hs_action != HSDPA_NOOP)
  {
    cell_trans->setup_cmd.req_mask = CPHY_HS_CHAN_CFG_INCL;
    if(cell_trans->setup_cmd.hsdpa_l1_ops != HSDPA_SETUP_OPS_STOP)
    {
      cell_trans->setup_cmd.hsdpa_l1_info->hs_pdsch_rl_psc = 
        (uint16)RRC_SCR_CODE_TO_PRI_SCR_CODE(rrcllc_cmd_under_process.rrcllc_cell_resel_info.scr_code);
      
      cell_trans->setup_cmd.hsdpa_l1_info->hs_req_mask =  0;

  
#ifdef FEATURE_WCDMA_HS_RACH
        if ((rrchsrach_camped_cell_supports_hsrach() == TRUE) && (reselection_config_ptr->ack_nack_support_on_hs_dpcch))
        {
          cell_trans->setup_cmd.hsdpa_l1_info->hs_req_mask |=  CPHY_HS_DPCCH_CFG_INCL;
          WRRC_MSG1_HIGH("RRCHSRACH: hs_req_mask 0x%x",cell_trans->setup_cmd.hsdpa_l1_info->hs_req_mask);
        }
#endif

      if(rrcllc_get_hs_status_in_e_fach(reselection_config_ptr) == TRUE && 
          ((curr_rrc_state == RRC_STATE_CELL_FACH)||
          (curr_rrc_state == RRC_STATE_CONNECTING)))
      {
#ifdef FEATURE_WCDMA_HS_FACH_DRX
        if(rrcenhstate_get_e_fach_drx_status(cell_trans->setup_cmd.hsdpa_l1_info))
        {
          cell_trans->setup_cmd.hsdpa_l1_info->hs_req_mask |= CPHY_HS_E_FACH_DRX_CFG_ALL_INCL;
          cell_trans->setup_cmd.hsdpa_l1_info->efach_drx_info.dhrnti = cell_trans->setup_cmd.hsdpa_l1_info->hs_dsch_info.h_rnti;
          
        }
        else
#endif
        {
        cell_trans->setup_cmd.hsdpa_l1_info->hs_req_mask |= CPHY_HS_E_FACH_CFG_ALL_INCL;
        }
      }
    }
    cell_trans->setup_cmd.signal_dl_mac_at_act_time = TRUE;
  }
  return;
}
#endif /* FEATURE_WCDMA_HS_FACH*/

/*========================================================================================

                                    R R C E N H S T A T E CFG.C

DESCRIPTION
  This file contains the definitions of the functions that handles the configuration of Enhanced FACH state.

  Copyright (c) 2010 by Qualcomm Technologies, Inc.  All Rights Reserved.
========================================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcenhstatecfg.c#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $
  
 when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/20/16   sg      Made changes to reduce F3s
08/01/15   sr      Made changes for FR29345 EFS/NV Settings to be Read per Sub
01/21/15   sa      Made changes for QTF with CRM
10/22/14   sp      Made changes to remove redundant F3s
10/22/14   gv      Made changes to have connected mode feature set independent of TA status 
08/04/14   sp      Made changes to fix compilation errors
07/21/14   as      Made changes to check-in F3 reduction chnages
07/15/14   ac      NV reorganization
06/09/14   db      RRC Logging Changes for CMAC and CRLC
05/26/14   db      Made changes to include the correct diverstiy value when the optiona IE 
                   'Primary PCCPCH info'is omitted
05/20/14   rmn     Made changes to save DS Tuneaway Status Change indication in connected state based on the concurrency NV
05/02/14   sa      Made changes to disable the FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST by default on DI.3.0
04/18/14   sp      Made changes to replace L1_DL_HS_DSCH_HARQ_MAX_PROC with L1_DL_HS_DSCH_HARQ_MAX_PROC_NO_MIMO
04/10/14   db      Made changes to enable/disable feature support in DSDS/DSDA/TSTS modes through NV
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
03/13/14   sp      Removed code under FEATURE_WCDMA_HS_PCH feature flag
02/13/14   ad      Made changes to remove redundant F3s
01/29/14   sa      Made changes to enable the FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST by default on DI.3.0
01/17/14   sp      Fixed compiler warning
01/09/14   sp      Removed code under FEATURE_CS_VOICE_OVER_HSPA feature
12/24/13   db      Introduced new NV to allow CPC,HSFACH,HSRACH,DC-HSDPA in DSDS and DSDA mode
12/11/13   db      Revert CR 566377 and reverted allocation of bcch-fach lc-id during copy sib to oc for efach
10/07/13   db      Made changes to configure downlink properly upon PCH to FACH thorugh MR
13/08/13   sp      Made changes to replace size argument in strlcpy with size of destination.
06/12/13   as      Made Changes to set the sttd indicator to FALSE when NW doesn't support
06/05/13   as      Made changes to disable hs-rach and mac-i by default thoguh NV.
05/14/13   as      Made changes to configure harq info from SIB5 for common mode
04/16/13   db      Made changes to set sttd indicator to FALSE when IE is not included in SIB5
03/26/13   db      Changes for unification of Cell Transition and CPHY setup handling.
03/21/13   db      Made changes to tie EDRX NV to EFACH
01/25/13   db      Made changes to not configure BCCH FACH in connecting state
11/02/12   db      EFACH Redesign changes to handle SysInofChange Indication
10/26/12   db      EFACH Redesign changes
10/23/12   pm      Made changes to correct enable HSRACH NV only when MACI and HSFACH
                   NVs are also ON
10/18/12   gv      Replace FEATURE_DUAL_SIM_WCDMA_TUNEAWAY with FEATURE_DUAL_SIM
10/03/12   pm      Removed the setting of hrnti status to valid
09/28/12   db      Made changes to correct NV check for EFACH support
09/27/12   pm      Made the changes to update edrx mask between cell reselections
08/14/12   as      Made changes to restore rlc size list for rach
7/24/12   geg      Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
18/06/12   ad      Changes for E-DRX enabling with HS_RACH
04/06/12   ad      Changes for HS-RACH feature
05/10/12   mn      Added changes to support EDRX for Rel8
02/17/12   db      Fixed Compiler errors and warnings
12/20/11   db      Made changes to restore dedicated queue after moving to DCH
12/08/11   db      Made changes to trigger cell update if H-RNTI is missing 
12/7/11    db      Fixed Osys bug while reading HRNTI
12/7/11    as      Made changes to backup queue info if queue is present
11/20/11   db      Made changes to backup and restore dedicated queue info
                   if queue id is same as in common queue
09/21/11   ad      Made corrections to nv changes to turn efach on when epch is on
09/16/11   rl      Made changes to remove the AEEstd library functions 
                   to CoreBsp secured function cal
09/08/11   as      Added E-DRX changes for R8
08/02/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.103.00
20/07/11   as      Made changes to fill correct L1 params for EFACH-DRX
07/05/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.101.00
07/29/11   as      Made changes to reset rlc size list properly during unmap rb
06/21/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.100.50
06/08/11   as      Made changes to not report EFACH capability if Tuneaway is active
05/24/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.98.50
05/09/11   as      Made changes to support E-FACH/E-PCH code for R7
05/20/11   kp      Added macro UE_MAX_COMMON_HRNTI as its getting moved out of uecomdef
04/26/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.96.50
04/18/11   as      Added code to support feature EFACH-DRX 
04/25/11   rl      Fixed the KW error
04/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.94.52
03/30/11   as      Made changes to update rb mapping info for re-enter in epch cell
03/28/11   rl      Added the file in Osys line due to HS-FACH/PCH changes. 
02/15/11   kp      Added code return if SIB5 ptr is NULL in rrcllc_get_previous_cell_sib5_in_e_fach
02/15/11   kp      Added code to restore RACH mapping for SRB1 after CU CNF 
01/18/11   as      Fixed compilation error
01/11/11   as      Fixed lint warning
01/11/11   as      Made changes to send num_pcch_transmissions to L1
                   for HS-PCH Blind decoding
01/06/11   as      Made changes to delete commonn queue info while moving
                   to non EFACH cell or DCH state
12/30/10   prk     In e-fach mode PCCH and BCCH RBs will have same LC ID, to
                   differentiate made changes to pass rb-id as argument to function.
12/20/10   prk     Added a new function to determine efach capability of 
                   previously camped cell.
12/15/10   kp      Added debug print for hs_pcch_info_valid
12/10/10   prk     Passing extra argument to the function to indicate whether to
                   restore all RBs or a particular RB.
11/22/10   prk     Copy SRB1 mapping from hanging RB in dedicated mode of e-fach.
11/19/10   prk     Indicate e-fach capability if UE supports e-fach, irrespective
                   of network support.
11/16/10   kp      Added code to init num_tb_size before filling it. 
10/25/10   prk     Removed the check for C-rnti. 
10/21/10   prk     Use number of pich instead of Pich index.
10/19/10   prk     ASN.1 migration to ASN1 9.3.0
10/12/10   su      Fixed klockwork warnings
10/04/10   prk     Fixed compilation warning.
10/04/10   prk     Added code to support FEATURE_WCDMA_HS_PCH.
10/04/10   prk     Merging latest e-fach fixes.
09/25/10   kp      Adding include files for NV inside HS_FACH feature. 
06/07/10   dm/gkg  Support for FEATURE_WCDMA_HS_FACH

===========================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "wcdma_variation.h"
#include <stringl/stringl.h>

#include "rrcenhstateproc.h"
#include "rrcenhstatecfg.h"


#ifdef FEATURE_WCDMA_HS_FACH

#include "string.h"
#include "fs_fcntl.h"
#include "fs_public.h"
#include "fs_sys_types.h"
#include "string.h"
#include "fs_lib.h"

#ifdef FEATURE_DUAL_SIM
#include "rrcwrm.h"
#endif
#ifdef TEST_FRAMEWORK
#error code not present
#endif
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

#define UE_MAX_COMMON_HRNTI 4
  
/*
  bit0 - e-fach enabled.
  bit1 - e-pch enabled
*/
uint16 rrc_feature_nv_item_value;
#ifdef FEATURE_WCDMA_HS_FACH_DRX
static uecomdef_status_e_type rrcenhstate_set_hs_fach_drx_info
(
  ordered_config_type *config_ptr,
  rrc_HS_DSCH_DrxCellfach_info* hs_dsch_fach_drx
);
#endif

/*====================================================================
FUNCTION: rrc_create_efach_nv_item()

DESCRIPTION:

**********************************************************
#define  RRC_EFACH_ENABLED   0x0001
#define  RRC_EPCH_ENABLED    0x0002
#define RRC_EFACH_DRX_ENABLED 0x0004
#define RRC_HSRACH_ENABLED 0x0008
#define  RRC_MAC_I_ENABLED   0x0010
**********************************************************

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/

void rrc_create_opt_feature_nv_item(sys_modem_as_id_e_type wrrc_as_id)
{
  wnv_init_efs_conf_and_nv_files_msim(WNV_CONF_ID_RRC_FEATURE, wrrc_as_id);
}
/*====================================================================
FUNCTION: rrc_read_efach_nv_item()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/

void rrc_read_opt_feature_nv_item(sys_modem_as_id_e_type wrrc_as_id)
{
  wnv_get_nv_msim(WNV_ID_RRC_FEATURE, &rrc_feature_nv_item_value, sizeof(rrc_feature_nv_item_value), wrrc_as_id);
  WRRC_MSG1_HIGH("EFACH NV item read is %x",rrc_feature_nv_item_value);
}



/*===========================================================================
FUNCTION rrcenhstate_get_mapping_from_sib5

DESCRIPTION
  This function pulls up SRB0/1 Mapping info from SIB5. 
  
DEPENDENCIES

RETURN VALUE
  SUCCESS
  FAILURE

SIDE EFFECTS
  None
===========================================================================*/
static uecomdef_status_e_type rrcenhstate_get_mapping_from_sib5
(
  rb_id_type rb_id,
  ordered_config_type *config_ptr,
  rrc_CommonRBMappingInfo *common_rb_mapping_info
)
{
  uint8 lc_index = UE_MAX_DL_LOGICAL_CHANNEL;

  if(common_rb_mapping_info == NULL)
  {
    WRRC_MSG0_ERROR ("EFACH:rrc_CommonRBMappingInfo not present in SIB5");
    return(FAILURE);
  }
  
  rrcllc_init_efach_mapping_per_rb(config_ptr, rb_id);

  /* Allocate LCH index */
  lc_index = rrcllc_allocate_lc_index_efach(config_ptr, common_rb_mapping_info->logicalChannelIdentity);
  if(lc_index == UE_MAX_DL_LOGICAL_CHANNEL)
  {
    /* Failure to allocate a new LC Index */
    WRRC_MSG0_ERROR("EFACH:Failure to allocate new LCH id");
    return(FAILURE);
  }

  WRRC_MSG2_HIGH("EFACH:MAC-EHS Q %d for SRB%d",common_rb_mapping_info->mac_ehs_QueueId,rb_id);

  config_ptr->efach_dl_logchan_info[lc_index].queue_id = common_rb_mapping_info->mac_ehs_QueueId;
  config_ptr->efach_dl_logchan_info[lc_index].rb_id = rb_id;
  config_ptr->efach_dl_logchan_info[lc_index].rlc_mode = UE_MODE_UNACKNOWLEDGED;

  if(rb_id == CCCH_RADIO_BEARER_ID)
  {
    config_ptr->efach_dl_logchan_info[lc_index].chan_type = UE_LOGCHAN_CCCH;
  }
  else if(rb_id == DCCH_UM_RADIO_BEARER_ID)
  {
    config_ptr->efach_dl_logchan_info[lc_index].chan_type = UE_LOGCHAN_DCCH;
  }
  else
  {
    WRRC_MSG1_ERROR("EFACH:Invalid RB id %d passed",rb_id);
  }

  return SUCCESS;
}

/*===========================================================================
FUNCTION rrcenhstate_set_rb_mapping_info_for_bcch_on_efach

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE
  Gets the index in Config database
SIDE EFFECTS
  None
===========================================================================*/
uint8 rrcenhstate_get_bcch_mapping_from_sib5
(
  ordered_config_type *config_ptr
)
{
  uint8 lc_index = UE_MAX_DL_LOGICAL_CHANNEL;
  rlc_lc_id_type  rlc_dl_log_chl_id;
  
  rlc_dl_log_chl_id = rrclcm_get_dl_rlc_lc_id (BCCH_FACH_RADIO_BEARER_ID);
  
  lc_index = rrcllc_find_lc_index_for_efach(config_ptr, BCCH_ON_HS_DSCH_LCH_ID +1,
  BCCH_FACH_RADIO_BEARER_ID
  );

  if(lc_index == UE_MAX_DL_LOGICAL_CHANNEL)
  {
    /* Allocate LC index */
    lc_index = rrcllc_allocate_lc_index_efach(config_ptr, BCCH_ON_HS_DSCH_LCH_ID +1
    );
 
    if(lc_index == UE_MAX_DL_LOGICAL_CHANNEL)
    {
      /* Failure to allocate a new LC Index */
      WRRC_MSG0_ERROR("MACEHS: Failure to allocate new LC id");
      return(UE_MAX_DL_LOGICAL_CHANNEL);
    }
  }
  config_ptr->efach_dl_logchan_info[lc_index].queue_id = MAC_EHS_QUEUE_ID_FOR_BCCH_PCCH;
  config_ptr->efach_dl_logchan_info[lc_index].rb_id = BCCH_FACH_RADIO_BEARER_ID;
  
  config_ptr->efach_dl_logchan_info[lc_index].chan_type = UE_LOGCHAN_BCCH;
  config_ptr->efach_dl_logchan_info[lc_index].rlc_mode = UE_MODE_TRANSPARENT;
  
  if(rlc_dl_log_chl_id != RRCLCM_RLC_LC_ID_ERROR)
  {
    config_ptr->efach_dl_logchan_info[lc_index].rlc_id = rlc_dl_log_chl_id;
  }
  return lc_index;
}


/*===========================================================================
FUNCTION rrcenhstate_set_mac_ehs_queue_info

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static uecomdef_status_e_type rrcenhstate_get_queue_from_sib5
(
  ordered_config_type *config_ptr,
  rrc_Common_MAC_ehs_ReorderingQueueList* mac_ehs_info
)
{
  uint8 idx=0;  

  if(mac_ehs_info == NULL)
    return FAILURE;

  WRRC_MSG0_HIGH("EFACH: Updating Common Queues into Config Ptr");
  while (mac_ehs_info->n > idx)
  {
    if (mac_ehs_info->elem[idx].mac_ehs_QueueId > INVALID_QUEUE_ID-1)
    {
      WRRC_MSG1_ERROR("MACEHS:MAC EHS Q Id %d, Range 0..7",
          mac_ehs_info->elem[idx].mac_ehs_QueueId);
      return FAILURE;
    }

    config_ptr->efach_common_queue[idx].queue_id = 
      mac_ehs_info->elem[idx].mac_ehs_QueueId;
    
    config_ptr->efach_common_queue[idx].window_size = 
      get_hs_window_size(mac_ehs_info->elem[idx].mac_ehsWindowSize);
      
    config_ptr->efach_common_queue[idx].release_timer = 
      get_hs_release_timer(mac_ehs_info->elem[idx].t1_ReleaseTimer);
    if(mac_ehs_info->elem[idx].m.reorderingResetTimerPresent)
    {
      config_ptr->efach_common_queue[idx].reset_timer = 
        mac_ehs_info->elem[idx].reorderingResetTimer + 1;
    }
    else
    {
      /*Default value is 2*/
      config_ptr->efach_common_queue[idx].reset_timer = 2;
    }
    /*MSG_9(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"EFACH_DEBUG:Queue-ID: %d\tCCTrchId %d\tRelease Timer %d\tWindow_Size %d\tReset Timer %d",
                                            config_ptr->efach_common_queue[idx]->queue_id,
                                            config_ptr->efach_common_queue[idx]->cctrch_id, 
                                            config_ptr->efach_common_queue[idx]->release_timer,
                                            config_ptr->efach_common_queue[idx]->window_size,
                                            config_ptr->efach_common_queue[idx]->reset_timer,0,0,0,0);*/

    idx++;
  }
  config_ptr->mac_dl_parms.efach_num_queue = idx;
  return SUCCESS;
}

/*====================================================================
FUNCTION      set_hsscch_info()

DESCRIPTION   This function sets the Channelization code required
              for HSPDSCH in L1 structure in OC.

DEPENDENCIES  None

RETURN VALUE  SUCCESS if the entire operation is successful,
              FAILURE otherwise

SIDE EFFECTS  None.
====================================================================*/
static uecomdef_status_e_type rrcenhstate_set_hsscch_info
(
  ordered_config_type *config_ptr,
  rrc_HS_SCCH_SystemInfo* hsscch_info_ptr
)
{
  rrc_HS_SCCH_SystemInfo_hS_SCCHChannelisationCodeInfo *hsscch_list_ptr = NULL;

  uint8 cnt = 0, idx=0;

  config_ptr->l1_hsdpa_info.hs_scch_info.sec_scr_code_valid = FALSE;

  /* Sec Scr Code unsupported by L1 */
  if (hsscch_info_ptr->m.dl_ScramblingCodePresent)
  {

    if ((hsscch_info_ptr->dl_ScramblingCode == 0) ||
        (hsscch_info_ptr->dl_ScramblingCode > 15))
    {
      WRRC_MSG1_ERROR("EFACH:Incorrect Sec Scr Code %d", 
          hsscch_info_ptr->dl_ScramblingCode);
      return FAILURE;
    }

    config_ptr->l1_hsdpa_info.hs_scch_info.sec_scr_code_valid = TRUE;

    config_ptr->l1_hsdpa_info.hs_scch_info.sec_scr_code = (uint8)
      hsscch_info_ptr->dl_ScramblingCode;
  }

  /* Channelization Codes always need to be a cummulative sum that network needs to specify.
  So initilize the number to 0 */


  config_ptr->l1_hsdpa_info.hs_scch_info.num_ch_code = 0;

  hsscch_list_ptr = &hsscch_info_ptr->hS_SCCHChannelisationCodeInfo;

  while ((hsscch_list_ptr->n > idx) && (cnt < L1_DL_HS_SCCH_CODES_MAX))
  {
    /* Spec allows allows channelisation code 0 also but since MSM6275 will
    not support secondary scrambling code, it will collide with OVSF 0 and 1
    on SF 256 for CPICH and PCCPCH channels. */
    if ((hsscch_list_ptr->elem[idx] > RRCLLC_MAX_SCCH_CHAN_CODE) || (hsscch_list_ptr->elem[idx] == 0))
    {
      WRRC_MSG1_ERROR("EFACH:Invalid Channelization code %d", hsscch_list_ptr->elem[idx]);
      return FAILURE;
    }
    config_ptr->l1_hsdpa_info.hs_scch_info.ch_code[cnt++] = (uint8) hsscch_list_ptr->elem[idx];
    idx++;
  }

  config_ptr->l1_hsdpa_info.hs_scch_info.num_ch_code = cnt;

  WRRC_MSG2_HIGH("EFACH:No of channelization codes %d, Sec Scr Code present: %d", cnt,hsscch_info_ptr->m.dl_ScramblingCodePresent);
  return SUCCESS;

}

/*====================================================================
FUNCTION      set_hsscch_info()

DESCRIPTION   This function sets the Channelization code required
              for HSPDSCH in L1 structure in OC.

DEPENDENCIES  None

RETURN VALUE  SUCCESS if the entire operation is successful,
              FAILURE otherwise

SIDE EFFECTS  None.
====================================================================*/
static uecomdef_status_e_type rrcenhstate_set_harq_info
(
  l1_dl_hs_dsch_harq_info_struct_type *dest_harq_ptr,
  rrc_HARQ_Info* harq_info_ptr
)
{

 /* Set HARQ Information */
  rrc_HARQ_Info_explicit_* harq_head_ptr = NULL;
  uint32 idx =0;
  uint8 cnt = 0;
 
 
  dest_harq_ptr->num_proc = (uint8)harq_info_ptr->numberOfProcesses;
  
  MSG_MED("EFACH:No of processes %d",harq_info_ptr->numberOfProcesses,0,0); 
  if (RRC_CHECK_COMMON_MSG_TYPE(harq_info_ptr->memoryPartitioning,
   rrc_HARQ_Info_memoryPartitioning_implicit))
  {
   dest_harq_ptr->mem_part_info_valid = FALSE;
   MSG_MED("EFACH:Implicit Mem Partition",0,0,0); 
  }
  else
  {
   dest_harq_ptr->mem_part_info_valid = TRUE;
   
   MSG_MED("EFACH:Explicit Mem Partition",0,0,0); 
   
   if ((harq_head_ptr = harq_info_ptr->memoryPartitioning.u.explicit_) == NULL)
   {
    WRRC_MSG0_ERROR("EFACH:HARQ Mem Part Ptr NULL");
    return FAILURE;
   }
   /* L1_DL_HS_DSCH_HARQ_MAX_PROC_NO_MIMO is used in the while loop instead of L1_DL_HS_DSCH_HARQ_MAX_PROC as 
      L1_DL_HS_DSCH_HARQ_MAX_PROC will be initialized only after CPHY_SETUP */
   while ((harq_head_ptr->n > idx) && (cnt < L1_DL_HS_DSCH_HARQ_MAX_PROC_NO_MIMO))
   {
    dest_harq_ptr->mem_size_type[cnt++] =(l1_dl_hs_dsch_harq_mem_sz_enum_type) harq_head_ptr->elem[idx];
    
    idx++;
    
   }
   
   WRRC_MSG1_HIGH("EFACH:No of HARQ mem sizes %d",cnt);
  }
  
  return SUCCESS;

}

/*====================================================================
FUNCTION      rrcenhstate_get_hrnti_from_sib5()

DESCRIPTION   Updates HRNTI from SIB5 into config ptr

DEPENDENCIES  Should be called only when common mode is to be configured

RETURN VALUE  SUCCESS if the entire operation is successful,
              FAILURE otherwise

SIDE EFFECTS  None.
====================================================================*/
static uecomdef_status_e_type rrcenhstate_get_hrnti_from_sib5
(
  ordered_config_type *config_ptr,
  rrc_HS_DSCH_CommonSystemInformation_common_H_RNTI_information* common_hrnti_info,
  rrc_state_e_type next_rrc_state
)
{

  rrc_HS_DSCH_CommonSystemInformation_common_H_RNTI_information* ie_ptr;
  uint8 num_of_hrnti = 0;
  uint8 hrnti_index = 0;

  ie_ptr = common_hrnti_info;
  if(ie_ptr == NULL)
  {
    return FAILURE;
  }
  
  while((ie_ptr->n > num_of_hrnti) && (num_of_hrnti <= UE_MAX_COMMON_HRNTI))
  {
    num_of_hrnti++;    
  }

  if(next_rrc_state == RRC_STATE_CONNECTING)
  {
    hrnti_index = (uint8)(rrc_convert_initial_ue_identity_into_int_value()
                    % num_of_hrnti);
  }
  else if(next_rrc_state == RRC_STATE_CELL_FACH)
  {
    hrnti_index = (uint8)(config_ptr->mac_dl_parms.rnti_info.urnti
                    % num_of_hrnti);
  }
  else
  {
    WRRC_MSG1_ERROR("EFACH: Invalid next state %d", next_rrc_state);
  }

  if(rrc_translate_hrnti(&ie_ptr ->elem[hrnti_index],
       &config_ptr->l1_hsdpa_info.hs_dsch_info.h_rnti) == FAILURE)
  {
    WRRC_MSG0_ERROR("EFACH: HRNTI invalid");
    return FAILURE;
  }
  
  config_ptr->l1_hsdpa_info.hs_dsch_info.efach_h_rnti_bmsk &=  L1_BCCH_H_RNTI_VALID;
  config_ptr->l1_hsdpa_info.hs_dsch_info.efach_h_rnti_bmsk |= L1_COMMON_H_RNTI_VALID;


  config_ptr->efach_hrnti_status = COMMON_HRNTI;
  config_ptr->efach_hrnti = config_ptr->l1_hsdpa_info.hs_dsch_info.h_rnti;
#ifdef TEST_FRAMEWORK
  #error code not present
#endif

  WRRC_MSG1_HIGH("EFACH: Updated Common HRNTI %d",config_ptr->efach_hrnti);

  return SUCCESS;
}


/*====================================================================
FUNCTION      rrcenhstate_copy_sib5()

DESCRIPTION 

DEPENDENCIES  Before calling this function, you should have done the check whether the cell supports EFACH

RETURN VALUE  
  SUCCESS if the entire operation is successful
  FAILURE otherwise

SIDE EFFECTS  None.
====================================================================*/
static uecomdef_status_e_type rrcenhstate_copy_sib5
(
  ordered_config_type *config_ptr,
  rrc_state_e_type next_rrc_state,
  rrc_e_fach_operation_mode_e_type rrc_e_fach_operation_mode  
)
{
  rrc_HS_DSCH_CommonSystemInformation *hs_dsch_common_sys_info_ptr = NULL;
  rrc_CommonRBMappingInfo default_srb1_MappingInfo;
#ifdef  FEATURE_WCDMA_HS_FACH_DRX
  rrc_HS_DSCH_DrxCellfach_info *hs_dsch_DrxCellfach_info = NULL;
#endif

  rrc_SysInfoType5    *sib5_ptr = NULL;
  sib5_ptr = (rrc_SysInfoType5 *) rrc_sibdb_return_sib_for_srv_cell(rrc_SIB5);
  
  if (sib5_ptr == NULL)
  {
     WRRC_MSG0_ERROR("EFACH: SIB 5 not present in SIB DB");
     return FAILURE;
  }

  hs_dsch_common_sys_info_ptr = &sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.\
    v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions.\
    v770NonCriticalExtensions.sysInfoType5_v770ext.modeSpecificInfo.u.fdd->hs_dsch_CommonSystemInformation;

#ifdef  FEATURE_WCDMA_HS_FACH_DRX
  if(rrcenhstate_camped_cell_supports_e_fach_drx())
  {
    hs_dsch_DrxCellfach_info = &sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.\
      v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions.\
      v770NonCriticalExtensions.v860NonCriticalExtensions.\
      sysInfoType5_v860ext.modeSpecificInfo.u.fdd->hs_dsch_DrxCellfach_info;
      (void)rrcenhstate_set_hs_fach_drx_info(config_ptr, hs_dsch_DrxCellfach_info);
  }

#endif

  /*CCCH Mapping is always taken from SIB5*/
  if( FAILURE == rrcenhstate_get_mapping_from_sib5(CCCH_RADIO_BEARER_ID, config_ptr,
                                                        &hs_dsch_common_sys_info_ptr->ccch_MappingInfo))
  {
    return FAILURE;
  }

  /*We need to out the check here: If CU is the active proc, then only take SRB1 mappings from SIB5*/
  if((rrc_e_fach_operation_mode == RRC_E_FACH_COMMON)&&
      (next_rrc_state == RRC_STATE_CELL_FACH))
  {
    
    if(hs_dsch_common_sys_info_ptr->m.srb1_MappingInfoPresent)
    {
      if(FAILURE == rrcenhstate_get_mapping_from_sib5(DCCH_UM_RADIO_BEARER_ID,config_ptr,
                                                           &hs_dsch_common_sys_info_ptr->srb1_MappingInfo))
      {
        return FAILURE;
      }
    }
    else
    {
      /*default values for SRB1 mapping is given in 10.3.6.36c HS-DSCH common system information*/
      default_srb1_MappingInfo.logicalChannelIdentity = 1;
      default_srb1_MappingInfo.mac_ehs_QueueId = 0;
      WRRC_MSG0_HIGH("EFACH:SRB1 info not present in SIB5, setting default parameters");
      if( FAILURE == rrcenhstate_get_mapping_from_sib5(DCCH_UM_RADIO_BEARER_ID,config_ptr,
                                                           &default_srb1_MappingInfo))
      {
        return FAILURE;
      }
    }
  }

  if( FAILURE == rrcenhstate_get_queue_from_sib5(config_ptr,
                          &hs_dsch_common_sys_info_ptr->common_MAC_ehs_ReorderingQueueList))
    {
      WRRC_MSG0_ERROR("EFACH: Failure in setting MAC E-HS info from SIB5");
      return FAILURE;
    }

  if(rrc_e_fach_operation_mode == RRC_E_FACH_COMMON) /*Need to remove this*/
  {
    if( FAILURE == rrcenhstate_get_hrnti_from_sib5(config_ptr,
                                              &hs_dsch_common_sys_info_ptr->common_H_RNTI_information,
                                               next_rrc_state))
    {
      WRRC_MSG0_ERROR("EFACH: Failure in setting SRB1 Mapping info from SIB5");
      return FAILURE;
    }
  }
  /* Harq info has to be configured from SIB5 for common mode or if dedicated harq info
     is not given in dedicated RRC message  */
  if((config_ptr->l1_hsdpa_info.hs_dsch_info.harq_info.num_proc == 0) || 
        (rrc_e_fach_operation_mode == RRC_E_FACH_COMMON))
  {
    if( FAILURE == rrcenhstate_set_harq_info(&config_ptr->l1_hsdpa_info.hs_dsch_info.harq_info,
                                             &hs_dsch_common_sys_info_ptr->harq_SystemInfo))
    {
      WRRC_MSG0_ERROR("EFACH: Failure in setting common HARQ info");
      return FAILURE;
    }
  }
  /*if(next_rrc_state == RRC_STATE_CELL_FACH)*/
   // This is to make sure that BCCH info goes into config database, so that copy sib5 does not have to be called for dedicated mode
//{
  if( UE_MAX_DL_LOGICAL_CHANNEL == rrcenhstate_get_bcch_mapping_from_sib5(config_ptr))
  {
    WRRC_MSG0_ERROR("EFACH: Failure in setting BCCH Mapping on eFACH");
    return FAILURE;
  }
    if(rrc_translate_hrnti(&hs_dsch_common_sys_info_ptr->bcchSpecific_H_RNTI, 
         &config_ptr->l1_hsdpa_info.hs_dsch_info.bcch_h_rnti ) == FAILURE)
    {
      WRRC_MSG0_ERROR("EFACH: BCCH HRNTI invalid");
      return FAILURE;
    }
    config_ptr->l1_hsdpa_info.hs_dsch_info.efach_h_rnti_bmsk
     |= L1_BCCH_H_RNTI_VALID;
//}

  if( FAILURE == rrcenhstate_set_hsscch_info(config_ptr, &hs_dsch_common_sys_info_ptr->hs_scch_SystemInfo))
  {
    WRRC_MSG0_ERROR("EFACH: Failure in setting SRB1 Mapping info from SIB5");
    return FAILURE;
  }

  /*By default set STTD indicator to FALSE*/
  config_ptr->l1_hsdpa_info.use_sttd = rrcllc_get_correct_pccpch_tx_diversity_from_sibs(next_rrc_state);
  WRRC_MSG1_HIGH("EFACH:Set STTD %d (0:F/1:T) for eFACH configuration",config_ptr->l1_hsdpa_info.use_sttd);

  return SUCCESS;
}


/*====================================================================
FUNCTION: rrcllc_find_lc_index_for_efach()

DESCRIPTION:
  
DEPENDENCIES:
  None.

RETURN VALUE:


SIDE EFFECTS:

  ====================================================================*/
uint8 rrcllc_find_lc_index_for_efach
(
  ordered_config_type *config_ptr,
  rrc_LogicalChannelIdentity logicalChannelIdentity,
  uint16 rb_id
)
{

  uint8 index = UE_MAX_DL_LOGICAL_CHANNEL;

  for(index = 0; index < UE_MAX_DL_LOGICAL_CHANNEL; index++)
  {
    if((config_ptr->efach_dl_lc_id_in_use[index] == TRUE) &&
      (config_ptr->efach_dl_logchan_info[index].rb_id == rb_id))
  {
      WRRC_MSG2_MED("MACEHS:LCH Id index %d, Id %d",index, logicalChannelIdentity);
      return index;
    }
  }

  return UE_MAX_DL_LOGICAL_CHANNEL;

}

/*====================================================================
FUNCTION: rrcllc_allocate_lc_index_efach()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:


SIDE EFFECTS:

====================================================================*/
uint8  rrcllc_allocate_lc_index_efach
( 
  ordered_config_type *config_ptr,
  rrc_LogicalChannelIdentity logicalChannelIdentity
)
{
  uint8 cnt = UE_MAX_DL_LOGICAL_CHANNEL;
  /* An index with the same DFLOW ID is NOT in use.
  * Allocate a new index */
  for (cnt =0; cnt <UE_MAX_DL_LOGICAL_CHANNEL; cnt++)
  {
    if ((config_ptr->efach_dl_lc_id_in_use[cnt] == FALSE) &&
      (config_ptr->efach_dl_logchan_info[cnt].mac_id == INVALID_LOGICAL_CHANNEL_ID))
    {
      /*give it to MAC as LC id -1 as recieved in MAC EHS header*/
      config_ptr->efach_dl_logchan_info[cnt].mac_id= logicalChannelIdentity -1;

      /* Increment no of DFLOWs */
      config_ptr->mac_dl_parms.efach_ndlchans++;
      
      WRRC_MSG2_HIGH("EFACH:Assign Index %d for new LC ID %d",
         cnt,logicalChannelIdentity);
      config_ptr->efach_dl_lc_id_in_use[cnt] = TRUE;
      break;
    }
  }
  return cnt;

}

/*====================================================================
FUNCTION: rrcenhstate_stop_efach_if_active()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:


SIDE EFFECTS:

====================================================================*/
void  rrcenhstate_stop_efach_if_active
( 
  void
)
{
  rrcllc_set_hs_status_in_e_fach(ordered_config_ptr,FALSE);
  if(rrcllc_get_hs_status_in_e_fach(current_config_ptr) == TRUE)
  {
    if(rrc_get_hsdpa_action() != HSDPA_START)
    {
      ordered_config_ptr->mac_dl_parms.mac_hs_or_ehs = MAC_EHS;
      ordered_config_ptr->mac_dl_parms.mac_hs_transition_type = MAC_HS_NO_TRANSITION;
      /*e-fach to R-99 transition-HS will be stopped*/
      rrcllc_determine_hsdpa_action_for_e_fach();
    }
    else
    {
      WRRC_MSG1_MED("EFACH:hsdpa_action %d",rrc_get_hsdpa_action());
    }
  }
#ifdef TEST_FRAMEWORK
  #error code not present
#endif
}

/*====================================================================
FUNCTION: rrcllc_check_efach_config_data()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_check_efach_config_data
(
  ordered_config_type *config_ptr,
  rrc_state_e_type next_rrc_state,
  rrc_e_fach_operation_mode_e_type efach_mode
)
{
  uint8 idx=0,idy=0;

  if(((next_rrc_state == RRC_STATE_CELL_FACH) && (efach_mode == RRC_E_FACH_DEDICATED)))
  {
    if(rrcllc_efach_mappings_are_given_for_rbs(config_ptr) == FALSE)
    {
      return FALSE;
    }
    if(config_ptr->efach_hrnti_status != DEDICATED_HRNTI)
    {
      WRRC_MSG0_HIGH("EFACH:InValid H-RNTI, HS cannot be started");
      return FALSE;
    }
    for(idx=0;idx<UE_MAX_DL_LOGICAL_CHANNEL;idx++)
    {
      if(config_ptr->efach_dl_lc_id_in_use[idx] == FALSE)
        continue;
      if(config_ptr->efach_dl_logchan_info[idx].rb_id >= MAX_RB) // Do not check for BCH
        continue;
      if(config_ptr->efach_dl_logchan_info[idx].rb_id == CCCH_RADIO_BEARER_ID) // Do not check for SRB0
        continue;
      for(idy=0;idy<UE_MAX_HS_QUEUE;idy++)
      {
        if(config_ptr->mac_ehs_queue_index_in_use[idy] == FALSE)
          continue;
        if(config_ptr->mac_ehs_queue_info[idy].queue_id == config_ptr->efach_dl_logchan_info[idx].queue_id)
          break;
      }
      if(idy == UE_MAX_HS_QUEUE )
      {
        WRRC_MSG1_HIGH("EFACH: Queue not found for RB_ID: %d",config_ptr->efach_dl_logchan_info[idx].rb_id);
        return FALSE;
      }
    }
  }
  return TRUE;
}


/*====================================================================
FUNCTION: rrcenhstate_check_for_efach_parms()

DESCRIPTION:
Checks if mapping, HRNTI, queues are present for configuring dedicated mode EFACH

DEPENDENCIES:
  None.

RETURN VALUE:


SIDE EFFECTS:

====================================================================*/
uecomdef_status_e_type  rrcenhstate_check_for_efach_parms
( 
  rrc_RRC_StateIndicator rrc_StateIndicator,
  boolean validate_directed_cell_info,
  boolean validation_check,
  boolean new_hrnti_present
)
{
  uecomdef_status_e_type status = SUCCESS;
  rrc_state_e_type next_rrc_state = RRC_STATE_MAX;

  if( rrc_StateIndicator == rrc_RRC_StateIndicator_cell_FACH )
  {
    if((validate_directed_cell_info == TRUE)&&(validation_check == TRUE)&&
       (rrcenhstate_camped_cell_supports_e_fach() == TRUE))
    {
      /*We are doing validation for connection setup only as for other procedures valiation happens at LLC
            For other procedures other than RCE
             If hrnti is missing CU will be triggered
             if Mapping is missed UE goes to Idle
         */
      if(((rrc_StateIndicator == RRC_STATE_CELL_PCH)||( rrc_StateIndicator == RRC_STATE_URA_PCH)))
      {
        status =  SUCCESS;
      }
      else
      {
        /*Copy hrnti to TOC as validation is happening on TOC. Queues are already in TOC*/
        
        transition_config.toc_ptr->efach_hrnti = ordered_config_ptr->efach_hrnti;
        transition_config.toc_ptr->efach_hrnti_status = ordered_config_ptr->efach_hrnti_status;

        switch(rrc_StateIndicator)
        {
          case rrc_RRC_StateIndicator_cell_FACH:
            next_rrc_state = RRC_STATE_CELL_FACH;
            break;
          case rrc_RRC_StateIndicator_cell_PCH:
            next_rrc_state = RRC_STATE_CELL_PCH;
            break;
          case rrc_RRC_StateIndicator_ura_PCH:
            next_rrc_state = RRC_STATE_URA_PCH;
            break;
          default:
            WRRC_MSG1_ERROR("Invalid state is paased %d",rrc_StateIndicator);
        };
         /*   */
        if(rrcllc_check_efach_config_data(transition_config.toc_ptr,
                                          next_rrc_state,
                                          RRC_E_FACH_DEDICATED) == FALSE)
        {
          WRRC_MSG0_HIGH("EFACH:HS can't be Started/Reconfig in Cell-FACH");
          status = FAILURE;
        }
      }
    }

    if((validate_directed_cell_info == FALSE)&&(new_hrnti_present == FALSE))
    {
      WRRC_MSG0_HIGH("EFACH: Invalidating HRNTI in OC, CC and TOC");
      ordered_config_ptr->efach_hrnti_status = INVALID_HRNTI;
      current_config_ptr->efach_hrnti_status = INVALID_HRNTI;
      transition_config.toc_ptr->efach_hrnti_status = INVALID_HRNTI;
    }
  }
  else
  {
    rrcenhstate_stop_efach_if_active();
  }
  return status;
}

/*====================================================================
FUNCTION: rrcenhstate_evaluate_efach_and_set_lower_layers_action()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:


SIDE EFFECTS:

====================================================================*/
uecomdef_status_e_type  rrcenhstate_evaluate_efach_and_set_lower_layers_action
( 
  ordered_config_type *config_ptr,
  rrc_state_e_type next_rrc_state,
  rrc_e_fach_operation_mode_e_type rrc_e_fach_operation_mode
)
{
  WRRC_MSG1_MED("EFACH:evaluating hsfach actions for lower layers next rrc state %d",next_rrc_state );
  if(rrcenhstate_camped_cell_supports_e_fach()== FALSE)
  {
    return SUCCESS;
  }

  if(((next_rrc_state == RRC_STATE_CELL_PCH)||( next_rrc_state == RRC_STATE_URA_PCH)))
  {
    rrcenhstate_stop_efach_if_active();
    return SUCCESS;
  }
  rrcllc_efach_update_dedicated_params(ordered_config_ptr);
  
  if(rrcllc_check_if_hs_can_be_started_reconfigured_in_e_fach(ordered_config_ptr,
                                                                next_rrc_state,rrc_e_fach_operation_mode) == FALSE)
  {
    rrcllc_set_hs_status_in_e_fach(ordered_config_ptr, FALSE);
    rrcenhstate_stop_efach_if_active();
    WRRC_MSG0_HIGH("EFACH:HS can't be Started/Reconfig in Cell-FACH");
    return FAILURE;
  }
  else
  {
    rrcllc_set_hs_status_in_e_fach(ordered_config_ptr, TRUE);
  }
  rrcllc_determine_hsdpa_action_for_e_fach();
  return SUCCESS;
}
/*====================================================================
FUNCTION: rrcenhstate_init_efach_mapping()

DESCRIPTION: Clears out HSFACH mapping info

DEPENDENCIES:
  None.

RETURN VALUE:


SIDE EFFECTS:

====================================================================*/
void  rrcenhstate_init_efach_mapping
( 
  ordered_config_type *config_ptr
)
{
  uint8 idx = 0;

  config_ptr->mac_dl_parms.efach_ndlchans= 0;
  for(idx = 0; idx < UE_MAX_DL_LOGICAL_CHANNEL; idx++)
  {
    config_ptr->efach_dl_lc_id_in_use[idx] = FALSE;
    config_ptr->efach_dl_logchan_info[idx].queue_id = INVALID_QUEUE_ID;
    config_ptr->efach_dl_logchan_info[idx].mac_id = INVALID_LOGICAL_CHANNEL_ID;
    config_ptr->efach_dl_logchan_info[idx].rb_id = RRC_INVALID_RB_ID;
    config_ptr->efach_dl_logchan_info[idx].rlc_id = 0;
    config_ptr->efach_dl_logchan_info[idx].chan_type = UE_LOGCHAN_NONE;
    config_ptr->efach_dl_logchan_info[idx].rlc_mode = UE_MODE_MAX_NUM;
  }
}
/*====================================================================
FUNCTION: rrcenhstate_init_efach_mapping_per_rb()

DESCRIPTION: Clears out HSFACH mapping info per rb

DEPENDENCIES:
  None.

RETURN VALUE:


SIDE EFFECTS:

====================================================================*/

void rrcllc_init_efach_mapping_per_rb(ordered_config_type *config_ptr, uint16 rb_id)
{
  uint8 idx = 0;

   for(idx=0;idx < UE_MAX_DL_LOGICAL_CHANNEL; idx++)
  {
     if((config_ptr->efach_dl_lc_id_in_use[idx] == TRUE) &&
       (config_ptr->efach_dl_logchan_info[idx].rb_id == rb_id))
     {
         config_ptr->efach_dl_lc_id_in_use[idx] = FALSE;
         config_ptr->efach_dl_logchan_info[idx].queue_id = INVALID_QUEUE_ID;
         config_ptr->efach_dl_logchan_info[idx].mac_id = INVALID_LOGICAL_CHANNEL_ID;
         config_ptr->efach_dl_logchan_info[idx].rb_id = RRC_INVALID_RB_ID;
         config_ptr->efach_dl_logchan_info[idx].rlc_id = 0;
         config_ptr->efach_dl_logchan_info[idx].chan_type = UE_LOGCHAN_NONE;
         config_ptr->efach_dl_logchan_info[idx].rlc_mode = UE_MODE_MAX_NUM;
         if(config_ptr->mac_dl_parms.efach_ndlchans != 0)
         { 
           config_ptr->mac_dl_parms.efach_ndlchans--;
         }
         break;
     }
  } 
}

/*====================================================================
FUNCTION: rrcenhstate_init_efach_queues()

DESCRIPTION: Clears out HSFACH queue info
                    flag -> dediacated or common
DEPENDENCIES:
  None.

RETURN VALUE:

SIDE EFFECTS:

====================================================================*/
void  rrcenhstate_init_efach_queues
( 
  ordered_config_type *config_ptr
)
{
  uint8 idx = 0;
  WRRC_MSG0_MED("EFACH:Init Common Queues ");
  for(idx = 0; idx < UE_MAX_COMMON_HS_QUEUE ; idx++)
  {
    config_ptr->efach_common_queue[idx].queue_id = INVALID_QUEUE_ID;
    config_ptr->efach_common_queue[idx].release_timer = 0;
    config_ptr->efach_common_queue[idx].window_size = 0;
  }
}

/*====================================================================
FUNCTION      rrcenhstate_get_queue_id()

DESCRIPTION 

DEPENDENCIES  Before calling this function, you should have done the check whether the cell supports EFACH

RETURN VALUE  
  SUCCESS if the entire operation is successful
  FAILURE otherwise

SIDE EFFECTS  None.
====================================================================*/
uint8 rrcenhstate_get_queue_id(ordered_config_type *config_ptr, rb_id_type  rb_id)
{
  uint8 ix =0;
  for(ix=0;ix< config_ptr->mac_dl_parms.efach_ndlchans;ix++)
  {
    if(config_ptr->mac_dl_parms.efach_dl_logchan_info[ix]->rb_id == rb_id)
      return config_ptr->mac_dl_parms.efach_dl_logchan_info[ix]->queue_id;
  }
  return INVALID_QUEUE_ID;

}
/*====================================================================
FUNCTION: rrcenhstate_sync_efach_queue()

DESCRIPTION: Clears out HSFACH queue info
                    flag -> dediacated or common
DEPENDENCIES:
  None.

RETURN VALUE:

SIDE EFFECTS:

====================================================================*/
void  rrcenhstate_sync_efach_queue
( 
  ordered_config_type *config_ptr
)
{
  uint8 ix =0;//,ccch_queue_id;
  //boolean found_ccch_queue = FALSE;
  if(config_ptr->hs_status_in_e_fach)
  {
    /*ccch_queue_id = rrcenhstate_get_queue_id(config_ptr,CCCH_RADIO_BEARER_ID);*/
    for(ix=0;ix< config_ptr->mac_dl_parms.efach_num_queue;ix++)
    {
      if(config_ptr->efach_hrnti_status == DEDICATED_HRNTI)
        config_ptr->mac_dl_parms.efach_queue_info[ix] = &config_ptr->mac_ehs_queue_info[ix];
      else
        config_ptr->mac_dl_parms.efach_queue_info[ix] = &config_ptr->efach_common_queue[ix];
      /*if(config_ptr->mac_dl_parms.efach_queue_info[ix]->queue_id == ccch_queue_id)
        found_ccch_queue = TRUE;*/
    }
    /*Adding CCCH queue if not present*/
    /*if((found_ccch_queue == FALSE)&& (config_ptr->efach_hrnti_status == DEDICATED_HRNTI))
    {
      WRRC_MSG0_HIGH("EFACH:Adding CCCH Queue");
      if(config_ptr->efach_common_queue[0].queue_id == ccch_queue_id)
        config_ptr->mac_dl_parms.efach_queue_info[ix] = &config_ptr->efach_common_queue[0];
      else 
        config_ptr->mac_dl_parms.efach_queue_info[ix] = &config_ptr->efach_common_queue[1];
      
      config_ptr->mac_dl_parms.efach_num_queue++;
    }*/
    
  }
}
/*====================================================================
FUNCTION: rrcenhstate_get_hs_dsch_common_info()

DESCRIPTION:
  
DEPENDENCIES:
 The check whether the cell supports EFACH or not must have been done

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
  ====================================================================*/
uecomdef_status_e_type rrcenhstate_get_hs_dsch_common_info
(
  ordered_config_type *config_ptr,
  rrc_state_e_type next_rrc_state,
  rrc_e_fach_operation_mode_e_type rrc_e_fach_operation_mode
)
{
  rrcllc_set_hs_action_in_e_fach(config_ptr,HSDPA_NOOP);
  config_ptr->l1_hsdpa_info.hs_dsch_info.efach_h_rnti_bmsk = 0;

  /*The check whether the cell supports EFACH or not must have been done*/
  if(rrcenhstate_copy_sib5(config_ptr, next_rrc_state, rrc_e_fach_operation_mode) == FAILURE)
  {
    WRRC_MSG0_ERROR("EFACH:Copying SIB5 HS params to Config DB failed.");
    return FAILURE;
  }

  config_ptr->mac_dl_parms.mac_hs_or_ehs = MAC_EHS;
  config_ptr->l1_hsdpa_info.hs_dsch_info.mac_hs_or_ehs = MAC_EHS;
  config_ptr->l1_hsdpa_info.hs_dsch_info.hsdsch_tb_size_alignment_type = 
    L1_DL_HS_DSCH_TB_TABLE_OCTET_ALIGNED;
  config_ptr->l1_hsdpa_info.hs_pdsch_rl_psc = rrc_ccm_get_curr_camped_psc();
  config_ptr->l1_hsdpa_info.hs_dpcch_info.info_present_bitmask = 0;

  if(rrc_e_fach_operation_mode == RRC_E_FACH_COMMON)  
  {
    config_ptr->mac_dl_parms.mac_hs_transition_type = MAC_HS_NO_TRANSITION;
//  HACK: Don't call it coz it'll be called anyway by can_be_started_reconfigured
//    rrcllc_set_hsdpa_l1_queue_info_for_e_fach(config_ptr);
  }

  return SUCCESS;

}
/*====================================================================
FUNCTION: rrcenhstate_verify_sib5_contents()

DESCRIPTION:
This function checks whether the CCCH mapping SIB5 is valid or not

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcenhstate_verify_sib5_contents(rrc_SysInfoType5 *sib5_ptr )
{
  rrc_CommonRBMappingInfo *ccch_mappinginfo_ptr = NULL;
  rrc_Common_MAC_ehs_ReorderingQueueList* common_mac_ehs_queue_list_ptr = NULL;
  uint8 cfgd_queues[8];
  uint8 num_cfgd_queues=0,idx,ccch_qid;
  ccch_mappinginfo_ptr = &sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.\
  v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions.\
  v770NonCriticalExtensions.sysInfoType5_v770ext.modeSpecificInfo.u.fdd->\
  hs_dsch_CommonSystemInformation.ccch_MappingInfo;

  common_mac_ehs_queue_list_ptr = &sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.\
  v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions.\
  v770NonCriticalExtensions.sysInfoType5_v770ext.modeSpecificInfo.u.fdd->\
  hs_dsch_CommonSystemInformation.common_MAC_ehs_ReorderingQueueList;

  while(common_mac_ehs_queue_list_ptr->n > num_cfgd_queues)
  {
    cfgd_queues[num_cfgd_queues] = (uint8)common_mac_ehs_queue_list_ptr->elem[num_cfgd_queues].mac_ehs_QueueId;
    num_cfgd_queues++;    
  }

  ccch_qid = (uint8)ccch_mappinginfo_ptr->mac_ehs_QueueId;
  for(idx=0;idx<num_cfgd_queues;idx++)
  {
    if(ccch_qid == cfgd_queues[idx])
    {
      return TRUE;
    }
  }
  
  return FALSE;  
}

/*====================================================================
FUNCTION: rrcenhstate_nv_supports_e_fach()

DESCRIPTION:
This function checks whether the NV supports eFACH operation or not

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcenhstate_nv_supports_e_fach
(
  void
)
{
#ifdef FEATURE_DUAL_SIM
  uint16 nv_value = rrc_get_concurrency_nv();
#endif

  if(!RRC_IS_EFACH_NV_TURNED_ON)
  {
    WRRC_MSG0_MED("EFACH NV disabled");
    return FALSE;
  }

#ifdef FEATURE_DUAL_SIM
  if(CHECK_CONCURRENCY_FOR_EFACH(nv_value))
  {
    WRRC_MSG1_MED("EFACH : EFACH NV is disabled for device_mode %d", wcdma_device_mode);
    return FALSE;
  }
#endif

  WRRC_MSG0_MED("EFACH NV enabled");
  return TRUE;

}
/*====================================================================
FUNCTION: rrcenhstate_camped_cell_supports_e_fach()

DESCRIPTION:
This function checks whether the currently camped cell supports eFACH operation or not

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcenhstate_camped_cell_supports_e_fach
(
  void
)
{
  rrc_SysInfoType5    *sib5_ptr = NULL;

  if( rrcenhstate_nv_supports_e_fach() == FALSE )
  {
    return FALSE;
  }

  sib5_ptr = (rrc_SysInfoType5 *)rrc_sibdb_return_sib_for_srv_cell(rrc_SIB5);

  if (sib5_ptr == NULL)
  {
     WRRC_MSG0_ERROR("EFACH:SIB 5 not present in SIB DB for ");
     return FALSE;
  }

  if(HSDSCH_COMMON_SYS_INFO_PRESENT_IN_SIB5(sib5_ptr))
  {
    WRRC_MSG0_MED("EFACH:HS Common Sys Info is present");
    if(rrcenhstate_verify_sib5_contents(sib5_ptr) == TRUE)
    {
      return TRUE;
    }
    else
    {
      WRRC_MSG0_ERROR("EFACH:Invalid CCCH mapping in SIB5");
      return FALSE;
    }
  }
  else
  {
    WRRC_MSG0_MED("EFACH:HS Common Sys Info not present");
    return FALSE;
  }
}


/*====================================================================
FUNCTION: rrcenhstate_print_cphy_setup()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcenhstate_print_cphy_setup(l1_setup_cmd_type *l1_cmd_ptr)
{

  MSG_9(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"EFACH_DEBUG:Sig act UL %d, DL %d req_mask 0X%x, hho_setup_mask 0X%x call_type_mask 0X%x freq_incl %d, rf_chan %d, drop_ul_phychan l1_ul_phychan_enum_type_value%d, drop_dl_phychan l1_dl_phychan_enum_type_value%d",
        l1_cmd_ptr->signal_ul_mac_at_act_time,l1_cmd_ptr->signal_dl_mac_at_act_time,
        l1_cmd_ptr->req_mask,l1_cmd_ptr->hho_setup_mask,l1_cmd_ptr->call_type_mask,
        l1_cmd_ptr->freq_incl,l1_cmd_ptr->rf_chan,
        l1_cmd_ptr->drop_ul_phychan,l1_cmd_ptr->drop_dl_phychan);
   
  /*
  MSG_HIGH("EFACH_DEBUG:dl_phychan l1_dl_phychan_enum_type_value%d, ul_phychan l1_ul_phychan_enum_type_value%d, hsdpa_l1_ops %d",
          l1_cmd_ptr->dl_phychan,l1_cmd_ptr->ul_phychan,
          l1_cmd_ptr->hsdpa_l1_ops);
  */
  if(l1_cmd_ptr->hsdpa_l1_info != NULL)
  {

    MSG_9(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"EFACH_DEBUG:HS-OP:%d, REQ-MASK 0X%x, HS-TYPE %d TB_size_alignment %d, PSC %d HRNTI 0X%x\tBCCH-HRNTI 0X%x\tBIT-MASK 0X%x CCTrchId %d",
          l1_cmd_ptr->hsdpa_l1_ops,l1_cmd_ptr->hsdpa_l1_info->hs_req_mask,
          l1_cmd_ptr->hsdpa_l1_info->hs_dsch_info.mac_hs_or_ehs,
          l1_cmd_ptr->hsdpa_l1_info->hs_dsch_info.hsdsch_tb_size_alignment_type,
          l1_cmd_ptr->hsdpa_l1_info->hs_pdsch_rl_psc,
          l1_cmd_ptr->hsdpa_l1_info->hs_dsch_info.h_rnti,
          l1_cmd_ptr->hsdpa_l1_info->hs_dsch_info.bcch_h_rnti,
          l1_cmd_ptr->hsdpa_l1_info->hs_dsch_info.efach_h_rnti_bmsk,
          l1_cmd_ptr->hsdpa_l1_info->hs_dsch_info.cctrch_id);

    MSG_9(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"EFACH_DEBUG:HARQ NumProc %d,Partition(Valid/Invalid::0/1) %d,sec_scr_code_valid %d, sec_scr_code %d num_ch_code %d",
         l1_cmd_ptr->hsdpa_l1_info->hs_dsch_info.harq_info.num_proc,
         l1_cmd_ptr->hsdpa_l1_info->hs_dsch_info.harq_info.mem_part_info_valid,
         l1_cmd_ptr->hsdpa_l1_info->hs_scch_info.sec_scr_code_valid, 
         l1_cmd_ptr->hsdpa_l1_info->hs_scch_info.sec_scr_code, 
         l1_cmd_ptr->hsdpa_l1_info->hs_scch_info.num_ch_code,0,0,0,0);

#ifdef FEATURE_WCDMA_HS_FACH_DRX
   MSG_9(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"EFACH_DRX:t_321 %d, drx_cycle_length %d rx_burst %d,can_hs_data_interrupt_drx %d, dhrnti 0X%x ",
         l1_cmd_ptr->hsdpa_l1_info->efach_drx_info.t_321, 
         l1_cmd_ptr->hsdpa_l1_info->efach_drx_info.drx_cycle_length, 
         l1_cmd_ptr->hsdpa_l1_info->efach_drx_info.rx_burst,
         l1_cmd_ptr->hsdpa_l1_info->efach_drx_info.can_hs_data_interrupt_drx, 
         l1_cmd_ptr->hsdpa_l1_info->efach_drx_info.dhrnti,0,0,0,0);
#endif
  }
}

/*====================================================================
FUNCTION: rrcenhstate_print_cell_transition_req()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcenhstate_print_cell_transition_req(l1_cell_transition_cmd_type *l1_cmd_ptr)
{

   MSG_9(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"EFACH_DEBUG:Signal act time to UL MAC %d, DL MAC %d dl_phychan %d, ul_phychan %d, hsdpa_l1_ops %d, cell_trans req_mask 0x%x",
        l1_cmd_ptr->setup_cmd.signal_ul_mac_at_act_time,l1_cmd_ptr->setup_cmd.signal_dl_mac_at_act_time,
        l1_cmd_ptr->setup_cmd.dl_phychan,l1_cmd_ptr->setup_cmd.ul_phychan,
        l1_cmd_ptr->setup_cmd.hsdpa_l1_ops,l1_cmd_ptr->setup_cmd.req_mask,0,0,0);

  if(l1_cmd_ptr->setup_cmd.hsdpa_l1_info != NULL)
  {
    MSG_9(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"FACH_DEBUG:HS-OP:%d, REQ MASK 0X%x, HS TYPE %d TB_size_alignment %d, PSC %d HRNTI 0X%x\tBCCH-HRNTI 0X%x\tBIT-MASK 0X%x CCTrchId %d ",
          l1_cmd_ptr->setup_cmd.hsdpa_l1_ops,l1_cmd_ptr->setup_cmd.hsdpa_l1_info->hs_req_mask,
          l1_cmd_ptr->setup_cmd.hsdpa_l1_info->hs_dsch_info.mac_hs_or_ehs,
          l1_cmd_ptr->setup_cmd.hsdpa_l1_info->hs_dsch_info.hsdsch_tb_size_alignment_type,
          l1_cmd_ptr->setup_cmd.hsdpa_l1_info->hs_pdsch_rl_psc,
          l1_cmd_ptr->setup_cmd.hsdpa_l1_info->hs_dsch_info.h_rnti,
          l1_cmd_ptr->setup_cmd.hsdpa_l1_info->hs_dsch_info.bcch_h_rnti,
          l1_cmd_ptr->setup_cmd.hsdpa_l1_info->hs_dsch_info.efach_h_rnti_bmsk,
          l1_cmd_ptr->setup_cmd.hsdpa_l1_info->hs_dsch_info.cctrch_id);

    MSG_9(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"HARQ NumProc %d, Partition(Valid/Invalid::0/1) %d, sec_scr_code_valid %d, sec_scr_code %d, num_ch_code %d",
         l1_cmd_ptr->setup_cmd.hsdpa_l1_info->hs_dsch_info.harq_info.num_proc,
         l1_cmd_ptr->setup_cmd.hsdpa_l1_info->hs_dsch_info.harq_info.mem_part_info_valid,
         l1_cmd_ptr->setup_cmd.hsdpa_l1_info->hs_scch_info.sec_scr_code_valid, 
         l1_cmd_ptr->setup_cmd.hsdpa_l1_info->hs_scch_info.sec_scr_code, 
         l1_cmd_ptr->setup_cmd.hsdpa_l1_info->hs_scch_info.num_ch_code,0,0,0,0);

  }
}
/*====================================================================
FUNCTION: rrcllc_get_previous_cell_hs_status_in_e_fach()

DESCRIPTION:

DEPENDENCIES:
  None
  
RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_get_previous_cell_hs_status_in_e_fach
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

  if( rrcenhstate_nv_supports_e_fach() == FALSE )
  {
    return FALSE;
  }


  rrc_ccm_get_previous_camped_cell_info(&prev_plmn_id,&prev_cell_id);
  PRINT_PLMN_ID(prev_plmn_id )
  WRRC_MSG1_HIGH("EFACH:prev cell id=%d",prev_cell_id);


  sib5_ptr = (rrc_SysInfoType5 *) rrc_sibdb_return_sib_for_cell(
                            rrc_SIB5, 
                            prev_plmn_id, prev_cell_id);

  if (sib5_ptr == NULL)
  {
     WRRC_MSG0_ERROR("EFACH:SIB 5 not present in SIB DB for ");
     status = FALSE;
     return status;
  }

  if(HSDSCH_COMMON_SYS_INFO_PRESENT_IN_SIB5(sib5_ptr))
  {
    WRRC_MSG0_HIGH("EFACH:HS Common Sys Info is present");
    if(rrcenhstate_verify_sib5_contents(sib5_ptr) == TRUE)
    {
      status = TRUE;
    }
    else
    {
      WRRC_MSG0_HIGH("EFACH:Invalid CCCH mapping in SIB5");
      status = FALSE;
    }
  }

  WRRC_MSG1_HIGH("EFACH:Get Status (0:F/1:T) = %d ",status);

  return status;
}

/*====================================================================
FUNCTION: rrcenhstate_previous_camped_cell_supports_e_fach()

DESCRIPTION:

DEPENDENCIES:
  None
  
RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcenhstate_previous_camped_cell_supports_e_fach
(
  void
)
{
  boolean status = FALSE;
  rrc_SysInfoType5 * sib5_ptr = NULL;
  rrc_plmn_identity_type prev_plmn_id;
  /* Cell ID */
  rrc_int_cell_id_type prev_cell_id = 0;

  /* Initialize PLMN to dummy value to keep lint happy */
  prev_plmn_id.mcc[0] = 0;
  prev_plmn_id.mcc[1] = 0;
  prev_plmn_id.mcc[2] = 0;
  prev_plmn_id.num_mnc_digits = 3;
  prev_plmn_id.mnc[0] = 0;
  prev_plmn_id.mnc[1] = 0;
  prev_plmn_id.mnc[2] = 0;

  if( rrcenhstate_nv_supports_e_fach() == FALSE  )
  {
    return FALSE;
  }


  rrc_ccm_get_previous_camped_cell_info(&prev_plmn_id,&prev_cell_id);
  PRINT_PLMN_ID(prev_plmn_id )
  WRRC_MSG1_HIGH("EFACH:prev cell id=%d",prev_cell_id);


  sib5_ptr = (rrc_SysInfoType5 *) rrc_sibdb_return_sib_for_cell(
                            rrc_SIB5, 
                            prev_plmn_id, prev_cell_id);

  if (sib5_ptr == NULL)
  {
     WRRC_MSG0_ERROR("EFACH:SIB 5 not present in SIB DB for ");
     return FALSE;
  }

  if(HSDSCH_COMMON_SYS_INFO_PRESENT_IN_SIB5(sib5_ptr))
  {
    if(rrcenhstate_verify_sib5_contents(sib5_ptr) == TRUE)
    {
      status = TRUE;
    }
    else
    {
      WRRC_MSG0_HIGH("EFACH:Invalid CCCH mapping in SIB5");
      status = FALSE;
    }
  }
  if(status)
  {
    WRRC_MSG0_HIGH("EFACH:Previous Cell Supports EFACH");
  }
  else
  {
    WRRC_MSG0_HIGH("EFACH:Previous Cell doesn't Supports EFACH");
  }
  return status;
}


/*====================================================================
FUNCTION: rrcenhstate_print_cmac_dl_setup()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcenhstate_print_cmac_dl_setup(mac_dl_config_type *mac_cmd_ptr)
{
  int i;

  MSG_9(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"EFACH_DEBUG:wait_for_l1_sig %d, num_bchs %d, num_fachs, %d num_dchs, %d,bch_info:cctrch_id %d, bcch_id %d,reset_ind %d, restore_tsn %d",
        mac_cmd_ptr->wait_for_l1_sig,
        mac_cmd_ptr->num_bchs,mac_cmd_ptr->num_fachs, mac_cmd_ptr->num_dchs,
        mac_cmd_ptr->bch_info[0].cctrch_id,mac_cmd_ptr->bch_info[0].bcch_id,
        mac_cmd_ptr->mac_hs_reset_indicator, mac_cmd_ptr->mac_hs_restore_tsns,0);

  MSG_9(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"EFACH_DEBUG:hsdpa_action %d, MAC-HS-TYPE[INVALID|HS|EHS] %d, Transition Type %d, rnti_valid %d, urnti %d, crnti %d",
        mac_cmd_ptr->mac_hsdpa_action,
        mac_cmd_ptr->mac_hs_or_ehs,mac_cmd_ptr->mac_hs_transition_type,
        mac_cmd_ptr->rnti_info.rnti_valid,mac_cmd_ptr->rnti_info.urnti,
        mac_cmd_ptr->rnti_info.crnti,0,0,0);

#ifdef FEATURE_WCDMA_HS_FACH
  if(mac_cmd_ptr->hs_status_in_e_fach == FALSE)
#endif
    return;
  if(mac_cmd_ptr->mac_hs_or_ehs == MAC_EHS)
  {
    WRRC_MSG2_HIGH("EFACH_DEBUG: #Logical Channels: %d \t #Queues %d",mac_cmd_ptr->efach_ndlchans,
                                                                mac_cmd_ptr->efach_num_queue);
    for (i=0; i< mac_cmd_ptr->efach_ndlchans;i++)
  {
     WRRC_MSG1_HIGH("EFACH_DEBUG:DL Logical Chan %d",i);

      MSG_9(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"EFACH_DEBUG:Queue-ID: %d\tRB-ID %d \tRLC-ID %d\tMAC-ID %d\tChan Type %d\tRlc Mode %d",
                                           mac_cmd_ptr->efach_dl_logchan_info[i]->queue_id, 
                                           mac_cmd_ptr->efach_dl_logchan_info[i]->rb_id,
                                           mac_cmd_ptr->efach_dl_logchan_info[i]->rlc_id,
                                           mac_cmd_ptr->efach_dl_logchan_info[i]->mac_id,
                                           mac_cmd_ptr->efach_dl_logchan_info[i]->chan_type,
                                           mac_cmd_ptr->efach_dl_logchan_info[i]->rlc_mode,0,0,0);
  }

    for (i=0; i< mac_cmd_ptr->efach_num_queue;i++)
  {
      MSG_9(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"EFACH_DEBUG:Queue-ID: %d\tCCTrchId %d\tRelease Timer %d\tWindow_Size %d\tReset Timer %d",
                                            mac_cmd_ptr->efach_queue_info[i]->queue_id,
                                            mac_cmd_ptr->efach_queue_info[i]->cctrch_id, 
                                            mac_cmd_ptr->efach_queue_info[i]->release_timer,
                                            mac_cmd_ptr->efach_queue_info[i]->window_size,
                                            mac_cmd_ptr->efach_queue_info[i]->reset_timer,0,0,0,0);
    }
  }
  
  }
/*====================================================================
FUNCTION: rrcenhstate_set_hrnti_status_for_transitions()

DESCRIPTION:
This function set hrnti status as valid
This funcion should be called during fach to fach redirection and
during dch to fach transitions only.

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcenhstate_set_hrnti_status_for_transitions
(
  void
)
{
  if((rrcenhstate_camped_cell_supports_e_fach() == TRUE) &&
     (rrc_get_state() == RRC_STATE_CELL_FACH ) &&
     (transition_config.toc_usage != TOC_INVALID)&&
     (transition_config.toc_ptr->efach_hrnti_status != DEDICATED_HRNTI))
  {
    /*Set Hrnti status as valid to keep rrcllc happy
       so that the final channel config with OTA info 
       will be passed. It doesn't cause any issue as we will be
      doing cell udpate after gettign chanel config cnf */
    transition_config.toc_ptr->efach_hrnti_status = DEDICATED_HRNTI;
    WRRC_MSG0_HIGH("EFACH:Updated HRNTI status in TOC to DEDICATED");
    return TRUE;
  }
  return FALSE;
}


boolean rrcenhstate_check_for_queue_id
(
  ordered_config_type *config_ptr,
  uint8  queue_id
)
{
  uint8 i=0;

  for(i=0; i< UE_MAX_HS_QUEUE; i++)
  {
    if(config_ptr->mac_ehs_queue_index_in_use[i] == FALSE)
      continue;
    
    if(config_ptr->mac_ehs_queue_info[i].queue_id == queue_id)
      return TRUE;
  }
  return FALSE;
}

#ifdef FEATURE_WCDMA_HS_FACH_DRX

/*====================================================================
FUNCTION: rrcenhstate_camped_cell_supports_e_fach_drx()

DESCRIPTION:
This function checks whether the currently camped cell supports eFACH drx operation or not

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcenhstate_camped_cell_supports_e_fach_drx
(
  void
)
{
  rrc_SysInfoType5    *sib5_ptr = NULL;

  if( rrcenhstate_nv_supports_e_fach_drx() == FALSE)
  {
    return FALSE;
  }

  sib5_ptr = (rrc_SysInfoType5 *)rrc_sibdb_return_sib_for_srv_cell(rrc_SIB5);

  if (sib5_ptr == NULL)
  {
     WRRC_MSG0_ERROR("EFACH_DRX:SIB 5 not present in SIB DB for ");
     return FALSE;
  }

  if(HSDSCH_COMMON_HS_DRX_SYS_INFO_PRESENT_IN_SIB5(sib5_ptr))

  {
    WRRC_MSG0_MED("EFACH_DRX:HS Common Sys Info is present");
    return TRUE;
  }
  else
  {
    WRRC_MSG0_MED("EFACH_DRX:HS Common Sys Info not present");
    return FALSE;
  }
}

/*====================================================================
FUNCTION      rrcenhstate_set_hs_fach_drx_info()

DESCRIPTION   This function sets the hs fach drx info
                 in L1 structure in OC.

DEPENDENCIES  None

RETURN VALUE  SUCCESS if the entire operation is successful,
              FAILURE otherwise

SIDE EFFECTS  None.
====================================================================*/
static uecomdef_status_e_type rrcenhstate_set_hs_fach_drx_info
(
  ordered_config_type *config_ptr,
  rrc_HS_DSCH_DrxCellfach_info* hs_dsch_fach_drx
)
{

  if(hs_dsch_fach_drx != NULL)
  {
   
    config_ptr->l1_hsdpa_info.efach_drx_info.t_321 = rrcenhstate_get_t_321_value(hs_dsch_fach_drx->t_321);
    config_ptr->l1_hsdpa_info.efach_drx_info.drx_cycle_length = rrcenhstate_get_drx_cycle_value(hs_dsch_fach_drx->hs_dsch_DrxCycleFach);
    config_ptr->l1_hsdpa_info.efach_drx_info.rx_burst = rrcenhstate_get_drx_burst_value(hs_dsch_fach_drx->hs_dsch_DrxBurstFach);
    config_ptr->l1_hsdpa_info.efach_drx_info.can_hs_data_interrupt_drx = hs_dsch_fach_drx->drxInterruption_hs_dsch;
  }
  else
  {
    WRRC_MSG0_ERROR("EFACH_DRX:rrc_HS_DSCH_DrxCellfach_info not present in SIB ");
    return(FAILURE);
  }
    

  return SUCCESS;
}
/*====================================================================
FUNCTION      rrcenhstate_get_t_321_value()

DESCRIPTION   This function returns the t_321 value.

DEPENDENCIES  None

RETURN VALUE  t 321 

SIDE EFFECTS  None.
====================================================================*/

uint16 rrcenhstate_get_t_321_value(rrc_T_321 t_321)
{
  uint16 int_t_321 = 0;
  
  switch(t_321)
  {
    case rrc_T_321_ms100:
      int_t_321 = 100;
      break;
    case rrc_T_321_ms200:
      int_t_321 = 200;
      break;
    case rrc_T_321_ms400:
      int_t_321 = 400;
      break;
    case rrc_T_321_ms800:
      int_t_321 = 800;
      break;
    default:
      WRRC_MSG1_ERROR("EFACH_DRX:Invalied t_321 value received : %d",t_321);
      break;
  }
  return int_t_321;
}

/*====================================================================
FUNCTION      rrcenhstate_get_drx_cycle_value()

DESCRIPTION   This function returns drx cycle value.

DEPENDENCIES  None

RETURN VALUE  drx cycle

SIDE EFFECTS  None.
====================================================================*/

uint8 rrcenhstate_get_drx_cycle_value(rrc_HS_DSCH_DrxCycleFach drx_cycle)
{
  uint8 int_drx_cycle = 0;
  
  switch(drx_cycle)
  {
    case rrc_HS_DSCH_DrxCycleFach_f4:
      int_drx_cycle = 4;
      break;
    case rrc_HS_DSCH_DrxCycleFach_f8:
      int_drx_cycle = 8;
      break;
    case rrc_HS_DSCH_DrxCycleFach_f16:
      int_drx_cycle = 16;
      break;
    case rrc_HS_DSCH_DrxCycleFach_f32:
      int_drx_cycle = 32;
      break;
    default:
      WRRC_MSG1_ERROR("EFACH_DRX:Invalied drx_cycle value received : %d",drx_cycle);
      break;
  }
  return int_drx_cycle;
}

/*====================================================================
FUNCTION      rrcenhstate_get_drx_burst_value()

DESCRIPTION   This function returns the drx burst value.

DEPENDENCIES  None

RETURN VALUE  drx burst

SIDE EFFECTS  None.
====================================================================*/

uint8 rrcenhstate_get_drx_burst_value(rrc_HS_DSCH_DrxBurstFach drx_burst)
{
  uint8 int_drx_burst = 0;
  
  switch(drx_burst)
  {
    case rrc_HS_DSCH_DrxBurstFach_f1:
      int_drx_burst = 1;
      break;
    case rrc_HS_DSCH_DrxBurstFach_f2:
      int_drx_burst = 2;
      break;
    case rrc_HS_DSCH_DrxBurstFach_f4:
      int_drx_burst = 4;
      break;
    case rrc_HS_DSCH_DrxBurstFach_f8:
      int_drx_burst = 8;
      break;
    case rrc_HS_DSCH_DrxBurstFach_f16:
      int_drx_burst = 16;
      break;
    default:
      WRRC_MSG1_ERROR("EFACH_DRX:Invalied drx_burst value received : %d",drx_burst);
      break;
  }
  return int_drx_burst;
}

/*====================================================================
FUNCTION: rrcenhstate_nv_supports_e_fach_drx()

DESCRIPTION:
This function checks whether the NV supports eFACH drx operation or not

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcenhstate_nv_supports_e_fach_drx
(
  void
)
{
#ifdef FEATURE_DUAL_SIM
  uint16 nv_value = rrc_get_concurrency_nv();
#endif

  if(!RRC_IS_EDRX_NV_TURNED_ON)
  {
    WRRC_MSG0_MED("EFACH_DRX: NV disabled");
    return FALSE;
  }

#ifdef FEATURE_DUAL_SIM
  if(CHECK_CONCURRENCY_FOR_EDRX(nv_value))
  {
    WRRC_MSG1_MED("EFACH_DRX : EDRX NV is disabled for device_mode %d", wcdma_device_mode);
    return FALSE;
  }
#endif

  WRRC_MSG0_MED("EFACH_DRX: NV enabled");
  return TRUE;

}

/*====================================================================
FUNCTION: rrcenhstate_get_e_fach_drx_status()

DESCRIPTION:
This function returns the EFACH DRX status 

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcenhstate_get_e_fach_drx_status
(
  l1_hsdpa_info_struct_type *hsdpa_l1_info
)
{
  boolean status = FALSE;
  if( rrcenhstate_nv_supports_e_fach_drx() == FALSE )
  {
    return FALSE;
  }
  if(rrcenhstate_camped_cell_supports_e_fach_drx() && 
    (hsdpa_l1_info->hs_dsch_info.efach_h_rnti_bmsk & L1_DEDICATED_H_RNTI_VALID) &&
#ifdef FEATURE_WCDMA_HS_RACH
    ((hsdpa_l1_info->efach_drx_info.can_hs_data_interrupt_drx)||
	 (rrchsrach_camped_cell_supports_hsrach())
    )
#else
    (hsdpa_l1_info->efach_drx_info.can_hs_data_interrupt_drx)
#endif
     &&
     (hsdpa_l1_info->efach_drx_info.rx_burst < hsdpa_l1_info->efach_drx_info.drx_cycle_length))
  {
    status = TRUE;;
  }
  else
  {
    status = FALSE;
  }
  WRRC_MSG1_HIGH("EFACH_DRX: status : %d",status);
  return status;
}

#endif /* FEATURE_WCDMA_HS_FACH_DRX  */

#ifdef TEST_FRAMEWORK
#error code not present
#endif
#endif /*FEATURE_WCDMA_HS_FACH*/

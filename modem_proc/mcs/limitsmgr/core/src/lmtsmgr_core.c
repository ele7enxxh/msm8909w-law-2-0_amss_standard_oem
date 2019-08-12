
/*!
  @file
  lmtsmgr_core.c

  @brief
  This file implements the core functions of LIMTS MGR module.

  @ingroup per_implementation
*/

/*=============================================================================

  Copyright (c) 2013-2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/core/src/lmtsmgr_core.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/10/16   jm      Reducing F3 messages in Limits Manager
05/09/16   jm      Set priority criteria for data+data high for DSDA only
01/25/16   jm      F3 Reduction
01/06/15   rj      Move Translation functions in Limitsmgr to a seperate file
12/23/14   rj      Validate conflict_tbl index from wwcoex_get_table_node
12/10/14   ag      For G+G data+mms, floor all tiers except tier 5 
12/03/14   sk      featurize tech state update to TRM with ASDiv feature flag
12/02/14   rj      KW fixes
11/25/14   ag      On non-BO PLs, use mmcp information for single sim stack prios
11/11/14   ag      fix incorrect casting of 32 bit freqId array to 16 bit
11/06/14   rj      QTF compilation error fix
11/05/14   rj      Cleanup DDR related processing from Core file
11/04/14   ag      Added TTL markers for profiling 
10/27/14   jm      Interface change to support 4-bit action
10/27/14   jm      KW fixes
10/27/14   rj      Cleanup SAR related processing
10/15/14   jm      Proper chain ownership check in slot level timeline
10/16/14   ag      Fixed error causing out of bound array access
10/08/14   jm      FreqId generation support in sim mode
09/23/14   sg      Spur mitigation support
10/08/14   ag      Added support for coex tech state indication
09/23/14   ag      For IRAT handover, reset lprio tech only if it was prev MAX
09/12/14   ag      Added support for call in distress
09/10/14   jm      Band avoidance support for 1X/HDR/WCDMA/TDSCDMA
09/09/14   rj      Merged non-IM BackOff and Blanking EFS
09/03/14   ag      Use num_entries to get other active tech instead of TRM lock
08/29/14   jm      Change SL TL sim to use FreqId Pointer
08/25/14   jm      Circular buffer for slot level activity tl
08/22/14   jm      Simulator support for slot level timeline
08/13/14   rj      Disable SVLTE+G Idle Mode Hopping
08/12/14   jm      Slot level timeline support
08/04/14   jm      channel id support for Single-TX DSDA
07/30/14   sg      Enabled filter path and backoff for DR-DSDS mode
07/22/14   rj      Cleanup FilterPath related BOLT feature in Lmtsmgr
07/15/14   jm      enable desense calculation for dr-dsds
06/23/14   sg      desense indication mechanism
05/09/14   rj      KW fixes 
05/06/14   jm      KW Warning Fixes
05/04/14   rj      Filter Path support to send GSM Tx specific flag info to RF
05/02/14   rj      KW fixes
05/01/14   rj      Update Coex power limits to MAX, when moved into filter path
04/28/14   jm      L+G Band Avoidance support
04/19/14   rj      1X-SLTE support added 
04/17/14   ag      Priorities for techs on same sub are fixed
04/17/14   rj      Check blanking EFS only if list returns action = BLANK
04/07/14   ag      Resetting the inactivity timer before sending dereg msg to GSM
03/27/14   jm      Proper error handling for get table node
03/21/14   rj      Only memscpy action array and don't re-write freqID_offsets
03/19/14   jm      WWAN coex based off of RF device assignment
03/19/14   ag      Reset power_change flag only in the outer tech loop
03/17/14   rj      WWcoex Action info packet not getting captured
03/17/14   rj      Fix for action table not updated in logging
03/17/14   jm      Fix for action table when mult. freq are given on same band
02/05/14   jm      Adding slot level backoff support for SVLTE/SGTDS
01/14/14   jm      Ensure freqID is 16-bit value
12/19/13   jm      GPS support for DDR coex
12/19/13   jm      DDR management phase 2 support (diversity power + GPS)
11/27/13   jm      Include min/max channel ranges as part of desensed channels
11/23/13   ag      Ignore freq/power comparisons for DSDS/TSTS mode
11/22/13   jm      Resolution of LLVM Warnings
11/21/13   jm      Adding DDR management support
11/16/13   rj      Adding macro prio callback support
11/06/13   rj      Adding prob_aggr_mask support
10/22/13   ag      Rescheduling searches based on power thresholds
09/19/13   rj      Adding support for GSM to ASID mapping
08/22/13   ag      KW fix and use high prio tech power for IM power backoff
08/09/13   ag      Added NPA notification for BL
07/24/13   ag      Support for conveying macro priority to techs
07/19/13   ag      Set action as unknown for DSDS/TSTS
07/01/13   rj      KW error fixes
06/28/13   ag      Support for EFS item to disable all SW mitigation
06/17/13   ag      Avoid memset of action table during power updates
06/13/13   rj      SVLTE/SVDO Coex for Triton/DI2.0
04/23/13   rj      Fix for sending pwr limits to GSM and WCDMA
04/06/13   ag      Support for DSDA WWAN Coexistence
02/20/13   rj      Fix CW issues
09/24/12   ag      Modified to handle single processor and msgr
04/22/12   ag      Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "mcs_variation.h"
#include "comdef.h"
#include "stringl.h"
#include "cm.h"
#include "sys.h"

#include "cxm.h"
#include "lmtsmgr_task.h"
#include "lmtsmgr_i.h"
#include "lmtsmgr_efs.h"
#include "lmtsmgr_msgr.h"

#include "sar_i.h"
#include "wwan_coex.h"
#include "batt_efs.h"
#include "subs_prio.h"
#include "wwcoex_conflict_table.h"
#include "wwcoex_inactivity.h"
#include "lmtsmgr_diag.h"
#include "wwcoex_action_iface.h"
#include "batt_npa.h"
#include "trm.h"
#include "wwcoex_ba.h"
#include "ddr_coex_i.h"
#include "lmtsmgr_translation.h"

/*=============================================================================

                            TYPEDEFS

=============================================================================*/

#ifdef FEATURE_BOLT_MODEM
  #define FEATURE_LMTSMGR_SAME_SUB_PRIO
#endif 

#define LMTSMGR_CHECK_DIFF_SUB(tech1, tech2) (((tech1)==CXM_TECH_GSM2) || ((tech2)==CXM_TECH_GSM2) || ((tech1)==CXM_TECH_GSM3) || ((tech2)==CXM_TECH_GSM3))

/* Get the other GSM tech */
#define GET_OTHER_GSM_TECH(tech) (((tech)==CXM_TECH_GSM1)?CXM_TECH_GSM2:CXM_TECH_GSM1)
/*=============================================================================

                         INTERNAL VARIABLES

=============================================================================*/

/* Global LMTSMGR state variables */
lmtsmgr_struct lmtsmgr;

/* Temp variable to hold the special ind mask value for each coex_input run */
static uint32 lmgr_temp_ind_mask = 0;

/* Temp result table for storing actions */
wwcoex_tbl_type temp_result_tbl;

/* Variables to store the current SUB and tech which are low priority */
sys_modem_as_id_e_type      lprio_data_sub = MAX_NUM_OF_SUBSCRIPTIONS;
cxm_tech_type               lprio_data_tech = CXM_TECH_MAX;
wwcoex_prio_table_upd_type  lprio_data_tbl_action = PRIO_TABLE_SET_MIN;

/*=============================================================================

                                FUNCTIONS

=============================================================================*/
sp_subs_states_enum_t lmtsmgr_get_macro_state
(
  cxm_tech_type     tech
);

#ifndef MCS_LIMITS_LOW_MEMORY

/*=======================================================================
FUNCTION LMTSMGR_IS_ON_SEPERATE_DEV
    
DESCRIPTION
  This function will determine if the two techs are on the same TRM device chain

DEPENDENCIES
  None
    
RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
static boolean lmtsmgr_is_on_seperate_dev(cxm_tech_type tech1, cxm_tech_type tech2)
{
  int i;
  int found_tech1 = -1;
  int found_tech2 = -1;
  for (i=0; i<TRM_MAX_CHAINS; i++)
  {
    if (lmtsmgr.chain_owner[i] == tech1)
    {
      found_tech1 = i;
    }
    else if (lmtsmgr.chain_owner[i] == tech2)
    {
      found_tech2 = i;
    }
  }

  return ((found_tech1 != -1) && (found_tech2 != -1) &&
          (found_tech1 != found_tech2));
}

#endif /* MCS_LIMITS_LOW_MEMORY */

/*=======================================================================
FUNCTION LMTSMGR_UPDATE_TECH_ASID_MAP
    
DESCRIPTION
  This function will Update tech_asid_map for GSM clients with ASID based 
  on multimode subscription. And it will also update the ASID for other 
  techs with multimode ASID

DEPENDENCIES
  None
    
RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/

static void lmtsmgr_update_tech_asid_map ( void )
{
  uint8 index = 0;

  /* Update all techs with multimode ASID */
  for (index = 0; index < (uint8)CXM_TECH_MAX; index++)
  {
    lmtsmgr.tech_state[index].sub_id = 
               lmtsmgr.subs_prio_list.tech_asid_map.multimode_as_id;
  }
  
  /* Correct the ASID for GSM techs */
  for (index = 0; index < MAX_NUM_OF_SUBSCRIPTIONS; index++)
  {
    switch (lmtsmgr.subs_prio_list.tech_asid_map.gsm_id_list[index])
    {
      case SP_GSM1:
        lmtsmgr.gsm_id_list[index] = CXM_TECH_GSM1;
        lmtsmgr.tech_state[CXM_TECH_GSM1].sub_id = (sys_modem_as_id_e_type)index;
        break;

      case SP_GSM2:
        lmtsmgr.gsm_id_list[index] = CXM_TECH_GSM2;
        lmtsmgr.tech_state[CXM_TECH_GSM2].sub_id = (sys_modem_as_id_e_type)index;
        break;

      case SP_GSM3:
        lmtsmgr.gsm_id_list[index] = CXM_TECH_GSM3;
        lmtsmgr.tech_state[CXM_TECH_GSM3].sub_id = (sys_modem_as_id_e_type)index;
        break;

      default:
          MSG_HIGH("lmtsmgr_update_tech_asid_map wrong num_subs %d ", index, 0, 0);
        break;
    }
  }
}

/*=======================================================================
FUNCTION lmtsmgr_get_tech_active_on_sub
    
DESCRIPTION
  This function will return the first tech with num_ul_entries > 0 for 
  a multimode SUB and return the right GSM tech for the other SUBs

DEPENDENCIES
  None
    
RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
cxm_tech_type lmtsmgr_get_tech_active_on_sub
(
  sys_modem_as_id_e_type         sub
)
{
  cxm_tech_type                  tech;
  sp_subscription_priority_list  sp_prio_list;

  sp_prio_list = lmtsmgr.subs_prio_list;

    /* Check for array overflow */
  if (sub >= MAX_NUM_OF_SUBSCRIPTIONS)
  {
      /* Should not happen... */
    LMTSMGR_MSG_1(FATAL, "Invalid SUB ID %d", sub);
    tech = CXM_TECH_MAX;
    //lprio_data_sub = MAX_NUM_OF_SUBSCRIPTIONS;
    return tech;
  }

    /* if data SUB is multimode, then need to find the tech */
  if (sub == sp_prio_list.tech_asid_map.multimode_as_id)
  {
      /* go through all clients to find the one on low priority SUB */
      for (tech=0; tech<CXM_TECH_MAX; tech++)
      {
        /* Only interested in techs with UL entries... if its GSM tech then
           it should match the entry in gsm_id_list for corresponding SUB */
        if (lmtsmgr.tech_state[tech].currList.num_ul_entries > 0)
        {
          /* If its GSM tech, also check if the id_list matches for the corr SUB*/
          if (IS_GSM_TECH(tech))
          {
          if (lmtsmgr.gsm_id_list[sub] != tech)
            {
            /* This is not the GSM tech on the SUB */
              continue;
            }
          }

          /* Found the tech */
          break;
        }
      }
  }
  else /* SUB is not multimode*/
  {
    /* We know the GSM techid using the SUB as index */
    tech = lmtsmgr.gsm_id_list[sub];
  }

  return tech;
}

/*=======================================================================
FUNCTION lmtsmgr_get_prio_criteria
    
DESCRIPTION
  This function will return which criteria is satisfied for changing the
  micro priorities.

DEPENDENCIES
  None
    
RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
lmtsmgr_prio_criteria_type lmtsmgr_get_prio_criteria ( void )
{
  sp_subscription_priority_list  sp_prio_list;
  lmtsmgr_prio_criteria_type     criteria = PRIO_CRITERIA_NONE;
  cxm_tech_type                  voice_tech;

  sp_prio_list = lmtsmgr.subs_prio_list;
  
  if (sp_prio_list.num <= 1)
  {
    criteria = PRIO_CRITERIA_NONE;
  }
  else if ( (sp_prio_list.order[0].state == SP_DATA_HIGH_ONLY_STATE) &&
            (sp_prio_list.order[1].state == SP_DATA_ONLY_STATE) &&
            (lmtsmgr.device_mode == SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE)) 
  {
    /* we are in data + data High DSDA combination */
    criteria = PRIO_CRITERIA_DATA_DATA_HIGH;
    LMTSMGR_MSG_1(HIGH, "Macro priority criteria %d is hit", criteria);
  }
  else if ( (sp_prio_list.order[0].state == SP_VOICE_HIGH_PRIORITY_STATE) &&
            (sp_prio_list.order[1].state == SP_DATA_ONLY_STATE) ) 
  {
    voice_tech = lmtsmgr_get_tech_active_on_sub(sp_prio_list.order[0].sub);
    
    if((voice_tech == CXM_TECH_DFLT_INVLD) ||
       (voice_tech == CXM_TECH_MAX))
    {
      /* Invalid Tech */
      LMTSMGR_MSG_1(FATAL, "Could not find tech for voice high SUB %d",
                    sp_prio_list.order[0].sub);      
      return criteria;
    }  
    
    if (lmtsmgr.tech_state[voice_tech].distress_state == DISTRESS_HIGH)
    {
      /* we are in voice + data combination */
      criteria = PRIO_CRITERIA_VOICE_DISTRESS_DATA;
      LMTSMGR_MSG_1(HIGH, "Macro priority criteria %d is hit", criteria);
    }
  }
  else if ( (sp_prio_list.order[0].state == SP_VOICE_HIGH_PRIORITY_STATE) &&
            (sp_prio_list.order[1].state == SP_DATA_HIGH_ONLY_STATE) )
  {
    voice_tech = lmtsmgr_get_tech_active_on_sub(sp_prio_list.order[0].sub);
    
    if((voice_tech == CXM_TECH_DFLT_INVLD) ||
       (voice_tech == CXM_TECH_MAX))
    {
      /* Invalid Tech */
      LMTSMGR_MSG_1(FATAL, "Could not find tech for voice high SUB %d",
                    sp_prio_list.order[0].sub);      
      return criteria;
    }  
    
    if (lmtsmgr.tech_state[voice_tech].distress_state == DISTRESS_HIGH)
    {
      /* we are in voice + data High combination */
      criteria = PRIO_CRITERIA_VOICE_DISTRESS_DATA_HIGH;
      LMTSMGR_MSG_1(HIGH, "Macro priority criteria %d is hit", criteria);
    }
  }
  else
  {
    criteria = PRIO_CRITERIA_NONE;
  }

  return criteria;
}

/*=======================================================================
FUNCTION LMTSMGR_GET_DATA_TBL_ACTION
    
DESCRIPTION
  This function will return the prio table action that needs to be applied
  to the data tech. For G+G, there is a special action that needs to be 
  taken.

DEPENDENCIES
  This api should be called only when the criteria to rail the priorities
  is met.. for example when data + mms is hit.
    
RETURN VALUE
  Priority table action to be taken.

SIDE EFFECTS
  None
========================================================================*/
wwcoex_prio_table_upd_type lmtsmgr_get_data_tbl_action 
(
  cxm_tech_type  lprio_tech 
)
{
  cxm_tech_type hprio_tech;
  
  /* init to the default efs based global action */
  wwcoex_prio_table_upd_type action = lprio_data_tbl_action;
  
  /* If its a G+G scenario, then change it... */
  if (IS_GSM_TECH(lprio_tech))
  {
    hprio_tech = GET_OTHER_GSM_TECH(lprio_tech);
    if (lmtsmgr.tech_state[hprio_tech].currList.num_ul_entries > 0)
    {
      action = PRIO_TABLE_SET_MIN_EXCLUDE_SUPER_CRIT;
    }
  }
    
  return action;
}

/*=======================================================================
FUNCTION LMTSMGR_UPDATE_CLIENT_PRIO_TABLE
    
DESCRIPTION
  This function will update client micro prio table of low macro clients 
  when any criteria is met. If criteria is not met, it will reset the prios
  to orginial state.

DEPENDENCIES
  None
    
RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/

void lmtsmgr_update_client_prio_table ( void )
{
  sp_subscription_priority_list  sp_prio_list;
  uint8                          index = 0;
  sys_modem_as_id_e_type         sub;
  cxm_tech_type                  lprio_tech;
  lmtsmgr_prio_criteria_type     criteria;
  wwcoex_prio_table_upd_type     data_tbl_action;

  sp_prio_list = lmtsmgr.subs_prio_list;
  /* Check what is the criteria... */
  criteria = lmtsmgr_get_prio_criteria();
  if ( criteria != PRIO_CRITERIA_NONE )
  {
    lprio_tech = lmtsmgr_get_tech_active_on_sub(sp_prio_list.order[1].sub);
    if (lprio_tech >= CXM_TECH_MAX)
    {
      LMTSMGR_MSG_1(FATAL, "Could not find UL tech for data SUB %d", 
                    sp_prio_list.order[1].sub);
      return;
    }

    /* If lprio tech has changed, only then update the prio table */
    if ( lprio_tech != lprio_data_tech )
    {
      if (lprio_data_tech != CXM_TECH_MAX)
    {
        /* If the previous low priority tech was valid, then first need to reset 
           its micro priorities */
        cxm_update_prio_table(lprio_data_tech, 1, 0, PRIO_TABLE_RESET_PREV);
      }

      /* Get which action needs to be applied */
      data_tbl_action = lmtsmgr_get_data_tbl_action(lprio_tech);

      /* Set the new lprio tech to MIN and save the state */
      cxm_update_prio_table(lprio_tech, 1, 0, data_tbl_action);
      lprio_data_tech = lprio_tech;
      lprio_data_sub = sp_prio_list.order[1].sub;
    }
  }
  /* if there is a lprio_data_tech and the criteria is not met, reset the prio 
     if lprio_data_tech is GSM, then instead of reset, set the prio acco to macro prio */
  else if ( (lprio_data_tech < CXM_TECH_MAX) && (!IS_GSM_TECH(lprio_data_tech)) )
  {
    cxm_update_prio_table(lprio_data_tech, 1, 0, PRIO_TABLE_RESET_PREV);

    /* Reset the lprio_data fields */
    lprio_data_tech = CXM_TECH_MAX;
    lprio_data_sub = MAX_NUM_OF_SUBSCRIPTIONS;
  }
  else /* Criteria is not met and the prio has been reset to original...
          just need to ensure GSM stack prios are updated as per macro prio */
  {
    /* GSM1 will always be mapped to multimode sub */
    /* GSM2/GSM3 will always be mapped to non-multimode subscription from gsm_id_list */
    lmtsmgr.curr_prio_offset++;
    lmtsmgr.curr_prio_offset %= LMTSMGR_NUM_GSM_PRIO_SETS;
    for (index = 0; index < sp_prio_list.num; index++)
    {
      sub = sp_prio_list.order[index].sub;
      if ((sub < (sys_modem_as_id_e_type)0) || (sub >= MAX_NUM_OF_SUBSCRIPTIONS))
      {
        LMTSMGR_MSG_2(FATAL,"Incorrect Sub# %d received on index %d",
                             sub, index);
        continue;
      }

      cxm_update_prio_table(lmtsmgr.gsm_id_list[sub], index, 
                            lmtsmgr.curr_prio_offset, PRIO_TABLE_MACRO_OFFSET_BASED);

      lprio_data_tech = CXM_TECH_MAX;
      lprio_data_sub = MAX_NUM_OF_SUBSCRIPTIONS;
    }
  }
}


/*=======================================================================
FUNCTION LMTSGR_UPD_PRIO_TBL_ON_TX_STATE
    
DESCRIPTION
  This function will update client prio table when there is a change in
  Tx state. This is required to take care of IRAT handovers during an 
  active data call

DEPENDENCIES
  None
    
RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
void lmtsmgr_upd_prio_tbl_on_tx_state
(
  cxm_tech_type   tech,
  boolean         is_tx_on
)
{
  sys_modem_as_id_e_type         sub;
  wwcoex_prio_table_upd_type data_tbl_action;

  /* No need to update prio table if there is no low prio data sub active */
  if (lprio_data_sub == MAX_NUM_OF_SUBSCRIPTIONS)
  {
    return;
  }

  if (is_tx_on == TRUE)
  {
    /* Tx is turning ON... check if its on the same sub as lprio data */
    sub = lmtsmgr.tech_state[tech].sub_id;
    if ( (sub == lprio_data_sub) && (lprio_data_tech == CXM_TECH_MAX) )
    {
      lprio_data_tech = tech;
      data_tbl_action = lmtsmgr_get_data_tbl_action(lprio_data_tech);
      cxm_update_prio_table(lprio_data_tech, 1, 0, data_tbl_action );
    }
  }
  /* If Tx is turning OFF, check if it is the lprio tech is the one turning OFF*/
  else if (lprio_data_tech == tech)
  {
    /* Reset the table to original */
    cxm_update_prio_table(lprio_data_tech, 1, 0, PRIO_TABLE_RESET_PREV);

    /* Reset the lprio_tech field... keep the lprio_sub field as is since we can have 
       a RAT handover */
    lprio_data_tech = CXM_TECH_MAX;
  }

  return;
}

/*=======================================================================
FUNCTION LMTSGR_SUBS_PRIO_CALLBACK
    
DESCRIPTION
  This is the callback function registered with subs prio. It merely 
  copies the subs_prio info to limitsmgr struct and sets a signal
  to the task.

DEPENDENCIES
  None
    
RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
void lmtsmgr_subs_prio_callback(sp_subscription_priority_list  subs_prio_list, 
                                                   sp_client_enum_t client_id)
{
  if (client_id < SP_MAX_CLIENT)
  {
    /* Enter Critical Section */
    LMTSMGR_ENTER_CRIT_SECT(lmtsmgr.crit_sect);

    lmtsmgr.subs_prio_list = subs_prio_list;

    lmtsmgr_set_sigs(LMTSMGR_SAR_CALL_BACK_SIG);

    /* Leave Critical Section */
    LMTSMGR_LEAVE_CRIT_SECT(lmtsmgr.crit_sect);
  }
  else
  {
    LMTSMGR_MSG_1(ERROR, "lmtsmgr_subs_prio_callback: client_id (%d) not valid ", client_id);
  }

}

/*=======================================================================
FUNCTION LMTSGR_SP_CALLBACK_PROCESS_INPUT
    
DESCRIPTION
  This is a signal handler for subs prio callback signal. This function
  goes over the changes in the subs prio and takes appropriate actions 
  like changing micro priorities, updating sar limit, broadcasting tech
  macro state, etc.

DEPENDENCIES
  None
    
RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
void lmtsmgr_sp_callback_process_input(void)
{
  uint8 index = 0;
  cxm_tech_type tech;

  LMTSMGR_MSG_2(HIGH, "Subs-prio->SAR: num: %d, activeMask: %d", 
                       lmtsmgr.subs_prio_list.num, 
                       lmtsmgr.subs_prio_list.active_tech_info.active_subscription_mask);
                       
  for (index = 0; index < lmtsmgr.subs_prio_list.num; index++)
  {
    LMTSMGR_DBG_MSG_3(HIGH, "Subs-prio->SAR: sub: %d, techMask[%d] %d", 
                       lmtsmgr.subs_prio_list.order[index].sub, index, 
                       lmtsmgr.subs_prio_list.active_tech_info.tech_supported_bitmask[index]);
  }

  /* Update Tech ASID mapping from Subs-Prio client to CXM clients */
  lmtsmgr_update_tech_asid_map();

  /* Update client priority tables of registered GSM clients */
  lmtsmgr_update_client_prio_table();
  
  /* Update Macro priority state of Tx techs */
  for (tech = 0; tech<CXM_TECH_MAX; tech++)
  {
    /* Only update Tx active techs... for idle techs, the prio will 
       be reset to baseline at Tx off freq indication */
    if (lmtsmgr.tech_state[tech].currList.num_ul_entries > 0)
    {
      lmtsmgr.tech_state[tech].macro_state = lmtsmgr_get_macro_state(tech);
    }
  }
  
  /* Process Subs-Prio CallBack function inputs */
  lmtsmgr_process_sar_input();

  /* Update tech state coex indication */
  lmtsmgr_process_coex_tech_state_change();
}

#ifndef MCS_LIMITS_LOW_MEMORY

/*=======================================================================
FUNCTION LMTSMGR_SET_TECH_BACKOFF_LIMITS_MAX
    
DESCRIPTION
  This function will set MAX limits for the Backoff for a tech

DEPENDENCIES
  None
    
RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/

static void lmtsmgr_set_tech_backoff_limits_max(cxm_tech_type tech)
{
  uint8 index = 0;
  lmtsmgr_power_info_type *pwr_info_t1;

  pwr_info_t1 = &lmtsmgr.tech_state[tech].pwr_info;

  sar_core_max_pwr_backoff(pwr_info_t1->new_limits.sar_plimit,
                            &pwr_info_t1->new_limits.num_valid_sar, tech);
  for (index=0; index<CXM_TECH_MAX; index++)
  {
    pwr_info_t1->new_limits.coex_plimit[index] = DEFAULT_MAX_PLIMIT;
    pwr_info_t1->new_limits.coex_slot_plimit[index] = DEFAULT_MAX_PLIMIT;
  }
  pwr_info_t1->new_limits.batt_plimit = DEFAULT_MAX_PLIMIT;
  lmtsmgr_notify_tech_plimit(tech);
}

#endif /* MCS_LIMITS_LOW_MEMORY */

void lmtsmgr_trm_state_update_callback 
(
  trm_state_info_type trm_state_info
)
{
  cxm_chain_owner_msg_s chain_own_msg;
  int i;
  
  /* Check input parameters */
  if (trm_state_info.client < TRM_1X && 
      trm_state_info.client > TRM_LAST_CLIENT)
  {
    return;
  }

  /* If the chain owner has changed, send a msgr message to Limitsmgr task */
  if (trm_state_info.chain_owner_changed == TRUE)
  {
    /* Remap TRM client to CXM client */
    for (i=0; i<TRM_MAX_CHAINS; i++)
    {
      chain_own_msg.owner.chain[i] = lmtsmgr_trans_trm_to_cxm_client(
                                               trm_state_info.chain_owner[i] );
    }

    lmtsmgr_msgr_send_msg(&chain_own_msg.msg_hdr, MCS_CXM_CHAIN_OWNER_UPDATE_IND, sizeof(chain_own_msg));
  }

  /* Call DDR coex function to handle TRM tech_state indication */
  ddr_coex_trm_tech_state_update_hdlr(trm_state_info);

}

#ifndef MCS_LIMITS_LOW_MEMORY

static boolean lmtsmgr_validate_hopping_para_changed 
(
  trm_modify_feature_data_input_info* hop
)
{
  if ((hop != NULL) && ((hop->data.new_hop_info.disable_hop != lmtsmgr.hop_input.disable_hop) ||
      (hop->data.new_hop_info.hop_type != lmtsmgr.hop_input.hop_type)))
  {
    return TRUE;
  }

  return FALSE;
}

#endif /* MCS_LIMITS_LOW_MEMORY */

/*=============================================================================

  FUNCTION:  lmtsmgr_update_hopping_behavior

=============================================================================*/
/*!
    @brief
    Checks Techs behavior and Calls TRM API to update 
    whether Idle Mode Hopping is to be enabled
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_update_hopping_behavior (void)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  uint8 index, dlindex;
  /* By Default always disable Hopping */
  trm_modify_feature_data_input_info temp;
  lmtsmgr_tech_state_type *gsm_st;
  uint32 gsm_num_dlfreqs;
  cxm_tech_type tech1;

  TTLMARKER(LMTSMGR_UPD_HOP_IN);
  
  temp.client_id = TRM_CM;
  temp.feature = TRM_FEATURE_HOPPING;
  temp.data.new_hop_info.disable_hop = FALSE;
  temp.data.new_hop_info.hop_type = TRM_HOPPING_TYPE_IDLE_MODE;

  /* Whether LTE is in Data Call and SVLTE is active */
  if (LMTSMGR_IS_TECH_UL_ACTIVE(CXM_TECH_LTE) && LMTSMGR_IS_SVLTE_FEATURE_MODE())
  {
    for (tech1 = (CXM_TECH_MAX-1); tech1 >= 0; tech1--)
    {
      /* Check it's GSM tech and GSM in IDLE mode */
      if (IS_GSM_TECH(tech1) &&
          (!LMTSMGR_IS_TECH_UL_ACTIVE(tech1) && LMTSMGR_IS_TECH_ACTIVE(tech1)))
      {
        gsm_st = &lmtsmgr.tech_state[tech1];
        gsm_num_dlfreqs = gsm_st->currList.num_dl_entries;
        for (index = 0; index < gsm_num_dlfreqs; index++)
        {
          dlindex = gsm_st->currList.dl_freqList[index];
          LMTSMGR_MSG_3(HIGH, "lmtsmgr_update_hopping_behavior: pwr lmt %d pwr %d, type %d", 
                   lmtsmgr.idle_mode_hopping_rxpwr_lmt, gsm_st->currList.links[dlindex].powerInfo.rx_power,
                   gsm_st->currList.links[dlindex].freqInfo.link_info.type);
          /* Determine if dl entry is NOT diveristy/PM  AND Rx Power is < -85 dBm (or EFS updated)  AND
                       Rx Power should only be applicable if There is Power update from GSM, else we will ignore Hopping check */
          if ((gsm_st->currList.links[dlindex].freqInfo.link_info.type != CXM_LNK_TYPE_DIVERSITY) && 
              (gsm_st->currList.links[dlindex].freqInfo.link_info.type != CXM_LNK_TYPE_POWER_MONITOR) &&
              (gsm_st->currList.links[dlindex].powerInfo.rx_power != CXM_UNKNOWN_POWER) &&
              (gsm_st->currList.links[dlindex].powerInfo.rx_power < lmtsmgr.idle_mode_hopping_rxpwr_lmt))
          {
            /* Disable Hopping */
            temp.data.new_hop_info.disable_hop = TRUE;
            temp.data.new_hop_info.hop_type = TRM_HOPPING_TYPE_IDLE_MODE;
            break;
          }
        }
        if (temp.data.new_hop_info.disable_hop) // Break from loop once we Found GSM in Idle mode hopping scenario 
        break;
      }
    }
  }

  if (lmtsmgr_validate_hopping_para_changed(&temp))
  {
    LMTSMGR_MSG_1(HIGH, "lmtsmgr_update_hopping_behavior: disable_hop %d", temp.data.new_hop_info.disable_hop);
    trm_modify_feature_info(&temp);
    lmtsmgr.hop_input.disable_hop = temp.data.new_hop_info.disable_hop;
    lmtsmgr.hop_input.hop_type = temp.data.new_hop_info.hop_type;
  }
  
  TTLMARKER(LMTSMGR_UPD_HOP_OUT);
  
#endif /* MCS_LIMITS_LOW_MEMORY */

}


/*=============================================================================

  FUNCTION:  lmtsmgr_core_get_sub_based_priority

=============================================================================*/
/*!
    @brief
    Returns priority based on the SUB priority order list from subs_prio module
    This shall be used when the 2 concurrent active techs are on different SUBs
 
    @return
    Priority tier
*/
/*===========================================================================*/
lmtsmgr_priority_type lmtsmgr_core_get_sub_based_priority
(
  cxm_tech_type tech
)
{
  sp_subscription_priority_list  sp_prio_list;
  sp_active_subscription_info*   active;
  uint8                          index = 0;
  lmtsmgr_priority_type          tier_ret = LMTSMGR_TIER_MAX;
  sys_modem_as_id_e_type         sub;

  /* Enter Critical Section */
  LMTSMGR_ENTER_CRIT_SECT(lmtsmgr.crit_sect);

  sp_prio_list = lmtsmgr.subs_prio_list;
  active = &sp_prio_list.active_tech_info;

  LMTSMGR_DBG_MSG_1(LOW, "sp_prio_list.num: %d", sp_prio_list.num);

    switch (tech)
    {
      case CXM_TECH_ONEX:
      case CXM_TECH_WCDMA:
      case CXM_TECH_LTE:
      case CXM_TECH_TDSCDMA:
      case CXM_TECH_HDR:
        /* Check if it is NOT GSM only sub as other techs should be mapped to multimode subscription */
        for (index = 0; index < sp_prio_list.num; index++)
        {
          sub = sp_prio_list.order[index].sub;
          if ((sub < (sys_modem_as_id_e_type)0) || (sub >= MAX_AS_IDS))
          {
             LMTSMGR_MSG_2(FATAL,"Incorrect Sub# %d received on index %d",
                           sub, index);
             continue;
          }

          if ((sub == sp_prio_list.tech_asid_map.multimode_as_id) && 
               (SP_CONV_ENUM_TO_BIT_MASK(lmtsmgr_trans_cxm_to_sys_id(tech)) &
                                   active->tech_supported_bitmask[sub]))
          {
            tier_ret = (lmtsmgr_priority_type)index;
      
            LMTSMGR_MSG_2(LOW, "get_priority: tech: %d, tier_ret: %d ", tech, tier_ret);
            break;
          }
        }
        break;
      
      case CXM_TECH_GSM1:
      case CXM_TECH_GSM2:
    #ifdef FEATURE_TRIPLE_SIM
      case CXM_TECH_GSM3:
    #endif
        /* GSM1 will always be mapped to multimode sub */
        /* GSM2/GSM3 will always be mapped to non-multimode subscription from gsm_id_list */
        for (index = 0; index < sp_prio_list.num; index++)
        {
          sub = sp_prio_list.order[index].sub;
          if ((sub < (sys_modem_as_id_e_type)0) || (sub >= MAX_AS_IDS))
          {
             LMTSMGR_MSG_2(FATAL,"Incorrect Sub# %d received on index %d",
                           sub, index);
             continue;
          }
          
          if (lmtsmgr.gsm_id_list[sub] == tech)
          {
            tier_ret = (lmtsmgr_priority_type)index;
            if (LMTSMGR_TIER_MAX == tier_ret)
            {
              tier_ret = LMTSMGR_TIER_2;
            }
            break;
          }
        }
        break;

      default:
        LMTSMGR_MSG_1(ERROR, "Tech id %d not valid", tech);
    }

  /* Leave Critical Section */
  LMTSMGR_LEAVE_CRIT_SECT(lmtsmgr.crit_sect);

  return tier_ret;
  }


#ifdef FEATURE_LMTSMGR_SAME_SUB_PRIO
/*=============================================================================

  FUNCTION:  lmtsmgr_core_get_same_sub_priority

=============================================================================*/
/*!
    @brief
    Returns a fixed priority for a tech on the basis that it is on the same
    SUB as the other concurrent active tech
 
    @return
    Priority tier
*/
/*===========================================================================*/
lmtsmgr_priority_type lmtsmgr_core_get_same_sub_priority
(
  cxm_tech_type tech
)
  {
  lmtsmgr_priority_type tier_ret;

    /* Determine priority for this transmitter 
       For now, priority is based on only tech. So GSM > LTE and GSM > TDSCDMA.
       But in the future we might have to look at the entire tech state or 
       extract priority from upper layers or APSS.
    */
    switch (tech)
    {
      case CXM_TECH_ONEX:
      case CXM_TECH_GSM1:
      case CXM_TECH_GSM2:
      case CXM_TECH_GSM3:
       tier_ret = LMTSMGR_TIER_1;
       break;
    
      case CXM_TECH_LTE:
      case CXM_TECH_TDSCDMA:
      case CXM_TECH_HDR:
       tier_ret = LMTSMGR_TIER_2;
       break;
      
      case CXM_TECH_WCDMA:
      default:
       tier_ret = LMTSMGR_TIER_MAX;
       break;
    }

  return tier_ret;
  }
 #endif

/*=============================================================================
  
  FUNCTION:  lmtsmgr_core_get_priority

=============================================================================*/
/*!
    @brief
    Updates the priorities for the two simultaneously active techs
 
    @return
    None
*/
/*===========================================================================*/
boolean lmtsmgr_core_get_priority 
(
  lmtsmgr_core_prio_table_type *tech1_prio,
  lmtsmgr_core_prio_table_type *tech2_prio
)
{
  sp_subs_states_enum_t tech1_macro_state, tech2_macro_state;  

  if ( (tech1_prio == NULL) || (tech1_prio == NULL) ||
     (!LMTSMGR_IS_TECH_VALID(tech1_prio->tech)) || (!LMTSMGR_IS_TECH_VALID(tech2_prio->tech)))
  {
    LMTSMGR_MSG_2(FATAL, "Incorrect args: ptr1 0x%x, ptr2 0x%x", 
                  tech1_prio, tech1_prio);
    return FALSE;
  }

  /* Are the 2 techs on diff SUB ? */
  if (LMTSMGR_CHECK_DIFF_SUB(tech1_prio->tech, tech2_prio->tech))
  { 
    tech1_prio->pri = lmtsmgr_core_get_sub_based_priority(tech1_prio->tech);
    tech2_prio->pri = lmtsmgr_core_get_sub_based_priority(tech2_prio->tech);
  }
  else
  {
    /* Get the macro state of both techs */
    tech1_macro_state = lmtsmgr.tech_state[tech1_prio->tech].macro_state;    
    tech2_macro_state = lmtsmgr.tech_state[tech2_prio->tech].macro_state;
                                                  
    if (tech1_macro_state > tech2_macro_state)
    {
      tech1_prio->pri = LMTSMGR_TIER_1;
      tech2_prio->pri = LMTSMGR_TIER_2;
    }
    else if (tech1_macro_state < tech2_macro_state)
    {
      tech1_prio->pri = LMTSMGR_TIER_2;
      tech2_prio->pri = LMTSMGR_TIER_1;
    }
    else
    {     
      /* Assuming tech1 is higher prio... as generally tech1 would be GSM*/      
      tech1_prio->pri = LMTSMGR_TIER_1;
      tech2_prio->pri = LMTSMGR_TIER_2;
    }
  }
  
  return TRUE;
}

/*=============================================================================

  FUNCTION:  wwcoex_check_special_inds

=============================================================================*/
/*!
    @brief
    Checks if any special indications need to be sent
 
    @return
    None
*/
/*===========================================================================*/
void wwcoex_check_special_inds(void)
{
  lmtsmgr_filter_path_type send_ind;

  send_ind.filtered_path_flag = FALSE;
  send_ind.gsm_aggr_filter_path_flag = FALSE;

  /* Has anything changed ? */
  if (lmtsmgr.special_ind_mask != lmgr_temp_ind_mask)
  {
    /* Save it as the current final state */
    lmtsmgr.special_ind_mask = lmgr_temp_ind_mask;

    /* Has the filter path flag set ? */
    if (lmtsmgr.special_ind_mask & (1<<FILTERED_PATH))
    {
      send_ind.filtered_path_flag = TRUE;
    }
    if (lmtsmgr.special_ind_mask & (1<<FILTERED_PATH_GSM_TX_ONLY))
    {
      send_ind.gsm_aggr_filter_path_flag = TRUE;
    }
    lmtsmgr_send_filter_path_ind(send_ind);
  }
}

/*=============================================================================

  FUNCTION:  lmtsmgr_process_txstate_change

=============================================================================*/
/*!
    @brief
    Processes any change in the Tx on/off state for a tech
 
    @return
    None
*/
/*===========================================================================*/

void lmtsmgr_process_txstate_change
(
  cxm_tech_type   tech_id,
  uint32          prev_num_ul,
  uint32          new_num_ul
)
{
  int i;

  TTLMARKER(LMTSMGR_PROC_TXST_CHG_IN);
  
  /* If the tech is invalid, return */
  if (tech_id >= CXM_TECH_MAX)
  {
    TTLMARKER(LMTSMGR_PROC_TXST_CHG_ERR1);
    return;
  }

  /* Has the state changed ? */
  if ( ((prev_num_ul == 0) && (new_num_ul == 0)) ||
       ((prev_num_ul > 0) && (new_num_ul > 0)) )
  {
    TTLMARKER(LMTSMGR_PROC_TXST_CHG_NOCHG);
    return;
  }

  if (new_num_ul > 0)
  {
    // Tech just turning TX ON
    LMTSMGR_MSG_1(HIGH, "Received Tech %d Tx On", tech_id); 

    /* Reset backoff limits to MAX */
    sar_core_max_pwr_backoff(lmtsmgr.tech_state[tech_id].pwr_info.new_limits.sar_plimit,
                            &lmtsmgr.tech_state[tech_id].pwr_info.new_limits.num_valid_sar, 
                            tech_id);
    lmtsmgr.tech_state[tech_id].pwr_info.new_limits.batt_plimit=DEFAULT_MAX_PLIMIT;
    for (i=0; i < CXM_TECH_MAX; i++)
    {
      lmtsmgr.tech_state[tech_id].pwr_info.new_limits.coex_plimit[i]=DEFAULT_MAX_PLIMIT;
      lmtsmgr.tech_state[tech_id].pwr_info.new_limits.coex_slot_plimit[i]=DEFAULT_MAX_PLIMIT;
    }


    if (!IS_WWAN_MITIGATION_DISABLED(lmtsmgr.mitigation_mask))
    {
      /* Request inactivity timeline registration */
      /* If tech has already registered, no need to send a request again */
      lmtsmgr_send_activity_info_req(tech_id, CXM_LNK_DRCTN_UL, TRUE);
    }

    /* Update macro state */
    lmtsmgr.tech_state[tech_id].macro_state = lmtsmgr_get_macro_state(tech_id);

    /* update any prio table if required... */
    lmtsmgr_upd_prio_tbl_on_tx_state(tech_id, TRUE);
  }
  else
  {
    LMTSMGR_MSG_1(HIGH, "Received Tech %d Tx Off", tech_id); 

    /* Reset the state */
    lmtsmgr.tech_state[tech_id].pwr_info.tx_pwr_valid = FALSE;
       
    if (!IS_WWAN_MITIGATION_DISABLED(lmtsmgr.mitigation_mask))
    {
#ifdef FEATURE_MODEM_COEXISTENCE_FW

      /* Deregister inactivity timer for this tech */
      wwcoex_deregister_activity(tech_id);
#endif

      /* Request inactivity timeline deregistration */
      lmtsmgr_send_activity_info_req(tech_id, CXM_LNK_DRCTN_UL, FALSE);
    }

    /* Update macro state */
    lmtsmgr.tech_state[tech_id].macro_state = SP_BASELINE_STATE;
	
    /* update any prio table if required... */
    lmtsmgr_upd_prio_tbl_on_tx_state(tech_id, FALSE);
  }

  /* Notify limits to tech */
  lmtsmgr_notify_tech_plimit( tech_id );
  
  TTLMARKER(LMTSMGR_PROC_TXST_CHG_OUT);
}

/*=============================================================================

  FUNCTION:  lmtsmgr_coex_init

=============================================================================*/
/*!
    @brief
    Initialize coex variables
 
    @return
    None
*/
/*===========================================================================*/
static void lmtsmgr_coex_init(void)
{
  uint32 i;
  
  /* Check if the two sizes are same */
  LMTSMGR_COMPILE_ASSERT(CXM_MAX_SUPPORTED_LINK_SETS == WWCOEX_MAX_SUPPORTED_LINKS);
  LMTSMGR_COMPILE_ASSERT(CXM_UNKNOWN_FREQID == WWCOEX_UNKNOWN_FREQID);

  /* Check if sim structures match local structures */
  LMTSMGR_COMPILE_ASSERT((sizeof(cxm_sim_tech_type) == sizeof(cxm_tech_type)));
  LMTSMGR_COMPILE_ASSERT((sizeof(cxm_sim_tech_link_direction) == sizeof(cxm_tech_link_direction)));
  LMTSMGR_COMPILE_ASSERT((sizeof(cxm_sim_slot_prio_e) == sizeof(cxm_tech_link_direction)));
  LMTSMGR_COMPILE_ASSERT((sizeof(cxm_sim_dtx_state_info) == sizeof(cxm_dtx_state_info)));
  LMTSMGR_COMPILE_ASSERT((sizeof(wwcoex_sim_gsm_call_type) == sizeof(wwcoex_gsm_call_type)));
  LMTSMGR_COMPILE_ASSERT((sizeof(cxm_sim_gsm_metric_info_s) == sizeof(cxm_gsm_metric_info_s)));
  LMTSMGR_COMPILE_ASSERT((sizeof(cxm_sim_tech_specific_metrics_u) == sizeof(cxm_tech_specific_metrics_u)));
  LMTSMGR_COMPILE_ASSERT(SET_SIM_SLOT_ACT_TL_ID == SET_SLOT_ACT_TL_ID_UNUSED);

  /* cxm_sim_timing_info_s and cxm_sim_timing_slot_entry are not compared
     as size is different due to Freqid (sim vs value) */

  /* Set the freqId range for each tech... */
  for (i=0; i<CXM_TECH_MAX; i++)
  {
    lmtsmgr.tech_state[i].freqId_range_start = i * LMTSMGR_MAX_FREQIDS_PER_TECH + 1;
    lmtsmgr.tech_state[i].freqId_range_end = 
       lmtsmgr.tech_state[i].freqId_range_start + LMTSMGR_MAX_FREQIDS_PER_TECH;

    /* Set macro state to baseline */
    lmtsmgr.tech_state[i].macro_state = SP_BASELINE_STATE;
  }

  /*! Initially assume we are in DSDA mode */
  wwcoex_init_tables(2,2);
  
  /*! Set Single-Tx DSDA indication as unknown */
  lmtsmgr.trm_single_tx_dsda = TRM_SINGLE_TX_DSDA_MAX;
  
  /* Read the mitigation mask */
  lmtsmgr.mitigation_mask = wwcoex_mitigation_mask_efs_lookup();
  lmtsmgr.en_mitigation_mask = wwcoex_mitigation_en_mask_efs_lookup();
  wwcoex_set_spur_mitigation_mask(IS_SPUR_MITIGATION_ENABLED(lmtsmgr.en_mitigation_mask));

  /* Read the Idle Mode Hopping Rx Power Limit */
  lmtsmgr.idle_mode_hopping_rxpwr_lmt = wwcoex_idle_mode_rxpwr_efs_lookup();

  LMTSMGR_MSG_2(HIGH, "Mitigation mask from EFS (disabled/enabled) is 0x%x 0x%x",
                lmtsmgr.mitigation_mask,
                lmtsmgr.en_mitigation_mask);

  wwcoex_init_inactivity_st();

  wwcoex_init_desense_stat();
  
}

/*=============================================================================

  FUNCTION:  lmtsmgr_init

=============================================================================*/
/*!
    @brief
    Initializes LMTSMGR and its global variables
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_init(void)
{
  cxm_tech_type i;
  uint32 efs_content;

  lmtsmgr_efs_init();

  wwan_coex_list_init();

  wwan_coex_spur_list_init();

  /* Reset the global struct */
  memset((void *)&lmtsmgr, 0, sizeof(lmtsmgr_struct));

  /* Creating a critial section *IS* a critical section */
  LMTSMGR_ENTER_ATOMIC_SECT();
    
  /* Initialize the Subs-Prio's critical section */
  LMTSMGR_INIT_CRIT_SECT(lmtsmgr.crit_sect);
  
  /* We have a critical section which can be used instead of INTLOCK'ing */
  LMTSMGR_LEAVE_ATOMIC_SECT();

  /* Reset the power limits for all technologies to TXSTOP */
  for (i=0; i < CXM_TECH_MAX; i++)
  {
    lmtsmgr_notify_tech_plimit(i);
  }

  /* Initialize the coex variables... */
  lmtsmgr_coex_init();

  /* Initialize the DSI service */
  sar_dsi_service_init();

  /*Register with Subscription Priority for priority list */
  sp_register_client(lmtsmgr_subs_prio_callback, SP_SAR);

  /* Initialize batt npa */
  batt_npa_init();

  /* Initialize DDR Coex */
  ddr_coex_init();

  /* Update CXM Prio Table */
  cxm_update_prio_table_init();
  
  /* Store the action for prio table update from efs file */
  efs_content = wwan_coex_read_prio_update_efs();
  if (efs_content > 0)
  {
    lprio_data_tbl_action = PRIO_TABLE_SET_MIN_EXCLUDE_SUPER_CRIT;
  }
  else
  {
    lprio_data_tbl_action = PRIO_TABLE_SET_MIN;
  }
}

#ifndef MCS_LIMITS_LOW_MEMORY

#ifdef FEATURE_MODEM_COEXISTENCE_SW

/*=============================================================================

  FUNCTION:  wwcoex_check_coex_with_dl

=============================================================================*/
/*!
    @brief
    Check a victim and/or aggressor UL frequency with all the victim DL
    frequencies. Also lookup into the efs tables and store the final action
    in the result table.
 
    @return
    TRUE
*/
/*===========================================================================*/
boolean wwcoex_check_coex_with_dl
(
  cxm_tech_type  tech1,
  cxm_tech_type  tech2,
  cxm_tech_type  vtech,
  cxm_tech_type  vtulindex,
  cxm_tech_type  atulindex,
  boolean        skip_im,
  wwcoex_tbl_type*  result_tbl,
  wwan_desense_efs_lookup_out* coex_output
)
{
  uint32 vtdlindex, tech1_index, tech2_index;
  lmtsmgr_tech_state_type* vt_st, *at_st;
  lmtsmgr_tech_link_type vtdl, atul, *vtul = NULL;
  cxm_tech_type atech;
  cxm_action_type action;
  lmtsmgr_core_prio_table_type tech1_prio_tbl,tech2_prio_tbl;

  /* Non-IM Freq compare argument structures */
  wwcoex_nonim_freqcompare_in  nonim_comp_in;
  wwcoex_nonim_freqcompare_out  nonim_comp_out;

   /* IM Freq compare argument structures */
  wwcoex_im_freqcompare_in      im_comp_in;
  wwcoex_im_freqcompare_out     im_comp_out;

  /* EFS look up input structures */
  wwcoex_blank_efs_lookup_in  lookup_in;
  cxm_tech_type hprio_tech;
  cxm_tech_type lprio_tech;

  TTLMARKER(LMTSMGR_CHK_COEX_DL_IN);
  
  if (coex_output == NULL)
  {
    LMTSMGR_MSG_0(FATAL, "ERROR:wwcoex_check_coex_with_dl coex_output NULL");
	TTLMARKER(LMTSMGR_CHK_COEX_DL_ERR1);
    return FALSE;
  }

  if (!LMTSMGR_IS_TECH_VALID(tech1) ||
     !LMTSMGR_IS_TECH_VALID(tech2) ||
     !LMTSMGR_IS_TECH_VALID(vtech))
  {
    LMTSMGR_MSG_3(FATAL, "ERROR:wwcoex_check_coex_with_dl tech input invalid %d %d %d",
                        tech1, tech2, vtech);
    TTLMARKER(LMTSMGR_CHK_COEX_DL_ERR2);
    return FALSE;
  }

#ifdef FEATURE_LMTSMGR_SIMULATOR_SUPPORT
  #error code not present
#else
  tech1_prio_tbl.tech = tech1;
  tech2_prio_tbl.tech = tech2;
  (void)lmtsmgr_core_get_priority (&tech1_prio_tbl, &tech2_prio_tbl);
  if (LMTSMGR_TIER_1 == tech1_prio_tbl.pri)
  {
    hprio_tech = tech1;
    lprio_tech = tech2;
  }
  else if (LMTSMGR_TIER_2 == tech1_prio_tbl.pri)
  {
    hprio_tech = tech2;
    lprio_tech = tech1;
  }
  else
  {
    #ifndef SWCXM_SIMULATOR
    LMTSMGR_MSG_3(FATAL,"Tech %d has invalid tier %d.. ignoring coex with tech %d",
                   tech1, tech1_prio_tbl.pri, tech2);
    TTLMARKER(LMTSMGR_CHK_COEX_DL_ERR3);
    return FALSE;
    #else /* Go ahead with tech1 as high priority */
    hprio_tech = tech1;
    lprio_tech = tech2;
    #endif
  }
#endif
  
  /* Initialize coex output/input */
  coex_output->aplimit_valid = FALSE;
  coex_output->aplimit = DEFAULT_MAX_PLIMIT;  
  coex_output->action = ACTION_NONE;  
  nonim_comp_out.action = ACTION_NONE;
  nonim_comp_out.desense = NO_DESENSE;
  nonim_comp_out.enh_action_mask = 0;
  im_comp_out.action = ACTION_NONE;
  im_comp_out.desense = NO_DESENSE;

  if (vtech == tech1)
  {
    atech = tech2;
  }
  else
  {
    atech = tech1;
  }

  vt_st = &lmtsmgr.tech_state[vtech];
  at_st = &lmtsmgr.tech_state[atech];

  /* Get pointers to UL freq and power info node */
  atul = at_st->currList.links[at_st->currList.ul_freqList[atulindex]];

  /* Dont need victim UL if IM needs to be skipped */
  if (skip_im == FALSE)
  {
    vtul = &vt_st->currList.links[vt_st->currList.ul_freqList[vtulindex]];
  }

  for (vtdlindex = 0; vtdlindex < vt_st->currList.num_dl_entries; vtdlindex++)
  {
    /* DL freq arr has ids into the currList freqinfo */
    vtdl = vt_st->currList.links[vt_st->currList.dl_freqList[vtdlindex]];

    /* for SS, ignore the div and PM frequencies */
    if ( (lmtsmgr.device_mode == SYS_MODEM_DEVICE_MODE_SINGLE_SIM) &&
         (vtdl.freqInfo.link_info.type > 0 ) )
    {
      continue;
    }

    /* Update common LUT lookup info */
    lookup_in.vtech = vtech;
    lookup_in.vband = lmtsmgr_trans_sys_bands_to_rf_bands(
                                      vtdl.freqInfo.link_info.band, 
                                      vtech);
    lookup_in.atech = atech;
    lookup_in.aband = lmtsmgr_trans_sys_bands_to_rf_bands(
                                      atul.freqInfo.link_info.band,
                                      atech);
    lookup_in.victim_rxpower = vtdl.powerInfo.rx_power;
    lookup_in.aggr_txpower = atul.powerInfo.tx_power;
    lookup_in.hprio_tech = hprio_tech;
    

    /* Check for IM only if there is no emissions issue */
    im_comp_out.desense = NO_DESENSE;
    if (skip_im == FALSE)
    {
      /* Check IM with tech1 DL so tech1 as the victim */
      im_comp_in.vtech = vtech;
      im_comp_in.atech = atech;
      im_comp_in.vtulfreqinfo = vtul->freqInfo;
      im_comp_in.atulfreqinfo = atul.freqInfo;
      im_comp_in.vtdlfreqinfo = vtdl.freqInfo;

      LMTSMGR_DBG_MSG_3(LOW, "Debug:Checking IM for freqIds %u %u %u", 
                       atul.freqInfo.freqid, vtul->freqInfo.freqid, 
                       vtdl.freqInfo.freqid );

      (void)wwcoex_im_compare_freqs(&im_comp_in, &im_comp_out);
    }

    if ( !IS_IM_DESENSE(im_comp_out.desense) )
    {
      /* There is no IM desense, check for any non-IM issue */

      LMTSMGR_DBG_MSG_4(LOW, "Debug:Checking Non-IM for tech %d, tech %d with freqIds %u %u", 
                      tech1, tech2, vtdl.freqInfo.freqid, atul.freqInfo.freqid );

      nonim_comp_in.vtech = vtech;
      nonim_comp_in.atech = atech;
      nonim_comp_in.vtdlfreqinfo = vtdl.freqInfo;
      nonim_comp_in.atulfreqinfo = atul.freqInfo;
      (void)wwcoex_nonim_compare_freqs(&nonim_comp_in, &nonim_comp_out);

      /* Check for any special indications*/
      if (lmtsmgr_is_on_seperate_dev(vtech, atech))
      {
        lmgr_temp_ind_mask = MAX(lmgr_temp_ind_mask, 
                                 nonim_comp_out.special_ind_mask);
      }

      if ( nonim_comp_out.action != ACTION_BACKOFF )
      {
        if (( nonim_comp_out.desense != NO_DESENSE ) && ( nonim_comp_out.action != ACTION_NONE ))
        {
          lookup_in.victim_txpower = CXM_UNKNOWN_POWER;
          lookup_in.desense = nonim_comp_out.desense;
          lookup_in.action = nonim_comp_out.action;

          /* Do a lookUp for Non-IM Blanking case */
          wwcoex_desense_efs_lookup(&lookup_in, coex_output);
          action = coex_output->action;
        }
        else
        {
          /* There is no non-IM desense */
          action = ACTION_NONE;
          if (nonim_comp_out.special_ind_mask & (1<<FILTERED_PATH))
          {
            /* No desense in this combination, so no backoff required */
            lmtsmgr.tech_state[lprio_tech].pwr_info.new_limits.coex_plimit[hprio_tech] = DEFAULT_MAX_PLIMIT;
            lmtsmgr.tech_state[lprio_tech].pwr_info.new_limits.coex_slot_plimit[hprio_tech] = DEFAULT_MAX_PLIMIT;
          }
        }

        /* Store the action in the result table...  */
        if (vtech == tech1)
        {
          tech2_index = atul.freqInfo.freqid - at_st->currList.freqId_offset;
          tech1_index = vtdl.freqInfo.freqid - vt_st->currList.freqId_offset;     
        }
        else
        {
          tech1_index = atul.freqInfo.freqid - at_st->currList.freqId_offset;
          tech2_index = vtdl.freqInfo.freqid - vt_st->currList.freqId_offset;
        }

        /* Store action/desense pair if action is required (ie do not 
           overwrite BLANKING with NONE */
        if (action == MAX(action, result_tbl->arr[tech1_index][tech2_index].action))
        {
          result_tbl->arr[tech1_index][tech2_index].action = action;
          result_tbl->arr[tech1_index][tech2_index].desense = nonim_comp_out.desense;
          result_tbl->is_valid = TRUE;

          LMTSMGR_DBG_MSG_4(MED, "Debug:Non-IM result stored for index %d %d action %d, desense %d", 
                    tech1_index, tech2_index, action, nonim_comp_out.desense );
        }
        else
        {
          LMTSMGR_DBG_MSG_4(MED, "Debug:Non-IM result IGNORED for index %d %d action %d, desense %d", 
                    tech1_index, tech2_index, action, nonim_comp_out.desense );
        }
      
      }
      else
      {
        if (NO_DESENSE == nonim_comp_out.desense)
        {
          /* No desense in this combination, so no backoff required */
          lmtsmgr.tech_state[lprio_tech].pwr_info.new_limits.coex_plimit[hprio_tech] = DEFAULT_MAX_PLIMIT;
          lmtsmgr.tech_state[lprio_tech].pwr_info.new_limits.coex_slot_plimit[hprio_tech] = DEFAULT_MAX_PLIMIT;
        }
        else
        {
          /* Noise Desense */
          lookup_in.desense = nonim_comp_out.desense;
          lookup_in.victim_txpower = CXM_UNKNOWN_POWER;
          lookup_in.action = nonim_comp_out.action;
        }
      }
    }
    else if (vtul != NULL)
    {
      if ( im_comp_out.action != ACTION_BACKOFF )
      {
        /* There is IM in tech1 DL freq */
        lookup_in.victim_txpower = vtul->powerInfo.tx_power;
        lookup_in.desense = im_comp_out.desense;
        lookup_in.action = im_comp_out.action;

        /* Do a lookUp for IM Blanking case */
        wwcoex_desense_efs_lookup(&lookup_in, coex_output);
        action = coex_output->action;

        /* Store the action in the result table... */
        if (vtech == tech1)
        {
          tech2_index = atul.freqInfo.freqid - at_st->currList.freqId_offset;
          tech1_index = vtdl.freqInfo.freqid - vt_st->currList.freqId_offset;     
        }
        else
        {
          tech1_index = atul.freqInfo.freqid - at_st->currList.freqId_offset;
          tech2_index = vtdl.freqInfo.freqid - vt_st->currList.freqId_offset;
        }

         /* Store action/desense pair if action is required (ie do not 
           overwrite BLANKING with NONE */
        if (action == MAX(action, result_tbl->arr[tech1_index][tech2_index].action))
        {
          result_tbl->arr[tech1_index][tech2_index].action = action;
          result_tbl->arr[tech1_index][tech2_index].desense = im_comp_out.desense;
          result_tbl->is_valid = TRUE;

          LMTSMGR_DBG_MSG_4(MED, "Debug:IM result stored for index %d %d action %d, desense %d", 
                      tech1_index, tech2_index, action, im_comp_out.desense );
        }
        else
        {
          LMTSMGR_DBG_MSG_4(MED, "Debug:IM result IGNORED for index %d %d action %d, desense %d", 
                    tech1_index, tech2_index, action, im_comp_out.desense );
        }


        /* Also need to update the result for Tx-Tx conflict... */
        if (vtech == tech1)
        {
          tech2_index = atul.freqInfo.freqid - at_st->currList.freqId_offset;
          tech1_index = vtul->freqInfo.freqid - vt_st->currList.freqId_offset;
        }
        else
        {
          tech1_index = atul.freqInfo.freqid - at_st->currList.freqId_offset;
          tech2_index = vtul->freqInfo.freqid - vt_st->currList.freqId_offset;
        }    

        /* For Tx - Tx result, if there is already a desense flag, 
          then dont change it */
        if (result_tbl->arr[tech1_index][tech2_index].desense == NO_DESENSE)
        {
          /* Action should be none for Tx-Tx conflict */
          result_tbl->arr[tech1_index][tech2_index].action = ACTION_NONE;
          result_tbl->arr[tech1_index][tech2_index].desense = im_comp_out.desense;
          result_tbl->is_valid = TRUE;

          LMTSMGR_DBG_MSG_4(LOW, "Debug:IM result stored for index %d %d action %d, desense %d", 
                      tech1_index, tech2_index, action, im_comp_out.desense );
        }
      }
      else
      {
        /* Victim and Aggressor power will remain same, and it will be taken into EFS_Lookup,
                   whether to provide Backoff for High Prio or Low Prio Tech based on IM1 or IM2 desense */
        lookup_in.victim_txpower = vtul->powerInfo.tx_power;

        lookup_in.desense = im_comp_out.desense;
        lookup_in.action = im_comp_out.action;
      }
    }
    
    /* If power is unknown, then dont trigger backoffs... */
    if ( ((im_comp_out.action == ACTION_BACKOFF) && IS_IM_DESENSE(im_comp_out.desense))||
           ((nonim_comp_out.action == ACTION_BACKOFF) && IS_NON_IM_DESENSE(nonim_comp_out.desense)) )
    {
      /* Set power limits if techs are on seperate chains & thresholds are crossed */
      if (lmtsmgr_is_on_seperate_dev(lprio_tech, hprio_tech) &&
          (TRUE == wwcoex_desense_efs_lookup(&lookup_in, coex_output)) )
      {
        if(coex_output->aplimit_valid)
        {
          /* Check if slot level backoff is required for this combo */
          if (nonim_comp_out.enh_action_mask & (1<<ENH_CXM_SLOT_LEVEL))
          {
            lmtsmgr.tech_state[lprio_tech].pwr_info.new_limits.coex_slot_plimit[hprio_tech] = coex_output->aplimit;
            lmtsmgr.tech_state[lprio_tech].pwr_info.new_limits.coex_plimit[hprio_tech] = DEFAULT_MAX_PLIMIT;

            /* Store the action in the result table... */
            if (vtech == tech1)
            {
              tech2_index = atul.freqInfo.freqid - at_st->currList.freqId_offset;
              tech1_index = vtdl.freqInfo.freqid - vt_st->currList.freqId_offset;     
            }
            else
            {
              tech1_index = atul.freqInfo.freqid - at_st->currList.freqId_offset;
              tech2_index = vtdl.freqInfo.freqid - vt_st->currList.freqId_offset;
            }

            result_tbl->arr[tech1_index][tech2_index].action = (coex_output->aplimit==DEFAULT_MAX_PLIMIT) ? 
                                                                ACTION_NONE : nonim_comp_out.action;
            result_tbl->arr[tech1_index][tech2_index].desense = nonim_comp_out.desense;
            result_tbl->is_valid = TRUE;

            LMTSMGR_DBG_MSG_4(MED, "Debug:Non-IM result stored for index %d %d action %d, desense %d", 
                      tech1_index, tech2_index, nonim_comp_out.action, nonim_comp_out.desense );
          }
          else
          {
            /* Slot level is not needed; just set power limits to RF. */
            lmtsmgr.tech_state[lprio_tech].pwr_info.new_limits.coex_plimit[hprio_tech] = coex_output->aplimit;
            lmtsmgr.tech_state[lprio_tech].pwr_info.new_limits.coex_slot_plimit[hprio_tech] = DEFAULT_MAX_PLIMIT;
          }

          while (vtdlindex < vt_st->currList.num_dl_entries)
          {
            /* DL freq arr has ids into the currList freqinfo */
            vtdl = vt_st->currList.links[vt_st->currList.dl_freqList[vtdlindex]];

            lmgr_temp_ind_mask = MAX(lmgr_temp_ind_mask, 
                              wwcoex_is_filtered_path_combo(&(atul.freqInfo.link_info), &(vtdl.freqInfo.link_info)));
            vtdlindex++;
          };
          break;
        }
        else
        {
          /* No Coex limit defined, set it to MAX */
          lmtsmgr.tech_state[lprio_tech].pwr_info.new_limits.coex_plimit[hprio_tech] = DEFAULT_MAX_PLIMIT;
          lmtsmgr.tech_state[lprio_tech].pwr_info.new_limits.coex_slot_plimit[hprio_tech] = DEFAULT_MAX_PLIMIT;
        }
      }
    }
    else
    {
      /* We should not reach here */
      LMTSMGR_MSG_4(FATAL, "wwcoex_check_coex_with_dl IM info %d, %d Non-IM info %d, %d", 
                      im_comp_out.action, im_comp_out.desense, nonim_comp_out.action, nonim_comp_out.desense );
    }
  }

  TTLMARKER(LMTSMGR_CHK_COEX_DL_OUT);
	
  return TRUE;
}


/*=========================================================================

  FUNCTION:  wwcoex_check_spur_with_dl

=============================================================================*/
/*!
    @brief
    Check if Spur exists between victim rx and agressor frequencies
 
    @dependency
    this api should be called only if spur mitigation is enabled
 
    @return
    TRUE
*/
/*===========================================================================*/
boolean wwcoex_check_spur_with_dl
(
  cxm_tech_type     tech1,
  cxm_tech_type     tech2,
  cxm_tech_type     vtech,
  uint32            vtdlindex,
  wwcoex_tbl_type*  result_tbl
)
{
  uint32  tech1_index, tech2_index, atulindex, atdlindex;
  lmtsmgr_tech_state_type* vt_st, *at_st;
  lmtsmgr_tech_link_type vtdl, atul, atdl;
  cxm_tech_type atech;
  uint16 *handle = NULL;

  /* Non-IM Freq compare argument structures */
  wwcoex_spur_freqcompare_in   spur_comp_in;
  wwcoex_spur_freqcompare_out  spur_comp_out;

  TTLMARKER(LMTSMGR_CHK_SPUR_DL_IN);
  
  /* find out the vtech and atech */
  if (vtech == tech1)
  {
    atech = tech2;
  }
  else
  {
    atech = tech1;
  }

  /* Check if the input techs are valid */
  if (!LMTSMGR_IS_TECH_VALID(atech) ||
      !LMTSMGR_IS_TECH_VALID(vtech))
  {
    LMTSMGR_MSG_2(FATAL, "ERROR:wwcoex_check_spur_with_dl tech input invalid"
                         "at %d vt %d",atech, vtech);
    TTLMARKER(LMTSMGR_CHK_SPUR_DL_ERR1);
    return FALSE;
  }

  /* Initialize coex output/input */
  spur_comp_out.action = ACTION_NONE;
  spur_comp_out.handle = WWCOEX_SPUR_HANDLE_NONE;

  vt_st = &lmtsmgr.tech_state[vtech];
  at_st = &lmtsmgr.tech_state[atech];

  /* Get pointers to DL freq and power info node */
  vtdl = vt_st->currList.links[vt_st->currList.dl_freqList[vtdlindex]];

  /* Check for Spurs with Agressor UL frequencies */
  /* TODO: This step can be avoided for non TDS and LTE-TDD bands */
  for (atulindex = 0; atulindex < at_st->currList.num_ul_entries; atulindex++)
  {
    /* start with fresh handle ptr */
    handle = NULL; 
    
    /* DL freq arr has ids into the currList freqinfo */
    atul = at_st->currList.links[at_st->currList.ul_freqList[atulindex]];

    /* TODO: Find the VCO mapping for tech 1 and tech 2 from RF */

    /* Fill in the input params for comparission */
    spur_comp_in.vtech_vco = 1;
    spur_comp_in.atech_vco = 2;
    spur_comp_in.vtech = vtech;
    spur_comp_in.atech = atech;
    spur_comp_in.vtfreqinfo = vtdl.freqInfo;
    spur_comp_in.atfreqinfo = atul.freqInfo;

    /* Check for spur between 2 frequencies */
    (void)wwcoex_spur_compare_freqs(&spur_comp_in, &spur_comp_out);

    /* Store the action in the result table */
    if (vtech == tech1)
    {
      tech2_index = atul.freqInfo.freqid - at_st->currList.freqId_offset;
      tech1_index = vtdl.freqInfo.freqid - vt_st->currList.freqId_offset;
      handle = &(result_tbl->arr[tech1_index][tech2_index].spur_handle_1);
    }
    else
    {
      tech1_index = atul.freqInfo.freqid - at_st->currList.freqId_offset;
      tech2_index = vtdl.freqInfo.freqid - vt_st->currList.freqId_offset;
      handle = &(result_tbl->arr[tech1_index][tech2_index].spur_handle_2);
    }

    if (handle != NULL)
    {
      /* store the results and handle */
      *handle = spur_comp_out.handle;
    }
  }

  /* Check for Spurs with Victim DL frequencies */
  for (atdlindex = 0; atdlindex < at_st->currList.num_dl_entries; atdlindex++)
  {
    /* start with fresh handle ptr */
    handle = NULL;
    
    /* DL freq arr has ids into the currList freqinfo */
    atdl = at_st->currList.links[at_st->currList.dl_freqList[atdlindex]];

    /* TODO: Find the VCO mapping for tech 1 and tech 2 from RF */

    /* fill in the input params for comparission */
    spur_comp_in.vtech_vco = 2;
    spur_comp_in.atech_vco = 1;
    spur_comp_in.vtech = vtech;
    spur_comp_in.atech = atech;
    spur_comp_in.vtfreqinfo = vtdl.freqInfo;
    spur_comp_in.atfreqinfo = atdl.freqInfo;

    /* If spur mitigation is disabled then return default action and handle */
    if(IS_SPUR_MITIGATION_ENABLED(lmtsmgr.en_mitigation_mask))
    {
      /* Check for spur between 2 frequencies */
      (void)wwcoex_spur_compare_freqs(&spur_comp_in, &spur_comp_out);
    }

    /* Store the action in the result table */
    if (vtech == tech1)
    {
      tech2_index = atdl.freqInfo.freqid - at_st->currList.freqId_offset;
      tech1_index = vtdl.freqInfo.freqid - vt_st->currList.freqId_offset;
      handle = &(result_tbl->arr[tech1_index][tech2_index].spur_handle_1);
    }
    else
    {
      tech1_index = atdl.freqInfo.freqid - at_st->currList.freqId_offset;
      tech2_index = vtdl.freqInfo.freqid - vt_st->currList.freqId_offset;
      handle = &(result_tbl->arr[tech1_index][tech2_index].spur_handle_2);
    }

    if (handle != NULL)
    {
      /* store the results and handle */
      *handle = spur_comp_out.handle;
    }
  }
 
  TTLMARKER(LMTSMGR_CHK_SPUR_DL_OUT);
  return TRUE;
}


/*=========================================================================

  FUNCTION:  wwcoex_copy_spur_from_conflict_tbl

=============================================================================*/
/*!
    @brief
    Copy the spur enties from conflict table
 
    @return
    TRUE
*/
/*===========================================================================*/
boolean wwcoex_copy_spur_from_conflict_tbl
(
  wwcoex_tbl_type*  conflict_tbl,
  wwcoex_tbl_type*  result_tbl
)
{
  uint32  i,j;

  if((conflict_tbl == NULL)||
     (result_tbl == NULL))
  {
    return FALSE;
  }

  /* Copy all the spur handles from conflict table to result table */
  for (i = 0; i < WWCOEX_MAX_SUPPORTED_LINKS; i++)
  {
    for (j = 0; j < WWCOEX_MAX_SUPPORTED_LINKS; j++)
    {
      result_tbl->arr[i][j].spur_handle_1 = conflict_tbl->arr[i][j].spur_handle_1;
      result_tbl->arr[i][j].spur_handle_2 = conflict_tbl->arr[i][j].spur_handle_2;
    }
  }
 
  return TRUE;
}


/*=============================================================================

  FUNCTION:  wwcoex_compare_freq_lists

=============================================================================*/
/*!
    @brief
    Compare freq lists of two technologies and store the actions in the
    result table.
 
    @return
    TRUE
*/
/*===========================================================================*/
cxm_wwcoex_state_info_s wwcoex_compare_freq_lists
(
  cxm_tech_type tech1,
  cxm_tech_type tech2,
  wwcoex_tbl_type* result_tbl,
  boolean          spur_input_changed
)
{
  uint32 tech1_num_ulfreqs, tech2_num_ulfreqs, tech1_num_dlfreqs, tech2_num_dlfreqs;
  lmtsmgr_tech_state_type* tech1_st, *tech2_st;
  lmtsmgr_tech_link_type t1ul, t2ul;
  uint32 t1ulindex, t2ulindex, t1dlindex, t2dlindex, tech1_index, tech2_index;
  cxm_action_type action;
  cxm_wwcoex_state_info_s state;
  uint32 t1index, t2index, total_table_enries, entries_with_desense;
  boolean send_to_diag = FALSE;

  /* Emissions lut arguments */
  wwcoex_sem_lookup_in sem_lookup_in;
  /* SVLTE/SVDO Coex Output */
  wwan_desense_efs_lookup_out coex_output;
  
  TTLMARKER(LMTSMGR_COMP_FREQ_IN);
  
  tech1_st = &lmtsmgr.tech_state[tech1];
  tech2_st = &lmtsmgr.tech_state[tech2];

  tech1_num_ulfreqs = tech1_st->currList.num_ul_entries;
  tech2_num_ulfreqs = tech2_st->currList.num_ul_entries;

  tech1_num_dlfreqs = tech1_st->currList.num_dl_entries;
  tech2_num_dlfreqs = tech2_st->currList.num_dl_entries;

  /* Start with blank table but keep tech offset/entry info */
  memset(&temp_result_tbl, 0, sizeof(wwcoex_tbl_type));
  temp_result_tbl.tech1_fid_offset = result_tbl->tech1_fid_offset;
  temp_result_tbl.tech1_num_entries = result_tbl->tech1_num_entries;
  temp_result_tbl.tech2_fid_offset = result_tbl->tech2_fid_offset;
  temp_result_tbl.tech2_num_entries = result_tbl->tech2_num_entries;
  temp_result_tbl.is_valid = TRUE;

  /* Shouldn't get here with techs not even in Rx state */
  LMTSMGR_ASSERT((tech1_num_dlfreqs > 0) && (tech2_num_dlfreqs > 0));

  if (tech1_num_ulfreqs > 0 && tech2_num_ulfreqs > 0)
  {
    // Both techs are transmitting
    TTLMARKER(LMTSMGR_COMP_FREQ_T1T2UL_IN);

    state.coex_st = COEX_DUAL_ACTIVE_ST;
    state.prob_victim_mask = (1<<tech1) | (1<<tech2);
    state.prob_aggr_mask = (1<<tech1) | (1<<tech2);

    /* check if im desense exists for the tech pair */
    if ( wwcoex_check_tech_pair_im(tech1, tech2) ) 
    {
      LMTSMGR_DBG_MSG_2(MED, "IM involved for tech %d & tech %d pair", 
                            tech1, tech2 );

      /* IM desense exists */
      for (t1ulindex = 0; t1ulindex < tech1_num_ulfreqs; t1ulindex++)
      {
        /* UL freq arr has ids into the currList freqinfo */
        t1ul = tech1_st->currList.links[tech1_st->currList.ul_freqList[t1ulindex]];

        for (t2ulindex = 0; t2ulindex < tech2_num_ulfreqs; t2ulindex++)
        {
          /* UL freq arr has ids into the currList freqinfo */
          t2ul = tech2_st->currList.links[tech2_st->currList.ul_freqList[t2ulindex]];

          /* Check Emissions threshold in LUT */
          sem_lookup_in.tech1 = tech1;
          sem_lookup_in.t1band = lmtsmgr_trans_sys_bands_to_rf_bands(
                                                      t1ul.freqInfo.link_info.band, 
                                                      tech1);
          sem_lookup_in.tech2 = tech2;
          sem_lookup_in.t2band = lmtsmgr_trans_sys_bands_to_rf_bands(
                                                      t2ul.freqInfo.link_info.band,
                                                      tech2);
          sem_lookup_in.t1_txpwr = t1ul.powerInfo.tx_power;
          sem_lookup_in.t2_txpwr = t2ul.powerInfo.tx_power;

          LMTSMGR_DBG_MSG_4(MED, "Debug:Checking SEM for tech %d, tech %d with powers 0x%x 0x%x", 
                      tech1, tech2, sem_lookup_in.t1_txpwr, sem_lookup_in.t2_txpwr );

          action = wwcoex_emission_efs_lookup(sem_lookup_in);

          /* Store the action */
          tech1_index = t1ul.freqInfo.freqid - tech1_st->currList.freqId_offset;
          tech2_index = t2ul.freqInfo.freqid - tech2_st->currList.freqId_offset;
        
          temp_result_tbl.arr[tech1_index][tech2_index].action = action;
          if (action != ACTION_NONE )
          {
            temp_result_tbl.arr[tech1_index][tech2_index].desense = SPURIOUS_EMISSIONS;
          }
          else
          {
            temp_result_tbl.arr[tech1_index][tech2_index].desense = NO_DESENSE;
          }
          LMTSMGR_DBG_MSG_3(LOW, "Debug:SEM result stored for index %d %d action %d", 
                      tech1_index, tech2_index, action );

          /* Check coex of Tech1 DL with Tech1 UL and Tech2 UL */
          wwcoex_check_coex_with_dl(tech1, tech2, 
                                    tech1,      // Victim tech
                                    t1ulindex,  // Victim UL index
                                    t2ulindex,  // Aggr UL index 
                                    (action != ACTION_NONE),
                                    &temp_result_tbl,
                                    &coex_output);

          if (coex_output.aplimit_valid == FALSE)
          {
            /* Check coex of tech2 DL with tech1 UL and tech 2 UL */
            wwcoex_check_coex_with_dl(tech1, tech2, 
                                      tech2,       // Victim tech
                                      t2ulindex,   // Victim UL index
                                      t1ulindex,   // Aggr UL index 
                                      (action != ACTION_NONE),
                                      &temp_result_tbl,
                                      &coex_output);
          }
        
          if (coex_output.aplimit_valid == TRUE)
            break;
        } /* End For */

        if (coex_output.aplimit_valid == TRUE)
          break;
      } /* End For */
    }
    else
    {
        LMTSMGR_DBG_MSG_2(MED, "No IM involved for tech %d,& tech %d pair", 
                            tech1, tech2 );
        /* IM desense doesnt exist for tech pair */
        for (t1ulindex = 0; t1ulindex < tech1_num_ulfreqs; t1ulindex++)
        {
           /* Check coex of Tech2 DL with Tech1 UL and skip IM */
           wwcoex_check_coex_with_dl(tech1, tech2, 
                                     tech2,   /* Victim tech */
                                     0,       /* No UL for victim */
                                     t1ulindex, 
                                     TRUE,    /* Skip IM checks */
                                   &temp_result_tbl,
                                     &coex_output);
           if (coex_output.aplimit_valid == TRUE)
              break;
        }

        for (t2ulindex = 0; t2ulindex < tech2_num_ulfreqs; t2ulindex++)
        {
           /* Check coex of Tech1 DL with Tech2 UL and skip IM */
           wwcoex_check_coex_with_dl(tech1, tech2, 
                                     tech1, /* Victim Tech */
                                     0, /* No UL index needed for victim */
                                     t2ulindex, 
                                     TRUE,  /* Skip IM checks */
                                   &temp_result_tbl,
                                     &coex_output);
           if (coex_output.aplimit_valid == TRUE)
              break;
        }
     }

    /* Log the state */
    send_to_diag = TRUE;

    /* Calculations for desense detection are done here.
       Desense detection is done only when both the techs have UL frequencies 
         - Parse through the conflcit table created for the 2 techs
         - Mark the number of entries in the table
         - Mark the number of entries that have valid desense action  
         - Average this value over a given period to detect desense */
    
    total_table_enries   = temp_result_tbl.tech1_num_entries * temp_result_tbl.tech2_num_entries;
    entries_with_desense = 0;

    for (t1index=0; t1index < temp_result_tbl.tech1_num_entries; t1index++)
    {
      for (t2index=0; t2index < temp_result_tbl.tech2_num_entries; t2index++)
      {
        if(temp_result_tbl.arr[t1index][t2index].action != ACTION_NONE)
        {
          entries_with_desense++;
        }
      }
    }

    /* Maintain a history of statistics to detect desense over a period */
    lmtsmgr.desense_stats.cxm_report_count++;
    lmtsmgr.desense_stats.cxm_entries_count += total_table_enries;
    lmtsmgr.desense_stats.cxm_desense_count += entries_with_desense;
    
	TTLMARKER(LMTSMGR_COMP_FREQ_T1T2UL_OUT);    
  }
  else if (tech1_num_ulfreqs > 0)
  {
    TTLMARKER(LMTSMGR_COMP_FREQ_T1UL_IN);
	
    state.coex_st = COEX_SINGLE_ACTIVE_ST;
    state.prob_victim_mask = 1<<tech2;
    state.prob_aggr_mask = 1<<tech1;

    for (t1ulindex = 0; t1ulindex < tech1_num_ulfreqs; t1ulindex++)
    {
      /* Check coex of Tech2 DL with Tech1 UL and skip IM */
      wwcoex_check_coex_with_dl(tech1, tech2, 
                                tech2,   /* Victim tech */
                                0,       /* No UL for victim */
                                t1ulindex, 
                                TRUE,    /* Skip IM checks */
                                &temp_result_tbl,
                                &coex_output);
      
      if (coex_output.aplimit_valid == TRUE)
        break;
    }

    /* Log the state */
    send_to_diag = TRUE;
	
	TTLMARKER(LMTSMGR_COMP_FREQ_T1UL_OUT);
  }
  else if (tech2_num_ulfreqs > 0)
  {
    TTLMARKER(LMTSMGR_COMP_FREQ_T2UL_IN);
	
    state.coex_st = COEX_SINGLE_ACTIVE_ST;
    state.prob_victim_mask = (1<<tech1);
    state.prob_aggr_mask = 1<<tech2;

    for (t2ulindex = 0; t2ulindex < tech2_num_ulfreqs; t2ulindex++)
    {
      /* Check coex of Tech1 DL with Tech2 UL and skip IM */
      wwcoex_check_coex_with_dl(tech1, tech2, 
                                tech1, /* Victim Tech */
                                0, /* No UL index needed for victim */
                                t2ulindex, 
                                TRUE,  /* Skip IM checks */
                                &temp_result_tbl,
                                &coex_output);
      if (coex_output.aplimit_valid == TRUE)
        break;
    }

    /* Log the state */
    send_to_diag = TRUE;
	
	TTLMARKER(LMTSMGR_COMP_FREQ_T2UL_OUT);
  }
  else
  {
    state.coex_st = COEX_INACTIVE_ST;
    state.prob_victim_mask = 0;
    state.prob_aggr_mask = 0;
  }

  if(IS_SPUR_MITIGATION_ENABLED(lmtsmgr.en_mitigation_mask))
  {
    TTLMARKER(LMTSMGR_COMP_FREQ_SPUR_IN);
    /* If the spur input has not changed then do not recalculate spurs
       copy the spurs already in the table 
       This code is used to avoid recalculation and creating new handles for
       power updates */
    if(spur_input_changed == FALSE)
    {
      wwcoex_copy_spur_from_conflict_tbl(result_tbl,
                                         &temp_result_tbl);
    }
    else
    {
      /* If a tech2 has DL freq then call spur API with tech2 as victim */
      if(tech2_num_dlfreqs > 0)
      {
        for (t2dlindex = 0; t2dlindex < tech2_num_dlfreqs; t2dlindex++)
        {
          /* Check spur of Tech2 DL with Tech1 UL or DL */
          wwcoex_check_spur_with_dl(tech1, 
                                    tech2,
                                    tech2,
                                    t2dlindex,
                                    &temp_result_tbl
                                   );
        }
        send_to_diag = TRUE;
      }

      /* If a tech1 has DL freq then call spur API with tech1 as victim */
      if(tech1_num_dlfreqs > 0)
      {
        for (t1dlindex = 0; t1dlindex < tech1_num_dlfreqs; t1dlindex++)
        {
          /* Check spur of Tech1 DL with Tech2 UL or DL */
          wwcoex_check_spur_with_dl(tech1, 
                                    tech2,
                                    tech1,
                                    t1dlindex,
                                    &temp_result_tbl
                                   );
        }
        send_to_diag = TRUE;
      }
    }
	
	TTLMARKER(LMTSMGR_COMP_FREQ_SPUR_OUT);
  }

  /* Send the packet to diag if the flag is set */
  if(send_to_diag == TRUE)
  {
    wwcoex_log_state(tech1, tech2, &temp_result_tbl);
  }

  /* Copy results to original table */
  memscpy((void *)(result_tbl->arr), 
          WWCOEX_MAX_SUPPORTED_LINKS * WWCOEX_MAX_SUPPORTED_LINKS *sizeof(coex_result_type),
          (void *)(temp_result_tbl.arr),
          WWCOEX_MAX_SUPPORTED_LINKS * WWCOEX_MAX_SUPPORTED_LINKS *sizeof(coex_result_type)); 

  TTLMARKER(LMTSMGR_COMP_FREQ_OUT);
  
  return state;
}
#endif /* FEATURE_MODEM_COEXISTENCE_SW */

#endif /* MCS_LIMITS_LOW_MEMORY */


/*=============================================================================

  FUNCTION:  wwcoex_query_max_action

=============================================================================*/
/*!
    @brief
    Queries cxm max action for all aggr UL freqIds with victim DL freqIds.
    
    @dependencies
    Aggr UL and victim DL freqIds should not be greater than WWCOEX_MAX_REQUESTING_IDS
 
    @return
    TRUE
*/
/*===========================================================================*/
uint32 wwcoex_query_max_action
(
  cxm_tech_type aggr_tech,
  cxm_tech_link_direction  aggr_dir,
  cxm_tech_type victim_tech,
  cxm_tech_link_direction  victim_dir
)
{
  lmtsmgr_tech_state_type *aggr_st, *victim_st;
  cxm_action_query_s query_st;
  uint16 requesting_ids[WWCOEX_MAX_REQUESTING_IDS];
  uint16 conflicting_ids[WWCOEX_MAX_REQUESTING_IDS];
  uint32 index, i;

  aggr_st = &lmtsmgr.tech_state[aggr_tech];
  victim_st = &lmtsmgr.tech_state[victim_tech];

  /* Reset all the fields */
  query_st.num_requesting_ids = 0;
  query_st.num_conflicting_ids = 0;

  if (aggr_dir == CXM_LNK_DRCTN_UL)
  {
    /* Assumption: there should be at most 8 UL aggr freqs */
    if (aggr_st->currList.num_ul_entries > WWCOEX_MAX_REQUESTING_IDS)
    { 
      query_st.num_requesting_ids = WWCOEX_MAX_REQUESTING_IDS;
      LMTSMGR_MSG_2(ERROR, "Num UL entries is incorrectly %d for tech %d", 
                   aggr_st->currList.num_ul_entries, aggr_tech);
    }
    else
    {
      query_st.num_requesting_ids = aggr_st->currList.num_ul_entries;
    }

    for (i=0; i < query_st.num_requesting_ids; i++)
    {
      index = aggr_st->currList.ul_freqList[i];
      requesting_ids[i] = (uint16)aggr_st->currList.links[index].freqInfo.freqid;
    }
  }


  if (victim_dir == CXM_LNK_DRCTN_DL)
  {
    /* Assumption: there should be at most 8 DL victim freqs */
    if (victim_st->currList.num_dl_entries > WWCOEX_MAX_REQUESTING_IDS)
    {
      query_st.num_conflicting_ids = WWCOEX_MAX_REQUESTING_IDS;
      LMTSMGR_MSG_2(ERROR, "Num DL entries is incorrectly %d for tech %d", 
                   victim_st->currList.num_dl_entries, victim_tech);
    }
    else
    {
      query_st.num_conflicting_ids = victim_st->currList.num_dl_entries;
    }

    for (i=0; i < query_st.num_conflicting_ids; i++)
    {
      index = victim_st->currList.dl_freqList[i];
      conflicting_ids[i] = (uint16)victim_st->currList.links[index].freqInfo.freqid;
    }
  }

  query_st.requesting_ids = requesting_ids;
  query_st.conflicting_ids = conflicting_ids;

  if ( FALSE == cxm_query_action(&query_st)) 
  {
    query_st.actions = 0;
  }
 
  return query_st.actions;
}

#ifndef MCS_LIMITS_LOW_MEMORY

/*=============================================================================

  FUNCTION:  lmtsmgr_update_lmtsmgr_coex_state

=============================================================================*/
/*!
    @brief
    Updates Coex state for the tech combination
 
    @return
    None
*/
/*===========================================================================*/
static void lmtsmgr_update_lmtsmgr_coex_state
(
  cxm_tech_type tech1,
  cxm_tech_type tech2,
  cxm_wwcoex_state_info_s* old_st
)
{
  lmtsmgr_tech_state_type* tech1_st, *tech2_st;
  cxm_wwcoex_state_info_s temp_state;

  /* Initial state */
  temp_state.coex_st = COEX_INACTIVE_ST;
  temp_state.prob_victim_mask = 0;
  temp_state.prob_aggr_mask = 0;

  if (!LMTSMGR_IS_TECH_VALID(tech1) ||
     !LMTSMGR_IS_TECH_VALID(tech2) ||
     old_st == NULL)
  {
    LMTSMGR_MSG_2(FATAL, "ERROR:lmtsmgr_update_lmtsmgr_coex_state tech input invalid %d %d",
                        tech1, tech2);
    return;
  }

  tech1_st = &lmtsmgr.tech_state[tech1];
  tech2_st = &lmtsmgr.tech_state[tech2];

  /* Update Coex state for the techs which are active */
  if (tech1_st->currList.num_ul_entries > 0 && 
       tech2_st->currList.num_ul_entries > 0)
  {
    // Both techs are transmitting
    temp_state.coex_st = COEX_DUAL_ACTIVE_ST;
    temp_state.prob_victim_mask = (1<<tech1) | (1<<tech2);
    temp_state.prob_aggr_mask = (1<<tech1) | (1<<tech2);
  }
  else if (tech1_st->currList.num_ul_entries > 0)
  {
    temp_state.coex_st = COEX_SINGLE_ACTIVE_ST;
    temp_state.prob_victim_mask = 1<<tech2;
    temp_state.prob_aggr_mask = 1<<tech1;
  }
  else if (tech2_st->currList.num_ul_entries > 0)
  {
    temp_state.coex_st = COEX_SINGLE_ACTIVE_ST;
    temp_state.prob_victim_mask = 1<<tech1;
    temp_state.prob_aggr_mask = 1<<tech2;
  }

  /* Set temp state as the new state only if it is higher... */
  if (temp_state.coex_st > old_st->coex_st)
  {
    old_st->coex_st = temp_state.coex_st;
  }

  /* Or the victim mask with the overall mask */
  old_st->prob_victim_mask |= temp_state.prob_victim_mask;
  old_st->prob_aggr_mask |= temp_state.prob_aggr_mask;

  return;
}



/*=============================================================================

  FUNCTION:  lmtsmgr_tech_combo_update_filter_path

=============================================================================*/
/*!
    @brief
    Updates Coex Filter Path for the tech combination for which there is no update
 
    @return
    None
*/
/*===========================================================================*/
static void lmtsmgr_tech_combo_update_filter_path
(
  cxm_tech_type tech1,
  cxm_tech_type tech2
)
{
  lmtsmgr_tech_state_type* tech1_st, *tech2_st;
  uint32 tech1_num_ulfreqs, tech2_num_ulfreqs, tech1_num_dlfreqs, tech2_num_dlfreqs;
  lmtsmgr_tech_link_type ul, dl;
  uint32 ulindex, dlindex;
  uint32 final_special_ind_mask;

  if (!LMTSMGR_IS_TECH_VALID(tech1) ||
     !LMTSMGR_IS_TECH_VALID(tech2))
  {
    LMTSMGR_MSG_2(FATAL, "ERROR:lmtsmgr_tech_combo_update_filter_path tech input invalid %d %d",
                        tech1, tech2);
    return;
  }


  if (lmtsmgr_is_on_seperate_dev(tech1, tech2))
  {
    /* Initial state */
    final_special_ind_mask = 0;
    tech1_st = &lmtsmgr.tech_state[tech1];
    tech2_st = &lmtsmgr.tech_state[tech2];

    tech1_num_ulfreqs = tech1_st->currList.num_ul_entries;
    tech2_num_ulfreqs = tech2_st->currList.num_ul_entries;
  
    tech1_num_dlfreqs = tech1_st->currList.num_dl_entries;
    tech2_num_dlfreqs = tech2_st->currList.num_dl_entries;
  
    /* Tech1 UL as aggressor and Tech2 DL as Victim */
    for (ulindex = 0; ulindex < tech1_num_ulfreqs; ulindex++)
    {
      /* UL freq arr has ids into the currList freqinfo */
      ul = tech1_st->currList.links[tech1_st->currList.ul_freqList[ulindex]];
      for (dlindex = 0; dlindex < tech2_num_dlfreqs; dlindex++)
      {
        dl = tech2_st->currList.links[tech2_st->currList.dl_freqList[dlindex]];

        final_special_ind_mask = MAX(final_special_ind_mask, 
                         wwcoex_is_filtered_path_combo(&(ul.freqInfo.link_info), &(dl.freqInfo.link_info)));
      }
    }

    /* Tech2 UL as aggressor and Tech1 DL as Victim */
    for (ulindex = 0; ulindex < tech2_num_ulfreqs; ulindex++)
    {
      /* UL freq arr has ids into the currList freqinfo */
      ul = tech2_st->currList.links[tech2_st->currList.ul_freqList[ulindex]];
      for (dlindex = 0; dlindex < tech1_num_dlfreqs; dlindex++)
      {
        dl = tech1_st->currList.links[tech1_st->currList.dl_freqList[dlindex]];
        final_special_ind_mask = MAX(final_special_ind_mask, 
                          wwcoex_is_filtered_path_combo(&(ul.freqInfo.link_info), &(dl.freqInfo.link_info)));
      }
    }

    lmgr_temp_ind_mask = MAX(lmgr_temp_ind_mask,final_special_ind_mask);
  }


  return;
}

#endif /* MCS_LIMITS_LOW_MEMORY */

/*=============================================================================

  FUNCTION:  lmtsmgr_process_coex_input

=============================================================================*/
/*!
    @brief
    Processes any change in the coex input variables
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_process_coex_input(void)
{

#ifndef MCS_LIMITS_LOW_MEMORY

#ifdef FEATURE_MODEM_COEXISTENCE_SW
  uint32 tbl_index;
  cxm_tech_type tech1, tech2;
  lmtsmgr_tech_state_type* tech1_st, *tech2_st;
  wwcoex_conflict_tbls_type* conflict_tbl;
  cxm_wwcoex_state_info_s new_state, temp_state;
  sys_modem_device_mode_e_type device_mode;
  uint32 onex_action_mask = 0, hdr_action_mask = 0;
  trm_get_info_input_type trm_info_in;
  trm_get_info_return_type trm_info_out;
  static boolean is_trm_simul_cap_init = FALSE;
  
  TTLMARKER(LMTSMGR_PROC_COEX_IN);

  /* Initial state */
  new_state.coex_st = COEX_INACTIVE_ST;
  new_state.prob_victim_mask = 0;
  new_state.prob_aggr_mask = 0;
  temp_state = new_state;

  /* Set the mask to 0 */
  lmgr_temp_ind_mask = 0;

  /* Check if CM mode has changed... */
  device_mode = cm_get_device_mode();

  /* Get the simultaneaous capacity setting from TRM */
  if (is_trm_simul_cap_init == FALSE)
  {
    /* Set the flag so TRM API is called once */
    is_trm_simul_cap_init = TRUE;

    /* Call TRM API and initilize DR-DSDS setting */
    trm_info_in.info_type = TRM_SIMUL_CAPABILITY;
    trm_info_in.client_id = 0;
    trm_get_info(&trm_info_in, &trm_info_out);
    LMTSMGR_ASSERT(trm_info_out.info_type == TRM_SIMUL_CAPABILITY);
    lmtsmgr.trm_simul_cap = trm_info_out.info.simul_cap_info;
    wwcoex_set_rf_mode(LMTSMGR_IS_DRDSDS_ENABLED());

    /* Retrieve Single-TX DSDA setting */
    trm_info_in.info_type = TRM_SINGLE_TX_CAPABILITY;
    trm_info_in.client_id = 0;
    trm_get_info(&trm_info_in, &trm_info_out);
    LMTSMGR_ASSERT(trm_info_out.info_type == TRM_SINGLE_TX_CAPABILITY);
    lmtsmgr.trm_single_tx_dsda = trm_info_out.info.single_tx_info.info;
  }
  
  /* check if device mode NV has changed... */
  if (lmtsmgr.device_mode != (uint32)device_mode)
  {
    /* Set the device mode in limitsmgr global space... */
    LMTSMGR_MSG_3(HIGH, "Device mode changed from %d to %d, Mitigation mask 0x%x",
                  lmtsmgr.device_mode, device_mode, lmtsmgr.mitigation_mask);
    lmtsmgr.device_mode = (uint32)device_mode;

    if (device_mode == SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE)
    {
      if (IS_WWAN_MITIGATION_DISABLED(lmtsmgr.mitigation_mask))
      {
        LMTSMGR_MSG_1(HIGH, "WWAN Mitigation is disabled 0x%x",
                      lmtsmgr.mitigation_mask);
        /* Mitigation is disabled... */
        wwcoex_set_sub_state(0,0);
      }
      else
      {
        wwcoex_set_sub_state(2,2);
      }
    }
    else /* Behave as if DSDS */
    {
      /* behave as if DSDS but run freq comparisions for SG*, SV* cases... */
      wwcoex_set_sub_state(2,1);
    }  
  }

  /* For SR-DSDS, TSTS, no need to run any freq comparisons */
  if ( (lmtsmgr.device_mode != SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE) &&
       (lmtsmgr.device_mode != SYS_MODEM_DEVICE_MODE_SINGLE_SIM) &&
       (!LMTSMGR_IS_DRDSDS_ENABLED()) )
  {
    /* Reset the flag for all techs */
    for (tech1 = (CXM_TECH_MAX-1); tech1 >= 0; tech1--)
    {
      tech1_st = &lmtsmgr.tech_state[tech1];
      tech1_st->freq_change = FALSE;
      tech1_st->power_change = FALSE;
    }
     
    TTLMARKER(LMTSMGR_PROC_COEX_DSDS_EXIT);

    /* No need to do anything else... */
    return;
  }  
  

  TTLMARKER(LMTSMGR_PROC_COEX_LOOP_IN);
  
  for (tech1 = (CXM_TECH_MAX-1); tech1 >= 0; tech1--)
  {
    tech1_st = &lmtsmgr.tech_state[tech1];
     
    /* Ignore the techs that are inactive */
    if (tech1_st->currList.num_entries == 0)
    {
      continue;
    }

    LMTSMGR_MSG_1(LOW, "proc_coex: Found tech %d active", tech1);

    tech2 = tech1 - 1;
    while (tech2 >= 0)
    {
      tech2_st = &lmtsmgr.tech_state[tech2];

      /* Only consider techs that are inactive */
      if (tech2_st->currList.num_entries > 0)
      {
        /* Tech1 and Tech2 are active i.e. atleast in rx state */
        
        // Get the table node for this concurrent scenario
        conflict_tbl = wwcoex_get_table_node((uint8)tech1, (uint8)tech2);
        if (conflict_tbl == NULL)
        {
          /* Should never come here */
          LMTSMGR_MSG_2(FATAL, "Could not find a table node for techs %d %d",
                        tech1, tech2);

          tech2--;

          continue;
        }

        if ( (tech1_st->power_change == FALSE) && 
             (tech2_st->power_change == FALSE) &&
             (tech1_st->freq_change == FALSE) && 
             (tech2_st->freq_change == FALSE) )
        {
          lmtsmgr_update_lmtsmgr_coex_state(tech1, tech2, &new_state);

          lmtsmgr_tech_combo_update_filter_path(tech1, tech2);

          tech2--;

          continue;
        }

        /* Is there a power update */
        if ((tech1_st->power_change == TRUE) || (tech2_st->power_change == TRUE))
        {
          /* just compare the current tables */
          tbl_index = conflict_tbl->current_tbl;

          if ((conflict_tbl->current_tbl >= 0) && (tbl_index < WWCOEX_MAX_TABLES_PER_SCENARIO))
          {
            /* Perform comparisons and store in this current tbl */
            temp_state = wwcoex_compare_freq_lists(tech1, tech2, 
                                                &conflict_tbl->tables[tbl_index],
                                                FALSE);
          }
          else
          {
            LMTSMGR_MSG_3(FATAL,"Tbl index %d incorrect for techs %d %d",
                                 conflict_tbl->current_tbl, tech1, tech2 );
          }

        }
        /* Is there a frequency update on both techs ? */
        else if (tech1_st->freq_change == TRUE && tech2_st->freq_change == TRUE)
        {
          /* Should not happen */
          LMTSMGR_MSG_2(ERROR, "Both techs %d and %d have freq_change set", 
                        tech1, tech2);

          /* Get the least used table index */
          tbl_index = wwcoex_get_oldest_tbl_index(conflict_tbl);
          if (tbl_index < WWCOEX_MAX_TABLES_PER_SCENARIO)
          {
            conflict_tbl->current_tbl = tbl_index;

              /* Update the freq offsets */
            conflict_tbl->tables[tbl_index].tech1_fid_offset = 
                                       tech1_st->currList.freqId_offset;
            conflict_tbl->tables[tbl_index].tech2_fid_offset = 
                                       tech2_st->currList.freqId_offset;
            conflict_tbl->tables[tbl_index].tech1_num_entries =
                                       tech1_st->currList.num_entries;
            conflict_tbl->tables[tbl_index].tech2_num_entries =
                                       tech2_st->currList.num_entries;
                                       
            /* Reset the table */
            memset((void*)conflict_tbl->tables[tbl_index].arr, 0, 
                         sizeof(conflict_tbl->tables[tbl_index].arr));

            /* Perform comparisons and store in this current tbl */
            temp_state = wwcoex_compare_freq_lists(tech1, tech2, 
                                                  &conflict_tbl->tables[tbl_index],
                                                  TRUE);
          }
          else
          {
            LMTSMGR_MSG_3(FATAL,"Tbl index %d incorrect for techs %d %d",
                             tbl_index, tech1, tech2 );
          }
        }
        /* Tech1 freq has changed */
        else if (tech1_st->freq_change == TRUE)
        {
          /* Get the least used table index */
          tbl_index = wwcoex_get_oldest_tbl_index(conflict_tbl);
          if (tbl_index < WWCOEX_MAX_TABLES_PER_SCENARIO)
          {
            conflict_tbl->current_tbl = tbl_index;

            LMTSMGR_DBG_MSG_2(MED, "Debug: Tech %d freq change is set, current tbl %d",
                          tech1, tbl_index);
                          
            /* Update the freq offsets */
            conflict_tbl->tables[tbl_index].tech1_fid_offset = 
                                       tech1_st->currList.freqId_offset;
            conflict_tbl->tables[tbl_index].tech2_fid_offset = 
                                       tech2_st->currList.freqId_offset;
            conflict_tbl->tables[tbl_index].tech1_num_entries =
                                       tech1_st->currList.num_entries;
            conflict_tbl->tables[tbl_index].tech2_num_entries =
                                       tech2_st->currList.num_entries;

            /* Reset the table */
            memset((void *)conflict_tbl->tables[tbl_index].arr, 0, 
                         sizeof(conflict_tbl->tables[tbl_index].arr));

            /* Perform comparisons and store in this current tbl */
            temp_state = wwcoex_compare_freq_lists(tech1, tech2, 
                                                  &conflict_tbl->tables[tbl_index],
                                                  TRUE);
          }
          else
          {
            LMTSMGR_MSG_3(FATAL,"Tbl index %d incorrect for techs %d %d",
                             tbl_index, tech1, tech2 );
          }

        }
        /* Tech2 frequency has changed */
        else 
        {
          /* Get the least used table index */
          tbl_index = wwcoex_get_oldest_tbl_index(conflict_tbl);
          if (tbl_index < WWCOEX_MAX_TABLES_PER_SCENARIO)
          {
            conflict_tbl->current_tbl = tbl_index;

            LMTSMGR_DBG_MSG_2(MED, "Debug: Tech %d freq change is set, current tbl %d",
                          tech2, tbl_index);

                /* Update the freq offsets */
            conflict_tbl->tables[tbl_index].tech1_fid_offset = 
                                       tech1_st->currList.freqId_offset;
            conflict_tbl->tables[tbl_index].tech2_fid_offset = 
                                       tech2_st->currList.freqId_offset;
            conflict_tbl->tables[tbl_index].tech1_num_entries =
                                       tech1_st->currList.num_entries;
            conflict_tbl->tables[tbl_index].tech2_num_entries =
                                       tech2_st->currList.num_entries;

            /* Reset the table */
            memset((void *)conflict_tbl->tables[tbl_index].arr, 0, 
                       sizeof(conflict_tbl->tables[tbl_index].arr));
                                     
            /* Perform comparisons and store in this current tbl */
            temp_state = wwcoex_compare_freq_lists(tech1, tech2, 
                                                &conflict_tbl->tables[tbl_index],
                                                TRUE);
          }
          else
          {
            LMTSMGR_MSG_3(FATAL,"Tbl index %d incorrect for techs %d %d",
                             tbl_index, tech1, tech2 );
          }
        }

        /* Set temp state as the new state only if it is higher... */
        if (temp_state.coex_st > new_state.coex_st)
        {
          new_state.coex_st = temp_state.coex_st;
        }
        /* Or the victim mask with the overall mask */
        new_state.prob_victim_mask |= temp_state.prob_victim_mask;
        new_state.prob_aggr_mask |= temp_state.prob_aggr_mask;
      }
    
      /* move to the next tech id */
      tech2--;
    }

    /* Reset the flag for tech 1 */
    tech1_st->freq_change = FALSE;
    tech1_st->power_change = FALSE;

    /* Notify techs of the new pwr limits */
    lmtsmgr_notify_tech_plimit(tech1);
  }

  TTLMARKER(LMTSMGR_PROC_COEX_LOOP_OUT);

  /* Check if G Tx interference with C2K Rx is possible to request G for 
     Tx Activity indication */
  onex_action_mask = wwcoex_query_max_action(CXM_TECH_GSM2, CXM_LNK_DRCTN_UL, 
                                             CXM_TECH_ONEX, CXM_LNK_DRCTN_DL);

  hdr_action_mask = wwcoex_query_max_action(CXM_TECH_GSM2, CXM_LNK_DRCTN_UL, 
                                              CXM_TECH_HDR, CXM_LNK_DRCTN_DL);
  
  LMTSMGR_MSG_2(MED, "Debug: Max query actions 1x: %d, HDR: %d", 
                     onex_action_mask,
                     hdr_action_mask);

  /* If either 1x or HDR has interference, send activity request to GSM */
  if ( (onex_action_mask > 0) || (hdr_action_mask > 0))
  {
    wwcoex_set_send_flag(CXM_TECH_GSM2, TRUE);
  }
  else
  {
    wwcoex_set_send_flag(CXM_TECH_GSM2, FALSE);
  }

  /* check if special ind mask has changed */
  wwcoex_check_special_inds();

  /* Check if coex state has changed and send the broadcast message */
  lmtsmgr_update_wwcoex_state(new_state);

  TTLMARKER(LMTSMGR_PROC_COEX_OUT);

#endif /* FEATURE_MODEM_COEXISTENCE_SW */

#endif /* MCS_LIMITS_LOW_MEMORY */

}

/*=============================================================================

  FUNCTION:  lmtsmgr_process_chain_owner_input

=============================================================================*/
/*!
    @brief
    Processes any change in the chain owner inputs
 
    @details
    This should only be called when chain owner is updated in TRM
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_process_chain_owner_input()
{

#ifndef MCS_LIMITS_LOW_MEMORY

  wwcoex_conflict_tbls_type* conflict_tbl;
  lmtsmgr_power_info_type *pwr_info_t1, *pwr_info_t2;
  uint32 tech1_num_dlfreqs, tech2_num_dlfreqs, tbl_index;
  cxm_tech_type tech1 = CXM_TECH_DFLT_INVLD, tech_temp;
  cxm_tech_type tech2;
  lmtsmgr_tech_state_type* tech1_st, *tech2_st;
  int i, j;
  boolean multi_chain_active = FALSE;

  for (i=0; i<TRM_MAX_CHAINS; i++)
  {
     tech_temp = lmtsmgr.chain_owner[i];
     if (!LMTSMGR_IS_TECH_VALID(tech_temp))
     {
        continue;
     }
     else
     {
       tech1 = tech_temp;
       tech1_st = &lmtsmgr.tech_state[tech_temp];
       tech1_num_dlfreqs = tech1_st->currList.num_dl_entries;
     }

     for (j=i+1; j<TRM_MAX_CHAINS; j++)
     {
        tech2 = lmtsmgr.chain_owner[j];

        /* If the techs are on seperate chains, compare the frequencies */
        if (LMTSMGR_IS_TECH_VALID(tech2) && (tech1 != tech2))
        {
          LMTSMGR_DBG_MSG_4(MED, "Comparing index %x (CXM_TECH: %x) with index %x (CXM_TECH: %x)",
                            i, tech1, j, tech2);

           tech2_st = &lmtsmgr.tech_state[tech2];
           tech2_num_dlfreqs = tech2_st->currList.num_dl_entries;

           LMTSMGR_MSG_4(LOW, "Comparing Freqs for Techs %x (num_dl: %x) %x (num_dl: %x)",
                         tech1, tech1_num_dlfreqs, tech2, tech2_num_dlfreqs);

           if (tech1_num_dlfreqs > 0 && tech2_num_dlfreqs > 0 &&
               (ACTION_BACKOFF == wwcoex_get_mitigation_action(tech1, tech2)))
           {
             multi_chain_active = TRUE;
             conflict_tbl = wwcoex_get_table_node(tech1, tech2);
             if((conflict_tbl == NULL) || 
                (conflict_tbl->current_tbl == WWCOEX_TBL_INVALID))
             {
               /* Should never come here */
               LMTSMGR_MSG_2(FATAL, "Could not find a table node for techs %d %d",
                                     tech1, tech2);
               continue;
             }
             tbl_index = conflict_tbl->current_tbl;

             /* 1st arg should be > 2nd arg in compare_freq_techs */
             if (tech1 > tech2)
             {
               wwcoex_compare_freq_lists(tech1, tech2, &conflict_tbl->tables[tbl_index], TRUE);
             }
             else
             {
               wwcoex_compare_freq_lists(tech2, tech1, &conflict_tbl->tables[tbl_index], TRUE);
             }

             /* Ensure that power limits are only set for active techs*/
             pwr_info_t1 = &lmtsmgr.tech_state[tech1].pwr_info;
             pwr_info_t2 = &lmtsmgr.tech_state[tech2].pwr_info;
             for (i=0; i<CXM_TECH_MAX; i++)
             {
               if (i != tech2)
               {
                 pwr_info_t1->new_limits.coex_plimit[i] = DEFAULT_MAX_PLIMIT;
                 pwr_info_t1->new_limits.coex_slot_plimit[i] = DEFAULT_MAX_PLIMIT;
               }
               if (i != tech1)
               {
                 pwr_info_t2->new_limits.coex_plimit[i] = DEFAULT_MAX_PLIMIT;
                 pwr_info_t2->new_limits.coex_slot_plimit[i] = DEFAULT_MAX_PLIMIT;
               }
             }
             wwcoex_check_special_inds();
             lmtsmgr_notify_tech_plimit(tech1);
             lmtsmgr_notify_tech_plimit(tech2);
           }
        }
     }
  }

  /* If single tech is on, set limits to max and ensure filter path is not used. */
  if (!multi_chain_active && (tech1 != CXM_TECH_DFLT_INVLD))
  {
    LMTSMGR_MSG_1(LOW, "Resetting pwr limits for Tech: %d", tech1);

    lmtsmgr_set_tech_backoff_limits_max(tech1);

    lmgr_temp_ind_mask = 0;
    wwcoex_check_special_inds();
  }

#endif /* MCS_LIMITS_LOW_MEMORY */

}

/*=============================================================================

  FUNCTION:  lmtsmgr_process_band_avoidance_input

=============================================================================*/
/*!
    @brief
    Processes any change in the band avoidance inputs (serving frequency,
    serving power, blacklist response)
 
    @details
    This should only be called when serving frequency, serving power,
    or a blacklist response has been sent from Tech L1
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_process_band_avoidance_input(void)
{

  cxm_tech_type                tech1 = CXM_TECH_DFLT_INVLD, tech2 = CXM_TECH_DFLT_INVLD;
  cxm_tech_type                hprio_tech, lprio_tech, tech_pos;
  lmtsmgr_core_prio_table_type tech1_prio_tbl,tech2_prio_tbl;
  uint32                       num_active_techs = 0, blist_id;
  lmtsmgr_ba_info_type         *ba_state;
  lmtsmgr_tech_ba_type         *hprio_ba_info, *lprio_ba_info;
  coex_ba_blist_data           new_blacklist;

  #ifdef TEST_FRAMEWORK
  #error code not present
#endif

  ba_state = &lmtsmgr.band_avoidance;
  memset(&new_blacklist, 0, sizeof(coex_ba_blist_data));

  /* Only enable feature if in DSDA mode and enable mask is set */
  if (!IS_WWAN_BAND_AVOIDANCE_ENABLED(lmtsmgr.en_mitigation_mask) || 
      LMTSMGR_IS_SGLTE_FEATURE_MODE() ||
      LMTSMGR_IS_DRDSDS_ENABLED())
  {
    LMTSMGR_MSG_2(MED, "Band Avoidance is disabled 0x%x (Device Mode: 0x%x)",
                  lmtsmgr.en_mitigation_mask, lmtsmgr.device_mode);

    //TODO: Corner case when BA is disabled but tech received BList
    return;
  }

  /* Only two techs should be in connected mode */
  for (tech_pos=CXM_TECH_LTE; tech_pos<CXM_TECH_MAX; tech_pos++)
  {
    if(ba_state->tech_info[tech_pos].num_entries > 0)
    {
      if (tech1 == CXM_TECH_DFLT_INVLD)
      {
        tech1 = tech_pos;
      }
      else //tech2 == CXM_TECH_DFLT_INVLD
      {
        tech2 = tech_pos;
      }
      num_active_techs++;
    }
  }

  if (num_active_techs > 2)
  {
    LMTSMGR_MSG_1(ERROR, "ERROR: Only two techs should be registered for Band Avoidance!",
                  num_active_techs);
    return;
  }
  else if (num_active_techs == 1) 
  {
    /* send empty blacklist to only active tech */
    if (FALSE == wwcoex_ba_comp_blacklists(&ba_state->currBlacklist, &new_blacklist))
    {
      /* Keep blacklist ID */
      blist_id = ba_state->currBlacklist.blacklist_id+1;
      memset(&ba_state->currBlacklist, 0, sizeof(coex_ba_blist_data));
      ba_state->currBlacklist.blacklist_id = blist_id;

      wwcoex_log_blacklist(tech1);
      lmtsmgr_send_blacklist_info(tech1, TRUE); /* Don't care but setting TRUE
                                                   will save memset call */
    }
    return;
  }
  else if ((tech1 == CXM_TECH_DFLT_INVLD) || (tech2 == CXM_TECH_DFLT_INVLD))
  {
    LMTSMGR_MSG_0(ERROR, "ERROR: Two techs required for Band Avoidance!");
    return;
  }

  /* Determine priority of two techs */
  tech1_prio_tbl.tech = tech1;
  tech2_prio_tbl.tech = tech2;
  (void)lmtsmgr_core_get_priority (&tech1_prio_tbl, &tech2_prio_tbl);
  if (LMTSMGR_TIER_1 == tech1_prio_tbl.pri)
  {
    hprio_tech = tech1;
    lprio_tech = tech2;
  }
  else if (LMTSMGR_TIER_2 == tech1_prio_tbl.pri)
  {
    hprio_tech = tech2;
    lprio_tech = tech1;
  }
  else
  {
    LMTSMGR_MSG_3(FATAL,"Tech %d has invalid tier %d.. ignoring band avoidance with tech %d",
                   tech1, tech1_prio_tbl.pri, tech2);
    return;
  }

  hprio_ba_info = &ba_state->tech_info[hprio_tech];
  lprio_ba_info = &ba_state->tech_info[lprio_tech];

  /* Do not send blacklist under the following cirumstances:
        - No problem is received from either tech
        - If blacklist response is old
  */
  if (((lprio_ba_info->ba_rsp_changed) &&
      ((lprio_ba_info->blist_rsp.bl_id != ba_state->currBlacklist.blacklist_id) ||
      (lprio_ba_info->blist_rsp.response == FREQ_AVOID_NO_PROBLEM))) ||
      ((hprio_ba_info->ba_rsp_changed) &&
      ((hprio_ba_info->blist_rsp.bl_id != ba_state->currBlacklist.blacklist_id) ||
      (hprio_ba_info->blist_rsp.response == FREQ_AVOID_NO_PROBLEM))))
  {
    /* Do nothing */
  }
  /* 
     Send blacklist to high priority tech under following circumstances:
        - When low priority tech NACK'd
        - Blacklist has been already sent to hprio tech, low priority frequency
          has not been updated, and there is power update by either tech 
  */
  else if ( ( (lprio_ba_info->blist_rsp.response == FREQ_AVOID_NACK) &&
              (lprio_ba_info->blist_rsp.bl_id == ba_state->currBlacklist.blacklist_id)) ||
            ( (lmtsmgr.band_avoidance.currBlTech == hprio_tech) && 
              (lprio_ba_info->freq_changed != TRUE) &&
              ( (hprio_ba_info->power_changed == TRUE) || 
                (lprio_ba_info->power_changed == TRUE) ) ) )
  {
    /* Send blacklist to higher priority tech */
    wwcoex_ba_gen_blacklist(hprio_tech, hprio_ba_info, lprio_tech, lprio_ba_info,
                            &new_blacklist);

    /* Force the high priority tech to change serving frequency */
    new_blacklist.serv_freq_state = SERV_FREQ_AVOID_REQ;

    /* Mode should be set to opposite of what lprio tech was sent
       (regardless of threshold crossing) */
    new_blacklist.serv_freq_mode = 
      (lprio_ba_info->serv_desense_mode == DESENSE_AGGR) ? DESENSE_VICTIM :
      (lprio_ba_info->serv_desense_mode == DESENSE_VICTIM) ? DESENSE_AGGR :
      lprio_ba_info->serv_desense_mode;

    /* If the new and current blacklist entries are different, copy
       new blacklist to data and send to appropriate tech */
    if(FALSE == wwcoex_ba_comp_blacklists(&ba_state->currBlacklist, &new_blacklist))
    {
      /* Increment blacklist id */
      new_blacklist.blacklist_id = ba_state->currBlacklist.blacklist_id+1;
      memscpy(&ba_state->currBlacklist, sizeof(coex_ba_blist_data),
              &new_blacklist, sizeof(coex_ba_blist_data));

      /* Empty blacklist NOT sent to lprio_tech as it should attempt to 
         perform BA regardless */

      wwcoex_log_blacklist(hprio_tech);
      lmtsmgr_send_blacklist_info(hprio_tech, FALSE);
    }

    lmtsmgr.band_avoidance.currBlTech = hprio_tech;
  }
  else
  {
    /* Send blacklist to lower priority tech */
    wwcoex_ba_gen_blacklist(lprio_tech, lprio_ba_info, hprio_tech, hprio_ba_info,
                            &new_blacklist);

    /* If the new and current blacklist entries are different, copy
       new blacklist to data and send to appropriate tech */
    if(FALSE == wwcoex_ba_comp_blacklists(&ba_state->currBlacklist, &new_blacklist))
    {
      /* Increment blacklist id */
      new_blacklist.blacklist_id = ba_state->currBlacklist.blacklist_id+1;

      memscpy(&ba_state->currBlacklist, sizeof(coex_ba_blist_data),
              &new_blacklist, sizeof(coex_ba_blist_data));

      /* Store desense mode */
      lprio_ba_info->serv_desense_mode = new_blacklist.serv_freq_mode;

      /* Send empty blacklist to hprio_tech if NACK'd */
      if (hprio_ba_info->blist_rsp.response == FREQ_AVOID_NACK)
      {
        LMTSMGR_MSG_2(MED, "Sending empty blacklist to Tech %d (ID: %d)", 
                       hprio_tech, new_blacklist.blacklist_id);
        lmtsmgr_send_blacklist_info(hprio_tech, TRUE);

        /* Increment blacklist ID again */
        new_blacklist.blacklist_id++;
      }

      wwcoex_log_blacklist(lprio_tech);
      lmtsmgr_send_blacklist_info(lprio_tech, FALSE);
    }
 
    lmtsmgr.band_avoidance.currBlTech = lprio_tech;
  }

  /* Reset flags */
  lprio_ba_info->ba_rsp_changed = FALSE;
  hprio_ba_info->ba_rsp_changed = FALSE;
}

/* Return the first tech besides G which is active... */
cxm_tech_type lmtsmgr_get_active_x_tech(void )
{
  cxm_tech_type i; 
  for (i=0;i<CXM_TECH_MAX; i++)
  {
    if ((lmtsmgr.tech_state[i].currList.num_entries > 0) && (!IS_GSM_TECH(i)))
    {
      /* Found tech active */
      LMTSMGR_MSG_1(MED, "Found tech %d active", i);
      break;
    }
  }

  return i;

}
/*=============================================================================

  FUNCTION:  lmtsmgr_process_slot_lvl_act_input

=============================================================================*/
/*!
    @brief
    Determines the conflict type for each Tx/Rx activity slot reported and
    sends this information (along with the original Tx/Rx info) to other active
    tech
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_process_slot_lvl_act_input(boolean is_sim_input)
{
  trm_get_info_input_type trm_info_in;
  trm_get_info_return_type trm_info_out;
  cxm_tech_type send_tech, rec_tech = CXM_TECH_MAX;
  cxm_timing_info_s* tech_timing_info;
  cxm_timing_slot_entry* slot_entry;
  cxm_action_query_s query_st;
  uint8 j, send_tech_chain = TRM_MAX_CHAINS;
  coex_freqid_link_s send_freq_info;
  lmtsmgr_tech_list_type* rec_tech_info;
  wwcoex_nonim_freqcompare_in  nonim_comp_in;
  wwcoex_nonim_freqcompare_out  nonim_comp_out;
  wwcoex_im_freqcompare_in      im_comp_in;
  wwcoex_im_freqcompare_out     im_comp_out;
  uint32 index, ul_size = 0, dl_size = 0;
  uint16 freqid_ul_list[CXM_MAX_SUPPORTED_LINK_SETS/2];
  uint16 freqid_dl_list[CXM_MAX_SUPPORTED_LINK_SETS/2], conf_freq_id;

  /* index already incremented in copying timing info */
  tech_timing_info = &lmtsmgr.tech_timing_info[lmtsmgr.tech_timing_info_index-1];

  /* If Single-Tx DSDA setting is unknown */
  if ( lmtsmgr.trm_single_tx_dsda == TRM_SINGLE_TX_DSDA_MAX)
  {
    /* Retrieve Single-TX DSDA setting */
    trm_info_in.info_type = TRM_SINGLE_TX_CAPABILITY;
    trm_info_in.client_id = 0;
    trm_get_info(&trm_info_in, &trm_info_out);
    LMTSMGR_ASSERT(trm_info_out.info_type == TRM_SINGLE_TX_CAPABILITY);
    lmtsmgr.trm_single_tx_dsda = trm_info_out.info.single_tx_info.info;
  }

  if (!is_sim_input)
  {
    /* Only want to process in Single TX DSDA mode */
    if (!LMTSMGR_IS_SINGLE_TX_DSDA_ENABLED() || 
        tech_timing_info->tech_id >= CXM_TECH_MAX)
    {
      return;
    }

    /* Determine TRM chain of tech that sent */
    send_tech = tech_timing_info->tech_id;
    for (index = 0; index<TRM_MAX_CHAINS; index++)
    {
      if(lmtsmgr.chain_owner[index] == send_tech)
      {
        send_tech_chain = index;
        break;
      }
    }
    
    /* Determine tech on other WTR */
    for (index=0; index<TRM_MAX_CHAINS; index++)
    {
      if(lmtsmgr.chain_owner[index] != CXM_TECH_DFLT_INVLD &&
         lmtsmgr.chain_owner[index] != CXM_TECH_MAX &&
         send_tech_chain != index &&
         lmtsmgr.chain_owner[index] != send_tech)
      {
        rec_tech = lmtsmgr.chain_owner[index];
        break;
      }
    }
  }
  else
  {
    LMTSMGR_MSG_0(MED, "DEBUG: Simulator enabled..");
    /* 
       Simulator is active so determine receiving tech
       based on following assumptions:

       GSM tech is the only tech sending
       Receiving tech is on chain 0 */

    /* If tech on chain 0 is anything but GSM */

    /* Determine TRM chain of tech that sent */
    send_tech = tech_timing_info->tech_id;

    /* TODO: Remove this check after bringup */
    if (send_tech != CXM_TECH_GSM2)
    {
      LMTSMGR_MSG_1(ERROR, "Invalid tech %d sent the timeline message", send_tech);
      send_tech = CXM_TECH_GSM2;
    }

    rec_tech = lmtsmgr_get_active_x_tech();
    send_tech_chain = TRM_GRANTED_CHAIN2; /* Bypass conditional check below */
  }

  if ((send_tech_chain == TRM_MAX_CHAINS) ||
      (rec_tech == CXM_TECH_MAX) ||
      (rec_tech == CXM_TECH_DFLT_INVLD))
  {
    /* Either sending tech provided timeline before
       acquiring TRM chain or there is no receiving tech*/
    LMTSMGR_MSG_2(MED, "DEBUG: Invalid tech %d or chain %d!", 
                  rec_tech, send_tech_chain);
    return; 
  }

  /* Get recieving tech's frequency info */
  rec_tech_info = &lmtsmgr.tech_state[rec_tech].currList;

  if (rec_tech_info->num_entries == 0)
  {
    return;
  }

  /* Generate seperate Ul/Dl list of freqids of receiving tech */
  for (index = 0; index<rec_tech_info->num_entries; index++)
  {
    if (rec_tech_info->links[index].freqInfo.link_info.direction == CXM_LNK_DRCTN_DL)
    {
      freqid_dl_list[dl_size] = (uint16)rec_tech_info->links[index].freqInfo.freqid;
      dl_size++;
    }
    else if (rec_tech_info->links[index].freqInfo.link_info.direction == CXM_LNK_DRCTN_UL)
    {
      freqid_ul_list[ul_size] = (uint16)rec_tech_info->links[index].freqInfo.freqid;
      ul_size++;
    }
  }

  /* Determine the potential conflict of tech sending tech's
     activity slot based on the receiving tech frequency info */
  for (index=0; index<tech_timing_info->num_entries; index++)
  {
    slot_entry = &tech_timing_info->slot_list[index];
    slot_entry->conflict_mask = 0;

    /* Other tech scheduling Tx slot will result in a channel
       conflict with Tx in Single-TX DSDA mode */
    if ((slot_entry->direction == CXM_LNK_DRCTN_UL) &&
        LMTSMGR_IS_SINGLE_TX_DSDA_ENABLED())
    {
      slot_entry->conflict_mask |= (1 << CXM_TL_CONFLICT_CHANNEL_TX);
    }

    if (slot_entry->freqid != CXM_UNKNOWN_FREQID)
    {
      switch (slot_entry->direction)
      {
        case CXM_LNK_DRCTN_UL:
        {
          query_st.num_conflicting_ids = 1;
		  conf_freq_id = (uint16)slot_entry->freqid;
          query_st.conflicting_ids = &conf_freq_id;
          query_st.conflicting_activity = ACTIVITY_TX;

          //TODO: Ignore power monitors when querying

          /* Query with all Rx frequencies of receiving tech */
          LMTSMGR_ASSERT(dl_size == rec_tech_info->num_dl_entries);
          query_st.num_requesting_ids = (rec_tech_info->num_dl_entries >= WWCOEX_MAX_REQUESTING_IDS) ? 
                                        WWCOEX_MAX_REQUESTING_IDS : rec_tech_info->num_dl_entries;
          query_st.requesting_ids = &freqid_dl_list[0];
          query_st.requesting_activity = ACTIVITY_RX;
          
          if(TRUE == cxm_query_action(&query_st))
          {
            /* Decrypt the action mask to find associated action type*/
            if (query_st.actions != ACTION_NONE)
            {
              /* If action type is other than none for any id, then there is desense issue*/
              slot_entry->conflict_mask |= (1 << CXM_TL_CONFLICT_DESENSE_RX);
              break;
            }
          }
          else
          {
            LMTSMGR_MSG_3(ERROR, "Failure in cxm_query_action! Send Tech: %d Dir: %d Rec Tech: %d",
                          send_tech, slot_entry->direction, rec_tech);
          }
        }
        break;

        case CXM_LNK_DRCTN_DL:
        {
          query_st.num_conflicting_ids = 1;
		  conf_freq_id = (uint16)slot_entry->freqid;
          query_st.conflicting_ids = &conf_freq_id;
          query_st.conflicting_activity = ACTIVITY_RX;

          //TODO: Ignore power monitors when querying

          /* Query with all Tx frequencies of receiving tech */
          LMTSMGR_ASSERT(ul_size == rec_tech_info->num_ul_entries);
          query_st.num_requesting_ids = (rec_tech_info->num_ul_entries >= WWCOEX_MAX_REQUESTING_IDS) ? 
                                         WWCOEX_MAX_REQUESTING_IDS : rec_tech_info->num_ul_entries;
          query_st.requesting_ids = &freqid_ul_list[0];
          query_st.requesting_activity = ACTIVITY_TX;
          
		  /* No need to query with Send tech Rx Rec Tech Tx conflict
		     if rec tech is in idle */
          if(query_st.num_requesting_ids == 0)
            break;

          if(TRUE == cxm_query_action(&query_st))
          {
            if (query_st.actions != ACTION_NONE)
            {
              /* If action type for any id is other than none, then there is desense issue */
              slot_entry->conflict_mask |= (1 << CXM_TL_CONFLICT_DESENSE_TX);
              break;
            }
          }
          else
          {
            LMTSMGR_MSG_3(ERROR, "Failure in cxm_query_action! Send Tech: %d Dir: %d Rec Tech: %d",
                          send_tech, slot_entry->direction, rec_tech);
          }
        }
        break;

        default: /* Direction can only be UL OR DL */
        {          
          LMTSMGR_MSG_3(ERROR, "Invalid direction %d for tech %d timeline index %!",
                        slot_entry->direction, send_tech, index);
        }

      } /* end switch */
    }
    else /* slot_entry->freqid == CXM_UNKNOWN_FREQID */
    {
      memset(&send_freq_info, 0, sizeof(coex_freqid_link_s));
      send_freq_info.link_info.band = slot_entry->band;
      send_freq_info.link_info.bandwidth = slot_entry->bandwidth;
      send_freq_info.link_info.direction = slot_entry->direction;
      send_freq_info.link_info.frequency = slot_entry->frequency;
      send_freq_info.freqid = CXM_UNKNOWN_FREQID;
      //TODO: Add channel to interface when scope expands to SGLTE/SGTDS[+G]

      switch (slot_entry->direction)
      {
        case CXM_LNK_DRCTN_UL:
        {
          nonim_comp_in.atech = send_tech;
          nonim_comp_in.vtech = rec_tech;
          nonim_comp_in.atulfreqinfo = send_freq_info;

          im_comp_in.atech = send_tech;
          im_comp_in.vtech = rec_tech;
          im_comp_in.atulfreqinfo = send_freq_info;

          /* For every known receiving tech DL.. */
          for (j = 0; j < rec_tech_info->num_dl_entries; j++)
          {
            if (rec_tech_info->links[rec_tech_info->dl_freqList[j]].freqInfo.link_info.type 
                == CXM_LNK_TYPE_POWER_MONITOR)
            {
              /* ignore power monitors */
              continue;
            }
            nonim_comp_in.vtdlfreqinfo = rec_tech_info->links[rec_tech_info->dl_freqList[j]].freqInfo; 
            im_comp_in.vtulfreqinfo = rec_tech_info->links[rec_tech_info->ul_freqList[0]].freqInfo; /* Tech should only have one Tx */
            im_comp_in.vtdlfreqinfo = rec_tech_info->links[rec_tech_info->dl_freqList[j]].freqInfo;

            nonim_comp_out.desense_mask = NO_DESENSE; 
            im_comp_out.desense_mask = NO_DESENSE;
             
            /* Perform frequency comparison with sending tech's Tx */
            wwcoex_nonim_compare_freqs(&nonim_comp_in, &nonim_comp_out);
            wwcoex_im_compare_freqs(&im_comp_in, &im_comp_out);
            
            if (((nonim_comp_out.desense_mask != NO_DESENSE) && (nonim_comp_out.special_ind_mask == 0)) ||
                (im_comp_out.desense_mask != NO_DESENSE))
            {
              /* If action type is other than none, then there is potential desense issue;
                 there are no power values to compare with */
              slot_entry->conflict_mask |= (1 << CXM_TL_CONFLICT_POT_DESENSE_RX);

              /* No need to compare all frequencies */
              break;
            }
          }
          break;

        }

        case CXM_LNK_DRCTN_DL:
        {
          nonim_comp_in.atech = rec_tech;
          nonim_comp_in.vtech = send_tech;
          nonim_comp_in.atulfreqinfo = rec_tech_info->links[rec_tech_info->ul_freqList[0]].freqInfo; /* Tech should only have one Tx */
          nonim_comp_in.vtdlfreqinfo = send_freq_info;
          nonim_comp_out.desense_mask = NO_DESENSE; 
            
          /* Perform frequency comparison with sending tech's Rx;
             CANNOT perform IM as we have no UL information */
          wwcoex_nonim_compare_freqs(&nonim_comp_in, &nonim_comp_out);
         
          if ((nonim_comp_out.desense_mask != NO_DESENSE) && (nonim_comp_out.special_ind_mask == 0))
          {
            /* If action type is other than none, then there is potential desense issue;
               there are no power values to compare with */
            slot_entry->conflict_mask |= (1 << CXM_TL_CONFLICT_POT_DESENSE_TX);
          }
          break;

        }

        default: /* Direction can only be UL OR DL */
        {          
          LMTSMGR_MSG_3(ERROR, "Invalid direction %d for tech %d timeline index %!",
                        slot_entry->direction, send_tech, index);
        }   
         
      } /* end switch */

    } /* end else*/

  } /* End for */

  /* Log packet */
  wwcoex_log_sl_act_tl(rec_tech);

  lmtsmgr_send_sl_activity_info(rec_tech, &lmtsmgr.tech_timing_info[lmtsmgr.tech_timing_info_index-1]);
}

/*=============================================================================

  FUNCTION:  lmtsmgr_core_deinit

=============================================================================*/
/*!
    @brief
    De-inits SP and any other memory allocations.
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_core_deinit(void)
{
  /* Deregister SP client */
  sp_deregister_client(SP_SAR);

  /* Stop QMI service */
  sar_dsi_service_deinit();

  /* Deinit sar efs*/
  sar_efs_deinit();

  /* Deinit wwcoex efs*/
  wwcoex_efs_deinit();
}


/*=============================================================================

  FUNCTION:  lmtsmgr_get_macro_state

=============================================================================*/
/*!
    @brief
    This function extracts the macro state for a tech
 
    @return
    None
*/
/*===========================================================================*/
sp_subs_states_enum_t lmtsmgr_get_macro_state
(
  cxm_tech_type     tech
)
{
  sp_subs_states_enum_t macro_state;
  sp_subscription_priority_list*  sp_prio_list;
  cxm_tech_type  other_tech;
  uint8 stack_idx,idx;
  sys_modem_as_id_e_type asid;
  boolean tx_active;

  sp_prio_list = &lmtsmgr.subs_prio_list;
  asid = lmtsmgr.tech_state[tech].sub_id;
  tx_active = (lmtsmgr.tech_state[tech].currList.num_ul_entries > 0) ? TRUE : FALSE;

  /* Setting up of macro State for the tech... lets start with baseline */
  macro_state = SP_BASELINE_STATE; 
  
  #ifdef FEATURE_LMTSMGR_SAME_SUB_PRIO
  if ((tx_active == TRUE) &&
      ( (lmtsmgr.device_mode == 0) || 
        (sp_prio_list->active_tech_info.multi_sim_config.max_standby_subscriptions == 1) ) )
  {
    /* If tech is in Tx and its single sim mode, then assume fixed
       prios for SAR, coex */
    if (lmtsmgr_core_get_same_sub_priority(tech) == LMTSMGR_TIER_2)
    {
      macro_state = SP_DATA_ONLY_STATE;
    }
    else
    {
      macro_state = SP_VOICE_HIGH_PRIORITY_STATE;
    }

    LMTSMGR_DBG_MSG_2(HIGH, "Using default macro state %d for tech %d",
                macro_state, tech);					
  }
  else
  #endif /* FEATURE_LMTSMGR_SAME_SUB_PRIO */ 
  {
    /* Find the position of the SUB in the prio order list */
    for (idx=0;idx<sp_prio_list->num; idx++)
    {
      if (sp_prio_list->order[idx].sub == asid)
      {
        /* Found the SUB */
        break;
      }  
    }
  
    if (idx == sp_prio_list->num)
    {
      /* Could not find the SUB in the list */
      macro_state = SP_BASELINE_STATE;	
    }
    /* Found SUB in the list: Check number of active stacks */
    else if (sp_prio_list->order[idx].num_active_techs == 0)
    {
      if (tx_active == TRUE)  
      {
        /* should never reach here */
        LMTSMGR_MSG_3(FATAL, "Num techs active is 0 for idx %d, asid %d, state %d in prio order",
                      idx, sp_prio_list->order[idx].sub, sp_prio_list->order[idx].state );
      }
    }
    else if (sp_prio_list->order[idx].num_active_techs == 1)
    {
      /* If tech Tx is active, then just use the macro state...
         no need to check for techid */
      if ( tx_active == TRUE )
      {
        macro_state = sp_prio_list->order[idx].tech_state[0].state;
      }
      else
      {
        /* Here there can be 2 cases... either IRAT handover is in progress
           or the tech is in baseline (idle) */
        other_tech = lmtsmgr_trans_sys_id_to_cxm_client(sp_prio_list->order[idx].tech_state[0].mode, asid);
        if (!LMTSMGR_IS_TECH_VALID(other_tech))
        {
          /* should not come here */
          LMTSMGR_MSG_3(FATAL, "Incorrect tech %d for sys mode %d, asid %d",
                  other_tech, sp_prio_list->order[idx].tech_state[0].mode, asid);
        }
        else
        {
          /* If this other tech has num ul entries > 0, that means it is active so this
             current Tx inactive tech should be in BASELINE,
             if the other tech has no UL entry then it must be in the IRAT handover phase */
          if (lmtsmgr.tech_state[other_tech].currList.num_ul_entries == 0)
          {
            macro_state = sp_prio_list->order[idx].tech_state[0].state;
          }
        }
      }
    }
    else /* More than 1 tech is Tx active on this SUB... */
    {
      for (stack_idx = 0; stack_idx < sp_prio_list->order[idx].num_active_techs; stack_idx++)
      {
        if (sp_prio_list->order[idx].tech_state[stack_idx].mode == lmtsmgr_trans_cxm_to_sys_id(tech))
        {
          /* Found the tech */
          break;
        }
      }
 
      if (stack_idx < sp_prio_list->order[idx].num_active_techs)
      {
        /* Found the tech in the list */
        macro_state = sp_prio_list->order[idx].tech_state[stack_idx].state;
      }
      else
      {
        /* Tech is active but cannot find in the list... should not happen */
        LMTSMGR_MSG_3(ERROR, "Tech %d has Tx %d on sub %d but not in prio list",
                        tech, tx_active, asid);
      }
    }
  }    

  return macro_state;
}
/*=============================================================================

  FUNCTION:  lmtsmgr_process_coex_tech_state_change

=============================================================================*/
/*!
    @brief
    This function processes any change in tech state and broadcasts an
    indication.

    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_process_coex_tech_state_change (void)
{
  cxm_tech_type tech;
  cxm_coex_tech_state_bcast_ind_s    tech_st_msg;
  cxm_coex_tech_data  temp_coex_st,*tech_coex_st, *tech1_coex_st, *tech2_coex_st;
  uint8 num_active_techs = 0, i, j;
  lmtsmgr_tech_state_type *tech_st;
#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY
  trm_asdiv_tech_state_info_type tech_st_for_trm;
#endif

  TTLMARKER(LMTSMGR_PCTSC_IN);
  
  memset((void *)&tech_st_msg, 0, sizeof(cxm_coex_tech_state_bcast_ind_s));

  for (tech=0; (tech<CXM_TECH_MAX) && (num_active_techs<CXM_MAX_CONCURRENT_TECHS); tech++)
  {
    tech_st = &lmtsmgr.tech_state[tech];

    /* If there are no serving frequencies, skip this tech... */
    if ( (tech_st->currList.num_entries == 0) ||
         (tech_st->currList.num_serving_dl_entries == 0) )
    {
      continue;
    }

    /* fill up the state information to tech_st_msg */
    tech_coex_st = &tech_st_msg.tech_state[num_active_techs];

    tech_coex_st->tech_id = tech;
    tech_coex_st->rx_active = TRUE;
    tech_coex_st->distress_state = tech_st->distress_state;
    tech_coex_st->as_id = tech_st->sub_id;

    /* Check UL information */
    if (tech_st->currList.num_ul_entries > 0)
    {
      tech_coex_st->tx_active = TRUE;

      /* for now there can be only 1 UL carrier */
      tech_coex_st->num_ul_carriers = 1;
      tech_coex_st->ul_bands[0] = 
        tech_st->currList.links[tech_st->currList.ul_freqList[0]].freqInfo.link_info.band;
    }
    else
    {
      tech_coex_st->tx_active = FALSE;
      tech_coex_st->num_ul_carriers = 0;
    }

    /* Check DL information */
    tech_coex_st->num_dl_carriers = 0;

    /* Copy the DL carrier bands */
    for (i=0; i<tech_st->currList.num_dl_entries; i++)
    {
      /* Ignore non-serving frequencies */
      if (tech_st->currList.links[tech_st->currList.dl_freqList[i]].freqInfo.link_info.type > 0)
      {
        continue;
    }

      if (tech_coex_st->num_dl_carriers >=CXM_MAX_CARRIERS)
        {
          /* should not come here */
          LMTSMGR_MSG_3(ERROR, "Tech %d num serving dl carriers %d more than expected %d",
                      tech, tech_st->currList.num_serving_dl_entries, CXM_MAX_CARRIERS);
          break;
        }

      tech_coex_st->dl_bands[tech_coex_st->num_dl_carriers] = 
        tech_st->currList.links[tech_st->currList.dl_freqList[i]].freqInfo.link_info.band;

      tech_coex_st->num_dl_carriers++;

      /* For GSM tech, there can be only one carrier so bail here to avoid adding
         same bands in freq hopping scenario */
      if (IS_GSM_TECH(tech))
      {
        break;
      }
    }

    tech_coex_st->macro_state = tech_st->macro_state;
                                                        
    num_active_techs++;
  }
  tech_st_msg.num_active_techs = num_active_techs;

  TTLMARKER(LMTSMGR_PCTSC_TECHLOOP_DONE);
  
  /* Sort the list based on macro state... */
  for (i=0; i<num_active_techs-1; i++)
  {
    tech1_coex_st = &tech_st_msg.tech_state[i];

    j=i+1;
    while (j < num_active_techs)
    {
      tech2_coex_st = &tech_st_msg.tech_state[j];

      /* If tech 2 has higher macro state or if the states are same but tech 2
         is on a higher prio SUB, then swap the states */
      if ( (tech1_coex_st->macro_state < tech2_coex_st->macro_state) ||
           ( (tech1_coex_st->macro_state == tech2_coex_st->macro_state) &&
             (tech2_coex_st->as_id == lmtsmgr.subs_prio_list.order[0].sub) ) )
      {
        /* Swap the 2 techs */
        memscpy(&temp_coex_st, sizeof(cxm_coex_tech_data),tech1_coex_st, sizeof(cxm_coex_tech_data));
        memscpy(tech1_coex_st, sizeof(cxm_coex_tech_data),tech2_coex_st, sizeof(cxm_coex_tech_data));
        memscpy(tech2_coex_st, sizeof(cxm_coex_tech_data),&temp_coex_st, sizeof(cxm_coex_tech_data));
      }
      j++;
    }
  }

  TTLMARKER(LMTSMGR_PCTSC_SORT_DONE);
  
  /* Check if the state has changed from previous */

  /* Send the indication message... */
  lmtsmgr_msgr_send_msg( &tech_st_msg.msg_hdr,
                           MCS_CXM_COEX_TECH_STATE_BCAST_IND,
                           sizeof(cxm_coex_tech_state_bcast_ind_s) );

  TTLMARKER(LMTSMGR_PCTSC_MSGR_SENT);
  
  LMTSMGR_DBG_MSG_1(MED, "Coex tech st ind: num_active_techs %d", num_active_techs);

  /* Update ASDIV about the state */
#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY
  tech_st_for_trm.num_active_techs = num_active_techs;
#endif /* Antenna Switch Div feature */

  for (i=0; i<num_active_techs; i++)
  {
#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY
    tech_st_for_trm.tech_state[i].client_id = 
            lmtsmgr_trans_cxm_to_trm_client(tech_st_msg.tech_state[i].tech_id);

    tech_st_for_trm.tech_state[i].as_id = tech_st_msg.tech_state[i].as_id;

    tech_st_for_trm.tech_state[i].macro_state = tech_st_msg.tech_state[i].macro_state;
#endif /* Antenna Switch Div feature */
    LMTSMGR_DBG_MSG_6(MED, "Coex tech st ind: [%d] = Tech %d, as_id %d, state %d, tx %d, rx %d, mask 0x%x",
                  i, tech_st_msg.tech_state[i].tech_id,
                  tech_st_msg.tech_state[i].as_id,
                  tech_st_msg.tech_state[i].macro_state,
                  tech_st_msg.tech_state[i].tx_active,
                  tech_st_msg.tech_state[i].rx_active );

    LMTSMGR_DBG_MSG_6(MED, "Coex tech st ind: [%d] = distress_st %d, ul_bands[0]=%d, dl_bands[0]=%d, num_ul_bands=%d, num_dl_bands=%d",
                  i, 
                  tech_st_msg.tech_state[i].distress_state,
                  tech_st_msg.tech_state[i].ul_bands[0],
                  tech_st_msg.tech_state[i].dl_bands[0],
                  tech_st_msg.tech_state[i].num_ul_carriers,
                  tech_st_msg.tech_state[i].num_dl_carriers );
  }
#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY
  trm_asdiv_update_tech_state_info(&tech_st_for_trm);
#endif /* Antenna Switch Div feature */
 
  TTLMARKER(LMTSMGR_PCTSC_OUT);
}

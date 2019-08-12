
/*!
  @file
  sar_core.c

  @brief
  This file implements the SAR core functions of LIMTSMGR module.

  @ingroup per_implementation
*/

/*=============================================================================

  Copyright (c) 2014-15 Qualcomm Technologies Incorporated.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/sar/src/sar_core.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/10/16   jm      Reducing F3 messages in Limits Manager
01/06/15   rj      Move Translation functions in Limitsmgr to a seperate file
11/04/14   ag      Added TTL markers for profiling
10/27/14   rj      Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "mcs_variation.h"
#include "comdef.h"
#include "stringl.h"
#include "sys.h"

#include "cxm.h"
#include "lmtsmgr_task.h"
#include "lmtsmgr_i.h"
#include "lmtsmgr_efs.h"
#include "lmtsmgr_msgr.h"
#include "batt_efs.h"
#include "batt_npa.h"

#include "sar_i.h"
#include "wwan_coex.h"
#include "lmtsmgr_translation.h"

/*=============================================================================

                            TYPEDEFS

=============================================================================*/

/*=============================================================================

                         INTERNAL VARIABLES

=============================================================================*/

/* Priority table: The content shall be aligned in ascending order of pri at 
   all times */
typedef struct {
  lmtsmgr_core_prio_table_type tier1[LMTSMGR_MAX_TIER_TECHS];
  uint32  num_tier1_entries;
  lmtsmgr_core_prio_table_type tier2[LMTSMGR_MAX_TIER_TECHS];
  uint32 num_tier2_entries;
} sar_core_prio_tier_type;

sar_core_prio_tier_type sar_core_tiers;


/*=============================================================================

                                FUNCTIONS

=============================================================================*/

/*=======================================================================
FUNCTION sar_core_clear_table
    
DESCRIPTION
  This function will clear tier table

DEPENDENCIES
  None
    
RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
static void sar_core_clear_table(void)
{
  int i;

  for (i=0; i<LMTSMGR_MAX_TIER_TECHS; i++)
  {
    sar_core_tiers.tier1[i].tech = CXM_TECH_MAX;
    sar_core_tiers.tier2[i].tech = CXM_TECH_MAX;
  }
  sar_core_tiers.num_tier1_entries = 0;
  sar_core_tiers.num_tier2_entries = 0;
}


/*=======================================================================
FUNCTION sar_core_max_pwr_backoff
    
DESCRIPTION
  This function will set MAX Pwr Backoff value

DEPENDENCIES
  None
    
RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
void sar_core_max_pwr_backoff(int16 *sar_plimit, uint8* num_valid_sar, cxm_tech_type tech)
{
  uint8 index = 0;

  /* If tech is GSM, SAR pwr limit should be sent for array of 10 */
  if (IS_GSM_TECH(tech))
  {
    for (index = 0; index < SAR_GSM_MAX_LIMIT_TYPE; index++)
    {
      sar_plimit[index] = DEFAULT_MAX_PLIMIT;
    }
    *num_valid_sar = SAR_GSM_MAX_LIMIT_TYPE;
  }
  else
  {
    sar_plimit[index] = DEFAULT_MAX_PLIMIT;
    *num_valid_sar = 1;
  }

}

/*=======================================================================
FUNCTION sar_core_insert_rec_in_tier
    
DESCRIPTION
  This function will insert record in correct tier

DEPENDENCIES
  None
    
RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
static void sar_core_insert_rec_in_tier
(
  lmtsmgr_core_prio_table_type tier[],
  uint32 *num_entries,
  lmtsmgr_core_prio_table_type* new_rec  
)
{
  uint32 pos;

  /* Move clients with a lower priority one position backwards */
  for ( pos = *num_entries ;  pos > 0 ;  pos-- )
  {
    /* Shift backwards one position in list, to current position */
    tier[ pos ] = tier[pos - 1];
  }

  /* Insert client in the newly created hole in priority order list */
  memscpy( (void *)&tier[pos], 
           sizeof(lmtsmgr_core_prio_table_type),
           (void *)new_rec, 
           sizeof(lmtsmgr_core_prio_table_type) );

  (*num_entries)++;

}


/*=======================================================================
FUNCTION sar_core_insert_in_table
    
DESCRIPTION
  This function will insert record in table

DEPENDENCIES
  None
    
RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
static void sar_core_insert_in_table
(
  lmtsmgr_core_prio_table_type* new_rec
)
{
  switch (new_rec->pri)
  {
    case LMTSMGR_TIER_1:
      /* This should go in the high tier */
      
      // Check if it has some space and if the tech is the same 
      if ( (sar_core_tiers.num_tier1_entries < LMTSMGR_MAX_TIER_TECHS) &&
           ( (sar_core_tiers.tier1[0].tech == CXM_TECH_MAX) ||
             (sar_core_tiers.tier1[0].tech == new_rec->tech) ) )
      {
        //Insert this new record
        sar_core_insert_rec_in_tier( sar_core_tiers.tier1, 
                                &sar_core_tiers.num_tier1_entries,
                                new_rec);
      }

      break;

    case LMTSMGR_TIER_2:
      /* This should go in the high tier */
      
      // Check if it has some space and if the tech is the same 
      if ( (sar_core_tiers.num_tier2_entries < LMTSMGR_MAX_TIER_TECHS) &&
           ( (sar_core_tiers.tier2[0].tech == CXM_TECH_MAX) ||
             (sar_core_tiers.tier2[0].tech == new_rec->tech) ) )
      {
        //Insert this new record
        sar_core_insert_rec_in_tier( sar_core_tiers.tier2, 
                                &sar_core_tiers.num_tier2_entries,
                                new_rec);
      }

      break;

    case LMTSMGR_TIER_MAX:
      break;
  }
}


/*=============================================================================

  FUNCTION:  lmtsmgr_process_sar_input

=============================================================================*/
/*!
    @brief
    Processes any change in the input variables for SAR/Batt
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_process_sar_input(void)
{
  lmtsmgr_core_prio_table_type pri_state[LMTSMGR_MAX_CONCURRENT_UL_TECHS]; 
  cxm_tech_type hprio_tech = CXM_TECH_MAX, lprio_tech = CXM_TECH_MAX;
  int i, cnt = 0;
  lmtsmgr_tech_state_type *tier1_tech, *tier2_tech, *tech_st;
  coex_freqid_link_s  tulinfo;

  sar_efs_lookup_in sar_input;
  sar_efs_lookup_out sar_output;

  batt_efs_lookup_in batt_input;
  batt_efs_lookup_out batt_output;

  uint8 subs_in_call = 0;
  
  TTLMARKER(LMTSMGR_PROC_SAR_IN);
  
  memset((void*)sar_output.hplimits, DEFAULT_MAX_PLIMIT, sizeof(sar_output.hplimits));

  // Reset the priority table 
  sar_core_clear_table();
  
  // Get the techs which are in UL state
  for (i=0; i<CXM_TECH_MAX; i++)
  {
    tech_st = &lmtsmgr.tech_state[i];
    if(tech_st->currList.num_ul_entries > 0)
    {
      pri_state[cnt].tech = i;
      tulinfo = tech_st->currList.links[tech_st->currList.ul_freqList[0]].freqInfo;
      pri_state[cnt].tx_rfdev = tulinfo.link_info.rf_device;
      pri_state[cnt].band = lmtsmgr_trans_sys_bands_to_rf_bands(tulinfo.link_info.band, i);
      cnt++;
      if (cnt >= LMTSMGR_MAX_CONCURRENT_UL_TECHS)
      {
        break;
      }
    }
  }

  if (cnt >= LMTSMGR_MAX_CONCURRENT_UL_TECHS)
  {
    // Get priority of the two techs and insert in the table 
    (void)lmtsmgr_core_get_priority(&pri_state[0], &pri_state[1]);
    sar_core_insert_in_table(&pri_state[0]);
    sar_core_insert_in_table(&pri_state[1]);
  }
  else if (cnt == 1)
  {
    /* There is only 1 tech active so assume tier 1 */
    pri_state[0].pri = LMTSMGR_TIER_1;
    sar_core_insert_in_table(&pri_state[0]);
  }
  else if (cnt == 0)
  {
    TTLMARKER(LMTSMGR_PROC_SAR_ERR1);
    return ;
  }

      LMTSMGR_DBG_MSG_4(LOW, "tier1-tech: %d, pri: %d, tier2-tech: %d, pri: %d ",
                       sar_core_tiers.tier1[0].tech,
                             sar_core_tiers.tier1[0].pri,
                             sar_core_tiers.tier2[0].tech,
                             sar_core_tiers.tier2[0].pri);
      
      LMTSMGR_DBG_MSG_2(LOW, "num_tier1_entries: %d, num_tier2_entries: %d ",
                             sar_core_tiers.num_tier1_entries,
                             sar_core_tiers.num_tier2_entries);

  /* If there are entries in tier 1 but none in tier 2, provide a max backoff 
     to all tier 1 entries */
  if ( (sar_core_tiers.num_tier1_entries > 0) && 
       (sar_core_tiers.num_tier2_entries == 0) )
  {
    /* New limits for hprio tech will be MAX */
    hprio_tech = sar_core_tiers.tier1[0].tech;

    /* Check if tech_id is valid */
    if (( hprio_tech <= CXM_TECH_DFLT_INVLD ) || ( hprio_tech >= CXM_TECH_MAX ))
    {
      LMTSMGR_MSG_1(ERROR, "Invalid tech %d", hprio_tech);
	  TTLMARKER(LMTSMGR_PROC_SAR_ERR2);
      return;
    }

    /* Send notification via NPA to therm service for BCL... */
    batt_npa_send_notification(hprio_tech, CXM_TECH_MAX);

    tier1_tech = &lmtsmgr.tech_state[hprio_tech];
    sar_core_max_pwr_backoff(tier1_tech->pwr_info.new_limits.sar_plimit,
                            &tier1_tech->pwr_info.new_limits.num_valid_sar, hprio_tech);
    tier1_tech->pwr_info.new_limits.batt_plimit = DEFAULT_MAX_PLIMIT;
    lmtsmgr_notify_tech_plimit(hprio_tech);
  }
  else if ( (sar_core_tiers.num_tier1_entries == 0) && 
            (sar_core_tiers.num_tier2_entries > 0) )
  {
    /* New limits for lprio tech will be MAX */
    lprio_tech = sar_core_tiers.tier2[0].tech;

    /* Check if tech_id is valid */
    if (( lprio_tech <= CXM_TECH_DFLT_INVLD ) || ( lprio_tech >= CXM_TECH_MAX ))
    {
      LMTSMGR_MSG_1(ERROR, "Invalid tech %d", lprio_tech);
	  TTLMARKER(LMTSMGR_PROC_SAR_ERR3);
      return;
    }

    /* Send notification via NPA to therm service for BCL... */
    batt_npa_send_notification(CXM_TECH_MAX, lprio_tech);

    tier2_tech = &lmtsmgr.tech_state[lprio_tech];
    sar_core_max_pwr_backoff(tier2_tech->pwr_info.new_limits.sar_plimit,
                            &tier2_tech->pwr_info.new_limits.num_valid_sar, lprio_tech);
    tier2_tech->pwr_info.new_limits.batt_plimit = DEFAULT_MAX_PLIMIT;
    lmtsmgr_notify_tech_plimit(lprio_tech);
  }
  else if ( (sar_core_tiers.num_tier1_entries > 0) && 
            (sar_core_tiers.num_tier2_entries > 0) )
  {
    hprio_tech = sar_core_tiers.tier1[0].tech;
    lprio_tech = sar_core_tiers.tier2[0].tech;

    /* Check if tech_id is valid */
    if ((( hprio_tech <= CXM_TECH_DFLT_INVLD ) || ( hprio_tech >= CXM_TECH_MAX )) ||
         (( lprio_tech <= CXM_TECH_DFLT_INVLD ) || ( lprio_tech >= CXM_TECH_MAX )))
    {
      LMTSMGR_MSG_2(ERROR, "Invalid tech hprio: %d, lprio: %d", hprio_tech, lprio_tech);
	  TTLMARKER(LMTSMGR_PROC_SAR_ERR4);
      return;
    }

    /* Send notification via NPA to therm service for BCL only if both SUBs are in 
       CALL state ... */
    subs_in_call = 0;
    for (i = 0; i < lmtsmgr.subs_prio_list.num; i++)
    {
      if (IS_SUB_IN_CALL(lmtsmgr.subs_prio_list.order[i].state))
      {
        subs_in_call++;
      }
    }

    if ((lmtsmgr.subs_prio_list.num != 2)|| (subs_in_call == 2))
    {
      batt_npa_send_notification(hprio_tech, lprio_tech);
    }

    tier1_tech = &lmtsmgr.tech_state[hprio_tech];
    tier2_tech = &lmtsmgr.tech_state[lprio_tech];
    
    /* Do we have Tx power from hprio tech? */
    if (tier1_tech->pwr_info.tx_pwr_valid)
    {
      sar_input.hptech = hprio_tech;
      sar_input.hpband = sar_core_tiers.tier1[0].band;
      sar_input.lptech = lprio_tech;
      sar_input.lpband = sar_core_tiers.tier2[0].band;
      sar_input.tx_power = tier1_tech->pwr_info.tx_power;
      sar_input.dsi = sar_get_current_dsi();
      if (TRUE == sar_efs_lookup(&sar_input,&sar_output))
      {
        if(sar_output.num_hplimits > 0)
        {
          /* SAR limit for high priority tech */
          memscpy( (void *)&tier1_tech->pwr_info.new_limits.sar_plimit, 
                   sizeof(tier1_tech->pwr_info.new_limits.sar_plimit),
                   (void *)&sar_output.hplimits,
                   sizeof(sar_output.hplimits) );

          tier1_tech->pwr_info.new_limits.num_valid_sar = sar_output.num_hplimits;
        }
        else
        {
          /* No limit defined, set it to MAX */
          sar_core_max_pwr_backoff(tier1_tech->pwr_info.new_limits.sar_plimit,
                                  &tier1_tech->pwr_info.new_limits.num_valid_sar, hprio_tech);
        }

        if(sar_output.lplimit_valid)
        {
          /* SAR limit for low priority tech */

          if (sar_output.num_lplimits == (SAR_GSM_MAX_LIMIT_TYPE))
          {
            for (i = 0; i < (SAR_GSM_MAX_LIMIT_TYPE); i++)
            {
                  tier2_tech->pwr_info.new_limits.sar_plimit[i] = sar_output.lplimit[i];
            }
                  tier2_tech->pwr_info.new_limits.num_valid_sar = SAR_GSM_MAX_LIMIT_TYPE;
          }
              else
              {
                tier2_tech->pwr_info.new_limits.sar_plimit[0] = sar_output.lplimit[0];
                  tier2_tech->pwr_info.new_limits.num_valid_sar = 1;
              }
        }
        else
        {
          /* No limit defined, set it to MAX */
          sar_core_max_pwr_backoff(tier2_tech->pwr_info.new_limits.sar_plimit,
                                  &tier2_tech->pwr_info.new_limits.num_valid_sar, lprio_tech);
        }
      }
      else
      {
        /* No SAR limits defined for this band combination, set limit to MAX */
        sar_core_max_pwr_backoff(tier1_tech->pwr_info.new_limits.sar_plimit,
                                &tier1_tech->pwr_info.new_limits.num_valid_sar, hprio_tech);
        sar_core_max_pwr_backoff(tier2_tech->pwr_info.new_limits.sar_plimit,
                                &tier2_tech->pwr_info.new_limits.num_valid_sar, lprio_tech);
      }

      /* Check if there is a backoff to hprio tech due for limiting battery 
         current */
      batt_input.hptech = hprio_tech;
      batt_input.hpband = sar_core_tiers.tier1[0].band;
      batt_input.lptech = lprio_tech;
      batt_input.lpband = sar_core_tiers.tier2[0].band;
      if (TRUE == batt_efs_lookup(&batt_input,&batt_output))
      {
        if(batt_output.hplimit_valid)
        {
          /* Batt limit for high priority tech */
          tier1_tech->pwr_info.new_limits.batt_plimit = batt_output.hplimit;
        }
        else
        {
          /* No limit defined, set it to MAX */
          tier1_tech->pwr_info.new_limits.batt_plimit = DEFAULT_MAX_PLIMIT;
        }
      }
      
      /* Notify techs of the new pwr limits */
      lmtsmgr_notify_tech_plimit(lprio_tech);
      lmtsmgr_notify_tech_plimit(hprio_tech);
    }
    else
    {
      LMTSMGR_MSG_1(MED, "Have not recieved Txpower from tech %d yet...", hprio_tech);
    }
  }
  else /* Error case */
  {
    LMTSMGR_MSG_0(LOW, "No entries in either tiers");
  }  
  
  TTLMARKER(LMTSMGR_PROC_SAR_OUT);
}


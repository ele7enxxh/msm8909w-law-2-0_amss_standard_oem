
/*!
  @file
  wwcoex_conflict_table.c

  @brief
  This file implements the conflict table management functions of LIMTSMGR COEX module.

  @ingroup per_implementation
*/

/*=============================================================================

  Copyright (c) 2013-2015 Qualcomm Technologies Incorporated.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/wwan_coex/src/wwcoex_conflict_table.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/02/15   sg      EFS based spur simulation support
10/30/14   jm      Interface change to support 4-bit action
10/27/14   jm      KW Fixes
08/22/14   sg      Spur mitigation support
08/18/14   sg      Slot level timeline simulator support
07/17/14   sg      efs based coex simulation support
07/03/14   jm      Support for desense calculation in DR-DSDS
01/14/14   jm      Ensure freqID is 16-bit value
11/23/13   rj      Handle Tech Rx Power Ind after Rx OFF indication
11/22/13   jm      Resolution of LLVM Warnings
07/29/13   ag      Fixed query highest action returning UNKNOWN for hybrid mode
03/23/13   ag      Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "comdef.h"
#include <stringl/stringl.h>
#include "wwcoex_action_iface.h"
#include "wwcoex_conflict_table.h"

/*=============================================================================

                            TYPEDEFS

=============================================================================*/


/*=============================================================================

                         INTERNAL VARIABLES

=============================================================================*/

#ifndef MCS_LIMITS_LOW_MEMORY

/*! conflict tables */
wwcoex_conflict_tbls_type  wwcoex_conflict_tbls[COEX_MAX_CONCURRENT_SCENARIOS];

/*! Spur related globals */
cxm_spur_info_s            wwcoex_spur_info[WWCOEX_MAX_SPUR_TABLE_SIZE];
uint16                     wwcoex_spur_info_idx;

/*! hard coded spurs for testing */
cxm_spur_info_s   wwcoex_sim_spur_info[WWCOEX_MAX_CONCURRENT_SPURS];
uint32            wwcoex_sim_spur_count;

#endif /* MCS_LIMITS_LOW_MEMORY */

/*! number of active/standby stacks */
uint32   wwcoex_num_standby_stacks    = 0;
uint32   wwcoex_num_active_stacks     = 0;
boolean  wwcoex_is_drdsds_mode        = FALSE;
boolean  wwcoex_is_lmts_task_running  = FALSE;
volatile boolean  wwcoex_is_spur_mitigation_en = FALSE;

/*! simulation input */
volatile uint32   wwcoex_simulation_input      = 0;


/*=============================================================================

                                FUNCTIONS

=============================================================================*/
/*===========================================================================
FUNCTION wwcoex_set_sub_state

DESCRIPTION
  This API will update the subscription state

DEPENDENCIES 
  None

RETURN VALUE  
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void wwcoex_set_sub_state
(
  uint32 num_standby_stacks,
  uint32 num_active_stacks
)
{

  wwcoex_num_standby_stacks = num_standby_stacks;
  wwcoex_num_active_stacks = num_active_stacks; 
}

/*===========================================================================
FUNCTION wwcoex_set_rf_mode

DESCRIPTION
  This API will update the RF mode

DEPENDENCIES 
  None

RETURN VALUE  
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void wwcoex_set_rf_mode
(
  boolean is_drdsds_enabled
)
{
  wwcoex_is_drdsds_mode = is_drdsds_enabled; 
}

/*===========================================================================
FUNCTION wwcoex_set_spur_mitigation_mask

DESCRIPTION
  This API will update the RF mode

DEPENDENCIES 
  None

RETURN VALUE  
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void wwcoex_set_spur_mitigation_mask
(
  boolean is_spur_mitigation_enabled
)
{
  wwcoex_is_spur_mitigation_en = is_spur_mitigation_enabled; 
}

/*===========================================================================
FUNCTION wwcoex_init_spur_sim

DESCRIPTION
  This API will init the spur simulator with preloaded values

DEPENDENCIES 
  None

RETURN VALUE  
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void wwcoex_init_spur_sim
(
  uint32 sim_input
)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  uint32 i;

   /* If the spur EFS is set then use that to override the spur info table */
  if(wwcoex_sim_spur_count > 0)
  {
    for(i=0;i<wwcoex_sim_spur_count;i++)
    {
      wwcoex_spur_info[i].spur_freq_offset      = wwcoex_sim_spur_info[i].spur_freq_offset;
      wwcoex_spur_info[i].spur_freq_uncertainty = 500;
      wwcoex_spur_info[i].victim_freq_id        = 1;
      wwcoex_spur_info[i].spur_level            = wwcoex_sim_spur_info[i].spur_level;
      wwcoex_spur_info[i].notch_depth           = wwcoex_sim_spur_info[i].notch_depth;
      wwcoex_spur_info[i].spur_type             = 2;
      
      wwcoex_spur_info_idx += 1;
    }
  }
  /* This condition is used by FW for VP test.
     FW-CXM overrides MCS behavior using this API for testing spur */
  else if(sim_input > 0)
  {
    /* If SIM is needed populate a dummy entry in the array */
    wwcoex_spur_info[0].spur_freq_offset = -9000;
    wwcoex_spur_info[0].spur_freq_uncertainty = 500;
    wwcoex_spur_info[0].victim_freq_id = 49136;
    wwcoex_spur_info[0].spur_level = -630;
    wwcoex_spur_info[0].notch_depth = 0;
    wwcoex_spur_info[0].spur_type = 2;

    wwcoex_spur_info_idx += 1;

    /* Enable spur mitigation */
    wwcoex_set_spur_mitigation_mask(TRUE);

    /* Override the SIM handle */
    wwcoex_sim_spur_count = 1;
  }

#else /* MCS_LIMITS_LOW_MEMORY */

  WWCOEX_DUMMY_CHECK(sim_input);

#endif /* MCS_LIMITS_LOW_MEMORY */

}


/*===========================================================================
FUNCTION wwcoex_init_tables

DESCRIPTION
  This API will init the wwcoex conflict tables

DEPENDENCIES 
  None

RETURN VALUE  
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void wwcoex_init_tables
(
  uint32 num_standby_stacks,
  uint32 num_active_stacks
)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  uint32 i;

  /* Allocate memory for conflict tables */
  for (i=0; i <COEX_MAX_CONCURRENT_SCENARIOS; i++)
  {
    memset(&wwcoex_conflict_tbls[i], 0, sizeof(wwcoex_conflict_tbls_type));

    wwcoex_conflict_tbls[i].current_tbl = WWCOEX_TBL_INVALID;
  }
  
  wwcoex_num_active_stacks = num_active_stacks;
  wwcoex_num_standby_stacks = num_standby_stacks;

#else /* MCS_LIMITS_LOW_MEMORY */

  WWCOEX_DUMMY_CHECK(num_standby_stacks);
  WWCOEX_DUMMY_CHECK(num_standby_stacks);

#endif /* MCS_LIMITS_LOW_MEMORY */

}

/*===========================================================================
FUNCTION wwcoex_check_task_state

DESCRIPTION
  This API will inform the caller whether lmtsmgr task is up

DEPENDENCIES 
  None

RETURN VALUE  
  TRUE  - if the lmtsmgr task is up
  FALSE - if the lmtsmgr task is not running

SIDE EFFECTS
  None
  
===========================================================================*/
boolean wwcoex_check_task_state()
{
  return wwcoex_is_lmts_task_running;
}

/*===========================================================================
FUNCTION wwcoex_set_task_state

DESCRIPTION
  This API will set lmtsmgr task up flag

DEPENDENCIES 
  None

RETURN VALUE  
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void wwcoex_set_task_state(boolean task_state)
{
  wwcoex_is_lmts_task_running = task_state;
}

/*=============================================================================

  FUNCTION:  wwcoex_find_match_node

=============================================================================*/
/*!
    @brief
    Find a node with matching techs
 
    @return
    index to the matching node node, MAX otherwise

*/
/*===========================================================================*/
uint32 wwcoex_find_match_node
(
  uint8 tech1,
  uint8 tech2
)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  uint32 index;

  /* Check if this tech combination already has a node */
  for (index=0; index<COEX_MAX_CONCURRENT_SCENARIOS; index++)
  {
    if ( wwcoex_conflict_tbls[index].current_tbl == WWCOEX_TBL_INVALID )
    {
      continue;
    }

    if ( ( (wwcoex_conflict_tbls[index].tech1 == tech1) && 
           (wwcoex_conflict_tbls[index].tech2 == tech2) ) ||
         ( (wwcoex_conflict_tbls[index].tech1 == tech2) &&
           (wwcoex_conflict_tbls[index].tech2 == tech1) ) )
    {
      /* Found a match */
      break;
    }
  }

  return index;

#else /* MCS_LIMITS_LOW_MEMORY */

  WWCOEX_DUMMY_CHECK(tech1);
  WWCOEX_DUMMY_CHECK(tech2);

  return COEX_MAX_CONCURRENT_SCENARIOS;

#endif /* MCS_LIMITS_LOW_MEMORY */

}

/*=============================================================================

  FUNCTION:  wwcoex_get_table_node

=============================================================================*/
/*!
    @brief
    Get the conflict table node corresponding to the two tech pair. If there
    is no table for the pair, then return an unused node.
 
    @return
    conflict table node

*/
/*===========================================================================*/
wwcoex_conflict_tbls_type* wwcoex_get_table_node
(
  uint8   tech1,
  uint8   tech2
)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  uint32 index;
  wwcoex_conflict_tbls_type* tbl_node=NULL;

  /* Check if there already exists a node for this tech combination */
  index = wwcoex_find_match_node(tech1, tech2);

  if (index >= COEX_MAX_CONCURRENT_SCENARIOS)
  {
    /* Could not find a match */

    /* Check if there is anything unused */
    for (index = 0; index < COEX_MAX_CONCURRENT_SCENARIOS; index++ )
    {
      if ( wwcoex_conflict_tbls[index].current_tbl == WWCOEX_TBL_INVALID )
      {
        tbl_node = &wwcoex_conflict_tbls[index];
        tbl_node->tech1 = tech1;
        tbl_node->tech2 = tech2;
        break; 
      }
    }
  }
  else
  {
    tbl_node = &wwcoex_conflict_tbls[index];
  }

  return tbl_node;

#else /* MCS_LIMITS_LOW_MEMORY */

  WWCOEX_DUMMY_CHECK(tech1);
  WWCOEX_DUMMY_CHECK(tech2);
  return NULL;

#endif /* MCS_LIMITS_LOW_MEMORY */

}

/*=============================================================================

  FUNCTION:  wwcoex_invalidate_tables

=============================================================================*/
/*!
    @brief
    Invalidates the conflict tables containing the specified tech
 
    @return
    NONE

*/
/*===========================================================================*/
void wwcoex_invalidate_tables
(
  uint8  tech
)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  uint8 i,j;

  /* Find nodes that have this tech and invalidate them */
  for (i=0; i<COEX_MAX_CONCURRENT_SCENARIOS; i++)
  {
    if (wwcoex_conflict_tbls[i].tech1 == tech || 
        wwcoex_conflict_tbls[i].tech2 == tech)
    {
      wwcoex_conflict_tbls[i].current_tbl = WWCOEX_TBL_INVALID;

      for (j=0; j<WWCOEX_MAX_TABLES_PER_SCENARIO; j++)
      {
        wwcoex_conflict_tbls[i].tables[j].tech1_num_entries = 0;
        wwcoex_conflict_tbls[i].tables[j].tech2_num_entries = 0;
      }
    }
  }

#else /* MCS_LIMITS_LOW_MEMORY */

  WWCOEX_DUMMY_CHECK(tech);

#endif /* MCS_LIMITS_LOW_MEMORY */

}

/*=============================================================================

  FUNCTION:  wwcoex_get_oldest_tbl_index

=============================================================================*/
/*!
    @brief
    Returns index to the oldest table
 
    @return
    NONE

*/
/*===========================================================================*/
int8 wwcoex_get_oldest_tbl_index
(
  wwcoex_conflict_tbls_type*  conflict_tbl
)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  int8 index;

  if (conflict_tbl->current_tbl < 0)
  {
    /* Table node is not valid... so use the first one */
    index = 0;
  }
  else
  {
    index = (conflict_tbl->current_tbl + 1) % WWCOEX_MAX_TABLES_PER_SCENARIO ;
  }

  return index;

#else /* MCS_LIMITS_LOW_MEMORY */

  WWCOEX_DUMMY_CHECK(conflict_tbl);

  return 0;

#endif /* MCS_LIMITS_LOW_MEMORY */

}

/*=============================================================================

  FUNCTION:  wwcoex_tbl_freqid_pair_lookup

=============================================================================*/
/*!
    @brief
    Search for a freq id pair in the conflict tables and return the action
 
    @return
    NONE

*/
/*===========================================================================*/
cxm_action_type wwcoex_tbl_freqid_pair_lookup
(
  uint32 freqid1,
  uint32 freqid2
)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  cxm_action_type action = ACTION_UNKNOWN;
  int index1, index2;
  uint32 i,j;
  wwcoex_tbl_type* tbl_ptr;
  boolean match_found = FALSE;

  for (i=0; i<COEX_MAX_CONCURRENT_SCENARIOS; i++)
  {
    /* Check if node is valid */
    if (wwcoex_conflict_tbls[i].current_tbl == WWCOEX_TBL_INVALID)
    {
      continue;
    }

    /* Node is valid so scan both tables. Start for current table first since
       that has more chances of finding a match */
    j = wwcoex_conflict_tbls[i].current_tbl;
    do
    {
      tbl_ptr =  &wwcoex_conflict_tbls[i].tables[j];

      /* Is the table valid ? */
      if ( (tbl_ptr->tech1_num_entries > 0) &&
           (tbl_ptr->tech2_num_entries > 0) )
      {
        index1 = freqid1 - tbl_ptr->tech1_fid_offset;
        index2 = freqid2 - tbl_ptr->tech2_fid_offset;

        if ( (index1 < 0) || (index1 >= tbl_ptr->tech1_num_entries) || 
             (index2 < 0) || (index2 >= tbl_ptr->tech2_num_entries)  )
        {
          /* Try the other way around */
          index1 = freqid2 - tbl_ptr->tech1_fid_offset;
          index2 = freqid1 - tbl_ptr->tech2_fid_offset;

          if ( (index1 >= 0) && (index1 < tbl_ptr->tech1_num_entries) &&
               (index2 >= 0) && (index2 < tbl_ptr->tech2_num_entries)  )
          {
            /* Found a match... */
            action = tbl_ptr->arr[index1][index2].action;
            match_found = TRUE;
            break;
          }
        }
        else
        {
          /* Found a match... */
          action = tbl_ptr->arr[index1][index2].action;
          match_found = TRUE;
          break;
        }
      }
      j = (j + 1) % WWCOEX_MAX_TABLES_PER_SCENARIO;
    }while (j != wwcoex_conflict_tbls[i].current_tbl);

    if (match_found == TRUE)
    {
      break;
    }
  }

  return action;

#else /* MCS_LIMITS_LOW_MEMORY */

  WWCOEX_DUMMY_CHECK(freqid1);
  WWCOEX_DUMMY_CHECK(freqid2);

  return ACTION_NONE;

#endif /* MCS_LIMITS_LOW_MEMORY */

}


/*=============================================================================

  FUNCTION:  wwcoex_tbl_freqid_pair_spur_lookup

=============================================================================*/
/*!
    @brief
    Search for a freq id pair in the conflict tables and return the spur 
    If the spur exists then the spur_info will be populated with the 
    right values
 
    @return
    NONE

*/
/*===========================================================================*/
cxm_action_type wwcoex_tbl_freqid_pair_spur_lookup
(
  uint32            freqid1,
  uint32            freqid2,
  uint16           *handle
)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  cxm_action_type spur_action = ACTION_SPUR_UNKNOWN;
  int index1, index2;
  uint32 i,j;
  wwcoex_tbl_type* tbl_ptr;
  boolean match_found = FALSE;

  /* initialize handle to unknown */
  *handle = WWCOEX_SPUR_HANDLE_UNKNOWN;

  for (i=0; i<COEX_MAX_CONCURRENT_SCENARIOS; i++)
  {
    /* Check if node is valid */
    if (wwcoex_conflict_tbls[i].current_tbl == WWCOEX_TBL_INVALID)
    {
      continue;
    }

    /* Node is valid so scan both tables. Start for current table first since
       that has more chances of finding a match */
    j = wwcoex_conflict_tbls[i].current_tbl;
    do
    {
      tbl_ptr =  &wwcoex_conflict_tbls[i].tables[j];

      /* Is the table valid ? */
      if ( (tbl_ptr->tech1_num_entries > 0) &&
           (tbl_ptr->tech2_num_entries > 0) )
      {
        index1 = freqid1 - tbl_ptr->tech1_fid_offset;
        index2 = freqid2 - tbl_ptr->tech2_fid_offset;

        if ( (index1 < 0) || (index1 >= tbl_ptr->tech1_num_entries) || 
             (index2 < 0) || (index2 >= tbl_ptr->tech2_num_entries)  )
        {
          /* Try the other way around */
          index1 = freqid2 - tbl_ptr->tech1_fid_offset;
          index2 = freqid1 - tbl_ptr->tech2_fid_offset;

          if ( (index1 >= 0) && (index1 < tbl_ptr->tech1_num_entries) &&
               (index2 >= 0) && (index2 < tbl_ptr->tech2_num_entries)  )
          {
            /* Found a match... */
            *handle = tbl_ptr->arr[index1][index2].spur_handle_2;
            if(*handle == WWCOEX_SPUR_HANDLE_NONE)
            {
              spur_action = ACTION_NONE;
            }
            else if(*handle == WWCOEX_SPUR_HANDLE_UNKNOWN)
            {
              spur_action = ACTION_SPUR_UNKNOWN;
            }
            else
            {
              spur_action = ACTION_SPUR;
            }
            match_found = TRUE;
            break;
          }
        }
        else
        {
          /* Found a match... */
          *handle = tbl_ptr->arr[index1][index2].spur_handle_1;
          if(*handle == WWCOEX_SPUR_HANDLE_NONE)
          {
            spur_action = ACTION_NONE;
          }
          else if(*handle == WWCOEX_SPUR_HANDLE_UNKNOWN)
          {
            spur_action = ACTION_SPUR_UNKNOWN;
          }
          else
          {
            spur_action = ACTION_SPUR;
          }
          match_found = TRUE;
          break;
        }
      }
      j = (j + 1) % WWCOEX_MAX_TABLES_PER_SCENARIO;
    }while (j != wwcoex_conflict_tbls[i].current_tbl);

    if (match_found == TRUE)
    {
      break;
    }
  }

  return spur_action;

#else /* MCS_LIMITS_LOW_MEMORY */

  WWCOEX_DUMMY_CHECK(freqid1);
  WWCOEX_DUMMY_CHECK(freqid2);
  WWCOEX_DUMMY_CHECK(handle);

  return ACTION_NONE;

#endif /* MCS_LIMITS_LOW_MEMORY */

}


/*=============================================================================

  FUNCTION:  wwcoex_tbl_freqid_lookup

=============================================================================*/
/*!
    @brief
    Search for a freq id in the conflict tables and return the MAX action
 
    @return
    NONE

*/
/*===========================================================================*/
cxm_action_type wwcoex_tbl_freqid_lookup
(
  uint32 freqid1
)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  cxm_action_type action = ACTION_NONE;
  int index1, index2;
  uint32 i,j,num_scenarios=COEX_MAX_CONCURRENT_SCENARIOS;
  wwcoex_tbl_type* tbl_ptr;
  boolean found = FALSE;

  for (i=0; i<COEX_MAX_CONCURRENT_SCENARIOS; i++)
  {
    /* Check if node is valid */
    if (wwcoex_conflict_tbls[i].current_tbl == WWCOEX_TBL_INVALID)
    {
      num_scenarios--;
      continue;
    }

    /* Node is valid so scan both tables. Start for current table first since
       that has more chances of finding a match */
    j = wwcoex_conflict_tbls[i].current_tbl;
    do
    {
      tbl_ptr =  &wwcoex_conflict_tbls[i].tables[j];

      /* Is the table valid ? */
      if ( (tbl_ptr->tech1_num_entries > 0) &&
           (tbl_ptr->tech2_num_entries > 0) )
      {
        /* Check first which columns does the index map to */
        index1 = freqid1 - tbl_ptr->tech1_fid_offset;
        if ( (index1 < 0) || (index1 >= tbl_ptr->tech1_num_entries) )
        {
          /* Try the other way around */
          index2 = freqid1 - tbl_ptr->tech2_fid_offset;

          if ( (index2 >= 0) && (index2 < tbl_ptr->tech2_num_entries) )
          {
            /* Its the secondary index.. So go through all the entries in first column. */
            for (index1 = 0; index1 < tbl_ptr->tech1_num_entries; index1++)
            {
              action = MAX((uint8)action, (uint8)(tbl_ptr->arr[index1][index2].action));
            }

            /* found id in this table so no need to check other table */
            found = TRUE;
            break;
          }
          //else cannot find the freqId in this table.. do nothing.
        }
        else
        {
          /* Its the primary index.. So go through all the entries in second column. */
          for (index2 = 0; index2 < tbl_ptr->tech2_num_entries; index2++)
          {
            action = MAX((uint8)action, (uint8)(tbl_ptr->arr[index1][index2].action));
          }

          /* found id in this table so no need to check other table */
          found = TRUE;
          break;
        }
      }
      j = (j + 1) % WWCOEX_MAX_TABLES_PER_SCENARIO;
    }while (j != wwcoex_conflict_tbls[i].current_tbl);
  }

  /* if not found in any tables and there is table for atleast one scenario, then return UNKNOWN... */
  if ((found==FALSE) && (num_scenarios > 0))
  {
    action = ACTION_UNKNOWN;
  }

  return action;

#else /* MCS_LIMITS_LOW_MEMORY */

  WWCOEX_DUMMY_CHECK(freqid1);

  return ACTION_NONE;

#endif

}

/*============================================================================

FUNCTION CXM_QUERY_HIGHEST_ACTION

DESCRIPTION
  Query the highest mitigation action for one freqId
  
DEPENDENCIES
  None

RETURN VALUE
  FALSE for incorrect arguments like NULL pointer, 0 entries in the list, 
  incorrect freqId
  TRUE otherwise

SIDE EFFECTS
  None

============================================================================*/
boolean cxm_query_highest_action
(
  cxm_highest_action_query_s* query_st
)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  int i;

  /* Check input... */
  if ( query_st == NULL || query_st->actions == NULL ||
       query_st->num_requesting_ids == 0 || query_st->requesting_ids == NULL || 
       query_st->num_requesting_ids > WWCOEX_MAX_REQUESTING_IDS )
  {
    return FALSE;
  }

  /* SW Coex mitigation is disabled */
  if (wwcoex_num_standby_stacks == 0)
  {
    memset((void *)(query_st->actions), 0, 
           query_st->num_requesting_ids * sizeof(cxm_action_type));
    return TRUE;
  }

  /* Scan through each entry in the table */
  for (i=0; i < query_st->num_requesting_ids; i++)
  {
    /* If any of the requesting Ids are unknown then return action as unknown */
    if ( (query_st->requesting_ids[i]) == WWCOEX_UNKNOWN_FREQID )
    {
      query_st->actions[i] = ACTION_UNKNOWN;
    }
    else
    {
       /* Lookup conflict table for freqids*/
       query_st->actions[i] = wwcoex_tbl_freqid_lookup(query_st->requesting_ids[i]);
    }
  }

  return TRUE;

#else /* MCS_LIMITS_LOW_MEMORY */

  WWCOEX_DUMMY_CHECK(query_st);

  return FALSE;

#endif
}

#ifndef MCS_LIMITS_LOW_MEMORY
/*============================================================================

FUNCTION CXM_PROCESS_FREQ_ID_FOR_SIMULATOR

DESCRIPTION
  Provides the selective blanking decision based on the freq ID of requesting
  and conflicting tech.
  
DEPENDENCIES
  None

RETURN VALUE
  cxm_action_type

SIDE EFFECTS
  None

============================================================================*/
static cxm_action_type cxm_process_freq_id_for_simulator
(
  uint32 req_id,
  uint32 conflict_id,
  cxm_action_query_s* query_st
)
{

  wwan_coex_activity_type  x_activity = ACTIVITY_INVALID;
  wwan_coex_activity_type  g_activity = ACTIVITY_INVALID;
  cxm_action_type action = ACTION_NONE;

  /* Since GSM is simulated, there is no query from GSM
     So the requesting tech is always 'X' and conflicting tech is GSM 
  */
  x_activity = query_st->requesting_activity;
  g_activity = query_st->conflicting_activity;

  switch(wwcoex_simulation_input)
  {
    case 1: /*X Rx - G Tx */
      if((x_activity == ACTIVITY_RX) &&
         (g_activity == ACTIVITY_TX))
      {
        action = ACTION_BLANK;
      }
      break;
    
    case 2: /*X Tx - G Rx */
      if((x_activity == ACTIVITY_TX) &&
         (g_activity == ACTIVITY_RX))
      {
        action = ACTION_BLANK;
      }
      break;

    case 3: /*X Tx - G Tx */
      if((x_activity == ACTIVITY_TX) &&
         (g_activity == ACTIVITY_TX))
      {
        action = ACTION_BLANK;
      }
      break;
      
    case 4:
      if(((x_activity == ACTIVITY_RX) &&
          (g_activity == ACTIVITY_TX)) ||
         ((x_activity == ACTIVITY_TX) &&
          (g_activity == ACTIVITY_TX)))
      {
        action = ACTION_BLANK;
      }
      break;
        
    default:
      action = ACTION_UNKNOWN;
  }

  return action;

}

#endif /* MCS_LIMITS_LOW_MEMORY */


#ifndef MCS_LIMITS_LOW_MEMORY
/*============================================================================

FUNCTION CXM_QUERY_ACTION_FOR_SIMULATOR

DESCRIPTION
  Query the mitigation action corresponding to the two techs freq combination.
  The result provided by this API is based on simulation EFS input instead
  of the conflict table for the 2 techs.
  
DEPENDENCIES
  None

RETURN VALUE
  FALSE for incorrect arguments like NULL pointer, 0 entries in the list, 
  incorrect freqId
  TRUE otherwise

SIDE EFFECTS
  None

============================================================================*/
static boolean cxm_query_action_for_simulator
(
  cxm_action_query_s* query_st
)
{

  int i,j;
  boolean blanket_decision = FALSE;
  cxm_action_type action = ACTION_NONE;
  cxm_action_type action_func = ACTION_NONE;

  /*-------------------------------------
    Tech1   Tech2   Result  EFS entry
    -------------------------------------
    X RX    G TX    Blank   0X01
    X TX    G RX    Blank   0X02
    X TX    G TX    Blank   0X03
    
    X TX    X TX    Blank   0X10
    X TX    X RX    Blank   0X11
    X TX    X X     Blank   0X12

    X X     X X     Blank   0X100
  -------------------------------------*/ 

  /* Invalid input to simulator */
  if((wwcoex_simulation_input == 0) ||
     (wwcoex_simulation_input > 0x100))
  {
    action = ACTION_UNKNOWN;
    blanket_decision = TRUE;
  }

  /* Always blank inout to simulator */
  else if (wwcoex_simulation_input == 0x100)
  {
    action = ACTION_BLANK;
    blanket_decision = TRUE;
  }

  /* Selective blanking but not tech based */
  else if (wwcoex_simulation_input >= 0x10)
  {
    blanket_decision = TRUE;
    
    switch (wwcoex_simulation_input)
    {
      case 0x10: /* TX - TX blanking */
        if((query_st->requesting_activity == ACTIVITY_TX) &&
           (query_st->conflicting_activity == ACTIVITY_TX))
        {
          action = ACTION_BLANK;
        }
        break;

      case 0x11: /* TX - RX blanking */
        if(((query_st->requesting_activity == ACTIVITY_TX) &&
            (query_st->conflicting_activity == ACTIVITY_RX)) ||
           ((query_st->conflicting_activity == ACTIVITY_TX) &&
            (query_st->requesting_activity == ACTIVITY_RX)))
        {
          action = ACTION_BLANK;
        }
        break;

      case 0x12: /* TX - XX blanking */
        if((query_st->requesting_activity == ACTIVITY_TX) ||
           (query_st->conflicting_activity == ACTIVITY_TX) )
        {
          action = ACTION_BLANK;
        }
        break;

      default:
        action = ACTION_UNKNOWN;
        break;
    }
  }

  /* Selective blanking based on GSM state */
  else
  {
    for (i=0; i < query_st->num_requesting_ids; i++)
    {
      for (j=0; j < query_st->num_conflicting_ids; j++)
      {
        action_func = cxm_process_freq_id_for_simulator
                      (query_st->requesting_ids[i],
                       query_st->conflicting_ids[j],
                       query_st);
        
        action = MAX(action, action_func);
      }
      /* Store the action into the mask */
      query_st->actions |= (action << (WWCOEX_ACTION_BIT_SIZE*i));
    } 
  }

  /* If the decision is not frequency ID specific then 
     apply the blanket decision to all */
  if(blanket_decision)
  {
    for (i=0; i < query_st->num_requesting_ids; i++)
    {
      query_st->actions |= (action << (WWCOEX_ACTION_BIT_SIZE*i));
    }
  }

  return TRUE;

}

#endif /* MCS_LIMITS_LOW_MEMORY */

/*============================================================================

FUNCTION CXM_QUERY_ACTION

DESCRIPTION
  Query the mitigation action corresponding to the two techs freq combination.
  
DEPENDENCIES
  None

RETURN VALUE
  FALSE for incorrect arguments like NULL pointer, 0 entries in the list, 
  incorrect freqId
  TRUE otherwise

SIDE EFFECTS
  None

============================================================================*/
boolean cxm_query_action
(
  cxm_action_query_s* query_st
)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  int i,j;
  cxm_action_type action = ACTION_NONE;
  cxm_action_type action_func = ACTION_NONE;

  /* Check input... */
  if ( query_st == NULL ||
       query_st->num_conflicting_ids == 0 || query_st->conflicting_ids == NULL ||
       query_st->num_requesting_ids == 0 || query_st->requesting_ids == NULL || 
       query_st->num_requesting_ids > WWCOEX_MAX_REQUESTING_IDS )
  {
    return FALSE;
  }
  
  

  /* If SW mitigation is disabled then num standby tasks will be 0; 
     Also if its DSDA, then Rx-Rx conflict should have action as NONE */
  if ( (wwcoex_num_standby_stacks == 0) ||
       ( ((wwcoex_is_drdsds_mode == TRUE)||
         (wwcoex_num_standby_stacks == wwcoex_num_active_stacks)) &&
         (query_st->requesting_activity == ACTIVITY_RX) && 
         (query_st->conflicting_activity == ACTIVITY_RX) ) ) 
  {
    return TRUE;
  }

  if(wwcoex_simulation_input != 0)  
  {
    return cxm_query_action_for_simulator(query_st);
  }

  /* Scan through each entry in the table */
  for (i=0; i < query_st->num_requesting_ids; i++)
  {
    if (query_st->requesting_ids[i] != WWCOEX_UNKNOWN_FREQID)
    {
      /* Start with no action */
      action = ACTION_NONE;
      
      for (j=0; j < query_st->num_conflicting_ids; j++)
      {
        /* If any of the Ids are unknown then return action as unknown */
        if ( query_st->conflicting_ids[j] != WWCOEX_UNKNOWN_FREQID )
        {
          /* Lookup conflict table for freqids*/
          action_func =  wwcoex_tbl_freqid_pair_lookup( 
                                            query_st->requesting_ids[i],
                                            query_st->conflicting_ids[j]);

          action = MAX(action, action_func);
        }
        else
        {
          action = MAX(action, ACTION_UNKNOWN);
        }
      }
    }
    else
    {
      /* Use unknown... */
      action = ACTION_UNKNOWN;	
    }

    /* Store the action into the mask */
    query_st->actions |= (action << (WWCOEX_ACTION_BIT_SIZE*i));
  }

  return TRUE;

#else /* MCS_LIMITS_LOW_MEMORY */

  /*Reset the mask */
  query_st->actions = 0;
  return TRUE;
  
#endif /* MCS_LIMITS_LOW_MEMORY */

  
  
}

/*============================================================================

FUNCTION CXM_QUERY_SPUR_ACTION

DESCRIPTION
  Query the mitigation action corresponding to the two techs freq combination.
  
DEPENDENCIES
  None

RETURN VALUE
  FALSE for incorrect arguments like NULL pointer, 0 entries in the list, 
  incorrect freqId
  TRUE otherwise

SIDE EFFECTS
  None

============================================================================*/
boolean cxm_query_spur_action_for_simulator
(
  cxm_spur_query_s* query_st
)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  uint32 i,j,shift;
  uint32 num_request_ids, num_conflict_ids;
  cxm_action_type action = ACTION_NONE;
  cxm_action_type action_func = ACTION_NONE;
  uint32 req_car_per_ch =0, curr_req_channel = 0, curr_req_carrier = 0;
  uint32 conf_car_per_ch =0, curr_conf_channel = 0;
  uint16 handle = 0;


  /* Number of requesting IDs and conflicting IDs are a function of 
     number of requesting channel and carriers per channel */
  req_car_per_ch = query_st->num_carr_per_requesting_ch;
  conf_car_per_ch = query_st->num_carr_per_conflicting_ch;
  
  num_request_ids  = query_st->num_requesting_ch*req_car_per_ch;
  num_conflict_ids = query_st->num_conflicting_ch*conf_car_per_ch;

  /* Scan through each entry in the table to get the handle */
  for (i=0; i < num_request_ids; i++)
  {
    curr_req_channel = i/req_car_per_ch;
    
    if (query_st->requesting_ids[i] != WWCOEX_UNKNOWN_FREQID)
    {
      /* Start with no action */
      action = ACTION_NONE;
      
      for (j=0; j < num_conflict_ids; j++)
      {
        curr_conf_channel = j/conf_car_per_ch;
        
        /* If any of the Ids are unknown then return action as unknown */
        if ( query_st->conflicting_ids[j] != WWCOEX_UNKNOWN_FREQID )
        {
          /* Do a look up only if channel ID does not match */
          if(query_st->requesting_ch_id[curr_req_channel] != 
             query_st->conflicting_ch_id[curr_conf_channel])
          {
            action_func = ACTION_SPUR;
            handle = wwcoex_sim_spur_count;
          }
          else
          {
            action_func = ACTION_NONE;
          }

          action = MAX(action, action_func);
        }
        else
        {
          action = MAX(action, ACTION_SPUR_UNKNOWN);
        }
      }
    }
    else
    {
      /* Use unknown... */
      action = ACTION_SPUR_UNKNOWN;
    }

    /* Find out which nibble needs to be set */
    curr_req_carrier = i%req_car_per_ch;
    shift = (curr_req_channel*WWCOEX_MAX_CARRIER_PER_CHANNEL)+curr_req_carrier;
    
    /* Store the action into the nibble */
    query_st->action |= (action << (WWCOEX_ACTION_BIT_SIZE*shift));
  }

  /* Contruct the handle to return to common FW */
  query_st->handle = handle;

  return TRUE;

#else /* MCS_LIMITS_LOW_MEMORY */

  /*Reset the mask */
  query_st->action = 0;
  return TRUE;
  
#endif /* MCS_LIMITS_LOW_MEMORY */

}

/*============================================================================

FUNCTION CXM_QUERY_SPUR_ACTION

DESCRIPTION
  Query the mitigation action corresponding to the two techs freq combination.
  
DEPENDENCIES
  None

RETURN VALUE
  FALSE for incorrect arguments like NULL pointer, 0 entries in the list, 
  incorrect freqId
  TRUE otherwise

SIDE EFFECTS
  None

============================================================================*/
boolean cxm_query_spur_action
(
  cxm_spur_query_s* query_st
)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  uint32 i,j,shift;
  uint32 num_request_ids, num_conflict_ids;
  cxm_action_type action = ACTION_NONE;
  cxm_action_type action_func = ACTION_NONE;
  uint32 req_car_per_ch =0, curr_req_channel = 0, curr_req_carrier = 0;
  uint32 conf_car_per_ch =0, curr_conf_channel = 0;
  uint16 temp_handle = 0, handle = 0, count=0;

   /* Check input */
  if ( query_st == NULL ||
       query_st->num_conflicting_ch == 0  || query_st->conflicting_ids == NULL   ||
       query_st->num_requesting_ch == 0   || query_st->requesting_ids == NULL    ||
       query_st->requesting_ch_id == NULL || query_st->conflicting_ch_id == NULL
     )
  {
    return FALSE;
  }

  /*Reset the mask and handle*/
  query_st->action  = ACTION_NONE;
  query_st->handle  = WWCOEX_INVALID_SPUR_HANDLE;

  /* TX is not a victim of Spur */
  if((wwcoex_is_spur_mitigation_en  == FALSE) ||
     (query_st->requesting_activity == ACTIVITY_TX))
  {
    return TRUE;
  }

  /* Valid input to simulator so use simulator */
  if(wwcoex_sim_spur_count > 0)
  {
    return cxm_query_spur_action_for_simulator(query_st);
  }

  /* Number of requesting IDs and conflicting IDs are a function of 
     number of requesting channel and carriers per channel */
  req_car_per_ch = query_st->num_carr_per_requesting_ch;
  conf_car_per_ch = query_st->num_carr_per_conflicting_ch;
  
  num_request_ids  = query_st->num_requesting_ch*req_car_per_ch;
  num_conflict_ids = query_st->num_conflicting_ch*conf_car_per_ch;

  /* Scan through each entry in the table to get the handle */
  for (i=0; i < num_request_ids; i++)
  {
    curr_req_channel = i/req_car_per_ch;
    
    if (query_st->requesting_ids[i] != WWCOEX_UNKNOWN_FREQID)
    {
      /* Start with no action */
      action = ACTION_NONE;
      
      for (j=0; j < num_conflict_ids; j++)
      {
        curr_conf_channel = j/conf_car_per_ch;
        
        /* If any of the Ids are unknown then return action as unknown */
        if ( query_st->conflicting_ids[j] != WWCOEX_UNKNOWN_FREQID )
        {
          /* Do a look up only if channel ID does not match */
          if(query_st->requesting_ch_id[curr_req_channel] != 
             query_st->conflicting_ch_id[curr_conf_channel])
          {
            /* Lookup conflict table for freqids*/
            action_func =  wwcoex_tbl_freqid_pair_spur_lookup( 
                                            query_st->requesting_ids[i],
                                            query_st->conflicting_ids[j],
                                            &temp_handle);
          }
          else
          {
            action_func = ACTION_NONE;
          }
          
          /* Multi carrier spurs are stored in contigous memory location
             Action is the MAX action for multi carriers comparission
             Handle is the MIN index for muti carrier comparission 
             count is the sum of number of spurs founds 
          */

          /* TODO: Overflow in mc cases is not handled */
          action = MAX(action, action_func);
          handle = MIN(handle,temp_handle);
          count  = count + (temp_handle & 0xF);
        }
        else
        {
          action = MAX(action, ACTION_SPUR_UNKNOWN);
        }
      }
    }
    else
    {
      /* Use unknown... */
      action = ACTION_SPUR_UNKNOWN;
    }

    /* Find out which nibble needs to be set */
    curr_req_carrier = i%req_car_per_ch;
    shift = (curr_req_channel*WWCOEX_MAX_CARRIER_PER_CHANNEL)+curr_req_carrier;
    
    /* FW action is stored per carrier per channel. 
       The max carriers per channel is 3 
       For each channel the results are stored in 12 consequitive bits or 3 nibbles
       The results for first channel start at bit 0
       the results for second channel start at bit 12 
    */
    query_st->action |= (action << (WWCOEX_ACTION_BIT_SIZE*shift));
  }

  /* Contruct the handle to return to common FW */
  query_st->handle = (handle&0xFFF0)+(count&0xF);

  return TRUE;

#else /* MCS_LIMITS_LOW_MEMORY */

  /*Reset the mask */
  query_st->action = 0;
  return TRUE;
  
#endif /* MCS_LIMITS_LOW_MEMORY */

}

/*============================================================================

FUNCTION CXM_QUERY_SPUR_INFO

DESCRIPTION
  Query the spur setting corresponding to a handle.
  This API is called from Tech FW to MCS to get spur informaton
  
DEPENDENCIES
  None

RETURN VALUE
  FALSE for incorrect arguments like NULL pointer or invalid handle
  TRUE otherwise

SIDE EFFECTS
  None

============================================================================*/
boolean cxm_get_spur_info_from_handle 
(
  uint16               spur_handle,
  cxm_spur_info_s      *spur_info,
  uint32               *num_spurs
)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  int i;
  uint16 index, count;
  
  /* check if the input is valid */
  if((spur_info == NULL)||
     (num_spurs == NULL)||
     (spur_handle == WWCOEX_SPUR_HANDLE_UNKNOWN)||
     (spur_handle == WWCOEX_SPUR_HANDLE_NONE))
  {
    return FALSE;
  }

  /* MSB 12 bits of handle is array index
     LSB 4 bits of the handle is count */
  count = spur_handle&0xF;
  index = (spur_handle>>4)&0xFFF;
  index = index % WWCOEX_MAX_SPUR_TABLE_SIZE;

  /* check for invalid count or overflow */
  if(count > WWCOEX_MAX_CONCURRENT_SPURS)
  {
    count = WWCOEX_MAX_CONCURRENT_SPURS;
  }
  
  /* Copy the spur information and count */
  for(i=0;i<count;i++)
  {
    spur_info[i] = wwcoex_spur_info[index];
    index = (index+1)%WWCOEX_MAX_SPUR_TABLE_SIZE;
  }
  *num_spurs = count;

  return TRUE;

#else /* MCS_LIMITS_LOW_MEMORY */

  WWCOEX_DUMMY_CHECK(spur_handle);
  WWCOEX_DUMMY_CHECK(spur_info);
  WWCOEX_DUMMY_CHECK(num_spurs);
  return FALSE;

#endif /* MCS_LIMITS_LOW_MEMORY */

}



/*===========================================================================
                              railway_aggregator.c

SERVICES:

DESCRIPTION:

INITIALIZATION AND SEQUENCING REQUIREMENTS:
  Description...

Copyright (c) 2013 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "railway.h"
#include "railway_internal.h"
#include "CoreVerify.h"
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "railway_config.h"
#include "railway_adapter.h"
#include "voltage_level.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
typedef struct railway_voter_s {
    railway_corner voltage_corner;
    rail_voltage_level voltage_level;
    railway_corner active_floor;
    rail_voltage_level active_floor_voltage_level;
    uint32 explicit_voltage_in_uv;
    int id;
    int rail;
    bool suppressible;
    bool sw_enable;
    bool corner_based_voting;
    bool level_based_voting;
    railway_voter_t voter_link;       //Need a better plan for the linked list.
} railway_voter_s;

//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// External References
//---------------------------------------------------------------------------


//===========================================================================
//                             Macro Definitions
//===========================================================================

//===========================================================================
//                           Function Definitions
//===========================================================================

/*===========================================================================
FUNCTION: railway_create_voter

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
railway_voter_t railway_create_voter(int rail, bool suppressible, int id)
{
  //Rail ID goes from 0 to (num_rails-1)
  CORE_VERIFY(rail<RAILWAY_CONFIG_DATA->num_rails);
  //Store all the voters in a linked list. To do, make this nicer.
  railway_voter_t voter = (railway_voter_t)malloc(sizeof(railway_voter_s));
  CORE_VERIFY_PTR(voter);
  voter->voltage_corner = RAILWAY_NO_REQUEST;
  voter->voltage_level = RAIL_VOLTAGE_LEVEL_OFF;
  voter->active_floor = RAILWAY_NO_REQUEST;
  voter->active_floor_voltage_level = RAIL_VOLTAGE_LEVEL_OFF;
  voter->corner_based_voting = FALSE;
  voter->level_based_voting = FALSE;
  voter->explicit_voltage_in_uv = 0;
  voter->suppressible = suppressible;
  voter->id = id;
  voter->rail = rail;
  voter->sw_enable = true;
  voter->voter_link = railway.rail_state[rail].voter_list_head;   //Will set the first voter's link to NULL, after that it just pushes out the list.
  railway.rail_state[rail].voter_list_head = voter;
  return voter;
}

/*===========================================================================
FUNCTION: railway_sw_enable_vote

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void railway_sw_enable_vote(railway_voter_t voter, bool sw_enable)
{
  CORE_VERIFY(voter);
  voter->sw_enable = sw_enable;
}

/*===========================================================================
FUNCTION: railway_corner_vote

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void railway_corner_vote(railway_voter_t voter, railway_corner corner)
{
  CORE_VERIFY(voter);
  CORE_VERIFY(corner<RAILWAY_CORNERS_COUNT);
  voter->voltage_corner = corner;
  if(corner)
    voter->corner_based_voting = TRUE;

  //Check corn and voltage level voting for each subystem 
  CORE_VERIFY(!(voter->corner_based_voting && voter->level_based_voting));
}

/*===========================================================================
FUNCTION: railway_level_vote

DESCRIPTION: Vote for Volateg level

RETURN VALUE:
===========================================================================*/
void railway_level_vote(railway_voter_t voter,rail_voltage_level vol_level)
{
  CORE_VERIFY(voter);
  CORE_VERIFY(vol_level <RAIL_VOLTAGE_LEVEL_MAX);
  voter->voltage_level = vol_level;
  if(vol_level)
  {
    voter->level_based_voting = TRUE;
  }

  if(voter->level_based_voting)
    voter->voltage_corner = rpm_rail_level_remap(vol_level);

  //Check corn and voltage level voting for each subystem 
  CORE_VERIFY(!(voter->corner_based_voting && voter->level_based_voting));
}

/*===========================================================================
FUNCTION: railway_active_floor_voltage_level_vote

DESCRIPTION: Vote for Volateg level

RETURN VALUE:
===========================================================================*/
void railway_active_floor_voltage_level_vote(railway_voter_t voter,rail_voltage_level vol_level)
{
  CORE_VERIFY(voter);
  CORE_VERIFY(vol_level <RAIL_VOLTAGE_LEVEL_MAX);

  voter->active_floor_voltage_level = vol_level;

  voter->active_floor = rpm_rail_level_remap(vol_level);

}

/*===========================================================================
FUNCTION: railway_active_floor_vote

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void railway_active_floor_vote(railway_voter_t voter, railway_corner corner)
{
  CORE_VERIFY(voter);
  CORE_VERIFY(corner<RAILWAY_CORNERS_COUNT);
  voter->active_floor = corner;
}

/*===========================================================================
FUNCTION: railway_uv_vote

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void railway_uv_vote(railway_voter_t voter, uint32 microvolts)
{
  CORE_VERIFY(voter);

  //Only allow this API for rails that support explicit voltage requests
  CORE_VERIFY(!microvolts || RAILWAY_CONFIG_DATA->rails[voter->rail].supports_explicit_voltage_requests);

  voter->explicit_voltage_in_uv = microvolts;
}

/*===========================================================================
FUNCTION: railway_aggregated_voltage_target_uv

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void railway_aggregated_voltage_target_uv(int rail, railway_settings* target_setting, bool for_sleep)
{
  CORE_VERIFY_PTR(target_setting);
  //Rail ID goes from 0 to (num_rails-1)
  CORE_VERIFY(rail<RAILWAY_CONFIG_DATA->num_rails);
    
  if(RAILWAY_CONFIG_DATA->rails[rail].aggregation_workaround)
  {
    RAILWAY_CONFIG_DATA->rails[rail].aggregation_workaround(rail);
  }
    
    const uint32* corner_voltages = railway.rail_state[rail].corner_uvs;
    target_setting->mode = RAILWAY_NO_REQUEST;
    target_setting->microvolts = 0;
    target_setting->vol_level = RAIL_VOLTAGE_LEVEL_OFF;
    
    
  railway_voter_t voter = railway.rail_state[rail].voter_list_head;
  bool sw_enable = false;

  //First, find highest corner vote.
  while (voter)
  {
    if(!(for_sleep && voter->suppressible)) //Ignore suppressible votes if we're asking for sleep.
    {
      if(voter->voltage_corner > target_setting->mode)
      {
        target_setting->mode = voter->voltage_corner;
      }
            
      if(voter->voltage_corner!=RAILWAY_NO_REQUEST)
      {
        sw_enable = sw_enable || voter->sw_enable;
      }
    }
    if(!for_sleep)  //Factor in active floor requests.
    {
      if(voter->active_floor > target_setting->mode)
      {
        target_setting->mode = voter->active_floor;
      }
    }
    voter = (railway_voter_t)voter->voter_link;
  }

    //Now fill in the voltage based on the corner_voltages
    target_setting->microvolts = corner_voltages[target_setting->mode];
    
    //Fill voltage level 
    target_setting->vol_level = rpm_rail_corner_remap(target_setting->mode);


  //Now see if there are any explicit voltage votes that override the corner voltage.
  voter = railway.rail_state[rail].voter_list_head;
  while (voter)
  {
    if(!(for_sleep && voter->suppressible)) //Ignore suppressible votes if we're asking for sleep.
    {
      if(voter->explicit_voltage_in_uv > target_setting->microvolts)
      {
        target_setting->microvolts = voter->explicit_voltage_in_uv;
      }
            
      if(voter->explicit_voltage_in_uv!=0)
      {
        sw_enable = sw_enable || voter->sw_enable;
      }
    }
    voter = (railway_voter_t)voter->voter_link;
  }
    
  // If no clients have both a non-zero voltage/corner request AND sw_enable set
  // then the target is 0/off.
  if(!sw_enable)
  {
    target_setting->microvolts = 0;
    target_setting->mode = RAILWAY_NO_REQUEST;
    target_setting->vol_level = RAIL_VOLTAGE_LEVEL_OFF;
  }
}

static void railway_do_set_corner_voltage(int rail, railway_corner corner, uint32 microvolts)
{
    CORE_VERIFY(rail<RAILWAY_CONFIG_DATA->num_rails);
    CORE_VERIFY(corner<RAILWAY_CORNERS_COUNT);

    if(corner>0 && railway.rail_state[rail].corner_uvs[corner-1]>microvolts)
    {
        railway_do_set_corner_voltage(rail, (railway_corner)(corner-1), microvolts); //Should recurse such that voltages for modes can't overlap
    }

    if(corner<(RAILWAY_CORNERS_COUNT-1) && railway.rail_state[rail].corner_uvs[corner+1]<microvolts)
    {
        railway_do_set_corner_voltage(rail, (railway_corner)(corner+1), microvolts); //Should recurse such that voltages for modes can't overlap
    }
    
	railway.rail_state[rail].corner_uvs[corner]=microvolts;
}

void railway_set_corner_voltage(int rail, railway_corner corner, uint32 microvolts)
{
    railway_do_set_corner_voltage(rail, corner, microvolts);
    
    //Now verify that the voltages are increasing with the corner
    for(int i=0; i<RAILWAY_CORNERS_COUNT-1; i++)
    {
        CORE_VERIFY(railway.rail_state[rail].corner_uvs[i] <= railway.rail_state[rail].corner_uvs[i+1]);
    }
}
uint32 railway_get_corner_voltage(int rail, railway_corner corner)
{
    CORE_VERIFY(rail<RAILWAY_CONFIG_DATA->num_rails);
    CORE_VERIFY(corner<RAILWAY_CORNERS_COUNT);

	return railway.rail_state[rail].corner_uvs[corner];
}

bool railway_voter_preventing_vdd_min(int id)
{
    //RailID 1 is Cx. Should probably add a #define.
    railway_voter_t voter = railway.rail_state[1].voter_list_head;
    while (voter)
    {
        if(voter->id==id)
        {
            if((!voter->suppressible) &&
                ((voter->voltage_corner>RAILWAY_RETENTION) ||
                 (voter->explicit_voltage_in_uv>railway.rail_state[1].corner_uvs[RAILWAY_RETENTION])))
                return true;
            else
                return false;
        }
        voter = voter->voter_link;
    }
    return false;
}

/*===========================================================================
FUNCTION: railway_get_current_settings

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void railway_get_current_settings(int rail, railway_settings* settings)
{
  CORE_VERIFY(rail < RAILWAY_CONFIG_DATA->num_rails);
  CORE_VERIFY_PTR(settings);

  memcpy(settings, &railway.rail_state[rail].current_active, sizeof(railway_settings));
}

/*===========================================================================
FUNCTION: railway_get_sleep_settings

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void railway_get_sleep_settings(int rail, railway_settings* settings)
{
  railway_settings rail_settings;
  CORE_VERIFY(rail < RAILWAY_CONFIG_DATA->num_rails);
  CORE_VERIFY_PTR(settings);
  railway_aggregated_voltage_target_uv(rail, &rail_settings, true);

  memcpy(settings, &rail_settings, sizeof(railway_settings));

}
/*===========================================================================
FUNCTION: rail_id

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
int rail_id(const char* rail)
{
  const railway_rail_config_t *rail_data = NULL;
    
  for(int i = 0; i < RAILWAY_CONFIG_DATA->num_rails; ++i)
  {
    rail_data = &RAILWAY_CONFIG_DATA->rails[i];
    if(!strcmp(rail, rail_data->vreg_name))
      return i;
  }
    
  return RAIL_NOT_SUPPORTED_BY_RAILWAY;
}

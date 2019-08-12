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
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include "railway_config.h"
#include "CoreVerify.h"
#include "DALSys.h"
#include "stringl/stringl.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
typedef struct railway_voter_s {
    railway_corner voltage_corner;
    int id;
    int rail;
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
railway_voter_t railway_create_voter(int rail, int id)
{
  //Rail ID goes from 0 to (num_rails-1)
  CORE_VERIFY(rail<RAILWAY_CONFIG_DATA->num_rails);
  //Store all the voters in a linked list. To do, make this nicer.
  railway_voter_t voter = NULL;
  
  DALResult dalResult = DALSYS_Malloc(sizeof(railway_voter_s), (void**)&voter);
  CORE_VERIFY_PTR(voter);
  CORE_VERIFY(dalResult==DAL_SUCCESS);
  
  voter->voltage_corner = RAILWAY_NO_REQUEST;
  voter->id = id;
  voter->rail = rail;
  voter->voter_link = railway.rail_state[rail].voter_list_head;   //Will set the first voter's link to NULL, after that it just pushes out the list.
  railway.rail_state[rail].voter_list_head = voter;
  return voter;
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
}

/*===========================================================================
FUNCTION: railway_aggregated_voltage_target_uv

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void railway_aggregated_voltage_target_uv(int rail, railway_settings* target_setting)
{
    CORE_VERIFY_PTR(target_setting);
    //Rail ID goes from 0 to (num_rails-1)
    CORE_VERIFY(rail<RAILWAY_CONFIG_DATA->num_rails);

    const uint32* corner_voltages = railway.rail_state[rail].corner_uvs;
    target_setting->mode = RAILWAY_NO_REQUEST;
    target_setting->microvolts = 0;

    railway_voter_t voter = railway.rail_state[rail].voter_list_head;

    //First, find highest corner vote.
    while (voter)
    {
        if(voter->voltage_corner > target_setting->mode)
        {
            target_setting->mode = voter->voltage_corner;
        }
        voter = (railway_voter_t)voter->voter_link;
    }

    //Now fill in the voltage based on the corner_voltages
    target_setting->microvolts = corner_voltages[target_setting->mode];
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

/*===========================================================================
FUNCTION: railway_get_current_settings

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void railway_get_current_settings(int rail, railway_settings* settings)
{
  CORE_VERIFY(rail < RAILWAY_CONFIG_DATA->num_rails);
  CORE_VERIFY_PTR(settings);

  memscpy(settings, sizeof(railway_settings), &railway.rail_state[rail].current_active, sizeof(railway_settings));
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

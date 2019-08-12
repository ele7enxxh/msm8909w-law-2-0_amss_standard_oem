/*===========================================================================
  railway_adapter.c

  SERVICES:

  DESCRIPTION:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:
  Description...

  Copyright (c) 2015 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government. Diversion contrary to U.S. law prohibited.
  ===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include <stdlib.h>

#include "rpmserver.h"
#include "../common/vect.h"

#include "railway.h"
#include "voltage_level.h"

#include "railway_internal.h"
#include "railway_adapter.h"
#include "railway_config.h"
#include "CoreVerify.h"
#include "pmapp_npa.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------

typedef struct
{
  int rail_num;
  vect_t  *voter_handles;
} cb_data_t;

typedef struct
{
  bool corner_vote;
  bool level_vote;
} rpm_rail_vote;

//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------
bool ghost_vote;

rpm_rail_vote rpm_master_rail_vote[RPM_MASTER_MAX_COUNT];

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------
//This translation is required because masters vote for levels 0-6, but we support more levels internally in the RPM.
//Masters don't vote for level RAILWAY_SVS_HIGH, RAILWAY_TURBO_HIGH nor RAILWAY_SUPER_TURBO_NO_CPR
const static railway_corner railway_master_corner_remap[] =
{
  RAILWAY_NO_REQUEST,
  RAILWAY_RETENTION,
  RAILWAY_SVS_LOW,
  RAILWAY_SVS_SOC,
  RAILWAY_NOMINAL,
  RAILWAY_TURBO,
  RAILWAY_SUPER_TURBO,
};

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// External References
//---------------------------------------------------------------------------
extern void railway_remap_target_specific_corners(rpm_translation_info *info, railway_voter_irep *req);

//===========================================================================
//                             Macro Definitions
//===========================================================================
#define RAILWAY_CORNER_REMAP_TABLE_SIZE (sizeof(railway_master_corner_remap)/sizeof(railway_corner))

//===========================================================================
//                           Function Definitions
//===========================================================================

/*===========================================================================
  FUNCTION: set_ghost_vote

  DESCRIPTION:

  RETURN VALUE:
  ===========================================================================*/
void set_ghost_vote(bool do_ghost_vote)
{
  ghost_vote=do_ghost_vote;
}

/*===========================================================================
  FUNCTION: rpm_rail_level_remap

  DESCRIPTION:map voltage level to corner

  RETURN VALUE:
  ===========================================================================*/
railway_corner rpm_rail_level_remap(rail_voltage_level voltage_level)
{
    railway_corner corner_val = RAILWAY_CORNERS_COUNT;
    CORE_VERIFY(voltage_level < RAIL_VOLTAGE_LEVEL_MAX);

    switch(voltage_level)
    {
      case RAIL_VOLTAGE_LEVEL_OFF:
        corner_val = RAILWAY_NO_REQUEST;
        break;
      case RAIL_VOLTAGE_LEVEL_RETENTION:
        corner_val = RAILWAY_RETENTION;
        break;
      case RAIL_VOLTAGE_LEVEL_SVS_LOW:
        corner_val = RAILWAY_SVS_LOW;
        break;
      case RAIL_VOLTAGE_LEVEL_SVS:
        corner_val = RAILWAY_SVS_SOC;
        break;
      case RAIL_VOLTAGE_LEVEL_SVS_HIGH:
        corner_val = RAILWAY_SVS_HIGH;
        break;
      case RAIL_VOLTAGE_LEVEL_NOMINAL:
        corner_val = RAILWAY_NOMINAL;
        break;
      case RAIL_VOLTAGE_LEVEL_NOMINAL_HIGH:
        corner_val = RAILWAY_NOMINAL_HIGH;
        break;
      case RAIL_VOLTAGE_LEVEL_TURBO:
        corner_val = RAILWAY_SUPER_TURBO;
        break;
      default:
	CORE_VERIFY(0);
    }
    return corner_val;          
}
/*===========================================================================
  FUNCTION: rpm_rail_corner_remap

  DESCRIPTION:map corner to voltage level

  RETURN VALUE:
  ===========================================================================*/
rail_voltage_level rpm_rail_corner_remap(railway_corner corn)
{
    rail_voltage_level level_val = RAIL_VOLTAGE_LEVEL_MAX;
    CORE_VERIFY(corn < RAILWAY_CORNERS_COUNT);

    switch(corn)
    {
      case RAILWAY_NO_REQUEST:
        level_val = RAIL_VOLTAGE_LEVEL_OFF;
        break;
      case RAILWAY_RETENTION:
        level_val = RAIL_VOLTAGE_LEVEL_RETENTION;
        break;
      case RAILWAY_SVS_LOW:
        level_val = RAIL_VOLTAGE_LEVEL_SVS_LOW;
        break;
      case RAILWAY_SVS_SOC:
        level_val = RAIL_VOLTAGE_LEVEL_SVS;
        break;
      case RAILWAY_SVS_HIGH:
        level_val = RAIL_VOLTAGE_LEVEL_SVS_HIGH;
        break;
      case RAILWAY_NOMINAL:
        level_val = RAIL_VOLTAGE_LEVEL_NOMINAL;
        break;
      case RAILWAY_NOMINAL_HIGH:
        level_val = RAIL_VOLTAGE_LEVEL_NOMINAL_HIGH;
        break;
      case RAILWAY_SUPER_TURBO:
        level_val = RAIL_VOLTAGE_LEVEL_TURBO;
        break;
      default:
	CORE_VERIFY(0);
    }
    return level_val;          
}
/*===========================================================================
  FUNCTION: railway_xlate

  DESCRIPTION:

  RETURN VALUE:
  ===========================================================================*/
void railway_xlate(rpm_translation_info *info)
{
  uint32               type, length;
  uint32               *value;
  railway_voter_irep *req = info->dest_buffer;
  rpm_master_client master_id = (rpm_master_client)info->client;
  /* Read until we run out of KVPs. */
  while( !kvp_eof(info->new_kvps) )
  {
    kvp_get( info->new_kvps, &type, &length, (const char **)&value);

    if(length==0)
    {   /* Implies we need to invalidate any previous request the client 
           might have on this kvp type. */
      switch(type)
      {
        case PM_NPA_KEY_SOFTWARE_ENABLE:
          req->sw_enable = 0;
          break;
        case PM_NPA_KEY_CORNER_LEVEL_KEY:
        case RAILWAY_INTERNAL_KEY_CORNER_LEVEL_KEY:
          req->corner = RAILWAY_NO_REQUEST;
          break;
        case PM_NPA_KEY_LEVEL_RAIL_VOLTAGE:
          req->voltage_level = RAIL_VOLTAGE_LEVEL_OFF;
          break;
        case PM_NPA_KEY_MICRO_VOLT:
          req->microvolts = 0;
          break;
        case PM_NPA_KEY_ACTIVE_FLOOR:
          req->active_floor = RAILWAY_NO_REQUEST;
          break;
        case PM_NPA_KEY_ACTIVE_FLOOR_LEVEL:
          req->active_floor_voltage_level = RAIL_VOLTAGE_LEVEL_OFF;
          break;
        default:
          break;
      }
    }
    else
    {
      switch(type)
      {
        case PM_NPA_KEY_SOFTWARE_ENABLE:
          CORE_VERIFY(sizeof(npa_resource_state) == length);
          req->sw_enable = *value;
          break;
        case PM_NPA_KEY_CORNER_LEVEL_KEY:
          CORE_VERIFY(sizeof(npa_resource_state) == length);
          CORE_VERIFY(*value<RAILWAY_CORNER_REMAP_TABLE_SIZE);
	      CORE_VERIFY((railway_corner)*value < RAILWAY_SUPER_TURBO_NO_CPR);
          req->corner = railway_master_corner_remap[*value];
		  
		  if (req->corner)
		  {
		      rpm_master_rail_vote[master_id].corner_vote = TRUE;			  
	          CORE_VERIFY(!(rpm_master_rail_vote[master_id].level_vote));
		  }
          break;
        case PM_NPA_KEY_MICRO_VOLT:
          CORE_VERIFY(sizeof(npa_resource_state) == length);
          req->microvolts = *value;
          break;
        case PM_NPA_KEY_ACTIVE_FLOOR:
          CORE_VERIFY(sizeof(npa_resource_state) == length);
          CORE_VERIFY(*value<RAILWAY_CORNER_REMAP_TABLE_SIZE);
	      CORE_VERIFY((railway_corner)*value < RAILWAY_SUPER_TURBO_NO_CPR);
          req->active_floor = railway_master_corner_remap[*value];
		  
		  if (req->active_floor)
		  {
	          rpm_master_rail_vote[master_id].corner_vote = TRUE;
			  CORE_VERIFY(!(rpm_master_rail_vote[master_id].level_vote));
		  }
          break;
        case RAILWAY_INTERNAL_KEY_CORNER_LEVEL_KEY:
          CORE_VERIFY(sizeof(npa_resource_state) == length);
          req->corner = (railway_corner)*value;
		  
		  if (req->corner)
		  {
	          rpm_master_rail_vote[master_id].corner_vote = TRUE;
	          CORE_VERIFY(!(rpm_master_rail_vote[master_id].level_vote));
		  }
          break;
        case PM_NPA_KEY_LEVEL_RAIL_VOLTAGE:
          CORE_VERIFY(sizeof(npa_resource_state) == length);
          req->voltage_level = (rail_voltage_level)*value;
		  
		  if (req->voltage_level)
		  {
	          rpm_master_rail_vote[master_id].level_vote = TRUE;
	          CORE_VERIFY(!(rpm_master_rail_vote[master_id].corner_vote));
		  }
	      break;
        case PM_NPA_KEY_ACTIVE_FLOOR_LEVEL:
          CORE_VERIFY(sizeof(npa_resource_state) == length);
          req->active_floor_voltage_level = (rail_voltage_level)*value;
		  
		  if(req->active_floor_voltage_level)
		  {
	          rpm_master_rail_vote[master_id].level_vote = TRUE;			
	          CORE_VERIFY(!(rpm_master_rail_vote[master_id].corner_vote));
		  }
          break;
        default:
          break;
      }
    }
  }
  
  // Remap the corner to suite client requirement
  railway_remap_target_specific_corners(info, req);
}

/*===========================================================================
  FUNCTION: railway_apply

  DESCRIPTION:

  RETURN VALUE:
  ===========================================================================*/
void railway_apply(rpm_application_info *info)
{
  cb_data_t   *rail_info = (cb_data_t *)info->cb_data;
  railway_voter_irep* new_irep;
  railway_voter_irep  invalidated_irep;

  //info->new_state can be NULL in the case that a request has been invalidated.
  //In that case, use a zeroed-out irep.
  if(info->new_state)
  {
    new_irep = (railway_voter_irep*)info->new_state;
  }
  else
  {
    memset(&invalidated_irep, 0, sizeof(railway_voter_irep));
    new_irep = &invalidated_irep;
  }

  uint32 voter = info->client;
  int rail_id = rail_info->rail_num;
  CORE_VERIFY(rail_id!=-1);

  // Ensure we have enough room to store a voter handle for this voter.
  if(voter >= vect_size(rail_info->voter_handles))
    vect_resize(rail_info->voter_handles, voter+1);

  // Ensure we have an internal voter handle.
  railway_voter_t h = (railway_voter_t)vect_at(rail_info->voter_handles, voter);
  if(!h)
  {
    bool is_external = info->client < rpm_get_num_ees();
    h = railway_create_voter(rail_id, is_external ? false : true, voter);
    vect_set(rail_info->voter_handles, voter, h);
  }

  railway_corner_vote(h, new_irep->corner);
  railway_level_vote(h, new_irep->voltage_level);
  railway_uv_vote(h, new_irep->microvolts);
  railway_sw_enable_vote(h, new_irep->sw_enable);
  railway_active_floor_vote(h, new_irep->active_floor);
  railway_active_floor_voltage_level_vote(h, new_irep->active_floor_voltage_level);

  if(!ghost_vote)
  {
    railway_transition_rail(rail_id);
  }

}

/*===========================================================================
  FUNCTION: railway_adapter_init

  DESCRIPTION:

  RETURN VALUE:
  ===========================================================================*/
void railway_adapter_init(void)
{
  for(int i = 0; i < RAILWAY_CONFIG_DATA->num_rails; i++)
  {
    const railway_rail_config_t *rail = &RAILWAY_CONFIG_DATA->rails[i];

    cb_data_t *cb_data = malloc(sizeof(cb_data_t));
    CORE_VERIFY_PTR(cb_data);
    cb_data->rail_num = i;
    cb_data->voter_handles = vect_create();
    CORE_VERIFY(cb_data->voter_handles);

    rpm_shadow_resource(rail->vreg_type, rail->vreg_num, sizeof(railway_voter_irep),
                          railway_xlate, railway_apply, cb_data);
  }
}

/*! \file geran_multi_sim.c

  Provides common functions for use by GERAN entities supporting Multi SIM.

                Copyright (c) 2009-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gcommon/src/geran_multi_sim.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $ */
/* $Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "geran_multi_sim.h"
#include "geran_dual_sim_g.h"
#include "geran_multi_sim_api.h"
#include <stringl.h>
#include "trm.h"
#ifndef PERLUTF
#include "ftm.h"
#include "subs_prio.h"
#endif /* !PERLUTF */
#include "geran_nv.h"
#include "cm.h"
#include "rex.h"
#include "gs.h"
/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

#ifdef PERLUTF
#error code not present
#endif /* PERLUTF */

/* define the GAS ID that must be assumed for multi-mode operations */
#define GERAN_MULTI_MODE_GAS_ID GERAN_ACCESS_STRATUM_ID_1

#ifndef FEATURE_TRIPLE_SIM
#define OTHER_GAS_ID(gid) ((gid) != GERAN_ACCESS_STRATUM_ID_1 ? GERAN_ACCESS_STRATUM_ID_1 : GERAN_ACCESS_STRATUM_ID_2)
#endif /* !FEATURE_TRIPLE_SIM */

#define GERAN_NUM_GTA_VETO_CLIENTS 8 /* number of different clients allowed to vote against GPRS Tuneaway */

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/
typedef struct
{
  sys_modem_as_id_e_type as_id;
  gas_id_t               gas_id_list[NUM_GERAN_DATA_SPACES];
} gas_map_table_t;

typedef struct
{
  /* array of votes, one per client */
  boolean client_vote[GERAN_NUM_GTA_VETO_CLIENTS];
} gta_veto_t;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 *--------------------------------------------------------------------------*/
static rex_crit_sect_type gta_lock_crit_sec;
static gta_veto_t         gta_veto_status[NUM_GERAN_DATA_SPACES];

#define GTA_VOTE_LOCK()   rex_enter_crit_sect(&gta_lock_crit_sec)
#define GTA_VOTE_UNLOCK() rex_leave_crit_sect(&gta_lock_crit_sec)

static geran_traffic_reason_t geran_traffic_reason[NUM_GERAN_DATA_SPACES] = {0};

#ifdef FEATURE_TRIPLE_SIM
/* TSTS - three Layer 1 task queues exist */
static const gs_queue_id_T gl1_queue_ids[NUM_GERAN_DATA_SPACES] = {GS_QUEUE_LAYER_1,
                                                                   GS_QUEUE_LAYER_1_2,
                                                                   GS_QUEUE_LAYER_1_3};
#else
#ifdef FEATURE_DUAL_SIM
/* DSDS - two Layer 1 task queues exist */
static const gs_queue_id_T gl1_queue_ids[NUM_GERAN_DATA_SPACES] = {GS_QUEUE_LAYER_1,
                                                                   GS_QUEUE_LAYER_1_2};
#else
/* Single Standby - one Layer 1 task queue exists */
static const gs_queue_id_T gl1_queue_ids[NUM_GERAN_DATA_SPACES] = {GS_QUEUE_LAYER_1};
#endif /* FEATURE_DUAL_SIM */
#endif 

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
static sys_modem_dual_standby_pref_e_type geran_sys_standby_pref;
static sys_modem_device_mode_e_type       geran_sys_device_mode;
static sys_modem_device_mode_e_type       geran_multi_sim_mode;
static sys_modem_as_id_e_type             geran_sys_multi_mode_sub;
static sys_subs_feature_t                 geran_sys_subs_feature;

static gas_id_t                           as_id_to_gas_id[NUM_GERAN_DATA_SPACES]; /* ASID to GAS ID mapping */
static sys_modem_as_id_e_type             gas_id_to_as_id[NUM_GERAN_DATA_SPACES]; /* GAS ID to ASID mapping */
static trm_grant_return_enum_type         geran_chain_exchanged[NUM_GERAN_DATA_SPACES][2];
static geran_exchange_direction_t         geran_exchange_on_release[NUM_GERAN_DATA_SPACES];

#define IS_SINGLE_STANDBY_MODE(dm) (SYS_MODEM_DEVICE_MODE_SINGLE_SIM == (dm))
#define IS_DUAL_STANDBY_MODE(dm) (SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY == (dm))
#define IS_DUAL_ACTIVE_MODE(dm) (SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE == (dm))
#define IS_DSDX_DEVICE_MODE(dm) (IS_DUAL_STANDBY_MODE(dm) || IS_DUAL_ACTIVE_MODE(dm))

#ifdef FEATURE_TRIPLE_SIM
#define IS_TRIPLE_STANDBY_MODE(dm) (SYS_MODEM_DEVICE_MODE_TRIPLE_SIM_TRIPLE_STANDBY == (dm))
#define IS_MULTI_STANDBY_MODE(dm)  (IS_DUAL_STANDBY_MODE(dm) || IS_TRIPLE_STANDBY_MODE(dm))
static const gas_map_table_t gas_map_table[NUM_GERAN_DATA_SPACES] = {
  {SYS_MODEM_AS_ID_1,{GERAN_ACCESS_STRATUM_ID_1,GERAN_ACCESS_STRATUM_ID_2,GERAN_ACCESS_STRATUM_ID_3}},
  {SYS_MODEM_AS_ID_2,{GERAN_ACCESS_STRATUM_ID_2,GERAN_ACCESS_STRATUM_ID_1,GERAN_ACCESS_STRATUM_ID_3}},
  {SYS_MODEM_AS_ID_3,{GERAN_ACCESS_STRATUM_ID_3,GERAN_ACCESS_STRATUM_ID_2,GERAN_ACCESS_STRATUM_ID_1}}
};

#else
#define IS_TRIPLE_STANDBY_MODE(dm) (FALSE)
#define IS_MULTI_STANDBY_MODE(dm)  IS_DUAL_STANDBY_MODE(dm)
static const gas_map_table_t gas_map_table[NUM_GERAN_DATA_SPACES] = {
  {SYS_MODEM_AS_ID_1,{GERAN_ACCESS_STRATUM_ID_1,GERAN_ACCESS_STRATUM_ID_2}},
  {SYS_MODEM_AS_ID_2,{GERAN_ACCESS_STRATUM_ID_2,GERAN_ACCESS_STRATUM_ID_1}}
};
#endif /* FEATURE_TRIPLE_SIM */
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
#ifdef FEATURE_LTE
static uint32 geran_lte_activity_status[NUM_GERAN_DATA_SPACES];
#endif /* FEATURE_LTE */
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 *--------------------------------------------------------------------------*/

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)

/*!
 * \brief Returns the equivalent AS-ID
 *
 * \param gas_id
 *
 * \return sys_modem_as_id_e_type
 */
sys_modem_as_id_e_type ftm_sub(gas_id_t gas_id)
{
#ifdef FEATURE_TRIPLE_SIM
  if (GERAN_ACCESS_STRATUM_ID_3 == gas_id)
  {
    return(SYS_MODEM_AS_ID_3);
  }
  else
#endif /* FEATURE_TRIPLE_SIM */
  if (GERAN_ACCESS_STRATUM_ID_2 == gas_id)
  {
    return(SYS_MODEM_AS_ID_2);
  }

  return(SYS_MODEM_AS_ID_1);
}

/*!
 * \brief Returns the equivalent GAS-ID
 *
 * \param as_id
 *
 * \return gas_id_t
 */
gas_id_t ftm_gas_id(sys_modem_as_id_e_type as_id)
{
#ifdef FEATURE_TRIPLE_SIM
  if (SYS_MODEM_AS_ID_3 == as_id)
  {
    return(GERAN_ACCESS_STRATUM_ID_3);
  }
  else
#endif /* FEATURE_TRIPLE_SIM */
  if (SYS_MODEM_AS_ID_2 == as_id)
  {
    return(GERAN_ACCESS_STRATUM_ID_2);
  }

  return(GERAN_ACCESS_STRATUM_ID_1);
}

/*!
 * \brief Sets the NAS-ID to GAS-ID mapping, based on which NAS-ID supports multi-mode.
 */
static void determine_sub_id_mapping(void)
{
  sys_modem_as_id_e_type as_id;
  gas_id_t               gas_id;
  int                    as_index;

#ifndef FEATURE_TRIPLE_SIM
  /* NOTE: if Triple-SIM-Dual-Active is supported, this will need to be revised */
  /* If the HW config is DSDA, the default can be overridden by EFS-NV setting */
  if (SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE == geran_sys_device_mode)
  {
    // Get the EFS-NV value (if present)
    int default_rf_chain = geran_nv_get_default_rf_chain();

    // If a default RF chain is defined (-1 is undefined) then apply a custom mapping
    if (default_rf_chain >= 0)
    {
      gas_id_t default_gas_id = (gas_id_t)default_rf_chain;
      gas_id_t geran_gonly_gas_id;

      if ((default_gas_id == GERAN_ACCESS_STRATUM_ID_1) ||
          (default_gas_id == GERAN_ACCESS_STRATUM_ID_2))
      {
        geran_gonly_gas_id = OTHER_GAS_ID(default_gas_id);

        as_id_to_gas_id[SYS_MODEM_AS_ID_1] = default_gas_id;
        as_id_to_gas_id[SYS_MODEM_AS_ID_2] = geran_gonly_gas_id;

        gas_id_to_as_id[default_gas_id] = SYS_MODEM_AS_ID_1;
        gas_id_to_as_id[geran_gonly_gas_id] = SYS_MODEM_AS_ID_2;

        // Exit
        return;
      }
    }
  }
#endif /* !FEATURE_TRIPLE_SIM */

  /* use the multimode ID to look up the ASID to GAS ID mapping */
  for (as_index = 0; as_index < NUM_GERAN_DATA_SPACES; as_index++)
  {
    if (gas_map_table[as_index].as_id == geran_sys_multi_mode_sub)
    {
      memscpy(as_id_to_gas_id,
              sizeof(gas_id_t) * NUM_GERAN_DATA_SPACES,
              gas_map_table[as_index].gas_id_list,
              sizeof(gas_id_t) * NUM_GERAN_DATA_SPACES);
      break;
    }
  }

  /* now set up the reverse mapping */
  for (as_index = 0; as_index < NUM_GERAN_DATA_SPACES; as_index++)
  {
    as_id = gas_map_table[as_index].as_id;
    for (gas_id = GERAN_ACCESS_STRATUM_ID_1; gas_id <= GERAN_ACCESS_STRATUM_ID_MAX; gas_id++)
    {
      if (as_id_to_gas_id[as_index] == gas_id)
      {
        gas_id_to_as_id[gas_id] = as_id;
      }
    }
  }

  // Print-out the mapping
  for (as_index = 0; as_index < NUM_GERAN_DATA_SPACES; as_index++)
  {
    MSG_GERAN_HIGH_2("AS-ID=%d -> GAS-ID=%d", as_index, as_id_to_gas_id[as_index]);
  }

  return;
}

/*!
 * \brief Returns the multi-SIM mode.
 *
 * \param device_mode
 * \param standby_pref
 * 
 * \return sys_modem_device_mode_e_type
 */
static sys_modem_device_mode_e_type determine_multi_sim_mode(
  sys_modem_device_mode_e_type device_mode,
  sys_modem_dual_standby_pref_e_type standby_pref
)
{
  sys_modem_device_mode_e_type multi_sim_mode = device_mode;

  // If SS, then return SS
  // If DSDS, then return SS/DSDS according to upper layer SS/DS indication
  // If DSDA, then return DSDA
  if (IS_MULTI_STANDBY_MODE(device_mode))
  {
    if (SYS_MODEM_DS_PREF_SINGLE_STANDBY == standby_pref)
    {
      multi_sim_mode = SYS_MODEM_DEVICE_MODE_SINGLE_SIM;
    }
  }
  else if (IS_DUAL_ACTIVE_MODE(device_mode))
  {
    if ((SYS_MODEM_DS_PREF_SINGLE_STANDBY == standby_pref) &&
         ((geran_sys_subs_feature == SYS_SUBS_FEATURE_MODE_SGLTE) ||
          (geran_sys_subs_feature == SYS_SUBS_FEATURE_MODE_SVLTE) ||
          (geran_sys_subs_feature == SYS_SUBS_FEATURE_MODE_SRLTE)))
    {
      multi_sim_mode = SYS_MODEM_DEVICE_MODE_SINGLE_SIM;
    }
  }

  MSG_GERAN_HIGH_3("device mode=%d, standby_pref=%d => multi_sim_mode=%d)",
                   (int)device_mode,
                   (int)standby_pref,
                   (int)multi_sim_mode);

  return(multi_sim_mode);
}

#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

/*!
 * \brief Returns gl1 task queue id.
 *
 * \param as_id
 * 
 * \return gs_queue_id_T
 */

gs_queue_id_T geran_api_get_l1_queue_id(const sys_modem_as_id_e_type as_id)
{
  gas_id_t gas_id = geran_map_nas_id_to_gas_id(as_id);

  return(gl1_queue_ids[geran_get_data_space_index(gas_id)]);
}



/*-----------------------------------------------------------------------------
 * Externalised Function Definitions
 *---------------------------------------------------------------------------*/
/*!
 * \brief Returns the multi-mode sub.
 *
 * \return gas_id_t
 */
sys_modem_as_id_e_type geran_get_multi_mode_sub(void)
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  return geran_sys_multi_mode_sub;
#else
  return(SYS_MODEM_AS_ID_1);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
}

/*!
 * \brief Returns the multi-mode sub.
 *
 * \return gas_id_t
 */
gas_id_t geran_get_multi_mode_gas_id(void)
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  return(GERAN_MULTI_MODE_GAS_ID);
#else
  return(GERAN_ACCESS_STRATUM_ID_1);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
}

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/*!
 * \brief Returns the multi-mode subs Feature 
 *
 * \return geran_sys_subs_feature
 */
 
sys_subs_feature_t geran_get_multi_mode_subs_feature(void)
{
  return(geran_sys_subs_feature);
}

/*!
 * \brief Sets the the multi-mode subs Feature 
 *
 * \Input geran_sys_subs_feature
 * returns Void
 */

void geran_set_multi_mode_subs_feature(sys_subs_feature_t geran_subs_feature)
{
  geran_sys_subs_feature = geran_subs_feature;
  MSG_GERAN_HIGH_1("Sys subs feature set to %d", (int)geran_sys_subs_feature);
  return;
}

#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

/*!
 * \brief Indicates if the given sub is the multi-mode sub.
 *
 * \param gas_id
 *
 * \return boolean - TRUE if this is the multi-mode sub, FALSE otherwise.
 */
boolean geran_is_multi_mode_sub(sys_modem_as_id_e_type as_id)
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  return(geran_sys_multi_mode_sub == as_id);
#else
  return(SYS_MODEM_AS_ID_1 == as_id);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
}

/*!
 * \brief Indicates if the given GAS-ID represents the multi-mode sub.
 *
 * \param gas_id
 *
 * \return boolean - TRUE if this is the multi-mode sub, FALSE otherwise.
 */
boolean geran_is_multi_mode_gas_id(gas_id_t gas_id)
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  return(GERAN_MULTI_MODE_GAS_ID == gas_id);
#else
  return(GERAN_ACCESS_STRATUM_ID_1 == gas_id);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
}

/*!
 * \brief Converts a GERAN access stratum ID to a NAS ID.
 *
 * \param gas_id (in)
 *
 * \return sys_modem_as_id_e_type
 */
sys_modem_as_id_e_type geran_map_gas_id_to_nas_id(gas_id_t gas_id)
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  // Mapping is handled differently in FTM mode
  if (ftm_get_mode() == FTM_MODE)
  {
    return ftm_sub(gas_id);
  }

  if (gas_id <= GERAN_ACCESS_STRATUM_ID_MAX)
  {
    return gas_id_to_as_id[gas_id];
  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  return(SYS_MODEM_AS_ID_1);
}

/*!
 * \brief Converts a NAS ID to a GERAN access stratum ID.
 *
 * \param as_id (in)
 *
 * \return gas_id_t
 */
gas_id_t geran_map_nas_id_to_gas_id(sys_modem_as_id_e_type as_id)
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_1;
  // Mapping is handled differently in FTM mode
  if (ftm_get_mode() == FTM_MODE)
  {
    gas_id = ftm_gas_id(as_id);
  }

  else if (as_id != SYS_MODEM_AS_ID_NONE)
  {
    if (as_id < MAX_AS_IDS)
    {
      gas_id = as_id_to_gas_id[as_id];
    }
  }
  return check_gas_id(gas_id);
#else
  return GERAN_ACCESS_STRATUM_ID_1;
#endif
}

/*===========================================================================

FUNCTION  geran_get_diag_sub_id

DESCRIPTION
  Convert GAS ID to diag subscription ID

RETURN VALUE
  Diag Subscription Id

SIDE EFFECTS
  None

===========================================================================*/
byte geran_get_diag_sub_id(const gas_id_t gas_id)
{
  byte nas_id;

  nas_id = geran_map_gas_id_to_nas_id(gas_id);
#ifdef FEATURE_TRIPLE_SIM
  if (SYS_MODEM_AS_ID_3 == nas_id)
  {
    return(3);
  }
#endif /* FEATURE_TRIPLE_SIM */

  if (SYS_MODEM_AS_ID_2 == nas_id)
  {
    return(2);
  }

  return(1);
}

/*===========================================================================

FUNCTION       geran_get_data_space_index

DESCRIPTION    Maps GAS-ID to data-space index
               Range check as_index to ensure no buffer overflow when it is used

DEPENDENCIES   None

RETURN VALUE   int - data-space index

SIDE EFFECTS   None

===========================================================================*/

int geran_get_data_space_index(const gas_id_t gas_id)
{
  int as_idx = GERAN_DATA_SPACE_INDEX_1;

  switch (check_gas_id(gas_id)) 
  {
    case GERAN_ACCESS_STRATUM_ID_1:
    {
      as_idx = GERAN_DATA_SPACE_INDEX_1;
      break;
    }
#ifdef FEATURE_DUAL_SIM
    case GERAN_ACCESS_STRATUM_ID_2:
    {
      as_idx = GERAN_DATA_SPACE_INDEX_2;
      break;
    }
#endif
#ifdef FEATURE_TRIPLE_SIM
    case GERAN_ACCESS_STRATUM_ID_3:
    {
      as_idx = GERAN_DATA_SPACE_INDEX_3;
      break;
    }
#endif
    default:
    {      
      break;
    }
  }
  return as_idx;
}

/*!
 * \brief Sets the multi-SIM configuration.
 *
 * \param new_device_mode
 * \param new_standby_pref
 * 
 * \return boolean - TRUE if the multi-SIM mode has changed, FALSE otherwise
 */
boolean geran_set_sys_multi_sim_config(
  const sys_modem_device_mode_e_type new_device_mode,
  const sys_modem_dual_standby_pref_e_type new_standby_pref
)
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)

  sys_modem_device_mode_e_type new_multi_sim_mode;

  boolean multi_sim_mode_changed = FALSE;

  MSG_GERAN_HIGH_2("Multi-SIM device mode %d (was %d)",
                   (int)new_device_mode,
                   (int)geran_sys_device_mode);

  geran_sys_device_mode = new_device_mode;

  // Determine the GERAN multi-SIM mode from the new combination of device_mode + standby_pref
  new_multi_sim_mode = determine_multi_sim_mode(new_device_mode, new_standby_pref);

  if (new_multi_sim_mode != geran_multi_sim_mode)
  {
    geran_multi_sim_mode = new_multi_sim_mode;
    multi_sim_mode_changed = TRUE;
  }

  MSG_GERAN_HIGH_2("Multi-SIM standby pref %d (was %d)",
                   (int)new_standby_pref,
                   (int)geran_sys_standby_pref);

  geran_sys_standby_pref = new_standby_pref;

  return multi_sim_mode_changed;

#else

  NOTUSED(new_device_mode);
  NOTUSED(new_standby_pref);

  return FALSE;

#endif /* FEATURE_DUAL_SIM) || FEATURE_TRIPLE_SIM */
}

/*!
 * \brief Gets the multi-SIM preference.
 *
 * \return standby_pref
 */
sys_modem_dual_standby_pref_e_type geran_get_sys_multi_sim_pref(void)
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  /* retrieve the stored preference */
  return(geran_sys_standby_pref);
#else
  return(SYS_MODEM_DS_PREF_SINGLE_STANDBY);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
}

/*!
 * \brief Returns the multi-SIM mode.
 *
 * \return sys_modem_device_mode_e_type
 */
sys_modem_device_mode_e_type geran_get_multi_sim_mode(void)
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  return(geran_multi_sim_mode);
#else
  return(SYS_MODEM_DEVICE_MODE_SINGLE_SIM);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
}

/*!
 * \brief Called when NAS updates the multi-mode subscription information
 *
 * \param multi_mode_sub (in) - the multi-mode subscription
 */
void geran_set_multi_mode_sub(sys_modem_as_id_e_type multi_mode_sub)
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  // Take a copy of the current multi-mode sub
  sys_modem_as_id_e_type old_multi_mode_sub = geran_sys_multi_mode_sub;

  // Store the new multi-mode sub
  geran_sys_multi_mode_sub = multi_mode_sub;

  // Refresh the AS-ID <-> GAS-ID mapping
  determine_sub_id_mapping();
#ifndef PERLUTF
  //Update SP with SUB ID
  sp_update_multimode_sub(geran_sys_multi_mode_sub); 
#endif /*PERLUTF*/
  
  // Highlight any changes
  if (geran_sys_multi_mode_sub != old_multi_mode_sub)
  {
    MSG_GERAN_HIGH_1("Multi-mode sub has changed (was 0x%02X)", (int)old_multi_mode_sub);
  }
#else
  NOTUSED(multi_mode_sub);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  return;
}

/*!
 * \brief Converts sys_modem_as_id_e_type to integer
 *
 * \param as_id (in) - Subscription identifier
 *
 * \return Integer in range [0..NUM_GERAN_DATA_SPACES]
 */
static int geran_map_asid_to_index(const sys_modem_as_id_e_type as_id)
{
  int as_index;

  switch(as_id)
  {
    case SYS_MODEM_AS_ID_1:
      as_index = 0;
      break;

    case SYS_MODEM_AS_ID_2:
      as_index = 1;
      break;

    case SYS_MODEM_AS_ID_3:
      as_index = 2;
      break;

    default:
      as_index = NUM_GERAN_DATA_SPACES;
      break;
  }

  if (as_index >= NUM_GERAN_DATA_SPACES)
  {
    MSG_GERAN_ERROR_1("ASID %d is out of range", (int)as_id);
    as_index = NUM_GERAN_DATA_SPACES;
  }

  /* caller has responsibility to range-check the result */
  return(as_index);
}

/*!
 * \brief Indicates whether GPRS Tuneaway should be blocked on the specified AS ID
 *
 * \param as_id (in) - Subscription identifier
 *
 * \return TRUE if GPRS Tuneaway is vetod by one or more clients
 */
boolean geran_api_get_tuneaway_blocked(const sys_modem_as_id_e_type as_id)
{
  boolean now_blocked = FALSE;
  int     as_index;

  /* convert inbound ASID to data space index */
  as_index = geran_map_asid_to_index(as_id);

  /* sanity check the result */
  if (as_index < NUM_GERAN_DATA_SPACES)
  {
    int vote_index;

    /* acquire exclusive access to the voting table while it is queried */
    GTA_VOTE_LOCK();
    for (vote_index = 0; vote_index < GERAN_NUM_GTA_VETO_CLIENTS; vote_index++)
    {
      if (TRUE == gta_veto_status[as_index].client_vote[vote_index])
      {
        /* this client has voted against GPRS Tuneaway for this ASID */
        now_blocked = TRUE;
        break;
      }
    }
    GTA_VOTE_UNLOCK();
  }

  return(now_blocked);
}

/*!
 * \brief Maps a client_id from the enum into an integer array index
 *
 * \param client_id (in) - client from enumerated list
 *
 * \return int - position of client's vote in veto list
 */
static int geran_get_client_vote_index(const geran_api_client_id_t client_id)
{
  switch(client_id)
  {
  case GERAN_API_CLIENT_EDTM:
     return(0);
  case GERAN_API_CLIENT_NACC:
     return(1);
  case GERAN_API_CLIENT_GMM:
     return(2);
  case GERAN_API_CLIENT_PDP:
     return(3);
  case GERAN_API_CLIENT_RLC:
     return(4);
  case GERAN_API_CLIENT_GL1:
     return(5);
  case GERAN_API_CLIENT_G2X:
     return(6);
  case GERAN_API_CLIENT_CMUI:
      return(7);
  case GERAN_API_CLIENT_RESET:
    return(GERAN_NUM_GTA_VETO_CLIENTS);
  case GERAN_API_CLIENT_UNKNOWN:
  /* fallthrough*/
  default:
     break;
  }

  /* caller's responsibility to range check this return value */
  MSG_GERAN_ERROR_1("Unexpected GTA client ID %d", (int)client_id);
  return(GERAN_NUM_GTA_VETO_CLIENTS);
}

/*!
 * \brief Applies a vote for blocking GPRS Tuneaway on a specified ASID
 *
 * \param client_id (in)   - client changing their vote
 * \param gta_blocked (in) - the new vote
 * \param as_id (in)       - subscription identifier
 *
 * \return void
 */
void geran_api_set_tuneaway_blocked(const geran_api_client_id_t client_id, const boolean gta_blocked, const sys_modem_as_id_e_type as_id)
{
  gas_id_t gas_id;
  int      as_index;

  /* determine a GAS ID for logging purposes */
  gas_id = geran_map_nas_id_to_gas_id(as_id);

  /* convert inbound ASID to data space index */
  as_index = geran_map_asid_to_index(as_id);

  /* sanity check the result */
  if (as_index < NUM_GERAN_DATA_SPACES)
  {
    int vote_index;

    /* check for special reset case */
    if (GERAN_API_CLIENT_RESET == client_id)
    {
      /* acquire exclusive access to the voting table while it is updated */
      GTA_VOTE_LOCK();
      for (vote_index = 0; vote_index < GERAN_NUM_GTA_VETO_CLIENTS; vote_index++)
      {
        /* reset every vote to the value set by the caller */
        gta_veto_status[as_index].client_vote[vote_index] = gta_blocked;
      }
      GTA_VOTE_UNLOCK();

      MSG_GERAN_HIGH_1_G("GTA veto status reset to %d (1=GTA blocked)", (int)gta_blocked);
    }
    else
    {
      /* map the client ID into an index into the voting table */
      vote_index = geran_get_client_vote_index(client_id);

      if (vote_index < GERAN_NUM_GTA_VETO_CLIENTS)
      {
        /* acquire exclusive access to the voting table while it is updated */
        GTA_VOTE_LOCK();
        gta_veto_status[as_index].client_vote[vote_index] = gta_blocked;
        GTA_VOTE_UNLOCK();

        MSG_GERAN_HIGH_2_G("GTA veto client %d vote set to %d (1=GTA blocked)", (int)client_id, (int)gta_blocked);
      }
    }
  }
  else
  {
    MSG_GERAN_ERROR_2_G("Unable to apply GTA veto for client %d, ASID 0x%02X", (int)client_id, (int)as_id);
  }

  return;
}


/*===========================================================================

FUNCTION     geran_set_chain_exchanged

DESCRIPTION  Records whether an RF chain exchange has taken place

PARAMS       trm_grant: grant status for RF chain

DEPENDENCIES None

RETURN VALUE TRUE if a valid chain exchange was recorded

===========================================================================*/
static boolean geran_set_chain_exchanged(const geran_exchange_direction_t direction,
                                         const trm_grant_return_enum_type        trm_grant,
                                         const gas_id_t                   gas_id)
{
  if (GERAN_EXCHANGE_NULL == direction)
  {
    MSG_GERAN_ERROR_0_G("Invalid parameter to geran_set_chain_exchanged()");
  }
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  else if (gas_id <= GERAN_ACCESS_STRATUM_ID_MAX)
  {
    int dir_idx;

    dir_idx = (GERAN_EXCHANGE_XTOG == direction) ? 0 : 1;
    geran_chain_exchanged[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)][dir_idx] = trm_grant;
    return(TRM_DENIAL != trm_grant);
  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  return(FALSE);
}

/*===========================================================================

FUNCTION     geran_was_lock_exchanged

DESCRIPTION  Indicates whether TRM lock exchange has taken place

PARAMS       gas_id

DEPENDENCIES None

RETURN VALUE TRUE if any RF chain was exchanged

===========================================================================*/
boolean geran_was_lock_exchanged(const geran_exchange_direction_t direction,
                                 const gas_id_t                   gas_id)
{
  if (GERAN_EXCHANGE_NULL == direction)
  {
    MSG_GERAN_ERROR_0_G("Invalid parameter to geran_was_lock_exchanged()");
  }
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  else if (gas_id <= GERAN_ACCESS_STRATUM_ID_MAX)
  {
    int dir_idx;

    dir_idx = (GERAN_EXCHANGE_XTOG == direction) ? 0 : 1;
    return(TRM_DENIAL != geran_chain_exchanged[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)][dir_idx]);
  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  return(FALSE);
}

/*===========================================================================

FUNCTION     geran_clear_lock_exchanged

DESCRIPTION  Resets the specified lock exchange data to TRM_DENIED

PARAMS       direction: GERAN_EXCHANGE_GTOX or GERAN_EXCHANGE_XTOG

DEPENDENCIES None

RETURN VALUE VOID

===========================================================================*/
void geran_clear_lock_exchanged(const geran_exchange_direction_t direction, const gas_id_t gas_id)
{
  /* just call the local utility function to reset the chain */
  (void)geran_set_chain_exchanged(direction, TRM_DENIAL, gas_id);

  return;
}

/*===========================================================================

FUNCTION     geran_lock_exchange_allowed

DESCRIPTION  Indicates whether specified GAS ID is allowed to exchange locks

PARAMS       GAS ID

DEPENDENCIES Result is based on multimode support as well as device mode

RETURN VALUE TRUE if lock exchange is allowed for the specified GAS ID

===========================================================================*/
static boolean geran_lock_exchange_allowed(const gas_id_t gas_id)
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  if (geran_is_multi_mode_gas_id(gas_id))
  {
    return(IS_MULTI_STANDBY_MODE(geran_get_multi_sim_mode()));
  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  return(FALSE);
}

/*===========================================================================

FUNCTION     geran_exchange_lock

DESCRIPTION  Exchanges the TRM lock between multimode GERAN and inter-RAT client

PARAMS       direction: GERAN_EXCHANGE_GTOX or GERAN_EXCHANGE_XTOG

DEPENDENCIES None

RETURN VALUE TRUE if the procedure succeeded, FALSE otherwise

===========================================================================*/
boolean geran_exchange_lock(const geran_exchange_direction_t direction, const gas_id_t gas_id)
{
  if (GERAN_EXCHANGE_NULL == direction)
  {
    MSG_GERAN_ERROR_0_G("Invalid parameter to geran_exchange_lock()");
    return(FALSE);
  }
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
#ifndef PERLUTF
  else
  {
    trm_grant_return_enum_type trm_granted;
    trm_client_enum_t      source_client;
    trm_client_enum_t      dest_client;
    trm_get_info_return_type   output_data;

    /* only exchange for multimode subscription in multi-standby mode */
    /* Note that this deliberately excludes Dual SIM Dual Active mode */
    if (FALSE == geran_lock_exchange_allowed(gas_id))
    {
      /* no exchange will be done, but need to ensure that the */
      /* inter-RAT client is not holding the lock indefinitely */
      if ((GERAN_EXCHANGE_XTOG == direction) &&
           geran_is_multi_mode_gas_id(gas_id))
      {
        /* explicitly release the inter-RAT client */
        /* this has no effect if it holds no chain */
        grm_release(TRM_IRAT, gas_id);
      }

      /* reset the exchange state variable and return it */
      return(geran_set_chain_exchanged(direction, TRM_DENIAL, gas_id));
    }

    if (GERAN_EXCHANGE_XTOG == direction)
    {
      /* X2G inter-RAT procedure */
      source_client = TRM_IRAT; /* X-RAT (LTE, WCDMA, TDS etc.) */
      dest_client   = TRM_GSM1; /* G-RAT (multimode capable AS) */
    }
    else
    {
      /* G2X inter-RAT procedure */
      source_client = TRM_GSM1; /* G-RAT (multimode capable AS) */
      dest_client   = TRM_IRAT; /* X-RAT (LTE, WCDMA, TDS etc.) */
    }

    /* make sure the source client already owns an RF chain */
    trm_granted = grm_get_granted(source_client, &output_data, gas_id);
    if (TRM_DENIAL != trm_granted)
    {
      trm_client_exchange_input_info input;

      /* nullify any existing unlock callback to avoid pre-emption */
      grm_retain_lock(source_client, NULL, gas_id);

      MSG_GERAN_HIGH_3_G("Client %d granted chain %d, RF device %d",
                         (int)source_client, (int)trm_granted, (int)(output_data.info.client_info.dev));

      /* clear down the state of the receiving client */
      grm_release(dest_client, gas_id);

      /* and then swap the states, so dest_client is holding a chain */
      input.client_1 = dest_client;
      input.client_2 = source_client;
      trm_client_exchange(&input);

      /* clear down the state of the source client */
      grm_release(source_client, gas_id);

      MSG_GERAN_HIGH_2_G("Inter-RAT exchange completed for client %d to %d",
                         (int)source_client, (int)dest_client);

      /* update the exchange state variable and return it */
      return(geran_set_chain_exchanged(direction, trm_granted, gas_id));
    }
    else
    {
      MSG_GERAN_ERROR_1_G("Client %d does not hold any RF chain", (int)source_client);
    }
  }
#endif /* !PERLUTF */
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  /* reset the exchange state variable and return it */
  return(geran_set_chain_exchanged(direction, TRM_DENIAL, gas_id));
}

/*===========================================================================

FUNCTION     geran_set_exchange_on_release

DESCRIPTION  Sets the flag to specify trm_exchange() instead of trm_release

PARAMS       direction: GERAN_EXCHANGE_NULL, GERAN_EXCHANGE_GTOX, or GERAN_EXCHANGE_XTOG

DEPENDENCIES None

RETURN VALUE VOID

===========================================================================*/
void geran_set_exchange_on_release(const geran_exchange_direction_t direction, const gas_id_t gas_id)
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  if (gas_id <= GERAN_ACCESS_STRATUM_ID_MAX)
  {
    geran_exchange_on_release[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)] = direction;
  }
  else
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
  {
    MSG_GERAN_HIGH_2("Exchange direction %d not supported for GAS ID 0x%02X", (int)direction, (int)gas_id);
  }

  return;
}

/*===========================================================================

FUNCTION     geran_get_exchange_on_release

DESCRIPTION  Gets the flag to determine trm_exchange() instead of trm_release

PARAMS       GAS ID

DEPENDENCIES None

RETURN VALUE GERAN_EXCHANGE_NULL, GERAN_EXCHANGE_GTOX or GERAN_EXCHANGE_XTOG

===========================================================================*/
geran_exchange_direction_t geran_get_exchange_on_release(const gas_id_t gas_id)
{
  if (geran_lock_exchange_allowed(gas_id))
  {
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
    if (gas_id <= GERAN_ACCESS_STRATUM_ID_MAX)
    {
      return(geran_exchange_on_release[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)]);
    }
    else
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
    {
      MSG_GERAN_HIGH_0("No inter-RAT lock exchange info available");
    }
  }

  return(GERAN_EXCHANGE_NULL);
}

/*===========================================================================

FUNCTION     geran_in_multi_standby_mode

DESCRIPTION  Indicates whether multi-standby (DSDS or TSTS) mode is active

PARAMS       None

DEPENDENCIES Device mode supplied by MMCP

RETURN VALUE TRUE if device mode is DSDS or TSTS, FALSE for SSSS and DSDA

===========================================================================*/
boolean geran_in_multi_standby_mode(void)
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  sys_modem_device_mode_e_type dev_mode = geran_get_multi_sim_mode();

  if (IS_MULTI_STANDBY_MODE(dev_mode))
  {
    return(TRUE);
  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  return(FALSE);
}

/*!
 * \brief Sets the reason for the pending GERAN traffic operation.
 *
 * \param traffic_reason (in) - new reason to be set
 * \param gas_id (in)         - access stratum ID
 */
void geran_set_traffic_reason(const geran_traffic_reason_t traffic_reason, const gas_id_t gas_id)
{
  if (gas_id <= GERAN_ACCESS_STRATUM_ID_MAX)
  {
    geran_traffic_reason[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)] = traffic_reason;
  }

  return;
}

/*!
 * \brief Gets the reason for the current GERAN traffic operation
 *
 * \param ps_ptr (out)        - set to TRUE if reason is for PS
 * \param gas_id (in)         - access stratum ID
 *
 * \return geran_traffic_reason_t - Most recent value of global traffic reason
 */
geran_traffic_reason_t geran_get_traffic_reason(boolean *ps_ptr, const gas_id_t gas_id)
{
  geran_traffic_reason_t local_reason;

  if (gas_id <= GERAN_ACCESS_STRATUM_ID_MAX)
  {
    local_reason = geran_traffic_reason[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)];
  }
  else
  {
    local_reason = GERAN_TRAFFIC_DEFAULT;
  }

  /* caller may optionally indicate that it needs to know whether */
  /* the value returned here corresponds to (E)GPRS packet access */
  if (ps_ptr != NULL)
  {
    if ((GERAN_TRAFFIC_PS_SIGNALLING == local_reason) ||
        (GERAN_TRAFFIC_PS_DATA       == local_reason) ||
        (GERAN_TRAFFIC_PS_OTHER      == local_reason))
    {
      *ps_ptr = TRUE;
    }
    else
    {
      *ps_ptr = FALSE;
    }
  }

  return(local_reason);
}

/*!
 * \To initialize as_id to gas_id and vice-versa conversion, this is required
 * \for GPLT as RR is not active to update it.
 */
void geran_as_id_gas_id_init( void )
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
    int default_rf_chain = 1;

    // If a default RF chain is defined (-1 is undefined) then apply a custom mapping
    if (default_rf_chain >= 0)
    {
      gas_id_t default_gas_id = (gas_id_t)default_rf_chain;
      gas_id_t geran_gonly_gas_id;

      if ((default_gas_id == GERAN_ACCESS_STRATUM_ID_1) ||
          (default_gas_id == GERAN_ACCESS_STRATUM_ID_2))
      {
        geran_gonly_gas_id = OTHER_GAS_ID(default_gas_id);

        as_id_to_gas_id[SYS_MODEM_AS_ID_1] = default_gas_id;
        as_id_to_gas_id[SYS_MODEM_AS_ID_2] = geran_gonly_gas_id;

        gas_id_to_as_id[default_gas_id] = SYS_MODEM_AS_ID_1;
        gas_id_to_as_id[geran_gonly_gas_id] = SYS_MODEM_AS_ID_2;
      }
    }
#endif
}

/*===========================================================================

FUNCTION     geran_abandon_lock_exchange 

DESCRIPTION  Release the lock held by the inter-RAT exchange client

PARAMS       direction, Gas-id

DEPENDENCIES 

RETURN VALUE 

===========================================================================*/

void geran_abandon_lock_exchange(const geran_exchange_direction_t direction,const gas_id_t gas_id)
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
#ifndef PERLUTF
  if (geran_is_multi_mode_gas_id(gas_id))
  {
    /*explicitly release the inter-RAT client */
    grm_release(TRM_IRAT, gas_id);
    geran_clear_lock_exchanged(direction, gas_id);
  }
#endif /*!PERLUTF */
  MSG_GERAN_HIGH_1_G("Abandoned inter-RAT lock exchange in direction=%d", (int)direction);
#else
  NOTUSED(direction);
  NOTUSED(gas_id);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

 return;
}

/*!
 * \brief Module initialisation function. Called during RR task initialisation.
 */
void geran_multi_sim_init(void)
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  gas_id_t gas_id;

  // Assume a default config of Single SIM
  geran_sys_device_mode = SYS_MODEM_DEVICE_MODE_SINGLE_SIM;
  geran_multi_sim_mode = SYS_MODEM_DEVICE_MODE_SINGLE_SIM;
  geran_sys_standby_pref = SYS_MODEM_DS_PREF_SINGLE_STANDBY;
  geran_sys_multi_mode_sub = SYS_MODEM_AS_ID_1;
  geran_sys_subs_feature   = SYS_SUBS_FEATURE_MODE_NORMAL;

  // Set a default AS-ID<->GAS-ID mapping (0->0, 1->1, etc)
  determine_sub_id_mapping();

  for (gas_id = GERAN_ACCESS_STRATUM_ID_1; gas_id <= GERAN_ACCESS_STRATUM_ID_MAX; gas_id++)
  {
    /* initialise the lock exchange flags for this access stratum */
    geran_set_exchange_on_release(GERAN_EXCHANGE_NULL, gas_id);
    (void)geran_set_chain_exchanged(GERAN_EXCHANGE_XTOG, TRM_DENIAL, gas_id);
    (void)geran_set_chain_exchanged(GERAN_EXCHANGE_GTOX, TRM_DENIAL, gas_id);
#ifdef FEATURE_LTE
    geran_lte_activity_status[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)] = LTE_ACTIVITY_NONE;
#endif /* FEATURE_LTE */
  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  /* initialise GTA voting mechanism */
  rex_init_crit_sect(&gta_lock_crit_sec);
  memset(gta_veto_status, 0, sizeof(gta_veto_status));

  /* initialise tracking data for origination of GERAN traffic */
  if (GERAN_TRAFFIC_DEFAULT != geran_get_traffic_reason(NULL, GERAN_ACCESS_STRATUM_ID_1))
  {
    geran_set_traffic_reason(GERAN_TRAFFIC_DEFAULT, GERAN_ACCESS_STRATUM_ID_1);
  }

  return;
}

/*===========================================================================

FUNCTION     geran_device_in_svlte_dsda_mode

DESCRIPTION  Indicates whether device is configued in SVLTE+G DSDA mode

PARAMS       None

DEPENDENCIES Device mode supplied by MMCP

RETURN VALUE TRUE if device is configued in SVLTE+G DSDA mode

===========================================================================*/
boolean geran_device_in_svlte_dsda_mode(void)
{
  boolean                      svlte_dsda_config = FALSE;

#ifdef FEATURE_SVLTE_DUAL_SIM
  sys_modem_device_mode_e_type dev_mode = geran_get_multi_sim_mode();
  uint8                        simult_cap = 0;

  if (IS_DUAL_ACTIVE_MODE(dev_mode))
  {
    trm_get_info_input_type input;
    trm_get_info_return_type output;

    input.client_id = TRM_GSM1;
    input.info_type = TRM_SIMUL_CAPABILITY;

    trm_get_info(&input, &output);

    simult_cap = output.info.simul_cap_info;
        
    if ((simult_cap & TRM_SVLTE_IS_ENABLED) && 
        (simult_cap & TRM_DSDA_IS_ENABLED))
    {
      svlte_dsda_config = TRUE;
    }
  }

  MSG_GERAN_HIGH_3("MULTISIM=%d, simult_cap=%#02X, svlte_dsda_config=%d", dev_mode, simult_cap, svlte_dsda_config);
#endif /* FEATURE_SVLTE_DUAL_SIM */
  
  return svlte_dsda_config;
}

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
#ifdef FEATURE_LTE
/*===========================================================================

FUNCTION     geran_set_lte_activity_status

DESCRIPTION  Updates tracking flags for LTE stack activity

PARAMS       uint32 bitmask of activities (1 implies enabled, 0 otherwise)
             GAS ID

DEPENDENCIES None

RETURN VALUE VOID

===========================================================================*/
void geran_set_lte_activity_status(const uint32 activity_mask, const gas_id_t gas_id)
{
  geran_lte_activity_status[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)] = activity_mask;
  return;
}

/*===========================================================================

FUNCTION     geran_get_lte_activity_status

DESCRIPTION  Returns tracking flags for LTE stack activity

PARAMS       GAS ID

DEPENDENCIES None

RETURN VALUE uint32 bitmask of activities

===========================================================================*/
uint32 geran_get_lte_activity_status(const gas_id_t gas_id)
{
  return(geran_lte_activity_status[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)]);
}
#endif /* FEATURE_LTE */
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

/* EOF */


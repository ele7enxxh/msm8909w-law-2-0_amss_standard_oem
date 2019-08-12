/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            RR Idle DRX Module

GENERAL DESCRIPTION
   This module contains the state machine for Idle DRX change

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2010-2015 Qualcomm Technologies, Incorporated. All Rights Reserved.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_idle_drx.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
26/04/15    RRP      Initial Revision.

===========================================================================*/
#include "rr_idle_drx.h"
#ifdef FEATURE_IDLE_DRX_SCALING
#include "amssassert.h"
#include "gs.h"
#include "ms_timer.h"
#include "ms.h"
#include "rr_general.h"
#include "rr_sys_info.h"
#include "cause.h"
#include "rr_log.h"
#include "timers.h"
#include "rr_gprs_debug.h"
#include "rr_l1_send.h"
#include "rr_gprs_dsc.h"
#include "gprs_mem.h"
#include "rr_msgr.h"
#include "rr_grr.h"
#include "rr_plmn_list.h"
#include "rr_nv.h"
#include "rr_shared_cell_selection.h"
#include "rr_resel.h"

/*****************************************************************************
***
***     External Functions & Variables
***
*****************************************************************************/

/*****************************************************************************
***
***     Local Typedefs, #defines & Variables
***
*****************************************************************************/

#define DRX_SCALING_FACTOR_MIN   1
#define DRX_SCALING_FACTOR_MAX   8
#define EXTENDED_BS_PA_MFRMS_MAX 27

#define EXTENDED_DSC_MIN               10
#define RR_IDLE_DRX_DSC_THRESHOLD_LOW  20 /* percentage */
#define RR_IDLE_DRX_DSC_THRESHOLD_HIGH 50 /* percentage */

typedef enum
{
  RR_IDLE_DRX_STATE_NULL,
  RR_IDLE_DRX_STATE_NORMAL_DRX_NMEAS_ON,
  RR_IDLE_DRX_STATE_NORMAL_DRX_NMEAS_OFF,
  RR_IDLE_DRX_STATE_LONG_DRX_NMEAS_ON,
  RR_IDLE_DRX_STATE_LONG_DRX_NMEAS_OFF,
}rr_idle_drx_state_e;

typedef struct
{
  rr_idle_drx_state_e      state;
  rr_idle_drx_state_e      old_state;

  /* variables to keep DRX scaling information */
  boolean                  drx_scaling_factor_valid;
  uint8                    drx_scaling_factor;

  /* variable to skip idle mode measurement */
  boolean                  skip_idle_mode_meas;

  /* variables to keep extended DSC and extended BS_PA_MFRMS */
  byte                     extended_bs_pa_mfrms;
  byte                     extended_dsc;

  /* variable to track when DSC goes below threshold */
  boolean                  is_dsc_below_th;

  /* pending request */
  geran_grr_mtc_cfg_req_t  pend_req;
  boolean                  pend_req_valid;
} rr_idle_drx_data_t;

static rr_idle_drx_data_t rr_idle_drx_data[NUM_GERAN_DATA_SPACES];


/*****************************************************************************
***
***     Function Prototypes
***
*****************************************************************************/

/*****************************************************************************
***
***     Function Definitions
***
*****************************************************************************/

/****************************************************************************
 *   Function name:  rr_idle_drx_state_name
 *   --------------
 *
 *   Description: Returns a character string representing the RR_IDLE_DRX state
 *   ------------
 *
 *   Parameters: state - enum for IDLE_DRX SM state
 *   -----------
 *
 *   Returns:  character string representing the state
 *   --------
 ****************************************************************************/
static char* rr_idle_drx_state_name(const rr_idle_drx_state_e state)
{
  switch (state)
  {
    case RR_IDLE_DRX_STATE_NULL:                    return "NULL";
    case RR_IDLE_DRX_STATE_NORMAL_DRX_NMEAS_ON:     return "NORMAL_DRX_NMEAS_ON";
    case RR_IDLE_DRX_STATE_NORMAL_DRX_NMEAS_OFF:    return "NORMAL_DRX_NMEAS_OFF";
    case RR_IDLE_DRX_STATE_LONG_DRX_NMEAS_ON:       return "LONG_DRX_NMEAS_ON";
    case RR_IDLE_DRX_STATE_LONG_DRX_NMEAS_OFF:      return "LONG_DRX_NMEAS_OFF";

    default:
    {
      MSG_GERAN_ERROR_1("Unknown state: %d", state);
      return "?";
    }
  }
}

/****************************************************************************
 *   Function name:  rr_idle_drx_get_data_ptr
 *   --------------
 *
 *   Description: Returns a pointer to the module data
 *   ------------
 *
 *   Parameters: GAS ID
 *   -----------
 *
 *   Returns:  pointer to the module data
 *   --------
 ****************************************************************************/
static rr_idle_drx_data_t *rr_idle_drx_get_data_ptr(const gas_id_t gas_id)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_idle_drx_data[as_index];
}

/****************************************************************************
 *   Function name:  rr_idle_drx_scale_dsc
 *   --------------
 *
 *   Description: This function scales current DSC. This is mainly used when 
 *   ------------ there is a transition from normal DRX to extended DRX or 
 *                extended DRX to normal DRX.
 *
 *   Parameters: old_dsc_max - MAX DSC value for prev configuration
 *   ----------- new_dsc_max - MAX DSC value for current configuration
 *               GAS ID
 *
 *   Returns:  new scaled DSC.
 *   --------
 ****************************************************************************/
static int rr_idle_drx_scale_dsc(const byte old_dsc_max, const byte new_dsc_max, const gas_id_t gas_id)
{
  RR_ASSERT_FATAL(old_dsc_max != 0);

  return (rr_gprs_return_gprs_dsc_counter_value(gas_id) * new_dsc_max / old_dsc_max);
}

/****************************************************************************
 *   Function name:  rr_idle_drx_can_configure_device
 *   --------------
 *
 *   Description: Checks if device can be configured with new DRX value.
 *   ------------
 *
 *   Parameters: GAS ID
 *   ----------- 
 *
 *   Returns:  TRUE if device can be configured, FALSE otherwise
 *   --------
 ****************************************************************************/
static boolean rr_idle_drx_can_configure_device(const gas_id_t gas_id)
{
  /* When in IDLE state */
  if (grr_in_camped_state(gas_id) && (rr_macinfo_get_mac_status(gas_id) == MAC_IDLE))
  {
    return TRUE;
  }
  /* During background PLMN search */
  else if (rr_is_control_state(RR_PLMN_LIST_SEARCH, gas_id) && !rr_plc_search_in_foreground(gas_id))
  {
    return TRUE;
  }
  /* During RACHing */
  else if (rr_in_connection_pending(gas_id) || grr_tbf_est_in_progress(gas_id))
  {
    return TRUE;
  }
  /* In rest of the cases do not configure */
  else
  {
    MSG_GERAN_HIGH_0_G("Device cannot be configured with new DRX");
    return FALSE;
  }
}

/****************************************************************************
 *   Function name:  rr_idle_drx_validate_mtc_cfg_req
 *   --------------
 *
 *   Description: Validates received data in MCT_CFG_REQ message.
 *   ------------
 *
 *   Parameters: *req - pointer to the MTC_CFG_REQ message
 *   ----------- GAS ID
 *
 *   Returns:  TRUE if received data is valid, FALSE otherwise
 *   --------
 ****************************************************************************/
static boolean rr_idle_drx_validate_mtc_cfg_req(geran_grr_mtc_cfg_req_t * const req, const gas_id_t gas_id)
{
  RR_NULL_CHECK_FATAL(req);

  if (req->drx_scaling_factor_valid)
  {
    if ((req->drx_scaling_factor < DRX_SCALING_FACTOR_MIN) || (req->drx_scaling_factor > DRX_SCALING_FACTOR_MAX))
    {
      MSG_GERAN_ERROR_1_G("Invalid DRX scaling factor %d", req->drx_scaling_factor);
      return FALSE;
    }
  }

  return TRUE;
}

/****************************************************************************
 *   Function name:  rr_idle_drx_save_mtc_cfg_req
 *   --------------
 *
 *   Description: Saves MTC_CFG_REQ for future processing
 *   ------------
 *
 *   Parameters: *req - pointer to the MTC_CFG_REQ message
 *   ----------- GAS ID
 *
 *   Returns:  NONE
 *   --------
 ****************************************************************************/
static void rr_idle_drx_save_mtc_cfg_req(geran_grr_mtc_cfg_req_t * const req, const gas_id_t gas_id)
{
  rr_idle_drx_data_t *rr_idle_drx_data_ptr = rr_idle_drx_get_data_ptr(gas_id);

  RR_NULL_CHECK_FATAL(rr_idle_drx_data_ptr);
  RR_NULL_CHECK_FATAL(req);

  MSG_GERAN_HIGH_0_G("GERAN_GRR_MTC_CFG_REQ saved for later use");
  rr_idle_drx_data_ptr->pend_req = *req;
  rr_idle_drx_data_ptr->pend_req_valid = TRUE;

  return;
}

/****************************************************************************
 *   Function name:  rr_idle_drx_compute_extended_bs_pa_mfrms_and_dsc
 *   --------------
 *
 *   Description: Computes extended BS_PA_MFRMS and scaled DSC
 *   ------------
 *
 *   Parameters: scaling_factor - Scaling factor
 *   ----------- bs_pa_mfrms - OTA received parameter
 *               *extended_bs_pa_mfrms - extended DRX (OUT Parameter)
 *               *extended_dsc - scaled DSC (OUT Parameter)
 *               GAS ID
 *
 *   Returns:  NONE
 *   --------
 ****************************************************************************/
static void rr_idle_drx_compute_extended_bs_pa_mfrms_and_dsc(
  const uint8    scaling_factor,
  const byte     bs_pa_mfrms,
  byte * const   extended_bs_pa_mfrms,
  byte * const   extended_dsc,
  const gas_id_t gas_id
)
{
  MSG_GERAN_HIGH_2_G("Scaling factor = %d, BS_PA_MFRMS = %d", scaling_factor, bs_pa_mfrms);
  
  RR_ASSERT_FATAL((bs_pa_mfrms >= BS_PA_MFRMS_MIN) && (bs_pa_mfrms <= BS_PA_MFRMS_MAX));
  RR_ASSERT_FATAL((scaling_factor >= DRX_SCALING_FACTOR_MIN) && (scaling_factor <= DRX_SCALING_FACTOR_MAX));

  /***************************************************************************
   * Paging multi-frame value during extended DRX is computed as below.
   * In the below table, first row represents the original BS_PA_MFRMS,
   * first column represents the scaling factor, and rest are computed
   * extended BS_PA_MFRMS.
   * extended BS_PA_MFRMS has been capped to 27.
   * 
   *  --------------------------------------------
   * |      2    3    4    5    6    7    8    9  |
   * |    ----------------------------------------|
   * | 1 |  2    3    4    5    6    7    8    9  |
   * | 2 |  4    6    8   10   12   14   16   18  |
   * | 3 |  6    9   12   15   18   21   24   27  |
   * | 4 |  8   12   16   20   24   21   24   27  |
   * | 5 | 10   15   20   25   24   21   24   27  |
   * | 6 | 12   18   24   25   24   21   24   27  |
   * | 7 | 14   21   24   25   24   21   24   27  |
   * | 8 | 16   24   24   25   24   21   24   27  |
   *  --------------------------------------------
   ***************************************************************************/
  if (extended_bs_pa_mfrms)
  {
    *extended_bs_pa_mfrms = MIN(scaling_factor, (EXTENDED_BS_PA_MFRMS_MAX / bs_pa_mfrms)) * bs_pa_mfrms;
    MSG_GERAN_HIGH_1_G("scaled BS_PA_MFRMS = %d", *extended_bs_pa_mfrms);
  }

  /***************************************************************************
   * DSC value during extended DRX is computed as below.
   * In the below table, first row represents the original BS_PA_MFRMS,
   * first column represents the scaling factor, and rest are computed
   * extended DSC.
   * DSC during extended DRX has been floored to 10.
   * 
   *  --------------------------------------------
   * |      2    3    4    5    6    7    8    9  |
   * |    ----------------------------------------|
   * | 1 | 45   30   22   18   15   12   11   10  |
   * | 2 | 22   15   11   10   10   10   10   10  |
   * | 3 | 15   10   10   10   10   10   10   10  |
   * | 4 | 11   10   10   10   10   10   10   10  |
   * | 5 | 10   10   10   10   10   10   10   10  |
   * | 6 | 10   10   10   10   10   10   10   10  |
   * | 7 | 10   10   10   10   10   10   10   10  |
   * | 8 | 10   10   10   10   10   10   10   10  |
   *  --------------------------------------------
   ***************************************************************************/
  if (extended_dsc)
  {
    *extended_dsc = MAX((GPRS_DSC_DIVIDEND / (scaling_factor * bs_pa_mfrms)), EXTENDED_DSC_MIN);
    MSG_GERAN_HIGH_1_G("scaled DSC = %d", *extended_dsc);
  }
  
  return;
}

/****************************************************************************
 *   Function name:  rr_idle_drx_process_config
 *   --------------
 *
 *   Description: This processes scaling information and stores extended 
 *   ------------ BS_PA_MFRMS and scaled DSC in the module context.
 *
 *   Parameters: GAS ID
 *   ----------- 
 *
 *   Returns:  NONE
 *   --------
 ****************************************************************************/
static void rr_idle_drx_process_config(const gas_id_t gas_id)
{
  rr_idle_drx_data_t *rr_idle_drx_data_ptr = rr_idle_drx_get_data_ptr(gas_id);

  RR_NULL_CHECK_FATAL(rr_idle_drx_data_ptr);

  if (rr_idle_drx_data_ptr->drx_scaling_factor_valid)
  {
    gprs_scell_info_t *scell = rr_gprs_get_scell_info(gas_id);
    byte extended_bs_pa_mfrms;
    byte extended_dsc;

    RR_NULL_CHECK_FATAL(scell);

    rr_idle_drx_compute_extended_bs_pa_mfrms_and_dsc(
      rr_idle_drx_data_ptr->drx_scaling_factor,
      scell->gsm.control_channel_description.BS_PA_MFRMS,
      &extended_bs_pa_mfrms,
      &extended_dsc,
      gas_id
    );
    
    rr_idle_drx_data_ptr->extended_bs_pa_mfrms = extended_bs_pa_mfrms;
    rr_idle_drx_data_ptr->extended_dsc = extended_dsc;
  }
  else
  {
    rr_idle_drx_data_ptr->extended_bs_pa_mfrms = 0;
    rr_idle_drx_data_ptr->extended_dsc = 0;
  }
  
  return;
}

/****************************************************************************
 *   Function name:  rr_idle_drx_get_new_state
 *   --------------
 *
 *   Description: Based on different conditions and scaling information this 
 *   ------------ function returns the new state of IDLE_DRX SM.
 *
 *   Parameters: GAS ID
 *   ----------- 
 *
 *   Returns:  New state for IDLE_DRX SM
 *   --------
 ****************************************************************************/
static rr_idle_drx_state_e rr_idle_drx_get_new_state(const gas_id_t gas_id)
{
  rr_idle_drx_data_t *rr_idle_drx_data_ptr = rr_idle_drx_get_data_ptr(gas_id);
  gprs_scell_info_t  *scell = rr_gprs_get_scell_info(gas_id);
  rr_idle_drx_state_e new_state;
  uint8               nc_mode;
  
  RR_NULL_CHECK_FATAL(rr_idle_drx_data_ptr);
  RR_NULL_CHECK_FATAL(scell);

  nc_mode = rr_gprs_get_nc_mode(gas_id);

  if (nc_mode == NC_MEAS_NC1 || nc_mode == NC_MEAS_NC2)
  {
    MSG_GERAN_HIGH_1_G("NORMAL_DRX_NMEAS_ON - nc_mode=%d", nc_mode);
    new_state = RR_IDLE_DRX_STATE_NORMAL_DRX_NMEAS_ON;
  }
  else if (rr_timer_is_running(RR_IDLE_DRX_SCALING_SUSPEND_TIMER, gas_id))
  {
    MSG_GERAN_HIGH_0_G("NORMAL_DRX_NMEAS_ON - IDLE_DRX_SCALING_SUSPEND timer running");
    new_state = RR_IDLE_DRX_STATE_NORMAL_DRX_NMEAS_ON;
  }
  else if (rr_idle_drx_data_ptr->is_dsc_below_th)
  {
    /* When DSC is below threshold we will be in legacy DRX with nMeas ON state.
     * If new config says to disable nMeas then it is not possible as DSC is below threshold.
     * If new config says to enable nMeas then we are already in nMeas ON state.
     * If new config says to scale DRX then it is not possible as DSC is below threshold.
     * If new config says to move to legacy DRX then we are already in legacy DRX.
     * So, there is no point in configuring the device when DSC is below threshold. */
    MSG_GERAN_HIGH_0_G("NORMAL_DRX_NMEAS_ON - DSC below threshold");
    new_state = RR_IDLE_DRX_STATE_NORMAL_DRX_NMEAS_ON;
  }
  else if (rr_idle_drx_data_ptr->drx_scaling_factor_valid)
  {
    if (rr_idle_drx_data_ptr->skip_idle_mode_meas)
    {
      new_state = RR_IDLE_DRX_STATE_LONG_DRX_NMEAS_OFF;
    }
    else
    {
      new_state = RR_IDLE_DRX_STATE_LONG_DRX_NMEAS_ON;
    }
  }
  else
  {
    if (rr_idle_drx_data_ptr->skip_idle_mode_meas)
    {
      new_state = RR_IDLE_DRX_STATE_NORMAL_DRX_NMEAS_OFF;
    }
    else
    {
      new_state = RR_IDLE_DRX_STATE_NORMAL_DRX_NMEAS_ON;
    }
  }

  return new_state;
}

/****************************************************************************
 *   Function name:  rr_idle_drx_check_config_change
 *   --------------
 *
 *   Description: This function determinese if there is any change in scaled DRX 
 *   ------------ and skip IDLE mode ncell meas configuration. 
 *                Based on return values (drx_changed, meas_changed, stop_meas)
 *                main control function decides whether to start/stop measurements
 *                and whether to inform GL1 about new configuration.
 *
 *   Parameters: *drx_changed - TRUE if change in DRX state (OUT Parameter)
 *   ----------- *meas_changed - TRUE if change in ncell meas status (OUT Parameter)
 *               *stop_meas - TURE if ncell meas need to be stopped (OUT Parameter)
 *               old_state - old configuration state (IN Parameter)
 *               new_state - new configuration state (IN Parameter)
 *               GAS ID
 *
 *   Returns:  NONE
 *   --------
 ****************************************************************************/
static void rr_idle_drx_check_config_change(
  boolean * const           drx_changed, 
  boolean * const           meas_changed, 
  boolean * const           stop_meas, 
  const rr_idle_drx_state_e old_state,
  const rr_idle_drx_state_e new_state,
  const gas_id_t            gas_id
)
{
  RR_NULL_CHECK_FATAL(drx_changed);
  RR_NULL_CHECK_FATAL(meas_changed);
  RR_NULL_CHECK_FATAL(stop_meas);

  *drx_changed  = FALSE;
  *meas_changed = FALSE;
  *stop_meas    = FALSE;
  
  switch (new_state)
  {
    case RR_IDLE_DRX_STATE_NULL:
    {
      /* do nothing */
      break;
    }
    
    case RR_IDLE_DRX_STATE_NORMAL_DRX_NMEAS_ON:
    {
      if (old_state == RR_IDLE_DRX_STATE_NULL)
      {
        *drx_changed = TRUE;
      }
      else if (old_state == RR_IDLE_DRX_STATE_NORMAL_DRX_NMEAS_OFF)
      {
        *meas_changed = TRUE;
      }
      else if (old_state == RR_IDLE_DRX_STATE_LONG_DRX_NMEAS_ON)
      {
        *drx_changed = TRUE;
      }
      else if (old_state == RR_IDLE_DRX_STATE_LONG_DRX_NMEAS_OFF)
      {
        *drx_changed = TRUE;
        *meas_changed = TRUE;
      }
      
      break;
    }
    
    case RR_IDLE_DRX_STATE_NORMAL_DRX_NMEAS_OFF:
    {
      *stop_meas = TRUE;
      
      if (old_state == RR_IDLE_DRX_STATE_NULL)
      {
        *meas_changed = TRUE;
      }
      else if (old_state == RR_IDLE_DRX_STATE_NORMAL_DRX_NMEAS_ON)
      {
        *meas_changed = TRUE;
      }
      else if (old_state == RR_IDLE_DRX_STATE_LONG_DRX_NMEAS_ON)
      {
        *drx_changed = TRUE;
        *meas_changed = TRUE;
      }
      else if (old_state == RR_IDLE_DRX_STATE_LONG_DRX_NMEAS_OFF)
      {
        *drx_changed = TRUE;
      }
      
      break;
    }
    
    case RR_IDLE_DRX_STATE_LONG_DRX_NMEAS_ON:
    {
      if (old_state == RR_IDLE_DRX_STATE_NULL)
      {
        *drx_changed = TRUE;
      }
      else if (old_state == RR_IDLE_DRX_STATE_NORMAL_DRX_NMEAS_ON)
      {
        *drx_changed = TRUE;
      }
      else if (old_state == RR_IDLE_DRX_STATE_NORMAL_DRX_NMEAS_OFF)
      {
        *drx_changed = TRUE;
        *meas_changed = TRUE;
      }
      else if (old_state == RR_IDLE_DRX_STATE_LONG_DRX_NMEAS_OFF)
      {
        *meas_changed = TRUE;
      }
      
      break;
    }
    
    case RR_IDLE_DRX_STATE_LONG_DRX_NMEAS_OFF:
    {
      *stop_meas = TRUE;
      
      if (old_state == RR_IDLE_DRX_STATE_NULL)
      {
        *drx_changed = TRUE;
        *meas_changed = TRUE;
      }
      else if (old_state == RR_IDLE_DRX_STATE_NORMAL_DRX_NMEAS_ON)
      {
        *drx_changed = TRUE;
        *meas_changed = TRUE;
      }
      else if (old_state == RR_IDLE_DRX_STATE_NORMAL_DRX_NMEAS_OFF)
      {
        *drx_changed = TRUE;
      }
      else if (old_state == RR_IDLE_DRX_STATE_LONG_DRX_NMEAS_ON)
      {
        *meas_changed = TRUE;
      }
      
      break;
    }
  }

  return;
}

/****************************************************************************
 *   Function name:  rr_idle_drx_apply_config
 *   --------------
 *
 *   Description: Based on the configuration we move to a new state and  
 *   ------------ scales the current DSC.
 *
 *   Parameters: *notify_L1 - TRUE if GL1 needs to be notified (OUT Parameter)
 *   ----------- *stop_meas - TURE if ncell meas need to be stopped (OUT Parameter)
 *               sf_changed - TURE if scaling factor is changed (IN Parameter)
 *               old_extended_dsc - DSC MAX for old long DRX configuration (IN Parameter)
 *               GAS ID
 *
 *   Returns:  TRUE if new configuration is applied, FALSE otherwise
 *   --------
 ****************************************************************************/
static boolean rr_idle_drx_apply_config(
  boolean * const notify_L1, 
  boolean * const stop_meas, 
  const boolean   sf_changed,
  const byte      old_extended_dsc,
  const gas_id_t  gas_id
)
{
  rr_idle_drx_data_t *rr_idle_drx_data_ptr = rr_idle_drx_get_data_ptr(gas_id);
  rr_idle_drx_state_e new_state;
  rr_idle_drx_state_e old_state;
  boolean drx_cfg_changed;
  boolean meas_cfg_changed;
  int new_dsc;

  RR_NULL_CHECK_FATAL(rr_idle_drx_data_ptr);
  RR_NULL_CHECK_FATAL(notify_L1);

  old_state = rr_idle_drx_data_ptr->state;
  new_state = rr_idle_drx_get_new_state(gas_id);
  rr_idle_drx_check_config_change(&drx_cfg_changed, &meas_cfg_changed, stop_meas, old_state, new_state, gas_id);

  /* Calculate new scaled DSC based on old and new states */
  if (drx_cfg_changed)
  {
    byte idle_dsc_max = (byte) rr_gprs_get_max_dsc_value(FALSE, gas_id);

    if ((new_state == RR_IDLE_DRX_STATE_NORMAL_DRX_NMEAS_ON) ||
        (new_state == RR_IDLE_DRX_STATE_NORMAL_DRX_NMEAS_OFF))
    {
      new_dsc = rr_idle_drx_scale_dsc(old_extended_dsc, idle_dsc_max, gas_id);
    }
    else
    {
      new_dsc = rr_idle_drx_scale_dsc(idle_dsc_max, rr_idle_drx_data_ptr->extended_dsc, gas_id);

      /* Legacy DRX to Long DRX is allowed only if new scaled DSC is above minimum value */
      if ((old_state == RR_IDLE_DRX_STATE_NORMAL_DRX_NMEAS_ON) &&
          (new_dsc < EXTENDED_DSC_MIN))
      {
        MSG_GERAN_HIGH_1_G("Cannot move to new state, DSC(%d) below MIN(10)", new_dsc);
        *notify_L1 = FALSE;
        return FALSE;
      }
    }
  }
  else if (sf_changed)
  {
    new_dsc = rr_idle_drx_scale_dsc(old_extended_dsc, rr_idle_drx_data_ptr->extended_dsc, gas_id);
  }

  if (drx_cfg_changed || sf_changed)
  {
    rr_gprs_set_gprs_dsc_counter_value(new_dsc, gas_id);
  }
  
  *notify_L1 = drx_cfg_changed || sf_changed || meas_cfg_changed;
  rr_idle_drx_data_ptr->state = new_state;
  
  return TRUE;
}

/****************************************************************************
 *   Function name:  rr_idle_drx_process_mtc_cfg_req
 *   --------------
 *
 *   Description: Processes MTC_CFG_REQ received from QMI-NAS
 *   ------------
 *
 *   Parameters: *req - pointer to the MTC_CFG_REQ message
 *   ----------- *notify_L1 - TRUE if GL1 needs to be notified (OUT Parameter)
 *               *stop_meas - TURE if ncell meas need to be stopped (OUT Parameter)
 *               GAS ID
 *
 *   Returns:  NONE
 *   --------
 ****************************************************************************/
static void rr_idle_drx_process_mtc_cfg_req(
  geran_grr_mtc_cfg_req_t * const req,
  boolean * const                 notify_L1, 
  boolean * const                 stop_meas, 
  const gas_id_t                  gas_id
)
{
  rr_idle_drx_data_t *rr_idle_drx_data_ptr = rr_idle_drx_get_data_ptr(gas_id);
  byte old_extended_dsc;
  boolean sf_changed = FALSE;

  RR_NULL_CHECK_FATAL(rr_idle_drx_data_ptr);
  RR_NULL_CHECK_FATAL(req);

  MSG_GERAN_HIGH_5_G("MTC_CFG_REQ:: state=%d, sf_valid=%d, sf=%d, skip_meas_valid=%d, skip_meas=%d", 
    rr_idle_drx_data_ptr->state,
    req->drx_scaling_factor_valid,
    req->drx_scaling_factor,
    req->skip_idle_mode_meas_valid,
    req->skip_idle_mode_meas
  );

  /* Save the config parameters in the context data */
  if (req->drx_scaling_factor_valid && (req->drx_scaling_factor != 1))
  {
    if ((rr_idle_drx_data_ptr->drx_scaling_factor_valid) && 
        (rr_idle_drx_data_ptr->drx_scaling_factor != req->drx_scaling_factor))
    {
      sf_changed = TRUE;
    }
    
    rr_idle_drx_data_ptr->drx_scaling_factor_valid = TRUE;
    rr_idle_drx_data_ptr->drx_scaling_factor = req->drx_scaling_factor;
  }
  else
  {
    rr_idle_drx_data_ptr->drx_scaling_factor_valid = FALSE;
    rr_idle_drx_data_ptr->drx_scaling_factor = 0;
  }
  
  if (req->skip_idle_mode_meas_valid)
  {
    rr_idle_drx_data_ptr->skip_idle_mode_meas = req->skip_idle_mode_meas;
  }
  else
  {
    rr_idle_drx_data_ptr->skip_idle_mode_meas = FALSE;
  }

  /* Save old extended DSC value to a local variable */
  old_extended_dsc = rr_idle_drx_data_ptr->extended_dsc;

  /* Process the received config to compute DRX and DSC */
  rr_idle_drx_process_config(gas_id);

  /* Now apply the config and move to new state */
  (void) rr_idle_drx_apply_config(notify_L1, stop_meas, sf_changed, old_extended_dsc, gas_id);

  return;
}

/****************************************************************************
 *   Function name:  rr_idle_drx_send_mtc_cfg_rsp
 *   --------------
 *
 *   Description: Sends MTC_CFG_RSP to NAS.
 *   ------------
 *
 *   Parameters: status - response status E_SUCCESS / E_FAILURE
 *   ----------- GAS ID
 *
 *   Returns:  NONE
 *   --------
 ****************************************************************************/
static void rr_idle_drx_send_mtc_cfg_rsp(const errno_enum_type status, const gas_id_t gas_id)
{
  geran_grr_mtc_cfg_rsp_t mtc_cfg_rsp;
  errno_enum_type         result;
  
  MSG_GERAN_HIGH_1_G("Sending GERAN_GRR_MTC_CFG_RSP (status=%d)", status);

  memset(&mtc_cfg_rsp, 0, sizeof(mtc_cfg_rsp));
  msgr_init_hdr(
    &mtc_cfg_rsp.msg_hdr,
    MSGR_GERAN_GRR,
    GERAN_GRR_MTC_CFG_RSP
  );

  mtc_cfg_rsp.status = status;

  result = rr_msgr_send(&mtc_cfg_rsp.msg_hdr, sizeof(mtc_cfg_rsp), gas_id);
  if (result != E_SUCCESS)
  {
    MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
  }

  return;
}

/****************************************************************************
 *   Function name:  rr_idle_drx_deinit
 *   --------------
 *
 *   Description: De-initialize function
 *   ------------ 
 *
 *   Parameters: GAS ID
 *   ----------- 
 *
 *   Returns:  NONE
 *   --------
 ****************************************************************************/
static void rr_idle_drx_deinit(const gas_id_t gas_id)
{
  rr_idle_drx_data_t *rr_idle_drx_data_ptr = rr_idle_drx_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_idle_drx_data_ptr);
  
  rr_timer_cancel(RR_IDLE_DRX_SCALING_SUSPEND_TIMER, gas_id);

  rr_idle_drx_data_ptr->state = RR_IDLE_DRX_STATE_NULL;
  rr_idle_drx_data_ptr->extended_bs_pa_mfrms = 0;
  rr_idle_drx_data_ptr->extended_dsc = 0;
  rr_idle_drx_data_ptr->is_dsc_below_th = FALSE;
  rr_idle_drx_data_ptr->pend_req_valid = FALSE;
  
  return;
}

/****************************************************************************
 *   Function name:  rr_idle_drx_init
 *   --------------
 *
 *   Description: Initialize function
 *   ------------ 
 *
 *   Parameters: GAS ID
 *   ----------- 
 *
 *   Returns:  NONE
 *   --------
 ****************************************************************************/
void rr_idle_drx_init(const gas_id_t gas_id)
{
  rr_idle_drx_data_t *rr_idle_drx_data_ptr = rr_idle_drx_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_idle_drx_data_ptr);
  memset(rr_idle_drx_data_ptr, 0, sizeof(rr_idle_drx_data_t));
  return;
}

/****************************************************************************
 *   Function name:  rr_idle_drx_start_scaling_suspend_timer
 *   --------------
 *
 *   Description: Starts RR_IDLE_DRX_SCALING_SUSPEND_TIMER.
 *   ------------
 *
 *   Parameters: GAS ID
 *   ----------- 
 *
 *   Returns:  TRUE if timer is started, FALSE otherwise
 *   --------
 ****************************************************************************/
boolean rr_idle_drx_start_scaling_suspend_timer(const gas_id_t gas_id)
{
  uint32 timeout_val = rr_nv_get_idle_drx_scaling_suspend_time_millisecs(RR_GAS_ID_TO_AS_ID);
  
  MSG_GERAN_MED_1_G("IDLE_DRX_SCALING_SUSPEND timeout value=%d", timeout_val);

  if (timeout_val > 0)
  {
    return rr_timer_start(RR_IDLE_DRX_SCALING_SUSPEND_TIMER, timeout_val, gas_id);
  }

  return FALSE;
}

/****************************************************************************
 *   Function name:  rr_idle_drx_get_extended_dsc
 *   --------------
 *
 *   Description: Returns MAX DCS value for extended DRX.
 *   ------------ Returns zero if we are not in extended DRX state.
 *
 *   Parameters: GAS ID
 *   ----------- 
 *
 *   Returns:  MAX DCS value for extended DRX
 *   --------
 ****************************************************************************/
byte rr_idle_drx_get_extended_dsc(const gas_id_t gas_id)
{
  rr_idle_drx_data_t *rr_idle_drx_data_ptr = rr_idle_drx_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_idle_drx_data_ptr);

  MSG_GERAN_HIGH_2_G("rr_idle_drx state=%d, extended_dsc=%d", rr_idle_drx_data_ptr->state, rr_idle_drx_data_ptr->extended_dsc);

  if ((rr_idle_drx_data_ptr->state == RR_IDLE_DRX_STATE_LONG_DRX_NMEAS_ON) ||
      (rr_idle_drx_data_ptr->state == RR_IDLE_DRX_STATE_LONG_DRX_NMEAS_OFF))
  {
    return rr_idle_drx_data_ptr->extended_dsc;
  }

  return 0;
}

/****************************************************************************
 *   Function name:  rr_idle_drx_scaling_in_use
 *   --------------
 *
 *   Description: Checks if scaled DRX is in use or not
 *   ------------
 *
 *   Parameters: GAS ID
 *   ----------- 
 *
 *   Returns:  TRUE if scaled DRX is in use, FALSE otherwise
 *   --------
 ****************************************************************************/
boolean rr_idle_drx_scaling_in_use(const gas_id_t gas_id)
{
  rr_idle_drx_data_t *rr_idle_drx_data_ptr = rr_idle_drx_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_idle_drx_data_ptr);

  if ((rr_idle_drx_data_ptr->state == RR_IDLE_DRX_STATE_LONG_DRX_NMEAS_ON) ||
      (rr_idle_drx_data_ptr->state == RR_IDLE_DRX_STATE_LONG_DRX_NMEAS_OFF))
  {
    return TRUE;
  }

  return FALSE;
}

/****************************************************************************
 *   Function name:  rr_idle_drx_is_nmeas_off
 *   --------------
 *
 *   Description: Checks if ncell meas is OFF
 *   ------------
 *
 *   Parameters: GAS ID
 *   ----------- 
 *
 *   Returns:  TRUE if nmeas is off, FALSE otherwise
 *   --------
 ****************************************************************************/
boolean rr_idle_drx_is_nmeas_off(const gas_id_t gas_id)
{
  rr_idle_drx_data_t *rr_idle_drx_data_ptr = rr_idle_drx_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_idle_drx_data_ptr);

  if ((rr_idle_drx_data_ptr->state == RR_IDLE_DRX_STATE_NORMAL_DRX_NMEAS_OFF) ||
      (rr_idle_drx_data_ptr->state == RR_IDLE_DRX_STATE_LONG_DRX_NMEAS_OFF))
  {
    return TRUE;
  }

  return FALSE;
}

/****************************************************************************
 *   Function name:  rr_idle_drx_check_dsc
 *   --------------
 *
 *   Description: Checks if DSC is below threshold or above threshold.
 *   ------------
 *
 *   Parameters: dsc_max - MAX DSC value for the current configuration
 *   ----------- dsc - Current DSC value
 *               GAS ID
 *
 *   Returns:  NONE
 *   --------
 ****************************************************************************/
void rr_idle_drx_check_dsc(const int dsc_max, const int dsc, const gas_id_t gas_id)
{
  rr_idle_drx_data_t *rr_idle_drx_data_ptr = rr_idle_drx_get_data_ptr(gas_id);
  int percent_dsc;

  RR_NULL_CHECK_FATAL(rr_idle_drx_data_ptr);
  RR_ASSERT_FATAL(dsc_max != 0);

  /* calculate the percentage of maximum DSC */
  percent_dsc = (dsc * 100) / dsc_max;

  MSG_GERAN_HIGH_3_G("Check DSC:: max=%d, dsc=%d, percent_dsc=%d", dsc_max, dsc, percent_dsc);

  /* If DSC is below MIN then generate BELOW_TH_LOW event */
  if ((dsc < EXTENDED_DSC_MIN) || (percent_dsc <= RR_IDLE_DRX_DSC_THRESHOLD_LOW))
  {
    rr_idle_drx_control(EV_IDLE_DRX_DSC_BELOW_TH_LOW, NULL, gas_id);
  }
  /* If DSC goes above threshold (50% of the MAX DSC) then generate ABOVE_TH_HIGH event */
  else if((rr_idle_drx_data_ptr->is_dsc_below_th) && 
          (dsc >= EXTENDED_DSC_MIN) && 
          (percent_dsc >= RR_IDLE_DRX_DSC_THRESHOLD_HIGH))
  {
    rr_idle_drx_control(EV_IDLE_DRX_DSC_ABOVE_TH_HIGH, NULL, gas_id);
  }

  return;
}

/****************************************************************************
 *   Function name:  rr_idle_drx_bs_pa_mfrms_changed
 *   --------------
 *
 *   Description: Re-computes DRX and DSC on change in BS_PA_MFRMS.
 *   ------------
 *
 *   Parameters: old_dsc_max - MAX DSC value based on old BS_PA_MFRMS
 *   ----------- GAS ID
 *
 *   Returns:  NONE
 *   --------
 ****************************************************************************/
void rr_idle_drx_bs_pa_mfrms_changed(const int old_dsc_max, const gas_id_t gas_id)
{
  rr_idle_drx_data_t *rr_idle_drx_data_ptr = rr_idle_drx_get_data_ptr(gas_id);
  byte new_dsc_max;
  int  new_dsc;

  RR_NULL_CHECK_FATAL(rr_idle_drx_data_ptr);

  /* Re-compute scaled DRX and scaled DSC */
  rr_idle_drx_process_config(gas_id);

  /* Get new MAX DSC value */
  new_dsc_max = (byte) rr_gprs_get_max_dsc_value(TRUE, gas_id);

  if ((rr_idle_drx_data_ptr->state == RR_IDLE_DRX_STATE_LONG_DRX_NMEAS_ON) ||
      (rr_idle_drx_data_ptr->state == RR_IDLE_DRX_STATE_LONG_DRX_NMEAS_OFF))
  {
    /* For scaled DRX, scale the current DSC */
    new_dsc = rr_idle_drx_scale_dsc((byte)old_dsc_max, new_dsc_max, gas_id);

    /* Set the current DSC to the new value */
    rr_gprs_set_gprs_dsc_counter_value(new_dsc, gas_id);
  }
  else
  {
    /* For legacy DRX, initialise the DSC */
    rr_gprs_initialise_dsc(gas_id);
  }

  /* Notify the SM so that it can send an indication to GL1 */
  rr_idle_drx_control(EV_IDLE_DRX_BS_PA_MFRMS_CHANGED, NULL, gas_id);

  return;
}

/****************************************************************************
 *   Function name:  rr_idle_drx_control
 *   --------------
 *
 *   Description: This is the control of the RR IDLE DRX State Machine 
 *   ------------ 
 *
 *   Parameters: drx_event - An event to the state machine
 *   ----------- *msg_ptr - Pointer to an input message (valid only if event is EV_INPUT_MESSAGE)
 *               GAS ID
 *
 *   Returns:  NONE
 *   --------
 ****************************************************************************/
void rr_idle_drx_control(rr_event_T drx_event, rr_cmd_bdy_type* msg_ptr, const gas_id_t gas_id)
{
  byte       message_id;
  byte       message_set;
  boolean    notify_L1 = FALSE;
  boolean    stop_meas = FALSE;
  
  rr_idle_drx_data_t *rr_idle_drx_data_ptr = rr_idle_drx_get_data_ptr(gas_id);

  RR_NULL_CHECK_FATAL(rr_idle_drx_data_ptr);

#ifdef PERLUTF
  #error code not present
#else
  MSG_GERAN_HIGH_2_G("rr_idle_drx_control:: drx_event=0x%x, state=%d", drx_event, rr_idle_drx_data_ptr->state);
#endif

  drx_event = rr_extract_message_header_details(drx_event, msg_ptr, &message_set, &message_id);

  switch (drx_event)
  {
    case EV_INPUT_MESSAGE:
    {
      if ((message_set == MS_MSGR_RR) && (msg_ptr->rr_msgr_msg.message.hdr.id == GERAN_GRR_MTC_CFG_REQ))
      {
        geran_grr_mtc_cfg_req_t *req_ptr = &msg_ptr->rr_msgr_msg.message.geran_grr_msg_req.mtc_cfg_req;

        /* check if received parameters are valid */
        if (rr_idle_drx_validate_mtc_cfg_req(req_ptr, gas_id))
        {
          /* Check if we can configure the device with new parameters. */
          if (rr_idle_drx_can_configure_device(gas_id))
          {
            rr_idle_drx_process_mtc_cfg_req(req_ptr, &notify_L1, &stop_meas, gas_id);
          }
          else
          {
            rr_idle_drx_save_mtc_cfg_req(req_ptr, gas_id);
          }
          
          rr_idle_drx_send_mtc_cfg_rsp(E_SUCCESS, gas_id);
        }
        else
        {
          rr_idle_drx_send_mtc_cfg_rsp(E_FAILURE, gas_id);
        }
      } /* GERAN_GRR_MTC_CFG_REQ */
      else
      {
        LOG_UNEXPECTED_MESSAGE(msg_ptr, rr_idle_drx_state_name(rr_idle_drx_data_ptr->state));
      }

      break;
    } /* EV_INPUT_MESSAGE */

    case RR_EV_CAMPED_ON_CELL:
    {
      /* Camped on a cell. Reset the curr state to NORMAL_DRX_NMEAS_ON */
      rr_idle_drx_data_ptr->is_dsc_below_th = FALSE;
      rr_idle_drx_data_ptr->state = RR_IDLE_DRX_STATE_NORMAL_DRX_NMEAS_ON;

      /* Initialise the DSC */
      rr_gprs_initialise_dsc(gas_id);

      if (rr_idle_drx_data_ptr->pend_req_valid)
      {
        MSG_GERAN_HIGH_0_G("Processing saved GERAN_GRR_MTC_CFG_REQ");
        rr_idle_drx_process_mtc_cfg_req(&rr_idle_drx_data_ptr->pend_req, &notify_L1, &stop_meas, gas_id);
        rr_idle_drx_data_ptr->pend_req_valid = FALSE;
      }
      else
      {
        /* Process the received config to compute DRX and DSC based on new serving cell param */
        rr_idle_drx_process_config(gas_id);
        /* Now apply the config and move to new state */
        (void) rr_idle_drx_apply_config(&notify_L1, &stop_meas, FALSE, 0, gas_id);
      }

      /* Always send MPH_UPDATE_IDLE_DRX_IND to GL1 after camped on a cell */
      notify_L1 = TRUE;
      
      break;
    } /* RR_EV_CAMPED_ON_CELL */

    case EV_IDLE_DRX_BS_PA_MFRMS_CHANGED:
    {
      MSG_GERAN_HIGH_0_G("BS_PA_MFRMS changed");
      if (rr_idle_drx_can_configure_device(gas_id))
      {
        notify_L1 = TRUE;
      }
      
      break;
    } /* EV_IDLE_DRX_BS_PA_MFRMS_CHANGED */

    case EV_MAC_IDLE_MODE:
    {
      /* Initialise the DSC on leaving transfer - 45.008, section 6.5 */
      rr_gprs_initialise_dsc(gas_id);
      /* no break - fall through */
    }
    case EV_NCO_MODE_0:
    case EV_NCO_MODE_1:
    case EV_NCO_MODE_2:
    {
      if (rr_idle_drx_data_ptr->state != RR_IDLE_DRX_STATE_NULL)
      {
        if (rr_idle_drx_data_ptr->pend_req_valid)
        {
          MSG_GERAN_HIGH_0_G("Processing saved GERAN_GRR_MTC_CFG_REQ");
          rr_idle_drx_process_mtc_cfg_req(&rr_idle_drx_data_ptr->pend_req, &notify_L1, &stop_meas, gas_id);
          rr_idle_drx_data_ptr->pend_req_valid = FALSE;
        }
        else
        {
          /* Now apply the config and move to new state */
          (void) rr_idle_drx_apply_config(&notify_L1, &stop_meas, FALSE, rr_idle_drx_data_ptr->extended_dsc, gas_id);
        }
      }
      else
      {
        LOG_UNEXPECTED_EVENT(rr_event_name(drx_event), rr_idle_drx_state_name(rr_idle_drx_data_ptr->state));
      }

      break;
    } /* EV_MAC_IDLE_MODE || EV_NCO_MODE_0 || EV_NCO_MODE_1 || EV_NCO_MODE_2 */
          
    case EV_IDLE_DRX_DSC_BELOW_TH_LOW:
    {
      if (rr_idle_drx_data_ptr->state != RR_IDLE_DRX_STATE_NULL)
      {
        if (rr_idle_drx_data_ptr->state != RR_IDLE_DRX_STATE_NORMAL_DRX_NMEAS_ON)
        {
          /* Set is_dsc_below_th to TRUE so that we will move to NORMAL_DRX_NMEAS_ON state */
          rr_idle_drx_data_ptr->is_dsc_below_th = TRUE;
          (void) rr_idle_drx_apply_config(&notify_L1, &stop_meas, FALSE, rr_idle_drx_data_ptr->extended_dsc, gas_id);
        }
      }
      else
      {
        LOG_UNEXPECTED_EVENT(rr_event_name(drx_event), rr_idle_drx_state_name(rr_idle_drx_data_ptr->state));
      }

      break;
    } /* EV_IDLE_DRX_DSC_BELOW_TH_LOW */
          
    case EV_IDLE_DRX_DSC_ABOVE_TH_HIGH:
    {
      if (rr_idle_drx_data_ptr->state != RR_IDLE_DRX_STATE_NULL)
      {
        if (rr_idle_drx_data_ptr->state == RR_IDLE_DRX_STATE_NORMAL_DRX_NMEAS_ON)
        {
          boolean applied;

          /* Set is_dsc_below_th to FALSE so that we will move to a new state */
          rr_idle_drx_data_ptr->is_dsc_below_th = FALSE;
          applied = rr_idle_drx_apply_config(&notify_L1, &stop_meas, FALSE, 0, gas_id);
           
          /* If we have not moved to a new state then set is_dsc_below_th back to TRUE */
          if (!applied)
          {
            rr_idle_drx_data_ptr->is_dsc_below_th = TRUE;
          }
        }
      }
      else
      {
        LOG_UNEXPECTED_EVENT(rr_event_name(drx_event), rr_idle_drx_state_name(rr_idle_drx_data_ptr->state));
      }

      break;
    } /* EV_IDLE_DRX_DSC_ABOVE_TH_HIGH */

    case EV_IDLE_DRX_SCALING_SUSPEND_TIMEOUT:
    {
      if (rr_idle_drx_data_ptr->state == RR_IDLE_DRX_STATE_NORMAL_DRX_NMEAS_ON)
      {
        /* Timeout can happen at any time. So, check if we are in a state where config can be applied. */
        if (rr_idle_drx_can_configure_device(gas_id))
        {
          if (rr_idle_drx_data_ptr->pend_req_valid)
          {
            MSG_GERAN_HIGH_0_G("Processing saved GERAN_GRR_MTC_CFG_REQ");
            rr_idle_drx_process_mtc_cfg_req(&rr_idle_drx_data_ptr->pend_req, &notify_L1, &stop_meas, gas_id);
            rr_idle_drx_data_ptr->pend_req_valid = FALSE;
          }
          else
          {
            /* Now apply the config and move to new state */
            (void) rr_idle_drx_apply_config(&notify_L1, &stop_meas, FALSE, 0, gas_id);
          }
        }
      }
      else
      {
        LOG_UNEXPECTED_EVENT(rr_event_name(drx_event), rr_idle_drx_state_name(rr_idle_drx_data_ptr->state));
      }

      break;
    } /* EV_IDLE_DRX_SCALING_SUSPEND_TIMEOUT */

    case EV_DEACTIVATE_GSM_MODE:
    case EV_RESET_SOFTWARE:
    {
      rr_idle_drx_deinit(gas_id);
      break;
    } /* EV_DEACTIVATE_GSM_MODE || EV_RESET_SOFTWARE */
    
    default:
    {
      LOG_UNEXPECTED_EVENT(rr_event_name(drx_event), rr_idle_drx_state_name(rr_idle_drx_data_ptr->state));
    }
  }


  if (stop_meas)
  {
    MSG_GERAN_HIGH_0_G("Stop measurements...");
    rr_reset_surrounding_database(gas_id);
    rr_gprs_reset_nmeas_in_reselection_list(gas_id);
    rr_g2wr_reset_wcdma_surrounding_database(gas_id);
  }

  if (notify_L1)
  {
    gprs_scell_info_t *scell = rr_gprs_get_scell_info(gas_id);
    boolean            sf_valid;
    boolean            skip_meas;
    RR_NULL_CHECK_FATAL(scell);

    if (rr_idle_drx_data_ptr->state == RR_IDLE_DRX_STATE_NORMAL_DRX_NMEAS_ON)
    {
      sf_valid  = FALSE;
      skip_meas = FALSE;
    }
    else if (rr_idle_drx_data_ptr->state == RR_IDLE_DRX_STATE_NORMAL_DRX_NMEAS_OFF)
    {
      sf_valid  = FALSE;
      skip_meas = TRUE;
    }
    else
    {
      sf_valid  = rr_idle_drx_data_ptr->drx_scaling_factor_valid;
      skip_meas = rr_idle_drx_data_ptr->skip_idle_mode_meas;
    }

    rr_l1_send_mph_update_idle_drx_ind(
      scell->gsm.control_channel_description.BS_PA_MFRMS,
      sf_valid,
      rr_idle_drx_data_ptr->extended_bs_pa_mfrms,
      skip_meas,
      gas_id
    );
  }

  if (rr_idle_drx_data_ptr->old_state != rr_idle_drx_data_ptr->state)
  {
#ifdef DEBUG_RR_TRACE_MSG
    rr_store_trace_msg_buf(
      RR_IDLE_DRX_SM,
      drx_event,
      rr_idle_drx_data_ptr->state,
      msg_ptr,
      gas_id
    );
#endif /* DEBUG_RR_TRACE_MSG */

    RR_LOG_STATE_TRANSITION(
      "rr_idle_drx_control",
      rr_idle_drx_data_ptr->old_state,
      rr_idle_drx_data_ptr->state,
      rr_idle_drx_state_name,
      gas_id
    );

    rr_idle_drx_data_ptr->old_state = rr_idle_drx_data_ptr->state;
  }

  return;
}
#endif /* FEATURE_IDLE_DRX_SCALING */

/* EOF */


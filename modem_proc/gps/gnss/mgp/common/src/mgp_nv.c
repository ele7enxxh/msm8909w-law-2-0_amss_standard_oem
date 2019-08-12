/*----------------------------------------------------------------------------
 *  Copyright (c) 2015 Qualcomm Atheros, Inc.
 *  All Rights Reserved.
 *  Qualcomm Atheros Confidential and Proprietary.
 *----------------------------------------------------------------------------
 */
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           MGP NV Items Module

GENERAL DESCRIPTION
  This module contains the MGP interface routines for the Non-Volatile Memory
  module.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/mgp/common/src/mgp_nv.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/01/31    spn    Change the default Value of NV 0x4544 ( CGPC IPC Logs )
06/02/08    ah     Modified Altitude Aiding default value from 0 to 3.
01/22/08    ah     Added more default values for new NV items related to ME.
                   Modified gpsOne RF Loss default value from 60 to 30.
03/14/07    ah     Added default values for new NV items related to ME.
                   Modified mgp_nv_read() by adding thread_id argument, to
                   support multiple threads reading NV items simultaneously.
03/12/07    ah     Added default values for existing NV items related to PE.
02/20/07    ah     Initial Release.

=============================================================================

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*
 * Include files
*/
#include <stdlib.h>
#include "comdef.h"
//#include "customer.h"

#include "mgp_nv.h"

#include "err.h"
#include "msg.h"
#include "aries_os_api.h"
#include "gnss_common.h"
#include "gps_consts.h"


/*
 * Constant definitions
*/
#define C_MGP_NV_READ_SIGNAL  0x01

/*
 * Structure definitions
*/

/* See CGPS_NV_Loader.pl script which will write MGP NV item default values */

static boolean            mgp_nv_initialized = FALSE;
static nv_cmd_type        mgp_nv_cmd;
static q_type             mgp_nv_wr_cmd_q;
static mgp_nv_clnt_s_type mgp_nv_clnt_tbl[MGP_NV_CLIENT_HANDLE_MAX];


/*
 * Function definitions
*/

/*===========================================================================

FUNCTION mgp_nv_init

DESCRIPTION
  This function is used to initialize internal MGP-NV data structure.
  It will be called when the first client calls mgp_nv_write_reg.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/


void mgp_nv_init (void)
{
  int i;

  if (mgp_nv_initialized == FALSE)
  {
    (void)q_init(&mgp_nv_wr_cmd_q);

    for (i = 0; i < MGP_NV_CLIENT_HANDLE_MAX; i++)
    {
      mgp_nv_clnt_tbl[i].active    = FALSE;
      mgp_nv_clnt_tbl[i].thread_id = (uint32)THREAD_ID_ERROR;
      mgp_nv_clnt_tbl[i].nv_cb_fn  = NULL;
    }

    mgp_nv_initialized = TRUE;
  }
  else
  {
    MSG_ERROR("MGP_NV_INIT: already initialized", 0 ,0 ,0);
  }
} /* mgp_nv_init */

/*===========================================================================

FUNCTION mgp_nv_write_reg

DESCRIPTION
  This function is used to register a client that will use this NV module.
  It will register the client's callback function after an NV Write command
  has completed, and return a client handle that is needed for future writes.

RETURN VALUE
  Client handle value (or 0xFF if error)

DEPENDENCIES

===========================================================================*/

uint32 mgp_nv_write_reg (uint32 thread_id, mgp_nv_cmd_cb_f_type nv_clnt_cb_fn)
{
  uint32 i;

  /* MGP NV Module Init sanity check */
  if (mgp_nv_initialized == FALSE)
  {
    MSG_HIGH("MGP_NV_INIT: Initializing MGP NV module", 0, 0, 0);
    mgp_nv_init();
  }

  /* Passed argument sanity check */
  if ((os_ThreadIdEnumType)thread_id >= THREAD_ID_MAX)
  {
    MSG_ERROR("MGP_NV_INIT: FAILED - Unknown parameter", 0, 0, 0);
    return (uint32)MGP_NV_CLIENT_HANDLE_ERROR;
  }

  /* Passed argument sanity check */
  if (nv_clnt_cb_fn == NULL)
  {
    MSG_ERROR("MGP_NV_INIT: FAILED - NULL parameter", 0, 0, 0);
    return (uint32)MGP_NV_CLIENT_HANDLE_ERROR;
  }

  /* Look for room in the client table to register NV write cmd callback */
  for (i = 0; i < MGP_NV_CLIENT_HANDLE_MAX; i++)
  {
    if (mgp_nv_clnt_tbl[i].active == TRUE)
    {
      if (mgp_nv_clnt_tbl[i].thread_id == thread_id)
      {
        MSG_ERROR("MGP_NV_INIT: already initialized", 0, 0, 0);
        return i;
      }
    }
    else /* mgp_nv_clnt_tbl[i].active == FALSE */
    {
      mgp_nv_clnt_tbl[i].active    = TRUE;
      mgp_nv_clnt_tbl[i].thread_id = thread_id;
      mgp_nv_clnt_tbl[i].nv_cb_fn  = nv_clnt_cb_fn;
      return i;
    }
  }

  /* Can't initialize since maximum number of clients initialized */
  MSG_ERROR("MGP_NV_INIT: FAILED - max # of clients reached", 0, 0, 0);
  return (uint32)MGP_NV_CLIENT_HANDLE_ERROR;
} /* mgp_nv_write_reg */

/*===========================================================================

FUNCTION mgp_nv_handle_wr_cmd

DESCRIPTION
  This function is used to handle a NV Write command for NV. This function
  services the request at the head of the NV Write command queue and waits
  until NV returns with a response.

RETURN VALUE
  None

DEPENDENCIES
  MGP NV write callback needs to be registered via call to mgp_nv_write_reg

===========================================================================*/

static void mgp_nv_handle_wr_cmd (void)
{
  mgp_nv_item_s_type* nv_ptr;

  if (mgp_nv_initialized)
  {
    nv_ptr = (mgp_nv_item_s_type*)q_check(&mgp_nv_wr_cmd_q);

    /* Send NV write command to NV module only if a request exists in queue */
    if (nv_ptr != NULL)
    {
      /* Populate NV write command */
      mgp_nv_cmd.item       = nv_ptr->item;
      mgp_nv_cmd.cmd        = NV_WRITE_F;
      mgp_nv_cmd.data_ptr   = (nv_item_type*)&(nv_ptr->data);
      mgp_nv_cmd.tcb_ptr    = os_Self();
      mgp_nv_cmd.sigs       = (uint32) C_OS_FLAG_NV_WRITE;
      mgp_nv_cmd.done_q_ptr = NULL;


#ifdef FEATURE_GNSS_NO_NV_WRITES
      #error code not present
#else
      /* Send NV write command to NV module */
      nv_cmd(&mgp_nv_cmd);
#endif
    }
  }
  else
  {
    MSG_ERROR("MGP_NV_HANDLE_WR_CMD: MGP NV not initialized", 0 ,0 ,0);
  }
  return;
} /* mgp_nv_handle_wr_cmd */

/*===========================================================================

FUNCTION mgp_nv_write

DESCRIPTION
  This function is used to queue NV Write commands to NV. The writes to NV
  are performed in asynchronous fashion.

RETURN VALUE
  TRUE  - NV write command successfully sent to NV
  FALSE - NV write command failed to be sent to NV

DEPENDENCIES
  MGP NV write callback needs to be registered via call to mgp_nv_write_reg

===========================================================================*/

boolean mgp_nv_write (nv_items_enum_type nv_item, const void* item_ptr, uint32 handle)
{
  mgp_nv_item_s_type* nv_ptr;

  if (mgp_nv_initialized)
  {
    /* Since NV read and writes should normally be done in GPS idle state
     * (no sessions), no separate allocation of space is memory is done.
     * The buffers will be used from a dynamic buffer pool.
     */
    nv_ptr = (mgp_nv_item_s_type*)os_MemAlloc(sizeof(mgp_nv_item_s_type), OS_MEM_SCOPE_TASK);

    /* Dynamic buffer allocation sanity check */
    if (nv_ptr == NULL)
    {
      MSG_ERROR("MGP_NV_WRITE: FAILED - no buffer", 0, 0, 0);

      os_MemFree((void**)&nv_ptr);
      return FALSE;
    }

    /* Passed argument sanity check */
    if (item_ptr == NULL)
    {
      MSG_ERROR("MGP_NV_WRITE: FAILED - NULL parameter", 0, 0, 0);

      os_MemFree((void**)&nv_ptr);
      return FALSE;
    }

    /* Passed argument sanity check */
    if (handle >= MGP_NV_CLIENT_HANDLE_MAX)
    {
      MSG_ERROR("MGP_NV_WRITE: FAILED - bad handle", 0, 0, 0);

      os_MemFree((void**)&nv_ptr);
      return FALSE;
    }

    /* Populate NV write command queue element */
    nv_ptr->item   = nv_item;
    nv_ptr->data   = *((mgp_nv_u_type*)item_ptr);
    nv_ptr->handle = handle;

    /* Queue a request for writing to NV. Put the command into the
     * NV Write command queue and services them one at a time.
     *
     *  NOTE - The funky check for nv_ptr->link == nv_ptr and the
     *  undesirable cast of nv_ptr to (q_link_type) is being done to
     *  resolve a Lint HIGH error.  Lint thinks there is a potential
     *  memory leak unless nv_ptr is passed to another function OR
     *  freed before returning from this function.  The IF check was
     *  added for safety since an explicit cast like this is very
     *  dangerous and will cause undesired behavior if nv_ptr->link
     *  and nv_ptr are not equivalent.  We will FATAL ERROR if they
     *  are not equivalent.
     */
    if( (void *) &nv_ptr->link == (void *) nv_ptr )
    {
      q_put(&mgp_nv_wr_cmd_q, (q_link_type *) nv_ptr);
    }
    else
    {
      ERR_FATAL("MGP_NV_WRITE: queue.h API structure invalid", 0 ,0 ,0);
    }

    if (q_cnt(&mgp_nv_wr_cmd_q) == 1)
    {
      /* If NV Write command queue has only one item, handle it immediately */
      mgp_nv_handle_wr_cmd();
    }
    return TRUE;
  }
  else
  {
    MSG_ERROR("MGP_NV_WRITE: MGP NV not initialized", 0 ,0 ,0);
    return FALSE;
  }
} /* mgp_nv_write */

/*===========================================================================

FUNCTION mgp_nv_write_done

DESCRIPTION
  This function is used to handle NV write callback status. If there are any
  clients who issued write requests, they will be notified and any pending
  write requests will be subsequently handled.

RETURN VALUE
  TRUE  - NV write success
  FALSE - NV write failure

DEPENDENCIES

===========================================================================*/

boolean mgp_nv_write_done (void)
{
  if((mgp_nv_wr_cmd_q.mutex == NULL) || (mgp_nv_initialized == FALSE))
  {
    MSG_ERROR("MGP_NV_WRITE: Queue Not Initialized", 0, 0, 0);
    return FALSE;  
  }
  /* First dequeue the write request at the head */
  mgp_nv_item_s_type* nv_ptr = ((mgp_nv_item_s_type*)q_get(&mgp_nv_wr_cmd_q));

  if (nv_ptr == NULL)
  {
    MSG_ERROR("MGP_NV_WRITE: No write requested", 0, 0, 0);
    os_MemFree((void**)&nv_ptr);
    return FALSE;
  }
  else
  {
    /* Make sure first dequeued element is same as current NV write element */
    if (mgp_nv_cmd.item != nv_ptr->item)
    {
      MSG_ERROR("MGP_NV_WRITE: FAILED - Non-matching item", 0, 0, 0);
      os_MemFree((void**)&nv_ptr);
      return FALSE;
    }
    else
    {
      /* Check NV write command result status */
      if (mgp_nv_cmd.status != NV_DONE_S)
      {
        /* NV write failed. */
        MSG_ERROR("MGP_NV_WRITE: FAILED - Error Status %d", mgp_nv_cmd.status, 0, 0);
        os_MemFree((void**)&nv_ptr);
        return FALSE;
      }

      /* Out-of-bound array index sanity check */
      if (nv_ptr->handle >= MGP_NV_CLIENT_HANDLE_MAX ||
          &nv_ptr->data == NULL)
      {
        MSG_ERROR("MGP_NV_WRITE: FAILED - bad parameters", 0, 0, 0);
        os_MemFree((void**)&nv_ptr);
        return FALSE;
      }
      else
      {
        /* Call appropriate client's registered callback function */
        mgp_nv_clnt_tbl[nv_ptr->handle].nv_cb_fn(nv_ptr->item, (void*)&nv_ptr->data, mgp_nv_cmd.status);

        /* Check if clients were waiting for NV write success */
        //pa_info_available(mgp_nv_cmd.item);

        /* Handle next NV write request */
        mgp_nv_handle_wr_cmd();
      }
    }
    os_MemFree((void**)&nv_ptr);
  }
  return TRUE;
} /* mgp_nv_write_done */

/*===========================================================================

FUNCTION mgp_nv_read_default

DESCRIPTION
  This function is used to return the default value for the NV item to be
  read. This should be used only when failing to read from NV.

RETURN VALUE
  TRUE  - NV read success
  FALSE - NV read failure

DEPENDENCIES

===========================================================================*/

static boolean mgp_nv_read_default (nv_items_enum_type nv_item, const void *item_ptr)
{
  uint32 i;

  MSG_MED("MGP_NV_READ: Default NV item %d returned", nv_item, 0, 0);

  switch (nv_item)
  {
    /*****************************************/
    /* Position Engine (PE) Related NV Items */
    /*****************************************/

    case NV_SIGMAREJECT_I:
      /* Default value: Sigma reject = 500 */
      ((nv_item_type*)item_ptr)->sigmareject = 500; //uint32
      break;

    case NV_VELNOISEINONESECDRIVE_I:
      /* Default value: Velocity noise in one second: drive mode = 150000 */
      ((nv_item_type*)item_ptr)->velnoiseinonesecdrive = 150000; //uint32
      break;

    case NV_VELNOISEINONESECPED_I:
      /* Default value: Velocity noise in one second: pedestrian mode = 80000 */
      ((nv_item_type*)item_ptr)->velnoiseinonesecped = 80000; //uint32
      break;

    case NV_VELNOISEINONESECSTATIC_I:
      /* Default value: Velocity noise in one second: static mode = 15000 */
      ((nv_item_type*)item_ptr)->velnoiseinonesecstatic = 15000; //uint32
      break;

    case NV_EXTRACLOCKBIASERROR_I:
      /* Default value: Extra clock bias error = 0 */
      ((nv_item_type*)item_ptr)->extraclockbiaserror = 0; //uint32
      break;

    case NV_CONFIGMASK_I:
      /* Default value: KF configuration mask = 130 */
      ((nv_item_type*)item_ptr)->configmask = 130; //uint16
      break;

    case NV_MAXPROPAGATIONWONEWMEAS_I:
      /* Default value: Maximum propagation without new measurements = 600 */
      ((nv_item_type*)item_ptr)->maxpropagationwonewmeas = 600; //uint32
      break;

    case NV_CONFIGCONTROLFLAG_I:
      /* Default value: KF Configuration Control Flag = 1 */
      ((nv_item_type*)item_ptr)->configcontrolflag = 1; //uint8
      break;

    case NV_AAGPS_DEVELOPMENT_TEST_CONTROL5_I:
      /* Default value: AAGPS Development Test Control 5 = 32 */
      ((nv_item_type*)item_ptr)->aagps_development_test_control5 = 32; //uint32
      break;

    case NV_GPS1_DYNAMIC_MODE_I:
      /* Default value: GPS Dynamic Mode Switching = 1 */
      ((nv_item_type*)item_ptr)->gps1_dynamic_mode = 1; //uint8
      break;

    case NV_CGPS_DGPS_CORRECTIONS_ALLOWED_I:
      /* Default value: CGPS DGPS corrections allowed = TRUE */
      ((nv_item_type*)item_ptr)->cgps_dgps_corrections_allowed = TRUE; //uint8
      break;

    case NV_CGPS_MAXIMUM_DGPS_INTERVAL_I:
      /* Default value: CGPS maximum DGPS interval = 6000 */
      ((nv_item_type*)item_ptr)->cgps_maximum_dgps_interval = 6000; //uint16
      break;

    case NV_CGPS_USE_FDIC_I:
      /* Default value: CGPS use FDIC = TRUE */
      ((nv_item_type*)item_ptr)->cgps_use_fdic = TRUE; //uint8
      break;

    case NV_CGPS_ALTITUDE_HOLD_MODE_I:
      /* Default value: CGPS Altitude hold mode = 3 */
      ((nv_item_type*)item_ptr)->cgps_altitude_hold_mode = 3; //uint8
      break;

    case NV_CGPS_PDOP_MASK_I:
      /* Default value: CGPS PDOP mask = 12 */
      ((nv_item_type*)item_ptr)->cgps_pdop_mask = 12; //uint8
      break;

    case NV_CGPS_2D_PDOP_MASK_I:
      /* Default value: CGPS 2-D PDOP mask = 8 */
      ((nv_item_type*)item_ptr)->cgps_2d_pdop_mask = 8; //uint8
      break;

    case NV_CGPS_REFERENCE_MODE_I:
      /* Default value: CGPS Reference mode = FALSE */
      ((nv_item_type*)item_ptr)->cgps_reference_mode = FALSE; //uint8
      break;

    case NV_CGPS_OPERATION_MODE_I:
      /* Default value: CGPS Operation mode = 1 (PE_SPEED_AND_ACCURACY) */
      ((nv_item_type*)item_ptr)->cgps_operation_mode = 1; //uint8
      break;

    case NV_CGPS_ELEVATION_MASK_I:
      /* Default value: CGPS Elevation mask = 5 */
      ((nv_item_type*)item_ptr)->cgps_elevation_mask = 5; //int8
      break;

    case NV_CGPS_MAX_ALMANAC_AGE_IN_WEEKS_I:
      /* Default value: CGPS maximum almanac age (weeks) = 50 */
      ((nv_item_type*)item_ptr)->cgps_max_almanac_age_in_weeks = 50; //uint8
      break;

    case NV_CGPS_STEERING_ON_STARTUP_I:
      /* Default value: CGPS steering on startup = TRUE */
      ((nv_item_type*)item_ptr)->cgps_steering_on_startup = TRUE; //uint8
      break;

    /********************************************/
    /* Measurement Engine (ME) Related NV Items */
    /********************************************/

    case NV_GPS1_GPS_RF_LOSS_I:
      /* Default value: gpsOne RF Loss = 30 */
      ((nv_item_type*)item_ptr)->gps1_gps_rf_loss = 30; //byte
      break;

    case NV_AAGPS_MAX_OSC_UNC_I:
      /* Default value: Maximum Oscillator Frequency Uncertainty = 20 ppm */
      ((nv_item_type*)item_ptr)->aagps_max_osc_unc = (uint32) C_MAX_CLK_FREQ_UNC; //uint32
      break;

    case NV_AAGPS_MAX_OSC_UNC_RATE_I:
      /* Default value: Maximum Oscillator Uncertainty Rate = 1 */
      ((nv_item_type*)item_ptr)->aagps_max_osc_unc_rate = 1; //uint32
      break;

    case NV_AAGPS_MAX_PLATFORM_VELOCITY_I:
      /* Default value: Maximum Platform Velocity = 34 */
      ((nv_item_type*)item_ptr)->aagps_max_platform_velocity = 34; //uint32
      break;

    case NV_AAGPS_MAX_PLATFORM_ACCL_I:
      /* Default value: Maximum Platform Acceleration = 1 */
      ((nv_item_type*)item_ptr)->aagps_max_platform_accl = 1; //uint32
      break;

    case NV_GPS1_SEEDPOS_OPTION_I:
      /* Default value: Seed Position Engine Option Mask = 0xFFFF */
      ((nv_item_type*)item_ptr)->gps1_seedpos_option = 0xFFFF; //uint16
      break;


    case NV_CGPS_MAX_OSC_UNC_ACTIVE_SYS_I:
      /* Default value: Max Clock Frequency Uncertainty (Active System) = 99 */
      ((nv_item_type*)item_ptr)->cgps_max_osc_unc_active_sys = 99; //uint32
      break;

    case NV_CGPS_MAX_OSC_UNC_RGS_I:
      /* Default value: Max Clock Frequency Uncertainty (RGS) = 99 */
      ((nv_item_type*)item_ptr)->cgps_max_osc_unc_rgs = 99; //uint32
      break;

    case NV_CGPS_MAX_OSC_UNC_TEMP_TBL_I:
      /* Default value: Max Clock Frequency Uncertainty (Temp Table) = 150 */
      ((nv_item_type*)item_ptr)->cgps_max_osc_unc_temp_tbl = 150; //uint32
      break;

    case NV_CGPS_MAX_OSC_UNC_RGS_OLD_I:
      /* Default value: Max Clock Frequency Uncertainty (Old RGS) = 600 */
      ((nv_item_type*)item_ptr)->cgps_max_osc_unc_rgs_old = 600; //uint32
      break;

    case NV_CGPS_MAX_CARRIER_CODE_FILTER_I:
      /* Default value: Max Carrier to Code Filter Constant = 0 */
      ((nv_item_type*)item_ptr)->cgps_max_carrier_code_filter = 0; //uint8
      break;

    case NV_CGPS_MAX_INTEGRATE_MS_I:
      /* Default value: Max Integration = 12000 */
      ((nv_item_type*)item_ptr)->cgps_max_integrate_ms = 12000; //uint32
      break;

    case NV_CGPS_ME_RESERVED1_I:
      /* Default value: ME Reserved1 = 0 */
      ((nv_item_type*)item_ptr)->cgps_me_reserved1 = 0; //uint32
      break;

    case NV_CGPS_ME_RESERVED2_I:
      /* Default value: ME Reserved2 = 0 */
      ((nv_item_type*)item_ptr)->cgps_me_reserved2 = 0; //uint32
      break;

    case NV_CGPS_ME_RESERVED3_I:
      /* Default value: ME Reserved3 = 0 */
      ((nv_item_type*)item_ptr)->cgps_me_reserved3 = 0; //uint32
      break;

    case NV_CGPS_ME_RESERVED4_I:
      /* Default value: ME Reserved4 = 0 */
      ((nv_item_type*)item_ptr)->cgps_me_reserved4 = 0; //uint32
      break;

    case NV_CGPS_MEM_CONFIG_I:
      /* Default value: CGPS Memory Configuration (High Mem vs Low Mem) = 0 */
      ((nv_item_type*)item_ptr)->cgps_mem_config = 0; //uint8
      break;

    case NV_GPS_ENGINE_TYPE_I:
      /* Default value: GPS Engine Type = 2 (gpsOne Gen 7 Engine) */
      ((nv_item_type*)item_ptr)->gps_engine_type = GPSONE_ENGINE_TYPE_GEN7; //uint8
      break;

    case NV_CGPS_VCO_AGE_I:
      /* Default value: VCO Age [ppm per year] = 90 */
      ((nv_item_type*)item_ptr)->cgps_vco_age = 90; //uint8
      break;

    case NV_CGPS_OPTIMISTIC_PUNC_I:
      /* Default value: Optimistic Position Uncertainty = 1 */
      ((nv_item_type*)item_ptr)->cgps_optimistic_punc = 1; //uint8
      break;

    case NV_CGPS_SBAS_ENABLED_I:
      /* Default value: SBAS enabled = FALSE */
      ((nv_item_type*)item_ptr)->cgps_sbas_enabled = 0; //uint8
      break;

    case NV_CGPS_SBAS_USER_PREFERENCE_I:
      /* Default value: SBAS user preference = 0 */
      ((nv_item_type*)item_ptr)->cgps_sbas_user_preference = 0; //uint8
      break;

    case NV_CGPS_DPO_CONTROL_I:
      /* Default value: DPO is enabled by default */
      /* Enable DPO only if not on a charger. Set the NV item to '2'
         For DPO NV item default, the CGPS_NV_Loader should use '1', but the code should default to '2'.
         The reason for this is that, if the NV item is set to '2', DPO would be disabled if the phone
         is connected to a charger. This might affect test results for field tests for customers
         and our test team. So, the plan is to leave CGPS_NV_Loader.pl to default enable DPO (with option '1')
         So, test teams can run the NV loader before starting any DPO specific tests.
      **/
      ((nv_item_type*)item_ptr)->cgps_dpo_control = 2; //uint8
      break;
//Ranjan_HACK_Aries5.0Merge
#if 0
    case NV_CGPS_DPO_ENTRY_CRITERIA_CONTROL_I:
      /* Default value: DPO is enabled by default */
      ((nv_item_type*)item_ptr)->cgps_dpo_entry_criteria_control = 0x00003FFF; //uint32
      break;
#endif

    case NV_CGPS_SBAS_WAAS_AREA_NODE1_I:
      /* Default value: (2, -45) lat = 4 deg, long = -90 deg */
      ((nv_item_type*)item_ptr)->cgps_sbas_waas_area_node1.latitude = 2; //int8
      ((nv_item_type*)item_ptr)->cgps_sbas_waas_area_node1.longitude = -45; //int8
      break;

    case NV_CGPS_SBAS_WAAS_AREA_NODE2_I:
      /* Default value: (25, -20) lat = 50 deg, long = -40 deg */
      ((nv_item_type*)item_ptr)->cgps_sbas_waas_area_node2.latitude = 25; //int8
      ((nv_item_type*)item_ptr)->cgps_sbas_waas_area_node2.longitude = -20; //int8
      break;

    case NV_CGPS_SBAS_WAAS_AREA_NODE3_I:
      /* Default value: (38, -40) lat = 76 deg, long = -80 deg */
      ((nv_item_type*)item_ptr)->cgps_sbas_waas_area_node3.latitude = 38; //int8
      ((nv_item_type*)item_ptr)->cgps_sbas_waas_area_node3.longitude = -40; //int8
      break;

    case NV_CGPS_SBAS_WAAS_AREA_NODE4_I:
      /* Default value: (38, -90) lat = 76 deg, long = -180 deg */
      ((nv_item_type*)item_ptr)->cgps_sbas_waas_area_node4.latitude = 38; //int8
      ((nv_item_type*)item_ptr)->cgps_sbas_waas_area_node4.longitude = -90; //int8
      break;

    case NV_CGPS_SBAS_WAAS_AREA_NODE5_I:
      /* Default value: (25, -90) lat = 50 deg, long = -180 deg */
      ((nv_item_type*)item_ptr)->cgps_sbas_waas_area_node5.latitude = 25; //int8
      ((nv_item_type*)item_ptr)->cgps_sbas_waas_area_node5.longitude = -90; //int8
      break;

    case NV_CGPS_SBAS_WAAS_AREA_NODE6_I:
      /* Default value: (127, 127) lat = INVALID, long = INVALID */
      ((nv_item_type*)item_ptr)->cgps_sbas_waas_area_node6.latitude = 
        C_SBAS_INVALID_AREA_NODE; //int8
      ((nv_item_type*)item_ptr)->cgps_sbas_waas_area_node6.longitude = 
        C_SBAS_INVALID_AREA_NODE; //int8
      break;

    case NV_CGPS_SBAS_EGNOS_AREA_NODE1_I:
      /* Default value: (13, -12) lat = 26 deg, long = -24 deg */
      ((nv_item_type*)item_ptr)->cgps_sbas_egnos_area_node1.latitude = 13; //int8
      ((nv_item_type*)item_ptr)->cgps_sbas_egnos_area_node1.longitude = -12; //int8
      break;

    case NV_CGPS_SBAS_EGNOS_AREA_NODE2_I:
      /* Default value: (18, 17) lat = 36 deg, long = 34 deg */
      ((nv_item_type*)item_ptr)->cgps_sbas_egnos_area_node2.latitude = 18; //int8
      ((nv_item_type*)item_ptr)->cgps_sbas_egnos_area_node2.longitude = 17; //int8
      break;

    case NV_CGPS_SBAS_EGNOS_AREA_NODE3_I:
      /* Default value: (27, 16) lat = 54 deg, long = 32 deg */
      ((nv_item_type*)item_ptr)->cgps_sbas_egnos_area_node3.latitude = 27; //int8
      ((nv_item_type*)item_ptr)->cgps_sbas_egnos_area_node3.longitude = 16; //int8
      break;

    case NV_CGPS_SBAS_EGNOS_AREA_NODE4_I:
      /* Default value: (37, 18) lat = 74 deg, long = 36 deg */
      ((nv_item_type*)item_ptr)->cgps_sbas_egnos_area_node4.latitude = 37; //int8
      ((nv_item_type*)item_ptr)->cgps_sbas_egnos_area_node4.longitude = 18; //int8
      break;

    case NV_CGPS_SBAS_EGNOS_AREA_NODE5_I:
      /* Default value: (37, -12) lat = 74, long = -24 */
      ((nv_item_type*)item_ptr)->cgps_sbas_egnos_area_node5.latitude = 37; //int8
      ((nv_item_type*)item_ptr)->cgps_sbas_egnos_area_node5.longitude = -12; //int8
      break;

    case NV_CGPS_SBAS_EGNOS_AREA_NODE6_I:
      /* Default value: (127, 127) lat = INVALID, long = INVALID */
      ((nv_item_type*)item_ptr)->cgps_sbas_egnos_area_node6.latitude = 
        C_SBAS_INVALID_AREA_NODE; //int8
      ((nv_item_type*)item_ptr)->cgps_sbas_egnos_area_node6.longitude = 
        C_SBAS_INVALID_AREA_NODE; //int8
      break;

    case NV_CGPS_SBAS_MSAS_AREA_NODE1_I:
      /* Default value: (3, 53) lat = 6 deg, long = 106 deg */
      ((nv_item_type*)item_ptr)->cgps_sbas_msas_area_node1.latitude = 3; //int8
      ((nv_item_type*)item_ptr)->cgps_sbas_msas_area_node1.longitude = 53; //int8
      break;

    case NV_CGPS_SBAS_MSAS_AREA_NODE2_I:
      /* Default value: (3, 73) lat = 6 deg, long = 146 deg */
      ((nv_item_type*)item_ptr)->cgps_sbas_msas_area_node2.latitude = 3; //int8
      ((nv_item_type*)item_ptr)->cgps_sbas_msas_area_node2.longitude = 73; //int8
      break;

    case NV_CGPS_SBAS_MSAS_AREA_NODE3_I:
      /* Default value: (15, 85) lat = 30 deg, long = 170 deg */
      ((nv_item_type*)item_ptr)->cgps_sbas_msas_area_node3.latitude = 15; //int8
      ((nv_item_type*)item_ptr)->cgps_sbas_msas_area_node3.longitude = 85; //int8
      break;

    case NV_CGPS_SBAS_MSAS_AREA_NODE4_I:
      /* Default value: (28, 85) lat = 56 deg, long = 170 deg */
      ((nv_item_type*)item_ptr)->cgps_sbas_msas_area_node4.latitude = 28; //int8
      ((nv_item_type*)item_ptr)->cgps_sbas_msas_area_node4.longitude = 85; //int8
      break;

    case NV_CGPS_SBAS_MSAS_AREA_NODE5_I:
      /* Default value: (28, 52) lat = 56, long = 104 */
      ((nv_item_type*)item_ptr)->cgps_sbas_msas_area_node5.latitude = 28; //int8
      ((nv_item_type*)item_ptr)->cgps_sbas_msas_area_node5.longitude = 52; //int8
      break;

    case NV_CGPS_SBAS_MSAS_AREA_NODE6_I:
      /* Default value: (127, 127) lat = INVALID, long = INVALID */
      ((nv_item_type*)item_ptr)->cgps_sbas_msas_area_node6.latitude = 
        C_SBAS_INVALID_AREA_NODE; //int8
      ((nv_item_type*)item_ptr)->cgps_sbas_msas_area_node6.longitude = 
        C_SBAS_INVALID_AREA_NODE; //int8
      break;

    case NV_CGPS_SBAS_GAGAN_AREA_NODE1_I:
      /* Default value: (3, 34) lat = 6 deg, long = 68 deg */
      ((nv_item_type*)item_ptr)->cgps_sbas_gagan_area_node1.latitude = 3; //int8
      ((nv_item_type*)item_ptr)->cgps_sbas_gagan_area_node1.longitude = 34; //int8
      break;

    case NV_CGPS_SBAS_GAGAN_AREA_NODE2_I:
      /* Default value: (3, 48) lat = 6 deg, long = 96 deg */
      ((nv_item_type*)item_ptr)->cgps_sbas_gagan_area_node2.latitude = 3; //int8
      ((nv_item_type*)item_ptr)->cgps_sbas_gagan_area_node2.longitude = 48; //int8
      break;

    case NV_CGPS_SBAS_GAGAN_AREA_NODE3_I:
      /* Default value: (15, 48) lat = 30 deg, long = 96 deg */
      ((nv_item_type*)item_ptr)->cgps_sbas_gagan_area_node3.latitude = 15; //int8
      ((nv_item_type*)item_ptr)->cgps_sbas_gagan_area_node3.longitude = 48; //int8
      break;

    case NV_CGPS_SBAS_GAGAN_AREA_NODE4_I:
      /* Default value: (18, 38) lat = 36 deg, long = 76 deg */
      ((nv_item_type*)item_ptr)->cgps_sbas_gagan_area_node4.latitude = 18; //int8
      ((nv_item_type*)item_ptr)->cgps_sbas_gagan_area_node4.longitude = 38; //int8
      break;

    case NV_CGPS_SBAS_GAGAN_AREA_NODE5_I:
      /* Default value: (13, 34) lat = 26 deg, long = 68 deg */
      ((nv_item_type*)item_ptr)->cgps_sbas_gagan_area_node5.latitude = 13; //int8
      ((nv_item_type*)item_ptr)->cgps_sbas_gagan_area_node5.longitude = 34; //int8
      break;

    case NV_CGPS_SBAS_GAGAN_AREA_NODE6_I:
      /* Default value: (127, 127) lat = INVALID, long = INVALID */
      ((nv_item_type*)item_ptr)->cgps_sbas_gagan_area_node6.latitude = 
        C_SBAS_INVALID_AREA_NODE; //int8
      ((nv_item_type*)item_ptr)->cgps_sbas_gagan_area_node6.longitude = 
        C_SBAS_INVALID_AREA_NODE; //int8
      break;

    case NV_CGPS_ER_ENABLE_I:
      /* Default value: 1 = ENABLED */
      ((nv_item_type*)item_ptr)->cgps_er_enable = 1; //uint8
      break;

    case NV_CGPS_ER_START_TIME_I:
      /* Default value: 60 seconds */
      ((nv_item_type*)item_ptr)->cgps_er_start_time = 60; //int16
      break;

    case NV_CGPS_ER_SLOT_PERIOD_I:
      /* Default value: 6 seconds */
      ((nv_item_type*)item_ptr)->cgps_er_slot_period = 6; //uint8
      break;

    case NV_CGPS_ER_SLOT_INTERVAL_I:
      /* Default value: 900 seconds */
      ((nv_item_type*)item_ptr)->cgps_er_slot_interval = 900; //uint16
      break;

    case NV_GNSS_GLO_CONTROL_I:
      /* Default value: ENABLED */
      ((nv_item_type*)item_ptr)->gnss_glo_control= 1; //uint8
      break;

    case NV_GNSS_GLO_PATH_LOSS_CENTER_0_I:
      /* Default value: ENABLED */
      ((nv_item_type*)item_ptr)->gnss_glo_path_loss_center_0 = 35; 
      break;

    case NV_GNSS_GLO_PATH_LOSS_CENTER_MINUS7_I:
      /* Default value: ENABLED */
      ((nv_item_type*)item_ptr)->gnss_glo_path_loss_center_minus7 = 35; 
      break;

    case NV_GNSS_GLO_PATH_LOSS_CENTER_PLUS6_I:
      /* Default value: ENABLED */
      ((nv_item_type*)item_ptr)->gnss_glo_path_loss_center_plus6 = 35; 
      break;


    /***************************/
    /* OS API Related NV Items */
    /***************************/

    case NV_AAGPS_IPC_DM_THREAD_MASK_I:
      /* Default value: IPC DM Thread Mask = 0 */
      for (i = 0; i < NUM_OF_AAGPS_IPC_DM_THREAD_MASK_I; i++)
      {
        if(((i >= 5) && (i <= 12)) || i == 19)
        {
          ((nv_item_type*)item_ptr)->aagps_ipc_dm_thread_mask[i] = 0x000FFFFF;
        }
        else
        {
          if(i == 1)
          {
            /* Enable all IPCs to MC task */
            ((nv_item_type*)item_ptr)->aagps_ipc_dm_thread_mask[i] = 0x0001FFEF;
          }
          else if ( ( i == 2) || ( i == 3 ) )
          {
            /* Enable MC->CC and MC->PP IPCs */
            ((nv_item_type*)item_ptr)->aagps_ipc_dm_thread_mask[i] = 0x0003;
          }
          else if ( i == 14 )
          {
            /* Enable MC->PGI and CC->PGI and PP->PGI IPCs */
            ((nv_item_type*)item_ptr)->aagps_ipc_dm_thread_mask[i] = 0x000F;
          }
          /* LBS Task has Thread ID of 16, and enable the 16th bit ( 0 based 
             based index for logging items to LBS Task  */
          else if ( i == 16 )
          {
            ((nv_item_type*)item_ptr)->aagps_ipc_dm_thread_mask[i] = 0x0001FFFF;
          }
          else if(i == 17)
          {
            /* TLM Task has Thread ID of 17, and enable the 17th bit ( 0 based
             based index for logging items to TLM Task
             Log IPC's from TLM, XTM, PGI, ALE and MC */
            ((nv_item_type*)item_ptr)->aagps_ipc_dm_thread_mask[i] = 0x00FFFFFF;
          }
          else if (i == 18)
          {
            /* XTM Task has Thread ID of 18, and enable the 18th bit ( 0 based
             based index for logging items to XTM Task
             Log IPC's from TLM, XTM, PGI, ALE and MC */
            ((nv_item_type*)item_ptr)->aagps_ipc_dm_thread_mask[i] = 0x00FFFFFF;
          }
          else if ((21 == i) || (22 == i))
          {
            /* Enable IPC logging for OTDOA tasks */
            ((nv_item_type*)item_ptr)->aagps_ipc_dm_thread_mask[i] = 0xFFFFFFFF;
          }
          else
          {
            ((nv_item_type*)item_ptr)->aagps_ipc_dm_thread_mask[i] = 0; //uint32
          }
        }
      }
      break;

    case NV_GNSS_MGP_ERROR_RECOVERY_I:
      /* This should be enabled by default for error recovery */
      ((nv_item_type*)item_ptr)->gnss_mgp_error_recovery = 1; /* uint8 */
      break;

    case NV_GNSS_ENABLE_EXTENDED_ON_FEATURE_FOR_GNSS_I:
      /* Disabling Extended Receiver ON functionality by default.
      */
      ((nv_item_type*)item_ptr)->gnss_enable_extended_on_feature_for_gnss = 0; /* uint8 */
      break;

    /* Add more default values here... */


    default:
      return FALSE;
  }

  return TRUE;
} /* mgp_nv_read_default */

/*===========================================================================

FUNCTION mgp_nv_read

DESCRIPTION
  This function is used to perform a synchronous read from NV.

RETURN VALUE
  TRUE  - NV read success
  FALSE - NV read failure (read default value instead)

DEPENDENCIES

===========================================================================*/

boolean mgp_nv_read (nv_items_enum_type nv_item, void* item_ptr, os_ThreadIdEnumType thread_id)
{
  nv_cmd_type cmd_buf;

  /* Passed argument sanity check */
  if (item_ptr == NULL)
  {
    MSG_ERROR("MGP_NV_READ: FAILED - NULL parameter", 0, 0, 0);
    return FALSE;
  }

  /* Populate NV read command */
  cmd_buf.item       = nv_item;
  cmd_buf.cmd        = NV_READ_F;
  cmd_buf.data_ptr   = (nv_item_type*)item_ptr;
  cmd_buf.tcb_ptr    = os_Self();
  cmd_buf.sigs       = (uint32) C_MGP_NV_READ_SIGNAL;
  cmd_buf.done_q_ptr = NULL;

  /* Clear NV Read signal before waiting on it */
  (void) os_ClrLocalSignal( C_MGP_NV_READ_SIGNAL );

  /* Send NV read command to NV module */
  nv_cmd(&cmd_buf);

  /* Block until NV Read signal is set */
  (void) os_WaitLocalSignal( C_MGP_NV_READ_SIGNAL );

  /* Clear NV Read signal after it is read out */
  (void) os_ClrLocalSignal( C_MGP_NV_READ_SIGNAL );

  /* Check status of NV Read */
  if (cmd_buf.status != NV_DONE_S)
  {
    /* Initial NV read failed. Return default value for NV item. */
    if (mgp_nv_read_default(nv_item, item_ptr))
    {
      MSG_MED("MGP_NV_READ: Reading default value for item %d", nv_item, 0, 0);
      return TRUE;
    }
    else
    {
      MSG_ERROR("MGP_NV_READ: FAILED - Error Status %d", cmd_buf.status, 0, 0);
      return FALSE;
    }
  }

  return TRUE;
} /* mgp_nv_read */

/*===========================================================================

FUNCTION mgp_nv_ValidateIpcMasks

DESCRIPTION
  This function is used to enable default recommended settings for
  IPC_NV_settings. If all masks are zero the default setting will be used

RETURN VALUE
   None
DEPENDENCIES

===========================================================================*/

void mgp_nv_ValidateIpcMasks(uint32 * p_NvIpcMasks)
{
  uint8 u_i;

  /* Make sure that we have at least one non-zero mask */

  for(u_i=0; u_i<NUM_OF_AAGPS_IPC_DM_THREAD_MASK_I; ++u_i)
  {
    if(p_NvIpcMasks[u_i])
    {
      MSG_MED("cd_NvInit: Requested NV_AAGPS_IPC_DM_THREAD_MASK_I is used",
              0, 0, 0);
      return;
    }
  }

  if( mgp_nv_read_default(NV_AAGPS_IPC_DM_THREAD_MASK_I, p_NvIpcMasks) )
  {
    MSG_MED("cd_NvInit: NV_AAGPS_IPC_DM_THREAD_MASK_I set to default values",
            0, 0, 0);
  }
  else
  {
    MSG_MED("cd_NvInit: NV_AAGPS_IPC_DM_THREAD_MASK_I unable to set to default values...using 0",
            0, 0, 0);
  }
}

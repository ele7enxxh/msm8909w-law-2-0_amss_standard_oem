/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 M M G S D I   T A S K  I N I T I A L I Z A T I O N  F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains routines used during the MMGSDI task initalization.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2012 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_task_init.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/04/16   ar      EONS selective logging
01/07/16   kv      UIM PSM handling updates
12/25/15   bcho    Send Card and session events based on SFS data in PSM mode
09/25/15   vr      Added support for ERA-GLONASS
04/03/15   bcho    Migrate to mcfg EFS APIs
08/27/15   vdc     Do not retry service registration if it fails once
07/09/15   ar      Validate PRL at diff steps if SD task not ready
04/17/15   nr      Phonebook record read optimization
12/15/14   kk      Fixed ENS settings after NV Refresh
11/15/14   ar      Reduced usage of MMGSDI_TASK_ENTER/LEAVE_CRIT_SECT
10/24/14   am      Fix compile warnings 
09/09/14   kk      Enhancements in NV handling
09/29/14   vv      Added support for the remote SFS
09/21/14   ar      Add check for task_state before accessing its globals
09/11/14   hh      Call rex_undef_timer() when timer is no longer needed
08/29/14   hh      Fix compliation warnings
08/29/14   tl      Split QMI initalization into separate functions
08/22/14   tl      Change client notification when app state is brought
                   down from SUB_READY TO PERSO_READY
08/14/14   tl      Add support for silent PIN verification
07/31/14   hh      Added CM init for UT
07/25/14   av      Auto onchip activation for multi slots
07/15/14   kk      NV Refresh: MCFG - MMGSDI interactions
07/15/14   yt      Support for NV refresh
06/30/14   vv      Remove Perso code
06/24/14   vdc     Move MMGSDI client id registration to MMGSDI init
04/18/14   tl      Add QMI SIM internal global initalization
04/09/14   tl      Perform SIM Lock initalization only in main task
03/27/14   yt      Read slot info from UIM during task init
03/25/14   tl      SIM Lock secondary revisions
03/18/14   tl      Introduce new SIM Lock feature
02/19/14   am      Added logic for clearing timers and queues
01/27/14   df      Rename FEATURE_MMGSDI_TEST_AUTOMATION as FEATURE_UIMDIAG
01/11/14   df      Fix off-target compilation error
01/10/14   df      Use v2.0 diag macros
12/04/13   spo     Do not set signals when MMGSDI task is already stopped
11/27/13   tl      Introduce refresh command queue
11/18/13   vv      Added support for parallel REFRESH
10/04/13   vv      Added NV support for ENS sub-features
10/01/13   av      Perform task-ready sig registrations before rcinit_handshake
09/26/13   av      Enable OnChip on bootup based on EFS item
09/23/13   vr      qtf-crm MOB migration
09/05/13   yt      Remove usage of secapi_get_random retry from perso
08/30/13   tl      Convert channel info table from static to dynamic allocation
08/09/13   kk      Deprecating uimcallctrl and moving task init functions
04/15/13   tl      Remove obsoleted featurization
02/18/12   av      Cleaned up remaining ALWAYS ON features
12/21/11   shr     Initial version
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "comdef.h"
#include "intconv.h"
#include "rex.h"
#include "task.h"
#include "queue.h"

#include "mmgsdi_task_init.h"
#include "mmgsdi_nv.h"
#include "mmgsdicache.h"
#include "mmgsdi_se13.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdi_refresh.h"
#include "mmgsdi_ruim_jcdma.h"
#include "mmgsdi_nv_refresh.h"
#include "nvruim_p.h"
#include "obdapp_p.h"
#include "mmgsdi_silent_pin.h"
#ifdef FEATURE_UIM_SIM_INTERNAL_INTERFACE
#include "qmi_sim_internal.h"
#endif /* FEATURE_UIM_SIM_INTERNAL_INTERFACE */
#ifdef FEATURE_SIMLOCK
#include "simlock_modem_p.h"
#include "simlock_modem_lib.h"
#include "mmgsdi_simlock.h"
#endif /* FEATURE_SIMLOCK */
#include "mmgsdi_nv_refresh.h"
#include "mmgsdi_pb_cache.h"
#include "mmgsdi_multi_profile.h"
#include "mmgsdi_psm.h"
#include "uim_common_efs.h"
#include "uim_selective_logging.h"

#ifdef FEATURE_UIMDIAG
#include "uimdiag.h"
#endif /* FEATURE_UIMDIAG  */

#include "fs_lib.h"
#include "fs_public.h"

#include "mcfg_refresh.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* MMGSDI always on (non-deferrable) timer group */
static timer_group_type mmgsdi_always_on_timer_group;

/*===========================================================================
                  F U N C T I O N   P R O T O T Y P E S
============================================================================*/

/*===========================================================================
FUNCTION MMGSDI_INIT_COMMON

DESCRIPTION
  Function initialize MMGSDI queues for the respective MMGSDI tasks.
  It initializes global variables in main MMGSDI task

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.
===========================================================================*/
void mmgsdi_init_common (
  mmgsdi_task_enum_type  mmgsdi_task
)
{
  uint32 i            = 0;
  uint32 j            = 0;
  int32  client_index = 0;

  /* ---------------------------------------------------------
      INITIALIZE ALL QUEUES
  --------------------------------------------------------- */
  (void) q_init(&mmgsdi_card_status_cmd_q[mmgsdi_task]);
  (void) q_init(&mmgsdi_high_pri_cmd_q[mmgsdi_task]);
  (void) q_init(&mmgsdi_internal_cmd_q[mmgsdi_task]);
  (void) q_init(&mmgsdi_task_cmd_q[mmgsdi_task]);
  (void) q_init(&mmgsdi_task_rsp_q[mmgsdi_task]);

  /* Initialize mmsgdi to uim command state */
  mmgsdi_uim_cmd[mmgsdi_task].cmd_in_progress = FALSE;
  mmgsdi_uim_cmd[mmgsdi_task].index = MMGSDI_MAX_CLIENT_REQ_INFO;

  if (mmgsdi_task != MMGSDI_TASK_MAIN)
  {
    return;
  }

  (void) q_init(&mmgsdi_nv_q);
  (void) q_init(&mmgsdi_refresh_cmd_q);
#ifdef FEATURE_SIMLOCK
  (void) q_init(&mmgsdi_simlock_rsp_q);
#endif /* FEATURE_SIMLOCK */

  /* ------------------------------------------------------------------------
  ** Initialization of the Critical Section only by main MMGSDI task
  ** ----------------------------------------------------------------------*/
  rex_init_crit_sect(mmgsdi_client_app_data_crit_sect_ptr);
  rex_init_crit_sect(mmgsdi_task_sync_crit_sect_ptr);
  rex_init_crit_sect(mmgsdi_task_state_crit_sect_ptr);

  /* --------------------------------------------------------
      INITIALIZE GLOBALS
  -------------------------------------------------------- */
  /* Initialize the MMGSDI cache */
  mmgsdi_cache_init();

  /* Initlize the MMGSDI PB cache data */
  mmgsdi_pb_init_cache_data();

  /* Initialize SE13 table variables */
  mmgsdi_se13_init();

  memset(mmgsdi_generic_data_ptr, 0x00, sizeof(mmgsdi_generic_data_type));

  memset(&mmgsdi_refresh_data_info, 0x00, sizeof(mmgsdi_refresh_data_info_type));
    /* Init the refresh mode to MMGSDI_REFRESH_MODE_ENUM_MAX */
  mmgsdi_refresh_data_info.refresh_mode = MMGSDI_REFRESH_MODE_ENUM_MAX;

  memset(mmgsdi_client_req_table_info_ptr, 0x00,
         MMGSDI_MAX_CLIENT_REQ_INFO*sizeof(mmgsdi_uim_client_req_user_data_type *));

  memset(mmgsdi_client_id_reg_table, 0x00,
         sizeof(mmgsdi_client_id_reg_table_type *) * MMGSDI_MAX_CLIENT_INFO);

  memset(mmgsdi_1x_prl, 0x00, sizeof(mmgsdi_1x_prl));

  for(i = 0; i < MMGSDI_MAX_NUM_SLOTS; i++)
  {
    mmgsdi_data_slot_ptr[i] = NULL;

    for(j = 0; j < MMGSDI_MAX_EXT_APP_INFO; j++)
    {
      mmgsdi_app_pin_table_ptr[i][j] = NULL;
    }

    for(j = 0; j < MMGSDI_MAX_PIN1_INFO; j++)
    {
      mmgsdi_app_pin1_table_ptr[i][j] = NULL;
    }

    mmgsdi_app_univ_pin_table_ptr[i] = NULL;
    mmgsdi_refresh_info_ptr[i] = NULL;
  }

#ifdef FEATURE_SIMLOCK
  memset(&mmgsdi_simlock_status_table, 0x00, sizeof(mmgsdi_simlock_status_table));
#endif /* FEATURE_SIMLOCK */

  memset(mmgsdi_channel_info_ptr_table,
         0x00,
         sizeof(mmgsdi_channel_info_ptr_table));

  for(i = 0; i < MMGSDI_MAX_AVAIL_GET_RSP_CACHE_ENTRIES; i++)
  {
    mmgsdi_apdu_get_rsp_table_ptr[i] = NULL;
  }

  (void)mmgsdi_nv_item_cache_init();

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  /* --------------------------------------------------------------------------
     We generate and add a client ID in the client registration table and
     update the global mmgsdi_generic_data_ptr here to make sure that a valid
     client id is available  (instead of default client ID (0x00)) when some
     notifications  were pushed by UIMDRV.
     Eg: When card error / link established notification is sent by UIMDRV.
     ----------------------------------------------------------------------- */
  if (mmgsdi_add_client_id_entry(&mmgsdi_generic_data_ptr->client_id,
                                 NULL, "gsdi") == MMGSDI_SUCCESS)
  {
    /* We set the flag is_client_evt_reg_cnf_sent is to TRUE here to make sure
       any card error /  card inserted events triggered by MMGSDI reach to
       itself. If we dont set this here then we might have a possible race
       condition between the slot specific task & main task and this can result
       in missing some initial events to MMGSDI */
    if(MMGSDI_SUCCESS == mmgsdi_util_get_client_id_index(
                                        mmgsdi_generic_data_ptr->client_id,
                                        &client_index))
    {
      mmgsdi_client_id_reg_table[client_index]->is_client_evt_reg_cnf_sent = TRUE;
    }
  }

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
}  /* mmgsdi_init_common */


/*===========================================================================
FUNCTION MMGSDI_INIT

DESCRIPTION
  Function called to initialize MMGSDI main task queues and global variables.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.  Function should only be called once during task initialization.
  On targets with RCInit, this function is called in RCInit context.
  Therefore, it should not include references to TCBs, NV
  operations, etc.
===========================================================================*/
void mmgsdi_init (
  void
)
{
  mmgsdi_init_common(MMGSDI_TASK_MAIN);
}  /* mmgsdi_init */


/*===========================================================================
FUNCTION MMGSDI_SLOT1_INIT

DESCRIPTION
  Function called to initialize MMGSDI slot-1 task queues

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.  Function should only be called once during task initialization.
  On targets with RCInit enabled, this function is called in RCInit context.
  Therefore, it should not include references to TCBs, NV
  operations, etc.
===========================================================================*/
void mmgsdi_slot1_init (
  void
)
{
  mmgsdi_init_common(MMGSDI_TASK_SLOT1);
} /* mmgsdi_slot1_init */


/*===========================================================================
FUNCTION MMGSDI_SLOT2_INIT

DESCRIPTION
  Function called to initialize MMGSDI slot-2 task queues

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.  Function should only be called once during task initialization.
  On targets with RCInit enabled, this function is called in RCInit context.
  Therefore, it should not include references to TCBs, NV
  operations, etc.
===========================================================================*/
void mmgsdi_slot2_init (
  void
)
{
  mmgsdi_init_common(MMGSDI_TASK_SLOT2);
} /* mmgsdi_slot2_init */


/*===========================================================================
FUNCTION MMGSDI_SLOT3_INIT

DESCRIPTION
  Function called to initialize MMGSDI slot-3 task queues

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.  Function should only be called once during task initialization.
  On targets with RCInit enabled, this function is called in RCInit context.
  Therefore, it should not include references to TCBs, NV
  operations, etc.
===========================================================================*/
void mmgsdi_slot3_init (
  void
)
{
  mmgsdi_init_common(MMGSDI_TASK_SLOT3);
} /* mmgsdi_slot3_init */


/*===========================================================================
FUNCTION MMGSDI_INIT_PRE_STARTUP

DESCRIPTION
  Function called to open base sessions and initialize QMI UIM.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.  Function should only be called once during task initialization.
===========================================================================*/
void mmgsdi_init_pre_startup(
  void
)
{
  mmgsdi_task_enum_type      mmgsdi_task    = MMGSDI_TASK_MAX;
  mmgsdi_nv_item_cache_type  mmgsdi_nv_item = {0};

  /* Do not proceed, if the current task is not the
     main MMGSDI task */
  MMGSDIUTIL_GET_CURRENT_MMGSDI_TASK(&mmgsdi_task);
  if(mmgsdi_task != MMGSDI_TASK_MAIN)
  {
    return;
  }

  /* -----------------------------------------------------------------------
     Open all GW, 1X & card sessions for this  client id. For provisioning
     sessions, it register for refresh with its  respective file list.
   ----------------------------------------------------------------------- */
  mmgsdi_open_base_sessions();

  /* Fetch App selection NV before QMI_UIM initializes */
  (void)mmgsdi_nv_get_item(NV_UIM_SELECT_DEFAULT_USIM_APP_I,&mmgsdi_nv_item, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC);

  /* On target, qmi uim registers for the QMI framework is done when qmi modem
     task notifies us that it is ready */
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
}  /* mmgsdi_init_pre_startup */


/*===========================================================================
FUNCTION MMGSDI_INIT_POST_STARTUP

DESCRIPTION
  Function called to define Necessary Timers and initialize certain services.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.  Function should only be called once during task initialization.
===========================================================================*/
void mmgsdi_init_post_startup(
  void
)
{
  mmgsdi_nv_item_cache_type                  mmgsdi_nv_data   = {0};
  mmgsdi_task_enum_type                      running_task     = MMGSDI_TASK_MAX;
  uim_hw_slot_info_type                      slot_info;
  uint8                                      i                = 0;
  mmgsdi_slot_id_enum_type                   slot_id          = MMGSDI_SLOT_AUTOMATIC;
  mmgsdi_nv_context_type                     nv_context       = MMGSDI_NV_CONTEXT_INVALID;
  mmgsdi_task_enum_type                      task_index       = MMGSDI_TASK_MAX;
#ifdef FEATURE_MODEM_CONFIG_REFRESH
  mcfg_refresh_registration_s_type           mcfg_reg         = {0};
#endif /*FEATURE_MODEM_CONFIG_REFRESH*/

  /* Do not proceed, if the current task is undetermined */
  MMGSDIUTIL_GET_CURRENT_MMGSDI_TASK(&running_task);
  if(running_task == MMGSDI_TASK_MAX)
  {
    return;
  }

  /* Sanity Timer used for the GSDI - UIM Interface */
  rex_def_timer (&mmgsdi_uim_sanity_timer[running_task], rex_self(), MMGSDI_UIM_SANITY_TIMER_EXPIRE_SIG);

  /* Timed sleep Timer used to replicate rex_sleep() functionality*/
  timer_def(&mmgsdi_timed_sleep_timer[running_task], &mmgsdi_always_on_timer_group,
            rex_self(), MMGSDI_TIMED_SLEEP_TIMER_SIG, NULL, 0);

  /* ------------------------------------------------------------------------
  ** Register for watchdog
  ** ----------------------------------------------------------------------*/

  gsdi_dog_rpt_var[running_task] = dog_hb_register_rex((rex_sigs_type) MMGSDI_RPT_TIMER_SIG);

  /* The below initializations are only applicable for main MMGSDI task */
  if(running_task != MMGSDI_TASK_MAIN)
  {
    return;
  }

  mmgsdi_silent_pin_init();

  /* Create Conf File  */
  uim_common_efs_create_conf_file();

#ifdef FEATURE_UIM_MULTI_PROFILE
  /* Initialize multi profile related global */
  mmgsdi_multi_profile_init();
#endif /* FEATURE_UIM_MULTI_PROFILE */

  mmgsdi_generic_data_ptr->num_slots = 0;
  memset(&slot_info, 0x00, sizeof(slot_info));
  slot_info = uim_get_hw_slot_info();
  for (i = 0; i < UIM_MAX_NUMBER_INSTANCES; i++)
  {
    if((slot_info.slot_status[i].slot_available) &&
       (slot_info.slot_status[i].slot_enabled))
    {
      mmgsdi_generic_data_ptr->num_slots++;
    }
  }

  /* Initialize ME based items */
  nv_context = MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC;
  (void)mmgsdi_nv_get_item(NV_ENS_ENABLED_I,&mmgsdi_nv_data,nv_context);
  (void)mmgsdi_nv_get_item(NV_UE_IMEI_I,&mmgsdi_nv_data,nv_context);
  (void)mmgsdi_nv_get_item(NV_HOMEZONE_ENABLED_I,&mmgsdi_nv_data,nv_context);
  (void)mmgsdi_nv_get_item(NV_FTM_MODE_I,&mmgsdi_nv_data,nv_context);
  (void)mmgsdi_nv_get_item(NV_ESN_ME_I,&mmgsdi_nv_data,nv_context);
  (void)mmgsdi_nv_get_item(NV_UIM_SELECT_DEFAULT_USIM_APP_I,&mmgsdi_nv_data,nv_context);
  (void)mmgsdi_nv_get_item(NV_SCM_I,&mmgsdi_nv_data,nv_context);
  (void)mmgsdi_nv_get_item(NV_MOB_CAI_REV_I,&mmgsdi_nv_data,nv_context);
  (void)mmgsdi_nv_get_item(NV_GPRS_ANITE_GCF_I,&mmgsdi_nv_data,nv_context);

  /* Read ME config NVs */
  (void)mmgsdi_get_me_config_params_from_nv();

  /* Cache the MEID value from NV memory */
#ifdef FEATURE_MMGSDI_3GPP2
  mmgsdi_cache_meid_data();
#endif /* FEATURE_MMGSDI_3GPP2 */  

  MMGSDIUTIL_ENTER_TASK_STATE_CRIT_SECT;

  /* Initialize the slot/subscription-based NV/EFS items */
  for(i = 0; i < MMGSDI_MAX_NUM_SLOTS && i < mmgsdi_generic_data_ptr->num_slots; i++)
  {
    (void)mmgsdi_util_get_slot_id(i, &slot_id);

    task_index = mmgsdi_util_get_task_from_slot(slot_id);
    if (task_index > MMGSDI_TASK_MAIN && task_index < MMGSDI_NUM_TASKS)
    {
      if (mmgsdi_task_state[task_index] == MMGSDI_TASK_INACTIVE)
      {
        continue;
      }
    
      /* Get the mmgsdi features' status via NV look up */
      mmgsdi_nv_init_features_status_nv_lookup(i);

      /* Get the external features' status via NV look up */
      mmgsdi_nv_init_external_features_status_nv_lookup(i);

      /* Refresh retry settings initialization */
      (void)mmgsdi_refresh_retry_init(i);

      /* Read the auto-onchip-activation EFS item */
      mmgsdi_nv_init_onchip_auto_activation_data(i);

      /* When CSIM is supported NVRUIM is a client of MMGSDI. This can cause a
         deadlock between NV and MMGSDI if NV via NVRUIM has a request pending to
         with MMGSDI and MMGSDI tries to read an NV item. To avoid this situation
         the NV items are read and cached early, before the session
         changed message is sent out, i.e. before NVRUIM starts queuing access
         commands to MMGSDI. These NV items are read as part of initialization
         as to avoid delaying the card inserted event. Rest of the NV items
         used by MMGSDI are read as part of power up before session changed */
    
      /* NOTE1: Some of these items might not be used in a 1x-only target but
         to simplify featurization and the caching logic all items are cached
         if CSIM is supported */
      nv_context = mmgsdi_util_get_efs_item_index_for_slot(slot_id);

      (void)mmgsdi_nv_get_item(NV_JCDMA_UIM_LOCK_I, &mmgsdi_nv_data, nv_context);
      (void)mmgsdi_nv_get_item(NV_JCDMA_RUIM_ID_I, &mmgsdi_nv_data, nv_context);
#ifdef FEATURE_3GPP_CSFB
      (void)mmgsdi_nv_get_item(NV_SERVICE_DOMAIN_PREF_I, &mmgsdi_nv_data, nv_context);
      (void)mmgsdi_nv_get_item(NV_DISABLE_CM_CALL_TYPE_I, &mmgsdi_nv_data, nv_context);
#endif/* FEATURE_3GPP_CSFB */

      mmgsdi_nv_init_ens_slot_enabled(i);

    }
  }

  MMGSDIUTIL_LEAVE_TASK_STATE_CRIT_SECT;

  /* Get the ENS sub-features status via NV look up */
  mmgsdi_nv_init_ens_sub_features_status_nv_lookup();

#ifndef FEATURE_UIM_TEST_FRAMEWORK
  /* ------------------------------------------------------------------------
    ** OBDApp init
    ** ----------------------------------------------------------------------*/
  obd_init(UIM_GSDI_TCB);

#ifdef FEATURE_UIM_SIM_INTERNAL_INTERFACE
  /* ------------------------------------------------------------------------
    ** QMI SIM internal global initalization
    ** ----------------------------------------------------------------------*/
  qmi_sim_internal_init_globals();
#endif /* FEATURE_UIM_SIM_INTERNAL_INTERFACE */

#ifdef FEATURE_UIMDIAG
  /* ------------------------------------------------------------------------
  ** Initialize UIM - DIAG Interface
  ** ----------------------------------------------------------------------*/
  uimdiag_init();
#endif /* FEATURE_UIMDIAG */
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */

#ifdef FEATURE_SIMLOCK
  /* Register for events from SIM Lock task */
  (void)simlock_register_event(SIMLOCK_REGISTER,
                               mmgsdi_simlock_evt_cb);

  /* Initalize and populate the mmgsdi_simlock_status_table with
     the activated SIM Lock categories. This will be referenced
     to determine if TMSI PLMN requires validation to protect
     against man-in-the-middle attacks.*/
  (void)simlock_get_status(mmgsdi_simlock_cb,
                           NULL);
#endif /* FEATURE_SIMLOCK */

#ifdef FEATURE_MODEM_CONFIG_REFRESH
  /* MCFG Registration */
  mcfg_reg.cb = mmgsdi_nv_refresh_mcfg_cb;
  /* RFU - defaulting it to 0*/
  mcfg_reg.group = 0;
  mcfg_reg.order = MCFG_REFRESH_ORDER_50;
  mcfg_reg.slot_mask = MCFG_REFRESH_INDEX_MASK_ALL;
  mcfg_reg.sub_mask = MCFG_REFRESH_INDEX_MASK_ALL;

  if(mcfg_refresh_register(&mcfg_reg) == FALSE)
  {
    UIM_MSG_ERR_0("MCFG Registration failed");
  }
#endif /*FEATURE_MODEM_CONFIG_REFRESH*/

#ifdef FEATURE_MMGSDI_3GPP2
  /* Initialize session based NVRUIM support */
  nvruim_session_init();
#endif /* FEATURE_MMGSDI_3GPP2 */

  /* Perform various registrations only for off-target. For on target the
     registrations are performed when the corresponding tasks are up */
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

  /* To read PSM UICC profile data from SFS, create slot, app and EFs cache
     from it and send card and session events. */
  mmgsdi_psm_initialize();

  /* Initialize the conditional logging by reading NV 73880 */
  uim_selective_logging_init();
}/* mmgsdi_init_post_startup */

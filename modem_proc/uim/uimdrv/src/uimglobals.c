/*===========================================================================

                            I N T E R N A L   U I M
                       G L O B A L   D E F I N I T I O N S

DESCRIPTION
  This contains all the global definitions for UIM

Copyright (c) 2013-2016 by QUALCOMM Technologies, Inc.
All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uimglobals.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/18/16   ssr     Remove msg for malloc failure
05/09/16   sam     Queue a open channel command internally after terminal profile
04/11/16   ks      Code changes needed for LPA support
11/23/15   ks      Recovery upon failure of close channel 
                   where ARAM app is selected
11/05/15   ks      Rel12 polling enhancement
10/16/15   sam     Added support to queue Explicit ADF selection
04/27/15   vr      Support to check the pending proactive command on card
02/28/15   ks      Explicit MF selection logic revisited
12/05/14   ks      Clearing path and POLL voting upon card insertion
11/10/14   akv     Support for user to customize LDO settings
10/14/14   av      Handle appropriately zero ICCID reported by uimdrv
09/18/14   akv     UIMDRV UT enhancements
07/25/14   ll      Fix link errors for ASSERT and ERR_FATAL
02/20/14   ks      Provision for explicit MF or ADF selction
                   And provision for inverse convention bootup
01/29/14   sj      ASSERT on Memory Leaks in UIMDRV
10/16/13   akv     Fixed KW errors
09/30/13   akv     Removal of multiple efs reads performed by uim tasks
05/13/13   nmb     Initial Revision
===========================================================================*/
#include "assert.h"                /* ASSERT utilities                     */

#include "uimglobals.h"
#include "uim_v.h"


uim_instance_global_type *uim_global_instances_ptr[UIM_MAX_INSTANCES];

/* Note - uim_shared_globals to be used only in sections of code guarded
   by critical sections.*/
uim_shared_global_type   uim_shared_globals;
uim_shared_global_type   *uim_shared_globals_ptr = &uim_shared_globals;

/*===========================================================================

FUNCTION UIM_GET_INSTANCE_PTR

DESCRIPTION
  This procedure returns a pointer to the global data structure for a given
  UIM instance.

DEPENDENCIES
  The global data structure must have been previously allocated by the
  uim_task main function through a call to uim_alloc_global_instance

RETURN VALUE
  Pointer to the instance global data

SIDE EFFECTS
  None
===========================================================================*/
uim_instance_global_type *uim_get_instance_ptr
(
  uim_instance_enum_type id
)
{
  if(id >= UIM_INSTANCE_INVALID || id < UIM_INSTANCE_1)
  {
    return NULL;
  }
  return uim_global_instances_ptr[id];
} /* uim_get_instance_ptr */

/*===========================================================================

FUNCTION UIM_INIT_COMMON_GLOBALS

DESCRIPTION
  This procedure initializes the global data structure commom
  across all UIM instances.

DEPENDENCIES
   None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void uim_init_common_globals(void)
{
  memset(uim_shared_globals_ptr, 0x00, sizeof(uim_shared_global_type));
} /* uim_init_common_globals */

/*===========================================================================

FUNCTION UIM_GET_COMMON_GLOBALS_PTR

DESCRIPTION
  This procedure returns a pointer to the global data structure commom
  across all UIM instances.

DEPENDENCIES
   uim_shared_globals to be used only in sections of code guarded
   by critical sections.

RETURN VALUE
  Pointer to the global data common across all uim instances.

SIDE EFFECTS
  None
===========================================================================*/
uim_shared_global_type *uim_get_common_globals_ptr(void)
{
  return uim_shared_globals_ptr;
} /* uim_get_common_globals_ptr */

/*===========================================================================

FUNCTION UIM_SET_INSTANCE_PTR_NULL

DESCRIPTION
  This sets the global data structure for a given  UIM instance to NULL

DEPENDENCIES
  The global data structure must have been previously allocated by the
  uim_task main function through a call to uim_alloc_global_instance

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void uim_set_instance_ptr_null
(
  uim_instance_enum_type id
)
{
   if(id >= UIM_INSTANCE_INVALID || id < UIM_INSTANCE_1)
  {
    return;
  }
  uim_global_instances_ptr[id] = NULL;
} /* uim_set_instance_ptr_null */


/*===========================================================================

FUNCTION UIM_ALLOC_GLOBAL_INSTANCE

DESCRIPTION
  This procedure allocates the memory of the global data structure for
  a given UIM instance.

DEPENDENCIES
  This function should be called only once for each task very early in the
  task main function

RETURN VALUE
  None.

SIDE EFFECTS
  Internally sets global uim_instances_ptr to the newly allocated memory
===========================================================================*/
void uim_alloc_global_instance
(
  uim_instance_enum_type id
)
{
  ASSERT(id < UIM_INSTANCE_INVALID);

  uim_global_instances_ptr[id] = uim_malloc(sizeof(uim_instance_global_type));
  if (NULL != uim_global_instances_ptr[id])
  {
    memset(uim_global_instances_ptr[id], 0x00, sizeof(uim_instance_global_type));
    uim_global_instances_ptr[id]->id = id;
  }
} /* uim_alloc_global_instance */


/*===========================================================================

FUNCTION UIM_RESET_GLOBALS

DESCRIPTION
  This function resets necessary UIM globals if UIM encounters an
  unexpected error during power up or an internal wake up and chooses to
  notify its clients by calling uim_notify_error.
  It is also called in the event of a card removal if hotswap is enabled.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_reset_globals
(
  uim_instance_global_type *uim_ptr
)
{
  /* Flag to indicate if the driver has encountered an overrun error */
  uim_ptr->flag.overrun_error          = FALSE;
  /* Flag to indicate that un-known status words have been received */
  uim_ptr->flag.bad_status_words_error = FALSE;
  /* Flag to indicate if the driver has encountered too many overrun errors */
  uim_ptr->flag.max_overrun_error      = FALSE;
  /* Flag to indicate if the driver has encountered too many rx break errors */
  uim_ptr->flag.max_rx_break_error     = FALSE;
  /* Flag to indicate if the driver has maxed the parity error count */
  uim_ptr->flag.max_parity_error       = FALSE;

  uim_ptr->flag.recovery_triggered     = FALSE;
  uim_ptr->flag.interface_gone_bad     = FALSE;
  uim_ptr->flag.fetch_pending          = FALSE;

  /* Reset the counters */
  uim_ptr->debug.convention_change_count = 1;

  /* The power up mode will be set after parsing the historical characters
     from the ATR */
  /* Set the count of technical problems to zero */
  uim_ptr->card_cmd.inc_tech_prob_count    = 0;
  uim_ptr->state.mf_selection.required     = 0;
  uim_ptr->state.mf_selection.channel      = 0;
  uim_ptr->state.last_selected.required    = FALSE;
  uim_ptr->state.last_selected.channel     = 0;

  uim_ptr->state.adf_selection.required     = 0;
  uim_ptr->state.adf_selection.channel      = 0;

  uim_ptr->state.aram_app.select_aram_failed = FALSE;
  uim_ptr->state.aram_app.channel            = 0xFF;

  uim_ptr->card_cmd.instrn_toggle_count        = 1;
  uim_ptr->flag.toggle_instrn_class            = FALSE;

/* reset the voltage_class_known_from_atr varialbe */
  uim_ptr->flag.voltage_class_known_from_atr   = FALSE;

  if (uim_ptr->hardware.uim_ldo.customize_ldo_setting &&
      uim_ptr->hardware.uim_ldo.desired_voltage_class != UIM_VOLTAGE_CLASS_INVALID)
  {
    /* Set the UIM interface voltage to desired voltage class */
    uim_ptr->state.current_voltage_class = uim_ptr->hardware.uim_ldo.desired_voltage_class;
  }
  else
  {
    /* Set the UIM interface voltage to the minimum voltage class */
    uim_ptr->state.current_voltage_class = UIM_VOLTAGE_CLASS_C;
  }

  uim_ptr->state.WI                            = 10;

  /* Indicate that we did not receive the ATR */
  uim_ptr->atr.atr_received                    = FALSE;
  uim_ptr->command.mode                        = UIM_NORMAL_MODE;
   /* Reset the flag */
   uim_ptr->flag.poll_pending                  = FALSE;
  (void) uim_clear_poll_timer(uim_ptr);
  uim_ptr->state.dfs_1.cdma_df_present         = FALSE;
  uim_ptr->state.dfs_1.gsm_df_present          = FALSE;
  uim_ptr->state.dfs_1.protocol                = UIM_NO_SUCH_MODE;

  /* Clear pending UIM Command and response signal */
  (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_CMD_RSP_SIG );
  (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_CMD_RSP_TIMEOUT_SIG );
  /* clear the signal as well just in case if it was set */
  (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_TRANSACTION_SIG );
  /* Re-setting to NULL so that in erroneous condition call back is not called */
  uim_ptr->silent_recover.user_data = NULL;
  uim_ptr->silent_recover.uim_silent_recovery_callback = NULL;

  /* Reset the iccid cache in case the next card does not have an ICCID */
  memset(&uim_ptr->cache.iccid_buf, 0x00, sizeof(uim_iccid_buf_type));
  
  uim_ptr->state.ctl =
   (uim_voter_type) ((int)UIM_NONE
                   | (int)UIM_MC
                   | (int)UIM_AUTH
                   | (int)UIM_UICC_SESSION
                     );
  uim_ptr->poll.is_proactive_polling_enabled = TRUE;
  uim_ptr->poll.presence_detection_state     = UIM_PD_POLLING_OFF;
  uim_ptr->flag.is_euicc                     = FALSE; 
  uim_reset_dir(uim_ptr);

  uim_ptr->state.channel_info.manage_channel_allowed = TRUE;
  uim_ptr->state.channel_info.unallocated_channel    = 0xFF;
} /* uim_reset_globals */

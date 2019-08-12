/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


              U I M   U T I L   S T A T E   M A C H I N E

GENERAL DESCRIPTION
  This module contains the state machine for the UIM UTILS of the UIM
  interface.

EXTERNALIZED FUNCTIONS

  uim_process_util_command
    Processes a generic UIM server command

  uim_send_util_err_rpt
    Processes an error condition for a util UIM server command

  uim_util_command_response
    This function processes the UIM responses from cdma APDUs.

INITIALIZATION REQUIREMENTS

Copyright (c) 2003,2009-2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uimutil.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/16   dd      F3 Optimization on curr path
05/23/16   na      F3 Optimization
05/18/16   ssr     Remove msg for malloc failure
05/04/16   lm      Iccid length is being filled from file size received in FCP of EF-ICCID
01/25/16   ssr     Delay local card power up in Modem SSR in BTSAP mode
11/11/15   sam     Explicit MF selection after call connect envelope command
10/16/15   sam     Added util function to check if a given AID is ARA-M aid
07/15/15   ks      Consolidating the F3s that print the directory path
06/03/15   ks      Select MF after EF-PSLOCI update for a buggy card
03/26/15   hyo     Select MF after EF-LOCI update for a buggy card
09/18/14   akv     UIMDRV UT enhancements
08/06/14   js      Handle SW 6280, 63C1 for non-telecom ADFs for activate ADF
07/03/14   sam     Explicit MF selection after location status envelop when 
                   NV 'UIMDRV_FEATURE_ENABLE_EXPLICIT_SELECTION_OF_MF_OR_ADF' is enabled
06/09/14   ll      Add interruptible time delay and return value for update params
06/04/14   na/js   Handling of case 4 commands for non telecom apps
03/21/14   ll      Remove return_ptr and remove/change uim_free to UIM_FREE
03/05/14   sam     Fix to pet dog during power up wait/delay
03/03/14   nmb     Directory maintenance I&T bug fixes
02/25/14   am      Fix compile error due to "stringl.h" inclusion
02/20/14   ak      Fix compile errors due to strict compiler on 9x35
01/29/14   sam     Usage of newer style diag macros
01/14/14   nmb     Move MANAGE CHANNEL command to generic cmd processing
01/14/14   nmb     Remove ACCESS_F and related unused functionality
12/18/13   sam     uim_util_get_hotswap_enabled_slot function is
                   wrongly returning false eventhough hotswap is enabled
10/15/13   sam     Removed include file virtual_uimdrv.h -> file moved to
                   offtarget location
10/03/13   na      F3 log optimization
09/26/13   ak      Remove unnecessary feature flags
09/20/13   ak      Hotswap debug message cleanup
09/16/13   yk      Removed the code for old targets and for old features
08/21/13   hyo/js  Hotswap support on MOB
08/13/31   js      UIMDRV Memory De-Couple changes
08/08/13   ks      Advanced Subscription Manager changes
07/12/13   nmb     Global Reorganization
06/24/13   js      TSTS changes
06/17/13   ks      Removal of F3 messages as part of memory optimization
05/16/13   na      Secure memory copy
02/12/13   js      General F3 macro cleanup
12/05/12   js      UIM Parallel Processing changes
10/26/12   akv     HW Enumeration changes (without features)
10/26/12   akv     HW Enumeration changes
08/09/12   js/rm   Fixes for UIM controller hotswap and hardware WWT features
05/25/12   js      Handle 62 00 SW with correct transaction status
05/14/12   js      Handle 62 00 SW for MANAGE CHANNEL as UIM_DONE
05/01/12   av      Replaced FEATURE_UIM_SUPPORT_HOTSWAP with an nvitem
03/20/12   rm      Changes for WWT and hotswap support
03/20/12   js      Correct P2 param for MANAGE CHANNEL (Open)
03/15/12   rm      Changes for DIME
02/21/12   rm/ak   Setting card detection polarity based on NV item
02/29/12   ak      Fix a klocwork warning
02/18/12   av      Cleaned up remaining ALWAYS ON features
01/20/12   ak      Platform ID/NV item logic for hotswap polarity
12/30/11   ssr     Manage channel for ICC mode
                   1) Fixed manage channel command response for empty response
                      and P2 data, where P2 contains the channel number
                   2) We should not allow Manage channel command in ICC mode
                   3) Reset the channel number to default channel whenever,
                      UIM is resetting the protocol to the ICC mode
12/14/11   rm      Fix to apply card detection polarity for card detection
                   interrupts as well
11/09/11   ms      Fixed compilation warning
09/09/11   ssr     Fix to allow the reset command when switch is un-pressed where
                   client want to disable the hotswap interrupt at runtime,
                   Added new API to return the real time card status,
                   Added new API to disable the hotswap interrupt
09/01/11   ak      GPIO hotswap support for 9x15
08/30/11   yb      Added support for complete recovery solution
                   Added support for caching channel information slot wise
                   Fixed crash on card removal and inserting new card
08/11/11   nmb     Expanded Logical Channels
07/08/11   rm      Catch-up with latest on Hot-swap
04/25/11   yb      Adding support for hotswap
01/17/11   yb      Fix for open channel issue
11/10/10   yt      Memory reduction changes
09/01/10   ps      Merge UIM server DSDS changes
09/26/09   kp      ZI memory reduction changes
05/02/09   ps      Fix 8200 Warnings
12/17/07   rm      Fix to correct the instruction case in some situations
12/11/06   tml     Clear the uim_last_sel_dir for the specific channel and slot
                   upon close channel request
04/03/06   pv      Lint Fixes
03/08/06   pv      Load the status words to the report buffer even if we
                   errored out because of un-known status words
11/14/05   pv      Lint Fixes
08/08/04   tml     Added user data
07/28/05   pv      replaced UIM_MAX_NUM_CDD_ATTEMPTS with UIM_MAX_NUM_ATTEMPTS
                   where not necessary.
07/12/05   tml     Assigned user data in util error
06/08/05   wli     Replaced UIM_MAX_NUM_ATTEMPTS with UIM_MAX_NUM_CMD_ATTEMPTS
           for command retry.
03/07/05   jk      Set the cmd count to max to avoid sending of failed PIN
                   related APDUs (so that PINs don't get blocked)
05/17/04   ck      Removed the use of logical channel table and instead use the
                   channel information from the command.
01/12/04   ck      Added a header file outside the feature to avoid warning.
12/29/03   ck      Merged in the inteface changes in the data direction
                   parameter of the command to the driver.
02/10/03   jk      Created module.  This module contains the UTIL state
                   machine.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"
#include "comdef.h"
#include "bit.h"
#include "uimdrv_msg.h"
#include "err.h"
#include "uimdrv.h"
#include "uimdrv_main.h"
#include "uimdrv_uartdm.h"
#include "uimi.h"
#include "uimutil.h"
#include <memory.h>
#include "uimgen.h"
#include "fs_public.h"
#include "DDIPlatformInfo.h"
#include <stringl/stringl.h>
#include "uimglobals.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*===========================================================================
  I N T E R N A L   F U N C T I O N   P R O T O T Y P E S
===========================================================================*/

/*===========================================================================

FUNCTION UIM_UTIL_GET_UIM_CLASS

DESCRIPTION
  Returns the appropriate standard or extended logical channel class byte.

DEPENDENCIES
  None

RETURN VALUE
  Class byte

SIDE EFFECTS
  None

===========================================================================*/
byte uim_util_get_uim_class
(
  byte             uim_ins_class,
  uim_channel_type channel_id
)
{
  if(channel_id < UIM_FIRST_EXTENDED_CHANNEL)
  {
    return (byte)(uim_ins_class | channel_id);
  }
  else
  {
    return (byte)((channel_id - UIM_FIRST_EXTENDED_CHANNEL)
                  | uim_ins_class | UIM_UICC_EXTENDED_LC_CLASS);
  }
}/* uim_util_get_uim_class */


/*===========================================================================
FUNCTION UIM_MALLOC

DESCRIPTION
  The uim_malloc called TMC alloc with different heap_size based the size
  of the allocation required.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  a pointer is allocated.
===========================================================================*/
void * uim_malloc(dword size)
{
  void *return_mem = NULL;
  return_mem = modem_mem_calloc(1,(size),MODEM_MEM_CLIENT_UIM);
  return return_mem;
}/* uim_malloc */


/*===========================================================================

FUNCTION UIM_MEMSCPY

DESCRIPTION
  This function provides the wrapper to memscpy, which is a secure version of
  memory copy
  dst_ptr is the destination buffer in which contents of src_ptr will be copied.
  dst_len represnts the length/dimension of the dst_ptr while src_len then length
  of contents to be copied into dst_ptr.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_memscpy
(
  void       *dst_ptr,
  size_t      dst_len,
  const void *src_ptr,
  size_t      src_len
)
{
  (void)memscpy(dst_ptr,
                dst_len,
                src_ptr,
                src_len);
} /* uim_memscpy */


/*===========================================================================

FUNCTION UIM_FORMAT_CHV

DESCRIPTION
  This procedure memsets the trailing characters in the CHV buffer with pad
  character (0xFF)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Changes the value of the CHV passed in as a parameter.

===========================================================================*/
void uim_format_chv
(
  byte                     *chv,   /* This points to the CHV buffer that holds the CHV digits.
                                      The digits in this buffer are formatted  */
  uim_instance_global_type *uim_ptr
)
{
  /* Index for processing the CHV buffer */
  byte i = 0;
  /* This holds the index of the last CHV digit */
  byte j = UIM_MAX_CHV_DIGITS;

  if(chv == NULL)
  {
    return;
  }

  /* Look for the end of the CHV digits. */
  for (i = 0; i < UIM_MAX_CHV_DIGITS; i++)
  {
     if (chv[i] == '\0')
     {
       /* This is past the last byte of the CHV digits. */
       /* Start adding pad characters after the CHV digits. */
       j = i;
       break;
     }
  }

  /* Fill the buffer with pad characters */
  for (i = j; i < UIM_MAX_CHV_DIGITS; i++)
  {
    chv[i] = UIM_CHV_PAD_CHAR;
  }
} /* uim_format_chv */


/*========================================================================

FUNCTION UIM_PROCESSING_STREAM_APDU

DESCRIPTION
  This function returns whether UIM is processing a stream apdu command.

DEPENDENCIES
  None

RETURN VALUE
  TRUE : UIM processing STREAM APDU COMMAND
  FALSE: UIM not processing STREAM APDU COMMAND

SIDE EFFECTS
  None

===========================================================================*/
boolean uim_processing_stream_apdu
(
  uim_instance_global_type *uim_ptr
)
{
  /* Check if we are processing ENVELOPE command */
  if( (UIM_STREAM_APDU_ST   == *uim_ptr->command.generic_state_ptr  ||
       UIM_STREAM_SELECT_ST == *uim_ptr->command.generic_state_ptr) &&
       UIM_GENERIC_CMD      ==  uim_ptr->state.cmd_mode)
  {
    return TRUE;
  }

  return FALSE;
} /* uim_processing_stream_apdu */


/*===========================================================================

FUNCTION UIM_IS_MORE_TIME_PCMD_FETCHED

DESCRIPTION
  This procedure tells whether the fetched command is MORE TIME or not

DEPENDENCIES
  None

RETURN VALUE
  TRUE - if the data buffer contains MORE TIME proactive command
  FALSE - if the data buffer does not contain MORE TIME proactive cmd

SIDE EFFECTS
  None

===========================================================================*/
boolean uim_is_more_time_pcmd_fetched(
  const uint8    *data
)
{
  boolean ret_status = FALSE;
  if ( (NULL !=data) && *data == UIM_PROACTIVE_CMD_TAG )
  {
    /* if length encoding is 1 */
    if ( (*(data + 1) != 0x81 && *(data + 5) == PROA_MORE_TIME) ||
         (*(data + 1) == 0x81 && *(data + 6) == PROA_MORE_TIME) )
    {
      UIM_MSG_MED_1 ("Card requested for MORE TIME 0x%x",(*(data + 1)));
      ret_status = TRUE;
    }
  }
  return (ret_status);
} /* uim_is_more_time_pcmd_fetched */


/*===========================================================================

FUNCTION uim_is_location_status_envelope

DESCRIPTION
The function verifies if the current envelop is location status envwlop or not
DEPENDENCIES
  None

RETURN VALUE
  TRUE -- if location status envelop
  FALSE -- if not a location status envelop

SIDE EFFECTS
 NONE
===========================================================================*/

boolean uim_is_location_status_envelope
(
  const uint8    *data
)
{
  if ( (NULL !=data) && *data == UIM_EVT_DOWNLOAD_ENVELOPE)
  {
    if((*(data + 2) == UIM_ENVELOPE_EVENT_TAG)&& 
	(*(data + 3) == 0x01)  && 
	(*(data + 4) == UIM_LOCATION_STATUS_EVT))
    {
      UIM_MSG_MED_0("Curent envelop recieved is a location status envelope");
      return TRUE;
    }
  }
  
  return FALSE;
} /* uim_is_more_time_pcmd_fetched() */


/*===========================================================================

FUNCTION uim_is_call_connect_envelope

DESCRIPTION
The function verifies if the current envelop is call connect evt envwlop or not
DEPENDENCIES
  None

RETURN VALUE
  TRUE -- if location status envelop
  FALSE -- if not a location status envelop

SIDE EFFECTS
 NONE
===========================================================================*/
boolean uim_is_call_connect_envelope
(
  const uint8    *data
)
{
  if ( (NULL !=data) && *data == UIM_EVT_DOWNLOAD_ENVELOPE)
  {
    if((*(data + 2) == UIM_ENVELOPE_EVENT_TAG)&& 
	(*(data + 3) == 0x01)  && 
	(*(data + 4) == 0x01))
    {
      UIM_MSG_MED_0("Curent envelop recieved is a call connect envelope");
      return TRUE;
    }
  }
  
  return FALSE;
} /* uim_is_call_connect_envelope() */


/*===========================================================================

FUNCTION uim_is_mf_selection_needed_after_update_bin_command

DESCRIPTION
The function verifies if the current update operation is on location status EF
DEPENDENCIES
  None

RETURN VALUE
  TRUE -- if location status EF or EF PSLOCI is the one to be updated
  FALSE --if location status EF or EF PSLOCI is not the one to be updated

SIDE EFFECTS
 NONE
===========================================================================*/
boolean uim_is_mf_selection_needed_after_update_bin_command
(
  uim_path_type *cmd_path_ptr

)
{
  uim_path_type ef_loci_path   = {0x3, {0x3F00,0x7FFF,0x6F7E}};
  uim_path_type ef_psloci_path = {0x3, {0x3F00,0x7FFF,0x6F73}};

  boolean       is_mf_selection_needed = FALSE;
  
  if ( (0 == memcmp(cmd_path_ptr->path, ef_loci_path.path, ef_loci_path.len * 2)) ||
       (0 == memcmp(cmd_path_ptr->path, ef_psloci_path.path, ef_psloci_path.len * 2)) )
  {
    is_mf_selection_needed = TRUE;
  }
  UIM_MSG_HIGH_1("is_mf_selection_needed 0x%x ", is_mf_selection_needed);
  return (is_mf_selection_needed);
} /* uim_is_location_status_update_command() */


/*===========================================================================

FUNCTION uim_clk_busy_wait

DESCRIPTION
  This function is a wrapper around clk_busy_wait to avoid data conversion
  issues for different targets and to validate the supplied values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_clk_busy_wait(dword busy_wait_time)
{
  if ( (int)busy_wait_time > UIM_MAX_CLK_BUSY_WAIT )
  {
    busy_wait_time = UIM_MAX_CLK_BUSY_WAIT;
  }

  /*lint -e{732} suppress "loss of sign" since different targets
   *have different signature for clk_busy_wait.  This should be
   *safe after the above check
   */
  DALSYS_BusyWait( (int) busy_wait_time );
} /* uim_clk_busy_wait */


/*===========================================================================

FUNCTION uim_timed_sleep_ext

DESCRIPTION
  This function is a wrapper around rex_timed_wait() to replace rex_sleep()
  with a non-deferrable timer with additional signals to wait for if needed.

DEPENDENCIES
  None

RETURN VALUE
  Return the signals 

SIDE EFFECTS
  None

===========================================================================*/
rex_sigs_type uim_timed_sleep_ext
(
  unsigned long             ms,
  uim_instance_global_type *uim_ptr,
  rex_sigs_type             additional_sigs_to_wait
)
{
#ifndef FEATURE_UIM_TEST_FRAMEWORK
  /* UIM timed sleep timer to replace rex_sleep */
  static rex_timer_type uim_timed_sleep_ext_timer[UIM_MAX_INSTANCES];
  static rex_timer_type *uim_timed_sleep_ext_timer_ptr[UIM_MAX_INSTANCES];
  rex_sigs_type  recved_singal;

  if(uim_timed_sleep_ext_timer_ptr[uim_ptr->id] == NULL)
  {
    timer_def( &uim_timed_sleep_ext_timer[uim_ptr->id], &uim_ptr->task.always_on_timer_group,
               uim_ptr->tcb_ptr, UIM_TIMED_SLEEP_TIMER_SIG,NULL, 0);
    uim_timed_sleep_ext_timer_ptr[uim_ptr->id] = &uim_timed_sleep_ext_timer[uim_ptr->id];

  }
  (void)rex_set_timer(&uim_timed_sleep_ext_timer[uim_ptr->id], ms );
  recved_singal = uim_pet_and_wait((UIM_TIMED_SLEEP_TIMER_SIG|additional_sigs_to_wait), uim_ptr);
  (void)rex_clr_timer(&uim_timed_sleep_ext_timer[uim_ptr->id]);
  (void)rex_clr_sigs(uim_ptr->tcb_ptr, UIM_TIMED_SLEEP_TIMER_SIG);
  return recved_singal;
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
} /* uim_timed_sleep_ext */


/*===========================================================================

FUNCTION uim_timed_sleep

DESCRIPTION
  This function is a wrapper around rex_timed_wait() to replace rex_sleep()
  with a non-deferrable timer with additional signals to wait for if needed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_timed_sleep
(
  unsigned long             ms,
  uim_instance_global_type *uim_ptr,
  rex_sigs_type             additional_sigs_to_wait
)
{
#ifndef FEATURE_UIM_TEST_FRAMEWORK
  /* UIM timed sleep timer to replace rex_sleep */
  static rex_timer_type uim_timed_sleep_timer[UIM_MAX_INSTANCES];
  static rex_timer_type *uim_timed_sleep_timer_ptr[UIM_MAX_INSTANCES];

  RETURN_IF_INSTANCE_INVALID(uim_ptr->id);

  if(uim_timed_sleep_timer_ptr[uim_ptr->id] == NULL)
  {
    timer_def( &uim_timed_sleep_timer[uim_ptr->id], &uim_ptr->task.always_on_timer_group,
               uim_ptr->tcb_ptr, UIM_TIMED_SLEEP_TIMER_SIG,NULL, 0);
    uim_timed_sleep_timer_ptr[uim_ptr->id] = &uim_timed_sleep_timer[uim_ptr->id];

  }
  (void)rex_set_timer(&uim_timed_sleep_timer[uim_ptr->id], ms );
  (void)uim_pet_and_wait((UIM_TIMED_SLEEP_TIMER_SIG|additional_sigs_to_wait), uim_ptr);
  (void)rex_clr_timer(&uim_timed_sleep_timer[uim_ptr->id]);
  (void)rex_clr_sigs(uim_ptr->tcb_ptr, UIM_TIMED_SLEEP_TIMER_SIG);
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
} /* uim_timed_sleep */


/*===========================================================================
FUNCTION UIM_MAX_CH_NUM_OPENED

DESCRIPTION
  This function returns the value of the maximum channel number which was
  opened before recovery

PARAMETER
  None

DEPENDENCIES
  uim_is_ch_open

RETURN VALUE
  uim_channel_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
uim_channel_type uim_max_ch_num_opened
(
  uim_instance_global_type *uim_ptr
)
{
  uim_channel_type ch=0;
  for(ch=UIM_MAX_CHANNELS-1; ch>0; ch--)
  {
    if(uim_ptr->state.is_ch_open[ch]!= FALSE)
    {
      return (ch);
    }
  }
  return 0;
} /* uim_max_ch_num_opened */


/*===========================================================================

FUNCTION UIM_START_POWER_SAVE

DESCRIPTION
  For USB cards this function informs the USB driver that the USB interface is
  not currently needed for any ICCD transaction and hence it can be suspended.
  For legacy ISO cards this function processes a request to turn the UIM clock
  off. It uses a global control variable to determine how to control the clock.

DEPENDENCIES
  For legacy ISO cards this function uses a global variable to determine
  how to control the UIM clock.

RETURN VALUE
  TRUE/FALSE.

SIDE EFFECTS
  For USB cards if all functional interfaces over the USB interface have voted
  for a suspend, the USB interface to the card will be suspended.
  For legacy ISO cards the UIM clock will either be left high or low or will be
   left on, depending on the preferences of the UIM card.

===========================================================================*/
boolean uim_start_power_save
(
  uim_instance_global_type *uim_ptr
)
{
  dword guardtime_wait_value = 0;
  /* Keep a local buffer for controlling the clock. */
  uim_op_params_type local_uim_op_params_buf;

  /* Return status */
  boolean status = TRUE;

  /* First, wait for the guardtime to pass.  This is 2 etus */
  if((uim_ptr->state.op_params_buf.FI < UIM_CRCF_SIZE) &&
     (uim_ptr->state.op_params_buf.DI < UIM_BRAF_SIZE))
  {
    /* No wait needed when card is not available */
    if(braf_values[uim_ptr->state.op_params_buf.DI] !=0)
    {
      guardtime_wait_value = ((crcf_values[uim_ptr->state.op_params_buf.FI]*1000000)/
                            (uim_hw_if.clkreg[uim_ptr->id].m_simClkFreq *
                             braf_values[uim_ptr->state.op_params_buf.DI]));
    }
  }
  else
  {
    UIMDRV_MSG_MED_0(uim_ptr->id, "Attempt to access array out of bounds");
    return FALSE;
  }
  if(!uim_ptr->flag.interface_gone_bad)
  {
    /* No wait needed when card is not available */
    if(braf_values[uim_ptr->state.op_params_buf.DI] !=0)
    {
      uim_clk_busy_wait (guardtime_wait_value);
      uim_clk_busy_wait (guardtime_wait_value);
    }
    /* Make sure to wait long enough after the last transaction to
    turn off the clock. */
    uim_clk_busy_wait ((1860*1000000)/uim_hw_if.clkreg[uim_ptr->id].m_simClkFreq);
  }

  if((uim_ptr->command.mode == UIM_PASSIVE_MODE) &&
     (uim_ptr->flag.passive_clk_stop_allowed == FALSE))
  {
    /* Storing the UIM clock preferences. Preferences could be
       CLOCK_STOP_HIGH,CLOCK_STOP_LOW and CLOCK_STOP_NOT_ALLOWED
     */
    uim_ptr->state.clock_stop_cntrl_passive_mode = uim_ptr->state.clock_stop_cntrl;
    uim_ptr->state.clock_stop_cntrl = UIM_CLOCK_STOP_NOT_ALLOWED;
    uim_ptr->flag.passive_clk_stop_allowed = TRUE;
  }
  else if((UIM_PASSIVE_MODE == uim_ptr->command.mode) &&
          (TRUE == uim_ptr->flag.passive_clk_stop_allowed))
  {
     /* Retriveing the stored value of CLOCK STOP preferences */
     uim_ptr->state.clock_stop_cntrl = uim_ptr->state.clock_stop_cntrl_passive_mode;
  }

  /* Determine what to do about stopping the UIM clock. */
  switch (uim_ptr->state.clock_stop_cntrl)
  {
    case UIM_CLOCK_STOP_HIGH:
      {
        /* Stop the clock at a high state */
        local_uim_op_params_buf.change_baud_rate = FALSE;
        local_uim_op_params_buf.change_clk_freq = TRUE;
        local_uim_op_params_buf.change_guardtime = FALSE;
        /* Get the last clock frequency used for the UIM. */
        local_uim_op_params_buf.clock_frequency = UIMDRV_CLK_STOP_HIGH;
        /* Program the clock frequency */
        status = uim_update_op_params( &local_uim_op_params_buf, uim_ptr);
      } /* end case - stop the clock at a high state */
      break;

    case UIM_CLOCK_STOP_LOW:
      {
        /* Stop the clock at a low state */
        local_uim_op_params_buf.change_baud_rate = FALSE;
        local_uim_op_params_buf.change_clk_freq = TRUE;
        local_uim_op_params_buf.change_guardtime = FALSE;
        /* Get the last clock frequency used for the UIM. */
        local_uim_op_params_buf.clock_frequency = UIMDRV_CLK_STOP_LOW;
        /* Program the clock frequency */
        status = uim_update_op_params( &local_uim_op_params_buf, uim_ptr);
      } /* end case - stop the clock at a low state */
      break;

    default:
      {
        /* Do not stop the clock as the card has given a preference to keep it
           on or the control variable is out of range. */
        status = FALSE;
      } /* end case - do not stop the clock */
      break;

  } /* end switch - check clock control */

  return ( status );
} /* uim_start_power_save */


/*===========================================================================

FUNCTION UIM_STOP_POWER_SAVE

DESCRIPTION
  For USB cards this function sends a request to the USB driver to resume
  the USB interface as we need to communicate with the card over the ICCD
  functional interface.
  For legacy ISO cards this function processes a request to turn the UIM clock
  back on. It uses a global control variable to determine how to control the
  clock. It also uses a global variable to determine what frequency to set
  the clock.

DEPENDENCIES
  For legacy ISO cards this function uses a global variable to determine how
  to control the UIM clock and another global variable to determine the
  clock frequency.

RETURN VALUE
  None

SIDE EFFECTS
  For USB cards the USB driver will be requested to resume the interface.
  For ISO cards the UIM clock will be turned on and set to a frequency
  previously determined.

===========================================================================*/
void uim_stop_power_save
(
  uim_instance_global_type *uim_ptr
)
{
  /* Keep a local buffer for controlling the clock. */
  uim_op_params_type local_uim_op_params_buf;

  /* Determine what to do about stopping the UIM clock. */
  switch (uim_ptr->state.clock_stop_cntrl)
  {
    case UIM_CLOCK_STOP_HIGH:
    case UIM_CLOCK_STOP_LOW:
      {
        /* Turn the clock back on */
        local_uim_op_params_buf.change_baud_rate = FALSE;
        local_uim_op_params_buf.change_clk_freq = TRUE;
        local_uim_op_params_buf.change_guardtime = FALSE;
        /* Get the last clock frequency used for the UIM. */
        local_uim_op_params_buf.clock_frequency =
          uim_ptr->state.op_params_buf.clock_frequency;
        /* Program the clock frequency */
        uim_update_op_params( &local_uim_op_params_buf, uim_ptr);
        /* Make sure to wait long enough after turning the clock back on
           before processing the next transaction. */
        uim_clk_busy_wait ((700*1000000)/uim_hw_if.clkreg[uim_ptr->id].m_simClkFreq);

      } /* end case - stop the clock at a low state */
      break;

    default:
      {
        /* Do not turn the clock back on as the clock was not turned off
           from before. */
      } /* end case - do not stop the clock */
      break;

  } /* end switch - check clock control */
} /* uim_stop_power_save */


/*===========================================================================

FUNCTION UIM_PRINT_CURR_PATH

DESCRIPTION
  This procedure prints the currently selected path

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_print_curr_path
(
 const uim_path_type      *path,
 uim_instance_global_type *uim_ptr
)
{
  uim_path_type temp_path;
  if(path->len == 0)
  { 
    return;
  }
  memset(&temp_path, 0, sizeof(uim_path_type));
  temp_path.len = path->len;
  uim_memscpy(temp_path.path,sizeof(temp_path.path),path->path, 2*(path->len));
  UIMDRV_MSG_HIGH_6(uim_ptr->id, "Path length is 0x%x Path 0x%X 0x%X 0x%X 0x%X 0x%X",
  	                temp_path.len,
                        temp_path.path[0], 
		        temp_path.path[1],
                        temp_path.path[2],
                        temp_path.path[3],
                        temp_path.path[4]);
} /* uim_print_curr_path */

/*===========================================================================

FUNCTION UIM_PRINT_CURR_PATH_EXT

DESCRIPTION
  This procedure prints the currently selected path only if it is changed from
  previous stored value.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_print_curr_path_ext
(
  const uim_path_type      *path,
  uim_instance_global_type *uim_ptr
)
{
  static uim_path_type	temp_path = {0, {0,0,0,0,0}};
 
  if((path->len == 0) ||
     ((path->len == temp_path.len) && 
      (0 == memcmp(path->path, temp_path.path, (temp_path.len * 2)))))
  { 
    return;
  }

  memset(&temp_path, 0, sizeof(uim_path_type));
  temp_path.len = path->len;
  uim_memscpy(temp_path.path,sizeof(temp_path.path),path->path, 2*(path->len));
  UIMDRV_MSG_HIGH_6(uim_ptr->id, "Path length is 0x%x Path 0x%X 0x%X 0x%X 0x%X 0x%X",
  	                temp_path.len,
                        temp_path.path[0], 
		        temp_path.path[1],
                        temp_path.path[2],
                        temp_path.path[3],
                        temp_path.path[4]);
  
  return;
} /* uim_print_curr_path_ext */

/*===========================================================================
FUNCTION UIM_IS_TELECOM_AID

DESCRIPTION
 This function is used to determine apps based on AID AID could belong to
 TELECOM as well as NON-TELECOM.For NON-TELECOM app,GET RESPONSE will not be
 issued upon receipt of sw1 as 0x62 and sw2 as 0x83

DEPENDENCIES
  None

RETURN VALUE
  TRUE   if AID is TELECOM or non-ADF type file is present, also default return value.
  FALSE  if AID is NON-TELECOM.

SIDE EFFECTS
  None

===========================================================================*/
boolean uim_is_telecom_aid
(
  uim_aid_type aid
)
{
  byte telecom_aid[UIM_TELECOM_AID_NUM][UIM_AID_RID_LEN] =
  {
    /**
    * 'A000000087' for the 3GPP
    * 'A000000343' for the 3GPP2.
    */
    {0xA0, 0x00, 0x00, 0x00, 0x87},
    {0xA0, 0x00, 0x00, 0x03, 0x43}
  };
  uint8  i = 0;

  /* Input param check */

  if (aid.aid_length == 0 || aid.aid_length >= UIM_AID_MAX_NAME_LENGTH )
  {
    return TRUE;
  }

  for(i = 0; i < UIM_TELECOM_AID_NUM; i++ )
  {
    if(0 == memcmp(aid.data,telecom_aid[i],(UIM_AID_RID_LEN )))
    {
      return TRUE;
    }
  }
  UIM_MSG_MED_0("NON-TELECOM AID found");
  return FALSE;
}/* uim_is_telecom_aid */


/*===========================================================================
FUNCTION UIM_IS_ARAM_AID

DESCRIPTION
 This function is used to determine is the AID is an ARA-M app AID

DEPENDENCIES
  None

RETURN VALUE
  TRUE   if AID is ARA-M AID.
  FALSE  if AID is not ARA-M AID.

SIDE EFFECTS
  None

===========================================================================*/
boolean uim_is_aram_aid
(
  uim_aid_type aid
)
{
  byte aram_aid[9] = {0xA0, 0x00, 0x00, 0x01, 0x51, 0x41, 0x43, 0x4C, 0x00};

  if( (0 != aid.aid_length) &&
      (memcmp(aid.data, aram_aid, aid.aid_length) == 0) )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}/* uim_is_aram_aid */


/*===========================================================================
FUNCTION UIM_UTIL_GET_EF_FILE_SIZE

DESCRIPTION
 This function is used to retrieve the file size from FCP parameter

DEPENDENCIES
  None

RETURN VALUE
  File size of EF

SIDE EFFECTS
  None

===========================================================================*/
unsigned int uim_util_get_ef_file_size
(
  uim_instance_global_type *uim_ptr,
  uim_rsp_buf_type      *rsp_ptr
)
{
  
  uint16 i = 0; /* index variable */
  uint16 j = 0; /* Temp storage for length */
  uint16 k = 0; /* Innerloop variable for proprietary information */
  
  byte const *data     = rsp_ptr->rsp.data;
  word num_bytes       = rsp_ptr->cmd_rsp_size;
    
  if (num_bytes > UIM_MAX_CHARS)
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id,
                    "uim_get_ef_file_size cmd rsp size is greater than rsp buf size");
    return ((unsigned int)0);
  }

  if(UIM_ICC  == uim_ptr->command.cmd_ptr->hdr.protocol)
  {
    return ((unsigned int)FLIPW(rsp_ptr->rsp.ef.file_size));
  }
  
  /* Process all the bytes in FCP */
  while (i < num_bytes)
  {
    if (data == NULL)
    {
      UIMDRV_MSG_ERR_0(uim_ptr->id,"uim_get_ef_file_size data is NULL");
      return ((unsigned int)0);
    }
    switch (data[i++])
    {
      case UIM_FCP_TEMPLATE_TAG:
        i++; /* Move past the length field */
        break;
  
      case UIM_FILE_DESCRIPTOR_TAG:
      if (i >= num_bytes)
      {
        return((unsigned int)0);
      }
      /* store the length to advance */
      j = data[i++];
      if ((i + j) >= num_bytes)
      {
        return((unsigned int)0);
      }
      switch (data[i] & UIM_UICC_EF_STRUCT_MASK)
      {
        case UIM_UICC_TRANSPARENT_EF:
        case UIM_UICC_LINEAR_EF:
        case UIM_UICC_CYCLIC_EF:
        default:
          break;
      }
      /* Skip j bytes */
      i+=j;
      break;
  
      case UIM_PROPRIETARY_TAG:
      if (i >= num_bytes)
      {
        return ((unsigned int)0);
      }
      k = i + data[i];
      i++;
      while(i < k)
      {
        if (i >= num_bytes)
        {
          return ((unsigned int)0);
        }
        switch(data[i++])
        {
          case UIM_CHARACTERISTICS_TAG:
          /* check here if i is a good index for "j" and the file_char */
          if ((i + 1) >= num_bytes)
          {
            return ((unsigned int)0);
          }
          j = data[i++];
          i+=j;
          break;
  
          case UIM_SUPPORTED_CMD_TAG:
          /* check here if i is a good index for "j" and supported_cmd_present */
            if ((i + 1) >= num_bytes)
            {
              return ((unsigned int)0);
            }
  
            j = data[i++];
            i+=j;
            break;
  
            default:
            if (i >= num_bytes)
            {
              return ((unsigned int)0);
            }
            j = data[i++];
            i+=j;
            break;
          }
        }
        break;

      case UIM_FILE_SIZE_TAG:
        /* j is at length offset  */
        j = data[i++];
        /* if Length is of more that one byte long */
        if (j > 1 )
        {
          return((int)FLIPW(data + i));
        }

        /* Length is of one byte long */
        return(data[i]);
      
      case UIM_FILE_ID_TAG:
      case UIM_DF_NAME_TAG:
      case UIM_TOTAL_FILE_SIZE_TAG:
      case UIM_SECURITY_ATTR_TAG1:
      case UIM_SECURITY_ATTR_TAG2:
      case UIM_SECURITY_ATTR_TAG3:
      case UIM_SECURITY_ATTR_TAG4:
      case UIM_LIFE_CYCLE_STATUS_TAG:

      case UIM_SFI_TAG:
       /* Ignore the tags that are not needed */
      default:
      if (i >= num_bytes)
      {
        return ((unsigned int)0);
      } /* Get the length */
      j = data[i++];
      /* Skip j bytes */
      i+=j;
      break;
    }
  }
  return ((unsigned int)0);
} /* uim_util_get_ef_file_size */
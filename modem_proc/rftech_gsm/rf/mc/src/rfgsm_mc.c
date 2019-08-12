/*!
   @file
   rfgsm_mc.c

   @brief
   This file contains all type declarations and definitions for the GSM Main
   Control layer.


   @details
   
*/

/*===========================================================================
Copyright (c) 1999 - 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/rf/mc/src/rfgsm_mc.c#1 $
$DateTime: 2016/12/13 07:59:51 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/08/16   bp      Put the QPOET to standby mode after GSM Tx burst ends 
02/29/16   par     Fix KW warning
02/23/16   par     Add MC logging buffer for failing sequences
02/05/15   par     Fix critical section lock relinquishing when sleep or wakeup
                   sequence fails
09/10/15   par     Changes to separate PRx and DRx gain pointer updates
04/23/15   par     Fix incorrect AFC being applied in GBTA cases
03/25/15   bp      Add BB headroom computation in NV reloading
01/28/15   mz      Fix wrong jdet_counter reset when rx band changes
01/22/15   piy     Fixed bug in Device status read feature for Mon Burst
1/16/15    sml     Reduce the number of tuner transaction for DRx device
                   when ASD probe burst is active
1/13/15    sml     GL1 -> RF handshake mechanism to handles cases when 
                   GSM ASD response is not being received as scripts 
                   weren't executed
01/12/15   sml     Changes to remove un-necessary tuner transactions 
                   during probe burst
12/02/14   zbz     Make rfc gsm timing ptr per device
12/03/14   sb      Call jdet on sequence before RX burst
12/02/14   sml     Read WTR or RF front end devices status in between Rx/Tx 
                   burst along with GRFC status
11/25/14   hoh     Apply temp comp to originally calculated frequency compensated AMAM/AMPM table
11/25/14   sw      Disable therm read during WTR dedicated hopping
11/18/14   sw/sc   Deprecate legacy state machines and move cmd buff out of rfa
11/14/14   zbz     Explicitly set RF Device frequency error to 0 in Enter Mode
11/11/14   sw      Re-enable temp comp
11/07/14   hoh     Disable temp comp temporarily to unblock MST testing
11/04/14   tws     Enable sawless, vbatt and temp comp on JO
10/23/14   sc      Reduce code execution limiting use of get qsym API
10/17/14   rr      Separate rx/tx alternate path core handles
10/06/14   sc      Deprecate unused functionality
10/08/14   ch      Store gmsk_modulation flag 
10/01/14   tws     Disable sawless, vbatt and temp comp on JO
10/01/14   tsr     GSM IDLE Mode RXD support
09/27/14   ch      Disable the CL Tuner on the 8PSK or unknown modulation 
09/26/14   hoh     Remove temp code to default LIF to 0 now that L1/FW changes are in
09/23/14   zbz     changes to support GSM and LTE/TDS coexistence related RFC changes
09/19/14   sw      Enable GSM thermistor read
09/15/14   rr      Fix for nv container loading
09/12/14   sml     Adding start API Pcycle to know about pre-emption done to RF task
09/11/14   sml     Fixing KW warning
11/19/13   cpm     support for FTM_PROGRAM_QFE_SCENARIO and Antenna Tuner force_detune() 
08/22/14   rr      Changes to support nv c3
08/19/14   zbz     Add RF GSM CCS execution time profiling
08/13/14   ggs     Disable VBatt and therm read if T_RUMI_EMULATION is defined
08/11/14   zbz     Add sanity check for GSM dynamic temp comp
08/04/14   tws     Stub functionlaity for RUMI pre-silicon
07/31/14   ggs     Fix bug which was causing core handle mutex to not be released if exit mode fails
07/30/14   hoh     Add enhanced rx feature support
07/24/14   tsr     DR-DSDS: Add support for device driver to handle worst case rx alpha for IDLE mode RX
07/28/14   hk      Enable WTR2605 L2G measurements
07/28/14   ec      Correct MC layer burst metrics processing API
07/21/14   sc      Add vreg vote count per sub per path
07/17/14   av      Disable therm read for DPM till final fixes for multisim therm are identified
07/11/14   ec      Add MC layer function to process burst metrics, enable SAWless feature on SAWless devices
07/04/14   jr      Changes for TSTS support
07/01/14   jr      Enable therm read on sub-2
06/25/14   zbz     Add feature to adjust temp comp table based on therm read during cal
06/24/14   av      Enable predistortion for DPM
06/18/14   tsr     RXD diversity call flow changes to enable DRX path along with PRX path
06/10/14   hk      Edge core parameter from GL1.
06/07/14   zhh     correct FEATURE_BOLT_PLUS_MODEM for rftech_gsm
05/29/14   sw      Re-enable therm read and vbatt compensation for BOLT single-sim (disabled by accident)
05/29/14   sw      Move therm read from enter mode to tx enable
05/28/14   sc      Allow MC init state in wakeup for IDLE WTR hopping
05/27/14   sb      Changes to add RFM APIs for GSM arfcn to frequency conversion
05/08/14   sc      Add RFLM GSM init once API interface
05/08/14   sc      Remove profiling for meas APIs as as_id cannot be acquired
05/02/14   sc      Remove profiling for meas APIs as as_id cannot be acquired
04/29/14   sc      Clean up debug flags and temp disable multi-SIM therm reads
04/24/14   ggs     Port CR581892: Update Coex Backoff parameters from GL1 
                   for each frame
03/28/14   sw      Disable tuners during X2G iRAT scenarios (port from Dime3)
04/10/14   sc      Fix KW error of possible dereference to RxD argument
04/07/14   ggs     KW fixes
03/31/14   tsr     Added support for handling combined Enter Mode for PRX and DRX
03/28/14   sw      Enable temperature compensation
03/26/14   sb      Add support for IP2 cal for Dime PM
03/18/14   sc      Add subscription based execution intelligence
03/11/14   tsr     Added RXD support for RX burst 
03/11/14   sw      Enable vbatt compensation
03/04/14   sw      ASD support on wakeup
02/28/14   sc      Pass core data through mc to core layers
02/26/14   sc      Pass core data through cmd processing and mc layer
02/19/14   tsr     GSM RX Diversity 
02/17/14   sc      Clean up mDSP APIs
02/13/14   ggs     Enable Pre-distortion for Bolt
02/11/14   vv      backedout the previous change
02/11/14   sw      Remove therm read TRITON featurisation
02/06/14   sg      Handle the RFC_DEVICE_MISSING_FATAL scenario for RF daughter card support
01/15/14   ggs     Disable AMAM AMPM table override for Bolt
01/15/14   sc      Remove unused APIs using FW shared memory
01/08/14   cj      Added API to update buffer index in rfgsm_core_handles
12/13/13   ry      Remove hardcoding of RFM_DEVICE_0
11/12/13   cj      pre_config and build_reset_scripts functions de-registered for Bolt
10/28/13   ggs     Downsample base AMAM temp comp LUT by a factor of 4 
10/23/13   sg      Added new flag to know if wakeup was done before the start of GSM burst
10/17/13   sc      Feature-guard population of GFW shared mem variables
10/17/13   ggs     Add support to override amam and ampm tables with default
                   values and disable pre-distortion and disable Vbatt/temp
                   comp for BOLT using debug flags
10/14/13   sc      Mainline core DM and RFLM code
10/11/13   ec	     Enable GTA state tracking (Triton Only)
09/25/13   sb      Added changes to support GSM TX calibration/online mode operation in coexistence
09/23/13   ggs     Disable Temp and Vbatt compensation on RUMI platform
09/17/13   sb      Fix check for right rx burst type 
09/17/13   sb      Support for SGLTE/SGTDS coexistence
09/12/13   sc      RFLM interface encompassed by core DM
08/28/13   ec      Creating RF GSM Core state machine with functions to init and
                   drive
08/16/13   ndb     Clean-up the SPI based antenna tuner support
08/14/13   sc      Revert Rx burst type variable name
08/02/13   sml     Fixing Complier warnings
07/31/13   sb      Make changes for BC config to be NV container based
07/31/13   av      GBTA support for both GL1 sub1 and GL1 sub2 call RF to build RX scripts
07/29/13   sml/svi ASD support for GSM tech
07/24/13   ec	   Get as_id for Frame Number logging from RF Task
07/18/13   sc      Update RFLM data information (featurised)
07/23/13   ec	   Fix Compilation Error
07/22/13   ec	   Use new F3s/Null pointer checks
07/15/13   cj      SGLTE changes for IRAT
07/16/13   sc      Move mc/core functionality out of dispatcher and into mc/core layer
07/12/13   sc      Add process Tx burst API
07/12/13   av      GTA EXIT mode
06/28/13   ec      Hardcode task context logged to CMD buffer in meas apis, gate mc
                   layer task id from rfgsm_mode from core handle prevent overhead in
                   IRAT scenario.
06/28/13   ec      Log the rf task context commands are executing in. Add macros 
				   to update the mc_state variable in the core handle.
06/25/13   ec      Make changes to support Therm Read Cancellation
06/20/13   ec      Add support for GSM Tune Away (GTA) enter mode. Enable 
                   temperature compensation on DS targets.
06/20/13   ec      Enable vbatt comp in DSDA/DSDS mode
05/30/13   tws     Enable temp comp in DSDA/DSDS mode
05/08/13   tws     Use rfgsm_core_enable_hal_bus()
05/07/13   tc      removed obsolete SPI function calls 
04/30/13   aka     fix for GSM reload where bad NV ptsr are left in core handles
04/24/13   cj      Added rfgsm_mc_pre_config and rfgsm_mc_build_reset_scripts API
04/08/13   pl      Add null function pointer to support pre_config
03/21/13   tsr     Added RF APPS->RF Task MSGR framework for temp comp
03/25/13   tws     Enable Vbatt compensation on non-DSDA PLs.
03/21/13   ec      Replace deprecated RFM_MAX_DEVICES with RFM_MAX_WAN_DEVICES
03/18/13   aka     GSM NV reload fix -use NV deinit for clean-up
03/12/13   tsr     Added to tag to each therm read event handle to identify the successful retrieval 
01/03/13   tws     Disable temperature compensation for Triton bringup.
02/25/13   sc      Added cm enter/exit to init/de-init iRAT during gl1 cm init/terminate 
02/19/13   sc      Added APIs to allow GL1 to alloc and free Rx meas buffers
02/15/13   tsr     Enable Temp Comp algorithm 
02/15/13   sc      Call rfgsm_core_exit_mode
02/12/13   sr      changes to free the memory alloocated during rfm_init() 
01/17/12   aka     Logic clean-up in rfgsm_mc_update_freq_error function
12/17/12   aka     Removing legacy GSM NV support
01/11/13   aka     Changes to support simultaneous Tx SAR
01/09/13   sr      Made changes to run vbatt/temp comp in rf app task context.
01/03/12   sc      Added therm read full call flow framework 
12/18/12   sc      Added therm read event generation framework
01/3/13    av      Changes to support chain2 for GSM
12/03/12   sc      Cleanup mDSP shared variable interface
11/26/12   aka     Changes to use a path specific gsm handle
11/21/12   av      Added rfm_device parameters to mc/core/mdsp/msm layers.
11/15/12   ec      Using new GL1 test apis for Triton compatibility
11/07/12   pl      Added cleanup function pointer support in API structure
11/02/12   jfc     Replace rf_hal_enable_rfcmd_app with rf_hal_bus_enable 
10/16/12   tsr     Cleanup GSM RF bringup flags
10/11/12   sc      Added prepare Rx burst API for inline GL1 iRAT readiness
10/11/12   sc      Cleanup up unrequired Rx burst & SBI/Non-SBI API burst buffer params
10/05/12   av      Expanded rf task architecture to include more tasks. 
10/05/12   tsr     Fixed compiler errors
10/05/12   tsr     First cut of time profile code 
10/04/12   sr      included the rfgsm_core_meas file. 
10/04/12   sr      Added IRAT APIs. 
09/25/12   tc      Mainline RF_HAS_RFA_GSM_TASK. 
09/24/12   sb      Made changes to update TXLM,RXLM modem chain 
09/18/12   sb      Added un-voting of GSM client through RF HAL API for requesting and releasing RFCMD app
08/28/12   sb/tsr  Added disable temp comp flag for bringup and removed the RF bring up feature
08/21/12   sc      Add and cleanup the bringup flags for GSM RF
08/13/12   tsr     Replaced Bringup feature with rumi_bup_gsm flag to facilitate Bringup
08/10/12   tsr     RF GSM CCS debug and Time profile flags added
08/08/12   sr      added rfm_dev parameter to tx_power api
08/06/12   sc      Edit API to update CCS header FW vars
08/02/12   sc      Add num slots to setup tx burst API
07/26/12   sr      Changes to APIS to take rfm_dev as parameter.
07/17/12   tsr     Additional GSM debug flags for RUMI Bring Up
07/12/12   tsr     Add GSM debug flags for RUMI Bring Up
07/06/12   sc      Edit API parameters to support CCS buffer format
07/06/12   sc      Remove unrequired num transactions init functionality
06/25/12   ggs     Added rfgsm_ftm_dispatch_init() to initialise rfgsm ftm msgr module
06/19/12   sc      Added Fast Scan status for RFGSM MC scope and corrected return types
06/19/12   sc      Added shared memory interface integration for Enter mode, Wakeup,
                   Tx band, Rx/Tx burst, and Tx power APIs
05/20/12   sb      Made changes to let RF drivers populate timing info correctly before passing onto FW
04/09/12   sb      Add changes to fix frequency error adjustment in FTM mode
03/28/12   bm      Reverting the clean up changes done in previous revision
08/26/11   aka     Update MC state during sleep and wakeup and some clean-up
                   during MC state change
02/06/12   kg      Reverting:return parameter to hardcoded value "TRUE" 
02/01/12   kg      Change return parameter from hardcoded value "TRUE" to "nv_init_success" in rfgsm_mc_init()
01/10/12   sr      changes to enable vbatt comp.
12/08/11   aak     Change to not power off the vregs in rfgsm_mc_exit_mode() 
11/29/11   sb      Added functionality for GSM IQ CAPTURE
10/27/11    sr     made changes to trigger SAR state change for GSM.
10/19/11   sb      Apply therm read in idle frame and enable FTM temp comp and vbatt comp during idle frame
10/18/11   lcl     Fixed compilation warning.
10/17/11    sr     made changes add F3 msgs via DPC task for isr overrun issue.
10/13/11   lcl     Added antenna tuner device init to enter_mode
10/11/11    sr     Added to log RFGSM_MC functions time profile,
09/20/11    sr     made changes to update the tx_band variable correctly.
08/25/11   sar      Removed obsolete file bbrx.h.
03/21/11    sr     Added LM support and timing info access interface.
11/22/10   te      correct rfgsm_mc_disable_tx() return value
10/14/10   mkv/whc Integrating Reload RFNV from SCMM into MDM9x00 (CL 1315259)
10/11/10   lcl     Changed MSG_LVL to MSG_LEGACY to avoid too many F3 messages
09/21/10   ggs     Added new api to return rf_warmup time
07/21/10   lcl     Remove SWC clock enabling again.
07/18/10   lcl     GL1 doesn't seem to doing SWC correct--failed GSM RX cal. 
                   Put in a temporary hack for now. 
07/14/10   lcl     Remove SWC clock enabling.  GL1 should take care of enabling 
                   the SWC clock after rfm_enter_mode(GSM) is done. 
02/11/09   sr      Removed the mc_state check in chan_tune to make the wtog work.
02/09/09   sr      moving the pmic calls to RFC.
02/08/09   sr      lint errors fix
29/01/10   sr      Changed the <tech>_mc_init() signature to fix warnings in 
                   <tech> specific builds
01/28/10    sr     added  power_clock_o/off functions
12/28/09   sr      High/Medium lint errors fix
12/22/09   sr     changed the debug msg macro from legacy to new one.
12/01/09   sr     return TRUE from rfgsm_mc_init() until we fix the nv_status.
                  Do not call the rfgsm_enter_mode() if the common_init is not successfull.
11/30/09   bmg    Added common_init_successful parameter to GSM initialization
                  Skip GSM initialization if the common NV could not be loaded
                  Changed init function return to boolean
11/18/09   dyc    Use correct PMIC switcher PM_VREG_RF2_ID vs PM_VREG_RF_ID
10/21/09   bmg    Changed pm_vreg.h to pm.h - proper include method for PMIC
10/15/09   wen    enable RF GSM sleep/wakeup 
09/22/09   dyc    Support for SMPS freq setting. 
09/17/09   sar    Removed deprecated file, adie.h for CMI conformance.
07/20/09   sr     moved the init_once() call to mc_init() and code cleanup.
07/13/09   dyc    Added temporary feature FEATURE_RF_USE_07_REL_CLKREG to support
                  clk_regime API transition.
07/06/09   sr     changes to make the operating band to rfcom_gsm_band type
06/26/09   sr     Code clean up
08/06/09   sr     SCMM bringup changes
03/25/09   sr     Added state transitions to block core functions calls with init failure
03/13/09   sr     code cleanup for SCMM build
10/14/08   sr     Initial version of GSM Main Control.

============================================================================*/

#include "msg.h"
#include "rfcom.h"
#include "rfcommon_core_types.h"
#include "rfgsm_mc.h"
#include "rfgsm_core.h"
#include "rfgsm_core_log.h"
#include "rfgsm_core_types.h"
#include "rfgsm_core_tx.h"
#include "rfgsm_core_util.h"
#include "ftm.h"
#include "rfm_internal.h" // For RUMI Bring up
#include "rf_hal_common.h"
#include "rfmeas_mc.h"
#include "rfgsm_core_meas.h"
#include "gtmrs.h"
#include "geran_test_mode_api.h"
#include "rfgsm_core_temp_comp.h"
#include "rfgsm_core_handle.h"
#include "rfcommon_locks.h"
#include "rfcommon_core.h"
#include "rfcommon_nv_mm.h"
#include "rfdevice_antenna_tuner.h"
#include "modem_mem.h"

/*=============================================================================

                      STATIC AND EXTERN FUNCTION PROTOTYPES

=============================================================================*/
rfgsm_mc_status_type rfgsm_mc_meas_init(void);
rfgsm_mc_status_type rfgsm_mc_meas_deinit(void);
static rfgsm_mc_status_type rfgsm_mc_allocate_rx_script_buffers( rfgsm_core_data_type *core_data );
static rfgsm_mc_status_type rfgsm_mc_free_rx_script_buffers( rfgsm_core_data_type *core_data );
static rfgsm_mc_status_type rfgsm_mc_get_timing_info( rfgsm_core_data_type *core_data, 
                                                      rfgsm_timing_info_type *timing_info );

/* External APIs for RF Time Profile */
extern unsigned long long qurt_get_core_pcycles(void);

#ifdef FEATURE_RF_ASDIV
/*---------------------------------------------------------------------------------------*/
extern boolean rfgsm_core_process_asd_read_rsp(rfgsm_core_data_type *core_data, uint8 ant_pos);

/*---------------------------------------------------------------------------------------*/
extern void rfgsm_core_set_antenna(rfgsm_core_data_type *core_data, uint8 ant_pos);

#endif

extern boolean rfgsm_core_process_device_status_read_rsp(uint8 sub_id, uint8 buffer_id, 
                                         rf_burst_type burst_type, uint8 event_type, uint8 event_tag, uint8 burst_num);


/*=============================================================================

                                DEFINITIONS

=============================================================================*/
// Struct that holds gsm debug flags
rfgsm_bup_flags_t  rfgsm_debug_flags = 
{
  1,        /* Disable GSM Antenna Tuner Flag */
  0,        /* Time Profiling Flag */
#if defined (T_RUMI_EMULATION)
  1,        /* Disable temp comp*/
  1,        /* Disable vbatt comp*/ 
#else
  0,        /* Disable temp comp*/
  0,        /* Disable vbatt comp*/
#endif 
  1,        /* rfgsm_init only Flag */
  0,        /* override_amam_ampm_tables */
  0,        /* predist_override: Disable Predistortion  */
  0,        /* sawless_override: Enable SAWless features */
};

#define RFGSM_MC_TASK(p, t)                 \
{                                           \
    if(p->rfgsm_mode == RFGSM_IRAT_MODE)    \
    {                                       \
        t = RF_MEAS_CALL;                   \
    } else                                  \
    {                                       \
        t = rfgsm_core_get_task_num();      \
    }                                       \
}

boolean rfgsm_mc_err_logging_enable = TRUE;
rfgsm_mc_err_buffer_type *rfgsm_mc_err_buffer_ptr = NULL;


/*=============================================================================

                               FUNCTION DEFINITIONS

=============================================================================*/
/*!
  @brief

  @details

  @param device: RF device
  @param caller_tcb_ptr: Calling task TCB pointer
  @param task_nv_wait_sig: Task signal to wait for when reading NV
  @param task_wait_func_ptr: Task's wait function to be called when reading NV
*/
boolean rfgsm_mc_init(boolean common_init_successful,
                      rex_tcb_type *caller_tcb_ptr,
                      rex_sigs_type task_nv_wait_sig,
                      void (*task_wait_func_ptr)( rex_sigs_type ))
{

  rfcommon_nv_status_type nv_status = RFCOMMON_NV_READ_FAILED;
  static boolean nv_init_success = FALSE;
  static boolean nv_items_read = FALSE;
  boolean status = FALSE;
  rfm_device_enum_type rfm_dev = RFM_DEVICE_0;
  rfgsm_core_handle_type* rfgsm_core_handle_ptr = NULL;
  rfgsm_nv_cmn_data_type* rfgsm_cmn_nv_ptr = NULL;

  uint8 flag = (uint8)FALSE;

  RF_MSG_1(MSG_LEGACY_HIGH,"rfgsm_mc_init", 0);

  rfgsm_nv_populate_coex_band_map_tbl();

  /* Initialise the core init once */
  if( rfgsm_core_init_once() != TRUE )
  {
    /* Core init once initialisation failed */
    RF_MSG(MSG_LEGACY_ERROR,"rfgsm_core_init_once failed");
    return FALSE;
  }

  if ( common_init_successful )
  {
    if (!nv_items_read)
    {

      rfgsm_nv_data_structure_init();

      /*Assumptions: Which needs to be corrected later when RFNV layer accepts nv container
      or we need a design in place for it. Right now we just loop for RFM devices to see if it is 
      supported on any bands for RX or TX. the cfg structure isn't used. This should work for the 
      present RFCs and near future RFCs*/
      for (rfm_dev=0;rfm_dev<RFM_MAX_WAN_DEVICES;rfm_dev++)
      {
        flag = (uint8)FALSE;

        /*load GSM NVs*/
        /*GSM INIT is not called for multiple paths as of now, need to check it for Triton*/
         nv_status = rfgsm_nv_load_items(rfm_dev, caller_tcb_ptr, task_nv_wait_sig, 
                                           task_wait_func_ptr, &flag);

        if (flag != (uint8)TRUE) // Check if logical device supported in rfc
        {
          continue;
        }

        if (nv_status == RFCOMMON_NV_READ_SUCCESS) // Check if all NVs were read correctly
        {
          /* Not missing critical NV items*/ 
          nv_init_success = TRUE;     
        }
        
        /* Remember that we've read the NV. */
        nv_items_read = TRUE;

        status = rfgsm_core_init(rfm_dev);
        if (!status)
        {
          RF_MSG_2(MSG_LEGACY_ERROR,
                   "rfgsm_core_init() Failed : rfm_dev %d, nv_init_success = %d", 
                    rfm_dev, nv_init_success);
          return status;
        }
        rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

        RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "NULL ptr when trying to set MC state!", FALSE);

        rfgsm_cmn_nv_ptr = rfgsm_nv_get_cmn_tbl_ptr(rfm_dev);
        if (rfgsm_cmn_nv_ptr != NULL)
        {
          if ((rfgsm_cmn_nv_ptr->cal_temp_nv_enable) && (rfgsm_cmn_nv_ptr->enh_tempcomp_enable_nv))
          {
            rfgsm_core_temp_comp_tbl_adjust(rfm_dev);
          }
        }
        else
        {
          RF_MSG_1(MSG_LEGACY_ERROR, " rfgsm_cmn_nv_ptr is NULL! rfm_dev = %d ",rfm_dev);
        }
      }
      
      rfgsm_core_validate_rx_timing_info();

    }

  } /* if ( common_init_successful ) */

  /*Moved the dispatch inits to rf_task and rf_Task_2 
  so that initialization can happen if task has only started */

  /* register the IRAT APIs with meas module */
  if( rfgsm_mc_meas_init() != RFGSM_MC_SUCCESS )
  {
    /* Initialisation of the IRAT fp table failed */
    return FALSE;
  }

  if (TRUE == rfgsm_mc_err_logging_enable)
  {
    /* If logging is enabled, allocate the needed buffer for MC logging */
    rfgsm_mc_err_buffer_ptr = modem_mem_alloc(sizeof(rfgsm_mc_err_buffer_type), MODEM_MEM_CLIENT_RFA );
	
    /* Check for allocation being success or failure */
    if (NULL == rfgsm_mc_err_buffer_ptr)
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Could not allocate memory!", 0);
      return FALSE;
    }
	
    /* If allocation is successful make sure to memset to zero before starting to use the buffer  */
    memset((void *)rfgsm_mc_err_buffer_ptr, 0, sizeof(rfgsm_mc_err_buffer_type));
  }
  /* Return TRUE since several calibrated NV's have critical attribute in GSM*/
  return TRUE;
}



/*----------------------------------------------------------------------------*/
/*!
  @brief free-up all the resources allocated during rfgsm_mc_init()

  @details
  free-up all the resources allocated during rfgsm_mc_init()

*/
boolean rfgsm_mc_deinit(void)
{
  rfm_device_enum_type rfm_dev;
  rfcom_gsm_band_type band;
  boolean rfm_device_supported = FALSE;
  rfc_cfg_info_type cfg_struct;
  rfgsm_core_handle_type* rfgsm_core_handle_ptr = NULL;
  uint8 rx_alternate_path;
  uint8 tx_alternate_path;  

  /* register NULL IRAT function ptr table */
  if( rfgsm_mc_meas_deinit() != RFGSM_MC_SUCCESS )
  {
    return FALSE;
  }

  rfgsm_core_deinit();

  for (rfm_dev=0;rfm_dev<RFM_MAX_WAN_DEVICES;rfm_dev++)
  {
    rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
    rx_alternate_path = (rfgsm_core_handle_ptr!=NULL)?rfgsm_core_handle_ptr->rx_alternate_path:0;
    tx_alternate_path = (rfgsm_core_handle_ptr!=NULL)?rfgsm_core_handle_ptr->tx_alternate_path:0;	

    for (band=0;band<RFCOM_NUM_GSM_BANDS;band++)
    {
      rfm_device_supported = rfc_gsm_get_cfg_info( rfm_dev, band, RFC_CONFIG_RX, &cfg_struct, rx_alternate_path );

      if (rfm_device_supported == FALSE)
      {
        rfm_device_supported = rfc_gsm_get_cfg_info( rfm_dev, band, RFC_CONFIG_TX, &cfg_struct, tx_alternate_path );
      }
 
      if (rfm_device_supported == TRUE)
      {
        break;
      }
    }
    if (rfm_device_supported == TRUE)
    {
      rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
      RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL in mc deinit!", FALSE);
      rfgsm_nv_deinit(rfm_dev);
    }
	
    modem_mem_free( rfgsm_mc_err_buffer_ptr, MODEM_MEM_CLIENT_RFA );
    rfgsm_mc_err_buffer_ptr = NULL;
  }
  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function calls rfgsm_mc functionality to create the RF CCS Events for all the
  Enter Mode..

  @param core_data      : rfgsm core structure that stores infomation like Multimode device
  @param enter_mode_ptr : Pointer to shared mem for Enter mode
  @retval               : True or False
*/
rfgsm_mc_status_type rfgsm_mc_build_enter_mode_ccs_events( rfgsm_core_data_type* core_data,
                                                         const void *enter_mode_ptr)
{
  rfgsm_mc_status_type mc_ret_status = RFGSM_MC_SUCCESS;

 if(!rfgsm_core_build_enter_mode_ccs_events(core_data,
                                            enter_mode_ptr))
  {
    RF_MSG_1(MSG_LEGACY_ERROR,
              "RFGSM Core Enter mode CCS Event Building Failed: RX Diversity is :%d", 
              core_data->enable_rx_diversity);
    mc_ret_status = RFGSM_MC_CORE_API_FAILURE;
  }

  return mc_ret_status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function calls rfgsm_mc functionality to create the RF CCS Events for all the
  Wakeup Mode..

  @param core_data      : rfgsm core structure that stores infomation like Multimode device
  @param enter_mode_ptr : Pointer to shared mem for Enter mode
  @retval               : True or False
*/
rfgsm_mc_status_type rfgsm_mc_build_wakeup_mode_ccs_events( rfgsm_core_data_type* core_data,
                                                           const void *wakeup_mode_ptr)
{
  rfgsm_mc_status_type mc_ret_status = RFGSM_MC_SUCCESS;

 if(!rfgsm_core_build_wakeup_mode_ccs_events(core_data,
                                             wakeup_mode_ptr))
  {
    RF_MSG_1(MSG_LEGACY_ERROR,
              "RFGSM Core wakeup mode CCS Event Building Failed: RX Diversity is :%d", 
              core_data->enable_rx_diversity);
    mc_ret_status = RFGSM_MC_CORE_API_FAILURE;
  }

  return mc_ret_status;
}


/*----------------------------------------------------------------------------*/
/*
  @brief

  @details

  @param user_data_ptr : Pointer to user data passed into callback
  @param cb_handler : Call back handler
*/

/*lint -esym(715, user_data_ptr, cb_handler) */ 
rfgsm_mc_status_type rfgsm_mc_enter_mode(rfgsm_core_data_type *core_data,
                                         uint32 rxlm_buf_idx,
                                         const void *enter_mode_ptr,
                                         rfgsm_timing_info_type *timing_info )
{
  /* Intialise status variable */
  rfgsm_mc_status_type mc_status = RFGSM_MC_ERROR;

  /* Get core lock */
  rf_common_enter_critical_section(&core_data->rfgsm_core_handle_ptr->rfgsm_core_enter_exit_crit_sect);

  /* Log the enter mode API call */
  RF_MSG( MSG_LEGACY_HIGH, "rfgsm_mc_enter_mode" );

  /* Temporarily assign until core layer deprecation of core handle var */
  core_data->rfgsm_core_handle_ptr->buffer_id = core_data->triple_buffer_id;

  // path = rfcommon_device_to_path(device);

  /* Execute core enter mode */
  if (!rfgsm_core_enter_mode(core_data, rxlm_buf_idx, enter_mode_ptr))
  {
    RF_MSG( MSG_LEGACY_ERROR, "rfgsm_core_enter_mode() Failed" );
    rf_common_leave_critical_section(&core_data->rfgsm_core_handle_ptr->rfgsm_core_enter_exit_crit_sect);
    
    /* Update failing case in log buffer */
    RFGSM_DEBUG_MC_ERR_LOG_Q(rfgsm_mc_err_buffer_ptr, 
                             RFGSM_MC_ENTER_MODE, 
                             core_data->rf_task, 
                             RFGSM_MC_CORE_API_FAILURE, 
                             (geran_test_mode_api_get_FN(core_data->as_id)),
                             0,
                             0 );

    return RFGSM_MC_CORE_API_FAILURE;
  }

  /* Explicitly set the RF Device frequency error to 0 */
  rfgsm_core_update_freq_error(core_data, 0);

  /* In GSM standalone mode */
  core_data->rfgsm_core_handle_ptr->rfgsm_mode = RFGSM_STANDALONE_MODE;

  /* Update the GL1 timing information */
  mc_status = rfgsm_mc_get_timing_info(core_data, timing_info);
  if( mc_status != RFGSM_MC_SUCCESS )
  {
    RF_MSG(MSG_LEGACY_ERROR, "rfgsm_mc_get_timing_info failed!");
    rf_common_leave_critical_section(&core_data->rfgsm_core_handle_ptr->rfgsm_core_enter_exit_crit_sect);
    return mc_status;
  }

  /* Release core lock */
  rf_common_leave_critical_section(&core_data->rfgsm_core_handle_ptr->rfgsm_core_enter_exit_crit_sect);

  return mc_status;
}



/*----------------------------------------------------------------------------*/
/*
  @brief

  @details

  @param user_data_ptr : Pointer to user data passed into callback
  @param cb_handler : Call back handler
*/

/*lint -esym(715, user_data_ptr, cb_handler) */ 
rfgsm_mc_status_type rfgsm_mc_rx_enter_mode(rfgsm_core_data_type *core_data,
                                            uint32 rxlm_buf_idx,
                                            const void *enter_mode_ptr)
{
  RF_MSG( MSG_LEGACY_HIGH, "rfgsm_mc_enter_mode" );

  /* Temporarily assign until core layer deprecation of core handle var */
  core_data->rfgsm_core_handle_ptr->buffer_id = core_data->triple_buffer_id;

  // path = rfcommon_device_to_path(device);

  if (!rfgsm_core_enter_mode(core_data, rxlm_buf_idx, enter_mode_ptr))
  {
    RF_MSG( MSG_LEGACY_ERROR, "rfgsm_core_enter_mode() Failed" );
    return RFGSM_MC_CORE_API_FAILURE;
  }

  /* In GSM standalone mode */
  core_data->rfgsm_core_handle_ptr->rfgsm_mode = RFGSM_STANDALONE_MODE;

  return RFGSM_MC_SUCCESS;
}



/*----------------------------------------------------------------------------*/
/*
  @brief 
  initialise for RF GSM iRAT
  called during gl1 cm rf init
*/
rfgsm_mc_status_type rfgsm_mc_cm_enter( rfgsm_core_data_type *core_data,
                                        rfgsm_timing_info_type *timing_info )
{
  /* Need to allocate the Rx measurement script buffers */
  rfgsm_mc_status_type cm_enter_status = RFGSM_MC_ERROR;

  /* Temporarily assign until core layer deprecation of core handle var */
  core_data->rfgsm_core_handle_ptr->buffer_id = core_data->triple_buffer_id;

  /* Allocate the Rx script buffers for IRAT */
  cm_enter_status = rfgsm_mc_allocate_rx_script_buffers(core_data);
  if ( cm_enter_status != RFGSM_MC_SUCCESS )
  {
    RF_MSG(MSG_LEGACY_ERROR, "rfgsm_mc_allocate_rx_script_buffers failed!");
    return cm_enter_status;
  }

  core_data->rfgsm_core_handle_ptr->rfgsm_mode = RFGSM_IRAT_MODE;

  cm_enter_status = rfgsm_mc_get_timing_info(core_data, timing_info);
  if ( cm_enter_status != RFGSM_MC_SUCCESS )
  {
    RF_MSG(MSG_LEGACY_ERROR, "rfgsm_mc_get_timing_info failed!");
    return cm_enter_status;
  }

  return cm_enter_status;
}



/*----------------------------------------------------------------------------*/
/*
  @brief 
  de-init for exiting RF GSM iRAT
  called during gl1 cm terminate
*/
rfgsm_mc_status_type rfgsm_mc_cm_exit( rfgsm_core_data_type *core_data )
{
  /* Need to free the Rx measurement script buffers */
  rfgsm_mc_status_type cm_exit_status = RFGSM_MC_ERROR;

  /* Temporarily assign until core layer deprecation of core handle var */
  core_data->rfgsm_core_handle_ptr->buffer_id = core_data->triple_buffer_id;

  cm_exit_status = rfgsm_mc_free_rx_script_buffers(core_data);

  /* Exiting GSM iRAT mode to unknown mode so return to default mode */
  if( cm_exit_status == RFGSM_MC_SUCCESS )
  {
    core_data->rfgsm_core_handle_ptr->rfgsm_mode = RFGSM_DEFAULT_MODE;
  }

  return cm_exit_status;
}



/*----------------------------------------------------------------------------*/
/*
  @brief

  @details

  @param user_data_ptr  : Pointer to user data passed into callback
  @param rfm_dev        : Device ID
*/

/*lint -esym(715, rfm_dev, user_data_ptr) */ 
rfgsm_mc_status_type rfgsm_mc_prepare_therm_read(rfgsm_core_data_type *core_data,
                                                 const void *user_data_ptr)
{
  if( rfgsm_debug_flags.disable_temp_comp == FALSE )
  {
    RF_MSG_2( MSG_LEGACY_HIGH, "rfgsm_mc_prepare_therm_read for sub % dev %d", core_data->sub_id, core_data->rfm_dev );

    /* Temporarily assign until core layer deprecation of core handle var */
    core_data->rfgsm_core_handle_ptr->buffer_id = core_data->triple_buffer_id;

    if ( !rfgsm_core_prepare_therm_read(core_data, user_data_ptr) )
    {
      RF_MSG( MSG_LEGACY_ERROR, "rfgsm_core_prepare_therm_read() Failed" );
      return RFGSM_MC_CORE_API_FAILURE;
    }
  }

  return RFGSM_MC_SUCCESS;
}



/*----------------------------------------------------------------------------*/
/*
  @brief

  @details

  @param user_data_ptr  : Pointer to user data passed into callback
  @param rfm_dev        : Device ID
*/

/*lint -esym(715, rfm_dev) */ 
rfgsm_mc_status_type rfgsm_mc_process_therm_read_rsp(rfgsm_core_data_type *core_data, 
                                                     uint8 tag, 
                                                     boolean therm_read_ok)
{
  /* Temporarily assign until core layer deprecation of core handle var */
  core_data->rfgsm_core_handle_ptr->buffer_id = core_data->triple_buffer_id;

  if( !rfgsm_core_process_therm_read_rsp(core_data, tag, therm_read_ok) )
  {
    RF_MSG( MSG_LEGACY_ERROR, "rfgsm_core_process_therm_read_rsp() Failed" );

    /* Update failing case in log buffer */
    RFGSM_DEBUG_MC_ERR_LOG_Q(rfgsm_mc_err_buffer_ptr, 
                             RFGSM_MC_THERM_READ_RSP, 
                             core_data->rf_task, 
                             RFGSM_MC_CORE_API_FAILURE, 
                             (geran_test_mode_api_get_FN(core_data->as_id)),
                             tag,
                             therm_read_ok );
	
    return RFGSM_MC_CORE_API_FAILURE;
  }

  return RFGSM_MC_SUCCESS;
}

/*----------------------------------------------------------------------------*/
/*
  @brief       
  Stops therm read 2-sec timer for device specified

  @details

  @param rfm_dev        : Device ID
*/

rfgsm_mc_status_type rfgsm_mc_stop_therm_read( rfm_device_enum_type rfm_dev )
{
  /* Stop therm read 2sec timer */
  rfgsm_core_temp_comp_stop( rfm_dev );

  return RFGSM_MC_SUCCESS;
}

/*----------------------------------------------------------------------------*/
/*
  @brief rfgsm_mc_process_device_status_read_rsp

  @details This API is called when the Response of Device Status Read is to be processed 
           and it calls rfgsm_core_process_device_status_read_rsp API to further process it.

  @param sub_id           : Subscription ID
  @param buffer_id        : Buffer ID
  @param burst_type       : Burst Type (Rx/Tx)
  @param event_type       : Event Type
  @param event_tag        : Device Status Read Event Tag
*/
rfgsm_mc_status_type rfgsm_mc_process_device_status_read_rsp(uint8 sub_id,
                                                             uint8 buffer_id, 
                                                             rf_burst_type burst_type, 
                                                             uint8 event_type, 
                                                             uint8 event_tag,
                                                             uint8 burst_num)
{
  if( !rfgsm_core_process_device_status_read_rsp(sub_id, buffer_id, burst_type, event_type, event_tag, burst_num))
  {
    RF_MSG( MSG_LEGACY_ERROR, "rfgsm_core_process_device_status_read_rsp() Failed" );

    /* Update failing case in log buffer */
    RFGSM_DEBUG_MC_ERR_LOG_Q(rfgsm_mc_err_buffer_ptr, 
                             RFGSM_MC_DEVICE_READ_RSP, 
                             (rfgsm_core_get_task_id_from_sub_id(sub_id)), 
                             RFGSM_MC_CORE_API_FAILURE, 
                             (geran_test_mode_api_get_FN(sub_id)),
                             event_type,
                             event_tag );
	
    return RFGSM_MC_CORE_API_FAILURE;
  }

  return RFGSM_MC_SUCCESS;
}


#ifdef FEATURE_RF_ASDIV
/*----------------------------------------------------------------------------*/
/*
  @brief

  @details

  @param user_data_ptr  : Pointer to user data passed into callback
  @param rfm_dev        : Device ID
*/

/*lint -esym(715, rfm_dev) */ 
rfgsm_mc_status_type rfgsm_mc_process_asd_read_rsp(rfgsm_core_data_type *core_data,
                                                   uint8 ant_pos)
{
  /* Temporarily assign until core layer deprecation of core handle var */
  core_data->rfgsm_core_handle_ptr->buffer_id = core_data->triple_buffer_id;

  if( !rfgsm_core_process_asd_read_rsp(core_data, ant_pos) )
  {
    RF_MSG(MSG_LEGACY_ERROR,"rfgsm_core_process_asd_read_rsp() Failed");
    return RFGSM_MC_CORE_API_FAILURE;
  }
  return RFGSM_MC_SUCCESS;
}
#endif



/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function set the temp comp flag upon receiving a msg from RF apps task
  and set the TX profile update flag.
 
  @param
  rfm_dev        : Device ID
  int16 temp_comp_scaled_value
  uint8 current_temp_bin_16
 
  @retval boolean
*/
rfgsm_mc_status_type rfgsm_mc_update_temp_comp_req(rfgsm_core_data_type *core_data,
                                                   int16 temp_comp_scaled_value,
                                                   uint8 current_temp_bin_16)
{
  /* Temporarily assign until core layer deprecation of core handle var */
  core_data->rfgsm_core_handle_ptr->buffer_id = core_data->triple_buffer_id;

  /* Update temp comp flag to indicate therm read is updated*/
  rfgsm_core_temp_comp_flag_set(core_data->rfgsm_core_handle_ptr);

  /*update scaled value to be used by TXAGC( used for calculating pcl based Tx power compensation*/
  rfgsm_core_temp_comp_pwr_update(core_data->rfgsm_core_handle_ptr, temp_comp_scaled_value);

  /*update temp comp bin index to be used by TXAGC( compensation for amam/pm tables)*/
  rfgsm_core_temp_comp_bin_index_update(core_data->rfgsm_core_handle_ptr, current_temp_bin_16);

  /*update tx profile update flag */
  rfgsm_core_set_tx_profile_update_flag(core_data->rfgsm_core_handle_ptr);

  return RFGSM_MC_SUCCESS;
}



/*----------------------------------------------------------------------------*/
/*
  @brief

  @details
*/
/*lint -esym(715, device, to_mode) */ 
rfgsm_mc_status_type rfgsm_mc_exit_mode(rfgsm_core_data_type *core_data)
{
  /* Get core lock */
  rf_common_enter_critical_section(&core_data->rfgsm_core_handle_ptr->rfgsm_core_enter_exit_crit_sect);

  /* Log the exit mode API call */
  RF_MSG( MSG_LEGACY_HIGH, "rfgsm_mc_exit_mode" );

  /* Temporarily assign until core layer deprecation of core handle var */
  core_data->rfgsm_core_handle_ptr->buffer_id = core_data->triple_buffer_id;

  /* Execute core exit mode */
  if (!rfgsm_core_exit_mode(core_data, RFM_INVALID_MODE))
  {
    RF_MSG( MSG_LEGACY_ERROR, "rfgsm_mc_exit_mode() Failed");
    /* Release core lock */
    rf_common_leave_critical_section(&core_data->rfgsm_core_handle_ptr->rfgsm_core_enter_exit_crit_sect);
	
    /* Update failing case in log buffer */
    RFGSM_DEBUG_MC_ERR_LOG_Q(rfgsm_mc_err_buffer_ptr, 
                             RFGSM_MC_EXIT_MODE, 
                             core_data->rf_task,
                             RFGSM_MC_CORE_API_FAILURE, 
                             (geran_test_mode_api_get_FN(core_data->as_id)),
                             0,
                             0 );		
    return RFGSM_MC_CORE_API_FAILURE;
  }

  /* Release core lock */
  rf_common_leave_critical_section(&core_data->rfgsm_core_handle_ptr->rfgsm_core_enter_exit_crit_sect);

  return RFGSM_MC_SUCCESS;
}



/*----------------------------------------------------------------------------*/
/*
  @brief

  @details

  @param device : RF Multimode device
  @param channel : Mode dependent channel
  @param synths : Synthesizor(s) to tune
  @param user_data_ptr : Pointer to user data passed into callback
  @param cb_handler : Call back handler
*/
rfgsm_mc_status_type rfgsm_mc_tune_to_chan( rfgsm_core_data_type *core_data,
                                            uint16 channel,
                                            rfm_synth_tune_enum_type synths )
{
  boolean tune_to_chan_status = TRUE;

  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();

  /* Tune the requested synthesizer(s) */
  switch ( synths )
  {

#if 0
    case RFM_TUNE_TX:
        tune_to_chan_status &= rfgsm_core_tune_tx(core_data, channel );
        break;
#endif

    case RFM_TUNE_RX:
        tune_to_chan_status &= rfgsm_core_tune_rx(core_data, channel );
        break;
#if 0
    case RFM_TUNE_TX_RX:
        tune_to_chan_status &= rfgsm_core_tune_tx(core_data, channel );
        tune_to_chan_status &= rfgsm_core_tune_rx(core_data, channel );
        break;
#endif

    default:
        RF_MSG( MSG_LEGACY_ERROR, "rfgsm_mc_tune_to_chan():Invalid GSM synthesizer." ); 
  }

  RFGSM_DEBUG_EN_Q( core_data->rfgsm_core_handle_ptr, //core handle
                    RFGSM_MC_TUNE_TO_CHAN,            //api
                    core_data->rf_task,               //rf task
                    0,                                //command proc start qs
                    synths,                           //arg1
                    channel,                          //arg2 
                    geran_test_mode_api_get_FN(core_data->as_id), //SW frame num
                    start_ustmr,                      //start time USTMR
                    rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                    start_pcycle,                     //start time pcycles
                    qurt_get_core_pcycles()-start_pcycle ); //duration pcycles

  if(!tune_to_chan_status)
  {
    return RFGSM_MC_CORE_API_FAILURE;
  }

  return RFGSM_MC_SUCCESS;
}



/*----------------------------------------------------------------------------*/
/*
  @brief

  @details

  @param core_data : core data 
  @param tx_burst_params : Tx burst params passed from GL1
*/
rfgsm_mc_status_type rfgsm_mc_tune_to_tx_chan( rfgsm_core_data_type *core_data,rfa_rf_gsm_tx_burst_type *tx_burst_params)
{
  boolean tune_to_chan_status = TRUE;

  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();

  tune_to_chan_status &= rfgsm_core_tune_tx(core_data, tx_burst_params);

  RFGSM_DEBUG_EN_Q( core_data->rfgsm_core_handle_ptr, //core handle
                    RFGSM_MC_TUNE_TO_CHAN,            //api
                    core_data->rf_task,               //rf task
                    0,                                //command proc start qs
                    RFM_TUNE_TX,                           //arg1
                    tx_burst_params->tx_chan_params.arfcn,                          //arg2 
                    geran_test_mode_api_get_FN(core_data->as_id), //SW frame num
                    start_ustmr,                      //start time USTMR
                    rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                    start_pcycle,                     //start time pcycles
                    qurt_get_core_pcycles()-start_pcycle ); //duration pcycles

  if(!tune_to_chan_status)
  {
    return RFGSM_MC_CORE_API_FAILURE;
  }

  return RFGSM_MC_SUCCESS;
}



/*----------------------------------------------------------------------------*/
/*
  @brief

  @details
*/
rfgsm_mc_status_type rfgsm_mc_set_band( rfgsm_core_data_type *core_data, rfgsm_band_type band )
{
  rfcom_gsm_band_type rfcom_band = RFCOM_NUM_GSM_BANDS;
  
  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();

  /* convert to rfcom band and which is the band type the rfgsm core will
  operate on */ 
  rfcom_band = rfgsm_core_convert_rfband_to_rfcom(band);

  /* @TODO add core API return and handling */

  rfgsm_core_set_band(core_data, rfcom_band);

  RFGSM_DEBUG_EN_Q( core_data->rfgsm_core_handle_ptr,  //core handle
                    RFGSM_MC_SET_BAND,                 //api
                    core_data->rf_task,                //rf task
                    0,                                 //command proc start qs
                    band,                              //arg1
                    0,                                 //arg2 
                    geran_test_mode_api_get_FN(core_data->as_id), //SW frame num
                    start_ustmr,                       //start time USTMR
                    rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                    start_pcycle,                      //start time pcycles
                    qurt_get_core_pcycles()-start_pcycle ); //duration pcycles

  return RFGSM_MC_SUCCESS;
}



/*----------------------------------------------------------------------------*/
/*
  @brief

  @details
*/
rfgsm_mc_status_type rfgsm_mc_set_tx_band( rfgsm_core_data_type *core_data,
                                           rfgsm_band_type tx_band,
                                           void *shared_mem_ptr)
{
  rfcom_gsm_band_type rfcom_band = RFCOM_NUM_GSM_BANDS;
  
  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();

  /* Temporarily assign until core layer deprecation of core handle var */
  core_data->rfgsm_core_handle_ptr->buffer_id = core_data->triple_buffer_id;

  /* convert to rfcom band and which is the band type the rfgsm core will
  operate on */ 
  rfcom_band = rfgsm_core_convert_rfband_to_rfcom(tx_band);

  /* @TODO add core API return and handling */

  rfgsm_core_set_tx_band(core_data, rfcom_band, shared_mem_ptr);

  RFGSM_DEBUG_EN_Q( core_data->rfgsm_core_handle_ptr, //core handle
                    RFGSM_MC_SET_TX_BAND,             //api
                    core_data->rf_task,               //rf task
                    0,                                //command proc start qs
                    tx_band,                          //arg1
                    0,                                //arg2
                    geran_test_mode_api_get_FN(core_data->as_id), //SW frame num
                    start_ustmr,                      //start time USTMR
                    rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                    start_pcycle,                     //start time pcycles
                    qurt_get_core_pcycles()-start_pcycle ); //duration pcycles

  return RFGSM_MC_SUCCESS;
}



/*----------------------------------------------------------------------------*/
/*
  @brief

  @details
*/
rfgsm_mc_status_type rfgsm_mc_enable_tx( rfgsm_core_data_type *core_data,
                                         uint32 txlm_buf_idx )
{
  /* Temporarily assign until core layer deprecation of core handle var */
  core_data->rfgsm_core_handle_ptr->buffer_id = core_data->triple_buffer_id;

  /* @TODO add core API return and handling */

  /* Configure RFC for Tx, Setup up modem,  start temp comp, start vbatt comp */ 
  rfgsm_core_enable_tx(TRUE, txlm_buf_idx, core_data);

  return RFGSM_MC_SUCCESS;
}



/*----------------------------------------------------------------------------*/
/*
  @brief

  @details
*/
rfgsm_mc_status_type rfgsm_mc_disable_tx( rfgsm_core_data_type *core_data,
                                          uint32 txlm_buf_idx )
{
  /* Temporarily assign until core layer deprecation of core handle var */
  core_data->rfgsm_core_handle_ptr->buffer_id = core_data->triple_buffer_id;

  /* @TODO add core API return and handling */

  /* Setup RFC to disable Tx, Stop temp comp, stop vbatt comp;  txlm_buffer_index NOT used */
  rfgsm_core_enable_tx(FALSE, 0, core_data);

  return RFGSM_MC_SUCCESS;
}



/*----------------------------------------------------------------------------*/
/*
  @brief

  @details
*/
rfgsm_mc_status_type rfgsm_mc_sleep( rfgsm_core_data_type *core_data )
{
  /* Get core lock */
  rf_common_enter_critical_section(&core_data->rfgsm_core_handle_ptr->rfgsm_core_enter_exit_crit_sect);

  /* Log the sleep API call */
  RF_MSG( MSG_LEGACY_HIGH, "rfgsm_mc_sleep" );

  /* Temporarily assign until core layer deprecation of core handle var */
  core_data->rfgsm_core_handle_ptr->buffer_id = core_data->triple_buffer_id;

  /* Execute core sleep */
  if (!rfgsm_core_sleep(core_data))
  {
    RF_MSG( MSG_LEGACY_ERROR, "rfgsm_core_sleep() Failed");
    rf_common_leave_critical_section(&core_data->rfgsm_core_handle_ptr->rfgsm_core_enter_exit_crit_sect);

    /* Update failing case in log buffer */
    RFGSM_DEBUG_MC_ERR_LOG_Q(rfgsm_mc_err_buffer_ptr, 
                             RFGSM_MC_SLEEP, 
                             core_data->rf_task, 
                             RFGSM_MC_CORE_API_FAILURE, 
                             (geran_test_mode_api_get_FN(core_data->as_id)),
                             core_data->rfm_dev,
                             core_data->rxd_rfm_dev );
	
    return RFGSM_MC_CORE_API_FAILURE;
  }

  /* Release core lock */
  rf_common_leave_critical_section(&core_data->rfgsm_core_handle_ptr->rfgsm_core_enter_exit_crit_sect);

  return RFGSM_MC_SUCCESS;
}



/*----------------------------------------------------------------------------*/
/*
  @brief

  @details
*/
rfgsm_mc_status_type rfgsm_mc_wakeup( rfgsm_core_data_type *core_data,
                                      const void *user_data_ptr )
{
  /* Get core lock */
  rf_common_enter_critical_section(&core_data->rfgsm_core_handle_ptr->rfgsm_core_enter_exit_crit_sect);

  /* Log the wakeup API call */
  RF_MSG( MSG_LEGACY_HIGH, "rfgsm_mc_wakeup" );

  /* Temporarily assign until core layer deprecation of core handle var */
  core_data->rfgsm_core_handle_ptr->buffer_id = core_data->triple_buffer_id;

  /* Execute core wakeup */
  if (!rfgsm_core_wakeup(core_data, user_data_ptr))
  {
    RF_MSG( MSG_LEGACY_ERROR, "rfgsm_core_wakeup() Failed");
    rf_common_leave_critical_section(&core_data->rfgsm_core_handle_ptr->rfgsm_core_enter_exit_crit_sect);

    /* Update failing case in log buffer */
    RFGSM_DEBUG_MC_ERR_LOG_Q(rfgsm_mc_err_buffer_ptr, 
                             RFGSM_MC_WAKEUP, 
                             core_data->rf_task, 
                             RFGSM_MC_CORE_API_FAILURE, 
                             (geran_test_mode_api_get_FN(core_data->as_id)),
                             core_data->rfm_dev,
                             core_data->rxd_rfm_dev );
	
    return RFGSM_MC_CORE_API_FAILURE;
  }

  #ifdef FEATURE_RF_ASDIV
    core_data->rfgsm_core_handle_ptr->set_prev_ant_pos = TRUE;
  #endif

  /* Release core lock */
  rf_common_leave_critical_section(&core_data->rfgsm_core_handle_ptr->rfgsm_core_enter_exit_crit_sect);

  return RFGSM_MC_SUCCESS;
}



/*----------------------------------------------------------------------------*/
/*
  @brief

  @details
*/
rfgsm_mc_status_type rfgsm_mc_setup_tx_burst( rfgsm_core_data_type *core_data,
                                              uint8 num_tx_slots, 
                                              void *tx_burst_event_ptr )
{
  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();

  /* Temporarily assign until core layer deprecation of core handle var */
  core_data->rfgsm_core_handle_ptr->buffer_id = core_data->triple_buffer_id;

  /* @TODO add core API status return not tx alpha return and handling */

  (void)rfgsm_core_setup_tx_burst(core_data, num_tx_slots, tx_burst_event_ptr );

  RFGSM_DEBUG_EN_Q( core_data->rfgsm_core_handle_ptr, //core handle
                    RFGSM_MC_SETUP_TX_BURST,          //api
                    core_data->rf_task,               //rf task
                    0,                                //command proc start qs
                    0,                                //arg1
                    0,                                //arg2
                    geran_test_mode_api_get_FN(core_data->as_id), //SW frame num
                    start_ustmr,                      //start time USTMR
                    rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                    start_pcycle,                     //start time pcycles
                    qurt_get_core_pcycles()-start_pcycle ); //duration pcycles

  return RFGSM_MC_SUCCESS;
}



/*----------------------------------------------------------------------------*/
/*
  @brief

  @details
*/
rfgsm_mc_status_type rfgsm_mc_set_rx_gain( rfgsm_core_data_type *core_data,
                                           dBx16_T rx_lvl_dbm, 
                                           rf_gain_T *gain, 
                                           uint8 lna_range,
                                           uint16 channel,
                                           rfgsm_enh_rx_params_type enh_rx_params)
{
  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();

  if( !rfgsm_core_set_rx_gain(core_data, rx_lvl_dbm, gain, lna_range, channel, enh_rx_params) )
  {
    RF_MSG( MSG_LEGACY_ERROR, "rfgsm_core_set_rx_gain failed");
    return RFGSM_MC_CORE_API_FAILURE;
  }

  RFGSM_DEBUG_EN_Q( core_data->rfgsm_core_handle_ptr, //core handle
                    RFGSM_MC_SET_RX_GAIN,             //api
                    core_data->rf_task,               //rf task
                    0,                                //command proc start qs
                    lna_range,                        //arg1
                    0,                                //arg2
                    geran_test_mode_api_get_FN(core_data->as_id), //SW frame num
                    start_ustmr,                      //start time USTMR
                    rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                    start_pcycle,                     //start time pcycles
                    qurt_get_core_pcycles()-start_pcycle ); //duration pcycles

  return RFGSM_MC_SUCCESS;
}



/*----------------------------------------------------------------------------*/
/*
  @brief

  @details
*/
rfgsm_mc_status_type rfgsm_mc_setup_rx_burst( rfgsm_core_data_type* core_data,
                                              rf_burst_type burst_type,
                                              rfgsm_rx_burst_type rx_burst_type)
{
  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();

  rfgsm_core_setup_rx_burst(core_data, burst_type, rx_burst_type);

  RFGSM_DEBUG_EN_Q( core_data->rfgsm_core_handle_ptr, //core handle
                    RFGSM_MC_SETUP_RX_BURST,          //api
                    core_data->rf_task,               //rf task
                    0,                                //command proc start qs
                    burst_type,                       //arg1
                    rx_burst_type,                    //arg2
                    geran_test_mode_api_get_FN(core_data->as_id), //SW frame num
                    start_ustmr,                      //start time USTMR
                    rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                    start_pcycle,                     //start time pcycles
                    qurt_get_core_pcycles()-start_pcycle ); //duration pcycles

  return RFGSM_MC_SUCCESS;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
   @fn rfgsm_mc_populate_device_rx_burst_buffer 
   @brief This function retrieves wtr device driver scripts for GSM Burst and stores them in rfgsm_core_handles pointer
   @param  rfm_dev - RFM device processing for
   @param  burst - Rx, Mon, 
   @retval boolean - success/failure indication
*/
void rfgsm_mc_populate_rx_burst_device_buffers( rfgsm_core_data_type* core_data,
                                                boolean enable_rx_diversity,
                                                rf_burst_type burst )
{
  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();

  /*Clear the RX script buffers first*/
  if(!rfgsm_core_clear_rx_burst_device_scripts(core_data->rfm_dev))
  {
    RF_MSG_1(RF_ERROR,"RF GSM WTR RX START/STOP script buffer was unsuccessfully cleared for rfm_dev|%d|",
                      core_data->rfm_dev);
  }


  if ( rfgsm_core_get_sawless_status(core_data->rfm_dev) )
  {
    if ( !rfgsm_core_get_jdet_config_scripts(core_data) ) 
    {
      RF_MSG_1(RF_ERROR,"RF GSM Rx Burst could not configure JDET for rfm_dev|%d|",
                        core_data->rfm_dev);
    }
  }

  /* Retrieve WTR device scripts for RX Burst */
  if( !rfgsm_core_populate_rx_burst_device_buffers( core_data,
                                                    enable_rx_diversity, 
                                                    burst))
  {
    RF_MSG_1(RF_ERROR,"RF GSM Rx Burst WTR script buffers are not retrieved for rfm_dev|%d|",
                      core_data->rfm_dev);
  }


  RFGSM_DEBUG_EN_Q( core_data->rfgsm_core_handle_ptr, //core handle
                    RFGSM_MC_POPULATE_RX_DEVICE_BUFF, //api
                    core_data->rf_task,               //rf task
                    0,                                //command proc start qs
                    enable_rx_diversity,              //arg1
                    burst,                            //arg2
                    geran_test_mode_api_get_FN(core_data->as_id), //SW frame num
                    start_ustmr,                      //start time USTMR
                    rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                    start_pcycle,                     //start time pcycles
                    qurt_get_core_pcycles()-start_pcycle ); //duration pcycles
}

/*----------------------------------------------------------------------------*/
/*
  @brief

  @details
*/
rfgsm_mc_status_type rfgsm_mc_set_amam_profile ( rfgsm_core_data_type *core_data,
                                                 rfgsm_power_profile_type pwr_profile[5],
                                                 uint8 num_tx_slots, uint8 num_assigned_uplink_slots )
{
  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();

  /* @TODO add core API return and handling */

  rfgsm_core_set_tx_power(core_data, 
                          pwr_profile, 
                          num_tx_slots,
                          num_assigned_uplink_slots);

  RFGSM_DEBUG_EN_Q( core_data->rfgsm_core_handle_ptr, //core handle
                    RFGSM_MC_SET_AMAM_PROFILE,        //api
                    core_data->rf_task,               //rf task
                    0,                                //command proc start qs
                    num_tx_slots,                     //arg1
                    num_assigned_uplink_slots,        //arg2
                    geran_test_mode_api_get_FN(core_data->as_id), //SW frame num
                    start_ustmr,                      //start time USTMR
                    rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                    start_pcycle,                     //start time pcycles
                    qurt_get_core_pcycles()-start_pcycle ); //duration pcycles

  return RFGSM_MC_SUCCESS;
}



/*----------------------------------------------------------------------------*/
/*!
  @details
   This function apply SAR limit passed from MCS

  @param sar_limit_gmsk[5] : SAR limits for GSM for all 5 different multislot configs
         sar_limit_8psk[5] : SAR limits for EDGE for all 5 different multislot configs

*/
rfgsm_mc_status_type rfgsm_mc_set_simult_sar_limit( rfgsm_core_data_type *core_data,
                                                    int16 sar_limit_gmsk[5],
                                                    int16 sar_limit_8psk[5])
{
  /* Temporarily assign until core layer deprecation of core handle var */
  core_data->rfgsm_core_handle_ptr->buffer_id = core_data->triple_buffer_id;

  /* @TODO add core API return and handling */

  rfgsm_core_set_simult_sar_limit(core_data->rfgsm_core_handle_ptr, sar_limit_gmsk, sar_limit_8psk);

  return RFGSM_MC_SUCCESS;
}



/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details

*/
rfgsm_mc_status_type rfgsm_mc_update_freq_error( rfgsm_core_data_type *core_data, int32 freq_err, rfgsm_freq_err_type freq_err_type )
{
  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();

  if( (ftm_get_mode() != FTM_MODE) ||
      // In FTM mode, send the freq error update only if it is not in override mode
      (ftm_get_mode() == FTM_MODE && (rfgsm_core_get_gl1_freq_error_flag(core_data->rfgsm_core_handle_ptr) == TRUE)) )
  {
    if (RFGSM_FREQ_ERR_RX_ONLY == freq_err_type)
    {
      rfgsm_core_update_rx_freq_error(core_data, freq_err);
    }
    else if (RFGSM_FREQ_ERR_TX_ONLY == freq_err_type)
    {
      rfgsm_core_update_tx_freq_error(core_data, freq_err);
    }
  }
  
  RFGSM_DEBUG_EN_Q( core_data->rfgsm_core_handle_ptr, //core handle
                    RFGSM_MC_UPDATE_FREQ_ERROR,       //api
                    core_data->rf_task,               //rf task
                    0,                                //command proc start qs
                    freq_err,                         //arg1
                    0,                                //arg2
                    geran_test_mode_api_get_FN(core_data->as_id), //SW frame num
                    start_ustmr,                      //start time USTMR
                    rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                    start_pcycle,                     //start time pcycles
                    qurt_get_core_pcycles()-start_pcycle ); //duration pcycles

  return RFGSM_MC_SUCCESS;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function calls the rfgsm_core function to allocate the iRAT measurement
  script buffers.

  This function must be called from GL1 for iRAT scenarios.
*/
rfgsm_mc_status_type rfgsm_mc_allocate_rx_script_buffers( rfgsm_core_data_type *core_data )
{
  /* @TODO add core API return and handling */
  rfgsm_core_allocate_rx_script_buffers(core_data);
  return RFGSM_MC_SUCCESS;
}



/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function calls the rfgsm_core function to deallocate the iRAT measurement
  script buffers.

  This function must be called from GL1 for iRAT scenarios.
*/
rfgsm_mc_status_type rfgsm_mc_free_rx_script_buffers( rfgsm_core_data_type *core_data )
{
  /* @TODO add core API return and handling */
  rfgsm_core_free_rx_script_buffers(core_data);
  return RFGSM_MC_SUCCESS;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function calls all the rfgsm_mc functionality to create the RF CCS for all the
  bursts requested from GL1.

  This function can be called from RF Task context from the rfgsm_cmd_processing dispatcher
  or iRAT scenarios.

  @param device                   : RF Multimode device
  @param num_rx_bursts            : Number of Rx/Monitor bursts required
  @param rx_chan_params           : Rx burst parameters for each burst i.e arfcn
  @param gfw_rf_burst_event_ptrs  : Pointer to an array of pointers to shared mem
*/
rfgsm_mc_status_type rfgsm_mc_prepare_rx_burst( rfgsm_core_data_type* core_data,
                                                uint8 burst_num,
                                                rfgsm_rx_chan_params_type* rx_chan_params,
                                                rfgsm_rx_diversity_chan_params_type* rxd_chan_params,
                                                boolean process_drx_burst)
{
  rfgsm_mc_status_type mc_ret_status = RFGSM_MC_SUCCESS;
  rfcom_gsm_band_type rx_band = rfgsm_core_convert_rfband_to_rfcom(rx_chan_params[burst_num].band);

  mc_ret_status = rfgsm_mc_update_freq_error( core_data, rx_chan_params[burst_num].freq_err, RFGSM_FREQ_ERR_RX_ONLY );

  if(mc_ret_status == RFGSM_MC_SUCCESS)
  {
    // if the band changes and the burst is not MON_BURST or PWR_BURST
    if ( rx_chan_params[burst_num].burst_type == RF_RX_BURST 
        && core_data->rfgsm_core_handle_ptr->hl_ll_band != rx_band)
    {
      //Reset JDET counter
      memset(core_data->rfgsm_core_handle_ptr->jdet_counter, 0, MAX_NUM_ARFCNS_IN_BAND * sizeof(uint8));
      core_data->rfgsm_core_handle_ptr->hl_ll_band = rx_band;
    }
  
    mc_ret_status = rfgsm_mc_set_band( core_data, rx_chan_params[burst_num].band );
  }

  if(FALSE == process_drx_burst)
  {
    if(mc_ret_status == RFGSM_MC_SUCCESS)
    {
      mc_ret_status = rfgsm_mc_set_rx_gain( core_data,
                                            rx_chan_params[burst_num].rx_lvl_dbm,
                                            rx_chan_params[burst_num].gain_ptr,
                                            rx_chan_params[burst_num].lna_range,
                                            rx_chan_params[burst_num].arfcn,
                                            rx_chan_params[burst_num].enh_rx_params);
    }
  }
  else
  {
    if(mc_ret_status == RFGSM_MC_SUCCESS)
    {
      mc_ret_status = rfgsm_mc_set_rx_gain( core_data,
                                            rxd_chan_params[burst_num].rx_lvl_dbm,
                                            rxd_chan_params[burst_num].gain_ptr,
                                            rxd_chan_params[burst_num].lna_range,
                                            rx_chan_params[burst_num].arfcn,
                                            rx_chan_params[burst_num].enh_rx_params);
    }
  }
  if(mc_ret_status == RFGSM_MC_SUCCESS)
  {
    mc_ret_status = rfgsm_mc_tune_to_chan( core_data, rx_chan_params[burst_num].arfcn, RFM_TUNE_RX );
  }

  if(mc_ret_status == RFGSM_MC_SUCCESS)
  {
    mc_ret_status = rfgsm_mc_setup_rx_burst( core_data,
                                             rx_chan_params[burst_num].burst_type,
                                             rx_chan_params[burst_num].probe_burst);
  }

  return mc_ret_status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function calls all the rfgsm_mc functionality to create the RF CCS for all the
  bursts requested from GL1.

  This function can be called from RF Task context from the rfgsm_cmd_processing dispatcher
  or iRAT scenarios.

  @param device                   : RF Multimode device
  @param num_rx_bursts            : Number of Rx/Monitor bursts required
  @param rx_chan_params           : Rx burst parameters for each burst i.e arfcn
  @param gfw_rf_burst_event_ptrs  : Pointer to an array of pointers to shared mem
*/
rfgsm_mc_status_type rfgsm_mc_process_rx_burst( rfgsm_core_data_type *core_data,
                                                           uint8 num_rx_bursts,
                                                           rfgsm_rx_chan_params_type* rx_chan_params,
                                                           rfgsm_rx_diversity_chan_params_type* rxd_chan_params, 
                                                           void **gfw_rf_burst_event_sm_ptrs )
{
  uint32 index;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  /*Local Copy of PRX device*/
  rfm_device_enum_type prx_dev = core_data->rfm_dev;
  rfcom_gsm_band_type rfcom_band;
  rfgsm_mc_status_type mc_ret_status = RFGSM_MC_SUCCESS;
  boolean process_drx_burst = FALSE;  

  if( (num_rx_bursts <= 0) || (num_rx_bursts > RFA_RF_GSM_MAX_RX_ACTIVITIES) )
  {
    RF_MSG( MSG_LEGACY_ERROR,"num_rx_bursts = zero. Did not create Rx CCS events." );
    return RFGSM_MC_INVALID_PARAM;
  }
  
  if(core_data->enable_rx_diversity)
  {
    /*Updating core handle only for DRx device for probe burst*/
    rfgsm_core_handle_ptr = rfgsm_core_handle_get( core_data->rxd_rfm_dev );
    if( rfgsm_core_handle_ptr == NULL )
    {
      RF_MSG_1(MSG_LEGACY_ERROR,
               "rfgsm_core_handle_ptr is NULL for device %d", core_data->rxd_rfm_dev);
    }
  }

  /* Get core lock */
  rf_common_enter_critical_section(&core_data->rfgsm_core_handle_ptr->rfgsm_core_crit_sect);

  /* Temporarily assign until core layer deprecation of core handle var */
  core_data->rfgsm_core_handle_ptr->buffer_id = core_data->triple_buffer_id;

  for( index = 0; index < num_rx_bursts; index++ )
  {
    /* Copy the current burst num to core handle for RFLM data info use */
    core_data->rfgsm_core_handle_ptr->rx_burst_num = index;

    core_data->rfgsm_core_handle_ptr->rx_burst_type = rx_chan_params[index].rx_burst_type;

    core_data->rfgsm_core_handle_ptr->rx_burst = rx_chan_params[index].burst_type;

    core_data->rfgsm_core_handle_ptr->probe_burst = rx_chan_params[index].probe_burst;

    rfcom_band = rfgsm_core_convert_rfband_to_rfcom(rx_chan_params[index].band);

    mc_ret_status = rfgsm_mc_prepare_rx_burst(core_data,
                                              index,
                                              rx_chan_params,
                                              rxd_chan_params,
                                              process_drx_burst);

    if(mc_ret_status == RFGSM_MC_SUCCESS)
    {
      mc_ret_status = rfgsm_mc_update_sawless_linearity_in_params(core_data->rfm_dev, 
                                                                  rx_chan_params[index].band,
                                                                  rx_chan_params[index].arfcn,
                                                                  rx_chan_params[index].high_lin_mode_active);
    }

    if(mc_ret_status != RFGSM_MC_SUCCESS){ break;}

    if(core_data->enable_rx_diversity) /* RXD Processing*/
    {
      if(rxd_chan_params != NULL)
      {

        /* This needs to be set here as it passed to device driver to retrieve scripts and gain parameters*/
        core_data->rfm_dev = core_data->rxd_rfm_dev;
        process_drx_burst = TRUE;

        mc_ret_status = rfgsm_mc_prepare_rx_burst(core_data,
                                                  index,
                                                  rx_chan_params,
                                                  rxd_chan_params,
                                                  process_drx_burst);

        if(mc_ret_status == RFGSM_MC_SUCCESS)
        {
          mc_ret_status = rfgsm_mc_update_sawless_linearity_in_params(core_data->rxd_rfm_dev, 
                                                                      rx_chan_params[index].band,
                                                                      rx_chan_params[index].arfcn,
                                                                      rxd_chan_params[index].high_lin_mode_active);
        }

        if(mc_ret_status != RFGSM_MC_SUCCESS){ break;}
      }
      else
      {
        mc_ret_status = RFGSM_MC_INVALID_PARAM;
        break;
      }
    }

    /*Reset the core data rfm dev to PRX device*/
    core_data->rfm_dev = prx_dev;
    
    /*DRX script will be populated along with PRX sciprt if RXD is enabled*/
    rfgsm_mc_populate_rx_burst_device_buffers( core_data,
                                               core_data->enable_rx_diversity,
                                               rx_chan_params[index].burst_type);

    mc_ret_status = rfgsm_mc_build_rx_burst_ccs_events(core_data,
                                                       gfw_rf_burst_event_sm_ptrs[index]);
    if(mc_ret_status != RFGSM_MC_SUCCESS){ break;}
  }

  if(mc_ret_status != RFGSM_MC_SUCCESS)
  {
    /* Update failing case in log buffer */
    RFGSM_DEBUG_MC_ERR_LOG_Q(rfgsm_mc_err_buffer_ptr, 
                             RFGSM_MC_SETUP_RX_BURST, 
                             core_data->rf_task, 
                             RFGSM_MC_CORE_API_FAILURE, 
                             (geran_test_mode_api_get_FN(core_data->as_id)),
                             ((core_data->rfm_dev)|(core_data->rxd_rfm_dev)),
                             num_rx_bursts );
  }
  /* Release core lock */
  rf_common_leave_critical_section(&core_data->rfgsm_core_handle_ptr->rfgsm_core_crit_sect);

  return mc_ret_status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function calls all the rfgsm_mc functionality to create the RF CCS for all the
  bursts requested from GL1.

  This function can be called from RF Task context from the rfgsm_cmd_processing dispatcher
  or iRAT scenarios.

  @param device                   : RF Multimode device
  @param num_rx_bursts            : Number of Rx/Monitor bursts required
  @param rx_chan_params           : Rx burst parameters for each burst i.e arfcn
  @param gfw_rf_burst_event_ptrs  : Pointer to an array of pointers to shared mem
*/
rfgsm_mc_status_type rfgsm_mc_build_rx_burst_ccs_events( rfgsm_core_data_type* core_data,
                                                         void *rx_burst_event_ptr)
{
  rfgsm_mc_status_type mc_ret_status = RFGSM_MC_SUCCESS;
  
  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();

  if(!rfgsm_core_build_rx_burst_ccs_events(core_data, rx_burst_event_ptr))
  {
    RF_MSG_1(MSG_LEGACY_ERROR,
              "RFGSM Core RX Burst CCS Event Building Failed: RX Diversity is :%d", 
              core_data->enable_rx_diversity);
    mc_ret_status = RFGSM_MC_CORE_API_FAILURE;
  }

  RFGSM_DEBUG_EN_Q( core_data->rfgsm_core_handle_ptr, //core handle
                    RFGSM_MC_BUILD_RX_BURST_CCS_EVENT,//api
                    core_data->rf_task,               //rf task
                    0,                                //command proc start qs
                    0,                                //arg1
                    0,                                //arg2
                    geran_test_mode_api_get_FN(core_data->as_id), //SW frame num
                    start_ustmr,                      //start time USTMR
                    rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                    start_pcycle,                     //start time pcycles
                    qurt_get_core_pcycles()-start_pcycle ); //duration pcycles

  return mc_ret_status;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function calls all the rfgsm_mc functionality to create the RF CCS for all the
  bursts requested from GL1.

  This function can be called from RF Task context from the rfgsm_cmd_processing dispatcher

  @param device           : RF Multimode device
  @param tx_burst_params  : Tx burst parameters for each frame
*/
rfgsm_mc_status_type rfgsm_mc_process_tx_burst( rfgsm_core_data_type *core_data,
                                                rfa_rf_gsm_tx_burst_type *tx_burst_params )
{
  /* Initialise variables */
  uint32 index = 0;
  rfgsm_power_profile_type pwr_profiles[RFGSM_MAX_TX_SLOTS_PER_FRAME];
  rfgsm_mc_status_type mc_ret_status = RFGSM_MC_SUCCESS;
  boolean gmsk_flag = TRUE;

  if ( tx_burst_params == NULL )
  {
    RF_MSG(MSG_LEGACY_ERROR,"tx_burst_params = NULL. Did not process Tx burst.");
    return RFGSM_MC_INVALID_PARAM;
  }

  /* Check number of Tx slots is not larger than max */
  if( tx_burst_params->num_tx_slots > RFGSM_MAX_TX_SLOTS_PER_FRAME )
  {
    RF_MSG_2(MSG_LEGACY_ERROR,
             "num_tx_slots %d too large. Max Tx slots is %d", 
             tx_burst_params->num_tx_slots, RFGSM_MAX_TX_SLOTS_PER_FRAME);

    /* Update failing case in log buffer */
    RFGSM_DEBUG_MC_ERR_LOG_Q(rfgsm_mc_err_buffer_ptr, 
                             RFGSM_MC_SETUP_TX_BURST, 
                             core_data->rf_task, 
                             RFGSM_MC_INVALID_PARAM, 
                             (geran_test_mode_api_get_FN(core_data->as_id)),
                             core_data->rfm_dev,
                             tx_burst_params->num_tx_slots );
	
    return RFGSM_MC_INVALID_PARAM;
  }

  /* Temporarily assign until core layer deprecation of core handle var */
  core_data->rfgsm_core_handle_ptr->buffer_id = core_data->triple_buffer_id;

  /*Initialise all the pwr profile data to zero*/
  memset(pwr_profiles, 0x0, sizeof(pwr_profiles));

  /* Fill in the power profile structure for the RF driver for each tx slot required */
  for (index = 0; index < tx_burst_params->num_tx_slots; index++)
  {
    pwr_profiles[index].power_level = tx_burst_params->tx_pwr_ctl_params[index].power_level;
    pwr_profiles[index].modulation = tx_burst_params->tx_pwr_ctl_params[index].modulation;
    /* sets GMSK flag to true if modulation is GMSK (else false for unknown or 8PSK)*/
    gmsk_flag &= (RF_MOD_GMSK == pwr_profiles[index].modulation);
    pwr_profiles[index].backoff[RFGSM_COEX_DESENSE_BACKOFF] = tx_burst_params->tx_pwr_ctl_params[index].backoff[RFGSM_COEX_DESENSE_BACKOFF];
    pwr_profiles[index].backoff[RFGSM_COEX_VBATT_BACKOFF] = tx_burst_params->tx_pwr_ctl_params[index].backoff[RFGSM_COEX_VBATT_BACKOFF];
  }
  /* store GMSK flag for forcing tuner manager to use_aol_tune_code when false*/
  core_data->rfgsm_core_handle_ptr->gmsk_modulation = gmsk_flag;

  mc_ret_status = rfgsm_mc_update_freq_error(core_data, tx_burst_params->tx_chan_params.freq_err, RFGSM_FREQ_ERR_TX_ONLY); 
  if(mc_ret_status != RFGSM_MC_SUCCESS){ return mc_ret_status;}

  /* Tune the UHF Tx synthesizers to the channel specified */

#if 0
  mc_ret_status = rfgsm_mc_tune_to_chan( core_data, 
                                         tx_burst_params->tx_chan_params.arfcn,
                                         RFM_TUNE_TX );
#endif

  mc_ret_status = rfgsm_mc_tune_to_tx_chan(core_data,tx_burst_params);

  if(mc_ret_status != RFGSM_MC_SUCCESS)
  {
    /* Update failing case in log buffer */
    RFGSM_DEBUG_MC_ERR_LOG_Q(rfgsm_mc_err_buffer_ptr, 
                             RFGSM_MC_SETUP_TX_BURST, 
                             core_data->rf_task, 
                             RFGSM_MC_CORE_API_FAILURE, 
                             (geran_test_mode_api_get_FN(core_data->as_id)),
                             core_data->rfm_dev,
                             tx_burst_params->num_tx_slots );  
    return mc_ret_status;
  }

  /* Configure the AM/AM and AM/PM tables for polar RF */
  mc_ret_status = rfgsm_mc_set_amam_profile( core_data,
                                             pwr_profiles,
                                             tx_burst_params->num_tx_slots,
                                             tx_burst_params->num_assigned_uplink_slots );
  if(mc_ret_status != RFGSM_MC_SUCCESS){ return mc_ret_status;}

  /* Setup the start, transition and stop buffers for SBI, GRFC, and RFFE necessary
   * for the Tx frame. */
  mc_ret_status = rfgsm_mc_setup_tx_burst( core_data,
                                           tx_burst_params->num_tx_slots,
                                           tx_burst_params->tx_burst_event_ptr );
  if(mc_ret_status != RFGSM_MC_SUCCESS){ return mc_ret_status;}

  return RFGSM_MC_SUCCESS;
}



/*----------------------------------------------------------------------------*/
/*
  @brief

  @details
*/
rfgsm_mc_status_type rfgsm_mc_cmd_dispatch(rfm_device_enum_type rfm_dev, int32 cmd, void *data)
{
  rfgsm_core_handle_type* rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!", FALSE);

  switch(cmd)
  {
  default:
    break;

  }

  return RFGSM_MC_SUCCESS;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Reading NV for GSM and populating the necessary variables. 

  @details
  Retrieves all the NV from GSM mode. Reads common and mode specific RF 
  configuration parameters from NV. This is called as part of the 
  LOAD_RF_NV function case where we need to read the calibration NV and write 
  them without having to reset.

  @param device: RF device
  @param caller_tcb_ptr: Calling task TCB pointer
  @param task_nv_wait_sig: Task signal to wait for when reading NV
  @param task_wait_func_ptr: Task's wait function to be called when reading NV
*/

boolean rfgsm_mc_reload_nv(rfm_device_enum_type rfm_dev,
                           rex_tcb_type *caller_tcb_ptr,
                           rex_sigs_type task_nv_wait_sig,
                           void (*task_wait_func_ptr)( rex_sigs_type ))
{
  uint8 flag = (uint8)FALSE;

  RF_MSG( MSG_LEGACY_HIGH, "rfgsm_mc_reload_nv" );

  /* free up all allocated buffers, reload NV allocates it again*/
  rfgsm_nv_deinit(rfm_dev);

  /* read GSM NVs and save in new allocated structures*/
  (void)rfgsm_nv_load_items(rfm_dev,
                            caller_tcb_ptr, 
                            task_nv_wait_sig, 
                            task_wait_func_ptr,
                            &flag);

  if (flag != (uint8)TRUE)
  {
    return FALSE;
  }
  /* re-compute all switch points based on new NV value and program Tx timing values to mdsp*/
  rfgsm_core_nv_init(rfm_dev);

  /* re-init NV ptrs in core handles*/
  rfgsm_core_handle_nv_init(rfm_dev);

  rfgsm_core_temp_comp_init(rfm_dev);

  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*
  @brief

  @details
*/
rfgsm_mc_status_type rfgsm_mc_get_timing_info( rfgsm_core_data_type *core_data, 
                                               rfgsm_timing_info_type *timing_info ) 
{
  /* Temporarily assign until core layer deprecation of core handle var */
  core_data->rfgsm_core_handle_ptr->buffer_id = core_data->triple_buffer_id;

  rfgsm_core_get_timing_info(core_data, timing_info, TRUE);

  return RFGSM_MC_SUCCESS;
}



/*----------------------------------------------------------------------------*/
/*
  @brief
  Do processing scheduled in a GSM idle frame.

  @details
  
  @param
  None 

*/
rfgsm_mc_status_type rfgsm_mc_do_idle_frame_processing(rfgsm_core_data_type *core_data)
{
  /* Temporarily assign until core layer deprecation of core handle var */
  core_data->rfgsm_core_handle_ptr->buffer_id = core_data->triple_buffer_id;

  rfgsm_core_do_idle_frame_processing(core_data);
  return RFGSM_MC_SUCCESS;
}

/*----------------------------------------------------------------------------*/
/*
  @brief

  @details
*/
boolean rfgsm_mc_update_sar_backoff(rfm_device_enum_type rfm_dev)
{
  rfgsm_core_handle_type* rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!", FALSE);

  rfgsm_core_set_tx_profile_update_flag(rfgsm_core_handle_ptr);
  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*
  @brief
  This function relays the pointer to GL1 structure containing information about IQ capture to rfgsm_core 
  @details
*/
boolean rfgsm_mc_log_iq_data(rfm_device_enum_type rfm_dev, GfwIqSamplesBuffer* data)
{
  rfgsm_core_log_iq_data(rfm_dev, data);
  return TRUE;
}



/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function will populate all the timing information related to GSM IRAT measurements. This
    timing information will be used by L1 scheduling the measurements during IRAT gap.
 
  @param rfm_meas_irat_info_type: data structure for all info to be returned
 
  @pre
  rfm_init() must have already been called.
  We should be in a certain technology.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did work
  correctly or not.
*/
rfm_meas_result_type rfgsm_mc_irat_info_get(rfm_meas_irat_info_type *irat_info_param)
{
  rfm_device_enum_type rfm_dev = RFM_DEVICE_0;
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  rfcom_mode_enum_type src_tech, target_tech;

  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();

  RF_NULL_CHECK_RTN(irat_info_param, "irat_info_param is NULL!", RFM_MEAS_COMMON_FAILURE);

  src_tech = irat_info_param->header.source_tech;
  target_tech = irat_info_param->header.target_tech;

  if ( target_tech == RFCOM_GSM_MODE)
  {
    rfm_dev = irat_info_param->header.target_param.device;
  }
  else if( src_tech == RFCOM_GSM_MODE)
  {
    rfm_dev = irat_info_param->header.source_param[0].device;
  }

  result = rfgsm_core_irat_info_get(irat_info_param);

  if( target_tech == RFCOM_GSM_MODE )
  {
    result &= rfgsm_core_irat_update_rx_timings(irat_info_param);
  }

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  if (rfgsm_core_handle_ptr == NULL)
  {
    RF_MSG_1(MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL for device %d !", rfm_dev);
  }
  else
  {
    RFGSM_DEBUG_EN_Q( rfgsm_core_handle_ptr,       //core handle
                      RFGSM_MC_MEAS_IRAT_INFO_GET, //api
                      RF_MEAS_CALL,                //rf task
                      0,                           //command proc start qs
                      0,                           //arg1
                      0,                           //arg2
                      geran_test_mode_api_get_FN(as_id), //SW frame num
                      start_ustmr,                 //start time USTMR
                      rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                      start_pcycle,                //start time pcycles
                      qurt_get_core_pcycles()-start_pcycle ); //duration pcycles
  }

  return(result);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function is called first when the measurement is requested. Each technology
    basically prepares for the measurement.
 
  @param rfm_meas_enter_param : This is a pointer that basically has a header that has
  the target technology and the source technology. Based on this header, the rest is casted as
  a tech specific pointer.
 
  @pre
  rfm_init() must have already been called.
  We should be in a certain technology.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did work
  correctly or not.
*/
rfm_meas_result_type rfgsm_mc_meas_enter( rfm_meas_enter_param_type *meas_enter_param)
{
  rfm_device_enum_type rfm_dev = RFM_DEVICE_0;
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS;
  rfcom_mode_enum_type src_tech = RFM_INVALID_MODE;
  rfcom_mode_enum_type target_tech = RFM_INVALID_MODE;

  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();

  src_tech = meas_enter_param->header.source_tech;
  target_tech = meas_enter_param->header.target_tech;

  if ( target_tech == RFCOM_GSM_MODE)
  {
    rfm_dev = meas_enter_param->header.target_param.device;
  }
  else if( src_tech == RFCOM_GSM_MODE)
  {
    rfm_dev = meas_enter_param->header.source_param[0].device;
  }

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);


  result = rfgsm_core_meas_enter(meas_enter_param);

  if (rfgsm_core_handle_ptr == NULL)
  {
    RF_MSG_1(MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL for device %d ", rfm_dev);
  }
  else
  {
    RFGSM_DEBUG_EN_Q( rfgsm_core_handle_ptr, //core handle
                      RFGSM_MC_MEAS_ENTER,   //api
                      RF_MEAS_CALL,          //rf task
                      0,                     //command proc start qs
                      0,                     //arg1
                      0,                     //arg2
                      geran_test_mode_api_get_FN(as_id), //SW frame num
                      start_ustmr,           //start time USTMR
                      rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                      start_pcycle,          //start time pcycles
                      qurt_get_core_pcycles()-start_pcycle ); //duration pcycles

    /* Drive the FSM - Not covering IRAT states until core states are proven */
    //rfgsm_core_fsm_drive(rfgsm_core_handle_ptr, MEAS_ENTER_IP);
  }
  return(result);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function is called to prepare the clean-up and start-up scripts.
 
  @param rfm_meas_setup_param : This is a pointer that basically has a header that has
  the target technology and the source technology. Based on this header, the rest is casted as
  a tech specific pointer.
 
  @pre
  rfm_meas_common_enter function must have been called.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did work
  correctly or not.
*/
rfm_meas_result_type rfgsm_mc_meas_build_scripts( rfm_meas_setup_param_type *meas_scripts_param)
{
  rfm_device_enum_type rfm_dev = RFM_DEVICE_0;
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  rfcom_mode_enum_type src_tech = RFM_INVALID_MODE;
  rfcom_mode_enum_type target_tech = RFM_INVALID_MODE;

  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();

  src_tech = meas_scripts_param->header.source_tech;
  target_tech = meas_scripts_param->header.target_tech;

  if ( target_tech == RFCOM_GSM_MODE)
  {
    rfm_dev = meas_scripts_param->header.target_param.device;
  }
  else if( src_tech == RFCOM_GSM_MODE)
  {
    rfm_dev = meas_scripts_param->header.source_param[0].device;
  }

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  result = rfgsm_core_meas_build_scripts(meas_scripts_param);

  if (rfgsm_core_handle_ptr == NULL)
  {
    RF_MSG_1(MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL! for device %d", rfm_dev);  
  }
  else
  {
    RFGSM_DEBUG_EN_Q( rfgsm_core_handle_ptr,       //core handle
                      RFGSM_MC_MEAS_BUILD_SCRIPTS, //api
                      RF_MEAS_CALL,                //rf task
                      0,                           //command proc start qs
                      0,                           //arg1
                      0,                           //arg2
                      geran_test_mode_api_get_FN(as_id), //SW frame num
                      start_ustmr,                 //start time USTMR
                      rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                      start_pcycle,                //start time pcycles
                      qurt_get_core_pcycles()-start_pcycle ); //duration pcycles
  }
  return(result);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function is called to exit the measurement mode. It is used to put the RF
    in the right state and do any clean-ups required.
 
  @param rfm_meas_setup_param : This is a pointer that basically has a header that has
  the target technology and the source technology. Based on this header, the rest is casted as
  a tech specific pointer.
 
  @pre
  rfm_meas_common_script_enter function must have been called.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did work
  correctly or not.
*/
rfm_meas_result_type rfgsm_mc_meas_exit( rfm_meas_exit_param_type *meas_exit_param)
{
  rfm_device_enum_type rfm_dev = RFM_DEVICE_0;
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  rfcom_mode_enum_type src_tech = RFM_INVALID_MODE;
  rfcom_mode_enum_type target_tech = RFM_INVALID_MODE;

  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();

  src_tech = meas_exit_param->header.source_tech;
  target_tech = meas_exit_param->header.target_tech;
  
  if ( target_tech == RFCOM_GSM_MODE)
  {
    rfm_dev = meas_exit_param->header.target_param.device;
  }
  else if( src_tech == RFCOM_GSM_MODE)
  {
    rfm_dev = meas_exit_param->header.source_param[0].device;
  }

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  result = rfgsm_core_meas_exit(meas_exit_param);

  if (rfgsm_core_handle_ptr == NULL)
  {
    RF_MSG_1(MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL for device %d! ", rfm_dev);  
  }
  else
  {
    RFGSM_DEBUG_EN_Q( rfgsm_core_handle_ptr, //core handle
                      RFGSM_MC_MEAS_EXIT,    //api
                      RF_MEAS_CALL,          //rf task
                      0,                     //command proc start qs
                      0,                     //arg1
                      0,                     //arg2
                      geran_test_mode_api_get_FN(as_id), //SW frame num
                      start_ustmr,           //start time USTMR
                      rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                      start_pcycle,          //start time pcycles
                      qurt_get_core_pcycles()-start_pcycle ); //duration pcycles

    /* Drive the FSM - Not covering IRAT states until core states are proven */
    //rfgsm_core_fsm_drive(rfgsm_core_handle_ptr, MEAS_EXIT_IP);
  }
  return(result);
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function is called to preconfig target WTR, if required.
 
  @param rfm_meas_setup_param : This is a pointer that has a header with target
   technology and the source technology info. Based on this header, the
   rest is casted as a tech specific pointer.
 
  @pre
  rfm_meas_common_enter function must have been called.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did work
  correctly or not.
*/
rfm_meas_result_type rfgsm_mc_pre_config( rfm_meas_setup_param_type *meas_scripts_param)
{
  rfm_device_enum_type rfm_dev = RFM_DEVICE_0;
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  rfcom_mode_enum_type src_tech, target_tech;

  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();

  RF_NULL_CHECK_RTN(meas_scripts_param, "meas_scripts_param is NULL!", RFM_MEAS_COMMON_FAILURE);

  src_tech = meas_scripts_param->header.source_tech;
  target_tech = meas_scripts_param->header.target_tech;

  if ( target_tech == RFCOM_GSM_MODE)
  {
    rfm_dev = meas_scripts_param->header.target_param.device;
  }
  else if( src_tech == RFCOM_GSM_MODE)
  {
    rfm_dev = meas_scripts_param->header.source_param[0].device;
  }

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  result = rfgsm_core_meas_pre_config(meas_scripts_param);

  if (rfgsm_core_handle_ptr == NULL)
  {
    RF_MSG_1(MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL for device %d!", rfm_dev);
  }
  else
  {
    RFGSM_DEBUG_EN_Q( rfgsm_core_handle_ptr,    //core handle
                      RFGSM_MC_MEAS_PRE_CONFIG, //api
                      RF_MEAS_CALL,             //rf task
                      0,                        //command proc start qs
                      0,                        //arg1
                      0,                        //arg2
                      geran_test_mode_api_get_FN(as_id), //SW frame num
                      start_ustmr,              //start time USTMR
                      rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                      start_pcycle,             //start time pcycles
                      qurt_get_core_pcycles()-start_pcycle ); //duration pcycles
  }
  return(result);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function disable second WTR after target tech measurements,
    in case if source tech and target tech are on different WTRs.
 
  @param rfm_meas_setup_param_type : This is a pointer that has a header with target
   technology and the source technology info. Based on this header, t
   he rest is casted as a tech specific pointer.
  

  @retval rfm_meas_result_type : an enum type denoting whether the function did work
  correctly or not.
 */
rfm_meas_result_type rfgsm_mc_build_reset_scripts(rfm_meas_setup_param_type *meas_scripts_param)
{
  rfm_device_enum_type rfm_dev = RFM_DEVICE_0;
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  rfcom_mode_enum_type src_tech = RFM_INVALID_MODE;
  rfcom_mode_enum_type target_tech = RFM_INVALID_MODE;

  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();

  src_tech = meas_scripts_param->header.source_tech;
  target_tech = meas_scripts_param->header.target_tech;
  
  if ( target_tech == RFCOM_GSM_MODE)
  {
    rfm_dev = meas_scripts_param->header.target_param.device;
  }
  else if( src_tech == RFCOM_GSM_MODE)
  {
    rfm_dev = meas_scripts_param->header.source_param[0].device;
  }

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  result = rfgsm_core_meas_cleanup(meas_scripts_param);

  rfgsm_core_disable_rffe(meas_scripts_param);
  
  if (rfgsm_core_handle_ptr == NULL)
  {
    RF_MSG_1(MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL for device %d", rfm_dev);  
  }
  else
  {
    RFGSM_DEBUG_EN_Q( rfgsm_core_handle_ptr,        //core handle
                      RFGSM_MC_BUILD_RESET_SCRIPTS, //api
                      RF_MEAS_CALL,                 //rf task
                      0,                            //command proc start qs
                      0,                            //arg1
                      0,                            //arg2
                      geran_test_mode_api_get_FN(as_id), //SW frame num
                      start_ustmr,                  //start time USTMR
                      rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                      start_pcycle,                 //start time pcycles
                      qurt_get_core_pcycles()-start_pcycle ); //duration pcycles
  }

  return(result);
}

#ifdef FEATURE_RF_ASDIV
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function route the set antenna request upon receiving a msg from RF apps task.
 
  @param
  rfm_dev        : Device ID
  uint8          : Antenna Position
 
  @retval None
*/
rfgsm_mc_status_type rfgsm_mc_set_antenna_req(rfgsm_core_data_type *core_data, uint8 ant_pos)
{
  /* Temporarily assign until core layer deprecation of core handle var */
  core_data->rfgsm_core_handle_ptr->buffer_id = core_data->triple_buffer_id;

  /* Transfer the request to rfgsm_core for processing*/
  rfgsm_core_set_antenna(core_data, ant_pos);  
  return RFGSM_MC_SUCCESS;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stops the callback that calls rfgsm_core_asd_rsp_trigger .It is called when there is no need to send abort request. 
*/
void rfgsm_mc_asd_rsp_check_stop( rfm_device_enum_type rfm_dev )
{
  rfgsm_core_asd_rsp_check_stop(rfm_dev);
}

#endif

/*----------------------------------------------------------------------------*/

static rfmeas_mc_func_tbl_type rfgsm_mc_meas_apis =
{
  rfgsm_mc_irat_info_get, /* rfmeas_mc_irat_info_get_fp*/
  rfgsm_mc_meas_enter, /* rfmeas_mc_enter_fp */
  rfgsm_mc_pre_config, /* rfmeas_mc_pre_config_fp */
  rfgsm_mc_build_reset_scripts, /* rfmeas_build_reset_scripts_fp */
  rfgsm_mc_meas_build_scripts, /* rfmeas_build_scripts_fp */
  rfgsm_mc_meas_exit, /* rfmeas_mc_exit_fp */
};

/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function registers the IRAT APIs for GSM with meas module.
 
  @param None

  @retval None
*/
rfgsm_mc_status_type rfgsm_mc_meas_init()
{
  if(!rfmeas_mc_register(RFCOM_GSM_MODE, &rfgsm_mc_meas_apis))
  {
    return RFGSM_MC_ERROR;
  }

  return RFGSM_MC_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function registers NULL for GSM with meas module.
 
  @param None

  @retval None
*/
rfgsm_mc_status_type rfgsm_mc_meas_deinit()
{
  /* Cmn function does not accept NULL fp table and therefore ignore error */
  (void)rfmeas_mc_register(RFCOM_GSM_MODE, (rfmeas_mc_func_tbl_type *)NULL);
  return RFGSM_MC_SUCCESS;
}




/*----------------------------------------------------------------------------*/

boolean rfgsm_mc_build_ip2_cal_script(rfgsm_core_data_type *core_data_ptr)
{
  boolean status = FALSE;
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_1;  //rfgsm_mc_retrieve_as_id_from_task(rf_task);

  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();

  if(core_data_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_mc_build_ip2_cal_script(), core_data ptr is NULL");  
    return FALSE;
  }

  status = rfgsm_core_build_ip2_cal_script(core_data_ptr);

  RFGSM_DEBUG_EN_Q( core_data_ptr->rfgsm_core_handle_ptr, //core handle
                    RFGSM_MC_DBG_START_IP2_CAL_REQUEST,   //api
                    core_data_ptr->rf_task,               //rf task
                    0,                                    //command proc start qs
                    0,                                    //arg1
                    0,                                    //arg2
                    geran_test_mode_api_get_FN(core_data_ptr->as_id), //SW frame num
                    start_ustmr,                          //start time USTMR
                    rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                    start_pcycle,                         //start time pcycles
                    qurt_get_core_pcycles()-start_pcycle ); //duration pcycles

  return status;
}


boolean rfgsm_mc_msm_init_ip2_cal(rfgsm_core_data_type *core_data_ptr, uint32 ip2_rxlm_idx, uint32 ip2_txlm_idx)
{

  if(core_data_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_mc_msm_init_ip2_cal(), core_data ptr is NULL");  
    return FALSE;
  }


  return (rfgsm_core_msm_init_ip2_cal(core_data_ptr,ip2_rxlm_idx,ip2_txlm_idx));
}


uint32 rfgsm_mc_convert_arfcn_to_tx_freq(rfgsm_band_type band, uint16 arfcn)
{
  rfcom_gsm_band_type rfcom_band = RFCOM_NUM_GSM_BANDS;

  if(band >= RF_MAX_BAND_TYPES)
  {

    RF_MSG_1( MSG_LEGACY_ERROR, "rfgsm_mc_convert_arfcn_to_tx_freq failed. Invalid band:%d", band );
    return 0xFFFFFFFF;
  }

  rfcom_band = rfgsm_core_convert_rfband_to_rfcom(band); 

  return (rfgsm_core_convert_arfcn_to_tx_freq( rfcom_band, arfcn ));

}



uint32 rfgsm_mc_convert_arfcn_to_rx_freq(rfgsm_band_type band, uint16 arfcn)
{
  rfcom_gsm_band_type rfcom_band = RFCOM_NUM_GSM_BANDS;

  if(band >= RF_MAX_BAND_TYPES)
  {

    RF_MSG_1( MSG_LEGACY_ERROR, "rfgsm_mc_convert_arfcn_to_rx_freq failed. Invalid band:%d", band );
    return 0xFFFFFFFF;
  }

  rfcom_band = rfgsm_core_convert_rfband_to_rfcom(band); 

  return (rfgsm_core_convert_arfcn_to_rx_freq( rfcom_band, arfcn ));

}

/*! 
 @brief
   This function reads the core handler to get the information about the edge core in use by GL1. This value is used by HAL layer to determine what is the EVMOD to use for the TXC.

 @return
   uint8 - denoting the edge core passed by L1.
*/

uint8 rfgsm_mc_edge_core(rfm_device_enum_type rfm_dev)
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  uint8 rfgsm_msm_edge_core=0;
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);


  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL for rfm_device:%d", rfm_dev);  
  }
  else
  {
    rfgsm_msm_edge_core = rfgsm_core_handle_ptr->edge_core;
  }

  return (rfgsm_msm_edge_core);
}
/*----------------------------------------------------------------------------*/
/*! 
 @brief
   This function process burst metrics passed in by GL1
 @return
   boolean success or failure
*/
boolean rfgsm_mc_process_burst_metrics(rfm_device_enum_type rfm_dev, 
                                       rfa_rf_gsm_rx_burst_metrics_type **metrics_ptr,
                                       uint8 num_entries)
{
  uint8 i = 0;
  boolean ret_val = TRUE;

  if (metrics_ptr == NULL)
  {
    return FALSE;
  }

  if ( rfgsm_debug_flags.sawless_override == TRUE )
  {
    return FALSE;
  }

  if ( num_entries > RFGSM_MAX_METRICS_ENTRIES )
  {
    return FALSE;
  }

  for ( i = 0; i < num_entries; i++ )
  {
    if ( metrics_ptr[i] == NULL)
    {
      RFGC_MSG_2(MSG_LEGACY_ERROR,
                 "Null metrics ptr encountered in array from GL1! index %d of %d",
                 i,
                 num_entries);
    }
    else if( rfgsm_core_process_burst_metrics(rfm_dev,
                                              metrics_ptr[i]->band,
                                              metrics_ptr[i]->arfcn,
                                              metrics_ptr[i]->rx_lvl_dbm,
                                              metrics_ptr[i]->snr,
                                              metrics_ptr[i]->jdet_value) == FALSE )
    {
      RFGC_MSG_5(MSG_LEGACY_ERROR,
                 "Failed to process burst metrics for band %d, arfcn %d, rx_lvl %d, snr %d, jdet %d",
                 metrics_ptr[i]->band,
                 metrics_ptr[i]->arfcn,
                 metrics_ptr[i]->rx_lvl_dbm,
                 metrics_ptr[i]->snr,
                 metrics_ptr[i]->jdet_value);
    }
  }

  return ret_val;
}

/*----------------------------------------------------------------------------*/
/*! 
 @brief
   This function will return SAWLess support for each band on each logical device
   into an 2dimensional array sized by RFM_MAX_WAN_DEVICES and (RFCOM_NUM_GSM_BANDS-1)
 @return
   boolean success or failure
*/
boolean rfgsm_mc_retrieve_sawless_support(boolean support_matrix[RFM_MAX_WAN_DEVICES][RFGSM_MAX_UNIQUE_RFCOM_BANDS])
{
  rfm_device_enum_type i = 0;
  rfcom_gsm_band_type j = 0;

  if (support_matrix == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_mc_process_burst_metrics: metrics_ptr is NULL!");
    return FALSE;
  }

  for ( i = 0; i < RFM_MAX_WAN_DEVICES; i++ ) 
  {
    for ( j = 0 ; j < (RFCOM_NUM_GSM_BANDS-1) ; j++ )
    {
      /*! @todo - This is currently not band specific, so the result will be the same 
        for all bands */
      support_matrix[i][j] = rfgsm_core_get_sawless_status( i );
    }
  }

  return TRUE;
}
/*----------------------------------------------------------------------------*/
/*! 
 @brief
   This function will update the High or Low lin state for an ARFCN\BAND combination
   so that it can be logged in GL1
 @return
   boolean success or failure
*/

rfgsm_mc_status_type rfgsm_mc_update_sawless_linearity_in_params(rfm_device_enum_type rfm_dev,
                                                                 rfgsm_band_type band,
                                                                 uint16          arfcn,
                                                                 uint16          *linearity_state)
{
  boolean hl_ll_state = FALSE;
  rfcom_gsm_band_type rfcom_band;

  if ( rfm_dev > RFM_MAX_WAN_DEVICES )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_mc_update_sawless_linearity_in_params: bad device passed: %d!", rfm_dev);
    return RFGSM_MC_INVALID_PARAM;
  }

  if ( rfgsm_core_get_sawless_status(rfm_dev) == FALSE )
  {
    /* Return with SUCCESS if not on SAWless device */
    return RFGSM_MC_SUCCESS;
  }

  if ( linearity_state == NULL )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_mc_update_sawless_linearity_in_params: NULL PTR!");
    return RFGSM_MC_SUCCESS;
  }

  rfcom_band = rfgsm_core_convert_rfband_to_rfcom(band);
  
  if ( rfgsm_core_is_arfcn_in_high_lin(rfm_dev, arfcn, rfcom_band, &hl_ll_state) == FALSE )
  {
    RFGC_MSG_3(MSG_LEGACY_ERROR, "Cannot get lin state for dev %d, arfcn %d, rfgsm_band %d", rfm_dev, arfcn, band);
    return RFGSM_MC_CORE_API_FAILURE;
  }

  if ( hl_ll_state == TRUE )  //...in HL mode for this ARFCN/BAND
  {
    *linearity_state = 0x1;
  }
  else  //...in LL mode for this ARFCN/BAND
  {
    *linearity_state = 0x0;
  }

  return RFGSM_MC_SUCCESS;
}

#ifdef FEATURE_TX_SHARING
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function calls rfgsm_mc functionality to create the RF CCS Events for all the
  Tx enable.

  @param core_data      : rfgsm core structure that stores infomation like Multimode device
  @param tx_enable_ptr : Pointer to shared mem for TX enable
  @retval               : True or False
*/
rfgsm_mc_status_type rfgsm_mc_build_tx_enable_ccs_events( rfgsm_core_data_type* core_data, rfm_device_enum_type rfm_dev)
{
  rfgsm_mc_status_type mc_ret_status = RFGSM_MC_SUCCESS;

 if(!rfgsm_core_build_tx_enable_ccs_events(core_data, rfm_dev))
  {
    RF_MSG_1(MSG_LEGACY_ERROR,
              "RFGSM Core TX Enable CCS Event Building Failed: %d", 
              0);
    mc_ret_status = RFGSM_MC_CORE_API_FAILURE;
  }

  return mc_ret_status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function calls rfgsm_mc functionality to create the RF CCS Events for all the
  Tx enable.

  @param core_data      : rfgsm core structure that stores infomation like Multimode device
  @param tx_enable_ptr : Pointer to shared mem for TX enable
  @retval               : True or False
*/
rfgsm_mc_status_type rfgsm_mc_build_tx_disable_ccs_events( rfgsm_core_data_type* core_data, rfm_device_enum_type rfm_dev)
{
  rfgsm_mc_status_type mc_ret_status = RFGSM_MC_SUCCESS;

 if(!rfgsm_core_build_tx_disable_ccs_events(core_data, rfm_dev))
  {
    RF_MSG_1(MSG_LEGACY_ERROR,
              "RFGSM Core TX Disable CCS Event Building Failed: %d", 
              0);
    mc_ret_status = RFGSM_MC_CORE_API_FAILURE;
  }

  return mc_ret_status;
}


#endif


/*----------------------------------------------------------------------------*/
/*
  @brief
  Get GSM CCS execution time

  @details

  @param
  device ID: rfm_dev
  rfgsm_ccs_exe_info_type: ccs_time_us
*/

uint32 rfgsm_mc_get_ccs_exe_us(rfm_device_enum_type rfm_dev, rf_buffer_intf *script_ptr) 
{
  uint32 mc_ccs_time_us;

  rfgsm_core_handle_type* rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!", 0);

  mc_ccs_time_us = rfgsm_core_get_ccs_exe_us(rfm_dev, script_ptr);

  return mc_ccs_time_us;
}

/*----------------------------------------------------------------------------*/

/*! 
 @brief
   This function programs the QFE scenario present in the atuner manager into the hardware

 @return
   boolean TRUE or FALSE
*/
boolean rfgsm_mc_program_qfe_scenario(void)
{
  boolean status;

#if 0
  rf_time_tick_type prof_tick;
  rf_time_type prof_time = 0; /* Time spent in function */
#endif

  status = rfgsm_core_antenna_tuner_program_qfe_scenario();

#if 0
  /*!@todo add time profiling using RFGSM_DEBUG_EN_Q */
  /* End Time Measurement */
  prof_time = rf_time_get_elapsed( prof_tick, RF_USEC );
  MSG_2(MSG_SSID_RF, MSG_LEGACY_MED, "rfgsm_core_antenna_tuner_program_qfe_scenario()::status=%d, prof_time=%d(us)", status, prof_time);
#endif

  return status;
}

/*!
  @file
  rf_cdma_mc.c
 
  @brief
  RF Driver's Common CDMA MC interface file.

  @details
  This file defines the Common CDMA Main Control interfaces.
 
  @addtogroup RF_CDMA_COMMON_MC
  @{
*/

/*==============================================================================

  Copyright (c) 2010 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/src/rf_cdma_mc.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/31/16   rs      Removed excessive F3 msgs
03/16/16   vr      1X2G QTA 
11/17/15   vr      Added dynamic txr_iq_dac setting update support
07/08/15   vr      Dev intf changes for MSM8909 + WTR2955
02/02/15   vr      Enabled the AsDiv Abort API
01/27/15   vc      Propogating immediate flag for FTM AsDiv support
01/23/15   vr     Modified the condition in the set antenna position API to 
                   return TRUE only if callback is NULL and also reqeusted 
	           config is same as prev one
12/08/14   spa     Add missing ASDiv featurization in wakeup rx helper
12/01/14   as/spa  Protected tuner configuration under ASDiv lock
11/13/14   spa     Get coupler port based on XSW position
11/12/14   spa     Change tx_nv_path to be per band per logical device
10/31/14   APU     Change param in rfc_update_alt_port_mapping() from 
                   rfc_band_type to int. 
10/23/14   spa     Query for xsw script settings in Rx wakeup
10/21/14   yb      Added priority configuration to config Tuners during CRAT scenarios
10/21/14   px      Added RFM Device support for PAPM
10/20/14   zhw     Use seperate buffer for QTA tune back
10/11/14   pk      OFT build compiler error fix
09/03/14   zhw/dyc Use MEAS_START_UP buffer for irat rx wakeup.
10/08/14   pk      Fix for RevB ACPR Performance issue (avoid DPD WTR setting)
10/06/14   zhw     Call wtr sleep script before rx_stop during retune
09/19/14   spa     Check for null antenna tuner token pointer 
09/16/14   spa     Call disable Rx/Tx WTR APIs for previous band/chan in retune
09/11/14   spa     WxEfication support for wakeup Tx
08/14/14   sbo     Create Tx Tuneback scripts
08/25/14   spa     Fix callflow in get RF on/off scripts API
07/30/14   zhw     IRAT alt rx path support
07/29/14   spa     Use Scripted mode for Wakeup Tx
07/18/14   dr      [RUMI] Jolokia Rumi bringup changes
07/17/14   spa     Added API rf_cdma_mc_update_alt_rx_path
06/28/14   fh      Added api for cleanup script
06/24/14   sty     KW fix to not de-ref null pointer
06/19/14   spa     Added SSMA callback API, API for dyn notch update triggering
05/23/14   pk      Fix for Tuner scerio stuck at zero
06/11/14   JJ      Get bandwidth for PA device
06/05/14   zhw     Enable ASD compilation
06/03/14   zhw     Remove hard-coded mode & band in sleep/wakeup callflow
05/29/14   zhw     Correct Debug F3
05/19/14   pk      Moved QPOET Standup script generation during rx_wkaeup into
                   IRAT mode check
05/14/14   APU     Suppy CL tuner with valid TxLM handle.
05/13/14   spa     Check for NULL event handle before calling config devices
05/05/14   sn      Added ASM sleep wakeup callflow
04/30/14   sn      updated PA interface
04/11/14   APU     Enabled AsDIV.
04/09/14   yb      Added support for coupler
04/09/14   sn      Updated PA interface
04/07/14   zhw     FED based C2K HDET support
04/07/14   spa     KW error fixes
04/04/14   spa     Remove "use_default_btf" flag
04/01/14   spa     Update return flag in get btf settings
03/31/14   zhw     Move ASM before PA config during Tx tune
03/10/14   as/spa  Added callback function as part of antenna switch
03/26/14   spa     Added API rf_cdma_mc_get_btf_delay to return BTF delays
03/19/14   zhw     Add SSBI support in rf_cdma_mc_do_immediate_script_write
03/18/14   APU     1> Have DM do the buffer deallocation.
                   2> Centralize allocation and association to 1 place.
03/12/14   APU     Split modem enter and exit into seperate rx and tx modem
                   enter and exit.
02/26/14   zhw     Init ASD position if radio is not active
02/24/14   cd      Set QPOET to max bias during Tx enable
02/03/14   cc      Fix rf_cdma_update_agc crash when it's called
01/21/14   zhw     FEATURE_QPOET_STANDBY_FOR_RX_SWITCH support
01/20/14   spa     Add API for immediate LNA writes in cal (debug only)
01/15/14   spa     Remove append_tx_on flag , hack is no longer required
01/09/14   spa     Use immediate writes for wakeup tx
01/09/14   zhw     CDMA NV loading memory leak fix
01/08/14   JJ      Removed transaction settings update
12/11/13   zhw     CDMA Tuner AOL Support
12/06/13   JJ      use script write for wakeup tx and remove immedate-write flag
12/02/13   sty     Minor changes to rf_cdma_mc_configure_front_end_for_retune
11/26/13   JJ      Spilt retune script to two and do immeidate write to both
11/21/13   JJ      Change to immediate write for wakeup_tx
08/15/13   zhw     Clean up CCS event handle right after TX RF on/off config
11/15/13   JJ      Change part of re-tune to immediate write
10/28/13  zhw/fh   Remove redundant PA on which cause PA turn on too early
10/03/13   zhw     Allow RF to tune to an invalid channel
11/18/13   sty     removed use of skip_direct_writes for direct writes - this is
                   effective only when RFLM API for rx_ and tx_tune are stubbed
                   out. This was a temp change put in during birngup when CCS was
                   not functional.
11/13/13   APU     Call seperate IRAT meas API as supposed to normal Rx tune API.
10/30/13   APU     Temp support for retune for D3925.
10/01/13   JJ      Fix a typo when calling rf_cdma_mdsp_configure_devices()
09/25/13   JJ      Add skip rf_cdma_mc_configure_rx_on_off flags
09/19/13   APU     MC changes to support DM.
09/04/13   zhw     Add Tuner AsDiv support
09/04/13   APU     Create a non 0 size buffer for RFFE LNA.
08/16/13   APU     Fixed a bug in rf_cdma_find_channel_validity_with_subclass(). 
08/15/13   Saul    SB. Made rf_state 1x/hdr specific.
08/01/13   APU     For IRAT use EVENT_IRAT_TUNE_IN ID instead of STATIC event ID.
07/24/13   zhw     Always use script mode for AsDiv comoon config
07/18/13   zhw     Add AsDiv Support for setting GRFC switches
07/15/13   zhw     Remove dev_status_w in mc layer call flow
07/15/13   aro     Temporarily disabled CDMA ASDiv
07/11/13   spa     Move APIs for mem corruptions sanity check to rf_cdma_data
07/10/13   spa     Off target fix
07/10/13   spa     Reduce F3 levels and remove duplicate calls (RF-FWRSP)
07/08/13   spa     Free semaphore if message sending fails
07/05/13   zhw/sty Clean up rf_cdma_data status mutable pointers
07/05/13   zhw     Tuner CL NV support
07/03/13   spa     Removed misleading F3s in Rx on/off API
07/02/13   spa     Use 'is_irat_mode' flag to determine scripting mode
07/01/13   APU     Call RX_STOP from 1 place and remove 
                   rf_1x_cleanup_post_measurement() because we use sleep when 
                   call meas_exit()
06/27/13   spa     Add 'is_irat_mode' flag to APIs to check if API call is in 
                   IRAT or non IRAT scenarios 
06/21/13   zhw     Tuner CL support. Added tx diable which clears timer for CL
06/21/13   zhw     Tuner CL support. Added RL meas call back registration
06/20/13   spa     Enable Discontinuous Rx (DLNA) feature
06/16/13   zhw     Added tech mc API for Antenna Switch Diversity support
06/12/12   APU     Added support to init iRAT specific entities from MC.
06/14/13   spa     Updated tuner call flow for retune in RXTX state
06/14/13   spa     Use CDMA wrapper object for antenna tuner
06/13/13   zhw     Added callback func and data for Tuner CL support
06/10/13   cd      Add support to clean-up Rx AGC LNA SM events
06/05/13   spa     Fix MDSP path being sent in for Rx on/off scripting
06/05/13   APU     Removed tx_static_nv_cal_ptr from helper_for_retune() APIs.
05/30/13   spa     Mainlined "use_new_fwrsp", use new RF dispatch implementation 
05/29/13   spa     Fixed order of GRFC script retrieval in retune API
05/29/13   spa     Added delay param to APIs for getting GRFC scripts
05/29/13   spa     Added support to get Rx On/Off scripts
05/29/13   spa     Renamed configure_on_off to configure_tx_on_off
05/28/13   JJ      Clean un-necessary cdma sys_band to rf_band_type conversion
05/24/13   APU     Eliminated unwanted event information structures.
05/22/13   APU     Print prof_ant_tuner_t_val to avoid compiler warning KW fixes.
05/22/13   APU     Print prof_ant_tuner_t_val and moved it inside #ifdef 
                   to avoid compiler warning.
05/20/13   APU     Made RFFE APIs common to HDR and 1X.
05/20/13   jmf     xPT IQ Capture ET Path Delay (+API change to return samp_rate)
05/06/13   spa     Set QPOET bias before PA writes in wakeup tx scripting
04/24/13   sty     Do not check for invalid band/chan during cal
04/24/13   sty     Added call to rf_cdma_get_rf_onoff_settings() for rf_off 
                   settings
04/22/13   APU     Consider the return value of 
                   rf_cdma_mc_is_band_chan_sub_class_supported()
04/19/13   APU     Neglect the return value of 
                   rf_cdma_mc_is_band_chan_sub_class_supported() until the test
                   trees are fixed.
04/18/13   sty/fh  Added SSBI retune rf ON/OFF support
04/17/13   APU     Added RFC check for 
                   rf_cdma_mc_is_band_chan_sub_class_supported()
04/15/13   APU     Added band chan validity for spreading rate 1 as per the 
                   standard.
04/11/13   ndb     Remvoed the unused header file 
04/10/13   ndb     Removed the unused Qtuner Nikel Functions
04/03 13   sn      Cleanup PA interface
03/28/13   zhw     Populate PA ON/OFF script for RFFE PA in RF ON/OFF event
03/22/13   sr      changes to PAPM device interface.
03/13/13   aro     Moved NV cleanup to CDMA
03/13/13   aro     Moved iC semaphore cleanup to ic Module
03/11/13   aro     Added cdma_deinit function to cleanup CDMA
03/07/13   sr      changed the "pwr_tracker" name to "papm".
03/07/13 sty/Saul  Fixed hal bus disable bug where it was not disabled if rx 
                   wake up failed.
02/25/13   zhw     Put rf_state back to sleep during process wakeup rxfailure
02/25/13   zhw     Added MC CCS event cleanup API
02/22/13   aro     Migrate XPT functions to common EPT module
01/28/13   spa     Remove refs to device_scripting flag, discontinue use of
                   immediate writes in wakeup/sleep Rx/Tx scripting sequences 
01/28/13   zhw     RF front end device wakeup/sleep API refactor
01/25/13   spa     Made F3 level medium in wakeup tx (for not putting pa in low)
01/24/13   spa     Remove call to force PA into Low state in wakeup Tx 
01/16/13   zhw     Use correct mapping of PA gain state to gain range from NV
01/15/13   cd      Add support for CDMA ASM aggregator functionality
01/12/13   cd      RxAGC LNA SM event configuration support
01/10/12   zhw     Move away from using common PA device state
01/08/13   shb     Check if device provides SSBI RF_ON script else use GRFC 
                   RF_ON script
01/04/13   sty     KW fix to use correct band type
12/27/12   adk     Moved rf_cdma_antenna_tuner_init() to rf_cdma_mc_init()
12/20/12   zhw     Remove call to config and on_off_gain in PA sleep callfow
12/19/12   Saul    CDMA. TX AGC XPT Dev. Use xpt cfg msg in online.
12/12/12   zhw     Integrate GRFC PA driver with CDMA
12/11/12   hdz     Enable TX/PA warm-up time and turn off delay time
12/09/12   zhw     Uses PA on_off_gain API to turn PA on/off
12/07/12   spa     Enable/Disable QPOET in wakeup/sleep Tx, with QPOET support
                   available in FW, this need not be RFcal specific anymore
12/04/12   aro     Added HDR support for RF-FW synchronization
11/26/12   adk     Dime RFC updates to get tuner device instance
11/21/12   spa     Enable/Disable QPOET during rfcal and bypass during online
11/12/12   sty     Do not un-vote in rf_cdma_process_wakeup_failure
                   if rf_state is not RF_CDMA_STATE_RX_PREP
11/09/12   zhw     Compiler warning fix
11/07/12   zhw     Support for RF device scripting (PA/ASM/QPOET)
11/07/12   jfc     Move the rf hal bus vote handle into rf_cdma_data 
11/06/12   zhw     PA On/Off script in RF On/Off event for non HCPA
11/05/12   zhw     Support for PA On/Off GRFC script in RF On/Off event
11/02/12   jfc     Replace rf_hal_enable_rfcmd_app with rf_hal_bus_enable 
10/31/12   adk     Merged antenna tuner code from Nikel.
10/26/12   spa     Function pointer implementation for SSBI based rf on/off
10/18/12   spa     Added rf_cdma_mc_configure_on_off for Rf on/off 
10/17/12   APU     Fixed compiler warnings
10/05/12   sty     Moved IRAT APIs to rf_cdma_meas.c
10/05/12   av      Expanded rf task architecture to include more tasks. 
10/04/12   sr      Added IRAT APIs. 
09/21/12   sty     Deleted device pointer from  rf_cdma_process_wakeup_failure
09/21/12   sty     Moved call to rf_hal_enable_rfcmd_app() in 
                   rf_cdma_process_wakeup_failure()
07/26/12   Saul    COMMON. Semaphores now initialized in common MC.
07/23/12   Saul    CDMA. Now using semaphore APIs from RF COMMON.
05/29/12   APU     Added rfm_cdma_is_tunable_on_device() to 
                   rf_cdma_mc_is_band_chan_supported() 
05/16/12   hdz     Added  rf_cdma_data_sanity_check_init() and 
                          rf_cdma_data_sanity_check() 
05/15/12   hdz     Move rf_cdma_apps_task_dispatch to rf_cdma_apps_task.c
05/14/12   aro     Updated HDR FW messages to use semaphores from SemPool for
                   RF-FW synchronization
05/11/12   spa     Added rf_cdma_mc_is_band_chan_supported() definition 
05/09/12   aro     Initialization for Semaphore
05/08/12   aro     Support for unique sequence number generation to be used for
                   RF-FW message synchronization
05/04/12   sb      Removed KW warning
05/01/12   zhaow   Updated rf_cdma_ic_stop() 
04/27/12   zhaow   Updated rf_cdma_ic_stop()
04/18/12   hdz     Added rf_cdma_apps_dispatch() and its supported code 
04/02/12   hdz     Added IC_stop and IC_start in rf_cdma_update_agc() 
03/30/12   hdz     Added check if rf_cdma_update_rx_agc() failed, no semaphore wait
03/29/12   sty     Added rf_cdma_process_wakeup_failure()
03/26/12   hdz     Added rf_cdma_update_agc()
03/22/12   aro     Deleted cdma_rf_task file
03/21/12   spa     Added max device check in rf_cdma_get_mutable_device_status()
03/21/12   aro     Added check for running state of RF_FWRSP_TASK
03/16/12   hdz     Rename temp_comp.is_combined_with_freq_comp to 
                   temp_comp.init_trigger 
01/04/12   hdz     Added init for hkadc_critcal_sect 
12/13/11   aro     Klocwork Fix
12/13/11   aro     Check for RFM APIs to see if all required tasks are running
12/07/11   aro     Deleted unused CDMA files
12/02/11   hdz     Renamed rf_1x_temp_comp_xxx to rf_cdma_temp_comp_xxx
11/21/11   aro     Mainlined debug variable to use new RF task dispatch
11/14/11   aro     Mainlined iC task debug flag
11/10/11   hdz     Init IsHKADCbusy to FALSE 
11/10/11   aro     Added check to bypass old dispatch init
10/27/11   aro     Added call to iC task to signal the task to start
10/24/11   hdz     Init IsHKADCbusy to TRUE 
10/20/11   hdz     Included rf_1x_temp_comp.h 
10/20/11   hdz     Added call to 
                   - rf_1x_dispatch_init() and
                   - rf_1x_temp_comp_init(); 
10/06/11   sty     Unconditionally initialize critical section in 
                   rf_cdma_mc_init()
09/20/11   aro     Added code to set the CDMA lock to Common RFM Lock
08/15/11   shb     Added power limiting module initialization
08/03/11   shb     Renamed hdet_data to hdet in rf_cdma_data_status_type
07/26/11   aro     Updated HDET init to pass logical device
07/13/11   sty     Deleted FEATURE_RUMI3_BRINGUP code
06/26/11   sty     Added calls to init HDET
06/20/11   aro     Changed function signature of intelliceiver init to pass
                   the intelliceiver data and logical device param
05/31/11   aro     Removed read_cdma_nv debug flag
05/23/11   sty     Check for return status in rf_cdma_register_msgs_to_rf_task
05/19/11   sty     Removed debug flag
04/22/11   aro     Removed ADC clock manager Init
04/21/11   aro     Updated CDMA Data interface for RF Calibraiton/Init
04/21/11   aro     Moved ADC Clock Manager to HAL
04/21/11   aro     Removed Critical Section Init to move to Core
04/21/11   aro     Renamed LOCKs function to move to Common area
04/13/11   sty     Changes for RUMI3 bringup
03/08/11   aro     Added call to initializa critical section
02/17/11   aro     Removed RFM Device Check to replace with RFM Mode check
02/17/11   sty     Renamed rfc_card_cdma.h to rfc_cdma.h
02/17/11   aro     Renamed intelliceiver function
01/31/11   aro     [1] Renamed CDMA get RF WU time function
                   [2] Added function to query Max device supported
01/25/11   sty     Fixed bug in checkin 
01/25/11   sty     Added rf_cdma_mc_get_warmup_time
01/18/11   sty     Added 1x and HDR MC init
01/13/11   aro     Added 1x and HDR MDSP init 
01/08/11   aro     Changed @ingroup to @addtogroup
01/05/11   aro     Moved @defgroup to rf_cdma_documentation.h
12/20/10   aro     Removed including of rfcom.h
12/20/10   aro     Added Loop to init all devices
12/17/10   aro     Added CDMA init function
12/16/10   aro     Doxygen Update
12/16/10   aro     Initial version

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rfm_internal.h"
#include "rfm_device_types.h"
#include "rfm_mode_types.h"
#include "rfcommon_msg.h"
#include "rf_cdma_mc.h"
#include "rf_cdma_data.h"
#include "rf_cdma_mdsp.h"
#include "rf_cdma_msm.h"
#include "rf_cdma_intelliceiver.h"
#include "rf_cdma_hdet.h"
#include "rf_cdma_power_limiting.h"
#include "rf_cdma_nv.h"
#include "rf_cdma_ic_task.h"
#include "rf_fwrsp_task.h"
#include "rf_cdma_rx_agc.h"
#include "rf_cdma_tx_agc.h"
#include "rfcommon_core_device_manager.h"
#include "rf_cdma_apps_task.h"
#include "rf_apps_task.h"
#include "rfm_cdma_band_types.h"
#include "rf_cdma_utils_freq.h"
#include "rf_hal_common.h"
#include "rf_hal_rffe.h"
#include "rf_cdma_constants.h"
#include "rf_cdma_meas.h"
#include "rfdevice_cdma_interface.h"
#include "rf_cdma_utils.h"
#include "rfcommon_asdiv_manager.h"
#include "modem_mem.h"
#include "rfmeas_mdsp.h"


#ifdef  FEATURE_CDMA1X
#include "rf_1x_mc.h"
#include "rf_1x_temp_comp.h"
#endif

#ifdef  FEATURE_HDR
#include "rf_hdr_mc.h"
#include "rf_hdr_temp_comp.h"
#endif
#include "rf_task_common_services.h"

/* PA and ASM */
#include "rfdevice_pa_intf.h"
#include "rfdevice_asm_intf.h"
/*include for QPOET set bias, remove when updated*/
#include "rfdevice_papm_intf.h"
#include "rfdevice_coupler_intf.h"
#include "rfdevice_cmn_intf.h"

#include "rf_hdr_mdsp.h"
#include "rf_1x_mdsp.h"
#include "stringl.h"

#include "rfcommon_concurrency_manager.h"
#include "rfcommon_asdiv_manager.h" /* To query for XSW position */
/*============================================================================*/
/*!
  @name Main Control Interface

  @brief
  This section holds all Main Control Interfaces to be used by RFM layer.
*/
/*! @{ */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Immediate write for RFFE script

  @param rf_script
  The generic script genearted for RFFE device

  @return
  Flag indicating whether CDMA driver Initialization is successful
*/
static void
rf_cdma_mc_do_immediate_script_write
(
  rf_buffer_intf *rf_script
)
{
  rf_hal_bus_rffe_type* rffe_script = NULL;
  const rf_hal_bus_resource_script_type* resource_ptr = NULL;
  uint8 num_of_rffe_trans = 0;
  rf_hal_bus_resource_script_settings_type settings;

  /* *************Do SSBI writes for the script******************** */
  /* Set the extra settings */
  memset(&settings, 0, sizeof(rf_hal_bus_resource_script_settings_type));

  resource_ptr = rf_buffer_get_script_ptr(rf_script, RF_HAL_BUS_RESOURCE_SSBI);

  if( resource_ptr != NULL && resource_ptr->num_trans > 0 )
  {
    RF_MSG_1( RF_HIGH, "Executing SSBI Scripts : num_transactions=%d",
              resource_ptr->num_trans);
    rf_hal_issue_script_generic(resource_ptr, &settings);
  }

  /* *************Do RFFE writes for the script******************** */
  /* Set the extra settings */
  memset(&settings, 0, sizeof(rf_hal_bus_resource_script_settings_type));

  resource_ptr = rf_buffer_get_script_ptr(rf_script, RF_HAL_BUS_RESOURCE_RFFE);

  if( resource_ptr != NULL && resource_ptr->num_trans > 0 )
  {
    rffe_script = (rf_hal_bus_rffe_type*)&(resource_ptr->script_ptr.rffe[0]);
    num_of_rffe_trans = resource_ptr->num_trans;
    RF_MSG_1( RF_HIGH, "Executing RFFE Scripts : num_transactions=%d",
              num_of_rffe_trans);
    rfhal_rffe_execute_buf_script( rffe_script,resource_ptr->num_trans,
                                   &settings);
  }

  /* ************** GRFC writes for the script *********************/
  memset(&settings, 0, sizeof(rf_hal_bus_resource_script_settings_type));
  resource_ptr = rf_buffer_get_script_ptr(rf_script, RF_HAL_BUS_RESOURCE_GRFC);

  if( resource_ptr != NULL && resource_ptr->num_trans > 0 )
  {
    RF_MSG_1( RF_HIGH, "Executing GRFC Scripts : num_transactions=%d",
                        resource_ptr->num_trans);

    rf_hal_issue_script_generic(resource_ptr, &settings);
  }
  /* *********************************************************** */
}/*rf_cdma_mc_do_immediate_rffe_write*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Common CDMA Driver Initialization

  @details
  This function will configure all internal CDMA (1x and HDR) driver state to a 
  known value. Any previous configuration information is lost.  Internal CDMA 
  driver sub-modules are also returned to their initial state.  The CDMA Driver 
  Initialization process is hardware independent, and assumes that the common 
  driver initialization routines have restored the hardware state to a 
  configurate that is consistent with power-on-reset.  This relieves the 
  technology specific driver from worrying about the hardware configuration,
  and eliminates redundant hardware resets that might be applied by each 
  technology as they re-initialize.  CDMA specific NV is re-loaded, 
  incoroporating any NV changes that have been written since the driver was
  last initialized.
 
  This function will execute only if the tech independent common init has
  completed successfully.
 
  @param common_init_successful
  Flag to indicate whether Tech independent Common Initialization was 
  successful

  @param caller_tcb_ptr
  TCB of the Task to used during NV Load
  
  @param task_nv_wait_sig
  Signal to Wait for NV LOad
  
  @param task_wait_func_ptr
  Pointer to the function to wait on.

  @return
  Flag indicating whether CDMA driver Initialization is successful
*/
boolean
rf_cdma_mc_init
(
  boolean common_init_successful,
  rex_tcb_type *caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (*task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean init_status = TRUE; /* Variable to track if CDMA Init passed */
  rfm_device_enum_type dev_idx; /* Variable for Device Loop */
  rf_cdma_data_status_type *dev_status_w; /* Device Status Write Pointer */
  rf_cdma_ic_data_type *ic_data; /* Intelliceiver Data Pointer */
  rf_cdma_hdet_data_type *hdet_data; /* HDET data Pointer */
  rf_cdma_plim_data_type *plim_data; /* Power Limiting Module data ptr */
  rf_cdma_temp_comp_data_type *temp_comp_data;  /* temp compensation data pointer */
  const rfc_cdma_logical_device_params_type *logical_dev;
  
  /* Initialize CDMA Locks as the FIRST step in MC init for CDMA. This is to
     prevent cases where rfm_init may fail, but other modules call RFM APIs.
     Initializing the critical section for RF-CDMA will prevent null-pointer
     crashes in these scenarios.
  */
  rf_cdma_set_lock_data(rfm_get_lock_data());
  
  /* Return if Common Initialization has fails */
  if ( common_init_successful == FALSE )
  {
    return FALSE;
  }

  /* PART I : Set all global CDMA RF Driver state to a known configuration */
  /*-----------------------------------------------------------------------*/

  init_status &= rf_cdma_init_device_status();

  /* PART II : Call Initialization routine for CDMA Drive Sub Component. */
  /*---------------------------------------------------------------------*/

  /* Signal RF iC Task to start */
  rf_cdma_ic_signal_to_start_task();

  /* Set all global CDMA RF Driver state to a known configuration */
  
  for ( dev_idx = RFM_DEVICE_0; dev_idx < RFM_MAX_DEVICES; dev_idx++ )
  {
    /* Get Read-only and Writable Device Status */
    dev_status_w = rf_cdma_get_mutable_device_status( dev_idx );

    /* Get Logical Device Parameter */
    logical_dev = rfc_cdma_get_logical_device_params( dev_idx );

    if ( dev_status_w != NULL )
    {
      /* Get Intelliceiver Data */
      ic_data = rf_cdma_get_mutable_intelliceiver( dev_idx );

      /* get pointer to HDET data for the device */
      hdet_data = rf_cdma_get_mutable_hdet( dev_idx );

      /* Get Power Limiting Data */
      plim_data = rf_cdma_get_mutable_plim( dev_idx );

      /*  Get pointer to 1x temp comp Data  */
      temp_comp_data = rf_cdma_get_mutable_temp_comp( dev_idx );

      /* HKADC is not in use initially */
      rf_cdma_data_set_isHKADCbusy( dev_idx, FALSE );
      
      /* Pout should NOT be skipped by default */
      rf_cdma_set_pout_comp_skip ( dev_idx , FALSE ) ;

      #ifndef T_RUMI_EMULATION
      /* DLNA support */
      rf_cdma_data_set_dlna_support(dev_idx);
      #endif
      init_status &= rf_cdma_data_sanity_check_init( dev_idx ); 

      /* Intelliceiver, Power Limiting module & HDET initialization */
      if ( logical_dev != NULL )
      {
        /* Check if IC data is not NULL*/
        if (ic_data != NULL )
        {
        /* Initialize Intelliceiver */
        init_status &= rf_cdma_ic_init( dev_idx, logical_dev, ic_data );
        }
        else
        {
          RF_MSG_1( RF_ERROR,"rf_cdma_mc_init: NULL IC data for device %d" ,
                    dev_idx );
        }

        /* Check if PLIM data is not NULL*/
        if ( plim_data != NULL )
        {
        /* Initialize Power Limiting module */
        init_status &= rf_cdma_plim_init( dev_idx, logical_dev, plim_data);
        }
        else
        {
          RF_MSG_1( RF_ERROR,"rf_cdma_mc_init: NULL Plim data for device %d" ,
                    dev_idx );
        }

        /* Check if HDET data is not NULL*/
        if ( hdet_data != NULL )
        {
        /* Initialize HDET */
        init_status &= rf_cdma_hdet_init( dev_idx, logical_dev, hdet_data );
        }
        else
        {
          RF_MSG_1( RF_ERROR,"rf_cdma_mc_init: NULL HDET data for device %d" ,
                    dev_idx );
        }

        /* Check if Temp Comp data is not NULL*/
        if ( temp_comp_data != NULL )
        {
        /* Initialize Temp Compensation */
          init_status &= rf_cdma_temp_comp_init( dev_idx, logical_dev,
                                                 temp_comp_data );

          /* Initialze to TRUE to enable freq comp and temp comq together*/
          temp_comp_data->init_trigger = TRUE;
        }
        else
        {
          RF_MSG_1( RF_ERROR,"rf_cdma_mc_init: NULL temp comp data for device "
                    "%d" , dev_idx );
        }

          /* Initialize Mutex lock for hkadc critical section - 1 per device */
        init_status &= 
          rf_common_init_critical_section( &dev_status_w->hkadc_critical_sect );
      }

    }/* if ( dev_status_w != NULL ) */
    else
    {
      RF_MSG_1( RF_ERROR,"rf_cdma_mc_init: NULL data for device %d" ,dev_idx );
    }/*if ( dev_status_w == NULL )*/
  }

  /* Load CDMA NV Data */
  init_status &= rf_cdma_nv_load( caller_tcb_ptr, task_nv_wait_sig,\
                                  task_wait_func_ptr);

  /* Register RF APPS CDMA dispatcher*/
  rf_apps_dispatch_register( rf_cdma_apps_dispatch, RF_APPS_1XHDR_ID );
 
  /* Init CDMA MDSP */
  init_status &= rf_cdma_mdsp_init();

  #ifdef FEATURE_CDMA1X
  /* init MDSP for 1x */
  init_status &= rf_1x_mc_init();
  #endif

  #ifdef FEATURE_HDR
  /* init MDSP for HDR */
  init_status &= rf_hdr_mc_init();
  #endif

  /* Return TRUE indicating the success for CDMA Driver Initialization */
  return init_status;

} /* rf_cdma_mc_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Common CDMA Driver Initialization

  @details
  This function will configure all internal CDMA (1x and HDR) driver state to a 
  known value. Any previous configuration information is lost.  Internal CDMA 
  driver sub-modules are also returned to their initial state.  The CDMA Driver 
  Initialization process is hardware independent, and assumes that the common 
  driver initialization routines have restored the hardware state to a 
  configurate that is consistent with power-on-reset.  This relieves the 
  technology specific driver from worrying about the hardware configuration,
  and eliminates redundant hardware resets that might be applied by each 
  technology as they re-initialize.  CDMA specific NV is re-loaded, 
  incoroporating any NV changes that have been written since the driver was
  last initialized.
 
  This function will execute only if the tech independent common init has
  completed successfully.
 
  @param common_init_successful
  Flag to indicate whether Tech independent Common Initialization was 
  successful

  @param caller_tcb_ptr
  TCB of the Task to used during NV Load
  
  @param task_nv_wait_sig
  Signal to Wait for NV LOad
  
  @param task_wait_func_ptr
  Pointer to the function to wait on.

  @return
  Flag indicating whether CDMA driver Initialization is successful
*/
boolean
rf_cdma_mc_deinit
(
  void
)
{
  boolean deinit_status = TRUE; /* Variable to track if CDMA Init passed */
  uint8 dev_idx;

  for (dev_idx = RFM_DEVICE_0; dev_idx < RFM_MAX_DEVICES; dev_idx++ )
  {
    /* Get Writable pointer to IC data */
    rf_cdma_ic_data_type* dev_ic_ptr_w = 
            rf_cdma_get_mutable_intelliceiver ( (rfm_device_enum_type)dev_idx );

    if ( (NULL != dev_ic_ptr_w) )
    {
      rf_cdma_ic_deinit(dev_ic_ptr_w);
    }
  } /* for (dev_idx = RFM_DEVICE_0; dev_idx < RFM_MAX_DEVICES; dev_idx++ ) */

  #ifdef FEATURE_CDMA1X
  /* init MDSP for 1x */
  deinit_status &= rf_1x_mc_deinit();
  #endif

  #ifdef FEATURE_HDR
  /* init MDSP for HDR */
  deinit_status &= rf_hdr_mc_deinit();
  #endif

  rf_cdma_nv_cleanup();

  rf_cdma_cleanup_device_status();

  /* Return TRUE indicating the success for CDMA Driver Initialization */
  return deinit_status;

} /* rf_cdma_mc_deinit */


/*! @} */

/*============================================================================*/
/*!
  @name Main Control Query

  @brief
  This section holds all Main Control Query Interfaces to be used by RFM layer.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query the RF card to get the RF Warmup time needed for the SW and HW to be 
  configured

  @param device
  The RF device to be configured

  @return
  The RF Warmup time in micro-secs (on success) and a -1 on failure
*/
int32
rf_cdma_get_warmup_time
(
  rfm_device_enum_type device
)
{
  int32 rf_wup_time;

  if (device >= RFCOM_MAX_DEVICES)
  {
    RF_MSG_1(RF_ERROR, "Invalid device = %d", device );
    rf_wup_time  = -1;
  }
  else
  {
    rf_wup_time = rfc_cdma_get_warmup_time(device);
    RF_MSG_1(RF_MED, "RF WU time is = %d (millisecs)", rf_wup_time );
  }

  return rf_wup_time;

} /* rf_cdma_get_warmup_time */

/*! @} */

/*============================================================================*/
/*!
  @name Housekeeping Query

  @brief
  This section holds all Main Control Query Interfaces to be used by RF
  Software .
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query if all required tasks are running
 
  @details
  This function will check if all the tasks required to perform RF action on
  a given RF device are in running state or not.
 
  @param device
  RFM device on which all tasks' status is to be checked
 
  @return
  Status indicating if all the required tasks are running or not.
*/
boolean
rf_cdma_required_tasks_running
(
  rfm_device_enum_type device
)
{
  boolean are_tasks_running = TRUE;
  rf_cdma_ic_task_data_type *ic_task_data; /* iC Task Data container */
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */

  /*--------------------------------------------------------------------------*/
  /* Check if the RF_TASK is running or not. RF Task is required to handle
  all FW responses and to perform RF algorithms like Temp Comp */
  are_tasks_running &= rfa_is_task_running(RF_TASK_MAIN);

  /*--------------------------------------------------------------------------*/
  /* Check if the RF_FWRSP_TASK is running or not. This is required to handle
  all FW responses */
  are_tasks_running &= rf_fwrsp_is_task_running();

  /*--------------------------------------------------------------------------*/
  /* Check if the RF_APPS_TASK is running or not. This is required to handle
  all FW responses */
  are_tasks_running &= rf_apps_is_task_running();

  /*--------------------------------------------------------------------------*/
  /* Check if the RF_IC_TASK is running or not. For a given device, if
  intelliciever operation is enabled, then RF_IC_TASK should be running. */

  /* Get pointer to the intelliceiver Task Data */
  ic_task_data = rf_cdma_get_ic_task_data();

  /* Get the device Status for validation */
  dev_status_r = rf_cdma_get_device_status( device );

  /* Perform NULL pointer check */
  if ( ( ic_task_data == NULL ) || ( dev_status_r == NULL ) )
  {
    RF_MSG( RF_ERROR, "rf_cdma_required_tasks_running: NULL Data Pointer" );
    are_tasks_running &= FALSE;
  } /* if ( ( ic_task_data == NULL ) || ( dev_status_r == NULL ) )*/
  else
  {
    if ( dev_status_r->intelliceiver.ic_supported == TRUE )
    {
      /* If iC is supported, then RF_IC_TASK must be running */
      if ( ic_task_data->housekeeping.is_task_running == FALSE )
      {
        are_tasks_running &= FALSE;
      } /* if ( ic_task_data->housekeeping.is_task_running == FALSE ) */
    } /* if ( dev_status_r->intelliceiver.ic_supported == TRUE ) */
  }

  /* Return the Running Status of all tasks */
  return are_tasks_running;

} /* rf_cdma_required_tasks_running */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Revert device info to sleep state 
 
  @details  
  This API is called when the prep_wakeup or exec_wakeup fails - so the
  RF state can be RF_CDMA_STATE_SLEEP (if prep_wakeup fails), or
  RF_CDMA_STATE_RX_PREP (if exec_wakeup fails)
    
  If the state is RF_CDMA_STATE_RX_PREP and this API is being executed, it means
  that exec_wakeup API failed - hence the vote to TCXO/VREG needs to be un-set.
 
  If the state is RF_CDMA_STATE_SLEEP, then prep_wakeup API failed - no vote 
  for VREG or TCXO was set. Hence no need to un-set the vote.
    
  This function sends a power-down vote to VREG module, and moves the device 
  state machine to sleep.
 
  @param device
  RFM device which needs to be put to sleep
    
  @param rf_mode
  RF mode in which the device SM is currently in
    
  @param band
  RF band to which the device tried tuning to (needed to unvote VREG)
    
*/ 
void 
rf_cdma_process_wakeup_failure
(
  const rfm_device_enum_type device,   
  const rfm_mode_enum_type rf_mode,    
  const rfm_cdma_band_class_type band
)
{
  const rf_cdma_data_status_type *dev_status_r; /* Device Status read Pointer */
  
  RF_MSG_3( RF_ERROR, "rf_cdma_process_wakeup_failure: Device %d, RF Mode %d, "
                      "Band %d", device, rf_mode, band );

  dev_status_r = rf_cdma_get_device_status( device );

  /* Perform NULL Pointer check */
  if ( dev_status_r == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_process_wakeup_failure: NULL data for "
                        "device %d", device );
    return;
  }

  /* un-vote only if exec_wakeup failed */
  if ( ( dev_status_r->data_1x.rf_state == RF_CDMA_STATE_RX_PREP ) ||
       ( dev_status_r->data_hdr.rf_state == RF_CDMA_STATE_RX_PREP ) )
  {
    boolean dev_voting_status = TRUE;

    /* Vote OFF for all RF Devices which are not needed for RFM Device.
       This API will vote OFF for TCXO buffer and NPLER BYPASS settings.
    */
    dev_voting_status &= rfcommon_core_dev_power_vote( device,
                                                    RFCMN_CORE_DEV_OFF,
                                                    RFCMN_CORE_DEV_NO_VOTE );
    if( dev_voting_status == FALSE )
    {
      RF_MSG_1( RF_ERROR, "rf_cdma_process_wakeup_failure: Device: %d "
                          "Pwr Down failed", device );
    }


    /* Revert the Vreg State, since Rx is not waking up */
    dev_voting_status &= 
      rfc_cdma_manage_vregs( device, rf_mode, band, RF_PATH_SLEEP_STATE);
  
    if( dev_voting_status == FALSE )
    {
      RF_MSG_1( RF_ERROR, "rf_cdma_process_wakeup_failure: Device: %d "
                          "VREG power down failed", device );
    }
    
  } /* if ( dev_status_r->rf_state == RF_CDMA_STATE_RX_PREP ) */

    /* un-vote the RFCMD app */ 
    if ( rf_mode == RFM_1X_MODE )
    {
        rf_hal_bus_client_handle* handle = 
                                  rf_cdma_data_get_rf_hal_bus_vote_handle(device);
      rf_hal_bus_enable (FALSE, &handle, RF_HAL_RFCMD_APP_CLIENT(1X));
      rf_cdma_data_set_rf_hal_bus_vote_handle(device, handle);
    }
    else if ( rf_mode == RFM_1XEVDO_MODE )
    {
      /* un-vote the RFCMD app */ 
        rf_hal_bus_client_handle* handle = 
                                  rf_cdma_data_get_rf_hal_bus_vote_handle(device);
      rf_hal_bus_enable (FALSE, &handle, RF_HAL_RFCMD_APP_CLIENT(EVDO));
      rf_cdma_data_set_rf_hal_bus_vote_handle(device, handle);
    }
    else
    {
      RF_MSG_2( RF_ERROR, "rf_cdma_process_wakeup_failure: Invalid Mode %d for"
                          " device %d !", rf_mode, device );
    }

  rf_cdma_mdsp_modem_rx_deallocate_resources ( device , rf_mode ,
      rf_cdma_data_get_rxlm_handle (device)) ;
  rf_cdma_reset_sw_state ( device ) ;

  return;

} /* rf_cdma_process_wakeup_failure */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  To find if the given channel is valid or not for bands without sub-classes. 
 
  @param band_descriptor
  The table that contains the band-channel informatiom
    
  @param chan
  The channel whose validity needs to be queried
    
  @param tbl_length
  The length in terms of records in the table that contains the band-chan info.
 
  @return channel
  If the channel is supported or not.
  TRUE: Supported
  FALSE: Not Supported
    
*/ 
boolean
rf_cdma_find_channel_validity_no_bc ( 
   const rf_cdma_band_descriptor* band_descriptor , 
   int16 chan , size_t tbl_length ) 
{
  boolean is_channel_valid = FALSE ;
  size_t count = 0 ;
  for ( ; count < tbl_length ; count++ )
  {
    if ( ( chan >= band_descriptor[count].lo_chan ) &&
         ( chan <= band_descriptor[count].hi_chan ) )
    {
      is_channel_valid = TRUE ;
      break ;
    }
  }
  return is_channel_valid ; 
}



/*----------------------------------------------------------------------------*/
/*!
  @brief Helper function to convert BC bitmasks to an array.
 
  @note sc_array should be big enough to holdaccomodate all sub
  classes.
 
  @param bc_mask
  Band Class mask to be converted into array format.
 
  @return sc_array[]
  Sub Class array.
 
  @return uint8
  Number of entries one needs to iterate to cover all Sub
  Classes.
*/ 
 uint8
 rf_cdma_bit_mask_to_array
 (
    uint32  bc_mask ,
    boolean sc_array[] 
 )
{
  uint8 count ;
  for ( count = 0 ; bc_mask != 0 ; count++ )
  {
    if ( bc_mask & 1 )
    {
      sc_array[count] = TRUE ;
    }
    else
    {
      sc_array[count] = FALSE ;
    }
    bc_mask >>= 1 ; 
  }
  return count ;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  To find if the given channel is valid or not for bands with sub-classes. 
 
  @param band_descriptor
  The table that contains the band-channel and sub-class informatiom
    
  @param chan
  The channel whose validity needs to be queried
    
  @param tbl_length
  The length in terms of records in the table that contains the band-chan info.
 
  @param *subclass
  The subclass retrieved by the table will be returned in this
  parameter. This value should be considered invalid if 
  is_channel_supported is FALSE.
 
  @param bc_mask
  The band class mask.
 
  @return is_channel_supported
  If the channel is supported or not.
  TRUE: Supported
  FALSE: Not Supported
    
*/ 
boolean
rf_cdma_find_channel_validity_with_subclass 
( 
   const rf_cdma_band_descriptor* band_descriptor , 
   int16 chan , 
   size_t tbl_length , 
   uint32 bc_mask , 
   int16* subclass 
) 
{
  boolean is_channel_valid = FALSE ;
  boolean adjacent_sc_failed = FALSE ;
  size_t count = 0 ;
  uint8 max_sc_rfc = 0 ;
  boolean subclass_rfc_arr[20] = { 0 } ;

  *subclass = 0 ;

  /* Convert RFC BitMask into Array */
  max_sc_rfc = rf_cdma_bit_mask_to_array ( bc_mask , subclass_rfc_arr ) ;

  for (   count = 0 ; 
        ( count < tbl_length) &&          /* Reached the end of table */
        ( max_sc_rfc >= band_descriptor[count].sub_class ) && /* Reached the max subclasses supported by RFC */          
        ( adjacent_sc_failed != TRUE ) ; /* Adjacent SC check failed  */
         count++ )
  {
    if ( subclass_rfc_arr[band_descriptor[count].sub_class] == TRUE )  
  {
    if ( ( chan >= band_descriptor[count].lo_chan ) &&
         ( chan <= band_descriptor[count].hi_chan ) )
    {
      is_channel_valid = TRUE ;
      *subclass = band_descriptor[count].sub_class ; 
      if ( band_descriptor[count].is_channel_group_conditionally_valid == TRUE )
      {
        /* Now check the adjacent sub class.  
        For bands with subclasses, 
        a channel is invalid 
        1> if it one of the channels listed as Not Valid
        2> And the ones that are Conditionally Valid but don't have the 
        adjacent subclass marked as supported. */

        /* Start with next table entry */
        size_t subclass_idx = count + 1 ;
        /* Adjacent SC is the next one */
        int16 max_sc = band_descriptor[count].sub_class + 1 ;
        for ( ; ( ( subclass_idx < tbl_length ) && 
                  ( band_descriptor[subclass_idx].sub_class <= max_sc ) ) ; 
                  subclass_idx++ )
        {
          /* Check if it is adjacent SC and not the current SC where 
             conditionally valid channel was found */
          if ( max_sc == band_descriptor[subclass_idx].sub_class )
          {
            if ( ( chan >= band_descriptor[subclass_idx].lo_chan ) &&
                 ( chan <= band_descriptor[subclass_idx].hi_chan ) )
            {
              return is_channel_valid ; 
            }
          }  /* if ( max_sc == band_descriptor[subclass_idx].sub_class ) */
        } /* For searching the sub class */

        /* if we havent found what we need in this next SC, time to quit */
        adjacent_sc_failed = TRUE ;
        is_channel_valid = FALSE ;
      } /* IF is_channel_group_conditionally_valid == TRUE  */ 
      else /* Found what we need time to quit */
      {
        break ;
      }
    } /* Channel has matched */
    }  /* RFC allows the sub class or not */
  } /* for ( ; (count < tbl_length) */
  return is_channel_valid ; 
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  To find if the sub-class is supported by RFC. 
 
  @param bc_mask
  The RFC bit mask that contains the supported sub classes.
    
  @param subclass
  The subclass whose validity needs to be queried
    
  @return is_channel_supported
  If the subclass by RFC is supported or not.
  TRUE: Supported
  FALSE: Not Supported
*/
 boolean
 rf_cdma_if_sc_rfc_combo_supported ( uint32 bc_mask , uint16 subclass ) 
 {
   boolean is_channel_valid = FALSE ;
   if ( bc_mask > 1 )
   {
     bc_mask = bc_mask >> subclass ;
     /* If the LSB is 1 sub class is supported */
     if ( bc_mask & 0x01 )
     {
       is_channel_valid = TRUE ;
     }
   }
   return is_channel_valid ;
 }

/*----------------------------------------------------------------------------*/
/*!
  @brief
  To find if the given channel is valid or not for a given band and device.
 
  @param device
  The RFM device.
 
  @param band
  RF band whose validity needs to be queried
 
  @param chan
  The channel whose validity needs to be queried
    
  @return channel
  If the channel is supported or not.
  TRUE: Supported
  FALSE: Not Supported
    
*/ 
boolean
rf_cdma_mc_is_band_chan_sub_class_supported 
(
  const rfm_device_enum_type device,
  const rfm_cdma_band_class_type band ,
  const rfm_cdma_chan_type chan 
)
{
  boolean is_channel_valid = FALSE ;
  size_t number_of_channel_groups = 0 ;
  const rf_cdma_band_descriptor* band_descriptor = 
    rf_get_band_descriptor ( band , &number_of_channel_groups ) ;

  if ( band_descriptor != NULL ) 
  {
    uint32 bc_mask = 0 ;
    if ( number_of_channel_groups > 0 )
    {
      if ( band_descriptor[0].sub_class != -1  )
      {
        int16 subclass = -1 ;
        bc_mask = rfc_cdma_get_sub_class_for_band ( device , band ) ;
        is_channel_valid =
         rf_cdma_find_channel_validity_with_subclass (  
            band_descriptor , chan , number_of_channel_groups , bc_mask , &subclass ) ;
        }
      else
      {
        is_channel_valid =
         rf_cdma_find_channel_validity_no_bc ( 
            band_descriptor , chan , number_of_channel_groups ) ;
      }
    }
    else
    {
      RF_MSG ( RF_ERROR , "rf_cdma_mc_is_band_chan_sub_class_supported(): "
               "band_descriptor[] has 0 elements " ) ;
    }
  }

  return is_channel_valid ;
} /* rf_cdma_mc_is_band_chan_sub_class_supported */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Checks if band and channel are valid and supported

  @details
  This function checks if the (band, chan) is valid and if NV has been loaded
  for the band. If either of these two conditions are not satisfied, the API
  returns a FALSE

  @param device
  Device for which Band and Chan is to be validated.

  @param band
  Band which is to be validated
 
  @param chan
  Channel which is to be validated
 
  @return
  TRUE if band and channel are supported and FALSE otherwise
*/
boolean
rf_cdma_mc_is_band_chan_supported
(
  const rfm_device_enum_type device,
  const rfm_cdma_band_class_type band,
  const rfm_cdma_chan_type chan
)
{
  boolean success = FALSE ; 
  uint32 rx = rf_cdma_get_rx_carrier_freq(band, chan) ;
  uint32 tx = rf_cdma_get_tx_carrier_freq(band, chan) ;
  if ( rx == 0 || tx == 0 )
  {
    /* If we can't determine the Rx or Tx frequency for this channel,
       then it isn't suppported. */
    /*RF_MSG_2 ( RF_MED , "rf_cdma_mc_is_band_chan_supported:"
               "rf_cdma_get_rx_carrier_freq() or rf_cdma_get_tx_carrier_freq() "
               "zero: %d KHz, %d KHz" , rx , tx ) ;*/
  }
  else
  {
    success = 
     rf_cdma_mc_is_band_chan_sub_class_supported ( device ,  band , chan) ;
    if ( FALSE == success )
    {
      RF_MSG_DBG_3 ( RF_ERROR , "rf_cdma_mc_is_band_chan_supported()" 
                 "rf_cdma_mc_is_band_chan_sub_class_supported () FAILED "
                 "device[%d] | band[%d] | chan[%d] " ,   
                 device ,  band , chan ) ;
    }

    /* Allow RF to tune to an invalid channel */
    success = TRUE;
    
    if ( TRUE == success )
    {
      success = 
       rfm_cdma_is_tunable_on_device ( device ,
                                     (const sys_band_class_e_type) band ) ;
    }
    else
    {
      /* RF_MSG ( RF_ERROR , "rf_cdma_mc_is_band_chan_supported(): "  
               "rf_cdma_mc_is_band_chan_sub_class_supported() has returned FAILURE. " ) ; */
    }
  }

  if ( FALSE == success )
  {
    RF_MSG_DBG_5 ( RF_ERROR , "rf_cdma_mc_is_band_chan_supported: failed for "
               "device %d, band %d, chan %d, Rx carrier freq %d KHz, Tx carrier"
			   " freq %d KHz ", device, band, chan, rx , tx ) ;
  }

  return success ;
} /* rf_cdma_mc_is_band_chan_supported */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Creates the RF ON/OFF event script based on the type of mode (1x/HDR)
 
  @details
  This API creates an RF ON/OFF event scripts based on the type of mode sent in
  and then sends the scripts to the MDSP layers for execution via Firmware based 
  on the timeline, the scripts can be either SSBI or GRFC based, depends on
  what is supported
 
  @param device
  Device which needs to be configured for RF ON/OFF
 
  @param band
  Band for which RF ON/OFF is needed
 
  @param rf_mode
  RF mode for which RF ON/OFF is needed
 
  @param buffer_index
  Buffer index where FW can find scripts - this is used to determine the shared
  memory location
 
  @return
  TRUE if ON/OFF was success, FALSE in failure case
*/
boolean
rf_cdma_mc_configure_tx_on_off
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band,
  rfm_mode_enum_type rf_mode,
  uint32 buffer_index
)
{
  boolean api_status = TRUE ; 
  /* Buffr object to hold On scripts */
  rf_buffer_intf *on_settings = NULL;
  /* Buffr object to hold Off scripts */
  rf_buffer_intf *off_settings = NULL;
  rfcommon_mdsp_event_handle** rf_off_event_handle = NULL;

  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */

  /*Get RF ON event handle*/
  rfcommon_mdsp_event_handle** rf_on_event_handle =
    rf_cdma_data_get_event_handle( device, RF_CDMA_EVENT_RF_ON );

  /* Check for NULL event handle */
  if ( rf_on_event_handle == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_mc_configure_tx_on_off: "
                      "Null RF ON event handle! ");
    return FALSE;
  }

  /* Get RF OFF event handle */
 rf_off_event_handle =
    rf_cdma_data_get_event_handle( device, RF_CDMA_EVENT_RF_OFF );

  /* Check for NULL event handle */
  if ( rf_off_event_handle == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_mc_configure_tx_on_off: "
                      "Null RF OFF event handle! ");
    return FALSE;
  }

  /* Get Logical Device Params */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_mc_configure_tx_on_off: "
                      "Null Logical Dev pointer");
    return FALSE;
  }

  /*-----------------------RF ON EVENT----------------------------*/
  /* Create generic buffer interface object to hold grfc settings 
     for RF ON*/
  on_settings = rf_buffer_create ( CFW_RF_EVENT_MAX_NUM_OF_SBI , 
                                   CFW_RF_EVENT_MAX_NUM_OF_RFFE , 
                                   RF_CDMA_MAX_GRFC_SCRIPT_SIZE ) ;

  /*Check if valid pointer was returned for buffer object*/
  if( on_settings == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_mc_configure_tx_on_off: "
                      "Unable to create buffer object for on settings");
    api_status = FALSE;
  }/*if( on_settings == NULL )*/
  else
  {
    api_status = rf_cdma_get_rf_onoff_settings( device, rf_mode, band, TRUE, 
                                                logical_dev, on_settings );

    /* Send to MDSP layer if API was success till this point */
    if ( api_status == TRUE )
    {
      /* Configure Tx ON event using the obtained settings*/
      api_status &= rf_cdma_mdsp_configure_devices( device ,
                    logical_dev->tx_mdsp_path,
                                                    rf_mode,
                                                    RF_CDMA_MDSP_CCS_EVENT_RF_ON_ID, 
                                                    rf_on_event_handle,
                                                    (rf_cdma_mdsp_get_event_info(RF_CDMA_MDSP_CCS_EVENT_RF_ON_ID)) ,
                                                    on_settings, 
                                                    buffer_index,
                    rf_cdma_data_get_txlm_handle (device) ,
                                                    NULL,
                                                    NULL);
    }/*if ( api_status == TRUE )*/
    else
    {
      RF_MSG( RF_ERROR, "rf_cdma_mc_configure_tx_on_off:"
                        "Failed to generate script for RF ON! " ); 
    }/*if ( api_status != TRUE )*/
  }/*if( on_settings != NULL )*/

  /*destroy buffer after use*/
  if ( on_settings != NULL )
  {
    rf_buffer_destroy( on_settings );
  }

  /*-----------------------RF OFF EVENT----------------------------*/
  /* Create generic buffer interface object to hold grfc settings 
     for RF OFF*/
  off_settings = rf_buffer_create ( CFW_RF_EVENT_MAX_NUM_OF_SBI , 
                                    CFW_RF_EVENT_MAX_NUM_OF_RFFE , 
                                    RF_CDMA_MAX_GRFC_SCRIPT_SIZE ) ;

  /*Check if valid pointer was returned for buffer object*/
  if( off_settings == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_mc_configure_tx_on_off: "
                      "Unable to create buffer object for off settings");
    api_status = FALSE;
  }/*if( off_settings == NULL )*/
  else
  {
    api_status = rf_cdma_get_rf_onoff_settings( device, rf_mode, band, FALSE, 
                                                logical_dev, off_settings );
             
    /* Send to MDSP layer if API was success til this point */
    if ( api_status == TRUE )
    {
      /* Configure Tx OFF event using the obtained settings*/
      api_status &= rf_cdma_mdsp_configure_devices( device ,
                    logical_dev->tx_mdsp_path,
                                                    rf_mode,
                                                    RF_CDMA_MDSP_CCS_EVENT_RF_OFF_ID, 
                                                    rf_off_event_handle,
                                                    (rf_cdma_mdsp_get_event_info(RF_CDMA_MDSP_CCS_EVENT_RF_OFF_ID)) ,
                                                    off_settings, 
                                                    buffer_index,
                    rf_cdma_data_get_txlm_handle (device) ,
                                                    NULL,
                                                    NULL);
    }/*if ( api_status == TRUE )*/
    else
    {
      RF_MSG( RF_ERROR, "rf_cdma_mc_configure_tx_on_off:Failed to generate "
                        "script for RF OFF! " ); 
    }/*if ( api_status != TRUE )*/
  }/*if( off_settings != NULL )*/

  if( api_status == FALSE )
  {
    RF_MSG_2( RF_ERROR, "rf_cdma_mc_configure_tx_on_off: mdsp RF ONOFF "
              "configuration failed Device %d band %d", device, band );
  }

  /*destroy buffer after use*/
  if ( off_settings != NULL )
  {
    rf_buffer_destroy( off_settings );
  }

  /* Clean up event handle and reset FW smem status after a config for dynamic
  event */
  rf_cdma_mc_cleanup_ccs_event( device, RF_CDMA_EVENT_RF_ON );
  rf_cdma_mc_cleanup_ccs_event( device, RF_CDMA_EVENT_RF_OFF );

  return api_status ;
} /* rf_cdma_mc_configure_tx_on_off() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Creates the RF ON/OFF event script based on the type of mode (1x/HDR)
 
  @details
  This API creates an RF ON/OFF event scripts based on the type of mode sent in
  and then sends the scripts to the MDSP layers for execution via Firmware based 
  on the timeline, the scripts can be either SSBI or GRFC based, depends on
  what is supported
 
  @param device
  Device which needs to be configured for RF ON/OFF
 
  @param band
  Band for which RF ON/OFF is needed
 
  @param rf_mode
  RF mode for which RF ON/OFF is needed

 @pram *logical_dev,
 logical dev for RF ON/OFF is neededed
 
  @param buffer_index
  Buffer index where FW can find scripts - this is used to determine the shared
  memory location
 
  @return
  TRUE if ON/OFF was success, FALSE in failure case
*/
boolean
rf_cdma_get_rf_onoff_settings
(
  const rfm_device_enum_type device,
  const rfm_mode_enum_type rf_mode,
  const rfm_cdma_band_class_type band,
  const boolean rf_on_off,
  const rfc_cdma_logical_device_params_type *logical_dev,
  rf_buffer_intf *settings_buffer
)
{
  boolean api_status = TRUE;
  int16 time_delays_us; /* time_delays for PA and TX on/off */
  rfcom_band_type_u rf_band; /* union needed for PA device calls */
  void* pa_device = NULL; /* pointer to PA object */
  rfdevice_pa_config_type pa_params_s;
  int32 num_grfc = 0;/* initialize to zero */
  rf_band.cdma_band = band;


  RF_MSG_4( RF_LOW, "   rf_cdma_get_rf_onoff_settings: Device %d, Mode %d, "
                    "Band %d, On_Off %d" ,device, rf_mode, band, rf_on_off );

  if ( rfm_get_calibration_state() != TRUE )
  {
    if ( rf_on_off == TRUE )
    {
      time_delays_us = RF_CDMA_PA_WARM_UP_US;
    }
    else
    {
      time_delays_us = RF_CDMA_PA_TURN_OFF_DELAY_US;
    }
  
    /* Get PA script from device for PA */
    pa_device = get_cdma_pa_device ( logical_dev->pa[band] ) ;

    pa_params_s.mode = rf_mode;
    pa_params_s.band = rf_band;
    pa_params_s.bw = RFDEVICE_PA_BW_NOT_APPLICABLE;
    pa_params_s.trigger_type = RFDEVICE_PA_TRIGGER_ON_OFF;
    pa_params_s.rfm_device = device;

    api_status &= rfdevice_set_pa_on_off ( &pa_params_s, rf_on_off, pa_device,
                                       settings_buffer, RFDEVICE_CREATE_SCRIPT, 
                                       time_delays_us );
  }

  if ( rf_on_off == TRUE )
  {
    if (rfm_get_calibration_state() != TRUE)
    {
      time_delays_us = RF_CDMA_TX_WARM_UP_US;
    }
    else
    {
      time_delays_us = RFDEVICE_NO_TIME_OFFSET; /* just append the settings to 
      existing buffer - do not add time offsets, since that would rearrange the
      tasks in CCS events  */
    }
  }
  else
  {
    time_delays_us = RF_CDMA_TX_TURN_OFF_DELAY_US;
  }


  /*  Check if GRFC on off supported - if not, use WTR scripts */
     /* Compute GRFC Programming Script for the given Band and Channel
  for TX ON/OFF based on the rf_on_off flag, if the number of GRFC returned
  is zero, it means that there is no GRFC support, resort to using WTR scripts
  in that scenario */
    api_status &= rfc_cdma_generate_tx_on_off_grfc_script( device, band, 
                rf_on_off, settings_buffer, time_delays_us,&num_grfc );

  /* No GRFC signals available, use WTR scripts */
  if ( num_grfc == 0 )
  {
    /* Populate ON/OFF settings with SSBI script using this API */
    api_status &= rfdevice_cdma_tx_get_tx_on_off_script( 
                                                   logical_dev->tx_device[band], 
                                                   rf_on_off, 
                                                   settings_buffer, 
                                                   time_delays_us );
  }

  /* API status is FALSE*/
  if ( api_status == FALSE )
  {
    RF_MSG_4( RF_ERROR, "rf_cdma_get_rf_onoff_settings: failed -- Device %d,"
                        "Mode %d, Band %d, On_Off %d" ,device, rf_mode, band, 
                        rf_on_off );
  }

  return api_status;

} /* rf_cdma_get_rf_onoff_settings */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Creates the Rx ON/OFF event script based on the type of mode (1x/HDR)
 
  @details
  This API creates an Rx ON/OFF event scripts based on the type of mode sent in
  and then sends the scripts to the MDSP layers for execution via Firmware based 
  on the timeline, the scripts can be either SSBI or GRFC based, depends on
  what is supported. This is used in discontinuous Rx scenarios. 
 
  @param device
  Device which needs to be configured for Rx ON/OFF
 
  @param band
  Band for which Rx ON/OFF is needed
 
  @param rf_mode
  RF mode for which Rx ON/OFF is needed
 
  @param buffer_index
  Buffer index where FW can find scripts - this is used to determine the shared
  memory location
 
  @return
  TRUE if ON/OFF was success, FALSE in failure case
*/
boolean
rf_cdma_mc_configure_rx_on_off
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band,
  rfm_mode_enum_type rf_mode,
  uint32 buffer_index
)
{
  boolean api_status = TRUE ; 
  /* Buffr object to hold On scripts */
  rf_buffer_intf *rx_on_settings = NULL;
  /* Buffr object to hold Off scripts */
  rf_buffer_intf *rx_off_settings = NULL;
  int16 time_delays_us = 0; 
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  rfcommon_mdsp_event_handle** rx_off_event_handle = NULL;

  /*Get RF ON event handle */
  rfcommon_mdsp_event_handle** rx_on_event_handle =
    rf_cdma_data_get_event_handle( device, RF_CDMA_EVENT_RX_ON );

  /* Check for NULL event handle*/
  if ( rx_on_event_handle == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_mc_configure_rx_on_off: "
            "Null Rx on event handle");
    return FALSE;
  }

  /* Get RF OFF event handle */
  rx_off_event_handle =
    rf_cdma_data_get_event_handle( device, RF_CDMA_EVENT_RX_OFF );

  /* Check for NULL event handle*/
  if ( rx_off_event_handle == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_mc_configure_rx_on_off: "
            "Null Rx off event handle");
    return FALSE;
  }

  /* Get Logical Device Params */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_mc_configure_rx_on_off: "
                      "Null Logical Dev pointer");
    return FALSE;
  }

  /*-----------------------Rx ON EVENT----------------------------*/
  /* Create generic buffer interface object to hold grfc settings 
     for Rx ON*/
  rx_on_settings = rf_buffer_create ( CFW_RF_EVENT_MAX_NUM_OF_SBI , 
                                   CFW_RF_EVENT_MAX_NUM_OF_RFFE , 
                                   RF_CDMA_MAX_GRFC_SCRIPT_SIZE ) ;

  /*Check if valid pointer was returned for buffer object*/
  if( rx_on_settings == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_mc_configure_rx_on_off: "
                      "Unable to create buffer object for on settings");
    api_status = FALSE;
  }/*if( on_settings == NULL )*/
  else
  {
    /* Get scripts for Rx ON */
    api_status = rfdevice_cdma_rx_get_rx_on_off_script(
                                                  logical_dev->rx_device[band], 
                                                  TRUE,/*Flag for Rx ON*/ 
                                                  rx_on_settings,
                                                  time_delays_us );
                                                
    /* Send to MDSP layer if API was success till this point */
    if ( api_status == TRUE )
    {
      /* Configure Tx ON event using the obtained settings */
      api_status &= rf_cdma_mdsp_configure_devices( device ,
                    logical_dev->rx_mdsp_path,
                                                    rf_mode,
                                                    RF_CDMA_MDSP_CCS_EVENT_RX_ON_ID, 
                                                    rx_on_event_handle,
                                                    (rf_cdma_mdsp_get_event_info(RF_CDMA_MDSP_CCS_EVENT_RX_ON_ID)) ,
                                                    rx_on_settings, 
                                                    buffer_index,
                    rf_cdma_data_get_rxlm_handle (device) ,
                                                    NULL,
                                                    NULL);
    }/*if ( api_status == TRUE )*/
    else
    {
      RF_MSG( RF_ERROR, "rf_cdma_mc_configure_rx_on_off:"
                        "Failed to generate script for Rx ON! " ); 
    }/*if ( api_status != TRUE )*/
  }/*if( on_settings != NULL )*/

  /*destroy buffer after use*/
  if ( rx_on_settings != NULL )
  {
    rf_buffer_destroy( rx_on_settings );
  }

  /*-----------------------Rx OFF EVENT----------------------------*/
  /* Create generic buffer interface object to hold grfc settings 
     for Rx OFF*/
  rx_off_settings = rf_buffer_create ( CFW_RF_EVENT_MAX_NUM_OF_SBI , 
                                    CFW_RF_EVENT_MAX_NUM_OF_RFFE , 
                                    RF_CDMA_MAX_GRFC_SCRIPT_SIZE ) ;

  /*Check if valid pointer was returned for buffer object*/
  if( rx_off_settings == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_mc_configure_rx_on_off: "
                      "Unable to create buffer object for off settings");
    api_status = FALSE;
  }/*if( off_settings == NULL )*/
  else
  {
    /* Get scripts for Rx OFF */
    api_status = rfdevice_cdma_rx_get_rx_on_off_script(
                                                   logical_dev->rx_device[band], 
                                                   FALSE,/*Flag for Rx off*/
                                                   rx_off_settings,
                                                   time_delays_us );
             
    /* Send to MDSP layer if API was success til this point */
    if ( api_status == TRUE )
    {
      /* Configure Tx OFF event using the obtained settings*/
      api_status &= rf_cdma_mdsp_configure_devices( device ,
                    logical_dev->rx_mdsp_path,
                                                    rf_mode,
                                                    RF_CDMA_MDSP_CCS_EVENT_RX_OFF_ID, 
                                                    rx_off_event_handle,
                                                    (rf_cdma_mdsp_get_event_info(RF_CDMA_MDSP_CCS_EVENT_RX_OFF_ID)) ,
                                                    rx_off_settings, 
                                                    buffer_index,
                    rf_cdma_data_get_rxlm_handle (device) ,
                                                    NULL,
                                                    NULL);
    }/*if ( api_status == TRUE )*/
    else
    {
      RF_MSG( RF_ERROR, "rf_cdma_mc_configure_rx_on_off:Failed to generate "
                        "script for Rx OFF! " ); 
    }/*if ( api_status != TRUE )*/
  }/*if( off_settings != NULL )*/

  if( api_status == FALSE )
  {
    RF_MSG_2( RF_ERROR, "rf_cdma_mc_configure_rx_on_off: mdsp Rx ONOFF "
              "configuration failed Device %d band %d", device, band );
  }

  /*destroy buffer after use*/
  if ( rx_off_settings != NULL )
  {
    rf_buffer_destroy( rx_off_settings );
  }

  return api_status ;
} /* rf_cdma_mc_configure_rx_on_off() */

/*! @} */

/*============================================================================*/
/*!
  @name RxAGC

  @brief
  This section holds all Main Control Interfaces to configure RxAGC.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Update Tx/Rx AGC for all eligible devices
 
  @details
  This function will loop through all devices and update tx/rx agc for all 
  eligible devices. The update order for the same RTR is RxAGC0,TxAGC,RxAGC1.
  At the end of updateing each Tx AGC, a semaphore wait is needed to sync FW
  to make sure that FW has finished resume_rx_agc for the last Rx device.
 
  @param void
   
  @return
  TRUE if all eligible device uopdating passed; Fail otherwise
*/
boolean 
rf_cdma_update_agc
(
  void
)
{
  uint8 rfm_device_id;
  boolean result = TRUE;
  rf_cdma_ic_data_type* ic_data;
  const rf_cdma_data_status_type* dev_status_r;
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device*/

  /* Obtain cdma lock*/
  rf_common_enter_critical_section( rf_cdma_get_lock_data() );

  /*----------------------------------------------------------------------------
     Loop through all devices and update eligible devices
  ----------------------------------------------------------------------------*/
  for ( rfm_device_id = RFM_DEVICE_0; rfm_device_id < RFM_MAX_DEVICES; 
        rfm_device_id++  )
  {

    /* Get writable rf_cdma_data pointer*/
    dev_status_r = rf_cdma_get_device_status( rfm_device_id );
     /* Get Logical Device Param */
    logical_dev = rfc_cdma_get_logical_device_params( rfm_device_id );

    if( (dev_status_r == NULL) || (logical_dev == NULL) )
    {
      RF_MSG_1( RF_ERROR, "rf_cdma_update_agc: NULL for device %d!",
                rfm_device_id );
    } /* NULL pointer, skip*/
    /* Based on the current mode of the device, check the appropriate data
      structure for its state, If device is in any other state besidex Rx/
      RXTX or RXDiv, skip updating */
    else if ((( dev_status_r->rf_mode == RFM_1X_MODE ) &&
              ( dev_status_r->data_1x.rf_state == RF_CDMA_STATE_RX ||
                dev_status_r->data_1x.rf_state == RF_CDMA_STATE_RXTX ||
                dev_status_r->data_1x.rf_state == RF_CDMA_STATE_RXDIV )) ||
             (( dev_status_r->rf_mode == RFM_1XEVDO_MODE ) &&
              ( dev_status_r->data_hdr.rf_state == RF_CDMA_STATE_RX ||
                dev_status_r->data_hdr.rf_state == RF_CDMA_STATE_RXTX ||
                dev_status_r->data_hdr.rf_state == RF_CDMA_STATE_RXDIV )))
    {
      /* Pointer to FW housekeeping data */
      rf_cdma_fwrsp_data_type *fwrsp_hk = NULL;

      ic_data = rf_cdma_get_mutable_intelliceiver( rfm_device_id );

      if ( ic_data != NULL )
      {
      /* Pause IC task if the device supports IC task*/
      if ( ic_data->ic_supported )
      {
        rf_cdma_ic_stop( ic_data , logical_dev );
      }
      }

      /* Reload rx agc for the device. Will skip and return FALSE if device is
      not eligible to update*/
      result = rf_cdma_update_rx_agc( rfm_device_id );

      /*Reload tx agc for the device. WIll skip and return FALSE if device is
      not eligible to update*/
      rf_cdma_update_tx_agc( rfm_device_id );

        /* Get FW Response Data */
        fwrsp_hk = rf_cdma_get_fwrsp_data( rfm_device_id );

      if ( fwrsp_hk == NULL )
      {
        ERR_FATAL( "rf_cdma_update_agc: NULL FWRSP data for dev %d", 
                   rfm_device_id, 0, 0 );
      }
      else
      {
        /* Wait for the Semaphore. This is to wait for FW response for 
        fw_resume_rx_agc_msg sent during rf_cdma_update_rx_agc. RF SW needs to 
        make sure FW has finished last rx_agc_configure and rx_agc_resume to 
        start next rx_agc_stop*/ 
        if ( result )
        {
          RF_MSG_2( RF_LOW, "[CDMA][RF-FW Sync] [D%d] IC_UPDATE : "
                    "Start Wait Sem @ 0x%x", rfm_device_id, 
                    fwrsp_hk->ic_update_rsp.sema_token );
          rfcommon_semaphore_wait( fwrsp_hk->ic_update_rsp.sema_token );
        } /* if ( result ) */
        else
        {
          /* Free semaphore if update Rx AGC failed */
          RF_MSG ( RF_ERROR, "rf_cdma_update_agc(): "
                             " rf_cdma_update_rx_agc Failed!!"); 
          /* Message sending failed, free the semaphore */
          RF_MSG_2 ( RF_LOW, "rf_cdma_update_agc():"
                     " Freeing the semaphore and not waiting "   
                     " [D%d] IC_UPDATE pushed back : Sem @ 0x%x" , 
                     rfm_device_id , fwrsp_hk->ic_update_rsp.sema_token ) ;
        }

        /* Push back semaphore after wait if mssg was sent successfully,
        or immediately if mssg sending failed*/
        rfcommon_semaphore_push_item( fwrsp_hk->ic_update_rsp.sema_token );

      }/* if ( fwrsp_hk != NULL )*/

      if ( ic_data != NULL )
      {
      /* Resume the IC task if the device supports IC task*/
      if ( ic_data->ic_supported )
      {
        rf_cdma_ic_start( ic_data, logical_dev,
                          dev_status_r->curr_band,
                          dev_status_r->lo_tuned_to_chan );
      }
      }
    } /* !( if( (dev_status_r == NULL) || (logical_dev == NULL) ) )*/
    else
    {
      RF_MSG_2( RF_ERROR, "rf_cdma_update_agc: device %d! is in sleep "
                " (mode :%d)",rfm_device_id,dev_status_r->rf_mode );
    }
  } /* for ( rfm_device_id = RFM_DEVICE_0; ... */
  
  /* Release cdma lock */
  rf_common_leave_critical_section( rf_cdma_get_lock_data() );

  return result;

} /* rf_cdma_update_agc */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Creates Tune back scripts 
 
  @details
  This API creates Tx tune back scripts for QTA
 
  @param device
  RFM device number.

  @param logical_dev
  Logical Dev pointer of which needs to be configured for Rx Wakeup
 
  @param band
  RF CDMA Band
 
  @param chan
  RF CDMA chan

  @param mode
  RF mode. 1X or DO.

  @param ant_tuner_script_token_ptr
  The pointer to the data structure that containts the antennae
  tuner script.

  @return
  TRUE for successfully filling in script. False for error condition.
 
*/
boolean
rf_cdma_mc_create_tx_tune_back_script
(
  const rfm_device_enum_type device,
  const rfc_cdma_logical_device_params_type *logical_dev,
  const rfm_cdma_band_class_type band,
  const rfm_cdma_chan_type chan,
  const rfm_mode_enum_type mode,
  ant_tuner_device_tokens_type* ant_tuner_script_token_ptr,
  rf_buffer_intf *rf_event_script
)
{
  boolean result = TRUE;

  const rf_cdma_tx_static_type *tx_static_nv_ptr;

  if(logical_dev == NULL)
  {
    return FALSE;
  }

  tx_static_nv_ptr = rf_cdma_nv_get_tx_static( logical_dev->tx_nv_path[band], 
                                               band);
  if ( tx_static_nv_ptr == NULL )
  {
    return FALSE;
  }

  /* Compute GRFC Programming Script for the given Band and Channel */
  result &= rfc_cdma_generate_tx_wakeup_grfc_script( device,
            band,
            rf_event_script,
            0 /*delay*/ );


  if ( NULL != logical_dev->sm[band] )
  {
    /* Wakeup ASM */
    result &= rf_cdma_asm_device_wakeup_tx( logical_dev->sm[band],
                                            mode, band,
                                            rf_event_script,
                                            RFDEVICE_CREATE_SCRIPT );

    /* Configure ASM for Tx */
    result &= rf_cdma_asm_device_enable_tx( logical_dev->sm[band],
                                            mode, band,
                                            rf_event_script,
                                            RFDEVICE_CREATE_SCRIPT ) ;
    if ( result == FALSE )
    {
      RF_MSG( RF_ERROR, "rf_cdma_mc_create_tx_tune_back_script: "
              "ASM driver failure");
    }
  }/*if ( NULL != logical_dev->sm[band] )*/

  if ( logical_dev->ant_tuner[band] != NULL )
    {
    rfcom_band_type_u band_in_common_format;
    	
    band_in_common_format.cdma_band = band;

    result &=
      rf_cdma_atuner_configure_for_tx( rf_cdma_data_get_txlm_handle(device) ,
                                      logical_dev->ant_tuner[band],
                                      mode,
                                      band_in_common_format,
                                      chan,
                                      RFDEVICE_CREATE_SCRIPT,
                                      rf_event_script,
                                      0, /* No timing offset */
                                      ant_tuner_script_token_ptr,
                                      (void*)&(tx_static_nv_ptr->ant_tuner_cl) );
    if ( result == FALSE )
  {
      RF_MSG( RF_ERROR, "rf_cdma_mc_create_tx_tune_back_script: "
              "Antenna tuner driver failure");
    }
  }

  return result;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Creates RF device (ASM and XSW) script for Wakeup Rx based on RF mode (1x/HDR)
 
  @details
  This API creates an RF device (ASM) script based on RF mode. This API should 
  be called everytime Rx is tuned. It takes a pointer to RF script buffer as 
  input and may or may not fill in the script depending on whether the RF device 
  that it searches for is present.
  Additionally the API queries for the current cross switch position and 
  then queries cross switch driver for the settings required for that xsw
  position
 
  @param logical_dev
  Logical Dev pointer of which needs to be configured for Rx Wakeup
 
  @param band
  RF CDMA Band
 
  @param mode
  RF mode. 1X or DO.
 
  @param rf_event_script
  RF script buffer to be filled in.
 
  @return
  TRUE for successfully filling in script. False for error condition. 
  Note that if no device is found, TRUE is still returned since it's valid 
*/
boolean
rf_cdma_mc_create_rx_asm_xsw_wakeup_script
(
  const rfm_device_enum_type device,
  const rfc_cdma_logical_device_params_type *logical_dev,
  const rfm_cdma_band_class_type band,
  const rfm_mode_enum_type mode,
  rf_buffer_intf *rf_event_script
)
{
  boolean result = TRUE;

  /* Use device scripting */
  rf_device_execution_type dev_action = RFDEVICE_CREATE_SCRIPT;

  /* This section will be executed only if there is a valid ASM */ 
  if ( NULL != logical_dev->sm[band] )
  {
    #ifdef FEATURE_RF_ASDIV
    rfcommon_asdiv_position_type position = 0;
    rf_cdma_asdiv_xsw_script_data xsw_script_data;
    #endif /* FEATURE_RF_ASDIV */


    /* Wakeup ASM */
    result &= rf_cdma_asm_device_wakeup_rx( logical_dev->sm[band],
                                            mode, band,
                                         rf_event_script, 
                                         dev_action );

    /* Configure ASM for Rx */
    result &= rf_cdma_asm_device_enable_rx( logical_dev->sm[band],
                                            mode, band,
                                            rf_event_script, 
                                            dev_action ) ; 

    #ifdef FEATURE_RF_ASDIV
    /* Populate data for the XSW (cross-switch) script */
    xsw_script_data.device = device;
    xsw_script_data.band = band;
    xsw_script_data.immediate = TRUE;

    /* Get current position of XSW */
    position = rfcommon_asdiv_get_current_position( device );

    /* Add settings for XSW position setting to the Rx wakeup Script */
    result &= rfc_cdma_generate_asdiv_xsw_script( position, 
                                                  rf_event_script,
                                                  &xsw_script_data );

    #endif /* FEATURE_RF_ASDIV */

    if ( result == FALSE )
    {
      RF_MSG( RF_ERROR, "rf_cdma_mc_create_rx_asm_wakeup_script:"
                        "ASM driver failure ");
    }
  }/*if ( NULL != logical_dev->sm )*/

  if ( result == FALSE )
  {
    RF_MSG( RF_ERROR, "rf_cdma_mc_create_rx_asm_wakeup_script: Error !! ");
  }

  return result;
} /* rf_cdma_mc_create_rx_asm_wakeup_script */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Creates RF device (ASM) script for Sleep Rx based on RF mode (1x/HDR)
 
  @details
  This API creates an RF device (ASM) script based on RF mode. This API should 
  be called everytime Rx is stopped. It takes a pointer to RF script buffer as
  input and may or may not fill in the script depending on whether the RF device 
  that it searches for is present.
 
  @param logical_dev
  Logical Dev pointer of which needs to be configured for Rx Sleep
 
  @param band
  RF CDMA Band
 
  @param mode
  RF mode. 1X or DO.
 
  @param rf_event_script
  RF script buffer to be filled in. 
   
  @return
  TRUE for successfully filling in script. False for error condition. 
  Note that if no device is found, TRUE is still returned since it's valid 
*/
boolean
rf_cdma_mc_create_rx_asm_sleep_script
(
  const rfc_cdma_logical_device_params_type *logical_dev,
  const rfm_cdma_band_class_type band,
  const rfm_mode_enum_type mode,
  rf_buffer_intf *rf_event_script
)
{
  boolean result = TRUE;

  /* Use device scripting */
  rf_device_execution_type dev_action = RFDEVICE_CREATE_SCRIPT;

  /* This section will be executed only if there is a valid ASM */ 
  if ( NULL != logical_dev->sm[band] )
  {

    /* Put ASM to sleep */
    result &= rf_cdma_asm_device_disable_rx( logical_dev->sm[band],
                                                mode,
                                                band,
                                                rf_event_script, 
              dev_action );

    /* Put ASM to sleep */
    result &= rf_cdma_asm_device_sleep_rx( logical_dev->sm[band],
                                           mode, band,
                                        rf_event_script, 
                                        dev_action );

    if ( result == FALSE )
    {
      RF_MSG( RF_ERROR, "rf_cdma_mc_create_rx_asm_sleep_script:"
                        "ASM driver failure ");
    }
  }/*if ( NULL != logical_dev->sm )*/

  if ( result == FALSE )
  {
    RF_MSG( RF_ERROR, "rf_cdma_mc_create_rx_asm_sleep_script: Error !! ");
  }

  return result;
} /* rf_cdma_mc_create_rx_asm_sleep_script */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Creates RF device (ASM, PA & QPOET) script for Wakeup Tx based on
  RF mode (1x/HDR)
 
  @details
  This API creates an RF device (ASM) script based on RF mode. This API should 
  be called everytime Tx is tuned. It takes a pointer to RF script buffer as 
  input and may or may not fill in the script depending on whether the RF device 
  that it searches for is present.
 
  @param device 
  RFM device number. 
   
  @param logical_dev
  Logical Dev pointer of which needs to be configured for Tx Wakeup
 
  @param band
  RF CDMA Band
 
  @param mode
  RF mode. 1X or DO.
 
  @param rf_event_script
  RF script buffer to be filled in.
 
  @return
  TRUE for successfully filling in script. False for error condition. 
  Note that if no device is found, TRUE is still returned since it's valid 
*/
boolean
rf_cdma_mc_create_tx_asm_pa_qpoet_wakeup_script
(
  const rfm_device_enum_type device,
  const rfc_cdma_logical_device_params_type *logical_dev,
  const rfm_cdma_band_class_type band,
  const rfm_mode_enum_type mode,
  rf_buffer_intf *rf_event_script
)
{
  boolean result = TRUE;
  rfcom_band_type_u rf_band;
  rfdevice_pa_config_type pa_params_s;
  /* Use device scripting */
  rf_device_execution_type dev_action = RFDEVICE_CREATE_SCRIPT;
  rf_band.cdma_band = band;


  /* Ensure that QPOET bias voltage is set before the PA device writes are 
  done */
  if ( NULL != logical_dev->qpoet_obj[band] )
  {
    void * qpoet_device = logical_dev->qpoet_obj[band];

    /* This API sets the QPOET device to the tech mode,this needs to be
    done because some settings for the device are tech specific*/
    result &= rfdevice_papm_enable( qpoet_device, device, mode, 
                                    rf_band,
                                    RFDEVICE_PAPM_BW_NOT_APPLICABLE,
                                    rf_event_script, RFDEVICE_CREATE_SCRIPT );


    /* Put QPOET to max bias mode  */
    result &= rfdevice_papm_set_mode_bias( qpoet_device, device, mode, rf_band,
                                           RFDEVICE_APT_MODE,
                                           3700, /* max bias */
                                           rf_event_script,
                                           RFDEVICE_CREATE_SCRIPT );
  }/*if ( NULL != logical_dev->qpoet_obj[band] )*/

  if ( NULL != logical_dev->sm[band] )
  {
    /* Wakeup ASM */
    result &= rf_cdma_asm_device_wakeup_tx( logical_dev->sm[band],
                                            mode, band,
                                            rf_event_script,
                                            dev_action );

    /* Configure ASM for Tx */
    result &= rf_cdma_asm_device_enable_tx( logical_dev->sm[band],
                                            mode, band,
                                            rf_event_script,
                                            dev_action ) ;
    if ( result == FALSE )
    {
      RF_MSG( RF_ERROR, "rf_cdma_mc_create_tx_asm_pa_qpoet_wakeup_script: "
              "ASM driver failure");
    }
  }/*if ( NULL != logical_dev->sm[band] )*/

  /* This section will be executed only if there is a valid ASM/PA/QPOET */ 
  if ( NULL != logical_dev->pa[band] )
  {
    void* pa_device = get_cdma_pa_device ( logical_dev->pa[band] ) ;

    if ( pa_device != NULL )
    {
      /* Get PA bandwidth */
      uint32 bw_khz = rf_cdma_data_get_rx_bw_khz(device);
      rfdevice_pa_bw_enum_type rfdevice_pa_bw;
      switch( bw_khz )
      {
        case RF_CDMA_BW_1X:
          rfdevice_pa_bw = RFDEVICE_PA_CDMA_BW_1X;
          break;
        case RF_CDMA_BW_3X:
          rfdevice_pa_bw = RFDEVICE_PA_CDMA_BW_3X;
          break;
        case RF_CDMA_BW_5X:
          rfdevice_pa_bw = RFDEVICE_PA_CDMA_BW_5X;
          break;
        case RF_CDMA_BW_7X:
          rfdevice_pa_bw = RFDEVICE_PA_CDMA_BW_7X;
          break;
        default:
          rfdevice_pa_bw = RFDEVICE_PA_BW_NOT_APPLICABLE;
          result &= FALSE;
        break;
      }

      pa_params_s.mode = mode;
      pa_params_s.band = rf_band;
      pa_params_s.bw = rfdevice_pa_bw;
      pa_params_s.trigger_type = RFDEVICE_PA_TRIGGER_WAKEUP;
      pa_params_s.rfm_device = device;

      /* Wakeup PA */
      result &= rfdevice_pa_wakeup  ( &pa_params_s, pa_device, rf_event_script ,
                                      dev_action, RFCOM_INVALID_TIMING_OFFSET );

      /* Config PA */
      pa_params_s.trigger_type = RFDEVICE_PA_TRIGGER_CONFIG;


      result &= rfdevice_pa_config ( &pa_params_s, pa_device , rf_event_script ,
                                     dev_action,
                                     RFCOM_INVALID_TIMING_OFFSET ) ;

      RF_MSG( RF_MED, "rf_cdma_mc_create_tx_asm_pa_qpoet_wakeup_script: "
                      "Skipping forcing of PA into Low state in wakeup" );

      if (rfm_get_calibration_state() == TRUE )
      {
        /* In Cal mode, Turn PA ON */

        pa_params_s.trigger_type = RFDEVICE_PA_TRIGGER_ON_OFF;
        result &= rfdevice_set_pa_on_off( &pa_params_s, TRUE, pa_device,
                                    rf_event_script, 
                                    dev_action,
                                    RFCOM_INVALID_TIMING_OFFSET ) ;
      }

      if ( result == FALSE )
      {
        RF_MSG( RF_ERROR, "rf_cdma_mc_create_tx_asm_pa_qpoet_wakeup_script: "
                          "PA driver failure");
      }
    }
    else
    {
      RF_MSG( RF_ERROR, "rf_cdma_mc_create_tx_asm_pa_qpoet_wakeup_script: "
                        "NULL PA pointer in cdma_pa object");
    }
  }/*if ( NULL != logical_dev->pa[band] )*/

  if ( result == FALSE )
  {
    RF_MSG( RF_ERROR, "rf_cdma_mc_create_tx_asm_pa_qpoet_wakeup_script: "
                      "Error !! ");
  }

  return result;
} /* rf_cdma_mc_create_tx_asm_pa_qpoet_wakeup_script */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Creates RF device (PA) script for Sleep Tx based on RF mode (1x/HDR)
 
  @details
  This API creates an RF device (ASM) script based on RF mode. This API should 
  be called everytime Tx is stopped. It takes a pointer to RF script buffer as 
  input and may or may not fill in the script depending on whether the RF device 
  that it searches for is present.
 
  @param logical_dev
  Logical Dev pointer of which needs to be configured for Tx Sleep
 
  @param band
  RF CDMA Band
 
  @param mode
  RF mode. 1X or DO.
 
  @param rf_event_script
  RF script buffer to be filled in.
 
  @return
  TRUE for successfully filling in script. False for error condition. 
  Note that if no device is found, TRUE is still returned since it's valid 
*/
boolean
rf_cdma_mc_create_tx_asm_pa_qpoet_sleep_script
(
  const rfm_device_enum_type device,
  const rfc_cdma_logical_device_params_type *logical_dev,
  const rfm_cdma_band_class_type band,
  const rfm_mode_enum_type mode,
  rf_buffer_intf *rf_event_script
)
{
  boolean result = TRUE;
  /* Use device scripting */
  rf_device_execution_type dev_action = RFDEVICE_CREATE_SCRIPT;
  rfdevice_pa_config_type pa_params_s;
  /* This section will be executed only if there is a valid ASM/PA/QPOET */ 
  if ( NULL != logical_dev->pa[band] )
  {
    void* pa_device = get_cdma_pa_device ( logical_dev->pa[band] ) ;

    if ( pa_device != NULL )
    {
      /* Put PA to sleep */
      pa_params_s.mode = mode;
      pa_params_s.band.cdma_band = band;
      pa_params_s.bw = RFDEVICE_PA_BW_NOT_APPLICABLE;
      pa_params_s.trigger_type = RFDEVICE_PA_TRIGGER_SLEEP;
      pa_params_s.rfm_device = RFM_DEVICE_0;

      result &= rfdevice_pa_sleep ( &pa_params_s, pa_device , rf_event_script ,
                                    dev_action, RFCOM_INVALID_TIMING_OFFSET ) ;

      if ( result == FALSE )
      {
        RF_MSG( RF_ERROR, "rf_cdma_mc_create_tx_asm_pa_qpoet_sleep_script: "
                          "PA driver failure");
      }
    }
    else
    {
      RF_MSG( RF_ERROR, "rf_cdma_mc_create_tx_asm_pa_qpoet_sleep_script: "
                        "NULL PA pointer in cdma_pa object");
    }
  }

  /* This section will be executed only if there is a valid ASM */
  if ( NULL != logical_dev->sm[band] )
  {

    /* Put ASM to sleep */
    result &= rf_cdma_asm_device_disable_tx( logical_dev->sm[band],
              mode,
              band,
              rf_event_script,
              dev_action );

    /* Put ASM to sleep */
    result &= rf_cdma_asm_device_sleep_tx( logical_dev->sm[band],
                                           mode, band,
                                           rf_event_script,
                                           dev_action );

    if ( result == FALSE )
    {
      RF_MSG( RF_ERROR, "rf_cdma_mc_create_rx_asm_sleep_script:"
              "ASM driver failure ");
    }
  }/*if ( NULL != logical_dev->sm )*/


    if ( NULL != logical_dev->qpoet_obj[band] )
    {
      void * qpoet_device = logical_dev->qpoet_obj[band];

#ifdef FEATURE_QPOET_STANDBY_FOR_RX_SWITCH
      if ( rfc_commmon_get_rx_qpoet_config_flag() == TRUE )
      {
        rfcom_band_type_u rf_qpoet_band; 
        rf_qpoet_band.cdma_band = band;

        result &= rfdevice_papm_standby(qpoet_device,rf_qpoet_band);

        if ( result == FALSE )
        {
          RF_MSG( RF_ERROR, "FEATURE_QPOET_STANDBY_FOR_RX_SWITCH: "
                            "QPOET standby at Tx sleep failure");
        }
        else
        {
          RF_MSG( RF_HIGH, "FEATURE_QPOET_STANDBY_FOR_RX_SWITCH: "
                           "QPOET standby at Tx sleep success");
        }
      }
      else
      {              
        RF_MSG( RF_MED, "FEATURE_QPOET_STANDBY_FOR_RX_SWITCH: "
                        "RX QPOET FLAG OFF");
        /* Disable power tracker for this particular mode */
        result &= rfdevice_papm_disable( qpoet_device, device, mode,
                                       rf_event_script, dev_action );
      }
#else
      /* Disable power tracker for this particular mode */
      result &= rfdevice_papm_disable( qpoet_device, device, mode,
                                       rf_event_script, dev_action );
#endif

      if ( result == FALSE )
      {
        RF_MSG( RF_ERROR, "rf_cdma_mc_create_tx_asm_pa_qpoet_sleep_script: "
                          "QPOET driver failure");
      }
    }/*if ( NULL != logical_dev->qpoet_obj[band] )*/

  if ( result == FALSE )
  {
    RF_MSG( RF_ERROR, "rf_cdma_mc_create_tx_asm_pa_qpoet_sleep_script: "
                      "Error !! ");
  }

  return result;
} /* rf_cdma_mc_create_tx_asm_pa_qpoet_sleep_script */


/*! @} */

/*============================================================================*/
/*!
  @name MC layer CCS event handle APIs

  @brief
  This section contains CCS event handle APIs
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief 
  Clean up RF CCS event handle. Free up allocated handle objects. Reset event 
  status bit field. 
  
  @param device 
  RFM device 
   
  @param event_type 
  RF CCS event handle to be cleaned up 
 
  @return
  True for success. False for failure
*/
boolean
rf_cdma_mc_cleanup_ccs_event
(
  rfm_device_enum_type device,
  rf_cdma_event_type event_type
)
{
  boolean result = TRUE;
  /* Obtain handle ptr from cdma data structure */
  rfcommon_mdsp_event_handle** event_handle = 
                           rf_cdma_data_get_event_handle( device, event_type );

  if( event_handle != NULL )
  {
    /* Reset event status bit field to IDLE no matter which state it is in.*/
    result &= rf_cdma_mdsp_ccs_abort_event( event_handle );

    /* Free up event handle object and related memory */
    result &= rf_cdma_mdsp_ccs_cleanup_event( event_handle );
  }
  else
  {
    RF_MSG( RF_ERROR, "rf_cdma_mc_cleanup_ccs_event: "
                      "Trying to clean up NULL handle");
  }

  return result;

}/* rf_cdma_mc_cleanup_ccs_event */

/*----------------------------------------------------------------------------*/
/*!
  @brief 
  Helper API to get current XSW position and translate it into coupler port
  the XSW position and coupler port are going to be the same 
  
  @param device 
  RFM device 
   
  @return
  Couple port to use, 0xFF in case there is no valid position
*/
static uint8
rf_cdma_mc_get_coupler_port
(
  rfm_device_enum_type device
)
{
  /* The couple port to send*/
  uint8 coupler_port;

  #ifdef FEATURE_RF_ASDIV
  rfcommon_asdiv_position_type position = 0;

  /* Get current position of XSW */
  position = rfcommon_asdiv_get_current_position( device );

  /* Translate XSW position to coupler port */
  switch ( position )
  {
     case ASDIV_POSITION_0:
       coupler_port = 0;
     break;

     case ASDIV_POSITION_1:
       coupler_port = 1;
     break;

     case ASDIV_POSITION_MAX:
     default:
       coupler_port = 0xFF;
     break;
  }
  #else
    coupler_port = 0;
  #endif /* ! FEATURE_RF_ASDIV */

  /* return the couple port */
  return coupler_port;

}/* rf_cdma_mc_get_coupler_port */

/*----------------------------------------------------------------------------*/
/*!
  @brief 
  Configure the RF device and RF Front-end to enable RX to
  retune RF to new band/chan

  @details
  -# This function calls the RF device API that creates a script containing all 
  settings needed to put the RF device to enable Rx
  -# Uses the grfc_script that has all the GRFC settings to be used for 
  enabling RX on the device
  -# Sends the above grfc script and RF device script to firmware, which then
  programs the RF front-end when triggered by RFSW via FW commands (such as
  RX_START)
   
  @param device
  RF device that needs to be enabled for RX
 
  @param rf_mode
  used to check if the device is in 1x or 1xEVDO mode
 
  @param rf_state
  used to check if the device is RX or RXTX state
 
  @param rxlm_handle
  LM handle for the device
 
  @param band
  RF band to which the device is to be tuned to 
 
  @param chan
  RF band to which the device is to be tuned to
 
  @param power_mode
  The I-ceiver power mode in which the RF device is to be tuned to (high-lin or 
  low-lin)
 
  @param rx_bw_khz
  Receive BW in KHz
 
  @param tx_bw_khz
  Tramsmit BW in KHz
 
  @param logical_dev
  Contains RFC data needed for opearation of the device
 
  @param script_mode
  used to check if tuning is for IRAT or normal mode
 
  @param tx_static_nv_cal_ptr
  The Tx NV data structure that max power limit information must
  be loaded into.
 
  @param ant_tuner_script_token_ptr
  The pointer to the data structure that containts the antennae
  tuner script.

  @param rf_state
  The RF state for which to configure front end

  @return
  Flag indicating a success (TRUE) or failure (FALSE)
*/
boolean
rf_cdma_mc_configure_front_end_for_retune 
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const rfm_cdma_band_class_type new_band,
  const rfm_cdma_chan_type new_chan,
  const rfm_cdma_power_mode_type power_mode,
  const uint32 rx_bw_khz,
  const uint32 tx_bw_khz,
  const rfdevice_cdma_script_operation_type script_mode,
  const rfc_cdma_logical_device_params_type *logical_dev,
  const rf_cdma_tx_static_type *tx_static_nv_cal_ptr,
  ant_tuner_device_tokens_type* ant_tuner_script_token_ptr,
  rf_cdma_state_type rf_state,
  rfm_cdma_band_class_type old_band,
  rf_buffer_intf* rx_tx_settings
)
{
  boolean result = TRUE;
  const rf_cdma_data_status_type *dev_status_r;/* Dev Status Read Ptr */
  rf_cdma_data_status_type *dev_status_w;/* Dev Status write Ptr */
  rfdevice_coupler_tech_cfg_type coupler_cfg;

  /* buffer that holds settings related to TX wakeup - a separate buffer is
     needed since Tx script for the RF device can potentially be very large */
  rf_buffer_intf* tx_wakeup_settings = NULL ;

  uint8 num_carriers;
  boolean is_xpt_enabled_for_hdr;

  dev_status_r = rf_cdma_get_device_status( device ) ;
  dev_status_w = rf_cdma_get_mutable_device_status( device ) ;

  /* Perform NULL Pointer check */
  if ( dev_status_r == NULL || dev_status_w == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_mc_configure_front_end_for_retune(): "
                        "NULL data for device %d", device );
    return FALSE;
  }

  /* Create generic buffer interface object for SSBI/RFFE/GRFC,
     allocate memory to max script sizes, since the memory is released when 
     object is destroyed later on.
     tx_wakeup_settings is used only for wakeup_tx, the remaining settings go
     into rx_tx_settings */

  tx_wakeup_settings = rf_buffer_create (  RF_CDMA_MAX_SSBI_SCRIPT_SIZE ,
                                     RF_CDMA_MAX_RFFE_SCRIPT_SIZE , 
                                     RF_CDMA_MAX_GRFC_SCRIPT_SIZE ) ;


  /*Check if valid pointer was returned for buffer object*/
  if( rx_tx_settings == NULL || tx_wakeup_settings == NULL)
  {
    RF_MSG( RF_ERROR, "rf_cdma_mc_configure_front_end_for_retune: "
                      "Unable to create buffer object ");
    result = FALSE;
  }
  else
  {
    /* Get current band */
    rfm_cdma_band_class_type band_to_sleep = old_band;

    /* Get the WTR Rx Sleep script for Old band and chan and Tune script for new 
       band & chan */ 
    if ( rf_state == RF_CDMA_STATE_RXDIV )
    {
      result &= rfdevice_cdma_create_rx_disable_diversity_script(
                    logical_dev->rx_device[old_band],
                    rx_tx_settings,
                    rx_bw_khz, tx_bw_khz );

      result &= rfdevice_cdma_create_rx_enable_diversity_script(
                          logical_dev->rx_device[new_band], 
                          dev_status_r->rf_mode, 
                          new_band, new_chan, rx_bw_khz, tx_bw_khz, power_mode, 
                  rx_tx_settings );
    }
    else
    {
      /* Populate device info for callback data */
      dev_status_w->ssma_cb_data.device = device;

      result &= rfdevice_cdma_create_rx_tune_script( 
                                      logical_dev->rx_device[new_band], 
                                      dev_status_r->rf_mode, new_band, new_chan, 
                                      rx_bw_khz, tx_bw_khz, power_mode, 
                  rx_tx_settings, script_mode,
                  (rf_cdma_spur_cb_type)rf_cdma_mc_ssma_notch_callback,
                  &(dev_status_w->ssma_cb_data) );

    }/* if !( dev_status_r->rf_state == RF_CDMA_STATE_RXDIV ) */
    /* Check if in RxTx state,append GRFC Tx sleep for previous band
    to the script . These Two checks for device to be in RXTX state are added 
    here for scenarios where we might have the same GRFC pins being used for Rx 
    and Tx, we need to preserve the order in which we populate GRFC scripts .
    the order that should be followed is: Sleep Tx on old band -> Sleep Rx on 
    old band -> wakeup Rx on new band -> wakeup Tx in new band . With this 
    order and proper delay params provided in between  the final GRFC state is 
    the desired one*/ 
    if ( rf_state == RF_CDMA_STATE_RXTX )
    {
      result &= rfc_cdma_generate_tx_sleep_grfc_script( device, band_to_sleep,
                rx_tx_settings,
                0 /*delay*/ );
    }

    /* Get SLeep Rx GRFC Programming Script for the current Band  */
    result &= rfc_cdma_generate_rx_sleep_grfc_script( device, band_to_sleep, 
              rx_tx_settings ,
              0 /* delay*/  );

    /* Get Sleep Rx devices (ASM) scripts for the current band */
    result &= rf_cdma_mc_create_rx_asm_sleep_script(logical_dev, 
                                                    band_to_sleep,
                                                    dev_status_r->rf_mode ,
              rx_tx_settings );

    /* Get Wakeup Rx GRFC Programming Script for NEW Band */
    result &= rfc_cdma_generate_rx_wakeup_grfc_script( device, 
                                                new_band,
              rx_tx_settings,
                                                RF_CDMA_GRFC_SCRIPT_START_DELTA 
                                                /* delay*/ );

    /* Get Wakeup Rx devices (ASM) scripts for the new band */
    result &= rf_cdma_mc_create_rx_asm_xsw_wakeup_script( device,
              logical_dev,
              new_band,
              dev_status_r->rf_mode ,
              rx_tx_settings );

    /* Check if in RxTx state, append GRFC Tx wakeup for new band
    to the script */
    if ( rf_state == RF_CDMA_STATE_RXTX )
    {
      /* Get GRFC scripts for new band Tx to wake up */
      result &= rfc_cdma_generate_tx_wakeup_grfc_script( device, 
                new_band,rx_tx_settings,
                                               RF_CDMA_GRFC_SCRIPT_START_DELTA
                                               /* delay*/  );
    }


    /* Generate "retune with Tx Off" script for everything but WTR 
       if not in RXTX state  */
    if( rf_state == RF_CDMA_STATE_RXDIV ||
        rf_state == RF_CDMA_STATE_RX )
    {

      if ( logical_dev->ant_tuner[band_to_sleep] != NULL &&
           logical_dev->ant_tuner[band_to_sleep] != logical_dev->ant_tuner[new_band] &&
           logical_dev->ant_tuner[new_band] != NULL )
      {

#ifdef FEATURE_RF_ASDIV
        /* Lock the ASDiv manager interface to avoid concurrency of ASDiv and Tuner configuration*/
        rfcommon_asdiv_manager_enter_lock();
#endif

        /* Put the current tuner to sleep if the new band doesn't use the same device */
        result &= rf_cdma_atuner_disable(logical_dev->ant_tuner[band_to_sleep],
                                         RFDEVICE_CREATE_SCRIPT,
                                         rx_tx_settings,
                                         0, /* No timing offset */
                                         ant_tuner_script_token_ptr);
        /* Initialize the new tuner */
        result &=
          rf_cdma_atuner_init(logical_dev->ant_tuner[new_band],
                                              dev_status_r->rf_mode,
                              RFDEVICE_CREATE_SCRIPT,
                              rx_tx_settings,
                                              0, /* No timing offset */
                              ant_tuner_script_token_ptr);

#ifdef FEATURE_RF_ASDIV
        /* Releae the ASDIV manager interface */
        rfcommon_asdiv_manager_release_lock();
#endif

      } /* Old band antenna tuner isn't NULL or is different */
      if ( logical_dev->ant_tuner[new_band] != NULL )
      {
        rfcom_band_type_u band_in_common_format;
        rfcom_antenna_tuner_priority_cfg tuner_priority_cfg;

        band_in_common_format.cdma_band = new_band;
        tuner_priority_cfg.rfm_device = device;

#ifdef FEATURE_RF_ASDIV
        /* Lock the ASDiv manager interface to avoid concurrency of ASDiv and Tuner configuration*/
        rfcommon_asdiv_manager_enter_lock();
#endif
        result &=
          rf_cdma_atuner_configure_for_rx(tuner_priority_cfg,
                                          logical_dev->ant_tuner[new_band],
                                          dev_status_r->rf_mode,
                                          band_in_common_format,
                                          new_chan,
                                          RFDEVICE_CREATE_SCRIPT,
                                          rx_tx_settings,
                                          0, /* No timing offset */
                                          ant_tuner_script_token_ptr );
#ifdef FEATURE_RF_ASDIV
        /* Releae the ASDIV manager interface */
        rfcommon_asdiv_manager_release_lock();
#endif

      } /* New band antenna tuner isn't NULL */
    }/*if device is in RX or RxDiv state*/
    /* Generate "retune with Tx On" script for everything but WTR 
       if in RXTX state  */
    else if ( rf_state == RF_CDMA_STATE_RXTX )
    {
      /* Populate WTR Wxe Preload and trigger scripts */
      rfdevice_meas_script_data_type script_data_ptr = {
        /* Execute the pre load part of the script immediately */
        RFDEVICE_MEAS_EXECUTE_IMMEDIATE ,
        /* NULL since we dont except any script back */
        NULL ,
        /* The trigger sctipt that FW will execute */
        RFDEVICE_MEAS_CREATE_SCRIPT ,
        /* Append the WTR trigger script */
        tx_wakeup_settings ,
        /* Default WXE index */
        0
      };

      if( tx_static_nv_cal_ptr == NULL )
      {
        RF_MSG( RF_ERROR, "rf_cdma_mc_configure_front_end_for_retune: "
                          "NULL TX static NV pointer for retune TX !! ");
        return FALSE;
      }

      if( logical_dev->ant_tuner[band_to_sleep] != NULL )
      {
        rfcom_band_type_u rf_band_to_sleep;

        rf_band_to_sleep.cdma_band = band_to_sleep;
#ifdef FEATURE_RF_ASDIV
        /* Lock the ASDiv manager interface to avoid concurrency of ASDiv and Tuner configuration*/
        rfcommon_asdiv_manager_enter_lock();
#endif
        /* Disable Atuner Tx. Clears timer in Tuner CL */
        result &= rf_cdma_atuner_tx_disable(logical_dev->ant_tuner[band_to_sleep],
                                              dev_status_r->rf_mode,
                                              rf_band_to_sleep,
                                              dev_status_r->curr_chans[RFM_CDMA_CARRIER_0],
                                            RFDEVICE_CREATE_SCRIPT,
                                            rx_tx_settings,
                                            0,
                                            ant_tuner_script_token_ptr );
#ifdef FEATURE_RF_ASDIV
        /* Releae the ASDIV manager interface */
        rfcommon_asdiv_manager_release_lock();
#endif

      }

      if ( logical_dev->ant_tuner[band_to_sleep] != NULL &&
           logical_dev->ant_tuner[band_to_sleep] != logical_dev->ant_tuner[new_band] &&
           logical_dev->ant_tuner[new_band] != NULL )
      {
#ifdef FEATURE_RF_ASDIV
        /* Lock the ASDiv manager interface to avoid concurrency of ASDiv and Tuner configuration*/
        rfcommon_asdiv_manager_enter_lock();
#endif

        /* Put the current tuner to sleep if the new band doesn't use the same device */
        result &= rf_cdma_atuner_disable(logical_dev->ant_tuner[band_to_sleep],
                                         RFDEVICE_CREATE_SCRIPT,
                                         rx_tx_settings,
                                              0, /* No timing offset */
                                         ant_tuner_script_token_ptr);

        /* Initialize the new tuner */
        result &=
          rf_cdma_atuner_init(logical_dev->ant_tuner[new_band],
                              dev_status_r->rf_mode ,
                              RFDEVICE_CREATE_SCRIPT,
                              rx_tx_settings,
                              0, /* No timing offset */
                              ant_tuner_script_token_ptr);
#ifdef FEATURE_RF_ASDIV
        /* Releae the ASDIV manager interface */
        rfcommon_asdiv_manager_release_lock();
#endif

      } /* Old band antenna tuner isn't NULL or is different */
      if ( logical_dev->ant_tuner[new_band] != NULL )
      {
        rfcom_band_type_u band_in_common_format;
        rfcom_antenna_tuner_priority_cfg tuner_priority_cfg;

        band_in_common_format.cdma_band = new_band;
        tuner_priority_cfg.rfm_device = device;

        result &=
          rf_cdma_atuner_configure_for_rx(tuner_priority_cfg,
                                          logical_dev->ant_tuner[new_band],
                                          dev_status_r->rf_mode,
                                          band_in_common_format,
                                          new_chan,
                                          RFDEVICE_CREATE_SCRIPT,
                                          rx_tx_settings,
                                          0, /* No timing offset */
                                          ant_tuner_script_token_ptr );
        result &=
          rf_cdma_atuner_configure_for_tx( rf_cdma_data_get_txlm_handle (device) ,
                                          logical_dev->ant_tuner[new_band],
                                          dev_status_r->rf_mode,
                                          band_in_common_format,
                                          new_chan,
                                          RFDEVICE_CREATE_SCRIPT,
                                          rx_tx_settings,
                                          0, /* No timing offset */
                                          ant_tuner_script_token_ptr,
                                          (void*)&(tx_static_nv_cal_ptr->ant_tuner_cl) );
      } /* New band antenna tuner isn't NULL */

      num_carriers = dev_status_r->num_carriers;
      is_xpt_enabled_for_hdr = rf_cdma_xpt_is_enabled_for_hdr(num_carriers);

      if ( NULL != logical_dev->coupler_obj[new_band] )
      { /* Get Tx coupler scripts */
        coupler_cfg.mode = dev_status_r->rf_mode;
        coupler_cfg.band.cdma_band = new_band;
        /* Hard code couple direction , this is known in the coupler driver*/
        coupler_cfg.direction = RFDEVICE_COUPLER_DIRECTION_FWD;
        /* Query coupler port based on XSW position */
        coupler_cfg.port = rf_cdma_mc_get_coupler_port( device );
        result &= rfdevice_coupler_config( logical_dev->coupler_obj[new_band],
                                           coupler_cfg,
                                           rx_tx_settings,
                                           RFDEVICE_CREATE_SCRIPT,
                                           0);
      }/*if( NULL != logical_dev->coupler_obj[band] )*/

      /* Get Sleep Tx device (WTR) scripts for the old band */
      result &= rfdevice_cdma_create_tx_sleep_script ( 
                               logical_dev->tx_device[old_band], 
                               tx_wakeup_settings );

      /* Get Wakeup Tx device (WTR) scripts for the new band */
      result &= rfdevice_cdma_create_tx_tune_script( 
                  logical_dev->tx_device[new_band],
                                             dev_status_r->rf_mode, 
                                             new_band, 
                                             new_chan, 
                                             tx_bw_khz, 
                  (rfdevice_dpd_enum_type)(rf_cdma_xpt_is_enabled( device )&& 
                  is_xpt_enabled_for_hdr ),
                  &script_data_ptr,
		  FALSE ) ;

      /* Get Wake up Tx devices (PA, ASM & QPOET ) scripts for the
         new band */
      result &= rf_cdma_mc_create_tx_asm_pa_qpoet_wakeup_script( device,
                                                             logical_dev,
                                                             new_band,
                                                             dev_status_r->rf_mode ,
                tx_wakeup_settings ) ;

     if ( rfm_get_calibration_state() == FALSE )
      {
        /* update rf_on/off settings in SM */
        result &= rf_cdma_mc_configure_tx_on_off( device, new_band, dev_status_r->rf_mode ,
                                               dev_status_r->script_index );
      }
      else
      {
        /* In cal mode, FW does not look at the shared memory - so append RF_ON
           script to static event instead .. no need to append RF_OFF settings*/
        result &= rf_cdma_get_rf_onoff_settings( device, dev_status_r->rf_mode,
                  new_band, TRUE /* RFON */,
                  logical_dev,
                  tx_wakeup_settings );
      }
    } /* if ( rf_state == RF_CDMA_STATE_RXTX ) */

    if ( result == TRUE )
    {
      /* Do immediate write for everything */
      rf_cdma_mc_do_immediate_script_write( rx_tx_settings );

      DALSYS_BusyWait(rf_cdma_debug_flags.retune_delay) ;

      rf_cdma_mc_do_immediate_script_write( tx_wakeup_settings );
    }
    else
    {
      RF_MSG( RF_ERROR, "rf_cdma_mc_configure_front_end_for_retune: Error !! ");
    }

  } /*if ( retune_script_immediate != NULL )*/

  if ( tx_wakeup_settings != NULL)
  {
    /* De-allocate memory for script container */
    rf_buffer_destroy( tx_wakeup_settings );
  }

  /* The following sequence finds out the current mode the device is in 
  (1x/HDR), it populates the shared memory with Rx ON/OFF info for 
   the new band */
  if ( result == TRUE )
  {
    /* Get current mode 1x/HDR */
    rfm_mode_enum_type curr_mode;
    curr_mode = rf_cdma_data_get_rf_mode( device );
    /* If current mode is HDR, populate shared mem with Rx ON/OFF event info */
    if ( curr_mode == RFM_1XEVDO_MODE )
    {
      RF_MSG_2 ( RF_LOW, "rf_cdma_mc_configure_front_end_for_retune: "
                       "Configuring Rx on/off scripts for Device %d band:%d" , 
                 device, new_band  );
      result = rf_cdma_mc_configure_rx_on_off( device, new_band, curr_mode,
                                                 dev_status_r->script_index  );
    }/*if ( curr_mode == RFM_1XEVDO_MODE )*/
    else
    {
      RF_MSG ( RF_LOW, "rf_cdma_mc_configure_front_end_for_retune: "
                        "Configure Rx ON/OFF supported only for HDR mode" ); 
    }
  }/*if ( result == TRUE )*/
  else
  {
    RF_MSG( RF_ERROR, "rf_cdma_mc_configure_front_end_for_retune: FAILED.");
  }

  return result;
} /* rf_cdma_mc_configure_front_end_for_retune */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure the RF device and RF Front-end to disable RX 

  @details
  -# This function calls the RF device API that creates a script containing all 
  settings needed to put the RF device to disable Rx
  -# Uses the grfc_script that has all the GRFC settings to be used for 
  disabling RX on the device
  -# Sends the above grfc script and RF device script to firmware, which then
  programs the RF front-end when triggered by RFSW via FW commands (such as
  RX_STOP)
   
  @param device
  RF device that needs to be disabled for RX
 
  @param rxlm_handle
  LM handle for the device
 
  @param is_slave_device
  used to check if device is in diversity mode
 
  @param band
  RF band to which the device is to be tuned to 
 
  @param script_index
  script_index - to be used by FW
 
  @param rx_bw_khz
  Receive BW in KHz
 
  @param tx_bw_khz
  Tramsmit BW in KHz
 
  @param logical_dev
  Contains RFC data needed for opearation of the device
 
  @param ant_tuner_script_token_ptr The pointer to the data
  structure that containts the antennae tuner script.
 
  @return
  Flag indicating a success (TRUE) or failure (FALSE)
*/
boolean
rf_cdma_mc_configure_front_end_for_rx_sleep
(  
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const boolean is_slave_device,
  const rfm_cdma_band_class_type band,
  const uint32 script_index,
  const uint32 rx_bw_khz,
  const uint32 tx_bw_khz,
  const rfc_cdma_logical_device_params_type *logical_dev,
  ant_tuner_device_tokens_type* ant_tuner_script_token_ptr
)
{

  boolean result = TRUE;
  /* Pointer to script buffer*/
  rf_buffer_intf *sleep_rx_script = NULL;
  rfm_mode_enum_type rf_mode = rf_cdma_data_get_rf_mode( device ) ;
  /* We should be in HDR or 1X mode already but if not 
  we are in Invalid mode something is very wrong */
  if ( rf_mode != RFM_INVALID_MODE )
  {
    /* Create generic buffer interface object for SSBI/RFFE/GRFC,
       allocate memory to max script sizes, since the memory is released when 
       object is destroyed later on anyway, APIs returning script size on the fly 
       aren't ready, all driver APIs hard coding it internally */
    sleep_rx_script = rf_buffer_create ( RF_CDMA_MAX_SSBI_SCRIPT_SIZE , 
                                         RF_CDMA_MAX_RFFE_SCRIPT_SIZE , 
                                         RF_CDMA_MAX_GRFC_SCRIPT_SIZE ) ;

    /*Check if valid pointer was returned for buffer object*/
    if( sleep_rx_script == NULL )
    {
      RF_MSG( RF_ERROR, "rf_cdma_mc_configure_front_end_for_rx_sleep: "
                        "Unable to create buffer object ");
      result = FALSE;
    }/*if( sleep_rx_script == NULL )*/
    else
    {
      /*--------------------------------------------------------------------------*/

      if ( is_slave_device == TRUE )
      {
        /* this is diversity device:: get the script needed to put RF device to 
           sleep */ 
        result &= rfdevice_cdma_create_rx_disable_diversity_script(
                                                 logical_dev->rx_device[band], 
                                                 sleep_rx_script, 
                                                 rx_bw_khz, tx_bw_khz );
      }/*if ( is_slave_device == TRUE )*/
      else
      {
        /* this is the main device */
        result &= rfdevice_cdma_create_rx_sleep_script( logical_dev->rx_device[band], 
                                                        sleep_rx_script, rx_bw_khz, 
                                                        tx_bw_khz ); 
      }/*if ( is_slave_device != TRUE  )*/

      /* Get GRFC Programming Script for the current Band */
      result &= rfc_cdma_generate_rx_sleep_grfc_script( device, 
                                                        band, 
                                                        sleep_rx_script,
                0 /* delay*/ );

      /* Get Sleep Rx devices (ASM) scripts for the current band */
      result &= rf_cdma_mc_create_rx_asm_sleep_script( logical_dev, 
                                                       band,
                                                       rf_mode,
                sleep_rx_script );

      if ( logical_dev->ant_tuner[band] != NULL )
      {
#ifdef FEATURE_RF_ASDIV
        /* Lock the ASDiv manager interface to avoid concurrency of ASDiv and Tuner configuration*/
        rfcommon_asdiv_manager_enter_lock();
#endif       
        result &= rf_cdma_atuner_disable(logical_dev->ant_tuner[band],
                                                  RFDEVICE_CREATE_SCRIPT,
                                                  sleep_rx_script,
                                                  0, /* No timing offset */
                                                  ant_tuner_script_token_ptr);
#ifdef FEATURE_RF_ASDIV
        /* Releae the ASDIV manager interface */
        rfcommon_asdiv_manager_release_lock();
#endif

      }
      #ifdef FEATURE_QPOET_STANDBY_FOR_RX_SWITCH
      if ( rfc_commmon_get_rx_qpoet_config_flag() == TRUE )
      {
         if ( NULL != logical_dev->qpoet_obj[band] )
         {
           /* Disable power tracker for this particular mode */
           result &= rfdevice_papm_disable( logical_dev->qpoet_obj[band], 
                                            device,
                                            rf_mode ,
                                            sleep_rx_script, 
                                            RFDEVICE_CREATE_SCRIPT );  
           if ( result == FALSE )
           {
             RF_MSG( RF_ERROR, "FEATURE_QPOET_STANDBY_FOR_RX_SWITCH: "
                               "QPOET disable at Rx sleep failure");
           }
           else
           {
             RF_MSG( RF_HIGH, "FEATURE_QPOET_STANDBY_FOR_RX_SWITCH: "
                              "RX QPOET disable at Rx sleep success");
           }
         }
         else
         {
           RF_MSG( RF_HIGH, "FEATURE_QPOET_STANDBY_FOR_RX_SWITCH: "
                            "RX QPOET FLAG ON. NO QPOET in 1x logical dev");
         }
      }
      else
      {
        RF_MSG( RF_MED, "FEATURE_QPOET_STANDBY_FOR_RX_SWITCH: "
                        "RX QPOET FLAG OFF");
      }

      #endif
    }/*if ! ( sleep_rx_script == NULL )*/

    if ( result == TRUE )
    {
      rfcommon_mdsp_event_handle** analog_script_event_handle =
        rf_cdma_data_get_event_handle( device, RF_CDMA_STATIC_EVENT );

      /* send device settings to FW */
      result &= rf_cdma_mdsp_configure_devices( device ,
                                                logical_dev->rx_mdsp_path,
                                                rf_mode,
                                                RF_CDMA_MDSP_CCS_EVENT_SLEEP_RX_ID,
                                                analog_script_event_handle,
                                                (rf_cdma_mdsp_get_event_info(RF_CDMA_MDSP_CCS_EVENT_SLEEP_RX_ID)),
                                                sleep_rx_script, 
                                                script_index,
                                                rf_cdma_data_get_rxlm_handle (device),
                                                NULL, NULL );
    }

    if ( result == FALSE )
    {  
      RF_MSG( RF_ERROR, "rf_cdma_mc_configure_front_end_for_rx_sleep: "
                        "Error !! ");
    }

    if ( sleep_rx_script != NULL )
    {
      /* De-allocate memory for GRFC script container */
      rf_buffer_destroy( sleep_rx_script );
    }
  } /* rf_mode == RFM_INVALID_MODE */
  else
  {
    result = FALSE ;
    RF_MSG ( RF_ERROR , "rf_cdma_mc_configure_front_end_for_rx_sleep(): "
             "rf_cdma_data_get_rf_mode()returned RFM_INVALID_MODE" ) ;
  }
  return result;

} /* rf_cdma_mc_configure_front_end_for_rx_sleep */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure the RF device and RF Front-end to disable Tx

  @details
  -# This function calls the RF device API that creates a script containing all 
  settings needed to put the RF device to disable Tx
  -# Uses the grfc_script that has all the GRFC settings to be used for 
  disabling Tx on the device
  -# Sends the above grfc script and RF device script to firmware, which then
  programs the RF front-end when triggered by RFSW via FW commands (such as
  TX_STOP)
   
  @param device
  RF device that needs to be disabled for TX
 
  @param txlm_handle
  LM handle for the device
 
  @param script_index
  script_index - to be used by FW 
 
  @param logical_dev
  Contains RFC data needed for opearation of the device
 
  @param ant_tuner_script_token_ptr The pointer to the data
  structure that containts the antennae tuner script.
 
  @return
  Flag indicating a success (TRUE) or failure (FALSE)
*/
boolean
rf_cdma_mc_configure_front_end_for_tx_sleep
(
  const rfm_device_enum_type device,
  const lm_handle_type txlm_handle,
  const rfm_cdma_band_class_type band,
  const uint32 script_index,
  const rfc_cdma_logical_device_params_type *logical_dev,
  ant_tuner_device_tokens_type* ant_tuner_script_token_ptr
)
{
  boolean result = TRUE;
  const rf_cdma_data_status_type* 
    dev_status_r = rf_cdma_get_device_status( device ) ;
  rfdevice_coupler_tech_cfg_type coupler_cfg;

  if ( NULL != dev_status_r )
  {
    /* Pointer to script buffer*/
    rf_buffer_intf *sleep_tx_script = NULL;
    rfm_mode_enum_type rf_mode = dev_status_r->rf_mode ;

    rfcommon_mdsp_event_handle** analog_script_event_handle = 
      rf_cdma_data_get_event_handle( device, RF_CDMA_STATIC_EVENT ) ;

    /* Create generic buffer interface object for SSBI/RFFE/GRFC,
       allocate memory to max script sizes, since the memory is released when 
       object is destroyed later on anyway, APIs returning script size on the fly 
       aren't ready, all driver APIs hard coding it internally */
    sleep_tx_script = rf_buffer_create ( RF_CDMA_MAX_SSBI_SCRIPT_SIZE , 
                                         RF_CDMA_MAX_RFFE_SCRIPT_SIZE , 
                                         RF_CDMA_MAX_GRFC_SCRIPT_SIZE ) ;

    /*Check if valid pointer was returned for buffer object*/
    if( sleep_tx_script == NULL )
    {
      RF_MSG( RF_ERROR, "rf_cdma_mc_configure_front_end_for_tx_sleep: "
                        "Unable to create buffer object ");
      result = FALSE;
    }/*if( sleep_tx_script == NULL )*/
    else
    {
      /*---------------------------------------------------------------------*/
      /* Compute RF device Programming Script for the given Band and Channel */
      result &= rfdevice_cdma_create_tx_sleep_script( logical_dev->tx_device[band],
                                                      sleep_tx_script );

      /* Compute GRFC Programming Script for the given Band and Channel */
      result &= rfc_cdma_generate_tx_sleep_grfc_script( device, 
                                                        band, 
                                                        sleep_tx_script,
                0 /* delay */);



      /* Get Sleep Tx devices (PA) scripts for the current band */
      result &= rf_cdma_mc_create_tx_asm_pa_qpoet_sleep_script( device,
                                                                logical_dev,
                                                                band,
                                                                rf_mode,
                                                                sleep_tx_script );

      if ( logical_dev->ant_tuner[band] != NULL )
      {
        rfcom_band_type_u band_in_common_format;
        rfcom_antenna_tuner_priority_cfg tuner_priority_cfg;

        band_in_common_format.cdma_band = band;
        tuner_priority_cfg.rfm_device = device;

#ifdef FEATURE_RF_ASDIV
        /* Lock the ASDiv manager interface to avoid concurrency of ASDiv
           and Tuner configuration*/
        rfcommon_asdiv_manager_enter_lock();
#endif

        /* Disable Atuner Tx. Clears timer in Tuner CL */
        result &= rf_cdma_atuner_tx_disable(logical_dev->ant_tuner[band],
                                            dev_status_r->rf_mode,
                                            band_in_common_format,
                                            dev_status_r->lo_tuned_to_chan,
                                            RFDEVICE_CREATE_SCRIPT,
                                            sleep_tx_script,
                                            0,
                                            ant_tuner_script_token_ptr );

        /* Disabling Tx puts the tuner back into Rx mode */
        result &=
          rf_cdma_atuner_configure_for_rx(tuner_priority_cfg,
                                              logical_dev->ant_tuner[band],
                                              rf_mode,
                                              band_in_common_format,
                                              dev_status_r->lo_tuned_to_chan,
                                              RFDEVICE_CREATE_SCRIPT,
                                              sleep_tx_script,
                                              0, /* No timing offset */
                                              ant_tuner_script_token_ptr );
#ifdef FEATURE_RF_ASDIV
        /* Releae the ASDIV manager interface */
        rfcommon_asdiv_manager_release_lock();
#endif

      } /* if ( logical_dev->ant_tuner[band] != NULL ) */

      if ( NULL != logical_dev->coupler_obj[band] )
      { /* Get Tx disable coupler scripts */
        coupler_cfg.mode = rf_mode;
        coupler_cfg.band.cdma_band = dev_status_r->curr_band ;
        /* Hard code coupler direction , this is known in the coupler driver*/
        coupler_cfg.direction = RFDEVICE_COUPLER_DIRECTION_OPEN;
        /* Query for coupler port based on XSW position */
        coupler_cfg.port = rf_cdma_mc_get_coupler_port( device );
        result &= rfdevice_coupler_config( logical_dev->coupler_obj[band],
                                           coupler_cfg,
                                           sleep_tx_script,
                                           RFDEVICE_CREATE_SCRIPT,
                                           0);
      }/*if( NULL != logical_dev->coupler_obj[band] )*/
    }/*if ! ( sleep_tx_script == NULL )*/

    if( result == TRUE ) 
    {   
      /* Push Analog frontend Script to MDSP */
      result &= rf_cdma_mdsp_configure_devices( device ,
                logical_dev->tx_mdsp_path,
                                                rf_mode,
                                                RF_CDMA_MDSP_CCS_EVENT_SLEEP_TX_ID,
                                                analog_script_event_handle,
                                                (rf_cdma_mdsp_get_event_info(RF_CDMA_MDSP_CCS_EVENT_SLEEP_TX_ID)),
                                                sleep_tx_script, 
                script_index,
                rf_cdma_data_get_txlm_handle (device) ,
                NULL, NULL );
    }/*if( result == TRUE )*/

    if ( result == FALSE )
    {  
      RF_MSG( RF_ERROR, "rf_cdma_mc_configure_front_end_for_tx_sleep: Error !! ");
    }

    if ( sleep_tx_script != NULL )
    {
      /* De-allocate memory for GRFC script container */
      rf_buffer_destroy( sleep_tx_script );
    }
  }
  return result;
} /* rf_cdma_mc_configure_front_end_for_tx_sleep */
 
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configures the RF Antenna tuner for RX

  @param logical_dev
  Contains RFC data needed for opearation of the device

  @param rf_mode
  used to check if the device 1x or 1xEVDO mode

  @param wakeup_rx_script
  Script pointer to the buffer interface which will have the
  antennae tuner script appened to it.

  @param band
  RF band to which the device is to be tuned to

  @param chan
  RF band to which the device is to be tuned to

  @param ant_tuner_script_token_ptr
  The pointer to the data structure that containts the antennae
  tuner script.

  @return
  Flag indicating a success (TRUE) or failure (FALSE)
*/
boolean
rf_cdma_mc_configure_atuner_rx
(
  const rfm_device_enum_type device,
  const rfc_cdma_logical_device_params_type *logical_dev  ,
  const rfm_mode_enum_type rf_mode ,
  rf_buffer_intf *wakeup_rx_script ,
  const rfm_cdma_band_class_type band ,
  const rfm_cdma_chan_type chan ,
  ant_tuner_device_tokens_type* ant_tuner_script_token_ptr
)
{
  boolean result = TRUE ;
#ifdef FEATURE_RF_HAS_QTUNER
  rf_time_tick_type prof_ant_tuner_t ; /* Ant Tuner Time Profile */
  rf_time_type prof_ant_tuner_t_val ;  /* Ant Tuner Time Profile */
  rfcom_band_type_u band_in_common_format ;
  rfcom_antenna_tuner_priority_cfg tuner_priority_cfg;
  band_in_common_format.cdma_band = band ;

  tuner_priority_cfg.rfm_device = device;
  prof_ant_tuner_t = rf_time_get_tick() ;
  result &=
    rf_cdma_atuner_init( logical_dev->ant_tuner[band],
                         rf_mode,
                         RFDEVICE_CREATE_SCRIPT,
                         wakeup_rx_script,
                         0, /* No timing offset */
                         ant_tuner_script_token_ptr );
  result &=
    rf_cdma_atuner_configure_for_rx( tuner_priority_cfg,
                                     logical_dev->ant_tuner[band],
                                     rf_mode,
                                     band_in_common_format,
                                     chan,
                                     RFDEVICE_CREATE_SCRIPT,
                                     wakeup_rx_script,
                                     0, /* No timing offset */
                                     ant_tuner_script_token_ptr );

  prof_ant_tuner_t_val = rf_time_get_elapsed( prof_ant_tuner_t , RF_USEC ) ;
  if ( result == TRUE )
  {
    RF_MSG_1 ( RF_LOW, "rf_cdma_mc_configure_atuner_rx: time profile"
               "Ant Tuner Time %d" , prof_ant_tuner_t_val ) ;
  }
  else
  {
    RF_MSG ( RF_ERROR ,
             "rf_cdma_mc_configure_atuner_rx: API failed.." ) ;
  }

#endif /* FEATURE_RF_HAS_QTUNER */
  return result ;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure the RF device and RF Front-end to enable RX 

  @details
  -# This function calls the RF device API that creates a script containing all 
  settings needed to put the RF device to enable Rx
  -# Uses the grfc_script that has all the GRFC settings to be used for 
  enabling RX on the device
  -# Sends the above grfc script and RF device script to firmware, which then
  programs the RF front-end when triggered by RFSW via FW commands (such as
  RX_START)
  
  @param device 
  RF device that needs to be enabled for RX
 
  @param rf_mode
  used to check if the device 1x or 1xEVDO mode
 
  @param is_slave_device
  flag used to check if device is in diversity 
 
  @param rxlm_handle
  LM handle for the device
 
  @param band
  RF band to which the device is to be tuned to 
 
  @param chan
  RF band to which the device is to be tuned to
 
  @param power_mode
  The I-ceiver power mode in which the RF device is to be tuned to (high-lin or 
  low-lin)
 
  @param script_index
  script_index - to be used by FW
 
  @param rx_bw_khz
  Receive BW in KHz
 
  @param tx_bw_khz
  Tramsmit BW in KHz
 
  @param logical_dev
  Contains RFC data needed for opearation of the device
 
  @param ant_tuner_script_token_ptr
  The pointer to the data structure that containts the antennae
  tuner script.
   
  @param is_irat_mode
  Boolean flag to check if API call is in IRAT mode or not, if in IRAT mode
  we do not need to populate the Rx on/off script, because L1 will never use
  DLNA control in IRAT 
 
  @param meas_type
  mes type to pass in the script type in IRAT or non-IRAT scenarios
  In IRAT scenarios, startup or cleanup script in No-IRAT mode always invalid. 
 
  @return
  Flag indicating a success (TRUE) or failure (FALSE)
*/
boolean
rf_cdma_mc_configure_front_end_for_rx_wakeup
(
  const rfm_device_enum_type device,
  const rfm_mode_enum_type rf_mode,
  const boolean is_slave_device,
  const lm_handle_type rxlm_handle,
  const rfm_cdma_band_class_type band,
  const rfm_cdma_chan_type chan,
  const rfm_cdma_power_mode_type power_mode,
  const uint32 script_index,
  const uint32 rx_bw_khz,
  const uint32 tx_bw_khz,
  const rfc_cdma_logical_device_params_type *logical_dev,
  ant_tuner_device_tokens_type* ant_tuner_script_token_ptr,
  boolean is_irat_mode,
  const rfdevice_meas_scenario_type meas_type
)
{
  boolean result = TRUE;
  /* Pointer to script*/
  rf_buffer_intf *wakeup_rx_script = NULL;
  rf_time_tick_type prof_grfc_t;      /* GRFC Time Profile */
  rf_time_tick_type prof_device_t;    /* RF device WTR Time Profile */
  rf_time_tick_type prof_asm_t;       /* RF device ASM Time Profile*/
  rf_time_type prof_grfc_t_val;      /* GRFC Time Profile */
  rf_time_type prof_device_t_val;    /* RF device WTR Time Profile */
  rf_time_type prof_asm_t_val;       /* RF device ASM Time Profile */
  /* Get current mode 1x/HDR */
  rfm_mode_enum_type curr_mode;

  if ( (is_irat_mode == TRUE) && (meas_type == RFDEVICE_MEAS_STARTUP_SCRIPT) )
  {
    /* Tune-away common meas START UP buffer. This makes sure the previous tech's
    reset script is executed */
    wakeup_rx_script = rfmeas_mdsp_buffer_get(RFM_MEAS_PROGRAM_STARTUP);
  }
  else
  {
    /* Create generic buffer interface object for SSBI/RFFE/GRFC,
       allocate memory to max script sizes, since the memory is released when 
       object is destroyed later on anyway, APIs returning script size on the fly 
       aren't ready, all driver APIs hard coding it internally */
    wakeup_rx_script = rf_buffer_create ( 0 , /* We don't use SSBI anymore */
                                          RF_CDMA_MAX_RFFE_SCRIPT_SIZE , 
                                          RF_CDMA_MAX_GRFC_SCRIPT_SIZE ) ;
  }

  /*Check if valid pointer was returned for buffer object*/
  if( wakeup_rx_script == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_mc_configure_front_end_for_rx_wakeup: "
                      "Unable to create buffer object ");
    result = FALSE;
  }/*if( wakeup_rx_script == NULL )*/
  else
  {
#ifdef FEATURE_RF_HAS_QTUNER
    if ( logical_dev->ant_tuner[band] != NULL )
    {
      #ifdef FEATURE_RF_ASDIV
      /* Lock the ASDiv manager interface to avoid concurrency of 
         ASDiv and Tuner configuration*/
      rfcommon_asdiv_manager_enter_lock();
      #endif

      result &= rf_cdma_mc_configure_atuner_rx (device, logical_dev , rf_mode ,
                wakeup_rx_script , band ,
                chan , ant_tuner_script_token_ptr ) ;

      #ifdef FEATURE_RF_ASDIV
      /* Release the ASDiv manager interface */
      rfcommon_asdiv_manager_release_lock();
      #endif

    }
#endif /* FEATURE_RF_HAS_QTUNER */

    /* Compute RF device Programming Script for the given Band and Channel */
    prof_device_t = rf_time_get_tick();

    if ( is_slave_device == TRUE )
    {
      /* this is a diversity device: Create Analog Receiver Programming Script 
         for given Power Mode and Chan and push it to firmware */
      result &= rfdevice_cdma_create_rx_enable_diversity_script(
                          logical_dev->rx_device[band], rf_mode, band, chan,
                              rx_bw_khz, tx_bw_khz, power_mode, wakeup_rx_script );
    } /*  if( is_slave_device == TRUE ) */
    else
    {
      /* For certain WTRs, where the script is too large we split the WTR
         script into 2 parts:
         1> Preload part: A partial WTR script that sends the bulk of Rx tune
            script to WTR but does not make any band\chan\tech changes to the
            WTR operation.
         2> Trigger part: A partial WTR script, contigent on the pre-load
            script executed properly when executed will bring the new WTR
            band\chan\tech settings into effect.
        So for IRAT, we will use the split script.
           */
      if ( is_irat_mode == FALSE )
      {
        rf_cdma_data_status_type *dev_status_w;/* Dev Status Read Ptr */
        dev_status_w = rf_cdma_get_mutable_device_status( device ) ;

        /* Perform NULL Pointer check */
        if ( dev_status_w == NULL )
        {
          RF_MSG_1( RF_ERROR, "rf_cdma_mc_configure_front_end_for_rx_wakeup(): "
                    "NULL data for device %d", device );
          result = FALSE;
        }
        else
        {
          /* Populate device info for callback data */
          dev_status_w->ssma_cb_data.device = device;
      /* Populate rx script in standalone script mode */
      /* Hard code script mode to CDMA standalone script to satisfy device
      interface */
        result &= rfdevice_cdma_create_rx_tune_script( logical_dev->rx_device[band],
                    rf_mode, band, chan, rx_bw_khz, tx_bw_khz, power_mode, 
                    wakeup_rx_script, RFDEVICE_CDMA_STANDALONE_SCRIPT,
                    (rf_cdma_spur_cb_type)rf_cdma_mc_ssma_notch_callback ,
                    &(dev_status_w->ssma_cb_data) );
        }
      }
      else
      {
        /* Populate WTR IRAT scripts */
        rfdevice_meas_script_data_type script_data_ptr = {
          /* Execute the pre load part of the script immediately */
          RFDEVICE_MEAS_EXECUTE_IMMEDIATE ,
          /* NULL since we dont except any script back */
          NULL ,
          /* The trigger sctipt that FW will execute */
          RFDEVICE_MEAS_CREATE_SCRIPT ,
          /* Append the WTR trigger script */
          wakeup_rx_script ,
          /* Default WXE index */
          0
        } ;

        if ( (meas_type == RFDEVICE_MEAS_CLEANUP_SCRIPT) || 
              (meas_type ==   RFDEVICE_MEAS_STARTUP_SCRIPT))
        {
        result &=
          rfdevice_cdma_create_rx_meas_script ( logical_dev->rx_device[band],
                                                rf_mode, band, chan, rx_bw_khz, tx_bw_khz, power_mode,
                                                  meas_type , &script_data_ptr ) ;
         }
         else
         {
          /*In IRAT mode, while building script should be either startup or cleanup script*/
          RF_MSG_1( RF_ERROR, "rf_cdma_mc_configure_front_end_for_rx_wakeup(): "
                    "Invalid IRAT scripts type %d", device );
          result = FALSE;
         }
      }
    }/* !(if ( is_slave_device == TRUE ))*/

    prof_device_t_val = rf_time_get_elapsed( prof_device_t, RF_USEC );
      
    prof_grfc_t = rf_time_get_tick( );

    result &= rfc_cdma_generate_rx_wakeup_grfc_script( device, 
                                                       band, 
                                                       wakeup_rx_script,
                                                       0 /* delay*/ );

    prof_grfc_t_val = rf_time_get_elapsed( prof_grfc_t, RF_USEC );

    /* Get Wakeup Rx devices (ASM) scripts for the new band */
    prof_asm_t = rf_time_get_tick();

    result &= rf_cdma_mc_create_rx_asm_xsw_wakeup_script( device,
                                                      logical_dev,
                                                      band, 
                                                      rf_mode,
                                                      wakeup_rx_script );

    prof_asm_t_val = rf_time_get_elapsed( prof_asm_t, RF_USEC );

    if ( rf_cdma_data_get_rf_state(device,rf_mode) == RF_CDMA_STATE_RXTX &&
	 is_irat_mode == TRUE &&
	 meas_type == RFDEVICE_MEAS_CLEANUP_SCRIPT )
    {
          result &= rf_cdma_mc_create_tx_tune_back_script(device,
                                                          logical_dev,
                                                          band,
                                                          chan,
                                                          rf_mode,
                                                          ant_tuner_script_token_ptr,
                                                          wakeup_rx_script);


    }


   #ifdef FEATURE_QPOET_STANDBY_FOR_RX_SWITCH
   if (is_irat_mode == FALSE)
   {
    if ( rfc_commmon_get_rx_qpoet_config_flag() == TRUE )
    {
       if ( NULL != logical_dev->qpoet_obj[band] )
       {
         rfcom_band_type_u rf_band; 
         rf_band.cdma_band = band;

         result &= rfdevice_papm_standby(logical_dev->qpoet_obj[band] ,rf_band);

         if ( result == FALSE )
         {
           RF_MSG( RF_ERROR, "FEATURE_QPOET_STANDBY_FOR_RX_SWITCH: "
                             "QPOET standby at Rx wakeup failure");
         }
         else
         {             
           RF_MSG( RF_HIGH, "FEATURE_QPOET_STANDBY_FOR_RX_SWITCH: "
                            "RX QPOET standby at Rx wakeup success");
         }

       }
       else
       {
         RF_MSG( RF_HIGH, "FEATURE_QPOET_STANDBY_FOR_RX_SWITCH: "
                          "RX QPOET FLAG ON. NO QPOET in 1x logical dev");
       }
    }
    else
    {
      RF_MSG( RF_MED, "FEATURE_QPOET_STANDBY_FOR_RX_SWITCH: "
                      "RX QPOET FLAG OFF");
    }
   }
   else
   {
     RF_MSG( RF_MED, "IRAT Mode is in progress");
   }
    #endif

  } /* !if( wakeup_rx_script == NULL )*/

  if ( result == TRUE )
  {
    /* Get handle to the CCS event */
    rf_cdma_mdsp_event_id_type event_id = RF_CDMA_MDSP_CCS_EVENT_WAKEUP_RX_ID ;
    rf_cdma_event_type event_type = RF_CDMA_STATIC_EVENT ;
    rfcommon_mdsp_event_handle** analog_script_event_handle = NULL ;
                  
    /* If We are in IRAT mode, use IRAT Tune IN event and ID */
    if ( is_irat_mode == TRUE )
    {
      event_type = RF_CDMA_EVENT_IRAT_TUNE_IN ;
      event_id = RF_CDMA_MDSP_CCS_EVENT_RF_IRAT_TUNE_IN_ID ; 
    }

    analog_script_event_handle = 
      rf_cdma_data_get_event_handle( device, event_type );
    /* Check for NULL */
    if ( analog_script_event_handle == NULL )
    {
      RF_MSG_1 ( RF_ERROR  , "rf_1x_mc_prep_wakeup_rx: "
                 "NULL event handle returned for event:%d " ,
                 event_type ) ;
    }
    else
    {
    /* Push Analog frontend Script to MDSP */
      result &= rf_cdma_mdsp_configure_devices( device ,
                logical_dev->rx_mdsp_path,
                                              rf_mode,
                                              event_id ,
                                              analog_script_event_handle,
                                              (rf_cdma_mdsp_get_event_info(event_id)),
                                              wakeup_rx_script, 
                script_index,
                rf_cdma_data_get_rxlm_handle (device) ,
                NULL, NULL );
    }/* if! ( analog_script_event_handle == NULL ) */
  }/* if ( result == TRUE ) */

  /* Check if we are in 1x/HDR mode, if in HDR mode, populate shared memory 
  with Rx on off scripts*/ 
  if ( result == TRUE )
  {
    RF_MSG_3 ( RF_LOW, "rf_cdma_mc_configure_front_end_for_rx_wakeup: "
               "time profile GRFC_time %d, Device_time %d, ASM_time %d",
               prof_grfc_t_val, prof_device_t_val, prof_asm_t_val );

    /* Get current mode (1x/HDR) */
    curr_mode = rf_cdma_data_get_rf_mode( device );

    /* If current mode is HDR and device is NOT in IRAT state, populate shared 
       mem with Rx ON/OFF event info */ 
    if ( ( curr_mode == RFM_1XEVDO_MODE ) && ( is_irat_mode == FALSE ) )
    {
      RF_MSG_2 ( RF_LOW, "rf_cdma_mc_configure_rffe_for_rx_wakeup: "
                       "Configuring Rx on/off scripts for Device %d band:%d" , 
                 device, band  );
      result = rf_cdma_mc_configure_rx_on_off( device, band, rf_mode,
                                               script_index  );
    }/*if ( ( curr_mode == RFM_1XEVDO_MODE ) && ( is_irat_mode == FALSE ) )*/
    else
    {
      RF_MSG ( RF_LOW, "rf_cdma_mc_configure_rffe_for_rx_wakeup: "
                        "Configure Rx ON/OFF supported only for HDR mode "
                        " and in Non-IRAT scenarios" ); 
    }
  }/*if ( result == TRUE )*/

  /* Tune-away uses common startup buffer. */
  if ( (is_irat_mode == TRUE) && (meas_type == RFDEVICE_MEAS_STARTUP_SCRIPT) )
  { 
    /* Intentionally left blank. Do nothing */   
  }
  else if ( wakeup_rx_script != NULL )
  {
    /* De-allocate memory for GRFC script container */
    rf_buffer_destroy( wakeup_rx_script );
  }

  if ( result == FALSE )
  {
    RF_MSG ( RF_ERROR, "rf_cdma_mc_configure_rffe_for_rx_wakeup: Error !!" );
  }


  return result;

} /* rf_cdma_mc_configure_front_end_for_rx_wakeup */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure the RF device and RF Front-end to enable TX 

  @details
  -# This function calls the RF device API that creates a script containing all 
  settings needed to put the RF device to enable Tx
  -# Uses the grfc_script that has all the GRFC settings to be used for 
  enabling TX on the device
  -# Sends the above grfc script and RF device script to firmware, which then
  programs the RF front-end when triggered by RFSW via FW commands (such as
  TX_START)
   
  @param device
  RF device that needs to be enabled for TX
 
  @param txlm_handle
  LM handle for the device
 
  @param rf_mode
  used to check if the device 1x or 1xEVDO mode
 
  @param band
  RF band to which the device is to be tuned to 
 
  @param chan
  RF band to which the device is to be tuned to
 
  @param rx_bw_khz
  Receive BW in KHz
 
  @param tx_bw_khz
  Tramsmit BW in KHz

  @param script_index
  script_index - to be used by FW
  
  @param logical_dev
  Contains RFC data needed for opearation of the device

  @param tx_static_nv_cal_ptr
  The Tx NV data structure that max power limit information must
  be loaded into.
 
  @param ant_tuner_script_token_ptr
  The pointer to the data structure that containts the antennae
  tuner script.
 
  @return
  Flag indicating a success (TRUE) or failure (FALSE)
*/
boolean 
rf_cdma_mc_configure_front_end_for_tx_wakeup
(
  const rfm_device_enum_type device,
  const lm_handle_type txlm_handle,
  const rfm_mode_enum_type rf_mode,
  const rfm_cdma_band_class_type band,
  const rfm_cdma_chan_type chan,
  const uint32 rx_bw_khz,
  const uint32 tx_bw_khz,
  const uint32 script_index,
  const rfc_cdma_logical_device_params_type *logical_dev,
  const rf_cdma_tx_static_type *tx_static_nv_cal_ptr,
  ant_tuner_device_tokens_type* ant_tuner_script_token_ptr
)
{
  boolean result = TRUE;
  uint8 num_carriers;
  boolean is_xpt_enabled_for_hdr;
  const rf_cdma_data_status_type *dev_status_r;/* Dev Status Read Ptr*/
  rfdevice_coupler_tech_cfg_type coupler_cfg;
  /* Buffr object to hold PA scripts */
  rf_buffer_intf *wakeup_tx_script = NULL;

  dev_status_r = rf_cdma_get_device_status( device );

  if (dev_status_r == NULL)
  {
    RF_MSG( RF_ERROR, "rf_cdma_mc_configure_front_end_for_tx_wakeup: "
		              "NULL device pointer ");    
    return FALSE;
  }
  
  /* Create generic buffer interface object for SSBI/RFFE/GRFC,
     allocate memory to max script sizes, since the memory is released when 
     object is destroyed later on anyway, APIs returning script size on the fly 
     aren't ready, all driver APIs hard coding it internally */
  wakeup_tx_script = rf_buffer_create ( RF_CDMA_MAX_SSBI_SCRIPT_SIZE , 
                                        RF_CDMA_MAX_RFFE_SCRIPT_SIZE , 
                                        RF_CDMA_MAX_GRFC_SCRIPT_SIZE ) ;

  /*Check if valid pointer was returned for buffer object*/
  if( wakeup_tx_script == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_mc_configure_front_end_for_tx_wakeup: "
                      "Unable to create buffer object ");
    result = FALSE;
  }/*if( wakeup_tx_script == NULL )*/
  else
  {
    /* Populate WTR Wxe Preload and trigger scripts */
    rfdevice_meas_script_data_type script_data_ptr = {
      /* Execute the pre load part of the script immediately */
      RFDEVICE_MEAS_EXECUTE_IMMEDIATE ,
      /* NULL since we dont except any script back */
      NULL ,
      /* The trigger sctipt that FW will execute */
      RFDEVICE_MEAS_CREATE_SCRIPT ,
      /* Append the WTR trigger script */
      wakeup_tx_script ,
      /* Default WXE index */
      0
    };
    /*-----------------------------------------------------------------------*/
    /* compute RF device script for TX enable and push it to firmware */
    num_carriers = dev_status_r->num_carriers;
    is_xpt_enabled_for_hdr = rf_cdma_xpt_is_enabled_for_hdr(num_carriers);
	
    result &= 
      rfdevice_cdma_create_tx_tune_script( logical_dev->tx_device[band],
                                           rf_mode, 
                                           band, 
                                           chan, 
                                           tx_bw_khz, 
                                           (rfdevice_dpd_enum_type)(rf_cdma_xpt_is_enabled( device )&& 
                                           is_xpt_enabled_for_hdr),
                                           &script_data_ptr,
					   FALSE );

    /* Compute GRFC Programming Script for the given Band and Channel */
    result &= rfc_cdma_generate_tx_wakeup_grfc_script( device, 
                                                       band, 
                                                       wakeup_tx_script,
                                                       0 /*delay*/ );

    /* Get Wake up Tx devices (PA, ASM & QPOET ) scripts */
    result &= rf_cdma_mc_create_tx_asm_pa_qpoet_wakeup_script( device,
                                                           logical_dev,
                                                           band, 
                                                           rf_mode,
                                                           wakeup_tx_script );
    if ( logical_dev->ant_tuner[band] != NULL )
    {
      rfcom_band_type_u band_in_common_format;

      band_in_common_format.cdma_band = band;

#ifdef FEATURE_RF_ASDIV
      /* Lock the ASDiv manager interface to avoid concurrency of ASDiv
         and Tuner configuration*/
      rfcommon_asdiv_manager_enter_lock();
#endif

      result &=
        rf_cdma_atuner_configure_for_tx( txlm_handle ,
                                        logical_dev->ant_tuner[band],
                                        rf_mode,
                                        band_in_common_format,
                                        chan,
                                        RFDEVICE_CREATE_SCRIPT,
                                        wakeup_tx_script,
                                        0, /* No timing offset */
                                        ant_tuner_script_token_ptr,
                                        (void*)&(tx_static_nv_cal_ptr->ant_tuner_cl) );
#ifdef FEATURE_RF_ASDIV
      /* Releae the ASDIV manager interface */
      rfcommon_asdiv_manager_release_lock();
#endif

    }
  }/*if ! ( wakeup_tx_script == NULL )*/


  if ( NULL != logical_dev->coupler_obj[band] )
  { /* Get Wake up coupler scripts */
    coupler_cfg.mode = rf_mode;
    coupler_cfg.band.cdma_band = band;
    /* Hard code coupler direction , this is known in the coupler driver*/
    coupler_cfg.direction = RFDEVICE_COUPLER_DIRECTION_FWD;
    /* Query for coupler port based on XSW position */
    coupler_cfg.port = rf_cdma_mc_get_coupler_port( device );
    result &= rfdevice_coupler_config( logical_dev->coupler_obj[band],
                                       coupler_cfg,
                                       wakeup_tx_script,
                                       RFDEVICE_CREATE_SCRIPT,
                                       0);
  }/*if( NULL != logical_dev->coupler_obj[band] )*/

  if ( result == TRUE )
  {
    /* Get handle to the CCS event */
    rf_cdma_mdsp_event_id_type event_id = RF_CDMA_MDSP_CCS_EVENT_WAKEUP_TX_ID ;
    rf_cdma_event_type event_type = RF_CDMA_STATIC_EVENT ;
    rfcommon_mdsp_event_handle** analog_script_event_handle = NULL ;

    analog_script_event_handle =
      rf_cdma_data_get_event_handle( device, event_type );

    /* Check for NULL */
    if ( analog_script_event_handle == NULL )
    {
      RF_MSG_1 ( RF_ERROR  , "rf_cdma_mc_configure_front_end_for_tx_wakeup: "
                 "NULL event handle returned for event:%d " ,
                 event_type ) ;
    }
    else
    {
    /* Push Analog frontend Script to MDSP */
      result &= rf_cdma_mdsp_configure_devices( device ,
                logical_dev->tx_mdsp_path,
                                              rf_mode,
                event_id ,
                                              analog_script_event_handle,
                (rf_cdma_mdsp_get_event_info(event_id)),
                                              wakeup_tx_script, 
                script_index,
                rf_cdma_data_get_txlm_handle (device) ,
                NULL, NULL );
    }/* if! ( analog_script_event_handle == NULL ) */
  } /* if ( result == TRUE )  */

  if ( result == FALSE )
  {
    RF_MSG (RF_ERROR, "rf_cdma_mc_configure_front_end_for_tx_wakeup: Error !!" );
  }

  if ( wakeup_tx_script != NULL )
  {
    /* De-allocate memory for script container */
    rf_buffer_destroy( wakeup_tx_script );
  }

  return result;
} /* rf_cdma_mc_configure_front_end_for_tx_wakeup */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  API that queries for WTR Rx sleep script for a given band. This API is
  specifically call under retune scenario outside of configure_rffe helper API.
  Because the API call may trigger a clear notch action to FW so it needs to be 
  called before Rx_Stop is sent. 

  @param rx_device
  WTR Device Ptr
 
  @param rx_bw_khz
  Receive BW in KHz
 
  @param tx_bw_khz
  Tramsmit BW in KHz
 
  @param rx_tx_settings
  RF script buffer
 
  @return boolean
  API status

 */
boolean
rf_cdma_mc_create_rx_tranceiver_sleep_script
(   
   rfdevice_trx_cdma_rx* rx_device,
   const uint32 rx_bw_khz,
   const uint32 tx_bw_khz,  
   rf_buffer_intf* rx_tx_settings 
)
{
  boolean api_status = TRUE;

  if ( rx_device == NULL || rx_tx_settings == NULL )
  {
    RF_MSG (RF_ERROR, "rf_cdma_mc_create_rx_tranceiver_sleep_script: "
                      "NULL ptr" );
    api_status = FALSE;
  }

  if ( api_status == TRUE )
  {
    rfdevice_cdma_rx_enter_critical_section( rx_device ); 

    api_status = rfdevice_cdma_create_rx_sleep_script( rx_device, 
                                                       rx_tx_settings, 
                                                       rx_bw_khz, tx_bw_khz );

    rfdevice_cdma_rx_leave_critical_section( rx_device );
  }

  return api_status;
}/* rf_cdma_mc_create_rx_tranceiver_sleep_script */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Static API which gets a pointer to the DM location based on
  the Tx AOL event type for given Tx device.

  @param device
  Device which needs to be configured for Rx ON/OFF

  @return DM*
  A uint32 Pointer to the DM buffer associated with Tx AOL
  event.

 */
uint32*
rf_cdma_mc_get_aol_tx_dm_buffer
(
  rfm_device_enum_type device
)
{
  return ( rf_cdma_mdsp_get_tx_aol_buffers (
             rf_cdma_data_get_txlm_handle ( device ) ) ) ;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief 
  Get Rx sampling rate of IQ capture for given RxLM. 
  
  @param device 
  RxLM Buffer Index 
   
  @return
  Sampling rate in Hz
*/

uint32
rf_cdma_mc_get_samp_rate
(
  uint8 rx_lm_buf_index
)
{
  return(rf_cdma_msm_get_samp_rate(rx_lm_buf_index));
}
/* rf_cdma_mc_get_samp_rate */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Clean up the Rx AGC LNA SM event handles before the next configuration
 
  @param device
  Device for which Rx AGC LNA SM events must be cleaned up
  
  @return
  TRUE if cleanup was success, FALSE in failure case
*/
boolean
rf_cdma_mc_cleanup_rx_agc_lna_ccs_events
(
  rfm_device_enum_type device
)
{
  boolean ret_val = TRUE;

  /* Clean up the LNA SM dynamic event handles */
  ret_val &= rf_cdma_mc_cleanup_ccs_event( device, RF_CDMA_EVENT_LNA_G0_SM );
  ret_val &= rf_cdma_mc_cleanup_ccs_event( device, RF_CDMA_EVENT_LNA_G1_SM );
  ret_val &= rf_cdma_mc_cleanup_ccs_event( device, RF_CDMA_EVENT_LNA_G2_SM );
  ret_val &= rf_cdma_mc_cleanup_ccs_event( device, RF_CDMA_EVENT_LNA_G3_SM );
  ret_val &= rf_cdma_mc_cleanup_ccs_event( device, RF_CDMA_EVENT_LNA_G4_SM );

  return ret_val;
}/* rf_cdma_mc_cleanup_rx_agc_lna_ccs_events */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  CDMA MC layer callback function for Tuner CL event. To be registered to Tuner 
  Manager module. 
 
  @param cb_data
  Call back data which is registered with the callback. 
  Must be of type rf_cdma_mc_tuner_cl_cb_data*
  
  @return
  void
*/
void
rf_cdma_mc_tuner_close_loop_meas_cb
(
   void* cb_data
)
{
    return;
}

#ifdef FEATURE_RF_ASDIV
/*----------------------------------------------------------------------------*/
/*!
  @brief
  CDMA MC layer API to handle set antenna request from RFM Common AsDiv Module. 
  The API queries Tuner object, creates tuner script buffer, 
  let common AsDiv module fill it, register AsDiv completion callback 
  and let it send the buffer to MDSP. 
 
  @param device 
  RFM device to perform switching on. 
   
  @param position 
  Antenna position to switch to. 
   
  @param immediate 
  Immediate write or script based 
   
  @param cb_func 
  Callback function which will be executed at the end of API 
   
  @param cb_data 
  Callback data input for the callback function. 
  
  @return
  boolean TRUE if success. FALSE if fail
*/
boolean
rf_cdma_mc_set_antenna_to_position
(
  const rfm_device_enum_type device,
  const uint8 position,
  const boolean immediate,
  void* cb_func,
  void* cb_data
)
{
  boolean api_status = TRUE;

  const rf_cdma_data_status_type *dev_r = NULL;
  const rfc_cdma_logical_device_params_type *logical_dev = NULL;
  rf_cdma_atuner* cdma_atuner_ptr = NULL;
  rf_buffer_intf* asdiv_script;

  rfcommon_asdiv_asm_conifg_type get_asm_script_cb;
  rfcommon_asdiv_send_script_type send_script_cb;
  rfcommon_asdiv_switch_start_notify_type switch_start_cb;
  rfcommon_asdiv_update_tuner_type update_tuner_cb;

  rf_cdma_asdiv_xsw_script_data xsw_script_data;

  /* Get Device Status */
  dev_r = rf_cdma_get_device_status( device );
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* NULL Pointer check */
  if ( dev_r == NULL || logical_dev == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_mc_set_antenna_to_position: Dev %d - NULL "
                        "device or RFC data", device );
    return FALSE;
  } /* if ( dev_r == NULL ) */

  /* Check for RF mode */
  if( dev_r->rf_mode != RFM_1X_MODE && dev_r->rf_mode != RFM_1XEVDO_MODE )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_mc_set_antenna_to_position: invalid mode %d", 
                        dev_r->rf_mode );
    return FALSE; 
  }
  if ( (rfm_l1_cb_type)cb_func == NULL)
  {
    /* Get Current switch position */
    if ( rfcommon_asdiv_get_current_position(device) == 
       position )
    { 
      RF_MSG_2( RF_HIGH, "rf_cdma_mc_set_antenna_to_position: "
                       "Current position %d is same as the position requested %d", 
                       rfcommon_asdiv_get_current_position(device) , 
                       position );
      return TRUE;
    }
  }
  
  /* Check for CDMA RF state that the device is in. */
  if( ( dev_r->data_1x.rf_state == RF_CDMA_STATE_RX )
           ||
           ( dev_r->data_1x.rf_state == RF_CDMA_STATE_RXDIV )
           ||
           ( dev_r->data_hdr.rf_state == RF_CDMA_STATE_RX )
           ||
           ( dev_r->data_hdr.rf_state == RF_CDMA_STATE_RXDIV ) )
  {
  }
  else if( ( dev_r->data_1x.rf_state == RF_CDMA_STATE_RXTX )
           ||
           ( dev_r->data_hdr.rf_state == RF_CDMA_STATE_RXTX ) )
  {
  }
  else
  {
    /* If radio is not active but device belongs to 1x/HDR, 
    update internal antenna position state so that upon next wakeup, switch 
    is configured */
    rfcommon_asdiv_init_switch_state(position);

    RF_MSG_2( RF_HIGH, "rf_cdma_mc_set_antenna_to_position: "
                       "Dev %d - set switch poisition %d", 
                        device, position );
    return TRUE;
  }

  xsw_script_data.device = device;
  xsw_script_data.band = dev_r->curr_band;
  xsw_script_data.immediate = immediate;

  get_asm_script_cb.cb_func = rfc_cdma_generate_asdiv_xsw_script;
  get_asm_script_cb.cb_data = &xsw_script_data;
  send_script_cb.cb_func = rfcommon_asdiv_manager_send_script;
  send_script_cb.cb_data = NULL;
  switch_start_cb.cb_func = NULL;
  switch_start_cb.cb_data = NULL;
  update_tuner_cb.cb_func = NULL;
  update_tuner_cb.cb_data = (uint8)device;

  if ( dev_r->rf_mode == RFM_1X_MODE )
  {
    update_tuner_cb.cb_func = (rfcommon_asdiv_update_tuner_cb_type)
                              rfm_1x_antenna_tuner_action_time;
  }
  else if ( dev_r->rf_mode == RFM_1XEVDO_MODE )
  {
    update_tuner_cb.cb_func = (rfcommon_asdiv_update_tuner_cb_type)
                              rfm_hdr_antenna_tuner_action_time;
  }
  else
  {
    RF_MSG_2( RF_ERROR, "rf_cdma_mc_set_antenna_to_position: "
              "Current RF State %d of Dev %d is not "
              "the Expected state - CDMA or 1x EVDO state",
              device, dev_r->rf_mode );
  }

  /*1. Create buffer */
  asdiv_script = rf_buffer_create ( RF_CDMA_MAX_SSBI_SCRIPT_SIZE, 
                                    RF_CDMA_MAX_RFFE_SCRIPT_SIZE, 
                                    RF_CDMA_MAX_GRFC_SCRIPT_SIZE ) ;   

  if( asdiv_script != NULL )
  {
    cdma_atuner_ptr = logical_dev->ant_tuner[dev_r->curr_band];

    api_status &= rf_cdma_atuner_asdiv_config_switch( cdma_atuner_ptr,
                                                      device,
                                                      dev_r->rf_mode,
                                                      position, 
                                                      asdiv_script, 
                                                      get_asm_script_cb,
                                                      send_script_cb,
                                                      switch_start_cb,
                  update_tuner_cb,
                                                      (rfm_l1_cb_type)cb_func, 
                                                      cb_data, 
                                                      FALSE );
  }

  if( asdiv_script != NULL )
  {
    rf_buffer_destroy( asdiv_script );
  }

  if( api_status == FALSE )
  {
    RF_MSG_2( RF_ERROR, "rf_cdma_mc_set_antenna_to_position: "
                        "Failure for device %d position %d", 
                        device, position  );
  }

  return api_status;

}/* rf_cdma_mc_set_antenna_to_position */


/*----------------------------------------------------------------------------*/
/*!
  @brief 
  CDMA MC layer API to handle set antenna request from RFM Common AsDiv Module. 
  The API queries Tuner object, and let common AsDiv driver abort. 
 
  @param device 
  RFM device to perform switching on. 
   
  @param position 
  Antenna position to switch to
   
  @param cb_func 
  Callback function which will be executed at the end of API 
   
  @param cb_data 
  Callback data input for the callback function. 
  
  @return
  void
*/
boolean
rf_cdma_mc_abort_set_antenna_to_position
(
  const rfm_device_enum_type device,
  const uint8 position,
  void* cb_func,
  void* cb_data
)
{
  boolean api_status = TRUE;

  const rf_cdma_data_status_type *dev_r = NULL;
  const rfc_cdma_logical_device_params_type *logical_dev = NULL;
  void* asdiv_atuner_ptr = NULL;

  /* Get Device Status */
  dev_r = rf_cdma_get_device_status( device );
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* NULL Pointer check */
  if ( dev_r == NULL || logical_dev == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_mc_abort_set_antenna_to_position: Dev %d - NULL "
                        "device or RFC data", device );
    return FALSE;
  } /* if ( dev_r == NULL ) */

  /* Check for RF mode */
  if( dev_r->rf_mode != RFM_1X_MODE && dev_r->rf_mode != RFM_1XEVDO_MODE )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_mc_abort_set_antenna_to_position: invalid mode %d", 
                        dev_r->rf_mode );
    return FALSE; 
  }

  asdiv_atuner_ptr = logical_dev->ant_tuner[dev_r->curr_band];

  if( asdiv_atuner_ptr != NULL )
  {
    api_status &= rfcommon_asdiv_manager_abort_switch( device, dev_r->rf_mode, asdiv_atuner_ptr );
  }
  else
  {
    RF_MSG_2( RF_ERROR, "rf_cdma_mc_abort_set_antenna_to_position: "
                        "NULL tuner pointer for device %d position %d", 
                        device, position  );
    api_status = FALSE;
  }

  if( api_status == FALSE )
  {
    RF_MSG_2( RF_ERROR, "rf_cdma_mc_abort_set_antenna_to_position: "
                        "Failure for device %d position %d", 
                        device, position  );
  }

  return api_status;
}/* rf_cdma_mc_abort_set_antenna_to_position */
#endif /* FEATURE_RF_ASDIV */

/*----------------------------------------------------------------------------*/
/*!
  @brief 
  This API sets all the state variables for the given device to
  be consistent with the sleep state.
 
  @details
  This API sets all the state variables for the given device to
  be consistent with the sleep state.

  When the device is in sleep state, all resources (such as LM
  handles, paired devices, etc) are relinquished. All
  freq-related info (like band, chan, IC state, etc) are
  reverted to initial/invalid state.".
 
  @param device 
  RFM device to perform reset on SW state variables. 
  
  @return
  void
*/
void
rf_cdma_reset_sw_state
(
   const rfm_device_enum_type device
)
{
  if ( device < RFCOM_MAX_DEVICES )
  {
    /*! @todo RFSB - All var must not be blindly reset, they must account for both techs. */

    /* Update CDMA Data structure with new RF State and  band/Chan */
    rf_cdma_init_freq_info_for_device(device) ;
    rf_cdma_data_set_assoc_dev( device, RFM_INVALID_DEVICE ) ;

    /* Update CDMA Data structure with new 1x RF State */
    rf_cdma_data_set_1x_rf_state( device, RF_CDMA_STATE_SLEEP ) ;

    /* Update CDMA Data structure with new hdr RF State */
    rf_cdma_data_set_hdr_rf_state( device, RF_CDMA_STATE_SLEEP ) ;

    /* invalidate RXLM */
    rf_cdma_data_set_rxlm_handle( device , RFM_INVALID_LM_BUFFER ) ;

  }
  else
  {
    RF_MSG_1 ( RF_ERROR , "rf_cdma_reset_sw_state(): "
               "Bad Device --> %d" , device ) ;
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This API will do an immediate write for the requested LNA state

  @details
  This API queries WTR driver for LNA register write information for the given
  LNA state and does an immediate write via CCS to the register. This API might
  be used In case RF decides to use immediate writes for LNA updates
  as opposed to scheduling issue seqs on the RFLM side, this API will be called.
  Correspondingly on the RFLM side, lna updates will be disabled.

  @param device
  RFM device for which LNA state change is requested

  @param gain_state
  Requested LNA gain state

  @return
  TRUE: Success/ FALSE: failure
*/
boolean
rf_cdma_mc_immediate_lna_write
(
  rfm_device_enum_type device,
  rfm_lna_gain_state_type gain_state
)
{
  boolean ret_val = FALSE;
  const rfc_cdma_logical_device_params_type *logical_dev = NULL;
  rfm_lna_gain_state_type gain_state_readback;
  logical_dev = rfc_cdma_get_logical_device_params(device);

  if ( logical_dev!= NULL )
  {
    /* Device Status Read Pointer - to get current band */
    const rf_cdma_data_status_type
    *dev_status_r = rf_cdma_get_device_status( device );

    /* Perform NULL Pointer check */
    if ( dev_status_r == NULL )
    {
      RF_MSG_1( RF_ERROR, "rf_cdma_mc_immediate_lna_write: "
                "NULL data for device %d", device );
    }
    else
    {
      /* Buffer to store LNA settings */
      rf_buffer_intf* lna_settings = NULL ;
      /* Create generic buffer interface object */
      lna_settings = rf_buffer_create ( CFW_RF_EVENT_MAX_NUM_OF_SBI  ,
                                        CFW_RF_EVENT_MAX_NUM_OF_RFFE ,
                                        0 );
      /* Check if valid buffer was returned*/
      if ( lna_settings != NULL )
      {
        /* Call device interface to obtain the device specific LNA SM settings */
        ret_val = rfdevice_cdma_create_rx_lna_gain_script(
                    logical_dev->rx_device[dev_status_r->curr_band],
                    dev_status_r->curr_band,
                    RFM_CDMA_POWER_MODE_NORMAL,/* power mode */
                    dev_status_r->rx_bw_khz,/* BW in KHz */
                    gain_state,
                    lna_settings );

        rf_cdma_mc_do_immediate_script_write( lna_settings );

        rf_buffer_destroy( lna_settings );

        if ( ret_val == TRUE )
        {
          /* Read back lna state to check if the write went through */
          rfdevice_cdma_rx_cmd_dispatch(
            logical_dev->rx_device[dev_status_r->curr_band],
            RFDEVICE_GET_LNA_STATE, &gain_state_readback);

          if ( gain_state_readback != gain_state )
          {
            RF_MSG_2( RF_ERROR,"rf_cdma_mc_immediate_lna_write -lna state "
                      "readback: %d | Expected LNA state: %d" ,
                      gain_state_readback, gain_state );

          }
          else
          {
            RF_MSG_2( RF_MED,"rf_cdma_mc_immediate_lna_write -lna state "
                      "readback:%d | Expected LNA state: %d",
                      gain_state_readback , gain_state);

          }
        }/* if ( ret_val == TRUE ) */
      }/* if ( lna_settings != NULL ) */
    }/*if ( dev_status_r == NULL )*/
  }/* if ( logical_dev!= NULL ) */
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_mc_immediate_lna_write: Null pointer for "
              "logical device (%d)", device );
  }

  return ret_val;

} /* rf_cdma_mc_immediate_lna_write */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API to return BTF delays

  @details
  This API is called to query for the total delays on the RF side. it queries
  the MSM layer for Tx digital front end delay, DAC delay, Rx digital front end
  delay and the ADC delay. The Analog fron end delay is the part of the NV.

  1) Analog Rx delay term (antenna through ADC input)
  2) Digital RxFE delay term (ADC output to mempool I/Q)
  3) Digital TxFE delay term (modulator input to DAC input)
  4) Analog Tx delay term (DAC output through antenna)

  The API converts all the delays into cx8 and adds them up and returns to L1
  In case of failure , the API will return negative one.

  @param curr_mode
  Current mode 1x of HDR

  @param device
  The device for which delays are queried

  @param dev_status_r
  Read only pointer to device status data structure

  @param logical_dev
  Read only pointer to logical device data structure

  @param btf_cx8
  Pointer to the btf value

  @return
  BTF value in cx8 units, negative one on failure

*/
boolean
rf_cdma_mc_get_btf_delay
(
  rfm_mode_enum_type curr_mode,
  rfm_device_enum_type device,
  const rf_cdma_data_status_type *dev_status_r,
  const rfc_cdma_logical_device_params_type *logical_dev,
  int16 *btf_cx8
)
{
  boolean ret_val;
  int32 analog_front_end_delay;
  int32 txfe_delay;/* The delay on Tx digital front end and DAC */
  /* The delay on Rx digital front end and ADC, this can be per carrier */
  int32 rxfe_delay[RFM_CDMA_CARRIER_NUM];

  /* Check for NULL pointers*/
  if ( ( dev_status_r == NULL ) || ( logical_dev == NULL ) )
  {
    RF_MSG_2( RF_ERROR, "rf_cdma_mc_get_btf_delay: NUll logical dev data "
              "or dev status ptr for device :%d | mode :%d" ,
              device, curr_mode );
    return FALSE;
  }

  /* BTF includes (all in cx8 units)
     1) Rx and Tx Ananlog front-end delays -- obtained from NV
     2) ADC and Digital RXFE delay -- from RxLM
     3) Digital TXFE and DAC delay -- from TxLM
  */
  /* Query Modem layer for Digital TXFE and DAC delay*/
  ret_val = rf_cdma_msm_get_txfe_delay( dev_status_r->txlm_handle,
                                        &txfe_delay );

  /* Query modem layer for ADC and Digital RXFE delay */
  ret_val &= rf_cdma_msm_get_rxfe_delay( dev_status_r->rxlm_handle,
                                         rxfe_delay );

  /* Check if calls to MSM layer were succesful */
  if ( ret_val == TRUE )
  {
    /* Based on the mode, query the appropriate NV for analog
       front end delay*/
    if ( curr_mode == RFM_1X_MODE )
    {
      const rf_1x_config_type *rf_1x_config_ptr;/* Pointer to 1x RF NV data */

      /* ensure that NV is valid for given device and band */
      rf_1x_config_ptr = rf_cdma_nv_get_1x_data( 
                               logical_dev->tx_nv_path[dev_status_r->curr_band],
                               dev_status_r->curr_band);

      /* NULL Pointer check */
      if ( rf_1x_config_ptr == NULL )
      {
        RF_MSG_3( RF_ERROR, "rf_cdma_mc_get_btf_delay: Invalid NV pointer on  "
                  "Dev %d, NV Path %d, Band %d", device,
                  logical_dev->tx_nv_path[dev_status_r->curr_band],
                  dev_status_r->curr_band );
        ret_val = FALSE;
      }
      else
      {
        /* Update analog front end value with the NV if the config pointer is
        not NULL */
        analog_front_end_delay = rf_1x_config_ptr->enc_btf_delay;
      }
    }/* if ( curr_mode == RFM_1X_MODE ) */
    else if ( curr_mode == RFM_1XEVDO_MODE )
    {
      /* Pointer to HDR RF NV data */
      const rf_hdr_config_type *rf_hdr_config_ptr;

      /* ensure that NV is valid for given device and band */
      rf_hdr_config_ptr = rf_cdma_nv_get_hdr_data( 
                          logical_dev->tx_nv_path[dev_status_r->curr_band],
                          dev_status_r->curr_band);

      /* NULL Pointer check */
      if ( rf_hdr_config_ptr == NULL )
      {
        RF_MSG_3( RF_ERROR, "rf_cdma_mc_get_btf_delay: Invalid NV pointer on  "
                  "Dev %d, NV Path %d, Band %d", device,
                  logical_dev->tx_nv_path[dev_status_r->curr_band],
                  dev_status_r->curr_band );
        ret_val = FALSE;
      }
      /* Zero, since HDR currently does not capture this NV item*/
      analog_front_end_delay = 0;
    }/* else if ( curr_mode == RFM_1XEVDO_MODE ) */
    else
    {
      RF_MSG_2( RF_ERROR, "rf_cdma_mc_get_btf_delay: Invalid mode:%d for "
                " Dev %d" , curr_mode, device );
      ret_val = FALSE;
    }

    /* Add up all the delays based on number of carriers */
    if ( ret_val == TRUE )
    {
      uint8 ctr;
      /* Initialize btf value to zero */
      *btf_cx8 = 0;
      *btf_cx8 = analog_front_end_delay + txfe_delay;
      for ( ctr=0 ; ctr < dev_status_r->num_carriers ; ctr++ )
      {
        *btf_cx8 = *btf_cx8+rxfe_delay[ctr];
      }
    }
  }/* if ( msm_api_status == TRUE ) */
  else
  {
    RF_MSG_2( RF_ERROR, "rf_cdma_mc_get_btf_delay: MSM API call failure for"
              " mode:%d for Dev %d" ,
              curr_mode, device );
    ret_val = FALSE;
  }

  return ret_val;
}/* rf_cdma_mc_get_btf_delay*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API used to trigger dynamic notch update for SSMA
 
  @details
  This API can be called to trigger an immediate dynamic notch update
  (when the Rx chain is already enabled) or in the future after Rx has been 
  enabled completely .
  The API applies notches based on the recommendations from the WTR driver
 
  @param curr_mode
  Current mode 1x of HDR

  @param device
  The device for which delays are queried

  @param ssma_reco
  Recommendations from the WTR driver

  @param num_notch_filters_used
  Number of notch filters used up

  @param clear_notch
  Flag to indicate whether then applied notches need to be cleared (when 
  exiting)

  @return
  TRUE on success/FALSE on failure

*/
boolean
rf_cdma_mc_trigger_dynamic_notch_update
(
  rfm_device_enum_type device,
  rfdevice_cdma_rx_reco_type ssma_reco,
  uint8 num_notch_filters_used,
  boolean clear_notch
)
{
  boolean api_status = TRUE;
  rf_cdma_notch_flt_settings_type notch_filt_settings;
  /* We have notch filters available for this chain !*/
  uint8 carr_idx;

  rf_cdma_data_status_type *dev_status_w; /* Device Status Write Pointer */ 
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */ 

  /*------------------------------------------------------------------------*/
  /* Get pointers to device state */
  /*------------------------------------------------------------------------*/
  logical_dev = rfc_cdma_get_logical_device_params( device );
  
  /* Get Writable Device Status */
  dev_status_w = rf_cdma_get_mutable_device_status( device );

  if ( ( logical_dev == NULL ) || ( dev_status_w == NULL ) )
  {
    /* unable to get a valid device pointer - bail out */
    RF_MSG_1( RF_ERROR, "rf_cdma_mc_trigger_dynamic_notch_update: unable to "
                        "get a valid device pointer for device %d ", device );
    return FALSE;
  }

  /*------------------------------------------------------------------------*/
  /* Checks for device state */
  /*------------------------------------------------------------------------*/
  if ( dev_status_w->rf_mode == RFM_1XEVDO_MODE )
  {
    if ( dev_status_w->data_hdr.rf_state == RF_CDMA_STATE_SLEEP )
    {
      /* Device is in sleep, bail out */
      RF_MSG_2( RF_ERROR, "rf_cdma_mc_trigger_dynamic_notch_update: device :%d "
                          "for RF mode:%d is in sleep state, cannot "
                          "apply notches! ", device, 
                          dev_status_w->rf_mode );

      api_status = FALSE;
    }
  }/* if ( dev_status_w->rf_mode == RFM_1XEVDO_MODE ) */
  else if ( dev_status_w->rf_mode == RFM_1X_MODE )
  {
    if ( dev_status_w->data_1x.rf_state == RF_CDMA_STATE_SLEEP )
    {
      /* Device is in sleep, bail out */
      RF_MSG_2( RF_ERROR, "rf_cdma_mc_trigger_dynamic_notch_update: device :%d "
                          "for RF mode:%d is in sleep state, cannot "
                          "apply notches! ", device, 
                          dev_status_w->rf_mode );

      api_status = FALSE;
    }
  }/* else if ( dev_status_w->rf_mode == RFM_1X_MODE ) */
  else
  {
    /* Device is in sleep, bail out */
    RF_MSG_2( RF_ERROR, "rf_cdma_mc_trigger_dynamic_notch_update: device :%d is "
                        " in unsupported RF mode:%d  ", device, 
                        dev_status_w->rf_mode );
     api_status = FALSE;
  }

  /*------------------------------------------------------------------------*/
  /* Check if it is safe to proceed */
  if ( api_status == TRUE )
  {
    if ( clear_notch == FALSE )
    {
      /* Check if there are any recos from WTR*/
      if ( ssma_reco.bb.has_reco == TRUE )
      {
        /* Check if there are notch filters available for use */
        if ( num_notch_filters_used >= RF_MAX_NUM_NOTCH_FILTERS_AVAIL )
        {
          RF_MSG_3( RF_HIGH, "rf_cdma_mc_trigger_dynamic_notch_update:All "
                             "notches currently in use for device:%d ,mode:%d"
                             "Max allowed notches per WB are :%d", device, 
                            dev_status_w->rf_mode,
                            RF_MAX_NUM_NOTCH_FILTERS_AVAIL);
          
          api_status = TRUE;
        }/* if (num_notch_filters_used >= RF_MAX_NUM_..... )*/
        else
        {
          /* Acquire Handle lock before writing any settings */
          api_status &= 
                ( rxlm_acquire_lock(dev_status_w->rxlm_handle) == LM_SUCCESS );
          /* Apply notches required for each carrier */
          for( carr_idx = 0; carr_idx < (int)dev_status_w->num_carriers; 
               carr_idx++ ) 
          {
            /* Start with all zeros */
            memset(&notch_filt_settings,0,sizeof(rf_cdma_notch_flt_settings_type));

            /* Convert notch recommendation from WTR into register settings */
            api_status = rf_cdma_msm_conv_recos_to_reg_vals( 
                    dev_status_w->rxlm_handle,
                    dev_status_w->curr_band, 
                    dev_status_w->curr_chans[carr_idx],
                    dev_status_w->rf_rx_reco.rf.lo_chan,
                    dev_status_w->num_carriers,
                    ssma_reco.bb.per_carrier_notch_reco[carr_idx],
                    &(notch_filt_settings));

            /* update notch config for the device */
            api_status &= rf_cdma_msm_update_notch_settings(
                                                 dev_status_w->rxlm_handle,
                                                 &notch_filt_settings );

            /* Based on the RF mode, call the appropriate FW to trigger the notch
            filter update */
            if ( dev_status_w->rf_mode == RFM_1XEVDO_MODE )
            {
              api_status &= rf_hdr_mdsp_trigger_dyn_notch_update( 
                                                 logical_dev->rx_mdsp_path,
                                                 dev_status_w->rxlm_handle,
                                                 NULL, NULL,NULL ); 
            }
            else 
            {
              api_status &= rf_1x_mdsp_trigger_dyn_notch_update( 
                                                 logical_dev->rx_mdsp_path,
                                                 dev_status_w->rxlm_handle,
                                                 NULL, NULL,NULL ); 
            }
            /* Update the number of notches used */
            dev_status_w->num_notches_used+= 
                                   notch_filt_settings.num_notch_filters_used;

          }/* for( carr_idx = 0; carr_idx < (int)dev_status->num_carriers.....*/
          api_status &=
                ( rxlm_release_lock(dev_status_w->rxlm_handle) == LM_SUCCESS );
        }/* if! ( num_notch_filters_used >= RF_MAX_NUM_NOTCH_FILTERS_AVAIL ) */
      }/* if ( ssma_reco.bb.has_reco == TRUE ) */
    }
    else
    {
      /* Start with all zeros */
      memset( &notch_filt_settings, 0, sizeof(rf_cdma_notch_flt_settings_type));

      /* Based on the RF mode, call the appropriate FW to trigger the notch
      filter update */
      if ( dev_status_w->rf_mode == RFM_1XEVDO_MODE )
      {
        api_status = rf_hdr_mdsp_trigger_dyn_notch_update( 
                                          logical_dev->rx_mdsp_path,
                                          dev_status_w->rxlm_handle,
                                          NULL, NULL,NULL ); 
      }
      else 
      {
        api_status = rf_1x_mdsp_trigger_dyn_notch_update( 
                                          logical_dev->rx_mdsp_path,
                                          dev_status_w->rxlm_handle,
                                          NULL, NULL,NULL ); 
      }
    }
  }/* if ( api_status == TRUE )*/
  else
  {
    /* API failure! */
    RF_MSG( RF_ERROR, "rf_cdma_mc_trigger_dynamic_notch_update: API failure" );
  }

  return api_status;
} /* rf_cdma_mc_trigger_dynamic_notch_update */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API used to do dynamic txr iq dac group settings
 
  @details
  Tx spectral inverstion feature enabled for MB\HB, in-order to inform the 
  MSM about this, we need to update the txr iq dac group settings otherwise our tx
  spectrum will be inverted and the data will not be decoded properly by BS.
  
  Example Sceario: If we make call on LB (BC0) and retune to HB\MB (BC1 or BC6)
  then we need to inform the MSM about the spectral inverstion feature for the new bands

  The last bit of the TXR_IQ_DAC group is used to inform the MSM
 
  @param device
  The device for which dynamic update needed

  @param band
  The new band to which the device will do Hand off

  @return
  TRUE on success/FALSE on failure

*/


boolean
rf_cdma_mc_trigger_dyn_txr_iq_dac_update
(
  rfm_device_enum_type device,
  const rfm_cdma_band_class_type band

)
{

  /* structure that wil ahve the cdma logical device details */
  const rfc_cdma_logical_device_params_type *logical_dev;
  
  /* Structure that will ahve  the TxLM dynamic settings*/
  rf_cdma_msm_dynamic_txlm_cfg_type dynamic_cfg;

  /* Structure that will have the current device status*/
  const rf_cdma_data_status_type *dev_status; 

  /* structure that will have the tx_device details */
  rfdevice_trx_cdma_tx* tx_device = NULL ;

  /* Variable that will have the txlm_handle for the current device*/
  lm_handle_type txlm_handle;

  /* Variable that will inform the status of the TxR IQ DAC dynamic settings update*/
  boolean status = TRUE;

  /* Create and Initialise the structure that will have the spectral inversion support details from WTR*/
  rfdevice_cdma_tx_spectral_inversion_type tx_spectral_inversion_data; 

  /* Get the current device status state machine and assign it to the local device status structure */
  dev_status = rf_cdma_get_device_status( device );

  /* Get the logical device details for CDMA and assign it to the local logical device pointer */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* if logical_device itself is NULL then dont process anything just return */
  if ( NULL == logical_dev || NULL == dev_status)
  {
    RF_MSG_2 ( RF_ERROR, "rf_cdma_mc_trigger_dyn_txr_iq_dac_update(): "
               " unable to get valid device pointer for device: %d , band: %d " , 
               device , band ) ;
    return FALSE;
  } /* if ( NULL == logical_dev ) */
  
  /* Get the tx_device from the logical device pointer for the band */
  tx_device = logical_dev->tx_device[band];
  
  /* if tx_device itself is NULL then dont process anything just return */
  if ( NULL == tx_device )
  {
    RF_MSG_2 ( RF_ERROR, "rf_cdma_mc_trigger_dyn_txr_iq_dac_update(): "
               " tx device is NULL for device: %d , band: %d " , 
               device , band ) ;
    return FALSE;
  } /* if ( NULL == logical_dev->tx_device[band] ) */
  
  /* Get the txlm_handle info from out */
  txlm_handle = rf_cdma_data_get_txlm_handle(device);

  /* Tx Specral inversion query from device state machine */
  tx_spectral_inversion_data.band = band;

  /* Initialise the spec inv flag to FALSE to avoid any tx issues 
     as other WTR might not support this feature and will not have the driver code to get the details 
     thorugh cmd dispatch*/
  dynamic_cfg.tx_spectral_inversion = FALSE;

  /* Query the WTR driver code through cmd to get the spec inv support details */
  rfdevice_cdma_tx_cmd_dispatch( tx_device, 
                                 RFDEVICE_IS_TX_SPECTRAL_INVERSION_NEEDED,
                                 &tx_spectral_inversion_data);
  
  /* Pass the spectral inverstion support detail queried from WTR to msm layer */
  dynamic_cfg.tx_spectral_inversion = tx_spectral_inversion_data.spectral_inversion;

  /* Acquire Handle lock before writing any settings */
  status = ( txlm_acquire_lock(dev_status->txlm_handle) == LM_SUCCESS );
	
  /* Call the txr iq dac msm layer API to update the settings */
  status &= rf_cdma_msm_update_txr_iq_dac_settings(device, dynamic_cfg, txlm_handle);

  /* Check the status of the txlm setting update and trigger the FW to execute it onlyif settngs are updated successfully 
  to avoid applying improper setting to MSM*/ 
  if (status == TRUE)
  {
    /* Trigger the FW by sending msg to apply the setting to the MSM based on the mode */
    if (dev_status->rf_mode == RFM_1X_MODE)
    {
      status &= rf_1x_mdsp_trigger_dyn_txr_iq_dac_update( logical_dev->tx_mdsp_path,
					                  txlm_handle,
							          NULL, NULL,NULL ); 
    }
    else if (dev_status->rf_mode == RFM_1XEVDO_MODE)
    {
      status &= rf_hdr_mdsp_trigger_dyn_txr_iq_dac_update( logical_dev->tx_mdsp_path,
					                  txlm_handle,
							          NULL, NULL,NULL ); 
    }
    else
    {
      status = FALSE;      
      RF_MSG_2( RF_ERROR, "rf_cdma_mc_trigger_dyn_txr_iq_dac_update: "
   	                  "rf mode: %d for the device: %d is not correct", dev_status->rf_mode, device );
    }
    if (status == FALSE)
    {
      RF_MSG_1( RF_ERROR, "rf_cdma_mc_trigger_dyn_txr_iq_dac_update: "
   	                  "txr iq dac msg trigger failed for device %d", device );

    }

  } /*if (status == TRUE) */
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_mc_trigger_dyn_txr_iq_dac_update: "
  	                "txr iq dac settings update failed for device %d", device );
  }

  /* Release the lock once the update is done */
  status &= ( txlm_release_lock(dev_status->txlm_handle) == LM_SUCCESS );
	
  if (status == FALSE)
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_mc_trigger_dyn_txr_iq_dac_update: "
  	                 "txr iq dac msg trigger failed for device %d", device );
              
  } /*if (status == FALSE) */
 
  return status;

}/* rf_cdma_mc_trigger_dyn_txr_iq_dac_update */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Callback triggered by WTR driver to apply SSMA notches

  @details
  This API triggered as a callback by the WTR driver when there are spurs to
  suppress for SSMA or to clear previously applied notches on a chain, which
  are not required anymore.
  For example, if HDR tunes to a particular band-chan combination on device 0
  and 1x wakes up with a band chan combination that can potentially cause
  spurs on HDR and affect performance, the spurs need to be suppressed using
  notch filters. This API takes the spur recommendations from WTR and
  Applies the necessary register settings for that 

  @param cb_data
  pointer to callback data of type rfdevice_cdma_ssma_callback_data_type
  Contains the device information, notch use information,
  flag which tells if the notches need to be cleared or new notches need to
  be applied and recommedations from the WTR
 
  @return
  nothing for now

*/
void
rf_cdma_mc_ssma_notch_callback
( 
  rfdevice_cdma_ssma_callback_data_type *cb_data
)
{
  boolean api_status = TRUE;

  if ( cb_data->apply_notch_immediate == TRUE )
  {
    api_status = rf_cdma_mc_trigger_dynamic_notch_update( cb_data->device,
                                             cb_data->ssma_reco,
                                             cb_data->num_notch_filters_used,
                                             cb_data->clear_notch );

  }/* if ( cb_data->clear_notch == FALSE ) */
  else
  {
    rf_cdma_data_status_type *dev_status_w; /* Device Status Write Pointer */ 
    /* Get Writable Device Status */
    dev_status_w = rf_cdma_get_mutable_device_status( cb_data->device );

    if (  dev_status_w == NULL ) 
    {
      /* unable to get a valid device pointer - bail out */
      RF_MSG_1( RF_ERROR, "rf_cdma_mc_ssma_notch_callback: unable to get a valid "
                          "device pointer for device %d ", cb_data->device );
      api_status = FALSE;
    }
    else
    {
      /* Save the callback data to apply notches once Rx is enabled */
      memscpy( &(dev_status_w->ssma_cb_data), 
               sizeof(rfdevice_cdma_ssma_callback_data_type),
               cb_data, sizeof(rfdevice_cdma_ssma_callback_data_type) ); 

      RF_MSG_1( RF_LOW, "rf_cdma_mc_ssma_notch_callback:immediate notch flag"
                         " is false for device:%d, will apply"
                         " recommendations after enabling rx chain  ", 
                         cb_data->device ); 
    }
  }/* if ( cb_data->clear_notch == TRUE ) */

  RF_MSG_1( RF_LOW, "rf_cdma_mc_ssma_notch_callback:"
                     "API status :%d", api_status);

  return;

}/*rf_cdma_mc_ssma_notch_callback*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function to update the Rx path in case of concurrency with another tech

  @details
  This function will be used to update the Rx path in case the default path is
  being used by another tech. The API will call the concurrency manager to
  get pointer to an alternative path, This will be followed by a call to the
  RFC , asking it to update the Rx path to the newly obtained one. When we tune
  we will be tuning to the new Rx path as opposed to the default one.
  This is done only for the Rx path, since there can be multiple Rx paths
  but there will only be a single Tx path 
 
  @param current mode
  Current tech 1x/HDR
 
  @param device
  Device on which 1x subsystem is to be entered
 
  @param band
  The band requested for tune
 
  @param is_irat_mode
  Flag to indicate if API call is in IRAT mode or not, since the alt path
  manager might treat IRAT differently
 
  @return success
  TRUE: Everything went well. Call successful. 
  FALSE: Some error. Call unsuccessful.
*/
boolean
rf_cdma_mc_update_alt_rx_path 
(  
  const rfm_mode_enum_type curr_mode,
  const rfm_device_enum_type device ,
  const rfm_cdma_band_class_type band,
  const boolean is_irat_mode
)
{
  /* RFC type band to be sent to the rfc_update_alt_port_mapping() */ 
  rf_card_band_type rfc_band ;
  /* Flag to check if the call to concurrency manager was success */
  rfcmn_status_type alt_path_status;
  /* New path returned by the concurreny manager , in case there is another
  tech active on the default path, the concurrency manager will give us
  an alternative path. To tune to the newly assigned path we will need to
  first query for the alternate path, then update the RFC mapping for the
  front end to match that path, so that the front end is tuned accordingly */
  uint8 alt_path , current_path ;
  boolean success = TRUE ;

  current_path = rfc_cdma_get_current_rx_path ( device , band ) ;
 
  if ( is_irat_mode == FALSE )
  {
    /* Convert RFM band to RFC type */
    rfc_band = rf_cdma_convert_rfm_band_to_card_band ( band ) ;

    /* Get the alternate Rx path for this device and band */
    alt_path_status =  rfcmn_concurrency_mgr_update_rx_path ( device ,
                                                              curr_mode ,
                                                              ((int)band) ,
                                                              &alt_path ) ;

    if ( alt_path_status == RFCMN_PATH_SEL_SUCCESS )
    {
      RF_MSG_2 ( RF_MED , 
                 "rf_cdma_mc_update_alt_rx_path(): AltPath: %d for dev: %d " ,
                 alt_path , device ) ;                                        


      if ( current_path == alt_path )
      {   
        RF_MSG_2 ( RF_MED , "rf_cdma_mc_update_alt_rx_path(): Non-IRAT "
                            "On Dev: %d, alt_path == current_path == %d " ,
                            device , current_path ) ;
      }
      else
      {
        RF_MSG_3 ( RF_MED , "rf_cdma_mc_update_alt_rx_path(): "
                            "||Dev: %d|alt_path %d|current_path %d|| " ,
                            device , alt_path , current_path ) ;
        /* Update the port mapping with the alternate path information */
        success = 
          rfc_update_alt_port_mapping ( device, curr_mode, rfc_band, alt_path ) ; 
      }

      /* Error fatal if path update failed since front end will not be 
      configured as expected if this fails */
      if (success == FALSE)
      {
        RF_MSG_3(RF_ERROR,"rf_cdma_mc_update_alt_rx_path:Falied to update " 
                 "Alt port map for tech:%d ,device:%d , band %d ",curr_mode,
                 device, band );
      }
    }
    else
    {
      if (alt_path_status == RFCMN_PATH_SEL_NOT_SUPPORTED)
      {
        success = TRUE;
        RF_MSG_3(RF_HIGH,"rf_cdma_mc_update_alt_rx_path: alt rx path selection" 
               "is not supported for tech:%d ,device:%d , band %d ",curr_mode,
               device, band );
      }
      else
      /* Error fatal if path update failed since front end will not be 
      configured as expected if this fails */
      {
        success = FALSE;
        RF_MSG_3(RF_ERROR,"rf_cdma_mc_update_alt_rx_path:Falied to get " 
               "Alt port for tech:%d ,device:%d , band %d ",curr_mode,
               device, band );
      }
    }
  }/* if ( is_irat_mode == FALSE )*/
  else
  {
    /* Convert RFM band to RFC type */
    rfc_band = rf_cdma_convert_rfm_band_to_card_band( band );

    /* Get the alternate Rx path for this device and band */
    alt_path_status =  rfcmn_concurrency_mgr_update_irat_rx_path( device ,
                                                                  curr_mode ,
                                                                  ((int)band) ,
                                                                  &alt_path ) ;

    if ( alt_path_status == RFCMN_PATH_SEL_SUCCESS )
    {
      /* Update the port mapping with the alternate path information */
     if ( current_path == alt_path )
      {   
        RF_MSG_2 ( RF_MED , "rf_cdma_mc_update_alt_rx_path(): IRAT"
                            "On Dev: %d, alt_path == current_path == %d " ,
                            device , current_path ) ;
      }
      else
      {
        RF_MSG_3 ( RF_MED , "rf_cdma_mc_update_alt_rx_path(): IRAT "
                            "||Dev: %d|alt_path %d|current_path %d|| " ,
                            device , alt_path , current_path ) ;
        /* Update the port mapping with the alternate path information */
        success = 
          rfc_update_alt_port_mapping ( device, curr_mode, rfc_band, alt_path ) ; 
      }

      /* Error fatal if path update failed since front end will not be 
      configured as expected if this fails */
      if (success == FALSE)
      {
        RF_MSG_3(RF_ERROR,"rf_cdma_mc_update_alt_rx_path IRAT:Falied to update " 
                 "Alt port map for tech:%d ,device:%d , band %d ",curr_mode,
                 device, band );
      }
    }
    else
    {
      if (alt_path_status == RFCMN_PATH_SEL_NOT_SUPPORTED)
      {
        success = TRUE;
        RF_MSG_3(RF_HIGH,"rf_cdma_mc_update_alt_rx_path IRAT: alt rx path selection" 
               "is not supported for tech:%d ,device:%d , band %d ",curr_mode,
             device, band );
      }
      else
      /* Error fatal if path update failed since front end will not be 
      configured as expected if this fails */
      {
        success = FALSE;
        RF_MSG_3(RF_ERROR,"rf_cdma_mc_update_alt_rx_path IRAT:Falied to get " 
               "Alt port for tech:%d ,device:%d , band %d ",curr_mode,
               device, band );
      }
    }
    success = TRUE;
  }

  return success ;
}/* rf_cdma_mc_update_alt_rx_path */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function to update the Tuner State variable

  @details
  This function does a dummy update to the Tuner state variable using
  Dummy scripts which are deleted immediatly after update.
 

  @param device
  Device on which 1x subsystem is to be entered

  @return void
*/
void rf_cdma_mc_set_tuner_mode_using_dummy_script
(  
  const rfm_device_enum_type device 
)
{
   const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */ 		 
   rfcom_band_type_u						  band_in_common_format;		   
   void 									 *dummy_script = NULL;		 
   ant_tuner_device_tokens_type 			 *ant_tuner_token_w;		   
   const rf_cdma_tx_static_type 			 *tx_static_nv_ptr; 	   
   const rf_cdma_data_status_type			 *dev_status_r; 
                 
  /*******************************************************************************/ 
  /*   Dummy update the Tuner state. Current Sequence of generating scripts is   */    
  /*   Startup,Cleanup,Rx Burst(s)                                               */           
  /*   Rx Burst puts the Tuner in Rx state and so we need to do a Dummy update   */    
  /*   of Tuner to Tx state.                                                     */              
  /*******************************************************************************/        
            
  dev_status_r = rf_cdma_get_device_status( device ) ;           
  /* Get Logical Device Param */           
  logical_dev = rfc_cdma_get_logical_device_params( device );   
  if(dev_status_r == NULL || logical_dev == NULL)            
  {                 
	 RF_MSG_1 ( RF_ERROR, "rf_cdma_mc_set_tuner_mode_using_dummy_script: "   
		"dev_status_r or logical_dev NULL pointer" 
		"for device %d", device );             
	 return ;       
  }        
  tx_static_nv_ptr = rf_cdma_nv_get_tx_static( 
                               logical_dev->tx_nv_path[dev_status_r->curr_band], 
                               dev_status_r->curr_band );  
  if ( tx_static_nv_ptr == NULL )     
  {                  
	RF_MSG_1 ( RF_ERROR, "rf_cdma_mc_set_tuner_mode_using_dummy_script: "   
		"Failed to get valid static NV pointer "    
		"for device %d", device );         
	return ;        
  }                            

  ant_tuner_token_w = rf_cdma_get_mutable_ant_tuner_token( device );    

  /* Make sure the toekn ptr is non-null */
  if ( ant_tuner_token_w == NULL )
  {
    RF_MSG_1 ( RF_ERROR, "rf_cdma_mc_set_tuner_mode_using_dummy_script: "   
      " Antenna tuner token pointer is NULL for  device %d", device );         
    return ;        
  }
  else
  {
    /* Dummy script so use minimum size for RFFE */    
    dummy_script = (void *)rf_buffer_create(0,      
                                    RF_CDMA_MAX_GRFC_SCRIPT_SIZE,                
                                              0);              			           
    band_in_common_format.cdma_band = dev_status_r->curr_band ;  

    rf_cdma_atuner_init( logical_dev->ant_tuner[dev_status_r->curr_band ],   
                       dev_status_r->rf_mode,                  
                       RFDEVICE_CREATE_SCRIPT,          
                       dummy_script,             
                       0, /* No timing offset */     
                       ant_tuner_token_w);	   

    rf_cdma_atuner_configure_for_tx( rf_cdma_data_get_txlm_handle (device) ,	
                                   logical_dev->ant_tuner[dev_status_r->curr_band ],		
                                   dev_status_r->rf_mode,						
                                   band_in_common_format,		
                                   dev_status_r->curr_chans[0],
                                   RFDEVICE_CREATE_SCRIPT,		
                                   dummy_script,			
                                   0, /* No timing offset */	
                                   ant_tuner_token_w,		
                                   (void*)&(tx_static_nv_ptr->ant_tuner_cl) );  
    /* Delete dummy transition buffers */   
    if (dummy_script!= NULL)       
    {           
       rf_buffer_delete((rf_buffer_intf*)dummy_script);  
    }      
  }/* if ! ( ant_tuner_token_w == NULL ) */

}/* rf_cdma_mc_set_tuner_mode_using_dummy_script */



/*----------------------------------------------------------------------------*/
/*! @brief 
  API to disable intelliceiver.

  @details
  API to disable intelliceiver. This function will put intelliceiver state
  machine to stop state. It will clear all the timers used for
  intelliceiver polling. This function will also compute next
  intelliceiver linearity mode by querying the WTR driver and
  checking the high linearity and low linearity mode counter.
  The computed linearity mode value will be used to start
  intellicevier in future when "Start Intelliceiver" function is
  called.
 
  @param device
  Device whose IC needs to be disabled.
 
  @return
  Status of function execution.
  TRUE: API Successful
  FALSE: API will return error.
*/
boolean
rf_cdma_mc_disable_ic
(
  const rfm_device_enum_type device   
)
{
  boolean success = FALSE ;
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  rf_cdma_ic_data_type *ic_data_w; /* Intelliceiver write pointer*/

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params ( device ) ;

  /* Get CDMA Data Write Pointer*/
  ic_data_w = rf_cdma_get_mutable_intelliceiver ( device ) ;

  /* Check for NULL pointers */
  if( ( logical_dev == NULL ) || ( ic_data_w == NULL ) )
  {
    /* unable to get a valid device pointer - bail out */
    RF_MSG_1( RF_ERROR, "rf_cdma_disable_ic - Unable to get a "
              " valid pointer for device %d", device ) ;
  } /* if ( dev_status_w == NULL ) ) */
  else
  {
    /* Stop IC */
    success = rf_cdma_ic_stop ( ic_data_w, logical_dev );

    if( success == FALSE  )
    {
      /* Failed to stop ICeiver */
      RF_MSG_1( RF_ERROR, "rf_cdma_disable_ic - Failed to disable "
                " IC for device %d", device );
    }
  }

  return success ;
} /* rf_cdma_disable_ic */

/*!
  @brief
  Enable IC on a given device

  @details
  This function will enable intelliceiver on a given device
 
  @param device
  Device whose IC needs to be enabled.
 
  @return
  Status of function execution.
  TRUE: API Successful
  FALSE: API will return error.
*/
boolean
rf_cdma_mc_enable_ic
(
  const rfm_device_enum_type device   
)
{
  boolean success = FALSE ;
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  rf_cdma_ic_data_type *ic_data_w; /* Intelliceiver write pointer*/
  rf_cdma_data_status_type* dev_status_w ; /* Device Status Write Pointer */

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params ( device ) ;

  /* Get CDMA Data Write Pointer*/
  ic_data_w = rf_cdma_get_mutable_intelliceiver ( device ) ;

  /* Get Writable Device Status */
  dev_status_w = rf_cdma_get_mutable_device_status( device ) ;

  /* Check for NULL pointers */
  if ( ( logical_dev == NULL ) || ( ic_data_w == NULL ) || 
       ( dev_status_w == NULL ) )
  {
    /* unable to get a valid device pointer - bail out */
    RF_MSG_1( RF_ERROR, "rf_cdma_enable_ic - Unable to get a "
              " valid pointer for device %d", device ) ;
  } /* if ( dev_status_w == NULL ) ) */
  else
  {
    /* Stop IC */
    success = rf_cdma_ic_start ( ic_data_w, logical_dev,
                                 dev_status_w->curr_band, 
                                 dev_status_w->lo_tuned_to_chan ) ;

    if( success == FALSE  )
    {
      /* Failed to start ICeiver */
      RF_MSG_1 ( RF_ERROR, "rf_cdma_enable_ic - "
                 "Failed to Start IC for device %d" , device );
    }
  }

  return success ;

} /* rf_cdma_enable_ic */

/*!
  @brief
  Return the PA switchpoints for given tx device and band
 
  @param rfm_device_enum_type: tx_device.
  Tx device for which PA SWPTS are queried.
 
  @param rfcom_cdma_band_type: band
 
  @param pa_rise_swpts: pointer to an array for PA rise
  switchpoints
 
  @param pa_fall_swpts: pointer to an array for PA fall
  switchpoints
 
  @param active_pa_states: number of active PA states
 
  @retval TRUE if PA switchpoints are populated in NV, FALSE if
  PA switchpoints are not populated in NV
*/

boolean 
rf_cdma_mc_get_pa_switchpoints
( 
  rfm_device_enum_type      device , 
  rfm_cdma_band_class_type  band , 
  int16                     pa_rise_swpts[] , 
  int16                     pa_fall_swpts[] , 
  int16*                    active_pa_states
) 
{
  boolean result = FALSE ; /* final result of API */

  if  ( FALSE == rfc_cdma_is_tx_device (device) )
  {
    RF_MSG_1 ( RF_ERROR, "rf_cdma_mc_get_pa_switchpoints: "
               "Device %d is not a Tx device API will return FALSE " , device ) ;
  }
  else
  {
    const rfc_cdma_logical_device_params_type* logical_dev = 
      rfc_cdma_get_logical_device_params ( device ) ;

    if ( NULL == logical_dev )
    {
      RF_MSG( RF_ERROR, "rf_cdma_mc_get_pa_switchpoints: "
                        "Null pointer returned for logical_dev ");
    }
    else
    {
      /* ensure that NV is valid for given device and band */
      const rf_1x_config_type* rf_1x_config_ptr = 
        rf_cdma_nv_get_1x_data ( logical_dev->tx_nv_path[band] , band ) ;

      if ( rf_1x_config_ptr == NULL )
      {
        RF_MSG( RF_ERROR, "rf_cdma_mc_get_pa_switchpoints: "
                          "Null pointer returned for 1x NV pointer " ) ;
      }
      else
      {
        rf_cdma_tx_static_type *tx_static_struct = 
          &(rf_1x_config_ptr->cdma_common_ptr->tx_static) ;
   
        if ( tx_static_struct == NULL )
        {
          RF_MSG( RF_ERROR, "rf_cdma_mc_get_pa_switchpoints: "
                              "Null pointer returned for tx_static_struct ");
        }
        else
        {
          if ( tx_static_struct->pa_static_nv_version == RFCMN_NV_LEGACY_FOUR_STATE_PA )
          {
            RF_MSG ( RF_MED , "rf_cdma_mc_get_pa_switchpoints: "
                         "4-State Legacy PA NV used. API will return FALSE " ) ;
          }
          else if ( tx_static_struct->pa_static_nv_version == RFCMN_NV_PA_STATIC_VER_1 )
          {
            /* Pointer to 1x static NV data */
            const rf_1x_tx_static_type* tx_1x_config_ptr = 
              &(rf_1x_config_ptr->tx_static) ;
            if ( NULL == tx_1x_config_ptr )
            {
              RF_MSG( RF_ERROR, "rf_cdma_mc_get_pa_switchpoints: "
                      "Null pointer returned for tx_1x_config_ptr " ) ;
            }
            else
            {
              /* Ptr to Tx switchpoints */
              rfm_pa_gain_state_type pa_state ;
              const rf_cdma_tx_switch_points_type* tx_1x_swpts_ptr ; 

              tx_1x_swpts_ptr = &tx_1x_config_ptr->voice_swpts ;
              *active_pa_states = tx_static_struct->num_active_pa_states ;
              
              for ( pa_state = RFM_PA_GAIN_STATE_0; 
                    pa_state < RFM_PA_GAIN_STATE_NUM; 
                    pa_state++ )
              {
                /* Populate PA Rise Val */
                pa_rise_swpts[pa_state] = tx_1x_swpts_ptr->rise[pa_state] ;
                /* Populate PA Fall Val */
                pa_fall_swpts[pa_state] = tx_1x_swpts_ptr->fall[pa_state] ;
              }

              RF_MSG_3 ( RF_LOW , "rf_cdma_mc_get_pa_switchpoints: " 
                         "TxDev: %d on band %d has %d active PA states" ,
                         device , band , *active_pa_states ) ;

              RF_MSG_8( RF_LOW, "rf_cdma_mc_get_pa_switchpoints: "
                        "PA SP [F0 %d, F1 %d, F2 %d, F3 %d, "
                        "F4 %d, F5 %d, F6 %d, F7 %d] dBm10" , 
                        pa_fall_swpts[RFM_PA_GAIN_STATE_0] , 
                        pa_fall_swpts[RFM_PA_GAIN_STATE_1] , 
                        pa_fall_swpts[RFM_PA_GAIN_STATE_2] , 
                        pa_fall_swpts[RFM_PA_GAIN_STATE_3] ,
                        pa_fall_swpts[RFM_PA_GAIN_STATE_4] ,
                        pa_fall_swpts[RFM_PA_GAIN_STATE_5] ,
                        pa_fall_swpts[RFM_PA_GAIN_STATE_6] ,
                        pa_fall_swpts[RFM_PA_GAIN_STATE_7] );
              RF_MSG_8( RF_LOW, "rf_cdma_mc_get_pa_switchpoints: "
                        "PA SP [R0 %d, R1 %d, R2 %d, R3 %d, "
                        "R4 %d, R5 %d, R6 %d, R7 %d] dBm10" , 
                        pa_rise_swpts[RFM_PA_GAIN_STATE_0] , 
                        pa_rise_swpts[RFM_PA_GAIN_STATE_1] , 
                        pa_rise_swpts[RFM_PA_GAIN_STATE_2] , 
                        pa_rise_swpts[RFM_PA_GAIN_STATE_3] ,
                        pa_rise_swpts[RFM_PA_GAIN_STATE_4] ,
                        pa_rise_swpts[RFM_PA_GAIN_STATE_5] ,
                        pa_rise_swpts[RFM_PA_GAIN_STATE_6] ,
                        pa_rise_swpts[RFM_PA_GAIN_STATE_7] );

              result = TRUE ;
            } /* if ( NULL == tx_1x_config_ptr )*/
          } /* if ( pa_static_nv_version == RFCMN_NV_PA_STATIC_VER_1 ) */
        } /* if ( tx_static_struct != NULL ) */
      } /* if ( rf_1x_config_ptr == NULL ) */
    } /* ( NULL != logical_dev ) */
  } /* is this a Tx device */

  return result ;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Prepare RF to make it ready for impending QTA.

  @param device
  The device on which QTA will be performed.
 
  @param params
  QTA params that need to be configured.
 
  @return
  Status of function execution.
  TRUE: API Successful.
  FALSE: API will return error.
*/
boolean
rf_cdma_mc_prep_qta
(
   const rfm_device_enum_type         device  ,
   rfm_cdma_qta_params*               params  
) 
{
  boolean success = TRUE ;

  /* Step 1: 
     Disable intelliceiver before entering QTA gap, FW loses the Rx chain when
     we enter the gap, therefore any intelliceiver operations are void, FW will
     reject RxAGC config and crash when IC update mssg is sent */
  
  success &= rf_cdma_mc_disable_ic ( device ) ;

  /* Other Pre QTA steps here... */  

  return success ;
}



/*----------------------------------------------------------------------------*/
/*!
  @brief
  Prepare RF after QTA has concluded.

  @param device
  The device on which QTA will be performed.
 
  @return
  Status of function execution.
  TRUE: API Successful.
  FALSE: API will return error.
*/
boolean
rf_cdma_mc_end_qta
(
  const rfm_device_enum_type         device
) 
{
  boolean success = TRUE ;

  /* Step 1: 
     Disable intelliceiver before entering QTA gap, FW loses the Rx chain when
     we enter the gap, therefore any intelliceiver operations are void, FW will
     reject RxAGC config and crash when IC update mssg is sent */
  
  success &= rf_cdma_mc_enable_ic ( device ) ;

  /* Other Post QTA steps here... */

  return success ;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This is a helper function for tune back .
 
  @param device
  The radio path to re-tune.

  @param is_slave_device
  Is slave device.
 
  @param rxlm_handle
  The Rx Link Manager buffer that shall be used to configure the
  modem hardware.
 
  @param band
  CDMA band
 
  @param num_band_chan
  The length of the band_chan parameter array.

  @param band
  The band to tune to.
 
  @param lo_tuned_to_chan
  The channel to tune to.

  @param script_index
  Script index.
 
  @param rx_bw_khz
  Rx BW
 
  @param tx_bw_khz
  Tx BW
  
  @param power_mode
  Current power mode
   
  @param is_irat_mode
  Boolean flag to check if the API is being called in IRAT or non-IRAT scenarios
  In IRAT scenarios, a lot of API calls can be skipped ( ex. DLNA Rx on/off 
  scripting, vregs etc.)
 
  @param meas_type 
  The current measurement type

  @return
  TRUE/FALSE
*/
boolean
rf_cdma_mc_helper_for_tune_back
(
  const rfm_device_enum_type device,
  const boolean is_slave_device,
  const lm_handle_type rxlm_handle,
  const rfm_cdma_band_class_type band,
  uint16 lo_tuned_to_chan,
  uint32 script_index,
  uint32 rx_bw_khz,
  uint32 tx_bw_khz,
  rfm_cdma_power_mode_type power_mode,
  boolean is_irat_mode,
  const rfdevice_meas_scenario_type meas_type,
  const rfm_mode_enum_type rf_mode
)
{
  boolean exec_success = TRUE;

  rf_cdma_data_status_type *dev_status_w; /* Device Status Write Pointer */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */

  /*--------------------------------------------------------------------------*/
  /* Get pointers to device state                                             */
  /*--------------------------------------------------------------------------*/
  logical_dev = rfc_cdma_get_logical_device_params( device );
  
  /* Get Writable Device Status */
  dev_status_w = rf_cdma_get_mutable_device_status( device );

  if ( ( logical_dev == NULL ) || ( dev_status_w == NULL ) )
  {
    /* unable to get a valid device pointer - bail out */
    RF_MSG_1( RF_ERROR, "rf_cdma_mc_helper_for_tune_back: unable to get a valid "
                        "device pointer for device %d ", device );
    return RFM_CDMA_ERROR_NULL_DATA;
  }


  /* Enter Device Critical Section, so that the device call is thread safe */
  rfdevice_cdma_rx_enter_critical_section( logical_dev->rx_device[band] );
 
  /* Call the CDMA driver to prepare the front end tune back scripts for 
  WTR\ASM\GRFC etc */
  exec_success &= 
  	rf_cdma_mc_configure_front_end_for_rx_wakeup( 
  	                          device,
                                  rf_mode, 
                                  is_slave_device,
                                  rxlm_handle, 
                                  band, 
                                  lo_tuned_to_chan, 
                                  power_mode, 
                                  script_index, 
                                  rx_bw_khz, 
                                  tx_bw_khz, 
                                  logical_dev,
                                  &dev_status_w->ant_tuner_script_token_data,
                                  is_irat_mode,
                                  meas_type);

  /* Leave Device Critical Section */
  rfdevice_cdma_rx_leave_critical_section( logical_dev->rx_device[band] ); 

  /* Send the rx_start_cfg message to FW to pass the IRAT\QTA related info */
  if ( exec_success == TRUE )
  {
    /* check the mode and call the start rx helper API accordingly */
    if (rf_mode == RFM_1XEVDO_MODE)
    {
      exec_success &= 
	  	rf_hdr_mc_send_start_rx_helper ( 
	  	                       device , 
                              (const rf_cdma_data_status_type*) dev_status_w , 
                               rxlm_handle ,
                               logical_dev,
                               is_irat_mode,
                               FALSE /*is_retune*/) ;
    }
    else if (rf_mode == RFM_1X_MODE)
    {
      exec_success &= 
	  	rf_1x_mc_send_start_rx_helper ( 
	  	                       device , 
                              (const rf_cdma_data_status_type*) dev_status_w , 
                               rxlm_handle ,
                               logical_dev,
                               is_irat_mode,
                               FALSE /*is_retune*/) ;
    }
    else
    { 
      RF_MSG_1( RF_ERROR, "rf_cdma_mc_helper_for_tune_back: rf mode %d is invalid ",
                 rf_mode );
	  exec_success &= FALSE;
    }

    if ( logical_dev->ant_tuner[band] != NULL )
    {
      rf_cdma_atuner_ack_script( logical_dev->ant_tuner[band], 
                                 &dev_status_w->ant_tuner_script_token_data );
    }

  } /* if ( exec_success == TRUE ) */

  /* Clean up all CCS events that happened on this device */
  exec_success &= rf_cdma_meas_cleanup_ccs_events(device);

  return exec_success;

} /* rf_cdma_mc_helper_for_tune_back */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This is the function for handling CDMA tune back .
 
  @param master_dev
  The master device.

  @return
  RFM_CDMA_HEALTHY_STATE/RFM_CDMA_ERROR_FAILURE
*/
rfm_wait_time_t
rf_cdma_mc_tune_back
(
  const rfm_device_enum_type master_dev,
  const rfm_mode_enum_type rf_mode
)
{
  rfm_wait_time_t ret_val = RFM_CDMA_HEALTHY_STATE;

  boolean exec_success = TRUE;

  rf_cdma_data_status_type *m_dev_status; /* Device Status Write Pointer */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  uint32 script_index;
  rfm_cdma_power_mode_type power_mode; 

  /*--------------------------------------------------------------------------*/
  /* Get pointers to device state                                             */
  /*--------------------------------------------------------------------------*/
  logical_dev = rfc_cdma_get_logical_device_params( master_dev );
  
  /* Get Writable Device Status */
  m_dev_status = rf_cdma_get_mutable_device_status( master_dev );

  if ( ( logical_dev == NULL ) || ( m_dev_status == NULL ) )
  {
    /* unable to get a valid device pointer - bail out */
    RF_MSG_1( RF_ERROR, "rf_cdma_mc_tune_back: unable to get a valid "
                        "device pointer for device %d ", master_dev );
    return RFM_CDMA_ERROR_NULL_DATA;
  }
  
  script_index =  m_dev_status->irat_data.irat_buffer_index ;

  if ( m_dev_status->irat_data.irat_rxlm_handle != m_dev_status->rxlm_handle )
  {
    exec_success = FALSE;
  }

  if ( exec_success == TRUE )
  {
    /* Set lowest power mode in IRAT scenario */
    power_mode = rf_cdma_ic_get_lowest_power_mode 
                                          ( &( m_dev_status->intelliceiver )) ;

    /* Call the helper API to prepare the tune back scripts and send message to FW */
    exec_success &= rf_cdma_mc_helper_for_tune_back (master_dev,
                                                     FALSE,
                                                     m_dev_status->rxlm_handle,
                                                     m_dev_status->curr_band,
                                                     m_dev_status->lo_tuned_to_chan,
                                                     script_index,
                                                     m_dev_status->rx_bw_khz,
                                                     m_dev_status->tx_bw_khz,
                                                     power_mode,
                                                     TRUE,
                                                     RFDEVICE_MEAS_CLEANUP_SCRIPT,
                                                     rf_mode);


    if ( m_dev_status->assoc_dev != RFM_INVALID_DEVICE )
    {
      /* Dev Status Read Ptr for slave device */
      const rf_cdma_data_status_type *s_dev_status; 

      /* holds the associated device */
      rfm_device_enum_type slave_dev = m_dev_status->assoc_dev;

      /* get device status for slave */
      s_dev_status = rf_cdma_get_device_status(slave_dev);

      /*NULL pointer check*/
      if( (s_dev_status != NULL) && 
          (rf_cdma_data_get_rf_state(slave_dev,rf_mode) == RF_CDMA_STATE_RXDIV) )
      {
        /* Call the helper API to prepare the tune back scripts and send message to FW */
        exec_success &= rf_cdma_mc_helper_for_tune_back (slave_dev,
                                                         TRUE,
                                                         s_dev_status->rxlm_handle,
                                                         m_dev_status->curr_band,
                                                         m_dev_status->lo_tuned_to_chan,
                                                         script_index,
                                                         m_dev_status->rx_bw_khz,
                                                         m_dev_status->tx_bw_khz,
                                                         power_mode,
                                                         TRUE,
                                                         RFDEVICE_MEAS_CLEANUP_SCRIPT,
                                                         rf_mode);
      } /* if ! ( dev_status_r_slave == NULL ) */

    } /* if ( dev_status_r->assoc_dev != RFM_MAX_DEVICES ) */

  }/* if ( exec_success == TRUE ) */  

  if ( exec_success == FALSE )
  {
    ret_val = RFM_CDMA_ERROR_FAILURE;
  }
  return ret_val; 
  
}



#endif /* FEATURE_CDMA1X */

/*! @} */

/*! @} */

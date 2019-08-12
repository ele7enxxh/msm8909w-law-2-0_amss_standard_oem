/*! 
  @file
  rf_hdr_meas.c
 
  @brief
  This file defines the HDR Measurement Interface.
 
  @addtogroup RF_CDMA_HDR_MEAS
  @{
*/

/*==============================================================================

  Copyright (c) 2012 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/hdr/rf/src/rf_hdr_meas.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/16/16   vr      club 1X2G/Do2G QTA changes
11/27/15   rs      Added NV control support for FR25072
11/23/15   rs      Add band as argument to wtr power down API
11/23/15   rs      Power down WTR in case of abort
03/16/15   vc      Remove CCS event cleanup in meas exit while aborted gap DO2G
01/12/15   vc      Cleanup the TQP resources in case of a aborted GAP.
12/02/14   as/spa  Protected tuner configuration under ASDiv lock
10/23/14   spa     Remove redundant F3s
10/23/14   spa     Stop IC at QTA  enter and Start at QTA exit
08/14/14   sbo     Create tuneback functions for QTA and do dummy update of tuner 
07/31/14   fh      Handle meas gap aborted for Do2G QTA per L1 request
07/16/14   fh      No rf state update for cleanup script
06/28/14   fh      Added api for cleanup script
06/23/14   fh      Change state to RXTX on meas exit
05/20/14   fh      Added DO2G support 
02/13/14   APU     Request TQ just before starting Rx. Release TQ before going 
                   to sleep thus guaranteeing symmetric TQ request\release.
11/27/13   APU     IRAT Changes FOR bolt.
01/16/13   pk      IRAT Reset Script for Tuner disable
08/22/13   spa     Check for OHO/abort only when CDMA is target tech
08/15/13   Saul    SB. Made rf_state 1x/hdr specific.
08/13/13   spa     Get device based on HDR being source or target in meas  exit
08/06/12   APU     In meas_exit, check target_param for NULL before accessing it
08/01/13   APU     Support for optimised hand off case where build scripts is 
                   skipped.    
07/08/13   APU     Use L1 abort flag to skip meas exit events.
07/05/13   spa     Use rf_cdma_meas_cleanup_ccs_events for CCS event cleanup
07/03/13   spa     Renamed scripts_build_scripts and added documentation
07/01/13   APU     Call RX_STOP from 1 place and remove 
                   rf_1x_cleanup_post_measurement() because we use sleep when 
                   call meas_exit()
06/27/13   spa     Removed unused/no-op APIs 
06/27/13   spa     Add 'is_irat_mode' flag to APIs to check if API call is in 
                   IRAT or non IRAT scenarios 
06/27/13   APU     Fix for D2L irat crash where sleep after gap had invalidated
                   Variables.
06/14/13   APU     IRAT APIs now use the MC APIs instead of having seperate code
                   and call flow.
06/14/13   spa     Use CDMA wrapper object for antenna tuner
06/04/13   cd      Add support to clean-up Rx AGC LNA SM events
05/30/13   spa     Mainlined "use_new_fwrsp", use new RF dispatch implementation 
05/29/13   spa     Added delay param to APIs for getting GRFC scripts
05/24/13   APU     Eliminated unwanted event information structures.
05/22/13   sty     Acquire CS only when needed - in 
                   rf_hdr_cleanup_post_measurement
05/22/13   sty     use RF_CDMA_RX_STOP_SETTLE_TIME_IRAT_CLEANUP for rx_stop
05/09/13   spa     Added missing header (compiler warning fix)
05/08/13   spa     Use common buffer for creating post meas script (as target)
04/29/13   sty     Reduced Rx settling time for IRAT scenarios
04/26/13   spa     Fix check for source/target in build reset script before 
                   printing F3 
04/13/13   spa     Added is_irat_mode flag to configure_rx_agc
04/11/13   ndb     Remvoed the unused header file 
04/10/13   ndb     Removed the unused Qtuner Nikel Functions
04/08/13   pl      rename build_cleanup_script top build_reset_script
04/08/13   pl      Add null function pointer to support pre_config
04/04/13   spa     Call cleanup post measurement API in build cleanup scripts
04/04/13   spa     Cleaned up unused code
04/01/13   spa     Migrated to RF Dispatch for RF-FW synchronization
03/19/13   spa     Updated cleanup CCS event API
02/06/13   bmg     Adding initial antenna tuner manager support
01/28/13   zhw     RF front end device wakeup/sleep API refactor
01/27/13   sty     Call rf_cdma_mc_create_rx_device_wakeup_script() in build 
                   script API instead of rf_cdma_mc_create_rx_device_sleep_script
01/18/13   fh      Triton compile error fixed
01/12/13   cd      RxAGC LNA SM event configuration support
12/27/12   shb     Updates for different physical rx_device per band
12/19/12   sty     Fixed return value in rf_hdr_cleanup_post_measurement
12/18/12   sty     KW fix for possible NULL pointer dereference
12/12/12   APU     1> Message Cleanup
                   2> Initialize the carrier index and channel array before 
                   sending it to configure RxAGC API.
12/04/12   aro     Added HDR support for RF-FW synchronization
12/03/12   APU     Changes to clean CCS events
11/29/12   av      Using proper featurization and removing featurization that is not needed
11/20/12   gvn     Featurize LTE for Triton 
11/19/13   APU     Implemented rf_1x_mc_meas_build_cleanup_scripts().
11/13/12   APU     Make changes to accomodate the meas framework changes.
11/09/12   zhw     Compiler warning fix
11/07/12   zhw     Support for RF device scripting. Rename RF event script
11/07/12   pl      Added cleanup function pointer support in API structure
10/31/12   adk     Merged antenna tuner code from Nikel.
10/17/12   APU     Fixed compiler warnings.
10/12/12   sty     Deleted refs to ant_tuner
10/10/12   APU     New IRAT interface changes.
09/27/12   zhw     Remove write_sbi/grfc_via_rfsw flag
09/17/12   sty     Changed order of args in rf_cdma_mdsp_configure_devices
09/14/12   spa     Removed refs to unsupported SPI antenna tuner scripts
09/13/12   sty     Renamed RF_CDMA_EVENT_ANALOG_SCRIPT to RF_CDMA_STATIC_EVENT
09/10/12   spa     Removed references to v2 device scripting APIs
09/10/12   spa     Removed unused API append_new_style_script_to_old_style
09/10/12   spa     Added WTR scripting support and removed legacy methods 
09/06/12   aro     FWRSP registration for IRAT_RX_START response
09/06/12   aro     FWRSP registration for ACQ_2_TRK_IND response
09/05/12   aro     Added RF-FW Sync callback parameter in HDR MDSP Interface
08/28/12   spa     Compiler warning fix 
08/28/12   spa     changed asm device to per band 
08/22/12   aro     Renamed interface names and type definitions (time prof)
08/22/12   aro     Changes to migrate to new precide time profiler
08/21/12   APU     Added support to get modem_chain from rfc_device_info
08/08/12   cvd     Replace timetick with rfcommon_time_profile interface 
07/27/12   APU     Enabled scripting for PA ASM. 
07/23/12   Saul    CDMA. Now using semaphore APIs from RF COMMON.
07/11/12   shb     Updated script_buffer to be of rf_buffer_intf* type 
                   instead of void ptr
07/06/12   spa     Updated rfc_cdma_generate_rx_wakeup_grfc_script to new params 
07/03/12   spa     Changed do_grfc_writes_in_rf_pre_state to boolean 
07/03/12   spa     Changed do_sbi_in_rf_pre_state to boolean type 
06/29/12   spa     Compiler warning fix  
06/28/12   APU     Removed old QFE specific calls. Replaced with new generic PA 
                   interface. 
06/28/12   APU     Changed QFE to the new generic PA interface. 
06/19/12   zhw     QFE1320 scripting support
06/13/12   aro     Updated RxLM interface to indicate, if dynamic config is to 
                   be applied or not + if ET configuration is to be applied
06/06/12   Saul    CDMA. Sending scripts as Dime shared mem format.
                   MC now using mdsp events.
05/29/12   zhw     Refactored API naming. Removed inclusion of rfm_internal.h  
05/29/12   zhw     Moved debug flag do_grfc_write to rf_cdma_data 
05/19/12   zhw     Moved debug flag do_sbi_write to rf_cdma_data 
05/14/12   aro     Removed CritSect from Meas Script Enter and Exit function
05/14/12   aro     Updated HDR FW messages to use semaphores from SemPool for
                   RF-FW synchronization
05/11/12   zw      Updated GRFC generating API 
05/10/12   zw      Renamed rtr_handle to script_index 
05/03/12   sty     use correct band while cleaning up IRAT in L->DO case 
05/01/12   zhaow   Updated rf_cdma_ic_stop()
04/27/12   zhaow   Updated rf_cdma_ic_stop()
04/26/12   sty     Removed zeroing out num_grfcs
04/25/12   sty     Fixed un-inited var in rf_hdr_meas_script_build_scripts 
04/19/12   zhaow   Updated set APIs usage 
04/19/12   Saul    1X/HDR. Handoff fix. Zero out GRFC num only once.
04/18/12   zhaow   Replaced write pointer assignment statements with set APIs 
                   of rf_cdma_data data structure
04/18/12   aro     Callback function for Rx/Tx Start/Stop interfaces
04/17/12   sty     Deleted references to enable_sbi_scripting
04/16/12   sty     Added GRFC script to rf_hdr_mdsp_configure_analog_script()  
04/11/12   Saul    QFE. Initial support.
04/09/12   sty     Added helper functions that:
                  (a)get and release device-locks                  
                  (b) program the RTR and grfcs
                  (c) send relevant commands to MDSP (RX_START, etc)
04/04/12   aro     Added callback functions to HDR Tx interfaces
04/03/12   aro     callback function for HDR MDSP Rx interfaces
03/29/12   shb     Added rf_state arg to rf_hdr_configure_rx_agc as it is needed 
                   by device lna phase offset query api 
03/22/12   aro     Migrated handling of HDR FW responses to RF_FWRSP task
03/21/12   spa     Added max device check in rf_cdma_get_mutable_device_status()
03/21/12   adk     Update rxf_get_iqmc_coef_type to retrieve IQMC coeff for SHDR
03/21/12   Saul    IRAT. KW Fix from previous check-in
03/21/12   Saul    IRAT. HDR to set second device to sleep in L2DO.
03/14/12   aro     Stopping intelliceiver operation during HDR cleanup
                   in DO2L irat
03/14/12   vrb     Configure RxLM API requires band of operation
03/08/12   spa     Updated execute_wakeup_rx to use CDMA specific band and chan 
03/02/12   Saul    MCDO. Rx AGC freq comp and device droop compensation framework.
02/08/12   shb     Added argument in CDMA device rx tune api to distinguish 
                   between MEAS script generation and standalone wakeup
01/20/12   shb     Updated CDMA device interface to use generic 
                   rfdevice_ssbi_write_type and rfdevice_script_type instead of
                   CDMA specific structures
01/19/12   Saul    MCDO. Added ICIFIR dynamic configuration.
01/19/12   aro     Updated all HAL interfaces to use RxLM/TxLm handle rather
                   than logical device
01/11/12   Saul    MCDO. Passing bandwidth for IQMC coefficients query.
                   Passing mode to Tx tune script.
01/10/12   Saul    MCDO. RTR now taking EVDO mode. 
                   Allow operation in multi-carrier mode and bandwidths.
01/04/12   aro     Cleaned up the Rx and Tx bandwidth configuration
01/03/12   aro     Interface to query TxLM chain for the given TxLM buffer
01/03/12   aro     Added TxLM Chain specific to 1x and HDR in logical
                   device param
12/29/11   aro     Interface update to pass ADC info to RxLM configure function
12/29/11   aro     Updated configure RxAGC interface to have RxLM buffer
12/21/11   aro     KW fix
12/21/11   aro     Added a new function to perform HDR cleanup which is
                   called before enter LTE script enter in DO2L measurement.
12/21/11   aro     Removed RX_STOP from HDR Script enter
12/21/11   aro     Updated HDR script functions to have one return in the code
                   to avoid early returns
12/21/11   aro     Added code to acquire and release CDMA critical section
12/19/11   sty     Init dyn_settings before using it
12/17/11   aro     Fixed the bug where the Script Flag gets hung in case of
                   iRAT script generation failure
12/16/11   Saul    IRAT. D2L HDR Stop Rx sequence change to delay premature 
                   RF sleep/vreg shutdown which affects LTE measurements.
12/13/11   Saul    MCDO. Passing rx/tx bandwidth to device driver.
12/06/11   adk     Code cleanup
11/30/11   sty     Added new arg while invoking rf_hdr_configure_rx_agc()
11/23/11   Saul    IRAT update. Added RTR reset in Rx reset API to fix lower
                   Rx AGC readings that happen in some channels.
11/21/11   Saul    IRAT update. Fixed Rx AGC.
11/15/11   Saul    IRAT update. Removed large unnecessary variable.
10/28/11   Saul    IRAT update. Added debug messages.
10/28/11   Saul    IRAT update. Antenna Tuner support.
10/05/11   Saul    IRAT update. Changed PLL settling from 500 to 615 CX1 units.
09/28/11   Saul    IRAT update. Added debug message.
09/27/11   adk     Moved SSBI scripting-related flags to rf_cdma_debug_flags.
09/21/11   adk     Removed obsolete code.
09/11/11   aro     Merged CDMA device interface to delete
                   rfdevice_1x_interface.h
09/08/11   Saul    IRAT bug-fix. Build scripts regardless of target tech.
09/08/11   Saul    IRAT update. HDR bug fix for using narrow-band 2.
08/25/11   aro     Changed the narrowband path for DO measurement to be NB2,
                   since current hardware does not support mapping from NB0
                   to SSW2
08/04/11   Saul    IRAT update. Added control to use actual RTR handle.
08/03/11   Saul    IRAT update. Now configuring flags so that FW writes scripts.
07/29/11   Saul    IRAT update. Skip setting GPIOs during IRAT.
07/28/11   Saul    IRAT updates. 
                   - Now sending rx start config message to fw.
                   - Re-store AGC ACCUM
07/25/11   Saul    IRAT updates. 
                   - Now sending pilot_meas_cfg message.
                   - GRFC start delta sent in cx8 units.
                   - Override of debug flags done in FTM meas APIs.
07/23/11   Saul    Converted rx wake up APIs to use GRFC generate script.
07/18/11   aro     Updated Configure RxAGC interface to pass only required
                   param to simplify intelliceiver interface
07/15/11   Saul    IRAT updates.
07/14/11   Saul    IRAT updates.
07/08/11   Saul    Preliminary IRAT changes.
07/08/11   Saul    Preliminary IRAT changes.
01/08/11   aro     Changed @ingroup to @addtogroup
01/05/11   aro     Moved @defgroup to rf_cdma_documentation.h
12/13/10   aro     Added HDR Measurement APIs
12/07/10   aro     Initial Revision.

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rf_hdr_meas.h"
#include "rfcommon_msg.h"
#include "rfm_cdma.h"
#include "rf_cdma_intelliceiver.h"
#include "rf_cdma_msm.h"
#include "rfcommon_locks.h"
#include "rfc_cdma.h"
#include "rf_cdma_utils.h"
#include "rf_cdma_utils_freq.h"
#include "rfdevice_cdma_interface.h"
#include "rf_hdr_mdsp.h"
#include "rf_hdr_mc.h"
#include "rf_hdr_rx_agc.h"
#include "rf_cdma_data.h"
#include "modem_mem.h"
#include "rf_cdma_constants.h"
#include "rf_hal_buffer.h"
#include "rf_cdma_constants.h"
#include "rfcommon_time_profile.h"
#include "rf_cdma_mc.h"
#include "rf_cdma_fwrsp.h"
#include "rfm_internal.h"
#include "rfmeas_mdsp.h"
#include "rf_cdma_meas.h"
#include "rfcommon_nv_mm.h"

/* New Buffer Interface */
#include "rf_hal_buffer.h"

/*============================================================================*/
/*!
  @name Helper functions for HDR Meas

  @brief
  This section holds all helper functions used by HDR Meas
*/
/*! @{ */
      
/*! @} */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function for tuneback   

  @details
  Tuneback function for QTA 

  @param irat_meas_param
  Build Script Parameter

  @param meas_type
  meas type to pass in the script type in IRAT or non-IRAT scenarios
  
  @return
  Status of function execution 
*/
static rfm_meas_result_type
rf_hdr_meas_configure_for_tune_back
(
  rfm_meas_hdr_setup_and_build_scripts_param_type *irat_meas_param,
  const rfdevice_meas_scenario_type meas_type
)
{
  rfm_meas_result_type result_type = RFM_MEAS_COMMON_FAILURE ;
  rfm_device_enum_type device;
  uint32 buffer_index;
  lm_handle_type rxlm_handle;
 
  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rf_cdma_get_lock_data());

  /* Continue only if irat_meas_param is not NULL */
  if(irat_meas_param == NULL)
  {
    RF_MSG( RF_ERROR, "RF HDR IRAT - NULL ptr detected for meas params." );
  } /* if(irat_meas_param == NULL) */
  else
  {
    /*------------------------------------------------------------------------*/
    /* Init local vars */
    /*------------------------------------------------------------------------*/
    boolean irat_cdma_success = FALSE ; 
    device = irat_meas_param->device;

    buffer_index = irat_meas_param->buffer_index ;
    rxlm_handle = irat_meas_param->rxlm_buf_index ;

    RF_MSG_3 ( RF_HIGH  , "rf_hdr_meas_configure_for_tune_back(): " 
               "| Dev: %d | Buffer: %d | rxlm: %d |" ,
               device , buffer_index , rxlm_handle ) ;

    /* Call API to initialize IRAT specific part of CDMA data structure, this 
    API updates the buffer index, rxlm handle to the params sent in by L1 & 
    intelliceiver power mode to the lowest power mode*/ 
    irat_cdma_success = 
      rf_cdma_init_irat ( device , buffer_index , rxlm_handle ) ;

    if ( TRUE == irat_cdma_success )
    {
      if(RFM_CDMA_HEALTHY_STATE != rf_cdma_mc_tune_back( device, RFM_1XEVDO_MODE ))
      {
          irat_cdma_success = FALSE;
      }
    }/* if ( TRUE == irat_cdma_success ) */
    else
    {
      RF_MSG ( RF_ERROR ,"rf_hdr_meas_script_build_scripts(): "
                         "rf_cdma_init_irat() " "has failed " ) ;
    }/* if ( TRUE != irat_cdma_success ) */

    if ( TRUE == irat_cdma_success )
    {
      result_type = RFM_MEAS_COMMON_SUCCESS ;
    }
    else
    {
      RF_MSG( RF_ERROR, "rf_hdr_meas_script_build_scripts(): "
                  "rf_hdr_mdsp_start_rx_irat Failed  " ) ;
    }
  } /* if !(irat_meas_param == NULL) */

  RF_MSG_1( RF_HIGH, "rf_hdr_meas_script_build_scripts : Status [%d]",
            result_type );

  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rf_cdma_get_lock_data());

  return result_type;

}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Setup and Build HDR Rx wakeup Script

  @details
  This functions initializes and start to build iRAT script for HDR, by calling 
  MC layer APIs, and populating FW shared mem with script to Wakeup Rx
  Once done, L1 sends the pilot meas config command to FW to trigger Rx 
  measurements and collect Rx AGC samples.

  @param hdr_setup_and_build_scripts_param
  Build Script Parameter

  @param meas_type
  meas type to pass in the script type in IRAT or non-IRAT scenarios
  In IRAT scenarios, startup or cleanup script in No-IRAT mode always invalid. 
 
  @return
  Status of function execution 
*/
static rfm_meas_result_type
rf_hdr_meas_configure_for_rx_wakeup 
(
  rfm_meas_hdr_setup_and_build_scripts_param_type *irat_meas_param,
  const rfdevice_meas_scenario_type meas_type
)
{
  rfm_meas_result_type result_type = RFM_MEAS_COMMON_FAILURE ;

  rfm_hdr_agc_acq_type agc_mode; /* var to hold AGC acq mode */
  rfm_device_enum_type device;
  uint32 buffer_index;
  lm_handle_type rxlm_handle;

  rfm_cdma_band_class_type band;/*Band type*/

  uint16 curr_chans[RFM_CDMA_CARRIER_NUM] = { RF_CDMA_INVALID_CHAN ,
    RF_CDMA_INVALID_CHAN , RF_CDMA_INVALID_CHAN } ; /* Channels Array */
  int32 carrier_index[RFM_CDMA_CARRIER_NUM] = { 0 , 
    RF_CDMA_INVALID_CARRIER_INDEX , RF_CDMA_INVALID_CARRIER_INDEX } ; /* Single carrier */
 
  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rf_cdma_get_lock_data());

  RF_MSG( RF_HIGH, "rf_hdr_meas_script_build_scripts : START" );

  /* Continue only if irat_meas_param is not NULL */
  if(irat_meas_param == NULL)
  {
    RF_MSG( RF_ERROR, "RF HDR IRAT - NULL ptr detected for meas params." );
  } /* if(irat_meas_param == NULL) */
  else
  {
    /*------------------------------------------------------------------------*/
    /* Init local vars */
    /*------------------------------------------------------------------------*/
    boolean irat_cdma_success = FALSE ; 
    device = irat_meas_param->device;

    band = irat_meas_param->band_chan.band ;
    curr_chans[0] = irat_meas_param->band_chan.chan_num ;
    buffer_index = irat_meas_param->buffer_index ;
    rxlm_handle = irat_meas_param->rxlm_buf_index ;

    agc_mode.mode = irat_meas_param->agc_mode ;
    agc_mode.params.fast_acq.agc_rtc_offset = irat_meas_param->agc_rtc_offset;
    agc_mode.params.resume_acq.agc_rtc_offset = irat_meas_param->agc_rtc_offset;
    agc_mode.params.resume_acq.prev_agc = irat_meas_param->prev_agc;
    
    RF_MSG_3 ( RF_HIGH  , "rf_hdr_meas_script_build_scripts(): " 
               "| Dev: %d | Buffer: %d | rxlm: %d |" ,
               device , buffer_index , rxlm_handle ) ;

    /* Call API to initialize IRAT specific part of CDMA data structure, this 
    API updates the buffer index, rxlm handle to the params sent in by L1 & 
    intelliceiver power mode to the lowest power mode*/ 
    irat_cdma_success = 
      rf_cdma_init_irat ( device , buffer_index , rxlm_handle ) ;

    if ( TRUE == irat_cdma_success )
    {
      /* Prepare radio for wakeup. This API is intended to 
      update the CDMA data structure and state machines. In IRAT scenario
      this API must NOT touch the vregs */ 
      irat_cdma_success = 
        ( rf_hdr_mc_prep_wakeup_rx ( device, rxlm_handle , 
                                     1 ,  /* Irat measurements are SCDO */
                                     band , curr_chans , carrier_index , 
                                     &agc_mode, TRUE, /* IRAT mode*/ 
                                     meas_type      ) >= 0 ) ;
      if ( TRUE == irat_cdma_success )
      {
        /* Call exec_wakeup_rx, this will prepare the digital and analog front 
        end for Rx, in IRAT mode , skip device power vote, DLNA update,
        Skip sending of Rx Start mssg and cleanup of Rx start CCS events */ 
        irat_cdma_success = 
          ( rf_hdr_mc_exec_wakeup_rx ( device, rxlm_handle, 
                                       TRUE, /* IRAT mode */
                                       meas_type/*meas script type*/) >=  0) ;

        if ( TRUE != irat_cdma_success )
        {
          RF_MSG ( RF_ERROR , "rf_hdr_meas_script_build_scripts(): "
                              "rf_hdr_mc_exec_wakeup_rx() " "has failed " ) ;
        }
      }/* if ( TRUE == irat_cdma_success )*/
      else
      {
        RF_MSG ( RF_ERROR ,"rf_hdr_meas_script_build_scripts(): "
                           "rf_hdr_mc_prep_wakeup_rx() has failed " ) ; 
      }/* if ( TRUE != irat_cdma_success )*/
    }/* if ( TRUE == irat_cdma_success ) */
    else
    {
      RF_MSG ( RF_ERROR ,"rf_hdr_meas_script_build_scripts(): "
                         "rf_cdma_init_irat() " "has failed " ) ;
    }/* if ( TRUE != irat_cdma_success ) */

    if ( TRUE == irat_cdma_success )
    {
      result_type = RFM_MEAS_COMMON_SUCCESS ;
    }
    else
    {
      RF_MSG( RF_ERROR, "rf_hdr_meas_script_build_scripts(): "
                  "rf_hdr_mdsp_start_rx_irat Failed  " ) ;
    }
  } /* if !(irat_meas_param == NULL) */

  RF_MSG_1( RF_HIGH, "rf_hdr_meas_script_build_scripts : Status [%d]",
            result_type );

  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rf_cdma_get_lock_data());

  return result_type;
} /* rf_hdr_meas_script_build_scripts */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Cleanup HDR to prepapre DO_2_X measurement

  @details
  This function will be called to perform cleanup on HDR before any DO to X
  iRAT. Cleanup refers to the stopping of RX. This will move the RF State
  to RF_CDMA_STATE_SHUTDOWN_FOR_IRAT state.
 
  @param irat_meas_param
  Pointer to iRAT measurement data pertaining to DO
 
  @return
  Status of function execution
*/
rfm_meas_result_type
rf_hdr_stop_rx_for_irat_meas
(
  rfm_meas_hdr_enter_param_type* irat_meas_param
)
{
  rfm_meas_result_type result_type = RFM_MEAS_COMMON_SUCCESS;
  rf_cdma_data_status_type *dev_status_w; /* Device Status Write Pointer */

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rf_cdma_get_lock_data());

  /* NULL pointer check */
  if(irat_meas_param == NULL)
  {
    RF_MSG( RF_ERROR, "rf_hdr_cleanup_for_measurement - NULL ptr detected "
                      "for meas params." );
    result_type = RFM_MEAS_COMMON_FAILURE;
  } /* if(irat_meas_param == NULL) */
  else
  {
    /* Get Writable Device Status */
    dev_status_w = rf_cdma_get_mutable_device_status( irat_meas_param->device );
  
    /* Check for NULL pointers */
    if ( dev_status_w == NULL )
    {
      /* unable to get a valid device pointer - bail out */
      RF_MSG_1( RF_ERROR, "rf_hdr_cleanup_for_measurement - Unable to get a "
                "valid pointer for device %d", irat_meas_param->device );
      result_type = RFM_MEAS_COMMON_FAILURE;
    } /* if ( dev_status_w == NULL ) ) */
    else
    {
       boolean success = rf_cdma_init_irat ( irat_meas_param->device , 
                                            dev_status_w->script_index ,
                                            dev_status_w->rxlm_handle ) ;
      if ( TRUE == success )
      {
        if ( rf_hdr_mc_sleep_rx ( irat_meas_param->device , 
                             dev_status_w->rxlm_handle,
                             TRUE /* IRAT mode */ ) < 0 )
        {
          RF_MSG ( RF_ERROR, "rf_hdr_cleanup_for_measurement(): "
                   "Call to rf_hdr_mc_sleep_rx() failed " ) ; 
          result_type = RFM_MEAS_COMMON_FAILURE;
        }
      }
    }
  }

  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rf_cdma_get_lock_data());

  return result_type;

} /* rf_hdr_cleanup_for_measurement */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function will populate all the timing information related to IRAT
  measurements. This timing information will be used by L1 scheduling the 
  measurements during IRAT gap.
 
  @param rfm_meas_irat_info_type: data structure for all info to be returned
 
  @pre
  rfm_init() must have already been called.
  We should be in a certain technology.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did
  work correctly or not.
*/
rfm_meas_result_type 
rf_hdr_mc_irat_info_get ( rfm_meas_irat_info_type* rfm_meas_irat_info_param )
{
  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS;

  RF_MSG ( RF_MED , "rf_hdr_mc_irat_info_get: Start" ) ;
  
  if ( rfm_meas_irat_info_param != NULL ) 
  {
    /* Target Tech is HDR; Source Tech does not matter  */
    if ( rfm_meas_irat_info_param->header.source_tech == RFM_1XEVDO_MODE )
    {
      if ( ( (NUM_OF_MAX_SBI_AFTER_WRITES * SBI_WRITE_TIME_IN_NS)/1000 ) < 
           GENERIC_PLL_LOCK_TIME_IN_US )
      {
        rfm_meas_irat_info_param->startup_time = 
          (NUM_OF_MAX_SBI_BEFORE_WRITES * SBI_WRITE_TIME_IN_NS)/1000 + 
          GENERIC_PLL_LOCK_TIME_IN_US ;
      }
      else
      {
        rfm_meas_irat_info_param->startup_time = 
           ( (NUM_OF_MAX_SBI_BEFORE_WRITES + NUM_OF_MAX_SBI_AFTER_WRITES) *
              SBI_WRITE_TIME_IN_NS )/1000;
      }
    }

    /* Source Tech is 1X; Target Tech does not matter  */
    if ( rfm_meas_irat_info_param->header.source_tech == RFM_1XEVDO_MODE )
    {
      if ((NUM_OF_MAX_SBI_AFTER_WRITES*SBI_WRITE_TIME_IN_NS)/1000 < 
          GENERIC_PLL_LOCK_TIME_IN_US )
      {
        rfm_meas_irat_info_param->cleanup_time = 
          ((NUM_OF_MAX_SBI_BEFORE_WRITES * SBI_WRITE_TIME_IN_NS)/1000) + 
          GENERIC_PLL_LOCK_TIME_IN_US ;
      }
      else
      {
        rfm_meas_irat_info_param->cleanup_time = 
          ( (NUM_OF_MAX_SBI_BEFORE_WRITES + NUM_OF_MAX_SBI_AFTER_WRITES ) * 
            SBI_WRITE_TIME_IN_NS )/1000;
      }
    }
  }
  else
  {
    result = RFM_MEAS_COMMON_FAILURE ;
    RF_MSG ( RF_ERROR , "rf_hdr_mc_irat_info_get(): "
             "rfm_meas_irat_info_param is NULL " ) ;
  }
  return result ;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function is called first when the measurement is requested. Each
  technology basically prepares for the measurement.
 
  @param rfm_meas_enter_param : This is a pointer that basically has a header
  that has the target technology and the source technology. Based on this 
  header, the rest is casted as a tech specific pointer.
 
  @pre
  rfm_init() must have already been called.
  We should be in a certain technology.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function
  did work correctly or not.
*/
rfm_meas_result_type 
rf_hdr_mc_meas_enter 
( 
   rfm_meas_enter_param_type* rfm_meas_enter_param 
)
{
  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS;

   RF_MSG ( RF_MED , "rf_hdr_mc_meas_enter: Start" ) ;

  if ( rfm_meas_enter_param == NULL )
  {
    RF_MSG ( RF_ERROR , "rf_hdr_mc_meas_enter(): "
             "rfm_meas_enter_param is NULL ");
    result = RFM_MEAS_COMMON_FAILURE ;
  }
  else
  {  /* Common Steps for any target tech begin **** */
    /* Common Steps for any target tech end ****** */

    /* Source Tech = HDR ; Target Tech = {header.target_tech} */
    switch (rfm_meas_enter_param->header.target_tech)
    {
      /* DO --> LTE */
      /* Perform Clean-up on HDR for DO2L measurement */
      case RFCOM_LTE_MODE:
        result = rf_hdr_stop_rx_for_irat_meas (
           (rfm_meas_hdr_enter_param_type*) rfm_meas_enter_param->source_param ) ;
        if ( FALSE == result ) 
        {
          RF_MSG ( RF_ERROR , 
                 "rf_hdr_mc_meas_enter(): "
                 "rf_hdr_cleanup_for_measurement has returned FALSE ") ;
        }
        break ;

      case RFCOM_GSM_MODE:
        break ;

      /* Add case statements here for other target techs like W, G etc.. */
      default:
        break;
    }

    /* Target Tech = HDR ; Source Tech = {header.source_tech} */
    switch (rfm_meas_enter_param->header.source_tech)
    {
      /* LTE --> HDR */
      case RFCOM_LTE_MODE:
        RF_MSG ( RF_LOW ,  "rf_hdr_mc_meas_enter(): "
               "Nothing to do here when target tech ") ;
        break ;
      /* Add case statements here for other source techs like W, G etc.. */

      default:
        break;
    }
  }
  return result ;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function is called to prepare the clean-up and start-up scripts.
 
  @param rfm_meas_setup_param : This is a pointer that basically has a header
  that has the target technology and the source technology. Based on this 
  header, the rest is casted as a tech specific pointer.
 
  @pre
  rfm_meas_common_enter function must have been called.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function
  did work correctly or not.
*/
rfm_meas_result_type 
rf_hdr_mc_meas_build_scripts
( 
    rfm_meas_setup_param_type* rfm_meas_setup_param 
)
{
   rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS ;
   rfdevice_meas_scenario_type meas_type =  RFDEVICE_MEAS_STARTUP_SCRIPT ; 

  RF_MSG ( RF_MED , "rf_hdr_mc_meas_build_scripts: Start " ) ;
  if ( rfm_meas_setup_param == NULL )
  {
    RF_MSG ( RF_ERROR , "rf_hdr_mc_meas_build_scripts(): "
             "NULL pointer in rfm_meas_common_script_build_scripts");
    result = RFM_MEAS_COMMON_FAILURE ;
  }
  else 
  { /* Source Tech is HDR ; Target Tech is header.target_tech */
    switch (rfm_meas_setup_param->header.target_tech)
    {
      /* HDR --> LTE */
      case RFCOM_LTE_MODE:
        break;

      /* HDR --> GSM*/
      case RFCOM_GSM_MODE:
       /*HDR is source tech, so build Cleanup script for home buffer*/ 
        meas_type = RFDEVICE_MEAS_CLEANUP_SCRIPT ;
       /*For DO2G QTA, GL1 will call for Prebuild*/
        result = rf_hdr_meas_configure_for_tune_back (
           (rfm_meas_hdr_setup_and_build_scripts_param_type*) 
           (rfm_meas_setup_param->source_param),
           meas_type) ;
        if ( FALSE == result) 
        {
          RF_MSG ( RF_ERROR , "rf_hdr_mc_meas_build_scripts(): " 
                   "rf_hdr_meas_script_build_scripts returned FALSE" ) ;
        }
        break;
      default:
        break ;
    }

    /* Target Tech is HDR ; Source Tech is header.source_tech */
    switch ( rfm_meas_setup_param->header.source_tech)
    {
      /* LTE --> HDR */
      case RFCOM_LTE_MODE:
        /*HDR is target tech, so build startup script for away buffer*/ 
        meas_type =  RFDEVICE_MEAS_STARTUP_SCRIPT ;
        /* Setup the measurement for HDR. */
        result = rf_hdr_meas_configure_for_rx_wakeup (
           (rfm_meas_hdr_setup_and_build_scripts_param_type*) 
           (rfm_meas_setup_param->target_param), 
           meas_type) ;
        if ( FALSE == result) 
        {
          RF_MSG ( RF_ERROR , "rf_hdr_mc_meas_build_scripts(): " 
                   "rf_hdr_meas_script_build_scripts returned FALSE" ) ;
        }
        break;
      default:
        break ;
    } /* switch (rfm_meas_setup_param->header.source_tech) */
  } /* else if (rfm_meas_setup_param == NULL) */

  return result ;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function is called to exit the measurement mode. It is used to put RF
  in the right state and do any clean-ups required.
 
  @param rfm_meas_setup_param : This is a pointer that basically has a header
  that has the target technology and the source technology.
  Based on this header, the rest is casted as
  a tech specific pointer.
 
  @pre
  rfm_meas_common_script_enter function must have been called.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did
  work correctly or not.
*/
rfm_meas_result_type 
rf_hdr_mc_meas_exit ( rfm_meas_exit_param_type* rfm_meas_exit_param )
{
  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS;
  RF_MSG ( RF_MED , "rf_hdr_mc_meas_exit() START" ) ;

  if ( rfm_meas_exit_param == NULL )
  {
    RF_MSG ( RF_ERROR , "rf_hdr_mc_meas_exit(): "
             "NULL pointer in rfm_meas_setup_param ") ;
    result = RFM_MEAS_COMMON_FAILURE ;
  }
  else
  { 
    if (   (((rfm_meas_hdr_exit_param_type*) 
            (rfm_meas_exit_param->target_param)) != NULL) )
    {
      rfm_device_enum_type device = RFM_MAX_DEVICES;

      /* Source Tech is HDR  ; Target Tech = header.target_tech */
      switch (rfm_meas_exit_param->header.target_tech)
      {
        /* HDR --> LTE */
        case RFCOM_LTE_MODE:
        {
          /* HDR is source tech, get device from source tech param */
          device = ((rfm_meas_hdr_exit_param_type*) 
                                 (rfm_meas_exit_param->source_param))->device ;

          RF_MSG_1( RF_LOW, "rf_hdr_mc_meas_exit(): Updating state for device" 
                            " %d to RF_CDMA_STATE_SHUTDOWN_FOR_IRAT",device ) ;

          /* Set device state to SHUTDOWN_FOR_IRAT */
          rf_cdma_data_set_hdr_rf_state ( device , RF_CDMA_STATE_SHUTDOWN_FOR_IRAT ) ;
          break;
        }

        /* HDR --> G */
        case RFCOM_GSM_MODE:
        {
          /* HDR is source tech, get device from source tech param */
          device = ((rfm_meas_hdr_exit_param_type*) 
                                 (rfm_meas_exit_param->source_param))->device ;
          /* Handle measurement_gap_aborted for Do2G QTA case*/
          if ( TRUE == rfm_meas_exit_param->is_measurement_gap_aborted )
          { 
            RF_MSG_1 ( RF_MED , "rf_hdr_mc_meas_exit():Abort happened Abort: %d " ,
                         rfm_meas_exit_param->is_measurement_gap_aborted) ;
          }
          else  
          {
            if ( rf_cdma_data_get_hdr_rf_state(device) == RF_CDMA_STATE_RXTX )
            {
              rf_cdma_mc_set_tuner_mode_using_dummy_script(device);
            }
          }
          break;
        }


        default:
          break;
      }

      /* Target Tech is HDR ;  Source Tech header.source_tech */
      switch ( rfm_meas_exit_param->header.source_tech )
      {
    #ifdef FEATURE_LTE
        /* LTE to HDR */
        case RFCOM_LTE_MODE:
          if ( ((rfm_meas_lte_exit_param_type*) 
                  (rfm_meas_exit_param->source_param)) == NULL )    
          {
            RF_MSG( RF_ERROR, "rf_hdr_mc_meas_exit(): "  
                    "NULL ptr detected for LTE/HDR meas params." ) ;
            result = RFM_MEAS_COMMON_FAILURE;
          }
          else
          {
            boolean skip_full_sleep = TRUE ;
            /* HDR is target tech, get device from target tech param */
            device = ((rfm_meas_hdr_exit_param_type*) 
                                  (rfm_meas_exit_param->target_param))->device;

            /* If we were never woken up, say in optimized handoff case, then we 
            should avoid doing full sleep */
            if ( rf_cdma_data_get_hdr_rf_state ( device ) == RF_CDMA_STATE_RX ) 
            {
              skip_full_sleep = FALSE ;
            }

            RF_MSG_1( RF_LOW, "rf_hdr_mc_meas_exit(): Updating state for device" 
                              " %d to RF_CDMA_STATE_SHUTDOWN_FOR_IRAT",device );

            /* Set device state to SHUTDOWN_FOR_IRAT */
            rf_cdma_data_set_hdr_rf_state ( device , RF_CDMA_STATE_SHUTDOWN_FOR_IRAT ) ;

            /* Perform HDR Clean-up after L2DO measurement */
            result =   rf_cdma_meas_cleanup_ccs_events( device ) ;

            /* We dont want to sleep if we were never woken up meaning if 
            wakeup_rx was not called we shouldnt be calling sleep_rx. This can 
            happen in cases like:
            A> abort 
            B> optimized handover 
            For events like A and B, where only meas_enter and meas_exit are called, 
            all other intermediate IRAT calls are skipped in such cases JUST clean RF state
            OTHERWISE
            Go on ahead with the RF sleep otherwise  */
            if ( ( TRUE == rfm_meas_exit_param->is_measurement_gap_aborted ) ||
                 ( TRUE == skip_full_sleep ) )
            {               
                if ( TRUE == rfcommon_nv_get_lpm_feature_enabled() )
				{
				  rfm_cdma_band_class_type band;
                  band = ((rfm_meas_hdr_exit_param_type*)(rfm_meas_exit_param->target_param))->band_chan.
                       band; 


				  /* Power down WTR since gap was aborted */
				  rf_cdma_meas_power_down_wtr_for_abort( device , band );
                }

              /* In abort case reset SW state machine so we can do a 
              successful enterMode-wakeup or measurement*/
              rf_cdma_reset_sw_state ( device ) ;
              RF_MSG_2 ( RF_ERROR , "rf_hdr_mc_meas_exit(): "  
                       "wakeup failure or abort happened. Skipping sleep: "
                        "|| Abort: %d | SkipFullSleep: %d ||" ,
                         rfm_meas_exit_param->is_measurement_gap_aborted ,
                         skip_full_sleep ) ;
			  
              //Cleanup the TQP resources in case of a aborted GAP
              if ( TRUE == rfm_meas_exit_param->is_measurement_gap_aborted )
              {
                rf_cdma_mdsp_modem_rx_deallocate_resources(device, 
                                                           RFM_1XEVDO_MODE, 
                                                           ((rfm_meas_hdr_exit_param_type*)(rfm_meas_exit_param->target_param))->rxlm_buf_index);
              }
            }
            else
            {
              /* Sleep with IRAT_SHUTDOWN == TRUE so we dont send Rx Stop again  */
              rf_hdr_mc_sleep_rx ( device , 
                                 ((rfm_meas_hdr_exit_param_type*) 
                                  (rfm_meas_exit_param->target_param))->rxlm_buf_index ,
                                 TRUE /*in IRAT mode*/) ; 
            }
          }
          break;
    #endif /*FEATURE_LTE, needs to be present for Targets that do not have LTE*/
        default:
          break ;
      } /* switch ( rfm_meas_exit_param->header.source_tech ) */
      rf_cdma_deinit_irat  ( device ) ;
    } /* (rfm_meas_exit_param->target_param)) != NULL) */
    else
    {
      RF_MSG( RF_ERROR, 
            "rf_hdr_mc_meas_exit(): rfm_meas_exit_param->target_param is NULL." ) ;
      result = RFM_MEAS_COMMON_FAILURE;
    }
  } /* else if (rfm_meas_setup_param == NULL) */
  return result ;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function is called to prepare the clean-up scripts.
 
  @param rfm_meas_setup_param : This is a pointer that basically has a header
  that has the target technology and the source technology. Based on this 
  header, the rest is casted as a tech specific pointer.
 
  @pre
  rfm_meas_common_enter function must have been called.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function
  did work correctly or not.
*/
rfm_meas_result_type 
rf_hdr_mc_meas_build_reset_scripts /* rfmeas_build_reset_script_fp */
( 
  rfm_meas_setup_param_type *rfm_meas_setup_param
)
{
  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS ;
  /* Pointer to script buffer*/
  rf_buffer_intf *cleanup_script = NULL;
  rfm_meas_hdr_setup_and_build_scripts_param_type *reset_param_ptr = NULL;
  rfm_meas_program_script_type script_type = RFM_MEAS_PROGRAM_STARTUP;
  rfm_device_enum_type device;
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */
  ant_tuner_device_tokens_type* ant_tuner_token_w; /*Tuner token write pointer*/
  rfm_cdma_band_class_type band;
  boolean status = TRUE;
  RF_MSG ( RF_MED , "rf_hdr_mc_meas_build_reset_scripts: Start " ) ;

  if (rfm_meas_setup_param == NULL)
  {
    RF_MSG ( RF_ERROR , "rf_hdr_mc_meas_build_reset_scripts: NULL Param Pointer " );
    return (RFM_MEAS_COMMON_FAILURE);
  }

  if((rfm_meas_setup_param->header.source_tech != RFM_1XEVDO_MODE)&&
    (rfm_meas_setup_param->header.target_tech == RFM_1XEVDO_MODE))
  {
    /* 1x is target tech, get device from target tech param */
    device = ((rfm_meas_hdr_exit_param_type*) 
                            (rfm_meas_setup_param->target_param))->device;

    /* Get Logical Device Param */
    logical_dev = rfc_cdma_get_logical_device_params( device );

    /* Get Device Status Read Pointer*/
    dev_status_r = rf_cdma_get_device_status( device );

    ant_tuner_token_w = rf_cdma_get_mutable_ant_tuner_token( device );

    /* Perform NULL Pointer check */
    if ( ( dev_status_r == NULL ) || ( logical_dev == NULL ) || 
         ( ant_tuner_token_w == NULL ) )
    {
      RF_MSG_1( RF_ERROR, "rf_hdr_mc_meas_build_reset_scripts: NULL data for device %d",
                device );
      return RFM_MEAS_COMMON_FAILURE;
    }
    /* check for valid band */
    band = dev_status_r->curr_band;

    reset_param_ptr = (rfm_meas_hdr_setup_and_build_scripts_param_type *)rfm_meas_setup_param->target_param;
    script_type = RFM_MEAS_PROGRAM_CLEANUP;
    cleanup_script = rfmeas_mdsp_buffer_get(script_type);
  }
	
  if ((reset_param_ptr!=NULL))
  {
    if (script_type == RFM_MEAS_PROGRAM_CLEANUP)
    {
      #ifdef FEATURE_RF_HAS_QTUNER
      #ifdef FEATURE_RF_ASDIV
      /* Lock the ASDiv manager interface to avoid concurrency of ASDiv and Tuner configuration*/
      rfcommon_asdiv_manager_enter_lock();
      #endif
      status &= rf_cdma_atuner_disable(logical_dev->ant_tuner[band],
				       RFDEVICE_CREATE_SCRIPT,
				       cleanup_script,
				       0, /* No timing offset */
				       ant_tuner_token_w);
      #ifdef FEATURE_RF_ASDIV
      /* Releae the ASDIV manager interface */
      rfcommon_asdiv_manager_release_lock();
      #endif
      #endif
    }
  }
	  
  if (status == TRUE)
  {
    return (result);
  }
  else
  {
    RF_MSG ( RF_ERROR , "rf_hdr_mc_meas_build_reset_scripts: status is FALSE " );
    return (RFM_MEAS_COMMON_FAILURE);
  }  
} /* rf_hdr_mc_meas_build_reset_scripts */


/*----------------------------------------------------------------------------*/

static rfmeas_mc_func_tbl_type rf_hdr_mc_meas_apis =
{
  rf_hdr_mc_irat_info_get , /* rfmeas_mc_irat_info_get_fp*/
  rf_hdr_mc_meas_enter , /* rfmeas_mc_enter_fp */
  NULL,                  /* rfmeas_mc_pre_config_fp */
  rf_hdr_mc_meas_build_reset_scripts, /* rfmeas_build_reset_scripts_fp */
  rf_hdr_mc_meas_build_scripts , /* rfmeas_build_scripts_fp */
  rf_hdr_mc_meas_exit /* rfmeas_mc_exit_fp */
} ;

/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function registers the IRAT APIs for HDR with meas module.
 
  @param None

  @retval None
*/
boolean 
rf_hdr_mc_meas_init()
{
  return ( rfmeas_mc_register(RFCOM_1XEVDO_MODE, &rf_hdr_mc_meas_apis)) ;
}
#endif /* FEATURE_CDMA1X */


/*! @} */


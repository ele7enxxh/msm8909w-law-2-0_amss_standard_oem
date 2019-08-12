/*!
  @file
  ftm_1x_control.c

  @brief
  This module contains 1x common control code for FTM.

  @addtogroup RF_FTM_1X_CONTROL
  @{
*/

/*==============================================================================

  Copyright (c) 2011 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/ftm/src/ftm_1x_control.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/02/15   vc      Toggling Frame Valid flag while doing retune to avoid RFCAVFS HO failure.
01/24/15   cd      Init Tx Freq Error to zero during FTM wakeup Tx
11/19/14   spa     Added FTM concurrecny manager check,done in single enable API
10/23/14   ck      Fix off-target error
10/22/14   ck      Temporarily unhooking ftm concurrency manager due to incomplete standalone test
10/22/14   ck      adapt the hooking to ftm conmgr due to interface change
10/16/14   jmf     [selftest] Implement multi-gain stage switch for ACLR improvement
09/22/14   spa     KW warning fixes
08/20/14   ndb     Add Tuner_ID as part of the response in "FTM_SET_TUNER_TUNE_CODE_OVERRIDE"
08/18/14   jmf     [SelfTest] ACLR meas changes
07/25/14   sty     off-target fix
04/23/14   cd      Support for 8 PA state functionality
04/10/14   JJ      Pass fw_smem_ptr to enable modulator api
04/07/13   sty     KW fix to fix potential null pointer access
03/28/14   APU     Hook up Tx logging interfaces in SW driver to log data 
                   provided by RFLM.
02/11/14   pk/zhw  Fix for Toggling Tx fails after enabling Diversity
12/17/13   JJ      Fixed an error that rfm_ret is not boolean type
12/06/13   JJ      Add check for enable modulator
11/26/13   cc      To fix IQ capture issue when turning on TX and then enabling Div
11/25/13   hdz     Added fws_app_enable(FW_APP_RFCMD)
11/20/13   sar     Fixed compiler warnings.
11/18/13   cd      Conditionally hook up RFLM-TxAGC override
11/11/13   JJ      Get smem addr and save in ftm_cdma_data
09/23/13   sty     compiler warning fixes
09/06/13   aka     memcpy cleanup
08/27/13   aka     Added support for 1x self-test commands 
06/28/13   JJ      Add API for antenna tuner to update tune code in ftm mode
06/20/13   JJ      Changed func ftm_1x_deregister_fw_msgs()
06/19/13   JJ      Add functions for register/de-register fw msgs 
05/20/13   spa     Updated set_second_chain_mode API to not force mode to BC0 in
                   OFS mode 
05/14/13   cc      Added support for FTM 1x IQ capture
05/06/13   spa     Use #def from QPOET driver for min Tx power in TxAGC override
04/24/13   sty     Removed redundant calls to enc_tx_enable()
04/23/13   zhw     Add support for FTM_1X_SET_PA_STATE in cal mode
04/18/13   spa/fh  Added SSBI retune ftm support
03/28/13   zhw     Clean up obsolete pa_range and apt_val FW interface
03/22/13   sty     Fixed warnings, code cleanup
03/14/13   aro     Deprecated Composite Calibration and TxSweep Commands
03/19/13   aro     Deleted ET config path
02/20/13   zhw     Remove 5ms delay as FW has stopped sending RF OFF at start
                   Config traffic state before FTM wakeup Tx
02/05/13   cd      Implicit xPT configuration is valid for enable tx as it 
                   does not affect APT cal right now
02/05/13   cd      Remove implicit FTM xPT configuration during wakeup Tx
02/04/13   bmg     Added ftm_1x_set_tuner_tune_code_override()
01/31/13   cd      Added FEATURE_RF_HAS_XPT_SUPPORT for Triton
01/22/13   zhw     Added 5ms delay between tx_start and modulator enable
01/21/13   nrk     Updated aggreated multi synth lock status check functions
12/27/12   cd      Added action types for IQ gain and Env scale overrides
12/19/12   cd      Program ET Tx configuration only in FTM mode
12/19/12   Saul    CDMA. TX AGC XPT Dev. Use xpt cfg msg in online.
12/18/12   cd      Add support to override delay value to FW
12/18/12   sty     backed out KW fixes
12/17/12   sty     KW fixes
12/04/12   zhw     APT PA Q Current support (interface cleanup)
11/28/12    nrk    Added APIs to support MULTI SYNTH STATE command and also making sure that aggregate
                   value is updated wheneven there is a PLL retune.
11/19/12   zhw     Cal mode interface change support for APT in Dime
11/09/12   zhw     Compiler warning fix
11/09/12   cd      Fixed compiler warning
11/08/12   cd      Fixed message macro
11/08/12   cd      - Changed legacy override to FW override
                   - Added xPT Tx AGC override
                   - Added functionality to configure ET path
09/28/12   aro     Added device to 1x FWS config interface
09/05/12   aro     Added RF-FW Sync callback parameter in MDSP Interface
08/29/12   ars/hdz Update FTM_SET_FREQ_ADJUST command to use new TCXOMGR API for blocking therm read     
08/22/12   aro     Renamed interface names and type definitions (time prof)
08/22/12   aro     Changes to migrate to new precide time profiler
08/17/12   aro     Sequence change to perform DAC Stop followed by TX Stop
08/17/12   spa     Fixed incorrect usage of RFM_MAX_DEVICES 
08/16/12   ars/hdz Modifed FTM frequency adjust logic to apply correction using XO manager API
08/08/12   cvd     Replace timetick with rfcommon_time_profile interface
08/06/12   hdz     Modified ftm_1x_set_tx_pwr_limit() to access rf_cdma_data
07/24/12   hdz     Added ftm_1x_set_tx_pwr_limit()
07/23/12   Saul    CDMA. Removed unnecessary header.
06/26/12   spa     Fixed diversity bug in ftm_1x_tear_down() 
06/07/12   spa     Fixed ftm_1x_tear_down to use RFM 1x teardown API 
06/04/12   spa     Reverted ftm_1x_tear_down to old method temporarily 
05/31/12   spa     Updated ftm_1x_tear_down to use RFM 1x teardown API 
05/16/12   vb      Added F3 msg when DAC Cal is starting 
05/16/12   vb      Do DAC cal for a device path only if a tech is enabled for it 
04/25/12   adk     Do 1x DAC cal for a device only if C2K is enabled for it.
05/03/12   zhaow   Fix off-target error
04/26/12   hdz     Change seqence in ftm_1x_disable_tx 
04/25/12   adk     In DAC cal, first set TX AGC in override mode, then enable TX.
                   Do 1x DAC cal only if C2K band BC0 is enabled in BC config.
04/24/12   APU     Added profiling for ftm_1x_configure_fw_state(), 
                   ftm_1x_set_mode(), ftm_1x_set_second_chain_mode() and 
                   ftm_1x_configure_mcpm_state(). 
04/20/12   adk     Use IREF_6 cal codes for IREF_5 mission mode.
04/03/12   aro     Updated FWS config function to perform SemWait in MC
04/03/12   aro     Updated FWS config function to manage state transition in
                   FTM
04/02/12   aro     Added CB as argument in Rx / Tx MDSP interfaces 
04/01/12   vb      For 1x DAC cal, doing all the buffer memory allocation only
                   once for both DAC-0 and DAC-1 Cal  
03/27/12   vb      Initialize to PA state 0 during DAC cal 
03/22/12   vb      Increased wait time after overriding TxAGC word and after Iref is programmed
                   to 1000us
03/26/12   vb      Added support for new RF device function to program DAC Iref  
03/22/12   vb      Added 500us wait time after overriding TxAGC word and after Iref is programmed
03/23/12   adk     Updated DAC cal to ensure IREF values are not overwritten by FW
03/22/12   aro     Mainlined new Dispatch for RF_TASK
03/22/12   aro     Deleted unused types/functions
03/22/12   aro     Added data structure to hold FW responses data with
                   an accessor function
03/22/12   aro     Migrated 1x FW response handling to RF_FWRSP task
03/22/12   aro     Deleted old RF_TASK based dispatching of CDMA
                   firmware responses
03/21/12   spa     Added max device check in rf_cdma_get_mutable_device_status()
03/13/12   aro     Added support to enable new RF_TASK dispatching. Code
                   is protected with debug flag
03/09/12   spa     Changed ftm_cdma_data_update_band_chan arguments to cdma band     
                   and chan type
03/08/12   hdz     Added ftm_1x_set_freq_adjust
03/06/12   aro     FTM 1x mode exit for the case when set mode is called with
                   mode ID as FTM_PHONE_MODE_SLEEP
03/05/12   adk     Undid change made just below.
02/22/12   adk     Updated DAC cal to ensure IREF values are not overwritten by FW
01/27/12   adk     Updated DAC cal to be able to calibrate without power cycling
01/17/12   aro     Enabled FW and MCPM voting for calibration mode as well
01/17/12   aro     Added "New Mode" as a argument in FTM EXIT function
01/03/12   aro     Documentatation Update for DAC Cal
01/03/12   aro     Converted F3 message to new macros
12/29/11   adk     Query RF card to find out which DAC cals are needed
12/28/11   adk     Implemented DAC_1 calibration in the 1x mode
12/22/11   adk     Implemented DAC_0 calibration in the 1x mode
12/07/11   aro     Fixed the sequencing issue where SET PDM overrides the PA
                   Bias to 0x7FFF clearing the last set PA Bias 
11/23/11   ems     Correct 1X exit transition for two devices
11/21/11   aro     Added fixes to enable DRx Composite Calibration
11/21/11   aro     Updated ftm_1x_disable_tx to return GOOD response even for
                   the case when this command is sent when Tx is OFF. This
                   is required to fix bad Tools sequence
11/18/11   aro     Added code not to do FW and MCPM voting during calibration
                   mode to allow power cycle less calibration between techs
11/17/11   aro     Enabled voting for CDMA FW State config and MCPM config
11/17/11   aro     Enabled FTM callback functions for RFM APIs
11/17/11   aro     Enabled voting for FW state config
11/17/11   aro     Added new MCPM configure function with voting mechanism
11/17/11   aro     Added RFM callback function for FTM
11/16/11   aro     Added function to vote and unvote FW disabe
11/16/11   aro     Added centralized infrastruture to manage FW state change
11/11/11   aro     Added check not to handle FW State change response for
                   nonFTM mode
11/10/11   aro     Upadated RF-MDSP interfaces to pass semaphore to 
                   wait on for blocking calls
11/10/11   aro     Defined CDMA semaphore as generic CDMA type
10/31/11   aro     Added handler function to handle FW state config response
10/11/11   sty     Added more functionality in ftm_1x_set_second_chain_mode
09/20/11   aro     Added new function to perform clear PA Range
                   override action
09/09/11   shb     Allow up to 128 HDET reads via ftm_1x_get_hdet_tx_sweep() as
                   it is not restricted by diag packet size
09/01/11   cd      Add APT support
                   - Removed PA Range functionality, replace with PA state
                   - Added interface to program APT bias val in FTM mode
08/31/11   cd      Disable 1x modulator during ftm_1x_tear_down() after sleep Tx
                   is done
08/11/11   aro     Added support to second chain test call
08/08/11   aro     Fixed 1x Tear Down function not to use Dev0 LM buffer of Dev1
08/03/11   aro     Added support for radio tear down for cal v3
07/30/11   aro     F3 Message Update
07/27/11   aro     [1] Moved Enable/Disable combiner interrupts to FTM 1x
                   Enter and Exit
                   [2] Updated radio tear down function to be based on RF state
                   on RF driver side
                   [3] Deleted XO calibration interface
07/25/11   aro     Added support to perform radio retune in RxTx state
07/13/11   sty     Added null pointer checks
07/07/11   sty     Added band info in arg list when API 
                   rfm_1x_ftm_set_tx_agc_override is invoked 
06/21/11   aro     Moved ftm_sys_control functionality to ftm_common_control
06/16/11   sar     Including internal 1x interface.
06/14/11   aro     Renamed TxAGC Override function + Signature to use PA State
06/13/11   aro     Added Internal command to set Lowest Tx Power
06/08/11   aro     Changed disable 1x modulator func signature to have device
06/07/11   aro     Changed 1x modulator function signature to have device
06/06/11   aro     Removed PA Range Inversion as Hardware does not invert it
                   anymore
06/06/11   sty     [1] Added fixes in ftm_1x_set_channel for retune case
06/06/11   sty     [1] Added sleep Primary in ftm_1x_set_channel()
                   [2] revert state to sleep instead of Invalid - since invalid
                   results in a deadlock condition
06/02/11   sty     Compiler warning fix
06/02/11   sty     Moved utility APIs to beginning of file
                   Added ftm_1x_resolve_mode() and ftm_1x_get_valid_tx_device()
                   Fixed implementation in ftm_1x_set_channel()
06/01/11   aro     Moved the CDMA Radio tear down to Set Mode
05/27/11   aro     Compiler Warning fix
05/27/11   aro     [1] Moved SV debug flag to main dispatch
                   [2] Moved Tx Modulator Enabled flag to FTM HAL
05/27/11   aro     Added SV Chain Calibration temp support
05/26/11   aro     Compiler Warning Fix
05/26/11   aro     [1] Fixed device check for Tx interfaces to support SV
                   [2] Added debug flag for device switch in SV mode
05/25/11   sty     Enable combiner interrupts after prep_wakeup
05/24/11   aro     Added workaround not to enable 1x Modulator more than 
                   once for Tx Calibration stabilty 
05/19/11   aro     Added function to configure XO Cal Mode for 1x
05/17/11   aro     Changed the order to configure Modulator after Tx Wakeup
05/13/11   aro     Added interface to configure combiner interrupts
05/05/11   aro     Replaced wakeup rx interface with Prep and execute function
04/22/11   sar     Removed deprecated file srch_mdsp.h.
04/28/11   sar     CMI-4 Cleanup.
04/27/11   sty     [1] Changed behavior in ftm_1x_set_channel() to use 
                   chain_config
                   [2] Changed behavior in ftm_1x_set_sec_chain_mode()
                   [3] Added checks in ftm_1x_disable_tx()
                   [4] Updated state vars in ftm_1x_tear_down()
04/25/11   aro     Renamed FTM RFMODE to FTM State
04/21/11   aro     Changed rfm_enter_mode() call to support RxLM Buffer
04/18/11   sty     Deleted unused ftm_1x_get_all_hdet_tx_sweep()
04/14/11   sty     Added OFS implementation in ftm_1x_set_sec_chain_mode() 
04/11/11   sty     Renamed ftm_secondary_chain_control_enum_type to 
                   ftm_sec_chain_ctl_enum_type
                   Check for failure condition in ftm_1x_enable_diversity()
04/07/11   aro     Updated argument type for TxLinearizer
03/30/11   aro     Added Profiling for Set Channel function
03/29/11   sty     New API ftm_1x_disable_tx() to disable Tx
03/29/11   sty     Removed bad check for diversity device
03/29/11   sty     Fixed warning
03/29/11   sty     Addded ftm_1x_set_diversity
03/28/11   aro     Added Disable Modulator
03/23/11   aro     Compiler Warning Fix
03/24/11   aro     Renamed FTM Enter Mode function
03/22/11   sty     Deleted APIs that enabled and disabled clks - this is taken 
                   care of by MCPM
                   Deleted redundant call to ftm_1x_set_txagc_src_to_fw() - the 
                   concerned register is already setup in TxLM during enable_tx
03/22/11   aro     Compiler Warning fix
03/22/11   aro     [1] Added Fix for Calibration crash fix by removing clock
                   disable in disable tx funciton
                   [2] Added RFM validity check to return bad/good responses
03/21/11   sty     Disable Tx clks and modulator on 1x sleep
03/16/11   sty     Added checks in ftm_1x_enable_tx() 
03/16/11   aro     Cleaned up Override Interface + added device argument
03/14/11   aro     Deleted FW enable and disable function to move to RF
03/07/11   aro     Updated ftm_1x_set_pa_range() interface
03/04/11   aro     Changed type for Set PA Range interface
03/03/11   aro     Removed device parameter for Tx Cal Data to reuse same
                   structure for multiple devices
03/02/11   aro     Renamed FTM and FTM-NS mode enums
03/01/11   aro     Moved  FTM 1x enter and exit to system control file
02/25/11   aro     Compiler Warning Fix
02/24/11   sty     Changed behavior of ftm_1x_get_hdet_tx_sweep()
                   Use FTM_NUM_HDET_TO_RETURN instead of 
                   FTM_CDMA_HDET_ARRAY_SIZE for HDET array size
02/23/11   sty     Fixed warnings
02/22/11   sty     Deleted FTM_1X_HDET_ARRAY_SIZE. Used FTM_CDMA_HDET_ARRAY_SIZE
                   instead
                   Renamed rfm_pa_r1_r0_type to rfm_pa_range_type
                   Update ftm_cdma_data with the overridden LUT value as part of 
                   ftm_1x_set_tx_avga
02/18/11   sty     Call ftm_1x_set_freq_offset() directly from 
                   ftm_1x_configure_tx_waveform()
02/18/11   sty     Program offset tone in ftm_1x_configure_tx_waveform
02/17/11   aro     Removed RFM Device Check to replace with RFM Mode check
02/17/11   sty     Use rfm_pa_r1_r0_type instead of rfm_pa_gain_state_type
02/17/11   sty     Added ftm_1x_set_pa_state
                   Used ftm_cdma_data_get_tx_sweep_ptr instead of 
                   ftm_cdma_data_get_tx_sweep_adr
02/17/11   sty     Fixed debug messages
02/16/11   sty     Fixed minor bug in ftm_1x_set_pa_range
02/16/11   sty     [1] Fixed return value check of RFM API in ftm_1x_enable_tx
                   [2] Implemented ftm_1x_set_tx_waveform_source, 
                       ftm_1x_set_pa_range, ftm_1x_set_tx_waveform_source                    
                   [3] Renamed ftm_1x_configure_tx_waveform to 
                       ftm_1x_set_tx_waveform
                   [4] Added ftm_1x_set_tx_avga
02/16/11   aro     Added placeholder for MCPM Call
02/16/11   aro     Removed Power Mode Param from Measure LNA/DVGA interfaces
02/16/11   aro     Moved around Doxygen group
02/16/11   aro     [1] Renamed 1x Tear down interface
                   [2] Added 1x Tear down in 1x exit
02/16/11   aro     [1] Added support to use RXLM/TXLM buffer for RFCal
                   [2] Added FW disable function
02/16/11   aro     Doxygen Warning removal
02/15/11   aro     [1] Added 1x enter and exit
                   [2] Removed FTM init env function
02/15/11   sty     Invoke rfm_1x_wakeup_tx instead of rfm_1x_ftm_enable_tx 
                   Changed rfm_ret behavior in ftm_1x_enable_tx
02/14/11   aro     [1] Updated Tear down function to cover RFM failure case
                   [2] Cleaned up Dispatch
02/14/11   sty     [1] Mimic L1 call sequence in ftm_1x_enable_tx
                   [2] Added return value for ftm_1x_init_ftm_environment
02/11/11   sty     Updated RF state on completion of ftm_1x_set_tx
02/10/11   sty     Implemented ftm_1x_set_tx(). 
                   Added TxLM buffers as arg in rfm APIs where applicable
02/09/11   aro     Added HDET query functions
02/09/11   aro     [1] Added Set Tx Interface
                   [2] Added Configure Tx Waveform interface
02/02/11   aro     Renamed Invalid Chan Enum
02/02/11   aro     [1] Added feature to return BAD response pkt for failed RFM
                   [2] F3 Message Update
02/02/11   aro     Added Mode Validity Check in Set Chan
02/02/11   aro     Implemented the requirement not to execute Set Chan 
                   on the device which is in RXDIV State. 
01/31/11   aro     Added support for Set Chan / Minor Fixes in Set Mode
01/31/11   aro     [1] Updated RF Tear Down function
                   [2] Minor fixes in Set Mode
01/15/11   aro     [1] Added FTM Set Mode function
                   [2] Added FTM 1x utility functions
01/12/11   aro     Initial Revision

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#ifdef FEATURE_FACTORY_TESTMODE
#include "ftm_common_dispatch.h"
#include "ftm_1x_control.h"
#include "ftm_cdma_data.h"
#include "ftm_task.h"
#include "ftm_msg.h"
#include "rfm_common.h"
#include "rfm_internal.h"
#include "rfm_1x.h"
#include "rfm_1x_ftm.h"
#include "rfm_cdma_ftm.h"
#include "rf_cdma_data.h"
#include "ftm_1x_msm.h"
#include "lm_types.h"
#include "rxlm_intf.h"
#include "txlm_intf.h"
#include "rex.h"
#include "rfcommon_time_profile.h"
#include "ftm_v.h"
#include "srch_rx_div.h"
#include "rf_cdma_tx_agc.h"
#include "rf_1x_mdsp.h"
#include "rfcommon_mdsp.h"
#include "rfcommon_msm_cal.h"
#include "rfm_cdma_band_types.h"
#include "rf_cdma_utils_freq.h"
#include "rfcommon_nv_mm.h"
#include "rf_test_task_v.h"
#include "fws.h"
#include "tcxomgr.h"
#include "rfcommon_core.h"
#include "rfcommon_data.h"
#include "rfm.h"
#include "enc_v.h"
#include "ftm_cdma_msm.h"
#include "rfm_ftm.h"
#include "ftm_rf_cmd.h"
/* This is included to access QPOET #def for min bias voltage */
#include "rfdevice_papm_intf.h"
#include "ftm_common_iq_processing.h"
#include "ftm_common_control.h"
//#include "ftm_common_xpt.h"
/* memscpy */
#include "stringl.h"
#include "ftm_1x_selftest.h"
#include "ftm_common_selftest.h"

#include "ftm_cdma_control.h"


static ftm_rf_fbrx_meas_tx_params_rsp_pkt_type ftm_rf_fbrx_meas_tx_params_rsp_pkt;
/* Variable used as Response packet for FTM_RF_MEAS_TX_PARAMS command */

static ftm_rf_fbrx_iq_capture_trigger_rsp_pkt_type ftm_rf_fbrx_iq_capture_trigger_rsp_pkt;
/* Variable used as Response packet for FTM_FBRX_IQ_CAPTURE_TRIGGER command */

static ftm_rf_fbrx_iq_capture_get_samples_rsp_pkt_type ftm_rf_fbrx_iq_capture_get_samples_rsp_pkt; 
/* Variable used as Response packet for FTM_FBRX_IQ_CAPTURE_GET_SAMPLES command */

/* Compiler warning fix */
extern tcxomgr_ppm_type tcxomgr_ft_curr_blocking( void );

/*============================================================================*/
/*!
  @name Utility Functions

  @brief
  Utility Functions for FTM usage
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Resolve Bandclass for given FTM mode

  @details
  When RF mode is requested to be set, PC will send the mode enum which
  does not directly correspond to a band class. To make SVDO RF calibration
  compatible to older routines, this API will be used by to resolve the band
  class for the given mode,

  DISCLAIMER : This API is used to facilitate the legacy RF Cal API. This API
  should not be called for other purpose. 

  @param mode
  FTM mode sent from the PC side

  @return
  Resolved Band class for given FTM Mode. The resturned structure will also
  hold default channel.
*/
sys_band_class_e_type
ftm_1x_resolve_bandclass
(
  ftm_mode_id_type mode
)
{
  sys_band_class_e_type band = SYS_BAND_BC_MAX;

  /* Assign Band Class based on mode requested and override the band info
  on band_chan structure */
  switch (mode)
  {
  case FTM_PHONE_MODE_CDMA:
  case FTM_PHONE_MODE_CDMA_800:
    band = SYS_BAND_BC0; /* Band Class 0 */
    break;
  case FTM_PHONE_MODE_CDMA_1900:
    band = SYS_BAND_BC1; /* Band Class 1 */
    break;
  case FTM_PHONE_MODE_JCDMA:
    band = SYS_BAND_BC3; /* Band Class 3 */
    break;
  case FTM_PHONE_MODE_CDMA_1800:
    band = SYS_BAND_BC4; /* Band Class 4 */
    break;
  case FTM_PHONE_MODE_CDMA_450:
    band = SYS_BAND_BC5; /* Band Class 5 */
    break;
  case FTM_PHONE_MODE_IMT:
    band = SYS_BAND_BC6; /* Band Class 6 */
    break;
  case FTM_PHONE_MODE_CDMA_BC10:
    band = SYS_BAND_BC10; /* Band Class 10 */
    break;
  case FTM_PHONE_MODE_CDMA_BC11:
    band = SYS_BAND_BC11; /* Band Class 11 */
    break;
  case FTM_PHONE_MODE_CDMA_BC14:
    band = SYS_BAND_BC14; /* Band Class 14 */
    break;
  case FTM_PHONE_MODE_CDMA_BC15:
    band = SYS_BAND_BC15; /* Band Class 15 */
    break;
  default:
    FTM_MSG_1( FTM_FATAL, "Resolve BC : Invalid FTM Mode Switch %d", mode);   
    break;
  }

  return band;
} /* ftm_1x_resolve_bandclass */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Resolve mode for given band

  @details
  When RF mode is requested to be set, PC will send the mode enum which
  does not directly correspond to a band class. To make SVDO RF calibration
  compatible to older routines, this API will be used by to resolve the band
  class for the given mode,

  @param band
  FTM mode sent from the PC side

  @return
  Resolved mode for given band
*/
ftm_mode_id_type
ftm_1x_resolve_mode
(
   sys_band_class_e_type band
)
{
  ftm_mode_id_type mode;

  /* Assign Band Class based on mode requested and override the band info
  on band_chan structure */
  switch (band)
  {
  case SYS_BAND_BC0:
    /* do not return FTM_PHONE_MODE_CDMA (even though it is associated with
       BC0) since FTM_PHONE_MODE_CDMA is deprecated*/
    mode = FTM_PHONE_MODE_CDMA_800; 
    break;
  case SYS_BAND_BC1:
    mode = FTM_PHONE_MODE_CDMA_1900; 
    break;
  case SYS_BAND_BC3:
    mode = FTM_PHONE_MODE_JCDMA; 
    break;
  case SYS_BAND_BC4:
    mode = FTM_PHONE_MODE_CDMA_1800; 
    break;
  case SYS_BAND_BC5:
    mode = FTM_PHONE_MODE_CDMA_450; 
    break;
  case SYS_BAND_BC6:
    mode = FTM_PHONE_MODE_IMT; 
    break;
  case SYS_BAND_BC10:
    mode = FTM_PHONE_MODE_CDMA_BC10; 
    break;
  case SYS_BAND_BC11:
    mode = FTM_PHONE_MODE_CDMA_BC11; 
    break;
  case SYS_BAND_BC14:
    mode = FTM_PHONE_MODE_CDMA_BC14; 
    break;
  case SYS_BAND_BC15:
    mode = FTM_PHONE_MODE_CDMA_BC15; 
    break;
  default:
    mode = FTM_PHONE_MODE_MAX;
    FTM_MSG_1( FTM_FATAL, "ftm_1x_resolve_mode: Invalid band %d", band );
    break;
  }

  return mode;
} /* ftm_1x_resolve_mode */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns a valid Tx device 

  @details
  Check if device has a valid Tx path.
  If the TXLM handle for the device is invalid, it could be possible that 
  this API has been called for a device that does not have a valid Tx path - 
  however, the API tries to find an  associated device that has a valid path

  Step1: See if there is an associated device
  Step2: Check if the associated device has a valid TxLM buffer
  Step3: perform all TX operations on the associated device
 
  @param device
  for which a valid associated Tx device is to be found

  @return
  TRUE on success and false on failure
*/
rfm_device_enum_type
ftm_1x_get_valid_tx_device
(
  rfm_device_enum_type device
)
{
  rfm_device_enum_type device_to_return; 
  ftm_cdma_data_status_type dev_state; /* hold current state of device */
  ftm_cdma_data_status_type assoc_dev_state;/*holds state of associated device*/
  rfm_device_enum_type assoc_dev; /* hold ID of associated device */

  ftm_cdma_data_get_device_status(device, &dev_state);

  /* --------------- Step1 --------------- */
  if (dev_state.txlm_handle == FTM_INVALID_LM_BUFFER)
  {
    FTM_MSG_1( FTM_HIGH, "ftm_1x_get_valid_tx_device: API called for "
               "device %d that has invalid TxLM buffer", device );

    device_to_return = RFM_INVALID_DEVICE;

    /* --------------- Step2 --------------- */
    if (dev_state.assoc_dev != RFM_INVALID_DEVICE)
    {
      FTM_MSG_1( FTM_HIGH, "ftm_1x_get_valid_tx_device: Found an associated "
                 "device = %d ", dev_state.assoc_dev );
      /* found an associated device - now check if the associated device has
         a valid Txlm buffer */
      assoc_dev = dev_state.assoc_dev;
      ftm_cdma_data_get_device_status(assoc_dev, &assoc_dev_state);
      
      /* --------------- Step3 --------------- */
      if(assoc_dev_state.txlm_handle != FTM_INVALID_LM_BUFFER)
      {
        FTM_MSG_2( FTM_HIGH, "ftm_1x_get_valid_tx_device: Found an associated "
                   "device = %d with valid TxLM buffer = %d. Using this device "
                   "for all Tx operations", assoc_dev, 
                   assoc_dev_state.txlm_handle);

        /* use the associated device for all Tx operations */
        device_to_return = assoc_dev;
      }
    }

  } /* if (dev_state.txlm_handle == FTM_INVALID_LM_BUFFER) */
  else
  {
    /* the current device is a valid Tx device - so just return the same device
    */
    device_to_return = device;
  }

  return device_to_return;

} /* ftm_1x_get_valid_tx_device */

/*! \} */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns  mutlsynth status on given device.

  @details
  This API is used to get aggregate multisynth lock value. It also does a 
  latest query before returning the value.

  @param device
  Device for which synth lock status is requested
  
  @param rx_synth_lock
  Stores Rx synth lock staus

  @param tx_synth_lock
  Stores Tx synth lock staus

  @return
  Status of execution completion of the function execution
*/
ftm_cdma_rsp_type 
ftm_1x_get_multisynth_status
(
  rfm_device_enum_type device,
  ftm_multi_synth_lock_status_enum_type *rx_synth_lock,
  ftm_multi_synth_lock_status_enum_type *tx_synth_lock 
)
{
  boolean result = FALSE;
  
  /* Update latest multi synth lock status */  
  ftm_1x_update_aggreated_multisynth_status(device);

  /* Get multi synth lock status */
  result = ftm_1x_get_aggregated_multi_synth_lock_status( device,
                                                          rx_synth_lock,
                                                          tx_synth_lock );

  if(result)
  {
    FTM_MSG_3( FTM_LOW, "1x multisynth status: device=%d, rx_synth_lock=%d,"
                     " tx_synth_lock=%d",device,*rx_synth_lock,*tx_synth_lock);
    return FTM_CDMA_GOOD_RSP;
  }
  else
  {
    FTM_MSG_1( FTM_HIGH, "ftm_1x_get_multisynth_status() failed on device=%d",device);  
    return FTM_CDMA_BAD_RSP;
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Updates mutlsynth status on given device..

  @details
  This API is used to update aggregated mutlsynth status by quering the latest 
  SYNTH lock status and latching on to UNLOCKED status if  unlock it detected 
  in any of the queries.

  @param device
  Device for which synth lock status update is requested

  @return
  None
*/
void 
ftm_1x_update_aggreated_multisynth_status
(
  rfm_device_enum_type device
)
{
  ftm_multi_synth_lock_status_enum_type rx_synth_lock = FTM_SYNTH_STATUS_INVALID;
  ftm_multi_synth_lock_status_enum_type tx_synth_lock = FTM_SYNTH_STATUS_INVALID;
  boolean result = FALSE;

  /* Get latest multi synth lock status */
  result = ftm_1x_get_latest_multisynth_status( device, 
                                                &rx_synth_lock, 
                                                &tx_synth_lock );
  if(result)
  {
    /* Update latest multi synth lock status */
    ftm_1x_update_latest_aggregated_multisynth_status( device,
                                                       rx_synth_lock,
                                                       tx_synth_lock ); 
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns  latest Mutlsynth status .

  @details
  This API is used to get latest Mutlsynth status by quering RFM layer which 
  inturn will query device layer.


  @param device
  Device for which synth lock status is requested

  @param rx_synth_lock
  Stores Rx synth lock staus

  @param tx_synth_lock
  Stores Tx synth lock staus

  @return
  None
*/
boolean 
ftm_1x_get_latest_multisynth_status
(
  rfm_device_enum_type device,
  ftm_multi_synth_lock_status_enum_type *rx_synth_lock,
  ftm_multi_synth_lock_status_enum_type *tx_synth_lock 
)
{		
  boolean cur_rx_synth_status = FALSE;
  boolean cur_tx_synth_status = FALSE;	
  ftm_cdma_data_status_type dev_state; /* hold current state of device */ 
  boolean result = TRUE;

  dev_state.rf_state = FTM_CDMA_STATE_INVALID;
  result = ftm_cdma_data_get_device_status(device, &dev_state);

  if ( result == FALSE )
  {
    FTM_MSG_1(FTM_ERROR,"ftm_1x_get_latest_multisynth_status: Invalid FTM "
                        "device pointer for device %d", device);
    return FALSE;
  }

  if((device == RFM_DEVICE_0)|| (device == RFM_DEVICE_2))
  {
    switch(dev_state.rf_state)
    {
      case FTM_CDMA_STATE_SLEEP:
        *rx_synth_lock = FTM_SYNTH_STATUS_DISABLED;
        *tx_synth_lock = FTM_SYNTH_STATUS_DISABLED;
        break;  
	   
      case FTM_CDMA_STATE_RX: 		   
	    	/* Get Rx synth status */
        rfm_get_synth_lock_status( device, 
                                   RFM_1X_MODE,
                                   RF_SYNTH_RX,
                                   &cur_rx_synth_status );
		
        *rx_synth_lock = (cur_rx_synth_status == TRUE) ? \
			                                         FTM_SYNTH_STATUS_LOCKED :\
			                                         FTM_SYNTH_STATUS_UNLOCKED;
        *tx_synth_lock = FTM_SYNTH_STATUS_DISABLED;
        break; 
			 
      case FTM_CDMA_STATE_RXTX:  
	    	/* Get Rx synth status */
        rfm_get_synth_lock_status( device,
                                   RFM_1X_MODE,
                                   RF_SYNTH_RX,
                                   &cur_rx_synth_status );
		
        *rx_synth_lock = (cur_rx_synth_status == TRUE) ? 
			                                         FTM_SYNTH_STATUS_LOCKED :\
			                                         FTM_SYNTH_STATUS_UNLOCKED;
 	      /* Get Tx synth status */
        rfm_get_synth_lock_status( device,
                                   RFM_1X_MODE,
                                   RF_SYNTH_TX,
                                   &cur_tx_synth_status );
	 
        *tx_synth_lock = (cur_tx_synth_status == TRUE) ? 
			                                         FTM_SYNTH_STATUS_LOCKED :\
			                                         FTM_SYNTH_STATUS_UNLOCKED;		   
        break;
	   
      default:		 
	      FTM_MSG_2( FTM_ERROR, "ftm_1x_get_latest_multisynth_status: "
                              "Unexpected RF state=%d on device=%d ", 
                              dev_state.rf_state,device );
	      result = FALSE;
        break;
    } /* switch(dev_state.rf_state) */
  }
  else if((device == RFM_DEVICE_1) || (device == RFM_DEVICE_3))
  {
    *tx_synth_lock = FTM_SYNTH_STATUS_NOT_APPLICABLE;
		 
    switch(dev_state.rf_state)
    {
      case FTM_CDMA_STATE_SLEEP:
        *rx_synth_lock = FTM_SYNTH_STATUS_DISABLED;
        break;  
		 
      case FTM_CDMA_STATE_RX:			  
	  	/* Get Rx synth status */
        rfm_get_synth_lock_status( device,
                                   RFM_1X_MODE,
                                   RF_SYNTH_RX,
                                   &cur_rx_synth_status );
		
        *rx_synth_lock = (cur_rx_synth_status == TRUE) ? \
			                                         FTM_SYNTH_STATUS_LOCKED :\
			                                         FTM_SYNTH_STATUS_UNLOCKED;
        break; 
			
      default: 		
        FTM_MSG_2( FTM_ERROR, "ftm_1x_get_latest_multisynth_status: Unexpected" 
                   "RF state=%d on device=%d", dev_state.rf_state,device );
		    result = FALSE;
    } /* switch(dev_state.rf_state) */
  }
  else
  {
    *tx_synth_lock = FTM_SYNTH_STATUS_NOT_APPLICABLE;
    *rx_synth_lock = FTM_SYNTH_STATUS_NOT_APPLICABLE;
	  FTM_MSG_1(FTM_ERROR,"ftm_1x_get_latest_multisynth_status: "
                        "Unsupported device %d",device); 
    result = FALSE;
  }

   return result;

} /* ftm_1x_get_latest_multisynth_status */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Reset mutlsynth status on given device.

  @details
  This API is used to reset multisynth lock status. This is the only way to 
  get the multisynth status out of UNLOCK status

  @param device
  Device for which synth lock status reset is requested

  @return
  Status of execution completion of the function execution
*/
ftm_cdma_rsp_type
ftm_1x_reset_multisynth_status
(
  rfm_device_enum_type device
)
{
  boolean result = FALSE;
  
  result = ftm_1x_reset_aggregated_multisynth_lock_status(device);
  
  if(result)
  {
    FTM_MSG_1( FTM_HIGH, "1x reset_multisynth_status on device=%d",device);
    return FTM_CDMA_GOOD_RSP;
  }
  else
  {
    return FTM_CDMA_BAD_RSP;
  }	  	
}

/*============================================================================*/
/*!
  @name Firmware State Control

  @brief
  Functions for the FTM to control FW state
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Vote FW State Change
 
  @details
  This funciton will update the vote mask wit the vote for current device
 
  @param device
  RF Device for which FW Disable vote is to be done.
*/
void
ftm_1x_vote_fw_state
(
  rfm_device_enum_type device
)
{
  ftm_cdma_fw_state_config_data_type *fw_data;

  /* Get FW Data */
  fw_data = ftm_cdma_get_fw_state_config_data();

  /* NULL pointer check */
  if ( fw_data == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_vote_fw_state: NULL FW data" );
    return;
  } /* if ( fw_data == NULL ) */

  /* Update the Vote mask only if the Calibration mode is disabled */
  fw_data->onex_fw.fw_state_vote_mask |= (1 << device);

  FTM_MSG_2( FTM_HIGH, "ftm_1x_vote_fw_state : New Vote 0x%x with Dev %d",
             fw_data->onex_fw.fw_state_vote_mask, device );

  return;

} /* ftm_1x_vote_fw_state */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Unvote FW State Change
 
  @details
  This funciton will update the vote mask wit the vote for current device,
 
  @param device
  RF Device for which FW Disable vote is to be done.
*/
void
ftm_1x_unvote_fw_state
(
  rfm_device_enum_type device
)
{
  ftm_cdma_fw_state_config_data_type *fw_data;
  uint32 new_vote_mask = 0;

  /* Get FW Data */
  fw_data = ftm_cdma_get_fw_state_config_data();

  /* NULL pointer check */
  if ( fw_data == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_unvote_fw_state: NULL FW data" );
    return;
  } /* if ( fw_data == NULL ) */

  /* Update the Vote mask only if the Calibration mode is disabled */
  new_vote_mask |= (1 << device );
  new_vote_mask = ~new_vote_mask;

  /* Update the Vote Mask */
  fw_data->onex_fw.fw_state_vote_mask &= new_vote_mask;

  FTM_MSG_2( FTM_HIGH, "ftm_1x_unvote_fw_state : New Vote 0x%x with Dev %d",
             fw_data->onex_fw.fw_state_vote_mask, device );

  return;

} /* ftm_1x_unvote_fw_state */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure 1x firmware state
 
  @details
  This function will configure the firmware state for FTM mode usage. However
  if the new FW state is RFM_CDMA_FW_STATE_DISABLE, then before proceeding
  with FW state change, current vote is checked.
 
  @param fw_state
  New firmware state to which FW transition is to be done.
 
  @return
  Flag indicating if the FW state transition was successful or not.
*/
boolean
ftm_1x_configure_fw_state
(
  rfm_device_enum_type device,
  rfm_cdma_fw_state_type fw_state
)
{
  ftm_cdma_fw_state_config_data_type *fw_data;
  boolean proceed_fw_state_chg = TRUE;
  boolean ret_val;
  rfm_cdma_fw_state_type current_fw_state; /* Current FW state */
  rf_time_type profiler_var_val;

  /* Get starting time tick to profile the function */
  rf_time_tick_type profiler_var = rf_time_get_tick() ; 

  /* Get FW Data */
  fw_data = ftm_cdma_get_fw_state_config_data();

  /* NULL pointer check */
  if ( fw_data == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_configure_fw_state: NULL FW data" );
    return FALSE;
  } /* if ( fw_data == NULL ) */

  /* To allow FW state transition to DISABLE, the fw_disbale_vote_mask should
  be 0. Otherwise FW disable cannot allowed. */
  if ( ( ( fw_state == RFM_CDMA_FW_STATE_DISABLE ) &&
         ( fw_data->onex_fw.fw_state_vote_mask != 0 ) ) ||
       ( fw_state == fw_data->onex_fw.fw_state ) )
  {
    proceed_fw_state_chg = FALSE;
  } /* if ( ( ( fw_state == RFM_CDMA_FW_STATE_DISABLE ) &&
         ( fw_data->onex_fw.fw_state_vote_mask != 0 ) ) ||
       ( fw_state == fw_data->onex_fw.fw_state ) ) */

  current_fw_state = fw_data->onex_fw.fw_state;

  /* Perform FW state change if allowed */
  if ( proceed_fw_state_chg == TRUE )
  {
    void* smem_addr = NULL;   /* shared memory addr from FW */
    ret_val = TRUE; /* Start with TRUE return value */

    /* Switch based on new requested FW Satte */
    switch( fw_state )
    {
      case RFM_CDMA_FW_STATE_ACTIVE:
      case RFM_CDMA_FW_STATE_MEAS:
      {
        /* Switch based on current FW State */
        switch ( current_fw_state )
        { /* switch ( current_fw_state ) */
          case RFM_CDMA_FW_STATE_DISABLE:
            fws_app_enable(FW_APP_RFCMD);
            fws_app_enable(FW_APP_CDMA1X);
            break;
          default:
            break;
        } /* switch ( current_fw_state ) */
        smem_addr = rfm_1x_ftm_config_fw_state( device, fw_state );
        if (smem_addr == NULL) 
        {
          ret_val = FALSE;
        }
      } /* switch( fw_state ) : case RFM_CDMA_FW_STATE_ACTIVE */
      break;
      
      case RFM_CDMA_FW_STATE_DISABLE:
      {
         smem_addr = rfm_1x_ftm_config_fw_state( device, fw_state );
         if (smem_addr == NULL) 
         {
           ret_val = FALSE;
         }
         else
         {
           fws_app_disable(FW_APP_RFCMD);
           fws_app_disable(FW_APP_CDMA1X);
         }
      } /* switch( fw_state ) : case RFM_CDMA_FW_STATE_DISABLE */
      break;
      
      default:
        FTM_MSG( FTM_ERROR, "ftm_1x_configure_fw_state: Requested fw state "
                            "not supported" );
      break;
    } /* switch( fw_state ) */

    /* Update the FW state */
    fw_data->onex_fw.fw_state = fw_state;
    if (ret_val == TRUE)
    {
      fw_data->onex_fw.smem_addr = smem_addr;
    }
    else
    {
      fw_data->onex_fw.smem_addr = NULL;
      FTM_MSG( FTM_ERROR, "ftm_1x_configure_fw_state: rfm_1x_ftm_config_fw_state() "
                          "failed and no valid shared memory addr is updated" );
    }
	
  } /* if ( proceed_fw_state_chg == TRUE ) */
  else
  {
    FTM_MSG_3( FTM_HIGH, "ftm_1x_configure_fw_state: Vote 0x%x - Skipped FW "
               "state change [%d --> %d]", fw_data->onex_fw.fw_state_vote_mask,
               current_fw_state, fw_state );
    ret_val = FALSE;
  } /* if ! ( proceed_fw_state_chg == TRUE ) */

  /* Get execution time for this function  */
  profiler_var_val = rf_time_get_elapsed ( profiler_var , RF_USEC ) ; 
  FTM_MSG_4 ( FTM_HIGH, "ftm_1x_configure_fw_state: New State [%d --> %d], "
             "Vote 0x%x, Execution Time: %d",current_fw_state, fw_state, 
             fw_data->onex_fw.fw_state_vote_mask , profiler_var_val );

  return ret_val;

} /* ftm_1x_configure_fw_state */

/*! \} */

/*============================================================================*/
/*!
  @name MCPM State Control

  @brief
  Functions for the FTM to control MCPM
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Vote MCPM
 
  @details
  This funciton will update the MCPM vote mask with the added vote for current 
  device
 
  @param device
  RF Device for which MCPM vote is to be done.
*/
void
ftm_1x_vote_mcpm
(
  rfm_device_enum_type device
)
{
  ftm_cdma_mcpm_config_data_type *mcpm_data;

  /* Get FW Data */
  mcpm_data = ftm_cdma_get_mcpm_state_ptr();

  /* NULL pointer check */
  if ( mcpm_data == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_vote_mcpm: NULL MCPM data" );
    return;
  } /* if ( fw_data == NULL ) */

  /* Update the Vote mask only if the Calibration mode is disabled */
  mcpm_data->onex_mcpm.mcpm_vote_mask |= (1 << device);

  FTM_MSG_2( FTM_HIGH, "ftm_1x_vote_mcpm : New Vote 0x%x with Dev %d",
             mcpm_data->onex_mcpm.mcpm_vote_mask, device );

  return;

} /* ftm_1x_vote_mcpm */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Unvote MCPM
 
  @details
  This funciton will update the MCPM vote mask with the removed vote for current 
  device
 
  @param device
  RF Device for which MCPM vote is to be done.
*/
void
ftm_1x_unvote_mcpm
(
  rfm_device_enum_type device
)
{
  ftm_cdma_mcpm_config_data_type *mcpm_data;
  uint32 new_vote_mask = 0;

  /* Get FW Data */
  mcpm_data = ftm_cdma_get_mcpm_state_ptr();

  /* NULL pointer check */
  if ( mcpm_data == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_unvote_mcpm: NULL MCPM data" );
    return;
  } /* if ( fw_data == NULL ) */

  /* Update the Vote mask only if the Calibration mode is disabled */
  new_vote_mask |= (1 << device );
  new_vote_mask = ~new_vote_mask;

  /* Update the Vote Mask */
  mcpm_data->onex_mcpm.mcpm_vote_mask &= new_vote_mask;

  FTM_MSG_2( FTM_HIGH, "ftm_1x_unvote_mcpm : New Vote 0x%x with Dev %d",
             mcpm_data->onex_mcpm.mcpm_vote_mask, device );

  return;

} /* ftm_1x_unvote_mcpm */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure 1x MCPM state
 
  @details
  This function will configure the MCPM state for FTM mode usage. However
  if the new FW state is DISABLE, then before proceeding
  with MCPM state change, current vote is checked.
 
  @param enable_mcpm
  New MCPM state to which MCPM transition is to be done.
 
  @return
  Flag indicating if the MCPM state transition was successful or not.
*/
boolean
ftm_1x_configure_mcpm_state
(
  boolean enable_mcpm
)
{
  ftm_cdma_mcpm_config_data_type *mcpm_data;
  boolean proceed_mcpm_state_chg = TRUE;
  boolean ret_val;
  rf_time_type profiler_var_val;

  /* Get starting time tick to profile the function */
  rf_time_tick_type profiler_var = rf_time_get_tick() ; 

  /* Get FW Data */
  mcpm_data = ftm_cdma_get_mcpm_state_ptr();

  /* NULL pointer check */
  if ( mcpm_data == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_configure_fw_state: NULL MCPM data" );
    return FALSE;
  }

  /* To allow MCPM state transition to DISABLE, the mcpm_vote_mask should
  be 0. Otherwise FW disable cannot allowed. */
  if ( ( enable_mcpm == FALSE ) &&
       ( mcpm_data->onex_mcpm.mcpm_vote_mask != 0 ) )
  {
    proceed_mcpm_state_chg = FALSE;
  }

  /* Perform FW state change if allowed */
  if ( proceed_mcpm_state_chg == TRUE )
  {
    if ( enable_mcpm == TRUE )
    {
      if ( mcpm_data->onex_mcpm.mcpm_configured == FALSE )
      {
        mcpm_data->onex_mcpm.mcpm_configured = TRUE;
        ftm_1x_enable_mcpm();

        /* Enable Combiner Interrupts ( this is done by L1 in online mode)
        This operation needs to be done before any message is sent to FW,
        since FW depends on the combiner interrupts to be running */
        ftm_1x_configure_combiner_interrupts( TRUE );
      }
      else
      {
        FTM_MSG( FTM_HIGH, "ftm_1x_configure_mcpm_state: Already enabled" );
      }
    }
    else
    {
      if ( mcpm_data->onex_mcpm.mcpm_configured == TRUE )
      {
        mcpm_data->onex_mcpm.mcpm_configured = FALSE;

        /* Disable Combiner Interrupts */
        ftm_1x_configure_combiner_interrupts( FALSE );

        ftm_1x_disable_mcpm();
      }
      else
      {
        FTM_MSG( FTM_HIGH, "ftm_1x_configure_mcpm_state: Already disabled" );
      }
    }

    ret_val = TRUE;
  } /* if ( proceed_fw_state_chg == TRUE ) */
  else
  {
    FTM_MSG_1( FTM_HIGH, "ftm_1x_configure_mcpm_state: Vote 0x%x - Skipped "
               "MCPM state change", mcpm_data->onex_mcpm.mcpm_vote_mask );
    ret_val = FALSE;
  }

  /* Get execution time for this function  */
  profiler_var_val = rf_time_get_elapsed ( profiler_var , RF_USEC ) ; 
  FTM_MSG_3 ( FTM_HIGH, "ftm_1x_configure_mcpm_state: New State %d, Vote 0x%x, Execution Time: %d",
             mcpm_data->onex_mcpm.mcpm_configured,
             mcpm_data->onex_mcpm.mcpm_vote_mask , 
             profiler_var_val );
  
  return ret_val;

} /* ftm_1x_configure_mcpm_state */

/*! \} */

/*============================================================================*/
/*!
  @name RFM Callback functions

  @brief
  Functions pertaining to RFM callback
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Callback function to be executed when RFM functions are called by FTM
 
  @details
  This function will be passed as a argument to the RFM APIs which will be
  executed at the end of RFM API execution.
 
  @param rfm_event
  RFM event for which this callback is called
 
  @param data
  Pointer to the echoed data received
*/
void
ftm_1x_rfm_callback
(
  rfm_cb_event_enum_type rfm_event,
  void *cb_data
)
{
  /* Handle cases based on type of RFM Event */
  switch( rfm_event )
  {
  case RFM_1X_RX_WAKEUP_COMPLETE:
  case RFM_1X_RX_SLEEP_COMPLETE:
  case RFM_TIME_DELAY_EXPIRED:
  case RFM_1X_ENABLE_COMPLETE:
  case RFM_1X_DISABLE_COMPLETE:
  case RFM_1X_RX_WAKEUP_PREP_COMPLETE:
  case RFM_1X_RX_TIMED_SLEEP_COMPLETE:
  case RFM_1X_TX_WAKEUP_COMPLETE:
  case RFM_1X_TX_SLEEP_COMPLETE:
  case RFM_1X_TUNE_COMPLETE:
  case RFM_1X_ENABLE_DIV_COMPLETE:
  case RFM_1X_DISABLE_DIV_COMPLETE:
  default:
    break;
  }

  FTM_MSG_1( FTM_MED, "ftm_1x_rfm_callback : Event %d Handled", rfm_event );

} /* ftm_1x_rfm_callback */

/*! \} */

/*============================================================================*/
/*!
  @name Rx Radio Control

  @brief
  Functions for the FTM to control the radio
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set 1x Mode

  @details
  This function will set specific Mode of operation for RF SW. This will enter
  and enable 1x Mode of Operation. This function will not perform any action
  on Hardware itself.
 
  To supports existing calibration procedure, this function will store the
  band class information sent in FTM Data structure. RF will be tuned to this
  band class when channel information is later passed using FTM_SET_CHAN
  command.
 
  Associated FTM Command : <b>FTM_SET_MODE</b>

  @param device RF device whose mode is to be set.
  @param mode FTM mode sent from the PC side

  @return
  Status of execution completion of the function execution
*/
ftm_cdma_rsp_type
ftm_1x_set_mode
(
  rfm_device_enum_type device,
  ftm_mode_id_type mode
)
{

  boolean init_ftm_env; /* has FTM env for 1x been initialized? */
  sys_channel_type band_chan; /* Band Class to which Mode is mapped */
  ftm_cdma_data_status_type dev_state; /* Var to hold current state */
  boolean rfm_ret = TRUE; /* Return val from RFM */
  boolean ftm_exit_status = FALSE; /* Status for FTM exit */
  ftm_cdma_rsp_type ret_val = FTM_CDMA_GOOD_RSP; /* Return Value */
  ftm_1x_rfm_callback_data_type cb_payload;
  rf_time_type profiler_var_val;

  /* Get starting time tick to profile the function */
  rf_time_tick_type profiler_var = rf_time_get_tick() ; 

  /* Must be in FTM_MODE */
  if(ftm_mode != FTM_MODE)
  {
    FTM_MSG_1( FTM_ERROR, "ftm_1x_set_mode: Not in FTM Mode - %d", ftm_mode );
    return FTM_CDMA_BAD_RSP; /* Failure */
  }

  /* Parameter Validation */
  if ( device >= RFM_MAX_DEVICES )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_1x_set_mode: Invalid Device - %d", device );
    return FTM_CDMA_BAD_RSP; /* Failure */
  }

  /* Check if the mode. For SLEEP mode, put the 1x radio on the given device   
  to sleep.*/
  if ( mode == FTM_PHONE_MODE_SLEEP )
  {
    /* Perform 1x Exit */
    if ( ftm_get_current_state(device) == FTM_STATE_1X )
    {
      ftm_exit_status = ftm_rfmode_exit( device, FTM_STATE_PARK );
    }

    if ( ftm_exit_status == FALSE )
    {
      ret_val = FTM_CDMA_BAD_RSP;
      FTM_MSG_1( FTM_ERROR, "ftm_1x_set_mode: FTM exit on dev %d failed",
                 device );
    } /* if ( ftm_exit_status = FALSE ) */
    else
    {
      FTM_MSG_1( FTM_LOW, "ftm_1x_set_mode: Sleep for 1x Dev %d", device );
    }

  } /* if ( mode == FTM_PHONE_MODE_SLEEP ) */
  else
  {
    /* Set up FTM environment for 1x */
    init_ftm_env = ftm_rfmode_enter( device, FTM_STATE_1X );

    /* ensure that FTM env for 1x has been set up */
    if (init_ftm_env == FALSE)
    {
      FTM_MSG_1( FTM_ERROR, "ftm_1x_set_mode: Failed FTM init for %d", device );
      return FTM_CDMA_BAD_RSP; /* Failure */
    }

    /* Resolve Band Class and Channel for given mode */
    band_chan.band = ftm_1x_resolve_bandclass( mode );

    /* Populate Invalid Channel as Channel is unknown at this stage. Channel
    will be populated during FTM_SET_CHAN */
    band_chan.chan_num = FTM_CDMA_INVALID_CHAN;

    /* Update FTM Data structure with band and chan */
    ftm_cdma_data_update_band_chan(device, 
                            rf_cdma_sys_band_type_to_rf_band_type(band_chan.band),
                                    (rfm_cdma_chan_type )band_chan.chan_num);

    (void)ftm_cdma_data_get_device_status( device, &dev_state );

    /* Tear Down RF to SLEEP State */  
    if ( !ftm_1x_tear_down(device) )
    {
        /* Enter 1x Mode */
        rfm_ret &= ( rfm_enter_mode( device, RFM_1X_MODE, NULL, NULL,
                                     dev_state.rxlm_handle) == 0 );

        cb_payload.device = device; /* Populate the CB Payload */

        rfm_ret &=  ftm_cdma_control_enable_tech( RFM_1X_MODE,
                                               device, 
                                               band_chan,
                                               dev_state.rxlm_handle, 
                                               &ftm_1x_rfm_callback, 
                                               &cb_payload );
      
    }/* if ( !ftm_1x_tear_down(device) ) */

    /* Since RFM failed, flag return value as BAD RESPONSE */
    if ( rfm_ret == FALSE )
    {
      ret_val = FTM_CDMA_BAD_RSP;
      ftm_cdma_data_update_rf_state ( device, FTM_CDMA_STATE_SLEEP );
    } /* if ( rfm_ret == FALSE ) */
    else
    {
      ret_val = FTM_CDMA_GOOD_RSP;
        
      /* Update FTM Data Structure */
      ftm_cdma_data_update_rf_state( device, FTM_CDMA_STATE_SLEEP );
    } /* if ! ( rfm_ret == FALSE ) */
  } /* if ! ( mode == FTM_PHONE_MODE_SLEEP ) */

  /* Get execution time for this function  */
  profiler_var_val = rf_time_get_elapsed ( profiler_var , RF_USEC ) ; 
  FTM_MSG_3( FTM_LOW, "ftm_1x_set_mode: Done for 1x Dev %d, Mode %d, "
             "Execution TIme: %d", device, mode , profiler_var_val ) ;

  return ret_val;

} /* ftm_1x_set_mode */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Tune to Band and Channel

  @details
  This function tunes the receiver to a particular band and channel. It 
  will Check if the phone is in either on RX, RXTX, or DIV state. If not, then
  no tuning is done. Set Mode is a pre-requisite for 
  successful execution of this function.
 
  If the current RF Mode is RX state, then this function will reprogram RF
  Chain to retune to new band and channel. If the current RF mode is TX state,
  then the API should change band/channel without altering the device state.
  If this API is called for the primary device which is operating in diversity 
  mode, then the band and channel shall also be changed for both
 
  Associated FTM Command : <b>FTM_SET_CHAN</b>

  @param device RF device on which the given channel is to be tuned
  @param rf_channel RF channel to be tuned to

  @return
  Status of execution completion of the function execution
*/
ftm_cdma_rsp_type
ftm_1x_set_channel
(
  rfm_device_enum_type device,
  sys_channel_num_type rf_channel
)
{
  ftm_cdma_data_status_type dev_state; /*holds current state of device*/
  ftm_cdma_data_status_type m_dev_state;/*holds current state of master device*/
  ftm_cdma_data_status_type s_dev_state; /* holds state of slave device */  
  rfm_device_enum_type m_device;  /* signifies the master-device */
  sys_channel_type band_chan; /* Band and Channel */
  ftm_cdma_rsp_type ret_val = FTM_CDMA_GOOD_RSP; /* Return Value */
  rfm_mode_enum_type curr_mode; /* Var to store current mode */
  boolean rfm_ret = TRUE; /* Return val from RFM */
  ftm_cdma_state_type new_rf_state; /* Variable to store New RF State */
  ftm_1x_rfm_callback_data_type cb_payload;
  rf_time_tick_type prof_cal_func; /* Function Profiling Variable */
  rf_time_type prof_cal_func_val;

  prof_cal_func = rf_time_get_tick(); /* Get Time tick for Function Profile */

  /* Must be in FTM_MODE */
  if(ftm_mode != FTM_MODE)
  {
    FTM_MSG_1( FTM_ERROR, "Set Chan : Not in FTM Mode - %d", ftm_mode );
    return FTM_CDMA_BAD_RSP; /* Failure */
  }

  /* Parameter Validation */
  if ( device > RFM_MAX_DEVICES )
  {
    FTM_MSG_1( FTM_ERROR, "Set Chan : Invalid Device - %d", device );
    return FTM_CDMA_BAD_RSP; /* Failure */
  }

  /* Get Current Mode */
  curr_mode = rfm_get_current_mode(device);

  /* Check for Mode Validity */
  if ( curr_mode != RFM_1X_MODE )
  {
    FTM_MSG_2( FTM_ERROR, "Set Chan : Invalid Mode - %d on dev %d", curr_mode,
               device );
    return FTM_CDMA_BAD_RSP; /* Failure */
  }

  /* Query the current RF State */
  (void)ftm_cdma_data_get_device_status( device, &dev_state );

  /* Get the current band from FTM Data structure. This should have been
     populated when FTM Set Mode is called.*/
  band_chan.band = rf_cdma_rf_band_type_to_sys_band_type(dev_state.curr_band);
  band_chan.chan_num = rf_channel;

  /* Perform Tuning to Channel based on Current RF State */
  switch( dev_state.rf_state )
  {
  case FTM_CDMA_STATE_SLEEP:
  {
    /* if device passed in is secondary (diversity) device, "then first need 
       to wake up primary", and then enable diversity.
       Need to wakeup primary because of tool-sequence - it calls set_mode for
       device-0 and then for device-1. This has the effect of putting the
       FTM STM and radio in sleep state
   */
    if (dev_state.chain_config == RECEIVE_DIVERSITY)
    {
      /* find out the master for the diversity device */
      m_device = ftm_cdma_resolve_master_device(device);
      
      /* Query the current RF State */
      (void)ftm_cdma_data_get_device_status( m_device, &m_dev_state );
      
      if ( m_dev_state.rf_state == FTM_CDMA_STATE_SLEEP )
      {
        /* wake up the primary/master device */
        cb_payload.device = m_device; /* Populate the CB Payload */      
        rfm_ret &= ( rfm_1x_prep_wakeup_rx( m_device, m_dev_state.rxlm_handle, 
                                            band_chan, &ftm_1x_rfm_callback,
                                            &cb_payload ) >= 0 );

        cb_payload.device = m_device; /* Populate the CB Payload */
        rfm_ret &= ( rfm_1x_exec_wakeup_rx( m_device, m_dev_state.rxlm_handle, 
                                            &ftm_1x_rfm_callback,
                                            &cb_payload ) >= 0 );
        if ( rfm_ret == TRUE )
        {
          ftm_cdma_data_update_rf_state ( m_device, FTM_CDMA_STATE_RX );
        }
      }

      /* it is now safe to enable diversity (assuming that the above call 
         succeeded */
      cb_payload.device = m_device; /* Populate the CB Payload */
      rfm_ret &= ( rfm_1x_enable_diversity( m_device, m_dev_state.rxlm_handle,
                                            device, dev_state.rxlm_handle, 
                                            &ftm_1x_rfm_callback,
                                            &cb_payload ) >=0 );
        
      if (rfm_ret == TRUE)
      {
        ftm_cdma_data_update_assoc_device( m_device, device);
        ftm_cdma_data_update_assoc_device( device, m_device);
        ftm_cdma_data_update_band_chan( m_device, 
                rf_cdma_sys_band_type_to_rf_band_type(band_chan.band),
                (rfm_cdma_chan_type )band_chan.chan_num );
      }

      new_rf_state = FTM_CDMA_STATE_RXDIV;

    } /* if (dev_state.chain_config == RECEIVE_DIVERSITY) */
    else
    {
      /* this is the straight-forward case where device passed in is
         not a diversity device. So need to just enable Rx */
      cb_payload.device = device; /* Populate the CB Payload */
      rfm_ret &= ( rfm_1x_prep_wakeup_rx( device, dev_state.rxlm_handle, 
                                          band_chan, &ftm_1x_rfm_callback,
                                          &cb_payload ) >= 0 );

      cb_payload.device = device; /* Populate the CB Payload */
      rfm_ret &= ( rfm_1x_exec_wakeup_rx( device, dev_state.rxlm_handle,
                                          &ftm_1x_rfm_callback,
                                          &cb_payload ) >= 0 );

      new_rf_state = FTM_CDMA_STATE_RX;
    }
    break;

  } /* FTM_CDMA_STATE_SLEEP */

  case FTM_CDMA_STATE_RXDIV:
  {
    /* it the state is FTM_CDMA_STATE_RXDIV, it means that the device passed
       in was a diversity device (i.e, not a master device) */
   
    /* doing this assignment just to make code readablity better */
    rfm_device_enum_type s_device = device;  /* signifies the slave-device */

    /* Query the current RF State */
    (void)ftm_cdma_data_get_device_status( s_device, &s_dev_state );

    m_device = s_dev_state.assoc_dev;

    (void)ftm_cdma_data_get_device_status( m_device, &m_dev_state );

    /* tune to new chan and band - do the same for associated device if 
       applicable */
    cb_payload.device = m_device; /* Populate the CB Payload */
    rfm_ret &= ( rfm_1x_retune( m_device, m_dev_state.rxlm_handle, band_chan, 
                                &ftm_1x_rfm_callback, &cb_payload ) >= 0 );

    /* check for RFM api results */
    if (rfm_ret == TRUE)
    {
      /* Update FTM Data Structure with current band and chan */
      ftm_cdma_data_update_band_chan( m_device, 
                rf_cdma_sys_band_type_to_rf_band_type(band_chan.band),
                (rfm_cdma_chan_type )band_chan.chan_num );
    }

    /* only need to update the RF state of device sent in arg list of the
       API */
    new_rf_state = FTM_CDMA_STATE_RXDIV;

    break;
  }

  case FTM_CDMA_STATE_RX:
    /* RF is already tuned to band and channel. RF will be now retuned to
    new Channel passed to this function. If the RF state is diversity, re-tune
    will take care of retuning all Devices. This will follow the updating of
    FTM data structure. */
    cb_payload.device = device; /* Populate the CB Payload */
    rfm_ret &= ( rfm_1x_retune( device, dev_state.rxlm_handle, band_chan, 
                                &ftm_1x_rfm_callback, &cb_payload ) >= 0 );

    /* 1) update the data structure for slave device.
       2) for the device passed in "arg list", info is updated at end of this
          API */
    if (rfm_ret == TRUE)
    {
      /* check if diversity info needs to be updated */
      if (dev_state.assoc_dev != RFM_INVALID_DEVICE)
      {
        rfm_device_enum_type slave_dev = dev_state.assoc_dev;
        /* get the current RF State for slave device */
        (void)ftm_cdma_data_get_device_status( slave_dev, &s_dev_state );

        /* update with new band and chan info for slave device */
        ftm_cdma_data_update_band_chan( slave_dev, 
                rf_cdma_sys_band_type_to_rf_band_type(band_chan.band),
                (rfm_cdma_chan_type )band_chan.chan_num );
      }
    } /* (rfm_ret == TRUE) */

    new_rf_state = FTM_CDMA_STATE_RX;
    break;

  case FTM_CDMA_STATE_RXTX:
    cb_payload.device = device; /* Populate the CB Payload */
	
	/* setting Frame valid to FALSE, this will trigger FW to run RF on/off scripts. */
    if( rfm_get_calibration_state() == FALSE )
    {
      ftm_1x_enable_baseband_tx( FALSE );
    }	
    rfm_ret &= ( rfm_1x_retune( device, dev_state.rxlm_handle, band_chan,
                                &ftm_1x_rfm_callback, &cb_payload ) >= 0 );

    /* 1) update the data structure for slave device.
       2) for the device passed in "arg list", info is updated at end of this
          API */
    if (rfm_ret == TRUE)
    {
      /* Mimic the L1 call to set Frame valid to TRUE , this will 
       trigger FW to run RF on/off scripts. Not needed in cal mode since 
       FW is put in override mode for cal. */  
      if( rfm_get_calibration_state() == FALSE )
      {
        ftm_1x_enable_baseband_tx( TRUE );
      }	

      /* check if diversity info needs to be updated */
      if (dev_state.assoc_dev != RFM_INVALID_DEVICE)
      {
        rfm_device_enum_type slave_dev = dev_state.assoc_dev;
        /* get the current RF State for slave device */
        (void)ftm_cdma_data_get_device_status( slave_dev, &s_dev_state );

        /* update with new band and chan info for slave device */
        ftm_cdma_data_update_band_chan( slave_dev, 
                rf_cdma_sys_band_type_to_rf_band_type(band_chan.band),
                (rfm_cdma_chan_type )band_chan.chan_num );
      }
    } /* (rfm_ret == TRUE) */

    new_rf_state = FTM_CDMA_STATE_RXTX;
    break;

  case FTM_CDMA_STATE_INVALID: /* Invalid State */
  default:
    ret_val = FTM_CDMA_BAD_RSP;
    FTM_MSG_1( FTM_ERROR, "Set Chan : Invalid RF State - %d", \
               dev_state.rf_state );
    break;

  } /* switch( dev_state.rf_state ) */

  /* Since RFM failed, flag return value as BAD RESPONSE */
  if ( rfm_ret == FALSE )
  {
    ret_val = FTM_CDMA_BAD_RSP;
    ftm_cdma_data_update_rf_state ( device, FTM_CDMA_STATE_SLEEP );
  }
  else if (ret_val == FTM_CDMA_GOOD_RSP)
  {
    /* Update FTM Data Structure with current band and chan */
    ftm_cdma_data_update_band_chan( device, 
                rf_cdma_sys_band_type_to_rf_band_type(band_chan.band),
                (rfm_cdma_chan_type )band_chan.chan_num );
    ftm_cdma_data_update_rf_state ( device, new_rf_state );
  }

  /* Update synth lock status */
  ftm_1x_update_aggreated_multisynth_status(device);

  /* Get Time for this ISR */
  prof_cal_func_val = rf_time_get_elapsed( prof_cal_func, RF_USEC );

  FTM_MSG_3( FTM_LOW, "Set Chan : Done for Device %d, Chan %d, Time %d",
             device, rf_channel, prof_cal_func_val );

  /* Return the State */
  return ret_val;

} /* ftm_1x_set_channel */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Sets the chain_config flag for the device to given value

  @details
  The chain_config flag carries significance only for secondary devices. This
  flag is used in ftm_1x_set_channel() to tune the Rx Chain has in OFS mode or 
  in diversity mode
 
  Associated FTM Command : <b>FTM_SET_SECONDARY_CHAIN</b>

  @param device
  RF slave device on which the given channel is to be tuned
 
  @param sec_chain_mode
  Indicates if secondary chain is to tuned in diversity / OFS mode, or be 
  disabled

  @return
  Status of execution completion of the function execution
*/
ftm_cdma_rsp_type
ftm_1x_set_second_chain_mode
(
  rfm_device_enum_type device,
  ftm_sec_chain_ctl_enum_type sec_chain_mode
)
{  
  ftm_cdma_rsp_type ret_val = FTM_CDMA_GOOD_RSP; /* Return Value */
  rf_time_tick_type profiler_var;
  rf_time_type profiler_var_val;

  /* check if driver is in FTM_MODE */
  if(ftm_mode != FTM_MODE)
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_set_sec_chain_mode: Not in FTM Mode");
    return FTM_CDMA_BAD_RSP; /* Failure */
  }

  /* Get starting time tick to profile the function */
  profiler_var = rf_time_get_tick(); 

  switch( sec_chain_mode )
  {
  case OFF_FREQUENCY_SEARCH:
    {
      /* This section assumes that the set_mode API has already been called 
      with the appropriate mode settings before calling the API for set  
      secondary chain mode, otherwise it will fail to set the desired band */ 
      ftm_cdma_data_status_type dev_state;/* current state of device*/
      ftm_mode_id_type mode_for_ofs; /* mode into which sec-chain should enter*/
      /* Get state info of device state machine*/ 
      (void)ftm_cdma_data_get_device_status( device, &dev_state );

      /* Get current mode from device state machine */
      mode_for_ofs = ftm_1x_resolve_mode
                  (rf_cdma_rf_band_type_to_sys_band_type(dev_state.curr_band));

      /* do a set-mode on sec-chain */
      ftm_1x_set_mode( device, mode_for_ofs);
  
      /* update state machine */
      ftm_cdma_data_update_chain_config(device, sec_chain_mode);
    }

    break;

  case DISABLE_SECONDARY_CHAIN:
    /* update state machine */
    ftm_cdma_data_update_chain_config(device, sec_chain_mode);

    /* disable rx-diversity*/
    ftm_1x_tear_down(device);

    break;

  case RECEIVE_DIVERSITY:
    {
      ftm_cdma_data_status_type m_dev_state;/* current state of master device*/
      ftm_mode_id_type mode_for_sec; /* mode into which sec-chain should enter*/
      rfm_device_enum_type m_device; /* master device of "device" in arg list*/
      
      /* find out the master for the diversity device */
      m_device = ftm_cdma_resolve_master_device(device);

      /* Get state info of master device */
      (void)ftm_cdma_data_get_device_status( m_device, &m_dev_state );

      /* mode for secondary should be the same as primary - so use the primay 
         chain band to look-up the mode */
      mode_for_sec = ftm_1x_resolve_mode
        (rf_cdma_rf_band_type_to_sys_band_type(m_dev_state.curr_band));
      
      /* do a set-mode on sec-chain - this also sets tunes the RF driver to a
      band based on mode_for_sec */
      ftm_1x_set_mode( device, mode_for_sec);

      if ( m_dev_state.rf_state != FTM_CDMA_STATE_SLEEP )
      {  
        ftm_cdma_data_update_chain_config(device, sec_chain_mode);
        ftm_1x_set_channel(device, m_dev_state.curr_chan);
      }
      else
      {
        ftm_cdma_data_update_chain_config(device, sec_chain_mode);
      }
    }
    break;

  default:
    ret_val = FTM_CDMA_BAD_RSP;
    FTM_MSG_1( FTM_ERROR, "ftm_1x_set_sec_chain_mode: Unsupported value = %d", 
               sec_chain_mode);
    break;
  } /* switch( sec_chain_mode ) */

  /* Get execution time for this function  */
  profiler_var_val = rf_time_get_elapsed ( profiler_var , RF_USEC ) ; 
  FTM_MSG_3 ( FTM_LOW, "ftm_1x_set_sec_chain_mode: Done for Device %d, Operation %d, Execution Time: %d", 
              device, sec_chain_mode, profiler_var_val ) ;

  /* Return the State */
  return ret_val;

} /* ftm_1x_set_second_chain_mode */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure 1x Radio for second chain test call mode

  @details
  The function configure 1x radio for second chain test call mode by
  setting the correct RxAGC path for TxAGC calculation.
 
  Associated FTM Command : <b>FTM_SECOND_CHAIN_TEST_CALL</b>

  @param device
  RF device for which second chain test call is to be done
 
  @param sec_chain_cmd
  Second Chain Test call command indicating which device to use to
  set RxAGC source for TxAGC

  @return
  Status of execution completion of the function execution
*/
ftm_cdma_rsp_type
ftm_1x_set_second_chain_test_call
(
  rfm_device_enum_type device,
  ftm_second_chain_test_call_type sec_chain_cmd
)
{
  ftm_cdma_rsp_type ret_val = FTM_CDMA_GOOD_RSP; /* Return Value */
  uint8 chain_select_mask = 0;
  boolean success_flag = TRUE; /* Sucess flag */

  /* check if driver is in FTM_MODE */
  if(ftm_mode != FTM_MODE)
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_set_second_chain_test_call: Not in FTM Mode");
    return FTM_CDMA_BAD_RSP; /* Failure */
  }

  switch (sec_chain_cmd)
  {

  case FTM_PRIMARY_CHAIN_ONLY:
    chain_select_mask = 0x1; /* Bit0 = 1 */
    break;

  case FTM_SECONDARY_CHAIN_ONLY:
    chain_select_mask = 0x2; /* Bit1 = 1 */
    break;

  case FTM_PRIMARY_AND_SECONDARY:
    chain_select_mask = 0x3; /* Bit1 = 1, Bit0 = 1 */
    break;

  default:
    chain_select_mask = 0x0;
    break;

  }

  /* Call 1xL1 to override the chain selection */
  success_flag &= srch_rx_div_override_chain_selection(chain_select_mask);

  if ( success_flag == TRUE )
  {
    ret_val = FTM_CDMA_GOOD_RSP;
  }
  else
  {
    ret_val = FTM_CDMA_BAD_RSP;
  }

  FTM_MSG_2( FTM_LOW, "ftm_1x_set_second_chain_test_call: Done for Device %d,"
             "Operation %d", device, sec_chain_cmd );

  /* Return the State */
  return ret_val;

} /* ftm_1x_set_second_chain_test_call */

/*! \} */

/*============================================================================*/
/*!
  @name Tx Radio Control

  @brief
  Functions for the FTM to control the radio
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Turn on the transmitter. This API mimics the calls that L1 would make
  to enable the transmitter

  @details
  This function is used to configure and enable the transmitter.
 
  Mimic the L1 sequence here 
  -# enable relevant clocks needed for stable TX operation
  -# configure and enable modulator for 1x
  -# configure and enable Tx 
 
  Associated FTM Command :
  -# <b>FTM_SET_TX_ON</b>
 
  @param device
  RF Device on which Tx is to be turned on or off

  @return
  Status of execution completion of the function execution
*/
ftm_cdma_rsp_type
ftm_1x_enable_tx
(
  rfm_device_enum_type device
)
{
  ftm_cdma_data_status_type dev_state; /* hold current state of device */
  ftm_cdma_rsp_type ret_val; /* Return Value of API */
  rfm_mode_enum_type curr_mode; /* Var to store current mode */
  rfm_wait_time_t rfm_ret; /* Return val from RFM */
  ftm_cdma_state_type next_rf_state; /* state in which the FTM state machine
    would transition to on successful completion of this API */
  ftm_1x_rfm_callback_data_type cb_payload;

  /* Must be in FTM_MODE */
  if(ftm_mode != FTM_MODE)
  {
    FTM_MSG_1( FTM_ERROR, "ftm_1x_enable_tx: Set Tx ON failed: Not in FTM Mode-" 
                          "%d", ftm_mode ); 
    return FTM_CDMA_BAD_RSP; /* Failure */
  }

  /* Get Current Mode */
  curr_mode = rfm_get_current_mode(device);

  /* Check for Mode Validity */
  if ( curr_mode != RFM_1X_MODE )
  {
    FTM_MSG_2( FTM_ERROR, "ftm_1x_enable_tx:Set Tx ON failed: Invalid Mode-%d" 
                          "on device %d", curr_mode, device );
    return FTM_CDMA_BAD_RSP; /* Failure */
  }

  /* check for a valid Tx device */
  device = ftm_1x_get_valid_tx_device(device);

  if ( device != RFM_INVALID_DEVICE)
  {
    ftm_cdma_data_get_device_status(device, &dev_state);
  }
  else
  {
    FTM_MSG(FTM_ERROR, "ftm_1x_enable_tx: Valid Tx device not found - bailing");
    return FTM_CDMA_BAD_RSP;
  }

  /* Since TX needs to be enabled, the next intended state is 
     FTM_CDMA_STATE_RXTX 
  */
  next_rf_state = FTM_CDMA_STATE_RXTX;

  /*
     Check to see if the current FTM RF state is already the same as
     state to be transitioned to.
     For ex: If TX is to be enabled and if the FTM state is already 
     FTM_CDMA_STATE_RXTX, then there is no need to execute further - just 
     return as success
  */
  if (next_rf_state == dev_state.rf_state)
  {
    FTM_MSG_2( FTM_HIGH, "ftm_1x_enable_tx: No operations needed.. Already in "
                         "State=%d on device=%d", 
                         next_rf_state, device );
    return FTM_CDMA_GOOD_RSP;
  }
  /* Step 1: force 1x into traffic mode */
  rfm_1x_set_pa_access_probe_config( device, TRUE );

  /* Set frequency adjustment parameters to default for FTM 
  Tx wakeup. Freq error is initialized to zero */
  (void)rfm_1x_ftm_set_tx_freq_adjust( 0 );

  /* Step 2: configure and enable Tx - use 0 for callback and userdata */
  cb_payload.device = device; /* Populate the CB Payload */
  rfm_ret = rfm_1x_wakeup_tx( device, dev_state.txlm_handle, 
                              &ftm_1x_rfm_callback, &cb_payload );

  /* Step 3: configure and enable modulator for 1x */
  if ( rfm_ret >= 0 )
  { 
    void* smem_addr = ftm_cdma_data_1x_get_fw_smem_addr();
    if (smem_addr != NULL) 
    {
      /* Configure and enable modulator for 1x */
      ftm_1x_enable_modulator( device, smem_addr );
    }
    else
    {
      rfm_ret = FALSE;
      FTM_MSG( FTM_ERROR, "ftm_1x_enable_tx: fw shared memory "
                          "ptr is NULL" );
    }
  }
  else
  {
    FTM_MSG_1( FTM_ERROR, "ftm_1x_enable_tx: enable_modulator is skipped "
                         "for Device %d", device );
  }  

  /* A rfm_ret that is less than 0 indicates that RFM API(s) failed */
  if ( rfm_ret < 0 )
  {
    ret_val = FTM_CDMA_BAD_RSP;
  }
  else
  { 
    ftm_cdma_data_update_rf_state(device, next_rf_state);

    ret_val = FTM_CDMA_GOOD_RSP;
  }

  return ret_val;

} /* ftm_1x_enable_tx */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Turn off the transmitter. This API mimics the calls that L1 would make
  to disable the transmitter

  @details
  This function is used to turn off the transmitter.
 
  Mimic the L1 sequence here 
  -# Disable Tx
  -# Disable modulator for 1x
  -# Disable relevant Tx clocks

  Associated FTM Command :
  -# <b>FTM_SET_TX_OFF</b>
 
  @param device
  RF Device on which Tx is to be disabled

  @return
  Status of execution completion of the function execution
*/
ftm_cdma_rsp_type
ftm_1x_disable_tx
(
  rfm_device_enum_type device
)
{
  ftm_cdma_data_status_type dev_state; /* Var to hold current state */
  ftm_cdma_rsp_type ret_val; /* Return Value of API */
  rfm_mode_enum_type curr_mode; /* Var to store current mode */
  rfm_wait_time_t rfm_ret; /* Return val from RFM */
  ftm_cdma_state_type next_rf_state; /* state in which the FTM state machine
    would transition to on successful completion of this API */
  ftm_1x_rfm_callback_data_type cb_payload;

  /* Must be in FTM_MODE */
  if(ftm_mode != FTM_MODE)
  {
    FTM_MSG_1( FTM_ERROR, "ftm_1x_disable_tx: Disable Tx failed: Not in FTM "
                          "Mode (=%d)", ftm_mode ); 
    return FTM_CDMA_BAD_RSP; /* Failure */
  }

  /* Get Current Mode */
  curr_mode = rfm_get_current_mode(device);

  /* Check for Mode Validity */
  if ( curr_mode != RFM_1X_MODE )
  {
    FTM_MSG_2( FTM_ERROR, "ftm_1x_disable_tx:Disable Tx failed: Invalid Mode-%d" 
                          "on device %d", curr_mode, device );
    return FTM_CDMA_BAD_RSP; /* Failure */
  }

  /* check for a valid Tx device */
  device = ftm_1x_get_valid_tx_device(device);

  if ( device != RFM_INVALID_DEVICE )
  {
    ftm_cdma_data_get_device_status(device, &dev_state);
  }
  else
  {
    FTM_MSG( FTM_HIGH, "ftm_1x_disable_tx: Valid Tx device not found - "
                       "bailing");

    /* Returning GOOD response: This is required to fix the bad tools
    sequence of doing TX_OFF at the start of the composte cal sweep for DRx
    chain :: EVIL */
    return FTM_CDMA_GOOD_RSP;
  }

  /* TX is already ON, so the next intended state would be FTM_CDMA_STATE_RX 
    (i.e, put TX to sleep) */
  next_rf_state = FTM_CDMA_STATE_RX;

  /*
     Check to see if the current FTM RF state is already the same as
     state to be transitioned to.
     For ex: If TX is to be enabled and if the FTM state is already 
     FTM_CDMA_STATE_RXTX, then there is no need to execute further - just 
     return as success
  */
  if (next_rf_state == dev_state.rf_state)
  {
    FTM_MSG_2( FTM_HIGH, "ftm_1x_disable_tx: No operations needed.. Already in "
                         "State=%d on device=%d", 
                         next_rf_state, device );
    return FTM_CDMA_GOOD_RSP;
  }

  cb_payload.device = device; /* Populate the CB Payload */

  /* Step1: Disable Modulator */
  ftm_1x_disable_modulator( device );

  /* Step2: disable Tx - use 0 for callback and userdata */
  rfm_ret = rfm_1x_sleep_tx( device, dev_state.txlm_handle, 
                             &ftm_1x_rfm_callback, &cb_payload );



  /* Step3: Disable relevant clocks - taken care of by MCPM  */

  /* A rfm_ret that is less than 0 indicates that RFM API(s) failed */
  if ( rfm_ret < 0 )
  {
    ret_val = FTM_CDMA_BAD_RSP;
  }
  else
  { 
    ftm_cdma_data_update_rf_state(device, next_rf_state);

    ret_val = FTM_CDMA_GOOD_RSP;
  }

  return ret_val;

} /* ftm_1x_disable_tx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure Tx Waveform to CDMA or CW

  @details
  This function will configure the Tx waveform to CDMA or CW depending
  on parameters passed. Tx Offset waveform is not supported
 
  Associated FTM Command : <b>FTM_SET_TX_WAVEFORM</b>
 
  @param device
  RF Device for which Tx waveform is to be configured
 
  @param ftm_tx_waveform
  Type of waveform to be generated

  @param offset_val
  Waveform attribute specifying the offset value

  @return
  Status of execution completion of the function execution
*/
ftm_cdma_rsp_type
ftm_1x_set_tx_waveform
(
  rfm_device_enum_type device,
  ftm_tx_waveform_type ftm_tx_waveform,
  ftm_tx_waveform_attrib_type offset_val
)
{
  /* program mdsp to get the intended offset */
  ftm_1x_set_freq_offset(offset_val);

  /* Set CDMA or CW */
  ftm_1x_set_cdma_cw(ftm_tx_waveform);

  return FTM_CDMA_GOOD_RSP;

} /* ftm_1x_configure_tx_waveform */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set the PA state range

  Associated FTM Command : <b>FTM_SET_PA_RANGE</b>
 
  @param device
  RF Device for which PA range is to be set
 
  @param pa_range
  Set the PA range to this value

  @return
  Status of execution completion of the function execution
*/
ftm_cdma_rsp_type
ftm_1x_set_pa_range
(
  const rfm_device_enum_type device,
  uint8 pa_range
)
{
  ftm_cdma_rsp_type ret_val; /* Response packet */
  boolean rfm_ret;                              /* holds result of RFM APIs */
  ftm_cdma_data_status_type *dev_ptr;           /* pointer to FTM CDMA data */

  dev_ptr = ftm_cdma_data_get_device_ptr(device);

  /* check for valid device ptr */
  if (dev_ptr == NULL)
  {
    FTM_MSG_1( FTM_ERROR, "ftm_1x_set_pa_range: Failed to get a valid "
                          "device pointer for device %d", device );
    return FTM_CDMA_BAD_RSP; /* Failure */
  }

  /* Set the PA range */
  rfm_ret = rfm_1x_ftm_set_pa_range( device, (rfm_pa_range_type)pa_range);

  if (rfm_ret == TRUE)
  {
    /* update FTM status with PA Range override information */
    dev_ptr->override_data.pa_range = pa_range;
    dev_ptr->override_data.pa_range_overriden = TRUE;
    /* Since PA Range is overriden, map PA Range to PA State and update
    FTM status */
    dev_ptr->override_data.pa_state = 
       rfm_cdma_map_pa_range_to_pa_state
       (
          device, 
          dev_ptr->curr_band, pa_range
       );
    ret_val = FTM_CDMA_GOOD_RSP;
  }
  else
  {
    ret_val = FTM_CDMA_BAD_RSP;
    FTM_MSG( FTM_ERROR, "ftm_1x_set_pa_range: Failed in RFM API"); 
  }

  return ret_val;

} /* ftm_1x_set_pa_range */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set the PA state range

  Associated FTM Command : <b>FTM_SET_PA_RANGE</b>
 
  @param device
  RF Device for which PA range is to be set
 
  @param pa_range
  Set the PA range to this value

  @return
  Status of execution completion of the function execution
*/
ftm_cdma_rsp_type
ftm_1x_clear_pa_range_override
(
  const rfm_device_enum_type device
)
{
  ftm_cdma_rsp_type ret_val; /* Response packet */
  ftm_cdma_data_status_type *dev_ptr;           /* pointer to FTM CDMA data */
  boolean rf_status;

  dev_ptr = ftm_cdma_data_get_device_ptr(device);

  /* check for valid device ptr */
  if (dev_ptr == NULL)
  {
    FTM_MSG_1( FTM_ERROR, "ftm_1x_clear_pa_range_override: Failed to get a valid "
                          "device pointer for device %d", device );
    return FTM_CDMA_BAD_RSP; /* Failure */
  }

  /* If PA Range is currently overriden, clear the override before */
  if ( dev_ptr->override_data.pa_range_overriden == TRUE )
  {
    rf_status = rfm_1x_ftm_clear_pa_range_override( device );

    if ( rf_status == TRUE )
    {
      dev_ptr->override_data.pa_range_overriden = FALSE;
      ret_val = FTM_CDMA_GOOD_RSP;
    }
    else
    {
      ret_val = FTM_CDMA_BAD_RSP;
      FTM_MSG( FTM_ERROR, "ftm_1x_clear_pa_range_override: Failed in RFM API"); 
    }
  }
  else
  {
    FTM_MSG( FTM_HIGH, "ftm_1x_clear_pa_range_override: Not in override mode"); 
    ret_val = FTM_CDMA_GOOD_RSP;
  }

  return ret_val;

} /* ftm_1x_clear_pa_range_override */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Override the 1x Tx AGC PA state machine.

  @details
  Forces the 1x Tx AGC to a particular PA state, regardless of the current
  Tx Gain Control value and PA Rise/Fall values.  The PA R0/R1 control pins
  and linearizer will be selected as if the PA state machine had selected
  this state through the normal course of operation.

  For correct Tx AGC operation, PA_R_MAP must have been set correctly, and
  the linearizer for PA states should have been loaded.

  Associated FTM Command : <b>FTM_SET_PA_STATE</b>

  Side effect:
  Output power may be artifically limited high or low, depending on the
  current value of Tx Gain Control.  If the desired Tx Power is outside
  of the calibrated range of the PA state that is forced, the Tx AGC
  error may be unbounded.

  @param device
  RF Device for which PA range is to be set
 
  @param state
  The state (between RFM_PA_GAIN_STATE_0 and RFM_PA_GAIN_STATE_3) to force the 
  Tx AGC to use.

  @return
  Status of execution completion of the function execution
*/
ftm_cdma_rsp_type 
ftm_1x_set_pa_state
(
  const rfm_device_enum_type device,
  rfm_pa_gain_state_type state
)
{

  ftm_cdma_rsp_type ret_val = FTM_CDMA_BAD_RSP; /* Response packet */
  boolean rfm_ret = FALSE;  /* holds result of RFM APIs */
  ftm_cdma_data_status_type *dev_ptr;           /* pointer to FTM CDMA data */
  int16 betap_gain = 0;

  dev_ptr = ftm_cdma_data_get_device_ptr(device);
  
  /* check for valid device ptr */
  if (dev_ptr == NULL)
  {
    FTM_MSG_1( FTM_ERROR, "ftm_1x_set_pa_state: Failed to get a valid "
                          "device pointer for device %d", device );
    return FTM_CDMA_BAD_RSP; /* Failure */
  }

  if ( state < RFM_PA_GAIN_STATE_NUM )
  {
    if( rfm_get_calibration_state() == FALSE)
    {
      rfm_ret = rfm_1x_ftm_set_pa_state(device, state);
    }
    else
    {
      /* Program RF device with AVGA value */
      rfm_ret = rfm_1x_ftm_set_fw_tx_agc_override( 
                               device, 
                               dev_ptr->curr_band,
                               state,
                               dev_ptr->override_data.tx_avga_idx, 
                               betap_gain, 
                               dev_ptr->override_data.apt_smps_bias_val,
                               dev_ptr->override_data.apt_pa_q_current,
                               dev_ptr->override_data.apt_pa_q_current_enable );
    }
  }
  else
  {
    FTM_MSG_1( FTM_ERROR, "ftm_1x_set_pa_state: Invalid PA State = %d", state); 
  }

  if (rfm_ret == TRUE)
  {
    dev_ptr->override_data.pa_state = state;
    ret_val = FTM_CDMA_GOOD_RSP;
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_set_pa_state: Failed in RFM API"); 
  }

  return ret_val; /* return final result of API */

} /* ftm_1x_set_pa_state */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Program the RF device to get the intended Tx power

  @details
  The analog VGA (AVGA) is programmed in the RF device based on current PA 
  State and index provided (avga_idx)
      
  Associated FTM Command : <b>FTM_SET_PDM</b>

  @param device
  RF Device for which Tx AVGA is to be set

  @param avga_idx

  @return
  Status of execution completion of the function execution
*/
ftm_cdma_rsp_type
ftm_1x_set_tx_avga
(
  rfm_device_enum_type device,
  uint16 avga_idx
)
{
  ftm_cdma_rsp_type ret_val; /* Response packet */
  boolean rfm_ret = FALSE;                      /* holds result of RFM API */
  ftm_cdma_data_status_type *dev_ptr;          /* Var to hold current state */
  rfm_mode_enum_type curr_mode; /* Var to store current mode */
  int16 betap_gain = 0;  /* zero for now */
  /* With APT enabled, we need to set max bias in order to support legacy cal.
  For calV3, this will come from the tool interface */

  /* Get Current Mode */
  curr_mode = rfm_get_current_mode(device);

  /* Check for Mode Validity */
  if ( curr_mode != RFM_1X_MODE )
  {
    FTM_MSG_2( FTM_ERROR, "ftm_1x_set_tx_avga: Invalid Mode - %d on device %d",
               curr_mode, device );
    return FTM_CDMA_BAD_RSP; /* Failure */
  }

  /* Query the current RF State */
  dev_ptr = ftm_cdma_data_get_device_ptr( device);

  /* check for valid device ptr */
  if (dev_ptr == NULL)
  {
    FTM_MSG_1( FTM_ERROR, "ftm_1x_set_tx_avga: Failed to get a valid "
                          "device pointer for device %d", device );
    return FTM_CDMA_BAD_RSP; /* Failure */
  }

  /* Program RF device with AVGA value */
  rfm_ret = rfm_1x_ftm_set_fw_tx_agc_override( 
                               device, 
                               dev_ptr->curr_band,
                               dev_ptr->override_data.pa_state,
                               avga_idx, betap_gain, 
                               dev_ptr->override_data.apt_smps_bias_val,
                               dev_ptr->override_data.apt_pa_q_current,
                               dev_ptr->override_data.apt_pa_q_current_enable );

  if (rfm_ret == TRUE)
  {
    /* update the FTM data structure */
    dev_ptr->override_data.tx_avga_idx = avga_idx;
    ret_val = FTM_CDMA_GOOD_RSP;
  }
  else
  {
    ret_val = FTM_CDMA_BAD_RSP;
    FTM_MSG_1( FTM_ERROR, "ftm_1x_set_tx_avga: RFM Failed for Dev %d", device );
  }

  FTM_MSG_2( FTM_MED, "ftm_1x_set_tx_avga: Dev %d to RGI %d",
             device, avga_idx );

  return ret_val;

} /* ftm_1x_set_tx_avga */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure Tx to lowest Power

  @details
  This function will determine the lowest PA Gain State and Tx AVGA index.
  These values will be programmed to RF to achieve lowest Tx Power.
      
  Associated FTM Command : <b>FTM_INT_SET_LOWEST_TX_PWR</b>

  @param device
  RF Device for which lowest Tx Power is to be set

  @return
  Status of execution completion of the function execution
*/
ftm_cdma_rsp_type
ftm_1x_set_tx_lowest_power
(
  rfm_device_enum_type device
)
{
  ftm_cdma_rsp_type ret_val = FTM_CDMA_BAD_RSP; /* Response packet */
  boolean rfm_ret = TRUE; /* holds result of RFM API */
  rfm_mode_enum_type curr_mode; /* Var to store current mode */
  uint8 rgi_min; /* Minimum AVGA Index */
  uint8 rgi_max; /* Maximum AVGA Index */
  ftm_cdma_data_status_type *dev_ptr;          /* Var to hold current state */

  /* Get Current Mode */
  curr_mode = rfm_get_current_mode(device);

  /* Check for Mode Validity */
  if ( curr_mode != RFM_1X_MODE )
  {
    FTM_MSG_2( FTM_ERROR, "ftm_1x_set_tx_lowest_power: Invalid Mode - %d on "
                          "device %d", curr_mode, device );
    return FTM_CDMA_BAD_RSP; /* Failure */
  }

  /* Query the current RF State */
  dev_ptr = ftm_cdma_data_get_device_ptr( device);

  /* check for valid device ptr */
  if (dev_ptr == NULL)
  {
    FTM_MSG_1( FTM_ERROR, "ftm_1x_set_tx_lowest_power: Failed to get a valid "
                          "device pointer for device %d", device );
    return FTM_CDMA_BAD_RSP; /* Failure */
  }

  /* Get min and max AVGA table index - this is a one-time thing */
  rfm_ret &= rfm_cdma_ftm_get_min_max_avga_idx( device, &rgi_min, &rgi_max);

  /* Set lowest Tx AGC - set lowest index for AVGA to get the lowest
  possible Tx power */ 
  /* For min Tx power the bias voltage that should be sent should come 
  from the PA driver, this is the minimum possible bias voltage that can 
  be set by the QPOET */ 
  rfm_ret &= rfm_1x_ftm_set_fw_tx_agc_override( device, dev_ptr->curr_band,
                                                RFM_PA_GAIN_STATE_0,
                                                rgi_min, 0,/*betaP*/
                                                RFDEVICE_PAPM_MIN_TX_BIAS_VOLTAGE, 
                                                0,/*Quiscent current*/ 
                                                FALSE);

  if (rfm_ret == TRUE)
  {
    ret_val = FTM_CDMA_GOOD_RSP;
  }
  else
  {
    ret_val = FTM_CDMA_BAD_RSP;
    FTM_MSG( FTM_ERROR, "ftm_1x_set_tx_lowest_power: RFM Failed " );
  }

  return ret_val;

} /* ftm_1x_set_tx_lowest_power */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Override XPT txagc in RF

  @details
  This function calls the RFM FTM interface to perform a Tx override from RF
 
  @param ovr_params
  Override parameters obtained from FTM dispatch
 
  @return
  TRUE is set for successful execution; FALSE otherwise
*/
boolean
ftm_1x_set_xpt_txagc_override
(
  ftm_xpt_txagc_override_type *ovr_params
)
{
  boolean ret_val = TRUE;
  rfcom_band_type_u rfm_band;

  /* FTM xPT Tx AGC override does not override the DPD HW */
  uint8  amam_bank_idx = -1;
  uint16 amam_start_offset = -1;
  uint8  amam_tbl_size = 0;
  uint8  ampm_bank_idx = -1;
  uint16 ampm_start_offset = -1;
  uint8  ampm_tbl_size = 0;
  uint8  epdt_bank_idx = -1;
  uint16 epdt_start_offset = -1;
  uint8  epdt_tbl_size = 0;

  ftm_cdma_data_status_type *dev_ptr;           /* pointer to FTM CDMA data */

  dev_ptr = ftm_cdma_data_get_device_ptr( ovr_params->device );

  /* check for valid device ptr */
  if (dev_ptr == NULL)
  {
    FTM_MSG_1( FTM_ERROR, "ftm_1x_set_xpt_txagc_override: "
                          "Failed to get a valid "
                          "device pointer for device %d", 
               ovr_params->device );
    return FALSE;
  }

  if ( rfcommon_debug_flags.use_rflm_txagc_override )
  {
    /* Process override params */
    ret_val &= rfm_1x_ftm_process_tx_override_params( 
                                      ovr_params->device,           
                                      ovr_params->xpt_mode,         
                                      amam_bank_idx,    
                                      amam_start_offset,
                                      amam_tbl_size,    
                                      ampm_bank_idx,    
                                      ampm_start_offset,
                                      ampm_tbl_size,    
                                      epdt_bank_idx,    
                                      epdt_start_offset,
                                      epdt_tbl_size,
                                      ovr_params->iq_gain_action_type,    
                                      ovr_params->iq_gain,
                                      ovr_params->env_scale_action_type,        
                                      ovr_params->env_scale,        
                                      ovr_params->rgi,              
                                      ovr_params->smps_bias,        
                                      ovr_params->pa_state,         
                                      ovr_params->pa_curr,
                                      ovr_params->delay_action_type,
                                      ovr_params->delay );

    /* Apply the Tx override through RFLM */
    rfm_band.cdma_band = dev_ptr->curr_band;
    ret_val &= rfm_ftm_apply_xpt_tx_agc_override( RFM_1X_MODE, ovr_params->device,
                                                  rfm_band );
  }
  else
  {
  /* Send override to RF */
  ret_val &= rfm_1x_ftm_set_rf_tx_agc_override( 
                                    ovr_params->device,           
                                    ovr_params->xpt_mode,         
                                    amam_bank_idx,    
                                    amam_start_offset,
                                    amam_tbl_size,    
                                    ampm_bank_idx,    
                                    ampm_start_offset,
                                    ampm_tbl_size,    
                                    epdt_bank_idx,    
                                    epdt_start_offset,
                                    epdt_tbl_size,
                                    ovr_params->iq_gain_action_type,    
                                    ovr_params->iq_gain,
                                    ovr_params->env_scale_action_type,        
                                    ovr_params->env_scale,        
                                    ovr_params->rgi,              
                                    ovr_params->smps_bias,        
                                    ovr_params->pa_state,         
                                    ovr_params->pa_curr,
                                    ovr_params->delay_action_type,
                                    ovr_params->delay );        
  }

  FTM_MSG_2( FTM_HIGH, "ftm_1x_set_xpt_txagc_override: Dev %d [Status %d]",
             ovr_params->device, ret_val );

  return ret_val;
}

/*! \} */

/*============================================================================*/
/*!
  @name Common Radio Control

  @brief
  This section includes all Helper Functions to be used for 1x Radio Control.
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Tear Down RF

  @details
  This function will Tear Down RF to Sleep State, no mater what the current
  state is. This is basically used when FTM_SET_MODE and FTM_SET_CHAN commands
  are executed.
 
  @param device RF Device on which RF is to be torn down
 
  @return
  Flag for the function to indicate whether tear down was required or not. Based 
  on this parameter, rfm_enter_mode() will be called in calling function. If 
  TRUE, rfm_enter_mode() is not called. This will also be FALSE for the case,
  when RFM failure is seen during tear-down; thus indicating the calling
  function that rfm_enter_mode() has to be done to reset everything.
*/
boolean
ftm_1x_tear_down
(
  rfm_device_enum_type device
)
{
  /* Flag indicating if tear down is needed */ 
  boolean was_tear_down_req = TRUE; 
  ftm_cdma_data_status_type *dev_status = NULL; /* FTM Device Status */
  ftm_cdma_data_status_type *assoc_dev_status = NULL; /*FTM assocev Status*/
  uint8 device_state_before_teardown;
  rfm_device_enum_type assoc_dev;

  /* Query the current RF State Ptr for master device */
  dev_status = ftm_cdma_data_get_device_ptr(device);

  /* NULL pointer check*/
  if(  dev_status == NULL )
  {
    FTM_MSG_1(FTM_ERROR,"ftm_1x_tear_down: NULL data for device %d",device);
  
    was_tear_down_req = FALSE;
  }/* if(  dev_status == NULL ) */
  else
  {
    /* Store the current RF state for the device */
     device_state_before_teardown = dev_status->rf_state;

    /* If the device was on RxTx state, then disable the modulator */
    if( device_state_before_teardown ==  FTM_CDMA_STATE_RXTX )
    {
      /* Disable the modulator during tear down */
      ftm_1x_disable_modulator( device );
    } /* if( device_state_before_teardown ==  RF_CDMA_STATE_RXTX ) */
  
    /* Tear down radio using RFM tear down API*/
    was_tear_down_req = rfm_1x_ftm_tear_down_radio( device );
      
    /* Reset FTM variables based on RFM Tear Down action */
    if ( was_tear_down_req )
    {
      /* Store the associated device, if present */
      assoc_dev = dev_status->assoc_dev;
  
      /* Reset the Data structure for given Device */
      dev_status->rf_state = FTM_CDMA_STATE_SLEEP;
      dev_status->assoc_dev = RFM_INVALID_DEVICE;
  
      /* Check if the associated device is valid */
      if ( assoc_dev != RFM_INVALID_DEVICE )
      {
      /* Get associated device pointer*/
        assoc_dev_status = 
                 ftm_cdma_data_get_device_ptr( assoc_dev );  
  
      /* NULL pointer check*/
        if( assoc_dev_status == NULL )
      {
          FTM_MSG_1( FTM_ERROR,"ftm_1x_tear_down: NULL data for Assoc "
                              "device %d", dev_status->assoc_dev );
      }/* if(  ftm_assoc_dev_status == NULL ) */
      else
      {
  
          /* Reset the state of associated device which is in DIV state.
          This is done because, when a teardown request for Slave device is
          received, there is no need to put the master device to sleep.
          However if there is a request for master device teardown, there
          we need need to put the slave device to sleep. */
          if ( assoc_dev_status->rf_state == FTM_CDMA_STATE_RXDIV )
          {  
  
            FTM_MSG_1( FTM_HIGH,"ftm_1x_tear_down: assc div status:%d",
                       assoc_dev_status->rf_state ); 
            assoc_dev_status->rf_state = FTM_CDMA_STATE_SLEEP;
            assoc_dev_status->assoc_dev = RFM_INVALID_DEVICE;
          } /* if ( assoc_dev_status->rf_state == RF_CDMA_STATE_RXDIV ) */
  
      }/* if ! (  ftm_assoc_dev_status == NULL ) */
      } /* if ( assoc_dev != RFM_INVALID_DEVICE ) */
     }/* if ( was_tear_down_req )*/
    else
    {
       FTM_MSG_1(FTM_HIGH,"ftm_1x_tear_down: Teardown not required on "
       "device:%d", device); 
     } /* if ! ( was_tear_down_req )*/ 
   } /* if ! (  dev_status == NULL ) */
  
  /* Return */
  return was_tear_down_req;
} /* ftm_1x_tear_down */


/*! \} */


/*============================================================================*/
/*!
  @name Tx Query

  @brief
  This section includes all Query functions to be used for Tx.
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Override the 1x Tx APT PA SMPS Bias value.
*/
ftm_cdma_rsp_type 
ftm_1x_set_tx_apt_smps_bias_val
(
  const rfm_device_enum_type device,
  uint16 apt_smps_bias_val
)
{
  ftm_cdma_rsp_type ret_val; /* Response packet */
  boolean rfm_ret = FALSE;                      /* holds result of RFM API */
  ftm_cdma_data_status_type *dev_ptr;          /* Var to hold current state */
  rfm_mode_enum_type curr_mode; /* Var to store current mode */
  int16 betap_gain = 0;  /* zero for now */

  /* Get Current Mode */
  curr_mode = rfm_get_current_mode(device);

  /* Check for Mode Validity */
  if ( curr_mode != RFM_1X_MODE )
  {
    FTM_MSG_2( FTM_ERROR, "ftm_1x_set_tx_apt_smps_bias_val: "
                          "Invalid Mode - %d on device %d",
               curr_mode, device );
    return FTM_CDMA_BAD_RSP; /* Failure */
  }

  /* Query the current RF State */
  dev_ptr = ftm_cdma_data_get_device_ptr( device);

  /* check for valid device ptr */
  if (dev_ptr == NULL)
  {
    FTM_MSG_1( FTM_ERROR, "ftm_1x_set_tx_apt_smps_bias_val: "
                          "Failed to get a valid "
                          "device pointer for device %d", device );
    return FTM_CDMA_BAD_RSP; /* Failure */
  }

  /* Program RF device with APT SMPS bias value. Use currently overriden
  information for PA state, AVGA index, and PA Quiescent current */
  rfm_ret = rfm_1x_ftm_set_fw_tx_agc_override( 
                               device,                                           
                               dev_ptr->curr_band,
                               dev_ptr->override_data.pa_state,
                               dev_ptr->override_data.tx_avga_idx,
                               betap_gain,
                               apt_smps_bias_val,                                                                          
                               dev_ptr->override_data.apt_pa_q_current,                                                    
                               dev_ptr->override_data.apt_pa_q_current_enable );                                           

  if (rfm_ret == TRUE)
  {
    /* update the FTM data structure */
    dev_ptr->override_data.apt_smps_bias_val = apt_smps_bias_val;
    ret_val = FTM_CDMA_GOOD_RSP;
  }
  else
  {
    ret_val = FTM_CDMA_BAD_RSP;
    FTM_MSG_1( FTM_ERROR, "ftm_1x_set_tx_apt_smps_bias_val: "
                          "RFM Failed for Dev %d", device );
  }

  FTM_MSG_2( FTM_MED, "ftm_1x_set_tx_apt_smps_bias_val: "
                      "Dev %d to APT SMPS Bias %d",
             device, apt_smps_bias_val );

  return ret_val;

} /* ftm_1x_set_tx_apt_smps_bias_val */

/*! \} */

/*============================================================================*/
/*!
  @name DAC Calibration

  @brief
  This section includes functions related to Tx DAC calibration
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform DAC calibration in 1x Mode

  @details
  This function will complete the TX DAC cal in the 1x mode.

  @param device
  RFM Device on which DAC calibration is to be done

  @param txlm_chain
  TxLM chain pertaining to the device for which the DAC calibration   
  is to be done. 

  @return   
  Status indicating the Status of DAC Calibration 
*/
ftm_common_dac_cal_rsp_type 
ftm_1x_do_tx_dac_cal
(
  rfm_device_enum_type device,
  txlm_chain_type txlm_chain,
  uint16 dac_cal_ant0,
  uint16 dac_cal_ant1
)
{
  boolean dac_cal_status = TRUE;
  ftm_cdma_rsp_type ftm_cdma_rsp = FTM_CDMA_BAD_RSP;
  const rfc_cdma_logical_device_params_type* logical_dev = NULL;
  const rfc_common_logical_device_params_type* cmn_logical_dev = NULL;
  rf_cdma_mdsp_tx_agc_physical_read_type tx_agc_data;
  uint32 tx_agc_adj_val;
  rfdevice_dac_iref_val iref_val;
  uint8 iref_count;
  /* Init with some PA state, does not matter which PA state we use */
  rfm_pa_gain_state_type rfm_pa_gain_state = RFM_PA_GAIN_STATE_0;
  uint8 rgi_min, rgi_max;
  uint16 avga_idx;
  boolean ret_val = FALSE;
  ftm_cdma_data_status_type dev_state; /* hold current state of device */
  uint16 tune_chan = 387;   /* 120 or 388 needed if BC0 not supported.
                               Note: JTACS ch are even only. */


  /*!@note
    - Do DAC cal on CDMA default band BC0 and channel 387
    - Trigger MDSP to start DAC cal after complete Tx Turn ON */

  logical_dev = rfc_cdma_get_logical_device_params( device );

  if( logical_dev == NULL)
  {
    FTM_MSG_ERROR("Logical device not initialized for DAC Cal",0,0,0);
    return FTM_COMMON_DAC_CAL_FAILURE;
  }

  cmn_logical_dev = rfc_common_get_logical_device_params( device );

  if(cmn_logical_dev == NULL)
  {
    FTM_MSG_ERROR("Common logical device not initialized for DAC Cal",0,0,0);
    return FTM_COMMON_DAC_CAL_FAILURE;
  }

  if( rfm_cdma_is_tunable_on_device( device, SYS_BAND_BC0) == TRUE)
  {

    FTM_MSG_1(FTM_LOW, "ftm_1x_do_tx_dac_cal(): C2K band BC0"
                       " enabled for Device = %d", device);
  }
  else
  {
    FTM_MSG_1(FTM_HIGH, "EXIT ftm_1x_do_tx_dac_cal(): C2K band BC0"
                        " NOT enabled for Device = %d", device);

    /* Returning as some other Tech may be capable of calibrating current logical path */
    return FTM_COMMON_DAC_CAL_NOT_ATTEMPTED;
  }

  FTM_MSG_1( FTM_HIGH, "DAC cal is being attempted for device path %d in 1x mode", device );

  /* For BC0, mode = FTM_PHONE_MODE_CDMA_800 */
  ftm_cdma_rsp = ftm_1x_set_mode( device, FTM_PHONE_MODE_CDMA_800);

  if( ftm_cdma_rsp == FTM_CDMA_BAD_RSP)
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_do_tx_dac_cal: ftm_1x_set_mode() failed" );
    return FTM_COMMON_DAC_CAL_FAILURE;
  }

  ftm_cdma_rsp = ftm_1x_set_channel( device, tune_chan); /* Tune to BC0 ch 387*/

  if( ftm_cdma_rsp == FTM_CDMA_BAD_RSP)
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_do_tx_dac_cal: ftm_1x_set_channel() failed" );
    return FTM_COMMON_DAC_CAL_FAILURE;
  }

  ftm_cdma_rsp = ftm_1x_set_tx_waveform( device, CW_WAVEFORM, 
                                         CW_OFFSET_ZER0_KHZ);

  if( ftm_cdma_rsp == FTM_CDMA_BAD_RSP)
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_do_tx_dac_cal: ftm_1x_set_tx_waveform() "
                        "failed" );
    return FTM_COMMON_DAC_CAL_FAILURE;
  }

  if ( ftm_cdma_data_get_device_status( device, &dev_state ) == FALSE)
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_do_tx_dac_cal(): dev_state == NULL" ) ;
    return FTM_COMMON_DAC_CAL_FAILURE;
  }

  if( dev_state.txlm_handle == FTM_INVALID_LM_BUFFER)
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_do_tx_dac_cal: txlm_handle = "
                        "FTM_INVALID_LM_BUFFER" );
    return FTM_COMMON_DAC_CAL_FAILURE;
  }

  /* Get min and max indices of the Analog VGA (AVGA) table */
  dac_cal_status &= rfm_cdma_ftm_get_min_max_avga_idx( device, &rgi_min, 
                                                       &rgi_max);

  avga_idx = ((uint16) rgi_min + (uint16) rgi_max) /2;

  /* Set mean Tx AGC index in AVGA table to get min Tx power */ 
  /* For min Tx power the bias voltage that should be sent should come 
  from the PA driver, this is the minimum possible bias voltage that can 
  be set by the QPOET */ 
  dac_cal_status &= rfm_1x_ftm_set_fw_tx_agc_override( device, RFM_CDMA_BC0,
                                                     rfm_pa_gain_state,
                                                     avga_idx, 0, /*betaP*/
                                                     RFDEVICE_PAPM_MIN_TX_BIAS_VOLTAGE, 
                                                     0, /*Quiscent current */
                                                     FALSE);

  /* Wait for 1000 us for Override to take effect */
  /* Note: 1000us is a lot of wait time but, for some reason when the override time is less than 
    1000us the tx_agc_data from MDSP is not correct*/
  DALSYS_BusyWait( 1000 );

  /* Turn Tx ON for DAC calibration.   
     Note: It does not matter what PA range and the PDM we configure by default
     part of the Tx ON sequence */
  ftm_cdma_rsp = ftm_1x_enable_tx( device);

  if( ftm_cdma_rsp == FTM_CDMA_BAD_RSP)
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_do_tx_dac_cal: ftm_1x_enable_tx() failed" );
    return FTM_COMMON_DAC_CAL_FAILURE;
  }

  if( dac_cal_status )
  {
    rf_1x_mdsp_get_tx_agc_physical_data( logical_dev->tx_mdsp_path, 
                                         dev_state.txlm_handle,
                                         &tx_agc_data );
  }
  else
  {
    FTM_MSG( FTM_ERROR, "Something went wrong during Tx AGC override" );
  }


  /* Perform DAC cal on all supported Irefs of the device */

  iref_count = 0;

  for( iref_val  = RFDEVICE_DAC_CAL_IREF_MIN;
      (iref_val <= RFDEVICE_DAC_CAL_IREF_MAX) && dac_cal_status;
       iref_val++ )
  {
    /* Clear the DAC Cal results status flag in MDSP */
    rfcommon_mdsp_clear_dac_cal_results();

    FTM_MSG_2( FTM_HIGH, "ftm_1x_do_tx_dac_cal: Started for Dev %d with "
                          "iREF %d", device, iref_val );

    /* Update the 32bit Tx AGC word with Iref of interest */  
    tx_agc_adj_val = tx_agc_data.tx_avga;

    if( iref_val == RFDEVICE_IREF_5) /* Do IREF_5 cal using IREF_6 current */
    {                                /* as per DAC designers' recommendation */

      /* Dynamic updates to DAC Config based on current Iref */
      rfcommon_msm_dac_cfg_dynamic_update( txlm_chain, 
                                           dev_state.txlm_handle, 
                                           RFDEVICE_IREF_6 );

    dac_cal_status = rfdevice_cmn_set_dac_iref( cmn_logical_dev->cmn_device, 
                                                  RFDEVICE_IREF_6,
                                                  FALSE, /* program_iref */
                                                  &tx_agc_adj_val);
    }
    else
    {
      /* Dynamic updates to DAC Config based on current Iref */
      rfcommon_msm_dac_cfg_dynamic_update( txlm_chain, 
                                           dev_state.txlm_handle, 
                                                iref_val);

      dac_cal_status = rfdevice_cmn_set_dac_iref( cmn_logical_dev->cmn_device, 
                                                  iref_val,
                                                  FALSE, /* program_iref */
                                                  &tx_agc_adj_val);
    }

    if( dac_cal_status)
    {    

      rf_1x_mdsp_tx_agc_output_override( logical_dev->tx_mdsp_path,
                  tx_agc_adj_val,
                  tx_agc_data.betap,
                  rfm_pa_gain_state,
                  NULL, NULL, NULL );
     
      /* We need atleast 20us for Iref to settle */
      /* Note:
       - 1000us is a lot of wait time but, we want to be very sure   
      that FW really is updating the Tx AGC word with Iref update
      - Lower delays were found that the MDSP was not upading the TxAGC word properly*/
      DALSYS_BusyWait( 1000 ); 

      /* Trigger MDSP to start DAC Cal */
      rf_1x_mdsp_configure_dac_cal( dac_cal_ant0, dac_cal_ant1, 
                                    dev_state.txlm_handle, NULL, NULL, NULL );
    }
    else
    {
      FTM_MSG_1( FTM_ERROR, "ftm_1x_do_tx_dac_cal: rfdevice_cmn_set_dac_iref"
                            " failed for iREF %d ", iref_val );
      break;
    }

    dac_cal_status &= rfcommon_mdsp_extract_dac_cal_results( txlm_chain, 
                                                             iref_val, 
                                                             iref_count);

    if( dac_cal_status == FALSE)
    {
      FTM_MSG_1( FTM_ERROR, "ftm_1x_do_tx_dac_cal: Could not extract results"
                    " from MDSP mem for iref_val = %d", iref_val );
      break;
    }

    iref_count++; 
  } /*End of for loop which does DAC Cal on all Irefs*/


  /* DAC Cal done, Turn Tx OFF */
    ftm_cdma_rsp = ftm_1x_disable_tx( device);

  ret_val = ftm_rfmode_exit( device, FTM_STATE_UNKNOWN);

  if( ret_val == FALSE)
  {
    FTM_MSG_1( FTM_ERROR, "ftm_1x_do_tx_dac_cal: ftm_1x_tear_down "
                          "failed for device", device );
    return FTM_COMMON_DAC_CAL_FAILURE;
  }

  if( dac_cal_status)
  {
    FTM_MSG_2( FTM_HIGH, "ftm_1x_do_tx_dac_cal: Successful for Device %d;"
                         " BC0, channel %d", device, tune_chan );
    return FTM_COMMON_DAC_CAL_SUCCESS;
  }
  else
  {
    FTM_MSG_1( FTM_ERROR, "ftm_1x_do_tx_dac_cal: Failed for Dev %d", device );
    return FTM_COMMON_DAC_CAL_FAILURE;
  }

} /* ftm_1x_do_tx_dac_cal */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform 1x set freq_adjust in FTM RF mode

  @details
  This function is a ftm API that sets tx freq ajust in FTM RF mode.
  The amount of frequency correction applied is either
  the user_freq_adjust_ppb or the combination of user_freq_adjust_ppb + the 
  frequency error estimate from XO manager. This selection is based on the input 
  value of enable_xo_freq_adjust.

  @param device
  RFM Device on which DAC calibration is to be done

  @param user_freq_adjust_ppb
  User defined tx frequency adjustment in ppb units 

  @param center_frequency_hz
  Tx carrier frequency for current cellular band/channel, in Hz units
  
  @param enable_xo_freq_adjust
  Flag to select XO manager API to account for temperature dependent frequency adjustment.

  @param status
  Status indicating the Status of ftm_1x_set_freq_adjust 

  @return   
  Total frequency adjust in Hz
*/
int32 ftm_1x_set_freq_adjust
(
  rfm_device_enum_type device,
  int32 user_freq_adjust_ppb, 
  uint32 center_frequency_hz, 
  uint8 enable_xo_freq_adjust,
  ftm_cdma_rsp_type *status
)
{
  rfm_mode_enum_type curr_mode;
  ftm_cdma_rsp_type ret_val = FTM_CDMA_BAD_RSP; /* Response packet */
  boolean rfm_ret = TRUE;
  
  int32 tx_freq_error_composite_hz = 0;
  int32 freq_error_xo_ppb =0;
  int32 freq_error_xo = 0;
  
  /* Get Current Mode */
  curr_mode = rfm_get_current_mode( device );

  /* Check for Mode Validity */
  if ( curr_mode != RFM_1X_MODE )
  {
    FTM_MSG_2( FTM_ERROR, "ftm_1x_set_freq_adjust: Invalid Mode - %d on "
                          "device %d", curr_mode, device );
    return FTM_CDMA_BAD_RSP; /* Failure */
  }

  FTM_MSG_3(FTM_LOW, "ftm_1x_set_freq_adjust: tx_freq_error_ppb : %d : centre_freq_hz : %d : Enable = %d", 
                       user_freq_adjust_ppb,
                       center_frequency_hz,
                       enable_xo_freq_adjust);

  if(enable_xo_freq_adjust)
  {

    freq_error_xo = (int32)tcxomgr_ft_curr_blocking();

    //Converts the frequency error estimated by XO Manager API to ppb units.
    freq_error_xo_ppb = (int32)((int64)((int64)((int64)(freq_error_xo <<16) * 1000)/ 1024)>> 16);

    FTM_MSG_1(FTM_LOW,"ftm_1x_set_freq_adjust: freq_error_xo_ppb : %d", freq_error_xo_ppb);
  
    if(!user_freq_adjust_ppb)
    {  
      // The formula below does:
      // 1) Converts the frequency error estimated by XO Manager API to Hz from ppb as expected by 
      // rfm_1x_ftm_set_tx_freq_adjust() API
      // tx_freq_error_composite_hz = (-1)* (freq_error_xo_ppb)*2^16 * center_frequency_hz) / 10^6 / 2^16

      tx_freq_error_composite_hz = (-1)* ((int32)((int64)((
                                         (int64)((freq_error_xo_ppb << 16))*
                                         (int64)((center_frequency_hz))
                                        )/1000000000) >>16 ));
  
  
      FTM_MSG_1(FTM_LOW,"ftm_1x_set_freq_adjust: tx_freq_error_composite_hz  : %d",tx_freq_error_composite_hz);
     
    }
    else
    {  
      // The formula below does:
      // 1) Adds the user input frequency error with frequency error estimated by XO Manager. 
      // 2) Converts the result in 1) from ppb units to Hz as expected by rfm_1x_ftm_set_tx_freq_adjust() API
      // tx_freq_error_composite_hz = (-1)* ((user_freq_adjust_ppb  + freq_error_xo_ppb)*2^16 * center_frequency_hz) / 10^6 / 2^16

      tx_freq_error_composite_hz = (-1)*(int32)((int64)((
                                          (int64)(((int64)((user_freq_adjust_ppb  + freq_error_xo_ppb) << 16))* center_frequency_hz ) 
                                          )/1000000000) >> 16) ;
  
      FTM_MSG_1(FTM_LOW,"ftm_1x_set_freq_adjust: tx_freq_error_composite_hz  : %d",tx_freq_error_composite_hz);
    }
  }
  else
  {
    tx_freq_error_composite_hz = user_freq_adjust_ppb;
    FTM_MSG_1(FTM_LOW,"ftm_1x_set_freq_adjust: tx_freq_error_composite_hz  : %d",tx_freq_error_composite_hz);
  }

  /* Set tx freq adjust */ 
  rfm_ret &= rfm_1x_ftm_set_tx_freq_adjust( (int16) tx_freq_error_composite_hz );

  if ( rfm_ret == TRUE )
  {
    ret_val = FTM_CDMA_GOOD_RSP;
  }
  else
  {
    ret_val = FTM_CDMA_BAD_RSP;
    FTM_MSG( FTM_ERROR, "ftm_1x_set_freq_adjust: RFM Failed " );
  }

  *status = ret_val;
  return (int32)tx_freq_error_composite_hz;

} /* End of ftm_1x_set_freq_adjust */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform ftm_1x_set_tx_pwr_limit in FTM RF mode

  @details
  This function is a ftm API that sets tx max power limit (dBm)  in FTM RF mode.

  @param device
  RFM Device on which Tx is on

  @param tx_pwr_limit_dbm
  Power limit in dBm 

  @return   
  Status indicating the Status of  ftm_1x_set_tx_pwr_limit
*/
ftm_cdma_rsp_type
ftm_1x_set_tx_pwr_limit
( 
  rfm_device_enum_type device,
  boolean tx_pwr_override, 
  int16 tx_pwr_limit_dbm
) 
{
  rfm_mode_enum_type curr_mode; /* Var to store current mode */
  boolean rfm_ret = TRUE;
  ftm_cdma_rsp_type ret_val = FTM_CDMA_GOOD_RSP;

  /* Get Current Mode */
  curr_mode = rfm_get_current_mode(device);

  /* Check for Mode Validity */
  if ( curr_mode != RFM_1X_MODE )
  {
    FTM_MSG_2( FTM_ERROR, "ftm_1x_set_tx_pwr_limit: Invalid Mode - %d on "
                          "device %d", curr_mode, device );
    return FTM_CDMA_BAD_RSP; /* Failure */
  }

   /* Override Tx power limit */ 
  rfm_ret &= rfm_1x_ftm_set_tx_pwr_limit( device, tx_pwr_override, tx_pwr_limit_dbm );

  if ( rfm_ret == TRUE )
  {
    ret_val = FTM_CDMA_GOOD_RSP;
  }
  else
  {
    ret_val = FTM_CDMA_BAD_RSP;
    FTM_MSG( FTM_ERROR, "ftm_1x_set_freq_adjust: RFM Failed " );
  } 

  return ret_val;
  
}

/*! @} */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set the tuner tune code setttings

  @details
  This function is used to override the tuner tune code based on the
  configuration passed to the device.

  @param device
  RF device whose settings need to be overriden

  @param override_flag
  Flag for enabling and disabling the override 

  @param data
  Pointer to the tuner tunecode config structure 

  @param tuner_nv_ptr   
  Pointer to the tuner NV data structure

  @param tuner_id_ptr   
  Pointer to the tuner_id

  @return ftm_tuner_override_status_type
  Return status of the override function
*/
ftm_tuner_override_status_type 
ftm_1x_set_tuner_tune_code_override
( 
  rfcom_device_enum_type device,
  uint8 override_flag,
  void *data,
  uint8* tuner_nv_ptr,
  uint8* tuner_id_ptr
)
{
  ftm_tuner_override_status_type status;

  status =
    (ftm_tuner_override_status_type)rfm_cdma_ftm_set_tuner_tune_code_override(device,
                                                                              override_flag,
                                                                              data,
                                                                              tuner_nv_ptr,
                                                                              tuner_id_ptr);

  return status;
} /* ftm_1x_set_tuner_tune_code_override() */
/*============================================================================

FUNCTION FTM_1x_IQ_CAPTURE_GET_CONFIG

DESCRIPTION
  This API is used to get the IQ capture configuration for CDMA.

DEPENDENCIES
  None

RETURN VALUE
  Always TRUE

SIDE EFFECTS
 None
=============================================================================*/
ftm_iq_capture_error_code_type ftm_1x_iq_capture_get_config
(
  ftm_iq_capture_get_config_type *data
)
{  
  ftm_iq_capture_error_code_type ret_value=FTM_IQ_SUCCESS;

  data->max_sample_size = FTM_CDMA_IQ_MAX_SAMPLES;
  data->sample_data_format = FTM_CDMA_IQ_SS_DATA_FORMAT;
  data->sampling_freq = FTM_CDMA_IQ_SAMPLE_FREQ_CHIPX2;
  data->max_diag_sample_size = FTM_CDMA_IQ_DIAG_MAX_SAMPLES;

  FTM_MSG_4(FTM_HIGH,"CDMA IQ capture configuration: Max sample size %d, Sample"
  	                 "data format width %d, Sampling freq %lu Hz, Max diag sample size %d",
  	                 data->max_sample_size,data->sample_data_format,data->sampling_freq,
  	                 data->max_diag_sample_size);

  return ret_value;
}

/*============================================================================

FUNCTION FTM_1X_IQ_CAPTURE_ACQUIRE_SAMPLES

DESCRIPTION
  This API is used to Acquire samples for CDMA and store the information related to them in a structure. The information include the
  device, carrier and number of samples acquired.

DEPENDENCIES
  RF chain should be configured before acquring. Device and carrier id should be correctly passed.

RETURN VALUE
  FTM_IQ_SUCCESS, - Everything is fine.
  FTM_IQ_GENERAL_FAILURE  - RF chain has not been configured

SIDE EFFECTS
 None
=============================================================================*/
ftm_iq_capture_error_code_type ftm_1x_iq_capture_acquire_samples
(
  rfcom_device_enum_type device,
  ftm_iq_capture_acquire_samples_type *data
)
{
  ftm_iq_capture_error_code_type ret_value=FTM_IQ_SUCCESS;
  ftm_cdma_data_status_type dev_state ; /* Var to hold current state */
  ftm_cdma_iq_capture_data_type *ptr;

  // Make the pointer point to static data structure that holds information of the acquired IQ samples 
  // and fill up the fields as requested.

  FTM_MSG_3(FTM_HIGH,"CDMA IQ Capture: Acquire samples command sent with "
                     "parameters Device %d, Carrier ID %d, No. of Req. samples %d",
                      device,data->carrier_id,data->sample_size);
  
  // First allocate memory for buffer that will be used to capture IQ samples.
  ftm_cdma_allocate_memory_for_iq_sample_buffer();  

  // Get the pointer handler for IQ capture data structure.
  ptr=ftm_cdma_get_iq_capture_data_ptr();

  if(ptr->iq_buf == NULL)
  {
    ret_value = FTM_IQ_GENERAL_FAILURE;
    FTM_MSG_ERROR("Unable to allocate memory for IQ buffer",0,0,0);      
  }	

  // Invalidate the current data
  ptr->data_available = FALSE; 


  // Fill up data structure as per the new command
  ptr->device = device;
  ptr->req_samples = data->sample_size;

  
  switch(data->carrier_id)
  {
    case 0:	
    ptr->carrier = RFM_CDMA_CARRIER_0;	 
    break;

    case 1:	
    ptr->carrier = RFM_CDMA_CARRIER_1;
    break;	 

    case 2:	
    ptr->carrier = RFM_CDMA_CARRIER_2;
    break;

    default:
    FTM_MSG_ERROR( "Wrong configuration passed: Device %d Carrier id %d",
		            ptr->device,data->carrier_id,0);
    ret_value = FTM_IQ_GENERAL_FAILURE;
    break;
  }
  
   
  // First check if the RX chain is properly configured before capturing IQ samples.
  if(ftm_cdma_data_get_device_status(ptr->device, &dev_state))
  {
    if((dev_state.rf_state == FTM_CDMA_STATE_SLEEP) || (dev_state.rf_state == FTM_CDMA_STATE_INVALID))
    {
       FTM_MSG_ERROR( "RX chain not configured for this path %d",ptr->device,0,0); 
       ret_value = FTM_IQ_GENERAL_FAILURE;
    }	 
  }
  else
  {
    FTM_MSG_ERROR( "Unable to get device information for the device %d",ptr->device,0,0); 
    ret_value = FTM_IQ_GENERAL_FAILURE;
  }
  

  // Check if number of samples requested is greater Max value possible  
  if(ptr->req_samples > FTM_CDMA_IQ_MAX_SAMPLES)
  {
    FTM_MSG_ERROR( "Number of samples requested %d > Max. value %d",
		           ptr->req_samples,FTM_CDMA_IQ_MAX_SAMPLES,0);	  
    ret_value = FTM_IQ_GENERAL_FAILURE;
  }	

  // If we pass all the check conditions, then extract IQ samples from sample server. 
  if(ret_value == FTM_IQ_SUCCESS)
  {     
	 if(!ftm_cdma_msm_get_rx_iq_samples( ptr->device, ptr->carrier,\
									   ptr->req_samples,\
									   ptr->iq_buf ))
  	{
									   
	 ret_value = FTM_IQ_GENERAL_FAILURE;
         FTM_MSG_ERROR( "Unable to acquire IQ samples",0,0,0);	  	 
 	} 
  }


  if(ret_value == FTM_IQ_SUCCESS)
  {
    ptr->data_available = TRUE;
    FTM_MSG_3( FTM_HIGH,"%d CDMA IQ sucessfully acquired for path %d carrier %d",
		                ptr->req_samples,ptr->device,ptr->carrier);
  }	
  else
  {
    ptr->data_available = FALSE;
    FTM_MSG_3(FTM_ERROR,"Unable to acquire CDMA IQ samples for path %d, carrier %d," 
		                "Requested size %d",ptr->device,ptr->carrier,ptr->req_samples);
  }

  return ret_value;

}


/*============================================================================

FUNCTION FTM_1X_IQ_CAPTURE_GET_SAMPLES

DESCRIPTION
  This API is used to get samples for CDMA  from buffer that has been used to capture samples from ACQUIRE SAMPLES. 

DEPENDENCIES
 The CAPTURE command should have been issued.

RETURN VALUE
  FTM_IQ_SUCCESS, - Everything is fine. 
  FTM_IQ_IQ_SAMPLES_UNAVAILABLE - CAPTURE command either did not got called or did not went through fine.
  FTM_IQ_ILLEGAL_COMBIMATION - Wrong values of either sample offset and sample size. ex: sample size > Max. sample size that diag can handle

SIDE EFFECTS
 None
=============================================================================*/
void ftm_1x_iq_capture_get_samples
(
  rfcom_device_enum_type device,
  ftm_iq_capture_get_samples_type *data,
  ftm_rsp_pkt_type* rsp_pkt_ptr
)
{

  ftm_cdma_iq_capture_data_type *acq_sample_ptr=NULL;
  ftm_iq_capture_get_samples_rsp_pkt_type *get_samples_rsp_pkt_ptr 
  	= (ftm_iq_capture_get_samples_rsp_pkt_type *)rsp_pkt_ptr->pkt_payload;
  int sample_number=0,counter=0;

  FTM_MSG_3(FTM_HIGH,"CDMA IQ Capture: Get samples command sent with parameters" 
  	                 "Device %d, Sample offset %d, No. of Req. samples %d",
  	                 device,data->sample_offset,data->sample_size);

  // get the pointer for IQ data
  acq_sample_ptr=ftm_cdma_get_iq_capture_data_ptr();  

  get_samples_rsp_pkt_ptr->sample_offset = data->sample_offset;
  get_samples_rsp_pkt_ptr->sample_size = data->sample_size;

  // start with Success flag and alter it if any sanity check fails.
  get_samples_rsp_pkt_ptr->ftm_error_code = FTM_IQ_SUCCESS;

  // Check if IQ data has been acquried sucessfully. 
  if (acq_sample_ptr->data_available == FALSE)
  {
    FTM_MSG_ERROR( "IQ samples are not acquired, please acquire them first",0,0,0);      
    get_samples_rsp_pkt_ptr->ftm_error_code = FTM_IQ_IQ_SAMPLES_UNAVAILABLE;
  } 

  // Check if IQ data has been acquired for the same device as requested. 
  if (acq_sample_ptr->device != device)
  {
    FTM_MSG_ERROR( "IQ samples are not acquired for this device, please acquire them first",0,0,0);      
    get_samples_rsp_pkt_ptr->ftm_error_code = FTM_IQ_IQ_SAMPLES_UNAVAILABLE;
  } 

  // Check the sample offset and sample size.
  if( (data->sample_offset + data->sample_size) > acq_sample_ptr->req_samples)
  {
    FTM_MSG_ERROR( "Sample offset or Sample size is wrong %d %d",data->sample_offset,data->sample_size,0);      
    get_samples_rsp_pkt_ptr->ftm_error_code = FTM_IQ_ILLEGAL_COMBIMATION;	 
  }

  // check if requested number of samples > Max. value that can be sent through Diag
  if( data->sample_size > FTM_CDMA_IQ_DIAG_MAX_SAMPLES ) 
  {
    FTM_MSG_ERROR( "Requested Sample size %d is greater than %d",data->sample_size,FTM_CDMA_IQ_DIAG_MAX_SAMPLES,0);	   
    get_samples_rsp_pkt_ptr->ftm_error_code= FTM_IQ_GENERAL_FAILURE;  
  }

  // Since all the sanity checks have passed, copy the data on to diag RSP packet and adjust its length.  
  if(get_samples_rsp_pkt_ptr->ftm_error_code == FTM_IQ_SUCCESS) 
  {
   // Copy the IQ data into diag command buffer 
   // Extract the I and Q information from acquired IQ buffer [sample_offset,sample_offset+sample_number) and copy it to diag in the format IQIQIQ
   for(sample_number = data->sample_offset,counter=0; sample_number < ( data->sample_size + data->sample_offset ) ;sample_number++,counter+=2)
   {
     get_samples_rsp_pkt_ptr->sample_buffer.sample_buffer_8_bit[counter] 
        = (int8)((acq_sample_ptr->iq_buf[sample_number] & 0xFF00) >> 8);
     get_samples_rsp_pkt_ptr->sample_buffer.sample_buffer_8_bit[counter+1] 
	 	= (int8)(acq_sample_ptr->iq_buf[sample_number] & 0xFF); 
   }

   // Length of the response packet = Header + No of samples * 2 bytes (one for I and Q)
   rsp_pkt_ptr->pkt_len = FTM_IQ_CAPTURE_GET_SAMPLE_HDR_SIZE + 2 * data->sample_size; // Header size +   (I + Q , each 1 byte) * total number ;
   FTM_MSG_2(FTM_HIGH,"%d CDMA IQ samples sent to Diag with sample offset %d",data->sample_size,data->sample_offset);
  }
  else
  {
    rsp_pkt_ptr->pkt_len = FTM_IQ_CAPTURE_GET_SAMPLE_HDR_SIZE;  
    FTM_MSG_2(FTM_ERROR,"Unable to capture IQ samples for CDMA for sample offset %d sample size %d",
		                data->sample_offset,data->sample_size);
  }
  
}


/*============================================================================

FUNCTION FTM_1X_CREATE_IQ_CAPTURE_GET_SAMPLES_RSP_DIAG_PKT

DESCRIPTION
  This API is create RSP packet and fills in the basic information

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
 None
=============================================================================*/

void ftm_1x_create_iq_capture_get_samples_rsp_diag_pkt
(
  rfm_device_enum_type device,
  ftm_rsp_pkt_type *rsp_pkt
)
{
  // Get pointer to response packet
  ftm_iq_capture_get_samples_rsp_pkt_type *rsp_pkt_paylaod;

  rsp_pkt_paylaod=&(ftm_cdma_get_rsp_pkt_data_ptr()->iq_capture_rsp);
  
  rsp_pkt->cmd=FTM_RSP_DO_NEW_PKT;
  rsp_pkt->pkt_payload=(void*)rsp_pkt_paylaod;
  rsp_pkt->delete_payload = FALSE;

  // Fill in the basic fields of the payload
  rsp_pkt_paylaod->cmd_code = 75;
  rsp_pkt_paylaod->subsys_id = FTM;  
  rsp_pkt_paylaod->subsys_cmd_code = (device == RFM_DEVICE_0) ? FTM_1X_C0_C : FTM_1X_C1_C ;
  rsp_pkt_paylaod->ftm_rf_cmd = FTM_IQ_CAPTURE_GET_SAMPLES;  
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Register FTM related 1x FW MSG responses with FWRSP Task

  @details
  This function check if 1x fw msgs is qualified for registration, if it's qualified,
  it calls rfm layer to register 1x fw msgs. 
 
*/
void
ftm_1x_register_fw_msgs
(
  void
)
{
  rfm_device_enum_type dev_idx;  /* Device for Looping */
  ftm_rfstate_enum_type ftm_rfmode; /* Current FTM RF Mode */
  uint8 counter = 0;   /* Counter for 1x mode in all devices */

  /* Loop all device to check if none of them has rfmode in 1x, */
  for ( dev_idx = RFM_DEVICE_0; dev_idx < RFM_MAX_DEVICES; dev_idx++ )
  {
    /* Get rfmode of each device */
    ftm_rfmode = ftm_get_current_state(dev_idx);
    /* If it's in 1x mode, increase counter by 1*/
    if ( ftm_rfmode == FTM_STATE_1X || ftm_rfmode == FTM_STATE_1X_NS ) 
    {
       counter++;
       break;
    }
  }

  /* If counter is 0, register fw msgs */
  if ( counter == 0 ) 
  {
    /*Register 1x fw messages */
    rfm_1x_ftm_register_fw_msgs();
    FTM_MSG( FTM_LOW, "ftm_1x_register_fw_msgs: 1x FW msgs is registered "
               "successfully. ");
  }
  else
  {
    FTM_MSG( FTM_LOW, "ftm_1x_register_fw_msgs: 1x FW msgs is already "
               "registered, no more 1x msgs registration is needed. " );
  }  

}/*ftm_1x_register_fw_msgs*/    

/*----------------------------------------------------------------------------*/
/*!
  @brief
  De-register FTM related 1x FW MSG responses with FWRSP Task

  @details
  This function check if 1x fw msgs is qualified for de-registration, if it's
  qualified, it calls rfm layer to de-register 1x fw msgs. 

*/
void
ftm_1x_deregister_fw_msgs
(
  void
)
{
  rfm_device_enum_type dev_idx;  /* Device for Looping */
  ftm_rfstate_enum_type ftm_rfmode; /* Current FTM RF Mode */
  uint8 counter = 0;   /* Counter for 1x mode in all devices */

  /* Loop all device to check if input device is the only device
  whose rfmode is 1x, */
  for ( dev_idx = RFM_DEVICE_0; dev_idx < RFM_MAX_DEVICES; dev_idx++ )
  {
    /* Get rfmode of each device */
    ftm_rfmode = ftm_get_current_state(dev_idx);
    /* If it's in 1x mode, increase counter by 1*/
    if ( ftm_rfmode == FTM_STATE_1X || ftm_rfmode == FTM_STATE_1X_NS ) 
    {
       counter++;
    }
  }

  /* If counter is 1, de-register fw msgs */
  if ( counter == 1 ) 
  {
    /*De-register 1x fw messages */
    rfm_1x_ftm_deregister_fw_msgs();
    FTM_MSG( FTM_LOW, "ftm_1x_deregister_fw_msgs: 1x FW msgs is de-registered "
               "successfully. " );
  }
  else
  {
    FTM_MSG( FTM_LOW, "ftm_1x_deregister_fw_msgs: 1x FW msgs can't be "
      "de-registered, other device is still in 1x mode. " );
  }  

}/*ftm_1x_deregister_fw_msgs*/   



/*----------------------------------------------------------------------------*/
/*!
  @brief
  Re-tune Antenna tuner by updating tune code
  

  @details
  This function is called to execute dynamical re-tuning on all active antenna
  tuner devices in 1x mode 
 
  @return
  Status of execution completion of the function execution

*/
boolean
ftm_1x_antenna_tuner_update_tune_code
(
  void
)
{
   int32 ret_val; 
   ret_val = rfm_1x_ftm_antenna_tuner_update_tune_code();

  /* If rfm_1x_ftm_antenna_tuner_update_tune_code return bad state */
  if ( ret_val != RFM_CDMA_HEALTHY_STATE ) 
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_antenna_tuner_update_tune_code: "
               "Antenna tuner update tune code failed" );
    return FALSE;
  }
  else
  {
    return TRUE;
  }  

}/* ftm_1x_antenna_tuner_update_tune_code */   

/*============================================================================

FUNCTION FTM_1X_FBRX_MEAS_TX_PARAMS

DESCRIPTION
  This API is used to capture and process Tx IQ samples using FBRx and to calculate
  various Tx parameters (i.e. Tx Power, EVM and ACLR)

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/

void ftm_1x_fbrx_meas_tx_params
(
   rfcom_device_enum_type device,
   uint16 num_averages,
   uint32 capture_offset,
   uint32 tx_measurement_config,
   ftm_rsp_pkt_type *rsp_pkt_ptr
   )
{

  rf_time_tick_type prof_tick;
  rf_time_type prof_time = -1;
  ftm_1x_selftest_meas_params_struct params;
  uint8 selftest_error;

  prof_tick = rf_time_get_tick();

  params.device = device;
  params.num_averages = num_averages;
  params.capture_offset = capture_offset;
  params.tx_measurement_config = tx_measurement_config;

  selftest_error = ftm_1x_selftest(
                               params,
                               rsp_pkt_ptr,
                               ftm_rf_fbrx_get_iq_acquired_samples_data_ptr(),
                               &ftm_rf_fbrx_meas_tx_params_rsp_pkt,
                               NULL, // &ftm_selftest_pwr_only,
                               0); //use_iq_rms_for_pwr_meas);

  if (!selftest_error)
  {
    prof_time = rf_time_get_elapsed( prof_tick, RF_MSEC );
    FTM_MSG_1( FTM_HIGH, "Self_test Time profiling 1x  "
                           "ExecTime %d ms", prof_time );
  }
  else
  {
    FTM_MSG( FTM_HIGH, "Self_test failed");
  }

}

/*============================================================================

FUNCTION FTM_FBRX_IQ_CAPTURE_TRIGGER

DESCRIPTION
  This command acquires one set of IQ samples and returns key parameters used for
  collecting Rx and TX IQ samples. The samples collected in on acquisition will be
  contiguous. Sampels from different acquisitions are not time coherent.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/
void ftm_1x_fbrx_iq_capture_trigger
(
   rfcom_device_enum_type device,
   uint8 sample_size,
   uint8 fbrx_chain,
   ftm_rsp_pkt_type *rsp_pkt
   )
{
   ftm_fbrx_test_error_code_type error_code = FTM_FBRX_TEST_SUCCESS;
   ftm_rf_cdma_fbrx_iq_acquired_samples_data_type *ftm_rf_fbrx_iq_acquired_samples_data_ptr = ftm_rf_fbrx_get_iq_acquired_samples_data_ptr();
   boolean capture_done;
   uint16 sample_capture_size = 9216;
   uint32 samp_rate = -1;
   ftm_common_selftest_fbrxgs_info_type fbrx_gs_info;

//  FTM_MSG_1(FTM_MED, "Preparing for the response packet - FTM_FBRX_IQ_CAPTURE_TRIGGER",0);

   // Update the strucutre with requested parameters. Start with data_available to be FALSE and change it to TRUE only when are able
   // acquire it sucessfully.
   fbrx_gs_info.override_enable = FALSE;
   ftm_rf_fbrx_iq_acquired_samples_data_ptr->data_available = FALSE;
   ftm_rf_fbrx_iq_acquired_samples_data_ptr->device = device;

   switch (sample_size)
   {
   case FTM_FBRX_SAMPLE_SIZE_1024:
      ftm_rf_fbrx_iq_acquired_samples_data_ptr->req_samples = 1024; break;
   case FTM_FBRX_SAMPLE_SIZE_2048:
      ftm_rf_fbrx_iq_acquired_samples_data_ptr->req_samples = 2048; break;
   case FTM_FBRX_SAMPLE_SIZE_3072:
      ftm_rf_fbrx_iq_acquired_samples_data_ptr->req_samples = 3072; break;
   case FTM_FBRX_SAMPLE_SIZE_4096:
      ftm_rf_fbrx_iq_acquired_samples_data_ptr->req_samples = 4096; break;
   case FTM_FBRX_SAMPLE_SIZE_5120:
      ftm_rf_fbrx_iq_acquired_samples_data_ptr->req_samples = 5120; break;
   case FTM_FBRX_SAMPLE_SIZE_6144:
      ftm_rf_fbrx_iq_acquired_samples_data_ptr->req_samples = 6144; break;
   case FTM_FBRX_SAMPLE_SIZE_7168:
      ftm_rf_fbrx_iq_acquired_samples_data_ptr->req_samples = 7168; break;
   case FTM_FBRX_SAMPLE_SIZE_8192:
      ftm_rf_fbrx_iq_acquired_samples_data_ptr->req_samples = 8192; break;
   case FTM_FBRX_SAMPLE_SIZE_9216:
      ftm_rf_fbrx_iq_acquired_samples_data_ptr->req_samples = 9216; break;
   default:
      ftm_rf_fbrx_iq_acquired_samples_data_ptr->req_samples = 9216;
      error_code = FTM_FBRX_TEST_IQ_SAMPLES_EXCEEDS_CAPTURE_LIMIT;
      FTM_MSG_1(FTM_ERROR, "Number of samples requested not supported", sample_size);
      break;
   }

   if( ftm_get_current_state(device) != FTM_STATE_1X)
   {
     capture_done = FALSE;
   }
   else
   {
     /* Trigger I/Q capture*/
     capture_done = ftm_common_fbrx_iq_capture_processing(
                                       FTM_RF_TECH_CDMA,
                                       fbrx_chain,
                                       FALSE,
                                       sample_capture_size,
                                       &(ftm_rf_fbrx_iq_acquired_samples_data_ptr->iq_buf_rx),
                                       &(ftm_rf_fbrx_iq_acquired_samples_data_ptr->iq_buf_tx),
                                       NULL,
                                       NULL,
                                       &samp_rate,
                                       &fbrx_gs_info);
   }
   
   if (capture_done == FALSE)
   {
      error_code = FTM_FBRX_TEST_IQ_SAMPLES_UNAVAILABLE;
   }
   else
   {
      ftm_rf_fbrx_iq_acquired_samples_data_ptr->data_available = TRUE;
   }

   // Fill in the constant fields of response packet. Sample buffer will be filled after sanity check.
   ftm_rf_fbrx_iq_capture_trigger_rsp_pkt.cmd_code = 75;
   ftm_rf_fbrx_iq_capture_trigger_rsp_pkt.subsys_id = FTM;
   ftm_rf_fbrx_iq_capture_trigger_rsp_pkt.subsys_cmd_code = (device == RFM_DEVICE_0) ? FTM_1X_C0_C : FTM_1X_C2_C; 
   ftm_rf_fbrx_iq_capture_trigger_rsp_pkt.ftm_rf_cmd = FTM_FBRX_IQ_CAPTURE_TRIGGER;

   // Update the final error code in response packet.-
   ftm_rf_fbrx_iq_capture_trigger_rsp_pkt.ftm_error_code = error_code;
   ftm_rf_fbrx_iq_capture_trigger_rsp_pkt.sample_data_format_tx = FTM_CDMA_FBRX_IQ_SAMPLE_DATA_FORMAT_TX;
   ftm_rf_fbrx_iq_capture_trigger_rsp_pkt.sample_data_format_rx = FTM_CDMA_FBRX_IQ_SAMPLE_DATA_FORMAT_RX;
   ftm_rf_fbrx_iq_capture_trigger_rsp_pkt.sample_size = sample_size;
   ftm_rf_fbrx_iq_capture_trigger_rsp_pkt.fbrx_chain = fbrx_chain;
   ftm_rf_fbrx_iq_capture_trigger_rsp_pkt.sampling_frequency = FTM_CDMA_FBRX_IQ_SAMPLE_FREQ;
   ftm_rf_fbrx_iq_capture_trigger_rsp_pkt.max_diag_sample_size_tx = FTM_CDMA_FBRX_IQ_DIAG_MAX_SAMPLES_TX;
   ftm_rf_fbrx_iq_capture_trigger_rsp_pkt.max_diag_sample_size_rx = FTM_CDMA_FBRX_IQ_DIAG_MAX_SAMPLES_RX;

   // Fill in the constant fields of response packet.Size of the packet will be based on the requested sample size.
   rsp_pkt->cmd = FTM_RSP_DO_NEW_PKT;
   rsp_pkt->pkt_payload = (void *)(&ftm_rf_fbrx_iq_capture_trigger_rsp_pkt);
   rsp_pkt->delete_payload = FALSE;
   rsp_pkt->pkt_len = sizeof(ftm_rf_fbrx_iq_capture_trigger_rsp_pkt); /*ToDo - pkt_len size should be computed based on measurements enabled in tx_measurement_config*/

   FTM_MSG_3(FTM_MED, "Inputs to IQ Capture trigger -> device: %d :sample_size : %d : fbrx_chain : %d",
             device, sample_size, fbrx_chain);
}

/*============================================================================

FUNCTION FTM_FBRX_IQ_CAPTURE_GET_SAMPLES

DESCRIPTION
  This API is used to get Tx and/or Rx IQ samples for cdma from buffer that has been used to capture samples from ACQUIRE SAMPLES. 

DEPENDENCIES
 FTM_FBRX_IQ_CAPTURE_TRIGGER command should have been issued before executing get sample command.

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/

void ftm_1x_fbrx_iq_capture_get_samples
(
   rfcom_device_enum_type device,
   uint8 sample_source,
   uint32 sample_offset,
   uint32 sample_size,
   ftm_rsp_pkt_type *rsp_pkt
   )
{
   ftm_rf_cdma_fbrx_iq_acquired_samples_data_type *ftm_rf_fbrx_iq_acquired_samples_data_ptr = ftm_rf_fbrx_get_iq_acquired_samples_data_ptr();

   //ftm_lte_pkt_type *lte_pkt_ptr;
   uint8 status = FTM_FBRX_TEST_SUCCESS;

   uint16 sample_buffer_size;
   uint16 data_byte_size;
   uint16 data_byte_offset;

   int32 *iq_buf_rx_ptr = NULL;
   int32 *iq_buf_tx_ptr = NULL;

//  FTM_MSG_1(FTM_MED, "Preparing for the response packet - FTM_FBRX_IQ_CAPTURE_TRIGGER",0);

   /***************** Do sanity checks *******************************/
   /*Check if IQ has been acquired for the same device*/
  if (ftm_rf_fbrx_iq_acquired_samples_data_ptr->device != device)
  {
    FTM_MSG( FTM_ERROR, "IQ samples are not acquired for this device, please acquire them first");	  
	  status = FTM_FBRX_TEST_IQ_SAMPLES_UNAVAILABLE;    
  }
  else
  {
    /*Check if data is available */
    if (ftm_rf_fbrx_iq_acquired_samples_data_ptr->data_available == FALSE )
    {
      FTM_MSG( FTM_ERROR, "IQ samples are not acquired, please acquire them first");      
      status = FTM_FBRX_TEST_IQ_SAMPLES_UNAVAILABLE;
    }
    else
    {
      /* Check if number of samples requested is greater than acquired.*/
      if((sample_offset + sample_size)> ftm_rf_fbrx_iq_acquired_samples_data_ptr->req_samples)
      {
        FTM_MSG_2( FTM_ERROR, "Sample offset(%d) or Sample size(%d) is wrong", sample_offset, sample_size);      
        status =  FTM_FBRX_TEST_ILLEGAL_COMBIMATION;	         
      }
      else
      {
        /*Check if the requested sample source is correct*/
        if ((sample_source != FTM_FBRX_IQ_SAMPLE_TX) 
            &&(sample_source != FTM_FBRX_IQ_SAMPLE_RX))
        {
          FTM_MSG_1( FTM_ERROR, "Requested sample source is not valid",sample_source);	   
          status =  FTM_FBRX_TEST_GENERAL_FAILURE;            
        }
        else
        {
          /*check Tx request for max sample size and memory alignment requirements*/
          if(sample_source == FTM_FBRX_IQ_SAMPLE_TX)
          {
            if(sample_size > FTM_CDMA_FBRX_IQ_DIAG_MAX_SAMPLES_TX) 
          { 
            FTM_MSG_3(FTM_ERROR, "Requested Sample size %d is greater than %d for sample source %d",
                           sample_size,
                           FTM_CDMA_FBRX_IQ_DIAG_MAX_SAMPLES_TX,
                           sample_source);	   

            status =  FTM_FBRX_TEST_GENERAL_FAILURE;              
          } 
            if(sample_size & 0x01) 
            { 
              FTM_MSG_2(FTM_ERROR, "Requested Sample size %d has to be an even number for source %d",
                             sample_size,
                             sample_source);	   
              status =  FTM_FBRX_TEST_GENERAL_FAILURE;              
            }
          }
          /*check Rx request for max sample size and memory alignment requirements*/
          else if(sample_source == FTM_FBRX_IQ_SAMPLE_RX)
          {
            if(sample_size > FTM_CDMA_FBRX_IQ_DIAG_MAX_SAMPLES_RX) 
          { 
            FTM_MSG_3( FTM_ERROR, "Requested Sample size %d is greater than %d for sample source %d",
                       sample_size,
                       FTM_CDMA_FBRX_IQ_DIAG_MAX_SAMPLES_RX,
                       sample_source);	   
              status = FTM_FBRX_TEST_GENERAL_FAILURE;              
            }
            if(sample_size & 0x07) 
            { 
              FTM_MSG_2( FTM_ERROR, "Requested Sample size %d has to be multiple of 8 for source %d",
                         sample_size,
                         sample_source);	   
            status = FTM_FBRX_TEST_GENERAL_FAILURE;              
            }
          }
          else
          {
            FTM_MSG_1( FTM_ERROR, "Requested Sample type is not suuported %d", sample_source);
            status = FTM_FBRX_TEST_ILLEGAL_COMBIMATION;    
          }

          /*if all sanity check passed, go ahead witrh sample dispatch*/
          if (status == FTM_FBRX_TEST_SUCCESS)
          {
            if(sample_source == FTM_FBRX_IQ_SAMPLE_TX)
            {
              data_byte_offset = sample_offset*2;
              iq_buf_tx_ptr = ftm_rf_fbrx_iq_acquired_samples_data_ptr->iq_buf_tx + (data_byte_offset/sizeof(int32));

              /* Buffer for samples and FFT - 8 bit I/8 bit Q*/
              sample_buffer_size = (sample_size * 2);

			  /* If pointer is NULL even after allocating memory, then declare it as failure*/
              if(ftm_rf_fbrx_iq_acquired_samples_data_ptr->iq_buf_tx_rsp_data == NULL)
              {
                ftm_rf_fbrx_iq_acquired_samples_data_ptr->iq_buf_tx_rsp_data = (int32*)ftm_malloc(FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG);
              }

			  /* If pointer is NULL even after allocating memory, then declare it as failure*/
              if(ftm_rf_fbrx_iq_acquired_samples_data_ptr->iq_buf_tx_rsp_data == NULL)
              {
                status = FTM_FBRX_TEST_MEM_ALLOCATION_FAILURE;	  
                FTM_MSG(FTM_ERROR, "Unable to allocate memory for Tx - IQ buffer");
              }
              else
              {
                ftm_rf_fbrx_iq_acquired_samples_data_ptr->tx_iq_data_siz_bytes = sample_buffer_size;

                /*get sample_size samples from the Rx buffer*/
                memscpy((void *)ftm_rf_fbrx_iq_acquired_samples_data_ptr->iq_buf_tx_rsp_data, FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG, (void *)iq_buf_tx_ptr, sample_buffer_size);

                /* Parse the Tx data*/
                ftm_common_parse_tx_IQ(FTM_STATE_1X, ftm_rf_fbrx_iq_acquired_samples_data_ptr->iq_buf_tx_rsp_data, sample_size);
              }
            }
            else if(sample_source == FTM_FBRX_IQ_SAMPLE_RX)
            {
              data_byte_offset = (sample_offset*20)/8;
              data_byte_size   = (sample_size*20)/8;
              iq_buf_rx_ptr = ftm_rf_fbrx_iq_acquired_samples_data_ptr->iq_buf_rx + (data_byte_offset/sizeof(int32));
			  
              /* Buffer for samples and FFT - 32 bit I/32 bit Q*/
              sample_buffer_size = (sample_size * 2 * 4);

              if(ftm_rf_fbrx_iq_acquired_samples_data_ptr->iq_buf_rx_rsp_data == NULL)
              {
                ftm_rf_fbrx_iq_acquired_samples_data_ptr->iq_buf_rx_rsp_data = (int32*)ftm_malloc(FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG);
              }
			  
              /* If pointer is NULL even after allocating memory, then declare it as failure*/
              if(ftm_rf_fbrx_iq_acquired_samples_data_ptr->iq_buf_rx_rsp_data == NULL)
              {
                status = FTM_FBRX_TEST_MEM_ALLOCATION_FAILURE;	  
                FTM_MSG(FTM_ERROR, "Unable to allocate memory for Rx - IQ buffer");
              }
              else
              {
                ftm_rf_fbrx_iq_acquired_samples_data_ptr->rx_iq_data_siz_bytes = sample_buffer_size;

                /*get sample_size samples from the Rx buffer*/
                memscpy((void *)ftm_rf_fbrx_iq_acquired_samples_data_ptr->iq_buf_rx_rsp_data, FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG, (void *)iq_buf_rx_ptr, data_byte_size);
			  
                /* Parse the data*/
              ftm_common_parse_PCFL20_to_IQ(FTM_STATE_1X, ftm_rf_fbrx_iq_acquired_samples_data_ptr->iq_buf_rx_rsp_data, sample_size);
              }
            }
            else
            {
              status = FTM_FBRX_TEST_ILLEGAL_COMBIMATION;
            }
          }
        }         
      }
    }
  }

   /**************** Fill the packet after check ************* */
  if (rsp_pkt != NULL)
  {    
     /***************** Fill up RSP packet with standard data *****************/

     // Fill in the constant fields of response packet. Sample buffer will be filled after sanity check.
     ftm_rf_fbrx_iq_capture_get_samples_rsp_pkt.cmd_code = 75;
     ftm_rf_fbrx_iq_capture_get_samples_rsp_pkt.subsys_id = FTM;
     ftm_rf_fbrx_iq_capture_get_samples_rsp_pkt.subsys_cmd_code = (device == RFM_DEVICE_0) ? FTM_1X_C0_C : FTM_1X_C1_C;
     ftm_rf_fbrx_iq_capture_get_samples_rsp_pkt.ftm_rf_cmd = FTM_FBRX_IQ_CAPTURE_GET_SAMPLES;

     ftm_rf_fbrx_iq_capture_get_samples_rsp_pkt.ftm_error_code = status;
     ftm_rf_fbrx_iq_capture_get_samples_rsp_pkt.sample_source = sample_source;
     ftm_rf_fbrx_iq_capture_get_samples_rsp_pkt.sample_offset = sample_offset;
     ftm_rf_fbrx_iq_capture_get_samples_rsp_pkt.sample_size = sample_size;

     // Fill in the constant fields of response packet.Size of the packet will be based on the requested sample size.
     rsp_pkt->cmd = FTM_RSP_DO_NEW_PKT;
     rsp_pkt->pkt_payload = (void *)(&ftm_rf_fbrx_iq_capture_get_samples_rsp_pkt);
     rsp_pkt->delete_payload = FALSE;
    /*******Fill up remaining part of RSP packet based on sanity checks and acquired data***********/
    if (status == FTM_FBRX_TEST_SUCCESS)
    {
      // Since all the error checks were a success, copy the IQ data into diag command buffer 
      // Extract the I and Q information from acquired IQ buffer [sample_offset,sample_offset+sample_number) and copy it to diag in the format IQIQIQ     
      if (sample_source == FTM_FBRX_IQ_SAMPLE_TX)
      {                

        memscpy(ftm_rf_fbrx_iq_capture_get_samples_rsp_pkt.sample_buffer.sample_buffer_8_bit,
               FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG,			
               ftm_rf_fbrx_iq_acquired_samples_data_ptr->iq_buf_tx_rsp_data,
               ftm_rf_fbrx_iq_acquired_samples_data_ptr->tx_iq_data_siz_bytes); 

        // Header size +   (I + Q , each 1 byte) * total number of samples
        rsp_pkt->pkt_len = FTM_RF_FBRX_IQ_CAPTURE_GET_SAMPLES_HDR_SIZE + ftm_rf_fbrx_iq_acquired_samples_data_ptr->tx_iq_data_siz_bytes;
        FTM_MSG_3(FTM_MED, "%d CDMA IQ samples (Source = %d ) sent to Diag with sample offset %d",
                  sample_size,
                  sample_source,
                  sample_offset);
      }               
      if (sample_source == FTM_FBRX_IQ_SAMPLE_RX)
      {                 

        memscpy(ftm_rf_fbrx_iq_capture_get_samples_rsp_pkt.sample_buffer.sample_buffer_32_bit,
               FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG,			
               ftm_rf_fbrx_iq_acquired_samples_data_ptr->iq_buf_rx_rsp_data,
               ftm_rf_fbrx_iq_acquired_samples_data_ptr->rx_iq_data_siz_bytes);

        // Header size +   (I + Q , each 2 byte) * total number of samples 
        rsp_pkt->pkt_len = FTM_RF_FBRX_IQ_CAPTURE_GET_SAMPLES_HDR_SIZE + ftm_rf_fbrx_iq_acquired_samples_data_ptr->rx_iq_data_siz_bytes;
        FTM_MSG_3(FTM_MED, "%d CDMA IQ samples (Source = %d ) sent to Diag with sample offset %d",
                  sample_size,
                  sample_source,
                  sample_offset);               
      }   
    }
    // If we fail checks, dont fill IQ samples. RSP packet will only have header.
    else
    {
      rsp_pkt->pkt_len = FTM_RF_FBRX_IQ_CAPTURE_GET_SAMPLES_HDR_SIZE;
      FTM_MSG_2(FTM_ERROR, "CDMA FBRx get IQ samples failed for SAMPLE SIZE %d and SAMPLE OFFSET %d",
                sample_size,
                sample_offset);
    }
  }
}

#endif /* FEATURE_CDMA1X */
#endif /* FEATURE_FACTORY_TESTMODE */

/*! @} */


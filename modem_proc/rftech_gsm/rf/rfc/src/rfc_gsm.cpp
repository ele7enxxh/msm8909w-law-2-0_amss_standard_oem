/*!
   @file
   rfc_gsm.c

   @brief
   This file contains all the GSM mode specific implementations to support RFC.
   These are platform independent.

*/

/*===========================================================================

Copyright (c) 2008 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:51 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/rf/rfc/src/rfc_gsm.cpp#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
06/08/16   bp      Put the QPOET to standby mode after GSM Tx burst ends 
03/05/15   vv      API support for rfc to capture modem chain to trx and VCO path mapping 
01/28/15   piy     Put ASM and tuner devices to sleep for all 
                   bands per device
01/19/15   sw      Use tuner start delta to program XSW
01/16/15   par     Optimize ASM disable calls during sleep
01/12/15   sml     Changes to remove un-necessary tuner transactions 
                   during probe burst
01/07/15   sml     Fix a bug in PAPM driver that prevented 
                   slot based bias programming
12/16/14   sml     Fixing KW and device instances to sleep logic 
12/16/14   sml     Put device instances to sleep only once 
12/15/14   sml     Rewrite ASD API to reduce wake-up timeline
12/08/14   zbz     Fix KW
12/02/14   zbz     Make rfc gsm timing ptr per device
11/19/14   dyc     Support updating rfc_asdiv_config_info table from RFC info
11/18/14   zbz     Add critical section mechanism for ASM driver
11/12/14   sw      Rename RX/TX script types to reflect actual devices
10/23/14   sc      Enable event type control from RFSW
10/21/14   yb      Added priority configuration to config Tuners during CRAT scenarios
10/17/14   rr      Separate rx/tx alternate path core handles
10/21/14   px      Added RFM Device support for PAPM
10/15/14   sw      Clear tuner rx stop script
10/14/14   sw      Use current band to get asdiv switch info from RFC
10/02/14   sw      Passing correct tuner instance for Prx device for
                   ASD supported RF cards
09/28/14   ch      Configure TX tuner device during init
09/24/14   hoh     Add multislot APT control feature
09/23/14   sw      Initialize the tuners during Rx burst so that during iRAT 
                   correct tune scripts are updated. Also acknowlege the scripts 
                   of tuner for config/Rx and Tx modes
09/23/14   sw      Update tuner state so that in case of ASDiv probe burst
                   coming after a Tx burst we will have the tuner settings
                   for the probe burst updated correctly.
09/23/14   sw      Clearing ASD scripts as part of rx burst config
09/16/14   sn      ASM set band RFM Device support
09/09/14   ch      Mainline FEATURE_RF_HAS_QTUNER 
08/26/14   zbz     Reduce number of tuner writes in ASDiv
08/22/14   rr      Changes to support nv c3
08/14/14   zbz     Add SMPS/ICQ/temp_rem to FTM log packet
08/07/14   ggs     Fix KW error: Do not update band support map for 
                   alternate path selection.
07/31/14   tsr     GSM Alternate path support
07/31/14   sw      Support both rffe and grfc-based xsw for ASD
07/24/14   cj      Added changes for TX sharing feature
07/25/14   sb      bug fixes for V1 IP2 cal functionality
07/28/14   hk      Init each device instance only once during enter mode/IRAT to reduce timeline
07/28/14   hk      Init each device instance only once during wakeup and do 
                   create scripts for CCS to reduce wakeup timelines
07/18/14   sn      ASM wakeup API is now script based instead of immediate
07/18/14   ch/tws  Set the antenna tuner manager port correctly
07/18/14   ch/sg   Optimize F3's for timeline saving
07/18/14   dr      [RUMI] Changes for JOLOKIA RUMI bringup
07/09/14   ch      clean up redundant Tuner init during Tx burst device config
07/08/14   tsr     DR-DSDS: Add worst case rx alpha for IDLE mode RX
07/03/14   av      Fixing the ASM writes issue and featurization for DPM
07/03/14   vv      Updated to new physical and logical device config structures from RFC
07/03/14   hk      ASM writes to create script instead of immediate. 
07/01/14   ch      Add Tuner manager tx_disable 
06/26/14   ch      Pass txlm_buf_index to tx_burst_device_config (for CL tuner initialisation)
06/26/14   sw      Fix ASD KW error
06/19/14   sb      changes to support cal on c2 for DSDA
06/02/14   sml     Adding necessary check condition in the case where tuners are not present 
                   and ASD switch is present to avoid un-necessary prints
06/06/14   ggs     Fix bug that was causing pa_start_adj NV value to change 
                   the shape of the falling edge.
06/06/14   sb      Reduce GSM heap memory usage
05/16/14   sn      Updated ASM call flow for sleep wakeup
05/12/14   kab     Remove RFCOM_RECEIVER_DUAL Support
05/05/14   sn      Added ASM sleep wakeup callflow
05/01/14   lcl     Klocwork error fixes
04/29/14   sc      Manage Vregs based on subscription
04/29/14   tsr     Mainline GSM RFC config v2 apis
04/24/14   ch      Removed tuner script from tx_stop
04/24/14   sw      Introduce rffe-based api for ASD processing
04/22/14   ch      init tuner object before set_tx_mode
04/14/14   sw      Disable tuners during X2G iRAT scenarios (port from DI3)
04/10/14   zbz     Fix KW
04/09/14   sn      Updated PA interface
04/07/14   sc      Vote vregs based only on single GSM tech
04/03/14   kg      Adding correct featurization for ASDiv
04/01/14   sw      Port from Dime3 (Re-design of asdiv to support multiple configurations) - reintroduced
03/31/14   yb      Added support for Tuner linearizer
03/28/14   kg      Added support for CL LM handle
03/28/14   hoh     Revert back CL5565452 and 5565810 due to boot up failure
03/28/14   sw      Fix compiler error
03/26/14   sw      Port from Dime3 (Re-design of asdiv to support multiple configurations)
03/20/14   sw      Introduce api to get signal-specific timing info
03/19/14   hoh     Added support for 8 PA gain states
03/19/14   kg      Add API get_rf_device_list- Merge from Dime ASD code changes
03/18/14   tsr     Fix for populating cascaded ASMs on SSKU
03/14/14   zbz     Fix bug in feature to control PA BIAS through NV item
03/10/14   zbz     Add feature to control PA(ICQ) BIAS through NV item
03/05/14   tsr     Program device scripts dynamically through CCS
02/25/14   tsr     Update RX PLL setting time from device driver to rfgsm core rx timing struct
02/24/14   tsr     Mainline Device driver script retrieval
02/11/14   vv      backedout the previous change
02/06/14   sg      Handle the RFC_DEVICE_MISSING_FATAL scenario for RF daughter card support
12/13/13   cc      Unify gpio type to rfc_sig_tdd_cfg_type for FDD and TDD tech
11/12/13   dps     Update init_rf_devices for C++ GSM transceiver conversion
10/25/13   tsr     Fix for appending APT Script to TX Non-sbi start script
10/25/13   sg      Fix KW error
10/24/13   sg      Perform tuner init for other tuner only if card supports ASDiv
10/23/13   tsr     Mainline D3295 changes for BOLT
10/23/13   sg      GSM ASDiv Tuner Support
09/27/13   tsr     Added support to retrieve individual device scripts
09/25/13   sb      Add NV hooks to GRFC PA range map
09/24/13   tws     Send acknowledgement to the tuner manager
09/05/13   nc      Cleanup code for manage_vregs
08/28/13   sml     Extracting ASD script depending on device id through
                   call back data
08/27/13   sb      Add changes to update RX timing for GSM RXD
08/21/13   cpm     backout - antenna tuner low power mode implementation - IRAT issues
08/12/13   cpm     antenna tuner low power mode implementation
08/12/13   sg      Cleanup code to handle all device types
08/07/13   cpm     atuner init() scripted into SBI events
08/01/13   sb      Added interface chanegs to support GSM RX diversity
07/29/13   sml/svi ASD support for GSM tech
07/29/13   tws     Use new RF_TASK_1 for GSM.
07/22/13   sn      Modified PAPM APT settings in tx band device config
07/19/13   tws     Only init PRX/TX logical devices until all device drivers
                   support GSM.
07/18/13   svi     Adding [QTUNER][RL] GSM NV Support.
07/07/13   sbo     Added BW support for PA device
07/03/13   vbh     Missing trigger call from PA API call
06/12/13   kg      Loop through all bands to init() tuner device
6/20/13    vbh     Added explicit trigger for the PA device
06/20/13   sb      Add hooks to populate Spur channel list from RFC
06/17/13   svi     "TX_GTR_THRES" NV values interpretation fix.
05/30/13   tws     Add support for GSM sub 3
05/30/13   tws     Mainline the context checking.
05/08/13   tws     Support DUAL_SIM for power & HAL voting
05/06/13   vb      Support for Tuner manager factory
05/02/13   vb      KW Fix: Check for Tuner Manager validity
05/02/13   vb      Tuner CA changes
04/10/13   sn      Cleanup PA/ASM interface
04/08/13   tc      Add timing hooks for QPOET and QTUNER
04/02/13   tsr     Changed rf_sigma_delta_max_db value as per MSM8974
04/02/13   sb      Fix Null check in init rf devices
04/01/13   par     Update PA range map table for Triton settings
04/01/13   sb      RFC gsm cleanup
03/27/13   sb      Added support for Enhanced APT
03/25/13   ec      Replace deprecated RFM_MAX_DEVICES with RFM_MAX_WAN_DEVICES
03/22/13   sr      changes to PAPM device interface.
03/20/13   sb      Fix timing passed in set PA range API
03/18/13   sb      Removed GRFCstart/stop script update from Set Tx band
03/18/13   sb      Fix PA control NV timing hook for all PA devices
03/13/13   ec        Disable QPOET with Tx, and move enable to Tx enable.
03/07/13   sr      changed the "pwr_tracker" name to "papm".
03/05/13   vrb     Replace RFC accessor param init_req boolean by a req enum
02/27/13   lcl     Added support for more than two of the sampe device type.
02/22/13   tc      Re-insert PA_ON in start script
02/16/13   vb      Enabled the Antenna tuner manager (after further optimizations in
                   Tuner manager)
02/16/13   vb      Disabled antenna tuner from GSM code as it was found causing
                   crash during GSM wakeup
02/13/13   sb      Enable antenna tuner device calls in RFC
02/12/13   sr      changes to correct debug message
02/05/13   sb      Created separate APIs to configure RF front end devices based on whether
                   they are called every TX/RX burst or only when the band is changed.
01/28/12   sb      Made changes to set the QPOET in APT mode from cold start during TX enable
                   to avert settling related issues during first TX burst
01/25/12   sb      Fixed a bug which erases QPOET transactions for Multi slot TX case
01/22/12   sb      Changes to enable QPOET transactions to go out every gsm frame
01/18/13   tws     Fix KW errors. Remove include msm.h
01/09/13   sn      Pass correct mod_type argument in set_gain_range call
12/03/13   ggs     Remove RF_ON from transition buffer.
01/3/13    av      Changes to support chain2 for GSM
12/21/12   sr      Use rx timing for rx ant switch timing.
12/21/12   tc      Remove PA_ON from start script to remove ORFS switching failues
12/20/12   ggs     Add framework for appending rf_on grfc info from rfc
12/19/12   sb      Optimize GSM APT
12/13/12   sb      Enabled APT in GSM
12/12/12   sr      made changes to support ante_switch device timing.
12/11/12   sb      Add hooks for GSM APT
12/07/12   tws     Fix Klocwork errors.
12/06/12   sr      made changes to support PA device timing.
12/03/12   aka     Changes to use NV ptr from rfgsm_core_handle_ptr
11/27/12   sr      made changes to avoid error messages until proper device support..
11/26/12   aka     Changes to use a path specific gsm handle
11/16/12   tsr     Tx Alpha and TX SBI start delta update for DIME based on wtr1605
11/02/12   sr      removed un-neccessary debug messages.
09/05/12   kai     Added tuner device interface and scripting support
10/30/12   sr      changes create tech rfc object if the tech is supported by
                   the specific rf-card.
10/25/12   ec      Change to support reduced rx sbi start time
10/24/12   sr/sb   Made changes to remove calling set_mode_bias from tx_band_device_config to avoid TX crash (SVVZ)
10/18/12   sb      Made changes to include SSBIs for PA/ASM devices in TX/RX burst scripts (SVVZ bringup)
10/04/12   sr      Removed the tx related signals configuration from
                   config_mode() api, as it is done in enable_tx(). The
                   config_mode() api get called during IRAT meas.
10/01/12   sb      Made changes to add rfm device to rf device interface and uppler layer APIs
09/24/12   sr      Made changes to remove hand-coded RFC files.
09/05/12   sr      fixed compiler warnings.
09/04/12   sr      added Tx_gain over-ride API and Moved the rf_buffer allocation to
                   rfgsm_mdsp file.
08/29/12   sb/tsr  Made changes to bypass QPOET RFFE writes in set TX band instead of during set TX_ENABLE
08/29/12   sb/tsr  Made changes to turn ASM off at the end of RX/TX burst
08/27/12   tsr     Changes for TX enable
04/24/12   tsr     Checking for Null ptr for ASM devices
08/24/12   sb/tsr  Made changes to clear the NON SBIs only during band change and SBIs during setup TX/RX burst
08/20/12   shb     Include "rfdevice_class.h"
08/19/12   sr      changes to get the tx band rfc config instead of rx during
                   tx band and tx gain change.
08/19/12   sr      changes to push the band-port info to ASM device.
08/10/12   sr      made changes to use the new device interface to push band-port info.
08/10/12   vrb     Use updated interfaces to push band port information to dev
08/10/12   sr      Made changes to add the rf_on signal to transition tx script.
08/09/12   ggs     Initialised APT bus type information
08/07/12   sr      added rf front-end device config api calls
08/07/12   sc      Create seperate Rx and Tx scripts for CCS buffer tracability
08/06/12   sr      fixed cut & paste err while allocating rf_buffers stop scripts.
08/03/12   sr      initialized the rf_devices[] ptrs .
07/27/12   sr      renamed rfdevice related enum types.
07/23/12   sr      Fixed the cust & paste err for the transition script size.
07/21/12   sr      Made changes to gain_range signal mapping
07/20/12   sr      Made changes to pass the RFM_DEVICE and rf_buffer to rfc_gsm
07/18/12   sr      added get_device api to return the rf device obj ptr
                   based on logical_path, rx_tx, band and device_type.
07/11/12   sr      changes to initialize the devices as required.
07/09/12   sc      Remove arguments to send nonsbi burst script API for CCS support
07/09/12   sr      changes to use the rfc_data object instead of rfc_ag function ptr table.
07/02/12   sr      renamed rfc_common_gsmctl.h to rfc_common_tdd.h as it is shared with TDS.
06/29/12   sr      rfc api name change and some rfc api calls need to re-visited.
06/28/12   sr      Made changes to use rf_hal_buffer for the burst scripts.
06/28/12   sr      removed RFD related rfc functions.
06/19/12   sc      Shared mem compilation support for stop acq
07/02/12   sn      PA/ASM device interface and scripting support
05/22/12   sr      Move the manage_vregs() to base class from rf-card files.
05/18/12   sr      RFC-EFS code clean-up changes.
05/04/12   sb      Added changes to support TXP indicator for GSM
04/26/12   dej     Add breaks and default case to band switch in tx_timing_adjust
04/25/12   jps     Updated comments
04/18/12   jps/lcl PA Range and PA R0/R1 logic decoupling
03/20/12   sr      Added support for rfc binary data
03/03/12   jps     Fixes to make sure GSM PA Range GRFC is not assumed of type
                   GRFC RF ENGINE
02/28/12   dej     Hooked up PA start/stop delta adjustments in FTM mode.
                   ( Added update_timing_based_on_card and tx_timing_adjust )
02/28/12   jps     Moved the PA Range GRFC initialization to tx_band_config
12/20/11   sn      Added support for QFE1320
01/05/12   sr      Made changes to use the PA_RANGE timing info from RFC auto-gen.
12/13/11   vrb     Fix prev change: Configure routing table forRx band sig lists only
12/08/11   vrb     Configure routing table for signals in all Tx/Rx band spec sig lists
11/04/11    sr     made changes to update ths tx timing based on
                   pa_start(stop)_time_offset NV value.
10/28/11   vrb     Add Desense channel list for each band, for DC Spur Removal
10/25/11   sr      Made changes to configure rfc signals to tx enable/disable mode.
10/17/11    sr     changed the rfc api to get the timing info. for the specified signal.
10/13/11    sr     changed the pa_range grfc timing as the refrence time has changed.
10/11/11    sr     added rfc tx disable/enable sleep/wakeup calls to configure
                   RFC signals for tx disable/enable  sleep/wakeup state.
10/11/11    sr     optimized code for ISR over-run issue.
10/03/11   sb      Reduced Rx PLL tune time from 250qs to 180qs
09/23/11   sr      Increased the warm-up time from 15ms to 20ms to avoid crash.
09/22/11   sb      Making FTM Temp Comp enable RFC dependent
09/22/11   vrb     Update PA_R timing constants to fix multi-slot template
09/20/11   sr      Increased the warm-up time from 10ms to 15ms to avoid crash.
09/19/11   sr      Enable GSM RF sleep/wakeup functionality.
08/26/11   sr      Initialize the RFC signals to deafult before changing the band.
08/23/09   sr      made changes to move the common functionality to base class.
                   and derived RFC classes will implement RFC specific code.
08/20/09   sr      added init_once() to update the GRFC timing based on NV.
02/18/11   sr      Added static get_instance() to create and return the obj ptr.
11/21/11   sr      Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfc_card.h"
#include "rfc_gsm.h"
#include "rfgsm_core_util.h"
#include "rfgsm_nv_cmn.h"
#include "rfc_common.h"
#include "rfc_common_tdd.h"
#include "rfc_common_data.h"
#include "rfc_card_gsm.h"
#include "rfgsm_core_timing_consts.h"
#include "modem_mem.h"
#include "rfgsm_core_types.h"
#include "rfgsm_core_handle.h"
#include "rfc_vreg_mgr.h"
#include "rf_buffer_intf.h"
#include "rfc_gsm_data.h"
#include "rfc_class.h"
#include "rfdevice_rxtx_common_class.h"
#include "rfdevice_asm.h"
#include "rfdevice_asm_intf.h"
#include "rfdevice_pa.h"
#include "rfdevice_pa_intf.h"
#include "rfdevice_papm_intf.h"
#include "rfdevice_papm.h"
#include "rfdevice_class.h"
#include "rfcommon_atuner_manager_factory.h"
#include "rfdevice_antenna_tuner.h"

#ifdef FEATURE_RF_ASDIV
#include "rfcommon_asdiv_manager.h"
#include "rfcommon_asdiv_tuner_manager.h"
#include "rfdevice_xsw.h"
#endif
#include "rfcommon_atuner_manager_intf.h"


extern "C" {
#include "rfgsm_core.h"
#include "rfdevice_gsm_intf.h"
#include "rfgsm_core_tx.h"
}

#include "rfcommon_nv_mm.h"

#include "rfcommon_concurrency_manager.h"

/* Overhead in the mdsp for processing sbi commands */
#define RF_MDSP_OVERHEAD  67

/* RF tune delays: These are used by  modem/geran/gdrivers/src/gl1_hw_cm.c */
#define RFGSM_MON_TUNE_DELAY_QS  250 //340  // 321?
#define RFGSM_RX_TUNE_DELAY_QS 250 // 491

/* these are NOT  used by RF Drivers. These are used by
   "..\..\modem\geran\gdrivers\src\gl1_hw_sched.c. These consts are not valid
   anymore */
#define RF_MDSP_RX_ALPHA_QS (RFGSM_RX_TUNE_DELAY_QS + RF_MDSP_OVERHEAD)
#define RF_MDSP_MON_ALPHA_QS (RFGSM_MON_TUNE_DELAY_QS +  RF_MDSP_OVERHEAD)

#define NUM_PA_RANGE_GRFC 4


/*----------------------------------------------------------------------------*/
/*  RX TIMINGS                                                                */
/*----------------------------------------------------------------------------*/
/*!
  @detail
  The frame tick is slammed at the output of the filter.  So, to really line up
  with the BS, we need to account for all the Rx chain delay (from the antenna
  to the output of the Channel Filter.
*/
#define RF_RX_CHAIN_TOTAL_DELAY   54 //39



#define RFC_GSM_RX_ANT_START_DELTA_QS -4
#define RFC_GSM_RX_ANT_STOP_DELTA_QS   4

#define RFC_GSM_RXF_MODEM_DELAY_QS 54  /* 50usec */

#define RFC_GSM_RX_DC_CAL_TIME_QS 110
#define RFC_GSM_RX_PLL_SETTLING_TIME_QS 163  /* 150us / 0.923 */
#define RFC_GSM_RX_MUX_DLY_FREEZ_TIME_QS 108 /* 100us / 0.923 */

#define RFC_GSM_RX_MON_DC_CAL_TIME_QS 60

/* time for mdsp to program the sbi and non-sbi to hw buffer. */
#define RFC_GSM_RX_MDSP_OVERHEAD 30

/* rx_alpha is where mdsp fw wakeup and writes the SBIs and non-sbi scripts
   to hw buffer and arms the timer for SBIs and non-sbi cntrls to go out at
   the specified time in the burst script. */
/* 20 sbi_writes * 2.3(each write takes 2.3qs) = 30 qs */
#define RFC_GSM_RX_ALPHA (RFC_GSM_RX_PLL_SETTLING_TIME_QS + 46 + RFC_GSM_RX_MDSP_OVERHEAD)

/*Initialise Idle case rx alpha to same value as normal rx alpha*/
#define RFC_GSM_IDLE_MODE_RX_ALPHA  RFC_GSM_RX_ALPHA

#define RFC_GSM_TX_PLL_SBIS 0  /* num of sbis to go before KV traing sequence. zero for wtr1605*/

#define RFC_GSM_TX_KV_LENGTH 76 /* KV training sequence symbols based on wtr1605*/

/* SBI Time taken to write 1 SSBI in QS = 2.2usecs =2.2/(577/625)QS*/
#define USEC_TO_QS_SBI_TIME 2.38

/* TX SBI start time = num of kv length symbols + Time taken by sbis that go before KV training sequence starts.
  The number of sbis is zero for wtr1605. added for future targets
  each sbi write time =2.2 usecs
*/
#define RFC_GSM_TX_SBI_START_DELTA_QS -((RFC_GSM_TX_KV_LENGTH * 4)+ (RFC_GSM_TX_PLL_SBIS * USEC_TO_QS_SBI_TIME))

#define RFC_GSM_TX_ALPHA_QS -(RFC_GSM_TX_SBI_START_DELTA_QS)

/*----------------------------------------------------------------------------*/
/*  RF SIGMA DELTA NOISE FLOOR for MSM8974                                    */
/*----------------------------------------------------------------------------*/

#define RFC_GSM_SIGMA_DELTA_MAX_DB 102

#ifdef FEATURE_RF_ASDIV
#define RFGSM_FE_NON_SSBI_TRANSACTIONS 20
#endif

/*===========================================================================
   Mode specific CORE and FEATURE configuration tables.
===========================================================================*/

mdsp_xo_desense_arfcn_struct rfc_gsm::rfc_gsm_desense_chan_list =
{
  {LAST_ELEMNT_IN_CHNL_LIST},   /* uint16 G850_chan_list [MAX_NMBR_DESENSE_CHNL_PER_BAND]; */
  {LAST_ELEMNT_IN_CHNL_LIST},   /* uint16 G900_chan_list [MAX_NMBR_DESENSE_CHNL_PER_BAND]; */
  {LAST_ELEMNT_IN_CHNL_LIST},   /* uint16 G1800_chan_list[MAX_NMBR_DESENSE_CHNL_PER_BAND]; */
  {LAST_ELEMNT_IN_CHNL_LIST}    /* uint16 G1900_chan_list[MAX_NMBR_DESENSE_CHNL_PER_BAND]; */
};

/* Default PA gain range mapping GR(R0,R1): GR0=(0,0), GR1=(0,1), GR2=(1,0), GR3=(1,1)*/
rfc_gsm_pa_gain_range_map_type rfc_gsm::rfc_gsm_pa_gain_range_map =
{
  {
    /* G0    G1    G2    G3 */
    {{0,0},{0,1},{1,0},{1,1},{0,0},{0,0},{0,0},{0,0}}, /* 850 */
    {{0,0},{0,1},{1,0},{1,1},{0,0},{0,0},{0,0},{0,0}}, /* 900 */
    {{0,0},{0,1},{0,1},{1,1},{0,0},{0,0},{0,0},{0,0}}, /* 1800 */
    {{0,0},{0,1},{0,1},{1,1},{0,0},{0,0},{0,0},{0,0}}  /* 1900 */
    /* Temp change - Final change needs to be the PA range map from NV */
  }
};


/*
 Store a supported band for each logical device, RX or TX.
 A supported band helps to obtain the physical device ptr.
*/

rfc_gsm_supported_band_map_type rfc_gsm::rfc_gsm_supported_band_map =
{
  {
    /* RX    TX */
    {RFCOM_NUM_GSM_BANDS,RFCOM_NUM_GSM_BANDS},
    {RFCOM_NUM_GSM_BANDS,RFCOM_NUM_GSM_BANDS},
    {RFCOM_NUM_GSM_BANDS,RFCOM_NUM_GSM_BANDS},
    {RFCOM_NUM_GSM_BANDS,RFCOM_NUM_GSM_BANDS},
    {RFCOM_NUM_GSM_BANDS,RFCOM_NUM_GSM_BANDS},
    {RFCOM_NUM_GSM_BANDS,RFCOM_NUM_GSM_BANDS},
    {RFCOM_NUM_GSM_BANDS,RFCOM_NUM_GSM_BANDS},
    {RFCOM_NUM_GSM_BANDS,RFCOM_NUM_GSM_BANDS}
  }
};

rfc_sig_cfg_type *rfc_gsm_tx_sig_list_ptrs[RFM_MAX_WAN_DEVICES][RFCOM_NUM_GSM_BANDS];


/* Tx gain related GRFCs info */
rfc_grfc_script_entry_type rfc_gsm_pa_range0_grfc_info[RFM_MAX_WAN_DEVICES];
rfc_grfc_script_entry_type rfc_gsm_pa_range1_grfc_info[RFM_MAX_WAN_DEVICES];
rfc_grfc_script_entry_type rfc_gsm_gtr_threshold_grfc_info[RFM_MAX_WAN_DEVICES];
rfc_grfc_script_entry_type rfc_gsm_rf_on_grfc_info[RFM_MAX_WAN_DEVICES];


rfc_gsm *rfc_gsm::rfc_gsm_ptr = (rfc_gsm *)NULL;

#ifdef FEATURE_DUAL_SIM
extern rf_task_data_type rf_task_data[RF_MAX_TASK];
#endif


rfc_gsm * rfc_gsm::get_instance(void)
{
  return rfc_gsm_ptr;
}

// create singleton object
rfc_gsm * rfc_gsm::create_instance(void)
{
  if (rfc_gsm_ptr == NULL)
  {
    rfc_gsm_ptr = (rfc_gsm *)new rfc_gsm();
  }

  return rfc_gsm_ptr;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function initializes the RFC data during power-up.

  @details
  This function initializes the RFC data during power-up.

  @param None.
*/

void rfc_gsm::init_once(rfm_device_enum_type rfm_dev)
{
  /* ssbi support*/
  uint8 i = 0;
  uint8 band_local = 0;
  rfc_phy_device_info_type *phy_devices_cfg = NULL;
  rfc_logical_device_info_type *logical_devices_cfg = NULL;
  uint8 dev_id_index = 0;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  rfc_device_info_type *device_info_ptr = NULL;
  rfdevice_type_enum_type dev_type = RFDEVICE_TYPE_INVALID;
  rfdevice_id_enum_type dev_id = RFDEVICE_INVALID;
  boolean break_loop = FALSE;
  rfc_gsm_data *rfc_data = rfc_gsm_data::get_instance();

  rfc_common_data *rfc_cmn_data = rfc_common_data::get_instance();
  rfc_cfg_params_type cfg = { RF_PATH_0, rfm_dev, RFC_CONFIG_RX, RFCOM_NUM_GSM_BANDS, RFC_REQ_DEFAULT_GET_DATA};

  if (rfm_dev >= RFM_MAX_WAN_DEVICES)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_gsm::init_once Invalid rfm_dev %d", rfm_dev);
    return;
  }

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  /* construct NV timing adjusted tx GRFC burst scripts */
  construct_tx_burst_script(rfm_dev);

  cfg.alternate_path = 0;
  cfg.rx_tx = RFC_CONFIG_RX;

  phy_devices_cfg = rfc_cmn_data->get_phy_device_cfg();
  logical_devices_cfg = rfc_cmn_data->get_logical_device_cfg();

  /* set RFC core gsm ptr after creation of handles*/
  rfdevice_gsm_set_rfc_gsm_core_ptr(rfm_dev,&rfc_gsm_core_config_table[rfm_dev]);

  if ((rfgsm_core_handle_ptr != NULL) && (logical_devices_cfg != NULL) && (phy_devices_cfg != NULL))
  {
    /* Check only if ssbi protocal has not been found supported previously. Once its found, it will never perform the check again*/
    if(FALSE == rfgsm_core_handle_ptr->ssbi_protocol_supported)
    {
      /*loop through all bands on the RX path*/
      for (band_local=0; band_local<(int)RFCOM_NUM_GSM_BANDS; band_local++)
      {
        /* -----------------determining SSBI supported device or not--------------------------------*/

        cfg.band = band_local;


        if( (rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr )== TRUE) && (rfgsm_core_handle_ptr != NULL))
        {
       
          if ( device_info_ptr != NULL )
          {
            
              i=0;
              /* loop through devices supported in GSM*/
              while (i < device_info_ptr->num_devices)
              {
                dev_type = device_info_ptr->rf_asic_info[i].device_type;
                dev_id   = device_info_ptr->rf_asic_info[i].device_id;

                /* Find transceiver device*/
                if (dev_type == RFDEVICE_TRANSCEIVER)
                {

                  /* loop through common device list*/
                  while(logical_devices_cfg[dev_id_index].rf_device_type != RFDEVICE_TYPE_INVALID)
                  {
                    /*find out device type and device ID in the cmn devices list */
                    if  ((logical_devices_cfg[dev_id_index].rf_device_type == dev_type) && (logical_devices_cfg[dev_id_index].rf_device_id == dev_id))
                    {
                      if (phy_devices_cfg[dev_id_index].rf_device_comm_protocol == RFDEVICE_COMM_PROTO_SSBI)
                      {

                          rfgsm_core_handle_ptr->ssbi_protocol_supported =  TRUE;
                          break_loop = TRUE;

                      }
                      break;
                    }
                    dev_id_index++;
                  }
                
                }

                if (TRUE == break_loop)
                {
                  break;
                }

                i++;
               }


            }       

         }

        
        

        /* ----------------------------------------------------------------------------------------*/
        if (TRUE == break_loop)
        {
          break;
        }


      }
    }
  }

}



// Constructor
rfc_gsm::rfc_gsm()
{
  uint8 log_dev = 0;
  uint8 rx_tx = 0;
  uint8 band = 0;
  uint8 dev_type = 0;
  uint8 dev_instance = 0;
  boolean result = FALSE;



  rfc_gsm_ptr = this;

  // initialize the gsm_rf_devices
  for (log_dev = 0; log_dev < RFM_MAX_WAN_DEVICES; log_dev++)
  {
    for(rx_tx = 0; rx_tx < RFC_CONFIG_RXTX_MAX; rx_tx++)
    {
      for (band = 0; band < RFCOM_NUM_GSM_BANDS; band++)
      {
        for(dev_type = 0; dev_type < RFDEVICE_TYPE_MAX_NUM; dev_type++)
        {
          for (dev_instance = 0; dev_instance < RFC_MAX_DEVICE_INSTANCES_PER_BAND; dev_instance++)
          {
            gsm_rf_devices[log_dev][rx_tx][band][dev_type][dev_instance] = NULL;

            if (dev_type ==  RFDEVICE_TRANSCEIVER)
            {
              //Initializing the RF path
              trx_device_rf_path[log_dev][rx_tx][band][dev_instance] = 0xFFFF;
            }

          }
        }
      }
    }
  }


  for (log_dev = 0; log_dev < RFM_MAX_WAN_DEVICES; log_dev++)
  {
    result = initialize_rfc_timing_info((rfm_device_enum_type)log_dev);
    for (band = 0; band < RFCOM_NUM_GSM_BANDS; band++)
    {
      rfc_gsm_tx_sig_list_ptrs[log_dev][band] = NULL;
    }
  }

  /*! @todo - All devices need to be initialised. However, if the RFC refers
     to an RF device that does not support GSM this will cause initialisation
     failures and break RX/TX functionality */

// initialize rfdevices with required rfc data
#ifndef T_RUMI_EMULATION
  for (log_dev = 0; log_dev < RFM_MAX_WAN_DEVICES; log_dev++)
  {
    init_rfdevices_for_all_bands((rfm_device_enum_type)log_dev);
  }
#endif

}


// Destructor
rfc_gsm::~rfc_gsm()
{
  uint8 rfm_dev;
  uint8 i =0;

  for (rfm_dev=0; rfm_dev<RFM_MAX_WAN_DEVICES; rfm_dev++)
  {
    for (i=0; i<RFCOM_NUM_GSM_BANDS; i++)
    {
      if (rfc_gsm_tx_sig_list_ptrs[rfm_dev][i]!=NULL)
      {
        modem_mem_free(rfc_gsm_tx_sig_list_ptrs[rfm_dev][i], MODEM_MEM_CLIENT_RFA);
      }
    }
  }
  rfc_gsm_ptr = NULL;

}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the default RFC intializations for GSM mode

   @details

*/

void rfc_gsm::config_mode( rfm_device_enum_type rfm_dev, rf_buffer_intf* enter_mode_buffer,uint8 alt_path)
{
  rfc_sig_cfg_type *sig_ptr = NULL;
  rfc_gsm_data *rfc_data = rfc_gsm_data::get_instance();
  rfc_cfg_params_type cfg = { RF_PATH_0, rfm_dev, RFC_CONFIG_RX, RFCOM_NUM_GSM_BANDS, RFC_REQ_INIT};
  rfdevice_class **dev_list = NULL;
  uint8 band_local = 0;
  uint8 i;
  uint32 script_token;
  boolean ret_val = FALSE;
  boolean gsm_rf_devices_config_mode_list[RFC_MAX_DEVICE_INSTANCES];

  /*MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"Select GSM mode rfm_dev %d", rfm_dev);*/

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC GSM Data Object is NOT Created", 0);
    return;
  }

  memset(&gsm_rf_devices_config_mode_list[0], 0, sizeof(gsm_rf_devices_config_mode_list));

  cfg.alternate_path = alt_path;
  cfg.rx_tx = RFC_CONFIG_RX;
  rfc_data->sig_cfg_data_get(&cfg, &sig_ptr );
  rfc_common_tdd_update_start_grfc_script(sig_ptr, (void *)enter_mode_buffer);

  /*loop through all bands on the RX path*/
  for (band_local=0; band_local<(int)RFCOM_NUM_GSM_BANDS; band_local++)
  {
    cfg.alternate_path = (uint32)alt_path;
    cfg.band = band_local;
    /*get RX tuner device*/
    dev_list = get_rf_device_list(rfm_dev, cfg.rx_tx, (rfcom_gsm_band_type)band_local, RFDEVICE_TUNER_MANAGER);
    if (dev_list != NULL)
    {
      uint8 index = 0;
      while (dev_list[index] != NULL && index < RFC_MAX_DEVICE_INSTANCES_PER_BAND && (!gsm_rf_devices_config_mode_list[index]))
      {
        ret_val = ((rfcommon_atuner_intf*)dev_list[index])->init(RFM_EGSM_MODE, RFDEVICE_CREATE_SCRIPT, enter_mode_buffer, 0, &script_token);
        if (ret_val && (script_token != ATUNER_INVALID_TOKEN_VAL))
        {
          ((rfcommon_atuner_intf*)dev_list[index])->acknowledge_script(script_token);
        }

        gsm_rf_devices_config_mode_list[index] = ret_val;
        index++;
      }
    }
    /*get asm device*/
    dev_list = get_rf_device_list(rfm_dev, cfg.rx_tx, (rfcom_gsm_band_type)band_local, RFDEVICE_ASM);
    if (dev_list != NULL)
    {
      uint8 index = 0;
      while (dev_list[index] != NULL && index < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
      {
        wakeup_device_config(dev_list[index], enter_mode_buffer, FALSE);
        index++;
      }
    }
  }

  cfg.alternate_path = 0;
  cfg.rx_tx = RFC_CONFIG_TX;
  rfc_data->sig_cfg_data_get(&cfg, &sig_ptr );
  rfc_common_tdd_update_start_grfc_script(sig_ptr, (void *)enter_mode_buffer);

  /*loop through all bands on the TX path*/
  for (band_local=0; band_local<(int)RFCOM_NUM_GSM_BANDS; band_local++)
  {
    cfg.alternate_path = 0;
    cfg.band = band_local;
   
    /*get TX tuner device*/    
    dev_list = get_rf_device_list(rfm_dev, cfg.rx_tx, (rfcom_gsm_band_type)band_local, RFDEVICE_TUNER_MANAGER);
    if (dev_list != NULL)
    {
      uint8 index = 0;
      while (dev_list[index] != NULL && index < RFC_MAX_DEVICE_INSTANCES_PER_BAND && (!gsm_rf_devices_config_mode_list[index]))
      {
        ret_val = ((rfcommon_atuner_intf*)dev_list[index])->init(RFM_EGSM_MODE, RFDEVICE_CREATE_SCRIPT, enter_mode_buffer, 0, &script_token);
        if (ret_val && (script_token != ATUNER_INVALID_TOKEN_VAL))
        {
          ((rfcommon_atuner_intf*)dev_list[index])->acknowledge_script(script_token);
        }

        gsm_rf_devices_config_mode_list[index] = ret_val;
        index++;
      }
    }

  }
} /* config_mode() */


/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the disabling of tuner in x2G case

   @details

*/

void rfc_gsm::disable_rffe( rfm_device_enum_type rfm_dev, rf_buffer_intf *buffer, rfcom_gsm_band_type band)
{

  rfdevice_class *dev_obj = NULL;
  uint32 script_token;
  uint8 instance = 0;

  if ((rfm_dev < RFM_MAX_DEVICES) && (band < RFCOM_NUM_GSM_BANDS))
  {
    for (instance = 0; instance< RFC_MAX_DEVICE_INSTANCES_PER_BAND; instance++)
    {
      if (gsm_rf_devices[rfm_dev][RFC_CONFIG_RX][band][RFDEVICE_TUNER_MANAGER][instance] != NULL)
      {
        dev_obj = gsm_rf_devices[rfm_dev][RFC_CONFIG_RX][band][RFDEVICE_TUNER_MANAGER][instance];
        ((rfcommon_atuner_intf*)dev_obj)->disable(RFDEVICE_CREATE_SCRIPT, buffer, 0, &script_token );

      }
    }
  }
  else
  {
    MSG_4(MSG_SSID_RF, MSG_LEGACY_ERROR,"Index out of range: %d !< %d  or %d !< %d ",
          rfm_dev, RFM_MAX_DEVICES, band, RFCOM_NUM_GSM_BANDS);
  }

}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the card specific meas data

   @details

*/
void *rfc_gsm::get_mode_config_data(rfm_device_enum_type rfm_dev,
                                    rfc_mode_config_data_enum_type config_data)
{
  void* data_config_ptr = NULL;

  if (rfm_dev >= RFM_MAX_WAN_DEVICES)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "invalid RFC data request with rfm_dev %d", rfm_dev);
    return data_config_ptr;
  }

  switch (config_data)
  {
  case RFC_MODE_CORE_DATA:
    data_config_ptr = (void *)&rfc_gsm_core_config_table[rfm_dev];
    break;
  default:            /* Invalid mode operation */
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Unknown RFC data request(rfm_dev=%d)",rfm_dev);
    break;
  }

  return( data_config_ptr );
}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function configures all the GPIOs/GRFCs, RF front end devices needed for RX burst

   @details
   It populates the device steeings in the start and stop buffer.

*/
void rfc_gsm::rx_burst_config(rfm_device_enum_type rfm_dev,
                              rfcom_gsm_band_type rfgsm_band,
                              uint16 arfcn,
                              uint8 alt_path)
{
  uint8 i = 0;
  rfc_cfg_params_type cfg = { RF_PATH_0, rfm_dev, RFC_CONFIG_RX, rfgsm_band, RFC_REQ_DEFAULT_GET_DATA};
  rfc_sig_cfg_type *sig_ptr = NULL;
  rfdevice_class **dev_list = NULL;
  boolean status = FALSE;
  uint32 instance = 0;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr;

  cfg.alternate_path = (uint32)alt_path;
  rfc_gsm_data *rfc_data = rfc_gsm_data::get_instance();
  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC GSM Data Object is NOT Created", 0);
    return;
  }

  if (rfgsm_band >= RFCOM_NUM_GSM_BANDS)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"invalid band = %d", rfgsm_band);
    return;
  }

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!");

  /*Clear script for GRFCs before retrieving*/
  rf_buffer_clear_script((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.rx_start_script[RX_GRFC_SCRIPT],
                         RF_BUFFER_ALL);
  rf_buffer_clear_script((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.rx_stop_script[RX_GRFC_SCRIPT],
                         RF_BUFFER_ALL);

  /*Clear Rx asm scripts befor populating*/
  rf_buffer_clear_script((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.rx_start_script[RX_ASM_SCRIPT],
                         RF_BUFFER_ALL);
  rf_buffer_clear_script((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.rx_stop_script[RX_ASM_SCRIPT],
                         RF_BUFFER_ALL);

  /* Clear Tuner scripts before populating */
  rf_buffer_clear_script((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.rx_start_script[RX_ATUNER_SCRIPT],
                         RF_BUFFER_ALL);
  rf_buffer_clear_script((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.rx_stop_script[RX_ATUNER_SCRIPT],
                         RF_BUFFER_ALL);

#ifdef FEATURE_RF_ASDIV 
  /* Clear XSW scripts before populating */
  if(!rf_buffer_clear_script( (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.rx_start_script[RX_XSW_SCRIPT], RF_BUFFER_ALL)||
	 !rf_buffer_clear_script( (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.rx_stop_script[RX_XSW_SCRIPT], RF_BUFFER_ALL))
  {
    RF_MSG(RF_ERROR,"RF GSM XSW RX START/STOP script buffer was unsuccessfully cleared");
  }
#endif

  /* configure the GPIOs or GRFCs */
  rfc_data->sig_cfg_data_get(&cfg, &sig_ptr );
  rfc_common_tdd_update_grfc_script( sig_ptr,
                                     (void *)rfgsm_core_handle_ptr->scripts.rx_start_script[RX_GRFC_SCRIPT],
                                     (void *)rfgsm_core_handle_ptr->scripts.rx_stop_script[RX_GRFC_SCRIPT]);


  /*loop through all device types*/
  for (i = 0; i < RFDEVICE_TYPE_MAX_NUM; i++)
  {
    dev_list = get_rf_device_list(rfm_dev, cfg.rx_tx, rfgsm_band, (rfdevice_type_enum_type)i);
    if (dev_list != NULL)
    {
      uint8 index = 0;
      while (dev_list[index] != NULL && index < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
      {
        status = rx_burst_device_config(rfm_dev,dev_list[index], rfgsm_band, sig_ptr, arfcn);

        if (status == FALSE)
        {

          MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"Failed to configure Rx rf-devices! rfm_dev=%d,band=%d,dev_type=%d",
                cfg.logical_device, cfg.band, (rfdevice_type_enum_type)i);
        }

        index++;
      }
    }

  }

} /* rx_burst_config() */

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements rf-front-end device configuration for the specified device.

   @details
   This function implements rf-front-end device configuration for the specified device. Based on the
   device type, calls a specific API to configure the device for the specified rx band.
*/
boolean rfc_gsm::rx_burst_device_config( rfm_device_enum_type rfm_dev,
    rfdevice_class *dev_obj,
    rfcom_gsm_band_type band,
    rfc_sig_cfg_type *sig_ptr,
    uint16 arfcn)
{
  boolean status = FALSE;
  rfdevice_type_enum_type dev_type = RFDEVICE_TYPE_INVALID;
  rfc_grfc_script_entry_type timing_info = {0};
  rfcom_band_type_u rfcom_band;
  uint32 script_token;
  boolean ret_val = FALSE;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  rfdevice_asm_cfg_type asm_cfg_s;
  rf_buffer_intf *fe_buffer;
  rfcom_antenna_tuner_priority_cfg tuner_priority_cfg;

#ifdef FEATURE_RF_ASDIV
  rf_buffer_intf *dummy_fe_buffer;
#endif

  if (NULL == dev_obj)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf-front-end device NOT Created! dev_obj = NULL", 0);
    return FALSE;
  }

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!", FALSE);

  rfcom_band.gsm_band = band;
  dev_type = dev_obj->rfdevice_type_get();
  tuner_priority_cfg.rfm_device = rfm_dev;

  switch (dev_type)
  {
  case RFDEVICE_ASM:

    rfc_common_tdd_get_grfc_info(sig_ptr, RFC_TIMING_ASM_CTL, &timing_info);

    asm_cfg_s.rfm_device = rfm_dev;
    asm_cfg_s.mode = RFM_EGSM_MODE;
    asm_cfg_s.band.gsm_band = band;
    asm_cfg_s.asm_rx_tx = RFDEVICE_ASM_RX_0;
    asm_cfg_s.asm_cell = RFDEVICE_ASM_CELL_0;
    asm_cfg_s.trigger_type = RFDEVICE_ASM_TRIGGER_ENABLE_RX;

    rf_common_enter_critical_section( &(((rfdevice_asm *)dev_obj)->rfdevice_critical_sect));

    rfdevice_asm_enable(&asm_cfg_s,
                        dev_obj,
                        (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.rx_start_script[RX_ASM_SCRIPT],
                        RFDEVICE_CREATE_SCRIPT,
                        timing_info.start_delta);

    asm_cfg_s.trigger_type = RFDEVICE_ASM_TRIGGER_DISABLE_RX;
    rfdevice_asm_disable(&asm_cfg_s,
                         dev_obj,
                         (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.rx_stop_script[RX_ASM_SCRIPT],
                         RFDEVICE_CREATE_SCRIPT,
                         timing_info.stop_delta);

    rf_common_leave_critical_section( &(((rfdevice_asm *)dev_obj)->rfdevice_critical_sect));

    status = TRUE;
    break;

  case RFDEVICE_TUNER_MANAGER:
     fe_buffer = (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.rx_start_script[RX_ATUNER_SCRIPT];
#ifdef FEATURE_RF_ASDIV
    if ((rfgsm_core_handle_ptr->probe_burst == RFGSM_RX_PROBE_BURST) 
         || (rfgsm_core_handle_ptr->set_antenna_position != RF_INVALID_VALUE_8BIT))
    {
      dummy_fe_buffer = rf_buffer_create(0, RFGSM_FE_NON_SSBI_TRANSACTIONS, 0);
      fe_buffer = dummy_fe_buffer;
    }    
#endif
      rfc_common_tdd_get_grfc_info(sig_ptr, RFC_TIMING_TUNER_CTL, &timing_info);

      ret_val = ((rfcommon_atuner_intf*)dev_obj)->init(RFM_EGSM_MODE, 
                RFDEVICE_CREATE_SCRIPT,
                fe_buffer,
                timing_info.start_delta,
                &script_token);

      if (ret_val && (script_token != ATUNER_INVALID_TOKEN_VAL))
      {
        ((rfcommon_atuner_intf*)dev_obj)->acknowledge_script(script_token);
      }

      ret_val = ((rfcommon_atuner_intf*)dev_obj)->set_rx_mode(tuner_priority_cfg,
                RFM_EGSM_MODE,
                rfcom_band,
                arfcn,
                RFDEVICE_CREATE_SCRIPT,
                fe_buffer,
                timing_info.start_delta,
                &script_token );

      if (ret_val && (script_token != ATUNER_INVALID_TOKEN_VAL))
      {
        ((rfcommon_atuner_intf*)dev_obj)->acknowledge_script(script_token);
      }
      //MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"RX RFDEVICE_TUNER_MANAGER script_token: %d",script_token );
      status = ret_val;
#ifdef FEATURE_RF_ASDIV
      if ((rfgsm_core_handle_ptr->probe_burst == RFGSM_RX_PROBE_BURST) 
           || (rfgsm_core_handle_ptr->set_antenna_position != RF_INVALID_VALUE_8BIT))
      {
        rf_buffer_delete(dummy_fe_buffer);
      }    
#endif
    break;

  case RFDEVICE_TRANSCEIVER:
  case RFDEVICE_TRANSMITTER:
  case RFDEVICE_RECEIVER:
  case RFDEVICE_PA:
  case RFDEVICE_TUNER:
  case RFDEVICE_PAPM:
  case RFDEVICE_THERM:
  case RFDEVICE_HDET:
  case RFDEVICE_COUPLER:
#ifdef FEATURE_RF_ASDIV
  case RFDEVICE_XSW:
#endif
    /* nothing todo here, tranceiver configuration is done from mc/core */
    status = TRUE;
    break;

  default:
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf-front-end device NOT Configured!", 0);
    break;
  }

  return status;
}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns the default rfc rx band config data.

   @details

*/
void *rfc_gsm::rx_band_config_get(rfm_device_enum_type rfm_dev,
                                  rfcom_gsm_band_type rfgsm_band,
                                  rfc_band_config_data_enum_type config_data)
{
  void* data_config_ptr = NULL;

  rfc_cfg_params_type cfg = { RF_PATH_0, rfm_dev, RFC_CONFIG_RX, rfgsm_band, RFC_REQ_DEFAULT_GET_DATA};
  rfc_sig_cfg_type *sig_ptr = NULL;
  rfc_gsm_data *rfc_data = rfc_gsm_data::get_instance();

  cfg.alternate_path = 0;
  if (rfgsm_band >= RFCOM_NUM_GSM_BANDS)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"invalid band (band=%d)",rfgsm_band);
    return data_config_ptr;
  }

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC GSM Data Object is NOT Created", 0);
    return data_config_ptr;
  }

  /* This for IRAT case only */
  if (RFC_GRFC_DATA == config_data)
  {
    rfc_data->sig_cfg_data_get(&cfg, &sig_ptr );
    data_config_ptr = sig_ptr;
  }
  return( data_config_ptr );

}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the card specific intializations only when the band
   changes.

   @details
   The band table which this RF card supports is not indexed in any way.
   Explicit logic will need to be provided to extract the band specific data
   structures.

*/
void rfc_gsm::tx_band_config(rfm_device_enum_type rfm_dev,
                             rfcom_gsm_band_type rfgsm_band,
                             rf_buffer_intf* tx_band_script)
{
  uint8 i = 0;
  boolean status = FALSE;
  rfc_cfg_params_type cfg = { RF_PATH_0, rfm_dev, RFC_CONFIG_TX, rfgsm_band, RFC_REQ_DEFAULT_GET_DATA};
  boolean tx_supported = TRUE;
  rfc_sig_cfg_type *sig_ptr = NULL;
  rfdevice_class **dev_list = NULL;
  rfc_gsm_data *rfc_data = NULL;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  uint32 instance = 0;

  cfg.alternate_path = 0;
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return;
  }

  rfc_data = rfc_gsm_data::get_instance();
  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC GSM Data Object is NOT Created", 0);
    return;
  }

  if (rfgsm_band >= RFCOM_NUM_GSM_BANDS)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"invalid band = %d", rfgsm_band);
    return;
  }


  rf_buffer_clear_script(tx_band_script, RF_BUFFER_ALL);

  tx_supported = rfc_data->sig_cfg_data_get(&cfg, &sig_ptr );
  if (tx_supported == TRUE)
  {

    /* pa _range 0 info */
    status = rfc_common_tdd_get_grfc_info(rfc_gsm_tx_sig_list_ptrs[rfm_dev][rfgsm_band], RFC_PA_RANGE0,
                                          &rfc_gsm_pa_range0_grfc_info[rfm_dev]);
    if (status == FALSE)
    {
      rfc_gsm_pa_range0_grfc_info[rfm_dev].grfc_id = RF_HAL_GRFC_MAX_NUM;
    }

    /* pa _range 1 info */
    status = rfc_common_tdd_get_grfc_info(rfc_gsm_tx_sig_list_ptrs[rfm_dev][rfgsm_band], RFC_PA_RANGE1,
                                          &rfc_gsm_pa_range1_grfc_info[rfm_dev]);
    if (status == FALSE)
    {
      rfc_gsm_pa_range1_grfc_info[rfm_dev].grfc_id = RF_HAL_GRFC_MAX_NUM;
    }

    /* pa _GTR threshold grfc info */
    status = rfc_common_tdd_get_grfc_info(rfc_gsm_tx_sig_list_ptrs[rfm_dev][rfgsm_band], RFC_TX_GTR_THRESH,
                                          &rfc_gsm_gtr_threshold_grfc_info[rfm_dev]);
    if (status == FALSE)
    {
      rfc_gsm_gtr_threshold_grfc_info[rfm_dev].grfc_id = RF_HAL_GRFC_MAX_NUM;
    }

    /* RF_ON grfc info */
    status = rfc_common_tdd_get_grfc_info(rfc_gsm_tx_sig_list_ptrs[rfm_dev][rfgsm_band], RFC_TX_ON_AG,
                                          &rfc_gsm_rf_on_grfc_info[rfm_dev]);
    if (status == FALSE)
    {
      rfc_gsm_rf_on_grfc_info[rfm_dev].grfc_id = RF_HAL_GRFC_MAX_NUM;
    }

//    /* configure the rf-front-end devices */


    /*loop through all device types*/
    for (i = 0; i < RFDEVICE_TYPE_MAX_NUM; i++)
    {
      dev_list = get_rf_device_list(rfm_dev, cfg.rx_tx, rfgsm_band, (rfdevice_type_enum_type)i);
      if (dev_list != NULL)
      {
        uint8 index = 0;
        while (dev_list[index] != NULL && index < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
        {
          status = tx_band_device_config(rfm_dev, dev_list[index], rfgsm_band, tx_band_script);

          if (status == FALSE)
          {
            MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"Failed to configure Tx rf-devices! rfm_dev=%d,band=%d,dev_type=%d",
                  cfg.logical_device, cfg.band, (rfdevice_type_enum_type)i);
          }

          index++;
        }
      }
    }

  }

} /* tx_band_config() */

/* ----------------------------------------------------------------------- */
/*!
  @brief
   This function implements rf-front-end device configuration for the specified device.

   @details
   This function implements rf-front-end device configuration for the specified device. Based on the
   device type, calls a specific API to configure the device for the specified tx band.
*/
boolean rfc_gsm::tx_band_device_config(rfm_device_enum_type rfm_dev, rfdevice_class* dev_obj, rfcom_gsm_band_type band,
                                       rf_buffer_intf* band_buffer)
{
  boolean status = FALSE;
  rfdevice_type_enum_type dev_type = RFDEVICE_TYPE_INVALID;
  rfc_grfc_script_entry_type timing_info = {0};
  rfcom_band_type_u rfcom_band;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  rfdevice_pa_config_type pa_params_s;
  rfdevice_asm_cfg_type asm_cfg_s;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return FALSE;
  }

  if (NULL == dev_obj)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf-front-end device NOT Created! dev_obj = NULL", 0);
    return FALSE;
  }

  rfcom_band.gsm_band = band;

  dev_type = dev_obj->rfdevice_type_get();
  switch (dev_type)
  {
  case RFDEVICE_PA:
    rfc_common_tdd_get_grfc_info(rfc_gsm_tx_sig_list_ptrs[rfm_dev][band], RFC_TIMING_PA_CTL, &timing_info);

    pa_params_s.mode = RFM_EGSM_MODE;
    pa_params_s.band = rfcom_band;
    pa_params_s.bw = RFDEVICE_PA_BW_NOT_APPLICABLE;
    pa_params_s.trigger_type = RFDEVICE_PA_TRIGGER_CONFIG;
    pa_params_s.rfm_device = rfm_dev;

    rfdevice_pa_config(&pa_params_s, (void *)dev_obj,
                       band_buffer,
                       RFDEVICE_CREATE_SCRIPT, 0);

    status = TRUE;
    break;

  case RFDEVICE_PAPM:
    //((rfdevice_papm *)dev_obj)->set_mode_bias(RFM_EGSM_MODE, RFDEVICE_BYPASS_MODE, 0,
    //                                                RFDEVICE_EXECUTE_IMMEDIATE, NULL);
    /*When the TX is enabled, QPOET is set to APT mode with a bias value of 3V*/
    ((rfdevice_papm *)dev_obj)->set_mode_bias(rfm_dev, RFM_EGSM_MODE, rfcom_band, 
                                              RFDEVICE_APT_MODE, 500, band_buffer,
                                              RFDEVICE_CREATE_SCRIPT, 0);

    ((rfdevice_papm *)dev_obj)->trigger(band_buffer, rfm_dev, RFDEVICE_CREATE_SCRIPT, 0);
    status = TRUE;
    break;

  case RFDEVICE_TRANSCEIVER:
  case RFDEVICE_TRANSMITTER:
  case RFDEVICE_RECEIVER:
  case RFDEVICE_TUNER:
  case RFDEVICE_TUNER_MANAGER:
  case RFDEVICE_THERM:
  case RFDEVICE_HDET:
    status = TRUE;
    break;

  case RFDEVICE_ASM:
    asm_cfg_s.rfm_device = rfm_dev;
    asm_cfg_s.mode = RFM_EGSM_MODE;
    asm_cfg_s.band.gsm_band = RFCOM_BAND_GSM900;
    asm_cfg_s.asm_rx_tx = RFDEVICE_ASM_TX_0;
    asm_cfg_s.asm_cell = RFDEVICE_ASM_CELL_0;
    asm_cfg_s.trigger_type = RFDEVICE_ASM_TRIGGER_WAKEUP;
    rfdevice_asm_wakeup(&asm_cfg_s, dev_obj, band_buffer, RFDEVICE_CREATE_SCRIPT, RFCOM_INVALID_TIMING_OFFSET);
    status = TRUE;
    break;

    break;
  case RFDEVICE_COUPLER:
#ifdef FEATURE_RF_ASDIV
  case RFDEVICE_XSW:
#endif
    /* nothing todo here, tranceiver configuration is done from mc/core */
    status = TRUE;
    break;

  default:
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf-front-end device NOT Configured!", 0);
    break;
  }

  return status;
}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function configures the GRFCs/GPIOs and RF front end devices needed every TX burst.

   @details
   It populates the start and stop buffer with the device configurations.

*/
void rfc_gsm::tx_burst_config( rfm_device_enum_type rfm_dev,
                               rfcom_gsm_band_type rfgsm_band,
                               uint16 arfcn)
{
  uint8 i = 0;
  boolean status = FALSE;
  rfc_cfg_params_type cfg = { RF_PATH_0, rfm_dev, RFC_CONFIG_TX, rfgsm_band, RFC_REQ_DEFAULT_GET_DATA};
  boolean tx_supported = TRUE;
  rfc_sig_cfg_type *sig_ptr = NULL;
  rfdevice_class **dev_list = NULL;
  rfc_gsm_data *rfc_data = NULL;
  uint32 instance = 0;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr;

  cfg.alternate_path = 0;
  if (rfm_dev >= RFM_MAX_WAN_DEVICES)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Invalid rfm device :%d", rfm_dev);
    return;
  }

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!");

  rfc_data = rfc_gsm_data::get_instance();
  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RFC GSM Data Object is NOT Created", 0);
    return;
  }

  if (rfgsm_band >= RFCOM_NUM_GSM_BANDS)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "invalid band = %d", rfgsm_band);
    return;
  }

  rf_buffer_clear_script((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_start_script[TX_GRFC_SCRIPT],
                         RF_BUFFER_ALL);
  rf_buffer_clear_script((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_stop_script[TX_GRFC_SCRIPT],
                         RF_BUFFER_ALL);

  /*Clear the ASM device scripts before populating*/
  rf_buffer_clear_script((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_start_script[TX_ASM_SCRIPT],
                         RF_BUFFER_ALL);
  rf_buffer_clear_script((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_stop_script[TX_ASM_SCRIPT],
                         RF_BUFFER_ALL);

  /*Clear the PA device scripts before populating*/
  rf_buffer_clear_script((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_start_script[TX_PA_SCRIPT],
                         RF_BUFFER_ALL);
  rf_buffer_clear_script((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_stop_script[TX_PA_SCRIPT],
                         RF_BUFFER_ALL);

  /*Clear the ATUNER device scripts before populating*/
  rf_buffer_clear_script((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_start_script[TX_ATUNER_SCRIPT],
                         RF_BUFFER_ALL);
  
  /*Clear APT device script before populating*/
  rf_buffer_clear_script((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_stop_script[TX_APT_SCRIPT],
                         RF_BUFFER_ALL);

  tx_supported = rfc_data->sig_cfg_data_get(&cfg, &sig_ptr);
  if (tx_supported == TRUE)
  {

    /* Configure for current band GRFC settings */
    rfc_common_tdd_update_grfc_script(rfc_gsm_tx_sig_list_ptrs[rfm_dev][rfgsm_band],
                                      (void *)rfgsm_core_handle_ptr->scripts.tx_start_script[TX_GRFC_SCRIPT],
                                      (void *)rfgsm_core_handle_ptr->scripts.tx_stop_script[TX_GRFC_SCRIPT]);

//  /* configure the rf-front-end devices */

    /*loop through all device types*/
    for (i = 0; i < RFDEVICE_TYPE_MAX_NUM; i++)
    {
      dev_list = get_rf_device_list(rfm_dev, cfg.rx_tx, rfgsm_band, (rfdevice_type_enum_type)i);
      if (dev_list != NULL)
      {
        uint8 index = 0;
        while (dev_list[index] != NULL && index < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
        {
          status = tx_burst_device_config(rfm_dev, dev_list[index], rfgsm_band, arfcn, rfgsm_core_handle_ptr->txlm_buf_index);

          if (status == FALSE)
          {
            MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"Failed to configure Tx rf-devices! rfm_dev=%d,band=%d,dev_type=%d",
                  cfg.logical_device, cfg.band, (rfdevice_type_enum_type)i);
          }

          index++;
        }
      }
    }
  }
} /* tx_burst_config()*/

#ifdef FEATURE_RF_ASDIV
/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function appends the ASD GRFCs depending upon antenna position request

   @details
   Appends the ASD GRFCs depending upon antenna position request

*/

boolean rfc_gsm::append_txd_grfc(uint8 command, rf_buffer_intf *buffer, void *cb_data)
{
  rfc_sig_cfg_type *sig_ptr = NULL;
  rfc_gsm_data *rfc_data;
  rfc_cfg_params_type cfg = { RF_PATH_0, RFM_DEVICE_0, RFC_CONFIG_RX, RFCOM_BAND_GSM850, RFC_REQ_DISABLE_ANT_SWAP_1};
  rfgsm_core_handle_type *rfgsm_core_handle_ptr;
  rfgsm_asdiv_xsw_script_data *xsw_script_data = (rfgsm_asdiv_xsw_script_data*)cb_data;

  rfc_data = rfc_gsm_data::get_instance();

  // Update the RFC cfg request with the correct device and band
  cfg.logical_device = xsw_script_data->device;
  cfg.band = xsw_script_data->band;
  cfg.alternate_path = 0;

  if( command == 0 ) 
  {
    // Here we are assumming that command 0 maps to DISABLE_ANT_SWAP
    cfg.req = RFC_REQ_DISABLE_ANT_SWAP_1;
  }
  else 
  {
    // Here we are assumming that command 1 maps to ENABLE_ANT_SWAP
    cfg.req = RFC_REQ_ENABLE_ANT_SWAP_1;
  }

  // Retrieve the switch information from the RFC
  rfc_data->sig_cfg_data_get(&cfg, &sig_ptr );

  if(sig_ptr == NULL)
  {
    return FALSE;
  }
  // Apply the switch info to the GRFC script
  rfc_common_tdd_update_start_grfc_script(sig_ptr, (void *)buffer);

  return TRUE;
  
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function appends the ASD RFFEs depending upon antenna position request

   @details
   Appends the ASD RFFEs depending upon antenna position request

*/

void rfc_gsm::append_asdiv_rffe(uint8 command, rf_buffer_intf *buffer, void *cb_data)
{
  boolean status = TRUE;
  rfc_gsm_data *rfc_data = NULL;
  rfdevice_xsw_config_type xsw_config;
  rfgsm_asdiv_xsw_script_data *xsw_script_data;
  rfc_device_info_type* device_info_ptr = NULL;
  uint8 index = 0;
  rfc_cfg_params_type cfg = { RF_PATH_0, RFM_DEVICE_0, RFC_CONFIG_RX, RFCOM_BAND_GSM850, RFC_REQ_DISABLE_ANT_SWAP_1};
  /*Device instance used to extract tech specific object*/
  uint32 instance = 0;
  rfc_intf *rfc_cmn = rfc_intf::get_instance();
  /*Device type used to extract tech specific object*/
  rfdevice_type_enum_type cmn_dev_type = RFDEVICE_TYPE_INVALID;
  rfc_data = rfc_gsm_data::get_instance();
  /* Pointer to device class to hold common object */
  rfdevice_class *device_obj = NULL;
  rf_device_execution_type immediate = RFDEVICE_CREATE_SCRIPT;

  /* Get the ASM script data */
  xsw_script_data = (rfgsm_asdiv_xsw_script_data*)cb_data;
  if ( xsw_script_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfc_gsm - append_asdiv_rffe: CB Data is null " );
    return;
  }

  /* Populate params */
  cfg.alternate_path = 0;
  cfg.logical_device = xsw_script_data->device; /* Not required*/
  cfg.band = xsw_script_data->band;
  xsw_config.band.gsm_band = xsw_script_data->band;
  xsw_config.mode = RFM_EGSM_MODE;

  if ( command ) // Here We am assumming that command which is antenna position and command 1 maps to ENABLE ANT SWAP position
  {
    cfg.req = RFC_REQ_ENABLE_ANT_SWAP_1;
    xsw_config.xsmode = RFDEVICE_XSW_PRX_DRX_PATH_SWAP;
  }
  else // Here We am assumming that command which is antenna position and command 0 maps to DISABLE ANT SWAP position
  {
    cfg.req = RFC_REQ_DISABLE_ANT_SWAP_1;
    xsw_config.xsmode = RFDEVICE_XSW_PRX_DRX_PATH_DEFAULT;
  }

  /* Immediate logic is reversed! */
  if (xsw_script_data->immediate)
  {
    immediate = RFDEVICE_EXECUTE_IMMEDIATE;
  }

  /* Debug only */
  /* RF_MSG_2( RF_MED, "rfc_gsm_card_intf::generate_asdiv_rffe_script: "
            "command %d immediate = %d", command, immediate);*/

  /* Get device info pointer */
  rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr );

  if (device_info_ptr != NULL)
  {
    /* loop through the device info ptr to find the index of each device object*/
    while (index < device_info_ptr->num_devices)
    {
      /*Get device type and device instance to retrieve device object*/
      cmn_dev_type = device_info_ptr->rf_asic_info[index].device_type;
      switch (cmn_dev_type)
      {
      case RFDEVICE_XSW:
        RF_MSG_1( RF_MED, "rfc_gsm_card_intf::generate_asdiv_rffe_script: "
                  "RFDEVICE_QXSW buffer=0x%08x", buffer);

        instance = device_info_ptr->rf_asic_info[index].instance;
        device_obj = rfc_cmn->get_cmn_rf_device_object(RFDEVICE_XSW,(uint8)instance);
        if( device_obj != NULL )
        {
          status = ((rfdevice_xsw *)device_obj)->set_cross_switch_config(&xsw_config,buffer,immediate,xsw_script_data->start_delta);
          status = ((rfdevice_xsw *)device_obj)->trigger(buffer,immediate, xsw_script_data->start_delta);
        }
        else
        {
          status = FALSE ;
        }
        break;

      default:
        break;
      }
      index++;
    }/*while (index < device_info_ptr->num_devices)*/
  }
  else
  {
    status = FALSE ;
  }
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function gets the timing info of signal type requested
   @details
   Gets the timing info of signal type requested

*/

void rfc_gsm::get_timing_info( rfm_device_enum_type rfm_dev, rfcom_gsm_band_type rfgsm_band, rfdevice_type_enum_type dev_type, rfc_signal_type sig_info, int16 *start_delta)
{

  rfc_sig_cfg_type *sig_ptr = NULL;
  rfc_gsm_data *rfc_data;
  rfc_cfg_params_type cfg = { RF_PATH_0, rfm_dev, RFC_CONFIG_RX, rfgsm_band, RFC_REQ_DEFAULT_GET_DATA};
  rfc_data = rfc_gsm_data::get_instance();
  rfc_grfc_script_entry_type timing_info = {0};
  rfdevice_class **dev_obj = NULL;

  cfg.alternate_path = 0;

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC GSM Data Object is NOT Created", 0);
    return;
  }

  if (rfgsm_band >= RFCOM_NUM_GSM_BANDS)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"invalid band = %d", rfgsm_band);
    return;
  }

  if (start_delta == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"start_delta is NULL", 0);
    return;
  }
    
  if ( rfm_dev >= RFM_MAX_WAN_DEVICES || cfg.rx_tx >= RFC_CONFIG_RXTX_MAX 
      || rfgsm_band >= RFCOM_NUM_GSM_BANDS || dev_type >= RFDEVICE_TYPE_MAX_NUM 
      || dev_type < RFDEVICE_TRANSCEIVER )
  {
    MSG_4(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_gsm::get_timing_info(%d, %d, %d, %d): Incorrect Parameters",
          rfm_dev, cfg.rx_tx, rfgsm_band, dev_type);	
    *start_delta = 0;
    return ;
  }

  dev_obj = gsm_rf_devices[rfm_dev][RFC_CONFIG_RX][rfgsm_band][dev_type]; 

   if (dev_obj == NULL)
  {
    // No need to print F3
    *start_delta = 0;
    return;
  }
   
  /* configure the GPIOs or GRFCs */
  rfc_data->sig_cfg_data_get(&cfg, &sig_ptr );

  if(rfc_common_tdd_get_grfc_info(sig_ptr, sig_info, &timing_info))
  {
    *start_delta = timing_info.start_delta;
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Tuner start delta not present in RFC. Setting start delta to 0", 0);
    *start_delta = 0;
  }

}

#endif

/* ----------------------------------------------------------------------- */
/* Ony TX burst related devices*/
/*!
  @brief
   This function implements rf-front-end device configuration for the specified device.

   @details
   This function implements rf-front-end device configuration for the specified device. Based on the
   device type, calls a specific API to configure the device for a specified tx band every tx burst.
*/
boolean rfc_gsm::tx_burst_device_config(rfm_device_enum_type rfm_dev,
                                        rfdevice_class *dev_obj,
                                        rfcom_gsm_band_type band,
                                        uint16 arfcn,
                                        uint32 txlm_buf_index)
{
  boolean status = FALSE;
  rfdevice_type_enum_type dev_type = RFDEVICE_TYPE_INVALID;
  rfc_grfc_script_entry_type timing_info = {0};
  rfcom_band_type_u rfcom_band;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
//  uint32 freq=0;
  uint8 slot_count=0;
  rfdevice_papm_mode_type mode;
  uint8 apt_mode;
  uint16 bias;
  rf_buffer_intf *apt_script_buf = NULL;

  uint32 script_token;
  boolean ret_val = FALSE;
  rfdevice_asm_cfg_type asm_cfg_s;

  rfcommon_atuner_cl_ctrl_info_type cl_info;

  rfdevice_pa_config_type pa_params_s;

  cl_info.lm_handle = txlm_buf_index; 

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!", FALSE);

  cl_info.cl_nv_ctrl_info  = &rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->ant_tuner_cl_data;

  if (NULL == dev_obj)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf-front-end device NOT Created! dev_obj = NULL", 0);
    return FALSE;
  }

  rfcom_band.gsm_band = band;

  dev_type = dev_obj->rfdevice_type_get();
  switch (dev_type)
  {
  case RFDEVICE_ASM:
    /*Get the ASM timing from rfc */
    rfc_common_tdd_get_grfc_info(rfc_gsm_tx_sig_list_ptrs[rfm_dev][band], RFC_TIMING_ASM_CTL, &timing_info);
    asm_cfg_s.rfm_device = rfm_dev;
    asm_cfg_s.mode = RFM_EGSM_MODE;
    asm_cfg_s.band.gsm_band = band;
    asm_cfg_s.asm_rx_tx = RFDEVICE_ASM_TX_0;
    asm_cfg_s.asm_cell = RFDEVICE_ASM_CELL_0;
    asm_cfg_s.trigger_type = RFDEVICE_ASM_TRIGGER_ENABLE_TX;

    rf_common_enter_critical_section( &(((rfdevice_asm *)dev_obj)->rfdevice_critical_sect));

    rfdevice_asm_enable(&asm_cfg_s,
                        dev_obj,
                        (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_start_script[TX_ASM_SCRIPT],
                        RFDEVICE_CREATE_SCRIPT,
                        timing_info.start_delta);

    asm_cfg_s.trigger_type = RFDEVICE_ASM_TRIGGER_DISABLE_TX;

    rfdevice_asm_disable(&asm_cfg_s,
                         dev_obj,
                         (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_stop_script[TX_ASM_SCRIPT],
                         RFDEVICE_CREATE_SCRIPT,
                         timing_info.stop_delta);

    rf_common_leave_critical_section( &(((rfdevice_asm *)dev_obj)->rfdevice_critical_sect));

    status = TRUE;
    break;

  case RFDEVICE_PA:
    /*Get the PA timing from rfc */
    rfc_common_tdd_get_grfc_info(rfc_gsm_tx_sig_list_ptrs[rfm_dev][band], RFC_TIMING_PA_CTL, &timing_info);
    pa_params_s.mode = RFM_EGSM_MODE;
    pa_params_s.band = rfcom_band;
    pa_params_s.bw = RFDEVICE_PA_BW_NOT_APPLICABLE;
    pa_params_s.trigger_type = RFDEVICE_PA_TRIGGER_ON_OFF;
    pa_params_s.rfm_device = rfm_dev;

    rfdevice_set_pa_on_off( &pa_params_s, TRUE, (void *)dev_obj,
                            (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_start_script[TX_PA_SCRIPT],
                            RFDEVICE_CREATE_SCRIPT,
                            timing_info.start_delta);

    rfdevice_set_pa_on_off( &pa_params_s, FALSE, (void *)dev_obj,
                            (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_stop_script[TX_PA_SCRIPT],
                            RFDEVICE_CREATE_SCRIPT,
                            timing_info.stop_delta);

    status = TRUE;
    break;

  case RFDEVICE_TUNER_MANAGER:
    rfc_common_tdd_get_grfc_info(rfc_gsm_tx_sig_list_ptrs[rfm_dev][band],
                                 RFC_TIMING_TUNER_CTL, &timing_info);

    ret_val = ((rfcommon_atuner_intf*)dev_obj)->set_tx_mode(RFM_EGSM_MODE,
               rfcom_band,
               arfcn,
               RFDEVICE_CREATE_SCRIPT,
               (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_start_script[TX_ATUNER_SCRIPT],
               timing_info.start_delta,
               &script_token,
               &cl_info
               );

    if (ret_val && (script_token != ATUNER_INVALID_TOKEN_VAL))
    {
      ((rfcommon_atuner_intf*)dev_obj)->acknowledge_script(script_token);
    }

    status = ret_val;
    break;

  case RFDEVICE_TRANSCEIVER:
  case RFDEVICE_TRANSMITTER:
  case RFDEVICE_RECEIVER:	
  case RFDEVICE_TUNER:
  case RFDEVICE_THERM:
  case RFDEVICE_HDET:
  case RFDEVICE_COUPLER:
#ifdef FEATURE_RF_ASDIV
  case RFDEVICE_XSW:
#endif
    /* nothing todo here, transceiver configuration occurs in the core/mc */
    status = TRUE;
    break;

  case RFDEVICE_PAPM:

    if (( (RF_HW_WTR2965_NON_CA_4373_V2_SAW == (rf_hw_type)rfcommon_nv_get_rf_config())) ||
                  (RF_HW_WTR2965_NON_CA_4373_V2 == (rf_hw_type)rfcommon_nv_get_rf_config()) )
    {
      for(slot_count=0; slot_count < rfgsm_core_handle_ptr->tx_log.num_slots; slot_count++)
      {

        bias = rfgsm_core_handle_ptr->tx_log.tx_slot_log[slot_count].smps_pdm;
        apt_mode= rfgsm_core_handle_ptr->tx_log.tx_slot_log[slot_count].apt_mode;

        /* Use tx_start_script if it's the very first or only Tx burst, use apt_script for interslot APT control in multislot scenario  */
        if (slot_count)
          apt_script_buf = (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.apt_script[slot_count];
        else
          apt_script_buf = (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_start_script[TX_APT_SCRIPT];

        if ((apt_mode == FORCED_APT_MODE) || (apt_mode == REGULAR_APT_MODE))
        {
           mode = RFDEVICE_APT_MODE;
        }
        else
        {
          mode = RFDEVICE_BYPASS_MODE;
        }

        // Clearing the APT buffer
        rf_buffer_clear_script( apt_script_buf, RF_BUFFER_ALL);

        if (slot_count)
        {
          // if RFC_TIMING_PAPM_MULTISLOT_CTL timing parameter doesn't exist in RFC, multislot APT feature is not supported - break out
          if(!rfc_common_tdd_get_grfc_info(rfc_gsm_tx_sig_list_ptrs[rfm_dev][band], RFC_TIMING_PAPM_MULTISLOT_CTL, &timing_info))
          {
            status = TRUE;
            break;
          }
        }
        else
        {
          // populate the timing info with values from autogen file
          rfc_common_tdd_get_grfc_info(rfc_gsm_tx_sig_list_ptrs[rfm_dev][band], RFC_TIMING_PAPM_CTL, &timing_info);
        }

        ((rfdevice_papm *)dev_obj)->set_mode_bias(rfm_dev,
                                                  RFM_EGSM_MODE,
                                                  rfcom_band,
                                                  mode,
                                                  bias,
                                                  apt_script_buf,
                                                  RFDEVICE_CREATE_SCRIPT,
                                                  timing_info.start_delta);
        ((rfdevice_papm *)dev_obj)->trigger(apt_script_buf,
                                            rfm_dev,
                                            RFDEVICE_CREATE_SCRIPT,
                                            timing_info.start_delta);
      }

      if (rfgsm_core_handle_ptr->tx_log.num_slots > 0)
        {
          // populate the timing info with values from autogen file
          rfc_common_tdd_get_grfc_info(rfc_gsm_tx_sig_list_ptrs[rfm_dev][band], RFC_TIMING_PAPM_CTL, &timing_info);

          /*timing_info.stop_delta = 100; */ /* ftm_gsm_qpoet_stop_delta;*/	  /*250; */

         ((rfdevice_papm *)dev_obj)->set_mode_bias(rfm_dev,
                                                    RFM_EGSM_MODE,
                                                    rfcom_band,
                                                    RFDEVICE_STANDBY_MODE,
                                                    500,
                                                    (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_stop_script[TX_APT_SCRIPT],
                                                    RFDEVICE_CREATE_SCRIPT,
                                                    timing_info.stop_delta);

         ((rfdevice_papm *)dev_obj)->trigger((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_stop_script[TX_APT_SCRIPT],
                                              rfm_dev,
                                              RFDEVICE_CREATE_SCRIPT,
                                              timing_info.stop_delta);
        }
    }

   status = TRUE;
   break;

  default:
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf-front-end device NOT Configured!", 0);
    break;
  }

  return status;

}


/* ----------------------------------------------------------------------- */
/*!
  @brief
  Configure the rf-card Tx gain with the specified pa gain state.

  @details
  Configure the rf-card for the specified Tx gain. Populate the the required GRFCs and gain based
  logic information to the specified buffer.

*/
void rfc_gsm::tx_gain_override(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band,
                               rfgsm_modulation_type mod_type, uint8 pa_range,
                               rf_buffer_intf *tx_trans_buffer, boolean bias_flag,
                               uint16 bias, uint8 apt_mode, boolean override_start_time)
{
  uint8 i = 0;
  boolean status = FALSE;
  rfc_cfg_params_type cfg = { RF_PATH_0, rfm_dev, RFC_CONFIG_TX, band, RFC_REQ_DEFAULT_GET_DATA};
  rfdevice_class **dev_list = NULL;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  uint32 pa_range_map = 0;
  uint8 pa_range_logic = 0;
  boolean default_pa_range_map_flag = FALSE;
  //uint16 pcl_index_pa_bias = 0xFFFF;
  //const uint16* pcl_index_pa_bias_ptr = &pcl_index_pa_bias;

  cfg.alternate_path = 0;
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  uint32 instance = 0;

  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return;
  }

  //pcl_index_pa_bias = rfgsm_core_handle_ptr->pcl_index_pa_bias;
  rfc_gsm_data *rfc_data = rfc_gsm_data::get_instance();
  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC GSM Data Object is NOT Created", 0);
    return;
  }

  if (rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl != NULL)
  {
    pa_range_map = rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->pa_range_map;

    if (pa_range_map == 0xFFFFFFFF) /* PA range map NVs not found*/
    {
      /*Load default pa range map*/
      default_pa_range_map_flag = TRUE;
    }
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_gsm::tx_gain_override, TX NV table is NULL", 0);

  }

  if (band >= RFCOM_NUM_GSM_BANDS || tx_trans_buffer == NULL || pa_range >= RFGSM_MAX_TX_GAIN_RANGE)
  {
    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid parameters band=%d, pa_range=%d, tx_trans_buffer=0x%x",
          band, pa_range, tx_trans_buffer);
    return;
  }


  rf_buffer_clear_script(tx_trans_buffer, RF_BUFFER_RFFE);
  rf_buffer_clear_script(tx_trans_buffer, RF_BUFFER_GRFC);

  /* pa range0 GRFC info  */
  if (rfc_gsm_pa_range0_grfc_info[rfm_dev].grfc_id != RF_HAL_GRFC_MAX_NUM)
  {
    if (default_pa_range_map_flag == FALSE)
    {
      pa_range_logic = ((pa_range_map >> (pa_range*NUM_PA_RANGE_GRFC)) & 0xF);
      pa_range_logic = (pa_range_logic >> 0) & 0x1;
    }
    else
    {
      pa_range_logic = rfc_gsm_pa_gain_range_map.pa_gain_map[band][pa_range][0];
    }

    tx_trans_buffer->append_grfc(rfc_gsm_pa_range0_grfc_info[rfm_dev].grfc_id,
                                 pa_range_logic,
                                 rfc_gsm_pa_range0_grfc_info[rfm_dev].start_delta);

  }

  /* pa range1 GRFC info  */
  if (rfc_gsm_pa_range1_grfc_info[rfm_dev].grfc_id != RF_HAL_GRFC_MAX_NUM)
  {

    if (default_pa_range_map_flag == FALSE)
    {
      pa_range_logic = ((pa_range_map >> (pa_range*NUM_PA_RANGE_GRFC)) & 0xF);
      pa_range_logic = (pa_range_logic >> 1) & 0x1;
    }
    else
    {
      pa_range_logic = rfc_gsm_pa_gain_range_map.pa_gain_map[band][pa_range][1];
    }

    tx_trans_buffer->append_grfc(rfc_gsm_pa_range1_grfc_info[rfm_dev].grfc_id,
                                 pa_range_logic,
                                 rfc_gsm_pa_range1_grfc_info[rfm_dev].start_delta);
  }

  /*loop through all device types*/
  for (i = 0; i < RFDEVICE_TYPE_MAX_NUM; i++)
  {
    dev_list = get_rf_device_list(rfm_dev, cfg.rx_tx, band, (rfdevice_type_enum_type)i);
    uint8 index = 0;

    if (dev_list != NULL)
    {
      while (dev_list[index] != NULL && index < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
      {
        status = tx_gain_device_config(rfm_dev, dev_list[index], (rfcom_gsm_band_type)cfg.band,
                                       pa_range, mod_type, tx_trans_buffer, 0,
                                       (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_start_script[TX_APT_SCRIPT],
                                       bias_flag , bias, apt_mode, override_start_time);
        if (status == FALSE)
        {
          MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"Failed to configure Tx rf-devices! rfm_dev=%d,band=%d,dev_type=%d",
                cfg.logical_device, cfg.band, (rfdevice_type_enum_type)i);
        }

        index++;
      }
    }
  }
}

/* ----------------------------------------------------------------------- */
/*!
  @brief
  Configure the rf-card for the specified Tx gain.

  @details
  Configure the rf-card for the specified Tx gain. Populate the the required GRFCs and gain based
  logic information to the specified buffer.

*/
void rfc_gsm::tx_gain_config( rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band,
                              rfgsm_modulation_type mod_type, uint8 slot_num, int32 pwr_in_dbm,
                              uint8 pa_range, rf_buffer_intf *tx_trans_buffer, boolean bias_flag,
                              uint16 bias, uint8 apt_mode, boolean override_start_time)
{
  uint8 i = 0;
  boolean status = FALSE;
  rfc_cfg_params_type cfg = { RF_PATH_0, rfm_dev, RFC_CONFIG_TX, band, RFC_REQ_DEFAULT_GET_DATA};
  rfdevice_class **dev_list = NULL;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  rfc_gsm_data *rfc_data = NULL;
  uint32 instance = 0;
  uint8 pa_range_logic = 0;
  boolean default_pa_range_map_flag =  FALSE;
  rf_buffer_intf *apt_script_buf = NULL;

  uint32 pa_range_map = 0;
  //uint16 pcl_index_pa_bias = 0xffff;
  //const uint16* pcl_index_pa_bias_ptr = &pcl_index_pa_bias;

  cfg.alternate_path = 0;
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return;
  }

  //pcl_index_pa_bias = rfgsm_core_handle_ptr->pcl_index_pa_bias;

  rfc_data = rfc_gsm_data::get_instance();
  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC GSM Data Object is NOT Created", 0);
    return;
  }

  if (rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl != NULL)
  {
    pa_range_map = rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->pa_range_map;


    if (pa_range_map == 0xFFFFFFFF) /* PA range map NVs not found*/
    {
      /*Load default pa range map*/
      default_pa_range_map_flag = TRUE;
    }
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_gsm::tx_gain_config, TX NV table is NULL", 0);

  }

  if (band >= RFCOM_NUM_GSM_BANDS || tx_trans_buffer == NULL || pa_range >= RFGSM_MAX_TX_GAIN_RANGE)
  {
    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid parameters band=%d, pa_range=%d, tx_trans_buffer=0x%x",
          band, pa_range, tx_trans_buffer);
    return;
  }

  rf_buffer_clear_script(tx_trans_buffer, RF_BUFFER_ALL);
#if 0
  /* toggle RF_ON GRFC in multi-slot case */
  /* RF_ON GRFC needs special handling for transition
    buffer. */
  if (rfc_gsm_rf_on_grfc_info[rfm_dev].grfc_id != RF_HAL_GRFC_MAX_NUM && slot_num > 0)
  {
    tx_trans_buffer->append_grfc(rfc_gsm_rf_on_grfc_info[rfm_dev].grfc_id, 0, 0);
    tx_trans_buffer->append_grfc(rfc_gsm_rf_on_grfc_info[rfm_dev].grfc_id, 1, 1);
  }
#endif
  /* pa range0 GRFC info  */
  if (rfc_gsm_pa_range0_grfc_info[rfm_dev].grfc_id != RF_HAL_GRFC_MAX_NUM)
  {
    if (default_pa_range_map_flag == FALSE)
    {
      pa_range_logic = ((pa_range_map >> (pa_range*NUM_PA_RANGE_GRFC)) & 0xF);
      pa_range_logic = (pa_range_logic >> 0 /* PA RANGE 0 GRFC */) & 0x1;
    }
    else
    {
      pa_range_logic = rfc_gsm_pa_gain_range_map.pa_gain_map[band][pa_range][0];
    }

    tx_trans_buffer->append_grfc(rfc_gsm_pa_range0_grfc_info[rfm_dev].grfc_id,
                                 pa_range_logic,
                                 rfc_gsm_pa_range0_grfc_info[rfm_dev].start_delta);



  }

  /* pa range1 GRFC info  */
  if (rfc_gsm_pa_range1_grfc_info[rfm_dev].grfc_id != RF_HAL_GRFC_MAX_NUM)
  {

    if (default_pa_range_map_flag == FALSE)
    {
      pa_range_logic = ((pa_range_map >> (pa_range*NUM_PA_RANGE_GRFC)) & 0xF);
      pa_range_logic = (pa_range_logic >> 1) & 0x1;
    }
    else
    {
      pa_range_logic = rfc_gsm_pa_gain_range_map.pa_gain_map[band][pa_range][1];
    }


    tx_trans_buffer->append_grfc(rfc_gsm_pa_range1_grfc_info[rfm_dev].grfc_id,
                                 pa_range_logic,
                                 rfc_gsm_pa_range1_grfc_info[rfm_dev].start_delta);

  }

  /* GTR threshold GRFC info */
  if((rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl != NULL) && (rfc_gsm_gtr_threshold_grfc_info[rfm_dev].grfc_id != RF_HAL_GRFC_MAX_NUM))
  {
    gsm_tx_gtr_thresh_type gtr_thresh = rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->tx_gtr_thresh;

    /*Comparing current Tx Power with Nv value. Unit used is dBm10 hence division of pwr_in_dbm which is in dbm100.*/
    if ((gtr_thresh.enable == 1) && (pwr_in_dbm/10 >= gtr_thresh.rise_thresh) )
    {
      tx_trans_buffer->append_grfc(rfc_gsm_gtr_threshold_grfc_info[rfm_dev].grfc_id,
                                   1, rfc_gsm_gtr_threshold_grfc_info[rfm_dev].start_delta);
    }
    else
    {
      tx_trans_buffer->append_grfc(rfc_gsm_gtr_threshold_grfc_info[rfm_dev].grfc_id,
                                   0, rfc_gsm_gtr_threshold_grfc_info[rfm_dev].start_delta);
    }
  }

  /* Use tx_start_script if it's the very first or only Tx burst, use apt_script for interslot APT control in multislot scenario  */
  if (slot_num)
    apt_script_buf = (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.apt_script[slot_num];
  else    
    apt_script_buf = (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_start_script[TX_APT_SCRIPT];

  /*loop through all device types*/
  for (i = 0; i < RFDEVICE_TYPE_MAX_NUM; i++)
  {
    dev_list = get_rf_device_list(rfm_dev, cfg.rx_tx, (rfcom_gsm_band_type)cfg.band, (rfdevice_type_enum_type)i);

    if (dev_list != NULL)
    {
      uint8 index = 0;
      while (dev_list[index] != NULL && index < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
      {
        status = tx_gain_device_config(rfm_dev, dev_list[index], (rfcom_gsm_band_type)cfg.band,
                                       pa_range, mod_type, tx_trans_buffer, slot_num,
                                       apt_script_buf,
                                       bias_flag , bias, apt_mode, override_start_time);

        if (status == FALSE)
        {
          MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"Failed to configure Tx rf-devices! rfm_dev=%d,band=%d,dev_type=%d",
                cfg.logical_device, cfg.band, (rfdevice_type_enum_type)i);
        }

        index++;
      }
    }
  }


}

/* ----------------------------------------------------------------------- */
/*!
  @brief
  Append the RF_ON disable/enable transactions for the rf-card configuration
  to the transition script

  @details
  RF_ON can be either ssbi or grfc controlled. If RF_ON GRFC exists in RFC AG,
  use GRFC, otherwise use ssbi control

  @todo
  This function will be used to populate transactions for a dedicated RF_OFF_ON_TRANSITION_EVENT

*/
void rfc_gsm::append_rf_off_on_entries(rfm_device_enum_type rfm_dev, rf_buffer_intf *tx_trans_buffer)
{

  if ( tx_trans_buffer == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid parameters tx_trans_buffer=0x%x",
          tx_trans_buffer);
    return;
  }

#if 0
  rf_buffer_clear_script(tx_trans_buffer, RF_BUFFER_SSBI);
  rf_buffer_clear_script(tx_trans_buffer, RF_BUFFER_RFFE);
  rf_buffer_clear_script(tx_trans_buffer, RF_BUFFER_GRFC);
#endif

  if ( rfm_dev < RFM_MAX_WAN_DEVICES &&
       rfc_gsm_rf_on_grfc_info[rfm_dev].grfc_id != RF_HAL_GRFC_MAX_NUM )
  {
    tx_trans_buffer->append_grfc(rfc_gsm_rf_on_grfc_info[rfm_dev].grfc_id, 0, 0);
    tx_trans_buffer->append_grfc(rfc_gsm_rf_on_grfc_info[rfm_dev].grfc_id, 1, 1);
  }
  return;
}

/* ----------------------------------------------------------------------- */
/*!
  @brief
   Retrieve the rf_on control information for the RF card configuration

  @details
   RF_ON can be either ssbi or grfc controlled. If RF_ON GRFC exists in RFC AG,
   return TRUE, otherwise return FALSE

  @todo
  rfc_gsm_rf_on_grfc_info is not DSDA compliant

*/
boolean rfc_gsm::rf_on_grfc_ctl( rfm_device_enum_type rfm_dev )
{

  if ( rfm_dev < RFM_MAX_WAN_DEVICES &&
       rfc_gsm_rf_on_grfc_info[rfm_dev].grfc_id != RF_HAL_GRFC_MAX_NUM )
  {
    return TRUE;
  }
  return FALSE;
}

/* ----------------------------------------------------------------------- */
/*!
  @brief
   Check if RFC_TIMING_PAPM_MULTISLOT_CTL timing information is present in the RF card configuration

  @details
   Multislot APT feature support is determined by the presence of this timing information, i.e. if this
   timing parameter is not present in the RFC then this feature is not supported

*/
boolean rfc_gsm::is_papm_multislot_ctl_supported( rfm_device_enum_type rfm_dev, rfcom_gsm_band_type rfgsm_band )
{
  rfc_grfc_script_entry_type timing_info = {0};

  if (rfm_dev < RFM_MAX_WAN_DEVICES)
    return rfc_common_tdd_get_grfc_info(rfc_gsm_tx_sig_list_ptrs[rfm_dev][rfgsm_band], RFC_TIMING_PAPM_MULTISLOT_CTL, &timing_info);
  else
    return FALSE;
}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements rf-front-end device configuration for the specified device.

   @details
   This function implements rf-front-end device configuration for the specified device. Based on the
   device type, calls a specific API to configure the device for the specified tx_gain.
*/

boolean rfc_gsm::tx_gain_device_config(rfm_device_enum_type rfm_dev, rfdevice_class *dev_obj, rfcom_gsm_band_type band,
                                       uint8 pa_range, rfgsm_modulation_type mod_type,
                                       rf_buffer_intf *tx_trans_buffer,uint8 slot_num, rf_buffer_intf *apt_buffer,
                                       boolean bias_flag, uint16 bias, uint8 apt_mode, boolean override_start_time)
{
  boolean status = FALSE;
  rfdevice_type_enum_type dev_type = RFDEVICE_TYPE_INVALID;
  rfc_grfc_script_entry_type timing_info = {0};
  rfcom_band_type_u rfcom_band;
  rfdevice_papm_mode_type mode;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  rfgsm_nv_cmn_data_type* rfgsm_nv_cmn_tbl_ptr = NULL;
  uint16 pa_icq = 0;
  boolean use_default_icq_pdm;
  boolean is_mod_type_gmsk;
  rfdevice_pa_config_type pa_params_s;
  rfdevice_pa_txagc_type pa_txagc_s;


  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return FALSE;
  }

  rfgsm_nv_cmn_tbl_ptr = rfgsm_nv_get_cmn_tbl_ptr(rfm_dev);
  if (rfgsm_nv_cmn_tbl_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_nv_cmn_tbl_ptr is NULL!", 0);
    return FALSE;
  }

  if (NULL == dev_obj)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf-front-end device NOT Created! dev_obj = NULL", 0);
    return FALSE;
  }

  rfcom_band.gsm_band = band;

  dev_type = dev_obj->rfdevice_type_get();

  switch (dev_type)
  {
  case RFDEVICE_PA:
    rfc_common_tdd_get_grfc_info(rfc_gsm_tx_sig_list_ptrs[rfm_dev][band], RFC_TIMING_PA_RANGE, &timing_info);

    is_mod_type_gmsk = (mod_type == RF_MOD_GMSK)?TRUE:FALSE;

    use_default_icq_pdm = 1 - rfgsm_nv_cmn_tbl_ptr->pa_icq_bias_nv_enable;

    if(rfgsm_nv_cmn_tbl_ptr->pa_icq_bias_nv_enable)
    {
      if( (pa_range < MAX_NUM_OF_GSM_PA_GAIN_STATE) && (mod_type < MAX_NUM_OF_GSM_MOD_TYPES) && (rfgsm_core_handle_ptr->current_rgi < MAX_NUM_OF_GSM_RGI_INDEX) )
      {
        pa_icq = (uint16)((rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->pa_icq_bias_data.pa_icq_bias[pa_range][mod_type][rfgsm_core_handle_ptr->current_rgi]) & 0xFFFF);
      }
      else
      {
        pa_icq = 0;
        MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR, "out-of-range index to get pa_icq_pdm: pa_range = %d; mod = %d; rgi = %d",pa_range, mod_type, rfgsm_core_handle_ptr->current_rgi);
      }
    }

    rfgsm_core_handle_ptr->tx_log.tx_slot_log[slot_num].icq_bias = pa_icq;

    pa_params_s.mode = RFM_EGSM_MODE;
    pa_params_s.band = rfcom_band;
    pa_params_s.bw = RFDEVICE_PA_BW_NOT_APPLICABLE;
    pa_params_s.trigger_type = RFDEVICE_PA_TRIGGER_TXAGC;
    pa_params_s.rfm_device = rfm_dev;

    pa_txagc_s.pa_gain_range = pa_range;
    pa_txagc_s.pa_icq_array_count = 1;
    pa_txagc_s.pa_icq_array_p = &pa_icq;
    pa_txagc_s.use_default_icq_pdm= use_default_icq_pdm;
    pa_txagc_s.is_mod_type_gmsk = is_mod_type_gmsk;

    rfdevice_set_pa_txagc(&pa_params_s, &pa_txagc_s, (void *)dev_obj,
                          tx_trans_buffer, RFDEVICE_CREATE_SCRIPT, timing_info.start_delta);

    status = TRUE;
    break;

  case RFDEVICE_PAPM:

    if (apt_mode == FORCED_APT_MODE)
    {
      mode = RFDEVICE_APT_MODE;
    }
    else if (apt_mode == REGULAR_APT_MODE)
    {
      mode = RFDEVICE_APT_MODE;
    }
    else
    {
      mode = RFDEVICE_BYPASS_MODE;
    }

    if (bias_flag)  // if PA bias needs to be set
    {
      // Clearing the APT buffer
      rf_buffer_clear_script( apt_buffer, RF_BUFFER_ALL);

      if(override_start_time)
      {
        timing_info.start_delta = 0;
      }
      else
      {
        if (slot_num)
        {
          // if RFC_TIMING_PAPM_MULTISLOT_CTL timing parameter doesn't exist in RFC, multislot APT feature is not supported - break out
          if (!rfc_common_tdd_get_grfc_info(rfc_gsm_tx_sig_list_ptrs[rfm_dev][band], RFC_TIMING_PAPM_MULTISLOT_CTL, &timing_info))
          {
            status = TRUE;
            break;
          }
        }
        else
        {
          // populate the timing info with values from autogen file
          rfc_common_tdd_get_grfc_info(rfc_gsm_tx_sig_list_ptrs[rfm_dev][band], RFC_TIMING_PAPM_CTL, &timing_info);
        }
      }
		
      ((rfdevice_papm *)dev_obj)->set_mode_bias(rfm_dev,
                                                RFM_EGSM_MODE,
                                                rfcom_band,
                                                mode,
                                                bias,
                                                apt_buffer,
                                                RFDEVICE_CREATE_SCRIPT,
                                                timing_info.start_delta);
      ((rfdevice_papm *)dev_obj)->trigger(apt_buffer,
                                          rfm_dev,
                                          RFDEVICE_CREATE_SCRIPT,
                                          timing_info.start_delta);

    }

    status = TRUE;
    break;

  case RFDEVICE_TRANSCEIVER:
  case RFDEVICE_TRANSMITTER:
  case RFDEVICE_RECEIVER:
  case RFDEVICE_TUNER:
  case RFDEVICE_TUNER_MANAGER:
  case RFDEVICE_THERM:
  case RFDEVICE_HDET:
  case RFDEVICE_ASM:
  case RFDEVICE_COUPLER:
#ifdef FEATURE_RF_ASDIV
  case RFDEVICE_XSW:
#endif
    /* nothing todo here, tranceiver configuration is done from mc/core */
    status = TRUE;
    break;

  default:
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf-front-end device NOT Configured!", 0);
    break;
  }

  return status;
}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns the default rfc tx band config data.

   @details

*/
void *rfc_gsm::tx_band_config_get(rfm_device_enum_type rfm_dev,
                                  rfcom_gsm_band_type rfgsm_band,
                                  rfc_band_config_data_enum_type config_data)
{
  void* data_config_ptr = NULL;

  if (rfgsm_band >= RFCOM_NUM_GSM_BANDS)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"invalid band (band=%d)",rfgsm_band);
    return data_config_ptr;
  }

  return( data_config_ptr );

}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the default RFC cmd dispatch function.

   @details

*/
int32 rfc_gsm::command_dispatch( rfm_device_enum_type rfm_dev, int32 cmd, void *data )
{

  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"unknow RFC GSM command (cmd == %d)",cmd);

  return 1;
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function puts the Tx band RFC signals into default state.

   @details

*/
int32 rfc_gsm::tx_disable( rfm_device_enum_type rfm_dev)
{
  rfc_cfg_params_type cfg = { RF_PATH_0, rfm_dev, RFC_CONFIG_TX, RFCOM_NUM_GSM_BANDS, RFC_REQ_DEFAULT_GET_DATA};
  uint8 i = 0;
  boolean status = FALSE;
  rfdevice_class **dev_list = NULL;
  rfc_sig_cfg_type *sig_ptr = NULL;
  rfc_gsm_data *rfc_data = NULL;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  uint32 instance = 0;
  uint16 arfcn;

  cfg.alternate_path = 0;
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return 0;
  }

  rfc_data = rfc_gsm_data::get_instance();
  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC GSM Data Object is NOT Created", 0);
    return 0;
  }

  /* set the previous band GRFC settings into default state */
  cfg.band = rfgsm_core_handle_ptr->tx_band;
  rfc_data->sig_cfg_data_get(&cfg, &sig_ptr );
  rfc_common_tdd_init_default(sig_ptr);
  
  arfcn = rfgsm_core_handle_ptr->rf_tx_arfcn;
#ifdef FEATURE_TX_SHARING
  /*Clear the PA device scripts before populating*/
  rf_buffer_clear_script((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_disable_script[TX_PA_SCRIPT],
                         RF_BUFFER_ALL);
  /*Clear the APT device scripts before populating*/
  rf_buffer_clear_script((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_disable_script[TX_APT_SCRIPT],
					     RF_BUFFER_ALL);
  /*Clear the FE device scripts before populating*/
  rf_buffer_clear_script((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_disable_script[TX_ATUNER_SCRIPT],
					     RF_BUFFER_ALL);

#endif
//  /* configure the rf-front-end devices */


  /*loop through all device types*/
  for (i = 0; i < RFDEVICE_TYPE_MAX_NUM; i++)
  {
    dev_list = get_rf_device_list(rfm_dev, cfg.rx_tx, (rfcom_gsm_band_type)cfg.band, (rfdevice_type_enum_type)i);

    if (dev_list != NULL)
    {
      uint8 index = 0;
      while (dev_list[index] != NULL && index < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
      {
        status = tx_disable_device_config(dev_list[index], (rfcom_gsm_band_type)cfg.band, arfcn, rfm_dev);

        if (status == FALSE)
        {

          MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"Failed to disable Tx rf-devices! rfm_dev=%d,band=%d,dev_type=%d",
                cfg.logical_device, cfg.band, (rfdevice_type_enum_type)i);
        }

        index++;
      }
    }
  }

  return 1;
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements rf-front-end device configuration for the specified device.

   @details
   This function implements rf-front-end device configuration for the specified device. Based on the
   device type, calls a specific API to configure the device to disable tx.
*/
boolean rfc_gsm::tx_disable_device_config(rfdevice_class *dev_obj, rfcom_gsm_band_type rfgsm_band, uint16 arfcn, rfm_device_enum_type rfm_dev)
{
  boolean status = FALSE;
  rfdevice_type_enum_type dev_type = RFDEVICE_TYPE_INVALID;
  rfdevice_pa_config_type pa_params_s;

  rfcom_band_type_u band;  
  uint32 script_token;  
#ifdef FEATURE_TX_SHARING
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
#endif

  if (NULL == dev_obj)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf-front-end device NOT Created! dev_obj = NULL", 0);
    return FALSE;
  }
#ifdef FEATURE_TX_SHARING
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
#endif
  dev_type = dev_obj->rfdevice_type_get();
  switch (dev_type)
  {
  case RFDEVICE_PA:
    pa_params_s.mode = RFCOM_GSM_MODE;
    pa_params_s.band.gsm_band = RFCOM_BAND_GSM900;
    pa_params_s.bw = RFDEVICE_PA_BW_NOT_APPLICABLE;
    pa_params_s.trigger_type = RFDEVICE_PA_TRIGGER_WAKEUP;
    pa_params_s.rfm_device = RFM_DEVICE_0;
#ifndef FEATURE_TX_SHARING
    rfdevice_pa_sleep(&pa_params_s, dev_obj, NULL, RFDEVICE_EXECUTE_IMMEDIATE, RFCOM_INVALID_TIMING_OFFSET);
#else
    rfdevice_pa_sleep(&pa_params_s,
					dev_obj,
					(rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_disable_script[TX_PA_SCRIPT],
					RFDEVICE_CREATE_SCRIPT,
					RFCOM_INVALID_TIMING_OFFSET);

#endif
    status = TRUE;
    break;

  case RFDEVICE_PAPM:

#ifndef FEATURE_TX_SHARING
    ((rfdevice_papm *)dev_obj)->disable(rfm_dev, RFM_EGSM_MODE, NULL, RFDEVICE_EXECUTE_IMMEDIATE);
    ((rfdevice_papm *)dev_obj)->trigger(NULL, rfm_dev, RFDEVICE_EXECUTE_IMMEDIATE,0);
#else
    ((rfdevice_papm *)dev_obj)->disable(rfm_dev, RFM_EGSM_MODE,
                                        (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_disable_script[TX_APT_SCRIPT],
                                        RFDEVICE_CREATE_SCRIPT);
    ((rfdevice_papm *)dev_obj)->trigger((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_disable_script[TX_APT_SCRIPT],
                                        rfm_dev,
                                        RFDEVICE_CREATE_SCRIPT,
                                        0);
#endif
    status = TRUE;
    break;

  case RFDEVICE_TUNER_MANAGER:
    band.gsm_band = rfgsm_band;    
#ifndef FEATURE_TX_SHARING
    status = ((rfcommon_atuner_intf*)dev_obj)->tx_disable(RFM_EGSM_MODE, band, arfcn, RFDEVICE_EXECUTE_IMMEDIATE, NULL, 0, &script_token );
#else
    status = ((rfcommon_atuner_intf*)dev_obj)->tx_disable(RFM_EGSM_MODE,
                                                          band,
                                                          arfcn,
                                                          RFDEVICE_CREATE_SCRIPT,
                                                          (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_disable_script[TX_ATUNER_SCRIPT],
                                                          0,
                                                          &script_token );
#endif

    /* No need to ACK tuner when the tuner script construction is in immediate mode*/
    if (!status)
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "TX RFDEVICE_TUNER_MANAGER tx_disable failed: token %d", script_token);    
    }
    break;

  case RFDEVICE_TRANSCEIVER:
  case RFDEVICE_TRANSMITTER:
  case RFDEVICE_RECEIVER:
  case RFDEVICE_TUNER:

  case RFDEVICE_THERM:
  case RFDEVICE_HDET:
  case RFDEVICE_ASM:
  case RFDEVICE_COUPLER:
#ifdef FEATURE_RF_ASDIV
  case RFDEVICE_XSW:
#endif
    /* nothing todo here, tranceiver configuration is done from mc/core */
    status = TRUE;
    break;

  default:
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf-front-end device NOT Configured!", 0);
    break;
  }

  return status;
}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function configures the RFC signals for tx.

   @details

*/
int32 rfc_gsm::tx_enable( rfm_device_enum_type rfm_dev)
{
  rfc_cfg_params_type cfg = { RF_PATH_0, rfm_dev, RFC_CONFIG_TX, RFCOM_NUM_GSM_BANDS, RFC_REQ_INIT};
  rfc_sig_cfg_type *sig_ptr = NULL;
  uint8 i = 0;
  boolean status = FALSE;
  rfdevice_class **dev_list = NULL;
  uint32 instance = 0;

  cfg.alternate_path = 0;
  rfc_gsm_data *rfc_data = rfc_gsm_data::get_instance();

#ifdef FEATURE_TX_SHARING
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
#endif

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC GSM Data Object is NOT Created", 0);
    return 0;
  }

#ifdef FEATURE_TX_SHARING
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
#endif
  /* set the GRFC settings to enable Tx */
  rfc_data->sig_cfg_data_get(&cfg, &sig_ptr );
  rfc_common_tdd_config_signal(sig_ptr);

  /* configure the rf-front-end devices */

  /* until the QPOET support scripting, we have set the bias with immediate write.
     Once, QPOET supports scripting we should remove the hard-coding of band and the
     init_flag. */
  cfg.band = RFCOM_BAND_GSM900;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;

#ifdef FEATURE_TX_SHARING
  /*Clear the APT device scripts before populating*/
  rf_buffer_clear_script((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_enable_script[TX_APT_SCRIPT],
                         RF_BUFFER_ALL);

  /*Clear the PA device scripts before populating*/
  rf_buffer_clear_script((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_enable_script[TX_PA_SCRIPT],
                         RF_BUFFER_ALL);
#endif

  /*loop through all device types*/
  for (i = 0; i < RFDEVICE_TYPE_MAX_NUM; i++)
  {
    dev_list = get_rf_device_list(rfm_dev, cfg.rx_tx, (rfcom_gsm_band_type)cfg.band, (rfdevice_type_enum_type)i);

    if (dev_list != NULL)
    {
      uint8 index = 0;
      while (dev_list[index] != NULL && index < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
      {
        status = tx_enable_device_config(dev_list[index], rfm_dev);

        if (status == FALSE)
        {

          MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"Failed to enable Tx rf-devices! rfm_dev=%d,band=%d,dev_type=%d",
                cfg.logical_device, cfg.band, (rfdevice_type_enum_type)i);
        }

        index++;
      }
    }
  }

  return 1;
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements rf-front-end device configuration for the specified device.

   @details
   This function implements rf-front-end device configuration for the specified device. Based on the
   device type, calls a specific API to configure the device to enable tx.
*/

boolean rfc_gsm::tx_enable_device_config(rfdevice_class *dev_obj, rfm_device_enum_type rfm_dev)
{
  boolean status = FALSE;
  rfdevice_type_enum_type dev_type = RFDEVICE_TYPE_INVALID;
  rfcom_band_type_u rfcom_band;
  rfdevice_pa_config_type pa_params_s;

#ifdef FEATURE_TX_SHARING
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
#endif
  if (NULL == dev_obj)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf-front-end device NOT Created! dev_obj = NULL", 0);
    return FALSE;
  }
#ifdef FEATURE_TX_SHARING
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!", FALSE);
#endif
  rfcom_band.gsm_band = RFCOM_BAND_GSM900;

  dev_type = dev_obj->rfdevice_type_get();
  switch (dev_type)
  {
  case RFDEVICE_PA:
    pa_params_s.mode = RFCOM_GSM_MODE;
    pa_params_s.band = rfcom_band;
    pa_params_s.bw = RFDEVICE_PA_BW_NOT_APPLICABLE;
    pa_params_s.trigger_type = RFDEVICE_PA_TRIGGER_WAKEUP;
    pa_params_s.rfm_device = RFM_DEVICE_0;
#ifndef FEATURE_TX_SHARING
    rfdevice_pa_wakeup(&pa_params_s, dev_obj, NULL, RFDEVICE_EXECUTE_IMMEDIATE, RFCOM_INVALID_TIMING_OFFSET);
#else
	rfdevice_pa_wakeup(&pa_params_s,
                        dev_obj,
                        (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_enable_script[TX_PA_SCRIPT],
                        RFDEVICE_CREATE_SCRIPT,
                        RFCOM_INVALID_TIMING_OFFSET);
#endif
    status = TRUE;
    break;

  case RFDEVICE_PAPM:
#ifndef FEATURE_TX_SHARING
    ((rfdevice_papm *)dev_obj)->enable(rfm_dev, RFM_EGSM_MODE, rfcom_band, RFDEVICE_PAPM_BW_NOT_APPLICABLE, NULL, RFDEVICE_EXECUTE_IMMEDIATE);
    ((rfdevice_papm *)dev_obj)->trigger(NULL, rfm_dev, RFDEVICE_EXECUTE_IMMEDIATE,0);
#else
    ((rfdevice_papm *)dev_obj)->enable(rfm_dev, 
                                       RFM_EGSM_MODE,
                                       rfcom_band,
                                       RFDEVICE_PAPM_BW_NOT_APPLICABLE,
                                       (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_enable_script[TX_APT_SCRIPT],
                                       RFDEVICE_CREATE_SCRIPT);
    ((rfdevice_papm *)dev_obj)->trigger((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_enable_script[TX_APT_SCRIPT],
		                        rfm_dev, RFDEVICE_CREATE_SCRIPT,0);
#endif
    status = TRUE;
    break;

  case RFDEVICE_TRANSCEIVER:
  case RFDEVICE_TRANSMITTER:
  case RFDEVICE_RECEIVER:
  case RFDEVICE_TUNER:
  case RFDEVICE_TUNER_MANAGER:
  case RFDEVICE_THERM:
  case RFDEVICE_HDET:
  case RFDEVICE_ASM:
  case RFDEVICE_COUPLER:
#ifdef FEATURE_RF_ASDIV
  case RFDEVICE_XSW:
#endif
    /* nothing todo here, tranceiver configuration is done from mc/core */
    status = TRUE;
    break;

  default:
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf-front-end device NOT Configured!", 0);
    break;
  }

  return status;
}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function puts the Init and Rx band RFC signals into default state.

   @details

*/
int32 rfc_gsm::sleep( rfm_device_enum_type rfm_dev)
{
  rfc_cfg_params_type cfg = { RF_PATH_0, rfm_dev, RFC_CONFIG_RX, RFCOM_NUM_GSM_BANDS, RFC_REQ_INIT};
  rfc_sig_cfg_type *sig_ptr = NULL;
  uint8 i = 0;
  boolean status = FALSE;
  rfdevice_class **dev_list = NULL;
  rfc_gsm_data *rfc_data = NULL;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  uint32 instance = 0;
  uint8 band_local;
  cfg.alternate_path = 0;
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  boolean gsm_rf_devices_sleep_list[RFDEVICE_TYPE_MAX_NUM][RFC_MAX_DEVICE_INSTANCES];
  
  //Add the devices which we want to be put to Sleep. 
  //This device list should be same as that during wakeup. 
  rfdevice_type_enum_type rx_device_sleep_list[2]={RFDEVICE_ASM, RFDEVICE_TUNER_MANAGER};

  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return 0;
  }

  rfc_data = rfc_gsm_data::get_instance();
  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC GSM Data Object is NOT Created", 0);
    return 0;
  }

  /* set the gsm rx init_table GRFC settings into default state */
  cfg.rx_tx = RFC_CONFIG_RX;
  rfc_data->sig_cfg_data_get(&cfg, &sig_ptr );
  rfc_common_tdd_init_default(sig_ptr);

  /* set the gsm tx init_table GRFC settings into default state */
  sig_ptr = NULL;
  cfg.rx_tx = RFC_CONFIG_TX;
  cfg.band = rfgsm_core_handle_ptr->band;
  rfc_data->sig_cfg_data_get(&cfg, &sig_ptr );
  rfc_common_tdd_init_default(sig_ptr);

  /*get ASM device for current band on TX path*/
  dev_list = get_rf_device_list(rfm_dev, cfg.rx_tx, (rfcom_gsm_band_type)cfg.band, RFDEVICE_ASM);
  if (dev_list != NULL)
  {
    uint8 index = 0;
    while (dev_list[index] != NULL && index < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
    {
      /* call sleep TX API*/
      sleep_device_config_tx(dev_list[index]);
      index++;
    }
  }
  
  /* set the gsm rx band_table GRFC settings into default state */
  sig_ptr = NULL;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.band = rfgsm_core_handle_ptr->band;
  rfc_data->sig_cfg_data_get(&cfg, &sig_ptr );
  rfc_common_tdd_init_default(sig_ptr);  

  memset(&gsm_rf_devices_sleep_list[0][0], 0, sizeof(gsm_rf_devices_sleep_list));
  
  /*loop through all bands for RX path*/
  for (band_local=0; band_local<(int)RFCOM_NUM_GSM_BANDS; band_local++)
  {
    cfg.band = band_local;

    /* get Sleep device list as in above array*/
    for (i = 0; i < (sizeof(rx_device_sleep_list)/sizeof(rfdevice_type_enum_type)); i++)
    {
      dev_list = get_rf_device_list(rfm_dev, cfg.rx_tx, (rfcom_gsm_band_type)cfg.band, rx_device_sleep_list[i]);

      if (dev_list != NULL)
      {
        uint8 index = 0;
        while (dev_list[index] != NULL && index < RFC_MAX_DEVICE_INSTANCES_PER_BAND && !gsm_rf_devices_sleep_list[rx_device_sleep_list[i]][index])
        {
          status = sleep_device_config(dev_list[index]);
  
          if (status == FALSE)
          {
  
            MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"Failed to configure rf-devices! rfm_dev=%d,band=%d,dev_type=%d",
                  cfg.logical_device, cfg.band, (rfdevice_type_enum_type)i);
          }
          gsm_rf_devices_sleep_list[rx_device_sleep_list[i]][index] = status;
          index++;
        }
      }
    }
  }
  return 1;
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements rf-front-end device configuration for the specified device.

   @details
   This function implements rf-front-end device configuration for the specified device. Based on the
   device type, calls a specific API to configure the device to sleep mode.
*/
boolean rfc_gsm::sleep_device_config_tx(rfdevice_class *dev_obj)
{
  boolean status = FALSE;
  rfdevice_type_enum_type dev_type = RFDEVICE_TYPE_INVALID;
  uint32 script_token;
  rfdevice_asm_cfg_type asm_cfg_s;
  if (NULL == dev_obj)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf-front-end device NOT Created! dev_obj = NULL", 0);
    return FALSE;
  }

  dev_type = dev_obj->rfdevice_type_get();
  switch (dev_type)
  {
  case RFDEVICE_ASM:

    asm_cfg_s.rfm_device = RFM_DEVICE_0;
    asm_cfg_s.mode = RFM_EGSM_MODE;
    asm_cfg_s.band.gsm_band = RFCOM_BAND_GSM900;
    asm_cfg_s.asm_rx_tx = RFDEVICE_ASM_TX_0;
    asm_cfg_s.asm_cell = RFDEVICE_ASM_CELL_0;
    asm_cfg_s.trigger_type = RFDEVICE_ASM_TRIGGER_SLEEP;
    rfdevice_asm_sleep(&asm_cfg_s, dev_obj, NULL, RFDEVICE_EXECUTE_IMMEDIATE, RFCOM_INVALID_TIMING_OFFSET);

    status = TRUE;
    break;

  case RFDEVICE_TUNER_MANAGER:    
  case RFDEVICE_TRANSCEIVER:
  case RFDEVICE_TRANSMITTER:
  case RFDEVICE_RECEIVER:
  case RFDEVICE_PA:
  case RFDEVICE_TUNER:
  case RFDEVICE_PAPM:
  case RFDEVICE_THERM:
  case RFDEVICE_HDET:
  case RFDEVICE_COUPLER:
#ifdef FEATURE_RF_ASDIV
  case RFDEVICE_XSW:
#endif
    /* nothing todo here, tranceiver configuration is done from mc/core */
    status = TRUE;
    break;

  default:
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf-front-end device NOT Configured!", 0);
    break;
  }


  return status;
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements rf-front-end device configuration for the specified device.

   @details
   This function implements rf-front-end device configuration for the specified device. Based on the
   device type, calls a specific API to configure the device to sleep mode.
*/
boolean rfc_gsm::sleep_device_config(rfdevice_class *dev_obj)
{
  boolean status = FALSE;
  rfdevice_type_enum_type dev_type = RFDEVICE_TYPE_INVALID;
  uint32 script_token;
  rfdevice_asm_cfg_type asm_cfg_s;
  if (NULL == dev_obj)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf-front-end device NOT Created! dev_obj = NULL", 0);
    return FALSE;
  }

  dev_type = dev_obj->rfdevice_type_get();
  switch (dev_type)
  {
  case RFDEVICE_ASM:

    asm_cfg_s.rfm_device = RFM_DEVICE_0;
    asm_cfg_s.mode = RFM_EGSM_MODE;
    asm_cfg_s.band.gsm_band = RFCOM_BAND_GSM900;
    asm_cfg_s.asm_rx_tx = RFDEVICE_ASM_RX_0;
    asm_cfg_s.asm_cell = RFDEVICE_ASM_CELL_0;
    asm_cfg_s.trigger_type = RFDEVICE_ASM_TRIGGER_NONE;

    asm_cfg_s.asm_rx_tx = RFDEVICE_ASM_RX_0;
    asm_cfg_s.trigger_type = RFDEVICE_ASM_TRIGGER_SLEEP;
    rf_common_enter_critical_section( &(((rfdevice_asm *)dev_obj)->rfdevice_critical_sect));
    rfdevice_asm_sleep(&asm_cfg_s, dev_obj, NULL, RFDEVICE_EXECUTE_IMMEDIATE, RFCOM_INVALID_TIMING_OFFSET);
    rf_common_leave_critical_section( &(((rfdevice_asm *)dev_obj)->rfdevice_critical_sect));
    status = TRUE;
    break;

  case RFDEVICE_TUNER_MANAGER:
    ((rfcommon_atuner_intf*)dev_obj)->disable(RFDEVICE_EXECUTE_IMMEDIATE, NULL, 0, &script_token );
    /* No need to ACK tuner when the tuner script construction is in immediate mode*/

    status = TRUE;
    break;

  case RFDEVICE_TRANSCEIVER:
  case RFDEVICE_TRANSMITTER:
  case RFDEVICE_RECEIVER:
  case RFDEVICE_PA:
  case RFDEVICE_TUNER:
  case RFDEVICE_PAPM:
  case RFDEVICE_THERM:
  case RFDEVICE_HDET:
  case RFDEVICE_COUPLER:
#ifdef FEATURE_RF_ASDIV
  case RFDEVICE_XSW:
#endif
    /* nothing todo here, tranceiver configuration is done from mc/core */
    status = TRUE;
    break;

  default:
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf-front-end device NOT Configured!", 0);
    break;
  }


  return status;
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function configures the Init RFC signals.

   @details

*/
int32 rfc_gsm::wakeup( rfm_device_enum_type rfm_dev,uint8  alt_path)
{
  rfc_cfg_params_type cfg = { RF_PATH_0, rfm_dev, RFC_CONFIG_RX, RFCOM_NUM_GSM_BANDS, RFC_REQ_INIT};
  rfc_sig_cfg_type *sig_ptr = NULL;
  uint8 i = 0;
  boolean status = FALSE;
  rfdevice_class **dev_list = NULL;
  rfc_gsm_data *rfc_data = NULL;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  uint32 instance = 0;
  uint8 band_local = 0;
  boolean pwr_off_in_sleep = FALSE;
  rf_buffer_intf *enter_mode_buffer = NULL;
  boolean gsm_rf_devices_wakeup_list[RFDEVICE_TYPE_MAX_NUM][RFC_MAX_DEVICE_INSTANCES];

  cfg.alternate_path = (uint32)alt_path;
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return 0;
  }

#ifndef FEATURE_TX_SHARING
  /*! @TODO: allocate shared memory for a wake-up script buffer. */
  enter_mode_buffer = (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.enter_mode_script;
#else
  /*! @TODO: allocate shared memory for a wake-up script buffer. */
  enter_mode_buffer = (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.rx_enter_mode_script;
#endif

  if(rfgsm_core_handle_ptr->rfc_gsm_core_ptr == NULL) {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_gsm_core_ptr is NULL! never mind", 0);
  } else {
    pwr_off_in_sleep = rfgsm_core_handle_ptr->rfc_gsm_core_ptr->pwr_off_in_sleep;
  }

  rfc_data= rfc_gsm_data::get_instance();
  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC GSM Data Object is NOT Created", 0);
    return 0;
  }
  memset(&gsm_rf_devices_wakeup_list[0][0], 0, sizeof(gsm_rf_devices_wakeup_list));
  /* configure the init rx GRFCs */
  rfc_data->sig_cfg_data_get(&cfg, &sig_ptr );
  rfc_common_tdd_config_signal ( sig_ptr);

  /* configure the init tx GRFCs */
  cfg.alternate_path = 0; 
  sig_ptr = NULL;
  cfg.rx_tx = RFC_CONFIG_TX;
  rfc_data->sig_cfg_data_get(&cfg, &sig_ptr );
  rfc_common_tdd_config_signal ( sig_ptr );

  /* configure the rf-front-end devices */
  cfg.alternate_path = (uint32)alt_path; 
  cfg.band = rfgsm_core_handle_ptr->band_before_sleep;
  cfg.rx_tx = RFC_CONFIG_RX;

  for (band_local=0; band_local<(int)RFCOM_NUM_GSM_BANDS; band_local++)
  {
    cfg.band = band_local;

    /*loop through all device types*/
    for (i = 0; i < RFDEVICE_TYPE_MAX_NUM; i++)
    {
      dev_list = get_rf_device_list(rfm_dev, cfg.rx_tx, (rfcom_gsm_band_type)cfg.band, (rfdevice_type_enum_type)i);

      if (dev_list != NULL)
      {
        uint8 index = 0;
        while (dev_list[index] != NULL && index < RFC_MAX_DEVICE_INSTANCES_PER_BAND && !gsm_rf_devices_wakeup_list[i][index])
        {
          status = wakeup_device_config(dev_list[index], enter_mode_buffer, pwr_off_in_sleep);

          if (status == FALSE)
          {

            MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"Failed to configure rf-devices! rfm_dev=%d,band=%d,dev_type=%d",
                  cfg.logical_device, cfg.band, (rfdevice_type_enum_type)i);
          }

          gsm_rf_devices_wakeup_list[i][index] = status;
          index++;
        }
      }

    }
  }



  return 1;
} /*rfc_gsm::wakeup*/

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements rf-front-end device configuration for the specified device.

   @details
   This function implements rf-front-end device configuration for the specified device. Based on the
   device type, calls a specific API to configure the device to wakeup mode.
*/
boolean rfc_gsm::wakeup_device_config(rfdevice_class *dev_obj, rf_buffer_intf *wakeup_buffer, boolean pwr_off_in_sleep)
{
  boolean status = FALSE;
  uint32 script_token;
  rfdevice_asm_cfg_type asm_cfg_s;

  rfdevice_type_enum_type dev_type = RFDEVICE_TYPE_INVALID;
  if (NULL == dev_obj)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf-front-end device NOT Created! dev_obj = NULL", 0);
    return FALSE;
  }

  dev_type = dev_obj->rfdevice_type_get();
  switch (dev_type)
  {
  case RFDEVICE_ASM:

    asm_cfg_s.rfm_device = RFM_DEVICE_0;
    asm_cfg_s.mode = RFM_EGSM_MODE;
    asm_cfg_s.band.gsm_band = RFCOM_BAND_GSM900;
    asm_cfg_s.asm_rx_tx = RFDEVICE_ASM_RX_0;
    asm_cfg_s.asm_cell = RFDEVICE_ASM_CELL_0;
    asm_cfg_s.trigger_type = RFDEVICE_ASM_TRIGGER_WAKEUP;
    rf_common_enter_critical_section( &(((rfdevice_asm *)dev_obj)->rfdevice_critical_sect));
    rfdevice_asm_wakeup(&asm_cfg_s, dev_obj, wakeup_buffer, RFDEVICE_CREATE_SCRIPT, RFCOM_INVALID_TIMING_OFFSET);
    rf_common_leave_critical_section( &(((rfdevice_asm *)dev_obj)->rfdevice_critical_sect));
    status = TRUE;
    break;

  case RFDEVICE_TUNER_MANAGER:
    if(pwr_off_in_sleep == TRUE)
    {
      status = ((rfcommon_atuner_intf*)dev_obj)->init(RFM_EGSM_MODE, RFDEVICE_CREATE_SCRIPT, wakeup_buffer, 0, &script_token);

      if (status && (script_token != ATUNER_INVALID_TOKEN_VAL))
      {
        ((rfcommon_atuner_intf*)dev_obj)->acknowledge_script(script_token);
      }
    }
    else
    {
      status = ((rfcommon_atuner_intf*)dev_obj)->init(RFM_EGSM_MODE, RFDEVICE_EXECUTE_IMMEDIATE, NULL, 0, &script_token);

      /* No need to ACK tuner when the script construction is in immediate mode*/
    }
    break;

  case RFDEVICE_TRANSCEIVER:
  case RFDEVICE_TRANSMITTER:
  case RFDEVICE_RECEIVER:
  case RFDEVICE_PA:
  case RFDEVICE_TUNER:
  case RFDEVICE_PAPM:
  case RFDEVICE_THERM:
  case RFDEVICE_HDET:
  case RFDEVICE_COUPLER:
#ifdef FEATURE_RF_ASDIV
  case RFDEVICE_XSW:
#endif
    /* nothing todo here, tranceiver configuration is done from mc/core */
    status = TRUE;
    break;

  default:
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf-front-end device NOT Configured!", 0);
    break;
  }

  return status;
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements VREG management for GSM mode

   @details

*/
void rfc_gsm::manage_vregs (rf_path_enum_type path, rfcom_gsm_band_type band,
                            rfm_path_state state, rfm_mode_enum_type mode)
{
  rfc_vreg_param vreg_param(path, mode);
  vreg_param.set_band(band);
  vreg_param.set_rfchip_number(RF_CHIP_0);
  vreg_param.set_rf_state(state);
  rfc_vreg_mgr *vreg_mgr_p = rfc_vreg_mgr::get_instance();

  if (vreg_mgr_p != NULL)
  {
    vreg_mgr_p->gsm_manage_power(vreg_param);
  }
  else
  {
    ERR_FATAL("Cannot Create VREG_MGR for for GSM",0,0,0);
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function update the rf signals with NV start/stop timing data.

  @details
  This function update the rf signals with NV start/stop timing data.

  @param band: GSM band for which timing data to be updated.
*/
void rfc_gsm::update_timing_based_on_nv(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band, rfc_sig_cfg_type *rf_signals)
{
  int32 start_time = 0;
  int32 stop_time = 0;
  rfgsm_nv_band_data_type *rfnv_tbl = NULL;
  gsm_tx_static_nv_type *tx_static_nv_data_ptr = NULL;

  if (band >= RFCOM_NUM_GSM_BANDS)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"invalid band = %d", band);
    return;
  }

  rfnv_tbl = rfgsm_nv_get_tbl_ptr(rfm_dev, band, RFGSM_NV_TX);

  if (rfnv_tbl == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"update_timing_based_on_nv NULL NV ptr!", 0);
    return;
  }

  tx_static_nv_data_ptr = rfnv_tbl->tx_static_nv_data_ptr;

  if (tx_static_nv_data_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"update_timing_based_on_nv NULL NV ptr!", 0);
    return;
  }

  /* move the start/stop deltas based on the nv start/stop delta parameter */
  start_time = tx_static_nv_data_ptr->tx_timing_data.pa_start_offset_adj + tx_static_nv_data_ptr->tx_timing_data.pa_en_start_offset_adj;
  stop_time =  tx_static_nv_data_ptr->tx_timing_data.pa_stop_offset_adj  + tx_static_nv_data_ptr->tx_timing_data.pa_en_stop_offset_adj;
  rfc_common_tdd_update_timing(rf_signals, RFC_PA_ON, start_time, stop_time);
  rfc_common_tdd_update_timing(rf_signals, RFC_TIMING_PA_CTL, start_time, stop_time);
  rfc_common_tdd_update_timing(rf_signals, RFC_GNSS_BLANK, start_time, stop_time);

  /* whenever we move the PA_EN start/stop delta, we have to move the ant_sel start/stop deltas
  relatively */
  start_time += tx_static_nv_data_ptr->tx_timing_data.ant_timing_start_offset_adj;
  stop_time  += tx_static_nv_data_ptr->tx_timing_data.ant_timing_stop_offset_adj;
  rfc_common_tdd_update_timing(rf_signals, RFC_ANT_SEL, start_time, stop_time);
  rfc_common_tdd_update_timing(rf_signals, RFC_TIMING_ASM_CTL, start_time, stop_time);
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function update the rf signals with start/stop timing data.

  @details
  This function update the rf signals with start/stop timing data.

  @param band: GSM band for which timing data to be updated.
*/
void rfc_gsm::update_timing_based_on_card(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band, rfc_sig_cfg_type *rf_signals)
{
  rfgsm_nv_band_data_type *rfnv_tbl = NULL;
  int32 start_time = 0;
  int32 stop_time = 0;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  gsm_tx_static_nv_type *tx_static_nv_data_ptr = NULL;

  if (band >= RFCOM_NUM_GSM_BANDS)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"invalid band = %d", band);
    return;
  }

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return;
  }

  /* Get NV table from rfsgm_core_handle*/
  rfnv_tbl = rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl;
  tx_static_nv_data_ptr = rfnv_tbl->tx_static_nv_data_ptr;

  /* move the start/stop deltas based on the nv start/stop delta parameter */
  start_time = rfgsm_core_handle_ptr->rf_pa_start_time +
               tx_static_nv_data_ptr->tx_timing_data.pa_start_offset_adj +
               tx_static_nv_data_ptr->tx_timing_data.pa_en_start_offset_adj;

  stop_time =  rfgsm_core_handle_ptr->rf_pa_stop_time +
               tx_static_nv_data_ptr->tx_timing_data.pa_stop_offset_adj +
               tx_static_nv_data_ptr->tx_timing_data.pa_en_stop_offset_adj;

  rfc_common_tdd_update_timing(rf_signals, RFC_PA_ON, start_time, stop_time);
  rfc_common_tdd_update_timing(rf_signals, RFC_TIMING_PA_CTL, start_time, stop_time);
  rfc_common_tdd_update_timing(rf_signals, RFC_GNSS_BLANK, start_time, stop_time);

  /* whenever we move the PA_EN start/stop delta, we have to move the ant_sel start/stop deltas
  relatively */
  start_time += tx_static_nv_data_ptr->tx_timing_data.ant_timing_start_offset_adj;
  stop_time  += tx_static_nv_data_ptr->tx_timing_data.ant_timing_stop_offset_adj;
  rfc_common_tdd_update_timing(rf_signals, RFC_ANT_SEL, start_time, stop_time);
  rfc_common_tdd_update_timing(rf_signals, RFC_TIMING_ASM_CTL, start_time, stop_time);

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function constructs the GRFC scripts for all bands.

  @details
  This function constructs the GRFC scripts for all bands.

  @param None.
*/
int32 rfc_gsm::tx_timing_adjust(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band)
{
  rfc_cfg_params_type cfg = { RF_PATH_0, rfm_dev, RFC_CONFIG_TX, band, RFC_REQ_DEFAULT_GET_DATA};
  rfc_sig_cfg_type *sig_ptr = NULL;

  cfg.alternate_path = 0;
  rfc_gsm_data *rfc_data = rfc_gsm_data::get_instance();

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC GSM Data Object is NOT Created", 0);
    return 0;
  }

  if (band >= RFCOM_NUM_GSM_BANDS)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"invalid band (band=%d)",band);
    return 0;
  }

  /* construct static and dynamic scripts for each band */
  if( rfm_dev < RFM_MAX_WAN_DEVICES &&
      rfc_data->sig_cfg_data_get(&cfg, &sig_ptr )==TRUE)
  {
    rfc_common_tdd_copy_sig_list(sig_ptr, rfc_gsm_tx_sig_list_ptrs[rfm_dev][band]);
    update_timing_based_on_card(rfm_dev, (rfcom_gsm_band_type)cfg.band, rfc_gsm_tx_sig_list_ptrs[rfm_dev][band]);
  }

  return 1;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function constructs the GRFC scripts for all bands.

  @details
  This function constructs the GRFC scripts for all bands.

  @param None.
*/
void rfc_gsm::construct_tx_burst_script(rfm_device_enum_type rfm_dev)
{
  uint8 i = 0;
  rfc_cfg_params_type cfg = { RF_PATH_0, rfm_dev, RFC_CONFIG_TX, RFCOM_NUM_GSM_BANDS, RFC_REQ_DEFAULT_GET_DATA};
  rfc_sig_cfg_type *sig_ptr = NULL;
  boolean tx_supported = TRUE;

  cfg.alternate_path = 0;
  rfc_gsm_data *rfc_data = rfc_gsm_data::get_instance();

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC GSM Data Object is NOT Created", 0);
    return;
  }


  /* construct tx scripts for each band */
  for (i=0; i < RFCOM_NUM_GSM_BANDS; i++)
  {
    cfg.band = (rfcom_gsm_band_type)i;
    tx_supported = rfc_data->sig_cfg_data_get(&cfg, &sig_ptr );
    if (tx_supported == TRUE)
    {
      if (rfc_gsm_tx_sig_list_ptrs[rfm_dev][i]==NULL)
      {
        rfc_gsm_tx_sig_list_ptrs[rfm_dev][i] = (rfc_sig_cfg_type*)modem_mem_alloc(sizeof(rfc_sig_cfg_type)*RFC_MAX_NUM_OF_GRFC, MODEM_MEM_CLIENT_RFA);
      }
      rfc_common_tdd_copy_sig_list(sig_ptr, rfc_gsm_tx_sig_list_ptrs[rfm_dev][i]);
      update_timing_based_on_nv(rfm_dev, (rfcom_gsm_band_type)cfg.band, rfc_gsm_tx_sig_list_ptrs[rfm_dev][i]);
    }
    // rfc_common_tdd_update_grfc_script(rfc_gsm_tx_sig_list, (void *)rfc_gsm_tx_start_scripts[i], (void *)rfc_gsm_tx_stop_scripts[i]);
  }

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function initializes the rfdevices with the rfc dependent data.

  @details
  This function initializes the rfdevices with the rfc dependent data.

*/
void rfc_gsm::init_rf_devices(rfc_cfg_params_type *cfg, rfc_device_info_type *device_info_ptr)
{
  rfdevice_class *cmn_dev_obj = NULL;
  rfdevice_class *tuner_cmn_dev_obj = NULL;
  rfdevice_type_enum_type dev_type = RFDEVICE_TYPE_INVALID;
  rfdevice_class *rfgsm_device = NULL;
  uint32 instance_array[RFDEVICE_TYPE_MAX_NUM];
  uint32 instance = 0;
  uint32 local_instance = 0;
  uint8 tuner_dev_instance = 0;
  uint8 ant_num = 0;
  uint8 i = 0;
  rfcom_band_type_u rfcom_band;
  boolean is_dev_supported;

#ifdef FEATURE_RF_ASDIV
  rfdevice_class *asd_tuner_mgr = NULL;
#endif

  rfc_intf *rfc_cmn = rfc_intf::get_instance();
  if (NULL == rfc_cmn)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC CMN Object is NOT Created", 0);
    return;
  }

  if (NULL == cfg || NULL == device_info_ptr)
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"init_rf_devices(0x%x, 0x%x) : Bad parameters!", cfg, device_info_ptr);
    return;
  }

  for (i=0; i<RFDEVICE_TYPE_MAX_NUM; i++)
  {
    /* reset number of instances for a different device types */
    instance_array[i] = 0;
  }

  rfcom_band.gsm_band = (rfcom_gsm_band_type)cfg->band;

  ant_num = device_info_ptr->ant_num;

  i = 0;
  while (i < device_info_ptr->num_devices)
  {
    dev_type = device_info_ptr->rf_asic_info[i].device_type;
    instance = device_info_ptr->rf_asic_info[i].instance;
    cmn_dev_obj = rfc_cmn->get_cmn_rf_device_object(dev_type, instance);
    rfc_device_availability_enum_type device_status = rfc_cmn->get_cmn_rf_device_status(dev_type, (uint8)instance);

    /* Set the flag to TRUE by default */
    is_dev_supported = TRUE;
    if ((cmn_dev_obj == NULL) || (dev_type == RFDEVICE_TYPE_INVALID) || (dev_type <  RFDEVICE_TRANSCEIVER))
    {
      is_dev_supported = FALSE;
      i++;
      continue;
    }

    if (instance_array[dev_type] < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
    {
      local_instance = instance_array[dev_type];
      instance_array[dev_type] = instance_array[dev_type]+1;
    }
    else
    {

      MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_gsm::init_rf_devices(),Number of instances per band:%d > RFC_MAX_DEVICE_INSTANCES_PER_BAND:%d, Overwriting GSM RF devices list",instance_array[dev_type],RFC_MAX_DEVICE_INSTANCES_PER_BAND);
      local_instance = 0;
    }

    switch (dev_type)
    {
    case RFDEVICE_TRANSCEIVER:
      if (cmn_dev_obj != NULL)
      {
        // Store the path info: PRX or DRX
        trx_device_rf_path[cfg->logical_device][cfg->rx_tx][cfg->band][local_instance] = device_info_ptr->rf_asic_info[i].phy_path_num; //path 0 means prx +tx , and path 1 means Drx

        if (cfg->rx_tx == RFC_CONFIG_RX)
        {
          rfgsm_device = ((rfdevice_rxtx_common_class *)cmn_dev_obj)->get_tech_instance(RFM_EGSM_MODE,
                         RFDEVICE_RX,
                         (rfdevice_trx_phy_path_enum_type)device_info_ptr->rf_asic_info[i].phy_path_num);

          if (rfgsm_device != NULL)
          {
            gsm_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][local_instance] = rfgsm_device;

            ((rfdevice_trx_gsm_rx*)rfgsm_device)->rx_set_band_data((rfcom_gsm_band_type)cfg->band,
                device_info_ptr->rf_asic_info[i].data,
                RFC_ASIC_INFO_DATA_SIZE);
          }
          else
          {
            MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_gsm::init_rf_devices(), GSM Rx tech instance ptr is NULL");
          }
        }
        else if (cfg->rx_tx == RFC_CONFIG_TX)
        {

          rfgsm_device = ((rfdevice_rxtx_common_class *)cmn_dev_obj)->get_tech_instance(RFM_EGSM_MODE,
                         RFDEVICE_TX,
                         (rfdevice_trx_phy_path_enum_type)device_info_ptr->rf_asic_info[i].phy_path_num);

          if (rfgsm_device != NULL)
          {
            gsm_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][local_instance] = rfgsm_device;

            ((rfdevice_trx_gsm_tx*)rfgsm_device)->tx_set_band_data((rfcom_gsm_band_type)cfg->band,
                device_info_ptr->rf_asic_info[i].data,
                RFC_ASIC_INFO_DATA_SIZE);
          }
          else
          {
            MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_gsm::init_rf_devices(), GSM Tx tech instance ptr is NULL");
          }
        }
      }
      else
      {
        MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_gsm::init_rf_devices(), common device object ptr is NULL for logical dev: %d, tx/rx:%d, band:%d",cfg->logical_device, cfg->rx_tx, cfg->band);
        break;
      }

	  if (cfg->rx_tx == RFC_CONFIG_RX)
      {
        /*call the below api to capture the transciever->phy path->modem chain(ADC) mapping to perform adc dc cal.*/
        if(!rfc_cmn->set_trx_phy_path_adc_mapping(
                                                  device_info_ptr->modem_chain,                    /*modem chain to map the ADC*/
                                                  device_info_ptr->rf_asic_info[i].phy_path_num,   /*physical path*/
                                                  device_info_ptr->rf_asic_info[i].instance,       /*Transciever device type instance*/
                                                  RFM_EGSM_MODE,                                   /*tech*/
                                                  rfcom_band,                                       /*band*/
                                                  (rfdevice_class*)rfgsm_device,                   /*rx device pointer*/
                                                  device_info_ptr->rf_asic_info[i].data,           /*port info*/
                                                  RFC_ASIC_INFO_DATA_SIZE                          /*data size*/
                                                 )
          )
         {
           RF_MSG_3( RF_ERROR, "rfc_cmn->set_trx_phy_path_adc_mapping() failed. check rfc"
                                "modem_chain=%d,phy_path_num=%d, trx logical instance=%d  ",
                                device_info_ptr->modem_chain, 
                                device_info_ptr->rf_asic_info[i].phy_path_num,
                                device_info_ptr->rf_asic_info[i].instance
                               );
         }
      }

      break;

    case RFDEVICE_ASM:
      gsm_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][local_instance] = cmn_dev_obj;
      if (cmn_dev_obj != NULL)
      {
        rf_common_enter_critical_section( &(((rfdevice_asm *)cmn_dev_obj)->rfdevice_critical_sect));
        if (cfg->rx_tx == RFC_CONFIG_RX)
        {
          ((rfdevice_asm *)cmn_dev_obj)->set_rx_band_map(cfg->logical_device, RFM_EGSM_MODE, rfcom_band,
              device_info_ptr->rf_asic_info[i].data,
              RFC_ASIC_INFO_DATA_SIZE);
        }
        else if (cfg->rx_tx == RFC_CONFIG_TX)
        {
          ((rfdevice_asm *)cmn_dev_obj)->set_tx_band_map(cfg->logical_device, RFM_EGSM_MODE, rfcom_band,
              device_info_ptr->rf_asic_info[i].data,
              RFC_ASIC_INFO_DATA_SIZE);
        }
        rf_common_leave_critical_section( &(((rfdevice_asm *)cmn_dev_obj)->rfdevice_critical_sect));
      }
      break;

    case RFDEVICE_PA:
      gsm_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][local_instance] = cmn_dev_obj;
      if (cmn_dev_obj != NULL)
      {
        ((rfdevice_pa *)cmn_dev_obj)->set_band_map(RFM_EGSM_MODE, rfcom_band,
            device_info_ptr->rf_asic_info[i].data,
            RFC_ASIC_INFO_DATA_SIZE);
      }
      break;
    case RFDEVICE_PAPM:
      gsm_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][local_instance] = cmn_dev_obj;
      if (cmn_dev_obj != NULL)
      {
        ((rfdevice_papm *)cmn_dev_obj)->set_band_map(cfg->logical_device, RFM_EGSM_MODE, rfcom_band,
                                                     device_info_ptr->rf_asic_info[i].data,
                                                     RFC_ASIC_INFO_DATA_SIZE);
      }
      break;
    case RFDEVICE_TRANSMITTER:
    case RFDEVICE_RECEIVER:
    case RFDEVICE_THERM:
    case RFDEVICE_HDET:
      gsm_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][local_instance] = cmn_dev_obj;
      break;


    case RFDEVICE_TUNER:
    {
      /*** Get Tuner Device ***/
      gsm_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][local_instance] = cmn_dev_obj;

      if ((cmn_dev_obj != NULL) && (cfg->rx_tx == RFC_CONFIG_TX))
      {
        /*** Set distortion configuration ***/
        ((rfdevice_antenna_tuner *)cmn_dev_obj)->set_band_distortion_config_map(RFM_EGSM_MODE, rfcom_band,
            device_info_ptr->rf_asic_info[i].data);
      }
      /*** Get Tuner manager ***/
      /* Grab the associated Tuner manager instance, for now there is one to one mapping between
         a tuner device and tuner manager*/
      tuner_cmn_dev_obj = rfc_cmn->get_cmn_rf_device_object(RFDEVICE_TUNER_MANAGER, instance);

      /* Make sure there is a valid Antenna tuner manager */
      if (tuner_cmn_dev_obj != NULL)
      {
        rfdevice_class *tune_port_obj = NULL;

       /* For the case of DSDA, DSDS: 
           - A tuner physical device can be shared for primary paths on each of P-cell and S-cell.
           - A tuner physical device can be shared for diversity paths on each of P-cell and S-cell.
           - A tuner physical device will NEVER be shared for both primary and diversity paths of P-cell (or S-cell).

           Hence Port-0 of Tuner manager can be assigned for RFM path-0 and RFM path-1 (P-cell paths)
                and Port-1 of Tuner manager will be assigned for RFM path-2 and RFM path-3 (S-cell paths)

           Port-0 and Port-1 are synonymous from Tuner Manager point of view, hence a tech can make request 
           using any of the ports. */

        /*--- Get Port-0 object ---*/
        if ( (cfg->logical_device == RFM_DEVICE_0) || (cfg->logical_device == RFM_DEVICE_1) )
        {
        tune_port_obj =  rfcommon_atuner_manager_factory_get_port_instance ( tuner_cmn_dev_obj,
                         ATUNER_CA_PORT_0 );
        }

       /*--- Get Port-1 object ---*/
        if ( (cfg->logical_device == RFM_DEVICE_2) || (cfg->logical_device == RFM_DEVICE_3) )
        {
          tune_port_obj =  rfcommon_atuner_manager_factory_get_port_instance ( tuner_cmn_dev_obj,
                                                                               ATUNER_CA_PORT_1 );
        }
        gsm_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][RFDEVICE_TUNER_MANAGER][local_instance] = tune_port_obj;
        tuner_dev_instance++;
      }

    }
    break;

    case RFDEVICE_COUPLER:
      gsm_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][local_instance] = cmn_dev_obj;
      /* Coupler is used in both RX and TX so populate the buffer for both paths */
      gsm_rf_devices[cfg->logical_device][RFC_CONFIG_TX][cfg->band][dev_type][local_instance] = cmn_dev_obj;
      break;

#ifdef FEATURE_RF_ASDIV
    case RFDEVICE_XSW:
      gsm_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][local_instance] = cmn_dev_obj;

      if ( cfg->req == RFC_REQ_DISABLE_ANT_SWAP_1 )
      {
        /* Update the XSW port map by invoking the API below */
        ((rfdevice_xsw *)cmn_dev_obj)->set_band_map_default(
          RFCOM_GSM_MODE,
          rfcom_band,
          device_info_ptr->rf_asic_info[i].data,
          RFC_ASIC_INFO_DATA_SIZE );

      }
      if ( cfg->req == RFC_REQ_ENABLE_ANT_SWAP_1 )
      {
        /* Update the XSW port map by invoking the API below */
        ((rfdevice_xsw *)cmn_dev_obj)->set_band_map_swap(
          RFCOM_GSM_MODE,
          rfcom_band,
          device_info_ptr->rf_asic_info[i].data,
          RFC_ASIC_INFO_DATA_SIZE );
      }
      break;
#endif /* FEATURE_RF_ASDIV */

    default:
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Unkown RF device type %d", dev_type);
      is_dev_supported = FALSE;
      break;
    }

    if (is_dev_supported)
    {
      /* Check if the requested device was assigned succesfully else ASSERT here so that error is caught early */
      if(gsm_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][local_instance] == NULL)
      {
        /*If the device is present in RFC but has NULL physical ptr, then we return a fatal error during rfm_init */
        if (device_status == RFC_DEVICE_MISSING_FATAL)
        {
          RF_MSG_3(RF_FATAL, "Failed to map device! band = %d dev_type = %d instance = %d",
                   cfg->band, dev_type, local_instance);
          return;
        }
      }
    }
    i++;
  }

#ifdef FEATURE_RF_ASDIV
  ant_params[cfg->logical_device][cfg->rx_tx][cfg->band].antenna_num = ant_num ;

   /* update the rfc_asdiv_config info table with RFC attributes */
   asd_tuner_mgr = rfc_cmn->get_cmn_rf_device_object(RFDEVICE_ASD_TUNER_MANAGER, 0);
   if ( asd_tuner_mgr != NULL )
   {
      (void)((rfcommon_asdiv_tuner_manager*)asd_tuner_mgr)->update_rfc_asdiv_config_info(cfg->req, \
                                                                                        ant_num,   \
                                                                                        dev_type,  \
                                                                                        instance);
   }
   else
   {
      RF_MSG(RF_ERROR, "ASDIV: asd_tuner_mgr returns NULL ptr");
   }

  /* If there is a Tuner device in the rf-card, we need make sure that there is at-least
     one Tuner Instance for each Antenna Path. This is required by the Tuner design and also by the ASDiv design.
     Based on the state(Rx/Tx/Sleep) of the antenna path, other tuner devices need to be configured.
     So, We need to create a Dummy Tuner Device Object.
  */

  /* @todo - Back out for now until we understand this code better */
  /* As it stands, this code throws an exception */

  /* If the tuner device is NOT present */
  if (tuner_dev_instance == 0 )
  {
    rfdevice_class *tuner_port_obj = NULL;

    /* get the dummy Tuner object if needed. If there are no tuners in the rf-card, Dummy Tuner Mgr will not be created. */
    rfcommon_atuner_manager_intf *tuner_mgr_obj = (rfcommon_atuner_manager_intf *)(rfc_cmn->get_dummy_tuner_mgr(ant_num));

    if (tuner_mgr_obj != NULL)
    {
      if ( (cfg->logical_device == RFM_DEVICE_0) || (cfg->logical_device == RFM_DEVICE_1) )
      {
        tuner_port_obj =  tuner_mgr_obj->get_port_instance( ATUNER_CA_PORT_0 );
      }
      if ( (cfg->logical_device == RFM_DEVICE_2) || (cfg->logical_device == RFM_DEVICE_3) )
      {
        tuner_port_obj =  tuner_mgr_obj->get_port_instance( ATUNER_CA_PORT_1 );
      }
      gsm_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][RFDEVICE_TUNER_MANAGER][tuner_dev_instance] = tuner_port_obj;
    }
  }
#endif /*FEATURE_RF_ASDIV*/
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function initializes the rf devices for all supported band configurations with the rfc
  dependent device info data.

  @details
  This function initializes the rf devices for all supported band configurations with the rfc
  dependent device info data.

*/
void rfc_gsm::init_rfdevices_for_all_bands(rfm_device_enum_type rfm_dev)
{
  rfc_cfg_params_type cfg = { RF_PATH_0, rfm_dev, RFC_CONFIG_RX, RFCOM_NUM_GSM_BANDS, RFC_REQ_DEFAULT_GET_DATA};
  uint8 rfgsm_band;
  rfc_device_info_type *device_info_ptr = NULL;

  cfg.alternate_path = 0;
  // create GSM RFC AG Data Object
  rfc_gsm_data *rfc_data = rfc_gsm_data::get_instance();
  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC GSM Data Object is NOT Created", 0);
    return;
  }

  // set the rfdevice port information for each supported band
  for (rfgsm_band = (uint8)RFCOM_BAND_GSM850; rfgsm_band < (uint8)RFCOM_NUM_GSM_BANDS; rfgsm_band++)
  {
    cfg.band = (rfcom_gsm_band_type)rfgsm_band;
    cfg.req = RFC_REQ_DEFAULT_GET_DATA;
    cfg.rx_tx = RFC_CONFIG_RX;
    if(rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr )==TRUE)
    {
      init_rf_devices(&cfg, device_info_ptr);
      // Store supported band for this logical device and RX
      rfc_gsm_supported_band_map.supported_band_map[rfm_dev][cfg.rx_tx] = (rfcom_gsm_band_type)rfgsm_band ;
    }

    cfg.rx_tx = RFC_CONFIG_TX;
    if(rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr )==TRUE)
    {
      init_rf_devices(&cfg, device_info_ptr);
      // Store supported band for this logical device and RX
      rfc_gsm_supported_band_map.supported_band_map[rfm_dev][cfg.rx_tx] = (rfcom_gsm_band_type)rfgsm_band ;
    }

#ifdef FEATURE_RF_ASDIV
    /* Cross Switch Device (XSW) Initialisation */
    /* Only need to push antenna switch band configuration once */
    /* Check for dev2 needed for SGLTE */
    if ( ( RFM_DEVICE_0 == rfm_dev ) || ( RFM_DEVICE_2 == rfm_dev ) )
    {
      /* Enable Antenna Swap Port Mapping */
      cfg.req = RFC_REQ_ENABLE_ANT_SWAP_1;
      cfg.rx_tx = RFC_CONFIG_RX; /* Antenna switch happens during RX */
      if(rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr )==TRUE)
      {
        init_rf_devices(&cfg, device_info_ptr);
      }

      /* Disable Antenna Swap Port Mapping */
      cfg.req = RFC_REQ_DISABLE_ANT_SWAP_1;
      if(rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr )==TRUE)
      {
        init_rf_devices(&cfg, device_info_ptr);
      }
    }
#endif /* FEATURE_RF_ASDIV */
  }

  // set the rfc_gsm_core ptr which has the timing information
  rfdevice_gsm_set_rfc_gsm_core_ptr(rfm_dev,&rfc_gsm_core_config_table[rfm_dev]);



}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function initializes the rfdevices with the rfc dependent data.

  @details
  This function initializes the rfdevices with the rfc dependent data.

*/
rfdevice_class *rfc_gsm::get_rf_device_obj(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx,
    rfcom_gsm_band_type band, rfdevice_type_enum_type dev_type, uint32 instance)
{
  rfdevice_class *dev_obj = NULL;

  if (rfm_dev >= RFM_MAX_WAN_DEVICES || rx_tx >= RFC_CONFIG_RXTX_MAX
      || band >= RFCOM_NUM_GSM_BANDS || dev_type >= RFDEVICE_TYPE_MAX_NUM || instance >= RFC_MAX_DEVICE_INSTANCES_PER_BAND)
  {
    MSG_5(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_gsm::get_device_obj(%d, %d, %d, %d, %d): Incorrect Parameters",
          rfm_dev, rx_tx, band, dev_type, instance);
    return dev_obj;
  }

  dev_obj = gsm_rf_devices[rfm_dev][rx_tx][band][dev_type][instance];

  return dev_obj;
}

/*!
  @brief
  This function obtains a supported band for the logical device and RX or TX.

  @details
  This function obtains a supported band for the logical device and RX or TX.

*/
rfcom_gsm_band_type rfc_gsm::get_supported_band(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx)
{

  if (rfm_dev >= RFM_MAX_WAN_DEVICES || rx_tx >= RFC_CONFIG_RXTX_MAX )
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_gsm::get_supported_band(log dev: %d, rx_tx: %d): Incorrect Parameters",
          rfm_dev, rx_tx);
    return RFCOM_NUM_GSM_BANDS;
  }

  return (rfc_gsm_supported_band_map.supported_band_map[rfm_dev][rx_tx]);
}


boolean rfc_gsm::get_cfg_info(rfm_device_enum_type rfm_dev,  rfcom_gsm_band_type rfgsm_band, rfc_rxtx_enum_type rx_tx, rfc_cfg_info_type *cfg_struct, uint32 alt_path)
{
  rfc_cfg_params_type cfg = { RF_PATH_0, rfm_dev, rx_tx, rfgsm_band, RFC_REQ_DEFAULT_GET_DATA};
  rfc_device_info_type *device_info_ptr = NULL;

  cfg.alternate_path = 0;
  // create GSM RFC AG Data Object
  rfc_gsm_data *rfc_data = rfc_gsm_data::get_instance();
  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC GSM Data Object is NOT Created", 0);
    return FALSE;
  }

  cfg.alternate_path = alt_path;
  rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr );
  if (device_info_ptr==NULL)
  {
    return FALSE;
  }
  cfg_struct->nv_container = device_info_ptr->nv_container;
  cfg_struct->modem_chain = device_info_ptr->modem_chain;
  return TRUE;

}


boolean rfc_gsm::populate_spur_chan_list()
{
  uint32 i = 0;
  uint16 counter_array[RFCOM_NUM_GSM_BANDS]= {0,0,0,0};
  uint16 j = 0;
  rfc_gsm_data *rfc_data = rfc_gsm_data::get_instance();

  rfc_gsm_properties_type* rfc_gsm_prop_ptr = NULL;

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC GSM Data Object is NOT Created", 0);
    return FALSE;
  }

  // Reset the spur channel list
  for(j=0; j < MAX_NMBR_DESENSE_CHNL_PER_BAND; j++)
  {
    rfc_gsm::rfc_gsm_desense_chan_list.G850_chan_list[j] = LAST_ELEMNT_IN_CHNL_LIST;
    rfc_gsm::rfc_gsm_desense_chan_list.G900_chan_list[j] = LAST_ELEMNT_IN_CHNL_LIST;
    rfc_gsm::rfc_gsm_desense_chan_list.G1800_chan_list[j] = LAST_ELEMNT_IN_CHNL_LIST;
    rfc_gsm::rfc_gsm_desense_chan_list.G1900_chan_list[j] = LAST_ELEMNT_IN_CHNL_LIST;

  }

  //populate the spur channel list

  if((rfc_data->get_gsm_properties(&rfc_gsm_prop_ptr))== TRUE)
  {

    while ( (rfc_gsm_prop_ptr->gsm_spur_channels[i].band != RFCOM_NUM_GSM_BANDS) &&   (rfc_gsm_prop_ptr->gsm_spur_channels[i].chan != RFC_INVALID_PARAM)  && (i<(RFCOM_NUM_GSM_BANDS*MAX_NMBR_DESENSE_CHNL_PER_BAND)))
    {

      switch ( rfc_gsm_prop_ptr->gsm_spur_channels[i].band )
      {

      case  RFCOM_BAND_GSM850:
        j = counter_array[RFCOM_BAND_GSM850];
        if (  j < MAX_NMBR_DESENSE_CHNL_PER_BAND)
        {
          rfc_gsm::rfc_gsm_desense_chan_list.G850_chan_list[j] = rfc_gsm_prop_ptr->gsm_spur_channels[i].chan;

          counter_array[RFCOM_BAND_GSM850]++;
        }
        else
        {
          MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Number of spur channels exceeds the limit (%d) for band :%d)",MAX_NMBR_DESENSE_CHNL_PER_BAND,rfc_gsm_prop_ptr->gsm_spur_channels[i].band);
        }
        break;

      case RFCOM_BAND_GSM900:
        j = counter_array[RFCOM_BAND_GSM900];
        if (  j < MAX_NMBR_DESENSE_CHNL_PER_BAND)
        {
          rfc_gsm::rfc_gsm_desense_chan_list.G900_chan_list[j] = rfc_gsm_prop_ptr->gsm_spur_channels[i].chan;
          counter_array[RFCOM_BAND_GSM900]++;

        }
        else
        {
          MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Number of spur channels exceeds the limit (%d) for band :%d)",MAX_NMBR_DESENSE_CHNL_PER_BAND,rfc_gsm_prop_ptr->gsm_spur_channels[i].band);
        }
        break;

      case RFCOM_BAND_GSM1800:
        j = counter_array[RFCOM_BAND_GSM1800];
        if (  j < MAX_NMBR_DESENSE_CHNL_PER_BAND)
        {
          rfc_gsm::rfc_gsm_desense_chan_list.G1800_chan_list[j] = rfc_gsm_prop_ptr->gsm_spur_channels[i].chan;

          counter_array[RFCOM_BAND_GSM1800]++;
        }
        else
        {
          MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Number of spur channels exceeds the limit (%d) for band :%d)",MAX_NMBR_DESENSE_CHNL_PER_BAND,rfc_gsm_prop_ptr->gsm_spur_channels[i].band);
        }
        break;

      case RFCOM_BAND_GSM1900:
        j = counter_array[RFCOM_BAND_GSM1900];
        if (  j < MAX_NMBR_DESENSE_CHNL_PER_BAND )
        {
          rfc_gsm::rfc_gsm_desense_chan_list.G1900_chan_list[j] = rfc_gsm_prop_ptr->gsm_spur_channels[i].chan;
          counter_array[RFCOM_BAND_GSM1900]++;
        }
        else
        {
          MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Number of spur channels exceeds the limit (%d) for band :%d)",MAX_NMBR_DESENSE_CHNL_PER_BAND,rfc_gsm_prop_ptr->gsm_spur_channels[i].band);
        }
        break;

      default:            /* Invalid mode operation */
        MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Unknown band to add in the spur channel list :%d)",rfc_gsm_prop_ptr->gsm_spur_channels[i].band);
        break;

      }

      i++;
    }


  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"GSM properties NULL", 0);
    return FALSE;
  }


  return TRUE;

}

/*!
  @brief
  This function returns the RX path for the corresponding logical device, band and local device instance.

  @details
  This function returns the RX path for the corresponding logical device, band and local device instance.

*/


/* Return the RX path for the corresponding logical device, band and local device instance*/
uint32 rfc_gsm::get_rf_path(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx, rfcom_gsm_band_type band,  uint32 instance)
{

  if (rfm_dev >= RFM_MAX_WAN_DEVICES || band >= RFCOM_NUM_GSM_BANDS || rx_tx >= RFC_CONFIG_RXTX_MAX || instance >= RFC_MAX_DEVICE_INSTANCES_PER_BAND)
  {
    MSG_5(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_gsm::get_rf_path(%d, %d, %d, %d, %d): Incorrect Parameters",
          rfm_dev, rx_tx, band, instance,0);
    return 0xFFFF;
  }


  return trx_device_rf_path[rfm_dev][rx_tx][band][instance];;
}


/*!
  @brief
  This function updates the RX burst parameters that affect RX alpha timing or first SSBI timing.

  @details
  The parameters include:
  Total number of SSBIs before PLL writes
  Total number of SSBIs after PLL writes
  Total RX burst processing time from the PLL writes

*/
boolean rfc_gsm::update_rx_timing_params(uint32 rx_burst_time,
    uint16 num_sbi_before_pll,
    uint16 num_sbi_from_pll,
                                         uint16 rx_pll_settling_time_qs,
                                         rfm_device_enum_type rfm_dev)
{

  /*Regular RX burst*/
  rfc_gsm::rfc_gsm_rx_timing_info[rfm_dev].rx_burst_processing_time_from_pll_qs = rx_burst_time;
  rfc_gsm::rfc_gsm_rx_timing_info[rfm_dev].rx_burst_sbis_before_pll = num_sbi_before_pll;
  rfc_gsm::rfc_gsm_rx_timing_info[rfm_dev].rx_burst_sbis_from_pll = num_sbi_from_pll;
  rfc_gsm::rfc_gsm_rx_timing_info[rfm_dev].pll_settling_time_qs = rx_pll_settling_time_qs;

  /*Monitor RX burst*/
  rfc_gsm::rfc_gsm_rx_mon_timing_info[rfm_dev].rx_burst_processing_time_from_pll_qs = rx_burst_time;
  rfc_gsm::rfc_gsm_rx_mon_timing_info[rfm_dev].rx_burst_sbis_before_pll = num_sbi_before_pll;
  rfc_gsm::rfc_gsm_rx_mon_timing_info[rfm_dev].rx_burst_sbis_from_pll = num_sbi_from_pll;
  rfc_gsm::rfc_gsm_rx_mon_timing_info[rfm_dev].pll_settling_time_qs = rx_pll_settling_time_qs;

  return TRUE;

}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the base ptr of list of device objects for the specified logical_path,
  rx or tx, band and device type. The list of object will be of same device type.

  @details
  This function returns the base ptr of list of device objects for the specified logical_path,
  rx or tx, band and device type. The list of object will be of same device type.

*/
rfdevice_class** rfc_gsm::get_rf_device_list(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx,
    rfcom_gsm_band_type band, rfdevice_type_enum_type dev_type)
{
  rfdevice_class **dev_obj = NULL;
  uint8 antenna = 0;
  rfdevice_class *asd_tuner_mgr = NULL;

  if (rfm_dev >= RFM_MAX_WAN_DEVICES || rx_tx >= RFC_CONFIG_RXTX_MAX
      || band >= RFCOM_NUM_GSM_BANDS || dev_type >= RFDEVICE_TYPE_MAX_NUM)
  {
    MSG_4(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_gsm::get_rf_device_list(%d, %d, %d, %d): Incorrect Parameters",
          rfm_dev, rx_tx, band, dev_type);
    return dev_obj;
  }

  dev_obj = gsm_rf_devices[rfm_dev][rx_tx][band][dev_type];

#ifdef FEATURE_RF_ASDIV
  /** If Asdiv Tuner Manager is present get the correct list of device objects
      based on the ASDiv Switch position for specific Antenna */

  rfc_intf *rfc_cmn = rfc_intf::get_instance();

  asd_tuner_mgr = rfc_cmn->get_cmn_rf_device_object(RFDEVICE_ASD_TUNER_MANAGER, 0);

  if ((dev_type == RFDEVICE_TUNER_MANAGER) && (asd_tuner_mgr!= NULL))
  {
    antenna = ant_params[rfm_dev][rx_tx][band].antenna_num ;

    dev_obj = (rfdevice_class **)(((rfcommon_asdiv_tuner_manager*)asd_tuner_mgr)->get_asdiv_device_list(antenna, dev_type, (rfdevice_class **)dev_obj));
  }
#endif /*FEATURE_RF_ASDIV*/

  return dev_obj;
}



/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function configures the TX/RX front end devices as required for IP2 cal.

*/

boolean rfc_gsm::ip2_cal_device_config(rfm_device_enum_type rfm_dev,
                                       rfcom_gsm_band_type band,
                                       uint16 arfcn,
                                       void *setup_script_ptr,
                                       void *cleanup_script_ptr)
{

  uint8 i = 0;
  rfc_cfg_params_type cfg = { RF_PATH_0, rfm_dev, RFC_CONFIG_RX, band, RFC_REQ_DEFAULT_GET_DATA};
  rfc_sig_cfg_type *sig_ptr = NULL;
  rfdevice_class **dev_list = NULL;
  rfdevice_type_enum_type dev_type;
  rfdevice_asm_cfg_type asm_cfg_s;

  cfg.alternate_path = 0;
  rfc_gsm_data *rfc_data = rfc_gsm_data::get_instance();
  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC GSM Data Object is NOT Created", 0);
    return FALSE;
  }

  if (band >= RFCOM_NUM_GSM_BANDS)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"invalid band = %d", band);
    return FALSE;
  }

  /* configure the GPIOs or GRFCs */
  rfc_data->sig_cfg_data_get(&cfg, &sig_ptr );
  rfc_common_tdd_update_grfc_script( sig_ptr,
                                     (void *)setup_script_ptr,
                                     (void *)cleanup_script_ptr);

  /*loop through all device types*/
  for (i = 0; i < RFDEVICE_TYPE_MAX_NUM; i++)
  {
    dev_list = get_rf_device_list(rfm_dev, cfg.rx_tx, band, (rfdevice_type_enum_type)i);
    if (dev_list != NULL)
    {
      uint8 index = 0;
      while (dev_list[index] != NULL && index < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
      {

        dev_type = dev_list[index]->rfdevice_type_get();

        switch (dev_type)
        {
        case RFDEVICE_ASM:

          asm_cfg_s.rfm_device = rfm_dev;
          asm_cfg_s.mode = RFM_EGSM_MODE;
          asm_cfg_s.band.gsm_band = band;
          asm_cfg_s.asm_rx_tx = RFDEVICE_ASM_RX_0;
          asm_cfg_s.asm_cell = RFDEVICE_ASM_CELL_0;
          asm_cfg_s.trigger_type = RFDEVICE_ASM_TRIGGER_ENABLE_RX;
          
          rfdevice_asm_enable(&asm_cfg_s,
                  dev_list[index],
                  (rf_buffer_intf *)setup_script_ptr,
                  RFDEVICE_CREATE_SCRIPT,
                  RFCOM_INVALID_TIMING_OFFSET);

          asm_cfg_s.trigger_type = RFDEVICE_ASM_TRIGGER_DISABLE_RX;
          rfdevice_asm_disable(&asm_cfg_s,
                               dev_list[index],
                               (rf_buffer_intf *)cleanup_script_ptr,
                               RFDEVICE_CREATE_SCRIPT,
                               RFCOM_INVALID_TIMING_OFFSET);

          break;

        default:
          MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf-front-end device type:%d NOT Configured!", dev_type);
          break;
        }
        index++;
      }
    }
  }
  return TRUE;

}

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  API to update the rf port enumeration for various devices 
 
  @details
  Function is used as part of path swapping procedure. It is called when a
  suitable alternate path is selected for a particular CA or DSDS/DSDA scenario.
  The alternate path info along with the current band and logical device is used
  to determine which specific device info structure for that band should be
  used. Based on the info, rf port enumeration for various rf devices are
  updated. 
 
  @param rfm_dev
  current logical device which needs port reconfiguration
 
  @param curr_band
  current rf band involved in path swapping or port selection 
 
  @param alt_path
  preferred alternate path to use for a specific logical device & band combo
 
  @return status
  TRUE if a successful match is found in RFC AG and rf port info update is
  complete, else return FALSE 
*/
boolean rfc_gsm::get_updated_rf_port( rfm_device_enum_type rfm_dev,
                                      rf_card_band_type curr_band,
                                      uint8 alt_path )
{
  rfcom_gsm_band_type  rfgsm_band = RFCOM_NUM_GSM_BANDS; 
  rfc_cfg_params_type  cfg;
  rfc_device_info_type *device_info_ptr;
  rfc_gsm_data *rfc_data = rfc_gsm_data::get_instance();

  if (rfc_data == NULL)
  {
    RF_MSG_1(RF_ERROR,"RFC GSM Data Object is NOT Created", 0);
    return FALSE;
  }
 
  rfgsm_band = rfgsm_core_rf_card_band_to_rfcom_gsm_band(curr_band);

  /* Initialize the config params structure */
  cfg.logical_device = rfm_dev;
  cfg.band           = (rfcom_gsm_band_type)rfgsm_band; 
  cfg.rx_tx          = RFC_CONFIG_RX;
  cfg.req            = RFC_REQ_DEFAULT_GET_DATA;
  cfg.port_state     = RFC_PORT_STATE_A;

  /* For GSM, alternate path assigned to a logical device is same for all four bands */
  cfg.alternate_path = (uint32)alt_path; 

    /* Get the column specific to a band, logical device & alt path */
    if(rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr )==TRUE)
    {
      init_rf_devices(&cfg, device_info_ptr);
    }

  return TRUE; 
}




boolean rfc_gsm::initialize_rfc_timing_info(rfm_device_enum_type rfm_dev)
{

  if (rfm_dev >= RFM_MAX_WAN_DEVICES)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Invalid rfm_dev %d", rfm_dev);
    return FALSE;
  }

  rfc_gsm_rx_timing_info[rfm_dev].rx_alpha_qs                          = RFC_GSM_RX_ALPHA;
  rfc_gsm_rx_timing_info[rfm_dev].idle_mode_rx_alpha_qs                = RFC_GSM_IDLE_MODE_RX_ALPHA;
  rfc_gsm_rx_timing_info[rfm_dev].ant_start_delta_qs                   = RFC_GSM_RX_ANT_START_DELTA_QS;
  rfc_gsm_rx_timing_info[rfm_dev].ant_stop_delta_qs                    = RFC_GSM_RX_ANT_STOP_DELTA_QS;
  rfc_gsm_rx_timing_info[rfm_dev].dc_cal_time_qs                       = RFC_GSM_RX_DC_CAL_TIME_QS;
  rfc_gsm_rx_timing_info[rfm_dev].modem_delay_time_qs                  = RFC_GSM_RXF_MODEM_DELAY_QS;
  rfc_gsm_rx_timing_info[rfm_dev].pll_settling_time_qs                 = RFC_GSM_RX_PLL_SETTLING_TIME_QS;
  rfc_gsm_rx_timing_info[rfm_dev].rx_mdsp_overhead                     = RFC_GSM_RX_MDSP_OVERHEAD;
  rfc_gsm_rx_timing_info[rfm_dev].mux_dly_freez_time_qs                = RFC_GSM_RX_MUX_DLY_FREEZ_TIME_QS;
  rfc_gsm_rx_timing_info[rfm_dev].rx_burst_processing_time_from_pll_qs = 0;
  rfc_gsm_rx_timing_info[rfm_dev].rx_burst_sbis_from_pll               = 0;
  rfc_gsm_rx_timing_info[rfm_dev].rx_burst_sbis_before_pll             = 0;

  rfc_gsm_rx_mon_timing_info[rfm_dev].rx_alpha_qs                          = RFC_GSM_RX_ALPHA;
  rfc_gsm_rx_mon_timing_info[rfm_dev].idle_mode_rx_alpha_qs                = RFC_GSM_IDLE_MODE_RX_ALPHA;
  rfc_gsm_rx_mon_timing_info[rfm_dev].ant_start_delta_qs                   = RFC_GSM_RX_ANT_START_DELTA_QS;
  rfc_gsm_rx_mon_timing_info[rfm_dev].ant_stop_delta_qs                    = RFC_GSM_RX_ANT_STOP_DELTA_QS;
  rfc_gsm_rx_mon_timing_info[rfm_dev].dc_cal_time_qs                       = RFC_GSM_RX_DC_CAL_TIME_QS;
  rfc_gsm_rx_mon_timing_info[rfm_dev].modem_delay_time_qs                  = RFC_GSM_RXF_MODEM_DELAY_QS;
  rfc_gsm_rx_mon_timing_info[rfm_dev].pll_settling_time_qs                 = RFC_GSM_RX_PLL_SETTLING_TIME_QS;
  rfc_gsm_rx_mon_timing_info[rfm_dev].rx_mdsp_overhead                     = RFC_GSM_RX_MDSP_OVERHEAD;
  rfc_gsm_rx_mon_timing_info[rfm_dev].mux_dly_freez_time_qs                = RFC_GSM_RX_MUX_DLY_FREEZ_TIME_QS;
  rfc_gsm_rx_mon_timing_info[rfm_dev].rx_burst_processing_time_from_pll_qs = 0;
  rfc_gsm_rx_mon_timing_info[rfm_dev].rx_burst_sbis_from_pll               = 0;
  rfc_gsm_rx_mon_timing_info[rfm_dev].rx_burst_sbis_before_pll             = 0;

  rfc_gsm_tx_timing_info[rfm_dev].num_pll_sbis          = RFC_GSM_TX_PLL_SBIS;
  rfc_gsm_tx_timing_info[rfm_dev].kv_length             = RFC_GSM_TX_KV_LENGTH;
  rfc_gsm_tx_timing_info[rfm_dev].tx_sbi_start_delta_qs = RFC_GSM_TX_SBI_START_DELTA_QS;
  rfc_gsm_tx_timing_info[rfm_dev].tx_alpha_qs           = RFC_GSM_TX_ALPHA_QS;

  rfc_gsm_core_config_table[rfm_dev].rx_timing                  = &rfc_gsm_rx_timing_info[rfm_dev];
  rfc_gsm_core_config_table[rfm_dev].rx_mon_timing              = &rfc_gsm_rx_mon_timing_info[rfm_dev];
  rfc_gsm_core_config_table[rfm_dev].tx_timing                  = &rfc_gsm_tx_timing_info[rfm_dev];
  rfc_gsm_core_config_table[rfm_dev].desense_chan_list          = &rfc_gsm_desense_chan_list;
  rfc_gsm_core_config_table[rfm_dev].pwr_off_in_sleep           = TRUE;
  rfc_gsm_core_config_table[rfm_dev].rf_warmup_time             = 20000;
  rfc_gsm_core_config_table[rfm_dev].rf_sigma_delta_max_db      = RFC_GSM_SIGMA_DELTA_MAX_DB;
  rfc_gsm_core_config_table[rfm_dev].rfc_gsm_tx_ref_clock_mode  = 0;
  rfc_gsm_core_config_table[rfm_dev].ftm_temp_comp_enable_flag  = TRUE;
  rfc_gsm_core_config_table[rfm_dev].qpa_status_flag            = FALSE;  
  rfc_gsm_core_config_table[rfm_dev].rfc_gsm_pa_gain_range_map  = &rfc_gsm_pa_gain_range_map;
  rfc_gsm_core_config_table[rfm_dev].rfc_gsm_apt_config         = RFC_APT_RFFE;
  rfc_gsm_core_config_table[rfm_dev].rfc_gsm_supported_band_map = &rfc_gsm_supported_band_map;
  
  return TRUE;
}

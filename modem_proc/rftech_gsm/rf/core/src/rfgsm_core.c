/*!
   @file
   rfgsm_core.c

   @brief

   @details

*/

/*===========================================================================
Copyright (c) 2008 - 2016 by Qualcomm Technologies Incorporated.
All Rights Reserved.

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/rf/core/src/rfgsm_core.c#1 $
  $DateTime: 2016/12/13 07:59:51 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
06/08/16   bp      Put the QPOET to standby mode after GSM Tx burst ends
04/13/16   ase     Crash if LPM->ON Rx wakeup or ON->LPM Rx sleep fail
02/23/16   par     Add MC logging buffer for failing sequences
02/12/16   ec      Reset JDET counter table only at first initialisation
01/27/16   mz      FR 30990: SAWless LTE TDD B39: enable IIP2 CAL on GSM SAW device
12/23/15   bp      Stop further processing of therm_read_rsp if temp. compensation is stopped through GSM Tx Disable.
12/15/15   bp      DRDS specific changes to ensure right values are returned to L1
12/10/15   dm      Fixing KW Errors
11/25/15   bp      FR25072: for device wakeup, vote for lowest/first supported band
09/11/15   par     Fix crash when sending ASDiv command in FTM mode
08/19/15   par     Deprecate slot mode control support
07/27/15   par     Pass lowest supported band in cases where GSM1800 band is not supported
04/23/15   par     Fix incorrect AFC being applied in GBTA cases
03/12/15   sml     Correct OFT error
03/12/15   sml     Use the rfc_gsm_get_rf_device_list for FTM_TUNER_TUNE_CODE_OVERRIDE_CMD
02/26/15   tws     Fix compiler warning
02/24/15   piy     Initialise device_status_rsp_umid to default value in rfgsm_core_data_init
02/23/15   sml     Passing band info while generating GSM Tx/Rx
                   SBI scripts to support band split across 2 WTRs
01/28/15   mz      Fix wrong jdet_counter reset when rx band changes
01/27/15   ec      Remove old debug NV access in burst metrics processing
01/27/15   piy     Logging Rx burst RxAGC info and disabling therm reads via debug NV
01/22/15   piy     Fixed bug in Device status read feature for Mon Burst
01/16/14   ec      Use new debug NV mechanism to set flags at intialisation time
1/14/15    sml     Correcting OFT compliation
1/13/15    sml     GL1 -> RF handshake mechanism to handles cases when
                   GSM ASD response is not being received as scripts
                   weren't executed
01/12/15   sml     Changes to remove un-nucessary tuner transactions
                   during probe burst
12/18/14   ec      Avoid processing metrics if JDET reading is invalid due to Therm read scheduling
12/15/14   sml     Enable DAC VREF only during Tx enable
12/12/14   tws     Include RFLM MSM INTF header
12/02/14   zbz     Make rfc gsm timing ptr per device
12/04/14   tws     Change jdet F3
12/03/14   sb      Enable Jdet
12/02/14   sml     Read WTR or RF front end devices status in between Rx/Tx
                   burst along with GRFC status
11/27/14   ec      Add code to set JDET read holdoff timer based on burst type
11/25/14   sb      HL/LL algorithm changes and bug fixes
11/18/14   sw/sc   Deprecate legacy state machines and move cmd buff out of rfa
11/12/14   sw      Rename RX/TX script types to reflect actual devices
11/04/14   tsr     Fix Alternate path bug in IRAT due to GSM 1800_B band support not available in RFC
10/27/14   sc      Fix sub mask being manipulated outside of enter-exit mutex
10/23/14   sc      Reduce code execution limiting use of get qsym API
10/23/14   sc      Enable event type control from RFSW
10/17/14   rr      Separate rx/tx alternate path core handles
10/06/14   sc      Deprecate unused functionality
10/02/14  ec/cpm   Move FTM LL override setup to HL table allocation in Enter Mode
10/01/14   tsr     GSM IDLE Mode RXD support
10/01/14   sb      Enhance IP2 F3 messages for improved logging
09/19/14   sb      Add IP2 cal moving averaging + pass/fail status
09/24/14   hoh     Add multislot APT control feature
09/23/14   zbz     changes to support GSM and LTE/TDS coexistence related RFC changes
09/23/14   sw      Boolean to indicate ASD scripts to be built only once (and deprecate skip_tuner_program)
09/19/14   sc      Force RxLM update for every wakeup
09/15/14   piy     Remove Excess F3s from rfgsm_core_update_alternate_rx_path_port_mapping
09/12/14   ec      Convert JDET reading from raw value into 2's complement
09/11/14   sml     Adding start API Pcycle to know about pre-emption done to RF task
09/09/14   sb      Fix for sawless support for PCS band
08/22/14   ec      Add code to stop out-of-bounds access of HL flag table
08/20/14   ndb     Add Tuner_ID as part of the response in "FTM_SET_TUNER_TUNE_CODE_OVERRIDE"
09/15/14   rr      Fix for nv container loading
09/06/14   sb      Fix timing of IP2 cal setup script
11/19/13   cpm     support for FTM_PROGRAM_QFE_SCENARIO and Antenna Tuner force_detune()
09/09/14   tsr     DR-DSDS C2/C3 RX calibration support
09/09/14   ch      Mainline FEATURE_RF_HAS_QTUNER
09/01/14   sc      Only update enter mode Tx timing information if Tx capable
08/29/14   tsr     Make number of tasks to zero in minimized enter mode
08/29/14   tws     Mainline alternate_path support
08/28/14   sc      Clean up Tx data initialisation to reduce wakeup timeline
08/26/14   zbz     Reduce number of tuner writes during ASDiv
08/22/14   rr      Changes to support nv c3
08/19/14   zbz     Add RF GSM CCS execution time profiling
08/14/14   zbz     Add SMPS/ICQ/temp_rem to FTM log packet
08/14/14   sc      Coex fix for non-tx capable device accessing Tx NV
08/14/14   sc      Ensure init of Tx variables on Tx capable device on wakeup
08/14/14   ec      Add allocation of HL table to core_init and move deallocation to destroy_handle
08/11/14   cj      Added tx sharing changes to enter mode and wakeup
08/11/14   sc      Ensure chain update with alternate path and include IRAT
08/08/14   par     Update WAN_TX_RESET for dev2 as well
08/07/14   sc      Replace wakeup data init after minimised wakeup
08/04/14   cj      Added changes to Tx enable/disable
08/04/14   tws     Stub functionlaity for RUMI pre-silicon
08/03/14   tsr     Pass RF COM band type to concurrency manager instead of rfc band type
08/03/14   tsr     Fix KW error for alternate path support
08/03/14   ec      Add code to handle HL NV items and new overrides for SAWless targets
08/03/14   jr      Remove temp comp start from wake-up
08/01/14   tws     Enable state machine debug on all targets. Fix logic bug.
07/31/14   tsr     GSM Alternate path support
07/30/14   cj      Added changes for Tx sharing feature
07/30/14   hoh     Add enhanced rx feature support
07/28/14   tsr     DR-DSDS: Add support for device driver to handle worst case rx alpha for IDLE mode RX
07/28/14   ec      Add Jammer Detector APIs and correct threshold values
07/25/14   sb      bug fixes for V1 IP2 cal functionality
07/21/14   sc      Add vreg vote count per sub per path
07/18/14   dr      [RUMI] Changes for JOLOKIA RUMI bringup
07/15/14   av      checked in proper fix for the sawless initialization
07/15/14   av      compiler warning fix
07/15/14   av      featurize sawless initialization for now.
07/14/14   ec      Fix compiler warnings due to variables with same name
07/14/14   ec      Add APIs to control HL/LL decision table, allocate and free at enter and exit
                   mode
07/08/14   tsr     DR-DSDS: Add worst case rx alpha for IDLE mode RX
07/08/14   jr      Changes for TSTS support
07/01/14   sc      Ensure variables updated if first enter for sub is minimised
07/01/14   jr      Enable therm read on sub-2
06/27/14   sc      RxLM and dev ID per sub and minimised call-flow for RxLM update logic
06/26/14   sw      Improved logging
06/18/14   tsr     RXD diversity call flow changes to enable DRX path along with PRX path
06/18/14   sw      Remove calls to internal geran function gl1_get_FN() from RF
                   (port CR609471 from Dime)
06/14/14   sb      Add hooks for sawless support
06/11/14   sc/ggs  IDLE WTR hopping wakeup requires rfgsm_msm_init when
                   rxlm_buff_idx from core handle is different to the one
                   passed from GL1
06/10/14   hk      Initializing edge_core to 0xFF
06/06/14   sb      Reduce GSM heap memory usage
05/30/14   ldg     Add slot mode control function
05/29/14   sb      IP2 cal bug fixes, updates
05/29/14   sc      Move exit mode RFLM DM mem de-alloc logic to RFLM DM
05/14/14   sw      Small correction to comment
05/19/14   tsr     Update AutoB writes based on uses_pll_flag since PLL writes are only for PRX devices
05/19/14   sc      Prevent duplicate tech hal voting and un-voting other sub
05/19/14   sc      Add set device call to wakeup for IDLE WTR hopping
05/08/14   sc      Add RFLM GSM init once API interface
05/06/14   tsr     Reuse ccs event handles for RX/TX start stop & trans
05/06/14   ggs     AMAM/AMPM tables should only be recalculated
                   for GSM when modulation type is 8PSK
05/02/14   sc      Clean enter-mode/wakeup event and provide intf index
04/30/14   sc      Manage Vregs based on subscription
04/30/14   hoh     Print error message if not enough space in tx trans script buffer
04/27/14   tsr     Fixed Bug to update rx band and rx nvs into core handle ptr during set band
04/24/14   ggs     Port CR581892: Initialise vabtt and desense back-offs to
                   zero
04/23/04   tsr     Add API to query PRX device associated with DRX device
04/16/14   sb      Fix KW
04/16/14   sb      IP2 cal Interface and functionality changes
04/14/14   tsr     Free DM memory only for PRX device
04/10/14   lcl     Fix Klocwork errors/warnings
04/07/14   sc      Vote for hal bus using only single GSM tech
04/07/14   sc      Clean event API
04/07/14   sc      Alloc/Free RFLM interface data in core-init/de-init
03/31/14   tsr     Added support for handling combined Enter Mode for PRX and DRX
03/26/14   tsr     Move the rfgsm_core_set_device_capability inside rfgsm_core_handle init
03/26/14   tsr     Add support to query rfm device for TX or RX capability
03/26/14   tsr     Separate out TX and RX enter mode for WTR to handle RXD
03/26/14   sb      Add support for IP2 cal for Dime PM
03/25/14   sc      Store device in RFLM GSM DM data table for TQ management
03/24/14   sc      Remove dynamic task priority manipulation
03/19/14   hoh     Added support for 8 PA gain states
03/18/14   sc      Add subscription based execution intelligence
03/13/14   sw      Use core_data for ASD
03/11/14   tsr     Program device scripts dynamically through CCS
03/11/14   sc      Expose RFLM overhead to RFSW to ensure GL1 account for delay
03/04/14   sw      ASD confirmation message now comes from RFLM (and not GFW)
03/04/14   tsr     Separate RX burst scripts and TX burst scripts retrieval from device driver
03/04/14   tsr     Move TX specific initialisation into Tx enter mode
02/28/14   sc      Pass core data through mc to core layers
02/25/14   tsr     Update RX Alpha based on wtr writes and pll settling time from device driver
02/24/14   dps     Added call to rx_init to get timing info for Rx burst scripts
02/19/14   tsr     GSM RX Diversity
02/17/14   sc      Clean up mDSP APIs
02/10/14   cj      rfgsm_buffer flag added to decide whether to use home/away buffer
02/05/14   sc      Use RFLM DM memory space for RF GSM data
01/30/14   sc      Manage data table independent of device
01/22/14   sw      Thermistor read changes
01/21/14   tsr     WTR TXAGC support for Multislot TX
01/21/14   ggs     Initial changes for TXC DCOC and DPD calibration
01/15/14   sc      Remove unused APIs using FW shared memory
01/08/14   cj      Added DM buffer allocation for burst script buffers in IRAT mode
12/26/13   hm      Script buffer creation with tech scaling factor, to fix expected vs observed delays in scripts
12/20/13   dps     Remove reference to rfdevice_gsm_init_once
12/13/13   ry      Fix Merge error
12/13/13   ry      Remove hardcoding of RFM_DEVICE_0
12/12/13   sc      Add modem-specific max AMAM/AMPM table entries
11/25/13   yb      Removing tune_code_override from rfdevice_antenna_tuner_intf layer.
                   Replacing with rfcommon_atuner_tune_code_override API.
11/19/13   cj      IRAT changes added for RX burst
11/08/13   hm      GSM interface changes for IRAT cases to determine WXE memory segment
10/31/13   sc      Feature-guard FW shared mem buffer population
10/29/13   tsr     Updated TX CCS event to support new v2 script buffers for WTR1625
10/29/13   tsr     TX Transition buffer script v2 for Bolt
10/28/13   ggs     Add Ping-pong AMAM AMPM ID to core handle
10/23/13   tsr     Mainline D3295 changes for BOLT
10/23/13   sg      GSM ASDiv with tuner changes
10/21/13   sc      Modify dynamic event building for scenario-specific wrappers
10/15/13   tsr     Correct the enum for RX Burst buffers
10/15/13   sc      Mainline L1-RF interface changes
10/14/13   tsr     Clear WTR Burst script before retrieving from WTR
10/14/13   sc      Mainline core DM and RFLM code
10/14/13   sml     Limiting the number of LNA ranges to five
                   depending upon a flag which needs to be
                   updated by WTR
10/11/13   ec	   Modify state machine to cope with GBTA
10/03/13   tsr     Fix the Compiler error from previous checkin
10/03/13   tsr     Added support to retrieve individual device scripts
10/04/13   sc      RFLM DM PA ramp table init after DM mem init during enter mode
10/03/13   sb      Add changes to receive MSGR cmd from MCS to switch to filtered/unfiltered
                   path for SGLTE coexistence (GSM DCS +LTE B39/TDS B39/B34)
10/03/13   sc      RFLM DM interface data and dynamic event development
10/02/13   tws     QPOET GSM Optimisations.
10/01/13   sb      Disable SGLTE Coexistence filtered path cal data
09/30/13   av      Remove excessive messaging when RFM apis are called late
09/27/13   sc      Modify RFLM GSM interface data programming functionality
09/20/13   par     Update RF task priority setting
09/17/13   sb      Fix check for right rx burst type
09/17/13   sb      Support for SGLTE/SGTDS coexistence
09/12/13   sc      RFLM interface encompassed by core DM
09/11/13   sc      Move calls for core DM mem alloc/free to enter/exit mode
08/28/13   ec      Fix Offtarget error
08/28/13   ec      Creating RF GSM Core state machine with functions to init and
                   drive
08/27/13   sb      Add changes to update RX timing for GSM RXD
08/23/13   sml     Acknowledging ASD switch
08/19/13   sc      Rename default num PA ramp entries
08/16/13   ndb     Clean-up the SPI based antenna tuner support
08/13/13   sb      GSM RXD changes
08/13/13   sc      Rename definitions ready for RFLM GSM merging
08/09/13   cpm     atuner init() scripted into SBI events
08/08/13   sc      Add PA ramp initialisation, storing NV tables in core DM
08/07/13   sc      Add call to default HMSC device flag to FALSE on init
08/06/13   sc      Initialise PA ramp management core handle data
08/06/13   sml     Fixing KW warning
08/02/13   sml     GSM RxD NV support
08/02/13   sml     Fixing Complier warnings
07/31/13   av      GBTA support for both GL1 sub1 and GL1 sub2 call RF to build RX scripts
07/30/13   pl      Enable new therm read interface
07/29/13   sml     Generic RFSW-FW interaction to generate an ACK for a CCS event
07/29/13   sml/svi ASD support for GSM tech
07/24/13   sc      Initialise and de-init the RFLM data manager info (featurised)
07/29/13   tws     Use RF_TASK_1 for GSM.
07/26/13   tws     New API functions to retrieve rx/tx band
07/25/13   ec      Return FALSE from rfgsm_core_init if NULL pointer found.
07/24/13   sc      Update RFLM data information (featurised)
07/23/13   tws     Fix merge issue. Code was accidentally removed.
07/22/13   ec	   Use new Null pointer checking and F3 macros
07/17/13   sc      Merge Vreg On/Off to Enter/Exit Wakeup/Sleep
07/16/13   sc      Move mc/core functionality out of dispatcher and into mc/core layer
07/18/13   svi     Adding [QTUNER][RL] GSM NV Support.
07/10/13   svi     Compiler Warning Fix.
07/08/13   svi     Closed Loop Mode Fix.
07/08/13   svi     Adding Support of Closed Loop Mode for Antenna Tuner.
07/04/13   tws     Use new api to get first supported GSM band
06/28/13   ec	   Remove Unknown task F3
06/20/13   sb      Add hooks to populate Spur channel list from RFC
06/20/13   ec      Add support for GSM Tune Away (GTA) enter mode-
06/19/13   ec      Mainline rfgsm_core_get_task_num()
06/19/13   svi     Fix vbatt comp issues
05/30/13   tws     Fix compilation issue on non-DSDx
05/30/13   tws     Mainline the context checking. Make the therm read trigger
                   umid dependent on context.
05/21/13   ggs     Fix for missing Qtuner writes during enter mode
05/13/13   ggs     Initialise default RX and TX NV table pointers in core handle
                   pointer based upon first band supported in BC NV mask
05/09/13   vb      Fix Tuner override API
05/08/13   tws     Support DUAL_SIM for power & HAL voting
05/07/13   tc      removed obsolete SPI function calls
05/03/13   sb      Fixes for vbatt read taking too long
05/02/13   vb      Tuner CA changes
04/30/13   aka     added rfgsm_core_handle_nv_init
04/22/13   av      Removing the hardcoding in rfm power clk on and power clk off function.
                   Causes issues for improper voting for device manager in X+G
04/16/13   sb      Initialize the headroom table only once during rfgsm_core_data_init
04/12/13   sb      Append APT RFFEs with absolute timing, not relative timing.
04/10/13   ndb     Removed the unused code of Qtuner NikeL
04/09/13   sr      removed legacy antenna tuner code which is not used.
03/28/13   aka     changed initial value of tx_log.num_slots to 0 from 255.
03/27/13   sb      Added support for Enhanced APT
03/20/13   ec      Replace deprecated RFM_MAX_DEVICES with RFM_MAX_WAN_DEVICES
03/18/13   sc      Initialise all core handle vars and clean up comments
03/18/13   svi     Replaced memcpy() by safer memscpy().
03/14/13   ec      Update rf_warmup_time from core_handle during get_timing_info
03/12/13   tsr     Cleanup the therm read event handle before rfgsm exit
03/05/13   tsr     Fix the bug which was corrupting rfc_gsm_core_ptr as a part of rfgsm_core_handle
03/01/13   tsr     Correct misleading f3 messages due to typo
02/28/13   tsr     Initialise num_slots to zero to avoid invalid TX log F3 messages.
02/28/13   lcl     Hardcode Qtuner to one instance.  Need to be fixed.
02/22/13   sb      Fix memory leak due to DALsys malloc called without free during vbatt read
02/22/13   tws     Make the IQ capture buffer dynamic.
02/21/13   tws     ftm_gsm_init_once is not device specific.
02/19/13   sc      Added APIs to allow GL1 to alloc and free Rx meas buffers
02/15/13   sb      Added FTM hook to override tuner tune code
02/13/13   sb      Enable antenna tuner device calls in RFC
02/13/13   tsr     Added GSM temp comp algorithm implementation
02/12/13   sc      Add init of GSM mode during core vars init and Wakeup
02/12/13   sr      changes to free the memory alloocated during rfm_init()
02/06/13   tsr     Added GSM temp comp algorithm implementation
02/06/13   tws     Add rfcommon_msg.h header and re-enable the F3 messages.
01/30/13   sc      Init the required GSM RF Task priority
01/29/12   sb      Added changes to obtain vbatt reading for vbatt comp
01/22/12   sb      Changes to enable QPOET transactions to go out every gsm frame
01/14/13   tsr     Added changes for Therm read implementation
01/11/13   aka     Changes to support simultaneous Tx SAR
01/09/13   sr      Made changes to run vbatt/temp comp in rf app task context.
01/03/12   sc      Added therm read full call flow framework and calc
12/18/12   sc      Added therm read event generation framework
01/3/13    av      Changes to support chain2 for GSM
12/14/12   ec	   Fix GSM NS frequency error issue
12/10/12   tws     Move global variables to the gsm core handler.
12/05/12   aka     Removed some hardcoding for device id
12/03/12   aka     Changes to use NV ptr from rfgsm_core_handle_ptr
12/03/12   ec      Fix compiler warning
12/03/12   tws     Move the FTM variable initialisation back to core.
11/29/12   tws     Move FTM prototypes.
11/28/12   aka     Changes to use rfgsm_core_handle_ptr from path based handler
11/26/12   aka     Compiler warning fixes
11/26/12   aka     Fixing some OFT build issues
11/26/12   aka     Changes to use a path specific gsm handle
11/23/12   tsr     Tx Alpha and TX SBI start delta update for DIME based on wtr1605
11/21/12   av      Added rfm_device parameters to mc/core/mdsp/msm layers.
11/19/12   sc      RF-Only mDSP update shared variables cleanup
11/16/12   sb      Made changes to convert RF_ON to SSBI
11/16/12   aka     Added RFDEVICE_GSM_SET_HMSC_STATE to set HSMC state in WTR driver
11/12/12   sc      Ensure a consistent Rx Alpha in prep for mDSP interface changes
11/08/12   tsr     Increasing the worst case ISR time to 4500QS
10/17/12   vb      Creating tuner managers during GSM core init
10/11/12   gh      Enable QTuners in enter_mode and wakeup
11/02/12   jfc     Replace rf_hal_enable_rfcmd_app with rf_hal_bus_enable
09/19/12   adk     Updated antenna tuner management APIs.
09/05/12   kai     Added tuner device interface and scripting support
10/19/12   sb      Made Changes to vote RFCMD app client during GSM wakeup and release during GSM sleep
10/18/12   tsr     Removed CCS time profilling api created during DIME RF GSM Bringup
10/18/12   tsr     Cleanup GSM bringup flags
10/18/12   sb      Made changes to include ssbis for PA/ASM in TX/RX burst script (SVVZ card bringup)
10/11/12   sc      Cleanup up unrequired Rx burst & SBI/Non-SBI API burst buffer params
10/05/12   tsr     First cut of RF time profile.
10/04/12   sr      Moved IRAT APIs to rfgsm_core_meas file.
10/04/12   sr      Added IRAT APIs.
10/01/12   sb      Made changes to add rfm device to rf device interface and uppler layer APIs
09/25/12   tc      Mainline RF_HAS_RFA_GSM_TASK.
09/25/12   tsr     Moved computation of RX and TX switchpoints to rfgsm_core_init
09/24/12   sb      Made changes to update TXLM,RXLM modem chain
09/24/12   tc      Mainline RF_HAS_RFA_GSM_TASK.
09/18/12   tc      Remove FEATURE_RF_HAS_QFE1320-related functionality.
09/18/12   sb      Added voting of GSM client through RF HAL API for requesting and releasing RFCMD app
09/06/12   ggs     Removed ftm_tx_cal_set_band. Set Tx band should always
                   be called from GL1 and not from ftm directly
08/25/12   tsr     Initialise ftm mutex irrespective of the mode
08/25/12   tsr     Changed TX alpha to work with new TX timeline reference(hardcoded)
08/24/12   sb/tsr  Made changes to clear the NON SBIs only during band change and SBIs during setup TX/RX burst
08/21/12   sc      Change GSM_RUMI bringup feature to task-specific feature
08/17/12   ec      Fixing Compilation error
08/09/12   ggs     Iniitialised ftm_tx_cal_set_band
08/08/12   ec      Fixed compilation error (rfgsm_handle initialisation)
08/01/12   ec      Modified rfgsm_card.ftm_slot_override initialisation to reflect changes
                   to RF side FTM processing
08/07/12   sb      Made MC/Core and rfdevice changes to support refactored WTR1605 changes
08/07/12   sc      Using seperate Rx and Tx scripts for CCS buffer tracability
08/06/12   sc      Move rf_buffer_clear for scripts to the dispatcher APIs
08/06/12   sc      Edit API to update CCS header FW vars
08/02/12   sc      Add num slots to setup Tx burst API
08/07/12   tsr     Fix the broken RFA tips with CL2664361
08/02/12   tsr     Changes for GSM Rumi Bring-up
07/26/12   sr      Changes to APIS to take rfm_dev as parameter.
                   Added code create and initialize core handle based on rfm_dev.
07/20/12   sr      Made changes to pass the RFM_DEVICE to rfc gsm functions
07/18/12   ec      Added code to initialise rfgsm_card.ftm_slot_override data
07/06/12   sc      Edit API parameters to support CCS buffer format
07/06/12   sc      Remove unrequired num transactions init functionality
07/02/12   sr      renamed rfc_common_gsmctl.h to rfc_common_tdd.h as it is shared with TDS.
06/27/12   sc      Passing correct pointer to device for CCS script populating
06/21/12   sc      Enable shared memory rf task support
06/20/12   shb     Updated GSM enter mode API to populate script buffer
07/12/12   sn      PA/ASM device interface and scripting support
07/09/12   aak     Move Quiet mode API from device to RFC
06/26/12    vb     Fix QFE1510 init for GSM
06/21/12    vb     Added support for QFE1510
05/22/12   sn      Reverting order of function call
05/15/12   sb      Added changes for debug
05/14/12   zc      Adding initialize for FTM matrix mutex
04/17/12   sn      QFE1320 function rename
03/26/12   sb      Changes in IQ capture to accomodate for OSR2.
02/28/12   dej     Hooked up PA start/stop delta adjustments in FTM mode.
                   ( Added rfgsm_card.tx_timing_adjust_flag )
02/16/12   sb      Fixing ftm_gsm_set_mode leaking rex critical sections
2/16/11    zc      Added support to turn on/off antenna tuner in rfgsm_core_power_clock_on/off
02/15/12   vrb     RF Common Device Manager Implementation.
02/06/12   dej     Disable GP_CLK
12/20/11   sn      Added support for QFE1320
01/10/12   sr      changes to enable vbatt comp.
12/07/11   sb      Support for GSM IQ capture in Online and FTM mode
11/29/11   sb      Added functionality for GSM IQ CAPTURE
11/18/11   aak     Klokwork fix in rfgsm_core_turn_on_vregs() and  rfgsm_core_turn_off_vregs()
11/17/11   sb      Removed TCXOMGR frequency update in FTM in IDLE mode
11/17/11   aak     Changes to implement Quiet mode
11/11/11   sr      queue the log api to DPC task only when needed.
10/28/11   sb      Added XO compensation to the 10 second timer in FTM mode
10/28/11    sr     made changes use the tx_nv_tbl ptr for any tx NV items
                   and rx_nv_tbl for any rx NV items.
10/27/11    sr     added sar_state to qxdm tx_log msg.
10/21/11   dej     Initialize temp comp dynamic range table
10/19/11   sb      Apply therm read in idle frame and enable FTM temp comp and vbatt comp during idle frame
10/17/11    sr     made changes add F3 msgs via DPC task for isr overrun issue.
10/13/11    sr     made changes add F3 msgs via DPC task.
10/11/11    sr     added rfc sleep/wakeup calls to configure RFC signals for sleep/wakeup state.
09/15/11    sr     added antenna tuner support code.
09/09/11   av      Power up needed during rfgsm_core_init() as we read revision
                   for initializing scripts.
09/09/11   av      Separate init once and enter_mode api for IRAT cases
08/09/11   sr      In the init_once() we don't need to turn on the VREGS
                   and write SBI registers in the device. Removing that code.
08/06/11   sr      RFC Cleanup: removed un-used code.
08/05/11   sr/plim Old RFC cleanup.
07/11/11    sr     removed un-used code.
07/05/11   vrb     Replacing direct calls to rfgsm_ds with wrapper functions
07/01/11   vrb     Replacing direct calls to rfgsm_ds with wrapper functions
04/28/11   rsr     Remove bbrx references.
04/28/11   rsr     Support for GSM to use NPA APIs
31/23/11   sr      Removed featurization code
03/21/11    sr     Added LM support and timing info access interface.
03/03/11    sr     merge from mdm9k branch
31/01/11   sr      Removed un-used code
01/17/11   dej     Mainline RF_DEVICE_MERGE_INTERIM_FEATURE
12/15/10   av      RFGSMCTL files cleanup
11/18/10   lcl     Fixed compiler warning
11/11/10   kma     Removed the temporary workaround
11/02/10   lcl     Fixed compiler warning
10/18/10   av/lcl  Clean up common RFC
10/14/10   mkv/whc Integrating Reload RFNV from SCMM into MDM9x00 (CL 1315259)
10/11/10   lcl    Changed MSG_LVL to MSG_LEGACY to avoid too many F3 messages
09/29/10   lcl     Return the status for rfgsm_core_init
21/09/10   ggs     Added new api to return rf_warmup time
09/21/10   lcl     Lint fix.
09/20/10   lcl     Added code to prevent crash w/o GSM card file for new card
09/01/10   av      GSM RFC Rearch
07/16/10   ap      Enable rfgsm_core_turn_off_vregs()
07/09/10   av      Hack to turn on the txco clk till tcxo manager/L1 fixes it.
06/27/10   av      Removed redundant function call.
06/24/09   rsr     support for scmm/mdm9k merge
03/18/10   lcl     Merged changed from QSC6295
03/09/10   tsr     added function pointer for setup_stop_acq
03/01/10   tsr     added rfc_manage_vregs to rfgsm_core_turn_on_vregs
01/12/10   vrb     Compiler Warnings Fixes
12/22/09   sr      changed the debug msg macro from legacy to new one.
12/08/09   sr      removed un-used code.
12/02/09   vbr     Initialize rfgsm_card variable to default in wakeup
                   if we turn off RTR during sleep.
10/26/09   sr      Compiler warnings fix
10/21/09   sr      new device driver interface changes.
10/15/09   wen     enable RF GSM sleep/wakeup
10/12/09   sr      Fixed warnings.
09/17/09   sar     Removed deprecated file, adie.h for CMI conformance.
07/20/09   sr      use new generic mdsp interface functions & code cleanup
07/11/09   sr      Initialize gsm specific msm related settings during enter_mode
07/06/09   sr      changes to make the operating band to rfcom_gsm_band type
07/04/09   sr      remove the tx_band setting from set_band()
06/26/09   sr      Code clean up
06/06/009  sr      unused code cleanup.
03/31/009  sr      added rfgsm_get_freq_error()
03/27/009  sr      rfgsm_core_ctl name changed to rfc_card_gsmctl
03/25/009  sr      check for rfc_gsm_ptr=NULL bfore using it.
03/18/09   sr      Changed the rfgsm_mdsp__() name to reflect new name
03/13/09   sr      SCMM build code celanup.
19/08/08   og      Mustang bring-up changes.

===========================================================================*/
#include "amssassert.h"
#include "customer.h"
#include "rfa_variation.h"
#include "rfcom.h"
#include "msg.h"
#include "rfdevice_gsm_intf.h"
#include "rfgsm_core_types.h"
#include "rfgsm_nv_cmn.h"
#include "rfgsm_mdsp.h"
#include "rfgsm_msm.h"
#include "rfgsm_msm_rflm_intf.h"
#include "rfcommon_core_types.h"
#include "rfcommon_core.h"
#include "rfcommon_msg.h"
#include "rfc_card.h"
#include "rfc_card_gsm.h"
#include "ftm.h"
#include "ftm_gsm_ctl.h"
#include "rfgsm_core_timing_consts.h"
#include "rfgsm_core.h"
#include "rfgsm_core_util.h"
#include "rfgsmcust.h"
#include "pm.h"
#include "rfc_common_tdd.h"
#include "tcxomgr.h"
#include "rfgsm_core_tx.h"
#include "ftm_gsm.h"
#include "rfcommon_core_device_manager.h"
#include "rfgsm_core_handle.h"
#include "rfgsm_core_temp_comp.h"
#include "rfgsm_core_vbatt_comp.h"
#include "rfgsm_core_apps_task_processing.h"
#include "modem_mem.h"
#include "rf_hal_common.h"
#include "rf_hal_buffer.h"
#include "rfm_internal.h"
#include "stringl.h"
#include "geran_msg.h"
#include "ftm_gsm_rfctl.h"
#include "geran_test_mode_api.h"
#include "rflm_time.h"
#include "rfgsm_data_manager.h"
#include "rflm_gsm_msg.h"
#include "rfcommon_atuner_intf.h"
#include "rfcommon_nv_mm.h"
#include "rfm_mode_types.h"
#include "rfgsm_mc.h"
#include "rfcommon_nv_mm.h"

#ifdef FEATURE_DUAL_SIM
#include "rf_task_common_services.h"
#endif

#ifdef FEATURE_RF_ASDIV
#include "rfcommon_asdiv_manager.h"
#include "rf_dispatch_snum.h"
#include "rflm_gsm_msg.h"
#endif

#include "rfcommon_concurrency_manager.h"

/*===========================================================================*/
/*                                                                           */
/*                                MACROS                                     */
/*                                                                           */
/*===========================================================================*/
/* Macros for status checks and manipulation of subscription mask */
#ifndef TEST_FRAMEWORK
#define RFGSM_CORE_NO_CURRENT_SUBS(mask)       (atomic_read((atomic_word_t*)&mask)==0)
#define RFGSM_CORE_ONLY_ACTIVE_SUB(mask, sub)  (atomic_read((atomic_word_t*)&mask)==(1<<sub))
#define RFGSM_CORE_SET_SUB_ACTIVE(mask, sub)   (atomic_set_bit((atomic_word_t*)&mask, sub))
#define RFGSM_CORE_SET_SUB_INACTIVE(mask, sub) (atomic_clear_bit((atomic_word_t*)&mask, sub))
#define RFGSM_CORE_CLEAR_SUB_MASK(mask)        (atomic_set((atomic_word_t*)&mask, 0))
#else //TEST_FRAMEWORK
#error code not present
#endif //TEST_FRAMEWORK

#define RFGSM_CORE_RFC_BAND_SUPPORTED(mask, band)  ((mask>>band) & 1)

/* Macro to increment the static vreg voting variable.                       */
/* The manipulation of rfgsm_vreg_vote must be under it's own mutex          */
/* protection because multiple threads for separate devices can execute this */
/* code at the same time. This is because multiple devices map to the same   */
/* path and vregs/mcpm votes operate based on path, which means device mutex */
/* doesn't guarantee lock.*/
#define RFGSM_CORE_VREG_VOTE_INC_RET(path, subscription, rf_state, result)                                         \
                              rf_common_enter_critical_section(&rfgsm_core_vreg_vote_crit_sect);         \
                              if((path<RF_PATH_MAX) && (subscription<RFLM_GSM_MAX_SUBSCRIPTIONS))        \
                              {                                                                          \
                                rfgsm_vreg_vote[path][subscription][rf_state]++;                                   \
                                result = rfgsm_vreg_vote[path][subscription][rf_state];                            \
                              }                                                                          \
                              else                                                                       \
                              {                                                                          \
                                RF_MSG_4(MSG_LEGACY_ERROR,                                               \
                                       "RF GSM core vreg vote invalid path %d, sub %d, rf_state %d, current vote %d!",\
                                       path, subscription, rf_state, rfgsm_vreg_vote[path][subscription][rf_state]);         \
                              }                                                                          \
                              rf_common_leave_critical_section(&rfgsm_core_vreg_vote_crit_sect);

/* Macro to decrement the static vreg voting variable.                       */
/* The manipulation of rfgsm_vreg_vote must be under it's own mutex          */
/* protection because multiple threads for separate devices can execute this */
/* code at the same time. This is because multiple devices map to the same   */
/* path and vregs/mcpm votes operate based on path, which means device mutex */
/* doesn't guarantee lock.*/
#define RFGSM_CORE_VREG_VOTE_DEC_RET(path, subscription, rf_state, result)                                         \
                              rf_common_enter_critical_section(&rfgsm_core_vreg_vote_crit_sect);         \
                              if((path<RF_PATH_MAX) && (subscription<RFLM_GSM_MAX_SUBSCRIPTIONS))        \
                              {                                                                          \
                                if(rfgsm_vreg_vote[path][subscription][rf_state] > 0)                              \
                                {                                                                        \
                                  rfgsm_vreg_vote[path][subscription][rf_state]--;                                 \
                                  result = rfgsm_vreg_vote[path][subscription][rf_state];                          \
                                  if(rfgsm_vreg_vote[path][subscription][RF_PATH_RXTX_STATE] > 0)        \
                                  {                                                                     \
                                    rfgsm_vreg_vote[path][subscription][RF_PATH_RXTX_STATE]--;          \
                                    RF_MSG_3(MSG_LEGACY_HIGH,                                               \
                                    "RF GSM core vreg vote for Tx state is decremented for path %d, sub %d, current vote %d!",\
                                       path, subscription, rfgsm_vreg_vote[path][subscription][RF_PATH_RXTX_STATE]);         \
                                    if(rfgsm_vreg_vote[path][subscription][RF_PATH_RXTX_STATE] != 0)    \
                                    {                                                                   \
                                      RF_MSG_3(MSG_LEGACY_ERROR,                                               \
                                       "RF GSM core vreg vote for Tx state is not zero path %d, sub %d, current vote %d!",\
                                       path, subscription, rfgsm_vreg_vote[path][subscription][RF_PATH_RXTX_STATE]);         \
									}                                                                   \
 								  }                                                                     \
                                }                                                                        \
                              }                                                                          \
                              else                                                                       \
                              {                                                                          \
                                RF_MSG_4(MSG_LEGACY_ERROR,                                               \
                                       "RF GSM core vreg vote invalid path %d, sub %d, rf_state %d, current vote %d!",\
                                       path, subscription, rf_state, rfgsm_vreg_vote[path][subscription][rf_state]);         \
                              }                                                                          \
                              rf_common_leave_critical_section(&rfgsm_core_vreg_vote_crit_sect);



/*===========================================================================*/
/*                                                                           */
/*                        DECLARATIONS\DEFINTIONS                            */
/*                                                                           */
/*===========================================================================*/
static rf_hal_bus_client_handle* rf_gsm_hal_bus_client_handle;
static rf_lock_data_type rfgsm_core_hal_bus_crit_sect;
static uint8 rfgsm_hal_bus_vote_count = 0;

extern rf_task_data_type rf_task_data[RF_MAX_TASK];

#define RFGSM_MAX_NUM_SLOTS GFW_MAX_BURSTS_PER_FRAME

#define RFGSM_HL_TABLE_BITS_PER_INDEX (sizeof(sawless_lin_table_entry)*8) //Assumes 8 bits per byte

/* RF card structure. It contains all information pertaining to the RF card to
   be programmed. Should be updated in rfgsm_config.
*/
rfgsm_card_type rfgsm_card;
/* rfc_gsm_core_ptr is initialised in rfgsm_core_init and referenced in
   rfgsm_core_get_warmup_time G
*/

/* rfgsm core handle will have all the state variables specific to a RFM_DEVICE.
   for now we can just have two core handles, but as required we can increase the number of core
   handles. */
static rfgsm_core_handle_type *rfgsm_core_handles[RFM_MAX_WAN_DEVICES] = {0};

uint32 rfgsm_core_worst_case_isr_start_time = 4500;

static const uint32 RFGSM_CORE_ASD_RSP_CHECK_START_INIT = 15;

/* Power Collapse Debug Variable */
extern boolean rf_disable_powercollapse;

/* Keep a count of how many votes a path AND subscription have. */
#define RF_GSM_MAX_VREG_VOTE_COUNT 2
static rf_lock_data_type rfgsm_core_vreg_vote_crit_sect;
static uint32 rfgsm_vreg_vote[RF_PATH_MAX][RFLM_GSM_MAX_SUBSCRIPTIONS][RF_PATH_MAX_STATE] = {{0}};

/* Define RFFE and SSBI CCS execution time in us */
#define SSBI_EXEC_TIME 2.2
#define RFFE_MULTI_BYTE_EXEC_TIME 4.1
#define ROUNDING_OFFSET 0.5
/*The assumption for below 40Qs is that in case of RACH we can read between
  RACH burst. RACH burst is of length 88Qs*/
#define RFGSM_CORE_DEVICE_STATUS_READ_TRIGGER_OFFSET -40

/* Define mapping between sub-id/rf_task, UMID  and RFLM therm response UMID*/
rfgsm_sub_to_therm_umid_map_type rfgsm_sub_to_therm_umid_map[3]= {
                                                                      { RF_TASK_MAIN,0,0},
                                                                      { RF_TASK_1,GERAN_FW_RFM_THERM_READ_CMD,RFLM_GSM_THERM_READ_RSP},
                                                                      { RF_TASK_2,GERAN_FW2_RFM_THERM_READ_CMD,RFLM_GSM_THERM_READ_SUB2_RSP}
                                                                     };

/*===========================================================================*/
/*                                                                           */
/*                          FUNCTION PROTOTYPES                              */
/*                                                                           */
/*===========================================================================*/
void rfgsm_core_enable_hal_bus( boolean enable, uint8 sub_id );
void rfgsm_core_turn_off_vregs(rf_path_enum_type path, uint8 sub_id);
void rfgsm_core_temp_comp_dynamic_range_tbl_init(rfm_device_enum_type rfm_dev);
extern boolean rfgsm_core_temp_comp_data_init(rfm_device_enum_type rfm_dev);
extern void rfgsm_mdsp_append_scripts(void * scripts1, void *scripts2);
extern uint64 rfcommon_nv_get_rf_bc_config(rf_path_enum_type path);
boolean rfgsm_core_enable_disable_hw( boolean enable,
                                      rfgsm_core_data_type *core_data,
                                      boolean prx_device,
                                      rf_path_enum_type rf_path );

boolean rfgsm_core_locate_arfcn_in_high_lin_table( uint16 arfcn,
                                                   rfcom_gsm_band_type band,
                                                   uint8 *entry,
                                                   uint8 *bit );


/*===========================================================================*/
/*                                                                           */
/*                         FUNCTION DEFINITIONS                              */
/*                                                                           */
/*===========================================================================*/
/*! @fn rfgsm_core_init                                                      */
/*! @brief This function initialises GSM RF once per device during power-up  */
/*!        from rcinit                                                       */
/*! @details This function does one-time initialisation of the RF devices,   */
/*!          NV, internal variables, vbatt and temp comp, and initialises    */
/*!          apps task                                                       */
/*! @param rfm_dev - the logical device to initialise                        */
/*! @retval boolean success status                                           */
boolean rfgsm_core_init(rfm_device_enum_type rfm_dev)
{
  boolean status=TRUE;
  boolean hmsc_flag = FALSE;
  boolean  enable_tcxo_buffer=TRUE;
  boolean  enable_dev_voting=FALSE;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr;
  rf_path_enum_type rf_path;
  boolean is_device_tx = TRUE;

  RFGC_MSG( MSG_LEGACY_HIGH, "rfgsm_core_init" );

  /* initialize the rfgsm core handle for the specified rfm_dev */
  rfgsm_core_handle_ptr = rfgsm_core_handle_create_and_init(rfm_dev);

  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!", FALSE);

  rf_path = rfcommon_core_device_to_path(rfm_dev);


  /*Find out if IP2 cal is supported or not*/
  rfgsm_core_init_sawless_status(rfm_dev);

  /* Explicitly allocate HL table for all GSM bands in case IRAT takes place without Enter Mode */
  if(rfgsm_core_get_sawless_status(rfm_dev))
  {
   	/* Explicitly allocate HL table for all GSM bands */
    if( rfgsm_core_allocate_high_lin_flag_table(rfm_dev) == FALSE )
    {
      RFGC_MSG(MSG_LEGACY_ERROR,
               "rfgsm_core_init, did not allocate HL table data!");
    }
  }

  /* Initialize the default gsm NV table and rx switch ptr */
  rfgsm_core_nv_init(rfm_dev);

  /* Initialize RFC data for GSM mode */
  rfc_gsm_init_once(rfm_dev);

  /* initalize mdsp fw related settings once per power-up  */
  rfgsm_mdsp_init_once(rfm_dev);

  /*Initialize gsm ftm */
  ftm_gsm_init_once();

  /* checking for rfc_gsm_ptr is not enough as new card file will update it
   * for the technology that requires the card file.  But since GSM doesn't
   * have the new card file, rfc_core_config_ptr is still NULL.  This check
   * is to prevent access to NULL RFC function pointers.
   */

  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr->rfc_gsm_core_ptr, "rfgsm_core_init: rfc_gsm_ptr==NULL", FALSE);

  /*We need to call rfdevice_gsm_init_once() api to intialize all the pointers but we read revison
  for lpf and that would cause issues. Till we find a better solution we need to do the below steps*/
    /* turn the vregs required by GSM */
  rfgsm_core_turn_on_vregs(rf_path, (uint8)RFLM_GSM_DEFAULT_SUBSCRIPTION, RF_PATH_RX_STATE);

  /*Enable TCXO buffer*/
   (void)rfdevice_gsm_cmd_dispatch( rfm_dev, rf_path, RFDEVICE_ENABLE_TCXO_BUFFER, &enable_tcxo_buffer);

  /* update the device layer with HMSC information*/
  rfdevice_gsm_cmd_dispatch( rfm_dev, rf_path, RFDEVICE_GSM_SET_HMSC_STATE, (void*)&hmsc_flag);

  /*Enable Device Power Voting */
  enable_dev_voting = rfcommon_core_dev_power_vote(rfm_dev, RFCMN_CORE_DEV_ON, RFCMN_CORE_DEV_NORMAL_VOTE);

  if(FALSE == enable_dev_voting)
  {
    RFGC_MSG( RF_FATAL, "rfgsm_core_init(): Device Pwr Up failed()");
  }

  /* Do one time initialization of WTR */
  (void)(rfdevice_gsm_init_once(rfm_dev));

  enable_tcxo_buffer = FALSE;

   /*Disable TCXO buffer*/
   (void)rfdevice_gsm_cmd_dispatch( rfm_dev, rf_path, RFDEVICE_ENABLE_TCXO_BUFFER, &enable_tcxo_buffer);

  /*Disable Device Power Voting */
  enable_dev_voting = rfcommon_core_dev_power_vote(rfm_dev, RFCMN_CORE_DEV_OFF, RFCMN_CORE_DEV_NO_VOTE);

  if(FALSE == enable_dev_voting)
  {
    RFGC_MSG( RF_FATAL, "rfgsm_core_init(): Device Pwr Down failed()");
  }

  rfgsm_core_turn_off_vregs(rf_path, (uint8)RFLM_GSM_DEFAULT_SUBSCRIPTION);

  /* Register RF APPS CDMA dispatcher*/
  rf_apps_dispatch_register( rfgsm_core_apps_task_cmd_dispatch, RF_APPS_GSM_ID );

  /*Check if device is TX capable*/
  is_device_tx = rfgsm_core_get_device_tx_capability(rfm_dev);

  if(is_device_tx)
  {
    /* Initialize the vbatt compensation */
    rfgsm_core_vbatt_comp_init(rfm_dev);

      /* Initialize the PA temp compensation */
    rfgsm_core_temp_comp_init(rfm_dev);

#ifndef T_RUMI_EMULATION

    // Non blocking vbatt read during core init
    rfgsm_core_vbatt_read_trigger(0, rfm_dev, FALSE);
#endif

  }

#ifndef T_RUMI_EMULATION
  rfgsm_core_calc_rx_timing(rfm_dev);
#endif

/*Asdiv response received error case timer intilization*/
#ifdef FEATURE_RF_ASDIV
  rfgsm_core_asd_rsp_check_init(rfm_dev);
#endif

  /* Allocate the RF SW interface data buffer */
  RF_NULL_CHECK_RTN(rfgsm_core_dm_alloc_intf_data_mem(),"rflm_gsm_intf is NULL!", FALSE);

  return( status );

} /* end of rf_init_once_gsm */



/*===========================================================================*/
/*! @fn rfgsm_core_deinit                                                    */
/*! @brief This destroys all resources initialised during power up core init */
/*! @details This function is called during power-down sequence              */
/*! @param void                                                              */
/*! @retval boolean success status                                           */
boolean rfgsm_core_deinit(void)
{
  /* Free the interface data */
  if(!rfgsm_core_dm_free_intf_data_mem())
  {
    RF_MSG(MSG_LEGACY_ERROR, "RF GSM core DM free RFLM data intf mem failed!");
    return FALSE;
  }

  /* Initialise the rfgsm hal vote mutex */
  rf_common_deinit_critical_section(&rfgsm_core_hal_bus_crit_sect);
  rf_common_deinit_critical_section(&rfgsm_core_vreg_vote_crit_sect);

  rfgsm_core_dm_rflm_deinit();

  rf_apps_dispatch_deregister();
  rfgsm_mdsp_deinit();
  rfgsm_core_handle_destroy();
  rfgsm_msm_adc_handle_destroy();

  return TRUE;
}



/*===========================================================================*/
/*! @fn rfgsm_core_init_once                                                 */
/*! @brief This executes initialisation only once and not per device         */
/*! @details This function is called during power-up sequence                */
/*! @param void                                                              */
/*! @retval boolean success status                                           */
boolean rfgsm_core_init_once(void)
{
  /* Initialise the rfgsm hal vote mutex */
  rf_common_init_critical_section(&rfgsm_core_hal_bus_crit_sect);
  rf_common_init_critical_section(&rfgsm_core_vreg_vote_crit_sect);

  /* Initialise RFLM GSM */
  if(!rfgsm_core_dm_rflm_init())
  {
    RF_MSG(MSG_LEGACY_ERROR, "RF GSM core DM RFLM init failed!");
    return FALSE;
  }

  return TRUE;
}



/*===========================================================================*/
/*! @fn rfgsm_core_get_task_num                                              */
/*! @brief get the current execution task context                            */
/*! @details This function acquires the tcb pointer and returns the task enum*/
/*! @param void                                                              */
/*! @retval task number                                                      */
rf_task_num_type rfgsm_core_get_task_num( void )
{
  rex_tcb_type   *tcb_ptr = rex_self();
  uint8 i;

  for (i=0; i<RF_MAX_TASK; i++)
  {
    if (tcb_ptr == rf_task_data[i].tcb_ptr)
    {
      return i;
    }
  }

  if (tcb_ptr == TASK_HANDLE(ftm))
  {
    /* We are executing in FTM context */
    return RF_TASK_FTM;
  } else if (tcb_ptr == TASK_HANDLE(rf_apps))
  {
    /* We are executing in RF_APPS context */
    return RF_TASK_APPS;
  }

  /* Unknown RF Task context, return RF_TASK_MAIN to be safe*/
  return RF_TASK_1;
}

/*===========================================================================*/
/*! @fn rfgsm_core_get_task_id_from_sub_id                                              */
/*! @brief get the current task context mapping to subid                           */
/*! @details This function acquires the subid and returns the corresponding RF_TASK*/
/*! @param uint8 sub_id                                                               */
/*! @retval task number                                                      */
rf_task_num_type rfgsm_core_get_task_id_from_sub_id(uint8 sub_id )
{

  if (sub_id > 1)
  {
    /* Unknown RF Task context, return RF_TASK_MAIN to be safe*/
    return RF_TASK_MAIN;
  }
  else
  {
    sub_id++;
    return (rf_task_num_type)sub_id;
  }
}


/*===========================================================================*/
/*! @fn rfgsm_core_enable_hal_bus                                            */
/*! @brief Enables or disables the HAL bus                                   */
/*! @details Vote for the hal bus based on current task context              */
/*! @param enable - vote for enabling or disabling                           */
/*! @param sub_id - subscription making the vote                             */
/*! @retval void                                                             */
void rfgsm_core_enable_hal_bus( boolean enable, uint8 sub_id )
{
  /* Variable to decide whether to execute the vote or not */
  boolean vote_call = FALSE;

  /* Lock the hal bus enable functionality for G+G DSDA */
  rf_common_enter_critical_section(&rfgsm_core_hal_bus_crit_sect);

  /***************************************************************************/
  /* The following logic:                                                    */
  /* * prevents unrequired repeated voting                                   */
  /* * maintains active subscription count                                   */
  /* * prevents one subscription from unvoting when another is still active  */

  /* If a susbcription needs a vote */
  if( enable == TRUE )
  {
    /* HERE WE PREVENT MULTIPLE VOTING TO IMPROVE EXECUTION OVERHEAD  */
    /* If this subscription is the only active subscription then vote */
    /* Otherwise there is no need to vote */
    if( rfgsm_hal_bus_vote_count == 0 )
    {
      /* We need to call the hal vote API */
      vote_call = TRUE;
    }

    /* Increment the subscription count */
    rfgsm_hal_bus_vote_count++;
  }
  else //enable == FALSE
  {
    /* Decrement the subscription count */
    if( rfgsm_hal_bus_vote_count > 0 )
    {
      rfgsm_hal_bus_vote_count--;
    }

    /* HERE WE PREVENT ONE SUB UNVOTING ANOTHER */
    /* If this subscription is the only active subscription then we are */
    /* allowed to unvote */
    if( rfgsm_hal_bus_vote_count == 0 )
    {
      /* We need to call the hal vote API */
      vote_call = TRUE;
    }
  }

  /* If the vote flag was set then call the API to either vote or unvote */
  if( vote_call == TRUE )
  {
#ifndef T_RUMI_EMULATION
    rf_hal_bus_enable(enable,
                      &rf_gsm_hal_bus_client_handle,
                      RF_HAL_RFCMD_APP_CLIENT(GSM));
#endif
  }

  /* UnLock the hal bus enable functionality for G+G DSDA */
  rf_common_leave_critical_section(&rfgsm_core_hal_bus_crit_sect);

  RF_MSG_4(MSG_LEGACY_HIGH,
           "RF GSM Hal Bus Enable %d, for sub %d, voted %d, vote count %d",
           enable, sub_id, vote_call, rfgsm_hal_bus_vote_count);
}



/*===========================================================================*/
/*! @fn rfgsm_core_is_device_prx                                             */
/*! @brief is the device Primary RX device                                   */
/*! @details This function returns if the device is Primary RX device        */
/*! @param rfm_dev - RF logical device to check                              */
/*! @retval boolean result - is PRx device or not                            */
boolean rfgsm_core_is_device_prx (rfm_device_enum_type rfm_dev )
{
  if (rfm_dev == RFM_DEVICE_0 || rfm_dev == RFM_DEVICE_2)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief Returns the PRX device associated with a given device

  @param rfm_dev

  @retval rfm_device_enum_type

*/
rfm_device_enum_type rfgsm_core_get_prx_dev( rfm_device_enum_type rfm_dev )
{
  switch ( rfm_dev )
  {
  default:
  case RFM_DEVICE_0:
  case RFM_DEVICE_1:
    return RFM_DEVICE_0;
  case RFM_DEVICE_2:
  case RFM_DEVICE_3:
    return RFM_DEVICE_2;
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief Returns the DRX device associated with a PRX device

  @param prx_rfm_dev

  @retval rfm_device_enum_type - Returns DRX device

*/
rfm_device_enum_type rfgsm_core_get_drx_dev( rfm_device_enum_type prx_device)
{
  switch ( prx_device )
  {
  case RFM_DEVICE_0:
    return RFM_DEVICE_1;
  case RFM_DEVICE_2:
    return RFM_DEVICE_3;
  default:
    return RFM_INVALID_DEVICE;
  }
}

/*===========================================================================*/
/*! @fn rfgsm_core_set_device_capability                                            */
/*! @brief This function queries and set the RX and TX capability of a RFM device for a given GSM Band*/
/*! @details This function queries rfc_gsm_data and set the RX and TX capability of a RFM device for a given GSM Band*/
/*! @param rfm_dev - RF logical device to check                              */
/*! @retval boolean result - is PRx device or not                            */
boolean rfgsm_core_set_device_capability(rfm_device_enum_type rfm_dev,
                                         rf_path_enum_type rf_path )
{

  if (!rfc_gsm_set_device_capability(rfm_dev,rf_path))
  {
    RF_MSG_1(MSG_LEGACY_ERROR, "RF GSM retrieving rfm device capability failed for rfm_device : %d",rfm_dev);
    return FALSE; ;
  }

  return TRUE;
}


/*===========================================================================*/
/*! @fn rfgsm_core_get_device_tx_capability                                   */
/*! @brief is the device Tx capable                                          */
/*! @details This function returns if the device is Tx capable for GSM       */
/*! @param rfm_dev - RF logical device to check                              */
/*! @retval boolean result - is device TX capable                            */
boolean rfgsm_core_get_device_tx_capability(rfm_device_enum_type rfm_dev )
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;;
  rfgsm_core_handle_ptr = rfgsm_core_handle_get( rfm_dev );

  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!", FALSE);

  /* If any of GSM Bands are supported for TX on the device, return true*/
  if( rfgsm_core_handle_ptr->device_capability.tx_supported_band_mask != 0 )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================*/
/*! @fn rfgsm_core_check_device_rx_capability                                 */
/*! @brief is the device Rx capable                                          */
/*! @details This function returns if the device is Rx capable               */
/*! @param rfm_dev - RF logical device to check                              */
/*! @retval boolean result - is PRx device or not                            */
boolean rfgsm_core_get_device_rx_capability( rfm_device_enum_type rfm_dev )
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;;
  rfgsm_core_handle_ptr = rfgsm_core_handle_get( rfm_dev );

  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!", FALSE);

  if( rfgsm_core_handle_ptr->device_capability.rx_supported_band_mask != 0 )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}



/*===========================================================================*/
/*! @fn rfgsm_core_enable_disable_hw                                         */
/*! @brief enable/disable device power, CCS cmd intf, and vreg voting        */
/*! @details called within enter/wakeup=enable sleep/exit=disable            */
/*! @param enable - flag to determine if enabling or disabling HW            */
/*! @param core_data - data for dev, sub, task, core handle etc              */
/*! @param prx_device - if the current device is PRx                         */
/*! @param rf_path - logical path based on device                            */
/*! @retval boolean success status                                           */
boolean rfgsm_core_enable_disable_hw( boolean enable,
                                      rfgsm_core_data_type *core_data,
                                      boolean prx_device,
                                      rf_path_enum_type rf_path )
{
  /* Initialise variables */
  boolean enable_tcxo_buffer = enable;
  boolean dev_voting = TRUE;
  rfm_device_enum_type rfm_dev = core_data->rfm_dev;
  uint8 sub_id = core_data->sub_id;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = core_data->rfgsm_core_handle_ptr;
  unsigned long long start_pcycle = 0;
  uint32 start_ustmr =0;

  boolean is_rf_fr_25072_enabled = rfcommon_nv_get_lpm_feature_enabled();

  /***************************************************************************/
  /* Enter/Wakeup */
  if(enable)
  {
    /* If not the PRx device no need to vote for HAL buffer */
    if(prx_device)
    {
      rfgsm_core_enable_hal_bus(TRUE, sub_id);
    }

    /* turn the vregs required by GSM */
    rfgsm_core_turn_on_vregs(rf_path, sub_id, RF_PATH_RX_STATE);

    /*Enable Device Power Voting */
    dev_voting = rfcommon_core_dev_power_vote(rfm_dev, RFCMN_CORE_DEV_ON, RFCMN_CORE_DEV_NORMAL_VOTE);

    if(FALSE == dev_voting)
    {
      RF_MSG( RF_FATAL, "rfgsm_core_enter_mode(): Device Pwr Up failed()");
    }


    if( TRUE == is_rf_fr_25072_enabled)
    {

    /* Profiling */
      start_pcycle = qurt_get_core_pcycles();
      start_ustmr = rfgsm_mc_read_ustmr_count();

     /* Power on RX device (LPM->ON) */
    if ( !rfdevice_gsm_rx_pwr_wakeup( rfm_dev, rfgsm_nv_get_lowest_supported_band(rfm_dev) ) )
    {
      ERR_FATAL("rfdevice_gsm_rx_pwr_wakeup(): RX Device %d Pwr Up (LPM->ON) failed for band %d",
                       rfm_dev, rfgsm_nv_get_lowest_supported_band(rfm_dev),0);
     return FALSE;
    }

        RFGSM_DEBUG_EN_Q( core_data->rfgsm_core_handle_ptr, //core handle
                 RFGSM_CORE_RX_PWR_WAKEUP, //api
                 core_data->rf_task, //rf task
                 0, //command proc start qs
                 0, //arg1
                 0, //arg2
                 geran_test_mode_api_get_FN(core_data->as_id), //SW frame num
                 start_ustmr, //start time USTMR
                 rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                 start_pcycle, //start time pcycles
                 qurt_get_core_pcycles()-start_pcycle ); //duration pcycles
    }

    /* Clear the script buffer before use */
    if(!rf_buffer_clear((rf_buffer_intf*)rfgsm_core_handle_ptr->scripts.enter_mode_script))
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,
            "RF GSM card enter mode script buffer was unsuccessfully cleared", 0);
      return FALSE;
    }
  }
  /***************************************************************************/
  /* Sleep/Exit */
  else
  {
    /* Only Do this Tx core reset on G call operating on first transceiver */
    if ((rfm_dev == RFM_DEVICE_0) || (rfm_dev == RFM_DEVICE_2))
    {
      (void)rfdevice_gsm_cmd_dispatch( rfm_dev, rf_path, RFDEVICE_WAN_TX_RESET, NULL);
      RF_MSG_1( RF_HIGH,
                "rfdevice_gsm_cmd_dispatch(): RFM Dev: %d executed for RFDEVICE_WAN_TX_RESET",
                rfm_dev);
    }

    if( TRUE == is_rf_fr_25072_enabled)
    {

      /* Profiling */
      start_pcycle = qurt_get_core_pcycles();
      start_ustmr = rfgsm_mc_read_ustmr_count();

    /* Power off RX device (ON->LPM) */
    if ( !rfdevice_gsm_rx_pwr_sleep( rfm_dev, rfgsm_nv_get_lowest_supported_band(rfm_dev) ) )
    {
      ERR_FATAL("rfdevice_gsm_rx_pwr_sleep()\\rfgsm_core_sleep(): RX Device %d Pwr Down (ON->LPM) failed for band %d",
              rfm_dev, rfgsm_nv_get_lowest_supported_band(rfm_dev),0);
      return FALSE;
    }

      RFGSM_DEBUG_EN_Q( core_data->rfgsm_core_handle_ptr, //core handle
                 RFGSM_CORE_RX_PWR_SLEEP, //api
                 core_data->rf_task, //rf task
                 0, //command proc start qs
                 0, //arg1
                 0, //arg2
                 geran_test_mode_api_get_FN(core_data->as_id), //SW frame num
                 start_ustmr, //start time USTMR
                 rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                 start_pcycle, //start time pcycles
                 qurt_get_core_pcycles()-start_pcycle ); //duration pcycles

    }

    /*Disable Device Voting*/
    dev_voting = rfcommon_core_dev_power_vote(rfm_dev, RFCMN_CORE_DEV_OFF, RFCMN_CORE_DEV_NO_VOTE);

    if(FALSE == dev_voting)
    {
      RF_MSG_1( RF_FATAL, "rfgsm_core_power_clock_off(): Device: %d Pwr Down failed()", rfm_dev);
    }

    /* Power off RF RX hardware */
    rfgsm_core_turn_off_vregs(rf_path, sub_id);

    /* If not the PRx device no need to vote for HAL buffer */
    if(prx_device)
    {
      rfgsm_core_enable_hal_bus(FALSE, sub_id);
    }
  }
  /***************************************************************************/

  return TRUE;
}



/*===========================================================================*/
/*! @fn rfgsm_core_enter_mode                                                */
/*! @brief Enable and initialise the RF for GSM mode                         */
/*! @details This function enables and sets up all the scripts necessary for */
/*!          GSM                                                             */
/*!          If only active subscription on this device then a full enter    */
/*!          mode is performed. If another subscription is active then only a*/
/*!          minimised enter mode is performed allocating RFLM memory        */
/*! @param core_data - data for dev, sub, task, core handle etc              */
/*! @param rxlm_buf_idx - modem chain index                                  */
/*! @param shared_mem_ptr - GFW shared memory pointer                        */
/*! @retval boolean success status                                           */
boolean rfgsm_core_enter_mode(rfgsm_core_data_type *core_data, uint32 rxlm_buf_idx, const void *shared_mem_ptr)
{
  /* Initialise variables */
  rf_path_enum_type rf_path;
  rfm_device_enum_type rfm_dev = core_data->rfm_dev;
  uint8 sub_id = core_data->sub_id;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = core_data->rfgsm_core_handle_ptr;
  boolean prx_device = TRUE;

#ifdef FEATURE_TX_SHARING
  uint16 temp_rx_num_trans = 0, temp_tx_num_trans = 0;
#endif
  RF_MSG( MSG_LEGACY_HIGH,"rfgsm_core_enter_mode" );

  rf_path = rfcommon_core_device_to_path(rfm_dev);

  prx_device = rfgsm_core_is_device_prx(rfm_dev);

  /* Allocate the subscription RFLM DM memory */
  if( !rfgsm_core_dm_alloc_data_manager_mem(sub_id) )
  {
    RF_MSG_1(MSG_LEGACY_ERROR, "RF GSM core DM data table alloc failed for sub %d!", sub_id);
    return FALSE;
  }

  //tx alternate path not supported for now
  rfgsm_core_handle_ptr->tx_alternate_path = 0;
  /*Initialise rx alternate path*/
  rfgsm_core_handle_ptr->rx_alternate_path = 0;

  /*Update the alt rx path settings to rfc. This api updates rfgsm_core_handle_ptr->alternate_path*/
  if (!rfgsm_core_update_alternate_rx_path_port_mapping(core_data, &rfgsm_core_handle_ptr->rx_alternate_path) )
  {
    RF_MSG_1(MSG_LEGACY_ERROR,
             "rfgsm_core_enter_mode, alternate path update for dev:%d! failed",
              rfm_dev);
  }
  
  /*After updating to alternative path(Sawless), rfgsm needs re-init*/
  if(override_gsm_port_to_sawless == TRUE)
  {
	rfgsm_core_init_sawless_status(rfm_dev);
	rfgsm_mdsp_init_once(rfm_dev);
  }


  /* Initialize the mdsp fw for GSM mode */
  rfgsm_mdsp_enter_mode_init(core_data);

  /* Initialize the enter mode rx */
  rfgsm_core_enter_mode_rx_init(core_data);

  /* Update the Tx timing data to WTR and RFLM DM if Tx capable */
  if( core_data->device_is_tx )
  {
    rfgsm_core_update_tx_timing(core_data);
  }

  /* Is this current execution the only active subscription on this device? */
  rfgsm_core_handle_ptr->no_active_subs = RFGSM_CORE_NO_CURRENT_SUBS(rfgsm_core_handle_ptr->sub_id_mask);

  /* If there is another subscription active on the device then return here */
  /* Do not need to re-vote/initialise the device as already intialised */
  if( !rfgsm_core_handle_ptr->no_active_subs )
  {
    /* turn on the vregs required by GSM */
    /* Note:Vregs are already on at this point and this is applying second sub vote */
    rfgsm_core_turn_on_vregs(rf_path, sub_id, RF_PATH_RX_STATE);

    /* Clear the enter mode event to prevent attempt to program stale event */
    if( !rfgsm_core_dm_clean_event( core_data, RFLM_GSM_ENTER_MODE_SCRIPT ) )
    {
      RF_MSG_1(MSG_LEGACY_ERROR, "Enter mode rfgsm_core_dm_clean_event failed for sub %d!", sub_id);
      return FALSE;
    }

    /* initialize the modem for GSM */
    rfgsm_msm_init(rxlm_buf_idx, rfm_dev, FALSE, rfgsm_core_handle_ptr->rx_alternate_path);

    RF_MSG_3(MSG_LEGACY_HIGH,
             "RF GSM active sub on dev %d, mask 0x%x, minimised enter performed for sub %d!",
             rfm_dev, rfgsm_core_handle_ptr->sub_id_mask, sub_id);

    /* Subscription active on device - set sub mask bit */
    RFGSM_CORE_SET_SUB_ACTIVE(rfgsm_core_handle_ptr->sub_id_mask, sub_id);

    return TRUE;
  }


//tx alternate path not supported for now
rfgsm_core_handle_ptr->tx_alternate_path = 0;

#ifdef FEATURE_RF_ASDIV
  /*Set the flag to set the antenna to previous position*/
  rfgsm_core_handle_ptr->set_prev_ant_pos = TRUE;
#endif

  /* Store the RxLM buffer idx */
  rfgsm_core_handle_ptr->rxlm_buf_index = rxlm_buf_idx;

#ifndef T_RUMI_EMULATION
  /* Enable RF Vregs, CCS cmd interface*/
  if( !rfgsm_core_enable_disable_hw(TRUE, core_data, prx_device, rf_path) )
  {
    RF_MSG(MSG_LEGACY_ERROR, "Enter mode rfgsm_core_enable_disable_hw failed!");
    return FALSE;
  }
#endif

#ifndef FEATURE_TX_SHARING
  /* initialize the RFC data structures and power management for GSM mode based on alternate path */
  rfc_gsm_mode_config(rfm_dev, (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.enter_mode_script, rfgsm_core_handle_ptr->rx_alternate_path);
#else
  /* initialize the RFC data structures and power management for GSM mode based on alternate path */
  rfc_gsm_mode_config(rfm_dev, (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.rx_enter_mode_script, rfgsm_core_handle_ptr->rx_alternate_path);

  temp_rx_num_trans = rf_buffer_get_num_trans((rf_buffer_intf*)rfgsm_core_handle_ptr->scripts.rx_enter_mode_script, RF_BUFFER_RFFE);

  RF_MSG_1(MSG_LEGACY_ERROR, "Rx enter mode rfc script num trans %d", temp_rx_num_trans);
#endif

  /* initialize the modem for GSM. rx lm handle gets updated based on alternate path */
  rfgsm_msm_init(rxlm_buf_idx, rfm_dev, FALSE, rfgsm_core_handle_ptr->rx_alternate_path);

   /* One time initialisation of RX WTR*/
  (void)(rfdevice_gsm_rx_enter_mode(rfm_dev,
                                    RFDEVICE_CREATE_SCRIPT,
                                    #ifndef FEATURE_TX_SHARING
                                    (rf_buffer_intf*)rfgsm_core_handle_ptr->scripts.enter_mode_script
                                    #else
                                    (rf_buffer_intf*)rfgsm_core_handle_ptr->scripts.rx_enter_mode_script
                                    #endif
                                    ));

  /* If PRx device need to initialise Tx */
  if( core_data->device_is_tx )
  {
	/* One time initialisation of TX WTR */
   (void)(rfdevice_gsm_tx_enter_mode(rfm_dev,
                                     RFDEVICE_CREATE_SCRIPT,
                                     #ifndef FEATURE_TX_SHARING
                                     (rf_buffer_intf*)rfgsm_core_handle_ptr->scripts.enter_mode_script
                                     #else
                                     (rf_buffer_intf*)rfgsm_core_handle_ptr->scripts.tx_enter_mode_script
                                     #endif
                                     ));
  }

#ifdef FEATURE_TX_SHARING
  temp_rx_num_trans = rf_buffer_get_num_trans((rf_buffer_intf*)rfgsm_core_handle_ptr->scripts.rx_enter_mode_script,
	                                          RF_BUFFER_RFFE);
  temp_tx_num_trans = rf_buffer_get_num_trans((rf_buffer_intf*)rfgsm_core_handle_ptr->scripts.tx_enter_mode_script,
	                                          RF_BUFFER_RFFE);
  RF_MSG_2(MSG_LEGACY_ERROR,
               "Rx enter mode num trans %d, tx enter mode num trans %d",
               temp_rx_num_trans, temp_tx_num_trans);
#endif

  rfgsm_core_handle_nv_init(rfm_dev);

  /* Perform any customer-requested initializations */
  rfgsmcust_init();

  /*Initialize DM data only if IP2 cal is supported*/
  if (!rfgsm_core_get_sawless_status(core_data->rfm_dev))
  {
    RF_MSG(MSG_LEGACY_ERROR, "rfgsm_core_enter_mode, Sawless not supported !");
  }
  else
  {
    rfgsm_core_handle_ptr->ip2_cal_alpha = rfgsm_core_handle_ptr->rfc_gsm_core_ptr->rx_timing->rx_alpha_qs + rfgsm_core_handle_ptr->rfc_gsm_core_ptr->tx_timing->tx_alpha_qs;

    if(!rfgsm_core_dm_init_ip2_cal_data(core_data))
    {
      RF_MSG(MSG_LEGACY_ERROR, "rfgsm_core_enter_mode, Failed to initialize IP2 cal data in DM !");
    }

    if ( rfgsm_core_build_jdet_read_scripts(core_data) == FALSE )
    {
      RF_MSG_1(MSG_LEGACY_ERROR,
               "rfgsm_core_enter_mode, did not build jdet read scripts for dev %d!",
               core_data->rfm_dev);
    }

	/* Explicitly allocate HL table for all GSM bands */
    if( rfgsm_core_allocate_high_lin_flag_table(core_data->rfm_dev) == FALSE )
    {
      RF_MSG_1(MSG_LEGACY_HIGH,
               "rfgsm_core_enter_mode, did not allocate HL table data for dev %d!",
               core_data->rfm_dev);
    }
  }

  /* Subscription active on device - set sub mask bit */
  RFGSM_CORE_SET_SUB_ACTIVE(rfgsm_core_handle_ptr->sub_id_mask, sub_id);

  return( TRUE );
} /* end of rfgsm_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function calls rfgsm_core functionality to create the RF CCS Events for all the
  Enter Mode.

  @param core_data      : rfgsm core structure that stores infomation like Multimode device
  @param enter_mode_ptr : Pointer to shared mem for Enter mode
  @retval               : True or False
*/

boolean rfgsm_core_build_enter_mode_ccs_events( rfgsm_core_data_type* core_data,
                                                const void *enter_mode_ptr)
{
  boolean ret_status = TRUE;

  /* If there is no other active subscription on the device, process enter mode script */
  if( core_data->rfgsm_core_handle_ptr->no_active_subs )
  {
    /* Program the enter mode event to RFLM DM */
    if( !rfgsm_core_dm_dynamic_enter_event_wrapper( core_data ) )
    {
      RF_MSG(MSG_LEGACY_ERROR,"RF GSM Enter mode RFLM data info event create failed!");
      ret_status &= FALSE;
    }
  }

  /* Populate the enter mode RFLM interface data information */
  if( !rfgsm_core_dm_populate_enter_mode_intf_data( core_data, (void*)enter_mode_ptr) )
  {
    RF_MSG(MSG_LEGACY_ERROR,"RF GSM Enter mode RFLM data info update failed!");
    ret_status &= FALSE;
  }

  return ret_status;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function calls rfgsm_core functionality to create the RF CCS Events for the
  wakeup Mode.

  @param core_data      : rfgsm core structure that stores infomation like Multimode device
  @param wakeup_mode_pt : Pointer to shared mem for Wakeup mode
  @retval               : True or False
*/

boolean rfgsm_core_build_wakeup_mode_ccs_events( rfgsm_core_data_type* core_data,
                                                 const void *wakeup_mode_ptr)
{
  boolean ret_status = TRUE;

  /* If there is another subscription active on the device then return */
  /* Do not need to re-vote/initialise the device as already intialised */
  if ( core_data->rfgsm_core_handle_ptr->no_active_subs )
  {
   /* Populate the wakeup RFLM event data table */
    if( !rfgsm_core_dm_dynamic_wakeup_event_wrapper(core_data) )
    {
      RF_MSG(MSG_LEGACY_ERROR,"RF GSM Wakeup RFLM dynamic event prog failed!");
      ret_status &= FALSE;
    }
  }

  /* Populate the wakeup RFLM data information */
  if( !rfgsm_core_dm_populate_wakeup_intf_data( core_data, (void*)wakeup_mode_ptr ) )
  {
    RF_MSG(MSG_LEGACY_ERROR,"RF GSM Wakeup RFLM data info update failed!");
    ret_status &= FALSE;
  }

  return ret_status;
}



/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details

   @param

   @retval

*/
boolean rfgsm_core_prepare_therm_read(rfgsm_core_data_type *core_data, const void *shared_mem_ptr)
{
  rf_path_enum_type rf_path;

  rf_path = rfcommon_core_device_to_path(core_data->rfm_dev);

  /* Check the therm read shared mem ptr from L1                         */
  RF_NULL_CHECK_RTN( shared_mem_ptr, "Therm read shared mem ptr is NULL!", FALSE);

  /* Store core handle ptr therm shared mem with shared mem from L1      */
  core_data->rfgsm_core_handle_ptr->therm_read.therm_read_shared_mem_ptr = (void*)shared_mem_ptr;

  if(!rf_buffer_clear((rf_buffer_intf*)core_data->rfgsm_core_handle_ptr->scripts.therm_read_script))
  {
    RF_MSG( MSG_LEGACY_ERROR, "RF GSM card therm read script buffer was unsuccessfully cleared" );
  }

  /* Set the GFW therm read trigger UMID based on current context */
  switch (rfgsm_core_get_task_num())
  {
  default:
  case RF_TASK_1:
    core_data->rfgsm_core_handle_ptr->therm_read.fw_therm_read_umid = rfgsm_sub_to_therm_umid_map[RF_TASK_1].therm_read_umid;/*GERAN_FW_RFM_THERM_READ_CMD */
    core_data->rfgsm_core_handle_ptr->therm_read.fw_therm_rsp_umid = rfgsm_sub_to_therm_umid_map[RF_TASK_1].therm_resp_umid;/*RFLM_GSM_THERM_READ_RSP*/
    break;
#ifdef FEATURE_DUAL_SIM
  case RF_TASK_2:
    core_data->rfgsm_core_handle_ptr->therm_read.fw_therm_read_umid = rfgsm_sub_to_therm_umid_map[RF_TASK_2].therm_read_umid;/*GERAN_FW2_RFM_THERM_READ_CMD*/
    core_data->rfgsm_core_handle_ptr->therm_read.fw_therm_rsp_umid = rfgsm_sub_to_therm_umid_map[RF_TASK_2].therm_resp_umid;/*RFLM_GSM_THERM_READ_SUB2_RSP*/
    break;
#endif
    /*! @todo add 3rd task here */
  }

  (void)rfdevice_gsm_cmd_dispatch( core_data->rfm_dev,
                                   rf_path,
                                   RFDEVICE_GSM_BUILD_THERM_READ_SCRIPT,
                                   (rf_buffer_intf*)core_data->rfgsm_core_handle_ptr->scripts.therm_read_script );

  return TRUE;
} /* end of rfgsm_core_prepare_therm_read */



/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details

   @param

   @retval

*/
boolean rfgsm_core_build_therm_read_event(rfgsm_core_data_type *core_data)
{
  RF_NULL_CHECK_RTN( core_data->rfgsm_core_handle_ptr->therm_read.therm_read_shared_mem_ptr, "Therm read shared mem ptr is NULL for device %d!", FALSE);

  if( !rfgsm_core_dm_dynamic_therm_event_wrapper(core_data) )
  {
    RF_MSG(MSG_LEGACY_ERROR,"RF GSM Therm Read RFLM dynamic event prog failed!");
    return FALSE;
  }

  /* Populate the therm read RFLM data information */
  if( !rfgsm_core_dm_populate_therm_read_intf_data( core_data,
                                                    core_data->rfgsm_core_handle_ptr->therm_read.therm_read_shared_mem_ptr ) )
  {
    RF_MSG(MSG_LEGACY_ERROR,"RF GSM Therm read RFLM data info update failed!");
    return FALSE;
  }

  return TRUE;
} /* end of rfgsm_core_build_therm_read_event */



/*===========================================================================*/
/*! @fn rfgsm_core_wakeup                                                    */
/*! @brief Enable and initialise the RF for GSM mode after sleep             */
/*! @details This function enables and sets up all the scripts necessary for */
/*!          GSM after going to sleep                                        */
/*!          If only active subscription on this device then a full wakeup   */
/*!          is performed. If another subscription is active then only a     */
/*!          minimised wakeup is performed allocating RFLM memory            */
/*! @param core_data - data for dev, sub, task, core handle etc              */
/*! @param shared_mem_ptr - GFW shared memory pointer                        */
/*! @retval boolean success status                                           */
boolean rfgsm_core_wakeup( rfgsm_core_data_type *core_data, const void *shared_mem_ptr )
{
  /* Initialise variables */
  rf_path_enum_type rf_path;
  rfm_device_enum_type rfm_dev = core_data->rfm_dev;
  uint8 sub_id = core_data->sub_id;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = core_data->rfgsm_core_handle_ptr;
  boolean dev_is_prx = TRUE;

  /* Get logical path and if device is PRx */
  rf_path = rfcommon_core_device_to_path(rfm_dev);
  dev_is_prx = rfgsm_core_is_device_prx(rfm_dev);


  //tx alternate path not supported for now
  rfgsm_core_handle_ptr->tx_alternate_path = 0;
  /*Initialise rx alternate path*/
  rfgsm_core_handle_ptr->rx_alternate_path = 0;

  /*Update the alt rx path settings to rfc. This api updates rfgsm_core_handle_ptr->alternate_path*/
  if (!rfgsm_core_update_alternate_rx_path_port_mapping(core_data, &rfgsm_core_handle_ptr->rx_alternate_path) )
  {
    RF_MSG_1(MSG_LEGACY_ERROR,
             "rfgsm_core_wakeup_mode, alternate path update for dev:%d! failed",
             rfm_dev);
  }

  rfgsm_core_handle_ptr->no_active_subs = RFGSM_CORE_NO_CURRENT_SUBS(rfgsm_core_handle_ptr->sub_id_mask);

  /* If there is another subscription active on the device then return */
  /* Do not need to re-vote/initialise the device as already intialised */
  if( !rfgsm_core_handle_ptr->no_active_subs )
  {
    /* turn on the vregs required by GSM */
    /* Note:Vregs are already on at this point and this is applying second sub vote */
    rfgsm_core_turn_on_vregs(rf_path, sub_id, RF_PATH_RX_STATE);

    /* Populate the wakeup RFLM data information */
    if( !rfgsm_core_dm_populate_wakeup_intf_data( core_data, (void*)shared_mem_ptr ) )
    {
      RF_MSG(MSG_LEGACY_ERROR,"RF GSM Wakeup RFLM data info update failed!");
      return FALSE;
    }

    /* Clear the enter mode event to prevent attempt to program stale event */
    if( !rfgsm_core_dm_clean_event( core_data, RFLM_GSM_ENTER_MODE_SCRIPT ) )
    {
      RF_MSG_1(MSG_LEGACY_ERROR, "Wakeup rfgsm_core_dm_clean_event failed for sub %d!", sub_id);
      return FALSE;
    }

    /* initialize the modem for GSM */
    rfgsm_msm_init(rfgsm_core_handle_ptr->rxlm_buf_index, rfm_dev, FALSE, rfgsm_core_handle_ptr->rx_alternate_path);

    RF_MSG_3(MSG_LEGACY_HIGH,
             "RF GSM active sub on dev %d, mask 0x%x, minimised wakeup performed for sub %d!",
             rfm_dev, rfgsm_core_handle_ptr->sub_id_mask, sub_id);

    /* Subscription active on device - set sub mask bit */
    RFGSM_CORE_SET_SUB_ACTIVE(rfgsm_core_handle_ptr->sub_id_mask, sub_id);

    return TRUE;
  } /* MINIMISED WAKEUP */

#ifndef T_RUMI_EMULATION
  if( !rfgsm_core_enable_disable_hw(TRUE, core_data, dev_is_prx, rf_path) )
  {
    RF_MSG(MSG_LEGACY_ERROR, "Wakeup rfgsm_core_enable_disable_hw failed!");
    return FALSE;
  }
#endif

  /* configure the RFC signals into wakeup state */
  rfc_gsm_wakeup(rfm_dev, rfgsm_core_handle_ptr->rx_alternate_path);

  /* initialize the modem for GSM */
  rfgsm_msm_init(rfgsm_core_handle_ptr->rxlm_buf_index, rfm_dev, FALSE, rfgsm_core_handle_ptr->rx_alternate_path);

  /* One time initialisation of RX WTR*/
  (void)(rfdevice_gsm_rx_enter_mode(rfm_dev,
                                    RFDEVICE_CREATE_SCRIPT,
                                    #ifndef FEATURE_TX_SHARING
                                    (rf_buffer_intf*)rfgsm_core_handle_ptr->scripts.enter_mode_script
                                    #else
                                    (rf_buffer_intf*)rfgsm_core_handle_ptr->scripts.rx_enter_mode_script
                                    #endif
                                    ));

  /* If PRx device need to initialise Tx */
  if( core_data->device_is_tx )
  {
    /* Update the Tx timing data to WTR and RFLM DM */
    rfgsm_core_update_tx_timing(core_data);

    /* One time initialisation of TX WTR */
    (void)(rfdevice_gsm_tx_enter_mode(rfm_dev,
                                      RFDEVICE_CREATE_SCRIPT,
                                      #ifndef FEATURE_TX_SHARING
                                      (rf_buffer_intf*)rfgsm_core_handle_ptr->scripts.enter_mode_script
                                      #else
                                      (rf_buffer_intf*)rfgsm_core_handle_ptr->scripts.tx_enter_mode_script
                                      #endif
                                      ));
  }

  /* The RTR was reset on sleep - so reinitialize all state variables to reflect this. */
  rfgsm_core_wakeup_data_init(rfm_dev);

  /* Set GSM mode flag to Standalone Mode*/
  rfgsm_core_handle_ptr->rfgsm_mode = RFGSM_STANDALONE_MODE;

  /* Subscription active on device - set sub mask bit */
  RFGSM_CORE_SET_SUB_ACTIVE(rfgsm_core_handle_ptr->sub_id_mask, sub_id);

  return TRUE;
} /* end of rfgsm_wakeup */



/*===========================================================================*/
/*! @fn rfgsm_core_sleep                                                     */
/*! @brief Disable and default the RF to sleep mode                          */
/*! @details This function disables and sets up all the voting necessary for */
/*!          GSM to go to sleep                                              */
/*!          If only active subscription on this device then a full sleep    */
/*!          is performed. If another subscription is active then only a     */
/*!          minimised sleep is performed (currently return and do nothing)  */
/*! @param core_data - data for dev, sub, task, core handle etc              */
/*! @retval boolean success status                                           */
boolean rfgsm_core_sleep( rfgsm_core_data_type *core_data )
{
  /* Initialise variables */
  rf_path_enum_type rf_path;
  boolean dev_is_prx = TRUE;
  rfm_device_enum_type rfm_dev = core_data->rfm_dev;
  uint8 sub_id = core_data->sub_id;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = core_data->rfgsm_core_handle_ptr;

  /* Get logical path and if device is PRx */
  rf_path = rfcommon_core_device_to_path(rfm_dev);

  /*If the device is Primary RX device*/
  dev_is_prx = rfgsm_core_is_device_prx(rfm_dev);

  /* Clear the enter mode event to prevent attempt to program stale event */
  if( !rfgsm_core_dm_clean_event( core_data, RFLM_GSM_ENTER_MODE_SCRIPT ) )
  {
    RF_MSG_1(MSG_LEGACY_ERROR, "Sleep rfgsm_core_dm_clean_event failed for sub %d!", core_data->sub_id);
    return FALSE;
  }

  /* If there is another subscription active on the device then return   */
  /* As another sub is active on the same device do not tear down device */
  if( !RFGSM_CORE_ONLY_ACTIVE_SUB(rfgsm_core_handle_ptr->sub_id_mask, sub_id) )
  {
    /* turn off the vregs required by GSM */
    /* Note: Vregs are required by other sub and will not shut down but voting off is required */
    rfgsm_core_turn_off_vregs(rf_path, sub_id);

    RF_MSG_3(MSG_LEGACY_HIGH,
             "RF GSM active sub on dev %d, mask 0x%x, minimised sleep performed for sub %d!",
             rfm_dev, rfgsm_core_handle_ptr->sub_id_mask, sub_id);
    RFGSM_CORE_SET_SUB_INACTIVE(rfgsm_core_handle_ptr->sub_id_mask, sub_id);

    return TRUE;
  }

  /* Remember the gsm band before we put the RF to sleep. This will be used in rfgsm_wakeup.*/
  rfgsm_core_handle_ptr->band_before_sleep = rfgsm_core_handle_ptr->band;

  /* If device is PRx then stop the temperature compensation */
  if(core_data->device_is_tx)
  {
    /* Stop PA Temp Tx compensation timer during sleep.*/
    rfgsm_core_temp_comp_stop(rfm_dev);
  }

#ifndef T_RUMI_EMULATION
  /* configure the RFC signals into sleep state */
  rfc_gsm_sleep(rfm_dev);

  if( !rfgsm_core_enable_disable_hw(FALSE, core_data, dev_is_prx, rf_path) )
  {
    RF_MSG(MSG_LEGACY_ERROR, "Sleep rfgsm_core_enable_disable_hw failed!");
    return FALSE;
  }
#endif

  /* Subscription inactive on device */
  RFGSM_CORE_SET_SUB_INACTIVE(rfgsm_core_handle_ptr->sub_id_mask, sub_id);

  return TRUE;
} /* end of rfgsm_sleep */



/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details

   @param

   @retval

*/
void rfgsm_core_set_band(rfgsm_core_data_type *core_data, rfcom_gsm_band_type band )
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(core_data->rfm_dev);

  RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!");
  /* setup nv table pointer */
  rfgsm_core_handle_ptr->rfgsm_rx_nv_tbl = rfgsm_nv_get_tbl_ptr(core_data->rfm_dev, band, RFGSM_NV_RX);
  RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr->rfgsm_rx_nv_tbl, "rfgsm_core_set_band NULL rfgsm_rx_nv_tbl!" );

  /* Initialize pointer to GSM/EGSM rx switch structure */
  rfgsm_core_handle_ptr->rfgsm_nv_rx_freq_comp_switch_ptr = rfgsm_nv_get_rx_freq_comp_switch_ptr(core_data->rfm_dev, band);
  RF_NULL_CHECK_RTN_V( rfgsm_core_handle_ptr->rfgsm_nv_rx_freq_comp_switch_ptr, "rfgsm_core_set_band NULL rfgsm_nv_rx_freq_comp_switch_ptr!" );

  /* If this band is SAWless, then load the switchpoints for HL mode */
  if ( rfgsm_core_get_band_sawless_status(core_data->rfm_dev, band) == TRUE )
  {
    rfgsm_core_handle_ptr->rfgsm_nv_hl_rx_switch_ptr = rfgsm_nv_get_hl_rx_freq_comp_switch_ptr(core_data->rfm_dev, band);
    RF_NULL_CHECK_RTN_V( rfgsm_core_handle_ptr->rfgsm_nv_hl_rx_switch_ptr, "rfgsm_core_set_band NULL rfgsm_nv_hl_rx_switch_ptr!" );
  }

  /* Initialize pointer to GSM/EGSM rx enh switch structure */
  rfgsm_core_handle_ptr->rfgsm_nv_enh_rx_switch_ptr = rfgsm_nv_get_enh_rx_switch_ptr(core_data->rfm_dev, band);
  RF_NULL_CHECK_RTN_V( rfgsm_core_handle_ptr->rfgsm_nv_enh_rx_switch_ptr, "rfgsm_core_set_band NULL rfgsm_nv_enh_rx_switch_ptr!" );



  rfgsm_core_handle_ptr->band = band;
} /* end of rfgsm_set_band */


/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details

   @param path

   @retval

*/
void rfgsm_core_turn_on_vregs(rf_path_enum_type path, uint8 sub_id, rfm_path_state rf_path_state)
{
  uint32 vreg_vote_count = 0;

#ifndef T_RUMI_EMULATION
  rfc_gsm_manage_vregs(path,RFCOM_BAND_GSM900, rf_path_state, sub_id);

  rfc_set_quiet_mode(RFM_EGSM_MODE, TRUE);

  /* Internal vreg voting is required to track number of votes on a path per */
  /* subscription. This is for when a subscription can be active on 2 WTRs,  */
  /* and separate RFM devices, at the same time but those devices map to the */
  /* same path (rfcommon_core_device_to_path). Because the devices map to the*/
  /* same path they map to the same common vreg and mcpm vote. This means    */
  /* that if a vreg off is called for one of the devices the vote for both   */
  /* would be lost. Vreg voting mechanism is to prevent this from happening. */
  /* The use case for this is WTR hopping in dedicated mode which requires   */
  /* the same subscription to enable the second WTR whilst in traffic and    */
  /* transfer the traffic to the new WTR.                                    */
#endif

  /* Increment the vreg vote */
  RFGSM_CORE_VREG_VOTE_INC_RET(path, sub_id, rf_path_state, vreg_vote_count);

  /* If the vote count exceeds RF_GSM_MAX_VREG_VOTE_COUNT there is an error.*/
  /* The error is where more vreg-on have been called without corresponding */
  /* vreg-off pair.                                                         */
  if(vreg_vote_count > RF_GSM_MAX_VREG_VOTE_COUNT)
  {
    if(rf_path_state == RF_PATH_RX_STATE)
    {
      ERR_FATAL ("RF GSM Vreg on count %d invalid for sub %d, path %d", vreg_vote_count, sub_id, path);
    }
    else
    {
      RF_MSG_4( RF_HIGH, "RF GSM Vreg on count %d for sub %d, path %d, rf_state %d", 
                vreg_vote_count, sub_id, path, rf_path_state);

    }
  }

  RF_MSG_4(MSG_LEGACY_HIGH,"RF GSM Vreg on for sub %d, path %d, vote count %d, rf_state %d ",
  	                        sub_id, path, vreg_vote_count, rf_path_state);
}

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details

   @param path :

   @retval

*/
void rfgsm_core_turn_off_vregs(rf_path_enum_type path, uint8 sub_id)
{
  /* Initialise local vreg vote count variable */
  uint8 vreg_vote_count = 0;

  /* For RF GSM vreg voting explanation see rfgsm_core_turn_on_vregs.*/
  /* Decrement the vreg vote for this subscription and path */
  RFGSM_CORE_VREG_VOTE_DEC_RET(path, sub_id, RF_PATH_RX_STATE, vreg_vote_count);

  /* If the sub and path already have a vote no need to disable */
  if(vreg_vote_count > 0)
  {
    RF_MSG_4(MSG_LEGACY_ERROR,
             "RF GSM Vreg off intentionally not executed for sub %d, path %d, vote count %d, rf_state %d",
             sub_id, path, vreg_vote_count, RF_PATH_RX_STATE);
    return;
  }

  RF_MSG_4(MSG_LEGACY_HIGH,"RF GSM Vreg off for sub %d, path %d, vote count %d, rf_state %d",
  	                        sub_id, path, vreg_vote_count, RF_PATH_RX_STATE);

#ifndef T_RUMI_EMULATION
  if (!rf_disable_powercollapse)
  {
    rfc_gsm_manage_vregs(path,RFCOM_BAND_GSM900, RF_PATH_SLEEP_STATE, sub_id);

    rfc_set_quiet_mode(RFM_EGSM_MODE, FALSE);
  }
#endif
}



/*----------------------------------------------------------------------------*/
/*!
   @fn rfgsm_core_populate_burst_rflm_data_info
   @brief This function populates the RFLM data info for Rx or Tx depending on
          burst paramter
   @param  rfm_dev - RFM device processing for
   @param  burst - Rx, Mon, Tx
   @param  num_tx_slots - number of Tx slots to process for within the frame
   @param  burst_event_ptr - pointer to GFW shared memory
   @retval boolean - success/failure indication
*/
boolean rfgsm_core_populate_burst_rflm_data_info( rfgsm_core_data_type *core_data,
                                                  rf_burst_type burst,
                                                  uint8 num_tx_slots,
                                                  void *burst_event_ptr )
{
  /* Check the GFW shared memory pointer */
  RF_NULL_CHECK_RTN(burst_event_ptr, "Invalid RF GSM core burst_event_ptr, pointer is NULL", FALSE);

  /* Choose which RFLM data info population API to call based on burst type */
  if ( burst == RF_TX_BURST )
  {
    if( !rfgsm_core_dm_populate_tx_burst_intf_data( core_data, burst_event_ptr, num_tx_slots ) )
    {
      RF_MSG(MSG_LEGACY_ERROR,"RF GSM Tx burst RFLM data info update failed!");
    }
  }
  else if ( burst == RF_RX_BURST || burst == RF_MON_BURST || burst == RF_PWR_BURST )
  {
    if( !rfgsm_core_dm_populate_rx_burst_intf_data( core_data, burst_event_ptr ) )
    {
      RF_MSG(MSG_LEGACY_ERROR,"RF GSM Rx burst RFLM data info update failed!");
    }
  }
  else
  {
    RF_MSG(MSG_LEGACY_ERROR,"RF GSM invalid core burst type, RFLM data info not populated");
    return FALSE;
  }

  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details

   @param

   @retval

*/
uint16 rfgsm_map_arfcn_linearly(rfcom_gsm_band_type band, uint16 arfcn)
{
    uint16 chan;

    if (band == RFCOM_BAND_GSM900)
    {
      /* Map the channel linearly */
      if( (arfcn >= 975) && (arfcn < 1024) )
      {
        chan = arfcn -975;
      }
      else /* arfcn <= 124 */
      {
        chan = arfcn + 49;
      }
    }
    else
    {
      chan = arfcn;
    }

  return( chan );
}



/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function is used to initialize the NV variables by reading the NV memory

   @details
   This function is useful to initialize the NV tables, card structure and also
   calculate and store the GSM Tx and Rx switchpoints.

   @param

   @retval

*/
void rfgsm_core_nv_init(rfm_device_enum_type rfm_dev)
{
  /* Precompute the receiver switch points as
  ** a runtime optimization to the rf_set_rx_gain()
  ** function.
  */
  rfgsm_nv_precompute_rx_switch_points(rfm_dev);
  rfgsm_nv_precompute_enh_rx_switch_points(rfm_dev);

  if ( TRUE == rfgsm_core_get_device_tx_capability(rfm_dev))
  {
    /*precompute the tx gain ranges based on NV switchpoints*/
    rfgsm_nv_precompute_tx_gain_ranges_info(rfm_dev);

    rfgsm_nv_init_amam_ampm_table( rfm_dev, RFCOM_BAND_GSM850 );
    rfgsm_nv_init_amam_ampm_table( rfm_dev, RFCOM_BAND_GSM900 );
    rfgsm_nv_init_amam_ampm_table( rfm_dev, RFCOM_BAND_GSM1800 );
    rfgsm_nv_init_amam_ampm_table( rfm_dev, RFCOM_BAND_GSM1900 );
  }

}



/*===========================================================================*/
/*! @fn rfgsm_core_exit_mode                                                 */
/*! @brief Disable and initialise for exiting GSM mode                       */
/*! @details This function is a full exit of GSM                             */
/*!          If current subscription is the only active on this device then  */
/*!          perform a full exit, otherwise another subscription is active   */
/*!          therefore performing only a minimised exit of freeing DM memory */
/*! @param core_data - data for dev, sub, task, core handle etc              */
/*! @retval boolean success status                                           */
boolean rfgsm_core_exit_mode(rfgsm_core_data_type *core_data, rfcom_mode_enum_type to_mode)
{
  /* Initialise variables */
  rf_path_enum_type rf_path;
  boolean dev_is_prx = TRUE;

  rfm_device_enum_type rfm_dev = core_data->rfm_dev;
  uint8 sub_id = core_data->sub_id;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = core_data->rfgsm_core_handle_ptr;

  /* Get logical path and if device is PRx */
  rf_path = rfcommon_core_device_to_path(rfm_dev);

  /*If the device is Primary RX device*/
  dev_is_prx = rfgsm_core_is_device_prx(rfm_dev);

  /* Free the IQ buffer as its no longer required */
  if( rfgsm_core_handle_ptr->iq_capture != NULL )
  {
    modem_mem_free( rfgsm_core_handle_ptr->iq_capture, MODEM_MEM_CLIENT_RFA );
    rfgsm_core_handle_ptr->iq_capture = NULL;
  }

  /* Free the RFLM memory for this subscription */
  if(!rfgsm_core_dm_free_data_manager_mem(sub_id))
  {
    RF_MSG_2(MSG_LEGACY_ERROR,
             "RF GSM core DM free RFLM mem failed for sub %d, dev %d!",
             sub_id, rfm_dev );
    return FALSE;
  }

  /* If there is another subscription active on the device then return   */
  /* As another sub is active on the same device do not tear down device */
  if( !RFGSM_CORE_ONLY_ACTIVE_SUB(rfgsm_core_handle_ptr->sub_id_mask, sub_id) )
  {
    /* turn off the vregs required by GSM */
    /* Note: Vregs are required by other sub and will not shut down but voting off is required */
    rfgsm_core_turn_off_vregs(rf_path, sub_id);

    RF_MSG_3(MSG_LEGACY_HIGH,
             "RF GSM active sub on dev %d, mask 0x%x, minimised exit performed for sub %d!",
             rfm_dev, rfgsm_core_handle_ptr->sub_id_mask, sub_id);
    RFGSM_CORE_SET_SUB_INACTIVE(rfgsm_core_handle_ptr->sub_id_mask, sub_id);

    return TRUE;
  }

  /* Stop temperature compensation and cleanup thermistor read */
  if(core_data->device_is_tx)
  {
    /* Stop PA Temp pwr compensation. */
    rfgsm_core_temp_comp_stop(rfm_dev);

    /*Clean up the last therm read event handle before exit*/
    rfgsm_core_therm_read_event_cleanup(rfm_dev);
  }

  /* going out of GSM mode; set the core variables into default state; we need
     to do this specially when we go to WCDMA mode start measuring GSM Ncell. */
  rfgsm_core_handle_init(rfm_dev);

#ifndef T_RUMI_EMULATION
  /* Disable Vregs, CCS cmd interface TQ */
  if( !rfgsm_core_enable_disable_hw(FALSE, core_data, dev_is_prx, rf_path) )
  {
    RF_MSG(MSG_LEGACY_ERROR, "Exit mode rfgsm_core_enable_disable_hw failed!");
    return FALSE;
  }
#endif

  /* Subscription now inactive on device - clear sub mask for this sub */
  RFGSM_CORE_SET_SUB_INACTIVE(rfgsm_core_handle_ptr->sub_id_mask, sub_id);

  return TRUE;
}



/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details

   @param

   @retval

*/
void rfgsm_core_get_timing_info(rfgsm_core_data_type *core_data, rfgsm_timing_info_type *timing_info, boolean hmsc_mode)
{
  uint16 num_gb = 0;
  uint16 num_pll_sbis = 0;
  rfc_gsm_core_config_type *rfc_gsm_core_ptr;

  RF_NULL_CHECK_RTN_V( timing_info,"rfgsm_core_get_timing_info(): timing_info==NULL");

  rfc_gsm_core_ptr = core_data->rfgsm_core_handle_ptr->rfc_gsm_core_ptr;

  RF_NULL_CHECK_RTN_V( rfc_gsm_core_ptr, "rfgsm_core_get_timing_info(): rfc_gsm_ptr==NULL");

  timing_info->mon_rf_setup_time_qs = RFGSM_RF_RX_SETUP_TIME_QS(rfc_gsm_core_ptr->rx_mon_timing->rx_burst_sbis_before_pll,
                                                                rfc_gsm_core_ptr->rx_mon_timing->pll_settling_time_qs);
  timing_info->mon_rf_setup_time_qs += RFLM_GSM_RX_MAX_PROC_OVERHEAD_QS;

  timing_info->rx_rf_setup_time_qs = RFGSM_RF_RX_SETUP_TIME_QS(rfc_gsm_core_ptr->rx_timing->rx_burst_sbis_before_pll,
                                                               rfc_gsm_core_ptr->rx_timing->pll_settling_time_qs);
  timing_info->rx_rf_setup_time_qs += RFLM_GSM_RX_MAX_PROC_OVERHEAD_QS;

  /* The overhead is added to accomodate worst case delay for RX burst writes due to rffe bus contention.
     This RX alpha is used for IDLE mode RX by GL1 in DR-DSDS */
  timing_info->rx_idle_rf_setup_time_qs = timing_info->rx_rf_setup_time_qs + RFGSM_WORST_CASE_RFFE_BUS_DELAY;

  num_gb = rfc_gsm_core_ptr->tx_timing->kv_length;
  num_pll_sbis = rfc_gsm_core_ptr->tx_timing->num_pll_sbis;

  /*  Tx RF setup time is negative of RF_GSM_TX_SBI_START_DELTA_QS  */
  timing_info->tx_rf_setup_time_qs = RFGSM_RF_TX_SETUP_TIME_QS(num_gb,num_pll_sbis);

  /* The rf warmup time is now communicated back to GL1 in the timing info structure */
  timing_info->rf_warmup_time_qs = rfc_gsm_core_ptr->rf_warmup_time;

  /*ip2 cal alpha time*/
  timing_info->ip2_cal_setup_time_qs = core_data->rfgsm_core_handle_ptr->ip2_cal_alpha;

}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   qxdm f3 messages for the rf tx related information.

   @details
   qxdm f3 messages for the rf tx related information.

   @param None.

   @retval None

*/
void rfgsm_core_tx_log(rfm_device_enum_type rfm_dev, rf_task_num_type rf_task)
{
  uint8 i = 0;

  rfgsm_core_handle_type *rfgsm_core_handle_ptr;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!");

  /* band, chan, num_slots info. */
  RFGC_L_MSG_4( MSG_LEGACY_HIGH,"tx_band = %d, tx_arfcn = %d num_slots = %d sar_state = %d",
        rfgsm_core_handle_ptr->tx_log.tx_band, rfgsm_core_handle_ptr->tx_log.tx_chan, rfgsm_core_handle_ptr->tx_log.num_slots, rfgsm_core_handle_ptr->tx_log.sar_state);

  /* vbatt and therm info. */
  RFGC_L_MSG_3( MSG_LEGACY_HIGH,"vbatt_mv = %d, therm_value = %d, temp_remainder = %d",
                rfgsm_core_handle_ptr->tx_log.vbatt_mv, rfgsm_core_handle_ptr->tx_log.therm_read, rfgsm_core_handle_ptr->tx_log.temp_remainder);

  for (i=0; i < rfgsm_core_handle_ptr->tx_log.num_slots; i++)
  {
    RFGC_L_MSG_5( MSG_LEGACY_HIGH, "pwr_index=%d, pwr=%d, pwr_offset=%d, rgi=%d, apt_mode=%d",
          rfgsm_core_handle_ptr->tx_log.tx_slot_log[i].power_index, rfgsm_core_handle_ptr->tx_log.tx_slot_log[i].pwr_in_dbm,
          rfgsm_core_handle_ptr->tx_log.tx_slot_log[i].tx_pwr_offset, rfgsm_core_handle_ptr->tx_log.tx_slot_log[i].rgi,
          rfgsm_core_handle_ptr->tx_log.tx_slot_log[i].apt_mode);

    RFGC_L_MSG_7( MSG_LEGACY_HIGH, "pre_dist=%d, pa_scale=%d, env_gain=%d, pa_range=%d, mod_type=%d, icq = %d, smps = %d",
          rfgsm_core_handle_ptr->tx_log.tx_slot_log[i].pre_dist_flag, rfgsm_core_handle_ptr->tx_log.tx_slot_log[i].pa_scale,
          rfgsm_core_handle_ptr->tx_log.tx_slot_log[i].env_gain, rfgsm_core_handle_ptr->tx_log.tx_slot_log[i].pa_range,
          rfgsm_core_handle_ptr->tx_log.tx_slot_log[i].mod_type, rfgsm_core_handle_ptr->tx_log.tx_slot_log[i].icq_bias,
          rfgsm_core_handle_ptr->tx_log.tx_slot_log[i].smps_pdm );
  }

}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function get called from RFAPP task, which will be get queued for callback every GSTMR frame.

   @details
   This function get called from RFAPP task, which will be get queued for callback every GSTMR frame.

   @param
   debug_msg_log_cmd: argument passed by the RFAPP task.

   @retval None.

*/
void rfgsm_core_msg_log_handler(rf_apps_cmd_type *debug_msg_log_cmd)
{
  rfm_device_enum_type rfm_dev = RFM_INVALID_DEVICE;
  rf_task_num_type  rf_task = RF_TASK_APPS;
  int32 cmd_buff_index = 0;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr;

  rfm_dev = (rfm_device_enum_type) debug_msg_log_cmd->payload[0];
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!");

  // extract the log_index from the payload
  cmd_buff_index = (debug_msg_log_cmd->payload[1] |
                    (debug_msg_log_cmd->payload[2] << 8));

  /* Tx information */
  if (rfgsm_core_handle_ptr->tx_log.log_flag)
  {
    rf_task = rfgsm_core_handle_ptr->cmd_buffs.entry[cmd_buff_index].rf_task;
    rfgsm_core_tx_log(rfm_dev, rf_task);
    rfgsm_core_handle_ptr->tx_log.log_flag = FALSE;
  }
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This queue the callback function for the RFAPP task.

   @details
   This queue the callback function for the RFAPP task.

   @param None

   @retval None

*/
void rfgsm_core_log(rfm_device_enum_type rfm_dev)
{
  boolean debug_log_flag = FALSE;
  int32 log_index = 0;
  int32 cmd_buff_log_index = 0;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr;

  rf_apps_cmd_type *debug_msg_log_cmd = NULL;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!");

  if (rfgsm_core_handle_ptr->tx_log.log_flag)
  {
    debug_log_flag = TRUE;
  }

  if (debug_log_flag)
  {
    /* Get the pointer to the free buffer of RF command queue. */
    if ( ( debug_msg_log_cmd = rf_apps_get_buf() ) == NULL )
    {
      RF_MSG( RF_ERROR, "rfgsm_core_log: RF command queue is full!" );
      return;
    }

    debug_msg_log_cmd->msgr_hdr.id = RFGSM_CORE_APPS_TASK_MSG_LOG_CMD;
    debug_msg_log_cmd->rex_hdr.cmd_hdr.tech_id = RF_APPS_GSM_ID;
    debug_msg_log_cmd->payload[0] = (uint8)rfm_dev;
    debug_msg_log_cmd->payload[1] = (uint8)(log_index & 0xFF);
    debug_msg_log_cmd->payload[2] = (uint8)((log_index >> 8) & 0xFF);

    /* place RFGSM_CORE_APPS_TASK_MSG_LOG_CMD to RF APP Task command queue */
    rf_apps_cmd_put( debug_msg_log_cmd );
  }
}

/*----------------------------------------------------------------------------*/
boolean rfgsm_core_perform_debug = FALSE; // by default don't call debug function
uint8 bs_d = 0;
extern boolean (*ftm_debug_funct)(uint8);

/*----------------------------------------------------------------------------*/
/*
  @brief
  Do processing required in a GSM idle frame.
  Currently only temp_comp is being done here.

  @details

  @param
  None

*/
void rfgsm_core_do_idle_frame_processing(rfgsm_core_data_type *core_data)
{
  /* Every idle frame send the vbatt read request*/
  if(core_data->rfgsm_core_handle_ptr->vbatt_comp_info.vbatt_adc_client_init_status)
  {
    rfgsm_core_vbatt_read_trigger(0, core_data->rfm_dev, FALSE);
  }

  if ( core_data->rfgsm_core_handle_ptr->vbatt_comp_info.vbatt_comp_started_flag &&
       core_data->rfgsm_core_handle_ptr->vbatt_comp_info.vbatt_adc_client_init_status )
  {

    //Clear the vbatt comp flag
    core_data->rfgsm_core_handle_ptr->vbatt_comp_info.vbatt_comp_started_flag = FALSE;

    rfgsm_core_vbatt_comp_trigger(core_data->rfm_dev);
  }
}

/*----------------------------------------------------------------------------*/
/*
  @brief
  This function copies the IQ samples pointed to by GL1 structure pointer, to a local static buffer 'rfgsm_iq_buffer'.

  @details
  There are three types of IQ capture. One type is single shot capture, in which the IQ samples are copied only till 'rfgsm_iq_buffer' is full.
  The other way is continous capture in which the 'rfgsm_iq_buffer' is overwritten from the first element when it is full.
  The third one is 'STOP_CAPTURE' during which IQ samples are not captured.

  @param
  GL1 interface -> GfwIqSamplesBuffer*

*/
void rfgsm_core_log_iq_data(rfm_device_enum_type rfm_dev, GfwIqSamplesBuffer* data)
{
  int32 slot_index = 0;
  int32 buffer_left = 0;
  uint32 numOSR2IQSamples = 0;
  int16* rfgsm_iq_buffer = NULL;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr;
  ftm_iqcapture_type iq_cap_type;

  rfgsm_core_iq_capture_type * iq_buffer_ptr = NULL;

  rfgsm_iq_buffer= rfgsm_core_get_iq_buffer_ptr(rfm_dev);
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  if ( (rfgsm_core_handle_ptr == NULL) || (rfgsm_iq_buffer == NULL) || ( data == NULL ) )// stop iq_capture
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR, "NULL data for device %d", rfm_dev);
    return;
  }

  iq_buffer_ptr = rfgsm_core_handle_ptr->iq_capture;

  if( iq_buffer_ptr == NULL )
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR, "IQ Buffer for device %d is NULL", rfm_dev);
    return;
  }

  iq_cap_type = iq_buffer_ptr->capture_type;

  if( iq_cap_type == FTM_IQ_CAPTURE_STOP )
  {
    /* Do nothing */
    return;
  }

  for( slot_index=0; slot_index < RFGSM_MAX_NUM_SLOTS; slot_index++ )
  {
    if(data[slot_index].numSamples  <= 0) // if samples to be written
    {
      continue;
    }
    numOSR2IQSamples = data[slot_index].numSamples * 4; // 2x for OSR2, 2x for I&Q

    buffer_left = RFGSM_MAX_IQ_BUFFER_LEN - iq_buffer_ptr->total_iq_samples_copied;

    /* if space left in buffer is more than the number of samples to be written */
    if (buffer_left >= numOSR2IQSamples)
    {
      memscpy( rfgsm_iq_buffer + iq_buffer_ptr->total_iq_samples_copied, sizeof(uint16)*buffer_left, data[slot_index].samples, sizeof(uint16)* numOSR2IQSamples );
      iq_buffer_ptr->total_iq_samples_copied += numOSR2IQSamples;
      if ( (iq_buffer_ptr->total_iq_samples_copied == RFGSM_MAX_IQ_BUFFER_LEN ) && (iq_cap_type == FTM_IQ_CAPTURE_CONTINOUS) )
      {
        iq_buffer_ptr->total_iq_samples_copied = 0;
      }

    }
    /* if space left in buffer is less than the number of samples to be written */
    else
    {
      //Fill the buffer with however many samples that can be fit
      memscpy(rfgsm_iq_buffer + iq_buffer_ptr->total_iq_samples_copied, sizeof(uint16)*buffer_left, data[slot_index].samples, sizeof(uint16)*buffer_left );

      if(iq_cap_type == FTM_IQ_CAPTURE_CONTINOUS)
      {
        //Overwrite the buffer from start for surplus samples
        memscpy(rfgsm_iq_buffer, sizeof(uint16)*RFGSM_MAX_IQ_BUFFER_LEN, data[slot_index].samples + buffer_left, sizeof(uint16)*(numOSR2IQSamples - buffer_left));
        iq_buffer_ptr->total_iq_samples_copied = numOSR2IQSamples - buffer_left;
      }
      else
      {
        break; // to exit loop
      }
    }
  }
}


/*----------------------------------------------------------------------------*/
/*
  @brief
  This function returns the pointer to local IQ buffer.

  @details

  @param

*/
int16* rfgsm_core_get_iq_buffer_ptr(rfm_device_enum_type rfm_dev)
{

  rfgsm_core_handle_type *rfgsm_core_handle_ptr;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN( rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!", NULL );

  RF_NULL_CHECK_RTN( rfgsm_core_handle_ptr->iq_capture, "IQ capture buffer has not been allocated", NULL );

  return rfgsm_core_handle_ptr->iq_capture->rfgsm_iq_buffer;
}



/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function calls the rfgsm_mdsp function to allocate the iRAT measurement
  script buffers.

  This function must be called from GL1 for iRAT scenarios.
*/
void rfgsm_core_allocate_rx_script_buffers( rfgsm_core_data_type *core_data )
{
  rfgsm_mdsp_allocate_rx_script_buffers(core_data->rfgsm_core_handle_ptr);

  /* Allocate DM buffer for X2G IRAT burst scripts */
  if( !rfgsm_core_dm_alloc_data_manager_mem(core_data->sub_id) )
  {
    RF_MSG(MSG_LEGACY_ERROR, "RF GSM core DM data table alloc failed!");
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function calls the rfgsm_mdsp function to deallocate the iRAT measurement
  script buffers.

  This function must be called from GL1 for iRAT scenarios.
*/
void rfgsm_core_free_rx_script_buffers( rfgsm_core_data_type *core_data )
{
  rfgsm_mdsp_free_rx_script_buffers(core_data->rfgsm_core_handle_ptr);

  /* Deallocate DM buffer for X2G IRAT burst scripts */
  if(!rfgsm_core_dm_free_data_manager_mem(core_data->sub_id))
  {
    RF_MSG(MSG_LEGACY_ERROR, "RF GSM core DM free RFLM mem failed!");
  }
}


/*----------------------------------------------------------------------------*/
void rfgsm_core_debug(rfm_device_enum_type rfm_dev)
{
    if( ftm_debug_funct != NULL )
    {

      if ( (*ftm_debug_funct)(bs_d) ) // if capture finished
      {
        rfgsm_core_perform_debug = FALSE;  // no more debug
        MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"capture finished",0);

        ftm_debug_funct = NULL;
      }
      else
      {
        rfgsm_core_perform_debug = TRUE;
        MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"capture not finished",0);
      }
    }
    else
    {
      rfgsm_core_perform_debug = FALSE;
      MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"Debug is NULL",0);
    }
}



/*----------------------------------------------------------------------------*/
/*
  @brief
  This API initialises the core handle variables that need to be initialise on
  init of GSM. This includes rfgsm_core_init, wakeup and exit

  @details
  1. The initialisation of the structure members is performed in the order in which
  they appear in the structure handle definition.

  2. There is an init once only section in which all the contained members will only
  be initialised once and need to be maintained. This is as opposed to the other
  members that will be re-initialised on wakeup and exit.

  @params
  rfm_device_enum_type rfm_dev    device type determining the device being used

  @retval
  boolean status                  determining the success of initialisation
*/
boolean rfgsm_core_data_init(rfm_device_enum_type rfm_dev)
{
  uint16 i = 0;
  uint16 j = 0;
  uint8 flag_ind;
  timer_attrib_struct_type rfgsm_core_timer_attrib;

  rfgsm_core_handle_type *rfgsm_core_handle_ptr;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!", FALSE);

  /*************************************************************************/
  /**************************** INIT ONCE VARIABLES ************************/
  flag_ind = 0;
  if ( rfgsm_core_handle_ptr->init_once == FALSE )
  {
    rfgsm_core_handle_ptr->init_once = TRUE;
    flag_ind = 1;

      /*By default unfiltered path will be used*/
    rfgsm_core_handle_ptr->use_coex_filtered_path_tx = FALSE;
    rfgsm_core_handle_ptr->use_coex_filtered_path_rx = FALSE;
    rfgsm_core_handle_ptr->coex_filtered_path_cal_flag = FALSE;
    rfgsm_core_handle_ptr->rfgsm_mode = RFGSM_DEFAULT_MODE;
    rfgsm_core_handle_ptr->rfgsm_buffer = RFGSM_DEFAULT_BUFFER;

    rfgsm_core_handle_ptr->scripts.enter_mode_script = NULL;
    rfgsm_core_handle_ptr->scripts.tx_band_script = NULL;
    rfgsm_core_handle_ptr->scripts.therm_read_script = NULL;
    for (i=0; i < RFGSM_MAX_TX_SLOTS_PER_FRAME; i++)
    {
      /*New transition buffer support for BOLT*/
      rfgsm_core_handle_ptr->scripts.tx_trans_script[TX_TRANS_WTR_SCRIPT][i] =NULL;
      rfgsm_core_handle_ptr->scripts.tx_trans_script[TX_TRANS_FE_SCRIPT][i] =NULL;
      rfgsm_core_handle_ptr->scripts.apt_script[i] = NULL;
    }
    for (i=0; i < RFGSM_DEVICE_MAX_TX_SLOTS; i++)
    {
       rfgsm_core_handle_ptr->scripts.tx_agc_trigger_gmsk_script[i] = NULL;
       rfgsm_core_handle_ptr->scripts.tx_agc_trigger_8psk_script[i] = NULL;
    }

    /*New Buffer support for bolt*/
    for (i=0; i < TX_MAX_SCRIPT; i++)
    {
      rfgsm_core_handle_ptr->scripts.tx_start_script[i] = NULL;
      rfgsm_core_handle_ptr->scripts.tx_stop_script[i] = NULL;
    }
    for (i=0; i < RX_MAX_SCRIPT; i++)
    {
      rfgsm_core_handle_ptr->scripts.rx_start_script[i] = NULL;
      rfgsm_core_handle_ptr->scripts.rx_stop_script[i] = NULL;
    }

    /* One-time initialisation of the time profile data */
    memset(&rfgsm_core_handle_ptr->cmd_buffs, 0, sizeof(rfgsm_cmd_buffer_type));

    /* vbatt_comp timer init */
    timer_get_params(&rfgsm_core_handle_ptr->rfgsm_core_vbatt_comp_clk_cb,
                     &rfgsm_core_timer_attrib);
    rfgsm_core_timer_attrib.tcb_sigs_obj = NULL;
    timer_set_params(&rfgsm_core_handle_ptr->rfgsm_core_vbatt_comp_clk_cb,
                     TIMER_UPDATE_NOTIFICATION_PARAMS,
                     &rfgsm_core_timer_attrib);

    /* temp_comp timer init */
    timer_get_params(&rfgsm_core_handle_ptr->rfgsm_core_temp_comp_clk_cb,
                     &rfgsm_core_timer_attrib);
    rfgsm_core_timer_attrib.tcb_sigs_obj = NULL;
    timer_set_params(&rfgsm_core_handle_ptr->rfgsm_core_temp_comp_clk_cb,
                     TIMER_UPDATE_NOTIFICATION_PARAMS,
                     &rfgsm_core_timer_attrib);

    /* asdiv_rsp_check timer init  */
    timer_get_params(&rfgsm_core_handle_ptr->rfgsm_core_asd_rsp_check_cb,
                     &rfgsm_core_timer_attrib);
    rfgsm_core_timer_attrib.tcb_sigs_obj = NULL;
    timer_set_params(&rfgsm_core_handle_ptr->rfgsm_core_asd_rsp_check_cb,
                     TIMER_UPDATE_NOTIFICATION_PARAMS,
                     &rfgsm_core_timer_attrib);

    rfgsm_core_handle_ptr->band_before_sleep = RFCOM_NUM_GSM_BANDS;

    rfgsm_core_handle_ptr->rxlm_buf_index = RFGSM_RXLM_BUF_IDX_INVALID;

    rfgsm_core_handle_ptr->apt_mode = INVALID_MODE;

    rfgsm_core_handle_ptr->rxlm_buf_index_ip2_cal = RFGSM_RXLM_BUF_IDX_INVALID;

    rfgsm_core_handle_ptr->txlm_buf_index_ip2_cal = RFGSM_RXLM_BUF_IDX_INVALID;

    rfgsm_core_handle_ptr->ip2_cal_alpha = 0xFFFFFFFF;

    /* Memset sawless support array to 0 (FALSE) */
    memset(rfgsm_core_handle_ptr->sawless_support, 0, sizeof(rfgsm_core_handle_ptr->sawless_support));

    rfgsm_core_handle_ptr->ssbi_protocol_supported =  FALSE;

    /*Initialize headroom table only once*/
    for (i=0;i<RFCOM_NUM_GSM_BANDS;i++)
    {
      rfgsm_core_handle_ptr->rfgsm_core_bb_headroom_tbl[i] = 0;
    }
#ifdef FEATURE_RF_ASDIV
    rfgsm_core_handle_ptr->set_antenna_position = (uint8)RF_INVALID_VALUE_8BIT;
    rfgsm_core_handle_ptr->configure_asd_scripts = FALSE;
    rfgsm_core_handle_ptr->asd_rsp = FALSE;
    rfgsm_core_handle_ptr->asd_fw_rsp_umid = 0;
    rfgsm_core_handle_ptr->asd_sub_id  = RFLM_GSM_DEFAULT_SUBSCRIPTION;
    rfgsm_core_handle_ptr->asd_l1_cb_func = NULL;
    rfgsm_core_handle_ptr->probe_burst = RFGSM_MAX_RX_BURST_TYPES;
#endif // FEATURE_RF_ASDIV

    /*Update this variable if WTR supports five LNA gain ranges*/
    rfgsm_core_handle_ptr->rf_has_five_gainranges = FALSE;

    /* Initialise the subscription ID mask all Fs to indicate free */
    rfgsm_core_handle_ptr->sub_id_mask = 0;
    rfgsm_core_handle_ptr->no_active_subs = TRUE;

    /*Initialise the device capability variables to FALSE*/
    rfgsm_core_handle_ptr->device_capability.tx_supported_band_mask = 0;
    rfgsm_core_handle_ptr->device_capability.rx_supported_band_mask = 0;


    /* Initialise HL table ptr to NULL and override flag to FALSE */
    if ( rfgsm_debug_flags.sawless_override == TRUE )
    {
      /* Set to MAX so that no linearity setting will occur in Rx burst */
      rfgsm_core_handle_ptr->sawless_linearity_control.linearity_override = RFGSM_SAWLESS_OVERRIDE_MAX_VAL;
    }
    else
    {
      rfgsm_core_handle_ptr->sawless_linearity_control.linearity_override = RFGSM_SAWLESS_NO_OVERRIDE;
    }

    /* We have the option to apply a linearity mode override based on debug NV */
    if( rfgsm_nv_check_debug_nv(rfm_dev, RFGSM_DEBUG_NV_SAWLESS_FORCE_MODE) == TRUE )
    {
      if ( rfgsm_nv_check_debug_nv(rfm_dev, RFGSM_DEBUG_NV_SAWLESS_MODE_FORCE_HIGH) == TRUE )
      {
        rfgsm_core_handle_ptr->sawless_linearity_control.linearity_override = RFGSM_SAWLESS_HL_OVERRIDE;
      }
      else
      {
        rfgsm_core_handle_ptr->sawless_linearity_control.linearity_override = RFGSM_SAWLESS_LL_OVERRIDE;
      }
    }

    rfgsm_core_handle_ptr->sawless_linearity_control.rx_hl_flag_table = NULL;

    /* These should be obtained via device calls to account for algorithm changes between SAWless chips */
    rfgsm_core_handle_ptr->sawless_linearity_control.linearity_switching_thresholds.jdet_high_thresh = 0x1F40; //1000 mv * 8 in Hex
    rfgsm_core_handle_ptr->sawless_linearity_control.linearity_switching_thresholds.jdet_med_thresh = 0xC80; //400 mV * 8 in Hex
    rfgsm_core_handle_ptr->sawless_linearity_control.linearity_switching_thresholds.rssi_less_snr_thresh_idle = -1680; //-105 in dbx16
    rfgsm_core_handle_ptr->sawless_linearity_control.linearity_switching_thresholds.rssi_thresh_pscan = -1584; //-99 in dbx16
    rfgsm_core_handle_ptr->sawless_linearity_control.linearity_switching_thresholds.snr_thresh = 13; // ~13 dB as power ratio

    rfgsm_core_handle_ptr->sawless_linearity_control.enable_logging = rfgsm_nv_check_debug_nv(rfm_dev, RFGSM_DEBUG_NV_SAWLESS_LOGGING_ENABLE);
    rfgsm_core_handle_ptr->sawless_linearity_control.ignore_jdet_in_algorithm = rfgsm_nv_check_debug_nv(rfm_dev, RFGSM_DEBUG_NV_SAWLESS_ALGORITHM_IGNORE_JDET);
    rfgsm_core_handle_ptr->sawless_linearity_control.disable_hl_power_scan = rfgsm_nv_check_debug_nv(rfm_dev, RFGSM_DEBUG_NV_SAWLESS_DISABLE_HL_POWER_SCAN);
    rfgsm_core_handle_ptr->sawless_linearity_control.disable_jdet_configuration = rfgsm_nv_check_debug_nv(rfm_dev, RFGSM_DEBUG_NV_SAWLESS_DISABLE_JDET);

    memset(rfgsm_core_handle_ptr->jdet_counter, 0, MAX_NUM_ARFCNS_IN_BAND * sizeof(uint8));
    rfgsm_core_handle_ptr->hl_ll_band = RFCOM_NUM_GSM_BANDS;

    /*Initializinf variables for device status read*/
    rfgsm_core_handle_ptr->device_config_to_read.event_tag = 0;
    rfgsm_core_handle_ptr->device_config_to_read.device_status_rsp_umid = 0;

    for(i=0; i < RFGSM_DEVICE_STATUS_MAX_BURST_TYPE; i++)
    {
      for (j=0; j < RFGSM_DEVICE_STATUS_READ_MAX_REGS; j++)
      {
        rfgsm_core_handle_ptr->device_config_to_read.params[i].address[j] = 0xFFFF;
        rfgsm_core_handle_ptr->device_config_to_read.params[i].slave_id[j] = 0xFF;
        rfgsm_core_handle_ptr->device_config_to_read.params[i].channel[j] = 0xFF;
        rfgsm_core_handle_ptr->device_config_to_read.params[i].half_rate[j] = 0xFF;
        rfgsm_core_handle_ptr->device_config_to_read.params[i].rd_delay[j] = 0xFF;
        rfgsm_core_handle_ptr->device_config_to_read.params[i].extended_cmd[j] = FALSE;
        rfgsm_core_handle_ptr->device_config_to_read.params[i].resource_type[j] = RF_HAL_BUS_RESOURCE_MAX;
      }
      rfgsm_core_handle_ptr->device_config_to_read.params[i].enable= FALSE;
      rfgsm_core_handle_ptr->device_config_to_read.params[i].num_regs = 0;
      rfgsm_core_handle_ptr->device_config_to_read.params[i].enable_grfc_read = FALSE;
    }

    /* This logging is enabled directly via Debug NV, so is set to default value
       during init_once because the default value will be set only one time i.e. during bootup */
    rfgsm_core_handle_ptr->rx_debug_logging_enable = FALSE;
  } // ^ rfgsm_core_handle_ptr->init_once == FALSE

  rfgsm_core_handle_ptr->edge_core = (uint8)RF_INVALID_VALUE_8BIT;


  /* IP2 cal related initialization*/
  rfgsm_core_handle_ptr->ip2_cal_params.ip2_cal_supported = FALSE;
  rfgsm_core_handle_ptr->ip2_cal_params.rfcom_band = RFCOM_NUM_GSM_BANDS;
  rfgsm_core_handle_ptr->ip2_cal_params.arfcn     = 0xFFFF;
  rfgsm_core_handle_ptr->ip2_cal_params.lna_range = 0;
  rfgsm_core_handle_ptr->ip2_cal_params.alg_params.num_cal_points = 0;
  rfgsm_core_handle_ptr->ip2_cal_params.alg_params.override_params = FALSE;
  rfgsm_core_handle_ptr->ip2_cal_params.alg_params.start_i_code = 0;
  rfgsm_core_handle_ptr->ip2_cal_params.alg_params.start_q_code = 0;
  rfgsm_core_handle_ptr->ip2_cal_params.alg_params.unit_step_size = 1;
  rfgsm_core_handle_ptr->ip2_cal_params.alg_params.width  = 1;
  rfgsm_core_handle_ptr->ip2_cal_params.tx_rgi = RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE;
  rfgsm_core_handle_ptr->ip2_cal_first_iter = TRUE;


  /*************************************************************************/
  /************* VARIABLES RESET ON INIT, WAKEUP & EXIT MODE ***************/
  rfgsm_core_handle_ptr->rfc_gsm_core_ptr =
            (rfc_gsm_core_config_type *)rfc_gsm_mode_config_get(rfm_dev, RFC_MODE_CORE_DATA);

  RF_NULL_CHECK_RTN( rfgsm_core_handle_ptr->rfc_gsm_core_ptr, "rfgsm_core_data_init(): rfc_gsm_ptr==NULL", FALSE);

  // Populate the spur channel list only during core init
  if (flag_ind == 1)
  {
    rfc_gsm_populate_spur_channel_list();
  }

  rfgsm_core_handle_ptr->band               = RFCOM_NUM_GSM_BANDS;
  rfgsm_core_handle_ptr->rx_band            = RFCOM_NUM_GSM_BANDS;
  rfgsm_core_handle_ptr->tx_band            = RFCOM_NUM_GSM_BANDS;
  rfgsm_core_handle_ptr->rf_tx_arfcn = (uint16) RF_INVALID_VALUE_16BIT;
  rfgsm_core_handle_ptr->rf_rx_arfcn = (uint16) RF_INVALID_VALUE_16BIT;

  rfgsm_core_handle_ptr->parange_override_val = (uint8)RF_INVALID_VALUE_8BIT;
  rfgsm_core_handle_ptr->rgi_override_val     = (uint16)RF_INVALID_VALUE_16BIT;
  rfgsm_core_handle_ptr->lna_range            = (uint8)RF_INVALID_VALUE_8BIT;

  rfgsm_core_handle_ptr->rf_pa_start_time           = 0;
  rfgsm_core_handle_ptr->rf_pa_stop_time            = 0;

  rfgsm_core_handle_ptr->tx_timing_adjust_flag      = FALSE;
  rfgsm_core_handle_ptr->tx_profile_update_required = TRUE;
  rfgsm_core_handle_ptr->amam_ampm_recalc_required  = TRUE;
  rfgsm_core_handle_ptr->slot_override_flag         = FALSE;
  rfgsm_core_handle_ptr->last_num_tx_slots          = 0;

  rfgsm_core_handle_ptr->rfgsm_curr_freq_error      = 0;
  rfgsm_core_handle_ptr->freq_error_flag_from_api   = TRUE;

  rfgsm_core_handle_ptr->hmsc_active = FALSE;

  rfgsm_core_handle_ptr->ftm_path_delay_cal_override = FALSE;
  rfgsm_core_handle_ptr->ftm_path_delay_override_val = (uint16) RF_INVALID_VALUE_16BIT;

  for(i=0;i<RFGSM_MAX_TX_SLOTS_PER_FRAME;i++)
  {
    rfgsm_core_handle_ptr->ftm_slot_override[i].slot_override_on  = FALSE;
    rfgsm_core_handle_ptr->ftm_slot_override[i].linear_rgi        = (uint16)RF_INVALID_VALUE_16BIT;
    rfgsm_core_handle_ptr->ftm_slot_override[i].pa_range          = (uint16)RF_INVALID_VALUE_16BIT;
    rfgsm_core_handle_ptr->ftm_slot_override[i].tx_pow_dbm        = (uint16)RF_INVALID_VALUE_16BIT;
    rfgsm_core_handle_ptr->ftm_slot_override[i].modulation        = RF_MOD_GMSK;

    rfgsm_core_handle_ptr->last_mod_type[i] = RF_MOD_UNKNOWN;
    rfgsm_core_handle_ptr->last_pwr_level[i] = (uint16)RF_INVALID_VALUE_16BIT;
    rfgsm_core_handle_ptr->last_desense_backoff[i] = 0;
    rfgsm_core_handle_ptr->last_vbatt_backoff[i] = 0;

    rfgsm_core_handle_ptr->rfc_gsmctl_linear_pa_tx_gain_buff[i].txCfg2GainSetupAmamTblIdx = (uint16)RF_INVALID_VALUE_16BIT;
    rfgsm_core_handle_ptr->rfc_gsmctl_linear_pa_tx_gain_buff[i].txCfg2GainSetupEnvGain = (uint16)RF_INVALID_VALUE_16BIT;
    rfgsm_core_handle_ptr->rfc_gsmctl_linear_pa_tx_gain_buff[i].txCfg2GainSetupPaScale = (uint16)RF_INVALID_VALUE_16BIT;
    rfgsm_core_handle_ptr->rfc_gsmctl_linear_pa_tx_gain_buff[i].txCfg2GainSetupPredist = (uint16)RF_INVALID_VALUE_16BIT;
    rfgsm_core_handle_ptr->rfc_gsmctl_linear_pa_tx_gain_buff[i].txPaSlotGrfcState[0] = (uint16)RF_INVALID_VALUE_16BIT;
    rfgsm_core_handle_ptr->rfc_gsmctl_linear_pa_tx_gain_buff[i].txPaSlotGrfcState[1] = (uint16)RF_INVALID_VALUE_16BIT;
    rfgsm_core_handle_ptr->rfc_gsmctl_linear_pa_tx_gain_buff[i].txPaSlotGrfcState[2] = (uint16)RF_INVALID_VALUE_16BIT;
    rfgsm_core_handle_ptr->rfc_gsmctl_linear_pa_tx_gain_buff[i].txPaSlotGrfcState[3] = (uint16)RF_INVALID_VALUE_16BIT;
    rfgsm_core_handle_ptr->rfc_gsmctl_linear_pa_tx_gain_buff[i].txSlotGainVal1 = (uint8)RF_INVALID_VALUE_8BIT;
    rfgsm_core_handle_ptr->rfc_gsmctl_linear_pa_tx_gain_buff[i].txSlotGainVal2 = (uint8)RF_INVALID_VALUE_8BIT;
    rfgsm_core_handle_ptr->rfc_gsmctl_linear_pa_tx_gain_buff[i].txPower = (int32)RF_INVALID_VALUE_32BIT;

    rfgsm_core_handle_ptr->simult_sar_limit_gmsk[i] = RFNV_GSM_SAR_BACK_OFF_LIMIT_DEFAULT;
    rfgsm_core_handle_ptr->simult_sar_limit_8psk[i] = RFNV_GSM_SAR_BACK_OFF_LIMIT_DEFAULT;
  } // ^ i=0;i<RFGSM_MAX_TX_SLOTS_PER_FRAME;i++

  for (i=0; i<RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE; i++)
  {
    rfgsm_core_handle_ptr->rfgsm_pmeas_g0[i] = (int32)RF_INVALID_VALUE_32BIT;
    rfgsm_core_handle_ptr->rfgsm_pmeas_g1[i] = (int32)RF_INVALID_VALUE_32BIT;
    rfgsm_core_handle_ptr->rfgsm_pmeas_g2[i] = (int32)RF_INVALID_VALUE_32BIT;
    rfgsm_core_handle_ptr->rfgsm_pmeas_g3[i] = (int32)RF_INVALID_VALUE_32BIT;
    rfgsm_core_handle_ptr->rfgsm_pmeas_g4[i] = (int32)RF_INVALID_VALUE_32BIT;
    rfgsm_core_handle_ptr->rfgsm_pmeas_g5[i] = (int32)RF_INVALID_VALUE_32BIT;
    rfgsm_core_handle_ptr->rfgsm_pmeas_g6[i] = (int32)RF_INVALID_VALUE_32BIT;
    rfgsm_core_handle_ptr->rfgsm_pmeas_g7[i] = (int32)RF_INVALID_VALUE_32BIT;
    rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g0[i] = (int32)RF_INVALID_VALUE_32BIT;
    rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g1[i] = (int32)RF_INVALID_VALUE_32BIT;
    rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g2[i] = (int32)RF_INVALID_VALUE_32BIT;
    rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g3[i] = (int32)RF_INVALID_VALUE_32BIT;
    rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g4[i] = (int32)RF_INVALID_VALUE_32BIT;
    rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g5[i] = (int32)RF_INVALID_VALUE_32BIT;
    rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g6[i] = (int32)RF_INVALID_VALUE_32BIT;
    rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g7[i] = (int32)RF_INVALID_VALUE_32BIT;
  } // ^ i=0; i<RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE; i++

  rfgsm_core_handle_ptr->rfgsm_enable_pcl_logging = rfgsm_nv_check_debug_nv(rfm_dev, RFGSM_DEBUG_NV_TX_PCL_CALC_LOGGING_ENABLE);

  rfgsm_core_handle_ptr->rf_tune_commanded = FALSE;

  rfgsm_core_handle_ptr->rfgsm_rx_nv_tbl = NULL;
  rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl = NULL;

  rfgsm_core_handle_ptr->rfgsm_nv_rx_freq_comp_switch_ptr = NULL;
  rfgsm_core_handle_ptr->rfgsm_nv_hl_rx_switch_ptr = NULL;

  rfgsm_core_handle_ptr->tx_log.ver_num           = 0;
  rfgsm_core_handle_ptr->tx_log.fn         = (uint32)RF_INVALID_VALUE_32BIT;
  rfgsm_core_handle_ptr->tx_log.tx_band    = RFCOM_NUM_GSM_BANDS;
  rfgsm_core_handle_ptr->tx_log.tx_chan    = (uint16)RF_INVALID_VALUE_16BIT;
  rfgsm_core_handle_ptr->tx_log.log_flag   = FALSE;
  rfgsm_core_handle_ptr->tx_log.delay_val  = (int16) RF_INVALID_VALUE_16BIT;
  rfgsm_core_handle_ptr->tx_log.freq_error = (int32) RF_INVALID_VALUE_32BIT;
  rfgsm_core_handle_ptr->tx_log.num_slots  = (uint8) 0;
  rfgsm_core_handle_ptr->tx_log.sar_state  = (int32) RF_INVALID_VALUE_32BIT;
  rfgsm_core_handle_ptr->tx_log.vbatt_mv   = (uint16)RF_INVALID_VALUE_16BIT;
  rfgsm_core_handle_ptr->tx_log.therm_read = (uint16)RF_INVALID_VALUE_16BIT;
  rfgsm_core_handle_ptr->tx_log.temp_comp_index   = (uint8)RF_INVALID_VALUE_8BIT;
  rfgsm_core_handle_ptr->tx_log.temp_comp_backoff = (int32)RF_INVALID_VALUE_32BIT;
  rfgsm_core_handle_ptr->tx_log.temp_remainder    = (uint16)RF_INVALID_VALUE_16BIT;

  rfgsm_core_handle_ptr->rx_log.log_flag = FALSE;
  rfgsm_core_handle_ptr->rx_log.rx_band  = RFCOM_NUM_GSM_BANDS;
  rfgsm_core_handle_ptr->rx_log.rx_chan  = (uint32)RF_INVALID_VALUE_32BIT;
  rfgsm_core_handle_ptr->rx_log.lna_gain = (uint32)RF_INVALID_VALUE_32BIT;

  rfgsm_core_handle_ptr->valx16 = (int16)RF_INVALID_VALUE_16BIT;

  rfgsm_core_handle_ptr->rfgsm_meas_device_backup_token = (uint8)RF_INVALID_VALUE_8BIT;

  rfgsm_core_handle_ptr->iq_capture = NULL;
  rfgsm_core_handle_ptr->rx_burst = RF_MAX_BURST_TYPES;

  /* RFLM specific vars */
  rfgsm_core_handle_ptr->buffer_id = (uint8)RF_INVALID_VALUE_8BIT;
  rfgsm_core_handle_ptr->rx_burst_num = (uint8)RF_INVALID_VALUE_8BIT;
  rfgsm_core_handle_ptr->rx_burst_type = RFGSM_MAX_RX_BURST_TYPES;
  rfgsm_core_handle_ptr->pa_ramp_buffer_id = (uint8)RFGSM_CORE_DOUBLE_BUF0;
  rfgsm_core_handle_ptr->amam_ampm_buffer_id = (uint8)RFGSM_CORE_DOUBLE_BUF0;
  rfgsm_core_handle_ptr->num_pa_ramp_table_entries = RFGSM_DEFAULT_NUM_RAMP_SAMPLES;
  rfgsm_core_handle_ptr->num_amam_table_entries = RFGSM_DEFAULT_NUM_AMAM_SAMPLES;
  rfgsm_core_handle_ptr->num_ampm_table_entries = RFGSM_DEFAULT_NUM_AMPM_SAMPLES;
  rfgsm_core_handle_ptr->pa_ramp_saturation_high = RFGSM_RAMP_SATURATION_HIGH_LIMIT;
  rfgsm_core_handle_ptr->pa_ramp_saturation_low = RFGSM_RAMP_SATURATION_LOW_LIMIT;

#ifdef FEATURE_RF_ASDIV
  rfgsm_core_handle_ptr->set_prev_ant_pos = FALSE;
#endif

  return TRUE;
}



/*----------------------------------------------------------------------------*/
/*
  @brief
  This API initialises the NV data ptrs in core handles including Rx and Tx NV tbl and Rx freq swpt

  @details

  @params
  rfm_device_enum_type rfm_dev    device type determining the device being used

  @retval
  boolean status                  determining the success of initialisation
*/
boolean rfgsm_core_handle_nv_init(rfm_device_enum_type rfm_dev)
{
  boolean status = TRUE;

  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  rfcom_gsm_band_type first_nv_band_supported;

  /* get handle for this device*/
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return FALSE;
  }

  first_nv_band_supported = rfgsm_nv_get_lowest_supported_band(rfm_dev);

  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"Default RX and Tx NV table pointers initialised to band = %d", first_nv_band_supported );

  /* Initialize Rx band pointer to the default gsm NV table */
  rfgsm_core_handle_ptr->rfgsm_rx_nv_tbl =
             rfgsm_nv_get_tbl_ptr(rfm_dev, first_nv_band_supported, RFGSM_NV_RX);

  if ( TRUE == rfgsm_core_get_device_tx_capability(rfm_dev))
  {
    /* Initialize Tx band pointer to the default gsm NV table */
    rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl =
               rfgsm_nv_get_tbl_ptr(rfm_dev, first_nv_band_supported, RFGSM_NV_TX);
  }

  /* Initialize pointer to GSM/EGSM rx switch structure */
  rfgsm_core_handle_ptr->rfgsm_nv_rx_freq_comp_switch_ptr =
             rfgsm_nv_get_rx_freq_comp_switch_ptr(rfm_dev, first_nv_band_supported);

  /* Initialize pointer to GSM/EGSM rx switch structure for SAWless HL mode */
  rfgsm_core_handle_ptr->rfgsm_nv_hl_rx_switch_ptr =
             rfgsm_nv_get_hl_rx_freq_comp_switch_ptr(rfm_dev, first_nv_band_supported);

  /* Initialize pointer to GSM/EGSM enhanced rx switch structure */
  rfgsm_core_handle_ptr->rfgsm_nv_enh_rx_switch_ptr =
             rfgsm_nv_get_enh_rx_switch_ptr(rfm_dev, first_nv_band_supported);

  return status;
}

/*----------------------------------------------------------------------------*/
/*
  @brief
  This API retrieves the sigma delta maximum from the hard-coded RFC config table

  @params
  rfm_device_enum_type rfm_dev    device type determining the device being used

  @retval
  rf_sigma_delta_max_db           sigma delta max value
*/
uint16 rfgsm_core_get_rf_sigma_delta_max_db
(
  rfm_device_enum_type rfm_dev
)
{
  uint16 rf_sigma_delta_max_db = 0;
  rfc_gsm_core_config_type *rfc_gsm_core_ptr = NULL;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return rf_sigma_delta_max_db;
  }

  rfc_gsm_core_ptr = rfgsm_core_handle_ptr->rfc_gsm_core_ptr;

  if (rfc_gsm_core_ptr != NULL)
  {
    rf_sigma_delta_max_db = rfc_gsm_core_ptr->rf_sigma_delta_max_db;
  }

  return( rf_sigma_delta_max_db );
}

/*----------------------------------------------------------------------------*/
/*
  @brief
  This API is called to place core handle vars including temp comp data and NV
  tables into default states when initialising and when exiting GSM mode.

  @params
  rfm_device_enum_type rfm_dev    device type determining the device being used

  @retval
  boolean status                  determining the success of initialisation
*/
boolean rfgsm_core_handle_init(rfm_device_enum_type rfm_dev)
{
  rf_path_enum_type rf_path;
  boolean init_status = TRUE;

  /* initialize the rfgsm_core related data */
  init_status &= rfgsm_core_data_init(rfm_dev);

  rf_path = rfcommon_core_device_to_path(rfm_dev);

  /* Store the device capability of the device in rfgsm_core_handle*/
  (void)rfgsm_core_set_device_capability(rfm_dev,
                                         rf_path);

  if ( TRUE == rfgsm_core_get_device_tx_capability(rfm_dev))
  {
    init_status &= rfgsm_core_temp_comp_data_init(rfm_dev);
  }

  /* Initialize the gsm NV data */
  init_status &= rfgsm_core_handle_nv_init(rfm_dev);

  init_status &= rfgsm_core_handle_rx_debug_logging_enable_init(rfm_dev);

  return init_status;
}

/*----------------------------------------------------------------------------*/
/*
  @brief
  This API is called to place core handle vars, excluding temp comp data, and NV
  tables into default states when waking up.

  @params
  rfm_device_enum_type rfm_dev    device type determining the device being used

  @retval
  boolean status                  determining the success of initialisation
*/
boolean rfgsm_core_wakeup_data_init(rfm_device_enum_type rfm_dev)
{
  boolean init_status = TRUE;

  /* initialize the rfgsm_core related data */
  init_status &= rfgsm_core_data_init(rfm_dev);

  /* Initialize the gsm NV data */
  init_status &= rfgsm_core_handle_nv_init(rfm_dev);

  return init_status;
}

/*----------------------------------------------------------------------------*/
/*
  @brief
  Retrieve the core handle pointer to access the core vars for device rfm_dev

  @param
  rfm_device_enum_type rfm_dev    device type determining the device being used

  @retval
  rfgsm_core_handle_type          pointer to the core handles for device rfm_dev
*/
rfgsm_core_handle_type* rfgsm_core_handle_get(rfm_device_enum_type rfm_dev)
{
  rfgsm_core_handle_type *handle = NULL;

  if (rfm_dev < RFM_MAX_WAN_DEVICES)
  {
    handle = rfgsm_core_handles[rfm_dev];
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Failed to get rfgsm_core_handle: invalid rfm_dev = %d",rfm_dev);
  }

  return handle;
}

/*----------------------------------------------------------------------------*/
/*
  @brief
  This API frees the memory allocated for core handle variables for each device

  @param
  rfm_device_enum_type rfm_dev

  @retval
*/
rfgsm_core_handle_type* rfgsm_core_handle_create_and_init(rfm_device_enum_type rfm_dev)
{
  if (rfm_dev < RFM_MAX_WAN_DEVICES)
  {
    if(rfgsm_core_handles[rfm_dev] == NULL) /* only create the handle if it NOT already created */
    {
      rfgsm_core_handles[rfm_dev] = (rfgsm_core_handle_type*)modem_mem_alloc(sizeof(rfgsm_core_handle_type), MODEM_MEM_CLIENT_RFA);

      if (rfgsm_core_handles[rfm_dev] == NULL) // failed to allocate memory
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Failed to create rfgsm_core_handle %d",rfm_dev);
      }
      else
      {
        /* Initialise Memory */
        memset( rfgsm_core_handles[rfm_dev], 0, sizeof(rfgsm_core_handle_type) );

        /* Set the initialisation for once-only variables to FALSE to ensure these get reset */
        rfgsm_core_handles[rfm_dev]->init_once = FALSE;
      }
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_core_handle has been already created! rfm_dev=%d", rfm_dev);
    }

    /* Initialise all core handle vars including temp comp, NV data handles */
    /* newly or previously created                                          */
    rfgsm_core_handle_init(rfm_dev);

    rf_common_init_critical_section(&rfgsm_core_handles[rfm_dev]->rfgsm_core_crit_sect);
    rf_common_init_critical_section(&rfgsm_core_handles[rfm_dev]->rfgsm_core_enter_exit_crit_sect);
  }

  return rfgsm_core_handles[rfm_dev];
}

/*----------------------------------------------------------------------------*/
/*
  @brief
  This API frees the memory allocated for core handle variables for each device.
  This should only be called on destroying of the whole UE, NOT just GSM.

  @retval
  boolean
*/
boolean rfgsm_core_handle_destroy(void)
{
  uint8 rfm_dev = 0;
  for (rfm_dev = 0; rfm_dev < RFM_MAX_WAN_DEVICES; rfm_dev++)
  {
    /* only destory the handle if its NOT already been destroyed */
    if(rfgsm_core_handles[rfm_dev] != NULL)
    {
      rf_common_deinit_critical_section(&rfgsm_core_handles[rfm_dev]->rfgsm_core_crit_sect);
      rf_common_deinit_critical_section(&rfgsm_core_handles[rfm_dev]->rfgsm_core_enter_exit_crit_sect);

	  if ( rfgsm_core_get_sawless_status(rfm_dev) == TRUE )
      { /* Deallocate all GSM bands HL tables*/
        if(rfgsm_core_deallocate_high_lin_flag_table(rfm_dev) == FALSE)
        {
          RF_MSG_1(MSG_LEGACY_HIGH, "Did not free HL table memory for dev %d: nothing to free", rfm_dev );
        }
      }

      if( rfgsm_core_handles[rfm_dev]->iq_capture != NULL )
      {
        /* Free the IQ capture buffer */
        modem_mem_free(rfgsm_core_handles[rfm_dev]->iq_capture, MODEM_MEM_CLIENT_RFA);
      }
      modem_mem_free(rfgsm_core_handles[rfm_dev], MODEM_MEM_CLIENT_RFA);
    }
  }

  return TRUE;
}


/*----------------------------------------------------------------------------*/
/* API to appent APT script */
void rfgsm_core_append_apt_script(rf_buffer_intf* dest_buf_ptr, rf_buffer_intf* src_buf_ptr)
{
  uint16 src_num_trans;
  uint16 space_available;
  uint8 j;
  const rf_hal_bus_resource_script_type* src_rffe_script_ptr;

  if ((src_buf_ptr == NULL) || (dest_buf_ptr == NULL) )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_append_apt_script: src_buf_ptr or dest_buf_ptr is NULL!", 0);
    return;
  }

  /* Append RFFE transactions */
  space_available = rf_buffer_get_space_available( dest_buf_ptr,RF_BUFFER_RFFE );
  src_num_trans = rf_buffer_get_num_trans( src_buf_ptr, RF_BUFFER_RFFE );
  src_rffe_script_ptr = rf_buffer_get_script_ptr(src_buf_ptr,RF_HAL_BUS_RESOURCE_RFFE);

  if ( src_num_trans <= space_available)
  {
    for (j=0; j< src_num_trans; j++)
    {
      rf_buffer_append_rffe(dest_buf_ptr,
                            src_rffe_script_ptr->script_ptr.rffe[j].slave_id,
                            src_rffe_script_ptr->script_ptr.rffe[j].channel,
                            src_rffe_script_ptr->script_ptr.rffe[j].addr,
                            src_rffe_script_ptr->script_ptr.rffe[j].data[0],
                            (src_rffe_script_ptr->script_ptr.rffe[j].rd_wr == RF_HAL_BUS_WRITE) ? RF_BUFFER_WRITE : RF_BUFFER_READ,
                            src_rffe_script_ptr->script_ptr.rffe[j].start_delta,
                            src_rffe_script_ptr->script_ptr.rffe[j].extended_cmd,
                            src_rffe_script_ptr->script_ptr.rffe[j].half_rate,
                            src_rffe_script_ptr->script_ptr.rffe[j].rd_delay,
                            TRUE);
    }
  }
  else
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_append_apt_script: space_available is less than src_num_trans!", space_available, src_num_trans);
  }

}


/* --------------------------API to append any two scripts--------------------------------*/
/*
  @brief
  This API appends source script to destination script.

  if override_time flag is set, only then the other params such as
  timing and time_ref can take effect.

  if override time flag is set and time_ref flag is true,
  all the transactions in the source buffer have start delta
  of 'timing' passed as argument.

  if override time flag is set and time_ref flag is false,
  all the relative timings of the transactions in source buffer
  are preserved but the earliest transaction in the source buffer
  can go out at an offset of 'timing' specified after the last
  transaction in the destination buffer.

  PS: last transaction in destination buffer need not be the final
  transaction in the destination buffer to go out.

  @retval
  none
*/
void rfgsm_core_append_script(rf_buffer_intf* dest_buf_ptr,
                              rf_buffer_intf* src_buf_ptr,
                              boolean override_time,
                              int16 timing,
                              boolean time_ref)
{
  uint16 src_num_trans;
  uint16 space_available;
  uint8 j;
  int16 smallest_start_delta = 0;
  int16 start_delta;
  int16 time_offset = 0;
  int16 last_transaction_timing = 0;
  const rf_hal_bus_resource_script_type* src_rffe_script_ptr;
  const rf_hal_bus_resource_script_type* dest_rffe_script_ptr;

  if ((src_buf_ptr == NULL) || (dest_buf_ptr == NULL) )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_append_script: src_buf_ptr or dest_buf_ptr is NULL!", 0);
    return;
  }

  /* Append RFFE transactions */
  space_available = rf_buffer_get_space_available( dest_buf_ptr,RF_BUFFER_RFFE );
  src_num_trans = rf_buffer_get_num_trans( src_buf_ptr, RF_BUFFER_RFFE );
  src_rffe_script_ptr = rf_buffer_get_script_ptr(src_buf_ptr,RF_HAL_BUS_RESOURCE_RFFE);
  dest_rffe_script_ptr = rf_buffer_get_script_ptr(dest_buf_ptr, RF_HAL_BUS_RESOURCE_RFFE);

  if ((src_rffe_script_ptr == NULL) || (dest_rffe_script_ptr == NULL) )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_append_script: script pointer in source or dest buffer is NULL!", 0);
    return;
  }

  if ( src_num_trans <= space_available)
  {
    if (!override_time)
    {
      for (j=0; j< src_num_trans; j++)
      {
        rf_buffer_append_rffe(dest_buf_ptr,
                              src_rffe_script_ptr->script_ptr.rffe[j].slave_id,
                              src_rffe_script_ptr->script_ptr.rffe[j].channel,
                              src_rffe_script_ptr->script_ptr.rffe[j].addr,
                              src_rffe_script_ptr->script_ptr.rffe[j].data[0],
                              (src_rffe_script_ptr->script_ptr.rffe[j].rd_wr == RF_HAL_BUS_WRITE) ? RF_BUFFER_WRITE : RF_BUFFER_READ,
                              src_rffe_script_ptr->script_ptr.rffe[j].start_delta,
                              src_rffe_script_ptr->script_ptr.rffe[j].extended_cmd,
                              src_rffe_script_ptr->script_ptr.rffe[j].half_rate,
                              src_rffe_script_ptr->script_ptr.rffe[j].rd_delay,
                              TRUE);
      }
    }
    else
    {
      /* If new time ref flag is false, preserve relative timings in the source buffer
         but transactions in source buffer can only take place after destination buffer
       */
      if (time_ref == FALSE)
      {
        /*Find the smallest start timing in the source buffer*/
        for (j = 0; j < src_num_trans; j++)
        {
          if (src_rffe_script_ptr->script_ptr.rffe[j].start_delta < smallest_start_delta)
          {
            smallest_start_delta = src_rffe_script_ptr->script_ptr.rffe[j].start_delta;
          }
        }
        /* If destination buffe is not empty*/
        if (dest_rffe_script_ptr->num_trans > 1)
        {
          /* Get the timing of the last transaction in destination buffer*/
          last_transaction_timing = dest_rffe_script_ptr->script_ptr.rffe[dest_rffe_script_ptr->num_trans - 1].start_delta;
          /* Calculate the time offset needed in the source buffer
             such that the earliest transaction in the source buffer
             goes right after the last transaction in the destination
             buffer.
           */
          time_offset = last_transaction_timing - smallest_start_delta;
        }
        else
        {
          /* If destination buffer is empty*/
          time_offset = 0;
        }
      }
      else
      {
        /* New_time_ref flag True means that use specified timing for all the
           transactions in the source buffer.
         */
        start_delta = timing;
        time_offset = 0;
      }

      for (j=0; j< src_num_trans; j++)
      {
        if (!time_ref)
        {
          /* Add calulated timing offset to each transaction in the source buffer*/
          start_delta = src_rffe_script_ptr->script_ptr.rffe[j].start_delta + time_offset;
          start_delta += timing;

        }

        rf_buffer_append_rffe(dest_buf_ptr,
                              src_rffe_script_ptr->script_ptr.rffe[j].slave_id,
                              src_rffe_script_ptr->script_ptr.rffe[j].channel,
                              src_rffe_script_ptr->script_ptr.rffe[j].addr,
                              src_rffe_script_ptr->script_ptr.rffe[j].data[0],
                              (src_rffe_script_ptr->script_ptr.rffe[j].rd_wr == RF_HAL_BUS_WRITE) ? RF_BUFFER_WRITE : RF_BUFFER_READ,
                              start_delta,
                              src_rffe_script_ptr->script_ptr.rffe[j].extended_cmd,
                              src_rffe_script_ptr->script_ptr.rffe[j].half_rate,
                              src_rffe_script_ptr->script_ptr.rffe[j].rd_delay,
                              TRUE);
      }
    }
  }
  else
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_append_apt_script: space_available is less than src_num_trans!", space_available, src_num_trans);
  }
}


/*----------------------------------------------------------------------------*/
/*
  @brief:rfgsm_core_set_tuner_tune_code_override


  @details
  This function is used to override the current tuner tunecode  settings.

  @param none
*/
uint32 rfgsm_core_set_tuner_tune_code_override( rfm_device_enum_type rfm_dev,uint8 override_flag,void *data,uint8* tuner_nv_ptr,uint8* tuner_id_ptr)
{
  uint32 status = 0xFFFFFFFF;
  void **tuner_mgr_list = NULL;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return status;
  }

  status = RFCMN_ATUNER_ERROR;

  // Since Qtuner is the same for TX /RX and in FTM mode we use same band for TX and RX
  tuner_mgr_list= rfc_gsm_get_rf_device_list ( rfm_dev,
                                               RFC_CONFIG_RX,
                                               rfgsm_core_handle_ptr->band,
                                               RFDEVICE_TUNER_MANAGER);

  if ( ( tuner_mgr_list != NULL ) &&
       ( tuner_mgr_list[0]!= NULL) )
  {
    status = rfcommon_atuner_tune_code_override( (rfcommon_atuner_intf*)tuner_mgr_list[0],
                                                  override_flag,
                                                  data,
                                                  tuner_nv_ptr,
                                                  tuner_id_ptr);
  }
  else
  {
    status = RFCMN_ATUNER_NOT_PRESENT;
  }

  return status;

}
/*----------------------------------------------------------------------------*/

uint16 rfgsm_core_get_max_pcl_index( rfm_device_enum_type rfm_dev, int16 *tx_pow_array, uint8 num_slots, uint8* slot_index)
{
  uint8 i = 0xff;
  uint16 pcl_index =0xffff;
  int16 max_pwr;
  gsm_tx_static_nv_type *tx_static_nv_data_ptr;

  rfgsm_core_handle_type *rfgsm_core_handle_ptr;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_gsm::tx_gain_device_config, rfgsm_core_handle_ptr is NULL!", 0);
    return i;
  }

  RF_NULL_CHECK_RTN( rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl, "rfgsm_tx_nv_tbl is NULL!. Check RF Tx Cal data", FALSE );
  RF_NULL_CHECK_RTN( rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr, "rfgsm_tx_nv_tbl->tx_static_nv_data_ptr is NULL!. Check RF Tx Cal data", FALSE );


  if (num_slots > 0)
  {
    max_pwr = tx_pow_array[0];
    for ( i=1;i < num_slots;i++ )
    {
      if (tx_pow_array[i] > max_pwr)
      {
        max_pwr = tx_pow_array[i];
        *slot_index = i;
      }
    }

    tx_static_nv_data_ptr = rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr;
    for (pcl_index=0;pcl_index<RFNV_NUMBER_OF_PCLS;pcl_index++)
    {
      if ( max_pwr <= tx_static_nv_data_ptr->power_levels[pcl_index] )
      {
        break;
      }
    }
  }

  return pcl_index;

}



uint16 rfgsm_core_get_pcl_index( rfm_device_enum_type rfm_dev, int16 tx_pwr)
{

  uint16 pcl_index =0xff;
  gsm_tx_static_nv_type *tx_static_nv_data_ptr;

  rfgsm_core_handle_type *rfgsm_core_handle_ptr;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_get_pcl_index, rfgsm_core_handle_ptr is NULL!", 0);
    return pcl_index;
  }

  RF_NULL_CHECK_RTN( rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl, "rfgsm_tx_nv_tbl is NULL!. Check RF Tx Cal data", FALSE );
  RF_NULL_CHECK_RTN( rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr, "rfgsm_tx_nv_tbl->tx_static_nv_data_ptr is NULL!. Check RF Tx Cal data", FALSE );

  tx_static_nv_data_ptr = rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr;
  for (pcl_index=0;pcl_index<RFNV_NUMBER_OF_PCLS;pcl_index++)
  {
    if ( tx_pwr <= tx_static_nv_data_ptr->power_levels[pcl_index] )
    {
      break;
    }
  }

  return pcl_index;

}


/*----------------------------------------------------------------------------*/
/*!
   @brief
   Returns the current TX RF band

*/
rfcom_gsm_band_type rfgsm_core_get_tx_band( rfm_device_enum_type rfm_dev )
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  if( TRUE == rfgsm_core_get_device_tx_capability(rfm_dev))
  {
    rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

    RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!", RFCOM_NUM_GSM_BANDS);

    return rfgsm_core_handle_ptr->tx_band;
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "Trying to get TX band from non-TX logical device %d ", rfm_dev);

    return RFCOM_NUM_GSM_BANDS;
  }
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Returns the current RX RF band

*/
rfcom_gsm_band_type rfgsm_core_get_rx_band( rfm_device_enum_type rfm_dev )
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!", RFCOM_NUM_GSM_BANDS);

  return rfgsm_core_handle_ptr->band;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function calculates the RX burst processing time from the PLL writes to end of the RX burst script.
   It generates the RX burst script by looping through all the logical devices and device instances assciated with each logical device.
   When PRx and DRx are using the same PLL, all the DRX writes are considered as Auto A writes.
   This API calculates the RX burst processing time for the Auto A writes and PLL writes including the CCS task switching delays.
   If this time exceeds the PLL settling time, the additional time offset needs to be reflected in RX alpha and first SSBI timing.
   In a scenario when PRX and DRX uses two different PLLs, this API determines the worst case RX script processing time for the Auto A + PLL writes.

   Returns the RX script processing time from PLL writes.

*/
void rfgsm_core_calc_rx_timing(rfm_device_enum_type rfm_dev)
{

  rfcom_gsm_band_type band;
  rfc_cfg_info_type cfg_struct = {0, RF_GSM_NV_PATH_NUM};
  rfgsmlib_sbi_burst_struct burst_script_infor;
  void *dummy_script = NULL;
  /*Total number of wtr transactions in RX Burst*/
  uint16 rx_burst_script_size= 0;
  /* RX time after the AutoB writes*/
  uint32 total_rx_time_from_pll = 0;
  uint16 buffer_mask;


  uint16 arfcn[RFCOM_NUM_GSM_BANDS] = {128,1,512,512};

  if (dummy_script == NULL)
  {
    dummy_script = (void *)rf_buffer_create_tech_scaling(0,40,30,NUM_QS_PER_100_USEC);
  }

  /*-----------initialize-----------*/
  burst_script_infor.burst = RF_RX_BURST;
  burst_script_infor.scripts = NULL;
  burst_script_infor.burst_alpha = 0;
  burst_script_infor.pll_settling_time_qs = 0;
  burst_script_infor.num_ssbis_after_pll = 0;
  burst_script_infor.num_ssbis_before_pll = 0;
  burst_script_infor.uses_pll_flag = TRUE;
  burst_script_infor.last_sbi_bus = 0xFF;
  burst_script_infor.num_bus_changes = 0;
  burst_script_infor.override_alpha  = FALSE;
  burst_script_infor.autoA_execution_time_qs = 0;
  burst_script_infor.autoB_execution_time_qs = 0;
  burst_script_infor.burst_timing_offset_adjust_qs = 0;
  band = rfgsm_nv_get_lowest_supported_band( rfm_dev );

  /*---------------------------------*/
  //for supported band
  if(rfc_gsm_get_cfg_info( rfm_dev, band, RFC_CONFIG_RX, &cfg_struct, 0))
  {
    rfdevice_gsm_set_rx_gain_range(rfm_dev,1);

    rfdevice_gsm_set_rx_band(rfm_dev, band, RFDEVICE_CREATE_SCRIPT, (rf_buffer_intf *)dummy_script);

    rfdevice_gsm_tune_rx(rfm_dev, arfcn[band], 0, 0);

    //ToDo: Need to add additional handling for IRAT scenarios,  if Script size is not same for IRAT and Standalone script
    rfdevice_gsm_set_start_script(rfm_dev, RFDEVICE_OPER_MODE_STANDALONE,
                                  &burst_script_infor, (rf_buffer_intf*)dummy_script,
                                  &buffer_mask, FALSE,
                                  band);

    if (burst_script_infor.uses_pll_flag == TRUE)
    {
      /*Get total number of transactions for current RX Burst script*/
      rx_burst_script_size = rf_buffer_get_num_trans((rf_buffer_intf*)dummy_script, RF_BUFFER_RFFE );
      if (rx_burst_script_size > 0)
      {
      burst_script_infor.num_ssbis_after_pll = rx_burst_script_size - burst_script_infor.num_ssbis_before_pll;
      }
    }
  }

  total_rx_time_from_pll = (RFGSM_MB_RFFE_TIME_CALC_QS(burst_script_infor.num_ssbis_after_pll) + RX_DC_CAL_DELAY_QS);

  if (dummy_script != NULL)
  {
    rf_buffer_delete(dummy_script);
  }

  if (rfgsm_core_is_device_prx(rfm_dev) == TRUE)
  {
    rfc_gsm_update_rx_timing_params( total_rx_time_from_pll,
                                     burst_script_infor.num_ssbis_before_pll,
                                     burst_script_infor.num_ssbis_after_pll,
                                     burst_script_infor.pll_settling_time_qs,
                                     rfm_dev );

    rfc_gsm_update_rx_timing_params( total_rx_time_from_pll,
                                     burst_script_infor.num_ssbis_before_pll,
                                     burst_script_infor.num_ssbis_after_pll,
                                     burst_script_infor.pll_settling_time_qs,
                                     rfgsm_core_get_drx_dev(rfm_dev) );
  }

  return;
}

/*----------------------------------------------------------------------------*/

#ifdef FEATURE_RF_ASDIV
/*----------------------------------------------------------------------------*/
/*
  @brief:rfgsm_core_set_antenna.


  @details
  This function process set antenna request by setting variable in rfgsm_core_handle.
  Antenna will be switched to new position during next TX/RX Burst.

  @param rfm_dev the device in use.
         ant_pos the desired antenna position.
*/
void rfgsm_core_set_antenna(rfgsm_core_data_type *core_data,
                            uint8 ant_pos)
{
  rfcommon_asdiv_position_type current_pos = rfcommon_asdiv_get_current_position(core_data->rfm_dev);

  if (current_pos != (uint32)ant_pos)
  {
    /* Set UMID to be returned by CCS based on sub id */
    switch (core_data->rfgsm_core_handle_ptr->asd_sub_id)
    {
    case RFLM_GSM_SUBSCRIPTION_1:
      core_data->rfgsm_core_handle_ptr->asd_fw_rsp_umid = RFLM_GSM_ASD_RSP;
      break;
    case RFLM_GSM_SUBSCRIPTION_2:
      core_data->rfgsm_core_handle_ptr->asd_fw_rsp_umid = RFLM_GSM_ASD_SUB2_RSP;
      break;
    case RFLM_GSM_SUBSCRIPTION_3:
      core_data->rfgsm_core_handle_ptr->asd_fw_rsp_umid = RFLM_GSM_ASD_SUB3_RSP;
      break;
    default:
      /* Shouldn't get here*/
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM Set Antenna Req: invalid sub id %d!", core_data->rfgsm_core_handle_ptr->asd_sub_id);
      break;
    }

    core_data->rfgsm_core_handle_ptr->set_antenna_position = ant_pos;
    core_data->rfgsm_core_handle_ptr->asd_rsp = TRUE;
    core_data->rfgsm_core_handle_ptr->configure_asd_scripts = TRUE;
  }
  else
  {
   core_data->rfgsm_core_handle_ptr->set_antenna_position = RF_INVALID_VALUE_8BIT;
   core_data->rfgsm_core_handle_ptr->asd_rsp = FALSE;
  }
}

/*----------------------------------------------------------------------------*/

/*!
   @brief boolean rfgsm_core_process_asd_read_rsp(rfm_device_enum_type rfm_dev, uint8 tag)

   @details
   This api process the ASD read response from RFLM. RFLM sends the ACK only when they have send the CCS
   scripts to HW. Hence when we get ACK from RFLM, it implies that scripts have been executed correctly
   @param
    rfm_device_enum_type
    uint8 tag

   @retval
   TRUE/FALSE
*/

boolean rfgsm_core_process_asd_read_rsp(rfgsm_core_data_type *core_data, uint8 ant_pos)
{
  core_data->rfgsm_core_handle_ptr->set_antenna_position = RF_INVALID_VALUE_8BIT;
  MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH,"ASD_ACK_IS_RECEIVED @ GFN %d for antenna switch position %d", 0, ant_pos);
  if(!rfcommon_asdiv_manager_state_ack())
    {
      MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"ASD ACK IS NOT COMPLETED", 0, 0);
    }
  return TRUE;
} /* end of rfgsm_core_process_asd_read_rsp */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initilize rfgsm_core_asd_rsp_check
*/

void rfgsm_core_asd_rsp_check_init( rfm_device_enum_type rfm_dev )
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!");

  /* define the asd call back */
  timer_def2( &rfgsm_core_handle_ptr->rfgsm_core_asd_rsp_check_cb, NULL );

  rfgsm_core_handle_ptr->asd_switch_info.rfm_dev = rfm_dev;
  rfgsm_core_handle_ptr->asd_switch_info.l1_cb = NULL;
  rfgsm_core_handle_ptr->asd_switch_info.sub_id = 0;
  rfgsm_core_handle_ptr->asd_switch_info.set_antenna_position = (uint8)RF_INVALID_VALUE_8BIT;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Starts the callback that calls rfgsm_core_asd_rsp_response_check_trigger.It is called when
  there is asd response processed.
*/
void rfgsm_core_asd_rsp_check_start( rfm_device_enum_type rfm_dev)
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V( rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!");

  timer_reg( &rfgsm_core_handle_ptr->rfgsm_core_asd_rsp_check_cb,
             (timer_t2_cb_type)rfgsm_core_asd_rsp_check_trigger,
             rfm_dev,
             RFGSM_CORE_ASD_RSP_CHECK_START_INIT,
             0);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stops the callback that calls rfgsm_core_asd_rsp_trigger .It is called when there is no need to send abort request.
*/
void rfgsm_core_asd_rsp_check_stop( rfm_device_enum_type rfm_dev )
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V( rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!");

  (void)timer_clr(&rfgsm_core_handle_ptr->rfgsm_core_asd_rsp_check_cb, T_NONE);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function is the call back function to send abort command to ASDIV common device and
  send failed result to Gl1 */

void rfgsm_core_asd_rsp_check_trigger(uint32 t_unused, timer_cb_data_type data)
{
  rfm_device_enum_type rfm_dev;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  rfa_rf_gsm_set_antenna_callback_s l1_cb_data;

  rfm_dev = (rfm_device_enum_type) data;
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  RF_NULL_CHECK_RTN_V( rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!");

  if(!rfcommon_asdiv_manager_abort_switch( rfm_dev, RFM_EGSM_MODE, NULL ))
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"ASD ABORT IS NOT COMPLETED", 0, 0);
  }
  else
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_HIGH,"ASD switch aborted");
  }

  if (rfgsm_core_handle_ptr!= NULL)
  {
    /* Antenna switch has not occurred => call the GL1 callback function to confirm failure */
    /* The callback function in turn informs TRM the switch has not occurred */
    if (rfgsm_core_handle_ptr->asd_switch_info.l1_cb != NULL)
    {

      /* Populate the L1 callback data */
      l1_cb_data.req_result = RFA_RF_GSM_EXECUTION_ERROR;
      l1_cb_data.ant_pos = rfgsm_core_handle_ptr->asd_switch_info.set_antenna_position;
      l1_cb_data.device = rfgsm_core_handle_ptr->asd_switch_info.rfm_dev;
      l1_cb_data.sub_id = rfgsm_core_handle_ptr->asd_switch_info.sub_id;

      /* Call the L1 callback function */
      rfgsm_core_handle_ptr->asd_switch_info.l1_cb( &l1_cb_data );

      //MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"Sent ASD abort command to GL1" = %d", l1_cb_data.req_result);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Gl1 callback is NULL while sending failed return to Gl1", 0);
    }
  }
}


#endif

/*----------------------------------------------------------------------------*/

/*!
   @brief boolean rfgsm_core_process_device_status_read_rsp

   @details
   This api process the device status read response from buffer id and burst type
   @param
    uint8 sub_id,
    uint8 buffer_id,
    rf_burst_type burst_type,
    uint8 event_type,
    uint8 event_tag

   @retval
   TRUE/FALSE
*/

boolean rfgsm_core_process_device_status_read_rsp(uint8 sub_id,
                                                  uint8 buffer_id,
                                                  rf_burst_type burst_type,
                                                  uint8 event_type,
                                                  uint8 event_tag,
                                                  uint8 burst_num)
{
  if(rfgsm_core_dm_device_status_read(sub_id, buffer_id, burst_type, event_type, event_tag, burst_num))
  {
    return TRUE;
  }

  return FALSE;
}


/*----------------------------------------------------------------------------*/

/*!
   @brief
   This API calculates the nearest frequency bin index based on the arfcn passed

   @details
   basd upon this bin index the TX/RX offset is picked and added to the TX or RX power
   @param
   logical device, band, arfcn, RX or TX

   @retval
   Bin index
*/
uint16 rfgsm_core_get_arfcn_bin_index_char_offset_tbl(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band, uint16 arfcn, rfc_rxtx_enum_type rx_tx)
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  uint16* arfcn_tbl = NULL;
  uint16 bin_indx = 0xFFFF;
  uint16 low_indx =0;
  uint16 high_indx = 0;

  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_get_arfcn_bin_index_char_offset_tbl, rfgsm_core_handle_ptr is NULL!", bin_indx);

  if (rx_tx == RFC_CONFIG_TX)
  {
    RF_NULL_CHECK_RTN( rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl, "rfgsm_tx_nv_tbl is NULL!. Check RF Tx Cal data", bin_indx );
    RF_NULL_CHECK_RTN( rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr, "rfgsm_tx_nv_tbl->tx_static_nv_data_ptr is NULL!. Check RF Tx Cal data", bin_indx );
    arfcn_tbl = rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->coex_tx_char_offset_info_tbl.tx_offset_arfcn_tbl;
  }
  else
  {
    RF_NULL_CHECK_RTN( rfgsm_core_handle_ptr->rfgsm_rx_nv_tbl, "rfgsm_rx_nv_tbl is NULL!. Check RF Rx Cal data", bin_indx );
    RF_NULL_CHECK_RTN( rfgsm_core_handle_ptr->rfgsm_rx_nv_tbl->rx_static_nv_data_ptr, "rfgsm_rx_nv_tbl->rx_static_nv_data_ptr is NULL!. Check RF Rx Cal data", bin_indx );
    arfcn_tbl = rfgsm_core_handle_ptr->rfgsm_rx_nv_tbl->rx_static_nv_data_ptr->coex_rx_char_offset_info_tbl.rx_offset_arfcn_tbl;
  }

  if(RFNV_MAX_ARFCN_BINS < 1)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_get_arfcn_bin_index_char_offset_tbl, Number of ARFCN bins is zero:%d", RFNV_MAX_ARFCN_BINS);
    return bin_indx;
  }

  high_indx = RFNV_MAX_ARFCN_BINS - 1;
  if (arfcn > arfcn_tbl[high_indx])
  {
    return high_indx;
  }

//  if (arfcn <= arfcn_tbl[0])
//  {
//    bin_indx = 0;
//    return bin_indx;
//  }

  for (bin_indx = 1;bin_indx < RFNV_MAX_ARFCN_BINS; bin_indx++)
  {
    if (arfcn <= arfcn_tbl[bin_indx])
    {
      low_indx =  bin_indx - 1;
      high_indx = bin_indx;
      if ( ((int16)(arfcn_tbl[high_indx] - arfcn)) < ((int16)(arfcn - arfcn_tbl[low_indx])) )
      {
        bin_indx = high_indx;
      }
      else
      {
        bin_indx = low_indx;
      }

      break;
    }

  }

  return bin_indx;
}


/*===========================================================================*/
/*! @fn rfgsm_core_build_ip2_cal_setup_script
    @brief This APi builds the setup script for IP2 cal needed before IP2 cal measurements
    @details
    This function is called by rfgsm_core_build_ip2_cal_script()
    Setup script conists of transactions from:
    Set RX band
    Set RX channel
    Set RX gain range
    RX start
    Set TX band
    Set TX channel
    Set TX RGI
    TX start
    Set TX AGC trigger
    TX loopback to RX

    This API also sets spectral inversion data and puts device in High lin mode.

    The IP2 cal alpha value is basically an addition of TX alpha and RX alpha.

    This script updates the start IQ cal codes based on which the set of IQ cal codes
    for current IP2 cal iteration will be generated.

    @param core_data_ptr, rfcom_band, arfcn, tx_rgi, lna range
    @retval boolean success = TRUE
 */


boolean rfgsm_core_build_ip2_cal_setup_script(rfgsm_core_data_type *core_data_ptr,
                                              rfcom_gsm_band_type rfcom_band,
                                              uint16 arfcn,
                                              uint16 tx_rgi,
                                              uint8 lna_range )
{

  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  uint8 last_min_im2_idx = 0;
  int32 last_min_im2_val = 0;
  boolean result =  TRUE;
  rfgsmlib_rx_gain_range_type lna_range_to_set = RFGSMLIB_GAIN_RANGE_1;
  rfgsmlib_sbi_burst_struct burst_script_infor;
  uint16 buffer_mask;
  rfm_device_enum_type rfm_dev = RFM_INVALID_DEVICE;
  int32 ip2_cal_alpha = 0;
  boolean high_lin_mode = TRUE;
  uint32 rsb_a_coef, rsb_b_coef;
  rfdevice_rsb_coef_type rfdev_rsb[RFCOM_NUM_GSM_BANDS];
  uint8 triple_buf_id = 0;
  uint8 burst_num = 0;
  uint8 rx_burst_type = RFGSM_RX_SYNC_BURST;
   /*for spec inv*/
  uint8 intf_idx =0;
  int16 tx_timing = 0;
  burst_script_infor.scripts = NULL;
  burst_script_infor.override_alpha =  TRUE; /* Need to handle this in device driver*/

  if (core_data_ptr == NULL)
  {

    RFGC_MSG( MSG_LEGACY_ERROR, "rfgsm_core_build_ip2_cal_setup_script(), Core data ptr is NULL" );
    return FALSE;
  }


  rfgsm_core_handle_ptr = core_data_ptr->rfgsm_core_handle_ptr;
  rfm_dev = core_data_ptr->rfm_dev;
  ip2_cal_alpha = rfgsm_core_handle_ptr->ip2_cal_alpha;
  /*----------------------------------- set spectarl inversion data----------------------------------------------*/

  rfdevice_gsm_cmd_dispatch(core_data_ptr->rfm_dev, RF_PATH_0, RFDEVICE_GSM_GET_RX_RSB_DATA, &rfdev_rsb[0]);
  /* Two's complement for inversion . RTR returns 17Q15 format coefficients but mdsp requires 16Q14 format.
     rfgsm_msm_get_bbrx_iq_mismatch_gain gets the value of the coefficients and returns a 16Q14 format   */

  rsb_a_coef = -(rfdev_rsb[rfcom_band].rsb_a_coef);
  rsb_b_coef = -(rfgsm_msm_get_coefb_bbrx_iqmc_compensated(rfdev_rsb[rfcom_band].rsb_b_coef, core_data_ptr->rfgsm_core_handle_ptr->rxlm_buf_index_ip2_cal));

  /*force buffer id to zero only to save spectral inv info in DM*/
  triple_buf_id = core_data_ptr->triple_buffer_id;
  core_data_ptr->triple_buffer_id = 0;

  rfgsm_core_dm_set_spectral_inv(core_data_ptr,
                                 rx_burst_type,
                                 burst_num,
                                 rsb_a_coef,
                                 rsb_b_coef);


  if(! rfgsm_core_dm_populate_ip2_intf_data(  core_data_ptr,
                                        rx_burst_type,
                                        burst_num,
                                              intf_idx ))
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_build_ip2_cal_setup_script, RF GSM core update spec inv date in DM failed");
    return FALSE;

  }


  /*restore triple buff id*/

  core_data_ptr->triple_buffer_id = triple_buf_id;


  /*--------------------------------RESET SCRIPTS----------------------------------------------*/

  if(!rf_buffer_clear_script((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.ip2_cal_setup_script,RF_BUFFER_ALL))
  {
     RFGC_MSG( MSG_LEGACY_ERROR, "IP2 cal setup script buffer was unsuccessfully cleared" );
     return FALSE;
  }

  /*--------------------------------BUILD SCRIPTS----------------------------------------------*/

  /* get last min idx from DM. For the very first IP2 cal, the cal step index for min IM2 from last time will be invalid*/

  if (!rfgsm_core_dm_get_last_ip2_cal_iter_result(core_data_ptr->sub_id, &last_min_im2_val, &last_min_im2_idx))
  {
     RFGC_MSG( MSG_LEGACY_ERROR, "IP2 cal setup script failed to get previous IP2 cal result from DM" );
     return FALSE;
  }


  RFGC_MSG_7( MSG_LEGACY_HIGH,
              "rfgsm_core_build_ip2_cal_setup_script, first_iter:%d, last_min_im2_val:%d , "
              "last_min_im2_idx:%d, FN:%d, curr_qs_count:%d, step_size:%d, width:%d ",
              rfgsm_core_handle_ptr->ip2_cal_first_iter,
              last_min_im2_val,
              last_min_im2_idx,
              geran_test_mode_api_get_FN(0),
              geran_test_mode_api_get_qsym_count(0),
              core_data_ptr->rfgsm_core_handle_ptr->ip2_cal_params.alg_params.unit_step_size,
              core_data_ptr->rfgsm_core_handle_ptr->ip2_cal_params.alg_params.width );



  if(!rfdevice_gsm_set_rx_band( rfm_dev,
                          rfcom_band,
                          RFDEVICE_CREATE_SCRIPT,
                          (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.ip2_cal_setup_script))
  {
    RFGC_MSG( MSG_LEGACY_ERROR, "IP2 cal setup script set RX band failed" );
    return FALSE;

  }



  rfdevice_gsm_cmd_dispatch( rfm_dev, RF_PATH_0, RFDEVICE_GSM_SET_HIGH_LIN_MODE, &high_lin_mode );


  /* Tell the library to tune */
  if(!rfdevice_gsm_tune_rx( rfm_dev, arfcn, 0, 0))
  {
    RFGC_MSG( MSG_LEGACY_ERROR, "IP2 cal setup script set RX arfcn failed" );
    return FALSE;
  }


  /*Set LNA range*/
  lna_range_to_set = (rfgsmlib_rx_gain_range_type)(lna_range + 1);


  if(!rfdevice_gsm_set_rx_gain_range(rfm_dev, (int32)lna_range_to_set))
  {
    RFGC_MSG( MSG_LEGACY_ERROR, "IP2 cal setup script set RX LNA range failed" );
    return FALSE;
  }


 /* Tune RX*/
  burst_script_infor.burst = RF_RX_BURST;
  burst_script_infor.new_time_ref = TRUE;
  burst_script_infor.burst_alpha = ip2_cal_alpha;

  if(!rfdevice_gsm_set_start_script(rfm_dev,
                                    RFDEVICE_OPER_MODE_STANDALONE,
                                    &burst_script_infor,
                                    (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.ip2_cal_setup_script,
  	                             &buffer_mask, FALSE,
  	                             rfcom_band))//wtr script here
  {
     RFGC_MSG( MSG_LEGACY_ERROR, "IP2 cal setup script RX start failed" );
     return FALSE;
  }

  /* Get the start delta for the last transaction*/
  tx_timing  = rf_buffer_get_last_transaction_time((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.ip2_cal_setup_script, RF_HAL_BUS_RESOURCE_RFFE);


  /* use the magnitude of the timing*/
  if (tx_timing < 0)
  {
     tx_timing *= -1;
  }



  /* Set TX band*/
  if(!rfdevice_gsm_set_tx_band( rfm_dev, rfcom_band,RFDEVICE_CREATE_SCRIPT,(rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.ip2_cal_setup_script))
  {
     RFGC_MSG( MSG_LEGACY_ERROR, "IP2 cal setup script set TX band failed" );

     /* Clear timed writes flag*/
     rfdevice_gsm_cmd_dispatch( rfm_dev, RF_PATH_0, RFDEVICE_GSM_CLEAR_TIMED_WRITES, NULL);

     return FALSE;
  }


   /* Clear timed writes flag*/
   rfdevice_gsm_cmd_dispatch( rfm_dev, RF_PATH_0, RFDEVICE_GSM_CLEAR_TIMED_WRITES, NULL);


   /*Set TX arfcn*/
  if (!rfdevice_gsm_tune_tx(rfm_dev, arfcn))
  {
     RFGC_MSG( MSG_LEGACY_ERROR, "IP2 cal setup script set TX arfcn failed" );
     return FALSE;
  }


  /* Set TXAGC RGI*/
  if(! rfdevice_gsm_set_rgi(rfm_dev, tx_rgi, RF_MOD_GMSK, 0, TRUE ))
  {
     RFGC_MSG( MSG_LEGACY_ERROR, "IP2 cal setup script set TX rgi failed" );
     return FALSE;

  }


  /* Tune TX*/
  burst_script_infor.burst = RF_TX_BURST;
  burst_script_infor.burst_alpha = tx_timing; // need to change this for IP2 cal
  burst_script_infor.new_time_ref = FALSE;
  if(!rfdevice_gsm_set_start_script(rfm_dev, RFDEVICE_OPER_MODE_STANDALONE,
  	                                &burst_script_infor,
  	                                (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.ip2_cal_setup_script,
  	                                &buffer_mask, FALSE,
  	                                rfcom_band))//wtr script here
  {
     RFGC_MSG( MSG_LEGACY_ERROR, "IP2 cal setup script TX start failed" );
     return FALSE;
  }

  /* Trigger for AGC*/

  rfgsm_core_append_script((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.ip2_cal_setup_script,
                           (rf_buffer_intf*)rfgsm_core_handle_ptr->scripts.tx_agc_trigger_gmsk_script[0],
                           TRUE,
                           0,
                           FALSE);

  /* Set up WTR for IP2 cal*/
  if(!rfdevice_gsm_build_ip2_cal_setup_script(rfm_dev,
                                              rfcom_band,
                                              last_min_im2_val,
                                              last_min_im2_idx ,
                                              rfgsm_core_handle_ptr->ip2_cal_first_iter,
                                              (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.ip2_cal_setup_script))
  {
    MSG( MSG_SSID_RF,MSG_LEGACY_ERROR,
         "rfgsm_core_build_ip2_cal_setup_script, failed to build ip2 cal setup script ");
    return FALSE;
  }


  return result;

}

/*===========================================================================*/
/*! @fn rfgsm_core_build_ip2_cal_cleanup_script
    @brief This APi builds the cleanup script for IP2 cal which restores the WTR state for regular operation.
    @details
    This function is called by rfgsm_core_build_ip2_cal_script()
    Cleanup script conists of transactions from:
    Disable High lin mode
    Set RX_on and TX_on signals to low thereby clearing TX loopback to RX


    @param rfm_dev, rfcom_band
    @retval boolean success = TRUE
 */


boolean rfgsm_core_build_ip2_cal_cleanup_script(rfm_device_enum_type rfm_dev,
                                                rfcom_gsm_band_type rfcom_band)
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  boolean high_lin_mode = FALSE;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_build_ip2_cal_cleanup_script, rfgsm_core_handle_ptr is NULL!", FALSE);

  /*default to low lin mode*/
  rfdevice_gsm_cmd_dispatch( rfm_dev, RF_PATH_0, RFDEVICE_GSM_SET_HIGH_LIN_MODE, &high_lin_mode );

  /*--------------------------------RESET SCRIPTS----------------------------------------------*/


  if(!rf_buffer_clear_script((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.ip2_cal_cleanup_script,RF_BUFFER_ALL))
  {
     RFGC_MSG( MSG_LEGACY_ERROR, "IP2 cal cleanup script buffer was unsuccessfully cleared" );
     return FALSE;
  }

  /*--------------------------------BUILD SCRIPTS----------------------------------------------*/

#ifndef SINGLE_STEP_IP2_DEBUG_MODE
  if (!rfdevice_gsm_build_ip2_cal_cleanup_script(rfm_dev,rfcom_band, (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.ip2_cal_cleanup_script, 0))
  {
       MSG( MSG_SSID_RF,MSG_LEGACY_ERROR,
            "rfgsm_core_build_ip2_cal_cleanup_script, failed to build ip2 cal cleanup script ");
       return FALSE;
  }

#endif


  return TRUE;

}


/*===========================================================================*/
/*! @fn rfgsm_core_build_ip2_cal_step_script
    @brief This APi builds the scripts for each IP2 cal step during which measurements are taken.
    @details
    This function is called by rfgsm_core_build_ip2_cal_script()
    Build script conists of WTR IQ cal codes.
    The params needed for IP2 cal are passed as params which include:
    start_i_code;
    start_q_code;
    Total number of IP2 cal nodes or number of steps
    distance between adjacent cal points
    Number of cal points around the start I Q code

    This API will generate the cal codes based on start I an Q codes.

    @param rfm_dev, rfcom_band, ip2_cal_info
    @retval boolean success = TRUE
 */
boolean rfgsm_core_build_ip2_cal_step_script(rfm_device_enum_type rfm_dev,
                                             rfcom_gsm_band_type rfcom_band,
                                             rfgsm_ip2_cal_alg_params_struct* ip2_cal_info,
                                             rfgsm_ip2_cal_grid_info_type *grid_info_ptr)
{

  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  uint8 cal_step_idx=0;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_build_ip2_cal_step_script, rfgsm_core_handle_ptr is NULL!", FALSE);

  /*--------------------------------RESET SCRIPTS----------------------------------------------*/

  for (cal_step_idx = 0; cal_step_idx < RFGSM_DEVICE_MAX_IP2_CAL_STEPS; cal_step_idx++)
  {

      if(!rf_buffer_clear_script((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.ip2_cal_step_script[cal_step_idx],RF_BUFFER_ALL))
      {
         RFGC_MSG_1( MSG_LEGACY_ERROR, "IP2 cal step script buffer was unsuccessfully cleared, index:%d ",cal_step_idx);
         return FALSE;
      }
  }


  /*--------------------------------BUILD SCRIPTS----------------------------------------------*/



  /* Use the start cal code only for the first step if the override flag is set*/
  if(!rfdevice_gsm_build_ip2_cal_step_script(rfm_dev,
                                             rfcom_band,
                                             (rf_buffer_intf **)rfgsm_core_handle_ptr->scripts.ip2_cal_step_script,
                                             ip2_cal_info,
                                             rfgsm_core_handle_ptr->ip2_cal_first_iter,
                                             0,
                                             grid_info_ptr))
  {
    MSG_1(MSG_SSID_RF,MSG_LEGACY_ERROR,"rfgsm_core_build_ip2_cal_step_script, failed to build ip2 cal step script for index: %d!",cal_step_idx);
    return FALSE;
  }


  return TRUE;

}


/*===========================================================================*/
/*! @fn rfgsm_core_msm_init_ip2_cal
    @brief This API populates up the TXLM and RXLM buffers with IP2 cal specific settings.
    @details
    This function eventually gets called from GL1.

    @param ip2_rxlm_idx, ip2_txlm_idx, core_data_ptr
    @retval boolean success = TRUE
 */

boolean rfgsm_core_msm_init_ip2_cal(rfgsm_core_data_type *core_data_ptr,
                                    uint32 ip2_rxlm_idx,
                                    uint32 ip2_txlm_idx)
{

  rfm_device_enum_type rfm_dev = RFM_MAX_DEVICES;

  if (core_data_ptr == NULL)
  {

    MSG(MSG_SSID_RF,MSG_LEGACY_ERROR,"rfgsm_core_msm_init_ip2_cal, Null core data ptr ");
    return FALSE;
  }

  rfm_dev = core_data_ptr->rfm_dev;

  if (rfm_dev >= RFM_MAX_DEVICES)
  {

    MSG_1(MSG_SSID_RF,MSG_LEGACY_ERROR,"rfgsm_core_msm_init_ip2_cal, Invalid logical device:%d ",rfm_dev);
    return FALSE;
  }

  if (!rfgsm_core_get_sawless_status(rfm_dev))
  {

    MSG_1(MSG_SSID_RF,MSG_LEGACY_ERROR,"rfgsm_core_msm_init_ip2_cal, IP2 cal is not supported on logical device:%d ",rfm_dev);
    return FALSE;
  }

  if ((ip2_rxlm_idx  == RFGSM_RXLM_BUF_IDX_INVALID) || (ip2_txlm_idx == RFGSM_RXLM_BUF_IDX_INVALID))
  {

    MSG_2(MSG_SSID_RF,MSG_LEGACY_ERROR,"rfgsm_core_msm_init_ip2_cal, Invalid buffer indices rxlm:%d  , txlm:%d ",ip2_rxlm_idx,ip2_txlm_idx);
    return FALSE;
  }


  MSG_2(MSG_SSID_RF,MSG_LEGACY_HIGH,"rfgsm_core_msm_init_ip2_cal, IP2 cal rxlm:%d  , txlm:%d ",ip2_rxlm_idx,ip2_txlm_idx);

  /* API to initialize static LM settings for IP2 cal*/
  rfgsm_msm_init(ip2_rxlm_idx, rfm_dev, TRUE, 0);
  rfgsm_msm_enable_tx(ip2_txlm_idx, rfm_dev, TRUE, 0);

  if (core_data_ptr->rfgsm_core_handle_ptr != NULL)
  {
    core_data_ptr->rfgsm_core_handle_ptr->rxlm_buf_index_ip2_cal = ip2_rxlm_idx;
    core_data_ptr->rfgsm_core_handle_ptr->txlm_buf_index_ip2_cal = ip2_txlm_idx;
  }
  else
  {

    MSG(MSG_SSID_RF,MSG_LEGACY_ERROR,"rfgsm_core_msm_init_ip2_cal, Failed to store IP2 cal TXLM and RXLM handles due to null handle ptr ");
  }


  return TRUE;

}



/*===========================================================================*/
/*! @fn rfgsm_core_build_ip2_cal_script
    @brief This API builds the scripts for IP2 cal setup, steps and cleanup.
    It controls how IP2 cal params are passed to internal APIs depending on online mode, cal mode or FTM override mode

    @param ore_data_ptr
    @retval boolean success = TRUE
 */

static rfgsm_ip2_cal_grid_info_type ip2_cal_overlap_grid;


boolean rfgsm_core_build_ip2_cal_script(rfgsm_core_data_type *core_data_ptr)
{


  boolean result = TRUE;
  rfcom_gsm_band_type rfcom_band = RFCOM_NUM_GSM_BANDS;
  //uint16 tx_rgi = RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE;
  //uint8 lna_range = 0;
  rfgsm_ip2_cal_alg_params_struct *ip2_cal_alg_params_ptr = NULL;
  rfgsm_core_handle_type* rfgsm_core_handle_ptr = core_data_ptr->rfgsm_core_handle_ptr;
  rfm_device_enum_type rfm_dev = core_data_ptr->rfm_dev;

  if (rfm_dev >= RFM_MAX_DEVICES)
  {

    MSG_1(MSG_SSID_RF,MSG_LEGACY_ERROR,"rfgsm_core_build_ip2_cal_script, Invalid logical device:%d ",rfm_dev);
    return FALSE;
  }

  if (!rfgsm_core_get_sawless_status(rfm_dev))
  {

    MSG_1(MSG_SSID_RF,MSG_LEGACY_ERROR,"rfgsm_core_build_ip2_cal_script, IP2 cal is not supported on logical device:%d ",rfm_dev);
    return FALSE;
  }

  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!", FALSE);


   /* In FTM mode, use overriden params*/
   if ( ftm_gsm_cal_mode_enabled(rfm_dev) )
   {
     rfcom_band = rfgsm_core_handle_ptr->ip2_cal_params.rfcom_band;

   }
   else
   {

     /* In online mode, use default values for current band*/
     rfcom_band = rfgsm_core_handle_ptr->tx_band;

     /* Use default params (arfcn..etc)*/
     if(!rfgsm_core_load_default_ip2_cal_params(rfm_dev, rfcom_band))
     {
        MSG_1(MSG_SSID_RF,MSG_LEGACY_ERROR,"rfgsm_core_build_ip2_cal_script, load default ip2 cal params failed for dev:%d ",rfm_dev);
        return FALSE;
     }
   }

   /* Point to the IP2 cal alg params*/
   ip2_cal_alg_params_ptr = &rfgsm_core_handle_ptr->ip2_cal_params.alg_params;


   /*--------------------------SETUP----------------------------------*/
   /* Build IP2 cal setup script*/
   if(!rfgsm_core_build_ip2_cal_setup_script(core_data_ptr, rfcom_band, rfgsm_core_handle_ptr->ip2_cal_params.arfcn,rfgsm_core_handle_ptr->ip2_cal_params.tx_rgi,rfgsm_core_handle_ptr->ip2_cal_params.lna_range))
   {

    MSG(MSG_SSID_RF,MSG_LEGACY_ERROR,"rfgsm_core_build_ip2_cal_script, failed to build ip2 cal setup script ");
    return FALSE;
   }


#ifdef ADD_ASM_WRITES

  if(!rfc_gsm_ip2_cal_config(rfm_dev,
                         rfcom_band,
                         rfgsm_core_handle_ptr->ip2_cal_params.arfcn,
                         rfgsm_core_handle_ptr->scripts.ip2_cal_setup_script,
                         rfgsm_core_handle_ptr->scripts.ip2_cal_cleanup_script))
  {

    MSG(MSG_SSID_RF,MSG_LEGACY_ERROR,"rfgsm_core_build_ip2_cal_script, failed to configure RFC device");
  }


#endif


  /* Push IP2 cal setup script to DM*/
  if(! rfgsm_core_dm_dynamic_ip2_cal_setup_event_wrapper(core_data_ptr))
  {
    MSG(MSG_SSID_RF,MSG_LEGACY_ERROR,"rfgsm_core_build_ip2_cal_script, failed to push ip2 cal setup script to DM");
    return FALSE;
  }




  /*-------------------------------STEP----------------------------------*/

  if (rfgsm_core_handle_ptr->ip2_cal_first_iter)
  {
    /*Clear the static memory for every iteration*/
    memset(&ip2_cal_overlap_grid, 0, sizeof(rfgsm_ip2_cal_grid_info_type));
  }

  ip2_cal_overlap_grid.do_average = FALSE;

  /* Build IP2 cal step script*/
  if( !rfgsm_core_build_ip2_cal_step_script(rfm_dev, rfcom_band, ip2_cal_alg_params_ptr, &ip2_cal_overlap_grid))
  {
    MSG(MSG_SSID_RF,MSG_LEGACY_ERROR,"rfgsm_core_build_ip2_cal_script, failed to build ip2 cal step script");
    return FALSE;
  }

  /* Push IP2 cal step script to DM*/
  if(!rfgsm_core_dm_dynamic_ip2_cal_step_event_wrapper(core_data_ptr ))
  {
    MSG(MSG_SSID_RF,MSG_LEGACY_ERROR,"rfgsm_core_build_ip2_cal_script, failed to push ip2 cal step script to DM");
    return FALSE;
  }

  /* Only for 2nd iteration onwards*/
  if (!rfgsm_core_handle_ptr->ip2_cal_first_iter)
  {

    /* update current grid and overlapping grid information*/
    if(!rfgsm_core_dm_set_update_ip2_cal_grid_info(core_data_ptr->sub_id, &ip2_cal_overlap_grid))
    {
      MSG(MSG_SSID_RF,MSG_LEGACY_ERROR,"rfgsm_core_build_ip2_cal_script, failed to update ip2 cal grid info");
      return FALSE;

    }

  }



  /*--------------------------CLEANUP----------------------------------*/

#ifndef SINGLE_STEP_IP2_DEBUG_MODE
  /* Build IP2 cal cleanup script*/
  if(!rfgsm_core_build_ip2_cal_cleanup_script(rfm_dev,rfcom_band))
  {
    MSG(MSG_SSID_RF,MSG_LEGACY_ERROR,"rfgsm_core_build_ip2_cal_script, failed to build ip2 cal cleanup script");
    return FALSE;
  }

  /* Push IP2 cal cleanup script to DM*/
  if(!rfgsm_core_dm_dynamic_ip2_cal_cleanup_event_wrapper( core_data_ptr ))
  {
    MSG(MSG_SSID_RF,MSG_LEGACY_ERROR,"rfgsm_core_build_ip2_cal_script, failed to push ip2 cal cleanup script to DM");
    return FALSE;
  }
#endif

  /* At the end of every iteration clear the first iteration indicator for next iteration */

  rfgsm_core_handle_ptr->ip2_cal_first_iter =  FALSE;

  MSG_3(MSG_SSID_RF,MSG_LEGACY_HIGH,"rfgsm_core_build_ip2_cal_script done, FN:%d, time (qs):%d, ustmr count:%d ",geran_test_mode_api_get_FN(0), geran_test_mode_api_get_qsym_count(0), rflm_time_read_ustmr_count());

  return result;
}

/*===========================================================================*/
/*! @fn rfgsm_core_get_sawless_status
    @brief This API indicates if sawless is supported on any of the GSM bands.
    @param rfm_dev
    @retval sawless supported = TRUE
 */
boolean rfgsm_core_get_sawless_status(rfm_device_enum_type rfm_dev)
{
  boolean sawless_indicator = FALSE;
  rfcom_gsm_band_type rfcom_band;

  rfgsm_core_handle_type* core_handle_ptr = NULL;

  core_handle_ptr = rfgsm_core_handle_get( rfm_dev);

  if (core_handle_ptr == NULL)
  {
    return FALSE;
  }

  for (rfcom_band = 0 ; rfcom_band < RFGSM_MAX_UNIQUE_RFCOM_BANDS; rfcom_band++)
  {
    sawless_indicator |= core_handle_ptr->sawless_support[rfcom_band];
  }

  return sawless_indicator;
}

boolean rfgsm_core_get_band_sawless_status(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type rfcom_band)
{
  rfgsm_core_handle_type* core_handle_ptr = NULL;

  core_handle_ptr = rfgsm_core_handle_get( rfm_dev);

  if (core_handle_ptr == NULL)
  {
    return FALSE;
  }

  if ( rfcom_band >= RFGSM_MAX_UNIQUE_RFCOM_BANDS )
  {
    return FALSE;
  }

  return core_handle_ptr->sawless_support[rfcom_band];
}


/* API to query about sawless support and update supported IP2 cal params*/
void rfgsm_core_init_sawless_status(rfm_device_enum_type rfm_dev)
{

  rfgsm_core_handle_type* core_handle_ptr = NULL;
  rfcom_gsm_band_type band = RFCOM_NUM_GSM_BANDS, sawless_band = RFGSM_MAX_UNIQUE_RFCOM_BANDS;
  uint8 flag = 0;


  core_handle_ptr = rfgsm_core_handle_get( rfm_dev);

  if (core_handle_ptr == NULL)
  {
    return;
  }

  for (band=0; band < RFCOM_NUM_GSM_BANDS; band++)
  {
     /* We currently store less than RFCOM_NUM_GSM_BANDS as GL1 does
         not distinguish between 1800 and 1800_B */
    sawless_band = (band < RFGSM_MAX_UNIQUE_RFCOM_BANDS) ? band : (RFGSM_MAX_UNIQUE_RFCOM_BANDS - 1);
        /*reset previous status*/
    core_handle_ptr->sawless_support[sawless_band] = FALSE;

    if(rfdevice_gsm_get_sawless_support(rfm_dev, sawless_band, &flag))
    {
      /* Only get store the SAWless status if the debug override is false,
         otherwise leave everything set to FALSE */
      if ( rfgsm_debug_flags.sawless_override == FALSE )
      {
        core_handle_ptr->sawless_support[sawless_band] |= (boolean)flag;

		MSG_2(MSG_SSID_RF,MSG_LEGACY_ERROR,"rfgsm_core_init_sawless_status: band %d, sawless %d. ",sawless_band, flag);
      }
    }

  }
}

/*===========================================================================*/
/*! @fn rfgsm_core_load_default_ip2_cal_params
    @brief This API fetches the default supported IP2 cal params from WTR device driver
     and updates the internal structures for execution .
    @param rfm_dev     , rfcom_band
    @retval success = TRUE
 */

boolean rfgsm_core_load_default_ip2_cal_params(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type rfcom_band)
{

  rfgsm_ip2_cal_params_type ip2_cal_params;
  rfgsm_core_handle_type* core_handle_ptr = NULL;

  core_handle_ptr = rfgsm_core_handle_get( rfm_dev);

  if (core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return FALSE;
  }

  if (rfcom_band >= RFCOM_NUM_GSM_BANDS )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfcom band passed is wrong:%d !", rfcom_band);
    return FALSE;
  }


  if(rfdevice_gsm_get_ip2_cal_params(rfm_dev, TRUE, &ip2_cal_params, rfcom_band))
  {

    core_handle_ptr->ip2_cal_params.alg_params.start_i_code = ip2_cal_params.alg_params.start_i_code;
    core_handle_ptr->ip2_cal_params.alg_params.start_q_code = ip2_cal_params.alg_params.start_q_code;
    core_handle_ptr->ip2_cal_params.alg_params.width = ip2_cal_params.alg_params.width;
    core_handle_ptr->ip2_cal_params.alg_params.unit_step_size = ip2_cal_params.alg_params.unit_step_size;
    core_handle_ptr->ip2_cal_params.alg_params.num_cal_points = ip2_cal_params.alg_params.num_cal_points; //((2*width)+1)*((2*width)+1);
    core_handle_ptr->ip2_cal_params.dc_settle_time_qs   = ip2_cal_params.dc_settle_time_qs;
    core_handle_ptr->ip2_cal_params.tx_rgi  = ip2_cal_params.tx_rgi;
    core_handle_ptr->ip2_cal_params.lna_range =  ip2_cal_params.lna_range;
    core_handle_ptr->ip2_cal_params.step_duration_qs = ip2_cal_params.step_duration_qs;
    core_handle_ptr->ip2_cal_params.alg_params.override_params = ip2_cal_params.alg_params.override_params;
    core_handle_ptr->ip2_cal_params.alg_params.num_iter = ip2_cal_params.alg_params.num_iter;

    /*Query RFC here*/
    core_handle_ptr->ip2_cal_params.ip2_cal_supported = rfgsm_core_get_sawless_status(rfm_dev);
    core_handle_ptr->ip2_cal_params.arfcn = ip2_cal_params.arfcn;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"rfgsm_core_load_default_ip2_cal_params, IP2 cal supported:%d ", core_handle_ptr->ip2_cal_params.ip2_cal_supported);
  }
  else
  {

    return FALSE;
  }


  return TRUE;
}


/*===========================================================================*/
/*! @fn rfgsm_core_use_ip2_cal_results
    @brief This API fetches the default supported IP2 cal params from WTR device driver
     and updates the internal structures for execution .
     It is also used to fetch the results of IP2 cal.
    @param rfm_dev     , rfcom_band
    @retval success = TRUE
 */
boolean rfgsm_core_use_ip2_cal_results(rfm_device_enum_type rfm_dev,
                                       rfcom_gsm_band_type rfcom_band,
                                       uint8 sub_id,
                                       rfgsm_ip2_cal_params_type *ip2_cal_params,
                                       uint8 flag_mask,
                                       boolean cal_end )
{
  boolean resp_status = TRUE;
  int32 min_im2_val = 0xFFFFFFFF;
  uint8 min_im2_step_idx = 0xFF;
  rfgsm_core_handle_type* core_handle_ptr = NULL;

  core_handle_ptr = rfgsm_core_handle_get( rfm_dev);

  if (core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return FALSE;
  }

  if (ip2_cal_params == NULL)
  {
    return FALSE;
  }


  if (flag_mask & 0x1)
  {
    /* Get last calculated min im2 and corresponding step index*/
    resp_status &= rfgsm_core_dm_get_last_ip2_cal_iter_result((rflm_gsm_subscription_id)sub_id, &min_im2_val, &min_im2_step_idx);

    if (!resp_status)
    {

      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Could not get min im2 index, min im2 val!", 0);

    }


    MSG_2( MSG_SSID_RF, MSG_LEGACY_HIGH,
           "rfgsm_core_use_ip2_cal_results, min im2 index:%d, min im2 val:%d ",min_im2_step_idx, min_im2_val);


    /* update the Ip2 cal codes and min im2 val after last iteration*/
    resp_status &= rfdevice_gsm_update_ip2_cal_results(rfm_dev,
                                                       rfcom_band,
                                                       min_im2_step_idx,
                                                       min_im2_val,
                                                       core_handle_ptr->ip2_cal_first_iter,
                                                       ((flag_mask & 0x2) >> 1));

    if (!resp_status)
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Could not update the ip2 cal results from last iteration!", 0);

    }

  }

  /* Get Ip2 cal codes and min im2 val after last iteration*/
  resp_status &= rfdevice_gsm_get_ip2_cal_params(rfm_dev , FALSE, ip2_cal_params, rfcom_band);


  if (!resp_status)
  {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Could not update the ip2 cal results from last iteration!", 0);
  }


  return resp_status;

}



/*----------------------------------------------------------------------------*/

/*!
   @brief
   Build the Jammer Detector read scripts

   @details
   Calls the device driver to build the jammer detector scripts

   @param *core_data_ptr - Contains core data used to extract device.

   @retval boolean success/failure indication

*/
boolean rfgsm_core_build_jdet_read_scripts(rfgsm_core_data_type *core_data_ptr)
{
  uint8 index = 0;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  rfm_device_enum_type rfm_dev = core_data_ptr->rfm_dev;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_build_jdet_read_scripts, rfgsm_core_handle_ptr is NULL!", FALSE);

  /*--------------------------------RESET SCRIPTS----------------------------------------------*/

  // Build RX monitor scripts
  for (index=0 ; index < RFGSM_DEVICE_MAX_JDET_READS_PER_RX; index++ )
  {
    if(!rf_buffer_clear_script((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.jdet_read_script[index],RF_BUFFER_ALL))
    {
      RFGC_MSG_1( MSG_LEGACY_ERROR, "JDET read cleanup script buffer %d was unsuccessfully cleared", index );
      return FALSE;
    }

    if ( !rfdevice_gsm_build_jdet_read_script(rfm_dev,
                                              (rfdevice_cmn_jdet_sample_type)index,
                                              ((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.jdet_read_script[index])))
    {
      RFGC_MSG_1( MSG_LEGACY_ERROR, "JDET read script %d not successfully built!", index );
      return FALSE;
    }
  }

  if ( !rfgsm_core_dm_init_jdet_read_data(core_data_ptr) )
  {
    RFGC_MSG( MSG_LEGACY_ERROR, "JDET init DM read data failed!");
    return FALSE;
  }

  return TRUE;
}

/*----------------------------------------------------------------------------*/

/*!
   @brief
   Build the Jammer Detector enable and disable scripts

   @details
   Calls the device driver to build the jammer detector scripts

   @param *core_data_ptr - Contains core data used to extract device.

   @retval boolean success/failure indication

*/

boolean rfgsm_core_get_jdet_config_scripts(rfgsm_core_data_type *core_data_ptr)
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  rfm_device_enum_type rfm_dev = core_data_ptr->rfm_dev;
  /* Set to default, will change the timer setting if we are doing power scan */
  /* Assuming that us and QS are equivalent to avoid conversion (0.92 QS per us)*/
  uint16 holdoff_time_us, mon_holdoff_time_us;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_enable_jdet, rfgsm_core_handle_ptr is NULL!", FALSE);

  //Disable Jdet if the flag is TRUE (read from debug NV at initialisation).
  if ( core_data_ptr->rfgsm_core_handle_ptr->sawless_linearity_control.disable_jdet_configuration == TRUE)
  {
    return TRUE;
  }

  if( !rfgsm_msm_rflm_get_jdet_holdoff_time( &holdoff_time_us, &mon_holdoff_time_us ) )
  {
    RFGC_MSG( MSG_LEGACY_ERROR, "Failed to retrieve JDET holdoffs" );
    return FALSE;
  }

  // If this is a monitor burst use the shorter holdoff time
  if( core_data_ptr->rfgsm_core_handle_ptr->rx_burst_type >= RFGSM_RX_MON_BURST )
  {
    holdoff_time_us = mon_holdoff_time_us;
  }

  if ( !rfdevice_gsm_enable_jdet(rfm_dev,
                                 holdoff_time_us,
                                 RFDEVICE_CREATE_SCRIPT,
                                 ((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.rx_start_script[RX_WTR_SCRIPT])))
  {
    RFGC_MSG_1( MSG_LEGACY_ERROR, "Failed to enable JDET in Rx Start for device %d!", rfm_dev );
    return FALSE;
  }

  /*! @todo move this logging check to the new NVs when available */
  if ( core_data_ptr->rfgsm_core_handle_ptr->sawless_linearity_control.enable_logging )
  {
    RFGC_MSG_2(MSG_LEGACY_MED,
               "Setting Jammer detector holdoff to %d us for burst type %d",
               holdoff_time_us,
               core_data_ptr->rfgsm_core_handle_ptr->rx_burst_type);
  }

  //Jdet off will be added to RX stop script in device driver

  return TRUE;
}
/*----------------------------------------------------------------------------*/

/*!
   @brief
   Used to allocate a chunk of memory to store HL/LL decisions as single bits

   @details
   The memory should be allocated during enter mode and freed upon exit.
   Seperate memory will be maintained per core handle due to different
   logical devices having seperate Rx paths. Memory will be accessed
   as an array of uint8s and bits within the chunks masked.
   Should be 121 uint8s long to accomodate all GSM ARFCNs for 4 bands.
   @param
   logical device

   @retval
   boolean flag to indicate if memory was allocated
*/
boolean rfgsm_core_allocate_high_lin_flag_table( rfm_device_enum_type rfm_dev )
{
  uint16 entries_to_alloc = 0;
  rfgsm_core_handle_type* core_handle_ptr = NULL;

  core_handle_ptr = rfgsm_core_handle_get( rfm_dev);

  if (core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return FALSE;
  }

  entries_to_alloc = RFGSM_CORE_NUM_GSM_ARFCN / RFGSM_HL_TABLE_BITS_PER_INDEX;

  if (( RFGSM_CORE_NUM_GSM_ARFCN % RFGSM_HL_TABLE_BITS_PER_INDEX ) != 0 )
  {
    entries_to_alloc += 1;
  }

  if((ftm_get_mode() == FTM_MODE) && (rfgsm_nv_check_debug_nv(rfm_dev, RFGSM_DEBUG_NV_SAWLESS_FORCE_MODE) == FALSE))
  {
    /* Force default mode to LL for FTM calibration, will be overridden to HL by tools for HL cal
       This will not happen if we have set the mode via the debug NV */
    core_handle_ptr->sawless_linearity_control.linearity_override = RFGSM_SAWLESS_LL_OVERRIDE;
  }

  MSG_2( MSG_SSID_RF,
         MSG_LEGACY_MED,
         "Linearity override set to %d for dev %d",
         core_handle_ptr->sawless_linearity_control.linearity_override,
         rfm_dev);

  if( core_handle_ptr->sawless_linearity_control.rx_hl_flag_table == NULL )
  {
    core_handle_ptr->sawless_linearity_control.rx_hl_flag_table = modem_mem_alloc((entries_to_alloc * sizeof(sawless_lin_table_entry)),
                                                                                  MODEM_MEM_CLIENT_RFA);
    if ( core_handle_ptr->sawless_linearity_control.rx_hl_flag_table == NULL)
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Mem Alloc failed for hl_flag_table on dev %d!", rfm_dev);
      return FALSE;
    }

    /* Start in HL mode */
    memset(core_handle_ptr->sawless_linearity_control.rx_hl_flag_table, 0xFF, (entries_to_alloc * sizeof(sawless_lin_table_entry)));

    /* Save the number of entries in the table so that we can check for out of bounds access */
    core_handle_ptr->sawless_linearity_control.num_table_entries = entries_to_alloc;

    MSG_2( MSG_SSID_RF,
           MSG_LEGACY_HIGH,
           "Allocated HL flag table of %d bits for dev %d",
           (entries_to_alloc * RFGSM_HL_TABLE_BITS_PER_INDEX),
           rfm_dev);  //Indicate how many bits were allocated

    return TRUE; //...if we allocated the memory
  }
  else
  {
    return FALSE; //...since band table has already been allocated
  }
}
/*----------------------------------------------------------------------------*/
/*!
   @brief
   Used to deallocate the chunk of memory to store HL/LL decisions

   @details
   This should always be called upon exit mode to free the memory
   @param
   logical device

   @retval
   boolean flag to indicate if memory was freed
*/
boolean rfgsm_core_deallocate_high_lin_flag_table( rfm_device_enum_type rfm_dev )
{
  rfgsm_core_handle_type* core_handle_ptr = NULL;

  core_handle_ptr = rfgsm_core_handle_get( rfm_dev);

  if (core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return FALSE;
  }

  if ( core_handle_ptr->sawless_linearity_control.rx_hl_flag_table != NULL )
  {
    modem_mem_free(core_handle_ptr->sawless_linearity_control.rx_hl_flag_table, MODEM_MEM_CLIENT_RFA);
    core_handle_ptr->sawless_linearity_control.rx_hl_flag_table = NULL;
    return TRUE; //...as we have deallocated
  }
  else
  {
    return FALSE; //...as there is nothing to deallocate
  }
}
/*----------------------------------------------------------------------------*/
/*!
   @brief
   Checks SAWless state and linearity mode and updates with HL rx switchpoint ptr
   if required

   @details
   This will check the SAWless state and linearity mode for the passed in logical device
   on the channel\band combination provided and will overwrite the passed in pointer
   with the HL Rx switchpoints pointer if required.

   @param
   logical device - device to check
   arfcn - rx channel to check
   band - rfcom band to check
   rx_swt_ptr - pointer to overwrite with HL switchpoint ptr if required

   @retval
   boolean flag to indicate success\fail
*/

boolean rfgsm_core_get_sawless_switchpoints(rfm_device_enum_type rfm_dev, uint16 arfcn, rfcom_gsm_band_type band, rfgsm_nv_rx_switch_type *rx_swt_ptr)
{
  boolean is_high_lin = FALSE;
  rfgsm_core_handle_type* core_handle_ptr = NULL;

  core_handle_ptr = rfgsm_core_handle_get( rfm_dev );

  if (core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return FALSE;
  }

  /* If band is not SAWless, then do not touch the passed in pointer */
  if ( rfgsm_core_get_band_sawless_status(rfm_dev, band) == FALSE )
  {
    return TRUE;
  }

  /* Check HL status of the channel */
  if ( rfgsm_core_is_arfcn_in_high_lin(rfm_dev, arfcn, band, &is_high_lin) == FALSE )
  {
    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"Could not get lin status for arfcn %d, band %d, dev %d!", arfcn, band, rfm_dev);
    return FALSE;
  }

  /* Override the HL setting if any global override is in effect */
  if ( core_handle_ptr->sawless_linearity_control.linearity_override == RFGSM_SAWLESS_HL_OVERRIDE )
  {
    is_high_lin = TRUE;
  }
  else if ( core_handle_ptr->sawless_linearity_control.linearity_override == RFGSM_SAWLESS_LL_OVERRIDE )
  {
    is_high_lin = FALSE;
  }

  /* If the ARFCN is in HL mode, use the HL switch points, otherwise do not touch the pointer */
  if ( is_high_lin )
  {
    rx_swt_ptr = core_handle_ptr->rfgsm_nv_hl_rx_switch_ptr;
    //MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"Returning hl ptr! 0x%08x", rx_swt_ptr);
  }

  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Indicate if an ARFCN should be in HL or LL mode

   @details
   Called whenever RFSW needs to check if an ARFCN should be Rxing in
   HL or LL mode (SAWless targets only)
   @param
   logical device, arfcn, band
   boolean flag to indicate if HL (TRUE) or LL (FALSE) should be used

   @retval
   boolean success/fail flag
*/
boolean rfgsm_core_is_arfcn_in_high_lin( rfm_device_enum_type rfm_dev, uint16 arfcn, rfcom_gsm_band_type band, boolean *is_high_lin )
{
   uint8 bit = 0; //The bit index within the 8 bit entry
   uint8 entry = 0; //The index of the 8 bit entry that the desired bit lies within
   rfgsm_core_handle_type* core_handle_ptr = NULL;

   core_handle_ptr = rfgsm_core_handle_get( rfm_dev );

   if (core_handle_ptr == NULL)
   {
     MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
     return FALSE;
   }

   if ( is_high_lin == NULL )
   {
     MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"High lin flag pointer is NULL!");
     return FALSE;
   }

   if ( (core_handle_ptr->sawless_linearity_control.rx_hl_flag_table) == NULL )
   {
     MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"High lin flag table is NULL!");
     return FALSE;
   }

   if ( rfgsm_core_locate_arfcn_in_high_lin_table(arfcn, band, &entry, &bit) == FALSE )
   {
     MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Failed to get bit/entry for ARFCN %d!", arfcn);
     return FALSE;
   }

   if ( (entry > core_handle_ptr->sawless_linearity_control.num_table_entries) || (bit > RFGSM_HL_TABLE_BITS_PER_INDEX))
   {
     MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"Out of bounds access attempted for ARFCN %d (Entry %d, Bit %d)!", arfcn, entry, bit);
     return FALSE;
   }

   /* Mask bit and check if set */
   if ( (core_handle_ptr->sawless_linearity_control.rx_hl_flag_table[entry] >> bit) & 0x1 )
   {
     *is_high_lin = TRUE;
     return TRUE; //...In HL mode (masked entry is not 0)
   }
   else
   {
     *is_high_lin = FALSE;
     return TRUE; //...In LL mode (masked entry is 0)
   }
}
/*----------------------------------------------------------------------------*/
/*!
   @brief
   Set a bit in the HL table to indicate ARFCN should be in HL mode

   @details
   Called whenever RFSW needs to update the HL/LL bit table to indicate
   that an ARFCN should use HL mode for Rx (set by default).
   @param
   logical device, arfcn, band

   @retval
   boolean success/fail
*/
boolean rfgsm_core_set_arfcn_to_high_lin ( rfm_device_enum_type rfm_dev, uint16 arfcn, rfcom_gsm_band_type band )
{
   uint8 bit = 0; //The bit index within the 8 bit entry
   uint8 entry = 0; //The index of the 8 bit entry that the desired bit lies within
   rfgsm_core_handle_type* core_handle_ptr = NULL;

   core_handle_ptr = rfgsm_core_handle_get( rfm_dev );

   if (core_handle_ptr == NULL)
   {
     MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!");
     return FALSE;
   }

   if ( rfgsm_core_locate_arfcn_in_high_lin_table(arfcn, band, &entry, &bit) == FALSE )
   {
     MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Failed to get bit/entry for ARFCN %d!", arfcn);
     return FALSE;
   }

   if ( (entry > core_handle_ptr->sawless_linearity_control.num_table_entries) || (bit > RFGSM_HL_TABLE_BITS_PER_INDEX))
   {
     MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"Out of bounds access attempted for ARFCN %d (Entry %d, Bit %d)!", arfcn, entry, bit);
     return FALSE;
   }

   /* Mask and set bit */
   core_handle_ptr->sawless_linearity_control.rx_hl_flag_table[entry] |= (uint8)(0x1 << bit);

   return TRUE;
}
/*----------------------------------------------------------------------------*/
/*!
   @brief
   Set a bit in the HL table to indicate ARFCN should be in LL mode

   @details
   Called whenever RFSW needs to update the HL/LL bit table to indicate
   that an ARFCN should use LL mode for Rx.
   @param
   logical device, arfcn, band

   @retval
   None
*/
boolean rfgsm_core_set_arfcn_to_low_lin ( rfm_device_enum_type rfm_dev, uint16 arfcn, rfcom_gsm_band_type band )
{
   uint8 bit = 0; //The bit index within the 8 bit entry
   uint8 entry = 0; //The index of the 8 bit entry that the desired bit lies within
   rfgsm_core_handle_type* core_handle_ptr = NULL;

   core_handle_ptr = rfgsm_core_handle_get( rfm_dev );

   if (core_handle_ptr == NULL)
   {
     MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!");
     return FALSE;
   }

   if ( rfgsm_core_locate_arfcn_in_high_lin_table(arfcn, band, &entry, &bit) == FALSE )
   {
     MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Failed to get bit/entry for ARFCN %d!", arfcn);
     return FALSE;
   }

   if ( (entry > core_handle_ptr->sawless_linearity_control.num_table_entries) || (bit > RFGSM_HL_TABLE_BITS_PER_INDEX))
   {
     MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"Out of bounds access attempted for ARFCN %d (Entry %d, Bit %d)!", arfcn, entry, bit);
     return FALSE;
   }

   /* Mask bit and clear */
   core_handle_ptr->sawless_linearity_control.rx_hl_flag_table[entry] &= ~(uint8)(0x1 << bit);

   return TRUE;
}
/*----------------------------------------------------------------------------*/
/*!
   @brief
   Helper function to return the bit and entry associated with a channel in
   the High Lin decision table

   @details
   Used for the common operation of locating the bit/entry of an ARFCN
   @param
   arfcn, band, pointer to entry var, pointer to bit var,

   @retval
   None
*/
boolean rfgsm_core_locate_arfcn_in_high_lin_table( uint16 arfcn, rfcom_gsm_band_type band, uint8 *entry, uint8 *bit )
{
  uint8 bit_in_entry = 0; //The bit index within the 8 bit entry
  uint8 entry_in_table = 0; //The index of the 8 bit entry that the desired bit lies within
  uint32 norm_chan = 0;  //The normalised channel index (since ARFCN within band does not start at 0)
  uint32 bit_in_table = 0;
  uint32 table_base = 0;

  if ( rfgsm_core_get_hl_table_base(band, &table_base) == FALSE )
  {
   MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Could not map base for band %d!", band);
   return FALSE;
  }

  /* Normalise the arfcn within the band */
  if ( rfgsm_core_get_norm_chan(band, &arfcn, &norm_chan) == FALSE )
  {
   MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Could not normalise ARFCN %d!", arfcn);
   return FALSE;
  }

  /* Need to access bit corresponding to normalised channel + base bit */
  bit_in_table = table_base + norm_chan;

  /* Determine which 8-bit index to access */
  entry_in_table = bit_in_table / RFGSM_HL_TABLE_BITS_PER_INDEX;

  /* Determine which bit in the indexed entry we need to mask */
  bit_in_entry = bit_in_table % RFGSM_HL_TABLE_BITS_PER_INDEX;

  *entry = entry_in_table;
  *bit = bit_in_entry;

   return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Wrapper function to pass the burst metrics down to the msm layer for processing
   in the linearity algorithm

   @details
   Used to wrap the msm layer algorithm
   @param
   arfcn, band, pointer to entry var, pointer to bit var,

   @retval
   None
*/
boolean rfgsm_core_process_burst_metrics( rfm_device_enum_type rfm_dev,
                                          rfgsm_band_type      band,
                                          uint16               arfcn,
                                          dBx16_T              rx_lvl_dbm,
                                          uint16               snr,
                                          uint16               jdet_value)
{
  int16 signed_jdet_value = 0;
  rfgsm_core_handle_type* core_handle_ptr = NULL;

  core_handle_ptr = rfgsm_core_handle_get( rfm_dev);

  if (core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return FALSE;
  }

  if ( core_handle_ptr->sawless_linearity_control.linearity_override != RFGSM_SAWLESS_NO_OVERRIDE )
  {
    /* An override has been set, do not update the linearity state via the algorithm */
    //MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "SAWless override (%d) set, metrics processing not run!", core_handle_ptr->sawless_linearity_control.linearity_override);
    return TRUE;
  }

  if ( jdet_value == RFLM_MSM_JDET_INVALID_VALUE )
  {
    /* Print invalid reading notification only if SAWless logging NV is enabled. */
    if ( core_handle_ptr->sawless_linearity_control.enable_logging == TRUE )
    {
      MSG_4(MSG_SSID_RF, MSG_LEGACY_MED, "Received invalid JDET reading (0x%x) for arfcn %d, band %d on dev %d, "
                                         "not processing metrics.", jdet_value, arfcn, band, rfm_dev );
    }
    /* Do not run algorithm if jdet_value is invalid */
    return TRUE;
  }

  //Shift the JDET reading up to get rid of the unwanted bits, remaining bits are in twos complement so cast to signed.
  signed_jdet_value = (int16)(jdet_value << RFGSM_MSM_JDET_MSB_DISCARD_BITS);

  //Right shift back as signed type to get in int16 format
  signed_jdet_value = signed_jdet_value >> RFGSM_MSM_JDET_MSB_DISCARD_BITS;

  if( rfgsm_msm_sawless_lin_update(rfm_dev,
                                   rfgsm_core_convert_rfband_to_rfcom(band),
                                   arfcn,
                                   rx_lvl_dbm,
                                   snr,
                                   signed_jdet_value) == FALSE )
  {
    return FALSE;
  }


  return TRUE;
}
#ifdef FEATURE_TX_SHARING
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function calls rfgsm_core functionality to create the RF CCS Events for tx enable.

  @param core_data      : rfgsm core structure that stores infomation like Multimode device
  @param tx_enable_ptr : Pointer to shared mem for tx enable
  @retval               : True or False
*/

boolean rfgsm_core_build_tx_enable_ccs_events( rfgsm_core_data_type* core_data,  rfm_device_enum_type rfm_dev)
{

  boolean ret_status = TRUE;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  rf_buffer_intf* tx_enable_ccs_scripts_taskq_b[] = {(rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.tx_enable_script[TX_PA_SCRIPT],
	                                                  (rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.tx_disable_script[TX_APT_SCRIPT],
                                                       NULL};

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!", FALSE);

  /* Program the tx event to data table */
  if( !rfgsm_core_dm_dynamic_common_event_wrapper( core_data,
                                            RFLM_GSM_TX_ENABLE_SCRIPT,
                                            RFLM_GSM_CCS_TASK_Q_B,
                                            (rf_buffer_intf**)tx_enable_ccs_scripts_taskq_b,
                                            sizeof(tx_enable_ccs_scripts_taskq_b)/sizeof((rf_buffer_intf**)tx_enable_ccs_scripts_taskq_b),
                                            NULL,
                                          0))
  {
    RF_MSG(MSG_LEGACY_ERROR,"RF GSM Tx Enable RFLM data info event create failed!");
	ret_status &= FALSE;
  }


  return ret_status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function calls rfgsm_core functionality to create the RF CCS Events for tx disable.

  @param core_data      : rfgsm core structure that stores infomation like Multimode device
  @param tx_disable_ptr : Pointer to shared mem for tx disable
  @retval               : True or False
*/

boolean rfgsm_core_build_tx_disable_ccs_events( rfgsm_core_data_type* core_data, rfm_device_enum_type rfm_dev)
{

  boolean ret_status = TRUE;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;


  rf_buffer_intf* tx_disable_ccs_scripts_taskq_b[] = {(rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.tx_disable_script[TX_PA_SCRIPT],
	                                                  (rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.tx_disable_script[TX_APT_SCRIPT],
													  (rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.tx_disable_script[TX_ATUNER_SCRIPT],
                                                       NULL};

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!", FALSE);

  /* Program the tx event to data table */
  if( !rfgsm_core_dm_dynamic_common_event_wrapper( core_data,
                                            RFLM_GSM_TX_DISABLE_SCRIPT,
                                            RFLM_GSM_CCS_TASK_Q_B,
                                            (rf_buffer_intf**)tx_disable_ccs_scripts_taskq_b,
                                            sizeof(tx_disable_ccs_scripts_taskq_b)/sizeof((rf_buffer_intf**)tx_disable_ccs_scripts_taskq_b),
                                            NULL,
                                          0))
  {
    RF_MSG(MSG_LEGACY_ERROR,"RF GSM Tx disable RFLM data info event create failed!");
	ret_status &= FALSE;
  }


  return ret_status;
}


#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  The function to update the Rx path in case of concurrency with another tech
  This Function should be called every Enter mode/Wakeup
  @details
  This function will be used to update the Rx path in case the default path is
  being used by another tech. The API will call the concurrency manager to
  get pointer to an alternative path, This will be followed by a call to the
  RFC , asking it to update the Rx path to the newly obtained one. When we tune
  we will be tuning to the new Rx path as opposed to the default one.
  This is done only for the Rx path, since there can be multiple Rx paths
  but there will only be a single Tx path

  @param Alternate path pointer
  Alt path pointer which will be used to update alt path in rfgsm core handles

  @param core_data
  rfgsm core data that contains information about rfm_dev

  @return success
  TRUE: Everything went well. Call successful.
  FALSE: Some error. Call unsuccessful.
*/
boolean rfgsm_core_update_alternate_rx_path_port_mapping( rfgsm_core_data_type *core_data,
                                                          uint8 *alt_path )
{
  /* RFC type band to be sent to the concurrency manager */
  rf_card_band_type rfc_band;
  /* Flag to check if the call to concurrency manager was success */
  rfcmn_status_type alt_path_status;

  /* GSM band for which alternate path needs to be updated
     For GSM, we'll be assigned same alternate path for all four bands */
  rfcom_gsm_band_type rfgsm_band;

  /*Return value for api*/
  boolean ret_val = FALSE;

   /* set the rfdevice port information for each supported band */
  for (rfgsm_band = (uint8)RFCOM_BAND_GSM850; rfgsm_band < (uint8)RFCOM_NUM_GSM_BANDS; rfgsm_band++)
  {
    /*Update rx alternate path for GSM bands supported by RFC*/
    if (RFGSM_CORE_RFC_BAND_SUPPORTED(core_data->rfgsm_core_handle_ptr->device_capability.rx_supported_band_mask, rfgsm_band))
    {
      /* Convert RFM band to RFC type */
      rfc_band = rfgsm_core_rfcom_gsm_band_to_rf_card_band( rfgsm_band );

      /* Get the alternate Rx path for this device and band */
      alt_path_status =  rfcmn_concurrency_mgr_update_rx_path( core_data->rfm_dev,
                                                               RFCOM_GSM_MODE,
                                                               (int)rfgsm_band,
                                                               alt_path );
	  RF_MSG_3(RF_LOW,"rfcmn_concurrency_mgr_update_rx_path: BAND %d,  alt_path_status %d, alt_path %d.", 
					rfgsm_band, alt_path_status, *alt_path);
					
      if ( alt_path_status == RFCMN_PATH_SEL_SUCCESS )
      {
	  	if(override_gsm_port_to_sawless==TRUE)
	  	{
	  	  *alt_path = ALTPATH_FOR_IIP2_ON_SAW_RFC;
	  	  RF_MSG_2(RF_HIGH,
	  			"rfgsm_core_update_alt_rx_path: alternate_rx_path is override to %d for IIP2 CAL. band %d.",
	  			*alt_path, rfgsm_band);
	  	}
        /* Update the port mapping with the alternate path information */
        ret_val = rfc_update_alt_port_mapping( core_data->rfm_dev,
                                               RFCOM_GSM_MODE,
                                               rfc_band,
                                               *alt_path );

        /* Error if path update failed since front end will not be
        configured as expected if this fails */
        if (ret_val == FALSE)
        {
          RF_MSG_2(RF_ERROR,"rfgsm_core_update_alt_rx_path: Failed to update "
                   "Alt port map for device:%d , band %d ",
                   core_data->rfm_dev, rfgsm_band );
        }
      }
      else
      {
        if (alt_path_status == RFCMN_PATH_SEL_NOT_SUPPORTED)
        {
          *alt_path = 0;
          ret_val = TRUE;
          RF_MSG_2(RF_HIGH,"rfgsm_core_update_alt_rx_path: alt rx path selection"
                   "is not supported for device :%d , band %d, default to alt path 0",
                   core_data->rfm_dev, rfgsm_band );
        }
        else
        /* Error fatal?? if path update failed since front end will not be
        configured as expected if this fails */
        {
          ret_val = FALSE;
          RF_MSG_2(RF_ERROR,"rfgsm_core_update_alt_rx_path:Failed to get "
                   "Alt port for device:%d , band %d ",
                   core_data->rfm_dev, rfgsm_band );
        }
      }
    }/*Band is supported in RFC*/
    else
    {
      RF_MSG_2(RF_HIGH,"GSM Band :%d is not supported for rfm_dev:%d, skipping setting alternate path",
               core_data->rfm_dev, rfgsm_band );
    }
  }
  return ret_val ;
}/* rfgsm_core_update_alt_rx_path */



/*----------------------------------------------------------------------------*/
/*!
  @brief rfgsm_core_update_irat_alternate_rx_path_port_mappin
  The function to update the Rx path in case of concurrency with another tech
  This Function should be called every IRAT meas Enter
  @details
  This function will be used to update the Rx path in case the default path is
  being used by another tech. The API will call the concurrency manager to
  get pointer to an alternative path, This will be followed by a call to the
  RFC , asking it to update the Rx path to the newly obtained one. When we tune
  we will be tuning to the new Rx path as opposed to the default one.
  This is done only for the Rx path, since there can be multiple Rx paths
  but there will only be a single Tx path

  @param rfm_dev
  Device on which GSM subsystem is to be entered

  @param Alternate path pointer
  Alt path pointer which will be used to update alt path in rfgsm core handles

  @param rfgsm_band
  /* GSM band for which alternate path needs to be updated

  @return success
  TRUE: Everything went well. Call successful.
  FALSE: Some error. Call unsuccessful.
*/
boolean rfgsm_core_update_irat_alternate_rx_path_port_mapping( rfm_device_enum_type rfm_dev,
                                                               uint8* alt_path,
                                                               uint8 rfc_rx_supported_band_mask)
{
  /* RFC type band to be sent to the concurrency manager */
  rf_card_band_type rfc_band;
  /* Flag to check if the call to concurrency manager was success */
  rfcmn_status_type alt_path_status;

  /* GSM band for which alternate path needs to be updated
     For GSM, we'll be assigned same alternate path for all four bands */
  rfcom_gsm_band_type rfgsm_band;

  /*Return value for api*/
  boolean ret_val = FALSE;

   /* set the rfdevice port information for each supported band */
  for (rfgsm_band = (uint8)RFCOM_BAND_GSM850; rfgsm_band < (uint8)RFCOM_NUM_GSM_BANDS; rfgsm_band++)
  {
    if (RFGSM_CORE_RFC_BAND_SUPPORTED(rfc_rx_supported_band_mask, rfgsm_band))
    {
      /* Convert RFM band to RFC type */
      rfc_band = rfgsm_core_rfcom_gsm_band_to_rf_card_band( rfgsm_band );

      /* Get the alternate Rx path for this device and band */
      alt_path_status =  rfcmn_concurrency_mgr_update_irat_rx_path( rfm_dev,
                                                                    RFCOM_GSM_MODE,
                                                                    (int)rfgsm_band,
                                                                    alt_path );

      if ( alt_path_status == RFCMN_PATH_SEL_SUCCESS )
      {
        /* Update the port mapping with the alternate path information */
        ret_val = rfc_update_alt_port_mapping( rfm_dev,
                                               RFCOM_GSM_MODE,
                                               rfc_band,
                                               *alt_path );

        /* Error if path update failed since front end will not be
        configured as expected if this fails */
        if (ret_val == FALSE)
        {
          RF_MSG_2(RF_ERROR,"rfgsm_core_update_alt_rx_path: Failed to update "
                   "Alt port map for device:%d , band %d ",
                   rfm_dev, rfgsm_band );
        }
      }
      else
      {
        if (alt_path_status == RFCMN_PATH_SEL_NOT_SUPPORTED)
        {
          *alt_path = 0;
          ret_val = TRUE;
        }
        else
        /* Error fatal?? if path update failed since front end will not be
        configured as expected if this fails */
        {
          ret_val = FALSE;
          RF_MSG_2(RF_ERROR,"rfgsm_core_update_alt_rx_path:Failed to get "
                   "Alt port for device:%d , band %d ",
                   rfm_dev, rfgsm_band );
        }
      }
    }/*Band is supported in RFC*/
    else
    {
      RF_MSG_2(RF_HIGH,"GSM Band :%d is not supported for rfm_dev:%d, skipping setting alternate path",
               rfgsm_band, rfm_dev );
    }
  }
  return ret_val ;
}/*rfgsm_core_update_irat_alternate_rx_path_port_mapping */



/*===========================================================================*/
/*!
  @brief
  Function to get alternate path for a given logical device. This Function should be called
  every RX burst
  @details
  Function to get alternate path per GSM band for a given logical device which require
  a band port map update to set up a new rf path

  @param rfm_dev
  current logical device for which path update is requested

  @param rf_band
  current tech specific band which needs alt path update

  @retval ret_val
  TRUE indicates band alt path retrieval is successful, else return FALSE

*/
boolean rfgsm_core_get_alternate_rx_path_per_band( rfm_device_enum_type rfm_dev,
                                                   rfcom_gsm_band_type rfgsm_band,
                                                   uint8* alt_path)
{
  boolean ret_val = FALSE;
  /* Flag to check if the call to concurrency manager was success */
  rfcmn_status_type alt_path_status;


  /* Get the alternate Rx path for this device and band */
  alt_path_status =  rfcmn_concurrency_mgr_update_rx_path( rfm_dev,
                                                           RFCOM_GSM_MODE,
                                                           (int)rfgsm_band,
                                                           alt_path );

  if ( alt_path_status == RFCMN_PATH_SEL_SUCCESS || alt_path_status == RFCMN_PATH_SEL_NOT_SUPPORTED)
  {
    /*incase of path not supported, concurrency manager returns default path*/
    ret_val = TRUE;
  }
  else
  /* Error fatal if path update failed since front end will not be
  configured as expected if this fails */
  {
     ret_val = FALSE;
     RF_MSG_2(RF_ERROR,"rfc_card gsm :update_alt_rx_path:Failed to get "
              "Alt port for device:%d , band %d ",
              rfm_dev, rfgsm_band );
  }
  return ret_val;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function gets the time it takes to execute scripts for
   enter-mode buffer for both RFFE nad SSBI.

   @param
   rf_buffer_intf * - pointer to script buffer

   @retval
   rf_wakeup_execution_time_us - The execution times required for script. Assuming
   no start offsets and we check if it is RFFE or SSBI based on flag ssbi_protocol_supported
   If SSBI is present: rf_gsm_init_time_us = ((num_ssbi_trans * SSBI_EXEC_TIME) + ROUNDING_OFFSET) ;
   If RFFE is present: rf_gsm_init_time_us = ((num_rffe_trans * RFFE_MULTI_BYTE_EXEC_TIME) + ROUNDING_OFFSET) ;

*/

uint32 rfgsm_core_get_ccs_exe_us( rfm_device_enum_type rfm_dev, rf_buffer_intf *script_ptr)
{
  uint16 num_trans = 0;
  uint32 rf_gsm_init_time_us = 0;
  rfgsm_core_handle_type* rfgsm_core_handle_ptr = NULL;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!", 0);

  if (rfgsm_core_handle_ptr->ssbi_protocol_supported)
  {
    num_trans = rf_buffer_get_num_trans(script_ptr, RF_BUFFER_SSBI);
    rf_gsm_init_time_us = (num_trans * SSBI_EXEC_TIME) + ROUNDING_OFFSET;
  }
  else
  {
    num_trans = rf_buffer_get_num_trans(script_ptr, RF_BUFFER_RFFE);
    rf_gsm_init_time_us = (num_trans * RFFE_MULTI_BYTE_EXEC_TIME) + ROUNDING_OFFSET;
  }

  return rf_gsm_init_time_us;
}


/*!
   @brief
   This function appends the Device Status Read Scripts to the Rx/Tx Stop Scripts
   depending upon the argument

   @param
   core_data* - pointer to core_data
   burst_type - variable to indicate burst_type ( 0: Rx_Burst, 1: Tx_Burst)
*/
void rfgsm_core_populate_device_status_read_buffers(rfgsm_core_data_type *core_data, rf_burst_type burst_type)
{
  uint8 index;
  /* Set the GFW therm read trigger UMID based on current context */
  switch (rfgsm_core_get_task_num())
  {
      case RF_TASK_1:
        core_data->rfgsm_core_handle_ptr->device_config_to_read.device_status_rsp_umid = RFLM_GSM_DEVICE_STATUS_READ_RSP;
        break;
    #ifdef FEATURE_DUAL_SIM
      case RF_TASK_2:
        core_data->rfgsm_core_handle_ptr->device_config_to_read.device_status_rsp_umid = RFLM_GSM_DEVICE_STATUS_READ_SUB2_RSP;
        break;
    #endif
	  default :
	    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR," Task %d is not supported. If required enable it",rfgsm_core_get_task_num());
		return;
	  break;
      /*! @todo add 3rd task here */
  }

  for(index = 0; index < core_data->rfgsm_core_handle_ptr->device_config_to_read.params[burst_type].num_regs; index++)
  {
    if(burst_type == RF_TX_BURST)
    {
      if(core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_TX_BURST].resource_type[index] == RF_HAL_BUS_RESOURCE_RFFE)
      {
        rf_buffer_append_rffe((rf_buffer_intf*)core_data->rfgsm_core_handle_ptr->scripts.tx_stop_script[TX_WTR_SCRIPT],
                              core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_TX_BURST].slave_id[index],
                              core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_TX_BURST].channel[index],
                              core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_TX_BURST].address[index],
                              0,
                              RF_BUFFER_READ,
                              RFGSM_CORE_DEVICE_STATUS_READ_TRIGGER_OFFSET,
                              core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_TX_BURST].extended_cmd[index],
                              core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_TX_BURST].half_rate[index],
                              core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_TX_BURST].rd_delay[index],
                              TRUE);
      }

      if(core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_TX_BURST].resource_type[index] == RF_HAL_BUS_RESOURCE_SSBI)
      {
        rf_buffer_append_ssbi_with_new_time((rf_buffer_intf*)core_data->rfgsm_core_handle_ptr->scripts.tx_stop_script[TX_WTR_SCRIPT],
                                            core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_TX_BURST].channel[index],
                                            core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_TX_BURST].address[index],
                                            0xFF,
                                            RF_BUFFER_READ,
                                            RFGSM_CORE_DEVICE_STATUS_READ_TRIGGER_OFFSET,
                                            TRUE);
      }
    }

    if (burst_type == RF_RX_BURST)
    {
      if(core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_RX_BURST].resource_type[index] == RF_HAL_BUS_RESOURCE_RFFE)
      {
        rf_buffer_append_rffe((rf_buffer_intf*)core_data->rfgsm_core_handle_ptr->scripts.rx_stop_script[RX_WTR_SCRIPT],
                               core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_RX_BURST].slave_id[index],
                               core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_RX_BURST].channel[index],
                               core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_RX_BURST].address[index],
                               0,
                               RF_BUFFER_READ,
                               RFGSM_CORE_DEVICE_STATUS_READ_TRIGGER_OFFSET,
                               core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_RX_BURST].extended_cmd[index],
                               core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_RX_BURST].half_rate[index],
                               core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_RX_BURST].rd_delay[index],
                               TRUE);
      }

      if(core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_RX_BURST].resource_type[index] == RF_HAL_BUS_RESOURCE_SSBI)
      {
        rf_buffer_append_ssbi_with_new_time((rf_buffer_intf*)core_data->rfgsm_core_handle_ptr->scripts.rx_stop_script[RX_WTR_SCRIPT],
                                            core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_RX_BURST].channel[index],
                                            core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_RX_BURST].address[index],
                                            0xFF,
                                            RF_BUFFER_READ,
                                            RFGSM_CORE_DEVICE_STATUS_READ_TRIGGER_OFFSET,
                                            TRUE);
      }
    }
  }
}

/*!
  @brief
  program activated QFE antenna tuner devices for detune

  @details
  This API will be called to program the tuner immediately and no script or FW
  action is required. Inside this API check all the active paths then give a call to
  tuner manager associated with appropriate tuners

  @return
  Flag indicating if the api operation was successful (TRUE) or failed (FALSE)

*/

boolean rfgsm_core_antenna_tuner_program_qfe_scenario ( void )
{
  rfc_rxtx_enum_type rx_tx;
  boolean is_rx_mode = FALSE;
  boolean status = TRUE;
  boolean dev_status = FALSE;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  rfcommon_atuner_intf* atuner_obj = NULL;
  rfcom_band_type_u rfcom_band;
  rfcom_gsm_band_type band;
  rfm_device_enum_type rfm_dev;

  script_token_type script_token;

  uint16 arfcn;
  uint32 local_instance = 0;

  rfcommon_atuner_intf* atuner_obj_done[5] = {NULL};
  int atuner_obj_done_idx = 0;
  int i;

  /* RFM_DEVICE_4 is actually maximum number of devices io RFM_MAX_WAN_DEVICES RFDEVICE_TYPE_MAX_NUM */
  for(rfm_dev = RFM_DEVICE_0; rfm_dev < RFM_DEVICE_4; rfm_dev++)
  {
    rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

    if(rfgsm_core_handle_ptr == NULL)
    {
      continue;
    }

    for(rx_tx = 0; rx_tx < RFC_CONFIG_RXTX_MAX; rx_tx++)
    {
      for (band = 0; band < RFCOM_NUM_GSM_BANDS; band++)
      {
        arfcn = (uint16) RF_INVALID_VALUE_16BIT;
        rfcom_band.gsm_band = band;
        is_rx_mode = TRUE;

        if(band == rfgsm_core_get_tx_band(rfm_dev))
        {
          is_rx_mode = FALSE;
          rfcom_band.gsm_band = band;
          arfcn = rfgsm_core_handle_ptr->rf_tx_arfcn;

          MSG_5(MSG_SSID_RF, MSG_LEGACY_MED,
                "rfgsm_core_antenna_tuner_program_qfe_scenario():in tx band:"
                "rfm_dev=%d, rx_tx=%d, is_rx_mode=%d, band=%d, arfcn=%d",
                rfm_dev, rx_tx, is_rx_mode, rfcom_band.gsm_band, arfcn);
        }

        if(arfcn == (uint16) RF_INVALID_VALUE_16BIT
           && band == rfgsm_core_get_rx_band(rfm_dev))
        {
          is_rx_mode = TRUE;
          rfcom_band.gsm_band = band;
          arfcn = rfgsm_core_handle_ptr->rf_rx_arfcn;

          MSG_5(MSG_SSID_RF, MSG_LEGACY_MED,
                "rfgsm_core_antenna_tuner_program_qfe_scenario():in rx band:"
                "rfm_dev=%d, rx_tx=%d, is_rx_mode=%d, band=%d, arfcn=%d",
                rfm_dev, rx_tx, is_rx_mode, rfcom_band.gsm_band, arfcn);
        }

        if(arfcn == (uint16) RF_INVALID_VALUE_16BIT)
        {
          /* channel was not assigned so the likely device was not assigned */
          continue;
        }

        /* As GSM does not support Carrier Aggregation(CA) of Tunner Manager, hence all the RFM paths
         *   make requests to tuner manager using port-0. we shall request only one instance.
         */
        for (local_instance = 0; local_instance < RFC_MAX_DEVICE_INSTANCES_PER_BAND; local_instance++)
        {
          atuner_obj = (rfcommon_atuner_intf*) rfc_gsm_get_rf_device_obj(rfm_dev,
                                                                         rx_tx,
                                                                         rfcom_band.gsm_band,
                                                                         RFDEVICE_TUNER_MANAGER, local_instance);
          if(atuner_obj == NULL)
          {
            local_instance = RFC_MAX_DEVICE_INSTANCES_PER_BAND;
            continue;
          }
          /* search for antenna tuner devices already processed */
          for (i = 0; i < 5 && i < atuner_obj_done_idx; i++)
          {
            if(atuner_obj == atuner_obj_done[i])
            {
              break;
            }
          }

          if(i == atuner_obj_done_idx)
          {
            if(i < 5)
            {
              atuner_obj_done[i] = atuner_obj;
              atuner_obj_done_idx++;
            }
            /* or else the stack is full. We will likely redo an harmless detune */
          }
          else
          {
            MSG_5(MSG_SSID_RF, MSG_LEGACY_MED,
                  "rfgsm_core_antenna_tuner_program_qfe_scenario():device already detuned:"
                  "rfm_dev=%d, rx_tx=%d, is_rx_mode=%d, band=%d, arfcn=%d",
                  rfm_dev, rx_tx, is_rx_mode, rfcom_band.gsm_band, arfcn);
            continue;
          }

          dev_status = rfcommon_atuner_manager_force_detune(atuner_obj,
                                                            RFCOM_GSM_MODE,
                                                            rfcom_band,
                                                            arfcn,
                                                            is_rx_mode,
                                                            RFDEVICE_EXECUTE_IMMEDIATE, NULL, 0, &script_token);
          if(dev_status == FALSE)
          {
            MSG_5(MSG_SSID_RF, MSG_LEGACY_ERROR,
                  "rfgsm_core_antenna_tuner_program_qfe_scenario():"
                  "failed detune for rfm_dev=%d, rx_tx=%d, is_rx_mode=%d, band=%d, arfcn=%d",
                  rfm_dev, rx_tx, is_rx_mode, rfcom_band.gsm_band, arfcn);
          }
          status &= dev_status;
        }
        status &= dev_status;
      }
    }
  }

  return status;
} /* rfgsm_core_antenna_tuner_program_qfe_scenario  */


/*!
  @brief
  This API checks the rx timing info in core_handle and ASSERT when timing is wrong

  @details
  This API checks the rx timing info for each rfm_device and find the worst case timing,
  and ASSERT if worse timing is wrong

  @return
  void

*/
void rfgsm_core_validate_rx_timing_info(void)
{
  rfm_device_enum_type rfm_dev;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  rfc_gsm_core_config_type *rfc_gsm_core_config_ptr = NULL;
  uint16 worst_case_num_wtr_writes_before_pll = 0;
  uint16 worst_case_pll_settling_time_qs = 0;

  for (rfm_dev = 0; rfm_dev < RFM_MAX_WAN_DEVICES; rfm_dev++)
  {
    rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
    RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!");

    rfc_gsm_core_config_ptr = rfgsm_core_handle_ptr->rfc_gsm_core_ptr;
    RF_NULL_CHECK_RTN_V( rfc_gsm_core_config_ptr, "rfgsm_core_validate_rx_timing_info(): rfc_gsm_core_config_ptr==NULL");

    if (rfc_gsm_core_config_ptr->rx_timing->rx_burst_sbis_before_pll > worst_case_num_wtr_writes_before_pll)
    {
      worst_case_num_wtr_writes_before_pll = rfgsm_core_handle_ptr->rfc_gsm_core_ptr->rx_timing->rx_burst_sbis_before_pll;
    }

    if (rfc_gsm_core_config_ptr->rx_timing->pll_settling_time_qs > worst_case_pll_settling_time_qs)
    {
      worst_case_pll_settling_time_qs = rfgsm_core_handle_ptr->rfc_gsm_core_ptr->rx_timing->pll_settling_time_qs;
    }
  }

  ASSERT(worst_case_num_wtr_writes_before_pll != RFGSM_MIN_RX_PLL_AUTOB_WRITES );
  ASSERT(worst_case_pll_settling_time_qs >= RFGSM_MIN_RX_PLL_SETTLING_TIME );

}

/*----------------------------------------------------------------------------*/
/*
  @brief
  This API is called to initialize Rx Debug Logging vars in core handle from the Debug NV
  while initializing.

  @params
  rfm_device_enum_type rfm_dev    device type determining the device being used

  @retval
  boolean status                  determining the success of initialisation
*/
boolean rfgsm_core_handle_rx_debug_logging_enable_init(rfm_device_enum_type rfm_dev)
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr;
  /* get handle for this device*/
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return FALSE;
  }

  rfgsm_core_handle_ptr->rx_debug_logging_enable = rfgsm_nv_check_debug_nv(rfm_dev, RFGSM_DEBUG_NV_RX_DEBUG_LOGGING_ENABLE);;

  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function updates the frequency error (in ppb).
   For example, if user passes a value of 2054 (ppb) it corresponds to 2054/1024 = 2.005 ppm. The resolution of frequency error is hence .001 ppm.

   @details

*/
void rfgsm_core_update_freq_error(rfgsm_core_data_type *core_data, int32  freq_err)
{
  core_data->rfgsm_core_handle_ptr->rfgsm_curr_freq_error = freq_err;

  /* update the device layer with gsm freq error information*/
  rfdevice_gsm_cmd_dispatch( core_data->rfm_dev, RF_PATH_0, RFDEVICE_GSM_SET_FREQ_ERROR, &freq_err);
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function returns the value of frequency error (in ppb)
   @details

   @param

   @retval

*/
int32 rfgsm_core_get_freq_error(rfgsm_core_handle_type *rfgsm_core_handle_ptr)
{
  return rfgsm_core_handle_ptr->rfgsm_curr_freq_error;
}


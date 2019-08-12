/*===========================================================================

            G S M   R F   C O N T R O L   F U N C T I O N S

DESCRIPTION
   This file contains implementations for FTM GSM RF control test functions

   Copyright (c) 2001 - 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  Initially, comments will be entered manually. Later, as the module is
  placed under version control, the version control tool will automatic-
  ally insert change comments into this section. [Delete this comment].

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/ftm/src/ftm_gsm_rfctl.c#1 $
  $DateTime: 2016/12/13 07:59:51 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/27/16   mz      FR 30990: SAWless LTE TDD B39: enable IIP2 CAL on GSM SAW device
12/01/15   bp      Disable temperature/vbatt compensation during IIP2 calibration
10/13/15   bp      raised ftm_gsm_get_frame_sweep_cal_running() to its meaning
09/14/15   par     Fix crash in FTM GSM deact sequence
01/16/14   ec      Use new debug NV mechanism for KV cal log enabling
12/12/14   tws     Translate between FTM and RF_HAL enum correctly
12/05/14   sml     Fix KW
12/02/14   sml     Read WTR or RF front end devices status in between Rx/Tx
                   burst along with GRFC status
11/27/14   tws     Mainline the call to geran_test_mode_api_do_ip2cal()
11/20/14   sw/sc   Deprecate legacy state machines and move cmd buff out of rfa
11/20/14   zbz     Fix KW
11/14/14   ck      add api to set gsm diversity
10/26/14   cpm     advanced set_band by one frame
10/23/14   sc      Enable event type control from RFSW
10/01/14   sb      Enhance IP2 F3 messages for improved logging
09/19/14   sb      Add IP2 cal moving averaging + pass/fail status
08/20/14   ndb     Add Tuner_ID as part of the response in "FTM_SET_TUNER_TUNE_CODE_OVERRIDE"
08/08/14   av      removed unused variable declaration.
08/01/14   ec      Add code to handle V4 cal for SAWless targets
08/01/14   lcl     OFT compilation fix
07/30/14   hoh     Add enhanced rx feature support
07/25/14   sb      bug fixes for V1 IP2 cal functionality
07/14/14   ec      Add FTM apis to control SAWLess linearity
07/11/14   zbz     Rx/Tx disable ends at block limit during band change in sweep handler
07/10/14   zbz     Delay Tx disable during band change for 2 frames for frame sweep
07/10/14   zbz     Return RSSI/LNA for Rx diversity chain during sweep cal
07/09/14   zbz     Fix crash in GSM FTM RF test
07/08/14   ck      Fix to correctly update mod_type in FTM GSM tx burst
07/03/14   jr      Enable therm read on sub-2
07/02/14   tws     Remove geran ip2 call from MSM8909 until ready
06/17/14   zbz     Fix bug that prevents tags_wr_index from being reset to 0
06/10/14   hk      sub_id interface in FTM mode
05/29/14   sb      IP2 cal bug fixes, updates
04/30/14   tsr     Remove RXD fields from gsm rx sweep results
04/23/14   tsr     Access FTM variables only for PRX device
04/22/14   ggs     Port CR636669: Set flag if frame sweep calibration is running.
04/16/14   tsr     GSM RX Diversity support GL1 dependant
04/16/14   sb      IP2 cal Interface and functionality changes
03/31/14   tsr     Add support for GSM RX Diversity
03/26/14   sb      Add support for IP2 cal for Dime PM
02/28/14   sc      Pass core data through mc to core layers
02/17/14   sc      Clean up mDSP APIs
12/16/13   ry      Fix OFF Target build errors
12/13/13   ry      Remove hardcoding of RFM_DEVICE_0
11/05/13   ggs     Various changes required to support Bolt modem:
                   Allocate/De-allocate DM memory for Tx calibration
11/05/13   ec      Remove empty parameters from F3 messages to save memory
09/13/13   jr      Added critical section to protect isr_task and ftm_task msgr clients
09/11/13   aka     fix to not use default values of mod in non-mod4 frames
09/10/13   sml     Fixing KW
08/15/13   ck      Add null check to prevent crash when calling kv cal for wtr1625
08/09/13   aka     initialize SMPS bias for TX_RGI_MEAS_OP in a frame_Sweep
                   Removed GSM frame alignment for single chain sweep on rfm_device 2
08/08/13   ec      Call new API to log IQ samples to a specified sub
08/07/13   ec	   Stub out as_id parameter temporarily to fix compiler warning
07/30/13   ec	   Use MSGR to configure IQ capture
07/18/13   tsr     Fix PA Range ftm slot override bug
07/12/13   aka     Turn off all Tx slots before starting Frame sweep
06/25/13   aka     dBm based pwr control cmd for GSM frame sweep
06/11/13   ec      Fix linear RGI override
06/10/13   cpm     preferred use of memscpy()
05/31/13   tsr     Fixed the bug in Continuous RX OFF which was causing crash when
                   Cont Rx was turned back on after turning Cont. RX off
05/23/13   ck      Fix corrupted first IQ samples
05/09/13   tsr     Fix and Enable Multislot RX Calibration
05/07/13   sb      Provide workaround to get KV2 cal working
05/06/13   aka     FTM frame sweep feature enhancements
04/23/13   ck      Add FTM IQ capture APIs to move from HWTC to FTM
04/22/13   sb      Add FTM hooks to override QPOET bias
04/18/13   sb      Add FTM hooks for overriding envelope gain, PA scaling and SMPS bias
04/16/13   svi     Replaced memcpy() by safer memscpy().
04/03/13   ec	   Initialise ftm_gsm_rfctl_vars if they are not initialised when retrieved.
04/04/13   aka     Added handling of stop_tx cmd for cont Tx case
04/02/13   aka     Support for band change in frame sweep functionality.
04/01/13   aka     Modified GSM acq type to GL1_HW_ACQ_GSM_XOCAL in ftm_gsm_do_rx_cmd_rx_cont
03/28/13   aka     GSM frame sweep feature changes.
03/29/13   ctk     Enable freq error xo reading from tcxo manager api
03/27/13   sb      Added support for Enhanced APT
03/25/13   ec 	   Fix KW error in RGI code
03/21/13   ec      Fix a bug where Linear RGI could not be adjusted for slots
                   4 and above
02/27/13   tws     Move static variables to device specific structure.
02/22/13   tws     Make the IQ capture buffer dynamic.
02/21/13   tws     Tidy up FTM variable initialisation.
02/19/13   tsr     Disable mulitslot RX during RX Calibration
02/15/13   sb      Added FTM hook to override tuner tune code
02/14/13   aka     Initialized rfm_dev field FTM handle with correct device
02/05/13   ec      Fix Klockwork errors
01/29/13   ec  	   Added in framework for multiple devices in Non Signalling
01/2/13    aka     Changes to support multi chain GSM operation in FTM mode
01/17/12   aka     Changes to enable freq offset setting for multiple chains
01/09/13   sr      Included temp comp and vbatt comp header files.
12/18/12   tws     Fix Klocwork error.
12/13/12   aka     scale back the GSM/edge tx gain values before returning
12/11/12   sb      Add hooks for GSM APT
12/10/12   tws     Path delay variables moved to core.
12/06/12   tws     Fix Klocwork errors.
12/05/12   aka     Added rfm_dev in ftm_gsm_get_env_gain_nv
12/05/12   tws     Use device2_as_id to get the correct ID for GL1. Mainline
                   the RF task context flag.
12/03/12   aka     Changes to use NV ptr from rfgsm_core_handle_ptr
11/30/12   tws     Remove AS_ID mapping until dependency is fixed.
11/30/12   tws     Fix off-target compile errors.
11/29/12   tws     Add device id & as_id to FTM. Make globals device dependent.
11/28/12   aka     Changes to use rfgsm_core_handle_ptr from path based handler
11/26/12   aka     Changes to use a path specific gsm handle
11/23/12   tws     Remove deprecated GERAN includes.
11/21/12   av      Added rfm_device parameters to mc/core/mdsp/msm layers.
11/19/12   aka     Added ftm_gsm_get_env_gain_nv to return NV value
11/13/12   ec	   Update to use new geran api for DSDA
11/05/12   ec      Add extern prototypes of GL1 functions to fix compiler warnings
10/16/12   tsr     Cleanup GSM bringup flags
09/28/12   ec      Remove redundant function to set tx alpha in FTM
10/10/12   ec      Changing name of prepare_tx_burst to reflect usage
10/01/12   sb      Made changes to add rfm device to rf device interface and uppler layer APIs
09/25/12   tc      Mainline RF_HAS_RFA_GSM_TASK.
09/18/12   tc      Remove FEATURE_RF_HAS_QFE1320-related functionality.
09/06/12   sr      Fixed slot based RGI setting.
08/17/12   ec      Fixing compilation error
08/16/12   ec      Further cleanup of set pa range function
08/09/12   ec      Checked in front end for first part of FTM tx refactoring, certain commands
                   will now signal RF through MSGR if RF Task is defined.
08/09/12   ggs     Cleaned up references to pa_r0_current_state and pa_r1_current_state
08/08/12   ec      Account for changes made to set_pa_range_cmd_type
08/07/12   ec      Unstub GL1 calls for prepare_tx_burst, make ftm_gsm_rf_call_in_rf_task_context
                   feature switched
08/02/12   tsr     Changes for RUMI Bringup
07/31/12   sc      Remove the use of non-DIME mDSP APIs under RF TASK
07/30/12   sc      Using temporary gain parameter struct to enable FW SU release
07/20/12   sr      Made changes to pass the RFM_DEVICE to rfc gsm functions
07/18/12   ec      Added new set_tx_pow functionality, to use MSGR to comm with RF Task
07/05/12   ec      Modified ftm_gsm_prepare_tx_burst to use gl1 function, so that RF task does
                   not break cont tx
06/25/12   ggs     Framework to use MSGR to send message from ftm to rf task
                   to set pa range
07/13/12   sb      Made changes to support 4 RSSI measurements per GSM burst in FTM_RX_GAIN_RANGE_CAL
06/21/12   ars     Update FTM_SET_FREQ_ADJUST command to use new TCXOMGR API for blocking therm read
05/16/12   zc      Adding variable delay to ftm_get_rssi_dbx16 command
05/15/12   sb      Added changes for debug
05/14/12   zc      Adding changes to make FTM frame matrix threadsafe
05/07/12   zw      Fixed off-target compiler error
04/18/12   ars     Modifed FTM frequency adjust logic to apply correction using XO manager API
04/26/12   jps     Put back changes done in CL 2371944
04/26/12   jps     Temporary remove changes done in CL 2371944
04/26/12   sb      Add FTM recovery from RXFE stuck
04/18/12   jps/lcl PA Range and PA R0/R1 logic decoupling
04/09/12   sb      Add changes to fix frequency error adjustment in FTM mode
04/01/12   lcl     Removed empty mdsp interface header file
03/23/12   sr      Removed the FEATURE_GSM_MDSP_QDSP6 featurization.
03/17/12   jfc     Remove deprecated mdsp_intf_v.h include
03/08/12   zc    Setting the current rx channel in ftm_gsm_core_setup_rx_burst_calver
03/07/12   jps/lcl temporary workaround to un-gate bringup
02/28/12   dej     Hooked up PA start/stop delta adjustments in FTM mode
02/28/12   sb      Added changes to support disabling Idle frame in FTM mode
02/17/12   sn      Added support for SMPS PDM control in FTM mode
01/25/12   sty     Fixed compile error on offtarget
12/20/11   sn      Added support for QFE1320
01/12/11   sr      Added include file "ftm_task.h" file fix compile error.
12/07/11   sb      Support for GSM IQ capture in Online and FTM mode
11/29/11   sb      Added functionality for GSM IQ CAPTURE
11/17/11   sb      Added FTM function to update frequency error
10/28/11    sr     made changes use the tx_nv_tbl ptr for any tx NV items
                   and rx_nv_tbl for any rx NV items.
09/07/11   sb/rsr  Changes for Delay match setting to use new FW interface.
08/31/11   vrb     Moving ftm_gsm_kv_cal_ave functionality into device driver
08/15/11   sr      klockwork fixes.
08/06/11   sr      RFC Cleanup: removed the include file.
08/02/11   sr      removed un-used code.
07/11/11    sr     removed un-used code.
07/07/11   vrb     Move device specific Tx DPLL Training Sequence into Device Driver
                   Support for KV Cal V2
07/01/11   vrb     Replacing direct calls to rfgsm_ds with wrapper functions
04/04/11   bmg     Fixed rfgsmlib_get_kv_cal_code_type to match actual use.
03/30/11   vrb     Added mdsp FW Interface change support.
03/21/11   sr      Added mdsp FW Interface change support.
03/16/11   sr      Removed un-used code.
03/11/11   sr      Removed un-used featurization
03/04/11    sr     merge fixes
03/03/11    sr     merge from mdm9k branch
31/01/11   sr      Removed un-used code
01/25/11   sr      removed the clk.h dependency and commented HWIO writes
                   because of compiler err and need to fixed later.
01/18/11   dej     Mainline RF_HAS_VOYAGER2_SUPPORT
01/17/11   dej     Mainline RF_DEVICE_MERGE_INTERIM_FEATURE
12/15/10   av      RFGSMCTL files cleanup
10/15/10   te      Remove unused fw shared variables CR 259435
10/08/10   ggs     CR257912 fix pulled in from QSC6295:
                   Made changes to turn-off the cont_tx in tdma_isr instead of from FTM task
10/07/10   lcl     Pull in RX calibration crash from QSC6295
09/24/10   rsr     Fixes for multislot RX. Fixes CR 257389
09/20/10   lcl     Pull in changes from QSC6295 for KV calibration fix
08/20/10   tsr     Remove INTLOCK/INTFREE code and fixed LINT errors
08/12/10   rsr     Removing reference to unused Headers for QDSP6 targets.
08/11/10   ggs     Changed Tx alpha calculation to help with pre-distortion calibration
08/05/10   lcl     Open up KV limit from 8 to 40, per QSC6295 recommendation.
08/03/10   rsr     Kw fixes.
07/27/10   ggs     Fixed compiler warnings
06/24/10   rsr     SCMM/MDM9k Merge support.
04/20/10   tsr     Added Voyager 2.0 Support
04/14/10   rsr     Fix compile warnings and lint.
04/13/10   rsr     Fix compile error.
04/05/10   rsr     Added feature for Multislot RX.
04/01/10   dnn     Added RF_HAS_VOYAGER2_SUPPORT to avoid compilation issue.
03/24/10   lcl     Added RF_HAS_VOYAGER2_SUPPORT to avoid compilation issue.
03/09/10   tsr     Added Function pointer for set_slot_override_flag and get_pa_range
03/09/10   tsr     Stubbed out RFGSM_GRFC_OMODE
02/09/10   tws     Fix race condition on multi-core processor.
01/20/10   tws     Support QDSP6 GSM FW.
01/06/10   sar     Renamed ftm_task.h to rf_test_task_v.h.
12/28/09   sr      High/Medium lint errors fix
12/22/09   sr      changed the debug msg macro from legacy to new one.
12/05/09   sr      Removed un-used code.
30/11/09   ntn     Code cleanup - changed kv_code_15bit to uint16
12/02/09   sr      Use the rfgsm_card.band for set_rgi() instead of tx_band.
11/30/09   jps     Mainlined FEATURE_GSM_PA_TEMP_COMP
11/18/09   jps     Updated parameter passed into get_kv_code function
10/26/09   jps     Removed feature FEATURE_RF_VBATT_TX_PWR_COMP
10/26/09   sr      Compiler warnings fix
10/21/09   sr      new device driver interface changes.
10/12/09   sr      Fixed warnings.
09/30/09   rsr     Added functionality to log burst metrics in FTM RX.
                   Bug fix in FTM_GSM_SET_LINEAR_RGI function
09/02/09    sr     changed the rfc_card_gsmctl_??? function calls to goes through
                   rfgsm_ctl function ptrs
07/27/09   rsr     CR181048 fix,code cleanup.
07/04/09   sr      removed the un-used code.
06/26/09   jps     Updated KV Cal algorithm
06/06/09   sr      Ported the continuous Rx functionality from LCU
04/02/09   sr      fixed kv_infor variable reference, which was not needed
04/01/09   sr      include file clean-up and replaced old function calls with
                   new ones
03/17/09   sr      Modified for rfnv name change.
10/28/08   cn      Fixed compiler warnings-enumerated type mixed with another type
10/15/08   adm     Added fix for initializing the punturing value in ftm_data to 0
                   as it caused mdsp stalls in FTM mode.
05/19/08   rmd     Enable/Disable the GSM PA (output for the bandgap voltage
                   (VREF_OUT)) when we turn the GSM TX ON/OFF.
03/10/08   dw      Fixed klocwork warnings
03/05/08   jfc     Set the modulation type in ftm_gsm_tx_burst()
12/06/07   jfc     Removing TXDAC_SEL write in ftm_gsm_set_tx()
10/26/07   rsr     Added new function ftm_set_smps_pdm()
09/21/07   jfc     Fix featurization for burst_infor->loopback_flag
08/08/07   jfc     Mainlined T_MSM6275 feature
08/08/07   jfc     Mainlined T_MSM6280 feature
07/12/07   jfc     Set GL1 lna range from ftm_lna_range
02/28/07   ycl     Fix compiler warnings.
02/12/07   jfc     Mainline T_RF_GWZRF6250 and T_RF_GWZRF6250P3
12/05/06   jfc/cs  CR104631 - Disable CHIPXN_DIV2 clk regime in GSM TCH mode on 7200
03/28/06   rsr     Lint cleanup.
                   Changed RX burst API to accept the number of frames over
                   which RSSI is averaged.
                   Added command to perform RX cal for all gain ranges.
03/14/06   og      Take timeslot gaps into account when deciding on Tta/Trb
                   or Ttb/Tra.
03/06/06   rsr     Fixes to remove compiler warnings.
12/08/05   jfc     Fixed bug where first RSSI measurement after LNA change
                    comes from previous LNA state.
10/06/05   rsr    Updates for MSM6280. (Changes to support change in TX_PHASE_INCREMENT
                  bit width.
08/23/05   rsr    Changes to support gl1 related (DTM) changes.
          /gfr
06/16/05   rsr    Don't call ftm_gsm_set_tx_power_level for POLAR targets.
                  This sets the old style pa profiles and overwrites the precharge value.
06/07/05   rsr    Removed call to ftm_gsm_prepare_tx_burst from
                  ftm_gsm_do_tx_burst_buffer_data because this causes the call to
                  rfgsm_setup_tx_burst to happen twice. The ftm_gsm_prepare_tx_burst
                  function is now used only by the contintuous transmit functions.
06/07/05   rsr    Calls to gl1_hw_schedule_reset_events and gl1_hw_schedule_free_events
                  removed and replace by a call to gl1_hw_frame_tick handler in
                  ftm_tdma_isr,per graham's suggestion.
05/26/05   jfc    Changed avg RSSI calculation in ftm_ftm_gsm_do_rx_cmd_rx_burst_metrics()
05/04/05   rsr    Removed warnings.Cleanup
04/21/05   rsr    Added support for new NV items , added functions to turn
                  calpath on and off
04/14/05   rsr/   Removed calls to gl1_hw_setup_tx_power_control from stop_tx functions.
          groff   Changes to gl1_hw_rx_normal_burst interface
04/11/05   rsr/   Changes to GL1 interface.
          groff
04/11/05   rsr    Reset the polarmodcfg values after opll calibration
03/23/05   lcl    GL1 interface change per gfr.
03/04/05   rsr    Added support for OPLL tx ,Fixed continuos receive function to use
                  right antenna settings from NV
02/09/05   rsr/   Renamed FEATURE_POLAR_RF to FEATURE_POLAR
           lcl
01/25/05   lcl    Fixed a modulor problem in path delay calculation.
01/18/05   jfc    Featurized changes for extra parameters for gl1_hw_rx_normal_burst().
1/13/05    jac    Added a line to set bit SLEEP_TXDAC_SEL in the SYSTEM MODE reg
                  when TX is enabled to allow the TX to work in FTM. Also, fixed
                  parameter list mismatches for 1X builds.
12/23/04   jfc    Fixed bug with PA start and stop time
7/13/04    fpe    Merged with Saber 4006 code base and added support for Jaguar GZRF -V6854-8 Rev A.
02/19/04   gfr    Support for GSM rotators.
02/12/04   lcl    Fixed continuous TX.
11/11/03   lcl    Removed ftm_gsm_configure_modulator() from ftm_gsm_init_tx().
7/18/03    rsr    Added support for tx cal sweep API
7/14/03    rsr    Added support for continuous receive API
06/24/03   lcl    Removed un-used variables.
3/06/02    rsr    Put in fix to remove invalid sequence number error when reading RSSI.
12/12/02   xw     Removed gsm_test_script_has
10/24/02   rsr    Added support for B2
                  changed gsm_pa_cntrl_en timing to come on with RF-ON
10/24/02   rsr    Removed the ta adjust in the mdsp_transmit_burst function
10/24/02   rsr    Added support to move the pa, rf_on, etc when the number of
                  leading bits changes.
10/16/02   thh    Removed definitions of RTR6200_RF_ON_TX_ON_START_DELTA_TX
                  and RTR6200_RF_ON_TX_ON_STOP_DELTA_TX.  These are now
                  exported from rfgsm.h.
07/25/01    ai    Added support for RX burst RSSI measurement, added LNA on/off
                  configuration, added support for PMB2411 RX IC
07/12/01    ai    Moved RX, TX, synth initialization code into gsm_rf_init()
                  Changed initial modulator configuration to non-spectral
                  inversion
02/27/01   avi    Created.

=============================================================================*/

/*===========================================================================
              INCLUDES
=============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#include "msg.h"
#include "ftm.h"
#include "ftm_gsm_rfprot.h"
#include "ftm_gsm_prot.h"
#include "ftm_gsm.h"
#include "mdsp_intf.h"
#include "ftm_gsm_ctl.h"
#include "gl1_hw.h"
#include "geran_test_mode_api.h"
#include "stringl.h"

#include "ftm_gsm_rfctl.h"
#include "gl1_defs.h"
#include "rf_test_task_v.h"
#include "ftm_task.h"
#include "rfgsm_mdsp_cal.h"
#include "rfgsm_msm.h"
#include "pm.h"

/* For declaration of rfgsm_nv_tbl */
#include "rfgsm_nv_cmn.h"
#include "rfgsm_core_types.h"
#include "rfgsm_core_handle.h"
#include "rfdevice_gsm_intf.h"
#include "rfgsm_core_tx.h"
#include "rfgsm_core.h"
#include "ftm_gsm.h"
#include "ftm_gsm_rfctl.h"
#include "ftm_common_control.h"
#include "rfgsm_core_temp_comp.h"
#include "rfgsm_core_vbatt_comp.h"
#include "rfgsm_data_manager.h"

//#include "ftm_hwtc_iqcapture.h"

/* For Providing mutex protection in rf mode to slot_assign*/
#include "rfcommon_locks.h"
#include "rex.h"
#include "rfc_card_gsm.h"

/* MSGR definitions for sending RFGSM command/responses to RF Task */
#include "rfgsm_ftm_msg.h"

/* RFM definitions for legacy api calls */
#include "rfm_gsm.h"
#include "rfm_internal.h"

#include "modem_mem.h"

/* Enable reading from tcxomgr in api ftm_set_freq_err*/
#include "comdef.h"
#include "tcxomgr.h"

/*===========================================================================
              DEFINES
=============================================================================*/

#define MAX_GET_RSSI_WAIT_TIME        4600 /*In ms, 1000 GSM Frames*/
#define PDM_MDSP_VCTCXO_ADJ_MASK           0x0FFF
#define FTM_MID_TX_POWER_LVL               7

#define FTM_MAX_BLOCK_OCTET           76
//definitions repeated in rfgsmmdsp.h (need to move to a .h file?)
#define PDM_MDSP_VCTCXO_BITWIDTH 12

#define FTM_NUM_FRAMES_IP2_CAL_SINGLE_ITERATION  2
#define MAX_IP2_CAL_SETUP_LENGTH_QS 580
#define MAX_IP2_CAL_CLEANUP_LENGTH_QS  100
#define MAX_IP2_CAL_STEP_LENGTH_QS  500
#define MAX_DC_SETTLING_TIME_QS 200
#define MAX_IP2_CAL_WIDTH 4

#define GSM_FRAME_LENGTH_QS 5000

/* Enable reading from tcxomgr in api ftm_set_freq_err*/
extern tcxomgr_ppm_type tcxomgr_ft_curr_blocking( void );

/* used in DSDx to align or delay time bases between two gstmrs*/
extern void gstmr_align_timebase(uint16 delay_qs);

/*! @brief these are the pointers to the malloc'ed ftm gsm variables structures */
ftm_gsm_rfctl_vars_s * ftm_gsm_rfctl_vars[RFM_MAX_WAN_DEVICES] = {NULL};



/*===========================================================================
             GLOBAL VARIABLES
=============================================================================*/

/*To protect thread protection for the slot_assign global variable*/
static rf_lock_data_type rfgsm_slot_mutex;


extern const uint8 ftm_multi_slot_data_tone[];
extern const uint8 ftm_multi_slot_data[];

/* Tx data for a tone */
extern const uint32 ftm_tx_data_tone[16];

/* Tx data for random data */
extern const uint32 ftm_tx_data_rnd[512][4];

// Variable used to save Acquired IQ sample information.
static ftm_gsm_iq_capture_info_type *ftm_gsm_iq_capture_info[RFM_MAX_WAN_DEVICES] = {NULL};

// Variable that is used as Response packet for GET SAMPLES Command
static ftm_iq_capture_get_samples_rsp_pkt_type ftm_gsm_iq_capture_get_samples_rsp_pkt;

/*======================================================================================
  DEFINITIONS NEEDED TO SEND/RECEIVE MESSAGES FROM FTM TASK TO RF TASK (AND VICE VERSA)
=======================================================================================*/

extern msgr_client_t ftm_rfgsm_msgr_client;

/* Debug flag to enable additional F3 messages to print UMIDs sent between FTM and RF task */
volatile boolean ftm_gsm_print_umid_debug = TRUE;

/*=======================================================================================*/


/*===========================================================================
                       EXTERNAL VARIABLES
=============================================================================*/

/* FTM GSM data struct */
static const ftm_gsm_state_type ftm_gsm_state_default  =
{
  (enum_type)FTM_PHONE_MODE_GSM_900,/* mode */
  0,                              /* phone_id*/
  1,                                /* channel */
  7,                                /* tx_power_level */
  OFF,                              /* TX state */
  OFF,                              /* PA state */
  (enum_type)FTM_LNA_RANGE_0,       /* lna_range PRX */
  (enum_type)FTM_LNA_RANGE_0,       /* lna_range DRX */
  ON,                               /* ftm_lna_on_off */
  OFF,                              /* Mixer state */
  30,                               /* vga_gain */
  0x0,                              /* rssi */
  0x0,                              /* rssi_drx */
  (int16)0x189D8,             /* two tone freq offset (Hz)*/
  0x00000,                          /* grfc_io_state */
  0x00000,                    /* ftm_grfc_omode */
  0x00000,                          /* ftm_grfc_polarity */
  0x00000,                          /* ftm_grfc_data */
  {
    0x0800,                       /* VCTCXO adjust */
    0x0000,                       /* I DC offset adjust */
    0x0000                        /* Q DC offset adjust */
  },
  {   /* Modulator Config */
    FALSE,                    /* cont_mod_en */
    FALSE,                        /* use_rotator */
    FALSE,                        /* mdsp_iq_sel */
    TRUE,                         /* spec_inv */
    TRUE,                         /* gd_bit_pol */
    0,                            /* gd_bit_bursts */
    0,                            /* os_rate = 48 */
    RF_MODLTR_TOTAL_LEADING_SYMBS,  /* gd_bit_lead */
    9,                            /* gd_bit_trail */
    148                           /* burst_length */
  },
  {   /* CHannel Filter Config */
    FALSE,                        /* auto_ack */
    TRUE,                         /* dma_ena */
    3,                            /* dma_sel = output stage 3 */
    FALSE,                        /* invert_y1i */
    TRUE,                         /* invert_y1q */
    FALSE                         /* use_rotator */
  },
  { /* I/Q DC offsets */
    0x0000,                   /* ftm_i_dc_offset */
    0x0000                        /* ftm_q_dc_offset */
  },

  0                         /* ftm_gsm_padac_override_val */
};

static boolean ftm_gsm_frame_sweep_running = FALSE;

/*===========================================================================

                      LOCAL FUNCTION DEFINITIONS

===========================================================================*/
void ftm_gsm_tx_burst( rfm_device_enum_type rfm_dev );

void ftm_gsm_rf_msgr_send_debug( rfm_device_enum_type rfm_dev,
                                 msgr_umid_type umid,
                                 errno_enum_type result );

void ftm_gsm_rf_msgr_receive_debug( msgr_umid_type umid,
                                    errno_enum_type result,
                                    msgr_umid_type received_umid,
                                    rfa_rf_gsm_ftm_cmd_outcome_t rsp_result);


void ftm_gsm_rf_task_set_slot_override_flag( rfm_device_enum_type rfm_dev,
                                             boolean flag );

void ftm_gsm_set_slot_override_flag ( rfm_device_enum_type rfm_dev,
                                      boolean flag );

void ftm_gsm_rf_task_set_path_delay( rfm_device_enum_type rfm_dev,
                                     uint16 input_delay_value );

void ftm_gsm_rf_task_set_linear_rgi( rfm_device_enum_type rfm_dev,
                                     uint16 rgi,
                                     uint16 env_gain,
                                     rfgsm_modulation_type mod_type,
                                     uint8 slot_num,
                                     boolean set_script,
                                     uint16 pa_scaling,
                                     uint16 smps_bias );

void ftm_gsm_rf_task_set_pa_range( rfm_device_enum_type rfm_dev,
                                   uint8 num_slot,
                                   uint16 pa_range,
                                   rfgsm_modulation_type mod_type );

int8 ftm_gsm_rf_task_set_tx_pow_dbm ( rfm_device_enum_type rfm_dev,
                                      int16 pow_dbm,
                                      int8 index );

ARFCN_T ftm_gsm_get_arfcn( rfm_device_enum_type rfm_dev );

ftm_gsm_iq_capture_info_type* ftm_gsm_iq_capture_info_create_and_get(rfm_device_enum_type rfm_dev);

/*===========================================================================
                      PUBLIC FUNCTION IMPLEMENTATIONS
===========================================================================*/



/*----------------------------------------------------------------------------*/
/*!
   @brief Populates the global variables for GSM FTM with their start values

   @details

   @param

   @retval

*/
void ftm_gsm_populate_variables (ftm_gsm_rfctl_vars_s * vars)
{
  uint8 i;

  /* Copy the default state into the global variable */
  memscpy( (void *) &vars->ftm_gsm_state,
          sizeof(vars->ftm_gsm_state),
          (void *) &ftm_gsm_state_default,
          sizeof(ftm_gsm_state_type) );

  vars->ftm_gl1_hw_gsm_band = SYS_BAND_NONE;

  vars->ftm_gsm_idle_proc_flag = TRUE;

  vars->ftm_frame_count = 0;

  vars->burst_metrics.ftm_rssi = 0;
  vars->burst_metrics.ftm_i_dc_offset = 0;
  vars->burst_metrics.ftm_q_dc_offset = 0;
  vars->burst_metrics.ftm_rssi_counter = 0;
  /*rxd*/
  vars->burst_metrics.ftm_rssi_drx = 0;
  vars->burst_metrics.ftm_i_dc_offset_drx = 0;
  vars->burst_metrics.ftm_q_dc_offset_drx = 0;

  vars->burst_metrics.transit_rx_frame_cnt = FTM_GSM_MIN_RX_FRAME_CNT;
  vars->burst_metrics.burst_num = 0;

  vars->gsm_frame_sweep_enabled = FALSE;
  vars->frame_sweep_fn_count = 0;
  vars->rx_slot_num = 0;
  vars->gsm_frame_sweep_paused_count = 0;
  vars->frame_sweep_header_ptr = NULL;

  vars->ftm_cont_tx = FALSE;
  vars->ftm_cont_rx = FALSE;

  vars->ftm_num_bursts_tx = 0;
  vars->ftm_num_bursts_rx = 0;

  vars->ftm_gsm_tx_freq_offset=0;

  vars->ftm_gsm_rfctl_kv_cal_per_tx_on = RFDEVICE_GSM_KV_CAL_PER_TX_ON;
  vars->ftm_gsm_rfctl_kv_tx_on_off = RFDEVICE_GSM_KV_CAL_TX_ON_OFF;
  vars->ftm_gsm_rfctl_kv_cal_v2_state = FALSE;

  /*Set the Flag to True to enable Multislot RX calibration*/
  vars->do_multi_slot_rx_cal_flag = TRUE;

  vars->ftm_gsm_cal_flag = FALSE;

  vars->tags_rd_index = 0;
  vars->tags_wr_index = 0;

  vars->average_rssi_available = FALSE;

  vars->num_rx_slots = 0;
  vars->multislot_rx_on = FALSE;

  vars->tx_cont_on = OFF;

  vars->tx_frame_matrix_persistent = TRUE;

  vars->data_mod=0;
  vars->ftm_FNmod4=0;   /* keep track of the frame in a block */

  for (i=0;i<FTM_MAX_GSM_TX_SLOT;i++)
  {
    vars->ftm_tx_signal[i].tx_power_index = 10;
    vars->ftm_tx_signal[i].modulation = GL1_DEFS_MODULATION_UNKNOWN;
  }
  vars->ftm_tx_signal[0].tn = GL1_DEFS_TN_0;
  vars->ftm_tx_signal[1].tn = GL1_DEFS_TN_2;
  vars->ftm_tx_signal[2].tn = GL1_DEFS_TN_3;
  vars->ftm_tx_signal[3].tn = GL1_DEFS_TN_4;

  /* Setup the default slot assignment */
  {
    const ftm_slot_type default_slot_assign = {0,FALSE,0xFFFF,GL1_HW_PDTCHU_MCS1,{0},0,0};

    for (i=0;i<FTM_MAX_GSM_SLOT_PER_FRAME;i++)
    {
      memscpy( (void *) &vars->ftm_slot_assign[i],
              sizeof(vars->ftm_slot_assign[i]),
              (void *) &default_slot_assign,
              sizeof(ftm_slot_type));

      vars->ftm_slot_assign[i].slot_num = i;
    }
    /* Activate slot 0 only */
      vars->ftm_slot_assign[0].on_off = TRUE;
      vars->ftm_slot_assign[0].powdBm = 1500;
  }

  vars->ftm_gsm_lna_range_in_rx_lvl_parm = FALSE;

  vars->ftm_gsm_rfctl_reset_rssi = FALSE;

  vars->wait_for_next_lna_range = FALSE;
  vars->next_lna_range_order = 0;

  memset(vars->last_power, 0, (sizeof(int16) * FTM_MAX_GSM_SLOT_PER_FRAME));

  // initialize the sequence of lna ranges in rx cal for a particular band and RX level
  for (i=0;i<8;i++)
  {
    vars->ftm_gsm_next_lna_range[i] = 0xFF;
  }
  vars->ftm_gsm_lna_range_ind_flag =  FALSE;
  vars->frames_to_wait = 0;

  /* Flags used to specify when to start sending rx burst
  ** commands to MDSP and when to stop
  */
  vars->beginning_rx = 0;
  vars->stop_rx = FTM_GSM_MIN_RX_FRAME_CNT;
  vars->stop_tx_triggered = FALSE;

  vars->ftm_gsm_rfctl_reset_transit_rx_frame_cnt = FALSE;

  vars->ftm_gsm_predist_override = FALSE;

  vars->gl1_freq_error_saved = FALSE;
  vars->gl1_freq_error = 0;
  vars->prev_freq_error = 0;
  vars->new_freq_error =0;

  vars->do_tx_da_cal = FALSE;

  vars->diversity_on = TRUE;

  vars->ftm_gsm_mode_enter = FALSE;

  ftm_gsm_reset_ip2_cal_vars(vars);

}


/*----------------------------------------------------------------------------*/
/*!
   @brief Allocates and initialises the global variables for GSM FTM

   @details

   @param rfm_dev
   The RFM device

   @retval
   Pointer to the FTM variables

*/
ftm_gsm_rfctl_vars_s * ftm_gsm_create_and_init_vars( rfm_device_enum_type rfm_dev )
{
  sys_modem_as_id_e_type sub_id = 0xFF;

  if( rfm_dev >= RFM_MAX_WAN_DEVICES )
  {
    MSG_1( MSG_SSID_FTM, MSG_LEGACY_ERROR, "Unknown rfm device %d", rfm_dev);
    return NULL;
  }

  if( ftm_gsm_rfctl_vars[rfm_dev] == NULL )
  {
    MSG_1( MSG_SSID_FTM, MSG_LEGACY_HIGH, "Initializing GSM FTM vars for device %d", rfm_dev);

    ftm_gsm_rfctl_vars[rfm_dev] = modem_mem_alloc( sizeof(ftm_gsm_rfctl_vars_s),
                                                   MODEM_MEM_CLIENT_RFA );

    if( ftm_gsm_rfctl_vars[rfm_dev] != NULL )
    {
      ftm_gsm_rfctl_vars[rfm_dev]->rfm_dev = rfm_dev;

      ftm_gsm_populate_variables(ftm_gsm_rfctl_vars[rfm_dev]);

      rfgsm_mdsp_cal_init_vars( &ftm_gsm_rfctl_vars[rfm_dev]->mdsp_cal_vars );

      ftm_gsm_ber_create_and_init_vars( ftm_gsm_rfctl_vars[rfm_dev]);

      /*Populate sub id*/
      sub_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );

      ftm_gsm_rfctl_vars[rfm_dev]->sub_id = (sub_id == SYS_MODEM_AS_ID_2)?1:0;

      RF_NULL_CHECK_RTN(rfgsm_core_dm_alloc_tx_cal_data_manager_mem( rfm_dev ),"rflm_gsm_tx_cal_data_ptr is NULL!", FALSE);
    }
    else
    {
      ERR_FATAL( "Mem alloc for FTM GSM RFCTL for device %d failed",rfm_dev,0,0);
      return NULL;
    }
  }
  else
  {
    MSG_1( MSG_SSID_FTM, MSG_LEGACY_HIGH, "GSM FTM RFCTL variables already inited for device %d", rfm_dev );
  }

  /* Everything went OK */
  return ftm_gsm_rfctl_vars[rfm_dev];

}

/*----------------------------------------------------------------------------*/
/*!
   @brief Tests if the GSM FTM variables have already been created
   and initialised

   @details

   @param rfm_dev
   The RFM device

   @retval
   TRUE if variables are available
   FALSE if variables are not available and need to be allocated

*/
boolean ftm_gsm_vars_created( rfm_device_enum_type rfm_dev )
{
  if ( (rfm_dev >= RFM_MAX_WAN_DEVICES) ||
       (ftm_gsm_rfctl_vars[rfm_dev] == NULL) )
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}


/*----------------------------------------------------------------------------*/
/*!
   @brief Frees the GSM global variables

   @details

   @param

   @retval

*/
void ftm_gsm_free_vars( rfm_device_enum_type rfm_dev )
{
  if( rfm_dev >= RFM_MAX_WAN_DEVICES )
  {
    MSG_1( MSG_SSID_FTM, MSG_LEGACY_ERROR, "Unknown rfm device %d", rfm_dev);
    return;
  }

  if( ftm_gsm_rfctl_vars[rfm_dev] != NULL )
  {
    modem_mem_free( ftm_gsm_rfctl_vars[rfm_dev], MODEM_MEM_CLIENT_RFA );

    ftm_gsm_rfctl_vars[rfm_dev] = NULL;

    MSG_1( MSG_SSID_FTM, MSG_LEGACY_HIGH, "GSM FTM RFCTL variables freed for device %d", rfm_dev );

    rfgsm_core_dm_free_tx_cal_data_manager_mem(rfm_dev);
  }
  else
  {
    MSG_1( MSG_SSID_FTM, MSG_LEGACY_ERROR, "Cannot free GSM FTM null variables for device %d", rfm_dev );
  }
}

/*----------------------------------------------------------------------------*/
/*!
   @brief Returns a pointer to the cal variables for a given device

   @details

   @param

   @retval

*/
ftm_gsm_rfctl_vars_s * ftm_gsm_get_vars_ptr( rfm_device_enum_type rfm_dev )
{

   // Always access the prx device params
  /*! @todo - revisit this assumption */
  rfm_dev = rfgsm_core_get_prx_dev(rfm_dev);

  if ( ftm_mode != FTM_MODE )
  {
    ERR_FATAL("Attempt to access FTM variables in ONLINE MODE", 0,0,0);
    return NULL;
  }

  if ( !(rfm_dev < RFM_MAX_WAN_DEVICES) )
  {
    ERR_FATAL("Invalid RF Device specified: %d", rfm_dev,0,0);
    return NULL;
  }

  if (ftm_gsm_rfctl_vars[rfm_dev] == NULL)
  {
    ftm_gsm_create_and_init_vars(rfm_dev);
  }

  return ftm_gsm_rfctl_vars[rfm_dev];
}

/*===========================================================================

FUNCTION FTM_GSM_GET_AVE_RSSI_READY_FLAG

DESCRIPTION
   In Rx burst mode, when the RSSI is ready, get this flag

DEPENDENCIES
   MDSP clocks must be running

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/

boolean ftm_gsm_get_ave_rssi_update_flag( rfm_device_enum_type rfm_dev )
{
  return ftm_gsm_get_vars_ptr(rfm_dev)->average_rssi_available;
}


/*===========================================================================

FUNCTION FTM_GSM_SET_AVE_RSSI_READY_FLAG

DESCRIPTION
   In Rx burst mode, when the RSSI is ready, set this flag

DEPENDENCIES
   MDSP clocks must be running

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/

void ftm_gsm_set_ave_rssi_update_flag( rfm_device_enum_type rfm_dev,
                                       boolean update )
{
  ftm_gsm_get_vars_ptr(rfm_dev)->average_rssi_available = update;
}

/*===========================================================================

FUNCTION FTM_GSM_INIT_FTM_SLOT_MUTEX

DESCRIPTION
   Initialize mutex for the global variables (slots) set by ftm_set_frame_matrix

DEPENDENCIES
   MDSP clocks must be running

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void ftm_gsm_init_ftm_slot_mutex()
{
  rf_common_init_critical_section(&rfgsm_slot_mutex);
}

/*===========================================================================

FUNCTION FTM_GSM_CAL_MODE_ENABLED

DESCRIPTION
   check if UE is in FTM Cal mode. In case of non-sig and Online, returns False

DEPENDENCIES
   MDSP clocks must be running

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
boolean ftm_gsm_cal_mode_enabled(rfm_device_enum_type rfm_dev)
{
  /*Since there is no separate ftm enter mode execution for DRX, FTM_STATE_GSM is only set for PRX*/
  if(ftm_get_current_state(rfgsm_core_get_prx_dev(rfm_dev)) == FTM_STATE_GSM) /*Check if in FTM cal mode*/
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================

FUNCTION FTM_GSM_PREDIST_OVERRIDE

DESCRIPTION
    This function sets a FTM flag which tells if predistortion can be nebaled in FTM mode.
    predist_override flag is by d efault false. if it is set TRUE, predist will not be enabled in FTM mode.

DEPENDENCIES

RETURN VALUE
   None.

SIDE EFFECTS
   None

===========================================================================*/
void ftm_gsm_set_predist_override( rfm_device_enum_type rfm_dev,
                       boolean enable_disable )
{

  ftm_gsm_rfctl_vars_s * vars_ptr = ftm_gsm_get_vars_ptr(rfm_dev);

  if ( vars_ptr == NULL )
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR, "FTM variables are NULL", 0);
    return;
  }

  vars_ptr->ftm_gsm_predist_override = enable_disable;

} /* end ftm_gsm_predist_override */


/*===========================================================================

FUNCTION FTM_GSM_PREDIST_OVERRIDE

DESCRIPTION
   check if EDGE Predistortion is in override mode in FTM mod
   returns by default FALSE in Online and Non-sig mode.

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
boolean ftm_gsm_get_predist_override(rfm_device_enum_type rfm_dev)
{
ftm_gsm_rfctl_vars_s * gvars;

if(ftm_get_current_state(rfm_dev) == FTM_STATE_GSM) /*Check if in FTM cal mode*/
{
  gvars = ftm_gsm_get_vars_ptr(rfm_dev);
  return gvars->ftm_gsm_predist_override;
}
else
{
  return FALSE;
}
}

/*===========================================================================

FUNCTION FTM_GSM_SET_PDM

DESCRIPTION
   Sets the defined PDM to a defined value

DEPENDENCIES
   MDSP clocks must be running

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void ftm_gsm_set_pdm( rfm_device_enum_type rfm_dev,
                      const ftm_id_val_type* pdm )
{
  dword value;

  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);
  sys_modem_as_id_e_type as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );

  switch (pdm->id)
  {
  case FTM_PDM_GSM_VCTCXO_ADJ:
    value = pdm->val & PDM_MDSP_VCTCXO_ADJ_MASK;
    /*  Convert the value to a 12-bit 2's complement number */
    if ( value >= (1 << (PDM_MDSP_VCTCXO_BITWIDTH-1) ) )
    {
      value = value - (1<<PDM_MDSP_VCTCXO_BITWIDTH) ;
    }
    geran_test_mode_api_set_vctcxo_pdm_val_leave_chipxn_div2_clock_on((int32)value,as_id);
    break;

  default:
    break;
  }

  gvars->ftm_gsm_state.ftm_pdm_val[ pdm->id ] = (uint16)pdm->val;
} /* end of ftm_gsm_set_pdm */


/*===========================================================================

FUNCTION FTM_GSM_SET_TX

DESCRIPTION
   Turns entire TX chain on or off. This function is a wrapper for MDSP
   driver function mdsp_set_tx()

DEPENDENCIES
   MDSP clocks must be running

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void ftm_gsm_set_tx( rfm_device_enum_type rfm_dev,
                     boolean on_off)
{
  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);

  if ( gvars->ftm_gsm_state.ftm_tx_state == on_off)
  {
    if((gvars->stop_tx_triggered != TRUE) || (on_off == OFF))
    {
      MSG_2(MSG_SSID_FTM, MSG_LEGACY_ERROR,
            "ftm_gsm_set_tx state is already : %d, pending stop_tx_triggered=%d",
            on_off, gvars->stop_tx_triggered);
    return;
  }

    MSG_2(MSG_SSID_FTM, MSG_LEGACY_MED,
          "ftm_gsm_set_tx state is already : %d, looses pending stop_tx_triggered= %d",
          on_off, gvars->stop_tx_triggered);
  }

  /* For stop tx cmd, set the flag stop_tx_trigerred to TRUE.
    It will be handled in ftm_gsm_tx_burst API with a check for ftm_FNmod4 and
    accordingly ftm_tx_state will be changed to Off in correct frame.
    This is the expected behaviour from GL1 for TX. Doing this removes some F3 MSG_ERRORS
    and makes sure that GL1 state machine is working correctly. */
  if (on_off == OFF)
  {
    int8 idx_slot=0;
    gvars->stop_tx_triggered = TRUE;
    if(! gvars->ftm_FNmod4)
    {
      for(idx_slot=0;idx_slot<FTM_MAX_GSM_SLOT_PER_FRAME; idx_slot++)
      gvars->ftm_slot_assign[idx_slot].on_off=OFF;
    }
  }
  else
  {
    gvars->stop_tx_triggered = FALSE;
    gvars->ftm_cont_tx = on_off;
    gvars->ftm_gsm_state.ftm_tx_state = on_off;
  }
} /* end of ftm_gsm_set_tx */



/*===========================================================================

FUNCTION FTM_GSM_SET_LNA_RANGE

DESCRIPTION
   Sets the LNA gain range to High or Low. The LNA is implemented as a part
   of PMB 2411 (DP2000) and programmed via uWire command. The setting will
   take effect on the next RX burst


DEPENDENCIES
   MDSP clocks must be running

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void ftm_gsm_set_lna_range( rfm_device_enum_type rfm_dev,
                            ftm_lna_range_type range )
{
  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);
  /* Get handle to the FTM gsm mutex */

  RFA_FTM_GSM_ISR_LOCK(&ftm_gsm_lock);

  /* verify which mode we're in, then delegate the LNA switching to the MDSP driver */
  switch (range)
  {
  case FTM_LNA_RANGE_0:
    gvars->ftm_gsm_state.ftm_lna_range = (enum_type)FTM_LNA_RANGE_0;
    gvars->ftm_gsm_state.ftm_lna_range_drx = (enum_type)FTM_LNA_RANGE_0;
    break;

  case FTM_LNA_RANGE_1:
    gvars->ftm_gsm_state.ftm_lna_range = (enum_type)FTM_LNA_RANGE_1;
    gvars->ftm_gsm_state.ftm_lna_range_drx = (enum_type)FTM_LNA_RANGE_1;
    break;

  case FTM_LNA_RANGE_2:
    gvars->ftm_gsm_state.ftm_lna_range = (enum_type)FTM_LNA_RANGE_2;
    gvars->ftm_gsm_state.ftm_lna_range_drx = (enum_type)FTM_LNA_RANGE_2;
    break;

  case FTM_LNA_RANGE_3:
    gvars->ftm_gsm_state.ftm_lna_range = (enum_type)FTM_LNA_RANGE_3;
    gvars->ftm_gsm_state.ftm_lna_range_drx = (enum_type)FTM_LNA_RANGE_3;
    break;

  case FTM_LNA_RANGE_4:
    gvars->ftm_gsm_state.ftm_lna_range = (enum_type)FTM_LNA_RANGE_4;
    gvars->ftm_gsm_state.ftm_lna_range_drx = (enum_type)FTM_LNA_RANGE_4;
    break;

  default:
    break;
  }

  /* If the burst is not on then we will have to handle the transitioning arfcn
  ** while also collecting rssi.
  */
  if (gvars->rxBurstOn)
  {
    /* Set the reset frame count flag */
    gvars->ftm_gsm_rfctl_reset_transit_rx_frame_cnt = TRUE;
  }

  /* Release FTM gsm mutex */
  /*lint -e{534} the function rex_set_pri(unsigned long)
   also returns previous priority which is of no interest to us */
  RFA_FTM_GSM_ISR_UNLOCK(&ftm_gsm_lock);

} /* end of ftm_gsm_set_lna_range */

/*===========================================================================

FUNCTION FTM_GSM_SET_NEXT_LNA_RANGE

DESCRIPTION
   Sets the LNA gain range to High or Low. The LNA is implemented as a part
   of PMB 2411 (DP2000) and programmed via uWire command. The setting will
   take effect on the next RX burst. This function is called to set the next LNA range
   in sequence during RX cal. It doesn't reset transit rx frame count.

DEPENDENCIES
   MDSP clocks must be running

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/

void ftm_gsm_set_next_lna_range( rfm_device_enum_type rfm_dev,
                                 ftm_lna_range_type range)
{
  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);

  RFA_FTM_GSM_ISR_LOCK(&ftm_gsm_lock);

  /* verify which mode we're in, then delegate the LNA switching to the MDSP driver */
  switch (range)
  {
  case FTM_LNA_RANGE_0:
    gvars->ftm_gsm_state.ftm_lna_range = (enum_type)FTM_LNA_RANGE_0;
    gvars->ftm_gsm_state.ftm_lna_range_drx = (enum_type)FTM_LNA_RANGE_0;
    break;

  case FTM_LNA_RANGE_1:
    gvars->ftm_gsm_state.ftm_lna_range = (enum_type)FTM_LNA_RANGE_1;
    gvars->ftm_gsm_state.ftm_lna_range_drx = (enum_type)FTM_LNA_RANGE_1;
    break;

  case FTM_LNA_RANGE_2:
    gvars->ftm_gsm_state.ftm_lna_range = (enum_type)FTM_LNA_RANGE_2;
    gvars->ftm_gsm_state.ftm_lna_range_drx = (enum_type)FTM_LNA_RANGE_2;
    break;

  case FTM_LNA_RANGE_3:
    gvars->ftm_gsm_state.ftm_lna_range = (enum_type)FTM_LNA_RANGE_3;
    gvars->ftm_gsm_state.ftm_lna_range_drx = (enum_type)FTM_LNA_RANGE_3;
    break;

  case FTM_LNA_RANGE_4:
    gvars->ftm_gsm_state.ftm_lna_range = (enum_type)FTM_LNA_RANGE_4;
    gvars->ftm_gsm_state.ftm_lna_range_drx = (enum_type)FTM_LNA_RANGE_4;
    break;

  /* The following states are used in V4 cal on SAWless targets to indicate that
     HL mode should be set. */
  case FTM_SAWLESS_HL_LNA_RANGE_0:
    gvars->ftm_gsm_state.ftm_lna_range = (enum_type)FTM_SAWLESS_HL_LNA_RANGE_0;
    gvars->ftm_gsm_state.ftm_lna_range_drx = (enum_type)FTM_SAWLESS_HL_LNA_RANGE_0;
    break;

  case FTM_SAWLESS_HL_LNA_RANGE_1:
    gvars->ftm_gsm_state.ftm_lna_range = (enum_type)FTM_SAWLESS_HL_LNA_RANGE_1;
    gvars->ftm_gsm_state.ftm_lna_range_drx = (enum_type)FTM_SAWLESS_HL_LNA_RANGE_1;
    break;

  case FTM_SAWLESS_HL_LNA_RANGE_2:
    gvars->ftm_gsm_state.ftm_lna_range = (enum_type)FTM_SAWLESS_HL_LNA_RANGE_2;
    gvars->ftm_gsm_state.ftm_lna_range_drx = (enum_type)FTM_SAWLESS_HL_LNA_RANGE_2;
    break;

  case FTM_SAWLESS_HL_LNA_RANGE_3:
    gvars->ftm_gsm_state.ftm_lna_range = (enum_type)FTM_SAWLESS_HL_LNA_RANGE_3;
    gvars->ftm_gsm_state.ftm_lna_range_drx = (enum_type)FTM_SAWLESS_HL_LNA_RANGE_3;
    break;

  case FTM_SAWLESS_HL_LNA_RANGE_4:
    gvars->ftm_gsm_state.ftm_lna_range = (enum_type)FTM_SAWLESS_HL_LNA_RANGE_4;
    gvars->ftm_gsm_state.ftm_lna_range_drx = (enum_type)FTM_SAWLESS_HL_LNA_RANGE_4;
    break;

  default:
    break;
  }
  RFA_FTM_GSM_ISR_UNLOCK(&ftm_gsm_lock);


}






/*===========================================================================

FUNCTION is_valid_arfcn

DESCRIPTION
   RF HW initialization function for the purposes of FTM

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
boolean ftm_gsm_is_valid_arfcn(ftm_mode_id_type mode, uint16 arfcn)
{
  boolean ret_val = FALSE;

  switch ( mode )
  {
  case FTM_PHONE_MODE_GSM_850:
    ret_val = ((arfcn <= 251) && (arfcn >= 128));
    break;

  case FTM_PHONE_MODE_GSM_900:
    ret_val = ((arfcn <= 124) || (arfcn >= 975 && arfcn <= 1023));
    break;

  case FTM_PHONE_MODE_GSM_1800:
    ret_val = ((arfcn <= 885 && arfcn >= 512));
    break;

  case FTM_PHONE_MODE_GSM_1900:
    ret_val = ((arfcn <= 810 && arfcn >= 512));
    break;

  default:
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"Should only be in GSM modes: %d", mode);
    break;
  }

  return ret_val;
} /* end of is_valid_arfcn */

/*===========================================================================

FUNCTION FTM_GSM_SET_ARFCN

DESCRIPTION
   Sets the ARFCN


DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/

void ftm_gsm_set_arfcn( rfm_device_enum_type rfm_dev,
                        uint16 arfcn )
{
  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!");

  /*  Get handle to the FTM gsm mutex */

  RFA_FTM_GSM_ISR_LOCK(&ftm_gsm_lock);

  if (rfgsm_nv_check_debug_nv(rfm_dev, RFGSM_DEBUG_NV_SAWLESS_FORCE_MODE) == FALSE)
  {
    /* If the mode is not forced in debug nv, then reset it to LL mode */
    rfgsm_core_handle_ptr->sawless_linearity_control.linearity_override = RFGSM_SAWLESS_LL_OVERRIDE;
  }
  else
  {
    if (rfgsm_nv_check_debug_nv(rfm_dev, RFGSM_DEBUG_NV_SAWLESS_MODE_FORCE_HIGH) == TRUE)
	{
	  rfgsm_core_handle_ptr->sawless_linearity_control.linearity_override = RFGSM_SAWLESS_HL_OVERRIDE;
	}
	else
	{
	  rfgsm_core_handle_ptr->sawless_linearity_control.linearity_override = RFGSM_SAWLESS_LL_OVERRIDE;
	}
  }

  /* check that we're asked to tune to a different channel */
  if ( arfcn != gvars->ftm_gsm_state.ftm_channel)
  {
    /* make sure the channel is valid */
    if ( ftm_gsm_is_valid_arfcn((ftm_mode_id_type)gvars->ftm_gsm_state.ftm_rf_mode, arfcn) )
    {
      gvars->ftm_gsm_state.ftm_channel = arfcn;
    }
  }

  /* If the burst is not on then we will have to handle the transitioning
  ** arfcn while also collecting rssi.
  */
  if (gvars->rxBurstOn)
  {
    /* Set the reset frame count flag */
    gvars->ftm_gsm_rfctl_reset_transit_rx_frame_cnt = TRUE;
  }

  /* Release FTM gsm mutex */
  /*lint -e{534} the function rex_set_pri(unsigned long)
   also returns previous priority which is of no interest to us */
  RFA_FTM_GSM_ISR_UNLOCK(&ftm_gsm_lock);

} /* end of ftm_gsm_set_arfcn */

/*===========================================================================

FUNCTION ftm_gsm_init_rx

DESCRIPTION
   RF HW initialization function for the purposes of FTM


DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void ftm_gsm_init_rx( rfm_device_enum_type rfm_dev )
{
  NOTUSED(rfm_dev);
} /* end of ftm_gsm_init_rx */

/*===========================================================================

FUNCTION ftm_gsm_init_tx

DESCRIPTION
   RF HW initialization function for the purposes of FTM


DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void ftm_gsm_init_tx( rfm_device_enum_type rfm_dev )
{
  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);
  sys_modem_as_id_e_type as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );

  /* Init tx power level */
  gvars->ftm_gsm_state.ftm_tx_power_lvl = FTM_MID_TX_POWER_LVL;

  /* set VCTCXO to mid-range */
  geran_test_mode_api_set_vctcxo_pdm_val_leave_chipxn_div2_clock_on(gl1_hw_get_vctcxo_pdm_default_val(),as_id);

  /* Set default tx function to ftm_gsm_do_tx_burst_buffer_data() */
  gvars->ftm_tx_funct = ftm_gsm_do_tx_burst_buffer_data;

} /* end of ftm_gsm_init_tx */

/*===========================================================================

FUNCTION   FTM_GSM_PREPARE_CONTINUOUS_TX

DESCRIPTION
   Sets up the Tx burst

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void ftm_gsm_prepare_continuous_tx( rfm_device_enum_type rfm_dev,
                                    gl1_defs_modulation_type* mod_type ,
                                    uint16* paindex )
{
  sys_modem_as_id_e_type as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );

  /*TUNE THE CHANNEL*/
   ARFCN_T  arfcn;
   arfcn = ftm_gsm_get_arfcn( rfm_dev );

   if ((mod_type != NULL) && (paindex != NULL))
   {
     geran_test_mode_api_rf_tx_burst(1,
                                     1,
                                     gl1_hw_arfcn_to_rfgsm_band(arfcn),
                                     arfcn.num ,
                                     geran_test_mode_api_rf_get_freq_error(as_id),
                                     paindex,
                                     mod_type,
                                     as_id);

    }

} /* end of ftm_gsm_prepare_tx_burst */

/*===========================================================================

FUNCTION   FTM_GSM_DO_TX_BURST_BUFFER_DATA

DESCRIPTION
   Sets up the Tx burst from the data buffer

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void ftm_gsm_do_tx_burst_buffer_data(rfm_device_enum_type rfm_dev )
{
  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);

  /* This code does an alternate rx and tx within one frame... This is for andrew's testing..
     the receive does not necessarily function but tunes the vco to a different frequency..
     DVT option only.. not permanent.*/
  gvars->tx_cont_on = OFF;

  ftm_gsm_tx_burst( rfm_dev );

  if (!(gvars->ftm_frame_count % 5000))
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"TXing data on frame %d", gvars->ftm_frame_count);
  }
} /* end of ftm_gsm_do_tx_burst_buffer_data */

/*===========================================================================

FUNCTION   FTM_GSM_DO_TX_CONT_RND

DESCRIPTION
   Transmits continuous psedo random data

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void ftm_gsm_do_tx_cont_rnd( rfm_device_enum_type rfm_dev )
{
  uint16 i;
  uint16 paindex[4] = {0,0,0,0};
  rf_path_enum_type rf_path;

  gl1_defs_modulation_type mod_type[4]= {GL1_DEFS_MODULATION_GMSK, GL1_DEFS_MODULATION_UNKNOWN,GL1_DEFS_MODULATION_UNKNOWN,GL1_DEFS_MODULATION_UNKNOWN};
  sys_modem_as_id_e_type as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );
  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);

  rf_path = rfcommon_core_device_to_path(rfm_dev);

  /*-------------------------------------------------------------*/
  /* Disable Tx if stop_tx_triggered flag is set*/
  if(gvars->stop_tx_triggered == TRUE)
  {
     gvars->ftm_gsm_state.ftm_tx_state = FALSE;
     gvars->ftm_cont_tx = FALSE;
     gvars->stop_tx_triggered = FALSE;
     MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"ftm_gsm_do_tx_cont_rnd, turning off Tx",0);
     return;
  }

  if ( gvars->tx_cont_on == OFF )
  {
    //ftm_gsm_prepare_tx_burst(NULL, NULL);
    //ftm_gsm_rfctl_set_tx_alpha();

    RFGSM_SLOT_LOCK(&rfgsm_slot_mutex);

    for (i=0; i<FTM_MAX_GSM_SLOT_PER_FRAME; i++)
    {
      if (gvars->ftm_slot_assign[i].on_off)
      {
        /* Get transmit power from TX burst matrix params.*/
        (void)ftm_gsm_set_tx_pow_dbm( rfm_dev, gvars->ftm_slot_assign[i].powdBm,0);
        paindex[0] = i;
        /* Get Modulation type from TX burst matrix params. */
        mod_type[0] = ( gvars->ftm_slot_assign[i].mcs < GL1_HW_PDTCHU_MCS5 ) ?
                      GL1_DEFS_MODULATION_GMSK: GL1_DEFS_MODULATION_8PSK;
        break;
      }
    }

    RFGSM_SLOT_UNLOCK(&rfgsm_slot_mutex);

    ftm_gsm_prepare_continuous_tx( rfm_dev, &mod_type[0], &paindex[0]);

    if (mod_type[0] == GL1_DEFS_MODULATION_8PSK )
    {
      mdsp_ftm_start_tx_rnd_cont(1,as_id);
    }
    else
    {
      if (gvars->tx_brst.data_source == (enum_type)FTM_TX_DATA_SOURCE_BUFFER)
      {
        /* Currently, only KV calibration needs user defined random data.  This code will
         * need to change if other task also need user defined random data.
         */
        ftm_gsm_kv_factory_data_type* ftm_kv_training_data = NULL;
        rfdevice_gsm_cmd_dispatch( rfm_dev, rf_path, RFDEVICE_GSM_GET_FACTORY_KV_CALIBRATION_DATA, (void*)(&ftm_kv_training_data));
        /* ftm_kv_training_data will be NULL if KV cal is not supported by the transceiver */
        if(NULL != ftm_kv_training_data)
        {
          mdsp_ftm_start_tx_data_cont(&(ftm_kv_training_data->data[0]),
                                ftm_kv_training_data->data_length,as_id);
        }
        else
        {
          MSG( MSG_SSID_FTM, MSG_LEGACY_ERROR, "ftm_kv_training_data is Null");
        }
      }
      else
      {
        mdsp_ftm_start_tx_rnd_cont(0,as_id);
      }
    }

    /* Enable continuous burst flag */
    gvars->tx_cont_on = ON;
  }
} /* end of ftm_gsm_do_tx_cont_rnd */

/*===========================================================================

FUNCTION   FTM_GSM_DO_TX_CONT_TONE

DESCRIPTION
   Transmits continuous tone data

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void ftm_gsm_do_tx_cont_tone( rfm_device_enum_type rfm_dev )
{
  uint16 i;
  uint16 paindex[4] = {0,0,0,0};
  gl1_defs_modulation_type mod_type[4]= { GL1_DEFS_MODULATION_GMSK, GL1_DEFS_MODULATION_UNKNOWN,
                                          GL1_DEFS_MODULATION_UNKNOWN,GL1_DEFS_MODULATION_UNKNOWN};

  sys_modem_as_id_e_type as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );
  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);

  /*-------------------------------------------------------------*/
  /* Disable Tx if stop_tx_triggered flag is set*/
  if(gvars->stop_tx_triggered == TRUE)
  {
     gvars->ftm_gsm_state.ftm_tx_state = FALSE;
     gvars->ftm_cont_tx = FALSE;
     gvars->stop_tx_triggered = FALSE;
     MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"ftm_gsm_do_tx_cont_tone, turning off Tx",0);
     return;
  }

  if ( gvars->tx_cont_on == OFF )
  {
    RFGSM_SLOT_LOCK(&rfgsm_slot_mutex);
    for ( i=0; i<FTM_MAX_GSM_SLOT_PER_FRAME; i++ )
    {
      if ( gvars->ftm_slot_assign[i].on_off )
      {
        /* Get transmit power from TX burst matrix params.*/
        (void)ftm_gsm_set_tx_pow_dbm( rfm_dev, gvars->ftm_slot_assign[i].powdBm,0);
        paindex[0] = i;

        /* Get Modulation type from TX burst matrix params. */
        if ( gvars->ftm_slot_assign[i].mcs >= GL1_HW_PDTCHU_MCS5 )
        {
          MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"8PSK tone not supported. Doing GMSK tone",0);
        }

        break;
      }
    }
    RFGSM_SLOT_UNLOCK(&rfgsm_slot_mutex);

    ftm_gsm_prepare_continuous_tx( rfm_dev, &mod_type[0], &paindex[0]);

    mdsp_ftm_start_tx_tone_cont(as_id);
    /* Enable continuous burst flag */
    gvars->tx_cont_on = ON;

  }
} /* end of ftm_gsm_do_tx_cont_tone */


/*===========================================================================

FUNCTION   FTM_GSM_DO_TX_STOP_CONT

DESCRIPTION
   Stops transmitting continuous data

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void ftm_gsm_do_tx_stop_cont(rfm_device_enum_type rfm_dev )
{
  sys_modem_as_id_e_type as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );
  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);

  if ( gvars->tx_cont_on == ON)
  {
    /* Disable continuous burst flag */
    ftm_gsm_prepare_continuous_tx( rfm_dev, NULL, NULL );

    mdsp_ftm_stop_tx_tone_cont(as_id);

    gvars->tx_cont_on = OFF;
  }
} /* end of ftm_gsm_do_tx_stop_cont */

/*===========================================================================

FUNCTION  ftm_get_arfcn

DESCRIPTION
  This function returns the FTM band and frequency channel.

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None
===========================================================================*/
ARFCN_T ftm_gsm_get_arfcn( rfm_device_enum_type rfm_dev )
{
  ARFCN_T ftm_infor;
  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);
  ftm_infor.band = gvars->ftm_gl1_hw_gsm_band;
  ftm_infor.num  = gvars->ftm_gsm_state.ftm_channel;

  return(ftm_infor);
}

void ftm_gsm_set_multislot_rx( rfm_device_enum_type rfm_dev,
                               uint8* multislot_ts)
{
  uint8 i = 0;

  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);

  gvars->num_rx_slots = 0;
  for (i = 0; i < (GERAN_API_MAX_GL1_DEFS_MAX_DL_MSGS - 1); i++)
   {
      if (multislot_ts[i]!= 0xF )
      {
        gvars->num_rx_slots++;
        gvars->signal_info[i].tn = (gl1_defs_tn_type) (multislot_ts[i]&0x7);
      }
   }
 if (gvars->num_rx_slots > 1)
 {
   gvars->multislot_rx_on = TRUE;
 }
 else
 {
   gvars->multislot_rx_on = FALSE;
 }
}


/* FTM GSM frame variables*/
static ftm_gsm_frame_sweep_config_type gsm_frame_sweep_config =
{
  NULL,
  0
};

static ftm_gsm_frame_sweep_result_type gsm_frame_sweep_result =
{
  NULL,
  2,
  0
};

/*===========================================================================

FUNCTION FTM_GSM_FRAME_SWEEP_CONFIG

DESCRIPTION
   This command is used to pass a test sequence to the UE before executing the test sequence.

Parameteres
      SEQUENCER_CONFIG_DATA: ptr to the sweep config data.
      LENGTH: size of sweep config data in bytes

RETURN VALUE
   TRUE if config command was successful

SIDE EFFECTS
   None

===========================================================================*/

boolean ftm_gsm_frame_sweep_config(uint8* sequencer_config_data, uint16 length)
{
  uint8* temp_ptr = NULL;

  if (gsm_frame_sweep_config.buffer == NULL)
  {
    gsm_frame_sweep_config.buffer = (uint8 *)modem_mem_alloc(GSM_FRAME_SWEEP_CONFIG_BUFFER_SIZE, MODEM_MEM_CLIENT_RFA);
    if (gsm_frame_sweep_config.buffer == NULL)
    {
      MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"failed to allocate memory", 0);
      return FALSE;
    }
  }

  /* Check that we are not going to overflow the buffer */
  if ((gsm_frame_sweep_config.used_bytes_count + length) >= GSM_FRAME_SWEEP_CONFIG_SIZE)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "ftm_gsm_frame_sweep_config - memory exhausted cmd length %d", length );
    return FALSE;
  }

  /* ptr to next free byte in the cmd buffer*/
  temp_ptr = gsm_frame_sweep_config.buffer + gsm_frame_sweep_config.used_bytes_count;
  memscpy((void*)temp_ptr,
	  GSM_FRAME_SWEEP_CONFIG_BUFFER_SIZE - gsm_frame_sweep_config.used_bytes_count,
	  (void*)sequencer_config_data, length);
  gsm_frame_sweep_config.used_bytes_count = gsm_frame_sweep_config.used_bytes_count + length;

  return TRUE;
}

/*===========================================================================

FUNCTION FTM_GSM_FRAME_SWEEP_EXECUTE

DESCRIPTION
   This command triggers the sweep config sequence execution.

DEPENDENCIES
      None.

RETURN VALUE
   TRUE if config command was successful

SIDE EFFECTS
   None

===========================================================================*/

boolean ftm_gsm_frame_sweep_execute(uint8 dummy)
{
  uint8 rfm_dev;
  uint16 device_count = 0;
  uint8 slot_index = 0;
  uint8 band;
  ftm_gsm_rfctl_vars_s * gvars;
  ftm_gsm_config_header_type *config_header_ptr;
  /* for dual-chain sweep, delay between 2 GSM ISRs is 4 GSM slots*/
  uint16 gstmr_delay_qs = 2500;

  config_header_ptr = (ftm_gsm_config_header_type *)(gsm_frame_sweep_config.buffer);

  if(config_header_ptr == NULL)
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"gsm_frame_sweep_execute failed", 0);
    return FALSE;
  }

  /* before executing check and allocate result buffers*/
  if (gsm_frame_sweep_result.buffer == NULL)
  {
    // Allocate and zero init the results buffer
    gsm_frame_sweep_result.buffer = modem_mem_calloc( 1,
                                                      sizeof(ftm_gsm_frame_sweep_response_type),
                                                      MODEM_MEM_CLIENT_RFA );
    if (gsm_frame_sweep_result.buffer == NULL)
    {
      MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"failed to allocate memory for result buffer", 0);
      return FALSE;
    }
  }

  /* Enable the frame tick handler for GSM sweep*/
  for (device_count = 0; device_count < config_header_ptr->num_device; device_count++)
  {
    rfm_dev = config_header_ptr->device_id[device_count];

    gvars = ftm_gsm_get_vars_ptr(rfm_dev);

    MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"ftm_gsm_frame_sweep enabling on device %d", rfm_dev);

    gvars->frame_sweep_header_ptr = gsm_frame_sweep_config.buffer + sizeof(ftm_gsm_config_header_type);

    band = ((ftm_gsm_frame_header_type *)gvars->frame_sweep_header_ptr)->band;

    /* set band from 1st frame payload*/
    ftm_gsm_set_mode( rfm_dev, (ftm_mode_id_type)band);

    if((config_header_ptr->num_device > 1) && (rfm_dev == RFM_DEVICE_2))
    {
      gstmr_align_timebase(gstmr_delay_qs);

      MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"gstmr_align_timebase %d", gstmr_delay_qs);
    }

    /*disable idle frame processing*/
    gvars->ftm_gsm_idle_proc_flag = FALSE;

    /*during frame_sweep, disable persistent flag*/
    gvars->tx_frame_matrix_persistent = FALSE;

    /* disable all slots which may be ON*/
    for (slot_index = 0; slot_index < FTM_MAX_GSM_SLOT_PER_FRAME; slot_index++)
    {
      gvars->ftm_slot_assign[slot_index].on_off = FALSE;
      gvars->ftm_slot_assign[slot_index].powdBm = 0xFFFF;
    }
  }

  /* Enable the frame tick handler to start GSM sweep*/
  for (device_count = 0; device_count < config_header_ptr->num_device; device_count++)
  {
   rfm_dev = config_header_ptr->device_id[device_count];

   gvars = ftm_gsm_get_vars_ptr(rfm_dev);

   /*set sequencer_enabled to TRUE to enable sweep from next frame tick*/
   gvars->gsm_frame_sweep_enabled = TRUE;
  }
  return TRUE;
}

boolean ftm_gsm_frame_sweep_cleanup (uint8 dummy)
{
  /*Initialize sequencer config structure*/
  gsm_frame_sweep_config.used_bytes_count = 0;

  if(gsm_frame_sweep_config.buffer != NULL)
  {
    /* release allocated memory*/
    modem_mem_free(gsm_frame_sweep_config.buffer, MODEM_MEM_CLIENT_RFA);
    gsm_frame_sweep_config.buffer = NULL;
  }

  /*initialize sequencer result structure*/
  gsm_frame_sweep_result.buff_write_count = 2;
  gsm_frame_sweep_result.buff_read_count = 0;

  if(gsm_frame_sweep_result.buffer != NULL)
  {
    /* release allocated memory*/
    modem_mem_free(gsm_frame_sweep_result.buffer, MODEM_MEM_CLIENT_RFA);
    gsm_frame_sweep_result.buffer = NULL;
  }
  return TRUE;
}

/*===========================================================================

FUNCTION FTM_GSM_FRAME_SWEEP_CONFIG

DESCRIPTION
   This command is used to get results collecet by UE during last frame_sweep cmd.
   It includes frame by frame results for all rx measurements which occurred during
   last test sequence.
   In case where same test sequence is executed multiple times by repeating sweep_execute
   command, all rx results will be appended in one results buffer and output of get_result
   cmd will return all those results.

Parameteres
      RESULT_BUFFER: ptr to the result buffer which is updated with result data.

RETURN VALUE
   TRUE if config command was successful

SIDE EFFECTS
   None

===========================================================================*/

boolean ftm_gsm_frame_sweep_get_results(ftm_gsm_frame_sweep_result_buffer_type* result_buffer)
{
  uint8* temp_ptr = NULL;

  /*fix the result packet size for now*/
  uint16 result_len;
  uint16 diag_limit =0;

  /* For Off-target Testing, set the Diag Limit to very low number so
    that multiple result request testing can be done. */
  #ifdef T_WINNT
    #error code not present
#else
    /* Get Diag Limitation on Max size of Response packet that can be sent */
    diag_limit = 3400;
  #endif /* T_WINNT */

  if (gsm_frame_sweep_result.buffer == NULL)
  {
    MSG_1( MSG_SSID_FTM, MSG_LEGACY_ERROR, "gsm_frame_sweep result buffer is NULL", 0);
    /* there are no bytes to read, return a packet with empty result buffer flag set*/
    result_buffer->empty_result_buffer = TRUE;
    result_buffer->buffer_size = 0;
    return FALSE;
  }

  MSG_2( MSG_SSID_FTM, MSG_LEGACY_HIGH, "ftm_gsm_frame_sweep_get_results %d %d", gsm_frame_sweep_result.buff_read_count, gsm_frame_sweep_result.buff_write_count);

  /* check if result buffer  has more data which is not read already*/
  if(gsm_frame_sweep_result.buff_read_count < gsm_frame_sweep_result.buff_write_count)
  {
    temp_ptr = (uint8 *)gsm_frame_sweep_result.buffer + gsm_frame_sweep_result.buff_read_count;

    /* calculate number of bytes to copy in this result buffer*/
    result_len = MIN(diag_limit, (uint16)(gsm_frame_sweep_result.buff_write_count - gsm_frame_sweep_result.buff_read_count));

    result_buffer->buffer_size = result_len;

    memscpy((void*)(result_buffer->buffer_data), GSM_FRAME_SWEEP_RESULT_BUFFER_SIZE, (void*)temp_ptr, result_len);

    /* update the read_count to track it*/
    gsm_frame_sweep_result.buff_read_count = gsm_frame_sweep_result.buff_read_count + result_len;

    /* set buffer empty flag FALSE if more data is left in result buffer*/
    if(gsm_frame_sweep_result.buff_read_count < gsm_frame_sweep_result.buff_write_count)
    {
      result_buffer->empty_result_buffer = FALSE;
    }
    else
    {
      result_buffer->empty_result_buffer = TRUE;

      MSG_1( MSG_SSID_FTM, MSG_LEGACY_HIGH, "gsm_frame_sweep, result buffer exhausted, perform init", 0);

      /* reset the buffer content to all 0*/
      memset(gsm_frame_sweep_result.buffer,  0, sizeof(ftm_gsm_frame_sweep_response_type));

      /*initialize sequencer result structure*/
      gsm_frame_sweep_result.buff_write_count = 2;   //uint16 payload_count is part of buffer
      gsm_frame_sweep_result.buff_read_count = 0;

      /*Initialize sequencer config structure*/
      gsm_frame_sweep_config.used_bytes_count = 0;
    }
  }
  else
  {
    /* there are no bytes to read, return a packet with empty result buffer flag set*/
    result_buffer->empty_result_buffer = TRUE;
    result_buffer->buffer_size = 0;
  }
  return TRUE;
}

/*===========================================================================

FUNCTION FTM_GSM_FRAME_SWEEP_CMD

DESCRIPTION
   processes GSM frame sweep cmds

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/

boolean ftm_gsm_frame_sweep_cmd( ftm_gsm_frame_sweep_cmd_id cmd_id, uint8* cmd_buffer, uint16 length)
{
  boolean status;

  ftm_gsm_frame_sweep_running = TRUE;

  MSG_2(MSG_SSID_FTM, MSG_LEGACY_HIGH,"ftm_gsm_frame_sweep_cmd cmd ID %d, length: %d", cmd_id, length);
  switch ( cmd_id )
  {
  case FTM_GSM_FRAME_SWEEP_CFG:
    status = ftm_gsm_frame_sweep_config(cmd_buffer, length);
	break;

  case FTM_GSM_FRAME_SWEEP_EXECUTE:
    status = ftm_gsm_frame_sweep_execute(0);
    break;

  case FTM_GSM_FRAME_SWEEP_GET_RESULTS:
    status = ftm_gsm_frame_sweep_get_results((ftm_gsm_frame_sweep_result_buffer_type *)cmd_buffer);
    break;

  case FTM_GSM_FRAME_SWEEP_CLEANUP:
    /*explicit clean-up cmd*/
    status = ftm_gsm_frame_sweep_cleanup(0);
    ftm_gsm_frame_sweep_running = FALSE;
    break;

  default:
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"ftm_gsm_frame_sweep unknown cmd: %d", cmd_id);
    status = FALSE;
    break;

 }
 return status;
}

/* fixed to 5 dead frames sweep band change */
#define FRAME_SWEEP_BAND_CHANGE_DELAY 4

/*===========================================================================

FUNCTION FTM_GSM_FRAME_SWEEP_HANDLER

DESCRIPTION
   This is the main handler which is executed in every frame by ftm_tdma_isr.
   if gsm_frame_sweep_enabled flag is set for given rfm device, it processes one frame from frame sweep config.
   This function runs in tdma isr context and can only process per slot config for one frame at a time.


DEPENDENCIES
      GSM Rx and Tx should be enabled before setting gsm_frame_sweep_enabled flag.

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/

void ftm_gsm_frame_sweep_handler(rfm_device_enum_type rfm_dev)
{
  ftm_gsm_frame_header_type *header_ptr;
  ftm_gsm_slot_payload_type *payload_ptr;
  ftm_frame_matrix_type tx_frame_matrix;
  ftm_gsm_tx_slot_config_type tx_slot_config;
  ftm_rx_burst_type ftm_rx_burst;
  uint8 num_frames_avg = 1;
  uint16 slot_count = 0;
  ftm_gsm_rfctl_vars_s * gvars;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr;
  rfgsm_enh_rx_params_type enh_rx_params;
  ftm_gsm_config_header_type *ftm_gsm_config_header;

  enh_rx_params.enh_rx_enable_flag = 0;
  enh_rx_params.enh_rx_offset_freq = 0;

  ftm_gsm_config_header = (ftm_gsm_config_header_type *)(gsm_frame_sweep_config.buffer);

  gvars = ftm_gsm_get_vars_ptr(rfm_dev);
  if (gvars == NULL)
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"NULL FTM handle for device= %d !", rfm_dev);
    return;
  }

  if (gvars->gsm_frame_sweep_enabled == FALSE)
  {
    return;
  }

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"NULL core handle for device= %d !", rfm_dev);
    return;
  }
  if (gvars->frame_sweep_header_ptr == NULL)
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"NULL frame_sweep_header_ptr device= %d !", rfm_dev);
    return;
  }

  header_ptr = (ftm_gsm_frame_header_type *)gvars->frame_sweep_header_ptr;
  payload_ptr = (ftm_gsm_slot_payload_type *)(gvars->frame_sweep_header_ptr + sizeof(ftm_gsm_frame_header_type));

  /* check if band change sequence is in progress*/
  if(gvars->gsm_frame_sweep_paused_count > 0)
  {
    /* decrement band_change_delay_count*/
    gvars->gsm_frame_sweep_paused_count--;

    /* is stop rx pending */
    if((int8)gvars->stop_rx < 0)
    {
      gvars->stop_rx++;
      if(gvars->stop_rx == 0)
      {
        gvars->rxBurstOn = FALSE;
        MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"frame_sweep: RX triggered late burst off", 0);
      }
    }

    if (gvars->gsm_frame_sweep_paused_count == 1)
    {
      ftm_gsm_frame_sweep_set_band( rfm_dev, (ftm_mode_id_type)header_ptr->band);

      MSG_3(MSG_SSID_FTM, MSG_LEGACY_HIGH,"frame_sweep: set band dev %d, ftm_cont_rx=%d ftm_cont_tx=%d",
                          rfm_dev, gvars->ftm_cont_rx, gvars->ftm_cont_tx);
	  return;
	}
	else if (gvars->gsm_frame_sweep_paused_count == 0)
	{
      /*during frame_sweep, disable persistent flag*/
      gvars->tx_frame_matrix_persistent = FALSE;

      /*Enable Tx*/
      ftm_gsm_set_tx(rfm_dev, TRUE);

      /*Enable Rx*/
      ftm_rx_burst.slot_num = 0;
      ftm_rx_burst.ftm_rx_funct = FTM_RX_FUNCTION_SINGLE_BURST;
      ftm_gsm_rx_burst_command(rfm_dev, &ftm_rx_burst, num_frames_avg);

      MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"frame_sweep: enabled Rx and Tx on dev %d",rfm_dev);
    }
    else
    {
      MSG_3(MSG_SSID_FTM, MSG_LEGACY_HIGH,"frame_sweep: paused count %d, ftm_cont_rx=%d ftm_cont_tx=%d",
                          gvars->gsm_frame_sweep_paused_count, gvars->ftm_cont_rx, gvars->ftm_cont_tx);
      return;
    }
  } // if(gvars->gsm_frame_sweep_paused_count > 0)

  /*if it is first frame tick, enable Rx and Tx now*/
  if (gvars->frame_sweep_fn_count == 0)
  {
    /*Enable Rx*/
    ftm_rx_burst.slot_num = 0;
    ftm_rx_burst.ftm_rx_funct = FTM_RX_FUNCTION_SINGLE_BURST;
    ftm_gsm_rx_burst_command(rfm_dev, &ftm_rx_burst, num_frames_avg);

    /*Enable Tx*/
    ftm_gsm_set_tx(rfm_dev, TRUE);

    /* set the data source to Random*/
    gvars->tx_brst.data_source = FTM_TX_DATA_SOURCE_PSDRND;
  }

  /*continue with frame sweep*/
  if( (gvars->frame_sweep_fn_count < ftm_gsm_config_header->num_frame) &&
      (gvars->frame_sweep_fn_count < MAX_NUM_OF_FRAMES))
  {
    /*If band is changed, stop rx/tx, trigger band change sequence and return*/
    if (gvars->ftm_gsm_state.ftm_rf_mode != header_ptr->band)
    {
      /*Set stop Rx flags towards the end of a block */
      int8 stop_remaining_bursts = (4 - gvars->ftm_FNmod4) % 4 + 1;

      if(stop_remaining_bursts > FTM_GSM_MIN_RX_FRAME_CNT)
      {
        gvars->stop_rx = (uint8) (FTM_GSM_MIN_RX_FRAME_CNT - stop_remaining_bursts);

        MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"frame_sweep: RX burst off delayed %d frames",
              stop_remaining_bursts - FTM_GSM_MIN_RX_FRAME_CNT);
      }
      else
      {
        gvars->stop_rx = 0;
        gvars->rxBurstOn = FALSE;
        stop_remaining_bursts = FTM_GSM_MIN_RX_FRAME_CNT;
      }

      /*set stop Tx*/
      ftm_gsm_set_tx(rfm_dev, OFF);

      MSG_3(MSG_SSID_FTM, MSG_LEGACY_MED,"frame_sweep: stop Rx Tx:dev_id=%d, stop_rx=%d, ftm_FNmod4=%d",
            rfm_dev, gvars->stop_rx, gvars->ftm_FNmod4);

      /* set paused_count to enable band change sequence from next frame*/
      gvars->gsm_frame_sweep_paused_count = FRAME_SWEEP_BAND_CHANGE_DELAY;

      MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"frame_sweep: new GSM band %d",header_ptr->band);
      return;
    }

    MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"frame_sweep: tick = %d", gvars->frame_sweep_fn_count);

    /* set channel if changed*/
    if (gvars->ftm_gsm_state.ftm_channel != header_ptr->chan)
    {
      /* make sure the channel is valid */
      if (ftm_gsm_is_valid_arfcn((ftm_mode_id_type)gvars->ftm_gsm_state.ftm_rf_mode, header_ptr->chan))
      {
        gvars->ftm_gsm_state.ftm_channel = header_ptr->chan;
      }
    }

    for(slot_count = 0; slot_count < header_ptr->num_slot_payload; slot_count++)
    {
      /* only process slot payloads for this rfm_dev*/
      if (payload_ptr->device_id == rfm_dev)
      {
        switch (payload_ptr->op_id)
        {
        case TX_PCL_MEAS_OP:
          tx_frame_matrix.slot_num = payload_ptr->slot_num;
          tx_frame_matrix.on_off   = TRUE;
          RF_NULL_CHECK_RTN_V( rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl, "rfgsm_tx_nv_tbl is NULL!. Check RF Tx cal data" );
          RF_NULL_CHECK_RTN_V( rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr, "rfgsm_tx_nv_tbl->tx_static_nv_data_ptr is NULL!. Check RF Tx cal data" );
          tx_frame_matrix.power_dB = rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->power_levels[payload_ptr->op_data.tx_pcl_meas_data.pcl];
          tx_frame_matrix.mcs      = payload_ptr->op_data.tx_pcl_meas_data.mcs;
          ftm_gsm_set_frame_matrix(rfm_dev, &tx_frame_matrix);
          break;

        case TX_RGI_MEAS_OP:
          tx_slot_config.slot_num = payload_ptr->slot_num;
          tx_slot_config.pa_range = payload_ptr->op_data.tx_rgi_meas_data.pa_state;
          tx_slot_config.rgi      = payload_ptr->op_data.tx_rgi_meas_data.rgi;
          tx_slot_config.env_gain = payload_ptr->op_data.tx_rgi_meas_data.env_gain;
          RF_NULL_CHECK_RTN_V( rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl, "rfgsm_tx_nv_tbl is NULL!. Check RF Tx cal data" );
          RF_NULL_CHECK_RTN_V( rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr, "rfgsm_tx_nv_tbl->tx_static_nv_data_ptr is NULL!. Check RF Tx cal data" );
          tx_slot_config.smps_bias= rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->smps_pdm_tbl.cal_smps_pdm_tbl[tx_slot_config.pa_range];
          tx_slot_config.mod_type_is_gsm = !payload_ptr->op_data.tx_rgi_meas_data.mod_type; //this is reverse logic...mod_type: GMSK = 0; 8PSK = 1 ---> mod_type_is_gsm: 1 = GMSK; 0 = 8PSK
          tx_slot_config.pa_scaling = RFGSM_MSM_PA_SCALE_UNITY_GAIN;
          ftm_gsm_set_tx_slot_config(rfm_dev, &tx_slot_config);
          break;

        case RX_RSSI_MEAS_OP:
          /* update slot num and LNA state information*/
          gvars->rx_slot_num = payload_ptr->slot_num;

          /* get gain value for this LNA range, it will be used for logging in ftm_gsm_do_rx_burst_processing API*/
          /* PRX */
          gvars->ftm_gsm_state.ftm_lna_range = (enum_type)payload_ptr->op_data.rx_rssi_meas_data.lna_state;
          gvars->ftm_gain_range.valx16 = rfgsm_core_get_lna_gain( rfm_dev, gvars->ftm_gsm_state.ftm_channel, gvars->ftm_gsm_state.ftm_lna_range);

          /* DRX */
          gvars->ftm_gsm_state.ftm_lna_range_drx = (enum_type)payload_ptr->op_data.rx_rssi_meas_data.lna_state;
          gvars->ftm_gain_range_drx.valx16 = rfgsm_core_get_lna_gain( rfm_dev+1, gvars->ftm_gsm_state.ftm_channel, gvars->ftm_gsm_state.ftm_lna_range_drx);

          break;

        case RX_DBM_MEAS_OP:
          /* update slot num and LNA state information based on expected pwr*/
          gvars->rx_slot_num = payload_ptr->slot_num;

          /* get LNA state and corresponding lna_gain value using expected pwr level, it will be used for logging in ftm_gsm_do_rx_burst_processing API*/
          /* PRX */
          rfgsm_core_set_radio_gain( rfm_dev,
                                     payload_ptr->op_data.rx_dbm_meas_data.expected_rx_pwr,
                                     &gvars->ftm_gain_range,
                                     enh_rx_params ); //gain range returned here is indexed 1-5
          gvars->ftm_gsm_state.ftm_lna_range = gvars->ftm_gain_range.range-1; //ftm lna & gl1 lna range interpretation is from 0-4
          /* DRX */
          rfgsm_core_set_radio_gain( rfm_dev+1,
                                     payload_ptr->op_data.rx_dbm_meas_data.expected_rx_pwr,
                                     &gvars->ftm_gain_range_drx,
                                     enh_rx_params ); //gain range returned here is indexed 1-5
          gvars->ftm_gsm_state.ftm_lna_range_drx = gvars->ftm_gain_range_drx.range-1; //ftm lna & gl1 lna range interpretation is from 0-4
          break;

        case TX_DBM_MEAS_OP:
          tx_frame_matrix.slot_num = payload_ptr->slot_num;
          tx_frame_matrix.on_off   = TRUE;
          tx_frame_matrix.power_dB = payload_ptr->op_data.tx_dbm_meas_data.power_dBm100;
          tx_frame_matrix.mcs	   = payload_ptr->op_data.tx_dbm_meas_data.mcs;
          ftm_gsm_set_frame_matrix(rfm_dev, &tx_frame_matrix);
          break;

        default:
          MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"frame_sweep: op_id %d is invalid",payload_ptr->op_id);
          break;
        }
      }

     /* increment payload_ptr to point to next slot's payload*/
     payload_ptr = (ftm_gsm_slot_payload_type *)((uint8 *)payload_ptr + sizeof(ftm_gsm_slot_payload_type));

    }

    /*update the frame header ptr in FTM handle for next frame */
    gvars->frame_sweep_header_ptr = (uint8 *)payload_ptr;

    /* increase the frame count*/
    gvars->frame_sweep_fn_count++;
  }
  else
  {
    /* disable frame sweep state machine*/
    gvars->gsm_frame_sweep_enabled = FALSE;
    gvars->frame_sweep_fn_count = 0;

    /*Set stop Rx flags*/
    gvars->stop_rx = 0;
    gvars->rxBurstOn = FALSE;

    /* disable Tx*/
    ftm_gsm_set_tx(rfm_dev, OFF);

    /*Enable idle frame operation back, it was disabled for frame sweep*/
    gvars->ftm_gsm_idle_proc_flag = TRUE;

    MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"frame_sweep: disabled on device %d", rfm_dev);
  }
}

void ftm_gsm_log_rx_burst_matrix( rfm_device_enum_type rfm_dev,
                                  ftm_gsm_frame_sweep_rx_tag_type gsm_rx_burst_tag,
                                  gl1_defs_metrics_rpt burst_matrix)
{
  ftm_gsm_result_payload_type* result_payload_ptr;
  dBx16_T dBx16_result;
  ftm_gsm_rfctl_vars_s * gvars;
  uint32 rssi;
  uint8 lna_state;
  uint16 gain_valx16;
  uint8 result_idx;

  gvars = ftm_gsm_get_vars_ptr(rfm_dev);

  if (gsm_frame_sweep_result.buffer == NULL || gvars == NULL)
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"NULL ptr for device= %d !", rfm_dev);
    return;
  }

  if (gsm_frame_sweep_result.buffer->payload_count < MAX_NUM_OF_FRAMES)
  {
    /* update device id and slot num information in result buffer - one slot per frame, one result per slot */
    result_payload_ptr = &(gsm_frame_sweep_result.buffer->result_payload[gsm_frame_sweep_result.buffer->payload_count++]);
    gsm_frame_sweep_result.buff_write_count += sizeof(ftm_gsm_result_payload_type);
  }
  else
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"frame_sweep: result buffer full = %d !", gsm_frame_sweep_result.buffer->payload_count);
    return;
  }

  /*Use Rx burst tag for this burst to fill result payload frame and slot values*/
  result_payload_ptr->device_id = rfm_dev;
  result_payload_ptr->frame_id = gsm_rx_burst_tag.frame_id;
  result_payload_ptr->slot_num = gsm_rx_burst_tag.slot_num;
  result_payload_ptr->num_of_results = 0;

  for( result_idx = 0; result_idx < MAX_NUM_OF_RESULTS_PER_SLOT; result_idx++ )
  {
    // Check we haven't exceeded the max number of results
    if( result_payload_ptr->num_of_results > MAX_NUM_OF_RESULTS_PER_SLOT )
    {
      MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"num of results = %d !", result_payload_ptr->num_of_results);
    }
    else
    {
      if( result_idx == 0 )
      {
        // PRX
        rssi = burst_matrix.rssi;
        lna_state = gsm_rx_burst_tag.lna_state;
        gain_valx16 = gsm_rx_burst_tag.gain_valx16;
      }
      else
      {
        // DRX
        rssi = burst_matrix.rssi_divrx;
        lna_state = gsm_rx_burst_tag.lna_state_drx;
        gain_valx16 = gsm_rx_burst_tag.gain_valx16_drx;
      }

      // Write the results to the payload
      result_payload_ptr->result[result_idx].op_id = RX_RSSI_MEAS_OP;
      result_payload_ptr->result[result_idx].op_result.rssi_meas_result.rssi = rssi;
      result_payload_ptr->result[result_idx].op_result.rssi_meas_result.chan = gsm_rx_burst_tag.chan;
      result_payload_ptr->result[result_idx].op_result.rssi_meas_result.lna_state = lna_state;

      if (rfgsm_core_ten_log_of_power_val(rssi, &dBx16_result))
      {
        result_payload_ptr->result[result_idx].op_result.rssi_meas_result.rx_pwr_dbm16 = (uint16)(dBx16_result - (dBx16_T)(gain_valx16));
      }

      MSG_6( MSG_SSID_FTM, MSG_LEGACY_HIGH, "payload_count %d result_idx %d, chan %d rssi %d rx_pwr_dbm16 %d lna_state %d",
             gsm_frame_sweep_result.buffer->payload_count - 1,
             result_idx,
             result_payload_ptr->result[result_idx].op_result.rssi_meas_result.chan,
             result_payload_ptr->result[result_idx].op_result.rssi_meas_result.rssi,
             result_payload_ptr->result[result_idx].op_result.rssi_meas_result.rx_pwr_dbm16,
             result_payload_ptr->result[result_idx].op_result.rssi_meas_result.lna_state );

      // Increment the result count in the payload
      result_payload_ptr->num_of_results++;
    } // result_payload_ptr->num_of_results <= MAX_NUM_OF_RESULTS_PER_SLOT
  }
}

/*===========================================================================

FUNCTION   FTM_GSM_RX_BURST_UPDATE RSSI

DESCRIPTION
   This API updates RSSI Upon receiving them from GL1 scheduled in ftm_gsm_do_rx_cmd_rx_burst_metrics

DEPENDENCIES
  RX Burst needs to be scheduled using ftm_gsm_do_rx_cmd_rx_burst_metrics first

PARAMS
  rfm_device

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void ftm_gsm_rx_burst_update_rssi(rfm_device_enum_type rfm_dev)
{
  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);

  if (gvars == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"gvars  is NULL for rfm_device :%d!", rfm_dev);
    return;
  }

  if ( gvars->burst_metrics.ftm_rssi_counter % gvars->ftm_gsm_rssi_no_frames_avg )
  {
    gvars->burst_metrics.ftm_rssi += gvars->gsm_rx_burst_metrics.rssi;
    gvars->burst_metrics.ftm_rssi_drx += gvars->gsm_rx_burst_metrics.rssi_divrx;
    gvars->burst_metrics.ftm_i_dc_offset += (uint32)gvars->gsm_rx_burst_metrics.dc_offset_i; /*lint !e571*/
    gvars->burst_metrics.ftm_q_dc_offset += (uint32)gvars->gsm_rx_burst_metrics.dc_offset_q; /*lint !e571*/
    gvars->burst_metrics.ftm_i_dc_offset_drx += (uint32)gvars->gsm_rx_burst_metrics.dc_offset_i_divrx; /*lint !e571*/
    gvars->burst_metrics.ftm_q_dc_offset_drx += (uint32)gvars->gsm_rx_burst_metrics.dc_offset_q_divrx; /*lint !e571*/
	gvars->burst_metrics.ftm_rssi_counter++;

  }
  else
  {
    /* if ftm_gsm_rssi_no_frames_avg = n, then the above if statement only counts upto n-1.
    ** Accumulate the metrics for the n'th frame before averaging.
    */
    gvars->burst_metrics.ftm_rssi += gvars->gsm_rx_burst_metrics.rssi;
    gvars->ftm_gsm_state.ftm_rssi = gvars->burst_metrics.ftm_rssi /gvars->ftm_gsm_rssi_no_frames_avg;
    gvars->burst_metrics.ftm_i_dc_offset += (uint32)gvars->gsm_rx_burst_metrics.dc_offset_i; /*lint !e571*/
    gvars->burst_metrics.ftm_q_dc_offset += (uint32)gvars->gsm_rx_burst_metrics.dc_offset_q; /*lint !e571*/
    gvars->burst_metrics.ftm_rssi_drx += gvars->gsm_rx_burst_metrics.rssi_divrx;
    gvars->ftm_gsm_state.ftm_rssi_drx = gvars->burst_metrics.ftm_rssi_drx /gvars->ftm_gsm_rssi_no_frames_avg;
    gvars->burst_metrics.ftm_i_dc_offset_drx += (uint32)gvars->gsm_rx_burst_metrics.dc_offset_i_divrx; /*lint !e571*/
    gvars->burst_metrics.ftm_q_dc_offset_drx += (uint32)gvars->gsm_rx_burst_metrics.dc_offset_q_divrx; /*lint !e571*/

    gvars->burst_metrics.ftm_rssi_counter = 1;

    ftm_gsm_set_ave_rssi_update_flag( rfm_dev, TRUE );

    gvars->ftm_gsm_state.ftm_i_q_dc_offset.ftm_i_dc_offset =
      (uint16)(gvars->burst_metrics.ftm_i_dc_offset /gvars->ftm_gsm_rssi_no_frames_avg);

    gvars->ftm_gsm_state.ftm_i_q_dc_offset.ftm_q_dc_offset =
      (uint16)(gvars->burst_metrics.ftm_q_dc_offset /gvars->ftm_gsm_rssi_no_frames_avg);

    gvars->ftm_gsm_state.ftm_i_q_dc_offset.ftm_i_dc_offset_drx =
      (uint16)(gvars->burst_metrics.ftm_i_dc_offset_drx /gvars->ftm_gsm_rssi_no_frames_avg);
    gvars->ftm_gsm_state.ftm_i_q_dc_offset.ftm_q_dc_offset_drx =
      (uint16)(gvars->burst_metrics.ftm_q_dc_offset_drx /gvars->ftm_gsm_rssi_no_frames_avg);

    gvars->burst_metrics.ftm_rssi = 0;
    gvars->burst_metrics.ftm_i_dc_offset = 0;
    gvars->burst_metrics.ftm_q_dc_offset = 0;

    gvars->burst_metrics.ftm_rssi_drx = 0;
    gvars->burst_metrics.ftm_i_dc_offset_drx = 0;
    gvars->burst_metrics.ftm_q_dc_offset_drx = 0;

  }

}
/*===========================================================================

FUNCTION   FTM_GSM_DO_RX_CMD_RX_BURST_METRICS

DESCRIPTION
   Does receive and gets data from Rx debug buffer

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void ftm_gsm_do_rx_cmd_rx_burst_metrics( rfm_device_enum_type rfm_dev )
{
  uint16 i = 0;
  uint32 temp_rssi;
  uint32 temp_rssi_drx;
  gl1_defs_rx_pkt_data_type data;
  uint32 f_count;
  boolean multi_slot_rx_cal_flag = FALSE;

  sys_modem_as_id_e_type as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );
  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);

  if (gvars == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"gvars  is NULL for rfm_device :%d!", rfm_dev);
    return;
  }

  multi_slot_rx_cal_flag = gvars->do_multi_slot_rx_cal_flag;

  /*
    If stop rx has been issued don't issue a burst command but wait for
  RSSI results from last 2 burst commands.This will take 2 frames.
  NOTE: The stop rx command sets stop_rx = 0.

   THIS DIAGRAM EXPLAINS THE CODE IMPLEMENTATION IN THIS FUNTION.

 |----> beginning_rx = 0 set by set_arfcn / set_lna_range / set rx on functions
 |
 |   ________________________________________FLOW CHART_________________________________________________________________
 |  |stop_rx = 3 |stop_rx = 3 |stop_rx = 3 |stop_rx =3 |          |stop_rx = 3     |stop_rx = 0      |stop_rx = 2      |
 |
 ---|beginning_rx|beginning_rx|beginning_rx|beginning_rx|          |beginning_rx    |beginning_rx    |beginning_rx     |
    |    = 0     |   = 1      |    = 2     |    = 3    |           |    = 3          |    = 3        |    = 3          |

    | gl1_hw_rx_ | gl1_hw_rx_ | gl1_hw_rx_ | gl1_hw_rx_ |          | gl1_hw_rx_     |                |                 |
    |normal_burst|normal_burst|normal_burst|normal_burst|    ....  |normal_burst    |                |                 |

    | initialize |            | Wait       | GET RSSI   |          | GET RSSI       | GET RSSI       | GET RSSI        |
    | rssi, i/q  | rssi = 0;  | rssi update| rssi update|          | rssi update    | rssi update    | rssi update     |
    | dc_offsets |            | dc_off "   | dc_off "   |          | dc_off "       | dc_off "       | dc_off "        |
    | beg_rx++   | beg_rx++   |            |            |          |                | stop_rx++      | stop_rx++       |
    |            |            |            |            |          |  stop_rx = 0   |                |set stop rx flag |
    |-----F1-----|-----F2-----|-----F3-----|-----F3-----|    ....  |-|----F(n-2)----|-----F(n-1)-----|-----Fn----------|
                                                                     |
                                                                     |-----> Set RX_OFF function sets stop_rx = 0
   NOTE: Transit_rx is a flag that is set when we change lna range or arfcn while rx is on.
         This tells us that we need to start re-gathering fresh rssi from mdsp and averaging them.
  */

  /* If the reset transition rx frame count flag is set, reset the frame counter and clear the flag */
  if( gvars->ftm_gsm_rfctl_reset_transit_rx_frame_cnt )
  {
    gvars->burst_metrics.transit_rx_frame_cnt = 0;
    gvars->ftm_gsm_rfctl_reset_transit_rx_frame_cnt = FALSE;
    ftm_gsm_set_ave_rssi_update_flag( rfm_dev, FALSE);
  }

  if(!multi_slot_rx_cal_flag)
  {
    gvars->wait_for_next_lna_range = FALSE;
    gvars->multislot_rx_on = FALSE;
  }

  if(  gvars->ftm_gsm_rfctl_reset_rssi )
  {
     gvars->ftm_gsm_rfctl_reset_rssi = FALSE;
     gvars->burst_metrics.ftm_rssi = 0;
     gvars->burst_metrics.ftm_rssi_counter = 1;
     gvars->burst_metrics.ftm_rssi_counter_drx = 1;
     gvars->burst_metrics.ftm_i_dc_offset = 0;
     gvars->burst_metrics.ftm_q_dc_offset = 0;

     gvars->burst_metrics.ftm_rssi_drx = 0;
     gvars->burst_metrics.ftm_i_dc_offset_drx = 0;
     gvars->burst_metrics.ftm_q_dc_offset_drx = 0;
  }

  // When the lna range  sequence is known, this flag gets triggered
  if( gvars->ftm_gsm_lna_range_ind_flag )
  {

    gvars->local_frame_cnt = gvars->first_call_fn; //store frame count when gain range cal was triggered
    gvars->ftm_gsm_lna_range_ind_flag = FALSE;
    gvars->next_lna_range_order = 0;
    gvars->wait_for_next_lna_range = TRUE;
  }

  if( gvars->wait_for_next_lna_range== TRUE)
  {
    f_count = geran_test_mode_api_get_FN(as_id);

    if( f_count > gvars->local_frame_cnt + gvars->frames_to_wait )
    {

      if(gvars->ftm_gsm_next_lna_range[gvars->next_lna_range_order] != 0xFF)
      {
           // set the next lna range in sequence
           ftm_gsm_set_next_lna_range( rfm_dev,
                                       gvars->ftm_gsm_next_lna_range[gvars->next_lna_range_order]);
           gvars->local_frame_cnt = f_count;
           gvars->next_lna_range_order++;
      }
      else
      {
        gvars->wait_for_next_lna_range = FALSE;
      }

    }


  }

 /* When issuing the first burst , wait 3 frames for RSSI results to be available
    -before reading RSSI.
  */
  if (gvars->beginning_rx >= FTM_GSM_MIN_RX_FRAME_CNT)
  {
    if (gvars->multislot_rx_on )
    {
      for (i = 0; i <GERAN_API_MAX_GL1_DEFS_MAX_DL_MSGS; i++)
      {
        gvars->gsm_rx_multislot_metrics_ptr[i] = &gvars->gsm_rx_multislot_metrics[i];
        gvars->gsm_rx_multislot_metrics[i].arfcn = ftm_gsm_get_arfcn( rfm_dev );
        gvars->gsm_rx_multislot_metrics[i].tn = gvars->signal_info[i].tn;
      }
      if( (gvars->tags_rd_index <  FTM_MAX_GSM_BURST_TAGS) &&
          (gvars->num_rx_slots < GERAN_API_MAX_GL1_DEFS_MAX_DL_MSGS) )
      {
        (void)geran_test_mode_api_get_rx_metrics( gvars->gsm_rx_multislot_metrics_ptr,
                                                  gvars->num_rx_slots,
                                                  FALSE,
                                                  FALSE,
                                                  &gvars->ftm_gsm_rx_multislot_tags[gvars->tags_rd_index][0],
                                                  as_id );
        temp_rssi = 0;
        temp_rssi_drx = 0;
        for (i = 0 ; i < gvars->num_rx_slots; i++)
        {
          temp_rssi += gvars->gsm_rx_multislot_metrics[i].rssi;
          gvars->gsm_rx_multislot_metrics[i].rssi = 0;
          temp_rssi_drx +=  gvars->gsm_rx_multislot_metrics[i].rssi_divrx;
          gvars->gsm_rx_multislot_metrics[i].rssi_divrx = 0;
        }

        gvars->gsm_rx_burst_metrics.rssi = temp_rssi/gvars->num_rx_slots;
        gvars->gsm_rx_burst_metrics.rssi_divrx = temp_rssi_drx/gvars->num_rx_slots;
        ftm_gsm_rx_burst_update_rssi(rfm_dev);
        for (i = 0; i < GERAN_API_MAX_GL1_DEFS_MAX_DL_MSGS ; i++)
        {
          (void)geran_test_mode_api_get_rx_data( FALSE,  /* not PTCCH */
                                                 gvars->gsm_rx_multislot_metrics[i].tn,
                                                 &gvars->ftm_gsm_rx_multislot_tags[gvars->tags_rd_index][i],
                                                 FALSE,
                                                 &data,
                                                 as_id );
        }

      }

    }
    else
    {
      mdsp_rf_send_iq_samples_to_rf( as_id );

      if (gvars->tags_rd_index <FTM_MAX_GSM_BURST_TAGS)
      {
        (void)geran_test_mode_api_get_sync_rx_metrics( &gvars->gsm_rx_burst_metrics,
                                                       &gvars->ftm_gsm_rx_tags[gvars->tags_rd_index],
                                                       as_id );

        gvars->gsm_rx_burst_metrics.valid = 1;

        memscpy( &gvars->gsm_rx_burst_msg_rpts[gvars->tags_rd_index],
                sizeof(gvars->gsm_rx_burst_msg_rpts[gvars->tags_rd_index]),
                &gvars->gsm_rx_burst_metrics,
                sizeof(gl1_defs_metrics_rpt));

        gvars->gsm_rx_burst_msg_ptrs[0] = &gvars->gsm_rx_burst_msg_rpts[0];
        gvars->gsm_rx_burst_msg_ptrs[1] = &gvars->gsm_rx_burst_msg_rpts[1];
        gvars->gsm_rx_burst_msg_ptrs[2] = &gvars->gsm_rx_burst_msg_rpts[2];
        gvars->gsm_rx_burst_msg_ptrs[3] = &gvars->gsm_rx_burst_msg_rpts[3];
      }
      ftm_gsm_rx_burst_update_rssi(rfm_dev);
    }

    if (++gvars->tags_rd_index == FTM_MAX_GSM_BURST_TAGS)
    {
      gvars->tags_rd_index = 0;
      if (!gvars->multislot_rx_on)
      {
        geran_test_mode_api_log_burst_metrics ( GL1_MSG_TCH,
                                                gvars->gsm_rx_burst_msg_ptrs,
                                                as_id );
      }

    }
  }


  if (gvars->stop_rx >= FTM_GSM_MIN_RX_FRAME_CNT)
  {
    gl1_hw_rx_burst_type   burst_info;
    gl1_hw_rx_pkt_burst_type multislot_burst_info;

    /* gl1_hw_schedule_reset_events(); */
    if (!gvars->multislot_rx_on)
    {
      memset( gvars->signal_info, 0, FTM_GSM_RFCTL_SIGNAL_INFO_MEM_SIZE);
    }
    memset(&burst_info, 0, sizeof(burst_info));

    for (i = 0 ; i < GERAN_API_MAX_GL1_DEFS_MAX_DL_MSGS; i++)
    {
      gvars->signal_info[i].rx_signal_strength = (sint15)0x1000; /* Don't set it to 0 */
      gvars->signal_info[i].lna_range          = (uint8)gvars->ftm_gsm_state.ftm_lna_range;  /* Shouldn't need more than 8 bits for LNA gain range */
      gvars->signal_info[i].rx_signal_strength_divrx = (sint15)0x1000; /* Don't set it to 0 */
      gvars->signal_info[i].lna_range_divrx    = (uint8)gvars->ftm_gsm_state.ftm_lna_range_drx;  /* Shouldn't need more than 8 bits for LNA gain range */
    }

    burst_info.channel_type        = GL1_HW_TCH_FS;
    burst_info.search_width        = GL1_HW_TCH_SEARCH_WIDTH;
    burst_info.arfcn               = ftm_gsm_get_arfcn( rfm_dev );
    burst_info.burst_num           = gvars->burst_metrics.burst_num++;

    if (gvars->burst_metrics.burst_num >= 4)
    {
      gvars->burst_metrics.burst_num = 0;
    }
    geran_test_mode_api_set_dedicated(TRUE,as_id);
    geran_test_mode_api_set_idle_mode(FALSE,as_id);

    multislot_burst_info.channel_type        = GL1_HW_PDTCHD;
    multislot_burst_info.search_width        = GL1_HW_GPRS_SEARCH_WIDTH;
    multislot_burst_info.decode_flag        = 1;
    multislot_burst_info.arfcn               = ftm_gsm_get_arfcn( rfm_dev );
    multislot_burst_info.burst_num           =  gvars->burst_metrics.burst_num;

    gvars->burst_metrics.burst_num += gvars->num_rx_slots;
    /* Seheduling Four slots Max for Calibration*/
    if( gvars->burst_metrics.burst_num >= GERAN_API_MAX_GL1_DEFS_MAX_DL_MSGS-1)
    {
      gvars->burst_metrics.burst_num = 0;
    }


    if (gvars->multislot_rx_on)
    {
      geran_test_mode_api_rx_dtm_gprs( &gvars->signal_info[0],
                                       gvars->num_rx_slots,
                                       &multislot_burst_info,
                                       &gvars->ftm_gsm_rx_multislot_tags[gvars->tags_wr_index][0],
                                       as_id );
    }
    else
    {
      if (gvars->tags_wr_index < FTM_MAX_GSM_BURST_TAGS )
      {
        geran_test_mode_api_rx_dtm_gsm( &gvars->signal_info[0],
                                        &burst_info,
                                        &gvars->ftm_gsm_rx_tags[gvars->tags_wr_index],
                                        as_id);
      }
    }

    if (++gvars->tags_wr_index == FTM_MAX_GSM_BURST_TAGS)
    {
      gvars->tags_wr_index = 0;
    }

  }
  else
  {
   /* Wait for three additional frames to collect RSSI which were scheduled before stop rx was set to zero*/
    if (gvars->stop_rx++ == FTM_GSM_MIN_RX_FRAME_CNT-1)
    {
      gvars->ftm_cont_rx = OFF;
    }

    return;
  }


  /*Check if trying to change arfcn or lna_range. IF yes, then reset rssi count*/
  if( gvars->beginning_rx < FTM_GSM_MIN_RX_FRAME_CNT ||
      gvars->burst_metrics.transit_rx_frame_cnt < FTM_GSM_MIN_RX_FRAME_CNT )

  {
    if( gvars->beginning_rx < FTM_GSM_MIN_RX_FRAME_CNT )
    {
      gvars->beginning_rx++;
    }

    if( gvars->burst_metrics.transit_rx_frame_cnt < FTM_GSM_MIN_RX_FRAME_CNT )
    {
      gvars->burst_metrics.transit_rx_frame_cnt++;
    }

    gvars->burst_metrics.ftm_rssi = 0;
    gvars->burst_metrics.ftm_rssi_drx = 0;
    gvars->burst_metrics.ftm_rssi_counter = 1;
    ftm_gsm_set_ave_rssi_update_flag( rfm_dev, FALSE);
    gvars->ftm_gsm_state.ftm_rssi = 0;
    gvars->burst_metrics.ftm_i_dc_offset = 0;
    gvars->burst_metrics.ftm_q_dc_offset = 0;
    gvars->burst_metrics.ftm_i_dc_offset_drx = 0;
    gvars->burst_metrics.ftm_q_dc_offset_drx = 0;
    return;
  } /* if (beginning_rx <FTM_MIN_RX_FRAME_CNT )*/

  if (!gvars->ftm_gsm_rssi_no_frames_avg)
  {
    gvars->ftm_gsm_state.ftm_rssi = 0;
    gvars->ftm_gsm_state.ftm_i_q_dc_offset.ftm_i_dc_offset = 0;
    gvars->ftm_gsm_state.ftm_i_q_dc_offset.ftm_q_dc_offset = 0;
    gvars->ftm_gsm_state.ftm_rssi_drx = 0;
    gvars->burst_metrics.ftm_i_dc_offset_drx = 0;
    gvars->burst_metrics.ftm_q_dc_offset_drx = 0;
    return;
  }

} /* end of ftm_gsm_do_rx_cmd_rx_burst_metrics */


/*===========================================================================

FUNCTION   FTM_GSM_DO_RX_BURST_PROCESSING

DESCRIPTION
  This API is used to schedule a single Rx burst in any slot of a frame.
  Rx burst is scheduled with given ARFCN and LNA state.
  Before scheduling new burst it reads RSSI for bursts programmed in past
  and logs it in ftm frame sweep result buffer

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/

void ftm_gsm_do_rx_burst_processing( rfm_device_enum_type rfm_dev )
{
  uint16 i = 0;
  static uint8 burst_num = 0;
  gl1_hw_rx_burst_type   burst_info;
  sys_modem_as_id_e_type as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );
  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);

  /*************Read RSSI for bursts programmed in past***************/

  /* read RSSI only if it is not first 3 frames, check it using value of beginning_rx*/
  if (gvars->beginning_rx >= FTM_GSM_MIN_RX_FRAME_CNT)
  {
    if (gvars->tags_rd_index < FTM_MAX_GSM_BURST_TAGS)
    {
      (void)geran_test_mode_api_get_sync_rx_metrics( &gvars->gsm_rx_burst_metrics,
                                                     &gvars->ftm_gsm_rx_tags[gvars->tags_rd_index],
                                                     as_id );

      /* log result in sequencer result structure*/
      ftm_gsm_log_rx_burst_matrix(rfm_dev, gvars->gsm_frame_sweep_tags[gvars->tags_rd_index], gvars->gsm_rx_burst_metrics);

      MSG_3(MSG_SSID_FTM, MSG_LEGACY_HIGH,"ftm_gsm_do_rx_burst_processing rfm_dev = %d, RSSI = %d, RSSI_DRX = %d",
                                         rfm_dev, gvars->gsm_rx_burst_metrics.rssi, gvars->gsm_rx_burst_metrics.rssi_divrx);

      /* reset tags_rd_index to 0 if its value is max i.e. 2*/
      if (++gvars->tags_rd_index == FTM_MAX_GSM_BURST_TAGS)
      {
        gvars->tags_rd_index = 0;
      }

      /* save the new RSSI*/
      gvars->ftm_gsm_state.ftm_rssi = gvars->gsm_rx_burst_metrics.rssi;
	  gvars->ftm_gsm_state.ftm_rssi_drx = gvars->gsm_rx_burst_metrics.rssi_divrx;

    }
  }
  else
  {
    /* increment beginning_rx*/
    gvars->beginning_rx++;
    gvars->ftm_gsm_state.ftm_rssi = 0;
    gvars->ftm_gsm_state.ftm_rssi_drx = 0;
  }

  /*************Schedule new burst for this frame***************/

  /* Schedule new burst only if Stop Rx is not issued, check it using value of stop_rx*/
  if (gvars->stop_rx >= FTM_GSM_MIN_RX_FRAME_CNT)
  {
    if (gvars->tags_wr_index < FTM_MAX_GSM_BURST_TAGS )
    {
      /* Schedule a new burst*/
      memset( gvars->signal_info, 0, FTM_GSM_RFCTL_SIGNAL_INFO_MEM_SIZE);
      memset(&burst_info, 0, sizeof(burst_info));

      /* set the Burst parameters*/
      for (i = 0 ; i < GERAN_API_MAX_GL1_DEFS_MAX_DL_MSGS; i++)
      {
        gvars->signal_info[i].tn                 = gvars->rx_slot_num;
        gvars->signal_info[i].rx_signal_strength = (sint15)0x1000; /* Don't set it to 0 */
        gvars->signal_info[i].lna_range          = (uint8)gvars->ftm_gsm_state.ftm_lna_range;
        gvars->signal_info[i].lna_range_divrx    = (uint8)gvars->ftm_gsm_state.ftm_lna_range_drx;
      }

      burst_info.channel_type  = GL1_HW_TCH_FS;
      burst_info.search_width  = GL1_HW_TCH_SEARCH_WIDTH;
      burst_info.arfcn         = ftm_gsm_get_arfcn( rfm_dev );
      burst_info.burst_num     = burst_num++;

      if (burst_num >= 4)
      {
        burst_num = 0;
      }

      geran_test_mode_api_set_dedicated(TRUE,as_id);
      geran_test_mode_api_set_idle_mode(FALSE,as_id);
      geran_test_mode_api_rx_dtm_gsm( &gvars->signal_info[0],
                                               &burst_info,
                                               &gvars->ftm_gsm_rx_tags[gvars->tags_wr_index],
                                               as_id);

      /* save frame sweep result log related info*/
      gvars->gsm_frame_sweep_tags[gvars->tags_wr_index].frame_id        = gvars->frame_sweep_fn_count - 1;
      gvars->gsm_frame_sweep_tags[gvars->tags_wr_index].slot_num        = gvars->rx_slot_num;
      gvars->gsm_frame_sweep_tags[gvars->tags_wr_index].chan            = gvars->ftm_gsm_state.ftm_channel;
      gvars->gsm_frame_sweep_tags[gvars->tags_wr_index].lna_state       = (uint8)gvars->ftm_gsm_state.ftm_lna_range;
      gvars->gsm_frame_sweep_tags[gvars->tags_wr_index].gain_valx16     = (uint16)gvars->ftm_gain_range.valx16;
      gvars->gsm_frame_sweep_tags[gvars->tags_wr_index].lna_state_drx   = (uint8)gvars->ftm_gsm_state.ftm_lna_range_drx;
      gvars->gsm_frame_sweep_tags[gvars->tags_wr_index].gain_valx16_drx = (uint16)gvars->ftm_gain_range_drx.valx16;

      /* reset tags_wr_index to 0 if its value is max i.e. 2*/
      if (++gvars->tags_wr_index == FTM_MAX_GSM_BURST_TAGS)
      {
        gvars->tags_wr_index = 0;
      }
    }
  }
  else
  {
    /* increment stop_rx*/
    gvars->stop_rx++;

    /*Set Rx off if 3 frames have passed*/
    if (gvars->stop_rx == FTM_GSM_MIN_RX_FRAME_CNT)
    {
      gvars->ftm_cont_rx = OFF;
    }
  }
}

/*===========================================================================

FUNCTION   FTM_GSM_GET_LNA_RANGE_FROM_RX_LVL

DESCRIPTION
   When Rx is turned on continuously the Lna range parameter is sent to Rf driver
   through the Rx level parameter. This function tells the RF driver whether it
   should interpret the rx level as an LNA range or not.

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
boolean ftm_gsm_get_lna_range_from_rx_lvl( rfm_device_enum_type rfm_dev )
{
  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);

  return  gvars->ftm_gsm_lna_range_in_rx_lvl_parm;
}
/*===========================================================================

FUNCTION   FTM_GSM_DO_RX_CMD_RX_CONT

DESCRIPTION
   Does receive keeps the rx circuitry on continuously

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void ftm_gsm_do_rx_cmd_rx_cont( rfm_device_enum_type rfm_dev  )
{

  dBx16_T rx_signal_strength;

  sys_modem_as_id_e_type as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );
  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);

  /* stop_rx and beginning_rx used in accordance with the rules described in
  ** ftm_do_rx_cmd_rx_burst_metrics.
  */
  if (gvars->beginning_rx == 0)
  {
    /* Reuse rx_signal_strength variable to pass lna range in case of FTM cont rx command. */
    rx_signal_strength = (sint15)gvars->ftm_gsm_state.ftm_lna_range;

    gvars->ftm_gsm_lna_range_in_rx_lvl_parm = TRUE;
    if (gvars->tags_wr_index < FTM_MAX_GSM_BURST_TAGS)
    {

      geran_test_mode_api_start_acquisition( ftm_gsm_get_arfcn( rfm_dev ),
                                             rx_signal_strength,
                                             0,
                                             0,
                                             FALSE,
                                             GL1_HW_ACQ_GSM_XOCAL,
                                             &gvars->ftm_gsm_rx_tags[gvars->tags_wr_index],
                                             as_id );
    }



    if ( ++gvars->tags_wr_index == FTM_MAX_GSM_BURST_TAGS )
    {
      gvars->tags_wr_index = 0;
    }
  }
  /* When issuing the first burst , wait 3 frames for RSSI results to be available
    -before reading RSSI. Do this make sure there are no errors in ping pong buffers.
  */
  if (gvars->beginning_rx >= FTM_GSM_MIN_RX_FRAME_CNT)
  {
     mdsp_rf_send_iq_samples_to_rf(as_id);
  }


  if (gvars->beginning_rx == FTM_GSM_MIN_RX_FRAME_CNT)
  {
    if (gvars->tags_rd_index < FTM_MAX_GSM_BURST_TAGS)
    {
      (void)geran_test_mode_api_get_sync_rx_metrics(&gvars->gsm_rx_burst_metrics,
                                                    &gvars->ftm_gsm_rx_tags[gvars->tags_rd_index],
                                                    as_id);

      gvars->gsm_rx_burst_metrics.valid = 1;

      memscpy( &gvars->gsm_rx_burst_msg_rpts[gvars->tags_rd_index],
              sizeof(gvars->gsm_rx_burst_msg_rpts[gvars->tags_rd_index]),
              &gvars->gsm_rx_burst_metrics,
              sizeof(gl1_defs_metrics_rpt) );

      gvars->gsm_rx_burst_msg_ptrs[0] = &gvars->gsm_rx_burst_msg_rpts[0];
      gvars->gsm_rx_burst_msg_ptrs[1] = &gvars->gsm_rx_burst_msg_rpts[1];
      gvars->gsm_rx_burst_msg_ptrs[2] = &gvars->gsm_rx_burst_msg_rpts[2];
      gvars->gsm_rx_burst_msg_ptrs[3] = &gvars->gsm_rx_burst_msg_rpts[3];
    }
    if (++gvars->tags_rd_index >= FTM_MAX_GSM_BURST_TAGS)
    {
      gvars->tags_rd_index = 0;
      geran_test_mode_api_log_burst_metrics ( GL1_MSG_TCH,
                                              gvars->gsm_rx_burst_msg_ptrs,
                                              as_id );
    }
    //av 4/29: updated ftm_rssi for cont..
    gvars->ftm_gsm_state.ftm_rssi = gvars->gsm_rx_burst_metrics.rssi;
    gvars->ftm_gsm_state.ftm_rssi_drx = gvars->gsm_rx_burst_metrics.rssi_divrx;
  }


  if (gvars->beginning_rx <= FTM_GSM_MIN_RX_FRAME_CNT  )
  {
    gvars->beginning_rx++;
  }

  /*Stop RX has to be in accordance with the rules described
    in ftm_do_rx_cmd_rx_burst_metrics
    i.e. three frames wait: stop_rx=0,stop_rx=1, stop_rx=2;
  */

  if ( gvars->stop_rx == FTM_GSM_MIN_RX_FRAME_CNT-1 )
  {
    geran_test_mode_api_stop_acquisition(0,as_id);

    gvars->ftm_cont_rx = OFF;
    gvars->ftm_gsm_lna_range_in_rx_lvl_parm = FALSE;
  }

  if ( gvars->stop_rx < FTM_GSM_MIN_RX_FRAME_CNT )
  {
      gvars->stop_rx++;
  }
} /* end of ftm_gsm_do_rx_cmd_rx_cont */

/*===========================================================================

FUNCTION  ftm_set_delay_match_cfg

DESCRIPTION
  This function set the delay for envelope and IQ path

DEPENDENCIES
   None
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/

int16 ftm_gsm_set_delay_match_cfg( rfm_device_enum_type rfm_dev,
                                   int16 delay_value )
{
  ftm_gsm_rf_task_set_path_delay( rfm_dev, delay_value );

  return(delay_value);
} /* end of ftm_set_delay_match_cfg */



/*===========================================================================
FUNCTION   FTM_GSM_WRITE_DATA_BLOCK

DESCRIPTION
   This function will write a block of data to mdsp.  Note, the modulation
   coding scheme (MCS) is part of the data block.

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void ftm_gsm_write_data_block( rfm_device_enum_type rfm_dev )
{
  gl1_defs_tx_pkt_data_type ftm_data;
  uint8  i, num_slot=0;

  sys_modem_as_id_e_type as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );
  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);

  RFGSM_SLOT_LOCK(&rfgsm_slot_mutex);

  for (i=0; i<FTM_MAX_GSM_SLOT_PER_FRAME; i++)
  {
    if (num_slot < FTM_MAX_GSM_TX_SLOT)
    {
      if( gvars->ftm_slot_assign[i].on_off )
      {
        if (gvars->tx_brst.data_source == (enum_type)FTM_TX_DATA_SOURCE_PSDRND )
        {
          /* Get data for the next slot */
          ftm_data.gprs.data =(uint8*) &ftm_multi_slot_data[gvars->data_mod*FTM_MAX_BLOCK_OCTET];
          ftm_data.egprs.data[0] = (uint8*)&ftm_multi_slot_data[gvars->data_mod*FTM_MAX_BLOCK_OCTET];
          ftm_data.egprs.data[1] = (uint8*)&ftm_multi_slot_data[(gvars->data_mod+1)*FTM_MAX_BLOCK_OCTET];

          gvars->data_mod += 2;             /* 2 buffers for MCS7-MCS9 */
          gvars->data_mod = gvars->data_mod % 40;  /* 40 = 2960 / FTM_MAX_BLOCK_OCTET */
        }
        else
        {
          /* Get data for the next slot */
          ftm_data.gprs.data = (uint8*)&ftm_multi_slot_data_tone[0];
          ftm_data.egprs.data[0] = (uint8*)&ftm_multi_slot_data_tone[0];
          ftm_data.egprs.data[1] = (uint8*)&ftm_multi_slot_data_tone[0];
        }

        ftm_data.egprs.puncturing[0] = (gl1_defs_puncturing_type)0;
        ftm_data.egprs.puncturing[1] = (gl1_defs_puncturing_type)0;


        /* Write data to mdsp memory*/
        if( !geran_test_mode_api_write_tx_pkt_data( &ftm_data,
                                                    FTM_MAX_BLOCK_OCTET,
                                                    (gl1_defs_tn_type)gvars->ftm_slot_assign[i].slot_num,
                                                    GL1_HW_PDCH,
                                                    gvars->ftm_slot_assign[i].mcs,
                                                    &gvars->ftm_slot_assign[i].tags,
                                                    as_id) )
        {
          MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"Cannot write to data buffer",0);
        }
        num_slot++;
      }
    }
  }
  RFGSM_SLOT_UNLOCK(&rfgsm_slot_mutex);

} /* end of ftm_gsm_write_data_block */

/*===========================================================================

FUNCTION   FTM_GET_BURST_INFO

DESCRIPTION
   This function fill in the tx burst information.

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void ftm_get_burst_info( rfm_device_enum_type rfm_dev,
                         gl1_hw_tx_pkt_burst_type *burst_infor)
{
  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);

  burst_infor->arfcn.band = gvars->ftm_gl1_hw_gsm_band;
  burst_infor->arfcn.num = gvars->ftm_gsm_state.ftm_channel;
  burst_infor->tsc = gvars->tx_brst.tsc_index;
  burst_infor->bsic = 0;
  burst_infor->timing_advance = 0;
  burst_infor->encode_flag = (gvars->ftm_FNmod4 == 0 ? TRUE : FALSE);
  burst_infor->loopback_flag = FALSE;
} /* end of ftm_get_burst_info */

/*===========================================================================

FUNCTION   FTM_GSM_TX_BURST

DESCRIPTION
   This function transmit a block of data by encode it and then transmit
   one frame at a time.

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void ftm_gsm_tx_burst(rfm_device_enum_type rfm_dev)
{
  gl1_hw_tx_pkt_burst_type ftm_burst_info;
  volatile uint8 num_slot=0;
  uint32 i;
  gl1_hw_tx_tags_type tags[FTM_MAX_GSM_TX_SLOT];

  boolean update = TRUE;
  sys_modem_as_id_e_type as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );
  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!");

  /* If stop_tx_triggered flag is set, disable Tx once ftm_FNmod4 becomes 0*/
  if(gvars->stop_tx_triggered == TRUE && gvars->ftm_FNmod4 == 0)
  {
     gvars->ftm_gsm_state.ftm_tx_state = FALSE;
     gvars->ftm_cont_tx = FALSE;
     gvars->stop_tx_triggered = FALSE;
     /*Enable persistent flag if disabled by frame sweep sequence previously*/
     gvars->tx_frame_matrix_persistent = TRUE;
     MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"ftm_gsm_tx_burst, turning off Tx",0);
     return;
  }

  if (gvars->ftm_FNmod4 == 0)
  {
    /* We should only write data at the beginning of a block */
    ftm_gsm_write_data_block( rfm_dev );
  }

  /* fill in the signal information for the burst */
  num_slot=0;

  RFGSM_SLOT_LOCK(&rfgsm_slot_mutex);

  for (i=0; i<FTM_MAX_GSM_SLOT_PER_FRAME; i++)
  {
    if( num_slot < FTM_MAX_GSM_TX_SLOT )
    {
      if (gvars->ftm_slot_assign[i].on_off)
      {
        if(gvars->ftm_slot_assign[i].powdBm != (int16)0xFFFF)  //typecast to get correct condition check
        {
          if( gvars->last_power[i] != gvars->ftm_slot_assign[i].powdBm && update)
          {
            gvars->last_power[i] = gvars->ftm_slot_assign[i].powdBm;
            rfgsm_core_set_tx_profile_update_flag(rfgsm_core_handle_ptr);
            update = FALSE;
          }

          gvars->ftm_tx_signal[num_slot].tx_power_index =
            ftm_gsm_set_tx_pow_dbm( rfm_dev, gvars->ftm_slot_assign[i].powdBm, (int8)num_slot);

          /* After updating the TX power in rf task context, reset the value.
             This to avoid updating the same TX power to the ftm slot override variable( in RF task) every frame.
             The request should only be updated if there is a new TX power request via set frame matrix*/
          gvars->ftm_slot_assign[i].powdBm = 0xFFFF;
        }

        gvars->ftm_tx_signal[num_slot].tn = (gl1_defs_tn_type)gvars->ftm_slot_assign[i].slot_num;

        if (gvars->ftm_slot_assign[i].mcs < GL1_HW_PDTCHU_MCS5 )
        {
          gvars->ftm_tx_signal[num_slot].modulation = GL1_DEFS_MODULATION_GMSK;
          rfgsm_core_handle_ptr->rfgsm_core_tx_pow_mod[num_slot].modulation = RF_MOD_GMSK;
        }
        else if (gvars->ftm_slot_assign[i].mcs <= GL1_HW_PDTCHU_MCS9 )
        {
          gvars->ftm_tx_signal[num_slot].modulation = GL1_DEFS_MODULATION_8PSK;
          rfgsm_core_handle_ptr->rfgsm_core_tx_pow_mod[num_slot].modulation = RF_MOD_8PSK;
        }

        /* Check the schedule on frame 2, discard untxed messages */
        if (gvars->ftm_FNmod4 == 2)
        {
          (void)geran_test_mode_api_get_tx_scheduled_result(&gvars->ftm_slot_assign[i].tags, TRUE,as_id);
        }

        /* Save tag */
        tags[num_slot] = gvars->ftm_slot_assign[i].tags;

        num_slot++;

        /* if frame matrix is not persistent, turn off slot in last frame of a 4 frame block*/
        if (gvars->tx_frame_matrix_persistent == FALSE && gvars->ftm_FNmod4 == 3)
        {
          gvars->ftm_slot_assign[i].on_off = FALSE;
          gvars->ftm_slot_assign[i].powdBm = 0xFFFF;
        }
      }
    }
  }

  RFGSM_SLOT_UNLOCK(&rfgsm_slot_mutex);

  /* get the tx burst information */
  ftm_get_burst_info( rfm_dev, &ftm_burst_info);

  /* transmit the burst */
   geran_test_mode_api_tx_gprs( gvars->ftm_tx_signal,
						  num_slot,
						  &ftm_burst_info,
						  tags,
						  num_slot,
						  num_slot,
						  as_id );

  /* Increment the frame number mod 4*/
  gvars->ftm_FNmod4++;
  gvars->ftm_FNmod4 = gvars->ftm_FNmod4 % 4;

} /* end of ftm_gsm_tx_burst */

/*===========================================================================

FUNCTION  ftm_set_frame_matrix

DESCRIPTION
  This function set burst frame matrix.  Note, it only set one slot at
  a time.  Need to call 7 times to configure all 7 slots.

DEPENDENCIES
   None
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void ftm_gsm_set_frame_matrix( rfm_device_enum_type rfm_dev,
                           const ftm_frame_matrix_type *bm)
{
  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);

  RFGSM_SLOT_LOCK(&rfgsm_slot_mutex);

  /* Slot ON_OFF state and mcs value should only be changed in first frame of 4 frame block*/
  /* note that gvars->ftm_FNmod4 will always be 0 if Tx is not bursting*/
  if (gvars->ftm_FNmod4 == 0)
  {
    gvars->ftm_slot_assign[bm->slot_num & 0x7].on_off = bm->on_off;

  }
  /* mcs1 maps to 0, mcs2 maps to 1, and so on..*/
  gvars->ftm_slot_assign[bm->slot_num & 0x7].mcs =
  (gl1_hw_channel_type)(bm->mcs+GL1_HW_PDTCHU_MCS1);

  gvars->ftm_slot_assign[bm->slot_num & 0x7].powdBm = bm->power_dB;
  ftm_gsm_set_slot_override_flag( rfm_dev, FALSE );

  /*Disable*/
  gvars->ftm_gsm_state.ftm_gsm_padac_override_val = 0xFFFF;
  RFGSM_SLOT_UNLOCK(&rfgsm_slot_mutex);

  MSG_2(MSG_SSID_FTM, MSG_LEGACY_MED,
        "ftm_gsm_set_frame_matrix():ftm_slot_assign[%d].mcs=%d ",
        bm->slot_num, gvars->ftm_slot_assign[bm->slot_num& 0x7].mcs);

} /* end of ftm_set_frame_matrix */


/*===========================================================================

FUNCTION   ftm_count_numbits.

DESCRIPTION
   Counts the number of bits sets in a byte.

DEPENDENCIES
   None

RETURN VALUE
   Number of set bits in the input byte.

SIDE EFFECTS
   None

===========================================================================*/
int8 ftm_gsm_count_numbits(uint8 val)
{
  /* Based on code found in web from
  ** http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetKernighan
  */
  int c;
  /*--------------------------------*/
  for ( c = 0 ; val; c++ )
  {
    val &= val-1;
  }
  return (int8)c;
}

/*===========================================================================

FUNCTION   ftm_gsm_set_no_frame_avg.

DESCRIPTION
   Sets the number of frames over which RSSI is averaged.

DEPENDENCIES
   This function should only be called from ftm_gsm_rx_burst_command,
   to ensure that we are not using some random value. The value to be used
   is sent as parameter to ftm_gsm_rx_burst_command and thus passed to this function.

RETURN VALUE
   Number of set bits in the input byte.

SIDE EFFECTS
   None

===========================================================================*/
void ftm_gsm_set_no_frame_avg ( rfm_device_enum_type rfm_dev,
                                uint8 val )
{
  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);

  if (val != 0)
  {
    gvars->ftm_gsm_rssi_no_frames_avg = val;
  }
  else
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"Num frames cannot be zero. Rssi will be set to zero", 0);
  }
}

/*===========================================================================

FUNCTION   ftm_gsm_do_rx_calibration.

DESCRIPTION
   Accepts a list of channels and a bitmask of which gain ranges to calibrate
   for each channel. Then returns the RSSI results for each of the channel/gain
   range combinations.

DEPENDENCIES
   None

RETURN VALUE
   List of RSSI readings for each of the input combinations.

SIDE EFFECTS
   None

===========================================================================*/
void  ftm_gsm_do_rx_calibration( rfm_device_enum_type rfm_dev,
                                 ftm_gsm_rxcal_parms_prx_drx_type * parm_ptr)
{
  uint8 i=0;uint8 j =0;
  ftm_rx_burst_type rx_burst_parms;
  uint32 local_frame_count=0;
  uint8 result_count = 0;
  int numbits;
  uint8 num_frames = 0;
  uint8 tn_array[5] = {0,1,2,3,0xF} ;
  uint16 lna_range_order=0;
  uint8 adjusted_num_frames;
  boolean multi_slot_rx_flag = FALSE;
  uint16 set_lna_count =0; // this keeps track of how many times lna range has been changed


  sys_modem_as_id_e_type as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );
  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);


  multi_slot_rx_flag = gvars->do_multi_slot_rx_cal_flag;


  if(multi_slot_rx_flag)
  {
    // Calculate the number of frames from number of time-slots passed by the tool
    num_frames = ( parm_ptr->num_avgs / 4) + ((parm_ptr->num_avgs % 4 > 0)? 1 : 0) ;

    //reset the lna range sequence everytime RX gain range cal is triggered
    for (i=0;i<8;i++)
    {
      gvars->ftm_gsm_next_lna_range[i] = 0xFF;
    }

    set_lna_count =0;

    if( num_frames <= FTM_GSM_MIN_RX_FRAME_CNT )
    {
      gvars->frames_to_wait = FTM_GSM_MIN_RX_FRAME_CNT ;
    }
    else
    {
      gvars->frames_to_wait = num_frames ;
    }

  }

  /*-----------------------------------------------------------------*/
  /* If we are asked to toggle the state of the receiver ,
     If rxBurst is OFF we should turn it on. */
  if ( parm_ptr->toggle_rx_on && !gvars->rxBurstOn )
  {
    /* Program rx burst ON. */
    /*lint -e{641} */
    rx_burst_parms.ftm_rx_funct = (ftm_enum_rx_function_type) FTM_RX_FUNCTION_GET_RSSI;
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"Number of frames to be averaged: %d", num_frames);

    if(multi_slot_rx_flag)
    {
      //set time slot numbers and turn on multi-slot rx
      ftm_gsm_set_multislot_rx( rfm_dev, tn_array);

      //pass in the actual number of frames in case of multi-slot scenario
      ftm_gsm_rx_burst_command( rfm_dev, &rx_burst_parms,num_frames);

      mdsp_ftm_wait_for_frame_tick(as_id);
    }
    else
    {
      ftm_gsm_rx_burst_command( rfm_dev,&rx_burst_parms,parm_ptr->num_avgs);
    }

    /* This command will wait for a number of frames to collect RSSI for all gain range.
    ** Turn off the 10sec timers for Vbatt and temp comp, so that we are not interrupted.
    */

    /* Stop Vbatt Tx pwr compensation. */
    rfgsm_core_vbatt_comp_stop(rfm_dev);


    /* Stop PA Temp pwr compensation. */
    rfgsm_core_temp_comp_stop(rfm_dev);


    parm_ptr->toggle_rx_on = 0;
  }

  /* Gain range bitmask has 8 positions. Loop through those and
     check which gain ranges are chosen for calibration          */
  numbits = ftm_gsm_count_numbits(parm_ptr->gain_range_bitmask);

  for ( i=0; i<8 ; i++ )
  {
    if(multi_slot_rx_flag)
    {
      /* If i'th bit location is set, then we need to calibrate this gain range.*/
      if ( parm_ptr->gain_range_bitmask & (0x1<<i) )
      {
        set_lna_count ++;

        if(set_lna_count == 1) // i.e. for the first lna range
        {
          //record the frame number when RX cal for a particular band and RX pwr level is triggered
          gvars->first_call_fn = geran_test_mode_api_get_FN(as_id);

          /* Set the first LNA range */
          ftm_gsm_set_lna_range( rfm_dev, (ftm_lna_range_type)i);

          //find next lna ranges in sequence
          for ( j=(i+1); j<8 ; j++ )
          {
            if (( parm_ptr->gain_range_bitmask & (0x1<<j) ) )
            {
              gvars->ftm_gsm_next_lna_range[lna_range_order]= j; // lna ranges extracted from the bit-mask are stored in sequence

              if (lna_range_order < 7)
              {
                lna_range_order++;
              }
            }

          }

          // Once the lna range sequence is calculated, an indicator flag is triggered
          //ftm_gsm_lna_range_ind_flag = TRUE;
        }

        /* Read current frame number to calculate the frames to wait/ waited before collecting Avg RSSI for one LNA range */
        local_frame_count= geran_test_mode_api_get_FN(as_id) ;

        if(set_lna_count == 1) // i.e. for the first lna range
        {
          adjusted_num_frames = num_frames + FTM_GSM_MIN_RX_FRAME_CNT;
          // Compensate for
          if(gvars->first_call_fn != local_frame_count)
          // This can happen if the first lna range is set at the end of previous frame and before local_frame_count is updated, it is the next frame
          {
            gvars->first_call_fn = local_frame_count;
          }

            // Once the lna range sequence is calculated, an indicator flag is triggered
          gvars->ftm_gsm_lna_range_ind_flag = TRUE;

          /* wait a certain number of frames before getting the RSSI averaged. */
          while ( geran_test_mode_api_get_FN(as_id) <= local_frame_count + adjusted_num_frames ) //while ( ftm_frame_count <= local_frame_count + adjusted_num_frames )
          {
            ftm_clk_rex_wait(1); /* this is to unblock other waiting threads. */
          }
        }
        else
        {
          // This case is for subsequent lna ranges

          adjusted_num_frames = gvars->frames_to_wait;

          //Reset RSSI before accumulating for the subsequent lna ranges
          gvars->ftm_gsm_rfctl_reset_rssi = TRUE;

          while ( geran_test_mode_api_get_FN(as_id) <= local_frame_count+adjusted_num_frames )
          {
            ftm_clk_rex_wait(1); // this is to unblock other waiting threads.
          }
        }

        /* save result */
        parm_ptr->rssi_result[i] = gvars->ftm_gsm_state.ftm_rssi;
        parm_ptr->rssi_result_rxd[i] = gvars->ftm_gsm_state.ftm_rssi_drx;

        result_count++;

        /* If we've got the results for all the gain ranges... bail out of loop*/
        if ( result_count >= numbits )
        {
          break;
        }
      }
      /* else just continue. */
    }
    else
    {
    /* If i'th bit location is set, then we need to calibrate this gain range.*/
    if ( parm_ptr->gain_range_bitmask & (0x1<<i) )
    {
      /* Set LNA range */
      ftm_gsm_set_lna_range( rfm_dev, (ftm_lna_range_type)i);

      /* Read current frame number and send Start GSM rx */
      local_frame_count= gvars->ftm_frame_count ;

      /* wait num_avg frames +2 to get the RSSI averaged. */
      while ( gvars->ftm_frame_count <= local_frame_count+parm_ptr->num_avgs+FTM_GSM_MIN_RX_FRAME_CNT )
      {
        ftm_clk_rex_wait(1); /* this is to unblock other waiting threads. */
      }

      /* save result */
      parm_ptr->rssi_result[i] = gvars->ftm_gsm_state.ftm_rssi;
      parm_ptr->rssi_result_rxd[i] = gvars->ftm_gsm_state.ftm_rssi_drx;

      result_count++;

      /* If we've got the results for all the gain ranges... bail out of loop*/
      if ( result_count >= numbits )
      {
        break;
      }
    }
    }
  }

  /* If we are asked to toggle the state of reciever and it its ON, turn it off. */
  if ( parm_ptr->toggle_rx_on && gvars->rxBurstOn )
  {

    /*lint -e{641}*/
    rx_burst_parms.ftm_rx_funct = (ftm_enum_rx_function_type) FTM_RX_FUNCTION_STOP_CONTINOUS_RX;

    if(multi_slot_rx_flag)
    {
      ftm_gsm_rx_burst_command( rfm_dev, &rx_burst_parms, num_frames);
    }
    else
    {
      ftm_gsm_rx_burst_command( rfm_dev, &rx_burst_parms, parm_ptr->num_avgs);
    }

    parm_ptr->toggle_rx_on = 0;

    /* Turn on the 10sec timers for Vbatt and temp comp, before exiting function. */

    /* Stop Vbatt Tx pwr compensation. */
    rfgsm_core_vbatt_comp_start(rfm_dev);


    /* Stop PA Temp pwr compensation. */
    if(rfm_dev == RFM_DEVICE_2)
    {
      rfgsm_core_temp_comp_start(rfm_dev,RFLM_GSM_SUBSCRIPTION_2);
    }
    else
    {
      rfgsm_core_temp_comp_start(rfm_dev,RFLM_GSM_DEFAULT_SUBSCRIPTION);
    }

  }

}


/*===========================================================================

FUNCTION   ftm_gsm_do_rx_calibration_prx.

DESCRIPTION
   Accepts a list of channels and a bitmask of which gain ranges to calibrate
   for each channel. Then returns the RSSI results for each of the channel/gain
   range combinations.

DEPENDENCIES
   None

RETURN VALUE
   List of RSSI readings for each of the input combinations.

SIDE EFFECTS
   None

===========================================================================*/
void  ftm_gsm_do_rx_calibration_prx( rfm_device_enum_type rfm_dev,
                                     ftm_gsm_rxcal_parms_type * parm_ptr )
{
  /* Use a temporary prx_drx parameters structure to call ftm_gsm_do_rx_calibration*/
  ftm_gsm_rxcal_parms_prx_drx_type params;

  /* Copy the RX cal parameters to the temporary structure */
  params.gain_range_bitmask = parm_ptr->gain_range_bitmask;
  params.num_avgs = parm_ptr->num_avgs;
  params.toggle_rx_on = parm_ptr->toggle_rx_on;

  /* Perform the calibration */
  ftm_gsm_do_rx_calibration(rfm_dev, &params );

  /* Copy the results back to the original parameters ptr discarding the rxd results */
  memscpy( parm_ptr, sizeof(ftm_gsm_rxcal_parms_type),
           &params, sizeof(ftm_gsm_rxcal_parms_type) );
}


/*===========================================================================

FUNCTION   ftm_gsm_do_rx_calibration_prx_drx.

DESCRIPTION
   Accepts a list of channels and a bitmask of which gain ranges to calibrate
   for each channel. Then returns the RSSI results for each of the channel/gain
   range combinations.

DEPENDENCIES
   None

RETURN VALUE
   List of RSSI readings for each of the input combinations.

SIDE EFFECTS
   None

===========================================================================*/
void  ftm_gsm_do_rx_calibration_prx_drx( rfm_device_enum_type rfm_dev,
                                         ftm_gsm_rxcal_parms_prx_drx_type * parm_ptr )
{
  ftm_gsm_do_rx_calibration(rfm_dev, parm_ptr );
}

/*===========================================================================

FUNCTION FTM_GSM_CAL_KV_AVE

DESCRIPTION
   This function will perform Kv calibration.  It will perform KV reading
   without turning off the modulator.

DEPENDENCIES
   Make sure the band and channel are setup correctly before callign this API.

RETURN VALUE
   Kv calibration code.

SIDE EFFECTS
   None.

===========================================================================*/
boolean ftm_gsm_cal_kv_ave( rfm_device_enum_type rfm_dev, rfgsmlib_kv_code_type *kv_value )
{
  boolean sucess_flag=TRUE;

  sys_modem_as_id_e_type as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );

  rfdevice_gsm_enable_kv_cal(rfm_dev,TRUE);

  /* Turn on TX */
  ftm_gsm_set_tx( rfm_dev, ON);

  /* MDSP process command on the frame tick boundary. Wait for it to process
   * the TX on command and 1 extra for security.  :)
   */
  mdsp_ftm_wait_for_frame_tick(as_id);
  mdsp_ftm_wait_for_frame_tick(as_id);
  mdsp_ftm_wait_for_frame_tick(as_id);

  ftm_clk_rex_wait(RFDEVICE_GSM_KV_CAL_DELAY);

  ftm_clk_rex_wait(2);

  sucess_flag = rfdevice_gsm_execute_kv_cal( rfm_dev,
                                             kv_value,
                                             ftm_gsm_get_vars_ptr(rfm_dev)->ftm_gsm_rfctl_kv_cal_per_tx_on );


  /* Disable KV adaptation */
  rfdevice_gsm_enable_kv_cal(rfm_dev,FALSE);

  /* Turn off TX */
  ftm_gsm_set_tx( rfm_dev, OFF);
  /* wait for 3 frame ticks, 1 FT is for frame tick ISR,
   1 FT is for RF to fill mdsp burst buffers and
   1 FT is for mdsp to process the buffers */
  mdsp_ftm_wait_for_frame_tick(as_id);
  mdsp_ftm_wait_for_frame_tick(as_id);
  mdsp_ftm_wait_for_frame_tick(as_id);
  return(sucess_flag);
}

/*===========================================================================

FUNCTION FTM_GSM_CAL_KV

DESCRIPTION
   This function will perform Kv calibration.  TX on will be turning on and off.

DEPENDENCIES
   Make sure the band and channel are setup correctly before callign this API.

RETURN VALUE
   Kv calibration code.

SIDE EFFECTS
   None.

===========================================================================*/
/*! @shahj: KV Cal ported from LCU. Initial version of RTR8600 does not
    have access to fine tune register. Hence, range checking on fine tune
    register is bypassed. Need to updated when newer version of RTR8600 has
    the correct hooks to tap into the fine tune register.
*/
uint16 ftm_gsm_cal_kv( rfm_device_enum_type rfm_dev )
{
  uint16  i;
  int32   kv_code=0;
  int32   final_kv=0;
  int32   temp=0;
  uint16  final_r1=0;
  uint16  success_counter=0;
  uint16  retry_counter=0;
  rfgsmlib_kv_code_type  kv_data;
  boolean debug_flag;

  debug_flag = rfgsm_nv_check_debug_nv( rfm_dev, RFGSM_DEBUG_NV_KV_CAL_LOGGING_ENABLE);

  for (i=0; i<RFDEVICE_GSM_KV_CAL_NUM_OF_RERUN_MAX; i++)
  {
    if (ftm_gsm_cal_kv_ave(rfm_dev,&kv_data) == TRUE)
    {
      /* Cast the KV code to int32 to prevent overflow as part of averaging computation */
      kv_code = (int32) kv_data.kv_code;

      /* The kv code is a 15 bit 2's complement number.  The following code
       * performs a sign extention to 32 bits number for averaging purpose.
       */
      if ( kv_code & RFDEVICE_GSM_KV_CAL_SIGN )
      {
        kv_code = (int32)((uint32)kv_code | RFDEVICE_GSM_KV_CAL_SIGN_EXT);
      }

      final_kv += kv_code;
      final_r1 += (uint16)kv_data.kv_code_r1;
      success_counter++;
    }
    else
    {
      retry_counter++;
    }

    ftm_clk_rex_wait(RFDEVICE_GSM_KV_CAL_DELAY);

    /* Exit if there is enough data */
    if (success_counter >= ftm_gsm_get_vars_ptr(rfm_dev)->ftm_gsm_rfctl_kv_tx_on_off)
    {
      break;
    }
  }

  if (success_counter > 0)
  {
    final_kv /= success_counter;
    final_r1 /= success_counter;
    temp = rfdevice_gsm_compute_final_kv_cal(rfm_dev, (int16)final_kv, final_r1);

    if (debug_flag == TRUE)
    {
       MSG_2( MSG_SSID_FTM,
              MSG_LEGACY_HIGH,
              "Counter sucess/retry:  %d / %d ",
              success_counter,retry_counter );

       MSG_3( MSG_SSID_FTM,
              MSG_LEGACY_HIGH,
              "Final value KV/R/iKV:      %d    %d    %d",
              final_kv,final_r1,temp );
    }

    final_kv = temp;
  }
  else
  {
    /* Set to indicate calibration is not okay.  Need re-run */
    final_kv = (int16)0x8000;
  }

  /* Set the calibrated code.  Note, we normally perform two KV calibration per band.
   * Thus, only the last KV calibration code is written to the RFA.
   */
  rfdevice_gsm_set_kv_cal_code(rfm_dev,(uint16)final_kv);

  return((uint16)final_kv);
}

/*===========================================================================

FUNCTION ftm_set_pa_range

DESCRIPTION
   This function will force the r0/r1 settings as required.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_gsm_set_pa_range( rfm_device_enum_type rfm_dev,
                       const ftm_gsm_pa_range_type * pa_range_params)
{
  uint8 num_slot = 0;
  int i = 0;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  if (rfgsm_core_handle_ptr == NULL)
  {
  	MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
  	return;
  }

  /* Load the current RF gain buffer into FTM before changing the R0/R1 value.*/
  RFGSM_SLOT_LOCK(&rfgsm_slot_mutex);
  for ( i=0; i<FTM_MAX_GSM_SLOT_PER_FRAME; i++ )
  {
    if ( (gvars->ftm_slot_assign[i].on_off) && (num_slot < FTM_MAX_GSM_TX_SLOT) )
    {
      if (pa_range_params->slot_num == i )
      {
        ftm_gsm_rf_task_set_pa_range( rfm_dev,
                                      num_slot,
                                      pa_range_params->pa_range,
                                      (gvars->ftm_slot_assign[i].mcs < GL1_HW_PDTCHU_MCS5)?RF_MOD_GMSK:RF_MOD_8PSK );

      }
      num_slot++;
    }
  }
  RFGSM_SLOT_UNLOCK(&rfgsm_slot_mutex);
  }

/*===========================================================================

FUNCTION ftm_gsm_set_linear_rgi

DESCRIPTION
   This function will force the rgi settings as required. It will also force edge env gain to either the
   gsm or edge value as requested. The previous pa range setting , for the specified slot, will be maintained .

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_gsm_set_linear_rgi( rfm_device_enum_type rfm_dev,
                             const ftm_gsm_linear_rgi_type * rgi_params)
{
  uint8 i = 0, rf_slot_num = 0;
  rfgsm_modulation_type mod_type;
  uint16 env_gain;
  gsm_linear_tx_gain_param_type tx_gain_param;
  rfc_gsm_core_config_type *rfc_gsm_core_ptr = NULL;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  uint16 pa_scaling = RFGSM_MSM_PA_SCALE_UNITY_GAIN;
  uint16 smps_bias = 0;

  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);

  rfgsm_core_handle_ptr = rfgsm_core_handle_get( rfm_dev );

  if (rfgsm_core_handle_ptr == NULL)
  {
	MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
	return;
  }

  rfc_gsm_core_ptr = rfgsm_core_handle_ptr->rfc_gsm_core_ptr;
  if( rfc_gsm_core_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"gsm rfc_ptr is NULL! Exiting set linear rgi! ", 0);
    return;
  }

  mod_type = (rgi_params->mod_type_is_gsm == 1)? RF_MOD_GMSK: RF_MOD_8PSK;

  tx_gain_param  = rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->tx_gain_param;

  env_gain = (rgi_params->mod_type_is_gsm == 1)? tx_gain_param.gsm_linear_tx_gain_val:tx_gain_param.edge_linear_tx_gain_val;


  RFGSM_SLOT_LOCK(&rfgsm_slot_mutex);
  for ( i=0; i<FTM_MAX_GSM_SLOT_PER_FRAME; i++ )
  {
    if ((gvars->ftm_slot_assign[i].on_off) && (rf_slot_num < FTM_MAX_GSM_TX_SLOT))
    {
      if (rgi_params->slot_num == i)
      {

        /* reset the dBm pwr to default 0xFFFF value */
        gvars->ftm_slot_assign[i].powdBm = 0xFFFF;

        ftm_gsm_rf_task_set_linear_rgi( rfm_dev,
                                          rgi_params->rgi,
                                          env_gain,
                                          mod_type,
                                          rf_slot_num,
                                          TRUE ,
                                          pa_scaling,
                                          smps_bias);


      }

      rf_slot_num++;
    }
  }
  RFGSM_SLOT_UNLOCK(&rfgsm_slot_mutex);

}




/*===========================================================================

FUNCTION ftm_gsm_set_tx_slot_config

DESCRIPTION
   This function will force the pa state, rgi settings and env_gain for a given rfm device and tx slot.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_gsm_set_tx_slot_config( rfm_device_enum_type rfm_dev,
                             const ftm_gsm_tx_slot_config_type * tx_slot_config)
{
  uint8 i = 0;
  uint16 num_slot = 0;
  uint16 env_gain = 0 ;
  uint16 pa_scaling = 0;
  uint16 smps_bias = 0;
  rfgsm_modulation_type mod_type = RF_MOD_GMSK;
  rfc_gsm_core_config_type *rfc_gsm_core_ptr = NULL;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  gsm_linear_tx_gain_param_type tx_gain_param;

  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);

  rfgsm_core_handle_ptr = rfgsm_core_handle_get( rfm_dev );

  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return;
  }

  rfc_gsm_core_ptr = rfgsm_core_handle_ptr->rfc_gsm_core_ptr;

  if( rfc_gsm_core_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"gsm rfc_ptr is NULL! Exiting set linear rgi! ", 0);
    return;
  }

  if (gvars->ftm_FNmod4 == 0)
  {
    /* enable the slot for Tx*/
    gvars->ftm_slot_assign[tx_slot_config->slot_num].on_off = TRUE;
    gvars->ftm_slot_assign[tx_slot_config->slot_num].mcs = (tx_slot_config->mod_type_is_gsm == 1)? GL1_HW_PDTCHU_MCS1: GL1_HW_PDTCHU_MCS8;
  }

  /* Use default value from the NV if passed value is 0*/
  if(tx_slot_config->env_gain == 0)
  {
    tx_gain_param  = rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->tx_gain_param;
    env_gain = (tx_slot_config->mod_type_is_gsm == 1)? tx_gain_param.gsm_linear_tx_gain_val:tx_gain_param.edge_linear_tx_gain_val;
  }
  else
  {
    env_gain = (tx_slot_config->env_gain) << 4 ;
  }

  /* if pa_scale is 0, use UNIT_PA_SCALE passed value is 0*/
  if(tx_slot_config->pa_scaling == 0)
  {
    pa_scaling = RFGSM_MSM_PA_SCALE_UNITY_GAIN;
  }
  else
  {
    pa_scaling = tx_slot_config->pa_scaling ;
  }

  smps_bias = tx_slot_config->smps_bias;

  RFGSM_SLOT_LOCK(&rfgsm_slot_mutex);

  for ( i = 0; i < FTM_MAX_GSM_SLOT_PER_FRAME; i++ )
  {
    if ( (gvars->ftm_slot_assign[i].on_off) && (num_slot < FTM_MAX_GSM_TX_SLOT) )
    {
      if (tx_slot_config->slot_num == i)
      {
        /* get the modulation*/
        mod_type = (tx_slot_config->mod_type_is_gsm == 1)? RF_MOD_GMSK: RF_MOD_8PSK;

        ftm_gsm_rf_task_set_pa_range( rfm_dev,
                                      num_slot,
                                      tx_slot_config->pa_range,
                                      mod_type);

        ftm_gsm_rf_task_set_linear_rgi( rfm_dev,
                                        tx_slot_config->rgi,
                                        env_gain,
                                        mod_type,
                                        num_slot,
                                        TRUE,
                                        pa_scaling,
                                        smps_bias);
      }
      num_slot++;
    }
  }

  RFGSM_SLOT_UNLOCK(&rfgsm_slot_mutex);
}

#ifdef FEATURE_FTM_HWTC
/*===========================================================================

FUNCTION ftm_get_active_slot_list

DESCRIPTION
   This function returns the list of active slots in the current frame.

DEPENDENCIES
   None.

RETURN VALUE
   number of active slots and list of active slots.

SIDE EFFECTS
   Only used by HWTC, not for regular AMSS use.

===========================================================================*/
void ftm_get_active_slot_list( rfm_device_enum_type rfm_dev,
                               uint8* num_active_slots,
                               uint8* active_slot_list )
{
  uint8 i = 0;

  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);

  *num_active_slots = 0;
  for ( i=0; i<FTM_MAX_GSM_SLOT_PER_FRAME; i++ )
  {
    if ( (gvars->ftm_slot_assign[i].on_off) && (*num_active_slots < FTM_MAX_GSM_TX_SLOT) )
    {
      active_slot_list[*num_active_slots] = i;
      *num_active_slots += 1;
    }
  }
}//end of ftm_get_active_slot_list
#endif /* FEATURE_FTM_HWTC*/

/*===========================================================================

FUNCTION FTM_GSM_DO_TX_KV_CAL

DESCRIPTION
   This function will perform Kv calibration.

DEPENDENCIES
   Make sure the band and channel before call this API.

RETURN VALUE
   Kv calibration code.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_gsm_do_tx_kv_cal( rfm_device_enum_type rfm_dev,
                           ftm_gsm_generic_2x16bit_u *chan)
{
  ftm_tx_burst_type       save_tx_burst_state;  /* store current FTM burst state */
  uint32                  kv_code;
  rfnv_item_id_enum_type  nv_item_name_kv;
  ARFCN_T                 chan_infor;
  rfgsm_nv_band_data_type *rfgsm_nv_tbl_ptr;
  rfcom_gsm_band_type     band;
  kv_cal_type             kv_cal_data;
  kv_cal_type             *kv_cal_data_ptr;

  uint16 tmp = 0;
  boolean debug_flag = FALSE;

  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);

  /* Tx params : slot_num, data_source, tsc_index, num_bursts, infinite_duration */
  ftm_tx_burst_type cont_burst_param = {0, (int)FTM_TX_DATA_SOURCE_BUFFER, 0, 0, 1};

  debug_flag = rfgsm_nv_check_debug_nv( rfm_dev, RFGSM_DEBUG_NV_KV_CAL_LOGGING_ENABLE);

  /* Save the current FTM tx burst state */
  memscpy((void*)&save_tx_burst_state,
         sizeof(save_tx_burst_state),
         (void*)&gvars->tx_brst,
         sizeof(ftm_tx_burst_type));

  /* Added to support kv cal v2 - Set to default for original Kv Cal*/
  if(FALSE == gvars->ftm_gsm_rfctl_kv_cal_v2_state)
  {
    gvars->ftm_gsm_rfctl_kv_cal_per_tx_on = RFDEVICE_GSM_KV_CAL_PER_TX_ON;
    gvars->ftm_gsm_rfctl_kv_tx_on_off = RFDEVICE_GSM_KV_CAL_TX_ON_OFF;
  }

  /* Save the current FTM channel */
  chan_infor = ftm_gsm_get_arfcn( rfm_dev );

  switch ( gvars->ftm_gl1_hw_gsm_band)
  {
  case SYS_BAND_CELL_850:
    nv_item_name_kv   = RFNV_GSM_C0_GSM850_KV_CAL_I;
    band              = RFCOM_BAND_GSM850;
    break;

  case SYS_BAND_EGSM_900:
    nv_item_name_kv   = RFNV_GSM_C0_GSM900_KV_CAL_I;
    band              = RFCOM_BAND_GSM900;
    break;

  case SYS_BAND_DCS_1800:
    nv_item_name_kv   = RFNV_GSM_C0_GSM1800_KV_CAL_I;
    band              = RFCOM_BAND_GSM1800;
    break;

  case SYS_BAND_PCS_1900:
    nv_item_name_kv   = RFNV_GSM_C0_GSM1900_KV_CAL_I;
    band              = RFCOM_BAND_GSM1900;
    break;

  default:
    MSG_1( MSG_SSID_FTM, MSG_LEGACY_ERROR, "unsupport band %d", gvars->ftm_gl1_hw_gsm_band );
    nv_item_name_kv   = (rfnv_item_id_enum_type) 0;
    band              = RFCOM_NUM_GSM_BANDS;
    break;
  }

  if ( (nv_item_name_kv != (rfnv_item_id_enum_type)0) &&
       (band != RFCOM_NUM_GSM_BANDS) )
  {
    /* Configure FTM to GMSK continues TX */
    ftm_gsm_tx_cont_command( rfm_dev, &cont_burst_param );

    /* Set low band edge channel */
    ftm_gsm_set_arfcn( rfm_dev, chan->generic_a_16bit_u );

    if(TRUE == gvars->ftm_gsm_rfctl_kv_cal_v2_state)
    {
      tmp = gvars->ftm_gsm_rfctl_kv_tx_on_off;
      if (gvars->ftm_gsm_rfctl_kv_tx_on_off == 1)
      {
        /*For KV2cal only, run at least 2 runs of the first channel, otherwise kv code for the 2nd channel is observed to be bad when NVs reset to zero*/
        gvars->ftm_gsm_rfctl_kv_tx_on_off =  2;
      }
    }

    kv_code = (uint32)ftm_gsm_cal_kv( rfm_dev );

    if ( debug_flag == TRUE )
    {
      MSG_3( MSG_SSID_FTM,
             MSG_LEGACY_HIGH,
             "chan %d  KV  %d / 0x%X",
             chan->generic_a_16bit_u,
             kv_code,kv_code );
    }

    if(TRUE == gvars->ftm_gsm_rfctl_kv_cal_v2_state)
    {
      /* Restore number of averages for second channel*/
      gvars->ftm_gsm_rfctl_kv_tx_on_off =  tmp;
    }

    /* Set high band edge channel */
    ftm_gsm_set_arfcn( rfm_dev, chan->generic_b_16bit_u );
    kv_code = kv_code | (uint32)(ftm_gsm_cal_kv(rfm_dev) << 16);

    if (debug_flag == TRUE)
    {
        MSG_3( MSG_SSID_FTM,
               MSG_LEGACY_HIGH,
               "chan %d  KV  %d / 0x%X",
               chan->generic_b_16bit_u,
               kv_code,kv_code );
    }

    /* Create the NV item to write */
    kv_cal_data.kv_cal_value= kv_code;
    kv_cal_data.kv_cal_low_chan = chan->generic_a_16bit_u;
    kv_cal_data.kv_cal_high_chan = chan->generic_b_16bit_u;

    /* Write KV calibration code to NV */
    rfcommon_rfnv_set_item(nv_item_name_kv,
                           (rfnv_item_type *)(&kv_cal_data),
                           sizeof(kv_cal_type),
                           &ftm_tcb,
                           FTM_NV_CMD_SIG,
                           ftm_sig_rex_wait_cb);

    /*Update RF NV structure with the calibrated ARFCN/KV_code */
    rfgsm_nv_tbl_ptr = rfgsm_nv_get_tbl_ptr( rfm_dev, band, RFGSM_NV_TX );

  	if (rfgsm_nv_tbl_ptr == NULL)
  	{
  	  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"ftm_gsm_do_tx_kv_cal NULL NV ptr!", 0);
  	  return;
  	}

    kv_cal_data_ptr = &(rfgsm_nv_tbl_ptr->tx_static_nv_data_ptr->kv_cal);
    kv_cal_data_ptr->kv_cal_value= kv_code;
    kv_cal_data_ptr->kv_cal_low_chan = chan->generic_a_16bit_u;
    kv_cal_data_ptr->kv_cal_high_chan = chan->generic_b_16bit_u;

    /*   Restore FTM tx burst state */
    memscpy( (void*)&gvars->tx_brst,
            sizeof(gvars->tx_brst),
            (void*)&save_tx_burst_state,
            sizeof(ftm_tx_burst_type));

    /* Restore FTM channel */
    ftm_gsm_set_arfcn( rfm_dev, chan_infor.num );

    /* Return the KV calibration code in case PC side needes it. */
    chan->generic_a_16bit_u = (uint16)(kv_code & 0xFFFF);
    chan->generic_b_16bit_u = (uint16)((kv_code >> 16) & 0xFFFF);
  }
  else
  {
    /* Code to indicate KV calibration problem */
    chan->generic_a_16bit_u = 0x8000;
    chan->generic_a_16bit_u = 0x8000;
  }
}

/*===========================================================================

FUNCTION FTM_GSM_DO_TX_KV_CAL_V2

DESCRIPTION
   This function will perform Kv calibration v2.
   Band and channels are set as part of this API

DEPENDENCIES
   None

RETURN VALUE
   Kv calibration codes

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_gsm_do_tx_kv_cal_v2( rfm_device_enum_type rfm_dev,
                              ftm_gsm_kv_cal_v2_type *ftm_kv_cal_v2_params)
{
  int band_loop_var;

  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);

  // First Perform Sanity Check on Input Data
  if(ftm_kv_cal_v2_params->num_bands > 4)
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"Requested Number of Bands: %d exceeds Max Supported : 4", ftm_kv_cal_v2_params->num_bands);
    ftm_kv_cal_v2_params->num_bands = 4;
  }
  if(ftm_kv_cal_v2_params->num_bands == 0)
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"Requested Number of Bands: %d. Nothing to do.", ftm_kv_cal_v2_params->num_bands);
    return;
  }
  if(ftm_kv_cal_v2_params->num_tx_cycles == 0)
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"Invalid Requested Number of Tx Cycles: %d. Setting to 1.", ftm_kv_cal_v2_params->num_tx_cycles);
    ftm_kv_cal_v2_params->num_tx_cycles = 1;
  }
  if(ftm_kv_cal_v2_params->num_tx_cycles > 10)
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"Invalid Requested Number of Tx Cycles: %d exceeds Max supported : 10", ftm_kv_cal_v2_params->num_tx_cycles);
    ftm_kv_cal_v2_params->num_tx_cycles = 1;
  }
  if(ftm_kv_cal_v2_params->num_meas == 0)
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"Invalid Requested Number of Measurements per tx on cycle: %d. Setting to 1.", ftm_kv_cal_v2_params->num_meas);
    ftm_kv_cal_v2_params->num_meas = 1;
  }
  if(ftm_kv_cal_v2_params->num_meas > 12)
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"Invalid Requested Number of Measurements per tx on cycle: %d exceeds Max supported : 12", ftm_kv_cal_v2_params->num_meas);
    ftm_kv_cal_v2_params->num_meas = 1;
  }
  //Ignore checking band names/channels

  //Set number of tx on off and number of measurements
  gvars->ftm_gsm_rfctl_kv_cal_per_tx_on = ftm_kv_cal_v2_params->num_meas;
  gvars->ftm_gsm_rfctl_kv_tx_on_off = ftm_kv_cal_v2_params->num_tx_cycles;
  gvars->ftm_gsm_rfctl_kv_cal_v2_state = TRUE;

  for(band_loop_var = 0;band_loop_var < ftm_kv_cal_v2_params->num_bands; band_loop_var++)
  {
    ftm_gsm_generic_2x16bit_u chan_kv_list;
    ftm_gsm_set_mode( rfm_dev,
                      (ftm_mode_id_type) ftm_kv_cal_v2_params->rfmode_list[band_loop_var]);

    /* Sleep for Set Mode to Settle */
    ftm_clk_rex_wait(5);

    chan_kv_list.generic_a_16bit_u = ftm_kv_cal_v2_params->kv_chan_code[2*band_loop_var];
    chan_kv_list.generic_b_16bit_u = ftm_kv_cal_v2_params->kv_chan_code[2*band_loop_var + 1];

    ftm_gsm_do_tx_kv_cal( rfm_dev, &chan_kv_list );

    ftm_kv_cal_v2_params->kv_chan_code[2*band_loop_var]     = chan_kv_list.generic_a_16bit_u;
    ftm_kv_cal_v2_params->kv_chan_code[2*band_loop_var + 1] = chan_kv_list.generic_b_16bit_u;
  }
  gvars->ftm_gsm_rfctl_kv_cal_v2_state = FALSE;
}

/*===========================================================================

FUNCTION FTM_GSM_SETUP_RX_BURST_FOR_EXPECTED_PWR

DESCRIPTION
   Sets up burst based on the expected pwr/pwr output by callbox.

DEPENDENCIES
   GSM mode and channel needs to be set

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void ftm_gsm_core_setup_rx_burst_calver( rfm_device_enum_type rfm_dev,
                                         int16* parameters )
{
  ftm_rx_burst_type burst_type;
  int16 power_dbm16;
  uint8 average_bursts = 0;
  rfgsm_core_data_type rfgsm_core_data;
  rfgsm_enh_rx_params_type enh_rx_params;
  ftm_gsm_rfctl_vars_s *gvars;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr;

  enh_rx_params.enh_rx_enable_flag = 0;
  enh_rx_params.enh_rx_offset_freq = 0;

  gvars = ftm_gsm_get_vars_ptr(rfm_dev);
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  if (rfgsm_core_handle_ptr == NULL)
  {
  	MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
  	return;
  }

  /* Set the core data */
  rfgsm_core_data.rf_task = RF_TASK_FTM;
  rfgsm_core_data.rfm_dev = rfm_dev;
  rfgsm_core_data.sub_id = 0; //Default until support available if required
  rfgsm_core_data.as_id = SYS_MODEM_AS_ID_1; //Default until GL1 export get AS ID func
  rfgsm_core_data.triple_buffer_id = RFLM_GSM_BUFFER_ID_NOT_REQUIRED;
  rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr;
  rfgsm_core_data.enable_rx_diversity = FALSE;

  burst_type.ftm_rx_funct =FTM_RX_FUNCTION_GET_RSSI;
  burst_type.slot_num = 0;

  if(parameters == NULL)
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"parameters passed to ftm_gsm_core_setup_rx_burst_calver is NULL",0 );
    return;
  }

  power_dbm16 = parameters[0];
  average_bursts = (uint8)parameters[1];

  /*Update Rx Channel*/
  RFA_FTM_GSM_ISR_LOCK(&ftm_gsm_lock);
  if(rfgsm_core_handle_ptr->rf_rx_arfcn != gvars->ftm_gsm_state.ftm_channel)
  {
    (void)rfgsm_core_tune_rx( &rfgsm_core_data, gvars->ftm_gsm_state.ftm_channel);
  }
  RFA_FTM_GSM_ISR_UNLOCK(&ftm_gsm_lock);

  rfgsm_core_set_radio_gain( rfm_dev, power_dbm16, &gvars->ftm_gain_range, enh_rx_params ); //gain range returned here is indexed 1-5
  ftm_gsm_set_lna_range( rfm_dev, gvars->ftm_gain_range.range-1 ); //ftm lna & gl1 lna range interpretation is from 0-4
  ftm_gsm_rx_burst_command( rfm_dev, &burst_type,average_bursts);

}

/*===========================================================================

FUNCTION FTM_GSM_GET_MEAS_PWR

DESCRIPTION
   Convert ftm_rssi to dbm16 value

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
int16 ftm_gsm_get_rssi_dbx16( rfm_device_enum_type rfm_dev )
{
  dBx16_T dBm_x16_value;
  rf_gain_range_T gain;
  uint32 wait_counter = 0;
  uint32 rssi;

  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);

  if( (gvars->ftm_rx_funct == ftm_gsm_do_rx_cmd_rx_burst_metrics) &&
      gvars->ftm_gsm_rssi_no_frames_avg)
  {
    while(!ftm_gsm_get_ave_rssi_update_flag( rfm_dev ) &&
          (wait_counter < MAX_GET_RSSI_WAIT_TIME))
    {
      ftm_clk_rex_wait(1);
      wait_counter++;
    }
  }

  if(wait_counter >= MAX_GET_RSSI_WAIT_TIME)
  {
    dBm_x16_value = 0xFFFF;
  }
  else
  {
    if (rfgsm_core_is_device_prx(rfm_dev))
    {
      rssi = gvars->ftm_gsm_state.ftm_rssi;
    }
    else
    {
      rssi = gvars->ftm_gsm_state.ftm_rssi_drx;
    }

    (void)rfgsm_core_compute_rx_power( rssi,
                                       &dBm_x16_value,
                                       &gain,
                                       &gvars->ftm_gain_range,
                                       rfm_dev);

    MSG_4(MSG_SSID_FTM,
          MSG_LEGACY_ERROR,
          "|dev:%d| RSSI Value: raw %d, db16 %d, dbm %d",
          rfm_dev,rssi,dBm_x16_value,dBm_x16_value/16 );

    ftm_gsm_set_ave_rssi_update_flag( rfm_dev, FALSE) ;
  }
  return((int16)dBm_x16_value);

}

/*===========================================================================

FUNCTION FTM_GSM_GET_ENV_GAIN_NV

DESCRIPTION
   updates passed data structure with values of GSM ENV gain NV for current Tx band

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/

void ftm_gsm_get_env_gain_nv(rfm_device_enum_type rfm_dev, ftm_gsm_env_gain_nv_data_type *data_ptr)
{
  gsm_linear_tx_gain_param_type *tx_gain_nv_ptr = NULL;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return;
  }

  /* Get NV data ptr for current Tx band*/
  if (rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl == NULL)
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"ftm_gsm_get_env_gain_nv, failed", 0);

    return;
  }

  tx_gain_nv_ptr = &(rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->tx_gain_param);

  memscpy((void*)data_ptr, sizeof(*data_ptr), (void*)tx_gain_nv_ptr, sizeof(ftm_gsm_env_gain_nv_data_type));

  /* scale back the GSM/edge tx gain values by same num_bits as done
   in rfgsm_nv_load_tx_gain_param (4 bits for Dime), tools need raw NV value and not scaled one */
  data_ptr->gsm_linear_tx_gain_val = (data_ptr->gsm_linear_tx_gain_val)>>4;
  data_ptr->edge_linear_tx_gain_val= (data_ptr->edge_linear_tx_gain_val)>>4;

  return;
}

/*===========================================================================
FUNCTION ftm_gsm_set_freq_err

DESCRIPTION
   This API is used for applying Tx/Rx frequency error compensation value.
   Frequency error is in ppb units. The amount of frequency correction applied is either
   the user_freq_adjust_ppb or the combination of user_freq_adjust_ppb + the
   frequency error estimate from XO manager. This selection is based on the input
   value of enable_xo_freq_adjust.

   For example, if user passes a value of 2054  (ppb) it corresponds to 2054/1024 = 2.005 ppm. The resolution of frequency error is hence .001 ppm.

DEPENDENCIES
   None.

RETURN VALUE
   int32 - Total Frequency adjust in Hz

SIDE EFFECTS
   None.

===========================================================================*/
int32 ftm_gsm_set_freq_err( rfm_device_enum_type rfm_dev,
                            int32 user_freq_adjust_ppb,
                            uint32 center_frequency_hz,
                            uint8 enable_xo_freq_adjust )
{
  int32 tx_freq_error_composite_hz = 0;
  int32 freq_error_xo_ppb =0;
  int32 freq_error_xo = 0;
  int32 tx_freq_error_composite_hz_temp = 0;
  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr( rfm_dev );
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  rfgsm_core_data_type rfgsm_core_data;

  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!", 0);

  MSG_3(MSG_SSID_FTM, MSG_LEGACY_HIGH,
        "ftm_gsm_set_freq_error: tx_freq_error_ppb : %d : centre_freq_hz : %d : Enable = %d",
        user_freq_adjust_ppb,
        center_frequency_hz ,
        enable_xo_freq_adjust);

  /* Set the core data */
  rfgsm_core_data.rf_task = RF_TASK_FTM;
  rfgsm_core_data.rfm_dev = rfm_dev;
  rfgsm_core_data.sub_id = 0; //Default until support available if required
  rfgsm_core_data.as_id = SYS_MODEM_AS_ID_1; //Default until GL1 export get AS ID func
  rfgsm_core_data.triple_buffer_id = RFLM_GSM_BUFFER_ID_NOT_REQUIRED;
  rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr;

  if(enable_xo_freq_adjust)
  {
    freq_error_xo = (int32)tcxomgr_ft_curr_blocking();

    //Convert the frequency error estimated by XO Manager API to ppb units
    freq_error_xo_ppb = (int32)((int64)((int64)((int64)(freq_error_xo <<16) * 1000)/ 1024)>> 16);

    MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"ftm_gsm_set_freq_error: freq_error_xo_ppb : %d", freq_error_xo_ppb);

    // Add the user input frequency error with frequency error estimated by XO Manager
      tx_freq_error_composite_hz = (-1)*(user_freq_adjust_ppb  + freq_error_xo_ppb);

      MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"ftm_gsm_set_freq_error: tx_freq_error_composite_hz : %d",tx_freq_error_composite_hz);
    }
  else
  {
    tx_freq_error_composite_hz = (-1)*(user_freq_adjust_ppb);
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"ftm_wcdma_set_freq_adjust: tx_freq_error_composite_hz : %d",tx_freq_error_composite_hz);
  }

  /* apply freq error  only if freq override is enabled*/
  if (rfgsm_core_get_gl1_freq_error_flag(rfgsm_core_handle_ptr) == FALSE)
  {
    /* before calculating the new freq error, check if gl1_freq_err field in ftm handle is updated with latest value or not
       and save it if it is not not saved yet */
    if ( gvars->gl1_freq_error_saved == FALSE)
  {
    if( enable_xo_freq_adjust )
    {
      gvars->gl1_freq_error = rfgsm_core_get_freq_error( rfgsm_core_handle_ptr ) - tx_freq_error_composite_hz;
    }
    else
    {
      gvars->gl1_freq_error = rfgsm_core_get_freq_error( rfgsm_core_handle_ptr );
    }
      // set the gl1_freq_error_saved flag to True
      gvars->gl1_freq_error_saved = TRUE;
  }

    if(tx_freq_error_composite_hz != 0)
    {
      gvars->new_freq_error = gvars->gl1_freq_error + tx_freq_error_composite_hz;  //pass freq_error from API on top of frequency error passed by GL1. add if freq_error observed -ve on the call box
    }
    else
    {
      gvars->new_freq_error = gvars->gl1_freq_error + gvars->prev_freq_error;  // if freq error =0 , i.e. unchanged, pass previous frequency error
    }

    gvars->prev_freq_error = tx_freq_error_composite_hz; // update previous frequency error

  }

  rfgsm_core_update_freq_error( &rfgsm_core_data, gvars->new_freq_error );

  tx_freq_error_composite_hz_temp = (int32)((int64)((
                                         (int64)(((int64)((gvars->new_freq_error) << 16))* center_frequency_hz )
                                         )/1000000000) >> 16) ;
  return tx_freq_error_composite_hz_temp;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialises the IQ capture buffer for a given device

  @details
  Allocates enough space for the iq capture buffer and supporting variables.
  Sets the iq_capture pointer in the core handles structure to the allocated
  memory.

  @retval
  None

*/
void ftm_gsm_init_iqcapture_buffer( rfm_device_enum_type rfm_dev )
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = rfgsm_core_handle_get( rfm_dev );

  if( rfgsm_core_handle_ptr == NULL )
  {
    MSG_1( MSG_SSID_FTM, MSG_LEGACY_ERROR, "Core handle for device %d is NULL", rfm_dev);
    return;
  }

  if( rfgsm_core_handle_ptr->iq_capture != NULL )
  {
    MSG_1( MSG_SSID_FTM, MSG_LEGACY_ERROR,
           "GSM IQ capture buffer for dev %d already inited", rfm_dev);
    return;
  }

  /* IQ capture buffer has not been allocated, do so now */
  rfgsm_core_handle_ptr->iq_capture =
     (rfgsm_core_iq_capture_type*)modem_mem_alloc(sizeof(rfgsm_core_iq_capture_type), MODEM_MEM_CLIENT_RFA);

  if ( rfgsm_core_handle_ptr->iq_capture == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Could not allocate GSM IQ buffer for dev %d", rfm_dev );
    return;
  }

  /* Reset the allocated buffer */
  rfgsm_core_handle_ptr->iq_capture->capture_type = FTM_IQ_CAPTURE_STOP;
  rfgsm_core_handle_ptr->iq_capture->total_iq_samples_copied = 0;
  memset(rfgsm_core_handle_ptr->iq_capture->rfgsm_iq_buffer, 0, RFGSM_MAX_IQ_BUFFER_LEN*sizeof(int16));

  MSG_1( MSG_SSID_FTM, MSG_LEGACY_HIGH, "GSM IQ capture buffer for device %d initialized", rfm_dev);
}



/*===========================================================================
FUNCTION ftm_gsm_get_iq_capture

DESCRIPTION
   This function used to set the IQ capture type and call GL1 interface function to start IQ capture

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_gsm_start_stop_iq_capture( rfm_device_enum_type rfm_dev,
                                    ftm_iqcapture_type iq_cap_type )
{
  rfa_rf_gsm_ftm_capture_iq_type_cmd_s    rfa_rf_gsm_ftm_capture_iq_cmd;
  rfa_rf_gsm_ftm_capture_iq_type_rsp_s    rfa_rf_gsm_ftm_capture_iq_rsp;
  errno_enum_type result;
  uint32 bytes_recvd;

  sys_modem_as_id_e_type as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );

  /* Get handle to the FTM gsm mutex */
  RFA_FTM_GSM_ISR_LOCK(&ftm_gsm_lock);

  /* Initialise msgr header */
  msgr_init_hdr( &rfa_rf_gsm_ftm_capture_iq_cmd.hdr, MSGR_RFA_RF_GSM_FTM, RFA_RF_GSM_FTM_CAPTURE_IQ_CMD );

  /* Populate payload parameters */
  rfa_rf_gsm_ftm_capture_iq_cmd.rfm_dev = rfm_dev;
  rfa_rf_gsm_ftm_capture_iq_cmd.iq_capture_type = iq_cap_type;

  /* Send msgr message to RF Task */
  result = msgr_send(&rfa_rf_gsm_ftm_capture_iq_cmd.hdr, sizeof(rfa_rf_gsm_ftm_capture_iq_type_cmd_s));

  /* Sanity check and print debug for msgr_send */
  ftm_gsm_rf_msgr_send_debug( rfm_dev, RFA_RF_GSM_FTM_CAPTURE_IQ_CMD, result);

  /* Blocking wait for set PA range RSP message from RF Task */
  result = msgr_receive( &ftm_rfgsm_msgr_client,
                         (uint8 *)&rfa_rf_gsm_ftm_capture_iq_rsp,
                         (uint32)sizeof(rfa_rf_gsm_ftm_capture_iq_type_rsp_s),
                         &bytes_recvd );

  /* Release FTM gsm mutex */
  RFA_FTM_GSM_ISR_UNLOCK(&ftm_gsm_lock);

  /* Sanity check and print debug for msgr_receive */
  ftm_gsm_rf_msgr_receive_debug( RFA_RF_GSM_FTM_CAPTURE_IQ_RSP,
                                 result,
                                 rfa_rf_gsm_ftm_capture_iq_rsp.hdr.id,
                                 rfa_rf_gsm_ftm_capture_iq_rsp.cmd_result );

  if (rfa_rf_gsm_ftm_capture_iq_rsp.iq_capture == NULL)
  {
    MSG(MSG_SSID_FTM, MSG_LEGACY_ERROR, "IQ Capture Pointer returned to FTM is NULL!");
    return;
  }

  if(iq_cap_type != FTM_IQ_CAPTURE_STOP)
  {
    /* Tell GL1 to start logging samples */
    mdsp_rf_set_iq_log(TRUE, as_id);
  }
  else
  {
    mdsp_rf_set_iq_log(FALSE, as_id);
  }

  MSG_2(MSG_SSID_FTM, MSG_LEGACY_MED, "IQ capture state %d for device %d", iq_cap_type, rfm_dev );
}

/*============================================================================

FUNCTION FTM_GSM_IQ_CAPTURE_GET_CONFIG

DESCRIPTION
  This API is used to get the IQ capture configuration for GSM to show in QRCT.

DEPENDENCIES
  None

RETURN VALUE
  Always TRUE

SIDE EFFECTS
 None
=============================================================================*/

ftm_iq_capture_error_code_type ftm_gsm_iq_capture_get_config(ftm_iq_capture_get_config_type *data)
{
  ftm_iq_capture_error_code_type ret_value=FTM_IQ_SUCCESS;

  data->max_sample_size = FTM_GSM_IQ_MAX_SAMPLES;
  data->sample_data_format = FTM_GSM_IQ_SS_SAMPLE_WIDTH;
  data->sampling_freq = FTM_GSM_IQ_SAMPLE_FREQ;
  data->max_diag_sample_size = FTM_GSM_IQ_DIAG_MAX_SAMPLES;

  return ret_value;
}

/*============================================================================

FUNCTION FTM_GSM_IQ_CAPTURE_ACQUIRE_SAMPLES

DESCRIPTION
  This API is used to Acquire samples for GSM and store the information related to them in a structure. The information include the
  device, carrier and number of samples acquired.

DEPENDENCIES
  RF chain should be configured before acquring. Device and carrier id should be correctly passed.

RETURN VALUE
  FTM_IQ_SUCCESS, - Everything is fine.
  FTM_IQ_GENERAL_FAILURE  - RF chain has not been configured

SIDE EFFECTS
 None
=============================================================================*/
ftm_iq_capture_error_code_type ftm_gsm_iq_capture_acquire_samples(rfm_device_enum_type rfm_dev, ftm_iq_capture_acquire_samples_type *data)
{

  // temporary pointer pointing to IQ capture buffer.
  int16* ftm_gsm_iq_capture_ptr = rfgsm_core_get_iq_buffer_ptr(rfm_dev);

  ftm_gsm_iq_capture_info_type* ftm_gsm_iq_capture_info_ptr;
  ftm_gsm_iq_capture_info_ptr = ftm_gsm_iq_capture_info_create_and_get(rfm_dev);

  //Nullcheck ftm_gsm_iq_capture_info_ptr
  if (ftm_gsm_iq_capture_info_ptr == NULL) {
    MSG( MSG_SSID_FTM, MSG_LEGACY_ERROR, "Unable to allocate memory for ftm_gsm_iq_capture_info_ptr" );
    return FTM_IQ_GENERAL_FAILURE;
  }
  // Allocate memory of Maximum sample size 2 * 2* 8192 bytes.
  if(ftm_gsm_iq_capture_info_ptr->gsm_iq_buf_ptr == NULL)
  {
    ftm_gsm_iq_capture_info_ptr->gsm_iq_buf_ptr=(int16*)ftm_malloc(2*FTM_IQ_SAMPLE_NUM_OF_BYTE*FTM_GSM_IQ_MAX_SAMPLES);
    // If memory is not allocated still, then declare it as failure.
    if(ftm_gsm_iq_capture_info_ptr->gsm_iq_buf_ptr == NULL)
    {
     MSG( MSG_SSID_FTM, MSG_LEGACY_ERROR, "Unable to allocate memory for IQ buffer" );
     return FTM_IQ_GENERAL_FAILURE;
    }
    memset(ftm_gsm_iq_capture_info_ptr->gsm_iq_buf_ptr,0,2*FTM_IQ_SAMPLE_NUM_OF_BYTE*FTM_GSM_IQ_MAX_SAMPLES);
  }

   // Setting the sample size
  ftm_gsm_iq_capture_info_ptr->number_of_samples_req=data->sample_size;
  // Unset the data available at the begining of  the function. Set it only if we are able to acquire samples successfully.
  ftm_gsm_iq_capture_info_ptr->data_available = FALSE;

  // First check if IQ capture is NULL
  if (ftm_gsm_iq_capture_ptr == NULL)
  {
    MSG( MSG_SSID_FTM, MSG_LEGACY_ERROR, "rfgsm_iq_buffer is NULL" );
    return FTM_IQ_GENERAL_FAILURE;
  }

  //check if samples are avialble on this rfm_dev
  if ( ((rfgsm_core_handle_get(rfm_dev))->iq_capture->capture_type == FTM_IQ_CAPTURE_SINGLE_SHOT)
    && ((rfgsm_core_handle_get(rfm_dev))->iq_capture->total_iq_samples_copied <= 0) )
  {
    MSG_1( MSG_SSID_FTM, MSG_LEGACY_ERROR, "This rfm_dev has %d iq samples collected",(rfgsm_core_handle_get(rfm_dev))->iq_capture->total_iq_samples_copied );
    return FTM_IQ_IQ_SAMPLES_UNAVAILABLE_ON_RFM_DEV;
  }

  // Check if Requested samples size > Max samples that can be acquired
  if (data->sample_size > FTM_GSM_IQ_MAX_SAMPLES)
  {
    MSG_2( MSG_SSID_FTM, MSG_LEGACY_ERROR, "Requested sample %d size exceeds the Max GSM IQ capture length %d", data->sample_size, FTM_GSM_IQ_MAX_SAMPLES );
    return FTM_IQ_GENERAL_FAILURE;
  }

  // Since we met all the check conditions, copy the data into static buffer which holds IQ data.
  // x samples indicates x I and Q pairs and each I/Q is of 2 bytes. So total size is 2*2*x.
  memscpy(ftm_gsm_iq_capture_info_ptr->gsm_iq_buf_ptr,
	  sizeof(int16) * FTM_IQ_SAMPLE_NUM_OF_BYTE * FTM_GSM_IQ_MAX_SAMPLES,
	  ftm_gsm_iq_capture_ptr,
	  2 * FTM_IQ_SAMPLE_NUM_OF_BYTE * data->sample_size);
  // Now set the data available flag.
  ftm_gsm_iq_capture_info_ptr->data_available = TRUE;

  return FTM_IQ_SUCCESS;

}

/*============================================================================

FUNCTION FTM_GSM_IQ_CAPTURE_GET_SAMPLES

DESCRIPTION
  This API is used to get samples for GSM from buffer that has been used to capture samples from ACQUIRE SAMPLES.

DEPENDENCIES
 The CAPTURE command should have been issued.

RETURN VALUE
None

SIDE EFFECTS
 None
=============================================================================*/
void ftm_gsm_iq_capture_get_samples(rfm_device_enum_type rfm_dev, ftm_iq_capture_get_samples_type *data,ftm_rsp_pkt_type *rsp_pkt)
{

  ftm_gsm_iq_capture_info_type* ftm_gsm_iq_capture_info_ptr;

  /***************** Fill up RSP packet with standard data *****************/

  // Fill in the constant fields of response packet. Sample buffer will be filled after sanity check.
  ftm_gsm_iq_capture_get_samples_rsp_pkt.cmd_code = 75;
  ftm_gsm_iq_capture_get_samples_rsp_pkt.subsys_id = FTM;
  if ( (rfm_dev == RFM_DEVICE_2) || (rfm_dev == RFM_DEVICE_3) )
  {
      ftm_gsm_iq_capture_get_samples_rsp_pkt.subsys_cmd_code = FTM_GSM_C2_C;
  }else
  {
      ftm_gsm_iq_capture_get_samples_rsp_pkt.subsys_cmd_code = FTM_GSM_C0_C;
  }

  ftm_gsm_iq_capture_get_samples_rsp_pkt.ftm_rf_cmd = FTM_IQ_CAPTURE_GET_SAMPLES;
  // Start with sucess flag and change it if we fail any check conditions.
  ftm_gsm_iq_capture_get_samples_rsp_pkt.ftm_error_code=FTM_IQ_SUCCESS;
  ftm_gsm_iq_capture_get_samples_rsp_pkt.sample_offset=data->sample_offset;
  ftm_gsm_iq_capture_get_samples_rsp_pkt.sample_size=data->sample_size;


  // Fill in the constant fields of response packet.Size of the packet will be based on the requested sample size.
  rsp_pkt->cmd = FTM_RSP_DO_NEW_PKT;
  rsp_pkt->pkt_payload=&ftm_gsm_iq_capture_get_samples_rsp_pkt;
  rsp_pkt->delete_payload = FALSE;

  ftm_gsm_iq_capture_info_ptr = (ftm_gsm_iq_capture_info_type*)ftm_gsm_iq_capture_info_create_and_get(rfm_dev);

  //check ftm_gsm_iq_capture_info_ptr is NULL
  if (ftm_gsm_iq_capture_info_ptr == NULL) {
    MSG( MSG_SSID_FTM, MSG_LEGACY_ERROR, "ftm_gsm_iq_capture_info_ptr is NULL" );
    ftm_gsm_iq_capture_get_samples_rsp_pkt.ftm_error_code= FTM_IQ_GENERAL_FAILURE;
  }
  else {
     if (ftm_gsm_iq_capture_info_ptr->data_available == FALSE || ftm_gsm_iq_capture_info_ptr->gsm_iq_buf_ptr == NULL)
     {
       MSG( MSG_SSID_FTM, MSG_LEGACY_ERROR, "IQ samples are not acquired, please acquire them first" );
       ftm_gsm_iq_capture_get_samples_rsp_pkt.ftm_error_code=FTM_IQ_IQ_SAMPLES_UNAVAILABLE;
     }

     // Check if sample offset and sample size requested are proper.
     if( (data->sample_offset + data->sample_size) > ftm_gsm_iq_capture_info_ptr->number_of_samples_req)
     {
      MSG_2( MSG_SSID_FTM, MSG_LEGACY_ERROR, "Sample offset or Sample size is wrong %d %d", data->sample_offset, data->sample_size );
      ftm_gsm_iq_capture_get_samples_rsp_pkt.ftm_error_code=FTM_IQ_ILLEGAL_COMBIMATION;
     }
  }

  if(  data->sample_size > FTM_GSM_IQ_DIAG_MAX_SAMPLES)
  {
    MSG_2( MSG_SSID_FTM, MSG_LEGACY_ERROR, "Requested Sample size %d is greater than %d", data->sample_size, FTM_GSM_IQ_DIAG_MAX_SAMPLES );
    ftm_gsm_iq_capture_get_samples_rsp_pkt.ftm_error_code=FTM_IQ_ILLEGAL_COMBIMATION;
  }

   /*******Fill up remaining part of RSP packet based on sanity checks and acquired data***********/

  // If all the sanity checks are passed, we will be sending IQ samples in Response packet.
  if(ftm_gsm_iq_capture_get_samples_rsp_pkt.ftm_error_code == FTM_IQ_SUCCESS)
  {
    // Since we met all the check conditions, copy the data.
   // Since extracted data is of format:  First sample: I(16 bits) and then second sample Q (16 bits)
   //    a) Sample offset x corresponds to 2*x element in the array
   //    b) Sample size y corresponds to 2*y elements (I + Q) => 2*2*y bytes
   memscpy(ftm_gsm_iq_capture_get_samples_rsp_pkt.sample_buffer.sample_buffer_16_bit,
	   sizeof(int16) * (FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG / 2),
	   &(ftm_gsm_iq_capture_info_ptr->gsm_iq_buf_ptr[(2*(data->sample_offset))]),
	   2*FTM_IQ_SAMPLE_NUM_OF_BYTE*data->sample_size);

   rsp_pkt->pkt_len = FTM_IQ_CAPTURE_GET_SAMPLE_HDR_SIZE + 2* sizeof(word) * data->sample_size; // Header size +   (I + Q , each 1 word) * total number

   MSG_2( MSG_SSID_FTM, MSG_LEGACY_ERROR, "%d GSM IQ samples sent to Diag from sample offset %d", data->sample_size, data->sample_offset );

  }
  // If we fail checks, dont fill IQ samples. RSP packet will only have header.
  else
  {

   rsp_pkt->pkt_len =  FTM_IQ_CAPTURE_GET_SAMPLE_HDR_SIZE;
   MSG_2( MSG_SSID_FTM, MSG_LEGACY_ERROR, "GSM IQ samples Capture failed for SAMPLE SIZE %d and SAMPLE OFFSET %d", data->sample_size, data->sample_offset );
  }

}


/*============================================================================

FUNCTION FTM_GSM_FREE_MEMORY

DESCRIPTION
  This API is used to free up the memory allocated in GSM FTM mode. This API will be generally called while exiting the mode

DEPENDENCIES
 None

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/
void ftm_gsm_iq_capture_info_free_memory(void)
{
  uint8 rfm_dev = 0;
  for (rfm_dev = 0; rfm_dev < RFM_MAX_WAN_DEVICES; rfm_dev++)
  {
   /* only destory if its NOT already been destroyed */
   if(ftm_gsm_iq_capture_info[rfm_dev] != NULL)
   {
     if( ftm_gsm_iq_capture_info[rfm_dev]->gsm_iq_buf_ptr != NULL )
     {
     /* Free the IQ capture buffer */
       ftm_free(ftm_gsm_iq_capture_info[rfm_dev]->gsm_iq_buf_ptr);
       ftm_gsm_iq_capture_info[rfm_dev]->gsm_iq_buf_ptr = NULL;
     }
     ftm_free(ftm_gsm_iq_capture_info[rfm_dev]);
     ftm_gsm_iq_capture_info[rfm_dev]=NULL;
   }
  }
}

/*============================================================================

FUNCTION FTM_GSM_IQ_CAPTURE_INFO_CREATE_AND_GET

DESCRIPTION
  This API is used to create and get ftm_gsm_iq_capture_info per rfm_dev in GSM FTM mode.

DEPENDENCIES
 None

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/
ftm_gsm_iq_capture_info_type* ftm_gsm_iq_capture_info_create_and_get(rfm_device_enum_type rfm_dev)
{
  if (rfm_dev < RFM_MAX_WAN_DEVICES)
  {
    if(ftm_gsm_iq_capture_info[rfm_dev] == NULL) /* only create the handle if it NOT already created */
    {
      ftm_gsm_iq_capture_info[rfm_dev] = (ftm_gsm_iq_capture_info_type*)ftm_malloc(sizeof(ftm_gsm_iq_capture_info_type));

      if (ftm_gsm_iq_capture_info[rfm_dev] == NULL) // failed to allocate memory
      {
        MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR, "Failed to create ftm_gsm_iq_capture_info for rfm_dev %d", rfm_dev );
      }
      else
      {
        /* Initialise Memory */
        ftm_gsm_iq_capture_info[rfm_dev]->data_available = FALSE;
        ftm_gsm_iq_capture_info[rfm_dev]->gsm_iq_buf_ptr = NULL;
        ftm_gsm_iq_capture_info[rfm_dev]->number_of_samples_req =0;
      }
    }
    else
    {
      MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR, "ftm_gsm_iq_capture_info has been already created! rfm_dev=%d", rfm_dev );
    }
    return ftm_gsm_iq_capture_info[rfm_dev];

  }
  else
  {
     MSG_2( MSG_SSID_RF, MSG_LEGACY_ERROR, "Intended rfm_dev %d exceeds the limit %d", rfm_dev, RFM_MAX_WAN_DEVICES );
     return (ftm_gsm_iq_capture_info_type*)NULL;
  }
}


/*===========================================================================
FUNCTION ftm_gsm_rf_task_set_pa_range

DESCRIPTION
  Send MSGR message to RF Task to set PA range. Use MSGR blocking wait
  to receive set PA range completion response from RF Task before continuing

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/

void ftm_gsm_rf_task_set_pa_range( rfm_device_enum_type rfm_dev,
                                   uint8 num_slot,
                                   uint16 pa_range,
                                   rfgsm_modulation_type mod_type )
{
  rfa_rf_gsm_ftm_set_pa_range_type_cmd_s  rfa_rf_gsm_ftm_set_pa_range_cmd;
  errno_enum_type result;
  uint32 bytes_recvd;

  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr( rfm_dev );

  /* Get handle to the FTM gsm mutex */
  RFA_FTM_GSM_ISR_LOCK(&ftm_gsm_lock);

  /* Initialise msgr header */
  msgr_init_hdr( &rfa_rf_gsm_ftm_set_pa_range_cmd.hdr, MSGR_RFA_RF_GSM_FTM, RFA_RF_GSM_FTM_SET_LINEAR_PA_RANGE_CMD );

  /* Populate payload parameters */
  rfa_rf_gsm_ftm_set_pa_range_cmd.rfm_dev = rfm_dev;
  rfa_rf_gsm_ftm_set_pa_range_cmd.pa_range_params.num_slot = num_slot;
  rfa_rf_gsm_ftm_set_pa_range_cmd.pa_range_params.pa_range = pa_range;
  rfa_rf_gsm_ftm_set_pa_range_cmd.pa_range_params.mod_type = mod_type;

  /* Send msgr message to RF Task */
  result = msgr_send(&rfa_rf_gsm_ftm_set_pa_range_cmd.hdr, sizeof(rfa_rf_gsm_ftm_set_pa_range_type_cmd_s));

  /* Sanity check and print debug for msgr_send */
  ftm_gsm_rf_msgr_send_debug( rfm_dev, RFA_RF_GSM_FTM_SET_LINEAR_PA_RANGE_CMD, result );

  /* Blocking wait for set PA range RSP message from RF Task */
  result = msgr_receive(&ftm_rfgsm_msgr_client,
                        (uint8 *)&gvars->rfa_rf_gsm_ftm_set_pa_range_rsp,
                        (uint32)sizeof(rfa_rf_gsm_ftm_set_pa_range_type_rsp_s),
                        &bytes_recvd);

  /* Release FTM gsm mutex */
  RFA_FTM_GSM_ISR_UNLOCK(&ftm_gsm_lock);

  /* Sanity check and print debug for msgr_receive */
  ftm_gsm_rf_msgr_receive_debug( RFA_RF_GSM_FTM_SET_LINEAR_PA_RANGE_RSP,
                                 result,
                                 gvars->rfa_rf_gsm_ftm_set_pa_range_rsp.hdr.id,
                                 gvars->rfa_rf_gsm_ftm_set_pa_range_rsp.cmd_result );
}



void ftm_gsm_override_gl1_freq_err( rfm_device_enum_type rfm_dev )
{
  rfgsm_core_set_gl1_freq_error_flag (rfm_dev, FALSE);
}
/*===========================================================================
FUNCTION ftm_gsm_set_tx_pow_dbm

DESCRIPTION
   Either uses MSGR or legacy api call to set tx power

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
int8 ftm_gsm_set_tx_pow_dbm( rfm_device_enum_type rfm_dev,
                             int16 pow_dbm,
                             int8 index )
{
  int8 rtn_index = 0;

  rtn_index = ftm_gsm_rf_task_set_tx_pow_dbm( rfm_dev, pow_dbm, index);

  return (rtn_index);
}


/*===========================================================================
FUNCTION ftm_gsm_rf_task_set_tx_pow_dbm

DESCRIPTION
  Send MSGR message to RF Task to set the tx power in dBm. Use MSGR blocking wait
  to receive set PA range completion response from RF Task before continuing

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
int8 ftm_gsm_rf_task_set_tx_pow_dbm ( rfm_device_enum_type rfm_dev,
                                      int16 pow_dbm,
                                      int8 index )
{
  rfa_rf_gsm_ftm_set_tx_pow_dbm_type_cmd_s  rfa_rf_gsm_ftm_set_pow_dbm_cmd;
  errno_enum_type result;
  uint32 bytes_recvd;

  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr( rfm_dev );

  /* Get handle to the FTM gsm mutex */
  RFA_FTM_GSM_ISR_LOCK(&ftm_gsm_lock);

  /* Initialise msgr header */
  msgr_init_hdr( &rfa_rf_gsm_ftm_set_pow_dbm_cmd.hdr, MSGR_RFA_RF_GSM_FTM, RFA_RF_GSM_FTM_SET_TX_POW_DBM_CMD );

  rfa_rf_gsm_ftm_set_pow_dbm_cmd.rfm_dev                        = rfm_dev;
  rfa_rf_gsm_ftm_set_pow_dbm_cmd.set_tx_pow_dbm_params.pow_dbm  = pow_dbm;
  rfa_rf_gsm_ftm_set_pow_dbm_cmd.set_tx_pow_dbm_params.index    = index;

  /* Send msgr message to RF Task */
  result = msgr_send(&rfa_rf_gsm_ftm_set_pow_dbm_cmd.hdr, sizeof(rfa_rf_gsm_ftm_set_tx_pow_dbm_type_cmd_s));

  /* Sanity check and print debug for msgr_send */
  ftm_gsm_rf_msgr_send_debug( rfm_dev, RFA_RF_GSM_FTM_SET_TX_POW_DBM_CMD, result );

  /* Blocking wait for set PA range RSP message from RF Task */
  result = msgr_receive(&ftm_rfgsm_msgr_client,
                        (uint8 *)&gvars->rfa_rf_gsm_ftm_set_tx_pow_dbm_rsp,
                        (uint32)sizeof(rfa_rf_gsm_ftm_set_tx_pow_dbm_type_rsp_s),
                        &bytes_recvd);

  /* Release FTM gsm mutex */
  RFA_FTM_GSM_ISR_UNLOCK(&ftm_gsm_lock);

  /* Sanity check and print debug for msgr_receive */
  ftm_gsm_rf_msgr_receive_debug( RFA_RF_GSM_FTM_SET_TX_POW_DBM_RSP,
                                result,
                                gvars->rfa_rf_gsm_ftm_set_tx_pow_dbm_rsp.hdr.id,
                                gvars->rfa_rf_gsm_ftm_set_tx_pow_dbm_rsp.cmd_result );

  return( gvars->rfa_rf_gsm_ftm_set_tx_pow_dbm_rsp.set_tx_pow_dbm_params.index );

}
/*===========================================================================
FUNCTION ftm_gsm_set_path_delay

DESCRIPTION
   Use either the legacy rf call or MSGR implementation to signal RF Task
DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_gsm_set_path_delay ( rfm_device_enum_type rfm_dev,
                              uint16 input_delay_value )
{
  ftm_gsm_rf_task_set_path_delay( rfm_dev, input_delay_value );
}

/*===========================================================================
FUNCTION ftm_gsm_rf_task_set_path_delay

DESCRIPTION
  1st Draft for testing

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_gsm_rf_task_set_path_delay( rfm_device_enum_type rfm_dev,
                                     uint16 input_delay_value )
{
  rfa_rf_gsm_ftm_set_path_delay_type_cmd_s  rfa_rf_gsm_ftm_set_path_delay_cmd;
  rfa_rf_gsm_ftm_set_path_delay_type_rsp_s  rfa_rf_gsm_ftm_set_path_delay_rsp;
  errno_enum_type result;
  uint32 bytes_recvd;

  /* Get handle to the FTM gsm mutex */
  RFA_FTM_GSM_ISR_LOCK(&ftm_gsm_lock);

  /* Initialise msgr header */
  msgr_init_hdr( &rfa_rf_gsm_ftm_set_path_delay_cmd.hdr, MSGR_RFA_RF_GSM_FTM, RFA_RF_GSM_FTM_SET_PATH_DELAY_CMD );

  /* Populate payload parameters */
  rfa_rf_gsm_ftm_set_path_delay_cmd.rfm_dev = rfm_dev;
  rfa_rf_gsm_ftm_set_path_delay_cmd.delay_value = input_delay_value;

  /* Send msgr message to RF Task */
  result = msgr_send(&rfa_rf_gsm_ftm_set_path_delay_cmd.hdr, sizeof(rfa_rf_gsm_ftm_set_path_delay_type_cmd_s));

  /* Sanity check and print debug for msgr_send */
  ftm_gsm_rf_msgr_send_debug( rfm_dev, RFA_RF_GSM_FTM_SET_PATH_DELAY_CMD, result);

  /* Blocking wait for set PA range RSP message from RF Task */
  result = msgr_receive(&ftm_rfgsm_msgr_client,
                    (uint8 *)&rfa_rf_gsm_ftm_set_path_delay_rsp,
                    (uint32)sizeof(rfa_rf_gsm_ftm_set_path_delay_type_rsp_s),
                    &bytes_recvd);

  /* Release FTM gsm mutex */
  RFA_FTM_GSM_ISR_UNLOCK(&ftm_gsm_lock);

/* Sanity check and print debug for msgr_receive */
  ftm_gsm_rf_msgr_receive_debug(RFA_RF_GSM_FTM_SET_PATH_DELAY_RSP,
                                 result,
                                 rfa_rf_gsm_ftm_set_path_delay_rsp.hdr.id,
                                 rfa_rf_gsm_ftm_set_path_delay_rsp.cmd_result );

}


/*===========================================================================
FUNCTION ftm_gsm_rf_task_set_linear_rgi

DESCRIPTION
  Signal RF Task via MSGR to set linear rgi and update flags.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_gsm_rf_task_set_linear_rgi( rfm_device_enum_type rfm_dev,
                                     uint16 rgi,
                                     uint16 env_gain,
                                     rfgsm_modulation_type mod_type,
                                     uint8 slot_num,
                                     boolean set_script,
                                     uint16 pa_scaling,
                                     uint16 smps_bias )
{
  rfa_rf_gsm_ftm_set_linear_rgi_type_cmd_s  rfa_rf_gsm_ftm_set_linear_rgi_cmd;
  rfa_rf_gsm_ftm_set_linear_rgi_type_rsp_s  rfa_rf_gsm_ftm_set_linear_rgi_rsp;
  errno_enum_type result;
  uint32 bytes_recvd;

  /* Get handle to the FTM gsm mutex */
  RFA_FTM_GSM_ISR_LOCK(&ftm_gsm_lock);

  /* Initialise msgr header */
  msgr_init_hdr( &rfa_rf_gsm_ftm_set_linear_rgi_cmd.hdr, MSGR_RFA_RF_GSM_FTM, RFA_RF_GSM_FTM_SET_LINEAR_RGI_CMD );

  /* Populate payload parameters */
  rfa_rf_gsm_ftm_set_linear_rgi_cmd.rfm_dev                             = rfm_dev;
  rfa_rf_gsm_ftm_set_linear_rgi_cmd.linear_rgi_params.rgi               = rgi;
  rfa_rf_gsm_ftm_set_linear_rgi_cmd.linear_rgi_params.env_gain          = env_gain;
  rfa_rf_gsm_ftm_set_linear_rgi_cmd.linear_rgi_params.mod_type_is_gsm   = !mod_type; //this is reverse logic...mod_type: GMSK = 0; 8PSK = 1 ---> mod_type_is_gsm: 1 = GMSK; 0 = 8PSK
  rfa_rf_gsm_ftm_set_linear_rgi_cmd.linear_rgi_params.slot_num          = slot_num;
  rfa_rf_gsm_ftm_set_linear_rgi_cmd.linear_rgi_params.set_script        = set_script;
  rfa_rf_gsm_ftm_set_linear_rgi_cmd.linear_rgi_params.pa_scaling        = (pa_scaling < 733 )? RFGSM_MSM_PA_SCALE_UNITY_GAIN:pa_scaling;
  rfa_rf_gsm_ftm_set_linear_rgi_cmd.linear_rgi_params.smps_bias         = smps_bias;



  /* Send msgr message to RF Task */
  result = msgr_send(&rfa_rf_gsm_ftm_set_linear_rgi_cmd.hdr, sizeof(rfa_rf_gsm_ftm_set_linear_rgi_type_cmd_s));

  /* Sanity check and print debug for msgr_send */
  ftm_gsm_rf_msgr_send_debug( rfm_dev, RFA_RF_GSM_FTM_SET_LINEAR_RGI_CMD, result);

  /* Blocking wait for set Linear RGI RSP message from RF Task */
  result = msgr_receive(&ftm_rfgsm_msgr_client,
                                (uint8 *)&rfa_rf_gsm_ftm_set_linear_rgi_rsp,
                                (uint32)sizeof(rfa_rf_gsm_ftm_set_linear_rgi_type_rsp_s),
                                &bytes_recvd);

  /* Release FTM gsm mutex */
  RFA_FTM_GSM_ISR_UNLOCK(&ftm_gsm_lock);

  /* Sanity check and print debug for msgr_receive */
  ftm_gsm_rf_msgr_receive_debug( RFA_RF_GSM_FTM_SET_LINEAR_RGI_RSP,
                                 result,
                                 rfa_rf_gsm_ftm_set_linear_rgi_rsp.hdr.id,
                                 rfa_rf_gsm_ftm_set_linear_rgi_rsp.cmd_result );

}


/*===========================================================================
FUNCTION ftm_gsm_set_slot_override_flag

DESCRIPTION
  Set slot override flag.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_gsm_set_slot_override_flag ( rfm_device_enum_type rfm_dev,
                                      boolean flag )
{
  ftm_gsm_rf_task_set_slot_override_flag( rfm_dev, flag );
}


/*===========================================================================
FUNCTION ftm_gsm_rf_task_set_slot_override_flag

DESCRIPTION
  Signal RF Task via MSGR to set slot override flag.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_gsm_rf_task_set_slot_override_flag( rfm_device_enum_type rfm_dev,
                                             boolean flag )
{
  rfa_rf_gsm_ftm_set_slot_override_flag_type_cmd_s  rfa_rf_gsm_ftm_set_slot_override_flag_cmd;
  rfa_rf_gsm_ftm_set_slot_override_flag_type_rsp_s  rfa_rf_gsm_ftm_set_slot_override_flag_rsp;
  errno_enum_type result;
  uint32 bytes_recvd;

  /* Get handle to the FTM gsm mutex */
  RFA_FTM_GSM_ISR_LOCK(&ftm_gsm_lock);

  /* Initialise msgr header */
  msgr_init_hdr( &rfa_rf_gsm_ftm_set_slot_override_flag_cmd.hdr, MSGR_RFA_RF_GSM_FTM, RFA_RF_GSM_FTM_SET_SLOT_OVERRIDE_FLAG_CMD );

  /* Populate payload parameters */
  rfa_rf_gsm_ftm_set_slot_override_flag_cmd.rfm_dev = rfm_dev;
  rfa_rf_gsm_ftm_set_slot_override_flag_cmd.override_flag = flag;

  /* Send msgr message to RF Task */
  result = msgr_send(&rfa_rf_gsm_ftm_set_slot_override_flag_cmd.hdr, sizeof(rfa_rf_gsm_ftm_set_slot_override_flag_type_cmd_s));

  /* Sanity check and print debug for msgr_send */
  ftm_gsm_rf_msgr_send_debug( rfm_dev, RFA_RF_GSM_FTM_SET_SLOT_OVERRIDE_FLAG_CMD, result);

  /* Blocking wait for set PA range RSP message from RF Task */
  result = msgr_receive( &ftm_rfgsm_msgr_client,
                         (uint8 *)&rfa_rf_gsm_ftm_set_slot_override_flag_rsp,
                         (uint32)sizeof(rfa_rf_gsm_ftm_set_slot_override_flag_type_rsp_s),
                         &bytes_recvd );

  /* Release FTM gsm mutex */
  RFA_FTM_GSM_ISR_UNLOCK(&ftm_gsm_lock);

  /* Sanity check and print debug for msgr_receive */
  ftm_gsm_rf_msgr_receive_debug( RFA_RF_GSM_FTM_SET_SLOT_OVERRIDE_FLAG_RSP,
                                 result,
                                 rfa_rf_gsm_ftm_set_slot_override_flag_rsp.hdr.id,
                                 rfa_rf_gsm_ftm_set_slot_override_flag_rsp.cmd_result );

}

/*===========================================================================

FUNCTION  ftm_gsm_rf_msgr_send_debug

DESCRIPTION
  Debug sanity check for Message Router send

===========================================================================*/
void ftm_gsm_rf_msgr_send_debug( rfm_device_enum_type rfm_dev,
                                 msgr_umid_type umid,
                                 errno_enum_type result )
{
  if (result != E_SUCCESS)
  {
    MSG_3(MSG_SSID_FTM, MSG_LEGACY_ERROR,"Dev %d, UMID 0x%x: msgr_send failed: %d", rfm_dev, umid, result);
  }

  if (ftm_gsm_print_umid_debug)
  {
    MSG_3(MSG_SSID_FTM, MSG_LEGACY_LOW,"Dev %d, UMID 0x%x: msgr_send: %d", rfm_dev, umid, result);
  }
}

/*===========================================================================

FUNCTION  ftm_gsm_rf_msgr_receive_debug

DESCRIPTION
  Debug sanity check for Message Router receive

===========================================================================*/
void ftm_gsm_rf_msgr_receive_debug( msgr_umid_type umid, errno_enum_type result, msgr_umid_type received_umid, rfa_rf_gsm_ftm_cmd_outcome_t rsp_result)
{
  if (result != E_SUCCESS)
  {
    MSG_2(MSG_SSID_FTM, MSG_LEGACY_ERROR,"UMID=0x%x: FTM task blocking msgr_receive failed: %d", umid, result);
  }

  if (ftm_gsm_print_umid_debug)
  {
    MSG_2(MSG_SSID_FTM, MSG_LEGACY_LOW,"UMID=0x%x: FTM Task blocking msgr_received: %d", umid, result);
  }

  /* Check that we received the expected RSP UMID */
  if(received_umid != umid)
  {
    MSG_2(MSG_SSID_FTM, MSG_LEGACY_ERROR,"FTM Task blocking receive message: Expected UMID=0x%x: Received UMID=0x%x", umid, received_umid);
  }

  /* Check that RF MSGR Req completed successfully */
  if(rsp_result != RFA_RF_GSM_FTM_SUCCESS)
  {
    MSG_2(MSG_SSID_FTM, MSG_LEGACY_ERROR,"FTM Task blocking receive message: UMID=0x%x: RF GSM FTM Task reported failure: %d", umid, rsp_result);
  }
}




/*===========================================================================
FUNCTION ftm_gsm_rf_task_tuner_override

DESCRIPTION
  Send MSGR message to RF Task to override the Tuner tune code. Use MSGR blocking wait
  to receive response from RF Task with status of write success/failure

DEPENDENCIES
   None.

RETURN VALUE
   error status

SIDE EFFECTS
   None.

===========================================================================*/
uint32 ftm_gsm_rf_task_tuner_override ( rfm_device_enum_type rfm_dev,
                                        uint8 override_flag,
                                        void* data,
                                        uint8* tuner_nv_ptr,
                                        uint8* tuner_id_ptr)
{
  rfa_rf_gsm_ftm_tuner_override_type_cmd_s  rfa_rf_gsm_ftm_tuner_override_cmd;
  errno_enum_type result;
  uint32 bytes_recvd;

  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr( rfm_dev );

  /* Get handle to the FTM gsm mutex */
  RFA_FTM_GSM_ISR_LOCK(&ftm_gsm_lock);

  /* Initialise msgr header */
  msgr_init_hdr( &rfa_rf_gsm_ftm_tuner_override_cmd.hdr, MSGR_RFA_RF_GSM_FTM, RFA_RF_GSM_FTM_TUNER_TUNE_CODE_OVERRIDE_CMD );

  rfa_rf_gsm_ftm_tuner_override_cmd.rfm_dev                     = rfm_dev;
  rfa_rf_gsm_ftm_tuner_override_cmd.tuner_params.override_flag  = override_flag;
  rfa_rf_gsm_ftm_tuner_override_cmd.tuner_params.data_ptr       = data;
  rfa_rf_gsm_ftm_tuner_override_cmd.tuner_params.tuner_nv_ptr   = tuner_nv_ptr;
  rfa_rf_gsm_ftm_tuner_override_cmd.tuner_params.error_status   = 0xFFFFFFFF;
  rfa_rf_gsm_ftm_tuner_override_cmd.tuner_params.tuner_id_ptr   = tuner_id_ptr;

  /* Send msgr message to RF Task */
  result = msgr_send(&rfa_rf_gsm_ftm_tuner_override_cmd.hdr, sizeof(rfa_rf_gsm_ftm_tuner_override_type_cmd_s));

  /* Sanity check and print debug for msgr_send */
  ftm_gsm_rf_msgr_send_debug( rfm_dev, RFA_RF_GSM_FTM_TUNER_TUNE_CODE_OVERRIDE_CMD, result );

  /* Blocking wait for RSP message from RF Task */
  result = msgr_receive(&ftm_rfgsm_msgr_client,
                        (uint8 *)&gvars->rfa_rf_gsm_ftm_tuner_override_rsp,
                        (uint32)sizeof(rfa_rf_gsm_ftm_tuner_override_type_rsp_s),
                        &bytes_recvd);

  /* Release FTM gsm mutex */
  RFA_FTM_GSM_ISR_UNLOCK(&ftm_gsm_lock);

  /* Sanity check and print debug for msgr_receive */
  ftm_gsm_rf_msgr_receive_debug( RFA_RF_GSM_FTM_TUNER_TUNE_CODE_OVERRIDE_RSP,
                                result,
                                gvars->rfa_rf_gsm_ftm_tuner_override_rsp.hdr.id,
                                gvars->rfa_rf_gsm_ftm_tuner_override_rsp.cmd_result );


  return (gvars->rfa_rf_gsm_ftm_tuner_override_rsp.tuner_params.error_status);

}

/*----------------------------------------------------------------------------*/

/*!
   @brief

   Returns a flag that indicates if frame sweep calibration is running

   @description

   the frame sweep activity state is matched with the global RFM calibration flag,
   or the internal FTM GSM calibration variable.

   this criteria is used by the GSM core TX AGC controller and had to be time optimised.

   @todo

   ftm_gsm_frame_sweep_running variable could be put onto ftm_gsm_rfctl_vars_s making
   it device dependent.

   @param[input] rfm_device_enum_type rfm_dev -- converted into the associated PRX device

   @retval
   TRUE if frame sweep calibration is running
   FALSE is frame sweep calibration is not running

*/
boolean ftm_gsm_get_frame_sweep_cal_running(rfm_device_enum_type rfm_dev)
{
  boolean is_sweep_cal_running = FALSE;
  ftm_gsm_rfctl_vars_s * gvars = NULL;

  if (ftm_mode != FTM_MODE)
  {
    return is_sweep_cal_running;
  }

  gvars = ftm_gsm_get_vars_ptr(rfm_dev);

  if (gvars == NULL)
  {
    return is_sweep_cal_running;
  }

  is_sweep_cal_running  =
    (
     (ftm_gsm_frame_sweep_running == TRUE) &&
     ((gvars->ftm_gsm_cal_flag == TRUE) ||
      (rfm_get_calibration_state() == TRUE))
     ? TRUE
     : FALSE
    );

  return is_sweep_cal_running;
}

/*----------------------------------------------------------------------------*/

/*!
   @brief API to send IP2 cal params from FTM to RF and start IP2 cal
   There are two different modes in which IP2 cal can be run: Override mode, Cal mode
   Based on step duration, max length of setup and cleanup it determines the time
   in terms of number of frames it takes for a single iteration.
   Once all the IP2 cal iterations are over, it fetches the final results and returns
   them to the caller function.


   @retval

*/
boolean override_gsm_port_to_sawless = FALSE;

boolean ftm_gsm_set_ip2_cal_params_override ( rfm_device_enum_type rfm_dev, ftm_pkt_type* cmd_ptr, ftm_rsp_pkt_type* rsp_pkt_ptr  )
{

    rfa_rf_gsm_ftm_ip2_cal_type_cmd_s rfa_rf_gsm_set_ip2_cal_params_cmd;
    rfa_rf_gsm_ftm_ip2_cal_type_rsp_s rfa_rf_gsm_set_ip2_cal_params_rsp;
    ftm_gsm_ip2_cal_resp_type* ip2_cal_resp; /* IP2 Cal specific response payload */
    errno_enum_type result;
    uint32 bytes_recvd;
    uint16 ip2_cal_frame_len = 0;
    ftm_rfstate_enum_type ftm_rf_state = FTM_STATE_UNKNOWN;
    uint32 start_time = geran_test_mode_api_get_qsym_count(SYS_MODEM_AS_ID_1);
    uint32 stop_time = 0;
    rfgsm_ip2_cal_results_type ip2_cal_results;

	boolean rfc_sawless_supported = TRUE;

    ftm_gsm_ip2_cal_params_type* ip2_cal_config =
      (ftm_gsm_ip2_cal_params_type*)&(cmd_ptr->rf_params.ftm_rf_factory_data.gsm_struct.ftm_gsm_data_union.ftm_gsm_ip2_cal_params);

    ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr( rfm_dev );


    ip2_cal_results.final_i_code = 0xFFFF;
    ip2_cal_results.final_q_code = 0xFFFF;
    ip2_cal_results.min_im2_val = 0xFFFFFFFF;
    ip2_cal_results.status = FALSE;

    ftm_rf_state = ftm_get_current_state(rfm_dev);


    if( !((ftm_rf_state == FTM_STATE_GSM)  || (ftm_rf_state == FTM_STATE_GSM_NS )) )
    {

      MSG_1( MSG_SSID_FTM, MSG_LEGACY_ERROR, "ftm_gsm_set_ip2_cal_params_override failed. Not in GSM mode: %d", ftm_rf_state );
       return FALSE;
    }


    if (ip2_cal_config == NULL)
    {
      return FALSE;
    }

    if (NULL == gvars)
    {
      MSG_1( MSG_SSID_FTM, MSG_LEGACY_ERROR, "ftm_gsm_set_ip2_cal_params_override failed. ftm variables null for device %d", rfm_dev );
      return FALSE;
    }

    /* Make sure IP2 cal is not running already*/
    if (TRUE == gvars->ip2_cal_data.do_ip2_cal)
    {

      MSG_1( MSG_SSID_FTM, MSG_LEGACY_ERROR, "ftm_gsm_set_ip2_cal_params_override failed. IP2 cal is already running. Please wait till it is complete. device: %d", rfm_dev );
      return FALSE;
    }

    if (rfdevice_gsm_get_sawless_support(rfm_dev, ip2_cal_config->rfcom_band, &rfc_sawless_supported)
		&& rfc_sawless_supported == FALSE)
    {
		override_gsm_port_to_sawless = TRUE;
		MSG( MSG_SSID_FTM, MSG_LEGACY_HIGH, "ftm_gsm_set_ip2_cal_params_override, override_gsm_port_to_sawless on SAW RFC!" );
		ftm_rfmode_exit(rfm_dev,FTM_STATE_GSM);
		ftm_rfmode_enter(rfm_dev,FTM_STATE_GSM);
    }

    if (!ip2_cal_config->use_default_params)
    {

      /* Get handle to the FTM gsm mutex */
      RFA_FTM_GSM_ISR_LOCK(&ftm_gsm_lock);

      /* Initialise msgr header */
      msgr_init_hdr( &rfa_rf_gsm_set_ip2_cal_params_cmd.hdr, MSGR_RFA_RF_GSM_FTM, RFA_RF_GSM_FTM_SET_IP2_CAL_PARAMS_CMD );

      rfa_rf_gsm_set_ip2_cal_params_cmd.rfm_dev                               = rfm_dev;
      rfa_rf_gsm_set_ip2_cal_params_cmd.sub_id                                = gvars->sub_id;
      rfa_rf_gsm_set_ip2_cal_params_cmd.ip2_cal_params.use_default_params     = ip2_cal_config->use_default_params;
      rfa_rf_gsm_set_ip2_cal_params_cmd.ip2_cal_params.rfcom_band             = ip2_cal_config->rfcom_band;
      rfa_rf_gsm_set_ip2_cal_params_cmd.ip2_cal_params.start_i_code           = ip2_cal_config->start_i_code;
      rfa_rf_gsm_set_ip2_cal_params_cmd.ip2_cal_params.start_q_code           = ip2_cal_config->start_q_code;
      //rfa_rf_gsm_set_ip2_cal_params_cmd.ip2_cal_params.num_cal_points         = ip2_cal_config->num_cal_points;


      if (ip2_cal_config->width > MAX_IP2_CAL_WIDTH)
      {

        MSG( MSG_SSID_FTM, MSG_LEGACY_HIGH, "ftm_gsm_set_ip2_cal_params_override, width over bounds,using default value" );
        ip2_cal_config->width = MAX_IP2_CAL_WIDTH;
      }

      rfa_rf_gsm_set_ip2_cal_params_cmd.ip2_cal_params.width                  = ip2_cal_config->width;

      if (ip2_cal_config->unit_step_size > MAX_IP2_CAL_WIDTH)
      {

        MSG( MSG_SSID_FTM, MSG_LEGACY_HIGH, "ftm_gsm_set_ip2_cal_params_override, unit step size over bounds,using default value" );
        ip2_cal_config->unit_step_size = MAX_IP2_CAL_WIDTH;
      }

      rfa_rf_gsm_set_ip2_cal_params_cmd.ip2_cal_params.unit_step_size         = ip2_cal_config->unit_step_size;

      rfa_rf_gsm_set_ip2_cal_params_cmd.ip2_cal_params.arfcn                  = ip2_cal_config->arfcn;
      rfa_rf_gsm_set_ip2_cal_params_cmd.ip2_cal_params.tx_rgi                 = ip2_cal_config->tx_rgi;
      rfa_rf_gsm_set_ip2_cal_params_cmd.ip2_cal_params.lna_range              = ip2_cal_config->lna_range;

      if ( ip2_cal_config->step_duration_qs > MAX_IP2_CAL_STEP_LENGTH_QS )
      {

        MSG( MSG_SSID_FTM, MSG_LEGACY_HIGH, "ftm_gsm_set_ip2_cal_params_override, step duration over bounds,using default value" );
        ip2_cal_config->step_duration_qs = MAX_IP2_CAL_STEP_LENGTH_QS;

      }

      rfa_rf_gsm_set_ip2_cal_params_cmd.ip2_cal_params.step_duration_qs       = ip2_cal_config->step_duration_qs;




      if( ip2_cal_config->dc_settling_time_qs > MAX_DC_SETTLING_TIME_QS )
      {
        MSG( MSG_SSID_FTM, MSG_LEGACY_HIGH, "ftm_gsm_set_ip2_cal_params_override, dc settling time over bounds, using default value");
        ip2_cal_config->dc_settling_time_qs = MAX_DC_SETTLING_TIME_QS;
      }

      rfa_rf_gsm_set_ip2_cal_params_cmd.ip2_cal_params.dc_settling_time_qs    = ip2_cal_config->dc_settling_time_qs;



      MSG_8( MSG_SSID_FTM, MSG_LEGACY_HIGH,
             "ftm_gsm_set_ip2_cal_params_override, i code: %d, q code:%d, unit_step_size:%d, width:%d, arfcn:%d, band:%d, TX rgi:%d, lna_range:%d ",
             ip2_cal_config->start_i_code, ip2_cal_config->start_q_code, ip2_cal_config->unit_step_size,
             ip2_cal_config->width, ip2_cal_config->arfcn, ip2_cal_config->rfcom_band, ip2_cal_config->tx_rgi, ip2_cal_config->lna_range );

      MSG_3( MSG_SSID_FTM, MSG_LEGACY_HIGH,
             "ftm_gsm_set_ip2_cal_params_override, step duration (qs): %d, dc settling time:%d, use default params:%d ",
             ip2_cal_config->step_duration_qs, ip2_cal_config->dc_settling_time_qs, ip2_cal_config->use_default_params  );

      /* Send msgr message to RF Task */
      result = msgr_send(&rfa_rf_gsm_set_ip2_cal_params_cmd.hdr, sizeof(rfa_rf_gsm_ftm_ip2_cal_type_cmd_s));

      if (result != E_SUCCESS)
      {

        MSG( MSG_SSID_FTM, MSG_LEGACY_HIGH, "ftm_gsm_set_ip2_cal_params_override, send cmd is not successful " );
        return FALSE;
      }

      /* Sanity check and print debug for msgr_send */
      ftm_gsm_rf_msgr_send_debug( rfm_dev, RFA_RF_GSM_FTM_SET_IP2_CAL_PARAMS_CMD, result );




      /* Blocking wait for RSP message from RF Task */
     result = msgr_receive(&ftm_rfgsm_msgr_client,
                         (uint8 *)&rfa_rf_gsm_set_ip2_cal_params_rsp,
                         (uint32)sizeof(rfa_rf_gsm_ftm_ip2_cal_type_rsp_s),
                         &bytes_recvd);

      /* Release FTM gsm mutex */
      RFA_FTM_GSM_ISR_UNLOCK(&ftm_gsm_lock);


     /* Sanity check and print debug for msgr_receive */
     ftm_gsm_rf_msgr_receive_debug( RFA_RF_GSM_FTM_SET_IP2_CAL_PARAMS_RSP,
                                  result,
                                  rfa_rf_gsm_set_ip2_cal_params_rsp.hdr.id,
                                  rfa_rf_gsm_set_ip2_cal_params_rsp.cmd_result );

     if (result != E_SUCCESS)
     {

       MSG( MSG_SSID_FTM, MSG_LEGACY_HIGH, "ftm_gsm_set_ip2_cal_params_override, Response is not successful " );
       return FALSE;
     }


     /*After sending the IP2 params to RF, Update FTM variables to start IP2 cal */
     gvars->ip2_cal_data.num_frames = ip2_cal_config->num_frames;
     gvars->ip2_cal_data.start_frame_ctr = 0;
     gvars->ip2_cal_data.num_cal_steps = ((2*ip2_cal_config->width)+1)*((2*ip2_cal_config->width)+1);
     gvars->ip2_cal_data.dc_settling_time_qs = ip2_cal_config->dc_settling_time_qs;
     gvars->ip2_cal_data.step_duration_qs = ip2_cal_config->step_duration_qs;
   }
   else
   {

     /* Stop Vbatt Tx pwr compensation.during IP2 cal */
     rfgsm_core_vbatt_comp_stop(rfm_dev);

     /* Stop PA Temp pwr compensation.during IP2 cal */
     rfgsm_core_temp_comp_stop(rfm_dev);

     MSG_2( MSG_SSID_FTM, MSG_LEGACY_HIGH, 
            "ftm_gsm_set_ip2_cal_params_override, fetching default params for dev: %d, band:%d ", 
            rfm_dev,ip2_cal_config->rfcom_band );

     /* Obtain the IP2 cal related info */
     if(!ftm_gsm_set_ip2_cal_params_default(rfm_dev, ip2_cal_config->rfcom_band))
     {

       MSG( MSG_SSID_FTM, MSG_LEGACY_ERROR, "ftm_gsm_set_ip2_cal_params_default failed " );
       return FALSE;
     }
   }

   stop_time = geran_test_mode_api_get_qsym_count(SYS_MODEM_AS_ID_1);

   /* Update band info passed and track it in FTM variables. Band info needs to be passed whether in default mode or not*/
   gvars->ip2_cal_data.band_in_use = ip2_cal_config->rfcom_band ;


   ip2_cal_frame_len = ((gvars->ip2_cal_data.num_cal_steps)*( gvars->ip2_cal_data.step_duration_qs))+ ((gvars->ip2_cal_data.num_cal_steps)*(gvars->ip2_cal_data.dc_settling_time_qs)) + MAX_IP2_CAL_SETUP_LENGTH_QS + MAX_IP2_CAL_CLEANUP_LENGTH_QS +625;


    /*Calculate how many frames it takes for one single IP2 cal*/
    gvars->ip2_cal_data.num_frames_per_iter = (ip2_cal_frame_len / GSM_FRAME_LENGTH_QS) + ((ip2_cal_frame_len % GSM_FRAME_LENGTH_QS) > 0 ? 1 : 0) + 1;

    MSG_2( MSG_SSID_FTM, MSG_LEGACY_HIGH, "ftm_gsm_set_ip2_cal_params_override, number of frames per IP2 cal iteration:%d, time taken:%d ", gvars->ip2_cal_data.num_frames_per_iter, (stop_time-start_time) );

    gvars->ip2_cal_data.do_ip2_cal = TRUE; // Start IP2 cal

    /*---------------------------------------- Add blocking wait to collect result----------------------------------------------*/


   MSG_1( MSG_SSID_FTM, MSG_LEGACY_HIGH,
          "ftm_gsm_set_ip2_cal_params_override, wait began for :%d qs ",
          ( (gvars->ip2_cal_data.num_frames_per_iter*GSM_FRAME_LENGTH_QS*gvars->ip2_cal_data.num_frames) + 1000 ));

   DALSYS_BusyWait( (gvars->ip2_cal_data.num_frames_per_iter*GSM_FRAME_LENGTH_QS*gvars->ip2_cal_data.num_frames) + GSM_FRAME_LENGTH_QS );

   MSG( MSG_SSID_FTM, MSG_LEGACY_HIGH, "ftm_gsm_set_ip2_cal_params_override, wait done " );



   /*---------------------------------------- fetching results after wait-----------------------------------------------*/
   ftm_gsm_get_ip2_cal_results(rfm_dev,
                               gvars->ip2_cal_data.band_in_use,
                               0x3,
                               &ip2_cal_results,
                               TRUE );

   MSG_4( MSG_SSID_FTM, MSG_LEGACY_HIGH,
          "ftm_gsm_set_ip2_cal_params_override, update IP2 cal results for band:%d, final i code:%d, final q code:%d, min im2 val:%d ",
          gvars->ip2_cal_data.band_in_use, ip2_cal_results.final_i_code, ip2_cal_results.final_q_code, ip2_cal_results.min_im2_val);

   MSG_1( MSG_SSID_FTM, MSG_LEGACY_HIGH,
          "ftm_gsm_set_ip2_cal_params_override, ip2_cal_status:%d ",ip2_cal_results.status);

   /*---------------------------------------- returning response packet to Diag-----------------------------------------------*/
   if(rsp_pkt_ptr != NULL)
   {



    ip2_cal_resp = (ftm_gsm_ip2_cal_resp_type*)((byte *)rsp_pkt_ptr->pkt_payload + sizeof(ftm_composite_cmd_header_type));

    ip2_cal_resp->status = ip2_cal_results.status;
    ip2_cal_resp->min_im2_result = ip2_cal_results.min_im2_val;
    ip2_cal_resp->final_i_code = ip2_cal_results.final_i_code;
    ip2_cal_resp->final_q_code = ip2_cal_results.final_q_code;
   }

   if(override_gsm_port_to_sawless == TRUE)
   {
     override_gsm_port_to_sawless = FALSE;
	 ftm_rfmode_exit(rfm_dev,FTM_STATE_GSM);
	 ftm_rfmode_enter(rfm_dev,FTM_STATE_GSM);
   }

   return TRUE;
}


/*----------------------------------------------------------------------------*/

/*!
   @brief This API calls GL1 API to trigger IP2 cal for every iteration of IP2 cal.
   This API keeps track of number of frames elapsed since start of IP2 cal and when to trigger
   the next iteation of IP2 cal.

   @details

   @param rfm_dev
   The logical device

   @retval none

*/

void ftm_gsm_do_ip2_cal(rfm_device_enum_type rfm_dev)
{
    ftm_gsm_rfctl_vars_s * vars_ptr = ftm_gsm_get_vars_ptr(rfm_dev);
    uint16 num_frames_single_iter = 0;


    if (vars_ptr == NULL)
    {

      MSG( MSG_SSID_FTM, MSG_LEGACY_ERROR, "ftm_gsm_do_ip2_cal, NULL ptr " );
      return;
    }

    if(FALSE == vars_ptr->ip2_cal_data.do_ip2_cal )
    {
      MSG_1( MSG_SSID_FTM, MSG_LEGACY_ERROR,
             "ftm_gsm_do_ip2_cal called despite do_ip2_cal is set:%d ",
             vars_ptr->ip2_cal_data.do_ip2_cal );
      return;
    }


    MSG_4( MSG_SSID_FTM, MSG_LEGACY_HIGH,
           "ftm_gsm_do_ip2_cal, Do IP2 cal:%d, start_frame_ctr:%d, number of IP2 cal frames left:%d, frame number:%d ",
           vars_ptr->ip2_cal_data.do_ip2_cal,vars_ptr->ip2_cal_data.start_frame_ctr,
           vars_ptr->ip2_cal_data.num_frames,geran_test_mode_api_get_FN(vars_ptr->sub_id) );

    num_frames_single_iter = vars_ptr->ip2_cal_data.num_frames_per_iter;

    if (num_frames_single_iter == 0)
    {
      MSG_1( MSG_SSID_FTM, MSG_LEGACY_ERROR, "ftm_gsm_do_ip2_cal, invalid number of frames per iteration:%d ", num_frames_single_iter );
      return;
    }


    if( 0 == vars_ptr->ip2_cal_data.num_frames)
    {


       /* Reset local data after Last IP2 cal frame*/
       ftm_gsm_reset_ip2_cal_vars(vars_ptr);


    }
    else
    {

      /*IP2 cal has started*/
      vars_ptr->ip2_cal_data.start_frame_ctr++;


      /* If geran API is called at frame tick for frame N-1, IP2 cal takes place in frame N. Call geran API to schedule IP2 cal*/


      if( (vars_ptr->ip2_cal_data.start_frame_ctr % num_frames_single_iter) == 1 )
      {

       /* exclude the very first frame when ftm_gsm_do_ip2_cal is called*/
       if (vars_ptr->ip2_cal_data.start_frame_ctr > 1)
       {
         /* decrement counter in the frame IP2 cal takes place*/
         vars_ptr->ip2_cal_data.num_frames--;
       }

       /* IP2 cal complete*/
       if( 0 == vars_ptr->ip2_cal_data.num_frames)
       {
          /* Reset after Last IP2 cal frame*/

          MSG_2( MSG_SSID_FTM, MSG_LEGACY_HIGH,
                 "ftm_gsm_do_ip2_cal, Stop IP2 cal. start_frame_ctr:%d, number of IP2 cal frames left:%d ",
                 vars_ptr->ip2_cal_data.start_frame_ctr, vars_ptr->ip2_cal_data.num_frames );
          vars_ptr->ip2_cal_data.do_ip2_cal = FALSE;
          vars_ptr->ip2_cal_data.start_frame_ctr = 0;

          return;
       }

       geran_test_mode_api_do_ip2cal( vars_ptr->ip2_cal_data.num_cal_steps,vars_ptr->ip2_cal_data.step_duration_qs,gl1_hw_rf_map_device2_as_id(rfm_dev,ftm_mode) );

      /*-------------*/


      }
    }

}

/*----------------------------------------------------------------------------*/

/*!
   @brief API to reset IP2 Cal variables

   @retval

*/

void ftm_gsm_reset_ip2_cal_vars(ftm_gsm_rfctl_vars_s * vars)
{
  if (vars == NULL)
  {

    MSG( MSG_SSID_FTM, MSG_LEGACY_ERROR, "ftm_gsm_reset_ip2_cal_vars failed. null ptr"  );
    return;
  }
  vars->ip2_cal_data.num_frames_per_iter = 0;
  vars->ip2_cal_data.do_ip2_cal = FALSE;

  vars->ip2_cal_data.num_frames = 0;
  vars->ip2_cal_data.start_frame_ctr = 0;
  vars->ip2_cal_data.num_cal_steps = 0;
  vars->ip2_cal_data.step_duration_qs = 0;
  vars->ip2_cal_data.dc_settling_time_qs = 0;
  vars->ip2_cal_data.band_in_use = 0xFF;

}



/*----------------------------------------------------------------------------*/
/*!
   @brief This API sends an cmd to RFGSM core to fetch and apply the default IP2 cal params
   supported in WTR spreadsheet.
   This API is typically called when IP2 cal is done in cal mode.

   @details

   @param rfm_dev, rfcom_band

   @retval none

*/

boolean ftm_gsm_set_ip2_cal_params_default(rfm_device_enum_type rfm_dev, uint8 rfcom_band)
{

    rfa_rf_gsm_ftm_get_ip2_cal_info_cmd_s rfa_rf_gsm_get_ip2_cal_params_cmd;
    rfa_rf_gsm_ftm_get_ip2_cal_info_rsp_s rfa_rf_gsm_get_ip2_cal_params_rsp;
    errno_enum_type result;
    uint32 bytes_recvd;
    ftm_rfstate_enum_type ftm_rf_state = FTM_STATE_UNKNOWN;
    ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);

    ftm_rf_state = ftm_get_current_state(rfm_dev);


    if (NULL == gvars)
    {
      MSG_1( MSG_SSID_FTM, MSG_LEGACY_ERROR, "ftm_gsm_set_ip2_cal_params_default failed. ftm variables null for device %d ", rfm_dev );
      return FALSE;
    }


    if( !((ftm_rf_state == FTM_STATE_GSM)  || (ftm_rf_state == FTM_STATE_GSM_NS )) )
    {
      MSG_1( MSG_SSID_FTM, MSG_LEGACY_ERROR, "ftm_gsm_set_ip2_cal_params_default failed. Not in GSM mode: %d ", ftm_rf_state );
      return FALSE;
    }


    /* Get handle to the FTM gsm mutex */
    RFA_FTM_GSM_ISR_LOCK(&ftm_gsm_lock);

    /* Initialise msgr header */
    msgr_init_hdr( &rfa_rf_gsm_get_ip2_cal_params_cmd.hdr, MSGR_RFA_RF_GSM_FTM, RFA_RF_GSM_FTM_GET_IP2_CAL_PARAMS_CMD );

    /*Populate data to send*/
    rfa_rf_gsm_get_ip2_cal_params_cmd.rfm_dev              = rfm_dev;
    rfa_rf_gsm_get_ip2_cal_params_cmd.rfcom_band           = rfcom_band;
    rfa_rf_gsm_get_ip2_cal_params_cmd.sub_id               = gvars->sub_id;

    /* Send msgr message to RF Task */
    result = msgr_send(&rfa_rf_gsm_get_ip2_cal_params_cmd.hdr, sizeof(rfa_rf_gsm_ftm_get_ip2_cal_info_cmd_s));


    ftm_gsm_rf_msgr_send_debug( rfm_dev, RFA_RF_GSM_FTM_GET_IP2_CAL_PARAMS_CMD, result );

    if (result != E_SUCCESS)
    {

     MSG( MSG_SSID_FTM, MSG_LEGACY_HIGH, "ftm_gsm_set_ip2_cal_params_default, send cmd is not successful " );
     return FALSE;
    }

     /* Blocking wait for RSP message from RF Task */
    result = msgr_receive(&ftm_rfgsm_msgr_client,
                     (uint8 *)&rfa_rf_gsm_get_ip2_cal_params_rsp,
                     (uint32)sizeof(rfa_rf_gsm_ftm_get_ip2_cal_info_rsp_s),
                     &bytes_recvd);

    /* Release FTM gsm mutex */
    RFA_FTM_GSM_ISR_UNLOCK(&ftm_gsm_lock);



    /* Sanity check and print debug for msgr_receive */
    ftm_gsm_rf_msgr_receive_debug( RFA_RF_GSM_FTM_GET_IP2_CAL_PARAMS_RSP,
                              result,
                              rfa_rf_gsm_get_ip2_cal_params_rsp.hdr.id,
                              rfa_rf_gsm_get_ip2_cal_params_rsp.cmd_result );

    if (result != E_SUCCESS)
    {

      MSG( MSG_SSID_FTM, MSG_LEGACY_HIGH, "ftm_gsm_set_ip2_cal_params_default, Response is not successful " );
      return FALSE;
    }


    MSG_5( MSG_SSID_FTM, MSG_LEGACY_HIGH, "ftm_gsm_set_ip2_cal_params_default, band:%d, number of iterations:%d, width:%d, step duration:%d, dc settling time:%d ",
           rfcom_band,
           rfa_rf_gsm_get_ip2_cal_params_rsp.num_frames,
           rfa_rf_gsm_get_ip2_cal_params_rsp.width,
           rfa_rf_gsm_get_ip2_cal_params_rsp.step_duration_qs,
           rfa_rf_gsm_get_ip2_cal_params_rsp.dc_settling_time_qs );

    gvars->ip2_cal_data.num_frames = rfa_rf_gsm_get_ip2_cal_params_rsp.num_frames;
    gvars->ip2_cal_data.start_frame_ctr = 0;
    gvars->ip2_cal_data.num_cal_steps = ((2*rfa_rf_gsm_get_ip2_cal_params_rsp.width)+1)*((2*rfa_rf_gsm_get_ip2_cal_params_rsp.width)+1);
    gvars->ip2_cal_data.step_duration_qs = rfa_rf_gsm_get_ip2_cal_params_rsp.step_duration_qs;
    gvars->ip2_cal_data.dc_settling_time_qs = rfa_rf_gsm_get_ip2_cal_params_rsp.dc_settling_time_qs;

    return TRUE;

}

/*----------------------------------------------------------------------------*/
/*!
   @brief ftm_gsm_get_ip2_cal_results
   This API fetches provides the flexibility to
   - update IP2 cal data and write to efs
   - only update ip2 cal data
   - read from efs
   @details

   @param
   flag_mask
   bit 0: update + write to EFS
   bit 1: update only
   bit 2: read from efs

   @retval

*/
boolean ftm_gsm_get_ip2_cal_results(rfm_device_enum_type rfm_dev,
                                    uint8 rfcom_band,
                                    uint8 flag_mask,
                                    rfgsm_ip2_cal_results_type *ip2_cal_results,
                                    boolean cal_end)
{

    rfa_rf_gsm_ftm_get_ip2_cal_results_cmd_s rfa_rf_gsm_get_ip2_cal_results_cmd;
    rfa_rf_gsm_ftm_get_ip2_cal_results_rsp_s rfa_rf_gsm_get_ip2_cal_results_rsp;
    errno_enum_type result;
    uint32 bytes_recvd;
    ftm_rfstate_enum_type ftm_rf_state = FTM_STATE_UNKNOWN;

    ftm_rf_state = ftm_get_current_state(rfm_dev);

    if( !((ftm_rf_state == FTM_STATE_GSM)  || (ftm_rf_state == FTM_STATE_GSM_NS )) )
    {
      MSG_1( MSG_SSID_FTM, MSG_LEGACY_ERROR, "ftm_gsm_get_ip2_cal_results failed. Not in GSM mode: %d ", ftm_rf_state );
      return FALSE;
    }


    /* Get handle to the FTM gsm mutex */
    RFA_FTM_GSM_ISR_LOCK(&ftm_gsm_lock);

    /* Initialise msgr header */
    msgr_init_hdr( &rfa_rf_gsm_get_ip2_cal_results_cmd.hdr, MSGR_RFA_RF_GSM_FTM, RFA_RF_GSM_FTM_GET_IP2_CAL_RESULTS_CMD );

    /*Populate data to send*/
    rfa_rf_gsm_get_ip2_cal_results_cmd.rfm_dev              = rfm_dev;
    rfa_rf_gsm_get_ip2_cal_results_cmd.sub_id               = 0;
    rfa_rf_gsm_get_ip2_cal_results_cmd.rfcom_band           = rfcom_band;
    rfa_rf_gsm_get_ip2_cal_results_cmd.flag_mask            = flag_mask;
    rfa_rf_gsm_get_ip2_cal_results_cmd.ip2_cal_end          = cal_end;

    /* Send msgr message to RF Task */
    result = msgr_send(&rfa_rf_gsm_get_ip2_cal_results_cmd.hdr, sizeof(rfa_rf_gsm_ftm_get_ip2_cal_results_cmd_s));


    ftm_gsm_rf_msgr_send_debug( rfm_dev, RFA_RF_GSM_FTM_GET_IP2_CAL_RESULTS_CMD, result );

    if (result != E_SUCCESS)
    {

       MSG( MSG_SSID_FTM, MSG_LEGACY_HIGH, "ftm_gsm_get_ip2_cal_results, send cmd is not successful " );
       return FALSE;
    }

     /* Blocking wait for RSP message from RF Task */
    result = msgr_receive(&ftm_rfgsm_msgr_client,
                     (uint8 *)&rfa_rf_gsm_get_ip2_cal_results_rsp,
                     (uint32)sizeof(rfa_rf_gsm_ftm_get_ip2_cal_results_rsp_s),
                     &bytes_recvd);

    /* Release FTM gsm mutex */
    RFA_FTM_GSM_ISR_UNLOCK(&ftm_gsm_lock);



    /* Sanity check and print debug for msgr_receive */
    ftm_gsm_rf_msgr_receive_debug( RFA_RF_GSM_FTM_GET_IP2_CAL_RESULTS_RSP,
                              result,
                              rfa_rf_gsm_get_ip2_cal_results_rsp.hdr.id,
                              rfa_rf_gsm_get_ip2_cal_results_rsp.cmd_result );

    if (result != E_SUCCESS)
    {

      MSG( MSG_SSID_FTM, MSG_LEGACY_HIGH, "ftm_gsm_get_ip2_cal_results, Response is not successful " );
      return FALSE;
    }



    if (ip2_cal_results != NULL)
    {
      ip2_cal_results->final_i_code = rfa_rf_gsm_get_ip2_cal_results_rsp.final_i_code;
      ip2_cal_results->final_q_code = rfa_rf_gsm_get_ip2_cal_results_rsp.final_q_code;
      ip2_cal_results->min_im2_val = rfa_rf_gsm_get_ip2_cal_results_rsp.min_im2_val;
      ip2_cal_results->status = rfa_rf_gsm_get_ip2_cal_results_rsp.status;
    }

    return TRUE;

}
/*----------------------------------------------------------------------------*/
/*!
   @brief - ftm_gsm_sawless_lin_override

   @details - Instructs RF sw to override the HL/LL status for a specific channel
   and band

   @param
   rfm_device_enum_type rfm_dev - logical device to which this change should be
   applied
   ftm_gsm_sawless_lin_type* lin_params - contains channel, band, and setting info

   @retval
   void
*/
boolean ftm_gsm_sawless_lin_override( rfm_device_enum_type rfm_dev,
                                      ftm_gsm_sawless_lin_type* lin_params )
{
  rfa_rf_gsm_ftm_set_sawless_lin_mode_cmd_s rfa_rf_gsm_set_sawless_lin_mode_cmd;
  rfa_rf_gsm_ftm_set_sawless_lin_mode_rsp_s rfa_rf_gsm_set_sawless_lin_mode_rsp;
  errno_enum_type result;
  uint32 bytes_recvd;
  ftm_rfstate_enum_type ftm_rf_state = FTM_STATE_UNKNOWN;

  ftm_rf_state = ftm_get_current_state(rfm_dev);

  if( !((ftm_rf_state == FTM_STATE_GSM)  || (ftm_rf_state == FTM_STATE_GSM_NS )) )
  {
    MSG_1( MSG_SSID_FTM, MSG_LEGACY_ERROR, "ftm_gsm_sawless_lin_override failed. Not in GSM mode: %d ", ftm_rf_state );
    return FALSE;
  }

  /* Get handle to the FTM gsm mutex */
  RFA_FTM_GSM_ISR_LOCK(&ftm_gsm_lock);

  /* Initialise msgr header */
  msgr_init_hdr( &rfa_rf_gsm_set_sawless_lin_mode_cmd.hdr, MSGR_RFA_RF_GSM_FTM, RFA_RF_GSM_FTM_SET_SAWLESS_LIN_MODE_CMD );

  /*Populate data to send*/
  rfa_rf_gsm_set_sawless_lin_mode_cmd.rfm_dev              = rfm_dev;
  rfa_rf_gsm_set_sawless_lin_mode_cmd.band                 = lin_params->band;
  rfa_rf_gsm_set_sawless_lin_mode_cmd.arfcn                = lin_params->arfcn;
  rfa_rf_gsm_set_sawless_lin_mode_cmd.use_high_lin_mode    = lin_params->use_high_lin;

  /* Send msgr message to RF Task */
  result = msgr_send(&rfa_rf_gsm_set_sawless_lin_mode_cmd.hdr, sizeof(rfa_rf_gsm_ftm_set_sawless_lin_mode_cmd_s));


  ftm_gsm_rf_msgr_send_debug( rfm_dev, RFA_RF_GSM_FTM_SET_SAWLESS_LIN_MODE_CMD, result );

  if (result != E_SUCCESS)
  {

     MSG( MSG_SSID_FTM, MSG_LEGACY_HIGH, "ftm_gsm_sawless_lin_override, send cmd is not successful " );
     return FALSE;
  }

   /* Blocking wait for RSP message from RF Task */
  result = msgr_receive( &ftm_rfgsm_msgr_client,
                         (uint8 *)&rfa_rf_gsm_set_sawless_lin_mode_rsp,
                         (uint32)sizeof(rfa_rf_gsm_ftm_set_sawless_lin_mode_rsp_s),
                         &bytes_recvd);

  /* Release FTM gsm mutex */
  RFA_FTM_GSM_ISR_UNLOCK(&ftm_gsm_lock);



  /* Sanity check and print debug for msgr_receive */
  ftm_gsm_rf_msgr_receive_debug( RFA_RF_GSM_FTM_SET_SAWLESS_LIN_MODE_RSP,
                                 result,
                                 rfa_rf_gsm_set_sawless_lin_mode_rsp.hdr.id,
                                 rfa_rf_gsm_set_sawless_lin_mode_rsp.cmd_result );

  if (result != E_SUCCESS)
  {

    MSG( MSG_SSID_FTM, MSG_LEGACY_HIGH, "ftm_gsm_sawless_lin_override, Response is not successful " );
    return FALSE;
  }

  return TRUE;

}
/*----------------------------------------------------------------------------*/
/*!
   @brief - ftm_gsm_get_sawless_lin_mode

   @details - Reads back the currently set HL/LL decision for the specified
   channel and band from RFSW

   @param
   rfm_device_enum_type rfm_dev - logical device to which this change should be
   applied
   ftm_gsm_sawless_lin_type* lin_params - contains channel, band, and setting info
   (to hold returned setting value)

   @retval
   void
*/
boolean ftm_gsm_get_sawless_lin_mode( rfm_device_enum_type rfm_dev,
                                      ftm_gsm_sawless_lin_type* lin_params )
{
  rfa_rf_gsm_ftm_get_sawless_lin_mode_cmd_s rfa_rf_gsm_get_sawless_lin_mode_cmd;
  rfa_rf_gsm_ftm_get_sawless_lin_mode_rsp_s rfa_rf_gsm_get_sawless_lin_mode_rsp;
  errno_enum_type result;
  uint32 bytes_recvd;
  ftm_rfstate_enum_type ftm_rf_state = FTM_STATE_UNKNOWN;

  ftm_rf_state = ftm_get_current_state(rfm_dev);

  if( !((ftm_rf_state == FTM_STATE_GSM)  || (ftm_rf_state == FTM_STATE_GSM_NS )) )
  {

    MSG_1( MSG_SSID_FTM, MSG_LEGACY_ERROR, "ftm_gsm_get_sawless_lin_mode failed. Not in GSM mode: %d", ftm_rf_state );
     return FALSE;
  }

  /* Get handle to the FTM gsm mutex */
  RFA_FTM_GSM_ISR_LOCK(&ftm_gsm_lock);

  /* Initialise msgr header */
  msgr_init_hdr( &rfa_rf_gsm_get_sawless_lin_mode_cmd.hdr, MSGR_RFA_RF_GSM_FTM, RFA_RF_GSM_FTM_GET_SAWLESS_LIN_MODE_CMD );

  /*Populate data to send*/
  rfa_rf_gsm_get_sawless_lin_mode_cmd.rfm_dev              = rfm_dev;
  rfa_rf_gsm_get_sawless_lin_mode_cmd.band                 = lin_params->band;
  rfa_rf_gsm_get_sawless_lin_mode_cmd.arfcn                = lin_params->arfcn;

  /* Send msgr message to RF Task */
  result = msgr_send(&rfa_rf_gsm_get_sawless_lin_mode_cmd.hdr, sizeof(rfa_rf_gsm_ftm_get_sawless_lin_mode_cmd_s));


  ftm_gsm_rf_msgr_send_debug( rfm_dev, RFA_RF_GSM_FTM_GET_SAWLESS_LIN_MODE_CMD, result );

  if (result != E_SUCCESS)
  {

     MSG( MSG_SSID_FTM, MSG_LEGACY_HIGH, "ftm_gsm_get_sawless_lin_mode, send cmd is not successful " );
     return FALSE;
  }

   /* Blocking wait for RSP message from RF Task */
  result = msgr_receive( &ftm_rfgsm_msgr_client,
                         (uint8 *)&rfa_rf_gsm_get_sawless_lin_mode_rsp,
                         (uint32)sizeof(rfa_rf_gsm_ftm_get_sawless_lin_mode_rsp_s),
                         &bytes_recvd);

  /* Release FTM gsm mutex */
  RFA_FTM_GSM_ISR_UNLOCK(&ftm_gsm_lock);



  /* Sanity check and print debug for msgr_receive */
  ftm_gsm_rf_msgr_receive_debug( RFA_RF_GSM_FTM_GET_SAWLESS_LIN_MODE_RSP,
                                 result,
                                 rfa_rf_gsm_get_sawless_lin_mode_rsp.hdr.id,
                                 rfa_rf_gsm_get_sawless_lin_mode_rsp.cmd_result );

  if (result != E_SUCCESS)
  {

    MSG( MSG_SSID_FTM, MSG_LEGACY_HIGH, "ftm_gsm_get_sawless_lin_mode, Response is not successful " );
    return FALSE;
  }

  /* Cmd will be updated with result and returned as part of rsp_pkt */
  lin_params->use_high_lin = rfa_rf_gsm_get_sawless_lin_mode_rsp.is_in_high_lin;

  MSG_3( MSG_SSID_FTM, MSG_LEGACY_HIGH, "ftm_gsm_get_sawless_lin_mode, band:%d, arfcn:%d, hl:%d",
         rfa_rf_gsm_get_sawless_lin_mode_rsp.band,
         rfa_rf_gsm_get_sawless_lin_mode_rsp.arfcn,
         rfa_rf_gsm_get_sawless_lin_mode_rsp.is_in_high_lin );

  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief - ftm_gsm_set_diversity

   @details - add capability to disable/enable diversity from tool command dispatch

   @param
   rfm_device_enum_type rfm_dev - logical device to which this change should be
   applied
   boolean on_off - turn on/off diversity

   @retval
   void
*/

boolean ftm_gsm_set_diversity(rfm_device_enum_type rfm_dev, word on_off )
{
  ftm_gsm_rfctl_vars_s * gvars = ftm_gsm_get_vars_ptr(rfm_dev);

  gvars->diversity_on = on_off>0? TRUE:FALSE;


  MSG_2( MSG_SSID_FTM, MSG_LEGACY_HIGH, "diversity is turned to %d on dev %d",
         gvars->diversity_on, rfm_dev );

  return TRUE;

}

rfcom_gsm_band_type ftm_gsm_convert_ftm_mode_id_type_to_rfcom(ftm_mode_id_type mode)
{
  rfcom_gsm_band_type rfcom_band = RFCOM_BAND_GSM850;
	switch(mode)
	{
	case FTM_PHONE_MODE_GSM_850:
		rfcom_band = RFCOM_BAND_GSM850;
		break;
	case FTM_PHONE_MODE_GSM_900:
		rfcom_band = RFCOM_BAND_GSM900;
		break;
	case FTM_PHONE_MODE_GSM_1800:
		rfcom_band = RFCOM_BAND_GSM1800;
		break;
	case FTM_PHONE_MODE_GSM_1900:
		rfcom_band = RFCOM_BAND_GSM1900;
		break;

	default:
		MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "invalid GSM FTM mode id %d to convert to rfcom gsm band, default to 850");
		rfcom_band = RFCOM_BAND_GSM850;
		break;
	}
	return rfcom_band;

}

/*===========================================================================
FUNCTION ftm_gsm_set_pa_bias_nv

DESCRIPTION
  Used to set smps pdm nv items from 25084 to 25091, and pa icq bias nv
  items from 26941 to 26950. Also, update pdm and icq values stored in
  rfgsm_core_handler.

DEPENDENCIES
   None.

RETURN VALUE
   ftm_gsm_error_code_type

SIDE EFFECTS
   None.

===========================================================================*/
ftm_gsm_error_code_type ftm_gsm_set_pa_bias_nv(rfm_device_enum_type rfm_dev,
	                                                  ftm_mode_id_type band,
	                                                  uint16* smps_online,
	                                                  uint8 num_pa_state,
	                                                  ftm_gsm_pa_bias_params_type* pa_bias_paras)
{
  rfnv_item_id_enum_type rfnv_item_smps;
  rfnv_item_id_enum_type rfnv_item_icq;
  uint8 pa_state_index = 0;
  ftm_gsm_smps_pdm_nv_data_type rfnv_item_to_set_pdm = {{0},{0},{0}};
  ftm_gsm_icq_bias_nv_data_type rfnv_item_to_set_icq = { {{{0}}} };
  rfcom_gsm_band_type rfcom_band;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  uint8 gain_state;

  /* get rfgsm_core_handle_ptr to update NV values loaded during booting up */
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  if ( rfgsm_core_handle_ptr == NULL )
  {
	MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is null for device %d ", rfm_dev);
	return FTM_GSM_GENERAL_FAILURE;
  }
  if ( rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_tx_nv_tbl is NULL!. Check RF Tx cal data" );
	return FTM_GSM_GENERAL_FAILURE;
  }

  if ( rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_tx_nv_tbl->tx_static_nv_data_ptr is NULL!. Check RF Tx cal data" );
	return FTM_GSM_GENERAL_FAILURE;
  }

  if (num_pa_state >= MAX_GSM_PA_GAIN_STATE)
  {
    MSG_2( MSG_SSID_FTM, MSG_LEGACY_ERROR, "num_pa_state (%d) >= MAX_GSM_PA_GAIN_STATE (%d)",
           num_pa_state, MAX_GSM_PA_GAIN_STATE );
  	return FTM_GSM_GENERAL_FAILURE;
  }

  /* convert band type from ftm_mode_id_type to rfcom_gsm_band_type */
  switch (band) {
  case FTM_PHONE_MODE_GSM_850:
      rfcom_band = RFCOM_BAND_GSM850;
	  break;
  case FTM_PHONE_MODE_GSM_900:
	  rfcom_band = RFCOM_BAND_GSM900;
	  break;
  case FTM_PHONE_MODE_GSM_1800:
	  rfcom_band = RFCOM_BAND_GSM1800;
	  break;
  case FTM_PHONE_MODE_GSM_1900:
	  rfcom_band = RFCOM_BAND_GSM1900;
	  break;
  default:
	  rfcom_band = RFCOM_BAND_GSM850;
	  break;
  }

  /* select pdm and icq nv items for later population based on bands*/
  switch(rfm_dev)
  {
  case RFM_DEVICE_0:
    rfnv_item_smps = RFNV_GSM_C0_GSM850_SMPS_PDM_TBL_I + rfcom_band;
	rfnv_item_icq = RFNV_GSM_C0_GSM850_PA_ICQ_BIAS + rfcom_band;
  break;

  case RFM_DEVICE_2:
    rfnv_item_smps = RFNV_GSM_C2_GSM850_SMPS_PDM_TBL_I + rfcom_band;
	rfnv_item_icq = RFNV_GSM_C2_GSM850_PA_ICQ_BIAS + rfcom_band;
  break;

  default:
    rfnv_item_smps = RFNV_GSM_C0_GSM850_SMPS_PDM_TBL_I + rfcom_band;
    rfnv_item_icq = RFNV_GSM_C0_GSM850_PA_ICQ_BIAS + rfcom_band;
  break;
  }



  /* Reorganize pa bias parameters to fit NV item structure.
     Also update PDM and ICQ values stored in the core handler*/

  /*loop through each pa_gain_state*/
  for(pa_state_index = 0; pa_state_index < num_pa_state; pa_state_index++)
  {
    gain_state = pa_bias_paras[pa_state_index].gain_state;

    if( gain_state >= MAX_GSM_PA_GAIN_STATE )
    {
      MSG_2( MSG_SSID_FTM, MSG_LEGACY_ERROR, "Invalid gain_state (%d) @ index %d",
             gain_state, pa_state_index );
    }
    else
    {
    /* copy cal pdm values to the nv item struct (rfnv_item_to_set_pdm). Also update cal pdm value in the handler*/
      rfnv_item_to_set_pdm.cal_smps_pdm_tbl[gain_state] = pa_bias_paras[pa_state_index].cal_pdm;

    rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->smps_pdm_tbl.cal_smps_pdm_tbl[pa_state_index] = pa_bias_paras[pa_state_index].cal_pdm;

    /* copy pa icq bias values to the nv item struct (rfnv_item_to_set_icq). Also update pa icq bias values in the handler*/
      memscpy ( &(rfnv_item_to_set_icq.icq_bias[gain_state]),
                sizeof(uint32)*MAX_GSM_MOD_TYPES*MAX_GSM_RGI_INDEX,
                &(pa_bias_paras[pa_state_index].icq_val[0]),
                sizeof(uint32)*MAX_GSM_MOD_TYPES*MAX_GSM_RGI_INDEX);

      memscpy ( &(rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->pa_icq_bias_data.pa_icq_bias[gain_state]),
                sizeof(uint32)*MAX_GSM_MOD_TYPES*MAX_GSM_RGI_INDEX,
                &(pa_bias_paras[pa_state_index].icq_val[0]),
                sizeof(uint32)*MAX_GSM_MOD_TYPES*MAX_GSM_RGI_INDEX);
    }
  }

  /* copy online pdm values to the nv item struct for GMSK and 8PSK. Also update in the handler*/
  memscpy( &(rfnv_item_to_set_pdm.gsm_smps_pdm_tbl[0]),
           sizeof(uint16)*MAX_GSM_PCL,
           &(smps_online[RF_MOD_GMSK*MAX_GSM_PCL]),
           sizeof(uint16)*MAX_GSM_PCL);

  memscpy( &(rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->smps_pdm_tbl.gsm_smps_pdm_tbl[0]),
           sizeof(uint16)*MAX_GSM_PCL,
           &(smps_online[RF_MOD_GMSK*MAX_GSM_PCL]),
           sizeof(uint16)*MAX_GSM_PCL);

  memscpy( &(rfnv_item_to_set_pdm.edge_smps_pdm_tbl[0]),
           sizeof(uint16)*MAX_GSM_PCL,
           &(smps_online[RF_MOD_8PSK*MAX_GSM_PCL]),
           sizeof(uint16)*MAX_GSM_PCL);

  memscpy( &(rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->smps_pdm_tbl.edge_smps_pdm_tbl[0]),
           sizeof(uint16)*MAX_GSM_PCL,
           &(smps_online[RF_MOD_8PSK*MAX_GSM_PCL]) ,
           sizeof(uint16)*MAX_GSM_PCL);

  /* Write pdm value to NV */
  rfcommon_rfnv_set_item(rfnv_item_smps,
                        (rfnv_item_type *)(&rfnv_item_to_set_pdm),
                        sizeof(ftm_gsm_smps_pdm_nv_data_type),
                        &ftm_tcb,
                        FTM_NV_CMD_SIG,
                        ftm_sig_rex_wait_cb);

  /* Write icq value to NV */
  rfcommon_rfnv_set_item(rfnv_item_icq,
                        (rfnv_item_type *)(&rfnv_item_to_set_icq),
                        sizeof(ftm_gsm_icq_bias_nv_data_type),
                        &ftm_tcb,
                        FTM_NV_CMD_SIG,
                        ftm_sig_rex_wait_cb);

  return FTM_GSM_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
   @brief - ftm_gsm_set_grfc_status_read_enable

   @details -Enables/Disables the Command to read GRFC Status

   @param
   rfm_device_enum_type rfm_dev - logical device to which this change should be
   applied
   burst_type - Rx/TX Burst for which GRFC Status Read is to be enabled
   enable - TRUE for ENABLE; FALSE for DISABLE

   @retval
   void
*/
void ftm_gsm_set_grfc_status_read_enable(rfm_device_enum_type rfm_dev,
					 rf_burst_type* burst_type, boolean enable)
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"NULL core handle for device= %d !", rfm_dev);
    return;
  }

  if (*burst_type < RFGSM_DEVICE_STATUS_MAX_BURST_TYPE)
  {
    rfgsm_core_handle_ptr->device_config_to_read.params[*burst_type].enable_grfc_read = enable;
  }
  else
  {
     MSG_2( MSG_SSID_FTM, MSG_LEGACY_ERROR, "ftm_gsm_set_grfc_status_read_enable burst type is greater than %d i.e., %d",RFGSM_DEVICE_STATUS_MAX_BURST_TYPE, *burst_type);
     return;
  }

  if (*burst_type < RFGSM_DEVICE_STATUS_MAX_BURST_TYPE)
  {
    if(rfgsm_core_handle_ptr->device_config_to_read.params[*burst_type].enable_grfc_read == TRUE)
    {
      MSG_2( MSG_SSID_FTM, MSG_LEGACY_HIGH, "GRFC Status Read Enabled for Device: %d, Burst type: 0x%X", rfm_dev, *burst_type);
    }
    if(rfgsm_core_handle_ptr->device_config_to_read.params[*burst_type].enable_grfc_read == FALSE)
    {
      MSG_2( MSG_SSID_FTM, MSG_LEGACY_HIGH, "GRFC Status Read Disabled for Device: %d, Burst type: 0x%X", rfm_dev, *burst_type);
    }
  }
}

/*----------------------------------------------------------------------------*/
/*!
   @brief - ftm_gsm_set_device_status_enable

   @details -Enables/Disables the Command to read GSM WTR/Device Status

   @param
   rfm_device_enum_type rfm_dev - logical device to which this change should be
   applied
   burst_type - Rx/TX Burst for which Device Status Read is to be enabled
   enable - TRUE for ENABLE; FALSE for DISABLE

   @retval
   void
*/
void ftm_gsm_set_device_status_enable(rfm_device_enum_type rfm_dev,
				      rf_burst_type* burst_type, boolean enable)
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"NULL core handle for device= %d !", rfm_dev);
    return;
  }

  if (*burst_type < RFGSM_DEVICE_STATUS_MAX_BURST_TYPE)
  {
    rfgsm_core_handle_ptr->device_config_to_read.params[*burst_type].enable = enable;
  }
  else
  {
    MSG_2( MSG_SSID_FTM, MSG_LEGACY_ERROR, "ftm_gsm_set_device_status_enable burst type is greater than %d i.e., %d",RFGSM_DEVICE_STATUS_MAX_BURST_TYPE, *burst_type);
    return;
  }

  if (*burst_type < RFGSM_DEVICE_STATUS_MAX_BURST_TYPE)
  {
    if(enable == TRUE)
    {
      MSG_3( MSG_SSID_FTM, MSG_LEGACY_HIGH, "Device Status Read Enabled for Device: %d, Burst type: 0x%X with %d number of registers", rfm_dev, *burst_type,
                                             rfgsm_core_handle_ptr->device_config_to_read.params[*burst_type].num_regs);
    }
    if(enable == FALSE)
    {
      /*When Device Status Read is Disabled, num_regs is reset to default value = 0 so again user need to add the registers to be read*/
      rfgsm_core_handle_ptr->device_config_to_read.params[*burst_type].num_regs = 0;
      MSG_2( MSG_SSID_FTM, MSG_LEGACY_HIGH, "Device Status Read Disabled for Device: %d, Burst type: 0x%X and reg list is cleared", rfm_dev, *burst_type);
    }
  }
}
/*----------------------------------------------------------------------------*/
/*!
   @brief - ftm_gsm_get_device_status_reg_list

   @details -Command to read GSM WTR/Device Status

   @param
   rfm_device_enum_type rfm_dev - logical device to which this change should be
   applied
   ftm_read_device_status* device_status - Pointer to the parameters of Device Status passed through the Diag Command

   @retval
   void
*/
void ftm_gsm_get_device_status_reg_list( rfm_device_enum_type rfm_dev,
                                         ftm_read_device_status* device_status)
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr;
  uint8 burst_index;
  uint8 reg_index;
  rf_hal_bus_resource_type ftm_resource_type_to_rf_hal_type[FTM_BUS_RESOURCE_NUM] =
  {
    RF_HAL_BUS_RESOURCE_SSBI,
    RF_HAL_BUS_RESOURCE_RFFE,
    RF_HAL_BUS_RESOURCE_GRFC
  };

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"NULL core handle for device= %d !", rfm_dev);
    return;
  }

  burst_index = device_status->burst_type;

  if (burst_index < RFGSM_DEVICE_STATUS_MAX_BURST_TYPE)
  {
    if(rfgsm_core_handle_ptr->device_config_to_read.params[burst_index].num_regs >= RFGSM_DEVICE_STATUS_READ_MAX_REGS)
    {
      MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"We are exceeding number of register to read. Current count %d", rfgsm_core_handle_ptr->device_config_to_read.params[burst_index].num_regs);
      return;
    }
  }
  else
  {
    MSG_2( MSG_SSID_FTM, MSG_LEGACY_ERROR, "ftm_gsm_get_device_status_reg_list burst type is greater than %d i.e., %d",RFGSM_DEVICE_STATUS_MAX_BURST_TYPE, burst_index);
    return;
  }

  // Sanity check the resource type
  if( device_status->resource_type >= FTM_BUS_RESOURCE_NUM )
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"Unknown resource type %d", device_status->resource_type);
    return;
  }

  if (burst_index < RFGSM_DEVICE_STATUS_MAX_BURST_TYPE )
  {
    MSG_6( MSG_SSID_FTM, MSG_LEGACY_HIGH, "ftm_gsm_get_device_status_reg_list,Device: %d, Burst Type: 0x%X, Register Address: 0x%X, slave id: %d, channel: %d, resource type: %d",
           rfm_dev,
           device_status->burst_type,
           device_status->address,
           device_status->slave_id,
           device_status->channel,
           device_status->resource_type);

    reg_index = rfgsm_core_handle_ptr->device_config_to_read.params[burst_index].num_regs;
    rfgsm_core_handle_ptr->device_config_to_read.params[burst_index].address[reg_index] = device_status->address;
    rfgsm_core_handle_ptr->device_config_to_read.params[burst_index].slave_id[reg_index] = device_status->slave_id;
    rfgsm_core_handle_ptr->device_config_to_read.params[burst_index].channel[reg_index] = device_status->channel;
    rfgsm_core_handle_ptr->device_config_to_read.params[burst_index].half_rate[reg_index] = device_status->half_rate;
    rfgsm_core_handle_ptr->device_config_to_read.params[burst_index].rd_delay[reg_index] = device_status->rd_delay;
    rfgsm_core_handle_ptr->device_config_to_read.params[burst_index].extended_cmd[reg_index] = device_status->extended_cmd;
    rfgsm_core_handle_ptr->device_config_to_read.params[burst_index].resource_type[reg_index] = ftm_resource_type_to_rf_hal_type[device_status->resource_type];

    rfgsm_core_handle_ptr->device_config_to_read.params[burst_index].num_regs++;
  }
}
